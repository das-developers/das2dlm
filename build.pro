; Build the project
; 
; WARNING: This is as single statement mode file!
;
; Run as: @build

; Get a new session
.RESET_SESSION

; Make C program libaries
.COMPILE make

tInfo = routine_info('make', /source)
sProjDir = file_dirname(tInfo.path)
sDirSep=path_sep()

print, 'Project directory is: ', sProjDir

cd, sProjDir

make

; Add in the dlm
sDLM = string(sProjDir, sDirSep, sDirSep, format='%s%sdlm%sdas2c.dlm')
print, 'dlm_load ', sDLM
dlm_load, sDLM

;Compile project files
.COMPILE examples/tra_l0_coverage

