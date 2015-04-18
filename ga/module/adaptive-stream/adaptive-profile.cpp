/*
 * Copyright (c) 2015 Suliadi Marsetya
 *
 *
 */

#include <stdio.h>
#include <errno.h>
#include "ga-common.h"
#include "adaptive-profile.h"

std::list<adaptiveProfile> aps;
bool compSort(adaptiveProfile &a, adaptiveProfile &b){ return a.packetLoss < b.packetLoss; }

bool parseConf(char* filename)
{
	FILE* fp;
	char* buf;
	fp = fopen(filename,"r");
	if(fp==NULL)
	{
		ga_error("adaptive-stream: Configuration file: %s not found %s\n", filename, strerror(errno));
		return false;
	}

	while(1)
	{
		fgets(buf,128,fp);
		if(buf[0]!='#')
		{
			adaptiveProfile ap;
			sscanf_s(buf,"%d %f %d %d %f %l %l", &ap.packetLoss, &ap.rtt, &ap.jitter, &ap.crf, &ap.fps, &ap.bitrate, &ap.vbv);
			aps.push_back(ap);
		}
		if(feof(fp))
			break;
	}
	fclose(fp);
	aps.sort(compSort);
	reportConf();
	return true;
}

void reportConf()
{
	std::list<adaptiveProfile>::iterator it;
	for(it=aps.begin();it!=aps.end();it++)
	{
		ga_error("adaptive-stream: configuration loaded %d %f %d %d %f %l %l\n", it->packetLoss, it->rtt, it->jitter, it->crf, it->fps, it->bitrate, it->vbv);
	}
}