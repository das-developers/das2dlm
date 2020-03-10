sUrl = 'http://planet.physics.uiowa.edu/das/das2Server?server=dataset' + $
'&dataset=Galileo/PWS/Survey_Electric&start_time=2001-001&end_time=2001-002'


; Bad call, test error handling
;das2c_readhttp("ardvark:/ /d oggy\t. cat ?\n\n\n = == ")

; now a good call
nId = das2c_readhttp(sUrl)

;das2c_queries(17)           ; Bad calls, test error handling
;das2c_queries("doggy");

das2c_queries()             ; Good call, test both versions
das2c_queries(nId) 

;das2c_dsinfo()              ; Bad calls, test error handling
;das2c_dsinfo(nId)  
;das2c_dsinfo(13, 12) 

das2c_dsinfo(nId, 0)        ; Good call

;das2c_datasets()            ; Bad calls, test error handling
;das2c_datasets('kitty', 78)
;das2c_datasets(0, 'wattie')

das2c_datasets(nId)         ; Good calls, test both versions
das2c_datasets(nId, 0)

das2c_physdims(nId, 0)      ; Good calls, test both versions
das2c_physdims(nId, 0, 1)

das2c_vars(nId, 0, 'time')
das2c_vars(nId, 0, 'time', 'center')


exit
