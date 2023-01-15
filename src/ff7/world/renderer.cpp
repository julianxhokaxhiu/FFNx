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

#include "renderer.h"

#include "../../renderer.h"

namespace ff7::world {

    // This draw call is the first UI call that marks the start of the first UI draw section
    void wm0_draw_minimap_quad_graphics_object(ff7_graphics_object* quad_graphics_object, ff7_game_obj* game_object) {
        newRenderer.setTimeFilterEnabled(false);
        ff7_externals.engine_draw_graphics_object(quad_graphics_object, game_object);
    }

    // This draw call is the first call related to world effects. It marks the end of the first UI draw section
    void wm0_draw_world_effects_1_graphics_object(ff7_graphics_object* world_effects_1_graphics_object, ff7_game_obj* game_object) {
        newRenderer.setTimeFilterEnabled(true);
        ff7_externals.engine_draw_graphics_object(world_effects_1_graphics_object, game_object);
    }

    // This draw call is the UI call that marks the second UI draw section
    void wm0_draw_minimap_points_graphics_object(ff7_graphics_object* minimap_points_graphics_object, ff7_game_obj* game_object) {
        newRenderer.setTimeFilterEnabled(false);
        ff7_externals.engine_draw_graphics_object(minimap_points_graphics_object, game_object);
    }

}
