/* Copyright (C) 2020 Chris Piker <chris-piker@uiowa.edu>
 *                    
 * This file is part of das2dlm, an Interactive Data Language (IDL) binding
 * for the das2C library.  IDL is a trademark of Harris Geospatial Solutions,
 * Inc.  The shared object generated by this code may be loaded by main 
 * programs, or other shared objects, even those with closed source licenses.
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


/* Purpose of this code:
   IDL forces DLM end users to look at data through a straw.  Complex objects
	can't just be "thrown over the fence" in the manner of Python C extensions.
	Thus we'll need to setup an internal table of query results and provide
	result inspection functions.
	
	I always wondered why the IDL CDF DLM was so clunky.  Now I know.  It's due
	to IDLs limitations. -cwp
*/

#define D2C_ENT_HTTP_QUERY 0xA
#define D2C_ENT_HTTPS_QUERY 0xB

/* ************************************************************************* */
/* DB entry "class" */

typedef struct query_db_ent{
	int nQueryId;   /* The query ID.  0 is never a legal ID */
	DasDs** lDs;    /* Pointer to the resulting datasets */
	size_t  uDs;    /* Number of resulting datasets */
	
	/* Query info */
	time_t  nTime;  /* The time at which the query was performed, seconds since
	                 * 1970 (unix epoch) */
	int     nType;  /* The query type, only HTTP & HTTPS allowed right now */
	char*   sHost;  /* The host that was queried */
	char*   sPort;  /* The port that was queried */	
	char*   sPath;  /* The path that was queried */
	
	size_t  uParam; /* The number of query parameters */
	char**  psKey;  /* The query param pointers  */
	char**  psVal;  /* The query value pointers  */
	char*   sQuery; /* The buffer containing the query */	
} QueryDbEnt;

/* ************************************************************************* */
/* Initalize the db (only call this once) */

/* TODO: Make thread safe! */
static QueryDbEnt** g_pDasIdlDb;      /* Pointer to result array */
static size_t       g_uDasIdlDbSz;    /* Size of the results storage array */
static int          g_nDbStored;      /* Number of results stored */
static int          g_nLastQueryId;   /* Last query ID used       */

static bool das2c_db_init(size_t uLen){
	
	g_nLastQueryId = 0;
	g_nDbStored = 0;
	
	g_uDasIdlDbSz = uLen;
	g_pDasIdlDb = (QueryDbEnt**) calloc(g_uDasIdlDbSz, sizeof(char*));
	if(g_pDasIdlDb == NULL){
		das_error(DLMERR, "Could not allocate internal DB memory");
		return false;
	}
	return true;
};

/* ************************************************************************* */
/* Free a DB entry */

void das2c_free_ent(QueryDbEnt* pEnt)
{
	
   if(pEnt == NULL) return;

   if((pEnt->lDs != NULL)&&(pEnt->uDs > 0)){
      for(size_t u = 0; u < pEnt->uDs; ++u)
         del_DasDs(pEnt->lDs[u]);
   }

   if(pEnt->sHost != NULL) free(pEnt->sHost);
   if(pEnt->sPort != NULL) free(pEnt->sPort);
   if(pEnt->sPath != NULL) free(pEnt->sPath);
	
	/* These contain internal pointers, free the top level array but NOT what
	   the elements point to! */
   if(pEnt->psKey != NULL) free(pEnt->psKey);
   if(pEnt->psVal != NULL) free(pEnt->psVal);
   
	if(pEnt->sQuery != NULL) free(pEnt->sQuery);

   free(pEnt);
}

/* ************************************************************************* */
/* Return the first open entry.  If no open entries reallocate buf. 
 * The top level array is just an array of pointers so that pointers
 * to individual ents aren't invalidated during a realloc proceedure.
 * 
 * Returns a blank DB entry structure, only the nQueryID field is valid.
 */
static QueryDbEnt* das2c_db_newEnt()
{
	size_t u = 0;
	for(u = 0; u < g_uDasIdlDbSz; ++u){
		if(g_pDasIdlDb[u] == NULL){
			g_pDasIdlDb[u] = (QueryDbEnt*) calloc(1, sizeof(QueryDbEnt));
			if(g_pDasIdlDb[u] == NULL){
				das_error(DLMERR, "Could not allocate new DB entry");
				return NULL;
			}
			else{ 
				g_nLastQueryId += 1;  /* 0 is never a valid ID */
				g_nDbStored += 1;
				
				g_pDasIdlDb[u]->nQueryId = g_nLastQueryId;
				return g_pDasIdlDb[u];
			}
		}
	}
	/* Okay no room in the inn */
	QueryDbEnt** pTmp = (QueryDbEnt**) realloc(g_pDasIdlDb, g_uDasIdlDbSz*2);
	if(pTmp == NULL){
		das_error(DLMERR, "Could not re-allocate new DB entry");
		return NULL;
	}
	
	g_pDasIdlDb = pTmp;
	/* Assumes NULL = 0, not strictly in the C99 standard */
	memset(g_pDasIdlDb + g_uDasIdlDbSz, 0, g_uDasIdlDbSz*sizeof(char*));
	
	u = g_uDasIdlDbSz;
	g_uDasIdlDbSz *= 2;
	
	/* Now try to make an entry */
	g_pDasIdlDb[u] = (QueryDbEnt*) calloc(1, sizeof(QueryDbEnt));
	
	if(g_pDasIdlDb[u] == NULL){
		das_error(DLMERR, "Could not allocate new DB entry");
		return NULL;
	}
	
	g_nLastQueryId += 1;
	g_nDbStored += 1;
	
	g_pDasIdlDb[u]->nQueryId = g_nLastQueryId;
	return g_pDasIdlDb[u];
}

/* ************************************************************************* */
/* Release a DB entry */

/* Return true if the entry existed, false otherwise */
static bool das2c_db_free_ent(int nQueryId)
{
	if((g_pDasIdlDb == NULL)||(g_uDasIdlDbSz == 0)) return false;
	
	size_t u = 0;
	int nFound = 0;
	QueryDbEnt* pEnt = NULL;
	
	for(u = 0; u < g_nLastQueryId && nFound < g_nDbStored; ++u){
		if(g_pDasIdlDb[u] == NULL) continue;
		pEnt = g_pDasIdlDb[u];
		
		if(pEnt->nQueryId == nQueryId){
			das2c_free_ent(pEnt);
			g_pDasIdlDb[u] = NULL;
			g_nDbStored -= 1;
			break;
		}
		
	}
	
	return true;
}

/* ************************************************************************* */
static const QueryDbEnt* das2c_db_getent(int nQueryId)
{
	size_t u;
	const QueryDbEnt* pEnt = NULL;
	
	int nFound = 0;
	for(u = 0; u < g_nLastQueryId && nFound < g_nDbStored; ++u){		
		if(g_pDasIdlDb[u] == NULL) continue;
		pEnt = g_pDasIdlDb[u];
		
		if((nQueryId != 0) && (pEnt->nQueryId == nQueryId))
			return pEnt;
		++nFound;
	}
	
	return NULL;
}

/* ************************************************************************* */
/* Add an dataset response to the db, IF it contans any data.               
 *
 * Returns: The new entry, or NULL if there was a problem. 
 */
static QueryDbEnt* das2c_db_addHttpEnt(
	const DasHttpResp* pRes, DasDs** lDs, size_t uDs
){
	char sBuf[1024] = {'\0'};
	if(pRes == NULL){
		das_error(DLMERR, "logic error");
		return NULL;
	}
	
	if( (lDs == NULL) || (uDs == 0)){
		daslog_debug_v(
			"Invalid response from %s dropped", das_url_toStr(&(pRes->url), sBuf, 1023)
		);
		return NULL;
	}
	
	QueryDbEnt* pEnt = das2c_db_newEnt();
	if(pEnt == NULL) 
		return NULL;
	
	pEnt->lDs = lDs;
	pEnt->uDs = uDs;
	pEnt->nTime = time(NULL);
	if(strcmp(pRes->url.sScheme, "https") == 0) pEnt->nType = D2C_ENT_HTTPS_QUERY;
	else pEnt->nType = D2C_ENT_HTTP_QUERY;
	pEnt->sHost = das_strdup(pRes->url.sHost);
	pEnt->sPort = das_strdup(pRes->url.sPort);
	pEnt->sPath = das_strdup(pRes->url.sPath);
	
	/* Yay, now for param parsing ... */
	pEnt->sQuery = das_strdup(pRes->url.sQuery);
	pEnt->uParam = 0;
	
	char* p = pEnt->sQuery;
	char c = '\0';
	
	if(pEnt->sQuery != NULL){
		/* field terminated by separators.  Seps are & and \0.  
		   Skip repeated seps */
		
		/* if first item isn't sep, you have a param */
		if((*p != '\0')&&(*p != '&')) pEnt->uParam = 1;
		
		/* Now find the rest ... */
		while(*p != '\0'){
			if(*p == '&'){ 
				/* Look ahead by 1 next item isn't a sep */
				c = *(p+1);
				if((c != '\0')&&(c != '&')) pEnt->uParam += 1;
			}
			++p;
		}
	}
	
	if(pEnt->uParam == 0) return NULL;  /* If no params, just bail */
	
	pEnt->psKey = (char**)(calloc(pEnt->uParam, sizeof(char*)));
	pEnt->psVal = (char**)(calloc(pEnt->uParam, sizeof(char*)));
		
	/* Go through the param array nulling out '&' and '=' and
	   saving pointers as needed */
	
	size_t uKey = 0;
	pEnt->psKey[0] = pEnt->sQuery;
	p = pEnt->psKey[0];
	
	while(*p != '\0'){
		
		/* Null all '&' chars and look to see if can start the next key */
		if(*p == '&'){
			*p = '\0';
			if( (*(p+1) != '\0') && (*(p+1) != '&')){ 
				++uKey;
				if(uKey >= pEnt->uParam){
					das_error(DLMERR, "Logic error");
					das2c_db_free_ent(pEnt->nQueryId);
					return NULL;
				}
				pEnt->psKey[uKey] = p+1;
			}
		}
		++p;
	}
	
	/* Now go trough all the keys and null out the first '=' */
	size_t u;
	for(u = 0; u <= uKey; ++u){
		
		p = pEnt->psKey[u];
		while(*p != '\0'){
			if(*p == '='){
				*p = '\0';
				if(*(p+1) != '\0') pEnt->psVal[u] = p+1;
				break;
			}
			++p;
		}
	}
	
	return pEnt;	
}

/* ************************************************************************* */

bool das2c_db_getPath(const QueryDbEnt* pEnt, char* sBuf, size_t uSz)
{
	size_t uLen = 0;
	size_t uWrote = 0;
	bool bAddPort = false;
	
	memset(sBuf, 0, uSz);
	
	switch(pEnt->nType){
	case D2C_ENT_HTTP_QUERY:  strncpy(sBuf, "http://", 7);   uWrote = 7; break;
	case D2C_ENT_HTTPS_QUERY:  strncpy(sBuf, "https://", 8); uWrote = 8; break;
	default:
		daslog_error_v("Unknown query type 0x%X", pEnt->nType);
		return false;
	}

	uLen = strlen(pEnt->sHost);
	if(pEnt->sHost && ((uWrote + uLen) < uSz) ){
		strncpy(sBuf + uWrote, pEnt->sHost, uLen);
		uWrote += uLen;
	}
	
	if(pEnt->sPort != NULL){
		if(pEnt->nType == D2C_ENT_HTTP_QUERY){
			bAddPort = (strcmp(pEnt->sPort, "80") != 0);
		}
		else{
			if(pEnt->nType == D2C_ENT_HTTP_QUERY)
				bAddPort = (strcmp(pEnt->sPort, "443") != 0);
		}
	}
	if(bAddPort){
		uLen = strlen(pEnt->sPort);
		if((uWrote + uLen + 1) < uSz){
			sBuf[uWrote] = ':'; ++uWrote;
			strncpy(sBuf + uWrote, pEnt->sPort, uLen);
			uWrote += uLen;
		}
	}
	
	if(pEnt->sPath != NULL){
		uLen = strlen(pEnt->sPath);
		if((uWrote + uLen) < uSz){
			strncpy(sBuf + uWrote, pEnt->sPath, uLen);
			/* uWrote += uLen; */
		}
	}
	
	return true;
}



