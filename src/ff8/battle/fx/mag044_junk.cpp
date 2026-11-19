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

// Effect 44: Junk: junk

#include "mag_common.h"

namespace ff8fx
{
namespace junk044
{
	using namespace eng;
	using namespace magc;

	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::g_Junk_CastContext)); }
	inline uint8_t *&Caster() { return var<uint8_t *>(game_addr(game::g_Junk_CasterEntity)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::g_Junk_PacketCursor)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::g_Junk_TexBase)); }
	inline TaskQueue *QDebris() { return (TaskQueue *)game_addr(game::g_Junk_DebrisQueue); }
	inline TaskQueue *QItems() { return (TaskQueue *)game_addr(game::g_Junk_ItemQueue); }
	inline TaskQueue *QSmoke() { return (TaskQueue *)game_addr(game::g_Junk_SmokeQueue); }
	inline TaskQueue *QControl() { return (TaskQueue *)game_addr(game::g_Junk_ControlQueue); }
	inline int16_t &BattleWord1D97712() { return var<int16_t>(game_addr(game::BATTLE_CAMERA_SHAKE_OFFSET_Y)); }

	static constexpr game::Id ORIG_RootTask = game::MAG_044_Junk_RootTask;
	static constexpr game::Id ORIG_ControlTask = game::MAG_044_Junk_ControlTask;
	static constexpr game::Id ORIG_SmokeTask = game::MAG_044_Junk_SmokeTask;
	static constexpr game::Id ORIG_ItemTask = game::MAG_044_Junk_ItemTask;
	static constexpr game::Id ORIG_DebrisTask = game::MAG_044_Junk_DebrisTask;
	static const void *SOUND_Junk() { return (const void *)game_addr(game::g_Junk_Sound); }
	static constexpr game::Id SEQ_Smoke = game::g_Junk_SmokeSequence;
	static constexpr game::Id MODEL_Debris = game::g_Junk_DebrisLayout;
	static constexpr game::Id ITEM_KINDS = game::g_Junk_ItemKinds;

	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t flag, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, flag, out); }
	inline int32_t ComputeCos(int32_t a) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(a); }
	inline void QueueChainTransformation(uint8_t *entity, int32_t id) { fn<void (__cdecl *)(uint8_t *, int32_t)>(game_addr(game::QueueChainTransformation))(entity, id); }
	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
	inline void StreamLoad(int32_t id, uint32_t dst, int32_t n) { fn<void (__cdecl *)(int32_t, uint32_t, int32_t)>(game_addr(game::Stream_QueueLoad))(id, dst, n); }
	inline void StreamPump() { fn<void (__cdecl *)()>(game_addr(game::Stream_Pump))(); }
	inline int32_t StreamBusy(int32_t a) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::BattleFile_PreLoad_IsLoaded))(a); }

#pragma pack(push, 1)
	struct RootNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t pad0E;
		uint8_t started;
		uint32_t arena;
	};
	struct ControlTarget
	{
		uint8_t *entity;
		uint32_t save1c;
		uint32_t save20;
		int16_t amp;
		int16_t pad;
	};
	struct ControlNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		int16_t rnd;
		int16_t pad12;
		ControlTarget tg[3];
		int16_t avg[3];
		int16_t pad4A;
	};
	struct SmokeNode
	{
		TaskNode hdr;
		int16_t pos[3];
		int16_t life;
		int16_t vel[3];
		int16_t pad1A;
		int16_t rot;
		int16_t pad1E;
	};
	struct ItemNode
	{
		TaskNode hdr;
		int16_t x, y, z;
		int16_t life;
		int16_t vx, vy, vz;
		int16_t pad1A;
		int16_t rot[3];
		int16_t pad22;
		int16_t drot[2];
		uint8_t pad28[4];
		const uint8_t *kind;
		uint8_t *entity;
	};
	struct DebrisNode
	{
		TaskNode hdr;
		int16_t x, y, z;
		int16_t delay;
		int16_t angle;
		int16_t speed;
		uint8_t layout[0x40];
	};
	struct PrimArg
	{
		Mat4x3 m;
		uint32_t morph;
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x14 && sizeof(ControlNode) == 0x4C && sizeof(SmokeNode) == 0x20, "Junk nodes");
	static_assert(sizeof(ItemNode) == 0x34 && sizeof(DebrisNode) == 0x58 && sizeof(PrimArg) == 0x24, "Junk nodes");

	static void RotationY(int16_t angle, Mat4x3 *out)
	{
		const int32_t a = -(int32_t)angle;
		uint32_t *w = (uint32_t *)out;
		w[0] = 0; w[1] = 0; w[2] = 0; w[3] = 0; w[4] = 0;
		const int32_t s = ComputeSin(a);
		const int32_t c = ComputeCos(a);
		int16_t *m = (int16_t *)out;
		m[6] = (int16_t)s;
		m[2] = (int16_t)-s;
		m[0] = (int16_t)c;
		m[4] = 0x1000;
		m[8] = (int16_t)c;
	}

	static void DamageEntity(uint8_t *entity)
	{
		const int32_t slot = (int32_t)((uint32_t)entity - game_addr(game::BattleEntitySlotData)) / 0x9C;
		for (uint32_t i = 0; i < Ctx()->actions[0].target_count; i++)
		{
			uint8_t *t = Ctx()->actions[0].targets + i * TARGET_STRIDE;
			if ((int32_t)t[0] == slot) ApplyActionResultToTarget(t);
		}
	}

	static void SmokeOne(SmokeNode *s, int16_t x, int16_t z, int32_t radius, int32_t spread, int32_t base)
	{
		const int32_t a = CrtRand();
		s->vel[0] = (int16_t)(ComputeCos(a) * radius / 4096);
		const int32_t sz = ComputeSin(a) * radius / 4096;
		s->vel[2] = (int16_t)sz;
		s->pos[0] = (int16_t)(x + s->vel[0]);
		s->pos[1] = 0;
		s->pos[2] = (int16_t)(z + sz);
		s->life = 0xD;
		s->vel[0] = (int16_t)(s->vel[0] / 8);
		s->vel[2] = (int16_t)((int16_t)sz / 8);
		s->vel[1] = (int16_t)-(radius / 8);
		s->rot = (int16_t)(CrtRand() * spread / 32768 + base);
	}

	static void SmokeAtCaster(int32_t count, int32_t bone, int32_t radius, int32_t spread, int32_t base)
	{
		int16_t pos[4];
		GetEffectSpawnPosition(Caster(), bone, 0, pos);
		while (count-- != 0)
		{
			SmokeNode *s = (SmokeNode *)AddTaskToQueue(QSmoke(), game_addr(ORIG_SmokeTask));
			if (!s) break;
			SmokeOne(s, pos[0], pos[2], radius, spread, base);
		}
	}

	static void SmokeAt(int32_t count, const int16_t *pos, int32_t radius, int32_t spread, int32_t base)
	{
		while (count-- != 0)
		{
			SmokeNode *s = (SmokeNode *)AddTaskToQueue(QSmoke(), game_addr(ORIG_SmokeTask));
			if (!s) break;
			SmokeOne(s, pos[0], pos[2], radius, spread, base);
		}
	}

	static ItemNode *NewItem(int32_t x, int32_t z, uint8_t *entity, int32_t kind)
	{
		ItemNode *it = (ItemNode *)AddTaskToQueue(QItems(), game_addr(ORIG_ItemTask));
		if (!it) return nullptr;
		it->y = (int16_t)0xF000;
		it->life = 0x14;
		it->vz = 0;
		it->vx = 0;
		it->vy = 0x320;
		it->kind = (const uint8_t *)(game_addr(ITEM_KINDS) + kind * 24);
		it->entity = entity;
		it->x = (int16_t)x;
		it->z = (int16_t)z;
		return it;
	}

	static void DrawSmoke(const SmokeNode *s)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		TransformCameraByShadowRotation(s->pos, (int32_t)s->rot, -((int32_t)s->rot >> 2));
		*(uint32_t *)h = game_addr(SEQ_Smoke);
		*(int16_t *)(h + 0x24) = 0;
		*(int16_t *)(h + 4) = (int16_t)(*(const uint16_t *)(game_addr(SEQ_Smoke) + 8) - s->life);
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static void DrawItem(const ItemNode *it)
	{
		const uint8_t *kind = it->kind;
		uint8_t *b = (uint8_t *)FieldAlloc(0x78);
		Mat4x3 *m = (Mat4x3 *)b;
		ComposeZYXRotationMatrix(it->rot, m);
		m->t[0] = it->x;
		m->t[1] = it->y;
		m->t[2] = it->z;
		ComposeAffineTransform(&Camera(), m, m);
		GteSetRotMatrix(m);
		GteSetTransVector(m);
		uint8_t *h = b + 0x20;
		if (it->life < 4)
		{
			*(uint32_t *)(h + 0x1C) = 0xC3;
			*(uint32_t *)(h + 8) = 0;
			*(int32_t *)(h + 0xC) = (4 - (int32_t)it->life) << 10;
		}
		else *(uint32_t *)(h + 0x1C) = 0;
		const int32_t k = (int32_t)it->life % (int32_t)*(const int16_t *)(kind + 0x10);
		*(uint32_t *)h = *(const uint32_t *)(kind + k * 4);
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x78);
	}

	static void __cdecl PartCallback(prim::Layout *l, prim::Record *r, int arg_)
	{
		const PrimArg *arg = (const PrimArg *)arg_;
		if (((int32_t)r->scale[2] | *(const int32_t *)r->scale) == 0) return;
		if (r->a >= 0x1000 && *(const uint32_t *)r->rgb == 0) return;
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		const int32_t object = (int16_t)r->flags_lo;
		uint8_t *model = l->data + *(const int32_t *)(l->data + object * 4 + 8);
		*(uint8_t **)h = model;
		const int16_t f0 = r->b0, f1 = r->b1;
		const int32_t nverts = *(const int32_t *)(model + 4);
		if (f0 == f1)
		{
			if (f0 == 0) *(uint8_t **)(h + 4) = model + 0xC;
			else *(uint8_t **)(h + 4) = model + (uint32_t)mul32(nverts, f0) * 8 + 0xC;
		}
		else
		{
			const int16_t t = r->b;
			if (t == 0)
			{
				if (f0 == 0) *(uint8_t **)(h + 4) = model + 0xC;
				else *(uint8_t **)(h + 4) = model + (uint32_t)mul32(nverts, f0) * 8 + 0xC;
			}
			else if (t == 0x1000)
			{
				if (f1 == 0) *(uint8_t **)(h + 4) = model + 0xC;
				else *(uint8_t **)(h + 4) = model + (uint32_t)mul32(nverts, f1) * 8 + 0xC;
			}
			else
			{
				BlendVertexFrames((uint32_t)model, f0, f1, t, arg->morph);
				*(uint32_t *)(h + 4) = arg->morph;
			}
		}
		Mat4x3 m = {};
		RotationFromAngles(r->rot, &m);
		m.t[0] = r->pos[0];
		m.t[1] = r->pos[1];
		m.t[2] = r->pos[2];
		ComposeAffineTransform(&arg->m, &m, &m);
		if (!(*(const uint32_t *)r->scale == 0x10001000 && r->scale[2] == 0x1000))
		{
			const int32_t v[3] = { r->scale[0], r->scale[1], r->scale[2] };
			Scale3DMatrix(&m, v);
		}
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		*(uint32_t *)(h + 0x1C) = 0x2000;
		*(int32_t *)(h + 0xC) = r->a;
		if (r->a != 0)
		{
			*(uint32_t *)(h + 0x1C) = 0x20C0;
			*(uint32_t *)(h + 8) = *(const uint32_t *)r->rgb;
		}
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static void DebrisMatrix(const DebrisNode *d, int16_t angle, PrimArg *arg)
	{
		RotationY(angle, &arg->m);
		arg->m.t[0] = d->x;
		arg->m.t[1] = d->y;
		arg->m.t[2] = d->z;
		ComposeAffineTransform(&Camera(), &arg->m, &arg->m);
	}
}
}

namespace ff8fx
{
namespace junk044
{
	static uint32_t __cdecl DebrisTask(TaskNode *n)
	{
		DebrisNode *d = (DebrisNode *)n;
		if (d->delay <= 0)
		{
			PrimArg arg;
			DebrisMatrix(d, d->angle, &arg);
			arg.morph = TexBase() + 0x212D4;
			if (prim::play((prim::Layout *)d->layout, PartCallback, (int)&arg, 0) == 0) return TASK_END;
		}
		d->angle = (int16_t)(d->angle + d->speed);
		d->delay--;
		return 0;
	}

	static uint32_t __cdecl ItemTask(TaskNode *n)
	{
		ItemNode *it = (ItemNode *)n;
		DrawItem(it);
		it->life--;
		if (it->life == 0) return TASK_END;
		const int16_t vx = it->vx;
		it->vy = (int16_t)(it->vy + 0x32);
		const int16_t vy = it->vy;
		it->x = (int16_t)(it->x + vx);
		it->y = (int16_t)(it->y + vy);
		const int32_t floor = it->entity ? *(const int16_t *)(it->entity + 0x92) : 0;
		if ((int32_t)it->y >= floor)
		{
			it->y = (int16_t)(it->y - vy);
			int32_t q = (int32_t)(((int64_t)0x55555555 * (int32_t)vy) >> 32);
			q -= (int32_t)vy;
			q >>= 1;
			q += (int32_t)((uint32_t)q >> 31);
			it->vy = (int16_t)q;
			it->drot[0] = (int16_t)((CrtRand() & 0x1FF) - 0x100);
			if (it->entity)
			{
				it->y = (int16_t)floor;
				int16_t pos[4];
				GetDefaultEffectPosition(it->entity, pos);
				int32_t a = CrtRand();
				int32_t count = 8;
				DebrisNode *d = (DebrisNode *)AddTaskToQueue(QDebris(), game_addr(ORIG_DebrisTask));
				while (d)
				{
					d->x = (int16_t)(ComputeSin(a) / 16 + pos[0]);
					d->z = (int16_t)(ComputeCos(a) / 16 + pos[2]);
					const uint8_t *e = it->entity;
					const int32_t r = CrtRand();
					d->y = (int16_t)((int16_t)(*(const int16_t *)(e + 0x92) + *(const int16_t *)(e + 0x1E)) - (r & 0xFF) - 0x80);
					d->delay = (int16_t)(count + 10);
					d->angle = (int16_t)CrtRand();
					d->speed = (int16_t)((CrtRand() & 0xFF) + 0x100);
					if (CrtRand() < 0x4000) d->speed = (int16_t)-d->speed;
					DecodeModelPrimLayout(game_addr(MODEL_Debris), d->layout, 0x40);
					a = a + (CrtRand() & 0xFF) + 0x180;
					if (--count == 0) break;
					d = (DebrisNode *)AddTaskToQueue(QDebris(), game_addr(ORIG_DebrisTask));
				}
				DamageEntity(it->entity);
				it->vz = 0x50;
				it->entity = nullptr;
			}
			else
			{
				const int32_t c = *(const int16_t *)(it->kind + 0x12);
				SmokeAt(c, &it->x, c * 200, *(const int16_t *)(it->kind + 0x14), *(const int16_t *)(it->kind + 0x16));
			}
		}
		it->z = (int16_t)(it->z + it->vz);
		it->rot[0] = (int16_t)(it->rot[0] + it->drot[0]);
		it->rot[1] = (int16_t)(it->rot[1] + it->drot[1]);
		return 0;
	}

	static uint32_t __cdecl SmokeTask(TaskNode *n)
	{
		SmokeNode *s = (SmokeNode *)n;
		DrawSmoke(s);
		s->life--;
		if (s->life == 0) return TASK_END;
		s->pos[1] = (int16_t)(s->pos[1] + s->vel[1]);
		s->pos[2] = (int16_t)(s->pos[2] + s->vel[2]);
		s->pos[0] = (int16_t)(s->pos[0] + s->vel[0]);
		return 0;
	}

	static uint32_t __cdecl ControlTask(TaskNode *n)
	{
		ControlNode *c = (ControlNode *)n;
		if (c->counter == 0) BdPlaySE(SOUND_Junk(), 0, 0x80);
		switch ((uint32_t)((int32_t)c->counter - 1))
		{
		case 0:
			SmokeAtCaster(3, 0x17, 0xC8, 0x7D0, 0x7D0);
			SmokeAtCaster(3, 0x15, 0xC8, 0x7D0, 0x7D0);
			BattleWord1D97712() = 0x96;
			break;
		case 7: case 21:
			SmokeAtCaster(3, 0x15, 0xC8, 0x7D0, 0x7D0);
			BattleWord1D97712() = 0x96;
			break;
		case 14: case 28:
			SmokeAtCaster(3, 0x17, 0xC8, 0x7D0, 0x7D0);
			BattleWord1D97712() = 0x96;
			break;
		default:
			break;
		}
		const uint32_t b = (uint32_t)((int32_t)c->counter - 3);
		if (b < 0x23)
		{
			const uint32_t phase = b % 7;
			if (phase == 0)
			{
				for (uint32_t i = 0; i < Ctx()->actions[0].target_count; i++)
					c->tg[i].amp = (int16_t)((CrtRand() & 0x1FF) + 0x100);
			}
			const int32_t s = ComputeSin((int32_t)(phase << 11) / 7 + 0x124);
			for (uint32_t i = 0; i < Ctx()->actions[0].target_count; i++)
			{
				uint8_t *e = c->tg[i].entity;
				*(int16_t *)(e + 0x1E) = (int16_t)((int16_t)(c->tg[i].save1c >> 16) - (int16_t)(mul32(c->tg[i].amp, s) / 4096));
				if (phase == 6)
				{
					SmokeAt(3, (const int16_t *)(e + 0x1C), 0x12C, 0x7D0, 0x7D0);
					QueueChainTransformation(e, 4);
				}
			}
		}
		const uint32_t f = (uint32_t)((int32_t)c->counter - 0xF);
		if (f < 0x20 && f % 3 == 0)
		{
			const uint32_t kind = ((uint32_t)(int32_t)c->rnd + (f >> 2)) % 6;
			const int32_t rz = CrtRand();
			const int32_t z = (rz & 0x7FF) + c->avg[2] - 0x400;
			const int32_t rx = CrtRand();
			const int32_t x = (rx & 0xFFF) + c->avg[0] - 0x800;
			ItemNode *it = NewItem(x, z, nullptr, (int32_t)kind);
			if (it)
			{
				it->rot[0] = (int16_t)(((CrtRand() * 800) >> 15) - 0x190);
				it->rot[1] = (int16_t)CrtRand();
				it->rot[2] = 0;
				it->drot[0] = (int16_t)((CrtRand() & 0xFF) - 0x80);
				it->drot[1] = (int16_t)((CrtRand() & 0xFF) - 0x80);
			}
		}
		if (c->counter == 0x3E)
		{
			for (uint32_t i = 0; i < Ctx()->actions[0].target_count; i++)
			{
				uint8_t *e = c->tg[i].entity;
				int16_t pos[4];
				GetDefaultEffectPosition(e, pos);
				ItemNode *it = NewItem(pos[0], pos[2], e, 6);
				if (it)
				{
					it->life = 0x18;
					it->rot[0] = (int16_t)((CrtRand() & 0x1FF) - 0x500);
					it->rot[1] = (int16_t)CrtRand();
					it->rot[2] = 0;
					it->drot[0] = 0;
					it->drot[1] = 0;
				}
			}
		}
		c->counter++;
		if (c->counter >= 0x64)
		{
			for (uint32_t i = 0; i < Ctx()->actions[0].target_count; i++)
			{
				uint8_t *e = c->tg[i].entity;
				*(uint32_t *)(e + 0x1C) = c->tg[i].save1c;
				*(uint32_t *)(e + 0x20) = c->tg[i].save20;
			}
			return TASK_END;
		}
		return 0;
	}

	static uint32_t __cdecl RootTask(TaskNode *n)
	{
		RootNode *r = (RootNode *)n;
		if (r->arena)
		{
			PacketCursor() = TexBase() + 0x12D4;
			r->arena = 0;
		}
		else
		{
			PacketCursor() = TexBase() + 0x112D4;
			r->arena = 1;
		}
		if (r->counter == 1 && !r->started)
		{
			r->started = 1;
			InitTaskQueuePool(QDebris(), (void *)TexBase(), 0x58, 0x20);
			InitTaskQueuePool(QItems(), (void *)(TexBase() + 0xB00), 0x34, 0x10);
			InitTaskQueuePool(QSmoke(), (void *)(TexBase() + 0xE40), 0x20, 0x20);
			InitTaskQueuePool(QControl(), (void *)(TexBase() + 0x1240), 0x4C, 1);
			ControlNode *c = (ControlNode *)AddTaskToQueue(QControl(), game_addr(ORIG_ControlTask));
			Memset32(&c->counter, 0, 0x10);
			c->avg[2] = 0;
			c->avg[1] = 0;
			c->avg[0] = 0;
			const int32_t count = Ctx()->actions[0].target_count;
			for (int32_t i = 0; i < count; i++)
			{
				uint8_t *e = Entity(Ctx()->actions[0].targets[i * TARGET_STRIDE]);
				c->tg[i].entity = e;
				c->tg[i].save1c = *(const uint32_t *)(e + 0x1C);
				c->tg[i].save20 = *(const uint32_t *)(e + 0x20);
				int16_t pos[4];
				GetDefaultEffectPosition(e, pos);
				c->avg[0] = (int16_t)(c->avg[0] + pos[0]);
				c->avg[1] = (int16_t)(c->avg[1] + pos[1]);
				c->avg[2] = (int16_t)(c->avg[2] + pos[2]);
			}
			c->avg[0] = (int16_t)((int32_t)c->avg[0] / count);
			c->avg[1] = (int16_t)((int32_t)c->avg[1] / count);
			c->avg[2] = (int16_t)((int32_t)c->avg[2] / count);
			c->rnd = (int16_t)CrtRand();
			StreamStateInit((void *)(TexBase() + 0x128C));
		}
		if (r->started && r->counter < 0x72)
		{
			ExecuteTaskQueue(QControl());
			ExecuteTaskQueue(QSmoke());
			ExecuteTaskQueue(QItems());
			ExecuteTaskQueue(QDebris());
		}
		if (r->counter == 0x73)
		{
			StreamLoad(0x168, TexBase() + 0x12D4, 1);
			StreamLoad(0x169, TexBase() + 0x12D4, 0);
		}
		if (r->started) StreamPump();
		if (r->counter > 0x73) return StreamBusy(1) != 0 ? TASK_END : 0;
		r->counter++;
		return 0;
	}
}

	void register_mag044_junk()
	{
		register_port(junk044::ORIG_RootTask, (void *)junk044::RootTask, "J044 RootTask", 44);
		register_port(junk044::ORIG_ControlTask, (void *)junk044::ControlTask, "J044 ControlTask", 44);
		register_port(junk044::ORIG_SmokeTask, (void *)junk044::SmokeTask, "J044 SmokeTask", 44);
		register_port(junk044::ORIG_ItemTask, (void *)junk044::ItemTask, "J044 ItemTask", 44);
		register_port(junk044::ORIG_DebrisTask, (void *)junk044::DebrisTask, "J044 DebrisTask", 44);
	}
}

