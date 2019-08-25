#ifndef __SDCARD_H
#define __SDCARD_H

#include "../core.h"

#if defined(sdcardsupport)
File SDpfile, SDgfile;
inline int SDread ();
inline void SDwrite (char c);
#endif


#endif
