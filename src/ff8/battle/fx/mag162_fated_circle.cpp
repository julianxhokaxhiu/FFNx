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

// Effect 162: Fated Circle

#include "mag_common.h"

namespace ff8fx
{
namespace circle162
{
	using namespace eng;
	using namespace magc;

	inline int32_t &CasterSlot() { return var<int32_t>(game_addr(game::MAG_162_FATED_CIRCLE_CasterSlot)); }
	inline CastContext *&Context() { return var<CastContext *>(game_addr(game::MAG_162_FATED_CIRCLE_CastContext)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::MAG_162_FATED_CIRCLE_TextureBase)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_162_FATED_CIRCLE_PacketCursor)); }
	inline int32_t &Length() { return var<int32_t>(game_addr(game::MAG_162_FATED_CIRCLE_TrailLength)); }
	inline int32_t &SwirlFlag() { return var<int32_t>(game_addr(game::MAG_162_FATED_CIRCLE_SwirlFlag)); }
	inline TaskQueue *QMaster() { return (TaskQueue *)game_addr(game::MAG_162_FATED_CIRCLE_MasterQueue); }
	inline TaskQueue *QMain() { return (TaskQueue *)game_addr(game::MAG_162_FATED_CIRCLE_MainQueue); }
	static constexpr game::Id POOL_A = game::MAG_162_FATED_CIRCLE_PoolA, POOL_A_END = game::MAG_162_FATED_CIRCLE_DrawOffsetPrim;
	static constexpr game::Id POOL_B = game::MAG_162_FATED_CIRCLE_PoolB, POOL_B_END = game::MAG_162_FATED_CIRCLE_TrailSplineBase;
	static constexpr game::Id BIND_OUT = game::MAG_162_FATED_CIRCLE_AnimBindBlock;
	static constexpr game::Id PTS_A = game::MAG_162_FATED_CIRCLE_TrailPointsBase, PTS_B = game::MAG_162_FATED_CIRCLE_TrailPointsTip;
	static constexpr game::Id SPL_A = game::MAG_162_FATED_CIRCLE_TrailSplineBase, SPL_B = game::MAG_162_FATED_CIRCLE_TrailSplineTip;
	static constexpr game::Id HIT_POS = game::MAG_162_FATED_CIRCLE_HitPoint, HIT_DIR = game::MAG_162_FATED_CIRCLE_HitDir, BASE_POS = game::MAG_162_FATED_CIRCLE_BasePoint, BASE_DIR = game::MAG_162_FATED_CIRCLE_BaseDir;
	static constexpr game::Id RING = game::MAG_162_FATED_CIRCLE_TrailRing, RING_END = game::MAG_162_FATED_CIRCLE_TrailLength;
	static constexpr game::Id MORPH_DELTA = game::MAG_162_FATED_CIRCLE_MorphDeltas, MORPH_VTX = game::MAG_162_FATED_CIRCLE_MorphVertices;
	static constexpr game::Id PRIM_ENV = game::MAG_162_FATED_CIRCLE_DrawEnvPrim, PRIM_AREA = game::MAG_162_FATED_CIRCLE_DrawAreaPrim, PRIM_OFFSET = game::MAG_162_FATED_CIRCLE_DrawOffsetPrim, OFFSET_WORDS = game::MAG_162_FATED_CIRCLE_DrawOffsetXY;

	static constexpr game::Id ORIG_Director = game::MAG_162_FATED_CIRCLE_DirectorTask, ORIG_Trail = game::MAG_162_FATED_CIRCLE_TrailTask, ORIG_Swipe = game::MAG_162_FATED_CIRCLE_SwipeTask, ORIG_Dust = game::MAG_162_FATED_CIRCLE_DustTask, ORIG_Shock = game::MAG_162_FATED_CIRCLE_ShockTask, ORIG_Glow = game::MAG_162_FATED_CIRCLE_GlowTask, ORIG_Shard = game::MAG_162_FATED_CIRCLE_ShardTask, ORIG_Orbit = game::MAG_162_FATED_CIRCLE_OrbitTask, ORIG_Burst = game::MAG_162_FATED_CIRCLE_BurstTask, ORIG_Rain = game::MAG_162_FATED_CIRCLE_RainTask, ORIG_Shell = game::MAG_162_FATED_CIRCLE_ShellTask, ORIG_Dome = game::MAG_162_FATED_CIRCLE_DomeTask, ORIG_Circle = game::MAG_162_FATED_CIRCLE_CircleTask, ORIG_Pillar = game::MAG_162_FATED_CIRCLE_PillarTask, ORIG_Ring = game::MAG_162_FATED_CIRCLE_RingTask, ORIG_Ember = game::MAG_162_FATED_CIRCLE_EmberTask, ORIG_Spark = game::MAG_162_FATED_CIRCLE_SparkTask, ORIG_Swirl = game::MAG_162_FATED_CIRCLE_SwirlTask, ORIG_Restore = game::MAG_162_FATED_CIRCLE_RestoreTextureTask, ORIG_Master = game::MAG_162_FATED_CIRCLE_MasterTask;

	static constexpr game::Id ANIM_DATA = game::MAG_162_FATED_CIRCLE_AnimSet;
	static const void *SOUND_A() { return (const void *)game_addr(game::MAG_162_FATED_CIRCLE_SoundStart); }
	static const void *SOUND_B() { return (const void *)game_addr(game::MAG_162_FATED_CIRCLE_SoundShell); }
	static constexpr game::Id MODEL_SWIPE = game::MAG_162_FATED_CIRCLE_SwipeModel, MODEL_SHOCK = game::MAG_162_FATED_CIRCLE_ShockModel, MODEL_GLOW = game::MAG_162_FATED_CIRCLE_GlowModel, MODEL_SHARD = game::MAG_162_FATED_CIRCLE_ShardModel, MODEL_BURST = game::MAG_162_FATED_CIRCLE_BurstModel, MODEL_RAIN = game::MAG_162_FATED_CIRCLE_RainModel, MODEL_SHELL = game::MAG_162_FATED_CIRCLE_ShellModel, MODEL_CIRCLE = game::MAG_162_FATED_CIRCLE_CircleModel;
	static constexpr game::Id SEQ_DUST = game::MAG_162_FATED_CIRCLE_DustSeq, SEQ_ORBIT = game::MAG_162_FATED_CIRCLE_OrbitSeq, SEQ_EMBER = game::MAG_162_FATED_CIRCLE_EmberSeq, SEQ_SPARK = game::MAG_162_FATED_CIRCLE_SparkSeq, SEQ_SWIRL = game::MAG_162_FATED_CIRCLE_SwirlSeq;
	static constexpr game::Id SHELL_FROM = game::unk_DA71A0, SHELL_TO = game::MAG_162_FATED_CIRCLE_DomeVertices;
	static constexpr game::Id CIRCLE_FROM = game::unk_DA2350, CIRCLE_TO = game::MAG_162_FATED_CIRCLE_CircleMorphTarget;
	static constexpr game::Id PILLAR_MODEL = game::MAG_162_FATED_CIRCLE_PillarModels, PILLAR_OFFX = game::MAG_162_FATED_CIRCLE_PillarOffsetX, PILLAR_OFFZ = game::MAG_162_FATED_CIRCLE_PillarOffsetZ;
	static constexpr game::Id ALT_VIEWPORT = game::MAG_162_FATED_CIRCLE_CaptureViewport;
	static constexpr game::Id TRAIL_LAST_QUAD = game::unk_23CF2C8;

	inline int32_t Normalize(const int32_t *in, int32_t *out) { return fn<int32_t (__cdecl *)(const int32_t *, int32_t *)>(game_addr(game::NormalizeVectorToFixedPoint))(in, out); }
	inline void VectorNormalS16(int16_t *in, int16_t *out) { fn<void (__cdecl *)(int16_t *, int16_t *)>(game_addr(game::VectorNormalS16))(in, out); }
	inline void GetEffectSpawnPosition(void *entity, int32_t bone, int32_t frac, int16_t *out) { fn<void (__cdecl *)(void *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, frac, out); }
	inline void CalculateCenterPosition(uint32_t mask, int16_t *out) { fn<void (__cdecl *)(uint32_t, int16_t *)>(game_addr(game::CalculateCenterPosition))(mask, out); }
	inline void MatVec(const Mat4x3 *m, const int16_t *in, int16_t *out) { fn<void (__cdecl *)(const Mat4x3 *, const int16_t *, int16_t *)>(game_addr(game::matrixMultiplyVector))(m, in, out); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void SplineSetup(int32_t n, const void *points, void *work) { fn<void (__cdecl *)(int32_t, const void *, void *)>(game_addr(game::Effect_SplineSetup))(n, points, work); }
	inline void SplineEval(int32_t n, void *work, void *out, int32_t t) { fn<void (__cdecl *)(int32_t, void *, void *, int32_t)>(game_addr(game::Effect_SplineEval))(n, work, out, t); }
	inline void GteSetFarColor(int32_t r, int32_t g, int32_t b) { fn<void (__cdecl *)(int32_t, int32_t, int32_t)>(game_addr(game::GTE_SetFarColor))(r, g, b); }
	inline void GteSetFarColor2(int32_t r, int32_t g, int32_t b) { fn<void (__cdecl *)(int32_t, int32_t, int32_t)>(game_addr(game::GTE_SetFarColorRGB))(r, g, b); }
	inline void GteLoadRGBC(const void *c) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadRGBC))(c); }
	inline void GteDPCS() { fn<void (__cdecl *)()>(game_addr(game::GTE_DPCS))(); }
	inline void GteStoreRGB2(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreRGB2))(dst); }
	inline void GteLoadRGB012(const void *a, const void *b, const void *c) { fn<void (__cdecl *)(const void *, const void *, const void *)>(game_addr(game::GTE_LoadRGB012))(a, b, c); }
	inline void GteDPCT() { fn<void (__cdecl *)()>(game_addr(game::GTE_DPCT))(); }
	inline void GteStoreRGB012(void *a, void *b, void *c) { fn<void (__cdecl *)(void *, void *, void *)>(game_addr(game::GTE_StoreRGB012))(a, b, c); }
	inline void GteLoadV012(const void *v0, const void *v1, const void *v2) { fn<void (__cdecl *)(const void *, const void *, const void *)>(game_addr(game::GTE_LoadV012))(v0, v1, v2); }
	inline void GteRTPT() { fn<void (__cdecl *)()>(game_addr(game::GTE_RTPT))(); }
	inline void GteReadFLAG(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadFLAG))(dst); }
	inline void GteReadSXY012Split(void *a, void *b, void *c) { fn<void (__cdecl *)(void *, void *, void *)>(game_addr(game::GTE_ReadSXY012_Split))(a, b, c); }
	inline void GteAVSZ3() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ3))(); }
	inline void GteAVSZ4() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ4))(); }
	inline void GteReadOTZ32(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadOTZ))(dst); }
	inline void GteNCLIP() { fn<void (__cdecl *)()>(game_addr(game::GTE_NCLIP))(); }
	inline void GteReadMAC0(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadMAC0))(dst); }
	inline void GteSetRotMatrixC(const Mat4x3 *m) { fn<void (__cdecl *)(const Mat4x3 *)>(game_addr(game::GTE_SetRotMatrix))(m); }
	inline void GteSetTransVectorC(const Mat4x3 *m) { fn<void (__cdecl *)(const Mat4x3 *)>(game_addr(game::GTE_SetTransVector))(m); }
	inline void BindEntityModelFromComData(uint8_t *entity, uint32_t out, uint32_t data) { fn<void (__cdecl *)(uint8_t *, uint32_t, uint32_t)>(game_addr(game::BattleEntity_BindExternalAnimSet))(entity, out, data); }
	inline void QueueChainTransformation(uint8_t *entity, int32_t seq) { fn<void (__cdecl *)(uint8_t *, int32_t)>(game_addr(game::QueueChainTransformation))(entity, seq); }
	inline int32_t BattleFileLoadStatus() { return fn<int32_t (__cdecl *)()>(game_addr(game::BattleFile_GetLoadingState))(); }
	inline void BattleFileLoad(int32_t file, uint32_t dst) { fn<void (__cdecl *)(int32_t, uint32_t)>(game_addr(game::BattleFile_CharacterLoad))(file, dst); }
	inline void QueueTIMUpload(uint32_t file) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::Battle_QueueTIMUpload_GetEOF))(file); }
	inline void SetDrawEnv(uint32_t prim, uint32_t env) { fn<void (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::SSIGPU_SetDrawEnv))(prim, env); }
	inline void InsertPrimAltViewport(uint32_t ot, uint32_t viewport) { fn<void (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::SSIGPU_InsertPrimAltViewport))(ot, viewport); }
	inline void SetDrawArea(uint32_t prim, const void *rect) { fn<void (__cdecl *)(uint32_t, const void *)>(game_addr(game::GPU_SetDrawAreaPrim))(prim, rect); }
	inline void SetDrawOffset(uint32_t prim, const void *xy) { fn<void (__cdecl *)(uint32_t, const void *)>(game_addr(game::SSIGPU_SetDrawOffsetPrim))(prim, xy); }
	inline void GteReadScreenOffset(int32_t *x, int32_t *y) { fn<void (__cdecl *)(int32_t *, int32_t *)>(game_addr(game::GTE_ReadScreenOffset))(x, y); }
	inline void GteSetScreenOffset(int32_t x, int32_t y) { fn<void (__cdecl *)(int32_t, int32_t)>(game_addr(game::Call_Bs_ParseCamera))(x, y); }
	inline uint32_t RenderGeometry(uint32_t model, void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(uint32_t, void *, uint32_t, int32_t, uint32_t)>(game_addr(game::RenderGeometry))(model, h, ot, mode, cursor); }
	inline void ComputeBonesWorldMatricesM(uint32_t anim_header, const Mat4x3 *root) { fn<void (__cdecl *)(uint32_t, const Mat4x3 *)>(game_addr(game::BS_ComputeBonesWorldMatrices))(anim_header, root); }

#pragma pack(push, 1)
	struct RootNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t done;
	};
	struct Node24
	{
		TaskNode hdr;
		int16_t c;
		int16_t e;
		int16_t w10, w12, w14, w16;
		int16_t w18;
		int16_t w1A;
		int16_t w1C;
		int16_t w1E;
		int16_t w20;
		int16_t w22;
	};
	struct Part
	{
		uint32_t flags;
		int16_t life;
		int16_t scale;
		int16_t pos[3];
		int16_t w0E;
		int16_t v[3];
		int16_t w16;
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
	struct MeshHeader
	{
		uint32_t model;
		uint32_t verts;
		uint8_t far[3];
		uint8_t pad0B;
		int32_t fade;
		int32_t scroll;
		int32_t vscroll;
		int16_t tpage[4];
		uint32_t mode;
		uint32_t faces;
		int32_t mac0;
		uint32_t pad2C;
		int32_t otz;
		uint32_t flag;
		int16_t v0[4], v1[4], v2[4];
		int16_t pad50[4];
		int16_t off[3];
		int16_t pad5E;
		int16_t ang[4];
		int16_t pad68[2];
		int16_t win[4];
		int32_t uv[3];
		uint32_t pad80[2];
		int32_t vmod;
		Mat4x3 cam;
		Mat4x3 face;
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x10 && sizeof(Node24) == 0x24 && sizeof(Part) == 0x18, "Fated Circle nodes");
	static_assert(sizeof(RingEntry) == 0x14 && sizeof(TrailWork) == 0x40 && sizeof(MeshHeader) == 0xCC, "Fated Circle records");

	inline uint8_t *Caster() { return Entity(CasterSlot()); }
	inline Node24 *AddMain(uint32_t fn) { return (Node24 *)AddTaskToQueue(QMain(), fn); }

	static int32_t FreeSlot(uint32_t pool, uint32_t end, int32_t max)
	{
		int32_t i = 0;
		for (uint32_t a = pool; *(const uint32_t *)a != 0; i++)
		{
			a += 0x18;
			if (a >= end) return -1;
		}
		return i >= max ? -1 : i;
	}
	inline Part *PartAt(uint32_t pool, int32_t i) { return (Part *)(pool + 0x18 * i); }

	static uint32_t g_slot28 = 0;
	static bool g_slot28_known = false;
}
}

namespace ff8fx
{
namespace circle162
{
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

	static void MorphDeltas(const int16_t *from, const int16_t *to, int32_t n, int32_t steps)
	{
		int16_t *d = (int16_t *)game_addr(MORPH_DELTA);
		for (; n > 0; n--, from += 4, to += 4, d += 4)
		{
			d[0] = (int16_t)(((int32_t)to[0] - from[0]) / steps);
			d[1] = (int16_t)(((int32_t)to[1] - from[1]) / steps);
			d[2] = (int16_t)(((int32_t)to[2] - from[2]) / steps);
		}
	}

	static void CaptureStage(int16_t *pos);

	static uint32_t __cdecl DirectorTask(TaskNode *tn)
	{
		g_slot28_known = false;
		if (UpdateFlags() & 0x201)
		{
			if (UpdateFlags() & 1) return 0;
			if (BattleFileLoadStatus() < 0) return 0;
		}
		Node24 *n = (Node24 *)tn;
		if (n->c == 1)
		{
			BindEntityModelFromComData(Caster(), game_addr(BIND_OUT), game_addr(ANIM_DATA));
			QueueChainTransformation(Caster(), 1);
		}
		if (n->c == 2)
		{
			AddMain(game_addr(ORIG_Trail))->c = 0;
			for (uint32_t a = game_addr(RING); a < game_addr(RING_END); a += 0x14) var<int32_t>(a) = 0;
		}
		if (n->c == 6)
		{
			int16_t q[4];
			GetDefaultEffectPosition(Caster(), q);
			const int16_t h = q[3];
			q[3] = q[1];
			q[1] = h;
			Node24 *t = AddMain(game_addr(ORIG_Swipe));
			t->c = 0;
			ff8fx::copy<(8)>(&t->w10, q);
			t->w18 = (int16_t)(CrtRand() % 0x1000);
			const int32_t r = CrtRand() % 0x28;
			t->w1C = 0xD00;
			t->w1A = (int16_t)(-0x32 - r);
			t = AddMain(game_addr(ORIG_Dust));
			t->c = 0;
			ff8fx::copy<(8)>(&t->w10, q);
			t = AddMain(game_addr(ORIG_Shock));
			t->c = 0;
			ff8fx::copy<(8)>(&t->w10, q);
			t->w1E = 0x400;
			t->w1C = 0x400;
		}
		if (n->c == 0x12)
		{
			int16_t q[4];
			ff8fx::copy<(8)>(q, (const void *)game_addr(BASE_POS));
			const int32_t *d = (const int32_t *)game_addr(BASE_DIR);
			q[0] = (int16_t)((mul32(d[0], 444) >> 12) + q[0]);
			q[1] = (int16_t)(q[1] + (int16_t)(mul32(d[1], 444) >> 12));
			q[2] = (int16_t)(q[2] + (int16_t)(mul32(d[2], 444) >> 12));
			ff8fx::copy<(8)>(&n->w10, q);
			Node24 *t = AddMain(game_addr(ORIG_Glow));
			t->c = 0;
			ff8fx::copy<(8)>(&t->w10, q);
			t->w1C = 0xC0;
			t->w1E = 0x60;
			t = AddMain(game_addr(ORIG_Shard));
			t->c = 0;
			ff8fx::copy<(8)>(&t->w10, q);
			t = AddMain(game_addr(ORIG_Orbit));
			t->c = 0;
			ff8fx::copy<(8)>(&t->w10, q);
		}
		if (n->c == 0x1F)
		{
			Node24 *t = AddMain(game_addr(ORIG_Burst));
			ff8fx::copy<(4)>(&t->w10, &n->w10);
			t->c = 0;
			ff8fx::copy<(4)>(&t->w14, &n->w14);
		}
		if (n->c == 0x28)
		{
			Node24 *t = AddMain(game_addr(ORIG_Rain));
			t->c = 0;
			GetEffectSpawnPosition(Caster(), 0xF0, 0, &t->w10);
			int16_t v[3];
			v[0] = 0;
			v[1] = 0;
			v[2] = 0xA28;
			MatVec((const Mat4x3 *)(Caster() + 0x40), v, v);
			t->w10 = (int16_t)(t->w10 + v[0]);
			t->w12 = (int16_t)(t->w12 + v[1]);
			t->w14 = (int16_t)(t->w14 + v[2]);
		}
		if (n->c == 0x39)
		{
			Node24 *t = AddMain(game_addr(ORIG_Shell));
			t->c = 0;
			GetDefaultEffectPosition(Caster(), &t->w10);
			t->w1C = 0x500;
			t->w1E = 0xD5;
			t->w18 = *(const int16_t *)(Caster() + 0xE);
			t = AddMain(game_addr(ORIG_Dome));
			t->c = 0;
			t->e = 0x16;
			CalculateCenterPosition(Context()->target_mask, &t->w10);
			t->w14 = (int16_t)(t->w14 + 0xBB8);
			t->w12 = (int16_t)0xFA88;
			t->w18 = (int16_t)(CrtRand() % 0x1000);
			t->w1C = 0x500;
			t->w1E = 0x100;
		}
		if (n->c == 0x47)
		{
			CalculateCenterPosition(Context()->target_mask, &n->w10);
			n->w14 = (int16_t)(n->w14 + (int16_t)0xFCE0);
			CaptureStage(&n->w10);
			BattleFileLoad(0x219, TexBase() + 0x10000);
		}
		if (n->c == 0x5D)
		{
			Node24 *t = AddMain(game_addr(ORIG_Swirl));
			ff8fx::copy<(4)>(&t->w10, &n->w10);
			t->c = 0;
			ff8fx::copy<(4)>(&t->w14, &n->w14);
			SwirlFlag() = 1;
		}
		if (n->c == 0x6E) ScreenFadeTask(0, 1, 0xA, 0xFF);
		if (n->c == 0x6C)
		{
			if (BattleFileLoadStatus() < 0) return 0;
			QueueTIMUpload(TexBase() + 0x10000);
			((RootNode *)AddTaskToQueue(QMaster(), game_addr(ORIG_Restore)))->counter = 0;
		}
		if (n->c == 0x6F)
		{
			Node24 *t = AddMain(game_addr(ORIG_Circle));
			ff8fx::copy<(4)>(&t->w10, &n->w10);
			ff8fx::copy<(4)>(&t->w14, &n->w14);
			t->w14 = (int16_t)(t->w14 + 0x8FC);
			t->c = 0;
			t->w1E = 0x380;
			t->w1C = 0x380;
			MorphDeltas((const int16_t *)game_addr(CIRCLE_FROM), (const int16_t *)game_addr(CIRCLE_TO), 0x5D, 0x18);
			SwirlFlag() = 0;
			for (int32_t i = 0; i < 8; i++)
			{
				Node24 *p = AddMain(game_addr(ORIG_Pillar));
				ff8fx::copy<(4)>(&p->w10, &n->w10);
				p->c = 0;
				p->e = (int16_t)i;
				ff8fx::copy<(4)>(&p->w14, &n->w14);
				p->w16 = (int16_t)(-0x12C - CrtRand() % 0x12C);
				p->w18 = (int16_t)(-0x1CC - CrtRand() % 0xB4);
				int32_t delay = (CrtRand() % 3) * i;
				delay += CrtRand() % 9;
				p->w1A = (int16_t)delay;
				p->w1C = 0;
				const int32_t r = CrtRand() % 0x19;
				p->w20 = 0;
				p->w1E = (int16_t)(r + 0x14);
				const int32_t s = CrtRand() % 0x1E + 0x23;
				p->w22 = (int16_t)s;
				if (p->w1E & 0x801) p->w22 = (int16_t)-s;
				Node24 *g = AddMain(game_addr(ORIG_Ring));
				g->e = (int16_t)i;
				ff8fx::copy<(4)>(&g->w10, &n->w10);
				g->c = 0;
				ff8fx::copy<(4)>(&g->w14, &n->w14);
				g->w1A = p->w1A;
			}
			Node24 *u = AddMain(game_addr(ORIG_Ember));
			ff8fx::copy<(4)>(&u->w10, &n->w10);
			ff8fx::copy<(4)>(&u->w14, &n->w14);
			u->w14 = (int16_t)(u->w14 + 0xA8C);
			u->c = 0;
		}
		if (n->c == 0x74)
		{
			ActionData *a = Context()->actions;
			for (int32_t i = 0; i < a->target_count; i++)
			{
				const uint8_t slot = a->targets[0x18 * i];
				Node24 *t = AddMain(game_addr(ORIG_Spark));
				t->c = 0;
				GetDefaultEffectPosition(Entity(slot), &t->w10);
				t->w1C = (int16_t)(mul32(*(const int16_t *)(Entity(slot) + 0x26), 2200) >> 12);
			}
		}
		if (n->c == 0x96) ApplyActionResultToTargets(Context()->actions->targets, Context()->actions->target_count);
		if (n->c >= 0xA2 && n->c > 4) SetScreenFlash((uint32_t)shl32(0xAA - n->c, 8), 0);
		if (n->c == 1) BdPlaySE(SOUND_A(), 0x8000, 0x80);
		if (n->c == 0x39) BdPlaySE(SOUND_B(), 0x8000, 0x80);
		n->c++;
		if (n->c <= 0xAA) return 0;
		SetScreenFlash(0, 0);
		return TASK_END;
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

	static void TrailHitPoint(const uint8_t *sa, const uint8_t *sb)
	{
		var<uint32_t>(game_addr(HIT_POS)) = *(const uint32_t *)sb;
		var<uint32_t>(game_addr(HIT_POS) + 4) = *(const uint32_t *)(sb + 4);
		int32_t *d = (int32_t *)game_addr(HIT_DIR);
		d[0] = (int32_t)*(const int16_t *)sb - *(const int16_t *)(sb + 8);
		d[1] = (int32_t)*(const int16_t *)(sb + 2) - *(const int16_t *)(sb + 0xA);
		d[2] = (int32_t)*(const int16_t *)(sb + 4) - *(const int16_t *)(sb + 0xC);
		Normalize(d, d);
		var<uint32_t>(game_addr(BASE_POS)) = *(const uint32_t *)sa;
		var<uint32_t>(game_addr(BASE_POS) + 4) = *(const uint32_t *)(sa + 4);
		int32_t *b = (int32_t *)game_addr(BASE_DIR);
		b[0] = (int32_t)var<int16_t>(game_addr(HIT_POS)) - *(const int16_t *)sa;
		b[1] = (int32_t)var<int16_t>(game_addr(HIT_POS) + 2) - *(const int16_t *)(sa + 2);
		b[2] = (int32_t)var<int16_t>(game_addr(HIT_POS) + 4) - *(const int16_t *)(sa + 4);
		Normalize(b, b);
	}

	static bool TrailDraw(const uint8_t *sa, const uint8_t *sb, TrailWork *w)
	{
		GteSetRotMatrix(&Camera());
		GteSetTransVector(&Camera());
		GteSetFarColor2(0, 0, 0);
		uint32_t *pk = (uint32_t *)PacketCursor();
		uint32_t *pk2 = pk + 9;
		w->base_col = 0x3A903030;
		w->col1 = 0x3A903030;
		int32_t tacc = 0;
		bool clipped = false;
		for (int32_t off = 0; off < 0xF0; off += 8, tacc += 0x1000)
		{
			w->col0 = w->col1;
			pk[0] = 0x8000000;
			GteLoadV012(sa + off, sa + off + 8, sb + off);
			GteRTPT();
			GteReadFLAG(&w->flag);
			clipped = (w->flag & 0x60000) != 0;
			if (clipped) continue;
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
			pk2[2] = pk[2];
			pk2[4] = pk[4];
			pk2[6] = pk[6];
			pk2[8] = pk[8];
			pk2[0] = 0x8000000;
			InsertPrimAutoDepth(var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 4 * (uint32_t)(w->otz >> 2) + 0x44, pk2);
			pk += 0x12;
			pk2 += 0x12;
		}
		PacketCursor() = (uint32_t)pk2;
		return clipped;
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
		const int32_t want = c >= Length() - 7 ? Length() - c : 7;
		const int32_t cnt = TrailGather((const uint8_t *)game_addr(RING), idx, want, (uint8_t *)game_addr(PTS_A), (uint8_t *)game_addr(PTS_B));
		g_slot28 = 1;
		if (cnt > 1)
		{
			TrailSplines(cnt, (const uint8_t *)game_addr(PTS_A), (const uint8_t *)game_addr(PTS_B), (uint8_t *)game_addr(SPL_A), (uint8_t *)game_addr(SPL_B));
			TrailHitPoint((const uint8_t *)game_addr(SPL_A), (const uint8_t *)game_addr(SPL_B));
			g_slot28 = TrailDraw((const uint8_t *)game_addr(SPL_A), (const uint8_t *)game_addr(SPL_B), w) ? (uint32_t)&w->flag : (uint32_t)(game_addr(TRAIL_LAST_QUAD) + 0xE8);
		}
		g_slot28_known = true;
		FieldFree(0x40);
		if (DrawOnly()) return 0;
		n->c++;
		return n->c >= Length() - 1 ? TASK_END : 0;
	}

	static inline int32_t DivNeg2000(int32_t v)
	{
		const int32_t q = (int32_t)(((int64_t)v * (int32_t)0xEF9DB22D) >> 39);
		return q + (int32_t)((uint32_t)q >> 31);
	}

	static void SwipeDraw(const Node24 *n, int16_t yaw, int32_t c, int32_t fade)
	{
		int16_t ang[3];
		ang[0] = 0;
		ang[1] = yaw;
		ang[2] = 0;
		Mat4x3 m;
		BuildRotationMatrixFromAngles(ang, &m);
		int32_t sv[3];
		m.t[0] = n->w10;
		m.t[1] = n->w12;
		sv[2] = n->w1C;
		sv[0] = n->w1C;
		m.t[2] = n->w14;
		sv[1] = DivNeg2000(shl32(*(const int16_t *)(Caster() + 0x3C), 12));
		Scale3DMatrix(&m, sv);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		uint8_t *h = AllocHeader(0x58);
		*(uint32_t *)h = game_addr(MODEL_SWIPE);
		*(uint32_t *)(h + 0x1C) = 0x30;
		if (c >= 3)
		{
			h[0xA] = 0;
			h[9] = 0;
			h[8] = 0;
			*(uint32_t *)(h + 0x1C) = 0xF0;
			*(int32_t *)(h + 0xC) = fade;
		}
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static uint32_t __cdecl SwipeTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		g_slot28_known = false;
		SwipeDraw(n, n->w18, n->c, mul32(n->c - 3, 409));
		if (DrawOnly()) return 0;
		const int16_t rate = n->w1A;
		n->w18 = (int16_t)(n->w18 + rate);
		n->c++;
		n->w1A = (int16_t)(rate - (int16_t)(rate >> 4));
		return n->c >= 0xD ? TASK_END : 0;
	}

	static void ShockDraw(const Node24 *n, int16_t s, int32_t c, int32_t fade)
	{
		int16_t ang[3];
		ang[0] = 0;
		ang[1] = 0;
		ang[2] = 0;
		Mat4x3 m;
		BuildRotationMatrixFromAngles(ang, &m);
		int32_t sv[3];
		sv[1] = 0x1000;
		m.t[1] = n->w12;
		m.t[0] = n->w10;
		sv[2] = s;
		sv[0] = s;
		m.t[2] = n->w14;
		Scale3DMatrix(&m, sv);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		uint8_t *h = AllocHeader(0x58);
		*(uint32_t *)h = game_addr(MODEL_SHOCK);
		*(uint32_t *)(h + 0x1C) = 3;
		if (c >= 4)
		{
			h[0xA] = 0;
			h[9] = 0;
			h[8] = 0;
			*(int32_t *)(h + 0xC) = fade;
			*(uint32_t *)(h + 0x1C) = 0xC3;
		}
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static uint32_t __cdecl ShockTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		g_slot28_known = false;
		ShockDraw(n, n->w1C, n->c, shl32(n->c - 4, 9));
		if (DrawOnly()) return 0;
		const int16_t rate = n->w1E;
		n->w1C = (int16_t)(n->w1C + rate);
		n->c++;
		n->w1E = (int16_t)(rate - rate / 4);
		return n->c >= 0xC ? TASK_END : 0;
	}

	static void GlowDraw(const Node24 *n, int16_t s, int32_t c, int32_t fade)
	{
		TransformCameraByShadowRotation(&n->w10, s, -0x100);
		uint8_t *h = AllocHeader(0x58);
		*(uint32_t *)h = game_addr(MODEL_GLOW);
		*(uint32_t *)(h + 0x1C) = 0;
		if (c >= 0x1C)
		{
			*(uint32_t *)(h + 8) = 0;
			*(uint32_t *)(h + 0x1C) = 0xC0;
			*(int32_t *)(h + 0xC) = fade;
		}
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static uint32_t __cdecl GlowTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		g_slot28_known = false;
		GlowDraw(n, n->w1C, n->c, shl32(n->c - 0x1C, 9));
		if (DrawOnly()) return 0;
		const int16_t rate = n->w1E;
		n->w1C = (int16_t)(n->w1C + rate);
		n->c++;
		n->w1E = (int16_t)(rate + (int16_t)(rate >> 6));
		return n->c >= 0x24 ? TASK_END : 0;
	}

	static void ShadowSprite(uint8_t *h, const int16_t *pos, int16_t scale, int16_t life)
	{
		*(int16_t *)(h + 4) = life;
		TransformCameraByShadowRotation(pos, scale, -(scale >> 4));
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
	}

	static uint32_t __cdecl DustTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		g_slot28_known = false;
		uint8_t *h = AllocHeader(0xB4);
		*(uint32_t *)h = game_addr(SEQ_DUST);
		*(int16_t *)(h + 0x24) = 0;
		int32_t count = 0;
		for (uint32_t a = game_addr(POOL_B); a < game_addr(POOL_B_END); a += 0x18)
		{
			Part *p = (Part *)a;
			if (!(p->flags & 1)) continue;
			ShadowSprite(h, p->pos, p->scale, p->life);
			if (DrawOnly()) continue;
			p->life++;
			if (*(const int16_t *)(h + 0x28) < 0)
			{
				p->flags = 0;
				continue;
			}
			p->pos[0] = (int16_t)(p->pos[0] + p->v[0]);
			p->pos[2] = (int16_t)(p->pos[2] + p->v[2]);
			p->v[0] = (int16_t)(p->v[0] - (int16_t)(p->v[0] >> 3));
			p->v[2] = (int16_t)(p->v[2] - (int16_t)(p->v[2] >> 3));
			count++;
		}
		FieldFree(0xB4);
		if (DrawOnly()) return 0;
		if (n->c >= 1 && n->c <= 2)
		{
			for (int32_t j = 0; j < 0xA; j++)
			{
				const int32_t i = FreeSlot(game_addr(POOL_B), game_addr(POOL_B_END), 0x50);
				if (i < 0) break;
				Part *p = PartAt(game_addr(POOL_B), i);
				p->flags = 1;
				p->life = 0;
				p->scale = (int16_t)(CrtRand() % 0x500 + 0x900);
				ff8fx::copy<(8)>(p->pos, &n->w10);
				p->pos[0] = (int16_t)(p->pos[0] + (int16_t)(CrtRand() % 0x190 - 0xC8));
				int32_t d[3];
				const int32_t r = CrtRand() % 0x190;
				d[1] = 0;
				p->pos[2] = (int16_t)(p->pos[2] + (int16_t)(r - 0xC8));
				d[0] = (int32_t)p->pos[0] - n->w10;
				d[2] = (int32_t)p->pos[2] - n->w14;
				Normalize(d, d);
				const int32_t s = CrtRand() % 0x5A + 0x1E;
				p->v[0] = (int16_t)(mul32(s, d[0]) >> 12);
				p->v[1] = 0;
				p->v[2] = (int16_t)(mul32(s, d[2]) >> 12);
			}
		}
		n->c++;
		return n->c >= 0x10 && count == 0 ? TASK_END : 0;
	}

	struct ShardWork
	{
		int32_t ref[3];
		int32_t v[3];
		int32_t axis[3];
		Mat4x3 m1, m2;
		int32_t sv[3];
	};

	static void ShardDraw(ShardWork *w, uint8_t *h, const Part *p, int16_t scale, int32_t fade)
	{
		int16_t ang[3];
		ang[0] = 0;
		ang[1] = 0;
		ang[2] = 0;
		BuildRotationMatrixFromAngles(ang, &w->m1);
		w->m1.t[0] = p->pos[0];
		w->m1.t[1] = p->pos[1];
		w->m1.t[2] = p->pos[2];
		w->v[0] = p->v[0];
		w->v[1] = p->v[1];
		w->v[2] = p->v[2];
		Normalize(w->v, w->v);
		const int32_t angle = RotationBetweenVectors(w->ref, w->v, w->axis);
		BuildAxisAngleRotationMatrix(angle, &w->m2, w->axis);
		MatrixMultiply(&w->m2, &w->m1);
		w->sv[2] = 0x400;
		w->sv[0] = 0x400;
		w->sv[1] = scale;
		Scale3DMatrix(&w->m1, w->sv);
		ComposeAffineTransform(&Camera(), &w->m1, &w->m1);
		GteSetRotMatrix(&w->m1);
		GteSetTransVector(&w->m1);
		if (p->life >= 4)
		{
			*(int32_t *)(h + 0xC) = fade;
			*(uint32_t *)(h + 0x1C) |= 0xC0;
		}
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
	}

	static uint32_t __cdecl ShardTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		g_slot28_known = false;
		uint8_t *h = AllocHeader(0x58);
		*(uint32_t *)h = game_addr(MODEL_SHARD);
		h[0xA] = 0;
		h[9] = 0;
		h[8] = 0;
		*(uint32_t *)(h + 0x1C) = 3;
		int32_t count = 0;
		ShardWork w;
		w.ref[0] = 0;
		w.ref[1] = -0x1000;
		w.ref[2] = 0;
		for (uint32_t a = game_addr(POOL_B); a < game_addr(POOL_B_END); a += 0x18)
		{
			Part *p = (Part *)a;
			if (!(p->flags & 2)) continue;
			ShardDraw(&w, h, p, p->scale, shl32(p->life - 4, 0xA));
			if (DrawOnly()) continue;
			p->life++;
			if (p->life >= 8)
			{
				p->flags = 0;
				continue;
			}
			p->scale = (int16_t)(p->scale - (int16_t)(p->scale >> 4));
			p->pos[0] = (int16_t)(p->pos[0] + p->v[0]);
			p->pos[1] = (int16_t)(p->pos[1] + p->v[1]);
			p->pos[2] = (int16_t)(p->pos[2] + p->v[2]);
			count++;
		}
		FieldFree(0x58);
		if (DrawOnly()) return 0;
		if (n->c >= 0 && n->c <= 0xE)
		{
			for (int32_t j = 0; j < 3; j++)
			{
				const int32_t i = FreeSlot(game_addr(POOL_B), game_addr(POOL_B_END), 0x50);
				if (i < 0) break;
				Part *p = PartAt(game_addr(POOL_B), i);
				p->flags = 2;
				p->life = 0;
				p->scale = (int16_t)(CrtRand() % 0x600 + 0x400);
				ff8fx::copy<(8)>(p->pos, &n->w10);
				int32_t d[3];
				d[0] = CrtRand() % 0x1000 - 0x800;
				d[1] = CrtRand() % 0x1000 - 0x800;
				d[2] = CrtRand() % 0x1000 - 0x800;
				Normalize(d, d);
				const int32_t s = CrtRand() % 0x320 + 0x320;
				p->pos[0] = (int16_t)(p->pos[0] + (int16_t)(mul32(s, d[0]) >> 12));
				p->pos[1] = (int16_t)(p->pos[1] + (int16_t)(mul32(s, d[1]) >> 12));
				p->pos[2] = (int16_t)(p->pos[2] + (int16_t)(mul32(s, d[2]) >> 12));
				const int32_t t = CrtRand() % 0x82 + 0x32;
				p->v[0] = (int16_t)(-mul32(t, d[0]) >> 12);
				p->v[1] = (int16_t)(-mul32(t, d[1]) >> 12);
				p->v[2] = (int16_t)(-mul32(t, d[2]) >> 12);
			}
		}
		n->c++;
		return n->c >= 8 && count == 0 ? TASK_END : 0;
	}

	static void OrbitSprite(uint8_t *h, const Node24 *n, const Part *p, int16_t angle, int16_t radius)
	{
		*(int16_t *)(h + 4) = p->life;
		int16_t u[3];
		u[2] = radius;
		int32_t axis[3];
		axis[0] = p->pos[0];
		axis[2] = p->pos[2];
		axis[1] = p->pos[1];
		Mat4x3 m;
		u[0] = 0;
		u[1] = 0;
		BuildAxisAngleRotationMatrix(angle, &m, axis);
		MatVec(&m, u, u);
		u[1] = (int16_t)(u[1] + n->w12);
		u[0] = (int16_t)(u[0] + n->w10);
		u[2] = (int16_t)(u[2] + n->w14);
		TransformCameraByShadowRotation(u, p->scale, -(p->scale >> 4));
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
	}

	static uint32_t __cdecl OrbitTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		g_slot28_known = false;
		uint8_t *h = AllocHeader(0xB4);
		*(uint32_t *)h = game_addr(SEQ_ORBIT);
		*(int16_t *)(h + 0x24) = 0;
		for (uint32_t a = game_addr(POOL_A); a < game_addr(POOL_A_END); a += 0x18)
		{
			Part *p = (Part *)a;
			if (!(p->flags & 1)) continue;
			OrbitSprite(h, n, p, p->w0E, p->v[0]);
			if (DrawOnly()) continue;
			p->life++;
			if (*(const int16_t *)(h + 0x28) < 0)
			{
				p->flags = 0;
				continue;
			}
			p->w0E = (int16_t)(p->w0E + p->w16);
			p->v[0] = (int16_t)(p->v[0] - p->v[1]);
		}
		FieldFree(0xB4);
		if (DrawOnly()) return 0;
		if (n->c >= 0 && n->c <= 4)
		{
			for (int32_t j = 0; j < 6; j++)
			{
				const int32_t i = FreeSlot(game_addr(POOL_A), game_addr(POOL_A_END), 0x28);
				if (i < 0) break;
				Part *p = PartAt(game_addr(POOL_A), i);
				p->flags = 1;
				p->life = 0;
				p->scale = (int16_t)(CrtRand() % 0x200 + 0x180);
				p->pos[0] = (int16_t)(CrtRand() % 0x1000 - 0x800);
				p->pos[1] = (int16_t)(CrtRand() % 0x1000 - 0x800);
				p->pos[2] = (int16_t)(CrtRand() % 0x1000 - 0x800);
				VectorNormalS16(p->pos, p->pos);
				p->w0E = (int16_t)(CrtRand() % 0x1000);
				p->v[0] = (int16_t)(CrtRand() % 0x384 + 0x258);
				const int32_t r = CrtRand() % 0xA;
				p->v[1] = (int16_t)(p->v[0] / (r + 0x10));
				p->w16 = (int16_t)(CrtRand() % 0x32 + 0x1E);
			}
		}
		n->c++;
		if (n->c < 0x17) return 0;
		for (uint32_t a = game_addr(POOL_A); a < game_addr(POOL_A_END); a += 0x18)
			if (*(const uint8_t *)a & 1) *(uint32_t *)a = 0;
		return TASK_END;
	}

	struct BurstWork
	{
		int32_t ref[3];
		int32_t v[3];
		int32_t axis[3];
		Mat4x3 m1, m2;
		int32_t sv[3];
	};

	static void BurstDraw(BurstWork *w, uint8_t *h, const Node24 *n, const Part *p, int16_t spin, int16_t scale, int32_t fade)
	{
		int16_t ang[3];
		ang[0] = 0;
		ang[1] = 0;
		ang[2] = 0;
		BuildRotationMatrixFromAngles(ang, &w->m1);
		w->m1.t[0] = n->w10;
		w->m1.t[1] = n->w12;
		w->m1.t[2] = n->w14;
		w->v[0] = p->pos[0];
		w->v[1] = p->pos[1];
		w->v[2] = p->pos[2];
		Normalize(w->v, w->v);
		int32_t angle = RotationBetweenVectors(w->ref, w->v, w->axis);
		BuildAxisAngleRotationMatrix(angle, &w->m2, w->axis);
		MatrixMultiply(&w->m2, &w->m1);
		const int32_t ax = w->axis[0];
		w->axis[0] = w->axis[2];
		w->axis[2] = ax;
		BuildAxisAngleRotationMatrix(spin, &w->m2, w->axis);
		MatrixMultiply(&w->m2, &w->m1);
		w->sv[1] = scale;
		w->sv[2] = scale;
		w->sv[0] = scale;
		Scale3DMatrix(&w->m1, w->sv);
		ComposeAffineTransform(&Camera(), &w->m1, &w->m1);
		GteSetRotMatrix(&w->m1);
		GteSetTransVector(&w->m1);
		if (p->life >= 4)
		{
			*(uint32_t *)(h + 0x1C) |= 0xC0;
			*(int32_t *)(h + 0xC) = fade;
		}
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
	}

	static uint32_t __cdecl BurstTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		g_slot28_known = false;
		uint8_t *h = AllocHeader(0x58);
		*(uint32_t *)h = game_addr(MODEL_BURST);
		h[0xA] = 0;
		h[9] = 0;
		h[8] = 0;
		*(uint32_t *)(h + 0x1C) = 0x33;
		int32_t count = 0;
		BurstWork w;
		w.ref[0] = 0;
		w.ref[1] = -0x1000;
		w.ref[2] = 0;
		for (uint32_t a = game_addr(POOL_B); a < game_addr(POOL_B_END); a += 0x18)
		{
			Part *p = (Part *)a;
			if (!(p->flags & 4)) continue;
			BurstDraw(&w, h, n, p, p->w0E, p->scale, shl32(p->life - 4, 0xA));
			if (DrawOnly()) continue;
			p->life++;
			if (p->life >= 8)
			{
				p->flags = 0;
				continue;
			}
			p->scale = (int16_t)(p->scale + (int16_t)(p->scale / 6));
			p->w0E = (int16_t)(p->w0E + p->w16);
			count++;
		}
		FieldFree(0x58);
		if (DrawOnly()) return 0;
		if (n->c >= 0 && n->c <= 1)
		{
			for (int32_t j = 0; j < 0xA; j++)
			{
				const int32_t i = FreeSlot(game_addr(POOL_B), game_addr(POOL_B_END), 0x50);
				if (i < 0) break;
				Part *p = PartAt(game_addr(POOL_B), i);
				p->flags = 4;
				p->life = 0;
				p->scale = (int16_t)(CrtRand() % 0x600 + 0x400);
				p->pos[0] = (int16_t)(CrtRand() % 0x1000 - 0x800);
				p->pos[1] = (int16_t)(CrtRand() % 0x1000 - 0x800);
				p->pos[2] = (int16_t)(CrtRand() % 0x1000 - 0x800);
				VectorNormalS16(p->pos, p->pos);
				p->w0E = (int16_t)(CrtRand() % 0x1000);
				p->w16 = (int16_t)(CrtRand() % 0x1E + 0xA);
			}
		}
		n->c++;
		return n->c >= 8 && count == 0 ? TASK_END : 0;
	}

	struct RainWork
	{
		int32_t ref[3];
		int32_t look[3];
		int32_t v[3];
		int32_t axis[3];
		Mat4x3 m1, m2;
		int32_t sv[3];
	};

	static void RainDraw(RainWork *w, uint8_t *h, const Part *p, const int16_t *pos, int32_t fade)
	{
		int16_t ang[3];
		ang[0] = 0;
		ang[1] = 0;
		ang[2] = 0;
		BuildRotationMatrixFromAngles(ang, &w->m1);
		w->m1.t[0] = (mul32(w->look[0], 800) >> 12) + pos[0];
		w->m1.t[1] = (mul32(w->look[1], 800) >> 12) + pos[1];
		w->v[0] = p->v[0];
		w->m1.t[2] = (mul32(w->look[2], 800) >> 12) + pos[2];
		w->v[2] = p->v[2];
		w->v[1] = p->v[1];
		Normalize(w->v, w->v);
		const int32_t angle = RotationBetweenVectors(w->ref, w->v, w->axis);
		BuildAxisAngleRotationMatrix(angle, &w->m2, w->axis);
		MatrixMultiply(&w->m2, &w->m1);
		w->sv[0] = p->scale;
		Scale3DMatrix(&w->m1, w->sv);
		ComposeAffineTransform(&Camera(), &w->m1, &w->m1);
		GteSetRotMatrix(&w->m1);
		GteSetTransVector(&w->m1);
		if (p->life >= 1)
		{
			*(int32_t *)(h + 0xC) = fade;
			*(uint32_t *)(h + 0x1C) |= 0xC0;
		}
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
	}

	static void RainLook(RainWork *w)
	{
		w->look[0] = (int32_t)var<int16_t>(game_addr(game::Battle_Camera_LookAt_XZ_s16)) - var<int16_t>(game_addr(game::Battle_Camera_world_XZ_s16));
		w->look[1] = (int32_t)var<int16_t>(game_addr(game::unk_B8B7FA)) - var<int16_t>(game_addr(game::unk_B8B7F2));
		w->look[2] = (int32_t)var<int16_t>(game_addr(game::Battle_Camera_LookAt_Y)) - var<int16_t>(game_addr(game::Battle_Camera_world_Y));
		Normalize(w->look, w->look);
	}

	static uint32_t __cdecl RainTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		g_slot28_known = false;
		uint8_t *h = AllocHeader(0x58);
		RainWork w;
		*(uint32_t *)h = game_addr(MODEL_RAIN);
		h[0xA] = 0;
		h[9] = 0;
		h[8] = 0;
		*(uint32_t *)(h + 0x1C) = 0x33;
		w.sv[2] = 0x580;
		w.sv[1] = 0x580;
		int32_t count = 0;
		w.ref[0] = -0x1000;
		w.ref[1] = 0;
		w.ref[2] = 0;
		RainLook(&w);
		for (uint32_t a = game_addr(POOL_B); a < game_addr(POOL_B_END); a += 0x18)
		{
			Part *p = (Part *)a;
			if (!(p->flags & 8)) continue;
			RainDraw(&w, h, p, p->pos, (int32_t)shl32(p->life, 0xB) - 0x800);
			if (DrawOnly()) continue;
			p->life++;
			if (p->life >= 3)
			{
				p->flags = 0;
				continue;
			}
			p->pos[0] = (int16_t)(p->pos[0] + p->v[0]);
			p->pos[1] = (int16_t)(p->pos[1] + p->v[1]);
			p->pos[2] = (int16_t)(p->pos[2] + p->v[2]);
			p->v[0] = (int16_t)(p->v[0] + (int16_t)(p->v[0] >> 4));
			p->v[1] = (int16_t)(p->v[1] + (int16_t)(p->v[1] >> 4));
			p->v[2] = (int16_t)(p->v[2] + (int16_t)(p->v[2] >> 4));
			count++;
		}
		FieldFree(0x58);
		if (DrawOnly()) return 0;
		if (n->c >= 0 && n->c <= 0xA)
		{
			int16_t u[3];
			u[0] = 0;
			u[1] = 0;
			u[2] = (int16_t)0xF6A0;
			MatVec((const Mat4x3 *)(Caster() + 0x40), u, u);
			for (int32_t j = 0; j < 8; j++)
			{
				const int32_t i = FreeSlot(game_addr(POOL_B), game_addr(POOL_B_END), 0x50);
				if (i < 0) break;
				Part *p = PartAt(game_addr(POOL_B), i);
				p->flags = 8;
				p->life = 0;
				p->scale = (int16_t)(CrtRand() % 0x1400 + 0x800);
				p->pos[0] = (int16_t)(n->w10 + u[0]);
				p->pos[1] = (int16_t)(n->w12 + u[1]);
				p->pos[2] = (int16_t)(n->w14 + u[2]);
				p->pos[0] = (int16_t)(p->pos[0] + (int16_t)(CrtRand() % 0x1F4 - 0xFA));
				p->pos[1] = (int16_t)(p->pos[1] + (int16_t)(CrtRand() % 0x1F4 - 0xFA));
				p->pos[2] = (int16_t)(p->pos[2] + (int16_t)(CrtRand() % 0x1F4 - 0xFA));
				int32_t d[3];
				d[0] = (int32_t)n->w10 - p->pos[0];
				d[1] = (int32_t)n->w12 - p->pos[1];
				d[2] = (int32_t)n->w14 - p->pos[2];
				Normalize(d, d);
				const int32_t s = CrtRand() % 0x64 + 0x3C;
				p->v[0] = (int16_t)(mul32(s, d[0]) >> 12);
				p->v[1] = (int16_t)(mul32(s, d[1]) >> 12);
				p->v[2] = (int16_t)(mul32(s, d[2]) >> 12);
			}
		}
		n->c++;
		return n->c >= 3 && count == 0 ? TASK_END : 0;
	}

	static uint32_t __cdecl EmberTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		g_slot28_known = false;
		uint8_t *h = AllocHeader(0xB4);
		*(uint32_t *)h = game_addr(SEQ_EMBER);
		*(int16_t *)(h + 0x24) = 0;
		int32_t count = 0;
		for (uint32_t a = game_addr(POOL_A); a < game_addr(POOL_A_END); a += 0x18)
		{
			Part *p = (Part *)a;
			if (!(p->flags & 1)) continue;
			ShadowSprite(h, p->pos, p->scale, p->life);
			if (UpdateFlags() & 0x201) continue;
			p->life++;
			if (p->life >= 0x10)
			{
				p->flags = 0;
				continue;
			}
			p->pos[0] = (int16_t)(p->pos[0] + p->v[0]);
			p->pos[1] = (int16_t)(p->pos[1] + p->v[1]);
			p->pos[2] = (int16_t)(p->pos[2] + p->v[2]);
			p->v[0] = (int16_t)(p->v[0] - (int16_t)(p->v[0] >> 3));
			p->v[1] = (int16_t)(p->v[1] - (int16_t)(p->v[1] >> 3));
			p->v[2] = (int16_t)(p->v[2] - (int16_t)(p->v[2] >> 4));
			count++;
		}
		FieldFree(0xB4);
		if (UpdateFlags() & 0x201) return 0;
		if (n->c >= 0 && n->c <= 0x16)
		{
			for (int32_t j = 0; j < 2; j++)
			{
				const int32_t i = FreeSlot(game_addr(POOL_A), game_addr(POOL_A_END), 0x28);
				if (i < 0) break;
				Part *p = PartAt(game_addr(POOL_A), i);
				p->flags = 1;
				p->life = 0;
				p->scale = (int16_t)(CrtRand() % 0x1000 + 0x900);
				ff8fx::copy<(8)>(p->pos, &n->w10);
				p->pos[0] = (int16_t)(p->pos[0] + (int16_t)(CrtRand() % 0x7D0 - 0x3E8));
				p->pos[1] = (int16_t)(p->pos[1] + (int16_t)(-0xC8 - CrtRand() % 0x7D));
				p->pos[2] = (int16_t)(p->pos[2] + (int16_t)(-0x12C - CrtRand() % 0x7D0));
				int32_t d[3];
				d[0] = (int32_t)p->pos[0] - n->w10;
				d[1] = (int32_t)p->pos[1] - n->w12;
				d[2] = (int32_t)p->pos[2] - n->w14;
				Normalize(d, d);
				const int32_t s = CrtRand() % 0x78 + 0xB4;
				p->v[0] = (int16_t)(mul32(s, d[0]) >> 12);
				p->v[1] = (int16_t)(mul32(s, d[1]) >> 12);
				p->v[2] = (int16_t)(mul32(s, d[2]) >> 12);
			}
		}
		n->c++;
		return n->c >= 0x10 && count == 0 ? TASK_END : 0;
	}

	static uint32_t __cdecl SparkTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		g_slot28_known = false;
		uint8_t *h = AllocHeader(0xB4);
		*(uint32_t *)h = game_addr(SEQ_SPARK);
		*(int16_t *)(h + 0x24) = 8;
		int32_t count = 0;
		for (uint32_t a = game_addr(POOL_B); a < game_addr(POOL_B_END); a += 0x18)
		{
			Part *p = (Part *)a;
			if (!(p->flags & 1)) continue;
			ShadowSprite(h, p->pos, p->scale, p->life);
			if (UpdateFlags() & 0x201) continue;
			p->life++;
			if (*(const int16_t *)(h + 0x28) < 0)
			{
				p->flags = 0;
				continue;
			}
			p->scale = (int16_t)(p->scale - (int16_t)(p->scale >> 5));
			count++;
		}
		FieldFree(0xB4);
		if (UpdateFlags() & 0x201) return 0;
		if (n->c >= 0 && n->c <= 6)
		{
			for (int32_t j = 0; j < 5; j++)
			{
				const int32_t i = FreeSlot(game_addr(POOL_B), game_addr(POOL_B_END), 0x50);
				if (i < 0) break;
				Part *p = PartAt(game_addr(POOL_B), i);
				p->flags = 1;
				p->life = 0;
				p->scale = (int16_t)(CrtRand() % 0xE00 + 0x800);
				ff8fx::copy<(8)>(p->pos, &n->w10);
				p->pos[0] = (int16_t)(p->pos[0] + (int16_t)(CrtRand() % 0x578 - 0x2BC));
				p->pos[1] = (int16_t)(p->pos[1] + (int16_t)(CrtRand() % 0x578 - 0x2BC));
				p->pos[2] = (int16_t)(p->pos[2] + (int16_t)(CrtRand() % 0x578 - 0x2BC));
				int32_t d[3];
				d[0] = (int32_t)p->pos[0] - n->w10;
				d[1] = (int32_t)p->pos[1] - n->w12;
				d[2] = (int32_t)p->pos[2] - n->w14;
				Normalize(d, d);
				const int32_t s = CrtRand() % 0xBE + 0x96;
				p->v[0] = (int16_t)(mul32(s, d[0]) >> 12);
				p->v[1] = (int16_t)(mul32(s, d[1]) >> 12);
				p->v[2] = (int16_t)(mul32(s, d[2]) >> 12);
			}
		}
		n->c++;
		return n->c >= 0x10 && count == 0 ? TASK_END : 0;
	}

	static void SwirlSprite(uint8_t *h, const Part *p, int16_t life, int32_t sz)
	{
		*(int16_t *)(h + 4) = life;
		int16_t ang[3];
		ang[0] = 0x400;
		ang[1] = 0;
		ang[2] = p->v[2];
		Mat4x3 m;
		BuildRotationMatrixFromAngles(ang, &m);
		int32_t sv[3];
		m.t[0] = p->pos[0];
		m.t[2] = p->pos[2];
		m.t[1] = p->pos[1];
		sv[1] = p->scale;
		sv[0] = p->scale;
		sv[2] = sz;
		Scale3DMatrix(&m, sv);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
	}

	static uint32_t SwirlBody(Node24 *n, uint32_t residue)
	{
		uint8_t *h = AllocHeader(0xB4);
		*(uint32_t *)h = game_addr(SEQ_SWIRL);
		*(int16_t *)(h + 0x24) = 8;
		for (uint32_t a = game_addr(POOL_B); a < game_addr(POOL_B_END); a += 0x18)
		{
			Part *p = (Part *)a;
			if (!(p->flags & 2)) continue;
			SwirlSprite(h, p, p->life, (int32_t)residue);
			if (UpdateFlags() & 0x201) continue;
			p->life = (int16_t)(p->life - 2);
			if (p->life < 0) p->flags = 0;
		}
		FieldFree(0xB4);
		if (UpdateFlags() & 0x201) return 0;
		if (n->c >= 0 && n->c <= 0xC)
		{
			const int32_t k = n->c / 7 + 2;
			for (int32_t j = 0; j < k; j++)
			{
				const int32_t i = FreeSlot(game_addr(POOL_B), game_addr(POOL_B_END), 0x50);
				if (i < 0) break;
				Part *p = PartAt(game_addr(POOL_B), i);
				p->flags = 2;
				p->life = 0x13;
				p->scale = (int16_t)(CrtRand() % 0x1400 + 0x900);
				p->pos[0] = n->w10;
				p->pos[1] = 0;
				p->pos[2] = (int16_t)(n->w14 + 0x898);
				int32_t spread = (CrtRand() % 0xBE) * shl32(n->c, 3);
				spread += (CrtRand() % 0x46 + 0xA) * 30;
				p->pos[0] = (int16_t)(p->pos[0] + (int16_t)(CrtRand() % spread - (spread >> 1)));
				p->pos[2] = (int16_t)(p->pos[2] + (int16_t)(CrtRand() % (spread >> 2) - (spread >> 3)));
				p->v[2] = (int16_t)(CrtRand() % 0x800);
			}
		}
		n->w14 = (int16_t)(n->w14 + (int16_t)0xFDBC);
		n->c++;
		return SwirlFlag() != 0 ? 0 : TASK_END;
	}

	static uint32_t __cdecl SwirlTask(TaskNode *tn)
	{
		return SwirlBody((Node24 *)tn, g_slot28_known ? g_slot28 : 0x1000);
	}

	static void ShellMatrix(const Node24 *n, int16_t yaw, int16_t pitch, int16_t s)
	{
		int16_t ang[3];
		ang[0] = pitch;
		ang[1] = yaw;
		ang[2] = 0;
		Mat4x3 m;
		ComposeZYXRotationMatrix(ang, &m);
		int32_t sv[3];
		m.t[0] = n->w10;
		m.t[1] = n->w12;
		sv[2] = s;
		sv[1] = s;
		sv[0] = s;
		m.t[2] = n->w14;
		Scale3DMatrix(&m, sv);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
	}

	static void ShellMorph(int16_t *dst, int32_t angle)
	{
		const int32_t s = ComputeSin(angle);
		const int16_t *from = (const int16_t *)game_addr(SHELL_FROM), *to = (const int16_t *)game_addr(SHELL_TO);
		for (int32_t i = 0; i < 168; i++, from += 4, to += 4, dst += 4)
		{
			dst[0] = (int16_t)((mul32((int32_t)to[0] - from[0], s) >> 12) + from[0]);
			dst[1] = (int16_t)((mul32((int32_t)to[1] - from[1], s) >> 12) + from[1]);
			dst[2] = (int16_t)((mul32((int32_t)to[2] - from[2], s) >> 12) + from[2]);
		}
	}

	static void ShellDraw(const Node24 *n, int16_t s, int32_t c, int32_t fade, int32_t angle, int16_t *vtx)
	{
		ShellMatrix(n, n->w18, 0x1F4, s);
		uint8_t *h = AllocHeader(0x58);
		*(uint32_t *)h = game_addr(MODEL_SHELL);
		h[0xA] = 0;
		h[9] = 0;
		h[8] = 0;
		*(uint32_t *)(h + 0x1C) = 0x33;
		if (c < 8 || c >= 0xC)
		{
			*(int32_t *)(h + 0xC) = fade;
			*(uint32_t *)(h + 0x1C) = 0xF3;
		}
		if (c >= 8)
		{
			*(uint32_t *)(h + 0x1C) |= 0x2000;
			*(uint32_t *)(h + 4) = (uint32_t)vtx;
			ShellMorph(vtx, angle);
		}
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static int32_t ShellFade(int32_t c) { return c < 8 ? shl32(8 - c, 9) : mul32(c - 0xC, 682); }

	static uint32_t __cdecl ShellTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		const int32_t c = n->c;
		ShellDraw(n, n->w1C, c, ShellFade(c), mul32(c - 8, 56), (int16_t *)game_addr(MORPH_VTX));
		g_slot28 = (uint32_t)(int32_t)n->w1C;
		g_slot28_known = true;
		if (DrawOnly()) return 0;
		if (n->c < 8) n->w1C = (int16_t)(n->w1C + (int16_t)(n->w1E >> 3));
		n->c++;
		return n->c >= 0x12 ? TASK_END : 0;
	}

	static void DomeDraw(const Node24 *n, int16_t s, int32_t c, int32_t fade)
	{
		ShellMatrix(n, n->w18, 0x29, s);
		uint8_t *h = AllocHeader(0x58);
		*(uint32_t *)h = game_addr(MODEL_SHELL);
		*(uint32_t *)(h + 4) = game_addr(SHELL_TO);
		h[0xA] = 0;
		h[9] = 0;
		h[8] = 0;
		*(uint32_t *)(h + 0x1C) = 0x2033;
		if (c >= 0xE)
		{
			*(uint32_t *)(h + 0x1C) = 0x20F3;
			*(int32_t *)(h + 0xC) = fade;
		}
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static uint32_t __cdecl DomeTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		if (n->e > 0)
		{
			if (DrawOnly()) return 0;
			n->e--;
			return 0;
		}
		DomeDraw(n, n->w1C, n->c, shl32(n->c - 0xE, 9));
		g_slot28 = (uint32_t)(int32_t)n->w1C;
		g_slot28_known = true;
		if (DrawOnly()) return 0;
		const int16_t rate = n->w1E;
		n->w1C = (int16_t)(n->w1C + rate);
		n->w1E = (int16_t)(rate + (int16_t)(rate >> 3));
		n->c++;
		return n->c >= 0x16 ? TASK_END : 0;
	}

	static inline uint32_t &P32(uint8_t *p, int o) { return *(uint32_t *)(p + o); }
	static inline uint16_t U16(const uint8_t *p, int o) { return *(const uint16_t *)(p + o); }

	static uint32_t TexWindow(uint8_t ox, uint8_t oy, uint16_t w, uint16_t hgt)
	{
		uint32_t v = ((uint32_t)oy & 0xF8) | 0xFFFE2000u;
		v <<= 5;
		v |= (uint32_t)ox & 0xF8;
		v <<= 5;
		v |= ~((uint32_t)hgt - 1) & 0xF8;
		v <<= 2;
		v |= (uint32_t)((int32_t)~((uint32_t)w - 1) >> 3) & 0x1F;
		return v;
	}

	static bool ClipOut(const uint8_t *xy0, const uint8_t *xy1, const uint8_t *xy2)
	{
		uint32_t f = U16(xy0, 0) > 0xA00 ? 1 : 0;
		if (U16(xy1, 0) > 0xA00) f |= 2;
		if (U16(xy2, 0) > 0xA00) f |= 4;
		if (U16(xy0, 2) > 0x6C0) f |= 0x10;
		if (U16(xy1, 2) > 0x6C0) f |= 0x20;
		if (U16(xy2, 2) > 0x6C0) f |= 0x40;
		return (f & 7) == 7 || (f & 0x70) == 0x70;
	}

	static uint32_t CircleFaces(MeshHeader *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		uint8_t *faces = (uint8_t *)h->faces;
		const int32_t texh = h->tpage[3];
		int32_t count = *(const int32_t *)faces;
		uint8_t *face = faces + 4;
		const uint8_t *verts = (const uint8_t *)h->verts;
		h->faces = (uint32_t)face;
		uint8_t *pk = (uint8_t *)cursor;
		if (count <= 0) return cursor;
		for (; count != 0; count--, face += 0x1C)
		{
			GteLoadV012(verts + 4 * U16(face, 4), verts + 4 * U16(face, 6), verts + 4 * U16(face, 8));
			GteRTPT();
			const uint32_t c0 = P32(face, 0) | 0x2000000;
			P32(face, 0) = c0;
			P32(pk, 8) = c0;
			P32(pk, 0) = 0xB000000;
			P32(pk, 0x10) = P32(face, 0xC);
			P32(pk, 0x1C) = P32(face, 0x10);
			P32(pk, 0x28) = P32(face, 8) >> 16;
			GteReadFLAG(&h->flag);
			if (h->flag & 0x60000) continue;
			GteNCLIP();
			GteReadMAC0(&h->mac0);
			if (h->mac0 < 0 && !(h->mode & 0x20)) continue;
			GteReadSXY012Split(pk + 0xC, pk + 0x18, pk + 0x24);
			GteAVSZ3();
			if (ClipOut(pk + 0xC, pk + 0x18, pk + 0x24)) continue;
			const int32_t d = h->vmod;
			if (d != 0)
			{
				int32_t a = pk[0x11] + d, b = pk[0x1D] + d, e = pk[0x29] + d;
				h->uv[0] = a;
				h->uv[1] = b;
				h->uv[2] = e;
				if (a >= 0x100 || b >= 0x100 || e >= 0x100)
				{
					h->uv[0] = a - texh;
					h->uv[1] = b - texh;
					h->uv[2] = e - texh;
				}
				else if (a < 0 || b < 0 || e < 0)
				{
					h->uv[0] = a + texh;
					h->uv[1] = b + texh;
					h->uv[2] = e + texh;
				}
				pk[0x11] = (uint8_t)h->uv[0];
				pk[0x1D] = (uint8_t)h->uv[1];
				pk[0x29] = (uint8_t)h->uv[2];
			}
			GteReadOTZ32(&h->otz);
			if (h->mode & 0x80)
			{
				GteLoadRGB012(face + 0x14, face + 0x18, pk + 8);
				GteSetIR0(h->fade);
				GteDPCT();
				GteStoreRGB012(pk + 0x14, pk + 0x20, pk + 8);
			}
			else
			{
				P32(pk, 0x14) = P32(face, 0x14);
				P32(pk, 0x20) = P32(face, 0x18);
			}
			const uint8_t *hb = (const uint8_t *)h;
			P32(pk, 4) = TexWindow(hb[0x18], hb[0x1A], U16(hb, 0x1C), U16(hb, 0x1E));
			P32(pk, 0x2C) = TexWindow(hb[0x6C], hb[0x6E], U16(hb, 0x70), U16(hb, 0x72));
			InsertPrimAutoDepth(ot + 4 * (uint32_t)(h->otz >> shift), pk);
			pk += 0x30;
		}
		h->faces = (uint32_t)face;
		return (uint32_t)pk;
	}

	static uint32_t CircleRender(MeshHeader *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		if (!(h->mode & 0x2000)) h->verts = h->model + 8;
		uint8_t *hb = (uint8_t *)h;
		h->faces = h->model + *(const uint32_t *)h->model;
		*(int16_t *)(hb + 0x6E) = 0;
		*(int16_t *)(hb + 0x6C) = 0;
		*(int16_t *)(hb + 0x72) = 0xFF;
		*(int16_t *)(hb + 0x70) = 0xFF;
		h->vmod = h->vscroll % (int32_t)h->tpage[3];
		GteSetFarColor(h->far[0], h->far[1], h->far[2]);
		h->faces += 0x18;
		return CircleFaces(h, ot, shift, cursor);
	}

	static void CircleMorph(int16_t *dst, int32_t k)
	{
		const int16_t *d = (const int16_t *)game_addr(MORPH_DELTA), *from = (const int16_t *)game_addr(CIRCLE_FROM);
		for (int32_t i = 0; i < 0x5D; i++, d += 4, from += 4, dst += 4)
		{
			dst[0] = (int16_t)(d[0] * k + from[0]);
			dst[1] = (int16_t)(d[1] * k + from[1]);
			dst[2] = (int16_t)(d[2] * k + from[2]);
		}
	}

	static void CircleDraw(const Node24 *n, int16_t s, int32_t c, int32_t vscroll, int32_t fade, const int16_t *vtx)
	{
		int16_t ang[3];
		ang[0] = 0;
		ang[1] = 0x800;
		ang[2] = 0;
		Mat4x3 m;
		ComposeZYXRotationMatrix(ang, &m);
		int32_t sv[3];
		m.t[1] = n->w12;
		m.t[0] = n->w10;
		sv[2] = s;
		sv[1] = s;
		sv[0] = s;
		m.t[2] = n->w14;
		Scale3DMatrix(&m, sv);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		MeshHeader *h = (MeshHeader *)AllocHeader(0xCC);
		h->tpage[0] = 0x40;
		h->tpage[2] = 0x40;
		h->model = game_addr(MODEL_CIRCLE);
		h->mode = 0x30;
		h->tpage[1] = 0;
		h->tpage[3] = 0x80;
		h->vscroll = vscroll;
		if (c >= 0x20)
		{
			h->far[2] = 0;
			h->far[1] = 0;
			h->far[0] = 0;
			h->fade = fade;
			h->mode = 0xF0;
		}
		if (c >= 0x10)
		{
			if (!vtx)
			{
				CircleMorph((int16_t *)game_addr(MORPH_VTX), (int16_t)(c - 0x10));
				vtx = (const int16_t *)game_addr(MORPH_VTX);
			}
			h->verts = (uint32_t)vtx;
			h->mode |= 0x2000;
		}
		PacketCursor() = CircleRender(h, RenderOT(), 2, PacketCursor());
		FieldFree(0xCC);
	}

	static uint32_t __cdecl CircleTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		g_slot28_known = false;
		const int32_t c = n->c;
		CircleDraw(n, n->w1C, c, mul32(c, 10), shl32(c - 0x20, 9), nullptr);
		if (DrawOnly()) return 0;
		const int16_t rate = n->w1E;
		n->w1C = (int16_t)(n->w1C + rate);
		n->c++;
		n->w1E = (int16_t)(rate - rate / 4);
		return n->c >= 0x28 ? TASK_END : 0;
	}

	static void PillarFace(MeshHeader *h, const uint8_t *verts, const uint8_t *face)
	{
		ff8fx::copy<(8)>(h->v0, verts + 4 * U16(face, 4));
		const int16_t cz = h->off[2];
		const int16_t ax = h->off[0];
		h->v0[0] = (int16_t)(h->v0[0] - ax);
		h->v0[2] = (int16_t)(h->v0[2] - cz);
		ff8fx::copy<(8)>(h->v1, verts + 4 * U16(face, 6));
		h->v1[0] = (int16_t)(h->v1[0] - ax);
		h->v1[2] = (int16_t)(h->v1[2] - cz);
		ff8fx::copy<(8)>(h->v2, verts + 4 * U16(face, 8));
		h->v2[0] = (int16_t)(h->v2[0] - ax);
		h->face.t[0] = ax;
		h->v2[2] = (int16_t)(h->v2[2] - cz);
		h->face.t[2] = cz;
		h->face.t[1] = h->off[1];
		BuildRotationMatrixFromAngles(h->ang, &h->face);
		ComposeAffineTransform(&h->cam, &h->face, &h->face);
		GteSetRotMatrixC(&h->face);
		GteSetTransVectorC(&h->face);
		GteLoadV012(h->v0, h->v1, h->v2);
		GteRTPT();
	}

	static uint32_t PillarFlat(MeshHeader *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		const uint8_t *verts = (const uint8_t *)h->verts;
		uint8_t *face = (uint8_t *)h->faces;
		int32_t count = *(const int32_t *)face;
		face += 4;
		h->faces = (uint32_t)face;
		if (count <= 0) return cursor;
		uint8_t *pk = (uint8_t *)cursor;
		for (; count != 0; count--, face += 0x14)
		{
			PillarFace(h, verts, face);
			uint32_t c0 = P32(face, 0);
			P32(pk, 0) = 0x7000000;
			P32(pk, 4) = c0;
			if (h->mode & 1) P32(pk, 4) = c0 | 0x2000000;
			P32(pk, 0xC) = P32(face, 0xC);
			P32(pk, 0x14) = P32(face, 0x10);
			P32(pk, 0x1C) = P32(face, 8) >> 16;
			GteReadFLAG(&h->flag);
			if (h->flag & 0x60000) continue;
			GteNCLIP();
			GteReadMAC0(&h->mac0);
			if (h->mac0 < 0) continue;
			GteReadSXY012Split(pk + 8, pk + 0x10, pk + 0x18);
			GteAVSZ3();
			if (ClipOut(pk + 8, pk + 0x10, pk + 0x18)) continue;
			GteReadOTZ32(&h->otz);
			if (h->mode & 0x40)
			{
				GteLoadRGBC(pk + 4);
				GteSetIR0(h->fade);
				GteDPCS();
				GteStoreRGB2(pk + 4);
			}
			InsertPrimAutoDepth(ot + 4 * (uint32_t)(h->otz >> shift), pk);
			pk += 0x20;
		}
		h->faces = (uint32_t)face;
		return (uint32_t)pk;
	}

	static uint32_t PillarGouraud(MeshHeader *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		uint8_t *pk = (uint8_t *)cursor;
		uint8_t *face = (uint8_t *)h->faces;
		int32_t count = *(const int32_t *)face;
		face += 4;
		const uint8_t *verts = (const uint8_t *)h->verts;
		h->faces = (uint32_t)face;
		if (count <= 0) return cursor;
		for (; count != 0; count--, face += 0x1C)
		{
			PillarFace(h, verts, face);
			uint32_t c0 = P32(face, 0);
			P32(pk, 0) = 0x9000000;
			P32(pk, 4) = c0;
			if (h->mode & 2) P32(pk, 4) = c0 | 0x2000000;
			P32(pk, 0x18) = P32(face, 0x10);
			P32(pk, 0xC) = P32(face, 0xC);
			P32(pk, 0x24) = P32(face, 8) >> 16;
			GteReadFLAG(&h->flag);
			if (h->flag & 0x60000) continue;
			GteNCLIP();
			GteReadMAC0(&h->mac0);
			if (h->mac0 < 0) continue;
			GteReadSXY012Split(pk + 8, pk + 0x14, pk + 0x20);
			GteAVSZ3();
			if (ClipOut(pk + 8, pk + 0x14, pk + 0x20)) continue;
			GteReadOTZ32(&h->otz);
			if (h->mode & 0x80)
			{
				GteLoadRGB012(face + 0x14, face + 0x18, pk + 4);
				GteSetIR0(h->fade);
				GteDPCT();
				GteStoreRGB012(pk + 0x10, pk + 0x1C, pk + 4);
			}
			else
			{
				P32(pk, 0x10) = P32(face, 0x14);
				P32(pk, 0x1C) = P32(face, 0x18);
			}
			InsertPrimAutoDepth(ot + 4 * (uint32_t)(h->otz >> shift), pk);
			pk += 0x28;
		}
		h->faces = (uint32_t)face;
		return (uint32_t)pk;
	}

	static uint32_t PillarRender(MeshHeader *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		h->verts = h->model + 8;
		h->faces = h->model + *(const uint32_t *)h->model;
		GteSetFarColor(h->far[0], h->far[1], h->far[2]);
		h->faces += 8;
		if (*(const uint32_t *)h->faces != 0) cursor = PillarFlat(h, ot, shift, cursor);
		else h->faces += 4;
		h->faces += 0xC;
		if (*(const uint32_t *)h->faces != 0) return PillarGouraud(h, ot, shift, cursor);
		h->faces += 4;
		return cursor;
	}

	static void PillarDraw(const Node24 *n, int16_t ang2)
	{
		int16_t ang[3];
		ang[0] = 0;
		ang[1] = 0;
		ang[2] = 0;
		Mat4x3 m;
		BuildRotationMatrixFromAngles(ang, &m);
		int32_t sv[3];
		m.t[1] = n->w12;
		m.t[0] = n->w10;
		sv[2] = 0x1000;
		sv[1] = 0x1000;
		sv[0] = 0x1000;
		m.t[2] = n->w14;
		Scale3DMatrix(&m, sv);
		MeshHeader *h = (MeshHeader *)AllocHeader(0xCC);
		const int32_t i = n->e;
		h->mode = 0;
		h->off[1] = 0;
		h->model = var<uint32_t>(game_addr(PILLAR_MODEL) + 4 * i);
		h->off[0] = var<int16_t>(game_addr(PILLAR_OFFX) + 4 * i);
		h->ang[0] = n->w1C;
		h->off[2] = var<int16_t>(game_addr(PILLAR_OFFZ) + 4 * i);
		h->ang[1] = n->w20;
		h->ang[2] = ang2;
		ComposeAffineTransform(&Camera(), &m, &h->cam);
		PacketCursor() = PillarRender(h, RenderOT(), 2, PacketCursor());
		FieldFree(0xCC);
	}

	static uint32_t __cdecl PillarTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		g_slot28_known = false;
		if (n->w1A > 0)
		{
			if (DrawOnly()) return 0;
			n->w1A--;
			return 0;
		}
		PillarDraw(n, (int16_t)(n->c << 1));
		if (DrawOnly()) return 0;
		const int16_t vy = n->w16;
		n->w12 = (int16_t)(n->w12 + vy);
		n->w16 = (int16_t)(vy - vy / 4);
		const int16_t vz = n->w18;
		n->w14 = (int16_t)(n->w14 + vz);
		if (vz < (int16_t)0xFF6A) n->w18 = (int16_t)(vz - (int16_t)(vz >> 5));
		const int16_t rx = n->w1E;
		n->w1C = (int16_t)(n->w1C + rx);
		const int16_t ry = n->w22;
		n->w20 = (int16_t)(n->w20 + ry);
		n->c++;
		n->w1E = (int16_t)(rx + (int16_t)(rx >> 4));
		n->w22 = (int16_t)(ry + (int16_t)(ry >> 4));
		return n->c >= 0x1E ? TASK_END : 0;
	}

	static void RingDraw(const Node24 *n, int32_t c, int32_t fade)
	{
		int16_t ang[3];
		ang[0] = 0;
		ang[1] = 0;
		ang[2] = 0;
		Mat4x3 m;
		BuildRotationMatrixFromAngles(ang, &m);
		int32_t sv[3];
		m.t[0] = n->w10;
		m.t[2] = n->w14;
		m.t[1] = 0x29;
		sv[2] = 0x1000;
		sv[0] = 0x1000;
		sv[1] = 0;
		Scale3DMatrix(&m, sv);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		uint8_t *h = AllocHeader(0x58);
		h[0xA] = 0;
		*(uint32_t *)h = var<uint32_t>(game_addr(PILLAR_MODEL) + 4 * n->e);
		h[9] = 0;
		h[8] = 0;
		*(int32_t *)(h + 0xC) = 0xC00;
		*(uint32_t *)(h + 0x1C) = 0xC0;
		if (c >= 0xC) *(int32_t *)(h + 0xC) = fade;
		PacketCursor() = RenderPrimModel(h, var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x4068, 0xD, PacketCursor());
		FieldFree(0x58);
	}

	static uint32_t __cdecl RingTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		g_slot28_known = false;
		if (n->w1A > 0)
		{
			if (DrawOnly()) return 0;
			n->w1A--;
			return 0;
		}
		RingDraw(n, n->c, mul32(0x14 - n->c, 384));
		if (DrawOnly()) return 0;
		n->c++;
		return n->c > 0x14 ? TASK_END : 0;
	}

	static inline int32_t Div39(int32_t x)
	{
		int32_t q = (int32_t)(((int64_t)x * (int32_t)0xD20D20D3) >> 32) + x;
		q >>= 5;
		return q + (int32_t)((uint32_t)q >> 31);
	}
	static inline int32_t Div18(int32_t x)
	{
		const int32_t q = (int32_t)(((int64_t)x * 0x38E38E39) >> 32) >> 2;
		return q + (int32_t)((uint32_t)q >> 31);
	}

	static void CaptureStage(int16_t *pos)
	{
		int16_t rect[4];
		rect[1] = 0x100;
		rect[3] = 0x100;
		rect[0] = 0x200;
		rect[2] = 0x80;
		int16_t ang[3];
		ang[0] = 0x400;
		ang[1] = 0;
		ang[2] = 0;
		Mat4x3 m;
		m.t[0] = -6 - Div39(pos[0]);
		m.t[1] = Div18(pos[2]) + 0x15;
		m.t[2] = var<int16_t>(game_addr(game::BATTLE_PROJECTION_DISTANCE));
		BuildRotationMatrixFromAngles(ang, &m);
		int32_t sv[3];
		sv[1] = 0;
		sv[0] = 0x1000000 / (0x130E000 / rect[2]);
		sv[2] = 0x1000000 / (0x1143000 / rect[3]);
		Scale3DMatrix(&m, sv);
		SetDrawEnv(game_addr(PRIM_ENV), game_addr(game::g_Battle_DrawEnv) + 92 * ((uint32_t)var<uint8_t>(game_addr(game::g_Battle_DisplayParity)) ^ 1));
		InsertPrimAutoDepth(RenderOT(), (void *)game_addr(PRIM_ENV));
		int32_t ox, oy;
		GteReadScreenOffset(&ox, &oy);
		GteSetScreenOffset(rect[2] / 2, rect[3] / 2);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		uint8_t *h = (uint8_t *)FieldAlloc(0x2C);
		*(uint32_t *)(h + 0x1C) = var<uint32_t>(game_addr(game::unk_1D989E4));
		*(int32_t *)(h + 0x10) = -1;
		*(int32_t *)(h + 0x20) = -1;
		*(int16_t *)(h + 0x14) = 0;
		*(int16_t *)(h + 0x16) = 0;
		*(int16_t *)(h + 0x18) = 0x140;
		*(int16_t *)(h + 0x1A) = 0xD8;
		*(int16_t *)(h + 0x24) = var<int16_t>(game_addr(game::unk_1D989BE));
		*(uint32_t *)(h + 4) = var<uint32_t>(game_addr(game::g_BattleVertexWorkspace));
		if (var<uint8_t>(game_addr(game::unk_1D989BD)) & 1)
		{
			BuildBoneMatricesFromPose((void *)game_addr(game::unk_1D989D0));
			ComputeBonesWorldMatricesM(game_addr(game::unk_1D989D0), &m);
			PacketCursor() = RenderGeometry(game_addr(game::unk_1D989C0), h, RenderOT(), 0x10, PacketCursor());
		}
		FieldFree(0x2C);
		GteSetScreenOffset(ox, oy);
		InsertPrimAltViewport(RenderOT(), game_addr(ALT_VIEWPORT));
		SetDrawArea(game_addr(PRIM_AREA), rect);
		InsertPrimAutoDepth(RenderOT(), (void *)game_addr(PRIM_AREA));
		var<int16_t>(game_addr(OFFSET_WORDS)) = rect[0];
		var<int16_t>(game_addr(OFFSET_WORDS) + 2) = rect[1];
		SetDrawOffset(game_addr(PRIM_OFFSET), (const void *)game_addr(OFFSET_WORDS));
		InsertPrimAutoDepth(RenderOT(), (void *)game_addr(PRIM_OFFSET));
	}

	static uint32_t __cdecl RestoreTask(TaskNode *tn)
	{
		RootNode *r = (RootNode *)tn;
		if (r->counter == 2)
		{
			TextureRestoreTask((void *)(TexBase() + 0x10000), &r->done);
			r->done = 0;
		}
		r->counter++;
		return r->done != 0 ? TASK_END : 0;
	}

	static uint32_t __cdecl MasterTask(TaskNode *tn)
	{
		RootNode *r = (RootNode *)tn;
		if (r->counter & 1) PacketCursor() = TexBase() + 0x8000;
		else PacketCursor() = TexBase();
		const int alive = ExecuteTaskQueue(QMain());
		r->counter++;
		return alive ? 0 : TASK_END;
	}
}

	void register_mag162_fated_circle()
	{
		register_port(circle162::ORIG_Master, (void *)circle162::MasterTask, "162 Master", 162);
		register_port(circle162::ORIG_Restore, (void *)circle162::RestoreTask, "162 Restore", 162);
		register_port(circle162::ORIG_Director, (void *)circle162::DirectorTask, "162 Director", 162);
		register_port(circle162::ORIG_Trail, (void *)circle162::TrailTask, "162 Trail", 162);
		register_port(circle162::ORIG_Swipe, (void *)circle162::SwipeTask, "162 Swipe", 162);
		register_port(circle162::ORIG_Dust, (void *)circle162::DustTask, "162 Dust", 162);
		register_port(circle162::ORIG_Shock, (void *)circle162::ShockTask, "162 Shock", 162);
		register_port(circle162::ORIG_Glow, (void *)circle162::GlowTask, "162 Glow", 162);
		register_port(circle162::ORIG_Shard, (void *)circle162::ShardTask, "162 Shard", 162);
		register_port(circle162::ORIG_Orbit, (void *)circle162::OrbitTask, "162 Orbit", 162);
		register_port(circle162::ORIG_Burst, (void *)circle162::BurstTask, "162 Burst", 162);
		register_port(circle162::ORIG_Rain, (void *)circle162::RainTask, "162 Rain", 162);
		register_port(circle162::ORIG_Shell, (void *)circle162::ShellTask, "162 Shell", 162);
		register_port(circle162::ORIG_Dome, (void *)circle162::DomeTask, "162 Dome", 162);
		register_port(circle162::ORIG_Circle, (void *)circle162::CircleTask, "162 Circle", 162);
		register_port(circle162::ORIG_Pillar, (void *)circle162::PillarTask, "162 Pillar", 162);
		register_port(circle162::ORIG_Ring, (void *)circle162::RingTask, "162 Ring", 162);
		register_port(circle162::ORIG_Ember, (void *)circle162::EmberTask, "162 Ember", 162);
		register_port(circle162::ORIG_Spark, (void *)circle162::SparkTask, "162 Spark", 162);
		register_port(circle162::ORIG_Swirl, (void *)circle162::SwirlTask, "162 Swirl", 162);
	}
}

