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

// Effect 142: Fira

#include "mag_common.h"

namespace ff8fx
{
namespace fira142
{
	using namespace eng;
	using namespace magc;

	inline TaskQueuePair &Queues() { return var<TaskQueuePair>(game_addr(game::unk_24BFB68)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::MAG142_FIRA_CastCtx)); }
	inline uint8_t *&TexBase() { return var<uint8_t *>(game_addr(game::MAG142_FIRA_TexBase)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG142_FIRA_PacketCursor)); }

	static constexpr game::Id ORIG_RootTask = game::MAG_142_FIRA_RootTask;
	static constexpr game::Id ORIG_WaitTexRestore = game::MAG_142_FIRA_WaitTIMUploadEnd;
	static constexpr game::Id ORIG_Director = game::MAG_142_FIRA_Director_Tick;
	static constexpr game::Id ORIG_SparkTask = game::MAG_142_FIRA_Spark_Tick;
	static constexpr game::Id ORIG_EmberTask = game::MAG_142_FIRA_Ember_Tick;
	static constexpr game::Id ORIG_ShockRingTask = game::MAG_142_FIRA_ShockRing_Tick;
	static constexpr game::Id ORIG_FlameColumnTask = game::MAG_142_FIRA_FlameColumn_Tick;
	static constexpr game::Id ORIG_BurstTask = game::MAG_142_FIRA_ExplosionBurst_Tick;
	static constexpr game::Id ORIG_ProjectileTask = game::MAG_142_FIRA_Projectile_Tick;
	static constexpr game::Id ORIG_TrailTask = game::MAG_142_FIRA_Trail_Tick;
	static const void *SEQ_Ember() { return (const void *)game_addr(game::unk_DDE100); }
	static const void *SEQ_ShockRing() { return (const void *)game_addr(game::unk_DDE0D8); }
	static const void *MODEL_FlameColumn() { return (const void *)game_addr(game::unk_DDEF14); }
	static const void *MODEL_Burst() { return (const void *)game_addr(game::unk_DDE6C4); }
	static const void *MODEL_Trail() { return (const void *)game_addr(game::unk_DDE2AC); }
	static const uint32_t *TRAIL_TABLE() { return (const uint32_t *)game_addr(game::unk_DDF454); }
	static const void *SOUND_Fira() { return (const void *)game_addr(game::unk_DDF44C); }

	inline uint32_t RenderOT44() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44; }

	struct RootNode { TaskNode hdr; uint16_t counter; int16_t done; };
	struct DirectorNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t action;
		int16_t target[4];
		int16_t caster[4];
		int16_t slot;
		int16_t caster_slot;
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
	struct ModelNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t delay;
		int16_t pos[3];
		int16_t pad16;
		int16_t yaw;
		int16_t yaw_vel;
		int16_t scale;
		int16_t scale_vel;
		int16_t scale_y;
		int16_t scale_y_vel;
	};
	struct ProjectileNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		int16_t pos[4];
		int16_t vel[3];
		uint8_t pad1E[6];
	};
	static_assert(sizeof(DirectorNode) == 0x24 && sizeof(SparkNode) == 0x24 && sizeof(EmberNode) == 0x24 && sizeof(ModelNode) == 0x24 && sizeof(ProjectileNode) == 0x24, "Fira nodes are 0x24 bytes");
}
}

namespace ff8fx
{
namespace fira142
{
	static uint32_t __cdecl RootTask(TaskNode *n)
	{
		RootNode *r = (RootNode *)n;
		if (r->counter & 1) PacketCursor() = (uint32_t)(TexBase() + 0x10000);
		else PacketCursor() = (uint32_t)TexBase();
		int alive = ExecuteTaskQueue(&Queues().second);
		if (alive == 0 && !(Ctx()->flags & 2))
		{
			RootNode *w = (RootNode *)AddTaskToQueue(&Queues().first, game_addr(ORIG_WaitTexRestore));
			w->counter = 0;
			w->done = 0;
		}
		r->counter++;
		return alive ? 0 : TASK_END;
	}

	static uint32_t __cdecl WaitTexRestore(TaskNode *n)
	{
		RootNode *w = (RootNode *)n;
		if (w->counter == 1) TextureRestoreTask(TexBase(), &w->done);
		int16_t done = w->done;
		w->counter++;
		return done ? TASK_END : 0;
	}

	static inline void Swap16(int16_t &a, int16_t &b)
	{
		a ^= b;
		b ^= a;
		a ^= b;
	}

	static int16_t ColumnScaleVel(int16_t r)
	{
		int32_t x = shl32(r, 12);
		int32_t hi = (int32_t)(((int64_t)x * (int32_t)0x859C5061) >> 32);
		hi += x;
		hi >>= 9;
		hi += (int32_t)((uint32_t)hi >> 31);
		return (int16_t)hi;
	}

	static int16_t Div30(int16_t v)
	{
		int32_t x = v;
		int32_t hi = (int32_t)(((int64_t)x * (int32_t)0x88888889) >> 32);
		hi += x;
		hi >>= 4;
		hi += (int32_t)((uint32_t)hi >> 31);
		return (int16_t)hi;
	}

	static uint32_t __cdecl DirectorTask(TaskNode *n)
	{
		DirectorNode *d = (DirectorNode *)n;
		if (DrawOnly()) return 0;
		if (d->counter == 0)
		{
			GetDefaultEffectPosition(Entity(d->slot), d->target);
			GetDefaultEffectPosition(Entity(d->caster_slot), d->caster);
			Swap16(d->target[1], d->target[3]);
			Swap16(d->caster[1], d->caster[3]);
		}
		if (d->counter == 1)
		{
			ProjectileNode *p = (ProjectileNode *)AddTaskToQueue(&Queues().second, game_addr(ORIG_ProjectileTask));
			ff8fx::copy<(8)>(p->pos, d->caster);
			p->counter = 0;
			p->vel[0] = (int16_t)((d->target[0] - d->caster[0]) / 8);
			p->vel[1] = (int16_t)((d->target[1] - d->caster[1]) / 8);
			p->vel[2] = (int16_t)((d->target[2] - d->caster[2]) / 8);
		}
		if (d->counter == 9)
		{
			ModelNode *b = (ModelNode *)AddTaskToQueue(&Queues().second, game_addr(ORIG_BurstTask));
			ff8fx::copy<(4)>(b->pos, d->target);
			b->counter = 0;
			ff8fx::copy<(4)>((uint8_t *)b + 0x14, &d->target[2]);
			b->yaw = (int16_t)(CrtRand() % 4096);
			int32_t r = CrtRand() % 40;
			b->scale = 0x104F;
			b->scale_y = 0;
			b->scale_y_vel = 0x2B7;
			b->yaw_vel = (int16_t)(r + 0x46);
		}
		if (d->counter == 0xB)
		{
			for (int k = 0; k < 3; k++)
			{
				ModelNode *f = (ModelNode *)AddTaskToQueue(&Queues().second, game_addr(ORIG_FlameColumnTask));
				f->counter = 0;
				int32_t r = CrtRand() % 6;
				f->pos[1] = (int16_t)(d->target[1] - (int16_t)(600 * k + 600));
				f->delay = (int16_t)(r + k);
				f->pos[0] = d->target[0];
				f->pos[2] = d->target[2];
				f->pos[1] += (int16_t)(CrtRand() % 300 - 150);
				f->yaw = (int16_t)(CrtRand() % 4096);
				f->yaw_vel = (int16_t)(CrtRand() % 90 + 40);
				int32_t rs = CrtRand() % 600;
				f->scale_y = 0x9C9;
				int16_t sv = ColumnScaleVel((int16_t)(rs + 0x4B0));
				f->scale = sv;
				f->scale_vel = Div30(sv);
			}
		}
		if (d->counter == 9)
		{
			ModelNode *g = (ModelNode *)AddTaskToQueue(&Queues().second, game_addr(ORIG_ShockRingTask));
			ff8fx::copy<(4)>(g->pos, d->target);
			g->counter = 0;
			ff8fx::copy<(4)>((uint8_t *)g + 0x14, &d->target[2]);
			g->yaw = (int16_t)(CrtRand() % 4096);
			g->scale = 0x1C00;
		}
		if (d->counter >= 0xA && d->counter <= 0xB)
		{
			for (int k = 0; k < 10; k++)
			{
				EmberNode *e = (EmberNode *)AddTaskToQueue(&Queues().second, game_addr(ORIG_EmberTask));
				e->frame = 0;
				e->delay = (int16_t)(CrtRand() % 6 + k);
				ff8fx::copy<(8)>(e->pos, d->target);
				e->pos[0] += (int16_t)(CrtRand() % 1800 - 900);
				e->pos[1] += (int16_t)(-400 - CrtRand() % 1100);
				e->pos[2] += (int16_t)(CrtRand() % 1800 - 900);
				e->vel_y = (int16_t)(-40 - CrtRand() % 100);
				e->scale = (int16_t)(CrtRand() % 0x500 + 0xA00);
			}
		}
		if (d->counter >= 9 && d->counter <= 0xB)
		{
			for (int k = 0; k < 12; k++)
			{
				SparkNode *s = (SparkNode *)AddTaskToQueue(&Queues().second, game_addr(ORIG_SparkTask));
				s->life = (int16_t)(CrtRand() % 8 + 10);
				int32_t r = CrtRand() % 6;
				s->pos[1] = d->target[3];
				s->pos[2] = d->target[2];
				s->delay = (int16_t)(r + k);
				s->pos[0] = d->target[0];
				s->pos[0] += (int16_t)(CrtRand() % 1800 - 900);
				s->pos[1] += (int16_t)(-200 - CrtRand() % 1000);
				s->pos[2] += (int16_t)(CrtRand() % 1800 - 900);
				s->vel[0] = (int16_t)(CrtRand() % 200 - 100);
				s->vel[1] = (int16_t)(-40 - CrtRand() % 70);
				s->vel[2] = (int16_t)(CrtRand() % 200 - 100);
				s->scale = (int16_t)(CrtRand() % 0x500 + 0xF00);
			}
		}
		if (d->counter == 0xE) ApplyActionResultToTarget(Ctx()->actions[d->action].targets);
		if (d->counter == 0x15)
		{
			ActionData *acts = Ctx()->actions;
			int next = d->action + 1;
			if (next <= acts[0].last_action)
			{
				DirectorNode *t = (DirectorNode *)AddTaskToQueue(&Queues().second, game_addr(ORIG_Director));
				ActionData *ad = &Ctx()->actions[next];
				t->counter = 0;
				t->action = (int16_t)next;
				t->slot = (int16_t)(uint16_t)ad->targets[0];
				t->caster_slot = (int16_t)(uint16_t)ad->attacker;
			}
		}
		if (d->action == 0 && d->counter <= 6) SetScreenFlash((uint32_t)(341 * d->counter), 0);
		else if ((uint16_t)d->action == (uint16_t)Ctx()->actions[0].last_action && d->counter >= 0x14)
			SetScreenFlash((uint32_t)(0x22A4 - 341 * d->counter), 0);
		if (d->counter == 3)
		{
			int16_t pos[4];
			GetDefaultEffectPosition(Entity(d->slot), pos);
			BdPlaySE3D(SOUND_Fira(), 0x100, pos);
		}
		d->counter++;
		if (d->counter <= 0x1A) return 0;
		if ((uint16_t)d->action == (uint16_t)Ctx()->actions[0].last_action) SetScreenFlash(0, 0);
		return TASK_END;
	}

	static void SparkDraw(const SparkNode *p)
	{
		TransformCameraByShadowRotation(p->pos, p->scale, -(p->scale >> 3));
		uint8_t *h = AllocHeader(0xB4);
		*(void **)h = EffectSpriteSharedSequence(5);
		*(uint16_t *)(h + 4) = (uint16_t)(9 - (int16_t)(p->life >> 1));
		h[0x20] = 4;
		*(uint16_t *)(h + 0x24) = 0x10;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT44(), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static void SparkUpdate(SparkNode *p)
	{
		int16_t v = p->vel[0];
		p->scale = (int16_t)(p->scale - 0x28);
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

	static void ModelMatrix(const int16_t *angles, bool zyx, const int16_t *pos, int32_t sx, int32_t sy, int32_t sz)
	{
		Mat4x3 m = {};
		if (zyx) ComposeZYXRotationMatrix(angles, &m);
		else BuildRotationMatrixFromAngles(angles, &m);
		m.t[0] = pos[0];
		m.t[1] = pos[1];
		m.t[2] = pos[2];
		int32_t sv[4] = { sx, sy, sz, 0 };
		Scale3DMatrix(&m, sv);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
	}

	static void ShockRingMatrix(const ModelNode *p)
	{
		int16_t angles[4] = { 0x400, p->yaw, 0, 0 };
		ModelMatrix(angles, true, p->pos, p->scale, p->scale, p->scale);
	}

	static void ShockRingHeader(uint8_t *h, int16_t c)
	{
		*(const void **)h = SEQ_ShockRing();
		*(uint16_t *)(h + 4) = 0;
		*(uint16_t *)(h + 0x24) = 0;
		uint8_t level;
		if (c < 6) level = (uint8_t)((uint8_t)c * 0x15);
		else if (c >= 0xE) level = (uint8_t)(0x60 - (uint8_t)((uint8_t)c << 4));
		else return;
		h[0x1E] = level;
		h[0x1D] = level;
		h[0x1C] = level;
		*(uint16_t *)(h + 0x24) = 4;
	}

	static uint32_t __cdecl ShockRingTask(TaskNode *n)
	{
		ModelNode *p = (ModelNode *)n;
		ShockRingMatrix(p);
		uint8_t *h = AllocHeader(0xB4);
		ShockRingHeader(h, p->counter);
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT44(), 2, PacketCursor());
		FieldFree(0xB4);
		if (DrawOnly()) return 0;
		p->counter++;
		return p->counter >= 0x16 ? TASK_END : 0;
	}

	static void FlameColumnMatrix(const ModelNode *p)
	{
		int16_t angles[4] = { 0, p->yaw, 0, 0 };
		ModelMatrix(angles, false, p->pos, p->scale, p->scale_y, p->scale);
	}

	static void FlameColumnHeader(uint8_t *h, int16_t c)
	{
		*(const void **)h = MODEL_FlameColumn();
		h[0xA] = 0;
		h[9] = 0;
		h[8] = 0;
		*(uint32_t *)(h + 0x1C) = 0x30;
		if (c < 4)
		{
			*(int32_t *)(h + 0xC) = (4 - c) << 10;
			*(uint32_t *)(h + 0x1C) = 0xF0;
		}
		else if (c >= 8)
		{
			*(int32_t *)(h + 0xC) = (c - 8) * 409;
			*(uint32_t *)(h + 0x1C) = 0xF0;
		}
	}

	static void FlameColumnUpdate(ModelNode *p)
	{
		int16_t v = p->yaw_vel;
		int16_t sv = p->scale_vel;
		p->yaw = (int16_t)(p->yaw + v);
		p->yaw_vel = (int16_t)(v - (int16_t)(v >> 4));
		p->scale = (int16_t)(p->scale + sv);
		int32_t x = sv;
		int32_t q = (int32_t)(((int64_t)x * 0x66666667) >> 32) >> 2;
		q += (int32_t)((uint32_t)q >> 31);
		p->counter++;
		p->scale_vel = (int16_t)(sv - q);
	}

	static uint32_t __cdecl FlameColumnTask(TaskNode *n)
	{
		ModelNode *p = (ModelNode *)n;
		if (p->delay > 0)
		{
			if (DrawOnly()) return 0;
			p->delay--;
			return 0;
		}
		FlameColumnMatrix(p);
		uint8_t *h = AllocHeader(0x58);
		FlameColumnHeader(h, p->counter);
		PacketCursor() = RenderPrimModel(h, RenderOT44(), 2, PacketCursor());
		FieldFree(0x58);
		if (DrawOnly()) return 0;
		FlameColumnUpdate(p);
		return p->counter >= 0x12 ? TASK_END : 0;
	}

	static void BurstMatrix(const ModelNode *p)
	{
		int16_t angles[4] = { 0, p->yaw, 0, 0 };
		ModelMatrix(angles, false, p->pos, p->scale, p->scale_y, p->scale);
	}

	static void BurstHeader(uint8_t *h, int16_t c)
	{
		*(const void **)h = MODEL_Burst();
		*(uint32_t *)(h + 0x1C) = 0x30;
		if (c >= 0xE)
		{
			h[0xA] = 0;
			h[9] = 0;
			h[8] = 0;
			*(int32_t *)(h + 0xC) = (c - 0xE) << 9;
			*(uint32_t *)(h + 0x1C) = 0xF0;
		}
	}

	static void BurstUpdate(ModelNode *p)
	{
		int16_t v = p->yaw_vel;
		p->yaw = (int16_t)(p->yaw + v);
		p->yaw_vel = (int16_t)(v - (int16_t)(v >> 4));
		v = p->scale_y_vel;
		p->scale_y = (int16_t)(p->scale_y + v);
		p->counter++;
		p->scale_y_vel = (int16_t)(v - (int16_t)(v >> 3));
	}

	static uint32_t __cdecl BurstTask(TaskNode *n)
	{
		ModelNode *p = (ModelNode *)n;
		BurstMatrix(p);
		uint8_t *h = AllocHeader(0x58);
		BurstHeader(h, p->counter);
		PacketCursor() = RenderPrimModel(h, RenderOT44(), 2, PacketCursor());
		FieldFree(0x58);
		if (DrawOnly()) return 0;
		BurstUpdate(p);
		return p->counter >= 0x16 ? TASK_END : 0;
	}

	static uint32_t __cdecl ProjectileTask(TaskNode *n)
	{
		ProjectileNode *p = (ProjectileNode *)n;
		if (DrawOnly()) return 0;
		int32_t count = (p->counter >> 1) + 1;
		int32_t spread = count * 400;
		if (count > 0)
		{
			int32_t half = spread >> 1;
			int32_t size_range = count * 80;
			for (int32_t left = count; left; left--)
			{
				ModelNode *t = (ModelNode *)AddTaskToQueue(&Queues().second, game_addr(ORIG_TrailTask));
				ff8fx::copy<(4)>(t->pos, p->pos);
				t->counter = 0;
				ff8fx::copy<(4)>((uint8_t *)t + 0x14, &p->pos[2]);
				t->pos[0] += (int16_t)(CrtRand() % spread - half);
				t->pos[2] += (int16_t)(CrtRand() % spread - half);
				t->yaw = (int16_t)(CrtRand() % 4096);
				t->scale = 0;
				int32_t r1 = CrtRand() % size_range;
				int32_t r2 = CrtRand() % 0x90;
				t->delay = (int16_t)(t->yaw & 1);
				t->scale_vel = (int16_t)(r2 + r1 + 0x50);
			}
		}
		p->pos[0] = (int16_t)(p->pos[0] + p->vel[0]);
		p->pos[1] = (int16_t)(p->pos[1] + p->vel[1]);
		p->pos[2] = (int16_t)(p->pos[2] + p->vel[2]);
		p->counter++;
		return p->counter >= 0xA ? TASK_END : 0;
	}

	static void TrailMatrix(const ModelNode *p)
	{
		int16_t angles[4] = { 0, p->yaw, 0, 0 };
		ModelMatrix(angles, false, p->pos, p->scale, p->scale, p->scale);
	}

	static void TrailHeader(uint8_t *h, int16_t c)
	{
		*(const void **)h = MODEL_Trail();
		*(uint32_t *)(h + 0x1C) = 0x1033;
		*(uint32_t *)(h + 0x18) = TRAIL_TABLE()[c];
		if (c >= 8)
		{
			h[0xA] = 0;
			h[9] = 0;
			h[8] = 0;
			*(int32_t *)(h + 0xC) = (c - 8) << 9;
			*(uint32_t *)(h + 0x1C) = 0x10F3;
		}
	}

	static void TrailUpdate(ModelNode *p)
	{
		int16_t v = p->scale_vel;
		p->scale = (int16_t)(p->scale + v);
		p->counter++;
		p->scale_vel = (int16_t)(v - (int16_t)(v >> 3));
	}

	static uint32_t __cdecl TrailTask(TaskNode *n)
	{
		ModelNode *p = (ModelNode *)n;
		if (p->delay > 0)
		{
			if (DrawOnly()) return 0;
			p->delay--;
			return 0;
		}
		TrailMatrix(p);
		uint8_t *h = AllocHeader(0x58);
		TrailHeader(h, p->counter);
		PacketCursor() = RenderPrimModel(h, RenderOT44(), 2, PacketCursor());
		FieldFree(0x58);
		if (DrawOnly()) return 0;
		TrailUpdate(p);
		return p->counter >= 0x10 ? TASK_END : 0;
	}
}

	void register_mag142_fira()
	{
		register_port(fira142::ORIG_RootTask, (void *)fira142::RootTask, "F142 RootTask", 142);
		register_port(fira142::ORIG_WaitTexRestore, (void *)fira142::WaitTexRestore, "F142 WaitTexRestore", 142);
		register_port(fira142::ORIG_Director, (void *)fira142::DirectorTask, "F142 Director", 142);
		register_port(fira142::ORIG_SparkTask, (void *)fira142::SparkTask, "F142 SparkTask", 142);
		register_port(fira142::ORIG_EmberTask, (void *)fira142::EmberTask, "F142 EmberTask", 142);
		register_port(fira142::ORIG_ShockRingTask, (void *)fira142::ShockRingTask, "F142 ShockRingTask", 142);
		register_port(fira142::ORIG_FlameColumnTask, (void *)fira142::FlameColumnTask, "F142 FlameColumnTask", 142);
		register_port(fira142::ORIG_BurstTask, (void *)fira142::BurstTask, "F142 BurstTask", 142);
		register_port(fira142::ORIG_ProjectileTask, (void *)fira142::ProjectileTask, "F142 ProjectileTask", 142);
		register_port(fira142::ORIG_TrailTask, (void *)fira142::TrailTask, "F142 TrailTask", 142);
	}
}

