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
#include "CorsairLightingProtocolResponse.h"
#include "CorsairLightingProtocolConstants.h"

void CorsairLightingProtocolResponse::send(const uint8_t* data, size_t size) const {
	uint8_t response[RESPONSE_SIZE];
	memset(response, 0x00, sizeof(response));
	if (size + 1 > sizeof(response)) {
		return;
	}
	response[0] = PROTOCOL_RESPONSE_OK;
	memcpy(response + 1, data, size);
	sendX(response, sizeof(response));
}

void CorsairLightingProtocolResponse::sendError() const {
	uint8_t response[RESPONSE_SIZE];
	memset(response, 0x00, sizeof(response));
	response[0] = PROTOCOL_RESPONSE_ERROR;
	sendX(response, sizeof(response));
}

void CorsairLightingProtocolResponse::send_P(const uint8_t* data, size_t size) const {
	uint8_t response[RESPONSE_SIZE];
	memset(response, 0x00, sizeof(response));
	if (size + 1 > sizeof(response)) {
		return;
	}
	memcpy_P(response + 1, data, size);
	sendX(response, sizeof(response));
}
