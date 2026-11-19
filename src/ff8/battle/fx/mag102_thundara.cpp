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

// Effect 102: Thundara

#include "mag_common.h"

namespace ff8fx
{
namespace thundara102
{
	using namespace eng;
	using namespace magc;

	inline TaskQueuePair &Queues() { return var<TaskQueuePair>(game_addr(game::MAG102_THUNDARA_queueRootAndBolts)); }
	inline TaskQueuePair &Particles() { return var<TaskQueuePair>(game_addr(game::MAG102_THUNDARA_queueSparksAndFlashes)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::MAG102_THUNDARA_packetArenaBase)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::MAG102_THUNDARA_castCtx)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG102_THUNDARA_packetCursor)); }

	static constexpr game::Id ORIG_RootTask = game::MAG_102_THUNDARA_Tick;
	static constexpr game::Id ORIG_BoltTask = game::MAG_102_THUNDARA_BoltTask;
	static constexpr game::Id ORIG_SparkTask = game::MAG_102_Spark_Tick;
	static constexpr game::Id ORIG_FlashTask = game::MAG_102_FlashSprite_Tick;
	static constexpr game::Id BOLT_POOL = game::MAG102_THUNDARA_boltPool;
	static const int BOLT_POOL_COUNT = 3;
	static const void *SOUND_Thundara() { return (const void *)game_addr(game::MAG102_THUNDARA_boltSoundId); }
	static const void *FLASH_Colour() { return (const void *)game_addr(game::MAG102_THUNDARA_screenFlashParam); }
	static constexpr game::Id SEQ_Strike = game::MAG102_THUNDARA_boltLayer_core_f0;
	static constexpr game::Id SEQ_UpperA = game::MAG102_THUNDARA_boltLayer_high_f2;
	static constexpr game::Id SEQ_UpperB = game::MAG102_THUNDARA_boltLayer_mid_f2;
	static constexpr game::Id SEQ_Core = game::MAG102_THUNDARA_boltLayer_ground_f2;
	static constexpr game::Id SEQ_Arc = game::MAG102_THUNDARA_boltLayer_burst_f5;
	static constexpr game::Id SEQ_ArcLow = game::MAG102_THUNDARA_boltLayer_highBurst_f5;
	static constexpr game::Id SEQ_Glow = game::MAG102_THUNDARA_boltLayer_afterglow_f8;
	static constexpr game::Id SEQ_Smoke = game::MAG102_THUNDARA_boltLayer_fade_f11;
	static constexpr game::Id SEQ_Fade = game::MAG102_THUNDARA_boltLayer_pulsing_f9;
	static constexpr game::Id SEQ_Spark = game::MAG102_THUNDARA_sparkSeq;
	static constexpr game::Id SEQ_FlashSprite = game::MAG102_THUNDARA_flashSeq;
	static constexpr game::Id MODEL_TiltedGlow = game::MAG102_THUNDARA_tiltedGlowModel;
	static constexpr game::Id MODEL_Column = game::MAG102_THUNDARA_columnModel;
	static constexpr game::Id MODEL_FlashBall = game::MAG102_THUNDARA_flashBallModel;

	inline uint32_t RenderOT44() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44; }

	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t flag, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, flag, out); }
	inline int32_t ComputeCos(int32_t angle) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(angle); }
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
	struct SparkNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t delay;
		uint8_t anim;
		int16_t pos[3];
		int16_t pad16;
		int16_t vel[3];
		int16_t pad1E;
		int16_t acc[3];
		int16_t pad26;
		int16_t level;
		int16_t dlevel;
	};
	struct BoltNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t action;
		uint8_t *entity;
		int16_t pos[4];
		SparkNode *sparks[20];
	};
	struct FlashNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		int16_t pos[3];
		int16_t pad16;
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x14 && sizeof(SparkNode) == 0x2C && sizeof(BoltNode) == 0x6C && sizeof(FlashNode) == 0x18, "Thundara nodes");

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

	static int16_t DivNeg10(int16_t v)
	{
		int32_t x = v;
		int32_t hi = (int32_t)(((int64_t)x * (int32_t)0x99999999) >> 32);
		hi >>= 2;
		hi += (int32_t)((uint32_t)hi >> 31);
		return (int16_t)hi;
	}

	static inline int32_t EntitySizeOf(const uint8_t *entity) { return *(const int16_t *)(entity + 0x26); }
	static inline int32_t EntityHeightOf(const uint8_t *entity) { return *(const int16_t *)(entity + 0x24); }
}
}

namespace ff8fx
{
namespace thundara102
{
	static uint32_t __cdecl RootTask(TaskNode *n)
	{
		RootNode *r = (RootNode *)n;
		if (!r->started)
		{
			r->started = 1;
			return 0;
		}
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
		if (r->counter == 0)
		{
			ActionData *acts = Ctx()->actions;
			if (r->action <= acts[0].last_action)
			{
				uint8_t *entity = Entity(acts[r->action].targets[0]);
				BoltNode *pool = (BoltNode *)game_addr(BOLT_POOL);
				bool busy = false;
				for (int k = 0; k < BOLT_POOL_COUNT; k++)
					if ((pool[k].hdr.flags & 1) && pool[k].entity == entity && pool[k].counter <= 0x1A)
					{
						busy = true;
						break;
					}
				if (busy) r->counter = 0;
				else
				{
					BoltNode *b = (BoltNode *)AddTaskToQueue(&Queues().second, game_addr(ORIG_BoltTask));
					FillDwords(&b->counter, 0, 0x18);
					b->action = (int16_t)(uint16_t)r->action;
					b->entity = Entity(Ctx()->actions[b->action].targets[0]);
					GetEffectSpawnPosition(b->entity, 0xF1, 0, b->pos);
					r->action++;
				}
			}
		}
		int bolts = ExecuteTaskQueue(&Queues().second);
		int sparks = ExecuteTaskQueue(&Particles().first);
		int flashes = ExecuteTaskQueue(&Particles().second);
		if (!bolts && !sparks && !flashes) return TASK_END;
		r->counter++;
		if (r->counter >= 0xF) r->counter = 0;
		return 0;
	}

	static void BuildTiltMatrix(int16_t angle, Mat4x3 *m, int32_t scale)
	{
		int16_t s16 = (int16_t)scale;
		int32_t s = s16;
		int32_t sn = mul32(ComputeSin(angle), s) >> 12;
		int32_t cs = mul32(ComputeCos(angle), s);
		m->m[1][2] = (int16_t)-sn;
		m->m[2][1] = (int16_t)sn;
		m->m[0][0] = s16;
		cs >>= 12;
		m->m[0][1] = 0;
		m->m[0][2] = 0;
		m->m[1][0] = 0;
		m->m[1][1] = (int16_t)cs;
		m->m[2][0] = 0;
		m->m[2][2] = (int16_t)cs;
	}

	static void ModelGte(Mat4x3 *m, int32_t x, int32_t y, int32_t z)
	{
		m->t[0] = x;
		m->t[1] = y;
		m->t[2] = z;
		ComposeAffineTransform(&Camera(), m, m);
		GteSetRotMatrix(m);
		GteSetTransVector(m);
	}

	static uint32_t TiltedGlow(const int16_t *pos, int32_t tilt, int32_t frame, int32_t duration, uint32_t ot, int mode, uint32_t cursor)
	{
		int32_t t = shl32(frame, 12) / duration;
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		int32_t s = ComputeSin(t >> 2);
		Mat4x3 m = {};
		BuildTiltMatrix((int16_t)tilt, &m, s);
		ModelGte(&m, pos[0], pos[1], pos[2]);
		*(uint32_t *)h = game_addr(MODEL_TiltedGlow);
		h[0xA] = 0;
		h[9] = 0;
		h[8] = 0;
		if (t < 0x400) *(int32_t *)(h + 0xC) = 0;
		else
		{
			int32_t x = t - 0x400;
			int32_t q = (int32_t)(((int64_t)x * 0x55555556) >> 32);
			q += (int32_t)((uint32_t)q >> 31);
			*(int32_t *)(h + 0xC) = q + x;
		}
		*(uint32_t *)(h + 0x1C) = 0xF3;
		cursor = RenderPrimModel(h, ot, mode, cursor);
		FieldFree(0x58);
		return cursor;
	}

	static uint32_t Column(const int16_t *pos, int32_t height, int32_t frame, int32_t duration, uint32_t ot, int mode, uint32_t cursor)
	{
		int32_t t = shl32(frame, 10) / duration;
		int32_t s = ComputeSin(t);
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		int32_t w = (s >> 2) + 0x200;
		int32_t v = 2 * s + 0x100 - (s >> 3) - (s >> 2);
		Mat4x3 m = {};
		m.m[0][0] = (int16_t)w;
		m.m[2][2] = (int16_t)w;
		m.m[1][1] = (int16_t)v;
		ModelGte(&m, pos[0], height, pos[2]);
		*(uint32_t *)h = game_addr(MODEL_Column);
		h[0xA] = 0;
		h[9] = 0;
		h[8] = 0;
		*(int32_t *)(h + 0xC) = s;
		*(uint32_t *)(h + 0x1C) = 0xF3;
		cursor = RenderPrimModel(h, ot, mode, cursor);
		FieldFree(0x58);
		return cursor;
	}

	static uint32_t FlashBall(const int16_t *pos, int32_t height, int32_t frame, int32_t duration, uint32_t ot, int mode, uint32_t cursor)
	{
		int32_t t = shl32(frame, 10) / duration;
		int32_t s = ComputeSin(t);
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		Mat4x3 m = {};
		m.m[0][0] = (int16_t)s;
		m.m[1][1] = (int16_t)s;
		m.m[2][2] = (int16_t)s;
		ModelGte(&m, pos[0], height, pos[2]);
		*(uint32_t *)h = game_addr(MODEL_FlashBall);
		h[0xA] = 0;
		h[9] = 0;
		h[8] = 0;
		*(int32_t *)(h + 0xC) = s;
		*(uint32_t *)(h + 0x1C) = 0xF3;
		cursor = RenderPrimModel(h, ot, mode, cursor);
		FieldFree(0x58);
		return cursor;
	}

	static void Layer(uint8_t *h, uint32_t seq, int32_t frame)
	{
		*(uint32_t *)h = seq;
		*(uint16_t *)(h + 4) = (uint16_t)frame;
		*(uint16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT44(), 2, PacketCursor());
	}

	static void BoltLayers(const BoltNode *p, int32_t f, uint8_t *h)
	{
		if (f < 8)
		{
			TransformCameraByShadowRotation(p->pos, 0x1000, -0x400);
			Layer(h, game_addr(SEQ_Strike), f);
		}
		if (f >= 2)
		{
			if (f < 10)
			{
				int16_t low[4];
				ff8fx::copy<(8)>(low, p->pos);
				low[1] = (int16_t)(low[1] + 0xC8);
				TransformCameraByShadowRotation(low, 0x1000, -0x400);
				Layer(h, game_addr(SEQ_UpperA), f - 2);
				TransformCameraByShadowRotation(p->pos, 0x1000, -0x400);
				*(uint32_t *)h = game_addr(SEQ_UpperB);
				PacketCursor() = InitEffectSequenceFromData(h, RenderOT44(), 2, PacketCursor());
			}
			if (f < 11)
			{
				TransformCameraByShadowRotation(p->pos, 0x1000, -0x400);
				Layer(h, game_addr(SEQ_Core), f - 2);
			}
		}
		if (f >= 1 && f < 13) PacketCursor() = TiltedGlow(p->pos, 0x200, f - 1, 0xC, RenderOT44(), 2, PacketCursor());
		if (f >= 0 && f < 12) PacketCursor() = Column(p->pos, EntityHeightOf(p->entity), f, 0xC, RenderOT44(), 2, PacketCursor());
		if (f >= 5)
		{
			if (f < 14)
			{
				TransformCameraByShadowRotation(p->pos, 0x1000, -0x400);
				Layer(h, game_addr(SEQ_Arc), f - 5);
			}
			if (f < 13)
			{
				int16_t low[4];
				ff8fx::copy<(8)>(low, p->pos);
				low[1] = (int16_t)(low[1] + 0xC8);
				TransformCameraByShadowRotation(low, 0x1000, -0x400);
				Layer(h, game_addr(SEQ_ArcLow), f - 5);
			}
		}
		if (f >= 3 && f < 15) PacketCursor() = TiltedGlow(p->pos, -0x200, f - 3, 0xC, RenderOT44(), 2, PacketCursor());
		if (f >= 4 && f < 0x14) PacketCursor() = FlashBall(p->pos, EntityHeightOf(p->entity), f - 4, 0x10, RenderOT44(), 2, PacketCursor());
		if (f >= 8 && f < 0x11)
		{
			TransformCameraByShadowRotation(p->pos, 0x1000, -0x400);
			Layer(h, game_addr(SEQ_Glow), f - 8);
		}
		if (f >= 0xB && f < 0x14)
		{
			TransformCameraByShadowRotation(p->pos, 0x1000, -0x400);
			Layer(h, game_addr(SEQ_Smoke), f - 0xB);
		}
		if (f >= 9 && f < 0x13)
		{
			int32_t k = f - 9;
			TransformCameraByShadowRotation(p->pos, 0x1000, -0x400);
			*(uint16_t *)(h + 4) = (uint16_t)k;
			int32_t x = shl32(k, 10);
			int32_t q = (int32_t)(((int64_t)x * 0x66666667) >> 32) >> 2;
			q += (int32_t)((uint32_t)q >> 31);
			*(uint32_t *)h = game_addr(SEQ_Fade);
			uint8_t level = (uint8_t)(0x80 - (uint8_t)(ComputeSin(q) >> 7));
			*(uint16_t *)(h + 0x24) = 4;
			h[0x1E] = level;
			h[0x1D] = level;
			h[0x1C] = level;
			PacketCursor() = InitEffectSequenceFromData(h, RenderOT44(), 2, PacketCursor());
		}
	}

	static void SpawnFlashes(const BoltNode *p)
	{
		for (int k = 2; k; k--)
		{
			FlashNode *f = (FlashNode *)AddTaskToQueue(&Particles().second, game_addr(ORIG_FlashTask));
			if (!f) continue;
			FillDwords(&f->counter, 0, 3);
			int32_t radius = (EntitySizeOf(p->entity) >> 1) + 0x12C;
			int32_t a = CrtRand();
			f->pos[0] = (int16_t)((mul32(ComputeSin(a), radius) >> 12) + p->pos[0]);
			f->pos[2] = (int16_t)((mul32(ComputeCos(a), radius) >> 12) + p->pos[2]);
			int32_t r1 = CrtRand();
			int32_t r2 = CrtRand();
			f->pos[1] = (int16_t)((mul32(r2 + r1 - 0x8000, radius) >> 16) + p->pos[1]);
		}
	}

	static uint32_t __cdecl BoltTask(TaskNode *n)
	{
		BoltNode *p = (BoltNode *)n;
		if (p->counter == 4) BdPlaySE3D(SOUND_Thundara(), 0x101, p->pos);
		if (p->counter == 0)
		{
			for (int k = 0; k < 20; k++)
			{
				SparkNode *s = (SparkNode *)AddTaskToQueue(&Particles().first, game_addr(ORIG_SparkTask));
				if (!s) break;
				p->sparks[19 - k] = s;
				FillDwords(&s->counter, 0, 8);
				int32_t radius = (EntitySizeOf(p->entity) >> 1) + 0xC8;
				int32_t a = CrtRand();
				s->delay = (uint8_t)(CrtRand() & 7);
				s->pos[0] = (int16_t)((mul32(ComputeSin(a), radius) >> 12) + p->pos[0]);
				s->pos[2] = (int16_t)((mul32(ComputeCos(a), radius) >> 12) + p->pos[2]);
				s->pos[1] = (int16_t)(p->pos[1] + 0x1F4);
				int16_t vy = (int16_t)(-0x200 - (CrtRand() & 0x3FF));
				s->vel[1] = vy;
				s->acc[1] = (int16_t)(-(int32_t)vy >> 4);
				s->dlevel = (int16_t)((CrtRand() & 0xF) + 0x18);
				s->anim = (uint8_t)CrtRand();
			}
		}
		if (p->counter == 0xC)
		{
			for (int k = 19; k >= 0; k--)
			{
				SparkNode *s = p->sparks[k];
				s->vel[1] = 0;
				s->vel[0] = (int16_t)(uint16_t)((uint32_t)(uint16_t)(s->pos[0] - p->pos[0]) << 4);
				s->vel[2] = (int16_t)(uint16_t)((uint32_t)(uint16_t)(s->pos[2] - p->pos[2]) << 4);
				s->acc[0] = DivNeg10(s->vel[0]);
				s->acc[2] = DivNeg10(s->vel[2]);
				s->dlevel = (int16_t)(-10 - (CrtRand() & 7));
			}
		}
		if (p->counter == 0xC) PacketCursor() = ScreenFlashQuad(FLASH_Colour(), PacketCursor());
		if (p->counter > 0xA)
		{
			int32_t f = p->counter - 0xB;
			uint8_t *h = AllocHeader(0xB4);
			BoltLayers(p, f, h);
			if (f == 0xC) PacketCursor() = ScreenFlashQuad(FLASH_Colour(), PacketCursor());
			if (f >= 4 && f < 0x14 && !(f & 1)) SpawnFlashes(p);
			FieldFree(0xB4);
		}
		if (p->counter == 0x1A) ApplyActionResultToTarget(Ctx()->actions[p->action].targets);
		if (p->counter >= 0x1F) return TASK_END;
		p->counter++;
		return 0;
	}

	static void SparkDraw(const SparkNode *p)
	{
		uint8_t *h = AllocHeader(0xB4);
		TransformCameraByShadowRotation(p->pos, 0x1000, -0x400);
		*(uint32_t *)h = game_addr(SEQ_Spark);
		*(uint16_t *)(h + 4) = (uint16_t)(p->anim % 20);
		uint8_t level = (uint8_t)p->level;
		h[0x1E] = level;
		h[0x1D] = level;
		h[0x1C] = level;
		*(uint16_t *)(h + 0x24) = 4;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT44(), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static void SparkUpdate(SparkNode *p)
	{
		uint8_t anim = p->anim;
		p->vel[0] = (int16_t)(p->vel[0] + p->acc[0]);
		p->vel[1] = (int16_t)(p->vel[1] + p->acc[1]);
		p->vel[2] = (int16_t)(p->vel[2] + p->acc[2]);
		p->anim = (uint8_t)(anim + 1);
		p->pos[0] = (int16_t)(p->pos[0] + (int16_t)(p->vel[0] >> 4));
		p->level = (int16_t)(p->level + p->dlevel);
		int16_t level = p->level;
		p->pos[1] = (int16_t)(p->pos[1] + (int16_t)(p->vel[1] >> 4));
		p->pos[2] = (int16_t)(p->pos[2] + (int16_t)(p->vel[2] >> 4));
		if (level < 0) p->level = 0;
		else if (level > 0x80) p->level = 0x80;
	}

	static uint32_t __cdecl SparkTask(TaskNode *n)
	{
		SparkNode *p = (SparkNode *)n;
		if (p->counter >= (int16_t)(uint16_t)p->delay)
		{
			SparkDraw(p);
			SparkUpdate(p);
		}
		p->counter++;
		return p->counter < 0x20 ? 0 : TASK_END;
	}

	static void FlashDraw(const FlashNode *p)
	{
		uint8_t *h = AllocHeader(0xB4);
		TransformCameraByShadowRotation(p->pos, 0xC00, -0x300);
		*(uint32_t *)h = game_addr(SEQ_FlashSprite);
		*(uint16_t *)(h + 0x24) = 0;
		*(uint16_t *)(h + 4) = (uint16_t)(*(const uint8_t *)&p->counter & 7);
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT44(), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static uint32_t __cdecl FlashTask(TaskNode *n)
	{
		FlashNode *p = (FlashNode *)n;
		FlashDraw(p);
		p->counter++;
		return p->counter < 8 ? 0 : TASK_END;
	}
}

	void register_mag102_thundara()
	{
		register_port(thundara102::ORIG_RootTask, (void *)thundara102::RootTask, "T102 RootTask", 102);
		register_port(thundara102::ORIG_BoltTask, (void *)thundara102::BoltTask, "T102 BoltTask", 102);
		register_port(thundara102::ORIG_SparkTask, (void *)thundara102::SparkTask, "T102 SparkTask", 102);
		register_port(thundara102::ORIG_FlashTask, (void *)thundara102::FlashTask, "T102 FlashTask", 102);
	}
}

