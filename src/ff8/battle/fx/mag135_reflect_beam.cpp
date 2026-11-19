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

// Effect 135: Reflect Beam

#include "mag_common.h"

namespace ff8fx
{
namespace reflect135
{
	using namespace eng;
	using namespace magc;

	inline uint8_t *&Caster() { return var<uint8_t *>(game_addr(game::g_ReflectBeam_Caster)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::g_ReflectBeam_MagicBuffer)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::g_ReflectBeam_CastContext)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::g_ReflectBeam_PacketCursor)); }
	inline Mat4x3 &Scale() { return var<Mat4x3>(game_addr(game::g_ReflectBeam_BeamScale)); }
	inline TaskQueue *QSprites() { return (TaskQueue *)game_addr(game::g_ReflectBeam_SpriteQueue); }
	inline TaskQueue *QReflects() { return (TaskQueue *)game_addr(game::g_ReflectBeam_ReflectQueue); }
	inline TaskQueue *QShots() { return (TaskQueue *)game_addr(game::g_ReflectBeam_ShotQueue); }
	inline TaskQueue *QSweep() { return (TaskQueue *)game_addr(game::g_ReflectBeam_SweepQueue); }

	static constexpr game::Id ORIG_MasterTask = game::MAG_135_ReflectBeam_MasterTask;
	static constexpr game::Id ORIG_SweepTask = game::MAG_135_ReflectBeam_SweepTask;
	static constexpr game::Id ORIG_ReflectTask = game::MAG_135_ReflectBeam_ReflectTask;
	static constexpr game::Id ORIG_BurstTask = game::MAG_135_ReflectBeam_BurstTask;
	static constexpr game::Id ORIG_GlintTask = game::MAG_135_ReflectBeam_GlintTask;
	static constexpr game::Id ORIG_SparkTask = game::MAG_135_ReflectBeam_SparkTask;
	static constexpr game::Id ORIG_ShotTask = game::MAG_135_ReflectBeam_ShotTask;
	static const uint32_t MORPH = 0x2D44;
	static const uint32_t ARENA_A = 0x2F64;
	static const uint32_t ARENA_B = 0x1AF64;
	static constexpr game::Id MODEL_Sweep = game::unk_11FDAA4;
	static constexpr game::Id MODEL_Reflect = game::unk_120031C;
	static constexpr game::Id MODEL_Shot = game::unk_11FBF48;
	static constexpr game::Id MODEL_ShotHit = game::unk_1201644;
	static constexpr game::Id PRIM_Beam = game::unk_11FA580;
	static constexpr game::Id SEQ_Burst = game::unk_11FA3C0;
	static constexpr game::Id SEQ_Glint = game::unk_11FA208;
	static constexpr game::Id SEQ_Spark = game::unk_11FA2CC;
	static constexpr game::Id SHOT_Bones = game::unk_1201EA8;
	static const void *SOUND_Beam() { return (const void *)game_addr(game::unk_1201D88); }

	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t flag, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, flag, out); }
	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void RotationFromAnglesB(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::MAG_063_RotationFromAnglesYXZ))(angles, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void MulMatrix3(Mat4x3 *a, const void *b) { fn<void (__cdecl *)(Mat4x3 *, const void *)>(game_addr(game::MatrixMultiply3x3))(a, b); }
	inline void GteMVMVA_RotV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0))(); }
	inline uint32_t RenderPrim2(void *h, uint32_t ot, int mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int, uint32_t)>(game_addr(game::MAG_070_RenderPrimModel))(h, ot, mode, cursor); }
	inline int32_t BuildOrthonormalBasis(const int32_t *v, Mat4x3 *m) { return fn<int32_t (__cdecl *)(const int32_t *, Mat4x3 *)>(game_addr(game::BuildOrthonormalBasis))(v, m); }
	inline void NormalizeVector16(const int16_t *in, int16_t *out) { fn<void (__cdecl *)(const int16_t *, int16_t *)>(game_addr(game::VectorNormalS16))(in, out); }
	inline int32_t Sqrt(int32_t v) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::Math_SqrtInt))(v); }

#pragma pack(push, 1)
	struct MasterNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t pad0E;
		uint8_t started;
		uint32_t arena;
	};
	struct SweepNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t count;
		uint8_t order[3];
		uint8_t next;
		uint8_t *ents[3];
		int16_t centre[3];
		int16_t pad26;
		uint8_t layout[0x2B8];
	};
	struct ShotNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t hit;
		const int16_t *bone;
		int16_t hitpos[4];
		uint8_t layout[0x1F8];
		uint8_t hitlayout[0x58];
	};
	struct ReflectNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t order;
		uint8_t *ent;
		int16_t pos[4];
		uint8_t layout[0x310];
	};
	struct SpriteNode
	{
		TaskNode hdr;
		int16_t pos[3];
		int16_t counter;
		int16_t vel[3];
		int16_t pad1A;
		int16_t drag[3];
		int16_t pad22;
	};
	struct PrimArg
	{
		int16_t pos[4];
		int32_t depth;
		uint32_t morph;
	};
#pragma pack(pop)
	static_assert(sizeof(MasterNode) == 0x14 && sizeof(SweepNode) == 0x2E0 && sizeof(ShotNode) == 0x26C, "Reflect Beam nodes");
	static_assert(sizeof(ReflectNode) == 0x32C && sizeof(SpriteNode) == 0x24, "Reflect Beam nodes");
	static_assert(sizeof(PrimArg) == 0x10, "Reflect Beam prim block");

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
		if (r->flags & 0x40000) RotationFromAnglesB(r->rot, &m);
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
		PacketCursor() = RenderPrim2(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static void Play(void *layout, const int16_t *pos4, int32_t depth)
	{
		PrimArg arg;
		ff8fx::copy<(8)>(arg.pos, pos4);
		arg.depth = depth;
		arg.morph = TexBase() + MORPH;
		prim::play((prim::Layout *)layout, PrimCallback, (int)&arg, 0);
	}

	static void BeamRender(uint8_t *h, Mat4x3 *M, int16_t w, int16_t L)
	{
		Scale().m[1][1] = w;
		Scale().m[0][0] = w;
		Scale().m[2][2] = L;
		MulMatrix3(M, &Scale());
		*(uint32_t *)h = game_addr(PRIM_Beam);
		ComposeAffineTransform(&Camera(), M, M);
		GteSetRotMatrix(M);
		GteSetTransVector(M);
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
	}

	static int32_t SegT(int32_t s) { return (int32_t)(((uint32_t)s << 12) / 10u); }
	static int16_t SegX(const SweepNode *w, int32_t t) { return (int16_t)(w->centre[0] - (int16_t)(mul32(t, 5000) >> 12) + 0x9C4); }
	static int32_t SegSine(int32_t t) { return ComputeSin((t >> 2) + 0x200); }
	static void SegmentRender(uint8_t *h, const SweepNode *w, const int16_t *P, int16_t X, int32_t sine)
	{
		const int32_t v[3] = { (int32_t)X - P[0], (int32_t)P[1] - w->centre[1], (int32_t)P[2] - w->centre[2] };
		Mat4x3 M;
		const int32_t len = BuildOrthonormalBasis(v, &M);
		M.t[0] = P[0];
		M.t[1] = P[1];
		M.t[2] = P[2];
		BeamRender(h, &M, (int16_t)((sine + 0x800) >> 1), (int16_t)(mul32(sine, len) >> 11));
	}

	static int32_t FlashLevel(int32_t c)
	{
		if (c < 0x13) return (int32_t)mul32(c - 15, 2200) / 4;
		if (c < 0x2D) return 0x898;
		return (int32_t)mul32(0x39 - c, 2200) / 12;
	}

	static int16_t ShotResidue()
	{
		return (int16_t)(((uint32_t)Caster() + 0x40) >> 16);
	}
}
}

namespace ff8fx
{
namespace reflect135
{
	static void SpriteDraw(const int16_t *pos, int16_t frame, uint32_t seq, int32_t a, int32_t b)
	{
		uint8_t *h = AllocHeader(0xB4);
		TransformCameraByShadowRotation(pos, a, b);
		*(uint32_t *)h = seq;
		*(int16_t *)(h + 4) = frame;
		*(uint16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static uint32_t __cdecl BurstTask(TaskNode *n)
	{
		SpriteNode *s = (SpriteNode *)n;
		SpriteDraw(s->pos, s->counter, game_addr(SEQ_Burst), 0x1770, -0x400);
		s->counter++;
		return s->counter < 0x10 ? 0 : TASK_END;
	}

	static uint32_t __cdecl GlintTask(TaskNode *n)
	{
		SpriteNode *s = (SpriteNode *)n;
		SpriteDraw(s->pos, s->counter, game_addr(SEQ_Glint), 0x800, -0x400);
		s->counter++;
		return s->counter < 7 ? 0 : TASK_END;
	}

	static uint32_t __cdecl SparkTask(TaskNode *n)
	{
		SpriteNode *s = (SpriteNode *)n;
		SpriteDraw(s->pos, s->counter, game_addr(SEQ_Spark), 0x800, -0x200);
		s->vel[0] = (int16_t)(s->vel[0] + s->drag[0]);
		s->vel[1] = (int16_t)(s->vel[1] + s->drag[1]);
		s->vel[2] = (int16_t)(s->vel[2] + s->drag[2]);
		s->pos[0] = (int16_t)(s->pos[0] + s->vel[0]);
		s->pos[1] = (int16_t)(s->pos[1] + s->vel[1]);
		s->pos[2] = (int16_t)(s->pos[2] + s->vel[2]);
		s->counter++;
		return s->counter < 8 ? 0 : TASK_END;
	}

	static void DamageEntity(uint8_t *entity)
	{
		const int32_t slot = (int32_t)((uint32_t)entity - game_addr(game::BattleEntitySlotData)) / 0x9C;
		for (uint32_t i = 0; i < Ctx()->actions[0].target_count; i++)
		{
			uint8_t *t = Ctx()->actions[0].targets + i * TARGET_STRIDE;
			if ((int32_t)t[0] == slot) ApplyActionResultToTarget(t);
		}
	}

	static uint32_t __cdecl ReflectTask(TaskNode *n)
	{
		ReflectNode *r = (ReflectNode *)n;
		if (r->counter < 0x10)
		{
			if (!(r->counter & 1))
			{
				SpriteNode *s = (SpriteNode *)AddTaskToQueue(QSprites(), game_addr(ORIG_GlintTask));
				if (s)
				{
					s->counter = 0;
					s->pos[0] = (int16_t)((CrtRand() & 0xFF) + r->pos[0] - 0x80);
					s->pos[1] = (int16_t)((CrtRand() & 0x7F) + r->pos[1] - 0x40);
					s->pos[2] = (int16_t)((CrtRand() & 0xFF) + r->pos[2] - 0x80);
				}
			}
		}
		else
		{
			if (r->counter < 0x20 && !(r->counter & 3))
			{
				SpriteNode *s = (SpriteNode *)AddTaskToQueue(QSprites(), game_addr(ORIG_BurstTask));
				if (s)
				{
					s->counter = 0;
					s->pos[0] = (int16_t)((CrtRand() & 0x1FF) + r->pos[0] - 0x100);
					s->pos[1] = (int16_t)((CrtRand() & 0x1FF) + r->pos[1] - 0x100);
					s->pos[2] = (int16_t)((CrtRand() & 0x1FF) + r->pos[2] - 0x100);
				}
			}
			if (r->counter < 0x14)
			{
				for (int k = 0; k < 4; k++)
				{
					SpriteNode *s = (SpriteNode *)AddTaskToQueue(QSprites(), game_addr(ORIG_SparkTask));
					if (!s) continue;
					ff8fx::copy<(4)>(s->pos, r->pos);
					ff8fx::copy<(4)>(&s->pos[2], &r->pos[2]);
					s->counter = 0;
					s->vel[0] = (int16_t)((CrtRand() & 0x1FF) - 0x100);
					s->vel[1] = (int16_t)((CrtRand() & 0x1FF) - 0x100);
					const int32_t z = (CrtRand() & 0x1FF) - 0x100;
					s->drag[0] = (int16_t)((0 - (int32_t)s->vel[0]) >> 4);
					s->vel[2] = (int16_t)z;
					s->drag[1] = (int16_t)((0 - (int32_t)s->vel[1]) >> 4);
					s->drag[2] = (int16_t)((0 - (int32_t)(int16_t)z) >> 4);
				}
			}
			PrimArg arg;
			ff8fx::copy<(8)>(arg.pos, r->pos);
			arg.depth = -0x80;
			arg.morph = TexBase() + MORPH;
			if (!prim::play((prim::Layout *)r->layout, PrimCallback, (int)&arg, 0)) return TASK_END;
		}
		if (r->counter == 0x19) DamageEntity(r->ent);
		r->counter++;
		return 0;
	}

	static const Mat4x3 *ShotBone(const ShotNode *s)
	{
		return (const Mat4x3 *)(**(uint8_t ***)(Caster() + 0x64) + (int32_t)*s->bone * 0x30 + 0x20);
	}
	static int32_t ShotAngle(int32_t counter) { return ((counter + 1) << 12) / 13; }
	static void ShotFrame(const Mat4x3 *bone, int16_t *Q, Mat4x3 *m)
	{
		ff8fx::copy<(sizeof(m->m))>(m->m, bone->m);
		GteSetRotMatrixCtrl(m);
		GteSetTransVectorCtrl(bone);
		Q[0] = 0;
		Q[1] = 0;
		Q[2] = -0x1000;
		GteLoadV0(Q);
		GteMVMVA_RotV0Tr();
		GteStoreIR123(Q);
	}
	static void ShotBasis(Mat4x3 *m, const int16_t *Q)
	{
		m->t[0] = Q[0];
		m->t[1] = Q[1];
		m->t[2] = Q[2];
		NormalizeVector16(m->m[0], m->m[0]);
		NormalizeVector16(m->m[1], m->m[1]);
		NormalizeVector16(m->m[2], m->m[2]);
	}
	static void ShotPoint(Mat4x3 *m, int32_t a, int16_t *E)
	{
		GteSetRotMatrixCtrl(m);
		GteSetTransVectorCtrl(m);
		E[0] = 0;
		E[1] = 0;
		E[2] = (int16_t)(a * 4);
		GteLoadV0(E);
		GteMVMVA_RotV0Tr();
		GteStoreIR123(E);
	}
	static bool ShotProject(const int16_t *Q, int32_t a, int16_t *E, int32_t *len)
	{
		if (E[1] > 0)
		{
			*len = a * 4;
			return false;
		}
		const int32_t q1 = Q[1];
		const int32_t dy = (int32_t)E[1] - q1;
		E[1] = 0;
		E[0] = (int16_t)(mul32((int32_t)Q[0] - E[0], q1) / dy + Q[0]);
		E[2] = (int16_t)(mul32((int32_t)Q[2] - E[2], q1) / dy + Q[2]);
		const int32_t d[3] = { (int32_t)E[0] - Q[0], 0 - q1, (int32_t)E[2] - Q[2] };
		GteLoadIR123_32b(d);
		GteSQR();
		int32_t mac[3];
		GteReadMAC123(mac);
		const int32_t r = Sqrt((int32_t)((uint32_t)mac[1] + (uint32_t)mac[2] + (uint32_t)mac[0]));
		*len = r + (r >> 4);
		return true;
	}
	static void ShotBeam(Mat4x3 *m, int16_t w, int16_t L)
	{
		Scale().m[1][1] = w;
		Scale().m[0][0] = w;
		Scale().m[2][2] = L;
		MulMatrix3(m, &Scale());
		uint8_t *h = AllocHeader(0x58);
		*(uint32_t *)h = game_addr(PRIM_Beam);
		*(uint32_t *)(h + 0x1C) = 0x30;
		ComposeAffineTransform(&Camera(), m, m);
		GteSetRotMatrix(m);
		GteSetTransVector(m);
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}
	static int16_t ShotWidth(int32_t a) { return (int16_t)(ComputeSin(a >> 1) >> 3); }

	static uint32_t __cdecl ShotTask(TaskNode *n)
	{
		ShotNode *s = (ShotNode *)n;
		const Mat4x3 *bone = ShotBone(s);
		if (s->counter < 0xD)
		{
			Mat4x3 m;
			int16_t Q[4] = {};
			ShotFrame(bone, Q, &m);
			Play(s->layout, Q, -0x80);
			ShotBasis(&m, Q);
			const int32_t a = ShotAngle(s->counter);
			int16_t E[4] = {};
			int32_t len;
			ShotPoint(&m, a, E);
			if (ShotProject(Q, a, E, &len))
			{
				s->hit = 1;
				ff8fx::copy<(6)>(s->hitpos, E);
				s->hitpos[3] = ShotResidue();
			}
			ShotBeam(&m, ShotWidth(a), (int16_t)len);
		}
		else if (s->hit == 0) return TASK_END;
		if (s->hit != 0)
		{
			PrimArg arg;
			ff8fx::copy<(8)>(arg.pos, s->hitpos);
			arg.depth = 0;
			arg.morph = TexBase() + MORPH;
			if (!prim::play((prim::Layout *)s->hitlayout, PrimCallback, (int)&arg, 0)) s->hit = 0;
		}
		s->counter++;
		return 0;
	}

	static uint32_t __cdecl SweepTask(TaskNode *n)
	{
		SweepNode *w = (SweepNode *)n;
		if (w->counter >= 15) SetScreenFlash((uint32_t)FlashLevel(w->counter), 0);
		const int32_t s0 = w->counter - 15;
		if ((uint32_t)s0 < 0x14)
		{
			int16_t P[4] = {};
			GetEffectSpawnPosition(Caster(), 0x1F, 0x1000, P);
			uint8_t *h = AllocHeader(0x58);
			h[0xA] = 0;
			h[9] = 0;
			h[8] = 0;
			for (int i = 2; i >= 0; i--)
			{
				const int32_t s = s0 - i;
				if ((uint32_t)s >= 10) continue;
				const int32_t t = SegT(s);
				const int16_t X = SegX(w, t);
				if (i != 0)
				{
					*(uint32_t *)(h + 0x1C) = 0xF0;
					*(int32_t *)(h + 0xC) = (i + 4) << 9;
				}
				else
				{
					*(uint32_t *)(h + 0x1C) = 0x30;
					if ((int16_t)w->next < w->count)
					{
						uint8_t *ent = w->ents[w->next];
						if (X < *(const int16_t *)(ent + 0x1C))
						{
							ReflectNode *r = (ReflectNode *)AddTaskToQueue(QReflects(), game_addr(ORIG_ReflectTask));
							if (r)
							{
								r->counter = 0;
								r->order = w->order[w->next];
								r->ent = ent;
								GetEffectSpawnPosition(ent, 0xF1, 0, r->pos);
								r->pos[1] = (int16_t)(r->pos[1] + 0x80);
								DecodeModelPrimLayout(game_addr(MODEL_Reflect), r->layout, 0x310);
							}
							w->next++;
						}
					}
				}
				SegmentRender(h, w, P, X, SegSine(t));
			}
			FieldFree(0x58);
			Play(w->layout, P, -0x80);
		}
		if (w->counter == 0) BdPlaySE(SOUND_Beam(), 0, 0x80);
		const int32_t c = w->counter;
		if ((uint32_t)c < 0x14 && !(c & 3))
		{
			ShotNode *s = (ShotNode *)AddTaskToQueue(QShots(), game_addr(ORIG_ShotTask));
			if (s)
			{
				s->hit = 0;
				s->counter = 0;
				s->bone = (const int16_t *)game_addr(SHOT_Bones) + ((uint32_t)c >> 2) % 3u;
				DecodeModelPrimLayout(game_addr(MODEL_Shot), s->layout, 0x1F8);
				DecodeModelPrimLayout(game_addr(MODEL_ShotHit), s->hitlayout, 0x58);
			}
		}
		if (w->counter >= 0x39)
		{
			SetScreenFlash(0, 0);
			return TASK_END;
		}
		w->counter++;
		return 0;
	}

	static void StartSweep()
	{
		SweepNode *w = (SweepNode *)AddTaskToQueue(QSweep(), game_addr(ORIG_SweepTask));
		Memset32(&w->counter, 0, 0xB5);
		const ActionData *act = Ctx()->actions;
		w->count = act->target_count;
		w->centre[2] = 0;
		w->centre[1] = 0;
		w->centre[0] = 0;
		for (int i = 0; i < w->count; i++)
		{
			w->order[i] = (uint8_t)i;
			uint8_t *ent = Entity(Ctx()->actions->targets[i * TARGET_STRIDE]);
			w->ents[i] = ent;
			int16_t p[4];
			GetDefaultEffectPosition(ent, p);
			w->centre[0] = (int16_t)(w->centre[0] + p[0]);
			w->centre[1] = (int16_t)(w->centre[1] + p[1]);
			w->centre[2] = (int16_t)(w->centre[2] + p[2]);
		}
		const int32_t count = w->count;
		w->centre[0] = (int16_t)(w->centre[0] / count);
		w->centre[1] = (int16_t)(w->centre[1] / count);
		w->centre[2] = (int16_t)(w->centre[2] / count);
		for (int i = 0; i < w->count - 1; i++)
		{
			for (int j = i + 1; j < w->count; j++)
			{
				uint8_t *a = w->ents[i], *b = w->ents[j];
				if (*(const int16_t *)(a + 0x1C) < *(const int16_t *)(b + 0x1C))
				{
					w->ents[i] = b;
					w->ents[j] = a;
					const uint8_t o = w->order[j];
					w->order[j] = w->order[i];
					w->order[i] = o;
				}
			}
		}
		DecodeModelPrimLayout(game_addr(MODEL_Sweep), w->layout, 0x2B8);
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
			InitTaskQueuePool(QSweep(), (void *)(TexBase() + 0x2A64), 0x2E0, 1);
			InitTaskQueuePool(QShots(), (void *)(TexBase() + 0x1704), 0x26C, 8);
			InitTaskQueuePool(QReflects(), (void *)(TexBase() + 0xD80), 0x32C, 3);
			InitTaskQueuePool(QSprites(), (void *)TexBase(), 0x24, 0x60);
			StartSweep();
		}
		int a0, a1, a2, a3;
		if (m->started)
		{
			a0 = ExecuteTaskQueue(QSweep());
			ComputeBonesWorldMatrices(Caster() + 0x60, Caster() + 0x40);
			a1 = ExecuteTaskQueue(QShots());
			BuildBoneMatricesFromPose(Caster() + 0x60);
			a2 = ExecuteTaskQueue(QReflects());
			a3 = ExecuteTaskQueue(QSprites());
		}
		else a0 = a1 = a2 = a3 = (int)n;
		if (m->started && a0 == 0 && a1 == 0 && a2 == 0 && a3 == 0) return TASK_END;
		m->counter++;
		return 0;
	}
}

	void register_mag135_reflect_beam()
	{
		register_port(reflect135::ORIG_MasterTask, (void *)reflect135::MasterTask, "135 MasterTask", 135);
		register_port(reflect135::ORIG_SweepTask, (void *)reflect135::SweepTask, "135 SweepTask", 135);
		register_port(reflect135::ORIG_ShotTask, (void *)reflect135::ShotTask, "135 ShotTask", 135);
		register_port(reflect135::ORIG_ReflectTask, (void *)reflect135::ReflectTask, "135 ReflectTask", 135);
		register_port(reflect135::ORIG_BurstTask, (void *)reflect135::BurstTask, "135 BurstTask", 135);
		register_port(reflect135::ORIG_GlintTask, (void *)reflect135::GlintTask, "135 GlintTask", 135);
		register_port(reflect135::ORIG_SparkTask, (void *)reflect135::SparkTask, "135 SparkTask", 135);
	}
}

