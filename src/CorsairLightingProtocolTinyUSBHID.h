#pragma once

#include "CLPUtils.h"
#include "CorsairLightingProtocolConstants.h"
#include "CorsairLightingProtocolController.h"
#include "CorsairLightingProtocolResponse.h"

#if defined(USE_TINYUSB)

#include "Adafruit_TinyUSB.h"

class CorsairLightingProtocolTinyUSBHID : CorsairLightingProtocolResponse {
public:
	/**
	 * Create a new adapter for CorsairLightingProtocolController using the default Serial Number.
	 *
	 * @param controller the CorsairLightingProtocolController
	 */
	CorsairLightingProtocolTinyUSBHID(CorsairLightingProtocolController* controller);
	/**
	 * Create a new adapter for CorsairLightingProtocolController using the given Serial Number for the usb interface.
	 *
	 * @param controller the CorsairLightingProtocolController
	 * @param serialNumber the Serial Number used for the USB interface
	 */
	CorsairLightingProtocolTinyUSBHID(CorsairLightingProtocolController* controller, const char* serialNumber);
	/**
	 * Setup the TinyUSB HID connection.
	 * This function must be called in setup.
	 */
	void setup(void);
	/**
	 * Read commands form HID interface and pass them to the contoller. This function must be called in loop.
	 */
	void update(void);

	static Command command;

	static int newData;

protected:
	CorsairLightingProtocolController* const controller;
	const char* serialNumber;
	void sendX(const uint8_t* data, const size_t x) const override;
};

#endif