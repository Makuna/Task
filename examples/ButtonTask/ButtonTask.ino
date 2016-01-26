// ButtonTask
// This demonstrates the use of the custom Task object feature of Task library
// It will instance two custom ButtonTasks to monitor two different pins and call back
// when they change state; with debouce and auto repeat support
// This requires two momentary buttons attached between pins 4 & 5 and ground, you
// can change the pins to any digital pin (analog pins may have restrictions)
// One button will turn on the on board led when it is pressed down,
// the other button turn off the on board led when it is released
// Both will send usefull information to the serial monitor

#include <Arduino.h>

// Pin 13 has an LED connected on most Arduino boards.
#define LedPin 13 // BUILTIN_LED
#define AButtonPin 4
#define BButtonPin 5

// include libraries
#include <Task.h>

// include sub files
#include "ButtonTask.h" // this implements the button task

TaskManager taskManager;
void HandleAButtonChanged(ButtonState state);
void HandleBButtonChanged(ButtonState state);

// foreward delcare functions passed to task constructors now required
ButtonTask AButtonTask(HandleAButtonChanged, AButtonPin); 
ButtonTask BButtonTask(HandleBButtonChanged, BButtonPin); 

uint8_t AButtonCount = 0;
uint8_t BButtonCount = 0;

void setup()
{
    Serial.begin(57600);
    pinMode(LedPin, OUTPUT);

    taskManager.StartTask(&AButtonTask); 
    taskManager.StartTask(&BButtonTask); 

    Serial.println("Running...");
}

void loop()
{
    taskManager.Loop();
}

void HandleAButtonChanged(ButtonState state)
{
    // apply on press
	if (state & ButtonState_Pressed) // any state that is pressed
	{
		if (state == ButtonState_Pressed)
		{
			digitalWrite(LedPin, HIGH);   // turn the LED on (HIGH is the voltage level)
			Serial.print("Pressed - ");
		}
		else
		{
			Serial.print("Repeated - ");
		}
		AButtonCount++;
		Serial.print("A Count = ");
		Serial.println(AButtonCount);
	}
}

void HandleBButtonChanged(ButtonState state)
{
    // apply on release only
    if (state == ButtonState_Released)
    {
        digitalWrite(LedPin, LOW);   // turn the LED off (LOW is the voltage level)

        BButtonCount++;
        Serial.print("B Count = ");
        Serial.println(BButtonCount);
    }
}
