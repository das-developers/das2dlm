# das2dlm
Das2dlm is an IDL (Interactive Data Language) DLM (Dynamically Loadable Module) that
provides bindings for the [das2C](https://github.com/das-developers/das2C) library.  It
is compatable with IDL 8.0 or newer.  Once installed, das2dlm functions are new IDL
system rountines and thus do not require any specific initialization calls such as
[linkimage](https://www.harrisgeospatial.com/docs/using_idl_LinkImage.html).

## Current Functionality

Reading local files and command pipes is not yet implemented.  Only queries
to HTTP servers providing a GET API are currently supported.  Caching query
output in local data files is also not supported in this early version.

This DLM is not tied to an specific query API, such as das2 or HAPI.  It issues
an HTTP request to a URL that you format.  On the other hand, this DLM very much
cares about the format of the data stream output by the server.  The supported
stream formats are limited to those that are parseable by the [das2C](https://github.com/das-developers/das2C)
library.  At the time of writing only das2.2.2 (or lower) streams
are supported, though once das2C supports das2.3 and HAPI 2.0 streams, this
module will too.
  
## Build/Download

The easiest way to get das2dlm is to download a [binary release](https://github.com/das-developers/das2dlm/releases).
The most recent release is version [0.4.1](https://github.com/das-developers/das2dlm/releases/tag/0.4.1).

If you would like to compile das2dlm from source code, instructions for doing so may be
found in:

  * [doc/build_linux.md](https://github.com/das-developers/das2dlm/blob/master/doc/build_linux.md)
  * [doc/build_windows.md](https://github.com/das-developers/das2dlm/blob/master/doc/build_windows.md)
  * [doc/build_mac.md](https://github.com/das-developers/das2dlm/blob/master/doc/build_mac.md)

Follow one of the files above to create the DLM.

## Install
After downloading the DLM or building it from source, you have two installation choices:
  1. Copy the contents into the IDL default extension location.  Since IDL can be installed
     in a variety of locations an exact path can't be provided here, but example 
     locations for various operating systems follow:
     ```
     /usr/local/harris/idl87/bin/bin.linux.x86_64    (Linux example)
     /Applications/harris/idl/bin/bin.darwin.x86_64  (MacOS example)
     C:\Program Files\Harris\IDL87\bin\bin.x86_64    (Windows example)
     ```
     Only the contents of the dlm directory should be copied in, not the whole directory
     itself.  So for example copy in `das2c.darwin.x86_64.so` and `das2c.dlm` not the
     whole directory `das2dlm-dsym-0.4.0`.
     
  2. Alternatively, you can copy the contents of the DLM to some other directory and update
     your `IDL_DLM_PATH` environment variable.  The default IDL path must be retained in
     the environment variable.  Examples of setting `IDL_DLM_PATH` for various operating
     systems follow:
     ```
     export IDL_DLM_PATH="/home/myname/das2dlm-dsym-0.4.0:<IDL_DEFAULT>"   (Linux/MacOS)
     set IDL_DLM_PATH=C:\Users\myname\das2dlm-dsym-0.4.0:<IDL_DEFAULT>     (Windows)
     ```
     The text `<IDL_DEFAULT>` is a flag  that must be included as the last item in the
     path or else IDL will not be able to find it's own modules. See
     [DLMs](https://www.harrisgeospatial.com/docs/DLM.html) in the IDL documentation
     for details.
     
## Usage

The following will download data from a Galileo mission das2 data source
at the University of Iowa, save the results, and output a DAS2C_QUERY
structure that can be used to access the data values.

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
The structure printed above is a DAS2C_QUERY structure.  Notice that value of the
`N_DSETS` field is 1.  That means this query returned one dataset.  Datasets are
*not* broken up by time.  A dataset does *not* represent a "files worth" of data.
Files don't exist as an exposed concept for das2 clients.  Rather, a dataset is 
defined as *a set of arrays correlated in index space*.  Most das2 queries
will return a single dataset, but that is not required and should not be expected
by clients.

The Juno Waves Survey data source is an example of a das2 data source that 
usually returns multiple datasets for a single query.  Often all electric field
data from the instrument are desired for a given time period; however, Waves has
multiple receiver bands.  Each band is taking measurements at different times,
thus four different time value arrays are needed to provide the time tags for
the four different amplitude arrays.  A single dataset groups together the 
time, frequency, and amplitude arrays for a given band.  Thus Waves E-field data
are transmitted as four sets of correlated arrays, i.e. four datasets.

DAS2C_QUERY structures are also generated by the das2c_queries() function.
All query result summarys can be listed by calling das2c_queries() with no arguments.
```idl
IDL> das2c_queries()
```

### Inspecting Datasets

Ask the query for dataset 0 and print some a human readable description.

```idl
IDL> ds = das2c_datasets(query, 0)
IDL> das2c_dsinfo(ds)
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
The SHAPE field bears explanation.  This is the overall extent of the dataset
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
affecting their physical meaning.

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
of the dataset.  The `-3` in the SHAPE array above is a degeneracy flag.  It
means that the value of the first index does affect the output from the 
`das2c_data()` function. The 'TYPE' field gives the IDL data type for data
values provided by this variable

### Accessing Data Arrays

The following code will get a single IDL array for all the center point values
in the time dimension.  It will return an array that is 4483 by 152 values long,
for a total of 686051 values.

```idl
IDL> pd_freq = das2c_pdims(ds, 'frequency')
IDL> v_freq = das2c_vars(pd_freq, 'center')
IDL> ary = das2c_data(v_freq)
```

The code above is not memory or time efficient.  Since the SHAPE field for the
frequency centers has a degenerate flag in the first index, we can spare 
ourselves alot of unnecessary memory usage if we just get a slice of frequencies
for any valid first index.  The code below illustrates a slice operation:

```idl
IDL> ary = das2c_data(v_freq, {I:0, J:'*'})
```
The anonymous structure given for the second argument is a *slice structure*.
The slice structure above indicates that a rank 1 array should be output and
this is to be accomplished by holding the first index constant at 0 and letting
the second index vary over all valid values.  The recognized fields in a slice
structure are:

  * I - The first index
  * J - The second index
  * K - The third index
  * L - The fourth index
  * M - The fifth index
  * N - The sixth index
  * 0 - The seventh index
  * P - The eighth index

Fields for indexes larger than the RANK of the dataset are ignored.  Currently
only integers, two-element arrays, and the string `'*'` are undersood by the
`das2c_data` function.  So for example `-1` could have been used for field I
to indicate the highest valid first index value, whatever that happened to be.

Given this interface, the following two lines of code are equivalent:
```idl
IDL> ary = das2c_data(v_freq)
IDL> ary = das2c_data(v_freq, {I:'*', J:'*'})
```

Further array slicing examples follow:
```idl
IDL> pd_spec = das2c_pdims(ds, 'electric')
IDL> v_spec  = das2c_vars(pd_spec, 'center')       ;Center point electric spectral densities

IDL> ary = das2c_data(v_spec, {i:0,j:0})           ;Slice struct tag names are case insensitive
IDL> ary = das2c_data(v_spec, {I:0})               ;Here the J range defaults to '*' 
IDL> ary = das2c_data(v_spec, {J:[-10,-1]})        ;get all I's, and last 10 J's
IDL> ary = das2c_data(v_spec, {J:[2,4], K:6, L:7}) ;dataset rank = 2, so K and L are ignored
```

**Hint** Building slice structures in a loop can be accomplished using the IDL
[CREATE_STRUCT](https://www.harrisgeospatial.com/docs/create_struct.html)
function.  As you iterate over each element in SHAPE, use CREATE_STRUCT to add
fields to a slice structure.

### Reading Metadata

Datasets contain metadata properties, such as titles and labels.  Metadata may be
attached either for the dataset as a whole, such as the 'title' property above or
to physical dimension variable groups.  The following gathers all properties for
the dataset
```idl
IDL> das2c_props(ds)
[
   {"KEY":"title", "TYPE":"String", "VALUE":"Galileo PWS - LRS Electric"}
]
```

Physical dimensions also have properties, these may be accessed in the same 
manner.
```idl
IDL> pd_amp = das2c_pdims(ds, 'electric')
IDL> das2c_props(pd_amp)
[
  {"KEY":"label",     "TYPE":"String",     "VALUE":"Spectral Density (V!a2!n m!a-2!n Hz!a-1!n)"},
  {"KEY":"scaleType", "TYPE":"String",     "VALUE":"log"},
  {"KEY":"range",     "TYPE":"DatumRange", "VALUE":"1.0e-17 to 1.0e-4 V**2 m**-2 Hz**-1"},
  {"KEY":"fill",      "TYPE":"double",     "VALUE":"0.0"}
]

IDL> pd_time = das2c_pdims(ds, 'time')
IDL> das2c_props(pd_time)
[
   {"KEY":"tagWidth",   "TYPE":"Datum",      "VALUE":"80 s"},
   {"KEY":"cacheRange", "TYPE":"DatumRange", "VALUE":"2001-01-01T00:00 to 2001-01-02T00:00 UTC"}
]
IDL> pd_freq = das2c_pdims(ds, 'frequency')
IDL> das2c_props(pd_freq)

[
   {"KEY":"label",     "TYPE":"String",     "VALUE":"Frequency (Hz)"},
   {"KEY":"scaleType", "TYPE":"String",     "VALUE":"log"},
   {"KEY":"range",     "TYPE":"DatumRange", "VALUE":"5 to 6000000 Hz"}
]
```

### Cleaning Up

To delete a das2c object and free it's memory use the `das2c_free` function as illustrated below:
```idl
IDL> das2c_free(query)
```
Calling `das2c_free` on a query releases all internal data structures and memory associated
with the query; however, it will not pull the rug out from under your code.  Any internal
data arrays that have been wrapped by
[IDL_ImportArray](https://www.harrisgeospatial.com/docs/CreateArrayFromExistingData.html)
will not be deleted until IDL is done with them.
