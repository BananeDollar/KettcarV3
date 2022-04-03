// LCDMenu.h

#ifndef _LCDMENU_h
#define _LCDMENU_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include"Settings.h"

class LCDMenu
{
protected:
	void DrawCursor();
	void DrawSettingsScreen();
	void DrawMainScreen();
public:
	const String settingsNames[settingsCount] = { "MaxSpd","MaxPdThr","MaxRmtThr" };
	int settingsValues[settingsCount];
	LCDMenu();
	void init();
	void ButtonPress(int);
	void RotaryInput(int);
	void SetSpeed(int);
};

#endif
