/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2026 Julian Xhokaxhiu                                   //
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

#include "../../ff7.h"
#include "../../gl.h"
#include "../../log.h"
#include "../defs.h"
#include "kawai.h"
#include "model.h"

#include <unordered_map>
#include <bx/math.h>

namespace ff7::field
{
    namespace
    {
        struct ModelEffect
        {
            kawai::State state;
            kawai::Effect draw_effect;
            std::array<kawai::Effect, 2> packet_effects{};
            std::array<uint8_t, 32> params{};
            size_t parameter_count = 0;
            int opcode = -1;
            bool pending = false;
            bool applied = false;
            int splash_frame = 0;
            hrc_data* model = nullptr;
            struc_110* instance = nullptr;
        };

        struct Binding
        {
            ff7_polygon_set* polygon_set;
        };

        std::array<ModelEffect, FF7_MAX_NUM_MODEL_ENTITIES> models{};
        std::unordered_map<polygon_data*, Binding> bindings;
        int current_model = -1;
        size_t current_packet = 0;
        int packet_frame = 0;

        kawai::Vector to_psx(const vector3<float>& position)
        {
            return {position.x, -position.z, position.y};
        }

        vector3<float> to_pc(const kawai::Vector& position)
        {
            return {position[0], position[2], -position[1]};
        }

        std::optional<kawai::gte::Matrix> fixed_model_rotation(const matrix& model, const matrix& inverse_camera)
        {
            kawai::gte::Matrix result{};
            vector3<float> world;
            for (size_t column = 0; column < 3; ++column)
            {
                kawai::Vector axis{};
                axis[column] = 1;
                const auto local = to_pc(axis);
                vector3<float> direction = {
                    local.x * model._11 + local.y * model._21 + local.z * model._31,
                    local.x * model._12 + local.y * model._22 + local.z * model._32,
                    local.x * model._13 + local.y * model._23 + local.z * model._33};
                world = {
                    direction.x * inverse_camera._11 + direction.y * inverse_camera._21 + direction.z * inverse_camera._31,
                    direction.x * inverse_camera._12 + direction.y * inverse_camera._22 + direction.z * inverse_camera._32,
                    direction.x * inverse_camera._13 + direction.y * inverse_camera._23 + direction.z * inverse_camera._33};
                for (float value : {world.x, world.y, world.z})
                    if (!std::isfinite(value) || std::abs(value) > 2147483000.0f) return std::nullopt;
                const auto fixed = kawai::fixed_normal(kawai::normalize({world.x, world.y, world.z}));
                for (size_t row = 0; row < 3; ++row) result[row][column] = fixed[row];
            }
            return result;
        }

        kawai::gte::Affine fixed_camera_matrix(const matrix& camera)
        {
            kawai::gte::Affine result;
            for (size_t row = 0; row < 3; ++row)
            {
                for (size_t column = 0; column < 3; ++column)
                    result.rotation[row][column] = kawai::gte::word(std::llround(camera.m[column][row] * 4096));
                result.translation[row] = kawai::gte::long_word(std::llround(camera.m[3][row]));
            }
            return result;
        }
    }

    KawaiModelScope::KawaiModelScope(struc_110* instance) : previous_model(current_model)
    {
        current_model = -1;
        if (getmode_cached()->driver_mode != MODE_FIELD) return;
        for (size_t i = 0; i < models.size(); ++i)
            if (models[i].instance == instance)
            {
                current_model = static_cast<int>(i);
                break;
            }
    }

    KawaiModelScope::~KawaiModelScope()
    {
        current_model = previous_model;
    }

    bool queue_kawai_effect(int model_id, uint8_t opcode, std::span<const uint8_t> params)
    {
        if (model_id < 0 || model_id >= FF7_MAX_NUM_MODEL_ENTITIES)
        {
            ffnx_error("KAWAI: invalid model id %d\n", model_id);
            return false;
        }
        auto& model = models[model_id];
        if (!kawai::handles(opcode))
        {
            model.pending = false;
            if (opcode == 0xE || opcode == 0xD || opcode == 0xFF)
            {
                model.state = {};
                model.draw_effect = {};
                model.packet_effects = {};
                model.applied = false;
            }
            return false;
        }
        model.opcode = opcode;
        model.pending = true;
        model.parameter_count = std::min(params.size(), model.params.size());
        std::copy_n(params.begin(), model.parameter_count, model.params.begin());
        return true;
    }

    bool apply_kawai_effect(int model_id, int opcode, ff7_hrc_polygon_data* model_data, int* result, int* complete)
    {
        if (model_id < 0 || model_id >= FF7_MAX_NUM_MODEL_ENTITIES || opcode < 0 || opcode > 0xFF) return false;
        const auto subcode = static_cast<uint8_t>(opcode);
        if (!kawai::handles(subcode)) return false;
        auto& model = models[model_id];
        if (model.pending && model.opcode == opcode)
        {
            auto candidate = model.state;
            if (!kawai::configure(candidate, subcode,
                std::span(model.params).first(model.parameter_count)))
            {
                ffnx_error("KAWAI: invalid %02X parameters or execute without initialization for model %d\n", opcode, model_id);
                model.state = {};
                model.draw_effect = {};
                model.packet_effects = {};
                model.pending = false;
                *result = -1;
                *complete = 1;
                return true;
            }
            model.state = candidate;
            model.pending = false;
            if (trace_all || trace_opcodes)
                ffnx_trace("KAWAI[%02X]: model=%d,executing=%d,parameter_count=%zu\n",
                    opcode, model_id, model.state.executing, model.parameter_count);
        }
        if (model_data && model_data->hrc_data)
        {
            model.model = model_data->hrc_data;
            model.instance = &model_data->struc_110;
            for (uint32_t i = 0; i < model.model->num_bones; ++i)
            {
                auto& bone = model.model->bones[i];
                if (!bone.rsd_array) continue;
                for (uint32_t j = 0; j < bone.num_rsd; ++j)
                {
                    auto* rsd = bone.rsd_array[j].rsd_data;
                    if (rsd && rsd->polygon_set && rsd->polygon_set->polygon_data)
                        bindings[rsd->polygon_set->polygon_data] = {rsd->polygon_set};
                }
            }
        }
        model.applied = model.state.executing || (model.state.initialized && model.state.opcode == 0xC);
        const bool direct = opcode >= 2 && opcode <= 5;
        *result = direct ? -1 : opcode;
        *complete = !model.state.executing || direct ||
            ((opcode == 6 || opcode == 8) && model.state.fade.finished);
        if (opcode == 9 && model.state.executing &&
            model.splash_frame++ % std::max(common_frame_multiplier, 1) != 0)
            return true;
        // SPLASH retains the native particle state machine; only its coloring is replaced.
        return opcode != 9;
    }

    void advance_kawai_effects(int frame_multiplier)
    {
        for (auto& model : models)
        {
            if (model.applied)
            {
                auto effect = model.state.effect;
                // PSX 0C initialization hides normal packets until its projected renderer executes.
                if (model.state.opcode == 0xC && !model.state.executing)
                    effect.kind = kawai::Kind::hidden;
                // AMBNT and LIGHT write only the active PSX packet buffer, including the final fade value.
                if (model.state.opcode == 2 || model.state.opcode == 6)
                    model.packet_effects[current_packet] = effect;
                else
                    model.packet_effects.fill(effect);
                kawai::advance(model.state, frame_multiplier);
            }
            model.draw_effect = model.packet_effects[current_packet];
            model.applied = false;
        }
        if (++packet_frame >= std::max(frame_multiplier, 1))
        {
            packet_frame = 0;
            current_packet ^= 1;
        }
    }

    void reset_kawai_effects()
    {
        models = {};
        bindings.clear();
        current_model = -1;
        current_packet = 0;
        packet_frame = 0;
        current_state.kawai_effect = {};
    }

    bool bind_kawai_effect(polygon_data* polygon)
    {
        current_state.kawai_effect = {};
        const auto found = bindings.find(polygon);
        if (found == bindings.end() || current_model < 0) return false;
        auto& model = models[current_model];
        if (model.draw_effect.kind == kawai::Kind::none || !model.model) return false;
        if (!*ff7_externals.field_level_data_pointer || !*ff7_externals.field_camera_data)
        {
            ffnx_error("KAWAI: field camera is unavailable\n");
            return false;
        }
        current_state.kawai_effect = model.draw_effect;
        if (current_state.kawai_effect.kind == kawai::Kind::positional_lighting)
        {
            int64_t radius = current_state.kawai_effect.light_radius;
            bool found_part = false;
            for (uint32_t i = 0; i < model.model->num_bones && !found_part; ++i)
            {
                const auto& bone = model.model->bones[i];
                if (!bone.rsd_array) continue;
                for (uint32_t j = 0; j < bone.num_rsd; ++j)
                {
                    const auto* rsd = bone.rsd_array[j].rsd_data;
                    if (!rsd || !rsd->polygon_set || !rsd->polygon_set->field_0) continue;
                    ++radius;
                    if (rsd->polygon_set->polygon_data == polygon)
                    {
                        found_part = true;
                        break;
                    }
                }
            }
            if (!found_part)
            {
                current_state.kawai_effect = {};
                ffnx_error("KAWAI: positional-light model part is unavailable\n");
                return false;
            }
            current_state.kawai_effect.light_radius = kawai::gte::word(radius);
        }
        ff7_get_field_view_matrix(&current_state.kawai_camera_matrix);
        return true;
    }

    bool has_kawai_splash_color()
    {
        const int id = ff7_kawai_current_model_id;
        return id >= 0 && id < models.size() && models[id].state.initialized && models[id].state.opcode == 9;
    }

    KawaiGeometry process_kawai_geometry(std::span<const nvertex> vertices,
        std::span<const vector3<float>> normals, std::span<const uint32_t> indices,
        std::span<const uint32_t> source_colors, light_data* light)
    {
        KawaiGeometry output;
        const auto& effect = current_state.kawai_effect;
        if (effect.kind == kawai::Kind::hidden) return output;
        if (indices.size() % 3 != 0 || (!normals.empty() && normals.size() != vertices.size()))
        {
            ffnx_error("KAWAI: inconsistent triangle or normal arrays\n");
            return {};
        }
        const bool custom_lighting = effect.kind == kawai::Kind::lighting ||
            effect.kind == kawai::Kind::positional_lighting;
        const bool gpu_color = kawai::gpu_color_offset(effect,
            light && !normals.empty() && game_lighting != GAME_LIGHTING_ORIGINAL);
        if ((gpu_color || custom_lighting || effect.kind == kawai::Kind::gradient || (effect.flags & 1)) &&
            source_colors.size() != vertices.size())
        {
            ffnx_error("KAWAI: source vertex colors are unavailable for this primitive\n");
            return {};
        }
        if (custom_lighting && normals.size() != vertices.size())
        {
            ffnx_error("KAWAI: normals are unavailable for custom lighting\n");
            return {};
        }
        matrix inverse;
        matrix inverse_camera;
        for (const auto& row : current_state.kawai_camera_matrix.m)
            for (float component : row)
                if (!std::isfinite(component) || std::abs(component) > 2147483000.0f)
                {
                    ffnx_error("KAWAI: invalid field camera transform\n");
                    return {};
                }
        bx::mtxInverse(inverse_camera.m[0], current_state.kawai_camera_matrix.m[0]);
        for (const auto& row : current_state.world_view_matrix.m)
            for (float component : row)
                if (!std::isfinite(component))
                {
                    ffnx_error("KAWAI: non-finite model transform\n");
                    return {};
                }
        const auto world_rotation = fixed_model_rotation(current_state.world_view_matrix, inverse_camera);
        if (!world_rotation)
        {
            ffnx_error("KAWAI: invalid fixed-point model transform\n");
            return {};
        }
        const auto camera_matrix = fixed_camera_matrix(current_state.kawai_camera_matrix);
        const auto camera_rotation = kawai::gte::compose(camera_matrix, {*world_rotation, {}}).rotation;
        const auto directions = kawai::gte::compose({kawai::light_directions(effect), {}},
            {camera_rotation, {}}).rotation;
        const auto plane_model = kawai::gte::to_plane(effect.plane, {});
        const auto plane_camera = kawai::gte::from_plane(effect.plane, camera_matrix);
        const bool cpu_lighting = light && game_lighting != GAME_LIGHTING_ORIGINAL &&
            (effect.kind == kawai::Kind::color_offset || effect.kind == kawai::Kind::below_level ||
                effect.kind == kawai::Kind::gradient);
        kawai::Effect base_lighting;
        if (cpu_lighting)
        {
            base_lighting.color = {128 * light->global_light_color.r, 128 * light->global_light_color.g,
                128 * light->global_light_color.b};
            const std::array<bgra_color, 3> colors = {light->light_color_1, light->light_color_2, light->light_color_3};
            const std::array<vector3<float>, 3> directions = {light->light_dir_1, light->light_dir_2, light->light_dir_3};
            for (size_t i = 0; i < 3; ++i)
            {
                base_lighting.light_colors[i] = {128 * colors[i].r, 128 * colors[i].g, 128 * colors[i].b};
                base_lighting.light_directions[i] = {-directions[i].x, -directions[i].y, -directions[i].z};
            }
            const kawai::Vector scripted = {light->scripted_light_color.r, light->scripted_light_color.g,
                light->scripted_light_color.b};
            for (size_t i = 0; i < 3; ++i)
            {
                base_lighting.color[i] *= scripted[i];
                for (auto& color : base_lighting.light_colors) color[i] *= scripted[i];
            }
        }
        if (effect.kind == kawai::Kind::clip)
        {
            const auto& m = current_state.world_view_matrix;
            const float determinant = m._11 * (m._22 * m._33 - m._23 * m._32) -
                m._12 * (m._21 * m._33 - m._23 * m._31) + m._13 * (m._21 * m._32 - m._22 * m._31);
            if (std::abs(determinant) < 1e-8f)
            {
                ffnx_error("KAWAI: cannot project a model with a singular transform\n");
                return {};
            }
            bx::mtxInverse(inverse.m[0], current_state.world_view_matrix.m[0]);
        }
        output.vertices.reserve(indices.size());
        if (!normals.empty()) output.normals.reserve(indices.size());

        for (size_t triangle = 0; triangle + 2 < indices.size(); triangle += 3)
        {
            std::array<nvertex, 3> primitive;
            std::array<kawai::Vector, 3> positions;
            std::array<kawai::gte::LongVector, 3> camera_positions;
            bool visible = false;
            for (size_t corner = 0; corner < 3; ++corner)
            {
                const auto index = indices[triangle + corner];
                if (index >= vertices.size())
                {
                    ffnx_error("KAWAI: vertex index %u exceeds vertex count %zu\n", index, vertices.size());
                    return {};
                }
                primitive[corner] = vertices[index];
                vector3<float> posed;
                transform_point(&current_state.world_view_matrix, &primitive[corner]._, &posed);
                // The native model-view matrix already maps PC local axes into PSX camera axes.
                vector3<float> world;
                transform_point(&inverse_camera, &posed, &world);
                kawai::gte::LongVector world_position{};
                const kawai::Vector coordinates = {world.x, world.y, world.z};
                // PC meshes are reauthored floating-point geometry, not PSX SVECTORs.
                // Quantize the posed field coordinates, not their small local vertices.
                for (size_t i = 0; i < 3; ++i)
                {
                    if (!std::isfinite(coordinates[i]) || std::abs(coordinates[i]) > 2147483000.0f)
                    {
                        ffnx_error("KAWAI: invalid model vertex coordinates\n");
                        return {};
                    }
                    world_position[i] = static_cast<int32_t>(std::floor(coordinates[i]));
                }
                camera_positions[corner] = kawai::gte::transform_world(camera_matrix, world_position);
                const auto transformed = effect.kind == kawai::Kind::gradient && (effect.flags & 0x10) ?
                    camera_positions[corner] : world_position;
                for (size_t i = 0; i < 3; ++i) positions[corner][i] = static_cast<float>(transformed[i]);
                if (effect.kind == kawai::Kind::clip)
                {
                    const auto plane_position = kawai::gte::transform_world(plane_model, world_position);
                    kawai::gte::Vector projected = {kawai::gte::ir(plane_position[0]),
                        kawai::gte::ir(plane_position[1]), kawai::gte::ir(plane_position[2])};
                    visible |= kawai::gte::project(projected, effect.plane);
                    const auto projected_camera = kawai::gte::transform(plane_camera, projected);
                    vector3<float> point = {static_cast<float>(projected_camera[0]),
                        static_cast<float>(projected_camera[1]), static_cast<float>(projected_camera[2])};
                    transform_point(&inverse, &point, &primitive[corner]._);
                }
            }
            if (effect.kind == kawai::Kind::clip && !visible) continue;
            const auto average = kawai::triangle_average(positions);
            for (size_t corner = 0; corner < 3; ++corner)
            {
                const auto index = indices[triangle + corner];
                auto& vertex = primitive[corner];
                if (gpu_color)
                {
                    // AMBNT tints albedo; only LIGHT is composed after lighting.
                    vertex.color.color = (vertex.color.color & 0xFF000000) | (source_colors[index] & 0xFFFFFF);
                    if (!effect.color_fade)
                        vertex.color.color = kawai::apply_color_offset(effect, positions[corner], vertex.color.color, source_colors[index]);
                }
                else if (effect.kind == kawai::Kind::color_offset ||
                    effect.kind == kawai::Kind::below_level || effect.kind == kawai::Kind::gradient)
                {
                    const bool relit = cpu_lighting && index < normals.size();
                    if (relit)
                    {
                        if (index < source_colors.size())
                            vertex.color.color = (vertex.color.color & 0xFF000000) | (source_colors[index] & 0xFFFFFF);
                        matrix rotation = current_state.world_view_matrix;
                        rotation._41 = rotation._42 = rotation._43 = 0;
                        auto normal = normals[index];
                        vector3<float> camera_normal;
                        transform_point(&rotation, &normal, &camera_normal);
                        auto camera_rotation = inverse_camera;
                        camera_rotation._41 = camera_rotation._42 = camera_rotation._43 = 0;
                        vector3<float> world_normal;
                        transform_point(&camera_rotation, &camera_normal, &world_normal);
                        vertex.color.color = kawai::enhanced_light_color(vertex.color.color, base_lighting,
                            kawai::normalize({world_normal.x, world_normal.y, world_normal.z}));
                    }
                    const bool per_vertex = effect.kind == kawai::Kind::gradient ? current_state.shademode != 0 :
                        (effect.flags & 2) != 0;
                    const auto& position = per_vertex ? positions[corner] : average;
                    // 0B adds its gradient after lighting, without restoring the unlit material color.
                    const auto source = relit && effect.kind == kawai::Kind::gradient ? vertex.color.color :
                        (index < source_colors.size() ? source_colors[index] : vertex.color.color);
                    vertex.color.color = kawai::apply_color_offset(effect, position, vertex.color.color, source);
                }
                else if (custom_lighting)
                {
                    const size_t lighting_corner = current_state.shademode ? corner : 0;
                    const auto lighting_index = indices[triangle + lighting_corner];
                    const auto normal = to_psx(normals[lighting_index]);
                    for (float component : normal)
                        if (!std::isfinite(component) || std::abs(component) > 524000)
                        {
                            ffnx_error("KAWAI: invalid model normal\n");
                            return {};
                        }
                    auto local_directions = directions;
                    if (effect.kind == kawai::Kind::positional_lighting)
                    {
                        const auto direction = kawai::gte::point_direction(effect.light_position, effect.light_radius,
                            camera_positions[lighting_corner], camera_rotation);
                        if (!direction)
                        {
                            ffnx_error("KAWAI: invalid positional-light radius or squared distance\n");
                            return {};
                        }
                        local_directions = {};
                        local_directions[0] = *direction;
                    }
                    vertex.color.color = kawai::light_color((vertex.color.color & 0xFF000000) |
                        (source_colors[lighting_index] & 0xFFFFFF), effect, kawai::fixed_normal(normal), local_directions);
                }
                output.vertices.push_back(vertex);
                if (!normals.empty()) output.normals.push_back(normals[index]);
            }
        }
        return output;
    }

    bool draw_kawai_model(polygon_data* polygon, indexed_primitive* primitive,
        std::span<const vector3<float>> normals, light_data* light, uint32_t clip,
        std::span<const uint32_t> source_colors)
    {
        if (current_state.kawai_effect.kind == kawai::Kind::none) return false;
        const auto found = bindings.find(polygon);
        if (source_colors.empty() && found != bindings.end() && polygon->vertexcolordata)
        {
            auto* polygon_set = found->second.polygon_set;
            for (uint32_t i = 0; i < polygon_set->numgroups; ++i)
                if (polygon_set->indexed_primitives[i] == primitive)
                {
                    source_colors = {reinterpret_cast<const uint32_t*>(polygon->vertexcolordata) +
                        polygon->groupdata[i].offvert, primitive->vertexcount};
                    break;
                }
        }
        std::vector<uint32_t> indices(primitive->indices, primitive->indices + primitive->indexcount);
        auto geometry = process_kawai_geometry({primitive->vertices, primitive->vertexcount}, normals, indices, source_colors, light);
        for (size_t start = 0; start < geometry.vertices.size(); start += 65535)
        {
            const auto count = static_cast<uint32_t>(std::min<size_t>(65535, geometry.vertices.size() - start));
            std::vector<WORD> batch_indices(count);
            for (uint32_t i = 0; i < count; ++i) batch_indices[i] = static_cast<WORD>(i);
            auto* batch_normals = geometry.normals.empty() ? nullptr : geometry.normals.data() + start;
            const bool cpu_color = current_state.kawai_effect.kind != kawai::Kind::clip &&
                !kawai::gpu_color_offset(current_state.kawai_effect,
                    light && batch_normals && game_lighting != GAME_LIGHTING_ORIGINAL);
            gl_draw_indexed_primitive(primitive->primitivetype, primitive->vertextype,
                geometry.vertices.data() + start, batch_normals, count,
                batch_indices.data(), count, nullptr, polygon->boundingboxdata, cpu_color ? nullptr : light, clip, true);
        }
        return true;
    }
}
