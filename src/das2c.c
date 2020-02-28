#define _POSIX_C_SOURCE 200112
#define _XOPEN_SOURCE 500

#include <pthread.h>
#include <time.h>
#include <stdio.h>

#include <das2/core.h>

#include "idl_export.h"

#define DASIDL_ERR_MSG_SZ 1024

/* Define message codes and their corresponding printf(3) format strings. 
   Note that message codes start at zero and each one is one less that the
	previous one. Codes must be monotonic and contiguous. */
static IDL_MSG_DEF msg_arr[] = {
#define M_TM_BAD 0
	{ "M_TM_BAD", "%NSomething bad happened" },

#define M_TM_GOOD -1
	{ "M_TM_GOOD", "%NSomething good happened"}
};

/* The load function fills in this message block handle with the opaque handle
   to the message block used for this module. Th other routines can then use
	it to throw errors from this block.  */

static IDL_MSG_BLOCK msg_block;


/* ************************************************************************* */
/* Provides mechanism to copy out errors to IDL, must be invoked manually.   */

#define ERROR2IDL_BUFSZ DASIDL_ERR_MSG_SZ + 256 + 64 + 64

static void das2c_error2idl()
{
	das_error_msg* pMsg = das_get_error();
	if(pMsg == NULL) return;
	
	char sErrBuf[ERROR2IDL_BUFSZ] = {'\0'};
	
	snprintf(
		sErrBuf, ERROR2IDL_BUFSZ - 1,
		"ERROR: %s\n       (Reported from %s in %s:%d)",
		pMsg->message, pMsg->sFunc, pMsg->sFile, pMsg->nLine
	);
	
	das_error_free(pMsg);
	
	/* Assume stack memory pointers are safe to pass here... */
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_LONGJMP, "%s", sErrBuf);
}


/* ************************************************************************* */
/* Bounce logs and errors to IDL                                             */

#define LOG2IDL_BUFSZ 1024
static void das2c_log2idl(int nLevel, const char* sMsg, bool bPrnTime)
{
	/* No need for thread safety in here, only called from das_log which
	   locks a mutex */
	const char* sLvl;
	char buf[32];
	char sTime[32];
#ifndef _WIN32
	struct tm bdTime;
#endif
	time_t tEpoch;
	buf[31] = '\0';
	sTime[31] = '\0';
	
	char sOutMsg[LOG2IDL_BUFSZ] = {'\0'};

	switch(nLevel){
		case DASLOG_TRACE: sLvl = "TRACE"; break;
		case DASLOG_DEBUG: sLvl = "DEBUG"; break;
		case DASLOG_INFO:  sLvl = "INFO"; break;
		case DASLOG_WARN:  sLvl = "WARNING"; break;
		case DASLOG_ERROR: sLvl = "ERROR"; break;
		case DASLOG_CRIT:  sLvl = "CRITICAL"; break;
		default:
			snprintf(buf, 31, "LEVEL %d MSG", nLevel); sLvl = buf; break;
	}

	if(bPrnTime){
		tEpoch = time(NULL);
#ifdef _WIN32
		/* Note: localtime is thread safe on win32, see note:
		 * http://sources.redhat.com/ml/pthreads-win32/2005/msg00011.html
		 */
		strftime(sTime, 31, "%Y-%m-%dT%H:%M:%S", localtime( &tEpoch));
#else
		strftime(sTime, 31, "%Y-%m-%dT%H:%M:%S", localtime_r( &tEpoch, &bdTime));
#endif

		snprintf(sOutMsg, LOG2IDL_BUFSZ - 1, "(%s, %s) ", sTime, sLvl);
	}
	else{
		snprintf(sOutMsg, LOG2IDL_BUFSZ - 1, "%s: ", sLvl);
	}
	snprintf(sOutMsg, LOG2IDL_BUFSZ - 1, "%s\n", sMsg);
	
	/* Treat all logs as info.  If the problem is bad enough we'll pop
	   an error which does get displayed even with !QUIET */
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, sOutMsg);
}


/* TODO: Add bindings to log level manipulation functions. */



/* ************************************************************************* */
/* Implementation of the readhttp IDL function */
static IDL_VPTR das2c_readhttp(int argc, IDL_VPTR* argv)
{
	/* IDL_MessageFromBlock(msg_block, M_TM_GOOD, IDL_MSG_RET); */
	/* return IDL_StrToSTRING("das2c_readhttp, doing nothing for now..."); */
	
	/* Get some test data if no args are provided */
	const char* sDefaultUrl = "http://planet.physics.uiowa.edu/das/das2Server"
	       "?server=dataset&dataset=Galileo/PWS/Survey_Electric"
	       "&start_time=2001-001&end_time=2001-002";

	char* sInitialUrl = NULL;
	if(argc < 1) sInitialUrl = das_strdup(sDefaultUrl);
	else sInitialUrl = IDL_VarGetString(argv[0]);
	
	/* TODO: Allow setting the user agent via a keyword */
	const char* sUserAgent = "SPEDAS";  
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
	DasDsBldr_release(pBldr); /* Free the correlated datasets from builder mem */

	
	/* TODO: Convert output to IDL structures (next session) 
	         For now just print info about the datasets */
	char sDsInfo[1024] = {'\0'};
	for(size_t u = 0; u < uDs; ++u){
		daslog_info_v(
			"Output not yet implemented, the following dataset was aquired:\n"
			"%s", DasDs_toStr(lDs[u], sDsInfo, 1023)
		);
		
		/* arrays have been detached by _release() above, delete them manually */
		del_DasDs(lDs[u]);
	}

	DasHttpResp_clear(&res);
	del_DasIO(pIn);
	
	/* TODO: Return actual data, not just count of datasets */
	IDL_VPTR pRet = IDL_GettmpULong(uDs);
	return pRet;
}

/* ************************************************************************* */
/* Load the module definition tables.
   These tables contain information on the functions and procedures that
   make up the das2c DLM. The information contained in these tables
   must be identical to that contained in das2c.dlm. */
int IDL_Load(void){

 	static IDL_SYSFUN_DEF2 function_addr[1] = {
		{ {das2c_readhttp}, "DAS2C_READHTTP", 0, IDL_MAXPARAMS, 0, NULL},
	};

	/*
	static IDL_SYSFUN_DEF2 procedure_addr[] = {
		{ (IDL_SYSRTN_GENERIC) testpro, "das2c_init" 0, IDL_MAX_ARRAY_DIM, 0, 0},
	};
	*/

 	/* Create a message block to hold our messages. Save its handle where the
	   other routines can access it. */
	msg_block = IDL_MessageDefineBlock("das2c",IDL_CARRAY_ELTS(msg_arr), msg_arr);
	if(msg_block == NULL) return IDL_FALSE;

	/* Initialize das2, send das2 longs to the IDL message system */
	das_init("das2c DLM", DASERR_DIS_RET, 0, DASLOG_INFO, das2c_log2idl);
	
	/* Override printing errors to stderr, save them instead */
	das_save_error(DASIDL_ERR_MSG_SZ);
	
 	/* Register our routine. The routines must be specified exactly the same as in 
	   testmodule.dlm. */
	return IDL_SysRtnAdd(function_addr, TRUE, IDL_CARRAY_ELTS(function_addr));
		
	
	/* We don't have any proceedures, just functions
	if(! IDL_SysRtnAdd(procedure_addr, FALSE, IDL_CARRAY_ELTS(procedure_addr)) )
		return FALSE;
	*/		
	return TRUE;
}
