#include "universal_buttons.h"

#include "defs.h"
#include "../gamepad.h"
#include "../globals.h"
#include "../gl.h"
#include "../joystick.h"
#include "../log.h"
#include "../renderer.h"
#include "../sdl_gamepad.h"
#include "../utils.h"

#include <algorithm>
#include <string.h>

static constexpr int prompt_size = 40;
static constexpr int config_prompt_size = 32;

enum class prompt_atlas
{
  keyboard,
  playstation,
  xbox,
  switch_controller,
  count,
};

static prompt_atlas current_prompt_atlas = prompt_atlas::playstation;

struct prompt_resource
{
  ff7_graphics_object* graphics_object = nullptr;
  ff7_texture_set* original_texture_set = nullptr;
  ff7_texture_set* polygon_original_texture_set = nullptr;
  uint32_t texture = 0;
  uint32_t texture_width = 0;
  uint32_t texture_height = 0;
};

static prompt_resource prompt_resources[(int)prompt_atlas::count];

static prompt_resource& resource_for(prompt_atlas atlas)
{
  return prompt_resources[(int)atlas];
}

static bool resource_available(prompt_atlas atlas)
{
  const prompt_resource& resource = resource_for(atlas);
  return resource.graphics_object && resource.texture;
}

static prompt_atlas select_prompt_atlas()
{
  if (use_sdl_gamepad)
  {
    if (!sdlgamepad.CheckConnection())
      return prompt_atlas::keyboard;
    if (sdlgamepad.IsSwitch())
      return prompt_atlas::switch_controller;
    return sdlgamepad.IsXbox() ? prompt_atlas::xbox : prompt_atlas::playstation;
  }

  if (xinput_connected || gamepad.CheckConnection())
    return prompt_atlas::xbox;
  if (joystick.CheckConnection())
    return prompt_atlas::playstation;
  return prompt_atlas::keyboard;
}

void universal_buttons_unload()
{
  for (prompt_resource& resource : prompt_resources)
  {
    ff7_texture_set* private_texture_set = nullptr;
    ff7_polygon_set* polygon_set = resource.graphics_object
      ? (ff7_polygon_set*)resource.graphics_object->polygon_set
      : nullptr;
    if (resource.graphics_object && resource.graphics_object->hundred_data
        && resource.original_texture_set)
    {
      private_texture_set = (ff7_texture_set*)resource.graphics_object->hundred_data->texture_set;
      resource.graphics_object->hundred_data->texture_set =
        (struct texture_set*)resource.original_texture_set;
    }
    if (polygon_set && polygon_set->hundred_data && resource.polygon_original_texture_set)
      polygon_set->hundred_data->texture_set =
        (struct texture_set*)resource.polygon_original_texture_set;

    ff7_externals.sub_671082(&resource.graphics_object);
    if (private_texture_set && private_texture_set != resource.original_texture_set)
    {
      delete private_texture_set->ogl.gl_set;
      delete[] private_texture_set->texturehandle;
      delete private_texture_set;
    }
    if (resource.texture)
      newRenderer.deleteTexture(resource.texture);
    resource = {};
  }
}

static void load_prompt_atlas(prompt_atlas atlas, struc_3* graphics_context,
  char* template_path, ff7_game_obj* game_object)
{
  char path[BASEDIR_LENGTH + 64];
  prompt_resource& resource = resource_for(atlas);
  const char* atlas_name = atlas == prompt_atlas::keyboard
    ? "buttons_pc_en.png"
    : atlas == prompt_atlas::xbox ? "buttons.png"
    : atlas == prompt_atlas::switch_controller ? "buttons_switch.png"
    : "buttons_ps4.png";
  _snprintf(path, sizeof(path), R"(%s\data\png\%s)", basedir, atlas_name);
  if (!fileExists(path))
    return;

  resource.graphics_object = ff7_externals.engine_load_graphics_object_6710AC(
    1, 12, graphics_context, template_path, (int)game_object->dx_sfx_something);
  if (!resource.graphics_object || !resource.graphics_object->hundred_data
      || !resource.graphics_object->hundred_data->texture_set)
    return;

  resource.texture = newRenderer.createTextureLibPng(
    path, &resource.texture_width, &resource.texture_height, true);
  uint32_t expected_width = atlas == prompt_atlas::keyboard ? 1000 : 512;
  uint32_t expected_height = atlas == prompt_atlas::keyboard ? 1200 : 512;
  if (!resource.texture || resource.texture_width != expected_width
      || resource.texture_height != expected_height)
  {
    ffnx_warning("Button prompt atlas has invalid dimensions: %s\n", path);
    return;
  }

  ff7_polygon_set* polygon_set = (ff7_polygon_set*)resource.graphics_object->polygon_set;
  if (!polygon_set || !polygon_set->hundred_data || !polygon_set->hundred_data->texture_set)
  {
    ffnx_warning("Button prompt graphics object has no render texture set\n");
    return;
  }

  resource.original_texture_set =
    (ff7_texture_set*)resource.graphics_object->hundred_data->texture_set;
  resource.polygon_original_texture_set = (ff7_texture_set*)polygon_set->hundred_data->texture_set;
  ff7_tex_header* texture_header = (ff7_tex_header*)resource.polygon_original_texture_set->tex_header;
  uint32_t texture_count = std::max(std::max(texture_header->palettes,
    texture_header->palette_index + 1), 8u);
  ff7_texture_set* private_texture_set =
    new ff7_texture_set(*resource.polygon_original_texture_set);
  private_texture_set->texturehandle = new uint32_t[texture_count];
  memcpy(private_texture_set->texturehandle, resource.polygon_original_texture_set->texturehandle,
    texture_count * sizeof(*private_texture_set->texturehandle));
  private_texture_set->texturehandle[7] = resource.texture;
  private_texture_set->ogl.gl_set = resource.polygon_original_texture_set->ogl.gl_set
    ? new gl_texture_set(*resource.polygon_original_texture_set->ogl.gl_set)
    : nullptr;
  if (private_texture_set->ogl.gl_set)
    private_texture_set->ogl.gl_set->force_filter = true;
  private_texture_set->ogl.external = true;
  private_texture_set->ogl.width = resource.texture_width;
  private_texture_set->ogl.height = resource.texture_height;
  resource.graphics_object->hundred_data->texture_set = (struct texture_set*)private_texture_set;
  polygon_set->hundred_data->texture_set = (struct texture_set*)private_texture_set;
  if (trace_all || trace_gamepad) ffnx_info("Using button prompt atlas: %s\n", path);
}

void universal_buttons_load(struc_3* graphics_context, char* template_path, ff7_game_obj* game_object)
{
  current_prompt_atlas = select_prompt_atlas();
  load_prompt_atlas(prompt_atlas::keyboard, graphics_context, template_path, game_object);
  load_prompt_atlas(prompt_atlas::playstation, graphics_context, template_path, game_object);
  load_prompt_atlas(prompt_atlas::xbox, graphics_context, template_path, game_object);
  load_prompt_atlas(prompt_atlas::switch_controller, graphics_context, template_path, game_object);
  if (!resource_available(current_prompt_atlas))
    current_prompt_atlas = prompt_atlas::playstation;
}

void universal_buttons_draw(ff7_game_obj* game_object)
{
  for (prompt_resource& resource : prompt_resources)
    if (resource.graphics_object && resource.texture)
      ff7_externals.engine_draw_graphics_object_66E641(resource.graphics_object, game_object);
}

void universal_buttons_reset()
{
  for (prompt_resource& resource : prompt_resources)
    if (resource.graphics_object)
      ff7_externals.reset_field_54_graphics_object_66E62C(resource.graphics_object);
}

void universal_buttons_flush_vanilla_field()
{
  ff7_externals.field_draw_text_boxes_and_text_graphics_object_6ECA68();
  ff7_game_obj* game_object = ff7_externals.engine_get_game_object_676578();
  universal_buttons_draw(game_object);
  universal_buttons_reset();
}

void universal_buttons_flush_menu(ff7_graphics_object* graphics_object, ff7_game_obj* game_object)
{
  ff7_externals.engine_draw_graphics_object_66E641(graphics_object, game_object);
  universal_buttons_draw(game_object);
  universal_buttons_reset();
}

struct prompt_sprite
{
  ff7_graphics_object* graphics_object;
  int u;
  int v;
  int width;
  int height;
  int texture_width;
  int texture_height;
};

static int keyboard_atlas_cell(byte key)
{
  switch (key)
  {
    case DIK_SPACE: return 1;
    case DIK_APOSTROPHE: return 2;
    case DIK_COMMA: return 3;
    case DIK_MINUS: return 4;
    case DIK_PERIOD: return 5;
    case DIK_SLASH: return 6;
    case DIK_0: return 7;
    case DIK_1: return 8;
    case DIK_2: return 9;
    case DIK_3: return 10;
    case DIK_4: return 11;
    case DIK_5: return 12;
    case DIK_6: return 13;
    case DIK_7: return 14;
    case DIK_8: return 15;
    case DIK_9: return 16;
    case DIK_SEMICOLON: return 17;
    case DIK_EQUALS: return 18;
    case DIK_A: return 19;
    case DIK_B: return 20;
    case DIK_C: return 21;
    case DIK_D: return 22;
    case DIK_E: return 23;
    case DIK_F: return 24;
    case DIK_G: return 25;
    case DIK_H: return 26;
    case DIK_I: return 27;
    case DIK_J: return 28;
    case DIK_K: return 29;
    case DIK_L: return 30;
    case DIK_M: return 31;
    case DIK_N: return 32;
    case DIK_O: return 33;
    case DIK_P: return 34;
    case DIK_Q: return 35;
    case DIK_R: return 36;
    case DIK_S: return 37;
    case DIK_T: return 38;
    case DIK_U: return 39;
    case DIK_V: return 40;
    case DIK_W: return 41;
    case DIK_X: return 42;
    case DIK_Y: return 43;
    case DIK_Z: return 44;
    case DIK_LBRACKET: return 45;
    case DIK_RBRACKET: return 46;
    case DIK_BACKSLASH: return 47;
    case DIK_ESCAPE: return 48;
    case DIK_RETURN: return 49;
    case DIK_TAB: return 50;
    case DIK_BACK: return 51;
    case DIK_INSERT: return 52;
    case DIK_DELETE: return 53;
    case DIK_RIGHT: return 54;
    case DIK_LEFT: return 55;
    case DIK_DOWN: return 56;
    case DIK_UP: return 57;
    case DIK_PRIOR: return 58;
    case DIK_NEXT: return 59;
    case DIK_HOME: return 60;
    case DIK_END: return 61;
    case DIK_CAPITAL: return 62;
    case DIK_SCROLL: return 63;
    case DIK_NUMLOCK: return 64;
    case DIK_SYSRQ: return 65;
    case DIK_PAUSE: return 66;
    case DIK_F1: return 67;
    case DIK_F2: return 68;
    case DIK_F3: return 69;
    case DIK_F4: return 70;
    case DIK_F5: return 71;
    case DIK_F6: return 72;
    case DIK_F7: return 73;
    case DIK_F8: return 74;
    case DIK_F9: return 75;
    case DIK_F10: return 76;
    case DIK_F11: return 77;
    case DIK_F12: return 78;
    case DIK_NUMPAD0: return 79;
    case DIK_NUMPAD1: return 80;
    case DIK_NUMPAD2: return 81;
    case DIK_NUMPAD3: return 82;
    case DIK_NUMPAD4: return 83;
    case DIK_NUMPAD5: return 84;
    case DIK_NUMPAD6: return 85;
    case DIK_NUMPAD7: return 86;
    case DIK_NUMPAD8: return 87;
    case DIK_NUMPAD9: return 88;
    case DIK_DECIMAL: return 89;
    case DIK_DIVIDE: return 90;
    case DIK_MULTIPLY: return 91;
    case DIK_SUBTRACT: return 92;
    case DIK_ADD: return 93;
    case DIK_NUMPADENTER: return 94;
    case DIK_LSHIFT: return 95;
    case DIK_LCONTROL: return 96;
    case DIK_LMENU: return 97;
    case DIK_LWIN: return 98;
    case DIK_RSHIFT: return 99;
    case DIK_RCONTROL: return 100;
    case DIK_RMENU: return 101;
    case DIK_RWIN: return 102;
    case DIK_APPS: return 103;
    case DIK_GRAVE: return 104;
    default: return -1;
  }
}

static bool sprite_for_button(prompt_atlas atlas, int button, prompt_sprite* sprite)
{
  prompt_resource& resource = resource_for(atlas);
  if (!resource_available(atlas))
    return false;

  if (atlas == prompt_atlas::keyboard)
  {
    int cell = keyboard_atlas_cell((byte)button);
    if (cell < 0)
      return false;
    *sprite = { resource.graphics_object, cell % 10 * 100, cell / 10 * 100, 100, 100,
      (int)resource.texture_width, (int)resource.texture_height };
    return true;
  }

  int column;
  int row;
  switch (button)
  {
    case 0:  column = 3; row = 0; break;
    case 1:  column = 4; row = 0; break;
    case 2:  column = 2; row = 0; break;
    case 3:  column = 2; row = 1; break;
    case 4:  column = 1; row = 1; break;
    case 5:  column = 1; row = 0; break;
    case 6:  column = 0; row = 0; break;
    case 7:  column = 0; row = 1; break;
    case 8:  column = 3; row = 1; break;
    case 11: column = 4; row = 1; break;
    case 12: column = 0; row = 2; break;
    case 13: column = 2; row = 2; break;
    case 14: column = 1; row = 2; break;
    case 15: column = 0; row = 3; break;
    default: return false;
  }

  *sprite = { resource.graphics_object, column * 100, row * 100, 100, 100,
    (int)resource.texture_width, (int)resource.texture_height };
  return true;
}

static bool submit_prompt_quad(const prompt_sprite& sprite, float x, float y, float z, int size)
{
  if (!common_externals.draw_graphics_object(1, (struct graphics_object*)sprite.graphics_object))
    return false;

  graphics_vertex* vertices = sprite.graphics_object->vertex_transform;
  vertices[0].position = { x, y, z, 1.0f };
  vertices[0].color = { 230, 230, 230, 255 };
  vertices[0].alpha_mask = -16777216;
  vertices[0].u = (float)sprite.u / sprite.texture_width;
  vertices[0].v = (float)sprite.v / sprite.texture_height;
  vertices[1] = vertices[0];
  vertices[1].position.y = y + size;
  vertices[1].v += (float)sprite.height / sprite.texture_height;
  vertices[2] = vertices[0];
  vertices[2].position.x = x + size;
  vertices[2].u += (float)sprite.width / sprite.texture_width;
  vertices[3] = vertices[2];
  vertices[3].position.y = y + size;
  vertices[3].v += (float)sprite.height / sprite.texture_height;
  *(byte*)sprite.graphics_object->curr_total_n_shape = 7;
  sprite.graphics_object->field_7C = 7;
  return true;
}

static int keyboard_binding_for_button(int button)
{
  if (button < 0 || button > 15 || button == 9 || button == 10)
    return -1;
  return button + 1;
}

int universal_buttons_draw_field_prompt(int button, int x, int y, float z)
{
  prompt_sprite sprite;
  if (current_prompt_atlas == prompt_atlas::keyboard)
  {
    int binding = keyboard_binding_for_button(button);
    if (!ff7_externals.input_mapping || binding < 0)
      return x;
    button = ff7_externals.input_mapping[binding];
  }
  if (!sprite_for_button(current_prompt_atlas, button, &sprite))
    return x;

  if (!submit_prompt_quad(sprite, (float)x, (float)y - prompt_size / 4.0f, z, prompt_size))
  {
    static bool logged_submit_failure = false;
    if (!logged_submit_failure)
    {
      ffnx_warning("Failed to submit field button prompt for button %d\n", button);
      logged_submit_failure = true;
    }
    return x;
  }
  return x + prompt_size;
}

int universal_buttons_draw_menu_prompt(int button, int x, int y, float z)
{
  prompt_sprite sprite;
  if (current_prompt_atlas == prompt_atlas::keyboard)
  {
    int binding = keyboard_binding_for_button(button);
    if (!ff7_externals.input_mapping || binding < 0)
      return x;
    button = ff7_externals.input_mapping[binding];
  }
  if (!sprite_for_button(current_prompt_atlas, button, &sprite))
    return x;

  if (!submit_prompt_quad(sprite, (float)x, (float)y - config_prompt_size / 4.0f,
      z, config_prompt_size))
    return x;
  return x + config_prompt_size;
}

static int button_for_jp_control(int control)
{
  static constexpr int buttons[] = {
    6, 5, 4, 12, 14, 13, 15, 11, 7, 2, 3, 1, 8, 0,
  };
  if (control < 0 || control >= sizeof(buttons) / sizeof(buttons[0]))
    return -1;
  return buttons[control];
}

int universal_buttons_draw_field_jp_control(int control, int x, int y, float z)
{
  return universal_buttons_draw_field_prompt(button_for_jp_control(control), x, y, z);
}

int universal_buttons_draw_menu_jp_control(int control, int x, int y, float z)
{
  return universal_buttons_draw_menu_prompt(button_for_jp_control(control), x, y, z);
}

int universal_buttons_draw_config_binding(int x, int y, byte* buffer, byte color, float z)
{
  static constexpr byte binding_indices[] = { 6, 7, 5, 8, 3, 4, 1, 2, 9, 12, 13, 15, 16, 14 };
  static constexpr byte controller_button_map[] = { 7, 6, 5, 4, 2, 3, 0, 1, 8, 11 };
  static constexpr int row_height = 24;
  static constexpr int first_row_y = 124;
  const int row = (y - first_row_y) / row_height;
  static constexpr int controller_column_x = 300;
  const int column = x >= controller_column_x ? 1 : 0;
  auto draw_text = [=]() {
    return ff7_japanese_edition
      ? common_submit_draw_text_from_buffer_jp(x, y, buffer, color, z)
      : ff7_externals.draw_string_from_buffer_sub_6F5B03(x, y, buffer, color, z);
  };
  if (!ff7_externals.config_input_mapping || row < 0 || row >= sizeof(binding_indices))
    return draw_text();

  const int binding = ff7_externals.config_input_mapping[25 * column + binding_indices[row]];
  const prompt_atlas connected_atlas = select_prompt_atlas();
  const prompt_atlas atlas = column == 0 ? prompt_atlas::keyboard
    : resource_available(connected_atlas) && connected_atlas != prompt_atlas::keyboard
      ? connected_atlas
      : prompt_atlas::playstation;
  int button = binding;
  if (column == 1)
  {
    switch (binding)
    {
      case 227: button = 12; break;
      case 228: button = 14; break;
      case 229: button = 15; break;
      case 230: button = 13; break;
      default:
        const int raw_button = binding - 235;
        button = raw_button >= 0 && raw_button < sizeof(controller_button_map)
          ? controller_button_map[raw_button]
          : -1;
        break;
    }
  }
  prompt_sprite sprite;
  if (!sprite_for_button(atlas, button, &sprite))
    return draw_text();

  const int size = config_prompt_size;
  if (!submit_prompt_quad(sprite, (float)x, (float)y - size / 4.0f, z, size))
    return draw_text();
  return x + size;
}

bool universal_buttons_parse_field_prompt(const byte* buffer, int* button, int* byte_count)
{
  *byte_count = 1;
  switch (buffer[0])
  {
    case 0xF6:
      switch (buffer[1])
      {
        case 0x0C: *button = 13; break;
        case 0x10: *button = 5; break;
        case 0x11: *button = 2; break;
        case 0x12: *button = 0; break;
        case 0x13: *button = 3; break;
        case 0x14: *button = 1; break;
        case 0x15: *button = 11; break;
        case 0x16: *button = 8; break;
        case 0x17: *button = 12; break;
        case 0x18: *button = 14; break;
        case 0x19: *button = 15; break;
        case 0x33: *button = 5; break;
        case 0x34: *button = 2; break;
        case 0x35: *button = 0; break;
        case 0x36: *button = 3; break;
        case 0x37: *button = 1; break;
        case 0x38: *button = 11; break;
        case 0x39: *button = 8; break;
        case 0x3A: *button = 12; break;
        case 0x3B: *button = 14; break;
        case 0x3C: *button = 15; break;
        case 0x3D: *button = 13; break;
        default: *button = 5; return true;
      }
      *byte_count = 2;
      return true;
    case 0xF7: *button = 4; return true;
    case 0xF8: *button = 7; return true;
    case 0xF9: *button = 6; return true;
    default: return false;
  }
}

int universal_buttons_field_prompt_width()
{
  return prompt_size / 2;
}

int universal_buttons_submit_vanilla_prompt(void* caller_frame, int use_alpha,
  struct graphics_object* graphics_object)
{
  byte*& buffer = *(byte**)((byte*)caller_frame + 0x14);
  int button;
  int byte_count;
  if (!universal_buttons_parse_field_prompt(buffer, &button, &byte_count))
    return 0;

  int16_t& x = *(int16_t*)((byte*)caller_frame + 0x08);
  int16_t y = *(int16_t*)((byte*)caller_frame + 0x0C);
  float z = *(float*)((byte*)caller_frame + 0x18);
  if (universal_buttons_draw_field_prompt(button, x, y, z) == x)
    return common_externals.draw_graphics_object(use_alpha, graphics_object);

  x += prompt_size - 16;
  if (byte_count == 2)
  {
    ++buffer;
    ++(*ff7_externals.field_text_box_curr_n_characters_DC3CB0);
  }
  *ff7_externals.field_do_draw_text_boxes_DC3CE8 = 1;
  return 0;
}