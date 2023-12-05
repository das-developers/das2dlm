; Produce an MAG FM-1 and FM-2 coverage plot


;das2c_loglevel('debug')   ; turn on debug logging (optional)

sServer  = 'https://tracers-dev.physics.uiowa.edu/stream'

aPaths = [ $
	'PreFlight/L0/MAG/FM-1_Coverage', $ 
	'PreFlight/L0/MAG/FM-2_Coverage' $
]

aPlots = tra_l0_coverage(sServer, aPaths, '2023-01-01', '2024-01-01', 86400.0)

sFile = "coverage_plot.png"
aPlots[0].save, sFile, width=1024, height=800, resolution=300

print, "File ", sFile, " written"

