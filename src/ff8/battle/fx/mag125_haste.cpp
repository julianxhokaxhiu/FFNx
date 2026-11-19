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

// Effect 125: Haste

#include "mag_common.h"

namespace ff8fx
{
namespace haste125
{
	using namespace eng;
	using namespace magc;

	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::unk_252136C)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::unk_2521370)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::g_Haste_PacketCursor)); }
	inline Mat4x3 *EffCam() { return (Mat4x3 *)game_addr(game::g_Haste_EffectCameraMatrix); }
	inline TaskQueue *QTargets() { return (TaskQueue *)game_addr(game::unk_25213B0); }
	inline uint32_t RenderOT(uint32_t off) { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + off; }

	static constexpr game::Id ORIG_RootTask = game::MAG_125_HASTE_RootTask;
	static constexpr game::Id ORIG_TargetTask = game::MAG_125_HASTE_TargetTask;
	static constexpr game::Id MODEL_Clock = game::unk_1248A0C;
	static constexpr game::Id MODEL_Lower = game::unk_124BD98;
	static const void *SOUND_Haste() { return (const void *)game_addr(game::unk_1252BD4); }

	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t flag, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, flag, out); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void RotationY(int16_t angle, Mat4x3 *out) { fn<void (__cdecl *)(int32_t, Mat4x3 *)>(game_addr(game::Effect_RotationYMatrix))((int32_t)(uint16_t)angle, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void MatrixMultiply3x3(Mat4x3 *a, const Mat4x3 *b) { fn<void (__cdecl *)(Mat4x3 *, const Mat4x3 *)>(game_addr(game::MatrixMultiply3x3))(a, b); }
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
		uint8_t *entity;
		int16_t pos[4];
		int16_t spawn[4];
		uint8_t clock[0x254];
		uint8_t lower[0x558];
	};
	struct PrimArg
	{
		Mat4x3 m;
		int32_t scale[3];
		int32_t scaled;
		uint32_t morph;
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x14 && sizeof(TargetNode) == 0x7D0, "Haste nodes");
	static_assert(sizeof(PrimArg) == 0x34, "Haste prim block");

	static void __cdecl ClockPart(prim::Layout *l, prim::Record *r, int arg_)
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
		if (r->flags & 0x1000)
		{
			GteSetRotMatrixCtrl(EffCam());
			GteLoadV0(off);
			GteMVMVA_RotV0();
			GteReadMAC123(m.t);
			if (!(r->flags & 0x8000)) MatrixMultiply(EffCam(), &m);
		}
		else if (r->flags & 0x200)
		{
			m.t[0] = off[0];
			m.t[1] = off[1];
			m.t[2] = off[2];
		}
		else
		{
			GteSetRotMatrixCtrl(&arg->m);
			GteLoadV0(off);
			GteMVMVA_RotV0();
			GteReadMAC123(m.t);
			if (!(r->flags & 0x8000)) MatrixMultiply(&arg->m, &m);
		}
		m.t[0] = (int32_t)((uint32_t)m.t[0] + (uint32_t)arg->m.t[0]);
		m.t[1] = (int32_t)((uint32_t)m.t[1] + (uint32_t)arg->m.t[1]);
		m.t[2] = (int32_t)((uint32_t)m.t[2] + (uint32_t)arg->m.t[2]);
		if (!(*(const uint32_t *)r->scale == 0x10001000 && r->scale[2] == 0x1000))
		{
			if (r->flags & 0x100)
			{
				Mat4x3 s = {};
				s.m[0][0] = r->scale[0];
				s.m[1][1] = r->scale[1];
				s.m[2][2] = r->scale[2];
				MatrixMultiply3x3(&m, &s);
			}
			else
			{
				int32_t v[3] = { r->scale[0], r->scale[1], r->scale[2] };
				Scale3DMatrix(&m, v);
			}
		}
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		*(uint32_t *)(h + 0x1C) = (r->flags & 0x4000) ? 0x2000 : 0x2030;
		if (r->flags & 0x2000) *(uint32_t *)(h + 0x1C) |= 0xC;
		*(int32_t *)(h + 0xC) = r->a;
		if (r->a != 0)
		{
			*(uint32_t *)(h + 0x1C) |= 0xC0;
			*(uint32_t *)(h + 8) = *(const uint32_t *)r->rgb;
		}
		PacketCursor() = RenderPrimModel(h, RenderOT(0x44), 2, PacketCursor());
		FieldFree(0x58);
	}
}
}

namespace ff8fx
{
namespace haste125
{
	static uint32_t __cdecl TargetTask(TaskNode *n)
	{
		TargetNode *t = (TargetNode *)n;
		if (t->counter == 6) BdPlaySE3D(SOUND_Haste(), 1, t->pos);
		PrimArg arg;
		arg.morph = TexBase() + 0x1770;
		const int32_t s = (*(const int16_t *)(t->entity + 0x26) >> 1) + 0x800;
		arg.scale[2] = s;
		arg.scale[1] = s;
		arg.scale[0] = s;
		arg.scaled = 1;
		RotationY((int16_t)-*(const int16_t *)(t->entity + 0xE), &arg.m);
		arg.m.t[0] = t->pos[0];
		arg.m.t[2] = t->pos[2];
		arg.m.t[1] = *(const int16_t *)(t->entity + 0x24);
		ComposeAffineTransform(&Camera(), &arg.m, &arg.m);
		int32_t left = prim::play((prim::Layout *)t->clock, ClockPart, (int)&arg, 0);
		RotationY((int16_t)-*(const int16_t *)(t->entity + 0xE), &arg.m);
		arg.m.t[0] = t->pos[0];
		arg.m.t[2] = t->pos[2];
		arg.m.t[1] = (int32_t)t->spawn[1] - (s >> 4) - (s >> 2);
		ComposeAffineTransform(&Camera(), &arg.m, &arg.m);
		left |= prim::play((prim::Layout *)t->lower, ClockPart, (int)&arg, 0);
		if (t->counter == 0x30) ScreenFadeTask(0, 1, 1, 0x60);
		if (left == 0)
		{
			ApplyActionResultToTarget(Ctx()->actions[t->action].targets);
			return TASK_END;
		}
		t->counter++;
		return 0;
	}

	static uint32_t __cdecl RootTask(TaskNode *n)
	{
		RootNode *r = (RootNode *)n;
		if (r->arena)
		{
			PacketCursor() = TexBase() + 0x1A70;
			r->arena = 0;
		}
		else
		{
			PacketCursor() = TexBase() + 0x19A70;
			r->arena = 1;
		}
		if (r->counter == 1)
		{
			if (!r->started)
			{
				r->started = 1;
				InitTaskQueuePool(QTargets(), (void *)TexBase(), 0x7D0, 3);
			}
			if (r->action <= Ctx()->actions[0].last_action)
			{
				TargetNode *t = (TargetNode *)AddTaskToQueue(QTargets(), game_addr(ORIG_TargetTask));
				Memset32(&t->counter, 0, 0x1F1);
				t->action = (int16_t)(uint16_t)r->action;
				t->entity = Entity(Ctx()->actions[t->action].targets[0]);
				GetDefaultEffectPosition(t->entity, t->pos);
				GetEffectSpawnPosition(t->entity, 0xF0, 0, t->spawn);
				DecodeModelPrimLayout(game_addr(MODEL_Clock), t->clock, 0x254);
				DecodeModelPrimLayout(game_addr(MODEL_Lower), t->lower, 0x558);
				r->action++;
			}
		}
		EffectCameraMatrix(&Camera(), EffCam());
		const int a = r->started ? ExecuteTaskQueue(QTargets()) : (int)n;
		if (r->started && a == 0) return TASK_END;
		r->counter++;
		if (r->counter >= 10) r->counter = 0;
		return 0;
	}
}

	void register_mag125_haste()
	{
		register_port(haste125::ORIG_RootTask, (void *)haste125::RootTask, "H125 RootTask", 125);
		register_port(haste125::ORIG_TargetTask, (void *)haste125::TargetTask, "H125 TargetTask", 125);
	}
}

