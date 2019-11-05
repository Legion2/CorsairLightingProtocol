
[![arduino-library-badge](https://www.ardu-badge.com/badge/Corsair%20Lighting%20Protocol.svg?)](https://www.ardu-badge.com/Corsair%20Lighting%20Protocol)
![Test Status](https://github.com/Legion2/CorsairLightingProtocol/workflows/Test/badge.svg)

**This library can be used to integrate custom/unofficial RGB strips with iCUE.**

## Features
* Add support of the Lighting Node PRO protocol to Arduino.
* Control LEDs with the Corsair Link or the iCUE software.
* Easy to use with [FastLED](http://fastled.io/).
* [Supported LED chipsets](https://github.com/FastLED/FastLED/wiki/Overview#chipsets).
* Persistent settings for use without a USB connection.
* Use multiple devices at the same time.
* Repeat or scale LED channels to arbitrary size.

![Overview](docs/Overview.png)

# Getting started

- [Requirements](#requirements)
- [Install the library](#install-the-library)
- [Create a Lighting Node PRO](#create-a-lighting-node-pro)
- [Use the library](#use-the-library)

## Requirements
The library is compatible with all boards using the MCU ATmega32U4.
This includes **Arduino Leonardo**, **SparkFun Pro Micro**, and **Arduino Micro**.
It also supports the Arduino Uno and Arduino Mega, **but** this requires the [HoodLoader2](https://github.com/NicoHood/HoodLoader2) bootloader, see [this wiki](https://github.com/Legion2/CorsairLightingProtocol/wiki/How-to-use-on-Arduino-Uno-and-Arduino-Mega) for more details.
It is **not** compatible with Arduino Nano.
In the rest of the documentation "Arduino" is used as a synonym for all supported boards regardless of the manufacturer.

As an IDE [Visual Studio](https://visualstudio.microsoft.com/downloads/) with the extension [Visual Micro](https://marketplace.visualstudio.com/items?itemName=VisualMicro.ArduinoIDEforVisualStudio) is recommended.
Visual Micro provides the required functionality for free, but sometimes ask you to buy the full version.
Using the Arduino IDE is difficult, but you can find a tutorial in the [wiki](https://github.com/Legion2/CorsairLightingProtocol/wiki/How-to-use-Arduino-IDE).

You have problems with a board not listed here, please open an [Issue](https://github.com/Legion2/CorsairLightingProtocol/issues).

## Install the library
Use the [Library-Manager](https://www.visualmicro.com/page/User-Guide.aspx?doc=Library-Manager.html#) to install this library or download a [release](https://github.com/Legion2/CorsairLightingProtocol/releases).
Additionally, the [FastLED](http://fastled.io/) library must be installed.

## Create a Lighting Node PRO
Open the example "LightingNodePRO", you can find it in the Visual Micro Explorer or the [examples directory](examples).
The library [FastLED](http://fastled.io/) is used to control the LEDs. For more information on [how to wire the leds](https://github.com/FastLED/FastLED/wiki/Wiring-leds) and [how to set up the LEDs in the code](https://github.com/FastLED/FastLED/wiki/Basic-usage#setting-up-the-leds) see the links.
After you have done the wiring or at least know which pin is the data pin, you can set this pin in the example sketch and upload it to the Arduino.
In Visual Studio use the "Release" [configuration](https://github.com/MicrosoftDocs/visualstudio-docs/blob/master/docs/debugger/how-to-set-debug-and-release-configurations.md#change-the-build-configuration).

To verify that the library works as expected, open the Windows settings -> devices -> connected devices. Somewhere in the list of devices, there should be a device called "Lighting Node PRO". (If not, please open an [Issue](https://github.com/Legion2/CorsairLightingProtocol/issues))
Open [iCUE](https://www.corsair.com/icue) there should also be the "Lighting Node PRO".

![iCUE RGB Strip example](docs/iCUE.jpg)

In iCUE open the "Lighting Setup" tab of the Lighting Node PRO(LNP) and set for both Lighting Channels the device to "RGB Light Strip" and the amount to a tenth of the LEDs you have.
iCUE groups the LEDs into groups of ten.
So if you have 20 LEDs, set the amount to 2.
Now you can create lighting effects in the "Lighting Channel #" tabs.

The example "SingleStripLightingNodePRO" only requires one LED strip connected to the Arduino.
Because the "SingleStripLightingNodePRO" concatenate the two channels, a total of 120 LEDs can be controlled on a single LED Strip.

## Use the library
If you want to use this library in your own sketch don't forget to use Visual Studio as IDE and include the [board.txt](examples/LightingNodePRO/board.txt) in the root directory of your sketch.

# Misc

- [How it works](#how-it-works)
- [Use multiple Devices](#use-multiple-devices)
- [Repeat or scale LED channel](#repeat-or-scale-led-channel)
- [Debugging](#debugging)

## How it works
This library uses the USB HID interface of the Arduino Micro or Leonardo.
In the board.txt the unique VID and PID of a "Lighting Node PRO" are defined.
After uploading a sketch with the library and these IDs, iCUE recognizes the Arduino as a Lighting Node PRO.
In iCUE you can then select the "Lighting Node PRO" and set some lighting effects.
iCUE sends these via the CorsairLightingProtocol to the Arduino.
These commands are understood by the library and converted into lighting effects on the RGB strips connected to the Arduino.

## Use multiple Devices
Each device has two unique IDs, that is, they should be unique.
You must give each device a unique ID.
There are two IDs that must be changed `SERIAL_NUMBER` and `DeviceID`.

The `SERIAL_NUMBER` can be changed in the board.txt file.
Uncomment the line with `SERIAL_NUMBER` and change some characters at the end, don't change the length and only use HEX characters(0-9 and A-F).
The `DeviceID` can be changed with the [tool](examples/DeviceIDTool/DeviceIDTool.ino)
Upload the DeviceIDTool sketch and then open the Serial monitor with baudrate 115200.
The tool displays the current DeviceID, you can type in a new DeviceID that is saved on the Arduino.
After that, you can upload another sketch.

## Repeat or scale LED channel
You can repeat or scale LED channel controlled by iCUE onto physical LED strips.
This is very useful if you have very long LED strips that are longer than 60/96 LEDs.
This is the maximum number iCUE supports.

To repeat or scale a LED channel you must apply the `CLP::repeat` or the `CLP:scale` function in the update hook of the FastLEDController.
The functions must be included from `FastLEDControllerUtils.h`.
See the [RepeatAndScale](examples/RepeatAndScale/RepeatAndScale.ino) example for the complete code.
Both functions take the FastLEDController pointer and the channel index as arguments.
Additionally, the `repeat` function takes as an argument how often the LED channel should be repeated.
For example, if you want to duplicate the channel you must pass `2` as argument.
The `scale` function takes as third argument the length of the physical LED strip to which it scales the channel using integer scaling.
For example you have 144 physical LEDs on you strip and 60 on the LED channel.
Then the third argument of the `scale` function is `144`.

For both functions it's **important**, that the CRGB arrays have at least the length of the physical LED strip.
This means if your LED channel from iCUE has 50 LEDs and you use the `repeat` function to control 100 physical LEDs you MUST declare the CRGB array at least with a length of 100.

## Debugging
For debugging don't use the integrated debugger of Visual Studio, as it will most likely break the USB communication.
Use the `DEBUG` macro and the Serial Monitor.
With the `-DDEBUG` flag you can enable debugging in the whole project.
Add this flag in the board.txt file to the `build.extra_flags`.

For advanced debugging you can use the [DebugSketch](examples/DebugSketch/DebugSketch.ino).
In the board.txt file, the default values for the debugging options can be set.
After uploading the sketch open the serial monitor with baud rate 115200 and set the line ending to "Line Feed"/"Newline" (`\n`).

# DISCLAIMERS
This is a DO IT YOURSELF project, use at your own risk!

# Credits
- [HoodLoader2](https://github.com/NicoHood/HoodLoader2)
- [Arduino HID Project](https://github.com/NicoHood/HID)
- [Protocol Information](https://github.com/audiohacked/OpenCorsairLink/issues/70)

## Related projects
- [CorsairArduinoController](https://github.com/TylerSeiford/CorsairArduinoController)
- [OpenCorsairLighting](https://github.com/McHauge/OpenCorsairLighting)
