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
;    'shape':    8 Long64 ; The extents of this dataset in index space.
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
; MODIFICATION HISTORY:
;  Written by: Chris Piker, 2020-03-09
;-
*/
		
/* Output structure definition */
static IDL_STRUCT_TAG_DEF _das2c_dataset_tags[] = {
	{"id",       NULL,      (void*)IDL_TYP_LONG},
	{"name",     NULL,      (void*)IDL_TYP_STRING},
	{"physdims", NULL,      (void*)IDL_TYP_LONG},
	{"props",    NULL,      (void*)IDL_TYP_LONG},
	{"shape",    g_aShape8, (void*)IDL_TYP_LONG64},
	{"size",     NULL,      (void*)IDL_TYP_LONG64},
	{0}
};

typedef struct _das2c_ds_sum{
	IDL_LONG   id;
	IDL_STRING name;
	IDL_LONG   physdims;
	IDL_LONG   props;
	IDL_LONG64 shape[8];
	IDL_LONG64 size;
} das2c_DsSummary;

#define D2C_DATASETS_MINA 1
#define D2C_DATASETS_MAXA 2
#define D2C_DATASETS_FLAG 0

static IDL_StructDefPtr g_das2c_pDsSumDef;

static void DAS2C_DATASET_def()
{
	g_das2c_pDsSumDef = IDL_MakeStruct("DAS2C_DATASET", _das2c_dataset_tags);
}
		
static IDL_VPTR das2c_datasets(int argc, IDL_VPTR* argv)
{
	/* Check args exist */
	if(argc < 1) das2c_IdlMsgExit("Query ID not provided");	
	
	int iQueryId;
	
	IDL_VPTR pTmpVar = NULL;
	pTmpVar = IDL_BasicTypeConversion(1, argv, IDL_TYP_LONG);
	iQueryId = pTmpVar->value.l;
	IDL_DELTMP(pTmpVar);
	
	int iDs = -1;  /* Dataset index, -1 = All datasets */
	
	if(argc > 1){
		pTmpVar = IDL_BasicTypeConversion(1, argv + 1, IDL_TYP_LONG);
		iDs = pTmpVar->value.l;
		IDL_DELTMP(pTmpVar);
		if(iDs < 0) das2c_IdlMsgExit("Invalid dataset index %d", iDs);
	}
	
	/* Check args valid */
	const DasIdlDbEnt* pEnt = das2c_db_getent(iQueryId);
	if(pEnt == NULL)
		das2c_IdlMsgExit("No query result has ID %d", iQueryId);
	
	if(iDs >= pEnt->uDs){
		if(pEnt->uDs == 0)
			das2c_IdlMsgExit(
				"Query result %d doesn't contain any datasets", iQueryId
			);
		else
			das2c_IdlMsgExit(
				"Query result %d dataset indices are 0 to %zu", 
				iQueryId, pEnt->uDs - 1
			);
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
	ptrdiff_t shape[16] = {0};
	int r = 0;  
	for(size_t u = 0; u < pEnt->uDs; ++u){
		
		if((iDs > -1)&&(iDs != u)) continue;
		
		pDs = pEnt->lDs[iDs];
		if(pDs == NULL) das2c_IdlMsgExit("Logic error, das2c_datasets.c");
		
		/* Write into IDL memory using parallel structure pointer */
		pData->id       = u;
		pData->physdims = DasDs_numDims(pDs, DASDIM_COORD) + 
				            DasDs_numDims(pDs, DASDIM_DATA);
		pData->props    = DasDesc_length((DasDesc*)pDs);
		if(DasDs_id(pDs) != NULL) 
			IDL_StrStore(&(pData->name), DasDs_id(pDs));
		
		/* Need to flag ragged datesets somehow */
		int nRank = DasDs_shape(pDs, shape);
		for(r = 0; r < nRank; ++r) pData->shape[r] = shape[r];
		
		pData->size = 0;
		for(uAry = 0; uAry < pDs->uArrays; ++uAry){
			pAry = pDs->lArrays[uAry];
			pData->size += DasAry_size(pAry);
		}
		
		++pData;
	}
	
	return pRet;
}
