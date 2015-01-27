// button should be attached to any io pin, and when pressed, it should connect the pin to ground

enum ButtonState
{
    ButtonState_Released =   0b00000000,
    ButtonState_Pressed =    0b00000001,
    ButtonState_AutoRepeat = 0b00000011,
    ButtonState_Tracking =   0b10000001
};

class ButtonTask : public Task
{
public:
    typedef void(*action)(ButtonState state);

    ButtonTask(action function, uint8_t pin) :
        Task(MsToTaskTime(3)), // check every three millisecond, 1-10 ms should be ok
        _buttonPin(pin),
        _callback(function)
    { 
    };

private:
    static const uint16_t _debouceMs = 50; // (30-100) are good values
    static const uint16_t _repeatDelayMs = 600; // (400 - 1200) are reasonable values
    static const uint16_t _repeatRateMs = 50; // (40-1000) are reasonable
    const uint8_t _buttonPin;
    const action _callback;
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
                    _callback(ButtonState_Released);
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
                    _callback(ButtonState_Pressed);
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
                    _callback(ButtonState_AutoRepeat);
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
                    _callback(ButtonState_AutoRepeat);
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