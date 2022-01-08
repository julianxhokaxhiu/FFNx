/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2021 Julian Xhokaxhiu                                   //
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

#include "../ff7.h"
#include "../patch.h"

constexpr int world_60fps_ratio = 2;

int ff7_pop_world_stack_multiply_wrapper()
{
    int ret = ff7_externals.pop_world_script_stack();
    return ret * world_60fps_ratio;
}

int ff7_pop_world_stack_divide_wrapper()
{
    int ret = ff7_externals.pop_world_script_stack();
    return ret / world_60fps_ratio;
}

void ff7_world_hook_init()
{
    // Movement related fix
    patch_divide_code<DWORD>(ff7_externals.world_init_variables_74E1E9 + 0x15D, world_60fps_ratio);
    replace_call_function(ff7_externals.run_world_event_scripts_system_operations + 0x172, ff7_pop_world_stack_divide_wrapper);
    replace_call_function(ff7_externals.run_world_event_scripts_system_operations + 0x584, ff7_pop_world_stack_divide_wrapper);

    // Camera related fix
    replace_call_function(ff7_externals.run_world_event_scripts_system_operations + 0x7DC, ff7_pop_world_stack_divide_wrapper);

    // Others
    replace_call_function(ff7_externals.run_world_event_scripts_system_operations + 0x8DF, ff7_pop_world_stack_multiply_wrapper);
}