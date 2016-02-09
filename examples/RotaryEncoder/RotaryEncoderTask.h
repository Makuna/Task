// This assumes the encoder is in a circuit that pulls the pins high in the normal off state
// and when then encoder is rotated the pin is pulled low
//
// you can just copy this whole file into your project and use it

enum EncoderButtonState
{
    EncoderButtonState_Released =   0b00000000,
    EncoderButtonState_Pressed =    0b00000001,
    EncoderButtonState_AutoRepeat = 0b00000011,
    EncoderButtonState_Tracking =   0b10000001
};

class RotaryEncoderTask : public Task
{
public:
    typedef void(*buttonAction)(EncoderButtonState state);
    typedef void(*rotationAction)(int8_t rotationDelta);

    RotaryEncoderTask(rotationAction rotationFunction,
            buttonAction buttonFunction,
            uint8_t pinClock, // also refered to as A
            uint8_t pinData,  // also refered to as B
            uint8_t pinButton,
            int32_t rotationValue = 0) :
        Task(MsToTaskTime(5)), // check every five millisecond, 1-10 ms should be ok
        _buttonPin(pinButton),
        _clockPin(pinClock),
        _dataPin(pinData),
        _rotationCallback(rotationFunction),
        _buttonCallback(buttonFunction),
        _buttonState(EncoderButtonState_Released),
        _clockState(HIGH),
        _rotationValue(rotationValue)
    { 
    };

    int32_t RotationValue()
    {
        return _rotationValue;
    }

private:
    static const uint16_t _debouceMs = 50; // (30-100) are good values
    static const uint16_t _repeatDelayMs = 600; // (400 - 1200) are reasonable values
    static const uint16_t _repeatRateMs = 50; // (40-1000) are reasonable

    const uint8_t _buttonPin;
    const uint8_t _clockPin;
    const uint8_t _dataPin;

    const rotationAction _rotationCallback;
    const buttonAction _buttonCallback;

    int32_t _rotationValue;
    uint16_t _buttonTimer;

    EncoderButtonState _buttonState;
    uint8_t _clockState;


    virtual bool OnStart()
    {
        pinMode(_buttonPin, INPUT_PULLUP);
        pinMode(_clockPin, INPUT_PULLUP);
        pinMode(_dataPin, INPUT_PULLUP);
        _buttonState = EncoderButtonState_Released;
        _clockState = HIGH;
        return true;
    }

    virtual void OnUpdate(uint32_t deltaTime)
    {
        uint16_t deltaTimeMs = TaskTimeToMs(deltaTime);

        CheckButton(deltaTimeMs);

        // ignore the rotary if button is pressed 
        if (_buttonState == EncoderButtonState_Released)
        {
            CheckRotation(deltaTimeMs);
        }
    }

    void CheckRotation(uint16_t deltaTimeMs)
    {
        uint8_t clockState = digitalRead(_clockPin);
        uint8_t dataState = digitalRead(_dataPin);

        // check for any change in the clock pin state
        if (clockState != _clockState)
        {
            if (clockState == LOW)
            {
                // just read clock trigger
                if (dataState == LOW)
                {
                    // clockwise
                    _rotationValue++;
                    _rotationCallback(1);
                }
                else
                {
                    // counter-clockwise
                    _rotationValue--;
                    _rotationCallback(-1);
                }
            }

            _clockState = clockState;
        }
    }

    void CheckButton(uint16_t deltaTimeMs)
    {
        EncoderButtonState pinState = (digitalRead(_buttonPin) == LOW) ? EncoderButtonState_Pressed : EncoderButtonState_Released;

        if (pinState != (_buttonState & EncoderButtonState_Pressed))
        {
            if (pinState == EncoderButtonState_Pressed)
            {
                // just read button down and start timer
                _buttonTimer = _debouceMs;
                _buttonState = EncoderButtonState_Tracking;
            }
            else
            {
                if ((_buttonState & EncoderButtonState_Tracking) == EncoderButtonState_Pressed) // not tracking
                {
                    // triggered released
                    _buttonCallback(EncoderButtonState_Released);
                }
                _buttonState = EncoderButtonState_Released;
            }
        }
        else
        {
            switch (_buttonState)
            {
            case EncoderButtonState_Tracking:
                if (deltaTimeMs >= _buttonTimer)
                {
                    // press debounced 
                    _buttonState = EncoderButtonState_Pressed;
                    _buttonTimer = _repeatDelayMs;
                    _buttonCallback(EncoderButtonState_Pressed);
                }
                else
                {
                    _buttonTimer -= deltaTimeMs;
                }
                break;

            case EncoderButtonState_Pressed:
                if (deltaTimeMs >= _buttonTimer)
                {
                    // auto repeat started
                    _buttonState = EncoderButtonState_AutoRepeat;
                    _buttonTimer = _repeatRateMs;
                    _buttonCallback(EncoderButtonState_AutoRepeat);
                }
                else
                {
                    _buttonTimer -= deltaTimeMs;
                }
                break;

            case EncoderButtonState_AutoRepeat:
                if (deltaTimeMs >= _buttonTimer)
                {
                    // auto repeat triggered again
                    _buttonTimer += _repeatRateMs;
                    _buttonCallback(EncoderButtonState_AutoRepeat);
                }
                else
                {
                    _buttonTimer -= deltaTimeMs;
                }
                break;
            }
        }
    }
};