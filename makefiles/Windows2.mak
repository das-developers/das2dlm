# Windows makefile
MAKE=nmake /nologo
CC=cl.exe /nologo

# IDL Include Path ################################################################
IDL_DIR="C:\Program Files\Exelis\IDL84\bin\bin.x86_64\external\include\"

# Library locations ###########################################################
# We have to make a "fat shared object" so that the IDL DLM will be self 
# contained.  This means that almost everything has to be statically linked.

# Defaults assume you have das2C and compiled lib. 
L_DAS2="e:\Codes\SPEDAS\das2\das2C\build\libdas2.3.lib"

ED=e:\Codes\SPEDAS\das2\vcpkg\installed\x64-windows-static\lib
EX_LIBS=$(ED)\expat.lib $(ED)\fftw3.lib $(ED)\libssl.lib $(ED)\libcrypto.lib $(L_DAS2)
EX_PTHREAD=$(ED)pthreadVC3.lib
# Advapi32.lib User32.lib Crypt32.lib ws2_32.lib

# Directory containing das2 with contains core.h
I_DAS2="e:\Codes\SPEDAS\das2\das2C\"

# Sources #####################################################################
# Warning: keep das2c.c first in the list below!  Since we are usin a wierd
#          #include scheme
BD=build.windows64
SD=src

SOURCES=$(SD)\das2c.c $(SD)\das2c_message.c $(SD)\das2c_db.c \
	$(SD)\das2c_queries.c $(SD)\das2c_dsinfo.c \
	$(SD)\das2c_datasets.c $(SD)\das2c_physdims.c $(SD)\das2c_readhttp.c

# Composite Defs ##############################################################

###CFLAGS=-g -std=c99 -Wall -fPIC -I$(IDL_DIR)/external/include -I$(I_DAS2)
###STATIC_LIBS=$(L_DAS2) $(L_FFTW3) $(L_EXPAT) $(L_SSL) $(L_CRYPTO) $(L_Z)
###LFLAGS=-Wall -fPIC -shared -Wl,-Bsymbolic $(STATIC_LIBS) $(DYN_LIBS)

CFLAGS=/I $(IDL_DIR) /I $(I_DAS2) /I $(SD)

DLMD=dlm
P_SUFFIX=windows.x86_64.dll

# Explicit Rules #############################################################

.PHONY: build

build:CREATE_DIRS $(DLMD)/das2c.$(P_SUFFIX) $(DLMD)/das2c.dlm

CREATE_DIRS:
	if not exist "$(BD)" mkdir "$(BD)"
	if not exist "$(DLMD)" mkdir "$(DLMD)"
	   
$(BD)/das2c.obj:$(SOURCES)
	$(CC) $(CFLAGS) /Fo:$@ /c $(SD)/das2c.c /link $(EX_PTHREAD)

$(DLMD)/das2c.$(P_SUFFIX):$(BD)/das2c.obj
	link /nologo /out:$@ $(BD)/das2c.obj $(EX_LIBS)

clean:
	rm -r $(BD) 
	rm $(DLMD)/das2c.$(P_SUFFIX)

