/*
;+
; OPTIONAL INPUTS: 
;  slice:    Gets a sub-set of the data values.  By default the output
;            array will have the same rank and extents as the overall
;            dataset.  Thus all data appears to be scatter data.
;-

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

#define D2C_DATA_MINA 4
#define D2C_DATA_MAXA 5
#define D2C_DATA_FLAG 0


/*
;+

;
;    'idxmap': 8 Long   ; How to map indices for this variables array into the
;                       ; overall datest index space.  A -1 means array values
;                       ; are degenerate in a given dataset index.
*/
		
/*
;+
; FUNCTION:
;  das2c_array
;
; PURPOSE:
;  Get low-level backing arrays.  
;  See das2c_get for general data access
;
; CALLING SEQUENCE:
;  Result = das2c_array(query_id, ds_index, pdim, role)
;
; INPUTS:
;  query_id: The identification integer for the stored query result as
;            returned by das2c_readhttp() or das2c_queries.
;
;  ds_index: The dataset index, often 0.  See das2c_datasets() for details.
;
;  pdim:     Either the physical dimension index (long), or the dimension's
;            name (string).  For more infor see das2c_physdims().
;
;  role:     A string providing the variable role. Variable role names are
;            standardized.  The most common one is 'center' which represents
;            the central point of a measurement in a particular physical
;            dimension.  The full set is:
;
;            'center', 'offest', 'min', 'max', 'width', 'mean', 'median',
;            'mode', 'reference', 'offset', 'max_error', 'min_error',
;            'uncertainty',  'std_dev', 'point_spread', 'weight'
;
; OUTPUT:
;            An array of values.  The IDL data type of the array depends
;            on the input data.  If the specified variable is a virtual
;            variable then this function return !NULL
;
; EXAMPLES:
;  List summary of all variable roles for the 'time' dimension in dataset 0 of
;  query result 27
;    das2c_vars(27, 0, 'time')
;
;  List summary information of the 'center' variable for 'time' in dataset 0 for
;  query result 27
;    das2c_vars(27, 0, 'time', 'center')
;
;  List summary information for the first variable in the first physical
;  physical dimension of the first datasets, what ever it happens to be.
;    das2c_vars(27, 0, 0, 0)
;
; MODIFICATION HISTORY:
;  Written by: Chris Piker, 2020-03-10
;-
*/
static IDL_VPTR das2c_api_array(int argc, IDL_VPTR* argv)
{
	
	return IDL_GettmpNULL();
}
