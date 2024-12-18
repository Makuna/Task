/*--------------------------------------------------------------------
Task is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

Task is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

See GNU Lesser General Public License at <http://www.gnu.org/licenses/>.
--------------------------------------------------------------------*/

#pragma once

#ifndef countof
// handy macro that calculates the count of elements in an array
#define countof(a) (sizeof(a) / sizeof(a[0]))
#endif

// Arduino has no standard for attributing methods used for ISRs
// even though some platforms require it, so to simplify the problem
// for end users, this provides a standard ISR_ATTR
#if !defined(ISR_ATTR)

#if defined(ARDUINO_ARCH_ESP8266) 
#define ISR_ATTR ICACHE_RAM_ATTR
#elif defined(ARDUINO_ARCH_ESP32)
#define ISR_ATTR ICACHE_RAM_ATTR
#else
#define ISR_ATTR
#endif

#endif // !defined(ISR_ATTR)

// if you need finer timing resolution than a millisecond
// then enable the below, but this will limit the max interval 
// to just over 70 minutes
// #define TASK_MICRO_RESOLUTION

#if defined TASK_MICRO_RESOLUTION
uint32_t badTaskTimeValue(void)
    __attribute__((error("constant is too large for 'TaskTime' type: max is 4,294,967 milliseconds")));
#define TaskTimePerMs 1000
#define TaskTimeAccuracy 10
#define GetTaskTime micros
#define TaskTimeValidate(ms) ((ms > (uint32_t(-1) / (uint32_t)TaskTimePerMs)) ? badTaskTimeValue() : ms)
#define MsToTaskTime(ms) (TaskTimeValidate(ms) * (uint32_t)TaskTimePerMs)
#define UsToTaskTime(us) ((uint32_t)us)
#define TaskTimeToMs(t) (t / (uint32_t)TaskTimePerMs)
#define TaskTimeToUs(t) ((uint32_t)t)
#else
//uint32_t badTaskTimeValue(void)
//    __attribute__((error("constant is too large for 'microseconds' conversion: max is 4,294,967 milliseconds")));
#define badTaskTimeValue() 1
#define TaskTimePerMs 1
#define TaskTimeAccuracy 1
#define GetTaskTime millis
#define MsToTaskTime(ms) ((uint32_t)ms)
#define UsToTaskTime(ms) (ms / (uint32_t)1000)
#define TaskTimeToMs(t) ((uint32_t)t)
#define TaskTimeValidate(t) ((t > (uint32_t(-1) / (uint32_t)1000)) ? badTaskTimeValue() : t)
#define TaskTimeToUs(t) (TaskTimeValidate(t) * (uint32_t)1000)
#endif

enum TaskState
{
    TaskState_Stopped,
    TaskState_Running,
    TaskState_Stopping
};

class Task
{
public:
    Task(uint32_t timeInterval) :
            _remainingTime(0),
            _timeInterval(timeInterval),
            _pNext(nullptr),
            _taskState(TaskState_Stopped)
    {
    }

    void setTimeInterval(uint32_t timeInterval)
    {
        _timeInterval = timeInterval;
        _remainingTime = timeInterval;
    }

    uint32_t getTimeInterval()
    {
        return _timeInterval;
    }

    TaskState getTaskState()
    {
        return _taskState;
    }

protected:
    virtual bool OnStart() { return true; };
    virtual void OnStop() {};
    virtual void OnUpdate(uint32_t deltaTime) {};

    uint32_t _remainingTime;
    uint32_t _timeInterval;

    void Stop()
    {
        if (_taskState == TaskState_Running)
        {
            OnStop();
            _taskState = TaskState_Stopping;
        }
    }

private:
    friend class TaskManager;
    Task* _pNext; // next task in list
    TaskState _taskState;

    void Start()
    {
        _remainingTime = _timeInterval;
        _taskState = TaskState_Running;
        if (!OnStart())
        {
            _taskState = TaskState_Stopping;
        }
    }
};

#include "MessageTask.h"
#include "FunctionTask.h"
#include "TaskMacros.h"
#include "TaskManager.h"
