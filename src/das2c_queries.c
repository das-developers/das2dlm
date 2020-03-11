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

/* Output structure definition */
static IDL_STRUCT_TAG_DEF _das2c_result_tags[] = {
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

typedef struct _das2c_result_sum{
	IDL_LONG   query;
	IDL_STRING server;
	IDL_STRING source;
	IDL_STRING begin;
	IDL_STRING end;
	IDL_STRING res;
	IDL_STRING extra;
	IDL_LONG   n_dsets;
	IDL_LONG64 n_vals;
} das2c_ResultSummary;

static IDL_StructDefPtr g_das2c_pResultDef;

static void DAS2C_QUERY_def()
{
	g_das2c_pResultDef = IDL_MakeStruct("DAS2C_QUERY", _das2c_result_tags);
}

/* ************************************************************************* */
/* Downstream helpers for manditory query_id arg processing */
static int das2c_args_query_id(int argc, IDL_VPTR* argv, int iArg)
{
	int iQueryId;
	IDL_VPTR pTmpVar = NULL;		
	
	if(argc <= iArg) das2c_IdlMsgExit("Query ID not provided");
	pTmpVar = IDL_BasicTypeConversion(1, argv + iArg, IDL_TYP_LONG);
	iQueryId = pTmpVar->value.l;
	IDL_DELTMP(pTmpVar);
	
	return iQueryId;
}

static const DasIdlDbEnt* das2c_check_query_id(int iQueryId)
{
	const DasIdlDbEnt* pEnt = das2c_db_getent(iQueryId);
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
;    'query':    Long    ; A unique ID for this query result
;    'n_dsets':  Long    ; The number of datasets returned
;    'n_vals':   Long64  ; The total number of values in the query result		
;    'server':   String  ; The network URL from which data were loaded
;    'source':   String  ; The data source ID string on the server, if known
;    'begin':    String  ; The min time value
;    'end':      String  ; the max time value
;    'res':      String  ; The resolution requested if any
;    'extra':    String  ; Any extra arguments sent to the server
;
; EXAMPLES:
;  List summary information on all stored results
;    das2c_queries()
;
;  List summary information on a single stored result
;    das2c_queries(23);
;
; MODIFICATION HISTORY:
;  Written by: Chris Piker, 2020-03-01
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
	
	/* Returns pRet->value.s.arr.data, if _das2c_queries_ret_s and
	 * _das2c_queries_tags are correct */
	das2c_ResultSummary* pData = (das2c_ResultSummary*) IDL_MakeTempStruct(
		g_das2c_pResultDef,   /* The opaque structure definition */
		1,      /* Number of dimesions */
		&dims,  /* Size of each dimension, (only one dimension) */
		&pRet,  /* The actual structure variable */
		TRUE    /* Zero out the array */
	);
	
	/* Now fill in the values.  Pray that some other thread hasn't
	   screwed up the query database */
	nFound = 0;
	
	DasIdlDbEnt* pEnt = NULL;
	size_t v = 0;
	const DasDs* pDs = NULL;
	size_t d = 0;
	const DasAry* pAry = NULL;
	size_t a = 0;
	for(u = 0; u < g_nLastQueryId && nFound < g_nDbStored; ++u){		
		if(g_pDasIdlDb[u] == NULL) continue;
		pEnt = g_pDasIdlDb[u];
		
		if((iQueryId == 0)||(pEnt->nQueryId == iQueryId)){
			pData->query = pEnt->nQueryId;
			
			
			pData->n_dsets = (IDL_LONG) pEnt->uDs;
			
			/* TODO: Add path component */
			if(pEnt->sHost) IDL_StrStore(&(pData->server), pEnt->sHost);
			
			/* Loop over the params, pulling out ones of interest */
			for(v = 0; v < pEnt->uParam; ++v){
				
				/* WARNING: Knowledge of das2 server interface used here
				            may need updates for das2.3 compatability */
				if(strcmp(pEnt->psKey[v], "dataset") == 0){
					if(pEnt->psVal[v] != NULL) 
						IDL_StrStore(&(pData->source), pEnt->psVal[v]);
				}
				if(strcmp(pEnt->psKey[v], "start_time") == 0){
					if(pEnt->psVal[v] != NULL) 
						IDL_StrStore(&(pData->begin), pEnt->psVal[v]);
				}
				if(strcmp(pEnt->psKey[v], "end_time") == 0){
					if(pEnt->psVal[v] != NULL) 
						IDL_StrStore(&(pData->end), pEnt->psVal[v]);
				}
				if(strcmp(pEnt->psKey[v], "resolution") == 0){
					if(pEnt->psVal[v] != NULL) 
						IDL_StrStore(&(pData->res), pEnt->psVal[v]);
				}
				if(strcmp(pEnt->psKey[v], "interval") == 0){
					if(pEnt->psVal[v] != NULL) 
						IDL_StrStore(&(pData->res), pEnt->psVal[v]);
				}
				if(strcmp(pEnt->psKey[v], "params") == 0){
					if(pEnt->psVal[v] != NULL) 
						IDL_StrStore(&(pData->extra), pEnt->psVal[v]);
				}
			}
			
			/* Now compute the total number of values in all arrays in
			   in all datasets */
			pData->n_vals = 0;
			for(d = 0; d < pEnt->uDs; ++d){
				pDs = pEnt->lDs[d];
				for(a = 0; a < pDs->uArrays; ++a){
					pAry = pDs->lArrays[a];
					pData->n_vals += DasAry_size(pAry);
				}
			}
			
			++pData;
			++nFound;
			if(iQueryId != 0) break;
		}
	}
	
	return pRet;
}
