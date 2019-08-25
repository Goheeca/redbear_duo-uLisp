#include "list.h"
#include "error.h"
#include "eval.h"

// List functions

object *fn_car_ (object *args, object *env) {
  (void) env;
  return carx(first_(args));
}

object *fn_cdr_ (object *args, object *env) {
  (void) env;
  return cdrx(first_(args));
}

object *fn_caar (object *args, object *env) {
  (void) env;
  return carx(carx(first_(args)));
}

object *fn_cadr (object *args, object *env) {
  (void) env;
  return carx(cdrx(first_(args)));
}

object *fn_cdar (object *args, object *env) {
  (void) env;
  return cdrx(carx(first_(args)));
}

object *fn_cddr_ (object *args, object *env) {
  (void) env;
  return cdrx(cdrx(first_(args)));
}

object *fn_caaar (object *args, object *env) {
  (void) env;
  return carx(carx(carx(first_(args))));
}

object *fn_caadr (object *args, object *env) {
  (void) env;
  return carx(carx(cdrx(first_(args))));
}

object *fn_cadar (object *args, object *env) {
  (void) env;
  return carx(cdrx(carx(first_(args))));
}

object *fn_caddr (object *args, object *env) {
  (void) env;
  return carx(cdrx(cdrx(first_(args))));
}

object *fn_cdaar (object *args, object *env) {
  (void) env;
  return cdrx(carx(carx(first_(args))));
}

object *fn_cdadr (object *args, object *env) {
  (void) env;
  return cdrx(carx(cdrx(first_(args))));
}

object *fn_cddar (object *args, object *env) {
  (void) env;
  return cdrx(cdrx(carx(first_(args))));
}

object *fn_cdddr (object *args, object *env) {
  (void) env;
  return cdrx(cdrx(cdrx(first_(args))));
}

object *fn_length (object *args, object *env) {
  (void) env;
  object *arg = first_(args);
  if (listp(arg)) return number(listlength(LENGTH, arg));
  if (!stringp_(arg)) error(LENGTH, PSTR("argument is not a list or string"), arg);
  return number(stringlength(arg));
}

object *fn_list (object *args, object *env) {
  (void) env;
  return args;
}

object *fn_reverse (object *args, object *env) {
  (void) env;
  object *list = first_(args);
  object *result = NULL;
  while (list != NULL) {
    if (improperp(list)) error(REVERSE, notproper, list);
    push_(first_(list),result);
    list = cdr_(list);
  }
  return result;
}

object *fn_nth (object *args, object *env) {
  (void) env;
  int n = checkinteger(NTH, first_(args));
  object *list = second_(args);
  while (list != NULL) {
    if (improperp(list)) error(NTH, notproper2, list);
    if (n == 0) return car_(list);
    list = cdr_(list);
    n--;
  }
  return nil_;
}

object *fn_assoc (object *args, object *env) {
  (void) env;
  object *key = first_(args);
  object *list = second_(args);
  return assoc(key,list);
}

object *fn_member (object *args, object *env) {
  (void) env;
  object *item = first_(args);
  object *list = second_(args);
  while (list != NULL) {
    if (improperp(list)) error(MEMBER, notproper, list);
    if (eq(item,car_(list))) return list;
    list = cdr_(list);
  }
  return nil_;
}

object *fn_apply (object *args, object *env) {
  object *previous = NULL;
  object *last = args;
  while (cdr_(last) != NULL) {
    previous = last;
    last = cdr_(last);
  }
  object *arg = car_(last);
  if (!listp(arg)) error(APPLY, PSTR("last argument is not a list"), arg);
  cdr_(previous) = arg;
  return apply(APPLY, first_(args), cdr_(args), env);
}

object *fn_funcall (object *args, object *env) {
  return apply(FUNCALL, first_(args), cdr_(args), env);
}

object *fn_append (object *args, object *env) {
  (void) env;
  object *head = NULL;
  object *tail;
  while (args != NULL) {
    object *list = first_(args);
    if (!listp(list)) error(APPEND, notalist, list);
    while (consp(list)) {
      object *obj = cons(car_(list), cdr_(list));
      if (head == NULL) head = obj;
      else cdr_(tail) = obj;
      tail = obj;
      list = cdr_(list);
      if (cdr_(args) != NULL && improperp(list)) error(APPEND, notproper, first_(args));
    }
    args = cdr_(args);
  }
  return head;
}

object *fn_mapc (object *args, object *env) {
  object *function = first_(args);
  object *list1 = second_(args);
  object *result = list1;
  object *list2 = cddr_(args);
  if (list2 != NULL) {
    list2 = car_(list2);
    object *result2 = list2;
    while (list1 != NULL && list2 != NULL) {
      if (improperp(list1)) error(MAPC, notproper2, result);
      if (improperp(list2)) error(MAPC, notproper3, result2);
      apply(MAPC, function, cons(car_(list1),cons(car_(list2),NULL)), env);
      list1 = cdr_(list1); list2 = cdr_(list2);
    }
  } else {
    while (list1 != NULL) {
      if (improperp(list1)) error(MAPC, notproper2, result);
      apply(MAPC, function, cons(car_(list1),NULL), env);
      list1 = cdr_(list1);
    }
  }
  return result;
}

object *fn_mapcar_ (object *args, object *env) {
  object *function = first_(args);
  object *list1 = second_(args);
  object *result = list1;
  object *list2 = cddr_(args);
  object *head = cons(NULL, NULL);
  push_(head,GCStack);
  object *tail = head;
  if (list2 != NULL) {
    list2 = car_(list2);
    object *result2 = list2;
    while (list1 != NULL && list2 != NULL) {
      if (improperp(list1)) error(MAPCAR, notproper2, result);
      if (improperp(list2)) error(MAPCAR, notproper3, result2);
      object *result = apply(MAPCAR, function, cons(car_(list1), cons(car_(list2),NULL)), env);
      object *obj = cons(result,NULL);
      cdr_(tail) = obj;
      tail = obj;
      list1 = cdr_(list1); list2 = cdr_(list2);
    }
  } else if (list1 != NULL) {
    while (list1 != NULL) {
      if (improperp(list1)) error(MAPCAR, notproper2, result);
      object *result = apply(MAPCAR, function, cons(car_(list1),NULL), env);
      object *obj = cons(result,NULL);
      cdr_(tail) = obj;
      tail = obj;
      list1 = cdr_(list1);
    }
  }
  pop_(GCStack);
  return cdr_(head);
}

object *fn_mapcan (object *args, object *env) {
  object *function = first_(args);
  object *list1 = second_(args);
  object *result = list1;
  object *list2 = cddr_(args);
  object *head = cons(NULL, NULL);
  push_(head,GCStack);
  object *tail = head;
  if (list2 != NULL) {
    list2 = car_(list2);
    object *result2 = list2;
    while (list1 != NULL && list2 != NULL) {
      if (improperp(list1)) error(MAPCAN, notproper2, result);
      if (improperp(list2)) error(MAPCAN, notproper3, result2);
      object *result = apply(MAPCAN, function, cons(car_(list1), cons(car_(list2),NULL)), env);
      while ((unsigned int)result >= PAIR_TAG) {
        cdr_(tail) = result;
        tail = result;
        result = cdr_(result);
      }
      if (cdr_(list1) != NULL && cdr_(list2) != NULL && result != NULL) error2(MAPCAN, PSTR("result is not a proper list"));
      list1 = cdr_(list1); list2 = cdr_(list2);
    }
  } else if (list1 != NULL) {
    while (list1 != NULL) {
      if (improperp(list1)) error(MAPCAN, notproper2, result);
      object *result = apply(MAPCAN, function, cons(car_(list1),NULL), env);
      while ((unsigned int)result >= PAIR_TAG) {
        cdr_(tail) = result;
        tail = result;
        result = cdr_(result);
      }
      if (cdr_(list1) != NULL && result != NULL) error2(MAPCAN, PSTR("result is not a proper list"));
      list1 = cdr_(list1);
    }
  }
  pop_(GCStack);
  return cdr_(head);
}
