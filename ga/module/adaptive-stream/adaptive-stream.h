/*
 *	adaptive-stream.cpp header file
 *
 */

#ifndef __ADAPTIVE_H__
#define __ADAPTIVE_H__

#include "ga-module.h"

MODULE MODULE_EXPORT void adaptive_add_sink(RTPSink* sink);
MODULE MODULE_EXPORT void adaptive_remove_sink(RTPSink* sink);

#endif