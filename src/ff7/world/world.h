/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2024 Julian Xhokaxhiu                                   //
//    Copyright (C) 2022 Cosmos                                             //
//    Copyright (C) 2022 Tang-Tang Zhou                                     //
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

#include "matrix.h"

namespace ff7::world
{
    enum input_key
    {
        L2 = 1,
        R2 = 2,
        L1 = 4,
        R1 = 8,
        TRIANGLE = 16,
        CIRCLE = 32,
        CROSS = 64,
        SQUARE = 128,
        SELECT = 256,
        UNK1 = 512,
        UNK2 = 1024,
        START = 2048,
        UP = 4096,
        RIGHT = 8192,
        DOWN = 16384,
        LEFT = 32768,
        ANY_DIRECTIONAL_KEY = UP | RIGHT | DOWN | LEFT
    };

    enum map_type {
        OVERWORLD = 0,
        UNDERWATER = 2,
        SNOWSTORM = 3
    };

    enum model_type {
        CLOUD = 0,
        TIFA,
        CID,
        HIGHWIND,
        WILD_CHOCOBO,
        TINY_BRONCO ,
        BUGGY,
        JUNON_CANNON,
        CARGO_SHIP,
        HIGHWIND_PROPELLERS,
        DIAMOND_WEAPON,
        ULTIMATE_WEAPON,
        FORT_CONDOR,
        SUBMARINE,
        GOLD_SAUCER,
        ROCKET_TOWN_ROCKET,
        ROCKET_TOWN_LAUNCH_PAD,
        SUNKEN_GELNIKA,
        UNDERWATER_REACTOR,
        CHOCOBO,
        MIDGAR_CANNON,
        MODEL_UNK1,
        MODEL_UNK2,
        MODEL_UNK3,
        NORTH_CRATER_BARRIER,
        ANCIENT_FOREST,
        KEY_ANCIENTS,
        MODEL_UNK4,
        RED_SUBMARINE,
        RUBY_WEAPON,
        EMERALD_WEAPON
    };

    constexpr int INVALID_DIRECTION = 9999;

    class World
    {
    public:
        const vector3<float>& GetJoystickDirection();
        void SetJoystickDirection(const vector3<float>& dir);

        float GetRightTrigger();
        void SetRightTrigger(float value);

        float GetLeftTrigger();
        void SetLeftTrigger(float value);

    private:
        vector3<float> joyDir = {0.0f, 0.0f, 0.0f};
        float rightTrigger = 0.0f;
        float leftTrigger = 0.0f;
    };

    inline const vector3<float>& World::GetJoystickDirection()
    {
        return joyDir;
    }

    inline void World::SetJoystickDirection(const vector3<float>& dir)
    {
        joyDir = dir;
    }

    inline float World::GetRightTrigger()
    {
        return rightTrigger;
    }

    inline void World::SetRightTrigger(float value)
    {
        rightTrigger = value;
    }

    inline float World::GetLeftTrigger()
    {
        return leftTrigger;
    }

    inline void World::SetLeftTrigger(float value)
    {
        leftTrigger = value;
    }

    extern World world;

    int get_player_movement_speed(int model_id);
}
