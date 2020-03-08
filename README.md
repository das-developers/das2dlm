# das2dlm
IDL binary extension das2 client (in development)

## What works and what doesn't
This project is far from ready for prime time.  It's early days yet.
here's what's working.

  * The das2C error and log message system has been hooked into IDL via 
    callabacks.

  * Data can be downloaded via the das2c_readhttp() function, but das2
    datastructures are not converted to IDL structures yet.
	
  * Stored query results can be listed using the das2c_queries()
    function.

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
```
Now run IDL as follows:

```
$ idl
IDL> s = 'http://planet.physics.uiowa.edu/das/das2Server?server=dataset' + $
IDL> '&dataset=Galileo/PWS/Survey_Electric&start_time=2001-001&end_time=2001-002'
IDL> query_id = das2c_readhttp(s)
IDL> query_id = das2c_readhttp(s)  ! not a typo, issue the command twice
```

You should get the output:
```idl
% DAS2C_READHTTP: Redirected to http://jupiter.physics.uiowa.edu/das/server?server=dataset&dataset=Galileo/PWS/Survey_Electri
                  c&start_time=2001-001&end_time=2001-002
```

Using the query id, get more info about the stored result:
```idl
IDL> das2c_queries(query_id)
{
    "id": 1,
    "datasets": 1,
    "server": "jupiter.physics.uiowa.edu",
    "source": "Galileo/PWS/Survey_Electric",
    "begin": "2001-001",
    "end": "2001-002",
    "res": "",
    "extra": "",
    "size": 686051
}
```
We see that this query returned one dataset.  Datasets are *not* broken up
by time.  A dataset does *not* represent a "files worth" of data.  Files 
don't exist as an exposed concept for das2 clients.

Rather, a dataset is a set of arrays that are correlated in index space.
Most das2 queries will return a single dataset, but that is not required
and should not be expected by clients.

The Juno Waves Survey data source is an example of a das2 data source that 
usually returns multiple datasets for a single query.  Often all electric field
data are desired for a given time period; however, Waves has multiple receiver
bands.  Each band is taking measurements at different times, thus four
different time value arrays are needed to hold the values for the four
different amplitude arrays.  A single datasets groups together the time,
frequency, and amplitude arrays for a given band.  Thus Waves E-field data
are transmitted as four diffent datasets.

Human readable information about a single dataset is provided by the 
das2c_dsinfo() function, for example:

```idl
IDL> print, das2c_dsinfo(query_id, 0) 
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

