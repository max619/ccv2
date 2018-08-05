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
