#ifndef __PLATFORM_H
#define __PLATFORM_H

//#include "../pgmspace_dummy.h"
#define PGM_P  const char *

#define bitRead(v, n) (((v) >> (n)) & 1)

#define yield()

#include "Particle.h"

typedef int BitOrder;
#define SPISettings __SPISettings

#define WiringPinMode PinMode

#endif
