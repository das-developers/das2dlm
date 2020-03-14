# Building das2dlm on Windows 10  (in progress)

Building das2dlm is some what tricky on Windows.  Using a prebuilt releases may
save time if your focus is on writing IDL code that uses das2dlm instead of
working on the module itself.  Using the pre-built DLM will be coverd in the
main README.md file as soon as a release is ready.

Windows development has been tested on Windows 10 using the command line
Visual C++ tool set.

## First build das2C

The big one.  First download and build
[das2C](https://github.com/das-developers/das2C) on your system.  This can be
involved but much of the system setup for building das2C applies to building
das2dlm.  Most of the environment and tool setup needed for build das2dlm is
handled by the das2C build instructions.

Keep the location of your das2C build output handy.  You will need to know
the path to `libdas2.3.lib` and the header files from the das2C project in
the "Check Makefile" section below.

In case you have already done that but it has been a while, you will need to
initialize your Visual Studio tools environment by opening a `cmd.exe` prompt
and issuing the following command to run the config script:

  `C:\opt\vs\2019\buildtools\VC\Auxiliary\Build\vcvars64.bat`
  
or 

  `"C:\Program Files(x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvars64.bat"`

if you using the default VC tools install location.  You should get the output:
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
If this works, then you now have a compiler on your path!  

**NOTE**: You will need to intialize the VC environment whenever you open
a new `cmd.exe` window when building or changing das2dlm.

## Get idl_export.h

You will need the `idl_export.h` file from your IDL distribution.  This is the
only file that is needed from the distribution.  There is no need to install
the full IDL package to get this file.  Just copy it from an existing 
installation that you own.  Unfortunatly the `idl_export.h` file cannot be
redistribute with the das2dlm sources due to usage restrictions.  You'll have
to get it on you're own.

## Check Makefile

Open the file `Windows.mak` and check on the following variables:

  * `SYSLIB_DIR` should point to the directory containing `libssl.a`, etc.
  * `I_IDL` should point to the directory containing `idl_export.h`
  * `L_DAS2` should be the full path to `libdas2.3.a`
  * `I_DAS2` should point to the directory containing the das2C include 
    directory, `das2`.

The default paths for the variables above assume that you clone git projects
under the path `%USERPROFIE%\git`. Undoubtable you'll have your own convention
and will need to modify the make file variables above.

If any of these are incorrect for your system then override them with 
environment variables, for example:

```batchfile
set I_IDL="C:\Program Files\Exelis\IDL84\bin\bin.x86_64\external\include\"
set L_DAS2="e:\Codes\SPEDAS\das2\das2C\build\libdas2.3.lib"
set I_DAS2="e:\Codes\SPEDAS\das2\das2C"


## Build

Build the DLM by running the command:
```batchfile
> nmake /f makefiles\Windows.mak
```
Output is in the `dlm` subdirectory.  If the build runs successfully, you
should see the output file `das2c.x86_64.dll`

## Testing

TODO:


















