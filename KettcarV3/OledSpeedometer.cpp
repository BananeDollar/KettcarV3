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

void OledSpeedometer::Update(float currentSpeed) // - 128 x 64
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
	for (int i = 0; i < 11; i++)
	{
		int textX = 64 + 50 * -cos(i * 0.3141592);
		int textY = 52 + 50 * -sin(i * 0.3141592);

		_oled.setCursor(textX, textY);
		_oled.println(i * 5);
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