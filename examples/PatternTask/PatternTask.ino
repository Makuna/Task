// PatternTask
// This demonstrates the use of the custom Task object feature of Task library
// It will use a custom task to provide a "pattern" technique to blink a LED, 
// The blink pattern is three consective blinks that are shorter than the previous blink but
// still spaced out between them by the same amount of time

#include <Arduino.h>

// Pin 13 has an LED connected on most Arduino boards.
#define LedPin LED_BUILTIN
// some boards invert the LED, so you may need to swap the HIGH/LOW
#define LedOn HIGH 
#define LedOff LOW

// include libraries
#include <Task.h>

// include sub files
#include "taskPattern.h"

TaskManager taskManager;

class TriBlink
{
public:
    static void OnAction0()
    {
        digitalWrite(LedPin, LedOff);   // turn the LED off (LOW is the voltage level)
    }

    static void OnAction1()
    {
        digitalWrite(LedPin, LedOn);   // turn the LED on (HIGH is the voltage level)
    }

    const static PatternElement  Pattern[]; // C11 doesn't allow Initialization in the class (see below)
    const static bool Repeat = false;
};

// Initialize the pattern, a cascading on time from long to short over three pulses, with the same off time
const PatternElement TriBlink::Pattern[] = {{600, 1}, {100, 0}, {300,1}, {100,0}, {150,1}, {100,0}};

TaskPattern<TriBlink> triBlinkTask; 

void OnTriggerPattern(uint32_t deltaTime)
{
    // start the blink task, 
    // it auto stops when done with its pattern due to not being a repeated task
    taskManager.StartTask(&triBlinkTask); 
}

// for demonstration, we use a task to simulate the action that would trigger the pattern
// we do this every 10 seconds
FunctionTask taskTriggerPattern(OnTriggerPattern, MsToTaskTime(10000)); 

void setup()
{
    pinMode(LedPin, OUTPUT);
    digitalWrite(LedPin, LedOff);

    taskManager.StartTask(&taskTriggerPattern); 
}

void loop()
{
    taskManager.Loop();
}

