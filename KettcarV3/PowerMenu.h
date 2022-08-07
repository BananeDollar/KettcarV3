// PowerMenu.h

#include "MenuType.h"
#pragma once

class PowerMenu : public MenuType
{
private:
	/// <summary>
	/// false = 12v Batery, true = main Batery
	/// </summary>
	bool _mainBatteryPower = false;
	/// <summary>
	/// -1 = no Battery, 1-3 Batteryindex
	/// </summary>
	int _currenttBatery = -1;

	/// <summary>
	/// Set the Relays to fit target Status.
	/// WARNING! USES DELAYS
	/// </summary>
	void SwitchToBattery(int batteryIndex);

	/// <summary>
	/// Sets the main Power Relay to fit target
	/// WARNING! USES DELAYS
	/// </summary>
	void SetMainPower(bool useMainBattery);
public:
	PowerMenu(LiquidCrystal_I2C* lcd, IntCallback menuChangeRequest, int* currentMenuLevel);
	void Init();
	void Draw();
	void UpdateVoltage(float lowVoltage, float highVoltage);
	void OnScroll(int cursorChange);
	void OnClick();
};


