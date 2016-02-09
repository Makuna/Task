// RotaryEncoder
// This demonstrates the use of the custom Task object feature of Task library
// It will instance one custom RotaryEncoderTask to monitor three pins that a 
// rotary encoder is connected to and make call backs when they change state;
// the press button will have debouce and auto repeat support;
// the rotary will tell direction and keep track of "clicks" rotated
// This requires a standard rotary encoder be attached to the defined pins,
//
// Usefull information will be sent to the serial monitor 
//
// make sure that you connect your rotary encoder correctly, include gnd and Vcc (+)

#include <Arduino.h>

// nothing special about these pins, just make sure they don't collide with
// other active features
#define ClockPin 4  // labeled either as CLK or as A
#define DataPin 5   // labeled either as DT or as B
#define ButtonPin 2 // labeled as SW

// include libraries
#include <Task.h>

// include sub files
#include "RotaryEncoderTask.h" // this implements the rotary encoder task

TaskManager taskManager;

// foreward delcare functions passed to task constructors now required
void HandleButtonChanged(EncoderButtonState state);
void HandleRotationChanged(int8_t rotationDelta);


RotaryEncoderTask RotaryTask(HandleRotationChanged,
    HandleButtonChanged,
    ClockPin,
    DataPin,
    ButtonPin);

void setup()
{
    Serial.begin(57600);
    while (!Serial); // wait for serial monitor to connect


    taskManager.StartTask(&RotaryTask);

    Serial.println("Running...");
}

void loop()
{
    taskManager.Loop();
}

void HandleButtonChanged(EncoderButtonState state)
{
    if (state == EncoderButtonState_Pressed)
    {
        Serial.println("Pressed - ");
    }
    else if (state == EncoderButtonState_Released)
    {
        Serial.println("Released - ");
    } 
    else
    {
        Serial.println("Auto-repeat - ");
    }
}

void HandleRotationChanged(int8_t rotationDelta)
{
    if (rotationDelta > 0)
    {
        Serial.print("clockwise = ");
    }
    else
    {
        Serial.print("counter-clockwise = ");
    }

    Serial.println(RotaryTask.RotationValue());
}
