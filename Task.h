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

#ifndef TASK_H
#define TASK_H

#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#include <pins_arduino.h>
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
    Task(uint32_t timeIntervalMs) :
            initialTimeMs(timeIntervalMs),
            remainingTimeMs(0),
            taskState(TaskState_Stopped)
    {
    }


protected:
    virtual void OnStart() {};
    virtual void OnStop() {};
    virtual void OnUpdate(uint32_t deltaTimeMs) = 0;

    uint32_t remainingTimeMs;
    uint32_t initialTimeMs;

private:
    friend class TaskManager;
    Task* pNext; // next task in list
    TaskState taskState;

    void Start()
    {
        remainingTimeMs = initialTimeMs;
        OnStart();
        taskState = TaskState_Running;
    }
    void Stop()
    {
        OnStop();
        taskState = TaskState_Stopping;
    }
};

#include "MessageTask.h"
#include "FunctionTask.h"
#include "TaskMacros.h"
#include "TaskManager.h"

#endif