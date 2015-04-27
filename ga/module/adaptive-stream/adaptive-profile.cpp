/*
 * Copyright (c) 2015 Suliadi Marsetya
 *
 *
 */

#include <stdio.h>
#include <errno.h>
#include <list>
#include "ga-common.h"
#include "ga-conf.h"
#include "ga-module.h"
#include "adaptive-profile.h"

std::list<adaptiveProfile> aps;
bool compSort(adaptiveProfile &a, adaptiveProfile &b){ return a.loss < b.loss; }

bool parseConf()
{
	int num = ga_conf_readint("adaptive-num");
	ga_error("adaptive-stream: profile count = %d\n", num);
	char* str = "adaptive-profile";
	char tmp[100];
	while(num>0)
	{
		adaptiveProfile ap;
		sprintf(tmp,"%s%d",str,num);
		ap.loss = ga_conf_mapreadint(tmp,"loss");
		ap.rtt = ga_conf_mapreaddouble(tmp,"rtt");
		ap.jitter = ga_conf_mapreadint(tmp,"jitter");
		ap.crf = ga_conf_mapreadint(tmp,"crf");
		ap.bitrate = ga_conf_mapreaddouble(tmp,"bitrate");
		ap.fps = ga_conf_mapreaddouble(tmp,"fps");
		ap.vbv = ga_conf_mapreaddouble(tmp,"vbv");
		aps.push_back(ap);
		num--;
	}
	aps.sort(compSort);
	reportConf();
	return true;
}

void reportConf()
{
	std::list<adaptiveProfile>::iterator it;
	for(it=aps.begin();it!=aps.end();it++)
	{
		ga_error("adaptive-stream: profile loaded %d %d %d %d %d %d %d\n", it->loss, it->rtt, it->jitter, it->crf, it->fps, it->bitrate, it->vbv);
	}
}

ga_ioctl_reconfigure_t createParam(adaptiveProfile ap)
{
	ga_ioctl_reconfigure_t params;
	params.id = 0;
	params.bitrateKbps = ap.bitrate;
	params.bufsize = ap.vbv;
	params.crf = ap.crf;
	params.framerate_n = ap.fps;
	params.framerate_d = 1;
	params.width = 320;
	params.height = 240;
	return params;
}

ga_ioctl_reconfigure_t selectProfile()
{
	return createParam(*aps.begin());
}