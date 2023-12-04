; Annotated example of generating a coverage plot generation for TRACERS MAG
; data using das2dlm.

;+
; 
function getData, sServer, sSource, sBeg, sEnd, inter=rInter, res=rRes, params=aParams
	compile_opt idl2

	sFmt = "%s?server=datest&dataset=%s&start_time=%s"
	sUrl = string(sServer, sSource, sBeg, sEnd, format=sFmt)

	; Interval and resolution are mutually exclusive.  Interval is for exactly
	; defined time periods, Resolution is an upper bound on the bin size.
	if keyword_set(inter) then begin
		sUrl = string(sUrl, rInter, format="%s&interval=%s")
	else
		if keyword_set(res) then begin
			sUrl = string(sUrl, rRes, format="%s&resolution=%s")
		endif
	endif

	; If an extra parameters were provided, add them in.
	if keyword_set(params) then begin
		sUrl = string(sUrl, format="%s&params=")
		for i = 0, n_elements(aParams) do begin

			if i EQ 0 then begin
				sUrl = string(sUrl, sParam, format="%s&params=%s")
			else
				sUrl = string(sUrl, sParam, format="%s,%s")
			endif

		endfor
	endif
	
	result = das2c_readhttp(sUrl)
	return result
end

das2c_loglevel('debug')   ; turn on debug logging (optional)

sServer  = 'https://tracers-dev.physics.uiowa.edu/stream'

; We could get a list of all data sources on the server like so, but we
; already know what we want, so skip it
;aSources = das2c_srclist(sServer)

sFm1Cover = 'PreFlight/L0/MAG/FM-1_Coverage'
sFm2Cover = 'PreFlight/L0/MAG/FM-2_Coverage'

; Coverage for FM-1
print, "Getting 2023 coverage data for MAG FM-1"
result1 = getData(sServer, sFm1Cover, '2023-01-01', '2024-01-01', inter=60.0)

print, "Getting 2023 coverage data for MAG FM-2"
result2  = getData(sServer, sSource2, sBeg, sEnd, inter=60.0)

aPlots = coveragePlot(result1, result2)

sFile = "ex01_tracers_mag_coverage.png"
aPlots[0].save, sFile, width=1024, height=800, resolution=300

print, 'Plot', sFile, ' written to current directory'

; There are various dataset inspection routines that could be used here
; but since we know what the form of the data are, we can skip them.
; Uncomment the following to get an array of dataset description structures
;
; das2c_datasets(result1)
; das2c_datasets(result2)

ds1 = das2c_datasets(result1, 0) ; get first dataset from each query result
ds2 = das2c_datasets(result2, 0) 

; We could list the variable groups in each dataset here. Uncomment the
; following to do so.  This is skipped because we know what the groups
; are, i.e. 1 time group, 1 total packets group, and 1 for each APID
;
; das2c_dsinfo(ds1)
; das2c_dsinfo(ds2)
; das2c_pdims(ds1)  ; ... or get structures we can loop over instead
; das2c_pdims(ds2)  ;     of a descriptive string for humans

; Pull out center values from the time dimension for each dataset
xTime1 = das2c_data(das2c_vars(das2c_pdims(ds1, 'time'), 'center'))
xTime2 = das2c_data(das2c_vars(das2c_pdims(ds2, 'time'), 'center'))

; Make a two panel plot with Y values from the pre/APID rates
; We're going to stack all
barplot(
	
)


; Helper function
