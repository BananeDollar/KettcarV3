// 
// 
// 

#include "VoltageReader.h"

VoltageReader::VoltageReader(int pin, int r1, int r2, int bufferSize)
{
	_r1 = r1;
	_r2 = r2;
	_bufferSize = bufferSize;
	_voltageBuffer = new float[bufferSize];
	_pin = pin;
}


void VoltageReader:: Update()
{
	// update Buffer Index
	if (_voltageBufferIndex < _bufferSize)
		_voltageBufferIndex++;
	else
		_voltageBufferIndex = 0;

	// fill Buffer at current index
	_voltageBuffer[_voltageBufferIndex] = analogRead(_pin);
}

float VoltageReader::GetVoltage()
{
	// calculate median of last 10 analog Reads
	float bufferMedian = 0;
	for (int i = 0; i < sizeof(_voltageBuffer); i++)
	{
		bufferMedian += _voltageBuffer[i];
	}
	bufferMedian = bufferMedian / float(_bufferSize);

	// calulate voltage at pin using bufferMedian
	float pinVoltage = (bufferMedian / 4095.0 * 5);
	// calucate input Voltage with Voltage Devider
	return pinVoltage * ((_r1 + _r2) / _r2);
	
	// debug return the raw pin value
	//return bufferMedian;
}
