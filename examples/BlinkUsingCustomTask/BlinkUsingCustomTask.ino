// BlinkUsingCustomTask
// This demonstrates the use of the custom Task object feature of Task library
// It will use a custom task to blink a LED repeatedly, 

#include <Arduino.h>

// Pin 13 has an LED connected on most Arduino boards.
#define LedPin 13 // BUILTIN_LED

// include libraries
#include <Task.h>

// include sub files
#include "taskBlinkLed.h"

TaskManager taskManager;

TaskBlinkLed taskBlinkLed(LedPin, MsToTaskTime(1000)); // every second it will turn on/off the LED


void setup()
{
    taskManager.StartTask(&taskBlinkLed); // start the blink task
}

void loop()
{
    taskManager.Loop();
}


