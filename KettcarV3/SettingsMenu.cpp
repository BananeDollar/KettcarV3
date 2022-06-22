#include "SettingsMenu.h"

SettingsMenu::SettingsMenu(LiquidCrystal_I2C* lcd, IntCallback menuChangeRequest, SimpleCallback oledUpdate) : MenuType(lcd, menuChangeRequest)
{
	_oledUpdate = oledUpdate;

	_maxCursorPosition = settingsCount;
}

void SettingsMenu::Draw()
{
	_lcd->clear();
	if (GetCurrentScroll() == 0)
	{
		_lcd->setCursor(0, 0);
		_lcd->print("- Einstellungen - ");
	}
	
	for (int i = 0; i < settingsCount; i++)
	{
		int pos = i - GetCurrentScroll() + 1;
		if (pos < 4 && (pos > 0 || GetCurrentScroll()!=0) && pos >= 0)
		{
			_lcd->setCursor(0, pos);
			_lcd->print(_settingsNames[i]);
			
			_lcd->setCursor(15, pos);
			_lcd->print(_settingsValues[i]);
		}
	}
	DrawCursor();
}

void SettingsMenu::Init()
{
	EEPROM.begin(20);
	delay(200);

	_settingsValues[MaxSpeedSettingIndex] = EEPROM.read(0);
	_settingsValues[SpeedometerGranularitySettingIndex] = EEPROM.read(2);
	_settingsValues[MaxPedalThrottleSettingIndex] = EEPROM.read(4);
	//_settingsValues[PedalDeadzoneSettingIndex] = 50; // Set by Main ino File
	_settingsValues[MaxRemoteThrottleSettingIndex] = EEPROM.read(6);
}

void SettingsMenu::OnScroll(int cursorChange)
{
	if (!_isEdditingValue)
	{
		_lastScroll = GetCurrentScroll();
		MoveCursor(cursorChange);
		if (_lastScroll != GetCurrentScroll())
		{
			Draw();
			_lastScroll = GetCurrentScroll();
		}
	}
	else
	{
		int cursorPos = GetCursorPosition();
		int line = cursorPos - GetCurrentScroll();
		
		int settingsIndex = cursorPos - 1;
		_settingsValues[settingsIndex] -= cursorChange * _settingsIncrementSteps[settingsIndex];

		if (_settingsValues[settingsIndex] < _minSettingsValues[settingsIndex])
		{
			_settingsValues[settingsIndex] = _minSettingsValues[settingsIndex];
		}
		else if (_settingsValues[settingsIndex] > _maxSettingsValues[settingsIndex])
		{
			_settingsValues[settingsIndex] = _maxSettingsValues[settingsIndex];
		}
			
		_lcd->setCursor(15, line);
		_lcd->print(String(_settingsValues[settingsIndex]) + "  ");
		_lcd->setCursor(19, line);
		_lcd->print("*");

		// Update oled when settingsIndex is 0 or 1
		if (settingsIndex == 0 || settingsIndex == 1)
		{
			_oledUpdate();
		}
	}
}

void SettingsMenu::OnClick()
{
	if (GetCursorPosition() == 0)
	{
		SaveSettings();
		_menuChangeRequest(0);
	}
	else
	{
		_isEdditingValue = !_isEdditingValue;

		if (_isEdditingValue)
		{
			_lcd->setCursor(14, GetCursorPosition() - GetCurrentScroll());
			_lcd->print("*");
			_lcd->setCursor(19, GetCursorPosition() - GetCurrentScroll());
			_lcd->print("*");
		}
		else
		{
			Draw();
		}
	}
}

void SettingsMenu::SaveSettings()
{
	EEPROM.put(0, _settingsValues[MaxSpeedSettingIndex]);
	EEPROM.put(2,_settingsValues[SpeedometerGranularitySettingIndex]);
	EEPROM.put(4,_settingsValues[MaxPedalThrottleSettingIndex]);
	EEPROM.put(6,_settingsValues[MaxRemoteThrottleSettingIndex]);
	EEPROM.commit();
}

int SettingsMenu::GetSettingValue(int index)
{
	return _settingsValues[index];
}

void SettingsMenu::SetSettingValue(int index, int value)
{
	_settingsValues[index] = value;
}
