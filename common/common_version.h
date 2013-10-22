/*
 * common_version.h
 *
 *  Created on: 2011-11-21
 *      Author: jimm
 */

#ifndef COMMON_VERSION_H_
#define COMMON_VERSION_H_

#define MAJOR		"1"
#define MINOR		"7"
#define BUILD		"100"


#define GET_VERSION(version)\
	const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};\
	int32_t year;\
	unsigned char month;\
	unsigned char day;\
	char temp[] = __DATE__;\
	unsigned char i;\
	year = atoi(temp + 7);\
	*(temp + 6) = 0;\
	day = atoi(temp + 4);\
	*(temp + 3) = 0;\
	for (i = 0; i < 12; i++)\
	{\
		if (!strcmp(temp, months[i]))\
		{\
			month = i + 1;\
			break;\
		}\
	}\
	sprintf(version,"Version: %s.%s.build%s, Release Date: %04d-%02d-%02d %s",MAJOR, MINOR, BUILD, year, month, day, __TIME__);


#endif /* COMMON_VERSION_H_ */
