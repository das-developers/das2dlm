; Produce an MAG FM-1 and FM-2 coverage plot


;das2c_loglevel('debug')   ; turn on debug logging (optional)

.reset_session

sServer  = 'https://tracers-dev.physics.uiowa.edu/stream'

aPaths = [ $
	'PreFlight/L0/ACE/FM-1_Coverage', $ 
	'PreFlight/L0/ACE/FM-2_Coverage' $
]

; Change to buffer=0 to plot in a window 
aPlots = tra_l0_coverage(sServer, aPaths, '2023-08-01', '2023-12-01', 86400.0, buffer=1)

sFile = "ex03_tra-ace_coverage.png"
aPlots[0].save, sFile, width=1024, height=800, resolution=300

print, "File ", sFile, " written"

