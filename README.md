# das2dlm
IDL binary extension das2 client (in development)

## What works and what doesn't

 # The das2C error and log message system has been hooked into IDL via 
   callabacks.

 # Data can be downloaded but das2 datastructures are not converted to
   IDL structures yet.
	
 # Authentication bindings haven't been added, only freely available 
   das2 sources may be used.


## Build

Building has been tested on CentOS 7.  Building on other systems should
work fine but the makefile will need to be edited.  Build instructions 
included here for CentOS 7 may be useful for those building on other versions
of Linux.  Compiling the DLM on windows has not been attempted yet though das2C
itself has been built and tested on Windows 7 and 10.

First download and build [das2C](https://github.com/das-developers/das2C) on
your system.


The DLM uses static libs for portability when ever possible.  Install these
packages first.  Instruction are also included in the makefile.
```build
sudo yum install fftw-devel fftw-static
sudo yum install expat-devel expat-static
sudo yum install openssl-devel openssl-static
sudo yum install zlib-devel
```

Open the file `makefile` and adjust `SYSLIB_DIR`, `L_DAS2` and `I_DAS2` as 
indicated in the comments.  For Debian based systems, `SYSLIB_DIR` will be
`/usr/lib` instead of `/usr/lib64`.  The values of the other two varibles 
will depend on where your built/install das2C.

Build the DLM by running the command:
```bash
$ make
```

Output is in the `dlm` subdirectory.

### Testing

The files in the `dlm` subdirectory have to be added to your IDL DLM path.
One way to do so follows.
```bash
cd dlm
IDL_DLM_PATH="${PWD}:<IDL_DEFAULT>"
export IDL_DLM_PATH

Now run IDL as follows:
```
$ idl
IDL> s = 'http://planet.physics.uiowa.edu/das/das2Server?server=dataset'
IDL> '&dataset=Galileo/PWS/Survey_Electric&start_time=2001-001&end_time=2001-002'
IDL> n = das2c_readhttp(s)
IDL> n = das2c_readhttp(s)  ! not a typo, issue the command twice
```

You should get the output:
```
% DAS2C_READHTTP: Redirected to http://jupiter.physics.uiowa.edu/das/server?server=dataset&dataset=Galileo/PWS/Survey_Electri
                  c&start_time=2001-001&end_time=2001-002
                  
% DAS2C_READHTTP: Output not yet implemented, the following dataset was aquired:
                  Dataset: 'electric_10' from group 'electric' | i:0..4483, j:0..152
                     Property: String | title | Galileo PWS - LRS Electric
                  
                     Data Dimension: electric
                        Property: String | label | Spectral Density (V!a2!n m!a-2!n Hz!a-1!n)
                        Property: String | scaleType | log
                        Property: DatumRange | range | 1.0e-17 to 1.0e-4 V**2 m**-2 Hz**-1
                        Property: double | fill | 0.0
                     
                        Variable: center | electric[i][j] V**2 m**-2 Hz**-1 | i:0..4483, j:0..152
                     
                     Coordinate Dimension: time
                        Property: Datum | tagWidth | 80 s
                        Property: DatumRange | cacheRange | 2001-01-01T00:00:00.000 to 2001-01-02T00:00:00.000 UTC
                        Property: String | label | %{RANGE}
                     
                        Variable: center | time[i] us2000 | i:0..4483, j:-
                     
                     Coordinate Dimension: frequency
                        Property: String | label | Frequency (Hz)
                        Property: String | scaleType | log
                        Property: DatumRange | range | 5 to 6000000 Hz
                        Property: Datum | tagWidth | 1.4 lo
```

There is a bug in the URL string argument parsing for `das2c_readhttp`, that fixes
itself on the second issue of the command (not a good sign).  Hence the duplicate
command above.
