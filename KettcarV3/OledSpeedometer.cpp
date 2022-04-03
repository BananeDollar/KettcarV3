// 
// 
// 

#include "OledSpeedometer.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>

Adafruit_SH1106 _oled;

OledSpeedometer::OledSpeedometer()
{
	_enableTimeout = true;
}

// create init function
void OledSpeedometer::init()
{
	_oled.begin(SH1106_SWITCHCAPVCC, 0x3C);
	_oled.clearDisplay();
	_oled.setTextSize(1);
	_oled.setTextColor(WHITE);

	_oled.println("Started!");
	_oled.display();
}

void OledSpeedometer::EnableTimeout(bool enableTimeout)
{
	_enableTimeout = enableTimeout;
	_timeoutCountdown = 0;
}

void OledSpeedometer::UpdateSettings(int value, int index)
{
	switch (index)
	{
	case MaxSpeedSettingIndex:
		_maxSpeed = value;
		break;
	case SpeedometerGranularitySettingIndex:
		_granularity = value;
		break;
	}
	_timeoutCountdown = 0;
}

void OledSpeedometer::Update(int currentSpeed) // - 128 x 64
{
	if (_enableTimeout)
	{
		if (currentSpeed == 0)
		{
			if (_timeoutCountdown == 0)
			{
				_timeoutCountdown = millis();
			}
			else if (millis() - _timeoutCountdown > 2000)
			{
				_oled.clearDisplay();
				_oled.display();
				return;
			}
		}
		else
		{
			_timeoutCountdown = 0;
		}
	}

	 // Replace with messurement
	// Clear Oled
	_oled.clearDisplay();

	// Gauge Line
	int gaugeEndX = 64 + 43 * -cos(currentSpeed * 2 * 0.03141592);
	int gaugeEndY = 55 + 43 * -sin(currentSpeed * 2 * 0.03141592);

	int gaugeOriX = 64 + 20 * -cos(currentSpeed * 2 * 0.03141592);
	int gaugeOriY = 55 + 20 * -sin(currentSpeed * 2 * 0.03141592);

	_oled.drawLine(gaugeOriX, gaugeOriY, gaugeEndX, gaugeEndY, WHITE);

	// Gauge Texts
	_oled.setTextSize(1);
	for (int i = 0; i < _granularity+1; i++)
	{
		int textX = 64 + 50 * -cos(i * 3.141592 / _granularity);
		int textY = 52 + 50 * -sin(i * 3.141592 / _granularity);

		_oled.setCursor(textX, textY);
		// Value
		_oled.println(i * int(((float)_maxSpeed) / ((float)_granularity)));
	}

	// Current Speed Text
	_oled.setTextSize(3);
	if (currentSpeed < 10)
	{
		_oled.setCursor(60, 40);
	}
	else
	{
		_oled.setCursor(50, 40);
	}
	_oled.println(currentSpeed);

	// Send to Oled
	_oled.display();
}