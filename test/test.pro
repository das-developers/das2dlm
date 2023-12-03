; Regression test script for das2dlm.  
;
; Run this from the project root using:
;
;    @test/test
;

s = 'https://jupiter.physics.uiowa.edu/das/server?server=dataset&dataset=Juno/WAV/Survey&&start_time=2017-01-01&end_time=2017-01-02&resolution=60.0'


print, 'Reading Juno Survey Data'
res = das2c_readhttp(s)

help, res

das2c_results(0)  ; should return null
das2c_results()   ; should return everything
das2c_results(1)  ; should return 1

;sParams = 'dataset=PreFlight/L1/MAG/fm-2/BDC_Burst&start_time=2023-11-14T18:00&end_time=2023-11-14T19:00'

das2c_loglevel('debug')
sServer = 'https://tracers-dev.physics.uiowa.edu/stream?server=dataset'
sParams = 'dataset=PreFlight/L0/MAG/FM-1_Coverage&start_time=2023-01-01&end_time=2024-01-01&interval=60.0'
sMagQuery = string(sServer, sParams, format="%s&%s")
res = das2c_readhttp(sMagQuery)

das2c_results()   ; should return 2 items
das2c_results(2)  ; should return the second item
das2c_results(-1) ; should return nothing

ds = das2c_datasets(res, 0) ; get the first dataset in the result
das2c_dsinfo(ds)            ; Print info about the first dataset (optional)

aGroups = das2c_pdims(ds)   ; Get all the variable groups in the dataset


; Get all the coverage data values.  We know that data are in coordinates of time
; but we don't know how many packet ids might be present
xTime = das2c_data(das2c_vars(das2c_pdims(ds, 'time'), 'center'))

; Loop over all the non time elements adding them to a plot
y221 =  das2c_data(das2c_vars(das2c_pdims(ds, 'num_x221'), 'center'))



plot($
	xTime, 
)