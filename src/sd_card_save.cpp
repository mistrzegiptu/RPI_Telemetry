#include <fstream>
#include <iostream>
#include <string>
#include "readings.hpp"

#include <chrono>

unsigned long millis_log() {
    static auto start = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
}

const std::string logPath = "/home/hydrive/readings.txt";
static std::ofstream logFile; 

void sd_card_save_init() {
    logFile.open(logPath, std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "Error opening log file.\n";
    }
}

void sd_card_save_append(Readings reading) {
    if (logFile.is_open()) {
        logFile<< millis_log() << ": " << readingToString(reading) << "\n"; //TODO: Change those values
    } else {
        std::cerr << "Log file not open, cannot write.\n";
    }
}

void sd_card_save_close() {
    if (logFile.is_open()) {
        logFile.close();
    }
}