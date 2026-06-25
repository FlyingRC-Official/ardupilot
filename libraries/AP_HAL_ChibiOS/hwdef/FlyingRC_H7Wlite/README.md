# FlyingRC H7Wlite Flight Controller

The FlyingRC H7Wlite is an STM32H743-based wing flight controller.

## Features

- MCU: STM32H743
- IMU: dual BMI270
- Alternative IMU hardware support: ICM42688, ICM42605, ICM20602, MPU6000
- Barometer: SPL06/SPA06, DPS310, BMP280, or MS56XX
- OSD: MAX7456
- microSD card logging
- 12 PWM outputs plus one LED output
- 2x I2C buses for external compass, airspeed, and other peripherals
- 1x CAN port
- USB

## UART Mapping

The serial ordering is:

- SERIAL0 -> USB
- SERIAL1 -> UART7 (TELEM1, with RTS/CTS)
- SERIAL2 -> USART1 (TELEM2)
- SERIAL3 -> USART2 (GPS1)
- SERIAL4 -> USART3 (GPS2)
- SERIAL5 -> UART8
- SERIAL6 -> UART4
- SERIAL7 -> USART6 (RC input)

## RC Input

RC input is configured on the USART6 RX pin. It supports all ArduPilot serial RC protocols.

USART6 can also be used as a normal UART by setting `BRD_ALT_CONFIG=1`. This enables the alternate USART6 RX configuration for protocols such as FPort that need a bidirectional UART receiver connection.

## OSD Support

FlyingRC H7Wlite supports analog OSD using `OSD_TYPE=1` with the MAX7456 driver.

## PWM Output

The board supports 12 PWM outputs plus one LED output.

The PWM outputs are grouped by timer:

- PWM 1-2: TIM8
- PWM 3-6: TIM5
- PWM 7-10: TIM4
- PWM 11-12: TIM15
- LED output: TIM1

Channels within the same timer group need to use the same output rate. If any channel in a group uses DShot then all channels in that group need to use DShot.

## Battery Monitoring

The firmware defaults enable the first battery monitor:

- `BATT_MONITOR`: 4
- `BATT_VOLT_PIN`: 10
- `BATT_CURR_PIN`: 11
- `BATT_VOLT_MULT`: 11.0
- `BATT_AMP_PERVLT`: 40.0

A second voltage/current ADC input is also defined:

- `BATT2_VOLT_PIN`: 18
- `BATT2_CURR_PIN`: 7
- `BATT2_VOLT_MULT`: 11.0

## Airspeed

An analog airspeed input is available on analog pin 4.

## Compass

FlyingRC H7Wlite does not have a built-in compass. External compasses can be attached on the I2C buses.

## GPIO

Two user-controllable PINIO outputs are defined:

- `PINIO1`: GPIO 81
- `PINIO2`: GPIO 82

## Loading Firmware

Initial firmware loading can be done with DFU by plugging in USB with the bootloader button pressed. Load the `FlyingRC_H7Wlite_bl.hex` or a `with_bl.hex` firmware image using your preferred DFU loading tool.

After the initial bootloader is installed, firmware can be updated from an ArduPilot ground station using the `.apj` firmware file.
