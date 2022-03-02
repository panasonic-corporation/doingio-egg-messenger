#pragma once

#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#define BUFFER_SIZE 20000

class SlackApiClient {
private:

public:
    static void getWssLink(JsonObject *response_json);
};