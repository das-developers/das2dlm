; Annotated example of generating a coverage plot generation for TRACERS MAG
; data using das2dlm.


sServer  = 'https://tracers-dev.physics.uiowa.edu/stream?server=dataset'
sSource1 = 'PreFlight/L0/MAG/FM-1_Coverage'
sSource2 = 'PreFlight/L0/MAG/FM-2_Coverage'
sBeg     = '2023-01-01'
sEnd     = '2024-01-01'
sInter   = '60.0'   ; We want to count packets per minute

sFormat  = "%s&dataset=%s&start_time=%s&end_time=%s&interval=%s"

; Coverage for FM-1
sQuery1  = string(sServer, sSource1, sBeg, sEnd, sInter, format=sFormat)

das2c_loglevel('debug')   ; turn on debug logging (optional)

print, "Getting 2023 coverage data for MAG FM-1"
result1  = das2c_readhttp(sQuery1)

print, "Getting 2023 coverage data for MAG FM-2"
result2  = das2c_readhttp(sQuery2)

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
barplot(
)

