#ifndef __SPI_H
#define __SPI_H

//#include <SPI.h>
#include "../hw/platform.h"

inline int spiread () { return SPI.transfer(0); }
inline void spiwrite (char c) { SPI.transfer(c); }

#endif
