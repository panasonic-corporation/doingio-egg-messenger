#include "Config.h"
#include "SlackApiClient.h"

void SlackApiClient::getWssLink(JsonObject *response_json) {
    WiFiClientSecure client;
    HTTPClient http_client;
    http_client.setReuse(false);
    http_client.begin(client, "slack.com", 443, "/api/apps.connections.open", true);
    http_client.addHeader("Authorization", String("Bearer ") + SLACK_TOKEN);
    http_client.addHeader("Content-Type", "application/x-www-form-urlencoded");
    // リクエスト
    int http_code = http_client.POST("");
    if (http_code == 200) {
        // response取得
        String json = http_client.getString();
        DynamicJsonDocument json_buffer(BUFFER_SIZE);
        auto error = deserializeJson(json_buffer, json);
        if (error) {
            Serial.print("Failed to deserializeJson()");
            Serial.println(error.c_str());
            http_client.end();
        } else {
            *response_json = json_buffer.as<JsonObject>();
            http_client.end();
        }
    } else {
        // エラーレスポンス
        Serial.println("Error on http request");
        http_client.end();
    }
}

