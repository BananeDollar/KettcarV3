#include "LCDMenu.h"
#include <LiquidCrystal_I2C.h>

// -- Menu --

LiquidCrystal_I2C _lcd(0x27, 20, 4);

bool _isDriving = false;

int _cursorPosition = 0;
int _maxSelection = 0;
int _settingsMenu = false;
int _editingValue = false;

//Constructor
LCDMenu::LCDMenu()
{
	settingsValues[MaxSpeedSettingIndex] = 50;
	settingsValues[SpeedometerGranularitySettingIndex] = 10;
	settingsValues[MaxPedalThrottleSettingIndex] = 100;
	settingsValues[MaxRemoteThrottleSettingIndex] = 10;
}

//init method
void LCDMenu::init()
{	
	_lcd.init();
	_lcd.backlight();
	_lcd.clear();

	_cursorPosition = 0;
	_maxSelection = 4;
	DrawMainScreen();
}

void LCDMenu::ButtonPress(int value) // 0 = no input, 1 = pressed, 2 = released
{
	if (value == 1)
	{
		if (!_settingsMenu) // is in Main Menu
		{
			switch (_cursorPosition)
			{
			case 0: // Tempomat

				break;
			case 3: // Einstellungen
				_settingsMenu = true;
				_cursorPosition = 0;
				_maxSelection = settingsCount +1;
				DrawSettingsScreen();
				break;
			}
		}
		else   // is in Settings
		{
			if (_cursorPosition == 0)
			{
				_cursorPosition = 0;
				_maxSelection = 4;
				_settingsMenu = false;
				DrawMainScreen();
			}
			else
			{
				_editingValue = !_editingValue;
			}
		}
	}

	DrawCursor();
}

void LCDMenu::RotaryInput(int value)
{
	if (_editingValue)
	{
		int newVal = settingsValues[_cursorPosition - 1] + value;
		
		// check if newVal is smaler than maxSettingsValues and larger than minSettingsValues
		if (newVal > maxSettingsValues[_cursorPosition - 1])
		{
			newVal = maxSettingsValues[_cursorPosition - 1];
		}
		else if (newVal < minSettingsValues[_cursorPosition - 1])
		{
			newVal = minSettingsValues[_cursorPosition - 1];
		}

		// Apply Value
		settingsValues[_cursorPosition - 1] = newVal;
		
		// Send Update to Listeners
		if (onSettingsChangeList[_cursorPosition - 1] != NULL)
		{
			onSettingsChangeList[_cursorPosition - 1](newVal, _cursorPosition - 1);
		}
	}
	else
	{
		// Clear last Cursor
		if (_cursorPosition >= 2 && _settingsMenu)
		{
			_lcd.setCursor(19, 2);
		}
		else
		{
			_lcd.setCursor(19, _cursorPosition);
		}
		_lcd.print(" ");
		_cursorPosition += value;

		//check if selectionindex is in bounds
		if (_cursorPosition < 0)
		{
			_cursorPosition = 0;
		}
		else if (_cursorPosition > _maxSelection - 1)
		{
			_cursorPosition = _maxSelection - 1;
		}
	}
	// Update Settings Menu
	if (_settingsMenu)
	{
		DrawSettingsScreen();
	}
	DrawCursor();
}

void LCDMenu::DrawCursor()
{
	int cursorY = 0;
	if (_cursorPosition >= 2 && _settingsMenu)
	{
		cursorY = 2;
	}
	else
	{
		cursorY = _cursorPosition;
	}
	_lcd.setCursor(19, cursorY);
	if (_editingValue)
	{
		_lcd.print("*");
		_lcd.setCursor(15, cursorY);
		_lcd.print("*");
	}
	else
	{
		_lcd.print("<");
		_lcd.setCursor(15, cursorY);
		_lcd.print(" ");
	}
}

void LCDMenu::SetSpeed(int speed)
{
	if (!_settingsMenu)
	{
		if (speed == 0)
		{
			if (_isDriving)
			{
				_isDriving = false;
				DrawMainScreen();
			}
		}
		else
		{
			if (!_isDriving)
			{
				_isDriving = true;
				DrawMainScreen();
			}
		}
	}
}

void LCDMenu::WriteExecutionTime(long executionTime)
{
	if (!_settingsMenu)
	{
		_lcd.setCursor(0,1);
		_lcd.print(String(executionTime) + "ms ");
	}
}

void LCDMenu::AddSetttingsChangeListener(SettingsListener listener, int settingsIndex)
{
	onSettingsChangeList[settingsIndex] = listener;
}


void LCDMenu::DrawMainScreen()
{
	_lcd.clear();
	_lcd.setCursor(0, 0);
	if (_isDriving)
	{
		_lcd.print("Tempomat");
	}
	else
	{
		/*
		 \xE1 = ä
		 \xEF  ö
		 \xF5 ü
		*/
		_lcd.print("R\xF5\ckw\xE1rts");
	}
	_lcd.setCursor(0, 3);
	_lcd.print("-Einstellungen-");

	DrawCursor();
}

void LCDMenu::DrawSettingsScreen()
{
	_lcd.clear();
	_lcd.setCursor(0, 0);
	_lcd.print("- Einstellungen -");

	int scroll = 0;
	if (_cursorPosition > 2)
	{
	scroll = _cursorPosition - 2;
	}

	for (int i = 0; i < 3; i++)
	{
		if (i + scroll < settingsCount)
		{
			_lcd.setCursor(0, i + 1);
			_lcd.print(settingsNames[i + scroll]);
			_lcd.setCursor(16, i + 1);
			_lcd.print(String(settingsValues[i + scroll]));
		}
	}
}
