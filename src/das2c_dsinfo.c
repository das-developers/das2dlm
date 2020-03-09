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
;  das2c_dsinfo
;
; PURPOSE:
;  Output summary information string about a dataset
;
; CALLING SEQUENCE:
;  Result = das2c_dsinfo(query_id, ds_index)
;
; INPUTS:
;  query_id: The identification integer for the stored query result as
;            returned by das2c_readhttp() or das2c_queries.
;
; OPTIONAL INPUTS:
;  ds_index: Output information on only a single dataset from the query
;            Most queries will output only a single dataset at index 0,
;            Though common sources such as Juno Waves output more than 1.
;
; OUTPUT:
;  This function returns an array of structures providing an overview of
;  each stored result.  Output structures have the fields:
;
;    'idx':      Long    ; The index of this dataset, starts from 0
;    'name':     String  ; The name of this dataset
;    'physdims': Long    ; The number of physical dimensions in the dataset
;    'props':    Long    ; The number of metadata properties in the dataset
;    'size':     Long64  ; The total number of values in the dataset   
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

#define D2C_DSINFO_MINA 2
#define D2C_DSINFO_MAXA 2
#define D2C_DSINFO_FLAG 0

static IDL_VPTR das2c_dsinfo(int argc, IDL_VPTR* argv)
{
	char sBuf[1024] = {'\0'};
	int nBuf = 1024;
	
	/* Check args exist */
	if(argc < 1) das2c_IdlMsgExit("Query ID not provided");	
	if(argc < 2) das2c_IdlMsgExit("Dataset number not provided");	
	
	int iQueryId;
	int iDsIdx;
	
	IDL_VPTR pTmpVar = NULL;
	pTmpVar = IDL_BasicTypeConversion(1, argv, IDL_TYP_LONG);
	iQueryId = pTmpVar->value.l;
	IDL_DELTMP(pTmpVar);
	
	pTmpVar = IDL_BasicTypeConversion(1, argv + 1, IDL_TYP_LONG);
	iDsIdx = pTmpVar->value.l;
	IDL_DELTMP(pTmpVar);
	if(iDsIdx < 0) das2c_IdlMsgExit("Invalid dataset index %d", iDsIdx);
	
	const DasIdlDbEnt* pEnt = das2c_db_getent(iQueryId);
	
	/* Check args valid */
	if(pEnt == NULL)
		das2c_IdlMsgExit("No query result has ID %d", iQueryId);
	
	if(iDsIdx >= pEnt->uDs){
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
		
	DasDs* pDs = pEnt->lDs[iDsIdx];
	if(pDs == NULL) das2c_IdlMsgExit("Logic error, das2c_dsinfo.c");
	
	DasDs_toStr(pDs, sBuf, nBuf);
	
	return (IDL_StrToSTRING(sBuf));
}
