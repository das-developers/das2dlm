# das2dlm
IDL binary extension das2 client (in development)

## What works and what doesn't
This project is far from ready for prime time.  It's early days yet.
here's what's working.

  * The das2C error and log message system has been hooked into IDL via 
    callabacks.

  * Data can be downloaded via the das2c_readhttp() function
	
  * Stored query results can be inspected using the das2c_queries(),
    das2c_datasets(), das2c_physdims(), das2c_vars(), das2c_dsprops(),
	 das2c_dsinfo(), das2c_dimprops()

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
$ make -f makefiles/Debian.mak  # Or other suitable makefile in makefiles
```

Output is in the `dlm` subdirectory.

## Testing

The files in the `dlm` subdirectory have to be added to your IDL DLM path.
One way to do so follows.
```bash
cd dlm
IDL_DLM_PATH="${PWD}:<IDL_DEFAULT>"
export IDL_DLM_PATH
```

### Get Data
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

### Inspect Datasets

Human readable information about a single dataset is provided by the 
das2c_dsinfo() function, for example:

```idl
IDL> print, das2c_dsinfo(query_id, 0)
```
```
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
		
		Variable: center | frequency[j] Hz | i:-, j:0..152
```
The following call provides a summary of all the datasets in the query result.
(output manually collapsed)
```idl
IDL> das2c_datasets()
{"id": 0, "name": "electric_10", "physdims": 3, "props": 1, "shape": [4483,152], "size": 686051 }
```

### Inspecting Physical Dimensions (i.e. Variable Groups)

The following call will provide a summary of all physical dimensions involved in the 0th
dataset. (output manually collapsed)
```idl
IDL> das2c_physdims(query_id, 0)
[
  {"id": 0, "name": "time", "vars": 1, "props": 3, "size": 4483 },
  {"id": 1, "name": "frequency", "vars": 1, "props": 4, "size": 152 },
  {"id": 2, "name": "electric", "vars": 1, "props": 4, "size": 681416 }
]
```
Since "physical dimension" is a long term, we'll shorten it to *p-dim*.  Each 
p-dim usually corresponds to a degree of freedom in the real world.  For example 
location, time, wave-amplitude etc.  These may happen to correspond to an array
dimension, but that is only a happenstance.  Array dimensions are merely book
keeping, as all datasets can be flattened into a set of 1-D arrays with out affecting
thier physical meaning.  To keep from confusing the two terms, we'll use the word
*rank* to describe the number of independently varying index values used to correlate
values in a dataset.

### Listing Variables

In each p-dim there are 1-N variables that supply data values.  Each variable 
provides values for a particular purpose.  The most common purpose is to provide
bin-center values.  The following code lists all the variables for a p-dim 'time'.
```idl
IDL> das2c_vars(query_id, 0, 'time')
[
  {"id": 0, "name": "time", "role": "center", "shape": [4483, -3], "size": 4483 },
]
```
The value `-3` in the shape field indicates that the variable is degenerate in 
the second index.  This is common for cubic datasets, but not all datasets are
cubic.  For example sweep frequency receivers typically step in time as well as
frequency.  Thus the time centers are an array as large as the amplitude dataset.

### Geting Data Ararys

TODO: Very next item


### Getting Metadata

Datasets contain metadata properties, such as titles and labels.  Metadata may be
attached either for the dataset as a whole, such as the 'title' property above or
to physical dimension variable groups.  The following two calls illustrate gathering
all the metadata from the 0th dataset.
```idl
IDL> das2c_dsprops(query_id, 0)
[
   {"key":"title", "type":"String", "value":"Galileo PWS - LRS Electric"}
]

IDL> das2c_dimprops(query_id, 0, 'electric')
[
  {"key":"label", "type":"String, "value":"Spectral Density (V!a2!n m!a-2!n Hz!a-1!n)"},
  {"key":"scaleType", "type":"String, "value":"log"},
  {"key":"range", "type":"DatumRange", "value":"1.0e-17 to 1.0e-4 V**2 m**-2 Hz**-1"},
  {"key":"fill", "type":"double", "value":"0.0"}
]

IDL> das2c_dimprops(query_id, 0, 'time')
[
	{"key":"tagWidth", "type":"Datum"; "value":"80 s"},
	{"key":"cacheRange", "type":"DatumRange",
	 "value":"2001-01-01T00:00:00.000 to 2001-01-02T00:00:00.000 UTC"
	},
]
IDL> das2c_dimprops(query_id, 0, 'frequency')
[
	{"key":"label", "type":"String, "value":"Frequency (Hz)"},
	{"key":"scaleType", "type":"String, "value":"log"},
	{"key":"range", "type":"DatumRange", "value":"5 to 6000000 Hz"}
]
```
