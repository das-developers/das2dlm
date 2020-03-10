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
 
/*
;+
; FUNCTION:
;  das2c_physdims
;
; PURPOSE:
;  List the physical dimensions represented in a das2 dataset.
;
; CALLING SEQUENCE:
;  Result = das2c_physdims(query_id, ds_index, pdim)
;
; INPUTS:
;  query_id: The identification integer for the stored query result as
;            returned by das2c_readhttp() or das2c_queries.
;
;  ds_index: The dataset index, often 0.  See das2c_datasets() for details.
;
; OPTIONAL INPUTS:
;  pdim:     Either the physical dimension index (long), or the dimension's
;            name (string).  This input is only required if information
;            concerting a single physical dimension is desired.  
;            	
;            Dimension names are somewhat standardized.  For example time tags
;            are usually in dimension 'time' and energy bin centers are
;            typically in a dimension named 'energy'.  Application code
;            shouldn't depend on this fact but it may use the standardized
;            names as plotting hints.
;
; OUTPUT:
;  This function returns an array of structures providing an overview of
;  each stored result.  Output structures have the fields:
;
;    'id':       Long    ; The ID number of this dataset, starts from 0
;
;    'name':     String  ; The name of this physical dimension, ex: 'time'
;
;    'vars':     Long    ; The number of variables carring data values for this
;                        ; physical dimension.
;    'props':    Long    ; The number of metadata properties for this dimension
;
;    'size':     Long64  ; The total number of values in the variables for 
;                        ; this dimension
;
; EXAMPLES:
;  List summary information on all physical dimensions for dataset 0 in
;  query result 27
;    das2c_physdims(27, 0)
;
;  List summary information on the time dimension in the same dataset.
;    das2c_physdims(27, 0, 'time')
;
;  List summary information on the dimension that happens to live at index 0.
;    das2c_physdims(27, 0, 0)
;
; TODO:
;  Should this function return !NULL if a requested physical dimension dosen't
;  exist in the dataset?
;
; MODIFICATION HISTORY:
;  Written by: Chris Piker, 2020-03-09
;-
*/

/* Output structure definition */
static IDL_STRUCT_TAG_DEF _das2c_physdim_tags[] = {
	{"id",       0, (void*)IDL_TYP_LONG},
	{"name",     0, (void*)IDL_TYP_STRING},
	{"vars",     0, (void*)IDL_TYP_LONG},
	{"props",    0, (void*)IDL_TYP_LONG},
	{"size",     0, (void*)IDL_TYP_LONG64},
	{0}
};	

typedef struct _das2c_pdim_sum{
	IDL_LONG   id;
	IDL_STRING name;
	IDL_LONG   vars;
	IDL_LONG   props;
	IDL_LONG64 size;
} das2c_PdimSummary;

static IDL_StructDefPtr g_das2c_pPdimSumDef;

static void DAS2C_PHYSDIM_def()
{
	g_das2c_pPdimSumDef = IDL_MakeStruct("DAS2C_PHYSDIM", _das2c_physdim_tags);
}

/* ************************************************************************* */
/* Downstream helper for pulling out dimesions, these don't return on error  */

/* returns either a dim id, or -1 and a dim string */
static int das2c_args_dim_id(
	int argc, IDL_VPTR* argv, int iArg, char* sName, size_t uLen
){
	int iDim = -1;
	const char* sTmp = NULL;
	memset(sName, 0, uLen);
	
	if(uLen < 2) das2c_IdlMsgExit("uLen too short");
	
	if(argc <= iArg)
		das2c_IdlMsgExit(
			"Physical dimension not specified, either a string or an integer "
			"is required for argument number %d", iArg
		);
	
	/* See if this is as string */
	if(argv[iArg]->type == IDL_TYP_STRING){
		sTmp = IDL_VarGetString(argv[iArg]);
		if(*sTmp == '\0') das2c_IdlMsgExit("Dimension name is empty");
		
		strncpy(sName, sTmp, uLen-1);
		return iDim;
	}
	
	IDL_VPTR pTmpVar = IDL_BasicTypeConversion(1, argv + iArg, IDL_TYP_LONG);
	
	iDim = pTmpVar->value.l;
	IDL_DELTMP(pTmpVar);
	if(iDim < 0) das2c_IdlMsgExit("Invalid physical dimension index %d", iDim);
		
	return iDim;
}

/* if dim id or name is valid, set the missing item and return the dim ptr */
static const DasDim* das2c_check_dim_id(
	const DasIdlDbEnt* pEnt, int iDs, int* iDim, char* sDim, size_t uLen
){
	int i = 0;
	const DasDs* pDs = das2c_check_ds_id(pEnt, iDs);
	const DasDim* pDim = NULL;
	
	if((sDim == NULL)||(sDim[0] == '\0')){
		/* Lookup by index and save the name */
		if((*iDim > 0)&&(*iDim < pDs->uDims))
			pDim = pDs->lDims[*iDim];
		else
			das2c_IdlMsgExit(
				"Query result %d, dataset %d doesn't have a physical dimension"
				" with index number '%d'", pEnt->nQueryId, iDs, *iDim
			);
		
		strncpy(sDim, pDim->sId, uLen);
	}
	else{
		/* Lookup by name and save the index */
		pDim = DasDs_getDimById(pDs, sDim);
		if(pDim == NULL)
			das2c_IdlMsgExit(
				"Query result %d, dataset %d doesn't have a physical dimension"
				" named '%s'", pEnt->nQueryId, iDs, sDim
			);
		
		for(i = 0; i < pDs->uDims; ++i){
			if(pDs->lDims[i] == pDim) *iDim = i;
		}
		if(*iDim == -1) das2c_IdlMsgExit("Logic error das2c_check_dim_id");
	}
	
	return pDim;
}


/* ************************************************************************* */
/* API Function, careful with changes! */	

#define D2C_PHYSDIMS_MINA 2
#define D2C_PHYSDIMS_MAXA 3
#define D2C_PHYSDIMS_FLAG 0

static IDL_VPTR das2c_api_physdims(int argc, IDL_VPTR* argv)
{
	/* Get/check Query ID */
	int iQueryId = das2c_args_query_id(argc, argv, 0);
	const DasIdlDbEnt* pEnt = das2c_check_query_id(iQueryId);
	
	/* Get/check dataset ID */
	int iDs = das2c_args_ds_id(argc, argv, 1);
	const DasDs* pDs = das2c_check_ds_id(pEnt, iDs);
	
	/* Get the dimension in question, or assume they want to know about
	 * all of them. */
	const DasDim* pTheDim = NULL;
	int iDim = -1;
	char sDim[128] = {'\0'};
	
	if(argc > 2){
		iDim = das2c_args_dim_id(argc, argv, 2, sDim, 127);
		pTheDim = das2c_check_dim_id(pEnt, iDs, &iDim, sDim, 127);
	}
	
	IDL_MEMINT dims = 1;
	if(pTheDim == NULL) dims = pDs->uDims;
	
	IDL_VPTR pRet;  /* the to-be-returned structure */
	
	/* Returns pRet->value.s.arr.data */
	das2c_PdimSummary* pData = (das2c_PdimSummary*) IDL_MakeTempStruct(
		g_das2c_pPdimSumDef,   /* The opaque structure definition */
		1,                   /* Number of dimesions */
		&dims,               /* Size of each dimension, (only one dimension) */
		&pRet,               /* The actual structure variable */
		TRUE                 /* Zero out the array */
	);

	const DasDim* pIterDim = NULL;
	const DasVar* pVar = NULL;
	const DasAry* pAry = NULL;
	size_t uVar = 0;
	
	for(size_t u = 0; u < pDs->uDims; ++u){
		pIterDim = pDs->lDims[u];
		if(pIterDim == NULL) das2c_IdlMsgExit("Logic error, das2c_physdims");
		
		/* If we have a constant dim to report on and this ain't it, skip */
		if((pTheDim != NULL)&&(pIterDim != pTheDim)) continue;

		/* Write into IDL memory using parallel structure pointer */
		
		/* TODO:  Consider adding a DasDim_getPointVar() call to flatten
		          waveform data. */
		pData->id       = u;
		pData->vars     = pIterDim->uVars;
		pData->props    = DasDesc_length((DasDesc*)pIterDim);
		
		if(DasDim_id(pIterDim) != NULL) 
			IDL_StrStore(&(pData->name), DasDim_id(pIterDim));
		
		pData->size = 0;
		for(uVar = 0; uVar < pIterDim->uVars; ++uVar){
			pVar = pIterDim->aVars[uVar];
			
			/* see if this variable is backed by an array */
			if( (pAry = DasVarAry_getArray((DasVar*)pVar)) != NULL)
				pData->size += DasAry_size(pAry);
		}
		
		if(pTheDim != NULL) break;
		++pData;
	}
	
	return pRet;
}

