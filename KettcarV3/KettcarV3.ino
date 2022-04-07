#include "LCDMenu.h"
#include "OledSpeedometer.h"
#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <printf.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <map>
#include <RotaryEncoder.h>
#include "Settings.h"
#include "PCF8574.h"

/*
-- I2C Adressen --
0x20 IO_Expander PCF8574
0x27 LCD
0x3C OLED - 128 x 64
-- I2C Adressen --
*/

// -- Screens --
OledSpeedometer speedometer;
LCDMenu lcdMenu;

PCF8574 io_expander(0x20);

// -- Rotary Encoder --
RotaryEncoder rotaryEncoder(pin_rotaryA, pin_rotaryB);

// -- RF24 - Radio --
RF24 radio(16, 17); // CE, CSN Radio
const byte address[6] = "RMCMD";

OneWire oneWire(pin_thermometer);
DallasTemperature thermometers(&oneWire);

// -- Settings --
int wirelessTimeoutDelay = 400; // Milliseconds

// -- Changing --
long lastRecievedTime;

int buttonInput = 0; // 0 = no input, 1 = pressed, 2 = released

int hallCount = 0;

bool enableWireless = false;

bool isInReverse = false;

int pedalDeadzone = 0;

int directThrottle;
int wirelessThrottle;
int wirelessSteer;
int currentSteer;

int maxPedalThrottle = 100;
int maxRemoteThrottle = 10;

int currentThrottle = 0;
int currentSpeed = 0;

// -- Header --
void IRAM_ATTR HallInterrupt();
void IRAM_ATTR OnRotaryEncoder();
void IRAM_ATTR OnButtonPress();

#pragma region Init / Setup

void setup()
{
	InitIO();

	delay(200);
	Serial.begin(9600);
	printf_begin();

	thermometers.begin();
	
	lcdMenu.init(OnMainMenuToggles);
	speedometer.init();
	
	InitRadio();

	Serial.print("Serial Expander Found:");
	Serial.println(io_expander.isConnected());

	Serial.println("Initialized");

	//hallBaseLineReading = analogRead(pin_hallSensor);
	currentThrottle = 0;

	digitalWrite(pin_BatteryA, LOW);
	digitalWrite(pin_BatteryB, LOW);
	digitalWrite(pin_BatteryC, LOW);

	lcdMenu.AddSetttingsChangeListener(UpdateSpeedometerSettings, MaxSpeedSettingIndex);
	lcdMenu.AddSetttingsChangeListener(UpdateSpeedometerSettings, SpeedometerGranularitySettingIndex);
	lcdMenu.AddSetttingsChangeListener(UpdateMaxPedalThrottle, MaxPedalThrottleSettingIndex);
	lcdMenu.AddSetttingsChangeListener(UpdateMaxRemoteThrottle, MaxRemoteThrottleSettingIndex);
	lcdMenu.AddSetttingsChangeListener(UpdatePedalDeadzone, PedalDeadzoneSettingIndex);

	lcdMenu.SetSetting(PedalDeadzoneSettingIndex, analogRead(pin_footPedal) + 50);
}

void InitIO()
{
	pinMode(pin_steerPotiA, INPUT_PULLUP);

	pinMode(pin_rotaryA, INPUT_PULLUP); // Encoder
	pinMode(pin_rotaryB, INPUT_PULLUP); // Encoder
	attachInterrupt(pin_rotaryA, OnRotaryEncoder, CHANGE); // Encoder
	attachInterrupt(pin_rotaryB, OnRotaryEncoder, CHANGE); // Encoder
	
	pinMode(pin_button, INPUT_PULLUP); // Button
	attachInterrupt(pin_button, OnButtonPress, CHANGE); // Button

	pinMode(pin_hallSensor, INPUT); // Hall Sensor
	attachInterrupt(pin_hallSensor, HallInterrupt, FALLING); // Hall Sensor

	pinMode(pin_footPedal, INPUT); // Foot Pedal

	pinMode(pin_BatteryA, OUTPUT); // Battery A
	pinMode(pin_BatteryB, OUTPUT); // Battery B
	pinMode(pin_BatteryC, OUTPUT); // Battery C

	io_expander.begin();

	io_expander.write(0, HIGH);
	io_expander.write(1, HIGH);
	io_expander.write(2, HIGH);
	io_expander.write(3, HIGH);
}

void InitRadio()
{
	radio.setPALevel(RF24_PA_MIN);

	Serial.print("RF24 Found:");
	Serial.println(radio.begin());

	radio.openReadingPipe(0, address);
	//radio.printDetails();
	radio.stopListening();
}
#pragma endregion

#pragma region Events

void OnMainMenuToggles(int value, int index) {
	switch (index)
	{
	case 0: // Reverse Tempomat
		if (currentSpeed == 0)
		{
			isInReverse = !isInReverse;
			lcdMenu.SetReverse(isInReverse);
			io_expander.write(0, isInReverse ? LOW:HIGH);
		}
		break;
	case 1: // Wireless
		if (enableWireless)
		{
			// Disable Wireless. Allways Possible
			enableWireless = false;
			lcdMenu.SetWirelessSignal(enableWireless, false);
			radio.stopListening();
		}
		else
		{
			// Enable Wireless. Only when standing still
			if (currentSpeed == 0)
			{
				enableWireless = true;
				lcdMenu.SetWirelessSignal(enableWireless, false);
				radio.startListening();
			}
		}
		break;
	}
}

void UpdatePedalDeadzone(int value, int index)
{
	pedalDeadzone = value;
}

void UpdateSpeedometerSettings(int value, int index)
{
	speedometer.UpdateSettings(value, index);
}

void UpdateMaxPedalThrottle(int value, int index)
{
	maxPedalThrottle = value;
}

void UpdateMaxRemoteThrottle(int value, int index)
{
	maxRemoteThrottle = value;
}

void IRAM_ATTR HallInterrupt()
{
	hallCount++;
}

void IRAM_ATTR OnRotaryEncoder()
{
	rotaryEncoder.tick();
}

void IRAM_ATTR OnButtonPress()
{
	if (!digitalRead(pin_button)) // button is pulldown
	{
		buttonInput = 1;
	}
	else
	{
		buttonInput = -1;
	}
}

#pragma endregion

void UpdateSteer()
{
	currentSteer = map(analogRead(pin_steerPotiA), 0, 4095, 0, 100);
	int diff = currentSteer - wirelessSteer;
	
	//lcdMenu.DisplayDebugValue(((String)diff) + "  ");

	if (diff < -10)
	{
		io_expander.write(2, HIGH);
		io_expander.write(3, LOW);
	}
	else if (diff > 10)
	{
		io_expander.write(2, LOW);
		io_expander.write(3, HIGH);
	}
	else
	{
		io_expander.write(2, HIGH);
		io_expander.write(3, HIGH);
	}
}

void loop(void)
{
	int executionTime = millis();
	
	int directThrottle = max((int)map(analogRead(pin_footPedal), pedalDeadzone, 4095, 0, maxPedalThrottle),0);

	if (enableWireless)
	{
		if (radio.available(0))
		{
			lastRecievedTime = millis();
			int values[2];
			radio.read(&values, sizeof(values));
			wirelessThrottle = values[0];
			wirelessSteer = values[1];
			lcdMenu.SetWirelessSignal(enableWireless, true);

			if (directThrottle == 0) // Only use wireless Value when foot pedal is not pressed
			{
				currentThrottle = map(wirelessThrottle, 0, 100, 0, maxRemoteThrottle);
			}
		}else if (millis() > lastRecievedTime + wirelessTimeoutDelay) // wireless timeout 
		{
			currentThrottle = 0;
			lcdMenu.SetWirelessSignal(enableWireless,false);
		}

		UpdateSteer();
	}
	else
	{
		currentThrottle = directThrottle;
	}
	
	if (rotaryEncoder.getPosition() != 0)
	{
		lcdMenu.RotaryInput(rotaryEncoder.getPosition());
		rotaryEncoder.setPosition(0);
	}

	if (buttonInput)
	{
		lcdMenu.ButtonPress(buttonInput);
		buttonInput = 0;
	}

	currentSpeed = currentThrottle; // TODO.... Hall Reading Magic
	lcdMenu.SetSpeed(currentSpeed);
	speedometer.Update(currentSpeed);
	
	lcdMenu.WriteExecutionTime(millis() - executionTime);
}
