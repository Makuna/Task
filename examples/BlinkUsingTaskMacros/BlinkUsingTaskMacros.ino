// BlinkUsingTaskMacros
// This demonstrates the use of the Task macros feature of Task library
// It will use a custom tasks defined using the helper macros to to blink
// a LED repeatedly, 

#include <Arduino.h>

// include libraries
#include <Task.h>

// include sub files
#include "taskBlinkLed.h"

TaskManager taskManager;

TaskBlinkLed taskBlinkLed(MsToTaskTime(1000)); // every second it will turn on/off the LED


void setup()
{
  taskManager.StartTask(&taskBlinkLed); // start the blink task
}

void loop()
{
  taskManager.Loop();
}


