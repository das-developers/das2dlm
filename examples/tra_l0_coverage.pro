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
;   aPlots = l0_coverage_plots(sServer, aPaths, '2023-01-01', '2024-01-01', 60.0)
;   
; MODIFICATION HISTORY:
;   C. Piker, 2023-12-04 - Initial
;
function tra_l0_coverage, sServer, aPaths, sBeg, sEnd, rBinSec

	if n_elements(aPaths) eq 0 then message, 'No data source paths provided'

	nPanels = n_elements(aPaths)
	aResult = replicate({das2c_result}, nPanels)

	; Get the coverage data
	sUrlFmt = '%s?server=dataset&dataset=%s&start_time=%s&end_time=%s&interval=%d'
	for i = 0, n_elements(aPaths) - 1 do begin 
		sUrl = string(sServer, aPaths[i], sBeg, sEnd, rBinSec, format=sUrlFmt)
		aResult[i] = das2c_readhttp(sUrl)
	endfor

	rYSz = 0.9 / nPanels
	aPanels = make_array(nPanels, /OBJ)

	print, "paths", n_elements(aPaths), "results", n_elements(aResult)

	; Make a barplot for each result, we're going to skip the totals
	yYSz = 0.9 / nPanels
	aColors = ['red','orange','green','cyan', 'blue','purple']

	for i = 0, nPanels - 1 do begin

		y0 = 0.96 - (rYSz * (i + 1))
		y1 = 0.96 - (rYSz * i  + rYSz*0.2)

		if aResult[i].n_dsets lt 1 then begin
			sMsg = string(aResult[i].source, format="Empty result for %s")
			message, sMsg, /informational
			continue
		endif

		ds = das2c_datasets(aResult[i], 0)

		; Get the center-point values from the 'time' group
		aTime = das2c_data(das2c_vars(das2c_pdims(ds, 'time'), 'center'))

		; bar-plot rates by APID
		j = 0
		foreach group, das2c_pdims(ds) do begin
			if (group.pdim eq 'time') or (group.pdim ne 'total') then continue

			aRate = das2c_data(das2c_vars(group, 'center'))

			if j eq 0 then begin
				b0 = barplot( /current, /buffer, position=[0.11, y0, 0.91, y1], $
					aTime, aRate, width=rBinSec, fill_color=aColors[j], $
					title=aPaths[i], xtickdir=1, ytickdir=1 $
				)
			endif else begin
				b1 = barplot( $
					aTime, aRate, width=rBinSec, fill_color=aColors[j], $
					position=[0.11, y0, 0.91, y1], $
					bottom_values=aLastRate, xtickdir=1, ytickdir=1, /overplot $
				)
			endelse
			j = j + 1
			aLastRate = aRate

		endforeach

		aPanels[i] = b0
	endfor

	;foreach result, aResults do begin
	;	das2c_free(result)
	;endforeach

	return, aPanels

end
