#pragma once

#include "types.h"

/*
 * Render states supported by the graphics engine
 *
 * Not all of these were implemented in the original games, even fewer are 
 * actually required. Named after corresponding Direct3D states.
 */
enum effects
{
	V_WIREFRAME,			// 0x1
	V_TEXTURE,				// 0x2
	V_LINEARFILTER,			// 0x4
	V_PERSPECTIVE,			// 0x8
	V_TMAPBLEND,			// 0x10
	V_WRAP_U,				// 0x20
	V_WRAP_V,				// 0x40
	V_UNKNOWN80,			// 0x80
	V_COLORKEY,				// 0x100
	V_DITHER,				// 0x200
	V_ALPHABLEND,			// 0x400
	V_ALPHATEST,			// 0x800
	V_ANTIALIAS,			// 0x1000
	V_CULLFACE,				// 0x2000
	V_NOCULL,				// 0x4000
	V_DEPTHTEST,			// 0x8000
	V_DEPTHMASK,			// 0x10000
	V_SHADEMODE,			// 0x20000
	V_SPECULAR,				// 0x40000
	V_LIGHTSTATE,			// 0x80000
	V_FOG,					// 0x100000
	V_TEXADDR,				// 0x200000
	V_UNKNOWN400000,        // 0x400000
	V_UNKNOWN800000,        // 0x800000
	V_ALPHAFUNC,            // 0x1000000
	V_ALPHAREF,             // 0x2000000
};

// helper definitions for all the different functions which should be provided by the graphics driver
typedef uint (gfx_init)(struct game_obj *);
typedef void (gfx_cleanup)(struct game_obj *);
typedef uint (gfx_lock)(uint);
typedef uint (gfx_unlock)(uint);
typedef void (gfx_flip)(struct game_obj *);
typedef void (gfx_clear)(uint, uint, uint, struct game_obj *);
typedef void (gfx_clear_all)(struct game_obj *);
typedef void (gfx_setviewport)(uint, uint, uint, uint, struct game_obj *);
typedef void (gfx_setbg)(struct bgra_color *, struct game_obj *);
typedef uint (gfx_prepare_polygon_set)(struct polygon_set *);
typedef uint (gfx_load_group)(uint, struct matrix_set *, struct p_hundred *, struct p_group *, struct polygon_data *, struct polygon_set *, struct game_obj *);
typedef void (gfx_setmatrix)(uint, struct matrix *, struct matrix_set *, struct game_obj *);
typedef void (gfx_unload_texture)(struct texture_set *);
typedef struct texture_set *(gfx_load_texture)(struct texture_set *, struct tex_header *, struct texture_format *);
typedef uint (gfx_palette_changed)(uint, uint, uint, struct palette *, struct texture_set *);
typedef uint (gfx_write_palette)(uint, uint, void *, uint, struct palette *, struct texture_set *);
typedef struct blend_mode *(gfx_blendmode)(uint, struct game_obj *);
typedef void (gfx_light_polygon_set)(struct polygon_set *, void *);
typedef void (gfx_field_64)(uint, uint, struct game_obj *);
typedef void (gfx_setrenderstate)(struct p_hundred *, struct game_obj *);
typedef void (gfx_field_74)(uint, struct game_obj *);
typedef void (gfx_field_78)(struct polygon_set *, struct game_obj *);
typedef void (gfx_draw_deferred)(struct struc_77 *, struct game_obj *);
typedef void (gfx_field_80)(struct graphics_object *, struct game_obj *);
typedef void (gfx_field_84)(uint, struct game_obj *);
typedef uint (gfx_begin_scene)(uint, struct game_obj *);
typedef void (gfx_end_scene)(struct game_obj *);
typedef void (gfx_field_90)(uint);
typedef void (gfx_polysetrenderstate)(struct polygon_set *, struct indexed_vertices *, struct game_obj *);
typedef void (gfx_draw_vertices)(struct polygon_set *, struct indexed_vertices *, struct game_obj *);
typedef void (gfx_field_EC)(struct game_obj *);

/*
 * This section defines some structures used internally by both game engines.
 * 
 * Documentation for some of them can be found on the Qhimm wiki, a lot of
 * information can be gleaned from the source code to this program but in many
 * cases nothing is known except the size and general layout of the structure.
 * 
 * Variable and structure names are mostly based on what they contain rather
 * than what they are for, a lot of names may be wrong, inappropriate or
 * downright misleading. Thread with caution!
 */

struct struc_81
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
	uint vertexcolor;
	struct texture_set *texture_set;
};

struct struc_173
{
	unsigned char color_op;
	unsigned char field_1;
	unsigned char scroll_uv;
	unsigned char scroll_v;
	unsigned char change_palette;
	unsigned char setrenderstate;
	unsigned char add_offsets;
	unsigned char field_7;
	uint color;
	uint field_C;
	uint field_10;
	uint x_offset;
	uint y_offset;
	uint z_offset;
	uint z_offset2;
	uint u_offset;
	uint v_offset;
	uint palette_index;
	struct p_hundred *hundred_data;
	unsigned char field_34[16];
};

struct struc_77
{
	struct struc_77 *next;
	uint current_group;
	struct polygon_set *polygon_set;
	struct p_hundred *hundred_data;
	uint use_matrix;
	struct matrix matrix;
	uint use_matrix_pointer;
	struct matrix *matrix_pointer;
	struct struc_173 struc_173;
};

struct bgra_color
{
	float b;
	float g;
	float r;
	float a;
};

struct heap
{
	struct heap *next;
	uint size;					// ?
	struct heap *last;			// ?
	uint field_C;
	uint field_10;
	uint field_14;
	uint field_18;
	uint field_1C;
	uint field_20;
	uint field_24;
	uint field_28;
	uint field_2C;
	uint field_30;
	void *callback_data;
	void *callback;
};

struct graphics_instance
{
	uint frame_counter;
	struct heap *heap;
};

struct p_edge
{
	word vertex1;
	word vertex2;
};

struct texcoords
{
	float u;
	float v;
};

struct p_polygon
{
	word field_0;
	word vertex1;
	word vertex2;
	word vertex3;
	word normals[3];
	word edges[3];
	uint field_14;
};

struct p_group
{
	uint polytype;
	uint offpoly;
	uint numpoly;
	uint offvert;
	uint numvert;
	uint offedge;
	uint numedge;
	uint field_1C;
	uint field_20;
	uint field_24;
	uint field_28;
	uint offtex;
	uint textured;
	uint texid;
};

struct boundingbox
{
	uint field_0;
	float max_x;
	float max_y;
	float max_z;
	float min_x;
	float min_y;
	float min_z;
};

struct nvertex
{
	struct point3d _;

	union
	{
		struct
		{
			float w;
			union
			{
				uint color;
				struct
				{
					unsigned char b;
					unsigned char g;
					unsigned char r;
					unsigned char a;
				};
			};
			uint specular;
		} color;

		struct point3d normal;
	};

	float u;
	float v;
};

struct struc_186
{
	struct graphics_object *graphics_object;
	uint polytype;
	uint field_8;
	struct point3d vertices[4];
	struct texcoords texcoords[4];
	uint colors[4];
	float w[4];
	struct nvertex *nvertex_pointer;
	uint palette_index;
};

struct struc_84
{
	struct struc_84 *next;
	uint field_4;
	struct struc_186 *struc_186;
	struct matrix matrix;
	struct struc_173 struc_173;
};

struct struc_49
{
	uint field_0;
	uint field_4;
	uint field_8;
	uint frame_counter;
	struct struc_84 *struc_84;
	struct graphics_instance *graphics_instance;
};

struct indexed_primitive
{
	uint field_0;
	uint vertex_size;
	uint primitivetype;
	uint vertextype;
	struct nvertex *vertices;
	uint vertexcount;
	word *indices;
	uint indexcount;
	uint flags;
	uint field_24;
};

struct matrix_set
{
	uint field_0;
	uint size;
	uint field_8;
	uint field_C;
	struct matrix *matrix_array;
	struct matrix *matrix_world;
	struct matrix *matrix_view;
	struct matrix *matrix_projection;
	void *d3dmatrix_world;
	void *d3dmatrix_view;
	void *d3dmatrix_projection;
};

struct polygon_data
{
	uint version;
	uint field_4;
	uint vertextype;
	uint numverts;
	uint numnormals;
	uint field_14;
	uint numtexcoords;
	uint numvertcolors;
	uint numedges;
	uint numpolys;
	uint field_28;
	uint field_2C;
	uint numhundreds;
	uint numgroups;
	uint numboundingboxes;
	uint has_normindextable;
	struct point3d *vertdata;
	struct point3d *normaldata;
	struct point3d *field_48;
	struct texcoords *texcoorddata;
	uint *vertexcolordata;
	uint *polycolordata;
	struct p_edge *edgedata;
	struct p_polygon *polydata;
	char *pc_name;
	void *field_64;
	struct p_hundred *hundredsdata;
	struct p_group *groupdata;
	struct boundingbox *boundingboxdata;
	uint *normindextabledata;
	uint field_78;
	struct polygon_lists *lists;
};

struct p_hundred
{
	uint field_0;
	uint field_4;
	uint field_8;
	uint field_C;
	uint field_10;
	struct texture_set *texture_set;
	uint field_18;
	uint field_1C;
	uint field_20;
	uint shademode;
	uint lightstate_ambient;
	uint field_2C;
	void *lightstate_material_pointer;
	uint srcblend;
	uint destblend;
	uint field_3C;
	uint alpharef;
	uint blend_mode;
	uint zsort;
	uint field_4C;
	uint field_50;
	uint field_54;
	uint field_58;
	uint vertex_alpha;
	uint field_60;
};

struct texture_format
{
	uint width;
	uint height;
	uint bytesperrow;
	void *field_C;
	uint use_palette;
	uint bitsperindex;
	uint indexed_to_8bit;
	uint palette_size;				// ?
	uint palettes;					// ?
	uint *palette_data;
	uint bitsperpixel;
	uint bytesperpixel;
	uint red_bits;
	uint green_bits;
	uint blue_bits;
	uint alpha_bits;
	uint red_mask;
	uint green_mask;
	uint blue_mask;
	uint alpha_mask;
	uint red_shift;
	uint green_shift;
	uint blue_shift;
	uint alpha_shift;
	uint red_truecolor_bitdiff;		// 8 - red_bits
	uint green_truecolor_bitdiff;	// 8 - green_bits
	uint blue_truecolor_bitdiff;	// etc
	uint alpha_truecolor_bitdiff;
	uint red_max;
	uint green_max;
	uint blue_max;
	uint alpha_max;
};

struct struc_91
{
	uint field_0;
	uint x_offset;
	uint y_offset;
	uint width;
	uint height;
	uint xscale;
	uint yscale;
	uint color_key;
	uint width2;
	uint height2;
	uint pitch2;
	uint bytesperpixel2;
	void *image_data2;
	struct texture_format tex_format;
	void *image_data;
};

struct palette
{
	uint field_0;
	uint palette_size;
	uint bitsperpixel;
	uint field_C;
	uint palette_entries;
	uint field_14;
	uint field_18;
	uint field_1C;
	uint field_20;
	void *d3dcolorpal;
	void *rgbapal;
	void *palette_entry;
	uint ddpalette;
	uint field_34;
	uint field_38;
	uint field_3C;
};

struct blend_mode
{
	uint field_0;
	uint zsort;
	uint vertex_alpha;
	uint srcblendmode;
	uint srcblendcaps;
	uint destblendmode;
	uint destblendcaps;
	uint field_1C;
	uint field_20;
};

typedef struct
{
	char dummy[0xCC];
} D3DDEVICEDESC;

struct main_obj
{
	void *init;
	void *cleanup;
	void *enter_main;
	void *exit_main;
	void *main_loop;
	uint field_14;
	uint field_18;
};
