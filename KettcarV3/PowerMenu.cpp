// 
// 
// 

#include "PowerMenu.h"

PowerMenu::PowerMenu(LiquidCrystal_I2C* lcd, IntCallback menuChangeRequest, int* currentMenuLevel) : MenuType(lcd, menuChangeRequest, currentMenuLevel)
{
	_maxCursorPosition = 3;
}

void PowerMenu::Init()
{
}

void PowerMenu::Draw()
{
	_lcd->clear();

	_lcd->setCursor(0, 0);
	_lcd->print("- Power Settings - ");

	_lcd->setCursor(0, 3);
	_lcd->print("Battery");

	DrawCursor();
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

void PowerMenu::OnScroll(int cursorChange)
{
	MoveCursor(cursorChange);
}

void PowerMenu::OnClick()
{

}

void PowerMenu::SwitchToBattery(int batteryIndex)
{

}

void PowerMenu::SetMainPower(bool useMainBattery)
{
	
}
