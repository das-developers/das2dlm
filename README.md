# Das2DLM
Das2dlm is an IDL (Interactive Data Language) DLM (Dynamically Loadable Module) that
provides bindings for the [das2C](https://github.com/das-developers/das2C) library.  It
is compatable with IDL 8.5 or newer.  Once installed, das2dlm functions are new IDL
system rountines and thus do not require any specific initialization calls such as
"linkimage".

*Note: This project has submodules, to clone them to use `--recursive`.*
```bash
git clone --recursive git@github.com:das-developers/das2dlm.git
```
(or the https equivalent)

## Current Functionality

As of v0.5, three intended API functions have not been written 
(see [the wiki](https://github.com/das-developers/das2dlm/wiki)) for details.

## Package Install

**das2dlm** is an IDL package and may be installed using the,
[IDL Package Manager (IPM)](https://www.nv5geospatialsoftware.com/docs/ipm.html). 
The IPM command only installs github.com releases (does not use clone).  If you
are using IDL 8.7.1 or higher, das2pro can be downloaded and installed by issuing
the single IDL command:

`IPM, /install, 'https://github.com/das-developers/das2dlm'`

The `IPM` command automatically handles updating your `IDL_DLM_PATH`.

To update das2dlm to the latest version run the IPM command:

`IPM, /update, 'das2dlm'`

and to remove das2dlm from your packages directory issue:

`IPM, /remove, 'das2dlm'`
  
## Manual Binary Install

If you are not running IDL 8.7.1 or higher, but are using at least IDL 8.5, 
das2dlm [binary releases](https://github.com/das-developers/das2dlm/releases)
will still work, but you'll have to download and unzip them manually.  The
contents of the zip file must be placed somewhere on your `IDL_DLM_PATH`. 
Note, the contents of the zip go directly into the directory, 
*do not make a sub-directory*.

To find your current IDL_DLM_PATH issue:
```idl
print, !DLM_PATH
```
If none of the directories output above seem suitable (or you just can't)
write to them, then add a custom directory to the IDL_DLM_PATH.  Here's
an example of doing so:
```bash
export IDL_DLM_PATH="$HOME/my_dlms:<IDL_DEFAULT>"
```
The text `<IDL_DEFAULT>` is a flag that must be included as the last item in the
path or else IDL will not be able to find it's own modules. See
[DLMs](https://www.nv5geospatialsoftware.com/docs/DLM.html) in the IDL documentation
for details.

## Building

This repository has submodules.  To get them automatically issue:
```bash
git clone --recursive git@github.com:das-developers/das2dlm.git
```
or http equilavent.  If you forget to use `--recursive` then issue:
```bash
git submodule update --init --recursive
```

To compile das2dlm from source code, instructions for doing so may be found in:

  * [doc/build_linux.md](https://github.com/das-developers/das2dlm/blob/master/doc/build_linux.md)
  * [doc/build_windows.md](https://github.com/das-developers/das2dlm/blob/master/doc/build_windows.md)
  * [doc/build_mac.md](https://github.com/das-developers/das2dlm/blob/master/doc/build_mac.md)

Follow one of the files above to create the DLM.  After creating the shared object (.so)
or dynamic load library (.dll), copy it, along with the `das2c.dlm` file into some
directory on your `IDL_DLM_PATH`.
     
## Usage

The das2 DLM communicates with das2 servers.  Each server provides access
to one or more data sources, each of which can produce a data stream.  The
following example lists all the data sources on a server commonly used for
the Juno Mission to Jupiter.
```idl
IDL> das2c_srclist("https://jupiter.physics.uiowa.edu/das/server")
```
Outputs...
```
  {
    "PATH": "Juno/Ephemeris/GanymedeCoRotational",
    "PROVIDES": "Juno Ganymede Co-Rotational orbit parameters"
  },
  {
    "PATH": "Juno/WAV/Survey",
    "PROVIDES": "Survey Mode Electric or Magnetic Spectral Densities"
  },
  ...
```
among other items.

Once a server and data source are known, HTTP GET URLs can be formed to 
retrieve data from a particular resource.

The following exmaples downloads data from a Galileo mission data source 
at the University of Iowa, saves the results, and outputs a DAS2C_RESULT
structure that can be used to access the data values.

```idl
IDL> s = 'https://jupiter.physics.uiowa.edu/das/server?server=dataset' + $
IDL> '&dataset=Galileo/PWS/Survey_Electric&start_time=2001-001&end_time=2001-002'
IDL> result = das2c_readhttp(s)
IDL> result
```
You should get the output:
```
{
    "RESULT": 1,
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
The structure printed above is a DAS2C_RESULT structure.  Notice that value of the
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

DAS2C_RESULT structures are also generated by the das2c_results() function.
All stored query results can be summarized by calling das2c_results() with no
arguments.
```idl
IDL> das2c_results()
```

### Inspecting Datasets

Ask the result for dataset 0 and print some a human readable description.

```idl
IDL> ds = das2c_datasets(result, 0)
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
IDL> das2c_datasets(result)
{ 
  "RESULT":   1,
  "DSET":     0,
  "NAME":     "electric_10",
  "RANK":     2,
  "SHAPE":    [-99, -99, -99, -99, -99, -99, 4483, 152], 
  "N_PDIMS":  3,
  "N_PROPS":  10,
  "N_VALS":   686051 
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
  {"RESULT":1, "DSET":0, "PDIM":"time",      "USE":"COORD", "N_VARS": 1, "N_PROPS": 3, "N_VALS": 4483 },
  {"RESULT":1, "DSET":0, "PDIM":"frequency", "USE":"COORD", "N_VARS": 1, "N_PROPS": 4, "N_VALS": 152 },
  {"RESULT":1, "DSET":0, "PDIM":"electric",  "USE":"DATA",  "N_VARS": 1, "N_PROPS": 4, "N_VALS": 681416 }
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
   "RESULT": 1,
   "DSET":   0,
   "PDIM":   "frequency",
   "VAR":    "center", 
   "UNITS":  "Hz"
   "SHAPE":  [-3, 152],
   "TYPE":   "FLOAT",
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

Though this is straight-forward, it is not memory efficent.  Since the SHAPE field
for the frequency centers has a degenerate flag in the first index, we can spare 
ourselves alot of unnecessary memory usage if we just get a slice of frequencies
for any valid first index.  The code below illustrates a slice the frequency array
on the first time point:

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

To delete a das2c result and free it's memory use the `das2c_free` function as illustrated below:
```idl
IDL> das2c_free(result)
```
Calling `das2c_free` on a query result releases all internal data structures and memory
associated with the result; however, it will not pull the rug out from under your code.
Any internal data arrays that have been wrapped by
[IDL_ImportArray](https://www.harrisgeospatial.com/docs/CreateArrayFromExistingData.html)
will not be deleted until IDL is done with them.


## Next Steps

The [das2dlm wiki](https://github.com/das-developers/das2dlm/wiki) provides documentation for each function.  

The examples directory contains working examples of using this module to generate various plots.
