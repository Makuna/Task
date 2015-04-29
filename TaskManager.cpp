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

#include <Arduino.h>
#include "Task.h"
#include "TaskManager.h"

#if defined(ESP8266)
#else
#include <avr/power.h>
#endif

#define USE_WDT 

TaskManager::TaskManager() :
        _pFirstTask( NULL ),
        _pLastTask( NULL )
{
#if defined(ESP8266)
    //Esp.wdtFeed();
    //Esp.wdtDisable();
#else
    wdt_reset();
    wdt_disable();
#endif

    _lastTick = GetTaskTime();
}

void TaskManager::Setup()
{
    _lastTick = GetTaskTime();
}

void TaskManager::StartTask(Task* pTask)
{
    if (pTask->_taskState != TaskState_Running)
    {
        // check if it has been stopped yet as it may be just stopping
        if (pTask->_taskState == TaskState_Stopped)
        {
            // append to the list
            if (_pFirstTask == NULL)
            {
                _pFirstTask = pTask;
                _pLastTask = pTask;
            }
            else
            {
                _pLastTask->_pNext = pTask;
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

#if defined(ESP8266)
//void TaskManager::Loop(uint16_t watchdogTimeOutMs)
void TaskManager::Loop()
#else
void TaskManager::Loop(uint8_t watchdogTimeOutFlag)
#endif
{
    uint32_t currentTick = GetTaskTime();
    uint32_t deltaTime = currentTick - _lastTick;

    if (deltaTime >= TaskTimeAccuracy)
    {
        _lastTick = currentTick; // update before calling process
        uint32_t nextWakeTime = ProcessTasks(deltaTime);

        RemoveStoppedTasks();
  
#if defined(ESP8266)
// Esp8266

#if defined(USE_WDT)
        //Esp.wdtFeed();
        //Esp.wdtEnable(watchdogTimeOutMs);
#endif

#else
// Arduino Normal
        // if the next task has more time available than the next
        // millisecond interupt, then sleep
        if (nextWakeTime > TaskTimePerMs)
        {
            // for idle sleep mode:
            // due to Millis() using timer interupt at 1 ms, 
            // the cpu will be woke up by that every millisecond 

#if defined(USE_WDT)
            // use watchdog timer for failsafe mode, 
            // total task update time should be less than watchdogTimeOutFlag
            wdt_reset();
            wdt_enable(watchdogTimeOutFlag);
#endif

            // just sleep
            set_sleep_mode(SLEEP_MODE_IDLE);
            cli();
            sleep_enable();
#if defined(BODSE)
            // lower power trick 
            // sleep_bod_disable() - i have seen this method called, but can't find it
            MCUCR |= _BV(BODS) | _BV(BODSE);  // turn on brown-out enable select
            MCUCR &= ~_BV(BODSE);        // this must be done within 4 clock cycles of above
#endif
            sei();
            sleep_cpu(); // will sleep in this call
            sleep_disable(); 
        }
#if defined(USE_WDT)
        else
        {
            wdt_reset(); // keep the dog happy
        }
#endif
#endif // Esp
    }
}

#if defined(ESP8266)
#else

void TaskManager::EnterSleep(uint8_t sleepMode)
{


#if defined(USE_WDT)
    // disable watchdog so it doesn't wake us up
    wdt_reset();
    wdt_disable();
#endif
    // prepare sleep
    set_sleep_mode(sleepMode);
    cli();
    sleep_enable();

#if defined(BODSE)
    // lower power trick 
    // sleep_bod_disable() - i have seen this method called, but can't find it
    MCUCR |= _BV(BODS) | _BV(BODSE);  // turn on brown-out enable select
    MCUCR &= ~_BV(BODSE);        // this must be done within 4 clock cycles of above
#endif

    sei();
    sleep_cpu(); // will sleep in this call
    sleep_disable();

#if defined(USE_WDT)
    // enable watch dog after wake up
    wdt_reset();
    wdt_enable(WDTO_500MS);
#endif

}
#endif // Esp

uint32_t TaskManager::ProcessTasks(uint32_t deltaTime)
{
    uint32_t nextWakeTime = ((uint32_t)-1); // MAX_UINT32

    // Update Tasks
    //
    Task* pIterate = _pFirstTask;
    while (pIterate != NULL)
    {
        // skip any non running tasks
        if (pIterate->_taskState == TaskState_Running)
        {
            if (pIterate->_remainingTime <= deltaTime)
            {
                // calc per task delta time
                uint32_t taskDeltaTime = pIterate->_timeInterval - pIterate->_remainingTime;
                taskDeltaTime += deltaTime;

                pIterate->OnUpdate(taskDeltaTime);

                // add the initial time so we don't loose any remainders
                pIterate->_remainingTime += pIterate->_timeInterval;

                // if we are still less than delta time, things are running slow
                // so push to the next update frame
                if (pIterate->_remainingTime <= deltaTime)
                {
                    pIterate->_remainingTime = deltaTime + TaskTimeAccuracy;
                }
            }

            uint32_t newRemainingTime = pIterate->_remainingTime - deltaTime;
            pIterate->_remainingTime = newRemainingTime;

            if (newRemainingTime < nextWakeTime)
            {
                nextWakeTime = newRemainingTime;
            }
        }

        pIterate = pIterate->_pNext;
    }
    return nextWakeTime;
}

void TaskManager::RemoveStoppedTasks()
{
    // walk task list and remove stopped tasks
    //
    Task* pIterate = _pFirstTask;
    Task* pIteratePrev = NULL;
    while (pIterate != NULL)
    {
        Task* pNext = pIterate->_pNext;
        if (pIterate->_taskState == TaskState_Stopping)
        {
            // Remove it
            pIterate->_taskState = TaskState_Stopped;
            pIterate->_pNext = NULL; 

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
                pIteratePrev->_pNext = pNext;
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
