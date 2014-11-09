Task
====
(sorry about the formatting below, still trying to figure out how to embed code)

Arduino Nonpreemptive multitasking library

Clone this into your Arduino\Library folder.

For a good starting point at understanding the goal of this library, please read https://learn.adafruit.com/multi-tasking-the-arduino-part-1?view=all.
This library was not based on that work though, but share a common direction.

The following demonstrates a simple non-blocking timer that allows the Arduino to sleep and save power while waiting for the timer.

#include <Task.h>

TaskManager taskManager;
FunctionTask testTimer(myFunction, 1000); // giving it a callback function, myFunction

void setup()
{
  taskManager.StartTask(&testTimer);
}

void loop()
{
  taskManager.Loop();
}

void myFunction(uint32_t deltaTimeMs)
{
    // called every second
}

To make a more complex task that can do something when it starts and stops, you can use the TASK_DECLARE macros

TASK_DECLARE_BEGIN(SimpleTask) // new task will be named SimpleTask
  TASK_DECLARE_FUNCTION OnStart() // optional
  {
    Serial.println("  SimpleTask Started");
  }
  TASK_DECLARE_FUNCTION OnStop() // optional
  {
    Serial.println("  SimpleTask Stopped");
  }
  TASK_DECLARE_FUNCTION OnUpdate(uint32_t deltaTimeMs)
  {
    Serial.println("  SimpleTask!");
  }
TASK_DECLARE_END  

And if you want to implement a very complex task, you can derive from Task and provide all the C++ goodness you want to.

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

If you want to have your project sleep and use less power, you can pass in sleep modes to loop that match your needs.  The processor will be placed in that sleep mode until the text task needs to run.  NOTE:  Other things may wake up the processor which will cause loop to be run again, but it will just re-enter sleep mode as needed.

void loop()
{
  taskManager.Loop(SLEEP_MODE_PWD_DOWN);
}
