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

// Effect 186: Blaster

#include "mag_common.h"

namespace ff8fx
{
namespace blaster186
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

	inline TaskQueue *RootQueue() { return (TaskQueue *)game_addr(game::MAG_186_Blaster_RootQueue); }
	inline TaskQueue *EffectQueue() { return (TaskQueue *)game_addr(game::MAG_186_Blaster_EffectQueue); }
	inline uint32_t &Ctx() { return var<uint32_t>(game_addr(game::MAG_186_Blaster_CastContext)); }
	inline int32_t &CasterSlot() { return var<int32_t>(game_addr(game::MAG_186_Blaster_CasterSlot)); }
	inline int32_t &TargetSlot() { return var<int32_t>(game_addr(game::MAG_186_Blaster_TargetSlot)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::MAG_186_Blaster_MagicBufferBase)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_186_Blaster_PacketCursor)); }
	inline uint32_t &FrameCursor() { return var<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)); }
	inline uint8_t *Caster() { return Entity(CasterSlot()); }
	inline uint8_t *Target() { return Entity(TargetSlot()); }
	inline Mat4x3 *Aim() { return (Mat4x3 *)game_addr(game::MAG_186_Blaster_Aim); }
	static constexpr game::Id PARTS = game::unk_22B7638, PARTS_END = game::unk_22B7F98;
	static constexpr game::Id MOUTH = game::unk_22BC108;
	static constexpr game::Id AIMV = game::unk_22BC110;
	static constexpr game::Id TGT = game::unk_22BC0F0;
	static constexpr game::Id BEAM = game::unk_22B7F98;
	static constexpr game::Id BEAMV = game::unk_22B7FA0;

	static constexpr game::Id ORIG_Root = game::MAG_186_Blaster_Root, ORIG_TexRestore = game::MAG_186_Blaster_TexRestoreTask, ORIG_Director = game::MAG_186_Blaster_Director, ORIG_Fade = game::MAG_186_Blaster_ScreenFade, ORIG_Charge = game::MAG_186_Blaster_Charge, ORIG_Beam = game::MAG_186_Blaster_Beam, ORIG_BeamRing = game::MAG_186_Blaster_BeamRing, ORIG_Bolts = game::MAG_186_Blaster_Bolts, ORIG_Bolt = game::MAG_186_Blaster_Bolt, ORIG_Sparks = game::MAG_186_Blaster_Sparks, ORIG_Glow = game::MAG_186_Blaster_Glow, ORIG_Wave = game::MAG_186_Blaster_ShockWave, ORIG_Splashes = game::MAG_186_Blaster_Splashes;
	static constexpr game::Id SEQ_Beam = game::MAG_186_Blaster_SeqBeam, SEQ_Spark = game::unk_D1DF7C, SEQ_Glow = game::unk_D1DDD0, SEQ_Splash = game::unk_D1E128;
	static constexpr game::Id MODEL_BeamRing = game::MAG_186_Blaster_ModelBeamRing, MODEL_Bolt = game::unk_D1EF4C, MODEL_Wave = game::unk_D1F754;
	static constexpr game::Id SOUND_Charge = game::MAG_186_Blaster_SoundCharge, AIM_FROM = game::unk_D20800, BOUNDS = game::unk_D2087C;
	static constexpr game::Id BOLT_MESHES = game::unk_D20810, WAVE_TEXTURES = game::unk_D20860;

	inline uint32_t RenderOT44() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44; }

	inline int32_t Normalize(const int32_t *in, int32_t *out) { return fn<int32_t (__cdecl *)(const int32_t *, int32_t *)>(game_addr(game::NormalizeVectorToFixedPoint))(in, out); }
	inline int32_t Sqrt(int32_t v) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::Math_SqrtInt))(v); }
	inline void ScaleMatrix(Mat4x3 *m, const int32_t *v) { fn<void (__cdecl *)(Mat4x3 *, const int32_t *)>(game_addr(game::scale3DMatrix))(m, v); }
	inline void GteMatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline int32_t SpawnPosition(uint8_t *entity, int32_t point, int32_t flag, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, point, flag, out); }
	inline void ScreenFadeColour(int32_t r, int32_t g, int32_t b) { fn<void (__cdecl *)(int32_t, int32_t, int32_t)>(game_addr(game::Effect_ScreenFadeColour))(r, g, b); }
	inline void GteSetLightMatrix(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_SetLightMatrix))(m); }
	inline void GteMVMVA_LightV0Bk() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_LightV0_Bk))(); }
	inline void GteSetBKFromMatrix(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_SetBackColorFromTrans))(m); }
	inline void GteSetRotScale(int32_t s) { fn<void (__cdecl *)(int32_t)>(game_addr(game::GTE_SetRotScale))(s); }
	inline void GteSetTransFromVec32(const int32_t *v) { fn<void (__cdecl *)(const int32_t *)>(game_addr(game::GTE_SetTransFromVec32))(v); }

	enum { P_MASK = 0, P_FRAME = 4, P_SIZE = 6, P_POS = 8, P_VEL = 0x10 };
}
}

namespace ff8fx
{
namespace blaster186
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

	static void MorphMesh(uint32_t a, uint32_t b, uint32_t out, int32_t t)
	{
		for (int i = 0; i < 0x66; i++, a += 8, b += 8, out += 8)
			for (int j = 0; j < 3; j++)
				S16(out, 2 * j) = (int16_t)((mul32(S16(b, 2 * j) - S16(a, 2 * j), t) >> 12) + S16(a, 2 * j));
	}

	static void BeamDraw(uint32_t n)
	{
		const int16_t ang[3] = { 0, 0, 0 };
		Mat4x3 m, r;
		ComposeZYXRotationMatrix(ang, &m);
		m.t[0] = S16(game_addr(BEAM), 0);
		m.t[1] = S16(game_addr(BEAM), 2);
		m.t[2] = S16(game_addr(BEAM), 4);
		const int32_t sc[3] = { mul32(S16(n, 0x1C), S16(game_addr(BEAM), 6)) / 820, S16(n, 0x20), 0 };
		ScaleMatrix(&m, sc);
		const int32_t from[3] = { -0x1000, 0, 0 };
		int32_t axis[3];
		const int32_t a = RotationBetweenVectors(from, (const int32_t *)game_addr(BEAMV), axis);
		BuildAxisAngleRotationMatrix(a, &r, axis);
		GteMatrixMultiply(&r, &m);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		const uint32_t h = P(FieldAlloc(0xB4));
		U16(h, 4) = (uint16_t)(U8(n, 0xC) & 3);
		U32(h, 0) = game_addr(SEQ_Beam);
		U16(h, 0x24) = 8;
		PacketCursor() = InitEffectSequenceFromData((void *)h, RenderOT44(), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static uint32_t __cdecl BeamTask(TaskNode *tn)
	{
		const uint32_t n = P(tn);
		BeamDraw(n);
		if (DrawOnly()) return 0;
		S16(n, 0xC) = (int16_t)(S16(n, 0xC) + 1);
		return S16(n, 0xC) >= S16(n, 0xE) ? TASK_END : 0;
	}

	static int32_t RingFade(uint32_t n, int32_t c) { return (0x1000 / S16(n, 0x22)) * (c - S16(n, 0x20)); }

	static void BeamRingDraw(uint32_t n, int32_t fade)
	{
		const int16_t r = S16(n, 0x1C);
		TransformCameraByShadowRotation((const void *)game_addr(BEAM), (int32_t)(uint16_t)r, -(r >> 3));
		const uint32_t h = P(FieldAlloc(0x58));
		U32(h, 0) = game_addr(MODEL_BeamRing);
		U32(h, 8) = 0;
		U32(h, 0x1C) = 0x33;
		if (S16(n, 0xC) >= S16(n, 0x20))
		{
			U32(h, 0x1C) = 0xF3;
			S32(h, 0xC) = fade;
		}
		PacketCursor() = RenderPrimModel((void *)h, RenderOT44(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static uint32_t __cdecl BeamRingTask(TaskNode *tn)
	{
		const uint32_t n = P(tn);
		BeamRingDraw(n, S16(n, 0xC) >= S16(n, 0x20) ? RingFade(n, S16(n, 0xC)) : 0);
		if (DrawOnly()) return 0;
		S16(n, 0xC) = (int16_t)(S16(n, 0xC) + 1);
		return S16(n, 0xC) >= S16(n, 0xE) ? TASK_END : 0;
	}

	static uint32_t __cdecl ChargeTask(TaskNode *tn)
	{
		const uint32_t n = P(tn);
		int16_t a[4];
		SpawnPosition(Caster(), 0x31, 0x1000, a);
		SpawnPosition(Caster(), 0x32, 0x1000, (int16_t *)game_addr(BEAM));
		S32(game_addr(BEAMV), 0) = a[0] - S16(game_addr(BEAM), 0);
		S32(game_addr(BEAMV), 4) = a[1] - S16(game_addr(BEAM), 2);
		S32(game_addr(BEAMV), 8) = a[2] - S16(game_addr(BEAM), 4);
		S16(game_addr(BEAM), 6) = (int16_t)Sqrt(Normalize((const int32_t *)game_addr(BEAMV), (int32_t *)game_addr(BEAMV)));
		S16(game_addr(BEAM), 0) = (int16_t)((S16(game_addr(BEAM), 0) + a[0]) / 2);
		S16(game_addr(BEAM), 2) = (int16_t)((S16(game_addr(BEAM), 2) + a[1]) / 2);
		S16(game_addr(BEAM), 4) = (int16_t)((S16(game_addr(BEAM), 4) + a[2]) / 2);
		if (DrawOnly()) return 0;
		const int16_t idx = S16(n, 0xE);
		const int32_t end = S32(game_addr(BOUNDS) + 4, 4 * idx), start = S32(game_addr(BOUNDS), 4 * idx);
		const int32_t c = S16(n, 0xC);
		if (c < end && c >= start && c == start)
		{
			if ((idx & 1) == 1)
			{
				const int32_t len = end - start;
				uint32_t t = NewTask(game_addr(ORIG_Beam));
				S16(t, 0xE) = (int16_t)len;
				S16(t, 0x1C) = 0x680;
				S16(t, 0x20) = 0x400;
				t = NewTask(game_addr(ORIG_BeamRing));
				S16(t, 0xE) = (int16_t)len;
				S16(t, 0x1C) = (int16_t)(CrtRand() % 0x300 + 0x900);
				S16(t, 0x20) = S16(t, 0x22) = (int16_t)(S16(t, 0xE) / 2);
			}
			S16(n, 0xE) = (int16_t)(S16(n, 0xE) + 1);
		}
		S16(n, 0xC) = (int16_t)(S16(n, 0xC) + 1);
		return S16(n, 0xC) >= 0x28 ? TASK_END : 0;
	}

	static int32_t BoltFade(uint32_t n, int32_t c) { return (0x1000 / S16(n, 0x22)) * (c - S16(n, 0x20)); }

	static void BoltDraw(uint32_t n, int32_t fade)
	{
		const int16_t ang[3] = { 0, 0, S16(n, 0x18) };
		Mat4x3 m;
		ComposeZYXRotationMatrix(ang, &m);
		m.t[0] = S16(game_addr(MOUTH), 0);
		m.t[1] = S16(game_addr(MOUTH), 2);
		m.t[2] = S16(game_addr(MOUTH), 4);
		const int32_t sc[3] = { S16(n, 0x1C), S16(n, 0x1C), shl32(S16(game_addr(MOUTH), 6), 12) / 5000 };
		ScaleMatrix(&m, sc);
		GteMatrixMultiply(Aim(), &m);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		const uint32_t h = P(FieldAlloc(0x58));
		U32(h, 8) = 0;
		U32(h, 0) = game_addr(MODEL_Bolt);
		U32(h, 0x1C) = 0x2033;
		U32(h, 4) = U32(game_addr(BOLT_MESHES), 4 * (S16(n, 0x1A) % 20));
		if (S16(n, 0xC) >= S16(n, 0x20))
		{
			U32(h, 0x1C) = 0x20F3;
			S32(h, 0xC) = fade;
		}
		PacketCursor() = RenderPrimModel((void *)h, RenderOT44(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static uint32_t __cdecl BoltTask(TaskNode *tn)
	{
		const uint32_t n = P(tn);
		BoltDraw(n, S16(n, 0xC) >= S16(n, 0x20) ? BoltFade(n, S16(n, 0xC)) : 0);
		if (DrawOnly()) return 0;
		S16(n, 0x1A) = (int16_t)(S16(n, 0x1A) + 1);
		S16(n, 0xC) = (int16_t)(S16(n, 0xC) + 1);
		return S16(n, 0xC) >= S16(n, 0xE) ? TASK_END : 0;
	}

	static uint32_t __cdecl BoltsTask(TaskNode *tn)
	{
		if (DrawOnly()) return 0;
		const uint32_t n = P(tn);
		const int16_t c = S16(n, 0xC);
		if (c >= 0 && c <= 0xC && (c & 1))
		{
			const uint32_t t = NewTask(game_addr(ORIG_Bolt));
			S16(t, 0xE) = (int16_t)(CrtRand() % 10 + 10);
			S16(t, 0x18) = (int16_t)((U8(n, 0xE) & 1) ? -0x200 : 0x200);
			S16(t, 0x1A) = (int16_t)(CrtRand() % 5);
			S16(t, 0x22) = 4;
			S16(t, 0x1C) = (int16_t)(CrtRand() % 0xA00 + 0xB00);
			S16(n, 0xE) = (int16_t)(S16(n, 0xE) + 1);
			S16(t, 0x1E) = (int16_t)(S16(t, 0x1C) / 4);
			S16(t, 0x20) = (int16_t)(S16(t, 0xE) - 4);
		}
		S16(n, 0xC) = (int16_t)(S16(n, 0xC) + 1);
		return S16(n, 0xC) >= 0x1E ? TASK_END : 0;
	}

	static void SparkDraw(uint32_t h, uint32_t p)
	{
		U16(h, 4) = U16(p, P_FRAME);
		const int16_t size = S16(p, P_SIZE);
		TransformCameraByShadowRotation((const void *)(p + P_POS), (int32_t)(uint16_t)size, -(size >> 4));
		PacketCursor() = InitEffectSequenceFromData((void *)h, RenderOT44(), 2, PacketCursor());
	}

	static uint32_t __cdecl SparksTask(TaskNode *tn)
	{
		const uint32_t n = P(tn);
		const uint32_t h = P(FieldAlloc(0xB4));
		U32(h, 0) = game_addr(SEQ_Spark);
		U16(h, 0x24) = 0;
		int alive = 0;
		for (uint32_t p = game_addr(PARTS); p < game_addr(PARTS_END); p += 0x18)
		{
			if (!(U8(p, P_MASK) & 1)) continue;
			SparkDraw(h, p);
			if (DrawOnly()) continue;
			S16(p, P_FRAME) = (int16_t)(S16(p, P_FRAME) + 1);
			if (S16(h, 0x28) < 0)
			{
				U32(p, P_MASK) = 0;
				continue;
			}
			alive++;
		}
		FieldFree(0xB4);
		if (DrawOnly()) return 0;
		const int16_t c = S16(n, 0xC);
		if (c >= 0 && c <= 0x10)
		{
			uint32_t p = FreeParticle();
			if (p)
			{
				U32(p, P_MASK) = 1;
				S16(p, P_FRAME) = 0;
				S16(p, P_SIZE) = (int16_t)(CrtRand() % 0x200 + 0x500);
				U32(p, P_POS) = U32(game_addr(MOUTH), 0);
				U32(p, P_POS + 4) = U32(game_addr(MOUTH), 4);
				S16(p, P_POS) = (int16_t)(S16(p, P_POS) + CrtRand() % 0x140 - 0xA0);
				S16(p, P_POS + 2) = (int16_t)(S16(p, P_POS + 2) + CrtRand() % 0x140 - 0xA0);
				S16(p, P_POS + 4) = (int16_t)(S16(p, P_POS + 4) + CrtRand() % 0x140 - 0xA0);
				p = FreeParticle();
				if (p)
				{
					U32(p, P_MASK) = 1;
					S16(p, P_FRAME) = 0;
					S16(p, P_SIZE) = (int16_t)(CrtRand() % 0x500 + 0x700);
					U32(p, P_POS) = U32(game_addr(TGT), 0);
					U32(p, P_POS + 4) = U32(game_addr(TGT), 4);
					S16(p, P_POS) = (int16_t)(S16(p, P_POS) + CrtRand() % 0x208 - 0x104);
					S16(p, P_POS + 2) = (int16_t)(S16(p, P_POS + 2) + CrtRand() % 0x208 - 0x104);
					S16(p, P_POS + 4) = (int16_t)(S16(p, P_POS + 4) + CrtRand() % 0x208 - 0x104);
				}
			}
		}
		S16(n, 0xC) = (int16_t)(S16(n, 0xC) + 1);
		return S16(n, 0xC) >= 4 && !alive ? TASK_END : 0;
	}

	static int32_t GlowColour(int32_t c) { return shl32(0x16 - c, 4); }

	static void GlowDraw(uint32_t n, int32_t colour)
	{
		const int16_t r = S16(n, 0x1C);
		TransformCameraByShadowRotation((const void *)game_addr(MOUTH), (int32_t)(uint16_t)r, -(r >> 4));
		const uint32_t h = P(FieldAlloc(0xB4));
		const int16_t c = S16(n, 0xC);
		U32(h, 0) = game_addr(SEQ_Glow);
		U16(h, 4) = (uint16_t)(c & 0xF);
		U16(h, 0x24) = 0;
		if (c >= 0xE)
		{
			U16(h, 0x24) = 4;
			U8(h, 0x1E) = U8(h, 0x1D) = U8(h, 0x1C) = (uint8_t)colour;
		}
		PacketCursor() = InitEffectSequenceFromData((void *)h, RenderOT44(), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static uint32_t __cdecl GlowTask(TaskNode *tn)
	{
		const uint32_t n = P(tn);
		GlowDraw(n, GlowColour(S16(n, 0xC)));
		if (DrawOnly()) return 0;
		S16(n, 0xC) = (int16_t)(S16(n, 0xC) + 1);
		return S16(n, 0xC) >= 0x16 ? TASK_END : 0;
	}

	static int32_t WaveFade(int32_t c) { return shl32(c - 0x10, 9); }

	static void WaveDraw(uint32_t n, int32_t fade)
	{
		const int16_t ang[3] = { 0, S16(n, 0x18), 0 };
		Mat4x3 m;
		ComposeZYXRotationMatrix(ang, &m);
		m.t[0] = S16(game_addr(TGT), 0);
		m.t[1] = S16(game_addr(TGT), 2);
		const int32_t s = S16(n, 0x1C);
		const int32_t sc[3] = { s, s, s };
		m.t[2] = S16(game_addr(TGT), 4);
		ScaleMatrix(&m, sc);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		const uint32_t h = P(FieldAlloc(0x58));
		const int16_t c = S16(n, 0xC);
		U32(h, 0) = game_addr(MODEL_Wave);
		U32(h, 8) = 0;
		U32(h, 0x10) = (uint32_t)(~((int32_t)c % 8) & 1);
		U32(h, 0x1C) = 0x1433;
		U32(h, 0x18) = U32(game_addr(WAVE_TEXTURES), 4 * (c & 7));
		if (c >= 0x10)
		{
			U32(h, 0x1C) = 0x14F3;
			S32(h, 0xC) = fade;
		}
		PacketCursor() = RenderPrimModel((void *)h, RenderOT44(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static uint32_t __cdecl WaveTask(TaskNode *tn)
	{
		const uint32_t n = P(tn);
		WaveDraw(n, WaveFade(S16(n, 0xC)));
		if (DrawOnly()) return 0;
		const int16_t c = S16(n, 0xC);
		int32_t d = ComputeSin(shl32(c, 11) / 24);
		d = (d * 40) >> 12;
		int16_t &w = *(int16_t *)(Target() + 0x20);
		const int16_t base = S16(n, 0x14);
		w = base;
		if (!(c & 1)) d = -d;
		w = (int16_t)(d + base);
		S16(n, 0xC) = (int16_t)(c + 1);
		if (S16(n, 0xC) < 0x18) return 0;
		w = base;
		return TASK_END;
	}

	static void SplashSetup(uint32_t h, uint32_t w)
	{
		U32(h, 0) = game_addr(SEQ_Splash);
		U16(h, 0x24) = 0;
		ff8fx::copy<(0x20)>((void *)(w + 8), (const void *)Aim());
		S32(w, 0x1C) = S16(game_addr(TGT), 0);
		S32(w, 0x20) = S16(game_addr(TGT), 2);
		S32(w, 0x24) = S16(game_addr(TGT), 4);
		ComposeAffineTransform(&Camera(), (const Mat4x3 *)(w + 8), (Mat4x3 *)(w + 8));
		GteSetLightMatrix((const void *)(w + 8));
		GteSetBKFromMatrix((const void *)(w + 8));
	}

	static void SplashDraw(uint32_t h, uint32_t w, uint32_t p)
	{
		GteLoadV0((const void *)(p + P_POS));
		GteMVMVA_LightV0Bk();
		GteSetRotScale(S16(p, P_SIZE));
		U16(h, 4) = U16(p, P_FRAME);
		GteReadMAC123((int32_t *)(w + 0x38));
		NormalizeVector((const int32_t *)(w + 0x38), (int32_t *)(w + 0x28));
		const int32_t k = -(S16(p, P_SIZE) >> 3);
		S32(w, 0x38) += mul32(k, S32(w, 0x28)) >> 12;
		S32(w, 0x3C) += mul32(S32(w, 0x2C), k) >> 12;
		S32(w, 0x40) += mul32(S32(w, 0x30), k) >> 12;
		GteSetTransFromVec32((const int32_t *)(w + 0x38));
		FrameCursor() = InitEffectSequenceFromData((void *)h, RenderOT44(), 2, FrameCursor());
	}

	static uint32_t __cdecl SplashesTask(TaskNode *tn)
	{
		const uint32_t n = P(tn);
		const uint32_t h = P(FieldAlloc(0xB4));
		uint32_t w = P(FieldAlloc(0x48));
		int alive = 0;
		SplashSetup(h, w);
		for (uint32_t p = game_addr(PARTS); p < game_addr(PARTS_END); p += 0x18)
		{
			if (!(U8(p, P_MASK) & 2)) continue;
			SplashDraw(h, w, p);
			if (DrawOnly()) continue;
			S16(p, P_FRAME) = (int16_t)(S16(p, P_FRAME) + 1);
			if (S16(h, 0x28) < 0)
			{
				U32(p, P_MASK) = 0;
				continue;
			}
			for (int j = 0; j < 3; j++)
			{
				S16(p, P_POS + 2 * j) = (int16_t)(S16(p, P_POS + 2 * j) + S16(p, P_VEL + 2 * j));
				S16(p, P_VEL + 2 * j) = (int16_t)(S16(p, P_VEL + 2 * j) - (int16_t)(S16(p, P_VEL + 2 * j) >> 2));
			}
			alive++;
		}
		FieldFree(0x48);
		FieldFree(0xB4);
		if (DrawOnly()) return 0;
		w = P(FieldAlloc(0x48));
		const int16_t c = S16(n, 0xC);
		if (c >= 0 && c <= 0xE)
		{
			for (int k = 0; k < 4; k++)
			{
				const uint32_t p = FreeParticle();
				if (!p) break;
				U32(p, P_MASK) = 2;
				S16(p, P_FRAME) = 0;
				S16(p, P_SIZE) = (int16_t)(CrtRand() % 0x800 + 0x900);
				S16(p, P_POS) = (int16_t)(CrtRand() % 0x190 - 0xC8);
				S16(p, P_POS + 2) = (int16_t)(CrtRand() % 0x190 - 0xC8);
				S16(p, P_POS + 4) = (int16_t)-(CrtRand() % 0x64);
				S32(w, 0x28) = CrtRand() % 0x1000 - 0x800;
				S32(w, 0x2C) = CrtRand() % 0x1000 - 0x800;
				S32(w, 0x30) = -(CrtRand() % 0x500);
				NormalizeVector((const int32_t *)(w + 0x28), (int32_t *)(w + 0x28));
				const int32_t sp = CrtRand() % 0x104 + 0xC8;
				S16(p, P_VEL) = (int16_t)(mul32(sp, S32(w, 0x28)) >> 12);
				S16(p, P_VEL + 2) = (int16_t)(mul32(S32(w, 0x2C), sp) >> 12);
				S16(p, P_VEL + 4) = (int16_t)(mul32(S32(w, 0x30), sp) >> 12);
			}
		}
		FieldFree(0x48);
		S16(n, 0xC) = (int16_t)(S16(n, 0xC) + 1);
		return S16(n, 0xC) >= 4 && !alive ? TASK_END : 0;
	}

	static int32_t FadeLevel(uint32_t n, int32_t c)
	{
		const int32_t e = S16(n, 0xE), top = S16(n, 0x1C);
		const int32_t up = e & 0xF, hold = (e >> 4) & 0xF, down = (e >> 8) & 0xF;
		if (c < up) return top / up * c;
		if ((c -= up) < hold) return top;
		if ((c -= hold) < down) return top - top / down * c;
		return -1;
	}

	static void FadeDraw(int32_t v) { ScreenFadeColour(v, v >> 2, v >> 2); }

	static uint32_t __cdecl FadeTask(TaskNode *tn)
	{
		const uint32_t n = P(tn);
		const int32_t v = FadeLevel(n, S16(n, 0xC));
		if (DrawOnly()) return 0;
		S16(n, 0xC) = (int16_t)(S16(n, 0xC) + 1);
		if (v < 0) return TASK_END;
		FadeDraw(v);
		return 0;
	}

	static uint32_t __cdecl DirectorTask(TaskNode *tn)
	{
		const uint32_t n = P(tn);
		SpawnPosition(Caster(), 0x15, 0x1000, (int16_t *)game_addr(MOUTH));
		if (S16(n, 0xC) == 0) GetDefaultEffectPosition(Target(), (int16_t *)game_addr(TGT));
		S32(game_addr(AIMV), 0) = S16(game_addr(TGT), 0) - S16(game_addr(MOUTH), 0);
		S32(game_addr(AIMV), 4) = 0;
		S32(game_addr(AIMV), 8) = S16(game_addr(TGT), 4) - S16(game_addr(MOUTH), 4);
		int32_t flat[3], axis[3];
		Mat4x3 m;
		NormalizeVector((const int32_t *)game_addr(AIMV), flat);
		int32_t a = RotationBetweenVectors((const int32_t *)game_addr(AIM_FROM), flat, axis);
		BuildAxisAngleRotationMatrix(a, Aim(), axis);
		S32(game_addr(AIMV), 4) = S16(game_addr(TGT), 2) - S16(game_addr(MOUTH), 2);
		S16(game_addr(MOUTH), 6) = (int16_t)Sqrt(Normalize((const int32_t *)game_addr(AIMV), (int32_t *)game_addr(AIMV)));
		a = RotationBetweenVectors(flat, (const int32_t *)game_addr(AIMV), axis);
		BuildAxisAngleRotationMatrix(a, &m, axis);
		GteMatrixMultiply(&m, Aim());
		const int32_t dx = S32(game_addr(AIMV), 0), dz = S32(game_addr(AIMV), 8);
		const int16_t c = S16(n, 0xC);
		S16(game_addr(MOUTH), 6) = (int16_t)(S16(game_addr(MOUTH), 6) - 300);
		S16(game_addr(MOUTH), 0) = (int16_t)(S16(game_addr(MOUTH), 0) + (mul32(dx, 300) >> 12));
		const int32_t dy = S32(game_addr(AIMV), 4);
		S16(game_addr(MOUTH), 2) = (int16_t)(S16(game_addr(MOUTH), 2) + (mul32(dy, 300) >> 12));
		S16(game_addr(MOUTH), 4) = (int16_t)(S16(game_addr(MOUTH), 4) + (mul32(dz, 300) >> 12));
		if (c == 0)
		{
			S16(game_addr(TGT), 0) = (int16_t)(S16(game_addr(TGT), 0) + (mul32(dx, -400) >> 12));
			S16(game_addr(TGT), 2) = (int16_t)(S16(game_addr(TGT), 2) + (mul32(dy, -400) >> 12));
			S16(game_addr(TGT), 4) = (int16_t)(S16(game_addr(TGT), 4) + (mul32(dz, -400) >> 12));
		}
		if (DrawOnly()) return 0;
		if (c == 0)
		{
			static constexpr game::Id KEYS[5] = { game::unk_D1E274, game::unk_D1E5AC, game::unk_D1E8E4, game::unk_D1EC1C, game::unk_D1E274 };
			for (int k = 0; k < 4; k++)
				for (int i = 0; i < 4; i++)
					MorphMesh(game_addr(KEYS[k]), game_addr(KEYS[k + 1]), U32(game_addr(game::unk_D20814) + 0x14 * k, 4 * i), 0x333 * (i + 1));
		}
		if (S16(n, 0xC) == 7)
		{
			const uint32_t t = NewTask(game_addr(ORIG_Charge));
			S16(t, 0xE) = 1;
		}
		if (S16(n, 0xC) == 0x27)
		{
			const uint32_t t = NewTask(game_addr(ORIG_Fade));
			S16(t, 0xE) = 0xE10;
			S16(t, 0x1C) = 0xFF;
		}
		if (S16(n, 0xC) == 0x28)
		{
			const uint32_t t = NewTask(game_addr(ORIG_Bolts));
			S16(t, 0xE) = 1;
		}
		if (S16(n, 0xC) == 0x27)
		{
			NewTask(game_addr(ORIG_Sparks));
			const uint32_t t = NewTask(game_addr(ORIG_Glow));
			S16(t, 0x1C) = 0xE00;
		}
		if (S16(n, 0xC) == 0x28)
		{
			const uint32_t t = NewTask(game_addr(ORIG_Wave));
			S16(t, 0x1C) = 0x1600;
			U32(t, 0x10) = *(const uint32_t *)(Target() + 0x1C);
			U32(t, 0x14) = *(const uint32_t *)(Target() + 0x20);
			S16(t, 0x18) = (int16_t)(*(const int16_t *)(Caster() + 0xE) + 0x800);
			NewTask(game_addr(ORIG_Splashes));
		}
		if (S16(n, 0xC) == 0x3C) ApplyActionResultToTarget((uint8_t *)U32(U32(Ctx(), 4), 8));
		const int16_t t = S16(n, 0xC);
		if (t <= 0x10) SetScreenFlash((uint32_t)shl32(t, 7), 0);
		else if (t >= 0x39) SetScreenFlash((uint32_t)shl32(0x41 - t, 8), 0);
		if (S16(n, 0xC) == 7) BdPlaySE((const void *)game_addr(SOUND_Charge), 0, 0x80);
		S16(n, 0xC) = (int16_t)(S16(n, 0xC) + 1);
		if (S16(n, 0xC) <= 0x41) return 0;
		SetScreenFlash(0, 0);
		return TASK_END;
	}

	static uint32_t __cdecl RootTask(TaskNode *tn)
	{
		const uint32_t n = P(tn);
		PacketCursor() = (U8(n, 0xC) & 1) ? TexBase() + 0xC000 : TexBase();
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
		if (S16(n, 0xC) == 1) TextureRestoreTask((void *)TexBase(), (void *)(n + 0xE));
		const int16_t done = S16(n, 0xE);
		S16(n, 0xC) = (int16_t)(S16(n, 0xC) + 1);
		return done ? TASK_END : 0;
	}

	void register_mag186_blaster_ports()
	{
		register_port(ORIG_Root, (void *)RootTask, "186 RootTask", 186);
		register_port(ORIG_TexRestore, (void *)TexRestoreTask, "186 TexRestoreTask", 186);
		register_port(ORIG_Director, (void *)DirectorTask, "186 DirectorTask", 186);
		register_port(ORIG_Fade, (void *)FadeTask, "186 FadeTask", 186);
		register_port(ORIG_Charge, (void *)ChargeTask, "186 ChargeTask", 186);
		register_port(ORIG_Beam, (void *)BeamTask, "186 BeamTask", 186);
		register_port(ORIG_BeamRing, (void *)BeamRingTask, "186 BeamRingTask", 186);
		register_port(ORIG_Bolts, (void *)BoltsTask, "186 BoltsTask", 186);
		register_port(ORIG_Bolt, (void *)BoltTask, "186 BoltTask", 186);
		register_port(ORIG_Sparks, (void *)SparksTask, "186 SparksTask", 186);
		register_port(ORIG_Glow, (void *)GlowTask, "186 GlowTask", 186);
		register_port(ORIG_Wave, (void *)WaveTask, "186 WaveTask", 186);
		register_port(ORIG_Splashes, (void *)SplashesTask, "186 SplashesTask", 186);
	}
}

	void register_mag186_blaster() { blaster186::register_mag186_blaster_ports(); }
}

