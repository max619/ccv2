/////////////////////////////////////////////////////////////////////////////
// Name:        modules/static.h
// Purpose:     Example Module to be used for developments.
// Author:      Maxim Bagryantsev
// Copyright:   (c) 2018 Appollo Pro
/////////////////////////////////////////////////////////////////////////////

#ifndef NUI_REALSENSE_MODULE_STATIC
#define NUI_REALSENSE_MODULE_STATIC
#include <memory>

void setFrameworkPtr(void * ptr);

void* getFrameworkPtr();

void** getStaticPointer();

#endif // !NUI_REALSENSE_MODULE_STATIC


