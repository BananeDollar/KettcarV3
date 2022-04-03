// LCDMenu.h

#ifndef _LCDMENU_h
#define _LCDMENU_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include"Settings.h"
#include <list>

class LCDMenu
{
	typedef void (*SettingsListener)(int value, int index);
protected:
	void DrawCursor();
	void DrawSettingsScreen();
	void DrawMainScreen();
	SettingsListener onSettingsChangeList[settingsCount]; // one listener per setting
public:
	const String settingsNames[settingsCount] = { "MaxSpd","SpdMtrGran","MaxPdThr","MaxRmtThr" };
	const int minSettingsValues[settingsCount] = { 1,1,0,0 };
	const int maxSettingsValues[settingsCount] = { 100,20,100,100 };
	int settingsValues[settingsCount];
	LCDMenu();
	void init();
	void ButtonPress(int);
	void RotaryInput(int);
	void SetSpeed(int);
	void WriteExecutionTime(long);
	void AddSetttingsChangeListener(SettingsListener listener, int settingsIndex);
};

#endif
