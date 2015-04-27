/*
 * Copyright (c) 2015 Suliadi Marsetya
 *
 *
 */

#ifndef __ADAPTIVEPROFILE_H__
#define __ADAPTIVEPROFILE_H__

class adaptiveProfile
{
public:
	int loss;
	int rtt;
	int jitter;
	int crf;
	int fps;
	int bitrate;
	int vbv;
};

bool parseConf();
void reportConf();
ga_ioctl_reconfigure_t selectProfile();

#endif