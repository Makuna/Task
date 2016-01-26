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
#include "MessageTask.h"

bool MessageTask::SendAsyncMessage(const Message& message, bool withinIsr)
{
    bool result = false;

    if (message.Size <= _messageSize)
    {
        // this maybe called from within an interrupt, so only stop them
        // when we are not as they are already stopped in an ISR
        if (!withinIsr)
        {
            noInterrupts();
        }
        uint8_t nextIndex = (_indexBack + 1) % _queueCount;
        if (nextIndex != _indexFront)
        {
            // copy message into queue
            volatile uint8_t* pQueue = _pQueue + (nextIndex * _messageSize);
            const uint8_t* pMessage = (const uint8_t*)&message;
            for (uint8_t i = 0; i < message.Size; i++)
            {
                *pQueue++ = *pMessage++;
            }
            result = true;
            _indexBack = nextIndex;
        }
        if (!withinIsr)
        {
            interrupts();
        }
    }

    return result;
}

bool MessageTask::PopMessage(uint8_t* pBuffer, uint8_t sizeBuffer)
{
    bool messagePopped = false;;

    sizeBuffer = min(sizeBuffer, _messageSize);
    noInterrupts();
    if (_indexFront != _indexBack) // messages are present
    {
        // remove one
        volatile uint8_t* pQueue = _pQueue + (_indexFront * _messageSize);
        uint8_t* pBuff = pBuffer;
        for (uint8_t i = 0; i < sizeBuffer; i++)
        {
            *pBuff++ = *pQueue++;
        }
        Message* pMessage = (Message*)pBuffer;
        // confirm the buffer was big enough for the entire message before 
        // it gets removed
        if (pMessage->Size <= sizeBuffer)
        {
            // remove it
            _indexFront = (_indexFront + 1) % _queueCount;
            messagePopped = true;
        }
    }
    interrupts();
    return messagePopped;
}