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

// Effect 345: LV5 Death

#include "act_engine.h"

namespace ff8fx
{
namespace act
{
namespace lv5_death
{
	static const Mod MOD_345 = { "lv5_death", 345, game::MAG_345_UNKNOWN_FL, game::MAG_100_CHOCOBOCLE_FL,
		{ {}, {}, game::unk_1524658, {}, {}, game::unk_2557204, {}, {}, {}, {}, {}, {}, game::MAG_345_Lv5Death_TextureFile, {}, {}, {}, game::unk_2557208, {}, {}, {}, {}, {}, {}, {}, game::unk_255D7F8, {}, game::unk_255DA98, {}, {}, {}, {}, {}, game::unk_255EA40, {}, {}, game::unk_255EA7C, {}, game::MAG_345_Lv5Death_ScratchStackPointer, {}, game::MAG_345_Lv5Death_ActorState },
		{ {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::sub_706B10, game::sub_7075D0, game::sub_707660, game::sub_7076C0, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::sub_709A40, game::sub_709B80, game::sub_709BC0, game::sub_70AE50, game::sub_70AEA0, game::sub_70AED0, game::sub_70AF50, game::sub_70B640, game::sub_70B690, game::sub_70B730, game::sub_70B7F0, game::sub_70C040, game::sub_70C440, game::sub_70C4D0, {}, {}, {}, {}, {}, game::MAG_345_Lv5Death_Emitter, game::MAG_345_sub_705ED0, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::sub_706CA0, game::sub_706D80, game::sub_706F20, game::sub_706FF0, game::sub_707110, game::sub_707370, game::sub_709B00, game::sub_709C50, game::sub_70A2B0, game::sub_70A300, game::sub_70A390, game::sub_70A460, game::sub_70ABF0, game::sub_70AD00, game::sub_70AD80, game::sub_70ADB0, game::MAG_345_RandRange, game::sub_70B600, game::MAG_345_RandAngleJitter, game::sub_70B800, game::sub_70B950, {}, game::sub_70C010, game::sub_70C1F0, game::sub_70C330, {}, game::sub_7075D0, game::MAG_345_sub_70D210, game::MAG_345_sub_70D250, game::MAG_345_sub_70D270, game::nullsub_1067, {}, {}, {}, {}, {}, {}, {} } };

	static constexpr game::Id PACKET_CURSOR = game::MAG_345_Lv5Death_PacketCursor;
	static constexpr game::Id ACTOR_STATE = game::MAG_345_Lv5Death_ActorState;
	static constexpr game::Id SCRATCH_SP = game::MAG_345_Lv5Death_ScratchStackPointer;
	static constexpr game::Id REAPER_DATA = game::unk_1524658;
	static constexpr game::Id ORIG_Master = game::MAG_345_Lv5Death_Master;
	static constexpr game::Id ORIG_Reaper = game::MAG_345_Lv5Death_Reaper;
	static constexpr game::Id ORIG_ActorSystem = game::MAG_345_Lv5Death_ActorSystem;
	static constexpr game::Id ORIG_Spark = game::MAG_345_Lv5Death_Spark;
	static constexpr game::Id REAPER_CTX = game::MAG_345_Lv5Death_ReaperRenderContext;

	namespace dx
	{
		static inline uint32_t sub_45E0B0(uint32_t a1) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::GTE_LoadIR123_u16))(a1); }
		static inline uint32_t sub_45E9D0() { return fn<uint32_t (__cdecl *)()>(game_addr(game::GTE_GPF))(); }
		static inline uint32_t sub_45EBF0() { return fn<uint32_t (__cdecl *)()>(game_addr(game::GTE_GPL))(); }
		static inline uint32_t GTE_SetLightMatrix(uint32_t a1) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::GTE_SetLightMatrix))(a1); }
		static inline uint32_t GTE_SetBackgroundVector(uint32_t a1, uint32_t a2, uint32_t a3) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_SetBackgroundVector))(a1, a2, a3); }
		static inline uint32_t GTE_MVMVA_LightV0_Bk() { return fn<uint32_t (__cdecl *)()>(game_addr(game::GTE_MVMVA_LightV0_Bk))(); }
		static inline uint32_t GTE_StoreSXY012_PolyFT3(uint32_t a1) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::GTE_StoreSXY012_PolyFT3))(a1); }
		static inline uint32_t GTE_StoreSXY012_PolyFT3_2(uint32_t a1) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::GTE_StoreSXY012_PolyFT3_2))(a1); }
	}
	using namespace dx;

	static inline void e6_copy16(uint32_t dst, uint32_t src)
	{
		U32(dst, 0) = U32(src, 0);
		U32(dst, 4) = U32(src, 4);
		U32(dst, 8) = U32(src, 8);
		U32(dst, 12) = U32(src, 12);
	}
	static inline void e6_rotate_zxy(uint32_t m, uint32_t ang)
	{
		x::MAG_022_sub_8DD770(m);
		if (U16(ang, 4) != 0) x::sub_8DD960(m, (uint32_t)(int32_t)S16(ang, 4));
		if (U16(ang, 0) != 0) x::sub_8DD7E0(m, (uint32_t)(int32_t)S16(ang, 0));
		if (U16(ang, 2) != 0) x::MAG_022_sub_8DD8A0(m, (uint32_t)(int32_t)S16(ang, 2));
	}
	static inline void e6_rotate_down_vector(uint32_t sp)
	{
		uint32_t v = sp + 0x20;
		U16(sp, 0x22) = 0xF000;
		U16(sp, 0x24) = 0;
		U16(v, 0) = 0;
		x::GTE_SetRotMatrix(sp);
		x::GTE_LoadV0(v);
		x::GTE_MVMVA_RotV0();
		x::GTE_StoreIR123(v);
	}
	static inline void e6_rand_mod(uint32_t sp, int32_t off)
	{
		if (U16(sp, off) != 0)
		{
			int32_t r = (int32_t)x::CrtRand();
			U16(sp, off) = (uint16_t)(r % (int32_t)S16(sp, off));
		}
	}
	static inline void b1_copy16(uint32_t dst, uint32_t src)
	{
		for (int i = 0; i < 16; i += 4)
			U32(dst, i) = U32(src, i);
	}
	static inline int32_t b2_fx(uint32_t v)
	{
		int32_t s = (int32_t)v;
		return add32(s, (s >> 31) & 0xFFFF) >> 16;
	}
	static inline bool t1_out_x(int16_t v) { return v < 0 || v > 0xA00; }
	static inline bool t1_out_y(int16_t v) { return v < 0 || v > 0x6C0; }
	static uint32_t t2_twin(uint32_t ctx, int32_t b)
	{
		uint32_t ecx = (uint32_t)(U8(ctx, b + 2) & 0xF8) | 0xFFFE2000u;
		uint32_t edx = (uint32_t)(U8(ctx, b) & 0xF8);
		ecx <<= 5;
		ecx |= edx;
		edx = U16(ctx, b + 6);
		ecx <<= 5;
		edx = ~(edx - 1) & 0xF8;
		ecx |= edx;
		edx = U16(ctx, b + 4);
		ecx <<= 2;
		edx = (uint32_t)((int32_t)~(edx - 1) >> 3) & 0x1F;
		ecx |= edx;
		return ecx;
	}
	static inline bool t2_outx(uint32_t p) { const int16_t v = S16(p, 0); return v < 0 || v > 0xA00; }
	static inline bool t2_outy(uint32_t p) { const int16_t v = S16(p, 0); return v < 0 || v > 0x6C0; }
	static inline void t2_depth_cue(uint32_t ctx, uint32_t pCode)
	{
		x::set_unk_1CA8A28(pCode);
		x::set_dword_1CA8A30(U32(ctx, 0xC));
		x::sub_45F270();
		x::set_param_with_dword_1CA8A68(pCode);
	}
	static inline uint32_t t2_ot(uint32_t ctx, uint32_t a2, uint32_t a3)
	{
		const int32_t z = add32(S32(ctx, 0x38), S32(ctx, 0x10));
		S32(ctx, 0x38) = z;
		if (z < 0)
			S32(ctx, 0x38) = 0;
		return a2 + (uint32_t)(S32(ctx, 0x38) >> (a3 & 31)) * 4u;
	}
	static inline bool t3_out_x(int16_t v) { return v < 0 || v > 0xA00; }
	static inline bool t3_out_y(int16_t v) { return v < 0 || v > 0x6C0; }
	static inline uint32_t t3_texwin(uint32_t p)
	{
		uint32_t t = ((uint32_t)U8(p, 2) & 0xF8) | 0xFFFE2000u;
		t = (t << 5) | ((uint32_t)U8(p, 0) & 0xF8);
		t = (t << 5) | (~((uint32_t)U16(p, 6) - 1) & 0xF8);
		t = (t << 2) | ((~((uint32_t)U16(p, 4) - 1) >> 3) & 0x1F);
		return t;
	}
	static inline void t3_scroll(uint32_t pkt, const int32_t *off, int n, uint16_t d, uint8_t wrap)
	{
		uint32_t s[4];
		uint32_t any = 0;
		for (int i = 0; i < n; i++)
		{
			s[i] = (uint32_t)U8(pkt, off[i]) + (uint32_t)d;
			any |= s[i];
		}
		const bool over = (int32_t)any > 0xFF;
		for (int i = 0; i < n; i++)
			U8(pkt, off[i]) = over ? (uint8_t)(s[i] - wrap) : (uint8_t)s[i];
	}
	static inline void t4_next_state(uint32_t node) { U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1); }

	static inline uint32_t task_end(uint32_t node, uint8_t status)
	{
		if ((status & 1) != 0 && U8(node, 0x28) == 0)
		{
			x::Effect_ReleaseLinkedTask(node);
			return 2;
		}
		return 0;
	}

	uint32_t __cdecl d_705DE0(void);
	uint32_t __cdecl d_705DA0(uint32_t a1);
	uint32_t __cdecl d_705F00(uint32_t a1);
	uint32_t __cdecl d_705ED0(uint32_t a1);
	uint32_t __cdecl d_706060(uint32_t a1);
	uint32_t __cdecl d_705FF0(uint32_t a1);
	uint32_t __cdecl d_7060B0(uint32_t a1);
	uint32_t __cdecl d_7060F0(uint32_t a1);
	uint32_t __cdecl d_706550(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl d_706450(uint32_t a1);
	uint32_t __cdecl d_7061F0(uint32_t a1);
	uint32_t __cdecl d_706130(uint32_t a1);
	uint32_t __cdecl d_70C8A0(uint32_t a1);
	uint32_t __cdecl d_70C840(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl d_70C810(uint32_t a1);
	uint32_t __cdecl d_706940(uint32_t a1);
	uint32_t __cdecl d_70C8B0(uint32_t a1);
	uint32_t __cdecl d_70C900(uint32_t a1);
	uint32_t __cdecl d_70D030(uint32_t a1);
	uint32_t __cdecl d_70D090(uint32_t a1);
	uint32_t __cdecl d_70C780(uint32_t a1);
	uint32_t __cdecl d_70C7C0(uint32_t a1);
	uint32_t __cdecl d_70D140(uint32_t a1);
	uint32_t __cdecl d_70CA40(uint32_t a1);
	uint32_t __cdecl d_70CA80(uint32_t a1);
	uint32_t __cdecl d_70CEC0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl d_70CD70(uint32_t a1);
	uint32_t __cdecl d_70CFC0(uint32_t a1);
	uint32_t __cdecl d_70CFF0(uint32_t a1);
	uint32_t __cdecl d_7076C0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl d_709B00(uint32_t a1);
	uint32_t __cdecl d_70ADB0(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl d_70B730(uint32_t a1, uint32_t a2);
	uint32_t __cdecl d_705C00(uint32_t a1);
	uint32_t __cdecl d_706AE0(uint32_t a1);
	uint32_t __cdecl d_706CA0(uint32_t a1);
	uint32_t __cdecl d_706F20(void);
	uint32_t __cdecl d_7075D0(uint32_t a1);
	uint32_t __cdecl d_707660(void);
	uint32_t __cdecl d_707820(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl d_707950(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl d_707B70(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl d_707E00(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl d_7082F0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl d_708E10(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl d_709370(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl d_709A40(void);
	uint32_t __cdecl d_709B80(void);
	uint32_t __cdecl d_709BC0(uint32_t a1);
	uint32_t __cdecl d_70ABF0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl d_70AD00(uint32_t a1, uint32_t a2);
	uint32_t __cdecl d_70AD80(uint32_t a1, uint32_t a2);
	uint32_t __cdecl d_70AE50(uint32_t a1);
	uint32_t __cdecl d_70AED0(uint32_t a1);
	uint32_t __cdecl d_70AF50(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl d_70B950(uint32_t a1);
	uint32_t __cdecl d_70C440(uint32_t a1, uint32_t a2);
	uint32_t __cdecl d_70C4D0(void);
	uint32_t __cdecl d_70C560(uint32_t a1);
	uint32_t __cdecl d_70C590(uint32_t a1, uint32_t a2);
	uint32_t __cdecl d_70C6B0(uint32_t a1);
	uint32_t __cdecl d_70D160(uint32_t a1);
	uint32_t __cdecl d_70D210(uint32_t a1);
	uint32_t __cdecl d_706B10(uint32_t a1);
	uint32_t __cdecl d_70D2A0(uint32_t a1);
}
}
}

namespace ff8fx
{
namespace act
{
namespace lv5_death
{
	uint32_t __cdecl d_705DE0(void)
	{
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(game_addr(game::unk_255EA44)), 0x870);
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(game_addr(game::unk_255EA88)), 0x9D80);
		MEM<uint16_t>(game_addr(game::unk_2557758)) = 0;
		MEM<uint16_t>(game_addr(game::unk_255EA50)) = 0;
		MEM<uint16_t>(game_addr(game::unk_255E610)) = 0;
		MEM<uint16_t>(game_addr(game::unk_25570A8)) = 0;
		return 0;
	}

	uint32_t __cdecl d_705DA0(uint32_t a1)
	{
		if (U8(a1, 0x28) != 0)
			return 0;
		uint32_t p = MEM<uint32_t>(game_addr(game::unk_255EA90));
		MEM<uint32_t>(game_addr(game::unk_255EA44)) = p;
		p += 0x870;
		MEM<uint32_t>(game_addr(game::unk_255EA88)) = p;
		p += 0x9D80;
		MEM<uint32_t>(game_addr(game::unk_255EA90)) = p;
		d_705DE0();
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl d_705F00(uint32_t a1)
	{
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(game_addr(REAPER_DATA)), 0x54);
		const uint32_t slot0 = U8(a1, 0x2D);
		x::GetEffectSpawnPosition(game_addr(game::BattleEntitySlotData) + slot0 * 0x9C, 0xF1, 0, MEM<uint32_t>(game_addr(REAPER_DATA)) + 8);
		const uint8_t slot = U8(a1, 0x2D);
		const uint32_t d = MEM<uint32_t>(game_addr(REAPER_DATA));
		U16(d, 0xA) = 0;
		const uint32_t ent = game_addr(game::BattleEntitySlotData) + (uint32_t)slot * 0x9C;
		U32(d, 0) = U32(ent, 0x1C);
		U32(d, 4) = U32(ent, 0x20);
		U16(d, 2) = 0;
		const int32_t size125 = mul32(S16(ent, 0x26), 125);
		const int32_t off = shl32(size125, 4) >> 12;
		if (slot <= 2)
			U16(d, 4) = (uint16_t)(U16(d, 4) + (uint16_t)(-0x200 - off));
		else
			U16(d, 4) = (uint16_t)(U16(d, 4) + (uint16_t)(off + 0x200));
		const int16_t dz = (int16_t)(U16(d, 0xC) - U16(d, 4));
		const int16_t dxv = (int16_t)(U16(d, 8) - U16(d, 0));
		const uint32_t angle = x::CartesianToGameAngle((uint32_t)(int32_t)dxv, (uint32_t)(int32_t)dz);
		U16(MEM<uint32_t>(game_addr(REAPER_DATA)), 0x4A) = (uint16_t)((angle - 0x800) & 0xFFF);
		return 0;
	}

	uint32_t __cdecl d_705ED0(uint32_t a1)
	{
		d_705F00(a1);
		x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_25571E0), game_addr(game::MAG_345_Lv5Death_Director), 0x48, a1);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl d_706060(uint32_t a1)
	{
		uint32_t d = MEM<uint32_t>(game_addr(REAPER_DATA));
		U16(d, 0x46) = (uint16_t)(U16(d, 0x46) + 1);
		const uint16_t req = U16(d, 0x42);
		if (U16(d, 0x40) != req)
		{
			U16(d, 0x40) = req;
			U16(d, 0x46) = 0;
			a_73A6D0();
			d = MEM<uint32_t>(game_addr(REAPER_DATA));
		}
		const uint16_t next = U16(d, 0x44);
		if (U16(d, 0x42) != next)
		{
			U16(d, 0x42) = next;
			a_73A6D0();
		}
		return a1;
	}

	uint32_t __cdecl d_705FF0(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[4];
		states[0] = game_addr(game::MAG_345_UploadReaperTIMs);
		states[1] = game_addr(game::MAG_345_SpawnReaperCreature);
		states[2] = game_addr(game::sub_70D1C0);
		states[3] = game_addr(game::nullsub_1064);
		d_706060(node);
		callp(states[S8(node, 0x29)], node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl d_7060B0(uint32_t a1)
	{
		if (U8(a1, 0x2E) == 0)
		{
			x::Battle_QueueTIMUpload_GetEOF(game_addr(game::MAG345_Reaper_BodyTIM));
			x::Battle_QueueTIMUpload_GetEOF(game_addr(game::MAG345_Reaper_WingsTIM));
		}
		const uint8_t st = U8(a1, 0x29);
		U16(a1, 0x44) = 1;
		U8(a1, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	uint32_t __cdecl d_7060F0(uint32_t a1)
	{
		U16(a1, 0x44) = (uint16_t)(U16(a1, 0x44) - 1);
		if (S16(a1, 0x44) > 0)
			return 0;
		const uint32_t n = x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_255EA30), game_addr(ORIG_Reaper), 0x140, a1);
		x::Effect_BindModelContainerSetAnim(n, game_addr(game::MAG345_Reaper_ModelContainer), 3);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl d_706550(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		(void)a3;
		const uint32_t ctx = a4;
		const uint32_t blk = U32(ctx, 0x48);
		uint32_t cursor = MEM<uint32_t>(U32(ctx, 0x4C));
		const uint32_t vbuf = U32(U32(ctx, 0x44), 4);
		uint32_t table = U32(a1, 4);
		const uint32_t bones = U32(a1, 0) + 0x10;
		const int32_t count = S32(table, 0);
		const uint32_t hdr = x::Field_Alloc(0x5C);
		const uint32_t mask = U32(blk, 0x7C);
		const uint32_t colour = U32(blk, 0x28) & 0xFFFFFF;
		table += 4;
		U32(hdr, 0x50) = mask;
		U32(hdr, 0x4C) = colour | 0x2C000000;
		U32(hdr, 0x48) = colour | 0x24000000;
		x::GTE_SetRotMatrix(ctx + 0x20);
		x::GTE_SetTransVector(ctx + 0x20);
		for (int32_t i = 0; i < count; i++)
		{
			uint32_t v = vbuf;
			table += 4;
			uint32_t obj = U32(a1, 4) + U32(table, -4);
			if (!((U32(hdr, 0x50) >> (i & 31)) & 1))
				continue;
			int32_t groups = S16(obj, 0);
			obj += 2;
			for (; groups > 0; groups--)
			{
				const int32_t bone = S16(obj, 0);
				obj += 2;
				const uint32_t m = bones + (uint32_t)(bone * 0x30) + 0x10;
				GTE_SetLightMatrix(m);
				GTE_SetBackgroundVector(U32(m, 0x14), U32(m, 0x18), U32(m, 0x1C));
				int32_t nv = S16(obj, 0);
				obj += 2;
				for (; nv > 0; nv--)
				{
					U16(hdr, 0x28) = U16(obj, 0);
					U16(hdr, 0x2A) = U16(obj, 2);
					U16(hdr, 0x2C) = U16(obj, 4);
					obj += 6;
					x::GTE_LoadV0(hdr + 0x28);
					GTE_MVMVA_LightV0_Bk();
					x::GTE_StoreIR123(hdr + 0x28);
					U32(v, 0) = U32(hdr, 0x28);
					U32(v, 4) = U32(hdr, 0x2C);
					v += 8;
				}
			}
			obj = (obj + 3) & ~3u;
			const int32_t n3 = S16(obj, 0);
			int32_t n4 = S16(obj, 2);
			uint32_t rec = obj + 0xC;
			uint32_t pkt = cursor;
			for (int32_t k = n3; k > 0; k--, rec += 0x10)
			{
				const uint32_t i0 = U16(rec, 0) & 0xFFF, i1 = U16(rec, 2) & 0xFFF, i2 = U16(rec, 4) & 0xFFF;
				U32(hdr, 0x38) = i0;
				U32(hdr, 0x3C) = i1;
				U32(hdr, 0x40) = i2;
				if (S16(vbuf + i0 * 8, 2) > 0 || S16(vbuf + i1 * 8, 2) > 0 || S16(vbuf + i2 * 8, 2) > 0)
					continue;
				x::GTE_LoadV012(vbuf + i0 * 8, vbuf + i1 * 8, vbuf + i2 * 8);
				x::GTE_RTPT();
				x::GTE_NCLIP();
				x::GTE_ReadMAC0(hdr + 0x54);
				if (S32(hdr, 0x54) <= 0)
					continue;
				x::GTE_AVSZ3();
				x::GTE_ReadOTZ(hdr + 0x58);
				U32(hdr, 0x58) = U32(hdr, 0x58) >> 2;
				GTE_StoreSXY012_PolyFT3(pkt);
				U32(pkt, 0xC) = U32(rec, 8);
				U32(pkt, 0x14) = U32(rec, 0xC);
				U16(pkt, 0x1C) = U16(rec, 6);
				U32(pkt, 4) = U32(hdr, 0x48);
				U32(pkt, 0) = 0x7000000;
				x::SSIGPU_InsertPrimAutoDepth(a2 + U32(hdr, 0x58) * 4, pkt);
				pkt += 0x20;
			}
			if (n4 > 0)
			{
				rec += 4;
				for (; n4 > 0; n4--, rec += 0x14)
				{
					const uint32_t i0 = U16(rec, -4) & 0xFFF, i1 = U16(rec, -2) & 0xFFF, i2 = U16(rec, 0) & 0xFFF;
					U32(hdr, 0x38) = i0;
					U32(hdr, 0x3C) = i1;
					U32(hdr, 0x40) = i2;
					if (S16(vbuf + i0 * 8, 2) > 0 || S16(vbuf + i1 * 8, 2) > 0 || S16(vbuf + i2 * 8, 2) > 0)
						continue;
					if (S16(vbuf + U32(hdr, 0x44) * 8, 2) > 0)
						continue;
					x::GTE_LoadV012(vbuf + i0 * 8, vbuf + i1 * 8, vbuf + i2 * 8);
					x::GTE_RTPT();
					x::GTE_NCLIP();
					x::GTE_ReadMAC0(hdr + 0x54);
					if (S32(hdr, 0x54) <= 0)
						continue;
					GTE_StoreSXY012_PolyFT3_2(pkt);
					const uint32_t i3 = U16(rec, 2) & 0xFFF;
					U32(hdr, 0x44) = i3;
					x::GTE_LoadV0(vbuf + i3 * 8);
					x::GTE_RTPS();
					x::GTE_ReadSXY2(pkt + 0x20);
					x::GTE_AVSZ4();
					x::GTE_ReadOTZ(hdr + 0x58);
					const uint32_t z = U32(hdr, 0x58) >> 2;
					U32(hdr, 0x58) = z;
					U32(pkt, 0xC) = U32(rec, 4);
					U16(pkt, 0x1C) = U16(rec, 0xC);
					U16(pkt, 0x24) = U16(rec, 0xE);
					U32(pkt, 0x14) = U32(rec, 8);
					U32(pkt, 0) = 0x9000000;
					U32(pkt, 4) = U32(hdr, 0x4C);
					x::SSIGPU_InsertPrimAutoDepth(a2 + z * 4, pkt);
					pkt += 0x28;
				}
			}
			cursor = pkt;
		}
		const uint32_t out = U32(ctx, 0x4C);
		x::Field_Free(0x5C);
		MEM<uint32_t>(out) = cursor;
		return out;
	}

	uint32_t __cdecl d_706450(uint32_t a1)
	{
		const uint32_t ctx = a1;
		const uint32_t blk = U32(ctx, 0x48);
		ff8fx::copy<(32)>((void *)ctx, (const void *)(blk + 0x40));
		const uint32_t anim = blk + 0x60;
		x::BattleModel_BuildBoneMatricesFromPose(anim);
		x::GTE_SetRotMatrix(game_addr(game::unk_1D97778));
		x::GTE_LoadIRFromMatrixColumn(ctx);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(ctx + 0x20);
		x::GTE_LoadIRFromMatrixColumn(ctx + 2);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(ctx + 0x22);
		x::GTE_LoadIRFromMatrixColumn(ctx + 4);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(ctx + 0x24);
		x::GTE_SetTransVector(game_addr(game::unk_1D97778));
		x::GTE_LoadV0FromDwords(ctx + 0x14);
		x::GTE_MVMVA_RotV0_Tr();
		x::GTE_ReadMAC123(ctx + 0x34);
		const uint32_t shadow = x::sub_5088A0(blk, MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x4040, 0x10, MEM<uint32_t>(U32(ctx, 0x4C)));
		MEM<uint32_t>(U32(ctx, 0x4C)) = shadow;
		d_706550(U32(blk, 0x64), MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 4, ctx);
		const uint32_t second = U32(blk, 0x78);
		if (second != 0)
			d_706550(U32(second, 4), MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 4, ctx);
		x::BattleModel_BuildBoneMatricesFromPose(anim);
		return 0;
	}

	uint32_t __cdecl d_7061F0(uint32_t a1)
	{
		const uint16_t f = U16(a1, 0x26);
		if (f & 4)
			return 0;
		if (f & 8)
		{
			x::ComposeZYXRotationMatrix(a1 + 0x3C, a1 + 0x70);
			S32(a1, 0x84) = S16(a1, 0x4C);
			S32(a1, 0x88) = S16(a1, 0x4E);
			S32(a1, 0x8C) = S16(a1, 0x50);
			x::scale3DMatrix(a1 + 0x70, a1 + 0x114);
			return d_706450(game_addr(REAPER_CTX));
		}
		MEM<uint32_t>(game_addr(PACKET_CURSOR)) = a_746C10(a1, game_addr(game::unk_255DC10), MEM<uint32_t>(game_addr(PACKET_CURSOR)));
		return 0;
	}

	uint32_t __cdecl d_706130(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[6];
		states[0] = game_addr(game::sub_706940);
		states[1] = game_addr(game::sub_70C8B0);
		states[2] = game_addr(game::sub_70C900);
		states[3] = game_addr(game::sub_70D030);
		states[4] = game_addr(game::sub_70D090);
		states[5] = game_addr(game::nullsub_1063);
		callp(states[S8(node, 0x29)], node);
		const int32_t pz = S16(node, 0x50);
		const int32_t py = S16(node, 0x4E);
		ff8fx::copy<(32)>((void *)game_addr(game::unk_1D99AB0), (const void *)(node + 0x70));
		const int32_t px = S16(node, 0x4C);
		S32(node, 0x8C) = pz;
		const uint8_t f = U8(node, 0x26);
		S32(node, 0x84) = px;
		S32(node, 0x88) = py;
		if (!(f & 4))
		{
			d_7061F0(node);
		}
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl d_70C8A0(uint32_t a1)
	{
		(void)a1;
		U16(MEM<uint32_t>(game_addr(game::unk_15297A0)), 0x20) = 0;
		return 0;
	}

	uint32_t __cdecl d_70C840(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		x::MAG_007_sub_8DCC00(a1, 0x54);
		U32(a1, 0x48) = a4;
		U32(a1, 0x40) = a2;
		U32(a1, 0x4C) = game_addr(PACKET_CURSOR);
		U32(a1, 0x50) = game_addr(PACKET_CURSOR);
		U32(a3, 4) = a2;
		U32(a1, 0x44) = a3;
		U16(a3, 0x14) = 0;
		U16(a3, 0x16) = 0;
		U16(a3, 0x18) = 0x140;
		U16(a3, 0x1A) = 0;
		U8(a3, 0x1C) = 0x80;
		U8(a3, 0x1D) = 0x80;
		U8(a3, 0x1E) = 0x80;
		U32(a3, 0x20) = 0xFFFFFFFF;
		U16(a3, 0x24) = 0;
		return a3;
	}

	uint32_t __cdecl d_70C810(uint32_t a1)
	{
		d_70C840(game_addr(REAPER_CTX), game_addr(game::MAG_345_Lv5Death_AndItsVertexBuffer), game_addr(game::unk_255EA98), a1 + 0x30);
		U8(a1, 0x26) |= 8;
		return 0;
	}

	uint32_t __cdecl d_706940(uint32_t a1)
	{
		const uint32_t d = MEM<uint32_t>(game_addr(REAPER_DATA));
		U32(a1, 0x4C) = U32(d, 0);
		const uint32_t w4 = U32(d, 4);
		const uint16_t yaw = U16(d, 0x4A);
		U32(a1, 0x60) = a1 + 0x114;
		U32(a1, 0x50) = w4;
		U16(a1, 0x3E) = yaw;
		U32(a1, 0x11C) = 0x1000;
		U32(a1, 0x118) = 0x1000;
		U32(a1, 0x114) = 0x1000;
		d_70C8A0(a1);
		if (U8(a1, 0x2E) == 0)
		{
			x::au_re_Battle_ReadAnimation_7(a1, 5);
			d_70C810(a1);
			a_73C100(a1, game_addr(ORIG_ActorSystem), game_addr(game::unk_1643698), 0, 0x14, 2);
			x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_25572E8), game_addr(game::sub_70C720), 0x70, a1);
			x::BdPlaySE(game_addr(game::unk_152465C), 0, 0x80);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
			return 0;
		}
		x::au_re_Battle_ReadAnimation_7(a1, 6);
		U16(a1, 0x13A) = 0x1000;
		U16(a1, 0x13C) = 0x1000;
		d_70C590(a1, 1);
		d_70C6B0(a1);
		x::BdPlaySE(game_addr(game::unk_1524660), 0, 0x80);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 2);
		return 0;
	}

	uint32_t __cdecl d_70C8B0(uint32_t a1)
	{
		if (U16(a1, 0x136) == 0x14)
			U8(a1, 0x26) &= 0xF7;
		if (x::au_re_Battle_ReadAnimation_8(a1) == 1)
		{
			U8(a1, 0x26) &= 0xF7;
			x::au_re_Battle_ReadAnimation_7(a1, 6);
			x::BdPlaySE(game_addr(game::unk_1524660), 0, 0x80);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl d_70C900(uint32_t a1)
	{
		U16(a1, 0x13A) = (uint16_t)(U16(a1, 0x13A) + 0xFC00);
		if (S16(a1, 0x13A) <= 0)
			U16(a1, 0x13A) = 0;
		U16(a1, 0x13C) = U16(a1, 0x13A);
		d_70C590(a1, 1);
		d_70C6B0(a1);
		if (U16(a1, 0x136) == 5)
		{
			const int32_t action = S8(a1, 0x2A);
			U16(MEM<uint32_t>(game_addr(REAPER_DATA)), 0x48) = 1;
			const uint32_t targets = U32(U32(U32(a1, 0xC), 4) + (uint32_t)(action * 20), 8);
			const int32_t target = S8(a1, 0x2B);
			if (!(U8(targets + (uint32_t)(target * 24), 3) & 4))
			{
				a_73C100(a1, game_addr(ORIG_ActorSystem), game_addr(game::unk_164808C), 0, 0x14, 0);
				x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_2557760), game_addr(game::sub_70C9E0), 0x118, a1);
			}
		}
		const uint32_t r = x::au_re_Battle_ReadAnimation_8(a1);
		if (U8(a1, 0x2E) == U8(a1, 0x2F))
		{
			if ((uint16_t)r == 1)
				U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
			return 0;
		}
		if (S16(a1, 0x136) >= 0xB)
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl d_70D030(uint32_t a1)
	{
		x::au_re_Battle_ReadAnimation_8(a1);
		U16(a1, 0x13A) = (uint16_t)(U16(a1, 0x13A) + 0x200);
		if (S16(a1, 0x13A) >= 0x1000)
		{
			const uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) |= 4;
			U16(a1, 0x13A) = 0x1000;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		U16(a1, 0x13C) = U16(a1, 0x13A);
		d_70C590(a1, 1);
		d_70C6B0(a1);
		return 0;
	}

	uint32_t __cdecl d_70D090(uint32_t a1)
	{
		if (U8(a1, 0x2E) == U8(a1, 0x2F))
			x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_25572E8), game_addr(game::sub_70D0E0), 0x70, a1);
		const uint32_t d = MEM<uint32_t>(game_addr(REAPER_DATA));
		U16(a1, 0x26) |= 1;
		U16(d, 0x4C) = 1;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl d_70C780(uint32_t a1)
	{
		if (S16(a1, 0x24) < 0x1C)
			return 0;
		U16(a1, 0x1C) = 0;
		uint32_t p = game_addr(game::unk_1D989BA);
		for (int i = 4; i != 0; i--, p += 0x2C)
		{
			MEM<uint16_t>(p - 0x28) = 0;
			MEM<uint8_t>(p) = 0;
			MEM<uint8_t>(p - 1) = 0;
			MEM<uint8_t>(p - 2) = 0;
		}
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl d_70C7C0(uint32_t a1)
	{
		U16(a1, 0x1C) = (uint16_t)(U16(a1, 0x1C) + 0x200);
		if (S16(a1, 0x1C) >= 0x600)
		{
			const uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) |= 1;
			U16(a1, 0x1C) = 0x600;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		const uint16_t level = U16(a1, 0x1C);
		uint32_t p = game_addr(game::unk_1D98992);
		for (int i = 4; i != 0; i--, p += 0x2C)
			MEM<uint16_t>(p) = level;
		return 0;
	}

	uint32_t __cdecl d_70D140(uint32_t a1)
	{
		if (S16(a1, 0x24) < 0)
			return a1;
		const uint8_t st = U8(a1, 0x29);
		U16(a1, 0x1C) = 0x600;
		U8(a1, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	uint32_t __cdecl d_70CA40(uint32_t a1)
	{
		const uint32_t slot = U8(a1, 0x2D);
		x::GetEffectSpawnPosition(game_addr(game::BattleEntitySlotData) + slot * 0x9C, 0xF1, 0, a1 + 0x1C);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	static void SparkGroup(uint32_t a1, uint32_t base, uint32_t m2, uint32_t pitch_mask, uint32_t pitch_add, int32_t pitch_sub)
	{
		const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_2557760), game_addr(ORIG_Spark), 0x118, a1);
		uint32_t vel = t + 0xF4;
		uint32_t mat = t + 0x54;
		for (int k = 4; k != 0; k--, mat += 0x20, vel += 8)
		{
			U32(vel, -0x20) = U32(a1, 0x1C);
			U32(vel, -0x1C) = U32(a1, 0x20);
			const int16_t yaw = (int16_t)(x::CrtRand() & 0xFFF);
			const uint32_t r = x::CrtRand();
			ff8fx::copy<(32)>((void *)m2, (const void *)base);
			const uint32_t pitch = ((r & pitch_mask) + (uint32_t)pitch_add - (uint32_t)pitch_sub) & 0xFFF;
			x::MAG_022_sub_8DD8A0(m2, (uint32_t)(int32_t)yaw);
			x::sub_8DD7E0(m2, (uint32_t)(int32_t)(int16_t)pitch);
			ff8fx::copy<(32)>((void *)mat, (const void *)m2);
			const uint32_t r3 = x::CrtRand() & 0x1FFF;
			U16(vel, 2) = (uint16_t)(0xFFFFF000u - r3);
			x::matrixMultiplyVector(m2, vel, vel);
		}
	}

	uint32_t __cdecl d_70CA80(uint32_t a1)
	{
		alignas(4) uint8_t base[32] = {}, m2[32] = {};
		const uint32_t slot = U8(a1, 0x2D);
		const uint32_t ent = game_addr(game::BattleEntitySlotData) + slot * 0x9C;
		x::MAG_022_sub_8DD770(P(base));
		x::MAG_022_sub_8DD8A0(P(base), (uint32_t)(int32_t)S16(ent, 0xE));
		x::sub_8DD960(P(base), 0x155);
		x::sub_8DD7E0(P(base), 0xEAB);
		const int16_t c = S16(a1, 0x24);
		int32_t n = 0;
		if (c == 1) n = 4;
		else if (c == 2) n = 3;
		else if (c == 3) n = 2;
		else if (c == 4) n = 1;
		for (; n > 0; n--)
			SparkGroup(a1, P(base), P(m2), 0x1FF, 0, 0x100);
		n = 0;
		if (c == 1 || c == 2) n = 2;
		else if (c == 3 || c == 4) n = 1;
		for (; n > 0; n--)
			SparkGroup(a1, P(base), P(m2), 0xFF, 0x380, 0);
		if (S16(a1, 0x24) >= 4)
		{
			const uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl d_70CEC0(uint32_t a1, uint32_t a2)
	{
		if (U8(a1, 0x26) & 4)
			return 0;
		alignas(4) uint8_t mb[32];
		const uint32_t M = P(mb);
		ff8fx::copy<(32)>(mb, (const void *)a2);
		S32(M, 0x14) = S16(a1, 0x1C);
		S32(M, 0x18) = S16(a1, 0x1E);
		S32(M, 0x1C) = S16(a1, 0x20);
		x::scale3DMatrix(M, a1 + 0x30);
		x::ComposeAffineTransform(game_addr(game::unk_1D97778), M, M);
		x::GTE_SetRotMatrix_W(M);
		x::GTE_SetTransVector_W(M);
		const uint32_t h = x::Field_Alloc(0x68);
		U32(h, 0) = U32(a1, 0x4C);
		U32(h, 8) = U32(a1, 0x40);
		U32(h, 0xC) = (uint32_t)(int32_t)S16(a1, 0x50);
		U16(h, 0x22) = 0x100;
		U16(h, 0x20) = 0x100;
		U16(h, 0x2A) = 0x100;
		U16(h, 0x28) = 0x100;
		U32(h, 0x10) = (uint32_t)(int32_t)S16(a1, 0x52);
		const uint32_t ot = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44;
		U16(h, 0x18) = 0;
		U16(h, 0x1A) = 0;
		U16(h, 0x1E) = 0;
		U16(h, 0x1C) = 0;
		U16(h, 0x26) = 0;
		U16(h, 0x24) = 0;
		U32(h, 0x14) = 0xF0;
		MEM<uint32_t>(game_addr(PACKET_CURSOR)) = d_707820(h, ot, 2, MEM<uint32_t>(game_addr(PACKET_CURSOR)));
		x::Field_Free(0x68);
		return 0;
	}

	static void SparkMove(uint32_t node)
	{
		uint32_t v = node + 0xF4;
		for (int k = 4; k != 0; k--, v += 8)
		{
			for (int j = 0; j < 3; j++)
			{
				const int16_t s = S16(v, 2 * j);
				S16(v, 2 * j) = (int16_t)(s - (int16_t)(s / 4));
			}
			for (int j = 0; j < 3; j++)
				S16(v, -0x20 + 2 * j) = (int16_t)(S16(v, -0x20 + 2 * j) + (int16_t)(S16(v, 2 * j) / 16));
		}
	}

	static void SparkDraw(uint32_t node)
	{
		const int32_t e = S16(node, 0x24) * 2;
		S32(node, 0x30) = MEM<int16_t>(game_addr(game::unk_1529720) + e);
		S32(node, 0x34) = MEM<int16_t>(game_addr(game::unk_1529740) + e);
		S32(node, 0x38) = MEM<int16_t>(game_addr(game::unk_1529760) + e);
		U16(node, 0x50) = MEM<uint16_t>(game_addr(game::unk_1529780) + e);
		uint32_t pos = node + 0xD4;
		uint32_t mat = node + 0x54;
		for (int k = 4; k != 0; k--, pos += 8, mat += 0x20)
		{
			U32(node, 0x1C) = U32(pos, 0);
			U32(node, 0x20) = U32(pos, 4);
			d_70CEC0(node, mat);
		}
	}

	uint32_t __cdecl d_70CD70(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[3];
		states[0] = game_addr(game::sub_70CFC0);
		states[1] = game_addr(game::sub_70CFF0);
		states[2] = game_addr(game::nullsub_1060);
		callp(states[S8(node, 0x29)], node);
		SparkMove(node);
		SparkDraw(node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl d_70CFC0(uint32_t a1)
	{
		U32(a1, 0x38) = 0x1000;
		U32(a1, 0x34) = 0x1000;
		U32(a1, 0x30) = 0x1000;
		const uint8_t st = U8(a1, 0x29);
		U32(a1, 0x4C) = game_addr(game::unk_1524664);
		U8(a1, 0x40) = 0x28;
		U8(a1, 0x41) = 0;
		U8(a1, 0x42) = 0xA;
		U8(a1, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	uint32_t __cdecl d_70CFF0(uint32_t a1)
	{
		if (S16(a1, 0x24) >= 0xE)
		{
			const uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl d_7076C0(uint32_t a1, uint32_t a2)
	{
		const uint32_t h = MEM<uint32_t>(game_addr(SCRATCH_SP)) - 0x68;
		U32(h, 0) = U32(a1, 0x170);
		const bool def3a = U16(a2, 0x3A) != 0;
		MEM<uint32_t>(game_addr(SCRATCH_SP)) = h;
		U32(h, 0x14) = 0;
		if (!def3a)
			U32(h, 0x14) = 0x30;
		const uint16_t fade = U16(a1, 0x1CE);
		if (fade != 0)
		{
			U32(h, 8) = U32(a1, 0x16C);
			const uint32_t fl = U32(h, 0x14);
			U32(h, 0xC) = (uint32_t)(int32_t)(int16_t)fade;
			U32(h, 0x14) = fl | 0xC0;
		}
		U32(h, 0x10) = 0;
		U16(h, 0x22) = 0x100;
		U16(h, 0x20) = 0x100;
		U16(h, 0x2A) = 0x100;
		U16(h, 0x28) = 0x100;
		const int8_t copies = S8(a1, 0x1D8);
		U16(h, 0x1E) = 0;
		U16(h, 0x1C) = 0;
		U16(h, 0x18) = 0;
		U16(h, 0x1A) = 0;
		U16(h, 0x26) = 0;
		U16(h, 0x24) = 0;
		if (copies == 1)
		{
			x::GTE_SetRotMatrix(a1 + 0xAC);
			x::GTE_SetTransVector(a1 + 0xAC);
			MEM<uint32_t>(game_addr(PACKET_CURSOR)) = d_707820(h, MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 2, MEM<uint32_t>(game_addr(PACKET_CURSOR)));
		}
		else if (copies > 0)
		{
			uint32_t pos = a1 + 0x19E;
			for (int32_t i = 0; i < S8(a1, 0x1D8); i++, pos += 8)
			{
				S32(a1, 0xC0) = S16(pos, -2);
				S32(a1, 0xC4) = S16(pos, 0);
				S32(a1, 0xC8) = S16(pos, 2);
				x::GTE_SetRotMatrix(a1 + 0xAC);
				x::GTE_SetTransVector(a1 + 0xAC);
				MEM<uint32_t>(game_addr(PACKET_CURSOR)) = d_707820(h, MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 2, MEM<uint32_t>(game_addr(PACKET_CURSOR)));
			}
		}
		const uint32_t top = MEM<uint32_t>(game_addr(SCRATCH_SP)) + 0x68;
		MEM<uint32_t>(game_addr(SCRATCH_SP)) = top;
		return top;
	}

	uint32_t __cdecl d_709B00(uint32_t a1)
	{
		const uint32_t act = a1;
		const uint32_t hdr = MEM<uint32_t>(game_addr(SCRATCH_SP)) - 0xB4;
		MEM<uint32_t>(game_addr(SCRATCH_SP)) = hdr;
		const uint32_t mtx = act + 0xAC;
		x::GTE_SetRotMatrix(mtx);
		x::GTE_SetTransVector(mtx);
		const uint32_t seq = U32(act, 0x170);
		const uint32_t cursor = MEM<uint32_t>(game_addr(PACKET_CURSOR));
		const int16_t variant = (int16_t)S8(act, 0x1D0);
		U32(hdr, 0) = seq;
		const uint32_t ot = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44;
		S16(hdr, 4) = variant;
		U16(hdr, 0x24) = 0;
		MEM<uint32_t>(game_addr(PACKET_CURSOR)) = x::InitEffectSequenceFromData_c19(hdr, ot, 2, cursor);
		MEM<uint32_t>(game_addr(SCRATCH_SP)) = MEM<uint32_t>(game_addr(SCRATCH_SP)) + 0xB4;
		return 0;
	}

	uint32_t __cdecl d_70ADB0(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		const uint32_t pool = MEM<uint32_t>(game_addr(game::unk_255EA44));
		int32_t idx = MEM<int16_t>(game_addr(game::unk_2557758));
		uint32_t slot = 0;
		int32_t tries = 0;
		do
		{
			if (U8(pool + (uint32_t)(idx * 27) * 4, 0x69) == 0)
			{
				slot = pool + (uint32_t)(idx * 27) * 4;
				x::MAG_007_sub_8DCC00(slot, 0x6C);
				U8(slot, 0x6A) = (uint8_t)a2;
				const uint32_t dir = MEM<uint32_t>(game_addr(ACTOR_STATE));
				U8(slot, 0x69) = 1;
				U16(dir, 0x14) = (uint16_t)(U16(dir, 0x14) + 1);
				U32(slot, 0x5C) = a1;
				U8(slot, 0x6B) = (uint8_t)a3;
				a_742CA0(slot, 0);
				break;
			}
			idx++;
			if (idx >= 0x13)
				idx = 0;
			tries++;
		} while (tries < 0x14);
		idx++;
		if (idx < 0x13)
			MEM<uint16_t>(game_addr(game::unk_2557758)) = (uint16_t)idx;
		else
			MEM<uint16_t>(game_addr(game::unk_2557758)) = 0;
		return slot;
	}

	uint32_t __cdecl d_70B730(uint32_t a1, uint32_t a2)
	{
		const uint32_t pool = MEM<uint32_t>(game_addr(game::unk_255EA88));
		int32_t i = MEM<int16_t>(game_addr(game::unk_255EA50));
		uint32_t slot = 0;
		int32_t tries = 0;
		for (;;)
		{
			const uint32_t off = (uint32_t)mul32(i, 0x2A0);
			if (U8(off + pool, 0x1D7) == 0)
			{
				slot = (uint32_t)mul32(i, 0x2A0) + pool;
				x::MAG_007_sub_8DCC00(slot, 0x2A0);
				const uint8_t def_idx = (uint8_t)a2;
				U32(slot, 0x1BC) = a1;
				const uint8_t b6b = U8(a1, 0x6B);
				const uint32_t sys = MEM<uint32_t>(game_addr(ACTOR_STATE));
				U8(slot, 0x1D7) = 1;
				U16(sys, 0x16) = (uint16_t)(U16(sys, 0x16) + 1);
				U8(slot, 0x1D6) = def_idx;
				U8(slot, 0x1D9) = b6b;
				a_742CA0(slot, 1);
				break;
			}
			i++;
			if (i >= 0x3B)
				i = 0;
			tries++;
			if (tries >= 0x3C)
				break;
		}
		i++;
		if (i >= 0x3B)
			MEM<uint16_t>(game_addr(game::unk_255EA50)) = 0;
		else
			MEM<uint16_t>(game_addr(game::unk_255EA50)) = (uint16_t)i;
		return slot;
	}

	uint32_t __cdecl d_705C00(uint32_t a1)
	{
		g_mod = &MOD_345;

		uint32_t states[11];
		ff8fx::copy<(8 * 4)>((void *)game_addr(game::Effect_SharedCamMatrixSnapshot), (const void *)game_addr(game::unk_1D97778));
		const uint32_t node = a1;
		MEM<uint32_t>(game_addr(game::MAG_345_Lv5Death_ScratchStackPointer)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		MEM<uint32_t>(game_addr(game::unk_2557208)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		states[0] = game_addr(game::MAG_345_sub_705D80);
		const uint8_t parity = U8(node, 0x5C);
		states[1] = game_addr(game::MAG_345_sub_705D90);
		states[2] = game_addr(game::MAG_345_sub_705DA0);
		states[3] = game_addr(game::MAG_345_sub_705E20);
		states[4] = game_addr(game::MAG_345_sub_70D2A0);
		states[5] = game_addr(game::MAG_345_sub_70D2E0);
		states[6] = game_addr(game::MAG_345_sub_70D2F0);
		states[7] = game_addr(game::MAG_345_sub_70D300);
		states[8] = game_addr(game::MAG_345_sub_70D330);
		states[9] = game_addr(game::MAG_345_sub_70D340);
		states[10] = game_addr(game::nullsub_1057);
		if ((parity & 1) != 0)
		{
			const uint32_t v1 = MEM<uint32_t>(game_addr(game::unk_255DC0C));
			const uint32_t v2 = MEM<uint32_t>(game_addr(game::unk_255CD64));
			MEM<uint32_t>(game_addr(game::MAG_345_Lv5Death_PacketCursor)) = v1;
			MEM<uint32_t>(game_addr(game::unk_2557204)) = v2;
		}
		else
		{
			const uint32_t v1 = MEM<uint32_t>(game_addr(game::unk_255DC08));
			const uint32_t v2 = MEM<uint32_t>(game_addr(game::unk_255CD60));
			MEM<uint32_t>(game_addr(game::MAG_345_Lv5Death_PacketCursor)) = v1;
			MEM<uint32_t>(game_addr(game::unk_2557204)) = v2;
		}
		x::Effect_UpdateTargetPosFromBones(node);
		callp(states[S8(node, 0x29)], node);
		U16(node, 0x5E) = 0;
		MEM<uint16_t>(game_addr(game::unk_255EA7C)) = 0;
		MEM<uint16_t>(game_addr(game::unk_255EA40)) = 0;
		const uint32_t queues[7] = { game_addr(game::unk_255DA98), game_addr(game::unk_25571E0), game_addr(game::unk_255EA30), game_addr(game::unk_255D7F8),
			game_addr(game::unk_25572E8), game_addr(game::unk_2557760), game_addr(game::unk_255DA88) };
		for (int i = 0; i < 7; i++)
		{
			const uint32_t n = x::ExecuteTaskQueue(queues[i]);
			U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)n);
		}
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x5C) = (uint16_t)(U16(node, 0x5C) + 1);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		if ((status & 1) != 0 && U8(node, 0x28) == 0)
		{
			x::Effect_ReleaseLinkedTask(node);
			return 2;
		}
		return 0;
	}

	uint32_t __cdecl d_706AE0(uint32_t a1)
	{
		if (S16(a1, 0x24) >= S16(a1, 0x298))
		{
			d_706B10(a1);
			d_7075D0(a1);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl d_706CA0(uint32_t a1)
	{
		const uint32_t arena = MEM<uint32_t>(game_addr(game::MAG_345_Lv5Death_ActorState));
		const uint32_t v = arena + 0x1B4;
		U32(v, 0) = (uint32_t)shl32((int32_t)S16(a1, 0x290), 16);
		const int32_t y = S16(a1, 0x292);
		const int32_t z = S16(a1, 0x294);
		U32(arena, 0x1B8) = (uint32_t)shl32(y, 16);
		U32(arena, 0x1BC) = (uint32_t)shl32(z, 16);
		b1_copy16(arena + 0x204, v);
		b1_copy16(arena + 0x1F4, v);
		b1_copy16(arena + 0x1E4, v);
		b1_copy16(arena + 0x1D4, v);
		b1_copy16(arena + 0x1C4, v);
		return U32(v, 0xC);
	}

	uint32_t __cdecl d_706F20(void)
	{
		const uint32_t arena = MEM<uint32_t>(game_addr(game::MAG_345_Lv5Death_ActorState));
		int32_t i = 0;
		if (S16(arena, 0x1C) <= 0)
			return arena;
		const uint32_t src = arena + 0x194;
		uint32_t dst = arena + 0x94;
		do
		{
			i++;
			b1_copy16(dst - 0x40, src);
			b1_copy16(dst, src);
			b1_copy16(dst + 0x40, src);
			b1_copy16(dst + 0x80, src);
			b1_copy16(dst + 0xC0, src);
			dst += 0x10;
		} while (i < (int32_t)S16(arena, 0x1C));
		return dst;
	}

	uint32_t __cdecl d_7075D0(uint32_t a1)
	{
		uint32_t done = 0;
		uint32_t sys = a1 + 0x34;
		U32(game_addr(game::MAG_345_Lv5Death_ActorState), 0) = sys;
		uint16_t phase = U16(sys, 0x20);
		switch ((int16_t)phase)
		{
		case 0:
			U16(sys, 0x20) = (uint16_t)(phase + 1);
			break;
		case 1:
			d_70C4D0();
			d_709B80();
			d_709A40();
			d_707660();
			sys = U32(game_addr(game::MAG_345_Lv5Death_ActorState), 0);
			U16(sys, 0x18) = (uint16_t)(U16(sys, 0x18) + 1);
			if (S16(sys, 0x18) >= S16(sys, 0x1A) && U16(sys, 0x14) == 0 && U16(sys, 0x16) == 0)
				U16(sys, 0x20) = (uint16_t)(U16(sys, 0x20) + 1);
			break;
		case 2:
			done = 1;
			break;
		default:
			break;
		}
		uint16_t c14 = U16(sys, 0x14);
		uint16_t c16 = U16(sys, 0x16);
		U16(game_addr(game::unk_255EA7C), 0) = (uint16_t)(U16(game_addr(game::unk_255EA7C), 0) + c14);
		U16(game_addr(game::unk_255EA40), 0) = (uint16_t)(U16(game_addr(game::unk_255EA40), 0) + c16);
		return done;
	}

	uint32_t __cdecl d_707660(void)
	{
		uint32_t act = U32(U32(game_addr(game::MAG_345_Lv5Death_ActorState), 0), 0x2C);
		while (act != 0)
		{
			if (U16(act, 8) == 1)
			{
				uint32_t sys = U32(game_addr(game::MAG_345_Lv5Death_ActorState), 0);
				int32_t k = S8(act, 0x1D6);
				uint32_t def = U32(U32(sys, 0x224), k * 4);
				uint8_t kind = U8(def, 0x16);
				if ((kind == 4 || kind == 5) && U32(act, 0x170) != 0)
					d_7076C0(act, def);
			}
			act = U32(act, 4);
		}
		return 0;
	}

	uint32_t __cdecl d_707820(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		const uint32_t hdr = a1;
		if ((U32(hdr, 0x14) & 0x2000) == 0)
			U32(hdr, 4) = U32(hdr, 0) + 8;
		{
			const uint32_t model = U32(hdr, 0);
			const uint32_t b = U8(hdr, 0xA);
			U32(hdr, 0x2C) = U32(model, 0) + model;
			const uint32_t g = U8(hdr, 9);
			const uint32_t r = U8(hdr, 8);
			x::someCameraWork_45DD60(r, g, b);
		}
		uint32_t cursor = a4;
		typedef uint32_t (__cdecl *Draw)(uint32_t, uint32_t, uint32_t, uint32_t);
		static const Draw lists[8] = { d_707950, d_707B70, d_707E00, d_7082F0, a_73D770, a_73D9C0, d_708E10, d_709370 };
		for (int i = 0; i < 8; i++)
		{
			const uint32_t list = U32(hdr, 0x2C);
			if (U32(list, 0) != 0)
				cursor = lists[i](hdr, a2, a3, cursor);
			else
				U32(hdr, 0x2C) = list + 4;
		}
		return cursor;
	}

	uint32_t __cdecl d_707950(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		const uint32_t ctx = a1;
		uint32_t cursor = a4;
		const uint32_t list = U32(ctx, 0x2C);
		uint32_t count = U32(list, 0);
		uint32_t rec = list + 4;
		U32(ctx, 0x2C) = rec;
		const uint32_t vbase = U32(ctx, 4);
		if ((int32_t)count <= 0)
		{
			U32(ctx, 0x2C) = rec;
			return a4;
		}
		uint32_t pkt = cursor + 0xC;
		do
		{
			x::GTE_LoadV012(vbase + (uint32_t)U16(rec, 4) * 4, vbase + (uint32_t)U16(rec, 6) * 4, vbase + (uint32_t)U16(rec, 8) * 4);
			x::GTE_RTPT();
			const uint32_t flags = U32(ctx, 0x14);
			uint32_t code = U32(rec, 0);
			U32(cursor, 0) = 0x4000000;
			U32(pkt, -8) = code;
			if (flags & 1)
			{
				code |= 0x2000000;
				U32(pkt, -8) = code;
			}
			if (flags & 4)
				U32(pkt, -8) &= 0xFDFFFFFF;
			x::GTE_ReadFLAG(ctx + 0x3C);
			if (!(U32(ctx, 0x3C) & 0x60000))
			{
				x::GTE_NCLIP();
				uint32_t clip = 0;
				x::GTE_ReadMAC0(ctx + 0x30);
				if (S32(ctx, 0x30) >= 0 || (U8(ctx, 0x14) & 0x10))
				{
					x::GTE_ReadSXY012_Split(pkt - 4, pkt, pkt + 4);
					x::GTE_AVSZ3();
					if (t1_out_x(S16(pkt, -4))) clip = 1;
					if (t1_out_x(S16(pkt, 0))) clip |= 2;
					if (t1_out_x(S16(pkt, 4))) clip |= 4;
					if (t1_out_y(S16(pkt, -2))) clip |= 0x10;
					if (t1_out_y(S16(pkt, 2))) clip |= 0x20;
					if (t1_out_y(S16(pkt, 6))) clip |= 0x40;
					if ((uint8_t)(clip & 7) != 7 && (uint8_t)(clip & 0x70) != 0x70)
					{
						x::GTE_ReadOTZ(ctx + 0x38);
						if (U8(ctx, 0x14) & 0x40)
						{
							x::set_unk_1CA8A28(pkt - 8);
							x::set_dword_1CA8A30(U32(ctx, 0x0C));
							x::sub_45F270();
							x::set_param_with_dword_1CA8A68(pkt - 8);
						}
						int32_t z = add32(S32(ctx, 0x38), S32(ctx, 0x10));
						S32(ctx, 0x38) = z;
						if (z < 0)
							S32(ctx, 0x38) = 0;
						z = S32(ctx, 0x38) >> (a3 & 31);
						x::SSIGPU_InsertPrimAutoDepth(a2 + (uint32_t)z * 4, cursor);
						cursor += 0x14;
						pkt += 0x14;
					}
				}
			}
			rec += 0xC;
		} while (--count);
		U32(ctx, 0x2C) = rec;
		return cursor;
	}

	uint32_t __cdecl d_707B70(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		const uint32_t ctx = a1;
		uint32_t pkt = a4;
		uint32_t rec = U32(ctx, 0x2C);
		int32_t count = S32(rec, 0);
		rec += 4;
		U32(ctx, 0x2C) = rec;
		const uint32_t vbase = U32(ctx, 4);
		if (count <= 0)
		{
			U32(ctx, 0x2C) = rec;
			return pkt;
		}
		do
		{
			x::GTE_LoadV012(vbase + (uint32_t)U16(rec, 4) * 4u, vbase + (uint32_t)U16(rec, 6) * 4u,
				vbase + (uint32_t)U16(rec, 8) * 4u);
			x::GTE_RTPT();
			{
				const uint32_t flags = U32(ctx, 0x14);
				const uint32_t code = U32(rec, 0);
				U32(pkt, 0) = 0x5000000;
				U32(pkt, 4) = code;
				if (flags & 1)
					U32(pkt, 4) = code | 0x2000000;
				if (flags & 4)
					U32(pkt, 4) = U32(pkt, 4) & 0xFDFFFFFFu;
			}
			x::GTE_ReadFLAG(ctx + 0x3C);
			if ((U32(ctx, 0x3C) & 0x60000) != 0)
				goto next;
			x::GTE_NCLIP();
			{
				uint32_t clip = 0;
				x::GTE_ReadMAC0(ctx + 0x30);
				if (S32(ctx, 0x30) < 0 && !(U8(ctx, 0x14) & 0x10))
					goto next;
				x::GTE_ReadSXY012_Split(pkt + 8, pkt + 0xC, pkt + 0x10);
				x::GTE_LoadV0(vbase + (uint32_t)U16(rec, 0xA) * 4u);
				x::GTE_RTPS();
				if (t2_outx(pkt + 8))
					clip = 1;
				if (t2_outx(pkt + 0xC))
					clip |= 2;
				if (t2_outx(pkt + 0x10))
					clip |= 4;
				if (t2_outy(pkt + 0xA))
					clip |= 0x10;
				if (t2_outy(pkt + 0xE))
					clip |= 0x20;
				if (t2_outy(pkt + 0x12))
					clip |= 0x40;
				x::GTE_ReadSXY2(pkt + 0x14);
				x::GTE_AVSZ4();
				if (t2_outx(pkt + 0x14))
					clip |= 8;
				if (t2_outy(pkt + 0x16))
					clip |= 0x80;
				if ((clip & 0xF) == 0xF)
					goto next;
				if ((clip & 0xF0) == 0xF0)
					goto next;
			}
			x::GTE_ReadOTZ(ctx + 0x38);
			if (U8(ctx, 0x14) & 0x40)
				t2_depth_cue(ctx, pkt + 4);
			{
				const uint32_t ot = t2_ot(ctx, a2, a3);
				x::SSIGPU_InsertPrimAutoDepth(ot, pkt);
				pkt += 0x18;
			}
		next:
			rec += 0xC;
			--count;
		} while (count != 0);
		U32(ctx, 0x2C) = rec;
		return pkt;
	}

	uint32_t __cdecl d_707E00(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		const uint32_t ctx = a1;
		uint32_t pkt = a4;
		uint32_t rec = U32(ctx, 0x2C);
		int32_t count = S32(rec, 0);
		rec += 4;
		const uint32_t vbase = U32(ctx, 4);
		U32(ctx, 0x2C) = rec;
		if (count <= 0)
		{
			U32(ctx, 0x2C) = rec;
			return pkt;
		}
		do
		{
			x::GTE_LoadV012(vbase + (uint32_t)U16(rec, 4) * 4u, vbase + (uint32_t)U16(rec, 6) * 4u,
				vbase + (uint32_t)U16(rec, 8) * 4u);
			x::GTE_RTPT();
			{
				const uint32_t flags = U32(ctx, 0x14);
				const uint32_t code = U32(rec, 0);
				U32(pkt, 0) = 0x7000000;
				U32(pkt, 4) = code;
				if (flags & 1)
					U32(pkt, 4) = code | 0x2000000;
				if (flags & 4)
					U32(pkt, 4) = U32(pkt, 4) & 0xFDFFFFFFu;
				const uint32_t uv2 = U32(rec, 8) >> 16;
				U32(pkt, 0xC) = U32(rec, 0xC);
				U32(pkt, 0x14) = U32(rec, 0x10);
				U32(pkt, 0x1C) = uv2;
			}
			x::GTE_ReadFLAG(ctx + 0x3C);
			if ((U32(ctx, 0x3C) & 0x60000) != 0)
				goto next;
			x::GTE_NCLIP();
			{
				uint32_t clip = 0;
				x::GTE_ReadMAC0(ctx + 0x30);
				if (S32(ctx, 0x30) < 0 && !(U8(ctx, 0x14) & 0x10))
					goto next;
				x::GTE_ReadSXY012_Split(pkt + 8, pkt + 0x10, pkt + 0x18);
				x::GTE_AVSZ3();
				if (t2_outx(pkt + 8))
					clip = 1;
				if (t2_outx(pkt + 0x10))
					clip |= 2;
				if (t2_outx(pkt + 0x18))
					clip |= 4;
				if (t2_outy(pkt + 0xA))
					clip |= 0x10;
				if (t2_outy(pkt + 0x12))
					clip |= 0x20;
				if (t2_outy(pkt + 0x1A))
					clip |= 0x40;
				if ((clip & 7) == 7)
					goto next;
				if ((clip & 0x70) == 0x70)
					goto next;
			}
			x::GTE_ReadOTZ(ctx + 0x38);
			if (U8(ctx, 0x14) & 0x40)
				t2_depth_cue(ctx, pkt + 4);
			{
				const uint32_t ot = t2_ot(ctx, a2, a3);
				const uint16_t add0 = U16(ctx, 0x18);
				const uint16_t add1 = U16(ctx, 0x1A);
				if ((uint16_t)(add0 | add1) != 0)
				{
					if (add0 != 0)
					{
						const uint32_t a = add0;
						const uint32_t c2 = U8(pkt, 0x1C) + a;
						const uint32_t c0 = U8(pkt, 0xC) + a;
						const uint32_t c1 = U8(pkt, 0x14) + a;
						uint8_t s = 0;
						if ((int32_t)(c2 | c1 | c0) > 0xFF)
							s = U8(ctx, 0x28);
						U8(pkt, 0xC) = (uint8_t)(c0 - s);
						U8(pkt, 0x14) = (uint8_t)(c1 - s);
						U8(pkt, 0x1C) = (uint8_t)(c2 - s);
					}
					const uint16_t add1b = U16(ctx, 0x1A);
					if (add1b != 0)
					{
						const uint32_t a = add1b;
						const uint32_t c2 = U8(pkt, 0x1D) + a;
						const uint32_t c0 = U8(pkt, 0xD) + a;
						const uint32_t c1 = U8(pkt, 0x15) + a;
						uint8_t s = 0;
						if ((int32_t)(c2 | c1 | c0) > 0xFF)
							s = U8(ctx, 0x2A);
						U8(pkt, 0xD) = (uint8_t)(c0 - s);
						U8(pkt, 0x15) = (uint8_t)(c1 - s);
						U8(pkt, 0x1D) = (uint8_t)(c2 - s);
					}
					const uint32_t poly = pkt;
					uint32_t prim = pkt + 0x20;
					pkt += 0x2C;
					U32(prim, 0) = 0x2000000;
					uint32_t tw = 0;
					if (ctx + 0x1C != 0)
						tw = t2_twin(ctx, 0x1C);
					U32(prim, 4) = tw;
					U32(prim, 8) = 0;
					x::SSIGPU_InsertPrimAutoDepth(ot, prim);
					x::SSIGPU_InsertPrimAutoDepth(ot, poly);
					prim = pkt;
					pkt += 0xC;
					U32(prim, 0) = 0x2000000;
					tw = 0;
					if (ctx + 0x24 != 0)
						tw = t2_twin(ctx, 0x24);
					U32(prim, 4) = tw;
					U32(prim, 8) = 0;
					x::SSIGPU_InsertPrimAutoDepth(ot, prim);
				}
				else
				{
					const uint32_t poly = pkt;
					const uint32_t mode = pkt + 0x20;
					pkt += 0x2C;
					const uint32_t tpage = x::sub_45C690(0, 1, 0, 0);
					x::sub_45BFC0(mode, 0, 0, tpage & 0xFFFF, 0);
					x::SSIGPU_InsertPrimAutoDepth(ot, mode);
					x::SSIGPU_InsertPrimAutoDepth(ot, poly);
				}
			}
		next:
			rec += 0x14;
			--count;
		} while (count != 0);
		U32(ctx, 0x2C) = rec;
		return pkt;
	}

	uint32_t __cdecl d_7082F0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		const uint32_t ctx = a1;
		uint32_t pkt = a4;
		uint32_t rec = U32(ctx, 0x2C);
		int32_t count = S32(rec, 0);
		rec += 4;
		const uint32_t vbase = U32(ctx, 4);
		U32(ctx, 0x2C) = rec;
		if (count <= 0)
		{
			U32(ctx, 0x2C) = rec;
			return pkt;
		}
		do
		{
			x::GTE_LoadV012(vbase + (uint32_t)U16(rec, 4) * 4u, vbase + (uint32_t)U16(rec, 6) * 4u,
				vbase + (uint32_t)U16(rec, 8) * 4u);
			x::GTE_RTPT();
			{
				const uint32_t flags = U32(ctx, 0x14);
				const uint32_t code = U32(rec, 0);
				U32(pkt, 0) = 0x9000000;
				U32(pkt, 4) = code;
				if (flags & 1)
					U32(pkt, 4) = code | 0x2000000;
				if (flags & 4)
					U32(pkt, 4) = U32(pkt, 4) & 0xFDFFFFFFu;
				U32(pkt, 0xC) = U32(rec, 0xC);
				const uint32_t uv1 = U32(rec, 0x10);
				const uint32_t uv23 = U32(rec, 0x14);
				U32(pkt, 0x1C) = uv23;
				U32(pkt, 0x14) = uv1;
				U32(pkt, 0x24) = uv23 >> 16;
			}
			x::GTE_ReadFLAG(ctx + 0x3C);
			if ((U32(ctx, 0x3C) & 0x60000) != 0)
				goto next;
			x::GTE_NCLIP();
			{
				uint32_t clip = 0;
				x::GTE_ReadMAC0(ctx + 0x30);
				if (S32(ctx, 0x30) < 0 && !(U8(ctx, 0x14) & 0x10))
					goto next;
				x::GTE_ReadSXY012_Split(pkt + 8, pkt + 0x10, pkt + 0x18);
				x::GTE_LoadV0(vbase + (uint32_t)U16(rec, 0xA) * 4u);
				x::GTE_RTPS();
				if (t2_outx(pkt + 8))
					clip = 1;
				if (t2_outx(pkt + 0x10))
					clip |= 2;
				if (t2_outx(pkt + 0x18))
					clip |= 4;
				if (t2_outy(pkt + 0xA))
					clip |= 0x10;
				if (t2_outy(pkt + 0x12))
					clip |= 0x20;
				if (t2_outy(pkt + 0x1A))
					clip |= 0x40;
				x::GTE_ReadSXY2(pkt + 0x20);
				x::GTE_AVSZ4();
				if (t2_outx(pkt + 0x20))
					clip |= 8;
				if (t2_outy(pkt + 0x22))
					clip |= 0x80;
				if ((clip & 0xF) == 0xF)
					goto next;
				if ((clip & 0xF0) == 0xF0)
					goto next;
			}
			x::GTE_ReadOTZ(ctx + 0x38);
			if (U8(ctx, 0x14) & 0x40)
				t2_depth_cue(ctx, pkt + 4);
			{
				const uint32_t ot = t2_ot(ctx, a2, a3);
				const uint16_t add0 = U16(ctx, 0x18);
				const uint16_t add1 = U16(ctx, 0x1A);
				if ((uint16_t)(add0 | add1) != 0)
				{
					if (add0 != 0)
					{
						const uint32_t a = add0;
						const uint32_t c0 = U8(pkt, 0xC) + a;
						const uint32_t c1 = U8(pkt, 0x14) + a;
						const uint32_t c2 = U8(pkt, 0x1C) + a;
						const uint32_t c3 = U8(pkt, 0x24) + a;
						uint8_t s = 0;
						if ((int32_t)(c3 | c2 | c1 | c0) > 0xFF)
							s = U8(ctx, 0x28);
						U8(pkt, 0xC) = (uint8_t)(c0 - s);
						U8(pkt, 0x14) = (uint8_t)(c1 - s);
						U8(pkt, 0x1C) = (uint8_t)(c2 - s);
						U8(pkt, 0x24) = (uint8_t)(c3 - s);
					}
					const uint16_t add1b = U16(ctx, 0x1A);
					if (add1b != 0)
					{
						const uint32_t a = add1b;
						const uint32_t c0 = U8(pkt, 0xD) + a;
						const uint32_t c1 = U8(pkt, 0x15) + a;
						const uint32_t c2 = U8(pkt, 0x1D) + a;
						const uint32_t c3 = U8(pkt, 0x25) + a;
						uint8_t s = 0;
						if ((int32_t)(c3 | c2 | c1 | c0) > 0xFF)
							s = U8(ctx, 0x2A);
						U8(pkt, 0xD) = (uint8_t)(c0 - s);
						U8(pkt, 0x15) = (uint8_t)(c1 - s);
						U8(pkt, 0x1D) = (uint8_t)(c2 - s);
						U8(pkt, 0x25) = (uint8_t)(c3 - s);
					}
					const uint32_t poly = pkt;
					uint32_t prim = pkt + 0x28;
					pkt += 0x34;
					U32(prim, 0) = 0x2000000;
					uint32_t tw = 0;
					if (ctx + 0x1C != 0)
						tw = t2_twin(ctx, 0x1C);
					U32(prim, 4) = tw;
					U32(prim, 8) = 0;
					x::SSIGPU_InsertPrimAutoDepth(ot, prim);
					x::SSIGPU_InsertPrimAutoDepth(ot, poly);
					prim = pkt;
					pkt += 0xC;
					U32(prim, 0) = 0x2000000;
					tw = 0;
					if (ctx + 0x24 != 0)
						tw = t2_twin(ctx, 0x24);
					U32(prim, 4) = tw;
					U32(prim, 8) = 0;
					x::SSIGPU_InsertPrimAutoDepth(ot, prim);
				}
				else
				{
					const uint32_t poly = pkt;
					const uint32_t mode = pkt + 0x28;
					pkt += 0x34;
					const uint32_t tpage = x::sub_45C690(0, 1, 0, 0);
					x::sub_45BFC0(mode, 0, 0, tpage & 0xFFFF, 0);
					x::SSIGPU_InsertPrimAutoDepth(ot, mode);
					x::SSIGPU_InsertPrimAutoDepth(ot, poly);
				}
			}
		next:
			rec += 0x18;
			--count;
		} while (count != 0);
		U32(ctx, 0x2C) = rec;
		return pkt;
	}

	uint32_t __cdecl d_708E10(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		static const int32_t U_OFF[3] = { 0x0C, 0x18, 0x24 };
		static const int32_t V_OFF[3] = { 0x0D, 0x19, 0x25 };
		const uint32_t ctx = a1;
		uint32_t pkt = a4;
		const uint32_t list = U32(ctx, 0x2C);
		uint32_t count = U32(list, 0);
		uint32_t rec = list + 4;
		const uint32_t vbase = U32(ctx, 4);
		U32(ctx, 0x2C) = rec;
		if ((int32_t)count <= 0)
		{
			U32(ctx, 0x2C) = rec;
			return pkt;
		}
		do
		{
			x::GTE_LoadV012(vbase + (uint32_t)U16(rec, 4) * 4, vbase + (uint32_t)U16(rec, 6) * 4, vbase + (uint32_t)U16(rec, 8) * 4);
			x::GTE_RTPT();
			const uint32_t flags = U32(ctx, 0x14);
			uint32_t code = U32(rec, 0);
			U32(pkt, 0) = 0x9000000;
			U32(pkt, 4) = code;
			if (flags & 2)
			{
				code |= 0x2000000;
				U32(pkt, 4) = code;
			}
			if (flags & 8)
				U32(pkt, 4) &= 0xFDFFFFFF;
			const uint32_t uv0 = U32(rec, 0x0C);
			const uint32_t w8 = U32(rec, 8);
			const uint32_t uv1 = U32(rec, 0x10);
			U32(pkt, 0x0C) = uv0;
			U32(pkt, 0x18) = uv1;
			U32(pkt, 0x24) = w8 >> 16;
			x::GTE_ReadFLAG(ctx + 0x3C);
			if (!(U32(ctx, 0x3C) & 0x60000))
			{
				x::GTE_NCLIP();
				uint32_t clip = 0;
				x::GTE_ReadMAC0(ctx + 0x30);
				if (S32(ctx, 0x30) >= 0 || (U8(ctx, 0x14) & 0x20))
				{
					x::GTE_ReadSXY012_Split(pkt + 8, pkt + 0x14, pkt + 0x20);
					x::GTE_AVSZ3();
					if (t3_out_x(S16(pkt, 8))) clip = 1;
					if (t3_out_x(S16(pkt, 0x14))) clip |= 2;
					if (t3_out_x(S16(pkt, 0x20))) clip |= 4;
					if (t3_out_y(S16(pkt, 0x0A))) clip |= 0x10;
					if (t3_out_y(S16(pkt, 0x16))) clip |= 0x20;
					if (t3_out_y(S16(pkt, 0x22))) clip |= 0x40;
					if ((uint8_t)(clip & 7) != 7 && (uint8_t)(clip & 0x70) != 0x70)
					{
						x::GTE_ReadOTZ(ctx + 0x38);
						if (U8(ctx, 0x14) & 0x80)
						{
							x::sub_45E120(rec + 0x14, rec + 0x18, pkt + 4);
							x::set_dword_1CA8A30(U32(ctx, 0x0C));
							x::sub_45F4C0();
							x::sub_45E370(pkt + 0x10, pkt + 0x1C, pkt + 4);
						}
						else
						{
							const uint32_t c1 = U32(rec, 0x14), c2 = U32(rec, 0x18);
							U32(pkt, 0x10) = c1;
							U32(pkt, 0x1C) = c2;
						}
						int32_t z = add32(S32(ctx, 0x38), S32(ctx, 0x10));
						S32(ctx, 0x38) = z;
						if (z < 0)
							S32(ctx, 0x38) = 0;
						z = S32(ctx, 0x38) >> (a3 & 31);
						const uint32_t ot = a2 + (uint32_t)z * 4;
						const uint16_t du = U16(ctx, 0x18);
						const uint16_t dv = U16(ctx, 0x1A);
						if ((uint16_t)(du | dv) != 0)
						{
							if (du != 0)
								t3_scroll(pkt, U_OFF, 3, du, U8(ctx, 0x28));
							const uint16_t dv2 = U16(ctx, 0x1A);
							if (dv2 != 0)
								t3_scroll(pkt, V_OFF, 3, dv2, U8(ctx, 0x2A));
							const uint32_t w1 = pkt + 0x28;
							U32(w1, 0) = 0x2000000;
							const uint32_t t1 = (ctx + 0x1C) != 0 ? t3_texwin(ctx + 0x1C) : 0;
							U32(w1, 4) = t1;
							U32(w1, 8) = 0;
							x::SSIGPU_InsertPrimAutoDepth(ot, w1);
							x::SSIGPU_InsertPrimAutoDepth(ot, pkt);
							const uint32_t w2 = pkt + 0x34;
							U32(w2, 0) = 0x2000000;
							const uint32_t t2 = (ctx + 0x24) != 0 ? t3_texwin(ctx + 0x24) : 0;
							U32(w2, 4) = t2;
							U32(w2, 8) = 0;
							x::SSIGPU_InsertPrimAutoDepth(ot, w2);
							pkt += 0x40;
						}
						else
						{
							const uint32_t dm = pkt + 0x28;
							const uint32_t tp = x::sub_45C690(0, 1, 0, 0);
							x::sub_45BFC0(dm, 0, 0, tp & 0xFFFF, 0);
							x::SSIGPU_InsertPrimAutoDepth(ot, dm);
							x::SSIGPU_InsertPrimAutoDepth(ot, pkt);
							pkt += 0x34;
						}
					}
				}
			}
			rec += 0x1C;
		} while (--count);
		U32(ctx, 0x2C) = rec;
		return pkt;
	}

	uint32_t __cdecl d_709370(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		static const int32_t U_OFF[4] = { 0x0C, 0x18, 0x24, 0x30 };
		static const int32_t V_OFF[4] = { 0x0D, 0x19, 0x25, 0x31 };
		const uint32_t ctx = a1;
		uint32_t pkt = a4;
		const uint32_t list = U32(ctx, 0x2C);
		uint32_t count = U32(list, 0);
		uint32_t rec = list + 4;
		const uint32_t vbase = U32(ctx, 4);
		U32(ctx, 0x2C) = rec;
		if ((int32_t)count <= 0)
		{
			U32(ctx, 0x2C) = rec;
			return pkt;
		}
		do
		{
			x::GTE_LoadV012(vbase + (uint32_t)U16(rec, 4) * 4, vbase + (uint32_t)U16(rec, 6) * 4, vbase + (uint32_t)U16(rec, 8) * 4);
			x::GTE_RTPT();
			const uint32_t flags = U32(ctx, 0x14);
			uint32_t code = U32(rec, 0);
			U32(pkt, 0) = 0xC000000;
			U32(pkt, 4) = code;
			if (flags & 2)
			{
				code |= 0x2000000;
				U32(pkt, 4) = code;
			}
			if (flags & 8)
				U32(pkt, 4) &= 0xFDFFFFFF;
			const uint32_t uv0 = U32(rec, 0x0C);
			const uint32_t uv1 = U32(rec, 0x10);
			const uint32_t uv23 = U32(rec, 0x14);
			U32(pkt, 0x0C) = uv0;
			U32(pkt, 0x24) = uv23;
			U32(pkt, 0x18) = uv1;
			U32(pkt, 0x30) = uv23 >> 16;
			x::GTE_ReadFLAG(ctx + 0x3C);
			if (!(U32(ctx, 0x3C) & 0x60000))
			{
				x::GTE_NCLIP();
				uint32_t clip = 0;
				x::GTE_ReadMAC0(ctx + 0x30);
				if (S32(ctx, 0x30) >= 0 || (U8(ctx, 0x14) & 0x20))
				{
					x::GTE_ReadSXY012_Split(pkt + 8, pkt + 0x14, pkt + 0x20);
					x::GTE_LoadV0(vbase + (uint32_t)U16(rec, 0x0A) * 4);
					x::GTE_RTPS();
					if (t3_out_x(S16(pkt, 8))) clip = 1;
					if (t3_out_x(S16(pkt, 0x14))) clip |= 2;
					if (t3_out_x(S16(pkt, 0x20))) clip |= 4;
					if (t3_out_y(S16(pkt, 0x0A))) clip |= 0x10;
					if (t3_out_y(S16(pkt, 0x16))) clip |= 0x20;
					if (t3_out_y(S16(pkt, 0x22))) clip |= 0x40;
					x::GTE_ReadSXY2(pkt + 0x2C);
					x::GTE_AVSZ4();
					if (t3_out_x(S16(pkt, 0x2C))) clip |= 8;
					if (t3_out_y(S16(pkt, 0x2E))) clip |= 0x80;
					if ((uint8_t)(clip & 0xF) != 0xF && (uint8_t)(clip & 0xF0) != 0xF0)
					{
						x::GTE_ReadOTZ(ctx + 0x38);
						if (U8(ctx, 0x14) & 0x80)
						{
							x::sub_45E120(rec + 0x18, rec + 0x1C, rec + 0x20);
							x::set_dword_1CA8A30(U32(ctx, 0x0C));
							x::sub_45F4C0();
							x::sub_45E370(pkt + 0x10, pkt + 0x1C, pkt + 0x28);
							x::set_unk_1CA8A28(pkt + 4);
							x::sub_45F270();
							x::set_param_with_dword_1CA8A68(pkt + 4);
						}
						else
						{
							const uint32_t c1 = U32(rec, 0x18), c2 = U32(rec, 0x1C), c3 = U32(rec, 0x20);
							U32(pkt, 0x10) = c1;
							U32(pkt, 0x1C) = c2;
							U32(pkt, 0x28) = c3;
						}
						int32_t z = add32(S32(ctx, 0x38), S32(ctx, 0x10));
						S32(ctx, 0x38) = z;
						if (z < 0)
							S32(ctx, 0x38) = 0;
						z = S32(ctx, 0x38) >> (a3 & 31);
						const uint32_t ot = a2 + (uint32_t)z * 4;
						const uint16_t du = U16(ctx, 0x18);
						const uint16_t dv = U16(ctx, 0x1A);
						if ((uint16_t)(du | dv) != 0)
						{
							if (du != 0)
								t3_scroll(pkt, U_OFF, 4, du, U8(ctx, 0x28));
							const uint16_t dv2 = U16(ctx, 0x1A);
							if (dv2 != 0)
								t3_scroll(pkt, V_OFF, 4, dv2, U8(ctx, 0x2A));
							const uint32_t w1 = pkt + 0x34;
							U32(w1, 0) = 0x2000000;
							const uint32_t t1 = (ctx + 0x1C) != 0 ? t3_texwin(ctx + 0x1C) : 0;
							U32(w1, 4) = t1;
							U32(w1, 8) = 0;
							x::SSIGPU_InsertPrimAutoDepth(ot, w1);
							x::SSIGPU_InsertPrimAutoDepth(ot, pkt);
							const uint32_t w2 = pkt + 0x40;
							U32(w2, 0) = 0x2000000;
							const uint32_t t2 = (ctx + 0x24) != 0 ? t3_texwin(ctx + 0x24) : 0;
							U32(w2, 4) = t2;
							U32(w2, 8) = 0;
							x::SSIGPU_InsertPrimAutoDepth(ot, w2);
							pkt += 0x4C;
						}
						else
						{
							const uint32_t dm = pkt + 0x34;
							const uint32_t tp = x::sub_45C690(0, 1, 0, 0);
							x::sub_45BFC0(dm, 0, 0, tp & 0xFFFF, 0);
							x::SSIGPU_InsertPrimAutoDepth(ot, dm);
							x::SSIGPU_InsertPrimAutoDepth(ot, pkt);
							pkt += 0x40;
						}
					}
				}
			}
			rec += 0x24;
		} while (--count);
		U32(ctx, 0x2C) = rec;
		return pkt;
	}

	uint32_t __cdecl d_709A40(void)
	{
		uint32_t act = U32(MEM<uint32_t>(game_addr(game::MAG_345_Lv5Death_ActorState)), 0x2C);
		if (act == 0)
			return 0;
		do
		{
			if (U16(act, 8) == 1)
			{
				uint32_t st = MEM<uint32_t>(game_addr(game::MAG_345_Lv5Death_ActorState));
				int32_t b = S8(act, 0x1D6);
				uint32_t bone = U32(U32(st, 0x224) + (uint32_t)b * 4, 0);
				uint8_t kind = U8(bone, 0x16);
				if ((kind == 0 || kind == 1 || kind == 2) && U32(act, 0x170) != 0)
				{
					int8_t n = S8(act, 0x1D8);
					if (n == 1)
					{
						d_709B00(act);
					}
					else if (n > 0)
					{
						int32_t i = 0;
						uint32_t sub = act + 0x19E;
						do
						{
							S32(act, 0xC0) = S16(sub, -2);
							S32(act, 0xC4) = S16(sub, 0);
							S32(act, 0xC8) = S16(sub, 2);
							d_709B00(act);
							i++;
							sub += 8;
						} while (i < S8(act, 0x1D8));
					}
				}
			}
			act = U32(act, 4);
		} while (act != 0);
		return 0;
	}

	uint32_t __cdecl d_709B80(void)
	{
		uint32_t act = U32(U32(game_addr(game::MAG_345_Lv5Death_ActorState), 0), 0x2C);
		while (act != 0)
		{
			int32_t type = S16(act, 8);
			if (type == 0)
				d_70AE50(act);
			else if (type == 1)
				d_709BC0(act);
			act = U32(act, 4);
		}
		return 0;
	}

	uint32_t __cdecl d_709BC0(uint32_t a1)
	{
		const uint32_t act = a1;
		uint32_t st = MEM<uint32_t>(game_addr(game::MAG_345_Lv5Death_ActorState));
		int32_t b = S8(act, 0x1D6);
		const uint32_t bone = U32(U32(st, 0x224) + (uint32_t)b * 4, 0);
		int32_t phase = S16(act, 0x1CC);
		if (phase == 0)
		{
			a_741050(act, bone);
			if (d_70AD00(act, bone) == 0)
			{
				a_740910(act, bone);
				a_741120(act, bone);
				U16(act, 0x1CA) = (uint16_t)(U16(act, 0x1CA) + 1);
				return 0;
			}
			U16(act, 0x1CC) = (uint16_t)(U16(act, 0x1CC) + 1);
			U32(act, 0x170) = 0;
			return 0;
		}
		if (phase == 1)
		{
			a_742CD0(act);
			st = MEM<uint32_t>(game_addr(game::MAG_345_Lv5Death_ActorState));
			U8(act, 0x1D7) = 0;
			U16(st, 0x16) = (uint16_t)(U16(st, 0x16) - 1);
		}
		return 0;
	}

	uint32_t __cdecl d_70ABF0(uint32_t a1, uint32_t a2)
	{
		const uint32_t data = a2;
		int32_t frame = S16(a1, 0x1CA);
		uint32_t i_dst = U8(U32(data, 0x12C), frame);
		uint32_t st = U32(game_addr(game::MAG_345_Lv5Death_ActorState), 0);
		uint32_t dst_tab = U32(st, 0x22C);
		uint32_t src_tab = U32(st, 0x230);
		uint32_t dst = U32(dst_tab, i_dst * 4);
		uint32_t i_src1 = U8(U32(data, 0x130), frame);
		uint32_t src1 = U32(src_tab, i_src1 * 4);
		uint32_t i_src2 = U8(U32(data, 0x134), frame);
		uint32_t src2 = U32(src_tab, i_src2 * 4);
		if (src1 == 0 || src2 == 0)
			return 0;
		uint32_t w_blk = x::Field_Alloc(8);
		int32_t frame2 = S16(a1, 0x1CA);
		uint32_t n = U32(dst, 4);
		int32_t w = S16(U32(data, 0x138), frame2 * 2);
		U32(w_blk, 4) = (uint32_t)w;
		U32(w_blk, 0) = (uint32_t)sub32(0x1000, w);
		if ((int32_t)n > 0)
		{
			uint32_t out = dst + 8;
			uint32_t in2 = src2 + 8;
			uint32_t delta = src1 - src2;
			do
			{
				x::set_dword_1CA8A30(U32(w_blk, 0));
				dx::sub_45E0B0(in2 + delta);
				dx::sub_45E9D0();
				x::set_dword_1CA8A30(U32(w_blk, 4));
				dx::sub_45E0B0(in2);
				dx::sub_45EBF0();
				x::GTE_StoreIR123(out);
				out += 8;
				in2 += 8;
			} while (--n != 0);
		}
		x::Field_Free(8);
		return 0;
	}

	uint32_t __cdecl d_70AD00(uint32_t a1, uint32_t a2)
	{
		int32_t kind = (int32_t)S8(a2, 0x1E);
		if (kind == 2)
		{
			U16(a1, 0x1C8) = (uint16_t)(U16(a1, 0x1C8) - 1);
			if (S16(a1, 0x1C8) < 0)
			{
				d_70AD80(a1, a2);
				return 1;
			}
		}
		else if (kind == 1)
		{
			U16(a1, 0x1C8) = (uint16_t)(U16(a1, 0x1C8) - 1);
			if (S16(a1, 0x1C8) >= 0)
				return 0;
			d_70AD80(a1, a2);
			return 1;
		}
		else if (kind != 0)
		{
			return 0;
		}
		if (U8(a1, 0x1D2) == 1)
		{
			d_70AD80(a1, a2);
			return 1;
		}
		return 0;
	}

	uint32_t __cdecl d_70AD80(uint32_t a1, uint32_t a2)
	{
		if (U8(a2, 0x30) == 1)
		{
			uint32_t desc_index = (a2 & 0xFFFF0000u) | (uint16_t)(int16_t)S8(a2, 0x31);
			uint32_t anchor = (uint16_t)(int16_t)S8(a1, 0x1D9);
			return d_70ADB0(a1, desc_index, anchor);
		}
		return a1;
	}

	uint32_t __cdecl d_70AE50(uint32_t a1)
	{
		if (U16(a1, 0x62) == 0)
		{
			a_742D00(a1);
			U16(a1, 0x62) = (uint16_t)(U16(a1, 0x62) + 1);
		}
		int32_t mode = S8(a1, 0x68);
		if (mode == 3)
			callp(game_addr(game::sub_70B800), a1);
		else if (mode == 4)
			callp(game_addr(game::sub_70B950), a1);
		d_70AED0(a1);
		U16(a1, 0x60) = (uint16_t)(U16(a1, 0x60) + 1);
		a_741B60(a1);
		return 0;
	}

	uint32_t __cdecl d_70AED0(uint32_t a1)
	{
		uint32_t sys = U32(game_addr(game::MAG_345_Lv5Death_ActorState), 0);
		int32_t k = S8(a1, 0x6A);
		uint32_t def = U32(U32(sys, 0x224), k * 4);
		int16_t frame = S16(a1, 0x60);
		if (frame > 0x27)
			return 0;
		uint32_t total = U8(U32(def, 0xC8), (int32_t)frame);
		if (total == 0)
			return 0;
		int32_t batch = S8(def, 0x36);
		int32_t full = (int32_t)total / batch;
		int32_t rest = (int32_t)total % batch;
		if (full > 0)
		{
			do
			{
				int32_t b = S8(def, 0x36);
				d_70AF50(a1, def, (uint32_t)b);
			} while (--full != 0);
		}
		if (rest > 0)
			d_70AF50(a1, def, (uint32_t)rest);
		return 0;
	}

	uint32_t __cdecl d_70AF50(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		uint32_t sp = U32(game_addr(game::MAG_345_Lv5Death_ScratchStackPointer), 0) - 0x50;
		int16_t kind = (int16_t)S8(a1, 0x6A);
		U32(game_addr(game::MAG_345_Lv5Death_ScratchStackPointer), 0) = sp;
		uint32_t node = d_70B730(a1, (uint16_t)kind);
		uint32_t desc = a2;
		uint8_t a3_lo = (uint8_t)a3;
		uint16_t w60 = U16(a1, 0x60);
		uint8_t mode = U8(desc, 0x16);
		U8(node, 0x1D8) = a3_lo;
		U16(sp, 0x4E) = w60;
		if (mode == 0 || mode == 1 || mode == 2)
		{
			uint32_t ctx = U32(game_addr(game::MAG_345_Lv5Death_ActorState), 0);
			int32_t k = S8(a1, 0x6A);
			uint32_t v = U32(U32(ctx, 0x228), shl32(k, 2));
			U32(node, 0x170) = v;
			U8(node, 0x1D1) = (uint8_t)a_7424B0(v);
			if (U8(desc, 0x16) == 2)
			{
				uint8_t b17 = U8(desc, 0x17);
				uint8_t b18 = U8(desc, 0x18);
				int32_t s1a = S8(desc, 0x1A);
				U8(node, 0x1D3) = b17;
				int32_t s19 = S8(desc, 0x19);
				U8(node, 0x1D4) = b18;
				U8(node, 0x1D5) = (uint8_t)a_742290((uint32_t)s19, (uint32_t)s1a);
			}
		}
		U16(node, 0x1DA) = U16(desc, 0x64);
		if ((int32_t)a3 > 0)
		{
			uint32_t src = a1 + 0x4C;
			uint32_t emat = a1 + 0x2C;
			uint32_t out_speed = node + 0x184;
			uint32_t mat = node + 0x0C;
			uint32_t pos = node + 0x12C;
			for (int32_t n = (int32_t)a3; n != 0; n--)
			{
				e6_copy16(pos - 0x50, src);
				if (U8(desc, 0x22) == 0)
					e6_copy16(pos, src);
				uint8_t spread = U8(desc, 0x34);
				if (spread == 1)
				{
					U16(sp, 0x28) = (uint16_t)(x::CrtRand() & 0xFFF);
					U16(sp, 0x2A) = (uint16_t)(x::CrtRand() & 0xFFF);
					U16(sp, 0x2C) = (uint16_t)(x::CrtRand() & 0xFFF);
					e6_rotate_zxy(sp, sp + 0x28);
					int32_t i2 = shl32(S16(sp, 0x4E), 1);
					if (U8(desc, 0x33) != 0)
					{
						U16(sp, 0x48) = U16(U32(desc, 0xCC), i2);
						U16(sp, 0x4A) = U16(U32(desc, 0xD0), i2);
						U16(sp, 0x4C) = U16(U32(desc, 0xD4), i2);
					}
					else
					{
						U16(sp, 0x48) = U16(U32(desc, 0xCC), i2);
						U16(sp, 0x4A) = U16(U32(desc, 0xD0), i2);
						U16(sp, 0x4C) = U16(U32(desc, 0xD4), i2);
						e6_rand_mod(sp, 0x48);
						e6_rand_mod(sp, 0x4A);
						e6_rand_mod(sp, 0x4C);
					}
					e6_rotate_down_vector(sp);
					int32_t ex = mul32(S16(sp, 0x48), S16(sp, 0x20));
					int32_t ey = mul32(S16(sp, 0x4A), S16(sp, 0x22));
					int32_t ez = mul32(S16(sp, 0x4C), S16(sp, 0x24));
					int32_t old = S32(pos, 0);
					ex = shl32(ex, 4);
					S32(sp, 0x38) = ex;
					S32(pos, 0) = add32(old, ex);
					old = S32(pos, 4);
					ey = shl32(ey, 4);
					S32(sp, 0x3C) = ey;
					S32(pos, 4) = add32(old, ey);
					old = S32(pos, 8);
					ez = shl32(ez, 4);
					S32(sp, 0x40) = ez;
					S32(pos, 8) = add32(old, ez);
				}
				else if (spread == 2)
				{
					U16(sp, 0x28) = (uint16_t)(x::CrtRand() & 0xFFF);
					U16(sp, 0x2A) = 0;
					U16(sp, 0x2C) = 0x400;
					e6_rotate_zxy(sp, sp + 0x28);
					int32_t i2 = shl32(S16(sp, 0x4E), 1);
					if (S8(desc, 0x33) != 1)
					{
						U16(sp, 0x48) = U16(U32(desc, 0xCC), i2);
						U16(sp, 0x4A) = U16(U32(desc, 0xD0), i2);
						U16(sp, 0x4C) = U16(U32(desc, 0xD4), i2);
						e6_rand_mod(sp, 0x48);
						e6_rand_mod(sp, 0x4A);
						e6_rand_mod(sp, 0x4C);
					}
					else
					{
						U16(sp, 0x48) = U16(U32(desc, 0xCC), i2);
						U16(sp, 0x4A) = U16(U32(desc, 0xD0), i2);
						e6_rand_mod(sp, 0x4A);
						int32_t e = S16(sp, 0x4E);
						U16(sp, 0x4C) = U16(U32(desc, 0xD4), shl32(e, 1));
					}
					if ((x::CrtRand() & 1) != 0)
						U16(sp, 0x4A) = (uint16_t)(0 - U16(sp, 0x4A));
					e6_rotate_down_vector(sp);
					int32_t ex = mul32(S16(sp, 0x48), S16(sp, 0x20));
					int32_t ey = shl32(S16(sp, 0x4A), 16);
					int32_t oldx = S32(pos, 0);
					S32(sp, 0x3C) = ey;
					int32_t ez = mul32(S16(sp, 0x4C), S16(sp, 0x24));
					ex = shl32(ex, 4);
					S32(sp, 0x38) = ex;
					S32(pos, 0) = add32(oldx, ex);
					int32_t ey2 = S32(sp, 0x3C);
					S32(pos, 4) = add32(S32(pos, 4), ey2);
					int32_t oldz = S32(pos, 8);
					ez = shl32(ez, 4);
					S32(sp, 0x40) = ez;
					S32(pos, 8) = add32(oldz, ez);
				}
				int32_t rmode = S8(desc, 0x21);
				if (rmode == 0)
					a_742350(sp + 0x30, desc + 0x3C, desc + 0x44);
				else if (rmode == 1)
				{
					uint32_t c0 = U32(sp, 0x28);
					uint32_t c1 = U32(sp, 0x2C);
					U32(sp, 0x30) = c0;
					U32(sp, 0x34) = c1;
				}
				e6_rotate_zxy(mat, sp + 0x30);
				x::GTE_SetRotMatrix(emat);
				x::GTE_LoadIRFromMatrixColumn(mat);
				x::GTE_MVMVA_RotIR();
				x::GTE_StoreIRToMatrixColumn(mat);
				x::GTE_LoadIRFromMatrixColumn(mat + 2);
				x::GTE_MVMVA_RotIR();
				x::GTE_StoreIRToMatrixColumn(mat + 2);
				x::GTE_LoadIRFromMatrixColumn(mat + 4);
				x::GTE_MVMVA_RotIR();
				x::GTE_StoreIRToMatrixColumn(mat + 4);
				if (U8(desc, 0x27) == 0)
				{
					uint32_t r1 = a_7422C0(U32(desc, 0x4C), U32(desc, 0x50));
					U32(out_speed, -0x10) = r1;
					uint32_t r2 = a_7422C0(U32(desc, 0x54), U32(desc, 0x58));
					U32(out_speed, 0) = r2;
				}
				out_speed += 4;
				pos += 0x10;
				mat += 0x20;
			}
		}
		a_742350(node + 0x194, desc + 0x148, desc + 0x150);
		uint8_t m1e = U8(desc, 0x1E);
		if (m1e == 1 || m1e == 2)
		{
			int32_t s20 = S8(desc, 0x20);
			int32_t s1f = S8(desc, 0x1F);
			U16(node, 0x1C8) = (uint16_t)a_742290((uint32_t)s1f, (uint32_t)s20);
		}
		if (U8(desc, 0x27) == 0)
		{
			U32(node, 0x1DC) = a_7422C0(U32(desc, 0x5C), U32(desc, 0x60));
			if ((int32_t)a3 > 0)
			{
				uint32_t q = node + 0x220;
				for (int32_t n = (int32_t)a3; n != 0; n--)
				{
					a_742300(q - 0x40, desc + 0x68, desc + 0x78);
					a_742300(q, desc + 0x88, desc + 0x98);
					a_742300(q + 0x40, desc + 0xA8, desc + 0xB8);
					q += 0x10;
				}
			}
		}
		d_70C440(node, desc);
		U32(game_addr(game::MAG_345_Lv5Death_ScratchStackPointer), 0) = U32(game_addr(game::MAG_345_Lv5Death_ScratchStackPointer), 0) + 0x50;
		return 0;
	}

	uint32_t __cdecl d_70B950(uint32_t a1)
	{
		const uint32_t act = a1;
		const uint32_t st = U32(game_addr(game::MAG_345_Lv5Death_ActorState), 0);
		int32_t aidx = S8(act, 0x6A);
		int32_t key = S16(act, 0x60);
		const uint32_t data = U32(U32(st, 0x224), aidx * 4);
		uint8_t sel = U8(U32(data, 0x108), key);
		uint8_t kind = U8(U32(data, 0x100), key);
		int16_t ax = 0, ay = 0, az = 0;
		uint32_t base = 0;
		bool have = false;
		if (kind == 0)
		{
			if (sel <= 3)
			{
				static const uint32_t offs0[4] = { 0x94, 0xD4, 0x114, 0x154 };
				base = st + (uint32_t)((int32_t)S8(act, 0x6B) << 4) + offs0[sel];
				have = true;
			}
		}
		else if (kind == 1)
		{
			if (sel <= 3)
			{
				static const uint32_t offs1[4] = { 0x1D4, 0x1E4, 0x1F4, 0x204 };
				base = st + offs1[sel];
				have = true;
			}
		}
		if (have)
		{
			int32_t vx = b2_fx(U32(base, 0));
			int32_t vy = b2_fx(U32(base, 4));
			ay = (int16_t)vy;
			int32_t vz = b2_fx(U32(base, 8));
			az = (int16_t)vz;
			ax = (int16_t)vx;
		}
		int16_t v[4];
		int16_t ir[8] = {};
		v[0] = (int16_t)U16(U32(data, 0x110), key * 2);
		v[1] = (int16_t)U16(U32(data, 0x114), key * 2);
		v[2] = (int16_t)U16(U32(data, 0x118), key * 2);
		v[3] = 0;
		x::GTE_SetRotMatrix(act + 0x2C);
		x::GTE_LoadV0(P(v));
		x::GTE_MVMVA_RotV0();
		x::GTE_StoreIR123(P(ir));
		az = (int16_t)(az + ir[2]);
		ay = (int16_t)(ay + ir[1]);
		const int32_t key2 = S16(act, 0x60);
		uint8_t kind2 = U8(U32(data, 0x104), key2);
		uint8_t sel2 = U8(U32(data, 0x10C), key2);
		ax = (int16_t)(ax + ir[0]);
		int16_t bx, by, bz;
		uint32_t base2 = 0;
		bool have2 = false;
		if (kind2 == 0)
		{
			if (sel2 <= 3)
			{
				static const uint32_t offs0[4] = { 0x94, 0xD4, 0x114, 0x154 };
				uint32_t g = U32(game_addr(game::MAG_345_Lv5Death_ActorState), 0);
				base2 = (uint32_t)((int32_t)S8(act, 0x6B) << 4) + g + offs0[sel2];
				have2 = true;
			}
		}
		else if (kind2 == 1)
		{
			if (sel2 <= 3)
			{
				static const uint32_t offs1[4] = { 0x1D4, 0x1E4, 0x1F4, 0x204 };
				uint32_t g = U32(game_addr(game::MAG_345_Lv5Death_ActorState), 0);
				base2 = g + offs1[sel2];
				have2 = true;
			}
		}
		if (have2)
		{
			int32_t vx = b2_fx(U32(base2, 0));
			int32_t vy = b2_fx(U32(base2, 4));
			int32_t vz = b2_fx(U32(base2, 8));
			bx = (int16_t)vx;
			by = (int16_t)vy;
			bz = (int16_t)vz;
		}
		else
		{
			bz = ir[2];
			by = ir[1];
			bx = ir[0];
		}
		v[0] = (int16_t)U16(U32(data, 0x11C), key2 * 2);
		v[1] = (int16_t)U16(U32(data, 0x120), key2 * 2);
		v[2] = (int16_t)U16(U32(data, 0x124), key2 * 2);
		x::GTE_SetRotMatrix(act + 0x2C);
		x::GTE_LoadV0(P(v));
		x::GTE_MVMVA_RotV0();
		x::GTE_StoreIR123(P(ir));
		bx = (int16_t)(bx + ir[0]);
		by = (int16_t)(by + ir[1]);
		const int32_t key3 = S16(act, 0x60);
		const uint32_t ttab = U32(data, 0x128);
		bz = (int16_t)(bz + ir[2]);
		const int32_t t = S16(ttab, key3 * 2);
		int32_t a = ax;
		S32(act, 0x4C) = shl32(add32(mul32(sub32(bx, a), t), shl32(a, 12)), 4);
		a = ay;
		S32(act, 0x50) = shl32(add32(mul32(sub32(by, a), t), shl32(a, 12)), 4);
		a = az;
		S32(act, 0x54) = shl32(add32(mul32(sub32(bz, a), t), shl32(a, 12)), 4);
		return 0;
	}

	uint32_t __cdecl d_70C440(uint32_t a1, uint32_t a2)
	{
		const uint32_t obj = a1;
		const uint32_t desc = a2;
		if (U8(desc, 0x28) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2C);
			d_70ADB0(obj, id, variant);
		}
		if (U8(desc, 0x29) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2D);
			d_70ADB0(obj, id, variant);
		}
		if (U8(desc, 0x2A) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2E);
			d_70ADB0(obj, id, variant);
		}
		if (U8(desc, 0x2B) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2F);
			d_70ADB0(obj, id, variant);
		}
		return 0;
	}

	uint32_t __cdecl d_70C4D0(void)
	{
		uint32_t dir = MEM<uint32_t>(game_addr(game::MAG_345_Lv5Death_ActorState));
		for (int32_t slot = 0; slot < 0x10; slot++)
		{
			uint32_t table = U32(dir, 0x224);
			uint32_t entry = U32(table, slot * 4);
			if (entry == 0)
				continue;
			uint32_t bit = (uint32_t)shl32(1, slot);
			uint32_t mask = U16(dir, 0x22);
			if (bit & mask)
				continue;
			uint8_t kind = U8(entry, 0x11);
			if (kind != 0 && kind != 1 && kind != 4)
				continue;
			int16_t group = (int16_t)S8(entry, 0x13);
			if (group != S16(dir, 0x18))
				continue;
			if (U8(entry, 0x37) == 1)
			{
				int32_t n = 0;
				if (S16(dir, 0x1C) <= 0)
					continue;
				do
				{
					d_70ADB0(0, (uint32_t)slot, (uint32_t)n);
					dir = MEM<uint32_t>(game_addr(game::MAG_345_Lv5Death_ActorState));
					n++;
				} while (n < (int32_t)S16(dir, 0x1C));
			}
			else
			{
				d_70ADB0(0, (uint32_t)slot, 0);
				dir = MEM<uint32_t>(game_addr(game::MAG_345_Lv5Death_ActorState));
			}
		}
		return 0;
	}

	uint32_t __cdecl d_70C560(uint32_t a1)
	{
		if (d_7075D0(a1) != 0)
		{
			uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) | 1);
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl d_70C590(uint32_t a1, uint32_t a2)
	{
		const uint32_t node = a1;
		a_733950(node + 0x30, U16(node, 0x13A), a2);
		return 0;
	}

	uint32_t __cdecl d_70C6B0(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t amb = MEM<uint32_t>(game_addr(game::CURRENT_BS_SHADOW_PALETTE));
		const int32_t level = S16(node, 0x13C);
		const uint8_t c0 = (uint8_t)(amb & 0xFF);
		const uint8_t c1 = (uint8_t)((amb >> 8) & 0xFF);
		const int32_t d0 = mul32((int32_t)c0, level) / 4096;
		U8(node, 0x5C) = (uint8_t)(c0 - (uint8_t)d0);
		const int32_t d1 = mul32((int32_t)c1, level) / 4096;
		U8(node, 0x5D) = (uint8_t)(c1 - (uint8_t)d1);
		const uint8_t c2 = MEM<uint8_t>(game_addr(game::unk_B8B9AA));
		const int32_t d2 = mul32((int32_t)c2, level) / 4096;
		U8(node, 0x5E) = (uint8_t)(c2 - (uint8_t)d2);
		return 0;
	}

	uint32_t __cdecl d_70D160(uint32_t a1)
	{
		U16(a1, 0x1C) = (uint16_t)(U16(a1, 0x1C) + 0xFF00);
		if (S16(a1, 0x1C) <= 0)
		{
			const uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) |= 1;
			U16(a1, 0x1C) = 0;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		const uint16_t level = U16(a1, 0x1C);
		uint32_t p = game_addr(game::unk_1D98992);
		for (int i = 4; i != 0; i--, p += 0x2C)
			MEM<uint16_t>(p) = level;
		return 0;
	}

	uint32_t __cdecl d_70D210(uint32_t a1)
	{
		const uint32_t d = MEM<uint32_t>(game_addr(game::unk_1524658));
		if (U16(d, 0x48) != 1)
			return 0;
		const uint32_t node = a1;
		const int32_t act = S8(node, 0x2A);
		const uint32_t ctx = U32(node, 0xC);
		const int32_t sub = S8(node, 0x2B);
		const uint32_t results = U32(ctx, 4);
		const uint32_t arr = U32(results + (uint32_t)(act * 20), 8);
		x::ApplyActionResultToTarget(arr + (uint32_t)(sub * 24));
		t4_next_state(node);
		return 0;
	}

	uint32_t __cdecl d_706B10(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t data = U32(node, 0x30);
		const uint16_t w29a = U16(node, 0x29A);
		const uint16_t w29c = U16(node, 0x29C);
		uint32_t st = node + 0x34;
		const uint16_t caster = (uint16_t)U8(node, 0x2C);
		U16(st, 0x22) = w29a;
		U16(st, 0x24) = w29c;
		U32(st, 0x224) = data + 4;
		U32(st, 0x228) = data + 0x44;
		U32(st, 0x22C) = data + 0x84;
		const uint32_t relocated = U32(data, 0);
		MEM<uint32_t>(game_addr(ACTOR_STATE)) = st;
		U32(st, 0x230) = data + 0xC4;
		if (relocated == 0)
		{
			a_740210(data);
			st = MEM<uint32_t>(game_addr(ACTOR_STATE));
			U32(data, 0) = 1;
		}
		const int32_t action = S8(node, 0x2A);
		U16(st, 0x1E) = caster;
		U16(st, 0x1C) = 1;
		const uint32_t rec = U32(U32(U32(node, 0x0C), 4) + (uint32_t)mul32(action, 5) * 4, 8);
		const int32_t target = S8(node, 0x2B);
		U32(st, 4) = U8(rec, (int32_t)mul32(target, 3) * 8);
		U16(st, 0x1A) = U16(node, 0x29E);
		a_73FC20();
		st = MEM<uint32_t>(game_addr(ACTOR_STATE));
		if (U16(st, 0x24) == 4)
			callp(F(F_73FB40), node);
		a_73FFB0();
		st = MEM<uint32_t>(game_addr(ACTOR_STATE));
		if (U16(st, 0x24) == 1)
		{
			callp(F(F_73FDC0), node);
			st = MEM<uint32_t>(game_addr(ACTOR_STATE));
		}
		if (U16(st, 0x24) == 3)
		{
			a_73FE90(node);
			st = MEM<uint32_t>(game_addr(ACTOR_STATE));
		}
		const int32_t s1 = S16(st, 0x1E);
		const uint32_t e1 = game_addr(game::BattleEntitySlotData) + (uint32_t)mul32(s1, 0x9C);
		const uint32_t d0c = U32(e1, 0x1C);
		const uint32_t d10 = U32(e1, 0x20);
		const uint32_t s2 = U32(st, 4);
		const uint32_t e2 = game_addr(game::BattleEntitySlotData) + s2 * 0x9C;
		const uint32_t d14 = U32(e2, 0x1C);
		const int32_t dx = (int16_t)(uint16_t)((uint16_t)d0c - (uint16_t)d14);
		const int32_t dy = (int16_t)(uint16_t)((uint16_t)(d0c >> 16) - (uint16_t)(d14 >> 16));
		const int32_t dz = (int16_t)(uint16_t)((uint16_t)d10 - (uint16_t)U32(e2, 0x20));
		const int32_t sq = add32(add32(mul32(dx, dx), mul32(dy, dy)), mul32(dz, dz));
		const uint32_t dist = x::Sqrt((uint32_t)sq);
		st = MEM<uint32_t>(game_addr(ACTOR_STATE));
		U32(st, 0) = dist;
		return 0;
	}

	uint32_t __cdecl d_70D2A0(uint32_t a1)
	{
		if (U8(a1, 0x63) != 0)
			return 0;
		const int8_t t = S8(a1, 0x2B);
		if ((int32_t)t < (int32_t)S16(a1, 0x5A) - 1)
		{
			U8(a1, 0x2B) = (uint8_t)(t + 1);
			U8(a1, 0x2E) = (uint8_t)(U8(a1, 0x2E) + 1);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) - 1);
			return 0;
		}
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	struct ModPort { game::Id addr; void *port; const char *name; };
	static const ModPort ENGINE_PORTS[] = {
		{ game::MAG_345_sub_705D80, (void *)a_73A0D0, "345 MAG_345_sub_705D80" },
		{ game::MAG_345_sub_705D90, (void *)a_73A0D0, "345 MAG_345_sub_705D90" },
		{ game::MAG_345_sub_705E20, (void *)a_73A170, "345 MAG_345_sub_705E20" },
		{ game::MAG_345_Lv5Death_Emitter, (void *)a_73A1A0, "345 MAG_345_sub_705E50" },
		{ game::sub_706270, (void *)a_746C10, "345 sub_706270" },
		{ game::sub_706A30, (void *)a_73C100, "345 sub_706A30" },
		{ game::MAG_345_Lv5Death_ActorSystem, (void *)a_73A380, "345 sub_706A80" },
		{ game::sub_706D80, (void *)a_73FC20, "345 sub_706D80" },
		{ game::sub_706FF0, (void *)a_73FE90, "345 sub_706FF0" },
		{ game::sub_707110, (void *)a_73FFB0, "345 sub_707110" },
		{ game::sub_707370, (void *)a_740210, "345 sub_707370" },
		{ game::sub_7088E0, (void *)a_73D770, "345 sub_7088E0" },
		{ game::sub_708B30, (void *)a_73D9C0, "345 sub_708B30" },
		{ game::sub_709C50, (void *)a_740910, "345 sub_709C50" },
		{ game::sub_70A2B0, (void *)a_740F70, "345 sub_70A2B0" },
		{ game::sub_70A300, (void *)a_740FC0, "345 sub_70A300" },
		{ game::sub_70A390, (void *)a_741050, "345 sub_70A390" },
		{ game::sub_70A460, (void *)a_741120, "345 sub_70A460" },
		{ game::sub_70AEA0, (void *)a_741B60, "345 sub_70AEA0" },
		{ game::MAG_345_RandRange, (void *)a_742290, "345 au_re__rand_2" },
		{ game::sub_70B600, (void *)a_7422C0, "345 sub_70B600" },
		{ game::sub_70B640, (void *)a_742300, "345 sub_70B640" },
		{ game::sub_70B690, (void *)a_742350, "345 sub_70B690" },
		{ game::MAG_345_RandAngleJitter, (void *)a_7423A0, "345 au_re__rand_2_0" },
		{ game::sub_70B7F0, (void *)a_7424B0, "345 sub_70B7F0" },
		{ game::sub_70B800, (void *)a_7424C0, "345 sub_70B800" },
		{ game::sub_70BFE0, (void *)a_742CA0, "345 sub_70BFE0" },
		{ game::sub_70C010, (void *)a_742CD0, "345 sub_70C010" },
		{ game::sub_70C040, (void *)a_742D00, "345 sub_70C040" },
		{ game::sub_70C1F0, (void *)a_742EB0, "345 sub_70C1F0" },
		{ game::sub_70C330, (void *)a_742FF0, "345 sub_70C330" },
		{ game::nullsub_1058, (void *)a_73A6D0, "345 nullsub_1058" },
		{ game::sub_70C5B0, (void *)a_733950, "345 sub_70C5B0" },
		{ game::sub_70C720, (void *)a_73A380, "345 sub_70C720" },
		{ game::nullsub_1059, (void *)a_73A6D0, "345 nullsub_1059" },
		{ game::sub_70C9E0, (void *)a_73A380, "345 sub_70C9E0" },
		{ game::nullsub_1060, (void *)a_73A6D0, "345 nullsub_1060" },
		{ game::nullsub_1061, (void *)a_73A6D0, "345 nullsub_1061" },
		{ game::sub_70D0E0, (void *)a_73A380, "345 sub_70D0E0" },
		{ game::nullsub_1062, (void *)a_73A6D0, "345 nullsub_1062" },
		{ game::nullsub_1063, (void *)a_73A6D0, "345 nullsub_1063" },
		{ game::sub_70D1C0, (void *)a_7475A0, "345 sub_70D1C0" },
		{ game::nullsub_1064, (void *)a_73A6D0, "345 nullsub_1064" },
		{ game::nullsub_1065, (void *)a_73A6D0, "345 nullsub_1065" },
		{ game::nullsub_1066, (void *)a_73A6D0, "345 nullsub_1066" },
		{ game::MAG_345_sub_70D250, (void *)a_7474B0, "345 MAG_345_sub_70D250" },
		{ game::MAG_345_sub_70D270, (void *)a_7474D0, "345 MAG_345_sub_70D270" },
		{ game::nullsub_1067, (void *)a_73A6D0, "345 nullsub_1067" },
		{ game::MAG_345_sub_70D2E0, (void *)a_73A0D0, "345 MAG_345_sub_70D2E0" },
		{ game::MAG_345_sub_70D2F0, (void *)a_747550, "345 MAG_345_sub_70D2F0" },
		{ game::MAG_345_sub_70D300, (void *)a_747560, "345 MAG_345_sub_70D300" },
		{ game::MAG_345_sub_70D330, (void *)a_747590, "345 MAG_345_sub_70D330" },
		{ game::MAG_345_sub_70D340, (void *)a_7475A0, "345 MAG_345_sub_70D340" },
		{ game::nullsub_1057, (void *)a_73A6D0, "345 nullsub_1057" },
		{ {}, nullptr, nullptr }
	};
	static const ModPort PORTS[] = {
		{ game::MAG_345_sub_705DE0, (void *)d_705DE0, "d_705DE0" },
		{ game::MAG_345_sub_705DA0, (void *)d_705DA0, "d_705DA0" },
		{ game::MAG_345_sub_705F00, (void *)d_705F00, "d_705F00" },
		{ game::MAG_345_sub_705ED0, (void *)d_705ED0, "d_705ED0" },
		{ game::sub_706060, (void *)d_706060, "d_706060" },
		{ game::MAG_345_Lv5Death_Director, (void *)d_705FF0, "d_705FF0" },
		{ game::MAG_345_UploadReaperTIMs, (void *)d_7060B0, "d_7060B0" },
		{ game::MAG_345_SpawnReaperCreature, (void *)d_7060F0, "d_7060F0" },
		{ game::sub_706550, (void *)d_706550, "d_706550" },
		{ game::sub_706450, (void *)d_706450, "d_706450" },
		{ game::sub_7061F0, (void *)d_7061F0, "d_7061F0" },
		{ game::MAG_345_Lv5Death_Reaper, (void *)d_706130, "d_706130" },
		{ game::sub_70C8A0, (void *)d_70C8A0, "d_70C8A0" },
		{ game::sub_70C840, (void *)d_70C840, "d_70C840" },
		{ game::sub_70C810, (void *)d_70C810, "d_70C810" },
		{ game::sub_706940, (void *)d_706940, "d_706940" },
		{ game::sub_70C8B0, (void *)d_70C8B0, "d_70C8B0" },
		{ game::sub_70C900, (void *)d_70C900, "d_70C900" },
		{ game::sub_70D030, (void *)d_70D030, "d_70D030" },
		{ game::sub_70D090, (void *)d_70D090, "d_70D090" },
		{ game::sub_70C780, (void *)d_70C780, "d_70C780" },
		{ game::sub_70C7C0, (void *)d_70C7C0, "d_70C7C0" },
		{ game::sub_70D140, (void *)d_70D140, "d_70D140" },
		{ game::sub_70CA40, (void *)d_70CA40, "d_70CA40" },
		{ game::sub_70CA80, (void *)d_70CA80, "d_70CA80" },
		{ game::sub_70CEC0, (void *)d_70CEC0, "d_70CEC0" },
		{ game::MAG_345_Lv5Death_Spark, (void *)d_70CD70, "d_70CD70" },
		{ game::sub_70CFC0, (void *)d_70CFC0, "d_70CFC0" },
		{ game::sub_70CFF0, (void *)d_70CFF0, "d_70CFF0" },
		{ game::sub_7076C0, (void *)d_7076C0, "d_7076C0" },
		{ game::sub_709B00, (void *)d_709B00, "d_709B00" },
		{ game::sub_70ADB0, (void *)d_70ADB0, "d_70ADB0" },
		{ game::sub_70B730, (void *)d_70B730, "d_70B730" },
		{ game::MAG_345_Lv5Death_Master, (void *)d_705C00, "d_705C00" },
		{ game::sub_706AE0, (void *)d_706AE0, "d_706AE0" },
		{ game::sub_706CA0, (void *)d_706CA0, "d_706CA0" },
		{ game::sub_706F20, (void *)d_706F20, "d_706F20" },
		{ game::sub_7075D0, (void *)d_7075D0, "d_7075D0" },
		{ game::sub_707660, (void *)d_707660, "d_707660" },
		{ game::sub_707820, (void *)d_707820, "d_707820" },
		{ game::sub_707950, (void *)d_707950, "d_707950" },
		{ game::sub_707B70, (void *)d_707B70, "d_707B70" },
		{ game::sub_707E00, (void *)d_707E00, "d_707E00" },
		{ game::sub_7082F0, (void *)d_7082F0, "d_7082F0" },
		{ game::sub_708E10, (void *)d_708E10, "d_708E10" },
		{ game::sub_709370, (void *)d_709370, "d_709370" },
		{ game::sub_709A40, (void *)d_709A40, "d_709A40" },
		{ game::sub_709B80, (void *)d_709B80, "d_709B80" },
		{ game::sub_709BC0, (void *)d_709BC0, "d_709BC0" },
		{ game::sub_70ABF0, (void *)d_70ABF0, "d_70ABF0" },
		{ game::sub_70AD00, (void *)d_70AD00, "d_70AD00" },
		{ game::sub_70AD80, (void *)d_70AD80, "d_70AD80" },
		{ game::sub_70AE50, (void *)d_70AE50, "d_70AE50" },
		{ game::sub_70AED0, (void *)d_70AED0, "d_70AED0" },
		{ game::sub_70AF50, (void *)d_70AF50, "d_70AF50" },
		{ game::sub_70B950, (void *)d_70B950, "d_70B950" },
		{ game::sub_70C440, (void *)d_70C440, "d_70C440" },
		{ game::sub_70C4D0, (void *)d_70C4D0, "d_70C4D0" },
		{ game::sub_70C560, (void *)d_70C560, "d_70C560" },
		{ game::sub_70C590, (void *)d_70C590, "d_70C590" },
		{ game::sub_70C6B0, (void *)d_70C6B0, "d_70C6B0" },
		{ game::sub_70D160, (void *)d_70D160, "d_70D160" },
		{ game::MAG_345_sub_70D210, (void *)d_70D210, "d_70D210" },
		{ game::sub_706B10, (void *)d_706B10, "d_706B10" },
		{ game::MAG_345_sub_70D2A0, (void *)d_70D2A0, "d_70D2A0" },
		{ {}, nullptr, nullptr }
	};
}
}

	void register_mag345_lv5_death()
	{
		act::register_module(345);
		for (const act::lv5_death::ModPort *p = act::lv5_death::ENGINE_PORTS; p->addr; p++)
			act::register_module_port(345, p->addr, p->port, p->name);
		for (const act::lv5_death::ModPort *p = act::lv5_death::PORTS; p->addr; p++)
			act::register_module_port(345, p->addr, p->port, p->name);
	}
}

