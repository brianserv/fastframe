/*
 * common.h
 *
 *  Created on: 2011-11-21
 *      Author: jimm
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>


#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#include <process.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#endif

#endif /* COMMON_H_ */
