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

struct Message
{
    Message(uint8_t classEnum, uint8_t size) :
        Size(size),
        Class(classEnum)
    {
    };

    uint8_t Size;
    uint8_t Class;
};

class MessageTask : public Task
{
public:
    bool SendAsyncMessage(const Message& message, bool withinIsr = false);

protected:
    const uint8_t  _messageSize;

    MessageTask(uint8_t maxMessageSize, 
            uint8_t maxQueueCount = 6, 
            uint32_t loopTime = TaskTimePerMs) :
        Task(loopTime),
        _queueCount(maxQueueCount),
        _messageSize(maxMessageSize),
        _indexFront(0),
        _indexBack(0)
    {
        _pQueue = new uint8_t[_queueCount * _messageSize]();
    };

    ~MessageTask()
    {
        delete [] _pQueue;
    }

    bool PopMessage(uint8_t* pBuffer, uint8_t sizeBuffer);

private:
    const uint8_t  _queueCount;
  
    volatile uint8_t* _pQueue;
    volatile uint8_t _indexFront;
    volatile uint8_t _indexBack;
};

