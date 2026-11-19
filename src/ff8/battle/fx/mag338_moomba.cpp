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

// Effect 338: Moomba - Friendship / MoombaMoomba

#include "fx_port.h"

namespace ff8fx
{
namespace m338
{
	using namespace eng;

	inline uint8_t *&CastCtx() { return var<uint8_t *>(game_addr(game::unk_25561CC)); }
	inline uint8_t *&Attacker() { return var<uint8_t *>(game_addr(game::unk_25561D0)); }
	inline uint8_t *&Target() { return var<uint8_t *>(game_addr(game::unk_25561D4)); }
	inline uint8_t *&SparkHint() { return var<uint8_t *>(game_addr(game::g_Moomba_SparkHint)); }
	inline TaskQueue &QueueDebris() { return var<TaskQueue>(game_addr(game::unk_25561E0)); }
	inline TaskQueue &QueuePrims() { return var<TaskQueue>(game_addr(game::unk_25561F0)); }
	inline TaskQueue &QueueCreature() { return var<TaskQueue>(game_addr(game::unk_2556200)); }
	inline TaskQueue &QueueRoot() { return var<TaskQueue>(game_addr(game::unk_2556210)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::unk_2556234)); }
	inline uint32_t &PacketCursor2() { return var<uint32_t>(game_addr(game::g_Moomba_PacketCursorNext)); }
	inline uint32_t &Flag23C() { return var<uint32_t>(game_addr(game::unk_255623C)); }
	inline int16_t &W(int k) { return var<int16_t>(game_addr(game::unk_2556240) + 2 * k); }
	inline uint8_t *&ModelBuffer() { return var<uint8_t *>(game_addr(game::unk_2556250)); }

	static constexpr game::Id ORIG_SequenceTask = game::GF_338Moomba_SequenceTask;
	static constexpr game::Id ORIG_CreatureTask = game::GF_338Moomba_CreatureTask;
	static constexpr game::Id ORIG_PrimTask = game::GF_338Moomba_PrimTask;
	static constexpr game::Id ORIG_DebrisTask = game::GF_338Moomba_DebrisTask;
	static constexpr game::Id CB_PrimObject = game::GF_338Moomba_PrimObjectCallback;
	static constexpr game::Id MODEL_Ring = game::unk_1371640;
	static constexpr game::Id MODEL_Debris = game::unk_1372CE8;
	static constexpr game::Id SEQ_Spark = game::unk_137130C;
	static constexpr game::Id SEQ_SparkFlash = game::unk_1371464;
	static constexpr game::Id SEQ_SparkFly = game::unk_13711FC;

	static const uint32_t MB_SPARKS = 0x0;
	static const uint32_t MB_SPARKS_SIZE = 0x600;
	static const uint32_t MB_SCRATCH = 0x1C90;
	static const uint32_t MB_SCRATCH_SIZE = 0x4000;
	static const uint32_t MB_STREAM_STATE = 0x5C90;
	static const uint32_t MB_ARENA0 = 0x5CD8;

	namespace x
	{
		inline int32_t Rand() { return fn<int32_t (__cdecl *)()>(game_addr(game::_rand))(); }
		inline int32_t Cos(int32_t a) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(a); }
		inline void RotY(int32_t angle, void *out) { fn<void (__cdecl *)(int32_t, void *)>(game_addr(game::Effect_RotationYMatrix))(angle, out); }
		inline void RotX(int32_t angle, void *out) { fn<void (__cdecl *)(int32_t, void *)>(game_addr(game::MAG_063_RotationXMatrix))(angle, out); }
		inline void Blend(const void *a, const void *b, int32_t wa, int32_t wb, void *out) { fn<void (__cdecl *)(const void *, const void *, int32_t, int32_t, void *)>(game_addr(game::VecLerp))(a, b, wa, wb, out); }
		inline void MulMatrix(void *a, const void *b) { fn<void (__cdecl *)(void *, const void *)>(game_addr(game::MatrixMultiply3x3))(a, b); }
		inline uint32_t RenderPrim2(void *h, uint32_t ot, int mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int, uint32_t)>(game_addr(game::MAG_070_RenderPrimModel))(h, ot, mode, cursor); }
		inline uint32_t SpriteDraw(void *h, uint32_t ot, int mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int, uint32_t)>(game_addr(game::MAG_065_RenderFlipbook))(h, ot, mode, cursor); }
		inline void Decode(uint32_t src, void *layout, int size) { fn<void (__cdecl *)(uint32_t, void *, int)>(game_addr(game::Effect_DecodeModelPrimLayout))(src, layout, size); }
		inline void SetAnim(void *e, int id) { fn<void (__cdecl *)(void *, int)>(game_addr(game::au_re_Battle_ReadAnimation_6))(e, id); }
		inline void AdvanceAnim(void *e) { fn<void (__cdecl *)(void *)>(game_addr(game::MAG_089_Angelo_AnimStep))(e); }
		inline uint32_t DrawModel(void *e, void *buf, uint32_t cursor, uint32_t a4) { return fn<uint32_t (__cdecl *)(void *, void *, uint32_t, uint32_t)>(game_addr(game::Effect_DrawCreatureModel))(e, buf, cursor, a4); }
		inline void BindModel(void *e, void *sections, uint32_t data) { fn<void (__cdecl *)(void *, void *, uint32_t)>(game_addr(game::Effect_BindModelContainerSections))(e, sections, data); }
		inline void Heights(void *entity, void *out) { fn<void (__cdecl *)(void *, void *)>(game_addr(game::MAG_070_ShockwavePulsar_MeasureModelHeight))(entity, out); }
		inline void SpawnPosition(void *entity, int bone, int a, void *out) { fn<void (__cdecl *)(void *, int, int, void *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, a, out); }
		inline void ChainTransformation(void *entity, int kind) { fn<void (__cdecl *)(void *, int)>(game_addr(game::QueueChainTransformation))(entity, kind); }
		inline void ApplyResultToTarget(void *targets) { fn<void (__cdecl *)(void *)>(game_addr(game::ApplyActionResultToTarget))(targets); }
		inline void PlaySE(uint32_t se, int a, int b) { fn<void (__cdecl *)(uint32_t, int, int)>(game_addr(game::BdPlaySE))(se, a, b); }
		inline void ReleaseVoice(uint32_t slot) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::BdSound_ReleaseVoiceSlot))(slot); }
		inline void Load(int id, uint32_t dst, int n) { fn<void (__cdecl *)(int, uint32_t, int)>(game_addr(game::Stream_QueueLoad))(id, dst, n); }
		inline void PreLoad() { fn<void (__cdecl *)()>(game_addr(game::Stream_Pump))(); }
		inline int PreLoad0(int a) { return fn<int (__cdecl *)(int)>(game_addr(game::BattleFile_PreLoad_IsLoaded))(a); }
	}

#pragma pack(push, 1)
	struct MasterNode
	{
		TaskNode hdr;
		uint16_t counter;
		uint8_t pad0E;
		uint8_t spawned;
		uint32_t parity;
	};

	struct CreatureNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		uint32_t voice;
		int16_t h0, h1;
		uint8_t *attacker;
		uint8_t pad1C[8];
		uint8_t model[0x9C];
		uint8_t sections[0x10];
	};

	struct PrimNode
	{
		TaskNode hdr;
		int16_t pos[3];
		int16_t yaw;
		uint8_t layout[0x184];
	};

	struct DebrisNode
	{
		TaskNode hdr;
		int16_t pos[3];
		int16_t vel[3];
		int8_t acc[3];
		uint8_t life;
		uint8_t rot[2];
		uint8_t spin[2];
		int16_t size;
		int16_t pad22;
	};

	struct Spark
	{
		uint32_t seq;
		int16_t pos[3];
		int16_t life;
		int16_t vel[3];
		int16_t unused12;
		int8_t acc[3];
		uint8_t pad17;
	};
#pragma pack(pop)
	static_assert(sizeof(MasterNode) == 0x14, "master node is 0x14 bytes");
	static_assert(sizeof(CreatureNode) == 0xD0, "creature node is 0xD0 bytes");
	static_assert(sizeof(PrimNode) == 0x198, "prim node is 0x198 bytes");
	static_assert(sizeof(DebrisNode) == 0x24, "debris node is 0x24 bytes");
	static_assert(sizeof(Spark) == 0x18, "spark is 0x18 bytes");

	inline Mat4x3 &Root(CreatureNode *cn) { return *(Mat4x3 *)(cn->model + 0x40); }
	inline Spark *Sparks(uint8_t *mb) { return (Spark *)(mb + MB_SPARKS); }
}
}

namespace ff8fx
{
namespace m338
{
	using namespace eng;

	static int32_t DivNeg10Sar2(int32_t x)
	{
		int32_t hi = (int32_t)(((int64_t)x * (int32_t)0x99999999) >> 32);
		hi >>= 2;
		hi += (int32_t)((uint32_t)hi >> 31);
		return hi >> 2;
	}

	static void SparksReset()
	{
		uint8_t *p = ModelBuffer() + 0xA;
		for (int i = 0x40; i; i--, p += 0x18) *(int16_t *)p = 0;
		SparkHint() = ModelBuffer();
	}

	static Spark *SparkAlloc()
	{
		uint8_t *MB = ModelBuffer();
		uint8_t *p = SparkHint();
		if (*(int16_t *)(p + 0xA) != 0)
		{
			p = MB;
			if (*(int16_t *)(p + 0xA) != 0)
			{
				int i = 0x40;
				for (;;)
				{
					p += 0x18;
					if (--i == 0) return nullptr;
					if (*(int16_t *)(p + 0xA) == 0) break;
				}
			}
		}
		SparkHint() = p < MB + 0x5E8 ? p + 0x18 : MB;
		return (Spark *)p;
	}

	static void SparksDraw(Spark *pool)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		*(int16_t *)(h + 0x60) = 0;
		*(int16_t *)(h + 0x58) = 0;
		*(int16_t *)(h + 0x50) = 0;
		*(int16_t *)(h + 0x48) = 0;
		for (int i = 0; i < 0x40; i++)
		{
			Spark *s = &pool[i];
			if (s->life == 0) continue;
			GteSetRotMatrixCtrl(&Camera());
			GteSetTransVectorCtrl(&Camera());
			GteLoadV0(s->pos);
			GteMVMVA_RotV0Tr();
			GteReadMAC123((int32_t *)(h + 0x98));
			ff8fx::fill<(0x14)>(h + 0x84, 0);
			*(int16_t *)(h + 0x94) = 0x800;
			*(int16_t *)(h + 0x8C) = 0x800;
			*(int16_t *)(h + 0x84) = 0x800;
			uint8_t *seq = (uint8_t *)s->seq;
			*(uint8_t **)h = seq;
			int32_t n = *(int16_t *)(seq + 8);
			int32_t k = n - ((int32_t)s->life - 1) % n;
			uint8_t *frame = seq + *(int16_t *)(seq + 8 + 2 * k);
			*(uint8_t **)(h + 0x2C) = frame;
			int32_t v = *(int32_t *)frame;
			*(int32_t *)(h + 0x30) = v;
			if (v < 0)
			{
				h[0x25] |= 1;
				*(int32_t *)(h + 0x30) = v & 0x7FFFFFFF;
			}
			*(uint8_t **)(h + 0x2C) = frame + 4;
			PacketCursor() = x::SpriteDraw(h, RenderOT(), 2, PacketCursor());
		}
		FieldFree(0xB4);
	}

	static void SparksMove(Spark *pool)
	{
		for (int i = 0; i < 0x40; i++)
		{
			Spark *s = &pool[i];
			if (s->life == 0) continue;
			int16_t vx = s->vel[0], vy = s->vel[1];
			s->pos[0] = (int16_t)(s->pos[0] + vx);
			s->pos[1] = (int16_t)(s->pos[1] + vy);
			s->vel[0] = (int16_t)(vx + s->acc[0] * 4);
			s->life--;
			s->vel[1] = (int16_t)(vy + s->acc[1] * 4);
			s->pos[2] = (int16_t)(s->pos[2] + (int16_t)(s->acc[2] * 4 + s->vel[2]));
		}
	}

	static void VertexPos(const int16_t *v, int16_t *out)
	{
		uint8_t *MB = ModelBuffer();
		uint8_t *bones = *(uint8_t **)*(uint8_t **)(MB + 0x1BC0 + 0x88) + 0x10;
		BuildBoneMatricesFromPose(MB + 0x1BC0 + 0x84);
		Mat4x3 m;
		ComposeAffineTransform((const Mat4x3 *)(MB + 0x1BC0 + 0x64), (const Mat4x3 *)(bones + 48 * (int32_t)v[3] + 0x10), &m);
		GteSetRotMatrixCtrl(&m);
		GteSetTransVectorCtrl(&m);
		GteLoadV0(v);
		GteMVMVA_RotV0Tr();
		GteStoreIR123(out);
	}

	static void SpawnSparksAt(uint32_t vertex)
	{
		int16_t at[4];
		VertexPos((const int16_t *)vertex, at);
		Spark *s = SparkAlloc();
		if (!s) return;
		s->pos[0] = at[0];
		s->pos[1] = at[1];
		s->life = 7;
		s->pos[2] = (int16_t)(at[2] - 500);
		s->vel[2] = 0;
		s->vel[0] = 0;
		s->vel[1] = 0;
		*(uint32_t *)s->acc = 0;
		s->unused12 = 0xBB8;
		s->seq = game_addr(SEQ_SparkFlash);
		for (int k = 7;;)
		{
			s = SparkAlloc();
			if (!s) return;
			s->pos[0] = at[0];
			s->pos[1] = at[1];
			s->pos[2] = (int16_t)(at[2] - 500);
			s->life = 0xA;
			int32_t r = x::Rand();
			s->vel[0] = (int16_t)((r & 0x7F) + 0x80);
			r = x::Rand();
			int32_t vy = (r & 0x7F) - 0x40;
			s->vel[1] = (int16_t)vy;
			s->pos[1] = (int16_t)(s->pos[1] + vy * 2);
			r = x::Rand();
			if (r & 1) s->vel[0] = (int16_t)-s->vel[0];
			s->acc[2] = 0;
			s->acc[0] = (int8_t)DivNeg10Sar2(s->vel[0]);
			s->vel[2] = 0;
			s->acc[1] = (int8_t)DivNeg10Sar2(s->vel[1]);
			s->unused12 = 0xC00;
			s->seq = game_addr(SEQ_SparkFly);
			if (--k == 0) return;
		}
	}

	static void DrawRing(int32_t px, int32_t py, int32_t pz, int32_t angle, int32_t fade, uint32_t model)
	{
		uint8_t *w = (uint8_t *)FieldAlloc(0x78);
		x::RotX(angle, w);
		Mat4x3 *m = (Mat4x3 *)w;
		m->t[0] = px;
		m->t[1] = py;
		m->t[2] = pz;
		ComposeAffineTransform(&Camera(), m, m);
		GteSetRotMatrix(m);
		GteSetTransVector(m);
		*(int32_t *)(w + 0x28) = 0;
		*(int32_t *)(w + 0x2C) = fade;
		*(int32_t *)(w + 0x3C) = 0xF0;
		*(uint32_t *)(w + 0x20) = model;
		*(int32_t *)(w + 0x38) = -0x80;
		PacketCursor() = x::RenderPrim2(w + 0x20, RenderOT(), 2, PacketCursor());
		FieldFree(0x78);
	}

	static void ScaledRotY(int32_t angle, int32_t radius, uint8_t *out)
	{
		ff8fx::fill<(0x14)>(out, 0);
		int32_t a = -(int32_t)(int16_t)angle;
		int32_t r = (int16_t)radius;
		int32_t s = mul32(ComputeSin(a), r) >> 12;
		int32_t c = mul32(x::Cos(a), r) >> 12;
		*(int16_t *)(out + 0xC) = (int16_t)s;
		*(int16_t *)(out + 8) = (int16_t)radius;
		*(int16_t *)(out + 0) = (int16_t)c;
		*(int16_t *)(out + 4) = (int16_t)-s;
		*(int16_t *)(out + 0x10) = (int16_t)c;
	}

	static void DebrisDraw(const int16_t pos[3], int32_t rx, int32_t ry, int16_t size)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x98);
		ScaledRotY(ry, size, h + 0x78);
		x::RotX(rx, h + 0x58);
		x::MulMatrix(h + 0x58, h + 0x78);
		Mat4x3 *m = (Mat4x3 *)(h + 0x58);
		m->t[0] = pos[0];
		m->t[1] = pos[1];
		m->t[2] = pos[2];
		ComposeAffineTransform(&Camera(), m, m);
		GteSetRotMatrix(m);
		GteSetTransVector(m);
		*(uint32_t *)h = game_addr(MODEL_Debris);
		*(uint32_t *)(h + 0x1C) = 0;
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x98);
	}

	static void PrimCtx(const PrimNode *p, uint8_t *ctx)
	{
		x::RotY(p->yaw, ctx);
		Mat4x3 *m = (Mat4x3 *)ctx;
		m->t[0] = p->pos[0];
		m->t[1] = p->pos[1];
		m->t[2] = p->pos[2];
		ComposeAffineTransform(&Camera(), m, m);
		*(uint32_t *)(ctx + 0x24) = (uint32_t)(ModelBuffer() + MB_SCRATCH);
		*(int32_t *)(ctx + 0x20) = -0x80;
	}

	static uint32_t __cdecl SequenceTask(TaskNode *n)
	{
		MasterNode *node = (MasterNode *)n;
		if (node->parity)
		{
			PacketCursor() = (uint32_t)(ModelBuffer() + MB_ARENA0);
			PacketCursor2() = (uint32_t)(ModelBuffer() + MB_ARENA0 + 0x10000);
			node->parity = 0;
		}
		else
		{
			PacketCursor() = (uint32_t)(ModelBuffer() + MB_ARENA0 + 0x10000);
			PacketCursor2() = (uint32_t)(ModelBuffer() + MB_ARENA0 + 0x20000);
			node->parity = 1;
		}
		Flag23C() = 0;

		if (node->counter == 2 && !node->spawned)
		{
			SparksReset();
			node->spawned = 1;
			InitTaskQueuePool(&QueueDebris(), ModelBuffer() + 0x600, 0x24, 0x40);
			InitTaskQueuePool(&QueueCreature(), ModelBuffer() + 0x1BC0, 0xD0, 1);
			InitTaskQueuePool(&QueuePrims(), ModelBuffer() + 0xF00, 0x198, 8);
			CreatureNode *cn = (CreatureNode *)AddTaskToQueue(&QueueCreature(), game_addr(ORIG_CreatureTask));
			Memset32(&cn->counter, 0, 0x31);
			cn->attacker = Attacker();
			x::Heights(Target(), &cn->h0);
			cn->voice = ClaimVoiceSlot((const void *)game_addr(game::unk_13710CC), 1, 0x80);
			x::BindModel(cn->model, cn->sections, game_addr(game::MAG338_Moomba_ModelContainer));
			x::SetAnim(cn->model, 0);
			x::RotY(0, &Root(cn));
			uint8_t *t = Target();
			W(0) = *(int16_t *)(t + 0x1C);
			W(1) = *(int16_t *)(t + 0x1E);
			W(2) = (int16_t)(*(int16_t *)(t + 0x26) / 4 + *(int16_t *)(t + 0x20) + 0x400);
			int16_t tx = *(int16_t *)(t + 0x1C);
			W(4) = tx;
			W(5) = (int16_t)(cn->h1 + *(int16_t *)(t + 0x1E));
			W(6) = (int16_t)(*(int16_t *)(t + 0x26) / 4 + *(int16_t *)(t + 0x20));
			int16_t *mpos = (int16_t *)(cn->model + 0x1C);
			Root(cn).t[0] = tx;
			mpos[0] = tx;
			Root(cn).t[1] = 0;
			mpos[1] = 0;
			Root(cn).t[2] = W(6);
			mpos[2] = W(6);
			StreamStateInit(ModelBuffer() + MB_STREAM_STATE);
		}

		uint32_t left = (uint32_t)n;
		if (node->spawned)
		{
			left = (uint32_t)ExecuteTaskQueue(&QueueCreature());
			ExecuteTaskQueue(&QueuePrims());
			ExecuteTaskQueue(&QueueDebris());
		}
		if (node->spawned && left == 0)
		{
			x::PreLoad();
			return x::PreLoad0(0) ? TASK_END : 0;
		}
		node->counter++;
		return 0;
	}

	static void SpawnPrim(uint32_t vec, uint32_t layout, int32_t yoff)
	{
		PrimNode *p = (PrimNode *)AddTaskToQueue(&QueuePrims(), game_addr(ORIG_PrimTask));
		if (!p) return;
		const int16_t *v = (const int16_t *)vec;
		p->pos[0] = (int16_t)(v[0] + W(4));
		p->pos[1] = (int16_t)(v[1] + W(1) + yoff);
		p->pos[2] = (int16_t)(v[2] + W(6));
		p->yaw = 0;
		x::Decode(layout, p->layout, 0x184);
	}

	static void SpawnBurst(uint32_t vertex)
	{
		PrimNode *p = (PrimNode *)AddTaskToQueue(&QueuePrims(), game_addr(ORIG_PrimTask));
		if (!p) return;
		int16_t at[4];
		VertexPos((const int16_t *)vertex, at);
		p->pos[1] = 0;
		p->pos[0] = at[0];
		p->pos[2] = at[2];
		p->yaw = 0;
		x::Decode(game_addr(game::unk_13755B8), p->layout, 0xF8);
		DebrisNode *d = (DebrisNode *)AddTaskToQueue(&QueueDebris(), game_addr(ORIG_DebrisTask));
		if (!d) return;
		for (int k = 8;;)
		{
			*(uint32_t *)d->acc = 0;
			d->acc[1] = 0x14;
			int32_t r = x::Rand();
			d->size = (int16_t)((r & 0x7FF) + 0x800);
			d->pos[0] = at[0];
			d->pos[1] = at[1];
			d->pos[2] = at[2];
			r = x::Rand();
			d->life = (uint8_t)((r & 3) + 7);
			r = x::Rand();
			d->vel[0] = (int16_t)((r & 0x7F) - 0x40);
			r = x::Rand();
			d->vel[2] = (int16_t)(-0x80 - (r & 0x7F));
			r = x::Rand();
			d->vel[1] = (int16_t)(-0x80 - (r & 0x7F));
			d->rot[0] = (uint8_t)x::Rand();
			d->rot[1] = (uint8_t)x::Rand();
			d->spin[0] = (uint8_t)x::Rand();
			d->spin[1] = (uint8_t)x::Rand();
			if (--k == 0) return;
			d = (DebrisNode *)AddTaskToQueue(&QueueDebris(), game_addr(ORIG_DebrisTask));
			if (!d) return;
		}
	}

	static uint32_t __cdecl PrimTask(TaskNode *n)
	{
		PrimNode *p = (PrimNode *)n;
		uint8_t ctx[0x28];
		PrimCtx(p, ctx);
		int r = prim::play((prim::Layout *)p->layout, (prim::Callback)game_addr(CB_PrimObject), (int)ctx, 0);
		return r ? 0 : TASK_END;
	}

	inline int32_t DebrisAngle(uint8_t b) { return (int16_t)((int8_t)b * 16); }

	static uint32_t __cdecl DebrisTask(TaskNode *n)
	{
		DebrisNode *d = (DebrisNode *)n;
		DebrisDraw(d->pos, DebrisAngle(d->rot[0]), DebrisAngle(d->rot[1]), d->size);
		d->life--;
		if (d->life == 0)
		{
			return TASK_END;
		}
		d->rot[0] = (uint8_t)(d->rot[0] + d->spin[0]);
		int16_t vy = d->vel[1], vx = d->vel[0];
		d->rot[1] = (uint8_t)(d->rot[1] + d->spin[1]);
		d->pos[1] = (int16_t)(d->pos[1] + vy);
		d->pos[0] = (int16_t)(d->pos[0] + vx);
		int16_t vz = d->vel[2];
		d->pos[2] = (int16_t)(d->pos[2] + vz);
		d->vel[0] = (int16_t)(d->acc[0] + vx);
		d->vel[1] = (int16_t)(d->acc[1] + vy);
		d->vel[2] = (int16_t)(d->acc[2] + vz);
		return 0;
	}

	static void Chain() { x::ChainTransformation(Target(), 4); }

	static uint32_t __cdecl CreatureTask(TaskNode *n)
	{
		CreatureNode *cn = (CreatureNode *)n;
		uint8_t *e = cn->model;
		Mat4x3 &root = Root(cn);

		{
			int16_t c = cn->counter;
			int32_t flash;
			if (c < 8) flash = shl32(c, 11) >> 3;
			else if (c > 0x44) flash = shl32(0x4C - c, 11) >> 3;
			else flash = 0x800;
			SetScreenFlash((uint32_t)flash, 0);
		}

		if (cn->counter == 7)
		{
			SpawnPrim(game_addr(game::unk_1376678), game_addr(game::unk_1372E60), cn->h1);
			Chain();
		}

		uint32_t e8 = (uint32_t)((int32_t)cn->counter - 8);
		if (e8 < 0x30)
		{
			switch ((int32_t)e8 - 4)
			{
			case 0: SpawnPrim(game_addr(game::unk_1376680), game_addr(game::unk_137420C), cn->h1); Chain(); break;
			case 3: SpawnPrim(game_addr(game::unk_1376688), game_addr(game::unk_1372E60), cn->h1); Chain(); break;
			case 6: SpawnPrim(game_addr(game::unk_1376690), game_addr(game::unk_137420C), cn->h1); Chain(); break;
			case 11: case 17: SpawnSparksAt(game_addr(game::unk_1376670)); Chain(); break;
			case 14: case 20: SpawnSparksAt(game_addr(game::unk_1376668)); Chain(); break;
			case 27: Chain(); SpawnBurst(game_addr(game::unk_1376660)); break;
			case 30: SpawnBurst(game_addr(game::unk_1376658)); break;
			case 33: SpawnBurst(game_addr(game::unk_1376660)); break;
			case 36:
				x::ApplyResultToTarget(*(void **)(*(uint8_t **)(CastCtx() + 4) + 8));
				SpawnBurst(game_addr(game::unk_1376658));
				break;
			default: break;
			}

			if (e8 >= 0x20 && e8 <= 0x2A)
			{
				int count = 2;
				Spark *s = SparkAlloc();
				if (!s) return 0;
				for (;;)
				{
					s->life = 0xC;
					int32_t r = x::Rand();
					uint8_t *t = Target();
					uint32_t nb = **(uint8_t **)*(uint8_t **)(t + 0x64);
					x::SpawnPosition(t, mul32(r, (int32_t)nb) >> 15, 0, s->pos);
					s->vel[0] = 0;
					s->vel[1] = 0;
					s->vel[2] = 0;
					*(uint32_t *)s->acc = 0;
					r = x::Rand();
					s->seq = game_addr(SEQ_Spark);
					s->unused12 = (int16_t)((r & 0xFFF) + 0x800);
					if (--count == 0) break;
					s = SparkAlloc();
					if (!s) return 0;
				}
			}

			uint32_t e32 = e8 - 0x18;
			if (e32 < 5)
			{
				int32_t s = ComputeSin(0x400 - (int32_t)((e32 << 10) >> 2));
				int16_t out[4];
				x::Blend((const void *)game_addr(game::unk_2556240), (const void *)game_addr(game::unk_2556248), 0x1000 - s, s, out);
				root.t[1] = out[1];
				root.t[2] = out[2];
			}
		}

		{
			uint32_t c = (uint32_t)(int32_t)cn->counter;
			if (c < 8)
			{
				uint32_t q7 = (c << 10) / 7;
				int32_t a = 0x400 - (int32_t)q7;
				int32_t s = ComputeSin(a);
				root.t[1] = (int32_t)W(5) - mul32(s, 2000) / 4096;
				root.t[2] = mul32(a, 4000) / 1024 + W(6);
				int32_t co = x::Cos((int32_t)q7);
				int32_t fade = 0x1000 - x::Cos((0x1000 - co) >> 2);
				DrawRing(root.t[0], root.t[1], root.t[2], (int32_t)(c << 8), fade, game_addr(MODEL_Ring));
			}
		}

		{
			uint32_t ee = (uint32_t)((int32_t)cn->counter - 0x38);
			if (ee < 4)
			{
				int32_t b = (int32_t)((ee << 10) / 3);
				int count = 8;
				Spark *s = SparkAlloc();
				if (s)
				{
					for (;;)
					{
						int32_t r1 = x::Rand();
						int32_t r2 = x::Rand();
						int32_t rad = (r2 & 0x3F) + 0x20;
						int32_t sn = ComputeSin(r1);
						s->vel[1] = 0;
						s->vel[0] = (int16_t)(mul32(sn, rad) / 4096);
						int32_t cs = x::Cos(r1);
						int32_t vz = mul32(cs, rad) / 4096;
						s->vel[2] = (int16_t)vz;
						s->pos[0] = (int16_t)(s->vel[0] + W(0));
						s->pos[1] = W(1);
						s->acc[2] = 0;
						s->pos[2] = (int16_t)(vz + W(2));
						s->acc[1] = 0;
						s->acc[0] = 0;
						s->life = 0xC;
						int32_t r3 = x::Rand();
						s->seq = game_addr(SEQ_Spark);
						s->unused12 = (int16_t)((r3 & 0xFFF) + 0xBB8);
						int old = count--;
						if (old == 0) break;
						s = SparkAlloc();
						if (!s) break;
					}
				}
				int32_t sn = ComputeSin(b);
				root.t[1] = (int32_t)W(1) - mul32(sn, 2000) / 4096;
				root.t[2] = mul32(b, 1000) / 1024 + W(2);
			}
		}

		{
			uint32_t ee = (uint32_t)((int32_t)cn->counter - 0x3C);
			if (ee < 8)
			{
				int32_t a = (int32_t)(((ee + 1) << 10) >> 3);
				int32_t s = ComputeSin(a);
				root.t[1] = (int32_t)W(1) - mul32(s, 4000) / 4096 - 0x7D0;
				root.t[2] = mul32(a, 4000) / 1024 + W(2) + 0x3E8;
			}
		}

		{
			uint32_t ee = (uint32_t)((int32_t)cn->counter - 0x38);
			if (ee < 0xC)
			{
				uint32_t q = ((ee << 12) / 12) >> 2;
				int32_t s = ComputeSin((int32_t)q) >> 2;
				int32_t fade = 0x1000 - ComputeSin(s);
				DrawRing(root.t[0], root.t[1], root.t[2], (int32_t)(ee << 8), fade, game_addr(MODEL_Ring));
			}
		}

		if ((uint16_t)cn->counter < 0x44)
		{
			x::AdvanceAnim(e);
			PacketCursor() = x::DrawModel(e, ModelBuffer() + MB_SCRATCH, PacketCursor(), PacketCursor2());
		}

		x::RotY(0, (void *)game_addr(game::unk_1D99AB0));
		var<int32_t>(game_addr(game::unk_1D99AC4)) = root.t[0];
		var<int32_t>(game_addr(game::unk_1D99AC8)) = root.t[1];
		var<int32_t>(game_addr(game::unk_1D99ACC)) = root.t[2];

		SparksDraw(Sparks(ModelBuffer()));
		SparksMove(Sparks(ModelBuffer()));

		if (cn->counter == 0) x::PlaySE(game_addr(game::unk_13710C8), 0, 0x80);
		{
			int16_t c = cn->counter;
			if (c == 8) x::SetAnim(e, 1);
			else if (c == 0x38) x::SetAnim(e, 2);
			else if (c == 0x3C) x::SetAnim(e, 3);
		}
		if (cn->counter == 0x48) x::ReleaseVoice(cn->voice);
		cn->counter++;
		if (cn->counter < 0x4C) return 0;
		Flag23C() = 0;
		x::Load(0x16A, (uint32_t)(ModelBuffer() + MB_ARENA0), 0);
		return TASK_END;
	}
}

	void register_mag338_moomba()
	{
		register_port(m338::ORIG_SequenceTask, (void *)m338::SequenceTask, "M338 SequenceTask", 338);
		register_port(m338::ORIG_CreatureTask, (void *)m338::CreatureTask, "M338 CreatureTask", 338);
		register_port(m338::ORIG_PrimTask, (void *)m338::PrimTask, "M338 PrimTask", 338);
		register_port(m338::ORIG_DebrisTask, (void *)m338::DebrisTask, "M338 DebrisTask", 338);
	}
}

