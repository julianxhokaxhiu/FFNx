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

// Effect 111: Aura

#include "mag_common.h"

namespace ff8fx
{
namespace aura111
{
	using namespace eng;
	using namespace magc;

	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::unk_2521974)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::unk_2521978)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::unk_2521994)); }
	inline TaskQueue *QFlames() { return (TaskQueue *)game_addr(game::unk_2521998); }
	inline TaskQueue *QTargets() { return (TaskQueue *)game_addr(game::unk_25219A8); }
	inline uint32_t RenderOT(uint32_t off) { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + off; }

	static constexpr game::Id ORIG_RootTask = game::MAG_111_Aura_RootTask;
	static constexpr game::Id ORIG_TargetTask = game::MAG_111_AURA_MainVisual_Tick;
	static constexpr game::Id ORIG_FlameTask = game::MAG_111_Aura_FlameTask;
	static constexpr game::Id MODEL_Aura = game::unk_12DB9E0;
	static constexpr game::Id MODEL_Flame = game::unk_12DB818;
	static constexpr game::Id SEQ_Glow = game::unk_12DB75C;
	static constexpr game::Id PART_Flags = game::unk_12E061C;
	static const void *SOUND_Aura() { return (const void *)game_addr(game::unk_12E0618); }

	inline int32_t ComputeCos(int32_t angle) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(angle); }
	inline int32_t Sqrt(int32_t v) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::Math_SqrtInt))(v); }
	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t flag, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, flag, out); }
	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
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
		uint8_t aura[0x34C];
	};
	struct FlameNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		int16_t pos[3];
		int16_t radius;
		int16_t angle;
		int16_t speed;
		int16_t fade;
		int16_t pad1E;
	};
	struct PrimArg
	{
		Mat4x3 m;
		uint32_t flags;
		uint32_t morph;
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x14 && sizeof(TargetNode) == 0x360 && sizeof(FlameNode) == 0x20, "Aura nodes");
	static_assert(sizeof(PrimArg) == 0x28, "Aura prim block");

	static void __cdecl AuraPart(prim::Layout *l, prim::Record *r, int arg_)
	{
		PrimArg *arg = (PrimArg *)arg_;
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
		m.t[0] = r->pos[0];
		m.t[1] = r->pos[1];
		m.t[2] = r->pos[2];
		const uint32_t flags = *(const uint32_t *)(arg->flags + 4 * (int16_t)r->index);
		if (flags & 0x10000000)
		{
			TransformVectorBy3x3Matrix(&arg->m, m.t, m.t);
			m.t[0] = (int32_t)((uint32_t)m.t[0] + (uint32_t)arg->m.t[0]);
			m.t[1] = (int32_t)((uint32_t)m.t[1] + (uint32_t)arg->m.t[1]);
			m.t[2] = (int32_t)((uint32_t)m.t[2] + (uint32_t)arg->m.t[2]);
		}
		else ComposeAffineTransform(&arg->m, &m, &m);
		if (!(*(const uint32_t *)r->scale == 0x10001000 && r->scale[2] == 0x1000))
		{
			int32_t v[3] = { r->scale[0], r->scale[1], r->scale[2] };
			Scale3DMatrix(&m, v);
		}
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		const uint32_t mode = (*(const uint32_t *)(arg->flags + 4 * (int16_t)r->index) & 0xDFFF) | 0x2000;
		*(uint32_t *)(h + 0x1C) = mode;
		*(int32_t *)(h + 0xC) = r->a;
		if (r->a != 0)
		{
			*(uint32_t *)(h + 0x1C) = mode | 0xC0;
			*(uint32_t *)(h + 8) = *(const uint32_t *)r->rgb;
		}
		PacketCursor() = RenderPrimModel(h, RenderOT(0x44), 2, PacketCursor());
		FieldFree(0x58);
	}

	static void RollScaleMatrix(int16_t angle, Mat4x3 *m, int32_t s)
	{
		const int32_t sn = mul32(ComputeSin(angle), s) >> 12;
		const int32_t cs = mul32(ComputeCos(angle), s) >> 12;
		m->m[0][1] = (int16_t)-sn;
		m->m[1][0] = (int16_t)sn;
		m->m[0][0] = (int16_t)cs;
		m->m[0][2] = 0;
		m->m[1][1] = (int16_t)cs;
		m->m[1][2] = 0;
		m->m[2][0] = 0;
		m->m[2][1] = 0;
		m->m[2][2] = 0x1000;
	}

	static int32_t FlameFade(const FlameNode *f)
	{
		const int16_t c = f->counter;
		if (c < 8)
		{
			const int32_t e = 0x1000 - (shl32(c, 12) / 8);
			return 0x1000 - (mul32(f->fade, 0x1000 - e) >> 12);
		}
		if (c > 0xC)
		{
			const int32_t e = shl32(c - 0xC, 12) / 8;
			return 0x1000 - (mul32(f->fade, 0x1000 - e) >> 12);
		}
		return 0x1000 - f->fade;
	}

	static void FlameDraw(const FlameNode *f, uint8_t *h)
	{
		Mat4x3 m = {};
		RollScaleMatrix(f->angle, &m, f->radius);
		GteSetRotMatrixCtrl(&Camera());
		GteLoadV0(f->pos);
		GteMVMVA_RotV0();
		GteReadMAC123(m.t);
		m.t[0] = (int32_t)((uint32_t)m.t[0] + (uint32_t)Camera().t[0]);
		m.t[1] = (int32_t)((uint32_t)m.t[1] + (uint32_t)Camera().t[1]);
		m.t[2] = (int32_t)((uint32_t)m.t[2] + (uint32_t)Camera().t[2] - 0x100u);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		PacketCursor() = RenderPrimModel(h, RenderOT(0x44), 2, PacketCursor());
	}

	static void FlameHeader(uint8_t *h, const FlameNode *f)
	{
		*(uint32_t *)h = game_addr(MODEL_Flame);
		h[0xA] = 0;
		h[9] = 0;
		h[8] = 0;
		*(int32_t *)(h + 0xC) = FlameFade(f);
		*(uint32_t *)(h + 0x1C) = 0xF3;
	}
}
}

namespace ff8fx
{
namespace aura111
{
	static uint32_t __cdecl FlameTask(TaskNode *n)
	{
		FlameNode *f = (FlameNode *)n;
		uint8_t *h = AllocHeader(0x58);
		FlameHeader(h, f);
		FlameDraw(f, h);
		FieldFree(0x58);
		if (f->counter >= 0x14) return TASK_END;
		f->angle = (int16_t)(f->angle + f->speed);
		f->counter++;
		return 0;
	}

	static void GlowDraw(const int16_t *pos, int32_t e)
	{
		TransformCameraByShadowRotation(pos, 0x1000, (int32_t)0xFFFFFF00);
		uint8_t *h = AllocHeader(0xB4);
		*(uint32_t *)h = game_addr(SEQ_Glow);
		*(int16_t *)(h + 4) = (int16_t)e;
		*(uint16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(0x44), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static uint32_t __cdecl TargetTask(TaskNode *n)
	{
		TargetNode *t = (TargetNode *)n;
		if (t->counter >= 0)
		{
			PrimArg arg;
			const int16_t s = (int16_t)Sqrt(shl32(*(const int16_t *)(t->entity + 0x26), 12));
			int16_t pos[4];
			arg.m.m[0][0] = s;
			arg.m.m[1][1] = s;
			arg.m.m[2][2] = s;
			arg.m.m[2][1] = 0;
			arg.m.m[2][0] = 0;
			arg.m.m[1][2] = 0;
			arg.m.m[1][0] = 0;
			arg.m.m[0][2] = 0;
			arg.m.m[0][1] = 0;
			arg.m.pad = 0;
			GetEffectSpawnPosition(t->entity, 0xF1, 0, pos);
			arg.m.t[0] = pos[0];
			arg.m.t[2] = pos[2];
			arg.m.t[1] = *(const int16_t *)(t->entity + 0x24);
			ComposeAffineTransform(&Camera(), &arg.m, &arg.m);
			arg.flags = game_addr(PART_Flags);
			arg.morph = TexBase() + 0xE20;
			prim::play((prim::Layout *)t->aura, AuraPart, (int)&arg, 0);
		}
		if ((uint32_t)(t->counter - 0xF) < 4)
		{
			int32_t angle = CrtRand();
			int32_t speed = (CrtRand() & 0xF) + 0x10;
			if (CrtRand() & 1) speed = -speed;
			for (int k = 0; k < 4; k++)
			{
				FlameNode *f = (FlameNode *)AddTaskToQueue(QFlames(), game_addr(ORIG_FlameTask));
				if (f)
				{
					Memset32(&f->counter, 0, 5);
					GetEffectSpawnPosition(t->entity, 0xF1, 0, f->pos);
					f->pos[1] = (int16_t)(*(const int16_t *)(t->entity + 0x24) - 0x258);
					const int32_t r = CrtRand() & 0x3FF;
					f->angle = (int16_t)angle;
					f->speed = (int16_t)speed;
					f->radius = (int16_t)(r + 0x320);
					f->fade = (int16_t)((CrtRand() & 0x7FF) + 0x800);
				}
				angle += 0x400;
			}
		}
		const int32_t e = t->counter - 0x1C;
		if ((uint32_t)e < 6)
		{
			if (e == 0) ScreenFadeTask(0, 1, 0, 0x80);
			int16_t pos[4];
			GetEffectSpawnPosition(t->entity, 0xF1, 0, pos);
			pos[1] = (int16_t)(*(const int16_t *)(t->entity + 0x24) - 0x258);
			GlowDraw(pos, e);
		}
		if (t->counter == 0)
		{
			int16_t pos[4];
			GetDefaultEffectPosition(t->entity, pos);
			BdPlaySE3D(SOUND_Aura(), 0x101, pos);
		}
		if (t->counter == 0x1D) ApplyActionResultToTarget(Ctx()->actions[t->action].targets);
		if (t->counter >= 0x22) return TASK_END;
		t->counter++;
		return 0;
	}

	static uint32_t __cdecl RootTask(TaskNode *n)
	{
		RootNode *r = (RootNode *)n;
		if (r->arena)
		{
			PacketCursor() = TexBase() + 0xE28;
			r->arena = 0;
		}
		else
		{
			PacketCursor() = TexBase() + 0x18E28;
			r->arena = 1;
		}
		if (r->counter == 1)
		{
			if (!r->started)
			{
				r->started = 1;
				InitTaskQueuePool(QTargets(), (void *)TexBase(), 0x360, 3);
				InitTaskQueuePool(QFlames(), (void *)(TexBase() + 0xA20), 0x20, 0x20);
			}
			ActionData *acts = Ctx()->actions;
			if (r->action <= acts[0].last_action)
			{
				uint8_t *entity = Entity(acts[r->action].targets[0]);
				TargetNode *pool = (TargetNode *)TexBase();
				bool busy = false;
				for (int k = 0; k < 3; k++)
					if ((*(const uint8_t *)&pool[k].hdr.flags & 1) && pool[k].entity == entity && pool[k].counter <= 0x1C)
					{
						busy = true;
						break;
					}
				if (busy) r->counter = 0;
				else
				{
					TargetNode *t = (TargetNode *)AddTaskToQueue(QTargets(), game_addr(ORIG_TargetTask));
					if (t)
					{
						Memset32(&t->counter, 0, 0xD5);
						t->action = (int16_t)(uint16_t)r->action;
						t->entity = entity;
						DecodeModelPrimLayout(game_addr(MODEL_Aura), t->aura, 0x34C);
						r->action++;
					}
				}
			}
		}
		int a, b;
		if (r->started)
		{
			a = ExecuteTaskQueue(QTargets());
			b = ExecuteTaskQueue(QFlames());
		}
		else
		{
			a = (int)n;
			b = (int)n;
		}
		if (r->started && a == 0 && b == 0) return TASK_END;
		r->counter++;
		if (r->counter >= 10) r->counter = 0;
		return 0;
	}
}

	void register_mag111_aura()
	{
		register_port(aura111::ORIG_RootTask, (void *)aura111::RootTask, "A111 RootTask", 111);
		register_port(aura111::ORIG_TargetTask, (void *)aura111::TargetTask, "A111 TargetTask", 111);
		register_port(aura111::ORIG_FlameTask, (void *)aura111::FlameTask, "A111 FlameTask", 111);
	}
}

