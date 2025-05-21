#pragma once

#include <string>

class ServerCommunication {
public:
    void begin();
    void mqttReconnect();
    void wifiReconnect();
    void publishMqttMessage(std::string payload);
private:
    std::string mqtt_server = "6aa9c1de47b0404e82629f1961517374.s1.eu.hivemq.cloud";
    std::string mqtt_username = "dataServer";
    std::string mqtt_password = "Hydrive1";
    std::string mqtt_publish_topic = "esp32/pub";
    const uint16_t mqtt_port = 8883;
};

extern ServerCommunication server_communication_instance;