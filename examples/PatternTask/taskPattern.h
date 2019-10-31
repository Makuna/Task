
struct PatternElement
{
    uint16_t interval : 15; // milliseconds before progressing to next action
    uint16_t action : 1; // the action to apply at the start of this element
};

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
            _activeElement %= countof(T_FEATURE::Pattern); // wrap if needed
        }
        else if (_activeElement >= countof(T_FEATURE::Pattern))
        {
            this->Stop();
            return;
        }
        ApplyActivePatternElement(delta);
    }

    void ApplyActivePatternElement(int32_t delta)
    {
        if (T_FEATURE::Pattern[_activeElement].action)
        {
            T_FEATURE::OnAction1();
        }
        else
        {
            T_FEATURE::OnAction0();
        }
        this->setTimeInterval(MsToTaskTime(T_FEATURE::Pattern[_activeElement].interval - delta));
    }
};