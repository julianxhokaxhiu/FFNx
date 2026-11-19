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

// Effect 273: Mad Rush

#include "mag_common.h"

namespace ff8fx
{
namespace madrush273
{
	using namespace eng;
	using namespace magc;

	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::MAG_273_MAD_RUSH_CastContext)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::MAG_273_MAD_RUSH_MagicBuffer)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_273_MAD_RUSH_PacketCursor)); }
	inline TaskQueue *QDust() { return (TaskQueue *)game_addr(game::MAG_273_MAD_RUSH_DustQueue); }
	inline TaskQueue *QPlay() { return (TaskQueue *)game_addr(game::MAG_273_MAD_RUSH_PlayQueue); }
	inline uint32_t RenderOT44() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44; }

	static constexpr game::Id ORIG_RootTask = game::MAG_273_MAD_RUSH_RootTask;
	static constexpr game::Id ORIG_PlayTask = game::MAG_273_MAD_RUSH_PlayTask;
	static constexpr game::Id ORIG_DustTask = game::MAG_273_MAD_RUSH_DustTask;
	static constexpr game::Id MODEL_LAYOUT = game::MAG_273_MAD_RUSH_Layout;
	static const uint32_t LAYOUT_BYTES = 0x2F4;
	static const uint32_t PLAY_BYTES = 0x310;
	static const uint32_t DUST_POOL = 0x930;
	static const uint32_t MORPH = 0xF30;
	static const uint32_t ARENA_A = 0x1250;
	static const uint32_t ARENA_B = 0x11250;
	static constexpr game::Id SEQ_DUST = game::MAG_273_MAD_RUSH_DustSequence;
	static const int DUST_FRAMES = 0xD;
	static const void *SOUND() { return (const void *)game_addr(game::MAG_273_MAD_RUSH_Sound); }

	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t flag, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, flag, out); }

#pragma pack(push, 1)
	struct RootNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t pad0E;
		uint8_t started;
		uint32_t arena;
	};
	struct PlayNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t index;
		uint8_t *entity;
		int16_t anchor[4];
		uint8_t layout[LAYOUT_BYTES];
	};
	struct DustNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t scale;
		int16_t pos[3];
		int16_t vy;
	};
	struct PrimArg
	{
		int16_t pos[4];
		uint32_t morph;
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x14 && sizeof(PlayNode) == PLAY_BYTES && sizeof(DustNode) == 0x18, "Mad Rush nodes");
	static_assert(sizeof(PrimArg) == 0xC, "Mad Rush prim block");

	static void __cdecl PrimCallback(prim::Layout *l, prim::Record *r, int arg_)
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
		m.t[0] = (int32_t)r->pos[0] + arg->pos[0];
		m.t[1] = (int32_t)r->pos[1] + arg->pos[1];
		m.t[2] = (int32_t)r->pos[2] + arg->pos[2];
		ComposeAffineTransform(&Camera(), &m, &m);
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
		PacketCursor() = RenderPrimModel(h, RenderOT44(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static void DustDraw(const DustNode *d)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		TransformCameraByShadowRotation(d->pos, d->scale, -((int32_t)d->scale >> 2));
		*(uint32_t *)h = game_addr(SEQ_DUST);
		*(int16_t *)(h + 4) = d->counter;
		*(uint16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT44(), 2, PacketCursor());
		FieldFree(0xB4);
	}
}
}

namespace ff8fx
{
namespace madrush273
{
	static uint32_t __cdecl DustTask(TaskNode *n)
	{
		DustNode *d = (DustNode *)n;
		DustDraw(d);
		d->counter++;
		if (d->counter >= DUST_FRAMES) return TASK_END;
		d->pos[1] = (int16_t)(d->pos[1] + d->vy);
		return 0;
	}

	static uint32_t __cdecl PlayTask(TaskNode *n)
	{
		PlayNode *p = (PlayNode *)n;
		if (p->counter < 0)
		{
			p->counter++;
			return 0;
		}
		PrimArg arg;
		ff8fx::copy<(sizeof(arg.pos))>(arg.pos, p->anchor);
		arg.morph = TexBase() + MORPH;
		const int left = prim::play((prim::Layout *)p->layout, PrimCallback, (int)&arg, 0);
		const int32_t c = p->counter - 0x14;
		if ((uint32_t)c < 10u)
		{
			const int32_t s = ComputeSin((int32_t)((uint32_t)shl32(c, 11) / 10u)) >> 11;
			for (int32_t k = s + 1; k; k--)
			{
				DustNode *d = (DustNode *)AddTaskToQueue(QDust(), game_addr(ORIG_DustTask));
				if (!d) continue;
				d->counter = 0;
				d->scale = (int16_t)((CrtRand() & 0x400) + 0x400);
				d->pos[0] = (int16_t)((CrtRand() & 0x1FF) + p->anchor[0] - 0x100);
				d->pos[2] = (int16_t)((CrtRand() & 0x1FF) + p->anchor[2] - 0x100);
				d->pos[1] = (int16_t)(p->anchor[1] - (CrtRand() & 0x3FF));
				d->vy = (int16_t)(-0x10 - (CrtRand() & 0x3F));
			}
		}
		if (left == 0)
		{
			ApplyActionResultToTarget(Ctx()->actions[0].targets + TARGET_STRIDE * p->index);
			return TASK_END;
		}
		p->counter++;
		return 0;
	}

	static uint32_t __cdecl RootTask(TaskNode *n)
	{
		RootNode *r = (RootNode *)n;
		if (r->arena)
		{
			PacketCursor() = TexBase() + ARENA_A;
			r->arena = 0;
		}
		else
		{
			PacketCursor() = TexBase() + ARENA_B;
			r->arena = 1;
		}
		if (r->counter == 1)
		{
			if (!r->started)
			{
				r->started = 1;
				InitTaskQueuePool(QPlay(), (void *)TexBase(), PLAY_BYTES, 3);
				InitTaskQueuePool(QDust(), (void *)(TexBase() + DUST_POOL), 0x18, 0x40);
			}
			for (int32_t k = 0; k < (int32_t)Ctx()->actions[0].target_count; k++)
			{
				PlayNode *p = (PlayNode *)AddTaskToQueue(QPlay(), game_addr(ORIG_PlayTask));
				if (!p) continue;
				Memset32(&p->counter, 0, 0xC1);
				p->index = (int16_t)k;
				p->counter = (int16_t)(-k * 2);
				p->entity = Entity(Ctx()->actions[0].targets[TARGET_STRIDE * k]);
				GetEffectSpawnPosition(p->entity, 0xF1, 0, p->anchor);
				p->anchor[1] = *(const int16_t *)(p->entity + 0x24);
				DecodeModelPrimLayout(game_addr(MODEL_LAYOUT), p->layout, LAYOUT_BYTES);
				BdPlaySE(SOUND(), 0, 0x80);
			}
		}
		int a, b;
		if (r->started)
		{
			a = ExecuteTaskQueue(QPlay());
			b = ExecuteTaskQueue(QDust());
		}
		else a = b = (int)n;
		if (r->started && a == 0 && b == 0) return TASK_END;
		r->counter++;
		return 0;
	}
}

	void register_mag273_mad_rush()
	{
		register_port(madrush273::ORIG_RootTask, (void *)madrush273::RootTask, "273 RootTask", 273);
		register_port(madrush273::ORIG_PlayTask, (void *)madrush273::PlayTask, "273 PlayTask", 273);
		register_port(madrush273::ORIG_DustTask, (void *)madrush273::DustTask, "273 DustTask", 273);
	}
}

