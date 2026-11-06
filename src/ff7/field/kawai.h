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

#pragma once

#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstdint>
#include <span>
#include <vector>
#include "kawai_gte.h"

namespace ff7::field::kawai
{
    using Vector = std::array<float, 3>;

    enum class Kind
    {
        none,
        color_offset,
        below_level,
        gradient,
        lighting,
        positional_lighting,
        clip,
        hidden
    };

    struct Effect
    {
        Kind kind = Kind::none;
        Vector color{};
        Vector target_color{};
        float level = 0;
        float target_level = 0;
        int16_t gradient_range = 0;
        uint8_t flags = 0;
        bool color_fade = false;
        std::array<Vector, 3> light_colors{};
        std::array<Vector, 3> light_directions{};
        gte::Plane plane{};
        gte::Vector light_position{};
        int16_t light_radius = 0;
    };

    struct Fade
    {
        std::array<int16_t, 4> value{};
        std::array<int16_t, 4> target{};
        std::array<int16_t, 4> step{};
        bool finished = false;
        int frame = 0;
    };

    struct State
    {
        Effect effect{};
        Fade fade{};
        uint8_t opcode = 0;
        bool initialized = false;
        bool executing = false;
    };

    inline int16_t signed_word(int value)
    {
        return std::bit_cast<int16_t>(static_cast<uint16_t>(value));
    }

    inline int16_t read_s16(std::span<const uint8_t> params, size_t offset)
    {
        return signed_word(static_cast<uint16_t>(params[offset]) |
            (static_cast<uint16_t>(params[offset + 1]) << 8));
    }

    inline bool handles(uint8_t opcode)
    {
        return opcode == 2 || opcode == 3 || opcode == 4 || opcode == 5 || opcode == 6 || opcode == 7 || opcode == 8 ||
            opcode == 9 || opcode == 0xB || opcode == 0xC;
    }

    inline size_t parameter_size(uint8_t opcode, bool initializing)
    {
        switch (opcode)
        {
            case 2: return 7;
            case 3: return 30;
            case 4: return 10;
            case 5: return 20;
            // Execute scripts need only the activation byte; the native dispatcher supplies the workspace slot.
            case 6: return initializing ? 21 : 1;
            case 7: return initializing ? 32 : 1;
            case 8: return initializing ? 27 : 1;
            case 9: return initializing ? 11 : 1;
            case 0xB: return initializing ? 19 : 1;
            case 0xC: return initializing ? 10 : 1;
            default: return 0;
        }
    }

    inline float dot(const Vector& a, const Vector& b)
    {
        return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
    }

    inline Vector normalize(Vector v)
    {
        const float length = std::sqrt(dot(v, v));
        if (length != 0)
            for (float& component : v) component /= length;
        return v;
    }

    inline void update_fade_color(State& state)
    {
        for (size_t i = 0; i < 3; ++i) state.effect.color[i] = state.fade.value[i];
        state.effect.level = state.fade.value[3];
    }

    inline bool configure(State& state, uint8_t opcode, std::span<const uint8_t> params)
    {
        if (!handles(opcode) || params.empty()) return false;
        const bool direct = opcode == 2 || opcode == 3 || opcode == 4 || opcode == 5;
        const bool initializing = direct || params[0] == 0;
        if (params.size() < parameter_size(opcode, initializing)) return false;
        if (initializing && opcode == 0xB && read_s16(params, 8) == read_s16(params, 16)) return false;
        if (initializing && opcode == 0xB && (params[18] & 0xC) == 0xC) return false;
        if (initializing && opcode == 0xC && read_s16(params, 2) == 0 &&
            read_s16(params, 4) == 0 && read_s16(params, 6) == 0) return false;
        std::optional<gte::Plane> plane;
        if (initializing && opcode == 0xC)
        {
            plane = gte::make_plane({read_s16(params, 2), read_s16(params, 4), read_s16(params, 6)},
                read_s16(params, 8));
            if (!plane) return false;
        }
        if (!initializing)
        {
            if (params[0] != 1 || !state.initialized || state.opcode != opcode) return false;
            state.executing = true;
            return true;
        }

        state = {};
        state.opcode = opcode;
        state.initialized = true;
        state.executing = direct;
        auto& effect = state.effect;
        if (opcode == 2)
        {
            // AMBNT offsets polygon colors, rather than changing the ambient light.
            effect.kind = Kind::color_offset;
            for (size_t i = 0; i < 3; ++i) effect.color[i] = read_s16(params, 2 * i);
            effect.flags = params[6];
        }
        else if (opcode == 3 || opcode == 7)
        {
            const size_t start = opcode == 3 ? 0 : 2;
            effect.kind = Kind::lighting;
            for (size_t i = 0; i < 3; ++i)
            {
                effect.color[i] = params[start + i];
                for (size_t j = 0; j < 3; ++j)
                {
                    effect.light_colors[i][j] = params[start + 3 + 3 * i + j];
                    effect.light_directions[i][j] = read_s16(params, start + 12 + 6 * i + 2 * j) / 4096.0f;
                }
            }
        }
        else if (opcode == 5)
        {
            effect.kind = Kind::positional_lighting;
            for (size_t i = 0; i < 3; ++i)
            {
                effect.light_position[i] = read_s16(params, 2 * i);
                effect.light_colors[0][i] = read_s16(params, 8 + 2 * i);
                effect.color[i] = read_s16(params, 14 + 2 * i);
            }
            effect.light_radius = read_s16(params, 6);
        }
        else if (opcode == 4 || opcode == 9)
        {
            effect.kind = Kind::below_level;
            if (opcode == 4)
            {
                for (size_t i = 0; i < 3; ++i) effect.color[i] = read_s16(params, 2 * i);
                effect.level = read_s16(params, 6);
                effect.flags = params[8];
            }
            else
            {
                effect.level = read_s16(params, 2);
                for (size_t i = 0; i < 3; ++i) effect.color[i] = read_s16(params, 4 + 2 * i);
                effect.flags = params[10];
            }
        }
        else if (opcode == 6 || opcode == 8)
        {
            effect.kind = opcode == 6 ? Kind::color_offset : Kind::below_level;
            effect.color_fade = opcode == 6;
            const size_t channels = opcode == 6 ? 3 : 4;
            for (size_t i = 0; i < channels; ++i)
            {
                state.fade.value[i] = read_s16(params, 2 + 2 * i);
                state.fade.target[i] = read_s16(params, 2 + 2 * channels + 2 * i);
                state.fade.step[i] = read_s16(params, 2 + 4 * channels + 2 * i);
                // PC LIGHT scripts can use positive steps toward negative targets.
                if (opcode == 6 && state.fade.target[i] < 0 && state.fade.step[i] > 0)
                    state.fade.step[i] = signed_word(-state.fade.step[i]);
            }
            effect.flags = params[2 + 6 * channels];
            update_fade_color(state);
        }
        else if (opcode == 0xB)
        {
            effect.kind = Kind::gradient;
            for (size_t i = 0; i < 3; ++i)
            {
                effect.color[i] = read_s16(params, 2 + 2 * i);
                effect.target_color[i] = read_s16(params, 10 + 2 * i);
            }
            effect.level = read_s16(params, 8);
            effect.target_level = read_s16(params, 16);
            effect.flags = params[18];
            if ((effect.flags & 0x18) == 0x18)
            {
                effect.level = signed_word(-static_cast<int>(effect.level));
                effect.target_level = signed_word(-static_cast<int>(effect.target_level));
            }
            effect.gradient_range = signed_word(static_cast<int>(effect.target_level - effect.level));
        }
        else
        {
            effect.kind = Kind::clip;
            effect.plane = *plane;
            effect.level = read_s16(params, 8);
        }
        return true;
    }

    inline void advance(State& state, int frame_multiplier)
    {
        if ((state.opcode != 6 && state.opcode != 8) || !state.executing || state.fade.finished) return;
        if (++state.fade.frame < std::max(frame_multiplier, 1)) return;
        state.fade.frame = 0;
        bool finished = true;
        const size_t channels = state.opcode == 6 ? 3 : 4;
        for (size_t i = 0; i < channels; ++i)
        {
            auto& value = state.fade.value[i];
            const int step = state.fade.step[i];
            value = signed_word(value + step);
            if ((step >= 0 && value >= state.fade.target[i]) || (step < 0 && value <= state.fade.target[i]))
                value = state.fade.target[i];
            else
                finished = false;
        }
        state.fade.finished = finished;
        update_fade_color(state);
    }

    inline float color_coordinate(const Effect& effect, const Vector& position)
    {
        const size_t axis = effect.kind == Kind::below_level ? ((effect.flags & 4) ? 1 : 2) :
            ((effect.flags & 0xC) == 4 ? 1 : ((effect.flags & 0xC) == 8 ? 2 : 0));
        float coordinate = position[axis];
        if (effect.kind == Kind::gradient && (effect.flags & 0x10) && axis != 0) coordinate = -coordinate;
        return coordinate;
    }

    inline Vector color_offset(const Effect& effect, const Vector& position)
    {
        if (effect.kind == Kind::color_offset) return effect.color;
        const float coordinate = color_coordinate(effect, position);
        if (effect.kind == Kind::below_level)
            return coordinate < effect.level ? effect.color : Vector{};
        if (coordinate >= effect.level) return {};
        const auto distance = static_cast<int64_t>(std::trunc(coordinate) - effect.level);
        const auto factor = gte::word(static_cast<int64_t>(gte::long_word(distance * 4096)) / effect.gradient_range);
        gte::Vector start{};
        gte::LongVector far_color{};
        for (size_t i = 0; i < 3; ++i)
        {
            start[i] = gte::word(static_cast<int32_t>(effect.color[i]) * 16);
            far_color[i] = static_cast<int32_t>(effect.target_color[i]) * 16;
        }
        const auto interpolated = gte::interpolate(start, far_color, factor);
        Vector result{};
        for (size_t i = 0; i < 3; ++i)
            result[i] = interpolated[i] / 16.0f;
        return result;
    }

    inline uint32_t add_color(uint32_t color, const Vector& offset)
    {
        uint32_t result = color & 0xFF000000;
        for (size_t i = 0; i < 3; ++i)
        {
            const size_t shift = 16 - 8 * i;
            const int component = static_cast<int>((color >> shift) & 255) + static_cast<int>(std::floor(offset[i]));
            result |= static_cast<uint32_t>(std::clamp(component, 0, 255)) << shift;
        }
        return result;
    }

    inline uint32_t apply_color_offset(const Effect& effect, const Vector& position,
        uint32_t current_color, uint32_t source_color)
    {
        if (effect.kind == Kind::gradient || ((effect.flags & 1) &&
            (effect.kind == Kind::color_offset || color_coordinate(effect, position) < effect.level)))
            current_color = (current_color & 0xFF000000) | (source_color & 0xFFFFFF);
        return add_color(current_color, color_offset(effect, position));
    }

    inline bool gpu_color_offset(const Effect& effect, bool enhanced_lighting)
    {
        return enhanced_lighting && effect.kind == Kind::color_offset;
    }

    inline bool post_lighting_color(const Effect& effect, bool enhanced_lighting)
    {
        return gpu_color_offset(effect, enhanced_lighting) && effect.color_fade;
    }

    inline std::array<float, 4> post_lighting_color_uniform(const Effect& effect, bool enhanced_lighting)
    {
        std::array<float, 4> result{};
        if (post_lighting_color(effect, enhanced_lighting))
        {
            for (size_t i = 0; i < 3; ++i) result[i] = std::floor(effect.color[i]) / 255.0f;
            result[3] = (result[0] != 0 || result[1] != 0 || result[2] != 0) ? 1.0f : 0.0f;
        }
        return result;
    }

    inline Vector triangle_average(const std::array<Vector, 3>& positions)
    {
        Vector average{};
        for (const auto& position : positions)
            for (size_t i = 0; i < 3; ++i) average[i] += std::trunc(position[i]);
        for (float& component : average) component = std::trunc(component / 3.0f);
        return average;
    }

    inline gte::Vector fixed_normal(const Vector& normal)
    {
        gte::Vector result{};
        for (size_t i = 0; i < 3; ++i) result[i] = gte::word(std::llround(normal[i] * 4096));
        return result;
    }

    inline gte::Matrix light_directions(const Effect& effect)
    {
        gte::Matrix result{};
        for (size_t i = 0; i < 3; ++i)
            result[i] = fixed_normal(effect.light_directions[i]);
        return result;
    }

    inline uint32_t light_color(uint32_t color, const Effect& effect, const gte::Vector& normal,
        const gte::Matrix& directions)
    {
        gte::Matrix colors{};
        gte::LongVector background{};
        for (size_t i = 0; i < 3; ++i)
        {
            background[i] = static_cast<int32_t>(effect.color[i] * 16);
            for (size_t j = 0; j < 3; ++j) colors[i][j] = gte::word(static_cast<int32_t>(effect.light_colors[j][i] * 16));
        }
        return gte::normal_color(color, directions, colors, background, normal);
    }

    inline uint32_t light_color(uint32_t color, const Effect& effect, const Vector& normal)
    {
        return light_color(color, effect, fixed_normal(normal), light_directions(effect));
    }

    inline uint32_t enhanced_light_color(uint32_t color, const Effect& effect, const Vector& normal)
    {
        Vector light = effect.color;
        for (size_t i = 0; i < 3; ++i)
        {
            const float intensity = std::max(dot(normal, effect.light_directions[i]), 0.0f);
            for (size_t j = 0; j < 3; ++j) light[j] += intensity * effect.light_colors[i][j];
        }
        uint32_t result = color & 0xFF000000;
        for (size_t i = 0; i < 3; ++i)
        {
            const size_t shift = 16 - 8 * i;
            const int component = static_cast<int>(((color >> shift) & 255) * std::clamp(light[i], 0.0f, 2047.0f) / 128.0f);
            result |= static_cast<uint32_t>(std::clamp(component, 0, 255)) << shift;
        }
        return result;
    }

    inline bool project_to_plane(Vector& position, const Effect& effect)
    {
        gte::Vector vertex{};
        for (size_t i = 0; i < 3; ++i) vertex[i] = gte::word(static_cast<int32_t>(position[i]));
        auto projected = gte::transform_ir(gte::to_plane(effect.plane, {}), vertex);
        const bool visible = gte::project(projected, effect.plane);
        const auto world = gte::transform(gte::from_plane(effect.plane, {}), projected);
        for (size_t i = 0; i < 3; ++i) position[i] = static_cast<float>(world[i]);
        return visible;
    }
}

template <typename T> struct vector3;
struct matrix;
struct polygon_data;
struct nvertex;
struct struc_110;
struct ff7_hrc_polygon_data;
struct ff7_polygon_set;
struct indexed_primitive;
struct light_data;

namespace ff7::field
{
    class KawaiModelScope
    {
        int previous_model;
    public:
        explicit KawaiModelScope(struc_110* instance);
        ~KawaiModelScope();
        KawaiModelScope(const KawaiModelScope&) = delete;
        KawaiModelScope& operator=(const KawaiModelScope&) = delete;
    };

    bool queue_kawai_effect(int model_id, uint8_t opcode, std::span<const uint8_t> params);
    bool apply_kawai_effect(int model_id, int opcode, ff7_hrc_polygon_data* model, int* result, int* complete);
    void advance_kawai_effects(int frame_multiplier);
    void reset_kawai_effects();
    bool bind_kawai_effect(polygon_data* polygon);
    bool has_kawai_splash_color();
    bool draw_kawai_model(polygon_data* polygon, indexed_primitive* primitive,
        std::span<const vector3<float>> normals, light_data* light, uint32_t clip,
        std::span<const uint32_t> source_colors = {});

    struct KawaiGeometry
    {
        std::vector<nvertex> vertices;
        std::vector<vector3<float>> normals;
    };

    KawaiGeometry process_kawai_geometry(std::span<const nvertex> vertices,
        std::span<const vector3<float>> normals, std::span<const uint32_t> indices,
        std::span<const uint32_t> source_colors = {}, light_data* light = nullptr);
}
