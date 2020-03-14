# Windows makefile
MAKE=nmake /nologo
CC=cl.exe /nologo

# Where to find stuff #########################################################
#
# Set default locations or use environment overrides.  To override any of 
# the macros below use (for example):
#
#   set I_IDL="C:\Program Files\Exelis\IDL87\bin\bin.x86_64\external\include"
#

# The directory containing idl_export.h
!IFNDEF I_IDL
I_IDL="C:\Program Files\Exelis\IDL84\bin\bin.x86_64\external\include"
!ENDIF

# Directory contaning das2 include directory.  (i.e. the one above the
# actual header files)
!IFNEDF I_DAS2
I_DAS2="$(USERPROFILE)\git\das2C"
!ENDIF

# Directory contaning libdas2.3.lib
!IFNDEF L_DAS2
L_DAS2="$(USERPROFILE)\git\das2C\build"
!ENDIF

# Directory containing vcpkg static libraries
!IFNDEF L_VCPKG
L_VCPGK="$(USERPROFILE)\git\vcpkg\installed\x64-windows-static\lib
!ENDIF

# Directory containing vcpkg header files
!IFNDEF I_VCPKG
L_VCPGK="$(USERPROFILE)\git\vcpkg\installed\x64-windows-static\include
!ENDIF

# Derived defintions ##########################################################

ALL_INC=/I $(I_DAS2) /I $(I_IDL) /I $(I_VCPKG)

STATIC_LIBS=$(L_DAS2)\libdas2.3.lib $(L_VCPKG)\expat.lib $(L_VCPKG)\fftw3.lib \
 $(L_VCPKG)\libssl.lib $(ED)\libcrypto.lib $(ED)\pthreadVC3.lib

ALL_LIBS= $(EX_LIBS) Advapi32.lib User32.lib Crypt32.lib ws2_32.lib


# Sources #####################################################################
# Warning: keep das2c.c first in the list below!  Since we are usin a wierd
#          #include scheme

SRCS=src\das2c.c src\das2c_message.c src\das2c_db.c src\das2c_queries.c \
 src\das2c_datasets.c src\das2c_dsinfo.c src\das2c_pdims.c src\das2c_vars.c \
 src\das2c_data.c src\das2c_readhttp.c
 
BD=build.windows64

# next line is wierd, it's due to all the static functions in DLMs so we just
# include all the .c files in one.
OBJS=$(BD)\das2c.obj

CFLAGS=$(CFLAGS) $(ALL_INC)

DLM_SUFFIX=.x86_64.dll

# Explicit Rules #############################################################

build:$(BD) dlm\das2c.$(DLM_SUFFIX)

$(BD):
	if not exist "$(BD)" mkdir "$(BD)"
	   
$(BD)/das2c.obj:$(SRCS)
	$(CC) $(CFLAGS) /Fo:$@ /c $(SD)/das2c.c /link $(EX_PTHREAD)

dlm\das2c$(DLM_SUFFIX):$(BD)\das2c.obj
	link /nologo /out:$@ $(BD)/das2c.obj $(ALL_LIBS)

clean:
	if exist $(BD) rmdir /S /Q $(BD)
	rm dlm/das2c.$(P_SUFFIX)

