sUrl = 'http://planet.physics.uiowa.edu/das/das2Server?server=dataset' + $
'&dataset=Galileo/PWS/Survey_Electric&start_time=2001-001&end_time=2001-002'


; Bad call, test error handling
;das2c_readhttp("ardvark:/ /d oggy\t. cat ?\n\n\n = == ")

; now a good call
print, 'nId = das2c_readhttp(sUrl)'
nId = das2c_readhttp(sUrl)

;das2c_queries(17)           ; Bad calls, test error handling
;das2c_queries("doggy");

print, 'das2c_queries()'    ; Good call, test both versions'
das2c_queries()         

print, 'das2c_queries(nId) '
das2c_queries(nId) 

;das2c_dsinfo()              ; Bad calls, test error handling
;das2c_dsinfo(nId)  
;das2c_dsinfo(13, 12) 

print, 'das2c_dsinfo(nId, 0)' ; Good call
das2c_dsinfo(nId, 0)      

;das2c_datasets()            ; Bad calls, test error handling
;das2c_datasets('kitty', 78)
;das2c_datasets(0, 'wattie')

print, 'das2c_datasets(nId)'  ; Good calls, test both versions
das2c_datasets(nId)


print, 'das2c_datasets(nId, 0)'
t = das2c_datasets(nId, 0)
t.shape[8-t.rank:-1]          ; Trim shape

print, 'das2c_physdims(nId, 0)'  ; Good calls, test both versions
das2c_physdims(nId, 0)      

print, 'das2c_physdims(nId, 0, ''electric'')'
das2c_physdims(nId, 0, 'electric')

print, 'das2c_vars(nId, 0, ''time'')'
das2c_vars(nId, 0, 'time')

print, 'das2c_vars(nId, 0, ''time'', ''center'')'
das2c_vars(nId, 0, 'time', 'center')
das2c_vars(nId, 0, 'frequency', 'center')
das2c_vars(nId, 0, 'electric', 'center')

print, 'das2c_dsprops(nId, 0)'
das2c_dsprops(nId, 0)

print, 'das2c_dimprops(nId, 0, ''time'')'
das2c_dimprops(nId, 0, 'time')

print, 'das2c_dimprops(nId, 0, ''frequency'')'
das2c_dimprops(nId, 0, 'frequency')

print, 'das2c_dimprops(nId, 0, ''electric'')'
das2c_dimprops(nId, 0, 'electric')

; Get the unique set of time values
vTime = das2c_vars(nId, 0, 'time','center')

vFreq = das2c_vars(nId, 0, 'frequency','center')

exit

das2c_data(nId, 0, 'time', 'center')
das2c_array(nId, 0, 'time',     'center',  0, '*')

; Get a ds sized array.
das2c_array(nId, 0, 'frequency', 'center')

; Get a partial array.
das2c_array(nId, 0, 'electric', 'center', 

das2c_array(nId, 0, 'electric', 'center', '*', 234)

; Get all the frequency values for the 0th time
das2c_array(nId, 0, 'frequency', 'center', '*', 0)

das2c_array(nId, 0, 'time', 'center', )


