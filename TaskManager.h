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

#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <avr/sleep.h>
#include <avr/wdt.h>

class TaskManager
{
public:
    TaskManager();

    bool Loop(uint8_t sleepMode = SLEEP_MODE_IDLE, uint8_t watchdogTimeOutFlag = WDTO_500MS, float watchdogTimeRatio = 1.084);
    void StartTask(Task* pTask);
    void StopTask(Task* pTask);

private:
    friend class Task;

    uint32_t _lastTick;
    Task* _pFirstTask;
    Task* _pLastTask;

    uint32_t ProcessTasks(uint32_t deltaTimeMs);
    void RemoveStoppedTasks();
    void WatchDogWakeupSleep(uint8_t sleepMode, 
            uint32_t nextWakeTimeMs, 
            uint8_t watchdogTimeOutFlag,
            uint32_t deltaTimeMs,
            float watchdogTimeRatio);
};

#endif