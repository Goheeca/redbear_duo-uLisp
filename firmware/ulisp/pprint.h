#ifndef __PPRINT_H
#define __PPRINT_H

#include "core.h"

// Pretty printer

extern const int PPINDENT;
extern const int PPWIDTH;

void pcount (char c);
int atomwidth (object *obj);
bool quoted (object *obj);
int subwidth (object *obj, int w);
int subwidthlist (object *form, int w);
void superprint (object *form, int lm, pfun_t pfun);

void supersub (object *form, int lm, int super, pfun_t pfun);
object *fn_pprint (object *args, object *env);
object *fn_pprintall (object *args, object *env);

#endif
