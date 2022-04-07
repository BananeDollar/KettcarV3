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
	void DrawWirelessStatus();
	void DrawDirectionText();
	SettingsListener onSettingsChangeList[settingsCount]; // one listener per setting
	SettingsListener onMenuToggle; // main Menu Toggles
public:
	const String settingsNames[settingsCount] = { "Max_Speed", "OLED_Steps", "Pedal_THR", "Pedal_DEAD", "Remote_THR" };
	const int minSettingsValues[settingsCount] = { 1, 1, 0, 0, 0 };
	const int moveSteps[settingsCount] = { 1, 1, 5, 5, 5 };
	const int maxSettingsValues[settingsCount] = { 100 ,20, 100, 4095 ,100 };
	int settingsValues[settingsCount];
	LCDMenu();
	void init(SettingsListener onMenuToggle);
	void ButtonPress(int);
	void RotaryInput(int);
	void SetSpeed(int);
	void WriteExecutionTime(long);
	void SetWirelessSignal(bool,bool);
	void AddSetttingsChangeListener(SettingsListener listener, int settingsIndex);
	void SetSetting(int, int);
	void SetReverse(bool);
	void DisplayDebugValue(String);
};

#endif
