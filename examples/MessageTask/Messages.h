
enum MessageClass
{
    MessageClass_Heartbeat,
    MessageClass_Button,
};

struct HeartbeatMessage : Message
{
    HeartbeatMessage(bool state) :
        Message(MessageClass_Heartbeat, sizeof(HeartbeatMessage)),
        State(state)
    {
    };

    bool State;
};
