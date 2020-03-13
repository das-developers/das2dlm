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
;  das2c_pdims
;
; PURPOSE:
;  List the physical dimensions represented in a das2 dataset.
;
; CALLING SEQUENCE:
;  Result = das2c_pdims(dataset)
;  Result = das2c_pdims(dataset, pdim)		
;
; INPUTS:
;  dataset:  A DAS2C_DSET structure as return by das2c_datasets()
;
; OPTIONAL INPUTS:
;  pdim:     The dimension's name (string).  This input is only required if
;            information concerting a single physical dimension is desired.  
;            	
;            Dimension names are somewhat standardized.  For example time tags
;            are usually in dimension 'time' and energy bin centers are
;            typically in a dimension named 'energy'.  Application code
;            shouldn't depend on this fact but it may use the standardized
;            names as plotting hints.
;
; OUTPUT:
;  This function returns an array of structures providing an overview of
;  each physical dimension represented in the dataset. Output structures
;  have the following fields:
;
;    'QUERY':    Long    ; The query ID of this dataset, starts from 1
;
;    'DSET':     Long    ; The ID number of this dataset, starts from 0
;
;    'PDIM':     String  ; The name of this physical dimension, ex: 'time'
;
;    'USE':      String  ; The intendend usage of values from this pdim,
;                        ; will be one of two strings, 'COORD' or 'DATA'
;
;    'N_VARS':   Long    ; The number of variables carring data values for this
;                        ; physical dimension.
;    'N_PROPS':  Long    ; The number of metadata properties for this dimension
;
;    'N_VALS':   Long64  ; The total number of values in the variables for 
;                        ; this dimension
;
;   If a particular dimension is requested by name and it doesn't exist in
;   the given dataset, !NULL is returned.
;
; EXAMPLES:
;  List summary information on all physical dimensions for dataset 0 in
;  query result 27:
;    query = das2c_query(27)
;    ds = das2c_datasets(query, 0)
;    das2c_pdims(ds)
;
;  List summary information on the time dimension in the same dataset.
;    das2c_pdims(ds, 'time')
;
; MODIFICATION HISTORY:
;  Written by: Chris Piker, 2020-03-09
;-
*/

/* Output structure definition */
static IDL_STRUCT_TAG_DEF DAS2C_PDIM_tags[] = {
	{"QUERY",    0, (void*)IDL_TYP_LONG},
	{"DSET",     0, (void*)IDL_TYP_LONG},
	{"PDIM",     0, (void*)IDL_TYP_STRING},

	{"USE",      0, (void*)IDL_TYP_STRING},

	{"N_VARS",   0, (void*)IDL_TYP_LONG},
	{"N_PROPS",  0, (void*)IDL_TYP_LONG},
	{"N_VALS",   0, (void*)IDL_TYP_LONG64},
	{0}
};	

typedef struct _das2c_pdim_data_s{
	IDL_LONG   query;
	IDL_LONG   dset;	
	IDL_STRING pdim;
	
	IDL_STRING use;
	
	IDL_LONG   n_vars;
	IDL_LONG   n_props;
	IDL_LONG64 n_vals;
	
} DAS2C_PDIM_data;

static IDL_StructDefPtr DAS2C_PDIM_pdef;

static void define_DAS2C_PDIM()
{
	DAS2C_PDIM_pdef = IDL_MakeStruct("DAS2C_PDIM", DAS2C_PDIM_tags);
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
	const QueryDbEnt* pEnt, int iDs, int* iDim, char* sDim, size_t uLen
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

#define D2C_PDIMS_MINA 1
#define D2C_PDIMS_MAXA 2
#define D2C_PDIMS_FLAG 0

static IDL_VPTR das2c_api_pdims(int argc, IDL_VPTR* argv)
{
	/* Get dataset, entry number and dataset number */
	int iQuery = -1;
	int iDs = -1;
	const DasDs* pDs = das2c_arg_to_ds(argc, argv, 0, &iQuery, &iDs);
	const DasDim* pTheDim = NULL;
		
	const char* sDim = NULL;
	if(argc > 1){
		if(argv[1]->type != IDL_TYP_STRING)
			das2c_IdlMsgExit("Dimension name string expected for argument 2");
		
		sDim = IDL_VarGetString(argv[1]);
		if(*sDim == '\0') das2c_IdlMsgExit("Dimension name is empty");
		
		pTheDim = DasDs_getDimById(pDs, sDim);
		if(pTheDim == NULL)
			return IDL_GettmpNULL();
	}
	
	IDL_MEMINT dims = 1;
	if(pTheDim == NULL) dims = pDs->uDims;
	
	IDL_VPTR pRet;  /* the to-be-returned structure */
	
	/* Returns pRet->value.s.arr.data */
	DAS2C_PDIM_data* pData = (DAS2C_PDIM_data*) IDL_MakeTempStruct(
		DAS2C_PDIM_pdef,   /* The opaque structure definition */
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
		if(pIterDim == NULL) das2c_IdlMsgExit("Logic error, das2c_pdims");
		
		/* If we have a constant dim to report on and this ain't it, skip */
		if((pTheDim != NULL)&&(pIterDim != pTheDim)) continue;

		/* Write into IDL memory using parallel structure pointer */
		
		/* TODO:  Consider adding a DasDim_getPointVar() call to flatten
		          waveform data. */
		pData->query    = iQuery;
		pData->dset     = iDs;
		
		if(DasDim_id(pIterDim) == NULL)
			das2c_IdlMsgExit("Logic error, das2c_pdims");
		
		IDL_StrStore(&(pData->pdim), DasDim_id(pIterDim));
		
		if(pIterDim->dtype == DASDIM_COORD) 
			IDL_StrStore(&(pData->use), "COORD");
		else
			IDL_StrStore(&(pData->use), "DATA");
			
		pData->n_vars     = pIterDim->uVars;
		pData->n_props    = DasDesc_length((DasDesc*)pIterDim);
		
		pData->n_vals = 0;
		for(uVar = 0; uVar < pIterDim->uVars; ++uVar){
			pVar = pIterDim->aVars[uVar];
			
			/* see if this variable is backed by an array */
			if( (pAry = DasVarAry_getArray((DasVar*)pVar)) != NULL)
				pData->n_vals += DasAry_size(pAry);
		}
		
		if(pTheDim != NULL) break;
		++pData;
	}
	
	return pRet;
}

