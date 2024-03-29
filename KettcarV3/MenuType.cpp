#include "MenuType.h"

void MenuType::MoveCursor(int newPos)
{
	// Delete old Cursor
	_lcd->setCursor(19, _currentCursorPosition - _currentScroll);
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

	if (_currentCursorPosition > 2 && _maxCursorPosition > 3)
	{
		if (_currentCursorPosition == _maxCursorPosition)
		{
			_currentScroll = _currentCursorPosition - 3;
		}
		else
		{
			_currentScroll = _currentCursorPosition - 2;
		}		
	}
	else
	{
		_currentScroll = 0;
	}
	
	DrawCursor();
}

void MenuType::DrawCursor()
{
	_lcd->setCursor(19, _currentCursorPosition - _currentScroll);
	_lcd->print("<");
}

/// <summary>
/// Check if this Menu is currently open
/// </summary>
/// <returns>open</returns>
bool MenuType::isOpen()
{
	return (*_currentMenuLevel == _myMenuIndex);
}

int MenuType::GetCursorPosition()
{
	return _currentCursorPosition;
}

int MenuType::GetCurrentScroll()
{
	return _currentScroll;
}

MenuType::MenuType(LiquidCrystal_I2C* lcd, IntCallback menuChangeRequest, int* menuLevel)
{
	_lcd = lcd;
	_menuChangeRequest = menuChangeRequest;
	_currentMenuLevel = menuLevel;
	
}

void MenuType::Init()
{
}

void MenuType::Draw()
{
}

void MenuType::OnScroll(int cursorChange)
{
}

void MenuType::OnClick()
{
}

void MenuType::SetMenuIndex(int index)
{
	_myMenuIndex = index;
}
