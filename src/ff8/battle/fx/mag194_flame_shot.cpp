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

// Effect 194: Flame Shot

#include "mag_common.h"

namespace ff8fx
{
namespace flame194
{
	using namespace eng;
	using namespace magc;

	inline int32_t &EndTick() { return var<int32_t>(game_addr(game::MAG_194_FLAMESHOT_AfterimageEndTick)); }
	inline int32_t &ModuleTick() { return var<int32_t>(game_addr(game::MAG_194_FLAMESHOT_ModuleTick)); }
	inline uint32_t &Flags() { return var<uint32_t>(game_addr(game::MAG_194_FLAMESHOT_Flags)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::MAG_194_FLAMESHOT_CastContext)); }
	inline int32_t &CasterSlot() { return var<int32_t>(game_addr(game::MAG_194_FLAMESHOT_CasterSlot)); }
	inline int32_t &PartCount() { return var<int32_t>(game_addr(game::MAG_194_FLAMESHOT_ParticleIdCount)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::MAG_194_FLAMESHOT_TextureBase)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_194_FLAMESHOT_PacketCursor)); }
	inline int32_t &FlightCount() { return var<int32_t>(game_addr(game::MAG_194_FLAMESHOT_FlightCount)); }
	inline TaskQueue *QRoot() { return (TaskQueue *)game_addr(game::MAG_194_FLAMESHOT_RootQueue); }
	inline TaskQueue *QShots() { return (TaskQueue *)game_addr(game::MAG_194_FLAMESHOT_ShotQueue); }
	inline TaskQueue *QAux() { return (TaskQueue *)game_addr(game::MAG_194_FLAMESHOT_AuxQueue); }
	static constexpr game::Id PARTS_A = game::MAG_194_FLAMESHOT_ParticlesEven, PARTS_B = game::MAG_194_FLAMESHOT_ParticlesOdd;
	static const int32_t PARTS_N = 150;
	static constexpr game::Id FLIGHTS = game::MAG_194_FLAMESHOT_FlightRecords;
	static constexpr game::Id SHOTS = game::MAG_194_FLAMESHOT_ShotRecords;
	static constexpr game::Id BIND_OUT = game::MAG_194_FLAMESHOT_AnimBindBlock;
	static constexpr game::Id BONES_NOW = game::MAG_194_FLAMESHOT_WeaponBonesNow;
	static constexpr game::Id GHOSTS = game::MAG_194_FLAMESHOT_GhostSlots, GHOSTS_END = game::MAG_194_FLAMESHOT_ShotCount;
	static const uint32_t ARENA_ODD = 0xC000;
	static const int32_t FLIGHT_POINTS = 24;

	static constexpr game::Id ORIG_RootTask = game::MAG_194_FLAMESHOT_RootTask;
	static constexpr game::Id ORIG_RestoreTask = game::MAG_194_FLAMESHOT_TextureRestoreTask;
	static constexpr game::Id ORIG_StartTask = game::MAG_194_FLAMESHOT_StartTask;
	static constexpr game::Id ORIG_ShotTask = game::MAG_194_FLAMESHOT_ShotTask;
	static constexpr game::Id ORIG_EndTask = game::MAG_194_FLAMESHOT_EndTask;
	static constexpr game::Id ORIG_FadeOutTask = game::MAG_194_FLAMESHOT_FadeOutTask;
	static constexpr game::Id ORIG_FadeInTask = game::MAG_194_FLAMESHOT_FadeInTask;
	static constexpr game::Id ORIG_AfterimageTask = game::MAG_194_FLAMESHOT_AfterimageTask;
	static constexpr game::Id ORIG_FlashTask = game::MAG_194_FLAMESHOT_FlashTask;
	static constexpr game::Id ORIG_SmokeTask = game::MAG_194_FLAMESHOT_SmokeTask;
	static constexpr game::Id ORIG_SpinTask = game::MAG_194_FLAMESHOT_SpinTask;
	static constexpr game::Id ORIG_GroundSmokeTask = game::MAG_194_FLAMESHOT_GroundSmokeTask;
	static constexpr game::Id ORIG_FlightTask = game::MAG_194_FLAMESHOT_FlightTask;
	static constexpr game::Id ORIG_HeadTask = game::MAG_194_FLAMESHOT_HeadTask;
	static constexpr game::Id ORIG_TrailTask = game::MAG_194_FLAMESHOT_TrailTask;
	static constexpr game::Id ORIG_EmberTask = game::MAG_194_FLAMESHOT_EmberTask;
	static constexpr game::Id ORIG_BurstTask = game::MAG_194_FLAMESHOT_BurstTask;
	static constexpr game::Id ORIG_RingTask = game::MAG_194_FLAMESHOT_RingTask;
	static constexpr game::Id ORIG_PillarTask = game::MAG_194_FLAMESHOT_PillarTask;
	static constexpr game::Id ORIG_FireTask = game::MAG_194_FLAMESHOT_FireTask;

	static constexpr game::Id ANIM_DATA = game::MAG_194_FLAMESHOT_AnimSetComData;
	static constexpr game::Id MODEL_Flash = game::MAG_194_FLAMESHOT_ModelFlash, MODEL_Spin = game::MAG_194_FLAMESHOT_ModelSpin, MODEL_Burst = game::MAG_194_FLAMESHOT_ModelBurst, MODEL_Ring = game::MAG_194_FLAMESHOT_ModelRing, MODEL_Pillar = game::MAG_194_FLAMESHOT_ModelPillar;
	static constexpr game::Id SEQ_Smoke = game::MAG_194_FLAMESHOT_SeqSmoke, SEQ_GroundSmoke = game::MAG_194_FLAMESHOT_SeqGroundSmoke, SEQ_Head = game::MAG_194_FLAMESHOT_SeqHead, SEQ_Flame = game::MAG_194_FLAMESHOT_SeqFlame;
	static const void *SOUND_Gun() { return (const void *)game_addr(game::MAG_194_FLAMESHOT_SoundGun); }
	static const int32_t *MUZZLE_REF() { return (const int32_t *)game_addr(game::MAG_194_FLAMESHOT_MuzzleAxis); }
	static const uint32_t *GHOST_COLOURS() { return (const uint32_t *)game_addr(game::MAG_194_FLAMESHOT_GhostColours); }
	static const uint32_t *EMBER_COLOURS() { return (const uint32_t *)game_addr(game::MAG_194_FLAMESHOT_EmberColours); }

	inline uint32_t BindExternalAnimSet(uint8_t *entity, uint32_t out, uint32_t data) { return fn<uint32_t (__cdecl *)(uint8_t *, uint32_t, uint32_t)>(game_addr(game::BattleEntity_BindExternalAnimSet))(entity, out, data); }
	inline uint32_t QueueChainTransformation(uint8_t *entity, uint32_t id) { return fn<uint32_t (__cdecl *)(uint8_t *, uint32_t)>(game_addr(game::QueueChainTransformation))(entity, id); }
	inline int32_t Normalize(const int32_t *in, int32_t *out) { return fn<int32_t (__cdecl *)(const int32_t *, int32_t *)>(game_addr(game::NormalizeVectorToFixedPoint))(in, out); }
	inline void MatVec(const Mat4x3 *m, const int16_t *in, int16_t *out) { fn<void (__cdecl *)(const Mat4x3 *, const int16_t *, int16_t *)>(game_addr(game::matrixMultiplyVector))(m, in, out); }
	inline void GteMatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline uint32_t RenderGeometry(uint32_t model, void *ctx, uint32_t ot, int32_t a, uint32_t cursor) { return fn<uint32_t (__cdecl *)(uint32_t, void *, uint32_t, int32_t, uint32_t)>(game_addr(game::RenderGeometry))(model, ctx, ot, a, cursor); }
	inline int32_t ComputeCos(int32_t a) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(a); }
	inline int32_t Sqrt(int32_t v) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::Math_SqrtInt))(v); }
	inline void SplineSetup(int32_t n, const void *points, void *work) { fn<void (__cdecl *)(int32_t, const void *, void *)>(game_addr(game::Effect_SplineSetup))(n, points, work); }
	inline void SplineEval(int32_t n, void *work, void *out, int32_t t) { fn<void (__cdecl *)(int32_t, void *, void *, int32_t)>(game_addr(game::Effect_SplineEval))(n, work, out, t); }
	inline uint32_t Mag151Render(void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::MAG_151_CannonBlow_ScrollRender))(h, ot, mode, cursor); }
	inline void GteSetLightMatrix(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_SetLightMatrix))(m); }
	inline void GteSetBackColorFromTrans(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_SetBackColorFromTrans))(m); }
	inline void GteMVMVA_LightV0Bk() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_LightV0_Bk))(); }
	inline void GteSetRotScale(int32_t s) { fn<void (__cdecl *)(int32_t)>(game_addr(game::GTE_SetRotScale))(s); }
	inline void GteTransFromMAC() { fn<void (__cdecl *)()>(game_addr(game::GTE_TransFromMAC))(); }
	inline void GteSetTransFromVec32(const int32_t *v) { fn<void (__cdecl *)(const int32_t *)>(game_addr(game::GTE_SetTransFromVec32))(v); }
	inline void GteLoadV012(const void *a, const void *b, const void *c) { fn<void (__cdecl *)(const void *, const void *, const void *)>(game_addr(game::GTE_LoadV012))(a, b, c); }
	inline void GteRTPT() { fn<void (__cdecl *)()>(game_addr(game::GTE_RTPT))(); }
	inline void GteReadFLAG(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadFLAG))(dst); }
	inline void GteReadSXY012Split(void *a, void *b, void *c) { fn<void (__cdecl *)(void *, void *, void *)>(game_addr(game::GTE_ReadSXY012_Split))(a, b, c); }
	inline void GteReadSZ3(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadSZ3))(dst); }
	inline uint32_t &FramePackets() { return var<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)); }

#pragma pack(push, 1)
	struct RootNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t done;
	};
	struct Node
	{
		TaskNode hdr;
		int16_t counter;
		int16_t f0E;
		int16_t pos[4];
		int16_t f18;
		int16_t f1A;
		int16_t f1C;
		int16_t f1E;
		uint8_t pad20[4];
	};
	struct ShotRec
	{
		int16_t pos[4];
		Mat4x3 m;
	};
	struct Flight
	{
		int16_t start[4];
		int16_t target[4];
		int16_t ctrl[3][4];
		int16_t pts[24][4];
	};
	struct Particle
	{
		int32_t id;
		int16_t frame;
		int16_t size;
		int16_t pos[4];
		int16_t vel[3];
		int16_t pad;
	};
	struct Ghost
	{
		int32_t used;
		uint8_t bones[0x80];
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x10 && sizeof(Node) == 0x24, "Flame Shot nodes");
	static_assert(sizeof(ShotRec) == 0x28 && sizeof(Flight) == 0xE8 && sizeof(Particle) == 0x18 && sizeof(Ghost) == 0x84, "Flame Shot records");

	inline ShotRec *Shot(int32_t i) { return (ShotRec *)(game_addr(SHOTS) + 0x28 * i); }
	inline Flight *FlightRec(int32_t k) { return (Flight *)(game_addr(FLIGHTS) + 0xE8 * k); }
	inline Ghost *GhostSlot(int32_t i) { return (Ghost *)(game_addr(GHOSTS) + 0x84 * i); }
	inline uint8_t *Caster() { return Entity(CasterSlot()); }
	inline Node *AddTask(TaskQueue *q, uint32_t fn) { return (Node *)AddTaskToQueue(q, fn); }
	inline Particle *PartArray(int16_t k) { return (Particle *)((k & 1) ? game_addr(PARTS_B) : game_addr(PARTS_A)); }
	inline Particle *FindFree(Particle *arr) { for (int32_t i = 0; i < PARTS_N; i++) if (arr[i].id == 0) return &arr[i]; return nullptr; }
}
}

namespace ff8fx
{
namespace flame194
{
	static void BoneFrame(uint8_t *anim_header, int32_t bone, int32_t *dir, int16_t *pos)
	{
		const uint8_t *sk = *(uint8_t **)*(uint8_t **)(anim_header + 4);
		if (bone >= 0xF0) bone = sk[bone - 0xEC];
		const uint8_t *b = sk + 0x10 + bone * 0x30;
		dir[1] = 0;
		dir[0] = 0;
		dir[2] = -0x1000;
		TransformVectorBy3x3Matrix((const Mat4x3 *)(b + 0x10), dir, dir);
		Normalize(dir, dir);
		pos[0] = *(const int16_t *)(b + 0x24);
		pos[1] = *(const int16_t *)(b + 0x28);
		pos[2] = *(const int16_t *)(b + 0x2C);
	}

	static void WeaponBone(uint8_t *entity, int32_t bone, int32_t *dir, int16_t *pos)
	{
		uint8_t *w = *(uint8_t **)(entity + 0x78);
		if (!w) return;
		BoneFrame(w, bone, dir, pos);
		Mat4x3 m;
		ComposeAffineTransform((const Mat4x3 *)(entity + 0x40), (const Mat4x3 *)(w + 0x24), &m);
		TransformVectorBy3x3Matrix(&m, dir, dir);
		Normalize(dir, dir);
		MatVec(&m, pos, pos);
		pos[0] = (int16_t)(pos[0] + (int16_t)m.t[0]);
		pos[1] = (int16_t)(pos[1] + (int16_t)m.t[1]);
		pos[2] = (int16_t)(pos[2] + (int16_t)m.t[2]);
	}

	static void Centroid(const uint8_t *targets, int32_t count, int16_t *out)
	{
		int16_t sx = 0, sy = 0, sz = 0;
		int16_t p[4];
		for (int32_t i = 0; i < count; i++)
		{
			GetDefaultEffectPosition(Entity(targets[i * TARGET_STRIDE]), p);
			sx = (int16_t)(sx + p[0]);
			sy = (int16_t)(sy + p[1]);
			sz = (int16_t)(sz + p[2]);
		}
		p[0] = (int16_t)(sx / count);
		p[1] = (int16_t)(sy / count);
		p[2] = (int16_t)(sz / count);
		ff8fx::copy<(8)>(out, p);
	}

	static void BoneSnapshot(uint8_t *w, uint8_t *dst)
	{
		const uint8_t *sk = *(const uint8_t **)*(uint8_t **)(w + 4);
		const int32_t n = sk[0];
		const uint8_t *src = sk + 0x20;
		for (int32_t k = 0; k < n; k++, src += 0x30, dst += 0x20) ff8fx::copy<(0x20)>(dst, src);
	}
	static void BoneRestore(uint8_t *w, const uint8_t *src)
	{
		uint8_t *sk = *(uint8_t **)*(uint8_t **)(w + 4);
		uint8_t *dst = sk + 0x20;
		for (int32_t k = 0; k < (int32_t)sk[0]; k++, src += 0x20, dst += 0x30) ff8fx::copy<(0x20)>(dst, src);
	}

	static void GhostDraw(uint8_t *entity)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x4C);
		uint8_t *w = *(uint8_t **)(entity + 0x78);
		if (w && !(entity[1] & 2))
		{
			Mat4x3 *m = (Mat4x3 *)h;
			ComposeAffineTransform(&Camera(), (const Mat4x3 *)(entity + 0x40), m);
			*(uint16_t *)(h + 0x34) = 0;
			*(uint16_t *)(h + 0x36) = 0;
			*(uint32_t *)(h + 0x24) = var<uint32_t>(game_addr(game::g_BattleVertexWorkspace));
			*(uint32_t *)(h + 0x3C) = *(const uint32_t *)(entity + 0x28);
			*(uint16_t *)(h + 0x38) = 0x140;
			*(uint16_t *)(h + 0x3A) = 0xD8;
			*(uint32_t *)(h + 0x30) = var<uint32_t>(game_addr(game::BATTLE_PACKET_BUFFER_END));
			*(uint32_t *)(h + 0x40) = 0xFFFFFFFF;
			const uint8_t c = entity[7];
			h[0x4A] = c;
			h[0x49] = c;
			h[0x48] = c;
			ComposeAffineTransform(m, (const Mat4x3 *)(w + 0x24), m);
			ComputeBonesWorldMatrices(w, m);
			FramePackets() = RenderGeometry(*(const uint32_t *)(w + 4), h + 0x20, RenderOT(), 4, FramePackets());
		}
		FieldFree(0x4C);
	}

	static void ModelDraw(uint32_t model, bool faded, int32_t fade)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		*(uint32_t *)h = model;
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0x1C) = 0x33;
		if (faded)
		{
			*(int32_t *)(h + 0xC) = fade;
			*(uint32_t *)(h + 0x1C) = 0xF3;
		}
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static void MuzzleModel(const Node *n, uint32_t model, bool faded, int32_t fade)
	{
		const ShotRec *s = Shot(n->f0E);
		int16_t ang[4];
		ang[0] = 0;
		ang[1] = 0;
		ang[2] = n->f18;
		ang[3] = 0;
		Mat4x3 m;
		ComposeZYXRotationMatrix(ang, &m);
		GteMatrixMultiply(&s->m, &m);
		m.t[0] = s->pos[0];
		m.t[1] = s->pos[1];
		const int32_t sc = n->f1C;
		const int32_t v[3] = { sc, sc, sc };
		m.t[2] = s->pos[2];
		Scale3DMatrix(&m, v);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		ModelDraw(model, faded, fade);
	}
	static int32_t FlashFade(int32_t c) { return (int32_t)((uint32_t)(c - 2) << 10); }
	static int32_t SpinFade(int32_t c) { return (int32_t)((uint32_t)(c - 2) << 9); }
	static int32_t BurstFade(int32_t c) { return mul32(c, 0x2AA) - 0x2AA; }
	static int32_t RingFade(int32_t c) { return mul32(c - 2, 0x155); }
	static int32_t PillarFade(int32_t c) { return (int32_t)((uint32_t)(c - 8) << 9); }

	static void AimMatrix(const Node *n)
	{
		int16_t ang[4];
		ang[0] = 0;
		ang[1] = n->f18;
		ang[2] = 0;
		ang[3] = 0;
		Mat4x3 m;
		ComposeZYXRotationMatrix(ang, &m);
		m.t[0] = n->pos[0];
		m.t[1] = n->pos[1];
		const int32_t sc = n->f1C;
		const int32_t v[3] = { sc, sc, sc };
		m.t[2] = n->pos[2];
		Scale3DMatrix(&m, v);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
	}
	static void RingDraw(const Node *n, bool faded, int32_t fade)
	{
		AimMatrix(n);
		ModelDraw(game_addr(MODEL_Ring), faded, fade);
	}
	static void PillarDraw(const Node *n, int32_t scroll, bool faded, int32_t fade)
	{
		AimMatrix(n);
		uint8_t *h = (uint8_t *)FieldAlloc(0x7C);
		*(uint32_t *)h = game_addr(MODEL_Pillar);
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0x20) = 0x33;
		*(int16_t *)(h + 0x18) = 0;
		*(int16_t *)(h + 0x1A) = 0;
		*(int16_t *)(h + 0x1C) = 0x80;
		*(int16_t *)(h + 0x1E) = 0x40;
		*(int32_t *)(h + 0x10) = scroll;
		*(int32_t *)(h + 0x14) = 0;
		if (faded)
		{
			*(uint32_t *)(h + 0x20) = 0xF3;
			*(int32_t *)(h + 0xC) = fade;
		}
		PacketCursor() = Mag151Render(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x7C);
	}
	static void BurstDraw(const Node *n, bool faded, int32_t fade)
	{
		TransformCameraByShadowRotation(n->pos, n->f1C, -((int32_t)n->f1C >> 3));
		ModelDraw(game_addr(MODEL_Burst), faded, fade);
	}

	static int16_t SmokeDraw(const Node *n)
	{
		TransformCameraByShadowRotation(Shot(n->f0E)->pos, n->f1C, -((int32_t)n->f1C >> 3));
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		*(uint32_t *)h = game_addr(SEQ_Smoke);
		*(int16_t *)(h + 4) = n->counter;
		*(int16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
		FieldFree(0xB4);
		return *(const int16_t *)(h + 0x28);
	}

	static int16_t GroundSmokeDraw(const Node *n)
	{
		const ShotRec *s = Shot(n->f0E);
		int16_t ang[4];
		ang[0] = 0x400;
		ang[1] = 0;
		ang[2] = 0;
		ang[3] = 0;
		Mat4x3 m;
		ComposeZYXRotationMatrix(ang, &m);
		m.t[0] = s->pos[0];
		m.t[1] = 0;
		const int32_t sc = n->f1C;
		const int32_t v[3] = { sc, sc, sc };
		m.t[2] = s->pos[2];
		Scale3DMatrix(&m, v);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		*(uint32_t *)h = game_addr(SEQ_GroundSmoke);
		*(int16_t *)(h + 4) = n->counter;
		*(int16_t *)(h + 0x24) = 8;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
		FieldFree(0xB4);
		return *(const int16_t *)(h + 0x28);
	}

	static void HeadDraw(const int16_t *pt, int16_t size)
	{
		TransformCameraByShadowRotation(pt, size, -((int32_t)size >> 3));
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		*(int16_t *)(h + 4) = 0;
		*(int16_t *)(h + 0x24) = 0;
		*(uint32_t *)h = game_addr(SEQ_Head);
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static void TrailDraw(const Particle *e, uint8_t *h)
	{
		*(int16_t *)(h + 4) = (int16_t)((e->frame << 1) + 1);
		TransformCameraByShadowRotation(e->pos, e->size, -((int32_t)e->size >> 4));
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
	}

	static void EmberSetup(uint8_t *q)
	{
		q[0x3C] = 0x80;
		q[0x34] = 0x80;
		*(uint32_t *)(q + 0x30) = 0x2E808080;
		q[0x40] = 0xA0;
		q[0x38] = 0xA0;
		*(uint16_t *)(q + 0x36) = 0x3E14;
		q[0x39] = 0x40;
		q[0x35] = 0x40;
		*(uint16_t *)(q + 0x3A) = 0x38;
		q[0x41] = 0x60;
		q[0x3D] = 0x60;
		*(int16_t *)(q + 0x18) = 0x100;
		*(int16_t *)(q + 0x10) = -0x100;
		*(int16_t *)(q + 0x12) = -0x100;
		*(int16_t *)(q + 0x1A) = -0x100;
		*(int16_t *)(q + 0x20) = -0x100;
		*(int16_t *)(q + 0x22) = 0x100;
		*(int16_t *)(q + 0x24) = 0;
		*(int16_t *)(q + 0x1C) = 0;
		*(int16_t *)(q + 0x14) = 0;
	}
	static void EmberLight(uint8_t *s)
	{
		ff8fx::copy<(0x20)>(s + 0x10, &Camera());
		GteSetLightMatrix(s + 0x10);
		GteSetBackColorFromTrans(s + 0x10);
	}
	static void EmberLoad(const int16_t *pos)
	{
		GteLoadV0(pos);
		GteMVMVA_LightV0Bk();
	}
	static bool EmberQuad(uint8_t *q, int16_t size, uint32_t colour, uint8_t *pk)
	{
		*(uint32_t *)(pk + 4) = colour;
		GteSetRotScale(size);
		GteTransFromMAC();
		GteLoadV012(q + 0x10, q + 0x18, q + 0x20);
		GteRTPT();
		*(uint32_t *)(pk + 0x1C) = *(const uint32_t *)(q + 0x3C);
		*(uint32_t *)(pk + 0xC) = *(const uint32_t *)(q + 0x34);
		*(uint32_t *)pk = 0x9000000;
		*(uint32_t *)(pk + 0x14) = *(const uint32_t *)(q + 0x38);
		*(uint32_t *)(pk + 0x24) = *(const uint32_t *)(q + 0x40);
		GteReadFLAG(q + 8);
		if (*(const uint32_t *)(q + 8) & 0x60000) return false;
		GteReadSXY012Split(pk + 8, pk + 0x10, pk + 0x18);
		*(uint16_t *)(pk + 0x20) = *(const uint16_t *)(pk + 0x10);
		*(uint16_t *)(pk + 0x22) = *(const uint16_t *)(pk + 0x1A);
		GteReadSZ3(q + 0xC);
		InsertPrimAutoDepth(*(const uint32_t *)q + (*(const int32_t *)(q + 0xC) >> *(const int32_t *)(q + 4)) * 4, pk);
		return true;
	}

	static void FireDraw(const Particle *e, uint8_t *s, uint8_t *h)
	{
		GteLoadV0(e->pos);
		GteMVMVA_LightV0Bk();
		GteSetRotScale(e->size);
		*(int16_t *)(h + 4) = e->frame;
		int32_t *v = (int32_t *)(s + 0x38), *d = (int32_t *)(s + 0x28);
		GteReadMAC123(v);
		Normalize(v, d);
		const int32_t k = -((int32_t)e->size >> 4);
		v[0] += mul32(d[0], k) >> 12;
		v[1] += mul32(d[1], k) >> 12;
		v[2] += mul32(d[2], k) >> 12;
		GteSetTransFromVec32(v);
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
	}
	static void FireLight(uint8_t *s)
	{
		ff8fx::copy<(0x20)>(s + 8, &Camera());
		GteSetLightMatrix(s + 8);
		GteSetBackColorFromTrans(s + 8);
	}

	static uint32_t __cdecl FlashTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		MuzzleModel(n, game_addr(MODEL_Flash), n->counter >= 2, FlashFade(n->counter));
		if (UpdateFlags() & 0x201) return 0;
		const int16_t step = n->f1E;
		n->f1C = (int16_t)(n->f1C + step);
		n->f1E = (int16_t)(step - (int16_t)(step / 4));
		n->counter++;
		return n->counter >= 6 ? TASK_END : 0;
	}

	static uint32_t __cdecl SpinTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		MuzzleModel(n, game_addr(MODEL_Spin), n->counter >= 2, SpinFade(n->counter));
		if (UpdateFlags() & 0x201) return 0;
		const int16_t spin = n->f1A, step = n->f1E;
		n->f18 = (int16_t)(n->f18 + spin);
		n->f1A = (int16_t)(spin - (int16_t)(spin >> 4));
		n->f1C = (int16_t)(n->f1C + step);
		n->f1E = (int16_t)(step - (int16_t)(step / 4));
		n->counter++;
		return n->counter >= 10 ? TASK_END : 0;
	}

	static uint32_t __cdecl SmokeTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		const int16_t next = SmokeDraw(n);
		if (UpdateFlags() & 0x201) return 0;
		n->counter++;
		return next < 0 ? TASK_END : 0;
	}

	static uint32_t __cdecl GroundSmokeTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		const int16_t next = GroundSmokeDraw(n);
		if (UpdateFlags() & 0x201) return 0;
		n->counter++;
		return next < 0 ? TASK_END : 0;
	}

	static uint32_t __cdecl FlightTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		Flight *t = FlightRec(n->f18);
		const ShotRec *s = Shot(n->f0E);
		if (UpdateFlags() & 0x201) return 0;
		if (n->counter == 0)
		{
			ff8fx::copy<(8)>(t->start, s->pos);
			ff8fx::copy<(8)>(t->target, n->pos);
			int32_t v[3];
			TransformVectorBy3x3Matrix(&s->m, MUZZLE_REF(), v);
			const int32_t dx = t->target[0] - t->start[0];
			const int32_t dz = t->target[2] - t->start[2];
			const int32_t half = Sqrt(mul32(dz, dz) + mul32(dx, dx)) >> 1;
			ff8fx::copy<(8)>(t->ctrl[0], t->start);
			t->ctrl[1][0] = (int16_t)((mul32(v[0], half) >> 12) + t->start[0]);
			t->ctrl[1][1] = (int16_t)((mul32(v[1], half) >> 12) + t->start[1]);
			t->ctrl[1][2] = (int16_t)((mul32(v[2], half) >> 12) + t->start[2]);
			ff8fx::copy<(8)>(t->ctrl[2], t->target);
			void *work = FieldAlloc(0x190);
			SplineSetup(3, t->ctrl, work);
			for (int32_t i = 0; i < FLIGHT_POINTS; i++) SplineEval(3, work, t->pts[i], (i * 0x1000) / 23);
			FieldFree(0x190);
			Node *hd = AddTask(QAux(), game_addr(ORIG_HeadTask));
			hd->counter = 0;
			hd->f0E = n->f18;
			hd->f18 = 0;
			hd->f1A = 5;
			hd->f1C = (int16_t)(CrtRand() % 0x180 + 0x400);
			Node *tr = AddTask(QAux(), game_addr(ORIG_TrailTask));
			tr->pos[3] = (int16_t)PartCount();
			PartCount()++;
			tr->counter = 0;
			tr->f0E = n->f18;
			tr->f18 = 0;
			tr->f1A = 5;
			Node *em = AddTask(QAux(), game_addr(ORIG_EmberTask));
			em->f0E = n->f18;
			em->pos[3] = (int16_t)PartCount();
			PartCount()++;
			em->counter = 0;
			em->f18 = 0;
			em->f1A = 5;
		}
		n->counter++;
		return n->counter >= 18 ? TASK_END : 0;
	}

	static void FlightStep(Node *n)
	{
		const int16_t speed = n->f1A;
		n->f18 = (int16_t)(n->f18 + speed);
		if (speed > 2) n->f1A = (int16_t)(speed - 1);
	}

	static uint32_t __cdecl HeadTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		HeadDraw(FlightRec(n->f0E)->pts[n->f18], n->f1C);
		if (UpdateFlags() & 0x201) return 0;
		FlightStep(n);
		n->counter++;
		return n->f18 >= FLIGHT_POINTS ? TASK_END : 0;
	}

	static uint32_t __cdecl TrailTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		Particle *arr = PartArray(n->f0E);
		*(volatile uint32_t *)&n_ = (uint32_t)arr;
		Flight *t = FlightRec(n->f0E);
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		int32_t alive = 0;
		*(int16_t *)(h + 0x24) = 0;
		const int32_t id = n->pos[3] + 1;
		*(uint32_t *)h = game_addr(SEQ_Flame);
		for (Particle *e = arr; e < arr + PARTS_N; e++)
		{
			if (e->id != id) continue;
			TrailDraw(e, h);
			if (UpdateFlags() & 0x201) continue;
			e->frame++;
			if (*(const int16_t *)(h + 0x28) < 0)
			{
				e->id = 0;
				continue;
			}
			e->size = (int16_t)(e->size + (int16_t)(e->size >> 5));
			alive++;
		}
		FieldFree(0xB4);
		if (UpdateFlags() & 0x201) return 0;
		FlightStep(n);
		if (n->f18 < FLIGHT_POINTS)
		{
			for (int32_t k = n->f18; k >= n->f18 - n->f1A; k--)
			{
				Particle *e = FindFree(arr);
				if (!e) break;
				e->id = id;
				e->frame = 0;
				e->size = (int16_t)(CrtRand() % 0x100 + 0x300);
				ff8fx::copy<(8)>(e->pos, t->pts[k]);
			}
		}
		n->counter++;
		return (n->counter >= 4 && alive == 0) ? TASK_END : 0;
	}

	static uint32_t __cdecl EmberTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		Particle *arr = PartArray(n->f0E);
		Flight *t = FlightRec(n->f0E);
		uint8_t *s = (uint8_t *)FieldAlloc(0x50);
		uint8_t *q = (uint8_t *)FieldAlloc(0x44);
		EmberSetup(q);
		int32_t prims = 0;
		EmberLight(s);
		uint8_t *pk = (uint8_t *)PacketCursor();
		const int32_t id = n->pos[3] + 1;
		*(uint32_t *)q = RenderOT();
		*(int32_t *)(q + 4) = 4;
		for (Particle *e = arr; e < arr + PARTS_N; e++)
		{
			if (e->id != id) continue;
			const int32_t frame = e->frame;
			EmberLoad(e->pos);
			const uint32_t colour = EMBER_COLOURS()[frame];
			if (!(UpdateFlags() & 0x201))
			{
				e->frame = (int16_t)(frame + 1);
				if (frame + 1 >= 8)
				{
					e->id = 0;
				}
				else
				{
					e->pos[0] = (int16_t)(e->pos[0] + e->vel[0]);
					e->pos[1] = (int16_t)(e->pos[1] + e->vel[1]);
					e->pos[2] = (int16_t)(e->pos[2] + e->vel[2]);
					e->vel[0] = (int16_t)(e->vel[0] - (int16_t)(e->vel[0] >> 4));
					e->vel[1] = (int16_t)(e->vel[1] - (int16_t)(e->vel[1] >> 4));
					e->vel[2] = (int16_t)(e->vel[2] - (int16_t)(e->vel[2] >> 4));
				}
			}
			if (EmberQuad(q, e->size, colour, pk))
			{
				pk += 0x28;
				prims++;
			}
		}
		FieldFree(0x44);
		PacketCursor() = (uint32_t)pk;
		FieldFree(0x50);
		if (UpdateFlags() & 0x201) return 0;
		uint8_t *w = (uint8_t *)FieldAlloc(0x50);
		FlightStep(n);
		if (n->f18 < FLIGHT_POINTS)
		{
			int32_t k = n->f18, b = k;
			if (k >= n->f18 - n->f1A)
			{
				do
				{
					for (int32_t j = 0; j < 4; j++)
					{
						Particle *e = FindFree(arr);
						if (!e) break;
						e->id = id;
						e->frame = 0;
						e->size = (int16_t)(CrtRand() % 0x100 + 0x180);
						ff8fx::copy<(8)>(e->pos, t->pts[k]);
						e->pos[0] = (int16_t)(e->pos[0] + (CrtRand() % 400 - 200));
						e->pos[1] = (int16_t)(e->pos[1] + (CrtRand() % 400 - 200));
						e->pos[2] = (int16_t)(e->pos[2] + (CrtRand() % 400 - 200));
						int32_t a = k + 1;
						if (a >= FLIGHT_POINTS)
						{
							a = k;
							k = k - 1;
							b = k;
						}
						int32_t *dir = (int32_t *)(w + 0x30), *axis = (int32_t *)(w + 0x40);
						dir[0] = t->pts[a][0] - t->pts[b][0];
						dir[1] = t->pts[a][1] - t->pts[b][1];
						dir[2] = t->pts[a][2] - t->pts[b][2];
						Normalize(dir, dir);
						const int32_t angle = RotationBetweenVectors(MUZZLE_REF(), dir, axis);
						BuildAxisAngleRotationMatrix(angle, (Mat4x3 *)(w + 0x10), axis);
						const int32_t r = CrtRand() % 0x1000;
						dir[0] = ComputeSin(r);
						dir[1] = ComputeCos(r);
						dir[2] = CrtRand() % 0x800;
						TransformVectorBy3x3Matrix((const Mat4x3 *)(w + 0x10), dir, dir);
						const int32_t sp = CrtRand() % 0x23 + 0xF;
						e->vel[0] = (int16_t)(mul32(dir[0], sp) >> 12);
						e->vel[1] = (int16_t)(mul32(dir[1], sp) >> 12);
						e->vel[2] = (int16_t)(mul32(dir[2], sp) >> 12);
						k = b;
					}
					k--;
					b = k;
				} while (k >= n->f18 - n->f1A);
			}
		}
		FieldFree(0x50);
		n->counter++;
		return (n->counter >= 8 && prims == 0) ? TASK_END : 0;
	}

	static uint32_t __cdecl BurstTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		BurstDraw(n, n->counter >= 1, BurstFade(n->counter));
		if (UpdateFlags() & 0x201) return 0;
		const int16_t step = n->f1E;
		n->f1C = (int16_t)(n->f1C + step);
		n->f1E = (int16_t)(step - (int16_t)(step / 6));
		n->counter++;
		return n->counter >= 7 ? TASK_END : 0;
	}

	static uint32_t __cdecl RingTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		RingDraw(n, n->counter >= 2, RingFade(n->counter));
		if (UpdateFlags() & 0x201) return 0;
		const int16_t spin = n->f1A;
		n->f18 = (int16_t)(n->f18 + spin);
		n->f1A = (int16_t)(spin - (int16_t)(spin / 12));
		const int16_t step = n->f1E;
		n->f1C = (int16_t)(n->f1C + step);
		n->f1E = (int16_t)(step - (int16_t)(step / 4));
		n->counter++;
		return n->counter >= 14 ? TASK_END : 0;
	}

	static uint32_t __cdecl PillarTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		PillarDraw(n, mul32(n->counter, -12), n->counter >= 8, PillarFade(n->counter));
		if (UpdateFlags() & 0x201) return 0;
		const int16_t spin = n->f1A;
		n->f18 = (int16_t)(n->f18 + spin);
		n->f1A = (int16_t)(spin - (int16_t)(spin / 12));
		const int16_t step = n->f1E;
		n->f1C = (int16_t)(n->f1C + step);
		n->f1E = (int16_t)(step - (int16_t)(step / 8));
		n->counter++;
		return n->counter >= 16 ? TASK_END : 0;
	}

	static uint32_t __cdecl FireTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		Particle *arr = PartArray(n->f0E);
		uint8_t *s = (uint8_t *)FieldAlloc(0x48);
		const int32_t id = n->pos[3] + 1;
		if (!(UpdateFlags() & 0x201) && n->counter <= 9)
		{
			for (int32_t j = 0; j < 3; j++)
			{
				Particle *e = FindFree(arr);
				if (!e) break;
				e->id = id;
				e->frame = 0;
				e->size = (int16_t)(CrtRand() % 0x900 + 0xA00);
				const int32_t a = CrtRand() % 0x1000;
				const int32_t r = (n->counter + 1) * 400;
				ff8fx::copy<(8)>(e->pos, n->pos);
				e->pos[0] = (int16_t)(e->pos[0] + (int16_t)(mul32(ComputeSin(a), r) >> 12));
				e->pos[1] = (int16_t)(e->pos[1] + (CrtRand() % 0x44C - 0x226));
				e->pos[2] = (int16_t)(e->pos[2] + (int16_t)(mul32(ComputeCos(a), r) >> 12));
			}
		}
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		int32_t alive = 0;
		*(int16_t *)(h + 0x24) = 0;
		FireLight(s);
		*(uint32_t *)h = game_addr(SEQ_Flame);
		for (Particle *e = arr; e < arr + PARTS_N; e++)
		{
			if (e->id != id) continue;
			FireDraw(e, s, h);
			if (UpdateFlags() & 0x201) continue;
			e->frame++;
			if (*(const int16_t *)(h + 0x28) < 0)
			{
				e->id = 0;
				continue;
			}
			alive++;
		}
		FieldFree(0xB4);
		FieldFree(0x48);
		if (UpdateFlags() & 0x201) return 0;
		n->counter++;
		return alive ? 0 : TASK_END;
	}

	static void AddSpin(const Node *n)
	{
		Node *p = AddTask(QAux(), game_addr(ORIG_SpinTask));
		p->counter = 0;
		p->f0E = n->f0E;
		p->f18 = (int16_t)(CrtRand() % 0x1000);
		p->f1A = (int16_t)(CrtRand() % 0x6E + 0x3C);
		if (p->f18 & 1) p->f1A = (int16_t)-p->f1A;
		const int16_t third = (int16_t)((CrtRand() % 0x300 + 0x600) / 3);
		p->f1E = third;
		p->f1C = third;
	}
	static Node *AddAim(const Node *n, uint32_t fn)
	{
		Node *p = AddTask(QAux(), fn);
		ff8fx::copy<(8)>(p->pos, n->pos);
		p->counter = 0;
		p->f0E = n->f0E;
		return p;
	}

	static uint32_t __cdecl ShotTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		if (UpdateFlags() & 0x201) return 0;
		if (!(Flags() & 1)) return 0;
		if (n->counter == 0)
		{
			if (Flags() & 4) return 0;
			QueueChainTransformation(Caster(), n->f18);
			Flags() |= 4;
		}
		if (n->counter == 15)
		{
			Ctx()->done = 1;
			Flags() &= ~4u;
		}
		if (n->counter == 0)
		{
			ShotRec *s = Shot(n->f0E);
			int16_t pos[4];
			int32_t dir[3];
			WeaponBone(Caster(), 2, dir, pos);
			s->pos[0] = (int16_t)((mul32(dir[0], 350) >> 12) + pos[0]);
			s->pos[1] = (int16_t)((mul32(dir[1], 350) >> 12) + pos[1]);
			s->pos[2] = (int16_t)((mul32(dir[2], 350) >> 12) + pos[2]);
			int32_t axis[3];
			const int32_t angle = RotationBetweenVectors(MUZZLE_REF(), dir, axis);
			*(volatile int32_t *)&n_ = angle;
			BuildAxisAngleRotationMatrix(angle, &s->m, axis);
			Centroid(Ctx()->actions->targets, Ctx()->actions->target_count, n->pos);
			n->pos[0] = (int16_t)(n->pos[0] + (CrtRand() % 2000 - 1000));
			n->pos[1] = (int16_t)(n->pos[1] + (CrtRand() % 2000 - 1000));
			if (n->pos[1] > -500) n->pos[1] = -500;
			n->pos[2] = (int16_t)(n->pos[2] + (CrtRand() % 2000 - 1000));
			Node *f = AddTask(QAux(), game_addr(ORIG_FlashTask));
			f->counter = 0;
			f->f0E = n->f0E;
			f->f18 = (int16_t)(CrtRand() % 0x1000);
			const int16_t third = (int16_t)((CrtRand() % 0x600 + 0xB00) / 3);
			f->f1E = third;
			f->f1C = third;
			Node *k = AddTask(QAux(), game_addr(ORIG_SmokeTask));
			k->counter = 0;
			k->f0E = n->f0E;
			k->f1C = (int16_t)(CrtRand() % 0x700 + 0x400);
			AddSpin(n);
			Node *l = AddTask(QAux(), game_addr(ORIG_FlightTask));
			l->f0E = n->f0E;
			ff8fx::copy<(8)>(l->pos, n->pos);
			const int32_t fc = FlightCount();
			l->counter = 0;
			l->f18 = (int16_t)(fc % 8);
			FlightCount() = fc + 1;
		}
		if (n->counter == 1)
		{
			Node *d = AddTask(QAux(), game_addr(ORIG_GroundSmokeTask));
			d->counter = 0;
			d->f0E = n->f0E;
			d->f1C = (int16_t)(CrtRand() % 0x380 + 0xB00);
		}
		if (n->counter >= 0 && n->counter <= 2) AddSpin(n);
		if (n->counter == 8)
		{
			Node *b = AddAim(n, game_addr(ORIG_BurstTask));
			const int16_t quarter = (int16_t)((CrtRand() % 0x600 + 0x1400) / 4);
			b->f1E = quarter;
			b->f1C = quarter;
		}
		if (n->counter == 9)
		{
			Node *r = AddAim(n, game_addr(ORIG_RingTask));
			r->f18 = (int16_t)(CrtRand() % 0x1000);
			r->f1A = (int16_t)(CrtRand() % 0xA0 + 0x5A);
			if (r->f18 & 1) r->f1A = (int16_t)-r->f1A;
			const int16_t third = (int16_t)((CrtRand() % 0x400 + 0xB80) / 3);
			r->f1E = third;
			r->f1C = third;
			Node *r2 = AddAim(n, game_addr(ORIG_RingTask));
			r2->f18 = (int16_t)(CrtRand() % 0x1000);
			r2->f1A = (int16_t)mul32((2 * (int32_t)r->f1A) / 3, *(volatile int32_t *)&n_);
			r2->f1C = (int16_t)(r->f1C - (int16_t)(r->f1C >> 4));
			r2->f1E = r->f1E;
			Node *p = AddAim(n, game_addr(ORIG_PillarTask));
			p->f18 = (int16_t)(CrtRand() % 0x1000);
			p->f1A = 0;
			const int16_t fifth = (int16_t)((CrtRand() % 0x400 + 0x1200) / 5);
			p->f1E = fifth;
			p->f1C = fifth;
			Node *fi = AddTask(QAux(), game_addr(ORIG_FireTask));
			fi->f0E = n->f0E;
			ff8fx::copy<(8)>(fi->pos, n->pos);
			fi->pos[3] = (int16_t)PartCount();
			PartCount()++;
			fi->counter = 0;
		}
		if (n->counter == 0xB) ApplyActionResultToTargets(Ctx()->actions[0].targets, Ctx()->actions[0].target_count);
		if (n->counter == 1) BdPlaySE(SOUND_Gun(), (n->f0E & 1) ? 1 : 2, 0x80);
		n->counter++;
		return n->counter >= 20 ? TASK_END : 0;
	}

	static uint32_t __cdecl FadeOutTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		SetScreenFlash(n->f18 - n->f18 / n->f0E * n->counter, 0);
		if (UpdateFlags() & 0x201) return 0;
		n->counter++;
		if (n->counter < n->f0E) return 0;
		SetScreenFlash(0, 0);
		return TASK_END;
	}

	static uint32_t __cdecl FadeInTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		SetScreenFlash(n->f18 / n->f0E * n->counter, 0);
		if (UpdateFlags() & 0x201) return 0;
		n->counter++;
		return n->counter >= n->f0E ? TASK_END : 0;
	}

	static uint32_t __cdecl AfterimageTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		const int32_t c = n->counter;
		Ghost *g = GhostSlot(c % 5);
		g->used = 1;
		uint8_t *e = Caster();
		BoneSnapshot(*(uint8_t **)(e + 0x78), g->bones);
		BoneSnapshot(*(uint8_t **)(Caster() + 0x78), (uint8_t *)game_addr(BONES_NOW));
		const uint32_t colour = *(const uint32_t *)(Caster() + 0x28);
		int32_t slot = c - 1 >= 0 ? (c - 1) % 5 : 0;
		const int32_t end = EndTick();
		const int32_t count = c >= end ? end - c + 5 : 5;
		*(volatile int32_t *)&n_ = count;
		for (int32_t k = 0; k < count; k++)
		{
			const Ghost *s = GhostSlot(slot);
			if (!s->used) break;
			BoneRestore(*(uint8_t **)(Caster() + 0x78), s->bones);
			*(uint32_t *)(Caster() + 0x28) = GHOST_COLOURS()[k];
			GhostDraw(Caster());
			if (--slot < 0) slot = 4;
		}
		BoneRestore(*(uint8_t **)(Caster() + 0x78), (const uint8_t *)game_addr(BONES_NOW));
		*(uint32_t *)(Caster() + 0x28) = colour;
		if (UpdateFlags() & 0x201) return 0;
		n->counter++;
		return n->counter >= EndTick() + 5 ? TASK_END : 0;
	}

	static uint32_t __cdecl StartTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		if (UpdateFlags() & 0x201) return 0;
		if (n->counter == 0)
		{
			BindExternalAnimSet(Caster(), game_addr(BIND_OUT), game_addr(ANIM_DATA));
			QueueChainTransformation(Caster(), 1);
			Node *f = AddTask(QAux(), game_addr(ORIG_FadeInTask));
			f->counter = 0;
			f->f0E = 8;
			f->f18 = 0x800;
			AddTask(QAux(), game_addr(ORIG_AfterimageTask))->counter = 0;
			EndTick() = 999999;
			for (uint32_t a = game_addr(GHOSTS); a < game_addr(GHOSTS_END); a += 0x84) *(int32_t *)a = 0;
		}
		n->counter++;
		if (n->counter < 7) return 0;
		Flags() |= 1;
		Ctx()->done = 1;
		return TASK_END;
	}

	static uint32_t __cdecl EndTask(TaskNode *n_)
	{
		if (UpdateFlags() & 0x201) return 0;
		if (Flags() & 4) return 0;
		Node *n = (Node *)n_;
		if (n->counter == 0)
		{
			QueueChainTransformation(Caster(), 2);
			Node *f = AddTask(QAux(), game_addr(ORIG_FadeOutTask));
			f->counter = 0;
			f->f0E = 8;
			f->f18 = 0x800;
			EndTick() = ModuleTick();
		}
		if (n->counter == 4) ApplyActionResultToTargets(Ctx()->actions[0].targets, Ctx()->actions[0].target_count);
		n->counter++;
		if (n->counter < 0xC) return 0;
		Flags() |= 2;
		Ctx()->done = 1;
		return TASK_END;
	}

	static uint32_t __cdecl RestoreTask(TaskNode *n_)
	{
		RootNode *r = (RootNode *)n_;
		if (r->counter == 1) TextureRestoreTask((void *)TexBase(), &r->done);
		const int16_t done = r->done;
		r->counter++;
		return done ? TASK_END : 0;
	}

	static uint32_t __cdecl RootTask(TaskNode *n_)
	{
		RootNode *r = (RootNode *)n_;
		PacketCursor() = (*(const uint8_t *)&r->counter & 1) ? TexBase() + ARENA_ODD : TexBase();
		const int shots = ExecuteTaskQueue(QShots());
		const int aux = ExecuteTaskQueue(QAux());
		if (!(UpdateFlags() & 0x201)) ModuleTick()++;
		if (aux == 0 && (Flags() & 2))
		{
			RootNode *t = (RootNode *)AddTaskToQueue(QRoot(), game_addr(ORIG_RestoreTask));
			t->counter = 0;
			t->done = 0;
		}
		r->counter++;
		return (aux == 0 && shots == 0 && (Flags() & 2)) ? TASK_END : 0;
	}
}

	void register_mag194_flame_shot()
	{
		register_port(flame194::ORIG_RootTask, (void *)flame194::RootTask, "194 RootTask", 194);
		register_port(flame194::ORIG_RestoreTask, (void *)flame194::RestoreTask, "194 RestoreTask", 194);
		register_port(flame194::ORIG_StartTask, (void *)flame194::StartTask, "194 StartTask", 194);
		register_port(flame194::ORIG_ShotTask, (void *)flame194::ShotTask, "194 ShotTask", 194);
		register_port(flame194::ORIG_EndTask, (void *)flame194::EndTask, "194 EndTask", 194);
		register_port(flame194::ORIG_FadeOutTask, (void *)flame194::FadeOutTask, "194 FadeOutTask", 194);
		register_port(flame194::ORIG_FadeInTask, (void *)flame194::FadeInTask, "194 FadeInTask", 194);
		register_port(flame194::ORIG_AfterimageTask, (void *)flame194::AfterimageTask, "194 AfterimageTask", 194);
		register_port(flame194::ORIG_FlashTask, (void *)flame194::FlashTask, "194 FlashTask", 194);
		register_port(flame194::ORIG_SmokeTask, (void *)flame194::SmokeTask, "194 SmokeTask", 194);
		register_port(flame194::ORIG_SpinTask, (void *)flame194::SpinTask, "194 SpinTask", 194);
		register_port(flame194::ORIG_GroundSmokeTask, (void *)flame194::GroundSmokeTask, "194 GroundSmokeTask", 194);
		register_port(flame194::ORIG_FlightTask, (void *)flame194::FlightTask, "194 FlightTask", 194);
		register_port(flame194::ORIG_HeadTask, (void *)flame194::HeadTask, "194 HeadTask", 194);
		register_port(flame194::ORIG_TrailTask, (void *)flame194::TrailTask, "194 TrailTask", 194);
		register_port(flame194::ORIG_EmberTask, (void *)flame194::EmberTask, "194 EmberTask", 194);
		register_port(flame194::ORIG_BurstTask, (void *)flame194::BurstTask, "194 BurstTask", 194);
		register_port(flame194::ORIG_RingTask, (void *)flame194::RingTask, "194 RingTask", 194);
		register_port(flame194::ORIG_PillarTask, (void *)flame194::PillarTask, "194 PillarTask", 194);
		register_port(flame194::ORIG_FireTask, (void *)flame194::FireTask, "194 FireTask", 194);
	}
}

