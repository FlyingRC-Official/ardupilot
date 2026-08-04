# FlyingRC F4Wing Mini Flight Controller

The [FlyingRC F4Wing Mini MK1](https://flyingrc-official.github.io/en/products/f4wing-mini-mk1/)
is a 2.8 g STM32F405 fixed-wing flight controller for small and ultra-light FPV
aircraft. It has no onboard BEC, so the flight controller and servos must be fed
from a clean, reliable external 5 V supply.

## Where to Buy

The board is available from the
[FlyingRC Taobao store](https://e7wgwo2ehnynhjklw2knt535zmlw176.world.taobao.com/shop/view_shop.htm?appUid=RAzN8HAiDiXrnbmP2phqB88hKp1Wt).

## Features

- STM32F405RGT6 microcontroller
- BMI270 IMU on current boards; ICM42688-P or ICM42605 on earlier revisions
- SPL06 or SPA06 barometer
- 6 primary PWM outputs plus the `S12` LED pad as PWM7
- 3 UARTs, a dedicated inverted SBUS input, I2C, and USB-C
- HD digital-VTX connector with direct battery-voltage pass-through
- External battery-voltage sensing; no analog current sensor
- No onboard BEC, dataflash, or analog OSD

## Pinout

![FlyingRC F4Wing Mini](FlyingRC_F4Wing_Mini_PINOUT.jpeg "FlyingRC F4Wing Mini")

## UART Mapping

The ArduPilot `SERIALx` number is not the same as the connector UART number.

| ArduPilot port | MCU peripheral | Board labels/use | Direction | Default protocol | DMA |
| --- | --- | --- | --- | --- | --- |
| `SERIAL0` | USB OTG1 | USB-C | bidirectional | MAVLink | USB |
| `SERIAL1` | USART1 | `T1`/`R1`, ELRS/CRSF | bidirectional | RC input (`23`) | RX and TX DMA |
| `SERIAL2` | UART5 | `T5`/`R5`, GPS | bidirectional | GPS (`5`) | interrupt driven (`NODMA`) |
| `SERIAL3` | UART4 | `T4`/`R4`, HD VTX | bidirectional | MSP DisplayPort (`42`) | interrupt driven (`NODMA`) |

## RC Input

ELRS/CRSF receivers connect to `R1` and `T1`; `SERIAL1_PROTOCOL` defaults to RC
input so bidirectional receiver telemetry is available. The dedicated `SBUS` pad
is a separate, hardware-inverted timer input and does not have a `SERIALx`
number. Connect only one receiver signal source during initial setup.

PPM is not supported on this target.

## GPS and I2C

Connect a serial GPS to `T5`/`R5` (`SERIAL2`). The `DA1`/`CL1` pads expose I2C1
for an external compass, airspeed sensor, or other supported I2C peripheral. The
board has no internal compass.

## OSD and HD VTX

Analog OSD is not supported. The `T4`/`R4` HD VTX connector defaults to MSP
DisplayPort on `SERIAL3`, and `OSD_TYPE2` defaults to `5`.

The HD VTX power pin is fed directly from the battery-voltage input. It is not a
regulated or software-switchable output. Verify that the connected VTX accepts
the battery voltage before applying power.

## PWM Output

| Output | Board label | MCU pin | Timer channel | Rate group |
| --- | --- | --- | --- | --- |
| PWM1 | `S1` | PC9 | TIM8_CH4 | 1 |
| PWM2 | `S2` | PC8 | TIM8_CH3 | 1 |
| PWM3 | `S3` | PB15 | TIM1_CH3N | 2 |
| PWM4 | `S4` | PA8 | TIM1_CH1 | 2 |
| PWM5 | `S5` | PB11 | TIM2_CH4 | 3 |
| PWM6 | `S6` | PB10 | TIM2_CH3 | 3 |
| PWM7 | `S12` LED pad | PB1 | TIM3_CH4 | 4 |

Outputs in the same rate group must use the same output rate. If one output in a
group uses DShot, every output in that group must use DShot.

## Battery Monitoring and Power

The SH1.0-2P battery connector is used for voltage sensing and HD VTX
pass-through power; it does not power the flight controller itself. The voltage
input supports up to 6S LiPo. The default battery parameters are:

- `BATT_MONITOR = 3`
- `BATT_VOLT_PIN = 14`
- `BATT_VOLT_MULT = 21.0`

There is no analog current sensor. Power the flight controller through its 5 V
input from an external BEC, and confirm common ground between the battery, BEC,
flight controller, ESC, and peripherals.

## LEDs

- `ACT` (blue): PA14
- `B/E` (green): PA13

## Physical

- Mounting: 17.8 x 17.8 mm
- Dimensions: 20.3 x 27.9 x 11.2 mm
- Weight: 2.8 g without soldered pins

## Loading Firmware

For the first installation, hold the BOOT button while connecting USB and load
the `FlyingRCF4WingMini` `with_bl.hex` firmware with an STM32 DFU tool. Later
updates can use the `.apj` file through an ArduPilot ground station.
