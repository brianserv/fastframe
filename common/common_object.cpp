/*
 * common_object.h
 *
 *  Created on: 2011-11-21
 *      Author: jimm
 */

#include "common/common_object.h"
#include "common/common_errordef.h"


CObject::CObject()
{

}

CObject::~CObject()
{

}

int32_t CObject::Initialize()
{
	return S_OK;
}

int32_t CObject::Resume()
{
	return S_OK;
}

int32_t CObject::Uninitialize()
{
	return S_OK;
}

