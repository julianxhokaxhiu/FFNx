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

// Effect 297: White Wind

#include "mag_common.h"

namespace ff8fx
{
namespace whitewind297
{
	using namespace eng;
	using namespace magc;

	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::MAG_297_WHITE_WIND_QUISTIS_CastContext)); }
	inline uint8_t *&Caster() { return var<uint8_t *>(game_addr(game::MAG_297_WHITE_WIND_QUISTIS_CasterEntity)); }
	inline TaskQueue *QScene() { return (TaskQueue *)game_addr(game::MAG_297_WHITE_WIND_QUISTIS_SceneQueue); }
	inline TaskQueue *QTargets() { return (TaskQueue *)game_addr(game::MAG_297_WHITE_WIND_QUISTIS_TargetQueue); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_297_WHITE_WIND_QUISTIS_PacketCursor)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::MAG_297_WHITE_WIND_QUISTIS_MagicBufferBase)); }
	inline Mat4x3 &ModMatrix() { return var<Mat4x3>(game_addr(game::MAG_297_WHITE_WIND_QUISTIS_ModuleCameraMatrix)); }

	static constexpr game::Id ORIG_RootTask = game::MAG_297_WHITE_WIND_QUISTIS_RootTask;
	static constexpr game::Id ORIG_SceneTask = game::MAG_297_WHITE_WIND_QUISTIS_SceneTask;
	static constexpr game::Id ORIG_TargetTask = game::MAG_297_WHITE_WIND_QUISTIS_TargetTask;
	static constexpr game::Id MODEL_Caster = game::MAG_297_WHITE_WIND_QUISTIS_CasterLayout;
	static constexpr game::Id MODEL_Sweep = game::MAG_297_WHITE_WIND_QUISTIS_SweepLayout;
	static constexpr game::Id MODEL_Target = game::MAG_297_WHITE_WIND_QUISTIS_TargetLayout;
	static const void *SOUND_Wind() { return (const void *)game_addr(game::MAG_297_WHITE_WIND_QUISTIS_SoundWind); }
	static const uint32_t MORPH = 0xAEC;
	static const uint32_t ARENA_A = 0xECC;
	static const uint32_t ARENA_B = 0x10ECC;

	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void RotationFromAnglesB(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::MAG_063_RotationFromAnglesYXZ))(angles, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
	inline uint32_t RenderPrimModel2(void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::MAG_070_RenderPrimModel))(h, ot, mode, cursor); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void MulMatrix3(Mat4x3 *a, const Mat4x3 *b) { fn<void (__cdecl *)(Mat4x3 *, const Mat4x3 *)>(game_addr(game::MatrixMultiply3x3))(a, b); }
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
	struct SceneNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		int16_t caster[4];
		uint8_t lay_caster[0x188];
		int16_t sweep[3];
		int16_t pad1A6;
		uint8_t lay_sweep[0x194];
	};
	struct TargetNode
	{
		TaskNode hdr;
		uint8_t *entity;
		int16_t pos[4];
		uint8_t layout[0x278];
	};
	struct PrimArg
	{
		Mat4x3 m;
		int32_t depth;
		uint32_t morph;
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x14 && sizeof(SceneNode) == 0x33C && sizeof(TargetNode) == 0x290, "White Wind nodes");
	static_assert(sizeof(PrimArg) == 0x28, "White Wind prim block");

	enum : int { PLAY_CASTER = 0, PLAY_SWEEP = 1, PLAY_TARGET = 2 };

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
		const int16_t off[4] = { r->pos[0], r->pos[1], r->pos[2], 0 };
		if (r->flags & 0x1000)
		{
			GteSetRotMatrixCtrl(&ModMatrix());
			GteLoadV0(off);
			GteMVMVA_RotV0();
			GteReadMAC123(m.t);
			if (!(r->flags & 0x8000)) MatrixMultiply(&ModMatrix(), &m);
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
				Mat4x3 d = {};
				d.m[0][0] = r->scale[0];
				d.m[1][1] = r->scale[1];
				d.m[2][2] = r->scale[2];
				MulMatrix3(&m, &d);
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

	static void Billboard(Mat4x3 *m)
	{
		ff8fx::copy<(18)>(m->m, Camera().m);
		TransformVectorBy3x3Matrix(&Camera(), m->t, m->t);
		m->t[0] = (int32_t)((uint32_t)m->t[0] + (uint32_t)Camera().t[0]);
		m->t[1] = (int32_t)((uint32_t)m->t[1] + (uint32_t)Camera().t[1]);
		m->t[2] = (int32_t)((uint32_t)m->t[2] + (uint32_t)Camera().t[2]);
	}

	static int16_t AnchorY(const uint8_t *e)
	{
		const int32_t h = *(const int16_t *)(e + 0x92);
		const int32_t v = ((2 * h) / 3 + h / 2) / 2;
		return (int16_t)(v + *(const int16_t *)(e + 0x24));
	}

	static void ApplyResult(const uint8_t *entity)
	{
		const int32_t slot = (int32_t)((uint32_t)entity - game_addr(game::BattleEntitySlotData)) / 0x9C;
		for (int i = 0; i < Ctx()->actions[0].target_count; i++)
		{
			uint8_t *rec = Ctx()->actions[0].targets + i * TARGET_STRIDE;
			if (*rec == slot) ApplyActionResultToTarget(rec);
		}
	}
}
}

namespace ff8fx
{
namespace whitewind297
{
	static uint32_t __cdecl TargetTask(TaskNode *n)
	{
		TargetNode *t = (TargetNode *)n;
		PrimArg arg;
		arg.m.t[0] = t->pos[0];
		arg.m.t[1] = t->pos[1];
		arg.m.t[2] = t->pos[2];
		Billboard(&arg.m);
		arg.depth = -0x80;
		arg.morph = TexBase() + MORPH;
		if (prim::play((prim::Layout *)t->layout, PrimCallback, (int)&arg, 0) == 0xA)
		{
			ApplyResult(t->entity);
			return TASK_END;
		}
		return 0;
	}

	static uint32_t __cdecl SceneTask(TaskNode *n)
	{
		SceneNode *s = (SceneNode *)n;
		PrimArg arg;
		if ((uint32_t)(int32_t)s->counter < 0x16)
		{
			if (s->counter == 0)
			{
				GetDefaultEffectPosition(Caster(), s->caster);
				s->caster[1] = AnchorY(Caster());
				DecodeModelPrimLayout(game_addr(MODEL_Caster), s->lay_caster, 0x188);
			}
			arg.m.t[0] = s->caster[0];
			arg.m.t[1] = s->caster[1];
			arg.m.t[2] = s->caster[2];
			Billboard(&arg.m);
			arg.depth = -0x80;
			arg.morph = TexBase() + MORPH;
			prim::play((prim::Layout *)s->lay_caster, PrimCallback, (int)&arg, 0);
		}
		if ((uint32_t)((int32_t)s->counter - 7) < 0x16)
		{
			if (s->counter == 7)
			{
				ActionData *a = Ctx()->actions;
				s->sweep[2] = 0;
				for (int i = 0; i < a->target_count; i++)
				{
					s->sweep[2] = (int16_t)(s->sweep[2] + *(const int16_t *)(Entity(a->targets[i * TARGET_STRIDE]) + 0x20));
					a = Ctx()->actions;
				}
				s->sweep[0] = 0;
				s->sweep[2] = (int16_t)((int32_t)s->sweep[2] / (int32_t)a->target_count);
				s->sweep[1] = AnchorY(Caster());
				DecodeModelPrimLayout(game_addr(MODEL_Sweep), s->lay_sweep, 0x194);
			}
			arg.m.t[0] = s->sweep[0];
			arg.m.t[1] = s->sweep[1];
			arg.m.t[2] = s->sweep[2];
			arg.m.m[0][0] = 0x1000;
			arg.m.m[2][2] = 0x800;
			arg.m.m[1][1] = 0x800;
			arg.m.m[2][1] = 0;
			arg.m.m[2][0] = 0;
			arg.m.m[1][2] = 0;
			arg.m.m[1][0] = 0;
			arg.m.m[0][2] = 0;
			arg.m.m[0][1] = 0;
			ComposeAffineTransform(&Camera(), &arg.m, &arg.m);
			arg.depth = -0x80;
			arg.morph = TexBase() + MORPH;
			prim::play((prim::Layout *)s->lay_sweep, PrimCallback, (int)&arg, 0);
		}
		if (s->counter == 1) BdPlaySE(SOUND_Wind(), 0, 0x80);
		const int32_t k = (int32_t)s->counter - 0x13;
		if ((uint32_t)k < 0xC && (k & 3) == 0)
		{
			const uint32_t i = (uint32_t)k >> 2;
			if (i < Ctx()->actions[0].target_count)
			{
				TargetNode *t = (TargetNode *)AddTaskToQueue(QTargets(), game_addr(ORIG_TargetTask));
				if (t)
				{
					uint8_t *e = Entity(Ctx()->actions[0].targets[i * TARGET_STRIDE]);
					t->entity = e;
					GetDefaultEffectPosition(e, t->pos);
					t->pos[1] = AnchorY(e);
					DecodeModelPrimLayout(game_addr(MODEL_Target), t->layout, 0x278);
				}
			}
		}
		s->counter++;
		return s->counter < 0x1D ? 0 : TASK_END;
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
		if (r->counter == 1 && !r->started)
		{
			r->started = 1;
			InitTaskQueuePool(QScene(), (void *)(TexBase() + 0x7B0), 0x33C, 1);
			InitTaskQueuePool(QTargets(), (void *)TexBase(), 0x290, 3);
			((SceneNode *)AddTaskToQueue(QScene(), game_addr(ORIG_SceneTask)))->counter = 0;
		}
		int a, b;
		if (r->started)
		{
			EffectCameraMatrix(&Camera(), &ModMatrix());
			a = ExecuteTaskQueue(QScene());
			b = ExecuteTaskQueue(QTargets());
		}
		else a = b = (int)n;
		if (r->started && a == 0 && b == 0) return TASK_END;
		r->counter++;
		return 0;
	}
}

	void register_mag297_white_wind_quistis()
	{
		register_port(whitewind297::ORIG_RootTask, (void *)whitewind297::RootTask, "297 RootTask", 297);
		register_port(whitewind297::ORIG_SceneTask, (void *)whitewind297::SceneTask, "297 SceneTask", 297);
		register_port(whitewind297::ORIG_TargetTask, (void *)whitewind297::TargetTask, "297 TargetTask", 297);
	}
}

