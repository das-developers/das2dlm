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

/* ************************************************************************* */
/* DAS2C_QUERY structure */

/* IDL defininiton of DAS2C_QUERY */
static IDL_STRUCT_TAG_DEF DAS2C_QUERY_tags[] = {
	{"QUERY",    0, (void*)IDL_TYP_LONG},

	{"SERVER",   0, (void*)IDL_TYP_STRING},
	{"SOURCE",   0, (void*)IDL_TYP_STRING},
	{"BEGIN",    0, (void*)IDL_TYP_STRING},
	{"END",      0, (void*)IDL_TYP_STRING},
	{"RES",      0, (void*)IDL_TYP_STRING},
	{"EXTRA",    0, (void*)IDL_TYP_STRING},

	{"N_DSETS",  0, (void*)IDL_TYP_LONG},
	{"N_VALS",   0, (void*)IDL_TYP_LONG64},
	{0}
};

/* Global struct definition pointer */
static IDL_StructDefPtr DAS2C_QUERY_pdef;

static void define_DAS2C_QUERY()
{
	DAS2C_QUERY_pdef = IDL_MakeStruct("DAS2C_QUERY", DAS2C_QUERY_tags);
}

/* C structure to use in casts for manipulating IDL struct memory */
typedef struct das2c_query_data_s{
	IDL_LONG   query;
	
	IDL_STRING server;
	IDL_STRING source;
	IDL_STRING begin;
	IDL_STRING end;
	IDL_STRING res;
	IDL_STRING extra;

	IDL_LONG   n_dsets;
	IDL_LONG64 n_vals;
} DAS2C_QUERY_data;


/* Formating a query struct using an entry object pointer */
static bool das2c_ent2query(DAS2C_QUERY_data* pDest, const QueryDbEnt* pSrc)
{		
	pDest->query = pSrc->nQueryId;
	pDest->n_dsets = (IDL_LONG) pSrc->uDs;

	char sBuf[512] = {'\0'};
	if(!das2c_db_getPath(pSrc, sBuf, 511)) return false;
	IDL_StrStore(&(pDest->server), sBuf);
	
	/* Loop over the params, pulling out ones of interest */
	size_t u = 0;
	for(u = 0; u < pSrc->uParam; ++u){
				
		/* WARNING: Knowledge of das2 server interface used here
		   may need updates for das2.3 compatability */
		if(strcmp(pSrc->psKey[u], "dataset") == 0){
			if(pSrc->psVal[u] != NULL) 
				IDL_StrStore(&(pDest->source), pSrc->psVal[u]);
		}
		if(strcmp(pSrc->psKey[u], "start_time") == 0){
			if(pSrc->psVal[u] != NULL) 
				IDL_StrStore(&(pDest->begin), pSrc->psVal[u]);
		}
		if(strcmp(pSrc->psKey[u], "end_time") == 0){
			if(pSrc->psVal[u] != NULL) 
				IDL_StrStore(&(pDest->end), pSrc->psVal[u]);
		}
		if(strcmp(pSrc->psKey[u], "resolution") == 0){
			if(pSrc->psVal[u] != NULL) 
				IDL_StrStore(&(pDest->res), pSrc->psVal[u]);
		}
		if(strcmp(pSrc->psKey[u], "interval") == 0){
			if(pSrc->psVal[u] != NULL) 
				IDL_StrStore(&(pDest->res), pSrc->psVal[u]);
		}
		if(strcmp(pSrc->psKey[u], "params") == 0){
			if(pSrc->psVal[u] != NULL) 
				IDL_StrStore(&(pDest->extra), pSrc->psVal[u]);
		}
	}
			
	/* Now compute the total number of values in all arrays in
	   in all datasets */
	size_t a = 0;
	DasDs* pDs = NULL;
	DasAry* pAry = NULL;
	
	pDest->n_vals = 0;
	for(u = 0; u < pSrc->uDs; ++u){
		pDs = pSrc->lDs[u];
		for(a = 0; a < pDs->uArrays; ++a){
			pAry = pDs->lArrays[a];
			pDest->n_vals += DasAry_size(pAry);
		}
	}	
	return true;
}

/* Get a DB entry from a query struct arg */
static const QueryDbEnt* das2c_arg_to_ent(int argc, IDL_VPTR* argv, int iArg)
{	
	if(argc <= iArg) das2c_IdlMsgExit("Expecting a struture for argument %d", iArg+1);
	
	/* See if this is a query struct */
	IDL_VPTR pVar = argv[iArg];
	
	if(pVar->type != IDL_TYP_STRUCT)
		das2c_IdlMsgExit("Argument %d is not a structure", iArg);	
	
	/* Get the query ID.  Use duck-typing here.  If there is a
	   field named "QUERY" and it can be converted to a LONG
		then good enough.  Consider it a DAS2C_QUERY structure */
	
	IDL_VPTR pFakeVar = NULL;
	IDL_MEMINT nOffset = IDL_StructTagInfoByName(
		pVar->value.s.sdef, "QUERY", IDL_MSG_LONGJMP, &pFakeVar
	);
	
	/* Get pointer to query field in first element of the structure array */
	/* Skipping the (i * elt_len) clause since i = 0 for first row */
	UCHAR* pData = pVar->value.s.arr->data + nOffset;
	
	/* Get the query ID value, accept a few different types */
	int32_t iQueryId = 0;  /* 0 is always a bad query ID */
	switch(pFakeVar->type){
	case IDL_TYP_INT:  iQueryId = *((IDL_INT*)pData);  break;
	case IDL_TYP_UINT: iQueryId = *((IDL_UINT*)pData); break;
	case IDL_TYP_LONG: iQueryId = *((IDL_LONG*)pData); break;
	/* other types have range larger than uint32_t */
	default:
		das2c_IdlMsgExit(" 'QUERY' value should be an INT, UINT or LONG");
		break;
	}

	const QueryDbEnt* pEnt = das2c_db_getent(iQueryId);
	if(pEnt == NULL) das2c_IdlMsgExit("No query result has ID %d", iQueryId);
	return pEnt;
}

/* ************************************************************************* */
/* API Function, careful with changes! */

#define D2C_QUERIES_MINA 0
#define D2C_QUERIES_MAXA 1
#define D2C_QUERIES_FLAG 0

/*
;+
; FUNCTION:
;  das2c_queries
;
; PURPOSE:
;  List stored das2 query results
;
; CALLING SEQUENCE:
;  Result = das2c_queries()
;  Result = das2c_queries(query_id)
;
; OPTIONAL INPUTS:
;  query_id: Output information on only a single stored query instead of
;            all that are loaded.
;
; OUTPUT:
;  This function returns an array of structures providing an overview of
;  each stored result.  Output structures have the fields:
;
;    QUERY:    Long     ; The ID number of the query that producted this
;                       ; dataest, starts from 1
;
;    N_DSETS:  Long     ; The number of datasets returned by the query
;
;    N_VALS:   Long64   ; The total number of actual data values in the
;                       ; dataset.  Due to virtual variables this may be less
;                       ; sum of the shapes of all variables.
;
; ADDITIONAL OUTPUT:
;  If the query was to a das2 server the following structure memebers 
;  will also be present:
;
;    SERVER:   String   ; The network URL from which data were loaded
;    SOURCE:   String   ; The data source ID string on the server, if known
;    BEGIN:    String   ; The min time value
;    END:      String   ; the max time value
;    RES:      String   ; The resolution requested if any
;    EXTRA:    String   ; Any extra arguments sent to the server
;
; EXAMPLES:
;  List summary information on all stored results
;    das2c_queries()
;
;  List summary information given a single stored result ID.
;    das2c_queries(23);
;
; MODIFICATION HISTORY:
;  Written by: Chris Piker, 2020-03-11
;-
*/
static IDL_VPTR das2c_api_queries(int argc, IDL_VPTR* argv)
{
	/* If no queries are stored, return the !NULL variable */
	if(g_nDbStored == 0) return IDL_GettmpNULL();
	
	IDL_LONG iQueryId = 0;
	IDL_VPTR pTmpVar = NULL;
	if(argc > 0){
		/* They only want to ask about a specific query */
		
		/* the idl function calls ensure simple in the background so we know that
 		   we don't have an array variable, though handling that should probably
		   be done at some point. */
		pTmpVar = IDL_BasicTypeConversion(1, argv, IDL_TYP_LONG);
		iQueryId = pTmpVar->value.l;
		
		/* Always use the DELTMP macro wrapper instead of blindly calling 
		   IDL_Deltmp() on variables */
		IDL_DELTMP(pTmpVar);
	}
	
	size_t u;
	int nFound = 0;     /* used to cut off the search early */
	for(u = 0; u < g_nLastQueryId && nFound < g_nDbStored; ++u){		
		if(g_pDasIdlDb[u] == NULL) continue;
		
		if(iQueryId == 0){ ++nFound; }
		else{
			if(g_pDasIdlDb[u]->nQueryId == iQueryId){
				++nFound;
				break;
			}
		}
	}
	
	if(nFound == 0) return IDL_GettmpNULL();
		
	IDL_MEMINT dims = nFound;
	IDL_VPTR pRet;
	
	/* Returns pRet->value.s.arr.data */
	DAS2C_QUERY_data* pData = (DAS2C_QUERY_data*) IDL_MakeTempStruct(
		DAS2C_QUERY_pdef, 1, /* ary dims */ &dims,  /* Sz of each dim */
		&pRet,  /* Actual idl varabile */  TRUE    /* Zero out the array */
	);
	
	/* Now fill in the values.  Pray that some other thread hasn't
	   screwed up the query database */
	nFound = 0;
	
	QueryDbEnt* pEnt = NULL;
	for(u = 0; u < g_nLastQueryId && nFound < g_nDbStored; ++u){		
		if(g_pDasIdlDb[u] == NULL) continue;
	
				
		if((iQueryId == 0)||(pEnt->nQueryId == iQueryId)){
			pEnt = g_pDasIdlDb[u];
			das2c_ent2query(pData, pEnt);	
					
			++pData;
			++nFound;
			if(iQueryId != 0) break;
		}
	}
	
	return pRet;
}
