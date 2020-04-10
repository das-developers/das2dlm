#das2c_readhttp

 This function sends an HTTP GET query string to a das2 server, parses the
 resulting data stream and stores the values and metadata.

##CALLING SEQUENCE
```idl
 Result = das2c_readhttp(Url, Agent)
```

##INPUTS
 Url: A string containing a full HTTP GET Url.  This looks like like it
		would in your web browser's address bar.  In fact you can test the
		Url just by pasting it into a browser window.

##OPTIONAL INPUTS
 Agent: Set your prefered user-agent string.  User-agent string are
   	typically saved in web-logs.  If parameter is not specified, then
		'das2dlm/VERSION' is sent as the user agent.

##OUTPUT
 This function returns a structure of type DAS2C_QUERY if it succeeds, or
 !NULL on failure.  The query structure is small.  It does not contain the
 actual data but may be used to get data and metadata on the query result.
 See das2c_queries() for a description of the DAC2_QUERY structure fields.

##SIDE EFFECTS
 Output data from the query are loaded into RAM.  Use the function das2c_free
 to release the query results after you are done with them.

##EXAMPLE

 Load 60 second resolution Galileo PWS low-frequency electric field spectra
 collected from noon Jan. 1st 2001 to 2:35 pm Jan 2nd 2001 UTC.
```idl
	sSrv = 'http://planet.physics.uiowa.edu/das/das2Server'
	sDs  = 'Galileo/PWS/Survey_Electric'
	sBeg = '2001-001T12:00'
	sEnd = '2001-002T14:35'
	sRes = '60'
	sFmt = '%s?server=dataset&dataset=%s&start_time=%s&end_time=%s&resolution=%s'
	sUrl = string(sSrv, sDs, sBeg, sEnd, sRes, format=sFmt)

	query = das2c_readhttp(sUrl, "spedas/3.20")
```
 Now get information about the query:
 
```idl
	das2c_datasets(query)
	das2c_info(query, 0)
```
MODIFICATION HISTORY:
 Written by: Chris Piker, 2020-03-11
