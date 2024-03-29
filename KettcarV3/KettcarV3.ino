#include "VoltageReader.h"
#include "PowerMenu.h"
#include <ESP32Tone.h>
#include <ESP32PWM.h>
#include <analogWrite.h>
#include "KetcarMenu.h"
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
#include <ESP32Servo.h>
//#include <WiFi.h>
//#include <ArduinoOTA.h>

/*
-- I2C Adressen --
0x20 IO_Expander PCF8574
0x27 LCD
0x3C OLED - 128 x 64
-- I2C Adressen --
*/

// -- Screens --
OledSpeedometer speedometer;

PCF8574 io_expander(0x20);
LiquidCrystal_I2C _lcd(0x27, 20, 4);

void ChangeMenu(int);
void UpdateSpeedometerSettings();
void SetReverse(bool);
KettcarMenu _kettcarMenu(&_lcd, &io_expander);
MainMenu _mainMenu(&_lcd, ChangeMenu, SetReverse , &_kettcarMenu._currentMenu);
SettingsMenu _settingsMenu(&_lcd, ChangeMenu,UpdateSpeedometerSettings, &_kettcarMenu._currentMenu);
PowerMenu _powerMenu(&_lcd, ChangeMenu, &_kettcarMenu._currentMenu);

// -- Rotary Encoder --
RotaryEncoder rotaryEncoder(pin_rotaryA, pin_rotaryB);

// -- RF24 - Radio --
RF24 radio(16,17); // CE, CSN Radio
const byte address[6] = "RMCMD";

OneWire oneWire(pin_thermometer);
DallasTemperature thermometers(&oneWire);

Servo steerServo;

// -- Voltage Readers --

VoltageReader _lowVoltageReader(pin_voltMessureA, 900000, 220000, 5); // 1000000 - 220000
VoltageReader _highVoltageReader(pin_voltMessureB, 1000000, 44000, 5); // 1000000 - 47000

// -- Settings --
const int wirelessTimeoutDelay = 400; // Milliseconds
const bool SerialDebugButtonPress = false;
const bool SerialDebugInit = false;
const int tmeperaturUpdateDelay = 2000;

// -- Changing --
long lastRecievedTime;
int buttonInput = 0; // 0 = no input, 1 = pressed, 2 = released
long lastButtonInputTime = 0;
int hallCount = 0;
int currentThrottle = 0;
int wirelessSteer;
float lastTemperatureUpdateMillis;

// -- Header --
void IRAM_ATTR HallInterrupt();
void IRAM_ATTR OnRotaryEncoder();
void IRAM_ATTR OnButtonPress();

#pragma region Init / Setup

void InitDebugPrint()
{
	Serial.print("PedalDeadzone:");
	Serial.println(analogRead(pin_footPedal) + 50);

	Serial.print("IO Expander Found:");
	Serial.println(io_expander.isConnected());

	Serial.print("I2C Freq:");
	Serial.println(Wire.getClock());
	
	Serial.print("Thermometers:");
	Serial.println(thermometers.getDeviceCount());

	Serial.println("Initialized");
}

void InitWIFI()
{
	/*WiFi.begin("PECS","Llap-3,141");
	WiFi.setHostname("Ketcar");

	ArduinoOTA.setHostname("Ketcar");
	ArduinoOTA.begin();


	ArduinoOTA.onStart([]()
	{
		_kettcarMenu.StartOTAUpdate();
	});

	ArduinoOTA.onEnd([]()
	{
		_kettcarMenu.OTAUpdateFinished();
	});*/
}

void InitIO()
{
	//pinMode(pin_steerPotiA, INPUT_PULLUP);

	pinMode(pin_rotaryA, INPUT_PULLUP); // Encoder
	pinMode(pin_rotaryB, INPUT_PULLUP); // Encoder
	
	pinMode(pin_button, INPUT_PULLUP); // Button

	pinMode(pin_hallSensor, INPUT); // Hall Sensor

	pinMode(pin_footPedal, INPUT); // Foot Pedal
	digitalWrite(pin_footPedal, LOW);

	pinMode(pin_BatteryA, OUTPUT); // Battery A
	pinMode(pin_BatteryB, OUTPUT); // Battery B
	pinMode(pin_BatteryC, OUTPUT); // Battery C
	pinMode(pin_PowerSwitch, OUTPUT); // Power Switch

	digitalWrite(pin_BatteryA, LOW);
	digitalWrite(pin_BatteryB, LOW);
	digitalWrite(pin_BatteryC, LOW);
	digitalWrite(pin_PowerSwitch, LOW);

	steerServo.attach(pin_steerServo);

	pinMode(pin_throttle,OUTPUT); // Throttle
	digitalWrite(pin_throttle, LOW);

	pinMode(pin_voltMessureA, INPUT); // Voltage Sense
	pinMode(pin_voltMessureB, INPUT); // Voltage Sense
	//pinMode(pin_ampMessure, INPUT); // Amperage Sense

	io_expander.begin();

	io_expander.write(0, HIGH);
	io_expander.write(1, HIGH);
	io_expander.write(2, HIGH);
	io_expander.write(3, HIGH);
}

void AttatchInterrupts()
{
	attachInterrupt(pin_rotaryA, OnRotaryEncoder, CHANGE); // Encoder
	attachInterrupt(pin_rotaryB, OnRotaryEncoder, CHANGE); // Encoder
	attachInterrupt(pin_button, OnButtonPress, CHANGE); // Button
	attachInterrupt(pin_hallSensor, HallInterrupt, FALLING); // Hall Sensor
}

void InitRadio()
{
	radio.setPALevel(RF24_PA_MIN);

	Serial.print("RF24 Found:");
	Serial.println(radio.begin());

	radio.openReadingPipe(0, address);
	//radio.printDetails();
	//radio.stopListening();
	radio.startListening();
}

void setup()
{
	InitIO();
	AttatchInterrupts();

	delay(200);
	Serial.begin(9600);
	if (SerialDebugInit)
		Serial.println("Started...");

	//printf_begin();

	thermometers.begin();
	thermometers.setWaitForConversion(false);
	lastTemperatureUpdateMillis = -tmeperaturUpdateDelay; // to force update on startup

	MenuType* menus[] = {&_mainMenu , &_settingsMenu, &_powerMenu};
	_kettcarMenu.Init(menus);
	
	speedometer.init();

	InitRadio();

	_settingsMenu.SetSettingValue(PedalDeadzoneSettingIndex, analogRead(pin_footPedal) + 50);

	Wire.setClock(400000);

	_kettcarMenu.Draw();

	//InitWIFI();

	steerServo.write(90);

	InitDebugPrint();
}
#pragma endregion

#pragma region Events

void SetReverse(bool reverse)
{
	io_expander.write(0, reverse);
	speedometer.SetReverse(reverse);
}

void UpdateSpeedometerSettings()
{
	speedometer.UpdateSettings(_settingsMenu.GetSettingValue(MaxSpeedSettingIndex), _settingsMenu.GetSettingValue(SpeedometerGranularitySettingIndex));
}

void ChangeMenu(int newMenu)
{
	_kettcarMenu.SetMenu(newMenu);
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

//void UpdateSteer()
//{
//	int currentSteer = map(analogRead(pin_steerPotiA), 0, 4095, 0, 100);
//	int diff = currentSteer - wirelessSteer;
//	
//	//lcdMenu.DisplayDebugValue(((String)diff) + "  ");
//
//	if (diff < -10)
//	{
//		io_expander.write(2, HIGH);
//		io_expander.write(3, LOW);
//	}
//	else if (diff > 10)
//	{
//		io_expander.write(2, LOW);
//		io_expander.write(3, HIGH);
//	}
//	else
//	{
//		io_expander.write(2, HIGH);
//		io_expander.write(3, HIGH);
//	}
//}

void UpdateMenuControlls()
{
	if (rotaryEncoder.getPosition() != 0)
	{
		_kettcarMenu.OnScroll(rotaryEncoder.getPosition());
		if (SerialDebugButtonPress)
		{
			Serial.print("OnEncodeScroll:");
			Serial.println(rotaryEncoder.getPosition());
		}
		rotaryEncoder.setPosition(0);
	}

	if (buttonInput == 1 && millis() - lastButtonInputTime > 200)
	{
		lastButtonInputTime = millis();
		_kettcarMenu.OnClick();
		buttonInput = 0;
		if (SerialDebugButtonPress)
			Serial.println("OnButtonPress");
	}
}

float UpdateWireless()
{
	int wirelessThrottle = 0;
	int wirelessSteer = 0;
	if (radio.available(0))
	{
		lastRecievedTime = millis();
		int values[2];
		radio.read(&values, sizeof(values));
		wirelessThrottle = values[0];
		wirelessSteer = values[1];
		_mainMenu.SetWirelessSignal(true);
		Serial.println(wirelessSteer);

		steerServo.write(map(wirelessSteer, 0, 100, 0, 180));
		wirelessThrottle = map(wirelessThrottle, 0, 100, 0, _settingsMenu.GetSettingValue(MaxRemoteThrottleSettingIndex));
	}
	else if (millis() > lastRecievedTime + wirelessTimeoutDelay) // wireless timeout 
	{
		wirelessThrottle = 0;
		_mainMenu.SetWirelessSignal(false);
	}
	return wirelessThrottle;
}

void UpdateTemperature()
{
	if(lastTemperatureUpdateMillis + tmeperaturUpdateDelay < millis())
	{
		lastTemperatureUpdateMillis = millis();

		thermometers.requestTemperatures();

		float temperatures[2];
		for (int i = 0; i < 2; i++)
		{
			temperatures[i] = thermometers.getTempCByIndex(i);
		}
	}
}

void UpdateVoltage()
{
	_lowVoltageReader.Update();
	_highVoltageReader.Update();
	_powerMenu.UpdateVoltage(_lowVoltageReader.GetVoltage(), _highVoltageReader.GetVoltage());
}

void loop(void)
{
	int executionTime = millis();
	int pedalInput = max((int)map(analogRead(pin_footPedal), _settingsMenu.GetSettingValue(PedalDeadzoneSettingIndex), 4095, 0, _settingsMenu.GetSettingValue(MaxPedalThrottleSettingIndex)), 0);
	int directThrottle = (1.0 / (1.0 + exp(-((pedalInput - 50.0) / 8.0))))*100.0;
	
	if (_mainMenu.GetWirelessEnabled() && directThrottle == 0)
	{
		currentThrottle = UpdateWireless(); // returns wireless throttle
	}
	else
	{
		currentThrottle = directThrottle;
	}
	
	UpdateMenuControlls();
	//UpdateTemperature();
	UpdateVoltage();

	int currentSpeed = currentThrottle; // TODO.... Hall Reading Magic
	
	speedometer.Update(currentSpeed);
	_mainMenu.UpdateCurrentSpeed(currentSpeed);
	
	steerServo.write(map(currentThrottle, 0, 100, 0, 180));
	
	analogWrite(pin_throttle,map(currentThrottle, 0, 100, 0, 255));

	//_mainMenu.drawDebugText(String(millis() - executionTime));
}
