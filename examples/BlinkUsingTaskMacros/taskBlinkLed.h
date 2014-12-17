
// Pin 13 has an LED connected on most Arduino boards.
#define ledPin 13

TASK_DECLARE_BEGIN(TaskBlinkLed)
  // put member variables here that are scoped to this object
  bool ledOn;
  
  TASK_DECLARE_FUNCTION OnStart() // optional
  {
    // put code here that will be run when the task starts
    ledOn = false;
    pinMode(ledPin, OUTPUT);
  }
  
  TASK_DECLARE_FUNCTION OnStop() // optional
  {
    // put code here that will be run when the task stops
    ledOn = false;
    digitalWrite(ledPin, LOW);    // turn the LED off by making the voltage LOW
  }
  
  TASK_DECLARE_FUNCTION OnUpdate(uint32_t deltaTimeMs)
  {
    if (ledOn)
    {
      digitalWrite(ledPin, LOW);    // turn the LED off by making the voltage LOW
    }
    else
    {
      digitalWrite(ledPin, HIGH);   // turn the LED on (HIGH is the voltage level)
    }
    
    ledOn = !ledOn; // toggle led state
  }
TASK_DECLARE_END  
