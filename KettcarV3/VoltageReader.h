// VoltageReader.h

#ifndef _VOLTAGEREADER_h
#define _VOLTAGEREADER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class VoltageReader
{
private:
	float* _voltageBuffer;
	int _voltageBufferIndex;
	int _bufferSize;
	int _pin;
	/// <summary>
	/// Vin -> R1 -> ESP32 -> R2 -> GND
	/// </summary>
	float _r1;
	/// <summary>
	/// Vin -> R1 -> ESP32 -> R2 -> GND
	/// </summary>
	float _r2;
public:
	VoltageReader(int pin, int r1, int r2, int bufferSize);
	void Update();
	float GetVoltage();
};

#endif