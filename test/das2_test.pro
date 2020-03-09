; Testing the das2cdlm
; First make sure you have added the directory containing the das2c.dlm to your
; IDL_DLM_PATH.  For example:
;
; $ IDL_DLM_PATH="${HOME}/git/das2c/dlm:<IDL_DEFAULT>"
; $ export IDL_DLM_PATH
; $ idl

; Query for Galileo PWS electric data at 1 minute resolution
sSrv = 'http://planet.physics.uiowa.edu/das/das2Server'   ; Requires a URL
sDs  = 'Galileo/PWS/Survey_Electric'                      ; Required param
sBeg = '2001-001'                                         ; Required param
sEnd = '2001-002'                                         ; Required param
sRes = '60'   ; Optional, if not specified intrinsic time resolution returned
sExtra = ''   ; Optional, some data sources define and use extra arguments

; Get data
sFmt = '%s?server=dataset&dataset=%s&start_time=%s&end_time=%s'
sUrl = string(sSrv, sDs, sBeg, sEnd, format=sFmt)

if strlen(sRes) gt 0 then sUrl += '&resolution=' + sRes
if strlen(sExtra) gt 0 then sUrl += '&params=' + sExtra

; Ask for data.  Advertise for spedas via the user agent string
id = das2c_readhttp(sUrl, agent="spedas/3.20")

das2c_queries()
{'id': 357, 'datasets': 1, 'server': , 'source': , 'begin': , 'end': , 'res': , 'extra': , 'size': }

das2c_datasets(357)
{'idx': 0, 'name':'dataset0', 'physdims':3}
{'idx': 1, 'name':'dataset1', 'physdims':3}

das2c_physdims(357, 0)
{'index': 0, 'name':'time', 'n_props':4, 'n_vars':1}

das2c_vars(357, 0, 'time')


das2c_props(357, 0)
{'name': 'title', 'dim':'time', 'type':'String', 'Value':'Galileo PWS - LRS Electric'}



nResults = das2c_len()
nDs      = das2c_len(357)
nDims    = das2c_len(357, 0)
nVars    = das2c_len(357, 0, 'time')

das2c_dataset_info(nRes, iDs)
'Dataset: "electric_10" from group "electric" | i:0..4483, j:0..152'
'   Property: String | title | Galileo PWS - LRS Electric
'
'   Data Dimension: electric
'      Property: String | label | 
'      Property: String | scaleType | log
'      Property: DatumRange | range | 1.0e-17 to 1.0e-4 V**2 m**-2 Hz**-1
'      Property: double | fill | 0.0
'   
'      Variable: center | electric[i][j] V**2 m**-2 Hz**-1 | i:0..4483, j:0..152
'   
'   Coordinate Dimension: time
'      Property: Datum | tagWidth | 80 s
'      Property: DatumRange | cacheRange | 2001-01-01T00:00:00.000 to 2001-01-02T00:00:00.000 UTC
'      Property: String | label | %{RANGE}
'   
'      Variable: center | time[i] us2000 | i:0..4483, j:-
'   
'   Coordinate Dimension: frequency
'      Property: String | label | Frequency (Hz)
'      Property: String | scaleType | log
'      Property: DatumRange | range | 5 to 6000000 Hz
'      Property: Datum | tagWidth | 1.4 log10Ratio
'   
'      Variable: center | frequency[j] Hz | i:-, j:0..152


 das2c_physdim_names(nRes, iDs)
'time'
'frequency'
'amplitude'

das2c_prop_names(nRes, iDs, 'time')
'tagWidth'
'cacheRange'
'label'

das2c_var_names(nRes, iDs)
'center'

das2c_var_data(nRes, iDs, 'time', 'center')
[ 0 , 2 ,3 ,4 ...  ]

das2c_var_units(nRes, iDs, 'time','center')
'Hz'


 das2c_prop(nId, iDs, 'time', 'tagWidth')
{ 'type':'Datum', "value": 80, 'units':'s'}

das2c_prop(nId, iDs, 'title')
{ 'type': 'String', 'value':'Galileo PWS - LRS Electric'}

das2c_free(nRes)
