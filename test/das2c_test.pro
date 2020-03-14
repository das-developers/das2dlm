sUrl = 'http://planet.physics.uiowa.edu/das/das2Server?server=dataset' + $
'&dataset=Galileo/PWS/Survey_Electric&start_time=2001-001&end_time=2001-002'

; good tests, not testing failure modes at this time
query = das2c_readhttp(sUrl)

query
das2c_datasets(query)
ds = das2c_datasets(query, 0)
ds
das2c_dsinfo(ds)

pd_time = das2c_pdims(ds, 'time')
pd_time
pd_freq = das2c_pdims(ds, 'frequency')
pd_freq
pd_amp  = das2c_pdims(ds, 'electric')
pd_amp

v_time  = das2c_vars(pd_time, 'center')
v_time
v_freq  = das2c_vars(pd_freq, 'center')
v_freq
v_amp   = das2c_vars(pd_amp,  'center')
v_amp

das2c_props(ds, 'title')

das2c_props(pd_time, 'label')
das2c_props(pd_time)
das2c_props(pd_freq, 'label')
das2c_props(pd_freq)
das2c_props(pd_amp,  'label')
das2c_props(pd_amp)

das2c_free(query)
exit

aTimes  = das2c_data(v_time, {i:0,   j:'*'} )
aFreqs  = das2c_data(v_freq, {i:'*', j:0  } )
aAmp    = das2c_data(v_amp)


; Next interation, for waveforms
exit
v_start  = das2c_vars(pd_time, 'reference')
v_offest = das2c_vars(pd_time, 'offset')
v_time   = das2c_mk_var(pd_time, 'reference', '+', 'offset')

aTimes   = das2c_data(v_time)
