// adaptive-stream.cpp : Defines the exported functions for the DLL application.
//

#include <stdio.h>
#include <pthread.h>
#include "ga-common.h"
#include "ga-module.h"

static pthread_t adaptive_tid;

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

void* adaptive_main(void* arg)
{
	while(true){
		ga_error("percobaan\n");
		Sleep(1000);
	}
}

static int
adaptive_start(void *arg) {
	if(pthread_create(&adaptive_tid, NULL, adaptive_main, NULL) != 0) {
		ga_error("start adaptive module failed.\n");
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