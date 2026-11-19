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

// Effect 310: Holy War

#include "mag_common.h"

namespace ff8fx
{
namespace holywar310
{
	using namespace eng;
	using namespace magc;

	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::MAG_310_HOLYWAR_CastContext)); }
	inline TaskQueue *QPlay() { return (TaskQueue *)game_addr(game::MAG_310_HOLYWAR_TargetQueue); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_310_HOLYWAR_PacketCursor)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::MAG_310_HOLYWAR_MagicBuffer)); }

	static constexpr game::Id ORIG_RootTask = game::MAG_310_HOLYWAR_RootTask;
	static constexpr game::Id ORIG_PlayTask = game::MAG_310_HOLYWAR_TargetTask;
	static constexpr game::Id MODEL_MAIN = game::MAG_310_HOLYWAR_MainLayout;
	static constexpr game::Id MODEL_ORBIT = game::MAG_310_HOLYWAR_OrbitLayout;
	static constexpr game::Id MODEL_DEBRIS = game::MAG_310_HOLYWAR_ShardModel;
	static const uint32_t NODE_BYTES = 0x3D0;
	static const uint32_t MORPH = 0x16E0;
	static const void *SOUND() { return (const void *)game_addr(game::MAG_310_HOLYWAR_Sound); }
	static const uint32_t ARENA_A = 0x16E8;
	static const uint32_t ARENA_B = 0x116E8;
	static const int32_t MAIN_TICKS = 0x28, ORBIT_TICKS = 0x12, DEBRIS_FIRST = 0x10, DEBRIS_TICKS = 0x14;

	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
	inline uint32_t RenderPrimModel2(void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::MAG_070_RenderPrimModel))(h, ot, mode, cursor); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void MatrixMultiply3(Mat4x3 *a, const Mat4x3 *b) { fn<void (__cdecl *)(Mat4x3 *, const Mat4x3 *)>(game_addr(game::MatrixMultiply3x3))(a, b); }
	inline void GteMVMVA_RotV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0))(); }
	inline int32_t ComputeCos(int32_t angle) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(angle); }

#pragma pack(push, 1)
	struct RootNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t next;
		uint8_t started;
		uint32_t arena;
	};
	struct Shard
	{
		int16_t ang[3];
		int16_t pad06;
		int16_t spin[3];
		int16_t pad0E;
	};
	struct PlayNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t scale;
		uint8_t *entity;
		Shard shard[16];
		int16_t anchor[4];
		uint8_t main[0x13C];
		uint8_t orbit[2][0xBC];
	};
	struct PrimArg
	{
		int16_t pos[4];
		int32_t scale[3];
		int32_t scaled;
		int32_t depth;
		uint32_t morph;
	};
	struct ShardDraw
	{
		Mat4x3 m;
		uint8_t h[0x58];
		int32_t scale[3];
		uint8_t pad84[4];
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x14 && sizeof(PlayNode) == NODE_BYTES, "Holy War nodes");
	static_assert(sizeof(PrimArg) == 0x20 && sizeof(ShardDraw) == 0x88, "Holy War blocks");

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
		int16_t off[4] = {};
		const int32_t scaled = arg->scaled;
		if (scaled)
		{
			off[0] = (int16_t)(mul32(r->pos[0], arg->scale[0]) >> 12);
			off[1] = (int16_t)(mul32(r->pos[1], arg->scale[1]) >> 12);
			off[2] = (int16_t)(mul32(r->pos[2], arg->scale[2]) >> 12);
		}
		else
		{
			off[0] = r->pos[0];
			off[1] = r->pos[1];
			off[2] = r->pos[2];
		}
		if (scaled) Scale3DMatrix(&m, arg->scale);
		const uint32_t flags = r->flags;
		if (flags & 0x200)
		{
			GteSetRotMatrixCtrl(&Camera());
			GteLoadV0(arg->pos);
			GteMVMVA_RotV0();
			GteReadMAC123(m.t);
			m.t[1] = (int32_t)((uint32_t)m.t[1] + (uint32_t)(int32_t)off[1]);
			m.t[0] = (int32_t)((uint32_t)m.t[0] + (uint32_t)(int32_t)off[0]);
			m.t[2] = (int32_t)((uint32_t)m.t[2] + (uint32_t)(int32_t)off[2]);
		}
		else
		{
			off[0] = (int16_t)(off[0] + arg->pos[0]);
			off[1] = (int16_t)(off[1] + arg->pos[1]);
			off[2] = (int16_t)(off[2] + arg->pos[2]);
			GteSetRotMatrixCtrl(&Camera());
			GteLoadV0(off);
			GteMVMVA_RotV0();
			GteReadMAC123(m.t);
			MatrixMultiply(&Camera(), &m);
		}
		m.t[0] = (int32_t)((uint32_t)m.t[0] + (uint32_t)Camera().t[0]);
		m.t[1] = (int32_t)((uint32_t)m.t[1] + (uint32_t)Camera().t[1]);
		m.t[2] = (int32_t)((uint32_t)m.t[2] + (uint32_t)Camera().t[2]);
		if (!(*(const uint32_t *)r->scale == 0x10001000 && r->scale[2] == 0x1000))
		{
			if (flags & 0x100)
			{
				Mat4x3 d = {};
				d.m[0][0] = r->scale[0];
				d.m[1][1] = r->scale[1];
				d.m[2][2] = r->scale[2];
				MatrixMultiply3(&m, &d);
			}
			else
			{
				const int32_t v[3] = { r->scale[0], r->scale[1], r->scale[2] };
				Scale3DMatrix(&m, v);
			}
		}
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		*(uint32_t *)(h + 0x1C) = 0x2030;
		*(int32_t *)(h + 0xC) = r->a;
		if (r->a != 0)
		{
			*(uint32_t *)(h + 0x1C) = 0x20F0;
			*(uint32_t *)(h + 8) = *(const uint32_t *)r->rgb;
		}
		*(int32_t *)(h + 0x18) = arg->depth;
		PacketCursor() = RenderPrimModel2(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static void OrbitPos(const PlayNode *p, int32_t c, int j, int16_t out[3])
	{
		const uint32_t q = (uint32_t)shl32(c, 12) / 18u;
		const int32_t radius = mul32((int32_t)(0x1000 - q), p->scale) / 0x1000;
		const int32_t angle = ((int32_t)q >> 1) + 0x800 * j;
		out[0] = (int16_t)(mul32(ComputeSin(angle), radius) / 0x1000 + p->anchor[0]);
		out[1] = p->anchor[1];
		out[2] = (int16_t)(mul32(ComputeCos(angle), radius) / 0x1000 + p->anchor[2]);
	}

	static int32_t ShardFade(int32_t k)
	{
		const uint32_t t = (uint32_t)shl32(k, 12);
		if ((uint32_t)k <= 5) return (int32_t)(0x1000 - t / 5u);
		return (int32_t)((uint32_t)shl32(k - 5, 12) / 15u);
	}
}
}

namespace ff8fx
{
namespace holywar310
{
	// The game leaves a non-zero word where the play task reads its scale flag: the layouts are always scaled
	static inline int32_t PrimScaled() { return 1; }

	static uint32_t PlayTaskBody(PlayNode *p)
	{
		PrimArg arg;
		arg.pos[3] = 0;
		arg.scaled = PrimScaled();
		if ((uint32_t)(int32_t)p->counter < (uint32_t)ORBIT_TICKS)
		{
			for (int j = 0; j < 2; j++)
			{
				OrbitPos(p, p->counter, j, arg.pos);
				arg.scale[2] = p->scale;
				arg.scale[1] = p->scale;
				arg.scale[0] = p->scale;
				arg.depth = 0;
				arg.morph = TexBase() + MORPH;
				prim::play((prim::Layout *)p->orbit[j], PrimCallback, (int)&arg, 0);
			}
		}
		if ((uint32_t)(int32_t)p->counter < (uint32_t)MAIN_TICKS)
		{
			ff8fx::copy<(sizeof(arg.pos))>(arg.pos, p->anchor);
			arg.scale[2] = p->scale;
			arg.scale[1] = p->scale;
			arg.scale[0] = p->scale;
			arg.depth = -(p->scale / 8);
			arg.morph = TexBase() + MORPH;
			prim::play((prim::Layout *)p->main, PrimCallback, (int)&arg, 0);
		}
		const int32_t k = p->counter - DEBRIS_FIRST;
		if ((uint32_t)k < (uint32_t)DEBRIS_TICKS)
		{
			ShardDraw *d = (ShardDraw *)FieldAlloc(0x88);
			*(uint32_t *)(d->h + 0) = game_addr(MODEL_DEBRIS);
			*(uint32_t *)(d->h + 8) = 0;
			*(int32_t *)(d->h + 0xC) = ShardFade(k);
			*(uint32_t *)(d->h + 0x1C) = 0xF0;
			*(int32_t *)(d->h + 0x18) = -(p->scale / 8);
			for (int i = 0; i < 16; i++)
			{
				Shard &s = p->shard[i];
				BuildRotationMatrixFromAngles(s.ang, &d->m);
				d->scale[2] = p->scale;
				d->scale[1] = p->scale;
				d->scale[0] = p->scale;
				Scale3DMatrix(&d->m, d->scale);
				d->m.t[0] = p->anchor[0];
				d->m.t[1] = p->anchor[1];
				d->m.t[2] = p->anchor[2];
				ComposeAffineTransform(&Camera(), &d->m, &d->m);
				GteSetRotMatrix(&d->m);
				GteSetTransVector(&d->m);
				PacketCursor() = RenderPrimModel2(d->h, RenderOT(), 2, PacketCursor());
				s.ang[0] = (int16_t)(s.ang[0] + s.spin[0]);
				s.ang[1] = (int16_t)(s.ang[1] + s.spin[1]);
				s.ang[2] = (int16_t)(s.ang[2] + s.spin[2]);
			}
			FieldFree(0x88);
		}
		if (p->counter == 1) BdPlaySE(SOUND(), 0, 0x80);
		if (p->counter == 0x20)
		{
			const int32_t slot = (int32_t)((uint32_t)p->entity - game_addr(game::BattleEntitySlotData)) / 0x9C;
			for (int32_t i = 0; i < (int32_t)Ctx()->actions[0].target_count; i++)
			{
				uint8_t *t = Ctx()->actions[0].targets + i * TARGET_STRIDE;
				if ((int32_t)*t == slot) ApplyActionResultToTarget(t);
			}
		}
		p->counter++;
		return p->counter < MAIN_TICKS ? 0 : TASK_END;
	}
	static uint32_t __cdecl PlayTask(TaskNode *n)
	{
		return PlayTaskBody((PlayNode *)n);
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
				InitTaskQueuePool(QPlay(), (void *)TexBase(), NODE_BYTES, 6);
			}
		}
		if (r->counter == 1 && r->next < Ctx()->actions[0].target_count)
		{
			PlayNode *p = (PlayNode *)AddTaskToQueue(QPlay(), game_addr(ORIG_PlayTask));
			uint8_t *e = Entity(Ctx()->actions[0].targets[r->next * TARGET_STRIDE]);
			p->counter = 0;
			p->entity = e;
			p->scale = (int16_t)((*(const int16_t *)(e + 0x26) >> 1) + 0x400);
			GetDefaultEffectPosition(e, p->anchor);
			const int32_t h = *(const int16_t *)(e + 0x92);
			p->anchor[1] = (int16_t)(h / 8 + h / 2 + *(const int16_t *)(e + 0x24));
			DecodeModelPrimLayout(game_addr(MODEL_MAIN), p->main, 0x13C);
			DecodeModelPrimLayout(game_addr(MODEL_ORBIT), p->orbit[0], 0xBC);
			DecodeModelPrimLayout(game_addr(MODEL_ORBIT), p->orbit[1], 0xBC);
			for (int i = 0; i < 16; i++)
			{
				Shard &s = p->shard[i];
				s.ang[0] = 0;
				s.ang[1] = (int16_t)((CrtRand() & 0x7FF) - 0x400);
				s.spin[0] = 0;
				s.ang[2] = (int16_t)(i << 8);
				s.spin[1] = (int16_t)((CrtRand() & 0x3F) - 0x20);
				s.spin[2] = (int16_t)((CrtRand() & 0x3F) - 0x20);
			}
			r->next++;
		}
		int a;
		if (r->started) a = ExecuteTaskQueue(QPlay());
		else a = (int)n;
		if (r->started && a == 0) return TASK_END;
		r->counter++;
		if (r->counter > 4) r->counter = 0;
		return 0;
	}
}

	void register_mag310_holy_war()
	{
		register_port(holywar310::ORIG_RootTask, (void *)holywar310::RootTask, "310 RootTask", 310);
		register_port(holywar310::ORIG_PlayTask, (void *)holywar310::PlayTask, "310 PlayTask", 310);
	}
}

