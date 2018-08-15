/**
* \file      nuiTouchInjectorSettings.cpp
* \author    Maxim Bagryantsev
* \date      2018
* \copyright Copyright 2012 NUI Group. All rights reserved.
* \copyright Copyright 2018 Appollo Pro. All rights reserved.
*/

#include "nuiTouchInjectorSettings.h"

nuiTouchInjectorSettings * nuiTouchInjectorSettings::LoadSettings(std::string path)
{
	std::ifstream f(path);
	Json::Reader reader;
	Json::Value val;
	if (reader.parse(f, val))
	{
		nuiTouchInjectorSettings* set = new nuiTouchInjectorSettings();
		set->loadFromJson(val);
		return set;
	}
	return NULL;
}

nuiTouchInjectorSettings::~nuiTouchInjectorSettings()
{
}

int & nuiTouchInjectorSettings::getXOffset()
{
	return this->offsetX;
}

int & nuiTouchInjectorSettings::getYOffset()
{
	return this->offsetY;
}

int & nuiTouchInjectorSettings::getScreenWidth()
{
	return this->screenWidth;
}

int & nuiTouchInjectorSettings::getScreenHeight()
{
	return this->screenHeight;
}

nuiTouchInjectorSettings::nuiTouchInjectorSettings()
{
}

void nuiTouchInjectorSettings::loadFromJson(Json::Value& value)
{
	offsetX = value.get("X", 0).asInt();
	offsetY = value.get("Y", 0).asInt();
	screenWidth = value.get("Width", 0).asInt();
	screenHeight = value.get("Height", 0).asInt();
}
