/*
 * Copyright (c) 2015 Suliadi Marsetya
 *
 *
 */

#ifndef __ADAPTIVE_H__
#define __ADAPTIVE_H__

#include "ga-module.h"

#if 1
MODULE MODULE_EXPORT void adaptive_add_sink(RTPSink* sink);
MODULE MODULE_EXPORT void adaptive_remove_sink(RTPSink* sink);
#else
void adaptive_add_sink(RTPSink* sink);
void adaptive_remove_sink(RTPSink* sink);
#endif

#endif