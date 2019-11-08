#ifndef __LIST_H
#define __LIST_H

#include "core.h"

// List functions

object *fn_car_ (object *args, object *env);
object *fn_cdr_ (object *args, object *env);
object *fn_caar (object *args, object *env);
object *fn_cadr (object *args, object *env);
object *fn_cdar (object *args, object *env);
object *fn_cddr_ (object *args, object *env);
object *fn_caaar (object *args, object *env);
object *fn_caadr (object *args, object *env);
object *fn_cadar (object *args, object *env);
object *fn_caddr (object *args, object *env);
object *fn_cdaar (object *args, object *env);
object *fn_cdadr (object *args, object *env);
object *fn_cddar (object *args, object *env);
object *fn_cdddr (object *args, object *env);
object *fn_length (object *args, object *env);
object *fn_list (object *args, object *env);
object *fn_reverse (object *args, object *env);
object *fn_nth (object *args, object *env);
object *fn_assoc (object *args, object *env);
object *fn_member (object *args, object *env);
object *fn_apply (object *args, object *env);
object *fn_funcall (object *args, object *env);
object *fn_append (object *args, object *env);
object *fn_mapc (object *args, object *env);
object *fn_mapcar_ (object *args, object *env);
object *fn_mapcan (object *args, object *env);

#include "frozen/lookup.h"
#define FREEZE_LI(no, str) \
    FREEZE(list_, no, str)
#define REIFY_LI(no, fn, min_arg, max_arg) \
    REIFY(list_, no, fn, min_arg, max_arg)

#define FREEZE_LIST \
    FREEZE_LI(car_, "car"); \
    FREEZE_LI(first_, "first"); \
    FREEZE_LI(cdr_, "cdr"); \
    FREEZE_LI(rest, "rest"); \
    FREEZE_LI(caar, "caar"); \
    FREEZE_LI(cadr, "cadr"); \
    FREEZE_LI(second_, "second"); \
    FREEZE_LI(cdar, "cdar"); \
    FREEZE_LI(cddr_, "cddr"); \
    FREEZE_LI(caaar, "caaar"); \
    FREEZE_LI(caadr, "caadr"); \
    FREEZE_LI(cadar, "cadar"); \
    FREEZE_LI(caddr, "caddr"); \
    FREEZE_LI(third_, "third"); \
    FREEZE_LI(cdaar, "cdaar"); \
    FREEZE_LI(cdadr, "cdadr"); \
    FREEZE_LI(cddar, "cddar"); \
    FREEZE_LI(cdddr, "cdddr"); \
    FREEZE_LI(length, "length"); \
    FREEZE_LI(list, "list"); \
    FREEZE_LI(reverse, "reverse"); \
    FREEZE_LI(nth, "nth"); \
    FREEZE_LI(assoc, "assoc"); \
    FREEZE_LI(member, "member"); \
    FREEZE_LI(apply, "apply"); \
    FREEZE_LI(funcall, "funcall"); \
    FREEZE_LI(append, "append"); \
    FREEZE_LI(mapc, "mapc"); \
    FREEZE_LI(mapcar_, "mapcar"); \
    FREEZE_LI(mapcan, "mapcan");

#define REIFY_LIST \
    REIFY_LI(car_, fn_car_, 1, 1), \
    REIFY_LI(first_, fn_car_, 1, 1), \
    REIFY_LI(cdr_, fn_cdr_, 1, 1), \
    REIFY_LI(rest, fn_cdr_, 1, 1), \
    REIFY_LI(caar, fn_caar, 1, 1), \
    REIFY_LI(cadr, fn_cadr, 1, 1), \
    REIFY_LI(second_, fn_cadr, 1, 1), \
    REIFY_LI(cdar, fn_cdar, 1, 1), \
    REIFY_LI(cddr_, fn_cddr_, 1, 1), \
    REIFY_LI(caaar, fn_caaar, 1, 1), \
    REIFY_LI(caadr, fn_caadr, 1, 1), \
    REIFY_LI(cadar, fn_cadar, 1, 1), \
    REIFY_LI(caddr, fn_caddr, 1, 1), \
    REIFY_LI(third_, fn_caddr, 1, 1), \
    REIFY_LI(cdaar, fn_cdaar, 1, 1), \
    REIFY_LI(cdadr, fn_cdadr, 1, 1), \
    REIFY_LI(cddar, fn_cddar, 1, 1), \
    REIFY_LI(cdddr, fn_cdddr, 1, 1), \
    REIFY_LI(length, fn_length, 1, 1), \
    REIFY_LI(list, fn_list, 0, CALL_ARGUMENTS_LIMIT), \
    REIFY_LI(reverse, fn_reverse, 1, 1), \
    REIFY_LI(nth, fn_nth, 2, 2), \
    REIFY_LI(assoc, fn_assoc, 2, 2), \
    REIFY_LI(member, fn_member, 2, 2), \
    REIFY_LI(apply, fn_apply, 2, CALL_ARGUMENTS_LIMIT), \
    REIFY_LI(funcall, fn_funcall, 1, CALL_ARGUMENTS_LIMIT), \
    REIFY_LI(append, fn_append, 0, CALL_ARGUMENTS_LIMIT), \
    REIFY_LI(mapc, fn_mapc, 2, 3), \
    REIFY_LI(mapcar_, fn_mapcar_, 2, 3), \
    REIFY_LI(mapcan, fn_mapcan, 2, 3),

#endif
