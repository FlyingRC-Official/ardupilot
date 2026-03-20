/*
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "AP_InertialSensor_QMI8658.h"

#include <utility>

#include <AP_HAL/AP_HAL.h>
#include <AP_HAL/utility/sparse-endian.h>
#include <AP_Math/AP_Math.h>

extern const AP_HAL::HAL& hal;

namespace {

enum Register : uint8_t {
    REG_WHO_AM_I  = 0x00,
    REG_CTRL1     = 0x02,
    REG_CTRL2     = 0x03,
    REG_CTRL3     = 0x04,
    REG_CTRL5     = 0x06,
    REG_CTRL7     = 0x08,
    REG_STATUSINT = 0x2D,
    REG_TEMP_L    = 0x33,
    REG_AX_L      = 0x35,
    REG_RESET     = 0x60,
};

struct PACKED SensorData {
    le16_t temp;
    le16_t ax;
    le16_t ay;
    le16_t az;
    le16_t gx;
    le16_t gy;
    le16_t gz;
};

static_assert(sizeof(SensorData) == 14, "QMI8658 sensor data must be 14 bytes");

constexpr uint8_t CHIP_ID = 0x05;
constexpr uint8_t RESET_CMD = 0xB0;

constexpr uint8_t CTRL1_AUTO_INCREMENT = 1U << 6;
constexpr uint8_t CTRL2_ACC_16G_940HZ = 0x33;
constexpr uint8_t CTRL3_GYR_2048DPS_940HZ = 0x73;
constexpr uint8_t CTRL5_NO_LPF = 0x00;
constexpr uint8_t CTRL7_ENABLE_6DOF_SYNC = 0x83;

constexpr uint8_t STATUSINT_AVAIL = 1U << 0;

constexpr uint16_t BACKEND_SAMPLE_RATE_HZ = 940;
constexpr uint32_t BACKEND_PERIOD_US = 1000000UL / BACKEND_SAMPLE_RATE_HZ;

constexpr float ACCEL_SCALE_16G = (GRAVITY_MSS * 16.0f) / 32768.0f;
constexpr float GYRO_SCALE_2048DPS = DEG_TO_RAD * (2048.0f / 32768.0f);
constexpr float TEMP_SCALE = 1.0f / 256.0f;

constexpr uint8_t INIT_RETRIES = 5;
constexpr uint8_t POWERUP_DELAY_MS = 20;
constexpr uint8_t RESET_DELAY_MS = 20;
constexpr uint16_t SENSOR_STARTUP_DELAY_MS = 160;

}

AP_InertialSensor_QMI8658::AP_InertialSensor_QMI8658(AP_InertialSensor &imu,
                                                     AP_HAL::OwnPtr<AP_HAL::Device> dev,
                                                     enum Rotation rotation)
    : AP_InertialSensor_Backend(imu)
    , _dev(std::move(dev))
    , _rotation(rotation)
{
}

AP_InertialSensor_Backend *AP_InertialSensor_QMI8658::probe(AP_InertialSensor &imu,
                                                            AP_HAL::OwnPtr<AP_HAL::SPIDevice> dev,
                                                            enum Rotation rotation)
{
    if (!dev) {
        return nullptr;
    }

    auto *sensor = NEW_NOTHROW AP_InertialSensor_QMI8658(imu, std::move(dev), rotation);
    if (!sensor) {
        return nullptr;
    }

    if (!sensor->init()) {
        delete sensor;
        return nullptr;
    }

    return sensor;
}

AP_InertialSensor_Backend *AP_InertialSensor_QMI8658::probe(AP_InertialSensor &imu,
                                                            AP_HAL::OwnPtr<AP_HAL::I2CDevice> dev,
                                                            enum Rotation rotation)
{
    if (!dev) {
        return nullptr;
    }

    auto *sensor = NEW_NOTHROW AP_InertialSensor_QMI8658(imu, std::move(dev), rotation);
    if (!sensor) {
        return nullptr;
    }

    if (!sensor->init()) {
        delete sensor;
        return nullptr;
    }

    return sensor;
}

void AP_InertialSensor_QMI8658::start()
{
    if (!_imu.register_accel(accel_instance, BACKEND_SAMPLE_RATE_HZ, _dev->get_bus_id_devtype(DEVTYPE_INS_QMI8658)) ||
        !_imu.register_gyro(gyro_instance, BACKEND_SAMPLE_RATE_HZ, _dev->get_bus_id_devtype(DEVTYPE_INS_QMI8658))) {
        return;
    }

    set_accel_orientation(accel_instance, _rotation);
    set_gyro_orientation(gyro_instance, _rotation);

    _periodic_handle = _dev->register_periodic_callback(BACKEND_PERIOD_US,
        FUNCTOR_BIND_MEMBER(&AP_InertialSensor_QMI8658::read_sample, void));
}

bool AP_InertialSensor_QMI8658::update()
{
    update_accel(accel_instance);
    update_gyro(gyro_instance);
    return true;
}

bool AP_InertialSensor_QMI8658::read_register(uint8_t reg, uint8_t &value)
{
    return _dev->read_registers(reg, &value, 1);
}

bool AP_InertialSensor_QMI8658::write_register(uint8_t reg, uint8_t value)
{
    return _dev->write_register(reg, value, true);
}

bool AP_InertialSensor_QMI8658::init()
{
    if (_dev->bus_type() == AP_HAL::Device::BUS_TYPE_SPI) {
        _dev->set_read_flag(0x80);
    }

    hal.scheduler->delay(POWERUP_DELAY_MS);

    WITH_SEMAPHORE(_dev->get_semaphore());

    _dev->set_speed(AP_HAL::Device::SPEED_LOW);

    for (uint8_t attempt = 0; attempt < INIT_RETRIES; attempt++) {
        if (!write_register(REG_RESET, RESET_CMD)) {
            continue;
        }
        hal.scheduler->delay(RESET_DELAY_MS);

        uint8_t whoami = 0;
        if (!read_register(REG_WHO_AM_I, whoami) || whoami != CHIP_ID) {
            continue;
        }

        if (!write_register(REG_CTRL1, CTRL1_AUTO_INCREMENT) ||
            !write_register(REG_CTRL2, CTRL2_ACC_16G_940HZ) ||
            !write_register(REG_CTRL3, CTRL3_GYR_2048DPS_940HZ) ||
            !write_register(REG_CTRL5, CTRL5_NO_LPF) ||
            !write_register(REG_CTRL7, CTRL7_ENABLE_6DOF_SYNC)) {
            continue;
        }

        hal.scheduler->delay(SENSOR_STARTUP_DELAY_MS);
        _dev->set_speed(AP_HAL::Device::SPEED_HIGH);
        return true;
    }

    return false;
}

void AP_InertialSensor_QMI8658::read_sample()
{
    WITH_SEMAPHORE(_dev->get_semaphore());

    uint8_t status = 0;
    if (!read_register(REG_STATUSINT, status)) {
        _inc_accel_error_count(accel_instance);
        _inc_gyro_error_count(gyro_instance);
        return;
    }

    if ((status & STATUSINT_AVAIL) == 0) {
        return;
    }

    SensorData data {};
    if (!_dev->read_registers(REG_TEMP_L, reinterpret_cast<uint8_t *>(&data), sizeof(data))) {
        _inc_accel_error_count(accel_instance);
        _inc_gyro_error_count(gyro_instance);
        return;
    }

    const uint64_t sample_us = AP_HAL::micros64();

    Vector3f accel{
        float(int16_t(le16toh(data.ax))),
        float(int16_t(le16toh(data.ay))),
        float(int16_t(le16toh(data.az)))
    };
    accel *= ACCEL_SCALE_16G;
    _rotate_and_correct_accel(accel_instance, accel);
    _notify_new_accel_raw_sample(accel_instance, accel, sample_us);

    Vector3f gyro{
        float(int16_t(le16toh(data.gx))),
        float(int16_t(le16toh(data.gy))),
        float(int16_t(le16toh(data.gz)))
    };
    gyro *= GYRO_SCALE_2048DPS;
    _rotate_and_correct_gyro(gyro_instance, gyro);
    _notify_new_gyro_raw_sample(gyro_instance, gyro, sample_us);

    _publish_temperature(accel_instance, float(int16_t(le16toh(data.temp))) * TEMP_SCALE);
}
