; Build the project
; 
; WARNING: This is as single statement mode file!
;
; For non-windows run as:
; 
;    .reset_session
;    @build
; 
; Since Windows has no standard file locations, you need to provide the location
; of your vcvars script and the vcpkg location.  Details vary, but an example
; follows:
; 
;   .reset_session
;   vcvars='C:\opt\vs\2019_community\VC\Auxiliary\Build\vcvars64.bat'
;   vcpkg='C:\Users\login\git\vcpkg' 
;   @build


; Make C program libaries
.COMPILE make

tInfo = routine_info('make', /source)
sProjDir = file_dirname(tInfo.path)
sDirSep=path_sep()

print, 'Project directory is: ', sProjDir

cd, sProjDir

if !version.os_family eq 'Windows' then make, vcvars=vcvars, vcpkg=vcpgk else make

; Add in the dlm
sDLM = string(sProjDir, sDirSep, sDirSep, format='%s%sdlm%sdas2c.dlm')
print, 'dlm_load ', sDLM
dlm_load, sDLM

;Compile project files
.COMPILE examples/tra_coverage
.COMPILE examples/tojulian
.COMPILE examples/tra_vector
.COMPILE examples/ex01_juno_waves_survey.pro
