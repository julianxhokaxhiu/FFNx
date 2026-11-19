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

// Effect 182: Card

#include "mag_common.h"

namespace ff8fx
{
namespace card182
{
	using namespace eng;
	using namespace magc;

	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::MAG_182_CARD_CastContext)); }
	inline uint32_t &TargetSlot() { return var<uint32_t>(game_addr(game::MAG_182_CARD_TargetSlot)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::MAG_182_CARD_TextureBase)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_182_CARD_PacketCursor)); }
	inline TaskQueue *EffectQueue() { return (TaskQueue *)game_addr(game::MAG_182_CARD_EffectQueue); }
	static constexpr game::Id TRAIL_POOL = game::MAG_182_CARD_TrailPool, TRAIL_END = game::MAG_182_CARD_SparklePool;
	static constexpr game::Id SPARK_POOL = game::MAG_182_CARD_SparklePool, SPARK_END = game::MAG_182_CARD_EffectQueue;
	static constexpr game::Id AFTER = game::MAG_182_CARD_AfterImages, AFTER_END = game::MAG_182_CARD_TextureBase;
	static const uint32_t ARENA_HALF = 0xC000;

	static constexpr game::Id ORIG_RootTask = game::MAG_182_CARD_RootTask;
	static constexpr game::Id ORIG_DirectorA = game::MAG_182_CARD_DirectorShort;
	static constexpr game::Id ORIG_DirectorB = game::MAG_182_CARD_DirectorCard;
	static constexpr game::Id ORIG_Sparkles = game::MAG_182_CARD_Sparkles;
	static constexpr game::Id ORIG_Ring = game::MAG_182_CARD_Ring;
	static constexpr game::Id ORIG_Shard = game::MAG_182_CARD_Shard;
	static constexpr game::Id ORIG_Card = game::MAG_182_CARD_SmallCard;
	static constexpr game::Id ORIG_CardModel = game::MAG_182_CARD_CardModel;
	static constexpr game::Id ORIG_TargetFade = game::MAG_182_CARD_TargetFade;
	static constexpr game::Id SEQ_Sparkle = game::MAG_182_CARD_SparkleSequence, SEQ_Card = game::MAG_182_CARD_SmallCardSequence;
	static constexpr game::Id MODEL_Ring = game::MAG_182_CARD_RingModel, MODEL_Card = game::MAG_182_CARD_CardModel3D;
	static constexpr game::Id SHARD_MODELS = game::MAG_182_CARD_ShardModels;
	static constexpr game::Id SPARK_COLOURS = game::MAG_182_CARD_SparkleColours;
	static const void *SOUND_CardA() { return (const void *)game_addr(game::MAG_182_CARD_SoundShort); }
	static const void *SOUND_CardB() { return (const void *)game_addr(game::MAG_182_CARD_SoundCard); }

	inline void GteSetFarColor(uint32_t r, uint32_t g, uint32_t b) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_SetFarColor))(r, g, b); }
	inline void GteLoadRGBC(const void *c) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadRGBC))(c); }
	inline void GteDPCS() { fn<void (__cdecl *)()>(game_addr(game::GTE_DPCS))(); }
	inline void GteStoreRGB2(void *c) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreRGB2))(c); }

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
		int16_t c;
		int16_t e;
		int16_t x, y, z;
		int16_t w16;
		int16_t w18;
		int16_t w1A;
		int16_t w1C;
		int16_t w1E;
		int16_t w20;
		int16_t w22;
	};
	struct Particle
	{
		uint32_t tag;
		int16_t frame;
		int16_t scale;
		int16_t pos[4];
		int16_t w10;
		int16_t vy;
		uint32_t w14;
	};
	struct AfterImage
	{
		uint32_t used;
		Mat4x3 m;
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x10 && sizeof(Node) == 0x24, "Card nodes");
	static_assert(sizeof(Particle) == 0x18 && sizeof(AfterImage) == 0x24, "Card pools");

	inline Particle *TrailPool() { return (Particle *)game_addr(TRAIL_POOL); }
	inline Particle *SparkPool() { return (Particle *)game_addr(SPARK_POOL); }
	inline AfterImage *After() { return (AfterImage *)game_addr(AFTER); }
	inline uint8_t *Target() { return Entity((int)TargetSlot()); }
	inline uint32_t &Cursor() { return PacketCursor(); }

	static uint32_t EndAt(int16_t c, int16_t n) { return c >= n ? TASK_END : 0; }

	static void ModelMatrix(int16_t angle, int32_t tx, int32_t ty, int32_t tz, int32_t sx, int32_t sy, int32_t sz, Mat4x3 *world)
	{
		int16_t ang[4];
		ang[0] = 0;
		ang[1] = angle;
		ang[2] = 0;
		Mat4x3 m;
		ComposeZYXRotationMatrix(ang, &m);
		m.t[0] = tx;
		m.t[1] = ty;
		m.t[2] = tz;
		int32_t sv[3];
		sv[0] = sx;
		sv[1] = sy;
		sv[2] = sz;
		Scale3DMatrix(&m, sv);
		if (world) *world = m;
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
	}

	static uint8_t *SparkHeader()
	{
		uint8_t *h = AllocHeader(0xB4);
		*(uint32_t *)h = game_addr(SEQ_Sparkle);
		*(int16_t *)(h + 0x24) = 4;
		return h;
	}
	static void SparkDraw(uint8_t *h, const Particle *p)
	{
		*(int16_t *)(h + 4) = p->frame;
		*(uint32_t *)(h + 0x1C) = var<uint32_t>(game_addr(SPARK_COLOURS) + (uint32_t)((int32_t)p->frame % 8) * 4);
		TransformCameraByShadowRotation(p->pos, p->scale, -((int32_t)p->scale >> 4));
		Cursor() = InitEffectSequenceFromData(h, RenderOT(), 2, Cursor());
	}

	static int32_t RingFade(int32_t c) { return c < 8 ? shl32(0x18 - c, 8) : c >= 0x1A ? shl32(c - 0x12, 8) : 0x800; }
	static void RingDraw(const Node *n, int16_t angle, int16_t scale, int32_t fade)
	{
		ModelMatrix(angle, n->x, 0, n->z, scale, scale, scale, nullptr);
		uint8_t *h = AllocHeader(0x58);
		*(uint32_t *)h = game_addr(MODEL_Ring);
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0x1C) = 0xF3;
		*(int32_t *)(h + 0xC) = fade;
		Cursor() = RenderPrimModel(h, RenderOT(), 2, Cursor());
		FieldFree(0x58);
	}

	static void ShardDraw(const Node *n, int16_t angle, int16_t scale, int16_t yscale)
	{
		ModelMatrix(angle, n->x, 0, n->z, scale, yscale, scale, nullptr);
		uint8_t *h = AllocHeader(0x58);
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0x1C) = 0x33;
		*(uint32_t *)h = var<uint32_t>(game_addr(SHARD_MODELS) + (uint32_t)n->e * 4);
		Cursor() = RenderPrimModel(h, RenderOT(), 2, Cursor());
		FieldFree(0x58);
	}

	static uint8_t *CardHeader()
	{
		uint8_t *h = AllocHeader(0xB4);
		*(uint32_t *)h = game_addr(SEQ_Card);
		*(int16_t *)(h + 0x24) = 8;
		return h;
	}
	static void CardSpriteDraw(uint8_t *h, int16_t frame, const int16_t *pos, int16_t scale)
	{
		*(int16_t *)(h + 4) = frame;
		TransformCameraByShadowRotation(pos, scale, -((int32_t)scale >> 3));
		Cursor() = InitEffectSequenceFromData(h, RenderOT(), 2, Cursor());
	}
	static void TrailDraw(uint8_t *h, const Particle *p)
	{
		*(int16_t *)(h + 4) = p->frame;
		TransformCameraByShadowRotation(p->pos, p->scale, -((int32_t)p->scale >> 4));
		Cursor() = InitEffectSequenceFromData(h, RenderOT(), 2, Cursor());
	}

	static int32_t AfterCount(int32_t c) { return c >= 0x1E ? 0x23 - c : 5; }
	static int32_t AfterFirst(int32_t c) { return c - 1 < 0 ? 0 : (c - 1) % 5; }
	static void CardModelDraw(int16_t c, int16_t angle, int16_t x, int16_t y, int16_t z, int16_t scale, AfterImage *ring, int32_t main_fade)
	{
		AfterImage *slot = &ring[(int32_t)c % 5];
		slot->used = 1;
		ModelMatrix(angle, x, y, z, scale, scale, scale, &slot->m);
		uint8_t *h = AllocHeader(0x58);
		*(uint32_t *)h = game_addr(MODEL_Card);
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0x1C) = 0;
		if (c < 0x1E)
		{
			if (c >= 0x16)
			{
				*(uint32_t *)(h + 0x1C) = 0xC3;
				*(int32_t *)(h + 0xC) = main_fade;
			}
			Cursor() = RenderPrimModel(h, RenderOT(), 2, Cursor());
		}
		*(uint32_t *)(h + 0x1C) |= 0xC3;
		*(int32_t *)(h + 0xC) = 0xC00;
		int32_t k = AfterFirst(c);
		const int32_t count = AfterCount(c);
		int32_t step = 0;
		for (int32_t i = 0; i < count; i++, step += 0x20)
		{
			if (ring[k].used == 0) break;
			Mat4x3 m;
			ComposeAffineTransform(&Camera(), &ring[k].m, &m);
			GteSetRotMatrix(&m);
			GteSetTransVector(&m);
			*(int32_t *)(h + 0xC) += step;
			Cursor() = RenderPrimModel(h, RenderOT(), 2, Cursor());
			if (--k < 0) k = 4;
		}
		FieldFree(0x58);
	}
	static int32_t CardModelFade(int32_t c) { return shl32(c - 0x16, 9); }

	static void TargetFadeColours(uint8_t *e, int32_t ir0)
	{
		uint32_t colour = 0x32000000;
		e[7] = (uint8_t)((uint32_t)(shl32(ir0, 8) - ir0) >> 12);
		const uint32_t far0 = var<uint32_t>(game_addr(game::CURRENT_BS_SHADOW_PALETTE));
		GteSetFarColor(far0 & 0xFF, (far0 >> 8) & 0xFF, var<uint8_t>(game_addr(game::unk_B8B9AA)));
		GteLoadRGBC(&colour);
		GteSetIR0(ir0);
		GteDPCS();
		GteStoreRGB2(e + 0x2C);
		const uint32_t far1 = var<uint32_t>(game_addr(game::CURRENT_BS_MODEL_PALETTE));
		GteSetFarColor(far1 & 0xFF, (far1 >> 8) & 0xFF, var<uint8_t>(game_addr(game::unk_B8B7DA)));
		uint32_t black = 0;
		GteLoadRGBC(&black);
		GteDPCS();
		GteStoreRGB2(e + 0x28);
		e[0x2B] = 2;
	}
}
}

namespace ff8fx
{
namespace card182
{
	static uint32_t __cdecl SparklesTask(TaskNode *tn)
	{
		Node *n = (Node *)tn;
		uint8_t *h = SparkHeader();
		int alive = 0;
		for (Particle *p = SparkPool(); (uint32_t)p < game_addr(SPARK_END); p++)
		{
			if (!(p->tag & 1)) continue;
			SparkDraw(h, p);
			if (DrawOnly()) continue;
			p->frame++;
			if (*(int16_t *)(h + 0x28) < 0) { p->tag = 0; continue; }
			p->pos[1] = (int16_t)(p->pos[1] + p->vy);
			p->vy = (int16_t)(p->vy - (int16_t)(p->vy >> 4));
			alive++;
		}
		FieldFree(0xB4);
		if (DrawOnly()) return 0;
		if (n->c >= 0 && n->c <= 0xE)
		{
			for (int k = 0; k < 3; k++)
			{
				int i = 0;
				Particle *p = SparkPool();
				for (; p->tag != 0; p++, i++)
					if ((uint32_t)(p + 1) >= game_addr(SPARK_END)) { p = nullptr; break; }
				if (!p || i >= 0x64) break;
				p->tag = 1;
				p->frame = 0;
				const int16_t s = n->w1E;
				p->scale = (int16_t)(((CrtRand() % 0x1400 + 0x1700) * n->w1C) >> 12);
				const int32_t rx = CrtRand() % (2 * (int32_t)s);
				p->pos[0] = (int16_t)(rx + n->x - s);
				const int32_t ry = CrtRand() % ((int32_t)n->w1E >> 1);
				p->pos[1] = (int16_t)(n->w16 - ry);
				const int16_t s2 = n->w1E;
				const int32_t rz = CrtRand() % (2 * (int32_t)s2);
				p->pos[2] = (int16_t)(rz + n->z - s2);
				p->vy = (int16_t)(-0x3C - CrtRand() % 0x78);
			}
		}
		n->c++;
		if (n->c >= 4 && alive == 0) return TASK_END;
		return 0;
	}

	static uint32_t __cdecl RingTask(TaskNode *tn)
	{
		Node *n = (Node *)tn;
		RingDraw(n, n->w18, n->w1C, RingFade(n->c));
		if (DrawOnly()) return 0;
		const int16_t ra = n->w1A, rs = n->w1E;
		n->w18 = (int16_t)(n->w18 + ra);
		n->w1C = (int16_t)(n->w1C + rs);
		n->w1E = (int16_t)(rs + rs / 32);
		n->c++;
		return EndAt(n->c, 0x22);
	}

	static uint32_t __cdecl ShardTask(TaskNode *tn)
	{
		Node *n = (Node *)tn;
		if (n->w16 > 0)
		{
			if (DrawOnly()) return 0;
			n->w16--;
			return 0;
		}
		ShardDraw(n, n->w18, n->w1C, n->w20);
		if (DrawOnly()) return 0;
		n->w18 = (int16_t)(n->w18 + n->w1A);
		if (n->c < 0xA) n->w20 = (int16_t)(n->w20 + n->w22);
		else if (n->c >= 0xE) n->w1C = (int16_t)(n->w1C - n->w1E);
		n->c++;
		return EndAt(n->c, 0x18);
	}

	static uint32_t __cdecl CardTask(TaskNode *tn)
	{
		Node *n = (Node *)tn;
		if (n->w18 > 0)
		{
			if (!DrawOnly()) n->w18--;
			return 0;
		}
		uint8_t *h = CardHeader();
		int alive = 0;
		if (n->c < 8) CardSpriteDraw(h, n->c, &n->x, n->w1C);
		for (Particle *p = TrailPool(); (uint32_t)p < game_addr(TRAIL_END); p++)
		{
			if (!(p->tag & (uint32_t)(int32_t)n->e)) continue;
			TrailDraw(h, p);
			if (DrawOnly()) continue;
			p->frame++;
			if (*(int16_t *)(h + 0x28) < 0) p->tag = 0;
			else alive++;
		}
		FieldFree(0xB4);
		if (DrawOnly()) return 0;
		if (n->c >= 0 && (int32_t)n->c <= (int32_t)n->w1A + 5)
		{
			for (int k = 0; k < 3; k++)
			{
				int i = 0;
				Particle *p = TrailPool();
				for (; p->tag != 0; p++, i++)
					if ((uint32_t)(p + 1) >= game_addr(TRAIL_END)) { p = nullptr; break; }
				if (!p || i >= 0x78) break;
				p->tag = (uint32_t)(int32_t)n->e;
				p->frame = 6;
				p->scale = (int16_t)(CrtRand() % 0x280 + 0x400);
				*(uint32_t *)&p->pos[0] = *(const uint32_t *)&n->x;
				*(uint32_t *)&p->pos[2] = *(const uint32_t *)&n->z;
				p->pos[0] = (int16_t)(p->pos[0] + (int16_t)(CrtRand() % 0x104 - 0x82));
				p->pos[1] = (int16_t)(p->pos[1] + (int16_t)(CrtRand() % 0x82 - 0x41));
				p->pos[2] = (int16_t)(p->pos[2] + (int16_t)(CrtRand() % 0x104 - 0x82));
			}
		}
		const int16_t vy = n->w1E;
		n->y = (int16_t)(n->y - vy);
		n->w1E = (int16_t)(vy - vy / 10);
		n->c++;
		if (n->c < 4 || alive) return 0;
		return TASK_END;
	}

	static uint32_t __cdecl CardModelTask(TaskNode *tn)
	{
		Node *n = (Node *)tn;
		CardModelDraw(n->c, n->w18, n->x, n->y, n->z, n->w1C, After(), CardModelFade(n->c));
		if (DrawOnly()) return 0;
		if (n->c >= 0x10) n->y = (int16_t)(n->y - n->w1E);
		const int16_t w = n->w1A;
		n->w18 = (int16_t)(n->w18 + w);
		n->w1A = (int16_t)(w - w / 16);
		n->c++;
		return EndAt(n->c, 0x23);
	}

	static uint32_t __cdecl TargetFadeTask(TaskNode *tn)
	{
		Node *n = (Node *)tn;
		const int32_t ir0 = 0x1000 - ComputeSin(shl32(n->c, 10) / n->w18);
		uint8_t *e = Entity(n->e);
		TargetFadeColours(e, ir0);
		if (DrawOnly()) return 0;
		if (n->c == 0) e[1] |= 8;
		n->c++;
		if (n->c < n->w18) return 0;
		uint16_t &flags = *(uint16_t *)e;
		flags = (uint16_t)(((flags & 0xF7FF) | (uint16_t)n->w1A) | 4);
		return TASK_END;
	}

	static void DirectorStart(Node *t)
	{
		uint16_t &flags = *(uint16_t *)Target();
		const uint16_t f = flags;
		t->e = (int16_t)(f & 8);
		flags = (uint16_t)(f & 0xFFF7);
		GetDefaultEffectPosition(Target(), &t->x);
		t->w16 = *(const int16_t *)(Target() + 0x3C);
		const int16_t size = EntitySize((int)TargetSlot());
		t->w1C = size;
		if (size > 0x1100) t->w1C = 0x1100;
		const int16_t s = t->w1C;
		t->w1E = (int16_t)(mul32(s, 2200) >> 12);
		if (s < 0x600) t->w1C = 0x600;
	}

	static void DirectorSpawn(Node *t)
	{
		Node *s = (Node *)AddTaskToQueue(EffectQueue(), game_addr(ORIG_Sparkles));
		*(uint32_t *)&s->x = *(const uint32_t *)&t->x;
		*(uint32_t *)&s->z = *(const uint32_t *)&t->z;
		s->w1C = t->w1C;
		s->c = 0;
		s->w1E = t->w1E;
		Node *r1 = (Node *)AddTaskToQueue(EffectQueue(), game_addr(ORIG_Ring));
		*(uint32_t *)&r1->x = *(const uint32_t *)&t->x;
		r1->c = 0;
		*(uint32_t *)&r1->z = *(const uint32_t *)&t->z;
		r1->w18 = (int16_t)(CrtRand() % 0x1000);
		const int32_t rr = CrtRand() % 0x28;
		const int32_t sc = (((int32_t)t->w1C * 5) << 9) >> 12;
		r1->w1A = (int16_t)(rr + 0x14);
		r1->w1E = (int16_t)(sc / 16);
		r1->w1C = (int16_t)(sc / 16);
		Node *r2 = (Node *)AddTaskToQueue(EffectQueue(), game_addr(ORIG_Ring));
		*(uint32_t *)&r2->x = *(const uint32_t *)&t->x;
		r2->c = 0;
		*(uint32_t *)&r2->z = *(const uint32_t *)&t->z;
		const int32_t a2 = CrtRand() % 0x1000;
		r2->w1A = (int16_t)-r1->w1A;
		r2->w18 = (int16_t)a2;
		r2->w1C = r1->w1C;
		for (int k = 0; k < 0xC; k++)
		{
			Node *d = (Node *)AddTaskToQueue(EffectQueue(), game_addr(ORIG_Shard));
			*(uint32_t *)&d->x = *(const uint32_t *)&t->x;
			d->c = 0;
			d->e = (int16_t)k;
			*(uint32_t *)&d->z = *(const uint32_t *)&t->z;
			d->w16 = (int16_t)(CrtRand() % 6 + k);
			d->w18 = (int16_t)(CrtRand() % 0x1000);
			int16_t rate = (int16_t)(CrtRand() % 0x3C + 0x19);
			d->w1A = rate;
			if (rate & 1) d->w1A = (int16_t)-rate;
			const int16_t sc2 = (int16_t)(((CrtRand() % 0x400 + 0x900) * (int32_t)t->w1C) >> 12);
			d->w1C = sc2;
			d->w1E = (int16_t)(sc2 / 10);
			const int16_t yr = (int16_t)((CrtRand() % 0x500 + 0xA00) / 10);
			d->w22 = yr;
			d->w20 = yr;
		}
	}

	static void DirectorFlash(int16_t c, int16_t end)
	{
		if (c <= 8) SetScreenFlash(shl32(c, 8), 0);
		else if (c >= end - 8) SetScreenFlash(shl32(end - c, 8), 0);
	}

	static void DirectorSound(const void *sound)
	{
		int16_t pos[4];
		GetDefaultEffectPosition(Target(), pos);
		BdPlaySE3D(sound, 0, pos);
	}

	static void DirectorResult(Node *t)
	{
		*(uint16_t *)Target() |= (uint16_t)t->e;
		ApplyActionResultToTarget(Ctx()->actions[0].targets);
	}

	static uint32_t __cdecl DirectorATask(TaskNode *tn)
	{
		if (DrawOnly()) return 0;
		Node *t = (Node *)tn;
		if (t->c == 0) DirectorStart(t);
		if (t->c == 1) DirectorSpawn(t);
		if (t->c == 0x23) DirectorResult(t);
		DirectorFlash(t->c, 0x28);
		if (t->c == 1) DirectorSound(SOUND_CardA());
		t->c++;
		if (t->c <= 0x28) return 0;
		SetScreenFlash(0, 0);
		return TASK_END;
	}

	static uint32_t __cdecl DirectorBTask(TaskNode *tn)
	{
		if (DrawOnly()) return 0;
		Node *t = (Node *)tn;
		if (t->c == 0) DirectorStart(t);
		if (t->c == 1) DirectorSpawn(t);
		if (t->c == 0x18)
		{
			for (int k = 0; k < 0xC; k++)
			{
				Node *d = (Node *)AddTaskToQueue(EffectQueue(), game_addr(ORIG_Card));
				d->c = 0;
				d->e = (int16_t)(1 << k);
				d->x = (int16_t)(CrtRand() % 0x384 + t->x - 0x1C2);
				d->y = (int16_t)(t->w16 - CrtRand() % 0x1C2 - 0x64);
				d->z = (int16_t)(CrtRand() % 0x384 + t->z - 0x1C2);
				const int16_t delay = (int16_t)(CrtRand() % 8 + k);
				d->w1A = delay;
				d->w18 = delay;
				d->w1C = (int16_t)(CrtRand() % 0x700 + 0xA00);
				d->w1E = (int16_t)(CrtRand() % 0xA0 + 0x3C);
			}
		}
		if (t->c == 0x1A)
		{
			Node *m = (Node *)AddTaskToQueue(EffectQueue(), game_addr(ORIG_CardModel));
			m->c = 0;
			m->x = t->x;
			m->y = (int16_t)(t->w16 - 0x1F4);
			m->z = t->z;
			m->w18 = (int16_t)(CrtRand() % 0x1000);
			m->w1C = 0x800;
			m->w1A = (int16_t)(CrtRand() % 0xC8 + 0x3E8);
			m->w1E = (int16_t)(CrtRand() % 0x23 + 0x41);
			for (AfterImage *a = After(); (uint32_t)a < game_addr(AFTER_END); a++) a->used = 0;
		}
		if (t->c == 0x14)
		{
			Node *f = (Node *)AddTaskToQueue(EffectQueue(), game_addr(ORIG_TargetFade));
			f->c = 0;
			f->e = (int16_t)TargetSlot();
			f->w18 = 6;
			f->w1A = (int16_t)(*(const uint16_t *)Target() & 0x800);
		}
		if (t->c == 0x40) DirectorResult(t);
		DirectorFlash(t->c, 0x41);
		if (t->c == 1) DirectorSound(SOUND_CardB());
		t->c++;
		if (t->c <= 0x41) return 0;
		SetScreenFlash(0, 0);
		return TASK_END;
	}

	static uint32_t __cdecl RootTask(TaskNode *tn)
	{
		RootNode *r = (RootNode *)tn;
		if (r->counter & 1) PacketCursor() = TexBase() + ARENA_HALF;
		else PacketCursor() = TexBase();
		const int a = ExecuteTaskQueue(EffectQueue());
		r->counter++;
		return a ? 0 : TASK_END;
	}
}

	void register_mag182_card()
	{
		register_port(card182::ORIG_RootTask, (void *)card182::RootTask, "C182 RootTask", 182);
		register_port(card182::ORIG_DirectorA, (void *)card182::DirectorATask, "C182 DirectorA", 182);
		register_port(card182::ORIG_DirectorB, (void *)card182::DirectorBTask, "C182 DirectorB", 182);
		register_port(card182::ORIG_Sparkles, (void *)card182::SparklesTask, "C182 Sparkles", 182);
		register_port(card182::ORIG_Ring, (void *)card182::RingTask, "C182 Ring", 182);
		register_port(card182::ORIG_Shard, (void *)card182::ShardTask, "C182 Shard", 182);
		register_port(card182::ORIG_Card, (void *)card182::CardTask, "C182 Card", 182);
		register_port(card182::ORIG_CardModel, (void *)card182::CardModelTask, "C182 CardModel", 182);
		register_port(card182::ORIG_TargetFade, (void *)card182::TargetFadeTask, "C182 TargetFade", 182);
	}
}

