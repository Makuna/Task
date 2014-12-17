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

#ifndef FUNCTIONTASK_H
#define FUNCTIONTASK_H

class FunctionTask : public Task
{
public:
  typedef void (*action)(uint32_t deltaTimeMs);
  
  FunctionTask(action function, uint32_t loopTimeMs) : 
    Task(loopTimeMs),
    callback(function)
  {};
  
private:
  action callback;
 
  virtual void OnUpdate(uint32_t deltaTimeMs)
  {
    callback(deltaTimeMs);
  }
};

#endif