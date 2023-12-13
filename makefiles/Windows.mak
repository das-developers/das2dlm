# Windows makefile
MAKE=nmake /nologo
CC=cl.exe /nologo /DWIN32

# Where to find stuff #########################################################
#
# Set default locations or use environment overrides.  To override any of 
# the macros below use (for example):
#
#  set IDL_DIR="C:\Program Files\Harris\IDL87"
#  
!IFNDEF VCPKG_DIR
VCPKG_DIR=$(USERPROFILE)\git\vcpkg
!ENDIF

!IFNDEF LIBRARY_INC
LIBRARY_INC=$(VCPKG_DIR)\installed\x64-windows-static\include
!ENDIF

!IFNDEF LIBRARY_LIB
LIBRARY_LIB=$(VCPKG_DIR)\installed\x64-windows-static\lib
!ENDIF

# The directory containing idl_export.h
!IFNDEF IDL_DIR
IDL_DIR=C:\Program Files\Exelis\IDL89
!ENDIF

!IFNDEF I_IDL
I_IDL=$(IDL_DIR)\external\include
!ENDIF

!IFNDEF L_IDL
L_IDL=$(IDL_DIR)\bin\bin.x86_64\idl.lib
!ENDIF

!IFNDEF I_DAS2
I_DAS2=deps\das2C
!ENDIF

# libdas2.3.lib file or equivalent
!IFNDEF L_DAS2
L_DAS2=deps\das2C\build.windows\libdas2.3.lib
!ENDIF


# Derived definitions ##########################################################

INC=/I "$(I_DAS2)" /I "$(I_IDL)" /I "$(LIBRARY_INC)"

ED=$(LIBRARY_LIB)
STATIC_LIBS="$(L_DAS2)" "$(L_IDL)" "$(ED)\libexpatMD.lib" \
 "$(ED)\fftw3.lib" "$(ED)\libssl.lib" "$(ED)\libcrypto.lib" \
 "$(ED)\zlib.lib" "$(ED)\pthreadVC3.lib"

ALL_LIBS= $(STATIC_LIBS) Advapi32.lib User32.lib Crypt32.lib ws2_32.lib


# Sources #####################################################################
# Warning: keep das2c.c first in the list below!  Since we are usin a wierd
#          #include scheme

SRCS=src\das2c.c src\das2c_message.c src\das2c_db.c src\das2c_results.c \
 src\das2c_datasets.c src\das2c_dsinfo.c src\das2c_pdims.c src\das2c_vars.c \
 src\das2c_data.c src\das2c_readhttp.c src\das2c_srclist.c src\das2c_creds.c
 #src\das2c_null_test.c src\das2c_GettmpNull.c
 
BD=build.windows

# next line is weird, it's due to all the static functions in DLMs so we just
# include all the .c files in one.
OBJS=$(BD)\das2c.obj

CFLAGS=$(CFLAGS) $(INC)

DLM_SUFFIX=.x86_64

# Explicit Rules #############################################################

build:$(BD) dlm\das2c$(DLM_SUFFIX).dll

$(BD):
	if not exist "$(BD)" mkdir "$(BD)"
	   
$(BD)\das2c.obj:$(SRCS)
	$(CC) $(CFLAGS) /Fo:$@ /c src\das2c.c

dlm\das2c$(DLM_SUFFIX).dll:$(BD)\das2c.obj
	link /nologo /ltcg /dll $(BD)\das2c.obj $(ALL_LIBS) /def:src\das2c.def /out:dlm\das2c$(DLM_SUFFIX).dll

clean:
	if exist $(BD) rmdir /S /Q $(BD)
	del dlm\das2c$(DLM_SUFFIX).exp dlm\das2c$(DLM_SUFFIX).lib dlm\das2c$(DLM_SUFFIX).dll
