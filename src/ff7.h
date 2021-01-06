/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2021 Julian Xhokaxhiu                                   //
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

#include <stdio.h>

#include "common.h"

/*
 * Primitive types supported by the engine, mostly a 1:1 mapping to PSX GPU
 * capabilities.
 *
 * Key:
 * T/S/L - triangle/square(quad)/line
 * F/G - flat/gouraud
 * T - textured
 * 2D/3D - self explanatory except 2D means no transforms at all, billboard sprites etc are still "3D"
 */
enum polygon_types
{
	PT_TF2D = 0,
	PT_TF3D,
	PT_TG2D,
	PT_TG3D,
	PT_T2D,
	PT_T3D,
	PT_TGT2D,
	PT_TGT3D,
	PT_SF2D,
	PT_SF3D,
	PT_SG2D,
	PT_SG3D,
	PT_S2D,
	PT_S3D,
	PT_SGT2D,
	PT_SGT3D,
	polygon_type_10,
	polygon_type_11,
	PT_LF2D,
	PT_L2D,
};

// FF7 modules, unknowns are either unused or not relevant to rendering
enum ff7_game_modes
{
	FF7_MODE_FIELD = 1,
	FF7_MODE_BATTLE,
	FF7_MODE_WORLDMAP,
	FF7_MODE_UNKNOWN4,
	FF7_MODE_MENU,
	FF7_MODE_HIGHWAY,
	FF7_MODE_CHOCOBO,
	FF7_MODE_SNOWBOARD,
	FF7_MODE_CONDOR,
	FF7_MODE_SUBMARINE,
	FF7_MODE_COASTER,
	FF7_MODE_CDCHECK,
	FF7_MODE_UNKNOWN13,
	FF7_MODE_SNOWBOARD2,
	FF7_MODE_UNKNOWN15,
	FF7_MODE_UNKNOWN16,
	FF7_MODE_BATTLE_MENU,
	FF7_MODE_UNKNOWN18,
	FF7_MODE_EXIT,
	FF7_MODE_MAIN_MENU,
	FF7_MODE_UNKNOWN21,
	FF7_MODE_INTRO,
	FF7_MODE_SWIRL,
	FF7_MODE_UNKNOWN24,
	FF7_MODE_UNKNOWN25,
	FF7_MODE_GAMEOVER,
	FF7_MODE_CREDITS,
	FF7_MODE_UNKNOWN28,
};

// 3D model flags
enum model_modes
{
	MDL_ROOT_ROTATION         = 0x0001,
	MDL_ROOT_ROTATION_NEGX    = 0x0002,
	MDL_ROOT_ROTATION_NEGY    = 0x0004,
	MDL_ROOT_ROTATION_NEGZ    = 0x0008,
	MDL_ROOT_TRANSLATION      = 0x0010,
	MDL_ROOT_TRANSLATION_NEGX = 0x0020,
	MDL_ROOT_TRANSLATION_NEGY = 0x0040,
	MDL_ROOT_TRANSLATION_NEGZ = 0x0080,
	MDL_USE_STRUC110_MATRIX   = 0x4000,
	MDL_USE_CAMERA_MATRIX     = 0x8000,
};

// internal structure for menu sprites (global values, may not be a structure at all)
struct menu_objects
{
	struct ff7_graphics_object *unknown1;
	struct ff7_graphics_object *unused;
	struct ff7_graphics_object *btl_win_a;
	struct ff7_graphics_object *btl_win_b;
	struct ff7_graphics_object *btl_win_c;
	struct ff7_graphics_object *btl_win_d;
	struct ff7_graphics_object *_btl_win;
	struct ff7_graphics_object *blend_btl_win_a;
	struct ff7_graphics_object *add_btl_win_a;
	struct ff7_graphics_object *add_btl_win_b;
	struct ff7_graphics_object *add_btl_win_c;
	struct ff7_graphics_object *add_btl_win_d;
	struct ff7_graphics_object *window_bg;
	struct ff7_graphics_object *blend_window_bg;
	struct ff7_graphics_object *unknown2;
	struct ff7_graphics_object *unknown3;
	struct ff7_graphics_object *unknown4;
	struct ff7_graphics_object *unknown5;
	struct ff7_graphics_object *menu_fade;
	struct ff7_graphics_object *font_a;
	struct ff7_graphics_object *font_b;
	struct ff7_graphics_object *menu_avatars[3];
	struct ff7_graphics_object *menu_avatars2[9];
	struct ff7_graphics_object *buster_tex;
	struct ff7_graphics_object *font;
	struct ff7_graphics_object *btl_win;
	struct ff7_graphics_object *blend_btl_win;
	struct ff7_graphics_object *add_btl_win;
};

// file modes
enum
{
	FF7_FMODE_READ = 0,
	FF7_FMODE_READ_TEXT,
	FF7_FMODE_WRITE,
	FF7_FMODE_CREATE,
};

/*
 * This section defines some structures used internally by the FF7 game engine.
 *
 * Documentation for some of them can be found on the Qhimm wiki, a lot of
 * information can be gleaned from the source code to this program but in many
 * cases nothing is known except the size and general layout of the structure.
 *
 * Variable and structure names are mostly based on what they contain rather
 * than what they are for, a lot of names may be wrong, inappropriate or
 * downright misleading. Thread with caution!
 */

struct list
{
	uint32_t use_assert_alloc;
	uint32_t field_4;
	uint32_t nodes;
	struct list_node *head;
	struct list_node *tail;
	void *destructor;
	void *recursive_find_cb;
	uint32_t field_1C;
};

struct list_node
{
	struct list_node *next;
	void *object;
};

struct file_context
{
	uint32_t mode;
	uint32_t use_lgp;
	uint32_t lgp_num;
	void (*name_mangler)(char *, char *);
};

struct ff7_file
{
	char *name;
	struct lgp_file *fd;
	struct file_context context;
};

struct ff7_indexed_vertices
{
	uint32_t field_0;
	uint32_t field_4;
	uint32_t count;
	uint32_t vertexcount;
	uint32_t field_10;
	struct nvertex *vertices;
	uint32_t indexcount;
	uint32_t field_1C;
	WORD *indices;
	uint32_t field_24;
	unsigned char *palettes;
	uint32_t field_2C;
	struct ff7_graphics_object *graphics_object;
};

struct ff7_graphics_object
{
	uint32_t polytype;
	uint32_t field_4;
	uint32_t field_8;
	struct p_hundred *hundred_data;
	struct matrix_set *matrix_set;
	struct polygon_set *polygon_set;
	uint32_t field_18;
	uint32_t field_1C;
	uint32_t field_20;
	float u_offset;
	float v_offset;
	void *dx_sfx_2C;
	void *graphics_instance;
	uint32_t field_34;
	uint32_t vertices_per_shape;
	uint32_t indices_per_shape;
	uint32_t vertex_offset;
	uint32_t index_offset;
	uint32_t field_48;
	uint32_t field_4C;
	uint32_t field_50;
	uint32_t field_54;
	uint32_t field_58;
	uint32_t field_5C;
	uint32_t field_60;
	uint32_t field_64;
	uint32_t field_68;
	uint32_t field_6C;
	uint32_t field_70;
	uint32_t field_74;
	uint32_t field_78;
	uint32_t field_7C;
	uint32_t field_80;
	uint32_t field_84;
	uint32_t field_88;
	struct ff7_indexed_vertices *indexed_vertices;
	gfx_polysetrenderstate *func_90;
	gfx_draw_vertices *func_94;
	uint32_t use_matrix_pointer;
	struct matrix *matrix_pointer;
	struct matrix matrix;
};

struct polygon_group
{
	uint32_t field_0;
	uint32_t numvert;
	void *driver_data;
	uint32_t field_C;
	uint32_t normindexes;
	uint32_t vertices;
	uint32_t vertex_colors;
	uint32_t texcoords;
	uint32_t texture_set;
};

struct struc_106
{
	uint32_t field_0;
	uint32_t color;
	struct point3d point;
	struct bgra_color d3dcol;
};

struct ff7_light
{
	uint32_t flags;
	uint32_t field_4;
	struct struc_106 *struc_106_1;
	struct struc_106 *struc_106_2;
	struct struc_106 *struc_106_3;
	struct bgra_color d3dcol4;
	struct bgra_color normd3dcol4;
	uint32_t color4;
	struct matrix field_38;
	struct matrix field_78;
	struct matrix field_B8;
	struct matrix field_F8;
	uint32_t field_138;
	struct matrix field_13C;
	uint32_t field_17C;
	uint32_t field_180;
	uint32_t field_184;
	uint32_t field_188;
	uint32_t field_18C;
	uint32_t field_190;
	uint32_t field_194;
	uint32_t field_198;
	struct matrix *matrix_pointer;
	uint32_t field_1A0;
	uint32_t field_1A4[256];
	uint32_t field_5A4;
	uint32_t color;
};

struct ff7_polygon_set
{
	uint32_t field_0;
	uint32_t field_4;
	uint32_t field_8;
	uint32_t field_C;
	uint32_t numgroups;
	struct struc_49 field_14;
	uint32_t field_2C;
	struct polygon_data *polygon_data;
	struct p_hundred *hundred_data;
	uint32_t per_group_hundreds;
	struct p_hundred **hundred_data_group_array;
	struct matrix_set *matrix_set;
	struct ff7_light *light;
	uint32_t field_48;
	void *execute_buffers;			// IDirect3DExecuteBuffer **
	struct indexed_primitive **indexed_primitives;
	uint32_t field_54;
	uint32_t field_58;
	struct polygon_group *polygon_group_array;
	uint32_t field_60;
	uint32_t field_64;
	uint32_t field_68;
	uint32_t field_6C;
	uint32_t field_70;
	uint32_t field_74;
	uint32_t field_78;
	uint32_t field_7C;
	uint32_t field_80;
	uint32_t field_84;
	uint32_t field_88;
	uint32_t field_8C;
	uint32_t field_90;
	uint32_t has_struc_173;
	uint32_t field_98;
	struct struc_173 *struc_173;
	uint32_t field_A0;
	uint32_t field_A4;
	uint32_t field_A8;
	uint32_t field_AC;
};

struct ff7_tex_header
{
	uint32_t version;
	uint32_t field_4;
	uint32_t color_key;
	uint32_t field_C;
	uint32_t field_10;
	union
	{
		struct
		{
			uint32_t minbitspercolor;
			uint32_t maxbitspercolor;
			uint32_t minalphabits;
			uint32_t maxalphabits;
		} v1_1;

		struct
		{
			uint32_t x;
			uint32_t y;
			uint32_t w;
			uint32_t h;
		} fb_tex;
	};
	union
	{
		struct
		{
			uint32_t minbitsperpixel;
			uint32_t maxbitsperpixel;
		} v1_2;

		struct
		{
			char *psx_name;
			char *pc_name;
		} file;
	};
	uint32_t field_2C;
	uint32_t palettes;					// ?
	uint32_t palette_entries;			// ?
	uint32_t bpp;
	struct texture_format tex_format;
	uint32_t use_palette_colorkey;
	char *palette_colorkey;
	uint32_t reference_alpha;
	uint32_t blend_mode;
	uint32_t field_CC;
	uint32_t palette_index;
	unsigned char *image_data;
	unsigned char *old_palette_data;
	uint32_t field_DC;
	uint32_t field_E0;
	uint32_t field_E4;
	uint32_t field_E8;
};

struct ff7_texture_set
{
	union
	{
		struct
		{
			void *ddsurface1;
			void *d3d2texture1;
			void *ddsurface2;
			void *d3d2texture2;
		} d3d;

		struct
		{
			uint32_t external;
			struct gl_texture_set *gl_set;
			uint32_t width;
			uint32_t height;
		} ogl;
	};

	uint32_t field_10;
	uint32_t field_14;
	uint32_t refcount;
	uint32_t field_1C;
	uint32_t field_20;
	uint32_t field_24;
	uint32_t field_28;
	uint32_t field_2C;
	uint32_t field_30;
	uint32_t field_34;
	uint32_t field_38;
	uint32_t field_3C;
	uint32_t field_40;
	uint32_t field_44;
	uint32_t field_48;
	uint32_t field_4C;
	uint32_t field_50;
	uint32_t field_54;
	uint32_t field_58;
	uint32_t field_5C;
	uint32_t field_60;
	uint32_t field_64;
	uint32_t field_68;
	uint32_t field_6C;
	uint32_t field_70;
	uint32_t field_74;
	uint32_t field_78;
	uint32_t *texturehandle;
	struct texture_format *texture_format;
	struct tex_header *tex_header;
	uint32_t palette_index;
	struct palette *palette;
	uint32_t field_90;
	uint32_t field_94;
	uint32_t field_98;
	uint32_t field_9C;
};

struct field_layer
{
	struct ff7_tex_header *tex_header;
	void *image_data;
	struct ff7_graphics_object *graphics_object;
	uint32_t present;
	uint32_t field_10;
	uint32_t field_14;
	WORD field_18;
	WORD type;
};

struct field_object
{
	char name[256];
	char field_100[256];
	char hrc_filename[256];
	char field_300[33];
	char field_321;
	char field_322;
	char field_323;
	char field_324;
	char field_325;
	char field_326;
	unsigned char r_ambient;
	unsigned char g_ambient;
	unsigned char b_ambient;
	unsigned char r_light1;
	unsigned char g_light1;
	unsigned char b_light1;
	unsigned char r_light2;
	unsigned char g_light2;
	unsigned char b_light2;
	unsigned char r_light3;
	unsigned char g_light3;
	unsigned char b_light3;
	unsigned char field_333;
	short x_light1;
	short y_light1;
	short z_light1;
	short x_light2;
	short y_light2;
	short z_light2;
	short x_light3;
	short y_light3;
	short z_light3;
	WORD field_346;
	WORD field_348;
	WORD num_animations;
	char anim_filenames[8880];
	char field_25FC[592];
	char field_284C[60];
	uint32_t field_2888;
};

struct struc_110
{
	uint32_t field_0;
	struct point3d position;
	struct point3d rotation;
	struct point3d scale;
	float scale_factor;
	struct matrix matrix;
	struct point3d *bone_positions;
	struct matrix *bone_matrices;
};

struct battle_chdir_struc
{
	uint32_t sucess;
	char olddir[200];
};

struct battle_hrc_bone
{
	uint32_t parent;
	float bone_length;
	uint32_t num_rsd;
};

struct battle_hrc_header
{
	uint32_t field_0;
	uint32_t field_4;
	uint32_t field_8;
	uint32_t bones;
	uint32_t field_10;
	uint32_t field_14;
	uint32_t num_textures;
	uint32_t num_animations_1;
	uint32_t animations_2_start_index;
	uint32_t num_weapons;
	uint32_t num_animations_2;
	uint32_t field_2C;
	struct battle_hrc_bone *bone_data;
};

struct anim_frame_header
{
	struct point3d root_rotation;
	struct point3d root_translation;
};

struct anim_frame
{
	struct anim_frame_header *header;
	struct point3d *data;
};

struct anim_header
{
	union
	{
		struct
		{
			uint32_t version;
		} version;

		struct
		{
			char *pc_name;
		} file;
	};
	uint32_t num_frames;
	uint32_t num_bones;
	char rotation_order[4];
	void *frame_data;
	struct anim_frame *anim_frames;
	uint32_t use_matrix_array;
	struct matrix *matrix_array;
	struct matrix *current_matrix_array;
};

struct hrc_data
{
	uint32_t field_0;
	uint32_t field_4;
	uint32_t debug;
	uint32_t flags;
	uint32_t num_bones;
	struct hrc_bone *bones;
	uint32_t field_18;
	struct list *bone_list;
	struct ff7_game_obj *game_object;
	struct matrix field_24;
	struct matrix field_64;
	uint32_t *field_A4;
};

struct hrc_bone
{
	char *bone_name;
	char *bone_parent;
	uint32_t parent_index;
	float bone_length;
	uint32_t num_rsd;
	char **rsd_names;
	struct rsd_array_member *rsd_array;
};

struct bone_list_member
{
	WORD bone_type;
	WORD bone_index;
};

struct rsd_array_member
{
	uint32_t field_0;
	struct rsd_data *rsd_data;
};

struct rsd_data
{
	struct matrix_set *matrix_set;
	struct ff7_polygon_set *polygon_set;
	struct pd_data *pd_data;
};

struct lgp_toc_entry
{
	char name[16];
	uint32_t offset;
	WORD unknown1;
	WORD conflict;
};

struct lookup_table_entry
{
	unsigned short toc_offset;
	unsigned short num_files;
};

struct conflict_entry
{
	char name[128];
	unsigned short toc_index;
};

struct conflict_list
{
	uint32_t num_conflicts;
	struct conflict_entry *conflict_entries;
};

struct lgp_folders
{
	struct conflict_list conflicts[1000];
};

struct hpmp_bar
{
	WORD x;
	WORD y;
	WORD w;
	WORD h;
	WORD value1;
	WORD max_value;
	WORD healing_animation;
	WORD value2;
	uint32_t color;
};

struct savemap_char
{
	char id;
	char level;
	char field_2;
	char field_3;
	char field_4;
	char field_5;
	char dex;
	char field_7;
	char field_8;
	char field_9;
	char field_A;
	char field_B;
	char field_C;
	char field_D;
	char current_limit_level;
	unsigned char current_limit_bar;
	char name[12];
	char equipped_weapon;
	char equipped_armor;
	char field_1E;
	char flags;
	char field_20;
	unsigned char level_progress_bar;
	WORD field_22;
	WORD field_24;
	WORD field_26;
	WORD field_28;
	WORD field_2A;
	WORD hp;
	WORD base_hp;
	WORD mp;
	WORD base_mp;
	uint32_t field_34;
	WORD max_hp;
	WORD max_mp;
	uint32_t current_exp;
	uint32_t field_40;
	uint32_t field_44;
	uint32_t field_48;
	uint32_t field_4C;
	uint32_t field_50;
	uint32_t field_54;
	uint32_t field_58;
	uint32_t field_5C;
	uint32_t field_60;
	uint32_t field_64;
	uint32_t field_68;
	uint32_t field_6C;
	uint32_t field_70;
	uint32_t field_74;
	uint32_t field_78;
	uint32_t field_7C;
	uint32_t exp_to_next_level;
};

#pragma pack(push,1)

struct savemap
{
	uint32_t checksum;
	char preview_level;
	char preview_portraits[3];
	char preview_char_name[16];
	WORD preview_hp;
	WORD preview_maxhp;
	WORD preview_mp;
	WORD preview_maxmp;
	uint32_t preview_gil;
	uint32_t preview_seconds;
	char preview_location[32];
	char ul_window_red;
	char ul_window_green;
	char ul_window_blue;
	char ur_window_red;
	char ur_window_green;
	char ur_window_blue;
	char ll_window_red;
	char ll_window_green;
	char ll_window_blue;
	char lr_window_red;
	char lr_window_green;
	char lr_window_blue;
	struct savemap_char chars[9];
	unsigned char party_members[3];
	char field_4FB;
	WORD items[320];
	uint32_t materia[200];
	uint32_t stolen_materia[48];
	uint32_t field_B5C;
	uint32_t field_B60;
	uint32_t field_B64;
	uint32_t field_B68;
	uint32_t field_B6C;
	uint32_t field_B70;
	uint32_t field_B74;
	uint32_t field_B78;
	uint32_t gil;
	uint32_t seconds;
	uint32_t field_B84;
	uint32_t field_B88;
	uint32_t field_B8C;
	uint32_t field_B90;
	WORD current_mode;
	WORD current_location;
	WORD field_B98;
	WORD x;
	WORD y;
	WORD z_walkmeshtri;
	char field_BA0;
	char field_BA1;
	char field_BA2;
	char field_BA3;
	char field_BA4[256];
	char field_CA4[256];
	char field_DA4[256];
	char field_EA4[256];
	char field_FA4[256];
	WORD phs_lock2;
	char field_10A6;
	char field_10A7;
	char field_10A8;
	char field_10A9;
	char field_10AA;
	char field_10AB;
	char field_10AC;
	WORD phs_lock;
	WORD phs_visi;
	char field_10B1;
	WORD field_10B2;
	uint32_t field_10B4;
	uint32_t field_10B8;
	uint32_t field_10BC;
	uint32_t field_10C0;
	uint32_t field_10C4;
	uint32_t field_10C8;
	uint32_t field_10CC;
	uint32_t field_10D0;
	uint32_t field_10D4;
	char battle_speed;
	char battle_msg_speed;
	char field_10DA;
	char field_10DB;
	uint32_t field_10DC;
	uint32_t field_10E0;
	uint32_t field_10E4;
	uint32_t field_10E8;
	char message_speed;
	char field_10ED;
	WORD field_10EE;
	uint32_t field_10F0;
};

struct weapon_data
{
	char field_0;
	char field_1;
	char field_2;
	char field_3;
	unsigned char attack_stat;
	char field_5;
	char field_6;
	char field_7;
	char field_8;
	char field_9;
	char field_A;
	char field_B;
	WORD field_C;
	WORD field_E;
	WORD field_10;
	WORD field_12;
	char stat_increase_types[4];
	char stat_increase_amounts[4];
	char field_1C[8];
	char field_24;
	char field_25;
	char field_26;
	char field_27;
	WORD field_28;
	WORD field_2A;
};

struct armor_data
{
	char field_0;
	char field_1;
	unsigned char defense_stat;
	unsigned char mdef_stat;
	char field_4;
	char field_5;
	char field_6;
	WORD field_7;
	char field_9[8];
	char field_11;
	WORD field_12;
	WORD field_14;
	WORD field_16;
	uint32_t field_18;
	uint32_t field_1C;
	WORD field_20;
	WORD field_22;
};

struct party_member_data
{
	char field_0;
	char field_1;
	char field_2;
	char field_3;
	char field_4;
	char field_5;
	char field_6;
	char field_7;
	WORD field_8;
	WORD field_A;
	WORD field_C;
	WORD field_E;
	WORD hp;
	WORD max_hp;
	WORD mp;
	WORD max_mp;
	WORD field_18;
	WORD field_1A;
	WORD field_1C;
	WORD field_1E;
	char field_20;
	char field_21;
	char field_22;
	char field_23;
	char field_24[24];
	WORD field_3C;
	WORD field_3E;
	WORD field_40;
	WORD field_42;
	uint32_t field_44;
	uint32_t field_48;
	uint32_t field_4C[24];
	char field_AC[8];
	uint32_t field_B4[21];
	uint32_t field_108[112];
	uint32_t field_2C8[32];
	uint32_t field_348[48];
	struct weapon_data weapon_data;
	uint32_t field_434;
	uint32_t field_438;
	uint32_t field_43C;
};

#pragma pack(pop)

struct field_tile
{
	short x;
	short y;
	float z;
	WORD field_8;
	WORD field_A;
	WORD img_x;
	WORD img_y;
	float u;
	float v;
	WORD fx_img_x;
	WORD fx_img_y;
	uint32_t field_1C;
	uint32_t field_20;
	uint32_t field_24;
	uint32_t field_28;
	WORD tile_size_x;
	WORD tile_size_y;
	WORD palette_index;
	WORD flags;
	char anim_group;
	char anim_bitmask;
	WORD field_36;
	char field_38[4096];
	WORD field_1038;
	WORD field_103A;
	uint32_t use_fx_page;
	uint32_t field_1040;
	uint32_t field_1044;
	uint32_t field_1048;
	uint32_t field_104C;
	char field_1050;
	char field_1051;
	char field_1052;
	char field_1053;
	WORD blend_mode;
	WORD page;
	WORD fx_page;
	WORD field_105A;
};

struct struc_3
{
	uint32_t field_0;
	uint32_t field_4;
	uint32_t convert_animations;
	uint32_t create_matrix_set;
	uint32_t field_10;
	uint32_t matrix_set_size;
	struct graphics_instance *graphics_instance;
	uint32_t field_1C;
	uint32_t blend_mode;
	uint32_t base_directory;
	struct ff7_tex_header *tex_header;
	uint32_t field_2C;
	uint32_t light;
	uint32_t field_34;
	float bone_scale_factor;
	uint32_t field_3C;
	struct file_context file_context;
	uint32_t field_50;
	uint32_t field_54;
	uint32_t field_58;
	uint32_t palette_index;
	uint32_t field_60;
	uint32_t field_64;
	uint32_t field_68;
	uint32_t field_6C;
	uint32_t field_70;
};

struct wordmatrix
{
	WORD _11;
	WORD _12;
	WORD _13;
	WORD _21;
	WORD _22;
	WORD _23;
	WORD _31;
	WORD _32;
	WORD _33;
	uint32_t _41;
	uint32_t _42;
	uint32_t _43;
};

struct struc_154_2
{
	short field_0;
	WORD field_2;
	WORD field_4;
	WORD field_6;
	WORD field_8;
	WORD field_A;
	WORD field_C;
	WORD field_E;
	uint32_t field_10;
	uint32_t field_14;
	unsigned char field_18[8];
};

struct struc_154_3
{
	short field_0;
	WORD field_2;
	WORD field_4;
	WORD field_6;
	WORD field_8;
	WORD field_A;
	WORD field_C;
	WORD field_E;
	uint32_t field_10;
	uint32_t field_14;
	unsigned char field_18[8];
};

struct struc_154
{
	short field_0;
	WORD field_2;
	WORD field_4;
	WORD field_6;
	WORD field_8;
	WORD field_A;
	WORD field_C;
	WORD field_E;
	uint32_t field_10;
	uint32_t field_14;
	unsigned char field_18[8];
};

struct struc_205
{
	short field_0;
	WORD field_2;
	WORD field_4;
	WORD field_6;
	WORD field_8;
	WORD field_A;
	WORD field_C;
	WORD field_E;
	uint32_t field_10;
	uint32_t field_14;
	unsigned char field_18[16];
};

struct movie_obj
{
	void *ddstream;
	uint32_t field_4;
	void *mediastream;
	uint32_t loop;
	uint32_t field_10;
	DDSURFACEDESC movie_sdesc;
	void *graphbuilder;
	uint32_t movie_surfaceheight;
	uint32_t field_88;
	void *amms;
	void *movie_surface;
	void *sample;
	uint32_t movie_left;
	uint32_t movie_top;
	uint32_t movie_right;
	uint32_t movie_bottom;
	uint32_t target_left;
	uint32_t target_top;
	uint32_t target_right;
	uint32_t target_bottom;
	void *sts1;
	void *vts1;
	void *sts2;
	void *vts2;
	void *st1;
	void *vt1;
	void *st2;
	void *vt2;
	uint32_t vt1handle;
	uint32_t vt2handle;
	uint32_t field_E0;
	uint32_t movie_surfacewidth;
	uint32_t field_E8;
	struct nvertex movie_vt2prim[4];
	struct nvertex movie_vt1prim[4];
	void *mediaseeking;
	uint32_t graphics_mode;
	uint32_t field_1F4;
	uint32_t field_1F8;
	uint32_t is_playing;
	uint32_t movie_end;
	uint32_t global_movie_flag;
};

struct dll_gfx_externals
{
	void *(*assert_free)(void *, const char *, uint32_t);
	void *(*assert_malloc)(uint32_t, const char *, uint32_t);
	void *(*assert_calloc)(uint32_t, uint32_t, const char *, uint32_t);
	struct texture_format *(*create_texture_format)();
	void (*add_texture_format)(struct texture_format *, struct game_obj *);
	struct game_obj *(*get_game_object)();
	uint32_t free_driver;
	uint32_t create_gfx_driver;
	void (*make_pixelformat)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, struct texture_format *);
	uint32_t gltexformat2texformat;
	uint32_t sub_686143;
	uint32_t sub_6861EC;
	uint32_t sub_68631E;
	uint32_t sub_686351;
	uint32_t pal_pixel2bgra;
	uint32_t pal_pixel2bgra_8bit;
	uint32_t texture_set_destroy_pal;
	struct palette *(*create_palette_for_tex)(uint32_t, struct tex_header *, struct texture_set *);
	uint32_t convert_texture;
	uint32_t texture_set_decref;
	struct texture_set *(*create_texture_set)();
	uint32_t write_palette;
	uint32_t rgba2d3dcol;
	uint32_t sub_6A5FEB;
	uint32_t sub_6A604A;
	uint32_t destroy_palette;
	uint32_t create_palette;
	uint32_t call_gfx_write_palette;
	uint32_t call_gfx_palette_changed;
	uint32_t sub_6A5A70;
	uint32_t sub_6A5BA0;
	uint32_t sub_6A5C3B;
	uint32_t sub_6A5CE2;
	void *(*sub_6A2865)(void *);
	gfx_load_group *generic_load_group;
	gfx_light_polygon_set *generic_light_polygon_set;
};

struct ff7_game_obj
{
	uint32_t unknown_0;
	uint32_t dc_horzres;
	uint32_t dc_vertres;
	uint32_t dc_bitspixel;
	uint32_t window_pos_x;
	uint32_t window_pos_y;
	uint32_t window_size_x;
	uint32_t window_size_y;
	uint32_t field_20;
	uint32_t field_24;
	uint32_t field_28;
	uint32_t field_2C;
	double countspersecond;
	time_t _countspersecond;
	uint32_t field_40;
	uint32_t field_44;
	double fps;
	uint32_t tsc;
	uint32_t field_54;					// tsc high bits?
	HINSTANCE hinstance;
	HWND hwnd;
	uint32_t field_60;
	uint32_t field_64;
	uint32_t field_68;
	uint32_t field_6C;
	uint32_t field_70;
	void *dddevice;
	void *dd2interface;
	void *front_surface[3];
	DDSURFACEDESC front_surface_desc[3];
	uint32_t field_1CC;
	uint32_t field_1D0;
	IDirectDrawClipper* dd_clipper;
	uint32_t field_1D8;
	DDSURFACEDESC d3d_surfacedesc;
	void *dd_interface;
	uint32_t field_24C;
	DDSURFACEDESC dd_surfacedesc;
	struct list *d3ddev_list;
	void *d3dinterface;
	void *surface_d3ddev;			// IDirect3DDevice
	struct list *textureformat_list;
	void *d3ddev_struct;
	void *d3dviewport;
	void *d3dmaterial;
	uint32_t field_2D8;
	void *d3d2interface;
	void *d3d2device;
	void *d3dviewport2;
	struct list *list_2E8;
	struct polygon_set *polygon_set_2EC;
	struct polygon_set *polygon_set_2F0;
	struct stack *matrix_stack1;
	struct stack *matrix_stack2;
	struct matrix *camera_matrix;
	struct graphics_instance *graphics_instance;
	uint32_t field_304;
	uint32_t field_308;
	uint32_t field_30C;
	uint32_t field_310;
	uint32_t field_314;
	uint32_t field_318;
	uint32_t field_31C;
	uint32_t field_320;
	D3DDEVICEDESC d3d_halcaps;
	D3DDEVICEDESC d3d_helcaps;
	DDCAPS_DX5 halcaps;
	DDCAPS_DX5 helcaps;
	uint32_t field_794;
	uint32_t field_798;
	struct texture_format surface_tex_format;
	uint32_t in_scene;
	struct p_hundred *hundred_array[5];
	void *applog_debug1;
	uint32_t applog_debug2;
	void *dxdbg_file;
	uint32_t field_840;
	uint32_t field_844;
	uint32_t _res_x;
	uint32_t _res_y;
	uint32_t _res_w;
	uint32_t _res_h;
	uint32_t field_858;
	uint32_t field_85C;
	uint32_t field_860;
	uint32_t field_864;
	uint32_t field_868;
	uint32_t field_86C;
	uint32_t field_870;
	uint32_t field_874;
	uint32_t field_878;
	uint32_t field_87C;
	uint32_t field_880;
	uint32_t field_884;
	uint32_t field_888;
	uint32_t field_88C;
	struct matrix matrix_890;
	struct matrix matrix_8D0;
	void *dx_sfx_something;
	struct list *tex_list_pointer;
	struct stack *stack_918;
	uint32_t field_91C;
	void *_3d2d_something;
	uint32_t field_924;
	uint32_t field_928;
	uint32_t field_92C;
	uint32_t field_930;
	struct gfx_driver *gfx_driver;
	void *_3dobject_pool;
	uint32_t field_93C;
	struct p_hundred *current_hundred;
	struct struc_81 *field_944;
	uint32_t field_948;
	uint32_t field_94C;
	uint32_t field_950;
	uint32_t window_width;
	uint32_t window_height;
	uint32_t colordepth;
	uint32_t field_960;
	uint32_t is_fullscreen;
	uint32_t field_968;
	uint32_t no_hardware;
	uint32_t field_970;
	uint32_t show_cursor;
	uint32_t colorkey;
	uint32_t field_97C;
	uint32_t field_980;
	uint32_t d3d2_flag;
	uint32_t field_988;
	uint32_t field_98C;
	uint32_t field_990;
	uint32_t field_994;
	uint32_t matrix_stack_size;
	uint32_t field_99C;
	uint32_t field_9A0;
	uint32_t field_9A4;
	uint32_t field_9A8;
	uint32_t field_9AC;
	uint32_t random_seed;
	char *window_title;
	char *window_class;
	uint32_t use_custom_wndclass;
	WNDCLASSA wndclass_struct;
	uint32_t use_custom_dwStyle;
	DWORD dwStyle;
	struct main_obj engine_loop_obj;
	struct main_obj game_loop_obj;
	void *wm_activate;
	uint32_t field_A2C;
	uint32_t field_A30;
	uint32_t field_A34;
	uint32_t field_A38;
	uint32_t field_A3C;
	uint32_t field_A40;
	uint32_t field_A44;
	uint32_t field_A48;
	uint32_t field_A4C;
	uint32_t field_A50;
	uint32_t field_A54;
	uint32_t field_A58;
	uint32_t field_A5C;
	uint32_t current_gfx_driver;
	uint32_t field_A64;
	uint32_t field_A68;
	uint32_t field_A6C;
	uint32_t field_A70;
	uint32_t field_A74;
	uint32_t field_A78;
	void *gfx_driver_data;
	uint32_t field_A80;
	uint32_t field_A84;
	void *create_gfx_driver;
	struct dll_gfx_externals *externals;
	uint32_t nvidia_fix;
	uint32_t tnt_fix;
	uint32_t no_riva_fix;
	uint32_t field_A9C;
};

struct ff7_gamepad_status
{
	uint32_t pos_x;
	uint32_t pos_y;
	uint32_t dpad_up;
	uint32_t dpad_down;
	uint32_t dpad_left;
	uint32_t dpad_right;
	uint32_t button1;
	uint32_t button2;
	uint32_t button3;
	uint32_t button4;
	uint32_t button5;
	uint32_t button6;
	uint32_t button7;
	uint32_t button8;
	uint32_t button9;
	uint32_t button10;
	uint32_t button11;
	uint32_t button12;
	uint32_t button13;
	uint32_t button14;
	uint32_t button15;
	uint32_t button16;
};

struct ff7_gfx_driver
{
	gfx_init *init;
	gfx_cleanup *cleanup;
	gfx_lock *lock;
	gfx_unlock *unlock;
	gfx_flip *flip;
	gfx_clear *clear;
	gfx_clear_all *clear_all;
	gfx_setviewport *setviewport;
	gfx_setbg *setbg;
	uint32_t field_24;
	struct bgra_color field_28;			// ?
	uint32_t field_38;
	uint32_t field_3C;
	gfx_prepare_polygon_set *prepare_polygon_set;
	gfx_load_group *load_group;
	gfx_setmatrix *setmatrix;
	gfx_unload_texture *unload_texture;
	gfx_load_texture *load_texture;
	gfx_palette_changed *palette_changed;
	gfx_write_palette *write_palette;
	gfx_blendmode *blendmode;
	gfx_light_polygon_set *light_polygon_set;
	gfx_field_64 *field_64;
	gfx_setrenderstate *setrenderstate;
	gfx_setrenderstate *_setrenderstate;
	gfx_setrenderstate *__setrenderstate;
	gfx_field_74 *field_74;
	gfx_field_78 *field_78;
	gfx_draw_deferred *draw_deferred;
	gfx_field_80 *field_80;
	gfx_field_84 *field_84;
	gfx_begin_scene *begin_scene;
	gfx_end_scene *end_scene;
	gfx_field_90 *field_90;
	gfx_polysetrenderstate *setrenderstate_flat2D;
	gfx_polysetrenderstate *setrenderstate_smooth2D;
	gfx_polysetrenderstate *setrenderstate_textured2D;
	gfx_polysetrenderstate *setrenderstate_paletted2D;
	gfx_polysetrenderstate *_setrenderstate_paletted2D;
	gfx_draw_vertices *draw_flat2D;
	gfx_draw_vertices *draw_smooth2D;
	gfx_draw_vertices *draw_textured2D;
	gfx_draw_vertices *draw_paletted2D;
	gfx_polysetrenderstate *setrenderstate_flat3D;
	gfx_polysetrenderstate *setrenderstate_smooth3D;
	gfx_polysetrenderstate *setrenderstate_textured3D;
	gfx_polysetrenderstate *setrenderstate_paletted3D;
	gfx_polysetrenderstate *_setrenderstate_paletted3D;
	gfx_draw_vertices *draw_flat3D;
	gfx_draw_vertices *draw_smooth3D;
	gfx_draw_vertices *draw_textured3D;
	gfx_draw_vertices *draw_paletted3D;
	gfx_polysetrenderstate *setrenderstate_flatlines;
	gfx_polysetrenderstate *setrenderstate_smoothlines;
	gfx_draw_vertices *draw_flatlines;
	gfx_draw_vertices *draw_smoothlines;
	gfx_field_EC *field_EC;
};

struct ff7_field_sfx_state {
	uint32_t u1;
	uint32_t volume1;
	uint32_t volume2;
	uint32_t u2;
	uint32_t u3;
	uint32_t u4;
	uint32_t pan1;
	uint32_t pan2;
	uint32_t u5;
	uint32_t u6;
	uint32_t u7;
	uint32_t u8;
	uint32_t u9;
	uint32_t u10;
	uint32_t u11;
	uint32_t frequency;
	uint32_t sound_id;
	IDirectSoundBuffer* buffer1;
	IDirectSoundBuffer* buffer2;
	uint32_t is_looped;
	uint32_t u12;
};

struct ff7_camdata
{
	// EYE
	int16_t eye_x;
	int16_t eye_y;
	int16_t eye_z;
	// TARGET
	int16_t target_x;
	int16_t target_y;
	int16_t target_z;
	// UP
	int16_t up_x;
	int16_t up_y;
	int16_t up_z;
	// FILLER?
	int16_t padding;
	// POSITION
	int32_t pos_x;
	int32_t pos_y;
	int32_t pos_z;
	// PAN
	int16_t pan_x;
	int16_t pan_y;
	// ZOOM
	int16_t zoom;
	// FILLER?
	int16_t padding2;
};

// --------------- end of FF7 imports ---------------

// memory addresses and function pointers from FF7.exe
struct ff7_externals
{
	uint32_t chocobo_fix;
	uint32_t midi_fix;
	void *snowboard_fix;
	uint32_t cdcheck;
	uint32_t get_inserted_cd_sub;
	uint32_t requiredCD;
	struct movie_obj *movie_object;
	void (*movie_sub_415231)(char *);
	void (*sub_665D9A)(struct matrix *, struct nvertex *, struct indexed_primitive *, struct p_hundred *, struct struc_186 *, struct ff7_game_obj *);
	void (*sub_671742)(uint32_t, struct p_hundred *, struct struc_186 *);
	void (*sub_6B27A9)(struct matrix *, struct indexed_primitive *, struct ff7_polygon_set *, struct p_hundred *, struct p_group *, void *, struct ff7_game_obj *);
	void (*sub_68D2B8)(uint32_t, struct ff7_polygon_set *, void *);
	void (*sub_665793)(struct matrix *, uint32_t, struct indexed_primitive *, struct ff7_polygon_set *, struct p_hundred *, struct p_group *, struct ff7_game_obj *);
	void (*matrix3x4)(struct matrix *);
	uint32_t matrix4x3_multiply;
	void *(*sub_6A2865)(void *);
	uint32_t sub_6B26C0;
	uint32_t sub_6B2720;
	uint32_t sub_673F5C;
	struct savemap *savemap;
	struct menu_objects *menu_objects;
	uint32_t magic_thread_start;
	void (*destroy_magic_effects)();
	uint32_t lgp_open_file;
	uint32_t lgp_read_file;
	uint32_t lgp_read;
	uint32_t lgp_get_filesize;
	uint32_t lgp_seek_file;
	void (*draw_character)(uint32_t, uint32_t, char *, uint32_t, float);
	uint32_t destroy_field_bk;
	uint32_t destroy_field_tiles;
	struct field_layer **field_layers;
	WORD *num_field_entities;
	struct field_object **field_objects;
	uint32_t open_field_file;
	WORD *field_id;
	char *field_file_name;
	uint32_t read_field_file;
	uint32_t battle_enter;
	uint32_t battle_loop;
	WORD *battle_id;
	DWORD *battle_mode;
	uint32_t battle_sub_429AC0;
	uint32_t battle_b3ddata_sub_428B12;
	uint32_t graphics_render_sub_68A638;
	uint32_t create_dx_sfx_something;
	uint32_t load_p_file;
	struct polygon_data *(*create_polygon_data)(uint32_t, uint32_t);
	void (*create_polygon_lists)(struct polygon_data *);
	void (*free_polygon_data)(struct polygon_data *);
	uint32_t battle_sub_42A0E7;
	uint32_t load_battle_stage;
	uint32_t load_battle_stage_pc;
	uint32_t read_battle_hrc;
	void (*battle_regular_chdir)(struct battle_chdir_struc *);
	void (*battle_context_chdir)(struct file_context *, struct battle_chdir_struc *);
	void (*swap_extension)(char *, char *, char *);
	void (*destroy_battle_hrc)(uint32_t, struct battle_hrc_header *);
	void (*battle_regular_olddir)(struct battle_chdir_struc *);
	void (*battle_context_olddir)(struct file_context *, struct battle_chdir_struc *);
	uint32_t load_animation;
	uint32_t field_load_animation;
	uint32_t field_load_models;
	uint32_t field_sub_60DCED;
	void (*destroy_animation)(struct anim_header *);
	uint32_t context_chdir;
	uint32_t lgp_chdir;
	struct lookup_table_entry **lgp_lookup_tables;
	struct lgp_toc_entry **lgp_tocs;
	struct lgp_folders *lgp_folders;
	uint32_t __read;
	uint32_t load_lgp;
	uint32_t open_lgp_file;
	FILE **lgp_fds;
	uint32_t battle_sub_437DB0;
	uint32_t sub_5CB2CC;
	uint32_t *midi_volume_control;
	uint32_t *midi_initialized;
	uint32_t menu_sub_6CDA83;
	uint32_t menu_sub_6CBD43;
	uint32_t menu_sub_701EE4;
	uint32_t phs_menu_sub;
	uint32_t menu_draw_party_member_stats;
	uint32_t *party_member_to_char_map;
	uint32_t menu_sub_6CB56A;
	uint32_t *menu_subs_call_table;
	uint32_t status_menu_sub;
	uint32_t draw_status_limit_level_stats;
	char *(*get_kernel_text)(uint32_t, uint32_t, uint32_t);
	uint32_t sub_5CF282;
	uint32_t get_equipment_stats;
	struct weapon_data *weapon_data_array;
	struct armor_data *armor_data_array;
	uint32_t field_sub_6388EE;
	uint32_t field_draw_everything;
	uint32_t field_pick_tiles_make_vertices;
	uint32_t field_layer2_pick_tiles;
	uint32_t *field_special_y_offset;
	uint32_t *field_layer2_tiles_num;
	uint32_t **field_layer2_palette_sort;
	struct field_tile **field_layer2_tiles;
	char *field_anim_state;
	void (*add_page_tile)(float, float, float, float, float, uint32_t, uint32_t);
	uint32_t field_load_textures;
	void (*field_convert_type2_layers)();
	void (*make_struc3)(uint32_t, struct struc_3 *);
	void (*make_field_tex_header_pal)(struct ff7_tex_header *);
	void (*make_field_tex_header)(struct ff7_tex_header *);
	struct ff7_graphics_object *(*_load_texture)(uint32_t, uint32_t, struct struc_3 *, char *, void *);
	uint32_t read_field_background_data;
	WORD *layer2_end_page;
	uint32_t create_d3d2_indexed_primitive;
	uint32_t destroy_d3d2_indexed_primitive;
	uint32_t enter_main;
	uint32_t kernel_init;
	uint32_t kernel_load_kernel2;
	uint32_t kernel2_reset_counters;
	uint32_t sub_4012DA;
	uint32_t kernel2_add_section;
	uint32_t kernel2_get_text;
	uint32_t draw_3d_model;
	void (*stack_push)(struct stack *);
	void *(*stack_top)(struct stack *);
	void (*stack_pop)(struct stack *);
	void (*_root_animation)(struct matrix *, struct anim_frame *, struct anim_header *, struct hrc_data *);
	void (*_frame_animation)(uint32_t, struct matrix *, struct point3d *, struct anim_frame *, struct anim_header *, struct hrc_bone *, struct hrc_data *);
	void (*root_animation)(struct matrix *, struct anim_frame *, struct anim_header *, struct hrc_data *);
	void (*frame_animation)(uint32_t, struct matrix *, struct point3d *, struct anim_frame *, struct anim_header *, struct hrc_bone *, struct hrc_data *);
	uint32_t *model_mode;
	uint32_t name_menu_sub_6CBD32;
	uint32_t name_menu_sub_719C08;
	uint32_t menu_sub_71894B;
	uint32_t menu_sub_718DBE;
	uint32_t menu_sub_719B81;
	uint32_t set_default_input_settings_save;
	uint32_t keyboard_name_input;
	uint32_t restore_input_settings;
	uint32_t dinput_getdata2;
	uint32_t init_stuff;
	uint32_t init_game;
	uint32_t sub_41A1B0;
	uint32_t init_directinput;
	uint32_t dinput_createdevice_mouse;
	void (*sub_69C69F)(struct matrix *, struct ff7_light *);
	uint32_t coaster_sub_5E9051;
	uint32_t coaster_sub_5EE150;
	uint32_t cleanup_game;
	uint32_t wm_activateapp;
	uint32_t get_gamepad;
	uint32_t update_gamepad_status;
	struct ff7_gamepad_status* gamepad_status;
	uint32_t music_is_locked;
	uint32_t field_initialize_variables;
	uint32_t music_lock_clear_fix;
	uint32_t sub_60DF96;
	uint32_t sub_60EEB2;
	uint32_t open_flevel_siz;
	uint32_t field_map_infos;
	uint32_t sound_operation;
	struct ff7_field_sfx_state* sound_states;
	uint32_t menu_sound_slider_loop;
	uint32_t call_menu_sound_slider_loop_sfx_up;
	uint32_t call_menu_sound_slider_loop_sfx_down;
	uint32_t menu_start;
	uint32_t battle_clear_sound_flags;
	uint32_t swirl_sound_effect;
	uint32_t field_init_event_sub_63BCA7;
	uint32_t field_init_event;
	uint32_t execute_opcode;
	uint32_t opcode_akao;
	uint32_t opcode_akao2;
	uint32_t opcode_cmusc;
	uint32_t field_music_helper;
	uint32_t field_music_helper_sound_op_call;
	uint32_t opcode_gameover;
	uint32_t opcode_message;
	uint32_t opcode_ask;
	uint32_t *sfx_initialized;
	uint32_t sfx_play_summon;
	uint32_t sfx_fill_buffer_from_audio_dat;
	uint32_t sfx_load_and_play_with_speed;
	uint32_t sfx_fmt_header;
	uint32_t battle_summon_leviathan_loop;
	uint32_t battle_limit_omnislash_loop;
	uint32_t sub_5F4A47;
	void (*reset_game_obj_sub_5F4971)(struct game_obj*);
	uint32_t engine_exit_game_mode_sub_666C78;
	void* (*sub_666C13)(struct game_obj*);
	void* (*sub_670F9B)(void*);
	WORD* word_CC0828;
	BYTE* byte_CC0D89;
	WORD* word_DB958A;
	uint32_t enter_gameover;
	uint32_t exit_gameover;
	void* (*start_gameover)();
	void* (*gameover_sub_6C12B1)();
	uint32_t on_gameover_enter;
	uint32_t on_gameover_exit;
	BYTE (*sub_60B260)();
	BYTE(*sub_767C55)();
	uint32_t field_battle_toggle;
	uint32_t worldmap_battle_toggle;
	uint32_t enter_field;
	uint32_t sub_63C17F;
	uint32_t sub_40B27B;
	WORD* word_CC0DD4;
	WORD* word_CC1638;
	uint32_t sub_630D50;
	WORD* opcode_message_loop_code;
	uint32_t sub_6310A1;
	WORD* opcode_ask_question_code;
};

uint32_t ff7gl_load_group(uint32_t group_num, struct matrix_set *matrix_set, struct p_hundred *hundred_data, struct p_group *group_data, struct polygon_data *polygon_data, struct ff7_polygon_set *polygon_set, struct ff7_game_obj *game_object);
void ff7gl_field_78(struct ff7_polygon_set *polygon_set, struct ff7_game_obj *game_object);
void ff7_init_hooks(struct game_obj *_game_object);
struct ff7_gfx_driver *ff7_load_driver(void *game_object);
