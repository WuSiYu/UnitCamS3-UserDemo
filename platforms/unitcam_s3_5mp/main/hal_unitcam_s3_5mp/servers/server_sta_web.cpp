/**
 * @file server_sta_web.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-08-06
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "../hal_unitcam_s3_5mp.h"
#include "../hal_config.h"
#include <mooncake.h>
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <LittleFS.h>
#include "apis/apis.h"
#include <assets/assets.h>

static AsyncWebServer* _sta_server = nullptr;

static constexpr uint32_t AP_WAIT_TIMEOUT_MS = 10000;
static constexpr uint32_t WIFI_CONNECT_TIMEOUT_MS = 30000;

void HAL_UnitCamS3_5MP::startStaWebServer()
{
    spdlog::info("start sta web server");

    // Start AP for AP_WAIT_TIMEOUT_MS seconds to allow reconfiguration
    spdlog::info("start ap for 10s");
    startApServer();

    uint32_t ap_wait_time_count = millis();
    bool led_state = true;
    while (millis() - ap_wait_time_count < AP_WAIT_TIMEOUT_MS)
    {
        // If get client
        if (WiFi.softAPgetStationNum() != 0)
        {
            // Hold AP mode
            spdlog::info("get client, hold ap mode");
            HAL::SetLedState(true);
            while (WiFi.softAPgetStationNum() != 0)
                delay(2000);

            // Reset counting
            ap_wait_time_count = millis();
        }

        delay(500);
        led_state = !led_state;
        HAL::SetLedState(led_state);
    }

    spdlog::info("no client, switch to sta mode");
    stopApServer();

    // Try connect wifi
    spdlog::info("try connect:\n{}\n{}", HAL::GetSystemConfig().wifiSsid, HAL::GetSystemConfig().wifiPass);
    WiFi.mode(WIFI_STA);
    WiFi.begin(HAL::GetSystemConfig().wifiSsid.c_str(), HAL::GetSystemConfig().wifiPass.c_str());

    {
        gpio_reset_pin(GPIO_NUM_14);
        pinMode(14, OUTPUT);
        ledcAttach(14, 12000, 8);
        uint32_t time_count = millis();
        bool is_connected = false;
        while (1)
        {
            for (int i = 0; i < 255; i++)
            {
                ledcWrite(14, i);
                delay(5);
            }

            for (int i = 255; i > 0; i--)
            {
                ledcWrite(14, i);
                delay(5);

                // Connected
                if (WiFi.status() == WL_CONNECTED)
                {
                    spdlog::info("connected");
                    is_connected = true;
                    break;
                }

                // Time out
                if (millis() - time_count > WIFI_CONNECT_TIMEOUT_MS)
                {
                    spdlog::error("connect failed, reboot..");

                    // Reboot
                    delay(300);
                    esp_restart();
                    delay(10000);
                }
            }

            if (is_connected)
                break;
        }
        ledcDetach(14);

        gpio_reset_pin((gpio_num_t)HAL_PIN_LED);
        gpio_set_direction((gpio_num_t)HAL_PIN_LED, GPIO_MODE_OUTPUT);
        gpio_set_pull_mode((gpio_num_t)HAL_PIN_LED, GPIO_PULLUP_ONLY);
    }
    spdlog::info("connect ok, ip: {}", WiFi.localIP().toString().c_str());

    // Create web server in STA mode
    _sta_server = new AsyncWebServer(80);

    // Load page
    _sta_server->on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
        AsyncWebServerResponse* response = request->beginResponse_P(
            200, "text/html", AssetPool::GetImage().index_html_gz, sizeof(AssetPool::GetImage().index_html_gz));
        response->addHeader("Content-Encoding", "gzip");
        request->send(response);
    });

    // Load apis
    load_cam_apis(*_sta_server);
    load_mic_apis(*_sta_server);
    load_system_apis(*_sta_server);
    load_poster_apis(*_sta_server);

    // Start web server
    spdlog::info("start web server on sta ip: {}", WiFi.localIP().toString().c_str());
    _sta_server->begin();

    // Light up
    HAL::SetLedState(true);
}
