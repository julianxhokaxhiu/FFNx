/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2022 Julian Xhokaxhiu                                   //
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

#include "../../ff7.h"

namespace ff7::battle
{
    const std::unordered_map<byte, int> numArgsOpCode = {
        {0x8E, 0},
        {0x8F, 0},
        {0x90, 3},
        {0x91, 1}, // Function added
        {0x92, 0},
        {0x93, 0}, // Function added
        {0x94, 5}, // Function added
        {0x95, 0},
        {0x96, 2}, // effect60 fn added (barret gun)
        {0x97, 2}, // Run enemy death animations
        {0x98, 1}, // Display action text
        {0x99, 6},
        {0x9A, 4},
        {0x9B, 0},
        {0x9C, 0},
        {0x9D, 1}, // Dispatches Tifa limit breaks
        {0x9E, 0xFF},
        {0x9F, 0},
        {0xA0, 1},
        {0xA1, 2},
        {0xA2, 1},
        {0xA3, 1},
        {0xA4, 0}, // Spell aura related
        {0xA5, 0}, // Spell aura related
        {0xA6, 0},
        {0xA7, 1},
        {0xA8, 2}, // Move actor to resting position
        {0xA9, 2},
        {0xAA, 0},
        {0xAB, 4},
        {0xAC, 1}, // Vincent related
        {0xAD, 5}, // Barret related machine gun effect
        {0xAE, 0}, // Resting position related
        {0xAF, 1}, // Resting position reset
        {0xB0, 0}, // Resting position related
        {0xB1, 0}, // Resting position related
        {0xB2, 0}, // nop
        {0xB3, 0xFF},
        {0xB4, 0}, // Y rotation
        {0xB5, 11},
        {0xB6, 1}, // running animation related
        {0xB7, 0}, // death effects
        {0xB8, 0},
        {0xB9, 1}, // setup animation camera data
        {0xBA, 2}, // resting Y rotation
        {0xBC, 1}, // Idle camera index
        {0xBD, 4}, // rotate to target animation
        {0xBE, 1}, // not Tifa stuff
        {0xBF, 2},
        {0xC1, 0xFF},
        {0xC2, 1}, // display damage
        {0xC3, 0}, // some effects
        {0xC4, 3}, // resting Y rotation (inverting direction)
        {0xC5, 0}, // set frames to wait to 0xBFD0F0
        {0xC6, 1}, // set 0xBFD0F0 frames to wait
        {0xC7, 3}, // enemy animation thing
        {0xC8, 5}, // effects thing
        {0xC9, 0}, // nop
        {0xCA, 0xFF},
        {0xCB, 8},
        {0xCC, 1}, // move effects thing
        {0xCE, 1},
        {0xCF, 8}, // 3d move effects
        {0xD0, 3}, // move effects
        {0xD1, 5}, // move effects
        {0xD2, 0},
        {0xD3, 0},
        {0xD4, 3}, // move effects
        {0xD5, 8}, // move effects
        {0xD6, 1}, // effects thing
        {0xD7, 2}, // effects thing
        {0xD8, 3}, // effects thing
        {0xDA, 1},
        {0xDB, 4}, // effect machine gun
        {0xDC, 3}, // rotation stuff
        {0xDD, 2}, // machine gun effects
        {0xDE, 2}, // machine gun stuff
        {0xDF, 0}, // resting Y rotation
        {0xE0, 0}, // spell aura related
        {0xE1, 0}, // appear model
        {0xE2, 0}, // vanish model
        {0xE3, 0}, // position actor
        {0xE4, 0}, // resting stuff
        {0xE5, 0}, // rotation stuff
        {0xE6, 0}, // spell aura stuff
        {0xE7, 1},
        {0xE8, 0},
        {0xE9, 3}, // move effects
        {0xEA, 0}, // display action string effect
        {0xEB, 0xFF},
        {0xEC, 0xFF},
        {0xED, 0}, // resting stuff
        {0xEE, 0xFF},
        {0xF0, 0}, // foot dust effect
        {0xF1, -1},
        {0xF2, 0}, // nop
        {0xF3, 0xFF},
        {0xF4, 0xFF},
        {0xF5, 1}, // game init enemies
        {0xF6, 0}, // Run normal enemy death animation effects
        {0xF7, 1}, // delay damage display effect
        {0xF8, 1}, // effect stuff
        {0xF9, 0}, // resets actor orientation
        {0xFA, 0},
        {0xFB, 4},
        {0xFC, 0}, // setting orientation for target-all action
        {0xFD, 6}, // set resting position
        {0xFE, 0xFF},
        {0xFF, 0xFF},
    };

    const std::unordered_set<byte> endingOpCode{{0xA2, 0xA7, 0xA9, 0xB6, 0xF1}};
}
