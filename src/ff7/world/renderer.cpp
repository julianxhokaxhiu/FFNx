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

#include "renderer.h"
#include "camera.h"
#include "cfg.h"
#include "gl.h"

#include "../defs.h"
#include "../../lighting.h"

namespace ff7::world {

    void init_load_wm_bot_blocks() {
        ff7_externals.world_init_load_wm_bot_block_7533AF();

        worldRenderer.loadWorldMapExternalMesh();
        worldRenderer.loadCloudsExternalMesh();
        worldRenderer.loadMeteorExternalMesh();
    }

    void destroy_graphics_objects() {
        ff7_externals.world_exit_destroy_graphics_objects_75A921();
        worldRenderer.unloadExternalMeshes();
    }

    void wm0_overworld_draw_all() {
       auto game_object = (struct ff7_game_obj *)common_externals.get_game_object();               
        if (game_object) update_view_matrix (game_object);
        worldRenderer.drawWorldMapExternalMesh();    
        ff7_externals.world_wm0_overworld_draw_all_74C179();
    }

    void wm0_overworld_draw_clouds()
    {
        worldRenderer.drawCloudsAndMeteorExternalMesh(*ff7_externals.is_meteor_flag_on_E2AAE4);
    }

    void wm0_overworld_draw_meteor()
    {
    }

    void wm2_underwater_draw_all() {
        auto game_object = (struct ff7_game_obj *)common_externals.get_game_object();               
        if (game_object) update_view_matrix (game_object);
        		const int worldmap_type = *ff7_externals.world_map_type_E045E8;
		newRenderer.setFogEnabled(true);
        worldRenderer.drawWorldMapExternalMesh();
        ff7_externals.world_wm2_underwater_draw_all_74C3F0();
        newRenderer.setFogEnabled(false);
    }

    void wm3_snowstorm_draw_all() {
        auto game_object = (struct ff7_game_obj *)common_externals.get_game_object();               
        if (game_object) update_view_matrix (game_object);
        worldRenderer.drawWorldMapExternalMesh();
        ff7_externals.world_wm3_snowstorm_draw_all_74C589();
    }

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


    int get_camera_rotation_z()
    {
        return 0;
    }

    void world_copy_position(vector4<int> *a1)
    {
        if ( a1 )
            *a1 = *ff7_externals.world_player_pos_E04918;
    }

    vector3<float> calcSphericalWorldPos(vector3<float> worldPos)
    {
        struct matrix viewMatrix;
        ::memcpy(&viewMatrix.m[0][0], newRenderer.getViewMatrix(), sizeof(viewMatrix.m));

        vector3<float> viewPos = {0.0f, 0.0f, 0.0f};
        transform_point(&viewMatrix, &worldPos, &viewPos);    

        struct matrix invViewMatrix;
        ::memcpy(&invViewMatrix.m[0][0], newRenderer.getInvViewMatrix(), sizeof(invViewMatrix.m));

        float rp = -250000;

        vector2<float> planedir = { viewPos.x, viewPos.z};
        float planeDirLength = sqrtf(planedir.x * planedir.x + planedir.y * planedir.y);
        planedir.x /= planeDirLength;
        planedir.y /= planeDirLength;

        vector2<float> plane = {viewPos.y, sqrtf((viewPos.x) * (viewPos.x) + (viewPos.z) * (viewPos.z))};
        vector2<float> planeScaled = {plane.x / rp, plane.y / rp};
        vector2<float> planeScaledExp = {exp(planeScaled.x) * cos(planeScaled.y), exp(planeScaled.x) * sin(planeScaled.y)};
        vector2<float> circle = { rp * planeScaledExp.x - rp, rp * planeScaledExp.y};

        vector3<float> newViewPos = {circle.y * planedir.x, circle.x, circle.y * planedir.y};

        vector3<float> newWorldPos = {0.0f, 0.0f, 0.0f};
        transform_point(&invViewMatrix, &newViewPos, &newWorldPos);

        return newWorldPos;
    }

    void world_draw_effects()
    {
        short v0;
        transform_matrix rot_matrix_0; 
        transform_matrix rot_matrix_1;
        int v3;
        transform_matrix rot_matrix_2;
        int v5;
        int v6;
        vector3<short> delta_point;
        vector4<short> camera_direction;
        vector4<int> a1;

        camera_direction.x = 0;
        camera_direction.y = -(short)ff7_externals.get_world_camera_front_rot_74D298();
        camera_direction.z = 0;
        ff7_externals.engine_apply_rotation_to_rot_matrix_662AD8((vector3<short>*)&camera_direction, &rot_matrix_1);
        camera_direction.x = -ff7_externals.world_get_world_current_camera_rotation_x_74D3C6();
        ff7_externals.engine_apply_rotation_to_rot_matrix_662AD8((vector3<short>*)&camera_direction, &rot_matrix_2);
        ff7_externals.world_copy_position_75042B(&a1);

        world_effect_2d_list_node* eff_node = *ff7_externals.dword_E35648;
        world_effect_2d_list_node* next_node = nullptr;
        while (eff_node) 
        {
            vector3<float> worldPos = {static_cast<float>(eff_node->x), static_cast<float>(eff_node->y), static_cast<float>(eff_node->z)};
            vector3<float> newWorldPos = calcSphericalWorldPos(worldPos);

            vector3<float> refPos = {static_cast<float>(a1.x), static_cast<float>(*ff7_externals.world_camera_delta_y_DE6A04), static_cast<float>(a1.z)};

            v6 = newWorldPos.x - refPos.x;
            v5 = newWorldPos.y - refPos.y;
            v3 = newWorldPos.z - refPos.z;

            // Save the next node first because world_submit_draw_effects_75C283 might modify the eff_node variable since it mutates the ESI register
            next_node = eff_node->next;
            if ( v6 > -30000 && v6 < 30000 && v3 > -30000 && v3 < 30000 )
            {
                delta_point.x = v6;
                v0 = 0;
                delta_point.y = v5 - v0;
                delta_point.z = v3;
                if ( eff_node->apply_rotation_y )
                {
                    camera_direction.z = 0;
                    camera_direction.x = 0;
                    camera_direction.y = eff_node->rotation_y;
                    ff7_externals.engine_apply_rotation_to_transform_matrix_6628DE((vector3<short>*)&camera_direction, (rotation_matrix*)&rot_matrix_0);
                    ff7_externals.engine_set_game_engine_rot_matrix_663673((rotation_matrix*)&rot_matrix_0);
                }
                else
                {
                    ff7_externals.engine_set_game_engine_rot_matrix_663673((rotation_matrix*)&rot_matrix_2);
                }

                ff7_externals.world_submit_draw_effects_75C283(
                    &eff_node->texture_data,
                    (int)&ff7_externals.byte_96D6A8[12 * eff_node->unknown_idx],
                    &delta_point,
                    eff_node->apply_rotation_y);
            }
            eff_node = next_node;
        }
    }

    vector4<short>* previous_position_backup = nullptr;
    void animate_world_snake()
    {
        __int16 unknown;
        __int16 v1;
        __int16 v2;
        int delta_x;
        int delta_y;
        world_snake_graphics_data *snake_data;
        vector4<short>* previous_position;
        vector4<int> v7;

        ff7_externals.sub_74C9A5();
        ff7_externals.world_copy_position_75042B(&v7);
        previous_position = *ff7_externals.world_snake_data_position_ptr_E2A18C - 1;
        if ( previous_position < ff7_externals.world_snake_data_position_E29F80 )
            previous_position = &(ff7_externals.world_snake_data_position_E29F80[47]);

        
        for ( snake_data = ff7_externals.world_snake_graphics_data_E2A490;
                snake_data < ff7_externals.world_snake_graphics_data_end_E2A6D0;
                ++snake_data )
        {     
            previous_position += 4;
            if ( previous_position >= ff7_externals.snake_position_size_of_array_E2A100 )
            previous_position -= 48;            
            
            if ( ff7_externals.sub_753366(((int)previous_position->x >> 13) + 26, ((int)previous_position->y >> 13) + 16) )
            {
                vector3<float> prevPos = { static_cast<float>(previous_position->x + 212992), 
                                           static_cast<float>(0),
                                           static_cast<float>(previous_position->y + 0x20000)};
                vector3<float> newPrevWorldPos = calcSphericalWorldPos(prevPos);

                delta_x = newPrevWorldPos.x - v7.x;
                if ( delta_x >= -30000 && delta_x <= 30000 )
                {
                    snake_data->delta_x = delta_x;
                    delta_y = newPrevWorldPos.z - v7.z;
                    if ( delta_y >= -30000 && delta_y <= 30000 )
                    {
                        snake_data->delta_y = newPrevWorldPos.z - LOWORD(v7.z);
                        v2 = previous_position->z + 2048;
                        unknown = previous_position->w;
                        v1 = newPrevWorldPos.y;//sub_762F9A(snake_data->delta_x, snake_data->delta_y);
                        previous_position_backup = previous_position;
                        ff7_externals.world_draw_snake_texture_75D544(300, 300, unknown + v1, v2, snake_data, 0);// first two parameters = width and height of one block of the snake
                        previous_position = previous_position_backup;
                    }
                }               
            }


        }
    }

    int world_sub_762F9A(int x, int z)
    {
        return 0;
    }

    void draw_shadow(ff7_graphics_object *, ff7_game_obj *)
    {
    }

    void engine_apply_4x4_matrix_product_between_matrices(matrix *a1, matrix *a2, matrix *a3)
    {
        struct game_mode* mode = getmode_cached();

        struct matrix projection_matrix;
        ::memcpy(&projection_matrix.m[0][0], &a2->m[0][0], sizeof(projection_matrix.m));    

        if (mode->driver_mode == MODE_WORLDMAP)
        {
            float f_offset = 0.0035f;
            float n_offset = 0.0f;

            float a = projection_matrix._33;
            float b = projection_matrix._43;


            float f = b / (a + 1.0f) + f_offset;
            float n = b / (a - 1.0f) + n_offset;

            
            projection_matrix._33 = -(f + n) / (f -n);
            projection_matrix._43 = -(2*f*n) / (f - n);            
        }
    
        ff7_externals.engine_apply_4x4_matrix_product_between_matrices_66C6CD(a1, &projection_matrix, a3);
    }

    void Renderer::loadWorldMapExternalMesh()
    {
        externalWorldMapModel.unloadExternalMesh();

        const int worldmap_type = *ff7_externals.world_map_type_E045E8;
        const int world_progress = *ff7_externals.world_progress_E28CB4;

        std::string wmStr = "wm" + std::to_string(worldmap_type);
        std::vector<std::string> files;

        files.push_back("wm" + std::to_string(worldmap_type));
        if(worldmap_type == 0)
        {
            files.push_back("wm0_0_" + std::to_string(world_progress > 0));
            files.push_back("wm0_1_" + std::to_string(world_progress > 1));
            files.push_back("wm0_2_" + std::to_string(world_progress > 2));
            files.push_back("wm0_3_" + std::to_string(world_progress > 3));
        }

        auto numFiles = files.size();
        for (int i = 0; i < numFiles; ++i)
        {
            char file_path_gltf[MAX_PATH];
            sprintf(file_path_gltf, "%s/%s/world/%s.gltf", basedir, external_mesh_path.data(), files[i].data());

            char tex_path[MAX_PATH];
            sprintf(tex_path, "%s/%s/world/textures/", basedir, external_mesh_path.data());

            externalWorldMapModel.importExternalMeshGltfFile(file_path_gltf, tex_path);
        }
    }

    void Renderer::loadCloudsExternalMesh()
    {
        externalCloudsModel.unloadExternalMesh();

        char file_path_gltf[MAX_PATH];
        sprintf(file_path_gltf, "%s/%s/world/clouds.gltf", basedir, external_mesh_path.data());

        char tex_path[MAX_PATH];
        sprintf(tex_path, "%s/%s/world/textures/", basedir, external_mesh_path.data());

        externalCloudsModel.importExternalMeshGltfFile(file_path_gltf, tex_path);
    }

    void Renderer::loadMeteorExternalMesh()
    {
        externalMeteorModel.unloadExternalMesh();

        char file_path_gltf[MAX_PATH];
        sprintf(file_path_gltf, "%s/%s/world/meteo.gltf", basedir, external_mesh_path.data());

        char tex_path[MAX_PATH];
        sprintf(tex_path, "%s/%s/world/textures/", basedir, external_mesh_path.data());

        externalMeteorModel.importExternalMeshGltfFile(file_path_gltf, tex_path);
    }

    void Renderer::unloadExternalMeshes()
    {
        externalWorldMapModel.unloadExternalMesh();
        externalCloudsModel.unloadExternalMesh();
        externalMeteorModel.unloadExternalMesh();    
    }

    bool Renderer::drawWorldMapExternalMesh()
    {
        if(gl_defer_world_external_mesh()) return false;

        auto shapeCount = externalWorldMapModel.shapes.size();
        int vertexOffset = 0;
        int indexOffset = 0;

        int world_pos_x = ff7_externals.world_player_pos_E04918->x;
        int world_pos_y = ff7_externals.world_player_pos_E04918->y;
        int world_pos_z = ff7_externals.world_player_pos_E04918->z;

        struct matrix viewMatrix;
        ::memcpy(&viewMatrix.m[0][0], newRenderer.getViewMatrix(), sizeof(viewMatrix.m));

        // Create a world matrix
        struct matrix worldViewMatrix[9];
        for (int gridX = -1; gridX <= 1; gridX++)
        {
            for (int gridZ = -1; gridZ <= 1; gridZ++)
            {
                struct matrix worldMatrix;
                identity_matrix(&worldMatrix);
                worldMatrix._41 = gridX * 294912;
                worldMatrix._42 = -500;
                worldMatrix._43 = gridZ * 229376;
                multiply_matrix(&worldMatrix, &viewMatrix, &worldViewMatrix[3 * (gridX + 1) + gridZ + 1]);
            }
        }

        newRenderer.setInterpolationQualifier(SMOOTH);
        newRenderer.setPrimitiveType();
        newRenderer.isTLVertex(false);
        newRenderer.setBlendMode(RendererBlendMode::BLEND_NONE);
        newRenderer.doTextureFiltering(true);
        newRenderer.doMirrorTextureWrap(true);
        newRenderer.isExternalTexture(true);
        newRenderer.isTexture(true);
        newRenderer.doDepthTest(true);
        newRenderer.doDepthWrite(true);
        newRenderer.setSphericalWorldRate(1.0f);

        if(enable_lighting)
        {
            // Light view frustum pointing to player position
            vector3<float> center = {
                static_cast<float>(world_pos_x),
                static_cast<float>(world_pos_y),
                static_cast<float>(world_pos_z)};

            vector3<float> centerViewSpace;
                    transform_point(&viewMatrix, &center, &centerViewSpace);
            lighting.updateLightMatrices(centerViewSpace);
        }

        struct matrix* pProjMatrix = nullptr;
        if(!ff8)
        {
            struct ff7_game_obj *game_object = (ff7_game_obj *)common_externals.get_game_object();
            if (game_object)
            {
                auto polygon_set = (ff7_polygon_set*)game_object->polygon_set_2EC;
                if(polygon_set)
                {
                    auto matrix_set = polygon_set->matrix_set;
                    pProjMatrix = matrix_set->matrix_projection;
                }

            }

            if(pProjMatrix != nullptr)
            {
                newRenderer.setD3DProjection(pProjMatrix);
                newRenderer.setD3DViweport(&d3dviewport_matrix);
            }
        }

        for (auto& iter : externalWorldMapModel.materials)
        {
            int texCount = iter.second.baseColorTexHandles.size();
            int frameInterval = iter.second.frameInterval * common_frame_multiplier;
            if(texCount == 1 || frameInterval == 0) continue;
            if(frame_counter % frameInterval == 0)
            {
                iter.second.texIndex++;
                if(iter.second.texIndex >= texCount) iter.second.texIndex = 0;
            }
        }

        const float maxDist = 275000.0f * 275000.0f;

        bool isFirstBinding = true;
        for (int i = 0; i < shapeCount; ++i)
        {
            auto& shape = externalWorldMapModel.shapes[i];

            newRenderer.setCullMode(shape.isDoubleSided ? RendererCullMode::DISABLED : RendererCullMode::BACK);

            vector3<float> center;
            center.x = 0.5f * (shape.max.x + shape.min.x);
            center.y = 0.5f * (shape.max.y + shape.min.y);
            center.z = 0.5f * (shape.max.z + shape.min.z);

            float radius = std::max(shape.max.x - shape.min.x, std::max(shape.max.y - shape.min.y, shape.max.z - shape.min.z));

            bool commonBindingSet = false;
            for (int gridX = -1; gridX <= 1; ++gridX)
            {
                for (int gridZ = -1; gridZ <= 1; ++gridZ)
                {
                    vector3<float> centerShifted;
                    centerShifted.x = center.x + gridX * 294912;
                    centerShifted.y = center.z;
                    centerShifted.z = center.y + gridZ * 229376;      

                    vector2<float> diff;
                    diff.x = world_pos_x - centerShifted.x;
                    diff.y = world_pos_z - centerShifted.z;

                    float sqrDist = diff.x * diff.x + diff.y * diff.y;
                    if (sqrDist > maxDist)
                    {
                        continue;
                    }

                    vector3<float> centerShiftedViewSpace;
                    transform_point(&viewMatrix, &centerShifted, &centerShiftedViewSpace);

                    if (centerShiftedViewSpace.z + radius < 0.0f)
                    {
                        continue;
                    }

                    if (std::abs(centerShiftedViewSpace.x) - radius >  175000.0f)
                    {
                        continue;
                    }

                    if (std::abs(centerShiftedViewSpace.y) - radius >  175000.0f)
                    {
                        continue;
                    }
                    
                    if (isFirstBinding)
                    {
                        newRenderer.setWorldViewMatrix(&worldViewMatrix[3 * (gridX + 1) + gridZ + 1]);
                        newRenderer.setCommonUniforms();
                        if (enable_lighting) newRenderer.setLightingUniforms();
                        isFirstBinding = false;
                    }
                    else
                    {				
                        newRenderer.setWorldViewMatrix(&worldViewMatrix[3 * (gridX + 1) + gridZ + 1], false);    
                        newRenderer.setUniform(RendererUniform::WORLD_VIEW, newRenderer.getWorldViewMatrix());
                    }

                    if(!commonBindingSet)
                    {
                        externalWorldMapModel.bindField3dVertexBuffer(vertexOffset, shape.vertices.size());
                        externalWorldMapModel.bindField3dIndexBuffer(indexOffset, shape.indices.size());

                        if(shape.pMaterial != nullptr)
                        {
                            if(shape.pMaterial->baseColorTexHandles.size() > 0)
                            {
                                auto baseColorTexHandle = shape.pMaterial->baseColorTexHandles[shape.pMaterial->texIndex];
                                if(bgfx::isValid(baseColorTexHandle))
                                    newRenderer.useTexture(baseColorTexHandle.idx, RendererTextureSlot::TEX_Y);
                                else newRenderer.useTexture(0, RendererTextureSlot::TEX_Y);
                            }

                            if(shape.pMaterial->normalTexHandles.size() > 0)
                            {
                                auto normalTexHandle = shape.pMaterial->normalTexHandles[0];
                                if(bgfx::isValid(normalTexHandle))
                                    newRenderer.useTexture(normalTexHandle.idx, RendererTextureSlot::TEX_NML);
                                else newRenderer.useTexture(0, RendererTextureSlot::TEX_NML);
                            }

                            if(shape.pMaterial->pbrTexHandles.size() > 0)
                            {
                                auto pbrTexHandle = shape.pMaterial->pbrTexHandles[0];
                                if(bgfx::isValid(pbrTexHandle))
                                    newRenderer.useTexture(pbrTexHandle.idx, RendererTextureSlot::TEX_PBR);
                                else newRenderer.useTexture(0, RendererTextureSlot::TEX_PBR);
                            }

                            newRenderer.bindTextures();

                            commonBindingSet = true;
                        }
                    }

                    if (enable_lighting)
                    {
                        newRenderer.drawToShadowMap(true, true);
                        newRenderer.drawWithLighting(true, true, true);
                    }
                    else newRenderer.draw(true, true, true);
                }
            }

            vertexOffset += shape.vertices.size();
            indexOffset += shape.indices.size();
        }

        newRenderer.discardAllBindings();

        newRenderer.doMirrorTextureWrap(false);

        if(shapeCount > 0) return true;
        else return false;
    }

    bool Renderer::drawCloudsExternalMesh()
    {
        int world_pos_x = ff7_externals.world_player_pos_E04918->x;
        int world_pos_y = ff7_externals.world_player_pos_E04918->y;
        int world_pos_z = ff7_externals.world_player_pos_E04918->z;

        struct matrix viewMatrix;
        ::memcpy(&viewMatrix.m[0][0], newRenderer.getViewMatrix(), sizeof(viewMatrix.m));

        struct matrix inverseViewMatrix;
        ::memcpy(&inverseViewMatrix.m[0][0], newRenderer.getInvViewMatrix(), sizeof(inverseViewMatrix.m));

        vector3<float> cameraPosViewSpace = {0.0f, 0.0f, 0.0f};
        vector3<float> cameraPos = {0.0f, 0.0f, 0.0f};
        transform_point(&inverseViewMatrix, &cameraPosViewSpace, &cameraPos);

        vector3<float> forward = {0.0f, 0.0f, 0.0f};
        vector3<float> focusPos = {static_cast<float>(world_pos_x), static_cast<float>(world_pos_y), static_cast<float>(world_pos_z)};
        subtract_vector(&focusPos, &cameraPos, &forward);
        forward.y = 0;
        normalize_vector(&forward);

        vector3<float> up = {0.0, 1.0, 0.0 };
        vector3<float> right = {0.0, 0.0, 0.0};
        cross_product(&forward, &up, &right);

        static const int numQuads = 5;

        static float frameCount = 0;
        frameCount++;

        // Create a world matrix
        struct matrix worldViewMatrix[numQuads];

        const float scaleX = 150000;
        const float scaleY = scaleX / 4;
        for(int i = 0; i < numQuads; ++i)
        {
            identity_matrix(&worldViewMatrix[i]);  

            float offset = 2.0f * scaleX * (i- numQuads / 2);
            float cameraOffset = -4.0f * scaleX * (std::remainder(ff7::world::camera.getRotationOffsetY(), 360.0f) / 360.0f);
            float totalOffset = offset + cameraOffset;

            struct matrix worldMatrix;
            identity_matrix(&worldMatrix);
            worldMatrix._11 = (i % 2 == 0 ? 1 : -1) *  scaleX * right.x;
            worldMatrix._12 = (i % 2 == 0 ? 1 : -1) *  scaleX * right.y;
            worldMatrix._13 = (i % 2 == 0 ? 1 : -1) * scaleX * right.z;
            worldMatrix._21 = -scaleY * up.x;
            worldMatrix._22 = -scaleY * up.y;
            worldMatrix._23 = -scaleY * up.z;
            worldMatrix._31 = forward.x;
            worldMatrix._32 = forward.y;
            worldMatrix._33 = forward.z;
            worldMatrix._41 = focusPos.x + forward.x * 100000 + totalOffset * right.x;
            worldMatrix._42 = 20000;
            worldMatrix._43 = focusPos.z + forward.z * 100000 + totalOffset * right.z;

            multiply_matrix(&worldMatrix, &viewMatrix, &worldViewMatrix[i]);
        
        }

        newRenderer.setInterpolationQualifier(SMOOTH);
        newRenderer.setPrimitiveType();
        newRenderer.isTLVertex(false);
        newRenderer.setBlendMode(RendererBlendMode::BLEND_ADD);
        newRenderer.doTextureFiltering(true);    
        newRenderer.isExternalTexture(true);
        newRenderer.isTexture(true);
        newRenderer.doDepthTest(true);
        newRenderer.doDepthWrite(false);
        newRenderer.setSphericalWorldRate(1.5f);

        struct matrix* pProjMatrix = nullptr;
        if(!ff8)
        {
            struct ff7_game_obj *game_object = (ff7_game_obj *)common_externals.get_game_object();
            if (game_object)
            {
                auto polygon_set = (ff7_polygon_set*)game_object->polygon_set_2EC;
                if(polygon_set)
                {
                    auto matrix_set = polygon_set->matrix_set;
                    pProjMatrix = matrix_set->matrix_projection;
                }

            }

            if(pProjMatrix != nullptr)
            {
                newRenderer.setD3DProjection(pProjMatrix);
                newRenderer.setD3DViweport(&d3dviewport_matrix);
            }
        }

        newRenderer.setWorldViewMatrix(&worldViewMatrix[0]);    
        newRenderer.setCommonUniforms();

        auto shapeCount = externalCloudsModel.shapes.size();
        for (int i = 0; i < shapeCount; ++i)
        {
            auto& shape = externalCloudsModel.shapes[i];

            newRenderer.setCullMode(shape.isDoubleSided ? RendererCullMode::DISABLED : RendererCullMode::BACK);

            externalCloudsModel.bindField3dVertexBuffer(0, shape.vertices.size());
            externalCloudsModel.bindField3dIndexBuffer(0, shape.indices.size());

            if(shape.pMaterial != nullptr)
            {
                if(shape.pMaterial->baseColorTexHandles.size() > 0)
                {
                    auto baseColorTexHandle = shape.pMaterial->baseColorTexHandles[shape.pMaterial->texIndex];
                    if(bgfx::isValid(baseColorTexHandle))
                        newRenderer.useTexture(baseColorTexHandle.idx, RendererTextureSlot::TEX_Y);
                    else newRenderer.useTexture(0, RendererTextureSlot::TEX_Y);
                }

                if(shape.pMaterial->normalTexHandles.size() > 0)
                {
                    auto normalTexHandle = shape.pMaterial->normalTexHandles[0];
                    if(bgfx::isValid(normalTexHandle))
                        newRenderer.useTexture(normalTexHandle.idx, RendererTextureSlot::TEX_NML);
                    else newRenderer.useTexture(0, RendererTextureSlot::TEX_NML);
                }

                if(shape.pMaterial->pbrTexHandles.size() > 0)
                {
                    auto pbrTexHandle = shape.pMaterial->pbrTexHandles[0];
                    if(bgfx::isValid(pbrTexHandle))
                        newRenderer.useTexture(pbrTexHandle.idx, RendererTextureSlot::TEX_PBR);
                    else newRenderer.useTexture(0, RendererTextureSlot::TEX_PBR);
                }

                newRenderer.bindTextures();      
            }

            for(int i = 0; i < numQuads; ++i)
            {
                if(i != 0)
                {
                    newRenderer.setWorldViewMatrix(&worldViewMatrix[i]);
                    newRenderer.setUniform(RendererUniform::WORLD_VIEW, newRenderer.getWorldViewMatrix());
                }

                newRenderer.draw(true, true, true);
            }
        }
        
        newRenderer.discardAllBindings();

        newRenderer.setSphericalWorldRate(1.0f);

        return true;
    }

    bool Renderer::drawMeteorExternalMesh()
    {
        int world_pos_x = ff7_externals.world_player_pos_E04918->x;
        int world_pos_y = ff7_externals.world_player_pos_E04918->y;
        int world_pos_z = ff7_externals.world_player_pos_E04918->z;

        struct matrix viewMatrix;
        ::memcpy(&viewMatrix.m[0][0], newRenderer.getViewMatrix(), sizeof(viewMatrix.m));

        struct matrix inverseViewMatrix;
        ::memcpy(&inverseViewMatrix.m[0][0], newRenderer.getInvViewMatrix(), sizeof(inverseViewMatrix.m));

        vector3<float> cameraPosViewSpace = {0.0f, 0.0f, 0.0f};
        vector3<float> cameraPos = {0.0f, 0.0f, 0.0f};
        transform_point(&inverseViewMatrix, &cameraPosViewSpace, &cameraPos);

        vector3<float> forward = {0.0f, 0.0f, 0.0f};
        vector3<float> focusPos = {static_cast<float>(world_pos_x), static_cast<float>(world_pos_y), static_cast<float>(world_pos_z)};
        subtract_vector(&focusPos, &cameraPos, &forward);
        forward.y = 0;
        normalize_vector(&forward);

        vector3<float> up = {0.0, 1.0, 0.0 };
        vector3<float> right = {0.0, 0.0, 0.0};
        cross_product(&forward, &up, &right);

        // Create a world matrix
        struct matrix worldViewMatrix;

        identity_matrix(&worldViewMatrix);

        const float scaleX = 100000;
        const float scaleY = scaleX / 2;

        float cameraOffset = -8.0f * scaleX * (std::remainder(ff7::world::camera.getRotationOffsetY(), 360.0f) / 360.0f);

        struct matrix worldMatrix;
        identity_matrix(&worldMatrix);
        worldMatrix._11 = scaleX * right.x;
        worldMatrix._12 = scaleX * right.y;
        worldMatrix._13 = scaleX * right.z;
        worldMatrix._21 = scaleY * up.x;
        worldMatrix._22 = scaleY * up.y;
        worldMatrix._23 = scaleY * up.z;
        worldMatrix._31 = forward.x;
        worldMatrix._32 = forward.y;
        worldMatrix._33 = forward.z;
        worldMatrix._41 = focusPos.x + forward.x * 150000 + cameraOffset * right.x;
        worldMatrix._42 = 40000;
        worldMatrix._43 = focusPos.z + forward.z * 150000 + cameraOffset * right.z;

        multiply_matrix(&worldMatrix, &viewMatrix, &worldViewMatrix);

        newRenderer.setInterpolationQualifier(SMOOTH);
        newRenderer.setPrimitiveType();
        newRenderer.isTLVertex(false);
        newRenderer.setBlendMode(RendererBlendMode::BLEND_ADD);
        newRenderer.doTextureFiltering(true);
        newRenderer.isExternalTexture(true);
        newRenderer.isTexture(true);
        newRenderer.doDepthTest(true);
        newRenderer.doDepthWrite(false);
        newRenderer.setSphericalWorldRate(4.0f);

        struct matrix* pProjMatrix = nullptr;
        if(!ff8)
        {
            struct ff7_game_obj *game_object = (ff7_game_obj *)common_externals.get_game_object();
            if (game_object)
            {
                auto polygon_set = (ff7_polygon_set*)game_object->polygon_set_2EC;
                if(polygon_set)
                {
                    auto matrix_set = polygon_set->matrix_set;
                    pProjMatrix = matrix_set->matrix_projection;
                }

            }

            if(pProjMatrix != nullptr)
            {
                newRenderer.setD3DProjection(pProjMatrix);
                newRenderer.setD3DViweport(&d3dviewport_matrix);
            }
        }

        newRenderer.setWorldViewMatrix(&worldViewMatrix);    
        newRenderer.setCommonUniforms();

        auto shapeCount = externalMeteorModel.shapes.size();
        for (int i = 0; i < shapeCount; ++i)
        {
            auto& shape = externalMeteorModel.shapes[i];

            newRenderer.setCullMode(shape.isDoubleSided ? RendererCullMode::DISABLED : RendererCullMode::BACK);

            externalMeteorModel.bindField3dVertexBuffer(0, shape.vertices.size());
            externalMeteorModel.bindField3dIndexBuffer(0, shape.indices.size());

            if(shape.pMaterial != nullptr)
            {
                if(shape.pMaterial->baseColorTexHandles.size() > 0)
                {
                    auto baseColorTexHandle = shape.pMaterial->baseColorTexHandles[shape.pMaterial->texIndex];
                    if(bgfx::isValid(baseColorTexHandle))
                        newRenderer.useTexture(baseColorTexHandle.idx, RendererTextureSlot::TEX_Y);
                    else newRenderer.useTexture(0, RendererTextureSlot::TEX_Y);
                }

                if(shape.pMaterial->normalTexHandles.size() > 0)
                {
                    auto normalTexHandle = shape.pMaterial->normalTexHandles[0];
                    if(bgfx::isValid(normalTexHandle))
                        newRenderer.useTexture(normalTexHandle.idx, RendererTextureSlot::TEX_NML);
                    else newRenderer.useTexture(0, RendererTextureSlot::TEX_NML);
                }

                if(shape.pMaterial->pbrTexHandles.size() > 0)
                {
                    auto pbrTexHandle = shape.pMaterial->pbrTexHandles[0];
                    if(bgfx::isValid(pbrTexHandle))
                        newRenderer.useTexture(pbrTexHandle.idx, RendererTextureSlot::TEX_PBR);
                    else newRenderer.useTexture(0, RendererTextureSlot::TEX_PBR);
                }

                newRenderer.bindTextures();      
            }

            newRenderer.draw(true, true, true);
        }
        
        newRenderer.discardAllBindings();

        newRenderer.setSphericalWorldRate(1.0f);

        return true;
    }

    bool Renderer::drawCloudsAndMeteorExternalMesh(bool isDrawMeteor)
    {
        if(gl_defer_cloud_external_mesh()) return false;

        if (isDrawMeteor) drawMeteorExternalMesh();
        drawCloudsExternalMesh();  

        return true;  
    }
}
