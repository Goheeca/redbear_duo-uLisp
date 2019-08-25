#ifndef __IMAGE_H
#define __IMAGE_H

#include "core.h"

// Compact image

void movepointer (object *from, object *to);
int compactimage (object **arg);

// Save-image and load-image

int saveimage (object *arg);
int loadimage (object *arg);
void autorunimage ();

#endif
