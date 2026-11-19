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

// Effect 112: Meteor Barret

#include "mag_common.h"

namespace ff8fx
{
namespace meteor112
{
	using namespace eng;
	using namespace magc;

	inline uint8_t *&Caster() { return var<uint8_t *>(game_addr(game::MAG_112_METEOR_BARRET_Caster)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::MAG_112_METEOR_BARRET_TexBase)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::MAG_112_METEOR_BARRET_Ctx)); }
	inline int16_t &ColumnTop() { return var<int16_t>(game_addr(game::MAG_112_METEOR_BARRET_ColumnTop)); }
	inline int16_t &ColumnSpan() { return var<int16_t>(game_addr(game::MAG_112_METEOR_BARRET_ColumnSpan)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_112_METEOR_BARRET_PacketCursor)); }
	inline const void *Black() { return (const void *)game_addr(game::MAG_112_METEOR_BARRET_ZeroColour); }
	inline TaskQueue *QDebris() { return (TaskQueue *)game_addr(game::MAG_112_METEOR_BARRET_QDebris); }
	inline TaskQueue *QSparks() { return (TaskQueue *)game_addr(game::MAG_112_METEOR_BARRET_QSparks); }
	inline TaskQueue *QSmoke() { return (TaskQueue *)game_addr(game::MAG_112_METEOR_BARRET_QSmoke); }
	inline TaskQueue *QDirector() { return (TaskQueue *)game_addr(game::MAG_112_METEOR_BARRET_QDirector); }
	inline uint32_t OTBase() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)); }

	static constexpr game::Id ORIG_RootTask = game::MAG_112_METEOR_BARRET_RootTask;
	static constexpr game::Id ORIG_DirectorTask = game::MAG_112_METEOR_BARRET_DirectorTask;
	static constexpr game::Id ORIG_SmokeTask = game::MAG_112_METEOR_BARRET_SmokeTask;
	static constexpr game::Id ORIG_DebrisTask = game::MAG_112_METEOR_BARRET_DebrisTask;
	static constexpr game::Id ORIG_SparkTask = game::MAG_112_METEOR_BARRET_SparkTask;
	static constexpr game::Id ORIG_TexRestoreTask = game::MAG_112_METEOR_BARRET_TexRestoreTask;

	static constexpr game::Id COM_Anim = game::MAG_112_METEOR_BARRET_ComAnim;
	static constexpr game::Id SEQ_Smoke = game::unk_12C5650;
	static constexpr game::Id SEQ_Spark = game::unk_12C5794;
	static constexpr game::Id MODEL_Column = game::unk_12C5928;
	static constexpr game::Id MODEL_Debris = game::unk_12C7230;
	static constexpr game::Id LAYOUT_Ground = game::unk_12C7420, LAYOUT_HandA = game::unk_12C8020, LAYOUT_HandB = game::unk_12CA7B4, LAYOUT_Fist = game::unk_12CB40C;
	static constexpr game::Id LAYOUT_Target = game::unk_12D19F4, LAYOUT_Billboard = game::unk_12D4684, LAYOUT_ImpactA = game::unk_12D8D28, LAYOUT_ImpactB = game::unk_12D9DC0;
	static constexpr game::Id FLAGS_Ground = game::unk_12DB65C, FLAGS_HandA = game::unk_12DB664, FLAGS_HandB = game::unk_12DB67C, FLAGS_Fist = game::unk_12DB6AC;
	static constexpr game::Id FLAGS_Impact = game::unk_12DB6D8, FLAGS_Billboard = game::unk_12DB6E0, FLAGS_Target = game::unk_12DB720;
	static constexpr game::Id CAMERA_Blob = game::MAG_112_METEOR_BARRET_CameraBlob;
	static const int16_t *SPARK_Dirs() { return (const int16_t *)game_addr(game::MAG_112_METEOR_BARRET_SparkDirs); }
	static const void *SOUND_Start() { return (const void *)game_addr(game::unk_12DB654); }
	static const void *SOUND_Hit() { return (const void *)game_addr(game::unk_12DB658); }

	enum : int { SITE_GROUND = 0xB4, SITE_HAND_A = 0x174, SITE_HAND_B = 0x284, SITE_FIST = 0x5BC, SITE_TARGET = 0x90C,
		SITE_BILLBOARD = 0xA18, SITE_IMPACT_A = 0xD98, SITE_IMPACT_B = 0xE78 };

	inline int32_t ComputeCos(int32_t a) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(a); }
	inline int32_t CartesianToGameAngle(int32_t x, int32_t z) { return fn<int32_t (__cdecl *)(int32_t, int32_t)>(game_addr(game::CartesianToGameAngle))(x, z); }
	inline int32_t SqrtInt(int32_t v) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::Math_SqrtInt))(v); }
	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t flag, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, flag, out); }
	inline uint32_t BindExternalAnimSet(uint8_t *entity, void *out, uint32_t data) { return fn<uint32_t (__cdecl *)(uint8_t *, void *, uint32_t)>(game_addr(game::BattleEntity_BindExternalAnimSet))(entity, out, data); }
	inline uint32_t QueueChainTransformation(uint8_t *entity, uint32_t id) { return fn<uint32_t (__cdecl *)(uint8_t *, uint32_t)>(game_addr(game::QueueChainTransformation))(entity, id); }
	inline void PlayCameraBlob(uint32_t blob) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::Battle_PlayCameraAnimation))(blob); }
	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline void MorphVertices(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_MorphVertices))(model, f0, f1, t, out); }
	inline void BlendColour(const void *a, const void *b, int32_t wa, int32_t wb, void *out) { fn<void (__cdecl *)(const void *, const void *, int32_t, int32_t, void *)>(game_addr(game::ColourLerp))(a, b, wa, wb, out); }
	inline void InsertPrimAltViewport(uint32_t bucket, void *packet) { fn<void (__cdecl *)(uint32_t, void *)>(game_addr(game::SSIGPU_InsertPrimAltViewport))(bucket, packet); }
	inline void GteMVMVA_RotV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0))(); }
	inline void ScreenFadeTask(int32_t a, int32_t b, int32_t c, int32_t d) { fn<void (__cdecl *)(int32_t, int32_t, int32_t, int32_t)>(game_addr(game::Effect_SpawnScreenFadeTask))(a, b, c, d); }

#pragma pack(push, 1)
	struct RootNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t action;
		uint8_t started;
		uint32_t arena;
		uint32_t pad14[2];
	};
	struct DirectorNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t action;
		uint8_t bind[0x20];
		uint32_t colour_a;
		uint32_t colour_b;
		uint32_t pad38;
		uint32_t saved_pos[2];
		int16_t land[4];
		int16_t ground[4];
		int16_t fist[4];
		int16_t anchor[4];
		int16_t impact[4];
		Mat4x3 fist_m;
		Mat4x3 impact_m;
		int32_t lift;
		uint8_t *target;
		uint8_t ground_l[0xC0];
		uint8_t hand_a[0x110];
		uint8_t hand_b[0x338];
		uint8_t fist_l[0x350];
		uint8_t target_l[0x10C];
		uint8_t billboard[0x380];
		uint8_t impact_a[0xE0];
		uint8_t impact_b[0xD0];
	};
	struct PlayArg
	{
		int16_t pos[3];
		int16_t radial;
		const Mat4x3 *m;
		const uint32_t *flags;
		uint32_t morph;
	};
	struct DirLocals
	{
		int16_t delta[4];
		int16_t spawn[4];
		PlayArg arg;
		Mat4x3 m;
	};
	struct SmokeNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t end;
		int16_t pos[3];
		int16_t size;
		uint32_t seq;
	};
	struct SparkNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t end;
		uint32_t seq;
		int16_t pos[3];
		int16_t size;
		int16_t vel[3];
		int16_t depth;
	};
	struct DebrisNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		int16_t pos[3];
		int16_t pad16;
		int16_t vel[3];
		int16_t pad1E;
		int16_t rot[3];
		int16_t pad26;
		int16_t spin[3];
		int16_t pad2E;
	};
	struct TexRestoreNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t done;
		uint8_t pad[0x20];
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x1C && sizeof(DirectorNode) == 0xF48, "Meteor Barret nodes");
	static_assert(sizeof(PlayArg) == 0x14 && sizeof(DirLocals) == 0x44, "Meteor Barret blocks");
	static_assert(sizeof(SmokeNode) == 0x1C && sizeof(SparkNode) == 0x24 && sizeof(DebrisNode) == 0x30 && sizeof(TexRestoreNode) == 0x30, "Meteor Barret particle nodes");
}
}

namespace ff8fx
{
namespace meteor112
{
	using namespace eng;
	using namespace magc;

	inline int16_t &W16(void *p, int off) { return *(int16_t *)((uint8_t *)p + off); }
	inline uint16_t &U16(void *p, int off) { return *(uint16_t *)((uint8_t *)p + off); }
	inline int32_t &W32(void *p, int off) { return *(int32_t *)((uint8_t *)p + off); }
	inline uint32_t &U32(void *p, int off) { return *(uint32_t *)((uint8_t *)p + off); }
	inline uint8_t &B8(void *p, int off) { return *((uint8_t *)p + off); }

	inline uint8_t *ActionTargets(int i) { return Ctx()->actions[i].targets; }

	static void YawMatrix(int32_t angle, Mat4x3 *m)
	{
		const int32_t a = (int16_t)angle;
		const int32_t s = ComputeSin(a);
		const int32_t c = ComputeCos(a);
		m->m[0][2] = (int16_t)s;
		m->m[2][0] = (int16_t)-s;
		m->m[0][0] = (int16_t)c;
		m->m[0][1] = 0;
		m->m[1][0] = 0;
		m->m[1][1] = 0x1000;
		m->m[1][2] = 0;
		m->m[2][1] = 0;
		m->m[2][2] = (int16_t)c;
	}

	static uint32_t ScreenTile(int32_t r, int32_t g, int32_t b, int32_t n, int32_t d, uint32_t cursor)
	{
		const int32_t k = d - n;
		if (k <= 0) return cursor;
		uint8_t *p = (uint8_t *)cursor;
		W16(p, 8) = 0;
		W16(p, 0x10) = 0;
		W16(p, 0xE) = 0;
		W16(p, 0xA) = 0;
		W16(p, 0x14) = 0x140;
		W16(p, 0x16) = 0xD8;
		W16(p, 0x12) = 0xD8;
		W16(p, 0xC) = 0x140;
		U32(p, 0) = 0x05000000;
		p[7] = 0x2A;
		p[4] = (uint8_t)(mul32(r, k) / d);
		p[5] = (uint8_t)(mul32(g, k) / d);
		p[6] = (uint8_t)(mul32(b, k) / d);
		InsertPrimAltViewport(OTBase() + 0x20, p);
		return cursor + 0x18;
	}

	static void ColumnBend(uint32_t model, int16_t *out, int32_t len)
	{
		int32_t count = *(const int32_t *)(model + 4);
		const uint8_t *v = (const uint8_t *)(model + 8);
		const int32_t l = (int16_t)len;
		do
		{
			GteLoadV0(v);
			GteMVMVA_RotV0Tr();
			GteStoreIR123(out);
			const int32_t a = shl32((int32_t)ColumnTop() - (int32_t)out[1], 10) / (int32_t)ColumnSpan() + (int32_t)0xFFFFFC00;
			const int32_t s = ComputeSin(ComputeSin(a) >> 2) + 0x1000;
			out[2] = (int16_t)(out[2] - (int16_t)(mul32(s, l) >> 12));
			v += 8;
			out += 4;
		} while (--count != 0);
	}

	static void HandsMidpoint(uint8_t *e, int16_t *out)
	{
		int16_t a[4];
		GetEffectSpawnPosition(e, 0x15, 0, a);
		GetEffectSpawnPosition(e, 0x16, 0, out);
		out[0] = (int16_t)(((int32_t)out[0] + (int32_t)a[0]) >> 1);
		out[1] = (int16_t)(((int32_t)out[1] + (int32_t)a[1]) >> 1);
		out[2] = (int16_t)(((int32_t)out[2] + (int32_t)a[2]) >> 1);
	}

	static void __cdecl PartCallback(prim::Layout *l, prim::Record *r, int arg_)
	{
		const PlayArg *arg = (const PlayArg *)arg_;
		const int32_t idx = (int16_t)r->index;
		if (arg->flags[idx] & 0x80000000u) return;
		if (((int32_t)r->scale[2] | *(const int32_t *)r->scale) == 0) return;
		if (r->a >= 0x1000 && *(const uint32_t *)r->rgb == 0) return;
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		const int32_t object = (int16_t)r->flags_lo;
		uint8_t *model = l->data + *(const int32_t *)(l->data + object * 4 + 8);
		*(uint8_t **)h = model;
		const int16_t f0 = r->b0, f1 = r->b1;
		if (f0 == f1)
		{
			if (f0 == 0) *(uint8_t **)(h + 4) = model + 0xC;
			else *(uint8_t **)(h + 4) = model + (uint32_t)mul32(*(const int32_t *)(model + 4), f0) * 8 + 0xC;
		}
		else
		{
			const int16_t t = r->b;
			if (t == 0)
			{
				if (f0 == 0) *(uint8_t **)(h + 4) = model + 0xC;
				else *(uint8_t **)(h + 4) = model + (uint32_t)mul32(*(const int32_t *)(model + 4), f0) * 8 + 0xC;
			}
			else if (t == 0x1000)
			{
				if (f1 == 0) *(uint8_t **)(h + 4) = model + 0xC;
				else *(uint8_t **)(h + 4) = model + (uint32_t)mul32(*(const int32_t *)(model + 4), f1) * 8 + 0xC;
			}
			else
			{
				MorphVertices((uint32_t)model, f0, f1, t, arg->morph);
				*(uint32_t *)(h + 4) = arg->morph;
			}
		}
		Mat4x3 m;
		int32_t n[3];
		ComposeZYXRotationMatrix(r->rot, &m);
		if (arg->flags[idx] & 0x20000000u)
		{
			m.t[0] = r->pos[0];
			m.t[1] = r->pos[1];
			m.t[2] = r->pos[2];
			ComposeAffineTransform(arg->m, &m, &m);
		}
		else
		{
			m.t[0] = (int32_t)r->pos[0] + (int32_t)arg->pos[0];
			m.t[1] = (int32_t)r->pos[1] + (int32_t)arg->pos[1];
			m.t[2] = (int32_t)r->pos[2] + (int32_t)arg->pos[2];
			if (arg->flags[idx] & 0x10000000u)
			{
				TransformVectorBy3x3Matrix(&Camera(), m.t, m.t);
				m.t[0] = (int32_t)((uint32_t)m.t[0] + (uint32_t)Camera().t[0]);
				m.t[1] = (int32_t)((uint32_t)m.t[1] + (uint32_t)Camera().t[1]);
				m.t[2] = (int32_t)((uint32_t)m.t[2] + (uint32_t)Camera().t[2]);
			}
			else ComposeAffineTransform(&Camera(), &m, &m);
		}
		if (arg->flags[idx] & 0x40000000u)
		{
			NormalizeVector(m.t, n);
			const int32_t rad = arg->radial;
			m.t[0] = m.t[0] - (mul32(rad, n[0]) >> 13);
			m.t[1] = m.t[1] - (mul32(rad, n[1]) >> 13);
			m.t[2] = m.t[2] - (mul32(rad, n[2]) >> 13);
		}
		if (!(*(const uint32_t *)r->scale == 0x10001000 && r->scale[2] == 0x1000))
		{
			n[0] = r->scale[0];
			n[1] = r->scale[1];
			n[2] = r->scale[2];
			Scale3DMatrix(&m, n);
		}
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		*(uint32_t *)(h + 0x1C) = (arg->flags[idx] & 0xDFFF) | 0x2000;
		*(int32_t *)(h + 0xC) = r->a;
		if (r->a != 0)
		{
			B8(h, 0x1C) |= 0xC0;
			*(uint32_t *)(h + 8) = *(const uint32_t *)r->rgb;
		}
		PacketCursor() = RenderPrimModel(h, OTBase() + 0x44, 2, PacketCursor());
		FieldFree(0x58);
	}

	static void SmokeDraw(const SmokeNode *s)
	{
		TransformCameraByShadowRotation(s->pos, s->size, -((int32_t)s->size >> 2));
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		*(uint32_t *)h = s->seq;
		W16(h, 4) = s->counter;
		W16(h, 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, OTBase() + 0x44, 2, PacketCursor());
		FieldFree(0xB4);
	}

	static uint32_t __cdecl SmokeTask(TaskNode *n)
	{
		SmokeNode *s = (SmokeNode *)n;
		SmokeDraw(s);
		s->counter++;
		return s->counter < s->end ? 0 : TASK_END;
	}

	static void SparkDraw(const SparkNode *s)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		TransformCameraByShadowRotation(s->pos, s->size, -((int32_t)s->depth >> 2));
		*(uint32_t *)h = s->seq;
		W16(h, 4) = s->counter;
		W16(h, 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, OTBase() + 0x44, 2, PacketCursor());
		FieldFree(0xB4);
	}

	static void SparkMove(SparkNode *s)
	{
		s->pos[0] = (int16_t)(s->pos[0] + s->vel[0]);
		s->pos[1] = (int16_t)(s->pos[1] + s->vel[1]);
		s->pos[2] = (int16_t)(s->pos[2] + s->vel[2]);
	}

	static uint32_t __cdecl SparkTask(TaskNode *n)
	{
		SparkNode *s = (SparkNode *)n;
		SparkDraw(s);
		SparkMove(s);
		s->counter++;
		return s->counter < s->end ? 0 : TASK_END;
	}

	static void DebrisDraw(const DebrisNode *d)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		Mat4x3 m;
		BuildRotationMatrixFromAngles(d->rot, &m);
		m.t[0] = d->pos[0];
		m.t[1] = d->pos[1];
		m.t[2] = d->pos[2];
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		*(uint32_t *)h = game_addr(MODEL_Debris);
		*(uint32_t *)(h + 0x1C) = 0;
		PacketCursor() = RenderPrimModel(h, OTBase() + 0x44, 2, PacketCursor());
		FieldFree(0x58);
	}

	static void DebrisMove(DebrisNode *d)
	{
		d->rot[0] = (int16_t)(d->rot[0] + d->spin[0]);
		d->rot[2] = (int16_t)(d->rot[2] + d->spin[2]);
		d->vel[1] = (int16_t)(d->vel[1] + 0xA);
		d->rot[1] = (int16_t)(d->rot[1] + d->spin[1]);
		d->pos[0] = (int16_t)(d->pos[0] + d->vel[0]);
		d->pos[1] = (int16_t)(d->pos[1] + d->vel[1]);
		d->pos[2] = (int16_t)(d->pos[2] + d->vel[2]);
		if (d->pos[1] > 0)
		{
			const int16_t v = d->vel[1];
			d->pos[1] = 0;
			d->vel[1] = (int16_t)((int16_t)(v >> 2) - v);
		}
	}

	static uint32_t __cdecl DebrisTask(TaskNode *n)
	{
		DebrisNode *d = (DebrisNode *)n;
		DebrisDraw(d);
		DebrisMove(d);
		d->counter++;
		return d->counter < 0x1E ? 0 : TASK_END;
	}

	static uint32_t __cdecl TexRestoreTask(TaskNode *n)
	{
		TexRestoreNode *w = (TexRestoreNode *)n;
		if (w->counter == 0) TextureRestoreTask((void *)(TexBase() + 0x1E88), &w->done);
		const int16_t done = w->done;
		w->counter++;
		return done ? TASK_END : 0;
	}

	static void PlayLayout(DirectorNode *d, int site, PlayArg *arg)
	{
		prim::play((prim::Layout *)((uint8_t *)d + site), PartCallback, (int)arg, 0);
	}

	static const int16_t PAD_DEBRIS = 0;

	static void GroundPhase(DirectorNode *d, DirLocals &L, int32_t k)
	{
		uint8_t *caster = Caster();
		GetEffectSpawnPosition(caster, 0xF1, 0, L.spawn);
		if ((uint32_t)k < 2)
		{
			int32_t angle = CrtRand();
			for (int32_t i = 8; i != 0; i--, angle += 0x200)
			{
				SparkNode *s = (SparkNode *)AddTaskToQueue(QSparks(), game_addr(ORIG_SparkTask));
				if (!s) continue;
				Memset32(&s->counter, 0, 6);
				s->seq = game_addr(SEQ_Smoke);
				s->end = 0xC;
				s->depth = 0x1000;
				s->size = 0x1000;
				s->pos[0] = W16(caster, 0x1C);
				s->pos[1] = W16(caster, 0x24);
				s->pos[2] = W16(caster, 0x20);
				s->vel[0] = (int16_t)(ComputeCos(angle) >> 3);
				s->vel[2] = (int16_t)(ComputeSin(angle) >> 3);
				const int32_t r = CrtRand() & 0xFFF;
				const int32_t f = r + (0x400 - (r >> 2));
				s->vel[0] = (int16_t)(mul32(s->vel[0], f) >> 14);
				s->vel[2] = (int16_t)(mul32(s->vel[2], f) >> 14);
			}
		}
		if (k == 0)
		{
			for (int32_t i = 0xC; i != 0; i--)
			{
				SmokeNode *s = (SmokeNode *)AddTaskToQueue(QSmoke(), game_addr(ORIG_SmokeTask));
				if (!s) continue;
				Memset32(&s->counter, 0, 4);
				ff8fx::copy<(8)>(s->pos, L.spawn);
				s->pos[0] = (int16_t)(s->pos[0] + (int16_t)((CrtRand() & 0x1FF) - 0x100));
				s->pos[2] = (int16_t)(s->pos[2] + (int16_t)((CrtRand() & 0x1FF) - 0x100));
				const int32_t r = CrtRand() & 0x7FF;
				s->seq = game_addr(SEQ_Smoke);
				s->end = 0xC;
				s->size = (int16_t)(r + 0x1000);
			}
			return;
		}
		L.delta[0] = (int16_t)(L.spawn[0] - d->ground[0]);
		L.delta[1] = (int16_t)(L.spawn[1] - d->ground[1]);
		L.delta[2] = (int16_t)(L.spawn[2] - d->ground[2]);
		YawMatrix(k << 6, &L.m);
		GteSetRotMatrix(&L.m);
		L.m.t[2] = 0;
		L.m.t[1] = 0;
		L.m.t[0] = 0;
		GteSetTransVector(&L.m);
		int32_t len = SqrtInt(mul32(L.delta[0], L.delta[0]) + mul32(L.delta[2], L.delta[2]));
		if (L.delta[2] > 0) len = -len;
		ColumnBend(game_addr(MODEL_Column), (int16_t *)(TexBase() + 0x1808), len);
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		YawMatrix(W16(caster, 0xE), &L.m);
		L.m.m[1][1] = (int16_t)shl32(-*(const int32_t *)&L.delta[1], 2);
		L.m.t[0] = d->ground[0];
		L.m.t[1] = W16(caster, 0x24);
		L.m.t[2] = d->ground[2];
		ComposeAffineTransform(&Camera(), &L.m, &L.m);
		GteSetRotMatrix(&L.m);
		GteSetTransVector(&L.m);
		*(uint32_t *)h = game_addr(MODEL_Column);
		*(uint32_t *)(h + 4) = TexBase() + 0x1808;
		if ((uint32_t)k >= 7)
		{
			B8(h, 0xA) = 0;
			B8(h, 9) = 0;
			B8(h, 8) = 0;
			*(uint32_t *)(h + 0xC) = (uint32_t)(shl32(k, 9) - 0xC01) & 0x1FFFFE00u;
			*(uint32_t *)(h + 0x1C) = 0x20F3;
		}
		else *(uint32_t *)(h + 0x1C) = 0x2033;
		PacketCursor() = RenderPrimModel(h, OTBase() + 0x44, 2, PacketCursor());
		FieldFree(0x58);
		if ((uint32_t)k < 2 || (uint32_t)k > 7) return;
		L.delta[3] = PAD_DEBRIS;
		const uint32_t dxy = *(const uint32_t *)&L.delta[0], dz = *(const uint32_t *)&L.delta[2];
		for (int32_t i = 2; i != 0; i--)
		{
			DebrisNode *p = (DebrisNode *)AddTaskToQueue(QDebris(), game_addr(ORIG_DebrisTask));
			if (!p) continue;
			Memset32(&p->counter, 0, 9);
			*(uint32_t *)&p->pos[0] = dxy;
			*(uint32_t *)&p->pos[2] = dz;
			p->vel[0] = (int16_t)((CrtRand() & 0x1F) + 0x20);
			if (CrtRand() & 1) p->vel[0] = (int16_t)-p->vel[0];
			p->vel[2] = (int16_t)((CrtRand() & 0x1F) + 0x20);
			if (CrtRand() & 1) p->vel[2] = (int16_t)-p->vel[2];
			p->rot[0] = (int16_t)CrtRand();
			p->rot[1] = (int16_t)CrtRand();
			p->rot[2] = (int16_t)CrtRand();
			p->spin[0] = (int16_t)((CrtRand() & 0x7F) - 0x40);
			p->spin[1] = (int16_t)((CrtRand() & 0x7F) - 0x40);
			p->spin[2] = (int16_t)((CrtRand() & 0x7F) - 0x40);
		}
	}

	static void ImpactPhase(DirectorNode *d, DirLocals &L, int32_t e)
	{
		uint8_t *caster = Caster();
		if (e == 0x26)
		{
			uint8_t *p = (uint8_t *)PacketCursor();
			PacketCursor() += 0x18;
			W16(p, 0x14) = 0x140;
			W16(p, 0xC) = 0x140;
			W16(p, 8) = 0;
			W16(p, 0x10) = 0;
			W16(p, 0xE) = 0;
			W16(p, 0xA) = 0;
			W16(p, 0x16) = 0xD8;
			W16(p, 0x12) = 0xD8;
			p[6] = 0x80;
			p[5] = 0x80;
			p[4] = 0x80;
			uint32_t *q = (uint32_t *)PacketCursor();
			PacketCursor() += 8;
			q[0] = 0x01000000;
			q[1] = 0xE1000220;
			InsertPrimAutoDepth(OTBase() + 0x20, q);
			U32(p, 0) = 0x05000000;
			p[7] = 0x2A;
			InsertPrimAltViewport(OTBase() + 0x20, p);
			q = (uint32_t *)PacketCursor();
			PacketCursor() += 8;
			q[0] = 0x01000000;
			q[1] = 0xE1000240;
			InsertPrimAutoDepth(OTBase() + 0x20, q);
		}
		else if (e == 0x27) ScreenFadeTask(0, 1, 1, 0xC0);
		if ((uint32_t)e >= 0x10) PacketCursor() = ScreenTile(0x28, 0x37, 0x32, e - 0x10, 3, PacketCursor());
		if ((uint32_t)e >= 0x20) PacketCursor() = ScreenTile(0x28, 0x37, 0x32, e - 0x20, 3, PacketCursor());
		if (e == 0)
		{
			DecodeModelPrimLayout(game_addr(LAYOUT_Target), d->target_l, 0x10C);
			DecodeModelPrimLayout(game_addr(LAYOUT_Billboard), d->billboard, 0x380);
			DecodeModelPrimLayout(game_addr(LAYOUT_ImpactA), d->impact_a, 0xE0);
			DecodeModelPrimLayout(game_addr(LAYOUT_ImpactB), d->impact_b, 0xD0);
			int32_t f = W16(caster, 0xE);
			if (f > 0x800) f -= 0x1000;
			W16(caster, 0xE) = (int16_t)(f >> 1);
			YawMatrix(W16(caster, 0xE), &d->impact_m);
			GetDefaultEffectPosition(d->target, d->anchor);
			U32(caster, 0x1C) = *(const uint32_t *)&d->anchor[0];
			U32(caster, 0x20) = *(const uint32_t *)&d->anchor[2];
			W16(caster, 0x1C) = (int16_t)(W16(caster, 0x1C) - (int16_t)(mul32(ComputeSin(W16(caster, 0xE)), (int32_t)W16(d->target, 0x26) + 0x1388) >> 14));
			W16(caster, 0x20) = (int16_t)(W16(caster, 0x20) - (int16_t)(mul32(ComputeCos(W16(caster, 0xE)), (int32_t)W16(d->target, 0x26) + 0x1388) >> 14));
			W16(caster, 0x1E) = 0;
			d->impact[0] = (int16_t)(W16(caster, 0x1C) - (int16_t)(ComputeSin(W16(caster, 0xE)) >> 2));
			const int32_t c = ComputeCos(W16(caster, 0xE));
			d->impact[1] = 0;
			d->impact[2] = (int16_t)(W16(caster, 0x20) - (int16_t)(c >> 2));
		}
		d->impact_m.t[0] = d->impact[0];
		d->impact_m.t[1] = d->impact[1];
		d->impact_m.t[2] = d->impact[2];
		ComposeAffineTransform(&Camera(), &d->impact_m, &L.m);
		L.arg.m = &L.m;
		L.arg.flags = (const uint32_t *)game_addr(FLAGS_Impact);
		L.arg.morph = TexBase() + 0x1808;
		PlayLayout(d, SITE_IMPACT_A, &L.arg);
		if ((uint32_t)e >= 0x29) PlayLayout(d, SITE_IMPACT_B, &L.arg);
		GteSetRotMatrixCtrl(&Camera());
		L.m.m[1][1] = 0x1000;
		L.m.m[0][0] = 0x1000;
		L.m.m[2][1] = 0;
		L.m.m[2][0] = 0;
		L.m.m[1][2] = 0;
		L.m.m[1][0] = 0;
		L.m.m[0][2] = 0;
		L.m.m[0][1] = 0;
		GteLoadV0(d->anchor);
		GteMVMVA_RotV0();
		GteReadMAC123(L.m.t);
		L.m.t[0] = (int32_t)((uint32_t)L.m.t[0] + (uint32_t)Camera().t[0]);
		L.m.t[1] = (int32_t)((uint32_t)L.m.t[1] + (uint32_t)Camera().t[1]);
		L.m.t[2] = (int32_t)((uint32_t)L.m.t[2] + (uint32_t)Camera().t[2]);
		L.arg.radial = (int16_t)(d->lift + 0xC8);
		L.arg.m = &L.m;
		L.arg.flags = (const uint32_t *)game_addr(FLAGS_Billboard);
		L.arg.morph = TexBase() + 0x1808;
		PlayLayout(d, SITE_BILLBOARD, &L.arg);
		if (e == 2)
		{
			for (*(int32_t *)&L.spawn[0] = 0x10; *(int32_t *)&L.spawn[0] != 0; (*(int32_t *)&L.spawn[0])--)
			{
				DebrisNode *p = (DebrisNode *)AddTaskToQueue(QDebris(), game_addr(ORIG_DebrisTask));
				if (!p) continue;
				int32_t spread = CrtRand();
				spread -= CrtRand();
				spread -= CrtRand();
				spread += CrtRand();
				Memset32(&p->counter, 0, 9);
				spread >>= 1;
				*(int32_t *)&L.delta[0] = spread;
				if (spread < 0) *(int32_t *)&L.delta[0] = -spread;
				const int32_t speed = 0x9C40 - *(const int32_t *)&L.delta[0];
				const int32_t a = spread + W16(caster, 0xE);
				const int32_t s = -ComputeSin(a);
				p->vel[0] = (int16_t)s;
				p->pos[0] = (int16_t)s;
				const int32_t c = -ComputeCos(a);
				p->vel[2] = (int16_t)c;
				p->pos[2] = (int16_t)c;
				p->pos[0] = (int16_t)((int16_t)(p->pos[0] >> 4) + d->impact[0]);
				p->pos[2] = (int16_t)((int16_t)((int16_t)c >> 4) + d->impact[2]);
				p->pos[1] = 0;
				p->vel[0] = (int16_t)(mul32(p->vel[0], speed) >> 0x14);
				p->vel[2] = (int16_t)(mul32((int16_t)c, speed) >> 0x14);
				p->vel[1] = (int16_t)(-mul32(CrtRand(), speed >> 2) >> 0x14);
				p->rot[0] = (int16_t)CrtRand();
				p->rot[1] = (int16_t)CrtRand();
				p->rot[2] = (int16_t)CrtRand();
				p->spin[0] = (int16_t)((CrtRand() & 0x7F) - 0x40);
				p->spin[1] = (int16_t)((CrtRand() & 0x7F) - 0x40);
				p->spin[2] = (int16_t)((CrtRand() & 0x7F) - 0x40);
			}
		}
		else
		{
			if ((uint32_t)e >= 0x26)
			{
				d->impact_m.t[0] = d->anchor[0];
				d->impact_m.t[1] = d->anchor[1];
				d->impact_m.t[2] = d->anchor[2];
				ComposeAffineTransform(&Camera(), &d->impact_m, &L.m);
				L.arg.radial = (int16_t)(d->lift + 0xC8);
				L.arg.m = &L.m;
				L.arg.flags = (const uint32_t *)game_addr(FLAGS_Target);
				L.arg.morph = TexBase() + 0x1808;
				PlayLayout(d, SITE_TARGET, &L.arg);
			}
			if ((uint32_t)e >= 0x29 && (uint32_t)e < 0x2D)
			{
				L.spawn[0] = (int16_t)CrtRand();
				L.spawn[1] = (int16_t)CrtRand();
				L.spawn[2] = (int16_t)CrtRand();
				BuildRotationMatrixFromAngles(L.spawn, &L.m);
				L.m.t[2] = 0;
				L.m.t[1] = 0;
				L.m.t[0] = 0;
				GteSetRotMatrix(&L.m);
				GteSetTransVector(&L.m);
				for (*(uint32_t *)&L.delta[0] = (uint32_t)(SPARK_Dirs() + 12); ; )
				{
					SparkNode *s = (SparkNode *)AddTaskToQueue(QSparks(), game_addr(ORIG_SparkTask));
					if (s)
					{
						Memset32(&s->counter, 0, 6);
						s->seq = game_addr(SEQ_Spark);
						s->end = 0xF;
						GteLoadV0((const void *)*(const uint32_t *)&L.delta[0]);
						GteMVMVA_RotV0Tr();
						GteStoreIR123(s->pos);
						const int16_t ir[3] = { s->pos[0], s->pos[1], s->pos[2] };
						*(uint32_t *)&s->vel[0] = *(const uint32_t *)&s->pos[0];
						*(uint32_t *)&s->vel[2] = *(const uint32_t *)&s->pos[2];
						s->pos[0] = (int16_t)((int16_t)(ir[0] >> 3) + d->anchor[0]);
						s->pos[1] = (int16_t)((int16_t)(ir[1] >> 3) + d->anchor[1]);
						s->pos[2] = (int16_t)((int16_t)(ir[2] >> 3) + d->anchor[2]);
						s->vel[0] = (int16_t)(s->vel[0] >> 7);
						s->vel[1] = (int16_t)(s->vel[1] >> 7);
						s->vel[2] = (int16_t)(s->vel[2] >> 7);
						s->size = 0x1000;
						s->depth = (int16_t)(d->lift + 0x1000);
					}
					const uint32_t at = *(const uint32_t *)&L.delta[0];
					*(uint32_t *)&L.delta[0] = at - 8;
					if (at == (uint32_t)SPARK_Dirs()) break;
				}
			}
		}
		if (W16(d->target, 0x26) >= 0xC00)
		{
			if (e != 0x2D) return;
		}
		else if (e != 0x32) return;
		U16(d->target, 0) &= 0xFFF7;
		ApplyActionResultToTarget(ActionTargets(d->action));
	}

	static uint32_t __cdecl DirectorTask(TaskNode *n)
	{
		DirectorNode *d = (DirectorNode *)n;
		uint8_t *caster = Caster();
		DirLocals L;
		if (d->counter == 2) BdPlaySE(SOUND_Start(), 0, 0x80);
		if (d->counter == 0x49) BdPlaySE(SOUND_Hit(), 0, 0x80);
		int32_t flash = (int32_t)0xFFFFD8F1;
		const int16_t c0 = d->counter;
		if (c0 < 4) { const int32_t v = shl32(c0, 8); if (v > flash) flash = v; }
		else if (c0 > 0x94) { const int32_t v = shl32(0x98 - c0, 8); if (v > flash) flash = v; }
		else flash = 0x400;
		SetScreenFlash((uint32_t)flash, 0);
		if (d->counter == 7)
		{
			W16(caster, 0x1C) = (int16_t)(W16(caster, 0x1C) - (int16_t)(mul32(ComputeSin(W16(caster, 0xE)), 0x3B6) >> 12));
			W16(caster, 0x20) = (int16_t)(W16(caster, 0x20) - (int16_t)(mul32(ComputeCos(W16(caster, 0xE)), 0x3B6) >> 12));
		}
		const int32_t k = (int32_t)d->counter - 0xA;
		if ((uint32_t)k <= 0xF)
		{
			if (k == 0)
			{
				DecodeModelPrimLayout(game_addr(LAYOUT_Ground), d->ground_l, 0xC0);
				GetEffectSpawnPosition(caster, 0xF1, 0, d->ground);
			}
			else
			{
				L.arg.pos[0] = d->ground[0];
				L.arg.pos[1] = W16(caster, 0x24);
				L.arg.pos[2] = d->ground[2];
				L.arg.flags = (const uint32_t *)game_addr(FLAGS_Ground);
				L.arg.morph = TexBase() + 0x1808;
				PlayLayout(d, SITE_GROUND, &L.arg);
				if ((uint32_t)(k - 1) <= 6)
				{
					const int32_t step = d->lift / 400;
					W16(caster, 0x1C) = (int16_t)(W16(caster, 0x1C) + (int16_t)(mul32(ComputeSin(W16(caster, 0xE)), step) / 160));
					W16(caster, 0x20) = (int16_t)(W16(caster, 0x20) + (int16_t)(mul32(ComputeCos(W16(caster, 0xE)), step) / 160));
				}
			}
			GroundPhase(d, L, k);
		}
		const int32_t h = (int32_t)d->counter - 0x11;
		if ((uint32_t)h <= 0x23)
		{
			if (h == 0)
			{
				DecodeModelPrimLayout(game_addr(LAYOUT_HandA), d->hand_a, 0x110);
				DecodeModelPrimLayout(game_addr(LAYOUT_HandB), d->hand_b, 0x338);
			}
			HandsMidpoint(caster, L.arg.pos);
			L.arg.radial = (uint32_t)h < 0x16 ? 0 : (int16_t)-0x400;
			L.arg.flags = (const uint32_t *)game_addr(FLAGS_HandA);
			L.arg.morph = TexBase() + 0x1808;
			PlayLayout(d, SITE_HAND_A, &L.arg);
			if ((uint32_t)h > 4)
			{
				L.arg.flags = (const uint32_t *)game_addr(FLAGS_HandB);
				L.arg.morph = TexBase() + 0x1808;
				PlayLayout(d, SITE_HAND_B, &L.arg);
			}
			if ((uint32_t)h >= 8) PacketCursor() = ScreenTile(0x32, 0x50, 0x50, h - 8, 3, PacketCursor());
			if (h == 0x15) ScreenFadeTask(0, 1, 1, 0x60);
			else if ((uint32_t)h >= 0x19) PacketCursor() = ScreenTile(0x28, 0x3C, 0x3C, h - 0x19, 5, PacketCursor());
		}
		if (d->counter == 0x22) ScreenFadeTask(0, 1, 1, 0x60);
		const int32_t f = (int32_t)d->counter - 0x30;
		if ((uint32_t)f < 0x32)
		{
			if (f == 0)
			{
				DecodeModelPrimLayout(game_addr(LAYOUT_Fist), d->fist_l, 0x350);
				int16_t *ang = &L.arg.pos[2];
				ang[0] = 0x12C;
				ang[1] = W16(caster, 0xE);
				ang[2] = (int16_t)f;
				ComposeZYXRotationMatrix(ang, &d->fist_m);
			}
			GetEffectSpawnPosition(caster, 0x15, 0, d->fist);
			d->fist_m.t[0] = d->fist[0];
			d->fist_m.t[1] = d->fist[1];
			d->fist_m.t[2] = d->fist[2];
			ComposeAffineTransform(&Camera(), &d->fist_m, &L.m);
			L.arg.m = &L.m;
			L.arg.flags = (const uint32_t *)game_addr(FLAGS_Fist);
			L.arg.morph = TexBase() + 0x1808;
			PlayLayout(d, SITE_FIST, &L.arg);
			if ((uint32_t)f >= 4)
			{
				PacketCursor() = ScreenTile(0x50, 0x3C, 0x1E, f - 4, 4, PacketCursor());
				if (f == 4) B8(d->target, 0) |= 8;
			}
		}
		if (d->counter == 0x43) ScreenFadeTask(0, 1, 6, 0xFF);
		const int32_t e = (int32_t)d->counter - 0x45;
		if ((uint32_t)e < 0x41) ImpactPhase(d, L, e);
		const int32_t j = (int32_t)d->counter - 0x2D;
		if ((uint32_t)j <= 4)
		{
			if (j == 0)
			{
				GetDefaultEffectPosition(d->target, d->land);
				const int32_t sz = (int32_t)W16(d->target, 0x26) + 0x1388;
				d->land[0] = (int16_t)(d->land[0] + (int16_t)(mul32(ComputeSin(W16(caster, 0xE)), sz) / 0x3000));
				const int32_t sz2 = (int32_t)W16(d->target, 0x26) + 0x1388;
				d->land[2] = (int16_t)(d->land[2] + (int16_t)(mul32(ComputeCos(W16(caster, 0xE)), sz2) / 0x3000));
				d->land[1] = (int16_t)(d->land[1] - (int16_t)(((int32_t)W16(d->target, 0x26) + 0x2710) >> 5));
			}
			const int32_t up = shl32(4 - j, 0xB);
			W16(caster, 0x1C) = (int16_t)((int16_t)(mul32(ComputeSin(W16(caster, 0xE)), up) >> 12) + d->land[0]);
			W16(caster, 0x20) = (int16_t)((int16_t)(mul32(ComputeCos(W16(caster, 0xE)), up) >> 12) + d->land[2]);
			W16(caster, 0x1E) = (int16_t)(d->land[1] - (int16_t)up);
		}
		const int32_t a = (int32_t)d->counter - 0x89;
		if ((uint32_t)a <= 6)
		{
			const int32_t t = (uint32_t)a < 4 ? (int32_t)(((uint32_t)a & 0xFFFFF) << 10) : 0x1000;
			BlendColour(&d->colour_a, Black(), 0x1000 - t, t, caster + 0x28);
			BlendColour(&d->colour_b, Black(), 0x1000 - t, t, caster + 0x2C);
			B8(caster, 1) |= 8;
			B8(caster, 0x2B) = 2;
			B8(caster, 0x2F) = 0x32;
			B8(caster, 7) = 0;
		}
		const int32_t b = (int32_t)d->counter - 0x91;
		if ((uint32_t)b <= 6)
		{
			const int32_t t = (int32_t)(((uint64_t)((uint32_t)b << 12) * 0xAAAAAAABull >> 32) >> 2);
			BlendColour(Black(), &d->colour_a, 0x1000 - t, t, caster + 0x28);
			BlendColour(Black(), &d->colour_b, 0x1000 - t, t, caster + 0x2C);
			B8(caster, 1) |= 8;
			B8(caster, 0x2B) = 2;
			B8(caster, 0x2F) = 0x32;
			B8(caster, 7) = 0;
		}
		if (d->counter == 0x8F) B8(caster, 0) |= 0xC;
		else if (d->counter == 0x91)
		{
			U32(caster, 0x1C) = d->saved_pos[0];
			U32(caster, 0x20) = d->saved_pos[1];
			U16(caster, 0) &= 0xFFF3;
		}
		else if (d->counter == 0x98)
		{
			U32(caster, 0x28) = d->colour_a;
			B8(caster, 0x2B) = 0;
			U32(caster, 0x2C) = d->colour_b;
			B8(caster, 0x2F) = 0x32;
			B8(caster, 7) = B8(d, 0x37);
			U16(caster, 0) &= 0xF7FF;
		}
		if (d->counter == 0x89)
		{
			TaskNode *w = AddTaskToQueue(QDebris(), game_addr(ORIG_TexRestoreTask));
			if (w) Memset32((uint8_t *)w + 0xC, 0, 9);
		}
		if (d->counter >= 0x98)
		{
			U16(caster, 0) &= 0xEFFF;
			return TASK_END;
		}
		d->counter++;
		return 0;
	}

	static uint32_t __cdecl RootTask(TaskNode *n)
	{
		RootNode *r = (RootNode *)n;
		if (r->arena)
		{
			PacketCursor() = TexBase() + 0x1E88;
			r->arena = 0;
		}
		else
		{
			PacketCursor() = TexBase() + 0x11E88;
			r->arena = 1;
		}
		if (r->counter == 1)
		{
			if (!r->started)
			{
				r->started = 1;
				InitTaskQueuePool(QDirector(), (void *)TexBase(), 0xF48, 1);
				InitTaskQueuePool(QSmoke(), (void *)(TexBase() + 0x1488), 0x1C, 0x20);
				InitTaskQueuePool(QSparks(), (void *)(TexBase() + 0x1248), 0x24, 0x10);
				InitTaskQueuePool(QDebris(), (void *)(TexBase() + 0xF48), 0x30, 0x10);
			}
			if (r->action <= Ctx()->actions[0].last_action)
			{
				DirectorNode *d = (DirectorNode *)AddTaskToQueue(QDirector(), game_addr(ORIG_DirectorTask));
				if (d)
				{
					uint8_t *caster = Caster();
					Memset32(&d->counter, 0, 0x3CF);
					B8(caster, 1) |= 0x10;
					d->action = r->action;
					d->target = Entity(ActionTargets(r->action)[0]);
					BindExternalAnimSet(caster, d->bind, game_addr(COM_Anim));
					QueueChainTransformation(caster, 1);
					W16(caster, 0xE) = (int16_t)CartesianToGameAngle((int32_t)W16(caster, 0x1C) - (int32_t)W16(d->target, 0x1C), (int32_t)W16(caster, 0x20) - (int32_t)W16(d->target, 0x20));
					d->lift = (int32_t)W16(d->target, 0x26) - 0x190;
					if (d->lift < 0) d->lift = 0;
					d->saved_pos[0] = U32(caster, 0x1C);
					d->saved_pos[1] = U32(caster, 0x20);
					W16(caster, 0x1C) = 0;
					W16(caster, 0x20) = 0x3E8;
					d->colour_a = U32(caster, 0x28);
					d->colour_b = U32(caster, 0x2C);
					B8(d, 0x37) = B8(caster, 7);
					r->action++;
					PlayCameraBlob(game_addr(CAMERA_Blob));
				}
			}
		}
		int queues = 1;
		if (r->started)
		{
			const int a = ExecuteTaskQueue(QDirector());
			const int b = ExecuteTaskQueue(QSmoke());
			const int c = ExecuteTaskQueue(QSparks());
			const int dd = ExecuteTaskQueue(QDebris());
			queues = a | b | c | dd;
		}
		if (r->started && queues == 0) return TASK_END;
		r->counter++;
		if (r->counter >= 10) r->counter = 0;
		return 0;
	}
}

	void register_mag112_meteor_barret()
	{
		register_port(meteor112::ORIG_RootTask, (void *)meteor112::RootTask, "112 RootTask", 112);
		register_port(meteor112::ORIG_DirectorTask, (void *)meteor112::DirectorTask, "112 DirectorTask", 112);
		register_port(meteor112::ORIG_SmokeTask, (void *)meteor112::SmokeTask, "112 SmokeTask", 112);
		register_port(meteor112::ORIG_DebrisTask, (void *)meteor112::DebrisTask, "112 DebrisTask", 112);
		register_port(meteor112::ORIG_SparkTask, (void *)meteor112::SparkTask, "112 SparkTask", 112);
		register_port(meteor112::ORIG_TexRestoreTask, (void *)meteor112::TexRestoreTask, "112 TexRestoreTask", 112);
	}
}

