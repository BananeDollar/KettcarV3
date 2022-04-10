#include "KetcarMenu.h"

KettcarMenu::KettcarMenu(LiquidCrystal_I2C* lcd, PCF8574* ioExpander)
{
	_lcd = lcd;
	_ioExpander = ioExpander;
}

void KettcarMenu::Init()
{
	MainMenu _mainMenu(_lcd, _ioExpander);
	SettingsMenu _settingsMenu(_lcd);

	*_subMenus = new MenuType[2]{ _mainMenu, _settingsMenu };
	
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
	Serial.println("KettcarMenu::OnScroll");
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