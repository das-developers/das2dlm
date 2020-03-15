# Building das2dlm on Windows 10 from Source

*Do you have to build from source?* Building das2dlm is somewhat tricky on
Windows.  Using a prebuilt releases may save time if your focus is on writing
IDL code that uses das2dlm instead of working on the module itself.  Using
the pre-built DLM will be covered in the main README.md file as soon as a
release is ready.

Similar to [das2C](https://github.com/das-developers/das2C), these build
instructions only assume that the command line Visual C++ tools are available
on your Windows system.  The full Visual Studio integrated development
environment is not requried.

## Prequisite: Build das2C

The big one.  First download and build
[das2C](https://github.com/das-developers/das2C) on your system.  This can be
involved but much of the system setup for building das2C applies to building
das2dlm.  Most of the environment and tool configuration needed for building
das2dlm is handled by the das2C build instructions.

Keep the location of your das2C directory handy.  You will need to know
the path to `libdas2.3.lib` and the header files from the das2C project in
the "Check the Makefile" section below.

## Get idl_export.h and idl.lib

You will need the `idl_export.h` and `idl.lib` files from your IDL distribution.
These are the only file that is needed from the distribution.  There is no need
to install the full IDL package to get these files.  Just copy them from an
existing installation that you are licensed to use.  Unfortunatly these files
cannot be redistrubed by anyone but Harris Geospatial. You'll have to get them
on you're own.

## Check the Makefile

Open the file `Windows.mak` and check on the following variables:

  * `I_VCPGK` should point to the directory containing `expat.h`, etc.
  * `L_VCPGK` should point to the directory containing `libssl.a`, etc.
  * `I_IDL` should point to the directory containing `idl_export.h`
  * `L_IDL` should point to the directory containing `idl.lib`
  * `L_DAS2` should be the to directory containing `libdas2.3.a`
  * `I_DAS2` should point to the directory containing the das2C include 
    directory, `das2`, which in turn contains `core.h`

The default paths for the variables above assume that you clone git projects
under the path `%USERPROFIE%\git`. Undoubtable you'll have your own convention
and will need to use the `set` command to alter one or more of the variables
above in your shell environment prior to calling `nmake`.

The following cmd.exe shell command provide an example of overriding these
variables in your shell.
```batchfile
set I_VCPKG="E:\Codes\SPEDAS\das2\vcpgk\installed\x64-windows-static\include"
set L_VCPKG="E:\Codes\SPEDAS\das2\vcpgk\installed\x64-windows-static\lib"
set I_IDL="C:\Program Files\Exelis\IDL84\external\include"
set L_IDL="C:\Program Files\Exelis\IDL84\bin\bin.x86_64"
set I_DAS2="E:\Codes\SPEDAS\das2\das2C"
set L_DAS2="E:\Codes\SPEDAS\das2\das2C\build.windows"
```

## Build

To put the compiler and linker on your %PATH%, issue the following command
for each new cmd.exe window you open:
  `"C:\Program Files(x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvars64.bat"`
or if you've setup your build environment using the instructions in the 
das2C module:
  `C:\opt\vs\2019\buildtools\VC\Auxiliary\Build\vcvars64.bat`

You should get the output:
```
  **********************************************************************
  ** Visual Studio 2019 Developer Command Prompt v16.4.4
  ** Copyright (c) 2019 Microsoft Corporation
  **********************************************************************
  [vcvarsall.bat] Environment initialized for: 'x64'
```
Then issue
```batchfile
  cl.exe
```
If this works, then the VC++ build tools are on your path!  

**NOTE**: You will need to intialize the VC environment whenever you open
a new `cmd.exe` window when building or changing das2dlm.

Build the DLM by running the command:
```batchfile
> nmake /f makefiles\Windows.mak
```
Output is in the `dlm` subdirectory.  If the build runs successfully, you
should see the output file `das2c.x86_64.dll`

## Test

Open a cmd.exe window and navigate to the root of the cloned das2dlm repository
for example:

```batchfile
cd %USERPROFILE%\git\das2dlm
```

Set the `IDL_DLM_PATH` environment variable to include your newly build *.DLL:

```batchfile
set IDL_DLM_PATH="%cd%\dlm;<IDL_DEFAULT>"
```
The section `<IDL_DEFAULT>` is not replacement text.  It is the a literal token that
IDL uses to indicate the default module directory, so use it as is.

There is a test IDL batch file to use it start IDL and issue a batch command
as follows.  The path to your 64-bit IDL executable may be in a different 
directory so adjust as needed:

```batchfile
C:\Users\you\git\das2dlm> "C:\Program Files\Harris\IDL87\bin\bin.x86_64\idl.exe"

IDL> @test/das2c_test.pro
```

The `@` symbol tells IDL to run the lines in the file as if they were typed
directly into the IDL interpreter.  If you get a lot of output that ends in...
```
{
    "QUERY": 1,
    "DSET": 0,
    "PDIM": "frequency",
    "VAR": "center",
    "UNITS": "Hz",
    "TYPE": "DOUBLE",
    "SHAPE": [152, -3],
    "N_VALS": 152
}
```

The congradulations, the DLM works.

## Install

There are at least two ways to install the DLM.

 1. Copy the `das2c.dlm` and `das2c.x86_64.dll` files to the IDL system
    module directory.  This varies based on where IDL is installed and what
    version you are running.  An example directory is:
    
    `C:\Program Files\Harris\IDL87\bin\bin.x86_64`
    
    In this case the install proceedure would just be the equivalent of:
    
    `copy dlm\das2c.dlm dlm\das2c.x86_64.dll "C:\Program Files\Harris\IDL87\bin\bin.x86_64"`
    

 2. Copy the whole das2dlm/dlm directory, not just the two files above,
    to some location on your computer and update your `IDL_DLM_PATH`
    environment variable to include the new directory.
    
    For example if you want to put the files in %USERPROFILE%\idl\dlm, then
    issue the following.
    
    ```batchfile
    mkdir %USERPROFILE%\idl\das2c
    copy dlm\*.* %USERPROFILE%\idl\das2c
    ```

    Next you'll need to update the IDL_DLM_PATH variable associated with your
    user environment.  To do this from the command line use the setx.exe
    command as illustrated below:
    
    ```batchfile
    setx IDL_DLM_PATH """%USERPROFILE%\idl\das2c;<IDL_DEFAULT>"""
    ```
    The **triple quotes** are critical.  Setx.exe will not affect the current
    cmd.exe session, but it will affect future command prompts.  You'll have
    to open a second command prompt to test it.  Do so now by issuing:
    
    ```batchfile
    echo %IDL_DLM_PATH%
    ```
    in a fresh command prompt.
    


















