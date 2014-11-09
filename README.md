Task
====
Arduino Nonpreemptive multitasking library

Clone this into your Arduino\Library folder.

For a good starting point at understanding the goal of this library, please read https://learn.adafruit.com/multi-tasking-the-arduino-part-1?view=all.
This library was not based on that work though, but they share a common direction.

Simple Arduino Sample
---------------------
The following demonstrates a simple non-blocking timer that allows the Arduino to sleep and save power while waiting for the timer.
<pre><code>#include &lt;Task.h&gt;
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
  taskManager.StopTask(&testTimer); // only want it to happen once, so remove it
  // called once after a second  
}  
</code></pre>
Defining your own Task object
-----------------------------
To make a more complex task that can do something when it starts and stops, you can use the TASK_DECLARE macros
<pre><code>
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
</code></pre>
Defining your own C++ Task Class
--------------------------------
And if you want to implement a very complex task, you can derive from Task and provide all the C++ goodness you want to.
<pre><code>
class MyTask : public Task  
{  
public:  
  MyTask() :   
    Task(33), // 30hz (30 times a second)
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
    Serial.println(count);  
    count++;  
  }  
};  
</code></pre>
Sleep Modes
-----------
If you want to have your project deep sleep and use less power, you can pass in sleep modes as defined in Arduino header sleep.h to loop() that match your needs. The Idle sleep mode is used by default, which will wake up every millisecond to see if anything needs to be run.  
The processor will be placed into the given sleep mode until the next task needs to run.  NOTE:  Other things may wake up the processor which will cause loop to be run again, but it will just re-enter sleep mode as needed.
<pre><code>
void loop()  
{  
  taskManager.Loop(SLEEP_MODE_PWD_DOWN);  
}  
</code></pre>
NOTE:  For more advanced scenarios, you can pass in a different sleep mode to the Loop() everytime you call it.  

Multiple Tasks are no problem
---------------------------
Below is a demonstration of using all of the tasks listed above.
<pre><code>#include &lt;Task.h&gt;

TaskManager taskManager;  

FunctionTask testTimer(myFunction, 5000); // giving it a callback function, myFunction  
SimpleTask simpleTask(700);  
MyTask myTask();  

void setup()  
{  
  taskManager.StartTask(&testTimer);  
  taskManager.StartTask(&simpleTask);
  taskManager.StartTask(&myTask);
}  
  
void loop()  
{  
  taskManager.Loop();  
}  
  
void myFunction(uint32_t deltaTimeMs)  
{  
  taskManager.StopTask(&testTimer); // only want it to happen once, so remove it
  // called once after 5 seconds
}  
</code></pre>

WatchDog support built in
-------------------------
A watch dog timer is being used to help wake up the Arduino when it sleeps.  With this support, your program will get the support and there is no need for you to implement it.  If your program crashes or goes into a tight loop for longer than 500ms, the processor will be reset and your program will restart and run Setup again.  
  
Make sure that none of your tasks are using the delay() method for any length of time and that they will complete in a timely manor otherwise you will trigger this watchdog when you didn't mean to.
