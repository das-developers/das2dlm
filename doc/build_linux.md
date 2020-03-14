# Building das2dlm on Linux

Building das2dlm is relativly easy on Linux, but a prebuild release may save
you time your focus is on writting IDL code uses das2dlm instead of working on
the module itself.  Using the pre-built DLM will be coverd in the main
README.md file as soon as a release is ready.

Linux development has been tested on CentOS 7 and KDE Neon.  Building on other
systems should work fine but the makefile will need to be edited.  Instruction
for building on Windows are in the build_windows.md file.


## First build das2C

The big one.  First download and build
[das2C](https://github.com/das-developers/das2C) on your system.  This can be
involved but much of the system setup for building das2C applies to building
das2dlm.

You will need to know the location of the libdas2.3.a and the header files
from the das2C project in the "Check Makefile" section below.

## Add system packages

The DLM uses static libs for portability when ever possible.  Install these
packages first.  For RedHat and CentOS the packages are:
```sh
sudo yum install fftw-devel fftw-static
sudo yum install expat-devel expat-static
sudo yum install openssl-devel openssl-static
sudo yum install zlib-devel
```
For Debian derived systems you'll need:
```sh
sudo apt install libfftw3 libfftw3-dev
sudo apt install libexpat1 libexpat1-dev
sudo apt install libssl libssl-dev
```

## Get idl_export.h

You will need the `idl_export.h` file from your IDL distribution.  This is the
only file that is needed from the distribution.  There is no need to install
the full IDL package to get this file.  Just copy it from an existing 
installation that you own.  Unfortunatly the `idl_export.h` file cannot be
redistribute with the das2dlm sources due to usage restrictions.  You'll have
to get it on you're own.


## Check makefile

Open the file `RedHat.mak` or `Debian.mak` depending on if your Linux system
is RedHat derived, or Debian derived, and check on the following variables:

  * `SYSLIB_DIR` should point to the directory containing `libssl.a`, etc.
  * `I_IDL` should point to the directory containing `idl_export.h`
  * `L_DAS2` should point to the directory containing `libdas2.3.a`
  * `I_DAS2` should point to the directory containing the das2C include 
    directory, `das2`.

If you're distribution is vastly different from either of these, copy over 
one of the two makefiles above and edit it.

## Build

Build the DLM by running the command:
```bash
$ make -f makefiles/Debian.mak  # Or other suitable makefile in makefiles
```
Output is in the `dlm` subdirectory.  If the build runs successfully, you
should see the output file `das2c.linux.x86_64.so`

## Testing

IDL uses the environment variable `IDL_DLM_PATH` to find new system rountines
that were not delivered with the IDL distribution.  You can add the `dlm` the
directory to the path using the following commands.

```sh
IDL_DLM_PATH="${PWD}/dlm:<IDL_DEFAULT>"
export IDL_DLM_PATH
```
The section `<IDL_DEFAULT>` is not replacement text.  It is the a literal token that
IDL uses to indicate the default module directory, so use it as is.

The test script consists of commands as they would be entered at the IDL prompt.
To run the script, redirect the contents of the script to standard input as 
illustrated below.

```sh
idl < test/das2c_test.pro
```

Or start IDL interactively and copy-and-paste the commands at the IDL 
interpreter prompt.

## Install

There are two ways to install the DLM.

 1. Copy the `das2c.dlm` and `das2c.linux.x86_64.so` files to the IDL system
    module directory.  This varies based on where IDL is installed and what
    version you are running.  An example directory is:
	 
    `/usr/local/harris/idl87/bin/bin.linux.x86_64`
		
    In this case the install proceedure would just be:
    
    `sudo cp dlm/das2c.dlm dlm/das2c.linux.x86_64.so /usr/local/harris/idl87/bin/bin.linux.x86_64`
	 

 2. Copy the whole das2dlm/dlm directory, not just the two files above,
    to some location on your computer and update your `IDL_DLM_PATH`
    environment variable to include the new directory.
	 
    For example if you want to put the files in $HOME/idl/dlm, then issue the following.
    ```sh
    mkdir -p $HOME/idl
    cp -r das2c/dlm $HOME/idl
    ```

    Then add the following lines to your .bash_profile or .bashrc files:
    ```sh
    dlmpathmunge () {
     if ! echo $IDL_DLM_PATH | /bin/egrep -q "(^|:)$1($|:)" ; then
      if \[ "$IDL_DLM_PATH" = "" \] ; then                        
       IDL_DLM_PATH="$1:<IDL_DEFAULT>"                        
      else                                                  
       IDL_DLM_PATH=$1:$IDL_DLM_PATH
      fi                                                    
     fi                                                       
    }
   
    dlmpathmunge $HOME/idl/dlm
    ```
















