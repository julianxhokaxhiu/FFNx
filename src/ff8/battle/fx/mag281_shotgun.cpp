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

#include "mag_common.h"

namespace ff8fx
{
namespace shotgun281
{
	using namespace eng;
	using namespace magc;

	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::g281_CastContext)); }
	inline uint8_t *&Caster() { return var<uint8_t *>(game_addr(game::g281_CasterEntity)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::g281_PacketCursor)); }
	inline Mat4x3 &BasisM() { return var<Mat4x3>(game_addr(game::g281_BasisToTarget)); }
	inline Mat4x3 &BasisR() { return var<Mat4x3>(game_addr(game::g281_BasisToCaster)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::g281_MagicBuffer)); }
	inline TaskQueue *QSparks() { return (TaskQueue *)game_addr(game::g281_SparkQueue); }
	inline TaskQueue *QHits() { return (TaskQueue *)game_addr(game::g281_HitQueue); }
	inline TaskQueue *QMain() { return (TaskQueue *)game_addr(game::g281_MainQueue); }

	static constexpr game::Id ORIG_MasterTask = game::MAG_281_Shotgun_MasterTask;
	static constexpr game::Id ORIG_MainTask = game::MAG_281_Shotgun_MainTask;
	static constexpr game::Id ORIG_SparkTask = game::MAG_281_Shotgun_SparkTask;
	static constexpr game::Id ORIG_HitTask = game::MAG_281_Shotgun_HitTask;
	static const uint32_t POOL_Hits = 0x1400;
	static const uint32_t POOL_Main = 0x1BC8;
	static const uint32_t MORPH = 0x1DE8;
	static const uint32_t ARENA_A = 0x2018;
	static const uint32_t ARENA_B = 0x12018;
	static constexpr game::Id MODEL_Muzzle = game::g281_MuzzleLayout;
	static constexpr game::Id MODEL_Hit = game::g281_HitLayout;
	static constexpr game::Id SEQ_MuzzleSpark = game::g281_MuzzleSparkSeq;
	static constexpr game::Id SEQ_HitSpark = game::g281_HitSparkSeq;
	static const int16_t *MUZZLE() { return (const int16_t *)game_addr(game::g281_MuzzleOffsetBone23); }
	static const void *SOUND_Shot() { return (const void *)game_addr(game::g281_Sound); }

	inline int32_t ComputeCos(int32_t angle) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(angle); }
	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void MatrixMulVector(const Mat4x3 *m, const int16_t *v, int16_t *out) { fn<void (__cdecl *)(const Mat4x3 *, const int16_t *, int16_t *)>(game_addr(game::matrixMultiplyVector))(m, v, out); }
	inline void GteMVMVA_RotV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0))(); }
	inline uint32_t RenderPrim2(void *h, uint32_t ot, int mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int, uint32_t)>(game_addr(game::MAG_070_RenderPrimModel))(h, ot, mode, cursor); }
	inline int32_t BuildOrthonormalBasis(int32_t *v, Mat4x3 *m) { return fn<int32_t (__cdecl *)(int32_t *, Mat4x3 *)>(game_addr(game::BuildOrthonormalBasis))(v, m); }

#pragma pack(push, 1)
	struct MasterNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t pad0E;
		uint8_t started;
		uint32_t arena;
	};
	struct MainNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		uint8_t *ent;
		uint8_t muzzle[0x20C];
	};
	struct HitNode
	{
		TaskNode hdr;
		int16_t pos[3];
		int16_t pad12;
		uint8_t layout[0x138];
	};
	struct SparkNode
	{
		TaskNode hdr;
		uint32_t seq;
		int16_t pos[3];
		int16_t counter;
		int16_t vel[3];
		int16_t life;
		int16_t drag[3];
		int16_t pad26;
	};
	struct PrimArg
	{
		Mat4x3 m;
		int32_t depth;
		uint32_t morph;
	};
#pragma pack(pop)
	static_assert(sizeof(MasterNode) == 0x14 && sizeof(MainNode) == 0x220 && sizeof(HitNode) == 0x14C, "Shotgun nodes");
	static_assert(sizeof(SparkNode) == 0x28, "Shotgun nodes");
	static_assert(sizeof(PrimArg) == 0x28, "Shotgun prim block");

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
		PacketCursor() = RenderPrim2(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static void BonePoint(const int16_t *v, int32_t *out)
	{
		const uint8_t *bones = *(uint8_t *const *)*(uint8_t *const *)(Caster() + 0x64);
		const Mat4x3 *bm = (const Mat4x3 *)(bones + v[3] * 0x30 + 0x20);
		GteSetRotMatrixCtrl(bm);
		GteSetTransVectorCtrl(bm);
		GteLoadV0(v);
		GteMVMVA_RotV0Tr();
		GteReadMAC123(out);
	}

	static int RandomBonePoint(uint8_t *ent, int32_t idx, int16_t *out)
	{
		int32_t *blk = (int32_t *)FieldAlloc(0x14);
		const uint8_t *skel = **(uint8_t *const *const *)(ent + 0x64);
		if (idx >= 0xF0) idx = skel[idx - 0xEC];
		const uint8_t *bone = skel + 0x10 + idx * 0x30;
		if (*(const int16_t *)bone < 0)
		{
			FieldFree(0x14);
			return 1;
		}
		TransformVectorBy3x3Matrix((const Mat4x3 *)(ent + 0x40), (const int32_t *)(bone + 0x24), blk);
		out[0] = (int16_t)(*(const int16_t *)(ent + 0x54) + (int16_t)blk[0]);
		out[1] = (int16_t)(*(const int16_t *)(ent + 0x58) + (int16_t)blk[1]);
		out[2] = (int16_t)(*(const int16_t *)(ent + 0x5C) + (int16_t)blk[2]);
		FieldFree(0x14);
		return 0;
	}

	static void Direction(int32_t a, int32_t b, int16_t *out)
	{
		const int32_t c = ComputeCos(a);
		out[1] = (int16_t)(0 - ComputeSin(a));
		out[0] = (int16_t)(mul32(ComputeSin(b), c) >> 12);
		out[2] = (int16_t)((int32_t)(0u - (uint32_t)mul32(ComputeCos(b), c)) >> 12);
	}

	static void SpriteDraw(int16_t *pos, int16_t frame, uint32_t seq)
	{
		TransformCameraByShadowRotation(pos, 0x320, -200);
		uint8_t *h = AllocHeader(0xB4);
		*(uint32_t *)h = seq;
		*(int16_t *)(h + 4) = frame;
		*(uint16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static void MainArg(PrimArg *arg)
	{
		ComposeAffineTransform(&Camera(), &BasisM(), &arg->m);
		arg->depth = -0x80;
		arg->morph = TexBase() + MORPH;
	}
}
}

namespace ff8fx
{
namespace shotgun281
{
	static uint32_t __cdecl SparkTask(TaskNode *n)
	{
		SparkNode *s = (SparkNode *)n;
		SpriteDraw(s->pos, s->counter, s->seq);
		s->pos[0] = (int16_t)(s->pos[0] + s->vel[0]);
		s->pos[1] = (int16_t)(s->pos[1] + s->vel[1]);
		s->pos[2] = (int16_t)(s->pos[2] + s->vel[2]);
		s->counter++;
		s->vel[1] = (int16_t)(s->drag[1] + s->vel[1]);
		s->vel[0] = (int16_t)(s->drag[0] + s->vel[0]);
		s->vel[2] = (int16_t)(s->drag[2] + s->vel[2]);
		return s->counter < s->life ? 0 : TASK_END;
	}

	static uint32_t __cdecl HitTask(TaskNode *n)
	{
		HitNode *h = (HitNode *)n;
		BasisR().t[0] = h->pos[0];
		BasisR().t[1] = h->pos[1];
		BasisR().t[2] = h->pos[2];
		PrimArg arg;
		ComposeAffineTransform(&Camera(), &BasisR(), &arg.m);
		arg.depth = -0x80;
		arg.morph = TexBase() + MORPH;
		return prim::play((prim::Layout *)h->layout, PrimCallback, (int)&arg, 0) ? 0 : TASK_END;
	}

	static uint32_t __cdecl MainTask(TaskNode *n)
	{
		MainNode *m = (MainNode *)n;
		const int32_t c = m->counter;
		if ((uint32_t)c < 0xE)
		{
			BonePoint(MUZZLE(), BasisM().t);
			PrimArg arg;
			MainArg(&arg);
			prim::play((prim::Layout *)m->muzzle, PrimCallback, (int)&arg, 0);
			if ((uint32_t)c >= 1 && (uint32_t)c <= 2)
			{
				for (int k = 0; k < 0x10; k++)
				{
					SparkNode *s = (SparkNode *)AddTaskToQueue(QSparks(), game_addr(ORIG_SparkTask));
					if (!s) break;
					s->pos[0] = (int16_t)BasisM().t[0];
					s->pos[1] = (int16_t)BasisM().t[1];
					s->pos[2] = (int16_t)BasisM().t[2];
					const int32_t angle = CrtRand();
					const int32_t radius = CrtRand() & 0x7F;
					int16_t v[3];
					v[0] = (int16_t)(mul32(ComputeSin(angle), radius) >> 12);
					v[1] = (int16_t)(mul32(ComputeCos(angle), radius) >> 12);
					v[2] = (int16_t)(-0x80 - (CrtRand() & 0x7F));
					s->counter = 0;
					s->life = 8;
					s->seq = game_addr(SEQ_MuzzleSpark);
					MatrixMulVector(&BasisM(), v, s->vel);
					s->drag[0] = (int16_t)((int32_t)(0 - (int32_t)s->vel[0]) >> 3);
					s->drag[1] = (int16_t)((int32_t)(0 - (int32_t)s->vel[1]) >> 3);
					s->drag[2] = (int16_t)((int32_t)(0 - (int32_t)s->vel[2]) >> 3);
				}
			}
		}
		if (c == 1) ScreenFadeTask(0, 1, 0, 0x60);
		if ((uint32_t)(m->counter - 4) < 8)
		{
			HitNode *h = (HitNode *)AddTaskToQueue(QHits(), game_addr(ORIG_HitTask));
			if (h)
			{
				do
				{
					uint8_t *ent = m->ent;
					const int32_t idx = mul32(CrtRand(), ***(const uint8_t *const *const *)(ent + 0x64)) >> 15;
					if (!RandomBonePoint(ent, idx, h->pos)) break;
				} while (true);
				h->pad12 = 0;
				DecodeModelPrimLayout(game_addr(MODEL_Hit), h->layout, 0x138);
				for (int k = 0; k < 8; k++)
				{
					SparkNode *s = (SparkNode *)AddTaskToQueue(QSparks(), game_addr(ORIG_SparkTask));
					if (!s) break;
					ff8fx::copy<(4)>(s->pos, h->pos);
					ff8fx::copy<(4)>(&s->pos[2], &h->pos[2]);
					const int32_t b = CrtRand();
					const int32_t a = CrtRand();
					Direction(a, b, s->vel);
					const int32_t scale = (CrtRand() & 0x3F) + 0x20;
					s->counter = 0;
					s->life = 0xA;
					s->vel[0] = (int16_t)(mul32(s->vel[0], scale) >> 11);
					s->seq = game_addr(SEQ_HitSpark);
					s->vel[1] = (int16_t)(mul32(s->vel[1], scale) >> 11);
					s->vel[2] = (int16_t)(mul32(s->vel[2], scale) >> 11);
					s->drag[0] = (int16_t)((int32_t)(0 - (int32_t)s->vel[0]) >> 3);
					s->drag[1] = (int16_t)((int32_t)(0 - (int32_t)s->vel[1]) >> 3);
					s->drag[2] = (int16_t)((int32_t)(0 - (int32_t)s->vel[2]) >> 3);
				}
			}
		}
		if (m->counter == 2) BdPlaySE(SOUND_Shot(), 0, 0x80);
		m->counter++;
		if (m->counter >= 0xE)
		{
			ApplyActionResultToTarget(Ctx()->actions[0].targets);
			return TASK_END;
		}
		return 0;
	}

	static uint32_t __cdecl MasterTask(TaskNode *n)
	{
		MasterNode *m = (MasterNode *)n;
		if (m->arena)
		{
			PacketCursor() = TexBase() + ARENA_A;
			m->arena = 0;
		}
		else
		{
			PacketCursor() = TexBase() + ARENA_B;
			m->arena = 1;
		}
		if (m->counter == 1 && !m->started)
		{
			m->started = 1;
			InitTaskQueuePool(QMain(), (void *)(TexBase() + POOL_Main), 0x220, 1);
			InitTaskQueuePool(QHits(), (void *)(TexBase() + POOL_Hits), 0x14C, 6);
			InitTaskQueuePool(QSparks(), (void *)TexBase(), 0x28, 0x80);
			MainNode *mn = (MainNode *)AddTaskToQueue(QMain(), game_addr(ORIG_MainTask));
			Memset32(&mn->counter, 0, 0x85);
			mn->ent = Entity(Ctx()->actions[0].targets[0]);
			DecodeModelPrimLayout(game_addr(MODEL_Muzzle), mn->muzzle, 0x20C);
			uint8_t *caster = Caster();
			int32_t v[3];
			v[0] = (int32_t)*(const int16_t *)(mn->ent + 0x1C) - *(const int16_t *)(caster + 0x1C);
			v[1] = 0;
			v[2] = (int32_t)*(const int16_t *)(caster + 0x20) - *(const int16_t *)(mn->ent + 0x20);
			BuildOrthonormalBasis(v, &BasisM());
			v[0] = (int32_t)(0u - (uint32_t)v[0]);
			v[2] = (int32_t)(0u - (uint32_t)v[2]);
			BuildOrthonormalBasis(v, &BasisR());
		}
		int a0, a2;
		if (m->started)
		{
			uint8_t *caster = Caster();
			ComputeBonesWorldMatrices(caster + 0x60, caster + 0x40);
			a0 = ExecuteTaskQueue(QMain());
			BuildBoneMatricesFromPose(Caster() + 0x60);
			ExecuteTaskQueue(QHits());
			a2 = ExecuteTaskQueue(QSparks());
		}
		else a0 = a2 = (int)n;
		if (m->started && a0 == 0 && a2 == 0) return TASK_END;
		m->counter++;
		return 0;
	}
}

	void register_mag281_shotgun()
	{
		register_port(shotgun281::ORIG_MasterTask, (void *)shotgun281::MasterTask, "281 MasterTask", 281);
		register_port(shotgun281::ORIG_MainTask, (void *)shotgun281::MainTask, "281 MainTask", 281);
		register_port(shotgun281::ORIG_HitTask, (void *)shotgun281::HitTask, "281 HitTask", 281);
		register_port(shotgun281::ORIG_SparkTask, (void *)shotgun281::SparkTask, "281 SparkTask", 281);
	}
}

