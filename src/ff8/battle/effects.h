/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2024 myst6re                                            //
//    Copyright (C) 2024 Julian Xhokaxhiu                                   //
//    Copyright (C) 2023 Cosmos                                             //
//    Copyright (C) 2023 Tang-Tang Zhou                                     //
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

namespace FF8BattleEffect {
    // See https://github.com/DarkShinryu/doomtrain/blob/master/Doomtrain/Resources/Magic_ID_List.txt
    enum Effect {
        Cure = 0,
        Leviathan = 5,
        Tonberry = 89,
        Siren = 94,
        Minimog = 95,
        BokoChocofire = 96,
        BokoChocoflare = 97,
        BokoChocometeor = 98,
        BokoChocobocle = 99,
        Quezacotl = 115,
        Phoenix = 139,
        Ultima = 148,
        Shiva = 184,
        Odin = 186,
        Doomtrain = 190,
        Cactuar = 198,
        Ifrit = 200,
        Bahamut = 201,
        Cerberus = 202,
        Alexander = 203,
        Brothers = 204,
        Eden = 205,
        Apocalypse = 220,
        Meteor = 222,
        Carbuncle = 277,
        Pandemona = 290,
        Diablos = 324,
        GilgameshZantetsukenReverse = 325,
        GilgameshZantetsuken = 326,
        GilgameshMasamune = 327,
        GilgameshExcaliber = 328,
        GilgameshExcalipoor = 329,
        Moomba = 337,
    };
}

namespace FF8BattleEffectOpcode {
    enum Opcode {
        UploadTexture39 = 39,
        UploadPalette75 = 75
    };
}
