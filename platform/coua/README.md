Getting Started with Contiki on the Weptech Coua
================================================

This guide aims to get you started with Contiki and the Weptech Coua development
board.

Coua features
================
The Coua features the following components: 
 * A Texas Instruments CC1310 Cortex®- M3 Microcontroller with 128KB Flash +
   20KB RAM and an integrated Sub-GHz radio core. Antennas can be connected via
   U.FL or SMA (assembly variant).
 * A FTDI FT234XD USB-to-UART converter providing a virtual COM port to the host
   system.
 * A reset button.
 * An ARM 20 pin JTAG connector.
 * Breakout headers to connect GPIO peripherals.

The CC1310 SoC at the core of the Coua offers the following key
features:

 * Deep Sleep support with RAM retention for ultra-low energy consumption.
 * Standard Cortex M3 peripherals (NVIC, SCB, SysTick)
 * Sleep Timer (underpins rtimers)
 * SysTick (underpins the platform clock and Contiki's timers infrastructure)
 * UART, I2C, SPI
 * Watchdog (in watchdog mode)
 * uDMA Controller for increased performance (RAM to/from RF)
 * Random number generator
 * Low Power Modes
 * General-Purpose Timers. NB: GPT0 is in use by the platform code, the
   remaining GPTs are available for application development.

Requirements
============
To get started with the Coua, you will need the following:

 * A toolchain to compile Contiki for the CC1310.
 * Maybe additional drivers so that your OS can communicate with your hardware.
 * Software to upload images to the CC1310.
 * Ideally, a JTAG interface that you can use to program your device. The device
   per se does support loading firmware via USB, but only if the previously
   installed firmware allows it and if you connect some additional hardware that
   can signal that you'd like the device to boot into bootloader mode, like a
   switch, button, or pin header for a jumper. There are single-purpose JTAG
   interfaces like the Segger J-Link EDU, but also development boards like the
   TI SmartRF06 that expose their JTAG interface so that you can use them to
   program the Coua.

This guide was tested on Ubuntu Linux 16.04.

Install a Toolchain
-------------------
The toolchain used to build contiki is arm-gcc, also used by other arm-based 
Contiki ports. If you are using Instant Contiki or the Weptech VM, you will have
a version pre-installed in your system. To find out if this is the case, try
this:

    $ arm-none-eabi-gcc -v
    Using built-in specs.
    Target: arm-none-eabi
    Configured with: /scratch/julian/lite-respin/eabi/src/gcc-4.3/configure
    ...
    (skip)
    ...
    Thread model: single
    gcc version 4.3.2 (Sourcery G++ Lite 2008q3-66)

The platform is currently being used/tested with the following toolchains:

* GNU Tools for ARM Embedded Processors. This is the recommended version.
  <https://launchpad.net/gcc-arm-embedded>
* Alternatively, you can use this older version for Linux. At the time of 
  writing, this is the version used by Contiki's regression tests. 
  <https://sourcery.mentor.com/public/gnu_toolchain/arm-none-eabi/arm-2008q3-66-arm-none-eabi-i686-pc-linux-gnu.tar.bz2>

Drivers
-------
On Windows, the drivers for the FTDI234 USB chip on the Coua will be
installed along with [smart-rf-flashprog](TI Flash Programmer 2). You can also
install these drivers directly from [ftdidrivers](FTDI).

On Linux, the ftdi\_sio kernel module works out of the box. You should see a
ttyUSB file under /dev as soon as you plug in the Coua.

Flashing your device
--------------------
After you have everything set up and compiled your firmware, you will need to
flash it onto the hardware in order to run it.

### Using a JTAG Interface like TI SmartRF06 + Uniflash
The CC1310's JTAG interface is exposed via the 20-pin header X5. Most ARM JTAG
debuggers, like the Segger J-Link EDU, feature that same header. Also some
development boards with JTAG debuggers on board, like the TI SmartRF06, expose
that header so you can use them to program your Coua board. This example
continues with the SmartRF06. Connect the header X5 on the Coua and the header
labelled "20-pin ARM JTAG Connector" on the SmartRF06 using an applicable cable
anmd paying close attention to the Pin1 markers.

On the SmartRF board, make sure that the jumpers "VDD to EM" and "Enable UART
over XDS100v3" are on, and the power source switch is set to USB. Disconnect the
Coua from USB. Connect the SRF06 to your computer and turn the board on.

You can use TI's [Flash Programmer 2](smart-rf-flashprog) or [Uniflash](uniflash)
programs to flash the device.  These instructions continue with Uniflash, but
finding the corresponding settings in Flash Programmer 2 should be
straightforward.

Select "CC1310F128" as your device and "Texas Instruments XDS100v3 USB Debug
Probe" as your connection. As your flash image, either select the .bin image of
your firmware and check the "Binary" checkbox, or select the .hex image and
leave the "Binary" checkbox unchecked. If using the binary image, make sure the
load address is set to 0x00. On the Settings tab, make sure that "Erase" is set
to "Entire Flash". Back on the Program tab, click on "Load Image". The console
at the bottom should tell you that everything went alright. To make sure, verify
the image by pressing "Verify Image".

Examples
========
Under `examples/coua` you will find the following examples.

mqtt-client
-----------
This example shows how to use Contiki's own mqtt app to send data to a MQTT
broker. The server that the Coua will send its MQTT messages to is IBM's
Internet of Things Quickstart Server per default, but you may adapt it to send
data to a server of your choice.

### Preparing a border router
This example expects a second 6LoWPAN device to act as a RPL border router with
both access to the RPL DAG via Sub-GHz 6LoWPAN and to the broader internet via
ethernet or SLIP. You can use the Weptech Saker with it's `ip64-border-router`
example firmware (refer to the Saker's README.md), or a different device, like a
second Coua board, running the `ipv6/rpl-border-router` example with SLIP. How
to run that example on the Coua will be described further down.

### Preparing the Coua
The example code is under `examples/coua/mqtt-client`.

#### Prepare the code
Configure the example.
1. In `project-conf.h`, configure the RPL to match your border router's
configuration by setting the correct PAN ID and RF Channel. Most border router
examples use the given default, so they should work out of the box.
2. Also in `project-conf.h`, configure the MQTT Client. You can set the IP and
port of the MQTT broker as well as the interval in which messages are generated.
The IP for the IBM Watson IoT platform is preconfigured, but can be set to the
IP of any computer that runs an MQTT server like Mosquitto.
The messages that are generated follow the format of IBM's Watson IoT Platform
as laid out in the [IBM Bluemix Documentation](ibm-bluemix-mqtt). The defines
that are related to the message generation will only become important once you
begin adapting the message format for your own application.

#### Compile the Code
Compile the code by running `make TARGET=coua`. 

#### Flash the Device
Flash the `mqtt-client.bin` image onto your device with the recipe described
under [Flashing your device](#flashing-your-device). 

### Running it all
Once the Coua and the border router are ready, run it all by connecting them to
USB to power them up (make sure the JTAG interface is disconnected). Check the
`printf` output of the devices using a serial program like `minicom` or `hterm`.
For both our example and the border router example firmwares, the serial
consoles are preconfigured for 8 data bits, 1 Stop bit, no parity at 115200
baud.
The Coua should now connect to your MQTT Server of choice and begin transmitting
data.

#### Viewing the data on a Mosquitto Server
If you are running your own Mosquitto server, you can connect to it with
the `mosquitto_sub` command by subscribing to the topic
`iot-2/evt/status/fmt/json`. This example is for a server running on localhost,
port 50000:
> mosquitto\_sub -h localhost -p 50000 -t iot-2/evt/status/fmt/json


#### Viewing the data on IBM Watson IoT Platform
You can see the way the IBM device IDs are constructed in
`client_mqtt_connect()` in `mqtt-client.c`. The client ID is constructed of the
org\_id and type\_id that you can configure in `project-conf.h`, and the device
ID that is constructed from the Coua's MAC address' first three and last three
bytes, in hexadecimal, without colons. The Coua will print its MAC address on
startup: 

>  Link layer addr: 00:12:4b:00:09:a1:b1:42

Therefore, the device ID for that device will be `00124ba1b142`.

With your device ID, check out the data that the MQTT server has received by
going to <https://quickstart.internetofthings.ibmcloud.com>. 

IPV6/rpl-border-router
----------------------
The border router example under `examples/ipv6/rpl-border-router` can be
compiled for the Coua as is. Just run `make TARGET=coua`, flash the
`border-router.bin` image onto your device and connect it to your computer. The
connection to the broader internet is made via SLIP which requires the
`tunslip6` program running on your computer. It can be found under `tools/` and
compiled by running `make tunslip6`. The resulting `tunslip6` binary can be
started by running `sudo ./tunslip6 fd00::1/64`. The specified address range
will be used by the RPL to allocate IP addresses.

Sensniff
--------
The Saker can be programmed to act as a sniffer to record wireless 802.15.4
communications using [Sensniff](https://github.com/g-oikonomou/sensniff). The
firmware code is available in Contiki under `examples/sensniff`.

### Prepare the code
In `target-conf.h` under `examples/sensniff/coua`, take note of the
`UART0_CONF_BAUD_RATE`, which is set to 460800 by default.

### Compile and flash the firmware
Compile the sniffer firmware by running `make TARGET=coua` and
flash it on the Saker using one of the recipes under [Flashing your
device](#flashing-your-device).

### Run the host tool
The host tool is available from Github, see above. Check out that repository and
read its `README.md`. You can run the host tool by running 
`python sensniff.py -b 460800 -d /dev/ttyUSBX`, exchanging `USBX` for whatever 
port where your Saker is connected. The host tool should report 
`Sniffing in channel: 0`. Make sure to use the baud rate that is configured in
`target-conf.h` as the `-b` parameter. 

A list of available commands will be printed when starting the tool or when you
type `h` or `?`. There are commands available to set the channel in which you
want to sniff.

### Run wireshark
Open the FIFO `/tmp/sensniff` in wireshark by running `wireshark -k -i
/tmp/sensniff`. Packets in the configured channel should appear. 

Advanced Topics
===============
The platform's functionality can be customised by tweaking the various
configuration directives in `platform/coua/contiki-conf.h`. Bear in
mind that defines specified in `contiki-conf.h` can be over-written by defines
specified in `project-conf.h`, which is a file commonly encountered in example
directories.

Thus, if you want to modify the platform's default behaviour, change values in
`contiki-conf.h`. If you want to configure custom behaviour for a specific
example, modify this example's `project-conf.h`. If you have any changes that
you think would be good global defaults, we would love to hear from you on
[Github](https://github.com/Weptech_elektronik).

> Note: Some defines in `contiki-conf.h` are not meant to be modified.

Using the Serial Bootloader
---------------------------
If the device is configured correctly and extended with some additional
hardware, it is possible to start the device in bootloader mode. It is then
possible to load firmware onto the device using the `cc2538-bsl.py` python
script that ships with contiki under tools/cc2538-bsl. In the platforms
`Makefile`, there is an `%.upload` target, so that you can then compile and
flash the device with one convenient console command like 
`make TARGET=coua mqtt-client.bin mqtt-client.upload`.

You need the possibility to externally pull a DIO pin high or low. As an
example, we have attached a push button and a small circuit that will pull DIO9,
which is the third pin on the header X1, low if pressed. Other possible
solutions would be a jumper over pins 2 and 3 of header X1, which pulls DIO9
high (second pin on X1 is VDD).

Once your hardware modifications are done, set `ROM_BOOTLOADER_ENABLE` to 1 in
`platform/coua/contiki-conf.h`. Addtionally, configure the bootloader in
`board.h` by setting the macros `SET_CCFG_BL_CONFIG_*`. They are wrapped in a
conditional statement that sets sane defaults if `ROM_BOOTLOADER_ENABLE` is 0.
Here are the values that we used for the DIO9-pulled-low button:

	#define SET_CCFG_BL_CONFIG_BOOTLOADER_ENABLE            0xC5
	#define SET_CCFG_BL_CONFIG_BL_LEVEL                     0x00
	#define SET_CCFG_BL_CONFIG_BL_PIN_NUMBER                IOID_9
	#define SET_CCFG_BL_CONFIG_BL_ENABLE                    0xC5

The values `0xC5` are magic numbers that enable the bootloader, where
`SET_CCFG_BL_CONFIG_BOOTLOADER_ENABLE` enables the bootloader per se while
`SET_CCFG_BL_CONFIG_BL_ENABLE` enables the backdoor to actually start it, so
both are necessary. `SET_CCFG_BL_CONFIG_BL_PIN_NUMBER` defines the DIO which
triggers the bootloader backdoor. `SET_CCFG_BL_CONFIG_BL_LEVEL` sets the level
on which the bootloader is started, 0 for low, 1 for high.

To start the bootloader mode, connect the device to your computer via USB, then
reset the device while holding down the button.

Then, flash the firmware on your device by running
    python cc2538-bsl.py -e -w -v path/to/image.bin
The given parameters instruct the script to erase the entire flash, write the
given image, and verify whether writing succeeded.

Please note that this script uses the .bin image that will be produced by the
compilation along with numerous others.

Node IEEE/RIME/IPv6 Addresses
-----------------------------
Nodes will generally autoconfigure their IPv6 address based on their IEEE
address. The IEEE address can be read directly from the CC2538 Info Page, or it
can be hard-coded. Additionally, the user may specify a 2-byte value at build
time, which will be used as the IEEE address' 2 LSBs.

To configure the IEEE address source location (Info Page or hard-coded), use the
`IEEE_ADDR_CONF_HARDCODED` define in contiki-conf.h:

* 0: Info Page
* 1: Hard-coded

If `IEEE_ADDR_CONF_HARDCODED` is defined as 1, the IEEE address will take its
value from the `IEEE_ADDR_CONF_ADDRESS` define. If `IEEE_ADDR_CONF_HARDCODED` is
defined as 0, the IEEE address can come from either the primary or secondary
location in the Info Page. To use the secondary address, define
`IEEE_ADDR_CONF_USE_SECONDARY_LOCATION` as 1.

Additionally, you can override the IEEE's 2 LSBs, by using the `NODEID` make
variable. The value of `NODEID` will become the value of the `IEEE_ADDR_NODE_ID`
pre-processor define. If `NODEID` is not defined, `IEEE_ADDR_NODE_ID` will not
get defined either. For example:

    make NODEID=0x79ab

This will result in the 2 last bytes of the IEEE address getting set to 0x79
0xAB.

UART Baud Rate
--------------
By default, the CC1310 UART is configured with a baud rate of 115200. It is easy
to increase this to 230400 by changing the value of `CC26XX_UART_CONF_BAUD_RATE`
in `contiki-conf.h` or `project-conf.h`.

    #define CC26XX_UART_CONF_BAUD_RATE 230400

Low-Power Modes
---------------
The platform takes advantage of the CC1310's power saving features. There are
function calls to the LPM code in `main\(\)` as well as the `transmit\(\)`
routines that put the chip in some low power mode between servicable events or
while the RF core handles a transmission.

The LPM code allows the registration of modules at runtime that get called when
the CPU wants to enter a low power mode, shuts down, or wakes back up. With
these functions, modules can prevent dropping to a power mode that they deem too
low for their operation, request power domains to be left powered on during
deep sleep, prepare for a deep sleep or shutdown, or reconfigure themselves
after a deep sleep. See `cc26xx-uart.c` for an example LPM module. There you
can also see that the `domain\_lock` variable can be set and unset at runtime.

There are three low power modes, "sleep", called "idle" in the CC1310 reference
manual, "deep sleep", called "standby" in the manual, and "shutdown". In sleep
mode, the CPU is shut down, but most other peripherals stay powered on and can
wake up the CPU. In deep sleep, the CPU as well as every power domain that has
not been explicitly locked by an LPM module will be powered down. The regular
sleep phases in `main\(\)` will either drop to sleep or deep sleep.

To determine which power mode to use, the following logic is followed:

* The deepest available low power mode can be hard-coded by using
  the `LPM_MODE_MAX_SUPPORTED` macro in the LPM driver (`lpm.[ch]`). Thus, it
  is possible to prohibit deep sleep altogether.
* Code modules which are affected by low power operation can 'register'
  themselves with the LPM driver.
* If the projected low-power duration is lower than `STANDBY_MIN_DURATION`,
  the chip will simply sleep.
* If the projected low power duration is sufficiently long, the LPM will visit
  all registered modules to query the maximum allowed power mode (maximum means
  sleep vs deep sleep in this context). It will then drop to this power mode.
  This is where a code module can forbid deep sleep if required.
* All registered modules will be notified when the chip is about to enter
  deep sleep, as well as after wake-up.

When the chip does enter deep sleep:

* The RF Core, VIMS, SYSBUS and CPU power domains are always turned off. Due to
  the way the RF driver works, the RFCORE PD should be off already.
* Peripheral clocks stop
* The Serial and Peripheral power domains are turned off, unless an LPM module
  requests them to stay operational. For example, the net-uart demo keeps the
  serial power domain powered on and the UART clocked under sleep and deep
  sleep in order to retain UART RX functionality.
* If both SERIAL and PERIPH PDs are turned off, we also switch power source to
  the uLDO for ultra low leakage under deep sleep.

The chip will come out of low power mode by one of the following events:

* Button press or, in the case of the SensorTag, a reed relay trigger
* Software clock tick (timer). The clock ticks at 128Hz, therefore the maximum
  time we will ever spend in a sleep mode is 7.8125ms. In hardware terms, this
  is an AON RTC Channel 2 compare interrupt.
* Rtimer triggers, as part of ContikiMAC's sleep/wake-up cycles. The rtimer
  sits on the AON RTC channel 0.

Shutdown can be entered by explicitly calling `lpm\_shutdown\(\)`. All LPM
modules that registered an according function are then notified and get the
chance to clean up. Shutdown can not be prevented by the modules. The call to
`lpm_shutdown\(\)` may register a wakeup pin with which the system can be called
from shutdown. The I/O pins are latched to retain their values during shutdown.
The system is then powered off completely, so that apart from the wakeup pin and
the retainment of I/O values, shutdown is indistinguishable from holding the
system in reset.

Code Size Optimisations
-----------------------
The build system currently uses optimization level `-Os`, which is controlled
indirectly through the value of the `SMALL` make variable. This value can be
overridden by example makefiles, or it can be changed directly in
`platform/coua/Makefile.coua`.

Historically, the `-Os` flag has caused problems with some toolchains. If you
are using one of the toolchains documented in this README, you should be able to
use it without issues. If for whatever reason you do come across problems, try
setting `SMALL=0` or replacing `-Os` with `-O2` in
`cpu/cc26xx-cc13xx/Makefile.cc26xx-cc13xx`.

Doxygen Documentation
=====================
This port's code has been documented with doxygen. To build the documentation,
navigate to `$(CONTIKI)/doc` and run `make`. This will build the entire contiki
documentation and may take a while.

If you want to build this platform's documentation only and skip the remaining
platforms, run this:

    make basedirs="platform/coua core cpu/cc26xx-cc13xx examples/coua"

Once you've built the docs, open `$(CONTIKI)/doc/html/index.html` and enjoy.

[smart-rf-studio]: http://www.ti.com/tool/smartrftm-studio "SmartRF Studio"
[smart-rf-flashprog]: http://www.ti.com/tool/flash-programmer "SmartRF Flash Programmer"
[uniflash]: http://processors.wiki.ti.com/index.php/Category:CCS_UniFlash "UniFlash"
[manual]: https://www.weptech.de/6LoWPAN_IoT_Gateway_EN.html?file=files/site/6LoWPAN-IoT-Gateway/6LoWPAN-IoT-Gateway_Manual_v1.4.pdf "Manual"
[ftdidrivers]:http://www.ftdichip.com/FTDrivers.htm
[ibm-bluemix-mqtt]:https://console.ng.bluemix.net/docs/services/IoT/devices/mqtt.html

