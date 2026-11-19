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

// Effect 3: Thunder

#include "mag_common.h"

namespace ff8fx
{
namespace thunder003
{
	using namespace eng;
	using namespace magc;

	inline TaskQueuePair &Queues() { return var<TaskQueuePair>(game_addr(game::unk_2557078)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::unk_2557054)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::unk_2557058)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::g_MAG003_PacketCursor)); }

	static constexpr game::Id ORIG_RootTask = game::MAG_003_THUNDER_Bolt_Tick;
	static constexpr game::Id ORIG_BoltTask = game::MAG_003_THUNDER_RenderBoltLayers;
	static constexpr game::Id BOLT_POOL = game::unk_2557000;
	static const int BOLT_POOL_COUNT = 3;
	static const void *SOUND_Thunder() { return (const void *)game_addr(game::unk_15244D0); }
	static const void *FLASH_Colour() { return (const void *)game_addr(game::unk_15244D4); }
	static constexpr game::Id SEQ_Strike = game::unk_1522FB0;
	static constexpr game::Id SEQ_Upper = game::unk_1523204;
	static constexpr game::Id SEQ_Core = game::unk_1523458;
	static constexpr game::Id SEQ_Arc = game::unk_1523578;
	static constexpr game::Id SEQ_Glow = game::unk_1523684;
	static constexpr game::Id SEQ_Flash = game::unk_15237A4;
	static constexpr game::Id SEQ_Ground = game::unk_1523DE0;
	static constexpr game::Id SEQ_Smoke = game::unk_1523928;

	inline uint32_t RenderOT44() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44; }

	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t flag, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, flag, out); }
	inline int32_t ComputeCos(int32_t angle) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(angle); }
	inline void GteMVMVA_RotV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0))(); }
	inline void InsertPrimAltViewport(uint32_t bucket, void *packet) { fn<void (__cdecl *)(uint32_t, void *)>(game_addr(game::SSIGPU_InsertPrimAltViewport))(bucket, packet); }

#pragma pack(push, 1)
	struct RootNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t action;
		uint8_t started;
		uint32_t arena;
	};
	struct BoltNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t action;
		uint8_t *entity;
		int16_t pos[3];
		int16_t size;
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x14 && sizeof(BoltNode) == 0x1C, "Thunder nodes");

	static void FillDwords(void *dst, uint32_t value, uint32_t count)
	{
		uint32_t *d = (uint32_t *)dst;
		for (uint32_t k = 0; k < count; k++) d[k] = value;
	}

	static uint32_t ScreenFlashQuad(const void *colour, uint32_t cursor)
	{
		uint8_t *p = (uint8_t *)cursor;
		*(uint16_t *)(p + 8) = 0;
		*(uint16_t *)(p + 0x10) = 0;
		*(uint16_t *)(p + 0xE) = 0;
		*(uint16_t *)(p + 0xA) = 0;
		*(uint16_t *)(p + 0x14) = 0x140;
		*(uint16_t *)(p + 0x16) = 0xD8;
		*(uint16_t *)(p + 0x12) = 0xD8;
		*(uint16_t *)(p + 0xC) = 0x140;
		*(uint32_t *)p = 0x5000000;
		*(uint32_t *)(p + 4) = *(const uint32_t *)colour;
		p[7] = 0x2A;
		InsertPrimAltViewport(var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x20, p);
		return cursor + 0x18;
	}

	static void RotX(int16_t angle, Mat4x3 *m)
	{
		int32_t s = ComputeSin(angle);
		int32_t c = ComputeCos(angle);
		m->m[1][2] = (int16_t)-s;
		m->m[2][1] = (int16_t)s;
		m->m[0][0] = 0x1000;
		m->m[0][1] = 0;
		m->m[0][2] = 0;
		m->m[1][0] = 0;
		m->m[1][1] = (int16_t)c;
		m->m[2][0] = 0;
		m->m[2][2] = (int16_t)c;
	}
}
}

namespace ff8fx
{
namespace thunder003
{
	static uint32_t __cdecl RootTask(TaskNode *n)
	{
		RootNode *r = (RootNode *)n;
		if (r->arena)
		{
			PacketCursor() = TexBase();
			r->arena = 0;
		}
		else
		{
			PacketCursor() = TexBase() + 0x8000;
			r->arena = 1;
		}
		if (r->counter == 1)
		{
			r->started = 1;
			ActionData *acts = Ctx()->actions;
			if (r->action <= acts[0].last_action)
			{
				uint8_t *entity = Entity(acts[r->action].targets[0]);
				BoltNode *pool = (BoltNode *)game_addr(BOLT_POOL);
				bool busy = false;
				for (int k = 0; k < BOLT_POOL_COUNT; k++)
					if ((pool[k].hdr.flags & 1) && pool[k].entity == entity && pool[k].counter <= 0xF)
					{
						busy = true;
						break;
					}
				if (busy) r->counter = 0;
				else
				{
					BoltNode *b = (BoltNode *)AddTaskToQueue(&Queues().second, game_addr(ORIG_BoltTask));
					FillDwords(&b->counter, 0, 4);
					b->action = (int16_t)(uint16_t)r->action;
					b->entity = entity;
					GetEffectSpawnPosition(entity, 0xF1, 0, b->pos);
					b->size = (int16_t)((-0x1000 - (int32_t)*(int16_t *)(b->entity + 0x26)) >> 2);
					r->action++;
				}
			}
		}
		int alive = ExecuteTaskQueue(&Queues().second);
		if (r->started && !alive) return TASK_END;
		r->counter++;
		if (r->counter >= 5) r->counter = 0;
		return 0;
	}

	static void Layer(uint8_t *h, uint32_t seq, int32_t frame)
	{
		*(uint32_t *)h = seq;
		*(uint16_t *)(h + 4) = (uint16_t)frame;
		*(uint16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT44(), 2, PacketCursor());
	}

	static void BoltLayers(const BoltNode *p, int32_t f)
	{
		uint8_t *h = AllocHeader(0xB4);
		int16_t local[4] = { 0, 0, 0, 0 };
		if (f < 7)
		{
			TransformCameraByShadowRotation(p->pos, 0x1000, p->size);
			Layer(h, game_addr(SEQ_Strike), f);
		}
		if (f >= 2 && f < 9)
		{
			int32_t size = p->size;
			ff8fx::copy<(8)>(local, p->pos);
			local[1] = (int16_t)(local[1] + 0xC8);
			TransformCameraByShadowRotation(local, 0x1000, size);
			Layer(h, game_addr(SEQ_Upper), f - 2);
		}
		if (f >= 1 && f < 10)
		{
			TransformCameraByShadowRotation(p->pos, 0x1000, p->size);
			Layer(h, game_addr(SEQ_Core), f - 1);
		}
		if (f >= 3 && f < 12)
		{
			TransformCameraByShadowRotation(p->pos, 0x1000, p->size);
			Layer(h, game_addr(SEQ_Arc), f - 3);
		}
		if (f >= 6 && f < 15)
		{
			TransformCameraByShadowRotation(p->pos, 0x1000, p->size);
			Layer(h, game_addr(SEQ_Glow), f - 6);
		}
		if (f >= 1)
		{
			if (f < 10)
			{
				TransformCameraByShadowRotation(p->pos, 0x1000, p->size);
				Layer(h, game_addr(SEQ_Flash), f - 1);
			}
			if (f < 14)
			{
				Mat4x3 m = {};
				RotX(0x5A, &m);
				local[0] = p->pos[0];
				local[1] = *(int16_t *)(p->entity + 0x24);
				local[2] = p->pos[2];
				GteSetRotMatrixCtrl(&Camera());
				GteLoadV0(local);
				GteMVMVA_RotV0();
				GteReadMAC123(m.t);
				m.t[0] += Camera().t[0];
				m.t[1] += Camera().t[1];
				m.t[2] = m.t[2] + Camera().t[2] + 0x1F4;
				GteSetRotMatrix(&m);
				GteSetTransVector(&m);
				Layer(h, game_addr(SEQ_Ground), f - 1);
			}
		}
		if (f >= 5 && f < 0x18)
		{
			TransformCameraByShadowRotation(p->pos, 0x1000, p->size);
			Layer(h, game_addr(SEQ_Smoke), f - 5);
		}
		FieldFree(0xB4);
	}

	static void BoltDraw(const BoltNode *p)
	{
		if (p->counter == 0) PacketCursor() = ScreenFlashQuad(FLASH_Colour(), PacketCursor());
		if (p->counter >= 1) BoltLayers(p, p->counter - 1);
	}

	static uint32_t __cdecl BoltTask(TaskNode *n)
	{
		BoltNode *p = (BoltNode *)n;
		if (p->counter == 1) BdPlaySE3D(SOUND_Thunder(), 0x101, p->pos);
		BoltDraw(p);
		if (p->counter == 5) ApplyActionResultToTarget(Ctx()->actions[p->action].targets);
		p->counter++;
		return p->counter < 0x19 ? 0 : TASK_END;
	}
}

	void register_mag003_thunder()
	{
		register_port(thunder003::ORIG_RootTask, (void *)thunder003::RootTask, "T003 RootTask", 3);
		register_port(thunder003::ORIG_BoltTask, (void *)thunder003::BoltTask, "T003 BoltTask", 3);
	}
}

