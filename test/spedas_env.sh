# sh/bash source script to setup the IDL_PATH, IDL_DLM_PATH and PS1 environment
# variables.  To use this:
#
# 1. Adjust the SPEDAS_IDL_DIR and CDF_DLM_DIR environment variables below.
#
# 2. $ cd ROOT_DAS2DLM_DIR
#    $ source test/spedas_env.sh
#
# Spedas can be downloaded from here:
#
#  http://spedas.org/downloads/spedas_3_2.zip



# My testing location, your's is probably more like: 
# $HOME/spedas_3_20/idl
SPEDAS_IDL_DIR=/home/${LOGNAME}/sp3.20

# CDF DLM location
CDF_DLM_DIR=/usr/local/naif/icy/lib

# ########################################################################### #
# Various environment varible manipulation functions
idlpathmunge () {
	if ! echo $IDL_PATH | /bin/egrep -q "(^|:)$1($|:)" ; then
		if [ "$IDL_PATH" = "" ] ; then                        
			IDL_PATH="$1:<IDL_DEFAULT>"                        
		else                                                  
			IDL_PATH=$1:$IDL_PATH
		fi                                                    
	fi                                                       
}

dlmpathmunge () {
	if ! echo $IDL_DLM_PATH | /bin/egrep -q "(^|:)$1($|:)" ; then
		if [ "$IDL_DLM_PATH" = "" ] ; then                        
			IDL_DLM_PATH="$1:<IDL_DEFAULT>"                        
		else                                                  
			IDL_DLM_PATH=$1:$IDL_DLM_PATH
		fi                                                    
	fi                                                       
}

promptmunge () {
	# If $1 is not in ENVTOKENS, add it
	if ! echo $ENVTOKENS | /bin/egrep -q "$1" ; then
		if [ "$ENVTOKENS" = "" ] ; then
	      ENVTOKENS=$1
	   else
	      ENVTOKENS="$1 $ENVTOKENS"
	   fi
	fi
	
	
	# If ENVTOKENS is not used in $PS1, add it
	if ! echo $PS1 | /bin/egrep -q "ENVTOKENS" ; then
		PS1="(\$ENVTOKENS) $PS1"
	fi
}

# ########################################################################### #
# Setting Envs #

idlpathmunge "+${SPEDAS_IDL_DIR}"
export IDL_PATH

# Get the CDF DLM module, your location will may differ
dlmpathmunge "${CDF_DLM_DIR}"

# Get the das2 DLM module...
dlmpathmunge "${PWD}/dlm"

export IDL_DLM_PATH

# Let user know we're in a special env state 
promptmunge spedas
