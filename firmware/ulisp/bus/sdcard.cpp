#include "sdcard.h"

#if defined(sdcardsupport)
#include <SD.h>

File SDpfile, SDgfile;

inline int SDread () {
  if (LastChar) {
    char temp = LastChar;
    LastChar = 0;
    return temp;
  }
  return SDgfile.read();
}

inline void SDwrite (char c) { SDpfile.write(c); }

#endif
