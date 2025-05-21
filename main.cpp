#include <gpiod.h>
#include <unistd.h>
#include <iostream>

#define GPIO_CHIP "/dev/gpiochip0"
#define GPIO_LINE 17     // GPIO 17 (Physical pin 11 on Pi)

int main() {
    gpiod_chip* chip = gpiod_chip_open(GPIO_CHIP);
    if (!chip) {
        std::cerr << "Failed to open GPIO chip\n";
        return 1;
    }

    gpiod_line* line = gpiod_chip_get_line(chip, GPIO_LINE);
    if (!line) {
        std::cerr << "Failed to get GPIO line\n";
        gpiod_chip_close(chip);
        return 1;
    }

    if (gpiod_line_request_output(line, "led-blinker", 0) < 0) {
        std::cerr << "Failed to request GPIO line as output\n";
        gpiod_chip_close(chip);
        return 1;
    }

    std::cout << "Blinking LED on GPIO " << GPIO_LINE << "...\n";

    for (int i = 0; i < 10; ++i) {
        gpiod_line_set_value(line, 1);
        usleep(500000); // 0.5 sec on
        gpiod_line_set_value(line, 0);
        usleep(500000); // 0.5 sec off
    }

    gpiod_line_release(line);
    gpiod_chip_close(chip);
    return 0;
}
