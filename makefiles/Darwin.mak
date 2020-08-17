# IDL Location ################################################################

ifeq ($(I_IDL),)
I_IDL=/Applications/harris/idl/external/include
endif

# das2C location ##############################################################

# Defaults assume you have das2C in $HOME/git/das2C, and that you 
# built it with "export N_ARCH=/".  Update as needed.
ifeq ($(L_DAS2),)
L_DAS2=$(HOME)/git/das2C/build./libdas2.3.a
endif

# Directory containing das2 with contains core.h
ifeq ($(I_DAS2),)
I_DAS2=$(HOME)/git/das2C
endif

# Library locations ###########################################################
# We have to make a "fat shared object" so that the IDL DLM will be self 
# contained.  This means that almost everything has to be staticlly linked.

ifeq ($(BREWLIB_DIR),)
BREWLIB_DIR=/usr/local/lib
endif

# brew install fftw
ifeq ($(L_FFTW3),)
L_FFTW3=$(BREWLIB_DIR)/libfftw3.a
endif

# brew install openssl
ifeq ($(SSL_DIR),)
SSL_DIR=/usr/local/Cellar/openssl\@1.1/1.1.1g
endif

ifeq ($(L_SSL),)
L_SSL=$(SSL_DIR)/lib/libssl.a
endif

ifeq ($(L_CRYPTO),)
L_CRYPTO=$(SSL_DIR)/lib/libcrypto.a
endif


# Going to use system provided /usr/lib or /System/Library versions for the 
# rest
ifeq ($(DYN_LIBS),)
DYN_LIBS=-lexpat -lz -lm -lpthread
endif

P_SUFFIX=darwin.x86_64.so

# Sources #####################################################################

# Warning: keep das2c.c first in the list below!  Since we are usin a wierd
#          #include scheme
SRCS=das2c.c das2c_message.c das2c_db.c das2c_readhttp.c das2c_queries.c \
 das2c_dsinfo.c das2c_datasets.c das2c_pdims.c das2c_vars.c das2c_props.c \
 das2c_data.c das2c_free.c
 
SRCS_IN=$(patsubst %, src/%, $(SRCS))

# Composite Defs ##############################################################

CC=clang
CFLAGS=-g -std=c99 -arch x86_64 -Wall -I$(I_IDL) -I$(I_DAS2)

STATIC_LIBS=$(L_DAS2) $(L_FFTW3) $(L_EXPAT) $(L_SSL) $(L_CRYPTO) $(L_Z)
LFLAGS=-undefined dynamic_lookup -Wl,-no_compact_unwind $(STATIC_LIBS) $(DYN_LIBS)

BD:=build.darwin64
DLMD:=dlm

# Explicit Rules #############################################################

.PHONY: build

build:$(BD) $(DLMD) $(DLMD)/das2c.$(P_SUFFIX) $(DLMD)/das2c.dlm

$(BD):
	@if [ ! -e "$(BD)" ]; then echo mkdir $(BD); \
        mkdir $(BD); chmod g+w $(BD); fi

$(DLMD):
	@if [ ! -e "$(DLMD)" ]; then echo mkdir $(DLMD); \
        mkdir $(DLMD); chmod g+w $(DLMD); fi
	   
$(BD)/das2c.o:$(SRCS_IN) $(L_DAS2) 
	$(CC) $(CFLAGS) -c $< -o $@

$(DLMD)/das2c.$(P_SUFFIX):$(BD)/das2c.o 
	$(CC) -o $@ $^ $(LFLAGS)

clean:
	rm -r $(BD) 
	rm $(DLMD)/das2c.$(P_SUFFIX)

