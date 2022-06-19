/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2022 Julian Xhokaxhiu                                   //
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
#include <array>
#include <span>

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

enum class cmd_id
{
	CMD_ATTACK = 0x01,
	CMD_MAGIC = 0x02,
	CMD_SUMMON = 0x03,
	CMD_ITEM = 0x04,
	CMD_STEAL = 0x05,
	CMD_SENSE = 0x06,
	CMD_COIN = 0x07,
	CMD_THROW = 0x08,
	CMD_MORPH = 0x09,
	CMD_DEATHBLOW = 0x0A,
	CMD_MANIPULATE = 0x0B,
	CMD_MIME = 0x0C,
	CMD_ENEMY_SKILL = 0x0D,
	CMD_MUG = 0x11,
	CMD_CHANGE = 0x12,
	CMD_DEFEND = 0x13,
	CMD_LIMIT = 0x14,
	CMD_W_MAGIC = 0x15,
	CMD_W_SUMMON = 0x16,
	CMD_W_ITEM = 0x17,
	CMD_SLASH_ALL = 0x18,
	CMD_DOUBLE_CUT = 0x19,
	CMD_FLASH = 0x1A,
	CMD_QUAD_CUT = 0x1B,
	CMD_ENEMY_ACTION = 0x20,
	CMD_POISONTICK = 0x23
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
	vector3<float> point;
	struct abgr_color d3dcol;
};

struct ff7_light
{
	uint32_t flags;
	uint32_t field_4;
	struct struc_106 *color_1;
	struct struc_106 *color_2;
	struct struc_106 *color_3;
	struct abgr_color global_light_color_abgr;
	struct abgr_color global_light_color_abgr_norm;
	uint32_t global_light_color_rgba;
	struct matrix field_38;
	struct matrix field_78;
	struct matrix field_B8;
	struct matrix field_F8;
	uint32_t field_138;
	struct matrix normal_matrix;
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
	vector3<float> position;
	vector3<float> rotation;
	vector3<float> scale;
	float scale_factor;
	struct matrix matrix;
	vector3<float> *bone_positions;
	struct matrix *bone_matrices;
};

struct battle_actor_data
{
	uint32_t index;
	uint32_t level;
	uint32_t formation_entry;
	uint32_t command_index;
	uint32_t action_index;
	uint32_t field_14;
	uint32_t allowed_targets;
	uint32_t field_1C;
	uint32_t command_animation;
	uint32_t attack_effect;
	uint32_t field_28;
	uint32_t field_2C;
	uint32_t self_mask_1;
	uint32_t self_mask_2;
	uint32_t mp_cost;
	uint32_t action_accuracy;
	uint32_t damage_calc;
	uint32_t action_element;
	uint32_t action_power;
	uint32_t attack_power;
	uint32_t action_target_mask;
	uint32_t field_54[131];
};

struct battle_actor_vars
{
	uint32_t statusMask;		   // 0x00
	uint32_t stateFlags;		   // 0x04
	byte index;					   // 0x08
	byte level;					   // 0x09
	byte unknown0;				   // 0x0A
	byte elementDamageMask;		   // 0x0B
	byte characterID;			   // 0x0C
	byte physAtk;				   // 0x0D
	byte magAtk;				   // 0x0E
	byte pEvade;				   // 0x0F
	byte idleAnimScript;		   // 0x10
	byte damageAnimID;			   // 0x11
	byte backDamageMult;		   // 0x12
	byte sizeScale;				   // 0x13
	byte dexterity;				   // 0x14
	byte luck;					   // 0x15
	byte idleAnimHolder;		   // 0x16
	byte lastCovered;			   // 0x17
	uint16_t lastTargets;		   // 0x18
	uint16_t prevAttackerMask;	   // 0x1A
	uint16_t prevPhysAttackerMask; // 0x1C
	uint16_t prevMagAttackerMask;  // 0x1E
	uint16_t defense;			   // 0x20
	uint16_t mDefense;
	uint16_t formationID;
	uint16_t absorbedElementsMask;
	uint16_t currentMP;
	uint16_t maxMP;
	int currentHP;
	int maxHP;
	uint32_t unknown1;
	uint32_t unknown2;
	uint32_t unknown3;
	uint32_t unknown4;
	uint32_t initalStatusMasks;
	uint32_t unknown5;
	byte unknown6;
	byte mEvade;
	byte actorRow;
	byte cameraData;
	uint16_t gilStolen;
	uint16_t itemStolen;
	uint16_t unknown7;
	byte missAnimScript;
	byte APValue;
	uint32_t gilValue;
	uint32_t expValue;
	byte unused8;
	byte unused9;
	uint16_t unused10; // This is being used for the tactical elements mod, unused in original game
	byte unused12;
	byte unused13;
	byte unused14;
	byte unused15;
};

struct battle_ai_context
{
	byte lastCommandIdx;  // 00
	byte lastActionIdx;	  // 08
	byte bankAccessValue; // 10
	byte dummyByte;		  // 18
	byte battleType;	  // 20
	byte colorMask;		  // 30
	byte limitLevel;	  // 38
	byte unk3;			  // 40
	byte unk4;			  // 48
	byte pad;
	uint16_t activeActorMask;	 // 50
	uint16_t scriptOwnerMask;	 // 60
	uint16_t actionTargetMask;	 // 70
	uint16_t actorAlliesMask;	 // 80
	uint16_t activeAlliesMask;	 // 90
	uint16_t actorEnemiesMask;	 // A0
	uint16_t activeEnemiesMask;	 // B0
	uint16_t actorPartyMask;	 // C0
	uint16_t enemyActorsMask;	 // D0
	uint16_t allActorsMask;		 // E0
	uint16_t unkMask2;			 // F0
	uint16_t unkMask3;			 // 0x100
	uint16_t endBattleFlags;	 // 2110
	uint16_t lastActionElements; // 2120
	uint16_t unkDword3;			 // 130
	uint16_t battleFormationIdx; // 140
	uint16_t lastAbsActionIdx;	 // 150
	uint16_t unkBattleFlags;	 // 160
	uint16_t specialAttackFlags; // 170
	uint16_t unkLimitDivisor;	 // 180
	uint16_t unkDword;			 // 190
	uint16_t stringArgs;
	uint16_t somethingEmerald; // 1A0
	uint32_t partyGil;		   // 1C0
	battle_actor_vars actor_vars[10];
};

struct battle_anim_event {
    byte attackerID;
    byte activeAllies;
    byte spellEffectID;
    byte commandIndex;
    byte actionFlags;
    byte animationScriptID;
    uint16_t actionIndex;
    uint16_t cameraData;
    uint16_t damageEventQueueIdx;
};

struct formation_camera{
	vector3<short> position;
	vector3<short> focal_point;
};

struct bcamera_position{
	vector3<short> point;
	WORD unused_6;
	short current_position;
	short frames_to_wait;
	byte field_C;
	byte field_D;
};

struct bcamera_fn_data{
	WORD field_0;
	WORD field_2;
	WORD n_frames;
	short field_6;
	short field_8;
	short field_A;
	short field_C;
	short field_E;
	short field_10;
	byte unused_12[6];
	byte field_18;
	byte unused_19[15];
};

struct battle_model_state
{
    uint16_t characterID;     // BE1178, 0
    uint16_t animScriptIndex; // BE117A, 2
    byte actionFlags;
    byte field_5;
    short field_6;
    uint16_t AnimationData; // BE1180, 8
    uint16_t animScriptPtr; // BE1182, 0xA
    uint16_t field_C;
    uint16_t runningAnimIdx; // 0xE
    uint16_t totalBones;
    uint16_t height;
    short field_14;
    uint16_t initialXRotation;
    uint16_t initialYRotation;
    uint16_t initialZRotation;
    uint16_t field_1C;
    uint16_t field_1E;
    uint16_t field_20;
    byte animationEffect; // BE119A, 0x22
    byte commandID;       // BE119B, 0x23
    byte field_24;
    byte field_25;
    byte actorIsNotActing;
    byte field_27;
    byte field_28;
    byte unkActorFlags;
    byte field_2A;
    byte bData0x12[16];         // 0x2B
    byte isScriptExecuting;     // BE11B3, 0x3B
    byte currentScriptPosition; // 0x3C
    byte waitFrames;            // 0x3D
    byte modelEffectFlags;      // 0x3E
    byte field_3F;
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
    byte padding3[0xE6];       // 0x78
	vector3<uint16_t> modelRotation; // BE12D8, 0x15E
    uint16_t field_164;
	vector3<short> modelPosition; // BE12DE, 0x166
    uint32_t field_16C;
    uint32_t *field_170;
    uint32_t field_174;
    byte padding5[0xA24];
    uint32_t playedAnimFrames;
    uint32_t currentPlayingFrame;
    uint32_t tableRelativeModelAnimIdx;
    uint32_t *modelDataPtr;
    byte padding4[0xF18];
    uint32_t setForLimitBreaks; // 0x1AC4
    uint32_t field_1AC8;
    float field_1ACC;
    float field_1AD0;
    float field_1AD4;
    uint32_t padding_1AD8;
    float field_1ADC;
    uint32_t padding_1AE0;
    uint32_t field_1AE4;
    uint32_t field_1AE8;
};

struct battle_model_state_small
{
    uint32_t field_0;
    uint16_t bData68[4];
    uint16_t field_C;
    uint16_t bData76[6];
    uint16_t bData88[6];
    uint16_t actorIsNotActing;
    uint16_t field_28;
    uint16_t field_2A;
    uint16_t someHPCopy;
    uint16_t field_2E;
    uint16_t someMPCopy;
    byte modelDataIndex; // 0x032
    byte field_33;
    byte innateStatusMask;
    byte field_35;
    byte field_36;
    byte field_37;
    uint16_t field_38;
    uint16_t field_3A;
    uint16_t field_3C;
    uint16_t actionIdx;
    byte field_40;
    byte field_41;
    byte field_42;
    byte field_43;
    byte field_44;
    byte field_45;
    byte field_46;
    byte field_47;
    byte field_48;
    byte field_49;
    byte field_4A;
    byte field_4B;
    byte field_4C;
    byte field_4D;
    byte field_4E;
    byte field_4F;
    byte field_50;
    byte field_51;
    byte field_52;
    byte field_53;
    byte field_54;
    byte field_55;
    byte field_56;
    byte field_57;
    byte field_58;
    byte field_59;
    byte field_5A;
    byte field_5B;
    byte field_5C;
    byte field_5D;
    byte field_5E;
    byte field_5F;
    byte field_60;
    byte field_61;
    byte field_62;
    byte field_63;
    byte field_64;
    byte field_65;
    byte field_66;
    byte field_67;
    byte field_68;
    byte field_69;
    byte field_6A;
    byte field_6B;
    byte field_6C;
    byte field_6D;
    byte field_6E;
    byte field_6F;
    byte field_70;
    byte field_71;
    byte field_72;
    byte field_73;
};

struct effect100_data
{
    uint16_t field_0;
    short field_2;
    short n_frames;
    short field_6;
    short field_8;
    short field_A;
    short field_C;
    short field_E;
    int field_10;
    int field_14;
    byte field_18;
    byte field_19;
	short field_1A;
    byte field_1C[4];
};

struct effect60_data
{
    uint16_t field_0;
    short field_2;
    short n_frames;
    short field_6;
    short field_8;
    short field_A;
    uint16_t padding;
    short field_E;
    int field_10;
    int field_14;
    byte field_18;
    byte field_19[7];
};

struct effect10_data
{
    uint16_t field_0;
    short field_2;
    short n_frames;
    short field_6;
    short field_8;
    short field_A;
    short field_C;
    short field_E;
    int field_10;
    int field_14;
    byte field_18;
    byte field_19;
    byte field_1A;
    byte field_1B[5];
};

struct material_anim_ctx
{
    uint32_t *materialRSD;
    uint32_t negateColumnFlags;
    WORD field_8;
    short transparency;
    short field_C;
    short paletteIdx;
};

struct palette_extra
{
    int x_offset;
    p_hundred *aux_gfx_ptr;
    int z_offset_2;
    int y_offset;
    int scroll_v;
    int v_offset;
    int z_offset;
    int field_1C;
    int field_20;
    int field_24;
    int field_28;
};

struct page_spt
{
    int field_0;
    short field_4;
    short field_6;
    short uScale;
    short vScale;
    short field_C;
    short palette_something;
    short field_10;
    short field_12;
};

struct tex_page_list
{
    WORD *field_0;
    page_spt *page_spt_ptr;
};

struct texture_spt
{
    int *spt_handle_copy;
    tex_page_list *pages;
    byte *spt_handle;
    int tex_page_count;
    uint32_t field_10[4];
    ff7_graphics_object *game_drawable[4];
};

struct texture_spt_anim_ctx
{
    texture_spt *effect_spt;
    ff7_graphics_object *effectDrawable;
    color_ui8 color;
    WORD field_C;
    WORD field_E;
};

#pragma pack(push, 1)
struct rotation_matrix
{
    short r3_sub_matrix[3][3];
    int position[3];
};
#pragma pack(pop)

struct battle_text_data
{
	short buffer_idx;
	short field_2;
	char wait_frames;
	char n_frames;
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
	vector3<float> root_rotation;
	vector3<float> root_translation;
};

struct anim_frame
{
	struct anim_frame_header *header;
	vector3<float> *data;
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
	WORD learned_limit_break;
	WORD num_kills;
	WORD used_n_limit_1_1;
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
	uint32_t equipped_materia[16];
	uint32_t exp_to_next_level;
};

struct chocobo_slot
{
	WORD sprint_speed;
	WORD max_sprint_speed;
	WORD speed;
	WORD max_speed;
	char acceleration;
	char cooperation;
	char intelligence;
	char personality;
	char p_count;
	char n_races_won;
	boolean is_female;
	char type;
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
	savemap_char chars[9];
	byte party_members[3];
	char padding_4FB;
	WORD items[320];
	uint32_t materia[200];
	uint32_t stolen_materia[48];
	char field_B5C[32];
	uint32_t gil;
	uint32_t seconds;
	uint32_t countdown_timer;
	char field_B88[12];
	WORD current_mode;
	WORD current_location;
	WORD field_B98;
	WORD x;
	WORD y;
	WORD z_walkmeshtri;
	char field_BA0[467];
	char yuffie_reg_mask;
	char field_D74[80];
	chocobo_slot chocobo_slots_first[4];
	char field_E04[240];
	char vincent_reg_mask;
	char field_EF5[399];
	chocobo_slot chocobo_slots_last[2];
	WORD phs_lock;
	WORD phs_visi;
	char field_10A8[48];
	char battle_speed;
	char battle_msg_speed;
	char config_bitmap_1;
	char config_bitmap_2;
	char controller_mapping[16];
	char message_speed;
	char field_10ED[7];
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
	uint32_t do_quit;
	uint32_t dc_horzres;
	uint32_t dc_vertres;
	uint32_t dc_bitspixel;
	uint32_t window_pos_x;
	uint32_t window_pos_y;
	uint32_t window_size_x;
	uint32_t window_size_y;
	uint32_t window_minimized;
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
	LPDIRECTDRAWSURFACE front_surface[3];
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
	uint32_t gfx_reset;
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
	uint32_t byte;
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
	vector3<short> eye;
	// TARGET
	vector3<short> target;
	// UP
	vector3<short> up;
	// FILLER?
	int16_t padding;
	// POSITION
	vector3<int> position;
	// PAN
	int16_t pan_x;
	int16_t pan_y;
	// ZOOM
	int16_t zoom;
	// FILLER?
	int16_t padding2;
};

struct ff7_shake_bg_data
{
	uint8_t do_shake;
	uint8_t shake_phase;
	char amp_index;
	char shake_curr_value;
	short shake_amplitude;
	short shake_initial;
	short shake_final;
	short shake_n_steps;
	short shake_idx;
};

struct ff7_modules_global_object
{
  uint8_t field_0;
  uint8_t game_mode;
  uint16_t battle_id;
  uint16_t field_model_pos_x;
  uint16_t field_model_pos_y;
  uint16_t field_8;
  uint16_t field_A;
  uint16_t field_C;
  uint16_t field_E;
  int16_t field_10;
  uint8_t field_12;
  uint8_t field_13;
  uint8_t field_14;
  uint8_t field_15;
  short field_16;
  uint16_t field_18;
  uint16_t field_1A;
  uint8_t field_1C;
  uint8_t field_1D;
  uint16_t field_1E;
  uint16_t field_20;
  uint16_t field_model_triangle_id;
  uint16_t field_model_anim_id;
  uint16_t previous_game_mode;
  uint16_t num_models;
  uint16_t field_model_id;
  uint16_t field_2C;
  uint16_t field_2E;
  uint16_t field_30;
  uint8_t field_32;
  uint8_t field_33;
  uint8_t field_34;
  uint8_t field_35;
  uint8_t field_36;
  uint8_t SCRLO_flag;
  uint8_t MPDSP_flag;
  uint8_t MVCAM_flag;
  uint8_t BGMOVIE_flag;
  uint8_t BTLON_flag;
  uint8_t field_3C;
  uint8_t field_3D;
  uint16_t field_3E;
  uint16_t field_40;
  uint16_t field_42;
  uint32_t midi_id;
  uint32_t field_48;
  uint16_t fade_type;
  uint16_t fade_adjustment;
  uint16_t fade_speed;
  short fade_r;
  short fade_g;
  short fade_b;
  uint16_t field_58;
  uint16_t field_5A;
  uint16_t field_5C;
  uint16_t nfade_r;
  uint16_t nfade_g;
  uint16_t nfade_b;
  uint16_t field_id;
  uint16_t field_66;
  uint32_t current_key_input_status;
  uint32_t previous_key_input_status;
  uint16_t field_70;
  uint16_t field_72;
  uint16_t field_74;
  uint16_t field_76;
  uint32_t field_78;
  uint16_t field_7C;
  uint32_t field_80;
  uint16_t field_84;
  uint16_t field_86;
  uint16_t MOVIE_frame;
  ff7_shake_bg_data shake_bg_x;
  ff7_shake_bg_data shake_bg_y;
  uint16_t bg2_scroll_speed_x;
  uint16_t bg2_scroll_speed_y;
  uint16_t bg3_scroll_speed_x;
  uint16_t bg3_scroll_speed_y;
  uint16_t field_AE;
  uint16_t field_B0;
  uint8_t field_B2[64];
  uint8_t background_sprite_layer[64];
  uint16_t field_132;
  uint32_t field_134;
};

struct ff7_field_script_header {
	WORD unknown1;			// Always 0x0502
	char nEntities;			// Number of entities
	char nModels;			// Number of models
	WORD wStringOffset;		// Offset to strings
	WORD nAkaoOffsets;		// Specifies the number of Akao/tuto blocks/offsets
	WORD scale;             // Scale of field. For move and talk calculation (9bit fixed point).
	WORD blank[3];
	char szCreator[8];      // Field creator (never shown)
	char szName[8];			// Field name (never shown)
};

struct field_event_data
{
	WORD field_0;
	WORD padding_2;
	DWORD field_4;
	byte field_8;
	byte padding_9;
	WORD field_A;
	vector3<int> model_pos;
	vector3<int> model_initial_pos;
	byte field_24[8];
	int field_2C;
	__int16 movement_ladder_jump_steps;
	__int16 movement_step_idx;
	byte padding_34;
	byte field_35;
	byte rotation_value;
	byte field_37;
	byte rotation_curr_value;
	byte rotation_n_steps;
	byte rotation_step_idx;
	byte rotation_steps_type;
	__int16 rotation_initial;
	__int16 rotation_final;
	int field_40;
	__int16 field_44;
	__int16 field_46;
	int field_48;
	__int16 field_4C;
	__int16 field_4E;
	int field_50;
	__int16 field_54;
	__int16 field_56;
	__int16 field_58;
	__int16 field_5A;
	char field_5C;
	byte padding_5D;
	byte field_5E;
	byte field_5F;
	byte field_60;
	byte field_61;
	byte field_62;
	char movement_type;
	char animation_id;
	byte padding_65;
	__int16 animation_speed;
	__int16 firstFrame;
	__int16 lastFrame;
	__int16 field_6C;
	__int16 field_direction_or_collision;
	__int16 movement_phase_maybe_70;
	__int16 collision_radius;
	__int16 talk_radius;
	WORD movement_speed;
	__int16 field_triangle_id;
	__int16 field_7A;
	vector3<int> model_final_pos;
};

struct field_animation_data
{
	int field_0;
	int actor_x;
	int actor_y;
	int actor_z;
	byte field_10[18];
	__int16 field_22;
	byte field_24[336];
	WORD field_174;
	WORD field_176;
	uint32_t *anim_frame_object;
	uint32_t *field_17C;
	byte field_180[16];
};

struct ff7_field_camera
{
	int16_t eye_x;
	int16_t eye_y;
	int16_t eye_z;
	int16_t target_x;
	int16_t target_y;
	int16_t target_z;
	int16_t up_x;
	int16_t up_y;
	int16_t up_z;
	int16_t pos_x;
	int16_t pan_x;
	int16_t pos_y;
	int16_t pan_y;
	int16_t pos_z;
	int16_t zoom;
};

struct field_gateway
{
	vector3<short> v1_exit_line;
	vector3<short> v2_exit_line;
	vector3<short> destination_vertex;
	SHORT field_id;
	byte unknown[4];
};

struct field_trigger
{
	vector3<short> v_corner1;
	vector3<short> v_corner2;
	byte bg_group_id;
	byte bg_frame_id;
	byte behavior;
	byte sound_id;
};

struct field_arrow
{
	int pos_x;
	int pos_y;
	int pos_z;
	int arrow_type;
};

struct field_camera_range
{
	short left;
	short bottom;
	short right;
	short top;
};

struct field_trigger_header
{
	byte field_name[9];
	byte control_direction;
	short focus_height;
	field_camera_range camera_range;
	byte field_14[4];
	short bg3_width;
	short bg3_height;
	short bg4_width;
	short bg4_height;
	short bg3_pos_x;
	short bg3_pos_y;
	short bg4_pos_x;
	short bg4_pos_y;
	short bg3_speed_x;
	short bg3_speed_y;
	short bg4_speed_x;
	short bg4_speed_y;
	short field_30[4];
	field_gateway gateways[12];
	field_trigger triggers[12];
	byte show_arrow_flag[12];
	field_arrow arrows[12];
};

struct world_event_data
{
	world_event_data *next_ptr;
	world_event_data *player_data_ptr;
	world_event_data *special_data_ptr;
	vector3<int> position;
	int field_18;
	vector3<int> prev_position;
	byte field_28[24];
	short facing;
	byte field_42[2];
	short offset_y;
	short curr_script_position;
	byte field_48[2];
	WORD walkmap_type;
	short direction;
	byte field_4E[2];
	byte model_id;
	byte animation_is_loop_mask;
	byte model_id_special;
	byte animation_frame_idx;
	byte field_54;
	byte movement_speed;
	byte wait_frames;
	byte is_function_running_maybe;
	byte animation_speed;
	byte field_59[3];
	char vertical_speed;
	byte animation_id;
	byte field_5E;
	char vertical_speed_2;
	byte field_60[104];
};

// --------------- end of FF7 imports ---------------

struct ff7_channel_6_state
{
	float volume;
	float panning;
};

// memory addresses and function pointers from FF7.exe
struct ff7_externals
{
	uint32_t chocobo_fix;
	uint32_t midi_fix;
	void *snowboard_fix;
	uint32_t cdcheck;
	uint32_t get_inserted_cd_sub;
	DWORD* insertedCD;
	uint8_t* requiredCD;
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
	battle_actor_data *battle_actor_data;
	DWORD *battle_mode;
	WORD *battle_location_id;
	WORD *battle_formation_id;
	uint32_t battle_sub_429AC0;
	uint32_t battle_sub_42D808;
	uint32_t battle_sub_42D992;
	uint32_t battle_sub_42DAE5;
	uint32_t battle_sub_427C22;
	uint32_t battle_menu_update_6CE8B3;
	uint32_t battle_sub_6DB0EE;
	char* is_battle_paused;
	std::span<uint32_t> battle_menu_state_fn_table;
	std::span<uint32_t> magic_effects_fn_table;
	uint32_t comet2_sub_5A42E5;
	uint32_t comet2_unload_sub_5A4359;
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
	uint32_t menu_sub_6FEDB0;
	void *(*menu_sub_6F5C0C)(uint32_t,uint32_t,uint8_t,uint8_t,uint32_t);
	void *(*menu_sub_6FAC38)(uint32_t,uint32_t,uint8_t,uint8_t,uint32_t);
	BOOL (*write_save_file)(char);
	uint32_t *menu_subs_call_table;
	uint32_t status_menu_sub;
	uint32_t draw_status_limit_level_stats;
	uint32_t timer_menu_sub;
	DWORD *millisecond_counter;
	char *(*get_kernel_text)(uint32_t, uint32_t, uint32_t);
	uint32_t sub_5CF282;
	uint32_t get_equipment_stats;
	struct weapon_data *weapon_data_array;
	struct armor_data *armor_data_array;
	uint32_t field_sub_6388EE;
	uint32_t field_draw_everything;
	uint32_t field_pick_tiles_make_vertices;
	uint32_t field_layer1_pick_tiles;
	uint32_t *field_layer1_tiles_num;
	uint32_t **field_layer1_palette_sort;
	field_tile **field_layer1_tiles;
	uint32_t field_layer2_pick_tiles;
	uint32_t *field_layer2_tiles_num;
	uint32_t **field_layer2_palette_sort;
	field_tile **field_layer2_tiles;
	uint32_t field_layer3_pick_tiles;
	uint32_t *field_layer3_tiles_num;
	uint32_t **field_layer3_palette_sort;
	field_tile **field_layer3_tiles;
	int *do_draw_layer3_CFFE3C;
	int *field_layer3_flag_CFFE40;
	uint32_t field_layer4_pick_tiles;
	uint32_t *field_layer4_tiles_num;
	uint32_t **field_layer4_palette_sort;
	field_tile **field_layer4_tiles;
	int *do_draw_layer4_CFFEA4;
	int *field_layer4_flag_CFFEA8;
	int *field_layer_CFF1D8;
	uint16_t *field_palette_D00088;
	uint32_t *field_special_y_offset;
	uint32_t *field_bg_multiplier;
	void (*add_page_tile)(float, float, float, float, float, uint32_t, uint32_t);
	double (*field_layer_sub_C23C0F)(ff7_field_camera*, int, int, int);
	field_trigger_header** field_triggers_header;
	ff7_field_camera* field_camera_CFF3D8;
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
	void (*_frame_animation)(uint32_t, struct matrix *, vector3<float> *, struct anim_frame *, struct anim_header *, struct hrc_bone *, struct hrc_data *);
	void (*root_animation)(struct matrix *, struct anim_frame *, struct anim_header *, struct hrc_data *);
	void (*frame_animation)(uint32_t, struct matrix *, vector3<float> *, struct anim_frame *, struct anim_header *, struct hrc_bone *, struct hrc_data *);
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
	uint32_t field_init_player_character_variables;
	uint32_t field_init_event_wrapper_63BCA7;
	uint32_t field_init_event_60BACF;
	uint32_t field_init_field_objects_60BCFA;
	uint32_t execute_opcode;
	uint32_t opcode_goldu;
	uint32_t opcode_dlitm;
	uint32_t opcode_smtra;
	uint32_t opcode_akao;
	uint32_t opcode_akao2;
	uint32_t opcode_bmusc;
	uint32_t opcode_fmusc;
	uint32_t opcode_cmusc;
	uint32_t opcode_canm1_canm2;
	uint32_t opcode_fade;
	uint32_t opcode_shake;
	uint32_t field_opcode_08_sub_61D0D4;
	void (*field_opcode_08_09_set_rotation_61DB2C)(short, byte, byte);
	uint32_t field_opcode_AA_2A_sub_616476;
	uint32_t field_opcode_turn_character_sub_616CB5;
	int (*field_get_rotation_final_636515)(vector3<int>*, vector3<int>*, int*);
	uint32_t field_music_helper;
	uint32_t field_music_helper_sound_op_call;
	uint32_t (*field_music_id_to_midi_id)(int16_t);
	uint32_t field_music_id_to_midi_id_call1;
	uint32_t field_music_id_to_midi_id_call2;
	uint32_t field_music_id_to_midi_id_call3;
	uint32_t opcode_gameover;
	uint32_t opcode_message;
	uint32_t opcode_ask;
	uint32_t opcode_wmode;
	uint32_t *sfx_initialized;
	uint32_t sfx_play_summon;
	uint32_t sfx_load_and_play_with_speed;
	uint32_t sfx_fmt_header;
	DWORD *sfx_play_effects_id_channel_6;
	uint32_t sfx_stop_channel_6;
	UINT *sfx_stop_channel_timer_handle;
	uint32_t battle_summon_leviathan_loop;
	uint32_t battle_limit_omnislash_loop;
	void (*reset_game_obj_sub_5F4971)(struct game_obj*);
	uint32_t engine_exit_game_mode_sub_666C78;
	void* (*sub_666C13)(struct game_obj*);
	void* (*sub_670F9B)(void*);
	WORD* word_CC0828;
	BYTE* byte_CC0D89;
	WORD* word_DB958A;
	BYTE* byte_CC164C;
	WORD* word_CC0DC6;
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
	uint32_t field_loop_sub_63C17F;
	uint32_t field_update_models_positions;
	int (*field_update_single_model_position)(short);
	void (*field_update_model_animation_frame)(short);
	int (*field_check_collision_with_target)(field_event_data*, short);
	int (*field_get_linear_interpolated_value)(int, int, int, int);
	int (*field_get_smooth_interpolated_value)(int, int, int, int);
	void (*field_evaluate_encounter_rate_60B2C6)();
	short *field_player_model_id;
	WORD *field_n_models;
	uint32_t field_update_camera_data;
	ff7_camdata** field_camera_data;
	uint32_t sub_40B27B;
	WORD* word_CC0DD4;
	WORD* word_CC1638;
	void (*field_update_background_positions)();
	uint32_t field_sub_64314F;
	void (*engine_sub_661976)(int, int);
	uint32_t engine_sub_66307D;
	void (*engine_sub_661465)(short*, float*);
	void (*engine_sub_66CF7E)(float*, vector3<float>*, vector3<float>*);
	vector2<int>* field_bg_offset;
	short* field_world_pos_x;
	short* field_world_pos_y;
	short* field_prev_world_pos_x;
	short* field_prev_world_pos_y;
	vector2<int>* field_vector2_CFF204;
	vector2<int>* field_vector2_CFF1F4;
	WORD* field_bg_flag_CC15E4;
	uint32_t field_sub_640EB7;
	uint32_t field_sub_661B68;
	void (*engine_sub_661B23)(int, int);
	void (*engine_sub_67CCDE)(float, float, float, float, float, float, float, ff7_game_obj*);
	uint32_t sub_630D50;
	uint32_t sub_631945;
	WORD* opcode_message_loop_code;
	int (*sub_6310A1)(uint8_t, uint8_t, uint8_t, uint8_t, WORD*);
	WORD* opcode_ask_question_code;
	void (*play_midi)(uint32_t);
	WORD *current_movie_frame;
	uint32_t opening_movie_play_midi_call;
	DWORD *opening_movie_music_start_frame;
	uint32_t fps_limiter_swirl;
	uint32_t fps_limiter_battle;
	uint32_t fps_limiter_coaster;
	uint32_t fps_limiter_condor;
	uint32_t fps_limiter_field;
	uint32_t fps_limiter_highway;
	uint32_t fps_limiter_snowboard;
	uint32_t fps_limiter_worldmap;
	uint32_t fps_limiter_chocobo;
	uint32_t fps_limiter_submarine;
	uint32_t fps_limiter_credits;
	uint32_t sub_5F5042;
	uint32_t sub_650F36;
	uint32_t sub_72381C;
	uint32_t sub_779E14;
	uint32_t battle_fps_menu_multiplier;
	DWORD *submarine_minigame_status;
	DWORD *field_limit_fps;
	DWORD *swirl_limit_fps;
	int16_t (*get_bank_value)(int16_t, int16_t);
	int8_t (*set_bank_value)(int16_t, int16_t, int16_t);
	int8_t (*get_char_bank_value)(int16_t, int16_t);
	uint32_t sub_611BAE;
	byte* current_entity_id;
	byte** field_script_ptr; //0xCBF5E8
	WORD* field_curr_script_position; //0xCC0CF8
	byte* field_model_id_array; //0xCBFB70
	field_event_data** field_event_data_ptr; // 0xCC0B60
	field_animation_data** field_animation_data_ptr; // 0xCFF738
	WORD* wait_frames_ptr; //0xCC0900
	char* animation_type_array; //0xCC0980
	WORD* field_game_moment; //0xDC08DC
	ff7_modules_global_object *modules_global_object; // 0xCC0D88
	ff7_modules_global_object **field_global_object_ptr; // 0xCBF9D8
	void (*sub_767039)(DWORD*,DWORD*,DWORD*);
	uint32_t play_battle_music_call;
	uint32_t (*play_battle_end_music)();
	uint32_t play_battle_music_win_call;
	uint32_t wm_change_music;
	uint32_t wm_play_music_call;
	uint32_t battle_fight_end;
	uint32_t battle_fanfare_music;
	int (*sub_630C48)(int16_t, int16_t, int16_t, int16_t, int16_t);
	uint32_t sub_408074;
	uint32_t sub_60BB58;
	byte** field_level_data_pointer;
	uint32_t sub_408116;
	char *word_CC16E8;
	int16_t* current_triangle_id;
	uint16_t* menu_battle_end_mode;
	uint32_t* pointer_functions_7C2980;
	uint32_t battle_enemy_killed_sub_433BD2;
	uint32_t battle_sub_5C7F94;
	uint32_t menu_battle_end_sub_6C9543;
	uint32_t menu_sub_71FF95, menu_shop_loop, get_materia_gil, opcode_increase_gil_call;
	uint32_t opcode_add_materia_inventory_call, menu_sub_6CBCF3, menu_sub_705D16, menu_sub_6CC17F;
	uint32_t display_battle_action_text_42782A;
	uint32_t display_battle_action_text_sub_6D71FA;
	uint32_t menu_decrease_item_quantity;
	uint32_t opcode_setbyte, opcode_biton;
	uint32_t sub_60FA7D;
	uint32_t menu_sub_7212FB;
	uint32_t load_save_file;
	uint32_t handle_actor_ready;
	WORD* battle_menu_state;
	uint32_t set_battle_menu_state_data;
	uint32_t dispatch_chosen_battle_action;
	uint32_t set_battle_targeting_data;
	uint16_t* issued_action_id;
	byte* issued_command_id;
	byte* issued_action_target_type;
	byte* issued_action_target_index;
	uint32_t field_load_models_atoi;
	uint32_t sub_6499F7;
	DWORD* input_ok_button_status;
	DWORD* input_run_button_status;

	// battle camera script externals
	uint32_t handle_camera_functions;
	uint32_t set_camera_focal_position_scripts;
	uint32_t set_camera_position_scripts;
	uint32_t add_fn_to_camera_fn_array;
	uint32_t execute_camera_functions;
	uint32_t battle_camera_sub_5C52F8;
	uint32_t battle_camera_sub_5C3E6F;
	uint32_t battle_camera_position_sub_5C3D0D;
	uint32_t battle_camera_position_sub_5C557D;
	uint32_t battle_camera_position_sub_5C5B9C;
	uint32_t battle_camera_focal_sub_5C5F5E;
	uint32_t battle_camera_focal_sub_5C5714;
	uint32_t battle_sub_430DD0;
	uint32_t battle_sub_429D8A;
	uint32_t update_battle_camera_sub_5C20CE;
	uint32_t set_battle_camera_sub_5C22BD;
	uint32_t battle_camera_sub_5C22A9;
	uint32_t compute_interpolation_to_formation_camera;
	uint32_t set_battle_camera_sub_5C2350;
	std::span<bcamera_fn_data> camera_fn_data;
	std::span<bcamera_position> battle_camera_position;
	std::span<bcamera_position> battle_camera_focal_point;
	std::span<uint32_t> camera_fn_array;
	byte* battle_camera_focal_scripts_8FEE30;
	byte* battle_camera_position_scripts_8FEE2C;
	DWORD* battle_camera_global_scripts_9A13BC;
	DWORD* battle_camera_position_scripts_9010D0;
	DWORD* battle_camera_focal_scripts_901270;
	byte* battle_camera_script_index;
	DWORD* battle_camera_script_offset;
	WORD* camera_fn_index;
	WORD* camera_fn_counter;
	vector3<short>* g_battle_camera_position;
	vector3<short>* g_battle_camera_focal_point;
	std::span<formation_camera> formation_camera;
	byte* curr_formation_camera_idx;
	byte* battle_enter_frames_to_wait;
	byte* g_variation_index;
	byte* is_camera_moving_BFB2DC;

	// animation script externals
	uint32_t battle_sub_42A5EB;
	uint32_t battle_sub_42E275;
	uint32_t battle_sub_42E34A;
	uint32_t battle_sub_5B9EC2;
	uint32_t battle_sub_5BD5E9;
	uint32_t run_summon_animations_script_5C1B81;
	uint32_t run_summon_animations_script_sub_5C1D9A;
	uint32_t run_animation_script;
	uint32_t add_fn_to_effect100_fn;
	uint32_t execute_effect100_fn;
	uint32_t add_fn_to_effect60_fn;
	uint32_t execute_effect60_fn;
	uint32_t add_fn_to_effect10_fn;
	uint32_t execute_effect10_fn;
	uint32_t battle_enemy_death_5BBD24;
	uint32_t battle_enemy_death_sub_5BBE32;
	uint32_t battle_iainuki_death_5BCAAA;
	uint32_t battle_iainuki_death_sub_5BCBB8;
	uint32_t battle_boss_death_5BC48C;
	uint32_t battle_boss_death_sub_5BC6ED;
	uint32_t battle_boss_death_sub_5BC5EC;
	uint32_t battle_boss_death_call_5BD436;
	uint32_t battle_melting_death_5BC21F;
	uint32_t battle_melting_death_sub_5BC32D;
	uint32_t battle_disintegrate_2_death_5BBA82;
	uint32_t battle_disintegrate_2_death_sub_5BBBDE;
	uint32_t battle_morph_death_5BC812;
	uint32_t battle_morph_death_sub_5BC920;
	uint32_t battle_disintegrate_1_death_5BBF31;
	uint32_t battle_disintegrate_1_death_sub_5BC04D;
	uint32_t battle_sub_42C0A7;
	uint32_t run_summon_animations_5C0E4B;
	uint32_t vincent_limit_fade_effect_sub_5D4240;
	uint32_t battle_sub_5BD96D;
	uint32_t battle_sub_425D29;
	uint32_t display_battle_damage_5BB410;
	uint32_t battle_sub_5BDA0F;
	uint32_t get_n_frames_display_action_string;
	uint32_t battle_sub_434C8B;
	uint32_t battle_sub_435D81;
	uint32_t battle_sub_426DE3;
	uint32_t battle_sub_426941;
	uint32_t battle_sub_426899;
	uint32_t battle_sub_4267F1;
	uint32_t battle_sub_5C1C8F;
	uint32_t battle_move_character_sub_426A26;
	uint32_t battle_move_character_sub_42739D;
	uint32_t battle_move_character_sub_426F58;
	uint32_t battle_move_character_sub_4270DE;
	uint32_t handle_aura_effects_425520;
	uint32_t run_aura_effects_5C0230;
	uint32_t limit_break_aura_effects_5C0572;
	uint32_t enemy_skill_aura_effects_5C06BF;
	uint32_t handle_summon_aura_5C0850;
	uint32_t summon_aura_effects_5C0953;
	uint32_t battle_sub_5C18BC;
	uint32_t battle_sub_4276B6;
	uint32_t battle_sub_4255B7;
	uint32_t battle_sub_425E5F;
	uint32_t battle_sub_5BCF9D;
	uint32_t battle_sub_425AAD;
	uint32_t battle_sub_427A6C;
	uint32_t battle_sub_427AF1;
	uint32_t battle_sub_427737;
	uint32_t battle_sub_4277B1;
	uint32_t battle_sub_5BCD42;
	uint32_t battle_sub_5BD050;
	uint32_t battle_smoke_move_handler_5BE4E2;
	uint32_t battle_sub_42A72D;
	void (*battle_play_sfx_sound_430D32)(uint16_t, short, char);
	uint32_t run_tifa_limit_effects;
	uint32_t tifa_limit_1_2_sub_4E3D51;
	uint32_t tifa_limit_2_1_sub_4E48D4;
	uint32_t aerith_limit_2_1_sub_45B0CF;
	uint32_t vincent_limit_satan_slam_camera_45CF2A;
	uint32_t barret_limit_4_1_camera_4688A2;
	uint32_t barret_limit_4_1_model_movement_4698EF;
	int *barret_limit_4_1_actor_id;
	uint32_t aerith_limit_4_1_camera_473CC2;
	uint32_t run_chocomog_movement_50B1A3;
	uint32_t run_chocomog_camera_509B10;
	uint32_t run_fat_chocobo_movement_509692;
	uint32_t run_fat_chocobo_camera_507CA4;
	uint32_t run_fat_chocobo_camera_shake_5095F5;
	uint32_t run_shiva_movement_592538;
	uint32_t run_shiva_camera_58E60D;
	uint32_t run_ifrit_movement_596702;
	uint32_t run_ifrit_camera_592A36;
	uint32_t run_ramuh_camera_597206;
	uint32_t run_titan_camera_59B4B0;
	uint32_t run_odin_gunge_movement_4A584D;
	uint32_t run_odin_gunge_camera_4A0F52;
	uint32_t run_odin_steel_movement_4A6CB8;
	uint32_t run_odin_steel_sub_4A9908;
	uint32_t run_odin_steel_camera_4A5D3C;
	uint32_t run_leviathan_camera_5B0716;
	uint32_t run_bahamut_movement_49ADEC;
	uint32_t run_bahamut_camera_497A37;
	uint32_t run_kujata_camera_4F9A4D;
	uint32_t run_alexander_movement_5078D8;
	uint32_t run_alexander_camera_501637;
	uint32_t run_phoenix_main_loop_516297;
	uint32_t run_phoenix_movement_518AFF;
	uint32_t run_phoenix_camera_515238;
	uint32_t run_bahamut_neo_movement_48D7BC;
	uint32_t run_bahamut_neo_camera_48C75D;
	uint32_t run_hades_camera_4B65A8;
	uint32_t run_typhoon_camera_4D594C;
	uint32_t run_typhoon_sub_4DA182;
	uint32_t run_bahamut_zero_main_loop_484A16;
	uint32_t run_bahamut_zero_movement_48BBFC;
	uint32_t run_bahamut_zero_camera_483866;
	uint32_t run_summon_kotr_sub_476857;
	uint32_t run_summon_kotr_main_loop_478031;
	std::array<uint32_t, 13> run_summon_kotr_knight_script;
	void(*add_kotr_camera_fn_to_effect100_fn_476AAB)(DWORD, DWORD, WORD);
	uint32_t run_kotr_camera_476AFB;
	vector3<int>* (*battle_sub_661000)(int);
	void (*engine_sub_663673)(WORD*);
	void (*engine_sub_663707)(DWORD*);
	void (*battle_sub_662ECC)(vector3<short>*, vector3<int>*, int*);
	uint32_t run_chocobuckle_main_loop_560C32;
	uint32_t run_confu_main_loop_5600BE;
	uint32_t bomb_blast_black_bg_effect_537427;
	uint32_t goblin_punch_flash_573291;
	uint32_t roulette_skill_main_loop_566287;
	uint32_t death_sentence_main_loop_5661A0;
	uint32_t death_kill_sub_loop_562C60;
	uint32_t death_kill_sub_loop_5624A5;
	uint32_t enemy_atk_camera_sub_439EE0;
	uint32_t enemy_atk_camera_sub_44A7D2;
	uint32_t enemy_atk_camera_sub_44EDC0;
	uint32_t enemy_atk_camera_sub_4522AD;
	uint32_t enemy_atk_camera_sub_457C60;
	uint32_t battle_update_3d_model_data;
	uint32_t battle_animate_material_texture;
	uint32_t battle_animate_texture_spt;
	rotation_matrix* (*get_global_model_matrix_buffer_66100D)();
	struc_84* (*get_draw_chain_68F860)(struc_49*, graphics_instance*);
	p_hundred* (*battle_sub_5D1AAA)(int, ff7_polygon_set*);
	int (*get_alpha_from_transparency_429343)(int);
	color_ui8 (*get_stored_color_66101A)();
	void (*battle_sub_68CF75)(char, struc_173*);
	void (*create_rot_matrix_from_word_matrix_6617E9)(rotation_matrix*, matrix*);
	struc_84* (*get_draw_chain_671C71)(ff7_graphics_object*);
	void (*battle_sub_6CE81E)();
	std::span<battle_model_state> g_battle_model_state;
	std::span<battle_model_state_small> g_small_battle_model_state;
	std::span<uint32_t> effect100_array_fn;
	std::span<effect100_data> effect100_array_data;
	uint16_t* effect100_counter;
	uint16_t* effect100_array_idx;
	std::span<uint32_t> effect60_array_fn;
	std::span<effect60_data> effect60_array_data;
	uint16_t* effect60_counter;
	uint16_t* effect60_array_idx;
	std::span<uint32_t> effect10_array_fn;
	std::span<effect10_data> effect10_array_data;
	uint16_t* effect10_counter;
	uint16_t* effect10_array_idx;
	short* effect10_array_data_8FE1F6;
	std::array<byte*, 14> animation_script_pointers;
	byte* g_is_effect_loading;
	byte* g_is_battle_paused;
	byte* g_actor_idle_scripts;
	byte* g_script_wait_frames;
	std::span<int*> g_script_args;
	byte* special_actor_id;
	int* field_battle_BFB2E0;
	float* field_float_battle_7B7680;
	byte* field_byte_DC0E11;
	byte* field_battle_byte_BF2E1C;
	byte* field_battle_byte_BE10B4;
	short* resting_Y_array_data;
	WORD* field_odin_frames_AEEC14;
	palette_extra* palette_extra_data_C06A00;
	uint32_t** global_game_data_90AAF0;
	std::span<uint32_t> limit_break_effects_fn_table;
	std::span<uint32_t> enemy_atk_effects_fn_table;
	std::span<uint32_t> enemy_skill_effects_fn_table;
	byte* byte_BCC788;
	vector3<int>** vector3_int_ptr_BCC6A8;
	vector3<short>* battle_ifrit_model_position;
	WORD* word_array_BCC768;

	// battle menu
	uint32_t display_battle_menu_6D797C;
	void (*display_cait_sith_slots_handler_6E2170)();
	void (*display_tifa_slots_handler_6E3135)();
	void (*display_battle_arena_menu_handler_6E384F)();
	uint32_t battle_set_do_render_menu;
	int *g_do_render_menu;
	uint32_t battle_menu_update_call;
	int *battle_menu_animation_idx;
	uint32_t set_battle_speed_4385CC;
	uint32_t battle_set_actor_timer_data_4339C2;
	uint32_t battle_sub_42F3E8;
	uint32_t battle_handle_player_mark_5B9C8E;
	uint32_t battle_handle_status_effect_anim_5BA7C0;
	uint32_t battle_update_targeting_info_6E6291;
	byte *targeting_actor_id_DC3C98;

	//battle 3d battleground
	uint32_t update_3d_battleground;
	void (*battleground_shake_train_42F088)();
	uint32_t battleground_vertical_scrolling_42F126;
	uint32_t battleground_midgar_flashback_rain_5BDC4F;

	// battle dialogue
	uint32_t battle_sub_42CBF9;
	uint32_t add_text_to_display_queue;
	uint32_t update_display_text_queue;
	uint32_t set_battle_text_active;
	uint32_t battle_sfx_play_effect_430D14;
	int (*battle_sub_66C3BF)();
	uint32_t battle_sub_43526A;
	uint32_t battle_sub_5C8931;
	uint32_t run_enemy_ai_script;
	uint32_t enqueue_script_action;
	uint32_t battle_sub_41B577;
	uint32_t battle_sub_41CCB2;
	std::span<battle_text_data> battle_display_text_queue;
	battle_ai_context *battle_context;
	std::span<battle_anim_event> anim_event_queue;
	byte* anim_event_index;
	int* g_is_battle_running_9AD1AC;
	WORD* field_battle_word_BF2E08;
	WORD* field_battle_word_BF2032;
	byte* g_active_actor_id;

	// world stuff
	uint32_t world_mode_loop_sub_74DB8C;
	uint32_t world_init_variables_74E1E9;
	uint32_t world_sub_7641A7;
	uint32_t run_world_event_scripts;
	uint32_t run_world_event_scripts_system_operations;
	uint32_t world_animate_all_models;
	uint32_t world_animate_single_model;
	uint32_t run_world_snake_ai_script_7562FF;
	uint32_t update_world_snake_position_7564CD;
	uint32_t world_sub_767540;
	uint32_t world_sub_767641;
	uint32_t world_opcode_message_sub_75EE86;
	uint32_t world_opcode_message_update_box_769050;
	uint32_t world_opcode_message_update_text_769C02;
	int (*get_world_encounter_rate)();
	int (*pop_world_script_stack)();
	uint32_t world_update_player_74EA48;
	int (*world_get_player_model_id)();
	int (*world_get_current_key_input_status)();
	int (*world_get_player_walkmap_type)();
	void(*world_sub_753D00)(vector3<short>*, short);
	void(*world_update_model_movement_762E87)(int, int);
	world_event_data** world_event_current_entity_ptr_E39AD8;
	world_event_data** world_event_current_entity_ptr_E3A7CC;
	int* is_wait_frames_zero_E39BC0;

	uint32_t swirl_main_loop;
	uint32_t swirl_loop_sub_4026D4;
};

uint32_t ff7gl_load_group(uint32_t group_num, struct matrix_set *matrix_set, struct p_hundred *hundred_data, struct p_group *group_data, struct polygon_data *polygon_data, struct ff7_polygon_set *polygon_set, struct ff7_game_obj *game_object);
void ff7gl_field_78(struct ff7_polygon_set *polygon_set, struct ff7_game_obj *game_object);
void ff7_init_hooks(struct game_obj *_game_object);
struct ff7_gfx_driver *ff7_load_driver(void *game_object);
