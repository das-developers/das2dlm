/* IDL 8.4 does not have IDL_GettmpNULL() implementation. This is an attempt to
   implement it */
#if IDL_VERSION_MINOR < 7

#ifndef IDL_V_NULL
#define IDL_V_NULL          64	 /* Variable is !NULL */
#endif

IDL_VPTR IDL_GettmpNULL()
{
	IDL_VPTR tmp = IDL_Gettmp();
	tmp->flags = IDL_V_NULL;
	return tmp;
}

#endif
