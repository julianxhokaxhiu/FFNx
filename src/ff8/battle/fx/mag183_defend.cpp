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
namespace defend183
{
	using namespace eng;
	using namespace magc;

	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::MAG_183_DEFEND_CastContext)); }
	inline uint32_t &CasterSlot() { return var<uint32_t>(game_addr(game::MAG_183_DEFEND_CasterSlot)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_183_DEFEND_PacketCursor)); }
	inline TaskQueue *EffectQueue() { return (TaskQueue *)game_addr(game::MAG_183_DEFEND_EffectQueue); }
	static constexpr game::Id ARENA0 = game::MAG_183_DEFEND_PacketArena0, ARENA1 = game::MAG_183_DEFEND_PacketArena1; static const uint32_t ARENA_SIZE = 0x8000;

	static constexpr game::Id ORIG_RootTask = game::MAG_183_DEFEND_RootTask;
	static constexpr game::Id ORIG_Director = game::MAG_183_DEFEND_Director;
	static constexpr game::Id ORIG_ShieldA = game::MAG_183_DEFEND_ShieldA;
	static constexpr game::Id ORIG_ShieldB = game::MAG_183_DEFEND_ShieldB;
	static constexpr game::Id MODEL_ShieldA = game::MAG_183_DEFEND_ShieldAModel, MODEL_ShieldB = game::MAG_183_DEFEND_ShieldBModel;
	static constexpr game::Id SHIELD_COLOURS = game::MAG_183_DEFEND_ShieldBColours;
	static const void *SOUND_Defend() { return (const void *)game_addr(game::MAG_183_DEFEND_Sound); }

	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t flag, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, flag, out); }
	inline void MatrixMultiplySVector(const Mat4x3 *m, const int16_t *in, int16_t *out) { fn<void (__cdecl *)(const Mat4x3 *, const int16_t *, int16_t *)>(game_addr(game::matrixMultiplyVector))(m, in, out); }
	inline uint32_t ScrollRender(void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::MAG_066_RenderScrollPrimModel))(h, ot, mode, cursor); }

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
		int16_t pad0E;
		int16_t pos[4];
		int16_t angle;
		int16_t pad1A;
		int16_t scale;
		int16_t step;
		uint8_t pad20[4];
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x10 && sizeof(EffectNode) == 0x24, "Defend nodes");

	inline uint8_t *Caster() { return Entity((int)CasterSlot()); }

	struct ShieldDraw
	{
		uint32_t model;
		int16_t front;
		int32_t scale;
		int32_t scroll;
		bool fade_on;
		int32_t fade;
		uint32_t colour;
		bool b;
	};

	static void DrawShield(const EffectNode *t, const ShieldDraw &d)
	{
		int16_t ang[4];
		ang[0] = 0;
		ang[1] = t->angle;
		ang[2] = 0;
		Mat4x3 m;
		ComposeZYXRotationMatrix(ang, &m);
		int16_t v[4];
		v[0] = 0;
		v[1] = 0;
		v[2] = d.front;
		MatrixMultiplySVector(&m, v, v);
		m.t[0] = t->pos[0] + v[0];
		m.t[1] = t->pos[1];
		m.t[2] = t->pos[2] + v[2];
		int32_t sv[3];
		sv[0] = d.scale;
		sv[1] = d.scale;
		sv[2] = d.scale;
		Scale3DMatrix(&m, sv);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		uint8_t *h = AllocHeader(0x90);
		if (d.b)
		{
			*(int16_t *)(h + 0x18) = 0x40;
			*(int16_t *)(h + 0x1C) = 0x40;
			*(uint32_t *)(h + 8) = 0;
			*(int16_t *)(h + 0x1A) = 0;
			*(uint32_t *)(h + 0x10) = 0;
			*(uint32_t *)h = d.model;
			*(uint32_t *)(h + 0x20) = 0x33;
			*(int16_t *)(h + 0x1E) = 0x80;
			*(int32_t *)(h + 0x14) = d.scroll;
			if (d.fade_on)
			{
				*(int32_t *)(h + 0xC) = d.fade;
				*(uint32_t *)(h + 0x20) = 0xF3;
			}
		}
		else
		{
			*(uint32_t *)(h + 8) = 0;
			*(int16_t *)(h + 0x18) = 0;
			*(int16_t *)(h + 0x1A) = 0;
			*(uint32_t *)(h + 0x10) = 0;
			*(uint32_t *)h = d.model;
			*(uint32_t *)(h + 0x20) = 0x33;
			*(int16_t *)(h + 0x1C) = 0x40;
			*(int16_t *)(h + 0x1E) = 0x80;
			*(int32_t *)(h + 0x14) = d.scroll;
			if (d.fade_on)
			{
				*(uint32_t *)(h + 0x20) = 0xF3;
				*(int32_t *)(h + 0xC) = d.fade;
			}
		}
		*(uint32_t *)(h + 0x24) = d.colour;
		PacketCursor() = ScrollRender(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x90);
	}

	static ShieldDraw ShieldAState(int16_t counter, int16_t scale)
	{
		ShieldDraw d;
		d.model = game_addr(MODEL_ShieldA);
		d.front = -600;
		d.scale = scale;
		d.scroll = mul32(counter, -10);
		d.fade_on = counter < 4;
		d.fade = d.fade_on ? shl32(4 - counter, 10) : 0;
		d.colour = 0x808080;
		d.b = false;
		return d;
	}

	static ShieldDraw ShieldBState(int16_t counter, int16_t scale)
	{
		ShieldDraw d;
		d.model = game_addr(MODEL_ShieldB);
		d.front = 200;
		d.scale = scale;
		d.scroll = mul32(counter, -6);
		d.fade_on = counter < 4 || counter >= 0x10;
		d.fade = counter < 4 ? shl32(4 - counter, 10) : counter >= 0x10 ? shl32(counter - 0x10, 10) : 0;
		d.colour = var<uint32_t>(game_addr(SHIELD_COLOURS) + (uint32_t)((int32_t)counter % 10) * 4);
		d.b = true;
		return d;
	}
}
}

namespace ff8fx
{
namespace defend183
{
	static uint32_t __cdecl ShieldATask(TaskNode *n)
	{
		EffectNode *t = (EffectNode *)n;
		DrawShield(t, ShieldAState(t->counter, t->scale));
		if (DrawOnly()) return 0;
		t->scale = (int16_t)(t->scale - t->step);
		t->counter++;
		return t->counter >= 0x10 ? TASK_END : 0;
	}

	static uint32_t __cdecl ShieldBTask(TaskNode *n)
	{
		EffectNode *t = (EffectNode *)n;
		DrawShield(t, ShieldBState(t->counter, t->scale));
		if (DrawOnly()) return 0;
		t->counter++;
		return t->counter >= 0x14 ? TASK_END : 0;
	}

	static uint32_t __cdecl DirectorTask(TaskNode *n)
	{
		if (DrawOnly()) return 0;
		EffectNode *t = (EffectNode *)n;
		if (t->counter == 0) GetEffectSpawnPosition(Caster(), 0xF1, 0, t->pos);
		if (t->counter == 1)
		{
			EffectNode *s = (EffectNode *)AddTaskToQueue(EffectQueue(), game_addr(ORIG_ShieldA));
			*(uint32_t *)&s->pos[0] = *(const uint32_t *)&t->pos[0];
			*(uint32_t *)&s->pos[2] = *(const uint32_t *)&t->pos[2];
			s->counter = 0;
			s->scale = 0x2000;
			s->step = 0x200;
			s->angle = *(const int16_t *)(Caster() + 0xE);
		}
		if (t->counter == 6)
		{
			EffectNode *s = (EffectNode *)AddTaskToQueue(EffectQueue(), game_addr(ORIG_ShieldB));
			*(uint32_t *)&s->pos[0] = *(const uint32_t *)&t->pos[0];
			*(uint32_t *)&s->pos[2] = *(const uint32_t *)&t->pos[2];
			s->counter = 0;
			s->scale = 0x700;
			s->angle = *(const int16_t *)(Caster() + 0xE);
		}
		if (t->counter == 0x1E) ApplyActionResultToTarget(Ctx()->actions[0].targets);
		if (t->counter == 1)
		{
			int16_t pos[4];
			GetDefaultEffectPosition(Caster(), pos);
			BdPlaySE3D(SOUND_Defend(), 0, pos);
		}
		t->counter++;
		return t->counter > 0x23 ? TASK_END : 0;
	}

	static uint32_t __cdecl RootTask(TaskNode *n)
	{
		RootNode *r = (RootNode *)n;
		PacketCursor() = game_addr(ARENA1);
		if (!(r->counter & 1)) PacketCursor() = game_addr(ARENA0);
		const int a = ExecuteTaskQueue(EffectQueue());
		r->counter++;
		return a ? 0 : TASK_END;
	}
}

	void register_mag183_defend()
	{
		register_port(defend183::ORIG_RootTask, (void *)defend183::RootTask, "D183 RootTask", 183);
		register_port(defend183::ORIG_Director, (void *)defend183::DirectorTask, "D183 Director", 183);
		register_port(defend183::ORIG_ShieldA, (void *)defend183::ShieldATask, "D183 ShieldA", 183);
		register_port(defend183::ORIG_ShieldB, (void *)defend183::ShieldBTask, "D183 ShieldB", 183);
	}
}

