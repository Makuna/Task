
const uint8_t MaxMessageSize = max(sizeof(HeartbeatMessage), sizeof(ButtonMessage));

// Pin 13 has an LED connected on most Arduino boards.
#define LedPin 13

class MainTask : public MessageTask
{
public:
    MainTask() :
        MessageTask(MaxMessageSize),
        AButtonCount(0),
        BButtonCount(0)
    {
    }

private:
    uint8_t AButtonCount;
    uint8_t BButtonCount;

    virtual bool OnStart()
    {
        pinMode(LedPin, OUTPUT);
        return true;
    }

    virtual void OnUpdate(uint32_t deltaTime)
    {
        uint8_t buffer[MaxMessageSize];

        if (PopMessage(buffer, MaxMessageSize))
        {
            Message* pMessage = (Message*)buffer;
            if (pMessage->Class == MessageClass_Heartbeat)
            {
                HeartbeatMessage* pHeartbeat = (HeartbeatMessage*)pMessage;
                if (pHeartbeat->State)
                {
                    digitalWrite(LedPin, HIGH);   // turn the LED on
                }
                else
                {
                    digitalWrite(LedPin, LOW);   // turn the LED off
                }
            }
            else if (pMessage->Class == MessageClass_Button)
            {
                ButtonMessage* pButton = (ButtonMessage*)pMessage;
                if (pButton->State & ButtonState_Pressed) // any state that is pressed
                {
                    switch (pButton->Id)
                    {
                    case 4:
                        AButtonCount++;
                        PrintState(pButton->State);
                        Serial.print("A ");
                        Serial.println(AButtonCount);
                        Serial.flush();
                        break;
                    case 5:
                        BButtonCount++;
                        PrintState(pButton->State);
                        Serial.print("B ");
                        Serial.println(BButtonCount);
                        Serial.flush();
                        break;
                    }
                }
            }
        }
    }

    void PrintState(ButtonState state)
    {
        if (state == ButtonState_Pressed)
        {
            Serial.print("Pressed - ");
        }
        else
        {
            Serial.print("Repeated - ");
        }
    }
};