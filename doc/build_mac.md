# Building das2dlm on Mac OS X

Building das2dlm is easy enough on MacOS, but a prebuild release may save
you time if your focus is on writting IDL code uses das2dlm instead of working
on the module itself.  Using the pre-built DLM is covered in the README.md
file.  The binary module should be very easy to deploy, just copy it's top level
directory and contents to the module path inside your IDL distribution.  If you still 
want to compile the software on MacOS anyway, even if just for fun, read on.


Mac development has been tested on 10.13 High Serria.  Building on other MacOS
versions should work fine but the makefile may need to be edited.  Instructions
for building on Windows are in the build_windows.md file and instructions for
building on Linux are in the build_linux.md file.

## First build das2C

The big one.  First download and build
[das2C](https://github.com/das-developers/das2C) on your system.  This can be
involved but much of the system setup for building das2C applies to building
das2dlm.

You will need to know the location of the libdas2.3.a and the header files
from the das2C project in the "Check Makefile" section below.

## Add system packages

The DLM uses static libs for portability whenever possible.  The `fft` and 
`openssl` packages installed by the `brew` commands in the das2C instructions
should have already installed some of the required static libraries.  If for
some reason you dont have these
```sh
brew install fftw
brew install openssl
```

## Get idl_export.h

You will need the `idl_export.h` file from your IDL 8.0 (or higher) distribution. 
This is the only file that is needed from the distribution.  There is no need to
install the full IDL package to get this file.  Just copy it from an existing 
installation that you own.

Unfortunatly the `idl_export.h` file cannot be redistribute with the das2dlm
sources due to usage restrictions.  You'll have to get it on your own.  On
my Mac the file was found in:
```bash
/Applications/harris/idl/external/include/idl_export.h
```

## Check makefile

Open the file `Darwin.mak` and check on the following variables:

  * `BREWLIB_DIR` should point to the directory containing `libfftw3.a`
  * `SSL_DIR` should point to the directory containing `libssl.a` and 
             `libcrypto.a`
  * `I_IDL` should point to the directory containing `idl_export.h`
  * `L_DAS2` should point to the directory containing `libdas2.3.a`
  * `I_DAS2` should point to the directory containing the das2C include 
    directory, `das2`.

If you use a different package manager such as MacPorts the default above will
certianly need to be edited.

## Build

Build the DLM by running the command:
```bash
$ make -f makefiles/Darwin.mak  # Or other suitable makefile in makefiles
```
Output is in the `dlm` subdirectory.  If the build runs successfully, you
should see the output file `das2c.darwin.x86_64.so`

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
/Applications/harris/idl/bin/bin.darwin.x86_64/idl < test/das2c_test.pro
```

Or start IDL interactively and copy-and-paste the commands at the IDL 
interpreter prompt.

## Install

There are at least two ways to install the DLM.

 1. Copy the `das2c.dlm` and `das2c.darwin.x86_64.so` files to the IDL system
    module directory.  This varies based on where IDL is installed and what
    version you are running.  An example directory is:
    
    `/Applications/harris/idl87/bin/bin.darwin.x86_64`
      
    In this case the install proceedure would just be:
    
    `sudo cp dlm/das2c.dlm dlm/das2c.darwin.x86_64.so /Applications/harris/idl87/bin/bin.darwin.x86_64`
    

 2. Copy the whole das2dlm/dlm directory, not just the two files above,
    to some location on your computer and update your `IDL_DLM_PATH`
    environment variable to include the new directory.
    
    For example if you want to put the files in $HOME/idl/dlm, then issue the following.
    ```sh
    mkdir -p $HOME/idl
    cp -r dlm $HOME/idl
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
















