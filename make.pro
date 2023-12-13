
; Invoke compilers, etc to build the project

pro make, vcvars=vcvars, vcpkg=vcpkg

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
		
		;Build das2C if needed
		cd, 'deps/das2C'
		spawn, 'make'
		cd, '../../'
		
		; Build the extension
		sFmt = 'env I_IDL=%s%sexternal%sinclude make -f makefiles%s%s'
		sCmd = string(!DIR, sSep, sSep, sSep, sMake, format=sFmt)
		print, 'Running: ', sCmd
		spawn, sCmd
	
	endif else begin
		if ~keyword_set(vcpkg) then begin
			message, 'Windows builds require the location of the vcpkg repository'+$
				'  Run make again with vcpkg=\path\to\vcpkg'
		endif
		if ~keyword_set(vcvars) then begin
			message, 'Windows builds require the location of the vcvars toolchain'+$
				'  setup script.  Run make again with vcvars=\path\to\vcvars64'
		endif
		
		sMakeCmd = 'nmake /f buildfiles\Windows.mak'
		sTestCmd = 'nmake /f buildfiles\Windows.mak run_test'
		sStatic = 'installed\x64-windows-static'
		sLib = 'set "LIBRARY_LIB='+vcpkg+'\'+sStatic+'\lib"'
		sInc = 'set "LIBRARY_INC='+vcpkg+'\'+sStatic+'\include"'
		sFmtTest = '%s && %s && %s && %s && %s'
		sFmt =     '%s && %s && %s && %s'
		sCmd = string(vcvars, sLib, sInc, sMakeCmd, sTestCmd, format=sFmtTest)
		;sCmd = string(vcvars, sLib, sInc, sMakeCmd, format=sFmt)
		print, sCmd 
		cd, 'deps\das2C'
		spawn, sCmd, /log_output
		cd, '..\..\'
		
		; build the extension
		sMakeCmd = 'nmake /f makefiles\Windows.mak'
		sVcPkg = 'set "VCPKG_DIR='+vcpkg+'"'
		sIdlDir = 'set "IDL_DIR='+!DIR+'"'
		sFmt = '%s && %s && %s && %s'
		sCmd = string(vcvars, sVcPkg, sIdlDir, sMakeCmd, format=sFmt)
		print, 'Running: ', sCmd
		spawn, sCmd, /log_output 
		
	endelse
	
end


