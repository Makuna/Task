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

// This wrapping header is to work around an Arduino library file name
// collision with built in files that come with some Arduino board support
// collections.  
// By including this unique name of <NonPreemptiveTask.h> it will work around
// this recently introduced issue and Arduino build will then correctly find
// the task library.
//
#include "Task.h"
