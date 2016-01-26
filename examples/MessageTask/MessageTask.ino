// MessageTask
// This demonstrates the use of the message passing feature of Task library
//
// It will instance two custom ButtonTasks to monitor two different pins and 
// they will send messages when they change state
// It will also have tasks that provide a heartbeat message
//
// This requires two momentary buttons attached between pins 4 & 5 and ground, you
// can change the pins to any digital pin 
//
// Usefull information will be sent to the serial monitor

#include <Arduino.h>

#define AButtonPin 4
#define BButtonPin 5

// include libraries
#include <Task.h>

// include sub files
#include "Messages.h" // this is where messages are defined
#include "ButtonTask.h" // this implements the button task
#include "MainTask.h" // this implements the message target core work at handling messages

TaskManager taskManager;

// foreward delcare functions passed to task constructors now required
void OnHeartBeatOn(uint32_t deltaTime);
void OnHeartBeatOff(uint32_t deltaTime);

MainTask mainTask;
ButtonTask buttonATask(&mainTask, AButtonPin); 
ButtonTask buttonBTask(&mainTask, BButtonPin); 
FunctionTask heartBeatOnTask(OnHeartBeatOn, MsToTaskTime(400)); 
FunctionTask heartBeatOffTask(OnHeartBeatOff, MsToTaskTime(600)); 

void setup()
{
    Serial.begin(57600);

    taskManager.StartTask(&mainTask);
    taskManager.StartTask(&buttonATask); 
    taskManager.StartTask(&buttonBTask); 
    taskManager.StartTask(&heartBeatOnTask); 

    Serial.println("Running...");
}

void loop()
{
    taskManager.Loop();
}

void OnHeartBeatOn(uint32_t deltaTime)
{
    HeartbeatMessage message(true);
    if (!mainTask.SendAsyncMessage(message))
    {
        Serial.println(">> message buffer overflow <<");
    }
    
    // toggle tasks
    taskManager.StopTask(&heartBeatOnTask); 
    taskManager.StartTask(&heartBeatOffTask); 
}

void OnHeartBeatOff(uint32_t deltaTime)
{
    HeartbeatMessage message(false);
    if (!mainTask.SendAsyncMessage(message))
    {
        Serial.println(">> message buffer overflow <<");
    }
    
    // toggle tasks
    taskManager.StopTask(&heartBeatOffTask); 
    taskManager.StartTask(&heartBeatOnTask); 
}
