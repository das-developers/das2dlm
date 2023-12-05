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

print, 'Project directory is: ', sProjDir

cd, sProjDir

make

; Add in the dlm
dlm_register, string(sProjDir, path_sep(), path_sep(), format='%s%sdlm%sdas2c.dlm')
dlm_load, 'das2c'

;Compile project files
.COMPILE examples/tra_l0_coverage

