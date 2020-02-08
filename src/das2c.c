#include <stdio.h>

#include <das2/core.h>

#include "idl_export.h"

/* Define message codes and their corresponding printf(3) format strings. 
   Note that message codes start at zero and each one is one less that the
	previous one. Codes must be monotonic and contiguous. */
static IDL_MSG_DEF msg_arr[] = {
#define M_TM_BAD 0
	{ "M_TM_BAD", "%NSomething bad happened" },

#define M_TM_GOOD -1
	{ "M_TM_GOOD", "%NSomething good happened"}
};

/* The load function fills in this message block handle with the opaque handle
   to the message block used for this module. Th other routines can then use
	it to throw errors from this block.  */

static IDL_MSG_BLOCK msg_block;

/* Implementation of the readhttp IDL function */
static IDL_VPTR das2c_readhttp(int argc, IDL_VPTR *argv)
{
	IDL_MessageFromBlock(msg_block, M_TM_GOOD, IDL_MSG_RET);
	return IDL_StrToSTRING("das2c_readhttp, doing nothing for now...");
}

/* ************************************************************************* */
/* Load the module definition tables.
   These tables contain information on the functions and procedures that
   make up the das2c DLM. The information contained in these tables
   must be identical to that contained in das2c.dlm. */
int IDL_Load(void){

 	static IDL_SYSFUN_DEF2 function_addr[1] = {
		{ {das2c_readhttp}, "DAS2C_READHTTP", 0, IDL_MAXPARAMS, 0, NULL},
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

	/* Attempt to initialize das2C */
	das_init("das2c DLM", DASERR_DIS_RET, 0, DASLOG_INFO, NULL);
	
 	/* Register our routine. The routines must be specified exactly the same as in 
	   testmodule.dlm. */
	return IDL_SysRtnAdd(function_addr, TRUE, IDL_CARRAY_ELTS(function_addr));
		
	
	/* We don't have any proceedures, just functions
	if(! IDL_SysRtnAdd(procedure_addr, FALSE, IDL_CARRAY_ELTS(procedure_addr)) )
		return FALSE;
	*/		
	return TRUE;
}
