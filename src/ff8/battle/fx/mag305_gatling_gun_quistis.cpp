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

#include "mag_common.h"

namespace ff8fx
{
namespace gg305
{
	using namespace eng;
	using namespace magc;

	inline uint32_t &TargetSlot() { return var<uint32_t>(game_addr(game::MAG_305_GATLING_GUN_QUISTIS_TargetSlot)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::MAG_305_GATLING_GUN_QUISTIS_CastContext)); }
	inline uint32_t &CasterSlot() { return var<uint32_t>(game_addr(game::MAG_305_GATLING_GUN_QUISTIS_CasterSlot)); }
	inline int16_t *Muzzle() { return (int16_t *)game_addr(game::MAG_305_GATLING_GUN_QUISTIS_Muzzle); }
	inline uint32_t &Cursor() { return var<uint32_t>(game_addr(game::MAG_305_GATLING_GUN_QUISTIS_PacketCursor)); }
	inline TaskQueue *QEffects() { return (TaskQueue *)game_addr(game::MAG_305_GATLING_GUN_QUISTIS_EffectQueue); }
	static constexpr game::Id ARENA_A = game::MAG_305_GATLING_GUN_QUISTIS_PacketArena1, ARENA_B = game::MAG_305_GATLING_GUN_QUISTIS_PacketArena0;

	static constexpr game::Id ORIG_RootTask = game::MAG_305_GATLING_GUN_QUISTIS_RootTask;
	static constexpr game::Id ORIG_MasterTask = game::MAG_305_GATLING_GUN_QUISTIS_DirectorTask;
	static constexpr game::Id ORIG_MuzzleTask = game::MAG_305_GATLING_GUN_QUISTIS_MuzzleTask;
	static constexpr game::Id ORIG_SmokeTask = game::MAG_305_GATLING_GUN_QUISTIS_SmokeTask;
	static constexpr game::Id ORIG_MissTask = game::MAG_305_GATLING_GUN_QUISTIS_MissTask;
	static constexpr game::Id ORIG_ImpactTask = game::MAG_305_GATLING_GUN_QUISTIS_ImpactTask;
	static constexpr game::Id ORIG_FlashTask = game::MAG_305_GATLING_GUN_QUISTIS_FlashTask;
	static constexpr game::Id ORIG_DustTask = game::MAG_305_GATLING_GUN_QUISTIS_DustTask;
	static constexpr game::Id ORIG_HitTask = game::MAG_305_GATLING_GUN_QUISTIS_HitTask;
	static constexpr game::Id ORIG_SparkTask = game::MAG_305_GATLING_GUN_QUISTIS_SparkTask;
	static constexpr game::Id ORIG_SparksTask = game::MAG_305_GATLING_GUN_QUISTIS_SparksTask;
	static constexpr game::Id MODEL_Muzzle = game::MAG_305_GATLING_GUN_QUISTIS_ModelMuzzle;
	static constexpr game::Id MODEL_Impact = game::MAG_305_GATLING_GUN_QUISTIS_ModelImpact;
	static constexpr game::Id MODEL_Spark = game::MAG_305_GATLING_GUN_QUISTIS_ModelSpark;
	static constexpr game::Id SEQ_Smoke = game::MAG_305_GATLING_GUN_QUISTIS_SeqSmoke;
	static constexpr game::Id SEQ_Flash = game::MAG_305_GATLING_GUN_QUISTIS_SeqFlash;
	static constexpr game::Id SEQ_Sparks = game::MAG_305_GATLING_GUN_QUISTIS_SeqSparks;
	static const void *SOUND_Gun() { return (const void *)game_addr(game::MAG_305_GATLING_GUN_QUISTIS_SoundFire); }
	static constexpr game::Id BIND_Block = game::MAG_305_GATLING_GUN_QUISTIS_BindBlock;
	static constexpr game::Id COM_Anim = game::MAG_305_GATLING_GUN_QUISTIS_AnimSetCom;

	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t flag, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, flag, out); }
	inline void MatrixMultiply3(Mat4x3 *a, const Mat4x3 *b) { fn<void (__cdecl *)(Mat4x3 *, const Mat4x3 *)>(game_addr(game::MatrixMultiply3x3))(a, b); }
	inline int32_t ComputeCos(int32_t a) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(a); }
	inline void BindEntityModelFromComData(uint8_t *entity, uint32_t out, uint32_t data) { fn<void (__cdecl *)(uint8_t *, uint32_t, uint32_t)>(game_addr(game::BattleEntity_BindExternalAnimSet))(entity, out, data); }
	inline void QueueChainTransformation(uint8_t *entity, int32_t id) { fn<void (__cdecl *)(uint8_t *, int32_t)>(game_addr(game::QueueChainTransformation))(entity, id); }

#pragma pack(push, 1)
	struct RootNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad;
	};
	struct Node
	{
		TaskNode hdr;
		int16_t counter;
		int16_t f0E;
		int16_t f10;
		int16_t f12;
		int16_t f14;
		int16_t f16;
		int16_t f18;
		int16_t f1A;
		int16_t f1C;
		int16_t f1E;
		uint8_t pad20[4];
	};
	struct Particle
	{
		int32_t id;
		int16_t frame;
		int16_t angle;
		int16_t pos[4];
		int16_t vel[3];
		int16_t f16;
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x10 && sizeof(Node) == 0x24, "Gatling Gun (Quistis) nodes");
	static_assert(sizeof(Particle) == 0x18, "Gatling Gun (Quistis) records");

	inline Particle *PartsA() { return (Particle *)game_addr(game::MAG_305_GATLING_GUN_QUISTIS_ParticlesA); }
	inline Particle *PartsB() { return (Particle *)game_addr(game::MAG_305_GATLING_GUN_QUISTIS_ParticlesB); }
	inline int16_t *EntPos(int32_t slot) { return (int16_t *)(Entity(slot) + 0x1C); }
	inline uint32_t rd32(const void *p) { return *(const uint32_t *)p; }
	inline Node *AddTask(uint32_t fn) { return (Node *)AddTaskToQueue(QEffects(), fn); }
}
}

namespace ff8fx
{
namespace gg305
{
	static void ModelDraw(uint32_t model, bool faded, int32_t fade, int times)
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
		for (int k = 0; k < times; k++) Cursor() = RenderPrimModel(h, RenderOT(), 2, Cursor());
		FieldFree(0x58);
	}

	static void MuzzleDraw(const Node *n, bool faded, int32_t fade)
	{
		int16_t ang[4];
		ang[0] = 0;
		ang[1] = n->f18;
		ang[2] = n->f1A;
		ang[3] = 0;
		Mat4x3 m;
		ComposeZYXRotationMatrix(ang, &m);
		m.t[0] = n->f10;
		m.t[2] = n->f14;
		m.t[1] = n->f12;
		const int32_t s = n->f1C;
		const int32_t v[3] = { s, s, s };
		Scale3DMatrix(&m, v);
		ComposeAffineTransform(&Camera(), &m, &m);
		TransformCameraByShadowRotation(&n->f10, 0, -((int32_t)n->f1C >> 3));
		GteSetRotMatrix(&m);
		ModelDraw(game_addr(MODEL_Muzzle), faded, fade, 2);
	}
	static int32_t MuzzleFade(int32_t c) { return shl32(c, 11); }

	static void ImpactDraw(const Node *n, bool faded, int32_t fade)
	{
		int16_t ang[4];
		ang[0] = 0;
		ang[1] = n->f18;
		ang[2] = 0;
		ang[3] = 0;
		Mat4x3 m;
		ComposeZYXRotationMatrix(ang, &m);
		m.t[0] = n->f10;
		m.t[1] = n->f12;
		const int32_t s = n->f1C;
		const int32_t v[3] = { s, s, s };
		m.t[2] = n->f14;
		Scale3DMatrix(&m, v);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		ModelDraw(game_addr(MODEL_Impact), faded, fade, 1);
	}
	static int32_t ImpactFade(int32_t c) { return shl32(c, 9); }

	static void SparkDraw(const Node *n, bool faded, int32_t fade)
	{
		int16_t ang[4];
		ang[0] = 0;
		ang[1] = n->f1A;
		ang[2] = 0;
		ang[3] = 0;
		Mat4x3 roll;
		BuildRotationMatrixFromAngles(ang, &roll);
		ang[0] = 0x400;
		ang[1] = n->f18;
		ang[2] = 0;
		Mat4x3 m;
		ComposeZYXRotationMatrix(ang, &m);
		m.t[0] = n->f10;
		m.t[1] = n->f12;
		const int32_t s = n->f1C;
		const int32_t v[3] = { s, s, s };
		m.t[2] = n->f14;
		Scale3DMatrix(&m, v);
		ComposeAffineTransform(&Camera(), &m, &m);
		MatrixMultiply3(&m, &roll);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		ModelDraw(game_addr(MODEL_Spark), faded, fade, 1);
	}
	static int32_t SparkFade(int32_t c) { return shl32(c, 10) - 0x400; }

	static void FlashDraw(const Node *n, int16_t frame)
	{
		TransformCameraByShadowRotation(&n->f10, n->f1C, -((int32_t)n->f1C >> 3));
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		*(int16_t *)(h + 4) = frame;
		*(uint32_t *)h = game_addr(SEQ_Flash);
		*(int16_t *)(h + 0x24) = 0;
		Cursor() = InitEffectSequenceFromData(h, RenderOT(), 2, Cursor());
		FieldFree(0xB4);
	}

	static void ParticleDraw(uint8_t *h, int16_t frame, const int16_t *pos, int16_t angle)
	{
		*(int16_t *)(h + 4) = frame;
		TransformCameraByShadowRotation(pos, angle, -((int32_t)angle >> 4));
		Cursor() = InitEffectSequenceFromData(h, RenderOT(), 2, Cursor());
	}

	static void SmokeMove(Particle *e)
	{
		e->angle = (int16_t)(e->angle + 0x18);
		e->pos[0] = (int16_t)(e->pos[0] + e->vel[0]);
		e->pos[1] = (int16_t)(e->pos[1] + e->vel[1]);
		e->pos[2] = (int16_t)(e->pos[2] + e->vel[2]);
		e->vel[0] = (int16_t)(e->vel[0] - (int16_t)(e->vel[0] >> 3));
		e->vel[2] = (int16_t)(e->vel[2] - (int16_t)(e->vel[2] >> 3));
	}
	static void DustMove(Particle *e)
	{
		e->angle = (int16_t)(e->angle + 0x20);
		e->pos[0] = (int16_t)(e->pos[0] + e->vel[0]);
		e->pos[2] = (int16_t)(e->pos[2] + e->vel[2]);
		e->vel[0] = (int16_t)(e->vel[0] - (int16_t)(e->vel[0] >> 3));
		e->vel[2] = (int16_t)(e->vel[2] - (int16_t)(e->vel[2] >> 3));
	}
	static void SparksMove(Particle *e)
	{
		e->pos[0] = (int16_t)(e->pos[0] + e->vel[0]);
		e->pos[1] = (int16_t)(e->pos[1] + e->vel[1]);
		e->pos[2] = (int16_t)(e->pos[2] + e->vel[2]);
		e->vel[0] = (int16_t)(e->vel[0] - e->vel[0] / 3);
		e->vel[1] = (int16_t)(e->vel[1] - e->vel[1] / 3);
		e->vel[2] = (int16_t)(e->vel[2] - e->vel[2] / 3);
	}

	inline Particle *FreeParticle(Particle *a)
	{
		int idx = 0;
		while (idx < 100 && a[idx].id != 0) idx++;
		return idx < 100 ? &a[idx] : nullptr;
	}

	static uint32_t __cdecl MuzzleTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		MuzzleDraw(n, n->counter >= 0, MuzzleFade(n->counter));
		if (UpdateFlags() & 0x201) return 0;
		n->counter++;
		return n->counter >= 2 ? TASK_END : 0;
	}

	static uint32_t __cdecl SmokeTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		int32_t alive = 0;
		*(uint32_t *)h = game_addr(SEQ_Smoke);
		*(uint32_t *)(h + 0x1C) = 0x404040;
		*(int16_t *)(h + 0x24) = 4;
		for (Particle *e = PartsA(); e < PartsA() + 100; e++)
		{
			if (!(*(const uint8_t *)&e->id & 1)) continue;
			ParticleDraw(h, e->frame, e->pos, e->angle);
			if (!(UpdateFlags() & 0x201))
			{
				e->frame++;
				if (*(const int16_t *)(h + 0x28) < 0) e->id = 0;
				else
				{
					SmokeMove(e);
					alive++;
				}
			}
		}
		FieldFree(0xB4);
		if (UpdateFlags() & 0x201) return 0;
		if (n->counter >= 0 && n->counter <= 0x17)
		{
			for (int k = 0; k < 3; k++)
			{
				Particle *e = FreeParticle(PartsA());
				if (!e) break;
				e->id = 1;
				e->frame = 0;
				e->angle = (int16_t)(CrtRand() % 0x600 + 0x400);
				*(uint32_t *)&e->pos[0] = rd32(&Muzzle()[0]);
				*(uint32_t *)&e->pos[2] = rd32(&Muzzle()[2]);
				int32_t v[3];
				v[0] = CrtRand() % 0x1000 - 0x800;
				v[1] = CrtRand() % 0x1000 - 0x800;
				v[2] = CrtRand() % 0x1000 - 0x800;
				NormalizeVector(v, v);
				const int32_t r = CrtRand() % 20;
				e->pos[0] = (int16_t)(e->pos[0] + (mul32(r, v[0]) >> 12));
				e->pos[1] = (int16_t)(e->pos[1] + (mul32(r, v[1]) >> 12));
				e->pos[2] = (int16_t)(e->pos[2] + (mul32(r, v[2]) >> 12));
				const int32_t sp = CrtRand() % 0x32 + 0x14;
				e->vel[0] = (int16_t)(mul32(sp, v[0]) >> 12);
				const int32_t up = CrtRand() % 0x23;
				e->vel[1] = (int16_t)(-10 - up);
				e->vel[2] = (int16_t)(mul32(sp, v[2]) >> 12);
			}
		}
		n->counter++;
		if (n->counter < 0x10 || alive != 0) return 0;
		return TASK_END;
	}

	static uint32_t __cdecl MissTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		if (UpdateFlags() & 0x201) return 0;
		if (n->f0E > 0)
		{
			n->f0E--;
			return 0;
		}
		Node *a = AddTask(game_addr(ORIG_ImpactTask));
		*(uint32_t *)&a->f10 = rd32(&n->f10);
		a->counter = 0;
		*(uint32_t *)&a->f14 = rd32(&n->f14);
		a->f18 = (int16_t)(CrtRand() % 0x800);
		a->f1C = 0x400;
		a->f1E = 0x100;
		Node *f = AddTask(game_addr(ORIG_FlashTask));
		*(uint32_t *)&f->f10 = rd32(&n->f10);
		*(uint32_t *)&f->f14 = rd32(&n->f14);
		f->counter = 0;
		f->f12 = (int16_t)0xFF38;
		f->f1C = (int16_t)(CrtRand() % 0x900 + 0xC00);
		Node *d = AddTask(game_addr(ORIG_DustTask));
		d->f0E = (int16_t)(1 << (n->f18 & 31));
		*(uint32_t *)&d->f10 = rd32(&n->f10);
		d->counter = 0;
		*(uint32_t *)&d->f14 = rd32(&n->f14);
		return TASK_END;
	}

	static uint32_t __cdecl ImpactTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		ImpactDraw(n, n->counter >= 0, ImpactFade(n->counter));
		if (UpdateFlags() & 0x201) return 0;
		const int16_t step = n->f1E;
		n->f1C = (int16_t)(n->f1C + step);
		n->f1E = (int16_t)(step - (int32_t)step / 8);
		n->counter++;
		return n->counter > 8 ? TASK_END : 0;
	}

	static uint32_t __cdecl FlashTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		FlashDraw(n, n->counter);
		if (UpdateFlags() & 0x201) return 0;
		n->counter++;
		return n->counter >= 0x10 ? TASK_END : 0;
	}

	static uint32_t __cdecl DustTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		int32_t alive = 0;
		*(uint32_t *)h = game_addr(SEQ_Smoke);
		*(uint32_t *)(h + 0x1C) = 0x404040;
		*(int16_t *)(h + 0x24) = 4;
		for (Particle *e = PartsA(); e < PartsA() + 100; e++)
		{
			if (!(e->id & (int32_t)n->f0E)) continue;
			ParticleDraw(h, e->frame, e->pos, e->angle);
			if (!(UpdateFlags() & 0x201))
			{
				e->frame++;
				if (*(const int16_t *)(h + 0x28) < 0) e->id = 0;
				else
				{
					DustMove(e);
					alive++;
				}
			}
		}
		FieldFree(0xB4);
		if (UpdateFlags() & 0x201) return 0;
		if (n->counter == 0)
		{
			for (int k = 0; k < 10; k++)
			{
				Particle *e = FreeParticle(PartsA());
				if (!e) break;
				e->id = n->f0E;
				e->frame = 0;
				e->angle = (int16_t)(CrtRand() % 0x600 + 0x800);
				*(uint32_t *)&e->pos[0] = rd32(&n->f10);
				*(uint32_t *)&e->pos[2] = rd32(&n->f14);
				const int32_t a = CrtRand() % 0x1000;
				const int32_t c = ComputeCos(a);
				const int32_t s = ComputeSin(a);
				const int32_t r = CrtRand() % 0x32 + 0x32;
				e->pos[0] = (int16_t)(e->pos[0] + (mul32(r, c) >> 12));
				e->pos[2] = (int16_t)(e->pos[2] + (mul32(r, s) >> 12));
				const int32_t sp = CrtRand() % 0x1E + 0x14;
				e->vel[0] = (int16_t)(mul32(sp, c) >> 12);
				e->vel[2] = (int16_t)(mul32(sp, s) >> 12);
			}
		}
		n->counter++;
		if (n->counter < 0x10 || alive != 0) return 0;
		return TASK_END;
	}

	static uint32_t __cdecl HitTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		if (UpdateFlags() & 0x201) return 0;
		if (n->f0E > 0)
		{
			n->f0E--;
			return 0;
		}
		int32_t v[3];
		v[0] = CrtRand() % 0x1000 - 0x800;
		v[1] = 0;
		v[2] = -(CrtRand() % 0x800);
		TransformVectorBy3x3Matrix((const Mat4x3 *)(Entity(TargetSlot()) + 0x40), v, v);
		NormalizeVector(v, v);
		const int32_t r = CrtRand() % 0xD2 + 0x3C;
		Node *s = AddTask(game_addr(ORIG_SparkTask));
		s->counter = 0;
		s->f10 = (int16_t)((mul32(r, v[0]) >> 12) + n->f10);
		s->f12 = n->f12;
		s->f14 = (int16_t)((mul32(r, v[2]) >> 12) + n->f14);
		s->f18 = (int16_t)(*(const int16_t *)(Entity(CasterSlot()) + 0xE) + 0x800);
		s->f1A = (int16_t)(CrtRand() % 0x800);
		const int16_t scale = (int16_t)(CrtRand() % 0x360 + 0x440);
		s->f1C = scale;
		s->f1E = (int16_t)((int32_t)scale / 4);
		Node *f = AddTask(game_addr(ORIG_FlashTask));
		*(uint32_t *)&f->f10 = rd32(&s->f10);
		f->counter = 0;
		*(uint32_t *)&f->f14 = rd32(&s->f14);
		f->f1C = (int16_t)(CrtRand() % 0xC00 + 0x1100);
		Node *g = AddTask(game_addr(ORIG_SparksTask));
		g->f0E = (int16_t)(1 << (n->f18 & 31));
		g->counter = 0;
		*(uint32_t *)&g->f10 = rd32(&s->f10);
		*(uint32_t *)&g->f14 = rd32(&s->f14);
		return TASK_END;
	}

	static uint32_t __cdecl SparkTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		SparkDraw(n, n->counter >= 1, SparkFade(n->counter));
		if (UpdateFlags() & 0x201) return 0;
		const int16_t step = n->f1E;
		n->f1C = (int16_t)(n->f1C + step);
		n->f1E = (int16_t)(step - (int32_t)step / 8);
		n->counter++;
		return n->counter >= 5 ? TASK_END : 0;
	}

	static uint32_t __cdecl SparksTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		int32_t alive = 0;
		*(uint32_t *)h = game_addr(SEQ_Sparks);
		*(int16_t *)(h + 0x24) = 1;
		for (Particle *e = PartsB(); e < PartsB() + 100; e++)
		{
			if (!(e->id & (int32_t)n->f0E)) continue;
			*(int32_t *)(h + 8) = e->f16;
			ParticleDraw(h, e->frame, e->pos, e->angle);
			if (!(UpdateFlags() & 0x201))
			{
				e->frame++;
				if (*(const int16_t *)(h + 0x28) < 0) e->id = 0;
				else
				{
					SparksMove(e);
					alive++;
				}
			}
		}
		FieldFree(0xB4);
		if (UpdateFlags() & 0x201) return 0;
		if (n->counter == 0)
		{
			for (int k = 0; k < 0xC; k++)
			{
				Particle *e = FreeParticle(PartsB());
				if (!e) break;
				e->id = n->f0E;
				e->frame = 0;
				e->angle = (int16_t)(CrtRand() % 0x400 + 0x500);
				*(uint32_t *)&e->pos[0] = rd32(&n->f10);
				*(uint32_t *)&e->pos[2] = rd32(&n->f14);
				int32_t v[3];
				v[0] = CrtRand() % 0x1000 - 0x800;
				v[1] = CrtRand() % 0x1000 - 0x800;
				v[2] = CrtRand() % 0x1000 - 0x800;
				NormalizeVector(v, v);
				const int32_t r = CrtRand() % 100 + 0x5A;
				e->pos[0] = (int16_t)(e->pos[0] + (mul32(r, v[0]) >> 12));
				e->pos[1] = (int16_t)(e->pos[1] + (mul32(r, v[1]) >> 12));
				e->pos[2] = (int16_t)(e->pos[2] + (mul32(r, v[2]) >> 12));
				const int32_t sp = CrtRand() % 0x17C + 0x82;
				e->vel[0] = (int16_t)(mul32(sp, v[0]) >> 12);
				e->vel[1] = (int16_t)(mul32(sp, v[1]) >> 12);
				e->vel[2] = (int16_t)(mul32(sp, v[2]) >> 12);
				e->f16 = (int16_t)(CrtRand() % 0x800);
			}
		}
		n->counter++;
		if (n->counter < 0xF || alive != 0) return 0;
		return TASK_END;
	}

	static void ComputeMuzzle()
	{
		GetEffectSpawnPosition(Entity(CasterSlot()), 0xF0, 0xC00, Muzzle());
		const int32_t ry = *(const int16_t *)(Entity(CasterSlot()) + 0xE);
		Muzzle()[0] = (int16_t)(Muzzle()[0] + (int16_t)(mul32(ComputeSin(ry), -100) >> 12));
		const int32_t rz = *(const int16_t *)(Entity(CasterSlot()) + 0xE);
		Muzzle()[2] = (int16_t)(Muzzle()[2] + (int16_t)(mul32(ComputeCos(rz), -100) >> 12));
	}

	static uint32_t __cdecl MasterTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		ComputeMuzzle();
		if (UpdateFlags() & 0x201) return 0;
		if (n->counter == 1)
		{
			BindEntityModelFromComData(Entity(CasterSlot()), game_addr(BIND_Block), game_addr(COM_Anim));
			QueueChainTransformation(Entity(CasterSlot()), 1);
		}
		if (n->counter >= 9 && n->counter <= 0x20 && (n->counter & 1))
		{
			Node *m = AddTask(game_addr(ORIG_MuzzleTask));
			*(uint32_t *)&m->f10 = rd32(&Muzzle()[0]);
			m->counter = 0;
			*(uint32_t *)&m->f14 = rd32(&Muzzle()[2]);
			m->f18 = *(const int16_t *)(Entity(CasterSlot()) + 0xE);
			m->f1A = (int16_t)(CrtRand() % 0x800);
			m->f1C = (int16_t)(CrtRand() % 0x280 + 0x600);
		}
		if (n->counter == 9) AddTask(game_addr(ORIG_SmokeTask))->counter = 0;
		if (n->counter == 8)
		{
			int16_t t[4];
			GetDefaultEffectPosition(Entity(TargetSlot()), t);
			const int16_t dx = (int16_t)(((int32_t)t[0] - Muzzle()[0]) / 6);
			const int16_t dz = (int16_t)(((int32_t)t[2] - Muzzle()[2]) / 6);
			for (int k = 1; k <= 5; k++)
			{
				Node *a = AddTask(game_addr(ORIG_MissTask));
				a->counter = 0;
				a->f0E = (int16_t)(2 * k);
				a->f12 = 0;
				a->f10 = (int16_t)(k * dx + Muzzle()[0]);
				a->f18 = (int16_t)k;
				a->f14 = (int16_t)(k * dz + Muzzle()[2]);
			}
		}
		if (n->counter == 0x12)
		{
			int16_t t[4];
			GetDefaultEffectPosition(Entity(TargetSlot()), t);
			const uint8_t *e = Entity(TargetSlot());
			const int16_t dy = (int16_t)(((int32_t)*(const int16_t *)(e + 0x36) - *(const int16_t *)(e + 0x3C)) / 8);
			for (int k = 1; k <= 7; k++)
			{
				Node *h = AddTask(game_addr(ORIG_HitTask));
				h->counter = 0;
				h->f0E = (int16_t)(2 * k);
				h->f10 = t[0];
				h->f12 = *(const int16_t *)(Entity(TargetSlot()) + 0x3C);
				const int32_t r = CrtRand() % 100;
				h->f12 = (int16_t)(h->f12 + (int16_t)(k * dy - r));
				h->f18 = (int16_t)k;
				h->f14 = t[2];
			}
		}
		if (n->counter <= 8) SetScreenFlash((uint32_t)(n->counter * 0xC0), 0);
		else if (n->counter >= 0x1C) SetScreenFlash((uint32_t)((0x24 - n->counter) * 0xC0), 0);
		if (n->counter == 0x1E) ApplyActionResultToTarget(Ctx()->actions[0].targets);
		if (n->counter == 8) BdPlaySE(SOUND_Gun(), 0, 0x80);
		n->counter++;
		if (n->counter <= 0x24) return 0;
		SetScreenFlash(0, 0);
		return TASK_END;
	}
	static uint32_t __cdecl RootTask(TaskNode *n_)
	{
		RootNode *r = (RootNode *)n_;
		Cursor() = game_addr(ARENA_A);
		if (!(*(const uint8_t *)&r->counter & 1)) Cursor() = game_addr(ARENA_B);
		const int a = ExecuteTaskQueue(QEffects());
		r->counter++;
		return a ? 0 : TASK_END;
	}
}

	void register_mag305_gatling_gun_quistis()
	{
		register_port(gg305::ORIG_RootTask, (void *)gg305::RootTask, "G305 RootTask", 305);
		register_port(gg305::ORIG_MasterTask, (void *)gg305::MasterTask, "G305 MasterTask", 305);
		register_port(gg305::ORIG_MuzzleTask, (void *)gg305::MuzzleTask, "G305 MuzzleTask", 305);
		register_port(gg305::ORIG_SmokeTask, (void *)gg305::SmokeTask, "G305 SmokeTask", 305);
		register_port(gg305::ORIG_MissTask, (void *)gg305::MissTask, "G305 MissTask", 305);
		register_port(gg305::ORIG_ImpactTask, (void *)gg305::ImpactTask, "G305 ImpactTask", 305);
		register_port(gg305::ORIG_FlashTask, (void *)gg305::FlashTask, "G305 FlashTask", 305);
		register_port(gg305::ORIG_DustTask, (void *)gg305::DustTask, "G305 DustTask", 305);
		register_port(gg305::ORIG_HitTask, (void *)gg305::HitTask, "G305 HitTask", 305);
		register_port(gg305::ORIG_SparkTask, (void *)gg305::SparkTask, "G305 SparkTask", 305);
		register_port(gg305::ORIG_SparksTask, (void *)gg305::SparksTask, "G305 SparksTask", 305);
	}
}

