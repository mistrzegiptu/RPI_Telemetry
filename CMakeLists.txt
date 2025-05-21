cmake_minimum_required(VERSION 3.10)
project(Telemetry)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

find_package(PkgConfig REQUIRED)

pkg_check_modules(LIBGPIOD REQUIRED libgpiod)
pkg_check_modules(MOSQUITTO REQUIRED libmosquitto)

add_executable(main
    src/main.cpp
    src/hall_sensor.cpp
    src/readings.cpp
    src/sd_card_save.cpp
    src/mqtt_communication.cpp
)

target_link_libraries(main
    ${LIBGPIOD_LIBRARIES}
    ${MOSQUITTO_LIBRARIES}
    pthread
)
