#ifndef __LOGIC_H
#define __LOGIC_H

#include "core.h"

// Bitwise operators

object *fn_logand (object *args, object *env);
object *fn_logior (object *args, object *env);
object *fn_logxor (object *args, object *env);
object *fn_lognot (object *args, object *env);
object *fn_ash (object *args, object *env);
object *fn_logbitp (object *args, object *env);

#include "frozen/lookup.h"
#define FREEZE_LO(no, str) \
    FREEZE(logic_, no, str)
#define REIFY_LO(no, fn, min_arg, max_arg) \
    REIFY(logic_, no, fn, min_arg, max_arg)

#define FREEZE_LOGIC \
    FREEZE_LO(logand, "logand"); \
    FREEZE_LO(logior, "logior"); \
    FREEZE_LO(logxor, "logxor"); \
    FREEZE_LO(lognot, "lognot"); \
    FREEZE_LO(ash, "ash"); \
    FREEZE_LO(logbitp, "logbitp"); \

#define REIFY_LOGIC \
    REIFY_LO(logand, fn_logand, 0, CALL_ARGUMENTS_LIMIT), \
    REIFY_LO(logior, fn_logior, 0, CALL_ARGUMENTS_LIMIT), \
    REIFY_LO(logxor, fn_logxor, 0, CALL_ARGUMENTS_LIMIT), \
    REIFY_LO(lognot, fn_lognot, 1, 1), \
    REIFY_LO(ash, fn_ash, 2, 2), \
    REIFY_LO(logbitp, fn_logbitp, 2, 2),

#endif
