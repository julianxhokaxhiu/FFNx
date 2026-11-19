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

// Effect 143: Firaga

#include "mag_common.h"

namespace ff8fx
{
namespace firaga143
{
	using namespace eng;
	using namespace magc;

	inline TaskQueuePair &Queues() { return var<TaskQueuePair>(game_addr(game::MAG143_effectQueues)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::MAG143_castContextPtr)); }
	inline int16_t *Anchor(int action) { return (int16_t *)(game_addr(game::MAG143_targetEffectPos) + 8 * action); }
	inline uint8_t *&TexBase() { return var<uint8_t *>(game_addr(game::MAG143_primBufferBase)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG143_primBufferCursor)); }

	static constexpr game::Id ORIG_RootTask = game::MAG_143_FIRAGA_RootTask_RunEffectQueue;
	static constexpr game::Id ORIG_Director = game::MAG_143_FIRAGA_Director_Tick;
	static constexpr game::Id ORIG_AreaFlameTask = game::MAG_143_FIRAGA_AreaFlame_Tick;
	static constexpr game::Id ORIG_BurstTask = game::MAG_143_FIRAGA_Burst_Tick;
	static constexpr game::Id ORIG_FlameSwirlTask = game::MAG_143_FIRAGA_FlameSwirl_Tick;
	static constexpr game::Id ORIG_FireballTask = game::MAG_143_FIRAGA_TargetFireball_Tick;
	static constexpr game::Id ORIG_SparkTask = game::MAG_143_FIRAGA_SparkParticle_Tick;
	static const void *SEQ_AreaFlame() { return (const void *)game_addr(game::unk_DDA594); }
	static const void *MODEL_Burst() { return (const void *)game_addr(game::unk_DDD208); }
	static const void *MODEL_FlameSwirl() { return (const void *)game_addr(game::MAG143_primModel_FlameSwirl); }
	static const void *MODEL_Fireball() { return (const void *)game_addr(game::MAG143_primModel_TargetFireball); }
	static const void *MODEL_Spark() { return (const void *)game_addr(game::MAG143_primModel_Spark); }
	static const void *SOUND_Firaga() { return (const void *)game_addr(game::MAG143_soundDef); }

	inline uint32_t RenderOT44() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44; }

	struct RootNode { TaskNode hdr; uint16_t counter; };
	struct DirectorNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t action;
		uint8_t pad10[0x10];
		int16_t slot;
		int16_t pad22;
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
	struct ModelNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t action;
		int16_t pos[3];
		int16_t pad16;
		int16_t yaw;
		int16_t yaw_vel;
		int16_t scale;
		int16_t scale_vel;
		uint8_t pad20[4];
	};
	struct SparkNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t action;
		int16_t offset[3];
		int16_t angle;
		int16_t axis[3];
		int16_t spin;
		int16_t pad20;
		int16_t size;
	};
	static_assert(sizeof(DirectorNode) == 0x24 && sizeof(FlameNode) == 0x24 && sizeof(ModelNode) == 0x24 && sizeof(SparkNode) == 0x24, "Firaga nodes are 0x24 bytes");
}
}

namespace ff8fx
{
namespace firaga143
{
	static uint32_t __cdecl RootTask(TaskNode *n)
	{
		RootNode *r = (RootNode *)n;
		if (r->counter & 1) PacketCursor() = (uint32_t)(TexBase() + 0x10000);
		else PacketCursor() = (uint32_t)TexBase();
		int alive = ExecuteTaskQueue(&Queues().second);
		r->counter++;
		return alive ? 0 : TASK_END;
	}

	static void RandomUnitVector(int32_t *v)
	{
		v[0] = CrtRand() % 4096 - 0x800;
		v[1] = CrtRand() % 4096 - 0x800;
		v[2] = CrtRand() % 4096 - 0x800;
		NormalizeVector(v, v);
	}

	static int16_t SparkSize(int16_t r)
	{
		int32_t x = shl32(r, 12);
		int32_t hi = (int32_t)(((int64_t)x * 0x44FC3A35) >> 32) >> 9;
		hi += (int32_t)((uint32_t)hi >> 31);
		return (int16_t)hi;
	}

	static uint32_t __cdecl DirectorTask(TaskNode *n)
	{
		DirectorNode *d = (DirectorNode *)n;
		if (DrawOnly()) return 0;
		if (d->counter == 0)
		{
			GetDefaultEffectPosition(Entity(d->slot), Anchor(d->action));
			Anchor(d->action)[3] = 1;
		}
		if (d->counter >= 1 && d->counter <= 0xE)
		{
			int32_t count = d->counter / 6 + 1;
			for (; count > 0; count--)
			{
				SparkNode *s = (SparkNode *)AddTaskToQueue(&Queues().second, game_addr(ORIG_SparkTask));
				s->counter = 0;
				s->action = d->action;
				int32_t dir[4] = {}, axis[4] = {};
				RandomUnitVector(dir);
				RandomUnitVector(axis);
				CrossProduct(dir, axis, axis);
				s->offset[0] = (int16_t)dir[0];
				s->offset[1] = (int16_t)dir[1];
				s->offset[2] = (int16_t)dir[2];
				s->angle = 0;
				s->axis[0] = (int16_t)axis[0];
				s->axis[1] = (int16_t)axis[1];
				s->axis[2] = (int16_t)axis[2];
				s->spin = (int16_t)(CrtRand() % 30 + 0x14);
				s->size = SparkSize((int16_t)(CrtRand() % 2500 + 0x5DC));
			}
		}
		if (d->counter == 4)
		{
			ModelNode *f = (ModelNode *)AddTaskToQueue(&Queues().second, game_addr(ORIG_FireballTask));
			f->action = d->action;
			f->counter = 0;
			f->scale = 0;
			f->scale_vel = 0x80;
			ff8fx::copy<(8)>(f->pos, Anchor(d->action));
		}
		if (d->counter == 0x20) ScreenFadeTask(0, 1, 2, 0xFF);
		if (d->counter == 0x21)
		{
			Anchor(d->action)[3] = 0;
			ModelNode *s = (ModelNode *)AddTaskToQueue(&Queues().second, game_addr(ORIG_FlameSwirlTask));
			s->counter = 0;
			ff8fx::copy<(8)>(s->pos, Anchor(d->action));
			s->yaw = (int16_t)(CrtRand() % 4096);
			int32_t r = CrtRand() % 30;
			s->scale = 0x355;
			s->scale_vel = 0x355;
			s->yaw_vel = (int16_t)(r + 0x3C);
			s = (ModelNode *)AddTaskToQueue(&Queues().second, game_addr(ORIG_FlameSwirlTask));
			ff8fx::copy<(4)>(s->pos, Anchor(d->action));
			s->counter = 0;
			ff8fx::copy<(4)>(&s->pos[2], &Anchor(d->action)[2]);
			s->yaw = (int16_t)(CrtRand() % 4096);
			s->yaw_vel = (int16_t)(-40 - CrtRand() % 20);
			s->scale = 0x2AA;
			s->scale_vel = 0x2AA;
		}
		if (d->counter == 0x21)
		{
			ModelNode *b = (ModelNode *)AddTaskToQueue(&Queues().second, game_addr(ORIG_BurstTask));
			b->counter = 0;
			ff8fx::copy<(8)>(b->pos, Anchor(d->action));
			b->scale = 0x555;
			b->scale_vel = 0x555;
		}
		if (d->counter >= 0x24 && d->counter <= 0x30)
		{
			int32_t spread = (EntitySize(d->slot) * 3400) >> 12;
			if (spread > 1800) spread = 1800;
			int32_t half = spread >> 1;
			for (int k = 0; k < 2; k++)
			{
				FlameNode *f = (FlameNode *)AddTaskToQueue(&Queues().second, game_addr(ORIG_AreaFlameTask));
				f->frame = 0;
				f->delay = (int16_t)(CrtRand() % 6 + k);
				ff8fx::copy<(8)>(f->pos, Anchor(d->action));
				f->pos[0] += (int16_t)(CrtRand() % spread - half);
				f->pos[1] += (int16_t)(CrtRand() % spread - half);
				int32_t rz = CrtRand() % spread;
				int32_t y = f->pos[1];
				f->pos[2] += (int16_t)(rz - half);
				int16_t h = EntityHeight(d->slot);
				f->ground = h;
				if (y > h - 0x1C2)
					f->pos[1] += (int16_t)(-350 - CrtRand() % 200);
				f->scale = (int16_t)(CrtRand() % 0xC00 + 0xB00);
			}
		}
		if (d->counter == 0x30) ApplyActionResultToTarget(Ctx()->actions[d->action].targets);
		if (d->counter == 0x2C)
		{
			ActionData *acts = Ctx()->actions;
			int next = d->action + 1;
			if (next <= acts[0].last_action)
			{
				DirectorNode *t = (DirectorNode *)AddTaskToQueue(&Queues().second, game_addr(ORIG_Director));
				t->counter = 0;
				t->action = (int16_t)next;
				t->slot = (int16_t)(uint16_t)Ctx()->actions[next].targets[0];
			}
		}
		if (d->action == 0 && d->counter <= 8) SetScreenFlash((uint32_t)shl32(d->counter, 8), 0x101060);
		else if ((uint16_t)d->action == (uint16_t)Ctx()->actions[0].last_action && d->counter >= 0x34)
			SetScreenFlash((uint32_t)shl32(0x3C - d->counter, 8), 0x101080);
		if (d->counter == 1)
		{
			int16_t pos[4];
			GetDefaultEffectPosition(Entity(d->slot), pos);
			BdPlaySE3D(SOUND_Firaga(), 0, pos);
		}
		d->counter++;
		if (d->counter <= 0x3C) return 0;
		if ((uint16_t)d->action == (uint16_t)Ctx()->actions[0].last_action) SetScreenFlash(0, 0);
		return TASK_END;
	}

	static void AreaFlameDraw(const FlameNode *p)
	{
		TransformCameraByShadowRotation(p->pos, p->scale, -(p->scale >> 4));
		uint8_t *h = AllocHeader(0xB4);
		*(const void **)h = SEQ_AreaFlame();
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
		*(uint16_t *)(h + 0x24) = 0xC;
		h[0x1E] = level;
		h[0x1D] = level;
		h[0x1C] = level;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT44(), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static uint32_t __cdecl AreaFlameTask(TaskNode *n)
	{
		FlameNode *p = (FlameNode *)n;
		if (p->delay > 0)
		{
			if (DrawOnly()) return 0;
			p->delay--;
			return 0;
		}
		AreaFlameDraw(p);
		if (DrawOnly()) return 0;
		p->frame++;
		return p->frame >= 0x10 ? TASK_END : 0;
	}

	static void ModelMatrix(int16_t ax, const ModelNode *p)
	{
		int16_t angles[4] = { ax, p->yaw, 0, 0 };
		Mat4x3 m = {};
		ComposeZYXRotationMatrix(angles, &m);
		m.t[0] = p->pos[0];
		m.t[1] = p->pos[1];
		m.t[2] = p->pos[2];
		int32_t sv[4] = { p->scale, p->scale, p->scale, 0 };
		Scale3DMatrix(&m, sv);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
	}

	static void BurstMatrix(const ModelNode *p) { ModelMatrix(0x400, p); }

	static void BurstHeader(uint8_t *h, int16_t c)
	{
		*(const void **)h = MODEL_Burst();
		*(uint32_t *)(h + 0x1C) = 0x33;
		if (c >= 0xA)
		{
			h[0xA] = 0;
			h[9] = 0;
			h[8] = 0;
			*(uint32_t *)(h + 0x1C) = 0xF3;
			*(int32_t *)(h + 0xC) = (c - 0xA) * 409;
		}
	}

	static void BurstUpdate(ModelNode *p)
	{
		int16_t v = p->scale_vel;
		int32_t x = v;
		int32_t q = (int32_t)(((int64_t)x * 0x2AAAAAAB) >> 32);
		p->scale = (int16_t)(p->scale + v);
		q >>= 1;
		q += (int32_t)((uint32_t)q >> 31);
		p->counter++;
		p->scale_vel = (int16_t)(v - q);
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
		return p->counter >= 0x14 ? TASK_END : 0;
	}

	static void FlameSwirlMatrix(const ModelNode *p) { ModelMatrix(0, p); }

	static void FlameSwirlHeader(uint8_t *h, int16_t c)
	{
		*(const void **)h = MODEL_FlameSwirl();
		*(uint32_t *)(h + 0x1C) = 0x30;
		if (c >= 0xE)
		{
			h[0xA] = 0;
			h[9] = 0;
			h[8] = 0;
			*(uint32_t *)(h + 0x1C) = 0xF0;
			*(int32_t *)(h + 0xC) = (c - 0xE) * 409;
		}
	}

	static void FlameSwirlUpdate(ModelNode *p)
	{
		int16_t v = p->scale_vel;
		p->yaw = (int16_t)(p->yaw + p->yaw_vel);
		p->scale = (int16_t)(p->scale + v);
		p->counter++;
		p->scale_vel = (int16_t)(v - v / 4);
	}

	static uint32_t __cdecl FlameSwirlTask(TaskNode *n)
	{
		ModelNode *p = (ModelNode *)n;
		FlameSwirlMatrix(p);
		uint8_t *h = AllocHeader(0x58);
		FlameSwirlHeader(h, p->counter);
		PacketCursor() = RenderPrimModel(h, RenderOT44(), 2, PacketCursor());
		FieldFree(0x58);
		if (DrawOnly()) return 0;
		FlameSwirlUpdate(p);
		return p->counter >= 0x18 ? TASK_END : 0;
	}

	static void FireballDraw(const ModelNode *p)
	{
		TransformCameraByShadowRotation(p->pos, p->scale, -(p->scale >> 5));
		uint8_t *h = AllocHeader(0x58);
		*(const void **)h = MODEL_Fireball();
		*(uint32_t *)(h + 0x1C) = 0x30;
		PacketCursor() = RenderPrimModel(h, RenderOT44(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static void FireballUpdate(ModelNode *p)
	{
		int16_t v = p->scale_vel;
		p->scale = (int16_t)(p->scale + v);
		p->counter++;
		p->scale_vel = (int16_t)(v + (int16_t)(v >> 6));
	}

	static uint32_t __cdecl FireballTask(TaskNode *n)
	{
		ModelNode *p = (ModelNode *)n;
		FireballDraw(p);
		if (DrawOnly()) return 0;
		int16_t alive = Anchor(p->action)[3];
		FireballUpdate(p);
		return alive ? 0 : TASK_END;
	}

	static void SparkMatrix(const SparkNode *p)
	{
		int32_t axis[4] = { p->axis[0], p->axis[1], p->axis[2], 0 };
		int32_t dir[4] = { p->offset[0], p->offset[1], p->offset[2], 0 };
		Mat4x3 m = {};
		BuildAxisAngleRotationMatrix(p->angle, &m, axis);
		TransformVectorBy3x3Matrix(&m, dir, dir);
		int32_t down[4] = { 0, -0x1000, 0, 0 };
		int32_t angle = RotationBetweenVectors(down, dir, axis);
		BuildAxisAngleRotationMatrix(angle, &m, axis);
		const int16_t *a = Anchor(p->action);
		m.t[0] = a[0];
		m.t[1] = a[1];
		m.t[2] = a[2];
		int32_t sv[4] = { 0x2000, p->size, 0x2000, 0 };
		Scale3DMatrix(&m, sv);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
	}

	static void SparkHeader(uint8_t *h, int16_t c)
	{
		*(const void **)h = MODEL_Spark();
		h[0xA] = 0;
		h[9] = 0;
		h[8] = 0;
		*(uint32_t *)(h + 0x1C) = 0x30;
		if (c < 6)
		{
			*(int32_t *)(h + 0xC) = 0x1000 - 682 * c;
			*(uint32_t *)(h + 0x1C) = 0xF0;
		}
		else if (c >= 0xE)
		{
			*(int32_t *)(h + 0xC) = 682 * (c - 0xE);
			*(uint32_t *)(h + 0x1C) = 0xF0;
		}
	}

	static void SparkUpdate(SparkNode *p)
	{
		p->angle = (int16_t)(p->angle + p->spin);
		p->counter++;
	}

	static uint32_t __cdecl SparkTask(TaskNode *n)
	{
		SparkNode *p = (SparkNode *)n;
		SparkMatrix(p);
		uint8_t *h = AllocHeader(0x58);
		SparkHeader(h, p->counter);
		PacketCursor() = RenderPrimModel(h, RenderOT44(), 2, PacketCursor());
		FieldFree(0x58);
		if (DrawOnly()) return 0;
		SparkUpdate(p);
		return p->counter >= 0x14 ? TASK_END : 0;
	}
}

	void register_mag143_firaga()
	{
		register_port(firaga143::ORIG_RootTask, (void *)firaga143::RootTask, "F143 RootTask", 143);
		register_port(firaga143::ORIG_Director, (void *)firaga143::DirectorTask, "F143 Director", 143);
		register_port(firaga143::ORIG_AreaFlameTask, (void *)firaga143::AreaFlameTask, "F143 AreaFlameTask", 143);
		register_port(firaga143::ORIG_BurstTask, (void *)firaga143::BurstTask, "F143 BurstTask", 143);
		register_port(firaga143::ORIG_FlameSwirlTask, (void *)firaga143::FlameSwirlTask, "F143 FlameSwirlTask", 143);
		register_port(firaga143::ORIG_FireballTask, (void *)firaga143::FireballTask, "F143 FireballTask", 143);
		register_port(firaga143::ORIG_SparkTask, (void *)firaga143::SparkTask, "F143 SparkTask", 143);
	}
}

