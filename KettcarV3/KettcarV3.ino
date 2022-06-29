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
KettcarMenu _kettcarMenu(&_lcd, &io_expander);
MainMenu _mainMenu(&_lcd, &io_expander, ChangeMenu);
SettingsMenu _settingsMenu(&_lcd, ChangeMenu,UpdateSpeedometerSettings);

// -- Rotary Encoder --
RotaryEncoder rotaryEncoder(pin_rotaryA, pin_rotaryB);

// -- RF24 - Radio --
RF24 radio(0, 17); // CE, CSN Radio
const byte address[6] = "RMCMD";

OneWire oneWire(pin_thermometer);
DallasTemperature thermometers(&oneWire);

Servo steerServo;

// -- Settings --
const int wirelessTimeoutDelay = 400; // Milliseconds
const bool SerialDebugButtonPress = false;
const bool SerialDebugInit = false;
const int tmeperaturUpdateDelay = 2000;

const int vd_R1 = 1000000;  // Vin -> R1 -> ESP32 -> R2 -> GND
const int vd_R2 = 46800;

// -- Changing --
long lastRecievedTime;
int buttonInput = 0; // 0 = no input, 1 = pressed, 2 = released
long lastButtonInputTime = 0;
int hallCount = 0;
int currentThrottle = 0;
int wirelessSteer;
float lastTemperatureUpdateMillis;
float currentBatteryVoltage;
float voltageBuffer[voltageBufferLength];
int voltageBufferIndex;

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

	digitalWrite(pin_BatteryA, LOW);
	digitalWrite(pin_BatteryB, LOW);
	digitalWrite(pin_BatteryC, LOW);

	steerServo.attach(2);

	//pinMode(pin_throttle,OUTPUT); // Throttle
	//digitalWrite(pin_throttle, LOW);

	pinMode(pin_voltMessure, INPUT); // Voltage Sense
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

	_mainMenu.Init();
	delay(200);
	_kettcarMenu.Init(&_mainMenu, &_settingsMenu);
	delay(200);
	_settingsMenu.Init();
	delay(200);
	speedometer.init();

	InitRadio();

	_settingsMenu.SetSettingValue(PedalDeadzoneSettingIndex, analogRead(pin_footPedal) + 50);

	Wire.setClock(400000);

	_kettcarMenu.Draw();

	//InitWIFI();

	steerServo.write(90);

	if (SerialDebugInit)
	{
		InitDebugPrint();
	}
}
#pragma endregion

#pragma region Events

void OTAUpdateStart() {

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
		speedometer.SetTemperatures(temperatures);
	}
}

void UpdateVoltage()
{
	// update Buffer Index
	if (voltageBufferIndex < voltageBufferLength)
		voltageBufferIndex++;
	else
		voltageBufferIndex = 0;

	// fill Buffer at current index
	voltageBuffer[voltageBufferIndex] = analogRead(pin_voltMessure);
	
	// calculate median of last 10 analog Reads
	float bufferMedian = 0;
	for (int i = 0; i < voltageBufferLength; i++)
	{
		bufferMedian += voltageBuffer[i];
	}

	// calulate voltage at pin using bufferMedian
	float pinVoltage = (bufferMedian/ 10.0) * (3.0 / 4095.0);

	// calucate input Voltage with Voltage Devider
	currentBatteryVoltage = pinVoltage * ((vd_R1 + vd_R2) / vd_R2);
}

void loop(void)
{
	int executionTime = millis();
	int directThrottle = max((int)map(analogRead(pin_footPedal), _settingsMenu.GetSettingValue(PedalDeadzoneSettingIndex), 4095, 0, _settingsMenu.GetSettingValue(MaxPedalThrottleSettingIndex)), 0);

	if (_mainMenu.GetWirelessEnabled() && directThrottle == 0)
	{
		currentThrottle = UpdateWireless(); // returns wireless throttle
	}
	else
	{
		currentThrottle = directThrottle;
	}
	
	UpdateMenuControlls();
	UpdateTemperature();
	UpdateVoltage();

	int currentSpeed = currentThrottle; // TODO.... Hall Reading Magic
	
	speedometer.Update(currentSpeed);
	
	_mainMenu.UpdateCurrentSpeed(currentSpeed);

	digitalWrite(pin_BatteryA, currentSpeed > 30);
	digitalWrite(pin_BatteryB, currentSpeed > 60);
	digitalWrite(pin_BatteryC, currentSpeed > 90);

	//ArduinoOTA.handle();

	//_mainMenu.drawDebugText(String(millis() - executionTime));
	_mainMenu.drawDebugText(String(currentBatteryVoltage) + "V");
}
