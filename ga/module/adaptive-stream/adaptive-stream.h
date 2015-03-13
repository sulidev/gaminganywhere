/*
 *	adaptive-stream.cpp header file
 *
 */

#ifndef __ADAPTIVE_H__
#define __ADAPTIVE_H__

__declspec(dllexport) void adaptive_add_sink(RTPSink* sink);
__declspec(dllexport) void adaptive_remove_sink(RTPSink* sink);

#endif