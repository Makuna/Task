# Task

Arduino Nonpreemptive multitasking library

Clone this into your Arduino\Library folder.

For a good starting point at understanding the direction of this library, please read https://learn.adafruit.com/multi-tasking-the-arduino-part-1?view=all.
This library was not based on that work though, but they share a common goals.

NOTE: Avoid the use of Delay in all high level code as the tasks timing should be used to replace it.

## Installing This Library
Create a directory in your Arduino\Library folder named "Task"
Clone (Git) this project into that folder.  
It should now show up in the import list.

## Samples
### BlinkUsingTaskFunctions
This demonstrates the use of the FunctionTask feature of Task library. It will use two FunctionTasks to to blink a LED repeatedly, by alternating which task is active and flpping the state of the LED pin.
In this example, tasks are declared at the top and are associated with a function that will be run when the task gets called to update.

```
FunctionTask taskTurnLedOn(OnUpdateTaskLedOn, 400); // turn on the led in 400ms
FunctionTask taskTurnLedOff(OnUpdateTaskLedOff,600); // turn off the led in 600ms

...

void OnUpdateTaskLedOn(uint32_t deltaTimeMs)
{
  digitalWrite(ledPin, HIGH);   // turn the LED on (HIGH is the voltage level)
  taskManager.StopTask(&taskTurnLedOn); // stop trying to turn the LED On
  taskManager.StartTask(&taskTurnLedOff); // start the task to turn the LED off
}


void OnUpdateTaskLedOff(uint32_t deltaTimeMs)
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
This is intended for intermediate level coders where more control is needed.  This exposes to the developer the fuctions that are called when the task is started and stopped along with updated.
The custom task is defined in the taskBlinkLed.h tab.

### BlinkUsingCustomTask
This demonstrates the use of the custom Task object feature of Task library. It will use a custom task to blink a LED repeatedly.
This is inteded for advanced level coders where the most control is required and knowledge of C++ is understood.

## Sleep Modes
If you want to have your project to deep sleep and use less power, you can pass in sleep modes as defined in Arduino header sleep.h to loop() that match your needs. The Idle sleep mode is used by default, which will wake up every millisecond to see if anything needs to be run.  
The processor will be placed into the given sleep mode until the next task needs to run.  

NOTE:  Other things may wake up the processor which will cause loop to be run again, but it will just re-enter sleep mode as needed.

```
void loop()  
{  
  taskManager.Loop(SLEEP_MODE_PWD_DOWN);  
}  
```
NOTE:  For more advanced scenarios, you can pass in a different sleep mode to the Loop() everytime you call it.  

## WatchDog Timer is enabled
This library will turn on the watchdog timer and by default set it too 500ms.  If any single function takes longer than 500ms, the Arduino will reset and start over.
This is usefull where a bug in the code may cause intermittant hang.  But it requires that the developer make sure to use the Tasks to keep a single function to small atomic piece of work.
The length of the WatchDog Timer can be changed by passing in one of the flags defined in wdt.h for the supported timer lengths to the loop method.

```
void loop()  
{  
  taskManager.Loop(SLEEP_MODE_IDLE, WDTO_1S); // use one second watch dog timer  
}  
```

## Multiple Tasks are no problem
While the samples show simple examples, the limit to the number of tasks is based on memory and the time spent in the active update calls.
You can have one task blinking an LED, while another task is samples a analog in and sets PWM output.  Just keep the work inside each update call to the minimum needed for that time period and set the time cycle on the task to an appropriet value.
