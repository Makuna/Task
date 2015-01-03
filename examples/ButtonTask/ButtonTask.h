// button should be attached to the pin, and when pressed, it should connect the pin to ground

enum ButtonState
{
    ButtonState_Pressed,
    ButtonState_Released
};

class ButtonTask : public Task
{
public:
    typedef void(*action)(ButtonState state);

    ButtonTask(action function, uint8_t pin) :
        Task(3), // check every three millisecond, 1-10 ms should be ok
        _buttonPin(pin),
        _lastValue(HIGH),
        _state(ButtonState_Released),
        _callback(function)
    { };

private:
    static const uint8_t _debouceMs = 50; // (30-100) are good values
    const uint8_t _buttonPin;
    const action _callback;
    uint8_t _lastValue;
    uint8_t _debouceTimer;
    ButtonState _state;

    virtual void OnStart()
    {
        pinMode(_buttonPin, INPUT_PULLUP);
    }

    virtual void OnUpdate(uint32_t deltaTimeMs)
    {
        uint8_t value = digitalRead(_buttonPin);

        if (value != _lastValue)
        {
            _lastValue = value;
            if (value == LOW)
            {
                // just read button down
                _debouceTimer = _debouceMs;
            }
            else if (_state == ButtonState_Pressed)
            {
                // triggered released
                _state = ButtonState_Released;
                _callback(_state);
            }
        }
        else
        {
            if (value == LOW && _state == ButtonState_Released)
            {
                // still button down
                if (deltaTimeMs >= _debouceTimer)
                {
                    // triggered press
                    _state = ButtonState_Pressed;
                    _callback(_state);
                }
                else
                {
                    _debouceTimer -= deltaTimeMs;
                }
            }
        }
    }
};