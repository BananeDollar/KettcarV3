#include "KetcarMenu.h"

SettingsMenu::SettingsMenu(LiquidCrystal_I2C* lcd) : MenuType(lcd)
{
	_maxCursorPosition = 6;

	_settingsValues[MaxSpeedSettingIndex] = 50;
	_settingsValues[SpeedometerGranularitySettingIndex] = 10;
	_settingsValues[MaxPedalThrottleSettingIndex] = 100;
	//_settingsValues[PedalDeadzoneSettingIndex] = 50; // Set by Main ino File
	_settingsValues[MaxRemoteThrottleSettingIndex] = 10;

}

void SettingsMenu::Draw()
{
	
}

void SettingsMenu::OnScroll(int cursorChange)
{
	
}

void SettingsMenu::OnClick()
{
	
}

int SettingsMenu::GetSettingValue(int index)
{
	return _settingsValues[index];
}

void SettingsMenu::SetSettingValue(int index, int value)
{
	_settingsValues[index] = value;
}
