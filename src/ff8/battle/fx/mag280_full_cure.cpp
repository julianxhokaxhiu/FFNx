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

// Effect 280: Full-cure

#include "mag_common.h"

namespace ff8fx
{
namespace fullcure280
{
	using namespace eng;
	using namespace magc;

	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::unk_2507FDC)); }
	inline TaskQueue *QSparkle() { return (TaskQueue *)game_addr(game::unk_2507FE8); }
	inline TaskQueue *QTarget() { return (TaskQueue *)game_addr(game::unk_2507FF8); }
	inline TaskQueue *QControl() { return (TaskQueue *)game_addr(game::unk_2508008); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::unk_250803C)); }
	inline int32_t &FlashLevel() { return var<int32_t>(game_addr(game::unk_2508044)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::unk_2508048)); }
	inline Mat4x3 &ModuleMatrix() { return var<Mat4x3>(game_addr(game::unk_2508050)); }

	static constexpr game::Id ORIG_RootTask = game::MAG_280_FULL_CURE_RootTask;
	static constexpr game::Id ORIG_ControlTask = game::MAG_280_FULL_CURE_ControlTask;
	static constexpr game::Id ORIG_TargetTask = game::MAG_280_FULL_CURE_TargetTask;
	static constexpr game::Id ORIG_SparkleTask = game::MAG_280_FULL_CURE_SparkleTask;
	static constexpr game::Id LAYOUT = game::unk_10A7A64;
	static constexpr game::Id SEQUENCE = game::unk_10A7974;
	static const void *SOUND() { return (const void *)game_addr(game::unk_10AB124); }
	static const uint32_t ARENA_A = 0x1DF0;
	static const uint32_t ARENA_B = 0x11DF0;
	static const uint32_t MORPH = 0x1BF0;
	static const int32_t CONTROL_TICKS = 0x41, TARGET_TICKS = 0x1E, SPARKLE_FIRST = 0x13, SPARKLE_FRAMES = 8;
	static const int32_t EFFECT_POINT = 0xF1;

	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t flag, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, flag, out); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void RotationFromAnglesB(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::MAG_063_RotationFromAnglesYXZ))(angles, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
	inline uint32_t RenderPrimModel2(void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::MAG_070_RenderPrimModel))(h, ot, mode, cursor); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void MatrixMultiply3(Mat4x3 *a, const Mat4x3 *b) { fn<void (__cdecl *)(Mat4x3 *, const Mat4x3 *)>(game_addr(game::MatrixMultiply3x3))(a, b); }
	inline void GteMVMVA_RotV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0))(); }

#pragma pack(push, 1)
	struct RootNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t pad0E;
		uint8_t started;
		uint32_t arena;
	};
	struct ControlNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t count;
		uint8_t *ent[3];
		uint8_t *&Ent(uint32_t i) { return ((uint8_t **)((uint8_t *)this + 0x10))[i]; }
	};
	struct TargetNode
	{
		TaskNode hdr;
		uint8_t *entity;
		int16_t anchor[3];
		int16_t counter;
		uint8_t layout[0x8C4];
	};
	struct SparkleNode
	{
		TaskNode hdr;
		int16_t pos[3];
		int16_t frame;
	};
	struct PrimArg
	{
		int16_t pos[3];
		int16_t counter;
		int32_t depth;
		uint32_t morph;
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x14 && sizeof(ControlNode) == 0x1C && sizeof(TargetNode) == 0x8DC && sizeof(SparkleNode) == 0x14, "Full-cure nodes");
	static_assert(sizeof(PrimArg) == 0x10, "Full-cure block");

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
		if (r->flags & 0x40000) RotationFromAnglesB(r->rot, &m);
		else RotationFromAngles(r->rot, &m);
		const uint32_t flags = r->flags;
		int16_t off[4] = { r->pos[0], r->pos[1], r->pos[2], 0 };
		if (flags & 0x1000)
		{
			GteSetRotMatrixCtrl(&Camera());
			GteLoadV0(arg->pos);
			GteMVMVA_RotV0();
			GteReadMAC123(m.t);
			GteSetRotMatrixCtrl(&ModuleMatrix());
			GteLoadV0(off);
			GteMVMVA_RotV0();
			int32_t v[3];
			GteReadMAC123(v);
			m.t[0] = (int32_t)((uint32_t)m.t[0] + (uint32_t)v[0]);
			m.t[1] = (int32_t)((uint32_t)m.t[1] + (uint32_t)v[1]);
			m.t[2] = (int32_t)((uint32_t)m.t[2] + (uint32_t)v[2]);
			MatrixMultiply(&ModuleMatrix(), &m);
		}
		else if (flags & 0x200)
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

	static int32_t ControlFlash(int16_t c)
	{
		if (c < 8) return (int32_t)c * 0x800 >> 3;
		if (c > 0x39) return (0x41 - (int32_t)c) * 0x800 >> 3;
		return 0x800;
	}
}
}

namespace ff8fx
{
namespace fullcure280
{
	static void ApplyResult(uint8_t *e)
	{
		const int32_t slot = (int32_t)((uint32_t)e - game_addr(game::BattleEntitySlotData)) / 0x9C;
		for (int32_t i = 0; i < (int32_t)Ctx()->actions[0].target_count; i++)
		{
			uint8_t *t = Ctx()->actions[0].targets + i * TARGET_STRIDE;
			if ((int32_t)*t == slot) ApplyActionResultToTarget(t);
		}
	}

	static uint32_t __cdecl SparkleTask(TaskNode *n)
	{
		SparkleNode *s = (SparkleNode *)n;
		TransformCameraByShadowRotation(s->pos, 0x800, -0x200);
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		*(uint32_t *)h = game_addr(SEQUENCE);
		*(int16_t *)(h + 4) = s->frame;
		*(int16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
		FieldFree(0xB4);
		s->frame++;
		return s->frame < SPARKLE_FRAMES ? 0 : TASK_END;
	}

	static uint32_t __cdecl TargetTask(TaskNode *n)
	{
		TargetNode *t = (TargetNode *)n;
		PrimArg arg;
		ff8fx::copy<(6)>(arg.pos, t->anchor);
		arg.counter = t->counter;
		arg.depth = -0x80;
		arg.morph = TexBase() + MORPH;
		prim::play((prim::Layout *)t->layout, PrimCallback, (int)&arg, 0);
		if (t->counter >= SPARKLE_FIRST && !(t->counter & 1))
		{
			SparkleNode *s = (SparkleNode *)AddTaskToQueue(QSparkle(), game_addr(ORIG_SparkleTask));
			if (s)
			{
				uint8_t *e = t->entity;
				const int32_t bones = **(const uint8_t *const *)*(const uint32_t *)(e + 0x64);
				GetEffectSpawnPosition(e, (int32_t)t->counter % bones, 0, s->pos);
				s->frame = 0;
			}
		}
		t->counter++;
		if (t->counter >= TARGET_TICKS)
		{
			ApplyResult(t->entity);
			return TASK_END;
		}
		return 0;
	}

	static uint32_t __cdecl ControlTask(TaskNode *n)
	{
		ControlNode *c = (ControlNode *)n;
		FlashLevel() = ControlFlash(c->counter);
		const uint32_t k = (uint32_t)(int32_t)c->counter;
		if (k < 0x3F && k % 20 == 0 && k / 20 < (uint32_t)(int32_t)c->count)
		{
			TargetNode *t = (TargetNode *)AddTaskToQueue(QTarget(), game_addr(ORIG_TargetTask));
			if (t)
			{
				uint8_t *e = c->Ent(k / 20);
				t->entity = e;
				GetEffectSpawnPosition(e, EFFECT_POINT, 0, t->anchor);
				t->counter = 0;
				DecodeModelPrimLayout(game_addr(LAYOUT), t->layout, 0x8C4);
				BdPlaySE(SOUND(), 0, 0x80);
			}
		}
		c->counter++;
		if (c->counter >= CONTROL_TICKS)
		{
			FlashLevel() = 0;
			return TASK_END;
		}
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
		FlashLevel() = 0;
		if (r->counter == 1 && !r->started)
		{
			r->started = 1;
			InitTaskQueuePool(QControl(), (void *)(TexBase() + 0x1BD4), 0x1C, 1);
			InitTaskQueuePool(QTarget(), (void *)(TexBase() + 0x140), 0x8DC, 3);
			InitTaskQueuePool(QSparkle(), (void *)TexBase(), 0x14, 0x10);
			ControlNode *c = (ControlNode *)AddTaskToQueue(QControl(), game_addr(ORIG_ControlTask));
			Memset32((uint8_t *)c + 0xC, 0, 4);
			c->count = (int16_t)Ctx()->actions[0].target_count;
			for (int32_t i = 0; i < c->count; i++) c->Ent((uint32_t)i) = Entity(Ctx()->actions[0].targets[i * TARGET_STRIDE]);
		}
		int a, b;
		if (r->started)
		{
			a = ExecuteTaskQueue(QControl());
			ExecuteTaskQueue(QTarget());
			b = ExecuteTaskQueue(QSparkle());
		}
		else a = b = (int)n;
		SetScreenFlash((uint32_t)FlashLevel(), 0);
		if (r->started && a == 0 && b == 0) return TASK_END;
		r->counter++;
		return 0;
	}
}

	void register_mag280_full_cure()
	{
		register_port(fullcure280::ORIG_RootTask, (void *)fullcure280::RootTask, "280 RootTask", 280);
		register_port(fullcure280::ORIG_ControlTask, (void *)fullcure280::ControlTask, "280 ControlTask", 280);
		register_port(fullcure280::ORIG_TargetTask, (void *)fullcure280::TargetTask, "280 TargetTask", 280);
		register_port(fullcure280::ORIG_SparkleTask, (void *)fullcure280::SparkleTask, "280 SparkleTask", 280);
	}
}

