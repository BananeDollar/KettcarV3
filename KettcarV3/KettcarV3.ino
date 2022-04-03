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

/*
-- I2C Adressen --
0x27 LCD
0x3C OLED - 128 x 64
-- I2C Adressen --
*/

// -- Screens --
OledSpeedometer speedometer;
LCDMenu lcdMenu;

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

int hallBaseLineReading = 0;
int footPedalBaseLineReading = 0;

int directThrottle;
int wirelessThrottle;

int maxPedalThrottle = 100;
int maxRemoteThrottle = 10;

int currentThrottle = 0;

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
	
	lcdMenu.init();
	speedometer.init();
	
	InitRadio();
	Serial.println("Initialized");

	//hallBaseLineReading = analogRead(pin_hallSensor);
	currentThrottle = 0;

	footPedalBaseLineReading = analogRead(pin_footPedal);

	digitalWrite(pin_BatteryA, LOW);
	digitalWrite(pin_BatteryB, LOW);
	digitalWrite(pin_BatteryC, LOW);

	lcdMenu.AddSetttingsChangeListener(UpdateSpeedometerSettings, MaxSpeedSettingIndex);
	lcdMenu.AddSetttingsChangeListener(UpdateSpeedometerSettings, SpeedometerGranularitySettingIndex);
	lcdMenu.AddSetttingsChangeListener(UpdateMaxPedalThrottle, MaxPedalThrottleSettingIndex);
	lcdMenu.AddSetttingsChangeListener(UpdateMaxRemoteThrottle, MaxRemoteThrottleSettingIndex);
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

void InitIO()
{
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
}

void InitRadio()
{
	radio.setPALevel(RF24_PA_MIN);
	radio.begin();
	radio.openReadingPipe(0, address);
	//radio.printDetails();
	radio.startListening();
}
#pragma endregion

#pragma region Events
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

void loop(void)
{
	int executionTime = millis();
	
	int directThrottle = map(analogRead(pin_footPedal), footPedalBaseLineReading, 4095, 0, 100);

	if (radio.available(0))
	{
		lastRecievedTime = millis();
		int values[1];
		radio.read(&values, sizeof(values));
		wirelessThrottle = values[0];

		if (directThrottle == 0) // Only use wireless Value when foot pedal is not pressed
		{
			currentThrottle = map(wirelessThrottle,0,100, 0, maxRemoteThrottle);
		}
	}
	
	if (directThrottle != 0) // if foot pedal is pressed
	{
		currentThrottle = map(directThrottle, 0, 100, 0, maxPedalThrottle);
	}
	else
	{
		if (millis() > lastRecievedTime + wirelessTimeoutDelay) // wireless timeout 
		{
			currentThrottle = 0;
		}
	}
	
	if (rotaryEncoder.getPosition() != 0)
	{
		lcdMenu.RotaryInput(-rotaryEncoder.getPosition());
		rotaryEncoder.setPosition(0);
	}

	if (buttonInput)
	{
		lcdMenu.ButtonPress(buttonInput);
		buttonInput = 0;
	}

	int speed = currentThrottle / 2;
	lcdMenu.SetSpeed(speed);
	speedometer.Update(speed);

	//lcdMenu.WriteExecutionTime(millis() - executionTime);
}
