// adaptive-stream.cpp : Defines the exported functions for the DLL application.
//

#include <stdio.h>
#include <pthread.h>
#include <liveMedia.hh>
#include <BasicUsageEnvironment.hh>
#include "ga-common.h"
#include "ga-module.h"
#include "adaptive-stream.h"
#include <set>

static pthread_t adaptive_tid;
static UsageEnvironment* env = NULL;
static std::set<RTPSink*> sinkSet;

static int adaptive_init(void* arg)
{
	//console allocation
	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
	return 0;
}

static int adaptive_deinit(void* arg)
{
	FreeConsole();
	return 0;
}

void adaptive_add_sink(RTPSink* sink)
{
	sinkSet.insert(sink);
}

void adaptive_remove_sink(RTPSink* sink)
{
	sinkSet.erase(sink);
}

void adaptive_report()
{
	std::set<RTPSink*>::iterator it;
	for(it=sinkSet.begin(); it!=sinkSet.end(); it++)
	{
		RTPSink* temp = *it;
		RTPTransmissionStatsDB& db = temp->transmissionStatsDB();
		RTPTransmissionStatsDB::Iterator statsIter(db);
		RTPTransmissionStats *stats = NULL;
		while((stats = statsIter.next()) != NULL)
		{
			ga_error("adaptive-stream: Packet lost=%d\n", stats->totNumPacketsLost());
		}
	}
}

void adaptive_check()
{
	if(sinkSet.size()>0)
		adaptive_report();
	else
		ga_error("adaptive-stream: waiting sinks\n");
	env->taskScheduler().scheduleDelayedTask(1000000,(TaskFunc*)adaptive_check,NULL);
}

void* adaptive_main(void* arg)
{
	TaskScheduler* scheduler = BasicTaskScheduler::createNew();
	env = BasicUsageEnvironment::createNew(*scheduler);
	ga_error("adaptive-stream: adaptive module started\n");
	env->taskScheduler().scheduleDelayedTask(1000000,(TaskFunc*)adaptive_check,NULL);
	env->taskScheduler().doEventLoop();
	return 0;
}

static int
adaptive_start(void *arg) {
	if(pthread_create(&adaptive_tid, NULL, adaptive_main, NULL) != 0) {
		ga_error("adaptive-stream: start adaptive module failed.\n");
		return -1;
	}
	return 0;
}

static int
adaptive_stop(void *arg) {
	pthread_cancel(adaptive_tid);
	return 0;
}

ga_module_t *
module_load() {
	static ga_module_t m;
	//
	bzero(&m, sizeof(m));
	m.type = GA_MODULE_TYPE_SERVER;
	m.name = strdup("adaptive-stream");
	m.init = adaptive_init;
	m.start = adaptive_start;
	m.stop = adaptive_stop;
	m.deinit = adaptive_deinit;
	return &m;
}