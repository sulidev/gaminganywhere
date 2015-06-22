/*
 * Copyright (c) 2015 Suliadi Marsetya
 *
 *
 */

#include <stdio.h>
#include <pthread.h>
#include <liveMedia.hh>
#include <BasicUsageEnvironment.hh>
#include "ga-common.h"
#include "ga-module.h"
#include "adaptive-stream.h"
#include "adaptive-profile.h"
#include <set>

static pthread_t adaptive_tid;
static UsageEnvironment* env = NULL;
static ga_module_t *vencoder;
static ga_ioctl_reconfigure_t current;
static ga_ioctl_reconfigure_t target;
static ga_ioctl_reconfigure_t newParam;
float losses;
long reconfigCount = 0;

void adaptive_stabilizer(void);

static int adaptive_init(void* arg)
{
	//console allocation
	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	//freopen("CONOUT$", "w", stderr);

	vencoder = (ga_module_t*) arg;
	return 0;
}

static int adaptive_deinit(void* arg)
{
	FreeConsole();
	return 0;
}

void adaptive_apply_param(ga_ioctl_reconfigure_t params)
{
	int err = ga_module_ioctl(vencoder, GA_IOCTL_RECONFIGURE, sizeof(params), &params);
	printf("adaptive-stream: -----------current parameter--------\n");
	printf("adaptive-stream: Crf: %d\n", params.crf);
	printf("adaptive-stream: Bitrate: %dkbps\n", params.bitrateKbps);
	printf("adaptive-stream: Vbv-Buffer: %dkbit\n", params.bufsize);
	printf("adaptive-stream: reconfigure result code: %d\n", err);
	printf("adaptive-stream: ------------------------------------\n");
}

void adaptive_reconfigure(float loss, float rtt, unsigned jitter)
{
	losses = loss;
	newParam = selectProfile(loss,rtt,jitter);
	if(current.id < 0){
		if(reconfigCount>3)
			adaptive_apply_param(newParam);
		current = newParam;
		reconfigCount++;
	}
	if(target.id < 0)
		target = newParam;
}

void adaptive_stabilizer()
{
	if(current.id < 0 || target.id < 0){
		env->taskScheduler().scheduleDelayedTask(100000,(TaskFunc*)adaptive_stabilizer,NULL);
		return;
	}

	if(current.bitrateKbps > target.bitrateKbps){
		current.bitrateKbps-=10;
	}else{
		if(current.bitrateKbps<1000)
			current.bitrateKbps+=1;
		else
			current.bitrateKbps+=10;
	}
	current.crf = target.crf;

	adaptive_apply_param(current);
	if(losses > 0.1)
		target.bitrateKbps--;
	else
		target.id = -1;

	env->taskScheduler().scheduleDelayedTask(100000,(TaskFunc*)adaptive_stabilizer,NULL);
}

void* adaptive_main(void* arg)
{
	current.id = -1;
	target.id = -1;
	TaskScheduler* scheduler = BasicTaskScheduler::createNew();
	env = BasicUsageEnvironment::createNew(*scheduler);
	env->taskScheduler().scheduleDelayedTask(1000000,(TaskFunc*)adaptive_stabilizer,NULL);
	printf("adaptive-stream: adaptive module started\n");
	profileMain();
	env->taskScheduler().doEventLoop();
	return 0;
}

static int
adaptive_start(void *arg) {
	if(pthread_create(&adaptive_tid, NULL, adaptive_main, NULL) != 0) {
		printf("adaptive-stream: start adaptive module failed.\n");
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
	m.type = GA_MODULE_TYPE_NULL;
	m.name = strdup("adaptive-stream");
	m.init = adaptive_init;
	m.start = adaptive_start;
	m.stop = adaptive_stop;
	m.deinit = adaptive_deinit;
	return &m;
}