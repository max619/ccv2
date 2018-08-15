#ifndef NUI_TOUCH_INJECTOR_SETTINGS
#define NUI_TOUCH_INJECTOR_SETTINGS

#include <string>
#include <json/json.h>
#include <fstream>

class nuiTouchInjectorSettings
{
public:
	static nuiTouchInjectorSettings* LoadSettings(std::string path);
	~nuiTouchInjectorSettings();
	int& getXOffset();
	int& getYOffset();
	int& getScreenWidth();
	int& getScreenHeight();


private:
	nuiTouchInjectorSettings();
	int offsetX, offsetY, screenWidth, screenHeight;
	void loadFromJson(Json::Value& value);
};

#endif // !NUI_TOUCH_INJECTOR_SETTINGS


