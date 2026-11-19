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

#include "act_engine.h"

namespace ff8fx
{
namespace act
{
namespace norgpod
{
	static const Mod MOD_020 = { "norgpod", 20, game::MAG_020_NORG_POD_OPENING_FL, game::MAG_019_UNK0_FL,
		{ {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_020_EmitterQueue, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {} },
		{ {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_020_NorgPod_EmitterTask, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_020_SpriteStrip_DrawQuads, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {} } };

	static constexpr game::Id PACKET_CURSOR = game::MAG_020_PacketCursor;
	static constexpr game::Id Q_EMITTER = game::MAG_020_EmitterQueue, Q_POD = game::MAG_020_LidDebrisQueue, Q_CHUNK = game::MAG_020_ChunkQueue, Q_STAGE = game::MAG_020_StageQueue;
	static constexpr game::Id ORIG_Master = game::MAG_020_NorgPod_MasterTask;
	static constexpr game::Id ORIG_Lid = game::MAG_020_NorgPod_LidTask;
	static constexpr game::Id ORIG_Debris = game::MAG_020_NorgPod_DebrisTask;
	static constexpr game::Id ORIG_Chunk = game::MAG_020_NorgPod_ChunkTask;
	static constexpr game::Id SOUND_Open = game::unk_1629C50;
	static inline uint32_t ENT(uint32_t slot) { return game_addr(game::BattleEntitySlotData) + slot * 0x9C; }

	namespace gx
	{
		static inline uint32_t QueueChainTransformation(uint32_t a1, uint32_t a2) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::QueueChainTransformation))(a1, a2); }
		static inline uint32_t setMonsterPresence(uint32_t a1) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::setMonsterPresence))(a1); }
	}

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
	static inline bool b1_out(int16_t v, int16_t hi) { return v < 0 || v > hi; }
	static inline bool b2_out_x(int16_t v) { return v < 0 || v > 0xA00; }
	static inline bool b2_out_y(int16_t v) { return v < 0 || v > 0x6C0; }
	static inline int32_t b2_fx(uint32_t v)
	{
		int32_t s = (int32_t)v;
		return add32(s, (s >> 31) & 0xFFFF) >> 16;
	}

	static inline uint32_t task_end(uint32_t node, uint8_t status)
	{
		if ((status & 1) != 0 && U8(node, 0x28) == 0)
		{
			x::Effect_ReleaseLinkedTask(node);
			return 2;
		}
		return 0;
	}

	static inline int32_t div3(int32_t v)
	{
		const int32_t hi = (int32_t)(((int64_t)v * 0x55555556LL) >> 32);
		return add32(hi, (int32_t)((uint32_t)hi >> 31));
	}
	static inline int32_t div10(int32_t v)
	{
		const int32_t hi = (int32_t)(((int64_t)v * 0x66666667LL) >> 32) >> 2;
		return add32(hi, (int32_t)((uint32_t)hi >> 31));
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

	namespace
	{
		uint32_t s2_twin(uint32_t ctx, int32_t b)
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
	}
	namespace
	{
		uint32_t s3_texwin(uint32_t ctx, int32_t o)
		{
			uint32_t c = ((uint32_t)U8(ctx, o + 2) & 0xF8) | 0xFFFE2000u;
			c = (c << 5) | ((uint32_t)U8(ctx, o) & 0xF8);
			c = (c << 5) | (~((uint32_t)U16(ctx, o + 6) - 1u) & 0xF8);
			c = (c << 2) | ((~((uint32_t)U16(ctx, o + 4) - 1u) >> 3) & 0x1F);
			return c;
		}

		void s3_scroll_uv(uint32_t pkt, int32_t o, uint32_t d, uint32_t ctx, int32_t wo)
		{
			int32_t a = (int32_t)(U8(pkt, o) + d);
			int32_t c = (int32_t)(U8(pkt, o + 0xC) + d);
			int32_t e = (int32_t)(U8(pkt, o + 0x18) + d);
			int32_t b = (int32_t)(U8(pkt, o + 0x24) + d);
			if ((b | e | c | a) > 0xFF)
			{
				uint8_t w = U8(ctx, wo);
				U8(pkt, o) = (uint8_t)(a - w);
				U8(pkt, o + 0xC) = (uint8_t)(c - w);
				U8(pkt, o + 0x18) = (uint8_t)(e - w);
				U8(pkt, o + 0x24) = (uint8_t)(b - w);
			}
			else
			{
				U8(pkt, o) = (uint8_t)a;
				U8(pkt, o + 0xC) = (uint8_t)c;
				U8(pkt, o + 0x18) = (uint8_t)e;
				U8(pkt, o + 0x24) = (uint8_t)b;
			}
		}

		inline bool s3_off(int16_t v, int16_t lim) { return v < 0 || v > lim; }

		uint32_t s3_quad(uint32_t ctx, uint32_t data, uint32_t vbase, uint32_t ot_base, uint32_t shift, uint32_t pkt)
		{
			x::GTE_LoadV012(vbase + ((uint32_t)U16(data, 4) << 2), vbase + ((uint32_t)U16(data, 6) << 2), vbase + ((uint32_t)U16(data, 8) << 2));
			x::GTE_RTPT();
			uint32_t flags = U32(ctx, 0x14);
			uint32_t col0 = U32(data, 0);
			U32(pkt, 0) = 0x0C000000;
			U32(pkt, 4) = col0;
			if (flags & 2)
			{
				col0 |= 0x2000000;
				U32(pkt, 4) = col0;
			}
			if (flags & 8)
				U32(pkt, 4) &= 0xFDFFFFFF;
			uint32_t uv0 = U32(data, 0xC);
			uint32_t uv1 = U32(data, 0x10);
			U32(pkt, 0xC) = uv0;
			uint32_t uv23 = U32(data, 0x14);
			U32(pkt, 0x24) = uv23;
			U32(pkt, 0x18) = uv1;
			U32(pkt, 0x30) = uv23 >> 16;
			x::GTE_ReadFLAG(ctx + 0x3C);
			if (U32(ctx, 0x3C) & 0x60000)
				return pkt;
			x::GTE_NCLIP();
			uint32_t clip = 0;
			x::GTE_ReadMAC0(ctx + 0x30);
			if (S32(ctx, 0x30) < 0 && !(U8(ctx, 0x14) & 0x20))
				return pkt;
			x::GTE_ReadSXY012_Split(pkt + 8, pkt + 0x14, pkt + 0x20);
			x::GTE_LoadV0(vbase + ((uint32_t)U16(data, 0xA) << 2));
			x::GTE_RTPS();
			if (s3_off(S16(pkt, 0x8), 0xA00)) clip = 1;
			if (s3_off(S16(pkt, 0x14), 0xA00)) clip |= 2;
			if (s3_off(S16(pkt, 0x20), 0xA00)) clip |= 4;
			if (s3_off(S16(pkt, 0xA), 0x6C0)) clip |= 0x10;
			if (s3_off(S16(pkt, 0x16), 0x6C0)) clip |= 0x20;
			if (s3_off(S16(pkt, 0x22), 0x6C0)) clip |= 0x40;
			x::GTE_ReadSXY2(pkt + 0x2C);
			x::GTE_AVSZ4();
			uint32_t c = clip;
			if (s3_off(S16(pkt, 0x2C), 0xA00)) c |= 8;
			if (s3_off(S16(pkt, 0x2E), 0x6C0)) c |= 0x80;
			if ((c & 0xF) == 0xF)
				return pkt;
			if ((c & 0xF0) == 0xF0)
				return pkt;
			x::GTE_ReadOTZ(ctx + 0x38);
			if (U8(ctx, 0x14) & 0x80)
			{
				x::sub_45E120(data + 0x18, data + 0x1C, data + 0x20);
				x::set_dword_1CA8A30(U32(ctx, 0xC));
				x::sub_45F4C0();
				x::sub_45E370(pkt + 0x10, pkt + 0x1C, pkt + 0x28);
				x::set_unk_1CA8A28(pkt + 4);
				x::sub_45F270();
				x::set_param_with_dword_1CA8A68(pkt + 4);
			}
			else
			{
				uint32_t c1 = U32(data, 0x18);
				uint32_t c2 = U32(data, 0x1C);
				U32(pkt, 0x10) = c1;
				uint32_t c3 = U32(data, 0x20);
				U32(pkt, 0x1C) = c2;
				U32(pkt, 0x28) = c3;
			}
			int32_t z = add32(S32(ctx, 0x38), S32(ctx, 0x10));
			S32(ctx, 0x38) = z;
			if (z < 0)
				S32(ctx, 0x38) = 0;
			uint32_t ot = ot_base + ((uint32_t)(S32(ctx, 0x38) >> (shift & 31)) << 2);
			uint16_t du = U16(ctx, 0x18);
			uint16_t dv = U16(ctx, 0x1A);
			if ((uint16_t)(du | dv) == 0)
			{
				x::SSIGPU_InsertPrimAutoDepth(ot, pkt);
				return pkt + 0x34;
			}
			if (du != 0)
				s3_scroll_uv(pkt, 0xC, du, ctx, 0x28);
			uint16_t dv2 = U16(ctx, 0x1A);
			if (dv2 != 0)
				s3_scroll_uv(pkt, 0xD, dv2, ctx, 0x2A);
			uint32_t prim = pkt;
			uint32_t twa = pkt + 0x34;
			U32(twa, 0) = 0x2000000;
			uint32_t w = (ctx + 0x1C != 0) ? s3_texwin(ctx, 0x1C) : 0;
			U32(twa, 4) = w;
			U32(twa, 8) = 0;
			x::SSIGPU_InsertPrimAutoDepth(ot, twa);
			x::SSIGPU_InsertPrimAutoDepth(ot, prim);
			uint32_t twb = pkt + 0x40;
			U32(twb, 0) = 0x2000000;
			w = (ctx + 0x24 != 0) ? s3_texwin(ctx, 0x24) : 0;
			U32(twb, 4) = w;
			U32(twb, 8) = 0;
			x::SSIGPU_InsertPrimAutoDepth(ot, twb);
			return pkt + 0x4C;
		}

		inline uint32_t s3_task_end(uint32_t n, uint8_t status)
		{
			if ((status & 1) && U8(n, 0x28) == 0)
			{
				x::Effect_ReleaseLinkedTask(n);
				return 2;
			}
			return 0;
		}
	}

	static inline void t4_next_state(uint32_t node) { U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1); }
	static inline void chunk_move(uint32_t b)
	{
		U16(b, 0x76) = (uint16_t)(U16(b, 0x76) + 0x100);
		for (int32_t o = 0x74; o <= 0x78; o += 2)
		{
			const int16_t v = S16(b, o);
			S16(b, o) = (int16_t)(v - (int32_t)v / 8);
		}
		for (int32_t o = 0; o <= 4; o += 2)
			S16(b, 0x54 + o) = (int16_t)(S16(b, 0x54 + o) + (int32_t)S16(b, 0x74 + o) / 16);
	}

	uint32_t __cdecl np_89FD00(uint32_t a1);
	uint32_t __cdecl np_89FE60(uint32_t a1);
	uint32_t __cdecl np_89FF30(uint32_t a1);
	uint32_t __cdecl np_89FFB0(uint32_t a1);
	uint32_t __cdecl np_8A0040(uint32_t a1);
	uint32_t __cdecl np_8A0070(uint32_t a1);
	uint32_t __cdecl np_8A00E0(uint32_t a1);
	uint32_t __cdecl np_8A01B0(uint32_t a1);
	uint32_t __cdecl np_8A0230(uint32_t a1);
	uint32_t __cdecl np_8A0900(uint32_t a1, uint32_t a2);
	uint32_t __cdecl np_8A0990(uint32_t a1);
	uint32_t __cdecl np_8A0AA0(uint32_t a1);
	uint32_t __cdecl np_8A0BC0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl np_8A0CF0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl np_8A0F10(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl np_8A11A0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl np_8A1620(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl np_8A20D0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl np_8A25D0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl np_8A2C20(uint32_t a1);
	uint32_t __cdecl np_8A2C60(uint32_t a1);
	uint32_t __cdecl np_8A2CB0(uint32_t a1);
	uint32_t __cdecl np_8A2CD0(uint32_t a1);
	uint32_t __cdecl np_8A2D30(uint32_t a1);
	uint32_t __cdecl np_8A2D90(uint32_t a1);
	uint32_t __cdecl np_8A2F70(uint32_t a1);
	uint32_t __cdecl np_8A2FE0(uint32_t a1);
	uint32_t __cdecl np_8A3070(uint32_t a1);
	uint32_t __cdecl np_8A3090(uint32_t a1);
	uint32_t __cdecl np_8A30E0(uint32_t a1);
	uint32_t __cdecl np_8A3120(uint32_t a1);
	uint32_t __cdecl np_8A31B0(uint32_t a1);
	uint32_t __cdecl np_8A31D0(uint32_t a1);
}
}
}

namespace ff8fx
{
namespace act
{
namespace norgpod
{
	uint32_t __cdecl np_89FD00(uint32_t a1)
	{
		g_mod = &MOD_020;

		uint32_t states[11];
		ff8fx::copy<(8 * 4)>((void *)game_addr(game::Effect_SharedCamMatrixSnapshot), (const void *)game_addr(game::unk_1D97778));
		const uint32_t node = a1;
		MEM<uint32_t>(game_addr(game::MAG_020_CameraCopyPtr)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		MEM<uint32_t>(game_addr(game::MAG_020_CameraCopyPtr2)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		states[0] = game_addr(game::MAG_020_NorgPod_Master_State0_Advance);
		const uint8_t parity = U8(node, 0x5C);
		states[1] = game_addr(game::MAG_020_NorgPod_Master_State1_Advance);
		states[2] = game_addr(game::MAG_020_NorgPod_Master_StartLightRampUp);
		states[3] = game_addr(game::MAG_020_NorgPod_Master_State3_SpawnEmitter);
		states[4] = game_addr(game::MAG_020_NorgPod_Master_ActionLoop);
		states[5] = game_addr(game::MAG_020_NorgPod_Master_StartLightRampDown);
		states[6] = game_addr(game::MAG_020_NorgPod_Master_State6_WaitQueuesEmpty);
		states[7] = game_addr(game::MAG_020_NorgPod_Master_State7_Advance);
		states[8] = game_addr(game::MAG_020_NorgPod_Master_State8_Advance);
		states[9] = game_addr(game::MAG_020_NorgPod_Master_State9_Finish);
		states[10] = game_addr(game::MAG_020_NorgPod_Master_State10_Empty);
		if ((parity & 1) != 0)
		{
			const uint32_t v1 = MEM<uint32_t>(game_addr(game::MAG_020_ArenaB));
			const uint32_t v2 = MEM<uint32_t>(game_addr(game::MAG_020_Arena2B));
			MEM<uint32_t>(game_addr(PACKET_CURSOR)) = v1;
			MEM<uint32_t>(game_addr(game::MAG_020_PacketCursor2)) = v2;
		}
		else
		{
			const uint32_t v1 = MEM<uint32_t>(game_addr(game::MAG_020_ArenaA));
			const uint32_t v2 = MEM<uint32_t>(game_addr(game::MAG_020_Arena2A));
			MEM<uint32_t>(game_addr(PACKET_CURSOR)) = v1;
			MEM<uint32_t>(game_addr(game::MAG_020_PacketCursor2)) = v2;
		}
		x::Effect_UpdateTargetPosFromBones(node);
		callp(states[S8(node, 0x29)], node);
		U16(node, 0x5E) = 0;
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_EMITTER)));
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_POD)));
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_CHUNK)));
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_STAGE)));
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x5C) = (uint16_t)(U16(node, 0x5C) + 1);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl np_89FE60(uint32_t a1)
	{
		if (U8(a1, 0x28) != 0)
			return 0;
		x::Effect_AddTaskAndInitFromCtx(game_addr(Q_STAGE), game_addr(game::MAG_020_NorgPod_LightRampUpTask), 0x40, a1);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl np_89FF30(uint32_t a1)
	{
		U16(a1, 0x1C) = (uint16_t)(U16(a1, 0x1C) + 0x100);
		if (S16(a1, 0x1C) >= 0x400)
		{
			const uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) |= 1;
			U16(a1, 0x1C) = 0x400;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		const uint16_t level = U16(a1, 0x1C);
		uint32_t p = game_addr(game::unk_1D98992);
		for (int i = 4; i != 0; i--, p += 0x2C)
			MEM<uint16_t>(p) = level;
		return 0;
	}

	uint32_t __cdecl np_89FFB0(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[4];
		states[0] = game_addr(game::MAG_020_NorgPod_Emitter_SpawnLid);
		states[1] = game_addr(game::MAG_020_NorgPod_Emitter_ReleaseMaster);
		states[2] = game_addr(game::MAG_020_NorgPod_Emitter_ApplyDamage);
		states[3] = game_addr(game::MAG_020_NorgPod_Emitter_State3_Empty);
		x::MAG_001_CURE_Emitter_UpdatePos(node);
		x::MAG_001_CURE_Emitter_ComputeModelBounds(node);
		callp(states[S8(node, 0x29)], node);
		if (U16(node, 0x24) == 0)
			x::BdPlaySE(game_addr(SOUND_Open), 0, 0x80);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl np_8A0040(uint32_t a1)
	{
		x::Effect_AddTaskAndInitFromCtx(game_addr(Q_POD), game_addr(ORIG_Lid), 0x78, a1);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl np_8A3070(uint32_t a1)
	{
		if (U16(a1, 0x24) == 0x1E)
		{
			U8(U32(a1, 0x10), 0x63) = 0;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return a1;
	}

	uint32_t __cdecl np_8A3090(uint32_t a1)
	{
		const uint32_t node = a1;
		if (S16(node, 0x24) < 0x1E)
			return 0;
		const int32_t ai = S8(node, 0x2A);
		const int32_t ti = S8(node, 0x2B);
		const uint32_t acts = U32(U32(node, 0xC), 4);
		const uint32_t recs = U32(acts + (uint32_t)(ai * 5) * 4, 8);
		x::ApplyActionResultToTarget(recs + (uint32_t)(ti * 3) * 8);
		const uint8_t st = U8(node, 0x29);
		U8(node, 0x26) |= 1;
		U8(node, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	uint32_t __cdecl np_8A30E0(uint32_t a1)
	{
		const uint32_t node = a1;
		if (U8(node, 0x63) != 0)
			return a1;
		const uint8_t idx = U8(node, 0x2A);
		if ((int16_t)(int8_t)idx < S16(node, 0x58))
		{
			const uint8_t loops = U8(node, 0x2E);
			U8(node, 0x2A) = (uint8_t)(idx + 1);
			const uint8_t st = U8(node, 0x29);
			U8(node, 0x2E) = (uint8_t)(loops + 1);
			U8(node, 0x29) = (uint8_t)(st - 1);
			return a1;
		}
		const uint8_t st = U8(node, 0x29);
		U16(node, 0x60) = 0xF;
		U8(node, 0x29) = (uint8_t)(st + 1);
		return a1;
	}

	uint32_t __cdecl np_8A3120(uint32_t a1)
	{
		U16(a1, 0x60) = (uint16_t)(U16(a1, 0x60) - 1);
		if (S16(a1, 0x60) > 0)
			return 0;
		x::Effect_AddTaskAndInitFromCtx(game_addr(Q_STAGE), game_addr(game::MAG_020_NorgPod_LightRampDownTask), 0x40, a1);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl np_8A31B0(uint32_t a1)
	{
		const uint8_t st = U8(a1, 0x29);
		U16(a1, 0x1C) = 0x400;
		U8(a1, 0x29) = (uint8_t)(st + 1);
		return a1;
	}

	uint32_t __cdecl np_8A31D0(uint32_t a1)
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

	uint32_t __cdecl np_8A0070(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[4];
		states[0] = game_addr(game::MAG_020_NorgPod_Lid_Init);
		states[1] = game_addr(game::MAG_020_NorgPod_Lid_WaitLaunch);
		states[2] = game_addr(game::MAG_020_NorgPod_Lid_Flight);
		states[3] = game_addr(game::MAG_020_NorgPod_Lid_State3_Empty);
		callp(states[S8(node, 0x29)], node);
		np_8A00E0(node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl np_8A2D30(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t ent = ENT(U8(node, 0x2C));
		const uint32_t slot = x::BdSound_ClaimVoiceSlot(game_addr(game::unk_17F36A0), 1, 0x80);
		U16(node, 0x74) = (uint16_t)slot;
		U32(node, 0x1C) = U32(ent, 0x1C);
		U32(node, 0x20) = U32(ent, 0x20);
		U16(node, 0x1E) = (uint16_t)(U16(node, 0x1E) + 0xEEC0);
		U32(ent, 0x7C) = U32(ent, 0x7C) & 0xFFFFFFFDu;
		np_8A2D90(node);
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl np_8A2D90(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t ent = ENT(U8(node, 0x2C));
		uint32_t bones = U32(U32(ent, 0x64), 0) + 0x40;
		const uint32_t blk = x::Field_Alloc(0x8C);
		const uint32_t saved7c = U32(ent, 0x7C);
		U32(ent, 0x7C) = 2;
		x::BattleModel_BuildBoneMatricesFromPose(ent + 0x60);
		x::MAG_022_sub_8DD770(blk + 0x40);
		S32(blk, 0x54) = S16(node, 0x58);
		const int32_t oy = S16(node, 0x5A);
		S32(blk, 0x5C) = S16(node, 0x5C);
		U32(bones, 0x28) = U32(bones, 0x28) + 0x1140;
		S32(blk, 0x58) = oy - 0x1140;
		x::MAG_022_sub_8DD8A0(blk + 0x40, (uint32_t)(int32_t)S16(node, 0x6A));
		x::sub_8DD7E0(blk + 0x40, (uint32_t)(int32_t)S16(node, 0x68));
		x::sub_8DD960(blk + 0x40, (uint32_t)(int32_t)S16(node, 0x6C));
		bones += 0x10;
		x::ComposeAffineTransform(blk + 0x40, bones, bones);
		ff8fx::copy<(8 * 4)>((void *)blk, (const void *)(ent + 0x40));
		x::GTE_SetRotMatrix(game_addr(game::unk_1D97778));
		x::GTE_LoadIRFromMatrixColumn(blk);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(blk + 0x20);
		x::GTE_LoadIRFromMatrixColumn(blk + 2);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(blk + 0x22);
		x::GTE_LoadIRFromMatrixColumn(blk + 4);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(blk + 0x24);
		x::GTE_SetTransVector(game_addr(game::unk_1D97778));
		x::GTE_LoadV0FromDwords(blk + 0x14);
		x::GTE_MVMVA_RotV0_Tr();
		x::GTE_ReadMAC123(blk + 0x34);
		x::ComposeAffineTransform(blk + 0x20, bones, bones);
		U32(blk, 0x64) = MEM<uint32_t>(game_addr(game::g_BattleVertexWorkspace));
		const uint8_t shade = U8(ent, 7);
		U8(blk, 0x8A) = shade;
		U8(blk, 0x89) = shade;
		U8(blk, 0x88) = shade;
		U32(blk, 0x7C) = U32(ent, 0x28);
		U16(blk, 0x74) = 0;
		U16(blk, 0x76) = 0;
		U16(blk, 0x84) = 0;
		U32(blk, 0x80) = U32(ent, 0x7C);
		U16(blk, 0x78) = 0x140;
		U16(blk, 0x7A) = 0xD8;
		U32(blk, 0x70) = MEM<uint32_t>(game_addr(game::BATTLE_PACKET_BUFFER_END));
		MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = x::RenderGeometry(U32(ent, 0x64), blk + 0x60, MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 4, MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)));
		x::BattleModel_BuildBoneMatricesFromPose(ent + 0x60);
		U32(ent, 0x7C) = saved7c;
		x::Field_Free(0x8C);
		return 0;
	}

	uint32_t __cdecl np_8A2F70(uint32_t a1)
	{
		const uint32_t node = a1;
		if (S16(node, 0x24) >= 0x22)
		{
			const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_STAGE), game_addr(game::Effect_ScreenTintTask), 0x40, node);
			U32(t, 0x34) = game_addr(game::unk_1629F14);
			U16(t, 0x38) = 1;
			U16(node, 0x60) = 0x120;
			U16(node, 0x62) = 0xFEC0;
			U16(node, 0x64) = 0x20;
			gx::QueueChainTransformation(game_addr(game::unk_1D97668), 0xC);
			const uint8_t st = U8(node, 0x29);
			U16(node, 0x70) = 0x18;
			U8(node, 0x29) = (uint8_t)(st + 1);
		}
		np_8A2D90(node);
		return 0;
	}

	uint32_t __cdecl np_8A2FE0(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint16_t ax0 = U16(node, 0x68);
		U16(node, 0x62) = (uint16_t)(U16(node, 0x62) + 0x28);
		const uint16_t cz0 = U16(node, 0x6C);
		const uint16_t vx = U16(node, 0x60);
		U16(node, 0x58) = (uint16_t)(U16(node, 0x58) + vx);
		U16(node, 0x68) = (uint16_t)((uint16_t)(ax0 - 0x40) & 0xFFF);
		U16(node, 0x5A) = (uint16_t)(U16(node, 0x5A) + U16(node, 0x62));
		U16(node, 0x5C) = (uint16_t)(U16(node, 0x5C) + U16(node, 0x64));
		const uint16_t cz = (uint16_t)((uint16_t)(cz0 + 0x30) & 0xFFF);
		U16(node, 0x70) = (uint16_t)(U16(node, 0x70) - 1);
		const bool over = S16(node, 0x70) <= 0;
		U16(node, 0x6C) = cz;
		if (over)
		{
			gx::setMonsterPresence(6);
			x::sub_4A2940((uint32_t)(int32_t)S16(node, 0x74));
			const uint8_t st = U8(node, 0x29);
			U8(node, 0x26) |= 1;
			U8(node, 0x29) = (uint8_t)(st + 1);
		}
		np_8A2D90(node);
		return 0;
	}

	uint32_t __cdecl np_8A00E0(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint16_t k = U16(node, 0x72);
		if (MEM<uint16_t>(game_addr(game::unk_1629F88) + (uint32_t)(int32_t)(int16_t)k * 2) != U16(node, 0x24))
			return 0;
		U16(node, 0x72) = (uint16_t)(k + 1);
		for (int32_t i = 0; i < 4; i++)
		{
			const int32_t r = (int32_t)x::CrtRand();
			const uint32_t off = game_addr(game::unk_1629F28) + (uint32_t)(r % 12) * 8;
			const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_POD), game_addr(ORIG_Debris), 0x78, node);
			U32(t, 0x1C) = U32(node, 0x1C);
			U32(t, 0x20) = U32(node, 0x20);
			U16(t, 0x1C) = (uint16_t)(U16(t, 0x1C) + MEM<uint16_t>(off));
			U16(t, 0x1E) = (uint16_t)(U16(t, 0x1E) + MEM<uint16_t>(off + 2));
			U16(t, 0x20) = (uint16_t)(U16(t, 0x20) + MEM<uint16_t>(off + 4));
			U16(t, 0x1C) = (uint16_t)(U16(t, 0x1C) + (uint16_t)((x::CrtRand() & 0xFF) - 0x80));
			U16(t, 0x1E) = (uint16_t)(U16(t, 0x1E) + (uint16_t)((x::CrtRand() & 0xFF) - 0x80));
			U16(t, 0x20) = (uint16_t)(U16(t, 0x20) + (uint16_t)((x::CrtRand() & 0xFF) - 0x80));
			if ((i & 1) != 0)
				U16(t, 0x1C) = (uint16_t)(0 - U16(t, 0x1C));
		}
		return 0;
	}

	uint32_t __cdecl np_8A01B0(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[3];
		states[0] = game_addr(game::MAG_020_NorgPod_Debris_Init);
		states[1] = game_addr(game::MAG_020_NorgPod_Debris_Animate);
		states[2] = game_addr(game::MAG_020_NorgPod_Debris_State2_Empty);
		callp(states[S8(node, 0x29)], node);
		const uint16_t tick = U16(node, 0x24);
		if (tick == 0 || tick == 1)
			np_8A0900(node, 1);
		np_8A0230(node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl np_8A2CB0(uint32_t a1)
	{
		uint32_t node = a1;
		uint8_t st = U8(node, 0x29);
		U32(node, 0x4c) = game_addr(game::unk_1629C54);
		st++;
		U16(node, 0x56) = 0xFE00;
		U16(node, 0x52) = 0xF;
		U8(node, 0x29) = st;
		return 0;
	}

	uint32_t __cdecl np_8A2CD0(uint32_t a1)
	{
		const uint32_t node = a1;
		if (a_743C20(node) != 0)
		{
			U8(node, 0x26) |= 5;
			t4_next_state(node);
		}
		return 0;
	}

	uint32_t __cdecl np_8A0230(uint32_t a1)
	{
		const uint32_t node = a1;
		if ((U8(node, 0x26) & 4) != 0)
			return 0;
		const uint32_t blk = x::Field_Alloc(0xD8);
		const uint32_t mat = blk + 0xB8;
		x::UnpackRotationMatrix(game_addr(game::unk_1D97778), mat);
		const int32_t px = S16(node, 0x1C);
		const int32_t py = S16(node, 0x1E);
		const int32_t pz = S16(node, 0x20);
		const uint32_t tr = blk + 0xCC;
		S32(blk, 0xD0) = py;
		S32(blk, 0xD4) = pz;
		S32(tr, 0) = px;
		x::GTE_SetRotMatrix(game_addr(game::unk_1D97778));
		x::GTE_LoadIRFromMatrixColumn(mat);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(mat);
		x::GTE_LoadIRFromMatrixColumn(blk + 0xBA);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(blk + 0xBA);
		x::GTE_LoadIRFromMatrixColumn(blk + 0xBC);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(blk + 0xBC);
		x::GTE_SetTransVector(game_addr(game::unk_1D97778));
		x::GTE_LoadV0FromDwords(tr);
		x::GTE_MVMVA_RotV0_Tr();
		x::GTE_ReadMAC123(tr);
		x::GTE_SetRotMatrix(mat);
		x::GTE_SetTransVector(mat);
		U32(blk, 0) = U32(node, 0x4C);
		const uint16_t frame = U16(node, 0x50);
		const uint16_t colour = U16(node, 0x56);
		U16(blk, 4) = frame;
		const uint32_t cursor = MEM<uint32_t>(game_addr(PACKET_CURSOR));
		const uint32_t ot = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44;
		U16(blk, 0x24) = 0;
		U16(blk, 0xB4) = colour;
		MEM<uint32_t>(game_addr(PACKET_CURSOR)) = a_7435E0(blk, ot, 2, cursor);
		x::Field_Free(0xD8);
		return 0;
	}

	uint32_t __cdecl np_8A0900(uint32_t a1, uint32_t a2)
	{
		int32_t n = (int16_t)a2;
		if (n <= 0)
			return 0;
		const uint32_t src = a1;
		do
		{
			const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_CHUNK), game_addr(ORIG_Chunk), 0x98, src);
			uint32_t v = t + 0x74;
			for (int k = 2; k != 0; k--, v += 8)
			{
				U32(v, -0x20) = U32(src, 0x1C);
				U32(v, -0x1C) = U32(src, 0x20);
				U16(v, 0) = (uint16_t)((x::CrtRand() & 0x3FFF) - 0x2000);
				U16(v, 4) = (uint16_t)((x::CrtRand() & 0x3FFF) - 0x2000);
				const uint32_t r = x::CrtRand() & 0xFFF;
				U16(v, 2) = (uint16_t)(0xFFFFFF00u - r);
			}
		} while (--n != 0);
		return 0;
	}

	uint32_t __cdecl np_8A0990(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[3];
		states[0] = game_addr(game::MAG_020_NorgPod_Chunk_Init);
		states[1] = game_addr(game::MAG_020_NorgPod_Chunk_SpinShrink);
		states[2] = game_addr(game::MAG_020_NorgPod_Chunk_State2_Empty);
		callp(states[S8(node, 0x29)], node);
		for (int k = 0; k < 2; k++)
			chunk_move(node + 8 * k);
		for (int k = 0; k < 2; k++)
		{
			const uint32_t p = node + 0x54 + 8 * k;
			U32(node, 0x1C) = U32(p, 0);
			U32(node, 0x20) = U32(p, 4);
			np_8A0AA0(node);
		}
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl np_8A2C20(uint32_t a1)
	{
		const uint32_t node = a1;
		U32(node, 0x4C) = game_addr(game::unk_1629E00);
		U32(node, 0x38) = 0x300;
		U32(node, 0x34) = 0x300;
		U32(node, 0x30) = 0x300;
		U16(node, 0x48) = (uint16_t)(x::CrtRand() & 0xFFF);
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl np_8A2C60(uint32_t a1)
	{
		const uint32_t node = a1;
		U16(node, 0x48) = (uint16_t)(U16(node, 0x48) + 0x20);
		if (S16(node, 0x24) < 8)
			return 0;
		const int32_t s = (int32_t)(U32(node, 0x38) + 0xFFFFFF40u);
		S32(node, 0x38) = s;
		if (s <= 0)
		{
			U8(node, 0x26) |= 1;
			U32(node, 0x38) = 0x10;
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		}
		const uint32_t sc = U32(node, 0x38);
		U32(node, 0x34) = sc;
		U32(node, 0x30) = sc;
		return 0;
	}

	uint32_t __cdecl np_8A0AA0(uint32_t a1)
	{
		const uint32_t node = a1;
		if ((U8(node, 0x26) & 4) != 0)
			return 0;
		uint32_t mbuf[8] = {};
		const uint32_t m = P(mbuf);
		x::MAG_022_sub_8DD770(m);
		x::MAG_022_sub_8DD8A0(m, (uint32_t)(int32_t)S16(node, 0x46));
		x::sub_8DD7E0(m, (uint32_t)(int32_t)S16(node, 0x44));
		x::sub_8DD960(m, (uint32_t)(int32_t)S16(node, 0x48));
		S32(m, 0x14) = S16(node, 0x1C);
		S32(m, 0x18) = S16(node, 0x1E);
		S32(m, 0x1C) = S16(node, 0x20);
		x::scale3DMatrix(m, node + 0x30);
		x::ComposeAffineTransform(game_addr(game::unk_1D97778), m, m);
		x::GTE_SetRotMatrix_W(m);
		x::GTE_SetTransVector_W(m);
		const uint32_t h = x::Field_Alloc(0x68);
		U32(h, 0) = U32(node, 0x4C);
		U32(h, 8) = U32(node, 0x40);
		S32(h, 0xC) = S16(node, 0x50);
		U16(h, 0x22) = 0x100;
		U16(h, 0x20) = 0x100;
		U16(h, 0x2A) = 0x100;
		U16(h, 0x28) = 0x100;
		S32(h, 0x10) = S16(node, 0x52);
		U16(h, 0x18) = 0;
		U16(h, 0x1A) = 0;
		U16(h, 0x1E) = 0;
		U16(h, 0x1C) = 0;
		U16(h, 0x26) = 0;
		U16(h, 0x24) = 0;
		U32(h, 0x14) = 0xF0;
		MEM<uint32_t>(game_addr(PACKET_CURSOR)) = np_8A0BC0(h, MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 2, MEM<uint32_t>(game_addr(PACKET_CURSOR)));
		x::Field_Free(0x68);
		return 0;
	}

	uint32_t __cdecl np_8A0BC0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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
		static const Draw lists[8] = { np_8A0CF0, np_8A0F10, np_8A11A0, np_8A1620, a_73D770, a_73D9C0, np_8A20D0, np_8A25D0 };
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

	uint32_t __cdecl np_8A0CF0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl np_8A0F10(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl np_8A11A0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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
		uint32_t pSXY2 = pkt + 0x18;
		uint32_t pSXY1 = pkt + 0x10;
		uint32_t pCode = pkt + 4;
		uint32_t pSXY0 = pkt + 8;
		do
		{
			x::GTE_LoadV012(vbase + (uint32_t)U16(rec, 4) * 4u, vbase + (uint32_t)U16(rec, 6) * 4u,
				vbase + (uint32_t)U16(rec, 8) * 4u);
			x::GTE_RTPT();
			{
				const uint32_t flags = U32(ctx, 0x14);
				const uint32_t code = U32(rec, 0);
				U32(pkt, 0) = 0x7000000;
				U32(pCode, 0) = code;
				if (flags & 1)
					U32(pCode, 0) = code | 0x2000000;
				if (flags & 4)
					U32(pCode, 0) = U32(pCode, 0) & 0xFDFFFFFFu;
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
				x::GTE_ReadSXY012_Split(pSXY0, pSXY1, pSXY2);
				x::GTE_AVSZ3();
				int16_t v = S16(pSXY0, 0);
				if (v < 0 || v > 0xA00)
					clip = 1;
				v = S16(pSXY1, 0);
				if (v < 0 || v > 0xA00)
					clip |= 2;
				v = S16(pSXY2, 0);
				if (v < 0 || v > 0xA00)
					clip |= 4;
				v = S16(pkt, 0xA);
				if (v < 0 || v > 0x6C0)
					clip |= 0x10;
				v = S16(pkt, 0x12);
				if (v < 0 || v > 0x6C0)
					clip |= 0x20;
				v = S16(pkt, 0x1A);
				if (v < 0 || v > 0x6C0)
					clip |= 0x40;
				if ((clip & 7) == 7)
					goto next;
				if ((clip & 0x70) == 0x70)
					goto next;
			}
			x::GTE_ReadOTZ(ctx + 0x38);
			if (U8(ctx, 0x14) & 0x40)
			{
				x::set_unk_1CA8A28(pCode);
				x::set_dword_1CA8A30(U32(ctx, 0xC));
				x::sub_45F270();
				x::set_param_with_dword_1CA8A68(pCode);
			}
			{
				const int32_t z = add32(S32(ctx, 0x38), S32(ctx, 0x10));
				S32(ctx, 0x38) = z;
				if (z < 0)
					S32(ctx, 0x38) = 0;
				const uint32_t ot = a2 + (uint32_t)(S32(ctx, 0x38) >> (a3 & 31)) * 4u;
				const uint16_t add0 = U16(ctx, 0x18);
				const uint16_t add1 = U16(ctx, 0x1A);
				if ((uint16_t)(add0 | add1) == 0)
				{
					x::SSIGPU_InsertPrimAutoDepth(ot, pkt);
					pSXY0 += 0x20;
					pkt += 0x20;
					pSXY1 += 0x20;
					pSXY2 += 0x20;
					pCode += 0x20;
				}
				else
				{
					if (add0 != 0)
					{
						const uint32_t a = add0;
						const uint32_t c2 = U8(pkt, 0x1C) + a;
						const uint32_t c1 = U8(pkt, 0x14) + a;
						const uint32_t c0 = U8(pkt, 0xC) + a;
						if ((int32_t)(c2 | c1 | c0) > 0xFF)
						{
							const uint8_t s = U8(ctx, 0x28);
							U8(pkt, 0xC) = (uint8_t)(c0 - s);
							U8(pkt, 0x14) = (uint8_t)(c1 - s);
							U8(pkt, 0x1C) = (uint8_t)(c2 - s);
						}
						else
						{
							U8(pkt, 0xC) = (uint8_t)c0;
							U8(pkt, 0x14) = (uint8_t)c1;
							U8(pkt, 0x1C) = (uint8_t)c2;
						}
					}
					const uint16_t add1b = U16(ctx, 0x1A);
					if (add1b != 0)
					{
						const uint32_t a = add1b;
						const uint32_t c2 = U8(pkt, 0x1D) + a;
						const uint32_t c1 = U8(pkt, 0x15) + a;
						const uint32_t c0 = U8(pkt, 0xD) + a;
						if ((int32_t)(c2 | c1 | c0) > 0xFF)
						{
							const uint8_t s = U8(ctx, 0x2A);
							U8(pkt, 0xD) = (uint8_t)(c0 - s);
							U8(pkt, 0x15) = (uint8_t)(c1 - s);
							U8(pkt, 0x1D) = (uint8_t)(c2 - s);
						}
						else
						{
							U8(pkt, 0xD) = (uint8_t)c0;
							U8(pkt, 0x15) = (uint8_t)c1;
							U8(pkt, 0x1D) = (uint8_t)c2;
						}
					}
					const uint32_t poly = pkt;
					pSXY1 += 0x2C;
					pSXY2 += 0x2C;
					pCode += 0x2C;
					pSXY0 += 0x2C;
					uint32_t prim = pkt + 0x20;
					pkt += 0x2C;
					U32(prim, 0) = 0x2000000;
					uint32_t tw = 0;
					if (ctx + 0x1C != 0)
						tw = s2_twin(ctx, 0x1C);
					U32(prim, 4) = tw;
					U32(prim, 8) = 0;
					x::SSIGPU_InsertPrimAutoDepth(ot, prim);
					x::SSIGPU_InsertPrimAutoDepth(ot, poly);
					pSXY0 += 0xC;
					pSXY1 += 0xC;
					prim = pkt;
					pkt += 0xC;
					pSXY2 += 0xC;
					pCode += 0xC;
					U32(prim, 0) = 0x2000000;
					tw = 0;
					if (ctx + 0x24 != 0)
						tw = s2_twin(ctx, 0x24);
					U32(prim, 4) = tw;
					U32(prim, 8) = 0;
					x::SSIGPU_InsertPrimAutoDepth(ot, prim);
				}
			}
		next:
			rec += 0x14;
			--count;
		} while (count != 0);
		U32(ctx, 0x2C) = rec;
		return pkt;
	}

	uint32_t __cdecl np_8A1620(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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
		uint32_t pSXY3 = pkt + 0x20;
		uint32_t pSXY2 = pkt + 0x18;
		uint32_t pSXY1 = pkt + 0x10;
		uint32_t pCode = pkt + 4;
		uint32_t pSXY0 = pkt + 8;
		do
		{
			x::GTE_LoadV012(vbase + (uint32_t)U16(rec, 4) * 4u, vbase + (uint32_t)U16(rec, 6) * 4u,
				vbase + (uint32_t)U16(rec, 8) * 4u);
			x::GTE_RTPT();
			{
				const uint32_t flags = U32(ctx, 0x14);
				const uint32_t code = U32(rec, 0);
				U32(pkt, 0) = 0x9000000;
				U32(pCode, 0) = code;
				if (flags & 1)
					U32(pCode, 0) = code | 0x2000000;
				if (flags & 4)
					U32(pCode, 0) = U32(pCode, 0) & 0xFDFFFFFFu;
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
				x::GTE_ReadSXY012_Split(pSXY0, pSXY1, pSXY2);
				x::GTE_LoadV0(vbase + (uint32_t)U16(rec, 0xA) * 4u);
				x::GTE_RTPS();
				int16_t v = S16(pSXY0, 0);
				if (v < 0 || v > 0xA00)
					clip = 1;
				v = S16(pSXY1, 0);
				if (v < 0 || v > 0xA00)
					clip |= 2;
				v = S16(pSXY2, 0);
				if (v < 0 || v > 0xA00)
					clip |= 4;
				v = S16(pkt, 0xA);
				if (v < 0 || v > 0x6C0)
					clip |= 0x10;
				v = S16(pkt, 0x12);
				if (v < 0 || v > 0x6C0)
					clip |= 0x20;
				v = S16(pkt, 0x1A);
				if (v < 0 || v > 0x6C0)
					clip |= 0x40;
				x::GTE_ReadSXY2(pSXY3);
				x::GTE_AVSZ4();
				v = S16(pSXY3, 0);
				if (v < 0 || v > 0xA00)
					clip |= 8;
				v = S16(pkt, 0x22);
				if (v < 0 || v > 0x6C0)
					clip |= 0x80;
				if ((clip & 0xF) == 0xF)
					goto next;
				if ((clip & 0xF0) == 0xF0)
					goto next;
			}
			x::GTE_ReadOTZ(ctx + 0x38);
			if (U8(ctx, 0x14) & 0x40)
			{
				x::set_unk_1CA8A28(pCode);
				x::set_dword_1CA8A30(U32(ctx, 0xC));
				x::sub_45F270();
				x::set_param_with_dword_1CA8A68(pCode);
			}
			{
				const int32_t z = add32(S32(ctx, 0x38), S32(ctx, 0x10));
				S32(ctx, 0x38) = z;
				if (z < 0)
					S32(ctx, 0x38) = 0;
				const uint32_t ot = a2 + (uint32_t)(S32(ctx, 0x38) >> (a3 & 31)) * 4u;
				const uint16_t add0 = U16(ctx, 0x18);
				const uint16_t add1 = U16(ctx, 0x1A);
				if ((uint16_t)(add0 | add1) == 0)
				{
					x::SSIGPU_InsertPrimAutoDepth(ot, pkt);
					pSXY0 += 0x28;
					pSXY1 += 0x28;
					pkt += 0x28;
					pSXY2 += 0x28;
					pSXY3 += 0x28;
					pCode += 0x28;
				}
				else
				{
					if (add0 != 0)
					{
						const uint32_t a = add0;
						const uint32_t c0 = U8(pkt, 0xC) + a;
						const uint32_t c1 = U8(pkt, 0x14) + a;
						const uint32_t c2 = U8(pkt, 0x1C) + a;
						const uint32_t c3 = U8(pkt, 0x24) + a;
						if ((int32_t)(c3 | c2 | c1 | c0) > 0xFF)
						{
							const uint8_t s = U8(ctx, 0x28);
							U8(pkt, 0xC) = (uint8_t)(c0 - s);
							U8(pkt, 0x14) = (uint8_t)(c1 - s);
							U8(pkt, 0x1C) = (uint8_t)(c2 - s);
							U8(pkt, 0x24) = (uint8_t)(c3 - s);
						}
						else
						{
							U8(pkt, 0x14) = (uint8_t)c1;
							U8(pkt, 0xC) = (uint8_t)c0;
							U8(pkt, 0x1C) = (uint8_t)c2;
							U8(pkt, 0x24) = (uint8_t)c3;
						}
					}
					const uint16_t add1b = U16(ctx, 0x1A);
					if (add1b != 0)
					{
						const uint32_t a = add1b;
						const uint32_t c0 = U8(pkt, 0xD) + a;
						const uint32_t c1 = U8(pkt, 0x15) + a;
						const uint32_t c2 = U8(pkt, 0x1D) + a;
						const uint32_t c3 = U8(pkt, 0x25) + a;
						if ((int32_t)(c3 | c2 | c1 | c0) > 0xFF)
						{
							const uint8_t s = U8(ctx, 0x2A);
							U8(pkt, 0xD) = (uint8_t)(c0 - s);
							U8(pkt, 0x15) = (uint8_t)(c1 - s);
							U8(pkt, 0x1D) = (uint8_t)(c2 - s);
							U8(pkt, 0x25) = (uint8_t)(c3 - s);
						}
						else
						{
							U8(pkt, 0x1D) = (uint8_t)c2;
							U8(pkt, 0xD) = (uint8_t)c0;
							U8(pkt, 0x15) = (uint8_t)c1;
							U8(pkt, 0x25) = (uint8_t)c3;
						}
					}
					pSXY0 += 0x34;
					pSXY3 += 0x34;
					pSXY2 += 0x34;
					const uint32_t poly = pkt;
					pCode += 0x34;
					pSXY1 += 0x34;
					uint32_t prim = pkt + 0x28;
					pkt += 0x34;
					U32(prim, 0) = 0x2000000;
					uint32_t tw = 0;
					if (ctx + 0x1C != 0)
						tw = s2_twin(ctx, 0x1C);
					U32(prim, 4) = tw;
					U32(prim, 8) = 0;
					x::SSIGPU_InsertPrimAutoDepth(ot, prim);
					x::SSIGPU_InsertPrimAutoDepth(ot, poly);
					pSXY0 += 0xC;
					pSXY1 += 0xC;
					pSXY2 += 0xC;
					prim = pkt;
					pkt += 0xC;
					pSXY3 += 0xC;
					pCode += 0xC;
					U32(prim, 0) = 0x2000000;
					tw = 0;
					if (ctx + 0x24 != 0)
						tw = s2_twin(ctx, 0x24);
					U32(prim, 4) = tw;
					U32(prim, 8) = 0;
					x::SSIGPU_InsertPrimAutoDepth(ot, prim);
				}
			}
		next:
			rec += 0x18;
			--count;
		} while (count != 0);
		U32(ctx, 0x2C) = rec;
		return pkt;
	}

	uint32_t __cdecl np_8A20D0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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
							x::SSIGPU_InsertPrimAutoDepth(ot, pkt);
							pkt += 0x28;
						}
					}
				}
			}
			rec += 0x1C;
		} while (--count);
		U32(ctx, 0x2C) = rec;
		return pkt;
	}

	uint32_t __cdecl np_8A25D0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		const uint32_t ctx = a1;
		uint32_t pkt = a4;
		uint32_t data = U32(ctx, 0x2C);
		int32_t count = S32(data, 0);
		data += 4;
		const uint32_t vbase = U32(ctx, 4);
		U32(ctx, 0x2C) = data;
		if (count <= 0)
		{
			U32(ctx, 0x2C) = data;
			return pkt;
		}
		do
		{
			pkt = s3_quad(ctx, data, vbase, a2, a3, pkt);
			data += 0x24;
		} while (--count);
		U32(ctx, 0x2C) = data;
		return pkt;
	}

	struct ModPort { game::Id addr; void *port; const char *name; };
	static const ModPort ENGINE_PORTS[] = {
		{ game::MAG_020_NorgPod_Master_State0_Advance, (void *)a_73A0D0, "020 MAG_020_sub_89FE40" },
		{ game::MAG_020_NorgPod_Master_State1_Advance, (void *)a_73A0D0, "020 MAG_020_sub_89FE50" },
		{ game::MAG_020_NorgPod_LightRampUpTask, (void *)a_73A380, "020 MAG_020_sub_89FE90" },
		{ game::MAG_020_NorgPod_LightRampUp_State0_ResetEntityRecords, (void *)a_7335D0, "020 MAG_020_sub_89FEF0" },
		{ game::MAG_020_NorgPod_LightRampUp_State2_Empty, (void *)a_73A6D0, "020 nullsub_1769" },
		{ game::MAG_020_NorgPod_Master_State3_SpawnEmitter, (void *)a_73A170, "020 MAG_020_sub_89FF80" },
		{ game::InitEffectSequenceFromData_c15, (void *)a_7435E0, "020 InitEffectSequenceFromData_c15" },
		{ game::MAG_020_SpriteStrip_DrawQuads, (void *)a_743720, "020 sub_8A0490" },
		{ game::MAG_020_EmitG3, (void *)a_73D770, "020 sub_8A1BA0" },
		{ game::MAG_020_EmitG4, (void *)a_73D9C0, "020 sub_8A1DF0" },
		{ game::MAG_020_NorgPod_Chunk_State2_Empty, (void *)a_73A6D0, "020 nullsub_1770" },
		{ game::MAG_020_StepCounterLimit, (void *)a_743C20, "020 sub_8A2CF0" },
		{ game::MAG_020_NorgPod_Debris_State2_Empty, (void *)a_73A6D0, "020 nullsub_1771" },
		{ game::MAG_020_NorgPod_Lid_State3_Empty, (void *)a_73A6D0, "020 nullsub_1772" },
		{ game::MAG_020_NorgPod_Emitter_State3_Empty, (void *)a_73A6D0, "020 nullsub_1773" },
		{ game::MAG_020_NorgPod_LightRampDownTask, (void *)a_73A380, "020 MAG_020_sub_8A3150" },
		{ game::MAG_020_NorgPod_LightRampDown_State2_Empty, (void *)a_73A6D0, "020 nullsub_1775" },
		{ game::MAG_020_NorgPod_Master_State6_WaitQueuesEmpty, (void *)a_747550, "020 MAG_020_sub_8A3220" },
		{ game::MAG_020_NorgPod_Master_State7_Advance, (void *)a_73A0D0, "020 MAG_020_sub_8A3230" },
		{ game::MAG_020_NorgPod_Master_State8_Advance, (void *)a_73A0D0, "020 MAG_020_sub_8A3240" },
		{ game::MAG_020_NorgPod_Master_State9_Finish, (void *)a_7475A0, "020 MAG_020_sub_8A3250" },
		{ game::MAG_020_NorgPod_Master_State10_Empty, (void *)a_73A6D0, "020 nullsub_1774" },
		{ {}, nullptr, nullptr }
	};
	static const ModPort PORTS[] = {
		{ game::MAG_020_NorgPod_MasterTask, (void *)np_89FD00, "020 np_89FD00" },
		{ game::MAG_020_NorgPod_Master_StartLightRampUp, (void *)np_89FE60, "020 np_89FE60" },
		{ game::MAG_020_NorgPod_LightRampUp_Step, (void *)np_89FF30, "020 np_89FF30" },
		{ game::MAG_020_NorgPod_EmitterTask, (void *)np_89FFB0, "020 np_89FFB0" },
		{ game::MAG_020_NorgPod_Emitter_SpawnLid, (void *)np_8A0040, "020 np_8A0040" },
		{ game::MAG_020_NorgPod_LidTask, (void *)np_8A0070, "020 np_8A0070" },
		{ game::MAG_020_NorgPod_Lid_SpawnDebris, (void *)np_8A00E0, "020 np_8A00E0" },
		{ game::MAG_020_NorgPod_DebrisTask, (void *)np_8A01B0, "020 np_8A01B0" },
		{ game::MAG_020_NorgPod_Debris_Draw, (void *)np_8A0230, "020 np_8A0230" },
		{ game::MAG_020_NorgPod_Debris_SpawnChunks, (void *)np_8A0900, "020 np_8A0900" },
		{ game::MAG_020_NorgPod_ChunkTask, (void *)np_8A0990, "020 np_8A0990" },
		{ game::MAG_020_NorgPod_Chunk_DrawPiece, (void *)np_8A0AA0, "020 np_8A0AA0" },
		{ game::MAG_020_NorgPod_RenderPrimModel, (void *)np_8A0BC0, "020 np_8A0BC0" },
		{ game::MAG_020_NorgPod_FlatTriList, (void *)np_8A0CF0, "020 np_8A0CF0" },
		{ game::MAG_020_NorgPod_FlatQuadList, (void *)np_8A0F10, "020 np_8A0F10" },
		{ game::MAG_020_NorgPod_TexTriList, (void *)np_8A11A0, "020 np_8A11A0" },
		{ game::MAG_020_NorgPod_TexQuadList, (void *)np_8A1620, "020 np_8A1620" },
		{ game::MAG_020_NorgPod_GouraudTexTriList, (void *)np_8A20D0, "020 np_8A20D0" },
		{ game::MAG_020_NorgPod_GouraudTexQuadList, (void *)np_8A25D0, "020 np_8A25D0" },
		{ game::MAG_020_NorgPod_Chunk_Init, (void *)np_8A2C20, "020 np_8A2C20" },
		{ game::MAG_020_NorgPod_Chunk_SpinShrink, (void *)np_8A2C60, "020 np_8A2C60" },
		{ game::MAG_020_NorgPod_Debris_Init, (void *)np_8A2CB0, "020 np_8A2CB0" },
		{ game::MAG_020_NorgPod_Debris_Animate, (void *)np_8A2CD0, "020 np_8A2CD0" },
		{ game::MAG_020_NorgPod_Lid_Init, (void *)np_8A2D30, "020 np_8A2D30" },
		{ game::MAG_020_NorgPod_Lid_Draw, (void *)np_8A2D90, "020 np_8A2D90" },
		{ game::MAG_020_NorgPod_Lid_WaitLaunch, (void *)np_8A2F70, "020 np_8A2F70" },
		{ game::MAG_020_NorgPod_Lid_Flight, (void *)np_8A2FE0, "020 np_8A2FE0" },
		{ game::MAG_020_NorgPod_Emitter_ReleaseMaster, (void *)np_8A3070, "020 np_8A3070" },
		{ game::MAG_020_NorgPod_Emitter_ApplyDamage, (void *)np_8A3090, "020 np_8A3090" },
		{ game::MAG_020_NorgPod_Master_ActionLoop, (void *)np_8A30E0, "020 np_8A30E0" },
		{ game::MAG_020_NorgPod_Master_StartLightRampDown, (void *)np_8A3120, "020 np_8A3120" },
		{ game::MAG_020_NorgPod_LightRampDown_Init, (void *)np_8A31B0, "020 np_8A31B0" },
		{ game::MAG_020_NorgPod_LightRampDown_Step, (void *)np_8A31D0, "020 np_8A31D0" },
		{ {}, nullptr, nullptr }
	};
}
}

	void register_mag020_norg_pod_opening()
	{
		act::register_module(20);
		for (const act::norgpod::ModPort *p = act::norgpod::ENGINE_PORTS; p->addr; p++)
			act::register_module_port(20, p->addr, p->port, p->name);
		for (const act::norgpod::ModPort *p = act::norgpod::PORTS; p->addr; p++)
			act::register_module_port(20, p->addr, p->port, p->name);
	}
}

