Task
====

Arduino Nonpreemptive multitasking library

Clone this into your Arduino\Library folder.

For a good starting point at understanding the goal of this library, please read https://learn.adafruit.com/multi-tasking-the-arduino-part-1?view=all.
This library was not based on that work.

The following demonstrates a simple non-blocking timer that allows the Arduino to sleep and save power while waiting for the timer.

#include <Task.h

TaskManager taskManager;
Task testTimer(1000);

void setup()
{
  taskManager.StartTask(&testTimer);
}

void loop()
{
  taskManager.Loop(SLEEP_MODE_STANDBY);
}

To make more complex tasks that actually do something, derive your own task from Task, and override the virtual method OnUpdate() and provide code that does something.

class MyTask : public Task
{
public:
  MyTask() : 
    Task(2000), // every two seconds
    count(0)
  {};
  
private:
  int count;
  
  // called when the task is started
  virtual void OnStart()
  {
    Serial.println("MyTask Started");
  }
  
  // called when the task is stopped
  virtual void OnStop()
  {
    Serial.println("MyTask Stopped");
  }
  
  // called at the interveral defined in the contructor, every 2000 milliseconds or two seconds
  virtual void OnUpdate(uint32_t currentTickMs)
  {
    Serial.print("MyTask Triggered ");
    Serial.println(count);
    count++;
  }
};
