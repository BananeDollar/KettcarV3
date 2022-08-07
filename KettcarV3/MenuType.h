#pragma once
#include <LiquidCrystal_I2C.h>
#include "PCF8574.h"
#include "Settings.h"

typedef void (*IntCallback)(int newMenu);
typedef void (*BoolCallback)(bool value);
typedef void (*SimpleCallback)();

class MenuType
{
private:
	int _currentCursorPosition;
	int _currentScroll;
	int _myMenuIndex = 0;
protected:
	int* _currentMenuLevel;
	int _maxCursorPosition;
	LiquidCrystal_I2C* _lcd;
	void MoveCursor(int);
	int GetCursorPosition();
	int GetCurrentScroll();
	IntCallback _menuChangeRequest;
	void DrawCursor();
	bool isOpen();
public:
	MenuType(LiquidCrystal_I2C* lcd, IntCallback menuChangeRequest, int* currentMenuLevel);
	virtual void Init();
	virtual void Draw();
	virtual void OnScroll(int cursorChange);
	virtual void OnClick();
	void SetMenuIndex(int index);
};