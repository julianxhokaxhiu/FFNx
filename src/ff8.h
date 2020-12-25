/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 Julian Xhokaxhiu                                   //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2020 Marcin Gomulak                                     //
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

#include "common.h"
#include "matrix.h"

// FF7 modules, unknowns are either unused or not relevant to rendering
enum ff8_game_modes
{
	FF8_MODE_CREDITS = 0,
	FF8_MODE_FIELD,
	FF8_MODE_WORLDMAP,
	FF8_MODE_SWIRL,
	FF8_MODE_AFTER_BATTLE,
	FF8_MODE_5,
	FF8_MODE_MENU,
	FF8_MODE_7,
	FF8_MODE_CARDGAME,
	FF8_MODE_9,
	FF8_MODE_TUTO,
	FF8_MODE_11,
	FF8_MODE_INTRO,
	FF8_MODE_100 = 100,
	FF8_MODE_BATTLE = 999
};

/*
 * This section defines some structures used internally by the FF8 game engine.
 *
 * Documentation for some of them can be found on the Qhimm wiki, a lot of
 * information can be gleaned from the source code to this program but in many
 * cases nothing is known except the size and general layout of the structure.
 *
 * Variable and structure names are mostly based on what they contain rather
 * than what they are for, a lot of names may be wrong, inappropriate or
 * downright misleading. Thread with caution!
 */

struct wm_polygon_source
{
	WORD unknown1;
	WORD unknown2;
	WORD unknown3;
	unsigned char u0;
	unsigned char v0;
	unsigned char u1;
	unsigned char v1;
	unsigned char u2;
	unsigned char v2;
};

struct ssigpu_packet52
{
	uint32_t unknown1;
	unsigned char b0;
	unsigned char g0;
	unsigned char r0;
	unsigned char command;
	WORD y0;
	WORD x0;
	unsigned char v0;
	unsigned char u0;
	WORD clut;
	unsigned char b1;
	unsigned char g1;
	unsigned char r1;
	unsigned char unused1;
	WORD y1;
	WORD x1;
	unsigned char v1;
	unsigned char u1;
	WORD texture;
	unsigned char b2;
	unsigned char g2;
	unsigned char r2;
	unsigned char unused2;
	WORD y2;
	WORD x2;
	unsigned char v2;
	unsigned char u2;
	WORD unused3;
};

struct psxvram_rect
{
	WORD x;
	WORD y;
	WORD w;
	WORD h;
};

struct sprite_viewport
{
	float width;
	float height;
	float field_8;
	float field_C;
	float scale_x;
	float scale_y;
	float offset_x;
	float offset_y;
};

struct font_object
{
	uint32_t dummy1[0x12];
	struct ff8_graphics_object *font_a;
	struct ff8_graphics_object *font_b;
};

struct struc_38
{
	uint32_t field_0;
	uint32_t field_4;
	uint32_t field_8;
	uint32_t field_C;
	uint32_t field_10;
	uint32_t field_14;
	uint32_t field_18;
	uint32_t field_1C;
	uint32_t field_20;
	struct ff8_graphics_object *graphics_object;
};

struct ff8_file_unk
{
	int field_0;
	char* filename;
	int field_8;
};

struct ff8_file_context
{
	int field_0;
	int field_4;
	char* field_8;
	void (*field_C)(char*, char*);
	struct ff8_file_unk* field_10;
};

struct ff8_file_fs
{
	int field_0;
	int field_4;
	int field_8;
	char* filename;
	int field_10;
	int field_14;
	int field_18;
	int field_1C;
	int field_20;
	int field_24;
};

struct ff8_file
{
	int field_0;
	char* filename;
	int field_8;
	struct ff8_file_context field_C;
	int field_20;
	int field_24;
	struct ff8_file_unk field_28;
	struct ff8_file_container* file_container;
};

struct ff8_file_container
{
	int field_0;
	int field_4;
	struct ff8_file* fs_disk_file_metadata;
	struct ff8_file_fs* fs_inside_file_metadata;
	void* file_data_index;
};

struct ff8_indexed_vertices
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
	struct ff8_graphics_object *graphics_object;
};

struct ff8_graphics_object
{
	uint32_t type;
	uint32_t field_4;
	uint32_t field_8;
	uint32_t has_texture_set;
	struct p_hundred *hundred_data;
	struct matrix_set *matrix_set;
	struct polygon_set *polygon_set;
	uint32_t is_tim_header;
	uint32_t field_20;
	uint32_t field_24;
	float u_offset;
	float v_offset;
	void *dx_sfx_2C;
	void *tex_info_filename_and_more;
	uint32_t field_38;
	uint32_t vertices_per_shape;
	uint32_t indices_per_shape;
	uint32_t vertex_offset;
	uint32_t index_offset;
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
	uint32_t field_8C;
	struct ff8_indexed_vertices *indexed_vertices;
	uint32_t field_94;
	uint32_t field_98;
	gfx_polysetrenderstate* setrenderstate;
	gfx_draw_vertices* draw;
	uint32_t use_matrix_pointer;
	struct matrix* matrix_pointer;
	struct matrix matrix;
};

struct ff8_polygon_set
{
	uint32_t field_0;
	uint32_t field_4;
	uint32_t field_8;
	uint32_t field_C;
	uint32_t field_10;
	uint32_t numgroups;
	struct struc_49 field_14;
	uint32_t field_2C;
	struct polygon_data *polygon_data;
	struct p_hundred *hundred_data;
	uint32_t per_group_hundreds;
	struct p_hundred **hundred_data_group_array;
	struct matrix_set *matrix_set;
	uint32_t field_44;
	uint32_t field_48;
	void *execute_buffers;			// IDirect3DExecuteBuffer **
	struct indexed_primitive **indexed_primitives;
	uint32_t field_54;
	uint32_t field_58;
	struct p_group *polygon_group_array;
	// unknown up to 0xB0
};

struct ff8_tex_header
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
	uint32_t field_D4;
	unsigned char *image_data;
	unsigned char *old_palette_data;
	uint32_t field_DC;
	uint32_t field_E0;
	uint32_t x;
	uint32_t y;
};

struct ff8_texture_set
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
	uint32_t dummy1[5];
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

struct texture_page
{
	uint32_t field_0;
	uint32_t x;
	uint32_t y;
	uint32_t width;
	uint32_t height;
	uint32_t color_key;
	uint32_t u;
	uint32_t v;
	uint32_t field_20;
	struct ff8_graphics_object *tri_gfxobj;
	struct ff8_graphics_object *quad_gfxobj;
	struct ff8_graphics_object *noblend_tri_gfxobj;
	struct ff8_graphics_object *noblend_quad_gfxobj;
	struct ff8_graphics_object *avg_tri_gfxobj;
	struct ff8_graphics_object *avg_quad_gfxobj;
	struct ff8_graphics_object *add_tri_gfxobj;
	struct ff8_graphics_object *add_quad_gfxobj;
	struct ff8_graphics_object *sub_tri_gfxobj;
	struct ff8_graphics_object *sub_quad_gfxobj;
	struct ff8_graphics_object *mode3_tri_gfxobj;
	struct ff8_graphics_object *mode3_quad_gfxobj;
	struct ff8_tex_header *tex_header;
	char *image_data;
	struct ff8_tex_header* sub_tex_header;
	char* sub_image_data;
};

struct struc_50
{
	uint32_t initialized;
	struct texture_page texture_page[8];
	uint32_t field_324;
	uint32_t field_328;
	uint32_t vram_needs_reload;
	uint32_t field_330;
	char dummy[256];
	uint32_t vram_x;
	uint32_t vram_y;
	uint32_t vram_width;
	uint32_t vram_height;
	uint32_t vram_palette_data;
	uint32_t field_448;
};

struct struc_51
{
	struct struc_50 struc_50_array[32];
};

struct camdata
{
	uint32_t field_0;
	uint32_t field_4;
	uint32_t field_8;
	uint32_t field_C;
	uint32_t field_10;
	uint32_t field_14;
	uint32_t field_18;
	uint32_t field_1C;
	uint32_t field_20;
	uint32_t field_24;
	unsigned char field_28;
	unsigned char field_29;
	unsigned char field_2A;
	unsigned char field_2B;
};

struct ff8_movie_obj
{
	WORD movie_frame;
	WORD field_2;
	uint32_t movie_surface_height;
	void *bink_struct;
	struct camdata *camdata_start;
	unsigned char camdata_buffer[312320];
	unsigned char movie_surface_desc[0x7C];
	void *movie_back_surface;
	uint32_t movie_surface_x;
	struct camdata *camdata_pointer;
	uint32_t movie_surface_y;
	void *movie_dd_surface;
	struct ff8_game_obj *movie_game_object;
	uint32_t movie_intro_pak;
	uint32_t movie_is_playing;
	uint32_t field_4C4AC;
	uint32_t field_4C4B0;
	uint32_t field_4C4B4;
	uint32_t field_4C4B8;
	uint32_t movie_file_handle;
	uint32_t bink_copy_flags;
};

struct ff8_game_obj
{
	uint32_t unknown_0;
	uint32_t dc_horzres;
	uint32_t dc_vertres;
	uint32_t dc_bitspixel;
	uint32_t field_10;
	uint32_t field_14;
	uint32_t field_18;
	uint32_t field_1C;
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
	uint32_t dummy1[0xD];
	void *d3d2device;
	void *d3dviewport2;
	uint32_t dummy2[0x6];
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
	uint32_t dummy3[0xD];
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
	uint32_t dummy4[0x30];
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
	uint32_t dummy5[2];
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
	uint32_t field_BD4;
	uint32_t field_BD8;
	uint32_t field_BDC;
	uint32_t field_BE0;
	uint32_t field_BE4;
	uint32_t field_BE8;
	uint32_t no_8bit_textures;
	uint32_t field_BF0;
	uint32_t tnt_fix;
	uint32_t field_BF8;
	uint32_t field_BFC;
	uint32_t field_C00;
	uint32_t field_C04;
	uint32_t field_C08;
	uint32_t field_C0C;
};

struct ff8_gfx_driver
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
	void *field_54;
	void *field_58;
	void *field_5C;
	void *field_60;
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
	void *field__84;
	void *field_88;
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

// --------------- end of FF8 imports ---------------

// memory addresses and function pointers from FF8.exe
struct ff8_externals
{
	struct font_object **fonts;
	uint32_t movie_hack1;
	uint32_t movie_hack2;
	uint32_t swirl_sub_56D390;
	uint32_t nvidia_hack1;
	uint32_t nvidia_hack2;
	struct sprite_viewport *menu_viewport;
	uint32_t main_loop;
	uint32_t sub_47CF60;
	uint32_t sub_47CCB0;
	uint32_t sub_534640;
	uint32_t sub_4972A0;
	uint32_t load_fonts;
	uint32_t swirl_main_loop;
	uint32_t field_main_loop;
	uint32_t sub_471F70;
	uint32_t sub_4767B0;
	uint32_t sub_4789A0;
	uint32_t sub_47CA90;
	int (*sub_52B3A0)();
	uint32_t battle_trigger_field;
	uint32_t battle_trigger_worldmap;
	uint32_t _load_texture;
	uint32_t sub_4076B6;
	uint32_t sub_41AC34;
	uint32_t load_texture_data;
	uint32_t sub_401ED0;
	uint32_t pubintro_init;
	uint32_t sub_467C00;
	uint32_t sub_468810;
	uint32_t sub_468BD0;
	uint32_t pubintro_main_loop;
	uint32_t credits_main_loop;
	uint32_t sub_52F300;
	DWORD* credits_loop_state;
	DWORD* credits_counter;
	uint32_t sub_470520;
	uint32_t sub_4A24B0;
	uint32_t sub_470630;
	uint32_t dd_d3d_start;
	uint32_t create_d3d_gfx_driver;
	uint32_t d3d_init;
	uint32_t sub_40BFEB;
	uint32_t tim2tex;
	uint32_t sub_41BC76;
	uint32_t d3d_load_texture;
	uint32_t sub_559910;
	uint32_t swirl_sub_56D1D0;
	uint32_t load_credits_image;
	uint32_t sub_52FE80;
	uint32_t sub_45D610;
	uint32_t sub_45D080;
	uint32_t sub_464BD0;
	uint32_t sub_4653B0;
	uint32_t sub_559E40;
	uint32_t sub_559F30;
	uint32_t sub_497380;
	uint32_t sub_4B3410;
	uint32_t sub_4BE4D0;
	uint32_t sub_4BECC0;
	uint32_t menu_draw_text;
	uint32_t (*get_character_width)(uint32_t);
	struct ff8_graphics_object **swirl_texture1;
	uint32_t sub_48D0A0;
	uint32_t open_lzs_image;
	uint32_t upload_psx_vram;
	void (*sub_464850)(uint32_t, uint32_t, uint32_t, uint32_t);
	WORD *psxvram_buffer;
	struct struc_51 *psx_texture_pages;
	uint32_t read_field_data;
	uint32_t upload_mim_file;
	char *field_filename;
	uint32_t load_field_models;
	uint32_t worldmap_main_loop;
	uint32_t worldmap_enter_main;
	uint32_t worldmap_sub_53F310;
	uint32_t worldmap_sub_53F310_loc_53F7EE;
	uint32_t wm_upload_psx_vram;
	uint32_t check_active_window;
	uint32_t sub_467D10;
	uint32_t dinput_sub_4692B0;
	LPDIRECTINPUTDEVICE8A dinput_gamepad_device;
	LPDIJOYSTATE2 dinput_gamepad_state;
	uint32_t dinput_init_gamepad;
	uint32_t pubintro_enter_main;
	uint32_t draw_movie_frame;
	uint32_t sub_529FF0;
	struct ff8_movie_obj *movie_object;
	uint32_t initialize_sound;
	void (*sub_5304B0)();
	uint32_t *enable_framelimiter;
	unsigned char *byte_1CE4907;
	unsigned char *byte_1CE4901;
	unsigned char *byte_1CE490D;
	uint32_t sub_45B310;
	uint32_t sub_45B460;
	uint32_t ssigpu_init;
	uint32_t *d3dcaps;
	uint32_t sub_53BB90;
	uint32_t sub_53E2A0;
	uint32_t sub_53E6B0;
	uint32_t sub_4023D0;
	uint32_t sub_53C750;
	uint32_t sub_544630;
	uint32_t sub_548080;
	uint32_t sub_549E80;
	uint32_t sub_546100;
	uint32_t sub_54A0D0;
	uint32_t sub_54D7E0;
	uint32_t sub_54FDA0;
	uint32_t sub_53FAC0;
	int (*sub_541C80)(int);
	uint32_t sub_54B460;
	uint32_t sub_558D70;
	uint32_t sub_545EA0;
	uint32_t sub_545F10;
	uint32_t sub_465720;
	uint32_t requiredDisk;
	uint32_t sm_battle_sound;
	uint32_t sdmusicplay;
	uint32_t(*sd_music_play)(uint32_t, char*, uint32_t);
	uint32_t sd_music_play_at;
	uint32_t* current_music_ids;
	uint32_t sub_46B800;
	uint32_t stop_music;
	uint32_t set_midi_volume;
	uint32_t sub_46C050;
	uint32_t sub_500900;
	uint32_t sub_501B60;
	uint32_t sub_46B3A0;
	uint32_t sub_46B3E0;
	uint32_t sub_4A6680;
	uint32_t sub_4A6660;
	uint32_t sub_4A3D20;
	uint32_t sub_4A3EE0;
	uint32_t sub_469C60;
	uint32_t sub_46DDC0;
	uint32_t start;
	uint32_t battle_main_loop;
	uint32_t is_window_active;
	uint32_t is_window_active_sub1;
	uint32_t is_window_active_sub2;
	void (*show_vram_window)();
	void (*refresh_vram_window)();
	char* music_path;
	uint32_t opcode_musicload;
	uint32_t opcode_crossmusic;
	uint32_t opcode_dualmusic;
	uint32_t opcode_choicemusic;
	uint32_t opcode_musicvoltrans;
	uint32_t opcode_musicvolfade;
	uint32_t opcode_musicskip;
	uint32_t opcode_musicvolsync;
	uint32_t opcode_getmusicoffset;
	uint32_t dmusic_segment_connect_to_dls;
	uint32_t choice_music;
	uint32_t load_midi_segment;
	uint32_t load_midi_segment_from_id;
	uint32_t play_midi_segments;
	uint32_t load_and_play_midi_segment;
	uint32_t stop_midi_segments;
	uint32_t sounds_cleanup;
	uint32_t volume_update;
	uint32_t volume_music_update;
	uint32_t music_load;
};

void ff8gl_field_78(struct ff8_polygon_set *polygon_set, struct ff8_game_obj *game_object);
void ff8_unload_texture(struct ff8_texture_set *texture_set);
void ff8_init_hooks(struct game_obj *_game_object);
struct ff8_gfx_driver *ff8_load_driver(void* game_object);
LPDIJOYSTATE2 ff8_update_gamepad_status();
bool ff8_skip_movies();
