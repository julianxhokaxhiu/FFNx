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

// Effect 132: Dissolving Acid

#include "mag_common.h"
#include "mag_ice_common.h"

namespace ff8fx
{
namespace acid132
{
	using namespace eng;
	using namespace magc;
	using namespace icec;

	inline uint8_t *&Caster() { return var<uint8_t *>(game_addr(game::g_Mag132_Caster)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::g_Mag132_MagicBuffer)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::g_Mag132_CastContext)); }
	inline uint8_t *&DropHeader() { return var<uint8_t *>(game_addr(game::g_Mag132_DropHeader)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::g_Mag132_PacketCursor)); }
	inline TaskQueue *QDrops() { return (TaskQueue *)game_addr(game::g_Mag132_QDrops); }
	inline TaskQueue *QUnused() { return (TaskQueue *)game_addr(game::g_Mag132_QUnused); }
	inline TaskQueue *QStreams() { return (TaskQueue *)game_addr(game::g_Mag132_QStreams); }
	inline TaskQueue *QLayouts() { return (TaskQueue *)game_addr(game::g_Mag132_QLayouts); }
	inline TaskQueue *QTargets() { return (TaskQueue *)game_addr(game::g_Mag132_QTargets); }

	static constexpr game::Id ORIG_MasterTask = game::MAG_132_DissolvingAcid_MasterTask;
	static constexpr game::Id ORIG_TargetTask = game::MAG_132_DissolvingAcid_TargetTask;
	static constexpr game::Id ORIG_StreamTask = game::MAG_132_DissolvingAcid_StreamTask;
	static constexpr game::Id ORIG_DropTask = game::MAG_132_DissolvingAcid_DropTask;
	static constexpr game::Id ORIG_SplashTask = game::MAG_132_DissolvingAcid_SplashTask;
	static constexpr game::Id ORIG_PuddleTask = game::MAG_132_DissolvingAcid_PuddleTask;
	static const uint32_t POINTS = 0x3BBC;
	static const uint32_t MORPH = 0x49BC;
	static constexpr game::Id MODEL_Drop = game::MAG_132_DissolvingAcid_ModelDrop;
	static constexpr game::Id SEQ_Stream = game::MAG_132_DissolvingAcid_SeqStream;
	static constexpr game::Id SEQ_Splash1 = game::MAG_132_DissolvingAcid_SeqSplash1;
	static constexpr game::Id SEQ_Splash2 = game::MAG_132_DissolvingAcid_SeqSplash2;
	static constexpr game::Id MODEL_Splash1 = game::MAG_132_DissolvingAcid_ModelSplash1;
	static constexpr game::Id MODEL_Splash2 = game::MAG_132_DissolvingAcid_ModelSplash2;
	static constexpr game::Id MODEL_Puddle = game::MAG_132_DissolvingAcid_ModelPuddle;
	static const void *SOUND_Acid() { return (const void *)game_addr(game::MAG_132_DissolvingAcid_SoundAcid); }
	static const uint8_t *STREAM_TABLE() { return (const uint8_t *)game_addr(game::Mag132_StreamTable); }
	static const int16_t *WEIGHTS() { return (const int16_t *)game_addr(game::Mag132_StreamWeights); }
	static const uint8_t *DIST_TABLE() { return (const uint8_t *)game_addr(game::Mag132_RibbonDistTable); }

	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void GteMVMVA_RotV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0))(); }
	inline void MatrixMultiplyVector(const Mat4x3 *m, const int16_t *in, int16_t *out) { fn<void (__cdecl *)(const Mat4x3 *, const int16_t *, int16_t *)>(game_addr(game::matrixMultiplyVector))(m, in, out); }
	inline void StoreOTZ4(int16_t *dst) { fn<void (__cdecl *)(int16_t *)>(game_addr(game::GTE_ReadSZ3Quarter))(dst); }
	inline uint32_t DrawDropSprite(void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::MAG_278_DrawBeamSprites))(h, ot, mode, cursor); }

#pragma pack(push, 1)
	struct MasterNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t action;
		uint8_t started;
		uint32_t arena;
	};
	struct TargetNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t action;
		uint8_t *entity;
	};
	struct Point
	{
		int16_t pos[3];
		int16_t width;
		int16_t sx, sy;
		int16_t otz;
		int16_t idx;
		int16_t sx2, sy2;
		uint8_t pad14[4];
		Point *next;
	};
	struct StreamNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t count;
		Point *head;
		Point *tail;
		int16_t start[3];
		int16_t phase;
		int16_t ctrl[4];
		int16_t end[4];
		uint8_t pad30[8];
		int16_t slot;
		int16_t width;
		int16_t add;
		int16_t pad3E;
	};
	struct DropNode
	{
		TaskNode hdr;
		int16_t pos[3];
		int16_t frame;
		int16_t vel[3];
		int16_t pad1A;
	};
	struct LayoutNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		int16_t pos[4];
		uint8_t layout[0x2C0];
	};
	struct PrimArg
	{
		int16_t pos[4];
		uint32_t morph;
	};
#pragma pack(pop)
	static_assert(sizeof(MasterNode) == 0x14 && sizeof(TargetNode) == 0x14 && sizeof(Point) == 0x1C && sizeof(StreamNode) == 0x40, "Dissolving Acid nodes");
	static_assert(sizeof(DropNode) == 0x1C && sizeof(LayoutNode) == 0x2D8 && sizeof(PrimArg) == 0xC, "Dissolving Acid nodes");

	static const int16_t TARGET_PAD = 0;

	static void __cdecl PartCallback(prim::Layout *l, prim::Record *r, int arg_)
	{
		const PrimArg *arg = (const PrimArg *)arg_;
		if (((int32_t)r->scale[2] | *(const int32_t *)r->scale) == 0) return;
		if (r->a >= 0x1000 && *(const uint32_t *)r->rgb == 0) return;
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
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
			GteSetRotMatrixCtrl(&Camera());
			GteLoadV0(arg->pos);
			GteMVMVA_RotV0();
			GteReadMAC123(m.t);
			m.t[1] = (int32_t)((uint32_t)m.t[1] + (uint32_t)(int32_t)off[1]);
			m.t[0] = (int32_t)((uint32_t)m.t[0] + (uint32_t)(int32_t)off[0]);
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
		m.t[1] = (int32_t)((uint32_t)m.t[1] + (uint32_t)Camera().t[1]);
		m.t[0] = (int32_t)((uint32_t)m.t[0] + (uint32_t)Camera().t[0]);
		m.t[2] = (int32_t)((uint32_t)m.t[2] + (uint32_t)Camera().t[2]);
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
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static void SequenceAt(uint8_t *h, const int16_t *pos, int32_t a, int32_t b, uint32_t seq, int16_t frame)
	{
		TransformCameraByShadowRotation(pos, a, b);
		*(uint32_t *)h = seq;
		*(int16_t *)(h + 4) = frame;
		*(uint16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
	}

	static void SplashSequences(const LayoutNode *s)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		if (s->counter < 9) SequenceAt(h, s->pos, 0x800, -0x200, game_addr(SEQ_Splash1), s->counter);
		const int32_t e = (int32_t)s->counter - 4;
		if ((uint32_t)e < 0xC) SequenceAt(h, s->pos, 0x800, -0x200, game_addr(SEQ_Splash2), (int16_t)e);
		FieldFree(0xB4);
	}

	static void StreamSequence(const StreamNode *s)
	{
		TransformCameraByShadowRotation(s->start, 0x1000, -0x400);
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		*(uint32_t *)h = game_addr(SEQ_Stream);
		*(int16_t *)(h + 4) = s->counter;
		*(uint16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static uint8_t *DropListHeader()
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		DropHeader() = h;
		*(uint32_t *)h = game_addr(MODEL_Drop);
		const uint8_t *model = *(const uint8_t **)h;
		*(uint16_t *)(h + 0x92) = 0;
		*(uint16_t *)(h + 0x90) = 0;
		*(uint16_t *)(h + 0x8E) = 0;
		*(uint16_t *)(h + 0x8A) = 0;
		*(uint16_t *)(h + 0x88) = 0;
		*(uint16_t *)(h + 0x86) = 0;
		*(uint16_t *)(h + 0x94) = 0x400;
		*(uint16_t *)(h + 0x8C) = 0x400;
		*(uint16_t *)(h + 0x84) = 0x400;
		*(uint16_t *)(h + 0x60) = 0;
		*(uint16_t *)(h + 0x58) = 0;
		*(uint16_t *)(h + 0x50) = 0;
		*(uint16_t *)(h + 0x48) = 0;
		h[0x20] = model[4];
		h[0x21] = model[5];
		h[0x22] = model[6];
		h[0x23] = model[7];
		return h;
	}

	static void DropDraw(uint8_t *h, const DropNode *d)
	{
		GteSetRotMatrixCtrl(&Camera());
		GteLoadV0(d->pos);
		GteMVMVA_RotV0();
		int32_t *t = (int32_t *)(h + 0x98);
		GteReadMAC123(t);
		t[0] = (int32_t)((uint32_t)t[0] + (uint32_t)Camera().t[0]);
		t[1] = (int32_t)((uint32_t)t[1] + (uint32_t)Camera().t[1]);
		t[2] = (int32_t)((uint32_t)t[2] + (uint32_t)Camera().t[2]);
		uint8_t *model = *(uint8_t **)h;
		uint8_t *frame = model + *(const uint16_t *)(model + 0xA + (int32_t)d->frame * 2);
		*(uint8_t **)(h + 0x2C) = frame;
		*(uint32_t *)(h + 0x30) = *(const uint32_t *)frame;
		*(uint8_t **)(h + 0x2C) = frame + 4;
		PacketCursor() = DrawDropSprite(h, RenderOT(), 2, PacketCursor());
	}

	static Point *AllocPoint(int16_t width)
	{
		Point *p = (Point *)(TexBase() + POINTS);
		for (int32_t i = 0x80; i; i--, p++)
		{
			if (p->width == -1)
			{
				p->next = nullptr;
				p->width = width;
				return p;
			}
		}
		return nullptr;
	}

	static void BezierPoint(const StreamNode *s, int32_t w, int16_t *out)
	{
		const int32_t iw = 0x1000 - w;
		int16_t tmp[3];
		GteSetIR0(iw);
		GteLoadIR123(s->start);
		GteGPF();
		GteSetIR0(w);
		GteLoadIR123(s->ctrl);
		GteGPL();
		GteStoreIR123(tmp);
		GteSetIR0(iw);
		GteLoadIR123(tmp);
		GteGPF();
		GteSetIR0(w);
		GteLoadIR123(s->end);
		GteGPL();
		GteStoreIR123(out);
	}

	static uint32_t ProjectChain(Point *p)
	{
		GteSetRotMatrix(&Camera());
		GteSetTransVector(&Camera());
		do
		{
			GteLoadV0(p);
			GteRTPS();
			GteReadSXY2(&p->sx);
			StoreOTZ4(&p->otz);
			p = p->next;
		} while (p);
		return var<uint32_t>(game_addr(game::GTE_SZ3));
	}

	static void SegEdges(Point *s, const Point *n, const int16_t *prev_dir, int16_t *dir)
	{
		if (s->otz < 0) return;
		int32_t ox = 0, oy = 0;
		if (n)
		{
			if (n->otz < 0) return;
			int32_t dx = ScreenPx(n->sx) - ScreenPx(s->sx);
			int32_t ndy = ScreenPx(s->sy) - ScreenPx(n->sy);
			int32_t adx = dx < 0 ? -dx : dx, ady = ndy < 0 ? -ndy : ndy;
			if (adx >= 0x80) adx = 0x7F;
			if (ady >= 0x80) ady = 0x7F;
			int32_t len = DIST_TABLE()[ady * 128 + adx];
			int32_t px, py;
			if (prev_dir)
			{
				if (len == 0)
				{
					dir[0] = prev_dir[0];
					dir[1] = prev_dir[1];
					px = prev_dir[0];
					py = prev_dir[1];
				}
				else
				{
					int32_t cx = shl32(ndy, 12) / len, cy = shl32(dx, 12) / len;
					int32_t sx = cx + prev_dir[0], sy = cy + prev_dir[1];
					dir[0] = (int16_t)cx;
					dir[1] = (int16_t)cy;
					if (sx == 0 && sy == 0) { sx = cx * 2; sy = cy * 2; }
					int32_t sq = (int32_t)((uint32_t)mul32(sx, sx) + (uint32_t)mul32(sy, sy));
					int32_t k = 0x4000 - (sq >> 13);
					px = mul32(k, sx) >> 13;
					py = mul32(k, sy) >> 13;
				}
			}
			else
			{
				if (len == 0) { s->otz = -1; return; }
				px = shl32(ndy, 12) / len;
				py = shl32(dx, 12) / len;
				dir[0] = (int16_t)px;
				dir[1] = (int16_t)py;
			}
			int32_t w = shl32(n->width, 5) / (s->otz + 0x200);
			ox = ScreenEighths(mul32(w, px) >> 12);
			oy = ScreenEighths(mul32(w, py) >> 12);
		}
		int16_t x = s->sx, y = s->sy;
		s->sx = (int16_t)(x - ox);
		s->sx2 = (int16_t)(x + ox);
		s->sy2 = (int16_t)(y + oy);
		s->sy = (int16_t)(y - oy);
	}

	static void Ribbon(Point *head, int32_t count, uint32_t slot)
	{
		int16_t dir[2] = { (int16_t)slot, (int16_t)(slot >> 16) };
		SegEdges(head, head->next, nullptr, dir);
		Point *s = head->next;
		for (int32_t i = count - 2; i; i--)
		{
			SegEdges(s, s->next, dir, dir);
			s = s->next;
		}
		SegEdges(s, nullptr, dir, nullptr);

		Point *a = head;
		for (int32_t i = count - 1; i; i--)
		{
			Point *b = a->next;
			if (a->otz > 0 && b->otz > 0 && a->idx <= 7)
			{
				uint32_t *p = (uint32_t *)PacketCursor();
				PacketCursor() += 0x34;
				p[2] = *(const uint32_t *)&a->sx;
				p[5] = *(const uint32_t *)&a->sx2;
				const uint32_t c = (uint32_t)mul32((uint16_t)(a->idx + 1), 7) << 13;
				uint32_t v = (c + 0x2000) & 0xFFFF;
				if (v > 0xFF00) v = 0xFF00;
				p[6] = v | 0xB90078;
				p[3] = v | 0x3C140048;
				*(uint16_t *)((uint8_t *)p + 0x24) = (uint16_t)(c | 0x48);
				*(uint16_t *)((uint8_t *)p + 0x30) = (uint16_t)(c | 0x78);
				p[8] = *(const uint32_t *)&b->sx;
				p[1] = 0x808080;
				p[11] = *(const uint32_t *)&b->sx2;
				p[4] = 0x808080;
				p[10] = 0x808080;
				p[7] = 0x808080;
				p[0] = 0xC000000;
				((uint8_t *)p)[7] = 0x3E;
				InsertPrimAutoDepth(var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + (((int32_t)b->otz + (int32_t)a->otz) >> 3) * 4 + 0x44, p);
			}
			a = b;
		}
	}

	static void SpawnSplash(const int16_t *end)
	{
		LayoutNode *s = (LayoutNode *)AddTaskToQueue(QLayouts(), game_addr(ORIG_SplashTask));
		s->counter = 0;
		*(uint32_t *)&s->pos[0] = *(const uint32_t *)&end[0];
		*(uint32_t *)&s->pos[2] = *(const uint32_t *)&end[2];
		if (CrtRand() & 1) DecodeModelPrimLayout(game_addr(MODEL_Splash1), s->layout, 0xB0);
		else DecodeModelPrimLayout(game_addr(MODEL_Splash2), s->layout, 0xB0);
	}
	static void SpawnPuddle(const int16_t *end)
	{
		LayoutNode *s = (LayoutNode *)AddTaskToQueue(QLayouts(), game_addr(ORIG_PuddleTask));
		s->counter = 0;
		*(uint32_t *)&s->pos[0] = *(const uint32_t *)&end[0];
		*(uint32_t *)&s->pos[2] = *(const uint32_t *)&end[2];
		DecodeModelPrimLayout(game_addr(MODEL_Puddle), s->layout, 0x78);
	}

	static void SpawnStream(const int16_t *a1, int16_t *a2, int32_t slot, int32_t width, const int16_t *v, const int16_t *entry)
	{
		Point *p = AllocPoint((int16_t)width);
		if (!p) return;
		StreamNode *s = (StreamNode *)AddTaskToQueue(QStreams(), game_addr(ORIG_StreamTask));
		if (!s)
		{
			p->width = -1;
			return;
		}
		p->idx = 0;
		if (entry[3] == 0)
		{
			int16_t d[4] = { (int16_t)(entry[0] >> 1), 0, 0, 0 };
			MatrixMultiplyVector((const Mat4x3 *)(Caster() + 0x40), d, d);
			a2[0] = (int16_t)(a2[0] + d[0]);
			a2[2] = (int16_t)(a2[2] + d[2]);
			a2[1] = 0;
			a2[3] = 1;
		}
		else a2[3] = 0;
		s->counter = 0;
		ff8fx::copy<(8)>(s->start, a1);
		ff8fx::copy<(8)>(s->ctrl, a1);
		s->ctrl[0] = (int16_t)(s->ctrl[0] + (int16_t)(v[0] >> 1));
		s->ctrl[1] = (int16_t)(s->ctrl[1] + v[1]);
		s->ctrl[2] = (int16_t)(s->ctrl[2] + (int16_t)(v[2] >> 1));
		ff8fx::copy<(8)>(s->end, a2);
		s->tail = p;
		s->head = p;
		s->phase = 0;
		s->count = 1;
		s->slot = (int16_t)slot;
		s->add = 8;
		s->width = (int16_t)width;
	}

	static void BonePoint(const int16_t *entry, int16_t *out)
	{
		uint8_t *c = Caster();
		Mat4x3 m = {};
		ComposeAffineTransform((const Mat4x3 *)(c + 0x40), (const Mat4x3 *)(**(uint8_t ***)(c + 0x64) + 0x20 + (int32_t)entry[3] * 0x30), &m);
		GteSetRotMatrixCtrl(&m);
		GteSetTransVectorCtrl(&m);
		GteLoadV0(entry);
		GteMVMVA_RotV0Tr();
		GteStoreIR123(out);
	}
}
}

namespace ff8fx
{
namespace acid132
{
	static uint32_t __cdecl DropTask(TaskNode *n)
	{
		DropNode *d = (DropNode *)n;
		DropDraw(DropHeader(), d);
		d->frame++;
		if (d->frame >= 8) return TASK_END;
		d->pos[1] = (int16_t)(d->pos[1] + d->vel[1]);
		d->pos[2] = (int16_t)(d->pos[2] + d->vel[2]);
		d->pos[0] = (int16_t)(d->pos[0] + d->vel[0]);
		return 0;
	}

	static uint32_t RunDrops()
	{
		DropListHeader();
		TaskQueue *q = QDrops();
		TaskNode *prev = nullptr;
		uint32_t alive = 0;
		for (TaskNode *t = q->head; t; t = t->next)
		{
			const uint32_t r = t->func(t);
			if (r & 2)
			{
				t->flags = 0;
				if (prev) prev->next = t->next;
				else q->head = t->next;
			}
			else
			{
				prev = t;
				alive++;
			}
		}
		q->tail = prev;
		FieldFree(0xB4);
		return alive;
	}

	static uint32_t __cdecl SplashTask(TaskNode *n)
	{
		LayoutNode *s = (LayoutNode *)n;
		PrimArg arg;
		ff8fx::copy<(sizeof(arg.pos))>(arg.pos, s->pos);
		arg.morph = TexBase() + MORPH;
		prim::play((prim::Layout *)s->layout, PartCallback, (int)&arg, 0);
		SplashSequences(s);
		s->counter++;
		return s->counter < 0x10 ? 0 : TASK_END;
	}

	static uint32_t __cdecl PuddleTask(TaskNode *n)
	{
		LayoutNode *s = (LayoutNode *)n;
		PrimArg arg;
		ff8fx::copy<(sizeof(arg.pos))>(arg.pos, s->pos);
		arg.morph = TexBase() + MORPH;
		return prim::play((prim::Layout *)s->layout, PartCallback, (int)&arg, 0) ? 0 : TASK_END;
	}

	static uint32_t __cdecl StreamTask(TaskNode *n)
	{
		StreamNode *s = (StreamNode *)n;
		bool grow = true;
		if (s->add > 0)
		{
			Point *p = AllocPoint(s->width);
			if (!p)
			{
				s->phase = 0x1000;
				grow = false;
			}
			else
			{
				p->idx = s->count;
				s->count++;
				s->tail->next = p;
				s->add--;
				s->tail = p;
			}
		}
		if (grow)
		{
			s->phase = (int16_t)(s->phase + 0x100);
			Point *p = s->head;
			int32_t w = WEIGHTS()[(int32_t)s->phase / 256];
			do
			{
				w -= 0xC0;
				if (w > 0x1080 && p == s->head)
				{
					if (p->idx == 0)
					{
						if (s->end[3] == 0) SpawnSplash(s->end);
						else SpawnPuddle(s->end);
					}
					Point *next = p->next;
					p->width = -1;
					s->count--;
					s->head = next;
					if (s->count < 2) return TASK_END;
				}
				if (w >= 0x1080) w = 0x1000;
				BezierPoint(s, w, p->pos);
				p = p->next;
			} while (p);

			for (int32_t k = 3; k; k--)
			{
				const int32_t r = CrtRand() & 3;
				Point *a = s->head;
				for (Point *b = a->next; b; a = b, b = b->next)
				{
					if (a->idx != r) continue;
					DropNode *d = (DropNode *)AddTaskToQueue(QDrops(), game_addr(ORIG_DropTask));
					if (!d) continue;
					*(uint32_t *)&d->pos[0] = *(const uint32_t *)&a->pos[0];
					*(uint32_t *)&d->pos[2] = *(const uint32_t *)&a->pos[2];
					d->frame = 0;
					int32_t q = CrtRand();
					d->vel[0] = (int16_t)((q & 0x7F) + (((int32_t)b->pos[0] - (int32_t)a->pos[0]) >> 1) - 0x40);
					q = CrtRand();
					d->vel[1] = (int16_t)((q & 0x7F) + (((int32_t)b->pos[1] - (int32_t)a->pos[1]) >> 1) - 0x40);
					q = CrtRand();
					d->vel[2] = (int16_t)((q & 0x7F) + (((int32_t)b->pos[2] - (int32_t)a->pos[2]) >> 1) - 0x40);
				}
			}
		}
		const uint32_t slot = ProjectChain(s->head);
		if (s->count >= 2) Ribbon(s->head, s->count, slot);
		if (s->counter < 8)
		{
			StreamSequence(s);
		}
		s->counter++;
		return 0;
	}

	static uint32_t __cdecl TargetTask(TaskNode *n)
	{
		TargetNode *t = (TargetNode *)n;
		if (t->counter == 0xA) BdPlaySE(SOUND_Acid(), 0, 0x80);
		const uint32_t e = (uint32_t)((int32_t)t->counter - 0x14);
		if (e < 0xA && !(e & 1))
		{
			const int16_t *entry = (const int16_t *)(STREAM_TABLE() + (e >> 1) * 16);
			int16_t a1[4];
			BonePoint(entry, a1);
			a1[3] = TARGET_PAD;
			uint8_t *ent = t->entity;
			int16_t a2[4];
			const int32_t bone = (int32_t)((uint32_t)CrtRand() * (uint32_t)***(uint8_t ***)(ent + 0x64)) >> 15;
			GetEffectSpawnPosition(ent, bone, 0, a2);
			int16_t v[4];
			MatrixMultiplyVector((const Mat4x3 *)(Caster() + 0x40), entry + 4, v);
			const int32_t slot = (int32_t)((uint32_t)t->entity - game_addr(game::BattleEntitySlotData)) / 0x9C;
			SpawnStream(a1, a2, slot, 0x96, v, entry + 4);
		}
		if (t->counter >= 0x2D)
		{
			ApplyActionResultToTarget(Ctx()->actions[0].targets);
			return TASK_END;
		}
		t->counter++;
		return 0;
	}

	static uint32_t __cdecl MasterTask(TaskNode *n)
	{
		MasterNode *m = (MasterNode *)n;
		if (m->arena)
		{
			PacketCursor() = TexBase() + 0x4BDC;
			m->arena = 0;
		}
		else
		{
			PacketCursor() = TexBase() + 0x14BDC;
			m->arena = 1;
		}
		if (m->counter == 1)
		{
			if (!m->started)
			{
				m->started = 1;
				InitTaskQueuePool(QTargets(), (void *)(TexBase() + 0x3B80), 0x14, 3);
				InitTaskQueuePool(QLayouts(), (void *)(TexBase() + 0xE00), 0x2D8, 0x10);
				InitTaskQueuePool(QStreams(), (void *)(TexBase() + 0xA00), 0x40, 0x10);
				InitTaskQueuePool(QUnused(), (void *)(TexBase() + 0x700), 0x18, 0x20);
				InitTaskQueuePool(QDrops(), (void *)TexBase(), 0x1C, 0x40);
				Point *p = (Point *)(TexBase() + POINTS);
				for (int32_t i = 0x80; i; i--, p++) p->width = -1;
			}
			ActionData *actions = Ctx()->actions;
			if (m->action <= actions[0].last_action)
			{
				uint8_t *ent = Entity(actions[m->action].targets[0]);
				bool busy = false;
				const TargetNode *t = (const TargetNode *)(TexBase() + 0x3B80);
				for (int32_t i = 0; i < 3; i++, t++)
				{
					if ((t->hdr.flags & 1) && t->entity == ent)
					{
						busy = true;
						break;
					}
				}
				if (busy) m->counter = 0;
				else
				{
					TargetNode *nt = (TargetNode *)AddTaskToQueue(QTargets(), game_addr(ORIG_TargetTask));
					if (nt)
					{
						Memset32(&nt->counter, 0, 2);
						nt->action = m->action;
						nt->entity = ent;
						m->action++;
					}
				}
			}
		}
		int a0, a1, a2, a3, a4;
		if (m->started)
		{
			a0 = ExecuteTaskQueue(QTargets());
			a1 = ExecuteTaskQueue(QLayouts());
			a2 = ExecuteTaskQueue(QUnused());
			a3 = ExecuteTaskQueue(QStreams());
			a4 = (int)RunDrops();
		}
		else a0 = a1 = a2 = a3 = a4 = (int)n;
		if (m->started && a0 == 0 && a1 == 0 && a2 == 0 && a3 == 0 && a4 == 0) return TASK_END;
		m->counter++;
		return 0;
	}
}

	void register_mag132_dissolving_acid()
	{
		register_port(acid132::ORIG_MasterTask, (void *)acid132::MasterTask, "132 MasterTask", 132);
		register_port(acid132::ORIG_TargetTask, (void *)acid132::TargetTask, "132 TargetTask", 132);
		register_port(acid132::ORIG_StreamTask, (void *)acid132::StreamTask, "132 StreamTask", 132);
		register_port(acid132::ORIG_DropTask, (void *)acid132::DropTask, "132 DropTask", 132);
		register_port(acid132::ORIG_SplashTask, (void *)acid132::SplashTask, "132 SplashTask", 132);
		register_port(acid132::ORIG_PuddleTask, (void *)acid132::PuddleTask, "132 PuddleTask", 132);
	}
}

