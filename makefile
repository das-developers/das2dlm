# IDL Location ################################################################

IDL_DIR=/usr/local/harris/idl87

# das2C location ##############################################################

# Defaults assume you have das2C in $HOME/git/das2C, and that you 
# built it with "export N_ARCH=/".  Update as needed.
L_DAS2=$(HOME)/git/das2C/build./libdas2.3.a

# Directory containing das2 with contains core.h
I_DAS2=$(HOME)/git/das2C


# Library locations ###########################################################
# We have to make a "fat shared object" so that the IDL DLM will be self 
# contained.  This means that almost everything has to be staticlly linked.

SYSLIB_DIR=/usr/lib64

# yum install fftw-devel fftw-static    (Fixme: Add Debian hints)
L_FFTW3=$(SYSLIB_DIR)/libfftw3.a

# yum install expat-devel expat-static  (Fixme: Add Debian hints)
L_EXPAT=$(SYSLIB_DIR)/libexpat.a

# yum install openssl-devel openssl-static
L_SSL=$(SYSLIB_DIR)/libssl.a
L_CRYPTO=$(SYSLIB_DIR)/libcrypto.a

# yum install zlib-devel zlib-static
# LibZ static seems to be breaking, find the problem 
# L_Z=$(SYSLIB_DIR)/libz.a

# Going to use dynamic pthread on Linux, on Windows it's a static item as well
DYN_LIBS=-lz -lm -lpthread

P_SUFFIX=linux.x86_64.so

# Composite Defs ##############################################################

CC=gcc
CFLAGS=-g -std=c99 -Wall -fPIC -I$(IDL_DIR)/external/include -I$(I_DAS2)


STATIC_LIBS=$(L_DAS2) $(L_FFTW3) $(L_EXPAT) $(L_SSL) $(L_CRYPTO) $(L_Z)
LFLAGS=-Wall -fPIC -shared -Wl,-Bsymbolic $(STATIC_LIBS) $(DYN_LIBS)

BD:=build.linux64
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
	   
$(BD)/das2c.o:src/das2c.c $(L_DAS2)
	$(CC) $(CFLAGS) -c $< -o $@

$(DLMD)/das2c.$(P_SUFFIX):$(BD)/das2c.o 
	$(CC) -o $@ $^ $(LFLAGS)
	
$(DLMD)/das2c.dlm:src/das2c.dlm
	cp $< $@

clean:
	rm -r $(BD) 
	rm $(DLMD)/das2c.$(P_SUFFIX) $(DLMD)/das2c.dlm

