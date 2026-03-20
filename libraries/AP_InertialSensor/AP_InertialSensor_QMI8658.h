#pragma once

#include <AP_HAL/AP_HAL.h>
#include <AP_HAL/SPIDevice.h>
#include <AP_HAL/I2CDevice.h>

#include "AP_InertialSensor.h"
#include "AP_InertialSensor_Backend.h"

#ifndef QMI8658_DEFAULT_ROTATION
#define QMI8658_DEFAULT_ROTATION ROTATION_NONE
#endif

class AP_InertialSensor_QMI8658 : public AP_InertialSensor_Backend {
public:
    static AP_InertialSensor_Backend *probe(AP_InertialSensor &imu,
                                            AP_HAL::OwnPtr<AP_HAL::SPIDevice> dev,
                                            enum Rotation rotation=QMI8658_DEFAULT_ROTATION);

    static AP_InertialSensor_Backend *probe(AP_InertialSensor &imu,
                                            AP_HAL::OwnPtr<AP_HAL::I2CDevice> dev,
                                            enum Rotation rotation=QMI8658_DEFAULT_ROTATION);

    void start() override;
    bool update() override;

private:
    AP_InertialSensor_QMI8658(AP_InertialSensor &imu,
                              AP_HAL::OwnPtr<AP_HAL::Device> dev,
                              enum Rotation rotation);

    bool init();
    void read_sample();
    bool read_register(uint8_t reg, uint8_t &value);
    bool write_register(uint8_t reg, uint8_t value);

    AP_HAL::OwnPtr<AP_HAL::Device> _dev;
    AP_HAL::Device::PeriodicHandle _periodic_handle;
    enum Rotation _rotation;
};
