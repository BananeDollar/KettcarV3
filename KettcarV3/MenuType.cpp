#include "MenuType.h"

void MenuType::MoveCursor(int newPos)
{
	// Delete old Cursor
	_lcd->setCursor(19, _currentCursorPosition);
	_lcd->print(" ");

	// Calculate New cursor Pos and Constrain it
	_currentCursorPosition += newPos;

	if (_currentCursorPosition > _maxCursorPosition)
	{
		_currentCursorPosition = _maxCursorPosition;
	}
	if (_currentCursorPosition < 0)
	{
		_currentCursorPosition = 0;
	}

	// Draw New Cursor
	_lcd->setCursor(19, _currentCursorPosition);
	_lcd->print("<");
}

int MenuType::GetCursorPosition()
{
	return _currentCursorPosition;
}

MenuType::MenuType(LiquidCrystal_I2C* lcd)
{
	Serial.println("BaseClass MenuType::MenuType");
	_lcd = lcd;
}

void MenuType::Draw()
{
	Serial.println("BaseClass MenuType::Draw");
}

void MenuType::OnScroll(int cursorChange)
{
	Serial.println("BaseClass MenuType::OnScroll");
}

void MenuType::OnClick()
{
	Serial.println("BaseClass MenuType::OnClick");
}
