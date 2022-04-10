#include "MenuType.h"
#include "PCF8574.h"
#include "KetcarMenu.h"
#pragma once
class MainMenu : public MenuType
{
private:
	bool _reverse;
	bool _wirelessEnabled;
	bool _wirelessSignal;
	int _currentSpeed;
	KettcarMenu* _menu;
	PCF8574* _ioExpander;
	void DrawDirectionText();
	void DrawWirelessStatus();
public:
	MainMenu(LiquidCrystal_I2C*, PCF8574*, KettcarMenu*);
	void Draw();
	void OnScroll(int cursorChange);
	void OnClick();
	void SetWirelessSignal(bool signal);
	bool GetWirelessEnabled();
	void UpdateCurrentSpeed(int);
};