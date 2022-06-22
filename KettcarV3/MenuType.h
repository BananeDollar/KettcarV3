#pragma once
#include <LiquidCrystal_I2C.h>
#include "PCF8574.h"
#include "Settings.h"

typedef void (*IntCallback)(int newMenu);
typedef void (*SimpleCallback)();

class MenuType
{
private:
	int _currentCursorPosition;
	int _currentScroll;
protected:
	int _maxCursorPosition;
	LiquidCrystal_I2C* _lcd;
	void MoveCursor(int);
	int GetCursorPosition();
	int GetCurrentScroll();
	IntCallback _menuChangeRequest;
	void DrawCursor();
public:
	MenuType(LiquidCrystal_I2C*, IntCallback);
	virtual void Draw();
	virtual void OnScroll(int cursorChange);
	virtual void OnClick();
};