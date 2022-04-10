#include "KetcarMenu.h"

KettcarMenu::KettcarMenu(LiquidCrystal_I2C* lcd, PCF8574* ioExpander)
{
	_lcd = lcd;
	_ioExpander = ioExpander;
}

void KettcarMenu::Init(MenuType* pointers...)
{
	// Menu Count 
	for (int i = 0; i < 2; i++)
	{
		_subMenus[i] = &pointers[i];
	}
	
	_lcd->init();
	_lcd->backlight();
	_lcd->clear();
	_lcd->println("Started");

	_currentMenu = 0;
}

void KettcarMenu::Draw()
{
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

MainMenu* KettcarMenu::GetMainMenu()
{
	return (MainMenu*)_subMenus[0];
}

SettingsMenu* KettcarMenu::GetSettingsMenu()
{
	return (SettingsMenu*)_subMenus[1];
}