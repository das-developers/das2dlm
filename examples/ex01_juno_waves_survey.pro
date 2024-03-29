; Plot four section Juno Waves electric Survey data
;
; Note: 
;   If you request newer data that have not been released to the PDS
;   the das2 server will ask for authenticiation.  The following 
;   procedure can be used to save and load your credentials.
;
;   1. Run the following to set and save your access information:
;      
;       das2c_credset(  $
;          'https://jupiter.physics.uiowa.edu/das/server',  $
;          'Juno Magnetospheric Working Group',  $
;          !null, !null, 'A_USER_NAME', 'A_PASSWORD'  $
;       )
;       das2c_credsave()
;
;      subsituting in a real username and password of course.
;
;   2. Each time you re-start IDL, run the following once to load all
;      your stored credentials and associated matching conditions.
;
;      das2c_credload()
;
;   3. If you need to delete your credentials remove the file named
;      '.das2_auth' from your $HOME (POSIX) or %USERPROFILE% (Windows)
;      directory
;
;   Das2DLM only hands out the cerdentials you specify when data
;   requests match the server root and authentication realm you 
;   specified.  That way random servers don't end up getting access
;   to your passwords.
;
;   For more detail, see the das2c_cred* functions in the API reference 
;   @ https://github.com/das-developers/das2dlm/wiki
   
pro ex01_juno_waves_survey, tmin=tmin, tmax=tmax, display=display
	compile_opt idl2

	; Generate a URL for the desired subset, we'll use a low-level
	; function for now, as catalog lookup is not yet implemented.
	sServer = 'https://jupiter.physics.uiowa.edu/das/server'
	sDataSet='Juno/WAV/Survey'

	; Print interface info about this data source
	;print, das2c_srcinfo(sServer, sDataset) ; Not implemented in v0.5

	
	if ~keyword_set(tmin) then begin 
		sBeg = '2017-02-02'
		sRegion = ', Perijove 4'
	endif else begin
		sBeg = tmin;
		sRegion = ''
	endelse

	if ~keyword_set(tmax) then sEnd = '2017-02-03' else sEnd = tmax;
	
	; ask for 60 second time bins ... or see below
	sRes = '60.0'  
	sFmt = '%s?server=dataset&dataset=%s&start_time=%s&end_time=%s&resolution=%s'
	sUrl = string(sServer, sDataset, sBeg, sEnd, sRes, format=sFmt)
	
	; ...alternate version to get intrinsic resolution
	; sFmt = '%s?server=dataset&dataset=%s&start_time=%s&end_time=%s'
	; sUrl = string(sServer, sDataset, sBeg, sEnd, format=sFmt)
	
	; Get a list of datasets
	result = das2c_readhttp(sUrl)
	aDs = das2c_datasets(result)
		
	ct = colortable(72, /reverse)	
	void = label_date(date_format=['%H:%M']) ; Setup the IDL date labeler

	nBuffer = 0
	if keyword_set(display) then nBuffer = 1
	
	for i = 0, aDs.length - 1 do begin
		dataset = aDs[3 - i]
		print, dataset
		
		;print, das2c_dsinfo(dataset)  ; print info about each dataset in the request
		
		nOverPlot = 0
		if i gt 0 then nOverPlot = 1
		
		vX = das2c_vars(das2c_pdims(dataset, 'time'), 'center')
		aX = toJulian(vX)

		vY = das2c_vars(das2c_pdims(dataset, 'frequency'), 'center')
		aY = das2c_data(vY)
		
		vZ = das2c_vars(das2c_pdims(dataset, 'spec_dens'), 'center')
		aZ = das2c_data(vZ)
		aZ = alog10(aZ)
	
		cont = contour( buffer=nBuffer $
			, aZ, aX, aY, /fill, rgb_table=ct $
			, position=[0.11,0.1,0.8,0.9], n_levels=20, overplot=nOverPlot $
			, xstyle=1, ystyle=1, xtickdir=1, ytickdir=1, axis_style=2 $
			, xthick=2, ythick=2, xticklen=0.01, yticklen=0.01 $
			, font_size=10 $
		)
		
	endfor
	
	; add a color bar	
	cb = colorbar(target=cont, $
		orientation=1, position=[0.82, 0.1, 0.85, 0.9], $
		taper=0, /border, textpos=1, font_size=9, tickformat='(F5.1)' $
	)
	
	; fix up the plot a bit
	cont.title = string( sServer, sDataSet, sRegion, $
		format='Juno Waves Electric Survey%s!c!d %s, %s' $
	)
	ax = cont.axes
	ax[1].log = 1
	ax[0].tickunits='Hours'
	ax[0].title = string(sBeg, sEnd, format="SCET %s to %s")
	ax[1].title = (das2c_props(das2c_pdims(dataset, 'frequency'), 'label')).value
	sZlbl = (das2c_props(das2c_pdims(dataset, 'spec_dens'), 'label')).value
	cb.title = string(sZLbl, format="Electric Spectral Density log!b10!n (%s)")
	
	sFile = 'ex01_juno_waves_survey.png'
	cont.save, sFile, width=1024, height=720, resolution=300
	
	nLeft = das2c_free(result)
	
	print, 'Plot ', sFile, ' printed to the current directory'
	
end


