#include "Config.h"
#include <ArduinoWebsockets.h>
#include <WiFi.h>
#include <FastLED.h>
#include "SlackApiClient.h"

using namespace websockets;

int state_list[8];
int tick = 0;

CRGB leds[NUM_LEDS];
WebsocketsClient client;

void wifiConfig()
{
    // 前回接続時情報で接続する
    connectWifi(30);
    Serial.println("");
    // 未接続の場合にはSmartConfig待受
    if (WiFi.status() != WL_CONNECTED)
    {
        WiFi.mode(WIFI_STA);
        WiFi.beginSmartConfig();
        Serial.println("Waiting for SmartConfig");
        while (!WiFi.smartConfigDone())
        {
            delay(500);
            Serial.print("#");
            // 30秒以上接続できなかったら再起動
            if (30000 < millis())
            {
                Serial.println("");
                Serial.println("Reset");
                ESP.restart();
            }
        }
        // Wi-fi接続
        Serial.println("");
        Serial.println("Waiting for WiFi");
        connectWifi(1);
    }
}

// WiFi接続
bool connectWifi(int retry_count)
{
    if ((WiFi.status() != WL_CONNECTED))
    {
        WiFi.begin();
        while (WiFi.status() != WL_CONNECTED)
        {
            retry_count--;
            if (retry_count < 0)
            {
                Serial.println("");
                return false;
            }
            Serial.print('.');
            delay(500);
        }
        Serial.println();
        Serial.printf("Connected, IP address : ");
        Serial.println(WiFi.localIP());
        return true;
    }
    else
    {
        return true;
    }
}

void onMessageCallback(WebsocketsMessage message) {
    Serial.print("Got Message: ");
    Serial.println(message.data());
    DynamicJsonDocument json_buffer(BUFFER_SIZE);
    auto error = deserializeJson(json_buffer, message.data());
    if (error)
    {
        Serial.print("Failed to deserializeJson()");
    }
    else
    {
        JsonObject response_json = json_buffer.as<JsonObject>();
        String type = response_json["payload"]["event"]["type"].as<String>();
        Serial.print("type : ");
        Serial.println(type);
        if (type.equals("reaction_added"))
        {
            int retry_attempt = response_json["retry_attempt"].as<int>();
            if (retry_attempt == 0)
            {
                String reaction = response_json["payload"]["event"]["reaction"].as<String>();
                Serial.println(reaction);
                setState(reaction);
            }
        }
        else
        {
            Serial.println("not reaction");
        }
    }
}

void onEventsCallback(WebsocketsEvent event, String data) {
    if (event == WebsocketsEvent::ConnectionOpened)
    {
        Serial.println("Connnection Opened");
    }
    else if (event == WebsocketsEvent::ConnectionClosed)
    {
        Serial.println("Connnection Closed");
    }
    else if (event == WebsocketsEvent::GotPing)
    {
        Serial.println("Got a Ping!");
    }
    else if (event == WebsocketsEvent::GotPong)
    {
        Serial.println("Got a Pong!");
    }
}

CRGB getColorFromState(int s) {
    switch (s) {
    case -1:
        return CRGB(255, 255, 255);
        break;
    case 0:
        return CRGB(255, 255, 0);
        break;
    case 1:
        return CRGB(100, 255, 0);
        break;
    case 2:
        return CRGB(0, 255, 0);
        break;
    case 3:
        return CRGB(0, 255, 255);
        break;
    case 4:
        return CRGB(0, 0, 255);
        break;
    case 5:
        return CRGB(255, 0, 255);
        break;
    case 6:
        return CRGB(255, 0, 0);
        break;
    case 7:
        return CRGB(255, 100, 0);
        break;
    default:
        break;
    }

}

void showLED() {
    float theta = tick * 0.04;
    float end = 1 * 2 * PI;
    if (theta > end) {
        theta = 0;
    }
    float brightness = 5 * MAX_BRIGHTNESS * (1 + cos(theta - PI)) * 0.5;
    if (brightness > MAX_BRIGHTNESS) brightness = MAX_BRIGHTNESS;
    int length = sizeof(state_list) / sizeof(state_list[0]);
    int color_num = 0;
    for (int i = 0; i < length; i++) {
        if (state_list[i] != -2) {
            color_num++;
        }
    }
    if (color_num == 0) return;
    FastLED.setBrightness(brightness);

    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = getColorFromState(state_list[(int)(theta * color_num / end)]);
    }
    FastLED.show();
    tick++;
}

void showState() {
    Serial.print("{");
    int length = sizeof(state_list) / sizeof(state_list[0]);
    for (int i = 0; i < length; i++) {
        Serial.print(state_list[i]);
        if (i != length - 1) {
            Serial.print(",");
        }
    }
    Serial.println("}");
}

void setState(String reaction) {
    int idx = 0;
    for (int i = 0; i < (sizeof(reaction_list) / sizeof(reaction_list[0])); i++) {
        for (int j = 0; j < (sizeof(reaction_list[i]) / sizeof(reaction_list[i][0])); j++) {
            if (reaction_list[i][j] == reaction) {
                state_list[idx] = i;
                tick = 0;
                idx++;
                break;
            }
        }
    }
    for (int i = idx; i < (sizeof(reaction_list) / sizeof(reaction_list[0])); i++) {
        state_list[i] = -2;
    }
    if (idx == 0) {
        state_list[0] = -1;
    }
    tick = 0;
    showState();
}

void setup() {
    Serial.begin(115200);

    for (int i = 0; i < (sizeof(state_list) / sizeof(state_list[0])); i++) {
        state_list[i] = -2;
    }

    wifiConfig();

    FastLED.addLeds<WS2812B, 27, GRB>(leds, NUM_LEDS);

    JsonObject response_json;
    SlackApiClient::getWssLink(&response_json);
    String wssLink = response_json["url"].as<String>();
    Serial.println(wssLink);

    client.onMessage(onMessageCallback);

    client.onEvent(onEventsCallback);

    client.setInsecure();
    bool result = client.connect(wssLink);
}

void loop()
{
    client.poll();
    showLED();
    delay(20);
}