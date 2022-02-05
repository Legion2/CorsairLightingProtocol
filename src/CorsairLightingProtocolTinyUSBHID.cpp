#include "CorsairLightingProtocolTinyUSBHID.h"

#if defined(USE_TINYUSB)

const char* corsairProducts[] = {CORSAIR_LNP_PRODUCT, CORSAIR_CP_PRODUCT,  CORSAIR_LNC_PRODUCT,
								 CORSAIR_SLC_PRODUCT, CORSAIR_SLT_PRODUCT, CORSAIR_CC_PRODUCT};
const int corsairPIDs[] = {CORSAIR_LNP_PID, CORSAIR_CP_PID,  CORSAIR_LNC_PID,
						   CORSAIR_SLC_PID, CORSAIR_SLT_PID, CORSAIR_CC_PID};

Command CorsairLightingProtocolTinyUSBHID::command;
int CorsairLightingProtocolTinyUSBHID::newData;

/* clang-format off */
uint8_t const hid_report[] = {HID_USAGE_PAGE_N (HID_USAGE_PAGE_VENDOR | 0xC0, 2),
							  HID_USAGE_N      (0x0C00,                       2),
							  HID_COLLECTION   (HID_COLLECTION_APPLICATION     ),
							  HID_REPORT_SIZE  (8                              ),
							  HID_LOGICAL_MIN  (0x00                           ),
							  HID_LOGICAL_MAX_N(0xff,                         2),
							  HID_REPORT_COUNT (RESPONSE_SIZE                  ),
							  HID_USAGE        (0x01                           ),
							  HID_INPUT        (HID_DATA | HID_VARIABLE | HID_ABSOLUTE),
							  HID_REPORT_COUNT (COMMAND_SIZE                   ),
							  HID_USAGE        (0x02                           ),
							  HID_OUTPUT       (HID_DATA | HID_VARIABLE | HID_ABSOLUTE),
							  HID_COLLECTION_END};
/* clang-format on */

Adafruit_USBD_HID TinyUSBDevice_HID(hid_report, sizeof(hid_report), HID_ITF_PROTOCOL_NONE, 2, true);

uint16_t get_report_callback(uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen) {
	(void)report_id;
	(void)report_type;
	(void)buffer;
	(void)reqlen;

	CLP_LOG(2, F("Get report callback!\r\n"));

	return 0;
}

void set_report_callback(uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize) {
	(void)report_id;
	(void)report_type;

	if (bufsize <= sizeof(Command)) {
		memcpy(&CorsairLightingProtocolTinyUSBHID::command.raw, buffer, bufsize);
		CorsairLightingProtocolTinyUSBHID::newData = 1;

		CLP_LOG(3, F("Received command: %02X\r\n"), CorsairLightingProtocolTinyUSBHID::command.command);
		CLP_LOG_DAT(4, &CorsairLightingProtocolTinyUSBHID::command.raw,
					sizeof(CorsairLightingProtocolTinyUSBHID::command), true);
	} else {
		CLP_LOG(2, F("Command too large\r\n"));
	}
}

CorsairLightingProtocolTinyUSBHID::CorsairLightingProtocolTinyUSBHID(CorsairLightingProtocolController* controller)
	: controller(controller), serialNumber(SERIAL_NUMBER) {}

CorsairLightingProtocolTinyUSBHID::CorsairLightingProtocolTinyUSBHID(CorsairLightingProtocolController* controller,
																	 const char* serialNumber)
	: controller(controller), serialNumber(serialNumber) {}

void CorsairLightingProtocolTinyUSBHID::setup(void) {
	CLP_LOG_FUNC(CLP_DEBUG_PORT.begin(CLP_DEBUG_BAUD));

	TinyUSBDevice.setManufacturerDescriptor(CORSAIR_MANUFACTURER);
	TinyUSBDevice.setProductDescriptor(corsairProducts[controller->getFirmware()->getProduct()]);
	TinyUSBDevice.setID(CORSAIR_VID, corsairPIDs[controller->getFirmware()->getProduct()]);
	TinyUSBDevice.setSerialDescriptor(serialNumber);

	TinyUSBDevice_HID.setReportCallback(get_report_callback, set_report_callback);
	TinyUSBDevice_HID.begin();

	while (!TinyUSBDevice.mounted()) delay(1);
}

void CorsairLightingProtocolTinyUSBHID::update(void) {
	if (newData) {
		controller->handleCommand(command, this);
		newData = 0;
	}
}

void CorsairLightingProtocolTinyUSBHID::sendX(const uint8_t* data, const size_t x) const {
	TinyUSBDevice_HID.sendReport(0, data, x);

	CLP_LOG(3, F("Sent response: %02X\r\n"), data[0]);
	CLP_LOG_DAT(4, data, x, true);
}

#endif