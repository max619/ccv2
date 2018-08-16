/** 
 * \file      nuiProperty.h
 * Some parts Copyright (C) 2010 Movid Authors.  All rights reserved.
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 * 
 * \author    Scott Halstvedt
 * \author    Maxim Bagryantsev
 * \date      2012-2018
 * \copyright Copyright 2012 NUI Group. All rights reserved.
 * \copyright Copyright 2018 Appollo Pro. All rights reserved.
 */


#ifndef NUI_PROPERTY_H
#define NUI_PROPERTY_H

#include <ostream>
#include <string>
#include <vector>
#include <map>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>

#include "nuiUtils.h"



typedef struct nuiPoint {
    double x;
    double y;
};

typedef std::vector<nuiPoint> nuiPointList;

typedef enum nuiPropertyType {
    NUI_PROPERTY_NONE,
    NUI_PROPERTY_BOOL,
    NUI_PROPERTY_STRING,
    NUI_PROPERTY_INTEGER,
    NUI_PROPERTY_DOUBLE,
	NUI_PROPERTY_FLOAT,
    NUI_PROPERTY_POINTLIST,
};

class nuiProperty;
class nuiLinkedProperty;

typedef void (*nuiPropertyCallback)(nuiProperty *property, void *userdata);

typedef void(*nuiPropertyUpdatedCallback)(std::string& name, nuiProperty* prop, nuiLinkedProperty* linkedProp, void* userdata);

class nuiLinkedProperty
{
public:
	nuiLinkedProperty();
	~nuiLinkedProperty();

	void* prop;
	int type;
	char* name;
	bool needCallback;
	nuiPropertyUpdatedCallback propUpdtCallback;
	void* userData;
};

void __execLinkedPropertyCallback(nuiProperty* prop, void* userdata);

class nuiProperty {
public:
	nuiProperty(nuiPropertyType type, const std::string &description = "");
    nuiProperty(bool value, const std::string &description = "");
    nuiProperty(const char *value, const std::string &description = "");
    nuiProperty(std::string value, const std::string &description = "");
    nuiProperty(int value, const std::string &description = "");
    nuiProperty(double value, const std::string &description = "");
	nuiProperty(float value, const std::string &description = "");
    nuiProperty(nuiPointList, const std::string &description = "");
    ~nuiProperty();

    nuiPropertyType getType();

    void setDescription(const std::string& desc);
    std::string getDescription();

    void setReadOnly(bool);
    bool isReadOnly();

    bool asBool();
    std::string asString();
    double asDouble();
	float asFloat();
    int asInteger();
    nuiPointList asPointList();

    void set(bool value, bool trigger = true);
    void set(std::string value, bool trigger = true);
    void set(const char* value, bool trigger = true);
    void set(int value, bool trigger = true);
    void set(double value, bool trigger = true);
	void set(float value, bool trigger = true);
    void set(nuiPointList value, bool trigger = true);

	nuiLinkedProperty* linkProperty(std::string name, int type, void * data, const std::string &description = "");

    bool haveMin();
    bool haveMax();
    bool haveChoices();
    int getMin();
    int getMax();
    std::string getChoices();
    void setMin(int val);
    void setMax(int val);
    void setChoices(const std::string &val);

    bool isText();

    void addCallback(nuiPropertyCallback callback, void *userdata);
    void removeCallback(nuiPropertyCallback callback);

    friend std::ostream& operator<< (std::ostream& o, const nuiProperty& f);

    static std::string getPropertyTypeName(nuiPropertyType type);

private:
    nuiProperty(const nuiProperty& property);
    nuiPropertyType type;
    std::map<nuiPropertyCallback, void*> callbacks;
    std::string description;
    void* val;
    bool readonly;
    bool have_min;
    bool have_max;
    bool have_choices;
    int val_min;
    int val_max;
    std::string val_choices;

    void free();
    void init(const std::string& description);
    void fireCallback();
};

#endif

