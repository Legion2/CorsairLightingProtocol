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
#ifndef _FanController_h
#define _FanController_h

#include "Arduino.h"

#include "CorsairLightingProtocol.h"
#include "IFanController.h"

#define FAN_NUM 6

#define FAN_MASK_AUTO 0x00
#define FAN_MASK_3PIN 0x01
#define FAN_MASK_4PIN 0x02
#define FAN_MASK_???? 0x03//TODO

class FanController : public IFanController {
public:
	virtual void handleFanControl(const Command & command, const CorsairLightingProtocolResponse* response) override;
protected:
};

#endif
