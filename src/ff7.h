/* 
 * FFNx - Complete OpenGL replacement of the Direct3D renderer used in 
 * the original ports of Final Fantasy VII and Final Fantasy VIII for the PC.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * ff7.h - data structures and functions used internally by FF7
 */

#pragma once

#include <ddraw.h>
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
	FF7_MODE_UNKNOWN22,
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
	uint use_assert_alloc;
	uint field_4;
	uint nodes;
	struct list_node *head;
	struct list_node *tail;
	void *destructor;
	void *recursive_find_cb;
	uint field_1C;
};

struct list_node
{
	struct list_node *next;
	void *object;
};

struct file_context
{
	uint mode;
	uint use_lgp;
	uint lgp_num;
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
	uint field_0;
	uint field_4;
	uint count;
	uint vertexcount;
	uint field_10;
	struct nvertex *vertices;
	uint indexcount;
	uint field_1C;
	word *indices;
	uint field_24;
	unsigned char *palettes;
	uint field_2C;
	struct ff7_graphics_object *graphics_object;
};

struct ff7_graphics_object
{
	uint polytype;
	uint field_4;
	uint field_8;
	struct p_hundred *hundred_data;
	struct matrix_set *matrix_set;
	struct polygon_set *polygon_set;
	uint field_18;
	uint field_1C;
	uint field_20;
	float u_offset;
	float v_offset;
	void *dx_sfx_2C;
	void *graphics_instance;
	uint field_34;
	uint vertices_per_shape;
	uint indices_per_shape;
	uint vertex_offset;
	uint index_offset;
	uint field_48;
	uint field_4C;
	uint field_50;
	uint field_54;
	uint field_58;
	uint field_5C;
	uint field_60;
	uint field_64;
	uint field_68;
	uint field_6C;
	uint field_70;
	uint field_74;
	uint field_78;
	uint field_7C;
	uint field_80;
	uint field_84;
	uint field_88;
	struct ff7_indexed_vertices *indexed_vertices;
	gfx_polysetrenderstate *func_90;
	gfx_draw_vertices *func_94;
	uint use_matrix_pointer;
	struct matrix *matrix_pointer;
	struct matrix matrix;
};

struct polygon_group
{
	uint field_0;
	uint numvert;
	void *driver_data;
	uint field_C;
	uint normindexes;
	uint vertices;
	uint vertex_colors;
	uint texcoords;
	uint texture_set;
};

struct struc_106
{
	uint field_0;
	uint color;
	struct point3d point;
	struct bgra_color d3dcol;
};

struct ff7_light
{
	uint flags;
	uint field_4;
	struct struc_106 *struc_106_1;
	struct struc_106 *struc_106_2;
	struct struc_106 *struc_106_3;
	struct bgra_color d3dcol4;
	struct bgra_color normd3dcol4;
	uint color4;
	struct matrix field_38;
	struct matrix field_78;
	struct matrix field_B8;
	struct matrix field_F8;
	uint field_138;
	struct matrix field_13C;
	uint field_17C;
	uint field_180;
	uint field_184;
	uint field_188;
	uint field_18C;
	uint field_190;
	uint field_194;
	uint field_198;
	struct matrix *matrix_pointer;
	uint field_1A0;
	uint field_1A4[256];
	uint field_5A4;
	uint color;
};

struct ff7_polygon_set
{
	uint field_0;
	uint field_4;
	uint field_8;
	uint field_C;
	uint numgroups;
	struct struc_49 field_14;
	uint field_2C;
	struct polygon_data *polygon_data;
	struct p_hundred *hundred_data;
	uint per_group_hundreds;
	struct p_hundred **hundred_data_group_array;
	struct matrix_set *matrix_set;
	struct ff7_light *light;
	uint field_48;
	void *execute_buffers;			// IDirect3DExecuteBuffer **
	struct indexed_primitive **indexed_primitives;
	uint field_54;
	uint field_58;
	struct polygon_group *polygon_group_array;
	uint field_60;
	uint field_64;
	uint field_68;
	uint field_6C;
	uint field_70;
	uint field_74;
	uint field_78;
	uint field_7C;
	uint field_80;
	uint field_84;
	uint field_88;
	uint field_8C;
	uint field_90;
	uint has_struc_173;
	uint field_98;
	struct struc_173 *struc_173;
	uint field_A0;
	uint field_A4;
	uint field_A8;
	uint field_AC;
};

struct ff7_tex_header
{
	uint version;
	uint field_4;
	uint color_key;
	uint field_C;
	uint field_10;
	union
	{
		struct
		{
			uint minbitspercolor;
			uint maxbitspercolor;
			uint minalphabits;
			uint maxalphabits;
		} v1_1;

		struct
		{
			uint x;
			uint y;
			uint w;
			uint h;
		} fb_tex;
	};
	union
	{
		struct
		{
			uint minbitsperpixel;
			uint maxbitsperpixel;
		} v1_2;

		struct
		{
			char *psx_name;
			char *pc_name;
		} file;
	};
	uint field_2C;
	uint palettes;					// ?
	uint palette_entries;			// ?
	uint bpp;
	struct texture_format tex_format;
	uint use_palette_colorkey;
	char *palette_colorkey;
	uint reference_alpha;
	uint blend_mode;
	uint field_CC;
	uint palette_index;
	unsigned char *image_data;
	unsigned char *old_palette_data;
	uint field_DC;
	uint field_E0;
	uint field_E4;
	uint field_E8;
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
			uint external;
			struct gl_texture_set *gl_set;
			uint width;
			uint height;
		} ogl;
	};

	uint field_10;
	uint field_14;
	uint refcount;
	uint field_1C;
	uint field_20;
	uint field_24;
	uint field_28;
	uint field_2C;
	uint field_30;
	uint field_34;
	uint field_38;
	uint field_3C;
	uint field_40;
	uint field_44;
	uint field_48;
	uint field_4C;
	uint field_50;
	uint field_54;
	uint field_58;
	uint field_5C;
	uint field_60;
	uint field_64;
	uint field_68;
	uint field_6C;
	uint field_70;
	uint field_74;
	uint field_78;
	uint *texturehandle;
	struct texture_format *texture_format;
	struct tex_header *tex_header;
	uint palette_index;
	struct palette *palette;
	uint field_90;
	uint field_94;
	uint field_98;
	uint field_9C;
};

struct field_layer
{
	struct ff7_tex_header *tex_header;
	void *image_data;
	struct ff7_graphics_object *graphics_object;
	uint present;
	uint field_10;
	uint field_14;
	word field_18;
	word type;
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
	word field_346;
	word field_348;
	word num_animations;
	char anim_filenames[8880];
	char field_25FC[592];
	char field_284C[60];
	uint field_2888;
};

struct struc_110
{
	uint field_0;
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
	uint sucess;
	char olddir[200];
};

struct battle_hrc_bone
{
	uint parent;
	float bone_length;
	uint num_rsd;
};

struct battle_hrc_header
{
	uint field_0;
	uint field_4;
	uint field_8;
	uint bones;
	uint field_10;
	uint field_14;
	uint num_textures;
	uint num_animations_1;
	uint animations_2_start_index;
	uint num_weapons;
	uint num_animations_2;
	uint field_2C;
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
			uint version;
		} version;

		struct
		{
			char *pc_name;
		} file;
	};
	uint num_frames;
	uint num_bones;
	char rotation_order[4];
	void *frame_data;
	struct anim_frame *anim_frames;
	uint use_matrix_array;
	struct matrix *matrix_array;
	struct matrix *current_matrix_array;
};

struct hrc_data
{
	uint field_0;
	uint field_4;
	uint debug;
	uint flags;
	uint num_bones;
	struct hrc_bone *bones;
	uint field_18;
	struct list *bone_list;
	struct ff7_game_obj *game_object;
	struct matrix field_24;
	struct matrix field_64;
	uint *field_A4;
};

struct hrc_bone
{
	char *bone_name;
	char *bone_parent;
	uint parent_index;
	float bone_length;
	uint num_rsd;
	char **rsd_names;
	struct rsd_array_member *rsd_array;
};

struct bone_list_member
{
	word bone_type;
	word bone_index;
};

struct rsd_array_member
{
	uint field_0;
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
	uint offset;
	word unknown1;
	word conflict;
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
	uint num_conflicts;
	struct conflict_entry *conflict_entries;
};

struct lgp_folders
{
	struct conflict_list conflicts[1000];
};

struct hpmp_bar
{
	word x;
	word y;
	word w;
	word h;
	word value1;
	word max_value;
	word healing_animation;
	word value2;
	uint color;
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
	word field_22;
	word field_24;
	word field_26;
	word field_28;
	word field_2A;
	word hp;
	word base_hp;
	word mp;
	word base_mp;
	uint field_34;
	word max_hp;
	word max_mp;
	uint current_exp;
	uint field_40;
	uint field_44;
	uint field_48;
	uint field_4C;
	uint field_50;
	uint field_54;
	uint field_58;
	uint field_5C;
	uint field_60;
	uint field_64;
	uint field_68;
	uint field_6C;
	uint field_70;
	uint field_74;
	uint field_78;
	uint field_7C;
	uint exp_to_next_level;
};

#pragma pack(push,1)

struct savemap
{
	uint checksum;
	char preview_level;
	char preview_portraits[3];
	char preview_char_name[16];
	word preview_hp;
	word preview_maxhp;
	word preview_mp;
	word preview_maxmp;
	uint preview_gil;
	uint preview_seconds;
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
	word items[320];
	uint materia[200];
	uint stolen_materia[48];
	uint field_B5C;
	uint field_B60;
	uint field_B64;
	uint field_B68;
	uint field_B6C;
	uint field_B70;
	uint field_B74;
	uint field_B78;
	uint gil;
	uint seconds;
	uint field_B84;
	uint field_B88;
	uint field_B8C;
	uint field_B90;
	word current_mode;
	word current_location;
	word field_B98;
	word x;
	word y;
	word z_walkmeshtri;
	char field_BA0;
	char field_BA1;
	char field_BA2;
	char field_BA3;
	char field_BA4[256];
	char field_CA4[256];
	char field_DA4[256];
	char field_EA4[256];
	char field_FA4[256];
	word phs_lock2;
	char field_10A6;
	char field_10A7;
	char field_10A8;
	char field_10A9;
	char field_10AA;
	char field_10AB;
	char field_10AC;
	word phs_lock;
	word phs_visi;
	char field_10B1;
	word field_10B2;
	uint field_10B4;
	uint field_10B8;
	uint field_10BC;
	uint field_10C0;
	uint field_10C4;
	uint field_10C8;
	uint field_10CC;
	uint field_10D0;
	uint field_10D4;
	char battle_speed;
	char battle_msg_speed;
	char field_10DA;
	char field_10DB;
	uint field_10DC;
	uint field_10E0;
	uint field_10E4;
	uint field_10E8;
	char message_speed;
	char field_10ED;
	word field_10EE;
	uint field_10F0;
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
	word field_C;
	word field_E;
	word field_10;
	word field_12;
	char stat_increase_types[4];
	char stat_increase_amounts[4];
	char field_1C[8];
	char field_24;
	char field_25;
	char field_26;
	char field_27;
	word field_28;
	word field_2A;
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
	word field_7;
	char field_9[8];
	char field_11;
	word field_12;
	word field_14;
	word field_16;
	uint field_18;
	uint field_1C;
	word field_20;
	word field_22;
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
	word field_8;
	word field_A;
	word field_C;
	word field_E;
	word hp;
	word max_hp;
	word mp;
	word max_mp;
	word field_18;
	word field_1A;
	word field_1C;
	word field_1E;
	char field_20;
	char field_21;
	char field_22;
	char field_23;
	char field_24[24];
	word field_3C;
	word field_3E;
	word field_40;
	word field_42;
	uint field_44;
	uint field_48;
	uint field_4C[24];
	char field_AC[8];
	uint field_B4[21];
	uint field_108[112];
	uint field_2C8[32];
	uint field_348[48];
	struct weapon_data weapon_data;
	uint field_434;
	uint field_438;
	uint field_43C;
};

#pragma pack(pop)

struct field_tile
{
	short x;
	short y;
	float z;
	word field_8;
	word field_A;
	word img_x;
	word img_y;
	float u;
	float v;
	word fx_img_x;
	word fx_img_y;
	uint field_1C;
	uint field_20;
	uint field_24;
	uint field_28;
	word tile_size_x;
	word tile_size_y;
	word palette_index;
	word flags;
	char anim_group;
	char anim_bitmask;
	word field_36;
	char field_38[4096];
	word field_1038;
	word field_103A;
	uint use_fx_page;
	uint field_1040;
	uint field_1044;
	uint field_1048;
	uint field_104C;
	char field_1050;
	char field_1051;
	char field_1052;
	char field_1053;
	word blend_mode;
	word page;
	word fx_page;
	word field_105A;
};

struct struc_3
{
	uint field_0;
	uint field_4;
	uint convert_animations;
	uint create_matrix_set;
	uint field_10;
	uint matrix_set_size;
	struct graphics_instance *graphics_instance;
	uint field_1C;
	uint blend_mode;
	uint base_directory;
	struct ff7_tex_header *tex_header;
	uint field_2C;
	uint light;
	uint field_34;
	float bone_scale_factor;
	uint field_3C;
	struct file_context file_context;
	uint field_50;
	uint field_54;
	uint field_58;
	uint palette_index;
	uint field_60;
	uint field_64;
	uint field_68;
	uint field_6C;
	uint field_70;
};

struct wordmatrix
{
	word _11;
	word _12;
	word _13;
	word _21;
	word _22;
	word _23;
	word _31;
	word _32;
	word _33;
	uint _41;
	uint _42;
	uint _43;
};

struct struc_154_2
{
	short field_0;
	word field_2;
	word field_4;
	word field_6;
	word field_8;
	word field_A;
	word field_C;
	word field_E;
	uint field_10;
	uint field_14;
	unsigned char field_18[8];
};

struct struc_154_3
{
	short field_0;
	word field_2;
	word field_4;
	word field_6;
	word field_8;
	word field_A;
	word field_C;
	word field_E;
	uint field_10;
	uint field_14;
	unsigned char field_18[8];
};

struct struc_154
{
	short field_0;
	word field_2;
	word field_4;
	word field_6;
	word field_8;
	word field_A;
	word field_C;
	word field_E;
	uint field_10;
	uint field_14;
	unsigned char field_18[8];
};

struct struc_205
{
	short field_0;
	word field_2;
	word field_4;
	word field_6;
	word field_8;
	word field_A;
	word field_C;
	word field_E;
	uint field_10;
	uint field_14;
	unsigned char field_18[16];
};

struct movie_obj
{
	void *ddstream;
	uint field_4;
	void *mediastream;
	uint loop;
	uint field_10;
	DDSURFACEDESC movie_sdesc;
	void *graphbuilder;
	uint movie_surfaceheight;
	uint field_88;
	void *amms;
	void *movie_surface;
	void *sample;
	uint movie_left;
	uint movie_top;
	uint movie_right;
	uint movie_bottom;
	uint target_left;
	uint target_top;
	uint target_right;
	uint target_bottom;
	void *sts1;
	void *vts1;
	void *sts2;
	void *vts2;
	void *st1;
	void *vt1;
	void *st2;
	void *vt2;
	uint vt1handle;
	uint vt2handle;
	uint field_E0;
	uint movie_surfacewidth;
	uint field_E8;
	struct nvertex movie_vt2prim[4];
	struct nvertex movie_vt1prim[4];
	void *mediaseeking;
	uint graphics_mode;
	uint field_1F4;
	uint field_1F8;
	uint is_playing;
	uint movie_end;
	uint global_movie_flag;
};

struct dll_gfx_externals
{
	void *(*assert_free)(void *, const char *, uint);
	void *(*assert_malloc)(uint, const char *, uint);
	void *(*assert_calloc)(uint, uint, const char *, uint);
	struct texture_format *(*create_texture_format)();
	void (*add_texture_format)(struct texture_format *, struct game_obj *);
	struct game_obj *(*get_game_object)();
	uint free_driver;
	uint create_gfx_driver;
	void (*make_pixelformat)(uint, uint, uint, uint, uint, struct texture_format *);
	uint gltexformat2texformat;
	uint sub_686143;
	uint sub_6861EC;
	uint sub_68631E;
	uint sub_686351;
	uint pal_pixel2bgra;
	uint pal_pixel2bgra_8bit;
	uint texture_set_destroy_pal;
	struct palette *(*create_palette_for_tex)(uint, struct tex_header *, struct texture_set *);
	uint convert_texture;
	uint texture_set_decref;
	struct texture_set *(*create_texture_set)();
	uint write_palette;
	uint rgba2d3dcol;
	uint sub_6A5FEB;
	uint sub_6A604A;
	uint destroy_palette;
	uint create_palette;
	uint call_gfx_write_palette;
	uint call_gfx_palette_changed;
	uint sub_6A5A70;
	uint sub_6A5BA0;
	uint sub_6A5C3B;
	uint sub_6A5CE2;
	void *(*sub_6A2865)(void *);
	gfx_load_group *generic_load_group;
	gfx_light_polygon_set *generic_light_polygon_set;
};

struct ff7_game_obj
{
	uint unknown_0;
	uint devicecaps_4;
	uint devicecaps_8;
	uint dcdepth;
	uint field_10;
	uint field_14;
	uint field_18;
	uint field_1C;
	uint field_20;
	uint field_24;
	uint field_28;
	uint field_2C;
	double countspersecond;
	time_t _countspersecond;
	uint field_40;
	uint field_44;
	double fps;
	uint tsc;
	uint field_54;					// tsc high bits?
	HINSTANCE hinstance;
	HWND hwnd;
	uint field_60;
	uint field_64;
	uint field_68;
	uint field_6C;
	uint field_70;
	void *dddevice;
	void *dd2interface;
	void *front_surface[3];
	DDSURFACEDESC front_surface_desc[3];
	uint field_1CC;
	uint field_1D0;
	IDirectDrawClipper* dd_clipper;
	uint field_1D8;
	DDSURFACEDESC d3d_surfacedesc;
	void *dd_interface;
	uint field_24C;
	DDSURFACEDESC dd_surfacedesc;
	struct list *d3ddev_list;
	void *d3dinterface;
	void *surface_d3ddev;			// IDirect3DDevice
	struct list *textureformat_list;
	void *d3ddev_struct;
	void *d3dviewport;
	void *d3dmaterial;
	uint field_2D8;
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
	uint field_304;
	uint field_308;
	uint field_30C;
	uint field_310;
	uint field_314;
	uint field_318;
	uint field_31C;
	uint field_320;
	D3DDEVICEDESC d3d_halcaps;
	D3DDEVICEDESC d3d_helcaps;
	DDCAPS_DX5 halcaps;
	DDCAPS_DX5 helcaps;
	uint field_794;
	uint field_798;
	struct texture_format surface_tex_format;
	uint in_scene;
	struct p_hundred *hundred_array[5];
	void *applog_debug1;
	uint applog_debug2;
	void *dxdbg_file;
	uint field_840;
	uint field_844;
	uint _res_x;
	uint _res_y;
	uint _res_w;
	uint _res_h;
	uint field_858;
	uint field_85C;
	uint field_860;
	uint field_864;
	uint field_868;
	uint field_86C;
	uint field_870;
	uint field_874;
	uint field_878;
	uint field_87C;
	uint field_880;
	uint field_884;
	uint field_888;
	uint field_88C;
	struct matrix matrix_890;
	struct matrix matrix_8D0;
	void *dx_sfx_something;
	struct list *tex_list_pointer;
	struct stack *stack_918;
	uint field_91C;
	void *_3d2d_something;
	uint field_924;
	uint field_928;
	uint field_92C;
	uint field_930;
	struct gfx_driver *gfx_driver;
	void *_3dobject_pool;
	uint field_93C;
	struct p_hundred *current_hundred;
	struct struc_81 *field_944;
	uint field_948;
	uint field_94C;
	uint field_950;
	uint res_w;
	uint res_h;
	uint colordepth;
	uint field_960;
	uint field_964;
	uint field_968;
	uint no_hardware;
	uint field_970;
	uint field_974;
	uint colorkey;
	uint field_97C;
	uint field_980;
	uint d3d2_flag;
	uint field_988;
	uint field_98C;
	uint field_990;
	uint field_994;
	uint matrix_stack_size;
	uint field_99C;
	uint field_9A0;
	uint field_9A4;
	uint field_9A8;
	uint field_9AC;
	uint random_seed;
	char *window_title;
	char *window_class;
	uint field_9BC;
	WNDCLASSA wndclass_struct;
	uint field_9E8;
	uint field_9EC;
	struct main_obj main_obj_9F0;
	struct main_obj main_obj_A0C;
	void *wm_activate;
	uint field_A2C;
	uint field_A30;
	uint field_A34;
	uint field_A38;
	uint field_A3C;
	uint field_A40;
	uint field_A44;
	uint field_A48;
	uint field_A4C;
	uint field_A50;
	uint field_A54;
	uint field_A58;
	uint field_A5C;
	uint current_gfx_driver;
	uint field_A64;
	uint field_A68;
	uint field_A6C;
	uint field_A70;
	uint field_A74;
	uint field_A78;
	void *gfx_driver_data;
	uint field_A80;
	uint field_A84;
	void *create_gfx_driver;
	struct dll_gfx_externals *externals;
	uint nvidia_fix;
	uint tnt_fix;
	uint no_riva_fix;
	uint field_A9C;
};

struct ff7_gamepad_status
{
	uint pos_x;
	uint pos_y;
	uint field_30;
	uint field_34;
	uint field_38;
	uint field_3C;
	uint button1;
	uint button2;
	uint button3;
	uint button4;
	uint button5;
	uint button6;
	uint button7;
	uint button8;
	uint button9;
	uint button10;
	uint button11;
	uint button12;
	uint button13;
	uint button14;
	uint button15;
	uint button16;
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
	uint field_24;
	struct bgra_color field_28;			// ?
	uint field_38;
	uint field_3C;
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

// --------------- end of FF7 imports ---------------

// memory addresses and function pointers from FF7.exe
struct ff7_externals
{
	uint chocobo_fix;
	uint midi_fix;
	void *snowboard_fix;
	uint cdcheck;
	uint get_inserted_cd_sub;
	uint requiredCD;
	struct movie_obj *movie_object;
	void (*movie_sub_415231)(char *);
	void (*sub_665D9A)(struct matrix *, struct nvertex *, struct indexed_primitive *, struct p_hundred *, struct struc_186 *, struct ff7_game_obj *);
	void (*sub_671742)(uint, struct p_hundred *, struct struc_186 *);
	void (*sub_6B27A9)(struct matrix *, struct indexed_primitive *, struct ff7_polygon_set *, struct p_hundred *, struct p_group *, void *, struct ff7_game_obj *);
	void (*sub_68D2B8)(uint, struct ff7_polygon_set *, void *);
	void (*sub_665793)(struct matrix *, uint, struct indexed_primitive *, struct ff7_polygon_set *, struct p_hundred *, struct p_group *, struct ff7_game_obj *);
	void (*matrix3x4)(struct matrix *);
	uint matrix4x3_multiply;
	void *(*sub_6A2865)(void *);
	uint sub_6B26C0;
	uint sub_6B2720;
	uint sub_673F5C;
	struct savemap *savemap;
	struct menu_objects *menu_objects;
	uint magic_thread_start;
	void (*destroy_magic_effects)();
	uint lgp_open_file;
	uint lgp_read_file;
	uint lgp_read;
	uint lgp_get_filesize;
	uint lgp_seek_file;
	void (*draw_character)(uint, uint, char *, uint, float);
	uint destroy_field_bk;
	uint destroy_field_tiles;
	struct field_layer **field_layers;
	word *num_field_entities;
	struct field_object **field_objects;
	uint open_field_file;
	char *field_file_name;
	uint read_field_file;
	uint battle_loop;
	uint battle_sub_429AC0;
	uint battle_b3ddata_sub_428B12;
	uint graphics_render_sub_68A638;
	uint create_dx_sfx_something;
	uint load_p_file;
	struct polygon_data *(*create_polygon_data)(uint, uint);
	void (*create_polygon_lists)(struct polygon_data *);
	void (*free_polygon_data)(struct polygon_data *);
	uint battle_sub_42A0E7;
	uint load_battle_stage;
	uint load_battle_stage_pc;
	uint read_battle_hrc;
	void (*battle_regular_chdir)(struct battle_chdir_struc *);
	void (*battle_context_chdir)(struct file_context *, struct battle_chdir_struc *);
	void (*swap_extension)(char *, char *, char *);
	void (*destroy_battle_hrc)(uint, struct battle_hrc_header *);
	void (*battle_regular_olddir)(struct battle_chdir_struc *);
	void (*battle_context_olddir)(struct file_context *, struct battle_chdir_struc *);
	uint load_animation;
	uint field_load_animation;
	uint field_load_models;
	uint field_sub_60DCED;
	void (*destroy_animation)(struct anim_header *);
	uint context_chdir;
	uint lgp_chdir;
	struct lookup_table_entry **lgp_lookup_tables;
	struct lgp_toc_entry **lgp_tocs;
	struct lgp_folders *lgp_folders;
	uint __read;
	uint load_lgp;
	uint open_lgp_file;
	FILE **lgp_fds;
	uint battle_sub_437DB0;
	uint sub_5CB2CC;
	uint *midi_volume_control;
	uint *midi_initialized;
	uint menu_sub_6CDA83;
	uint menu_sub_6CBD43;
	uint menu_sub_701EE4;
	uint phs_menu_sub;
	uint menu_draw_party_member_stats;
	uint *party_member_to_char_map;
	uint menu_sub_6CB56A;
	uint *menu_subs_call_table;
	uint status_menu_sub;
	uint draw_status_limit_level_stats;
	char *(*get_kernel_text)(uint, uint, uint);
	uint sub_5CF282;
	uint get_equipment_stats;
	struct weapon_data *weapon_data_array;
	struct armor_data *armor_data_array;
	uint field_sub_6388EE;
	uint field_draw_everything;
	uint field_pick_tiles_make_vertices;
	uint field_layer2_pick_tiles;
	uint *field_special_y_offset;
	uint *field_layer2_tiles_num;
	uint **field_layer2_palette_sort;
	struct field_tile **field_layer2_tiles;
	char *field_anim_state;
	void (*add_page_tile)(float, float, float, float, float, uint, uint);
	uint field_load_textures;
	void (*field_convert_type2_layers)();
	void (*make_struc3)(uint, struct struc_3 *);
	void (*make_field_tex_header_pal)(struct ff7_tex_header *);
	void (*make_field_tex_header)(struct ff7_tex_header *);
	struct ff7_graphics_object *(*_load_texture)(uint, uint, struct struc_3 *, char *, void *);
	uint read_field_background_data;
	word *layer2_end_page;
	uint create_d3d2_indexed_primitive;
	uint destroy_d3d2_indexed_primitive;
	uint enter_main;
	uint kernel_init;
	uint kernel_load_kernel2;
	uint kernel2_reset_counters;
	uint sub_4012DA;
	uint kernel2_add_section;
	uint kernel2_get_text;
	uint draw_3d_model;
	void (*stack_push)(struct stack *);
	void *(*stack_top)(struct stack *);
	void (*stack_pop)(struct stack *);
	void (*_root_animation)(struct matrix *, struct anim_frame *, struct anim_header *, struct hrc_data *);
	void (*_frame_animation)(uint, struct matrix *, struct point3d *, struct anim_frame *, struct anim_header *, struct hrc_bone *, struct hrc_data *);
	void (*root_animation)(struct matrix *, struct anim_frame *, struct anim_header *, struct hrc_data *);
	void (*frame_animation)(uint, struct matrix *, struct point3d *, struct anim_frame *, struct anim_header *, struct hrc_bone *, struct hrc_data *);
	uint *model_mode;
	uint name_menu_sub_6CBD32;
	uint name_menu_sub_719C08;
	uint menu_sub_71894B;
	uint menu_sub_718DBE;
	uint menu_sub_719B81;
	uint set_default_input_settings_save;
	uint keyboard_name_input;
	uint restore_input_settings;
	uint dinput_getdata2;
	uint dinput_getstate2;
	uint init_stuff;
	uint init_game;
	uint sub_41A1B0;
	uint init_directinput;
	uint dinput_createdevice_mouse;
	uint dinput_acquire_keyboard;
	void (*sub_69C69F)(struct matrix *, struct ff7_light *);
	uint coaster_sub_5E9051;
	uint coaster_sub_5EE150;
	uint cleanup_game;
	uint cleanup_midi;
	uint wm_activateapp;
	uint get_gamepad;
	uint update_gamepad_status;
	struct ff7_gamepad_status* gamepad_status;
};

uint ff7gl_load_group(uint group_num, struct matrix_set *matrix_set, struct p_hundred *hundred_data, struct p_group *group_data, struct polygon_data *polygon_data, struct ff7_polygon_set *polygon_set, struct ff7_game_obj *game_object);
void ff7gl_field_78(struct ff7_polygon_set *polygon_set, struct ff7_game_obj *game_object);
struct ff7_gfx_driver *ff7_load_driver(struct ff7_game_obj *game_object);
void ff7_post_init();
