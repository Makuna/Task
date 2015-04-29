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

#define TASK_DECLARE_BEGIN(name) \
    class name : public Task      \
    {                             \
    public:                       \
      name(uint32_t timeInterval) :                    \
        Task(timeInterval)     \
      {};                         \
    private:                      

#define TASK_DECLARE_START virtual bool OnStart()
#define TASK_DECLARE_STOP virtual void OnStop()
#define TASK_DECLARE_UPDATE virtual void OnUpdate(uint32_t deltaTime)

#define TASK_DECLARE_END  };

