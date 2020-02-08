##############################################################################
# Generic definitions for: Native Programs + Hosted IDL
#
ifeq ($(PREFIX),)
ifeq ($(HOME),)
PREFIX:=$(USERPROFILE)
else
PREFIX=$(HOME)
endif
endif

ifeq ($(N_ARCH),)
N_ARCH=$(shell uname -s).$(shell uname -p)
endif

ifeq ($(DAS2C_INCDIR),)
DAS2C_INCDIR=$(PREFIX)/include
endif

ifeq ($(DAS2C_LIBDIR),)
DAS2C_LIBDIR=$(PREFIX)/lib/$(N_ARCH)
endif

ifeq ($(INST_SHARE),)
INST_SHARE=$(PREFIX)/share
endif

ifeq ($(IDL_DIR),)
$(error IDL_DIR is not defined, a typical value is /usr/local/idl)
endif

IDL_RELEASE=8.7

BD:=build.$(N_ARCH)

# Platform defs ##############################################################

CC=gcc -std=c99 -Wall
CFLAGS=-fPIC -I$(IDL_DIR)/external/include -I$(DAS2C_INCDIR)

# We have to make a "fat shared object" so that the IDL DLM will be self 
# contained.  This means that almost everything has to be staticlly linked.

LFLAGS=-shared -Wl,-Bsymbolic -L$(DAS2C_LIBDIR)  -lfftw3 -lexpat -lssl -lcrypto -lz -lm -lpthread

# Pattern Rules ###############################################################

$(BD)/%.o:src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Explicit Rules #############################################################

bulid:$(BD) $(BD)/das2c.so

$(BD):
	@if [ ! -e "$(BD)" ]; then echo mkdir $(BD); \
        mkdir $(BD); chmod g+w $(BD); fi
	   

$(BD)/das2c.so:$(BD)/das2c.o $(DAS2C_LIBDIR)/libdas2.3.a
	$(CC) $(LFLAGS) -o $@ $^

clean: 
	rm -r $(BD)
