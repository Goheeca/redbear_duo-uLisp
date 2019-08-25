#include "tail_call.h"
#include "eval.h"

// Tail-recursive forms

object *tf_progn (object *args, object *env) {
  if (args == NULL) return nil_;
  object *more = cdr_(args);
  while (more != NULL) {
    object *result = eval(car_(args),env);
    if (tstflag_(RETURNFLAG)) return result;
    args = more;
    more = cdr_(args);
  }
  return car_(args);
}

object *tf_return (object *args, object *env) {
  setflag_(RETURNFLAG);
  return tf_progn(args, env);
}

object *tf_if (object *args, object *env) {
  if (args == NULL || cdr_(args) == NULL) error2(IF, PSTR("missing argument(s)"));
  if (eval(first_(args), env) != nil_) return second_(args);
  args = cddr_(args);
  return (args != NULL) ? first_(args) : nil_;
}

object *tf_cond (object *args, object *env) {
  while (args != NULL) {
    object *clause = first_(args);
    if (!consp(clause)) error(COND, PSTR("illegal clause"), clause);
    object *test = eval(first_(clause), env);
    object *forms = cdr_(clause);
    if (test != nil_) {
      if (forms == NULL) return test; else return tf_progn(forms, env);
    }
    args = cdr_(args);
  }
  return nil_;
}

object *tf_when (object *args, object *env) {
  if (args == NULL) error2(WHEN, noargument);
  if (eval(first_(args), env) != nil_) return tf_progn(cdr_(args),env);
  else return nil_;
}

object *tf_unless (object *args, object *env) {
  if (args == NULL) error2(UNLESS, noargument);
  if (eval(first_(args), env) != nil_) return nil_;
  else return tf_progn(cdr_(args),env);
}

object *tf_case (object *args, object *env) {
  object *test = eval(first_(args), env);
  args = cdr_(args);
  while (args != NULL) {
    object *clause = first_(args);
    if (!consp(clause)) error(CASE, PSTR("illegal clause"), clause);
    object *key = car_(clause);
    object *forms = cdr_(clause);
    if (consp(key)) {
      while (key != NULL) {
        if (eq(test,car_(key))) return tf_progn(forms, env);
        key = cdr_(key);
      }
    } else if (eq(test,key) || eq(key,tee)) return tf_progn(forms, env);
    args = cdr_(args);
  }
  return nil_;
}

object *tf_and (object *args, object *env) {
  if (args == NULL) return tee;
  object *more = cdr_(args);
  while (more != NULL) {
    if (eval(car_(args), env) == NULL) return nil_;
    args = more;
    more = cdr_(args);
  }
  return car_(args);
}

object *tf_or (object *args, object *env) {
  while (args != NULL) {
    if (eval(car_(args), env) != NULL) return car_(args);
    args = cdr_(args);
  }
  return nil_;
}
