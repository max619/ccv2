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