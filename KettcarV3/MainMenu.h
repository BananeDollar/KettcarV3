#include "MenuType.h"
#pragma once
class MainMenu : public MenuType
{
private:
	bool _reverse;
	bool _wirelessEnabled;
	bool _wirelessSignal;
	int _currentSpeed;
	PCF8574* _ioExpander;
	void DrawDirectionText();
	void DrawWirelessStatus();
public:
	MainMenu(LiquidCrystal_I2C*, PCF8574*, IntCallback);
	void Init();
	void Draw();
	void OnScroll(int cursorChange);
	void OnClick();
	void SetWirelessSignal(bool signal);
	bool GetWirelessEnabled();
	void UpdateCurrentSpeed(int);
	void drawDebugText(String);
};