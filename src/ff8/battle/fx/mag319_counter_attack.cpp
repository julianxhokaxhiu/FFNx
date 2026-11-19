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

// Effect 319: Counter-attack

#include "mag_common.h"
#include <stddef.h>

namespace ff8fx
{
namespace counter319
{
	using namespace eng;
	using namespace magc;

	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_319_CounterAttack_PacketCursor)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::MAG_319_CounterAttack_CastContext)); }
	inline uint8_t *&Caster() { return var<uint8_t *>(game_addr(game::MAG_319_CounterAttack_CasterEntity)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::MAG_319_CounterAttack_MagicBufferBase)); }
	inline TaskQueue *QSprites() { return (TaskQueue *)game_addr(game::MAG_319_CounterAttack_SpritesQueue); }
	inline TaskQueue *QBones() { return (TaskQueue *)game_addr(game::MAG_319_CounterAttack_BonesQueue); }
	inline TaskQueue *QMain() { return (TaskQueue *)game_addr(game::MAG_319_CounterAttack_MainQueue); }

	static constexpr game::Id ORIG_RootTask = game::MAG_319_CounterAttack_RootTask;
	static constexpr game::Id ORIG_MainTask = game::MAG_319_CounterAttack_MainTask;
	static constexpr game::Id ORIG_SpriteTask = game::MAG_319_CounterAttack_SpriteTask;
	static constexpr game::Id ORIG_BoneScaleTask = game::MAG_318_StrongCounterAttack_BoneScaleTask;
	static const uint32_t POOL_Bones = 0x898, POOL_Sprites = 0x924;
	static const uint32_t STREAM_STATE = 0xAA4;
	static const uint32_t MORPH = 0xAEC;
	static const uint32_t ARENA_A = 0xD1C;
	static const uint32_t ARENA_B = 0x10D1C;
	static const uint32_t STREAM_DEST = 0x20D1C;
	static constexpr game::Id MODEL_A = game::MAG_319_CounterAttack_ModelA;
	static constexpr game::Id MODEL_B = game::MAG_319_CounterAttack_ModelB;
	static constexpr game::Id PRIM_Spike = game::unk_FF13F0;
	static constexpr game::Id SEQ_Sprite = game::MAG_319_CounterAttack_SeqSprite;
	static const void *SOUND() { return (const void *)game_addr(game::unk_FF75CC); }
	static const int16_t *OFFSETS() { return (const int16_t *)game_addr(game::unk_FF7780); }

	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t flag, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, flag, out); }
	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void MulMatrix3(Mat4x3 *a, const void *b) { fn<void (__cdecl *)(Mat4x3 *, const void *)>(game_addr(game::MatrixMultiply3x3))(a, b); }
	inline void GteMVMVA_RotV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0))(); }
	inline uint32_t RenderPrim2(void *h, uint32_t ot, int mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int, uint32_t)>(game_addr(game::MAG_070_RenderPrimModel))(h, ot, mode, cursor); }
	inline int32_t BuildOrthonormalBasis(const int32_t *v, Mat4x3 *m) { return fn<int32_t (__cdecl *)(const int32_t *, Mat4x3 *)>(game_addr(game::BuildOrthonormalBasis))(v, m); }
	inline void MatrixMulVector(const Mat4x3 *m, const int16_t *v, int16_t *out) { fn<void (__cdecl *)(const Mat4x3 *, const int16_t *, int16_t *)>(game_addr(game::matrixMultiplyVector))(m, v, out); }
	inline void VecLerp(const void *a, const void *b, int32_t wa, int32_t wb, void *out) { fn<void (__cdecl *)(const void *, const void *, int32_t, int32_t, void *)>(game_addr(game::VecLerp))(a, b, wa, wb, out); }
	inline void StreamLoad(int32_t id, uint32_t dst, int32_t kind) { fn<void (__cdecl *)(int32_t, uint32_t, int32_t)>(game_addr(game::Stream_QueueLoad))(id, dst, kind); }
	inline void StreamPump() { fn<void (__cdecl *)()>(game_addr(game::Stream_Pump))(); }
	inline int32_t StreamDone(int32_t a) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::BattleFile_PreLoad_IsLoaded))(a); }

#pragma pack(push, 1)
	struct RootNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t started;
		uint8_t pad0F;
		uint32_t arena;
	};
	struct MainNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t pad0E[0x4A];
		uint8_t *ent;
		int16_t spawn[4];
		int16_t pos[4];
		int16_t off[3][4];
		Mat4x3 M;
		Mat4x3 M2;
		uint8_t layout_a[0x368];
		uint8_t layout_b[0x46C];
	};
	struct BoneNode
	{
		TaskNode hdr;
		uint8_t *bone;
		int16_t delay;
		int16_t sy;
		int16_t dy;
		int16_t sx;
		int16_t dx;
		int16_t pad1A;
	};
	struct SpriteNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		int16_t pos[3];
		int16_t pad16;
	};
	struct PrimArg
	{
		Mat4x3 m;
		int32_t depth;
		uint32_t morph;
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x14 && sizeof(MainNode) == 0x898 && sizeof(BoneNode) == 0x1C, "Counter-attack nodes");
	static_assert(sizeof(SpriteNode) == 0x18 && sizeof(PrimArg) == 0x28, "Counter-attack nodes");
	static_assert(offsetof(MainNode, ent) == 0x58 && offsetof(MainNode, M) == 0x84 && offsetof(MainNode, layout_a) == 0xC4, "Counter-attack main node");

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
		RotationFromAngles(r->rot, &m);
		int16_t off[4] = { r->pos[0], r->pos[1], r->pos[2], 0 };
		if (r->flags & 0x200)
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

	static void SpikePoint(const MainNode *m, int32_t c2, int16_t *L)
	{
		const uint32_t t = ((uint32_t)c2 << 12) / 6u;
		VecLerp(m->pos, m->spawn, (int32_t)t, (int32_t)(0x1000 - t), L);
	}

	static void SpikeDraw(Mat4x3 *M, const int16_t *L, PrimArg *arg)
	{
		M->t[0] = L[0];
		M->t[1] = L[1];
		M->t[2] = L[2];
		ComposeAffineTransform(&Camera(), M, &arg->m);
		GteSetRotMatrix(&arg->m);
		GteSetTransVector(&arg->m);
		uint8_t *h = AllocHeader(0x58);
		*(uint32_t *)h = game_addr(PRIM_Spike);
		*(uint32_t *)(h + 0x1C) = 0;
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static void SpriteDraw(const int16_t *pos, int16_t frame)
	{
		uint8_t *h = AllocHeader(0xB4);
		TransformCameraByShadowRotation(pos, 0x1000, -0x400);
		*(uint32_t *)h = game_addr(SEQ_Sprite);
		*(int16_t *)(h + 4) = frame;
		*(uint16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
		FieldFree(0xB4);
	}
}
}

namespace ff8fx
{
namespace counter319
{
	static uint32_t __cdecl SpriteTask(TaskNode *n)
	{
		SpriteNode *s = (SpriteNode *)n;
		SpriteDraw(s->pos, s->counter);
		s->counter++;
		return s->counter < 0xA ? 0 : TASK_END;
	}

	static void StartBoneScale(int32_t bone, int16_t delay, int16_t dx, int16_t dy)
	{
		BoneNode *b = (BoneNode *)AddTaskToQueue(QBones(), game_addr(ORIG_BoneScaleTask));
		b->bone = **(uint8_t ***)(Caster() + 0x64) + bone * 0x30 + 0x10;
		b->delay = delay;
		b->dx = dx;
		b->dy = dy;
		b->sy = 0;
		b->sx = 0;
	}

	static uint32_t __cdecl MainTask(TaskNode *n)
	{
		MainNode *m = (MainNode *)n;
		PrimArg arg;
		if (m->counter == 2)
		{
			GetEffectSpawnPosition(Caster(), 0xD, 0x800, m->spawn);
			int32_t v[3] = { (int32_t)m->pos[0] - m->spawn[0], (int32_t)m->spawn[1] - m->pos[1], (int32_t)m->spawn[2] - m->pos[2] };
			BuildOrthonormalBasis(v, &m->M);
			MatrixMulVector(&m->M, OFFSETS(), m->off[0]);
			MatrixMulVector(&m->M, OFFSETS() + 4, m->off[1]);
			MatrixMulVector(&m->M, OFFSETS() + 8, m->off[2]);
			v[0] = 0 - v[0];
			v[2] = 0 - v[2];
			BuildOrthonormalBasis(v, &m->M2);
		}
		const int32_t c2 = m->counter - 2;
		if ((uint32_t)c2 <= 6)
		{
			if (c2 == 0) *(uint32_t *)(Caster() + 0x7C) &= ~2u;
			int16_t L[4];
			SpikePoint(m, c2, L);
			m->M.t[0] = (int32_t)m->off[0][0] + L[0];
			m->M.t[1] = (int32_t)m->off[0][1] + L[1];
			m->M.t[2] = (int32_t)m->off[0][2] + L[2];
			ComposeAffineTransform(&Camera(), &m->M, &arg.m);
			arg.morph = TexBase() + MORPH;
			arg.depth = -0x80;
			prim::play((prim::Layout *)m->layout_a, PrimCallback, (int)&arg, 0);
			SpikeDraw(&m->M, L, &arg);
		}
		const int32_t c7 = m->counter - 7;
		if ((uint32_t)c7 < 0x20)
		{
			m->M2.t[0] = (int32_t)m->pos[0] - m->off[0][0];
			m->M2.t[1] = (int32_t)m->pos[1] - m->off[0][1];
			m->M2.t[2] = (int32_t)m->pos[2] - m->off[0][2];
			ComposeAffineTransform(&Camera(), &m->M2, &arg.m);
			arg.morph = TexBase() + MORPH;
			arg.depth = -0x80;
			prim::play((prim::Layout *)m->layout_b, PrimCallback, (int)&arg, 0);
			if ((uint32_t)c7 >= 8 && (uint32_t)c7 < 0xF && !(c7 & 1))
			{
				for (int k = 4; k != 0; k--)
				{
					SpriteNode *s = (SpriteNode *)AddTaskToQueue(QSprites(), game_addr(ORIG_SpriteTask));
					if (!s) continue;
					s->counter = 0;
					s->pos[0] = (int16_t)((CrtRand() & 0x3FF) + m->pos[0] - 0x200);
					s->pos[1] = (int16_t)((CrtRand() & 0x3FF) + m->pos[1] - 0x200);
					s->pos[2] = (int16_t)((CrtRand() & 0x3FF) + m->pos[2] - 0x200);
				}
			}
		}
		if (m->counter == 1) BdPlaySE(SOUND(), 0, 0x80);
		if (m->counter == 0xC) ApplyActionResultToTarget(Ctx()->actions[0].targets);
		if (m->counter == 0x1B)
		{
			*(uint32_t *)(Caster() + 0x7C) |= 2u;
			StartBoneScale(0xA, 0, 0x200, 0x400);
			StartBoneScale(0xD, 8, 0x200, 0x400);
			StartBoneScale(0x17, 0x10, 0x200, 0x400);
			StartBoneScale(0x18, 0x10, 0x200, 0x400);
			StartBoneScale(0x19, 0x10, 0x200, 0x400);
		}
		m->counter++;
		if (m->counter >= 0x27)
		{
			StreamLoad(0x168, TexBase() + STREAM_DEST, 0);
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
		if (r->counter == 1)
		{
			if (!r->started)
			{
				r->started = 1;
				InitTaskQueuePool(QMain(), (void *)TexBase(), 0x898, 1);
				InitTaskQueuePool(QBones(), (void *)(TexBase() + POOL_Bones), 0x1C, 5);
				InitTaskQueuePool(QSprites(), (void *)(TexBase() + POOL_Sprites), 0x18, 0x10);
			}
			MainNode *m = (MainNode *)AddTaskToQueue(QMain(), game_addr(ORIG_MainTask));
			Memset32(&m->counter, 0, 0x223);
			m->ent = Entity(Ctx()->actions[0].targets[0]);
			GetDefaultEffectPosition(m->ent, m->pos);
			DecodeModelPrimLayout(game_addr(MODEL_A), m->layout_a, 0x368);
			DecodeModelPrimLayout(game_addr(MODEL_B), m->layout_b, 0x46C);
			StreamStateInit((void *)(TexBase() + STREAM_STATE));
		}
		int a0, a1, a2;
		if (r->started)
		{
			a0 = ExecuteTaskQueue(QMain());
			a1 = ExecuteTaskQueue(QBones());
			a2 = ExecuteTaskQueue(QSprites());
		}
		else a0 = a1 = a2 = (int)n;
		if (r->counter >= 1) StreamPump();
		if (r->started)
		{
			if (a0 == 0 && a1 == 0 && a2 == 0 && StreamDone(a2) != 0) return TASK_END;
			r->counter = 0x40;
		}
		r->counter++;
		return 0;
	}
}

	void register_mag319_counter_attack()
	{
		register_port(counter319::ORIG_RootTask, (void *)counter319::RootTask, "319 RootTask", 319);
		register_port(counter319::ORIG_MainTask, (void *)counter319::MainTask, "319 MainTask", 319);
		register_port(counter319::ORIG_SpriteTask, (void *)counter319::SpriteTask, "319 SpriteTask", 319);
	}
}

