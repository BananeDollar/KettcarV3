#include "MainMenu.h"
#include <Arduino.h>

MainMenu::MainMenu(LiquidCrystal_I2C* lcd, PCF8574* io_expander, KettcarMenu* menu) : MenuType(lcd)
{
	_menu = menu;
	_maxCursorPosition = 3;
	_ioExpander = io_expander;
}

void MainMenu::Draw()
{
	DrawDirectionText();
	DrawWirelessStatus();
	MoveCursor(0); //Draw start cursor
}

void MainMenu::OnScroll(int cursorChange)
{
	MoveCursor(cursorChange);
}

void MainMenu::OnClick()
{
	switch (GetCursorPosition())
	{
	case 0: // Reverse Tempomat
		if (_currentSpeed == 0)
		{
			_reverse = !_reverse;
			DrawDirectionText();
			_ioExpander->write(0, _reverse ? LOW : HIGH);
		}
		break;
	case 1: // Wireless
		if (_wirelessEnabled)
		{
			// Disable Wireless. Allways Possible
			_wirelessEnabled = false;
		}
		else
		{
			// Enable Wireless. Only when standing still
			if (_currentSpeed == 0)
			{
				_wirelessEnabled = true;
			}
		}
		DrawWirelessStatus();
		break;
	}
}

void MainMenu::SetWirelessSignal(bool signal)
{
	if (signal != _wirelessSignal)
	{
		_wirelessSignal = signal;
		DrawWirelessStatus();
	}
}

bool MainMenu::GetWirelessEnabled()
{
	return _wirelessEnabled;
}

void MainMenu::UpdateCurrentSpeed(int speed)
{
	_currentSpeed = speed;
}

void MainMenu::DrawDirectionText()
{
	_lcd->setCursor(0, 0);
	if (_reverse)
	{
		_lcd->print("R\xF5\ckw\xE1rts");
	}
	else
	{
		_lcd->print("Vorw\xE1rts");
	}
}

void MainMenu::DrawWirelessStatus()
{
	_lcd->setCursor(0, 1);
	if (!_wirelessEnabled)
	{
		_lcd->print("Manual    ");
	}
	else
	{
		if (_wirelessSignal)
		{
			_lcd->print("- REMOTE -");
		}
		else
		{
			_lcd->print("NO  SIGNAL");
		}
	}
}