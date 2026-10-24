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

// Native item code AddMoreItems relocates, checked against the 2013 executables
// (EN, FR, DE, ES, IT, JP, JP_NV) and the 1.2 US one.
// Offsets are per layout: JP_NV uses JP's, the 1.2 releases their 2013 counterpart's.
// An offset of 0 means the layout has no such code.

enum item_layout { layout_en, layout_fr, layout_de, layout_es, layout_it, layout_jp, layout_count };

enum item_fn : uint8_t
{
	fn_world_car_fuel,
	fn_item_menu,
	fn_shot_window_update,
	fn_add_item,
	fn_execute_command,
	fn_monster_ai_section,
	fn_battle_loop,
	fn_gf_cinematic_tick,
	fn_monster_ai,
	fn_compute_command,
	fn_command_window_init,
	fn_apply_damage,
	fn_shop_commit,
	fn_menu_load_vibrate,
	fn_item_menu_init,
	fn_menu_hosted_async,
	fn_main_menu_init,
	fn_status_menu_init,
	fn_refine_menu_init,
	fn_main_menu_controller,
	fn_junk_shop_init,
	fn_junk_shop_upgrade_list,
	fn_junk_shop_upgrade_menu,
	fn_shop_update,
	fn_shop_init,
	fn_world_director,
	fn_script_has_item,
	fn_item_name,
	fn_item_description,
	fn_shot_result,
	fn_remove_one_item,
	fn_pick_confused_action,
	fn_random_battle_item,
	fn_battle_item_amount,
	fn_remove_saved_item,
	fn_battle_exit_writeback,
	fn_change_battle_item_amount,
	fn_anim_seq_apply_all,
	fn_apply_result_to_targets,
	fn_apply_result_to_target,
	fn_battle_result_update,
	fn_battle_items_to_inventory,
	fn_has_battle_item,
	fn_update_battle_items,
	fn_build_battle_items,
	fn_item_command_amount,
	fn_command_window_update,
	fn_command_window_width,
	fn_build_limit_menu,
	fn_reserve_battle_order,
	fn_menu_get_file,
	fn_execute_menu_command,
	fn_menu_read_into_buffer,
	fn_shot_ammo_mask,
	fn_clear_empty_slots,
	fn_battle_item_rows,
	fn_status_ammo_counts,
	fn_refine_menu_update,
	fn_junk_shop_draw,
	fn_shop_stage_inventory,
	fn_shop_draw,
	fn_shop_draw_owned,
	fn_shop_draw_list,
	fn_shop_draw_rows,
	fn_shop_prices,
	fn_battle_order_edit_init,
	fn_open_magic_window,
	fn_list_window_update,
	fn_list_window_init,
	fn_world_vehicle_board,
	fn_world_vehicle_steering,
	fn_world_find_fuel,
	fn_has_item,
	fn_jp_shop_row_item,
	fn_count
};

struct item_offsets { uint16_t at[layout_count]; };
constexpr item_offsets same(uint16_t offset) { return { { offset, offset, offset, offset, offset, offset } }; }

static void item_find_roots(uint32_t *fn)
{
	fn[fn_world_car_fuel] = uint32_t(ff8_externals.sub_54FDA0);
	fn[fn_item_menu] = uint32_t(ff8_externals.menu_use_items_sub_4F81F0);
	fn[fn_shot_window_update] = uint32_t(ff8_externals.sub_4AD8D0);
	fn[fn_add_item] = uint32_t(ff8_externals.add_item_to_player_sub_47ED00);
	fn[fn_execute_command] = uint32_t(ff8_externals.sub_485610);
	fn[fn_monster_ai_section] = uint32_t(ff8_externals.battle_sub_4877F0);
	fn[fn_battle_loop] = uint32_t(ff8_externals.loc_47D490);
	fn[fn_gf_cinematic_tick] = uint32_t(ff8_externals.sub_50B2A0);
	fn[fn_monster_ai] = uint32_t(ff8_externals.battle_ai_opcode_sub_487DF0);
	fn[fn_compute_command] = uint32_t(ff8_externals.battle_sub_48D200);
	fn[fn_command_window_init] = uint32_t(ff8_externals.sub_4BCBE0);
	fn[fn_apply_damage] = uint32_t(ff8_externals.battle_sub_48FE20);
	fn[fn_shop_commit] = uint32_t(ff8_externals.menu_shop_update_gil_and_items_4EB9F0);
	fn[fn_menu_load_vibrate] = uint32_t(ff8_externals.open_battle_vibrate_vib);
	fn[fn_item_menu_init] = uint32_t(ff8_externals.menu_callbacks[2].func);
	fn[fn_menu_hosted_async] = uint32_t(ff8_externals.sub_4B3310);
	fn[fn_main_menu_init] = uint32_t(ff8_externals.menu_callbacks[1].func);
	fn[fn_status_menu_init] = uint32_t(ff8_externals.menu_callbacks[5].func);
	fn[fn_refine_menu_init] = uint32_t(ff8_externals.menu_callbacks[19].func);
	fn[fn_main_menu_controller] = uint32_t(ff8_externals.main_menu_controller);
	fn[fn_junk_shop_init] = uint32_t(ff8_externals.menu_callbacks[12].func);
	fn[fn_junk_shop_upgrade_list] = uint32_t(ff8_externals.sub_4EA770);
	fn[fn_junk_shop_upgrade_menu] = uint32_t(ff8_externals.menu_junkshop_sub_4EA890);
	fn[fn_shop_update] = uint32_t(ff8_externals.menu_shop_sub_4EBE40);
	fn[fn_shop_init] = uint32_t(ff8_externals.menu_callbacks[11].func);
	fn[fn_world_director] = uint32_t(ff8_externals.worldmap_with_fog_sub_53FAC0);
	fn[fn_script_has_item] = common_externals.execute_opcode_table[0x16A];
}

// fn is the target of the call (or the absolute value) at the offset inside from.
struct item_fn_ref { item_fn fn, from; bool call; item_offsets offset; };
static const item_fn_ref item_fn_refs[] = {
	{ fn_item_name, fn_world_car_fuel, true, { 0x102, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF } },
	{ fn_item_description, fn_item_menu, true, same(0x17F) },
	{ fn_shot_result, fn_shot_window_update, true, same(0x1CC) },
	{ fn_remove_one_item, fn_shot_result, true, same(0x3B) },
	{ fn_pick_confused_action, fn_execute_command, true, same(0x2E7) },
	{ fn_random_battle_item, fn_pick_confused_action, true, same(0x1C6) },
	{ fn_battle_item_amount, fn_monster_ai_section, true, same(0x27F) },
	{ fn_remove_saved_item, fn_monster_ai_section, true, same(0x291) },
	{ fn_battle_exit_writeback, fn_battle_loop, true, same(0x1F) },
	{ fn_change_battle_item_amount, fn_execute_command, true, same(0x54D) },
	{ fn_anim_seq_apply_all, fn_gf_cinematic_tick, true, same(0x51E) },
	{ fn_apply_result_to_targets, fn_anim_seq_apply_all, true, same(0xF) },
	{ fn_apply_result_to_target, fn_apply_result_to_targets, true, same(0x14) },
	{ fn_battle_result_update, fn_apply_result_to_target, true, same(0x1B) },
	{ fn_battle_items_to_inventory, fn_battle_result_update, true, same(0x17E) },
	{ fn_has_battle_item, fn_monster_ai_section, true, same(0x23A) },
	{ fn_update_battle_items, fn_change_battle_item_amount, true, same(0x75) },
	{ fn_build_battle_items, fn_battle_result_update, true, same(0x19C) },
	{ fn_item_command_amount, fn_compute_command, true, { 0xA1C, 0x9F0, 0x9F7, 0xA1A, 0xA44, 0x9D9 } },
	{ fn_command_window_update, fn_command_window_init, false, same(0xF) },
	{ fn_command_window_width, fn_command_window_update, true, same(0x264) },
	{ fn_build_limit_menu, fn_command_window_width, true, same(0x10) },
	{ fn_reserve_battle_order, fn_shop_commit, true, same(0x76) },
	{ fn_menu_get_file, fn_menu_load_vibrate, true, same(0xA) },
	{ fn_execute_menu_command, fn_command_window_update, true, same(0x462) },
	{ fn_menu_read_into_buffer, fn_item_menu_init, true, same(0x23) },
	{ fn_shot_ammo_mask, fn_menu_hosted_async, true, same(0x40) },
	{ fn_clear_empty_slots, fn_item_menu, true, { 0x329D, 0x329D, 0x329D, 0x329D, 0x329D, 0x30A7 } },
	{ fn_battle_item_rows, fn_execute_menu_command, false, same(0x1A5) },
	{ fn_status_ammo_counts, fn_status_menu_init, true, same(0x13) },
	{ fn_refine_menu_update, fn_refine_menu_init, false, same(0xE) },
	{ fn_junk_shop_draw, fn_junk_shop_init, false, same(0xB) },
	{ fn_shop_stage_inventory, fn_junk_shop_init, true, same(0xC9) },
	{ fn_shop_draw, fn_shop_init, false, same(0x34) },
	{ fn_shop_draw_owned, fn_shop_draw, true, { 0x241, 0x241, 0x241, 0x241, 0x241, 0x295 } },
	{ fn_shop_draw_list, fn_shop_draw, true, { 0x26B, 0x26B, 0x26B, 0x26B, 0x26B, 0x2BF } },
	{ fn_shop_draw_rows, fn_shop_draw_list, false, same(0x106) },
	{ fn_shop_prices, fn_shop_init, true, same(0x1E6) },
	{ fn_battle_order_edit_init, fn_item_menu, true, { 0x151D, 0x151D, 0x151D, 0x151D, 0x151D, 0x1514 } },
	{ fn_open_magic_window, fn_execute_menu_command, true, same(0x197) },
	{ fn_list_window_update, fn_open_magic_window, false, same(0x28) },
	{ fn_list_window_init, fn_open_magic_window, true, same(0x18) },
	{ fn_world_vehicle_board, fn_world_director, true, { 0x28A, 0x28B, 0x28B, 0x28B, 0x28B, 0x28B } },
	{ fn_world_vehicle_steering, fn_world_director, true, { 0xF4, 0xF2, 0xF2, 0xF2, 0xF2, 0xF2 } },
	{ fn_world_find_fuel, fn_world_vehicle_steering, true, { 0x570, 0x583, 0x583, 0x583, 0x583, 0x583 } },
	{ fn_has_item, fn_script_has_item, true, same(0x1A) },
	{ fn_jp_shop_row_item, fn_shop_draw, true, { 0x0, 0x0, 0x0, 0x0, 0x0, 0x1F1 } },
};

// native: the operand's value relative to the native array (or the constant itself);
// value: what it becomes, relative to the expanded array.
struct item_site { item_fn fn; item_offsets offset; uint8_t width; item_target target; uint32_t native, value; };
static const item_site item_sites[] = {
	{ fn_remove_one_item, same(0x9), 4, item_target::inventory, 0x1, 0x1 },
	{ fn_remove_one_item, same(0x1C), 4, item_target::inventory, 0x18D, 0x1FF },
	{ fn_add_item, same(0x10), 4, item_target::inventory, 0x0, 0x0 },
	{ fn_add_item, same(0x24), 4, item_target::constant, 0xC6, 0xFF },
	{ fn_add_item, same(0x2B), 4, item_target::inventory, 0x0, 0x0 },
	{ fn_add_item, same(0x3C), 4, item_target::constant, 0xC6, 0xFF },
	{ fn_pick_confused_action, same(0x1F4), 4, item_target::battle_items, 0x9, 0x9 },
	{ fn_pick_confused_action, same(0x240), 4, item_target::battle_items, 0x9, 0x9 },
	{ fn_pick_confused_action, same(0x26F), 4, item_target::battle_items, 0x9, 0x9 },
	{ fn_pick_confused_action, same(0x2BB), 4, item_target::battle_items, 0x9, 0x9 },
	{ fn_execute_command, same(0x59A), 4, item_target::live_items, 0x0, 0x0 },
	{ fn_execute_command, same(0x5AA), 4, item_target::live_items, 0xA0, 0x1E0 },
	{ fn_execute_command, same(0x5B6), 4, item_target::live_items, 0x1, 0x1 },
	{ fn_execute_command, same(0x5C5), 4, item_target::inventory, 0x0, 0x0 },
	{ fn_execute_command, same(0x5DB), 4, item_target::constant, 0xC6, 0xFF },
	{ fn_battle_item_amount, same(0x8), 4, item_target::live_items, 0x0, 0x0 },
	{ fn_battle_item_amount, same(0x19), 4, item_target::live_items, 0xA0, 0x1E0 },
	{ fn_battle_item_amount, same(0x27), 4, item_target::live_items, 0x1, 0x1 },
	{ fn_remove_saved_item, same(0x6), 4, item_target::inventory, 0x0, 0x0 },
	{ fn_remove_saved_item, same(0x1A), 4, item_target::constant, 0xC6, 0xFF },
	{ fn_battle_exit_writeback, same(0x68), 4, item_target::live_items, 0x0, 0x0 },
	{ fn_battle_exit_writeback, same(0x79), 4, item_target::inventory, 0x0, 0x0 },
	{ fn_battle_exit_writeback, same(0x8D), 4, item_target::constant, 0xC6, 0xFF },
	{ fn_battle_exit_writeback, same(0x94), 4, item_target::inventory, 0x0, 0x0 },
	{ fn_battle_exit_writeback, same(0xA5), 4, item_target::constant, 0xC6, 0xFF },
	{ fn_battle_exit_writeback, same(0xB7), 4, item_target::live_items, 0xA0, 0x1E0 },
	{ fn_change_battle_item_amount, same(0x18), 4, item_target::live_items, 0x0, 0x0 },
	{ fn_change_battle_item_amount, same(0x29), 4, item_target::live_items, 0xA0, 0x1E0 },
	{ fn_change_battle_item_amount, same(0x32), 4, item_target::live_items, 0x0, 0x0 },
	{ fn_change_battle_item_amount, same(0x40), 4, item_target::live_items, 0xA0, 0x1E0 },
	{ fn_change_battle_item_amount, same(0x50), 4, item_target::live_items, 0x1, 0x1 },
	{ fn_change_battle_item_amount, same(0x5D), 4, item_target::live_items, 0x1, 0x1 },
	{ fn_change_battle_item_amount, same(0x68), 4, item_target::live_items, 0x1, 0x1 },
	{ fn_change_battle_item_amount, same(0x6F), 4, item_target::live_items, 0x0, 0x0 },
	{ fn_change_battle_item_amount, same(0x81), 4, item_target::live_items, 0x0, 0x0 },
	{ fn_change_battle_item_amount, same(0x92), 4, item_target::live_items, 0xA0, 0x1E0 },
	{ fn_change_battle_item_amount, same(0xA2), 4, item_target::live_items, 0x1, 0x1 },
	{ fn_change_battle_item_amount, same(0xAF), 4, item_target::live_items, 0x1, 0x1 },
	{ fn_change_battle_item_amount, same(0xB8), 4, item_target::live_items, 0x0, 0x0 },
	{ fn_battle_items_to_inventory, same(0x4), 4, item_target::live_items, 0x0, 0x0 },
	{ fn_battle_items_to_inventory, same(0x15), 4, item_target::inventory, 0x0, 0x0 },
	{ fn_battle_items_to_inventory, same(0x29), 4, item_target::constant, 0xC6, 0xFF },
	{ fn_battle_items_to_inventory, same(0x30), 4, item_target::inventory, 0x0, 0x0 },
	{ fn_battle_items_to_inventory, same(0x41), 4, item_target::constant, 0xC6, 0xFF },
	{ fn_battle_items_to_inventory, same(0x53), 4, item_target::live_items, 0xA0, 0x1E0 },
	{ fn_monster_ai_section, same(0x225), 4, item_target::live_items, 0x0, 0x0 },
	{ fn_monster_ai_section, same(0x232), 4, item_target::live_items, 0xA0, 0x1E0 },
	{ fn_has_battle_item, same(0x5), 4, item_target::live_items, 0x0, 0x0 },
	{ fn_has_battle_item, same(0x15), 4, item_target::live_items, 0xA0, 0x1E0 },
	{ fn_monster_ai, { 0x2186, 0x2190, 0x218B, 0x21BA, 0x21A6, 0x2162 }, 4, item_target::battle_items, 0x9, 0x9 },
	{ fn_item_command_amount, same(0x8), 4, item_target::inventory, 0x0, 0x0 },
	{ fn_item_command_amount, same(0x19), 4, item_target::inventory, 0x18C, 0x1FE },
	{ fn_item_command_amount, same(0x29), 4, item_target::inventory, 0x1, 0x1 },
	{ fn_build_limit_menu, same(0x8F), 4, item_target::inventory, 0x0, 0x0 },
	{ fn_build_limit_menu, same(0xA1), 4, item_target::inventory, 0x18C, 0x1FE },
	{ fn_build_limit_menu, same(0xF0), 4, item_target::inventory, 0x1, 0x1 },
	{ fn_compute_command, { 0x6A, 0x6C, 0x6C, 0x6C, 0x6C, 0x6C }, 4, item_target::battle_items, 0x6, 0x6 },
	{ fn_compute_command, { 0x102, 0x103, 0x103, 0x103, 0x103, 0x11D }, 4, item_target::battle_items, 0x4, 0x4 },
	{ fn_compute_command, { 0x960, 0x939, 0x93B, 0x95E, 0x988, 0x922 }, 4, item_target::battle_items, 0x16, 0x16 },
	{ fn_compute_command, { 0x96B, 0x940, 0x946, 0x969, 0x993, 0x929 }, 4, item_target::battle_items, 0x4, 0x4 },
	{ fn_apply_damage, same(0x441), 4, item_target::battle_items, 0x14, 0x14 },
	{ fn_apply_damage, same(0x44C), 4, item_target::battle_items, 0x17, 0x17 },
	{ fn_apply_damage, same(0x457), 4, item_target::battle_items, 0xD, 0xD },
	{ fn_apply_damage, same(0x463), 4, item_target::battle_items, 0xE, 0xE },
	{ fn_apply_damage, same(0x46F), 4, item_target::battle_items, 0x10, 0x10 },
	{ fn_apply_damage, { 0xE19, 0xD7E, 0xDB5, 0xD88, 0xD9E, 0xD7D }, 4, item_target::battle_items, 0xA, 0xA },
	{ fn_apply_damage, { 0xE34, 0xD99, 0xDD0, 0xDA3, 0xDB9, 0xD98 }, 4, item_target::battle_items, 0xB, 0xB },
	{ fn_apply_damage, { 0xE42, 0xDA7, 0xDDE, 0xDB1, 0xDC7, 0xDA6 }, 4, item_target::battle_items, 0x7, 0x7 },
	{ fn_apply_damage, { 0xE4A, 0xDAF, 0xDE6, 0xDB9, 0xDCF, 0xDAE }, 4, item_target::battle_items, 0x6, 0x6 },
	{ fn_execute_menu_command, same(0x1AC), 1, item_target::constant, 0x20, 0x60 },
	{ fn_shot_ammo_mask, same(0x2B), 4, item_target::inventory, 0x0, 0x0 },
	{ fn_shot_ammo_mask, same(0x30), 4, item_target::constant, 0xC6, 0xFF },
	{ fn_clear_empty_slots, same(0x3), 4, item_target::inventory, 0x0, 0x0 },
	{ fn_clear_empty_slots, same(0x8), 4, item_target::constant, 0xC6, 0xFF },
	{ fn_battle_item_rows, same(0x1), 4, item_target::live_items, 0x0, 0x0 },
	{ fn_status_ammo_counts, same(0x40), 4, item_target::inventory, 0x0, 0x0 },
	{ fn_status_ammo_counts, same(0x45), 4, item_target::constant, 0xC6, 0xFF },
	{ fn_refine_menu_init, same(0xFD), 4, item_target::inventory, 0x0, 0x0 },
	{ fn_refine_menu_init, same(0x129), 1, item_target::constant, 0x12, 0x18 },
	{ fn_refine_menu_init, same(0x153), 1, item_target::constant, 0x12, 0x18 },
	{ fn_refine_menu_init, same(0x17D), 1, item_target::constant, 0x12, 0x18 },
	{ fn_refine_menu_init, same(0x1A7), 1, item_target::constant, 0x12, 0x18 },
	{ fn_refine_menu_update, same(0x6AD), 4, item_target::constant, 0xC6, 0xFF },
	{ fn_refine_menu_update, same(0x6F6), 4, item_target::constant, 0xC6, 0xFF },
	{ fn_refine_menu_update, same(0x750), 4, item_target::constant, 0xC6, 0xFF },
	{ fn_refine_menu_update, same(0x788), 4, item_target::constant, 0xC6, 0xFF },
	{ fn_refine_menu_update, same(0x848), 4, item_target::constant, 0xC6, 0xFF },
	{ fn_refine_menu_update, same(0x87C), 4, item_target::constant, 0xC6, 0xFF },
	{ fn_refine_menu_update, same(0xC91), 4, item_target::inventory, 0x0, 0x0 },
	{ fn_refine_menu_update, same(0xCAB), 4, item_target::constant, 0xC6, 0xFF },
	{ fn_main_menu_controller, { 0x1D53, 0x1D97, 0x1D97, 0x1D97, 0x1D97, 0x1E60 }, 4, item_target::constant, 0x1400, 0x1E80 },
	{ fn_junk_shop_init, same(0x182), 4, item_target::shop_quantities, 0x0, 0x0 },
	{ fn_junk_shop_upgrade_list, same(0xB3), 4, item_target::shop_quantities, 0x0, 0x0 },
	{ fn_junk_shop_upgrade_menu, { 0x5FF, 0x5FF, 0x5FF, 0x5FF, 0x5FF, 0x62E }, 4, item_target::shop_quantities, 0x0, 0x0 },
	{ fn_junk_shop_draw, same(0x2B0), 4, item_target::shop_quantities, 0x0, 0x0 },
	{ fn_shop_commit, same(0x21), 4, item_target::inventory, 0x0, 0x0 },
	{ fn_shop_commit, same(0x26), 4, item_target::constant, 0xC6, 0xFF },
	{ fn_shop_commit, same(0x32), 4, item_target::shop_quantities, 0x0, 0x0 },
	{ fn_shop_commit, same(0x45), 4, item_target::shop_quantities, 0x0, 0x0 },
	{ fn_shop_commit, same(0x4D), 4, item_target::inventory, 0x0, 0x0 },
	{ fn_shop_commit, same(0x57), 4, item_target::shop_quantities, 0x0, 0x0 },
	{ fn_shop_commit, same(0x81), 4, item_target::constant, 0xC8, 0x100 },
	{ fn_shop_stage_inventory, same(0x19), 4, item_target::constant, 0x32, 0x40 },
	{ fn_shop_stage_inventory, same(0x20), 4, item_target::shop_quantities, 0x0, 0x0 },
	{ fn_shop_stage_inventory, same(0x27), 4, item_target::inventory, 0x0, 0x0 },
	{ fn_shop_stage_inventory, same(0x2C), 4, item_target::constant, 0xC6, 0xFF },
	{ fn_shop_stage_inventory, same(0x42), 4, item_target::shop_quantities, 0x0, 0x0 },
	{ fn_shop_update, same(0x1C3), 4, item_target::shop_quantities, 0x0, 0x0 },
	{ fn_shop_update, same(0x1CF), 4, item_target::constant, 0xC6, 0xFF },
	{ fn_shop_update, same(0x34C), 4, item_target::constant, 0xC6, 0xFF },
	{ fn_shop_update, same(0x36C), 4, item_target::shop_quantities, 0x0, 0x0 },
	{ fn_shop_update, same(0x382), 2, item_target::constant, 0xC6, 0xFF },
	{ fn_shop_update, same(0x4C9), 1, item_target::constant, 0x19, 0x20 },
	{ fn_shop_update, same(0x502), 4, item_target::constant, 0x32, 0x40 },
	{ fn_shop_update, same(0x509), 4, item_target::shop_quantities, 0x0, 0x0 },
	{ fn_shop_update, same(0x50E), 4, item_target::inventory, 0x0, 0x0 },
	{ fn_shop_update, same(0x515), 4, item_target::constant, 0xC6, 0xFF },
	{ fn_shop_update, same(0x52A), 4, item_target::shop_quantities, 0x0, 0x0 },
	{ fn_shop_update, same(0x63A), 4, item_target::constant, 0xC6, 0xFF },
	{ fn_shop_update, same(0x662), 4, item_target::buy_prices, 0x0, 0x0 },
	{ fn_shop_update, same(0x6CF), 4, item_target::shop_quantities, 0x0, 0x0 },
	{ fn_shop_update, same(0x72F), 4, item_target::shop_quantities, 0x0, 0x0 },
	{ fn_shop_update, same(0x946), 4, item_target::constant, 0xC6, 0xFF },
	{ fn_shop_update, same(0x96F), 4, item_target::buy_prices, 0x0, 0x0 },
	{ fn_shop_update, same(0x97E), 4, item_target::shop_quantities, 0x0, 0x0 },
	{ fn_shop_update, same(0x998), 2, item_target::constant, 0xC6, 0xFF },
	{ fn_shop_update, same(0x9C5), 4, item_target::constant, 0xC6, 0xFF },
	{ fn_shop_update, same(0x9EE), 4, item_target::sell_prices, 0x0, 0x0 },
	{ fn_shop_update, same(0xA13), 4, item_target::shop_quantities, 0x0, 0x0 },
	{ fn_shop_update, same(0xA1B), 4, item_target::shop_quantities, 0x0, 0x0 },
	{ fn_shop_update, same(0xD66), 4, item_target::constant, 0xC6, 0xFF },
	{ fn_shop_update, same(0xD81), 4, item_target::shop_quantities, 0x0, 0x0 },
	{ fn_shop_update, same(0xD8B), 4, item_target::constant, 0xC6, 0xFF },
	{ fn_shop_update, same(0x100D), 4, item_target::constant, 0xC6, 0xFF },
	{ fn_shop_update, same(0x1028), 4, item_target::shop_quantities, 0x0, 0x0 },
	{ fn_shop_update, same(0x1032), 4, item_target::constant, 0xC6, 0xFF },
	{ fn_shop_draw, { 0x184, 0x184, 0x184, 0x184, 0x184, 0x0 }, 4, item_target::constant, 0xC6, 0xFF },
	{ fn_shop_draw, { 0x1AE, 0x1AE, 0x1AE, 0x1AE, 0x1AE, 0x203 }, 4, item_target::buy_prices, 0x0, 0x0 },
	{ fn_shop_draw, { 0x1B7, 0x1B7, 0x1B7, 0x1B7, 0x1B7, 0x20C }, 4, item_target::sell_prices, 0x0, 0x0 },
	{ fn_shop_draw_owned, same(0x57), 4, item_target::constant, 0xC6, 0xFF },
	{ fn_shop_draw_owned, same(0x84), 4, item_target::shop_quantities, 0x0, 0x0 },
	{ fn_shop_draw_rows, same(0x38), 4, item_target::constant, 0xC6, 0xFF },
	{ fn_shop_draw_rows, same(0x73), 4, item_target::sell_prices, 0x0, 0x0 },
	{ fn_shop_draw_rows, same(0x7F), 4, item_target::shop_quantities, 0x0, 0x0 },
	{ fn_shop_draw_rows, same(0x85), 4, item_target::constant, 0xC6, 0xFF },
	{ fn_shop_draw_rows, same(0x9B), 4, item_target::buy_prices, 0x0, 0x0 },
	{ fn_shop_draw_rows, same(0xC1), 4, item_target::buy_prices, 0x0, 0x0 },
	{ fn_shop_init, same(0x9A), 4, item_target::inventory, 0x0, 0x0 },
	{ fn_shop_init, same(0xC1), 4, item_target::constant, 0x32, 0x40 },
	{ fn_shop_init, same(0xC8), 4, item_target::shop_quantities, 0x0, 0x0 },
	{ fn_shop_init, same(0xCD), 4, item_target::constant, 0xC6, 0xFF },
	{ fn_shop_init, same(0xE4), 4, item_target::shop_quantities, 0x0, 0x0 },
	{ fn_shop_prices, same(0x15), 4, item_target::sell_prices, 0x0, 0x0 },
	{ fn_shop_prices, same(0x1D), 4, item_target::buy_prices, 0x0, 0x0 },
	{ fn_shop_prices, same(0x30), 4, item_target::constant, 0xC8, 0x100 },
	{ fn_item_menu_init, same(0xE1), 4, item_target::inventory, 0x0, 0x0 },
	{ fn_item_menu_init, same(0xE8), 4, item_target::inventory, 0x0, 0x0 },
	{ fn_battle_order_edit_init, same(0x11), 4, item_target::constant, 0xC6, 0xFF },
	{ fn_item_menu, { 0x96F, 0x96F, 0x96F, 0x96F, 0x96F, 0x969 }, 4, item_target::constant, 0xC6, 0xFF },
	{ fn_item_menu, { 0xEB8, 0xEB8, 0xEB8, 0xEB8, 0xEB8, 0xEAF }, 1, item_target::constant, 0x12, 0x18 },
	{ fn_item_menu, { 0x101F, 0x101F, 0x101F, 0x101F, 0x101F, 0x1016 }, 4, item_target::constant, 0x11, 0x17 },
	{ fn_item_menu, { 0x225B, 0x225B, 0x225B, 0x225B, 0x225B, 0x2134 }, 4, item_target::constant, 0xC6, 0xFF },
	{ fn_item_menu, { 0x387D, 0x387D, 0x387D, 0x387D, 0x387D, 0x3656 }, 4, item_target::constant, 0x11, 0x17 },
	{ fn_item_menu, { 0x3A22, 0x3A22, 0x3A22, 0x3A22, 0x3A22, 0x37FB }, 1, item_target::constant, 0x12, 0x18 },
	{ fn_list_window_update, same(0x523), 4, item_target::list_pending, 0x0, 0x0 },
	{ fn_list_window_update, same(0x590), 4, item_target::list_pending, 0x0, 0x0 },
	{ fn_list_window_update, same(0x596), 4, item_target::list_pending, 0x0, 0x0 },
	{ fn_list_window_update, same(0x677), 4, item_target::list_pending, 0x0, 0x0 },
	{ fn_list_window_update, same(0x74E), 4, item_target::list_pending, 0x0, 0x0 },
	{ fn_list_window_update, same(0x78E), 4, item_target::list_pending, 0x0, 0x0 },
	{ fn_list_window_update, same(0x7F7), 4, item_target::list_pending, 0x0, 0x0 },
	{ fn_list_window_update, same(0x85B), 4, item_target::list_pending, 0x0, 0x0 },
	{ fn_list_window_update, same(0x866), 4, item_target::list_pending, 0x0, 0x0 },
	{ fn_list_window_update, same(0x8FB), 4, item_target::list_pending, 0x0, 0x0 },
	{ fn_list_window_update, same(0x928), 4, item_target::list_pending, 0x0, 0x0 },
	{ fn_list_window_update, same(0x972), 4, item_target::list_pending, 0x0, 0x0 },
	{ fn_list_window_update, same(0x99B), 4, item_target::list_pending, 0x0, 0x0 },
	{ fn_list_window_update, same(0xDC7), 4, item_target::list_pending, 0x0, 0x0 },
	{ fn_list_window_update, same(0xF79), 4, item_target::list_pending, 0x0, 0x0 },
	{ fn_list_window_update, same(0xF7F), 4, item_target::list_pending, 0x0, 0x0 },
	{ fn_list_window_update, same(0x1277), 4, item_target::list_pending, 0x0, 0x0 },
	{ fn_list_window_init, same(0xC9), 4, item_target::list_pending, 0x0, 0x0 },
	{ fn_world_vehicle_board, { 0x52A, 0x52B, 0x52B, 0x52B, 0x52B, 0x52C }, 4, item_target::inventory, 0x0, 0x0 },
	{ fn_world_vehicle_board, { 0x542, 0x543, 0x543, 0x543, 0x543, 0x544 }, 4, item_target::inventory, 0x18C, 0x1FE },
	{ fn_world_car_fuel, { 0x8C, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A }, 4, item_target::inventory, 0x0, 0x0 },
	{ fn_world_car_fuel, { 0xA3, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1 }, 4, item_target::inventory, 0x18C, 0x1FE },
	{ fn_world_car_fuel, { 0xD0, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD }, 4, item_target::inventory, 0x1, 0x1 },
	{ fn_world_car_fuel, { 0xE1, 0xDE, 0xDE, 0xDE, 0xDE, 0xDE }, 4, item_target::inventory, 0x1, 0x1 },
	{ fn_world_car_fuel, { 0xEA, 0xE7, 0xE7, 0xE7, 0xE7, 0xE7 }, 4, item_target::inventory, 0x1, 0x1 },
	{ fn_world_car_fuel, { 0x193, 0x18F, 0x18F, 0x18F, 0x18F, 0x18F }, 4, item_target::inventory, 0x0, 0x0 },
	{ fn_world_car_fuel, { 0x1A9, 0x1A5, 0x1A5, 0x1A5, 0x1A5, 0x1A5 }, 4, item_target::inventory, 0x18C, 0x1FE },
	{ fn_world_find_fuel, { 0xA, 0x7, 0x7, 0x7, 0x7, 0x7 }, 4, item_target::inventory, 0x0, 0x0 },
	{ fn_world_find_fuel, { 0x24, 0x21, 0x21, 0x21, 0x21, 0x21 }, 4, item_target::inventory, 0x18C, 0x1FE },
	{ fn_has_item, same(0x5), 4, item_target::inventory, 0x0, 0x0 },
	{ fn_has_item, same(0x15), 4, item_target::inventory, 0x18C, 0x1FE },
	{ fn_jp_shop_row_item, { 0x0, 0x0, 0x0, 0x0, 0x0, 0x5 }, 4, item_target::constant, 0xC6, 0xFF },
};

struct item_call { item_fn fn; item_offsets offset; };
static const item_call save_image_calls[] = { { fn_main_menu_controller, same(0xB20) }, { fn_main_menu_controller, same(0xB81) } };
static const item_call load_inventory_calls[] = { { fn_main_menu_controller, { 0x1E8E, 0x1ED2, 0x1ED2, 0x1ED2, 0x1ED2, 0x1F8C } } };
static const item_call new_game_calls[] = { { fn_main_menu_controller, { 0x1371, 0x1371, 0x1371, 0x1371, 0x1371, 0x13E9 } } };
static const item_call reload_direct_calls[] = { { fn_main_menu_init, same(0x9C) }, { fn_menu_read_into_buffer, same(0x3D) } };
static const item_call item_navigation_source_calls[] = { { fn_item_menu, same(0x12B) }, { fn_item_menu, { 0x36F0, 0x36F0, 0x36F0, 0x36F0, 0x36F0, 0x34C9 } } };
static const item_call item_navigation_target_calls[] = { { fn_item_menu, { 0xA60, 0xA60, 0xA60, 0xA60, 0xA60, 0xA5A } } };
static const item_call load_asset_calls[] = { { fn_menu_get_file, same(0x2C) } };
static const item_call random_calls[] = { { fn_random_battle_item, same(0x42) } };

