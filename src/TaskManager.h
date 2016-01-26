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

#if !defined(ESP8266)
#include <avr/sleep.h>
#include <avr/wdt.h>
#endif

class TaskManager
{
public:
    TaskManager();

    void Setup();
#if defined(ESP8266)
    // void Loop(uint16_t watchdogTimeOutMs = 500);
    void Loop();
#else
    void Loop(uint8_t watchdogTimeOutFlag = WDTO_500MS);
#endif
    void StartTask(Task* pTask);
    void StopTask(Task* pTask);
    void ResetTask(Task* pTask)
    {
        StopTask(pTask);
        StartTask(pTask);
    }
#if defined(ESP8266)
    
#else
    void EnterSleep(uint8_t sleepMode = SLEEP_MODE_PWR_DOWN);
#endif
    
    uint32_t CurrentTaskTime()
    {
        return _lastTick;
    }

private:
    friend class Task;

    uint32_t _lastTick;
    Task* _pFirstTask;
    Task* _pLastTask;

    uint32_t ProcessTasks(uint32_t deltaTime);
    void RemoveStoppedTasks();
};

