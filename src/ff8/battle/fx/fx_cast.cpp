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

// Magic cast glow

#include "mag_common.h"

namespace ff8fx
{
namespace cast
{
	using namespace eng;
	using namespace magc;

	static constexpr game::Id ORIG_Spawner = game::sub_574C80;
	static constexpr game::Id ORIG_Sparkle = game::sub_574F50;
	static constexpr game::Id ORIG_Trail = game::sub_5751A0;
	static constexpr game::Id ORIG_Glow = game::sub_575250;
	static constexpr game::Id ORIG_Ring = game::sub_5753A0;
	static TaskQueue *QUEUE() { return (TaskQueue *)game_addr(game::TASK_QUEUE_ANIM_SEQ_99_B0_B1_B4_A5); }

#pragma pack(push, 1)
	struct SpawnerNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		int16_t pos[4];
		uint8_t pad18[8];
		uint8_t *ent;
	};
	struct RingNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t delay;
		int16_t pos[4];
		int16_t angle;
		int16_t pad1A;
		int16_t scale_xz;
		int16_t pad1E;
		int16_t scale_y;
		int16_t grow_y;
	};
	struct GlowNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		int16_t pos[4];
		uint8_t pad18[4];
		int16_t scale;
		int16_t grow;
		uint8_t pad20[4];
	};
	struct SparkleNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t delay;
		int16_t pos[3];
		int16_t speed_y;
		int16_t angle;
		int16_t spin;
		int16_t size;
		int16_t pad1E;
		int16_t radius;
		int16_t frame;
	};
	struct TrailNode
	{
		TaskNode hdr;
		int16_t left;
		int16_t pad0E;
		int16_t pos[3];
		int16_t speed_y;
		uint8_t pad18[4];
		int16_t size;
		int16_t pad1E;
		uint8_t pad20[4];
	};
#pragma pack(pop)
	static_assert(sizeof(SpawnerNode) == 0x24 && sizeof(RingNode) == 0x24 && sizeof(GlowNode) == 0x24, "cast nodes");
	static_assert(sizeof(SparkleNode) == 0x24 && sizeof(TrailNode) == 0x24, "cast nodes");

	inline bool Paused() { return (UpdateFlags() & 1) != 0; }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)); }
	inline void BdPlaySystemSE3D(int32_t id, int32_t a, const int16_t *pos) { fn<void (__cdecl *)(int32_t, int32_t, const int16_t *)>(game_addr(game::BdPlaySystemSE3D))(id, a, pos); }
	inline uint32_t EffectPrimModel(int id) { return fn<uint32_t (__cdecl *)(int)>(game_addr(game::sub_571240))(id); }
	inline void MatrixMulVector(const Mat4x3 *m, const int16_t *v, int16_t *out) { fn<void (__cdecl *)(const Mat4x3 *, const int16_t *, int16_t *)>(game_addr(game::matrixMultiplyVector))(m, v, out); }

	static int32_t SizeScale(const uint8_t *ent)
	{
		int32_t s = (int32_t)*(const int16_t *)(ent + 0x26) * 9011 >> 12;
		return s > 0x7FFF ? 0x7FFF : s;
	}

	static void ModelMatrix(int16_t angle_y, const int16_t *pos, int32_t sx, int32_t sy)
	{
		int16_t ang[4] = {};
		Mat4x3 m;
		ang[0] = 0;
		ang[1] = angle_y;
		ang[2] = 0;
		BuildRotationMatrixFromAngles(ang, &m);
		m.t[0] = pos[0];
		m.t[1] = pos[1];
		m.t[2] = pos[2];
		int32_t sv[3] = { sx, sy, sx };
		Scale3DMatrix(&m, sv);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
	}

	static void RingHeader(uint8_t *h, const RingNode *r)
	{
		*(uint32_t *)h = EffectPrimModel(1);
		*(uint32_t *)(h + 0x14) = 0x3867;
		*(uint32_t *)(h + 0x1C) = 0x230;
		if (r->counter >= 5)
		{
			const int32_t c = (int32_t)r->counter - 5;
			h[0xA] = 0;
			h[9] = 0;
			h[8] = 0;
			*(uint32_t *)(h + 0x1C) = 0x2F0;
			*(int32_t *)(h + 0xC) = c * 409;
		}
	}

	static void GlowHeader(uint8_t *h, const GlowNode *g)
	{
		*(uint32_t *)h = EffectPrimModel(7);
		*(uint32_t *)(h + 0x1C) = 0x33;
		if (g->counter >= 0)
		{
			h[0xA] = 0;
			h[9] = 0;
			h[8] = 0;
			*(uint32_t *)(h + 0x1C) = 0xF3;
			*(int32_t *)(h + 0xC) = (int32_t)g->counter * 292;
		}
	}

	static void RenderModel(uint8_t *h)
	{
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static void SparklePoint(const SparkleNode *s, int16_t v[4])
	{
		int16_t ang[4] = {};
		Mat4x3 m;
		v[2] = (int16_t)-s->radius;
		v[0] = 0;
		v[1] = 0;
		v[3] = 0;
		ang[0] = 0;
		ang[1] = s->angle;
		ang[2] = 0;
		BuildRotationMatrixFromAngles(ang, &m);
		MatrixMulVector(&m, v, v);
		v[2] = (int16_t)(v[2] + s->pos[2]);
		v[0] = (int16_t)(v[0] + s->pos[0]);
		v[1] = (int16_t)(v[1] + s->pos[1]);
	}

	static void SpriteDraw(const int16_t *pos, int16_t size, int16_t frame)
	{
		TransformCameraByShadowRotation(pos, size, -((int32_t)size >> 3));
		uint8_t *h = AllocHeader(0xB4);
		*(void **)h = EffectSpriteSharedSequence(5);
		*(int16_t *)(h + 4) = frame;
		*(uint16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static uint32_t RingStep(RingNode *r)
	{
		const int16_t g = r->grow_y;
		r->scale_y = (int16_t)(r->scale_y + g);
		r->grow_y = (int16_t)(g + (int16_t)(g >> 5));
		r->counter++;
		return r->counter > 0xF ? TASK_END : 0;
	}

	static uint32_t GlowStep(GlowNode *g)
	{
		const int16_t d = g->grow;
		g->scale = (int16_t)(g->scale + d);
		g->grow = (int16_t)(d - (int16_t)((int32_t)d / 8));
		g->counter++;
		return g->counter > 0xE ? TASK_END : 0;
	}

	static uint32_t SparkleStep(SparkleNode *s)
	{
		const int16_t v = s->speed_y;
		s->pos[1] = (int16_t)(s->pos[1] + v);
		s->speed_y = (int16_t)(v + (int16_t)(v >> 5));
		s->angle = (int16_t)(s->angle + s->spin);
		s->radius = (int16_t)(s->radius + (int16_t)(s->radius >> 6));
		const int16_t c = s->counter;
		if (c >= 0xC) s->frame++;
		if (s->frame > 9) s->frame = 0;
		s->counter = (int16_t)(c + 1);
		return s->counter > 0x12 ? TASK_END : 0;
	}

	static uint32_t TrailStep(TrailNode *t)
	{
		const int16_t v = t->speed_y;
		t->size = (int16_t)(t->size - 0x40);
		t->pos[1] = (int16_t)(t->pos[1] + v);
		t->speed_y = (int16_t)(v + (int16_t)(v >> 4));
		t->left--;
		return t->left < 0 ? TASK_END : 0;
	}
}
}

namespace ff8fx
{
namespace cast
{
	static uint32_t __cdecl SpawnerTask(TaskNode *n)
	{
		if (Paused()) return 0;
		SpawnerNode *sp = (SpawnerNode *)n;
		uint8_t *ent = sp->ent;
		if (sp->counter == 0)
		{
			GetDefaultEffectPosition(ent, sp->pos);
			BdPlaySystemSE3D(5, 0, sp->pos);
			BdPlaySystemSE3D(6, 0, sp->pos);
			if (sp->counter != 0) goto done;
			{
				const int32_t base = SizeScale(ent);
				const int32_t step = base / 5;
				int32_t div = 8;
				int32_t i = 0;
				do
				{
					RingNode *r = (RingNode *)AddTaskToQueue(QUEUE(), game_addr(ORIG_Ring));
					r->delay = (int16_t)(i * 3);
					*(uint32_t *)&r->pos[0] = *(const uint32_t *)&sp->pos[0];
					*(uint32_t *)&r->pos[2] = *(const uint32_t *)&sp->pos[2];
					r->counter = 0;
					r->pos[1] = *(const int16_t *)(ent + 0x24);
					r->angle = (int16_t)(CrtRand() % 2048);
					r->scale_xz = (int16_t)(i * step + base);
					const int32_t q = base / div;
					r->scale_y = 0;
					div += 2;
					r->grow_y = (int16_t)(q - i * 8);
					i++;
				} while (div < 0x12);
			}
			if (sp->counter != 0) goto done;
			{
				const int32_t s = SizeScale(ent);
				GlowNode *g = (GlowNode *)AddTaskToQueue(QUEUE(), game_addr(ORIG_Glow));
				*(uint32_t *)&g->pos[0] = *(const uint32_t *)&sp->pos[0];
				*(uint32_t *)&g->pos[2] = *(const uint32_t *)&sp->pos[2];
				g->pos[1] = *(const int16_t *)(ent + 0x24);
				const int16_t s2 = (int16_t)(s + s);
				g->counter = 0;
				g->scale = s2;
				g->grow = (int16_t)((int32_t)s2 / 4);
			}
			if (sp->counter != 0) goto done;
			{
				int32_t r0 = (int32_t)*(const int16_t *)(ent + 0x26) * 10012 >> 12;
				if (r0 > 0x7FFF) r0 = 0x7FFF;
				const int32_t radius = r0 * 900 >> 12;
				for (int32_t i = 0; i < 6; i++)
				{
					SparkleNode *s = (SparkleNode *)AddTaskToQueue(QUEUE(), game_addr(ORIG_Sparkle));
					s->counter = 0;
					s->delay = (int16_t)(CrtRand() % 6 + i);
					*(uint32_t *)&s->pos[0] = *(const uint32_t *)&sp->pos[0];
					*(uint32_t *)&s->pos[2] = *(const uint32_t *)&sp->pos[2];
					s->pos[1] = *(const int16_t *)(ent + 0x24);
					s->pos[1] = (int16_t)(s->pos[1] + (-400 - CrtRand() % 150));
					s->speed_y = (int16_t)(-(CrtRand() % 40) - (i * 4 + 40));
					s->angle = (int16_t)((CrtRand() % 400 + 400) * i);
					const int32_t sp_r = CrtRand() % 200;
					s->size = 0x1000;
					s->radius = (int16_t)radius;
					s->frame = 0;
					s->spin = (int16_t)(sp_r + 0x50);
				}
			}
		}
	done:
		sp->counter++;
		return sp->counter > 0x1E ? TASK_END : 0;
	}

	static uint32_t __cdecl SparkleTask(TaskNode *n)
	{
		SparkleNode *s = (SparkleNode *)n;
		if (s->delay > 0)
		{
			if (Paused()) return 0;
			s->delay--;
			return 0;
		}
		int16_t v[4];
		SparklePoint(s, v);
		SpriteDraw(v, s->size, s->frame);
		if (Paused()) return 0;
		const int16_t c = s->counter;
		if (c >= 1 && c <= 0xC)
		{
			for (int k = 3; k != 0; k--)
			{
				TrailNode *t = (TrailNode *)AddTaskToQueue(QUEUE(), game_addr(ORIG_Trail));
				if (!t) continue;
				t->left = (int16_t)(CrtRand() % 3 + 4);
				*(uint32_t *)&t->pos[0] = *(const uint32_t *)&v[0];
				*(uint32_t *)&t->pos[2] = *(const uint32_t *)&v[2];
				t->pos[0] = (int16_t)(t->pos[0] + (0x1E - CrtRand() % 0x3C));
				t->pos[1] = (int16_t)(t->pos[1] + (0x1E - CrtRand() % 0x3C));
				const int32_t dz = CrtRand() % 0x3C;
				t->speed_y = (int16_t)(s->speed_y >> 2);
				t->pos[2] = (int16_t)(t->pos[2] + (0x1E - dz));
				t->size = (int16_t)(CrtRand() % 0x320 + 0x840);
			}
		}
		return SparkleStep(s);
	}

	static uint32_t __cdecl TrailTask(TaskNode *n)
	{
		TrailNode *t = (TrailNode *)n;
		SpriteDraw(t->pos, t->size, (int16_t)(9 - t->left));
		if (Paused()) return 0;
		return TrailStep(t);
	}

	static uint32_t __cdecl GlowTask(TaskNode *n)
	{
		GlowNode *g = (GlowNode *)n;
		ModelMatrix(0, g->pos, g->scale, 0x1000);
		uint8_t *h = AllocHeader(0x58);
		GlowHeader(h, g);
		RenderModel(h);
		if (Paused()) return 0;
		return GlowStep(g);
	}

	static uint32_t __cdecl RingTask(TaskNode *n)
	{
		RingNode *r = (RingNode *)n;
		if (r->delay > 0)
		{
			if (Paused()) return 0;
			r->delay--;
			return 0;
		}
		ModelMatrix(r->angle, r->pos, r->scale_xz, r->scale_y);
		uint8_t *h = AllocHeader(0x58);
		RingHeader(h, r);
		RenderModel(h);
		if (Paused()) return 0;
		return RingStep(r);
	}
}

	void register_fx_cast()
	{
		register_port(cast::ORIG_Spawner, (void *)cast::SpawnerTask, "cast 574C80 CastGlow_Spawner", 0);
		register_port(cast::ORIG_Sparkle, (void *)cast::SparkleTask, "cast 574F50 CastGlow_Sparkle", 0);
		register_port(cast::ORIG_Trail, (void *)cast::TrailTask, "cast 5751A0 CastGlow_SparkleTrail", 0);
		register_port(cast::ORIG_Glow, (void *)cast::GlowTask, "cast 575250 CastGlow_Glow", 0);
		register_port(cast::ORIG_Ring, (void *)cast::RingTask, "cast 5753A0 CastGlow_Ring", 0);
	}
}

