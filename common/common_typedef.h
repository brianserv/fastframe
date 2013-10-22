/*
 * common_typedef.h
 *
 *  Created on: 2011-11-21
 *      Author: jimm
 */

#ifndef COMMON_TYPEDEF_H_
#define COMMON_TYPEDEF_H_



#ifdef __GNUC__
#include <stdint.h>
#include <inttypes.h>
#endif

#if defined(WIN32) || defined(_WIN32)
typedef char						int8_t;
typedef unsigned char				uint8_t;
typedef short						int16_t;
typedef unsigned short				uint16_t;
typedef int							int32_t;
typedef unsigned int				uint32_t;
typedef long long					int64_t;
typedef unsigned long long			uint64_t;
#elif defined(_WIN64)
typedef char						int8_t;
typedef unsigned char				uint8_t;
typedef short						int16_t;
typedef unsigned short				uint16_t;
typedef int							int32_t;
typedef unsigned int				uint32_t;
typedef long 						int64_t;
typedef unsigned long				uint64_t;
#endif

#if defined(WIN32) || defined(_WIN32)
#pragma warning(disable:4996)
#pragma warning(disable:4005)
#endif

#if defined(WIN32) || defined(_WIN32)
#define OS_WIN32 1;
#else
#define OS_LINUX 1;
#endif

#endif /* COMMON_TYPEDEF_H_ */
