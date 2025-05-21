#pragma once

#include <string>

struct Readings {
    float voltage[5];
    float current[5];
    float velocity1;
};

void readings_init();
Readings exec_reading();
std::string readingToString(const Readings& reading);