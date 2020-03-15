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

/* ************************************************************************* */
/* API Function, careful with changes! */

#define D2C_DATA_MINA 1
#define D2C_DATA_MAXA 2
#define D2C_DATA_FLAG 0

/*
;+
; FUNCTION:
;  das2c_data
;
; PURPOSE:
;  Get data arrays from das2 variables
;
; CALLING SEQUENCE:
;  Result = das2c_data(var)
;  Result = das2c_data(var, slice)
;
; INPUTS:
;  var:  A DAS2C_VAR structure as returned by das2c_vars()
;
; OPTIONAL INPUTS:
;  slice: An anoymous structure providing a sub range in index space.  By 
;         default das2c_data() returns an array that has the same extent in
;         index space as the overall dataset (see DAS2C_DSET.SHAPE).  This
;         makes it easy to plot data as all correlated values, no matter the
;         underlying storage format, appear at the exact same index set.
; 
;         To save output memory, or when working with a subset of the data, a
;         slice structure be provided which has the following fields:
; 
;           I - The location in the left-most (fastest moving) index.
;           J - The location in the next to fastest moving index
; 
;          ... and so on up to the letter 'P'.
; 
;          The following example specifies a single point in an 8 dimensional
;          index space, which is the maximum number of array dimensions in IDL:
; 
;            {I:-2, J:3, K:13, L:7, M:1567, N:-2, O:42, P:117}
; 
;          Which is equivalent to:
;	 	
;            [-2,3,13,7,1567,-2,42,117]
; 
;          in standard IDL array subset notation.  Negative intergers are 
;          interpreted to indicate offsets from the end of the index range.
;          Where -1 is the last legal index value.
; 
;          To indicate that an index should be allowed to vary over it's entire
;          range, use the string '*' for the field value.  See the examples 
;          below for the use of '*'.
;	
;          Any field not specified is assumed to have the value '*'.  Thus the
;          following two calls produce identical outputs:
;
;             das2c_data(var, {I:'*', J:'*', K:'*'})
;             das2c_data(var)
;
;          Any fields in the slice structure that apply to dimensions higher
;          than the RANK of the dataset are ignored. (see DAS2C_DSET.RANK)
;
;          Currently only integers (both positive and negative) and the string
;          '*' are understood as slice field values.  Arrays and ranges are not
;          currently supported, though such support could be added if desired.
;         
;
; OUTPUT:
;          An array of values.  The IDL data type of the array is the same
;          as the DAS2C_VAR.TYPE field.
;
; EXAMPLES:
;  Get all time and frequency center coordinates from a dataset regardless of 
;  its RANK and iterate over all coordinates.  This opperation is common when
;  generating spectrograms.
;
;    v_time  = das2c_vars( das2c_pdims(ds, 'time'),      'center' )
;    v_freq  = das2c_vars( das2c_pdims(ds, 'frequency'), 'center' )
;    v_amp   = das2c_vars( das2c_pdims(ds, 'electric'),  'center' )
;
;    a_time  = das2c_data(v_time)
;    a_freq  = das2c_data(v_time)
;    a_amp   = das2c_data(v_time)
;
;    a_time  = reform(a_time, n_elements(a_time) )
;    a_freq  = reform(a_freq, n_elements(a_freq) )
;    a_amp   = reform(a_amp,  n_elements(a_amp)  )
;
;    for I=0,n_elements(a_time)-1 do begin
;      ; some plotting/binning code here
;    endfor
;
;  So long as the dataset has the indicated physical dimensions, this code 
;  works regardless of the internal storage mechanisims.
;
;  Get a all unique center time values for a rank 2 dataset where time is not a
;  function of the first index.
;
;    pd_time = das2c_pdims(ds, 'time')
;    v_time = das2c_vars(pd_time, 'center')
;    a_time = das2c_data(vTime, {I:0, J:'*'})
;
;  Get all unique frequency values for a rank
;
; MODIFICATION HISTORY:
;  In progress
;-
*/
static IDL_VPTR das2c_api_data(int argc, IDL_VPTR* argv)
{
	/* Get the variable or quit */
	
	/*ptrdiff_t aDsShape[DASIDX_MAX] = DASIDX_INIT_UNUSED;
	int nDsRank = 0;
	const DasVar* pVar = NULL;
	pVar = das2c_arg_to_var(argc, argv, 0, NULL, NULL, &nDsRank, aDsShape); * /
	
	*/
	
	/* Simplistic Algorithm:  
	
	   1. Get total extent requested
		
		2. Is it Zero items?   -yes-> return null
		       |
		       V
		2. Do I have an array                -no-> make temp array
		       | yes
				 V
		3. Is the extent equal to the array  -no-> make temp arary
		       | yes
				 V
		4. Wrap the array either the permanent or temporary array
	*/
	
	/*int aExtents[IDL_MAX_ARRAY_DIM]; */
	
	
	/* Just wrap the basic array now as a test */
	
		
	return IDL_GettmpNULL();
}
