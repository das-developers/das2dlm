;+
; FUNCTION:
;   tra_vector
; 
; PURPOSE
;  Produce an N-panel plot of waveform data for a single dataset
;
; CALLING SEQUENCE:
;   graphic_array = tra_vector(sServiceRoot, sPath, sBegin, sEnd)
;
; PARAMETERS:
;   sServiceRoot: A string providing the root URL of a das2 server providing x-y
;                    data
;   sPaths:       A strings providing data sources path on the server
;   sBegin:       The inclusive start date-time in ISO-8601 format as a string
;   sEnd:         The exclusive upper bound data-time in ISO-8601 format as a
;                    string
; KEYWORDS:
;   apids:        A comma separated list of APIDs that are desired, see example
;                   below
;   buffer:       If set to 0, plots will display to the current graphic which
;                 defaults to the screen
;
; OUTPUT:
;   An array of PLOT graphics which can be used to write a plot to a file
;
; EXAMPLE:
; Plot MSC quicklook data to the screen, but only get the uncompressed APID
; 
;   sServer = 'https://tracers-dev.physics.uiowa.edu/stream'
;   sPath = 'PreFlight/QL/MSC/FM-1/bac'
;   sApids = 'x242'
;   sBeg = '2023-11-30T22:20'
;   sEnd = '2023-11-30T22:30'
;   oPlot = tra_vector(sServer, sPath, sBeg, sEnd, apids='x242', buffer=0) 
; 
; MODIFICATION HISTORY:
;  C. Piker, 2023-12-07 - Initial Version
;-
function tra_vector, sServer, sPath, sBeg, sEnd, apids=apids, Buffer=buffer
	; Note: This function makes use of the new dasflex API instead of the 
	; legacy das2 server=dsdf interface.  A proper implementation would read
	; the catalog node for the given data source, and thus be protected 
	; agains API changes, but since das2c_node is not yet implemented, we'll
	; just hard-code it for now.

	; The server may offer other formats, but this is the only one das2C v2.3
	; understands
	sOutput='format.type=das&format.version=2'
	sPath = sPath.ToLower()
	if keyword_set(apids) then begin
		sFmt='%s/source/%s/flex?read.time.min=%s&read.time.max=%s&read.apid=%s&%s'
		sUrl = string(sServer, sPath, sBeg, sEnd, apids, sOutput, format=sFmt) 
	endif else begin
		sFmt='%s/source/%s/flex?read.time.min=%s&read.time.max=%s&%s'
		sUrl = string(sServer, sPath, sBeg, sEnd, sOutput, format=sFmt)
	endelse
	
	nBuffer=0
	if n_elements(buffer) eq 1 then nBuffer=buffer
	
	print, 'Query: ' + sUrl
	nBeg = systime(/julian)
	tResult = das2c_readhttp(sUrl)
	nEnd = systime(/julian)
	
	print, string(tResult.n_vals, (nEnd - nBeg)*86400, format="Fetched/parsed %d values in %0.2f seconds") 
	
	tDs = das2c_datasets(tResult, 0) ; assume single dataset in result
	tProp = das2c_props(tDs, 'title')
	sTitle = tProp.value
	 
	; Convert the time values and count the vector components
	nComp = 0
	foreach tPd, das2c_pdims(tDs) do begin
		if tPd.pdim eq 'time' then begin
			aJulian = toJulian( das2c_vars(das2c_pdims(tDs, 'time'), 'center') )
		endif else nComp = nComp + 1
	endforeach 
	
	; Unlike in das3.0, in das2.2 format vectors are not clearly identified.
	aGraphics = make_array(nComp, /OBJ)
	
	nBuffer = 0
	if keyword_set(buffer) then nBuffer = buffer
	
	; Three level time data display
	void = label_date(date_format=['%I:%S', '%H', '%Y-%N-%D'])
	
	; Use mag common axis colors
	aColors = ['red','green','blue','black']
	iComp = 0	
	foreach tPd, das2c_pdims(tDs) do begin
	
		if tPd.pdim eq 'time' then continue
		
		vComp = das2c_vars(tPd, 'center')
			
		tProp = das2c_props(tPd, 'label')
		if tProp eq !null then sComp = tPd.pdim else sComp = tProp.value

		if iComp eq 0 then begin
			tTitle = das2c_props(tDs, 'title')
			sYTitle = vComp.units
			
			aGraphics[iComp] = plot(/current, buffer=nBuffer, $
				aJulian, das2c_data(vComp), ytitle=sYTitle, $
				xtickunits=['time','Hour','Day'], $
				position = [0.1, 0.25, 0.95, 0.90], $
				dimensions=[1280,440], name=sComp, $
				xtickformat=['LABEL_DATE','LABLE_DATE','LABEL_DATE'], $ 
				title=sTitle, color=aColors[iComp], xstyle=1, $
				xticklen=0.03, yticklen=0.012 $
			)
		endif else begin
			aGraphics[iComp] = plot( /overplot, $
				aJulian, das2c_data(vComp), $
				color=aColors[iComp], name=sComp $
			)
		endelse
		
		iComp = iComp + 1
	endforeach
	
	leg = legend(target=aGraphics, transparency=100, /auto_text_color, $
		position=[0.95,0.90] $
	)
	
	; TODO: Add a PSD dataset to the result as long as we are here and plot that
	;       too.  Code to do fast PSD in FFTW with overlap can be added to
	;       das2dlm if there is a desire for it. Would take about a day.
	;psd = das2c_psd(ds, ['Bx','By','Bz'], 128, 4, window='hann')
		
	
	; free the result, we're done with it
	nRemaining = das2c_free(tResult)
	
	return, aGraphics
end

