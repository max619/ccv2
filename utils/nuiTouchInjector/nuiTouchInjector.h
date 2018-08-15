/**
* \file      nuiTouchInjector.h
* \author    Maxim Bagryantsev
* \date      2018
* \copyright Copyright 2012 NUI Group. All rights reserved.
* \copyright Copyright 2018 Appollo Pro. All rights reserved.
*/

#ifndef NUI_TOUCH_INJECTOR
#define NUI_TOUCH_INJECTOR

#include <Windows.h>
#include <TuioClient.h>
#include <TuioListener.h>
#include <UdpReceiver.h>
#include "nuiTouchInjectorSettings.h"

using namespace TUIO2;

class nuiTouchInjector : public TuioListener
{
public:
	nuiTouchInjector(int port);
	~nuiTouchInjector();

	void tuioAdd(TuioObject *tobj);
	void tuioUpdate(TuioObject *tobj);
	void tuioRemove(TuioObject *tobj);
	void tuioRefresh(TuioTime frameTime);
	void setSettings(nuiTouchInjectorSettings* settings);

	void run();
private: 
	TuioClient * tuioClient;
	OscReceiver* receiver;
	bool running;
	nuiTouchInjectorSettings* settings;
	std::map<int, POINTER_TOUCH_INFO*> touchPoints;

	void addTouchPoint(TuioPointer* pointer);
	void updateTouchPoint(TuioPointer* pointer);
	void removeTouchPoint(TuioPointer* pointer);

	POINTER_TOUCH_INFO* fromTuioPointer(TuioPointer* pointer);
	void updateTouchPointFromTuioPointer(TuioPointer* pointer, POINTER_TOUCH_INFO* touch);
	bool isTouchPointerInsideScreen(TuioPointer* pointer);

};
#endif // !NUI_TOUCH_INJECTOR



