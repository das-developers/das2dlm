; TRACERS L0 Coverage Plotter ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; ;


;+
; FUNCTION:
;   tra_l0_coverage
; 
; PURPOSE:
;   Produce an N-panel plot of packets per bin by APID
;
; CALLING SEQUENCE:
;   plot_array = l0_coverage_plots(sServiceRoot, aPaths, sBegin, sEnd, rBinSz)
;
; INPUTS:
;   sServiceRoot: A string providing the root URL of a das2 server providing x-y data
;
;   aPaths:       An array of strings providing data sources paths on the server
;
;   sBegin:       The inclusive start date-time in ISO-8601 format as a string
;
;   sEnd:         The exclusive upper bound data-time in ISO-8601 format as a string
;
;   rBinSz:       The size of each packet count bin in seconds, 60.0 is recommended
;   
; OUTPUT:
;   An array of BARPLOT structures which can be used to write to the plot to a file
;
; EXAMPLE:
;   sServer = 'https://tracers-dev.physics.uiowa.edu/stream'
;   aPaths  = ['PreFlight/L0/MAG/FM-1_Coverage', 'PreFlight/L0/MAG/FM-2_Coverage' ]
;   aPlots = tra_l0_coverage(sServer, aPaths, '2023-01-01', '2024-01-01', 60.0)
;   
; MODIFICATION HISTORY:
;   C. Piker, 2023-12-04 - Initial
;
function tra_l0_coverage, sServer, aPaths, sBeg, sEnd, rBinSec, buffer=buffer

	if n_elements(aPaths) eq 0 then message, 'No data source paths provided'

	nPanels = n_elements(aPaths)
	aResult = replicate({das2c_result}, nPanels)

	; Get the coverage data
	sUrlFmt = '%s?server=dataset&dataset=%s&start_time=%s&end_time=%s&interval=%d'
	for iPath = 0, n_elements(aPaths) - 1 do begin 
		sUrl = string(sServer, aPaths[iPath], sBeg, sEnd, rBinSec, format=sUrlFmt)
		aResult[iPath] = das2c_readhttp(sUrl)
	endfor
	
	; Figure out a good Y label
	if rBinSec eq 86400 then sYLabel = 'Packets/day' else $
	if rBinSec eq 3600 then sYLabel = 'Packets/hour' else $
	if rBinSec eq 60 then sYLabel = 'Packets/minute' else $
	sYLabel = string(rBinSec, format='Packets/%d sec')

	rYSz = 0.9 / nPanels
	aPanels = make_array(nPanels, /OBJ)
	
	void = label_date(date_format=['%Y-%N-%D']) ; Setup date labeler

	; Get time extents for all panels
	rMax = -1.0d+31
	rMin = 1.0e+31
	aApidsInPanel = make_array(nPanels, /integer, value=0)
	for iPanel = 0, nPanels - 1 do begin
		if aResult[iPanel].n_dsets lt 1 then begin
			sMsg = string(aResult[iPanel].source, format="Empty result for %s")
			message, sMsg, /informational
			continue
		endif

		ds = das2c_datasets(aResult[iPanel], 0)
		
		vTime = das2c_vars(das2c_pdims(ds, 'time'), 'center')
		aJulian = toJulian(vTime)
		rPanelMin = min(aJulian)
		rPanelMax = max(aJulian)
		if rMin gt rPanelMin then rMin = rPanelMin
		if rMax lt rPanelMax then rMax = rPanelMax
		
		foreach group, das2c_pdims(ds) do begin
			if (group.pdim ne 'time') and (group.pdim ne 'total') then begin
				aApidsInPanel[iPanel] = aApidsInPanel[iPanel] + 1
			endif
		endforeach
	endfor
	; Expand range to pickup half-widths in days
	rMin = rMin - 0.5*(rBinSec/86400)
	rMax = rMax + 0.5*(rBinSec/86400)


	; Make a barplot for each result, we're going to skip the totals
	yYSz = 0.9 / nPanels
	aColors = ['red','orange','green','cyan', 'blue','purple']

	for iPanel = 0, nPanels - 1 do begin

		y0 = 0.96 - (rYSz * (iPanel + 1))
		y1 = 0.96 - (rYSz * iPanel  + rYSz*0.2)
		x0 = 0.15
		x1 = 0.83

		ds = das2c_datasets(aResult[iPanel], 0)

		; Get the center-point values from the 'time' group
		vTime = das2c_vars(das2c_pdims(ds, 'time'), 'center')
		aJulian = toJulian(vTime)

		; Place to store graphics we generate
		aGraphic = make_array(aApidsInPanel[iPanel], /OBJ)

		; bar-plot rates by APID
		iApid = 0
		foreach group, das2c_pdims(ds) do begin
			if (group.pdim eq 'time') or (group.pdim eq 'total') then continue

			aRate = das2c_data(das2c_vars(group, 'center'))
			
			sName = strmid((das2c_props(group, 'label')).value, 1, 4)
			if sName eq !null then sName = group.pdim

			if iApid eq 0 then begin
				aGraphic[iApid] = barplot( $
					/current,  buffer=buffer, position=[x0, y0, x1, y1], $
					aJulian, aRate, width=rBinSec, fill_color=aColors[iApid], $
					title=aPaths[iPanel], xtickunits='time', xtickformat='label_date', $
					xtickdir=0, ytickdir=0, xrange=[rMin, rMax], bottom_color='white', $
					ytit=sYLabel, xshowtext=0, name=sName, xticklen=0.03, yticklen=0.02 $
				)
				 
			endif else begin
				aGraphic[iApid] = barplot( $
					aJulian, aLastRate + aRate, width=rBinSec, $
					fill_color=aColors[iApid], position=[x0, y0, x1, y1], $
					bottom_values=aLastRate, bottom_color='white', name=sName, $
					/overplot $
				)
			endelse
			iApid = iApid + 1
			aLastRate = aRate

		endforeach
		
		; make a legend for this panel
		leg = legend(target=aGraphic, position=[x1+.02, y1], /auto_text_color)
		leg.horizontal_alignment = 0 ; left align
		leg.transparency = 100
		leg.font_size = 8
		leg.font_style = 2 ; italic
		
		aPanels[iPanel] = aGraphic[0]
	endfor
	
	; turn on the time lables for the bottom plot only
	(aPanels[nPanels - 1])['axis0'].showtext = 1

	; Free the data, we're done with it
	for iPanel = 0, nPanels - 1 do begin
		nLeft = das2c_free(aResult[iPanel])
	endfor

	return, aPanels

end
