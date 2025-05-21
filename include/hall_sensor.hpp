#pragma once

#include <chrono>

#define VELOCITY_MEASURE_PIN_1 16
#define VELOCITY_MEASURE_PIN_2 4

const double WHEEL_CIRCUMFERENCE = 1.4444;

class HallSensor {
public:
    HallSensor();
    void begin();
    void loop();

    float current_velocity;
    
private:
    static void addRotationSensor();
    void calculateVelocity();


    static volatile long rotation_sensor1;
    static volatile long rotations_sensor2;
    static std::chrono::steady_clock::time_point interrupt_time;

    std::chrono::steady_clock::time_point last_velocity_measure;
    const std::chrono::milliseconds measure_time_ms{1500};
};

extern HallSensor hall_sensor_instance;
