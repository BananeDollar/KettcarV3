#include "MainMenu.h"
#include <Arduino.h>

byte upArrow[8] = {
  B00100,
  B01110,
  B10101,
  B00100,
  B00100,
  B00100,
  B00100,
};
byte downArrow[8] = {
  B00100,
  B00100,
  B00100,
  B00100,
  B10101,
  B01110,
  B00100,
  B00000
};
byte cogWheel[8] = {
  B00000,
  B10101,
  B01110,
  B11011,
  B01110,
  B10101,
  B00000,
  B00000
};

// https://www.pixilart.com/draw

MainMenu::MainMenu(LiquidCrystal_I2C* lcd, PCF8574* io_expander, IntCallback menuChangeRequest) : MenuType(lcd, menuChangeRequest)
{
	_maxCursorPosition = 3;
	_ioExpander = io_expander;
}

void MainMenu::Init()
{
	_lcd->createChar(0, upArrow);
	_lcd->createChar(1, downArrow);
}

void MainMenu::Draw()
{
	_lcd->clear();
	DrawDirectionText();
	DrawWirelessStatus();
	_lcd->setCursor(0,3);
	_lcd->print("Einstellungen");
	DrawCursor(); //Draw start cursor
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
	case 3: // Settings
		_menuChangeRequest(1);
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
		_lcd->print("R\xF5\ckw\xE1rts ");
		_lcd->setCursor(10,0);
		_lcd->write(byte(0));
	}
	else
	{
		_lcd->print("Vorw\xE1rts ");
		_lcd->setCursor(10, 0);
		_lcd->write(byte(1));
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