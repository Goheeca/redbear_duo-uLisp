#ifndef __GC_H
#define __GC_H

#include "core.h"

// Garbage collection

void markobject (object *obj);
void sweep ();
void gc (object *form, object *env);

object *fn_gc (object *obj, object *env);
object *fn_room (object *args, object *env);


#endif
