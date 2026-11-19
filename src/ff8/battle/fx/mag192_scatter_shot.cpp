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

// Effect 192: Scatter Shot

#include "mag_common.h"

namespace ff8fx
{
namespace scatter192
{
	using namespace eng;
	using namespace magc;

	inline int32_t &EndTick() { return var<int32_t>(game_addr(game::MAG_192_SCATTERSHOT_AfterimageEndTick)); }
	inline int32_t &ModuleTick() { return var<int32_t>(game_addr(game::MAG_192_SCATTERSHOT_ModuleTick)); }
	inline uint32_t &Flags() { return var<uint32_t>(game_addr(game::MAG_192_SCATTERSHOT_Flags)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::MAG_192_SCATTERSHOT_CastContext)); }
	inline int32_t &CasterSlot() { return var<int32_t>(game_addr(game::MAG_192_SCATTERSHOT_CasterSlot)); }
	inline int32_t &ImpactCount() { return var<int32_t>(game_addr(game::MAG_192_SCATTERSHOT_ImpactCount)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::MAG_192_SCATTERSHOT_TextureBase)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_192_SCATTERSHOT_PacketCursor)); }
	inline TaskQueue *QRoot() { return (TaskQueue *)game_addr(game::MAG_192_SCATTERSHOT_RootQueue); }
	inline TaskQueue *QShots() { return (TaskQueue *)game_addr(game::MAG_192_SCATTERSHOT_ShotQueue); }
	inline TaskQueue *QAux() { return (TaskQueue *)game_addr(game::MAG_192_SCATTERSHOT_AuxQueue); }
	static constexpr game::Id PARTS_A = game::MAG_192_SCATTERSHOT_ParticlesEven, PARTS_B = game::MAG_192_SCATTERSHOT_ParticlesOdd;
	static const int32_t PARTS_N = 300;
	static constexpr game::Id SHOTS = game::MAG_192_SCATTERSHOT_ShotRecords;
	static constexpr game::Id BIND_OUT = game::MAG_192_SCATTERSHOT_AnimBindBlock;
	static constexpr game::Id BONES_NOW = game::MAG_192_SCATTERSHOT_WeaponBonesNow;
	static constexpr game::Id GHOSTS = game::MAG_192_SCATTERSHOT_GhostSlots, GHOSTS_END = game::MAG_192_SCATTERSHOT_ShotCount;
	static const uint32_t ARENA_ODD = 0x10000;

	static constexpr game::Id ORIG_RootTask = game::MAG_192_SCATTERSHOT_RootTask;
	static constexpr game::Id ORIG_RestoreTask = game::MAG_192_SCATTERSHOT_TextureRestoreTask;
	static constexpr game::Id ORIG_StartTask = game::MAG_192_SCATTERSHOT_StartTask;
	static constexpr game::Id ORIG_ShotTask = game::MAG_192_SCATTERSHOT_ShotTask;
	static constexpr game::Id ORIG_EndTask = game::MAG_192_SCATTERSHOT_EndTask;
	static constexpr game::Id ORIG_FadeOutTask = game::MAG_192_SCATTERSHOT_FadeOutTask;
	static constexpr game::Id ORIG_FadeInTask = game::MAG_192_SCATTERSHOT_FadeInTask;
	static constexpr game::Id ORIG_AfterimageTask = game::MAG_192_SCATTERSHOT_AfterimageTask;
	static constexpr game::Id ORIG_FlashTask = game::MAG_192_SCATTERSHOT_FlashTask;
	static constexpr game::Id ORIG_Flash2Task = game::MAG_192_SCATTERSHOT_Flash2Task;
	static constexpr game::Id ORIG_SmokeTask = game::MAG_192_SCATTERSHOT_SmokeTask;
	static constexpr game::Id ORIG_BlastTask = game::MAG_192_SCATTERSHOT_BlastTask;
	static constexpr game::Id ORIG_GroundSmokeTask = game::MAG_192_SCATTERSHOT_GroundSmokeTask;
	static constexpr game::Id ORIG_ImpactTask = game::MAG_192_SCATTERSHOT_ImpactTask;
	static constexpr game::Id ORIG_MissTask = game::MAG_192_SCATTERSHOT_MissTask;
	static constexpr game::Id ORIG_MissRingTask = game::MAG_192_SCATTERSHOT_MissRingTask;
	static constexpr game::Id ORIG_MissDustTask = game::MAG_192_SCATTERSHOT_MissDustTask;

	static constexpr game::Id ANIM_DATA = game::MAG_192_SCATTERSHOT_AnimSetComData;
	static constexpr game::Id MODEL_Flash = game::MAG_192_SCATTERSHOT_ModelFlash, MODEL_Flash2 = game::MAG_192_SCATTERSHOT_ModelFlash2, MODEL_Blast = game::MAG_192_SCATTERSHOT_ModelBlast, MODEL_Ring = game::MAG_192_SCATTERSHOT_ModelMissRing;
	static constexpr game::Id SEQ_Smoke = game::MAG_192_SCATTERSHOT_SeqSmoke, SEQ_GroundSmoke = game::MAG_192_SCATTERSHOT_SeqGroundSmoke, SEQ_Impact = game::MAG_192_SCATTERSHOT_SeqImpact, SEQ_Spark = game::MAG_192_SCATTERSHOT_SeqSpark, SEQ_Dust = game::MAG_192_SCATTERSHOT_SeqMissDust;
	static const void *SOUND_Gun() { return (const void *)game_addr(game::MAG_192_SCATTERSHOT_SoundGun); }
	static const int32_t *MUZZLE_REF() { return (const int32_t *)game_addr(game::MAG_192_SCATTERSHOT_MuzzleAxis); }
	static const uint32_t *GHOST_COLOURS() { return (const uint32_t *)game_addr(game::MAG_192_SCATTERSHOT_GhostColours); }

	inline uint32_t BindExternalAnimSet(uint8_t *entity, uint32_t out, uint32_t data) { return fn<uint32_t (__cdecl *)(uint8_t *, uint32_t, uint32_t)>(game_addr(game::BattleEntity_BindExternalAnimSet))(entity, out, data); }
	inline uint32_t QueueChainTransformation(uint8_t *entity, uint32_t id) { return fn<uint32_t (__cdecl *)(uint8_t *, uint32_t)>(game_addr(game::QueueChainTransformation))(entity, id); }
	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t angle, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, angle, out); }
	inline int32_t Normalize(const int32_t *in, int32_t *out) { return fn<int32_t (__cdecl *)(const int32_t *, int32_t *)>(game_addr(game::NormalizeVectorToFixedPoint))(in, out); }
	inline void MatVec(const Mat4x3 *m, const int16_t *in, int16_t *out) { fn<void (__cdecl *)(const Mat4x3 *, const int16_t *, int16_t *)>(game_addr(game::matrixMultiplyVector))(m, in, out); }
	inline void GteMatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline uint32_t RenderGeometry(uint32_t model, void *ctx, uint32_t ot, int32_t a, uint32_t cursor) { return fn<uint32_t (__cdecl *)(uint32_t, void *, uint32_t, int32_t, uint32_t)>(game_addr(game::RenderGeometry))(model, ctx, ot, a, cursor); }
	inline int32_t ComputeCos(int32_t a) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(a); }
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
	static_assert(sizeof(RootNode) == 0x10 && sizeof(Node) == 0x24, "Scatter Shot nodes");
	static_assert(sizeof(ShotRec) == 0x28 && sizeof(Particle) == 0x18 && sizeof(Ghost) == 0x84, "Scatter Shot records");

	inline ShotRec *Shot(int32_t i) { return (ShotRec *)(game_addr(SHOTS) + 0x28 * i); }
	inline Ghost *GhostSlot(int32_t i) { return (Ghost *)(game_addr(GHOSTS) + 0x84 * i); }
	inline uint8_t *Caster() { return Entity(CasterSlot()); }
	inline Node *AddTask(TaskQueue *q, uint32_t fn) { return (Node *)AddTaskToQueue(q, fn); }
	inline Particle *PartArray(int16_t number) { return (Particle *)((number & 1) ? game_addr(PARTS_B) : game_addr(PARTS_A)); }
	inline Particle *PartsB() { return (Particle *)game_addr(PARTS_B); }
}
}

namespace ff8fx
{
namespace scatter192
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

	static void MuzzleModel(const Node *n, int16_t roll, uint32_t model, bool faded, int32_t fade)
	{
		const ShotRec *s = Shot(n->f0E);
		int16_t ang[4];
		ang[0] = 0;
		ang[1] = 0;
		ang[2] = roll;
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
	static int32_t Flash2Fade(int32_t c) { return mul32(c, 0x555) - 0x555; }
	static int32_t BlastFade(int32_t c) { return mul32(c, 0x2AA) - 0x2AA; }
	static int32_t RingFade(int32_t c) { return (int32_t)((uint32_t)(c - 4) << 10); }

	static void RingDraw(const Node *n, bool faded, int32_t fade)
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
		ModelDraw(game_addr(MODEL_Ring), faded, fade);
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

	static void ImpactFlashDraw(const Node *n, uint8_t *h)
	{
		*(int16_t *)(h + 4) = n->counter;
		*(uint32_t *)h = game_addr(SEQ_Impact);
		*(int16_t *)(h + 0x24) = 8;
		TransformCameraByShadowRotation(n->pos, n->f1C, -(int32_t)n->pos[3]);
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
	}

	static void SparkDraw(const Particle *e, int16_t depth, uint8_t *h)
	{
		*(int16_t *)(h + 4) = e->frame;
		TransformCameraByShadowRotation(e->pos, e->size, -(int32_t)depth);
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
	}
	static void SparkMove(Particle *e)
	{
		e->pos[0] = (int16_t)(e->pos[0] + e->vel[0]);
		e->pos[1] = (int16_t)(e->pos[1] + e->vel[1]);
		e->pos[2] = (int16_t)(e->pos[2] + e->vel[2]);
		e->vel[0] = (int16_t)(e->vel[0] - (int16_t)(e->vel[0] >> 2));
		e->vel[1] = (int16_t)(e->vel[1] - (int16_t)(e->vel[1] >> 2));
		e->vel[2] = (int16_t)(e->vel[2] - (int16_t)(e->vel[2] >> 2));
	}

	static void DustDraw(const Particle *e, uint8_t *h)
	{
		*(int16_t *)(h + 4) = e->frame;
		TransformCameraByShadowRotation(e->pos, e->size, -((int32_t)e->size >> 4));
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
	}
	static void DustMove(Particle *e)
	{
		e->size = (int16_t)(e->size + 0x20);
		e->pos[0] = (int16_t)(e->pos[0] + e->vel[0]);
		e->pos[2] = (int16_t)(e->pos[2] + e->vel[2]);
		e->vel[0] = (int16_t)(e->vel[0] - (int16_t)(e->vel[0] >> 3));
		e->vel[2] = (int16_t)(e->vel[2] - (int16_t)(e->vel[2] >> 3));
	}
	static void DustHeader(uint8_t *h)
	{
		*(uint32_t *)h = game_addr(SEQ_Dust);
		*(uint32_t *)(h + 0x1C) = 0x404040;
		*(int16_t *)(h + 0x24) = 4;
	}

	static uint32_t __cdecl FlashTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		MuzzleModel(n, n->f18, game_addr(MODEL_Flash), n->counter == 1, 0xC00);
		if (UpdateFlags() & 0x201) return 0;
		n->counter++;
		return n->counter >= 2 ? TASK_END : 0;
	}

	static uint32_t __cdecl Flash2Task(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		MuzzleModel(n, n->f18, game_addr(MODEL_Flash2), n->counter >= 1, Flash2Fade(n->counter));
		if (UpdateFlags() & 0x201) return 0;
		const int16_t step = n->f1E;
		n->f1C = (int16_t)(n->f1C + step);
		n->f1E = (int16_t)(step - (int16_t)(step / 3));
		n->counter++;
		return n->counter >= 4 ? TASK_END : 0;
	}

	static uint32_t __cdecl BlastTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		MuzzleModel(n, 0, game_addr(MODEL_Blast), n->counter >= 1, BlastFade(n->counter));
		if (UpdateFlags() & 0x201) return 0;
		const int16_t step = n->f1E;
		n->f1C = (int16_t)(n->f1C + step);
		n->f1E = (int16_t)(step - (int16_t)(step / 4));
		n->counter++;
		return n->counter >= 7 ? TASK_END : 0;
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

	static uint32_t __cdecl ImpactTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		if (n->f18 > 0)
		{
			if (!(UpdateFlags() & 0x201)) n->f18--;
			return 0;
		}
		Particle *arr = PartArray(n->f0E);
		*(volatile uint32_t *)&n_ = (uint32_t)arr;
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		if (n->counter < 0x10) ImpactFlashDraw(n, h);
		*(int16_t *)(h + 0x24) = 8;
		*(uint32_t *)h = game_addr(SEQ_Spark);
		int32_t alive = 0;
		const int32_t id = n->f0E + 1;
		for (Particle *e = arr; e < arr + PARTS_N; e++)
		{
			if (e->id != id) continue;
			SparkDraw(e, n->pos[3], h);
			if (UpdateFlags() & 0x201) continue;
			e->frame++;
			if (*(const int16_t *)(h + 0x28) < 0)
			{
				e->id = 0;
				continue;
			}
			SparkMove(e);
			alive++;
		}
		FieldFree(0xB4);
		if (UpdateFlags() & 0x201) return 0;
		if (n->counter == 0)
		{
			for (int32_t k = 0; k < 10; k++)
			{
				int32_t i = 0;
				while (i < PARTS_N && arr[i].id != 0) i++;
				if (i >= PARTS_N) break;
				Particle *e = &arr[i];
				e->id = id;
				e->frame = 0;
				e->size = (int16_t)(CrtRand() % 0x480 + 0x100);
				*(uint32_t *)&e->pos[0] = *(const uint32_t *)&n->pos[0];
				*(uint32_t *)&e->pos[2] = *(const uint32_t *)&n->pos[2];
				e->pos[0] = (int16_t)(e->pos[0] + (CrtRand() % 300 - 0x96));
				e->pos[1] = (int16_t)(e->pos[1] + (CrtRand() % 300 - 0x96));
				e->pos[2] = (int16_t)(e->pos[2] + (CrtRand() % 300 - 0x96));
				int32_t v[3];
				v[0] = CrtRand() % 0x1000 - 0x800;
				v[1] = CrtRand() % 0x1000 - 0x800;
				v[2] = CrtRand() % 0x1000 - 0x800;
				Normalize(v, v);
				const int32_t sp = CrtRand() % 400 + 0xB4;
				e->vel[0] = (int16_t)(mul32(sp, v[0]) >> 12);
				e->vel[1] = (int16_t)(mul32(sp, v[1]) >> 12);
				e->vel[2] = (int16_t)(mul32(sp, v[2]) >> 12);
			}
		}
		n->counter++;
		return (n->counter >= 4 && alive == 0) ? TASK_END : 0;
	}

	static uint32_t __cdecl MissTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		if (n->f18 > 0)
		{
			if (UpdateFlags() & 0x201) return 0;
			n->f18--;
			return 0;
		}
		Node *r = AddTask(QAux(), game_addr(ORIG_MissRingTask));
		*(uint32_t *)&r->pos[0] = *(const uint32_t *)&n->pos[0];
		r->counter = 0;
		*(uint32_t *)&r->pos[2] = *(const uint32_t *)&n->pos[2];
		r->f18 = (int16_t)(CrtRand() % 0x1000);
		const int16_t quarter = (int16_t)((CrtRand() % 0x600 + 0x700) / 4);
		r->f1E = quarter;
		r->f1C = quarter;
		Node *d = AddTask(QAux(), game_addr(ORIG_MissDustTask));
		d->f0E = n->f0E;
		*(uint32_t *)&d->pos[0] = *(const uint32_t *)&n->pos[0];
		d->counter = 0;
		*(uint32_t *)&d->pos[2] = *(const uint32_t *)&n->pos[2];
		return TASK_END;
	}

	static uint32_t __cdecl MissRingTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		RingDraw(n, n->counter >= 4, RingFade(n->counter));
		if (UpdateFlags() & 0x201) return 0;
		const int16_t step = n->f1E;
		n->f1C = (int16_t)(n->f1C + step);
		n->f1E = (int16_t)(step - (int16_t)(step / 5));
		n->counter++;
		return n->counter >= 8 ? TASK_END : 0;
	}

	static uint32_t __cdecl MissDustTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		Particle *arr = PartArray(n->f0E);
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		DustHeader(h);
		int32_t alive = 0;
		for (Particle *e = arr; e < arr + PARTS_N; e++)
		{
			if (e->id != (int32_t)n->f0E) continue;
			DustDraw(e, h);
			if (UpdateFlags() & 0x201) continue;
			e->frame++;
			if (*(const int16_t *)(h + 0x28) < 0)
			{
				e->id = 0;
				continue;
			}
			DustMove(e);
			alive++;
		}
		FieldFree(0xB4);
		if (UpdateFlags() & 0x201) return 0;
		if (n->counter == 0)
		{
			for (int32_t k = 0; k < 10; k++)
			{
				int32_t i = 0;
				while (i < PARTS_N && arr[i].id != 0) i++;
				if (i >= PARTS_N) break;
				Particle *e = &arr[i];
				e->id = n->f0E;
				e->frame = 0;
				e->size = (int16_t)(CrtRand() % 0x600 + 0x800);
				*(uint32_t *)&e->pos[0] = *(const uint32_t *)&n->pos[0];
				*(uint32_t *)&e->pos[2] = *(const uint32_t *)&n->pos[2];
				const int32_t a = CrtRand() % 0x1000;
				const int32_t c = ComputeCos(a);
				const int32_t s = ComputeSin(a);
				Particle *b = &PartsB()[i];
				const int32_t r = CrtRand() % 50 + 50;
				b->pos[0] = (int16_t)(b->pos[0] + (int16_t)(mul32(r, c) >> 12));
				b->pos[2] = (int16_t)(b->pos[2] + (int16_t)(mul32(r, s) >> 12));
				const int32_t sp = CrtRand() % 30 + 20;
				b->vel[0] = (int16_t)(mul32(sp, c) >> 12);
				b->vel[2] = (int16_t)(mul32(sp, s) >> 12);
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
		if (n->counter == 8)
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
			BuildAxisAngleRotationMatrix(angle, &s->m, axis);
			Node *f = AddTask(QAux(), game_addr(ORIG_FlashTask));
			f->counter = 0;
			f->f0E = n->f0E;
			f->f18 = (int16_t)(CrtRand() % 0x1000);
			f->f1C = (int16_t)(CrtRand() % 0x500 + 0x900);
			Node *g = AddTask(QAux(), game_addr(ORIG_Flash2Task));
			g->counter = 0;
			g->f0E = n->f0E;
			g->f18 = (int16_t)(CrtRand() % 0x1000);
			const int16_t third = (int16_t)((CrtRand() % 0x600 + 0x800) / 3);
			g->f1E = third;
			g->f1C = third;
			Node *k = AddTask(QAux(), game_addr(ORIG_SmokeTask));
			k->counter = 0;
			k->f0E = n->f0E;
			k->f1C = (int16_t)(CrtRand() % 0x700 + 0x400);
			Node *d = AddTask(QAux(), game_addr(ORIG_GroundSmokeTask));
			d->f0E = n->f0E;
			d->counter = 0;
			d->f1C = (int16_t)(mul32((int32_t)k->f1C * 13, 0x200) >> 12);
			Node *b = AddTask(QAux(), game_addr(ORIG_BlastTask));
			b->counter = 0;
			b->f0E = n->f0E;
			const int16_t bthird = (int16_t)((CrtRand() % 0x300 + 0xA00) / 3);
			b->f1E = bthird;
			b->f1C = bthird;
		}
		if (n->counter == 1)
		{
			for (int32_t i = 0; i < (int32_t)Ctx()->actions->target_count; i++)
			{
				const int32_t slot = Ctx()->actions->targets[i * TARGET_STRIDE];
				uint8_t *t = Entity(slot);
				int32_t bone = ***(const uint8_t ***)(t + 0x64);
				*(volatile int32_t *)&n_ = bone;
				if (bone)
				{
					bone = CrtRand() % bone;
					*(volatile int32_t *)&n_ = bone;
				}
				Node *im = AddTask(QAux(), game_addr(ORIG_ImpactTask));
				im->f0E = (int16_t)ImpactCount();
				ImpactCount()++;
				im->counter = 0;
				GetEffectSpawnPosition(t, bone, CrtRand() % 0x1000, im->pos);
				im->pos[3] = (int16_t)(mul32(*(const int16_t *)(t + 0x26), 2000) >> 12);
				const int32_t size = CrtRand() % 0xC00 + 0x800;
				im->f1C = (int16_t)size;
				im->f18 = (int16_t)(size & 1);
				if (CrtRand() & 1)
				{
					int32_t bone2 = ***(const uint8_t ***)(t + 0x64);
					*(volatile int32_t *)&n_ = bone2;
					if (bone2)
					{
						bone2 = CrtRand() % bone2;
						*(volatile int32_t *)&n_ = bone2;
					}
					Node *i2 = AddTask(QAux(), game_addr(ORIG_ImpactTask));
					i2->f0E = (int16_t)ImpactCount();
					i2->counter = 0;
					ImpactCount()++;
					GetEffectSpawnPosition(t, bone2, CrtRand() % 0x1000, i2->pos);
					i2->pos[3] = im->pos[3];
					const int32_t size2 = CrtRand() % 0xC00 + 0x800;
					i2->f1C = (int16_t)size2;
					i2->f18 = (int16_t)(size2 & 1);
				}
				else
				{
					Node *m = AddTask(QAux(), game_addr(ORIG_MissTask));
					m->counter = 0;
					m->f0E = (int16_t)ImpactCount();
					ImpactCount()++;
					GetDefaultEffectPosition(t, m->pos);
					const int16_t s = im->pos[3];
					m->pos[3] = s;
					*(volatile int16_t *)&n_ = s;
					const int32_t dx = CrtRand() % (2 * (int32_t)s) - s;
					m->pos[1] = 0;
					m->pos[0] = (int16_t)(m->pos[0] + (int16_t)dx);
					const int32_t dz = CrtRand() % (2 * (int32_t)s) - s;
					m->pos[2] = (int16_t)(m->pos[2] + (int16_t)dz);
					m->f18 = (int16_t)(*(const uint8_t *)&m->pos[0] & 1);
				}
			}
			ApplyActionResultToTargets(Ctx()->actions[0].targets, Ctx()->actions[0].target_count);
			BdPlaySE(SOUND_Gun(), (n->f0E & 1) ? 1 : 2, 0x80);
		}
		n->counter++;
		return n->counter >= 10 ? TASK_END : 0;
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

	void register_mag192_scatter_shot()
	{
		register_port(scatter192::ORIG_RootTask, (void *)scatter192::RootTask, "192 RootTask", 192);
		register_port(scatter192::ORIG_RestoreTask, (void *)scatter192::RestoreTask, "192 RestoreTask", 192);
		register_port(scatter192::ORIG_StartTask, (void *)scatter192::StartTask, "192 StartTask", 192);
		register_port(scatter192::ORIG_ShotTask, (void *)scatter192::ShotTask, "192 ShotTask", 192);
		register_port(scatter192::ORIG_EndTask, (void *)scatter192::EndTask, "192 EndTask", 192);
		register_port(scatter192::ORIG_FadeOutTask, (void *)scatter192::FadeOutTask, "192 FadeOutTask", 192);
		register_port(scatter192::ORIG_FadeInTask, (void *)scatter192::FadeInTask, "192 FadeInTask", 192);
		register_port(scatter192::ORIG_AfterimageTask, (void *)scatter192::AfterimageTask, "192 AfterimageTask", 192);
		register_port(scatter192::ORIG_FlashTask, (void *)scatter192::FlashTask, "192 FlashTask", 192);
		register_port(scatter192::ORIG_Flash2Task, (void *)scatter192::Flash2Task, "192 Flash2Task", 192);
		register_port(scatter192::ORIG_BlastTask, (void *)scatter192::BlastTask, "192 BlastTask", 192);
		register_port(scatter192::ORIG_SmokeTask, (void *)scatter192::SmokeTask, "192 SmokeTask", 192);
		register_port(scatter192::ORIG_GroundSmokeTask, (void *)scatter192::GroundSmokeTask, "192 GroundSmokeTask", 192);
		register_port(scatter192::ORIG_ImpactTask, (void *)scatter192::ImpactTask, "192 ImpactTask", 192);
		register_port(scatter192::ORIG_MissTask, (void *)scatter192::MissTask, "192 MissTask", 192);
		register_port(scatter192::ORIG_MissRingTask, (void *)scatter192::MissRingTask, "192 MissRingTask", 192);
		register_port(scatter192::ORIG_MissDustTask, (void *)scatter192::MissDustTask, "192 MissDustTask", 192);
	}
}

