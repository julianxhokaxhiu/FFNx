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

// Effect 288: Rapture

#include "mag_common.h"

namespace ff8fx
{
namespace rapture288
{
	using namespace eng;
	using namespace magc;

	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::unk_2507C44)); }
	inline TaskQueue *QSparkle() { return (TaskQueue *)game_addr(game::unk_2507C48); }
	inline TaskQueue *QFeather() { return (TaskQueue *)game_addr(game::unk_2507C58); }
	inline TaskQueue *QTarget() { return (TaskQueue *)game_addr(game::unk_2507C68); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::unk_2507CA0)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::unk_2507CA4)); }

	static constexpr game::Id ORIG_RootTask = game::MAG_288_RAPTURE_RootTask;
	static constexpr game::Id ORIG_TargetTask = game::MAG_288_RAPTURE_TargetTask;
	static constexpr game::Id ORIG_FeatherTask = game::MAG_288_RAPTURE_FeatherTask;
	static constexpr game::Id ORIG_SparkleTask = game::MAG_288_RAPTURE_SparkleTask;
	static constexpr game::Id LAYOUT_A = game::unk_10749A8;
	static constexpr game::Id LAYOUT_B = game::unk_1075800;
	static constexpr game::Id FEATHER_MODEL = game::unk_10747B0;
	static constexpr game::Id SEQUENCE = game::unk_1074658;
	static const void *SOUND() { return (const void *)game_addr(game::unk_1078970); }
	static constexpr game::Id DEPTH_VEC = game::unk_1078978;
	static constexpr game::Id COLOURS = game::unk_1078980;
	static const uint32_t ARENA_A = 0x2460;
	static const uint32_t ARENA_B = 0x1A460;
	static const uint32_t MORPH = 0x2340;
	static const int32_t TARGET_END = 0x4A, SPARKLE_FRAMES = 12;

	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t flag, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, flag, out); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void RotationFromAnglesB(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::MAG_063_RotationFromAnglesYXZ))(angles, out); }
	inline void RotationY(int16_t angle, Mat4x3 *out) { fn<void (__cdecl *)(int32_t, Mat4x3 *)>(game_addr(game::Effect_RotationYMatrix))((int32_t)(uint16_t)angle, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
	inline uint32_t RenderPrimModel2(void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::MAG_070_RenderPrimModel))(h, ot, mode, cursor); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void GteMVMVA_RotV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0))(); }
	inline void MatrixMultiplyVector(const Mat4x3 *m, const int16_t *in, int16_t *out) { fn<void (__cdecl *)(const Mat4x3 *, const int16_t *, int16_t *)>(game_addr(game::matrixMultiplyVector))(m, in, out); }
	inline void BlendColour(const void *a, const void *b, int32_t wa, int32_t wb, void *out) { fn<void (__cdecl *)(const void *, const void *, int32_t, int32_t, void *)>(game_addr(game::ColourLerp))(a, b, wa, wb, out); }
	inline int32_t ComputeCos(int32_t angle) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(angle); }
	inline uint32_t mulhi_u(uint32_t a, uint32_t b) { return (uint32_t)(((uint64_t)a * b) >> 32); }

#pragma pack(push, 1)
	struct RootNode
	{
		TaskNode hdr;
		int16_t wrap;
		int16_t ticks;
		uint8_t next;
		uint8_t started;
		uint8_t pad12[2];
		uint32_t arena;
	};
	struct TargetNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t saved_y;
		uint8_t flag;
		uint8_t pad11[3];
		uint8_t *entity;
		uint8_t layout_a[0xFC];
		uint8_t layout_b[0x30C];
	};
	struct FeatherNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t scale;
		int16_t pos[3];
		int16_t vy;
		int16_t ang[3];
		int16_t pad1E;
		int8_t spin[3];
		uint8_t pad23;
	};
	struct SparkleNode
	{
		TaskNode hdr;
		int16_t pos[3];
		int16_t frame;
	};
	struct PrimArg
	{
		Mat4x3 m;
		int32_t scale[3];
		int32_t scaled;
		int32_t depth;
		uint32_t morph;
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x18 && sizeof(TargetNode) == 0x420 && sizeof(FeatherNode) == 0x24 && sizeof(SparkleNode) == 0x14, "Rapture nodes");
	static_assert(sizeof(PrimArg) == 0x38, "Rapture block");

	inline int16_t &S16(uint8_t *p, int o) { return *(int16_t *)(p + o); }

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
		const int32_t scaled = arg->scaled;
		int16_t off[4] = {};
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
		m.t[1] = (int32_t)((uint32_t)m.t[1] + (uint32_t)arg->m.t[1]);
		m.t[0] = (int32_t)((uint32_t)m.t[0] + (uint32_t)arg->m.t[0]);
		m.t[2] = (int32_t)((uint32_t)m.t[2] + (uint32_t)arg->m.t[2]);
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
		PacketCursor() = RenderPrimModel2(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static int32_t TargetScale(int16_t size) { return ((((int32_t)size >> 1) + 0x800) >> 2); }

	static void BlockA(uint8_t *e, int16_t facing, PrimArg *a)
	{
		RotationY((int16_t)-facing, &a->m);
		int16_t p[3];
		GetEffectSpawnPosition(e, 0xF1, 0, p);
		a->m.t[0] = p[0];
		a->m.t[2] = p[2];
		a->m.t[1] = S16(e, 0x24);
		ComposeAffineTransform(&Camera(), &a->m, &a->m);
	}
	static void BlockB(uint8_t *e, int16_t facing, PrimArg *a)
	{
		RotationY((int16_t)-facing, &a->m);
		int16_t p[3];
		GetEffectSpawnPosition(e, 0xF0, 0, p);
		a->m.t[0] = p[0];
		a->m.t[1] = p[1];
		a->m.t[2] = p[2];
		ComposeAffineTransform(&Camera(), &a->m, &a->m);
		int16_t v[3];
		MatrixMultiplyVector(&a->m, (const int16_t *)game_addr(DEPTH_VEC), v);
		a->depth = mul32((int32_t)S16(e, 0x26) + 0x800, v[2]) >> 15;
	}

	static void FeatherDraw(const FeatherNode *f)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		Mat4x3 m = {};
		BuildRotationMatrixFromAngles(f->ang, &m);
		m.t[0] = f->pos[0];
		m.t[1] = f->pos[1];
		m.t[2] = f->pos[2];
		const int32_t v[3] = { f->scale, f->scale, f->scale };
		Scale3DMatrix(&m, v);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		*(uint32_t *)h = game_addr(FEATHER_MODEL);
		*(uint32_t *)(h + 0x1C) = 0;
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}
	static void SparkleDraw(const SparkleNode *s)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		TransformCameraByShadowRotation(s->pos, 0x1000, -0x400);
		*(uint32_t *)h = game_addr(SEQUENCE);
		*(int16_t *)(h + 4) = s->frame;
		*(int16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
		FieldFree(0xB4);
	}
}
}

namespace ff8fx
{
namespace rapture288
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
		SparkleDraw(s);
		s->pos[1] = (int16_t)(s->pos[1] + 0x1E);
		s->frame++;
		return s->frame < SPARKLE_FRAMES ? 0 : TASK_END;
	}

	static uint32_t __cdecl FeatherTask(TaskNode *n)
	{
		FeatherNode *f = (FeatherNode *)n;
		FeatherDraw(f);
		f->ang[0] = (int16_t)(f->ang[0] + (int16_t)f->spin[0]);
		f->ang[1] = (int16_t)(f->ang[1] + (int16_t)f->spin[1]);
		f->ang[2] = (int16_t)(f->ang[2] + (int16_t)f->spin[2]);
		f->pos[1] = (int16_t)(f->pos[1] + f->vy);
		f->counter++;
		if (f->counter >= 10)
		{
			f->scale = (int16_t)(f->scale - 0x100);
			if (f->scale < 0x100) return TASK_END;
		}
		return 0;
	}

	static uint32_t __cdecl TargetTask(TaskNode *n)
	{
		TargetNode *t = (TargetNode *)n;
		uint8_t *e = t->entity;
		int16_t def[4];
		GetDefaultEffectPosition(e, def);
		const int32_t s = TargetScale(S16(e, 0x26));
		PrimArg arg;
		arg.scale[0] = arg.scale[1] = arg.scale[2] = s;
		arg.scaled = 1;
		const int32_t c = t->counter;
		if ((uint32_t)(c - 4) < 0x1A)
		{
			if ((uint32_t)(c - 4) < 4)
			{
				for (int k = 0; k < 2; k++)
				{
					FeatherNode *f = (FeatherNode *)AddTaskToQueue(QFeather(), game_addr(ORIG_FeatherTask));
					if (!f) continue;
					f->counter = 0;
					f->scale = (int16_t)((CrtRand() & 0x7FF) + 0x800);
					const int32_t a = CrtRand();
					const int32_t rad = mul32(CrtRand() + 0x10000, s) >> 16;
					f->pos[0] = (int16_t)((mul32(ComputeSin(a), rad) >> 12) + def[0]);
					f->pos[1] = 0;
					f->pos[2] = (int16_t)((mul32(ComputeCos(a), rad) >> 12) + def[2]);
					f->vy = (int16_t)((int32_t)(0u - (uint32_t)mul32(CrtRand() + 0x8000, s)) >> 18);
					f->ang[0] = (int16_t)CrtRand();
					f->ang[1] = (int16_t)CrtRand();
					f->ang[2] = (int16_t)CrtRand();
					f->spin[0] = (int8_t)(CrtRand() + 0x80);
					f->spin[1] = (int8_t)(CrtRand() + 0x80);
					f->spin[2] = (int8_t)(CrtRand() + 0x80);
				}
			}
			BlockA(e, S16(e, 0xE), &arg);
			arg.morph = TexBase() + MORPH;
			arg.depth = 0;
			prim::play((prim::Layout *)t->layout_a, PrimCallback, (int)&arg, 0);
		}
		if ((uint32_t)(c - 8) < 0x42 && !(*(const uint16_t *)e & 4) && (*(const uint16_t *)e & 2))
		{
			BlockB(e, S16(e, 0xE), &arg);
			arg.morph = TexBase() + MORPH;
			prim::play((prim::Layout *)t->layout_b, PrimCallback, (int)&arg, 0);
		}
		if (c == 0) BdPlaySE(SOUND(), 0, 0x80);
		if ((uint32_t)c < 0x10 && !(c & 1))
		{
			SparkleNode *p = (SparkleNode *)AddTaskToQueue(QSparkle(), game_addr(ORIG_SparkleTask));
			if (p)
			{
				const int32_t a = CrtRand();
				const int32_t rad = mul32(CrtRand() + 0x10000, s) >> 16;
				p->pos[0] = (int16_t)((mul32(ComputeSin(a), rad) >> 12) + def[0]);
				p->pos[2] = (int16_t)((mul32(ComputeCos(a), rad) >> 12) + def[2]);
				p->pos[1] = (int16_t)((int32_t)(0u - (uint32_t)mul32(CrtRand() + 0x8000, s)) >> 15);
				p->frame = 0;
			}
		}
		const uint32_t k = (uint32_t)(c - 0x17);
		if (k < 0x28)
		{
			const int32_t w = (int32_t)(mulhi_u(k << 13, 0xCCCCCCCDu) >> 5);
			e[1] |= 0x10;
			if (t->flag) S16(e, 0x1E) = (int16_t)((mul32(ComputeCos(w) - 0x1000, s) >> 14) + t->saved_y);
			else
			{
				const int32_t a = ComputeCos(w >> 2);
				const int32_t b = ComputeCos((0x1000 - a) >> 2);
				const int32_t up = mul32(0x1000 - b, s);
				const int32_t bob = mul32(ComputeCos(w) - 0x1000, s);
				S16(e, 0x1E) = (int16_t)((bob >> 12) - (up >> 10) + t->saved_y);
			}
		}
		else if (k == 0x28)
		{
			*(uint16_t *)e &= 0xEFFF;
			S16(e, 0x1E) = t->saved_y;
			ApplyResult(e);
			if (!t->flag) e[0] |= 4;
		}
		if (t->counter >= TARGET_END) return TASK_END;
		t->counter++;
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
		if (r->wrap == 1)
		{
			if (!r->started)
			{
				r->started = 1;
				InitTaskQueuePool(QTarget(), (void *)TexBase(), 0x420, 6);
				InitTaskQueuePool(QFeather(), (void *)(TexBase() + 0x18C0), 0x24, 0x30);
				InitTaskQueuePool(QSparkle(), (void *)(TexBase() + 0x1F80), 0x14, 0x30);
			}
			if (r->next < Ctx()->actions[0].target_count)
			{
				TargetNode *t = (TargetNode *)AddTaskToQueue(QTarget(), game_addr(ORIG_TargetTask));
				if (t)
				{
					Memset32((uint8_t *)t + 0xC, 0, 0x105);
					const uint8_t *rec = Ctx()->actions[0].targets + r->next * TARGET_STRIDE;
					t->entity = Entity(rec[0]);
					t->flag = rec[3] & 4;
					t->saved_y = S16(t->entity, 0x1E);
					DecodeModelPrimLayout(game_addr(LAYOUT_A), t->layout_a, 0xFC);
					DecodeModelPrimLayout(game_addr(LAYOUT_B), t->layout_b, 0x30C);
				}
				r->next++;
			}
		}
		uint32_t level = 0, colour = 0;
		const uint32_t k = (uint32_t)((int32_t)r->ticks - 1);
		if (k < 0x4A)
		{
			if (k < 0xC) level = mulhi_u((k * 3) << 10, 0xAAAAAAABu) >> 3;
			else if (k >= 0x3E) level = mulhi_u(0x37800u - ((k * 3) << 10), 0xAAAAAAABu) >> 3;
			else level = 0xC00;
			const uint32_t x = (k * 5) << 12;
			const uint32_t q = mulhi_u(x, 0xBACF914Du);
			const uint32_t w = (((x - q) >> 1) + q) >> 6;
			const int32_t idx = (int32_t)w >> 12, frac = (int32_t)(w & 0xFFF);
			BlendColour((const void *)(game_addr(COLOURS) + idx * 4), (const void *)(game_addr(COLOURS) + 4 + idx * 4), 0x1000 - frac, frac, &colour);
		}
		int a;
		if (r->started)
		{
			a = ExecuteTaskQueue(QTarget());
			ExecuteTaskQueue(QFeather());
			ExecuteTaskQueue(QSparkle());
		}
		else a = (int)colour;
		SetScreenFlash(level, colour);
		if (r->started && a == 0)
		{
			SetScreenFlash(0, 0);
			return TASK_END;
		}
		r->wrap++;
		if (r->wrap >= 10) r->wrap = 0;
		r->ticks++;
		return 0;
	}
}

	void register_mag288_rapture()
	{
		register_port(rapture288::ORIG_RootTask, (void *)rapture288::RootTask, "288 RootTask", 288);
		register_port(rapture288::ORIG_TargetTask, (void *)rapture288::TargetTask, "288 TargetTask", 288);
		register_port(rapture288::ORIG_FeatherTask, (void *)rapture288::FeatherTask, "288 FeatherTask", 288);
		register_port(rapture288::ORIG_SparkleTask, (void *)rapture288::SparkleTask, "288 SparkleTask", 288);
	}
}

