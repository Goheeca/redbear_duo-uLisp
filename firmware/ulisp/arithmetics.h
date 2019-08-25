#ifndef __ARITHMETICS_H
#define __ARITHMETICS_H

#include "core.h"

// Arithmetic functions

object *add_floats (object *args, float fresult);
object *fn_add (object *args, object *env);
object *subtract_floats (object *args, float fresult);
object *negate (object *arg);
object *fn_subtract (object *args, object *env);
object *multiply_floats (object *args, float fresult);
object *fn_multiply (object *args, object *env);
object *divide_floats (object *args, float fresult);
object *fn_divide (object *args, object *env);
object *fn_mod (object *args, object *env);
object *fn_oneplus (object *args, object *env);
object *fn_oneminus (object *args, object *env);
object *fn_abs (object *args, object *env);
object *fn_random (object *args, object *env);
object *fn_maxfn (object *args, object *env);
object *fn_minfn (object *args, object *env);

// Arithmetic comparisons

object *fn_noteq (object *args, object *env);
object *fn_numeq (object *args, object *env);
object *fn_less (object *args, object *env);
object *fn_lesseq (object *args, object *env);
object *fn_greater (object *args, object *env);
object *fn_greatereq (object *args, object *env);
object *fn_plusp (object *args, object *env);
object *fn_minusp (object *args, object *env);
object *fn_zerop (object *args, object *env);
object *fn_oddp (object *args, object *env);
object *fn_evenp (object *args, object *env);

// Number functions

object *fn_integerp (object *args, object *env);
object *fn_numberp (object *args, object *env);

// Floating-point functions

object *fn_floatfn (object *args, object *env);
object *fn_floatp (object *args, object *env);
object *fn_sin (object *args, object *env);
object *fn_cos (object *args, object *env);
object *fn_tan (object *args, object *env);
object *fn_asin (object *args, object *env);
object *fn_acos (object *args, object *env);
object *fn_atan (object *args, object *env);
object *fn_sinh (object *args, object *env);
object *fn_cosh (object *args, object *env);
object *fn_tanh (object *args, object *env);
object *fn_exp (object *args, object *env);
object *fn_sqrt (object *args, object *env);
object *fn_log (object *args, object *env);
int intpower (int base, int exp);
object *fn_expt (object *args, object *env);
object *fn_ceiling (object *args, object *env);
object *fn_floor (object *args, object *env);
object *fn_truncate (object *args, object *env);
int myround (float number);
object *fn_round (object *args, object *env);

#include "frozen/lookup.h"
#define FREEZE_AR(no, str) \
    FREEZE(arithmetics_, no, str)
#define REIFY_AR(no, fn, min_arg, max_arg) \
    REIFY(arithmetics_, no, fn, min_arg, max_arg)

#define FREEZE_ARITHMETICS \
    FREEZE_AR(add, "+"); \
    FREEZE_AR(subtract, "-"); \
    FREEZE_AR(multiply, "*"); \
    FREEZE_AR(divide, "/"); \
    FREEZE_AR(mod, "mod"); \
    FREEZE_AR(oneplus, "1+"); \
    FREEZE_AR(oneminus, "1-"); \
    FREEZE_AR(abs, "abs"); \
    FREEZE_AR(random, "random"); \
    FREEZE_AR(max, "max"); \
    FREEZE_AR(min, "min"); \
    FREEZE_AR(noteq, "/="); \
    FREEZE_AR(numeq, "="); \
    FREEZE_AR(less, "<"); \
    FREEZE_AR(lesseq, "<="); \
    FREEZE_AR(greater, ">"); \
    FREEZE_AR(greatereq, ">="); \
    FREEZE_AR(plusp, "plusp"); \
    FREEZE_AR(minusp, "minusp"); \
    FREEZE_AR(zerop, "zerop"); \
    FREEZE_AR(oddp, "oddp"); \
    FREEZE_AR(evenp, "evenp"); \
    FREEZE_AR(integerp, "integerp"); \
    FREEZE_AR(numberp, "numberp"); \
    FREEZE_AR(floatfn, "float"); \
    FREEZE_AR(floatp, "floatp"); \
    FREEZE_AR(sin, "sin"); \
    FREEZE_AR(cos, "cos"); \
    FREEZE_AR(tan, "tan"); \
    FREEZE_AR(asin, "asin"); \
    FREEZE_AR(acos, "acos"); \
    FREEZE_AR(atan, "atan"); \
    FREEZE_AR(sinh, "sinh"); \
    FREEZE_AR(cosh, "cosh"); \
    FREEZE_AR(tanh, "tanh"); \
    FREEZE_AR(exp, "exp"); \
    FREEZE_AR(sqrt, "sqrt"); \
    FREEZE_AR(log, "log"); \
    FREEZE_AR(expt, "expt"); \
    FREEZE_AR(ceiling, "ceiling"); \
    FREEZE_AR(floor, "floor"); \
    FREEZE_AR(truncate, "truncate"); \
    FREEZE_AR(round, "round");

#define REIFY_ARITHMETICS \
    REIFY_AR(add, fn_add, 0, CALL_ARGUMENTS_LIMIT), \
    REIFY_AR(subtract, fn_subtract, 1, CALL_ARGUMENTS_LIMIT), \
    REIFY_AR(multiply, fn_multiply, 0, CALL_ARGUMENTS_LIMIT), \
    REIFY_AR(divide, fn_divide, 1, CALL_ARGUMENTS_LIMIT), \
    REIFY_AR(mod, fn_mod, 2, 2), \
    REIFY_AR(oneplus, fn_oneplus, 1, 1), \
    REIFY_AR(oneminus, fn_oneminus, 1, 1), \
    REIFY_AR(abs, fn_abs, 1, 1), \
    REIFY_AR(random, fn_random, 1, 1), \
    REIFY_AR(max, fn_maxfn, 1, CALL_ARGUMENTS_LIMIT), \
    REIFY_AR(min, fn_minfn, 1, CALL_ARGUMENTS_LIMIT), \
    REIFY_AR(noteq, fn_noteq, 1, CALL_ARGUMENTS_LIMIT), \
    REIFY_AR(numeq, fn_numeq, 1, CALL_ARGUMENTS_LIMIT), \
    REIFY_AR(less, fn_less, 1, CALL_ARGUMENTS_LIMIT), \
    REIFY_AR(lesseq, fn_lesseq, 1, CALL_ARGUMENTS_LIMIT), \
    REIFY_AR(greater, fn_greater, 1, CALL_ARGUMENTS_LIMIT), \
    REIFY_AR(greater_eq, fn_greatereq, 1, CALL_ARGUMENTS_LIMIT), \
    REIFY_AR(plusp, fn_plusp, 1, 1), \
    REIFY_AR(minusp, fn_minusp, 1, 1), \
    REIFY_AR(zerop, fn_zerop, 1, 1), \
    REIFY_AR(oddp, fn_oddp, 1, 1), \
    REIFY_AR(evenp, fn_evenp, 1, 1), \
    REIFY_AR(integerp, fn_integerp, 1, 1), \
    REIFY_AR(numberp, fn_numberp, 1, 1), \
    REIFY_AR(floatfn, fn_floatfn, 1, 1), \
    REIFY_AR(floatp, fn_floatp, 1, 1), \
    REIFY_AR(sin, fn_sin, 1, 1), \
    REIFY_AR(cos, fn_cos, 1, 1), \
    REIFY_AR(tan, fn_tan, 1, 1), \
    REIFY_AR(asin, fn_asin, 1, 1), \
    REIFY_AR(acos, fn_acos, 1, 1), \
    REIFY_AR(atan, fn_atan, 1, 2), \
    REIFY_AR(sinh, fn_sinh, 1, 1), \
    REIFY_AR(cosh, fn_cosh, 1, 1), \
    REIFY_AR(tanh, fn_tanh, 1, 1), \
    REIFY_AR(exp, fn_exp, 1, 1), \
    REIFY_AR(sqrt, fn_sqrt, 1, 1), \
    REIFY_AR(log, fn_log, 1, 2), \
    REIFY_AR(expt, fn_expt, 2, 2), \
    REIFY_AR(ceiling, fn_ceiling, 1, 2), \
    REIFY_AR(floor, fn_floor, 1, 2), \
    REIFY_AR(truncate, fn_truncate, 1, 2), \
    REIFY_AR(round, fn_round, 1, 2),

#endif
