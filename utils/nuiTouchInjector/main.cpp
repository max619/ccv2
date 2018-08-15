/**
* \file      main.cpp
* \author    Maxim Bagryantsev
* \date      2018
* \copyright Copyright 2012 NUI Group. All rights reserved.
* \copyright Copyright 2018 Appollo Pro. All rights reserved.
*/


#include "nuiTouchInjector.h"

int main(int args, char** arg)
{
	nuiTouchInjectorSettings* settings = nuiTouchInjectorSettings::LoadSettings("./cfg.json");
	if (settings == NULL)
		return -1;
	nuiTouchInjector* injector = new nuiTouchInjector(3333);
	injector->setSettings(settings);
	injector->run();
	delete injector;
	return 0;
}