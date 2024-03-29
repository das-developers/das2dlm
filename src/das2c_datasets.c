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
 
		
/* Output structure definitions.*/
static IDL_STRUCT_TAG_DEF DAS2C_DSET_tags[] = {
   {"RESULT",    NULL,     (void*)IDL_TYP_LONG},
	{"DSET",     NULL,      (void*)IDL_TYP_LONG},

	{"NAME",     NULL,      (void*)IDL_TYP_STRING},
	{"RANK",     NULL,      (void*)IDL_TYP_LONG},
	{"SHAPE",    g_aShape8, (void*)IDL_TYP_LONG64},

	{"N_PDIMS",  NULL,      (void*)IDL_TYP_LONG},
	{"N_PROPS",  NULL,      (void*)IDL_TYP_LONG},
	{"N_VALS",   NULL,      (void*)IDL_TYP_LONG64},
	{0}
};

typedef struct das2c_dset_data_s{
	IDL_LONG   result;
	IDL_LONG   dset;
	
	IDL_STRING name;
	IDL_LONG   rank;
	IDL_LONG64 shape[8];
	
	IDL_LONG   n_pdims;
	IDL_LONG   n_props;
	IDL_LONG64 n_vals;
} DAS2C_DSET_data;

static IDL_StructDefPtr DAS2C_DSET_pdef;

static void define_DAS2C_DSET()
{
	DAS2C_DSET_pdef = IDL_MakeStruct("DAS2C_DSET", DAS2C_DSET_tags);
}

static int das2c_args_ds_id(int argc, IDL_VPTR* argv, int iArg){
	int iDs = -1;
	if(argc <= iArg) das2c_IdlMsgExit("Dataset index not provided");
	IDL_VPTR pTmpVar = IDL_BasicTypeConversion(1, argv+iArg, IDL_TYP_LONG);
	iDs = pTmpVar->value.l;
	IDL_DELTMP(pTmpVar);
	if(iDs < 0) das2c_IdlMsgExit("Invalid dataset index %d", iDs);
	return iDs;
}

/* Returns dataset pointer if index is valid */
static const DasDs* das2c_check_ds_id(const ResultDbEnt* pEnt, int iDs)
{
	if(pEnt == NULL) das2c_IdlMsgExit("Logic error, das2c_datasets.c");
	
	if(iDs >= pEnt->uDs){
		if(pEnt->uDs == 0)
			das2c_IdlMsgExit(
				"Result ID %d doesn't contain any datasets", pEnt->nResultId
			);
		else
			das2c_IdlMsgExit(
				"Result %d dataset indices are 0 to %zu", 
				pEnt->nResultId, pEnt->uDs - 1
			);
	}
	const DasDs* pDs = pEnt->lDs[iDs];
	if(pDs == NULL) das2c_IdlMsgExit("Logic error, das2c_datasets.c");
	return pDs;
}

/* Put a Das2Ds into a DAS2C_DSET structure */
static void das2c_ds_to_dset(
	int nResultId, int iDs, DAS2C_DSET_data* pDest, const DasDs* pSrc
){
	size_t uDim = 0, uAry = 0;
	const DasAry* pAry = NULL;
	ptrdiff_t shape[DASIDX_MAX] = DASIDX_INIT_UNUSED;
	int nRank, r = 0;  
	
	/* Write into IDL memory using parallel structure pointer */
	pDest->result   = nResultId;
	pDest->dset     = iDs;
	
	if(DasDs_id(pSrc) != NULL)
		IDL_StrStore(&(pDest->name), (char*)DasDs_id(pSrc) /*strstore copies*/);
		
	/* Need to flag ragged datesets somehow */
	nRank = DasDs_shape(pSrc, shape);	
	pDest->rank    = nRank;
		
	for(r = 0; r < IDL_MAX_ARRAY_DIM; ++r){ 
		if(r < nRank) pDest->shape[(IDL_MAX_ARRAY_DIM - 1) - r] = shape[r];
		else pDest->shape[(IDL_MAX_ARRAY_DIM - 1) - r] = D2C_OVER_RANK_FLAG;
	}

	pDest->n_pdims  = DasDs_numDims(pSrc, DASDIM_COORD) + 
	                  DasDs_numDims(pSrc, DASDIM_DATA);
	
	pDest->n_props = DasDesc_length((DasDesc*)pSrc);   /* Global props */
	for(uDim = 0; uDim < pSrc->uDims; ++uDim)          /* dim props */
		pDest->n_props += DasDesc_length( (DasDesc*)(pSrc->lDims[uDim]) );
			
	pDest->n_vals = 0;
	for(uAry = 0; uAry < pSrc->uArrays; ++uAry){
		pAry = pSrc->lArrays[uAry];
		pDest->n_vals += DasAry_size(pAry);
	}
}

/* Get a DB entry from a query struct arg */
static DasDs* das2c_arg_to_ds(
	int argc, IDL_VPTR* argv, int iArg, int* piResult, int* piDset
){	
	const ResultDbEnt* pEnt = das2c_arg_to_ent(argc, argv, iArg);
	if(piResult != NULL) *piResult = pEnt->nResultId;
	
	/* See if this is a query struct */
	IDL_VPTR pVar = argv[iArg];
	
	if(pVar->type != IDL_TYP_STRUCT)
		das2c_IdlMsgExit("Argument %d is not a structure", iArg+1);	
	
	/* Get the DSET ID.  Use duck-typing here.  If there is a
	   field named "DSET" and it can be converted to a LONG
		then good enough.  Consider it a DAS2C_DSET structure */
	
	IDL_VPTR pFakeVar = NULL;
	IDL_MEMINT nOffset = IDL_StructTagInfoByName(
		pVar->value.s.sdef, "DSET", IDL_MSG_LONGJMP, &pFakeVar
	);
	
	/* Get pointer to query field in first element of the structure array */
	/* Skipping the (i * elt_len) clause since i = 0 for first row */
	UCHAR* pData = pVar->value.s.arr->data + nOffset;
	
	/* Get the query ID value, accept a few different types */
	int32_t iDs = -1;  /* -1 is always a bad dataset ID */
	switch(pFakeVar->type){
	case IDL_TYP_INT:  iDs = *((IDL_INT*)pData);  break;
	case IDL_TYP_UINT: iDs = *((IDL_UINT*)pData); break;
	case IDL_TYP_LONG: iDs = *((IDL_LONG*)pData); break;
	/* other types have range larger than uint32_t */
	default:
		das2c_IdlMsgExit(" 'DSET' value should be an INT, UINT or LONG");
		break;
	}

	if((iDs < 0)||(iDs > pEnt->uDs)||(pEnt->lDs[iDs]==NULL))
		das2c_IdlMsgExit(
			"No dataset at index %d for query result %d", iDs, pEnt->nResultId
		);
	
	if(piDset != NULL) *piDset = iDs;
	return pEnt->lDs[iDs];
}

/* ************************************************************************* */
/* API Function, careful with changes! */

#define D2C_DATASETS_MINA 1
#define D2C_DATASETS_MAXA 2
#define D2C_DATASETS_FLAG 0

/*
;+
; FUNCTION:
;  das2c_datasets
;
; PURPOSE:
;  List stored datasets in a das2 query result
;
; CALLING SEQUENCE:
;  Result = das2c_datasets(result)
;  Result = das2c_datasets(result, ds_index)
;
; INPUTS:
;  query: A query structure as returned by das2c_readhttp() or das2c_queries()
;
; OPTIONAL INPUTS:
;  ds_index: The dataset index. Only required if information about a single
;            dataset is desired. Most query results only contain a single
;            dataset at index 0. However, some common ones such as the
;            Juno Waves Survey data source return more that one.
;
; OUTPUT:
;  This function returns an array of structures providing an overview of
;  each stored result.  Output structures have the fields:
;
;    QUERY:    Long     ; The ID number of the query that producted this
;                       ; dataest, starts from 1
;
;    DSET:     Long     ; The ID number of this dataset, starts from 0
;
;    NAME:     String   ; The name of this dataset, dataset names are not
;                       ; usually unique.  This limits thier utility.
;
;    RANK:     Long     ; The number of entries in 'shape' that matter.  
;                       ; Since IDL uses a 'first index fastest' memory layout
;                       ; only the last 'rank' values of 'shape' actually
;                       ; matter.
;
;    SHAPE:    8 Long64 ; The extents of this dataset in the full IDL index
;                       ; space.  
;                       ;
;                       ; Ideally this array would be of length 'rank', 
;                       ; however due to limitations on DLMs, this array is
;                       ; always 8 elements long. An example of trimming
;                       ; the shape array is provided in EXAMPLES below.
;
;    N_PDIMS:  Long     ; The number of physical dimensions in the dataset
;
;    N_PROPS:  Long     ; The number of metadata properties in the dataset
;
;    N_VALS:   Long64   ; The total number of actual data values in the
;                       ; dataset.  Due to virtual variables this may be less
;                       ; sum of the shapes of all variables.
;
; EXAMPLES:
;  Get an array of dataset strutures for datasets in a HTTP GET query.
;    result = das2c_readhttp(url_string)
;    ds_arr = das2c_datasets(result)
;
;  Get the first dataset struture for result ID 27.
;    result = das2c_queries(27)
;    ds = das2c_datasets(result, 0);
;
;  Trim the shape array down to the actual rank of the dataset.
;    ds = das2c_datesets(result, 0);
;    shape = ds.shape[8 - ds.rank : -1 ]
;
; MODIFICATION HISTORY:
;  C. Piker, 2020-03-10 - Initial
;-
*/
static IDL_VPTR das2c_api_datasets(int argc, IDL_VPTR* argv)
{
	/* Get DB entry */
	const ResultDbEnt* pEnt = das2c_arg_to_ent(argc, argv, 0);
	
	int iDs = -1; /* Flag for all dims */
	
	if(argc > 1){
		iDs = das2c_args_ds_id(argc, argv, 1);
		das2c_check_ds_id(pEnt, iDs);
	}
		
	IDL_MEMINT dims = 1;
	if(iDs == -1) dims = pEnt->uDs;
	
	IDL_VPTR pRet;
	
	/* Returns pRet->value.s.arr.data */
	DAS2C_DSET_data* pData = (DAS2C_DSET_data*) IDL_MakeTempStruct(
		DAS2C_DSET_pdef, /* The opaque structure definition */
		1,               /* Number of dimesions */
		&dims,           /* Size of each dimension, (only one dimension) */
		&pRet,           /* The actual structure variable */
		TRUE             /* Zero out the array */
	);

	DasDs* pDs = NULL;
	for(size_t u = 0; u < pEnt->uDs; ++u){
		
		if((iDs > -1)&&(iDs != u)) continue;
		
		pDs = pEnt->lDs[u];
		if(pDs == NULL) das2c_IdlMsgExit("Logic error, das2c_datasets.c");
		
		das2c_ds_to_dset(pEnt->nResultId, u, pData, pDs);
		
		++pData;
	}
	
	return pRet;
}
