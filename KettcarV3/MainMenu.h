#include "MenuType.h"
#pragma once
class MainMenu : public MenuType
{
private:
	bool _reverse;
	bool _wirelessEnabled;
	bool _wirelessSignal;
	int _currentSpeed;
	BoolCallback _setReverse;
	void DrawDirectionText();
	void DrawWirelessStatus();
public:
	MainMenu(LiquidCrystal_I2C*, IntCallback, BoolCallback, int* menuLevel);
	void Init();
	void Draw();
	void OnScroll(int cursorChange);
	void OnClick();
	void SetWirelessSignal(bool signal);
	bool GetWirelessEnabled();
	void UpdateCurrentSpeed(int);
	void drawDebugText(String);
};