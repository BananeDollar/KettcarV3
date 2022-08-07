#include "KetcarMenu.h"

KettcarMenu::KettcarMenu(LiquidCrystal_I2C* lcd, PCF8574* ioExpander)
{
	_lcd = lcd;
	_ioExpander = ioExpander;
}

void KettcarMenu::Init(MenuType* menus[])
{
	// Menu Count 
	
	for (int i = 0; i < 3; i++)
	{
		_subMenus[i] = menus[i];
		_subMenus[i]->Init();
		_subMenus[i]->SetMenuIndex(i);
	}

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