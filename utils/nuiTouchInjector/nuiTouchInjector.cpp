/**
* \file      nuiTouchInjector.cpp
* \author    Maxim Bagryantsev
* \date      2018
* \copyright Copyright 2012 NUI Group. All rights reserved.
* \copyright Copyright 2018 Appollo Pro. All rights reserved.
*/

#include "nuiTouchInjector.h"

#ifdef _DEBUG
#define WRITE_DEBUG_TOUCH_MESSAGE(msg) std::cout << msg  << std::endl;
#else
#define WRITE_DEBUG_TOUCH_MESSAGE(msg) ;
#endif // _DEBUG


nuiTouchInjector::nuiTouchInjector(int port)
{
	pointerId = 0;
	this->settings = NULL;
	this->receiver = new UdpReceiver(port);
	this->tuioClient = new TuioClient(receiver);
	this->tuioClient->addTuioListener(this);
	this->tuioClient->connect();

	InitializeTouchInjection(25, TOUCH_FEEDBACK_NONE);
}


nuiTouchInjector::~nuiTouchInjector()
{
	if (settings != NULL)
		delete settings;
	this->tuioClient->disconnect();
	delete this->tuioClient;
	delete this->receiver;
}

void nuiTouchInjector::tuioAdd(TuioObject * tobj)
{
	if (tobj->containsNewTuioToken()) WRITE_DEBUG_TOUCH_MESSAGE("add tok " << tobj->getTuioToken()->getSessionID());
	if (tobj->containsNewTuioPointer()) 
	{
		WRITE_DEBUG_TOUCH_MESSAGE("add ptr " << tobj->getTuioPointer()->getSessionID());
		addTouchPoint(tobj->getTuioPointer());
	}
	if (tobj->containsNewTuioBounds()) WRITE_DEBUG_TOUCH_MESSAGE("add bnd " << tobj->getTuioBounds()->getSessionID());
	if (tobj->containsNewTuioSymbol()) WRITE_DEBUG_TOUCH_MESSAGE("add sym " << tobj->getTuioSymbol()->getSessionID());
	std::cout << std::flush;
}

void nuiTouchInjector::tuioUpdate(TuioObject * tobj)
{
	if (tobj->containsTuioToken()) WRITE_DEBUG_TOUCH_MESSAGE("upd tok " << tobj->getTuioToken()->getSessionID());
	if (tobj->containsTuioPointer())
	{
		WRITE_DEBUG_TOUCH_MESSAGE("upd ptr " << tobj->getTuioPointer()->getSessionID());;
		updateTouchPoint(tobj->getTuioPointer());
	}
	if (tobj->containsTuioBounds()) WRITE_DEBUG_TOUCH_MESSAGE("upd bnd " << tobj->getTuioBounds()->getSessionID());
	if (tobj->containsTuioSymbol()) WRITE_DEBUG_TOUCH_MESSAGE("upd sym " << tobj->getTuioSymbol()->getSessionID());
	std::cout << std::flush;
}

void nuiTouchInjector::tuioRemove(TuioObject * tobj)
{
	if (tobj->containsTuioToken()) WRITE_DEBUG_TOUCH_MESSAGE("rm tok " << tobj->getTuioToken()->getSessionID());
	if (tobj->containsTuioPointer())
	{
		WRITE_DEBUG_TOUCH_MESSAGE("rm ptr " << tobj->getTuioPointer()->getSessionID());
		removeTouchPoint(tobj->getTuioPointer());
	}
	if (tobj->containsTuioBounds()) WRITE_DEBUG_TOUCH_MESSAGE("rm bnd " << tobj->getTuioBounds()->getSessionID());
	if (tobj->containsTuioSymbol()) WRITE_DEBUG_TOUCH_MESSAGE("rm sym " << tobj->getTuioSymbol()->getSessionID());
	std::cout << std::flush;
}

void nuiTouchInjector::tuioRefresh(TuioTime frameTime)
{
#ifdef _DEBUG
	std::cout << "refresh " << frameTime.getFrameID() << " " << frameTime.getTotalMilliseconds() << std::endl;
	std::cout << std::flush;
#endif // _DEBUG	
}

void nuiTouchInjector::setSettings(nuiTouchInjectorSettings * settings)
{
	this->settings = settings;
}

void nuiTouchInjector::run()
{
	running = true;
	while (running) {
		Sleep(10);
	}
}

void nuiTouchInjector::addTouchPoint(TuioPointer * pointer)
{
	if (!isTouchPointerInsideScreen(pointer))
		return;

	POINTER_TOUCH_INFO* touch = fromTuioPointer(pointer);
	touch->pointerInfo.pointerFlags = POINTER_FLAG_DOWN | POINTER_FLAG_INRANGE | POINTER_FLAG_INCONTACT;

	BOOL success = InjectTouchInput(1, touch);
	if (success)
		this->touchPoints[pointer->getPointerID()] = touch;
	else
		free(touch);
}

void nuiTouchInjector::updateTouchPoint(TuioPointer * pointer)
{
	std::map<int, POINTER_TOUCH_INFO*>::iterator it = touchPoints.find(pointer->getPointerID());
	if (it == touchPoints.end())
	{
		if (!isTouchPointerInsideScreen(pointer))
		{
			//If need processing of out of scrren pointers
		}
		else
		{
			
			POINTER_TOUCH_INFO* touch = fromTuioPointer(pointer);

			touch->pointerInfo.pointerFlags = POINTER_FLAG_DOWN | POINTER_FLAG_INRANGE | POINTER_FLAG_INCONTACT;
			BOOL success = InjectTouchInput(1, touch);
			if (success)
				this->touchPoints[pointer->getPointerID()] = touch;
			else
				free(touch);

		}
	}else if (!isTouchPointerInsideScreen(pointer))
	{
		updateTouchPointFromTuioPointer(pointer, it->second);
		it->second->pointerInfo.pointerFlags = POINTER_FLAG_UP;

		BOOL success = InjectTouchInput(1, it->second);

		POINTER_TOUCH_INFO* ptr = it->second;
		touchPoints.erase(it);
		free(ptr);
	}
	else
	{
		updateTouchPointFromTuioPointer(pointer, it->second);
		it->second->pointerInfo.pointerFlags = POINTER_FLAG_UPDATE | POINTER_FLAG_INRANGE | POINTER_FLAG_INCONTACT;

		BOOL success = InjectTouchInput(1, it->second);

		if (!success)
		{
			POINTER_TOUCH_INFO* ptr = it->second;
			touchPoints.erase(it);
			free(ptr);
		}
	}
}

void nuiTouchInjector::removeTouchPoint(TuioPointer * pointer)
{
	std::map<int, POINTER_TOUCH_INFO*>::iterator it = touchPoints.find(pointer->getPointerID());
	if (it == touchPoints.end())
		return;

	updateTouchPointFromTuioPointer(pointer, it->second);
	it->second->pointerInfo.pointerFlags = POINTER_FLAG_UP;

	BOOL success = InjectTouchInput(1, it->second);

	POINTER_TOUCH_INFO* ptr = it->second;
	touchPoints.erase(it);
	free(ptr);

	//std::cout << "tracked points " << touchPoints.size() << std::endl;
}

POINTER_TOUCH_INFO * nuiTouchInjector::fromTuioPointer(TuioPointer * pointer)
{
	POINTER_TOUCH_INFO* touch = (POINTER_TOUCH_INFO*)malloc(sizeof(POINTER_TOUCH_INFO));
	memset(touch, 0, sizeof(POINTER_TOUCH_INFO));

	touch->pointerInfo.pointerType = PT_TOUCH; //we're sending touch input
	touch->pointerInfo.pointerId = pointer->getPointerID();          //contact 0
	//pointerId++;
	touch->pointerInfo.ptPixelLocation.x = static_cast<LONG>(pointer->getPosition().getX() * settings->getScreenWidth() + settings->getXOffset());
	touch->pointerInfo.ptPixelLocation.y = static_cast<LONG>(pointer->getPosition().getY() * settings->getScreenHeight() + settings->getYOffset());
	touch->touchFlags = TOUCH_FLAG_NONE;
	touch->touchMask = TOUCH_MASK_CONTACTAREA | TOUCH_MASK_ORIENTATION | TOUCH_MASK_PRESSURE;
	touch->orientation = 90;
	touch->pressure = 32000;

	touch->rcContact.top = touch->pointerInfo.ptPixelLocation.y - 2;
	touch->rcContact.bottom = touch->pointerInfo.ptPixelLocation.y + 2;
	touch->rcContact.left = touch->pointerInfo.ptPixelLocation.x - 2;
	touch->rcContact.right = touch->pointerInfo.ptPixelLocation.x + 2;

	return touch;
}

void nuiTouchInjector::updateTouchPointFromTuioPointer(TuioPointer * pointer, POINTER_TOUCH_INFO * touch)
{
	touch->pointerInfo.ptPixelLocation.x = static_cast<LONG>(pointer->getPosition().getX() * settings->getScreenWidth() + settings->getXOffset());
	touch->pointerInfo.ptPixelLocation.y = static_cast<LONG>(pointer->getPosition().getY() * settings->getScreenHeight() + settings->getYOffset());
	touch->rcContact.top = touch->pointerInfo.ptPixelLocation.y - 2;
	touch->rcContact.bottom = touch->pointerInfo.ptPixelLocation.y + 2;
	touch->rcContact.left = touch->pointerInfo.ptPixelLocation.x - 2;
	touch->rcContact.right = touch->pointerInfo.ptPixelLocation.x + 2;
}

bool nuiTouchInjector::isTouchPointerInsideScreen(TuioPointer * pointer)
{
	//pointer->getPosition.getX() < 0.f || pointer->getPosition.getX() > 1.f || pointer->getPosition.getY() < 0.f || pointer->getPosition.getY() > 1.f
	float x = pointer->getPosition().getX();
	float y = pointer->getPosition().getY();

	return x > 0.f && x<1.f && y>0.f && y < 1.f;
}
