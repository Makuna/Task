// PatternTask
// This demonstrates the use of the custom Task object feature of Task library
// It will use a custom task to provide a "pattern" technique to blink a LED, 

#include <Arduino.h>

// Pin 13 has an LED connected on most Arduino boards.
#define LedPin 13 // BUILTIN_LED

// include libraries
#include <Task.h>

// include sub files
#include "taskPattern.h"

TaskManager taskManager;

void OnLedOn()
{
	digitalWrite(LedPin, HIGH);   // turn the LED on (HIGH is the voltage level)
}

void OnLedOff()
{
	digitalWrite(LedPin, LOW);   // turn the LED off (LOW is the voltage level)
}

PatternElement  triBlinkPattern[] = {{200, 1}, {100, 0}, {200,1}, {100,0}, {200,1}, {100,0}};

TaskPattern triBlinkTask( triBlinkPattern, countof(triBlinkPattern), OnLedOff, OnLedOn, false); 

void OnMakePattern(uint32_t deltaTime)
{
	// start the blink task, 
	// it auto stops when done with its pattern due to not being a repeated task
	taskManager.StartTask(&triBlinkTask); 
}

// for demonstration, we use a task to simulate the action that plays the pattern
// trigger pattern every 10 seconds
FunctionTask taskMakePattern(OnMakePattern, MsToTaskTime(10000)); 

void setup()
{
	pinMode(LedPin, OUTPUT);
    taskManager.StartTask(&taskMakePattern); 
}

void loop()
{
    taskManager.Loop();
}


