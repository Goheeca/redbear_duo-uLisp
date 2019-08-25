#ifndef __DEFINITIONS_H
#define __DEFINITIONS_H

#include <stdint.h>

// C Macros

#define nil_                NULL
#define car_(x)             (((object *) (x))->car_)
#define cdr_(x)             (((object *) (x))->cdr_)

#define first_(x)           (((object *) (x))->car_)
#define second_(x)          (car_(cdr_(x)))
#define cddr_(x)            (cdr_(cdr_(x)))
#define third_(x)           (car_(cdr_(cdr_(x))))

#define push_(x, y)         ((y) = cons((x),(y)))
#define pop_(y)             ((y) = cdr_(y))

#define integerp_(x)        ((x) != NULL && (x)->type == NUMBER_TAG)
#define floatp_(x)          ((x) != NULL && (x)->type == FLOAT_TAG)
#define symbolp_(x)         ((x) != NULL && (x)->type == SYMBOL_TAG)
#define stringp_(x)         ((x) != NULL && (x)->type == STRING_TAG)
#define characterp_(x)      ((x) != NULL && (x)->type == CHARACTER_TAG)
#define streamp_(x)         ((x) != NULL && (x)->type == STREAM_TAG)

#define mark_(x)            (car_(x) = (object *)(((uintptr_t)(car_(x))) | MARKBIT))
#define unmark_(x)          (car_(x) = (object *)(((uintptr_t)(car_(x))) & ~MARKBIT))
#define marked_(x)          ((((uintptr_t)(car_(x))) & MARKBIT) != 0)
#define MARKBIT            1

#define setflag_(x)         (Flags_ = Flags_ | 1<<(x))
#define clrflag_(x)         (Flags_ = Flags_ & ~(1<<(x)))
#define tstflag_(x)         (Flags_ & 1<<(x))

// Constants

const int TRACEMAX = 3; // Number of traced functions
enum type { ZERO_TAG=0, SYMBOL_TAG=2, NUMBER_TAG=4, STREAM_TAG=6, CHARACTER_TAG=8, FLOAT_TAG=10, STRING_TAG=12, PAIR_TAG=14 };  // STRING_TAG and PAIR_TAG must be last
enum token { UNUSED, BRA, KET, QUO, DOT };

// Typedefs

typedef unsigned int symbol_t;

typedef struct sobject {
  union {
    struct {
      sobject *car_;
      sobject *cdr_;
    };
    struct {
      unsigned int type;
      union {
        symbol_t name;
        int integer;
        float single_float;
      };
    };
  };
} object;

typedef object *(*fn_ptr_type)(object *, object *);

typedef struct {
  const char *string;
  fn_ptr_type fptr;
  uint8_t min;
  uint8_t max;
} tbl_entry_t;

typedef int (*gfun_t)();
typedef void (*pfun_t)(char);

#endif
