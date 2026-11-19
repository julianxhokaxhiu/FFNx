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

// Effect 118: Aero

#include "mag_common.h"

namespace ff8fx
{
namespace aero118
{
	using namespace eng;
	using namespace magc;

	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::unk_25215DC)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::unk_25215E0)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::unk_25215FC)); }
	inline Mat4x3 &EffectCamera() { return var<Mat4x3>(game_addr(game::unk_2521600)); }
	inline TaskQueue *QLeaves() { return (TaskQueue *)game_addr(game::unk_2521620); }
	inline TaskQueue *QTargets() { return (TaskQueue *)game_addr(game::unk_2521630); }
	inline uint32_t RenderOT(uint32_t off) { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + off; }

	static constexpr game::Id ORIG_RootTask = game::MAG_118_AERO_Director_Tick;
	static constexpr game::Id ORIG_TargetTask = game::MAG_118_Aero_TargetTask;
	static constexpr game::Id ORIG_LeafTask = game::MAG_118_Aero_LeafTask;
	static constexpr game::Id MODEL_Whirl = game::unk_1287DBC;
	static constexpr game::Id MODEL_Gust = game::unk_128C110;
	static constexpr game::Id MODEL_Leaf = game::unk_1287C44;
	static const void *SOUND_Aero() { return (const void *)game_addr(game::unk_128D5AC); }

	inline int32_t ComputeCos(int32_t angle) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(angle); }
	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
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
		uint8_t whirl[0x4EC];
		uint8_t gust[0x184];
	};
	struct LeafNode
	{
		TaskNode hdr;
		int16_t action;
		int16_t reach;
		int16_t scale;
		int16_t angle;
		int16_t speed;
		int16_t cx;
		int16_t cz;
		int16_t pos[3];
		int16_t pad20;
		int16_t cosv;
		int16_t vy;
		int16_t sinv;
		int16_t pad28;
		int16_t ang[3];
		int16_t fall;
		int8_t spin[3];
		uint8_t pad35[3];
		void *owner;
	};
	struct PrimArg
	{
		int16_t pos[4];
		int32_t scale[3];
		int32_t scaled;
		int32_t depth;
		uint32_t morph;
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x14 && sizeof(TargetNode) == 0x68C && sizeof(LeafNode) == 0x3C, "Aero nodes");
	static_assert(sizeof(PrimArg) == 0x20, "Aero prim block");

	static void __cdecl WindPart(prim::Layout *l, prim::Record *r, int arg_)
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
		if (flags & 0x1000)
		{
			GteSetRotMatrixCtrl(&Camera());
			GteLoadV0(arg->pos);
			GteMVMVA_RotV0();
			GteReadMAC123(m.t);
			GteSetRotMatrixCtrl(&EffectCamera());
			GteLoadV0(off);
			GteMVMVA_RotV0();
			int32_t mac[3];
			GteReadMAC123(mac);
			m.t[0] = (int32_t)((uint32_t)m.t[0] + (uint32_t)mac[0]);
			m.t[1] = (int32_t)((uint32_t)m.t[1] + (uint32_t)mac[1]);
			m.t[2] = (int32_t)((uint32_t)m.t[2] + (uint32_t)mac[2]);
			if (!(r->flags & 0x8000)) MatrixMultiply(&EffectCamera(), &m);
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
			if (!(r->flags & 0x8000)) MatrixMultiply(&Camera(), &m);
		}
		m.t[0] = (int32_t)((uint32_t)m.t[0] + (uint32_t)Camera().t[0]);
		m.t[1] = (int32_t)((uint32_t)m.t[1] + (uint32_t)Camera().t[1]);
		m.t[2] = (int32_t)((uint32_t)m.t[2] + (uint32_t)Camera().t[2]);
		if (!(*(const uint32_t *)r->scale == 0x10001000 && r->scale[2] == 0x1000))
		{
			if (r->flags & 0x100)
			{
				Mat4x3 d = {};
				d.m[0][0] = r->scale[0];
				d.m[1][1] = r->scale[1];
				d.m[2][2] = r->scale[2];
				MatrixMultiply3(&m, &d);
			}
			else
			{
				for (int k = 0; k < 3; k++)
				{
					const int16_t s = r->scale[k];
					if (s == 0x1000) continue;
					m.m[k][0] = (int16_t)(mul32(m.m[k][0], s) >> 12);
					m.m[k][1] = (int16_t)(mul32(m.m[k][1], s) >> 12);
					m.m[k][2] = (int16_t)(mul32(m.m[k][2], s) >> 12);
				}
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
		PacketCursor() = RenderPrimModel2(h, RenderOT(0x44), 2, PacketCursor());
		FieldFree(0x58);
	}

	static bool LeafMove(LeafNode *f)
	{
		const int16_t oc = ((const TargetNode *)f->owner)->counter;
		if (oc < 0x28)
		{
			if (oc >= 0xA)
			{
				f->vy = (int16_t)(f->vy - 4);
				f->speed = (int16_t)(f->speed + 0x10);
			}
			f->scale = (int16_t)(f->scale + 0x100);
			if (f->scale > 0x1000) f->scale = 0x1000;
			f->pos[1] = (int16_t)(f->pos[1] + f->vy);
			const int32_t y = f->pos[1] >> 6;
			const int32_t radius = shl32(f->reach, 5) / (mul32(y, y) + 0x200);
			f->angle = (int16_t)(f->angle + f->speed);
			const int32_t c = ComputeCos(f->angle);
			f->cosv = (int16_t)c;
			f->pos[0] = (int16_t)((mul32((int16_t)c, radius) >> 9) + f->cx);
			const int32_t s = ComputeSin(f->angle);
			f->sinv = (int16_t)s;
			f->pos[2] = (int16_t)((mul32((int16_t)s, radius) >> 9) + f->cz);
		}
		else
		{
			const int16_t n = f->fall;
			f->fall = (int16_t)(n + 1);
			if (n > 0x10)
			{
				f->scale = (int16_t)(f->scale - 0x200);
				if (f->scale <= 0) return false;
			}
			f->vy = (int16_t)(f->vy + 0x10);
			f->pos[1] = (int16_t)(f->pos[1] + f->vy);
			if (f->pos[1] >= 0) f->vy = (int16_t)-f->vy;
			f->pos[0] = (int16_t)(f->pos[0] + (int16_t)(f->cosv >> 6));
			f->pos[2] = (int16_t)(f->pos[2] + (int16_t)(f->sinv >> 6));
		}
		f->ang[0] = (int16_t)(f->ang[0] + (int16_t)(f->spin[0] << 3));
		f->ang[1] = (int16_t)(f->ang[1] + (int16_t)(f->spin[1] << 3));
		f->ang[2] = (int16_t)(f->ang[2] + (int16_t)(f->spin[2] << 3));
		return true;
	}

	static void LeafDraw(const LeafNode *f)
	{
		uint8_t *h = AllocHeader(0x58);
		*(uint32_t *)h = game_addr(MODEL_Leaf);
		Mat4x3 m = {};
		BuildRotationMatrixFromAngles(f->ang, &m);
		const int32_t v[3] = { f->scale, f->scale, f->scale };
		Scale3DMatrix(&m, v);
		m.t[0] = f->pos[0];
		m.t[1] = f->pos[1];
		m.t[2] = f->pos[2];
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		*(uint32_t *)(h + 0x1C) = 0;
		PacketCursor() = RenderPrimModel(h, RenderOT(0x44), 2, PacketCursor());
		FieldFree(0x58);
	}
}
}

namespace ff8fx
{
namespace aero118
{
	static uint32_t __cdecl LeafTask(TaskNode *n)
	{
		LeafNode *f = (LeafNode *)n;
		if (!LeafMove(f)) return TASK_END;
		LeafDraw(f);
		return 0;
	}

	static uint32_t __cdecl TargetTask(TaskNode *n)
	{
		TargetNode *t = (TargetNode *)n;
		int alive = 1;
		if (t->counter == 8) BdPlaySE3D(SOUND_Aero(), 1, t->pos);
		const int32_t reach = (*(const int16_t *)(t->entity + 0x26) >> 1) + 0x800;
		if (t->counter > 0xA)
		{
			PrimArg arg;
			arg.pos[0] = t->pos[0];
			arg.pos[2] = t->pos[2];
			arg.pos[1] = *(const int16_t *)(t->entity + 0x24);
			arg.pos[3] = 0;
			arg.scale[2] = reach;
			arg.scale[1] = reach;
			arg.scale[0] = reach;
			arg.scaled = 1;
			arg.depth = (int32_t)(0xFFFFF800u - (uint32_t)(int32_t)*(const int16_t *)(t->entity + 0x26)) >> 5;
			arg.morph = TexBase() + 0x22A4;
			alive = prim::play((prim::Layout *)t->whirl, WindPart, (int)&arg, 0);
			GetDefaultEffectPosition(t->entity, arg.pos);
			alive |= prim::play((prim::Layout *)t->gust, WindPart, (int)&arg, 0);
		}
		if ((uint16_t)t->counter < 0x14)
		{
			LeafNode *f = (LeafNode *)AddTaskToQueue(QLeaves(), game_addr(ORIG_LeafTask));
			if (f)
			{
				Memset32(&f->action, 0, 0xC);
				f->action = t->action;
				f->cx = t->pos[0];
				f->cz = t->pos[2];
				const int32_t y = CrtRand() & 0xFF;
				f->owner = t;
				f->pos[1] = (int16_t)y;
				f->reach = (int16_t)reach;
				f->scale = 0x200;
				f->angle = (int16_t)CrtRand();
				f->speed = 0x40;
				f->ang[0] = (int16_t)CrtRand();
				f->ang[1] = (int16_t)CrtRand();
				f->ang[2] = (int16_t)CrtRand();
				f->spin[0] = (int8_t)(CrtRand() + 0x80);
				f->spin[1] = (int8_t)(CrtRand() + 0x80);
				const int32_t s2 = CrtRand();
				f->vy = (int16_t)0xFFFC;
				f->spin[2] = (int8_t)(s2 + 0x80);
			}
		}
		t->counter++;
		if (alive == 0)
		{
			ApplyActionResultToTarget(Ctx()->actions[t->action].targets);
			t->action = -1;
			return TASK_END;
		}
		return 0;
	}

	static uint32_t __cdecl RootTask(TaskNode *n)
	{
		RootNode *r = (RootNode *)n;
		if (r->arena)
		{
			PacketCursor() = TexBase() + 0x2584;
			r->arena = 0;
		}
		else
		{
			PacketCursor() = TexBase() + 0x1A584;
			r->arena = 1;
		}
		if (r->counter == 1)
		{
			if (!r->started)
			{
				r->started = 1;
				InitTaskQueuePool(QTargets(), (void *)TexBase(), 0x68C, 3);
				InitTaskQueuePool(QLeaves(), (void *)(TexBase() + 0x13A4), 0x3C, 0x40);
			}
			ActionData *acts = Ctx()->actions;
			if (r->action <= acts[0].last_action)
			{
				TargetNode *t = (TargetNode *)AddTaskToQueue(QTargets(), game_addr(ORIG_TargetTask));
				Memset32(&t->counter, 0, 0x1A0);
				t->action = (int16_t)(uint16_t)r->action;
				t->entity = Entity(Ctx()->actions[t->action].targets[0]);
				GetDefaultEffectPosition(t->entity, t->pos);
				DecodeModelPrimLayout(game_addr(MODEL_Whirl), t->whirl, 0x4EC);
				DecodeModelPrimLayout(game_addr(MODEL_Gust), t->gust, 0x184);
				r->action++;
			}
		}
		EffectCameraMatrix(&Camera(), &EffectCamera());
		int a, b;
		if (r->started)
		{
			a = ExecuteTaskQueue(QTargets());
			b = ExecuteTaskQueue(QLeaves());
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

	void register_mag118_aero()
	{
		register_port(aero118::ORIG_RootTask, (void *)aero118::RootTask, "A118 RootTask", 118);
		register_port(aero118::ORIG_TargetTask, (void *)aero118::TargetTask, "A118 TargetTask", 118);
		register_port(aero118::ORIG_LeafTask, (void *)aero118::LeafTask, "A118 LeafTask", 118);
	}
}

