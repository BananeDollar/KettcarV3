#pragma once
#include <LiquidCrystal_I2C.h>

class MenuType
{
private:
	int _currentCursorPosition;
protected:
	int _maxCursorPosition;
	LiquidCrystal_I2C* _lcd;
	void MoveCursor(int);
	int GetCursorPosition();
public:
	MenuType(LiquidCrystal_I2C*);
	virtual void Draw();
	virtual void OnScroll(int cursorChange);
	virtual void OnClick();
};