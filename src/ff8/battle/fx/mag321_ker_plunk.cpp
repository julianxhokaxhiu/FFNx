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

// Effect 321: Ker Plunk: the targets lifted and spun

#include "mag_common.h"

namespace ff8fx
{
namespace kerplunk321
{
	using namespace eng;
	using namespace magc;

	template<typename T> inline T &AT(uint32_t p, int32_t o) { return *(T *)(p + o); }
	inline int16_t &S16(uint32_t p, int32_t o) { return AT<int16_t>(p, o); }
	inline uint16_t &U16(uint32_t p, int32_t o) { return AT<uint16_t>(p, o); }
	inline int32_t &S32(uint32_t p, int32_t o) { return AT<int32_t>(p, o); }
	inline uint32_t &U32(uint32_t p, int32_t o) { return AT<uint32_t>(p, o); }
	inline uint8_t &U8(uint32_t p, int32_t o) { return AT<uint8_t>(p, o); }
	inline uint32_t P(const void *p) { return (uint32_t)p; }

	inline TaskQueue *RootQueue() { return (TaskQueue *)game_addr(game::MAG_321_KerPlunk_RootQueue); }
	inline TaskQueue *EffectQueue() { return (TaskQueue *)game_addr(game::MAG_321_KerPlunk_EffectQueue); }
	inline uint32_t &Ctx() { return var<uint32_t>(game_addr(game::MAG_321_KerPlunk_CastContext)); }
	inline int32_t &CasterSlot() { return var<int32_t>(game_addr(game::MAG_321_KerPlunk_CasterSlot)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_321_KerPlunk_PacketCursor)); }
	inline uint32_t &VoiceSlot() { return var<uint32_t>(game_addr(game::MAG_321_KerPlunk_VoiceSlot)); }
	inline uint32_t Saved(int slot) { return game_addr(game::unk_221D420) + (uint32_t)slot * 0x24; }
	static constexpr game::Id PARTS = game::unk_221BCA0, PARTS_END = game::MAG_321_KerPlunk_EffectQueue;
	static constexpr game::Id ARENA_A = game::unk_221D528, ARENA_B = game::unk_222B528;

	static constexpr game::Id ORIG_Root = game::MAG_321_KerPlunk_Root, ORIG_TexRestore = game::MAG_321_KerPlunk_TexRestoreTask, ORIG_Director = game::MAG_321_KerPlunk_Director, ORIG_Ring = game::MAG_321_KerPlunk_Rings, ORIG_Dust = game::MAG_321_KerPlunk_Dust, ORIG_Debris = game::MAG_321_KerPlunk_Debris, ORIG_Flash = game::MAG_321_KerPlunk_Flash, ORIG_Lift = game::MAG_321_KerPlunk_Lift, ORIG_Burst = game::MAG_321_KerPlunk_Burst, ORIG_TargetRing = game::MAG_321_KerPlunk_TargetRing;
	static constexpr game::Id MODEL_Ring = game::MAG_321_KerPlunk_ModelRing, MODEL_TargetRing = game::unk_CD1D5C, MODEL_Debris = game::unk_CD1C24, SEQ_Dust = game::unk_CD1390;
	static constexpr game::Id SOUND_Loop = game::MAG_321_KerPlunk_SoundLoop, SOUND_Start = game::unk_CD2AF0, SOUND_End = game::unk_CD2AF4;

	inline uint32_t RenderOT44() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44; }

	inline void ScaleMatrix(Mat4x3 *m, const int32_t *v) { fn<void (__cdecl *)(Mat4x3 *, const int32_t *)>(game_addr(game::scale3DMatrix))(m, v); }
	inline void MatVec(const Mat4x3 *m, const int16_t *in, int16_t *out) { fn<void (__cdecl *)(const Mat4x3 *, const int16_t *, int16_t *)>(game_addr(game::matrixMultiplyVector))(m, in, out); }
	inline void CentrePosition(uint32_t mask, int16_t *out) { fn<void (__cdecl *)(uint32_t, int16_t *)>(game_addr(game::CalculateCenterPosition))(mask, out); }
	inline int32_t ComputeCos(int32_t a) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(a); }
	inline void ScreenFeedback(int32_t a, int32_t r, int32_t g, int32_t b, int32_t n) { fn<void (__cdecl *)(int32_t, int32_t, int32_t, int32_t, int32_t)>(game_addr(game::Battle_RequestScreenFeedback))(a, r, g, b, n); }
	inline int32_t VoiceActive(uint32_t v) { return fn<int32_t (__cdecl *)(uint32_t)>(game_addr(game::BdSound_IsVoiceSlotActive))(v); }
	inline void VoiceRelease(uint32_t v) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::BdSound_ReleaseVoiceSlot))(v); }
	inline void GteSetLightMatrix(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_SetLightMatrix))(m); }
	inline void GteMVMVA_LightV0Bk() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_LightV0_Bk))(); }
	inline void GteSetBKFromMatrix(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_SetBackColorFromTrans))(m); }
	inline void GteSetRotScale(int32_t s) { fn<void (__cdecl *)(int32_t)>(game_addr(game::GTE_SetRotScale))(s); }
	inline void GteSetTransFromVec32(const int32_t *v) { fn<void (__cdecl *)(const int32_t *)>(game_addr(game::GTE_SetTransFromVec32))(v); }

	enum { P_MASK = 0, P_FRAME = 4, P_SIZE = 6, P_POS = 8 };
}
}

namespace ff8fx
{
namespace kerplunk321
{
	static uint32_t NewTask(uint32_t f)
	{
		const uint32_t t = P(AddTaskToQueue(EffectQueue(), f));
		S16(t, 0xC) = 0;
		return t;
	}

	static uint32_t FreeParticle()
	{
		for (uint32_t p = game_addr(PARTS); p < game_addr(PARTS_END); p += 0x18)
			if (!U32(p, P_MASK)) return p;
		return 0;
	}

	static void RingDraw(uint32_t n, uint32_t model, const int32_t *sc, int32_t from, int32_t fade)
	{
		const int16_t ang[3] = { 0, 0, 0 };
		Mat4x3 m;
		ComposeZYXRotationMatrix(ang, &m);
		m.t[1] = S16(n, 0x12);
		m.t[0] = S16(n, 0x10);
		m.t[2] = S16(n, 0x14);
		ScaleMatrix(&m, sc);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		const uint32_t h = P(FieldAlloc(0x58));
		U32(h, 0) = model;
		U32(h, 8) = 0;
		U32(h, 0x1C) = 0x33;
		if (S16(n, 0xC) >= from)
		{
			U32(h, 0x1C) = 0xF3;
			S32(h, 0xC) = fade;
		}
		PacketCursor() = RenderPrimModel((void *)h, RenderOT44(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static int32_t RingFade(int32_t c, int32_t from) { return shl32(c - from, 9); }

	static void PointRingDraw(uint32_t n, int32_t fade)
	{
		const int32_t s = S16(n, 0x1C);
		const int32_t sc[3] = { s, s, s };
		RingDraw(n, game_addr(MODEL_Ring), sc, 0xC, fade);
	}

	static void TargetRingDraw(uint32_t n, int32_t fade)
	{
		const int32_t sc[3] = { S16(n, 0x1C), S16(n, 0x20), S16(n, 0x1C) };
		RingDraw(n, game_addr(MODEL_TargetRing), sc, 6, fade);
	}

	static uint32_t __cdecl RingTask(TaskNode *tn)
	{
		const uint32_t n = P(tn);
		if (S16(n, 0xE) > 0)
		{
			if (DrawOnly()) return 0;
			S16(n, 0xE) = (int16_t)(S16(n, 0xE) - 1);
			return 0;
		}
		PointRingDraw(n, RingFade(S16(n, 0xC), 0xC));
		if (DrawOnly()) return 0;
		const int16_t d = S16(n, 0x1E);
		S16(n, 0x1C) = (int16_t)(S16(n, 0x1C) + d);
		S16(n, 0x1E) = (int16_t)(d - d / 8);
		S16(n, 0xC) = (int16_t)(S16(n, 0xC) + 1);
		return S16(n, 0xC) >= 0x14 ? TASK_END : 0;
	}

	static uint32_t __cdecl TargetRingTask(TaskNode *tn)
	{
		const uint32_t n = P(tn);
		if (S16(n, 0xE) > 0)
		{
			if (DrawOnly()) return 0;
			S16(n, 0xE) = (int16_t)(S16(n, 0xE) - 1);
			return 0;
		}
		TargetRingDraw(n, RingFade(S16(n, 0xC), 6));
		if (DrawOnly()) return 0;
		const int16_t d = S16(n, 0x22);
		S16(n, 0x20) = (int16_t)(S16(n, 0x20) + d);
		S16(n, 0x22) = (int16_t)(d - d / 10);
		S16(n, 0xC) = (int16_t)(S16(n, 0xC) + 1);
		return S16(n, 0xC) >= 0xE ? TASK_END : 0;
	}

	static void SeqSetup(uint32_t h, uint32_t w)
	{
		U16(h, 0x24) = 0;
		ff8fx::copy<(0x20)>((void *)(w + 8), (const void *)&Camera());
		U32(h, 0) = game_addr(SEQ_Dust);
		GteSetLightMatrix((const void *)(w + 8));
		GteSetBKFromMatrix((const void *)(w + 8));
	}

	static void SeqDraw(uint32_t h, uint32_t w, uint32_t p)
	{
		GteLoadV0((const void *)(p + P_POS));
		GteMVMVA_LightV0Bk();
		GteSetRotScale(S16(p, P_SIZE));
		U16(h, 4) = U16(p, P_FRAME);
		GteReadMAC123((int32_t *)(w + 0x38));
		NormalizeVector((const int32_t *)(w + 0x38), (int32_t *)(w + 0x28));
		const int32_t k = -(S16(p, P_SIZE) >> 3);
		S32(w, 0x38) += mul32(S32(w, 0x28), k) >> 12;
		S32(w, 0x3C) += mul32(S32(w, 0x2C), k) >> 12;
		S32(w, 0x40) += mul32(S32(w, 0x30), k) >> 12;
		GteSetTransFromVec32((const int32_t *)(w + 0x38));
		PacketCursor() = InitEffectSequenceFromData((void *)h, RenderOT44(), 2, PacketCursor());
	}

	static uint32_t __cdecl DustTask(TaskNode *tn)
	{
		const uint32_t n = P(tn);
		const uint32_t w = P(FieldAlloc(0x48));
		if (!DrawOnly() && S16(n, 0xC) <= 0xA)
		{
			U32(w, 0) = U32(n, 0x10);
			U32(w, 4) = U32(n, 0x14);
			for (int k = 0; k < 6; k++)
			{
				const uint32_t p = FreeParticle();
				if (!p) break;
				U32(p, P_MASK) = 1;
				S16(p, P_FRAME) = 0;
				S16(p, P_SIZE) = (int16_t)(CrtRand() % 0xA00 + 0x900);
				U32(p, P_POS) = U32(w, 0);
				U32(p, P_POS + 4) = U32(w, 4);
				S16(p, P_POS) = (int16_t)(S16(p, P_POS) + CrtRand() % 0x1130 - 0x898);
				S16(p, P_POS + 2) = (int16_t)(S16(p, P_POS + 2) - CrtRand() % 0x12C);
				S16(p, P_POS + 4) = (int16_t)(S16(p, P_POS + 4) + CrtRand() % 0x1130 - 0x898);
				S16(p, 0x12) = (int16_t)(-5 - CrtRand() % 0x2D);
			}
		}
		const uint32_t h = P(FieldAlloc(0xB4));
		int alive = 0;
		SeqSetup(h, w);
		for (uint32_t p = game_addr(PARTS); p < game_addr(PARTS_END); p += 0x18)
		{
			if (!(U8(p, P_MASK) & 1)) continue;
			SeqDraw(h, w, p);
			if (DrawOnly()) continue;
			if (S16(h, 0x28) < 0)
			{
				U32(p, P_MASK) = 0;
				continue;
			}
			S16(p, P_FRAME) = (int16_t)(S16(p, P_FRAME) + 1);
			S16(p, P_POS + 2) = (int16_t)(S16(p, P_POS + 2) + S16(p, 0x12));
			S16(p, 0x10) = (int16_t)(S16(p, 0x10) - (int16_t)(S16(p, 0x10) >> 4));
			alive++;
		}
		FieldFree(0xB4);
		FieldFree(0x48);
		if (DrawOnly()) return 0;
		S16(n, 0xC) = (int16_t)(S16(n, 0xC) + 1);
		return alive ? 0 : TASK_END;
	}

	static uint32_t __cdecl BurstTask(TaskNode *tn)
	{
		const uint32_t n = P(tn);
		const uint32_t w = P(FieldAlloc(0x48));
		if (!DrawOnly() && S16(n, 0xC) <= 3)
		{
			U32(w, 0) = U32(n, 0x10);
			U32(w, 4) = U32(n, 0x14);
			for (int k = 0; k < 8; k++)
			{
				const uint32_t p = FreeParticle();
				if (!p) break;
				U32(p, P_MASK) = (uint32_t)(int32_t)S16(n, 0xE);
				S16(p, P_FRAME) = 0;
				S16(p, P_SIZE) = (int16_t)(CrtRand() % 0x400 + 0x600);
				U32(p, P_POS) = U32(w, 0);
				U32(p, P_POS + 4) = U32(w, 4);
				const int32_t a = CrtRand() % 0x1000;
				const int32_t sp = CrtRand() % 0x5A + 0x46;
				S16(p, 0x10) = (int16_t)(mul32(ComputeSin(a), sp) >> 12);
				S16(p, 0x12) = (int16_t)-(CrtRand() % 0x78);
				S16(p, 0x14) = (int16_t)(mul32(ComputeCos(a), sp) >> 12);
			}
		}
		const uint32_t h = P(FieldAlloc(0xB4));
		int alive = 0;
		SeqSetup(h, w);
		const uint32_t mask = (uint32_t)(int32_t)S16(n, 0xE);
		for (uint32_t p = game_addr(PARTS); p < game_addr(PARTS_END); p += 0x18)
		{
			if (!(mask & U32(p, P_MASK))) continue;
			SeqDraw(h, w, p);
			if (DrawOnly()) continue;
			if (S16(h, 0x28) < 0)
			{
				U32(p, P_MASK) = 0;
				continue;
			}
			S16(p, P_FRAME) = (int16_t)(S16(p, P_FRAME) + 1);
			S16(p, P_POS) = (int16_t)(S16(p, P_POS) + S16(p, 0x10));
			S16(p, P_POS + 4) = (int16_t)(S16(p, P_POS + 4) + S16(p, 0x14));
			S16(p, 0x10) = (int16_t)(S16(p, 0x10) - (int16_t)(S16(p, 0x10) >> 3));
			S16(p, 0x14) = (int16_t)(S16(p, 0x14) - (int16_t)(S16(p, 0x14) >> 3));
			alive++;
		}
		FieldFree(0xB4);
		FieldFree(0x48);
		if (DrawOnly()) return 0;
		S16(n, 0xC) = (int16_t)(S16(n, 0xC) + 1);
		return alive ? 0 : TASK_END;
	}

	static int32_t DebrisScale(int32_t s, int32_t f) { return f >= 8 ? s + mul32(f - 8, -(s / 8)) : s; }

	static void DebrisDraw(uint32_t h, uint32_t w, uint32_t p, int32_t s)
	{
		U16(w, 0) = U16(p, 0x10);
		U16(w, 4) = U16(p, 0x14);
		ComposeZYXRotationMatrix((const int16_t *)w, (Mat4x3 *)(w + 0x28));
		S32(w, 0x44) = S16(p, P_POS + 4);
		S32(w, 0x3C) = S16(p, P_POS);
		S32(w, 0x40) = S16(p, P_POS + 2);
		S32(w, 0x50) = s;
		S32(w, 0x4C) = s;
		S32(w, 0x48) = s;
		ScaleMatrix((Mat4x3 *)(w + 0x28), (const int32_t *)(w + 0x48));
		ComposeAffineTransform((const Mat4x3 *)(w + 8), (const Mat4x3 *)(w + 0x28), (Mat4x3 *)(w + 0x28));
		GteSetRotMatrix((const Mat4x3 *)(w + 0x28));
		GteSetTransVector((const Mat4x3 *)(w + 0x28));
		PacketCursor() = RenderPrimModel((void *)h, RenderOT44(), 2, PacketCursor());
	}

	static void DebrisSetup(uint32_t h, uint32_t w)
	{
		ff8fx::copy<(0x20)>((void *)(w + 8), (const void *)&Camera());
		U32(h, 0) = game_addr(MODEL_Debris);
		U32(h, 0x1C) = 0x30;
		U16(w, 2) = 0;
	}

	static uint32_t __cdecl DebrisTask(TaskNode *tn)
	{
		const uint32_t n = P(tn);
		const uint32_t w = P(FieldAlloc(0x68));
		if (!DrawOnly() && S16(n, 0xC) <= 5)
		{
			U32(w, 0) = U32(n, 0x10);
			U32(w, 4) = U32(n, 0x14);
			for (int k = 0; k < 5; k++)
			{
				const uint32_t p = FreeParticle();
				if (!p) break;
				U32(p, P_MASK) = 2;
				S16(p, P_FRAME) = 0;
				S16(p, P_SIZE) = (int16_t)(CrtRand() % 0x500 + 0x300);
				U32(p, P_POS) = U32(w, 0);
				U32(p, P_POS + 4) = U32(w, 4);
				S16(p, P_POS) = (int16_t)(S16(p, P_POS) + CrtRand() % 0xED8 - 0x76C);
				S16(p, P_POS + 2) = (int16_t)(S16(p, P_POS + 2) - CrtRand() % 0xFA);
				S16(p, P_POS + 4) = (int16_t)(S16(p, P_POS + 4) + CrtRand() % 0xED8 - 0x76C);
				S16(p, 0x10) = (int16_t)(CrtRand() % 0x1000);
				S16(p, 0x12) = (int16_t)(-0x6E - CrtRand() % 0xF0);
				S16(p, 0x14) = (int16_t)(S16(p, 0x10) >> 1);
				int16_t v = (int16_t)(CrtRand() % 0x46 + 0x1E);
				S16(p, 0xE) = v;
				if (U8(p, 0x10) & 1) S16(p, 0xE) = (int16_t)-v;
				v = (int16_t)(CrtRand() % 0x28 + 0xF);
				S16(p, 0x16) = v;
				if (U8(p, 0x14) & 1) S16(p, 0x16) = (int16_t)-v;
			}
		}
		const uint32_t h = P(FieldAlloc(0x58));
		int alive = 0;
		DebrisSetup(h, w);
		for (uint32_t p = game_addr(PARTS); p < game_addr(PARTS_END); p += 0x18)
		{
			if (!(U8(p, P_MASK) & 2)) continue;
			const int16_t f = S16(p, P_FRAME);
			DebrisDraw(h, w, p, DebrisScale(S16(p, P_SIZE), f));
			if (DrawOnly()) continue;
			if (f >= 0x10)
			{
				U32(p, P_MASK) = 0;
				continue;
			}
			S16(p, P_FRAME) = (int16_t)(f + 1);
			S16(p, P_POS + 2) = (int16_t)(S16(p, P_POS + 2) + S16(p, 0x12));
			S16(p, 0x12) = (int16_t)(S16(p, 0x12) - (int16_t)(S16(p, 0x12) >> 3));
			S16(p, 0x10) = (int16_t)(S16(p, 0x10) + S16(p, 0xE));
			S16(p, 0x14) = (int16_t)(S16(p, 0x14) + S16(p, 0x16));
			alive++;
		}
		FieldFree(0x58);
		FieldFree(0x68);
		if (DrawOnly()) return 0;
		S16(n, 0xC) = (int16_t)(S16(n, 0xC) + 1);
		return alive ? 0 : TASK_END;
	}

	static uint32_t __cdecl FlashTask(TaskNode *tn)
	{
		ScreenFeedback(0, 0xFF, 0xFF, 0xFF, 0x3F);
		if (DrawOnly()) return 0;
		const uint32_t n = P(tn);
		S16(n, 0xC) = (int16_t)(S16(n, 0xC) + 1);
		return S16(n, 0xC) >= 0x14 ? TASK_END : 0;
	}

	static uint32_t __cdecl LiftTask(TaskNode *tn)
	{
		const uint32_t n = P(tn);
		const int16_t c = S16(n, 0xC);
		const int slot = S16(n, 0xE);
		const uint32_t e = P(Entity(slot)), s = Saved(slot);
		if (c == 0)
		{
			U16(s, 0) = (uint16_t)(U16(e, 0) & 0x1020);
			U32(s, 0xC) = U32(e, 0x1C);
			U32(s, 0x10) = U32(e, 0x20);
			U32(s, 4) = U32(e, 0xC);
			U32(s, 8) = U32(e, 0x10);
		}
		if (c < 0x42)
		{
			U16(e, 0) |= 0x1020;
			S16(e, 0xC) = (int16_t)(mul32(S16(n, 0x18), c) / 12);
			S16(e, 0xE) = (int16_t)(shl32(c, 13) / 12);
			int16_t d[3] = { (int16_t)(S16(n, 0x10) - S16(s, 0xC)), (int16_t)(S16(n, 0x12) - S16(s, 0xE)), (int16_t)(S16(n, 0x14) - S16(s, 0x10)) };
			Mat4x3 m;
			ComposeZYXRotationMatrix((const int16_t *)(e + 0xC), &m);
			MatVec(&m, d, d);
			S16(e, 0x1C) = (int16_t)(S16(n, 0x10) - d[0]);
			S16(e, 0x1E) = (int16_t)(S16(n, 0x12) - d[1]);
			S16(e, 0x20) = (int16_t)(S16(n, 0x14) - d[2]);
		}
		if (c < 0xC) S16(e, 0x1E) = (int16_t)(S16(e, 0x1E) + (mul32(ComputeSin(shl32(c, 10) / 12), S16(n, 0x20)) >> 12));
		else if (c < 0x36) S16(e, 0x1E) = (int16_t)(S16(e, 0x1E) + S16(n, 0x20));
		else if (c < 0x42) S16(e, 0x1E) = (int16_t)(S16(e, 0x1E) + (mul32(ComputeSin(shl32(c - 0x36, 10) / 12 + 0x400), S16(n, 0x20)) >> 12));
		else
		{
			U32(e, 0x1C) = U32(s, 0xC);
			U32(e, 0x20) = U32(s, 0x10);
			U32(e, 0xC) = U32(s, 4);
			U32(e, 0x10) = U32(s, 8);
		}
		if (DrawOnly()) return 0;
		if (S16(n, 0xC) == 0x42)
		{
			const uint32_t t = NewTask(game_addr(ORIG_Burst));
			S16(t, 0x12) = 0;
			S16(t, 0x10) = S16(n, 0x10);
			S16(t, 0x14) = S16(n, 0x14);
			S16(t, 0xE) = (int16_t)(4 << (U8(n, 0x1A) & 31));
		}
		S16(n, 0xC) = (int16_t)(S16(n, 0xC) + 1);
		if (S16(n, 0xC) < 0x44) return 0;
		U16(e, 0) = (uint16_t)((U16(e, 0) & 0xEFDF) | U16(s, 0));
		ApplyActionResultToTarget((uint8_t *)(U32(U32(Ctx(), 4), 8) + (uint32_t)(S16(n, 0x1A) * 0x18)));
		return TASK_END;
	}

	static uint32_t __cdecl DirectorTask(TaskNode *tn)
	{
		if (DrawOnly()) return 0;
		const uint32_t n = P(tn);
		uint8_t *caster = Entity(CasterSlot());
		if (S16(n, 0xC) == 0)
		{
			GetDefaultEffectPosition(caster, (int16_t *)(n + 0x10));
			const int32_t sn = ComputeSin(*(const int16_t *)(caster + 0xE));
			S16(n, 0x12) = 0;
			S16(n, 0x10) = (int16_t)(S16(n, 0x10) + (mul32(sn, -2800) >> 12));
			const int32_t cs = ComputeCos(*(const int16_t *)(caster + 0xE));
			S16(n, 0x14) = (int16_t)(S16(n, 0x14) + (mul32(cs, -2800) >> 12));
			VoiceSlot() = ClaimVoiceSlot((const void *)game_addr(SOUND_Loop), 1, 0x80);
		}
		if (S16(n, 0xC) == 0x18)
		{
			for (int i = 0; i < 3; i++)
			{
				const uint32_t t = P(AddTaskToQueue(EffectQueue(), game_addr(ORIG_Ring)));
				S16(t, 0xE) = (int16_t)(3 * i);
				U32(t, 0x10) = U32(n, 0x10);
				S16(t, 0xC) = 0;
				U32(t, 0x14) = U32(n, 0x14);
				S16(t, 0x1E) = S16(t, 0x1C) = 0x5CC;
			}
			uint32_t t = P(AddTaskToQueue(EffectQueue(), game_addr(ORIG_Dust)));
			U32(t, 0x10) = U32(n, 0x10);
			S16(t, 0xC) = 0;
			U32(t, 0x14) = U32(n, 0x14);
			t = P(AddTaskToQueue(EffectQueue(), game_addr(ORIG_Debris)));
			U32(t, 0x10) = U32(n, 0x10);
			S16(t, 0xC) = 0;
			U32(t, 0x14) = U32(n, 0x14);
			NewTask(game_addr(ORIG_Flash));
			const uint32_t tg = U32(Ctx(), 4);
			for (int k = 0; k < U8(tg, 0x10); k++)
			{
				const int slot = *(const uint8_t *)(U32(tg, 8) + (uint32_t)k * 0x18);
				const uint32_t l = NewTask(game_addr(ORIG_Lift));
				S16(l, 0xE) = (int16_t)slot;
				GetDefaultEffectPosition(Entity(slot), (int16_t *)(l + 0x10));
				int32_t v = shl32(CrtRand() % 3 + 1, 12);
				S16(l, 0x18) = (int16_t)v;
				if (v & 1) S16(l, 0x18) = (int16_t)-v;
				S16(l, 0x1A) = (int16_t)k;
				S16(l, 0x20) = (int16_t)(-0x1770 - CrtRand() % 0x9C4);
				const uint32_t r = NewTask(game_addr(ORIG_TargetRing));
				S16(r, 0xE) = 1;
				S16(r, 0x10) = S16(l, 0x10);
				S16(r, 0x12) = 0;
				S16(r, 0x14) = S16(l, 0x14);
				S16(r, 0x18) = (int16_t)(CrtRand() % 0x1000);
				S16(r, 0x1C) = (int16_t)(CrtRand() % 0x280 + 0x900);
				S16(r, 0x20) = S16(r, 0x22) = (int16_t)((CrtRand() % 0x800 + 0x1800) / 8);
			}
		}
		if (S16(n, 0xC) == 1) BdPlaySE((const void *)game_addr(SOUND_Start), 0, 0x80);
		if (S16(n, 0xC) == 0x5C)
		{
			int16_t at[4];
			CentrePosition(U16(Ctx(), 2), at);
			BdPlaySE3D((const void *)game_addr(SOUND_End), 0, at);
		}
		if (S16(n, 0xC) == 0x61 && VoiceActive(VoiceSlot())) VoiceRelease(VoiceSlot());
		S16(n, 0xC) = (int16_t)(S16(n, 0xC) + 1);
		return S16(n, 0xC) > 0x64 ? TASK_END : 0;
	}

	static uint32_t __cdecl RootTask(TaskNode *tn)
	{
		const uint32_t n = P(tn);
		PacketCursor() = (U8(n, 0xC) & 1) ? game_addr(ARENA_B) : game_addr(ARENA_A);
		const int running = ExecuteTaskQueue(EffectQueue());
		if (!running)
		{
			const uint32_t w = P(AddTaskToQueue(RootQueue(), game_addr(ORIG_TexRestore)));
			S16(w, 0xC) = 0;
			S16(w, 0xE) = 0;
		}
		S16(n, 0xC) = (int16_t)(S16(n, 0xC) + 1);
		return running ? 0 : TASK_END;
	}

	static uint32_t __cdecl TexRestoreTask(TaskNode *tn)
	{
		const uint32_t n = P(tn);
		if (S16(n, 0xC) == 1) TextureRestoreTask((void *)game_addr(ARENA_A), (void *)(n + 0xE));
		const int16_t done = S16(n, 0xE);
		S16(n, 0xC) = (int16_t)(S16(n, 0xC) + 1);
		return done ? TASK_END : 0;
	}

	void register_mag321_ker_plunk_ports()
	{
		register_port(ORIG_Root, (void *)RootTask, "321 RootTask", 321);
		register_port(ORIG_TexRestore, (void *)TexRestoreTask, "321 TexRestoreTask", 321);
		register_port(ORIG_Director, (void *)DirectorTask, "321 DirectorTask", 321);
		register_port(ORIG_Ring, (void *)RingTask, "321 RingTask", 321);
		register_port(ORIG_Dust, (void *)DustTask, "321 DustTask", 321);
		register_port(ORIG_Debris, (void *)DebrisTask, "321 DebrisTask", 321);
		register_port(ORIG_Flash, (void *)FlashTask, "321 FlashTask", 321);
		register_port(ORIG_Lift, (void *)LiftTask, "321 LiftTask", 321);
		register_port(ORIG_Burst, (void *)BurstTask, "321 BurstTask", 321);
		register_port(ORIG_TargetRing, (void *)TargetRingTask, "321 TargetRingTask", 321);
	}
}

	void register_mag321_ker_plunk() { kerplunk321::register_mag321_ker_plunk_ports(); }
}

