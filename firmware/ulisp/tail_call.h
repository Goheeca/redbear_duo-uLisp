#ifndef __TAIL_CALL_H
#define __TAIL_CALL_H

#include "core.h"

// Tail-recursive forms

object *tf_progn (object *args, object *env);
object *tf_return (object *args, object *env);
object *tf_if (object *args, object *env);
object *tf_cond (object *args, object *env);
object *tf_when (object *args, object *env);
object *tf_unless (object *args, object *env);
object *tf_case (object *args, object *env);
object *tf_and (object *args, object *env);
object *tf_or (object *args, object *env);

#endif
