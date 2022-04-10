// KetcarMenu.h

#ifndef _KETCARMENU_h
#define _KETCARMENU_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include"Settings.h"
#include <list>
#include "MainMenu.h"
#include "SettingsMenu.h"
#include "MenuType.h"

class KettcarMenu
{
private:
	LiquidCrystal_I2C* _lcd;
	PCF8574* _ioExpander;
	int _cursorPosition = 0;
	int _currentMenu;
	int _currentSpeed;
	MenuType* _subMenus[2];
public:
	KettcarMenu(LiquidCrystal_I2C*, PCF8574*);
	void Init();
	void Draw();
	void OnScroll(int cursorChange);
	void OnClick();
	MainMenu* GetMainMenu();
	SettingsMenu* GetSettingsMenu();
};

extern KettcarMenu KetcarMenu;

#endif

