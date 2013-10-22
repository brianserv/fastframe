/*
 * frame_msg_impl.h
 *
 *  Created on: 2011-11-22
 *      Author: jimm
 */

#ifndef FRAME_MSG_IMPL_H_
#define FRAME_MSG_IMPL_H_

#include <string.h>

#include "common/common_typedef.h"
#include "common/common_def.h"
#include "frame_namespace.h"

FRAME_NAMESPACE_BEGIN

class IProtocol
{
public:
	virtual ~IProtocol(){}
	virtual int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset) = 0;
	virtual int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset) = 0;
	virtual void Dump(char* buf, const uint32_t size, uint32_t& offset) = 0;
	virtual int32_t GetSize() = 0;
};

typedef IProtocol	IMsgBody;
typedef IProtocol	IMsgHead;

#define SIZE_INTERFACE()	\
	public:	\
	int32_t GetSize(){return sizeof(*this);}

#define MESSAGE_BODY_BEGIN(message_name)		\
	class message_name : public IMsgBody		\
	{	\
		SIZE_INTERFACE()

#define MESSAGE_BODY_END()		};


#define STU_OFFSET sizeof(void*)

#define MESSAGEBODY_IMPLEMENT(message_body,encode_method, decode_method, dump_method)	\
class C##message_body : public IMsgBody, public message_body	\
{	\
public:	\
	C##message_body()	\
	{	\
		memset((message_body*)(((uint8_t*)this) + STU_OFFSET), 0, sizeof(message_body)); \
	}	\
	virtual ~C##message_body()	\
	{	\
	}	\
public:	\
	virtual int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)	\
	{	\
		return encode_method(buf, size, offset, *(message_body*)(((uint8_t*)this) + STU_OFFSET));	\
	}	\
	virtual int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)	\
	{	\
		return decode_method(buf, size, offset, *(message_body*)(((uint8_t*)this) + STU_OFFSET));	\
	}	\
	virtual void Dump(char* buf, const uint32_t size, uint32_t& offset)	\
	{	\
		return dump_method(buf, size, offset, *(message_body*)(((uint8_t*)this) + STU_OFFSET));	\
	}	\
};

#define CREATE_MSGBODY_INSTANCE(message_body)	new C##message_body

class CMessageMapDecl
{
	enum
	{
		enmMaxMessageIDCount = 1024
	};
public:
	CMessageMapDecl()
	{
		m_nMsgBodyCount = 0;
		memset(m_arrMsgID,0,sizeof(m_arrMsgID));
		memset(m_arrMsgBodyInstance, 0, sizeof(m_arrMsgBodyInstance));
	}
	~CMessageMapDecl(){}

	//»ñÈ¡MSGID->IMsgBody
	IMsgBody* GetMessageBody(uint32_t MsgID)
	{
		for(int32_t i = 0; i < m_nMsgBodyCount; i++)
		{
			if(m_arrMsgID[i] == MsgID)
			{
				return m_arrMsgBodyInstance[i];
			}
		}

		return NULL;
	}

	//×¢²áMsgID->IMsgBody
	void AddMsgBody(uint32_t MsgID, IMsgBody *pMsgBody)
	{
		if(m_nMsgBodyCount < enmMaxMessageIDCount)
		{
			m_arrMsgID[m_nMsgBodyCount] = MsgID;
			m_arrMsgBodyInstance[m_nMsgBodyCount] = pMsgBody;
			m_nMsgBodyCount++;
		}
	}

	void Clear()
	{
		m_nMsgBodyCount = 0;
		memset(m_arrMsgID, 0, sizeof(m_arrMsgID));
		memset(m_arrMsgBodyInstance, 0, sizeof(m_arrMsgBodyInstance));
	}
protected:
#ifdef _UNITTEST
public:
#endif
	int32_t		m_nMsgBodyCount;
	uint32_t 	m_arrMsgID[enmMaxMessageIDCount];
	IMsgBody*	m_arrMsgBodyInstance[enmMaxMessageIDCount];
};

#define	CREATE_MESSAGEMAPDECL_INSTANCE		CSingleton<CMessageMapDecl>::CreateInstance
#define	GET_MESSAGEMAPDECL_INSTANCE			CSingleton<CMessageMapDecl>::GetInstance
#define	DESTROY_MESSAGEMAPDECL_INSTANCE		CSingleton<CMessageMapDecl>::DestroyInstance

#define g_MessageMapDecl					GET_MESSAGEMAPDECL_INSTANCE()

#define MESSAGEMAP_DECL_BEGIN(entity)	\
class CMessageMapDecl_##entity	\
{	\
public:	\
	CMessageMapDecl_##entity()	\
	{	\
	}	\
	~CMessageMapDecl_##entity()	\
	{	\
	}	\
	void DeclMsgMap()	\
	{

#define MESSAGEMAP_DECL_MSG(id,msgbody)	\
	GET_MESSAGEMAPDECL_INSTANCE().AddMsgBody(id,msgbody);

#define MESSAGEMAP_DECL_END()	\
	}	\
};

#define INITIALIZE_MESSAGEMAP_DECL(entity)	\
	CMessageMapDecl_##entity l_##entity;	\
	l_##entity.DeclMsgMap()



FRAME_NAMESPACE_END
#endif /* FRAME_MSG_IMPL_H_ */
