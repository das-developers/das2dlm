; A test of TRACERS/MAG QL Burst plotting

sServer = 'https://tracers-dev.physics.uiowa.edu/stream'

;get a list of MAG sources on the server (optional)
;a = das2c_srclist(sServer)
;foreach s, a do if s.path.contains('/MAG/') then print, s.path

sPath = 'PreFlight/QL/MAG/FM-1/bdc_burst'
aPlots = tra_vector( $
	sServer, sPath, '2023-11-14T18:16:19', '2023-11-14T18:16:31', apids='x222', $
	buffer=1 $
)

sFile = 'ex04_tra-mag_ql_burst.png'
aPlots[0].save, sFile, width=1280, height=440, resolution=300

print, 'file ', sFile, ' written'

