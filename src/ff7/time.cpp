/****************************************************************************/
//    Copyright (C) 2023 Cosmos                                             //
//                                                                          //
//    This file is part of FFNx                                             //
//                                                                          //
//    FFNx is free software: you can redistribute it and/or modify          //
//    it under the terms of the GNU General Public License as published by  //
//    the Free Software Foundation, either version 3 of the License         //
//                                                                          //
//    FFNx is distributed in the hope that it will be useful,               //
//    but WITHOUT ANY WARRANTY; without even the implied warranty of        //
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         //
//    GNU General Public License for more details.                          //
/****************************************************************************/

#include "time.h"
#include "cfg.h"
#include "field/background.h"

#include "../patch.h"
#include "../ff7.h"
#include "../renderer.h"
#include "../lighting.h"

namespace ff7
{
    Time time;

    void Time::init()
    {
        loadConfig();
        initParamsFromConfig();
    }

    void Time::loadConfig()
    {
        char _fullpath[MAX_PATH];
        sprintf(_fullpath, "%s/%s/config.toml", basedir, external_time_cycle_path.c_str());

        try
        {
            config = toml::parse_file(_fullpath);
        }
        catch (const toml::parse_error &err)
        {
            config = toml::parse("");
        }
    }

    void Time::initParamsFromConfig()
    {
        toml::array* morningColorArray = config["morning_color"].as_array();
        if(morningColorArray != nullptr && morningColorArray->size() == 3)
        {
            morningColor.x = morningColorArray->get(0)->value<float>().value_or(1.0);
            morningColor.y = morningColorArray->get(1)->value<float>().value_or(1.0);
            morningColor.z  = morningColorArray->get(2)->value<float>().value_or(1.0);
        }
        toml::array* middayColorArray = config["midday_color"].as_array();
        if(middayColorArray != nullptr && middayColorArray->size() == 3)
        {
            middayColor.x = middayColorArray->get(0)->value<float>().value_or(1.0);
            middayColor.y = middayColorArray->get(1)->value<float>().value_or(1.0);
            middayColor.z  = middayColorArray->get(2)->value<float>().value_or(1.0);
        }
        toml::array* afternoonColorArray = config["afternoon_color"].as_array();
        if(afternoonColorArray != nullptr && afternoonColorArray->size() == 3)
        {
            afternoonColor.x = afternoonColorArray->get(0)->value<float>().value_or(1.0);
            afternoonColor.y = afternoonColorArray->get(1)->value<float>().value_or(1.0);
            afternoonColor.z  = afternoonColorArray->get(2)->value<float>().value_or(1.0);
        }
        toml::array* nightColorArray = config["night_color"].as_array();
        if(nightColorArray != nullptr && nightColorArray->size() == 3)
        {
            nightColor.x = nightColorArray->get(0)->value<float>().value_or(1.0);
            nightColor.y = nightColorArray->get(1)->value<float>().value_or(1.0);
            nightColor.z  = nightColorArray->get(2)->value<float>().value_or(1.0);
        }

        sunriseTime = config["sunrise_time"].value_or(6.0) / 24.0;
        morningTime = config["morning_time"].value_or(7.0) / 24.0;
        middayTime = config["midday_time"].value_or(15.0) / 24.0;
        afternoonTime = config["afternoon_time"].value_or(19.0) / 24.0;
        nightTime = config["night_time"].value_or(20.0) / 24.0;

        framesPerMinute = config["frames_per_minute"].value_or(15);

        auto optionAddressStr = config["options_address"].value<std::string>();
        if(optionAddressStr.has_value())
        {
            auto str = optionAddressStr.value();
            pOptions = (byte*)(std::strtol(str.data(), nullptr, 16));
        }

        auto minutesAddressStr = config["minutes_address"].value<std::string>();
        if(minutesAddressStr.has_value())
        {
            auto str = minutesAddressStr.value();
            pMinutes = (byte*)(std::strtol(str.data(), nullptr, 16));
        }

        auto hoursAddressStr = config["hours_address"].value<std::string>();
        if(hoursAddressStr.has_value())
        {
            auto str = hoursAddressStr.value();
            pHours = (byte*)(std::strtol(str.data(), nullptr, 16));
        }

        auto daysAddressStr = config["days_address"].value<std::string>();
        if(daysAddressStr.has_value())
        {
            auto str = daysAddressStr.value();
            pDays = (byte*)(std::strtol(str.data(), nullptr, 16));
        }

        auto monthsAddressStr = config["months_address"].value<std::string>();
        if(monthsAddressStr.has_value())
        {
            auto str = monthsAddressStr.value();
            pMonths = (byte*)(std::strtol(str.data(), nullptr, 16));
        }

        auto monthChar0AddressStr = config["month_char_0_address"].value<std::string>();
        if(monthChar0AddressStr.has_value())
        {
            auto str = monthChar0AddressStr.value();
            pMonthChar0 = (byte*)(std::strtol(str.data(), nullptr, 16));
        }

        auto monthChar1AddressStr = config["month_char_1_address"].value<std::string>();
        if(monthChar1AddressStr.has_value())
        {
            auto str = monthChar1AddressStr.value();
            pMonthChar1 = (byte*)(std::strtol(str.data(), nullptr, 16));
        }

        auto monthChar2AddressStr = config["month_char_2_address"].value<std::string>();
        if(monthChar2AddressStr.has_value())
        {
            auto str = monthChar2AddressStr.value();
            pMonthChar2 = (byte*)(std::strtol(str.data(), nullptr, 16));
        }

        for (int i = 0; i < 12; ++i)
        {
            monthChar0[i] = config["month_" + std::to_string(i) + "_char_0"].value_or(0);
            monthChar1[i] = config["month_" + std::to_string(i) + "_char_1"].value_or(0);
            monthChar2[i] = config["month_" + std::to_string(i) + "_char_2"].value_or(0);
        }
    }

    void time_hook_init()
    {
        replace_call_function(ff7_externals.field_draw_everything + 0x360, ff7::field::draw_gray_quads_sub_644E90);
    }

    bx::Vec3 Time::mixColor(float time, float timeMin, float timeMax, bx::Vec3 color0, bx::Vec3 color1)
    {
        bx::Vec3 color = bx::Vec3(0.0, 0.0, 0.0);
        float t = (time - timeMin) / (timeMax - timeMin);
        color.x = std::lerp(color0.x, color1.x, t);
        color.y = std::lerp(color0.y, color1.y, t);
        color.z = std::lerp(color0.z, color1.z, t);
        return color;
    }

    void Time::update()
    {
        newRenderer.setTimeEnabled(false);
        newRenderer.setTimeFilterEnabled(false);

        if(pOptions == nullptr || pMinutes == nullptr || pHours == nullptr || pDays == nullptr || pMonths == nullptr)
        {
            return;
        }

        // Early return if bit 0 of options variable is not set (global disable)
        if((*pOptions & (1 << 0)) == 0) return;

        struct game_mode* mode = getmode_cached();
        if(mode->driver_mode != MODE_FIELD &&
           mode->driver_mode != MODE_BATTLE &&
           mode->driver_mode != MODE_WORLDMAP)
        {
            return;
        }

        // Progress time if bit 1 of options variable is set
        if(*pOptions & (1 << 1)) frame_count++;

        int modeFramesPerMinute = framesPerMinute;
        if(mode->driver_mode == MODE_BATTLE)
            modeFramesPerMinute *= battle_frame_multiplier;
        else
            modeFramesPerMinute *= 2 * common_frame_multiplier;

        if(frame_count >= modeFramesPerMinute)
        {
            (*pMinutes) = (*pMinutes) + 1;
            frame_count = 0;
        }
        if(*pMinutes >= 60 )
        {
            (*pHours) = (*pHours) + 1;
            *pMinutes = 0;
            frame_count = 0;
        }
        if(*pHours >= 24)
        {
            (*pDays) = (*pDays) + 1;
            *pHours = 0;
        }
        if((*pDays) >= 32)
        {
            (*pMonths) = (*pMonths) + 1;
            *pDays = 1;
        }
        if((*pMonths) >= 12)
        {
            (*pMonths) = 0;
        }

        if(pMonthChar0 != nullptr && pMonthChar1 != nullptr && pMonthChar2 != nullptr)
        {
            *pMonthChar0 = monthChar0[*pMonths];
            *pMonthChar1 = monthChar1[*pMonths];
            *pMonthChar2 = monthChar2[*pMonths];
        }

        if(mode->driver_mode == MODE_FIELD ||
           mode->driver_mode == MODE_BATTLE)
        {
            // Disable filter if bit 2 of options variable is not set (e.g. when indoor fields)
            if((*pOptions & (1 << 2)) == 0)
            {
                newRenderer.setTimeEnabled(false);
                newRenderer.setTimeFilterEnabled(false);
                return;
            }
        }

        float time = (*pHours * 60.0f * modeFramesPerMinute + *pMinutes * modeFramesPerMinute + frame_count);
        time /= (1440.0f * modeFramesPerMinute);

        bx::Vec3 color = bx::Vec3(0.0, 0.0, 0.0);
        if(time < sunriseTime) color = nightColor;
        else if(time < morningTime) color = mixColor(time, sunriseTime, morningTime, nightColor, morningColor);
        else if(time < middayTime) color = mixColor(time, morningTime, middayTime, morningColor, middayColor);
        else if(time < afternoonTime) color = mixColor(time, middayTime, afternoonTime, middayColor, afternoonColor);
        else if(time < nightTime) color = mixColor(time, afternoonTime, nightTime, afternoonColor, nightColor);
        else color = nightColor;

        newRenderer.setTimeEnabled(true);
        newRenderer.setTimeFilterEnabled(true);
        newRenderer.setTimeColor(color);
    }
}