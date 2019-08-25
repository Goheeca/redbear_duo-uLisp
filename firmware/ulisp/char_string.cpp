#include "char_string.h"
#include "read.h"
#include "eval.h"
#include "print.h"
#include "error.h"

// Characters

object *fn_char (object *args, object *env) {
  (void) env;
  object *arg = first_(args);
  if (!stringp_(arg)) error(CHAR, notastring, arg);
  char c = nthchar(arg, checkinteger(CHAR, second_(args)));
  if (c == 0) error2(CHAR, PSTR("index out of range"));
  return character(c);
}

object *fn_charcode (object *args, object *env) {
  (void) env;
  return number(checkchar(CHARCODE, first_(args)));
}

object *fn_codechar (object *args, object *env) {
  (void) env;
  return character(checkinteger(CODECHAR, first_(args)));
}

object *fn_characterp (object *args, object *env) {
  (void) env;
  return characterp_(first_(args)) ? tee : nil_;
}

// Strings

object *fn_stringp (object *args, object *env) {
  (void) env;
  return stringp_(first_(args)) ? tee : nil_;
}

bool stringcompare (symbol_t name, object *args, bool lt, bool gt, bool eq) {
  object *arg1 = first_(args); if (!stringp_(arg1)) error(name, notastring, arg1);
  object *arg2 = second_(args); if (!stringp_(arg2)) error(name, notastring, arg2);
  arg1 = cdr_(arg1);
  arg2 = cdr_(arg2);
  while ((arg1 != NULL) || (arg2 != NULL)) {
    if (arg1 == NULL) return lt;
    if (arg2 == NULL) return gt;
    if (arg1->integer < arg2->integer) return lt;
    if (arg1->integer > arg2->integer) return gt;
    arg1 = car_(arg1);
    arg2 = car_(arg2);
  }
  return eq;
}

object *fn_stringeq (object *args, object *env) {
  (void) env;
  return stringcompare(STRINGEQ, args, false, false, true) ? tee : nil_;
}

object *fn_stringless (object *args, object *env) {
  (void) env;
  return stringcompare(STRINGLESS, args, true, false, false) ? tee : nil_;
}

object *fn_stringgreater (object *args, object *env) {
  (void) env;
  return stringcompare(STRINGGREATER, args, false, true, false) ? tee : nil_;
}

object *fn_sort (object *args, object *env) {
  if (first_(args) == NULL) return nil_;
  object *list = cons(nil_,first_(args));
  push_(list,GCStack);
  object *predicate = second_(args);
  object *compare = cons(NULL, cons(NULL, NULL));
  object *ptr = cdr_(list);
  while (cdr_(ptr) != NULL) {
    object *go = list;
    while (go != ptr) {
      car_(compare) = car_(cdr_(ptr));
      car_(cdr_(compare)) = car_(cdr_(go));
      if (apply(SORT, predicate, compare, env)) break;
      go = cdr_(go);
    }
    if (go != ptr) {
      object *obj = cdr_(ptr);
      cdr_(ptr) = cdr_(obj);
      cdr_(obj) = cdr_(go);
      cdr_(go) = obj;
    } else ptr = cdr_(ptr);
  }
  pop_(GCStack);
  return cdr_(list);
}

object *fn_stringfn (object *args, object *env) {
  (void) env;
  object *arg = first_(args);
  int type = arg->type;
  if (type == STRING_TAG) return arg;
  object *obj = myalloc();
  obj->type = STRING_TAG;
  if (type == CHARACTER_TAG) {
    object *cell = myalloc();
    cell->car_ = NULL;
    uint8_t shift = (sizeof(int)-1)*8;
    cell->integer = (arg->integer)<<shift;
    obj->cdr_ = cell;
  } else if (type == SYMBOL_TAG) {
    char *s = symbolname(arg->name);
    char ch = *s++;
    object *head = NULL;
    int chars = 0;
    while (ch) {
      if (ch == '\\') ch = *s++;
      buildstring(ch, &chars, &head);
      ch = *s++;
    }
    obj->cdr_ = head;
  } else error(STRINGFN, PSTR("can't convert to string"), arg);
  return obj;
}

object *fn_concatenate (object *args, object *env) {
  (void) env;
  object *arg = first_(args);
  symbol_t name = arg->name;
  if (name != STRINGFN) error2(CONCATENATE, PSTR("only supports strings"));
  args = cdr_(args);
  object *result = myalloc();
  result->type = STRING_TAG;
  object *head = NULL;
  int chars = 0;
  while (args != NULL) {
    object *obj = first_(args);
    if (obj->type != STRING_TAG) error(CONCATENATE, notastring, obj);
    obj = cdr_(obj);
    while (obj != NULL) {
      int quad = obj->integer;
      while (quad != 0) {
         char ch = quad>>((sizeof(int)-1)*8) & 0xFF;
         buildstring(ch, &chars, &head);
         quad = quad<<8;
      }
      obj = car_(obj);
    }
    args = cdr_(args);
  }
  result->cdr_ = head;
  return result;
}

object *fn_subseq (object *args, object *env) {
  (void) env;
  object *arg = first_(args);
  if (!stringp_(arg)) error(SUBSEQ, notastring, arg);
  int start = checkinteger(SUBSEQ, second_(args));
  int end;
  args = cddr_(args);
  if (args != NULL) end = checkinteger(SUBSEQ, car_(args)); else end = stringlength(arg);
  object *result = myalloc();
  result->type = STRING_TAG;
  object *head = NULL;
  int chars = 0;
  for (int i=start; i<end; i++) {
    char ch = nthchar(arg, i);
    if (ch == 0) error2(SUBSEQ, PSTR("index out of range"));
    buildstring(ch, &chars, &head);
  }
  result->cdr_ = head;
  return result;
}

int gstr () {
  if (LastChar) {
    char temp = LastChar;
    LastChar = 0;
    return temp;
  }
  char c = nthchar(GlobalString, GlobalStringIndex++);
  return (c != 0) ? c : '\n'; // -1?
}

object *fn_readfromstring (object *args, object *env) {
  (void) env;
  object *arg = first_(args);
  if (!stringp_(arg)) error(READFROMSTRING, notastring, arg);
  GlobalString = arg;
  GlobalStringIndex = 0;
  return read(gstr);
}

void pstr (char c) {
  buildstring(c, &GlobalStringIndex, &GlobalString);
}

object *fn_princtostring (object *args, object *env) {
  (void) env;
  object *arg = first_(args);
  object *obj = myalloc();
  obj->type = STRING_TAG;
  GlobalString = NULL;
  GlobalStringIndex = 0;
  char temp = PrintReadably;
  PrintReadably = 0;
  printobject(arg, pstr);
  PrintReadably = temp;
  obj->cdr_ = GlobalString;
  return obj;
}

object *fn_prin1tostring (object *args, object *env) {
  (void) env;
  object *arg = first_(args);
  object *obj = myalloc();
  obj->type = STRING_TAG;
  GlobalString = NULL;
  GlobalStringIndex = 0;
  printobject(arg, pstr);
  obj->cdr_ = GlobalString;
  return obj;
}
