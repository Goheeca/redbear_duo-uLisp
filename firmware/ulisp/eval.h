#ifndef __EVAL_H
#define __EVAL_H

#include "core.h"

// Handling closures

object *closure (int tc, symbol_t name, object *state, object *function, object *args, object **env);
object *apply (symbol_t name, object *function, object *args, object *env);

// Main evaluator

extern uint8_t End;

object *eval (object *form, object *env);

#endif
