sUrl = 'https://planet.physics.uiowa.edu/das/das2Server?server=dataset' + $
'&dataset=Galileo/PWS/Survey_Electric&start_time=2001-001&end_time=2001-002'

; good tests, not testing failure modes at this time
query = das2c_readhttp(sUrl)

;query
;das2c_datasets(query)
ds = das2c_datasets(query, 0)
help, ds
s = das2c_dsinfo(ds)
print, s

pd_time = das2c_pdims(ds, 'time')
;help, pd_time
pd_freq = das2c_pdims(ds, 'frequency')
;help, pd_freq
pd_amp  = das2c_pdims(ds, 'electric')
;help, pd_amp

v_time  = das2c_vars(pd_time, 'center')
;help, v_time
v_freq  = das2c_vars(pd_freq, 'center')
;help, v_freq
v_amp   = das2c_vars(pd_amp,  'center')
;help, v_amp

aTimes  = das2c_data(v_time, {i:0,j:0} ) 

;aTimes  = das2c_data(v_time)
;aFreqs  = das2c_data(v_freq, {i:'*', j:0  } )  ; TODO
;aFreqs  = das2c_data(v_freq)

aAmp    = das2c_data(v_amp)

;help, das2c_props(ds, 'title')
;help, das2c_props(pd_time, 'label')
;help, das2c_props(pd_time)
;help, das2c_props(pd_freq, 'label')
;help, das2c_props(pd_freq)
;help, das2c_props(pd_amp,  'label')
;help, das2c_props(pd_amp)

;help, 
das2c_free(query)
