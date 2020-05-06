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
 * ff8.h - data structures and functions used internally by FF8
 */

#pragma once

#include "common.h"
#include "matrix.h"

// FF7 modules, unknowns are either unused or not relevant to rendering
enum ff8_game_modes
{
	FF8_MODE_0 = 0,
	FF8_MODE_1,
	FF8_MODE_WORLDMAP,
	FF8_MODE_SWIRL,
	FF8_MODE_4,
	FF8_MODE_5,
	FF8_MODE_MENU,
	FF8_MODE_7,
	FF8_MODE_CARDGAME,
	FF8_MODE_9,
	FF8_MODE_10,
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
	word unknown1;
	word unknown2;
	word unknown3;
	unsigned char u0;
	unsigned char v0;
	unsigned char u1;
	unsigned char v1;
	unsigned char u2;
	unsigned char v2;
};

struct ssigpu_packet52
{
	uint unknown1;
	unsigned char b0;
	unsigned char g0;
	unsigned char r0;
	unsigned char command;
	word y0;
	word x0;
	unsigned char v0;
	unsigned char u0;
	word clut;
	unsigned char b1;
	unsigned char g1;
	unsigned char r1;
	unsigned char unused1;
	word y1;
	word x1;
	unsigned char v1;
	unsigned char u1;
	word texture;
	unsigned char b2;
	unsigned char g2;
	unsigned char r2;
	unsigned char unused2;
	word y2;
	word x2;
	unsigned char v2;
	unsigned char u2;
	word unused3;
};

struct psxvram_rect
{
	word x;
	word y;
	word w;
	word h;
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
	uint dummy1[0x12];
	struct ff8_graphics_object *font_a;
	struct ff8_graphics_object *font_b;
};

struct struc_38
{
	uint field_0;
	uint field_4;
	uint field_8;
	uint field_C;
	uint field_10;
	uint field_14;
	uint field_18;
	uint field_1C;
	uint field_20;
	struct ff8_graphics_object *graphics_object;
};

struct ff8_file
{
	uint field_0;
	char *name;
	uint field_8;
};

struct ff8_indexed_vertices
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
	struct ff8_graphics_object *graphics_object;
};

struct ff8_graphics_object
{
	uint polytype;
	uint field_4;
	uint field_8;
	uint field_10;
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
	struct ff8_indexed_vertices *indexed_vertices;
	gfx_polysetrenderstate *func_90;
	gfx_draw_vertices *func_94;
	uint use_matrix_pointer;
	struct matrix *matrix_pointer;
	struct matrix matrix;
};

struct ff8_polygon_set
{
	uint field_0;
	uint field_4;
	uint field_8;
	uint field_C;
	uint field_10;
	uint numgroups;
	struct struc_49 field_14;
	uint field_2C;
	struct polygon_data *polygon_data;
	struct p_hundred *hundred_data;
	uint per_group_hundreds;
	struct p_hundred **hundred_data_group_array;
	struct matrix_set *matrix_set;
	uint field_44;
	uint field_48;
	void *execute_buffers;			// IDirect3DExecuteBuffer **
	struct indexed_primitive **indexed_primitives;
	uint field_54;
	uint field_58;
	struct p_group *polygon_group_array;
	// unknown up to 0xB0
};

struct ff8_tex_header
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
	uint field_D4;
	unsigned char *image_data;
	unsigned char *old_palette_data;
	uint field_DC;
	uint field_E0;
	uint field_E4;
	uint field_E8;
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
	uint dummy1[5];
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

struct texture_page
{
	uint field_0;
	uint field_4;
	uint field_8;
	uint width;
	uint height;
	uint field_14;
	uint field_18;
	uint field_1C;
	uint field_20;
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
	uint field_5C;
};

struct struc_50
{
	uint field_0;
	struct texture_page texture_page;
};

struct struc_51
{
	struct struc_50 struc_50_array[8];
	char dummy[300];
};

struct camdata
{
	uint field_0;
	uint field_4;
	uint field_8;
	uint field_C;
	uint field_10;
	uint field_14;
	uint field_18;
	uint field_1C;
	uint field_20;
	uint field_24;
	unsigned char field_28;
	unsigned char field_29;
	unsigned char field_2A;
	unsigned char field_2B;
};

struct ff8_movie_obj
{
	word movie_frame;
	word field_2;
	uint movie_surface_height;
	void *bink_struct;
	struct camdata *camdata_start;
	unsigned char camdata_buffer[312320];
	unsigned char movie_surface_desc[0x7C];
	void *movie_back_surface;
	uint movie_surface_x;
	struct camdata *camdata_pointer;
	uint movie_surface_y;
	void *movie_dd_surface;
	struct ff8_game_obj *movie_game_object;
	uint movie_intro_pak;
	uint movie_is_playing;
	uint field_4C4AC;
	uint field_4C4B0;
	uint field_4C4B4;
	uint field_4C4B8;
	uint movie_file_handle;
	uint bink_copy_flags;
};

struct ff8_game_obj
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
	uint dummy1[0xD];
	void *d3d2device;
	void *d3dviewport2;
	uint dummy2[0x6];
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
	uint dummy3[0xD];
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
	uint dummy4[0x30];
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
	uint dummy5[2];
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
	uint field_BD4;
	uint field_BD8;
	uint field_BDC;
	uint field_BE0;
	uint field_BE4;
	uint field_BE8;
	uint no_8bit_textures;
	uint field_BF0;
	uint tnt_fix;
	uint field_BF8;
	uint field_BFC;
	uint field_C00;
	uint field_C04;
	uint field_C08;
	uint field_C0C;
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
	uint field_24;
	struct bgra_color field_28;			// ?
	uint field_38;
	uint field_3C;
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
	uint movie_hack1;
	uint movie_hack2;
	uint swirl_sub_56D390;
	uint nvidia_hack1;
	uint nvidia_hack2;
	struct sprite_viewport *menu_viewport;
	uint main_loop;
	uint sub_47CF60;
	uint sub_47CCB0;
	uint sub_534640;
	uint sub_4972A0;
	uint load_fonts;
	uint swirl_main_loop;
	uint mode1_main_loop;
	uint sub_471F70;
	uint sub_4767B0;
	uint _load_texture;
	uint sub_4076B6;
	uint sub_41AC34;
	uint load_texture_data;
	uint sub_401ED0;
	uint pubintro_init;
	uint sub_467C00;
	uint sub_468810;
	uint sub_468BD0;
	uint pubintro_main_loop;
	uint credits_main_loop;
	uint sub_470520;
	uint sub_4A24B0;
	uint sub_470630;
	uint dd_d3d_start;
	uint create_d3d_gfx_driver;
	uint d3d_init;
	uint sub_40BFEB;
	uint tim2tex;
	uint sub_41BC76;
	uint d3d_load_texture;
	uint sub_559910;
	uint swirl_sub_56D1D0;
	uint load_credits_image;
	uint sub_52FE80;
	uint sub_45D610;
	uint sub_45D080;
	uint sub_464BD0;
	uint sub_4653B0;
	uint sub_559E40;
	uint sub_559F30;
	uint sub_497380;
	uint sub_4B3410;
	uint sub_4BE4D0;
	uint sub_4BECC0;
	uint menu_draw_text;
	uint (*get_character_width)(uint);
	struct ff8_graphics_object **swirl_texture1;
	uint sub_48D0A0;
	uint open_lzs_image;
	uint upload_psx_vram;
	void (*sub_464850)(uint, uint, uint, uint);
	word *psxvram_buffer;
	struct struc_51 *psx_texture_pages;
	uint read_field_data;
	uint upload_mim_file;
	char *field_filename;
	uint load_field_models;
	uint worldmap_enter_main;
	uint worldmap_sub_53F310;
	uint sub_545E20;
	uint sub_653410;
	uint wm_upload_psx_vram;
	uint check_active_window;
	uint sub_467D10;
	uint dinput_sub_468D80;
	uint dinput_sub_4692B0;
	uint pubintro_enter_main;
	uint draw_movie_frame;
	uint sub_529FF0;
	struct ff8_movie_obj *movie_object;
	uint sub_469640;
	uint sub_46DBF0;
	void (*sub_5304B0)();
	uint *enable_framelimiter;
	unsigned char *byte_1CE4907;
	unsigned char *byte_1CE4901;
	unsigned char *byte_1CE490D;
	uint sub_45B310;
	uint sub_45B460;
	uint ssigpu_init;
	uint *d3dcaps;
	uint sub_53BB90;
	uint sub_53C750;
	uint sub_544630;
	uint sub_548080;
	uint sub_549E80;
	uint sub_546100;
	uint sub_54A0D0;
	uint sub_54D7E0;
	uint sub_54FDA0;
	uint sub_53FAC0;
	uint sub_545EA0;
	uint sub_545F10;
	uint worldmap_main_loop;
	uint sub_465720;
	uint requiredDisk;
};

void ff8gl_field_78(struct ff8_polygon_set *polygon_set, struct ff8_game_obj *game_object);
void ff8_unload_texture(struct ff8_texture_set *texture_set);
struct ff8_gfx_driver *ff8_load_driver(struct ff8_game_obj *game_object);
void ff8_post_init();
