; Convert to julian dates
function toJulian, vTime
	aTime = das2c_data(vTime)
	
	; This is straightforward for time systems that don't understand leap 
	; seconds.  Just get the ticks in to fraction of a day, then add the
	; julian epoch time.
	case vTime.units of
		'us2000': begin
			rEpoch = greg2jul(1,1,2000,0,0,0)
			aJulian = (aTime / (1e+6 * 86400)) + rEpoch
		end
		't2000': begin
			rEpoch = greg2jul(1,1,2000,0,0,0)
			aJulian = (aTime / 86400) + rEpoch
		end
		't1970': begin
			rEpoch = greg2jul(1,1,1970,0,0,0)
			aJulian = (aTime / 86400) + rEpoch
		end
		else: message, "Unknown time units"
	endcase
	
	return, aJulian
end
