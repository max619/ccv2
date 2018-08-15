/**
* \file      static.cpp
* \author    Maxim Bagryantsev
* \date      2018
* \copyright Copyright 2012 NUI Group. All rights reserved.
* \copyright Copyright 2018 Appollo Pro. All rights reserved.
*/

#include "static.h"

void setFrameworkPtr(void * ptr)
{
	void** sptr = getStaticPointer();
	(*sptr) = ptr;
}

void * getFrameworkPtr()
{
	void** ptr = getStaticPointer();
	return *ptr;
}

void ** getStaticPointer()
{
	static void* ptr;

	return &ptr;
}
