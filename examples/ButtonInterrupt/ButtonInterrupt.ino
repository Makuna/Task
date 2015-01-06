// ButtonTask
// This demonstrates the use of the Sleep feature of the Task library
// It will instance two custom ButtonTasks to monitor two different pins and call back
// when they change state; with debouce and auto repeat support.
// An external interrupt is tied to the buttons to wake the Arduino up on any press
// There is two tasks implementing a LED Blink showing a "heartbeat" that can only run 
// when awake.
// There is a task that will put the Arduino to a deep sleep 15 seconds after a 
// button press.
// 
// This requires two momentary buttons attached between pins 4 & 5 and ground, 
// you can change the pins to any digital pin.
// This requires a diode per button be attached between the button pin and a single
// external interrupt pin 18, you can change this.  The cathode (black band) end
// of the diode is attached to the button pin; the anode to the interrupt pin.
//
// usefull information is sent to the serial monitor

// Pin 13 has an LED connected on most Arduino boards.
#define LedPin 13
#define AButtonPin 4
#define BButtonPin 5

// for Due, only Pin should be used
// for Leonardo, swap pin 2 & 3
//                                       // Mega only on right
#define WakeInterruptNumber  5 // 0 // 1 //  2 //  3 //  4 //  5
#define WakeInterruptPin    18 // 2 // 3 // 21 // 20 // 19 // 18

// include libraries
#include <Task.h>

// include sub files
#include "ButtonTask.h" // this implements the button task

TaskManager taskManager;

ButtonTask AButtonTask(OnAButtonChanged, AButtonPin); 
ButtonTask BButtonTask(OnBButtonChanged, BButtonPin); 
FunctionTask taskTurnLedOn(OnUpdateTaskLedOn, 400); // turn on the led in 400ms
FunctionTask taskTurnLedOff(OnUpdateTaskLedOff,600); // turn off the led in 600ms
FunctionTask taskAutoSleep(OnAutoSleep, 15000); // sleep in 15 seconds

uint8_t AButtonCount = 0;
uint8_t BButtonCount = 0;

void setup()
{
    Serial.begin(57600);
    pinMode(LedPin, OUTPUT);

    taskManager.StartTask(&AButtonTask); 
    taskManager.StartTask(&BButtonTask); 
    taskManager.StartTask(&taskTurnLedOn); // start with turning it on
    taskManager.StartTask(&taskAutoSleep);

	// setup wake up interrupt pin
    pinMode(WakeInterruptPin, INPUT_PULLUP);
    attachInterrupt(WakeInterruptNumber, NULL, FALLING);
    // for Due, just use WakeInterruptPin instead of WakeInterruptNumber, like
    // attachInterrupt(WakeInterruptPin, NULL, FALLING);

    Serial.println("Running...");
    Serial.flush();
}

void loop()
{
    taskManager.Loop();
}

void OnAButtonChanged(ButtonState state)
{
    // reset timer
    taskManager.ResetTask(&taskAutoSleep); 

    // example to show how to react on press and auto repeat only
    if (state & ButtonState_Pressed) // any state that is pressed
    {
        AButtonCount++;

        if (state == ButtonState_Pressed)
        {
            Serial.print("Pressed - ");
        }
        else
        {
            Serial.print("Repeated - ");
        }
        Serial.print("A Count = ");
        Serial.println(AButtonCount);
        Serial.flush();
    }

}

void OnBButtonChanged(ButtonState state)
{
    // reset timer
    taskManager.ResetTask(&taskAutoSleep); 

    // example to show how to react on release only
    if (state == ButtonState_Released)
    {
        BButtonCount++;

        Serial.print("B Count = ");
        Serial.println(BButtonCount);
        Serial.flush();
    }
}

void OnUpdateTaskLedOn(uint32_t deltaTimeMs)
{
    digitalWrite(LedPin, HIGH);   // turn the LED on (HIGH is the voltage level)
    taskManager.StopTask(&taskTurnLedOn); // stop trying to turn the LED On
    taskManager.StartTask(&taskTurnLedOff); // start the task to turn the LED off
}

void OnUpdateTaskLedOff(uint32_t deltaTimeMs)
{
    digitalWrite(LedPin, LOW);    // turn the LED off by making the voltage LOW
    taskManager.StopTask(&taskTurnLedOff); // stop trying to turn the LED Off
    taskManager.StartTask(&taskTurnLedOn); // start the task to turn the LED On
}

void OnAutoSleep(uint32_t deltaTimeMs)
{
    Serial.println("sleep");
    Serial.flush(); // flush is important, we are about to sleep the complete Arduino
    digitalWrite(LedPin, LOW);    // turn the LED off by making the voltage LOW

    taskManager.EnterSleep(); // this will not return until something wakes the Arduino

    digitalWrite(LedPin, HIGH);   // turn the LED on (HIGH is the voltage level)
    Serial.println("AWAKE");
    Serial.flush();
}