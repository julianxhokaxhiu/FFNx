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

#pragma once

#include "globals.h"
#include "bx/math.h"

namespace ff7
{
    void time_hook_init();

    class Time
    {
        public:
            void init();
            void update();

        private:
            void loadConfig();
            void initParamsFromConfig();
            bx::Vec3 mixColor(float time, float timeMin, float timeMax, bx::Vec3 color0, bx::Vec3 color1);

        private:
            // Config
            toml::parse_result config;

            bx::Vec3 morningColor  = bx::Vec3(1.0, 1.0, 1.0);
            bx::Vec3 middayColor  = bx::Vec3(1.0, 1.0, 1.0);
            bx::Vec3 afternoonColor  = bx::Vec3(1.0, 1.0, 1.0);
            bx::Vec3 nightColor  = bx::Vec3(1.0, 1.0, 1.0);

            float sunriseTime = 1.0;
            float morningTime = 1.0;
            float middayTime = 1.0;
            float afternoonTime = 1.0;
            float nightTime = 1.0;

            int frame_count = 0;

            int framesPerMinute = 0;

            byte* pOptions = nullptr;
            byte* pMinutes = nullptr;
            byte* pHours = nullptr;
            byte* pDays = nullptr;
            byte* pMonths = nullptr;

            byte* pMonthChar0 = nullptr;
            byte* pMonthChar1 = nullptr;
            byte* pMonthChar2 = nullptr;

            byte monthChar0[12];
            byte monthChar1[12];
            byte monthChar2[12];
    };

    extern Time time;
}