/*
 * Copyright (c) 2015 Suliadi Marsetya
 *
 *
 */

#ifndef __ADAPTIVEPROFILE_H__
#define __ADAPTIVEPROFILE_H__

class Profile{
public:
	int crf;
	int bitrate;
	int vbv;
	int fps;
};

class Stat{
public:
	int loss;
	int delay;
	int jitter;
};

void profileMain();
ga_ioctl_reconfigure_t selectProfile(int, int, int);

#endif