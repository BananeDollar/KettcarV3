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
	
	_lcd->init();
	_lcd->backlight();
	_lcd->clear();
	_lcd->println("Started");

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

void KettcarMenu::StartOTAUpdate()
{
	// Clear lcd and write the text "OTA-Update Started" in the second row
	_lcd->clear();
	_lcd->setCursor(0, 1);
	_lcd->print("OTA-Update Started");
}

void KettcarMenu::OTAUpdateFinished()
{
	// the same as StartOTAUpdate but the Text is "Rebooting..."
	_lcd->clear();
	_lcd->setCursor(0, 1);
	_lcd->print("Rebooting...");
}
