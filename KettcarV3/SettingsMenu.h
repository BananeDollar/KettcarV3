#pragma once
#include "MenuType.h"

class SettingsMenu : public MenuType
{
private:
	const String _settingsNames[settingsCount] = { "Max_Speed", "OLED_Steps", "Pedal_THR", "Pedal_DEAD", "Remote_THR" };
	const int _minSettingsValues[settingsCount] = { 1, 1, 0, 0, 0 };
	const int _moveSteps[settingsCount] = { 1, 1, 5, 5, 5 };
	const int _maxSettingsValues[settingsCount] = { 100 ,20, 100, 4095 ,100 };
	int _settingsValues[settingsCount];
public:
	SettingsMenu(LiquidCrystal_I2C* lcd);
	void Draw();
	void OnScroll(int cursorChange);
	void OnClick();
	int GetSettingValue(int index);
	void SetSettingValue(int index, int value);
};