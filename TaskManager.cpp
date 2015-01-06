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

#include "Task.h"
#include <avr/power.h>

TaskManager::TaskManager() :
        _pFirstTask( NULL ),
        _pLastTask( NULL )
{
    _lastTick = millis();
}

void TaskManager::StartTask(Task* pTask)
{
    if (pTask->taskState != TaskState_Running)
    {
        // check if it has been stopped yet as it may be just stopping
        if (pTask->taskState == TaskState_Stopped)
        {
            // append to the list
            if (_pFirstTask == NULL)
            {
                _pFirstTask = pTask;
                _pLastTask = pTask;
            }
            else
            {
                _pLastTask->pNext = pTask;
                _pLastTask = pTask;
            }
        }
        pTask->Start();
    }
}

void TaskManager::StopTask(Task* pTask)
{
    pTask->Stop();
}

void TaskManager::Loop(uint8_t watchdogTimeOutFlag)
{
    uint32_t currentTick = millis();
    uint32_t deltaTimeMs = currentTick - _lastTick;

    if (deltaTimeMs > 0)
    {
        uint32_t nextWakeTimeMs = ProcessTasks(deltaTimeMs);

        RemoveStoppedTasks();
  
        // calc how long that took to process, 
        // calc a good sleep time into deltaTimeMs
        //
        _lastTick = currentTick;
        currentTick = millis();
        deltaTimeMs = currentTick - _lastTick;

        // if we have a task with less time available that what the last process pass
        // took, no need to sleep at all
        if (nextWakeTimeMs > deltaTimeMs + 1)
        {
            // for idle sleep mode:
            // due to Millis() using timer interupt at 1 ms, 
            // the cpu will be woke up by that every millisecond 

            // use watchdog timer for failsafe mode, 
            // total task update time should be less than watchdogTimeOutFlag
            wdt_reset();
            wdt_enable(watchdogTimeOutFlag);

            // just sleep
            set_sleep_mode(SLEEP_MODE_IDLE);
            sleep_enable();
            sleep_cpu(); // will sleep in this call
            sleep_disable(); 
        }
    }
}

void TaskManager::EnterSleep(uint8_t sleepMode)
{
    // disable watchdog so it doesn't wake us up
    wdt_reset();
    wdt_disable();

    // prepare sleep
    set_sleep_mode(sleepMode);
    sleep_enable();
    sleep_cpu(); // will sleep in this call
    sleep_disable();
}

uint32_t TaskManager::ProcessTasks(uint32_t deltaTimeMs)
{
    uint32_t nextWakeTimeMs = ((uint32_t)-1); // MAX_UINT32

    // Update Tasks
    //
    Task* pIterate = _pFirstTask;
    while (pIterate != NULL)
    {
        // skip any non running tasks
        if (pIterate->taskState == TaskState_Running)
        {
            if (pIterate->remainingTimeMs <= deltaTimeMs)
            {
                // calc per task delta time
                uint32_t taskDeltaTime = max(1, ((pIterate->initialTimeMs - pIterate->remainingTimeMs) + deltaTimeMs) - 1);

                pIterate->OnUpdate(taskDeltaTime);

                // add the initial time so we don't loose any remainders
                pIterate->remainingTimeMs += pIterate->initialTimeMs;

                // if we are still less than delta time, things are running slow
                // so push to the next update frame
                if (pIterate->remainingTimeMs <= deltaTimeMs)
                {
                    pIterate->remainingTimeMs = deltaTimeMs + 1;
                }
            }

            pIterate->remainingTimeMs -= deltaTimeMs;

            if (pIterate->remainingTimeMs < nextWakeTimeMs)
            {
                nextWakeTimeMs = pIterate->remainingTimeMs;
            }
        }

        pIterate = pIterate->pNext;
    }
    return nextWakeTimeMs;
}

void TaskManager::RemoveStoppedTasks()
{
    // walk task list and remove stopped tasks
    //
    Task* pIterate = _pFirstTask;
    Task* pIteratePrev = NULL;
    while (pIterate != NULL)
    {
        Task* pNext = pIterate->pNext;
        if (pIterate->taskState == TaskState_Stopping)
        {
            // Remove it
            pIterate->taskState = TaskState_Stopped;
            pIterate->pNext = NULL; 

            if (pIterate == _pFirstTask)
            {
                // first one, correct our first pointer
                _pFirstTask = pNext;
                if (pIterate == _pLastTask)
                {
                    // last one, correct our last pointer
                    _pLastTask = _pFirstTask;
                }
            }
            else
            {
                // all others correct the previous to remove it
                pIteratePrev->pNext = pNext;
                if (pIterate == _pLastTask)
                {
                    // last one, correct our last pointer
                    _pLastTask = pIteratePrev;
                }
            }
        }
        else
        {
            // didn't remove, advance the previous pointer
            pIteratePrev = pIterate;
        }
        pIterate = pNext; // iterate to the next
    }
}
