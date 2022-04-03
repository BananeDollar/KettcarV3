// OledSpeedometer.h

#ifndef _OLEDSPEEDOMETER_h
#define _OLEDSPEEDOMETER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class OledSpeedometer
{
protected:
	// timeout
	bool _enableTimeout;
	float _timeoutCountdown;
	
public:
	OledSpeedometer();
	void init();
	void Update(float);
	void EnableTimeout(bool);
};

#endif

