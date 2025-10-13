/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include <stdio.h>
#include <mooncake_log.h>
#include <M5Unified.hpp>
#include <mooncake.h>
#include <assets.h>
#include <vector>
#include <apps.h>
#include <hal.h>

using namespace mooncake;

void draw_firmware_version()
{
    GetHAL().display.setEpdMode(epd_mode_t::epd_quality);
    GetHAL().display.loadFont(font_montserrat_medium_36);
    GetHAL().display.setTextDatum(middle_center);
    GetHAL().display.setTextColor(TFT_BLACK);
    GetHAL().display.drawString("FactoryTest: V0.4", GetHAL().display.width() / 2, GetHAL().display.height() / 2);
}

void draw_gray_scale_bars()
{
    GetHAL().display.setEpdMode(epd_mode_t::epd_quality);
    std::vector<uint32_t> colors = {0xffffff, 0xeeeeee, 0xdddddd, 0xcccccc, 0xbbbbbb, 0xaaaaaa, 0x999999, 0x888888,
                                    0x777777, 0x666666, 0x555555, 0x444444, 0x333333, 0x222222, 0x111111, 0x000000};
    GetHAL().display.fillScreen(TFT_WHITE);
    for (int i = 0; i < 16; i++) {
        GetHAL().display.fillRect(i * 60, 0, 60, 540, colors[i]);
    }
}

void check_full_display_refresh_request(bool force = false)
{
    static uint32_t last_full_refresh_time = GetHAL().millis();

    // Refresh full display every 15 seconds
    if (GetHAL().millis() - last_full_refresh_time > 15000 || force) {
        GetHAL().display.setEpdMode(epd_mode_t::epd_quality);
        GetHAL().display.drawPng(img_bg_start, img_bg_end - img_bg_start, 0, 0);

        // Notice apps to refresh
        GetHAL().requestRefresh();

        last_full_refresh_time = GetHAL().millis();
    }
}

extern "C" void app_main(void)
{
    GetHAL().init();

    draw_firmware_version();
    GetHAL().delay(2000);

    draw_gray_scale_bars();
    GetHAL().delay(3000);

    // Install apps
    GetMooncake().installApp(std::make_unique<AppPower>());
    GetMooncake().installApp(std::make_unique<AppSdCard>());
    GetMooncake().installApp(std::make_unique<AppRtc>());
    GetMooncake().installApp(std::make_unique<AppBuzzer>());
    GetMooncake().installApp(std::make_unique<AppImu>());
    GetMooncake().installApp(std::make_unique<AppWifi>());

    // Trigger startup refresh
    check_full_display_refresh_request(true);

    while (1) {
        M5.update();
        check_full_display_refresh_request();
        GetMooncake().update();
        GetHAL().clearRefreshRequest();
        GetHAL().feedTheDog();
    }
}
