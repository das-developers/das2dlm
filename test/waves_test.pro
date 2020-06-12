

pro waves_test

	sUrl = 'http://jupiter.physics.uiowa.edu/das/server' + $
	       '?server=dataset' + $
	       '&dataset=Juno/WAV/Burst_Waveforms' + $
	       '&start_time=2017-086T02:00' + $ 
	       '&end_time=2017-086T16:00'
	 
	query = das2c_readhttp(sUrl)
	 
	ds = das2c_datasets(query, 0)
	help, ds
end
