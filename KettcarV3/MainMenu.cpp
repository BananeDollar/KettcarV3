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
byte wrench[8] = {
  B10001,
  B10001,
  B11111,
  B00100,
  B00100,
  B00100,
  B00100,
  B00100
};

byte cogwheel[8] = {
  B00000,
  B10101,
  B01110,
  B11011,
  B01110,
  B10101,
  B00000,
  B00000
};

byte battery[8] = {
  B01110,
  B11111,
  B10001,
  B10001,
  B10001,
  B10001,
  B10001,
  B11111
};

// https://www.pixilart.com/draw

MainMenu::MainMenu(LiquidCrystal_I2C* lcd, IntCallback menuChangeRequest, BoolCallback setReverse, int* menuLevel) : MenuType(lcd, menuChangeRequest, menuLevel)
{
	_maxCursorPosition = 3;
	_setReverse = setReverse;
}

void MainMenu::Init()
{
	_lcd->init();
	_lcd->backlight();
	
	_lcd->createChar(0, upArrow);
	_lcd->createChar(1, downArrow);
	_lcd->createChar(2, wrench);
	_lcd->createChar(3, battery);
	_lcd->clear();
}

void MainMenu::Draw()
{
	_lcd->clear();
	DrawDirectionText();
	DrawWirelessStatus();
	_lcd->setCursor(0, 2);
	_lcd->print("Power");
	_lcd->setCursor(17, 2);
	_lcd->write((uint8_t)3);
	_lcd->setCursor(0, 3);
	_lcd->print("Einstellungen");
	_lcd->setCursor(17, 3);
	_lcd->write((uint8_t)2);
	
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
	case 0: // Reverse
		if (_currentSpeed == 0)
		{
			_reverse = !_reverse;
			DrawDirectionText();
			_setReverse(_reverse);
		}
		else // Tempomat
		{
		
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
	case 2:
		_menuChangeRequest(2);
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

void MainMenu::drawDebugText(String text)
{
	if (isOpen())
	{
		_lcd->setCursor(0, 2);
		_lcd->print(text + "  ");
	}
}

void MainMenu::DrawDirectionText()
{
	_lcd->setCursor(0, 0);
	if (_reverse)
	{
		_lcd->print("R\xF5\ckw\xE1rts ");
		_lcd->setCursor(17,0);
		_lcd->write((uint8_t)1);
	}
	else
	{
		_lcd->print("Vorw\xE1rts ");
		_lcd->setCursor(17, 0);
		_lcd->write((uint8_t)0);
	}
}

void MainMenu::DrawWirelessStatus()
{
	if (isOpen())
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
}