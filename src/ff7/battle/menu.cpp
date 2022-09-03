/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2022 Julian Xhokaxhiu                                   //
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

#include "../../globals.h"

#include "menu.h"

namespace ff7::battle
{
    void update_battle_menu()
    {
        ((void(*)())ff7_externals.battle_menu_update_6CE8B3)();
        if(*ff7_externals.g_do_render_menu)
            (*ff7_externals.battle_menu_animation_idx)++;
    }

    void display_tifa_slots_handler()
    {
        if(*ff7_externals.g_do_render_menu)
            ff7_externals.display_tifa_slots_handler_6E3135();
    }

    void display_cait_sith_slots_handler()
    {
        if(*ff7_externals.g_do_render_menu)
            ff7_externals.display_cait_sith_slots_handler_6E2170();
    }

    void display_battle_arena_menu_handler()
    {
        if(*ff7_externals.g_do_render_menu)
            ff7_externals.display_battle_arena_menu_handler_6E384F();
    }

    void delay_battle_target_pointer_animation_type()
    {
        if(frame_counter % battle_frame_multiplier == 0)
        {
            (*ff7_externals.targeting_actor_id_DC3C98)++;
        }
    }
}