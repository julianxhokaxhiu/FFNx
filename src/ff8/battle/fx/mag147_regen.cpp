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

// Effect 147: Regen

#include "mag_common.h"

namespace ff8fx
{
namespace regen147
{
	using namespace eng;
	using namespace magc;

	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::g_Regen_CastContext)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::g_Regen_PacketCursor)); }
	inline TaskQueue *EffectQueue() { return (TaskQueue *)game_addr(game::g_Regen_EffectQueue); }
	inline uint32_t RenderOT44() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44; }

	static constexpr game::Id ORIG_Root = game::MAG_147_REGEN_RootTask;
	static constexpr game::Id ORIG_Director = game::MAG_147_REGEN_DirectorTask;
	static constexpr game::Id ORIG_Envelope = game::MAG_147_REGEN_EnvelopeTask;
	static constexpr game::Id ORIG_Leaf = game::MAG_147_REGEN_LeafTask;
	static constexpr game::Id MODEL_Envelope = game::Regen_EnvelopeModel;
	static constexpr game::Id SEQ_Leaf = game::Regen_LeafSeq;
	static const void *SOUND_Regen() { return (const void *)game_addr(game::unk_DD05CC); }
	static constexpr game::Id TRAILS = game::g_Regen_LeafTrails;
	static constexpr game::Id ARENA_EVEN = game::unk_2479BB8, ARENA_ODD = game::unk_2485BB8;

	inline int32_t ComputeCos(int32_t angle) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(angle); }
	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t flag, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, flag, out); }

#pragma pack(push, 1)
	struct RootNode
	{
		TaskNode hdr;
		int16_t counter;
		uint16_t pad0E;
	};
	struct DirectorNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t action;
		int16_t pos[4];
		int16_t scale;
		int16_t rise;
		uint8_t pad1C[4];
		int16_t slot;
		uint16_t pad22;
	};
	struct EnvelopeNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t kind;
		int16_t pos[4];
		int16_t angle;
		int16_t spin;
		int16_t sxz;
		int16_t dsxz;
		int16_t sy;
		int16_t dsy;
	};
	struct LeafNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t delay;
		int16_t pos[3];
		int16_t action;
		int16_t vel[3];
		int16_t leaf;
		int16_t size;
		uint16_t pad22;
	};
	struct TrailPart
	{
		int16_t counter;
		int16_t size;
		int16_t pos[3];
		int16_t rise;
		int16_t vel[3];
		int16_t unused;
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x10 && sizeof(DirectorNode) == 0x24 && sizeof(EnvelopeNode) == 0x24 && sizeof(LeafNode) == 0x24, "Regen nodes");
	static_assert(sizeof(TrailPart) == 0x14, "Regen trail particle");

	static inline TrailPart *TrailOf(int32_t action, int32_t leaf)
	{
		return (TrailPart *)(game_addr(TRAILS) + (uint32_t)mul32(leaf + action * 8, 0x140));
	}

	static inline int32_t DivMagic(int32_t x, int32_t magic, int sh)
	{
		int32_t hi = (int32_t)(((int64_t)x * magic) >> 32) >> sh;
		return hi + (int32_t)((uint32_t)hi >> 31);
	}

	static int32_t EnvelopeFade(int16_t counter, int16_t kind)
	{
		const int32_t base = kind ? 0x800 : 0;
		const int32_t step = (kind ? 0x800 : 0x1000) / 16;
		if (counter < 4) return 0x1000 - mul32(step, counter);
		if (counter >= 0xE) return mul32(step, counter - 0xE) + base;
		return base;
	}

	static void EnvelopeDraw(const EnvelopeNode *e, int32_t fade)
	{
		int16_t angles[4] = { 0, e->angle, 0, 0 };
		Mat4x3 m;
		BuildRotationMatrixFromAngles(angles, &m);
		m.t[0] = e->pos[0];
		m.t[1] = e->pos[1];
		m.t[2] = e->pos[2];
		int32_t scale[3] = { e->sxz, e->sy, e->sxz };
		Scale3DMatrix(&m, scale);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		h[0xA] = 0;
		h[9] = 0;
		h[8] = 0;
		*(uint32_t *)h = game_addr(MODEL_Envelope);
		*(int32_t *)(h + 0xC) = fade;
		*(uint32_t *)(h + 0x1C) = 0xF0;
		PacketCursor() = RenderPrimModel(h, RenderOT44(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static void EnvelopeUpdate(EnvelopeNode *e)
	{
		e->sxz = (int16_t)(e->sxz + e->dsxz);
		e->angle = (int16_t)(e->angle + e->spin);
		const int32_t d = e->dsy;
		e->sy = (int16_t)(e->sy + d);
		e->dsy = (int16_t)(DivMagic(d, 0x2AAAAAAB, 1) + d);
		e->counter++;
	}
}
}

namespace ff8fx
{
namespace regen147
{
	static uint32_t __cdecl EnvelopeTask(TaskNode *n)
	{
		EnvelopeNode *e = (EnvelopeNode *)n;
		EnvelopeDraw(e, EnvelopeFade(e->counter, e->kind));
		if (DrawOnly()) return 0;
		EnvelopeUpdate(e);
		return e->counter >= 0x1E ? TASK_END : 0;
	}

	static int32_t LeafShade(int16_t counter) { return (uint8_t)(0x70 - (uint8_t)((uint8_t)counter * 6)); }

	static uint8_t *LeafDraw(const LeafNode *p, int32_t shade)
	{
		TransformCameraByShadowRotation(p->pos, p->size, -(p->size >> 3));
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		*(uint32_t *)h = game_addr(SEQ_Leaf);
		if (p->counter < 0xE)
		{
			*(int16_t *)(h + 4) = p->counter >= 8 ? (int16_t)(p->counter - 8) : 0;
			*(uint16_t *)(h + 0x24) = 0;
			PacketCursor() = InitEffectSequenceFromData(h, RenderOT44(), 2, PacketCursor());
			int16_t angles[4] = { 0x400, 0, 0, 0 };
			Mat4x3 m;
			BuildRotationMatrixFromAngles(angles, &m);
			m.t[0] = p->pos[0];
			const int32_t s = p->size * 2;
			int32_t scale[3] = { s, s, s };
			m.t[1] = 0;
			m.t[2] = p->pos[2];
			Scale3DMatrix(&m, scale);
			ComposeAffineTransform(&Camera(), &m, &m);
			GteSetRotMatrix(&m);
			GteSetTransVector(&m);
			*(int16_t *)(h + 4) = 1;
			h[0x1E] = (uint8_t)shade;
			h[0x1D] = (uint8_t)shade;
			h[0x1C] = (uint8_t)shade;
			*(uint16_t *)(h + 0x24) = 4;
			PacketCursor() = InitEffectSequenceFromData(h, RenderOT44(), 2, PacketCursor());
		}
		FieldFree(0xB4);
		*(uint16_t *)(h + 0x24) = 0;
		*(int16_t *)(h + 4) = 8;
		return h;
	}

	static void TrailDraw(uint8_t *h, const TrailPart *t)
	{
		if (t->counter < 0xA) *(int16_t *)(h + 4) = t->counter;
		TransformCameraByShadowRotation(t->pos, t->size, -(t->size >> 3));
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT44(), 2, PacketCursor());
	}

	static void TrailUpdate(TrailPart *t, int16_t leaf_counter)
	{
		t->counter++;
		if (t->counter >= 0x1A)
		{
			t->counter = -1;
			return;
		}
		if (leaf_counter < 0xE)
		{
			t->size = (int16_t)(t->size - 0x10);
			t->pos[1] = (int16_t)(t->pos[1] + t->rise);
			return;
		}
		t->size = (int16_t)(t->size - (int16_t)(t->size >> 4));
		t->pos[0] = (int16_t)(t->pos[0] + t->vel[0]);
		t->pos[1] = (int16_t)(t->pos[1] + t->vel[1]);
		t->pos[2] = (int16_t)(t->pos[2] + t->vel[2]);
		t->vel[0] = (int16_t)(t->vel[0] + (int16_t)(t->vel[0] >> 6));
		t->vel[1] = (int16_t)(t->vel[1] + (int16_t)(t->vel[1] >> 6));
		t->vel[2] = (int16_t)(t->vel[2] + (int16_t)(t->vel[2] >> 6));
	}

	static void LeafMove(LeafNode *p)
	{
		const int16_t vx = p->vel[0], vy = p->vel[1], vz = p->vel[2];
		p->pos[0] = (int16_t)(p->pos[0] + vx);
		p->pos[1] = (int16_t)(p->pos[1] + vy);
		p->pos[2] = (int16_t)(p->pos[2] + vz);
		p->vel[0] = (int16_t)(vx - (int16_t)(vx >> 2));
		p->vel[1] = (int16_t)(vy + (int16_t)(vy >> 3));
		p->vel[2] = (int16_t)(vz - (int16_t)(vz >> 2));
	}

	static uint32_t __cdecl LeafTask(TaskNode *n)
	{
		LeafNode *p = (LeafNode *)n;
		if (p->delay > 0)
		{
			if (DrawOnly()) return 0;
			p->delay--;
			return 0;
		}
		uint8_t *h = LeafDraw(p, LeafShade(p->counter));
		TrailPart *trail = TrailOf(p->action, p->leaf);
		for (int k = 0; k < 16; k++)
		{
			TrailPart *t = &trail[k];
			if (t->counter < 0) continue;
			TrailDraw(h, t);
			if (DrawOnly()) continue;
			TrailUpdate(t, p->counter);
		}
		if (DrawOnly()) return 0;
		if (p->counter >= 2 && p->counter <= 0xB)
		{
			int k = 0;
			while (k < 16 && trail[k].counter >= 0) k++;
			if (k < 16)
			{
				TrailPart *t = &trail[k];
				t->counter = 1;
				const int32_t r0 = CrtRand();
				t->size = (int16_t)(r0 % 0x600 + 0xD00);
				*(uint32_t *)&t->pos[0] = *(const uint32_t *)&p->pos[0];
				*(uint32_t *)&t->pos[2] = *(const uint32_t *)&p->pos[2];
				const int32_t r1 = CrtRand();
				t->pos[0] = (int16_t)(t->pos[0] + (r1 % 200 - 100));
				const int32_t r2 = CrtRand();
				t->pos[1] = (int16_t)(t->pos[1] + (r2 % 100 - 50));
				const int32_t r3 = CrtRand();
				t->pos[2] = (int16_t)(t->pos[2] + (r3 % 200 - 100));
				const int32_t r4 = CrtRand();
				t->rise = (int16_t)(-10 - r4 % 30);
				const int32_t r5 = CrtRand();
				t->vel[0] = (int16_t)(r5 % 40 - 20);
				const int32_t r6 = CrtRand();
				t->vel[1] = (int16_t)(r6 % 40 - 20);
				const int32_t r7 = CrtRand();
				t->vel[2] = (int16_t)(r7 % 40 - 20);
				const int32_t r8 = CrtRand();
				t->unused = (int16_t)(r8 % 12 + 12);
			}
		}
		LeafMove(p);
		p->counter++;
		return p->counter >= 0x28 ? TASK_END : 0;
	}

	static uint32_t __cdecl DirectorTask(TaskNode *n)
	{
		if (DrawOnly()) return 0;
		DirectorNode *d = (DirectorNode *)n;
		int16_t local[4];
		if (d->counter == 0)
		{
			GetDefaultEffectPosition(Entity(d->slot), d->pos);
			d->pos[3] = EntityHeight(d->slot);
			int32_t s = (mul32(EntitySize(d->slot), 5600) + 0x1388) >> 12;
			if (s > 0x898) s = 0x898;
			d->scale = (int16_t)DivMagic(shl32(s, 12), 0x10624DD3, 6);
			GetEffectSpawnPosition(Entity(d->slot), 0xF0, 0x1000, local);
			d->rise = (int16_t)DivMagic(local[1] - 0x384, 0x2AAAAAAB, 2);
		}
		if (d->counter == 1)
		{
			for (int kind = 0; kind < 2; kind++)
			{
				EnvelopeNode *e = (EnvelopeNode *)AddTaskToQueue(EffectQueue(), game_addr(ORIG_Envelope));
				*(uint32_t *)&e->pos[0] = *(const uint32_t *)&d->pos[0];
				*(uint32_t *)&e->pos[2] = *(const uint32_t *)&d->pos[2];
				e->counter = 0;
				e->kind = (int16_t)kind;
				e->pos[1] = d->pos[3];
				const int32_t r0 = CrtRand();
				e->angle = (int16_t)(r0 % 4096);
				const int32_t r1 = CrtRand();
				e->sxz = 0x1800;
				e->dsxz = kind ? (int16_t)-0xDB : (int16_t)-0x100;
				e->sy = 0;
				e->dsy = kind ? 0x57 : 0x66;
				e->spin = (int16_t)(r1 % 60 + 0x1E);
			}
		}
		if (d->counter == 2)
		{
			int32_t radius = (mul32(EntitySize(d->slot), 4000) >> 12) + 0xC8;
			if (radius > 0x3E8) radius = 0x3E8;
			const int32_t r0 = CrtRand();
			int32_t angle = r0 % 4096;
			const int32_t half = radius / 2;
			for (int leaf = 0; leaf < 8; leaf++)
			{
				LeafNode *p = (LeafNode *)AddTaskToQueue(EffectQueue(), game_addr(ORIG_Leaf));
				angle += 0x200;
				p->counter = 0;
				p->pos[0] = d->pos[0];
				p->pos[1] = d->pos[3];
				p->pos[2] = d->pos[2];
				p->vel[0] = (int16_t)(mul32(ComputeCos(angle), half) >> 12);
				const int32_t r1 = CrtRand();
				p->vel[1] = (int16_t)(-0x1E - r1 % 0x46);
				p->vel[2] = (int16_t)(mul32(ComputeSin(angle), half) >> 12);
				const int32_t r2 = CrtRand();
				p->leaf = (int16_t)leaf;
				p->size = (int16_t)(r2 % 1024 + 0x1000);
				p->action = d->action;
				TrailPart *trail = TrailOf(d->action, leaf);
				for (int k = 0; k < 16; k++) trail[k].counter = -1;
			}
		}
		if (d->counter == 0x1C)
		{
			ApplyActionResultToTarget(Ctx()->actions[d->action].targets);
			const int32_t next = d->action + 1;
			if (next <= Ctx()->actions[0].last_action)
			{
				DirectorNode *nd = (DirectorNode *)AddTaskToQueue(EffectQueue(), game_addr(ORIG_Director));
				nd->counter = 0;
				nd->action = (int16_t)next;
				nd->slot = Ctx()->actions[next].targets[0];
			}
		}
		if (d->counter == 1)
		{
			GetDefaultEffectPosition(Entity(d->slot), local);
			BdPlaySE3D(SOUND_Regen(), 0, local);
		}
		d->counter++;
		return d->counter > 0x30 ? TASK_END : 0;
	}

	static uint32_t __cdecl RootTask(TaskNode *n)
	{
		RootNode *r = (RootNode *)n;
		PacketCursor() = game_addr(ARENA_ODD);
		if (!(*(const uint8_t *)&r->counter & 1)) PacketCursor() = game_addr(ARENA_EVEN);
		const int alive = ExecuteTaskQueue(EffectQueue());
		r->counter++;
		return alive ? 0 : TASK_END;
	}
}

	void register_mag147_regen()
	{
		register_port(regen147::ORIG_Root, (void *)regen147::RootTask, "R147 Root", 147);
		register_port(regen147::ORIG_Director, (void *)regen147::DirectorTask, "R147 Director", 147);
		register_port(regen147::ORIG_Envelope, (void *)regen147::EnvelopeTask, "R147 Envelope", 147);
		register_port(regen147::ORIG_Leaf, (void *)regen147::LeafTask, "R147 Leaf", 147);
	}
}

