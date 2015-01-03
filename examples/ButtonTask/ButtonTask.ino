// ButtonTask
// This demonstrates the use of the custom Task object feature of Task library
// It will instance two custom ButtonTasks to monitor two different pins and call back
// when they change state; with debouce support
// This requires two momentary buttons attached between pins 4 & 5 and ground 
// One button will turn on the on board led when it is pressed down,
// the other button turn off the on board led when it is released

// Pin 13 has an LED connected on most Arduino boards.
#define LedPin 13
#define OnButtonPin 4
#define OffButtonPin 5

// include libraries
#include <Task.h>

// include sub files
#include "ButtonTask.h"

TaskManager taskManager;

ButtonTask onButton(OnButtonChanged, OnButtonPin); 
ButtonTask offButton(OffButtonChanged, OffButtonPin); 

uint8_t onButtonCount = 0;
uint8_t offButtonCount = 0;

void setup()
{
    Serial.begin(57600);
    pinMode(LedPin, OUTPUT);
    taskManager.StartTask(&onButton); 
    taskManager.StartTask(&offButton); 

    Serial.println("Running...");
}

void loop()
{
    taskManager.Loop();
}

void OnButtonChanged(ButtonState state)
{
    // apply on press
    if (state == ButtonState_Pressed)
    {
        digitalWrite(LedPin, HIGH);   // turn the LED on (HIGH is the voltage level)

        onButtonCount++;
        Serial.print("OnCount = ");
        Serial.println(onButtonCount);
    }
}

void OffButtonChanged(ButtonState state)
{
    // apply on release
    if (state == ButtonState_Released)
    {
        digitalWrite(LedPin, LOW);   // turn the LED off (LOW is the voltage level)

        offButtonCount++;
        Serial.print("OffCount = ");
        Serial.println(offButtonCount);
    }
}
