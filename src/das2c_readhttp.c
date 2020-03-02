/* Copyright (C) 2020 Chris Piker <chris-piker@uiowa.edu>
 *                    
 * This file is part of das2dlm, an Interactive Data Language (IDL) binding
 * for the das2C library.  IDL is a trademark of Harris Geospatial Solutions,
 * Inc.  The shared object generated by this code may be loaded by main 
 * programs with closed source licenses.
 *
 * das2dlm is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 2.1 as published
 * by the Free Software Foundation.
 *
 * das2dlm is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 2.1 along with libdas2; if not, see <http://www.gnu.org/licenses/>.
 */

/*
;+
; FUNCTION:
;  das2c_readhttp
;
; PURPOSE:
;  This function sends an HTTP GET query string to a das2 server, parses the
;  resulting data stream and stores the values and metadata.
;
; CALLING SEQUENCE:
;  Result = das2c_readhttp(Url, Agent)
;
; INPUTS:
;  Url: A string containing a full HTTP GET Url.  This looks like like it
;       would in your web browser's address bar.  In fact you can test the
;       Url just by pasting it into a browser window.
;
; OPTIONAL INPUTS:
;  Agent: Set your prefered user-agent string.  User-agent string are
;	     typically saved in web-logs.  If parameter is not specified, then
;       'das2dlm/VERSION' is sent as the user agent.
;
; OUTPUT:
;  This function returns a unique ID for the results of this query, or 0 if
;  the query failed.  The QueryID can be used to retrieve data arrays and
;  metadata returned from the server.
;
; SIDE EFFECTS:
;  Output data from the query are loaded into RAM.  Use the function das2c_free
;  to release the query results after you are done with them.
;
; EXAMPLE:
;  Load 60 second resolution Galileo PWS low-frequency electric field spectra
;  collected from noon Jan. 1st 2001 to 2:35 pm Jan 2nd 2001 UTC.
;
;    sSrv = 'http://planet.physics.uiowa.edu/das/das2Server'
;    sDs  = 'Galileo/PWS/Survey_Electric'
;    sBeg = '2001-001T12:00'
;    sEnd = '2001-002T14:35'
;    sRes = '60'
;    sFmt = '%s?server=dataset&dataset=%s&start_time=%s&end_time=%s&resolution=%s'
;    sUrl = string(sSrv, sDs, sBeg, sEnd, sRes, format=sFmt)
;
;    id = das2c_readhttp(sUrl, "spedas/3.20")
;
;  Now get information about the query:
;
;    das2c_dataset(id)
;    das2c_dsinfo(id, 0)
;
; MODIFICATION HISTORY:
;  Written by: Chris Piker, 2020-03-01
;
;-
*/
#define D2C_READHTTP_MINA  1
#define D2C_READHTTP_MAXA  2
#define D2C_READHTTP_FLAG  0

static IDL_VPTR das2c_readhttp(int argc, IDL_VPTR* argv)
{
	if(argc < 1)
		IDL_Message(
			IDL_M_NAMED_GENERIC, IDL_MSG_LONGJMP,"HTTP GET URL string not provided"
		);
	
	sInitialUrl = IDL_VarGetString(argv[0]);
	
	/* TODO: Allow setting the user agent via a keyword */
	const char* sUserAgent = "das2dlm/" DAS2DLM_S_VER;
	if(argv > 1)
		sUserAgent = IDL_VarGetString(argv[1]);
		
	float rConSec = DASHTTP_TO_MIN * DASHTTP_TO_MULTI;

	bool bOkay = false;
	DasHttpResp res;
	
	DasCredMngr* pAuth = NULL;

	bOkay = das_http_getBody(sInitialUrl, sUserAgent, pAuth, &res, rConSec);

	char sError[1024] = {'\0'};
	if(!bOkay){
		snprintf(
			sError, 1023, "%d, Could not get body for URL, reason: %s", 
			res.nCode, res.sError
		);
		IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_LONGJMP, sError);
		DasHttpResp_clear(&res);
	}
	
	char sUrl[512] = {'\0'};
	das_url_toStr(&(res.url), sUrl, 511);
	if(strcmp(sUrl, sInitialUrl) != 0)
		daslog_info_v("Redirected to %s", sUrl);

	DasIO* pIn;

	if(DasHttpResp_useSsl(&res))
		pIn = new_DasIO_ssl("libdas2", res.pSsl, "r");
	else
		pIn = new_DasIO_socket("libdas2", res.nSockFd, "r");

	DasDsBldr* pBldr = new_DasDsBldr();
	DasIO_addProcessor(pIn, (StreamHandler*)pBldr);
	
	/* TODO: Consider adding a second stream processor that calls back to IDL to
	   allow for progress reporting and task interruption */
	/* DasIO_addProcessor(pIn, (StreamHandler*) pIdlIface); */

	int nRet = DAS_OKAY;

	/* Could go with per-packet callbacks instead so that we can report 
	   status and give a chance to cancel the operation.  For now just 
		get it all at once. */
	nRet = DasIO_readAll(pIn);

	if(nRet != DAS_OKAY){
		/* Bounce das2 error message to IDL's error facility */
		del_DasIO(pIn);
		del_DasDsBldr(pBldr);
		DasHttpResp_clear(&res);
		das2c_error2idl();       /* Long jumps, we're done */
	}

	/* Get the datasets from the builder and release it */
	size_t uDs = 0;
	DasDs** lDs = DasDsBldr_getDataSets(pBldr, &uDs);
	DasDsBldr_release(pBldr); /* Detach datasets from builder */
	del_DasIO(pIn);           /* No longer need the IO object */
	
	DasIdlDbEnt* pEnt = das2c_db_addHttpEnt(&res, lDs, uDs);
	DasHttpResp_clear(&res);
	
	/* addHttpEnt cleans up any memory on an error */
	IDL_VPTR pRet;
	if(pEnt == NULL)  pRet = IDL_GettmpLong(0);
	else              pRet = IDL_GettmpLong(pEnt->nQueryId);
	
	return pRet;
}
