/** 
 * \file      nuiDynamicLibrary.cpp
 * \author    Anatoly Churikov
 * \author    Anatoly Lushnikov
 * \date      2012-2013
 * \copyright Copyright 2012 NUI Group. All rights reserved.
 */

#ifdef WIN32
  #include <Windows.h>
#else
  #include <dlfcn.h>
#endif

#include "nuiDynamicLibrary.h"
#include <sstream>
#include <iostream>

nuiDynamicLibrary::nuiDynamicLibrary(void* handle, std::string path) : handle_(handle), pathToLibrary(path)
{
}

nuiDynamicLibrary::~nuiDynamicLibrary ()
{
	if (handle_)
	{
#ifndef WIN32
	dlclose(handle_);
#else
	FreeLibrary((HMODULE)handle_);
#endif
	handle_ = NULL;
	}
}

nuiDynamicLibrary *nuiDynamicLibrary::load(const std::string name, std::string &errorString)
{
  errorString.empty();

  if (name.empty()) 
  {
    errorString = "Empty path.";
    return NULL;
  }

  void *handle = NULL;

#ifdef WIN32
  handle = LoadLibrary(name.c_str());
  if (handle == NULL)
  {
    DWORD errorCode = GetLastError();
    std::stringstream ss;
    ss << std::string("LoadLibrary(") << name 
      << std::string(") Failed. errorCode: ") 
      << errorCode; 
    errorString = ss.str();
  }
#else
	handle = dlopen(name.c_str(), RTLD_NOW);
	if (!handle) 
	{
		std::string dlErrorString;
		const char *zErrorString = ::dlerror();
		if (zErrorString)
		dlErrorString = zErrorString;
		errorString += "Failed to load \"" + name + '"';
		if(dlErrorString.size())
		errorString += ": " + dlErrorString;
		return NULL;
	}
#endif
	return new nuiDynamicLibrary(handle, name);
}

void *nuiDynamicLibrary::getSymbol(const std::string &symbol)
{
	if (handle_ == NULL)
	    return NULL;
#ifdef WIN32
	return GetProcAddress((HMODULE)handle_, symbol.c_str());
#else
	return dlsym(handle_, symbol.c_str());
#endif
}

bool nuiDynamicLibrary::unload( void* handle )
{
  if(handle == NULL)
    return NULL;
#ifdef WIN32
  return FreeLibrary((HMODULE)handle);
#else
  return dlclose(handle)
#endif
}

void* nuiDynamicLibrary::getHandle()
{
  return handle_;
}

std::string nuiDynamicLibrary::getPath()
{
  return pathToLibrary;
}
