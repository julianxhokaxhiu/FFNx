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

// Effect 21: Triple

#include "mag_common.h"

namespace ff8fx
{
namespace triple021
{
	using namespace eng;
	using namespace magc;

#pragma pack(push, 1)
	struct FxNode
	{
		TaskNode hdr;
		CastContext *ctx;
		FxNode *root;
		FxNode *emitter;
		FxNode *parent;
		int16_t pos[4];
		int16_t counter;
		uint16_t flags;
		uint8_t children;
		int8_t phase;
		int8_t action;
		int8_t target;
		uint8_t attacker;
		uint8_t slot;
		uint8_t line;
		uint8_t last;
	};
	struct RootNode
	{
		FxNode h;
		uint8_t bones[0x28];
		int16_t last_action;
		int16_t target_count;
		int16_t tick;
		int16_t alive;
		uint8_t pad60[3];
		uint8_t busy;
	};
	struct EmitterNode
	{
		FxNode h;
		uint8_t anchor[0x18];
		int16_t bmin[4];
		int16_t bmax[4];
		int16_t signal;
		int16_t pad5A;
	};
	struct SeqNode
	{
		FxNode h;
		uint8_t pad30[0x10];
	};
	struct ModelNode
	{
		FxNode h;
		int32_t scale[3];
		uint32_t pad3C;
		uint8_t rgb[4];
		int16_t angle[3];
		int16_t pad4A;
		uint32_t model;
		int16_t fade;
		int16_t pad52;
		int16_t base[4];
		uint8_t rgb1[4];
		uint8_t rgb2[4];
		int16_t fade1;
		int16_t fade2;
		int16_t orbit;
		int16_t flagged;
		int16_t flash;
		int16_t radius;
		int16_t flash_on;
		int16_t orbit_step;
		int16_t morph;
		int16_t arg;
		int16_t step;
		int16_t pad7A;
	};
	struct SpriteNode
	{
		FxNode h;
		int32_t scale[3];
		uint32_t pad3C;
		int16_t ax;
		int16_t ay;
		uint32_t pad44;
		uint8_t rgb[4];
		uint32_t seq;
		int16_t frame;
		int16_t last;
		int16_t shade;
		int16_t pad56;
		int16_t base[4];
		int16_t pad60;
		int16_t vel;
		uint32_t pad64;
		int16_t radius;
		int16_t angle;
		int16_t pulse;
		int16_t pad6E;
	};
	struct FlashNode
	{
		FxNode h;
		int16_t pad30;
		int16_t vel;
		uint32_t pad34;
		int16_t target[4];
		int16_t tx, ty;
		int16_t cx, cy;
		int16_t u, v;
		int16_t tw, th;
	};
#pragma pack(pop)
	static_assert(sizeof(FxNode) == 0x30 && sizeof(RootNode) == 0x64 && sizeof(EmitterNode) == 0x5C, "Triple nodes");
	static_assert(sizeof(SeqNode) == 0x40 && sizeof(ModelNode) == 0x7C && sizeof(SpriteNode) == 0x70, "Triple nodes");
	static_assert(sizeof(FlashNode) == 0x50, "Triple nodes");

	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::g_Triple_PacketCursor)); }
	inline TaskQueue *QEmitters() { return (TaskQueue *)game_addr(game::MAG_021_EmitterQueue); }
	inline TaskQueue *QSequencers() { return (TaskQueue *)game_addr(game::MAG_021_SequencerQueue); }
	inline TaskQueue *QSprites() { return (TaskQueue *)game_addr(game::MAG_021_SpriteQueue); }
	inline TaskQueue *QModels() { return (TaskQueue *)game_addr(game::MAG_021_ModelQueue); }
	inline TaskQueue *QFlashes() { return (TaskQueue *)game_addr(game::MAG_021_FlashQueue); }

	static constexpr game::Id ORIG_Root = game::MAG_021_TRIPLE_Tick;
	static constexpr game::Id ORIG_Emitter = game::MAG_021_TRIPLE_Emitter_Tick;
	static constexpr game::Id ORIG_BigRune = game::MAG_021_TRIPLE_BigRune_Tick;
	static constexpr game::Id ORIG_Morph = game::MAG_021_TRIPLE_MorphRing_Tick;
	static constexpr game::Id ORIG_Sprite = game::MAG_021_TRIPLE_Sprite_Tick;
	static constexpr game::Id ORIG_Sequencer = game::MAG_021_TRIPLE_Sequencer_Tick;
	static constexpr game::Id ORIG_Rune = game::MAG_021_TRIPLE_Rune_Tick;
	static constexpr game::Id ORIG_Column = game::MAG_021_TRIPLE_Column_Tick;
	static constexpr game::Id ORIG_Glow = game::MAG_021_TRIPLE_Glow_Tick;
	static constexpr game::Id ORIG_Mote = game::MAG_021_TRIPLE_Mote_Tick;
	static constexpr game::Id ORIG_Spark = game::MAG_021_TRIPLE_Spark_Tick;
	static constexpr game::Id ORIG_Flash = game::MAG_021_TRIPLE_Flash_Tick;

	static const void *SOUND_Triple() { return (const void *)game_addr(game::unk_16269D8); }
	static constexpr game::Id MODEL_BigRune1 = game::unk_1627380, MODEL_BigRune2 = game::unk_1628218, MODEL_Rune = game::unk_1626CF0;
	static constexpr game::Id MODEL_Column = game::unk_1626F00, MODEL_ColumnFlash = game::unk_1627160, MODEL_Morph = game::unk_1627970;
	static constexpr game::Id MORPH_A = game::unk_16276E8, MORPH_B = game::unk_1629000;
	static constexpr game::Id SEQ_Sprite = game::unk_1626B3C, SEQ_Glow = game::unk_1626CC8, SEQ_Mote = game::unk_16269DC, SEQ_Spark = game::unk_1626C18;
	static constexpr game::Id FADE_TABLE_A = game::g_Triple_FadeTableA, FADE_TABLE_B = game::g_Triple_FadeTableB;
	static constexpr game::Id ARENA_A = game::g_Triple_ArenaA, ARENA_B = game::g_Triple_ArenaB;

	inline FxNode *AddEffectTask(TaskQueue *q, uint32_t task, int32_t size, void *parent) { return fn<FxNode *(__cdecl *)(TaskQueue *, uint32_t, int32_t, void *)>(game_addr(game::Effect_AddTaskAndInitFromCtx))(q, task, size, parent); }
	inline void ReleaseLinkedTask(void *n) { fn<void (__cdecl *)(void *)>(game_addr(game::Effect_ReleaseLinkedTask))(n); }
	inline void EmitterUpdatePos(void *n) { fn<void (__cdecl *)(void *)>(game_addr(game::MAG_001_CURE_Emitter_UpdatePos))(n); }
	inline void ComputeModelBounds(void *n) { fn<void (__cdecl *)(void *)>(game_addr(game::MAG_001_CURE_Emitter_ComputeModelBounds))(n); }
	inline void UpdateTargetPosFromBones(void *n) { fn<void (__cdecl *)(void *)>(game_addr(game::Effect_UpdateTargetPosFromBones))(n); }
	inline void CopyAnchorXZFromSource(void *n, int16_t *out) { fn<void (__cdecl *)(void *, int16_t *)>(game_addr(game::Effect_CopyAnchorXZFromSource))(n, out); }
	inline void CopyMidpointFromSource(void *n, int16_t *out) { fn<void (__cdecl *)(void *, int16_t *)>(game_addr(game::Effect_CopyMidpointFromSource))(n, out); }
	inline void CopyAnchorTopFromSource(void *n, int16_t *out) { fn<void (__cdecl *)(void *, int16_t *)>(game_addr(game::Effect_CopyAnchorTopFromSource))(n, out); }
	inline void IdentityMatrix(Mat4x3 *m) { fn<void (__cdecl *)(Mat4x3 *)>(game_addr(game::Mtx_Identity))(m); }
	inline void RotateX(Mat4x3 *m, int32_t a) { fn<void (__cdecl *)(Mat4x3 *, int32_t)>(game_addr(game::Mtx_RotX))(m, a); }
	inline void RotateY(Mat4x3 *m, int32_t a) { fn<void (__cdecl *)(Mat4x3 *, int32_t)>(game_addr(game::Mtx_RotY))(m, a); }
	inline void MatrixMulVector(const Mat4x3 *m, const int16_t *v, int16_t *out) { fn<void (__cdecl *)(const Mat4x3 *, const int16_t *, int16_t *)>(game_addr(game::matrixMultiplyVector))(m, v, out); }
	inline int32_t ComputeCos(int32_t a) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(a); }
	inline void InsertPrimAltViewport(uint32_t ot, void *prim) { fn<void (__cdecl *)(uint32_t, void *)>(game_addr(game::SSIGPU_InsertPrimAltViewport))(ot, prim); }

	template<typename T> static inline T &F(uint8_t *p, uint32_t off) { return *(T *)(p + off); }

	static int16_t FadeTable(uint32_t table, int16_t k)
	{
		if (k > 0xB) k = 0xB;
		else if (k < 0) k = 0;
		return ((const int16_t *)table)[k];
	}

	static void DrawModel(const ModelNode *n)
	{
		if (n->h.flags & 4) return;
		Mat4x3 m;
		BuildRotationMatrixFromAngles(n->angle, &m);
		m.t[0] = n->h.pos[0];
		m.t[1] = n->h.pos[1];
		m.t[2] = n->h.pos[2];
		Scale3DMatrix(&m, n->scale);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		F<uint32_t>(h, 0) = n->model;
		F<uint32_t>(h, 8) = *(const uint32_t *)n->rgb;
		F<int32_t>(h, 0xC) = n->fade;
		F<uint32_t>(h, 0x1C) = 0xF0;
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static void DrawSpriteTurned(const SpriteNode *p)
	{
		if (p->h.flags & 4) return;
		Mat4x3 m;
		IdentityMatrix(&m);
		RotateY(&m, p->ay);
		RotateX(&m, p->ax);
		m.t[0] = p->h.pos[0];
		m.t[1] = p->h.pos[1];
		m.t[2] = p->h.pos[2];
		Scale3DMatrix(&m, p->scale);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		F<uint32_t>(h, 0) = p->seq;
		F<int16_t>(h, 4) = p->frame;
		F<uint32_t>(h, 0x1C) = *(const uint32_t *)p->rgb;
		F<int16_t>(h, 0x24) = 4;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static void DrawGroundGlow(const SpriteNode *p, uint32_t seq, const uint32_t *colour)
	{
		if (p->h.flags & 4) return;
		Mat4x3 m;
		IdentityMatrix(&m);
		RotateX(&m, 0x400);
		m.t[0] = p->h.pos[0];
		m.t[1] = 0;
		m.t[2] = p->h.pos[2];
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		F<uint32_t>(h, 0) = seq;
		F<int16_t>(h, 4) = 0;
		F<uint32_t>(h, 0x1C) = *colour;
		F<int16_t>(h, 0x24) = 4;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static void DrawSpriteFacing(const SpriteNode *p)
	{
		if (p->h.flags & 4) return;
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		TransformCameraByShadowRotation(p->h.pos, 0x1000, p->shade);
		F<uint32_t>(h, 0) = p->seq;
		F<int16_t>(h, 4) = p->frame;
		F<int16_t>(h, 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static void MorphVertices(const uint8_t *a, const uint8_t *b, uint8_t *out, int16_t t)
	{
		const uint8_t *pa = a + 8;
		const uint8_t *pb = b + 8;
		uint8_t *po = out + 8;
		int32_t count = *(const int32_t *)(a + 4);
		int32_t *w = (int32_t *)FieldAlloc(8);
		w[1] = t;
		w[0] = 0x1000 - t;
		for (; count > 0; count--)
		{
			GteSetIR0(w[0]);
			GteLoadIR123(pa);
			GteGPF();
			GteSetIR0(w[1]);
			GteLoadIR123(pb);
			GteGPL();
			GteStoreIR123(po);
			pa += 8;
			pb += 8;
			po += 8;
		}
		FieldFree(8);
	}

	static void SpawnSprites(ModelNode *n, int16_t mode)
	{
		if (n->h.counter < 2 || n->h.counter > 4) return;
		for (int32_t i = 0; i < 4; i++)
		{
			const int32_t k = (i + (int32_t)n->h.counter * 4) % 4;
			SpriteNode *p = (SpriteNode *)AddEffectTask(QSprites(), game_addr(ORIG_Sprite), 0x70, n);
			p->scale[0] = 0x1000;
			p->scale[1] = 0x1000;
			p->scale[2] = 0x1000;
			p->seq = game_addr(SEQ_Sprite);
			p->last = 7;
			int16_t v[4];
			if (mode == 0)
			{
				const int32_t r = CrtRand() % 0x400;
				v[1] = 0;
				v[0] = (int16_t)(r - 0x200);
				v[2] = (int16_t)-0x2C0;
				p->ay = (int16_t)(k << 10);
			}
			else if (mode == 1)
			{
				const int32_t r = CrtRand() % 0x400;
				v[1] = 0;
				v[0] = (int16_t)(r - 0x200);
				v[2] = (int16_t)-0x2C0;
				p->ay = (int16_t)((k << 10) + 0x200);
			}
			Mat4x3 m;
			IdentityMatrix(&m);
			RotateY(&m, p->ay);
			MatrixMulVector(&m, v, v);
			p->h.pos[0] = (int16_t)(p->h.pos[0] + v[0]);
			p->h.pos[1] = (int16_t)(p->h.pos[1] + v[1]);
			p->h.pos[2] = (int16_t)(p->h.pos[2] + v[2]);
		}
	}

	static void SpawnMorph(ModelNode *n)
	{
		if (n->h.counter != 0) return;
		ModelNode *m = (ModelNode *)AddEffectTask(QModels(), game_addr(ORIG_Morph), 0x7C, n);
		CopyAnchorXZFromSource(m, m->h.pos);
		m->h.pos[1] = (int16_t)(m->h.pos[1] - 0x280);
		m->angle[1] = 0;
		m->model = game_addr(MODEL_Morph);
		m->fade = 0;
		m->scale[0] = 0x1000;
		m->scale[1] = 0x1000;
		m->scale[2] = 0x1000;
		m->rgb[0] = 0;
		m->rgb[1] = 0;
		m->rgb[2] = 0;
	}

	static void SpawnBigRune(EmitterNode *e)
	{
		ModelNode *n = (ModelNode *)AddEffectTask(QModels(), game_addr(ORIG_BigRune), 0x7C, e);
		CopyAnchorXZFromSource(n, n->h.pos);
		n->h.pos[1] = (int16_t)(n->h.pos[1] - 0x280);
		n->scale[0] = 0x1000;
		n->scale[1] = 0x1000;
		n->scale[2] = 0x1000;
		n->angle[1] = 0;
		n->model = game_addr(MODEL_BigRune1);
		n->fade1 = 0x800;
		n->fade2 = 0;
		n->rgb1[0] = 0xFF;
		n->rgb1[1] = 0xFF;
		n->rgb1[2] = 0xFF;
		n->rgb2[0] = 0;
		n->rgb2[1] = 0;
		n->rgb2[2] = 0;
	}

	static void BigRunePhase(ModelNode *n)
	{
		switch (n->h.phase)
		{
		case 0:
			n->fade1 = (int16_t)(n->fade1 - 0x200);
			*(uint8_t *)&n->h.flags |= 2;
			if (n->fade1 <= 0)
			{
				n->fade1 = 0;
				n->rgb1[0] = 0;
				n->rgb1[1] = 0;
				n->rgb1[2] = 0;
				n->h.phase++;
			}
			break;
		case 1:
		{
			n->step++;
			const int16_t a = FadeTable(game_addr(FADE_TABLE_A), n->step);
			n->fade = a;
			n->fade1 = a;
			const int16_t b = FadeTable(game_addr(FADE_TABLE_B), n->step);
			n->fade = b;
			n->fade2 = b;
			if (n->step >= 0xB)
			{
				n->h.flags = (uint16_t)((n->h.flags & 0xFFFD) | 5);
				n->h.phase++;
			}
			break;
		}
		default: break;
		}
	}

	static void BigRuneDraw(ModelNode *n)
	{
		ff8fx::copy<(4)>(n->rgb, n->rgb1);
		n->fade = n->fade1;
		n->model = game_addr(MODEL_BigRune1);
		DrawModel(n);
		ff8fx::copy<(4)>(n->rgb, n->rgb2);
		n->fade = n->fade2;
		n->model = game_addr(MODEL_BigRune2);
		DrawModel(n);
	}
}
}

namespace ff8fx
{
namespace triple021
{
	static uint32_t __cdecl BigRuneTask(TaskNode *t)
	{
		ModelNode *n = (ModelNode *)t;
		BigRunePhase(n);
		BigRuneDraw(n);
		if (n->h.flags & 2)
		{
			SpawnSprites(n, 0);
			SpawnSprites(n, 1);
			SpawnMorph(n);
		}
		n->h.counter++;
		if ((n->h.flags & 1) && n->h.children == 0)
		{
			ReleaseLinkedTask(n);
			return TASK_END;
		}
		return 0;
	}

	static void MorphPhase(ModelNode *n)
	{
		if (n->h.phase != 0) return;
		n->step++;
		const int16_t a = FadeTable(game_addr(FADE_TABLE_A), n->step);
		n->fade = a;
		n->morph = a;
		if (n->step >= 0xB)
		{
			*(uint8_t *)&n->h.flags |= 1;
			n->h.phase++;
		}
	}

	static uint32_t __cdecl MorphTask(TaskNode *t)
	{
		ModelNode *n = (ModelNode *)t;
		MorphPhase(n);
		MorphVertices((const uint8_t *)game_addr(MORPH_A), (const uint8_t *)game_addr(MORPH_B), (uint8_t *)game_addr(MODEL_Morph), n->morph);
		DrawModel(n);
		n->h.counter++;
		if ((n->h.flags & 1) && n->h.children == 0)
		{
			ReleaseLinkedTask(n);
			return TASK_END;
		}
		return 0;
	}

	static void SpritePhase(SpriteNode *p)
	{
		uint8_t g;
		switch (p->h.phase)
		{
		case 0: g = 0x18; break;
		case 1: g = 0x30; break;
		case 2: g = 0x40; break;
		case 3: p->h.phase++; return;
		case 4: g = 0x30; break;
		case 5: g = 0x18; break;
		case 6:
			*(uint8_t *)&p->h.flags |= 1;
			p->rgb[0] = 0;
			p->rgb[1] = 0;
			p->rgb[2] = 0;
			return;
		default: return;
		}
		p->rgb[0] = g;
		p->rgb[1] = g;
		p->rgb[2] = g;
		p->h.phase++;
	}

	static uint32_t __cdecl SpriteTask(TaskNode *t)
	{
		SpriteNode *p = (SpriteNode *)t;
		SpritePhase(p);
		p->ax = (int16_t)(CrtRand() & 0xFFF);
		const int32_t r = CrtRand() % 7;
		p->frame = (int16_t)((uint8_t)(r + (uint8_t)p->frame + 1) & 7);
		DrawSpriteTurned(p);
		p->h.counter++;
		if ((p->h.flags & 1) && p->h.children == 0)
		{
			ReleaseLinkedTask(p);
			return TASK_END;
		}
		return 0;
	}

	static void Scatter(SpriteNode *p, int16_t n)
	{
		int32_t d = n;
		if (n == 0) d = 1;
		const int32_t yaw = CrtRand() & 0xFFF;
		const int32_t pitch = CrtRand() & 0xFFF;
		Mat4x3 m;
		IdentityMatrix(&m);
		RotateY(&m, (int16_t)yaw);
		RotateX(&m, (int16_t)pitch);
		int16_t v[4];
		int16_t out[4];
		v[0] = 0;
		v[1] = 0;
		v[2] = (int16_t)(CrtRand() % (int16_t)d);
		MatrixMulVector(&m, v, out);
		p->h.pos[0] = (int16_t)(p->h.pos[0] + out[0]);
		p->h.pos[1] = (int16_t)(p->h.pos[1] + (int16_t)(out[1] * 2));
		p->h.pos[2] = (int16_t)(p->h.pos[2] + out[2]);
	}

	static int32_t LifetimeExpired(SpriteNode *p)
	{
		p->frame++;
		if (p->frame > p->last)
		{
			*(uint8_t *)&p->h.flags |= 4;
			p->frame = p->last;
			return 1;
		}
		return 0;
	}

	static void SparkMove(SpriteNode *p)
	{
		p->h.pos[1] = (int16_t)(p->h.pos[1] + p->vel);
		if (LifetimeExpired(p))
		{
			*(uint8_t *)&p->h.flags |= 1;
			p->h.phase++;
		}
	}

	static uint32_t __cdecl SparkTask(TaskNode *t)
	{
		SpriteNode *p = (SpriteNode *)t;
		switch (p->h.phase)
		{
		case 0:
		{
			const FxNode *parent = p->h.parent;
			ff8fx::copy<(8)>(p->h.pos, parent->pos);
			Scatter(p, 0x80);
			p->last = 4;
			p->rgb[0] = 0x80;
			p->rgb[1] = 0x80;
			p->rgb[2] = 0x80;
			p->vel = (int16_t)(-8 - (CrtRand() & 0x1F));
			p->h.phase++;
			break;
		}
		case 1: SparkMove(p); break;
		default: break;
		}
		DrawSpriteFacing(p);
		p->h.counter++;
		if ((p->h.flags & 1) && p->h.children == 0)
		{
			ReleaseLinkedTask(p);
			return TASK_END;
		}
		return 0;
	}

	static void ColumnPhase(ModelNode *n, bool spawn)
	{
		switch (n->h.phase)
		{
		case 0:
			n->fade = (int16_t)(n->fade - 0x400);
			n->scale[0] += 0x200;
			n->scale[2] += 0x200;
			if (n->fade <= 0)
			{
				n->fade = 0;
				n->scale[0] = 0x800;
				n->scale[2] = 0x800;
				n->h.phase++;
			}
			break;
		case 1:
			n->scale[1] += 0x400;
			if (n->scale[1] >= 0x1000) n->h.phase++;
			break;
		case 2:
			n->scale[1] += 0x400;
			if (n->scale[1] >= 0x1800)
			{
				n->scale[1] = 0x1800;
				n->h.phase++;
			}
			break;
		case 3:
			n->scale[0] += 0x400;
			n->scale[1] -= 0x400;
			n->scale[2] += 0x400;
			if (n->scale[1] <= 0x1000)
			{
				n->scale[0] = 0x1000;
				n->scale[1] = 0x1000;
				n->scale[2] = 0x1000;
				if (n->flagged == 0) n->flash = 1;
				else ((EmitterNode *)n->h.emitter)->signal = 1;
				n->h.phase++;
			}
			break;
		case 4:
			if (((EmitterNode *)n->h.emitter)->signal == 2)
			{
				n->flash = 3;
				n->h.phase++;
			}
			break;
		case 5:
			if (n->flash == 0)
			{
				if (n->fade <= 0xA00 && spawn)
				{
					SpriteNode *p = (SpriteNode *)AddEffectTask(QSprites(), game_addr(ORIG_Spark), 0x70, n);
					p->seq = game_addr(SEQ_Spark);
				}
				n->fade = (int16_t)(n->fade + 0x200);
				if (n->fade >= 0x1000)
				{
					*(uint8_t *)&n->h.flags |= 5;
					n->fade = 0x1000;
					n->h.phase++;
				}
			}
			break;
		default: break;
		}
	}

	static void ColumnFlash(ModelNode *n)
	{
		const int16_t s = n->flash;
		switch (s)
		{
		case 1:
		case 3:
			n->flash_on = 1;
			n->fade = (int16_t)(s == 1 ? 0x400 : 0x800);
			n->rgb[0] = 0xFF;
			n->rgb[1] = 0xFF;
			n->rgb[2] = 0xFF;
			n->flash = (int16_t)(s + 1);
			return;
		case 2:
			n->fade = (int16_t)(n->fade - 0x100);
			if (n->fade > 0) return;
			break;
		case 4:
			n->fade = (int16_t)(n->fade - 0x200);
			if (n->fade > 0) return;
			break;
		default: return;
		}
		n->fade = 0;
		n->rgb[0] = 0;
		n->rgb[1] = 0;
		n->rgb[2] = 0;
		n->flash_on = 0;
		n->flash = 0;
	}

	static void ColumnMotion(ModelNode *n)
	{
		const int16_t orbit = (int16_t)((n->orbit_step + n->orbit) & 0xFFF);
		n->angle[1] = (int16_t)((n->angle[1] + 0x140) & 0xFFF);
		n->orbit = orbit;
		const int32_t dx = mul32(ComputeSin(n->orbit), n->radius) / 0x4000;
		const int32_t dz = mul32(ComputeCos(n->orbit), n->radius) / 0x4000;
		ff8fx::copy<(8)>(n->h.pos, n->base);
		n->h.pos[0] = (int16_t)(n->h.pos[0] + dx);
		n->h.pos[2] = (int16_t)(n->h.pos[2] + dz);
	}

	static void ColumnDraw(ModelNode *n)
	{
		if (n->flash_on == 1)
		{
			n->model = game_addr(MODEL_ColumnFlash);
			DrawModel(n);
			n->model = game_addr(MODEL_Column);
		}
		DrawModel(n);
	}

	static uint32_t __cdecl ColumnTask(TaskNode *t)
	{
		ModelNode *n = (ModelNode *)t;
		ColumnPhase(n, true);
		ColumnFlash(n);
		ColumnMotion(n);
		ColumnDraw(n);
		n->h.counter++;
		if ((n->h.flags & 1) && n->h.children == 0)
		{
			ReleaseLinkedTask(n);
			return TASK_END;
		}
		return 0;
	}

	static void AddGrey(SpriteNode *p, uint8_t d)
	{
		p->rgb[1] = (uint8_t)(p->rgb[1] + d);
		p->rgb[0] = (uint8_t)(p->rgb[0] + d);
		p->rgb[2] = (uint8_t)(p->rgb[2] + d);
	}

	static void GlowPhase(SpriteNode *p)
	{
		switch (p->h.phase)
		{
		case 0:
			AddGrey(p, 4);
			if (p->rgb[0] >= 0x20) p->h.phase++;
			break;
		case 1:
			if (((EmitterNode *)p->h.emitter)->signal == 2)
			{
				p->pulse = 3;
				p->h.phase++;
			}
			break;
		case 2:
			if (p->pulse == 0)
			{
				AddGrey(p, 0xFC);
				if (p->rgb[0] == 0)
				{
					*(uint8_t *)&p->h.flags |= 5;
					p->h.phase++;
				}
			}
			break;
		default: break;
		}
	}

	static void GlowPulse(SpriteNode *p)
	{
		switch (p->pulse)
		{
		case 1:
		case 3:
			p->rgb[0] = 0x40;
			p->rgb[1] = 0x40;
			p->rgb[2] = 0x40;
			p->pulse = (int16_t)(p->pulse + 1);
			break;
		case 2:
		case 4:
			AddGrey(p, 0xFC);
			if (p->rgb[0] <= 0x20) p->pulse = 0;
			break;
		default: break;
		}
	}

	static void GlowPlace(SpriteNode *p)
	{
		ff8fx::copy<(8)>(p->h.pos, p->base);
		p->h.pos[0] = (int16_t)(p->h.pos[0] + ComputeSin(p->angle) / 4);
		p->h.pos[2] = (int16_t)(p->h.pos[2] + ComputeCos(p->angle) / 4);
	}

	static uint32_t __cdecl GlowTask(TaskNode *t)
	{
		SpriteNode *p = (SpriteNode *)t;
		GlowPhase(p);
		GlowPulse(p);
		GlowPlace(p);
		DrawGroundGlow(p, game_addr(SEQ_Glow), (const uint32_t *)p->rgb);
		p->h.counter++;
		if ((p->h.flags & 1) && p->h.children == 0)
		{
			ReleaseLinkedTask(p);
			return TASK_END;
		}
		return 0;
	}

	static void Orbit(SpriteNode *p, int16_t r, int16_t angle)
	{
		p->h.pos[0] = (int16_t)(p->h.pos[0] + mul32(ComputeSin(angle), r) / 4096);
		p->h.pos[2] = (int16_t)(p->h.pos[2] + mul32(ComputeCos(angle), r) / 4096);
	}

	static void MoteUpdate(SpriteNode *p)
	{
		if (p->h.phase == 0)
		{
			p->rgb[1] = (uint8_t)(p->rgb[1] + 0x20);
			p->rgb[2] = (uint8_t)(p->rgb[2] + 0x20);
			p->rgb[0] = (uint8_t)(p->rgb[0] + 0x20);
			if (p->rgb[0] >= 0x80)
			{
				p->rgb[0] = 0x80;
				p->rgb[1] = 0x80;
				p->rgb[2] = 0x80;
				p->h.phase++;
			}
		}
		p->radius = (int16_t)(p->radius - 0x60);
		if (p->radius <= 0) *(uint8_t *)&p->h.flags |= 1;
		p->ay = (int16_t)((p->ay + 0x60) & 0xFFF);
	}

	static void MotePlace(SpriteNode *p)
	{
		ff8fx::copy<(8)>(p->h.pos, p->base);
		Orbit(p, p->radius, p->ay);
	}

	static uint32_t __cdecl MoteTask(TaskNode *t)
	{
		SpriteNode *p = (SpriteNode *)t;
		MoteUpdate(p);
		MotePlace(p);
		DrawSpriteFacing(p);
		p->frame++;
		if (p->frame > p->last) p->frame = 0;
		p->h.counter++;
		if ((p->h.flags & 1) && p->h.children == 0)
		{
			ReleaseLinkedTask(p);
			return TASK_END;
		}
		return 0;
	}

	static void SpawnColumn(ModelNode *r)
	{
		ModelNode *c = (ModelNode *)AddEffectTask(QModels(), game_addr(ORIG_Column), 0x7C, r);
		c->radius = 0x1000;
		c->flash_on = 0;
		c->flagged = r->flagged;
		c->arg = r->arg;
		CopyAnchorXZFromSource(c, c->h.pos);
		c->h.pos[1] = (int16_t)(c->h.pos[1] - 0x280);
		ff8fx::copy<(8)>(c->base, c->h.pos);
		c->orbit = r->orbit;
		c->angle[1] = r->orbit;
		c->arg = r->arg;
		c->model = game_addr(MODEL_Column);
		c->fade = 0x1000;
		c->scale[0] = 0x10;
		c->scale[1] = 0x10;
		c->scale[2] = 0x10;
	}

	static void SpawnMote(ModelNode *r)
	{
		if (r->h.counter < 0 || r->h.counter > 6) return;
		SpriteNode *p = (SpriteNode *)AddEffectTask(QSprites(), game_addr(ORIG_Mote), 0x70, r);
		ff8fx::copy<(8)>(p->base, p->h.pos);
		p->seq = game_addr(SEQ_Mote);
		p->radius = (int16_t)((CrtRand() & 0x1FF) + 0x200);
		p->last = 0xC;
		p->ay = (int16_t)(CrtRand() & 0xFFF);
		p->scale[0] = 0x1000;
		p->scale[1] = 0x1000;
		p->scale[2] = 0x1000;
	}

	static void RunePhase(ModelNode *n)
	{
		switch (n->h.phase)
		{
		case 0:
			n->fade = (int16_t)(n->fade - 0x200);
			*(uint8_t *)&n->h.flags |= 2;
			if (n->fade <= 0)
			{
				n->fade = 0;
				n->h.phase++;
			}
			break;
		case 1:
			n->fade = (int16_t)(n->fade + 0x200);
			n->scale[0] -= 0x200;
			n->scale[1] -= 0x200;
			n->scale[2] -= 0x200;
			if (n->fade >= 0x1000)
			{
				n->fade = 0x1000;
				n->scale[0] = 0x10;
				n->scale[1] = 0x10;
				n->scale[2] = 0x10;
				n->h.flags = (uint16_t)((n->h.flags & 0xFFFD) | 5);
				n->h.phase++;
			}
			break;
		default: break;
		}
	}

	static void RuneMotion(ModelNode *n)
	{
		n->angle[1] = (int16_t)((n->angle[1] + 0x140) & 0xFFF);
		ff8fx::copy<(8)>(n->h.pos, n->base);
		n->h.pos[0] = (int16_t)(n->h.pos[0] + ComputeSin(n->orbit) / 4);
		n->h.pos[2] = (int16_t)(n->h.pos[2] + ComputeCos(n->orbit) / 4);
	}

	static uint32_t __cdecl RuneTask(TaskNode *t)
	{
		ModelNode *n = (ModelNode *)t;
		RunePhase(n);
		RuneMotion(n);
		if (n->h.flags & 2)
		{
			if (n->h.counter == 3) SpawnColumn(n);
			SpawnMote(n);
		}
		DrawModel(n);
		n->h.counter++;
		if ((n->h.flags & 1) && n->h.children == 0)
		{
			ReleaseLinkedTask(n);
			return TASK_END;
		}
		return 0;
	}

	static void SpawnRune(SeqNode *s, int32_t angle, int16_t arg, int16_t flagged)
	{
		ModelNode *r = (ModelNode *)AddEffectTask(QModels(), game_addr(ORIG_Rune), 0x7C, s);
		CopyAnchorXZFromSource(r, r->h.pos);
		r->h.pos[1] = (int16_t)(r->h.pos[1] - 0x280);
		ff8fx::copy<(8)>(r->base, r->h.pos);
		const int16_t a = (int16_t)(angle & 0xFFF);
		r->arg = arg;
		r->flagged = flagged;
		r->orbit = a;
		r->model = game_addr(MODEL_Rune);
		r->fade = 0x1000;
		r->scale[0] = 0x1000;
		r->scale[1] = 0x1000;
		r->scale[2] = 0x1000;
		SpriteNode *g = (SpriteNode *)AddEffectTask(QSprites(), game_addr(ORIG_Glow), 0x70, s);
		CopyAnchorXZFromSource(g, g->h.pos);
		ff8fx::copy<(8)>(g->base, g->h.pos);
		g->angle = a;
	}

	static uint32_t __cdecl SequencerTask(TaskNode *t)
	{
		SeqNode *s = (SeqNode *)t;
		switch (s->h.phase)
		{
		case 0:
			if (s->h.counter >= 0)
			{
				SpawnRune(s, 0, 0, 0);
				SpawnRune(s, 0x800, 0, 0);
				s->h.phase++;
			}
			break;
		case 1:
			if (s->h.counter >= 3)
			{
				SpawnRune(s, 0x200, 0, 0);
				SpawnRune(s, 0xA00, 0, 0);
				s->h.phase++;
			}
			break;
		case 2:
			if (s->h.counter >= 6)
			{
				SpawnRune(s, 0x400, 0, 0);
				SpawnRune(s, 0xC00, 0, 0);
				s->h.phase++;
			}
			break;
		case 3:
			if (s->h.counter >= 9)
			{
				SpawnRune(s, 0x600, 0, 0);
				SpawnRune(s, 0xE00, 0, 1);
				*(uint8_t *)&s->h.flags |= 1;
				s->h.phase++;
			}
			break;
		default: break;
		}
		s->h.counter++;
		if ((s->h.flags & 1) && s->h.children == 0)
		{
			ReleaseLinkedTask(s);
			return TASK_END;
		}
		return 0;
	}

	static void SpawnFlash(EmitterNode *e)
	{
		const uint8_t *targets = e->h.ctx->actions[e->h.action].targets;
		if (targets[3] & 4) return;
		FlashNode *f = (FlashNode *)AddEffectTask(QFlashes(), game_addr(ORIG_Flash), 0x50, e);
		CopyAnchorTopFromSource(f, f->h.pos);
		ff8fx::copy<(8)>(f->target, f->h.pos);
		f->tx = 0x240;
		f->ty = 0x120;
		f->cx = 0x140;
		f->cy = 0xF4;
		f->u = 0;
		f->v = 0x20;
		f->tw = 0x28;
		f->th = 0x10;
	}

	static void FlashPhase(FlashNode *f)
	{
		switch (f->h.phase)
		{
		case 0:
			f->vel = (int16_t)-0x90;
			f->h.phase++;
			break;
		case 1:
			f->vel = (int16_t)(f->vel + 0x28);
			f->h.pos[1] = (int16_t)(f->h.pos[1] + f->vel);
			if (f->h.pos[1] >= f->target[1])
			{
				f->h.pos[1] = f->target[1];
				f->h.phase++;
			}
			break;
		case 2:
			if (f->h.counter >= 0xF)
			{
				*(uint8_t *)&f->h.flags |= 1;
				f->h.phase++;
			}
			break;
		default: break;
		}
	}

	static void FlashDraw(const FlashNode *f)
	{
		uint8_t *p = (uint8_t *)PacketCursor();
		const int32_t ty = f->ty;
		uint32_t tp = ((uint32_t)(uint16_t)f->tx & 0x3C0) | 0x2800;
		tp = (uint32_t)(uint16_t)((int16_t)tp >> 6);
		tp |= (uint32_t)(ty >> 4) & 0x10;
		tp |= ((uint32_t)ty & 0x200) << 2;
		F<uint16_t>(p, 0x16) = (uint16_t)tp;
		const uint8_t v = (uint8_t)f->v;
		F<uint8_t>(p, 0xD) = v;
		F<uint16_t>(p, 0xE) = (uint16_t)((((uint32_t)(uint16_t)f->cx >> 4) & 0x3F) | (uint16_t)((uint16_t)f->cy << 6));
		const uint8_t u = (uint8_t)f->u;
		F<uint8_t>(p, 0xC) = u;
		F<uint8_t>(p, 0x1C) = u;
		const uint8_t u1 = (uint8_t)(u + (uint8_t)f->tw);
		F<uint8_t>(p, 0x15) = v;
		const uint8_t v1 = (uint8_t)(v + (uint8_t)f->th);
		F<uint32_t>(p, 0) = 0x9000000;
		F<uint8_t>(p, 7) = 0x2F;
		F<uint8_t>(p, 0x14) = u1;
		F<uint8_t>(p, 0x1D) = v1;
		F<uint8_t>(p, 0x24) = u1;
		F<uint8_t>(p, 0x25) = v1;
		GteSetTransVectorCtrl(&Camera());
		GteSetRotMatrixCtrl(&Camera());
		GteLoadV0(f->h.pos);
		GteRTPS();
		int16_t sxy[4];
		GteReadSXY2(sxy);
		sxy[0] = (int16_t)(sxy[0] >> 3);
		sxy[1] = (int16_t)(sxy[1] >> 3);
		const int32_t half = f->tw / 2;
		const int16_t hh = f->th;
		F<int16_t>(p, 8) = (int16_t)(sxy[0] - half);
		F<int16_t>(p, 0xA) = (int16_t)(sxy[1] - hh);
		F<int16_t>(p, 0x10) = (int16_t)(sxy[0] + half);
		F<int16_t>(p, 0x12) = (int16_t)(sxy[1] - hh);
		F<int16_t>(p, 0x18) = (int16_t)(sxy[0] - half);
		F<int16_t>(p, 0x1A) = sxy[1];
		F<int16_t>(p, 0x20) = (int16_t)(sxy[0] + half);
		F<int16_t>(p, 0x22) = sxy[1];
		InsertPrimAltViewport(var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)), p);
		PacketCursor() = (uint32_t)(p + 0x28);
	}

	static uint32_t __cdecl FlashTask(TaskNode *t)
	{
		FlashNode *f = (FlashNode *)t;
		FlashPhase(f);
		FlashDraw(f);
		f->h.counter++;
		if ((f->h.flags & 1) && f->h.children == 0)
		{
			ReleaseLinkedTask(f);
			return TASK_END;
		}
		return 0;
	}

	static uint32_t __cdecl EmitterTask(TaskNode *t)
	{
		EmitterNode *e = (EmitterNode *)t;
		EmitterUpdatePos(e);
		ComputeModelBounds(e);
		switch (e->h.phase)
		{
		case 0:
			AddEffectTask(QSequencers(), game_addr(ORIG_Sequencer), 0x40, e);
			e->h.phase++;
			break;
		case 1:
			if (e->h.counter >= 0x18)
			{
				((RootNode *)e->h.root)->busy = 0;
				SpawnFlash(e);
				e->h.phase++;
			}
			break;
		case 2:
			ApplyActionResultToTarget(e->h.ctx->actions[e->h.action].targets);
			*(uint8_t *)&e->h.flags |= 1;
			e->h.phase++;
			break;
		default: break;
		}
		if (e->h.counter == 0)
		{
			int16_t mid[4];
			CopyMidpointFromSource(e, mid);
			BdPlaySE(SOUND_Triple(), 0, 0x80);
		}
		switch (e->signal)
		{
		case 1:
			ScreenFadeTask(0, 1, 2, 0x80);
			SpawnBigRune(e);
			e->signal++;
			break;
		case 2:
			e->signal = 0;
			break;
		default: break;
		}
		e->h.counter++;
		if ((e->h.flags & 1) && e->h.children == 0)
		{
			ReleaseLinkedTask(e);
			return TASK_END;
		}
		return 0;
	}

	static void RootPhase(RootNode *r)
	{
		switch (r->h.phase)
		{
		case 0:
			r->h.phase++;
			break;
		case 1:
			r->busy = 1;
			AddEffectTask(QEmitters(), game_addr(ORIG_Emitter), 0x5C, r);
			r->h.phase++;
			break;
		case 2:
			if (r->busy == 0)
			{
				const int8_t a = r->h.action;
				if ((int16_t)a < r->last_action)
				{
					r->h.action = (int8_t)(a + 1);
					r->h.phase--;
				}
				else r->h.phase++;
			}
			break;
		case 3:
			if (r->alive == 0)
			{
				*(uint8_t *)&r->h.flags |= 1;
				r->h.phase++;
			}
			break;
		default: break;
		}
	}

	static uint32_t __cdecl RootTask(TaskNode *t)
	{
		RootNode *r = (RootNode *)t;
		if (r->tick & 1) PacketCursor() = var<uint32_t>(game_addr(ARENA_A));
		else PacketCursor() = var<uint32_t>(game_addr(ARENA_B));
		UpdateTargetPosFromBones(r);
		RootPhase(r);
		r->alive = (int16_t)ExecuteTaskQueue(QEmitters());
		r->alive = (int16_t)(r->alive + ExecuteTaskQueue(QSequencers()));
		r->alive = (int16_t)(r->alive + ExecuteTaskQueue(QSprites()));
		r->alive = (int16_t)(r->alive + ExecuteTaskQueue(QModels()));
		r->alive = (int16_t)(r->alive + ExecuteTaskQueue(QFlashes()));
		r->tick++;
		r->h.counter++;
		if ((r->h.flags & 1) && r->h.children == 0)
		{
			ReleaseLinkedTask(r);
			return TASK_END;
		}
		return 0;
	}
}

	void register_mag021_triple()
	{
		register_port(triple021::ORIG_Root, (void *)triple021::RootTask, "021 Root", 21);
		register_port(triple021::ORIG_Emitter, (void *)triple021::EmitterTask, "021 Emitter", 21);
		register_port(triple021::ORIG_Sequencer, (void *)triple021::SequencerTask, "021 Sequencer", 21);
		register_port(triple021::ORIG_Rune, (void *)triple021::RuneTask, "021 Rune", 21);
		register_port(triple021::ORIG_Column, (void *)triple021::ColumnTask, "021 Column", 21);
		register_port(triple021::ORIG_Spark, (void *)triple021::SparkTask, "021 Spark", 21);
		register_port(triple021::ORIG_Glow, (void *)triple021::GlowTask, "021 Glow", 21);
		register_port(triple021::ORIG_Mote, (void *)triple021::MoteTask, "021 Mote", 21);
		register_port(triple021::ORIG_BigRune, (void *)triple021::BigRuneTask, "021 BigRune", 21);
		register_port(triple021::ORIG_Morph, (void *)triple021::MorphTask, "021 Morph", 21);
		register_port(triple021::ORIG_Sprite, (void *)triple021::SpriteTask, "021 Sprite", 21);
		register_port(triple021::ORIG_Flash, (void *)triple021::FlashTask, "021 Flash", 21);
	}
}

