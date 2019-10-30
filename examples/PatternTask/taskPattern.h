
struct PatternElement
{
    uint16_t interval : 15; // milliseconds before progressing to next action
    uint16_t action : 1; // the action to apply at the start of this element
};

class TaskPattern : public Task
{
public:
    typedef void(*Action)();

    TaskPattern(PatternElement* pattern, size_t patternCount, Action action0, Action action1, bool repeat = false ) : 
        Task(2),
        _pattern(pattern),
        _patternCount(patternCount),
        _callback0(action0),
        _callback1(action1),
        _repeat(repeat),
        _activeElement(0)
    { };

private:
    const PatternElement* _pattern;
    const size_t _patternCount;
    const Action _callback0;
    const Action _callback1;
    const bool _repeat;
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
        int32_t delta = TaskTimeToMs(deltaTime) - _pattern[_activeElement].interval;

        // move to next element
        _activeElement++;
        if (_repeat)
        {
            _activeElement %= _patternCount; // wrap if needed
        }
        else if (_activeElement >= _patternCount)
        {
            this->Stop();
            return;
            
        }
        ApplyActivePatternElement(delta);
    }

    void ApplyActivePatternElement(int32_t delta)
    {
        if (_pattern[_activeElement].action)
        {
            _callback1();
        }
        else
        {
            _callback0();
        }
        this->setTimeInterval(MsToTaskTime(_pattern[_activeElement].interval - delta));
    }
};
