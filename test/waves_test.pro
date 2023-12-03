

pro waves_test

	sUrl = $
		'https://jupiter.physics.uiowa.edu/das/server' + $
	       '?server=dataset&dataset=Juno/WAV/Burst_Waveforms' + $
	       '&start_time=2017-086T02:00&end_time=2017-086T16:00' + $

	 
	query = das2c_readhttp(sUrl)
	 
	ds = das2c_datasets(query, 0)
	help, ds
end


; Test script

s = 'https://jupiter.physics.uiowa.edu/das/server?server=dataset&dataset=Juno/WAV/Survey&&start_time=2017-01-01&end_time=2017-01-02&resolution=60.0'
res = das2c_readhttp(s)
das2c_results(1)  