/**
* \file      static.h
* \author    Maxim Bagryantsev
* \date      2018
* \copyright Copyright 2012 NUI Group. All rights reserved.
* \copyright Copyright 2018 Appollo Pro. All rights reserved.
*/

#ifndef NUI_REALSENSE_MODULE_STATIC
#define NUI_REALSENSE_MODULE_STATIC
#include <memory>

void setFrameworkPtr(void * ptr);

void* getFrameworkPtr();

void** getStaticPointer();

#endif // !NUI_REALSENSE_MODULE_STATIC


