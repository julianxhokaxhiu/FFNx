/****************************************************************************/
//    Copyright (C) 2026 Julian Xhokaxhiu                                   //
//    Copyright (C) 2026 HobbitDur                                          //
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

#include "kernel_enums.h"

#include <stddef.h>
#include <stdint.h>
#include <span>

// Packed on-disk records. Section numbers in the wiki/editor are one-based.
#pragma pack(push, 1)

struct ff8_kernel_gf_learning_slot
{
	uint8_t summon_status_accuracy_or_unused; // Summon accuracy in slot 0; unused in the other slots.
	uint8_t level_or_prereq_slot; // Level 0..100, or prerequisite slot + 101.
	uint8_t excluded_slot; // Checked only with a prerequisite; 0xFF means none.
	uint8_t ability_id;
};

struct ff8_kernel_junction_flags
{
	ff8_kernel_junction_flags_byte0 low;
	ff8_kernel_junction_flags_byte1 middle;
	ff8_kernel_junction_flags_byte2 high;
};

struct ff8_kernel_character_flags
{
	ff8_kernel_character_flags_byte0 low;
	ff8_kernel_character_flags_byte1 middle;
	ff8_kernel_character_flags_byte2 high;
};

struct ff8_kernel_duel_move
{
	uint8_t start_move;
	uint8_t next_sequence1;
	uint8_t next_sequence2;
	uint8_t next_sequence3;
};

struct ff8_kernel_magic_quantity
{
	uint8_t id;
	uint8_t amount;
};

// 1. Battle commands

struct ff8_kernel_battle_command
{
	uint16_t offset_ability_name;
	uint16_t offset_ability_description;
	uint8_t ability_data_id;
	ff8_kernel_command_menu_flags menu_flags;
	ff8_kernel_target_info target_info;
	uint8_t execution_type; // Unused in vanilla; FFNx execution type / disabled marker.
};

// 2. Magic

struct ff8_kernel_magic
{
	uint16_t offset_spell_name;
	uint16_t offset_spell_description;
	uint16_t attack_animation_id;
	uint8_t target_hit_animation;
	ff8_kernel_attack_type attack_type;
	uint8_t spell_power;
	ff8_kernel_status_window_flags status_window_flags;
	ff8_kernel_target_info target_info;
	ff8_kernel_attack_flags attack_flags;
	uint8_t draw_resist;
	uint8_t hit_count;
	ff8_kernel_element element;
	uint8_t padding1;
	ff8_kernel_status2 status2;
	ff8_kernel_status1 status1;
	uint8_t status_attack_accuracy;
	uint8_t hp_junction_value;
	uint8_t str_junction_value;
	uint8_t vit_junction_value;
	uint8_t mag_junction_value;
	uint8_t spr_junction_value;
	uint8_t spd_junction_value;
	uint8_t eva_junction_value;
	uint8_t hit_junction_value;
	uint8_t luck_junction_value;
	ff8_kernel_element j_elem_attack;
	uint8_t j_elem_attack_value;
	ff8_kernel_element j_elem_defense;
	uint8_t j_elem_defense_value;
	uint8_t j_status_attack_value;
	uint8_t j_status_defense_value;
	ff8_kernel_junction_status j_statuses_attack;
	ff8_kernel_junction_status j_statuses_defend;
	uint8_t quezacoalt_compatibility;
	uint8_t shiva_compatibility;
	uint8_t ifrit_compatibility;
	uint8_t siren_compatibility;
	uint8_t brothers_compatibility;
	uint8_t diablos_compatibility;
	uint8_t carbuncle_compatibility;
	uint8_t leviathan_compatibility;
	uint8_t pandemona_compatibility;
	uint8_t cerberus_compatibility;
	uint8_t alexander_compatibility;
	uint8_t doomtrain_compatibility;
	uint8_t bahamut_compatibility;
	uint8_t cactuar_compatibility;
	uint8_t tonberry_compatibility;
	uint8_t eden_compatibility;
	uint16_t padding2;
};

// 3. Junctionable gfs

struct ff8_kernel_junctionable_gf
{
	uint16_t offset_gf_attack_name;
	uint16_t offset_gf_attack_description;
	uint16_t attack_animation_id;
	ff8_kernel_attack_type attack_type;
	uint8_t gf_power;
	ff8_kernel_status_window_flags status_window_flags;
	ff8_kernel_target_info target_info;
	ff8_kernel_attack_flags attack_flags;
	uint8_t target_hit_animation;
	uint8_t hit_count;
	ff8_kernel_element element;
	ff8_kernel_status1 status1;
	ff8_kernel_status2 status2;
	uint8_t gf_hp_modifier1;
	uint8_t gf_hp_modifier2;
	uint8_t gf_hp_modifier3;
	uint8_t padding1;
	uint8_t next_level_modifier1;
	uint8_t next_level_modifier2;
	uint8_t unused_status_attack_accuracy;
	ff8_kernel_gf_learning_slot ability_data[21];
	uint8_t padding2;
	uint8_t quezacolt_compatibility;
	uint8_t shiva_compatibility;
	uint8_t ifrit_compatibility;
	uint8_t siren_compatibility;
	uint8_t brothers_compatibility;
	uint8_t diablos_compatibility;
	uint8_t carbuncle_compatibility;
	uint8_t leviathan_compatibility;
	uint8_t pandemona_compatibility;
	uint8_t cerberus_compatibility;
	uint8_t alexander_compatibility;
	uint8_t doomtrain_compatibility;
	uint8_t bahamut_compatibility;
	uint8_t cactuar_compatibility;
	uint8_t tonberry_compatibility;
	uint8_t eden_compatibility;
	uint16_t gf_boost_params; // Low byte: first safe phase; high byte: total window; each x15 ticks.
	uint8_t power_mod;
	uint8_t level_mod;
};

// 4. Enemy attacks

struct ff8_kernel_enemy_attack
{
	uint16_t offset_attack_name;
	uint16_t attack_animation_id;
	uint8_t camera_change;
	uint8_t target_hit_animation;
	ff8_kernel_attack_type attack_type;
	uint8_t attack_power;
	ff8_kernel_attack_flags attack_flags;
	uint8_t hit_count_and_name_flag;
	ff8_kernel_element attack_element;
	uint8_t attack_crit_bonus;
	uint8_t status_attack_accuracy;
	uint8_t hit_rate;
	ff8_kernel_status1 status1;
	ff8_kernel_status2 status2;
};

// 5. Weapons

struct ff8_kernel_weapon
{
	uint16_t offset_weapon_name;
	ff8_kernel_renzokuken_flags renzokuken_finishers;
	uint8_t padding;
	uint8_t character_id;
	ff8_kernel_attack_type attack_type;
	uint8_t attack_power;
	uint8_t hit_rate;
	uint8_t str_bonus;
	uint8_t weapon_tier;
	uint8_t crit_bonus;
	uint8_t is_melee_weapon;
};

// 6. Renzokuken finishers

struct ff8_kernel_renzokuken_finisher
{
	uint16_t offset_limit_name;
	uint16_t offset_limit_description;
	uint16_t attack_animation_id;
	ff8_kernel_attack_type attack_type;
	uint8_t padding1;
	uint8_t attack_power;
	uint8_t target_hit_animation;
	ff8_kernel_target_info target_info;
	ff8_kernel_attack_flags attack_flags;
	uint8_t hit_count;
	ff8_kernel_element element_attack;
	uint8_t element_attack_percent;
	uint8_t status_attack_accuracy;
	uint16_t padding2;
	ff8_kernel_status1 status1;
	ff8_kernel_status2 status2;
};

// 7. Characters

struct ff8_kernel_character
{
	uint16_t offset_character_name;
	uint8_t crisis_level_hp_multiplier;
	ff8_kernel_gender gender;
	uint8_t limit_break_id;
	uint8_t limit_break_param;
	uint16_t exp_modifier;
	uint8_t hp[4];
	uint8_t str[4];
	uint8_t vit[4];
	uint8_t mag[4];
	uint8_t spr[4];
	uint8_t spd[4];
	uint8_t luck[4];
};

// 8. Battle items

struct ff8_kernel_battle_item
{
	uint16_t offset_item_name;
	uint16_t offset_item_description;
	uint16_t attack_animation_id;
	ff8_kernel_attack_type attack_type;
	uint8_t attack_power;
	uint8_t battle_flag;
	ff8_kernel_target_info target_info;
	ff8_kernel_attack_flags attack_flags_and_selectability;
	uint8_t target_hit_animation;
	uint8_t padding;
	uint8_t status_attack_accuracy;
	ff8_kernel_status1 status1;
	ff8_kernel_status2 status2;
	uint8_t hit_rate;
	uint8_t random_select_flag;
	uint8_t hit_count;
	ff8_kernel_element element;
};

// 9. Non battle items

struct ff8_kernel_non_battle_item
{
	uint16_t offset_to_item_name;
	uint16_t offset_to_item_description;
};

// 10. Non junctionable gfs

struct ff8_kernel_non_junctionable_gf
{
	uint16_t offset_gf_attack_name;
	uint16_t attack_animation_id;
	ff8_kernel_attack_type attack_type;
	uint8_t gf_power;
	uint8_t status_attack_accuracy;
	ff8_kernel_target_info target_info;
	ff8_kernel_attack_flags attack_flags;
	uint8_t target_hit_animation;
	uint8_t hit_count;
	ff8_kernel_element element;
	ff8_kernel_status2_flag_low_byte status2_low_byte;
	ff8_kernel_status2_byte1_flags status2_byte1;
	ff8_kernel_status2_byte2_flags status2_byte2;
	ff8_kernel_status2_byte3_flags status2_byte3;
	ff8_kernel_status1_flag_low_byte status1_low_byte;
	uint8_t padding;
	uint8_t power_mod;
	uint8_t level_mod;
};

// 11. Command ability data

struct ff8_kernel_command_ability_data
{
	uint16_t attack_animation_id;
	uint8_t padding;
	uint8_t target_hit_animation;
	ff8_kernel_attack_type attack_type;
	uint8_t attack_power;
	ff8_kernel_attack_flags attack_flags;
	uint8_t hit_count;
	ff8_kernel_element element;
	uint8_t status_attack_accuracy;
	ff8_kernel_status1 status1;
	ff8_kernel_status2 status2;
};

// 12. Junction abilities

struct ff8_kernel_junction_ability
{
	uint16_t name_offset;
	uint16_t description_offset;
	uint8_t ap_required;
	ff8_kernel_junction_flags granted_ability_flags;
};

// 13. Command abilities

struct ff8_kernel_command_ability
{
	uint16_t name_offset;
	uint16_t description_offset;
	uint8_t ap_required;
	uint8_t battle_command_id;
	uint16_t padding;
};

// 14. Stat percentage abilities

struct ff8_kernel_stat_percentage_ability
{
	uint16_t name_offset;
	uint16_t description_offset;
	uint8_t ap_required;
	ff8_kernel_stat_index stat_to_increase;
	uint8_t increase_value;
	uint8_t padding;
};

// 15. Character abilities

struct ff8_kernel_character_ability
{
	uint16_t name_offset;
	uint16_t description_offset;
	uint8_t ap_required;
	ff8_kernel_character_flags flags;
};

// 16. Party abilities

struct ff8_kernel_party_ability
{
	uint16_t name_offset;
	uint16_t description_offset;
	uint8_t ap_required;
	ff8_kernel_party_flags flag;
	uint16_t padding;
};

// 17. Gf abilities

struct ff8_kernel_gf_ability
{
	uint16_t name_offset;
	uint16_t description_offset;
	uint8_t ap_required;
	uint8_t enable_boost;
	ff8_kernel_gf_stat stat_to_increase;
	uint8_t increase_value;
};

// 18. Menu abilities

struct ff8_kernel_menu_ability
{
	uint16_t name_offset;
	uint16_t description_offset;
	uint8_t ap_required;
	ff8_kernel_refine_table m00x_file_index;
	uint8_t start_offset;
	uint8_t end_offset;
};

// 19. Temporary character limits

struct ff8_kernel_temporary_character_limit
{
	uint16_t limit_name_offset;
	uint16_t limit_description_offset;
	uint16_t attack_animation_id;
	ff8_kernel_attack_type attack_type;
	uint8_t attack_power;
	uint8_t target_hit_animation;
	ff8_kernel_status_window_flags status_window_flags;
	ff8_kernel_target_info target_info;
	ff8_kernel_attack_flags attack_flags;
	uint8_t hit_count;
	ff8_kernel_element element_attack;
	uint8_t element_attack_percent;
	uint8_t status_attack_accuracy;
	ff8_kernel_status1 status1;
	uint16_t padding;
	ff8_kernel_status2 status2;
};

// 20. Blue magic

struct ff8_kernel_blue_magic
{
	uint16_t limit_name_offset;
	uint16_t limit_description_offset;
	uint16_t attack_animation_id;
	uint8_t target_hit_animation;
	ff8_kernel_attack_type attack_type;
	ff8_kernel_status_window_flags status_window_flags;
	ff8_kernel_target_info target_info;
	ff8_kernel_attack_flags attack_flags;
	uint8_t hit_count;
	ff8_kernel_element element;
	uint8_t status_attack_accuracy;
	uint8_t crit_bonus;
	uint8_t padding2;
};

// 21. Blue magic parameters

struct ff8_kernel_blue_magic_parameter
{
	ff8_kernel_status2 status2;
	ff8_kernel_status1 status1;
	uint8_t attack_power;
	uint8_t hit_rate;
};

// 22. Shot

struct ff8_kernel_shot
{
	uint16_t limit_name_offset;
	uint16_t limit_description_offset;
	uint16_t attack_animation_id;
	ff8_kernel_attack_type attack_type;
	uint8_t attack_power;
	uint8_t target_hit_animation;
	ff8_kernel_status_window_flags status_window_flags;
	ff8_kernel_target_info target_info;
	ff8_kernel_attack_flags attack_flags;
	uint8_t hit_count;
	ff8_kernel_element element_attack;
	uint8_t element_attack_percent;
	uint8_t status_attack_accuracy;
	ff8_kernel_status1 status1;
	uint8_t used_item_index;
	uint8_t crit_increase;
	ff8_kernel_status2 status2;
};

// 23. Duel

struct ff8_kernel_duel
{
	uint16_t limit_name_offset;
	uint16_t limit_description_offset;
	uint16_t attack_animation_id;
	ff8_kernel_attack_type attack_type;
	uint8_t attack_power;
	uint8_t target_hit_animation;
	uint8_t padding;
	ff8_kernel_target_info target_info;
	ff8_kernel_attack_flags attack_flags;
	uint8_t hit_count;
	ff8_kernel_element element_attack;
	uint8_t element_attack_percent;
	uint8_t status_attack_accuracy;
	uint16_t sequence_button1;
	uint16_t sequence_button2;
	uint16_t sequence_button3;
	uint16_t sequence_button4;
	uint16_t sequence_button5;
	ff8_kernel_status1 status1;
	ff8_kernel_status2 status2;
};

// 24. Duel parameters

struct ff8_kernel_duel_parameters
{
	ff8_kernel_duel_move moves[25];
};

// 25. Rinoa limits part1

struct ff8_kernel_rinoa_limit_part1
{
	uint16_t ability_name_offset;
	uint16_t ability_description_offset;
	ff8_kernel_status_window_flags status_window_flags;
	ff8_kernel_target_info target_info;
	uint8_t unused_ability_data_id;
	uint8_t padding;
};

// 26. Rinoa limits part2

struct ff8_kernel_rinoa_limit_part2
{
	uint16_t limit_name_offset;
	uint16_t attack_animation_id;
	ff8_kernel_attack_type attack_type;
	uint8_t attack_power;
	uint8_t target_hit_animation;
	uint8_t padding;
	ff8_kernel_target_info target_info;
	ff8_kernel_attack_flags attack_flags;
	uint8_t hit_count;
	ff8_kernel_element element_attack;
	uint8_t element_attack_percent;
	uint8_t status_attack_accuracy;
	ff8_kernel_status1 status1;
	ff8_kernel_status2 status2;
};

// 27. Slot array

struct ff8_kernel_slot_array
{
	uint8_t slot_set_id[60];
};

// 28. Slot sets

struct ff8_kernel_slot_set
{
	ff8_kernel_magic_quantity magic[8];
};

// 29. Devour

struct ff8_kernel_devour
{
	uint16_t devour_description_offset;
	uint8_t damage_or_heal_flag;
	uint8_t hp_heal_dmg_quantity_flag;
	ff8_kernel_status2 status2;
	ff8_kernel_status1 status1;
	uint8_t raised_stat_flag;
	uint8_t raised_stat_hp_quantity;
};

// 30. Misc

struct ff8_kernel_misc
{
	uint8_t sleep_timer;
	uint8_t haste_timer;
	uint8_t slow_timer;
	uint8_t stop_timer;
	uint8_t regen_timer;
	uint8_t protect_timer;
	uint8_t shell_timer;
	uint8_t reflect_timer;
	uint8_t aura_timer;
	uint8_t curse_timer;
	uint8_t doom_timer;
	uint8_t invincible_timer;
	uint8_t petrifying_timer;
	uint8_t float_timer;
	uint8_t atb_speed_multiplier;
	uint8_t gilga_angelo_summon_interval;
	uint8_t death_limit_effect;
	uint8_t poison_limit_effect;
	uint8_t petrify_limit_effect;
	uint8_t darkness_limit_effect;
	uint8_t silence_limit_effect;
	uint8_t berserk_limit_effect;
	uint8_t zombie_limit_effect;
	uint8_t unused_status1_bit7_limit_effect;
	uint8_t sleep_limit_effect;
	uint8_t haste_limit_effect;
	uint8_t slow_limit_effect;
	uint8_t stop_limit_effect;
	uint8_t regen_limit_effect;
	uint8_t protect_limit_effect;
	uint8_t shell_limit_effect;
	uint8_t reflect_limit_effect;
	uint8_t aura_limit_effect;
	uint8_t curse_limit_effect;
	uint8_t doom_limit_effect;
	uint8_t invincible_limit_effect;
	uint8_t petrifying_limit_effect;
	uint8_t float_limit_effect;
	uint8_t confusion_limit_effect;
	uint8_t drain_limit_effect;
	uint8_t eject_limit_effect;
	uint8_t double_limit_effect;
	uint8_t triple_limit_effect;
	uint8_t defend_limit_effect;
	uint8_t immune_physical_limit_effect;
	uint8_t immune_magic_limit_effect;
	uint8_t charged_limit_effect;
	uint8_t back_attack_limit_effect;
	uint8_t duel_duration_crisis_1;
	uint8_t duel_initial_sequence_crisis_1;
	uint8_t duel_duration_crisis_2;
	uint8_t duel_initial_sequence_crisis_2;
	uint8_t duel_duration_crisis_3;
	uint8_t duel_initial_sequence_crisis_3;
	uint8_t duel_duration_crisis_4;
	uint8_t duel_initial_sequence_crisis_4;
	uint8_t shot_duration_crisis_1;
	uint8_t shot_duration_crisis_2;
	uint8_t shot_duration_crisis_3;
	uint8_t shot_duration_crisis_4;
};

// 31. Misc text offsets

struct ff8_kernel_misc_text_offset
{
	uint16_t text_offset;
};

// Common initial sequence of every eight-byte learned-ability record.
struct ff8_kernel_ability_header
{
	uint16_t name_offset;
	uint16_t description_offset;
	uint8_t ap_required;
};

union ff8_kernel_ability
{
	ff8_kernel_ability_header common;
	ff8_kernel_junction_ability junction;
	ff8_kernel_command_ability command;
	ff8_kernel_stat_percentage_ability stat_percentage;
	ff8_kernel_character_ability character;
	ff8_kernel_party_ability party;
	ff8_kernel_gf_ability gf;
	ff8_kernel_menu_ability menu;
};

#pragma pack(pop)

static_assert(sizeof(ff8_kernel_gf_learning_slot) == 0x4);
static_assert(sizeof(ff8_kernel_junction_flags) == 0x3);
static_assert(sizeof(ff8_kernel_character_flags) == 0x3);
static_assert(sizeof(ff8_kernel_duel_move) == 0x4);
static_assert(sizeof(ff8_kernel_magic_quantity) == 0x2);
static_assert(sizeof(ff8_kernel_battle_command) == 0x8);
static_assert(sizeof(ff8_kernel_magic) == 0x3C);
static_assert(sizeof(ff8_kernel_junctionable_gf) == 0x84);
static_assert(sizeof(ff8_kernel_enemy_attack) == 0x14);
static_assert(sizeof(ff8_kernel_weapon) == 0xC);
static_assert(sizeof(ff8_kernel_renzokuken_finisher) == 0x18);
static_assert(sizeof(ff8_kernel_character) == 0x24);
static_assert(sizeof(ff8_kernel_battle_item) == 0x18);
static_assert(sizeof(ff8_kernel_non_battle_item) == 0x4);
static_assert(sizeof(ff8_kernel_non_junctionable_gf) == 0x14);
static_assert(sizeof(ff8_kernel_command_ability_data) == 0x10);
static_assert(sizeof(ff8_kernel_junction_ability) == 0x8);
static_assert(sizeof(ff8_kernel_command_ability) == 0x8);
static_assert(sizeof(ff8_kernel_stat_percentage_ability) == 0x8);
static_assert(sizeof(ff8_kernel_character_ability) == 0x8);
static_assert(sizeof(ff8_kernel_party_ability) == 0x8);
static_assert(sizeof(ff8_kernel_gf_ability) == 0x8);
static_assert(sizeof(ff8_kernel_menu_ability) == 0x8);
static_assert(sizeof(ff8_kernel_temporary_character_limit) == 0x18);
static_assert(sizeof(ff8_kernel_blue_magic) == 0x10);
static_assert(sizeof(ff8_kernel_blue_magic_parameter) == 0x8);
static_assert(sizeof(ff8_kernel_shot) == 0x18);
static_assert(sizeof(ff8_kernel_duel) == 0x20);
static_assert(sizeof(ff8_kernel_duel_parameters) == 0x64);
static_assert(sizeof(ff8_kernel_rinoa_limit_part1) == 0x8);
static_assert(sizeof(ff8_kernel_rinoa_limit_part2) == 0x14);
static_assert(sizeof(ff8_kernel_slot_array) == 0x3C);
static_assert(sizeof(ff8_kernel_slot_set) == 0x10);
static_assert(sizeof(ff8_kernel_devour) == 0xC);
static_assert(sizeof(ff8_kernel_misc) == 0x3C);
static_assert(sizeof(ff8_kernel_misc_text_offset) == 0x2);
static_assert(sizeof(ff8_kernel_ability) == 8);
static_assert(offsetof(ff8_kernel_renzokuken_finisher, status1) == 0x12);
static_assert(offsetof(ff8_kernel_renzokuken_finisher, status2) == 0x14);
static_assert(offsetof(ff8_kernel_junctionable_gf, ability_data) == 0x1B);

// Header offset indices; add one for the wiki/editor section number.
enum class ff8_kernel_section : uint32_t
{
	battle_commands,
	magic,
	junctionable_gfs,
	enemy_attacks,
	weapons,
	renzokuken_finishers,
	characters,
	battle_items,
	non_battle_items,
	non_junctionable_gfs,
	command_ability_data,
	junction_abilities,
	command_abilities,
	stat_percentage_abilities,
	character_abilities,
	party_abilities,
	gf_abilities,
	menu_abilities,
	temporary_character_limits,
	blue_magic,
	blue_magic_parameters,
	shot,
	duel,
	duel_parameters,
	rinoa_limits_part1,
	rinoa_limits_part2,
	slot_array,
	slot_sets,
	devour,
	misc,
	misc_text_offsets,
	battle_command_text,
	magic_text,
	junctionable_gf_text,
	enemy_attack_text,
	weapon_text,
	renzokuken_text,
	character_names,
	battle_item_text,
	non_battle_item_text,
	non_junctionable_gf_text,
	junction_ability_text,
	command_ability_text,
	stat_percentage_text,
	character_ability_text,
	party_ability_text,
	gf_ability_text,
	menu_ability_text,
	temporary_character_limit_text,
	blue_magic_text,
	shot_text,
	duel_text,
	rinoa_limit_part1_text,
	rinoa_limit_part2_text,
	devour_text,
	misc_text,
	count,
};

struct ff8_kernel_header
{
	uint32_t section_count;
	uint32_t section_offsets[56];
};
static_assert(sizeof(ff8_kernel_header) == 0xE4);

// Only the executable compatibility image has fixed-size tables. File reads use its header.
#pragma pack(push, 1)
struct ff8_kernel_vanilla_data
{
	ff8_kernel_header header;
	ff8_kernel_battle_command battle_commands[39];
	ff8_kernel_magic magic[57];
	ff8_kernel_junctionable_gf junctionable_gfs[16];
	ff8_kernel_enemy_attack enemy_attacks[384];
	ff8_kernel_weapon weapons[33];
	ff8_kernel_renzokuken_finisher renzokuken_finishers[4];
	ff8_kernel_character characters[11];
	ff8_kernel_battle_item battle_items[33];
	ff8_kernel_non_battle_item non_battle_items[166];
	ff8_kernel_non_junctionable_gf non_junctionable_gfs[16];
	ff8_kernel_command_ability_data command_ability_data[12];
	ff8_kernel_junction_ability junction_abilities[20];
	ff8_kernel_command_ability command_abilities[19];
	ff8_kernel_stat_percentage_ability stat_percentage_abilities[19];
	ff8_kernel_character_ability character_abilities[20];
	ff8_kernel_party_ability party_abilities[5];
	ff8_kernel_gf_ability gf_abilities[9];
	ff8_kernel_menu_ability menu_abilities[24];
	ff8_kernel_temporary_character_limit temporary_character_limits[5];
	ff8_kernel_blue_magic blue_magic[16];
	ff8_kernel_blue_magic_parameter blue_magic_parameters[64];
	ff8_kernel_shot shot[8];
	ff8_kernel_duel duel[10];
	ff8_kernel_duel_parameters duel_parameters[1];
	ff8_kernel_rinoa_limit_part1 rinoa_limits_part1[2];
	ff8_kernel_rinoa_limit_part2 rinoa_limits_part2[5];
	ff8_kernel_slot_array slot_array[1];
	ff8_kernel_slot_set slot_sets[16];
	ff8_kernel_devour devour[16];
	ff8_kernel_misc misc[1];
	ff8_kernel_misc_text_offset misc_text_offsets[128];
};
#pragma pack(pop)
static_assert(sizeof(ff8_kernel_vanilla_data) == 0x4E08);

// Derived from the mapped compatibility image, rather than a second list of numeric offsets.
inline constexpr uint32_t ff8_kernel_vanilla_offsets[] = {
	offsetof(ff8_kernel_vanilla_data, battle_commands),
	offsetof(ff8_kernel_vanilla_data, magic),
	offsetof(ff8_kernel_vanilla_data, junctionable_gfs),
	offsetof(ff8_kernel_vanilla_data, enemy_attacks),
	offsetof(ff8_kernel_vanilla_data, weapons),
	offsetof(ff8_kernel_vanilla_data, renzokuken_finishers),
	offsetof(ff8_kernel_vanilla_data, characters),
	offsetof(ff8_kernel_vanilla_data, battle_items),
	offsetof(ff8_kernel_vanilla_data, non_battle_items),
	offsetof(ff8_kernel_vanilla_data, non_junctionable_gfs),
	offsetof(ff8_kernel_vanilla_data, command_ability_data),
	offsetof(ff8_kernel_vanilla_data, junction_abilities),
	offsetof(ff8_kernel_vanilla_data, command_abilities),
	offsetof(ff8_kernel_vanilla_data, stat_percentage_abilities),
	offsetof(ff8_kernel_vanilla_data, character_abilities),
	offsetof(ff8_kernel_vanilla_data, party_abilities),
	offsetof(ff8_kernel_vanilla_data, gf_abilities),
	offsetof(ff8_kernel_vanilla_data, menu_abilities),
	offsetof(ff8_kernel_vanilla_data, temporary_character_limits),
	offsetof(ff8_kernel_vanilla_data, blue_magic),
	offsetof(ff8_kernel_vanilla_data, blue_magic_parameters),
	offsetof(ff8_kernel_vanilla_data, shot),
	offsetof(ff8_kernel_vanilla_data, duel),
	offsetof(ff8_kernel_vanilla_data, duel_parameters),
	offsetof(ff8_kernel_vanilla_data, rinoa_limits_part1),
	offsetof(ff8_kernel_vanilla_data, rinoa_limits_part2),
	offsetof(ff8_kernel_vanilla_data, slot_array),
	offsetof(ff8_kernel_vanilla_data, slot_sets),
	offsetof(ff8_kernel_vanilla_data, devour),
	offsetof(ff8_kernel_vanilla_data, misc),
	offsetof(ff8_kernel_vanilla_data, misc_text_offsets),
	sizeof(ff8_kernel_vanilla_data),
};

template<ff8_kernel_section> struct ff8_kernel_section_type;
template<> struct ff8_kernel_section_type<ff8_kernel_section::battle_commands> { using type = ff8_kernel_battle_command; };
template<> struct ff8_kernel_section_type<ff8_kernel_section::magic> { using type = ff8_kernel_magic; };
template<> struct ff8_kernel_section_type<ff8_kernel_section::junctionable_gfs> { using type = ff8_kernel_junctionable_gf; };
template<> struct ff8_kernel_section_type<ff8_kernel_section::enemy_attacks> { using type = ff8_kernel_enemy_attack; };
template<> struct ff8_kernel_section_type<ff8_kernel_section::weapons> { using type = ff8_kernel_weapon; };
template<> struct ff8_kernel_section_type<ff8_kernel_section::renzokuken_finishers> { using type = ff8_kernel_renzokuken_finisher; };
template<> struct ff8_kernel_section_type<ff8_kernel_section::characters> { using type = ff8_kernel_character; };
template<> struct ff8_kernel_section_type<ff8_kernel_section::battle_items> { using type = ff8_kernel_battle_item; };
template<> struct ff8_kernel_section_type<ff8_kernel_section::non_battle_items> { using type = ff8_kernel_non_battle_item; };
template<> struct ff8_kernel_section_type<ff8_kernel_section::non_junctionable_gfs> { using type = ff8_kernel_non_junctionable_gf; };
template<> struct ff8_kernel_section_type<ff8_kernel_section::command_ability_data> { using type = ff8_kernel_command_ability_data; };
template<> struct ff8_kernel_section_type<ff8_kernel_section::junction_abilities> { using type = ff8_kernel_junction_ability; };
template<> struct ff8_kernel_section_type<ff8_kernel_section::command_abilities> { using type = ff8_kernel_command_ability; };
template<> struct ff8_kernel_section_type<ff8_kernel_section::stat_percentage_abilities> { using type = ff8_kernel_stat_percentage_ability; };
template<> struct ff8_kernel_section_type<ff8_kernel_section::character_abilities> { using type = ff8_kernel_character_ability; };
template<> struct ff8_kernel_section_type<ff8_kernel_section::party_abilities> { using type = ff8_kernel_party_ability; };
template<> struct ff8_kernel_section_type<ff8_kernel_section::gf_abilities> { using type = ff8_kernel_gf_ability; };
template<> struct ff8_kernel_section_type<ff8_kernel_section::menu_abilities> { using type = ff8_kernel_menu_ability; };
template<> struct ff8_kernel_section_type<ff8_kernel_section::temporary_character_limits> { using type = ff8_kernel_temporary_character_limit; };
template<> struct ff8_kernel_section_type<ff8_kernel_section::blue_magic> { using type = ff8_kernel_blue_magic; };
template<> struct ff8_kernel_section_type<ff8_kernel_section::blue_magic_parameters> { using type = ff8_kernel_blue_magic_parameter; };
template<> struct ff8_kernel_section_type<ff8_kernel_section::shot> { using type = ff8_kernel_shot; };
template<> struct ff8_kernel_section_type<ff8_kernel_section::duel> { using type = ff8_kernel_duel; };
template<> struct ff8_kernel_section_type<ff8_kernel_section::duel_parameters> { using type = ff8_kernel_duel_parameters; };
template<> struct ff8_kernel_section_type<ff8_kernel_section::rinoa_limits_part1> { using type = ff8_kernel_rinoa_limit_part1; };
template<> struct ff8_kernel_section_type<ff8_kernel_section::rinoa_limits_part2> { using type = ff8_kernel_rinoa_limit_part2; };
template<> struct ff8_kernel_section_type<ff8_kernel_section::slot_array> { using type = ff8_kernel_slot_array; };
template<> struct ff8_kernel_section_type<ff8_kernel_section::slot_sets> { using type = ff8_kernel_slot_set; };
template<> struct ff8_kernel_section_type<ff8_kernel_section::devour> { using type = ff8_kernel_devour; };
template<> struct ff8_kernel_section_type<ff8_kernel_section::misc> { using type = ff8_kernel_misc; };
template<> struct ff8_kernel_section_type<ff8_kernel_section::misc_text_offsets> { using type = ff8_kernel_misc_text_offset; };

// A non-owning view of one validated file. Equal adjacent offsets represent an empty section.
class ff8_kernel_file
{
public:
	ff8_kernel_file(const char *data, uint32_t size);
	bool valid() const { return valid_; }
	const ff8_kernel_header &header() const { return *reinterpret_cast<const ff8_kernel_header *>(data_); }
	std::span<const char> bytes(ff8_kernel_section section) const;

	template<ff8_kernel_section Section>
	std::span<const typename ff8_kernel_section_type<Section>::type> section() const
	{
		using record = typename ff8_kernel_section_type<Section>::type;
		auto data = bytes(Section);
		return { reinterpret_cast<const record *>(data.data()), data.size() / sizeof(record) };
	}

private:
	const char *data_;
	uint32_t size_;
	bool valid_;
};
