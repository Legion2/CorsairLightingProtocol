/*
   Copyright 2019 Leon Kiefer

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

	   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
#ifndef _CLPUtils_h
#define _CLPUtils_h

#include "Arduino.h"

#define toBigEndian(a) highByte(a), lowByte(a)
uint16_t fromBigEndian(const byte& byte1, const byte& byte2);
// convert value from range 0-100 to 0-255
#define convert100To255(a) (a * 2.5546875f)
// convert value from range 0-255 to 0-100
#define convert255To100(a) (a / 2.5546875f)

#endif
