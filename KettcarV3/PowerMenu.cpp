// 
// 
// 

#include "PowerMenu.h"

const int _batteryPins[] = { pin_BatteryA ,pin_BatteryB ,pin_BatteryC };
const String _batteryNames[] = {"None", "A", "B", "C"};

PowerMenu::PowerMenu(LiquidCrystal_I2C* lcd, IntCallback menuChangeRequest, int* currentMenuLevel) : MenuType(lcd, menuChangeRequest, currentMenuLevel)
{
	_maxCursorPosition = 3;
}

void PowerMenu::Init()
{
	digitalWrite(pin_PowerSwitch, false);
	digitalWrite(_batteryPins[0], false);
	digitalWrite(_batteryPins[1], false);
	digitalWrite(_batteryPins[2], false);
	SetMainPower(false);
	_currenttBatery = -1;
	_targetbatterySelection = -1;
}

void PowerMenu::Draw()
{
	_lcd->clear();

	_lcd->setCursor(0, 0);
	_lcd->print("- Power Settings - ");

	_lcd->setCursor(0, 3);
	_lcd->print("Battery:");
	_lcd->print(_batteryNames[_targetbatterySelection+1]);

	if (_selectingBattery)
	{
		_lcd->setCursor(19, 3);
		_lcd->print("*");
	}
	else
	{
		DrawCursor();
	}
}

void PowerMenu::UpdateVoltage(float lowVoltage, float highVoltage)
{
	if (isOpen())
	{
		_lcd->setCursor(0, 1);
		_lcd->print("LV:");
		_lcd->print(lowVoltage);
		_lcd->print("  ");

		_lcd->setCursor(10, 1);
		_lcd->print("HV:");
		_lcd->print(highVoltage);
		_lcd->print("  ");
	}
}

void PowerMenu::SetStatusText(String text)
{
	_lcd->setCursor(0, 2);
	_lcd->print(text);
}

void PowerMenu::OnScroll(int cursorChange)
{
	if (!_selectingBattery)
	{
		MoveCursor(cursorChange);
	}
	else
	{
		_targetbatterySelection = min(2,max(-1, _targetbatterySelection+cursorChange));
		Draw();
	}
}

void PowerMenu::OnClick()
{
	switch (GetCursorPosition())
	{
	case 0:
		_menuChangeRequest(0);
		break;
	case 3:
		if (!_selectingBattery)
		{
			// Enter Menu
			_selectingBattery = true;

			_lcd->setCursor(19, 3);
			_lcd->print("*");
		}
		else
		{
			_selectingBattery = false;
			// Exit Menu and Apply changes
			SwitchToBattery(_targetbatterySelection);
		}
		break;
	}
}

void PowerMenu::SwitchToBattery(int batteryIndex)
{
	if (_currenttBatery != batteryIndex)
	{
		SetStatusText("Switching:"+ _batteryNames[_currenttBatery+1] + " -> " + _batteryNames[batteryIndex+1]);
		SetMainPower(false);
		delay(500);
		digitalWrite(_batteryPins[_currenttBatery], false);
		_currenttBatery = batteryIndex;
		if (_currenttBatery != -1) // if target is a battery
		{
			delay(500);
			digitalWrite(_batteryPins[_currenttBatery], true);
			delay(500);
			SetMainPower(true);
		}
		Draw();
	}
}

void PowerMenu::SetMainPower(bool useMainBattery)
{
	if (_mainBatteryPower != useMainBattery) // only switch if value changed
	{
		_mainBatteryPower = useMainBattery;
		digitalWrite(pin_PowerSwitch, useMainBattery);
	}
}
