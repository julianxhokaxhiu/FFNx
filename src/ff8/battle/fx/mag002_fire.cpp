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

// Effect 2: Fire

#include "mag_common.h"

namespace ff8fx
{
namespace fire002
{
	using namespace eng;
	using namespace magc;

	inline TaskQueuePair &Queues() { return var<TaskQueuePair>(game_addr(game::MAG002_FIRE_queues)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::MAG002_FIRE_castCtx)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG002_FIRE_pktCursor)); }
	inline uint32_t *TargetFree() { return (uint32_t *)game_addr(game::MAG002_FIRE_slotFree); }
	static constexpr game::Id ARENA_EVEN = game::MAG002_FIRE_vramEven, ARENA_ODD = game::MAG002_FIRE_vramOdd;

	static constexpr game::Id ORIG_RootTask = game::MAG_002_FIRE_RootTask;
	static constexpr game::Id ORIG_DirectorMulti = game::MAG_002_FIRE_Director_MultiTarget;
	static constexpr game::Id ORIG_DirectorSingle = game::MAG_002_FIRE_Director_SingleTarget;
	static constexpr game::Id ORIG_FlameTask = game::MAG_002_FIRE_Flame_Tick;
	static constexpr game::Id ORIG_SparkTask = game::MAG_002_FIRE_Spark_Tick;
	static constexpr game::Id ORIG_EmberTask = game::MAG_002_FIRE_Ember_Tick;
	static const void *SEQ_Flame() { return (const void *)game_addr(game::MAG002_FIRE_flameSprite); }
	static const void *SEQ_FlameGlow() { return (const void *)game_addr(game::MAG002_FIRE_groundGlowSprite); }
	static const void *SEQ_Ember() { return (const void *)game_addr(game::MAG002_FIRE_emberSprite); }
	static const void *SOUND_Fire() { return (const void *)game_addr(game::MAG002_FIRE_soundId); }

	inline uint32_t RenderOT44() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44; }

	struct RootNode { TaskNode hdr; uint16_t counter; };
	struct DirectorNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t action;
		uint8_t pad10[0x10];
		int16_t slot;
		int16_t sub;
	};
	struct FlameNode
	{
		TaskNode hdr;
		int16_t frame;
		int16_t delay;
		int16_t pos[3];
		int16_t ground;
		uint8_t pad18[4];
		int16_t scale;
		uint8_t pad1E[6];
	};
	struct SparkNode
	{
		TaskNode hdr;
		int16_t life;
		int16_t delay;
		int16_t pos[3];
		int16_t pad16;
		int16_t vel[3];
		int16_t pad1E;
		int16_t scale;
		int16_t pad22;
	};
	struct EmberNode
	{
		TaskNode hdr;
		int16_t frame;
		int16_t delay;
		int16_t pos[3];
		int16_t vel_y;
		uint8_t pad18[4];
		int16_t scale;
		uint8_t pad1E[6];
	};
	static_assert(sizeof(DirectorNode) == 0x24 && sizeof(FlameNode) == 0x24 && sizeof(SparkNode) == 0x24 && sizeof(EmberNode) == 0x24, "Fire nodes are 0x24 bytes");
}
}

namespace ff8fx
{
namespace fire002
{
	static uint32_t __cdecl RootTask(TaskNode *n)
	{
		RootNode *r = (RootNode *)n;
		PacketCursor() = game_addr(ARENA_ODD);
		if (!(r->counter & 1)) PacketCursor() = game_addr(ARENA_EVEN);
		int alive = ExecuteTaskQueue(&Queues().second);
		r->counter++;
		return alive ? 0 : TASK_END;
	}

	static void SpawnNextDirector(DirectorNode *d)
	{
		ActionData *acts = Ctx()->actions;
		int next = d->action + 1;
		if (next > acts[0].last_action) return;
		DirectorNode *t;
		if (acts[next].target_count > 1)
		{
			t = (DirectorNode *)AddTaskToQueue(&Queues().second, game_addr(ORIG_DirectorMulti));
			t->counter = 0;
			t->action = (int16_t)next;
		}
		else
		{
			t = (DirectorNode *)AddTaskToQueue(&Queues().second, game_addr(ORIG_DirectorSingle));
			t->action = (int16_t)next;
			t->counter = 0;
			t->slot = (int16_t)(uint16_t)Ctx()->actions[next].targets[0];
		}
		t->sub = 0;
	}

	static int32_t SpreadOf(int slot)
	{
		int32_t s = (EntitySize(slot) * 2800) >> 12;
		return s > 1000 ? 1000 : s;
	}

	static uint32_t __cdecl DirectorMulti(TaskNode *n)
	{
		DirectorNode *d = (DirectorNode *)n;
		if (DrawOnly()) return 0;
		if (d->sub == 12) SpawnNextDirector(d);
		d->sub++;
		if (d->counter == 1)
		{
			ActionData *ad = &Ctx()->actions[d->action];
			if (ad->target_count > 0)
			{
				int off = 0;
				for (int k = 0;;)
				{
					int slot = ad->targets[off];
					int32_t spread = SpreadOf(slot);
					int16_t pos[4];
					GetDefaultEffectPosition(Entity(slot), pos);
					int32_t half = spread >> 1;
					FlameNode *f = (FlameNode *)AddTaskToQueue(&Queues().second, game_addr(ORIG_FlameTask));
					f->frame = 0;
					f->delay = (int16_t)(CrtRand() % 6);
					ff8fx::copy<(8)>(f->pos, pos);
					f->pos[0] += (int16_t)(CrtRand() % spread - half);
					f->pos[1] += (int16_t)(CrtRand() % spread - half);
					int32_t rz = CrtRand() % spread;
					int16_t h = EntityHeight(slot);
					f->ground = h;
					f->pos[2] += (int16_t)(rz - half);
					if (f->pos[1] > h - 0x226)
						f->pos[1] += (int16_t)(-500 - CrtRand() % 300);
					f->scale = (int16_t)(CrtRand() % 0x600 + 0xD00);
					off += TARGET_STRIDE;
					k++;
					ad = &Ctx()->actions[d->action];
					if (k >= ad->target_count) break;
				}
			}
		}
		if (d->counter == 8)
		{
			ActionData *ad = &Ctx()->actions[d->action];
			ApplyActionResultToTargets(ad->targets, ad->target_count);
		}
		if (d->counter == 6) BdPlaySE(SOUND_Fire(), 0x100, 0x80);
		d->counter++;
		return d->counter >= 0x16 ? TASK_END : 0;
	}

	static uint32_t __cdecl DirectorSingle(TaskNode *n)
	{
		DirectorNode *d = (DirectorNode *)n;
		if (DrawOnly()) return 0;
		if (d->sub == 12) SpawnNextDirector(d);
		int slot = d->slot;
		d->sub++;
		if (TargetFree()[slot] == 0 && d->counter == 0) return 0;
		if (d->counter == 0) TargetFree()[slot] = 0;
		if (d->counter == 1)
		{
			int32_t spread = SpreadOf(slot);
			int16_t pos[4];
			GetDefaultEffectPosition(Entity(slot), pos);
			int32_t half = spread >> 1;
			for (int k = 0; k < 4; k++)
			{
				FlameNode *f = (FlameNode *)AddTaskToQueue(&Queues().second, game_addr(ORIG_FlameTask));
				f->frame = 0;
				f->delay = (int16_t)(CrtRand() % 6 + 2 * k);
				ff8fx::copy<(8)>(f->pos, pos);
				f->pos[0] += (int16_t)(CrtRand() % spread - half);
				f->pos[1] += (int16_t)(CrtRand() % spread - half);
				f->pos[2] += (int16_t)(CrtRand() % spread - half);
				int16_t h = EntityHeight(d->slot);
				f->ground = h;
				if (f->pos[1] > h - 0x226)
					f->pos[1] += (int16_t)(-500 - CrtRand() % 300);
				f->scale = (int16_t)(CrtRand() % 0x600 + 0xD00);
			}
		}
		if (d->counter == 8) ApplyActionResultToTarget(Ctx()->actions[d->action].targets);
		if (d->counter == 6)
		{
			int16_t pos[4];
			GetDefaultEffectPosition(Entity(d->slot), pos);
			BdPlaySE3D(SOUND_Fire(), 0x100, pos);
		}
		d->counter++;
		if (d->counter < 0x16) return 0;
		TargetFree()[d->slot] = 1;
		return TASK_END;
	}

	static void FlameDraw(const FlameNode *p)
	{
		TransformCameraByShadowRotation(p->pos, p->scale, -(p->scale >> 4));
		uint8_t *h = AllocHeader(0xB4);
		*(const void **)h = SEQ_Flame();
		*(uint16_t *)(h + 4) = (uint16_t)p->frame;
		*(uint16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT44(), 2, PacketCursor());
		int16_t angles[4] = { 0x400, 0, 0, 0 };
		Mat4x3 m = {};
		BuildRotationMatrixFromAngles(angles, &m);
		m.t[0] = p->pos[0];
		m.t[1] = p->ground;
		m.t[2] = p->pos[2];
		int32_t sv[4] = { p->scale, p->scale, 0x1000, 0 };
		Scale3DMatrix(&m, sv);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		int32_t y = p->pos[1];
		if (y < -2000) y = -2000;
		int32_t q = (int32_t)(((int64_t)shl32(y, 12) * 0x10624DD3) >> 32) >> 7;
		q += (int32_t)((uint32_t)q >> 31);
		uint8_t level = (uint8_t)((q * 160) / 4096 + 0xA0);
		*(const void **)h = SEQ_FlameGlow();
		*(uint16_t *)(h + 0x24) = 0xC;
		*(uint16_t *)(h + 4) = (uint16_t)p->frame;
		h[0x1E] = level;
		h[0x1D] = level;
		h[0x1C] = level;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT44(), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static uint32_t __cdecl FlameTask(TaskNode *n)
	{
		FlameNode *p = (FlameNode *)n;
		if (p->delay > 0)
		{
			if (DrawOnly()) return 0;
			p->delay--;
			return 0;
		}
		FlameDraw(p);
		if (DrawOnly()) return 0;
		if (p->frame == 0)
		{
			for (int k = 0; k < 4; k++)
			{
				EmberNode *e = (EmberNode *)AddTaskToQueue(&Queues().second, game_addr(ORIG_EmberTask));
				e->frame = 0;
				e->delay = (int16_t)(CrtRand() % 6 + k);
				ff8fx::copy<(8)>(e->pos, p->pos);
				e->pos[0] += (int16_t)(CrtRand() % 300 - 150);
				e->pos[1] -= (int16_t)(CrtRand() % 100);
				e->pos[2] += (int16_t)(CrtRand() % 300 - 150);
				e->vel_y = (int16_t)(-20 - CrtRand() % 30);
				e->scale = (int16_t)(CrtRand() % 0x500 + 0xA00);
			}
			for (int k = 0; k < 10; k++)
			{
				SparkNode *s = (SparkNode *)AddTaskToQueue(&Queues().second, game_addr(ORIG_SparkTask));
				s->life = (int16_t)(CrtRand() % 4 + 5);
				s->delay = (int16_t)(CrtRand() % 4 + k);
				ff8fx::copy<(8)>(s->pos, p->pos);
				s->pos[0] += (int16_t)(CrtRand() % 400 - 200);
				s->pos[1] -= (int16_t)(CrtRand() % 200);
				s->pos[2] += (int16_t)(CrtRand() % 400 - 200);
				s->vel[0] = (int16_t)(CrtRand() % 80 - 40);
				s->vel[1] = (int16_t)(-20 - CrtRand() % 50);
				s->vel[2] = (int16_t)(CrtRand() % 80 - 40);
				s->scale = (int16_t)(CrtRand() % 0x600 + 0x900);
			}
		}
		p->frame++;
		return p->frame >= 0x10 ? TASK_END : 0;
	}

	static void SparkDraw(const SparkNode *p)
	{
		TransformCameraByShadowRotation(p->pos, p->scale, -(p->scale >> 3));
		uint8_t *h = AllocHeader(0xB4);
		*(void **)h = EffectSpriteSharedSequence(5);
		*(uint16_t *)(h + 4) = (uint16_t)(9 - p->life);
		h[0x20] = 4;
		*(uint16_t *)(h + 0x24) = 0x10;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT44(), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static void SparkUpdate(SparkNode *p)
	{
		int16_t v = p->vel[0];
		p->scale = (int16_t)(p->scale - 0x40);
		p->pos[0] = (int16_t)(p->pos[0] + v);
		p->vel[0] = (int16_t)(v - (int16_t)(v >> 4));
		v = p->vel[1];
		p->pos[1] = (int16_t)(p->pos[1] + v);
		p->vel[1] = (int16_t)(v + (int16_t)(v >> 4));
		v = p->vel[2];
		p->pos[2] = (int16_t)(p->pos[2] + v);
		p->vel[2] = (int16_t)(v - (int16_t)(v >> 4));
		p->life--;
	}

	static uint32_t __cdecl SparkTask(TaskNode *n)
	{
		SparkNode *p = (SparkNode *)n;
		if (p->delay > 0)
		{
			if (DrawOnly()) return 0;
			p->delay--;
			return 0;
		}
		SparkDraw(p);
		if (DrawOnly()) return 0;
		SparkUpdate(p);
		return p->life < 0 ? TASK_END : 0;
	}

	static void EmberDraw(const EmberNode *p)
	{
		TransformCameraByShadowRotation(p->pos, p->scale, -(p->scale >> 3));
		uint8_t *h = AllocHeader(0xB4);
		*(uint16_t *)(h + 4) = (uint16_t)p->frame;
		*(const void **)h = SEQ_Ember();
		*(uint16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT44(), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static void EmberUpdate(EmberNode *p)
	{
		int16_t v = p->vel_y;
		p->pos[1] = (int16_t)(p->pos[1] + v);
		p->vel_y = (int16_t)(v + (int16_t)(v >> 5));
		int16_t s = p->scale;
		p->scale = (int16_t)(s - (int16_t)(s >> 6));
		p->frame++;
	}

	static uint32_t __cdecl EmberTask(TaskNode *n)
	{
		EmberNode *p = (EmberNode *)n;
		if (p->delay > 0)
		{
			if (DrawOnly()) return 0;
			p->delay--;
			return 0;
		}
		EmberDraw(p);
		if (DrawOnly()) return 0;
		EmberUpdate(p);
		return p->frame >= 0x10 ? TASK_END : 0;
	}
}

	void register_mag002_fire()
	{
		register_port(fire002::ORIG_RootTask, (void *)fire002::RootTask, "F002 RootTask", 2);
		register_port(fire002::ORIG_DirectorMulti, (void *)fire002::DirectorMulti, "F002 DirectorMulti", 2);
		register_port(fire002::ORIG_DirectorSingle, (void *)fire002::DirectorSingle, "F002 DirectorSingle", 2);
		register_port(fire002::ORIG_FlameTask, (void *)fire002::FlameTask, "F002 FlameTask", 2);
		register_port(fire002::ORIG_SparkTask, (void *)fire002::SparkTask, "F002 SparkTask", 2);
		register_port(fire002::ORIG_EmberTask, (void *)fire002::EmberTask, "F002 EmberTask", 2);
	}
}

