/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2026 Julian Xhokaxhiu                                   //
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

#include "../../globals.h"
#include "../../utils.h"
#include "../../log.h"

#include "kawai_eyetx.h"
#include "kawai.h"

#include <array>
#include <unordered_map>

namespace ff7::field
{
    namespace
    {
        struct ff7_model_custom_data
        {
            int has_mouth;
            int current_mouth_idx;
            p_hundred *mouth_tex;
            p_hundred *left_eye_tex;
            p_hundred *right_eye_tex;
        };

        ff7_model_custom_data ff7_model_data[FF7_MAX_NUM_MODEL_ENTITIES]{};
        std::array<int, FF7_MAX_NUM_MODEL_ENTITIES> blink_frames{};
        constexpr int BLINKING_FRAMES = 4;
    }

    void ff7_field_initialize_eyetx()
    {
        for (auto &custom : ff7_model_data)
        {
            custom.current_mouth_idx = 0;
            custom.mouth_tex = nullptr;
        }
    }

    void ff7_field_reset_blink_frames()
    {
        blink_frames.fill(BLINKING_FRAMES);
    }

    void ff7_field_set_mouth_index(byte model_id, byte mouth_index)
    {
        if (model_id >= FF7_MAX_NUM_MODEL_ENTITIES)
        {
            ffnx_error("subcode[EYETX]: Invalid model ID %u for mouth index\n", model_id);
            return;
        }
        ff7_model_data[model_id].current_mouth_idx = mouth_index;
    }

    namespace
    {
        struct facial_texture
        {
            std::string filename;
            p_hundred *texture = nullptr;
            bool custom_found = false;
        };

        struct facial_texture_cache
        {
            bool initialized = false;
            bool valid = false;
            bool has_eyes = false;
            bool eyes_selected = false;
            bool is_npc = false;
            byte eye_index = MAXBYTE;
            const field_animation_data *animation = nullptr;
            const ff7_hrc_polygon_data *frame_object = nullptr;
            p_hundred *native_left_eye = nullptr;
            p_hundred *native_right_eye = nullptr;
            std::string directory;
            std::string model_filename;
            std::string model_name;
            std::string default_left_filename;
            std::string default_right_filename;
            std::string left_basename;
            std::string right_basename;
            std::string left_extension;
            std::string right_extension = ".TEX";
            std::string mouth_basename;
            std::unordered_map<byte, facial_texture> left_eyes;
            std::unordered_map<byte, facial_texture> right_eyes;
            std::unordered_map<int, facial_texture> mouths;
            std::unordered_map<std::string, p_hundred *> owned_textures;
        };

        std::array<facial_texture_cache, FF7_MAX_NUM_MODEL_ENTITIES> facial_textures;
        bool facial_field_initialized = false;
        WORD facial_field_id = 0;
        std::string facial_basedir;
        std::string facial_direct_path;
        std::string facial_directory;

        void clear_facial_textures(int model_id)
        {
            auto &custom = ff7_model_data[model_id];
            custom.left_eye_tex = nullptr;
            custom.right_eye_tex = nullptr;
            custom.mouth_tex = nullptr;
            custom.has_mouth = 0;

            auto &cache = facial_textures[model_id];
            for (const auto &[filename, texture] : cache.owned_textures)
                if (texture) ff7_externals.field_unload_model_tex(texture);
            cache = {};
        }

        void ensure_facial_texture_field()
        {
            if (facial_field_initialized && facial_field_id == *common_externals.current_field_id &&
                facial_basedir == basedir && facial_direct_path == direct_mode_path)
                return;

            // Path changes are handled per model when rebound, so other live overrides stay valid.
            if (!facial_field_initialized || facial_field_id != *common_externals.current_field_id)
            {
                for (int i = 0; i < FF7_MAX_NUM_MODEL_ENTITIES; ++i)
                    clear_facial_textures(i);
            }

            facial_field_initialized = true;
            facial_field_id = *common_externals.current_field_id;
            facial_basedir = basedir;
            facial_direct_path = direct_mode_path;
            facial_directory = facial_basedir + "/" + facial_direct_path + "/flevel/";
        }

        bool split_facial_filename(const char *path, std::string &basename, std::string &extension)
        {
            char name[_MAX_FNAME];
            char ext[_MAX_EXT];
            if (_splitpath_s(path, nullptr, 0, nullptr, 0, name, sizeof(name), ext, sizeof(ext)))
            {
                ffnx_error("subcode[EYETX]: Invalid facial texture filename: %s\n", path);
                return false;
            }
            basename = name;
            extension = ext;
            return true;
        }

        facial_texture_cache &get_facial_texture_cache(int model_id, field_animation_data *animation)
        {
            auto &cache = facial_textures[model_id];
            const char *model_filename = (const char *)(*ff7_externals.field_models_data + 10380 * model_id + 512);
            const auto &defaults = ff7_externals.field_models_eye_blink_buffer[animation->eye_texture_idx == 9 ? 0 : animation->eye_texture_idx];
            const char *left_filename = defaults.static_left_eye_filename ? defaults.static_left_eye_filename : "";
            const char *right_filename = defaults.static_right_eye_filename ? defaults.static_right_eye_filename : "";

            if (cache.initialized && cache.animation == animation && cache.frame_object == animation->anim_frame_object &&
                cache.native_left_eye == animation->static_left_eye_tex && cache.native_right_eye == animation->static_right_eye_tex &&
                cache.directory == facial_directory && cache.model_filename == model_filename && cache.eye_index == animation->eye_texture_idx &&
                cache.has_eyes == !!defaults.has_eyes &&
                cache.default_left_filename == left_filename && cache.default_right_filename == right_filename)
                return cache;

            clear_facial_textures(model_id);
            cache.initialized = true;
            cache.animation = animation;
            cache.frame_object = animation->anim_frame_object;
            cache.native_left_eye = animation->static_left_eye_tex;
            cache.native_right_eye = animation->static_right_eye_tex;
            cache.directory = facial_directory;
            cache.model_filename = model_filename;
            cache.eye_index = animation->eye_texture_idx;
            cache.has_eyes = !!defaults.has_eyes;
            cache.is_npc = cache.eye_index == 9;
            cache.default_left_filename = left_filename;
            cache.default_right_filename = right_filename;

            std::string extension;
            if (!split_facial_filename(model_filename, cache.model_name, extension))
                return cache;
            if (cache.has_eyes && (!*left_filename || !*right_filename))
            {
                ffnx_error("subcode[EYETX]: Missing default eye filenames for model %s\n", cache.model_name.c_str());
                return cache;
            }
            if (*left_filename && !split_facial_filename(left_filename, cache.left_basename, cache.left_extension))
                return cache;
            if (*right_filename && !split_facial_filename(right_filename, cache.right_basename, cache.right_extension))
                return cache;
            cache.mouth_basename = cache.is_npc ? "npc" : cache.right_basename.substr(0, cache.right_basename.find('_'));
            cache.valid = true;
            return cache;
        }

        bool facial_texture_exists(const std::string &basename, const char *part, bool trace_missing)
        {
            const std::string path = facial_directory + basename + ".TEX";
            const bool found = fileExists(path.c_str());
            if (!found && trace_missing && (trace_all || trace_direct || trace_opcodes))
                ffnx_trace("subcode[EYETX]: Custom %s texture not found: %s\n", part, path.c_str());
            return found;
        }

        p_hundred *load_facial_texture(facial_texture_cache &cache, std::string &filename, bool eye)
        {
            auto [entry, inserted] = cache.owned_textures.try_emplace(filename, nullptr);
            if (!inserted) return entry->second;

            if (eye)
            {
                // Native field teardown owns the original eyes, not these cached expressions.
                ff7_model_eye_texture_data eye_data{};
                field_animation_data temporary_animation{};
                eye_data.has_eyes = 1;
                eye_data.static_left_eye_filename = filename.data();
                ff7_externals.field_load_model_eye_tex(&eye_data, &temporary_animation);
                entry->second = temporary_animation.static_left_eye_tex;
            }
            else
            {
                struc_3 info;
                ff7_externals.create_struc_3_info_sub_67455E(&info);
                info.base_directory = (uint32_t)ff7_externals.field_unk_909288;
                info.file_context.use_lgp = 1;
                info.file_context.lgp_num = 1;
                info.file_context.name_mangler = 0;
                entry->second = ff7_externals.field_load_model_tex(0, 0, filename.data(), &info, common_externals.get_game_object());
            }
            if (!entry->second)
                ffnx_error("subcode[EYETX]: Could not load facial texture: %s\n", filename.c_str());
            return entry->second;
        }

        const facial_texture &get_facial_eye(facial_texture_cache &cache, byte index, bool right)
        {
            auto &eyes = right ? cache.right_eyes : cache.left_eyes;
            auto [entry, inserted] = eyes.try_emplace(index);
            auto &selection = entry->second;
            if (!inserted) return selection;

            const char *part = right ? "right eye" : "left eye";
            const auto &basename = right ? cache.right_basename : cache.left_basename;
            const auto &extension = right ? cache.right_extension : cache.left_extension;
            std::string candidate = "eye_" + cache.model_name + (right ? "r_" : "_") + std::to_string(index);
            selection.custom_found = facial_texture_exists(candidate, part, index > 2);
            if (!selection.custom_found)
            {
                candidate = (cache.is_npc ? (right ? "npc_eye2r" : "npc_eye2") : basename) + std::string("_") + std::to_string(index);
                selection.custom_found = facial_texture_exists(candidate, part, index > 2);
            }
            selection.filename = (selection.custom_found ? candidate : basename) + extension;
            p_hundred *native_eye = right ? cache.native_right_eye : cache.native_left_eye;
            if (!selection.custom_found && !cache.is_npc && native_eye)
                selection.texture = native_eye;
            else
                selection.texture = load_facial_texture(cache, selection.filename, true);
            return selection;
        }

        const facial_texture &get_facial_mouth(facial_texture_cache &cache, int index)
        {
            auto [entry, inserted] = cache.mouths.try_emplace(index);
            auto &selection = entry->second;
            if (!inserted) return selection;

            std::string candidate = "mouth_" + cache.model_name + "_" + std::to_string(index);
            selection.custom_found = facial_texture_exists(candidate, "mouth", index > 0);
            if (!selection.custom_found && !cache.mouth_basename.empty())
            {
                candidate = cache.mouth_basename + "_mouth_" + std::to_string(index);
                selection.custom_found = facial_texture_exists(candidate, "mouth", index > 0);
            }
            if (selection.custom_found)
            {
                selection.filename = candidate + cache.right_extension;
                selection.texture = load_facial_texture(cache, selection.filename, false);
            }
            return selection;
        }
    }

    void ff7_handle_KAWAI_reset()
    {
        static WORD last_field_id = 0;

        if (last_field_id != *common_externals.current_field_id)
        {
            last_field_id = *common_externals.current_field_id;
            reset_kawai_effects();
            ensure_facial_texture_field();

            // Reset eyes blinking
            ff7_externals.field_model_blink_data_D000C8->blink_left_eye_mode = 0;
            ff7_externals.field_model_blink_data_D000C8->blink_right_eye_mode = 0;

            for(int i = 0; i < FF7_MAX_NUM_MODEL_ENTITIES; i++)
            {
                // Reset mouths
                ff7_model_data[i].current_mouth_idx = 0;
                ff7_model_data[i].mouth_tex = nullptr;
                ff7_model_data[i].has_mouth = 0;
            }
        }
    }

    int ff7_internal_blink_eye(field_animation_data *field_anim_data, field_model_blink_data *blink_data)
    {
        int i;
        ff7_polygon_set *polygon_set = nullptr;
        struct hrc_bone *bones;
        hrc_data *hrc_data;
        int blink_left_eye_mode;
        int blink_right_eye_mode;
        const bool cached_eyes = ff7_advanced_blinking && facial_textures[blink_data->model_id].eyes_selected;
        p_hundred *left_eye = cached_eyes ? ff7_model_data[blink_data->model_id].left_eye_tex : field_anim_data->static_left_eye_tex;
        p_hundred *right_eye = cached_eyes ? ff7_model_data[blink_data->model_id].right_eye_tex : field_anim_data->static_right_eye_tex;

        if ( field_anim_data->anim_frame_object )
        {
            hrc_data = field_anim_data->anim_frame_object->hrc_data;
            if ( hrc_data )
            {
                int bone_idx = 0;
                bones = field_anim_data->anim_frame_object->hrc_data->bones;
                if ( (char)field_anim_data->eye_texture_idx >= 33 )
                    return 1;
                while ( bone_idx < (signed int)hrc_data->num_bones && _strcmpi(bones->bone_name, "head") )
                {
                    ++bone_idx;
                    ++bones;
                }
                if ( bone_idx == hrc_data->num_bones ) return 0;
                polygon_set = bones->rsd_array->rsd_data->polygon_set;
                ff7_externals.field_sub_6A2736(polygon_set);
                // Mouth replacement logic
                if (ff7_advanced_blinking && polygon_set && polygon_set->numgroups > 3 && ff7_model_data[blink_data->model_id].mouth_tex)
                {
                    if (polygon_set->hundred_data_group_array[3] != NULL)
                    {
                        polygon_set->hundred_data_group_array[3] = ff7_model_data[blink_data->model_id].mouth_tex;
                        polygon_set->per_group_hundreds = 1;
                    }
                }
                blink_left_eye_mode = blink_data->blink_left_eye_mode;
                for ( i = 0; i < (signed int)polygon_set->numgroups && !polygon_set->hundred_data[i].texture_set; ++i );
                if ( i == polygon_set->numgroups )return 0;
                if ( blink_left_eye_mode == 1 )
                {
                    if (ff7_advanced_blinking && polygon_set->numgroups > 1 && ff7_model_data[blink_data->model_id].left_eye_tex) polygon_set->hundred_data_group_array[1] = ff7_model_data[blink_data->model_id].left_eye_tex;
                    ff7_externals.field_sub_6A2782(i, &polygon_set->hundred_data[i], polygon_set);
                }
                else
                {
                    if ( blink_left_eye_mode != 2 )
                    {
                        if (ff7_advanced_blinking && !ff7_model_data[blink_data->model_id].has_mouth) polygon_set->per_group_hundreds = 0;
                        return 0;
                    }
                    polygon_set->per_group_hundreds = 1;
                    if ( !left_eye )
                    goto LABEL_22;
                    ff7_externals.field_sub_6A2782(i, left_eye, polygon_set);
                }
                blink_right_eye_mode = blink_data->blink_right_eye_mode;
                if ( ++i == polygon_set->numgroups ) return 0;
                if ( blink_right_eye_mode == 1 )
                {
                    if (ff7_advanced_blinking && polygon_set->numgroups > 2 && ff7_model_data[blink_data->model_id].right_eye_tex) polygon_set->hundred_data_group_array[2] = ff7_model_data[blink_data->model_id].right_eye_tex;
                    ff7_externals.field_sub_6A2782(i, &polygon_set->hundred_data[i], polygon_set);
                    return 1;
                }
                if ( blink_right_eye_mode != 2 )
                {
                    if (ff7_advanced_blinking && !ff7_model_data[blink_data->model_id].has_mouth) polygon_set->per_group_hundreds = 0;
                    return 0;
                }
                polygon_set->per_group_hundreds = 1;
                if ( right_eye )
                {
                    ff7_externals.field_sub_6A2782(i, right_eye, polygon_set);
                    return 1;
                }
            LABEL_22:
                ff7_externals.field_sub_6A2782(i, &polygon_set->hundred_data[i], polygon_set);
                return 0;
            }
        }

        return 0;
    }

    int ff7_field_blink_eye_sub_649B50(field_animation_data *field_anim_data, field_model_blink_data *blink_data)
    {
        const byte model_id = blink_data->model_id;
        if (model_id >= FF7_MAX_NUM_MODEL_ENTITIES)
        {
            ffnx_error("field_blink_eye_sub: Invalid model ID %u\n", model_id);
            return 0;
        }

        // A synchronous EYETX callback can run before the regular field reset.
        ensure_facial_texture_field();
        if (ff7_advanced_blinking)
        {
            if (field_anim_data->eye_texture_idx < 10)
            {
                auto &cache = get_facial_texture_cache(model_id, field_anim_data);
                auto &custom = ff7_model_data[model_id];
                const byte left_index = blink_data->blink_left_eye_mode;
                const byte right_index = blink_data->blink_right_eye_mode;
                const int mouth_index = custom.current_mouth_idx;

                if (trace_all || trace_opcodes)
                    ffnx_trace("field_blink_eye_sub: curr_entity_id=%u,curr_model_id=%u,curr_eye_index=%u,curr_mouth_index=%u,curr_model_name=%s\n",
                        *ff7_externals.current_entity_id, model_id, field_anim_data->eye_texture_idx, mouth_index, cache.model_name.c_str());

                if (cache.valid)
                {
                    bool left_found = false, right_found = false;
                    if (cache.has_eyes)
                    {
                        const auto &left = get_facial_eye(cache, left_index, false);
                        const auto &right = get_facial_eye(cache, right_index, true);
                        left_found = left.custom_found;
                        right_found = right.custom_found;
                        custom.left_eye_tex = left.texture;
                        custom.right_eye_tex = right.texture;
                        cache.eyes_selected = true;
                    }
                    const auto &mouth = get_facial_mouth(cache, mouth_index);
                    custom.mouth_tex = mouth.texture;
                    custom.has_mouth = mouth.texture != nullptr;

                    // Keep native modes for mixed requests; extended expressions use the closed-eye path.
                    if (left_index > 2 && right_index > 2 && (left_found || right_found))
                    {
                        blink_data->blink_left_eye_mode = 2;
                        blink_data->blink_right_eye_mode = 2;
                    }
                }
            }
            else if (facial_textures[model_id].initialized)
                clear_facial_textures(model_id);
        }

        // Original code
        int ret = ff7_internal_blink_eye(field_anim_data, blink_data);

        // 60FPS patch: wait time and blink time
        if(ff7_fps_limiter == FPS_LIMITER_60FPS)
        {
            if(blink_data->blink_left_eye_mode == 2 && blink_data->blink_right_eye_mode == 2)
            {
                auto &field_event_data = (*ff7_externals.field_event_data_ptr)[blink_data->model_id];
                if(blink_frames[blink_data->model_id] > 0)
                {
                    field_event_data.blink_wait_frames = 0;
                    blink_frames[blink_data->model_id]--;
                }
                else
                {
                    field_event_data.blink_wait_frames = 64 * common_frame_multiplier + getRandomInt(0, 32 * common_frame_multiplier);
                    blink_frames[blink_data->model_id] = BLINKING_FRAMES;
                }
            }
        }

        return ret;
    }

    int ff7_field_models_eye_to_model(char* model_name)
    {
        // Cloud
        if ( !_strcmpi(model_name, "AAAA") )
            return 0;
        if ( !_strcmpi(model_name, "AFIE") )
            return 0;
        if ( !_strcmpi(model_name, "BHFF") )
            return 0;
        if ( !_strcmpi(model_name, "BUGE") )
            return 0;
        if ( !_strcmpi(model_name, "DLFB") )
            return 0;
        if ( !_strcmpi(model_name, "EIHD") )
            return 0;
        if ( !_strcmpi(model_name, "EKBF") )
            return 0;
        if ( !_strcmpi(model_name, "ENAB") )
            return 0;
        if ( !_strcmpi(model_name, "HTJE") )
            return 0;

        // Tifa
        if ( !_strcmpi(model_name, "AAGB") )
            return 1;
        if ( !_strcmpi(model_name, "AXJA") )
            return 1;
        if ( !_strcmpi(model_name, "EQIB") )
            return 1;
        if ( !_strcmpi(model_name, "BIDB") )
            return 1;
        if ( !_strcmpi(model_name, "AGGB") )
            return 1;
        if ( !_strcmpi(model_name, "BUAC") )
            return 1;

        // Aerith
        if ( !_strcmpi(model_name, "AUFF") )
            return 2;
        if ( !_strcmpi(model_name, "CAHC") )
            return 2;
        if ( !_strcmpi(model_name, "AZBB") )
            return 2;
        if ( !_strcmpi(model_name, "CQGA") )
            return 2;
        if ( !_strcmpi(model_name, "DIFF") )
            return 2;
        if ( !_strcmpi(model_name, "CPJF") ) // Ifalna - shares the same eyes
            return 2;

        // Barret
        if ( !_strcmpi(model_name, "ACGD") )
            return 3;
        if ( !_strcmpi(model_name, "FQCB") )
            return 3;
        if ( !_strcmpi(model_name, "AYFB") )
            return 3;
        if ( !_strcmpi(model_name, "AIBA") )
            return 3;

        // Red XIII
        if ( !_strcmpi(model_name, "ADDA") )
            return 4;
        if ( !_strcmpi(model_name, "HVJF") )
            return 4;

        // Cid
        if ( !_strcmpi(model_name, "ABDA") )
            return 5;
        if ( !_strcmpi(model_name, "AIHB") )
            return 5;

        // Vincent
        if ( !_strcmpi(model_name, "AEHD") )
            return 6;
        if ( !_strcmpi(model_name, "BIJD") )
            return 6;

        // Yuffie
        if ( !_strcmpi(model_name, "ABJB") )
            return 7;
        if ( !_strcmpi(model_name, "FEEA") )
            return 7;
        if ( !_strcmpi(model_name, "AHDF") )
            return 7;

        // Cait Sith
        if ( !_strcmpi(model_name, "AEBC") )
            return 8;

        return 9; // Defaults to Cloud eye
    }
}
