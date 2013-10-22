/*
 * common_util.h
 *
 *  Created on: 2011-3-30
 *      Author: chenl
 */

#ifndef COMMON_UTIL_H_
#define COMMON_UTIL_H_

//分支预测
#ifndef WIN32		// only for GCC
	#define likely(x)		__builtin_expect(!!(x), 1)
	#define unlikely(x)		__builtin_expect(!!(x), 0)
#else
	#define likely(x) 		x
	#define unlikely(x) 	x
#endif


//预抓取数据送到cacheline
//prefetch宏不检查addr是否是一个合法地址
//使用数据后 数据将继续留在cacheline中
#ifdef EXTREME_OPTIMAL
	#define prefetch_read(addr) 		__builtin_prefetch(addr, 0, 3)
	#define prefetch_write(addr)		__builtin_prefetch(addr, 1, 3)
#else
	#define prefetch_read(addr)
	#define prefetch_read(addr)
#endif

// macro for creating bitmap
#define X )*2+1
#define _ )*2
#define BM_32 ((((((((((((((((((((((((((((((((0
#define BM_16 ((((((((((((((((0
//eg: create a 32bit bitmap
//#define TEST_CONST BM_32 X _ _ _ _ _ X X _ X _ X X X _ X _ X _ _ X X X _ X _ _ _ X _ X X
//TEST_CONST == 0x835d4e8b

#define DEBUG_MEM_MAGIC		0xdead4ead

#endif /* COMMON_UTIL_H_ */
