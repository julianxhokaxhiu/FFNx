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

// Effect 130: Eerie Sound Wave

#include "mag_common.h"

namespace ff8fx
{
namespace eerie130
{
	using namespace eng;
	using namespace magc;

	inline uint8_t *&Caster() { return var<uint8_t *>(game_addr(game::g130_CasterEntity)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::g130_MagicBufferBase)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::g130_CastContext)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::g130_PacketCursor)); }
	inline TaskQueue *QImpacts() { return (TaskQueue *)game_addr(game::g130_QueueImpacts); }
	inline TaskQueue *QWaves() { return (TaskQueue *)game_addr(game::g130_QueueWaves); }
	inline TaskQueue *QEmitter() { return (TaskQueue *)game_addr(game::g130_QueueEmitter); }
	inline uint32_t RenderOT() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44; }

	static constexpr game::Id ORIG_MasterTask = game::MAG_130_EerieSoundWave_MasterTask;
	static constexpr game::Id ORIG_EmitterTask = game::MAG_130_EerieSoundWave_EmitterTask;
	static constexpr game::Id ORIG_WaveTask = game::MAG_130_EerieSoundWave_WaveTask;
	static constexpr game::Id ORIG_ImpactTask = game::MAG_130_EerieSoundWave_ImpactTask;
	static constexpr game::Id MODEL_Emitter = game::unk_1221138;
	static constexpr game::Id MODEL_Wave = game::unk_12228A8;
	static constexpr game::Id MODEL_Impact = game::unk_1220990;
	static const void *SOUND_Wave() { return (const void *)game_addr(game::unk_122380C); }

	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t flag, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, flag, out); }
	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void RotationFromAngles2(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::MAG_063_RotationFromAnglesYXZ))(angles, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void MulMatrix3(Mat4x3 *a, const void *b) { fn<void (__cdecl *)(Mat4x3 *, const void *)>(game_addr(game::MatrixMultiply3x3))(a, b); }
	inline void GteMVMVA_RotV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0))(); }
	inline void RotY(int32_t angle, Mat4x3 *out) { fn<void (__cdecl *)(int32_t, Mat4x3 *)>(game_addr(game::Effect_RotationYMatrix))(angle, out); }
	inline uint32_t RenderPrim2(void *h, uint32_t ot, int mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int, uint32_t)>(game_addr(game::MAG_070_RenderPrimModel))(h, ot, mode, cursor); }
	inline int32_t BuildOrthonormalBasis(const int32_t *v, Mat4x3 *m) { return fn<int32_t (__cdecl *)(const int32_t *, Mat4x3 *)>(game_addr(game::BuildOrthonormalBasis))(v, m); }
	inline void BlendVectors(const int16_t *a, const int16_t *b, int32_t wa, int32_t wb, int16_t *out) { fn<void (__cdecl *)(const int16_t *, const int16_t *, int32_t, int32_t, int16_t *)>(game_addr(game::VecLerp))(a, b, wa, wb, out); }

#pragma pack(push, 1)
	struct MasterNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t pad0E;
		uint8_t started;
		uint32_t arena;
	};
	struct EmitterNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t count;
		uint8_t *ents[3];
		uint8_t idx[3];
		uint8_t pad1F;
		uint8_t layout[0x4AC];
	};
	struct WaveNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t no_hit;
		uint8_t idx;
		uint8_t *entity;
		int16_t start[4];
		uint8_t layout[0x58];
	};
	struct ImpactNode
	{
		TaskNode hdr;
		int16_t pos[4];
		uint8_t *entity;
		uint8_t layout[0xA8];
	};
	struct PrimArg
	{
		Mat4x3 m;
		int32_t depth;
		uint32_t morph;
	};
#pragma pack(pop)
	static_assert(sizeof(MasterNode) == 0x14 && sizeof(EmitterNode) == 0x4CC && sizeof(WaveNode) == 0x74 && sizeof(ImpactNode) == 0xC0, "Eerie Sound Wave nodes");
	static_assert(sizeof(PrimArg) == 0x28, "Eerie Sound Wave prim block");

	struct StackWords
	{
		int16_t m22;
	};
	static StackWords &Stack()
	{
		static StackWords s = {};
		return s;
	}

	static void __cdecl PrimCallback(prim::Layout *l, prim::Record *r, int arg_)
	{
		const PrimArg *arg = (const PrimArg *)arg_;
		if (((int32_t)r->scale[2] | *(const int32_t *)r->scale) == 0) return;
		if (r->a >= 0x1000 && *(const uint32_t *)r->rgb == 0) return;
		uint8_t *h = AllocHeader(0x58);
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
		if (r->flags & 0x40000) RotationFromAngles2(r->rot, &m);
		else RotationFromAngles(r->rot, &m);
		int16_t off[4] = { r->pos[0], r->pos[1], r->pos[2], 0 };
		int32_t t[3];
		if (r->flags & 0x200)
		{
			t[0] = off[0];
			t[1] = off[1];
			t[2] = off[2];
		}
		else
		{
			GteSetRotMatrixCtrl(&arg->m);
			GteLoadV0(off);
			GteMVMVA_RotV0();
			GteReadMAC123(m.t);
			MatrixMultiply(&arg->m, &m);
			t[0] = m.t[0];
			t[1] = m.t[1];
			t[2] = m.t[2];
		}
		m.t[0] = (int32_t)((uint32_t)t[0] + (uint32_t)arg->m.t[0]);
		m.t[1] = (int32_t)((uint32_t)t[1] + (uint32_t)arg->m.t[1]);
		m.t[2] = (int32_t)((uint32_t)t[2] + (uint32_t)arg->m.t[2]);
		if (!(*(const uint32_t *)r->scale == 0x10001000 && r->scale[2] == 0x1000))
		{
			if (r->flags & 0x100)
			{
				int16_t s[9] = { r->scale[0], 0, 0, 0, r->scale[1], 0, 0, 0, r->scale[2] };
				MulMatrix3(&m, s);
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
		PacketCursor() = RenderPrim2(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}
}
}

namespace ff8fx
{
namespace eerie130
{
	static void DamageEntity(uint8_t *entity)
	{
		const int32_t slot = (int32_t)((uint32_t)entity - game_addr(game::BattleEntitySlotData)) / 0x9C;
		for (uint32_t i = 0; i < Ctx()->actions[0].target_count; i++)
		{
			uint8_t *t = Ctx()->actions[0].targets + i * TARGET_STRIDE;
			if ((int32_t)t[0] == slot) ApplyActionResultToTarget(t);
		}
	}

	static uint32_t __cdecl ImpactTask(TaskNode *n)
	{
		ImpactNode *p = (ImpactNode *)n;
		PrimArg arg;
		arg.m.m[1][1] = 0x1000;
		arg.m.m[0][0] = 0x1000;
		arg.m.t[0] = p->pos[0];
		arg.m.t[1] = p->pos[1];
		arg.m.m[2][1] = 0;
		arg.m.m[2][0] = 0;
		arg.m.m[1][2] = 0;
		arg.m.m[1][0] = 0;
		arg.m.m[0][2] = 0;
		arg.m.m[0][1] = 0;
		arg.m.t[2] = p->pos[2];
		arg.m.m[2][2] = Stack().m22;
		arg.m.pad = 0;
		ComposeAffineTransform(&Camera(), &arg.m, &arg.m);
		Stack().m22 = arg.m.m[2][2];
		arg.depth = -0x80;
		arg.morph = TexBase() + 0xC7C;
		const int played = prim::play((prim::Layout *)p->layout, PrimCallback, (int)&arg, 0);
		if (played == 0) return TASK_END;
		if (played == 8) DamageEntity(p->entity);
		return 0;
	}

	static uint32_t __cdecl WaveTask(TaskNode *n)
	{
		WaveNode *w = (WaveNode *)n;
		int16_t pos[4];
		GetDefaultEffectPosition(w->entity, pos);
		int32_t d[3];
		d[0] = (int32_t)pos[0] - w->start[0];
		d[1] = (int32_t)pos[1] - w->start[1];
		d[2] = (int32_t)pos[2] - w->start[2];
		if (w->counter < 0x18)
		{
			const int32_t s = ComputeSin(shl32(w->counter, 10) / 24);
			BlendVectors(w->start, pos, 0x1000 - s, s, pos);
		}
		if (w->counter == 0 && w->no_hit == 0)
		{
			ImpactNode *p = (ImpactNode *)AddTaskToQueue(QImpacts(), game_addr(ORIG_ImpactTask));
			if (p)
			{
				GetDefaultEffectPosition(w->entity, p->pos);
				p->entity = w->entity;
				DecodeModelPrimLayout(game_addr(MODEL_Impact), p->layout, 0xA8);
			}
		}
		PrimArg arg;
		BuildOrthonormalBasis(d, &arg.m);
		arg.m.t[0] = pos[0];
		arg.m.t[1] = pos[1];
		arg.m.t[2] = pos[2];
		ComposeAffineTransform(&Camera(), &arg.m, &arg.m);
		Stack().m22 = arg.m.m[2][0];
		arg.depth = 0;
		arg.morph = TexBase() + 0xC7C;
		if (prim::play((prim::Layout *)w->layout, PrimCallback, (int)&arg, 0) == 0) return TASK_END;
		w->counter++;
		return 0;
	}

	static uint32_t __cdecl EmitterTask(TaskNode *n)
	{
		EmitterNode *e = (EmitterNode *)n;
		int16_t pos[4];
		GetEffectSpawnPosition(Caster(), 0xB, 0x4000, pos);
		pos[1] = (int16_t)(pos[1] - 200);
		PrimArg arg;
		RotY((int16_t)-*(const int16_t *)(Caster() + 0xE), &arg.m);
		arg.m.t[0] = pos[0];
		arg.m.t[1] = pos[1];
		arg.m.t[2] = pos[2];
		ComposeAffineTransform(&Camera(), &arg.m, &arg.m);
		Stack().m22 = arg.m.m[1][1];
		arg.depth = -0x80;
		arg.morph = TexBase() + 0xC7C;
		if (prim::play((prim::Layout *)e->layout, PrimCallback, (int)&arg, 0) == 0) return TASK_END;
		if (e->counter == 10) BdPlaySE(SOUND_Wave(), 0, 0x80);
		pos[3] = 0;
		GetEffectSpawnPosition(Caster(), 0xB, 0x1000, pos);
		const int32_t c = e->counter;
		if (c < (int32_t)e->count * 12)
		{
			const int32_t r = c % 12;
			if ((uint32_t)r % 3 == 0)
			{
				WaveNode *w = (WaveNode *)AddTaskToQueue(QWaves(), game_addr(ORIG_WaveTask));
				if (w)
				{
					w->idx = e->idx[e->counter / 12];
					w->no_hit = r != 9;
					w->entity = e->ents[e->counter / 12];
					w->counter = 0;
					ff8fx::copy<(8)>(w->start, pos);
					DecodeModelPrimLayout(game_addr(MODEL_Wave), w->layout, 0x58);
				}
			}
		}
		e->counter++;
		return 0;
	}

	static uint32_t __cdecl MasterTask(TaskNode *n)
	{
		MasterNode *m = (MasterNode *)n;
		Stack().m22 = 0;
		if (m->arena)
		{
			PacketCursor() = TexBase() + 0xF7C;
			m->arena = 0;
		}
		else
		{
			PacketCursor() = TexBase() + 0x18F7C;
			m->arena = 1;
		}
		if (m->counter == 1 && !m->started)
		{
			m->started = 1;
			InitTaskQueuePool(QEmitter(), (void *)(TexBase() + 0x7B0), 0x4CC, 1);
			InitTaskQueuePool(QWaves(), (void *)(TexBase() + 0x240), 0x74, 0xC);
			InitTaskQueuePool(QImpacts(), (void *)TexBase(), 0xC0, 3);
			EmitterNode *e = (EmitterNode *)AddTaskToQueue(QEmitter(), game_addr(ORIG_EmitterTask));
			Memset32(&e->counter, 0, 0x130);
			DecodeModelPrimLayout(game_addr(MODEL_Emitter), e->layout, 0x4AC);
			uint8_t *const b = (uint8_t *)e;
			e->count = (int16_t)Ctx()->actions[0].target_count;
			for (int32_t i = 0; i < e->count; i++)
			{
				b[0x1C + i] = (uint8_t)i;
				*(uint8_t **)(b + 0x10 + 4 * i) = Entity(Ctx()->actions[0].targets[i * TARGET_STRIDE]);
			}
			for (int32_t i = 0; i < e->count - 1; i++)
			{
				for (int32_t j = i + 1; j < e->count; j++)
				{
					uint8_t *a = *(uint8_t **)(b + 0x10 + 4 * i);
					uint8_t *c = *(uint8_t **)(b + 0x10 + 4 * j);
					if (*(const int16_t *)(a + 0x1C) < *(const int16_t *)(c + 0x1C))
					{
						*(uint8_t **)(b + 0x10 + 4 * i) = c;
						*(uint8_t **)(b + 0x10 + 4 * j) = a;
						const uint8_t x = b[0x1C + j];
						const uint8_t y = b[0x1C + i];
						b[0x1C + i] = x;
						b[0x1C + j] = y;
					}
				}
			}
		}
		int a0, a1, a2;
		if (m->started)
		{
			a0 = ExecuteTaskQueue(QEmitter());
			a1 = ExecuteTaskQueue(QWaves());
			a2 = ExecuteTaskQueue(QImpacts());
		}
		else a0 = a1 = a2 = (int)n;
		if (m->started && a0 == 0 && a1 == 0 && a2 == 0) return TASK_END;
		m->counter++;
		return 0;
	}
}

	void register_mag130_eerie_sound_wave()
	{
		register_port(eerie130::ORIG_MasterTask, (void *)eerie130::MasterTask, "130 MasterTask", 130);
		register_port(eerie130::ORIG_EmitterTask, (void *)eerie130::EmitterTask, "130 EmitterTask", 130);
		register_port(eerie130::ORIG_WaveTask, (void *)eerie130::WaveTask, "130 WaveTask", 130);
		register_port(eerie130::ORIG_ImpactTask, (void *)eerie130::ImpactTask, "130 ImpactTask", 130);
	}
}

