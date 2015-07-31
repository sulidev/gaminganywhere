/*
 * Copyright (c) 2015 Suliadi Marsetya
 *
 *
 */

#ifndef __ADAPTIVEPROFILE_H__
#define __ADAPTIVEPROFILE_H__

#include <string>

void profileMain();
ga_ioctl_reconfigure_t selectProfile(float, float, unsigned);
void writeLog2(std::string[]);
std::string concatStr(char*, int);
#endif