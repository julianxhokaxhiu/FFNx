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

// Native battle effects: task types and game functions

#pragma once

#include <stdint.h>
#include <algorithm>

#include "fx_address.h"

// From patch.h, which cannot be included here (windows.h macros clash with the ports)
void memcpy_code(uint32_t offset, const void *data, uint32_t size);

namespace ff8fx
{
	template<uint32_t N> struct Bytes { uint8_t b[N]; };
	inline void copy_bytes(void *dst, const void *src, uint32_t size) { std::copy_n((const uint8_t *)src, size, (uint8_t *)dst); }
	inline void fill_bytes(void *dst, uint8_t value, uint32_t size) { std::fill_n((uint8_t *)dst, size, value); }
	template<uint32_t N> inline void copy(void *dst, const void *src) { *(Bytes<N> *)dst = *(const Bytes<N> *)src; }
	template<uint32_t N> inline void fill(void *dst, int value)
	{
		uint8_t *d = (uint8_t *)dst;
		for (uint32_t i = 0; i < N; i++) d[i] = (uint8_t)value;
	}
	inline bool same_bytes(const void *a, const void *b, uint32_t size)
	{
		const uint8_t *x = (const uint8_t *)a, *y = (const uint8_t *)b;
		for (uint32_t i = 0; i < size; i++) if (x[i] != y[i]) return false;
		return true;
	}

#pragma pack(push, 1)
	struct TaskNode
	{
		uint16_t flags;
		uint16_t sequence;
		TaskNode *next;
		uint32_t (__cdecl *func)(TaskNode *);
	};
	struct TaskQueue
	{
		TaskNode *head, *tail;
		void *pool;
		int16_t node_size, capacity;
	};
	struct TaskQueuePair
	{
		TaskQueue first;
		TaskQueue second;
	};
	struct Mat4x3
	{
		int16_t m[3][3];
		int16_t pad;
		int32_t t[3];
	};
#pragma pack(pop)
	static_assert(sizeof(TaskNode) == 12, "TaskNodeHeader is 12 bytes");
	static_assert(sizeof(TaskQueue) == 16, "TaskQueueHeader is 16 bytes");
	static_assert(sizeof(TaskQueuePair) == 32, "TaskQueueExample is 32 bytes");
	static_assert(sizeof(Mat4x3) == 32, "FF8_PSX_Matrix4x3 is 32 bytes");

	typedef uint32_t (__cdecl *TaskFn)(TaskNode *);

	static const uint32_t TASK_END = 2;

	namespace eng
	{
		template<typename T> inline T &var(uint32_t addr) { return *(T *)addr; }
		template<typename F> inline F fn(uint32_t addr) { return (F)addr; }

		inline int ExecuteTaskQueue(TaskQueue *q) { return fn<int (__cdecl *)(TaskQueue *)>(game_addr(game::ExecuteTaskQueue))(q); }
		inline void InitTaskQueuePool(TaskQueue *q, void *pool, uint32_t node_size, int count) { fn<void (__cdecl *)(TaskQueue *, void *, uint32_t, int)>(game_addr(game::BdLink_InitTaskQueuePool))(q, pool, node_size, count); }
		inline TaskNode *AddTaskToQueue(TaskQueue *q, uint32_t task_fn) { return fn<TaskNode *(__cdecl *)(TaskQueue *, uint32_t)>(game_addr(game::AddTaskToQueue))(q, task_fn); }
		inline TaskNode *AddTaskToQueue(TaskQueue *q, game::Id task_fn) { return AddTaskToQueue(q, game_addr(task_fn)); }
		inline void *FieldAlloc(int size) { return fn<void *(__cdecl *)(int)>(game_addr(game::Field_Alloc))(size); }
		inline void FieldFree(int size) { fn<void (__cdecl *)(int)>(game_addr(game::Field_Free))(size); }
		inline void Memset32(void *dst, uint32_t value, uint32_t dwords) { fn<void (__cdecl *)(void *, uint32_t, uint32_t)>(game_addr(game::Effect_Memset32_wrapper))(dst, value, dwords); }
		inline void GteSetRotMatrix(const Mat4x3 *m) { fn<void (__cdecl *)(const Mat4x3 *)>(game_addr(game::GTE_SetRotMatrix_W))(m); }
		inline void GteSetTransVector(const Mat4x3 *m) { fn<void (__cdecl *)(const Mat4x3 *)>(game_addr(game::GTE_SetTransVector_W))(m); }
		inline void BuildRotationMatrixFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::BuildRotationMatrixFromAngles))(angles, out); }
		inline void ComposeAffineTransform(const Mat4x3 *a, const Mat4x3 *b, Mat4x3 *out) { fn<void (__cdecl *)(const Mat4x3 *, const Mat4x3 *, Mat4x3 *)>(game_addr(game::ComposeAffineTransform))(a, b, out); }
		inline void EffectCameraMatrix(const Mat4x3 *camera, Mat4x3 *out) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::MAG_069_OrthonormalizeCameraMatrix))(camera, out); }
		inline void GteLoadV0(const void *coord) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadV0))(coord); }
		inline void GteRTPS() { fn<void (__cdecl *)()>(game_addr(game::GTE_RTPS))(); }
		inline void GteReadSXY2(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadSXY2))(dst); }
		inline void GteReadOTZ(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadOTZ_SZ3div4))(dst); }
		inline void GteSetIR0(int32_t v) { fn<void (__cdecl *)(int32_t)>(game_addr(game::GTE_SetIR0))(v); }
		inline void GteLoadIR123(const void *v3) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadIR123_u16))(v3); }
		inline void GteGPF() { fn<void (__cdecl *)()>(game_addr(game::GTE_GPF))(); }
		inline void GteGPL() { fn<void (__cdecl *)()>(game_addr(game::GTE_GPL))(); }
		inline void GteStoreIR123(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreIR123))(dst); }
		inline void GteSetRotMatrixCtrl(const Mat4x3 *m) { fn<void (__cdecl *)(const Mat4x3 *)>(game_addr(game::GTE_SetRotMatrix))(m); }
		inline void GteSetTransVectorCtrl(const Mat4x3 *m) { fn<void (__cdecl *)(const Mat4x3 *)>(game_addr(game::GTE_SetTransVector))(m); }
		inline void GteMVMVA_RotV0Tr() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0_Tr))(); }
		inline void GteReadDataReg(int reg, void *dst) { fn<void (__cdecl *)(int, void *)>(game_addr(game::GTE_ReadDataReg))(reg, dst); }
		inline void GteReadDataRegTo(void *dst, int reg) { fn<void (__cdecl *)(void *, int)>(game_addr(game::GTE_ReadDataRegTo))(dst, reg); }
		inline void GteSetRotDiagonal(const int32_t *v3) { fn<void (__cdecl *)(const int32_t *)>(game_addr(game::GTE_SetRotDiagonal_v32))(v3); }
		inline void GteLoadIR123_32(const int32_t *v3) { fn<void (__cdecl *)(const int32_t *)>(game_addr(game::GTE_LoadIR123_32b))(v3); }
		inline void GteLoadIR123_32b(const int32_t *v3) { fn<void (__cdecl *)(const int32_t *)>(game_addr(game::GTE_LoadIR123_32))(v3); }
		inline void GteOP() { fn<void (__cdecl *)()>(game_addr(game::GTE_OP_v2))(); }
		inline void GteSQR() { fn<void (__cdecl *)()>(game_addr(game::GTE_SQR))(); }
		inline void GteReadMAC123(int32_t *dst) { fn<void (__cdecl *)(int32_t *)>(game_addr(game::GTE_ReadMAC123))(dst); }
		inline void GteSumMAC123(int32_t *dst) { fn<void (__cdecl *)(int32_t *)>(game_addr(game::GTE_SumMAC123))(dst); }
		inline void GteSetLZCS(int32_t v) { fn<void (__cdecl *)(int32_t)>(game_addr(game::GTE_SetLZCS))(v); }
		inline void GteReadLZCR(int32_t *dst) { fn<void (__cdecl *)(int32_t *)>(game_addr(game::GTE_ReadLZCR))(dst); }
		inline int32_t ComputeSin(int32_t angle) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeSin))(angle); }
		inline void InsertPrimAutoDepth(uint32_t bucket, void *packet) { fn<void (__cdecl *)(uint32_t, void *)>(game_addr(game::SSIGPU_InsertPrimAutoDepth))(bucket, packet); }
		inline uint32_t RenderPrimModel(void *header, uint32_t ot, int mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int, uint32_t)>(game_addr(game::Effect_RenderPrimModel))(header, ot, mode, cursor); }
		inline void SetScreenFlash(uint32_t level, uint32_t mode) { fn<void (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::Effect_SetScreenFlash))(level, mode); }
		inline void TransformCameraByShadowRotation(const void *pos, int32_t a, int32_t b) { fn<void (__cdecl *)(const void *, int32_t, int32_t)>(game_addr(game::TransformCameraByShadowRotation))(pos, a, b); }
		inline uint32_t InitEffectSequenceFromData(void *header, uint32_t ot, int mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int, uint32_t)>(game_addr(game::InitEffectSequenceFromData))(header, ot, mode, cursor); }
		inline void ComputeBonesWorldMatrices(void *anim_header, void *root) { fn<void (__cdecl *)(void *, void *)>(game_addr(game::BS_ComputeBonesWorldMatrices))(anim_header, root); }
		inline void BuildBoneMatricesFromPose(void *anim_header) { fn<void (__cdecl *)(void *)>(game_addr(game::BattleModel_BuildBoneMatricesFromPose))(anim_header); }
		inline uint32_t ClaimVoiceSlot(const void *sound, int a, int b) { return fn<uint32_t (__cdecl *)(const void *, int, int)>(game_addr(game::BdSound_ClaimVoiceSlot))(sound, a, b); }
		inline void StreamStateInit(void *state) { fn<void (__cdecl *)(void *)>(game_addr(game::BattleFile_StreamState_Init))(state); }

		inline Mat4x3 &Camera() { return var<Mat4x3>(game_addr(game::unk_1D97778)); }
		inline uint32_t RenderOT() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 68; }
	}

	inline int32_t mul32(int32_t a, int32_t b) { return (int32_t)((uint32_t)a * (uint32_t)b); }
	inline int32_t shl32(int32_t a, int n) { return (int32_t)((uint32_t)a << n); }
	inline int32_t ScreenPx(int32_t eighths) { return (int32_t)((int16_t)eighths >> 3); }
	inline int32_t ScreenEighths(int32_t pixels) { return pixels * 8; }

	inline int16_t PsxPixels(int16_t pc) { return (int16_t)(pc >> 3); }

	void register_port(uint32_t orig, void *port, const char *name, int effect_id);
	inline void register_port(game::Id orig, void *port, const char *name, int effect_id) { register_port(game_addr(orig), port, name, effect_id); }
	void set_ports(bool on);
	bool module_ported(int effect_id);
	const char *port_name(uint32_t orig);
	extern bool g_active;
	void register_all();
	void install();
	extern void (*g_queue_seen)(TaskQueue *q);

	namespace prim
	{
		struct Layout { uint8_t *data; int32_t frame; uint8_t state[4]; };
		struct Record
		{
			uint16_t index, flags_lo; uint32_t flags;
			int16_t pos[3], pad0; int16_t rot[3], pad1; int16_t scale[3], pad2;
			uint8_t rgb[4];
			int16_t a;
			int16_t b;
			int16_t b0, b1;
		};
		typedef void(__cdecl *Callback)(Layout *l, Record *r, int arg);
		int play(Layout *l, Callback cb, int arg, int paused);
		void install_verify();
	}

	void register_mag069_griever();
	void register_mag116_quezacotl();
	void register_mag140_phoenix();
	void register_mag185_shiva();
	void register_mag187_odin();
	void register_mag199_cactuar();
	void register_mag278_carbuncle();
	void register_mag291_pandemona();
	void register_mag325_diablos();
	void register_mag326_odin_reverse();
	void register_mag338_moomba();
	void register_mag095_siren();
	void register_mag096_minimog();
	void register_mag090_tonberry();
	void register_mag097_boko();
	void register_mag002_fire();
	void register_mag142_fira();
	void register_mag143_firaga();
	void register_mag003_thunder();
	void register_mag102_thundara();
	void register_mag105_thundaga();
	void register_mag144_blizzard();
	void register_mag103_blizzara();
	void register_mag104_blizzaga();
	void register_mag022_bio();
	void register_mag038_quake();
	void register_mag149_ultima();
	void register_mag175_holy();
	void register_mag034_pain();
	void register_mag148_meltdown();
	void register_mag039_drain();
	void register_mag014_death();
	void register_mag036_confuse();
	void register_mag124_flare();
	void register_mag145_sleep();
	void register_mag146_tornado();
	void register_mag107_demi();
	void register_mag109_dispel();
	void register_mag111_aura();
	void register_mag118_aero();
	void register_mag123_slow();
	void register_mag119_stop();
	void register_mag125_haste();
	void register_mag115_float();
	void register_mag114_zombie();
	void register_mag117_break();
	void register_mag108_berserk();
	void register_mag122_silence();
	void register_mag121_blind();
	void register_mag147_regen();
	void register_mag024_esuna();
	void register_mag032_protect();
	void register_mag033_shell();
	void register_fx_glint();
	void register_fx_cast();
	void register_fx_sprite();
	void register_mag035_life();
	void register_mag027_fulllife();
	void register_mag028_curaga();
	void register_mag106_reflect();
	void register_mag025_cura();
	void register_mag001_cure();
	void register_mag004_double();
	void register_mag021_triple();
	void register_mag040_scan();
	void register_mag222_water();
	void register_mag223_meteor();
	void register_mag120_petrify_stare();
	void register_mag152_ultrasonic_waves();
	void register_mag127_petrify_stare();
	void register_mag029_wind_blast();
	void register_mag235_breath();
	void register_mag231_disease_breath();
	void register_mag233_earthquake();
	void register_mag026_clash();
	void register_mag010_ray_bomb();
	void register_mag011_storm_breath();
	void register_mag155_sand_storm();
	void register_mag168_sleeping_gas();
	void register_mag154_ultra_waves();
	void register_mag180_suicide();
	void register_mag169_gastric_juice();
	void register_mag126_electric_discharge();
	void register_mag153_sticky_web();
	void register_mag013_dark_mist();
	void register_mag232_breath_of_death();
	void register_mag234_fart();
	void register_mag030_counter_laser_eye();
	void register_mag041_dribble();
	void register_mag190_chain_gun();
	void register_mag048_magma_breath();
	void register_mag008_doom();
	void register_mag009_doom_activation();
	void register_mag012_blade_shot();
	void register_mag015_draw();
	void register_mag037_drink_magic();
	void register_mag019_raldo_throw();
	void register_mag020_norg_pod_opening();
	void register_mag023_psycho_blast();
	void register_mag031_heartbreak();
	void register_mag042_shoot();
	void register_mag043_melting_bubble();
	void register_mag044_junk();
	void register_mag045_stare();
	void register_mag046_sigh();
	void register_mag047_curse();
	void register_mag049_resonance();
	void register_mag054_everyones_grudge();
	void register_mag055_aqua_breath();
	void register_mag060_raijin_attack();
	void register_mag063_helix_attack();
	void register_mag064_griever_tail();
	void register_mag065_great_attractor();
	void register_mag066_griever_ultimecia_death();
	void register_mag068_scan();
	void register_mag075_hells_judgement();
	void register_mag070_shockwave_pulsar();
	void register_mag076_ultimecia_final_form_spawn();
	void register_mag078_mighty_guard();
	void register_mag077_final_form_death();
	void register_mag079_ultimecia_final_death();
	void register_mag080_ultimecia_junction();
	void register_mag081_ultimecia_unk4();
	void register_mag082_ultimecia_blow_away();
	void register_mag083_ultimecia_absorb();
	void register_mag101_trauma_grab();
	void register_mag113_bad_breath();
	void register_mag128_gastric_juice();
	void register_mag129_breath();
	void register_mag130_eerie_sound_wave();
	void register_mag131_bad_breath();
	void register_mag132_dissolving_acid();
	void register_mag133_hypnotize();
	void register_mag134_beam_laser();
	void register_mag135_reflect_beam();
	void register_mag136_oil_shot();
	void register_mag137_oil_blast();
	void register_mag138_saliva();
	void register_mag179_bomb_glow();
	void register_mag276_xatm092_death();
	void register_mag277_funguar_laser();
	void register_mag315_fake_president_death();
	void register_mag289_brrawghh();
	void register_mag239_ochu_dance();
	void register_mag240_ochu_earthquake();
	void register_mag281_shotgun();
	void register_mag304_boomerang_sword();
	void register_mag322_zan();
	void register_mag323_metsu();
	void register_mag311_gravija();
	void register_mag258_mad_cow_special();
	void register_mag256_sand_shake();
	void register_mag244_bgh_turret_explode();
	void register_mag220_light_pillar();
	void register_mag253_twin_homing_laser();
	void register_mag254_homing_laser_281();
	void register_mag255_homing_laser_282();
	void register_mag249_bgh_attack();
	void register_mag250_beam_cannon();
	void register_mag248_unknown_248();
	void register_mag266_tiamat_death();
	void register_mag268_tri_point_death();
	void register_mag270_ultima_weapon_death();
	void register_mag228_ice_strike();
	void register_mag166_megiddo_flame();
	void register_mag312_megiddo_flame();
	void register_mag174_degenerator();
	void register_mag213_storm_breath();
	void register_mag217_energy_bomber();
	void register_mag345_lv5_death();
	void register_mag150_gatling_gun();
	void register_mag181_kamikaze();
	void register_mag313_ray_bomb();
	void register_mag284_micro_missiles();
	void register_mag282_evil_eye();
	void register_mag285_thunder_summon();
	void register_mag286_mini_pulse_cannon();
	void register_mag279_mega_spark();
	void register_mag287_mega_pulse_cannon();
	void register_mag214_omega_gravija();
	void register_mag207_maelstrom();
	void register_mag172_morph();
	void register_mag171_poison_gas();
	void register_mag170_acid();
	void register_mag156_wild_cannon_blow();
	void register_mag151_cannon_blow();
	void register_mag186_blaster();
	void register_mag321_ker_plunk();
	void register_mag301_gargantua_burrow();
	void register_mag298_bgh251f2_destruction();
	void register_mag290_trauma_shatter();
	void register_mag283_magic_summon();
	void register_mag241_bgh_gatling_gun();
	void register_mag242_bgh_beam_cannon();
	void register_mag243_bgh_turret1_explode();
	void register_mag245_bgh_turret3_explode();
	void register_mag246_bgh_turret4_explode();
	void register_mag247_bgh_death();
	void register_mag238_breath();
	void register_mag252_corona();
	void register_mag257_mega_flare();
	void register_mag267_catoblepas_death();
	void register_mag331_unknown_331();
	void register_mag269_gargantua_269();
	void register_mag236_gas();
	void register_mag237_explosion();
	void register_mag211_adel_death();
	void register_mag219_terra_break();
	void register_mag265_sphinxara_265();
	void register_mag208_final_sorceress_death();
	void register_mag264_krysta_264();
	void register_mag221_apocalypse();
	void register_mag344_griever_368();
	void register_mag302_snipe_laser();
	void register_mag318_strong_counter_attack();
	void register_mag324_tonberry_king_death();
	void register_mag319_counter_attack();
	void register_mag316_red_giant();
	void register_mag314_abadon();
	void register_mag308_mobile_type8();
	void register_mag320_dark_flare();
	void register_mag303_remove_mask();
	void register_mag209_sorceress_spawn();
	void register_mag167_zantetsuken();
	void register_mag218_gilgamesh_kills_seifer();
	void register_mag251_demon_slice();
	void register_mag210_bloodfest();
	void register_mag173_ice_breath();
	void register_mag176_sand_storm_adamantoise();
	void register_mag157_breath();
	void register_mag158_melt_eye();
	void register_mag178_10000_needles();
	void register_mag177_1000_needles();
	void register_mag226_white_wind();
	void register_mag227_ultimecia_first_death();
	void register_mag139_sonic_wave();
	void register_mag050_potion();
	void register_mag051_hipotion();
	void register_mag053_megapotion();
	void register_mag058_elixir();
	void register_mag059_megalixir();
	void register_mag052_xpotion();
	void register_mag005_phoenix_down();
	void register_mag007_mega_phoenix();
	void register_mag292_soft();
	void register_mag293_eye_drops();
	void register_mag294_antidote();
	void register_mag295_echo_screen();
	void register_mag296_holy_water();
	void register_mag067_remedy();
	void register_mag309_hero();
	void register_mag310_holy_war();
	void register_mag160_renzokuken();
	void register_mag163_rough_divide();
	void register_mag162_fated_circle();
	void register_mag164_blasting_zone();
	void register_mag165_lion_heart();
	void register_mag183_defend();
	void register_mag182_card();
	void register_mag057_treatment();
	void register_mag056_absorb();
	void register_mag016_recover();
	void register_mag061_revive();
	void register_mag272_lv_down();
	void register_mag271_lv_up();
	void register_mag273_mad_rush();
	void register_mag062_devour();
	void register_mag184_ultra_waves_quistis();
	void register_mag074_lvdeath_quistis();
	void register_mag073_mighty_guard_quistis();
	void register_mag072_aqua_breath_quistis();
	void register_mag297_white_wind_quistis();
	void register_mag230_fire_breath_quistis();
	void register_mag229_homing_laser_quistis();
	void register_mag306_degenerator_quistis();
	void register_mag307_ray_bomb_quistis();
	void register_mag305_gatling_gun_quistis();
	void register_mag071_laser_eye_quistis();
	void register_mag317_acid_quistis();
	void register_mag300_bad_breath_quistis();
	void register_mag299_micro_missiles_quistis();
	void register_mag275_electrocute_quistis();
	void register_mag260_shockwave_pulsar_quistis();
	void register_mag280_full_cure();
	void register_mag288_rapture();
	void register_mag274_duel();
	void register_mag189_wall();
	void register_mag085_the_end();
	void register_mag084_angel_wing();
	void register_mag112_meteor_barret();
	void register_mag212_burning_rave();
	void register_mag093_angelo_recover();
	void register_mag094_angelo_reverse();
	void register_mag215_my_final_heaven();
	void register_mag091_angelo_rush();
	void register_mag216_different_beat();
	void register_mag092_angelo_search();
	void register_mag088_invincible_moon();
	void register_mag188_normal_shot();
	void register_mag196_quick_shot();
	void register_mag192_scatter_shot();
	void register_mag194_flame_shot();
	void register_mag086_angelo_cannon();
	void register_mag195_canister_shot();
	void register_mag193_dark_shot();
	void register_mag197_armor_shot();
	void register_mag087_angelo_strike();
	void register_mag198_hyper_shot();
	void register_mag200_no_mercy();
	void register_mag089_wishing_star();
	void register_mag261_desperado();
	void register_mag263_massive_anchor();
	void register_mag262_blood_pain();
	void register_mag017_elvoret_entrance();
	void register_mag110_biggs_wedge_death();
	void register_mag018_elvoret_death();
	void register_mag161_renzokuken_elvoret();
	void register_mag159_renzokuken_xatm();
	void register_mag141_renzokuken_5hits();
	void register_mag259_renzokuken_6hits();
	void register_mag332_renzokuken_7hits();
	void register_mag333_renzokuken_8hits();
	void register_mag334_renzokuken_bahamut();
	void register_mag335_renzokuken_norg();
	void register_mag336_renzokuken_ultima_weapon();
	void register_mag337_renzokuken_sorceress();
	void register_mag339_renzokuken_adel();
	void register_mag340_renzokuken_ultimecia();
	void register_mag341_renzokuken_jumbo_cactuar();
	void register_mag342_renzokuken_ultimecia_griever();
	void register_mag343_renzokuken_griever();
	void register_gfc_ifrit();
	void register_gfc_leviathan();
	void register_gfc_bahamut();
	void register_gfc_cerberus();
	void register_gfc_alexander();
	void register_gfc_brothers();
	void register_gfc_eden();
	void register_mag191_doomtrain();
	void register_mag327_gilgamesh();
}
