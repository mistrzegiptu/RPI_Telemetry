#include "readings.hpp"
#include "hall_sensor.hpp"
#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <cstdint>
#include <gpiod.h>
#include <string>
#include <sstream>
#include <iomanip>

#define SPI_DEVICE "/dev/spidev0.0"
#define CS_GPIO_CHIP "/dev/gpiochip0"
#define CS_PIN_1 5
#define CS_PIN_2 6
#define CS_PIN_3 22
#define CS_PIN_4 7
#define CS_PIN_5 12

const int cs_pins[] = {CS_PIN_1, CS_PIN_2, CS_PIN_3, CS_PIN_4, CS_PIN_5};
const int num_devices = sizeof(cs_pins) / sizeof(cs_pins[0]);


const float vRef = 5.0;
const float constVoltageScalling = 0.96;
const float scaleVoltage = (10.0 + 1.2) / 1.2;

const float zeroCurrentVoltage = 2.551;
const float sensitivityACS = 0.040;
const float constCurrentScalling = 1.06;

int spi_fd = -1;
gpiod_line_bulk cs_lines;
gpiod_chip* chip = nullptr;

void readings_init() {
    memset(&cs_lines, 0, sizeof(cs_lines));

    spi_fd = open(SPI_DEVICE, O_RDWR);

    if (spi_fd < 0) {
        std::cerr << "Failed to open SPI devices" << "\n";
        return;
    }

    uint8_t mode = SPI_MODE_0 | SPI_NO_CS;
    uint32_t speed = 1000000;

    ioctl(spi_fd, SPI_IOC_WR_MODE, &mode);
    ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);

    chip = gpiod_chip_open(CS_GPIO_CHIP);
    if(!chip){
        std::cerr << "Failed to open GPIO chip " << "\n";
        return;
    }

    for(int pin: cs_pins){
        gpiod_line* line = gpiod_chip_get_line(chip, pin);
        if(pin != 7 && (line == nullptr || gpiod_line_request_output(line, "spi-cs", 1) < 0)){
            std::cerr << "Failed to request GPIO line for CS pin " << pin << "\n";
            gpiod_chip_close(chip);
            return;
        }
        gpiod_line_bulk_add(&cs_lines, line);
    }
}

void selectCS(int index){
    gpiod_line_set_value(cs_lines.lines[index], 0);
}

void deselectCS(int index){
    gpiod_line_set_value(cs_lines.lines[index], 1);
}

float adcToVoltage(uint16_t adcValue) {
    float voltage = (adcValue * vRef) / 4095.0;
    voltage *= constVoltageScalling;
    return voltage * scaleVoltage;
}

float adcToCurrent(uint16_t adcValue) {
    float currentVoltage = (adcValue * vRef) / 4095.0;
    currentVoltage *= constCurrentScalling;
    currentVoltage -= zeroCurrentVoltage;
    return currentVoltage / sensitivityACS;
}

uint16_t readADCWithCS(int deviceIndex, uint8_t channel) {
    if (deviceIndex >= num_devices || channel > 1) return 0;

    selectCS(deviceIndex);

    uint8_t tx[] = { 0b00000001, (uint8_t)(0b10000000 | (channel << 6)), 0x00 };
    uint8_t rx[3] = {};

    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx,
        .rx_buf = (unsigned long)rx,
        .len = 3,
        .speed_hz = 1000000,
        .delay_usecs = 0,
        .bits_per_word = 8,
    };

    if (ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr) < 1) {
        std::cerr << "SPI transfer failed " << "\n";
        deselectCS(deviceIndex);
        return 0;
    }

    deselectCS(deviceIndex);

    uint16_t result = ((rx[1] & 0x0F) << 8) | rx[2];
    return result;
}

extern HallSensor hall_sensor_instance;

Readings exec_reading() {
    Readings reading;
    
    for(int i = 0; i < 5; i++){
        float voltage = readADCWithCS(i, 0);
        float current = readADCWithCS(i, 1);
        
        reading.voltage[i] = adcToVoltage(voltage);
        reading.current[i] = adcToCurrent(current);
    }

    reading.velocity1 = hall_sensor_instance.current_velocity;

    return reading;
}

std::string readingToString(const Readings& reading) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(3);

    for (int i = 0; i < 5; ++i) {
        oss << reading.voltage[i] << ";";
    }

    for (int i = 0; i < 5; ++i) {
        oss << reading.current[i] << ";";
    }

    oss << reading.velocity1;

    return oss.str();
}