// button should be attached to any io pin, and when pressed, it should connect the pin to ground

enum ButtonState
{
    ButtonState_Released =   0b00000000,
    ButtonState_Pressed =    0b00000001,
    ButtonState_AutoRepeat = 0b00000011,
    ButtonState_Tracking =   0b10000001
};

struct ButtonMessage : Message
{
    ButtonMessage(uint8_t id, ButtonState state) :
        Message(MessageClass_Button, sizeof(ButtonMessage)),
        Id(id),
        State(state)
    {
    };

    uint8_t Id;
    ButtonState State;
};

class ButtonTask : public Task
{
public:
    ButtonTask(MessageTask* pMessageTarget, uint8_t pin) :
        Task(MsToTaskTime(3)), // check every three millisecond, 1-10 ms should be ok
        _buttonPin(pin),
        _pMessageTarget(pMessageTarget)
    { 
    };

private:
    static const uint16_t _debouceMs = 50; // (30-100) are good values
    static const uint16_t _repeatDelayMs = 600; // (400 - 1200) are reasonable values
    static const uint16_t _repeatRateMs = 50; // (40-1000) are reasonable
    const uint8_t _buttonPin;
    MessageTask* _pMessageTarget;
    uint16_t _timer;
    ButtonState _state;

    virtual bool OnStart()
    {
        pinMode(_buttonPin, INPUT_PULLUP);
        _state = ButtonState_Released;
        return true;
    }

    virtual void OnUpdate(uint32_t deltaTime)
    {
        uint16_t deltaTimeMs = TaskTimeToMs(deltaTime);
        ButtonState pinState = (digitalRead(_buttonPin) == LOW) ? ButtonState_Pressed : ButtonState_Released;

        if (pinState != (_state & ButtonState_Pressed))
        {
            if (pinState == ButtonState_Pressed)
            {
                // just read button down and start timer
                _timer = _debouceMs;
                _state = ButtonState_Tracking;
            }
            else
            {
                if ((_state & ButtonState_Tracking) == ButtonState_Pressed) // not tracking
                {
                    // triggered released
                    ButtonMessage message(_buttonPin, ButtonState_Released);
                    if (!_pMessageTarget->SendAsyncMessage(message))
                    {
                        Serial.println(">> message buffer overflow <<");
                    }
                }
                _state = ButtonState_Released;
            }
        }
        else
        {
            switch (_state)
            {
            case ButtonState_Tracking:
                if (deltaTimeMs >= _timer)
                {
                    // press debounced 
                    _state = ButtonState_Pressed;
                    _timer = _repeatDelayMs;
                    ButtonMessage message(_buttonPin, ButtonState_Pressed);
                    if (!_pMessageTarget->SendAsyncMessage(message))
                    {
                        Serial.println(">> message buffer overflow <<");
                    }
                }
                else
                {
                    _timer -= deltaTimeMs;
                }
                break;

            case ButtonState_Pressed:
                if (deltaTimeMs >= _timer)
                {
					// auto repeat started
                    _state = ButtonState_AutoRepeat;
                    _timer = _repeatRateMs;
                    ButtonMessage message(_buttonPin, ButtonState_AutoRepeat);
                    if (!_pMessageTarget->SendAsyncMessage(message))
                    {
                        Serial.println(">> message buffer overflow <<");
                    }
                }
                else
                {
                    _timer -= deltaTimeMs;
                }
                break;

            case ButtonState_AutoRepeat:
                if (deltaTimeMs >= _timer)
                {
                    // auto repeat triggered again
                    _timer += _repeatRateMs;
                    ButtonMessage message(_buttonPin, ButtonState_AutoRepeat);
                    if (!_pMessageTarget->SendAsyncMessage(message))
                    {
                        Serial.println(">> message buffer overflow <<");
                    }
                }
                else
                {
                    _timer -= deltaTimeMs;
                }
                break;
            }
        }
    }
};