# FlyingRC F4Wing Mini Flight Controller

FlyingRC F4Wing Mini is an ultra mini flight controller produced by the company [FlyingRC](https://cs.flyingrc.cn/en)
[Website product link](https://cs.flyingrc.cn/en/products/57). 
It is recommended to purchase the product from retailers on aliexpress or shopee.
This flight controller is most suitable for lite(<500g) and ultra-lite(<250g) fixed-wing FPV builds.
Note that the flight controller draws power from the same external BEC that is used for powering servos, please ensure that the external BEC is reliable.

## Features

- STM32F405RGT6 microcontroller
- ICM42605(for HW_V1 and HW_V3) or ICM42688-P(for HW_V2) IMU 
- SPL06(for HW_V1) or SPA06(for HW_V2 and HW_V3) barometer
- No onboard BEC - to minimize size
- No onboard Memory - to minimize size
- 3x UART
- 1x SBUS Input
- 7x PWM
- 1x I2C

## Pinout

![FlyingRC F4Wing Mini](FlyingRC_F4Wing_Mini_PINOUT.jpeg "FlyingRC F4Wing Mini")

## UART Mapping

 - SERIAL0 -> USB
 - SERIAL1 -> USART1 (Telem)
 - SERIAL2 -> UART5 (GPS)
 - SERIAL3 -> UART4 (DJI) 

## RC Input

The default RC input is configured on the UART3 (RX3/SBUS). Non SBUS,  single wire serial inputs can be directly tied to RX3 if SBUS pin is left unconnected. RC could  be applied instead at a different UART port such as UART4 or UART8, and set the protocol to receive RC data: ``SERIALn_PROTOCOL = 23`` and change SERIAL3 _Protocol to something other than '23'.

- PPM is NOT supported.  
- SBUS connects to the SBUS pin.   
- CRSF/ELRS is recommended to connect to RX1 and TX1.

## OSD Support

Analog OSD is not supported.
DisplayPort OSD is available on the HD VTX connector.

## VTX Support

The SH1.0-6P connector supports a DJI Air Unit / HD VTX connection. Protocol defaults to DisplayPort. Pin 1 of the connector is the battery voltage so be careful not to connect this to a peripheral requiring 5v. DisplayPort OSD is enabled by default on SERIAL3.

## VTX power control

GPIO 83 controls the VTX BEC output to pins marked "12V" and is included on the HD VTX connector. Setting this GPIO low removes voltage supply to this pin/pad. By default RELAY3 is configured to control this pin and sets the GPIO high.

## PWM Output

The FlyingRC F4Wing Mini supports up to 7 PWM (6 + LED) outputs. All the PWM outputs are provided through the standard 2.54mm-pitch connectors.

The PWM is in 4 groups:

* PWM 1-2 in group1
* PWM 3-4 in group2
* PWM 5-6 in group3
* PWM LED in group4

Channels within the same group need to use the same output rate. If
any channel in a group uses DShot then all channels in the group need
to use DShot.

## Battery Monitoring

The board only has a internal voltage sensor, users can connect the SH1.0-2P connector to the battery voltage directly.
Analog current sensor is NOT supported.
The voltage sensor can handle up to 6S LiPo batteries.

The default battery parameters are:

 - :ref:BATT_MONITOR<BATT_MONITOR> = 3
 - :ref:BATT_VOLT_PIN<BATT_VOLT_PIN__AP_BattMonitor_Analog> = 14
(VOLT pin)
 - :ref:BATT_VOLT_MULT<BATT_VOLT_MULT__AP_BattMonitor_Analog> = 21.0

## Compass

The FlyingRC F4Wing Mini does not have a built-in compass, but you can attach an external compass using I2C on the SDA and SCL connector.

## GPIOs

No PinIO available on this board.

## Physical

- Mounting: 17.8 x 17.8mm, 1.2mm
- Dimensions: 20.3 x 27.9 x 11.2 mm
- Weight: 2.8g (without soldering Pins)

## Firmware

Firmware for the FlyingRC F4Wing Mini is available from [ArduPilot Firmware Server](https://firmware.ardupilot.org) under the `FlyingRCF4WingMini` target.

## Loading Firmware

To flash firmware initially, connect USB while holding the bootloader button and use DFU to load the `with_bl.hex` file. Subsequent updates can be applied using `.apj` files through a ground station.
