/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
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

#include "../../external_mesh.h"

namespace ff7::world {

    void init_load_wm_bot_blocks();
    void destroy_graphics_objects();

    void wm0_overworld_draw_all();
    void wm0_overworld_draw_clouds();
    void wm0_overworld_draw_meteor();
    void wm2_underwater_draw_all();
    void wm3_snowstorm_draw_all();

    void wm0_draw_minimap_quad_graphics_object(ff7_graphics_object* quad_graphics_object, ff7_game_obj* game_object);
    void wm0_draw_world_effects_1_graphics_object(ff7_graphics_object* world_effects_1_graphics_object, ff7_game_obj* game_object);
    void wm0_draw_minimap_points_graphics_object(ff7_graphics_object* minimap_points_graphics_object, ff7_game_obj* game_object);

    int get_camera_rotation_z();

    void world_copy_position(vector4<int> *a1);
    void world_draw_effects();
    void animate_world_snake();
    int world_sub_762F9A(int a1, int arg4);

    void draw_shadow(ff7_graphics_object *, ff7_game_obj *);

    void engine_apply_4x4_matrix_product_between_matrices(matrix *a1, matrix *a2, matrix *a3);

    class Renderer
    {
    public:
        void loadWorldMapExternalMesh();
        bool drawWorldMapExternalMesh();
        void loadCloudsExternalMesh();
        void loadMeteorExternalMesh();
        void unloadExternalMeshes();
        bool drawCloudsExternalMesh();
        bool drawMeteorExternalMesh();
        bool drawCloudsAndMeteorExternalMesh(bool isDrawMeteor);

    private:
        ExternalMesh externalWorldMapModel;
        ExternalMesh externalSnakeModel;
        ExternalMesh externalCloudsModel;
        ExternalMesh externalMeteorModel;
    };

    Renderer worldRenderer;
}