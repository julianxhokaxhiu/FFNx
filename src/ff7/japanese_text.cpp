/****************************************************************************/
//    Copyright (C) 2024 Cosmos                                             //
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
#include "../globals.h"
#include "../log.h"
#include "../ff7.h"
#include "../gl.h"
#include "../patch.h"
#include "../redirect.h"
#include "../renderer.h"
#include "../utils.h"
#include <string.h>

static void multibyte_load_widths();
int common_submit_draw_char_from_buffer_6F564E_jp(int x, int vertex_y, int n_shapes, uint16_t letter, float z_value);
static bool jp_small_glyphs = true;
static ff7_graphics_object* jp_prompt_graphics_object = nullptr;
static ff7_texture_set* jp_prompt_original_texture_set = nullptr;
static ff7_texture_set* jp_prompt_polygon_original_texture_set = nullptr;
static uint32_t jp_prompt_texture = 0;
static uint32_t jp_prompt_texture_width = 0;
static uint32_t jp_prompt_texture_height = 0;
static constexpr int jp_prompt_size = 40;

static void jp_unload_prompt_graphics_object()
{
  ff7_texture_set* private_texture_set = nullptr;
  ff7_polygon_set* polygon_set = jp_prompt_graphics_object
    ? (ff7_polygon_set*)jp_prompt_graphics_object->polygon_set
    : nullptr;
  if (jp_prompt_graphics_object && jp_prompt_graphics_object->hundred_data
      && jp_prompt_original_texture_set)
  {
    private_texture_set = (ff7_texture_set*)jp_prompt_graphics_object->hundred_data->texture_set;
    jp_prompt_graphics_object->hundred_data->texture_set = (struct texture_set*)jp_prompt_original_texture_set;
  }
  if (polygon_set && polygon_set->hundred_data
      && jp_prompt_polygon_original_texture_set)
  {
    polygon_set->hundred_data->texture_set =
      (struct texture_set*)jp_prompt_polygon_original_texture_set;
  }
  ff7_externals.sub_671082(&jp_prompt_graphics_object);
  if (private_texture_set && private_texture_set != jp_prompt_original_texture_set)
  {
    delete private_texture_set->ogl.gl_set;
    delete[] private_texture_set->texturehandle;
    delete private_texture_set;
  }
  jp_prompt_original_texture_set = nullptr;
  jp_prompt_polygon_original_texture_set = nullptr;
  if (jp_prompt_texture)
    newRenderer.deleteTexture(jp_prompt_texture);
  jp_prompt_texture = 0;
  jp_prompt_texture_width = 0;
  jp_prompt_texture_height = 0;
}

static void jp_load_prompt_graphics_object(struc_3* graphics_context, char* template_path, ff7_game_obj* game_object)
{
  char path[BASEDIR_LENGTH + 64];
  _snprintf(path, sizeof(path), R"(%s\data\png\buttons_ps4.png)", basedir);
  if (!fileExists(path))
    return;

  jp_prompt_graphics_object = ff7_externals.engine_load_graphics_object_6710AC(
    1, 12, graphics_context, template_path, (int)game_object->dx_sfx_something);
  if (!jp_prompt_graphics_object || !jp_prompt_graphics_object->hundred_data
      || !jp_prompt_graphics_object->hundred_data->texture_set)
    return;

  jp_prompt_texture = newRenderer.createTextureLibPng(
    path, &jp_prompt_texture_width, &jp_prompt_texture_height, true);
  if (!jp_prompt_texture || jp_prompt_texture_width != 512 || jp_prompt_texture_height != 512)
  {
    jp_unload_prompt_graphics_object();
    ffnx_warning("Japanese button prompt atlas must be 512x512: %s\n", path);
    return;
  }
  ff7_polygon_set* polygon_set = (ff7_polygon_set*)jp_prompt_graphics_object->polygon_set;
  if (!polygon_set || !polygon_set->hundred_data || !polygon_set->hundred_data->texture_set)
  {
    jp_unload_prompt_graphics_object();
    ffnx_warning("Japanese button prompt graphics object has no render texture set\n");
    return;
  }

  jp_prompt_original_texture_set = (ff7_texture_set*)jp_prompt_graphics_object->hundred_data->texture_set;
  jp_prompt_polygon_original_texture_set =
    (ff7_texture_set*)polygon_set->hundred_data->texture_set;
  ff7_tex_header* texture_header = (ff7_tex_header*)jp_prompt_polygon_original_texture_set->tex_header;
  uint32_t texture_count = std::max(std::max(texture_header->palettes,
    texture_header->palette_index + 1), 8u);
  ff7_texture_set* private_texture_set = new ff7_texture_set(*jp_prompt_polygon_original_texture_set);
  private_texture_set->texturehandle = new uint32_t[texture_count];
  memcpy(private_texture_set->texturehandle, jp_prompt_polygon_original_texture_set->texturehandle,
    texture_count * sizeof(*private_texture_set->texturehandle));
  private_texture_set->texturehandle[7] = jp_prompt_texture;
  private_texture_set->ogl.gl_set = jp_prompt_polygon_original_texture_set->ogl.gl_set
    ? new gl_texture_set(*jp_prompt_polygon_original_texture_set->ogl.gl_set)
    : nullptr;
  if (private_texture_set->ogl.gl_set)
    private_texture_set->ogl.gl_set->force_filter = true;
  private_texture_set->ogl.external = true;
  private_texture_set->ogl.width = jp_prompt_texture_width;
  private_texture_set->ogl.height = jp_prompt_texture_height;
  jp_prompt_graphics_object->hundred_data->texture_set = (struct texture_set*)private_texture_set;
  polygon_set->hundred_data->texture_set = (struct texture_set*)private_texture_set;
  ffnx_info("Using Japanese button prompt atlas: %s\n", path);
}

static void jp_draw_prompt_graphics_object(ff7_game_obj* game_object)
{
  if (!jp_prompt_graphics_object || !jp_prompt_texture)
    return;

  ff7_externals.engine_draw_graphics_object_66E641(jp_prompt_graphics_object, game_object);
}

void engine_load_menu_graphics_objects_6C1468_jp(int a1)
{
  multibyte_load_widths();
  uint32_t v1;
  uint32_t v2;
  uint32_t v3;
  uint32_t v4;
  uint32_t v5;
  uint32_t v6;
  char *menu_win_texture_path;
  char *menu_font_texture_path;
  char *battle_menu_win_d_texture_path;
  char *battle_menu_win_c_texture_path;
  char *battle_menu_win_b_texture_path;
  char *battle_menu_win_a_texture_path;
  char *menu_font_b_graphics_object;
  char *menu_font_a_texture_path;
  struc_3 a2;
  int viewport_type_404D80;
  ff7_game_obj *game_object_676578;
  int v18;

  viewport_type_404D80 = ff7_externals.engine_get_viewport_type_404D80();
  game_object_676578 = ff7_externals.engine_get_game_object_676578();
  jp_unload_prompt_graphics_object();
  if ( viewport_type_404D80 == 2 )
  {
    ff7_externals.sub_671082(ff7_externals.menu_font_a_graphics_object_DC100C);
    ff7_externals.sub_671082(ff7_externals.menu_font_b_graphics_object_DC1010);
    ff7_externals.sub_671082(ff7_externals.menu_win_a_blend_4_graphics_object_DC0FC8);
    ff7_externals.sub_671082(ff7_externals.menu_win_b_blend_4_graphics_object_DC0FCC);
    ff7_externals.sub_671082(ff7_externals.menu_win_c_blend_4_graphics_object_DC0FD0);
    ff7_externals.sub_671082(ff7_externals.menu_win_d_blend_4_graphics_object_DC0FD4);
    ff7_externals.sub_671082(ff7_externals.menu_win_a_blend_0_graphics_object_DC0FDC);
    ff7_externals.sub_671082(ff7_externals.menu_win_a_blend_1_graphics_object_DC0FE0);
    ff7_externals.sub_671082(ff7_externals.menu_win_b_blend_1_graphics_object_DC0FE4);
    ff7_externals.sub_671082(ff7_externals.menu_win_c_blend_1_graphics_object_DC0FE8);
    ff7_externals.sub_671082(ff7_externals.menu_win_d_blend_1_graphics_object_DC0FEC);

    // jp
    ff7_externals.sub_671082(&ff7_externals.menu_jafont_1_graphics_object);
  }
  else
  {
    ff7_externals.sub_671082(ff7_externals.menu_font_blend_4_graphics_object_DC1048);
    ff7_externals.sub_671082(ff7_externals.menu_win_blend_4_graphics_object_DC104C);
    ff7_externals.sub_671082(ff7_externals.menu_win_blend_0_graphics_object_DC1050);
    ff7_externals.sub_671082(ff7_externals.menu_win_blend_1_graphics_object_DC1054);
  }
  ff7_externals.sub_671082(ff7_externals.menu_win_c_blend_4_diff_graphics_object_DC0FD8);
  v18 = ff7_externals.sub_674530();
  ff7_externals.sub_67453A(0);
  ff7_externals.make_struc3_6745E6(4, &a2);
  a2.file_context.use_lgp = 1;
  a2.file_context.lgp_num = 4;
  a2.file_context.name_mangler = 0;
  a2.base_directory = (uint32_t)ff7_externals.unk_DC1074;
  a2.field_0 |= 0x10u;
  a2.field_50 |= 1u;
  v1 = a2.field_70;
  v2 = MAKEWORD(LOBYTE(a2.field_70) | 0x20, HIWORD(v1));
  a2.field_70 = v1;
  if ( viewport_type_404D80 == 2 )
  {
    // Load Japanese font textures
    ff7_externals.engine_set_blendmode_674659(4, &a2);
    ff7_externals.menu_jafont_1_graphics_object = ff7_externals.engine_load_graphics_object_6710AC(1, 12, &a2, "jafont_1.tim", (int)game_object_676578->dx_sfx_something);     ff7_externals.engine_set_blendmode_674659(4, &a2);
    ff7_externals.engine_set_blendmode_674659(4, &a2);
    ff7_externals.menu_jafont_2_graphics_object = ff7_externals.engine_load_graphics_object_6710AC(1, 12, &a2, "jafont_2.tim", (int)game_object_676578->dx_sfx_something);     ff7_externals.engine_set_blendmode_674659(4, &a2);
    ff7_externals.engine_set_blendmode_674659(4, &a2);
    ff7_externals.menu_jafont_3_graphics_object = ff7_externals.engine_load_graphics_object_6710AC(1, 12, &a2, "jafont_3.tim", (int)game_object_676578->dx_sfx_something);     ff7_externals.engine_set_blendmode_674659(4, &a2);
    ff7_externals.engine_set_blendmode_674659(4, &a2);
    ff7_externals.menu_jafont_4_graphics_object = ff7_externals.engine_load_graphics_object_6710AC(1, 12, &a2, "jafont_4.tim", (int)game_object_676578->dx_sfx_something);     ff7_externals.engine_set_blendmode_674659(4, &a2);
    ff7_externals.engine_set_blendmode_674659(4, &a2);
    ff7_externals.menu_jafont_5_graphics_object = ff7_externals.engine_load_graphics_object_6710AC(1, 12, &a2, "jafont_5.tim", (int)game_object_676578->dx_sfx_something);     ff7_externals.engine_set_blendmode_674659(4, &a2);
    ff7_externals.engine_set_blendmode_674659(4, &a2);
    ff7_externals.menu_jafont_6_graphics_object = ff7_externals.engine_load_graphics_object_6710AC(1, 12, &a2, "jafont_6.tim", (int)game_object_676578->dx_sfx_something);
    ff7_externals.engine_set_blendmode_674659(4, &a2);
    if ( a1 )
      menu_font_a_texture_path = ff7_externals.aUsfont_a_h_tim;
    else
      menu_font_a_texture_path = ff7_externals.aUsfont_a_l_tim;
    *ff7_externals.menu_font_a_graphics_object_DC100C = ff7_externals.engine_load_graphics_object_6710AC(1, 12, &a2, menu_font_a_texture_path, (int)game_object_676578->dx_sfx_something);
    ff7_externals.engine_set_blendmode_674659(4, &a2);
    if ( a1 )
      menu_font_b_graphics_object = ff7_externals.aUsfont_b_h_tim;
    else
      menu_font_b_graphics_object = ff7_externals.aUsfont_b_l_tim;
    *ff7_externals.menu_font_b_graphics_object_DC1010 = ff7_externals.engine_load_graphics_object_6710AC(1, 12, &a2, menu_font_b_graphics_object, (int)game_object_676578->dx_sfx_something);
    if ( a1 )
      battle_menu_win_a_texture_path = ff7_externals.aBtl_win_a_h_ti;
    else
      battle_menu_win_a_texture_path = ff7_externals.aBtl_win_a_l_ti;
    ff7_externals.engine_set_blendmode_674659(4, &a2);
    *ff7_externals.menu_win_a_blend_4_graphics_object_DC0FC8 = ff7_externals.engine_load_graphics_object_6710AC(1, 12, &a2, battle_menu_win_a_texture_path, (int)game_object_676578->dx_sfx_something);
    ff7_externals.engine_set_blendmode_674659(0, &a2);
    *ff7_externals.menu_win_a_blend_0_graphics_object_DC0FDC = ff7_externals.engine_load_graphics_object_6710AC(1, 12, &a2, battle_menu_win_a_texture_path, (int)game_object_676578->dx_sfx_something);
    ff7_externals.engine_set_blendmode_674659(1, &a2);
    *ff7_externals.menu_win_a_blend_1_graphics_object_DC0FE0 = ff7_externals.engine_load_graphics_object_6710AC(1, 12, &a2, battle_menu_win_a_texture_path, (int)game_object_676578->dx_sfx_something);
    if ( a1 )
      battle_menu_win_b_texture_path = ff7_externals.aBtl_win_b_h_ti;
    else
      battle_menu_win_b_texture_path = ff7_externals.aBtl_win_b_l_ti;
    ff7_externals.engine_set_blendmode_674659(4, &a2);
    *ff7_externals.menu_win_b_blend_4_graphics_object_DC0FCC = ff7_externals.engine_load_graphics_object_6710AC(1, 12, &a2, battle_menu_win_b_texture_path, (int)game_object_676578->dx_sfx_something);
    jp_load_prompt_graphics_object(&a2, battle_menu_win_b_texture_path, game_object_676578);
    ff7_externals.engine_set_blendmode_674659(1, &a2);
    *ff7_externals.menu_win_b_blend_1_graphics_object_DC0FE4 = ff7_externals.engine_load_graphics_object_6710AC(1, 12, &a2, battle_menu_win_b_texture_path, (int)game_object_676578->dx_sfx_something);
    if ( a1 )
      battle_menu_win_c_texture_path = ff7_externals.aBtl_win_c_h_ti;
    else
      battle_menu_win_c_texture_path = ff7_externals.aBtl_win_c_l_ti;
    ff7_externals.engine_set_blendmode_674659(4, &a2);
    *ff7_externals.menu_win_c_blend_4_graphics_object_DC0FD0 = ff7_externals.engine_load_graphics_object_6710AC(1, 12, &a2, battle_menu_win_c_texture_path, (int)game_object_676578->dx_sfx_something);
    ff7_externals.engine_set_blendmode_674659(1, &a2);
    *ff7_externals.menu_win_c_blend_1_graphics_object_DC0FE8 = ff7_externals.engine_load_graphics_object_6710AC(1, 12, &a2, battle_menu_win_c_texture_path, (int)game_object_676578->dx_sfx_something);
    v2 = a2.field_70;
    v2 = MAKEWORD(a2.field_70 & 0xDF, HIWORD(v2));
    v3 = v2;
    v3 = MAKEWORD(a2.field_70 & 0x5F | 0x80, HIWORD(v3));
    a2.field_70 = v3;
    ff7_externals.engine_set_blendmode_674659(4, &a2);
    *ff7_externals.menu_win_c_blend_4_diff_graphics_object_DC0FD8 = ff7_externals.engine_load_graphics_object_6710AC(1, 12, &a2, battle_menu_win_c_texture_path, (int)game_object_676578->dx_sfx_something);
    v4 = a2.field_70;
    v4 = MAKEWORD(a2.field_70 & 0x7F, HIWORD(v4));
    a2.field_70 = v4 | 0x20;
    if ( a1 )
      battle_menu_win_d_texture_path = ff7_externals.aBtl_win_d_h_ti;
    else
      battle_menu_win_d_texture_path = ff7_externals.aBtl_win_d_l_ti;
    ff7_externals.engine_set_blendmode_674659(4, &a2);
    *ff7_externals.menu_win_d_blend_4_graphics_object_DC0FD4 = ff7_externals.engine_load_graphics_object_6710AC(1, 12, &a2, battle_menu_win_d_texture_path, (int)game_object_676578->dx_sfx_something);
    ff7_externals.engine_set_blendmode_674659(1, &a2);
    *ff7_externals.menu_win_d_blend_1_graphics_object_DC0FEC = ff7_externals.engine_load_graphics_object_6710AC(1, 12, &a2, battle_menu_win_d_texture_path, (int)game_object_676578->dx_sfx_something);
  }
  else
  {
    ff7_externals.engine_set_blendmode_674659(4, &a2);
    if ( a1 )
      menu_font_texture_path = ff7_externals.aUsfont_h_tim;
    else
      menu_font_texture_path = ff7_externals.aUsfont_l_tim;
    *ff7_externals.menu_font_blend_4_graphics_object_DC1048 = ff7_externals.engine_load_graphics_object_6710AC(1, 12, &a2, menu_font_texture_path, (int)game_object_676578->dx_sfx_something);
    if ( a1 )
      menu_win_texture_path = ff7_externals.aBtl_win_h_tim;
    else
      menu_win_texture_path = ff7_externals.aBtl_win_l_tim;
    ff7_externals.engine_set_blendmode_674659(4, &a2);
    *ff7_externals.menu_win_blend_4_graphics_object_DC104C = ff7_externals.engine_load_graphics_object_6710AC(1, 12, &a2, menu_win_texture_path, (int)game_object_676578->dx_sfx_something);
    ff7_externals.engine_set_blendmode_674659(0, &a2);
    *ff7_externals.menu_win_blend_0_graphics_object_DC1050 = ff7_externals.engine_load_graphics_object_6710AC(1, 12, &a2, menu_win_texture_path, (int)game_object_676578->dx_sfx_something);
    ff7_externals.engine_set_blendmode_674659(1, &a2);
    *ff7_externals.menu_win_blend_1_graphics_object_DC1054 = ff7_externals.engine_load_graphics_object_6710AC(1, 12, &a2, menu_win_texture_path, (int)game_object_676578->dx_sfx_something);
    v5 = a2.field_70;
    v5 = MAKEWORD(a2.field_70 & 0xDF, HIWORD(v5));
    v6 = v5;
    v6 = MAKEWORD(a2.field_70 & 0x5F | 0x80, HIWORD(v6));
    a2.field_70 = v6;
    ff7_externals.engine_set_blendmode_674659(4, &a2);
    *ff7_externals.menu_win_c_blend_4_diff_graphics_object_DC0FD8 = ff7_externals.engine_load_graphics_object_6710AC(1, 12, &a2, menu_win_texture_path, (int)game_object_676578->dx_sfx_something);
  }
  ff7_externals.sub_67453A(v18);
}

int charWidthData[6][256] =
{
    { // Jap - 0
        30, 30, 28, 31, 30, 30, 29, 29, 30, 30, 29, 30, 31, 30, 29, 27,
        30, 29, 29, 29, 31, 30, 28, 23, 30, 30, 30, 31, 29, 31, 30, 30,
        31, 30, 30, 31, 31, 29, 21, 28, 29, 30, 30, 27, 31, 30, 30, 29,
        29, 30, 30, 22, 22, 22, 22, 23, 22, 22, 22, 22, 22, 51, 62, 16,
        28, 29, 24, 30, 26, 29, 29, 29, 28, 29, 26, 29, 29, 28, 25, 23,
        28, 28, 25, 25, 30, 28, 28, 23, 27, 29, 28, 30, 25, 28, 26, 28,
        29, 28, 26, 28, 29, 28, 20, 25, 25, 24, 28, 28, 24, 27, 28, 28,
        28, 29, 29, 29, 29, 27, 23, 30, 29, 31, 24, 29, 27, 27, 25, 30,
        27, 29, 26, 28, 29, 27, 27, 24, 21, 22, 30, 27, 25, 31, 25, 26,
        29, 29, 29, 28, 25, 27, 25, 30, 26, 29, 25, 27, 23, 24, 24, 25,
        24, 24, 21, 23, 24, 23, 21, 23, 22, 20, 24, 24, 24, 25, 11, 21,
        29, 14, 8, 23, 24, 21, 24, 24, 20, 19, 25, 22, 14, 16, 22, 18,
        27, 22, 26, 21, 27, 22, 21, 24, 22, 24, 31, 24, 23, 23, 14, 22,
        28, 27, 27, 29, 30, 12, 25, 22, 11, 0, 27, 23, 23, 23, 12, 22,
        11, 23, 23, 0, 0, 0, 31, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        // ^ index 0xE6 (XIII glyph): was 32, but the width field is only 5 bits
        //   (& 0x1F), so 32 truncated to width 0 -> zero advance -> the next
        //   character overlapped it. 31 = max width -> advance matches the
        //   drawn glyph width (no overlap).
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    },{ // Jap - 1
        31, 31, 31, 31, 31, 30, 31, 31, 30, 31, 31, 31, 31, 30, 31, 31,
        31, 31, 31, 30, 31, 31, 31, 31, 31, 31, 31, 30, 31, 31, 31, 28,
        31, 31, 31, 31, 31, 31, 31, 31, 31, 30, 31, 31, 31, 31, 31, 31,
        30, 31, 31, 31, 31, 30, 31, 31, 31, 31, 31, 31, 29, 31, 31, 31,
        30, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 30, 31, 31,
        31, 31, 30, 31, 31, 31, 29, 31, 31, 31, 31, 31, 30, 30, 31, 31,
        31, 31, 31, 30, 31, 31, 31, 31, 31, 31, 31, 29, 31, 31, 31, 29,
        31, 30, 31, 30, 31, 31, 30, 31, 31, 31, 31, 31, 30, 31, 31, 31,
        31, 31, 31, 31, 31, 31, 31, 30, 31, 31, 31, 31, 31, 31, 30, 31,
        31, 31, 31, 30, 30, 31, 30, 31, 30, 31, 31, 30, 31, 31, 31, 30,
        31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,
        31, 31, 31, 31, 31, 31, 30, 31, 31, 28, 30, 31, 31, 31, 31, 27,
        31, 30, 28, 31, 31, 29, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,
        31, 31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        12, 12, 8, 8, 8, 22, 8, 15, 13, 27, 32, 17, 27, 17, 30, 0,
        0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0,
    },{ // Jap - 2
        31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 30, 31, 31, 31, 31, 31,
        31, 30, 31, 30, 31, 31, 31, 31, 31, 31, 31, 31, 30, 30, 31, 30,
        31, 31, 31, 31, 31, 31, 30, 31, 31, 31, 30, 30, 31, 31, 31, 31,
        31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 30, 31, 31, 31, 31, 31,
        31, 31, 31, 31, 31, 30, 31, 31, 31, 31, 30, 31, 31, 29, 31, 31,
        31, 31, 31, 29, 31, 30, 31, 31, 31, 31, 31, 30, 31, 31, 30, 31,
        31, 31, 29, 30, 30, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,
        30, 31, 31, 31, 28, 30, 31, 31, 31, 30, 31, 31, 31, 31, 31, 31,
        31, 30, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,
        31, 31, 31, 31, 31, 29, 31, 31, 31, 31, 31, 31, 31, 31, 31, 29,
        31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 30, 29, 31, 31,
        31, 31, 31, 31, 30, 31, 31, 31, 31, 31, 31, 29, 31, 31, 31, 31,
        31, 31, 31, 31, 31, 31, 31, 31, 31, 30, 31, 31, 31, 31, 31, 31,
        31, 31, 31, 31, 29, 31, 31, 31, 31, 31, 31, 31, 31, 29, 31, 31,
        31, 31, 30, 31, 30, 31, 30, 29, 30, 31, 31, 31, 31, 31, 31, 31,
        31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 30, 31, 31, 31, 31,
    },{ // Jap - 3
        29, 30, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,
        31, 31, 31, 31, 30, 31, 31, 31, 31, 31, 31, 31, 31, 30, 31, 31,
        31, 31, 31, 31, 31, 31, 30, 30, 31, 30, 31, 31, 31, 31, 31, 31,
        28, 31, 31, 28, 31, 31, 31, 31, 31, 29, 31, 31, 31, 31, 31, 31,
        31, 30, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,
        31, 31, 31, 31, 31, 31, 31, 30, 31, 31, 31, 31, 31, 31, 31, 31,
        31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 30, 31, 31, 31, 31, 31,
        31, 31, 30, 31, 30, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,
        31, 31, 31, 31, 31, 30, 30, 31, 31, 30, 31, 31, 29, 31, 31, 31,
        31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,
        31, 31, 31, 31, 30, 31, 30, 31, 31, 31, 31, 30, 31, 31, 30, 31,
        31, 31, 31, 31, 31, 31, 31, 30, 30, 31, 31, 31, 31, 30, 31, 30,
        31, 29, 30, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,
        31, 31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        19, 20, 19, 20, 19, 17, 19, 18, 7, 12, 18, 10, 25, 18, 20, 20,
        20, 14, 17, 16, 18, 19, 27, 18, 19, 17, 0, 0, 0, 0, 0, 0,
    },{ // Jap - 4
        31, 31, 31, 31, 31, 30, 31, 31, 30, 31, 31, 31, 31, 31, 31, 31,
        31, 31, 30, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,
        31, 31, 31, 30, 31, 31, 31, 31, 31, 30, 31, 31, 31, 31, 31, 31,
        29, 31, 31, 30, 31, 31, 31, 29, 31, 30, 31, 31, 31, 30, 31, 30,
        31, 30, 31, 31, 31, 31, 31, 31, 31, 30, 29, 30, 31, 31, 31, 31,
        31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 30, 31, 31, 31, 31,
        31, 30, 31, 31, 30, 31, 31, 31, 31, 31, 31, 31, 31, 30, 29, 31,
        31, 31, 31, 31, 30, 31, 31, 31, 31, 30, 30, 31, 31, 31, 31, 31,
        30, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 30,
        31, 31, 31, 31, 31, 31, 30, 29, 31, 31, 30, 31, 31, 31, 31, 31,
        31, 31, 31, 31, 31, 30, 31, 31, 31, 31, 31, 30, 31, 31, 30, 31,
        31, 31, 31, 31, 31, 31, 31, 31, 30, 30, 31, 31, 31, 31, 31, 31,
        30, 31, 31, 31, 31, 31, 31, 31, 30, 31, 31, 31, 31, 31, 31, 31,
        31, 31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    },{ // Jap - 5
        31, 31, 30, 31, 31, 31, 31, 31, 31, 31, 30, 31, 31, 31, 31, 31,
        31, 31, 31, 30, 31, 31, 31, 31, 31, 31, 31, 31, 30, 31, 31, 30,
        31, 31, 31, 30, 31, 31, 31, 30, 30, 31, 30, 31, 31, 31, 31, 31,
        31, 31, 31, 31, 31, 30, 30, 31, 31, 31, 31, 31, 31, 30, 31, 31,
        31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 30, 31, 31, 31, 31,
        31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,
        31, 31, 31, 30, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 29, 31,
        31, 31, 31, 31, 31, 31, 31, 30, 31, 30, 31, 31, 31, 30, 31, 31,
        31, 31, 31, 29, 31, 30, 31, 31, 31, 30, 31, 31, 31, 29, 31, 31,
        31, 31, 31, 30, 31, 30, 31, 31, 31, 31, 30, 31, 31, 31, 31, 31,
        31, 31, 30, 31, 30, 31, 31, 31, 31, 31, 30, 31, 31, 31, 31, 31,
        31, 31, 31, 29, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 30, 31,
        31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 30,
        30, 31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    }
};

// multibyte (EN) advance: table value IS the advance in screen units (max 31 covers wide Arabic);
// JP keeps the original half-width semantics (32px texel cells -> 16 units).
static inline float z_half_width(int w) { return ff7_japanese_edition ? std::ceil(0.5f * (float)w) : (float)w; }

static const unsigned char jp_spacing_primary[256] = {
  59, 58, 55, 62, 59, 60, 57, 57, 59, 58, 57, 60, 61, 60, 58, 52,
  59, 57, 57, 56, 62, 59, 55, 46, 59, 60, 60, 61, 58, 60, 59, 59,
  61, 60, 59, 60, 61, 59, 40, 56, 57, 58, 57, 53, 61, 58, 59, 57,
  57, 58, 57, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 42, 50, 49,
  56, 57, 47, 60, 50, 58, 57, 57, 55, 58, 50, 58, 57, 56, 49, 46,
  56, 54, 48, 49, 59, 56, 55, 46, 53, 58, 55, 59, 50, 56, 51, 56,
  57, 54, 52, 56, 57, 55, 39, 48, 50, 46, 55, 55, 48, 52, 55, 55,
  55, 58, 57, 58, 58, 53, 46, 59, 58, 60, 47, 56, 54, 53, 49, 58,
  54, 56, 52, 56, 58, 53, 53, 47, 40, 43, 60, 52, 49, 60, 48, 51,
  57, 58, 57, 55, 50, 53, 49, 58, 51, 57, 49, 54, 45, 48, 48, 49,
  48, 47, 42, 45, 47, 47, 41, 45, 43, 40, 49, 48, 47, 48, 30, 42,
  56, 58, 32, 46, 48, 41, 46, 46, 39, 38, 48, 43, 26, 32, 43, 35,
  53, 43, 51, 41, 52, 43, 43, 46, 43, 47, 61, 47, 45, 45, 36, 44,
  56, 53, 53, 57, 59, 30, 50, 44, 44, 64, 54, 46, 45, 48, 48, 42,
  44, 46, 46, 0, 0, 0, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static const unsigned char jp_spacing_fa[32] = {
  24, 24, 15, 16, 16, 43, 16, 30, 25, 54, 63, 35, 53, 35, 64, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static const unsigned char jp_spacing_fc[32] = {
  36, 38, 36, 38, 37, 33, 36, 35, 14, 25, 34, 18, 50, 36, 39, 38,
  38, 27, 33, 31, 36, 37, 54, 36, 37, 34, 0, 0, 0, 0, 0, 0
};

static inline int jp_spacing_metric(uint16_t letter, int char_width)
{
  if (!ff7_japanese_edition)
    return char_width;

  if ((letter & 0xFF) >= 0xFA && (letter & 0xFF) <= 0xFE
      && ((letter >> 8) < 0xFA || (letter >> 8) > 0xFE))
    letter = (uint16_t)((letter << 8) | (letter >> 8));

  int spacing = 64;
  if (letter <= 0xFF)
    spacing = jp_spacing_primary[letter];
  else if ((uint16_t)(letter + 0x520) <= 0x1F)
    spacing = jp_spacing_fa[letter & 0x1F];
  else if ((uint16_t)(letter + 0x320) <= 0x1F)
    spacing = jp_spacing_fc[letter & 0x1F];

  spacing += 5;
  return spacing < 64 ? spacing : 64;
}

static inline float jp_spacing_advance(uint16_t letter, int left_padding, int char_width, float scale_factor)
{
  if (ff7_japanese_edition)
    return (float)left_padding + (float)jp_spacing_metric(letter, char_width) * 0.3125f;
  return (float)left_padding + (float)jp_spacing_metric(letter, char_width) * 0.25f * scale_factor;
}

static inline int jp_center_advance(uint16_t letter, int left_padding, int char_width)
{
  if (!ff7_japanese_edition)
    return left_padding + (int)std::ceil(z_half_width(char_width));
  return 10 * jp_spacing_metric(letter, char_width) / 64;
}

// ff7_multibyte_font: override the hardcoded width table from <basedir>/multibyte_widths.bin
// (6*256 bytes, one per font sheet/code, same (pad<<5|width) packing as window.bin member 3),
// so translations can tune advances without recompiling FFNx.
static byte multibyte_icon_mask[256] = {0};
static int multibyte_field_linestep_q = 128;   // field line advance in QUARTER px (128 = 32.0), live-tunable

// Resolve a multibyte tuning file through the standard layers: override_path first, then the
// per-release data path (data/lang-*/kernel on Steam/GOG/Store/2026, data/kernel on 1998).
static bool multibyte_resolve_path(const char *name, char *out, size_t out_size)
{
  char in[MAX_PATH]{ 0 };
  _snprintf(in, sizeof(in), R"(data\kernel\%s)", name);
  return redirect_path_with_override(in, out, out_size) != 1;
}

static void multibyte_load_widths()
{
  // Hot-reload: re-read the widths file whenever its mtime changes (checked at most 1x/sec),
  // so letter advances can be tuned live while the game runs (width_gui.py writes the file).
  static bool tried = false;
  static long long last_mtime = -1;
  static DWORD last_check = 0;
  if (!ff7_multibyte_font) return;
  DWORD now = GetTickCount();
  if (tried && (now - last_check) < 1000) return;
  last_check = now;
  char path[MAX_PATH]{ 0 };
  // line step re-read every 1s tick — must NOT sit behind the widths mtime gate,
  // or moving only the spacing slider never reaches it
  multibyte_resolve_path("multibyte_linestep.bin", path, sizeof(path));
  FILE *lf = fopen(path, "rb");
  if (lf)
  {
    unsigned char lb[2]; size_t ln = fread(lb, 1, 2, lf);
    int q = (ln == 2) ? (lb[0] | (lb[1] << 8)) : (ln == 1 ? lb[0] * 4 : 0);  // 1-byte legacy = whole px
    if (q >= 80 && q <= 160 && q != multibyte_field_linestep_q)
      multibyte_field_linestep_q = q;
    fclose(lf);
  }
  multibyte_resolve_path("multibyte_widths.bin", path, sizeof(path));
  struct _stat64 st;
  bool first = !tried;
  if (_stat64(path, &st) == 0)
  {
    if (tried && (long long)st.st_mtime == last_mtime) return;
    last_mtime = (long long)st.st_mtime;
  }
  tried = true;
  FILE *f = fopen(path, "rb");
  if (!f) return;
  unsigned char buf[6 * 256];
  if (fread(buf, 1, sizeof(buf), f) == sizeof(buf))
  {
    for (int i = 0; i < 6; i++)
      for (int j = 0; j < 256; j++)
        charWidthData[i][j] = buf[i * 256 + j];
  }
  else ffnx_error("ff7_multibyte_font: %s wrong size (need 1536 bytes)\n", path);
  fclose(f);
  if (!first) return;
  multibyte_resolve_path("multibyte_iconmask.bin", path, sizeof(path));
  f = fopen(path, "rb");
  if (f)
  {
    fread(multibyte_icon_mask, 1, 256, f);
    fclose(f);
  }
}

bgra_byte get_character_color(int n_shapes)
{
  bgra_byte color = { 255, 255, 255, 255 };
  switch (n_shapes)
  {
    case 0:
      color = { 106, 106, 106, 255 };
    break;
    case 1:
      color = { 189, 98, 7, 255 };
    break;
    case 2:
      color = { 10, 0, 189, 255 };
    break;
    case 3:
      color = { 230, 10, 230, 255 };
    break;
    case 4:
      color = { 124, 230, 90, 255 };
    break;
    case 5:
      color = { 230, 230, 10, 255 };
    break;
    case 6:
      color = { 10, 230, 230, 255 };
    break;
    case 7:
      color = { 230, 230, 230, 255 };
    break;
  }

  return color;
}

static int jp_physical_button_for_action(int action)
{
  if (ff7_externals.savemap && (ff7_externals.savemap->config_bitmap_1 & 0x04))
  {
    for (int button = 0; button < 16; ++button)
      if ((byte)ff7_externals.savemap->controller_mapping[button] == action)
        return button;
  }

  return action < 16 ? action : -1;
}

struct jp_prompt_sprite
{
  ff7_graphics_object* graphics_object;
  int u;
  int v;
  int width;
  int height;
  int texture_width;
  int texture_height;
};

static bool jp_prompt_sprite_for_button(int button, jp_prompt_sprite* sprite)
{
  if (jp_prompt_graphics_object)
  {
    int column;
    int row;
    switch (button)
    {
      case 0:  column = 3; row = 0; break; // L2
      case 1:  column = 4; row = 0; break; // R2
      case 2:  column = 2; row = 0; break; // L1
      case 3:  column = 2; row = 1; break; // R1
      case 4:  column = 1; row = 1; break; // Triangle
      case 5:  column = 1; row = 0; break; // Circle
      case 6:  column = 0; row = 0; break; // Cross
      case 7:  column = 0; row = 1; break; // Square
      case 8:  column = 3; row = 1; break; // Select / Share
      case 11: column = 4; row = 1; break; // Start / Options
      case 12: column = 0; row = 2; break; // Up
      case 13: column = 2; row = 2; break; // Right
      case 14: column = 1; row = 2; break; // Down
      case 15: column = 0; row = 3; break; // Left
      default: return false;
    }

    *sprite = { jp_prompt_graphics_object, column * 100, row * 100, 100, 100,
      (int)jp_prompt_texture_width, (int)jp_prompt_texture_height };
    return true;
  }

  switch (button)
  {
    case 0:  *sprite = { *ff7_externals.menu_win_b_blend_4_graphics_object_DC0FCC,  32, 160, 32, 32, 256, 256 }; break; // L2
    case 1:  *sprite = { *ff7_externals.menu_win_a_blend_4_graphics_object_DC0FC8, 224, 160, 32, 32, 256, 256 }; break; // R2
    case 2:  *sprite = { *ff7_externals.menu_win_b_blend_4_graphics_object_DC0FCC,   0, 160, 32, 32, 256, 256 }; break; // L1
    case 3:  *sprite = { *ff7_externals.menu_win_a_blend_4_graphics_object_DC0FC8, 192, 160, 32, 32, 256, 256 }; break; // R1
    case 4:  *sprite = { *ff7_externals.menu_win_b_blend_4_graphics_object_DC0FCC,   0, 128, 32, 32, 256, 256 }; break; // Triangle
    case 5:  *sprite = { *ff7_externals.menu_win_a_blend_4_graphics_object_DC0FC8, 192, 128, 32, 32, 256, 256 }; break; // Circle
    case 6:  *sprite = { *ff7_externals.menu_win_b_blend_4_graphics_object_DC0FCC,  32, 128, 32, 32, 256, 256 }; break; // Cross
    case 7:  *sprite = { *ff7_externals.menu_win_a_blend_4_graphics_object_DC0FC8, 224, 128, 32, 32, 256, 256 }; break; // Square
    case 8:  *sprite = { *ff7_externals.menu_win_a_blend_4_graphics_object_DC0FC8, 192, 192, 32, 32, 256, 256 }; break; // Select
    case 11: *sprite = { *ff7_externals.menu_win_a_blend_4_graphics_object_DC0FC8, 224, 192, 32, 32, 256, 256 }; break; // Start
    case 12: *sprite = { *ff7_externals.menu_win_a_blend_4_graphics_object_DC0FC8, 160,  64, 32, 32, 256, 256 }; break; // Up
    case 13: *sprite = { *ff7_externals.menu_win_a_blend_4_graphics_object_DC0FC8, 160,  96, 32, 32, 256, 256 }; break; // Right
    case 14: *sprite = { *ff7_externals.menu_win_a_blend_4_graphics_object_DC0FC8, 192,  64, 32, 32, 256, 256 }; break; // Down
    case 15: *sprite = { *ff7_externals.menu_win_a_blend_4_graphics_object_DC0FC8, 192,  96, 32, 32, 256, 256 }; break; // Left
    default: return false;
  }

  return true;
}

static bool jp_submit_field_quad(ff7_graphics_object* graphics_object, float x, float y, float z,
  float width, float height, float u, float v, float u_width, float v_height, bgra_byte color, byte shape)
{
  if (!graphics_object || !common_externals.draw_graphics_object(1, (struct graphics_object*)graphics_object))
    return false;

  graphics_vertex* vertices = graphics_object->vertex_transform;
  vertices[0].position = { x, y, z, 1.0f };
  vertices[0].color = color;
  vertices[0].alpha_mask = -16777216;
  vertices[0].u = u;
  vertices[0].v = v;
  vertices[1] = vertices[0];
  vertices[1].position.y = y + height;
  vertices[1].v = v + v_height;
  vertices[2] = vertices[0];
  vertices[2].position.x = x + width;
  vertices[2].u = u + u_width;
  vertices[3] = vertices[2];
  vertices[3].position.y = y + height;
  vertices[3].v = v + v_height;
  *(byte*)graphics_object->curr_total_n_shape = shape;
  graphics_object->field_7C = shape;
  return true;
}

static int jp_draw_field_prompt_button(int button, int x, int y, float z)
{
  jp_prompt_sprite sprite;
  if (!jp_prompt_sprite_for_button(button, &sprite))
    return x;

  if (!jp_submit_field_quad(sprite.graphics_object, (float)x,
      (float)y - jp_prompt_size / 4.0f, z, (float)jp_prompt_size, (float)jp_prompt_size,
      (float)sprite.u / sprite.texture_width, (float)sprite.v / sprite.texture_height,
      (float)sprite.width / sprite.texture_width, (float)sprite.height / sprite.texture_height,
      get_character_color(7), 7))
  {
    static bool logged_submit_failure = false;
    if (!logged_submit_failure)
    {
      ffnx_warning("Failed to submit Japanese button prompt for button %d\n", button);
      logged_submit_failure = true;
    }
  }
  return x + jp_prompt_size;
}

static int jp_draw_field_prompt_action(int action, int x, int y, float z)
{
  static constexpr byte action_map[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 11, 12, 13, 14, 15
  };

  if (action < 0 || action >= sizeof(action_map))
    return x;

  return jp_draw_field_prompt_button(jp_physical_button_for_action(action_map[action]), x, y, z);
}

static int jp_draw_field_letter(uint16_t letter, int x, int y, float z, bgra_byte color, int color_index)
{
  int page = letter > 0xFF ? (letter >> 8) - 0xF9 : 0;
  byte character = (byte)letter;
  ff7_graphics_object* graphics_objects[] = {
    ff7_externals.menu_jafont_1_graphics_object,
    ff7_externals.menu_jafont_2_graphics_object,
    ff7_externals.menu_jafont_3_graphics_object,
    ff7_externals.menu_jafont_4_graphics_object,
    ff7_externals.menu_jafont_5_graphics_object,
    ff7_externals.menu_jafont_6_graphics_object,
  };
  int char_width = charWidthData[page][character] & 0x1F;
  int spacing = jp_spacing_metric(letter, char_width);
  float width = spacing * 0.3125f;
  jp_submit_field_quad(graphics_objects[page], (float)x, (float)y, z, width, 20.0f,
    32.0f * (character % 16) / 512.0f, 32.0f * (character / 16) / 512.0f,
    spacing * 0.5f / 512.0f, 32.0f / 512.0f, color, (byte)(2 * color_index));
  return x + (int)width;
}

static int jp_draw_field_fd_control(byte control, int x, int y, float z, bgra_byte color, int color_index)
{
  if (control <= 0xFD)
    return jp_draw_field_prompt_action(control & 0x0F, x, y, z);

  if (control == 0xFE)
  {
    x = jp_draw_field_prompt_action(4, x, y, z);
    x = jp_draw_field_letter(0xFAE7, x, y, z, color, color_index);
    return jp_draw_field_prompt_action(5, x, y, z);
  }

  x = jp_draw_field_letter(0xFA7D, x, y, z, color, color_index);
  return jp_draw_field_letter(0xFD33, x, y, z, color, color_index);
}

static int jp_measure_field_letter(uint16_t letter, bool use_fixed_spacing)
{
  if (use_fixed_spacing)
    return 10;

  int page = letter > 0xFF ? (letter >> 8) - 0xF9 : 0;
  byte character = (byte)letter;
  int char_width = charWidthData[page][character] & 0x1F;
  int left_padding = charWidthData[page][character] >> 5;
  return jp_center_advance(letter, left_padding, char_width);
}

static int jp_measure_field_fd_control(byte control, bool use_fixed_spacing)
{
  if (control <= 0xFD)
    return jp_prompt_size / 2;
  if (control == 0xFE)
    return jp_prompt_size + jp_measure_field_letter(0xFAE7, use_fixed_spacing);
  return jp_measure_field_letter(0xFA7D, use_fixed_spacing)
    + jp_measure_field_letter(0xFD33, use_fixed_spacing);
}

/////////////////////////////////////////////////////////////////////
int16_t field_submit_draw_text_640x480_6E706D_jp(int16_t character_x, int16_t character_y, int16_t text_box_right_position, byte *buffer_text, float z_value)
{
  multibyte_load_widths();   // hot-reload here too: dialogs must respond to live width tuning
  int _lsq_acc = 0;   // quarter-px remainder for fractional line stepping
  float scaleFactor = ff7_japanese_edition ? 1.25f : 1.0f;  // JP upscales 1.25x; multibyte (EN) draws native 1.0x
  int special_character_do_draw;
  graphics_vertex *window_vertices;
  int character_do_draw;
  graphics_vertex *character_bottom_right;
  graphics_vertex *character_top_right;
  graphics_vertex *character_bottom_left;
  graphics_vertex *character_top_left;
  graphics_vertex *special_character_top_right;
  graphics_vertex *special_character_bottom_left;
  graphics_vertex *special_character_top_left;
  int16_t offset_character_x;
  float character_u_width;
  int16_t current_character;
  float character_v;
  int16_t character_n_shapes;
  float special_character_u;
  float character_u;
  int16_t character;
  int16_t i;
  ff7_graphics_object *graphics_object;
  int16_t text_offset_spacing;
  float character_x_width;
  int16_t chararacter_u_in_byte;
  int16_t graphics_object_v_in_byte;
  char character_count;
  int16_t offset_u_in_byte;
  float character_u_width_in_byte;

  bool kanjiDetected = false;
  bool possibleOpcode = true; // 0xFEu i ssometimes JP text, and sometimes an FE opcode.  we must parse the opcodes.
  bool heartAtD9 = false;     // used to decide if d9 is suppose dot be a heart from btl_win;
  bool isPrompt = false;      // if true, and it's within range, make it a button prompt
  int curPage = 0;            // track which ja_font page we are on, so we can check widths later to set the above.
  int charWidth = 16;
  int leftPadding = 0;
  character_x = (*ff7_externals.field_current_window_pos_x_DC3CB4) + 20; // Fix first line for nameless windows. without this, piano instructions don't line up.
  character_count = 0;
  for ( i = 0;
        i < 1024
     && (*ff7_externals.field_remaining_character_length_DC3CCC)
     && (kanjiDetected || *buffer_text != 0xFF)
     && (kanjiDetected || *buffer_text != 0xE8);
        ++i )
  {
    if ( !kanjiDetected && *buffer_text == 0xE7 )
    {
      character_x = (*ff7_externals.field_current_window_pos_x_DC3CB4) + 20; // need to indent this far for pointers to point properly
      _lsq_acc += multibyte_field_linestep_q;
      character_y += _lsq_acc / 4; _lsq_acc %= 4;
      ++buffer_text;
      // Window field +0x16 (field_text_line_row) must count newlines, same as the vanilla JP path.
      // The old code incremented the pointer itself instead of the value it points to, so this
      // field never advanced past 0 for multibyte text. The engine only sends its "window closed"
      // signal once this row count and the character count below both reach their expected
      // values, so leaving this at 0 stalls the field script after any multi-line message.
      ++(*ff7_externals.field_text_line_row_DC3CB8);
      // Character count must include newlines, not just drawn glyphs, matching vanilla behavior.
      // Both fixes on these two lines are required together; either alone still stalls the window.
      ++(*ff7_externals.field_text_box_curr_n_characters_DC3CB0);
    }
    else
    {
      if (ff7_japanese_edition && buffer_text[0] == 0xFD && buffer_text[1] >= 0xF0)
      {
        int color_index = *ff7_externals.word_91F028;
        if (*ff7_externals.word_DC3CC4)
          color_index = ((unsigned __int8)((*ff7_externals.word_DC3CC8) >> 2) - character_count) & 7;
        else if (*ff7_externals.word_DC3CC0)
          color_index = (((*ff7_externals.word_DC3CC8) >> 2) & 1) ? color_index : 0;

        character_x = jp_draw_field_fd_control(buffer_text[1], character_x, character_y, z_value,
          get_character_color(color_index), color_index);
        buffer_text += 2;
        *ff7_externals.field_text_box_curr_n_characters_DC3CB0 += 2;
        --(*ff7_externals.field_remaining_character_length_DC3CCC);
        *ff7_externals.field_do_draw_character_DC3CEC = 1;
        *ff7_externals.field_do_draw_text_boxes_DC3CE8 = 1;
        ++character_count;
        continue;
      }

      heartAtD9 = false;
      isPrompt = false;
      switch ( *buffer_text )
      {
        case 0xFAu:
          ++(*ff7_externals.field_text_box_curr_n_characters_DC3CB0);
          ++buffer_text;
          graphics_object = ff7_externals.menu_jafont_2_graphics_object;
          kanjiDetected = true;
          possibleOpcode = false; // only 0xFEu *might* be an opcode.
          curPage = 1;
          charWidth = charWidthData[1][*buffer_text] & 0x1F;
          leftPadding = charWidthData[1][*buffer_text] >> 5;
          continue;
        case 0xFBu:
          ++(*ff7_externals.field_text_box_curr_n_characters_DC3CB0);
          ++buffer_text;
          graphics_object = ff7_externals.menu_jafont_3_graphics_object;
          kanjiDetected = true;
          possibleOpcode = false;
          curPage = 2;
          charWidth = charWidthData[2][*buffer_text] & 0x1F;
          leftPadding = charWidthData[2][*buffer_text] >> 5;
          continue;
        case 0xFCu:
          ++(*ff7_externals.field_text_box_curr_n_characters_DC3CB0);
          ++buffer_text;
          graphics_object = ff7_externals.menu_jafont_4_graphics_object;
          kanjiDetected = true;
          possibleOpcode = false;
          curPage = 3;
          charWidth = charWidthData[3][*buffer_text] & 0x1F;
          leftPadding = charWidthData[3][*buffer_text] >> 5;
          continue;
        case 0xFDu:
          ++(*ff7_externals.field_text_box_curr_n_characters_DC3CB0);
          ++buffer_text;
          graphics_object = ff7_externals.menu_jafont_5_graphics_object;
          kanjiDetected = true;
          possibleOpcode = false;
          curPage = 4;
          charWidth = charWidthData[4][*buffer_text] & 0x1F;
          leftPadding = charWidthData[4][*buffer_text] >> 5;
          continue;
        case 0xFEu:
          ++(*ff7_externals.field_text_box_curr_n_characters_DC3CB0);
          ++buffer_text;
          if (*buffer_text < 0xD2u) // real JP text.
          {
            graphics_object = ff7_externals.menu_jafont_6_graphics_object;
            kanjiDetected = true;
            possibleOpcode = false;
            curPage = 5;
            charWidth = charWidthData[5][*buffer_text] & 0x1F;
            leftPadding = charWidthData[5][*buffer_text] >> 5;
            continue;
          }
          else
          {
            curPage = 0;
            --buffer_text; // it was really an opcode, back up one character again and fall through to default so we can parse it later.
          }
        default:
          if(!kanjiDetected)
          {
            graphics_object = ff7_externals.menu_jafont_1_graphics_object;
            charWidth = charWidthData[0][*buffer_text] & 0x1F;
            leftPadding = charWidthData[0][*buffer_text] >> 5;
            possibleOpcode = true; // it SHOULD already be true, but just in case.
            curPage = 0;           // set page back to zero;
          }
          kanjiDetected = false;
          break;
      }

      offset_character_x = 0;
      switch ( *buffer_text )
      {
        case 0xFEu: // might be an opcode, or it might be second byte of a JP character
          if (possibleOpcode) // if it's an opcode.
          {
            ++buffer_text;
            ++(*ff7_externals.field_text_box_curr_n_characters_DC3CB0);
            if (*buffer_text < 0xDAu) // color
            {
              (*ff7_externals.word_91F028) = *buffer_text++ - 210; // write color to game memory.
              break;
            }
            if (*buffer_text == 218)
            {
              (*ff7_externals.word_DC3CC0) ^= 1u; // set flash flag
              ++buffer_text;
              break;
            }
            if (*buffer_text == 219)
            {
              (*ff7_externals.word_DC3CC4) ^= 1u; // set rainbow flag.
              ++buffer_text;
              break;
            }
            if (*buffer_text != 233)  // if we aren't goign to the next window
              goto LABEL_39;  // not a prompts skip the check for them.
            (*ff7_externals.dword_DC3CD4) ^= 1u;
            ++buffer_text;
            break;
          }
        default:
          // check if the heart slot is empty
          if (charWidthData[0][0xD9u] == 0) // if width is zero for that
            heartAtD9 = true;                            // then that slot is a heart
          // check if it could be a prompt.
          if (charWidthData[curPage][*buffer_text] == 0) // msut have zero spacing
          {
            // check range
            if (*buffer_text > 0xF5u || *buffer_text < 0xFAu)
              isPrompt = true;
          }
          if (!possibleOpcode || (!isPrompt && (*buffer_text != 0xd9u || !heartAtD9))) // check for prompts if on first page, and for heart if on first page and actual japanese
          {
            heartAtD9 = false;
            isPrompt = false;
            text_offset_spacing = 0;
            graphics_object_v_in_byte = 0;
LABEL_39:
            if ( (*ff7_externals.word_DC3CC0) || (*ff7_externals.word_DC3CC4) ) // if a color flag is set
            {
              if ( (*ff7_externals.word_DC3CC4) ) // rainboe
              {
                character_n_shapes = ((unsigned __int8)((*ff7_externals.word_DC3CC8) >> 2) - character_count) & 7; // get character color, but modify by character count
              }
              else if ( (((*ff7_externals.word_DC3CC8) >> 2) & 1) != 0 ) // flash
              {
                character_n_shapes = (*ff7_externals.word_91F028); // get flash color
              }
              else
              {
                if ( !(*ff7_externals.word_91F028) )  // if we didn't assign a color
                {
                  character_x += offset_character_x; // advance the start position of the character.
                  break;
                }
                character_n_shapes = 0;  // go back to normal is the color is cleared.
              }
            }
            else
            {
              character_n_shapes = (*ff7_externals.word_91F028); // read external to select chacter color normally.
            }
            if (!ff7_japanese_edition && multibyte_icon_mask[*buffer_text])
              character_n_shapes = 8; // icon cells: force pure white so icon art keeps true colors so icon art keeps true colors
            current_character = *buffer_text;
            character = current_character;
            uint16_t field_letter = curPage == 0
              ? (uint16_t)character
              : (uint16_t)(((0xFA + curPage - 1) << 8) | character);
            int field_spacing = jp_spacing_metric(field_letter, charWidth);
            if (ff7_japanese_edition)
              leftPadding = 0;
            offset_u_in_byte = 32 * (character % 16);
            graphics_object_v_in_byte += 32 * (character / 16); // calculate character position in sheet so we render the rigth character
            // SOFT-WRAP (Arabic long-line wrap). Not the soft-lock cause (verified: scene still locks
            // with this disabled). Kept for Arabic line wrapping. zaphod77 PR#925 leaves it off (JP text
            // pre-wrapped in flevel); Arabic needs it since RTL lines can exceed the window width.
            if (!ff7_japanese_edition)
            {
              int character_advance = (*ff7_externals.dword_DC3CD4)
                ? 30
                : (int)std::ceil(jp_spacing_advance(
                    curPage == 0 ? (uint16_t)*buffer_text
                                  : (uint16_t)(((0xFA + curPage - 1) << 8) | *buffer_text),
                    leftPadding, charWidth, scaleFactor));
              if ( character_x - (*ff7_externals.field_current_window_pos_x_DC3CB4) + character_advance > text_box_right_position )
              {
                character_x = (*ff7_externals.field_current_window_pos_x_DC3CB4) + 20;
                _lsq_acc += multibyte_field_linestep_q;
                character_y += _lsq_acc / 4; _lsq_acc %= 4;
                ++(*ff7_externals.field_text_line_row_DC3CB8);   // deref: soft-wrap advances the real row (see newline block).
              }
            }
            if ( !(*ff7_externals.dword_DC3CD4) ) // if not going to next window
              character_x += leftPadding; // apply padding
            if ( offset_u_in_byte <= 480 ) // can't actually fail, but just in case...
            {
              chararacter_u_in_byte = 32 * (character % 16);
              if ( offset_u_in_byte == 480 )
              {
                character_u_width_in_byte = ff7_japanese_edition ? (float)field_spacing * 0.5f : 32.0f;
                character_x_width = ff7_japanese_edition
                  ? field_spacing * 0.3125f
                  : 16.0f*scaleFactor; // scale character
              }
              else
              {
                character_u_width_in_byte = ff7_japanese_edition ? (float)field_spacing * 0.5f : 32.0f;
                character_x_width = ff7_japanese_edition
                  ? field_spacing * 0.3125f
                  : 16.0f * scaleFactor; // scale character
              }
              character_do_draw = common_externals.draw_graphics_object(1, (struct graphics_object*)graphics_object); // try and fetch the graphics object.
            }
            if ( character_do_draw )
            {
              auto color = get_character_color(character_n_shapes); // set color from variable

              character_u = (double)chararacter_u_in_byte / 512.0;
              character_v = (double)graphics_object_v_in_byte / 512.0;
              character_u_width = character_u_width_in_byte / 512.0;
              character_top_left = graphics_object->vertex_transform;
              character_top_left->position.x = (float)character_x;
              character_top_left->position.y = (float)character_y;
              character_top_left->position.z = z_value;
              character_top_left->position.w = 1.0;
              character_top_left->color = color; // the set color
              character_top_left->alpha_mask = -16777216;
              character_top_left->u = character_u;
              character_top_left->v = character_v;
              character_bottom_left = graphics_object->vertex_transform + 1;
              character_bottom_left->position.x = (float)character_x;
              character_bottom_left->position.y = (double)character_y + 16.0f*scaleFactor; // height is scaled
              character_bottom_left->position.z = z_value;
              character_bottom_left->position.w = 1.0;
              character_bottom_left->color = color;
              character_bottom_left->alpha_mask = -16777216;
              character_bottom_left->u = character_u;
              character_bottom_left->v = character_v + 32.0f / 512.0f;
              character_top_right = graphics_object->vertex_transform + 2;
              character_top_right->position.x = (double)character_x + (double)character_x_width; // must use full char width to render properly
              character_top_right->position.y = (float)character_y;
              character_top_right->position.z = z_value;
              character_top_right->position.w = 1.0;
              character_top_right->color = color;
              character_top_right->alpha_mask = -16777216;
              character_top_right->u = character_u + character_u_width;
              character_top_right->v = character_v;
              character_bottom_right = graphics_object->vertex_transform + 3;
              character_bottom_right->position.x = (double)character_x + (double)character_x_width;
              character_bottom_right->position.y = (double)character_y + 16.0f*scaleFactor;
              character_bottom_right->position.z = z_value;
              character_bottom_right->position.w = 1.0;
              character_bottom_right->color = color;
              character_bottom_right->alpha_mask = -16777216;
              character_bottom_right->u = character_u + character_u_width;
              character_bottom_right->v = character_v + 32.0f / 512.0f;
              *(byte *)graphics_object->curr_total_n_shape = 2 * character_n_shapes;
              graphics_object->field_7C = 2 * character_n_shapes;
              (*ff7_externals.field_do_draw_character_DC3CEC) = 1;
            }
            if ( (*ff7_externals.dword_DC3CD4) )  // if goign to next window
              character_x += 30; // extra padding
            else
              character_x += ff7_japanese_edition
                ? (int)(field_spacing * 0.3125f)
                : std::ceil(z_half_width(charWidth)*scaleFactor); // scaled up to match scaling we did above
            --(*ff7_externals.field_remaining_character_length_DC3CCC);
            ++buffer_text;
            ++(*ff7_externals.field_text_box_curr_n_characters_DC3CB0);
          }
          else
          {
            heartAtD9 = false;  // clear flag now that we are here
            isPrompt = false;   // clear flag now that we are here;
            int prompt_button = -1;
            if (jp_prompt_graphics_object)
            {
              switch (*buffer_text)
              {
                case 0xF6u:
                  switch (buffer_text[1])
                  {
                    case 0x33u: prompt_button = 5; break;  // Circle
                    case 0x34u: prompt_button = 2; break;  // L1
                    case 0x35u: prompt_button = 0; break;  // L2
                    case 0x36u: prompt_button = 3; break;  // R1
                    case 0x37u: prompt_button = 1; break;  // R2
                    case 0x38u: prompt_button = 11; break; // Start / Options
                    case 0x39u: prompt_button = 8; break;  // Select / Share
                    case 0x3Au: prompt_button = 12; break; // Up
                    case 0x3Bu: prompt_button = 14; break; // Down
                    case 0x3Cu: prompt_button = 15; break; // Left
                    case 0x3Du: prompt_button = 13; break; // Right
                    default: prompt_button = 5; break;     // Circle
                  }
                  if (buffer_text[1] >= 0x33u && buffer_text[1] <= 0x3Du)
                  {
                    ++buffer_text;
                    ++(*ff7_externals.field_text_box_curr_n_characters_DC3CB0);
                  }
                  break;
                case 0xF7u: prompt_button = 4; break; // Triangle
                case 0xF8u: prompt_button = 7; break; // Square
                case 0xF9u: prompt_button = 6; break; // Cross
              }
            }
            if (prompt_button >= 0)
            {
              int color_index = character_n_shapes;
              if (*ff7_externals.word_DC3CC4)
                color_index = ((unsigned __int8)(*ff7_externals.word_DC3CC8 >> 2) - character_count) & 7;
              else if (*ff7_externals.word_DC3CC0)
                color_index = ((*ff7_externals.word_DC3CC8 >> 2) & 1)
                  ? *ff7_externals.word_91F028
                  : 0;
              character_x = jp_draw_field_prompt_button(prompt_button, character_x, character_y, z_value);
              *ff7_externals.field_do_draw_text_boxes_DC3CE8 = 1;
              ++buffer_text;
              --(*ff7_externals.field_remaining_character_length_DC3CCC);
              ++(*ff7_externals.field_text_box_curr_n_characters_DC3CB0);
              break;
            }
            switch ( *buffer_text ) // what button prompt do we have?
            {
              case 0xD9u: // heart
                offset_u_in_byte = 144;
                graphics_object_v_in_byte = 208;
                graphics_object = *ff7_externals.menu_win_d_blend_4_graphics_object_DC0FD4;
                special_character_do_draw = common_externals.draw_graphics_object(1, (struct graphics_object*)graphics_object);
                break;

              case 0xF6u:
                // check for squeenix extended prompts, and grab from correct place in btl_win
                ++buffer_text; // go to next character
                switch (*buffer_text)
                {
                case 0x3Du: // , in jp sheet. right
                  offset_u_in_byte = 160;
                  graphics_object_v_in_byte = 96;
                  graphics_object = *ff7_externals.menu_win_a_blend_4_graphics_object_DC0FC8;
                  special_character_do_draw = common_externals.draw_graphics_object(1, (struct graphics_object*)graphics_object);
                  ++(*ff7_externals.field_text_box_curr_n_characters_DC3CB0); // finish advance because this was a doublebyte
                  break;
                case 0x3Cu: // 9 in JP sheet. left
                  offset_u_in_byte = 192;
                  graphics_object_v_in_byte = 96;
                  graphics_object = *ff7_externals.menu_win_a_blend_4_graphics_object_DC0FC8;
                  special_character_do_draw = common_externals.draw_graphics_object(1, (struct graphics_object*)graphics_object);
                  ++(*ff7_externals.field_text_box_curr_n_characters_DC3CB0); // finish advance because this was a doublebyte
                  break;
                case 0x3Bu: // 8 in JP sheet. down
                  offset_u_in_byte = 192;
                  graphics_object_v_in_byte = 64;
                  graphics_object = *ff7_externals.menu_win_a_blend_4_graphics_object_DC0FC8;
                  special_character_do_draw = common_externals.draw_graphics_object(1, (struct graphics_object*)graphics_object);
                  ++(*ff7_externals.field_text_box_curr_n_characters_DC3CB0); // finish advance because this was a doublebyte
                  break;
                case 0x3Au: // 7 in JP sheet. Up
                  offset_u_in_byte = 160;
                  graphics_object_v_in_byte = 64;
                  graphics_object = *ff7_externals.menu_win_a_blend_4_graphics_object_DC0FC8;
                  special_character_do_draw = common_externals.draw_graphics_object(1, (struct graphics_object*)graphics_object);
                  ++(*ff7_externals.field_text_box_curr_n_characters_DC3CB0); // finish advance because this was a doublebyte
                  break;
                case 0x39u: // 6 in JP sheet. select/b9
                  offset_u_in_byte = 192;
                  graphics_object_v_in_byte = 192;
                  graphics_object = *ff7_externals.menu_win_a_blend_4_graphics_object_DC0FC8;
                  special_character_do_draw = common_externals.draw_graphics_object(1, (struct graphics_object*)graphics_object);
                  ++(*ff7_externals.field_text_box_curr_n_characters_DC3CB0); // finish advance because this was a doublebyte
                  break;
                case 0x38u: // 5 in JP sheet. start/b10
                  offset_u_in_byte = 224;
                  graphics_object_v_in_byte = 192;
                  graphics_object = *ff7_externals.menu_win_a_blend_4_graphics_object_DC0FC8;
                  special_character_do_draw = common_externals.draw_graphics_object(1, (struct graphics_object*)graphics_object);
                  ++(*ff7_externals.field_text_box_curr_n_characters_DC3CB0); // finish advance because this was a doublebyte
                  break;
                case 0x37u: // 4 in JP sheet r2/b8
                  offset_u_in_byte = 224;
                  graphics_object_v_in_byte = 160;
                  graphics_object = *ff7_externals.menu_win_a_blend_4_graphics_object_DC0FC8;
                  special_character_do_draw = common_externals.draw_graphics_object(1, (struct graphics_object*)graphics_object);
                  ++(*ff7_externals.field_text_box_curr_n_characters_DC3CB0); // finish advance because this was a doublebyte
                  break;
                case 0x36u: // 3 in JP sheet. r1/b6
                  offset_u_in_byte = 192;
                  graphics_object_v_in_byte = 160;
                  graphics_object = *ff7_externals.menu_win_a_blend_4_graphics_object_DC0FC8;
                  special_character_do_draw = common_externals.draw_graphics_object(1, (struct graphics_object*)graphics_object);
                  ++(*ff7_externals.field_text_box_curr_n_characters_DC3CB0); // finish advance because this was a doublebyte
                  break;
                case 0x35u: // 2 in jp sheet. l2/b7
                  offset_u_in_byte = 32;
                  graphics_object_v_in_byte = 160;
                  graphics_object = *ff7_externals.menu_win_b_blend_4_graphics_object_DC0FCC;
                  special_character_do_draw = common_externals.draw_graphics_object(1, (struct graphics_object*)graphics_object);
                  ++(*ff7_externals.field_text_box_curr_n_characters_DC3CB0); // finish advance because this was a doublebyte
                  break;
                case 0x34u: // 1 in jp sheet. l1/b5
                  offset_u_in_byte = 0;
                  graphics_object_v_in_byte = 160;
                  graphics_object = *ff7_externals.menu_win_b_blend_4_graphics_object_DC0FCC;
                  special_character_do_draw = common_externals.draw_graphics_object(1, (struct graphics_object*)graphics_object);
                  ++(*ff7_externals.field_text_box_curr_n_characters_DC3CB0); // finish advance because this was a doublebyte
                  break;
                case 0x33u: // 0 in JP sheet. circle/B3, for now, as we haven't implemented true x/o swap
                  offset_u_in_byte = 192;
                  graphics_object_v_in_byte = 128;
                  graphics_object = *ff7_externals.menu_win_a_blend_4_graphics_object_DC0FC8;
                  special_character_do_draw = common_externals.draw_graphics_object(1, (struct graphics_object*)graphics_object);
                  ++(*ff7_externals.field_text_box_curr_n_characters_DC3CB0); // finish advance because this was a doublebyte
                  break;
                default:
                  --buffer_text; // not extended code, undo the peek and don't advance. normal circle/B3 prompt
                  offset_u_in_byte = 192;
                  graphics_object_v_in_byte = 128;
                  graphics_object = *ff7_externals.menu_win_a_blend_4_graphics_object_DC0FC8;
                  special_character_do_draw = common_externals.draw_graphics_object(1, (struct graphics_object*)graphics_object);
                  break;
               }
                break;
              case 0xF7u:
                offset_u_in_byte = 32;
                graphics_object_v_in_byte = 128;
                graphics_object = *ff7_externals.menu_win_b_blend_4_graphics_object_DC0FCC;
                special_character_do_draw = common_externals.draw_graphics_object(1, (struct graphics_object*)graphics_object);
                break;
              case 0xF8u:
                offset_u_in_byte = 0;
                graphics_object_v_in_byte = 128;
                graphics_object = *ff7_externals.menu_win_b_blend_4_graphics_object_DC0FCC;
                special_character_do_draw = common_externals.draw_graphics_object(1, (struct graphics_object*)graphics_object);
                break;
              case 0xF9u:
                offset_u_in_byte = 224;
                graphics_object_v_in_byte = 128;
                graphics_object = *ff7_externals.menu_win_a_blend_4_graphics_object_DC0FC8;
                special_character_do_draw = common_externals.draw_graphics_object(1, (struct graphics_object*)graphics_object);

              default:
                break;
            }
            if ( (*ff7_externals.word_DC3CC0) || (*ff7_externals.word_DC3CC4) ) // if a color flag is set
            {
              if ( (*ff7_externals.word_DC3CC4) ) // rainboe
              {
                character_n_shapes = ((unsigned __int8)((*ff7_externals.word_DC3CC8) >> 2) - character_count) & 7; // get character color, but modify by character count
              }
              else if ( (((*ff7_externals.word_DC3CC8) >> 2) & 1) != 0 ) // flash
              {
                character_n_shapes = (*ff7_externals.word_91F028); // get flash color
              }
              else
              {
                character_n_shapes = 0;  // go back to normal is the color is cleared.
              }
            }
            if ( special_character_do_draw )
            {
              auto color = offset_u_in_byte == 144
                ? get_character_color(character_n_shapes)
                : get_character_color(7);

              special_character_u = (double)offset_u_in_byte / 256.0f;
              special_character_top_left = graphics_object->vertex_transform;
              special_character_top_left->position.x = (float)character_x;
              special_character_top_left->position.y = (double)character_y;
              special_character_top_left->position.z = z_value;
              special_character_top_left->position.w = 1.0;
              special_character_top_left->color = color;
              special_character_top_left->alpha_mask = -16777216;
              special_character_top_left->u = special_character_u;
              special_character_top_left->v = (double)graphics_object_v_in_byte / 256.0f; // no longer ignores graphics_object_v_in_byte
              special_character_bottom_left = graphics_object->vertex_transform + 1;
              special_character_bottom_left->position.x = (float)character_x;
              special_character_bottom_left->position.y = (double)character_y + 16.0f*scaleFactor; // same scaling as i did for JP text.
              special_character_bottom_left->position.z = z_value;
              special_character_bottom_left->position.w = 1.0;
              special_character_bottom_left->color = color;
              special_character_bottom_left->alpha_mask = -16777216;
              special_character_bottom_left->u = special_character_u;
              if (offset_u_in_byte == 144) // it's the heart
                special_character_bottom_left->v = (double)graphics_object_v_in_byte / 256.0f + 0.0625; // no longer ignores graphics_object_v_in_byte
              else
                special_character_bottom_left->v = (double)graphics_object_v_in_byte / 256.0f + 0.125; // no longer ignores graphics_object_v_in_byte
              special_character_top_right = graphics_object->vertex_transform + 2;
              special_character_top_right->position.x = (double)character_x + 16.0f * scaleFactor;
              special_character_top_right->position.y = (double)character_y;
              special_character_top_right->position.z = z_value;
              special_character_top_right->position.w = 1.0;
              special_character_top_right->color = color;
              special_character_top_right->alpha_mask = -16777216;
              if (offset_u_in_byte == 144) // it's the heart
                special_character_top_right->u = special_character_u + 0.0625;
              else
                special_character_top_right->u = special_character_u + 0.125;
              special_character_top_right->v = (double)graphics_object_v_in_byte / 256.0f; // no longer ignores graphics_object_v_in_byte
              window_vertices = graphics_object->vertex_transform;
              window_vertices[3].position.x = (double)character_x + 16.0f * scaleFactor;
              window_vertices[3].position.y = (double)character_y + 16.0f * scaleFactor;
              window_vertices[3].position.z = z_value;
              window_vertices[3].position.w = 1.0;
              window_vertices[3].color = color;
              window_vertices[3].alpha_mask = -16777216;
              if (offset_u_in_byte == 144) // it's the heart
                window_vertices[3].u = special_character_u + 0.0625;
              else
                window_vertices[3].u = special_character_u + 0.125;
              if (offset_u_in_byte == 144) // it's the heart
                window_vertices[3].v = (double)graphics_object_v_in_byte / 256.0f + 0.0625; // no longer ignores graphics_object_v_in_byte
              else
                window_vertices[3].v = (double)graphics_object_v_in_byte / 256.0f + 0.125; // no longer ignores graphics_object_v_in_byte
              *(byte *)graphics_object->curr_total_n_shape = 7;
              graphics_object->field_7C = 7;
            }
            (*ff7_externals.field_do_draw_text_boxes_DC3CE8) = 1;
            ++buffer_text;
            --(*ff7_externals.field_remaining_character_length_DC3CCC);
            ++(*ff7_externals.field_text_box_curr_n_characters_DC3CB0);
            character_x += (short)(16.0f * scaleFactor);
          }
          break;
      }
    }
    ++character_count;
  }
  return character_y;
}

void field_draw_text_boxes_and_text_graphics_object_6ECA68_jp()
{
  ff7_game_obj *game_object;

  game_object = ff7_externals.engine_get_game_object_676578();
  if ( *ff7_externals.menu_is_small_viewport_320_240_DC130C == 1 )
  {
    // 320x240 viewport not needed
  }
  else
  {
    if ( *ff7_externals.dword_DC3CE0 )
    {
      ff7_externals.engine_gfx_draw_predefined_polygon_set_field_84_sub_660E95(0, game_object);
      ff7_externals.engine_gfx_set_single_renderstate_sub_660C3A(2, 0, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_window_bg_graphics_object_DC0FF0, game_object);
      ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_window_bg_graphics_object_DC0FF0);
      *ff7_externals.dword_DC3CE0 = 0;
    }
    if ( *ff7_externals.dword_DC3CDC )
    {
      ff7_externals.engine_gfx_draw_predefined_polygon_set_field_84_sub_660E95(1, game_object);
      ff7_externals.engine_gfx_set_single_renderstate_sub_660C3A(2, 0, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_blend_window_bg_graphics_object_DC0FF4, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_a_blend_0_graphics_object_DC0FDC, game_object);
      ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_win_a_blend_0_graphics_object_DC0FDC);
      ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_blend_window_bg_graphics_object_DC0FF4);
      *ff7_externals.dword_DC3CDC = 0;
    }
    ff7_externals.engine_gfx_draw_predefined_polygon_set_field_84_sub_660E95(0, game_object);
    ff7_externals.engine_gfx_set_single_renderstate_sub_660C3A(2, 0, game_object);
    if ( *ff7_externals.field_do_draw_character_DC3CEC )
    {
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_font_a_graphics_object_DC100C, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_font_b_graphics_object_DC1010, game_object);
      ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_font_a_graphics_object_DC100C);
      ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_font_b_graphics_object_DC1010);

      // jp
      ff7_externals.engine_draw_graphics_object_66E641(ff7_externals.menu_jafont_1_graphics_object, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(ff7_externals.menu_jafont_2_graphics_object, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(ff7_externals.menu_jafont_3_graphics_object, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(ff7_externals.menu_jafont_4_graphics_object, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(ff7_externals.menu_jafont_5_graphics_object, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(ff7_externals.menu_jafont_6_graphics_object, game_object);
      ff7_externals.reset_field_54_graphics_object_66E62C(ff7_externals.menu_jafont_1_graphics_object);
      ff7_externals.reset_field_54_graphics_object_66E62C(ff7_externals.menu_jafont_2_graphics_object);
      ff7_externals.reset_field_54_graphics_object_66E62C(ff7_externals.menu_jafont_3_graphics_object);
      ff7_externals.reset_field_54_graphics_object_66E62C(ff7_externals.menu_jafont_4_graphics_object);
      ff7_externals.reset_field_54_graphics_object_66E62C(ff7_externals.menu_jafont_5_graphics_object);
      ff7_externals.reset_field_54_graphics_object_66E62C(ff7_externals.menu_jafont_6_graphics_object);

      *ff7_externals.field_do_draw_character_DC3CEC = 0;
    }
    if ( *ff7_externals.field_do_draw_text_boxes_DC3CE8 || *ff7_externals.text_box_do_draw_menu_win_c_blend_4_DC3CE4 )
    {
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_a_blend_4_graphics_object_DC0FC8, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_b_blend_4_graphics_object_DC0FCC, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_c_blend_4_graphics_object_DC0FD0, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_c_blend_4_diff_graphics_object_DC0FD8, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_d_blend_4_graphics_object_DC0FD4, game_object);
      jp_draw_prompt_graphics_object(game_object);
      ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_win_a_blend_4_graphics_object_DC0FC8);
      ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_win_b_blend_4_graphics_object_DC0FCC);
      ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_win_c_blend_4_graphics_object_DC0FD0);
      ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_win_d_blend_4_graphics_object_DC0FD4);
      ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_win_c_blend_4_diff_graphics_object_DC0FD8);
      if (jp_prompt_graphics_object)
        ff7_externals.reset_field_54_graphics_object_66E62C(jp_prompt_graphics_object);
      *ff7_externals.text_box_do_draw_menu_win_c_blend_4_DC3CE4 = 0;
      *ff7_externals.field_do_draw_text_boxes_DC3CE8 = 0;
    }
    if ( *ff7_externals.text_box_do_draw_black_quad_graphics_object_DC3CF0 )
    {
      ff7_externals.engine_gfx_draw_predefined_polygon_set_field_84_sub_660E95(1, game_object);
      ff7_externals.engine_gfx_set_single_renderstate_sub_660C3A(2, 0, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_text_box_quad_graphics_object_DC1008, game_object);
      ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_text_box_quad_graphics_object_DC1008);
      *ff7_externals.dword_DC3D00 = 0;
      *ff7_externals.text_box_do_draw_black_quad_graphics_object_DC3CF0 = 0;
      if ( *ff7_externals.do_draw_text_box_DC3CF8 )
        *ff7_externals.should_draw_text_box_black_quad_DC3D04 = 0;
    }
  }
}

static int jp_submit_draw_text_from_buffer(int16_t x, int16_t y, byte* buffer, byte n_shapes, float z_value, bool small_glyphs)
{
  bool previous_small_glyphs = jp_small_glyphs;
  jp_small_glyphs = small_glyphs;
  if (!buffer)
  {
    jp_small_glyphs = previous_small_glyphs;
    return x;
  }

  struct game_mode* mode = getmode_cached();
  if (mode->driver_mode == MODE_MENU && buffer == ff7_externals.menu_time_label)
    x += 4;

  for (int i = 0; i < 1024 && buffer[i] != 0xFF; ++i)
  {
    uint16_t letter = buffer[i];
    if (buffer[i] >= 0xF8 && buffer[i] <= 0xFE && buffer[i + 1] != 0xFF)
      letter = (uint16_t)(buffer[i] << 8 | buffer[++i]);
    x = (int16_t)common_submit_draw_char_from_buffer_6F564E_jp(x, y, n_shapes, letter, z_value);
  }
  jp_small_glyphs = previous_small_glyphs;
  return x;
}

int common_submit_draw_text_from_buffer_jp(int16_t x, int16_t y, byte* buffer, byte n_shapes, float z_value)
{
  return jp_submit_draw_text_from_buffer(x, y, buffer, n_shapes, z_value, true);
}

int common_submit_draw_text_from_buffer_large_jp(int16_t x, int16_t y, byte* buffer, byte n_shapes, float z_value)
{
  return jp_submit_draw_text_from_buffer(x, y, buffer, n_shapes, z_value, false);
}

int common_submit_draw_char_from_buffer_large_6F564E_jp(int x, int vertex_y, int n_shapes, uint16_t letter, float z_value)
{
  bool previous_small_glyphs = jp_small_glyphs;
  jp_small_glyphs = false;
  int ret = common_submit_draw_char_from_buffer_6F564E_jp(x, vertex_y, n_shapes, letter, z_value);
  jp_small_glyphs = previous_small_glyphs;
  return ret;
}

int common_submit_draw_char_from_buffer_6F564E_jp(int x, int vertex_y, int n_shapes, uint16_t letter, float z_value)
{
  multibyte_load_widths();   // 1s-gated hot-reload for live width tuning

  // FIXME: this function can draw characters with different scaling, dependent on what sorta text is being printed.
  // But it needs to know what the source of hte text that was put into the buffer was to work this out, and that info is NOT passed as a parameter
  // will need to hook the function that loads texts to the buffer and set a global based on where in memory the original text is.

  double scaleFactor = jp_small_glyphs ? 1.0f : 1.25f;
  float xPosFudge = 0;
  float yPosFudge = jp_small_glyphs ? 4.0f : 0.0f;
  graphics_vertex* bottom_right;
  graphics_vertex* top_right;
  graphics_vertex* bottom_left;
  graphics_vertex* top_left;
  float vertex_u_width;
  float vertex_v;
  float vertex_u;
  uint16_t character;
  ff7_graphics_object* character_graphics_object;
  int16_t offset_text_spacing;
  float vertex_width;
  int16_t image_u;
  int16_t offset_image_v;
  int16_t image_v;
  int16_t offset_image_u;
  byte* p_letter;
  float image_u_width;
  int vertex_x;
  bool heartAtD9 = false;

  int charWidth = 16;
  int leftPadding = 0;
  uint16_t original_letter = letter;

  if ((letter & 0xFF) >= 0xFA && (letter & 0xFF) <= 0xFE
      && ((letter >> 8) < 0xFA || (letter >> 8) > 0xFE))
    letter = (uint16_t)((letter << 8) | (letter >> 8));
  original_letter = letter;
  p_letter = (byte*)&letter;
  offset_image_u = 0; // initialise to zero
  offset_image_v = 0;
  if (!ff7_japanese_edition && charWidthData[0][0xD9u] == 0)
    heartAtD9 = true;
  if (letter == 0xF8)
    return x;
  if (letter == 0xD9 && (ff7_japanese_edition || heartAtD9))
  {
    character_graphics_object = *ff7_externals.menu_win_d_blend_4_graphics_object_DC0FD4;
    offset_image_u = 144;
    offset_image_v = 208;
    charWidth = 0x1f;
    leftPadding = 0;
    goto LABEL_9;
  }
  switch ((byte)(letter >> 8))
  {
  case 0x00:
    character_graphics_object = ff7_externals.menu_jafont_1_graphics_object;
    break;
  case 0xF8:
    return x;
  case 0xFA:
    p_letter = (byte*)&letter;
    character_graphics_object = ff7_externals.menu_jafont_2_graphics_object;
    goto LABEL_9;
  case 0xFB:
    p_letter = (byte*)&letter;
    character_graphics_object = ff7_externals.menu_jafont_3_graphics_object;
    goto LABEL_9;
  case 0xFC:
    p_letter = (byte*)&letter;
    character_graphics_object = ff7_externals.menu_jafont_4_graphics_object;
    goto LABEL_9;
  case 0xFD:
    p_letter = (byte*)&letter;
    character_graphics_object = ff7_externals.menu_jafont_5_graphics_object;
    goto LABEL_9;
  case 0xFE:
    p_letter = (byte*)&letter;
    character_graphics_object = ff7_externals.menu_jafont_6_graphics_object;
    goto LABEL_9;
  default:
    character_graphics_object = ff7_externals.menu_jafont_1_graphics_object;
    break;
  }

  if (!ff7_japanese_edition)
  {
    int page = 0;
    if ((letter >> 8) >= 0xFA && (letter >> 8) <= 0xFE)
      page = (letter >> 8) - 0xF9;
    charWidth = charWidthData[page][*p_letter] & 0x1F;
    leftPadding = charWidthData[page][*p_letter] >> 5;
  }

  switch ((byte)letter)
  {
  default:
    offset_text_spacing = 0;
    offset_image_v = 0;
  LABEL_9:
    letter = *(byte*)p_letter;
    if (offset_image_u == 0) // only do this if we idn't set stuff above for the heart.
    {
      offset_image_u = 32 * (letter % 16);
      image_v = 32 * (letter / 16) + offset_image_v;
      image_u = 32 * (letter % 16);
      if (offset_image_u <= 480)
      {
        if (offset_image_u == 480)
        {
          image_u_width = ff7_japanese_edition ? (float)jp_spacing_metric(original_letter, charWidth) * 0.5f : 32.0f;
          vertex_width = ff7_japanese_edition ? (float)jp_spacing_metric(original_letter, charWidth) * 0.25f : 16;
        }
        else
        {
          image_u_width = ff7_japanese_edition ? (float)jp_spacing_metric(original_letter, charWidth) * 0.5f : 32.0f;
          vertex_width = ff7_japanese_edition ? (float)jp_spacing_metric(original_letter, charWidth) * 0.25f : 16;
        }
      }
      else
      {
        image_u_width = ff7_japanese_edition ? (float)jp_spacing_metric(original_letter, charWidth) * 0.5f : 32.0f;
        vertex_width = ff7_japanese_edition ? (float)jp_spacing_metric(original_letter, charWidth) * 0.25f : 16;
      }
    }
    else
    {
      image_u_width = 16.0;  // heart.
      vertex_width = 16;
      image_u = offset_image_u; // exactly equal to offset set above
      image_v = offset_image_v;
    }
    vertex_x = x + leftPadding;
    if (ff7_externals.g_get_do_render_menu_6CDBF2() && common_externals.draw_graphics_object(1, (struct graphics_object*)character_graphics_object))
    {
      auto color = get_character_color(n_shapes);
      vertex_u = (double)image_u / 512.0f;
      if (offset_image_u == 144) // heart
      {
        vertex_u = (double)vertex_u * 2.0f; // image half as big
      }
      vertex_v = (double)image_v / 512.0f;
      if (offset_image_u == 144) // heart
      {
        vertex_v = (double)vertex_v * 2.0f; // image half as big
      }
      vertex_u_width = image_u_width / 512.0f;
      if (offset_image_u == 144) // heart
      {
        vertex_u_width = (double)vertex_u_width * 2.0f; // image half as big
      }
      top_left = character_graphics_object->vertex_transform;
      top_left->position.x = (float)vertex_x + xPosFudge;
      top_left->position.y = (float)vertex_y + yPosFudge;
      top_left->position.z = z_value;
      top_left->position.w = 1.0;
      top_left->color = color;
      top_left->alpha_mask = 0xFF000000;
      if (offset_image_u == 144) // heart
        top_left->alpha_mask = -16777216;
      top_left->u = vertex_u;
      top_left->v = vertex_v;
      bottom_left = character_graphics_object->vertex_transform + 1;
      bottom_left->position.x = (float)vertex_x + xPosFudge;
      bottom_left->position.y = (double)vertex_y + 16.0 * scaleFactor + yPosFudge;
      bottom_left->position.z = z_value;
      bottom_left->position.w = 1.0;
      bottom_left->color = color;
      bottom_left->alpha_mask = 0xFF000000;
      if (offset_image_u == 144) // heart
        bottom_left->alpha_mask = -16777216;
      bottom_left->u = vertex_u;
      bottom_left->v = vertex_v + 32.0f / 512.0f;
      top_right = character_graphics_object->vertex_transform + 2;
      top_right->position.x = (double)vertex_x + (double)vertex_width * scaleFactor + xPosFudge;
      top_right->position.y = (float)vertex_y + yPosFudge;
      top_right->position.z = z_value;
      top_right->position.w = 1.0;
      top_right->color = color;
      top_right->alpha_mask = 0xFF000000;
      if (offset_image_u == 144) // heart
        top_right->alpha_mask = -16777216;
      top_right->u = vertex_u + vertex_u_width;
      top_right->v = vertex_v;
      bottom_right = character_graphics_object->vertex_transform + 3;
      bottom_right->position.x = (double)vertex_x + (double)vertex_width * scaleFactor + xPosFudge;
      bottom_right->position.y = (double)vertex_y + 16.0 * scaleFactor + yPosFudge;
      bottom_right->position.z = z_value;
      bottom_right->position.w = 1.0;
      bottom_right->color = color;
      bottom_right->alpha_mask = -16777216;
      bottom_right->u = vertex_u + vertex_u_width;
      bottom_right->v = vertex_v + 32.0f / 512.0f;
      *(byte*)character_graphics_object->curr_total_n_shape = 2 * n_shapes;
      character_graphics_object->field_7C = 2 * n_shapes;
      if (offset_image_u == 144) // heart
      {
        *(byte*)character_graphics_object->curr_total_n_shape = n_shapes & 7;
        character_graphics_object->field_7C = n_shapes & 7;
      }
    }
    return ff7_japanese_edition
      ? x + (int)((float)jp_spacing_metric(original_letter, charWidth)
          * (jp_small_glyphs ? 0.25f : 0.3125f))
      : vertex_x + std::ceil(z_half_width(charWidth) * scaleFactor);
  }
}

void menu_draw_everything_6CC9D3_jp()
{
  ff7_game_obj* game_object;

  if (ff7_externals.g_get_do_render_menu_6CDBF2())
  {
    game_object = ff7_externals.engine_get_game_object_676578();
    ff7_externals.engine_gfx_draw_predefined_polygon_set_field_84_sub_660E95(0, game_object);
    ff7_externals.engine_gfx_set_single_renderstate_sub_660C3A(2, 0, game_object);
    ff7_externals.engine_gfx_draw_graphics_object_polygon_set_field_80_sub_660E6A(*ff7_externals.menu_unknown3_graphics_object_DC0FFC, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_window_bg_graphics_object_DC0FF0, game_object);
    if (*ff7_externals.menu_is_small_viewport_320_240_DC130C == 1)
    {
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_blend_4_graphics_object_DC104C, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_c_blend_4_diff_graphics_object_DC0FD8, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_font_blend_4_graphics_object_DC1048, game_object);
    }
    else
    {
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_a_blend_4_graphics_object_DC0FC8, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_c_blend_4_graphics_object_DC0FD0, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_c_blend_4_diff_graphics_object_DC0FD8, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_b_blend_4_graphics_object_DC0FCC, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_d_blend_4_graphics_object_DC0FD4, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_font_a_graphics_object_DC100C, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_font_b_graphics_object_DC1010, game_object);

      // jp
      ff7_externals.engine_draw_graphics_object_66E641(ff7_externals.menu_jafont_1_graphics_object, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(ff7_externals.menu_jafont_2_graphics_object, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(ff7_externals.menu_jafont_3_graphics_object, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(ff7_externals.menu_jafont_4_graphics_object, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(ff7_externals.menu_jafont_5_graphics_object, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(ff7_externals.menu_jafont_6_graphics_object, game_object);
    }
    if (*ff7_externals.dword_DC12EC == 9 || *ff7_externals.dword_DC12E4)
    {
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_avatar2_1_graphics_object_DC1020, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_avatar2_2_graphics_object_DC1024, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_avatar2_3_graphics_object_DC1028, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_avatar2_4_graphics_object_DC102C, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_avatar2_5_graphics_object_DC1030, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_avatar2_6_graphics_object_DC1034, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_avatar2_7_graphics_object_DC1038, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_avatar2_8_graphics_object_DC103C, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_avatar2_9_graphics_object_DC1040, game_object);
    }
    else
    {
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_avatar_1_graphics_object_DC1014, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_avatar_2_graphics_object_DC1018, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_avatar_3_graphics_object_DC101C, game_object);
    }
    if (*ff7_externals.engine_game_mode_word_CBF9DC == 20)
    {
      ff7_externals.engine_gfx_set_single_renderstate_sub_660C3A(2, 1, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_buster_tex_graphics_object_DC1044, game_object);
      ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_buster_tex_graphics_object_DC1044);
    }
    ff7_externals.engine_gfx_draw_predefined_polygon_set_field_84_sub_660E95(1, game_object);
    ff7_externals.engine_gfx_set_single_renderstate_sub_660C3A(2, 0, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_unknown4_graphics_object_DC1000, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_blend_window_bg_graphics_object_DC0FF4, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_unknown2_graphics_object_DC0FF8, game_object);
    if (*ff7_externals.menu_is_small_viewport_320_240_DC130C == 1)
    {
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_blend_0_graphics_object_DC1050, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_blend_1_graphics_object_DC1054, game_object);
    }
    else
    {
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_a_blend_0_graphics_object_DC0FDC, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_a_blend_1_graphics_object_DC0FE0, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_c_blend_1_graphics_object_DC0FE8, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_b_blend_1_graphics_object_DC0FE4, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_d_blend_1_graphics_object_DC0FEC, game_object);
    }
    ff7_externals.engine_gfx_set_single_renderstate_sub_660C3A(9, 1, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_text_box_quad_graphics_object_DC1008, game_object);
    ff7_externals.engine_gfx_setviewport_sub_66067A(*ff7_externals.menu_viewport_x_DC105C, *ff7_externals.menu_viewport_y_DC1060, *ff7_externals.menu_viewport_width_DC1064, *ff7_externals.menu_viewport_view_DC1068, game_object);
  }
}

static void draw_jafonts(ff7_game_obj* game_object)
{
  ff7_graphics_object* jafont_objects[] = {
    ff7_externals.menu_jafont_1_graphics_object,
    ff7_externals.menu_jafont_2_graphics_object,
    ff7_externals.menu_jafont_3_graphics_object,
    ff7_externals.menu_jafont_4_graphics_object,
    ff7_externals.menu_jafont_5_graphics_object,
    ff7_externals.menu_jafont_6_graphics_object,
  };

  for (ff7_graphics_object* graphics_object : jafont_objects)
    ff7_externals.engine_draw_graphics_object_66E641(graphics_object, game_object);
}

static void reset_jafonts()
{
  ff7_graphics_object* jafont_objects[] = {
    ff7_externals.menu_jafont_1_graphics_object,
    ff7_externals.menu_jafont_2_graphics_object,
    ff7_externals.menu_jafont_3_graphics_object,
    ff7_externals.menu_jafont_4_graphics_object,
    ff7_externals.menu_jafont_5_graphics_object,
    ff7_externals.menu_jafont_6_graphics_object,
  };

  for (ff7_graphics_object* graphics_object : jafont_objects)
    ff7_externals.reset_field_54_graphics_object_66E62C(graphics_object);
}

ff7_game_obj* menu_draw_with_viewport_6FA12F_jp(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
  ff7_game_obj* game_object = ff7_externals.engine_get_game_object_676578();

  ff7_externals.engine_gfx_draw_predefined_polygon_set_field_84_sub_660E95(0, game_object);
  ff7_externals.engine_gfx_set_single_renderstate_sub_660C3A(2, 0, game_object);
  ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_window_bg_graphics_object_DC0FF0, game_object);
  ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_a_blend_4_graphics_object_DC0FC8, game_object);
  ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_b_blend_4_graphics_object_DC0FCC, game_object);
  ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_c_blend_4_graphics_object_DC0FD0, game_object);
  ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_c_blend_4_diff_graphics_object_DC0FD8, game_object);
  ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_d_blend_4_graphics_object_DC0FD4, game_object);
  ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_font_a_graphics_object_DC100C, game_object);
  ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_font_b_graphics_object_DC1010, game_object);
  draw_jafonts(game_object);
  ff7_externals.engine_gfx_draw_predefined_polygon_set_field_84_sub_660E95(1, game_object);
  ff7_externals.engine_gfx_set_single_renderstate_sub_660C3A(2, 0, game_object);
  ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_a_blend_1_graphics_object_DC0FE0, game_object);
  ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_b_blend_1_graphics_object_DC0FE4, game_object);
  ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_c_blend_1_graphics_object_DC0FE8, game_object);
  ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_d_blend_1_graphics_object_DC0FEC, game_object);
  ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_blend_window_bg_graphics_object_DC0FF4, game_object);
  ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_blend_window_bg_graphics_object_DC0FF4);
  ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_font_a_graphics_object_DC100C);
  ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_font_b_graphics_object_DC1010);
  reset_jafonts();
  ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_win_a_blend_4_graphics_object_DC0FC8);
  ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_win_b_blend_4_graphics_object_DC0FCC);
  ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_win_c_blend_4_graphics_object_DC0FD0);
  ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_win_c_blend_4_diff_graphics_object_DC0FD8);
  ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_win_d_blend_4_graphics_object_DC0FD4);
  ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_win_a_blend_1_graphics_object_DC0FE0);
  ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_win_b_blend_1_graphics_object_DC0FE4);
  ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_win_c_blend_1_graphics_object_DC0FE8);
  ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_win_d_blend_1_graphics_object_DC0FEC);
  ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_window_bg_graphics_object_DC0FF0);
  return ff7_externals.engine_gfx_setviewport_sub_66067A(x, y, width, height, game_object);
}

ff7_game_obj* menu_draw_640x480_6FA347_jp()
{
  ff7_game_obj* game_object = ff7_externals.engine_get_game_object_676578();

  ff7_externals.engine_gfx_draw_predefined_polygon_set_field_84_sub_660E95(0, game_object);
  ff7_externals.engine_gfx_set_single_renderstate_sub_660C3A(2, 0, game_object);
  ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_window_bg_graphics_object_DC0FF0, game_object);
  ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_a_blend_4_graphics_object_DC0FC8, game_object);
  ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_b_blend_4_graphics_object_DC0FCC, game_object);
  ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_c_blend_4_graphics_object_DC0FD0, game_object);
  ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_c_blend_4_diff_graphics_object_DC0FD8, game_object);
  ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_d_blend_4_graphics_object_DC0FD4, game_object);
  ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_font_a_graphics_object_DC100C, game_object);
  ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_font_b_graphics_object_DC1010, game_object);
  draw_jafonts(game_object);
  ff7_externals.engine_gfx_draw_predefined_polygon_set_field_84_sub_660E95(1, game_object);
  ff7_externals.engine_gfx_set_single_renderstate_sub_660C3A(2, 0, game_object);
  ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_blend_window_bg_graphics_object_DC0FF4, game_object);
  ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_blend_window_bg_graphics_object_DC0FF4);
  ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_a_blend_1_graphics_object_DC0FE0, game_object);
  ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_b_blend_1_graphics_object_DC0FE4, game_object);
  ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_c_blend_1_graphics_object_DC0FE8, game_object);
  ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_d_blend_1_graphics_object_DC0FEC, game_object);
  ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_font_a_graphics_object_DC100C);
  ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_font_b_graphics_object_DC1010);
  reset_jafonts();
  ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_window_bg_graphics_object_DC0FF0);
  ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_win_a_blend_4_graphics_object_DC0FC8);
  ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_win_b_blend_4_graphics_object_DC0FCC);
  ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_win_c_blend_4_graphics_object_DC0FD0);
  ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_win_c_blend_4_diff_graphics_object_DC0FD8);
  ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_win_d_blend_4_graphics_object_DC0FD4);
  ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_win_a_blend_1_graphics_object_DC0FE0);
  ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_win_b_blend_1_graphics_object_DC0FE4);
  ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_win_c_blend_1_graphics_object_DC0FE8);
  ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_win_d_blend_1_graphics_object_DC0FEC);
  return ff7_externals.engine_gfx_setviewport_sub_66067A(0, 0, 640, 480, game_object);
}

void battle_draw_menu_everything_6CEE84_jp()
{
  ff7_game_obj *game_object;

  game_object = ff7_externals.engine_get_game_object_676578();
  if ( *ff7_externals.menu_is_small_viewport_320_240_DC130C == 1 )
  {
    ff7_externals.engine_gfx_draw_predefined_polygon_set_field_84_sub_660E95(0, game_object);
    ff7_externals.engine_gfx_set_single_renderstate_sub_660C3A(2, 0, game_object);
    ff7_externals.engine_gfx_draw_graphics_object_polygon_set_field_80_sub_660E6A(*ff7_externals.menu_unknown3_graphics_object_DC0FFC, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_window_bg_graphics_object_DC0FF0, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_blend_4_graphics_object_DC104C, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_font_blend_4_graphics_object_DC1048, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_c_blend_4_diff_graphics_object_DC0FD8, game_object);
    ff7_externals.engine_gfx_draw_predefined_polygon_set_field_84_sub_660E95(1, game_object);
    ff7_externals.engine_gfx_set_single_renderstate_sub_660C3A(2, 0, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_unknown4_graphics_object_DC1000, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_unknown5_graphics_object_DC1004, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_blend_window_bg_graphics_object_DC0FF4, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_unknown2_graphics_object_DC0FF8, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_blend_0_graphics_object_DC1050, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_blend_1_graphics_object_DC1054, game_object);
  }
  else
  {
    ff7_externals.engine_gfx_draw_predefined_polygon_set_field_84_sub_660E95(0, game_object);
    ff7_externals.engine_gfx_set_single_renderstate_sub_660C3A(2, 0, game_object);
    ff7_externals.engine_gfx_draw_graphics_object_polygon_set_field_80_sub_660E6A(*ff7_externals.menu_unknown3_graphics_object_DC0FFC, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_window_bg_graphics_object_DC0FF0, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_a_blend_4_graphics_object_DC0FC8, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_b_blend_4_graphics_object_DC0FCC, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_c_blend_4_graphics_object_DC0FD0, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_c_blend_4_diff_graphics_object_DC0FD8, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_d_blend_4_graphics_object_DC0FD4, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_font_a_graphics_object_DC100C, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_font_b_graphics_object_DC1010, game_object);

    // jp
    ff7_externals.engine_draw_graphics_object_66E641(ff7_externals.menu_jafont_1_graphics_object, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(ff7_externals.menu_jafont_2_graphics_object, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(ff7_externals.menu_jafont_3_graphics_object, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(ff7_externals.menu_jafont_4_graphics_object, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(ff7_externals.menu_jafont_5_graphics_object, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(ff7_externals.menu_jafont_6_graphics_object, game_object);

    ff7_externals.engine_gfx_draw_predefined_polygon_set_field_84_sub_660E95(1, game_object);
    ff7_externals.engine_gfx_set_single_renderstate_sub_660C3A(2, 0, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_unknown4_graphics_object_DC1000, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_unknown5_graphics_object_DC1004, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_blend_window_bg_graphics_object_DC0FF4, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_unknown2_graphics_object_DC0FF8, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_a_blend_0_graphics_object_DC0FDC, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_a_blend_1_graphics_object_DC0FE0, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_c_blend_1_graphics_object_DC0FE8, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_b_blend_1_graphics_object_DC0FE4, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_d_blend_1_graphics_object_DC0FEC, game_object);
  }
  ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_text_box_quad_graphics_object_DC1008, game_object);
  ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_unknown4_graphics_object_DC1000);
  ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_unknown5_graphics_object_DC1004);
  ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_blend_window_bg_graphics_object_DC0FF4);
  ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_unknown2_graphics_object_DC0FF8);
  ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_text_box_quad_graphics_object_DC1008);
}

void battle_draw_graphics_object_and_jafonts(ff7_graphics_object* graphics_object, ff7_game_obj* game_object)
{
  ff7_externals.engine_draw_graphics_object_66E641(graphics_object, game_object);

  ff7_graphics_object* jafont_objects[] = {
    ff7_externals.menu_jafont_1_graphics_object,
    ff7_externals.menu_jafont_2_graphics_object,
    ff7_externals.menu_jafont_3_graphics_object,
    ff7_externals.menu_jafont_4_graphics_object,
    ff7_externals.menu_jafont_5_graphics_object,
    ff7_externals.menu_jafont_6_graphics_object,
  };

  for (ff7_graphics_object* graphics_object : jafont_objects)
  {
    ff7_externals.engine_draw_graphics_object_66E641(graphics_object, game_object);
  }
}

void battle_reset_graphics_object_and_jafonts(ff7_graphics_object* graphics_object)
{
  ff7_externals.reset_field_54_graphics_object_66E62C(graphics_object);

  ff7_graphics_object* jafont_objects[] = {
    ff7_externals.menu_jafont_1_graphics_object,
    ff7_externals.menu_jafont_2_graphics_object,
    ff7_externals.menu_jafont_3_graphics_object,
    ff7_externals.menu_jafont_4_graphics_object,
    ff7_externals.menu_jafont_5_graphics_object,
    ff7_externals.menu_jafont_6_graphics_object,
  };

  for (ff7_graphics_object* jafont_graphics_object : jafont_objects)
  {
    ff7_externals.reset_field_54_graphics_object_66E62C(jafont_graphics_object);
  }
}

void draw_text_top_display_6D1CC0_jp(int a1, int16_t menu_box_idx, char a3, uint16_t a4) // used printing centered texts.
{
  // probably should be scaled up, but until the other one is fixed, not bothering.
  double scaleFactor = 1.0f; // default scale factor. only one ever used for field texts. use 1.0 for normal small text behavior
  // no x position fudging for battle text.
  float yPosFudge = 4;       // smaller text is lower.

  __int64 v4;
  __int64 menu_width;
  graphics_vertex *v6;
  graphics_vertex *v7;
  graphics_vertex *v8;
  graphics_vertex *v9;
  graphics_vertex *v10;
  graphics_vertex *v11;
  graphics_vertex *v12;
  graphics_vertex *v13;
  graphics_vertex *v14;
  graphics_vertex *v15;
  graphics_vertex *v16;
  graphics_vertex *v17;
  graphics_vertex *v18;
  graphics_vertex *v19;
  graphics_vertex *v20;
  graphics_vertex *v21;
  graphics_vertex *v22;
  graphics_vertex *v23;
  graphics_vertex *v24;
  graphics_vertex *v25;
  graphics_vertex *v26;
  graphics_vertex *v27;
  graphics_vertex *v28;
  graphics_vertex *v29;
  graphics_vertex *v30;
  graphics_vertex *v31;
  graphics_vertex *v32;
  graphics_vertex *v33;
  graphics_vertex *v34;
  graphics_vertex *v35;
  graphics_vertex *v36;
  graphics_vertex *v37;
  graphics_vertex *v38;
  graphics_vertex *v39;
  graphics_vertex *v40;
  graphics_vertex *v41;
  graphics_vertex *v42;
  graphics_vertex *v43;
  graphics_vertex *v44;
  graphics_vertex *v45;
  graphics_vertex *v46;
  graphics_vertex *v47;
  graphics_vertex *v48;
  graphics_vertex *v49;
  graphics_vertex *v50;
  graphics_vertex *v51;
  graphics_vertex *v52;
  graphics_vertex *v53;
  graphics_vertex *v54;
  graphics_vertex *v55;
  graphics_vertex *v56;
  graphics_vertex *v57;
  graphics_vertex *v58;
  graphics_vertex *v59;
  graphics_vertex *v60;
  graphics_vertex *v61;
  graphics_vertex *v62;
  graphics_vertex *v63;
  graphics_vertex *v64;
  graphics_vertex *v65;
  graphics_vertex *v66;
  graphics_vertex *v67;
  graphics_vertex *v68;
  graphics_vertex *v69;
  graphics_vertex *v70;
  graphics_vertex *v71;
  graphics_vertex *v72;
  graphics_vertex *v73;
  graphics_vertex *v74;
  graphics_vertex *v75;
  graphics_vertex *v76;
  graphics_vertex *v77;
  graphics_vertex *v78;
  graphics_vertex *v79;
  graphics_vertex *v80;
  graphics_vertex *v81;
  graphics_vertex *v82;
  graphics_vertex *v83;
  graphics_vertex *v84;
  graphics_vertex *v85;
  graphics_vertex *v86;
  graphics_vertex *v87;
  graphics_vertex *v88;
  graphics_vertex *vertex_transform;
  int v90;
  graphics_vertex *v91;
  graphics_vertex *v92;
  graphics_vertex *v93;
  graphics_vertex *v94;
  int v95;
  int16_t v96;
  int16_t v97;
  float v98;
  float v99;
  float v100;
  int offset_y;
  float v102;
  float v103;
  int offset_x;
  int16_t v105;
  int16_t v106;
  int16_t v107;
  int16_t v108;
  int16_t v109;
  int16_t v110;
  int16_t v111;
  int16_t menu_height;
  int16_t v113;
  int16_t v114;
  int16_t v115;
  int16_t v116;
  int16_t v117;
  char v118;
  int16_t j;
  int16_t v120;
  int16_t i;
  int16_t v122;
  attack_name_fixed_buffer *v123;
  attack_name_fixed_buffer *v124;
  ff7_graphics_object *a2 = nullptr;
  int16_t v126;
  int16_t v127;
  int v128;
  int16_t v129;
  int16_t v130;
  int v131;
  int16_t v132;
  int16_t v133;
  attack_name_fixed_buffer *text_sub_41963C;
  attack_name_fixed_buffer *v135;
  attack_name_fixed_buffer *v136;
  float v137;

  v131 = *ff7_externals.menu_viewport_x_DC105C;
  v128 = *ff7_externals.menu_viewport_y_DC1060;
  v118 = 0;
  offset_x = (*ff7_externals.battle_menu_data_DC3630)[menu_box_idx].offset_x;
  offset_y = (*ff7_externals.battle_menu_data_DC3630)[menu_box_idx].offset_y;
  switch ( menu_box_idx )
  {
    case 22:
      menu_box_idx = 25;
      offset_x = (*ff7_externals.battle_menu_data_DC3630)[25].offset_x;
      offset_y = (*ff7_externals.battle_menu_data_DC3630)[25].offset_y;
      switch ( a3 )
      {
        case 0:
        case 2:
          text_sub_41963C = ff7_externals.kernel_get_text_sub_41963C(0, a4, 8);
          break;
        case 3:
          text_sub_41963C = ff7_externals.kernel_get_text_sub_41963C(6u, a4, 8);
          break;
        case 4:
        case 8:
          text_sub_41963C = ff7_externals.kernel_get_text_sub_41963C(4u, a4, 8);
          break;
        case 7:
          ff7_externals.sub_6D70F1(a4);
          text_sub_41963C = (attack_name_fixed_buffer *)ff7_externals.byte_DC3640;
          break;
        case 13:
          text_sub_41963C = ff7_externals.kernel_get_text_sub_41963C(2u, a4, 8);
          break;
        case 20:
          text_sub_41963C = ff7_externals.kernel_get_text_sub_41963C(3u, a4, 8);
          break;
        case 32:
          text_sub_41963C = ff7_externals.kernel_get_text_sub_41963C(9u, a4, 8);
          break;
        default:
          text_sub_41963C = ff7_externals.kernel_get_text_sub_41963C(0, 0, 8);
          break;
      }
      break;
    case 23:
      text_sub_41963C = (attack_name_fixed_buffer *)*ff7_externals.battle_text_buffer_DC208C;
      break;
    case 25:
      text_sub_41963C = (attack_name_fixed_buffer *)*ff7_externals.battle_text_buffer_DC208C;
      break;
  }
  if ( *ff7_externals.menu_is_small_viewport_320_240_DC130C == 1 )
  {
    // 320x240 viewport not needed
  }
  else
  {
    v95 = 0;
    v123 = text_sub_41963C;
    v106 = 0;
    for ( j = 0; j < 256 && text_sub_41963C->name[0] != 255; ++j ) // this code set the start point for centered text.
    {
      int charWidth = 16;
      int leftPadding = 0;
      bool isKanjiDetected = false;
      switch ( text_sub_41963C->name[0] )
      {
        case 0xFAu:
          text_sub_41963C = (attack_name_fixed_buffer *)((char *)text_sub_41963C + 1);
          charWidth = charWidthData[1][*(byte*)(text_sub_41963C)] & 0x1F;
          leftPadding = charWidthData[1][*(byte*)(text_sub_41963C)] >> 5;
          v106 += jp_center_advance((uint16_t)(0xFA00 | *(byte*)(text_sub_41963C)), leftPadding, charWidth);
          isKanjiDetected = true;
          ++v95;
          break;
        case 0xFBu:
          text_sub_41963C = (attack_name_fixed_buffer *)((char *)text_sub_41963C + 1);
          charWidth = charWidthData[2][*(byte*)(text_sub_41963C)] & 0x1F;
          leftPadding = charWidthData[2][*(byte*)(text_sub_41963C)] >> 5;
          v106 += jp_center_advance((uint16_t)(0xFB00 | *(byte*)(text_sub_41963C)), leftPadding, charWidth);
          isKanjiDetected = true;
          ++v95;
          break;
        case 0xFCu:
          text_sub_41963C = (attack_name_fixed_buffer *)((char *)text_sub_41963C + 1);
          charWidth = charWidthData[3][*(byte*)(text_sub_41963C)] & 0x1F;
          leftPadding = charWidthData[3][*(byte*)(text_sub_41963C)] >> 5;
          v106 += jp_center_advance((uint16_t)(0xFC00 | *(byte*)(text_sub_41963C)), leftPadding, charWidth);
          isKanjiDetected = true;
          ++v95;
          break;
        case 0xFDu:
          text_sub_41963C = (attack_name_fixed_buffer *)((char *)text_sub_41963C + 1);
          charWidth = charWidthData[4][*(byte*)(text_sub_41963C)] & 0x1F;
          leftPadding = charWidthData[4][*(byte*)(text_sub_41963C)] >> 5;
          v106 += jp_center_advance((uint16_t)(0xFD00 | *(byte*)(text_sub_41963C)), leftPadding, charWidth);
          isKanjiDetected = true;
          ++v95;
          break;
        case 0xFEu:
          text_sub_41963C = (attack_name_fixed_buffer *)((char *)text_sub_41963C + 1);
          charWidth = charWidthData[5][*(byte*)(text_sub_41963C)] & 0x1F;
          leftPadding = charWidthData[5][*(byte*)(text_sub_41963C)] >> 5;
          v106 += jp_center_advance((uint16_t)(0xFE00 | *(byte*)(text_sub_41963C)), leftPadding, charWidth);
          isKanjiDetected = true;
          ++v95;
          break;
        case 0xF8u:
          v118 = 1;
          text_sub_41963C = (attack_name_fixed_buffer *)((char *)text_sub_41963C + 1);
          break;
        default:
          if(!isKanjiDetected)
          {
            charWidth = charWidthData[0][*(byte*)(text_sub_41963C)] & 0x1F;
            leftPadding = charWidthData[0][*(byte*)(text_sub_41963C)] >> 5;
            v106 += jp_center_advance((uint16_t)*(byte*)(text_sub_41963C), leftPadding, charWidth);
          }
          isKanjiDetected = false;
          ++v95;
          break;
      }
      text_sub_41963C = (attack_name_fixed_buffer *)((char *)text_sub_41963C + 1);
    }
    v135 = v123;
    v106 = (short)(((float)v106 * scaleFactor));  // recenter based on JP text scale factor.
    v4 = (*ff7_externals.battle_menu_data_DC3630)[menu_box_idx].menu_width;
    v107 = (((int)v4 - HIWORD(v4)) >> 1) - v106 / 2; // starting point for text set.
    v120 = 0;
    bool isKanjiDetected = false;                    // reset for second pass
    int charWidth = 16;
    int leftPadding = 0;
    ff7_graphics_object* graphics_object = ff7_externals.menu_jafont_1_graphics_object;
    while ( v120 < 256 && v135->name[0] != 255 )
    {
      switch ( v135->name[0] )
      {
        case 0xF8u:
          v135 = (attack_name_fixed_buffer *)((char *)v135 + 2);
          goto LABEL_31;
        case 0xFAu:
          v135 = (attack_name_fixed_buffer *)((char *)v135 + 1);
          graphics_object = ff7_externals.menu_jafont_2_graphics_object;
          charWidth = charWidthData[1][v135->name[0]] & 0x1F;
          leftPadding = charWidthData[1][v135->name[0]] >> 5;
          isKanjiDetected = true;
          continue;
        case 0xFBu:
          v135 = (attack_name_fixed_buffer *)((char *)v135 + 1);
          graphics_object = ff7_externals.menu_jafont_3_graphics_object;
          charWidth = charWidthData[2][v135->name[0]] & 0x1F;
          leftPadding = charWidthData[2][v135->name[0]] >> 5;
          isKanjiDetected = true;
          continue;
        case 0xFCu:
          v135 = (attack_name_fixed_buffer *)((char *)v135 + 1);
          graphics_object = ff7_externals.menu_jafont_4_graphics_object;
          charWidth = charWidthData[3][v135->name[0]] & 0x1F;
          leftPadding = charWidthData[3][v135->name[0]] >> 5;
          isKanjiDetected = true;
          continue;
        case 0xFDu:
          v135 = (attack_name_fixed_buffer *)((char *)v135 + 1);
          graphics_object = ff7_externals.menu_jafont_5_graphics_object;
          charWidth = charWidthData[4][v135->name[0]] & 0x1F;
          leftPadding = charWidthData[4][v135->name[0]] >> 5;
          isKanjiDetected = true;
          continue;
        case 0xFEu:
          v135 = (attack_name_fixed_buffer *)((char *)v135 + 1);
          graphics_object = ff7_externals.menu_jafont_6_graphics_object;
          charWidth = charWidthData[5][v135->name[0]] & 0x1F;
          leftPadding = charWidthData[5][v135->name[0]] >> 5;
          isKanjiDetected = true;
          continue;
        default:
          if (!isKanjiDetected)
          {
            graphics_object = ff7_externals.menu_jafont_1_graphics_object;
            charWidth = charWidthData[0][v135->name[0]] & 0x1F;
            leftPadding = charWidthData[0][v135->name[0]] >> 5;
          }
          isKanjiDetected = false;

          v105 = v135->name[0];
          v132 = 32 * (v105 % 16);
          v129 = 32 * (v105 / 16);
          if ( v132 <= 480 )
          {
            v127 = v132;
            if ( v132 == 480 )
            {
              v137 = 32;
              v126 = 16;
            }
            else
            {
              v137 = 32;
              v126 = 16;
            }
            a2 = graphics_object;
          }
          v108 = v107;   // change! was adding character width before printing character isntead of after.  this was incorrect. will add it at end of loop later
          v96 = leftPadding;   // padding from above.
LABEL_49:
          if (ff7_externals.g_get_do_render_menu_6CDBF2() && common_externals.draw_graphics_object(1, (struct graphics_object*)a2))
          {
            // let's go and print some text.
            auto color = get_character_color(7);
            color.a = 128;
            v102 = (double)v127 / 512.0;
            v99 = (double)v129 / 512.0;
            v98 = v137 / 512.0;
            v94 = a2->vertex_transform;
            v94->position.x = (double)offset_x + (double)v108; // add centering offset, adjusted for placed characters
            v94->position.y = (double)offset_y + (double)12+yPosFudge;
            v94->position.z = 0.0;
            v94->position.w = 1.0;
            v94->color = color;
            v94->alpha_mask = -16777216;
            v94->u = v102;
            v94->v = v99;
            v93 = a2->vertex_transform + 1;
            v93->position.x = (double)offset_x + (double)v108;
            v93->position.y = (double)offset_y + (double)12 + 16.0 * scaleFactor+yPosFudge; // not scaling up yet.
            v93->position.z = 0.0;
            v93->position.w = 1.0;
            v93->color = color;
            v93->alpha_mask = -16777216;
            v93->u = v102;
            v93->v = v99 + 32.0f / 512.0f;
            v92 = a2->vertex_transform + 2;
            v92->position.x = (double)offset_x + (double)v108 + (double)v126 * scaleFactor; // add base width to right corners
            v92->position.y = (double)offset_y + (double)12 + yPosFudge;
            v92->position.z = 0.0;
            v92->position.w = 1.0;
            v92->color = color;
            v92->alpha_mask = -16777216;
            v92->u = v102 + v98;
            v92->v = v99;
            v91 = a2->vertex_transform + 3;
            v91->position.x = (double)offset_x + (double)v108 + (double)v126 * scaleFactor;
            v91->position.y = (double)offset_y + (double)12 + 16.0*scaleFactor+yPosFudge;
            v91->position.z = 0.0;
            v91->position.w = 1.0;
            v91->color = color;
            v91->alpha_mask = -16777216;
            v91->u = v102 + v98;
            v91->v = v99 + 32.0f / 512.0f;
            *(byte *)a2->curr_total_n_shape = 14;
            a2->field_7C = 14;
          }
          v135 = (attack_name_fixed_buffer *)((char *)v135 + 1);
          v107 = v96 + v108+ std::ceil(z_half_width(charWidth)*scaleFactor); // character width+padding+previous centering offset.
LABEL_31:
          ++v120;
          break;
      }
    }
  }
  v115 = (*ff7_externals.battle_menu_data_DC3630)[menu_box_idx].menu_width;
  menu_height = (*ff7_externals.battle_menu_data_DC3630)[menu_box_idx].menu_height;
  if ( *ff7_externals.menu_is_small_viewport_320_240_DC130C == 1 )
  {
    // 320x240 viewport not needed
  }
  else
  {
    if ( ff7_externals.g_get_do_render_menu_6CDBF2()
      && !*ff7_externals.g_is_battle_paused_DC0E6C
      && common_externals.draw_graphics_object(8, (struct graphics_object*)(*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object) )
    {
      v85 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform;
      v85->position.x = (float)offset_x;
      v85->position.y = (float)offset_y;
      v85->position.z = 0.0101;
      v85->position.w = 1.0;
      v85->color = { 255, 255, 255, 128 };
      v85->alpha_mask = -16777216;
      v85->u = 0.0;
      v85->v = 0.8125;
      v84 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + 1;
      v84->position.x = (float)offset_x;
      v84->position.y = (double)offset_y + 8.0;
      v84->position.z = 0.0101;
      v84->position.w = 1.0;
      v84->color = { 255, 255, 255, 128 };
      v84->alpha_mask = -16777216;
      v84->u = 0.0;
      v84->v = 0.03125 + 0.8125;
      v83 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + 2;
      v83->position.x = (double)offset_x + 8.0;
      v83->position.y = (float)offset_y;
      v83->position.z = 0.0101;
      v83->position.w = 1.0;
      v83->color = { 255, 255, 255, 128 };
      v83->alpha_mask = -16777216;
      v83->u = 0.03125 + 0.0;
      v83->v = 0.8125;
      v82 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + 3;
      v82->position.x = (double)offset_x + 8.0;
      v82->position.y = (double)offset_y + 8.0;
      v82->position.z = 0.0101;
      v82->position.w = 1.0;
      v82->color = { 255, 255, 255, 128 };
      v82->alpha_mask = -16777216;
      v82->u = 0.03125 + 0.0;
      v82->v = 0.03125 + 0.8125;
      *(byte *)(*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->curr_total_n_shape = 0;
      (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->field_7C = 0;
      (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform = (graphics_vertex *)((char *)(*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_offset);
      ++(*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->curr_total_n_shape;
      v81 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform;
      v81->position.x = (double)offset_x + (double)v115 - 8.0;
      v81->position.y = (float)offset_y;
      v81->position.z = 0.0101;
      v81->position.w = 1.0;
      v81->color = { 255, 255, 255, 128 };
      v81->alpha_mask = -16777216;
      v81->u = 0.09375;
      v81->v = 0.8125;
      v80 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + 1;
      v80->position.x = (double)offset_x + (double)v115 - 8.0;
      v80->position.y = (double)offset_y + 8.0;
      v80->position.z = 0.0101;
      v80->position.w = 1.0;
      v80->color = { 255, 255, 255, 128 };
      v80->alpha_mask = -16777216;
      v80->u = 0.09375;
      v80->v = 0.03125 + 0.8125;
      v79 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + 2;
      v79->position.x = (double)offset_x + (double)v115 - 8.0 + 8.0;
      v79->position.y = (float)offset_y;
      v79->position.z = 0.0101;
      v79->position.w = 1.0;
      v79->color = { 255, 255, 255, 128 };
      v79->alpha_mask = -16777216;
      v79->u = 0.03125 + 0.09375;
      v79->v = 0.8125;
      v78 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + 3;
      v78->position.x = (double)offset_x + (double)v115 - 8.0 + 8.0;
      v78->position.y = (double)offset_y + 8.0;
      v78->position.z = 0.0101;
      v78->position.w = 1.0;
      v78->color = { 255, 255, 255, 128 };
      v78->alpha_mask = -16777216;
      v78->u = 0.03125 + 0.09375;
      v78->v = 0.03125 + 0.8125;
      *(byte *)(*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->curr_total_n_shape = 0;
      (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->field_7C = 0;
      (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform = (graphics_vertex *)((char *)(*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_offset);
      ++(*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->curr_total_n_shape;
      v77 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform;
      v77->position.x = (float)offset_x;
      v77->position.y = (double)offset_y + (double)menu_height - 8.0;
      v77->position.z = 0.0101;
      v77->position.w = 1.0;
      v77->color = { 255, 255, 255, 128 };
      v77->alpha_mask = -16777216;
      v77->u = 0.125;
      v77->v = 0.78125;
      v76 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + 1;
      v76->position.x = (float)offset_x;
      v76->position.y = (double)offset_y + (double)menu_height - 8.0 + 8.0;
      v76->position.z = 0.0101;
      v76->position.w = 1.0;
      v76->color = { 255, 255, 255, 128 };
      v76->alpha_mask = -16777216;
      v76->u = 0.125;
      v76->v = 0.03125 + 0.78125;
      v75 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + 2;
      v75->position.x = (double)offset_x + 8.0;
      v75->position.y = (double)offset_y + (double)menu_height - 8.0;
      v75->position.z = 0.0101;
      v75->position.w = 1.0;
      v75->color = { 255, 255, 255, 128 };
      v75->alpha_mask = -16777216;
      v75->u = 0.03125 + 0.125;
      v75->v = 0.78125;
      v74 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + 3;
      v74->position.x = (double)offset_x + 8.0;
      v74->position.y = (double)offset_y + (double)menu_height - 8.0 + 8.0;
      v74->position.z = 0.0101;
      v74->position.w = 1.0;
      v74->color = { 255, 255, 255, 128 };
      v74->alpha_mask = -16777216;
      v74->u = 0.03125 + 0.125;
      v74->v = 0.03125 + 0.78125;
      *(byte *)(*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->curr_total_n_shape = 0;
      (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->field_7C = 0;
      (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform = (graphics_vertex *)((char *)(*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_offset);
      ++(*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->curr_total_n_shape;
      v73 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform;
      v73->position.x = (double)offset_x + (double)v115 - 8.0;
      v73->position.y = (double)offset_y + (double)menu_height - 8.0;
      v73->position.z = 0.0101;
      v73->position.w = 1.0;
      v73->color = { 255, 255, 255, 128 };
      v73->alpha_mask = -16777216;
      v73->u = 0.21875;
      v73->v = 0.78125;
      v72 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + 1;
      v72->position.x = (double)offset_x + (double)v115 - 8.0;
      v72->position.y = (double)offset_y + (double)menu_height - 8.0 + 8.0;
      v72->position.z = 0.0101;
      v72->position.w = 1.0;
      v72->color = { 255, 255, 255, 128 };
      v72->alpha_mask = -16777216;
      v72->u = 0.21875;
      v72->v = 0.03125 + 0.78125;
      v71 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + 2;
      v71->position.x = (double)offset_x + (double)v115 - 8.0 + 8.0;
      v71->position.y = (double)offset_y + (double)menu_height - 8.0;
      v71->position.z = 0.0101;
      v71->position.w = 1.0;
      v71->color = { 255, 255, 255, 128 };
      v71->alpha_mask = -16777216;
      v71->u = 0.03125 + 0.21875;
      v71->v = 0.78125;
      v70 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + 3;
      v70->position.x = (double)offset_x + (double)v115 - 8.0 + 8.0;
      v70->position.y = (double)offset_y + (double)menu_height - 8.0 + 8.0;
      v70->position.z = 0.0101;
      v70->position.w = 1.0;
      v70->color = { 255, 255, 255, 128 };
      v70->alpha_mask = -16777216;
      v70->u = 0.03125 + 0.21875;
      v70->v = 0.03125 + 0.78125;
      *(byte *)(*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->curr_total_n_shape = 0;
      (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->field_7C = 0;
      (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform = (graphics_vertex *)((char *)(*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_offset);
      ++(*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->curr_total_n_shape;
      v69 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform;
      v69->position.x = (double)offset_x + 8.0;
      v69->position.y = (float)offset_y;
      v69->position.z = 0.0101;
      v69->position.w = 1.0;
      v69->color = { 255, 255, 255, 128 };
      v69->alpha_mask = -16777216;
      v69->u = 0.0;
      v69->v = 0.75;
      v68 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + 1;
      v68->position.x = (double)offset_x + 8.0;
      v68->position.y = (double)offset_y + 8.0;
      v68->position.z = 0.0101;
      v68->position.w = 1.0;
      v68->color = { 255, 255, 255, 128 };
      v68->alpha_mask = -16777216;
      v68->u = 0.0;
      v68->v = 0.75 + 0.03125;
      v67 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + 2;
      v67->position.x = (double)offset_x + 8.0 + (double)v115 - 16.0;
      v67->position.y = (float)offset_y;
      v67->position.z = 0.0101;
      v67->position.w = 1.0;
      v67->color = { 255, 255, 255, 128 };
      v67->alpha_mask = -16777216;
      v67->u = 0.0 + 0.0625;
      v67->v = 0.75;
      v66 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + 3;
      v66->position.x = (double)offset_x + 8.0 + (double)v115 - 16.0;
      v66->position.y = (double)offset_y + 8.0;
      v66->position.z = 0.0101;
      v66->position.w = 1.0;
      v66->color = { 255, 255, 255, 128 };
      v66->alpha_mask = -16777216;
      v66->u = 0.0 + 0.0625;
      v66->v = 0.75 + 0.03125;
      *(byte *)(*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->curr_total_n_shape = 0;
      (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->field_7C = 0;
      (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform = (graphics_vertex *)((char *)(*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_offset);
      ++(*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->curr_total_n_shape;
      v65 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform;
      v65->position.x = (double)offset_x + 8.0;
      v65->position.y = (double)offset_y + (double)menu_height - 8.0;
      v65->position.z = 0.0101;
      v65->position.w = 1.0;
      v65->color = { 255, 255, 255, 128 };
      v65->alpha_mask = -16777216;
      v65->u = 0.125;
      v65->v = 0.84375;
      v64 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + 1;
      v64->position.x = (double)offset_x + 8.0;
      v64->position.y = (double)offset_y + (double)menu_height - 8.0 + 8.0;
      v64->position.z = 0.0101;
      v64->position.w = 1.0;
      v64->color = { 255, 255, 255, 128 };
      v64->alpha_mask = -16777216;
      v64->u = 0.125;
      v64->v = 0.03125 + 0.84375;
      v63 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + 2;
      v63->position.x = (double)offset_x + 8.0 + (double)v115 - 16.0;
      v63->position.y = (double)offset_y + (double)menu_height - 8.0;
      v63->position.z = 0.0101;
      v63->position.w = 1.0;
      v63->color = { 255, 255, 255, 128 };
      v63->alpha_mask = -16777216;
      v63->u = 0.0625 + 0.125;
      v63->v = 0.84375;
      v62 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + 3;
      v62->position.x = (double)offset_x + 8.0 + (double)v115 - 16.0;
      v62->position.y = (double)offset_y + (double)menu_height - 8.0 + 8.0;
      v62->position.z = 0.0101;
      v62->position.w = 1.0;
      v62->color = { 255, 255, 255, 128 };
      v62->alpha_mask = -16777216;
      v62->u = 0.0625 + 0.125;
      v62->v = 0.03125 + 0.84375;
      *(byte *)(*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->curr_total_n_shape = 0;
      (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->field_7C = 0;
      (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform = (graphics_vertex *)((char *)(*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_offset);
      ++(*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->curr_total_n_shape;
      v61 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform;
      v61->position.x = (double)offset_x + (double)v115 - 8.0;
      v61->position.y = (double)offset_y + 8.0;
      v61->position.z = 0.0101;
      v61->position.w = 1.0;
      v61->color = { 255, 255, 255, 128 };
      v61->alpha_mask = -16777216;
      v61->u = 0.21875;
      v61->v = 0.875;
      v60 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + 1;
      v60->position.x = (double)offset_x + (double)v115 - 8.0;
      v60->position.y = (double)offset_y + 8.0 + (double)menu_height - 16.0;
      v60->position.z = 0.0101;
      v60->position.w = 1.0;
      v60->color = { 255, 255, 255, 128 };
      v60->alpha_mask = -16777216;
      v60->u = 0.21875;
      v60->v = 0.875 + 0.0625;
      v59 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + 2;
      v59->position.x = (double)offset_x + (double)v115 - 8.0 + 8.0;
      v59->position.y = (double)offset_y + 8.0;
      v59->position.z = 0.0101;
      v59->position.w = 1.0;
      v59->color = { 255, 255, 255, 128 };
      v59->alpha_mask = -16777216;
      v59->u = 0.21875 + 0.03125;
      v59->v = 0.875;
      v58 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + 3;
      v58->position.x = (double)offset_x + (double)v115 - 8.0 + 8.0;
      v58->position.y = (double)offset_y + 8.0 + (double)menu_height - 16.0;
      v58->position.z = 0.0101;
      v58->position.w = 1.0;
      v58->color = { 255, 255, 255, 128 };
      v58->alpha_mask = -16777216;
      v58->u = 0.21875 + 0.03125;
      v58->v = 0.875 + 0.0625;
      *(byte *)(*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->curr_total_n_shape = 0;
      (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->field_7C = 0;
      (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform = (graphics_vertex *)((char *)(*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_offset);
      ++(*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->curr_total_n_shape;
      v57 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform;
      v57->position.x = (float)offset_x;
      v57->position.y = (double)offset_y + 8.0;
      v57->position.z = 0.0101;
      v57->position.w = 1.0;
      v57->color = { 255, 255, 255, 128 };
      v57->alpha_mask = -16777216;
      v57->u = 0.0;
      v57->v = 0.875;
      v56 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + 1;
      v56->position.x = (float)offset_x;
      v56->position.y = (double)offset_y + 8.0 + (double)menu_height - 16.0;
      v56->position.z = 0.0101;
      v56->position.w = 1.0;
      v56->color = { 255, 255, 255, 128 };
      v56->alpha_mask = -16777216;
      v56->u = 0.0;
      v56->v = 0.0625 + 0.875;
      v55 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + 2;
      v55->position.x = (double)offset_x + 8.0;
      v55->position.y = (double)offset_y + 8.0;
      v55->position.z = 0.0101;
      v55->position.w = 1.0;
      v55->color = { 255, 255, 255, 128 };
      v55->alpha_mask = -16777216;
      v55->u = 0.03125 + 0.0;
      v55->v = 0.875;
      v54 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + 3;
      v54->position.x = (double)offset_x + 8.0;
      v54->position.y = (double)offset_y + 8.0 + (double)menu_height - 16.0;
      v54->position.z = 0.0101;
      v54->position.w = 1.0;
      v54->color = { 255, 255, 255, 128 };
      v54->alpha_mask = -16777216;
      v54->u = 0.03125 + 0.0;
      v54->v = 0.0625 + 0.875;
      *(byte *)(*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->curr_total_n_shape = 0;
      (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->field_7C = 0;
    }
    v116 = (*ff7_externals.battle_menu_data_DC3630)[menu_box_idx].menu_width - 12;
    v113 = (*ff7_externals.battle_menu_data_DC3630)[menu_box_idx].menu_height - 12;
    if ( v118
      && ff7_externals.g_get_do_render_menu_6CDBF2()
      && !*ff7_externals.g_is_battle_paused_DC0E6C
      && common_externals.draw_graphics_object(1, (struct graphics_object*)(*ff7_externals.battle_graphics_data_ptr_9ADFD8)->graphics_data_other_array[0]) )
    {
      v53 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->graphics_data_other_array[0]->vertex_transform;
      v53->position.x = (double)offset_x + 6.0;
      v53->position.y = (double)offset_y + 6.0;
      v53->position.z = 0.0101;
      v53->position.w = 1.0;
      v53->color = { 0, 0, 112, 144 };
      v53->alpha_mask = -16777216;
      v52 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->graphics_data_other_array[0]->vertex_transform + 1;
      v52->position.x = (double)offset_x + 6.0;
      v52->position.y = (double)offset_y + 6.0 + (double)(*ff7_externals.battle_menu_data_DC3630)[menu_box_idx].menu_height - 12.0;
      v52->position.z = 0.0101;
      v52->position.w = 1.0;
      v52->color = { 0, 0, 112, 144 };
      v52->alpha_mask = -16777216;
      v51 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->graphics_data_other_array[0]->vertex_transform + 2;
      v51->position.x = (double)offset_x + 6.0 + (double)(*ff7_externals.battle_menu_data_DC3630)[menu_box_idx].menu_width - 12.0;
      v51->position.y = (double)offset_y + 6.0;
      v51->position.z = 0.0101;
      v51->position.w = 1.0;
      v51->color = { 0, 0, 112, 144 };
      v51->alpha_mask = -16777216;
      v50 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->graphics_data_other_array[0]->vertex_transform + 3;
      v50->position.x = (double)offset_x + 6.0 + (double)(*ff7_externals.battle_menu_data_DC3630)[menu_box_idx].menu_width - 12.0;
      v50->position.y = (double)offset_y + 6.0 + (double)(*ff7_externals.battle_menu_data_DC3630)[menu_box_idx].menu_height - 12.0;
      v50->position.z = 0.0101;
      v50->position.w = 1.0;
      v50->color = { 0, 0, 112, 144 };
      v50->alpha_mask = -16777216;
    }
    if ( ff7_externals.g_get_do_render_menu_6CDBF2()
      && !*ff7_externals.g_is_battle_paused_DC0E6C
      && common_externals.draw_graphics_object(1, (struct graphics_object*)(*ff7_externals.battle_graphics_data_ptr_9ADFD8)->box_color_graphics_object) )
    {
      v49 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->box_color_graphics_object->vertex_transform;
      v49->position.x = (double)offset_x + 6.0;
      v49->position.y = (double)offset_y + 6.0;
      v49->position.z = 0.0101;
      v49->position.w = 1.0;
      v49->color = *ff7_externals.dword_91EFC8;
      v49->alpha_mask = -16777216;
      v48 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->box_color_graphics_object->vertex_transform + 1;
      v48->position.x = (double)offset_x + 6.0;
      v48->position.y = (double)offset_y + (double)v113 + 6.0;
      v48->position.z = 0.0101;
      v48->position.w = 1.0;
      v48->color = *ff7_externals.dword_91EFCC;
      v48->alpha_mask = -16777216;
      v47 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->box_color_graphics_object->vertex_transform + 2;
      v47->position.x = (double)offset_x + (double)v116 + 6.0;
      v47->position.y = (double)offset_y + 6.0;
      v47->position.z = 0.0101;
      v47->position.w = 1.0;
      v47->color = *ff7_externals.dword_91EFD0;
      v47->alpha_mask = -16777216;
      v46 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->box_color_graphics_object->vertex_transform + 3;
      v46->position.x = (double)offset_x + (double)v116 + 6.0;
      v46->position.y = (double)offset_y + (double)v113 + 6.0;
      v46->position.z = 0.0101;
      v46->position.w = 1.0;
      v46->color = *ff7_externals.dword_91EFD4;
      v46->alpha_mask = -16777216;
    }
  }
}

void main_menu_draw_everything_maybe_6C0B91_jp()
{
  ff7_game_obj *game_object;

  game_object = ff7_externals.engine_get_game_object_676578();
  ff7_externals.engine_gfx_draw_predefined_polygon_set_field_84_sub_660E95(0, game_object);
  ff7_externals.engine_gfx_set_single_renderstate_sub_660C3A(2, 0, game_object);
  ff7_externals.engine_draw_graphics_object_66E641(*(ff7_graphics_object**)ff7_externals.menu_objects, game_object);
  ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_window_bg_graphics_object_DC0FF0, game_object);
  if ( *ff7_externals.menu_is_small_viewport_320_240_DC130C == 1 )
  {
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_blend_4_graphics_object_DC104C, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_c_blend_4_diff_graphics_object_DC0FD8, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_font_blend_4_graphics_object_DC1048, game_object);
  }
  else
  {
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_a_blend_4_graphics_object_DC0FC8, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_c_blend_4_graphics_object_DC0FD0, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_c_blend_4_diff_graphics_object_DC0FD8, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_b_blend_4_graphics_object_DC0FCC, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_d_blend_4_graphics_object_DC0FD4, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_font_a_graphics_object_DC100C, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_font_b_graphics_object_DC1010, game_object);

    // jp
    ff7_externals.engine_draw_graphics_object_66E641(ff7_externals.menu_jafont_1_graphics_object, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(ff7_externals.menu_jafont_2_graphics_object, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(ff7_externals.menu_jafont_3_graphics_object, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(ff7_externals.menu_jafont_4_graphics_object, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(ff7_externals.menu_jafont_5_graphics_object, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(ff7_externals.menu_jafont_6_graphics_object, game_object);
  }
  ff7_externals.engine_gfx_draw_predefined_polygon_set_field_84_sub_660E95(1, game_object);
  ff7_externals.engine_gfx_set_single_renderstate_sub_660C3A(2, 0, game_object);
  if ( *ff7_externals.menu_is_small_viewport_320_240_DC130C == 1 )
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_blend_0_graphics_object_DC1050, game_object);
  else
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_a_blend_0_graphics_object_DC0FDC, game_object);
  ff7_externals.engine_gfx_setviewport_sub_66067A(*ff7_externals.menu_viewport_x_DC105C, *ff7_externals.menu_viewport_y_DC1060, *ff7_externals.menu_viewport_width_DC1064, *ff7_externals.menu_viewport_view_DC1068, game_object);
}

void auto_resize_text_box(int16_t WINDOW_ID, int16_t* pOutW, int16_t* pOutH)
{
  // as many textboxes in flevel are set wrong, we need to resize them.
  float scaleFactor = ff7_japanese_edition ? 1.25f : 1.0f; // resizer needs to match the draw scale (JP 1.25 / multibyte EN 1.0)
	int16_t W = 0;
	int16_t H = 0;
	int16_t maxW = 0; // used to remember the longest row so far.
	int16_t maxH = 0;
  // first store what the flevel says it is, in case we need to give up
  *pOutW = ff7_externals.text_box_window_data_array_CFF5B8[WINDOW_ID].window_width;
  *pOutH = ff7_externals.text_box_window_data_array_CFF5B8[WINDOW_ID].window_height;

  byte* buffer_text = (byte*)ff7_externals.current_dialog_string_pointer[WINDOW_ID];
  bool isKanjiDetected = false;
  bool possibleOpcode = true; // some opcodes mmust be parsed, so we must look for them
  bool useFixedSpacing = false;
  int charWidth = 0;
  int leftPadding = 0;
  uint16_t letter = 0;
	for ( int i = 0;	i < 1024; ++i )
	{
    byte character = buffer_text[i];
    byte next_character = buffer_text[i + 1];

    if(character == 0xFF) break;

    switch ( character )
    {
      case 0xFAu:
        charWidth = charWidthData[1][next_character] & 0x1F;
        leftPadding = charWidthData[1][next_character] >> 5;
        letter = (uint16_t)(0xFA00 | next_character);
        isKanjiDetected = true;
        possibleOpcode = false; // not an opcode for sure
        continue;
      case 0xFBu:

        charWidth = charWidthData[2][next_character] & 0x1F;
        leftPadding = charWidthData[2][next_character] >> 5;
        letter = (uint16_t)(0xFB00 | next_character);
        isKanjiDetected = true;
        possibleOpcode = false;
        continue;
      case 0xFCu:
        charWidth = charWidthData[3][next_character] & 0x1F;
        leftPadding = charWidthData[3][next_character] >> 5;
        letter = (uint16_t)(0xFC00 | next_character);
        isKanjiDetected = true;
        possibleOpcode = false;
        continue;
      case 0xFDu:
        if (ff7_japanese_edition && next_character >= 0xF0)
        {
          W += jp_measure_field_fd_control(next_character, useFixedSpacing);
          ++i;
          possibleOpcode = true;
          isKanjiDetected = false;
          continue;
        }
        charWidth = charWidthData[4][next_character] & 0x1F;
        leftPadding = charWidthData[4][next_character] >> 5;
        letter = (uint16_t)(0xFD00 | next_character);
        isKanjiDetected = true;
        possibleOpcode = false;
        continue;
      case 0xFEu:
        if (next_character < 0xD2u)
        {
          charWidth = charWidthData[5][next_character] & 0x1F;
          leftPadding = charWidthData[5][next_character] >> 5;
          letter = (uint16_t)(0xFE00 | next_character);
          isKanjiDetected = true;
          possibleOpcode = false; // not an opcode
          continue;
        }
        // fall through
      default:
        if(!isKanjiDetected)
        {
          charWidth = charWidthData[0][character] & 0x1F;
          leftPadding = charWidthData[0][character] >> 5;
          letter = character;
          possibleOpcode = true; // again, this shouldn't be required, but can't hurt.
        }
        isKanjiDetected = false;
        break;
    }

    // character names need to be counted to resize proprely.
    if(possibleOpcode && character >= 0xEA && character <= 0xF5)
    {
      auto name_buffer = ff7_externals.sub_6CB9B8(character - 0xEA);
      for (int j = 0; j < 9; ++j)
      {
        auto name_char = name_buffer[j];

        if (name_char == 0xFF) break;

        int name_page = 0;
        uint16_t name_letter = name_char;
        if (name_char >= 0xFA && name_char <= 0xFE && j + 1 < 9 && name_buffer[j + 1] != 0xFF)
        {
          name_page = name_char - 0xF9;
          name_char = name_buffer[++j];
          name_letter = (uint16_t)(((0xF9 + name_page) << 8) | name_char);
        }

        charWidth = charWidthData[name_page][name_char] & 0x1F;
        leftPadding = charWidthData[name_page][name_char] >> 5;
        W += useFixedSpacing
          ? 10
          : ff7_japanese_edition
          ? jp_center_advance(name_letter, leftPadding, charWidth)
          : leftPadding + std::ceil(z_half_width(charWidth));
      }

      continue; // back to the start, we already added to the length
    }
    if (ff7_japanese_edition && possibleOpcode
        && (character == 0xF6u || character == 0xF7u
          || character == 0xF8u || character == 0xF9u))
    {
      W += jp_prompt_size / 2;
      if (character == 0xF6u && next_character >= 0x33u && next_character <= 0x3Du)
        ++i;
      continue;
    }
    // if its' an opcode, then we need to account for variables
    if (possibleOpcode && (character == 0XFEu))
    {
      switch (next_character)
      {
        case 0xE9u: // monospace toggle
          useFixedSpacing = !useFixedSpacing;
          i = i + 1;
          continue;
        case 0xDEu: // these are variable length
        case 0xE1u: // FIXME: actually parse them and account for string length
          W += 50;
          i = i + 1;
          continue;
        case 0xE2u: // fixed length string. this, i can parse well enough.
          W += 60;
          i = i + 5; // skip the opcode bytes for next go around 5 out of six, with the last one done at start of loop
          continue;
        default:
          // The driver consumes every FE D2..FF pair as one extended control code.
          // In particular, FE E7 must not leave E7 to be parsed as a bare newline.
          i = i + 1;
          continue;
      }
    }
    // more special character handling
    if(possibleOpcode && character == 0xE7) // next line
		{
      maxW = std::max(maxW, W); // update max
      W = 0;
			H += multibyte_field_linestep_q / 4;
      continue;
		}
    if(possibleOpcode && character == 0xE8) // next window
		{
			maxW = std::max(maxW, W); // update maxes
			maxH = std::max(maxH, H);
			W = 0;
			H = 0;
      continue;
		}

    if (ff7_japanese_edition)
      W += useFixedSpacing
        ? 10
        : jp_center_advance(letter, leftPadding, charWidth);
    else
      W += leftPadding + std::ceil(z_half_width(charWidth));
	}
  float pOutWtmp = ff7_japanese_edition
    ? (float)(std::max(maxW, W) + 25)
    : (std::max(maxW, W) + 40) * scaleFactor;
  *pOutW = ff7_japanese_edition ? (int)pOutWtmp : (int)(pOutWtmp / 2);
	*pOutH = (std::max(maxH, H) + 50) / 2;
}

void field_text_box_window_opening_6317A9_jp(short WINDOW_ID)
{
  // The vanilla create routine (0x631586) assigns this window's owner (CC0960[win] = the entity
  // that opened it) before marking it active, and clears both owner and mode together on close.
  // On the multibyte path a window can end up active with no owner assigned (0xFF) — the owner
  // check below then never matches the current entity, the window never grows, and the field
  // script that's waiting on it deadlocks. Restore the normal owner assignment for any window
  // found in this orphaned state before continuing.
  if ( ff7_externals.field_text_box_window_entity_id_CC0960[WINDOW_ID] == 0xFF )
    ff7_externals.field_text_box_window_entity_id_CC0960[WINDOW_ID] = *ff7_externals.current_entity_id_byte_CC0964;

  // auto_resize_text_box recomputes the window's target width/height from the text every time
  // it's called, including reading back the values it wrote the previous call — so calling it
  // every frame makes the target keep moving and the window's grow animation never reaches it,
  // stalling the open. Instead, compute the target once here while the window is still small,
  // then hold width/height fixed so the animation converges normally, matching vanilla behavior.
  // Field files already ship with correctly sized windows, so this only fixes the animation target.
  if ( ff7_japanese_edition || (ff7_externals.text_box_window_data_array_CFF5B8[WINDOW_ID].current_window_width < 8) ) // must run every frame as before to properly handle japanese edition.
  {
    if (ff7_externals.text_box_window_data_array_CFF5B8[WINDOW_ID].window_pos_x < 0)
      ff7_externals.text_box_window_data_array_CFF5B8[WINDOW_ID].window_pos_x = 0;              // if off the left, move it back on. :)

    if (ff7_field_autosize_text_box)
    {
      int16_t W = 0, H = 0;
      auto_resize_text_box(WINDOW_ID, &W, &H);
      ff7_externals.text_box_window_data_array_CFF5B8[WINDOW_ID].window_width = W;
      ff7_externals.text_box_window_data_array_CFF5B8[WINDOW_ID].window_height = H;
    }
  }

  if ( ff7_externals.field_text_box_window_entity_id_CC0960[WINDOW_ID] == *ff7_externals.current_entity_id_byte_CC0964 )
  {
    ff7_externals.text_box_window_data_array_CFF5B8[WINDOW_ID].current_window_width += ff7_externals.text_box_window_data_array_CFF5B8[WINDOW_ID].window_width / 4;

    if ( ff7_externals.text_box_window_data_array_CFF5B8[WINDOW_ID].current_window_width < 8 )
      ff7_externals.text_box_window_data_array_CFF5B8[WINDOW_ID].current_window_width = 8;

    if ( ff7_externals.text_box_window_data_array_CFF5B8[WINDOW_ID].current_window_width > ff7_externals.text_box_window_data_array_CFF5B8[WINDOW_ID].window_width )
      ff7_externals.text_box_window_data_array_CFF5B8[WINDOW_ID].current_window_width = ff7_externals.text_box_window_data_array_CFF5B8[WINDOW_ID].window_width;

    ff7_externals.text_box_window_data_array_CFF5B8[WINDOW_ID].current_window_height += ff7_externals.text_box_window_data_array_CFF5B8[WINDOW_ID].window_height / 4;

    if ( ff7_externals.text_box_window_data_array_CFF5B8[WINDOW_ID].current_window_height < 8 )
      ff7_externals.text_box_window_data_array_CFF5B8[WINDOW_ID].current_window_height = 8;

    if ( ff7_externals.text_box_window_data_array_CFF5B8[WINDOW_ID].current_window_height > ff7_externals.text_box_window_data_array_CFF5B8[WINDOW_ID].window_height )
      ff7_externals.text_box_window_data_array_CFF5B8[WINDOW_ID].current_window_height = ff7_externals.text_box_window_data_array_CFF5B8[WINDOW_ID].window_height;

    if (
      ff7_externals.text_box_window_data_array_CFF5B8[WINDOW_ID].current_window_width == ff7_externals.text_box_window_data_array_CFF5B8[WINDOW_ID].window_width
      && ff7_externals.text_box_window_data_array_CFF5B8[WINDOW_ID].current_window_height == ff7_externals.text_box_window_data_array_CFF5B8[WINDOW_ID].window_height
    )
      ff7_externals.text_box_window_data_array_CFF5B8[WINDOW_ID].window_mode = 2;
  }
}

int sub_6F54A2_jp(byte *a1)
{
  int v2;
  int v3;
  int v4;
  float scaleFactor = ff7_japanese_edition ? 1.25f : 1.0f; //JP: treat as large for pointer alignment; multibyte EN: native 1.0
  v3 = 0;
  v2 = 0;
  bool kanjiDetected = false;
  int charWidth = 0;
  int leftPadding = 0;
  while ( v3 < 64 && a1 && (unsigned __int8)*a1 != 255 )
  {
    auto next_char = a1 + 1;
    switch ( *a1 )
    {
      case 0xFAu:
        kanjiDetected = true;
        charWidth = charWidthData[1][*next_char] & 0x1F;
        leftPadding = charWidthData[1][*next_char] >> 5;
        ++a1;
        ++v3;
        continue;
      case 0xFBu:
        kanjiDetected = true;
        charWidth = charWidthData[2][*next_char] & 0x1F;
        leftPadding = charWidthData[2][*next_char] >> 5;
        ++a1;
        ++v3;
        continue;
      case 0xFCu:
        kanjiDetected = true;
        charWidth = charWidthData[3][*next_char] & 0x1F;
        leftPadding = charWidthData[3][*next_char] >> 5;
        ++a1;
        ++v3;
        continue;
      case 0xFDu:
        kanjiDetected = true;
        charWidth = charWidthData[4][*next_char] & 0x1F;
        leftPadding = charWidthData[4][*next_char] >> 5;
        ++a1;
        ++v3;
        continue;
      case 0xFEu:

        kanjiDetected = true;
        charWidth = charWidthData[5][*next_char] & 0x1F;
        leftPadding = charWidthData[5][*next_char] >> 5;
        ++a1;
        ++v3;
        continue;
      default:
        if(!kanjiDetected)
        {
          charWidth = charWidthData[0][*a1] & 0x1F;
          leftPadding = charWidthData[0][*a1] >> 5;
        }
        kanjiDetected = false;
        break;
    }
    v2 += leftPadding + std::ceil(std::ceil(z_half_width(charWidth))*scaleFactor); // round after EACH multiplication should align properly with pointers. hooray for inherited jank from no FP in PS1
    ++a1;
    ++v3;
  }
  v2 += 3; // final correction needed for text to align with pointers
  return v2;
}

// ===========================================================================
//  JP name-entry screen: 3-mode (hiragana / katakana / eisuu) support
// ---------------------------------------------------------------------------
//  The classic ff7_ja.exe (detected as US 1.02 engine) already contains the
//  full machinery for a 3-mode JP name screen, but ships with US data:
//    * grid draw loop (0x7191E1 / 0x7198A0) reads the *displayed* grid at
//      0x921D70, but only when the char-mode var [0x00DD46F4]==2 (always 2).
//    * 0x921D70 statically holds only 7 valid hiragana rows (rows 7-8 overflow
//      into adjacent data -> the garbled bottom two rows seen in-game).
//    * full master grids DO exist: hiragana @0x921E10, katakana @0x921E78
//      (each a 90-byte / 10col x 9row table of jafont_1 glyph indices).
//    * the right menu is drawn with a 7-slot loop (0x719224 `cmp 7`) but the
//      table at 0x921D48 only defines 5 US entries (space/delete/ok/default/
//      cancel); slots 5-6 overflow.
//    * menu actions dispatch through a 7-entry jump table @0x719B61.
//  Strategy (no full re-impl): keep [0x00DD46F4]==2 so both draw and the
//  glyph picker (getGlyph @0x718B9A, case 2) use 0x921D70, and simply copy the
//  selected master grid into 0x921D70 whenever the mode changes. Then relocate
//  the menu table to a proper 7-entry JP table, neutralize the first three
//  action slots, and perform the mode switch from a per-frame C hook that
//  mirrors the engine's own dispatch condition (cancel is dropped).
// ===========================================================================

// Full 9-row grids as compile-time data (jafont_1 glyph indices, verified from
// the jafont_1.tex atlas). These mirror the engine's masters (hiragana@0x921E10,
// katakana@0x921E78) but as our own constants, so the displayed grid never
// depends on the live exe .data being ready when we install (the live masters
// are not yet populated at FFNx-init time, which corrupted the initial copy).
static const uint8_t jp_name_grid_hiragana[90] = {
  /* row0 */ 0x6B,0x6D,0x69,0x6F,0x71,0xA5,0xA7,0xA9,0xAB,0xAD, // a i u e o (small)
  /* row1 */ 0x4B,0x4D,0x4F,0x51,0x53,0x0B,0x0D,0x0F,0x11,0x13, // ka.. (ga..)
  /* row2 */ 0x55,0x57,0x59,0x5B,0x5D,0x15,0x17,0x19,0x1B,0x1D, // sa.. (za..)
  /* row3 */ 0x5F,0x61,0x63,0x65,0x67,0x1F,0x21,0x23,0x25,0x27, // ta.. (da..)
  /* row4 */ 0x73,0x75,0x77,0x79,0x7B,0x3F,0x3F,0x3F,0x3F,0xD1, // na.. _ _ _ _ ~
  /* row5 */ 0x41,0x43,0x45,0x47,0x49,0x01,0x03,0x05,0x07,0x09, // ha.. (ba..)
  /* row6 */ 0x7D,0x7F,0x81,0x83,0x85,0x2A,0x2C,0x2E,0x30,0x32, // ma.. (pa..)
  /* row7 */ 0x91,0x93,0x95,0x9F,0xA1,0xA3,0x9D,0x3F,0x3D,0x3E, // ya yu yo (small) _ , .
  /* row8 */ 0x87,0x89,0x8B,0x8D,0x8F,0x97,0x9B,0x99,0xAE,0xAF, // ra.. wa wo n ! ?
};
static const uint8_t jp_name_grid_katakana[90] = { // = hiragana index - 1
  /* row0 */ 0x6A,0x6C,0x68,0x6E,0x70,0xA4,0xA6,0xA8,0xAA,0xAC,
  /* row1 */ 0x4A,0x4C,0x4E,0x50,0x52,0x0A,0x0C,0x0E,0x10,0x12,
  /* row2 */ 0x54,0x56,0x58,0x5A,0x5C,0x14,0x16,0x18,0x1A,0x1C,
  /* row3 */ 0x5E,0x60,0x62,0x64,0x66,0x1E,0x20,0x22,0x24,0x26,
  /* row4 */ 0x72,0x74,0x76,0x78,0x7A,0xD7,0xD8,0xDF,0xE0,0xD0, // na.. [ ] ( ) -
  /* row5 */ 0x40,0x42,0x44,0x46,0x48,0x00,0x02,0x04,0x06,0x08,
  /* row6 */ 0x7C,0x7E,0x80,0x82,0x84,0x29,0x2B,0x2D,0x2F,0x31,
  /* row7 */ 0x90,0x92,0x94,0x9E,0xA0,0xA2,0x9C,0x28,0xCE,0xD2, // ya yu yo (small) vu . ...
  /* row8 */ 0x86,0x88,0x8A,0x8C,0x8E,0x96,0x9A,0x98,0xD5,0xD4, // ra.. wa wo n : /
};

// Our own 90-byte displayed-grid buffer. The engine's grid-draw (0x7191E1,
// 0x7198A0) and glyph-picker (getGlyph case 2, 0x718BF3) originally read the
// grid at 0x921D70; we repoint those three reads at this buffer instead. The
// static 0x921D70 has its first 8 bytes zeroed somewhere before the screen
// appears (shifting the initial hiragana by 8 cells), so owning the buffer
// makes the displayed grid immune to that clobber.
static uint8_t jp_name_current_grid[90];

// Eisuu (alphanumeric) 90-byte grid (10col x 9row). No eisuu master exists in
// the US engine, so this is built from the jafont_1 glyph atlas (verified by
// dumping jafont_1.tex: A-Z=0xB4..0xCD, 0-9=0x33..0x3C, .=0xB2, alpha=0xDB,
// beta=0xDC, *=0xCF, %=0xD3, &=0xD6, ->=0xDA, XIII=0xE6). 0x3F is the blank
// cell. 0xD9 is the heart (it has no glyph of its own in jafont_1).
static const uint8_t jp_name_grid_eisuu[90] = {
  /* row0  A B C D E F G H I J */ 0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,
  /* row1  K L M N O P Q R S T */ 0xBE,0xBF,0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,
  /* row2  U V W X Y Z _ . a b */ 0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0x3F,0xB2,0xDB,0xDC,
  /* row3  0 1 2 3 4 5 6 7 8 9 */ 0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,
  /* row4  * % _ _ & _ _ ♥ ->XIII*/0xCF,0xD3,0x3F,0x3F,0xD6,0x3F,0x3F,0xD9,0xDA,0xE6,
  /* row5 */ 0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,
  /* row6 */ 0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,
  /* row7 */ 0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,
  /* row8 */ 0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,
};

// Relocated 7-entry menu table. Each entry: up to 7 jafont_1 codes + 0xFF
// terminator, 8 bytes/entry (matches `lea reg,[idx*8 + table]`).
// Codes derived from decoded hiragana/katakana indices and the existing US
// table (space/delete/ok/default reused verbatim).
static const uint8_t jp_name_menu_table[7][8] = {
  {0x43,0x87,0x0B,0x73,0xFF,0x00,0x00,0x00}, // ひらがな (hi ra ga na)
  {0x4A,0x5E,0x4A,0x72,0xFF,0x00,0x00,0x00}, // カタカナ (ka ta ka na)
  {0x6F,0x6D,0x59,0x69,0xFF,0x00,0x00,0x00}, // えいすう (e i su u)
  {0x58,0x2F,0xD0,0x58,0xFF,0x00,0x00,0x00}, // スペース (space)
  {0x55,0x4F,0x17,0xA3,0xFF,0x00,0x00,0x00}, // さくじょ (delete)
  {0x51,0x9D,0x65,0x6D,0xFF,0x00,0x00,0x00}, // けってい (confirm)
  {0x24,0x44,0xAC,0x8A,0x66,0xFF,0x00,0x00}, // デフォルト (default)
};

static void jp_name_apply_mode(int mode)
{
  const uint8_t* src;
  if (mode == 1)      src = jp_name_grid_katakana;
  else if (mode == 2) src = jp_name_grid_eisuu;
  else                src = jp_name_grid_hiragana;
  memcpy(jp_name_current_grid, src, 90);
}

// Decision (OK) button bit for the engine's menu-input check
// (menu_input_check_6F53F1 ANDs its mask against a 32-bit button-state
// word). The engine's own dispatch in menu_sub_718DBE tests the bit in
// both halves of that word (it pushes exactly this mask), so we do the same.
#define JP_NAME_DECISION_MASK (0x0020 | (0x0020 << 16))

// Per-frame wrapper around name_menu_sub_719C08 (cdecl, one argument, called
// every frame while the name screen is up). Installed with
// replace_call_function on its single call site, so the original function
// stays untouched and can be called directly. The menu-action jump table is
// reached via a `jmp`, so its targets cannot be plain C functions (there is
// no return address on the stack to `ret` to). Instead the three mode slots
// are pointed at the engine's own continuation (the action becomes a no-op)
// and the mode switch happens here, mirroring the engine's dispatch
// condition inside menu_sub_718DBE:
//   decision button pressed this frame -> menu_input_check_6F53F1(decision mask)
//   right (menu) pane focused          -> *name_menu_selected_pane_921ED4 == 1
//   cursor on one of the 3 mode rows   -> menu-pane cursor row <= 2
// The engine still plays the decision SE itself right before its no-op
// dispatch, so the feedback is unchanged.
static void jp_name_frame_719C08(int a1)
{
  ((void(__cdecl*)(int))ff7_externals.name_menu_sub_719C08)(a1);

  if (*ff7_externals.name_menu_selected_pane_921ED4 == 1)
  {
    // cursor row of the right (menu) pane; the cursor state is one struct
    // per pane, 0x38 bytes each
    uint32_t row = *(uint32_t*)(ff7_externals.name_menu_pane_cursor_rows_DD453C + 1 * 0x38);
    if (row <= 2 && ff7_externals.menu_input_check_6F53F1(JP_NAME_DECISION_MASK))
      jp_name_apply_mode((int)row);
  }
}

// menu_sub_71894B is the name-screen init, called once per screen entry from
// the per-frame loop name_menu_sub_719C08 (guarded by an "inited" flag which
// resets when the screen closes). Its single call site is redirected here
// (replace_call_function), so we call the untouched original directly and
// then force hiragana, so every fresh name screen starts in hiragana instead
// of inheriting the previous character's last-used mode (e.g. Barret's
// screen opening in eisuu after Cloud).
static void jp_name_init_71894B()
{
  ((void(__cdecl*)())ff7_externals.menu_sub_71894B)();
  jp_name_apply_mode(0); // force hiragana for the new screen
}

void name_input_jp_install()
{
  // Both hooks are installed at the (single) call sites, keeping the original
  // functions untouched so they can be called directly from the wrappers.
  // reset to hiragana on every name-screen entry (see jp_name_init_71894B);
  // the init call sits inside name_menu_sub_719C08 (same offset ff7_data.h
  // uses to derive menu_sub_71894B)
  replace_call_function(ff7_externals.name_menu_sub_719C08 + 0x2A, jp_name_init_71894B);

  // mode switching for the first three menu rows (see jp_name_frame_719C08);
  // the per-frame call sits inside name_menu_sub_6CBD32 (same offset
  // ff7_data.h uses to derive name_menu_sub_719C08)
  replace_call_function(ff7_externals.name_menu_sub_6CBD32 + 0x7, jp_name_frame_719C08);

  // relocate the 7-slot menu table: patch the disp32 of both
  //   lea ecx,[eax*8+table]  (menu text draw,   disp32 @menu_sub_718DBE+0x479)
  //   lea eax,[edx*8+table]  (2nd draw routine, disp32 @menu_sub_718DBE+0xB33)
  patch_code_dword(ff7_externals.menu_sub_718DBE + 0x479, (DWORD)(uintptr_t)&jp_name_menu_table[0][0]);
  patch_code_dword(ff7_externals.menu_sub_718DBE + 0xB33, (DWORD)(uintptr_t)&jp_name_menu_table[0][0]);

  // rewire the 7-entry menu action jump table. The original US layout is
  //   [0]=space [1]=delete [2]=confirm [3]=default [4]=cancel
  // and the 7-item JP menu needs
  //   [0..2]=hiragana/katakana/eisuu [3]=space [4]=delete [5]=confirm [6]=default
  // The mode rows become a no-op for the engine (they jump straight to the
  // dispatch continuation); the actual mode change is done in plain C by the
  // per-frame hook (see jp_name_frame_719C08).
  patch_code_dword(ff7_externals.name_menu_action_jump_table_719B61 + 0 * 4, ff7_externals.name_menu_action_continue_71914C);
  patch_code_dword(ff7_externals.name_menu_action_jump_table_719B61 + 1 * 4, ff7_externals.name_menu_action_continue_71914C);
  patch_code_dword(ff7_externals.name_menu_action_jump_table_719B61 + 2 * 4, ff7_externals.name_menu_action_continue_71914C);
  patch_code_dword(ff7_externals.name_menu_action_jump_table_719B61 + 3 * 4, ff7_externals.name_menu_action_space_71905D);   // スペース  (append blank)
  patch_code_dword(ff7_externals.name_menu_action_jump_table_719B61 + 4 * 4, ff7_externals.name_menu_action_delete_71906C);  // さくじょ
  patch_code_dword(ff7_externals.name_menu_action_jump_table_719B61 + 5 * 4, ff7_externals.name_menu_action_confirm_719076); // けってい
  patch_code_dword(ff7_externals.name_menu_action_jump_table_719B61 + 6 * 4, ff7_externals.name_menu_action_default_719147); // デフォルト

  // repoint the three reads of the displayed grid at our own buffer (the
  // patches store the buffer's address; the engine then reads the buffer's
  // contents every frame while drawing the grid / picking glyphs):
  //   grid draw 1:   movzx dx,[ecx+eax+grid]  (disp32 @menu_sub_718DBE+0x428)
  //   grid draw 2:   movzx cx,[eax+edx+grid]  (disp32 @menu_sub_718DBE+0xAE7)
  //   glyph picker:  mov   al,[edx+ecx+grid]  (disp32 @menu_sub_718B9A+0x5C)
  patch_code_dword(ff7_externals.menu_sub_718DBE + 0x428, (DWORD)(uintptr_t)&jp_name_current_grid[0]);
  patch_code_dword(ff7_externals.menu_sub_718DBE + 0xAE7, (DWORD)(uintptr_t)&jp_name_current_grid[0]);
  patch_code_dword(ff7_externals.menu_sub_718B9A + 0x5C, (DWORD)(uintptr_t)&jp_name_current_grid[0]);

  // the right menu now has 7 items; move the column up so all 7 fit on screen.
  //   menu text draw  : y = 0x14A + i*0x22 -> base 0xD2 (imm32 @menu_sub_718DBE+0x486)
  //   menu cursor hand: y = 0x14C + i*0x22 -> base 0xD4 (imm32 @menu_sub_718DBE+0x20E)
  patch_code_dword(ff7_externals.menu_sub_718DBE + 0x486, 0xD2);
  patch_code_dword(ff7_externals.menu_sub_718DBE + 0x20E, 0xD4);

  // initialise the displayed grid to the full 9-row hiragana table
  jp_name_apply_mode(0);
}
