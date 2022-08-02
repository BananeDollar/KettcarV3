#include "KetcarMenu.h"

KettcarMenu::KettcarMenu(LiquidCrystal_I2C* lcd, PCF8574* ioExpander)
{
	_lcd = lcd;
	_ioExpander = ioExpander;
}

void KettcarMenu::Init(MenuType* a, MenuType* b)
{
	// Menu Count 
	
	_subMenus[0] = a;
	_subMenus[1] = b;

	_currentMenu = 0;
}

void KettcarMenu::Draw()
{
	_lcd->clear();
	_subMenus[_currentMenu]->Draw();
}

void KettcarMenu::OnScroll(int cursorChange)
{
	_subMenus[_currentMenu]->OnScroll(cursorChange);
}

void KettcarMenu::OnClick()
{
	_subMenus[_currentMenu]->OnClick();
}

void KettcarMenu::SetMenu(int newMenu)
{
	_currentMenu = newMenu;
	Draw();
}