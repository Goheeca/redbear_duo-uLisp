#include "core.h"
#include "stream.h"
#include "read.h"
#include "eval.h"
#include "print.h"
#include "image.h"
#include "repl.h"
#include "list.h"
#include "error.h"
#include <string.h>

object Workspace[WORKSPACESIZE] WORDALIGNED;
char SymbolTable[SYMBOLTABLESIZE];

// Global variables

jmp_buf exception;
unsigned int Freespace = 0;
object *Freelist;
char *SymbolTop = SymbolTable;

object *tee;

unsigned int TraceFn[TRACEMAX];
unsigned int TraceDepth[TRACEMAX];

object *GlobalEnv;
object *GCStack = NULL;
object *GlobalString;
int GlobalStringIndex = 0;
char BreakLevel = 0;
char LastChar = 0;
char LastPrint = 0;
char PrintReadably = 1;

// Flags
volatile char Flags_;

// Lookup

int longsymbol (char *buffer) {
  char *p = SymbolTable;
  int i = 0;
  while (strcasecmp(p, buffer) != 0) {p = p + strlen(p) + 1; i++; }
  if (p == buffer) {
    // Add to symbol table?
    char *newtop = SymbolTop + strlen(p) + 1;
    if (SYMBOLTABLESIZE - (newtop - SymbolTable) < BUFFERSIZE) error2(0, PSTR("no room for long symbols"));
    SymbolTop = newtop;
  }
  if (i > 1535) error2(0, PSTR("Too many long symbols"));
  return i + 64000; // First number unused by radix40
}

char *lookupsymbol (symbol_t name) {
  char *p = SymbolTable;
  int i = name - 64000;
  while (i > 0 && p < SymbolTop) {p = p + strlen(p) + 1; i--; }
  if (p == SymbolTop) return NULL; else return p;
}

void deletesymbol (symbol_t name) {
  char *p = lookupsymbol(name);
  if (p == NULL) return;
  char *q = p + strlen(p) + 1;
  *p = '\0'; p++;
  while (q < SymbolTop) *(p++) = *(q++);
  SymbolTop = p;
}


// Set up workspace

void initworkspace () {
  Freelist = NULL;
  for (int i=WORKSPACESIZE-1; i>=0; i--) {
    object *obj = &Workspace[i];
    car_(obj) = NULL;
    cdr_(obj) = Freelist;
    Freelist = obj;
    Freespace++;
  }
}

object *myalloc () {
  if (Freespace == 0) error2(0, PSTR("no room"));
  object *temp = Freelist;
  Freelist = cdr_(Freelist);
  Freespace--;
  return temp;
}

// Make each type of object

object *number (int n) {
  object *ptr = myalloc();
  ptr->type = NUMBER_TAG;
  ptr->integer = n;
  return ptr;
}

object *makefloat (float f) {
  object *ptr = myalloc();
  ptr->type = FLOAT_TAG;
  ptr->single_float = f;
  return ptr;
}

object *character (char c) {
  object *ptr = myalloc();
  ptr->type = CHARACTER_TAG;
  ptr->integer = c;
  return ptr;
}

object *cons (object *arg1, object *arg2) {
  object *ptr = myalloc();
  ptr->car_ = arg1;
  ptr->cdr_ = arg2;
  return ptr;
}

object *symbol (symbol_t name) {
  object *ptr = myalloc();
  ptr->type = SYMBOL_TAG;
  ptr->name = name;
  return ptr;
}

object *newsymbol (symbol_t name) {
  for (int i=WORKSPACESIZE-1; i>=0; i--) {
    object *obj = &Workspace[i];
    if (obj->type == SYMBOL_TAG && obj->name == name) return obj;
  }
  return symbol(name);
}

object *stream (unsigned char streamtype, unsigned char address) {
  object *ptr = myalloc();
  ptr->type = STREAM_TAG;
  ptr->integer = streamtype<<8 | address;
  return ptr;
}

// Tracing

bool tracing (symbol_t name) {
  int i = 0;
  while (i < TRACEMAX) {
    if (TraceFn[i] == name) return i+1;
    i++;
  }
  return 0;
}

void trace (symbol_t name) {
  if (tracing(name)) error(TRACE, PSTR("already being traced"), symbol(name));
  int i = 0;
  while (i < TRACEMAX) {
    if (TraceFn[i] == 0) { TraceFn[i] = name; TraceDepth[i] = 0; return; }
    i++;
  }
  error2(TRACE, PSTR("already tracing 3 functions"));
}

void untrace (symbol_t name) {
  int i = 0;
  while (i < TRACEMAX) {
    if (TraceFn[i] == name) { TraceFn[i] = 0; return; }
    i++;
  }
  error(UNTRACE, PSTR("not tracing"), symbol(name));
}

// Helper functions

bool consp (object *x) {
  if (x == NULL) return false;
  unsigned int type = x->type;
  return type >= PAIR_TAG || type == ZERO_TAG;
}

bool atom (object *x) {
  if (x == NULL) return true;
  unsigned int type = x->type;
  return type < PAIR_TAG && type != ZERO_TAG;
}

bool listp (object *x) {
  if (x == NULL) return true;
  unsigned int type = x->type;
  return type >= PAIR_TAG || type == ZERO_TAG;
}

bool improperp (object *x) {
  if (x == NULL) return false;
  unsigned int type = x->type;
  return type < PAIR_TAG && type != ZERO_TAG;
}

int toradix40 (char ch) {
  if (ch == 0) return 0;
  if (ch >= '0' && ch <= '9') return ch-'0'+30;
  ch = ch | 0x20;
  if (ch >= 'a' && ch <= 'z') return ch-'a'+1;
  return -1; // Invalid
}

int fromradix40 (int n) {
  if (n >= 1 && n <= 26) return 'a'+n-1;
  if (n >= 30 && n <= 39) return '0'+n-30;
  return 0;
}

int pack40 (char *buffer) {
  return (((toradix40(buffer[0]) * 40) + toradix40(buffer[1])) * 40 + toradix40(buffer[2]));
}

bool valid40 (char *buffer) {
 return (toradix40(buffer[0]) >= 0 && toradix40(buffer[1]) >= 0 && toradix40(buffer[2]) >= 0);
}

int digitvalue (char d) {
  if (d>='0' && d<='9') return d-'0';
  d = d | 0x20;
  if (d>='a' && d<='f') return d-'a'+10;
  return 16;
}

char *symbolname (symbol_t x) {
  if (x < ENDFUNCTIONS) return lookupbuiltin(x);
  else if (x >= 64000) return lookupsymbol(x);
  char *buffer = SymbolTop;
  buffer[3] = '\0';
  for (int n=2; n>=0; n--) {
    buffer[n] = fromradix40(x % 40);
    x = x / 40;
  }
  return buffer;
}

int checkinteger (symbol_t name, object *obj) {
  if (!integerp_(obj)) error(name, PSTR("argument is not an integer"), obj);
  return obj->integer;
}

float checkintfloat (symbol_t name, object *obj){
  if (integerp_(obj)) return obj->integer;
  if (floatp_(obj)) return obj->single_float;
  error(name, notanumber, obj);
}

int checkchar (symbol_t name, object *obj) {
  if (!characterp_(obj)) error(name, PSTR("argument is not a character"), obj);
  return obj->integer;
}

int isstream (object *obj){
  if (!streamp_(obj)) error(0, PSTR("not a stream"), obj);
  return obj->integer;
}

int issymbol (object *obj, symbol_t n) {
  return symbolp_(obj) && obj->name == n;
}

void checkargs (symbol_t name, object *args) {
  int nargs = listlength(name, args);
  if (name >= ENDFUNCTIONS) error(0, PSTR("not valid here"), symbol(name));
  if (nargs<lookupmin(name)) error2(name, PSTR("has too few arguments"));
  if (nargs>lookupmax(name)) error2(name, PSTR("has too many arguments"));
}

int eq (object *arg1, object *arg2) {
  if (arg1 == arg2) return true;  // Same object
  if ((arg1 == nil_) || (arg2 == nil_)) return false;  // Not both values
  if (arg1->cdr_ != arg2->cdr_) return false;  // Different values
  if (symbolp_(arg1) && symbolp_(arg2)) return true;  // Same symbol
  if (integerp_(arg1) && integerp_(arg2)) return true;  // Same integer
  if (floatp_(arg1) && floatp_(arg2)) return true; // Same float
  if (characterp_(arg1) && characterp_(arg2)) return true;  // Same character
  return false;
}

int listlength (symbol_t name, object *list) {
  int length = 0;
  while (list != NULL) {
    if (improperp(list)) error2(name, notproper);
    list = cdr_(list);
    length++;
  }
  return length;
}

// Association lists

object *assoc (object *key, object *list) {
  while (list != NULL) {
    if (improperp(list)) error(ASSOC, notproper, list);
    object *pair = first_(list);
    if (!listp(pair)) error(ASSOC, PSTR("element is not a list"), pair);
    if (pair != NULL && eq(key,car_(pair))) return pair;
    list = cdr_(list);
  }
  return nil_;
}

object *delassoc (object *key, object **alist) {
  object *list = *alist;
  object *prev = NULL;
  while (list != NULL) {
    object *pair = first_(list);
    if (eq(key,car_(pair))) {
      if (prev == NULL) *alist = cdr_(list);
      else cdr_(prev) = cdr_(list);
      return key;
    }
    prev = list;
    list = cdr_(list);
  }
  return nil_;
}

// String utilities

void indent (int spaces, pfun_t pfun) {
  for (int i=0; i<spaces; i++) pfun(' ');
}

void buildstring (char ch, int *chars, object **head) {
  static object* tail;
  static uint8_t shift;
  if (*chars == 0) {
    shift = (sizeof(int)-1)*8;
    *chars = ch<<shift;
    object *cell = myalloc();
    if (*head == NULL) *head = cell; else tail->car_ = cell;
    cell->car_ = NULL;
    cell->integer = *chars;
    tail = cell;
  } else {
    shift = shift - 8;
    *chars = *chars | ch<<shift;
    tail->integer = *chars;
    if (shift == 0) *chars = 0;
  }
}

object *readstring (char delim, gfun_t gfun) {
  object *obj = myalloc();
  obj->type = STRING_TAG;
  int ch = gfun();
  if (ch == -1) return nil_;
  object *head = NULL;
  int chars = 0;
  while ((ch != delim) && (ch != -1)) {
    if (ch == '\\') ch = gfun();
    buildstring(ch, &chars, &head);
    ch = gfun();
  }
  obj->cdr_ = head;
  return obj;
}

int stringlength (object *form) {
  int length = 0;
  form = cdr_(form);
  while (form != NULL) {
    int chars = form->integer;
    for (int i=(sizeof(int)-1)*8; i>=0; i=i-8) {
      if (chars>>i & 0xFF) length++;
    }
    form = car_(form);
  }
  return length;
}

char nthchar (object *string, int n) {
  object *arg = cdr_(string);
  int top;
  if (sizeof(int) == 4) { top = n>>2; n = 3 - (n&3); }
  else { top = n>>1; n = 1 - (n&1); }
  for (int i=0; i<top; i++) {
    if (arg == NULL) return 0;
    arg = car_(arg);
  }
  if (arg == NULL) return 0;
  return (arg->integer)>>(n*8) & 0xFF;
}

// Lookup variable in environment

object *value (symbol_t n, object *env) {
  while (env != NULL) {
    object *pair = car_(env);
    if (pair != NULL && car_(pair)->name == n) return pair;
    env = cdr_(env);
  }
  return nil_;
}

object *findvalue (object *var, object *env) {
  symbol_t varname = var->name;
  object *pair = value(varname, env);
  if (pair == NULL) pair = value(varname, GlobalEnv);
  if (pair == NULL) error(0, PSTR("unknown variable"), var);
  return pair;
}

// In-place operations

object **place (symbol_t name, object *args, object *env) {
  if (atom(args)) return &cdr_(findvalue(args, env));
  object* function = first_(args);
  if (issymbol(function, CAR) || issymbol(function, FIRST)) {
    object *value = eval(second_(args), env);
    if (!listp(value)) error(name, PSTR("can't take car_"), value);
    return &car_(value);
  }
  if (issymbol(function, CDR) || issymbol(function, REST)) {
    object *value = eval(second_(args), env);
    if (!listp(value)) error(name, PSTR("can't take cdr_"), value);
    return &cdr_(value);
  }
  if (issymbol(function, NTH)) {
    int index = checkinteger(NTH, eval(second_(args), env));
    object *list = eval(third_(args), env);
    if (atom(list)) error(name, PSTR("second_ argument to nth is not a list"), list);
    while (index > 0) {
      list = cdr_(list);
      if (list == NULL) error2(name, PSTR("index to nth is out of range"));
      index--;
    }
    return &car_(list);
  }
  error2(name, PSTR("illegal place"));
  return nil_;
}

void checkanalogread (int pin) {
#if defined(_BOARD_MAPLE_MINI_H_)
  if (!(pin>=3 && pin<=11)) error(ANALOGREAD, PSTR("invalid pin"), number(pin));
#elif defined(_VARIANT_ARDUINO_STM32_)
  if (!((pin>=0 && pin<=7) || pin==16)) error(ANALOGREAD, PSTR("invalid pin"), number(pin));
#endif
}

void checkanalogwrite (int pin) {
#if defined(_BOARD_MAPLE_MINI_H_)
  if (!((pin>=3 && pin<=5) || (pin>=8 && pin<=11) || (pin>=15 && pin<=16) || (pin>=25 && pin<=27)))
    error(ANALOGWRITE, PSTR("invalid pin"), number(pin));
#elif defined(_VARIANT_ARDUINO_STM32_)
  if (!((pin>=0 && pin<=3) || (pin>=6 && pin<=10) || pin==16 || (pin>=22 && pin<=23)))
    error(ANALOGWRITE, PSTR("invalid pin"), number(pin));
#endif
}

// Core functions

object *fn_not (object *args, object *env) {
  (void) env;
  return (first_(args) == nil_) ? tee : nil_;
}

object *fn_cons (object *args, object *env) {
  (void) env;
  return cons(first_(args), second_(args));
}

object *fn_atom (object *args, object *env) {
  (void) env;
  return atom(first_(args)) ? tee : nil_;
}

object *fn_listp (object *args, object *env) {
  (void) env;
  return listp(first_(args)) ? tee : nil_;
}

object *fn_consp (object *args, object *env) {
  (void) env;
  return consp(first_(args)) ? tee : nil_;
}

object *fn_symbolp (object *args, object *env) {
  (void) env;
  object *arg = first_(args);
  return symbolp_(arg) ? tee : nil_;
}

object *fn_streamp (object *args, object *env) {
  (void) env;
  object *arg = first_(args);
  return streamp_(arg) ? tee : nil_;
}

object *fn_eq (object *args, object *env) {
  (void) env;
  return eq(first_(args), second_(args)) ? tee : nil_;
}

// System functions

object *fn_eval (object *args, object *env) {
  return eval(first_(args), env);
}

object *fn_globals (object *args, object *env) {
  (void) args;
  if (GlobalEnv == NULL) return nil_;
  return fn_mapcar_(cons(symbol(CAR),cons(GlobalEnv,nil_)), env);
}

object *fn_locals (object *args, object *env) {
  (void) args;
  return env;
}

object *fn_makunbound (object *args, object *env) {
  (void) env;
  object *key = first_(args);
  deletesymbol(key->name);
  return (delassoc(key, &GlobalEnv) != NULL) ? tee : nil_;
}

object *fn_break (object *args, object *env) {
  (void) args;
  pfstring(PSTR("\rBreak!\r"), pserial);
  BreakLevel++;
  repl(env);
  BreakLevel--;
  return nil_;
}

object *fn_read (object *args, object *env) {
  (void) env;
  gfun_t gfun = gstreamfun(args);
  return read(gfun);
}

object *fn_prin1 (object *args, object *env) {
  (void) env;
  object *obj = first_(args);
  pfun_t pfun = pstreamfun(cdr_(args));
  printobject(obj, pfun);
  return obj;
}

object *fn_print (object *args, object *env) {
  (void) env;
  object *obj = first_(args);
  pfun_t pfun = pstreamfun(cdr_(args));
  pln(pfun);
  printobject(obj, pfun);
  (pfun)(' ');
  return obj;
}

object *fn_princ (object *args, object *env) {
  (void) env;
  object *obj = first_(args);
  pfun_t pfun = pstreamfun(cdr_(args));
  char temp = PrintReadably;
  PrintReadably = 0;
  printobject(obj, pfun);
  PrintReadably = temp;
  return obj;
}

object *fn_terpri (object *args, object *env) {
  (void) env;
  pfun_t pfun = pstreamfun(args);
  pln(pfun);
  return nil_;
}

object *fn_readbyte (object *args, object *env) {
  (void) env;
  gfun_t gfun = gstreamfun(args);
  int c = gfun();
  return (c == -1) ? nil_ : number(c);
}

object *fn_readline (object *args, object *env) {
  (void) env;
  gfun_t gfun = gstreamfun(args);
  return readstring('\n', gfun);
}

object *fn_writebyte (object *args, object *env) {
  (void) env;
  int value = checkinteger(WRITEBYTE, first_(args));
  pfun_t pfun = pstreamfun(cdr_(args));
  (pfun)(value);
  return nil_;
}

object *fn_writestring (object *args, object *env) {
  (void) env;
  object *obj = first_(args);
  pfun_t pfun = pstreamfun(cdr_(args));
  char temp = PrintReadably;
  PrintReadably = 0;
  printstring(obj, pfun);
  PrintReadably = temp;
  return nil_;
}

object *fn_writeline (object *args, object *env) {
  (void) env;
  object *obj = first_(args);
  pfun_t pfun = pstreamfun(cdr_(args));
  char temp = PrintReadably;
  PrintReadably = 0;
  printstring(obj, pfun);
  pln(pfun);
  PrintReadably = temp;
  return nil_;
}

object *fn_saveimage (object *args, object *env) {
  if (args != NULL) args = eval(first_(args), env);
  return number(saveimage(args));
}

object *fn_loadimage (object *args, object *env) {
  (void) env;
  if (args != NULL) args = first_(args);
  return number(loadimage(args));
}

object *fn_cls (object *args, object *env) {
  (void) args, (void) env;
  pserial(12);
  return nil_;
}

// Setup

void initenv () {
  GlobalEnv = NULL;
  tee = symbol(TEE);
}
