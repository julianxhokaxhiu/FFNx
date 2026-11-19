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

// Effect 190: Chain Gun

#include "mag_common.h"

namespace ff8fx
{
namespace cg190
{
	using namespace eng;
	using namespace magc;

	inline uint32_t &TargetSlot() { return var<uint32_t>(game_addr(game::g_ChainGun_TargetSlot)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::g_ChainGun_CastContext)); }
	inline uint32_t &CasterSlot() { return var<uint32_t>(game_addr(game::g_ChainGun_CasterSlot)); }
	inline int16_t *Muzzle() { return (int16_t *)game_addr(game::g_ChainGun_MuzzlePos); }
	inline uint32_t &Cursor() { return var<uint32_t>(game_addr(game::g_ChainGun_PacketCursor)); }
	inline TaskQueue *QEffects() { return (TaskQueue *)game_addr(game::g_ChainGun_EffectQueue); }
	static constexpr game::Id ARENA_A = game::g_ChainGun_PacketArenaA, ARENA_B = game::g_ChainGun_PacketArenaB;

	static constexpr game::Id ORIG_RootTask = game::MAG_190_ChainGun_RootTask;
	static constexpr game::Id ORIG_MasterTask = game::MAG_190_ChainGun_MasterTask;
	static constexpr game::Id ORIG_MuzzleTask = game::MAG_190_ChainGun_MuzzleFlashTask;
	static constexpr game::Id ORIG_SmokeTask = game::MAG_190_ChainGun_SmokeTask;
	static constexpr game::Id ORIG_MissTask = game::MAG_190_ChainGun_MissTask;
	static constexpr game::Id ORIG_ImpactTask = game::MAG_190_ChainGun_GroundImpactTask;
	static constexpr game::Id ORIG_FlashTask = game::MAG_190_ChainGun_ImpactFlashTask;
	static constexpr game::Id ORIG_DustTask = game::MAG_190_ChainGun_DustTask;
	static constexpr game::Id ORIG_HitTask = game::MAG_190_ChainGun_HitTask;
	static constexpr game::Id ORIG_SparkTask = game::MAG_190_ChainGun_SparkModelTask;
	static constexpr game::Id ORIG_SparksTask = game::MAG_190_ChainGun_SparksTask;
	static constexpr game::Id MODEL_Muzzle = game::unk_D18CDC;
	static constexpr game::Id MODEL_Impact = game::unk_D18FDC;
	static constexpr game::Id MODEL_Spark = game::unk_D192A4;
	static constexpr game::Id SEQ_Smoke = game::unk_D1899C;
	static constexpr game::Id SEQ_Flash = game::unk_D187A0;
	static constexpr game::Id SEQ_Sparks = game::unk_D18B48;
	static const void *SOUND_Gun() { return (const void *)game_addr(game::unk_D1956C); }

	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t flag, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, flag, out); }
	inline void MatrixMultiply3(Mat4x3 *a, const Mat4x3 *b) { fn<void (__cdecl *)(Mat4x3 *, const Mat4x3 *)>(game_addr(game::MatrixMultiply3x3))(a, b); }
	inline int32_t ComputeCos(int32_t a) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(a); }

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
	struct Shot
	{
		int32_t active;
		int32_t kind;
		int16_t pos[4];
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x10 && sizeof(Node) == 0x24, "Chain Gun nodes");
	static_assert(sizeof(Particle) == 0x18 && sizeof(Shot) == 0x10, "Chain Gun records");

	inline Particle *PartsA() { return (Particle *)game_addr(game::g_ChainGun_SmokeParticles); }
	inline Particle *PartsB() { return (Particle *)game_addr(game::g_ChainGun_SparkParticles); }
	inline Shot *Shots() { return (Shot *)game_addr(game::g_ChainGun_Shots); }
	inline int16_t *EntPos(int32_t slot) { return (int16_t *)(Entity(slot) + 0x1C); }
	inline uint32_t rd32(const void *p) { return *(const uint32_t *)p; }
	inline Node *AddTask(uint32_t fn) { return (Node *)AddTaskToQueue(QEffects(), fn); }
}
}

namespace ff8fx
{
namespace cg190
{
	static void FillShots(int32_t start, int32_t count, int32_t slot)
	{
		const int32_t end = start + count;
		for (int32_t i = start; i < end; i++)
		{
			Shot *s = &Shots()[i];
			s->active = 1;
			if (!(i & 1) && CrtRand() % 2 == 0)
			{
				s->kind = -1;
				*(uint32_t *)&s->pos[0] = *(const uint32_t *)(Entity(slot) + 0x1C);
				*(uint32_t *)&s->pos[2] = *(const uint32_t *)(Entity(slot) + 0x20);
				continue;
			}
			s->kind = slot;
			int32_t bone = 0;
			const uint8_t anchors = ***(const uint8_t ***)(Entity(slot) + 0x64);
			if (anchors) bone = CrtRand() % anchors;
			const int32_t angle = CrtRand() % 0x1000;
			GetEffectSpawnPosition(Entity(slot), bone, angle, s->pos);
		}
	}

	static void LineShots(int32_t start, int32_t count, const int16_t *a, const int16_t *b)
	{
		const int16_t ax = a[0];
		const int16_t dx = (int16_t)(((int32_t)b[0] - ax) / (count + 1));
		const int16_t az = a[2];
		const int16_t dz = (int16_t)(((int32_t)b[2] - az) / (count + 1));
		const int32_t end = start + count;
		for (int32_t k = 1; start + k - 1 < end; k++)
		{
			Shot *s = &Shots()[start + k - 1];
			s->active = 1;
			s->kind = -1;
			s->pos[0] = (int16_t)(k * dx + ax);
			s->pos[1] = 0;
			s->pos[2] = (int16_t)(k * dz + az);
		}
	}

	static void BuildShots()
	{
		for (Shot *s = Shots(); s < Shots() + 48; s++) s->active = 0;
		const ActionData *act = Ctx()->actions;
		const uint8_t *t = act->targets;
		switch (act->target_count)
		{
		case 1:
			FillShots(0, 0xC, t[0]);
			return;
		case 2:
		{
			int32_t lo = t[0], hi = t[TARGET_STRIDE];
			if (hi < lo) { const int32_t x = lo; lo = hi; hi = x; }
			switch ((1 << hi) | (1 << lo))
			{
			case 3:
				FillShots(0, 5, hi);
				LineShots(5, 2, EntPos(hi), EntPos(lo));
				FillShots(7, 5, lo);
				break;
			case 5:
			{
				int16_t mid[4];
				mid[0] = (int16_t)(((int32_t)EntPos(lo)[0] + EntPos(hi)[0]) / 2);
				mid[1] = 0;
				mid[2] = (int16_t)(((int32_t)EntPos(lo)[2] + EntPos(hi)[2]) / 2);
				mid[3] = 0;
				LineShots(0, 2, mid, EntPos(hi));
				FillShots(2, 3, hi);
				LineShots(5, 4, EntPos(hi), EntPos(lo));
				FillShots(9, 3, lo);
				break;
			}
			case 6:
				FillShots(0, 2, lo);
				LineShots(2, 2, EntPos(lo), EntPos(hi));
				FillShots(4, 3, hi);
				LineShots(7, 2, EntPos(hi), EntPos(lo));
				FillShots(9, 3, lo);
				break;
			}
			break;
		}
		case 3:
			break;
		default:
			return;
		}
		int32_t lo = t[0], mid = t[TARGET_STRIDE], hi = t[2 * TARGET_STRIDE];
		if (mid < lo) { const int32_t x = lo; lo = mid; mid = x; }
		if (hi < mid) { const int32_t x = mid; mid = hi; hi = x; }
		if (mid < lo) { const int32_t x = lo; lo = mid; mid = x; }
		FillShots(0, 2, mid);
		LineShots(2, 2, EntPos(mid), EntPos(hi));
		FillShots(4, 3, hi);
		LineShots(7, 2, EntPos(hi), EntPos(lo));
		FillShots(9, 3, lo);
	}

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
		*(int16_t *)(h + 0x24) = 0;
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
				e->angle = (int16_t)(CrtRand() % 0x600 + 0x800);
				*(uint32_t *)&e->pos[0] = rd32(&Muzzle()[0]);
				*(uint32_t *)&e->pos[2] = rd32(&Muzzle()[2]);
				int32_t v[3];
				v[0] = CrtRand() % 0x1000 - 0x800;
				v[1] = CrtRand() % 0x1000 - 0x800;
				v[2] = CrtRand() % 0x1000 - 0x800;
				NormalizeVector(v, v);
				const int32_t r = CrtRand() % 200;
				e->pos[0] = (int16_t)(e->pos[0] + (mul32(r, v[0]) >> 12));
				e->pos[1] = (int16_t)(e->pos[1] + (mul32(r, v[1]) >> 12));
				e->pos[2] = (int16_t)(e->pos[2] + (mul32(r, v[2]) >> 12));
				const int32_t sp = CrtRand() % 0x50 + 0x1E;
				e->vel[0] = (int16_t)(mul32(sp, v[0]) >> 12);
				const int32_t up = CrtRand() % 0x28;
				e->vel[1] = (int16_t)(-15 - up);
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
		d->f0E = n->f18;
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
			if (e->id != (int32_t)n->f0E) continue;
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
		g->f0E = n->f18;
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
			if (e->id != (int32_t)n->f0E) continue;
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

	static uint32_t __cdecl MasterTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		GetEffectSpawnPosition(Entity(CasterSlot()), 0xB, 0x1800, Muzzle());
		if (UpdateFlags() & 0x201) return 0;
		if (n->counter == 0)
		{
			BuildShots();
			n->f0E = 0;
		}
		if (n->counter >= 2 && n->counter <= 0x19 && (n->counter & 1))
		{
			Node *m = AddTask(game_addr(ORIG_MuzzleTask));
			*(uint32_t *)&m->f10 = rd32(&Muzzle()[0]);
			m->counter = 0;
			*(uint32_t *)&m->f14 = rd32(&Muzzle()[2]);
			m->f18 = *(const int16_t *)(Entity(CasterSlot()) + 0xE);
			m->f1A = (int16_t)(CrtRand() % 0x800);
			m->f1C = (int16_t)(CrtRand() % 0x500 + 0xA00);
			const int32_t i = n->f0E;
			const Shot *s = &Shots()[i];
			if (s->active)
			{
				if (s->kind >= 0)
				{
					Node *t = AddTask(game_addr(ORIG_HitTask));
					*(uint32_t *)&t->f10 = rd32(&s->pos[0]);
					t->counter = 0;
					t->f0E = 0;
					*(uint32_t *)&t->f14 = rd32(&s->pos[2]);
					t->f18 = (int16_t)(i + 1);
				}
				else
				{
					Node *t = AddTask(game_addr(ORIG_MissTask));
					t->counter = 0;
					t->f0E = 0;
					*(uint32_t *)&t->f10 = rd32(&s->pos[0]);
					*(uint32_t *)&t->f14 = rd32(&s->pos[2]);
					t->f10 = (int16_t)(t->f10 + (CrtRand() % 1000 - 500));
					t->f14 = (int16_t)(t->f14 + (CrtRand() % 1000 - 500));
					t->f18 = (int16_t)(i + 1);
				}
				n->f0E++;
			}
		}
		if (n->counter == 2) AddTask(game_addr(ORIG_SmokeTask))->counter = 0;
		if (n->counter == 0x1E) ApplyActionResultToTargets(Ctx()->actions[0].targets, Ctx()->actions[0].target_count);
		if (n->counter == 1) BdPlaySE(SOUND_Gun(), 0, 0x80);
		n->counter++;
		return n->counter > 0x24 ? TASK_END : 0;
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

	void register_mag190_chain_gun()
	{
		register_port(cg190::ORIG_RootTask, (void *)cg190::RootTask, "C190 RootTask", 190);
		register_port(cg190::ORIG_MasterTask, (void *)cg190::MasterTask, "C190 MasterTask", 190);
		register_port(cg190::ORIG_MuzzleTask, (void *)cg190::MuzzleTask, "C190 MuzzleTask", 190);
		register_port(cg190::ORIG_SmokeTask, (void *)cg190::SmokeTask, "C190 SmokeTask", 190);
		register_port(cg190::ORIG_MissTask, (void *)cg190::MissTask, "C190 MissTask", 190);
		register_port(cg190::ORIG_ImpactTask, (void *)cg190::ImpactTask, "C190 ImpactTask", 190);
		register_port(cg190::ORIG_FlashTask, (void *)cg190::FlashTask, "C190 FlashTask", 190);
		register_port(cg190::ORIG_DustTask, (void *)cg190::DustTask, "C190 DustTask", 190);
		register_port(cg190::ORIG_HitTask, (void *)cg190::HitTask, "C190 HitTask", 190);
		register_port(cg190::ORIG_SparkTask, (void *)cg190::SparkTask, "C190 SparkTask", 190);
		register_port(cg190::ORIG_SparksTask, (void *)cg190::SparksTask, "C190 SparksTask", 190);
	}
}

