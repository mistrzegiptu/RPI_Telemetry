#include "readings.hpp"
#include "hall_sensor.hpp"
#include "sd_card_save.hpp"
#include "mqtt_communication.hpp"

#include <chrono>
#include <thread>
#include <iostream>

unsigned long millis() {
    static auto start = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
}

int main() {
    std::cout << "Initializing...\n";

    hall_sensor_instance.begin();
    readings_init();
    sd_card_save_init();

    server_communication_instance.begin();

    std::this_thread::sleep_for(std::chrono::seconds(5));

    std::cout << "Running loop...\n";

    const unsigned long measurementInterval = 10;
    const unsigned long flushInterval = 1000;

    unsigned long lastMeasurementTime = millis();
    unsigned long lastFlushTime = millis();

    while (true) {
        unsigned long now = millis();

        if (now - lastMeasurementTime >= measurementInterval) {
            lastMeasurementTime = now;

            Readings reading = exec_reading();
            std::cout << readingToString(reading) << "\n";
            sd_card_save_append(reading);

            if (now - lastFlushTime >= flushInterval) {
                lastFlushTime = now;

                std::cout<<"Flushing"<<std::endl;
                server_communication_instance.publishMqttMessage(readingToString(reading));
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    sd_card_save_close();
    return 0;
}