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

// Effect 158: Melt-Eye

#include "mag_common.h"

namespace ff8fx
{
namespace melteye158
{
	using namespace eng;
	using namespace magc;

	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::unk_23DB5B0)); }
	inline uint32_t &CasterSlot() { return var<uint32_t>(game_addr(game::unk_23DB5B4)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::unk_23EB5BC)); }
	inline TaskQueue *EffectQueue() { return (TaskQueue *)game_addr(game::unk_23D8C10); }
	static constexpr game::Id ARENA0 = game::unk_23DB5BC, ARENA1 = game::unk_23E35BC; static const uint32_t ARENA_SIZE = 0x8000;
	static constexpr game::Id DROPS = game::unk_23D7B30, DROPS_END = game::unk_23D82B0;
	static constexpr game::Id SPARKS = game::unk_23D82B0, SPARKS_END = game::unk_23D8C10;
	static constexpr game::Id BEAMS = game::unk_23D9A30;
	static const int DROP_COUNT = 80, SPARK_COUNT = 100;

	static constexpr game::Id ORIG_RootTask = game::au_re_BdlinkTask_23;
	static constexpr game::Id ORIG_Director = game::MAG_158_sub_602FA0;
	static constexpr game::Id ORIG_Beam = game::MAG_158_sub_6030E0;
	static constexpr game::Id ORIG_Ring = game::MAG_158_sub_6034E0;
	static constexpr game::Id ORIG_Trail = game::MAG_158_sub_6036F0;
	static constexpr game::Id ORIG_Splash = game::MAG_158_sub_603920;
	static constexpr game::Id ORIG_SplashModel = game::MAG_158_sub_6039B0;
	static constexpr game::Id ORIG_Drops = game::MAG_158_sub_603B00;
	static constexpr game::Id EYES = game::unk_DB07C0;
	static constexpr game::Id MODEL_Ring = game::unk_DB0158, MODEL_Splash = game::unk_DB0548;
	static constexpr game::Id SEQ_Trail = game::unk_DB007C, SEQ_Splash = game::unk_DAFCAC, SEQ_Drop = game::unk_DAFED0;
	static const void *SOUND_MeltEye() { return (const void *)game_addr(game::unk_DB07B8); }

	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t frac, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, frac, out); }
	inline uint32_t MatrixMultiplyVector(const void *m, const void *in, void *out) { return fn<uint32_t (__cdecl *)(const void *, const void *, void *)>(game_addr(game::matrixMultiplyVector))(m, in, out); }
	inline void CalculateCenterPosition(uint32_t mask, int16_t *out) { fn<void (__cdecl *)(uint32_t, int16_t *)>(game_addr(game::CalculateCenterPosition))(mask, out); }
	inline void SplineSetup(int32_t n, const void *points, void *work) { fn<void (__cdecl *)(int32_t, const void *, void *)>(game_addr(game::Effect_SplineSetup))(n, points, work); }
	inline void SplineEval(int32_t n, void *work, void *out, int32_t t) { fn<void (__cdecl *)(int32_t, void *, void *, int32_t)>(game_addr(game::Effect_SplineEval))(n, work, out, t); }

#pragma pack(push, 1)
	struct RootNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad;
	};
	struct EffectNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t w0E;
		int16_t pos[4];
		int16_t w18, w1A, w1C, w1E;
		uint32_t pad20;
	};
	struct Beam
	{
		int16_t start[4];
		int16_t end[4];
		int16_t bone[4];
		int16_t ctrl[4][4];
		int16_t path[48][4];
	};
	struct Drop
	{
		uint32_t mask;
		int16_t frame;
		int16_t size;
		int16_t pos[4];
		int16_t vel[3];
		int16_t pad16;
	};
	struct Spark
	{
		uint32_t mask;
		int16_t frame;
		int16_t size;
		int16_t pos[4];
		uint8_t pad10[8];
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x10 && sizeof(EffectNode) == 0x24, "Melt-Eye nodes");
	static_assert(sizeof(Beam) == 0x1B8 && sizeof(Drop) == 0x18 && sizeof(Spark) == 0x18, "Melt-Eye tables");

	inline Beam *BeamRec(int16_t i) { return (Beam *)(game_addr(BEAMS) + (uint32_t)(int32_t)i * sizeof(Beam)); }
	inline Drop *Drops() { return (Drop *)game_addr(DROPS); }
	inline Spark *Sparks() { return (Spark *)game_addr(SPARKS); }
	inline uint8_t *Caster() { return Entity((int)CasterSlot()); }
	inline const void *CasterMatrix() { return Caster() + 0x40; }
	inline const int16_t *Eye(int32_t i) { return (const int16_t *)(game_addr(EYES) + (uint32_t)i * 8); }

	static uint8_t *ModelHeader(uint32_t model, int16_t counter, int16_t from, int32_t step)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		*(uint32_t *)h = model;
		*(int32_t *)(h + 8) = 0;
		*(int32_t *)(h + 0x1C) = 0x33;
		if (counter >= from)
		{
			*(int32_t *)(h + 0x1C) = 0xF3;
			*(int32_t *)(h + 0xC) = ((int32_t)counter - from) * step;
		}
		return h;
	}

	static void RingMatrix(const EffectNode *t)
	{
		int16_t ang[3] = { 0, 0, 0 };
		Mat4x3 m;
		ComposeZYXRotationMatrix(ang, &m);
		int32_t sc[3];
		sc[2] = t->w1C;
		sc[1] = t->w1C;
		sc[0] = t->w1C;
		m.t[0] = 0;
		m.t[1] = 0;
		m.t[2] = 0;
		Scale3DMatrix(&m, sc);
		const int32_t ref[3] = { 0, 0, -0x1000 };
		const Beam *b = BeamRec(t->w0E);
		int32_t dir[3];
		dir[0] = b->path[0][0] - t->pos[0];
		dir[1] = b->path[0][1] - t->pos[1];
		dir[2] = b->path[0][2] - t->pos[2];
		NormalizeVector(dir, dir);
		int32_t axis[3];
		const int32_t angle = RotationBetweenVectors(ref, dir, axis);
		Mat4x3 r;
		BuildAxisAngleRotationMatrix(angle, &r, axis);
		r.t[0] = b->start[0];
		r.t[1] = b->start[1];
		r.t[2] = b->start[2];
		ComposeAffineTransform(&r, &m, &m);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
	}
	static void RenderModel(uint8_t *h, int mode)
	{
		PacketCursor() = RenderPrimModel(h, RenderOT(), mode, PacketCursor());
		FieldFree(0x58);
	}
	static void RingDraw(const EffectNode *t)
	{
		RingMatrix(t);
		RenderModel(ModelHeader(game_addr(MODEL_Ring), t->counter, 8, 682), -2);
	}

	static void SplashModelMatrix(const EffectNode *t)
	{
		int16_t ang[3];
		ang[0] = 0x400;
		ang[1] = t->w18;
		ang[2] = 0;
		Mat4x3 m;
		ComposeZYXRotationMatrix(ang, &m);
		m.t[0] = t->pos[0];
		m.t[1] = t->pos[1];
		int32_t sc[3];
		sc[2] = t->w1C;
		sc[1] = t->w1C;
		sc[0] = t->w1C;
		m.t[2] = t->pos[2];
		Scale3DMatrix(&m, sc);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
	}
	static void SplashModelDraw(const EffectNode *t)
	{
		SplashModelMatrix(t);
		RenderModel(ModelHeader(game_addr(MODEL_Splash), t->counter, 6, 0x200), 2);
	}

	static void SpriteAt(uint8_t *h, const int16_t *pos, int32_t size, int shift)
	{
		TransformCameraByShadowRotation(pos, size, -(size >> shift));
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
	}
	static uint8_t *SeqHeader(uint32_t seq, int16_t frame)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		*(uint32_t *)h = seq;
		*(int16_t *)(h + 4) = frame;
		*(int16_t *)(h + 0x24) = 0;
		return h;
	}

	static uint8_t *SplashDraw(const EffectNode *t)
	{
		TransformCameraByShadowRotation(t->pos, t->w1C, -((int32_t)t->w1C >> 3));
		uint8_t *h = SeqHeader(game_addr(SEQ_Splash), t->counter);
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
		FieldFree(0xB4);
		return h;
	}

	static int TrailPathDraw(uint8_t *h, const EffectNode *t, int32_t &j, int32_t &size)
	{
		const Beam *b = BeamRec(t->w0E);
		j = t->w18;
		size = t->w1C;
		int k = 0;
		for (; k < 7; k++, j--, size -= 0x30)
		{
			if (j >= 0x30) continue;
			if (j < 0) break;
			SpriteAt(h, b->path[j], size, 3);
		}
		return k;
	}
	static uint8_t *TrailHeader(const EffectNode *t)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		*(int16_t *)(h + 0x24) = 0;
		*(uint32_t *)h = game_addr(SEQ_Trail);
		*(int16_t *)(h + 4) = (int16_t)(t->counter & 3);
		return h;
	}

	static int FreeSpark()
	{
		int i = 0;
		for (uint32_t a = game_addr(SPARKS); a < game_addr(SPARKS_END); a += sizeof(Spark), i++)
			if (*(uint32_t *)a == 0) return i < SPARK_COUNT ? i : -1;
		return -1;
	}
	static int FreeDrop()
	{
		int i = 0;
		for (uint32_t a = game_addr(DROPS); a < game_addr(DROPS_END); a += sizeof(Drop), i++)
			if (*(uint32_t *)a == 0) return i < DROP_COUNT ? i : -1;
		return -1;
	}
}
}

namespace ff8fx
{
namespace melteye158
{
	static uint32_t __cdecl RingTask(TaskNode *nd)
	{
		EffectNode *t = (EffectNode *)nd;
		RingDraw(t);
		if (DrawOnly()) return 0;
		const int16_t d = t->w1E;
		t->w1C = (int16_t)(t->w1C - d);
		t->w1E = (int16_t)(d - (int16_t)(d / 8));
		t->counter++;
		return t->counter >= 0xE ? TASK_END : 0;
	}

	static uint32_t __cdecl SplashModelTask(TaskNode *nd)
	{
		EffectNode *t = (EffectNode *)nd;
		SplashModelDraw(t);
		if (DrawOnly()) return 0;
		const int16_t a = t->w1A, g = t->w1E;
		t->w18 = (int16_t)(t->w18 + a);
		t->w1C = (int16_t)(t->w1C + g);
		t->w1A = (int16_t)(a - (int16_t)(a >> 4));
		t->w1E = (int16_t)(g - (int16_t)(g / 8));
		t->counter++;
		return t->counter >= 0xE ? TASK_END : 0;
	}

	static uint32_t __cdecl SplashTask(TaskNode *nd)
	{
		EffectNode *t = (EffectNode *)nd;
		const uint8_t *h = SplashDraw(t);
		if (DrawOnly()) return 0;
		t->counter++;
		return *(const int16_t *)(h + 0x28) < 0 ? TASK_END : 0;
	}

	static uint32_t __cdecl TrailTask(TaskNode *nd)
	{
		EffectNode *t = (EffectNode *)nd;
		int alive = 0;
		const uint32_t mask = 1u << (t->w0E & 31);
		uint8_t *h = TrailHeader(t);
		int32_t j, size;
		const int walked = TrailPathDraw(h, t, j, size);
		if (!DrawOnly() && t->w18 < 0x37)
		{
			const int16_t *p = BeamRec(t->w0E)->path[0] + j * 4;
			for (int s = 0; s < 5; s++, p -= 4, j--)
			{
				const int i = FreeSpark();
				if (i < 0 || walked < 7 || j < 0) continue;
				Spark *sp = &Sparks()[i];
				const int16_t fr = (int16_t)(s + 4);
				sp->frame = fr;
				if (fr >= 8) continue;
				sp->mask = mask;
				sp->size = (int16_t)size;
				*(uint32_t *)&sp->pos[0] = *(const uint32_t *)&p[0];
				*(uint32_t *)&sp->pos[2] = *(const uint32_t *)&p[2];
			}
		}
		for (Spark *sp = Sparks(); (uint32_t)sp < game_addr(SPARKS_END); sp++)
		{
			if (!(sp->mask & mask)) continue;
			*(int16_t *)(h + 4) = sp->frame;
			SpriteAt(h, sp->pos, sp->size, 4);
			if (DrawOnly()) continue;
			sp->frame = (int16_t)(sp->frame + 2);
			if (sp->frame >= 8) sp->mask = 0;
			else alive++;
		}
		FieldFree(0xB4);
		if (DrawOnly()) return 0;
		const int16_t v = t->w1A;
		t->w18 = (int16_t)(t->w18 + v);
		if (v > 5) t->w1A = (int16_t)(v - 1);
		t->counter++;
		if (t->w18 >= 0x37 && alive == 0) return TASK_END;
		return 0;
	}

	static uint32_t __cdecl DropsTask(TaskNode *nd)
	{
		const uint16_t pad_residue = 0;
		EffectNode *t = (EffectNode *)nd;
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		int alive = 0;
		*(uint32_t *)h = game_addr(SEQ_Drop);
		*(int16_t *)(h + 0x24) = 0;
		const uint32_t mask = (uint32_t)(int32_t)t->w0E;
		for (Drop *d = Drops(); (uint32_t)d < game_addr(DROPS_END); d++)
		{
			if (!(d->mask & mask)) continue;
			*(int16_t *)(h + 4) = d->frame;
			SpriteAt(h, d->pos, d->size, 4);
			if (DrawOnly()) continue;
			d->frame++;
			if (*(const int16_t *)(h + 0x28) < 0)
			{
				d->mask = 0;
				continue;
			}
			d->size = (int16_t)(d->size - 4);
			d->pos[0] = (int16_t)(d->pos[0] + d->vel[0]);
			d->pos[1] = (int16_t)(d->pos[1] + d->vel[1]);
			d->pos[2] = (int16_t)(d->pos[2] + d->vel[2]);
			d->vel[0] = (int16_t)(d->vel[0] - (int16_t)(d->vel[0] >> 5));
			d->vel[1] = (int16_t)(d->vel[1] - (int16_t)(d->vel[1] >> 5));
			d->vel[2] = (int16_t)(d->vel[2] - (int16_t)(d->vel[2] >> 5));
			alive++;
		}
		FieldFree(0xB4);
		if (DrawOnly()) return 0;
		if (t->counter >= 0 && t->counter <= 0x10)
		{
			int16_t p1[4], p2[4];
			MatrixMultiplyVector(CasterMatrix(), Eye(t->w18), p1);
			MatrixMultiplyVector(CasterMatrix(), Eye(t->w18) + 4, p2);
			p1[3] = (int16_t)pad_residue;
			p1[0] = (int16_t)(p1[0] + t->pos[0]);
			p1[1] = (int16_t)(p1[1] + t->pos[1]);
			p1[2] = (int16_t)(p1[2] + t->pos[2]);
			p2[0] = (int16_t)(p2[0] + t->pos[0]);
			p2[1] = (int16_t)(p2[1] + t->pos[1]);
			p2[2] = (int16_t)(p2[2] + t->pos[2]);
			int32_t dir[3];
			dir[0] = p2[0] - p1[0];
			dir[1] = p2[1] - p1[1];
			dir[2] = p2[2] - p1[2];
			NormalizeVector(dir, dir);
			for (int k = 0; k < 2; k++)
			{
				const int i = FreeDrop();
				if (i < 0) break;
				Drop *d = &Drops()[i];
				d->mask = (uint32_t)(int32_t)t->w0E;
				d->frame = 0;
				d->size = (int16_t)(CrtRand() % 0x600 + 0x700);
				*(uint32_t *)&d->pos[0] = *(const uint32_t *)&p1[0];
				*(uint32_t *)&d->pos[2] = *(const uint32_t *)&p1[2];
				const int32_t speed = CrtRand() % 0x4B + 0x1E;
				int32_t r = CrtRand() % 0x28;
				d->vel[0] = (int16_t)(r + (mul32(speed, dir[0]) >> 12) - 0x14);
				r = CrtRand() % 0x28;
				d->vel[1] = (int16_t)(r + (mul32(speed, dir[1]) >> 12) - 0x14);
				r = CrtRand() % 0x28;
				d->vel[2] = (int16_t)(r + (mul32(speed, dir[2]) >> 12) - 0x14);
			}
		}
		t->counter++;
		if (t->counter >= 4 && alive == 0) return TASK_END;
		return 0;
	}

	static void BeamPath(EffectNode *t, Beam *b)
	{
		GetEffectSpawnPosition(Caster(), 0xF0, 0, b->bone);
		*(uint32_t *)&b->start[0] = *(const uint32_t *)&Eye(t->w18)[0];
		*(uint32_t *)&b->start[2] = *(const uint32_t *)&Eye(t->w18)[2];
		MatrixMultiplyVector(CasterMatrix(), b->start, b->start);
		b->start[0] = (int16_t)(b->start[0] + b->bone[0]);
		b->start[1] = (int16_t)(b->start[1] + b->bone[1]);
		b->start[2] = (int16_t)(b->start[2] + b->bone[2]);
		CalculateCenterPosition(Ctx()->target_mask, b->end);
		b->end[0] = (int16_t)(b->end[0] + (CrtRand() % 0x1450 - 0xA28));
		b->end[2] = (int16_t)(b->end[2] + (CrtRand() % 0x7D0 - 0x3E8));
		*(uint32_t *)&b->ctrl[0][0] = *(const uint32_t *)&b->start[0];
		*(uint32_t *)&b->ctrl[0][2] = *(const uint32_t *)&b->start[2];
		*(uint32_t *)&b->ctrl[3][0] = *(const uint32_t *)&b->end[0];
		*(uint32_t *)&b->ctrl[3][2] = *(const uint32_t *)&b->end[2];
		const int16_t x0 = b->start[0];
		const int32_t dx = ((int32_t)b->end[0] - x0) / 3;
		const int32_t dz = ((int32_t)b->end[2] - b->start[2]) / 3;
		b->ctrl[1][0] = (int16_t)(x0 + dx);
		const int32_t r1 = CrtRand() % 0xAF0;
		b->ctrl[1][1] = (int16_t)((int16_t)(b->start[1] - r1) - 0x1F4);
		b->ctrl[1][2] = (int16_t)(b->start[2] + dz);
		b->ctrl[2][0] = (int16_t)(dx * 2 + b->start[0]);
		const int32_t r2 = CrtRand() % 0x258;
		b->ctrl[2][1] = (int16_t)((int16_t)(b->ctrl[1][1] - r2) - 0x32);
		b->ctrl[2][2] = (int16_t)(dz * 2 + b->start[2]);
		void *work = FieldAlloc(0x190);
		SplineSetup(4, b->ctrl, work);
		for (int32_t u = 0, i = 0; u < 0x30000; u += 0x1000, i++)
			SplineEval(4, work, b->path[i], u / 47);
		FieldFree(0x190);
	}

	static uint32_t __cdecl BeamTask(TaskNode *nd)
	{
		EffectNode *t = (EffectNode *)nd;
		Beam *b = BeamRec(t->w0E);
		if (DrawOnly()) return 0;
		if (t->counter == 0) BeamPath(t, b);
		if (t->counter == 1)
		{
			EffectNode *n = (EffectNode *)AddTaskToQueue(EffectQueue(), game_addr(ORIG_Ring));
			const uint32_t e0 = *(const uint32_t *)game_addr(EYES), e1 = *(const uint32_t *)(game_addr(EYES) + 4);
			n->counter = 0;
			n->w0E = t->w0E;
			*(uint32_t *)&n->pos[0] = e0;
			*(uint32_t *)&n->pos[2] = e1;
			MatrixMultiplyVector(CasterMatrix(), n->pos, n->pos);
			n->pos[0] = (int16_t)(n->pos[0] + b->bone[0]);
			n->pos[1] = (int16_t)(n->pos[1] + b->bone[1]);
			n->pos[2] = (int16_t)(n->pos[2] + b->bone[2]);
			const int16_t s = (int16_t)(CrtRand() % 0x400 + 0x600);
			n->w1C = s;
			n->w1E = (int16_t)(s / 7);
		}
		if (t->counter == 7)
		{
			EffectNode *n = (EffectNode *)AddTaskToQueue(EffectQueue(), game_addr(ORIG_Trail));
			n->counter = 0;
			n->w0E = t->w0E;
			n->w18 = 0;
			n->w1A = 7;
			n->w1C = (int16_t)(CrtRand() % 0x200 + 0x900);
		}
		if (t->counter == 0x10)
		{
			EffectNode *n = (EffectNode *)AddTaskToQueue(EffectQueue(), game_addr(ORIG_Splash));
			n->counter = 0;
			n->pos[0] = b->end[0];
			const int32_t r = CrtRand() % 0x190;
			n->pos[2] = b->end[2];
			n->pos[1] = (int16_t)(-0x64 - r);
			n->w1C = (int16_t)(CrtRand() % 0x600 + 0xE00);
			EffectNode *m = (EffectNode *)AddTaskToQueue(EffectQueue(), game_addr(ORIG_SplashModel));
			*(uint32_t *)&m->pos[0] = *(const uint32_t *)&n->pos[0];
			m->counter = 0;
			*(uint32_t *)&m->pos[2] = *(const uint32_t *)&n->pos[2];
			m->w18 = (int16_t)(CrtRand() % 0x1000);
			const int16_t spin = (int16_t)(CrtRand() % 0x64 + 0x78);
			m->w1A = spin;
			if (m->w18 & 1) m->w1A = (int16_t)-spin;
			const int16_t g = (int16_t)((CrtRand() % 0x800 + 0x2000) / 6);
			m->w1E = g;
			m->w1C = g;
		}
		t->counter++;
		return t->counter >= 0x1E ? TASK_END : 0;
	}

	static uint32_t __cdecl DirectorTask(TaskNode *nd)
	{
		if (DrawOnly()) return 0;
		EffectNode *t = (EffectNode *)nd;
		if (t->counter >= 1 && t->counter <= 0xF)
		{
			EffectNode *n = (EffectNode *)AddTaskToQueue(EffectQueue(), game_addr(ORIG_Beam));
			n->counter = 0;
			n->w0E = t->w0E;
			const int32_t eye = CrtRand() % 6 + 1;
			t->w0E++;
			n->w18 = (int16_t)eye;
		}
		if (t->counter == 1)
		{
			GetEffectSpawnPosition(Caster(), 0xF0, 0, t->pos);
			for (int k = 0; k < 2; k++)
			{
				EffectNode *n = (EffectNode *)AddTaskToQueue(EffectQueue(), game_addr(ORIG_Drops));
				*(uint32_t *)&n->pos[0] = *(const uint32_t *)&t->pos[0];
				n->counter = 0;
				n->w0E = (int16_t)(k + 1);
				*(uint32_t *)&n->pos[2] = *(const uint32_t *)&t->pos[2];
				n->w18 = (int16_t)(k ? 9 : 7);
			}
		}
		if (t->counter == 0x28) ApplyActionResultToTargets(Ctx()->actions[0].targets, Ctx()->actions[0].target_count);
		if (t->counter == 1) BdPlaySE(SOUND_MeltEye(), 0, 0x80);
		t->counter++;
		return t->counter > 0x2A ? TASK_END : 0;
	}

	static uint32_t __cdecl RootTask(TaskNode *nd)
	{
		RootNode *r = (RootNode *)nd;
		PacketCursor() = game_addr(ARENA1);
		if (!(r->counter & 1)) PacketCursor() = game_addr(ARENA0);
		const int a = ExecuteTaskQueue(EffectQueue());
		r->counter++;
		return a ? 0 : TASK_END;
	}
}

	void register_mag158_melt_eye()
	{
		register_port(melteye158::ORIG_RootTask, (void *)melteye158::RootTask, "M158 RootTask", 158);
		register_port(melteye158::ORIG_Director, (void *)melteye158::DirectorTask, "M158 Director", 158);
		register_port(melteye158::ORIG_Beam, (void *)melteye158::BeamTask, "M158 Beam", 158);
		register_port(melteye158::ORIG_Ring, (void *)melteye158::RingTask, "M158 Ring", 158);
		register_port(melteye158::ORIG_Trail, (void *)melteye158::TrailTask, "M158 Trail", 158);
		register_port(melteye158::ORIG_Splash, (void *)melteye158::SplashTask, "M158 Splash", 158);
		register_port(melteye158::ORIG_SplashModel, (void *)melteye158::SplashModelTask, "M158 SplashModel", 158);
		register_port(melteye158::ORIG_Drops, (void *)melteye158::DropsTask, "M158 Drops", 158);
	}
}

