# Task

Arduino Nonpreemptive multitasking library

Clone this into your Arduino\Library folder.

For a good starting point at understanding the direction of this library, please read https://learn.adafruit.com/multi-tasking-the-arduino-part-1?view=all.
This library was not based on that work though, but they share common goals.

NOTE: Avoid the use of Delay in all high level code as the tasks timing should be used to replace it.

## Installing This Library
Create a directory in your Arduino\Library folder named "Task"
Clone (Git) this project into that folder.  
It should now show up in the import list.

## TaskTime
### Conversion macros
The resolution of the task interval is based on millis() or micros().  The default is millis().  To allow the code to easily switch, callers should use the macros to wrap constants.

```
uint32_t taskTime = MsToTaskTime(400); // 400 ms
uint32_t timeMilliseconds = TaskTimeToMs(taskTime);
uint32_t timeMicroseconds = TaskTimeToUs(taskTime);
taskTime = UsToTaskTime(400000) // 4000000us, 400ms also
```
This is important for passing in time interval when the task is created, and when handling the deltaTime parameter in the Update method.

```
FunctionTask taskTurnLedOn(OnUpdateTaskLedOn, MsToTaskTime(400)); 400ms

void OnUpdateTaskLedOn(uint32_t deltaTime)
{
	uint32_t deltaTimeMs = TaskTimeToMs(deltaTime);
}
```
### Getting the current Update Time
If you call millis() or micros(), you will get the time as it is right then.  If you want relative time from update to update, you can get this from the TaskManager.  This value will change only on the update interval, and should only be used within an update call.
```
	uint32_t updateTaskTime = taskManager.CurrentTaskTime();
```

### Using higher resolution time
To switch the library to use a higher resolution interval (microseconds), you need to edit the Task.h file in the library/Task folder and uncomment the following line.

```
//#define TASK_MICRO_RESOLUTION
```
If you have used the macros mentioned above, your project should compile and run, but now at a higher resoltion.
By doing this, the longest time a task interval can be is just over 70 minutes.
TaskTime values will now be microseconds, but the time interval may still be larger as some Arduinos will give values that increment by 2, 4, or even 8.

## Samples
### BlinkUsingTaskFunctions
This demonstrates the use of the FunctionTask feature of Task library. It will use two FunctionTasks to to blink a LED repeatedly, by alternating which task is active and flpping the state of the LED pin.
In this example, tasks are declared at the top and are associated with a function that will be run when the task gets called to update.

```
FunctionTask taskTurnLedOn(OnUpdateTaskLedOn, MsToTaskTime(400)); // turn on the led in 400ms
FunctionTask taskTurnLedOff(OnUpdateTaskLedOff, MsToTaskTime(600)); // turn off the led in 600ms

...

void OnUpdateTaskLedOn(uint32_t deltaTime)
{
  digitalWrite(ledPin, HIGH);   // turn the LED on (HIGH is the voltage level)
  taskManager.StopTask(&taskTurnLedOn); // stop trying to turn the LED On
  taskManager.StartTask(&taskTurnLedOff); // start the task to turn the LED off
}


void OnUpdateTaskLedOff(uint32_t deltaTime)
{
  digitalWrite(ledPin, LOW);    // turn the LED off by making the voltage LOW
  taskManager.StopTask(&taskTurnLedOff); // stop trying to turn the LED Off
  taskManager.StartTask(&taskTurnLedOn); // start the task to turn the LED On
}
```
Then one of the tasks is started in the setup function.

```
void setup()
{
  pinMode(ledPin, OUTPUT);
  
  taskManager.StartTask(&taskTurnLedOn); // start with turning it on
}
```
The tasks will continously toggle their running state handing off to the other to turn off and on the the led.

### BlinkUsingTaskMacros
This demonstrates the use of the Task macros feature of Task library. It will use a custom tasks defined using the helper macros to to blink a LED repeatedly.
This is intended for intermediate level coders where more control is needed.  This exposes to the developer the functions that are called when the task is started and stopped along with updated.
The custom task is defined in the taskBlinkLed.h tab.

### BlinkUsingCustomTask
This demonstrates the use of the custom Task object feature of Task library. It will use a custom task to blink a LED repeatedly.
This is intended for advanced level coders where the most control is required and knowledge of C++ is understood.

### ButtonTask
This demonstrates the use of the custom Task object feature of Task library.  
It will instance two custom ButtonTasks to monitor two different pins and call back when they change state; with debouce and auto repeat support.  

This requires two momentary buttons attached to any io pins and ground. One button will turn on the on board led when it is pressed down, the other button turn off the on board led when it is released. Both will send usefull information to the serial monitor.  
The custom task is defined in the ButtonTask.h tab.

### ButtonInterrupt
This demonstrates the use of the Sleep feature of the Task library.
It will instance two custom ButtonTasks to monitor two different pins and call back when they change state; with debouce and auto repeat support.
An external interrupt is tied to the buttons to wake the Arduino up on any button press.
There are two tasks implementing a LED Blink showing a "heartbeat" that can only run when awake.
There is a task that will put the Arduino into a deep sleep 15 seconds after any button press.

This requires two momentary buttons attached any io pins and ground.
This requires a diode per button be attached between the button pin and a single external interrupt pin.  The cathode (black band) end of the diode is attached to the button pin; the anode to the interrupt pin.

### MessageTask
This demonstrates the use of the message passing feature of Task library.
It will instance two custom ButtonTasks to monitor two different pins and they will send messages when they change state. It will also have tasks that provide a heartbeat message.

This requires two momentary buttons attached between pins 4 & 5 and ground, you can change the pins to any digital pin 

## Sleep Modes (advanced feature)
If you want to have your project to deep sleep and use less power, you can call the EnterSleep method and pass in a sleep modes as defined in Arduino header sleep.h. The default is SLEEP_MODE_PWR_DOWN if you dont provide one. 
The processor will be placed into the given sleep mode until it is woke up.  Different sleep modes have different actions that can cause it to wake.  But external interrupts will wake up the Arduino for all sleep modes.  

```
    Serial.println("sleep");
    Serial.flush(); // flush is important, we are about to sleep the complete Arduino

    taskManager.EnterSleep(); // this will not return until something wakes the Arduino

    Serial.println("AWAKE");
    Serial.flush(); 
```

## WatchDog Timer is enabled
This library will turn on the watchdog timer and by default set it too 500ms.  If any single function takes longer than 500ms, the Arduino will reset and start over.
This is usefull where a bug in the code may cause an intermittant hang.  But it requires that the developer make sure to use the Tasks to keep a single function to a small atomic piece of work.
The length of the WatchDog Timer can be changed by passing in one of the flags defined in wdt.h for the supported timer lengths to the loop method.

```
void loop()  
{  
  taskManager.Loop(WDTO_1S); // use one second watch dog timer  
}  
```

## Multiple Tasks are no problem
While the samples show simple examples, the limit to the number of tasks is based on memory and the time spent in the active update calls.
You can have one task blinking an LED, while another task samples a analog in and sets PWM output.  Just keep the work inside each update call to the minimum needed for that time period and set the time cycle on the task to an appropriet value.
