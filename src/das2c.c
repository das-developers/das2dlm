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

#define _POSIX_C_SOURCE 200112
#define _XOPEN_SOURCE 500

#include <pthread.h>
#include <time.h>
#include <stdio.h>

#include <das2/core.h>

#include "idl_export.h"

#define DLMERR 63

/* Check to see that das array max indecies are at least as big as
   the max IDL index, if not code in das2c_vars, das2c_array and 
	das2c_datasets will need to change */

#if IDL_MAX_ARRAY_DIM != 8
#error IDL has changed, DLM code need to be updated
#endif

#if IDL_MAX_ARRAY_DIM > DASIDX_MAX
#error das2C has changed, DLM code need to be updated
#endif



/* For any rank array outputs, set placeholder for values outside
   the rank of the dataset */
#define D2C_OVER_RANK_FLAG -99

/* Set the version number advertised in user agent strings by default */
#define DAS2DLM_S_VER "0.2"

/* ************************************************************************* */
/* Include C files directly since the module is composed almost entirely of  */
/* static functions.                                                         */

#include "das2c_message.c"   /* Infastructure */
#include "das2c_db.c"

/* used in structure definitions for index_map and relate shape items */
static IDL_MEMINT g_aShape1[2];
static IDL_MEMINT g_aShape2[2];
static IDL_MEMINT g_aShape3[2];
static IDL_MEMINT g_aShape4[2];
static IDL_MEMINT g_aShape5[2];
static IDL_MEMINT g_aShape6[2];
static IDL_MEMINT g_aShape7[2];
static IDL_MEMINT g_aShape8[2];

#include "das2c_queries.c"   /* exported functions */
#include "das2c_datasets.c"  
#include "das2c_dsinfo.c"
#include "das2c_pdims.c"
#include "das2c_vars.c"
/*#include "das2c_props.c"
#include "das2c_data.c"
#include "das2c_free.c" */
#include "das2c_readhttp.c"

/* ************************************************************************* */
/* Load the module definition tables.
   These tables contain information on the functions and procedures that
   make up the das2c DLM. The information contained in these tables
   must be identical to that contained in das2c.dlm. */
int IDL_Load(void){	

	/* Define the shape 1-8 struct def array */
	g_aShape1[0] = 1;  g_aShape1[1] = 1;
	g_aShape2[0] = 1;  g_aShape2[1] = 2;
	g_aShape3[0] = 1;  g_aShape3[1] = 3;
	g_aShape4[0] = 1;  g_aShape4[1] = 4;
	g_aShape5[0] = 1;  g_aShape5[1] = 5;
	g_aShape6[0] = 1;  g_aShape6[1] = 6;
	g_aShape7[0] = 1;  g_aShape7[1] = 7;
	g_aShape8[0] = 1;  g_aShape8[1] = 8;

	/* Define our structures */
	define_DAS2C_QUERY();
	define_DAS2C_DSET();
	define_DAS2C_PDIM();
	define_DAS2C_VAR();
/*	define_DAS2C_PROP(); */
		
 	static IDL_SYSFUN_DEF2 function_addr[] = {
		{ 
			{(IDL_SYSRTN_GENERIC)das2c_api_queries}, "DAS2C_QUERIES",
			D2C_QUERIES_MINA, D2C_QUERIES_MAXA, D2C_QUERIES_FLAG, NULL
		},		
		{ 
			{(IDL_SYSRTN_GENERIC)das2c_api_datasets}, "DAS2C_DATASETS",
			D2C_DATASETS_MINA, D2C_DATASETS_MAXA, D2C_DATASETS_FLAG, NULL
		},
		{ 
			{(IDL_SYSRTN_GENERIC)das2c_api_dsinfo}, "DAS2C_DSINFO",
		   D2C_DSINFO_MINA, D2C_DSINFO_MAXA, D2C_DSINFO_FLAG, NULL
		},		
		{ 
			{(IDL_SYSRTN_GENERIC)das2c_api_pdims}, "DAS2C_PDIMS",
			D2C_PDIMS_MINA, D2C_PDIMS_MAXA, D2C_PDIMS_FLAG, NULL
		},
		{ 
			{(IDL_SYSRTN_GENERIC)das2c_api_vars},     "DAS2C_VARS",
			D2C_VARS_MINA,     D2C_VARS_MAXA,     D2C_VARS_FLAG,     NULL
		},
/*		{ 
			{(IDL_SYSRTN_GENERIC)das2c_api_dsprops},  "DAS2C_PROPS",
			D2C_DSPROPS_MINA,  D2C_PROPS_MAXA,  D2C_PROPS_FLAG,  NULL
		},
		{
			{(IDL_SYSRTN_GENERIC)das2c_api_data},    "DAS2C_DATA",
			D2C_DATA_MINA,     D2C_DATA_MAXA,    D2C_DATA_FLAG,      NULL
		},
		{
			{(IDL_SYSRTN_GENERIC)das2c_api_free},     "DAS2C_FREE",
			D2C_FREE_MINA,     D2C_FREE_MAXA,     D2C_FREE_FLAG,     NULL
		},
*/		/* The "get data" functions, only one for now */
		{
			{(IDL_SYSRTN_GENERIC)das2c_api_readhttp}, "DAS2C_READHTTP",
			D2C_READHTTP_MINA, D2C_READHTTP_MAXA, D2C_READHTTP_FLAG, NULL
		}
		
		/* The ancillary functions, none so far */
	};

	/*
	static IDL_SYSFUN_DEF2 procedure_addr[] = {
		{ (IDL_SYSRTN_GENERIC) testpro, "das2c_init" 0, IDL_MAX_ARRAY_DIM, 0, 0},
	};
	*/

 	/* Create a message block to hold our messages. Save its handle where the
	   other routines can access it. */
	msg_block = IDL_MessageDefineBlock("das2c", IDL_CARRAY_ELTS(msg_arr), msg_arr);
	if(msg_block == NULL) return IDL_FALSE;

	/* Initialize das2, send das2 longs to the IDL message system */
	das_init("das2c DLM", DASERR_DIS_RET, 0, DASLOG_INFO, das2c_log2idl);
	
	/* Initialze the query result database.  Searches are linear so
	 * start small */
	if(! das2c_db_init(64) ) return FALSE;
	
	/* Override printing errors to stderr, save them instead */
	das_save_error(DASIDL_ERR_MSG_SZ);
	
	/* Switch info string print order to fast index first */
	das_varindex_prndir(false);
	
 	/* Register our routine. The routines must be specified exactly the same as 
	   in das2c.dlm, not sure how to sycronize this automatically */
	return IDL_SysRtnAdd(function_addr, TRUE, IDL_CARRAY_ELTS(function_addr));
		
	
	/* We don't have any proceedures, just functions
	if(! IDL_SysRtnAdd(procedure_addr, FALSE, IDL_CARRAY_ELTS(procedure_addr)) )
		return FALSE;
	*/		
	return TRUE;
}
