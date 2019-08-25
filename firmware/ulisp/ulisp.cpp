#include "ulisp.h"

void ulisp_setup () {
  initworkspace();
  initenv();
  initsleep();
  pfstring(PSTR("uLisp 2.8 "), pserial); pln(pserial);
}

void ulisp_reset () {
  // Come here after error
  for (int i=0; i<TRACEMAX; i++) TraceDepth[i] = 0;
  #if defined(sdcardsupport)
  SDpfile.close(); SDgfile.close();
  #endif
  #if defined(lisplibrary)
  if (!tstflag_(LIBRARYLOADED)) { setflag_(LIBRARYLOADED); loadfromlibrary(NULL); }
  #endif
}

void ulisp_repl () {
  repl(NULL);
}
