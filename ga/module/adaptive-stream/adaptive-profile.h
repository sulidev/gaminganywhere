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
	int crf;
	int fps;
	int bitrate;
	int vbv;
};

void profileMain();
ga_ioctl_reconfigure_t selectProfile();

#endif