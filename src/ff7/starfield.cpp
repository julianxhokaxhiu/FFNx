/****************************************************************************/
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

#include "starfield.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <numeric>
#include <vector>

#include "cfg.h"
#include "common.h"
#include "ff7/defs.h"
#include "ff7/widescreen.h"
#include "gamehacks.h"
#include "gl.h"
#include "globals.h"
#include "log.h"
#include "patch.h"
#include "renderer.h"

namespace ff7::starfield
{
namespace
{
    constexpr int star_rows = 16;
    constexpr double tick_duration = 1.0 / 30.0;
    constexpr uint32_t prelude_music_id = 95;
    using Clock = std::chrono::steady_clock;
    using SoundOperation = uint32_t (*)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);

    struct Star
    {
        int x;
        int y;
        float z;
    };

    std::vector<Star> stars;
    std::vector<Star> previous_stars;
    std::vector<size_t> draw_order;
    main_obj native_loop;
    main_obj_fn *native_credits_exit;
    int (*update_input)(ff7_game_obj *);
    Clock::time_point last_tick;
    double accumulated_time;
    bool alternate_bank;
    bool input_ready;
    bool active;

    void initialize_stars(uint32_t seed)
    {
        const float width = widescreen_enabled ? static_cast<float>(wide_viewport_width) : static_cast<float>(game_width);
        const int columns = 2 * static_cast<int>(std::ceil(8.0f * width / game_width));
        stars.resize(static_cast<size_t>(columns) * star_rows);
        draw_order.resize(stars.size());
        for (size_t i = 0; i < stars.size(); ++i)
        {
            seed = seed * 1103515245u + 12345u;
            stars[i] = {
                -(columns / 2) * 0x400 + static_cast<int>(i % columns) * 0x400,
                -0x3ED + static_cast<int>(i / columns) * 0x100,
                static_cast<float>(static_cast<int>((seed >> 16) & 0x7FFF) / 2 + 0x4000)
            };
        }
        std::iota(draw_order.begin(), draw_order.end(), size_t{0});
        previous_stars = stars;
        alternate_bank = false;
    }

    void advance_stars()
    {
        previous_stars = stars;
        alternate_bank = !alternate_bank;
        const int step = alternate_bank ? 0x20 : 0x80;
        for (auto &star : stars)
        {
            star.z -= step;
            if (star.z < 0) star.z = 0x4000;
        }
    }

    Star interpolate_star(const Star &previous, const Star &current, float alpha)
    {
        Star result = current;
        // A respawn is a teleport, not a backwards flight through the starfield.
        if (previous.z >= current.z) result.z = std::lerp(previous.z, current.z, alpha);
        return result;
    }

    float project(int coordinate, float depth, int center, bool smooth)
    {
        // GTE perspective division saturates at 2, including vertices behind the camera.
        const float scale = depth <= 0x100 ? 2.0f : 512.0f / depth;
        const float projected = coordinate * scale;
        return std::clamp((smooth ? projected : std::floor(projected)) + center, -1024.0f, 1023.0f);
    }

    void build_streak(const Star &star, nvertex *vertices, float scale_x, float scale_y, float center_x, bool smooth)
    {
        const float x0 = project(star.x, star.z - 0x400, 320, smooth);
        const float y0 = project(star.y, star.z - 0x400, 120, smooth);
        const float x1 = project(star.x, star.z - 0x600, 320, smooth);
        const float y1 = project(star.y, star.z - 0x600, 120, smooth);
        const float dx = x1 - x0;
        const float dy = y1 - y0;
        const float length = std::sqrt(dx * dx + dy * dy);
        const float offset_x = length > 0.0f ? -dy / (2.0f * length) : 0.5f;
        const float offset_y = length > 0.0f ? dx / (2.0f * length) : 0.0f;
        const float end_y = length > 0.0f ? y1 : y1 + 1.0f;
        const float translate_x = center_x - 320.0f * scale_x;
        const int depth = static_cast<int>(star.z) >> 6;
        const uint32_t color = 0xFF000000u
            | (static_cast<uint32_t>(std::max(0, 0xA0 - depth)) << 16)
            | (static_cast<uint32_t>(std::max(0, 0xDC - depth)) << 8)
            | static_cast<uint32_t>(std::max(0, 0xFA - depth));

        // Quads retain a one-PSX-pixel width when the internal rendering resolution changes.
        vertices[0] = {{(x0 + offset_x) * scale_x + translate_x, (y0 + offset_y) * scale_y, 0.0f},
            {1.0f, 0xFF000000u, 0}, 0.0f, 0.0f};
        vertices[1] = {{(x0 - offset_x) * scale_x + translate_x, (y0 - offset_y) * scale_y, 0.0f},
            {1.0f, 0xFF000000u, 0}, 0.0f, 0.0f};
        vertices[2] = {{(x1 + offset_x) * scale_x + translate_x, (end_y + offset_y) * scale_y, 0.0f},
            {1.0f, color, 0}, 0.0f, 0.0f};
        vertices[3] = {{(x1 - offset_x) * scale_x + translate_x, (end_y - offset_y) * scale_y, 0.0f},
            {1.0f, color, 0}, 0.0f, 0.0f};
    }

    void draw(game_obj *game_object)
    {
        const bool smooth = ff7_fps_limiter >= FPS_LIMITER_60FPS;
        auto rendered_stars = stars;
        if (smooth)
        {
            const float alpha = static_cast<float>(accumulated_time / tick_duration);
            for (size_t i = 0; i < stars.size(); ++i)
                rendered_stars[i] = interpolate_star(previous_stars[i], stars[i], alpha);
        }
        std::vector<nvertex> vertices(stars.size() * 4);
        std::vector<WORD> indices(stars.size() * 6);
        const float width = widescreen_enabled ? static_cast<float>(wide_viewport_width) : static_cast<float>(game_width);
        const float left = widescreen_enabled ? static_cast<float>(wide_viewport_x) : 0.0f;
        const float height = widescreen_enabled ? static_cast<float>(wide_game_height) : static_cast<float>(game_height);
        // PSX ClearOTag traverses near-to-far, with AddPrim reversing each bucket's insertion order.
        std::sort(draw_order.begin(), draw_order.end(), [&rendered_stars](size_t left, size_t right)
        {
            const int left_bucket = std::max(0, static_cast<int>((3 * rendered_stars[left].z - 0xE00) / 12)) >> 6;
            const int right_bucket = std::max(0, static_cast<int>((3 * rendered_stars[right].z - 0xE00) / 12)) >> 6;
            return left_bucket == right_bucket ? left > right : left_bucket < right_bucket;
        });
        for (size_t i = 0; i < stars.size(); ++i)
        {
            build_streak(rendered_stars[draw_order[i]], vertices.data() + i * 4,
                game_width / 640.0f, height / 240.0f, left + width / 2.0f, smooth);
            const WORD base = static_cast<WORD>(i * 4);
            const WORD quad[] = {base, static_cast<WORD>(base + 1), static_cast<WORD>(base + 2),
                static_cast<WORD>(base + 1), static_cast<WORD>(base + 3), static_cast<WORD>(base + 2)};
            std::copy(std::begin(quad), std::end(quad), indices.begin() + i * 6);
        }

        newRenderer.setBackgroundColor(0.0f, 0.0f, 0.0f);
        common_clear_all(game_object);
        common_begin_scene(0, game_object);
        const bool time_filter = newRenderer.isTimeFilterEnabled();
        const bool fog = newRenderer.isFogEnabled();
        newRenderer.setTimeFilterEnabled(false);
        newRenderer.setFogEnabled(false);
        gl_bind_texture_set(nullptr);
        current_state.fb_texture = false;
        current_state.kawai_effect = {};
        gl_set_blend_func(static_cast<uint32_t>(RendererBlendMode::BLEND_DISABLED));
        internal_set_renderstate(V_NOCULL, true, game_object);
        internal_set_renderstate(V_DEPTHTEST, false, game_object);
        internal_set_renderstate(V_DEPTHMASK, false, game_object);
        internal_set_renderstate(V_ALPHATEST, false, game_object);
        internal_set_renderstate(V_SHADEMODE, true, game_object);
        gl_draw_indexed_primitive(static_cast<uint32_t>(RendererPrimitiveType::PT_TRIANGLES), TLVERTEX,
            vertices.data(), nullptr, static_cast<uint32_t>(vertices.size()),
            indices.data(), static_cast<uint32_t>(indices.size()), nullptr, nullptr, nullptr, false, false);
        common_end_scene(game_object);
        newRenderer.setTimeFilterEnabled(time_filter);
        newRenderer.setFogEnabled(fog);
    }

    void *enter(game_obj *game_object)
    {
        last_tick = Clock::now();
        initialize_stars(static_cast<uint32_t>(last_tick.time_since_epoch().count()));
        accumulated_time = tick_duration;
        input_ready = false;
        active = true;
        update_input(reinterpret_cast<ff7_game_obj *>(game_object));
        const auto sound_operation = reinterpret_cast<SoundOperation>(ff7_externals.sound_operation);
        sound_operation(0x10, prelude_music_id, 0, 0, 0, 0);
        sound_operation(0xC0, 127, 0, 0, 0, 0);
        if (trace_all) ffnx_trace("Post-credits starfield started\n");
        return nullptr;
    }

    void *exit(game_obj *game_object)
    {
        active = false;
        input_ready = false;
        reinterpret_cast<SoundOperation>(ff7_externals.sound_operation)(0xF0, 0, 0, 0, 0, 0);
        if (trace_all) ffnx_trace("Post-credits starfield stopped\n");
        return nullptr;
    }

    void *main_loop(game_obj *game_object)
    {
        ff7_limit_fps();
        auto *ff7_game_object = reinterpret_cast<ff7_game_obj *>(game_object);
        update_input(ff7_game_object);
        if (!ff7_externals.get_button_held(-1)) input_ready = true;
        if (input_ready && ff7_externals.get_button_pressed(-1))
        {
            ff7_externals.engine_switch_game_loop_sub_666CF2(&native_loop, ff7_game_object);
            if (ff7_game_object->field_924 && ff7_game_object->game_loop_obj.main_loop == native_loop.main_loop)
            {
                *common_externals._mode = FF7_MODE_MAIN_MENU;
                input_ready = false;
            }
            else ffnx_error("Could not return from the post-credits starfield to the title screen\n");
        }

        const auto now = Clock::now();
        const double elapsed = std::chrono::duration<double>(now - last_tick).count();
        last_tick = now;
        accumulated_time += std::min(elapsed * gamehacks.getCurrentSpeedhack(), 0.25);
        while (accumulated_time >= tick_duration)
        {
            advance_stars();
            accumulated_time -= tick_duration;
        }
        draw(game_object);
        return nullptr;
    }

    void *credits_exit(game_obj *game_object)
    {
        auto *ff7_game_object = reinterpret_cast<ff7_game_obj *>(game_object);
        const bool completed = ff7_game_object->field_924
            && *common_externals._mode == FF7_MODE_UNKNOWN28
            && reinterpret_cast<uint32_t>(ff7_game_object->game_loop_obj.main_loop) != ff7_externals.credits_main_loop;
        native_credits_exit(game_object);
        if (completed)
        {
            // Cleanup has released credits and ending3; replace only the queued dispatcher loop.
            native_loop = ff7_game_object->game_loop_obj;
            ff7_game_object->game_loop_obj.enter_main = enter;
            ff7_game_object->game_loop_obj.exit_main = exit;
            ff7_game_object->game_loop_obj.main_loop = main_loop;
            ff7_game_object->game_loop_obj.field_14 = nullptr;
            ff7_game_object->game_loop_obj.field_18 = nullptr;
            *common_externals._mode = FF7_MODE_UNKNOWN28;
        }
        return nullptr;
    }
}

bool is_active()
{
    return active;
}

void init(ff7_game_obj *game_object)
{
    const auto dispatcher = reinterpret_cast<uint32_t>(game_object->engine_loop_obj.main_loop);
    const uint32_t credits_exit_address = get_absolute_value(dispatcher, 0x4DF);
    if (get_absolute_value(dispatcher, 0x5DF) != credits_exit_address
        || get_absolute_value(dispatcher, 0x5CA) != ff7_externals.credits_main_loop)
    {
        ffnx_error("Post-credits starfield disabled: unexpected ending dispatcher hooks\n");
        return;
    }
    native_credits_exit = reinterpret_cast<main_obj_fn *>(credits_exit_address);
    update_input = reinterpret_cast<int (*)(ff7_game_obj *)>(get_relative_call(ff7_externals.credits_main_loop, 0x44));
    patch_code_uint(dispatcher + 0x5DF, reinterpret_cast<uint32_t>(credits_exit));
}
}
