#include "mqtt_communication.hpp"
#include <mosquitto.h>
#include <iostream>
#include <thread>
#include <chrono>

const std::string CLIENT_ID = "RaspberryPiClient";
const std::string CA_CERT_PATH = "/home/hydrive/certs/ca.crt";

ServerCommunication server_communication_instance;

struct mosquitto* mqtt_client = nullptr;

void ServerCommunication::begin() {
    mosquitto_lib_init();

    mqtt_client = mosquitto_new(CLIENT_ID.c_str(), true, nullptr);
    if (!mqtt_client) {
        std::cerr << "Failed to create Mosquitto client." << std::endl;
        return;
    }

    // Set username and password
    mosquitto_username_pw_set(mqtt_client, mqtt_username.c_str(), mqtt_password.c_str());

    // Set TLS options
    int ret = mosquitto_tls_set(mqtt_client, CA_CERT_PATH.c_str(), nullptr, nullptr, nullptr, nullptr);
    if (ret != MOSQ_ERR_SUCCESS) {
        std::cerr << "Failed to set TLS: " << mosquitto_strerror(ret) << std::endl;
        return;
    }

    // Connect to the broker
    ret = mosquitto_connect(mqtt_client, mqtt_server.c_str(), mqtt_port, 60);
    if (ret != MOSQ_ERR_SUCCESS) {
        std::cerr << "MQTT connection failed: " << mosquitto_strerror(ret) << std::endl;
        return;
    }

    std::cout << "MQTT Connected!" << std::endl;

    // Start a background loop thread
    mosquitto_loop_start(mqtt_client);
}

void ServerCommunication::mqttReconnect() {
    if (!mqtt_client) return;

    int ret = mosquitto_reconnect(mqtt_client);
    if (ret == MOSQ_ERR_SUCCESS) {
        std::cout << "MQTT Reconnected!" << std::endl;
    } else {
        std::cerr << "MQTT reconnection failed: " << mosquitto_strerror(ret) << std::endl;
    }
}

void ServerCommunication::wifiReconnect() {
    std::cout << "WiFi reconnection should be handled externally via OS tools." << std::endl;
}

void ServerCommunication::publishMqttMessage(std::string payload) {
    if (!mqtt_client) return;

    int ret = mosquitto_publish(
        mqtt_client,
        nullptr,
        mqtt_publish_topic.c_str(),
        payload.size(),
        payload.c_str(),
        1, // QoS
        true // Retained
    );

    if (ret != MOSQ_ERR_SUCCESS) {
        std::cerr << "MQTT publish failed: " << mosquitto_strerror(ret) << std::endl;
    }
}