/*--------------------------------------------------------------------
Task is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

Task is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with NeoPixel.  If not, see
<http://www.gnu.org/licenses/>.
--------------------------------------------------------------------*/

#ifndef TASK_H
#define TASK_H

#include <avr/sleep.h>

#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#include <pins_arduino.h>
#endif


class Task
{
public:
    Task(uint32_t loopTimeMs) :
            initialTimeMs(loopTimeMs),
            remainingTimeMs(0)
    {
    }

protected:
    virtual void OnStart() {};
    virtual void OnStop() {};
    virtual void OnUpdate(uint32_t currentTickMs) {};

    uint32_t remainingTimeMs;
    uint32_t initialTimeMs;

private:
    friend class TaskManager;
    Task* pNext; // next task in list
};

class TaskManager
{
public:
    TaskManager() :
        pFirst( NULL ),
        pLast( NULL )
    {
        lastTick = millis();
    }

    void Loop(uint8_t sleepMode = SLEEP_MODE_IDLE);
    void StartTask(Task* pTask);
    void StopTask(Task* pTask);

private:
    friend class Task;

    uint32_t lastTick;
    Task* pFirst;
    Task* pLast;
};

#endif