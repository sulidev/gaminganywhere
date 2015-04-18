/*
 * Copyright (c) 2015 Suliadi Marsetya
 *
 *
 */

#ifndef __ADAPTIVEPROFILE_H__
#define __ADAPTIVEPROFILE_H__

#include <list>

class adaptiveProfile
{
public:
	int packetLoss;
	float rtt;
	int jitter;
	int crf;
	float fps;
	long bitrate;
	long vbv;
};

bool parseConf(char*);
void reportConf();

#endif