/* Copyright (C) 2020 Chris Piker <chris-piker@uiowa.edu>
 *                    
 * This file is part of das2dlm, an Interactive Data Language (IDL) binding
 * for the das2C library.  IDL is a trademark of Harris Geospatial Solutions,
 * Inc.  The shared object generated by this code may be loaded by main 
 * programs, or other shared object, with closed source licenses.
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

/* Set the version number advertised in user agent strings by default */
#define DAS2DLM_S_VER "0.1"

/* ************************************************************************* */
/* Include C files directly since the module is composed almost entirely of  */
/* static functions.                                                         */

#include "das2c_message.c"   /* Infastructure */
#include "das2c_db.c"

#include "das2c_queries.c"   /* exported functions */
/*#include "das2c_dsinfo.c"
#include "das2c_datasets.c"
#include "das2c_vars.c"
#include "das2c_props.c"
#include "das2c_array.c"
#include "das2c_free.c"*/
#include "das2c_readhttp.c"

/* ************************************************************************* */
/* Load the module definition tables.
   These tables contain information on the functions and procedures that
   make up the das2c DLM. The information contained in these tables
   must be identical to that contained in das2c.dlm. */
int IDL_Load(void){

	/* Define our structures */
	g_pDas2c_query_sdef = das2c_query_sdef();


 	static IDL_SYSFUN_DEF2 function_addr[] = {
		
		/* the "database" functions, since manipulating IDL_TYP_PTR and
		   IDL_TYP_OBJREF are illegal (grrr....) */
		{ (IDL_SYSRTN_GENERIC)das2c_queries,  "DAS2C_QUERIES",  D2C_QUERIES_MINA,  D2C_QUERIES_MAXA,  D2C_QUERIES_FLAGS,  NULL},
/*		{ (IDL_SYSRTN_GENERIC)das2c_dsinfo,   "DAS2C_DSINFO",   D2C_DSINFO_MINA,   D2C_DSINFO_MAXA,   D2C_DSINFO_FLAGS,   NULL},
		{ (IDL_SYSRTN_GENERIC)das2c_datasets, "DAS2C_DATASETS", D2C_DATASETS_MINA, D2C_DATASETS_MAXA, D2C_DATASETS_FLAGS, NULL},
		{ (IDL_SYSRTN_GENERIC)das2c_physdims, "DAS2C_PHYSDIMS", D2C_PHYSDIMS_MINA, D2C_PHYSDIMS_MAXA, D2C_PHYSDIMS_FLAGS, NULL},
		{ (IDL_SYSRTN_GENERIC)das2c_vars,     "DAS2C_VARS",     D2C_VARS_MINA,     D2C_VARS_MAXA,     D2C_VARS_FLAGS,     NULL},
		{ (IDL_SYSRTN_GENERIC)das2c_props,    "DAS2C_PROPS",    D2C_PROPS_MINA,    D2C_PROPS_MAXA,    D2C_PROPS_FLAGS,    NULL},
		{ (IDL_SYSRTN_GENERIC)das2c_array,    "DAS2C_ARRAY",    D2C_ARRAY_MINA,    D2C_ARRAY_MAXA,    D2C_ARRAY_FLAGS,    NULL},
		{ (IDL_SYSRTN_GENERIC)das2c_free,     "DAS2C_FREE",     D2C_FREE_MINA,     D2C_FREE_MAXA,     D2C_FREE_FLAGS,     NULL},
*/				
		/* The "get data" functions, only one for now */
		{ (IDL_SYSRTN_GENERIC)das2c_readhttp, "DAS2C_READHTTP", D2C_READHTTP_MINA, D2C_READHTTP_MAXA, D2C_READHTTP_FLAGS, NULL}
		
		/* The ancillary functions, none so far */
	};

	/*
	static IDL_SYSFUN_DEF2 procedure_addr[] = {
		{ (IDL_SYSRTN_GENERIC) testpro, "das2c_init" 0, IDL_MAX_ARRAY_DIM, 0, 0},
	};
	*/

 	/* Create a message block to hold our messages. Save its handle where the
	   other routines can access it. */
	msg_block = IDL_MessageDefineBlock("das2c",IDL_CARRAY_ELTS(msg_arr), msg_arr);
	if(msg_block == NULL) return IDL_FALSE;

	/* Initialize das2, send das2 longs to the IDL message system */
	das_init("das2c DLM", DASERR_DIS_RET, 0, DASLOG_INFO, _das2c_log2idl);
	
	/* Initialze the query result database.  Searches are linear so
	 * start small */
	if(! _das2c_db_init(64) ) return FALSE;
	
	/* Override printing errors to stderr, save them instead */
	das_save_error(DASIDL_ERR_MSG_SZ);
	
 	/* Register our routine. The routines must be specified exactly the same as in 
	   testmodule.dlm. */
	return IDL_SysRtnAdd(function_addr, TRUE, IDL_CARRAY_ELTS(function_addr));
		
	
	/* We don't have any proceedures, just functions
	if(! IDL_SysRtnAdd(procedure_addr, FALSE, IDL_CARRAY_ELTS(procedure_addr)) )
		return FALSE;
	*/		
	return TRUE;
}
