/*--------------------------------------------------------------------
Timer is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

Timer is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with NeoPixel.  If not, see
<http://www.gnu.org/licenses/>.
--------------------------------------------------------------------*/

#include "Task.h"
#include <avr/power.h>
#include <avr/wdt.h>

void _taskmanagerwakeup()
{
    // empty method for wakeup timer
}

void TaskManager::Loop(uint8_t sleepMode)
{
    uint32_t currentTick = millis();
    uint32_t deltaTimeMs = currentTick - lastTick;

    if (deltaTimeMs > 0)
    {
        uint32_t leastTimeMs = ((uint32_t)-1);

        Task* pIterate = pFirst;
        while (pIterate != NULL)
        {
            // store in case of inline removal in OnUpdate call
            Task* pNext = pIterate->pNext; 

            if (pIterate->remainingTimeMs <= deltaTimeMs)
            {
                // add the initial time so we don't loose any remainders
                pIterate->remainingTimeMs += pIterate->initialTimeMs;
                pIterate->OnUpdate(currentTick);
            }
            pIterate->remainingTimeMs -= deltaTimeMs;

            if (pIterate->remainingTimeMs < leastTimeMs)
            {
                leastTimeMs = pIterate->remainingTimeMs;
            }

            pIterate = pNext;
        }
        lastTick = currentTick;

        // check how long that took to process
        currentTick = millis();
        deltaTimeMs = currentTick - lastTick;

        if (leastTimeMs > deltaTimeMs + 1)
        {
            // Watch Dog Wake Up method, minimum time is 15ms
            if (leastTimeMs > 14)
            {
                // sleep the processor for leastTimeMs
                uint8_t sleepTime = WDTO_15MS;
                if (leastTimeMs < 30)
                {
                    sleepTime = WDTO_15MS;
                }
                else if (leastTimeMs < 60)
                {
                    sleepTime = WDTO_30MS;
                }
                else if (leastTimeMs < 120)
                {
                    sleepTime = WDTO_60MS;
                }
                else if (leastTimeMs < 250)
                {
                    sleepTime = WDTO_120MS;
                }
                else if (leastTimeMs < 500)
                {
                    sleepTime = WDTO_250MS;
                }
                else if (leastTimeMs < 1000)
                {
                    sleepTime = WDTO_500MS;
                }
                else if (leastTimeMs < 2000)
                {
                    sleepTime = WDTO_1S;
                }
                else if (leastTimeMs < 4000)
                {
                    sleepTime = WDTO_2S;
                }
                else if (leastTimeMs < 8000)
                {
                    sleepTime = WDTO_4S;
                }
                else //if (leastTimeMs > 8000)
                {
                    sleepTime = WDTO_8S;
                }

                wdt_enable(sleepTime);

                set_sleep_mode(sleepMode);
                sleep_enable();
                sleep_cpu(); // will sleep in this call
                sleep_disable(); 

                wdt_reset();
                wdt_disable();
            }
        }
    }
}

void TaskManager::StartTask(Task* pTask)
{
    if (pFirst == NULL)
    {
        pFirst = pTask;
        pLast = pTask;
    }
    else
    {
        pLast->pNext = pTask;
        pLast = pTask;
    }

    pTask->OnStart();
}

void TaskManager::StopTask(Task* pTask)
{
    if (pFirst != NULL)
    {
        bool wasRemoved = false;
        if (pFirst == pTask)
        {
            if (pLast == pTask)
            {
                // removed the only one
                pFirst = NULL;
                pLast = NULL;
            }
            else
            {
                // removed the first one
                pFirst = pFirst->pNext;
            }
            wasRemoved = true;
        }
        else
        {
            Task* pPrev = pFirst;
            Task* pIterate = pPrev->pNext;
            while (pIterate != NULL)
            {
                if (pIterate == pTask)
                {
                    // remove from chain
                    pPrev->pNext = pIterate->pNext;
                    if (pLast == pTask)
                    {
                        // removed the last one
                        pLast = pPrev;
                    }
                    wasRemoved = true;
                    pIterate = NULL;
                }
                else
                {
                    pIterate = pIterate->pNext;
                }
            }
        }

        if (wasRemoved)
        {
            pTask->OnStop();
        }
    }
}