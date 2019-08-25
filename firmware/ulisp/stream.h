#ifndef __STREAM_H
#define __STREAM_H

#include "core.h"

// Streams

gfun_t gstreamfun (object *args);
pfun_t pstreamfun (object *args);

enum stream { SERIALSTREAM, I2CSTREAM, SPISTREAM, SDSTREAM };

#endif
