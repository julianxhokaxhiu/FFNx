/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2023 Julian Xhokaxhiu                                   //
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

namespace ff7::battle
{
    // Camera
    void camera_hook_init();
    void update_battle_camera(short cameraScriptIndex);
    void update_idle_battle_camera();

    // Animations
    void animations_hook_init();

    // Battle
    void magic_thread_start(void (*func)());
    void load_battle_stage(int param_1, int battle_location_id, int **param_3);
    void battle_sub_5C7F94(int param_1, int param_2);
    void display_battle_action_text_sub_6D71FA(short command_id, short action_id);

    // Menu
    void battle_menu_enter();
    void draw_ui_graphics_objects_wrapper(int flag, int type);
}