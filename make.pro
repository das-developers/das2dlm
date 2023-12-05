
; Invoke compilers, etc to build the project

pro make

	; Print the current location
	cd, current=sPwd
	print, 'Running in ', sPwd

	; Get the path separator
	sSep = path_sep()
	
	; Maybe use !version here

	; Pick makefile based on the OS
	if sSep eq '/' then begin
		sOS = 'linux64' 
		if file_test('/etc/lsb-release', /regular) then begin
			sMake = 'Debian.mak'
		endif else begin
			if file_test('/etc/redhat-release', /regular) then begin
				sMake = 'RedHat.mak'
			endif else begin
				sMake = 'Darwin.mak'
				sOS = 'darwin'
			endelse
		endelse
	
	endif else begin
		message, 'Windows build not yet implemented'
	endelse

	;Build das2C if needed
	cd, 'deps/das2C'
	spawn, 'make'
	cd, '../../'

	; Build the extension
	sFmt = 'env I_IDL=%s%sexternal%sinclude make -f makefiles%s%s'
	sCmd = string(!DIR, sSep, sSep, sSep, sMake, format=sFmt)
	print, 'Running: ', sCmd
	spawn, sCmd
	
end


