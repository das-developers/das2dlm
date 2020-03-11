# das2dlm
IDL binary extension das2 client (in development)

## What works and what doesn't
This project is far from ready for prime time.  It's early days yet.
here's what's working.

  * Nothing.  This branch is in the middle of an API refactor.
  
## Build and Config

Building has been tested on CentOS 7 and KDE Neon.  Building on other systems
should work fine but the makefile will need to be edited.  Build instructions 
included here for CentOS 7 may be useful for those building on other versions
of Linux.  Compiling the DLM on windows has not been attempted yet though das2C
itself has been built and tested on Windows 7 and 10.

First download and build [das2C](https://github.com/das-developers/das2C) on
your system.

### Prerequisites

The DLM uses static libs for portability when ever possible.  Install these
packages first.  Instruction are also included in the makefile.
```build
sudo yum install fftw-devel fftw-static
sudo yum install expat-devel expat-static
sudo yum install openssl-devel openssl-static
sudo yum install zlib-devel
```

### Build

Open the file `makefile` and adjust `SYSLIB_DIR`, `L_DAS2` and `I_DAS2` as 
indicated in the comments.  For Debian based systems, `SYSLIB_DIR` will be
`/usr/lib` instead of `/usr/lib64`.  The values of the other two varibles 
will depend on where your built/install das2C.

Build the DLM by running the command:
```bash
$ make -f makefiles/Debian.mak  # Or other suitable makefile in makefiles
```

Output is in the `dlm` subdirectory.

### IDL Environment Setup

The files in the `dlm` subdirectory have to be added to your IDL DLM path.
One way to do so follows.
```bash
cd dlm
IDL_DLM_PATH="${PWD}:<IDL_DEFAULT>"
export IDL_DLM_PATH
```

## Usage

Reading local files and command pipes is not yet implemented, only queries
to open HTTP das2 servers is currently supported.

```idl
IDL> s = 'http://planet.physics.uiowa.edu/das/das2Server?server=dataset' + $
IDL> '&dataset=Galileo/PWS/Survey_Electric&start_time=2001-001&end_time=2001-002'
IDL> query = das2c_readhttp(s)
IDL> query
```
You should get the output:
```
% DAS2C_READHTTP: Redirected to http://jupiter.physics.uiowa.edu/das/server?server=dataset&dataset=Galileo/PWS/Survey_Electri
                  c&start_time=2001-001&end_time=2001-002
{
    "QUERY": 1,
    "SERVER": "jupiter.physics.uiowa.edu",
    "SOURCE": "Galileo/PWS/Survey_Electric",
    "BEGIN": "2001-001",
    "END": "2001-002",
    "RES": "",
    "EXTRA": "",
    "N_DSETS": 1,
    "N_VALS": 686051
}
```

All query result summarys can be listed by calling das2c_queries(), e.g.:
```idl
IDL> das2c_queries()
```

The value of the `N_DSETS` field is 1.  That means this query returned one
dataset.  Datasets are *not* broken up by time.  A dataset does *not* represent
a "files worth" of data.  Files don't exist as an exposed concept for das2
clients.  

Rather, a dataset is a set of arrays that are correlated in index space. Most 
das2 queries will return a single dataset, but that is not required and should
not be expected by clients.

The Juno Waves Survey data source is an example of a das2 data source that 
usually returns multiple datasets for a single query.  Often all electric field
data are desired for a given time period; however, Waves has multiple receiver
bands.  Each band is taking measurements at different times, thus four different
time value arrays are needed to hold the values for the four different amplitude
arrays.  A single datasets groups together the time, frequency, and amplitude
arrays for a given band.  Thus Waves E-field data are transmitted as four
diffent datasets.

### Inspect Datasets

Ask the query for dataset 0 and print some a human readable description.

```idl
IDL> ds = das2c_datasets(query, 0)
IDL> print, das2c_info(ds)
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
IDL> das2c_datasets(query)
{ 
  "QUERY":   1,
  "DSET":    0,
  "NAME":    "electric_10",
  "RANK":    2,
  "SHAPE":   [-99, -99, -99, -99, -99, -99, 4483, 152], 
  "N_PDIMS": 3,
  "N_PROPS": 10,
  "N_VALS":  686051 
 }
```
The SHAPE field bears explaination.  This is the overall extent of the dataset
in index space.  Due to a limitation in the IDL DLM interface for structure
arrays, all SHAPE fields must be the same size.  Since IDL arrays may accessed
by up to 8 indexes (i.e have up to 8 array dimensions), the SHAPE value always
has 8 elements, however only the last RANK elements are valid.  A bit of IDL
code will cut out the unneeded elements:
```idl
IDL> ds.shape[ 8 - ds.rank : -1 ]
```

### Inspecting Physical Dimensions (i.e. Variable Groups)

The following call will provide a summary of all physical dimensions involved in
the dataset. (output manually collapsed)
```idl
IDL> das2c_pdims(ds)
[
  {"QUERY":1, "DSET":0, "PDIM":"time",      "USE":"COORD", "N_VARS": 1, "N_PROPS": 3, "N_VALS": 4483 },
  {"QUERY":1, "DSET":0, "PDIM":"frequency", "USE":"COORD", "N_VARS": 1, "N_PROPS": 4, "N_VALS": 152 },
  {"QUERY":1, "DSET":0, "PDIM":"electric",  "USE":"DATA",  "N_VARS": 1, "N_PROPS": 4, "N_VALS": 681416 }
]
```
Since "physical dimension" is a long term, we'll shorten it to p-dim.  Each
p-dim usually corresponds to a degree of freedom in the real world.  For example 
location, time, wave-amplitude etc.  These may happen to correspond to an array
dimension, but that is only a happenstance.  Array dimensions are merely book
keeping, as all datasets can be flattened into a set of 1-D arrays with out
affecting thier physical meaning.

The p-dims in play for this dataset are:

  * time - a coordinate space
  * frequency - another coordinate space
  * electric - a data space

### Listing Variables

In each p-dim there are 1-N variables that supply data values.  Each variable 
provides values for a particular purpose.  The most common purpose is to provide
bin-center values.  The following code lists all the variables for the 
'frequency' dimension.

```idl
IDL> pd_freq = das2c_pdims(ds, 'frequency')
IDL> das2c_vars(pd_freq)
{ 
   "QUERY": 1,
   "DSET":  0,
   "PDIM":  "frequency",
   "VAR":   "center", 
   "UNITS": "Hz"
   "SHAPE": [-3, 152],
   "TYPE":  "FLOAT",
   "N_VALS": 152 
},
```

Turns out there is only one.  Also, since all variables have the same shape as
the overall dataset, SHAPE fields for variables are only as long as the RANK 
of the dataset.  The `-3` values in the SHAPE array is a degeneracy flag.  This
says that it doesn't matter what value is given for the first index when accesing
data for this variable, it won't affect the output.

The 'TYPE' field gives the IDL data type for data values provided by this
variable

### Geting Data Ararys

The following code will get a single IDL array for all the center point values
in the time dimension.  It will return an array that is 4483 by 152 values long,
for a total of 686051 values.

```idl
IDL> pd_freq = das2c_pdims(ds, 'frequency')
IDL> v_freq = das2c_vars(pd_freq, 'center')
IDL> ary = das2c_data(v_freq)
```

The code above is not memory or time efficient.  Since the SHAPE field for the
frequency centers has a degenerate flag in the first index.  We can spare 
ourselves alot of unnecessary memory usage if we just get a slice of frequencies
for any valid first index.  The code below illustrates a slice operation:

```idl
IDL> ary = das2c_data(v_freq, {I:0, J:'*'})
```
The anoymous structure given for the second argument is a `slice structure`.
It says, "hold the first index constant at 0 and let the second index vary over
all valid indices".  The fields in a slice structure are:

  * I - The first index
  * J - The second index
  * K - The third index
  * L - The fourth index
  * M - The fifth index
  * N - The sixth index
  * 0 - The seventh index
  * P - The eigth index

Fields for indexes larger than the RANK of the dataset are ignored.  Currently
only positive and negative intergers and the string `'*'` are undersood by the
`das2c_data` function.  So for example `-1` could have been used for the I value
to indicate that the highest valid first index, whatever that happened to be.

Usage of the IDL [CREATE_STRUCT](https://www.harrisgeospatial.com/docs/create_struct.html)
command can be used to interate over SHAPE fields and generate efficient slice
structures.

### Getting Metadata

Datasets contain metadata properties, such as titles and labels.  Metadata may be
attached either for the dataset as a whole, such as the 'title' property above or
to physical dimension variable groups.  The following gathers all properties for
the dataset
```idl
IDL> das2c_props(ds)
[
   {"KEY":"title", "PTYPE":"String", "VALUE":"Galileo PWS - LRS Electric"}
]
```

Physical dimensions also have properties, these may be accessed in the same 
manner.
```idl
IDL> pd_amp = das2c_pdims(ds, 'electric')
IDL> das2c_props(pd_amp)
[
  {"KEY":"label",     "PTYPE":"String",     "VALUE":"Spectral Density (V!a2!n m!a-2!n Hz!a-1!n)"},
  {"KEY":"scaleType", "PTYPE":"String",     "VALUE":"log"},
  {"KEY":"range",     "PTYPE":"DatumRange", "VALUE":"1.0e-17 to 1.0e-4 V**2 m**-2 Hz**-1"},
  {"KEY":"fill",      "PTYPE":"double",     "VALUE":"0.0"}
]

IDL> pd_time = das2c_pdims(ds, 'time')
IDL> das2c_props(pd_time)
[
   {"KEY":"tagWidth",   "PTYPE":"Datum",      "VALUE":"80 s"},
   {"KEY":"cacheRange", "PTYPE":"DatumRange", "VALUE":"2001-01-01T00:00 to 2001-01-02T00:00 UTC"
   },
]
IDL> pd_freq = das2c_pdims(ds, 'frequency')
IDL> das2c_props(pd_freq)

[
   {"KEY":"label",     "PTYPE":"String",     "VALUE":"Frequency (Hz)"},
   {"KEY":"scaleType", "PTYPE":"String",     "VALUE":"log"},
   {"KEY":"range",     "PTYPE":"DatumRange", "VALUE":"5 to 6000000 Hz"}
]
```

