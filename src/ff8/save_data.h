/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2025 Julian Xhokaxhiu                                   //
//    Copyright (C) 2025 Tang-Tang Zhou                                     //
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

#define G_FORCE_NUM 16
#define CHAR_NUM 8

#include <cstdint>

struct savemap_ff8_header {
	uint16_t location_id;
	uint16_t char1_curr_hp;
	uint16_t char1_max_hp;
	uint16_t save_count;
	uint32_t gil;
	uint32_t played_time_secs;
	uint8_t char1_lvl;
	uint8_t char1_portrait;
	uint8_t char2_portrait;
	uint8_t char3_portrait;
	uint8_t squall_name[12];
	uint8_t rinoa_name[12];
	uint8_t angelo_name[12];
	uint8_t boko_name[12];
	uint32_t curr_disk;
	uint32_t curr_save;
};

struct savemap_ff8_gf {
	uint8_t name[12];
	uint32_t exp;
	uint8_t unk1;
	uint8_t exists;
	uint16_t HPs;
	uint8_t complete_abilities[16]; // 115+1 valides, 124 existantes
	uint8_t APs[24];                // 22 + 2 unused
	uint16_t kills;
	uint16_t KOs;
	uint8_t learning;
	uint8_t forgotten1; // 22 + 2 unused
	uint8_t forgotten2;
	uint8_t forgotten3;
};

struct savemap_ff8_character {
	uint16_t current_hp;
	uint16_t max_hp;
	uint32_t exp;
	uint8_t model_id;
	uint8_t weapon_id;
	uint8_t str;
	uint8_t vit;
	uint8_t mag;
	uint8_t spr;
	uint8_t spd;
	uint8_t lck;
	uint16_t magics[32];
	uint8_t commands[3];
	uint8_t unk1; // unused command (padding)
	uint8_t abilities[4];
	uint16_t gfs;
	uint8_t unk2;              // used unknown value
	uint8_t alternative_model; // Seed costume/Galbadia costume
	uint8_t j_hp;
	uint8_t j_str;
	uint8_t j_vit;
	uint8_t j_mag;
	uint8_t j_spr;
	uint8_t j_spd;
	uint8_t j_eva;
	uint8_t j_hit;
	uint8_t j_lck;
	uint8_t j_atk_ele;
	uint8_t j_atk_mtl;
	uint8_t j_def_elem[4];
	uint8_t j_def_mtl[4];
	uint8_t unk3; // padding ?
	uint16_t gf_compat[G_FORCE_NUM];
	uint16_t kills;
	uint16_t kos;
	uint8_t exists;
	uint8_t unk4;
	uint8_t status;
	uint8_t unk5; // padding ?
};

struct savemap_ff8_shop {
	uint8_t items[16];
	uint8_t visited;
	uint8_t unk1[3];
};

struct savemap_ff8_limit_break {
	uint16_t quistis_lb;
	uint16_t zell_lb;
	uint8_t irvine_lb;
	uint8_t selphie_lb;
	uint8_t angelo_c_lb;
	uint8_t angelo_k_lb;
	uint8_t angelo_p_lb[8];
};

struct savemap_ff8_items {
	uint8_t battle_order[32];
	uint16_t items[198];
};

#pragma pack(push, 1)
struct savemap_ff8_battle {
	uint32_t unk1;
	uint32_t victory_count;
	uint16_t unk2;
	uint16_t battle_escaped;
	uint32_t unk3;
	uint32_t tomberry_vaincus;
	uint32_t tomberry_sr_vaincu;
	uint32_t ufo_battle_encountered;
	uint32_t elmidea_battle_r1;
	uint32_t succube_battle_elemental;
	uint32_t trex_battle_mental;
	uint32_t battle_irvine;
	uint8_t magic_drawn_once[8];
	uint8_t ennemy_scanned_once[20];
	uint8_t renzokuken_auto;
	uint8_t renzokuken_indicator;
	uint8_t unk4;
};
#pragma pack(pop)

struct savemap_ff8_field_h {
	uint32_t unk1;
	uint32_t steps;
	uint32_t payslip;
	uint32_t unk2;
	uint16_t seedExp;
	uint16_t unk3;
	uint32_t victory_count;
	uint16_t unk4;
	uint16_t battle_escaped;
	uint16_t kills[8];
	uint16_t ko[8];
	uint8_t unk5[8];
	uint32_t monster_kills;
	uint32_t gils;
	uint32_t dream_gils;
	uint32_t current_frame;
	uint16_t last_field_id;
	uint8_t current_car_rent;
	uint8_t music_util;
	uint8_t move_find_ondine;
	uint8_t unk6[15];
	uint32_t unk7;
	uint32_t music_related;
	uint32_t unk8;
	uint8_t draw_points[64];
	uint16_t steps2;
	uint16_t battle_mode;
	uint16_t unk9;
	uint8_t unkA[11];
	uint8_t music_volume;
	uint8_t unkB;
	uint8_t music_played;
	uint8_t unkC;
	uint8_t music_is_played;
	uint8_t unkD;
	uint8_t battle_music;
	uint8_t curr_disk;
	uint8_t unkE;
	uint8_t music_is_loaded;
	uint8_t battle_off;
	uint8_t unkF;
	uint8_t save_enabled;
	uint8_t unkG[3];
	uint8_t music_loaded;
	uint8_t unkH[42];
};

struct savemap_ff8_triple_triad {
	uint8_t cards[77];
	uint8_t card_locations[33];
	uint8_t cards_rare[5];
	uint8_t unk1;
	uint16_t victory_count;
	uint16_t defeat_count;
	uint16_t equality_count;
	uint16_t unk2;
	uint32_t unk3;
};

// For more info: https://hobbitdur.github.io/FF8ModdingWiki/technical-reference/miscellaneous/game-save-format
struct savemap_ff8 {
	uint16_t checksum;
	uint16_t fixed_value; // always 0x8FF
	savemap_ff8_header header;
	savemap_ff8_gf gfs[G_FORCE_NUM];
	savemap_ff8_character chars[CHAR_NUM];
	savemap_ff8_shop shop[20];
	uint8_t config[20];
	uint8_t party[4]; // 0xFF terminated
	uint32_t known_weapons;
	uint8_t griever_name[12];
	uint16_t unk1;
	uint16_t unk2;
	uint32_t gil;
	uint32_t gil_laguna;
	savemap_ff8_limit_break lb;
	savemap_ff8_items items;
	uint32_t game_time;
	uint32_t countdown;
	savemap_ff8_battle battle;
	uint8_t tutorial_infos[16];
	uint8_t seed_test_lvl;
	uint32_t unk3;
	uint8_t party_other[4];
	uint32_t unk4;
	uint16_t module; // 1=field, 2=worldmap, 3=battle
	uint16_t curr_loc;
	uint16_t prev_loc;
	uint16_t x[3];  // coord x (party1, party2, party3)
	uint16_t y[3];  // coord y (party1, party2, party3)
	uint16_t id[3]; // triangle (party1, party2, party3)
	uint8_t dir[3]; // direction (party1, party2, party3)
	uint8_t unk7[5];
	savemap_ff8_field_h field_header;
	uint8_t field[1024];
	uint8_t worldmap[128];
	savemap_ff8_triple_triad triple_triad;
	uint8_t choco_world[64];
};

