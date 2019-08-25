#ifndef __LISP_LIBRARY_H
#define __LISP_LIBRARY_H

#include "core.h"

// LispLibrary

#if defined(lisplibrary)
int glibrary ();
void loadfromlibrary (object *env);
object *fn_require (object *args, object *env);
object *fn_listlibrary (object *args, object *env);
#endif

#endif
