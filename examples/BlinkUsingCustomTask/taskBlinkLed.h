

class TaskBlinkLed : public Task
{
public:
  TaskBlinkLed(uint8_t pin, uint32_t timerTimeMs) : // pass any custom arguments you need
     Task(timerTimeMs),
     ledPin(pin),
     ledOn(false)  // initialize members here
  { };
  
private:  
  // put member variables here that are scoped to this object
  bool ledOn;
  const uint8_t ledPin; // const means can't change other than in constructor
  
  virtual void OnStart() // optional
  {
    // put code here that will be run when the task starts
    ledOn = false;
    pinMode(ledPin, OUTPUT);
  }
  
  virtual void OnStop() // optional
  {
    // put code here that will be run when the task stops
    ledOn = false;
    digitalWrite(ledPin, LOW);    // turn the LED off by making the voltage LOW
  }
  
  virtual void OnUpdate(uint32_t deltaTimeMs)
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
}; 
