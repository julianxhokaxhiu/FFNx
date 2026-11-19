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

// Effect 197: Armor Shot

#include "mag_common.h"

namespace ff8fx
{
namespace armor197
{
	using namespace eng;
	using namespace magc;

	inline int32_t &EndTick() { return var<int32_t>(game_addr(game::MAG_197_ARMORSHOT_AfterimageEndTick)); }
	inline int32_t &ModuleTick() { return var<int32_t>(game_addr(game::MAG_197_ARMORSHOT_ModuleTick)); }
	inline uint32_t &Flags() { return var<uint32_t>(game_addr(game::MAG_197_ARMORSHOT_Flags)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::MAG_197_ARMORSHOT_CastContext)); }
	inline int32_t &CasterSlot() { return var<int32_t>(game_addr(game::MAG_197_ARMORSHOT_CasterSlot)); }
	inline int32_t &IdCount() { return var<int32_t>(game_addr(game::MAG_197_ARMORSHOT_ParticleIdCount)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::MAG_197_ARMORSHOT_TextureBase)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_197_ARMORSHOT_PacketCursor)); }
	inline int32_t &ArcCount() { return var<int32_t>(game_addr(game::MAG_197_ARMORSHOT_ArcCount)); }
	inline TaskQueue *QRoot() { return (TaskQueue *)game_addr(game::MAG_197_ARMORSHOT_RootQueue); }
	inline TaskQueue *QShots() { return (TaskQueue *)game_addr(game::MAG_197_ARMORSHOT_ShotQueue); }
	inline TaskQueue *QAux() { return (TaskQueue *)game_addr(game::MAG_197_ARMORSHOT_AuxQueue); }
	static constexpr game::Id PARTS_A = game::MAG_197_ARMORSHOT_ParticlesEven, PARTS_B = game::MAG_197_ARMORSHOT_ParticlesOdd;
	static const int32_t PARTS_N = 150;
	static constexpr game::Id SHOTS = game::MAG_197_ARMORSHOT_ShotRecords;
	static constexpr game::Id ARCS = game::MAG_197_ARMORSHOT_ArcRecords;
	static const int32_t ARC_POINTS = 24;
	static constexpr game::Id BIND_OUT = game::MAG_197_ARMORSHOT_AnimBindBlock;
	static constexpr game::Id BONES_NOW = game::MAG_197_ARMORSHOT_WeaponBonesNow;
	static constexpr game::Id GHOSTS = game::MAG_197_ARMORSHOT_GhostSlots, GHOSTS_END = game::MAG_197_ARMORSHOT_ShotCount;
	static const uint32_t ARENA_ODD = 0xC000;

	static constexpr game::Id ORIG_RootTask = game::MAG_197_ARMORSHOT_RootTask;
	static constexpr game::Id ORIG_RestoreTask = game::MAG_197_ARMORSHOT_TextureRestoreTask;
	static constexpr game::Id ORIG_StartTask = game::MAG_197_ARMORSHOT_StartTask;
	static constexpr game::Id ORIG_ShotTask = game::MAG_197_ARMORSHOT_ShotTask;
	static constexpr game::Id ORIG_EndTask = game::MAG_197_ARMORSHOT_EndTask;
	static constexpr game::Id ORIG_FadeOutTask = game::MAG_197_ARMORSHOT_FadeOutTask;
	static constexpr game::Id ORIG_FadeInTask = game::MAG_197_ARMORSHOT_FadeInTask;
	static constexpr game::Id ORIG_AfterimageTask = game::MAG_197_ARMORSHOT_AfterimageTask;
	static constexpr game::Id ORIG_MuzzleFlashTask = game::MAG_197_ARMORSHOT_MuzzleFlashTask;
	static constexpr game::Id ORIG_MuzzleConeTask = game::MAG_197_ARMORSHOT_MuzzleConeTask;
	static constexpr game::Id ORIG_GroundFlashTask = game::MAG_197_ARMORSHOT_GroundFlashTask;
	static constexpr game::Id ORIG_BoneSmokeTask = game::MAG_197_ARMORSHOT_BoneSmokeTask;
	static constexpr game::Id ORIG_BeamTask = game::MAG_197_ARMORSHOT_BeamTask;
	static constexpr game::Id ORIG_PathRingTask = game::MAG_197_ARMORSHOT_PathRingTask;
	static constexpr game::Id ORIG_ImpactRingTask = game::MAG_197_ARMORSHOT_ImpactRingTask;
	static constexpr game::Id ORIG_ImpactSpinTask = game::MAG_197_ARMORSHOT_ImpactSpinTask;
	static constexpr game::Id ORIG_ArmorFlashTask = game::MAG_197_ARMORSHOT_ArmorFlashTask;
	static constexpr game::Id ORIG_DebrisTask = game::MAG_197_ARMORSHOT_DebrisTask;
	static constexpr game::Id ORIG_TargetSparkTask = game::MAG_197_ARMORSHOT_TargetSparkTask;
	static constexpr game::Id ORIG_ArcTask = game::MAG_197_ARMORSHOT_ArcTask;
	static constexpr game::Id ORIG_ArcSparkTask = game::MAG_197_ARMORSHOT_ArcSparkTask;

	static constexpr game::Id ANIM_DATA = game::MAG_197_ARMORSHOT_AnimSetComData;
	static constexpr game::Id MODEL_MuzzleFlash = game::MAG_197_ARMORSHOT_ModelMuzzleFlash, MODEL_Cone = game::MAG_197_ARMORSHOT_ModelMuzzleCone, MODEL_GroundFlash = game::MAG_197_ARMORSHOT_ModelGroundFlash, MODEL_PathRing = game::MAG_197_ARMORSHOT_ModelPathRing;
	static constexpr game::Id MODEL_Beam = game::MAG_197_ARMORSHOT_ModelBeam, MODEL_BeamHead = game::MAG_197_ARMORSHOT_ModelBeamHead, MODEL_ImpactRing = game::MAG_197_ARMORSHOT_ModelImpactRing, MODEL_ImpactSpin = game::MAG_197_ARMORSHOT_ModelImpactSpin;
	static constexpr game::Id MODEL_Armor = game::MAG_197_ARMORSHOT_ModelArmorFlash;
	static constexpr game::Id SEQ_BoneSmoke = game::MAG_197_ARMORSHOT_SeqBoneSmoke, SEQ_Debris = game::MAG_197_ARMORSHOT_SeqDebris, SEQ_Spark = game::MAG_197_ARMORSHOT_SeqSpark;
	static const void *SOUND_Gun() { return (const void *)game_addr(game::MAG_197_ARMORSHOT_SoundGun); }
	static const int32_t *DIR_BACK() { return (const int32_t *)game_addr(game::MAG_197_ARMORSHOT_DirBack); }
	static const int32_t *DIR_FRONT() { return (const int32_t *)game_addr(game::MAG_197_ARMORSHOT_DirFront); }
	static const int32_t *DIR_DOWN() { return (const int32_t *)game_addr(game::MAG_197_ARMORSHOT_DirDown); }
	static const uint32_t *GHOST_COLOURS() { return (const uint32_t *)game_addr(game::MAG_197_ARMORSHOT_GhostColours); }

	inline uint32_t BindExternalAnimSet(uint8_t *entity, uint32_t out, uint32_t data) { return fn<uint32_t (__cdecl *)(uint8_t *, uint32_t, uint32_t)>(game_addr(game::BattleEntity_BindExternalAnimSet))(entity, out, data); }
	inline uint32_t QueueChainTransformation(uint8_t *entity, uint32_t id) { return fn<uint32_t (__cdecl *)(uint8_t *, uint32_t)>(game_addr(game::QueueChainTransformation))(entity, id); }
	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t angle, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, angle, out); }
	inline int32_t Normalize(const int32_t *in, int32_t *out) { return fn<int32_t (__cdecl *)(const int32_t *, int32_t *)>(game_addr(game::NormalizeVectorToFixedPoint))(in, out); }
	inline int32_t Sqrt(int32_t v) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::Math_SqrtInt))(v); }
	inline void MatVec(const Mat4x3 *m, const int16_t *in, int16_t *out) { fn<void (__cdecl *)(const Mat4x3 *, const int16_t *, int16_t *)>(game_addr(game::matrixMultiplyVector))(m, in, out); }
	inline void GteMatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline uint32_t RenderGeometry(uint32_t model, void *ctx, uint32_t ot, int32_t a, uint32_t cursor) { return fn<uint32_t (__cdecl *)(uint32_t, void *, uint32_t, int32_t, uint32_t)>(game_addr(game::RenderGeometry))(model, ctx, ot, a, cursor); }
	inline void GteSetLightMatrix(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_SetLightMatrix))(m); }
	inline void GteSetBackColorFromTrans(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_SetBackColorFromTrans))(m); }
	inline void GteMVMVA_LightV0Bk() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_LightV0_Bk))(); }
	inline void GteSetRotScale(int32_t s) { fn<void (__cdecl *)(int32_t)>(game_addr(game::GTE_SetRotScale))(s); }
	inline void GteSetTransFromVec32(const int32_t *v) { fn<void (__cdecl *)(const int32_t *)>(game_addr(game::GTE_SetTransFromVec32))(v); }
	inline void SplineSetup(int32_t n, const void *points, void *work) { fn<void (__cdecl *)(int32_t, const void *, void *)>(game_addr(game::Effect_SplineSetup))(n, points, work); }
	inline void SplineEval(int32_t n, void *work, void *out, int32_t t) { fn<void (__cdecl *)(int32_t, void *, void *, int32_t)>(game_addr(game::Effect_SplineEval))(n, work, out, t); }
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
		int16_t f20;
		int16_t f22;
	};
	struct ShotRec
	{
		int16_t pos[4];
		int32_t dir[3];
		uint8_t pad14[4];
		Mat4x3 m;
	};
	struct ArcRec
	{
		int16_t a[4];
		int16_t b[4];
		int16_t ctrl[3][4];
		int16_t pts[ARC_POINTS][4];
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
	static_assert(sizeof(RootNode) == 0x10 && sizeof(Node) == 0x24, "Armor Shot nodes");
	static_assert(sizeof(ShotRec) == 0x38 && sizeof(ArcRec) == 0xE8 && sizeof(Particle) == 0x18 && sizeof(Ghost) == 0x84, "Armor Shot records");

	inline ShotRec *Shot(int32_t i) { return (ShotRec *)(game_addr(SHOTS) + 0x38 * i); }
	inline ArcRec *Arc(int32_t i) { return (ArcRec *)(game_addr(ARCS) + 0xE8 * i); }
	inline Ghost *GhostSlot(int32_t i) { return (Ghost *)(game_addr(GHOSTS) + 0x84 * i); }
	inline uint8_t *Caster() { return Entity(CasterSlot()); }
	inline Node *AddTask(TaskQueue *q, uint32_t fn) { return (Node *)AddTaskToQueue(q, fn); }
	inline Particle *PartArray(int16_t k) { return (Particle *)((k & 1) ? game_addr(PARTS_B) : game_addr(PARTS_A)); }
	inline int32_t FindFree(const Particle *arr) { for (int32_t i = 0; i < PARTS_N; i++) if (arr[i].id == 0) return i; return -1; }
}
}

namespace ff8fx
{
namespace armor197
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

	static int32_t ClampedSize(const uint8_t *entity)
	{
		const int32_t s = *(const int16_t *)(entity + 0x26);
		if (s > 0x1100) return 0x1100;
		if (s < 0x600) return 0x600;
		return s;
	}

	static void IdentityRotation(Mat4x3 *m)
	{
		uint32_t *w = (uint32_t *)m;
		w[3] = 0;
		w[2] = 0;
		w[1] = 0;
		w[0] = 0;
		m->m[2][2] = 0x1000;
		m->m[1][1] = 0x1000;
		m->m[0][0] = 0x1000;
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

	static uint8_t *ModelHeader(uint32_t model, bool faded, int32_t fade)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		*(uint32_t *)h = model;
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0x1C) = 0x33;
		if (faded)
		{
			*(uint32_t *)(h + 0x1C) = 0xF3;
			*(int32_t *)(h + 0xC) = fade;
		}
		return h;
	}
	static void ModelDraw(uint32_t model, bool faded, int32_t fade)
	{
		uint8_t *h = ModelHeader(model, faded, fade);
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}
	static void ModelMatrix(Mat4x3 *m)
	{
		ComposeAffineTransform(&Camera(), m, m);
		GteSetRotMatrix(m);
		GteSetTransVector(m);
	}

	static int32_t FadeFrom2(int32_t c) { return (int32_t)((uint32_t)(c - 2) << 9); }
	static int32_t FadeFrom4(int32_t c) { return (int32_t)((uint32_t)(c - 4) << 9); }
	static int32_t FadeFrom6(int32_t c) { return (int32_t)((uint32_t)(c - 6) << 9); }
	static int32_t IringFade(int32_t c) { return (int32_t)((uint32_t)(c - 2) << 10); }
	static int32_t Fade2AA(int32_t c) { return mul32(c - 6, 0x2AA); }

	static void MuzzleModel(const Node *n, int16_t yaw, int16_t roll, uint32_t model, bool faded, int32_t fade)
	{
		const ShotRec *s = Shot(n->f0E);
		int16_t ang[4];
		ang[0] = 0;
		ang[1] = yaw;
		ang[2] = roll;
		ang[3] = 0;
		Mat4x3 m;
		ComposeZYXRotationMatrix(ang, &m);
		GteMatrixMultiply(&s->m, &m);
		m.t[0] = s->pos[0];
		m.t[1] = s->pos[1];
		m.t[2] = s->pos[2];
		const int32_t sc = n->f1C;
		const int32_t v[3] = { sc, sc, sc };
		Scale3DMatrix(&m, v);
		ModelMatrix(&m);
		ModelDraw(model, faded, fade);
	}

	static void GroundFlashDraw(const Node *n, bool faded, int32_t fade)
	{
		const ShotRec *s = Shot(n->f0E);
		int16_t ang[4];
		ang[0] = 0;
		ang[1] = n->f18;
		ang[2] = 0;
		ang[3] = 0;
		Mat4x3 m;
		ComposeZYXRotationMatrix(ang, &m);
		m.t[0] = s->pos[0];
		m.t[1] = 0;
		m.t[2] = s->pos[2];
		const int32_t sc = n->f1C;
		const int32_t v[3] = { sc, sc, sc };
		Scale3DMatrix(&m, v);
		ModelMatrix(&m);
		ModelDraw(game_addr(MODEL_GroundFlash), faded, fade);
	}

	static void PathRingDraw(const Node *n, bool faded, int32_t fade)
	{
		const ShotRec *s = Shot(n->f0E);
		int16_t ang[4];
		ang[0] = 0;
		ang[1] = 0;
		ang[2] = n->f18;
		ang[3] = 0;
		Mat4x3 m;
		ComposeZYXRotationMatrix(ang, &m);
		int32_t axis[3];
		const int32_t angle = RotationBetweenVectors(DIR_BACK(), s->dir, axis);
		Mat4x3 r;
		BuildAxisAngleRotationMatrix(angle, &r, axis);
		GteMatrixMultiply(&r, &m);
		m.t[1] = n->pos[1];
		m.t[0] = n->pos[0];
		m.t[2] = n->pos[2];
		const int32_t v[3] = { n->f1C, n->f1C, n->f20 };
		Scale3DMatrix(&m, v);
		ModelMatrix(&m);
		ModelDraw(game_addr(MODEL_PathRing), faded, fade);
	}

	static int16_t BeamDraw(const Node *n, bool faded, int32_t fade, int32_t head_size)
	{
		const ShotRec *s = Shot(n->f0E);
		int16_t ang[4];
		ang[0] = 0;
		ang[1] = 0;
		ang[2] = 0;
		ang[3] = 0;
		Mat4x3 m;
		ComposeZYXRotationMatrix(ang, &m);
		int32_t axis[3];
		const int32_t angle = RotationBetweenVectors(DIR_BACK(), s->dir, axis);
		Mat4x3 r;
		BuildAxisAngleRotationMatrix(angle, &r, axis);
		GteMatrixMultiply(&r, &m);
		m.t[0] = s->pos[0];
		m.t[1] = s->pos[1];
		m.t[2] = s->pos[2];
		const int32_t v[3] = { n->f1C, n->f1C, ((int32_t)n->f20 << 12) / 6000 };
		Scale3DMatrix(&m, v);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		uint8_t *h = ModelHeader(game_addr(MODEL_Beam), faded, fade);
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
		int16_t p[4];
		p[0] = (int16_t)((mul32(s->dir[0], n->f20) >> 12) + s->pos[0]);
		p[1] = (int16_t)((mul32(s->dir[1], n->f20) >> 12) + s->pos[1]);
		p[2] = (int16_t)((mul32(s->dir[2], n->f20) >> 12) + s->pos[2]);
		p[3] = (int16_t)(head_size < 0 ? CrtRand() % 0x300 + 0x800 : head_size);
		TransformCameraByShadowRotation(p, p[3], -((int32_t)p[3] >> 3));
		*(uint32_t *)h = game_addr(MODEL_BeamHead);
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
		return p[3];
	}

	static void ImpactModelDraw(const Node *n, const int32_t *ref, uint32_t model, bool faded, int32_t fade)
	{
		const ShotRec *s = Shot(n->f0E);
		int16_t ang[4];
		ang[0] = 0;
		ang[1] = 0;
		ang[2] = n->f18;
		ang[3] = 0;
		Mat4x3 m;
		ComposeZYXRotationMatrix(ang, &m);
		int32_t axis[3];
		const int32_t angle = RotationBetweenVectors(ref, s->dir, axis);
		Mat4x3 r;
		BuildAxisAngleRotationMatrix(angle, &r, axis);
		GteMatrixMultiply(&r, &m);
		const int32_t sc = n->f1C;
		const int32_t v[3] = { sc, sc, sc };
		Scale3DMatrix(&m, v);
		TransformCameraByShadowRotation(n->pos, n->f1C, -(int32_t)n->pos[3]);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		ModelDraw(model, faded, fade);
	}

	static void ArmorFlashDraw(const Node *n, bool faded, int32_t fade)
	{
		int16_t ang[4];
		ang[0] = 0;
		ang[1] = n->f18;
		ang[2] = 0;
		ang[3] = 0;
		Mat4x3 m;
		ComposeZYXRotationMatrix(ang, &m);
		const int32_t sc = n->f1C;
		const int32_t v[3] = { sc, sc, sc };
		Scale3DMatrix(&m, v);
		TransformCameraByShadowRotation(n->pos, n->f1C, -(int32_t)n->pos[3]);
		ModelDraw(game_addr(MODEL_Armor), faded, fade);
	}

	static void SparkBegin(uint8_t *s, uint8_t *h, uint32_t seq)
	{
		*(int16_t *)(h + 0x24) = 0;
		ff8fx::copy<(0x20)>(s + 8, &Camera());
		*(uint32_t *)h = seq;
		GteSetLightMatrix(s + 8);
		GteSetBackColorFromTrans(s + 8);
	}
	static void SparkDraw(const Particle *e, int32_t pull, uint8_t *s, uint8_t *h)
	{
		GteLoadV0(e->pos);
		GteMVMVA_LightV0Bk();
		GteSetRotScale(e->size);
		*(int16_t *)(h + 4) = e->frame;
		int32_t *v = (int32_t *)(s + 0x38), *d = (int32_t *)(s + 0x28);
		GteReadMAC123(v);
		Normalize(v, d);
		const int32_t k = -pull;
		v[0] += mul32(d[0], k) >> 12;
		v[1] += mul32(d[1], k) >> 12;
		v[2] += mul32(d[2], k) >> 12;
		GteSetTransFromVec32(v);
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
	}
	static int32_t BoneSmokePull(const Particle *e) { return (int32_t)e->size >> 4; }

	static void DebrisBegin(const Node *n, uint8_t *s, uint8_t *h)
	{
		*(uint32_t *)h = game_addr(SEQ_Debris);
		*(int16_t *)(h + 0x24) = 0x208;
		int16_t *ang = (int16_t *)s;
		ang[0] = 0;
		ang[1] = 0;
		ang[2] = 0;
		Mat4x3 *m0 = (Mat4x3 *)(s + 8);
		ComposeZYXRotationMatrix(ang, m0);
		m0->t[0] = n->pos[0];
		m0->t[1] = n->pos[1];
		m0->t[2] = n->pos[2];
		ComposeAffineTransform(&Camera(), m0, m0);
		GteSetRotMatrix(m0);
		GteSetTransVector(m0);
		*(int32_t *)(s + 0x70) = 0x1000;
		*(int32_t *)(s + 0x68) = 0x1000;
	}
	static void DebrisDraw(const Particle *e, uint8_t *s, uint8_t *h)
	{
		Mat4x3 *m = (Mat4x3 *)(s + 0x28);
		IdentityRotation(m);
		m->t[0] = e->pos[0];
		m->t[1] = e->pos[1];
		m->t[2] = e->pos[2];
		*(int32_t *)(s + 0x6C) = e->size;
		Scale3DMatrix(m, (const int32_t *)(s + 0x68));
		int32_t *v = (int32_t *)(s + 0x78);
		v[0] = e->vel[0];
		v[1] = e->vel[1];
		v[2] = e->vel[2];
		Normalize(v, v);
		const int32_t angle = RotationBetweenVectors(DIR_DOWN(), v, (int32_t *)(s + 0x88));
		BuildAxisAngleRotationMatrix(angle, (Mat4x3 *)(s + 0x48), (const int32_t *)(s + 0x88));
		GteMatrixMultiply((const Mat4x3 *)(s + 0x48), m);
		ComposeAffineTransform((const Mat4x3 *)(s + 8), m, m);
		GteSetRotMatrix(m);
		GteSetTransVector(m);
		*(int16_t *)(h + 4) = e->frame;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
	}
	static void DebrisMove(Particle *e)
	{
		e->pos[0] = (int16_t)(e->pos[0] + e->vel[0]);
		e->pos[1] = (int16_t)(e->pos[1] + e->vel[1]);
		e->pos[2] = (int16_t)(e->pos[2] + e->vel[2]);
		e->vel[0] = (int16_t)(e->vel[0] - (int16_t)(e->vel[0] >> 2));
		e->vel[1] = (int16_t)(e->vel[1] - (int16_t)(e->vel[1] >> 2));
		e->vel[2] = (int16_t)(e->vel[2] - (int16_t)(e->vel[2] >> 2));
	}

	static void ArcSparkBegin(uint8_t *h)
	{
		*(uint32_t *)h = game_addr(SEQ_Spark);
		*(int16_t *)(h + 0x24) = 0;
	}
	static void ArcSparkDraw(const Particle *e, uint8_t *h)
	{
		*(int16_t *)(h + 4) = (int16_t)(e->frame * 2 + 1);
		TransformCameraByShadowRotation(e->pos, e->size, -((int32_t)e->size >> 4));
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
	}

	static uint32_t __cdecl MuzzleFlashTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		MuzzleModel(n, n->f18, 0, game_addr(MODEL_MuzzleFlash), n->counter >= 2, FadeFrom2(n->counter));
		if (UpdateFlags() & 0x201) return 0;
		const int16_t step = n->f1E;
		n->f1C = (int16_t)(n->f1C + step);
		n->f1E = (int16_t)(step - (int16_t)(step / 3));
		n->counter++;
		return n->counter >= 10 ? TASK_END : 0;
	}

	static uint32_t __cdecl MuzzleConeTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		MuzzleModel(n, 0, n->f18, game_addr(MODEL_Cone), n->counter >= 6, FadeFrom6(n->counter));
		if (UpdateFlags() & 0x201) return 0;
		const int16_t step = n->f1E;
		n->f1C = (int16_t)(n->f1C + step);
		n->f1E = (int16_t)(step - (int16_t)(step / 5));
		n->counter++;
		return n->counter >= 14 ? TASK_END : 0;
	}

	static uint32_t __cdecl GroundFlashTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		GroundFlashDraw(n, n->counter >= 2, FadeFrom2(n->counter));
		if (UpdateFlags() & 0x201) return 0;
		n->counter++;
		return n->counter >= 10 ? TASK_END : 0;
	}

	static uint32_t __cdecl BoneSmokeTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		Particle *arr = PartArray(n->f0E);
		uint8_t *s = (uint8_t *)FieldAlloc(0x48);
		const int32_t id = n->f0E + 1;
		if (!(UpdateFlags() & 0x201) && n->counter <= 3)
		{
			for (int32_t j = 0; j < 2; j++)
			{
				const int32_t i = FindFree(arr);
				if (i < 0) break;
				Particle *e = &arr[i];
				e->id = id;
				e->frame = 0;
				e->size = (int16_t)(CrtRand() % 0x280 + 0x320);
				GetEffectSpawnPosition(Caster(), 0x1D, 0xC00, e->pos);
				e->pos[0] = (int16_t)(e->pos[0] + (CrtRand() % 0x32 - 0x19));
				e->pos[1] = (int16_t)(e->pos[1] + (CrtRand() % 0x32 - 0x19));
				e->pos[2] = (int16_t)(e->pos[2] + (CrtRand() % 0x32 - 0x19));
				Particle *e2 = (Particle *)((uint32_t)arr + 0x18 * (uint32_t)(i + 1));
				e2->id = id;
				e2->frame = 0;
				e2->size = (int16_t)(CrtRand() % 0x280 + 0x320);
				GetEffectSpawnPosition(Caster(), 0x1E, 0xC00, e2->pos);
				e2->pos[0] = (int16_t)(e2->pos[0] + (CrtRand() % 0x32 - 0x19));
				e2->pos[1] = (int16_t)(e2->pos[1] + (CrtRand() % 0x32 - 0x19));
				e2->pos[2] = (int16_t)(e2->pos[2] + (CrtRand() % 0x32 - 0x19));
			}
		}
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		int32_t alive = 0;
		SparkBegin(s, h, game_addr(SEQ_BoneSmoke));
		for (Particle *e = arr; e < arr + PARTS_N; e++)
		{
			if (e->id != id) continue;
			SparkDraw(e, BoneSmokePull(e), s, h);
			if (UpdateFlags() & 0x201) continue;
			e->frame++;
			if (*(const int16_t *)(h + 0x28) < 0)
			{
				e->id = 0;
				continue;
			}
			e->size = (int16_t)(e->size + (int16_t)(e->size >> 4));
			alive++;
		}
		FieldFree(0xB4);
		FieldFree(0x48);
		if (UpdateFlags() & 0x201) return 0;
		n->counter++;
		return alive ? 0 : TASK_END;
	}

	static uint32_t __cdecl BeamTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		BeamDraw(n, n->counter >= 4, FadeFrom4(n->counter), -1);
		if (UpdateFlags() & 0x201) return 0;
		if (n->counter < 4) n->f20 = (int16_t)(n->f20 + n->f22);
		n->counter++;
		return n->counter >= 12 ? TASK_END : 0;
	}

	static uint32_t __cdecl PathRingTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		if (n->pos[3] > 0)
		{
			if (UpdateFlags() & 0x201) return 0;
			n->pos[3]--;
			return 0;
		}
		PathRingDraw(n, n->counter >= 4, FadeFrom4(n->counter));
		if (UpdateFlags() & 0x201) return 0;
		const int16_t spin = n->f1A;
		n->f18 = (int16_t)(n->f18 + spin);
		n->f1A = (int16_t)(spin - (int16_t)(spin / 10));
		const int16_t step = n->f1E;
		n->f1C = (int16_t)(n->f1C + step);
		n->f1E = (int16_t)(step - (int16_t)(step / 4));
		n->counter++;
		return n->counter >= 12 ? TASK_END : 0;
	}

	static uint32_t __cdecl ImpactRingTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		ImpactModelDraw(n, DIR_FRONT(), game_addr(MODEL_ImpactRing), n->counter >= 2, IringFade(n->counter));
		if (UpdateFlags() & 0x201) return 0;
		const int16_t step = n->f1E;
		n->f1C = (int16_t)(n->f1C + step);
		n->f1E = (int16_t)(step - (int16_t)(step / 4));
		n->counter++;
		return n->counter >= 6 ? TASK_END : 0;
	}

	static uint32_t __cdecl ImpactSpinTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		ImpactModelDraw(n, DIR_BACK(), game_addr(MODEL_ImpactSpin), n->counter >= 6, Fade2AA(n->counter));
		if (UpdateFlags() & 0x201) return 0;
		const int16_t spin = n->f1A;
		n->f18 = (int16_t)(n->f18 + spin);
		n->f1A = (int16_t)(spin - (int16_t)(spin / 16));
		const int16_t step = n->f1E;
		n->f1C = (int16_t)(n->f1C + step);
		n->f1E = (int16_t)(step - (int16_t)(step / 4));
		n->counter++;
		return n->counter >= 12 ? TASK_END : 0;
	}

	static uint32_t __cdecl ArmorFlashTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		ArmorFlashDraw(n, n->counter >= 6, Fade2AA(n->counter));
		if (UpdateFlags() & 0x201) return 0;
		const int16_t step = n->f1E;
		n->f1C = (int16_t)(n->f1C + step);
		n->f1E = (int16_t)(step - (int16_t)(step / 3));
		n->counter++;
		return n->counter >= 12 ? TASK_END : 0;
	}

	static uint32_t __cdecl DebrisTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		Particle *arr = PartArray(n->f0E);
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		uint8_t *s = (uint8_t *)FieldAlloc(0x98);
		int32_t alive = 0;
		DebrisBegin(n, s, h);
		const int32_t id = n->f0E + 1;
		for (Particle *e = arr; e < arr + PARTS_N; e++)
		{
			if (e->id != id) continue;
			DebrisDraw(e, s, h);
			if (UpdateFlags() & 0x201) continue;
			e->frame++;
			if (*(const int16_t *)(h + 0x28) < 0)
			{
				e->id = 0;
				continue;
			}
			DebrisMove(e);
			alive++;
		}
		FieldFree(0x98);
		FieldFree(0xB4);
		if (UpdateFlags() & 0x201) return 0;
		uint8_t *t = (uint8_t *)FieldAlloc(0x98);
		if (n->counter >= 0 && n->counter <= 2)
		{
			for (int32_t k = 0; k < 8; k++)
			{
				const int32_t i = FindFree(arr);
				if (i < 0) break;
				Particle *e = &arr[i];
				e->id = id;
				e->frame = 0;
				e->size = (int16_t)(CrtRand() % 0x1000 + 0x1400);
				int32_t *v = (int32_t *)(t + 0x68);
				v[0] = CrtRand() % 0x1000 - 0x800;
				v[1] = CrtRand() % 0x1000 - 0x800;
				v[2] = CrtRand() % 0x1000 - 0x800;
				Normalize(v, v);
				const int32_t r = CrtRand() % 0x190 + 0x12C;
				e->pos[0] = (int16_t)(mul32(r, v[0]) >> 12);
				e->pos[1] = (int16_t)(mul32(v[1], r) >> 12);
				e->pos[2] = (int16_t)(mul32(v[2], r) >> 12);
				const int32_t sp = CrtRand() % 0x104 + 0xDC;
				e->vel[0] = (int16_t)(mul32(sp, v[0]) >> 12);
				e->vel[1] = (int16_t)(mul32(v[1], sp) >> 12);
				e->vel[2] = (int16_t)(mul32(v[2], sp) >> 12);
			}
		}
		FieldFree(0x98);
		n->counter++;
		return (n->counter >= 4 && alive == 0) ? TASK_END : 0;
	}

	static uint32_t __cdecl TargetSparkTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		Particle *arr = PartArray(n->f0E);
		uint8_t *s = (uint8_t *)FieldAlloc(0x48);
		const int32_t id = n->f0E + 1;
		if (!(UpdateFlags() & 0x201) && n->counter <= 2)
		{
			const int32_t anchors = ***(const uint8_t ***)(Entity(n->f1A) + 0x64);
			for (int32_t j = 0; j < 3; j++)
			{
				const int32_t i = FindFree(arr);
				if (i < 0) break;
				Particle *e = &arr[i];
				e->id = id;
				e->frame = 0;
				e->size = (int16_t)(CrtRand() % 0xA00 + 0x580);
				const int32_t bone = anchors ? CrtRand() % anchors : 0;
				GetEffectSpawnPosition(Entity(n->f1A), bone, CrtRand() % 0x1000, e->pos);
				e->pos[0] = (int16_t)(e->pos[0] + (CrtRand() % 1000 - 500));
				e->pos[1] = (int16_t)(e->pos[1] + (CrtRand() % 1000 - 500));
				e->pos[2] = (int16_t)(e->pos[2] + (CrtRand() % 1000 - 500));
			}
		}
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		int32_t alive = 0;
		SparkBegin(s, h, game_addr(SEQ_Spark));
		for (Particle *e = arr; e < arr + PARTS_N; e++)
		{
			if (e->id != id) continue;
			SparkDraw(e, n->pos[3], s, h);
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

	static uint32_t __cdecl ArcTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		ArcRec *a = Arc(n->f18);
		if (UpdateFlags() & 0x201) return 0;
		if (n->counter == 0)
		{
			uint8_t *t = Entity(n->f1A);
			const int32_t anchors = ***(const uint8_t ***)(t + 0x64);
			const int32_t bone = anchors ? CrtRand() % anchors : 0;
			GetEffectSpawnPosition(Entity(n->f1A), bone, CrtRand() % 0x1000, a->a);
			int32_t d[3];
			d[0] = a->a[0] - n->pos[0];
			d[1] = a->a[1] - n->pos[1];
			d[2] = a->a[2] - n->pos[2];
			Normalize(d, d);
			const int32_t r = CrtRand() % 0x9C4 + 0x5DC;
			a->b[0] = (int16_t)((mul32(d[0], r) >> 12) + a->a[0]);
			a->b[1] = (int16_t)(a->a[1] - (int16_t)((CrtRand() % 0x4B0 + 0x1F4) >> 1));
			a->b[2] = (int16_t)((mul32(d[2], r) >> 12) + a->a[2]);
			ff8fx::copy<(8)>(a->ctrl[0], a->a);
			a->ctrl[1][0] = (int16_t)(((int32_t)a->a[0] + a->b[0]) >> 1);
			const int32_t up = CrtRand() % 0x4B0;
			a->ctrl[1][1] = (int16_t)(((((int32_t)a->b[1] + a->a[1]) >> 1) - up) - 0x1F4);
			a->ctrl[1][2] = (int16_t)(((int32_t)a->b[2] + a->a[2]) >> 1);
			ff8fx::copy<(8)>(a->ctrl[2], a->b);
			void *work = FieldAlloc(0x190);
			SplineSetup(3, a->ctrl, work);
			for (int32_t i = 0; i < ARC_POINTS; i++) SplineEval(3, work, a->pts[i], (i * 0x1000) / 23);
			FieldFree(0x190);
		}
		if (n->counter == 0)
		{
			Node *k = AddTask(QAux(), game_addr(ORIG_ArcSparkTask));
			k->f0E = n->f18;
			const int32_t num = IdCount();
			k->pos[3] = (int16_t)num;
			IdCount() = num + 1;
			k->counter = 0;
			k->f18 = 0;
			k->f1A = 5;
		}
		n->counter++;
		return n->counter >= 10 ? TASK_END : 0;
	}

	static uint32_t __cdecl ArcSparkTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		*(volatile uint32_t *)&n_ = (uint32_t)PartArray(n->pos[3]);
		const ArcRec *a = Arc(n->f0E);
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		const int32_t id = n->pos[3] + 1;
		ArcSparkBegin(h);
		int32_t alive = 0;
		Particle *arr = (Particle *)*(volatile uint32_t *)&n_;
		for (Particle *e = arr; e < arr + PARTS_N; e++)
		{
			if (e->id != id) continue;
			ArcSparkDraw(e, h);
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
		if (UpdateFlags() & 0x201) return 0;
		const int16_t speed = n->f1A;
		n->f18 = (int16_t)(n->f18 + speed);
		if (speed > 2) n->f1A = (int16_t)(speed - 1);
		if (n->f18 < ARC_POINTS)
		{
			Particle *ar = (Particle *)*(volatile uint32_t *)&n_;
			for (int32_t p = n->f18; p >= n->f18 - n->f1A; p--)
			{
				const int32_t i = FindFree(ar);
				if (i < 0) break;
				Particle *e = &ar[i];
				e->id = id;
				e->frame = 0;
				e->size = (int16_t)(CrtRand() % 0x200 + 0x480);
				ff8fx::copy<(8)>(e->pos, a->pts[p]);
			}
		}
		n->counter++;
		return (n->counter >= 4 && alive == 0) ? TASK_END : 0;
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
		if (n->counter == 0x16)
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
			const int32_t angle = RotationBetweenVectors(DIR_BACK(), dir, axis);
			BuildAxisAngleRotationMatrix(angle, &s->m, axis);
			GetDefaultEffectPosition(Entity(n->f1A), n->pos);
			if (n->pos[1] > s->pos[1]) n->pos[1] = s->pos[1];
			s->dir[0] = n->pos[0] - s->pos[0];
			s->dir[1] = n->pos[1] - s->pos[1];
			s->dir[2] = n->pos[2] - s->pos[2];
			*(volatile int32_t *)&n_ = Sqrt(Normalize(s->dir, s->dir));
			uint8_t *t = Entity(n->f1A);
			n->pos[3] = (int16_t)(mul32(*(const int16_t *)(t + 0x26), 2000) >> 12);
			n->f1C = (int16_t)(mul32(ClampedSize(t), 2000) >> 12);
			Node *f = AddTask(QAux(), game_addr(ORIG_MuzzleFlashTask));
			f->counter = 0;
			f->f0E = n->f0E;
			f->f18 = (int16_t)(CrtRand() % 0x1000);
			const int16_t third = (int16_t)((CrtRand() % 0x380 + 0xA80) / 3);
			f->f1E = third;
			f->f1C = third;
			Node *c = AddTask(QAux(), game_addr(ORIG_MuzzleConeTask));
			c->counter = 0;
			c->f0E = n->f0E;
			c->f18 = (int16_t)(CrtRand() % 0x1000);
			const int16_t third2 = (int16_t)((CrtRand() % 0x200 + 0xA00) / 3);
			c->f1E = third2;
			c->f1C = third2;
			Node *g = AddTask(QAux(), game_addr(ORIG_GroundFlashTask));
			g->counter = 0;
			g->f0E = n->f0E;
			g->f18 = (int16_t)(CrtRand() % 0x1000);
			g->f1C = (int16_t)(CrtRand() % 0x480 + 0x980);
			Node *b = AddTask(QAux(), game_addr(ORIG_BoneSmokeTask));
			const int32_t num = IdCount();
			b->counter = 0;
			b->f0E = (int16_t)num;
			IdCount() = num + 1;
			const int32_t sixth = *(volatile int32_t *)&n_ / 6;
			for (int32_t k = 1, frac = 0x200; frac <= 0xA00; k++, frac += 0x200)
			{
				Node *r = AddTask(QAux(), game_addr(ORIG_PathRingTask));
				ff8fx::copy<(8)>(r->pos, s->pos);
				r->f0E = n->f0E;
				r->pos[0] = (int16_t)(r->pos[0] + (mul32(mul32(k, sixth), s->dir[0]) >> 12));
				r->pos[1] = (int16_t)(r->pos[1] + (mul32(mul32(s->dir[1], k), sixth) >> 12));
				r->pos[2] = (int16_t)(r->pos[2] + (mul32(mul32(s->dir[2], k), sixth) >> 12));
				r->counter = 0;
				r->pos[3] = (int16_t)(k - 1);
				r->f18 = (int16_t)(CrtRand() % 0x1000);
				r->f1A = (int16_t)(CrtRand() % 0xDC + 0x78);
				const int32_t shrink = 0x1000 - ComputeSin(frac / 5);
				const int16_t sc = (int16_t)((mul32(CrtRand() % 0x380 + 0xE00, shrink) >> 12) / 3);
				r->f1E = sc;
				r->f1C = sc;
				r->f20 = (int16_t)(CrtRand() % 0x600 + 0x1100);
			}
			Node *m = AddTask(QAux(), game_addr(ORIG_BeamTask));
			m->f0E = n->f0E;
			m->counter = 0;
			const int16_t half = (int16_t)(*(volatile int32_t *)&n_ / 2);
			m->f1C = 0x480;
			m->f22 = half;
			m->f20 = half;
		}
		if (n->counter == 1)
		{
			Node *r = AddTask(QAux(), game_addr(ORIG_ImpactRingTask));
			ff8fx::copy<(8)>(r->pos, n->pos);
			r->counter = 0;
			r->f0E = n->f0E;
			r->f18 = (int16_t)(CrtRand() % 0x1000);
			*(volatile int32_t *)&n_ = CrtRand() % 0x280 + 0x500;
			const int16_t third = (int16_t)(*(volatile int32_t *)&n_ / 3);
			r->f1E = third;
			r->f1C = third;
			Node *p = AddTask(QAux(), game_addr(ORIG_ImpactSpinTask));
			ff8fx::copy<(8)>(p->pos, r->pos);
			p->counter = 0;
			p->f0E = n->f0E;
			p->f18 = (int16_t)(CrtRand() % 0x1000);
			p->f1A = (int16_t)(CrtRand() % 0x104 + 0xA0);
			const int16_t quarter = (int16_t)((*(volatile int32_t *)&n_ + 0x300) / 4);
			p->f1E = quarter;
			p->f1C = quarter;
		}
		if (n->counter == 6)
		{
			Node *f = AddTask(QAux(), game_addr(ORIG_ArmorFlashTask));
			ff8fx::copy<(8)>(f->pos, n->pos);
			f->counter = 0;
			f->f0E = n->f0E;
			f->f18 = (int16_t)(CrtRand() % 0x1000);
			const int32_t k = ((int32_t)n->f1C << 12) / 2400;
			const int16_t third = (int16_t)((mul32(CrtRand() % 0x480 + 0xA00, k) >> 12) / 3);
			f->f1E = third;
			f->f1C = third;
			Node *d = AddTask(QAux(), game_addr(ORIG_DebrisTask));
			const int32_t num = IdCount();
			d->f0E = (int16_t)num;
			IdCount() = num + 1;
			ff8fx::copy<(4)>(d->pos, n->pos);
			d->counter = 0;
			ff8fx::copy<(4)>(d->pos + 2, n->pos + 2);
			Node *sp = AddTask(QAux(), game_addr(ORIG_TargetSparkTask));
			const int32_t num2 = IdCount();
			sp->f0E = (int16_t)num2;
			IdCount() = num2 + 1;
			sp->counter = 0;
			sp->pos[3] = n->pos[3];
			sp->f1A = n->f1A;
			for (int32_t j = 0; j < 3; j++)
			{
				Node *a = AddTask(QAux(), game_addr(ORIG_ArcTask));
				ff8fx::copy<(8)>(a->pos, n->pos);
				const int32_t arc = ArcCount();
				a->counter = 0;
				a->f18 = (int16_t)(arc % 8);
				ArcCount() = arc + 1;
				a->f1A = n->f1A;
			}
		}
		if (n->counter == 6) ApplyActionResultToTargets(Ctx()->actions[0].targets, Ctx()->actions[0].target_count);
		if (n->counter == 1) BdPlaySE(SOUND_Gun(), (n->f0E & 1) ? 1 : 2, 0x80);
		n->counter++;
		return n->counter >= 25 ? TASK_END : 0;
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
		if (n->counter == 6) ApplyActionResultToTargets(Ctx()->actions[0].targets, Ctx()->actions[0].target_count);
		n->counter++;
		if (n->counter < 0xE) return 0;
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

	void register_mag197_armor_shot()
	{
		register_port(armor197::ORIG_RootTask, (void *)armor197::RootTask, "197 RootTask", 197);
		register_port(armor197::ORIG_RestoreTask, (void *)armor197::RestoreTask, "197 RestoreTask", 197);
		register_port(armor197::ORIG_StartTask, (void *)armor197::StartTask, "197 StartTask", 197);
		register_port(armor197::ORIG_ShotTask, (void *)armor197::ShotTask, "197 ShotTask", 197);
		register_port(armor197::ORIG_EndTask, (void *)armor197::EndTask, "197 EndTask", 197);
		register_port(armor197::ORIG_FadeOutTask, (void *)armor197::FadeOutTask, "197 FadeOutTask", 197);
		register_port(armor197::ORIG_FadeInTask, (void *)armor197::FadeInTask, "197 FadeInTask", 197);
		register_port(armor197::ORIG_AfterimageTask, (void *)armor197::AfterimageTask, "197 AfterimageTask", 197);
		register_port(armor197::ORIG_MuzzleFlashTask, (void *)armor197::MuzzleFlashTask, "197 MuzzleFlashTask", 197);
		register_port(armor197::ORIG_MuzzleConeTask, (void *)armor197::MuzzleConeTask, "197 MuzzleConeTask", 197);
		register_port(armor197::ORIG_GroundFlashTask, (void *)armor197::GroundFlashTask, "197 GroundFlashTask", 197);
		register_port(armor197::ORIG_BoneSmokeTask, (void *)armor197::BoneSmokeTask, "197 BoneSmokeTask", 197);
		register_port(armor197::ORIG_BeamTask, (void *)armor197::BeamTask, "197 BeamTask", 197);
		register_port(armor197::ORIG_PathRingTask, (void *)armor197::PathRingTask, "197 PathRingTask", 197);
		register_port(armor197::ORIG_ImpactRingTask, (void *)armor197::ImpactRingTask, "197 ImpactRingTask", 197);
		register_port(armor197::ORIG_ImpactSpinTask, (void *)armor197::ImpactSpinTask, "197 ImpactSpinTask", 197);
		register_port(armor197::ORIG_ArmorFlashTask, (void *)armor197::ArmorFlashTask, "197 ArmorFlashTask", 197);
		register_port(armor197::ORIG_DebrisTask, (void *)armor197::DebrisTask, "197 DebrisTask", 197);
		register_port(armor197::ORIG_TargetSparkTask, (void *)armor197::TargetSparkTask, "197 TargetSparkTask", 197);
		register_port(armor197::ORIG_ArcTask, (void *)armor197::ArcTask, "197 ArcTask", 197);
		register_port(armor197::ORIG_ArcSparkTask, (void *)armor197::ArcSparkTask, "197 ArcSparkTask", 197);
	}
}

