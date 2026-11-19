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

// Effect 120: Petrify Stare

#include "mag_common.h"

namespace ff8fx
{
namespace petrify120
{
	using namespace eng;
	using namespace magc;

	inline uint32_t &FlashLevel() { return var<uint32_t>(game_addr(game::g_PetrifyStare_FlashLevel)); }
	inline uint8_t *&Caster() { return var<uint8_t *>(game_addr(game::g_PetrifyStare_CasterEntity)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::g_PetrifyStare_TexBase)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::g_PetrifyStare_CastContext)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::g_PetrifyStare_PacketCursor)); }
	inline TaskQueue *QTargets() { return (TaskQueue *)game_addr(game::g_PetrifyStare_TargetQueue); }
	inline uint32_t RenderOT(uint32_t off) { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + off; }

	static constexpr game::Id ORIG_RootTask = game::MAG_120_PetrifyStare_RootTask;
	static constexpr game::Id ORIG_TargetTask = game::MAG_120_PetrifyStare_TargetTask;
	static constexpr game::Id MODEL_Stare = game::unk_1277640;
	static constexpr game::Id MODEL_Stone = game::unk_1279C68;
	static const void *SOUND_Stare() { return (const void *)game_addr(game::unk_127DB4C); }

	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t flag, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, flag, out); }
	inline int32_t ComputeCos(int32_t a) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(a); }
	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void RotationFromAnglesC(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::MAG_117_RotationFromAnglesZYX))(angles, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
	inline uint32_t RenderPrimModel2(void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::MAG_070_RenderPrimModel))(h, ot, mode, cursor); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void GteMVMVA_RotV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0))(); }

#pragma pack(push, 1)
	struct RootNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t action;
		uint8_t started;
		uint32_t arena;
	};
	struct TargetNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t action;
		int16_t anchor[4];
		int16_t stone[4];
		uint8_t *entity;
		uint8_t stare[0x140];
		uint8_t rock[0x4F4];
	};
	struct PrimArg
	{
		int16_t pos[4];
		int32_t depth;
		uint32_t morph;
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x14 && sizeof(TargetNode) == 0x658, "Petrify Stare nodes");
	static_assert(sizeof(PrimArg) == 0x10, "Petrify Stare prim block");

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
		if (r->flags & 0x400) RotationFromAnglesC(r->rot, &m);
		else RotationFromAngles(r->rot, &m);
		int16_t off[4] = { r->pos[0], r->pos[1], r->pos[2], 0 };
		if (r->flags & 0x200)
		{
			GteSetRotMatrixCtrl(&Camera());
			GteLoadV0(arg->pos);
			GteMVMVA_RotV0();
			GteReadMAC123(m.t);
			m.t[0] = (int32_t)((uint32_t)m.t[0] + (uint32_t)(int32_t)off[0]);
			m.t[1] = (int32_t)((uint32_t)m.t[1] + (uint32_t)(int32_t)off[1]);
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
			const int32_t v[3] = { r->scale[0], r->scale[1], r->scale[2] };
			Scale3DMatrix(&m, v);
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
		PacketCursor() = RenderPrimModel2(h, RenderOT(0x44), 2, PacketCursor());
		FieldFree(0x58);
	}
}
}

namespace ff8fx
{
namespace petrify120
{
	static int32_t SizeDepth(const uint8_t *entity)
	{
		return (int32_t)(0xFFFFF800u - (uint32_t)(int32_t)*(const int16_t *)(entity + 0x26)) >> 5;
	}

	static uint32_t __cdecl TargetTask(TaskNode *n)
	{
		TargetNode *t = (TargetNode *)n;
		if (t->counter < 10) GetEffectSpawnPosition(Caster(), 0xF0, 0, t->anchor);
		PrimArg arg;
		arg.pos[0] = (int16_t)((-ComputeSin(*(const int16_t *)(Caster() + 0xE)) >> 4) + t->anchor[0]);
		arg.pos[2] = (int16_t)((-ComputeCos(*(const int16_t *)(Caster() + 0xE)) >> 4) + t->anchor[2]);
		arg.pos[1] = t->anchor[1];
		arg.pos[3] = 0;
		arg.depth = SizeDepth(Caster());
		arg.morph = TexBase() + 0x658;
		prim::play((prim::Layout *)t->stare, PartCallback, (int)&arg, 0);
		if (t->counter == 4) ScreenFadeTask(0, 1, 0, 0x80);
		if ((uint32_t)((int32_t)t->counter - 0xE) < 0x64)
		{
			if (t->counter == 0xE) GetDefaultEffectPosition(t->entity, t->stone);
			ff8fx::copy<(8)>(arg.pos, t->stone);
			arg.depth = SizeDepth(t->entity);
			arg.morph = TexBase() + 0x658;
			prim::play((prim::Layout *)t->rock, PartCallback, (int)&arg, 0);
		}
		if (t->counter == 0)
		{
			int16_t pos[4];
			GetDefaultEffectPosition(Caster(), pos);
			BdPlaySE3D(SOUND_Stare(), 0x100, pos);
		}
		if (t->counter == 0x2A) ApplyActionResultToTarget(Ctx()->actions[t->action].targets);
		if (t->counter > 0x30) return TASK_END;
		t->counter++;
		return 0;
	}

	static uint32_t __cdecl RootTask(TaskNode *n)
	{
		RootNode *r = (RootNode *)n;
		if (r->arena)
		{
			PacketCursor() = TexBase() + 0x660;
			r->arena = 0;
		}
		else
		{
			PacketCursor() = TexBase() + 0x18660;
			r->arena = 1;
		}
		FlashLevel() = 0;
		if (r->counter == 1)
		{
			if (!r->started)
			{
				r->started = 1;
				InitTaskQueuePool(QTargets(), (void *)TexBase(), 0x658, 1);
			}
			if (r->action <= Ctx()->actions[0].last_action)
			{
				uint8_t *entity = Entity(Ctx()->actions[r->action].targets[0]);
				TargetNode *t = (TargetNode *)AddTaskToQueue(QTargets(), game_addr(ORIG_TargetTask));
				if (t)
				{
					Memset32(&t->counter, 0, 0x193);
					t->action = (int16_t)(uint16_t)r->action;
					t->entity = entity;
					DecodeModelPrimLayout(game_addr(MODEL_Stare), t->stare, 0x140);
					DecodeModelPrimLayout(game_addr(MODEL_Stone), t->rock, 0x4F4);
					r->action++;
				}
			}
		}
		int a;
		if (r->started) a = ExecuteTaskQueue(QTargets());
		else a = (int)n;
		SetScreenFlash(FlashLevel(), 0);
		if (r->started && a == 0) return TASK_END;
		r->counter++;
		if (r->counter >= 10) r->counter = 0;
		return 0;
	}
}

	void register_mag120_petrify_stare()
	{
		register_port(petrify120::ORIG_RootTask, (void *)petrify120::RootTask, "P120 RootTask", 120);
		register_port(petrify120::ORIG_TargetTask, (void *)petrify120::TargetTask, "P120 TargetTask", 120);
	}
}

