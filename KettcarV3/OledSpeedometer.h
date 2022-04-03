// OledSpeedometer.h

#ifndef _OLEDSPEEDOMETER_h
#define _OLEDSPEEDOMETER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include"Settings.h"

class OledSpeedometer
{
protected:
	// timeout
	bool _enableTimeout;
	float _timeoutCountdown;
	int _maxSpeed = 100;
	int _granularity = 10;
public:
	OledSpeedometer();
	void init();
	void Update(int);
	void EnableTimeout(bool);
	void UpdateSettings(int, int);
};

#endif

