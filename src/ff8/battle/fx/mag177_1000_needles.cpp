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

// Effect 177: 1000 Needles

#include "mag_common.h"

namespace ff8fx
{
namespace needles177
{
	using namespace eng;
	using namespace magc;

	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::unk_2348550)); }
	inline uint32_t &TargetSlot() { return var<uint32_t>(game_addr(game::unk_2341948)); }
	inline uint32_t &CasterSlot() { return var<uint32_t>(game_addr(game::unk_2348554)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::unk_236855C)); }
	inline TaskQueue *EffectQueue() { return (TaskQueue *)game_addr(game::unk_2347730); }
	static constexpr game::Id ARENA0 = game::unk_234855C, ARENA1 = game::unk_235855C; static const uint32_t ARENA_SIZE = 0x10000;
	static constexpr game::Id SPRITE_MATRIX = game::unk_2368560;
	static constexpr game::Id NEEDLES = game::unk_2341970, NEEDLES_END = game::unk_2345B10;
	static constexpr game::Id SPARKS = game::unk_2345B10, SPARKS_END = game::unk_2347730;
	static const int NEEDLE_COUNT = 300, SPARK_COUNT = 300;

	static constexpr game::Id ORIG_RootTask = game::au_re_BdlinkTask_12;
	static constexpr game::Id ORIG_Director = game::MAG_177_sub_5CF820;
	static constexpr game::Id ORIG_NeedleTask = game::MAG_177_sub_5CF8E0;
	static constexpr game::Id NEEDLE_VERTS = game::unk_D44E14;
	static constexpr game::Id NEEDLE_POLYS = game::unk_D44E34;
	static constexpr game::Id SEQ_Spark = game::unk_D44DC0;
	static constexpr game::Id SPARK_LIFT = game::unk_D44E88;
	static const void *SOUND_Needles() { return (const void *)game_addr(game::unk_D44E10); }
	static const int16_t TICK_SPAWN = 1, TICK_DAMAGE = 0x32, TICK_LAST = 0x37;
	static const int32_t SCALE_RANGE = 0x400, SCALE_BASE = 0x300;

	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t frac, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, frac, out); }
	inline void GteLoadIRFromMatrixColumn(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadIRFromMatrixColumn))(m); }
	inline void GteMVMVA_RotIR() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotIR))(); }
	inline void GteStoreIRToMatrixColumn(void *m) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreIRToMatrixColumn))(m); }
	inline void GteLoadV0FromDwords(const int32_t *v) { fn<void (__cdecl *)(const int32_t *)>(game_addr(game::GTE_LoadV0FromDwords))(v); }
	inline void GteLoadV012(const void *a, const void *b, const void *c) { fn<void (__cdecl *)(const void *, const void *, const void *)>(game_addr(game::GTE_LoadV012))(a, b, c); }
	inline void GteRTPT() { fn<void (__cdecl *)()>(game_addr(game::GTE_RTPT))(); }
	inline void GteReadFLAG(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadFLAG))(dst); }
	inline void GteNCLIP() { fn<void (__cdecl *)()>(game_addr(game::GTE_NCLIP))(); }
	inline void GteReadMAC0(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadMAC0))(dst); }
	inline void GteReadSXY012(void *a, void *b, void *c) { fn<void (__cdecl *)(void *, void *, void *)>(game_addr(game::GTE_ReadSXY012_Split))(a, b, c); }
	inline void GteAVSZ3() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ3))(); }
	inline void GteReadOTZ32(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadOTZ))(dst); }

#pragma pack(push, 1)
	struct RootNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad;
	};
	struct EffectNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		int16_t pos[4];
		uint8_t pad18[0xC];
	};
	struct Needle
	{
		uint32_t active;
		int16_t steps;
		uint8_t pad06[0xA];
		int16_t step[3];
		int16_t pad16;
		Mat4x3 m;
	};
	struct Spark
	{
		uint32_t active;
		int16_t left;
		int16_t pad06;
		int16_t pos[3];
		uint8_t pad0E[0xA];
	};
	struct PolyRec
	{
		uint32_t code;
		uint16_t v[3];
		uint16_t pad;
		uint32_t uv0, uv1;
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x10 && sizeof(EffectNode) == 0x24, "1000 Needles nodes");
	static_assert(sizeof(Needle) == 0x38 && sizeof(Spark) == 0x18 && sizeof(PolyRec) == 0x14, "1000 Needles tables");

	inline Needle *Needles() { return (Needle *)game_addr(NEEDLES); }
	inline Spark *Sparks() { return (Spark *)game_addr(SPARKS); }
	inline uint8_t *Caster() { return Entity((int)CasterSlot()); }
	inline uint8_t *Target() { return Entity((int)TargetSlot()); }
	inline int32_t EffectBones(uint8_t *entity) { return **(const uint8_t *const *)*(const uint32_t *)(entity + 0x64); }

	static uint32_t DrawNeedlePolys(uint8_t *b, uint32_t ot, int shift, uint32_t cursor)
	{
		const PolyRec *rec = (const PolyRec *)(b + 0x8C);
		uint8_t *verts = b + 0x6C;
		for (int k = 0; k < 4; k++, rec++)
		{
			uint32_t *p = (uint32_t *)cursor;
			GteLoadV012(verts + rec->v[0] * 4, verts + rec->v[1] * 4, verts + rec->v[2] * 4);
			GteRTPT();
			p[0] = 0x6000000;
			p[1] = rec->code;
			GteReadFLAG(b + 0xC);
			if (*(int32_t *)(b + 0xC) & 0x60000) continue;
			GteNCLIP();
			GteReadMAC0(b);
			if (*(int32_t *)b <= 0) continue;
			int16_t *xy0 = (int16_t *)(cursor + 8), *xy1 = (int16_t *)(cursor + 0x10), *xy2 = (int16_t *)(cursor + 0x18);
			GteReadSXY012(xy0, xy1, xy2);
			GteAVSZ3();
			uint32_t off = 0;
			if (xy0[0] < 0 || xy0[0] > 0xA00) off = 1;
			if (xy1[0] < 0 || xy1[0] > 0xA00) off |= 2;
			if (xy2[0] < 0 || xy2[0] > 0xA00) off |= 4;
			if (xy0[1] < 0 || xy0[1] > 0x6C0) off |= 0x10;
			if (xy1[1] < 0 || xy1[1] > 0x6C0) off |= 0x20;
			if (xy2[1] < 0 || xy2[1] > 0x6C0) off |= 0x40;
			if ((off & 7) == 7 || (off & 0x70) == 0x70) continue;
			GteReadOTZ32(b + 8);
			p[3] = rec->uv0;
			p[5] = rec->uv1;
			InsertPrimAutoDepth(ot + (uint32_t)(*(int32_t *)(b + 8) >> shift) * 4, p);
			cursor += 0x1C;
		}
		return cursor;
	}

	static void DrawNeedle(uint8_t *b, const Needle *n)
	{
		Mat4x3 *cam = (Mat4x3 *)(b + 0x2C);
		Mat4x3 *m = (Mat4x3 *)(b + 0x4C);
		GteSetRotMatrixCtrl(cam);
		GteLoadIRFromMatrixColumn(&n->m.m[0][0]);
		GteMVMVA_RotIR();
		GteStoreIRToMatrixColumn(&m->m[0][0]);
		GteLoadIRFromMatrixColumn(&n->m.m[0][1]);
		GteMVMVA_RotIR();
		GteStoreIRToMatrixColumn(&m->m[0][1]);
		GteLoadIRFromMatrixColumn(&n->m.m[0][2]);
		GteMVMVA_RotIR();
		GteStoreIRToMatrixColumn(&m->m[0][2]);
		GteSetTransVectorCtrl(cam);
		GteLoadV0FromDwords(n->m.t);
		GteMVMVA_RotV0Tr();
		GteReadMAC123(m->t);
		GteSetRotMatrixCtrl(m);
		GteSetTransVectorCtrl(m);
		PacketCursor() = DrawNeedlePolys(b, RenderOT(), 2, PacketCursor());
	}

	static Mat4x3 *SpriteTransform(const int16_t *pos, int16_t scale, int32_t z)
	{
		Mat4x3 *m = (Mat4x3 *)game_addr(SPRITE_MATRIX);
		m->m[2][2] = scale;
		m->m[1][1] = scale;
		m->m[0][0] = scale;
		GteSetRotMatrix(&Camera());
		GteSetTransVector(&Camera());
		GteLoadV0(pos);
		GteRTPS();
		int16_t sxy[2];
		GteReadSXY2(sxy);
		sxy[0] = (int16_t)(sxy[0] / 8);
		sxy[1] = (int16_t)(sxy[1] / 8);
		m->t[0] = sxy[0] - 0xA0;
		m->t[1] = sxy[1] - 0x6C;
		m->t[2] = var<int16_t>(game_addr(game::BATTLE_PROJECTION_DISTANCE)) + z;
		GteSetRotMatrix(m);
		return m;
	}

	static void DrawSpark(uint8_t *h, const Spark *s, int32_t lift)
	{
		*(int16_t *)(h + 4) = 0;
		Mat4x3 *m = SpriteTransform(s->pos, 0x100, 0);
		m->t[1] += lift;
		GteSetTransVectorCtrl(m);
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
	}
	inline int32_t SparkLift(int16_t left) { return var<int32_t>(game_addr(SPARK_LIFT) + (uint32_t)(int32_t)left * 4); }

	static uint8_t *NeedleBlock()
	{
		uint8_t *b = (uint8_t *)FieldAlloc(0xDC);
		ff8fx::copy<(0x20)>(b + 0x6C, (const void *)game_addr(NEEDLE_VERTS));
		ff8fx::copy<(0x50)>(b + 0x8C, (const void *)game_addr(NEEDLE_POLYS));
		ff8fx::copy<(0x20)>(b + 0x2C, &Camera());
		return b;
	}
	static uint8_t *SparkHeader()
	{
		uint8_t *seq = (uint8_t *)FieldAlloc(0x28);
		ff8fx::copy<(0x28)>(seq, (const void *)game_addr(SEQ_Spark));
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		*(uint8_t **)h = seq;
		*(int16_t *)(h + 0x24) = 0;
		return h;
	}

	static int FreeNeedle()
	{
		int i = 0;
		for (uint32_t a = game_addr(NEEDLES); a < game_addr(NEEDLES_END); a += sizeof(Needle), i++)
			if (*(uint32_t *)a == 0) return i < NEEDLE_COUNT ? i : -1;
		return -1;
	}
	static int FreeSpark()
	{
		int i = 0;
		for (uint32_t a = game_addr(SPARKS); a < game_addr(SPARKS_END); a += sizeof(Spark), i++)
			if (*(uint32_t *)a == 0) return i < SPARK_COUNT ? i : -1;
		return -1;
	}

	static void SpawnNeedles(uint8_t *s)
	{
		int16_t *from = (int16_t *)s, *to = (int16_t *)(s + 8);
		int32_t *scale = (int32_t *)(s + 0x10), *ref = (int32_t *)(s + 0x20), *dir = (int32_t *)(s + 0x30), *axis = (int32_t *)(s + 0x40);
		const int32_t caster_bones = EffectBones(Caster());
		const int32_t target_bones = EffectBones(Target());
		for (int k = 0; k < 0x19; k++)
		{
			const int i = FreeNeedle();
			if (i < 0) return;
			Needle *n = &Needles()[i];
			n->active = 1;
			const int32_t steps = CrtRand() % 4 + 4;
			n->steps = (int16_t)steps;
			int32_t r = CrtRand();
			GetEffectSpawnPosition(Caster(), r % caster_bones, r % 0x1000, from);
			from[0] = (int16_t)(from[0] + (CrtRand() % 200 - 100));
			from[1] = (int16_t)(from[1] + (CrtRand() % 200 - 100));
			from[2] = (int16_t)(from[2] + (CrtRand() % 200 - 100));
			r = CrtRand();
			GetEffectSpawnPosition(Target(), r % target_bones, r % 0x1000, to);
			to[0] = (int16_t)(to[0] + (CrtRand() % 200 - 100));
			to[1] = (int16_t)(to[1] + (CrtRand() % 200 - 100));
			to[2] = (int16_t)(to[2] + (CrtRand() % 200 - 100));
			dir[0] = to[0] - from[0];
			dir[1] = to[1] - from[1];
			dir[2] = to[2] - from[2];
			n->step[0] = (int16_t)(dir[0] / steps);
			n->step[1] = (int16_t)(dir[1] / steps);
			n->step[2] = (int16_t)(dir[2] / steps);
			NormalizeVector(dir, dir);
			const int32_t angle = RotationBetweenVectors(ref, dir, axis);
			BuildAxisAngleRotationMatrix(angle, &n->m, axis);
			const int32_t sc = CrtRand() % SCALE_RANGE + SCALE_BASE;
			scale[2] = sc;
			scale[1] = sc;
			scale[0] = sc;
			Scale3DMatrix(&n->m, scale);
			n->m.t[0] = from[0];
			n->m.t[1] = from[1];
			n->m.t[2] = from[2];
		}
	}
}
}

namespace ff8fx
{
namespace needles177
{
	static uint32_t __cdecl NeedleTask(TaskNode *nd)
	{
		EffectNode *t = (EffectNode *)nd;
		uint8_t *s = (uint8_t *)FieldAlloc(0x50);
		*(int32_t *)(s + 0x20) = 0;
		*(int32_t *)(s + 0x24) = 0;
		*(int32_t *)(s + 0x28) = -0x1000;
		if (!DrawOnly() && t->counter >= 0 && t->counter <= 0x27) SpawnNeedles(s);
		FieldFree(0x50);
		uint8_t *b = NeedleBlock();
		int moved = 0;
		for (Needle *n = Needles(); (uint32_t)n < game_addr(NEEDLES_END); n++)
		{
			if (!(n->active & 1)) continue;
			DrawNeedle(b, n);
			if (DrawOnly()) continue;
			n->steps--;
			if (n->steps >= 0)
			{
				n->m.t[0] += n->step[0];
				n->m.t[1] += n->step[1];
				n->m.t[2] += n->step[2];
				moved++;
				continue;
			}
			n->active = 0;
			const int i = FreeSpark();
			if (i < 0) continue;
			Spark *sp = &Sparks()[i];
			sp->active = 1;
			sp->left = 6;
			sp->pos[0] = (int16_t)n->m.t[0];
			sp->pos[1] = (int16_t)n->m.t[1];
			sp->pos[2] = (int16_t)n->m.t[2];
		}
		FieldFree(0xDC);
		uint8_t *h = SparkHeader();
		int alive = 0;
		for (Spark *sp = Sparks(); (uint32_t)sp < game_addr(SPARKS_END); sp++)
		{
			if (!(sp->active & 1)) continue;
			const int16_t left = sp->left;
			DrawSpark(h, sp, SparkLift(left));
			if (DrawOnly()) continue;
			const int32_t next = (int32_t)left - 1;
			if (next > 0)
			{
				sp->left = (int16_t)next;
				alive++;
			}
			else sp->active = 0;
		}
		FieldFree(0xB4);
		FieldFree(0x28);
		if (DrawOnly()) return 0;
		t->counter++;
		return (alive | moved) ? 0 : TASK_END;
	}

	static uint32_t __cdecl DirectorTask(TaskNode *nd)
	{
		if (DrawOnly()) return 0;
		EffectNode *t = (EffectNode *)nd;
		if (t->counter == 0) GetEffectSpawnPosition(Target(), 0xF0, 0x1000, t->pos);
		if (t->counter == TICK_SPAWN)
		{
			EffectNode *n = (EffectNode *)AddTaskToQueue(EffectQueue(), game_addr(ORIG_NeedleTask));
			*(uint32_t *)&n->pos[0] = *(const uint32_t *)&t->pos[0];
			n->counter = 0;
			*(uint32_t *)&n->pos[2] = *(const uint32_t *)&t->pos[2];
		}
		if (t->counter == TICK_DAMAGE) ApplyActionResultToTarget(Ctx()->actions[0].targets);
		if (t->counter == TICK_SPAWN) BdPlaySE(SOUND_Needles(), 0, 0x80);
		t->counter++;
		return t->counter > TICK_LAST ? TASK_END : 0;
	}

	static uint32_t __cdecl RootTask(TaskNode *nd)
	{
		RootNode *r = (RootNode *)nd;
		PacketCursor() = game_addr(ARENA1);
		if (!(r->counter & 1)) PacketCursor() = game_addr(ARENA0);
		const int a = ExecuteTaskQueue(EffectQueue());
		r->counter++;
		return a ? 0 : TASK_END;
	}
}

	void register_mag177_1000_needles()
	{
		register_port(needles177::ORIG_RootTask, (void *)needles177::RootTask, "N177 RootTask", 177);
		register_port(needles177::ORIG_Director, (void *)needles177::DirectorTask, "N177 Director", 177);
		register_port(needles177::ORIG_NeedleTask, (void *)needles177::NeedleTask, "N177 NeedleTask", 177);
	}
}

