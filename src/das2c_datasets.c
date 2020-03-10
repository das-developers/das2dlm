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
static IDL_STRUCT_TAG_DEF _das2c_dataset_tags[] = {
	{"ID",       NULL,      (void*)IDL_TYP_LONG},
	{"NAME",     NULL,      (void*)IDL_TYP_STRING},
	{"PHYSDIMS", NULL,      (void*)IDL_TYP_LONG},
	{"PROPS",    NULL,      (void*)IDL_TYP_LONG},
	{"RANK",     NULL,      (void*)IDL_TYP_LONG},
	{"SHAPE",    g_aShape8, (void*)IDL_TYP_LONG64},
	{"SIZE",     NULL,      (void*)IDL_TYP_LONG64},
	{0}
};

typedef struct _das2c_ds_sum{
	IDL_LONG   id;
	IDL_STRING name;
	IDL_LONG   physdims;
	IDL_LONG   props;
	IDL_LONG   rank;
	IDL_LONG64 shape[8];
	IDL_LONG64 size;
} das2c_DsSummary;

static IDL_StructDefPtr g_das2c_pDsSumDef;

static void DAS2C_DATASET_def()
{
	g_das2c_pDsSumDef = IDL_MakeStruct("DAS2C_DATASET", _das2c_dataset_tags);
}

/* ************************************************************************* */
/* Downstream helpers for pulling out datasets, these don't return on error  */

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
static const DasDs* das2c_check_ds_id(const DasIdlDbEnt* pEnt, int iDs)
{
	if(pEnt == NULL) das2c_IdlMsgExit("Logic error, das2c_datasets.c");
	
	if(iDs >= pEnt->uDs){
		if(pEnt->uDs == 0)
			das2c_IdlMsgExit(
				"Query result %d doesn't contain any datasets", pEnt->nQueryId
			);
		else
			das2c_IdlMsgExit(
				"Query result %d dataset indices are 0 to %zu", 
				pEnt->nQueryId, pEnt->uDs - 1
			);
	}
	const DasDs* pDs = pEnt->lDs[iDs];
	if(pDs == NULL) das2c_IdlMsgExit("Logic error, das2c_datasets.c");
	return pDs;
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
;  Result = das2c_datasets(query_id, ds_index)
;
; INPUTS:
;  query_id: The identification integer for the stored query result as
;            returned by das2c_readhttp() or das2c_queries.
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
;    'id':       Long     ; The ID number of this dataset, starts from 0
;
;    'name':     String   ; The name of this dataset, dataset names are not
;                         ; usually unique.  This limits thier utility.
;
;    'physdims': Long     ; The number of physical dimensions in the dataset
;
;    'props':    Long     ; The number of metadata properties in the dataset
;
;    'rank':     Long     ; The number of entries in 'shape' that matter.  
;                         ; Since IDL uses a 'first index fastest' memory layout
;                         ; only the last 'rank' values of 'shape' actually
;                         ; matter.
;
;    'shape':    8 Long64 ; The extents of this dataset in the full IDL index
;                         ; space.  
;                         ;
;                         ; Ideally this array would be of length 'rank', 
;                         ; however due to limitations on DLMs, this array is
;                         ; always 8 elements long. An example of trimming
;                         ; the shape array is provided in EXAMPLES below.
;
;    'size':     Long64   ; The total number of values in the dataset may be
;                         ; less that nth  
;
; EXAMPLES:
;  List summary information on all datasets from a query with ID 27
;    das2c_datasets(27)
;
;  List summary information on the first dataset from query ID 27
;    das2c_queries(27, 0);
;
;  Trim the shape array down to the actual rank of the dataset.
;    ds = das2c_queries(27, 0);
;    shape = ds.shape[8 - ds.rank : -1 ]
;
; MODIFICATION HISTORY:
;  Written by: Chris Piker, 2020-03-10
;-
*/
static IDL_VPTR das2c_api_datasets(int argc, IDL_VPTR* argv)
{
	
	/* Get/check Query ID */
	int iQueryId = das2c_args_query_id(argc, argv, 0);
	const DasIdlDbEnt* pEnt = das2c_check_query_id(iQueryId);
	
	/* Handle optional dataset ID */
	int iDs = -1;  /* Dataset index, -1 = All datasets */
		
	if(argc > 1){
		iDs = das2c_args_ds_id(argc, argv, 1);
		das2c_check_ds_id(pEnt, iDs);
	}
		
	IDL_MEMINT dims = pEnt->uDs;
	if(iDs == -1) dims = 1;
	
	IDL_VPTR pRet;  /* the to-be-returned structure */
	
	/* Returns pRet->value.s.arr.data */
	das2c_DsSummary* pData = (das2c_DsSummary*) IDL_MakeTempStruct(
		g_das2c_pDsSumDef,   /* The opaque structure definition */
		1,                   /* Number of dimesions */
		&dims,               /* Size of each dimension, (only one dimension) */
		&pRet,               /* The actual structure variable */
		TRUE                 /* Zero out the array */
	);

	DasDs* pDs = NULL;
	size_t uAry = 0;
	DasAry* pAry = NULL;
	ptrdiff_t shape[DASIDX_MAX] = DASIDX_INIT_UNUSED;
	int nRank, r = 0;  
	for(size_t u = 0; u < pEnt->uDs; ++u){
		
		if((iDs > -1)&&(iDs != u)) continue;
		
		pDs = pEnt->lDs[u];
		if(pDs == NULL) das2c_IdlMsgExit("Logic error, das2c_datasets.c");
		
		/* Write into IDL memory using parallel structure pointer */
		pData->id       = u;
		pData->physdims = DasDs_numDims(pDs, DASDIM_COORD) + 
				            DasDs_numDims(pDs, DASDIM_DATA);
		pData->props    = DasDesc_length((DasDesc*)pDs);
		if(DasDs_id(pDs) != NULL) 
			IDL_StrStore(&(pData->name), DasDs_id(pDs));
		
		/* Need to flag ragged datesets somehow */
		nRank = DasDs_shape(pDs, shape);
		
		pData->rank    = nRank;
		
		for(r = 0; r < IDL_MAX_ARRAY_DIM; ++r){ 
			if(r < nRank) pData->shape[(IDL_MAX_ARRAY_DIM - 1) - r] = shape[r];
			else pData->shape[(IDL_MAX_ARRAY_DIM - 1) - r] = D2C_OVER_RANK_FLAG;
		}
		
		pData->size = 0;
		for(uAry = 0; uAry < pDs->uArrays; ++uAry){
			pAry = pDs->lArrays[uAry];
			pData->size += DasAry_size(pAry);
		}
		
		++pData;
	}
	
	return pRet;
}
