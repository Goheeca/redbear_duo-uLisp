#ifndef __NOTE_H
#define __NOTE_H

#include "../core.h"

// Note

void tone (int pin, int note);
void noTone (int pin);

void playnote (int pin, int note, int octave);
void nonote (int pin);

#endif
