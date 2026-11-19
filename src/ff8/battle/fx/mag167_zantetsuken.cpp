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

// Effect 167: Zantetsuken of the monster Odin

#include "fx_port.h"
#include <stddef.h>

namespace ff8fx
{
namespace z167
{
	using namespace eng;

	namespace x
	{
		inline int32_t CrtRand() { return fn<int32_t (__cdecl *)()>(game_addr(game::_rand))(); }
		inline void ComposeZYXRotationMatrix(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::ComposeZYXRotationMatrix))(angles, out); }
		inline void Scale3DMatrix(Mat4x3 *m, const int32_t *v) { fn<void (__cdecl *)(Mat4x3 *, const int32_t *)>(game_addr(game::scale3DMatrix))(m, v); }
		inline void GteMatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
		inline void NormalizeVector16(const int16_t *in, int16_t *out) { fn<void (__cdecl *)(const int16_t *, int16_t *)>(game_addr(game::VectorNormalS16))(in, out); }
		inline uint32_t MatrixMultiplyVector(const Mat4x3 *m, const int16_t *in, int16_t *out) { return fn<uint32_t (__cdecl *)(const Mat4x3 *, const int16_t *, int16_t *)>(game_addr(game::matrixMultiplyVector))(m, in, out); }
		inline void TransformVectorBy3x3Matrix(const Mat4x3 *m, const int32_t *in, int32_t *out) { fn<void (__cdecl *)(const Mat4x3 *, const int32_t *, int32_t *)>(game_addr(game::TransformVectorBy3x3Matrix))(m, in, out); }
		inline void TransformCoordinateToCameraSpace(const int16_t *in, void *out, void *flag) { fn<void (__cdecl *)(const int16_t *, void *, void *)>(game_addr(game::TransformCoordinateToCameraSpace))(in, out, flag); }
		inline int32_t GetEffectSpawnPosition(void *entity, int32_t bone, int32_t frac, int16_t *out) { return fn<int32_t (__cdecl *)(void *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, frac, out); }
		inline void SplineSetup(int32_t n, uint32_t points, void *work) { fn<void (__cdecl *)(int32_t, uint32_t, void *)>(game_addr(game::Effect_SplineSetup))(n, points, work); }
		inline void SplineEval(int32_t n, void *work, uint32_t out, int32_t t) { fn<void (__cdecl *)(int32_t, void *, uint32_t, int32_t)>(game_addr(game::Effect_SplineEval))(n, work, out, t); }
		inline void SetProjection(int32_t h) { fn<void (__cdecl *)(int32_t)>(game_addr(game::Battle_SetProjectionH))(h); }
		inline uint32_t DrawShadow(void *entity, uint32_t ot, int32_t shift, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::BattleEntity_DrawShadow))(entity, ot, shift, cursor); }
		inline void BdPlaySE(uint32_t se, int32_t a, int32_t b) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t)>(game_addr(game::BdPlaySE))(se, a, b); }
		inline void ScreenFade(int32_t a, int32_t b, int32_t c, int32_t d) { fn<void (__cdecl *)(int32_t, int32_t, int32_t, int32_t)>(game_addr(game::Effect_SpawnScreenFadeTask))(a, b, c, d); }
		inline void ApplyActionResultToTargets(uint32_t list, uint32_t count) { fn<void (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::ApplyActionResultToTargets))(list, count); }
		inline void GteSetLightMatrix(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_SetLightMatrix))(m); }
		inline void GteSetBackColorFromTrans(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_SetBackColorFromTrans))(m); }
		inline void GteMVMVA_LightV0Bk() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_LightV0_Bk))(); }
		inline void GteSetRotScale(int32_t s) { fn<void (__cdecl *)(int32_t)>(game_addr(game::GTE_SetRotScale))(s); }
		inline void GteTransFromMAC() { fn<void (__cdecl *)()>(game_addr(game::GTE_TransFromMAC))(); }
		inline void GteLoadV012(const void *a, const void *b, const void *c) { fn<void (__cdecl *)(const void *, const void *, const void *)>(game_addr(game::GTE_LoadV012))(a, b, c); }
		inline void GteRTPT() { fn<void (__cdecl *)()>(game_addr(game::GTE_RTPT))(); }
		inline void GteReadFLAG(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadFLAG))(dst); }
		inline void GteReadSXY012Split(void *a, void *b, void *c) { fn<void (__cdecl *)(void *, void *, void *)>(game_addr(game::GTE_ReadSXY012_Split))(a, b, c); }
		inline void GteAVSZ4() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ4))(); }
		inline void GteReadOTZ32(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadOTZ))(dst); }
		inline void GteLoadRGBC(const void *c) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadRGBC))(c); }
		inline void GteDPCS() { fn<void (__cdecl *)()>(game_addr(game::GTE_DPCS))(); }
		inline void GteStoreRGB2(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreRGB2))(dst); }
		inline void GteSetFarColor(int32_t r, int32_t g, int32_t b) { fn<void (__cdecl *)(int32_t, int32_t, int32_t)>(game_addr(game::GTE_SetFarColor))(r, g, b); }
		inline void GteSetFarColor2(int32_t r, int32_t g, int32_t b) { fn<void (__cdecl *)(int32_t, int32_t, int32_t)>(game_addr(game::GTE_SetFarColorRGB))(r, g, b); }
		inline uint32_t SliceRender(void *geom, void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, void *, uint32_t, int32_t, uint32_t)>(game_addr(game::MAG_167_sub_5E4030))(geom, h, ot, mode, cursor); }
	}
	using namespace x;

	inline bool Paused() { return (var<uint32_t>(game_addr(game::battle_to_update_flags_dword_1D96A9C)) & 0x201) != 0; }
	inline TaskQueue &SubQueue() { return var<TaskQueue>(game_addr(game::unk_23BE130)); }
	inline uint8_t *CastCtx() { return var<uint8_t *>(game_addr(game::unk_23BEF50)); }
	inline uint32_t CasterSlot() { return var<uint32_t>(game_addr(game::unk_23BEF54)); }
	inline uint32_t &Cursor() { return var<uint32_t>(game_addr(game::unk_23BF030)); }
	inline uint32_t &EngineCursor() { return var<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)); }
	inline uint32_t OT() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44; }
	inline uint8_t *Actions() { return *(uint8_t **)(CastCtx() + 4); }
	inline uint8_t *Entity(uint32_t slot) { return (uint8_t *)(game_addr(game::BattleEntitySlotData) + 156 * slot); }
	inline uint8_t *Caster() { return Entity(CasterSlot()); }
	static constexpr game::Id HIST_A = game::unk_23BD350;
	static constexpr game::Id HIST_B = game::unk_23BD3A8;
	static constexpr game::Id SPARK_HIST = game::unk_23BD408;
	static constexpr game::Id POOL = game::unk_23BD5B8;
	static constexpr game::Id SPLINE_A = game::unk_23BDF18;
	static constexpr game::Id SPLINE_B = game::unk_23BE018;
	static constexpr game::Id BLADE_P0 = game::unk_23BE120;
	static constexpr game::Id BLADE_P1 = game::unk_23BE128;
	static constexpr game::Id TRAIL_RECS = game::unk_23BEF60;

	static constexpr game::Id ORIG_Master = game::au_re_BdlinkTask_20;
	static constexpr game::Id ORIG_TimelineTask = game::MAG_167_sub_5E2AD0;
	static constexpr game::Id ORIG_SparkTask = game::MAG_167_sub_5E2F30;
	static constexpr game::Id ORIG_TrailTask = game::MAG_167_sub_5E33E0;
	static constexpr game::Id ORIG_PrimATask = game::MAG_167_sub_5E38D0;
	static constexpr game::Id ORIG_PrimBTask = game::MAG_167_sub_5E3A00;
	static constexpr game::Id ORIG_SliceTask = game::MAG_167_sub_5E3C20;

#pragma pack(push, 1)
	struct Node24
	{
		TaskNode hdr;
		int16_t c;
		int16_t e;
		int16_t p10, p12, p14, p16;
		int16_t a18;
		int16_t w1A;
		int16_t s1C, s1E;
		int16_t s20, s22;
	};
	struct Rec
	{
		uint32_t alive;
		int16_t age;
		int16_t size;
		int16_t x, y, z;
		int16_t w0E;
		int16_t vx, vy, vz;
		int16_t w16;
	};
#pragma pack(pop)
	static_assert(sizeof(Node24) == 0x24, "sub-queue node is 0x24 bytes");
	static_assert(sizeof(Rec) == 0x18, "spark record is 0x18 bytes");
}
}

namespace ff8fx
{
namespace z167
{
	inline Rec *RecOf(int i) { return (Rec *)(game_addr(POOL) + 0x18 * i); }

	static uint32_t __cdecl Master(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		uint32_t arena = var<uint32_t>(game_addr(game::unk_23BF02C));
		Cursor() = (*(const uint8_t *)&n->c & 1) ? arena + 0x8000 : arena;
		int left = ExecuteTaskQueue(&SubQueue());
		n->c++;
		return left ? 0 : TASK_END;
	}

	static void HideStage() { for (uint32_t a = game_addr(game::unk_1D98991); a < game_addr(game::unk_1D98A41); a += 0x2C) var<uint8_t>(a) |= 2; }
	static void ShowStage() { for (uint32_t a = game_addr(game::unk_1D98991); a < game_addr(game::unk_1D98A41); a += 0x2C) var<uint8_t>(a) &= 0xFD; }

	static void ClearFlag4()
	{
		var<uint16_t>((uint32_t)Caster()) &= 0xFFFB;
		for (uint32_t a = game_addr(game::BattleEntitySlotData); a < game_addr(game::unk_1D97494); a += 0x9C)
		{
			uint16_t f = var<uint16_t>(a);
			if (f & 2) var<uint16_t>(a) = (uint16_t)(f & 0xFFFB);
		}
	}

	static void SetFlag(uint8_t bit)
	{
		Caster()[0] |= bit;
		for (uint32_t a = game_addr(game::BattleEntitySlotData); a < game_addr(game::unk_1D97494); a += 0x9C)
		{
			uint16_t f = var<uint16_t>(a);
			if (f & 2) var<uint16_t>(a) = (uint16_t)(f | bit);
		}
	}

	static int32_t BonePosition(uint8_t *ent, int32_t bone, int32_t frac, int16_t *out)
	{
		int16_t v[3] = { 0, 0, 0 };
		if (!(ent[0] & 2))
		{
			out[0] = *(int16_t *)(ent + 0x1C);
			out[1] = *(int16_t *)(ent + 0x1E);
			out[2] = *(int16_t *)(ent + 0x20);
			return -1;
		}
		uint8_t *h = (uint8_t *)FieldAlloc(0x34);
		uint8_t *sk = **(uint8_t ***)(ent + 0x64);
		uint8_t *bones = sk + 0x10;
		int32_t f = frac;
		if (bone >= 0xF0)
		{
			uint32_t b = sk[bone - 0xEC];
			if (b & 0x80)
			{
				f = 0x1000;
				bone = (int32_t)(b & 0x7F);
			}
			else bone = (int32_t)b;
		}
		uint8_t *rec = bones + 0x30 * bone;
		if (*(int16_t *)rec >= 0) v[2] = (int16_t)(mul32(*(int16_t *)(rec + 2), f) >> 12);
		else v[2] = 0;
		GteSetRotMatrix((const Mat4x3 *)(rec + 0x10));
		GteSetTransVector((const Mat4x3 *)(rec + 0x10));
		TransformCoordinateToCameraSpace(v, h + 0x20, h + 0x30);
		out[0] = *(int16_t *)(h + 0x20);
		out[1] = *(int16_t *)(h + 0x24);
		out[2] = *(int16_t *)(h + 0x28);
		FieldFree(0x34);
		return 0;
	}

	static Node24 *Spawn(uint32_t task_fn)
	{
		Node24 *n = (Node24 *)AddTaskToQueue(&SubQueue(), task_fn);
		n->c = 0;
		return n;
	}

	static void SpawnSlices()
	{
		for (int32_t i = 0; i < (int32_t)Actions()[0x10]; i++)
		{
			uint32_t slot = (*(uint8_t **)(Actions() + 8))[0x18 * i];
			Node24 *n = (Node24 *)AddTaskToQueue(&SubQueue(), game_addr(ORIG_SliceTask));
			n->c = 0;
			n->e = (int16_t)slot;
			uint8_t *ent = Entity(slot);
			BonePosition(ent, 0xF1, 0, &n->p10);
			int32_t size = *(int16_t *)(ent + 0x26);
			int32_t cap = size;
			int32_t r = CrtRand() % 256;
			int32_t half = mul32(r + 0x80, size) >> 12;
			int32_t range = half * 2;
			int32_t d = CrtRand() % range;
			n->p10 = (int16_t)(n->p10 + (int16_t)(d - half));
			d = CrtRand() % range;
			n->p12 = (int16_t)(n->p12 + (int16_t)(d - half));
			d = CrtRand() % range;
			n->p14 = (int16_t)(n->p14 + (int16_t)(d - half));
			d = CrtRand() % 0x500;
			n->s1C = 0;
			n->a18 = (int16_t)(d - 0x280);
			if (cap > 0xA00) cap = 0xA00;
			d = CrtRand() % 0x30 + 0x30;
			n->s1E = (int16_t)(mul32(d, cap) >> 12);
		}
	}

	static uint32_t __cdecl TimelineTask(TaskNode *tn)
	{
		var<uint32_t>(game_addr(game::unk_23BF028)) = 1;
		if (Paused()) return 0;
		Node24 *n = (Node24 *)tn;
		if (n->c == 0x1B)
		{
			Spawn(game_addr(ORIG_SparkTask));
			for (uint32_t a = game_addr(SPARK_HIST); a < game_addr(game::unk_23BD444); a += 0x14) var<uint32_t>(a) = 0;
		}
		if (n->c == 0x2C)
		{
			Spawn(game_addr(ORIG_TrailTask));
			for (uint32_t a = game_addr(TRAIL_RECS); a < game_addr(game::unk_23BF028); a += 0x14) var<uint32_t>(a) = 0;
		}
		if (n->c == 0x40)
		{
			Node24 *p = Spawn(game_addr(ORIG_PrimATask));
			p->p10 = (int16_t)0xFF56;
			p->p12 = (int16_t)0xFFCE;
			p->s1E = 0x700;
			p->s1C = 0x700;
			p->s20 = 0x800;
		}
		if (n->c == 0x44)
		{
			Node24 *p = Spawn(game_addr(ORIG_PrimBTask));
			p->p10 = 0;
			p->p12 = 0;
			p->s1E = 0x500;
			p->s1C = 0x500;
		}
		if (n->c == 0x61 && Actions()[0x10] != 0) SpawnSlices();
		if (n->c == 0x40)
		{
			ShowStage();
			SetFlag(4);
		}
		if (n->c == 0x4D)
		{
			HideStage();
			ClearFlag4();
		}
		if (n->c == 0x48) ScreenFade(4, 3, 5, 0xFF);
		if (n->c == 0x4A)
		{
			SetFlag(8);
			uint8_t *e = Caster();
			var<uint32_t>(game_addr(game::unk_23BE118)) = *(uint32_t *)(e + 0x1C);
			uint32_t w = *(uint32_t *)(e + 0x20);
			*(int16_t *)(e + 0x1E) = (int16_t)(*(int16_t *)(e + 0x1E) + 0x2BC);
			*(int16_t *)(e + 0x20) = (int16_t)(*(int16_t *)(e + 0x20) + 0x1130);
			var<uint32_t>(game_addr(game::unk_23BE11C)) = w;
		}
		if (n->c == 0x8F) ApplyActionResultToTargets(*(uint32_t *)(Actions() + 8), Actions()[0x10]);
		if (n->c == 0x1C) BdPlaySE(game_addr(game::unk_D71564), 0, 0x80);
		n->c++;
		if (n->c <= 0x91) return 0;
		*(uint16_t *)(*(uint8_t **)(Caster() + 0x74) + 0x2C) |= 0x40;
		return TASK_END;
	}

	static int32_t BoneHistory(int32_t c, uint8_t *s)
	{
		GetEffectSpawnPosition(Caster(), 0x28, 0, (int16_t *)s);
		GetEffectSpawnPosition(Caster(), 0x28, 0x1000, (int16_t *)(s + 8));
		int16_t *a = (int16_t *)s, *b = (int16_t *)(s + 8);
		int16_t d0 = (int16_t)(b[0] - a[0]), d1 = (int16_t)(b[1] - a[1]), d2 = (int16_t)(b[2] - a[2]);
		a[0] = d0;
		a[1] = d1;
		a[2] = d2;
		NormalizeVector16(a, a);
		int32_t idx = c % 3;
		uint32_t *t = (uint32_t *)(game_addr(SPARK_HIST) + 0x14 * idx);
		t[0] = 1;
		t[1] = *(uint32_t *)(s + 8);
		t[2] = *(uint32_t *)(s + 0xC);
		t[3] = *(uint32_t *)(s + 0);
		t[4] = *(uint32_t *)(s + 4);
		int32_t n = 0;
		for (int32_t j = idx; n < 3; n++)
		{
			const uint32_t *e = (const uint32_t *)(game_addr(SPARK_HIST) + 0x14 * j);
			if (!e[0]) break;
			var<uint32_t>(game_addr(HIST_A) + 8 * n) = e[1];
			var<uint32_t>(game_addr(HIST_A) + 8 * n + 4) = e[2];
			var<uint32_t>(game_addr(HIST_B) + 8 * n) = e[3];
			var<uint32_t>(game_addr(HIST_B) + 8 * n + 4) = e[4];
			if (--j < 0) j = 2;
		}
		return n;
	}

	static void SparkSetup(uint8_t *h, uint8_t *s)
	{
		ff8fx::copy<(0x20)>(s + 0x10, &Camera());
		*(uint32_t *)h = game_addr(game::unk_D6F8D0);
		*(uint32_t *)(h + 0x1C) = 0x483830;
		*(int16_t *)(h + 0x24) = 0xC;
		GteSetLightMatrix(s + 0x10);
		GteSetBackColorFromTrans(s + 0x10);
	}

	static void SparkDrawOne(uint8_t *h, const int16_t *pos, int16_t size, int16_t frame)
	{
		GteLoadV0(pos);
		GteMVMVA_LightV0Bk();
		GteSetRotScale(size);
		*(int16_t *)(h + 4) = frame;
		GteTransFromMAC();
		Cursor() = InitEffectSequenceFromData(h, OT(), 2, Cursor());
	}

	static void Integrate(Rec *r)
	{
		r->size = (int16_t)(r->size + (int16_t)(r->size >> 5));
		r->x = (int16_t)(r->x + r->vx);
		r->y = (int16_t)(r->y + r->vy);
		r->z = (int16_t)(r->z + r->vz);
		r->vx = (int16_t)(r->vx - (int16_t)(r->vx >> 3));
		r->vy = (int16_t)(r->vy - (int16_t)(r->vy >> 3));
		r->vz = (int16_t)(r->vz - (int16_t)(r->vz >> 3));
	}

	static void SplineRails(int32_t n, int32_t points, int32_t div)
	{
		uint8_t *w = (uint8_t *)FieldAlloc(0x190);
		SplineSetup(n, game_addr(HIST_A), w);
		for (int32_t i = 0, t = 0; i < points; i++, t += 0x1000) SplineEval(n, w, game_addr(SPLINE_A) + 8 * i, t / div);
		SplineSetup(n, game_addr(HIST_B), w);
		for (int32_t i = 0, t = 0; i < points; i++, t += 0x1000) SplineEval(n, w, game_addr(SPLINE_B) + 8 * i, t / div);
		FieldFree(0x190);
	}

	static uint32_t __cdecl SparkTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		uint8_t *s = (uint8_t *)FieldAlloc(0x50);
		int32_t hist = BoneHistory(n->c, s);
		SparkSetup(h, s);
		int32_t count = 0;
		for (int i = 0; i < 100; i++)
		{
			Rec *r = RecOf(i);
			if (!(*(const uint8_t *)&r->alive & 1)) continue;
			SparkDrawOne(h, &r->x, r->size, r->age);
			if (Paused()) continue;
			r->age++;
			if (*(int16_t *)(h + 0x28) < 0)
			{
				r->alive = 0;
				continue;
			}
			Integrate(r);
			count++;
		}
		FieldFree(0x50);
		FieldFree(0xB4);
		if (Paused()) return 0;
		int16_t c = n->c;
		if (c >= 0 && c <= 0xB && hist > 1)
		{
			SplineRails(hist, 6, 5);
			for (int j = 0; j < 5; j++)
			{
				int i = 0;
				while (i < 100 && RecOf(i)->alive != 0) i++;
				if (i >= 100) break;
				Rec *r = RecOf(i);
				r->alive = 1;
				r->age = 0;
				int32_t sz = CrtRand() % 0x380;
				uint32_t pa = var<uint32_t>(game_addr(SPLINE_A) + 8 * j), pb = var<uint32_t>(game_addr(SPLINE_A) + 8 * j + 4);
				r->size = (int16_t)(sz + 0x280);
				*(uint32_t *)&r->x = pa;
				*(uint32_t *)&r->z = pb;
				r->x = (int16_t)(r->x + (int16_t)(CrtRand() % 10 - 5));
				r->y = (int16_t)(r->y + (int16_t)(CrtRand() % 10 - 5));
				r->z = (int16_t)(r->z + (int16_t)(CrtRand() % 10 - 5));
				int32_t sp = CrtRand() % 0xF + 0x19;
				const int16_t *d = (const int16_t *)(game_addr(SPLINE_B) + 8 * j);
				int32_t k = CrtRand() % 10;
				r->vx = (int16_t)(k + (mul32(d[0], sp) >> 12) - 5);
				k = CrtRand() % 10;
				r->vy = (int16_t)(k + (mul32(d[1], sp) >> 12) - 5);
				k = CrtRand() % 10;
				r->vz = (int16_t)(k + (mul32(d[2], sp) >> 12) - 5);
			}
		}
		n->c++;
		if (n->c < 4 || count != 0) return 0;
		return TASK_END;
	}

	static void BladePoints(uint8_t *s, int16_t *p0, int16_t *p1)
	{
		int16_t v[4] = { 0x400, 0, 0x800, 0 };
		Mat4x3 m1, m2;
		ComposeZYXRotationMatrix(v, &m1);
		uint8_t *e = Caster();
		int32_t *sc = (int32_t *)(s + 0x30);
		m1.t[0] = 0;
		m1.t[1] = 0;
		m1.t[2] = 0;
		sc[2] = 0x1A00;
		sc[1] = 0x1A00;
		sc[0] = 0x1A00;
		Scale3DMatrix(&m1, sc);
		ComposeAffineTransform((const Mat4x3 *)(e + 0x40), (const Mat4x3 *)(**(uint8_t ***)(e + 0x64) + 0xB30), &m2);
		GteMatrixMultiply(&m2, &m1);
		v[0] = 0;
		v[1] = 0;
		v[2] = 0;
		TransformVectorBy3x3Matrix(&m2, m1.t, m1.t);
		int32_t t0 = (int32_t)((uint32_t)m1.t[0] + (uint32_t)m2.t[0]);
		int32_t t1 = (int32_t)((uint32_t)m1.t[1] + (uint32_t)m2.t[1]);
		int32_t t2 = (int32_t)((uint32_t)m1.t[2] + (uint32_t)m2.t[2]);
		m1.t[1] = t1;
		m1.t[2] = t2;
		m1.t[0] = t0;
		v[0] = 0;
		v[1] = 1000;
		v[2] = -1000;
		MatrixMultiplyVector(&m1, v, p0);
		p0[0] = (int16_t)(p0[0] + (int16_t)t0);
		p0[1] = (int16_t)(p0[1] + (int16_t)t1);
		p0[2] = (int16_t)(p0[2] + (int16_t)t2);
		v[0] = 0;
		v[1] = 0x1838;
		v[2] = 0x258;
		MatrixMultiplyVector(&m1, v, p1);
		p1[0] = (int16_t)(p1[0] + (int16_t)t0);
		p1[2] = (int16_t)(p1[2] + (int16_t)t2);
		p1[1] = (int16_t)(p1[1] + (int16_t)t1);
	}

	static void TrailDraw(uint32_t rail_a, uint32_t rail_b, uint8_t *s)
	{
		GteSetRotMatrix(&Camera());
		GteSetTransVector(&Camera());
		GteSetFarColor2(0, 0, 0);
		uint32_t pk = Cursor();
		*(uint32_t *)(s + 0x14) = 0x3A903030;
		*(uint32_t *)(s + 0x1C) = 0x3A903030;
		uint32_t pk2 = pk + 0x24;
		int32_t acc = 0;
		for (uint32_t j = 0; j < 0xF0; j += 8, acc += 0x1000)
		{
			*(uint32_t *)(s + 0x10) = *(uint32_t *)(s + 0x14);
			uint32_t *a = (uint32_t *)pk, *b = (uint32_t *)pk2;
			a[0] = 0x8000000;
			GteLoadV012((const void *)(rail_a + j), (const void *)(rail_a + j + 8), (const void *)(rail_b + j));
			GteRTPT();
			GteReadFLAG(s + 0xC);
			if (*(uint32_t *)(s + 0xC) & 0x60000) continue;
			GteReadSXY012Split(a + 2, a + 4, a + 6);
			GteLoadV0((const void *)(rail_b + j + 8));
			GteRTPS();
			GteReadSXY2(a + 8);
			GteAVSZ4();
			GteReadOTZ32(s + 8);
			GteSetIR0(acc / 30);
			GteLoadRGBC(s + 0x1C);
			GteDPCS();
			GteStoreRGB2(s + 0x14);
			uint32_t c0 = *(uint32_t *)(s + 0x10), c1 = *(uint32_t *)(s + 0x14);
			a[5] = c0;
			a[1] = c0;
			a[7] = c1;
			a[3] = c1;
			InsertPrimAutoDepth(var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 4 * (uint32_t)(*(int32_t *)(s + 8) >> 2) + 0x44, a);
			b[5] = *(uint32_t *)(s + 0x10);
			b[3] = 0x3A000000;
			b[1] = 0x3A000000;
			b[2] = a[2];
			b[7] = *(uint32_t *)(s + 0x14);
			b[4] = a[4];
			b[6] = a[6];
			b[8] = a[8];
			b[0] = 0x8000000;
			InsertPrimAutoDepth(var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 4 * (uint32_t)(*(int32_t *)(s + 8) >> 2) + 0x44, b);
			pk += 0x48;
			pk2 += 0x48;
		}
		Cursor() = pk2;
	}

	static uint32_t __cdecl TrailTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		uint8_t *s = (uint8_t *)FieldAlloc(0x40);
		BladePoints(s, (int16_t *)game_addr(BLADE_P0), (int16_t *)game_addr(BLADE_P1));
		int32_t idx = (int32_t)n->c % 10;
		uint32_t *rec = (uint32_t *)(game_addr(TRAIL_RECS) + 0x14 * idx);
		rec[0] = 1;
		rec[1] = var<uint32_t>(game_addr(BLADE_P0));
		rec[2] = var<uint32_t>(game_addr(BLADE_P0) + 4);
		rec[3] = var<uint32_t>(game_addr(BLADE_P1));
		rec[4] = var<uint32_t>(game_addr(BLADE_P1) + 4);
		int32_t cnt = 0;
		for (int32_t j = idx; cnt < 10; cnt++)
		{
			const uint32_t *e = (const uint32_t *)(game_addr(TRAIL_RECS) + 0x14 * j);
			if (!e[0]) break;
			var<uint32_t>(game_addr(HIST_A) + 8 * cnt) = e[1];
			var<uint32_t>(game_addr(HIST_A) + 8 * cnt + 4) = e[2];
			const uint32_t *src = cnt == 0 ? (const uint32_t *)game_addr(BLADE_P1) : e + 3;
			var<uint32_t>(game_addr(HIST_B) + 8 * cnt) = src[0];
			var<uint32_t>(game_addr(HIST_B) + 8 * cnt + 4) = src[1];
			if (--j < 0) j = 9;
		}
		if (cnt > 1)
		{
			SplineRails(cnt, 32, 31);
			TrailDraw(game_addr(SPLINE_A), game_addr(SPLINE_B), s);
		}
		FieldFree(0x40);
		if (Paused()) return 0;
		n->c++;
		return n->c >= 0x14 ? TASK_END : 0;
	}

	static void PrimDraw(const Node24 *n, uint32_t model, int16_t rz, int32_t sx, int32_t syz)
	{
		int16_t v[4] = { 0, 0, rz, 0 };
		Mat4x3 m;
		ComposeZYXRotationMatrix(v, &m);
		m.t[2] = 0x120;
		m.t[1] = n->p12;
		m.t[0] = n->p10;
		int32_t sc[3] = { sx, syz, syz };
		Scale3DMatrix(&m, sc);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		*(uint32_t *)h = model;
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0x1C) = 0x33;
		SetProjection(0x120);
		Cursor() = RenderPrimModel(h, OT(), 2, Cursor());
		SetProjection(var<int16_t>(game_addr(game::BATTLE_PROJECTION_DISTANCE)));
		FieldFree(0x58);
	}

	static uint32_t PrimTask(Node24 *n, int16_t end)
	{
		if (Paused()) return 0;
		int16_t v = n->s1E;
		n->s1C = (int16_t)(n->s1C + v);
		n->s1E = (int16_t)(v - (int16_t)((int32_t)v / 3));
		n->c++;
		return n->c >= end ? TASK_END : 0;
	}

	static uint32_t __cdecl PrimATask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		PrimDraw(n, game_addr(game::unk_D6FA7C), 0x8AC, n->s1C, n->s20);
		return PrimTask(n, 0xD);
	}

	static uint32_t __cdecl PrimBTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		PrimDraw(n, game_addr(game::unk_D7042C), 0, n->s1C, n->s1C);
		return PrimTask(n, 9);
	}

	static void PlaneDistance(const int16_t *p, int16_t *nrm)
	{
		int32_t d = mul32(p[2], nrm[2]) + mul32(p[1], nrm[1]);
		d = d + mul32(p[0], nrm[0]);
		nrm[3] = (int16_t)-(d >> 12);
	}

	static uint32_t g_vertex_scratch_override = 0;

	static void TargetDraw(uint8_t *h, uint8_t *ent, const Mat4x3 *moved)
	{
		if (!(ent[0] & 0x20))
			EngineCursor() = DrawShadow(ent, var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x4064, 0x10, EngineCursor());
		Mat4x3 *r0 = (Mat4x3 *)(h + 0x190), *r1 = (Mat4x3 *)(h + 0x1B0);
		ComposeAffineTransform(&Camera(), (const Mat4x3 *)(ent + 0x40), r0);
		ComposeAffineTransform(&Camera(), moved, r1);
		*(uint32_t *)(h + 4) = g_vertex_scratch_override ? g_vertex_scratch_override : var<uint32_t>(game_addr(game::g_BattleVertexWorkspace));
		*(uint32_t *)(h + 0x18) = *(uint32_t *)(ent + 0x28);
		*(uint32_t *)(h + 0x10) = *(uint32_t *)(ent + 0x7C);
		EngineCursor() = SliceRender(*(void **)(ent + 0x64), h, OT(), 2, EngineCursor());
		BuildBoneMatricesFromPose(ent + 0x60);
		uint8_t *sub = *(uint8_t **)(ent + 0x78);
		if (sub && !(ent[1] & 2))
		{
			*(uint32_t *)(h + 0x10) = 0xFFFFFFFF;
			ComposeAffineTransform(r0, (const Mat4x3 *)(sub + 0x24), r0);
			ComposeAffineTransform(r1, (const Mat4x3 *)(*(uint8_t **)(ent + 0x78) + 0x24), r1);
			EngineCursor() = SliceRender(*(void **)(sub + 4), h, OT(), 2, EngineCursor());
			BuildBoneMatricesFromPose(sub);
		}
	}

	static int32_t SliceFadeW(int32_t c) { return 0x1000 - ComputeSin(shl32(c - 0x20, 10) / 8); }

	static void SliceDraw(uint8_t *h, const Node24 *n, uint8_t *ent, int32_t offset, int32_t w)
	{
		int16_t v[4] = { 0, 0, n->a18, 0 };
		Mat4x3 m;
		ComposeZYXRotationMatrix(v, &m);
		*(uint32_t *)(h + 0x40) = *(const uint32_t *)&n->p10;
		*(uint32_t *)(h + 0x44) = *(const uint32_t *)&n->p14;
		int16_t *nrm = (int16_t *)(h + 0xA0);
		nrm[0] = 0;
		nrm[1] = (int16_t)0xF000;
		nrm[2] = 0;
		MatrixMultiplyVector(&m, nrm, nrm);
		PlaneDistance((const int16_t *)(h + 0x40), nrm);
		v[0] = 0x1000;
		v[1] = 0;
		v[2] = 0;
		GteMatrixMultiply((const Mat4x3 *)(ent + 0x40), &m);
		MatrixMultiplyVector(&m, v, v);
		int32_t d = offset;
		if (v[1] < 0) d = -d;
		Mat4x3 moved = *(const Mat4x3 *)(ent + 0x40);
		moved.t[0] = (int32_t)((uint32_t)moved.t[0] + (uint32_t)(mul32(v[0], d) >> 12));
		moved.t[1] = (int32_t)((uint32_t)moved.t[1] + (uint32_t)(mul32(v[1], d) >> 12));
		moved.t[2] = (int32_t)((uint32_t)moved.t[2] + (uint32_t)(mul32(v[2], d) >> 12));
		if (w >= 0)
		{
			uint32_t k = (uint32_t)(shl32(w, 7) >> 12);
			uint32_t col = ((((k | 0x3200u) << 8) | k) << 8) | k;
			*(uint32_t *)(ent + 0x2C) = col;
			GteSetFarColor(var<uint8_t>(game_addr(game::CURRENT_BS_MODEL_PALETTE)), var<uint8_t>(game_addr(game::unk_B8B7D9)), var<uint8_t>(game_addr(game::unk_B8B7DA)));
			uint32_t zero = 0;
			GteLoadRGBC(&zero);
			GteSetIR0(w);
			GteDPCS();
			GteStoreRGB2(ent + 0x28);
			ent[0x2B] = 2;
			ent[7] = 0;
		}
		TargetDraw(h, ent, &moved);
	}

	static uint32_t __cdecl SliceTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		if (n->c < 0x28)
		{
			uint8_t *ent = Entity((uint32_t)(int32_t)n->e);
			ent[0] |= 4;
			uint8_t *h = (uint8_t *)FieldAlloc(0x1D0);
			SliceDraw(h, n, ent, n->s1C, n->c >= 0x20 ? SliceFadeW(n->c) : -1);
			FieldFree(0x1D0);
		}
		if (Paused()) return 0;
		int16_t cc = n->c;
		if (cc < 4) n->s1C = (int16_t)(n->s1C + (int16_t)(n->s1E * 3));
		else
		{
			int16_t v = n->s1E;
			n->s1C = (int16_t)(n->s1C + v);
			int16_t nv = (int16_t)(v - (int16_t)((int32_t)v / 10));
			n->s1E = nv < 2 ? (int16_t)2 : nv;
		}
		n->c = (int16_t)(cc + 1);
		return n->c > 0x28 ? TASK_END : 0;
	}
}

	void register_mag167_zantetsuken()
	{
		register_port(z167::ORIG_Master, (void *)z167::Master, "167 Master", 167);
		register_port(z167::ORIG_TimelineTask, (void *)z167::TimelineTask, "167 TimelineTask", 167);
		register_port(z167::ORIG_SparkTask, (void *)z167::SparkTask, "167 SparkTask", 167);
		register_port(z167::ORIG_TrailTask, (void *)z167::TrailTask, "167 TrailTask", 167);
		register_port(z167::ORIG_PrimATask, (void *)z167::PrimATask, "167 PrimATask", 167);
		register_port(z167::ORIG_PrimBTask, (void *)z167::PrimBTask, "167 PrimBTask", 167);
		register_port(z167::ORIG_SliceTask, (void *)z167::SliceTask, "167 SliceTask", 167);
	}
}

