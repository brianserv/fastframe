/*
 * common_codeengine.h
 *
 *  Created on: 2011-11-21
 *      Author: jimm
 */

#ifndef COMMON_CODEENGINE_H_
#define COMMON_CODEENGINE_H_

#include <stdio.h>
#include <memory.h>
#include <string.h>

#include "common_typedef.h"


class CEndian
{
public:
	CEndian()
	{
		union
		{
			uint16_t	s1;
			uint8_t		c2[2];
		} un;
		un.s1 = 0x0102;
		m_bBigEndian = (un.c2[0] == 0x01);
	}

	inline bool IsBigEndian()
	{
		return m_bBigEndian;
	}

	bool			m_bBigEndian;
};


class CCodeEngine
{
public:
	CCodeEngine();
	virtual ~CCodeEngine();

public:
	/* 数据打包，将编码结果放到缓冲区中
	*  buf      - 目的缓冲区，所占用内存在函数内部分配，而由调用者释放
	*  size     - 目的缓冲区长度
	*  offset   - 输入打包数据放入到缓冲区的起始偏移量，输出为该偏移量与
	*             新打包的数据长度之和
	*  data     - 待打包的数据
	*  return   - 错误码
	*/
	static int32_t Encode(uint8_t* buf, const uint32_t size, uint32_t& offset, const bool data);
	static int32_t Encode(uint8_t* buf, const uint32_t size, uint32_t& offset, const int8_t data);
	static int32_t Encode(uint8_t* buf, const uint32_t size, uint32_t& offset, const int16_t data);
	static int32_t Encode(uint8_t* buf, const uint32_t size, uint32_t& offset, const int32_t data);
	static int32_t Encode(uint8_t* buf, const uint32_t size, uint32_t& offset, const int64_t data);
	static int32_t Encode(uint8_t* buf, const uint32_t size, uint32_t& offset, const uint8_t data);
	static int32_t Encode(uint8_t* buf, const uint32_t size, uint32_t& offset, const uint16_t data);
	static int32_t Encode(uint8_t* buf, const uint32_t size, uint32_t& offset, const uint32_t data);
	static int32_t Encode(uint8_t* buf, const uint32_t size, uint32_t& offset, const uint64_t data);
	static int32_t Encode(uint8_t* buf, const uint32_t size, uint32_t& offset, const uint8_t *data, const uint32_t length, const uint32_t maxLength);
	static int32_t Encode(uint8_t* buf, const uint32_t size, uint32_t& offset, const char *data, const uint16_t maxLength);
	static int32_t Encode(uint8_t* buf, const uint32_t size, uint32_t& offset, const double data);

public:
	/* 数据解包，对缓冲区中的数据进行解析
	*  buf      - 数据缓冲区，包含待解析的数据
	*  size     - 数据缓冲区长度
	*  offset   - 输入缓冲区中数据解包的起始偏移量，输出为该偏移量与新解包
	*             的数据长度之和
	*  data     - 返回结果的结果，所占用的内存在函数内部分配，由调用者释放
	*  return   - 错误码
	*/
	static int32_t Decode(const uint8_t *buf, const uint32_t size, uint32_t& offset, bool& data);
	static int32_t Decode(const uint8_t *buf, const uint32_t size, uint32_t& offset, int8_t& data);
	static int32_t Decode(const uint8_t *buf, const uint32_t size, uint32_t& offset, int16_t& data);
	static int32_t Decode(const uint8_t *buf, const uint32_t size, uint32_t& offset, int32_t& data);
	static int32_t Decode(const uint8_t *buf, const uint32_t size, uint32_t& offset, int64_t& data);
	static int32_t Decode(const uint8_t *buf, const uint32_t size, uint32_t& offset, uint8_t& data);
	static int32_t Decode(const uint8_t *buf, const uint32_t size, uint32_t& offset, uint16_t& data);
	static int32_t Decode(const uint8_t *buf, const uint32_t size, uint32_t& offset, uint32_t& data);
	static int32_t Decode(const uint8_t *buf, const uint32_t size, uint32_t& offset, uint64_t& data);
	static int32_t Decode(const uint8_t *buf, const uint32_t size, uint32_t& offset, uint8_t* data, const uint32_t length, const uint32_t maxLength);
	static int32_t Decode(const uint8_t *buf, const uint32_t size, uint32_t& offset, char* data, const uint16_t maxLength);
	static int32_t Decode(const uint8_t *buf, const uint32_t size, uint32_t& offset, double& data);

protected:
	static CEndian	ms_endian;
};


#endif /* COMMON_CODEENGINE_H_ */
