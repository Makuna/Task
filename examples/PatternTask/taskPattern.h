//
// a PatternElement defines the milliseconds before progressing to the next 
// action and the action to apply at the start.
// currently it will use 15 bits for the time and 1 bit for the action
// it could be easily extended to support more actions by changing bit counts...
// 14 bits for time and 2 bits for action = 16.384 seconds and 4 actions 
// 13 bits for time and 3 bits for action = 8.192 seconds and 8 actions
//
struct PatternElement
{
    uint16_t interval : 15; // milliseconds
    uint16_t action : 1;  
};

static_assert(sizeof(PatternElement) == 2, "PatternElement expected to be 16 bits in size, please check your definition");

template<class T_FEATURE> class TaskPattern : public Task
{
public:
    TaskPattern() :
        Task(2),
        _activeElement(0)
    { };

private:
    size_t _activeElement;

    virtual bool OnStart() // optional
    {
        // when started, always begin at the first action
        // you may not always want this, you may want to start and stop and 
        // just resume the current action, if so, just comment out this next line
        _activeElement = 0; 

        ApplyActivePatternElement(0);
        return true;
    }

    virtual void OnStop() // optional
    {
    }

    virtual void OnUpdate(uint32_t deltaTime)
    {
        // capture the difference in time passed versus expected
        int32_t delta = TaskTimeToMs(deltaTime) - T_FEATURE::Pattern[_activeElement].interval;

        // move to next element
        _activeElement++;
        if (T_FEATURE::Repeat)
        {
            // wrap active pattern element if needed
            _activeElement %= countof(T_FEATURE::Pattern); 
        }
        else if (_activeElement >= countof(T_FEATURE::Pattern))
        {
            // we reached the last one, just stop
            this->Stop();
            return;
        }
        ApplyActivePatternElement(delta);
    }

    void ApplyActivePatternElement(int32_t delta)
    {
        switch (T_FEATURE::Pattern[_activeElement].action)
        {
        case 0:
            T_FEATURE::OnAction0();
            break;

        case 1:
            T_FEATURE::OnAction1();
            break;
        // if you need more actions, just add them here
        }

        this->setTimeInterval(MsToTaskTime(T_FEATURE::Pattern[_activeElement].interval - delta));
    }
};