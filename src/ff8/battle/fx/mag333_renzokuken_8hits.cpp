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

// Effect 333: Renzokuken

#include "mag_common.h"

namespace ff8fx
{
namespace renzo333
{
	using namespace eng;
	using namespace magc;

	inline int32_t &CasterSlot() { return var<int32_t>(game_addr(game::MAG333_CasterSlot)); }
	inline int32_t &Length() { return var<int32_t>(game_addr(game::MAG333_Length)); }
	inline int32_t &Finisher() { return var<int32_t>(game_addr(game::MAG333_FinisherFollows)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::MAG333_TextureBase)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG333_PacketCursor)); }
	inline TaskQueue *QMain() { return (TaskQueue *)game_addr(game::MAG333_MainQueue); }
	inline TaskQueue *QHit() { return (TaskQueue *)game_addr(game::TASK_QUEUE_ANIM_SEQ_99_B0_B1_B4_A5); }
	static constexpr game::Id POOL_A = game::MAG333_FlashPool, POOL_A_END = game::MAG333_SparkPool;
	static constexpr game::Id POOL_B = game::MAG333_SparkPool, POOL_B_SPAWN_END = game::unk_21FA5D0, POOL_B_END = game::MAG333_HitRecords;
	static constexpr game::Id POOL_C = game::MAG333_DebrisPool, POOL_C_END = game::MAG333_TrailSplineBase;
	static constexpr game::Id HIT_REC = game::MAG333_HitRecords;
	static constexpr game::Id PTS_A = game::MAG333_TrailPointsBase, PTS_B = game::MAG333_TrailPointsTip;
	static constexpr game::Id SPL_A = game::MAG333_TrailSplineBase, SPL_B = game::MAG333_TrailSplineTip;
	static constexpr game::Id HIT_POS = game::MAG333_HitPoint;
	static constexpr game::Id HIT_DIR = game::MAG333_HitDir;
	static constexpr game::Id SAVED_MAT = game::MAG333_RingCasterMatrix;
	static constexpr game::Id SAVED_POS = game::MAG333_RingHitPoint, SAVED_DIR = game::MAG333_RingHitDir;
	static constexpr game::Id RING = game::MAG333_TrailRing, RING_END = game::MAG333_Length;
	inline uint8_t StageCount() { return var<uint8_t>(game_addr(game::unk_1D98214)); }
	inline uint8_t StageLen(int i) { return var<uint8_t>(game_addr(game::unk_1D98215) + i); }

	static constexpr game::Id ORIG_Controller = game::MAG333_Renzokuken8Hits_ControllerTask, ORIG_HitSpark = game::MAG333_Renzokuken8Hits_HitSparkTask, ORIG_Sparkle = game::MAG333_Renzokuken8Hits_SparkleTask, ORIG_Trail = game::MAG333_Renzokuken8Hits_TrailTask, ORIG_SparkSpawn = game::MAG333_Renzokuken8Hits_SparkSpawnTask, ORIG_Hit = game::MAG333_Renzokuken8Hits_HitTask, ORIG_Slash = game::MAG333_Renzokuken8Hits_SlashTask, ORIG_RingA = game::MAG333_Renzokuken8Hits_RingATask, ORIG_RingB = game::MAG333_Renzokuken8Hits_RingBTask, ORIG_Master = game::MAG333_Renzokuken8Hits_MasterTask;

	static const void *SOUND() { return (const void *)game_addr(game::MAG333_Sound); }
	static constexpr game::Id MODEL_SPARKLE = game::MAG333_SparkleModel, SPARKLE_FADE = game::MAG333_SparkleFadeByTick, SPARKLE_DEPTH = game::MAG333_SparkleDepthByFrame;
	static constexpr game::Id ANIM_DATA = game::MAG333_AnimSets, BIND_OUT = game::MAG333_AnimBindBlock;
	static constexpr game::Id MODEL_SLASH = game::MAG333_SlashModel, MODEL_STREAK = game::MAG333_StreakModel, MODEL_RING_A = game::MAG333_RingAModel, MODEL_RING_B = game::MAG333_RingBModel;
	static constexpr game::Id SEQ_FLASH = game::MAG333_FlashSeq, SEQ_DEBRIS = game::MAG333_DebrisSeq, SEQ_SPARK = game::MAG333_SparkSeq, SPARK_FRAMES = game::MAG333_SparkFrames;
	static constexpr game::Id SLASH_REF = game::MAG333_SlashRefDir;
	static constexpr game::Id HIT_LEAD = game::MAG333_HitLeadTicks;
	static constexpr game::Id HIT_OFFS = game::MAG333_HitOffsets;

	inline int32_t Normalize(const int32_t *in, int32_t *out) { return fn<int32_t (__cdecl *)(const int32_t *, int32_t *)>(game_addr(game::NormalizeVectorToFixedPoint))(in, out); }
	inline int32_t SqrtInt(int32_t v) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::Math_SqrtInt))(v); }
	inline void GetEffectSpawnPosition(void *entity, int32_t bone, int32_t frac, int16_t *out) { fn<void (__cdecl *)(void *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, frac, out); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void MatVec(const Mat4x3 *m, const int16_t *in, int16_t *out) { fn<void (__cdecl *)(const Mat4x3 *, const int16_t *, int16_t *)>(game_addr(game::matrixMultiplyVector))(m, in, out); }
	inline void SplineSetup(int32_t n, const void *points, void *work) { fn<void (__cdecl *)(int32_t, const void *, void *)>(game_addr(game::Effect_SplineSetup))(n, points, work); }
	inline void SplineEval(int32_t n, void *work, void *out, int32_t t) { fn<void (__cdecl *)(int32_t, void *, void *, int32_t)>(game_addr(game::Effect_SplineEval))(n, work, out, t); }
	inline void GteSetFarColor(int32_t r, int32_t g, int32_t b) { fn<void (__cdecl *)(int32_t, int32_t, int32_t)>(game_addr(game::GTE_SetFarColor))(r, g, b); }
	inline void GteSetFarColor2(int32_t r, int32_t g, int32_t b) { fn<void (__cdecl *)(int32_t, int32_t, int32_t)>(game_addr(game::GTE_SetFarColorRGB))(r, g, b); }
	inline void GteLoadRGBC(const void *c) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadRGBC))(c); }
	inline void GteDPCS() { fn<void (__cdecl *)()>(game_addr(game::GTE_DPCS))(); }
	inline void GteStoreRGB2(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreRGB2))(dst); }
	inline void GteLoadV012(const void *v0, const void *v1, const void *v2) { fn<void (__cdecl *)(const void *, const void *, const void *)>(game_addr(game::GTE_LoadV012))(v0, v1, v2); }
	inline void GteRTPT() { fn<void (__cdecl *)()>(game_addr(game::GTE_RTPT))(); }
	inline void GteReadFLAG(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadFLAG))(dst); }
	inline void GteReadSXY012Split(void *a, void *b, void *c) { fn<void (__cdecl *)(void *, void *, void *)>(game_addr(game::GTE_ReadSXY012_Split))(a, b, c); }
	inline void GteAVSZ4() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ4))(); }
	inline void GteReadOTZ32(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadOTZ))(dst); }
	inline void GteMVMVA_RotV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0))(); }
	inline void BindEntityModelFromComData(uint8_t *entity, uint32_t out, uint32_t data) { fn<void (__cdecl *)(uint8_t *, uint32_t, uint32_t)>(game_addr(game::BattleEntity_BindExternalAnimSet))(entity, out, data); }
	inline void QueueChainTransformation(uint8_t *entity, int32_t seq) { fn<void (__cdecl *)(uint8_t *, int32_t)>(game_addr(game::QueueChainTransformation))(entity, seq); }
	inline void AddAnimSeqTask(uint8_t *entity, int32_t type) { fn<void (__cdecl *)(uint8_t *, int32_t)>(game_addr(game::HitEffect_QueueFunctionTask))(entity, type); }

#pragma pack(push, 1)
	struct RootNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
	};
	struct Node24
	{
		TaskNode hdr;
		int16_t c;
		int16_t e;
		int16_t w10, w12, w14;
		int16_t w16;
		int16_t w18;
		int16_t w1A;
		int16_t w1C;
		int16_t w1E;
		int16_t w20;
		int16_t w22;
	};
	struct FlashP
	{
		int16_t life, scale;
		int16_t pos[4];
	};
	struct SparkP
	{
		int16_t life, scale;
		int16_t pos[3];
		int16_t vy;
		int16_t frame;
		int16_t pad0E;
	};
	struct DebrisP
	{
		int16_t life, scale;
		int16_t pos[4];
		int16_t vel[3];
		int16_t pad12;
	};
	struct HitRec
	{
		int16_t pos[4];
		int32_t dir[4];
		int32_t count;
		int16_t angle[16];
		int16_t width[16];
		int16_t off[16];
		int16_t speed[16];
	};
	struct RingEntry
	{
		int32_t alive;
		uint32_t base[2];
		uint32_t tip[2];
	};
	struct TrailWork
	{
		uint8_t pad00[8];
		int32_t otz;
		uint32_t flag;
		uint32_t col0;
		uint32_t col1;
		uint32_t pad18;
		uint32_t base_col;
		int16_t base[4];
		int16_t tip[4];
		int32_t dir[4];
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x10 && sizeof(Node24) == 0x24, "Renzokuken nodes");
	static_assert(sizeof(FlashP) == 0xC && sizeof(SparkP) == 0x10 && sizeof(DebrisP) == 0x14, "Renzokuken pools");
	static_assert(sizeof(HitRec) == 0x9C && sizeof(RingEntry) == 0x14 && sizeof(TrailWork) == 0x40, "Renzokuken records");

	inline uint8_t *Caster() { return Entity(CasterSlot()); }

	static void BoneFrame(uint8_t *anim_header, int32_t bone, int32_t *dir, int16_t *pos)
	{
		const uint8_t *sk = *(uint8_t **)*(uint8_t **)(anim_header + 4);
		if (bone >= 0xF0) bone = sk[bone - 0xEC];
		const uint8_t *b = sk + 0x10 + bone * 0x30;
		dir[1] = 0;
		dir[0] = 0;
		dir[2] = -0x1000;
		TransformVectorBy3x3Matrix((const Mat4x3 *)(b + 0x10), dir, dir);
		Normalize(dir, dir);
		pos[0] = *(const int16_t *)(b + 0x24);
		pos[1] = *(const int16_t *)(b + 0x28);
		pos[2] = *(const int16_t *)(b + 0x2C);
	}

	static void WeaponBone(uint8_t *entity, int32_t bone, int32_t *dir, int16_t *pos)
	{
		uint8_t *w = *(uint8_t **)(entity + 0x78);
		if (!w) return;
		BoneFrame(w, bone, dir, pos);
		Mat4x3 m;
		ComposeAffineTransform((const Mat4x3 *)(entity + 0x40), (const Mat4x3 *)(w + 0x24), &m);
		TransformVectorBy3x3Matrix(&m, dir, dir);
		Normalize(dir, dir);
		MatVec(&m, pos, pos);
		pos[0] = (int16_t)(pos[0] + (int16_t)m.t[0]);
		pos[1] = (int16_t)(pos[1] + (int16_t)m.t[1]);
		pos[2] = (int16_t)(pos[2] + (int16_t)m.t[2]);
	}
}
}

namespace ff8fx
{
namespace renzo333
{
	static uint32_t __cdecl ControllerTask(TaskNode *tn)
	{
		if (DrawOnly()) return 0;
		Node24 *n = (Node24 *)tn;
		if (n->c == 0)
		{
			uint16_t *flags = (uint16_t *)Caster();
			const uint16_t f = *flags;
			n->e = (int16_t)(f & 0x800);
			*flags = (uint16_t)(f | 0x800);
		}
		if (n->c == 1)
		{
			BindEntityModelFromComData(Caster(), game_addr(BIND_OUT), game_addr(ANIM_DATA));
			QueueChainTransformation(Caster(), (Finisher() != 0) + 1);
		}
		if (n->c == 2)
		{
			const int32_t last = (int32_t)StageCount() + 1;
			Length() = 0;
			for (int32_t i = 1; i <= last; i++) Length() = Length() + var<uint8_t>(game_addr(game::unk_1D98214) + i);
			Node24 *t = (Node24 *)AddTaskToQueue(QMain(), game_addr(ORIG_Trail));
			t->c = 0;
			for (uint32_t a = game_addr(RING); a < game_addr(RING_END); a += 0x14) var<int32_t>(a) = 0;
			t = (Node24 *)AddTaskToQueue(QMain(), game_addr(ORIG_SparkSpawn));
			t->c = 0;
			t->e = (int16_t)(CrtRand() % 0x18);
			t = (Node24 *)AddTaskToQueue(QMain(), game_addr(ORIG_Hit));
			t->c = 0;
		}
		if (Finisher() != 0 && (int32_t)n->c == Length())
		{
			Node24 *t = (Node24 *)AddTaskToQueue(QHit(), game_addr(ORIG_HitSpark));
			t->c = 0;
			t->w1A = (int16_t)CasterSlot();
		}
		if (n->c == 0x4C)
		{
			Node24 *a = (Node24 *)AddTaskToQueue(QMain(), game_addr(ORIG_RingA));
			a->c = 0;
			const int16_t sa = (int16_t)((CrtRand() % 0xC00 + 0x2400) / 3);
			a->w1E = sa;
			a->w1C = sa;
			Node24 *b = (Node24 *)AddTaskToQueue(QMain(), game_addr(ORIG_RingB));
			b->c = 0;
			const int16_t sb = (int16_t)((CrtRand() % 0x600 + 0x2000) / 3);
			ff8fx::copy<(0x18)>((void *)game_addr(SAVED_POS), (const void *)game_addr(HIT_POS));
			b->w1E = sb;
			b->w1C = sb;
			ff8fx::copy<(0x20)>((void *)game_addr(SAVED_MAT), Caster() + 0x40);
		}
		if (n->c <= 0x10)
			SetScreenFlash((uint32_t)shl32(n->c, 7), 0);
		else if ((int32_t)n->c >= Length() - 8 && n->c > 4 && Finisher() == 0)
			SetScreenFlash((uint32_t)shl32(Length() - n->c, 8), 0);
		bool tint = false;
		int32_t k = 0;
		if (n->c <= 8)
		{
			GteSetFarColor(0xF0, 0x60, 0x40);
			k = n->c;
			tint = true;
		}
		else if ((int32_t)n->c >= Length() - 8 && n->c > 4)
		{
			GteSetFarColor(0xF0, 0x60, 0x40);
			k = Length() - n->c;
			tint = true;
		}
		if (tint)
		{
			uint32_t colour = var<uint32_t>(game_addr(game::CURRENT_BS_MODEL_PALETTE));
			GteLoadRGBC(&colour);
			GteSetIR0(shl32(k, 9));
			GteDPCS();
			GteStoreRGB2(Caster() + 0x28);
		}
		if (n->c == 1) BdPlaySE(SOUND(), 0x8000, 0x80);
		n->c++;
		if ((int32_t)n->c <= Length()) return 0;
		if (Finisher() == 0) SetScreenFlash(0, 0);
		*(uint32_t *)(Caster() + 0x28) = var<uint32_t>(game_addr(game::CURRENT_BS_MODEL_PALETTE));
		uint16_t *flags = (uint16_t *)Caster();
		*flags = (uint16_t)((*flags & 0xF7FF) | (uint16_t)n->e);
		return TASK_END;
	}

	static uint32_t __cdecl HitSparkTask(TaskNode *tn)
	{
		if (DrawOnly()) return 0;
		Node24 *n = (Node24 *)tn;
		if (n->c >= 4 && n->c <= 8)
		{
			int16_t p[4];
			GetDefaultEffectPosition(Entity(n->w1A), p);
			const int16_t h = p[3];
			p[3] = p[1];
			p[1] = h;
			for (int32_t i = 0; i < 6; i++)
			{
				Node24 *s = (Node24 *)AddTaskToQueue(QHit(), game_addr(ORIG_Sparkle));
				s->c = 0;
				s->e = (int16_t)(CrtRand() % 6 + i);
				const uint8_t *sk = **(uint8_t ***)(Entity(n->w1A) + 0x64);
				int32_t bone = sk[0];
				if (bone != 0) bone = CrtRand() % bone;
				int16_t q[4];
				GetEffectSpawnPosition(Entity(n->w1A), bone, 0, q);
				q[0] = (int16_t)(q[0] + (int16_t)(CrtRand() % 0xAF - 0x57));
				q[1] = (int16_t)(q[1] + (int16_t)(CrtRand() % 0xAF - 0x57));
				q[2] = (int16_t)(q[2] + (int16_t)(CrtRand() % 0xAF - 0x57));
				int16_t y = q[1];
				if (y > -600)
				{
					y = (int16_t)(y - 600);
					q[1] = y;
				}
				int32_t v[3] = { (int32_t)q[0] - p[0], (int32_t)y - p[1], (int32_t)q[2] - p[2] };
				Normalize(v, v);
				v[0] = shl32(v[0], 2);
				v[2] = shl32(v[2], 2);
				Normalize(v, v);
				const int32_t len = CrtRand() % 0x258 + 0x15E;
				s->w10 = (int16_t)((mul32(len, v[0]) >> 12) + q[0]);
				s->w12 = (int16_t)((mul32(len, v[1]) >> 12) + q[1]);
				s->w18 = (int16_t)bone;
				s->w14 = (int16_t)((mul32(len, v[2]) >> 12) + q[2]);
				s->w1A = n->w1A;
				s->w1C = (int16_t)(CrtRand() % 8);
				s->w20 = (int16_t)(CrtRand() % 0x680 + 0x80);
			}
		}
		if (n->c == 1) AddAnimSeqTask(Entity(n->w1A), 3);
		n->c++;
		return n->c >= 0x1E ? TASK_END : 0;
	}

	static uint32_t __cdecl SparkleTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		if (n->e > 0)
		{
			if (DrawOnly()) return 0;
			n->e--;
			return 0;
		}
		int16_t q[4];
		GetEffectSpawnPosition(Entity(n->w1A), n->w18, shl32(n->c, 9), q);
		int32_t d[3] = { (int32_t)var<int16_t>(game_addr(game::Battle_Camera_world_XZ_s16)) - q[0], 0, (int32_t)var<int16_t>(game_addr(game::Battle_Camera_world_Y)) - q[2] };
		int32_t u[3] = { 0, 0, -0x1000 };
		Normalize(d, d);
		int32_t axis[3];
		int32_t angle = RotationBetweenVectors(u, d, axis);
		int16_t ang[3] = { 0, 0, 0 };
		Mat4x3 m1;
		BuildRotationMatrixFromAngles(ang, &m1);
		Mat4x3 m2;
		m1.t[0] = 0xF0;
		m1.t[1] = 0;
		m1.t[2] = 0;
		BuildAxisAngleRotationMatrix(angle, &m2, axis);
		MatrixMultiply(&m2, &m1);
		int32_t spin = -shl32(n->c, 6);
		if (axis[1] < 0) spin = -spin;
		BuildAxisAngleRotationMatrix(spin, &m2, d);
		MatrixMultiply(&m2, &m1);
		int32_t e[3] = { (int32_t)n->w10 - q[0], (int32_t)n->w12 - q[1], (int32_t)n->w14 - q[2] };
		u[0] = 0;
		u[1] = -0x1000;
		u[2] = 0;
		const int32_t len = SqrtInt(Normalize(e, e));
		angle = RotationBetweenVectors(u, e, axis);
		BuildAxisAngleRotationMatrix(angle, &m2, axis);
		m2.t[0] = q[0];
		m2.t[2] = q[2];
		int32_t sc[3];
		sc[2] = n->w20;
		sc[0] = n->w20;
		m2.t[1] = q[1];
		sc[1] = shl32(len, 12) / 2000;
		Scale3DMatrix(&m2, sc);
		ComposeAffineTransform(&Camera(), &m2, &m2);
		ComposeAffineTransform(&m2, &m1, &m1);
		GteSetRotMatrix(&m1);
		GteSetTransVector(&m1);
		uint8_t *h = AllocHeader(0x58);
		*(uint32_t *)h = game_addr(MODEL_SPARKLE);
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0xC) = var<uint16_t>(game_addr(SPARKLE_FADE) + 2 * n->c);
		*(uint32_t *)(h + 0x18) = var<uint16_t>(game_addr(SPARKLE_DEPTH) + 2 * n->w1C);
		*(uint32_t *)(h + 0x1C) = 0x10F3;
		var<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = RenderPrimModel(h, RenderOT(), 2, var<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)));
		FieldFree(0x58);
		if (DrawOnly()) return 0;
		n->w1C++;
		if (n->w1C >= 8) n->w1C = 0;
		n->c++;
		return n->c >= 8 ? TASK_END : 0;
	}

	static void TrailPoints(TrailWork *w)
	{
		WeaponBone(Caster(), 1, w->dir, w->base);
		w->tip[0] = (int16_t)((int16_t)(mul32(w->dir[0], 666) >> 12) + w->base[0]);
		w->tip[1] = (int16_t)((int16_t)(mul32(w->dir[1], 666) >> 12) + w->base[1]);
		w->tip[2] = (int16_t)((int16_t)(mul32(w->dir[2], 666) >> 12) + w->base[2]);
	}

	static int32_t TrailGather(const uint8_t *ring, int32_t idx, int32_t want, uint8_t *pa, uint8_t *pb)
	{
		int32_t cnt = 0;
		if (want <= 0) return 0;
		do
		{
			const RingEntry *e = (const RingEntry *)(ring + 0x14 * idx);
			if (!e->alive) break;
			idx--;
			*(uint32_t *)(pa + 8 * cnt) = e->base[0];
			*(uint32_t *)(pa + 8 * cnt + 4) = e->base[1];
			*(uint32_t *)(pb + 8 * cnt) = e->tip[0];
			*(uint32_t *)(pb + 8 * cnt + 4) = e->tip[1];
			if (idx < 0) idx = 6;
			cnt++;
		} while (cnt < want);
		return cnt;
	}

	static void TrailSplines(int32_t cnt, const uint8_t *pa, const uint8_t *pb, uint8_t *sa, uint8_t *sb)
	{
		uint8_t *wk = (uint8_t *)FieldAlloc(0x190);
		SplineSetup(cnt, pa, wk);
		for (int32_t k = 0, t = 0; k < 32; k++, t += 0x1000) SplineEval(cnt, wk, sa + 8 * k, t / 31);
		SplineSetup(cnt, pb, wk);
		for (int32_t k = 0, t = 0; k < 32; k++, t += 0x1000) SplineEval(cnt, wk, sb + 8 * k, t / 31);
		FieldFree(0x190);
	}

	static void TrailHitPoint(const uint8_t *sb, uint8_t *pos, int32_t *dir)
	{
		*(uint32_t *)pos = *(const uint32_t *)(sb + 8);
		*(uint32_t *)(pos + 4) = *(const uint32_t *)(sb + 0xC);
		dir[0] = (int32_t)*(const int16_t *)sb - *(const int16_t *)(sb + 8);
		dir[1] = (int32_t)*(const int16_t *)(sb + 2) - *(const int16_t *)(sb + 0xA);
		dir[2] = (int32_t)*(const int16_t *)(sb + 4) - *(const int16_t *)(sb + 0xC);
		Normalize(dir, dir);
	}

	static void TrailDraw(const uint8_t *sa, const uint8_t *sb, TrailWork *w)
	{
		GteSetRotMatrix(&Camera());
		GteSetTransVector(&Camera());
		GteSetFarColor2(0, 0, 0);
		uint32_t *pk = (uint32_t *)PacketCursor();
		uint32_t *pk2 = pk + 9;
		w->col1 = 0x3A903030;
		w->base_col = 0x3A903030;
		int32_t tacc = 0;
		for (int32_t off = 0; off < 0xF0; off += 8, tacc += 0x1000)
		{
			w->col0 = w->col1;
			pk[0] = 0x8000000;
			GteLoadV012(sa + off, sa + off + 8, sb + off);
			GteRTPT();
			GteReadFLAG(&w->flag);
			if (w->flag & 0x60000) continue;
			GteReadSXY012Split(pk + 2, pk + 4, pk + 6);
			GteLoadV0(sb + off + 8);
			GteRTPS();
			GteReadSXY2(pk + 8);
			GteAVSZ4();
			GteReadOTZ32(&w->otz);
			GteSetIR0(tacc / 30);
			GteLoadRGBC(&w->base_col);
			GteDPCS();
			GteStoreRGB2(&w->col1);
			pk[5] = pk[1] = w->col0;
			pk[7] = pk[3] = w->col1;
			InsertPrimAutoDepth(var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 4 * (uint32_t)(w->otz >> 2) + 0x44, pk);
			pk2[5] = w->col0;
			pk2[3] = pk2[1] = 0x3A000000;
			pk2[7] = w->col1;
			pk2[4] = pk[4];
			pk2[2] = pk[2];
			pk2[6] = pk[6];
			pk2[8] = pk[8];
			pk2[0] = 0x8000000;
			InsertPrimAutoDepth(var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 4 * (uint32_t)(w->otz >> 2) + 0x44, pk2);
			pk += 0x12;
			pk2 += 0x12;
		}
		PacketCursor() = (uint32_t)pk2;
	}

	static uint32_t __cdecl TrailTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		TrailWork *w = (TrailWork *)FieldAlloc(0x40);
		TrailPoints(w);
		const int32_t c = n->c;
		const int32_t idx = c % 7;
		RingEntry *e = (RingEntry *)(game_addr(RING) + 0x14 * idx);
		e->alive = 1;
		e->base[0] = *(const uint32_t *)&w->base[0];
		e->base[1] = *(const uint32_t *)&w->base[2];
		e->tip[0] = *(const uint32_t *)&w->tip[0];
		e->tip[1] = *(const uint32_t *)&w->tip[2];
		const int32_t want = c < Length() - 7 ? 7 : Length() - c;
		const int32_t cnt = TrailGather((const uint8_t *)game_addr(RING), idx, want, (uint8_t *)game_addr(PTS_A), (uint8_t *)game_addr(PTS_B));
		if (cnt > 1)
		{
			TrailSplines(cnt, (const uint8_t *)game_addr(PTS_A), (const uint8_t *)game_addr(PTS_B), (uint8_t *)game_addr(SPL_A), (uint8_t *)game_addr(SPL_B));
			TrailHitPoint((const uint8_t *)game_addr(SPL_B), (uint8_t *)game_addr(HIT_POS), (int32_t *)game_addr(HIT_DIR));
			TrailDraw((const uint8_t *)game_addr(SPL_A), (const uint8_t *)game_addr(SPL_B), w);
		}
		FieldFree(0x40);
		if (DrawOnly()) return 0;
		n->c++;
		return (int32_t)n->c >= Length() - 1 ? TASK_END : 0;
	}

	static int32_t FreeSlot(uint32_t pool, uint32_t end, uint32_t stride)
	{
		int32_t i = 0;
		for (uint32_t a = pool; *(const int16_t *)a >= 0; i++)
		{
			a += stride;
			if (a >= end) return -1;
		}
		return i >= 0x28 ? -1 : i;
	}

	static uint32_t __cdecl SparkSpawnTask(TaskNode *tn)
	{
		if (DrawOnly()) return 0;
		Node24 *n = (Node24 *)tn;
		if ((int32_t)n->c < Length() - 8)
		{
			for (int32_t j = 0; j < 3; j++)
			{
				const int32_t i = FreeSlot(game_addr(POOL_B), game_addr(POOL_B_SPAWN_END), 0x10);
				if (i < 0) break;
				SparkP *p = (SparkP *)(game_addr(POOL_B) + 0x10 * i);
				p->life = 0;
				p->scale = (int16_t)(CrtRand() % 0x300 + 0x400);
				const uint8_t *sk = **(uint8_t ***)(Caster() + 0x64);
				int32_t bone = sk[0];
				if (bone != 0) bone = CrtRand() % bone;
				GetEffectSpawnPosition(Caster(), bone, 0, p->pos);
				p->vy = (int16_t)(CrtRand() % 10 + 2);
				p->frame = (int16_t)((int32_t)n->e % 0x18);
			}
		}
		n->c++;
		n->e++;
		return (int32_t)n->c >= Length() - 1 ? TASK_END : 0;
	}

	static uint32_t __cdecl HitTask(TaskNode *tn)
	{
		if (DrawOnly()) return 0;
		Node24 *n = (Node24 *)tn;
		int32_t sum = 0;
		for (int32_t b = 0; b < (int32_t)StageCount(); b++)
		{
			const int32_t lead = var<int32_t>(game_addr(HIT_LEAD) + 4 * b);
			sum += StageLen(b);
			if ((int32_t)n->c != sum - lead) continue;
			Node24 *s = (Node24 *)AddTaskToQueue(QMain(), game_addr(ORIG_Slash));
			s->c = 0;
			s->e = (int16_t)b;
			const int16_t sc = (int16_t)(CrtRand() % 0x280 + 0x400);
			s->w1E = sc;
			s->w1C = sc;
			int16_t hp[4];
			MatVec((const Mat4x3 *)(Caster() + 0x40), (const int16_t *)(game_addr(HIT_OFFS) + 8 * b), hp);
			hp[0] = (int16_t)(hp[0] + var<int16_t>(game_addr(HIT_POS)));
			hp[1] = (int16_t)(hp[1] + var<int16_t>(game_addr(HIT_POS) + 2));
			hp[2] = (int16_t)(hp[2] + var<int16_t>(game_addr(HIT_POS) + 4));
			hp[3] = 0;
			HitRec *r = (HitRec *)(game_addr(HIT_REC) + 0x9C * b);
			ff8fx::copy<(8)>(r->pos, hp);
			ff8fx::copy<(16)>(r->dir, (const void *)game_addr(HIT_DIR));
			r->count = 0x10;
			for (int32_t k = 0; k < r->count; k++)
			{
				r->angle[k] = (int16_t)(CrtRand() % 0x1000);
				const int32_t wv = CrtRand() % 0x800 + 0x800;
				r->width[k] = (int16_t)wv;
				r->off[k] = (int16_t)((mul32(wv, 1000) - 300) >> 12);
				r->speed[k] = (int16_t)(CrtRand() % 200 + 0x78);
			}
			for (int32_t j = 0; j < 3; j++)
			{
				const int32_t i = FreeSlot(game_addr(POOL_A), game_addr(POOL_A_END), 0xC);
				if (i < 0) break;
				FlashP *p = (FlashP *)(game_addr(POOL_A) + 0xC * i);
				p->life = 0;
				p->scale = (int16_t)(CrtRand() % 0x600 + 0xA00);
				ff8fx::copy<(8)>(p->pos, hp);
				p->pos[0] = (int16_t)(p->pos[0] + (int16_t)(CrtRand() % 0x15E - 0xAF));
				p->pos[1] = (int16_t)(p->pos[1] + (int16_t)(CrtRand() % 0x15E - 0xAF));
				p->pos[2] = (int16_t)(p->pos[2] + (int16_t)(CrtRand() % 0x15E - 0xAF));
			}
			for (int32_t j = 0; j < 0xC; j++)
			{
				const int32_t i = FreeSlot(game_addr(POOL_C), game_addr(POOL_C_END), 0x14);
				if (i < 0) break;
				DebrisP *p = (DebrisP *)(game_addr(POOL_C) + 0x14 * i);
				p->life = 0;
				p->scale = (int16_t)(CrtRand() % 0x800 + 0xC00);
				ff8fx::copy<(8)>(p->pos, hp);
				const int32_t sp = CrtRand() % 200 + 0x8C;
				const int32_t *d = (const int32_t *)game_addr(HIT_DIR);
				const int32_t r0 = CrtRand() % 200;
				p->vel[0] = (int16_t)(r0 + (mul32(d[0], sp) >> 12) - 0x64);
				const int32_t r1 = CrtRand() % 200;
				p->vel[1] = (int16_t)(r1 + (mul32(d[1], sp) >> 12) - 0x64);
				const int32_t r2 = CrtRand() % 200;
				p->vel[2] = (int16_t)(r2 + (mul32(d[2], sp) >> 12) - 0x64);
				if (b == 3)
				{
					p->vel[0] = (int16_t)-p->vel[0];
					p->vel[1] = (int16_t)-p->vel[1];
					p->vel[2] = (int16_t)-p->vel[2];
				}
			}
		}
		n->c++;
		return (int32_t)n->c >= Length() - 1 ? TASK_END : 0;
	}

	static uint32_t __cdecl SlashTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		HitRec *r = (HitRec *)(game_addr(HIT_REC) + 0x9C * n->e);
		int32_t axis[3];
		const int32_t angle = RotationBetweenVectors((const int32_t *)game_addr(SLASH_REF), r->dir, axis);
		Mat4x3 m;
		BuildAxisAngleRotationMatrix(angle, &m, axis);
		m.t[0] = r->pos[0];
		m.t[1] = r->pos[1];
		m.t[2] = r->pos[2];
		ComposeAffineTransform(&Camera(), &m, &m);
		int16_t ang[3] = { 0, 0, 0 };
		Mat4x3 m3;
		BuildRotationMatrixFromAngles(ang, &m3);
		int32_t sv[3];
		m3.t[0] = 0;
		m3.t[1] = 0;
		m3.t[2] = 0;
		sv[1] = n->w1C;
		sv[0] = n->w1C;
		sv[2] = 0x1000;
		Scale3DMatrix(&m3, sv);
		ComposeAffineTransform(&m, &m3, &m3);
		GteSetRotMatrix(&m3);
		GteSetTransVector(&m3);
		uint8_t *h = AllocHeader(0x58);
		*(uint32_t *)h = game_addr(MODEL_SLASH);
		*(uint32_t *)(h + 0x1C) = 0x30;
		if (n->c >= 2)
		{
			h[0xA] = 0;
			h[9] = 0;
			h[8] = 0;
			*(uint32_t *)(h + 0x1C) = 0xF0;
			*(int32_t *)(h + 0xC) = mul32(n->c - 2, 682);
		}
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
		*(uint32_t *)h = game_addr(MODEL_STREAK);
		*(uint32_t *)(h + 0x1C) |= 3;
		for (int32_t k = 0; k < r->count; k++)
		{
			ang[2] = r->angle[k];
			BuildRotationMatrixFromAngles(ang, &m3);
			int16_t v[4];
			v[3] = 0;
			v[2] = 0;
			v[0] = 0;
			v[1] = r->off[k];
			GteLoadV0(v);
			GteSetRotMatrix(&m3);
			GteMVMVA_RotV0();
			GteReadMAC123(m3.t);
			const int32_t wd = r->width[k];
			sv[1] = wd;
			sv[0] = (wd >> 1) + wd;
			Scale3DMatrix(&m3, sv);
			ComposeAffineTransform(&m, &m3, &m3);
			GteSetRotMatrix(&m3);
			GteSetTransVector(&m3);
			PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
			if (!DrawOnly())
			{
				r->off[k] = (int16_t)(r->off[k] + r->speed[k]);
				r->speed[k] = (int16_t)(r->speed[k] - (int16_t)(r->speed[k] >> 4));
			}
		}
		FieldFree(0x58);
		if (DrawOnly()) return 0;
		const int16_t rate = n->w1E;
		n->w1C = (int16_t)(n->w1C + rate);
		n->w1E = (int16_t)(rate - rate / 6);
		n->c++;
		return n->c >= 8 ? TASK_END : 0;
	}

	enum RingKind { RING_A, RING_B };
	struct RingDef { game::Id orig, model; int32_t fade_from, ticks; bool sep_y; };
	static const RingDef RINGS[2] = { { ORIG_RingA, MODEL_RING_A, 4, 0x10, false }, { ORIG_RingB, MODEL_RING_B, 6, 0x12, false } };
	static void RingDraw(int kind, const Node24 *n, int32_t sx, int32_t sy, int32_t fade)
	{
		(void)sy;
		Mat4x3 m = var<Mat4x3>(game_addr(SAVED_MAT));
		int16_t v[4];
		v[0] = 0;
		v[1] = 400;
		v[2] = -900;
		v[3] = 0;
		MatVec(&m, v, v);
		m.t[0] = (int32_t)var<int16_t>(game_addr(SAVED_POS)) + v[0];
		m.t[1] = (int32_t)var<int16_t>(game_addr(SAVED_POS) + 2) + v[1];
		m.t[2] = (int32_t)var<int16_t>(game_addr(SAVED_POS) + 4) + v[2];
		int32_t sv[3];
		sv[2] = sx;
		sv[1] = sx;
		sv[0] = sx;
		Scale3DMatrix(&m, sv);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		uint8_t *h = AllocHeader(0x58);
		*(uint32_t *)h = game_addr(RINGS[kind].model);
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0x1C) = 0x33;
		if (n->c >= RINGS[kind].fade_from)
		{
			*(uint32_t *)(h + 0x1C) = 0xF3;
			*(int32_t *)(h + 0xC) = fade;
		}
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}
	static int32_t RingFade(int kind, int32_t c) { return mul32(c - RINGS[kind].fade_from, 341); }
	static int32_t RingSX(const Node24 *n) { return n->w1C; }
	static int32_t RingSY(int kind, const Node24 *n) { (void)kind; return n->w1C; }

	static uint32_t RingTask(int kind, Node24 *n)
	{
		RingDraw(kind, n, RingSX(n), RingSY(kind, n), RingFade(kind, n->c));
		if (DrawOnly()) return 0;
		const int16_t rate = n->w1E;
		n->w1C = (int16_t)(n->w1C + rate);
		n->w1E = (int16_t)(rate - (int16_t)(rate / 4));
		n->c++;
		return n->c >= RINGS[kind].ticks ? TASK_END : 0;
	}
	static uint32_t __cdecl RingATask(TaskNode *tn) { return RingTask(RING_A, (Node24 *)tn); }
	static uint32_t __cdecl RingBTask(TaskNode *tn) { return RingTask(RING_B, (Node24 *)tn); }

	static void ParticleDraws()
	{
		uint8_t *h = AllocHeader(0xB4);
		*(uint32_t *)h = game_addr(SEQ_FLASH);
		*(int16_t *)(h + 0x24) = 0;
		for (uint32_t a = game_addr(POOL_A); a < game_addr(POOL_A_END); a += 0xC)
		{
			FlashP *p = (FlashP *)a;
			if (p->life < 0) continue;
			*(int16_t *)(h + 4) = p->life;
			TransformCameraByShadowRotation(p->pos, p->scale, -(p->scale >> 4));
			PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
			if (UpdateFlags() & 0x201) continue;
			p->life++;
			if (*(const int16_t *)(h + 0x28) < 0) p->life = -1;
		}
		*(uint32_t *)h = game_addr(SEQ_DEBRIS);
		for (uint32_t a = game_addr(POOL_C); a < game_addr(POOL_C_END); a += 0x14)
		{
			DebrisP *p = (DebrisP *)a;
			if (p->life < 0) continue;
			*(int16_t *)(h + 4) = p->life;
			TransformCameraByShadowRotation(p->pos, p->scale, -(p->scale >> 4));
			PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
			if (UpdateFlags() & 0x201) continue;
			p->life++;
			if (*(const int16_t *)(h + 0x28) < 0)
			{
				p->life = -1;
				continue;
			}
			p->scale = (int16_t)(p->scale - (int16_t)(p->scale >> 5));
			p->pos[0] = (int16_t)(p->pos[0] + p->vel[0]);
			p->pos[1] = (int16_t)(p->pos[1] + p->vel[1]);
			p->pos[2] = (int16_t)(p->pos[2] + p->vel[2]);
			p->vel[0] = (int16_t)(p->vel[0] - (int16_t)(p->vel[0] >> 3));
			p->vel[1] = (int16_t)(p->vel[1] - (int16_t)(p->vel[1] >> 3));
			p->vel[2] = (int16_t)(p->vel[2] - (int16_t)(p->vel[2] >> 3));
		}
		*(uint32_t *)h = game_addr(SEQ_SPARK);
		*(int16_t *)(h + 0x24) = 4;
		for (uint32_t a = game_addr(POOL_B); a < game_addr(POOL_B_END); a += 0x10)
		{
			SparkP *p = (SparkP *)a;
			if (p->life < 0) continue;
			*(int16_t *)(h + 4) = p->life;
			*(uint32_t *)(h + 0x1C) = var<uint32_t>(game_addr(SPARK_FRAMES) + 4 * p->frame);
			TransformCameraByShadowRotation(p->pos, p->scale, -(p->scale >> 4));
			PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
			if (UpdateFlags() & 0x201) continue;
			p->life++;
			if (*(const int16_t *)(h + 0x28) < 0)
			{
				p->life = -1;
				continue;
			}
			p->pos[1] = (int16_t)(p->pos[1] + p->vy);
			p->frame++;
			if (p->frame >= 0x18) p->frame = 0;
		}
		FieldFree(0xB4);
	}

	static uint32_t __cdecl MasterTask(TaskNode *tn)
	{
		RootNode *r = (RootNode *)tn;
		if (r->counter & 1) PacketCursor() = TexBase() + 0x8000;
		else PacketCursor() = TexBase();
		const int alive = ExecuteTaskQueue(QMain());
		ParticleDraws();
		r->counter++;
		return alive ? 0 : TASK_END;
	}
}

	void register_mag333_renzokuken_8hits()
	{
		register_port(renzo333::ORIG_Master, (void *)renzo333::MasterTask, "333 Master", 333);
		register_port(renzo333::ORIG_Controller, (void *)renzo333::ControllerTask, "333 Controller", 333);
		register_port(renzo333::ORIG_Trail, (void *)renzo333::TrailTask, "333 Trail", 333);
		register_port(renzo333::ORIG_SparkSpawn, (void *)renzo333::SparkSpawnTask, "333 SparkSpawn", 333);
		register_port(renzo333::ORIG_Hit, (void *)renzo333::HitTask, "333 Hit", 333);
		register_port(renzo333::ORIG_Slash, (void *)renzo333::SlashTask, "333 Slash", 333);
		register_port(renzo333::ORIG_RingA, (void *)renzo333::RingATask, "333 RingA", 333);
		register_port(renzo333::ORIG_RingB, (void *)renzo333::RingBTask, "333 RingB", 333);
		register_port(renzo333::ORIG_HitSpark, (void *)renzo333::HitSparkTask, "333 HitSpark", 333);
		register_port(renzo333::ORIG_Sparkle, (void *)renzo333::SparkleTask, "333 Sparkle", 333);
	}
}

