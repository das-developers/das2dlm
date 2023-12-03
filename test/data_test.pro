sUrl = 'https://jupiter.physics.uiowa.edu/das/server?server=dataset' + $
'&dataset=Galileo/PWS/Survey_Electric&start_time=2001-001&end_time=2001-002'

; good tests, not testing failure modes at this time
query = das2c_readhttp(sUrl)

ds = das2c_datasets(query, 0)
pd_time = das2c_pdims(ds, 'time')
pd_freq = das2c_pdims(ds, 'frequency')
pd_amp  = das2c_pdims(ds, 'electric')

v_time  = das2c_vars(pd_time, 'center')
;v_freq  = das2c_vars(pd_freq, 'center')
;v_amp   = das2c_vars(pd_amp,  'center')

;print, 'aTimes  = das2c_data(v_time)'
;aTimes  = das2c_data(v_time)

print, 'aTimes  = das2c_data(v_time, {i:1,j:1} )'
aTimes  = das2c_data(v_time, {i:1,j:1} )
print, size(aTimes)
print, aTimes

print, 'aTimes  = das2c_data(v_time, {i:[0,1], j:[0,1]} )'
aTimes  = das2c_data(v_time, {i:[0,1], j:[0,1]} )
print, size(aTimes)
print, aTimes

print, 'aTimes  = das2c_data(v_time, {i:[0,1], j:''*''} )'
aTimes  = das2c_data(v_time, {i:[0,1], j:'*'} )
print, size(aTimes)
print, aTimes


;aTimes  = das2c_data(v_time)
;aFreqs  = das2c_data(v_freq, {i:'*', j:0  } )  ; TODO
;aFreqs  = das2c_data(v_freq)

;aAmp    = das2c_data(v_amp)

;help, das2c_props(ds, 'title')
;help, das2c_props(pd_time, 'label')
;help, das2c_props(pd_time)
;help, das2c_props(pd_freq, 'label')
;help, das2c_props(pd_freq)
;help, das2c_props(pd_amp,  'label')
;help, das2c_props(pd_amp)

;help, 
;das2c_free(query)
