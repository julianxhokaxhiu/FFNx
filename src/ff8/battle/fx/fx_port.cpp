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

// Native battle effects: registry

#include "fx_port.h"

namespace ff8fx
{
	struct Port { uint32_t orig; void *port; const char *name; int effect_id; uint8_t saved[5]; };

	static const int PORTS_MAX = 16384;
	static Port g_ports[PORTS_MAX];
	static int g_nports = 0;
	static const int INDEX_SIZE = 32768;
	static int16_t g_index[INDEX_SIZE];
	static bool g_index_ready = false;

	bool g_active = false;

	static inline uint32_t slot_of(uint32_t orig) { return (orig * 2654435761u) >> 17; }

	void register_port(uint32_t orig, void *port, const char *name, int effect_id)
	{
		if (!g_index_ready)
		{
			for (int i = 0; i < INDEX_SIZE; i++) g_index[i] = -1;
			g_index_ready = true;
		}
		if (g_nports >= PORTS_MAX) return;
		g_ports[g_nports] = { orig, port, name, effect_id };
		uint32_t h = slot_of(orig);
		while (g_index[h & (INDEX_SIZE - 1)] >= 0) h++;
		g_index[h & (INDEX_SIZE - 1)] = (int16_t)g_nports;
		g_nports++;
	}

	static const Port *find(uint32_t orig)
	{
		if (!g_index_ready) return nullptr;
		for (uint32_t h = slot_of(orig);; h++)
		{
			int i = g_index[h & (INDEX_SIZE - 1)];
			if (i < 0) return nullptr;
			if (g_ports[i].orig == orig) return &g_ports[i];
		}
	}

	// Ported task functions jump to their port, so the game's own executor runs them
	void set_ports(bool on)
	{
		if (on == g_active) return;
		for (int i = 0; i < g_nports; i++)
		{
			Port &p = g_ports[i];
			if (find(p.orig) != &p) continue;
			if (on)
			{
				ff8fx::copy<5>(p.saved, (const void *)p.orig);
				uint8_t jump[5] = { 0xE9 };
				*(int32_t *)(jump + 1) = (int32_t)((uint32_t)p.port - p.orig - 5);
				memcpy_code(p.orig, jump, 5);
			}
			else memcpy_code(p.orig, p.saved, 5);
		}
		g_active = on;
	}

	const char *port_name(uint32_t orig)
	{
		const Port *p = find(orig);
		return p ? p->name : nullptr;
	}

	static int canonical_effect(int effect_id)
	{
		if (effect_id >= 328 && effect_id <= 330) return 327;
		return effect_id;
	}

	bool module_ported(int effect_id)
	{
		effect_id = canonical_effect(effect_id);
		for (int i = 0; i < g_nports; i++)
			if (g_ports[i].effect_id == effect_id) return true;
		return false;
	}

	void register_all()
	{
		static bool done = false;
		if (done) return;
		done = true;
		register_mag069_griever();
		register_mag116_quezacotl();
		register_mag140_phoenix();
		register_mag185_shiva();
		register_mag187_odin();
		register_mag199_cactuar();
		register_mag278_carbuncle();
		register_mag291_pandemona();
		register_mag325_diablos();
		register_mag326_odin_reverse();
		register_mag338_moomba();
		register_mag095_siren();
		register_mag096_minimog();
		register_mag090_tonberry();
		register_mag097_boko();
		register_mag002_fire();
		register_mag142_fira();
		register_mag143_firaga();
		register_mag003_thunder();
		register_mag102_thundara();
		register_mag105_thundaga();
		register_mag144_blizzard();
		register_mag103_blizzara();
		register_mag104_blizzaga();
		register_mag022_bio();
		register_mag038_quake();
		register_mag149_ultima();
		register_mag175_holy();
		register_mag034_pain();
		register_mag148_meltdown();
		register_mag039_drain();
		register_mag014_death();
		register_mag036_confuse();
		register_mag124_flare();
		register_mag145_sleep();
		register_mag146_tornado();
		register_mag107_demi();
		register_mag109_dispel();
		register_mag111_aura();
		register_mag118_aero();
		register_mag123_slow();
		register_mag119_stop();
		register_mag125_haste();
		register_mag115_float();
		register_mag114_zombie();
		register_mag117_break();
		register_mag108_berserk();
		register_mag122_silence();
		register_mag121_blind();
		register_mag147_regen();
		register_mag024_esuna();
		register_mag032_protect();
		register_mag033_shell();
		register_fx_glint();
		register_fx_cast();
		register_fx_sprite();
		register_mag035_life();
		register_mag027_fulllife();
		register_mag028_curaga();
		register_mag106_reflect();
		register_mag025_cura();
		register_mag001_cure();
		register_mag004_double();
		register_mag021_triple();
		register_mag040_scan();
		register_mag222_water();
		register_mag223_meteor();
		register_mag120_petrify_stare();
		register_mag152_ultrasonic_waves();
		register_mag127_petrify_stare();
		register_mag029_wind_blast();
		register_mag235_breath();
		register_mag231_disease_breath();
		register_mag233_earthquake();
		register_mag026_clash();
		register_mag010_ray_bomb();
		register_mag011_storm_breath();
		register_mag155_sand_storm();
		register_mag168_sleeping_gas();
		register_mag154_ultra_waves();
		register_mag180_suicide();
		register_mag169_gastric_juice();
		register_mag126_electric_discharge();
		register_mag153_sticky_web();
		register_mag013_dark_mist();
		register_mag232_breath_of_death();
		register_mag234_fart();
		register_mag030_counter_laser_eye();
		register_mag041_dribble();
		register_mag190_chain_gun();
		register_mag048_magma_breath();
		register_mag008_doom();
		register_mag009_doom_activation();
		register_mag012_blade_shot();
		register_mag015_draw();
		register_mag037_drink_magic();
		register_mag019_raldo_throw();
		register_mag020_norg_pod_opening();
		register_mag023_psycho_blast();
		register_mag031_heartbreak();
		register_mag042_shoot();
		register_mag043_melting_bubble();
		register_mag044_junk();
		register_mag045_stare();
		register_mag046_sigh();
		register_mag047_curse();
		register_mag049_resonance();
		register_mag054_everyones_grudge();
		register_mag055_aqua_breath();
		register_mag060_raijin_attack();
		register_mag063_helix_attack();
		register_mag064_griever_tail();
		register_mag065_great_attractor();
		register_mag066_griever_ultimecia_death();
		register_mag068_scan();
		register_mag075_hells_judgement();
		register_mag070_shockwave_pulsar();
		register_mag076_ultimecia_final_form_spawn();
		register_mag078_mighty_guard();
		register_mag077_final_form_death();
		register_mag079_ultimecia_final_death();
		register_mag080_ultimecia_junction();
		register_mag081_ultimecia_unk4();
		register_mag082_ultimecia_blow_away();
		register_mag083_ultimecia_absorb();
		register_mag101_trauma_grab();
		register_mag113_bad_breath();
		register_mag128_gastric_juice();
		register_mag129_breath();
		register_mag130_eerie_sound_wave();
		register_mag131_bad_breath();
		register_mag132_dissolving_acid();
		register_mag133_hypnotize();
		register_mag134_beam_laser();
		register_mag135_reflect_beam();
		register_mag136_oil_shot();
		register_mag137_oil_blast();
		register_mag138_saliva();
		register_mag179_bomb_glow();
		register_mag276_xatm092_death();
		register_mag277_funguar_laser();
		register_mag315_fake_president_death();
		register_mag289_brrawghh();
		register_mag239_ochu_dance();
		register_mag240_ochu_earthquake();
		register_mag281_shotgun();
		register_mag304_boomerang_sword();
		register_mag322_zan();
		register_mag323_metsu();
		register_mag311_gravija();
		register_mag258_mad_cow_special();
		register_mag256_sand_shake();
		register_mag244_bgh_turret_explode();
		register_mag220_light_pillar();
		register_mag253_twin_homing_laser();
		register_mag254_homing_laser_281();
		register_mag255_homing_laser_282();
		register_mag249_bgh_attack();
		register_mag250_beam_cannon();
		register_mag248_unknown_248();
		register_mag266_tiamat_death();
		register_mag268_tri_point_death();
		register_mag270_ultima_weapon_death();
		register_mag228_ice_strike();
		register_mag166_megiddo_flame();
		register_mag312_megiddo_flame();
		register_mag174_degenerator();
		register_mag213_storm_breath();
		register_mag217_energy_bomber();
		register_mag345_lv5_death();
		register_mag150_gatling_gun();
		register_mag181_kamikaze();
		register_mag313_ray_bomb();
		register_mag284_micro_missiles();
		register_mag282_evil_eye();
		register_mag285_thunder_summon();
		register_mag286_mini_pulse_cannon();
		register_mag279_mega_spark();
		register_mag287_mega_pulse_cannon();
		register_mag214_omega_gravija();
		register_mag207_maelstrom();
		register_mag172_morph();
		register_mag171_poison_gas();
		register_mag170_acid();
		register_mag156_wild_cannon_blow();
		register_mag151_cannon_blow();
		register_mag186_blaster();
		register_mag321_ker_plunk();
		register_mag301_gargantua_burrow();
		register_mag298_bgh251f2_destruction();
		register_mag290_trauma_shatter();
		register_mag283_magic_summon();
		register_mag241_bgh_gatling_gun();
		register_mag242_bgh_beam_cannon();
		register_mag243_bgh_turret1_explode();
		register_mag245_bgh_turret3_explode();
		register_mag246_bgh_turret4_explode();
		register_mag247_bgh_death();
		register_mag238_breath();
		register_mag252_corona();
		register_mag257_mega_flare();
		register_mag267_catoblepas_death();
		register_mag331_unknown_331();
		register_mag269_gargantua_269();
		register_mag236_gas();
		register_mag237_explosion();
		register_mag211_adel_death();
		register_mag219_terra_break();
		register_mag265_sphinxara_265();
		register_mag208_final_sorceress_death();
		register_mag264_krysta_264();
		register_mag221_apocalypse();
		register_mag344_griever_368();
		register_mag302_snipe_laser();
		register_mag318_strong_counter_attack();
		register_mag324_tonberry_king_death();
		register_mag319_counter_attack();
		register_mag316_red_giant();
		register_mag314_abadon();
		register_mag308_mobile_type8();
		register_mag320_dark_flare();
		register_mag303_remove_mask();
		register_mag209_sorceress_spawn();
		register_mag167_zantetsuken();
		register_mag218_gilgamesh_kills_seifer();
		register_mag251_demon_slice();
		register_mag210_bloodfest();
		register_mag173_ice_breath();
		register_mag176_sand_storm_adamantoise();
		register_mag157_breath();
		register_mag158_melt_eye();
		register_mag178_10000_needles();
		register_mag177_1000_needles();
		register_mag226_white_wind();
		register_mag227_ultimecia_first_death();
		register_mag139_sonic_wave();
		register_mag050_potion();
		register_mag051_hipotion();
		register_mag053_megapotion();
		register_mag058_elixir();
		register_mag059_megalixir();
		register_mag052_xpotion();
		register_mag005_phoenix_down();
		register_mag007_mega_phoenix();
		register_mag292_soft();
		register_mag293_eye_drops();
		register_mag294_antidote();
		register_mag295_echo_screen();
		register_mag296_holy_water();
		register_mag067_remedy();
		register_mag309_hero();
		register_mag310_holy_war();
		register_mag160_renzokuken();
		register_mag163_rough_divide();
		register_mag162_fated_circle();
		register_mag164_blasting_zone();
		register_mag165_lion_heart();
		register_mag183_defend();
		register_mag182_card();
		register_mag057_treatment();
		register_mag056_absorb();
		register_mag016_recover();
		register_mag061_revive();
		register_mag272_lv_down();
		register_mag271_lv_up();
		register_mag273_mad_rush();
		register_mag062_devour();
		register_mag184_ultra_waves_quistis();
		register_mag074_lvdeath_quistis();
		register_mag073_mighty_guard_quistis();
		register_mag072_aqua_breath_quistis();
		register_mag297_white_wind_quistis();
		register_mag230_fire_breath_quistis();
		register_mag229_homing_laser_quistis();
		register_mag306_degenerator_quistis();
		register_mag307_ray_bomb_quistis();
		register_mag305_gatling_gun_quistis();
		register_mag071_laser_eye_quistis();
		register_mag317_acid_quistis();
		register_mag300_bad_breath_quistis();
		register_mag299_micro_missiles_quistis();
		register_mag275_electrocute_quistis();
		register_mag260_shockwave_pulsar_quistis();
		register_mag280_full_cure();
		register_mag288_rapture();
		register_mag274_duel();
		register_mag189_wall();
		register_mag085_the_end();
		register_mag084_angel_wing();
		register_mag112_meteor_barret();
		register_mag212_burning_rave();
		register_mag093_angelo_recover();
		register_mag094_angelo_reverse();
		register_mag215_my_final_heaven();
		register_mag091_angelo_rush();
		register_mag216_different_beat();
		register_mag092_angelo_search();
		register_mag088_invincible_moon();
		register_mag188_normal_shot();
		register_mag196_quick_shot();
		register_mag192_scatter_shot();
		register_mag194_flame_shot();
		register_mag086_angelo_cannon();
		register_mag195_canister_shot();
		register_mag193_dark_shot();
		register_mag197_armor_shot();
		register_mag087_angelo_strike();
		register_mag198_hyper_shot();
		register_mag200_no_mercy();
		register_mag089_wishing_star();
		register_mag261_desperado();
		register_mag263_massive_anchor();
		register_mag262_blood_pain();
		register_mag017_elvoret_entrance();
		register_mag110_biggs_wedge_death();
		register_mag018_elvoret_death();
		register_mag161_renzokuken_elvoret();
		register_mag159_renzokuken_xatm();
		register_mag141_renzokuken_5hits();
		register_mag259_renzokuken_6hits();
		register_mag332_renzokuken_7hits();
		register_mag333_renzokuken_8hits();
		register_mag334_renzokuken_bahamut();
		register_mag335_renzokuken_norg();
		register_mag336_renzokuken_ultima_weapon();
		register_mag337_renzokuken_sorceress();
		register_mag339_renzokuken_adel();
		register_mag340_renzokuken_ultimecia();
		register_mag341_renzokuken_jumbo_cactuar();
		register_mag342_renzokuken_ultimecia_griever();
		register_mag343_renzokuken_griever();
		register_gfc_ifrit();
		register_gfc_leviathan();
		register_gfc_bahamut();
		register_gfc_cerberus();
		register_gfc_alexander();
		register_gfc_brothers();
		register_gfc_eden();
		register_mag191_doomtrain();
		register_mag327_gilgamesh();
	}
}
