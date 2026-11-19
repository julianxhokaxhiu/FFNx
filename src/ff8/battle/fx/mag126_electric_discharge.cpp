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

// Effect 126: Electric Discharge

#include "mag_common.h"

namespace ff8fx
{
namespace disch126
{
	using namespace eng;
	using namespace magc;

	inline uint32_t &FlashLevel() { return var<uint32_t>(game_addr(game::g_Mag126_FlashLevel)); }
	inline uint8_t *&Caster() { return var<uint8_t *>(game_addr(game::g_Mag126_CasterEntity)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::g_Mag126_TexBase)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::g_Mag126_CastContext)); }
	inline uint32_t *SavedTints() { return (uint32_t *)game_addr(game::g_Mag126_SavedPartyTint); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::g_Mag126_PacketCursor)); }
	inline TaskQueue *QLayouts() { return (TaskQueue *)game_addr(game::g_Mag126_LayoutQueue); }
	inline TaskQueue *QSparks() { return (TaskQueue *)game_addr(game::g_Mag126_SparkQueue); }
	inline TaskQueue *QCrawlers() { return (TaskQueue *)game_addr(game::g_Mag126_CrawlerQueue); }
	inline TaskQueue *QArcs() { return (TaskQueue *)game_addr(game::g_Mag126_ArcQueue); }
	inline TaskQueue *QMain() { return (TaskQueue *)game_addr(game::g_Mag126_MainQueue); }
	static constexpr game::Id ZERO_VEC = game::g_Mag126_ZeroVector;
	inline int32_t &FadeChance() { return var<int32_t>(game_addr(game::Mag126_ScreenFadeChance)); }
	inline uint32_t OTBase() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)); }

	static const uint32_t ARC_POINTS = 0x22B4, CRAWL_POINTS = 0x72B4, ARC_FREE = 0xC2B4, CRAWL_FREE = 0xC2B8, MORPH = 0xC2BC;

	static constexpr game::Id ORIG_RootTask = game::MAG_126_ElectricDischarge_RootTask;
	static constexpr game::Id ORIG_MainTask = game::MAG_126_ElectricDischarge_MainTask;
	static constexpr game::Id ORIG_LayoutTask = game::MAG_126_ElectricDischarge_GlowLayoutTask;
	static constexpr game::Id ORIG_ArcTask = game::MAG_126_ElectricDischarge_ArcTask;
	static constexpr game::Id ORIG_SparkTask = game::MAG_126_ElectricDischarge_SparkTask;
	static constexpr game::Id ORIG_CrawlerTask = game::MAG_126_ElectricDischarge_CrawlerTask;
	static constexpr game::Id SEQ_Spark = game::MAG_126_ElectricDischarge_SeqSpark;
	static constexpr game::Id SEQ_SparkB = game::MAG_126_ElectricDischarge_SeqSparkB;
	static constexpr game::Id MODEL_Glow = game::MAG_126_ElectricDischarge_ModelGlow;
	static const void *SOUND_Discharge() { return (const void *)game_addr(game::MAG_126_ElectricDischarge_SoundDischarge); }
	static const void *TINT_Colour() { return (const void *)game_addr(game::unk_1242280); }
	static const uint8_t *BONES_Caster() { return (const uint8_t *)game_addr(game::unk_1242284); }
	static const int16_t *CRAWL_Scripts() { return (const int16_t *)game_addr(game::unk_124228C); }
	static const uint8_t *LEN_Table() { return (const uint8_t *)game_addr(game::unk_1242A0C); }
	static const int16_t *BLEND_Table() { return (const int16_t *)game_addr(game::unk_1246A0C); }

	inline int32_t Rand() { return fn<int32_t (__cdecl *)()>(game_addr(game::Effect_SharedLcg_Rand))(); }
	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t flag, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, flag, out); }
	inline int32_t ComputeCos(int32_t a) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(a); }
	inline void BlendColour(const void *a, const void *b, int32_t wa, int32_t wb, void *out) { fn<void (__cdecl *)(const void *, const void *, int32_t, int32_t, void *)>(game_addr(game::ColourLerp))(a, b, wa, wb, out); }
	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void RotationFromAnglesB(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::MAG_063_RotationFromAnglesYXZ))(angles, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void GteMVMVA_RotV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0))(); }
	inline void GteLoadV0u(const void *v) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadV0_u16))(v); }

#pragma pack(push, 1)
	struct RootNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t pad0e;
		uint8_t started;
		uint32_t arena;
	};
	struct MainNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0e;
		int16_t party;
		int16_t pad12;
	};
	struct Point
	{
		int16_t a[3];
		int16_t b;
		int16_t c[3];
		int16_t d;
		int16_t sxy[2];
		int16_t otz;
		int16_t pad16;
		int16_t edge[2];
		int16_t pad1c[2];
		int16_t fade;
		int16_t step;
		Point *next;
	};
	struct ArcNode
	{
		TaskNode hdr;
		int16_t pad0c;
		int16_t count;
		Point *tail;
		Point *head;
		int16_t pos[3];
		int16_t t;
		int16_t end[3];
		int16_t speed;
		int16_t vel[3];
		int16_t forks;
		int16_t dir[3];
		int16_t slot;
		int16_t width;
		int16_t style;
	};
	struct SparkNode
	{
		TaskNode hdr;
		int16_t frame;
		uint8_t kind;
		int8_t slot;
		int16_t pos[3];
		int16_t scale;
	};
	struct CrawlerNode
	{
		TaskNode hdr;
		int16_t steps;
		int16_t count;
		int16_t width;
		int16_t fade;
		int16_t step;
		int16_t pad16;
		const int16_t *script;
		Point *tail;
		Point *head;
		uint8_t *model;
	};
	struct LayoutNode
	{
		TaskNode hdr;
		uint8_t layout[0xC8];
	};
	struct PrimArg
	{
		int16_t pos[4];
		uint32_t morph;
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x14 && sizeof(MainNode) == 0x14, "Electric Discharge nodes");
	static_assert(sizeof(Point) == 0x28 && sizeof(ArcNode) == 0x3C && sizeof(SparkNode) == 0x18, "Electric Discharge nodes");
	static_assert(sizeof(CrawlerNode) == 0x28 && sizeof(LayoutNode) == 0xD4 && sizeof(PrimArg) == 0xC, "Electric Discharge nodes");

	inline int8_t CrawlDrift(const Point *p, int i) { return ((const int8_t *)p)[0xC + i]; }
	inline int8_t CrawlBone(const Point *p) { return ((const int8_t *)p)[0xF]; }

	static void RibbonEdge(Point *p, const Point *next, const int16_t *prev, int16_t *out, int32_t width, int32_t bias)
	{
		if (p->otz < 0) return;
		int32_t ox, oy;
		if (!next)
		{
			ox = 0;
			oy = 0;
		}
		else
		{
			if (next->otz < 0) return;
			const int32_t dx = ScreenPx(next->sxy[0]) - ScreenPx(p->sxy[0]);
			int32_t ax = dx < 0 ? -dx : dx;
			if (ax >= 0x80) ax = 0x7F;
			const int32_t dy = ScreenPx(p->sxy[1]) - ScreenPx(next->sxy[1]);
			int32_t ay = dy < 0 ? -dy : dy;
			if (ay >= 0x80) ay = 0x7F;
			const int32_t len = LEN_Table()[ay * 128 + ax];
			int32_t ux, uy;
			if (prev)
			{
				if (len == 0)
				{
					out[0] = prev[0];
					out[1] = prev[1];
					ux = prev[0];
					uy = prev[1];
				}
				else
				{
					const int32_t nx = shl32(dy, 12) / len;
					const int32_t ny = shl32(dx, 12) / len;
					int32_t sx = nx + prev[0];
					int32_t sy = ny + prev[1];
					out[0] = (int16_t)nx;
					out[1] = (int16_t)ny;
					if (sx == 0 && sy == 0)
					{
						sx = nx + nx;
						sy = ny + ny;
					}
					const int32_t k = 0x4000 - ((mul32(sx, sx) + mul32(sy, sy)) >> 13);
					ux = mul32(k, sx) >> 13;
					uy = mul32(k, sy) >> 13;
				}
			}
			else
			{
				if (len == 0)
				{
					p->otz = -1;
					return;
				}
				ux = shl32(dy, 12) / len;
				uy = shl32(dx, 12) / len;
				out[0] = (int16_t)ux;
				out[1] = (int16_t)uy;
			}
			const int32_t w = shl32(width, 5) / ((int32_t)p->otz + bias);
			ox = ScreenEighths(mul32(w, ux) >> 12);
			oy = ScreenEighths(mul32(w, uy) >> 12);
		}
		const int16_t x = p->sxy[0], y = p->sxy[1];
		p->sxy[0] = (int16_t)(x - ox);
		p->edge[0] = (int16_t)(x + ox);
		p->edge[1] = (int16_t)(y + oy);
		p->sxy[1] = (int16_t)(y - oy);
	}

	static void RibbonQuads(Point *tail, int32_t count, uint32_t uv0, uint32_t uv1, uint16_t uv2, uint16_t uv3)
	{
		Point *a = tail;
		for (int32_t i = count - 1; i != 0; i--)
		{
			Point *b = a->next;
			if (a->otz > 0 && b->otz > 0)
			{
				uint8_t *pk = (uint8_t *)PacketCursor();
				PacketCursor() += 0x34;
				*(uint32_t *)(pk + 8) = *(const uint32_t *)a->sxy;
				*(uint32_t *)(pk + 0x14) = *(const uint32_t *)a->edge;
				*(uint32_t *)(pk + 0xC) = uv0;
				*(uint32_t *)(pk + 0x18) = uv1;
				*(uint16_t *)(pk + 0x24) = uv2;
				*(uint16_t *)(pk + 0x30) = uv3;
				*(uint32_t *)(pk + 0x20) = *(const uint32_t *)b->sxy;
				*(uint32_t *)(pk + 0x2C) = *(const uint32_t *)b->edge;
				const int32_t ca = (int16_t)(a->fade >> 4);
				const uint32_t cola = (uint32_t)((shl32(shl32(ca, 8) | ca, 8)) | ca);
				*(uint32_t *)(pk + 0x10) = cola;
				*(uint32_t *)(pk + 4) = cola;
				*(uint32_t *)pk = 0xC000000;
				pk[7] = 0x3E;
				const int32_t cb = (int16_t)(b->fade >> 4);
				const uint32_t colb = (uint32_t)((shl32(shl32(cb, 8) | cb, 8)) | cb);
				*(uint32_t *)(pk + 0x28) = colb;
				*(uint32_t *)(pk + 0x1C) = colb;
				InsertPrimAutoDepth(OTBase() + (((int32_t)a->otz + b->otz) >> 5) * 4 + 0x44, pk);
			}
			a = b;
		}
	}

	static uint32_t Ribbon(Point *tail, int32_t count, int32_t width_word, int32_t bias, uint32_t uv0, uint32_t uv1, uint16_t uv2, uint16_t uv3, uint32_t dir0)
	{
		int16_t dir[2] = { (int16_t)dir0, (int16_t)(dir0 >> 16) };
		auto width = [&](const Point *n) { return (int32_t)((const int16_t *)n)[width_word]; };
		RibbonEdge(tail, tail->next, nullptr, dir, tail->next ? width(tail->next) : 0, bias);
		Point *p = tail->next;
		for (int32_t i = count - 2; i != 0; i--)
		{
			RibbonEdge(p, p->next, dir, dir, width(p->next), bias);
			p = p->next;
		}
		RibbonEdge(p, nullptr, dir, nullptr, 0, bias);
		RibbonQuads(tail, count, uv0, uv1, uv2, uv3);
		return (uint16_t)dir[0] | (uint32_t)(uint16_t)dir[1] << 16;
	}

	static void ArcRibbon(Point *tail, int32_t count, int32_t style)
	{
		const int32_t sz = (int32_t)var<uint32_t>(game_addr(game::GTE_SZ3));
		const int32_t h2 = (int32_t)((var<uint32_t>(game_addr(game::GTE_H_Zoom)) & 0xFFFF) >> 1);
		const uint32_t v = shl32(style + 0xC0, 8);
		Ribbon(tail, count, 3, 0x200, v | 0x3D140008, v | 0xB70038, (uint16_t)(v | 8), (uint16_t)(v | 0x38), (uint32_t)(sz > h2 ? sz : h2));
	}

	static void ArcProject(Point *p)
	{
		do
		{
			GteLoadV0(p);
			GteRTPS();
			GteReadSXY2(p->sxy);
			GteReadOTZ(&p->otz);
			p = p->next;
		} while (p);
	}

	static void CrawlerPointWorld(const uint8_t *model, const Point *p, int16_t w[4])
	{
		const Mat4x3 *m = (const Mat4x3 *)(*(uint8_t *const *)model + 0x20 + CrawlBone(p) * 0x30);
		GteSetRotMatrixCtrl(m);
		GteSetTransVectorCtrl(m);
		GteLoadV0u(*(uint8_t *const *)(model + 4) + p->a[0] * 2);
		GteMVMVA_RotV0Tr();
		GteStoreIR123(w);
	}
	static void CrawlerPointProject(Point *p, int16_t w[4])
	{
		w[0] = (int16_t)(w[0] + p->a[2]);
		w[1] = (int16_t)(w[1] + p->b);
		w[2] = (int16_t)(w[2] + p->c[0]);
		GteSetRotMatrixCtrl(&Camera());
		GteSetTransVectorCtrl(&Camera());
		GteLoadV0(w);
		GteRTPS();
		GteReadSXY2(p->sxy);
		GteReadOTZ(&p->otz);
	}
	static uint32_t CrawlerRibbon(Point *tail, int32_t count, uint32_t dir0)
	{
		return Ribbon(tail, count, 1, 0x400, 0x3D14C008, 0xB7C038, 0xC008, 0xC038, dir0);
	}
}
}

namespace ff8fx
{
namespace disch126
{
	static uint32_t CrawlerDraw(uint8_t *model, Point *tail, int32_t count, uint32_t dir0, uint16_t pad)
	{
		Point *p = tail;
		do
		{
			int16_t w[4] = { 0, 0, 0, (int16_t)pad };
			CrawlerPointWorld(model, p, w);
			CrawlerPointProject(p, w);
			p = p->next;
		} while (p);
		return CrawlerRibbon(tail, count, dir0);
	}

	static Point *AllocArcPoint(const int16_t *pos, int16_t width)
	{
		const uint32_t base = TexBase();
		Point *p = var<Point *>(base + ARC_FREE);
		if (p) var<Point *>(base + ARC_FREE) = nullptr;
		else
		{
			p = (Point *)(base + ARC_POINTS);
			int n = 0x200;
			while (p->b != -1)
			{
				p++;
				if (--n == 0) return nullptr;
			}
		}
		*(uint32_t *)p->a = *(const uint32_t *)pos;
		*(uint32_t *)&p->a[2] = *(const uint32_t *)&pos[2];
		p->b = width;
		p->d = 2;
		p->fade = 0x200;
		p->step = 0x300;
		p->next = nullptr;
		return p;
	}

	static Point *AllocCrawlerPoint(int32_t bone, int32_t vertex, int32_t width, int32_t drift, int32_t fade, int32_t step)
	{
		const uint32_t base = TexBase();
		Point *p = var<Point *>(base + CRAWL_FREE);
		if (p) var<Point *>(base + CRAWL_FREE) = nullptr;
		else
		{
			p = (Point *)(base + CRAWL_POINTS);
			int n = 0x200;
			while (p->a[0] != -1)
			{
				p++;
				if (--n == 0) return nullptr;
			}
		}
		p->c[0] = 0;
		p->b = 0;
		p->a[2] = 0;
		int8_t *d = (int8_t *)p + 0xC;
		if (drift)
		{
			for (int i = 0; i < 3; i++)
			{
				int32_t r = Rand();
				r += Rand();
				d[i] = (int8_t)((r >> 10) - 0x20);
			}
		}
		else
		{
			d[2] = 0;
			d[1] = 0;
			d[0] = 0;
		}
		d[3] = (int8_t)bone;
		p->a[1] = (int16_t)width;
		p->a[0] = (int16_t)vertex;
		p->step = (int16_t)step;
		p->fade = (int16_t)fade;
		p->next = nullptr;
		return p;
	}

	static Point *SpawnArc(const int16_t *vel, const int16_t *from, const int16_t *to, const int16_t *dir, int32_t slot, int32_t t0, int32_t speed, int32_t width, int32_t style, int32_t forks)
	{
		Point *pt = AllocArcPoint(from, (int16_t)width);
		if (!pt) return nullptr;
		ArcNode *a = (ArcNode *)AddTaskToQueue(QArcs(), game_addr(ORIG_ArcTask));
		if (!a)
		{
			pt->b = -1;
			return nullptr;
		}
		Memset32(&a->pad0c, 0, 0xC);
		a->pad0c = 0;
		ff8fx::copy<(8)>(a->pos, from);
		ff8fx::copy<(8)>(a->end, to);
		ff8fx::copy<(8)>(a->vel, vel);
		a->t = (int16_t)t0;
		a->forks = (int16_t)forks;
		a->speed = (int16_t)speed;
		a->head = pt;
		a->tail = pt;
		a->count = 1;
		if (!dir)
		{
			a->dir[0] = (int16_t)((Rand() & 0xFF) - 0x80);
			a->dir[1] = (int16_t)((Rand() & 0xFF) - 0x80);
			a->dir[2] = (int16_t)((Rand() & 0xFF) - 0x80);
		}
		else ff8fx::copy<(8)>(a->dir, dir);
		a->slot = (int16_t)slot;
		a->width = (int16_t)width;
		a->style = (int16_t)style;
		return pt;
	}

	static void SpawnCrawlerScript(uint8_t *model, const int16_t *script, int32_t steps, int32_t width)
	{
		const int32_t r0 = Rand() & 0x3FF;
		const int32_t r1 = Rand() & 0x1FF;
		const int32_t fade = r0 + r1 + 0x100;
		const int16_t step = (int16_t)-(int16_t)((int16_t)fade >> 3);
		const int16_t bone = script[0];
		script++;
		Point *pt = AllocCrawlerPoint(bone, script[0], width, 0, fade, step);
		if (!pt) return;
		CrawlerNode *c = (CrawlerNode *)AddTaskToQueue(QCrawlers(), game_addr(ORIG_CrawlerTask));
		if (!c)
		{
			pt->a[0] = -1;
			var<Point *>(TexBase() + CRAWL_FREE) = pt;
			return;
		}
		Memset32(&c->steps, 0, 7);
		c->step = step;
		c->script = script + 1;
		c->head = pt;
		c->tail = pt;
		c->fade = (int16_t)fade;
		c->model = model;
		c->steps = (int16_t)steps;
		c->width = (int16_t)width;
		c->count = 1;
	}

	static void SpawnCrawler(uint8_t *model, const int16_t *table, int32_t index, int32_t steps, int32_t scale)
	{
		const int16_t *script = table + table[index + 1];
		static const int32_t widths[4] = { 0x96, 0xB4, 0xDC, 0x104 };
		int32_t w = widths[Rand() & 3];
		if (scale != 0x1000) w = mul32(w, scale) >> 12;
		SpawnCrawlerScript(model, script, steps, w);
	}

	static void AnglesToDir(const int16_t *ang, int16_t *out)
	{
		const int32_t c0 = ComputeCos(ang[0]);
		out[1] = (int16_t)-ComputeSin(ang[0]);
		out[0] = (int16_t)((int32_t)(0u - (uint32_t)mul32(ComputeSin(ang[1]), c0)) >> 12);
		out[2] = (int16_t)(mul32(ComputeCos(ang[1]), c0) >> 12);
	}

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

	static uint32_t __cdecl LayoutTask(TaskNode *n)
	{
		LayoutNode *g = (LayoutNode *)n;
		PrimArg arg;
		GetDefaultEffectPosition(Caster(), arg.pos);
		arg.pos[1] = (int16_t)(arg.pos[1] + 0xC8);
		arg.morph = TexBase() + MORPH;
		return prim::play((prim::Layout *)g->layout, PartCallback, (int)&arg, 0) ? 0 : TASK_END;
	}

	static void SparkDraw(SparkNode *s)
	{
		TransformCameraByShadowRotation(s->pos, s->scale, -(s->scale >> 2));
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		*(uint32_t *)h = s->kind ? game_addr(SEQ_SparkB) : game_addr(SEQ_Spark);
		*(int16_t *)(h + 4) = s->frame;
		*(int16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static uint32_t __cdecl SparkTask(TaskNode *n)
	{
		SparkNode *s = (SparkNode *)n;
		if ((uint8_t)s->slot != 0xFF && s->frame <= 4)
		{
			uint8_t *e = Entity(s->slot);
			e[1] |= 8;
			const int32_t sn = ComputeSin(shl32(s->frame, 8));
			BlendColour(&SavedTints()[s->slot], TINT_Colour(), 0x1000 - sn, sn, e + 0x28);
		}
		SparkDraw(s);
		s->frame++;
		return s->frame < 8 ? 0 : TASK_END;
	}

	static uint32_t __cdecl ArcTask(TaskNode *n)
	{
		ArcNode *a = (ArcNode *)n;
		for (int k = 8; k != 0; k--)
		{
			if (a->t >= 0x1000) continue;
			a->pos[0] = (int16_t)(a->pos[0] + a->vel[0]);
			a->t = (int16_t)(a->speed + a->t);
			a->pos[1] = (int16_t)(a->pos[1] + a->vel[1]);
			a->pos[2] = (int16_t)(a->pos[2] + a->vel[2]);
			a->vel[0] = (int16_t)(a->vel[0] + ((Rand() & 0x7F) - 0x40));
			a->vel[1] = (int16_t)(a->vel[1] + ((Rand() & 0x7F) - 0x40));
			a->vel[2] = (int16_t)(a->vel[2] + ((Rand() & 0x7F) - 0x40));
			if (Rand() < 0x1000)
			{
				a->pos[0] = (int16_t)(a->pos[0] + ((Rand() & 0x1FF) - 0x100));
				a->pos[1] = (int16_t)(a->pos[1] + ((Rand() & 0x1FF) - 0x100));
				a->pos[2] = (int16_t)(a->pos[2] + ((Rand() & 0x1FF) - 0x100));
			}
			int16_t p[4] = { 0, 0, 0, 0 };
			GteSetIR0(0x1000 - a->t);
			GteLoadIR123(a->pos);
			GteGPF();
			GteSetIR0(a->t);
			GteLoadIR123(a->end);
			GteGPL();
			GteStoreIR123(p);
			if (a->t >= 0x1000)
			{
				if (a->slot >= 0)
				{
					if (Rand() < FadeChance())
					{
						ScreenFadeTask(0, 1, 0, 0x40);
						FadeChance() = 0x1000;
					}
					else FadeChance() += 0x1000;
					SparkNode *s = (SparkNode *)AddTaskToQueue(QSparks(), game_addr(ORIG_SparkTask));
					if (s)
					{
						s->frame = 0;
						ff8fx::copy<(8)>(s->pos, p);
						s->kind = 0;
						s->scale = (int16_t)shl32(a->width, 4);
						s->slot = (int8_t)a->slot;
					}
				}
				else if (p[1] >= 0)
				{
					SparkNode *s = (SparkNode *)AddTaskToQueue(QSparks(), game_addr(ORIG_SparkTask));
					if (s)
					{
						s->frame = 0;
						ff8fx::copy<(8)>(s->pos, p);
						s->kind = 1;
						s->scale = (int16_t)shl32(a->width, 5);
						s->slot = (int8_t)a->slot;
					}
				}
			}
			Point *pt = AllocArcPoint(p, a->width);
			if (!pt)
			{
				a->t = 0x1000;
				continue;
			}
			a->head->next = pt;
			a->count++;
			const int32_t blend = BLEND_Table()[((uint16_t)a->t >> 1) & 0xFFF];
			a->head = pt;
			const int16_t hold = pt->d;
			GteSetIR0(blend);
			GteLoadIR123(a->dir);
			GteGPF();
			GteSetIR0(0x1000 - blend);
			GteLoadIR123((const void *)game_addr(ZERO_VEC));
			GteGPL();
			GteStoreIR123(pt->c);
			pt->d = hold;
			if (a->t >= 0xC00) continue;
			if (Rand() >= shl32(a->forks, 13)) continue;
			int16_t to[4] = { 0, 0, 0, 0 }, vel[4] = { 0, 0, 0, 0 };
			int32_t d;
			if (Rand() > 0x4000) d = -0x200 - (Rand() & 0x1FF);
			else d = (Rand() & 0x1FF) + 0x200;
			to[0] = (int16_t)(d + p[0]);
			if (Rand() > 0x4000) d = -0x200 - (Rand() & 0x1FF);
			else d = (Rand() & 0x1FF) + 0x200;
			to[1] = (int16_t)(d + p[1]);
			to[2] = (int16_t)((Rand() & 0x3FF) + p[2] + 0x400);
			vel[0] = (int16_t)((Rand() & 0x1FF) + a->vel[0] - 0x100);
			vel[1] = (int16_t)((Rand() & 0x1FF) + a->vel[1] - 0x100);
			vel[2] = (int16_t)((Rand() & 0x1FF) + a->vel[2] - 0x100);
			Point *f = SpawnArc(vel, p, to, a->dir, -1, a->t, 0x200, a->width >> 1, a->style, 0);
			if (f) ff8fx::copy<(8)>(f->c, pt->c);
			a->forks--;
		}
		Point *q = a->tail;
		do
		{
			if (q->fade > 0)
			{
				q->fade = (int16_t)(q->fade + q->step);
				if (q->fade <= 0) q->fade = 0;
				else if (q->d != 0 && q->step <= 0) q->d--;
				else if (q->step > -0x180) q->step = (int16_t)(q->step - 0x140);
			}
			q = q->next;
		} while (q);
		ArcProject(a->tail);
		if (a->count >= 2) ArcRibbon(a->tail, a->count, a->style);
		q = a->tail;
		do
		{
			q->a[0] = (int16_t)(q->a[0] + q->c[0]);
			q->a[1] = (int16_t)(q->a[1] + q->c[1]);
			q->a[2] = (int16_t)(q->a[2] + q->c[2]);
			q = q->next;
		} while (q);
		q = a->tail;
		while (q->fade == 0 && q->next->fade == 0)
		{
			var<Point *>(TexBase() + ARC_FREE) = q;
			Point *next = q->next;
			q->b = -1;
			a->count--;
			a->tail = next;
			if (a->count < 2) return TASK_END;
			q = next;
		}
		return 0;
	}

	static uint32_t g_crawl_slot;
	static uint16_t g_crawl_pad;
	static const uint16_t ALLOC_RET_HI = 0x006B;
	static __declspec(noinline) uint32_t CrawlerTaskBody(CrawlerNode *c, uint32_t slot, uint16_t pad)
	{
		uint16_t vpad = pad;
		const uint32_t slot0 = slot;
		uint32_t edi = 0;
		for (TaskNode *q = QCrawlers()->head; q && q != &c->hdr; q = q->next) edi = (uint32_t)q;
		int32_t k = c->steps;
		do
		{
			if (!c->script) continue;
			int16_t bone = *c->script++;
			int32_t drift = 1;
			while (bone < 0)
			{
				SpawnCrawlerScript(c->model, c->script, c->steps, c->width);
				slot = 2;
				vpad = (uint16_t)((uint32_t)QCrawlers() >> 16);
				c->script += -1 - bone;
				edi = (uint32_t)c->script;
				bone = *c->script++;
			}
			const int16_t vertex = *c->script++;
			const int16_t *peek = c->script;
			int16_t w = *peek;
			if (w < 0)
			{
				peek -= w;
				w = *peek;
			}
			if (w == 0x7FFF)
			{
				if (c->count <= 3) goto kill;
				c->script = nullptr;
				drift = 0;
			}
			{
				const int32_t width = c->width;
				Point *pt = AllocCrawlerPoint(bone, vertex, (mul32(Rand(), width) >> 15) + width, drift, c->fade, c->step);
				vpad = ALLOC_RET_HI;
				slot = (edi & 0xFFFF0000u) | (uint16_t)bone;
				if (!pt) goto kill;
				c->head->next = pt;
				c->count++;
				c->head = pt;
			}
		} while (--k != 0);
		{
			Point *q = c->tail;
			do
			{
				q->a[2] = (int16_t)(q->a[2] + CrawlDrift(q, 0));
				q->b = (int16_t)(q->b + CrawlDrift(q, 1));
				q->c[0] = (int16_t)(q->c[0] + CrawlDrift(q, 2));
				if (q->fade > 0)
				{
					q->fade = (int16_t)(q->fade + q->step);
					if (q->fade <= 0) q->fade = 0;
				}
				q = q->next;
			} while (q);
		}
		g_crawl_slot = slot;
		g_crawl_pad = vpad;
		if (c->count >= 2) g_crawl_slot = CrawlerDraw(c->model, c->tail, c->count, slot, vpad);
		{
			Point *q = c->tail;
			while (q->fade <= 0x10 && q->next->fade <= 0x10)
			{
				var<Point *>(TexBase() + CRAWL_FREE) = q;
				q->a[0] = -1;
				q = q->next;
				c->count--;
				c->tail = q;
				if (c->count < 2) return TASK_END;
			}
		}
		return 0;
	kill:
		g_crawl_slot = slot;
		g_crawl_pad = vpad;
		{
			Point *q = c->tail;
			var<Point *>(TexBase() + CRAWL_FREE) = q;
			do
			{
				q->a[0] = -1;
				q = q->next;
			} while (q);
		}
		return TASK_END;
	}
	static bool g_crawl_first;
	static uint32_t __cdecl CrawlerTask(TaskNode *n)
	{
		const bool first = g_crawl_first;
		g_crawl_first = false;
		return CrawlerTaskBody((CrawlerNode *)n, first ? 0 : g_crawl_slot, first ? 0 : g_crawl_pad);
	}

	static uint32_t __cdecl MainTask(TaskNode *n)
	{
		MainNode *m = (MainNode *)n;
		const int16_t c = m->counter;
		if (c < 4 || c > 0x30)
		{
			const int32_t level = shl32(c < 4 ? c : 0x34 - c, 9);
			if (level > (int32_t)FlashLevel()) FlashLevel() = (uint32_t)level;
		}
		else if ((int32_t)FlashLevel() < 0x800) FlashLevel() = 0x800;
		if ((uint32_t)(int32_t)c < 8)
		{
			const int32_t s = c * 3;
			SpawnCrawler(*(uint8_t **)(Caster() + 0x64), CRAWL_Scripts(), s % CRAWL_Scripts()[0], 3, 0x1000);
			SpawnCrawler(*(uint8_t **)(Caster() + 0x64), CRAWL_Scripts(), (s + 1) % CRAWL_Scripts()[0], 3, 0x1000);
			SpawnCrawler(*(uint8_t **)(Caster() + 0x64), CRAWL_Scripts(), (s + 2) % CRAWL_Scripts()[0], 3, 0x1000);
		}
		if (m->counter == 0)
		{
			int16_t pos[4];
			GetDefaultEffectPosition(Caster(), pos);
			BdPlaySE3D(SOUND_Discharge(), 0x100, pos);
		}
		const int32_t c8 = m->counter - 8;
		if ((uint32_t)c8 < 0x34)
		{
			const int32_t r = Rand();
			SpawnCrawler(*(uint8_t **)(Caster() + 0x64), CRAWL_Scripts(), mul32(r, CRAWL_Scripts()[0]) >> 15, 3, 0x1000);
		}
		if ((uint32_t)c8 < 0xC)
		{
			int16_t from[4], dir[4] = { 0, 0, 0, 0 }, vel[4] = { 0, 0, 0, 0 };
			GetDefaultEffectPosition(Caster(), from);
			int16_t ang[2];
			ang[0] = (int16_t)((Rand() & 0x3FF) - 0x100);
			ang[1] = (int16_t)((uint32_t)shl32(c8, 10) / 12 + shl32(c8 & 3, 10));
			AnglesToDir(ang, dir);
			from[0] = (int16_t)(from[0] + (dir[0] >> 8));
			from[1] = (int16_t)(from[1] + (dir[1] >> 8));
			from[2] = (int16_t)(from[2] + (dir[2] >> 8));
			int16_t to[4] = { (int16_t)(dir[0] + from[0]), (int16_t)(dir[1] + from[1]), (int16_t)(dir[2] + from[2]), 0 };
			vel[0] = (int16_t)((Rand() & 0x7F) - 0x40);
			vel[1] = (int16_t)((Rand() & 0x7F) - 0x40);
			vel[2] = (int16_t)((Rand() & 0x7F) - 0x40);
			const int32_t style = Rand() & 1;
			Point *pt = SpawnArc(vel, from, to, nullptr, -1, 0, 0x100, 0xFA, style, 3);
			if (pt) ff8fx::copy<(8)>(pt->c, (const void *)game_addr(ZERO_VEC));
		}
		const int32_t c14 = m->counter - 0xE;
		if ((uint32_t)c14 < 0x1E)
		{
			if (!(c14 & 1))
			{
				int16_t from[4] = { 0, 0, 0, 0 }, centre[4], to[4] = { 0, 0, 0, 0 };
				const int32_t r = Rand();
				GetEffectSpawnPosition(Caster(), BONES_Caster()[(uint32_t)(r * 5) >> 15], 0, from);
				GetDefaultEffectPosition(Caster(), centre);
				uint8_t *e;
				do
				{
					const int16_t slot = m->party;
					e = Entity(slot);
					m->party = (int16_t)(slot + 1);
					if (m->party >= 3) m->party = 0;
				} while (!(e[0] & 2));
				const int32_t rb = Rand();
				GetEffectSpawnPosition(e, mul32(rb, **(uint8_t **)*(uint8_t **)(e + 0x64)) >> 15, 0, to);
				int16_t vel[4] = { 0, 0, 0, 0 };
				vel[0] = (int16_t)(from[0] - centre[0]);
				int16_t vy = (int16_t)(from[1] - centre[1]);
				vel[1] = vy > 0 ? (int16_t)(vy >> 3) : (int16_t)(vy >> 1);
				vel[2] = (int16_t)((int16_t)(from[2] - centre[2]) >> 2);
				const int32_t style = Rand() & 1;
				const int32_t slot = (int32_t)(e - Entity(0)) / 0x9C;
				Point *pt = SpawnArc(vel, from, to, nullptr, slot, 0, 0x100, 0xFA, style, 3);
				if (pt) ff8fx::copy<(8)>(pt->c, (const void *)game_addr(ZERO_VEC));
			}
			if (!(c14 & 3))
			{
				LayoutNode *g = (LayoutNode *)AddTaskToQueue(QLayouts(), game_addr(ORIG_LayoutTask));
				DecodeModelPrimLayout(game_addr(MODEL_Glow), g->layout, 0xC8);
			}
		}
		if (m->counter >= 0x34)
		{
			const ActionData *act = Ctx()->actions;
			ApplyActionResultToTargets(act->targets, act->target_count);
			return TASK_END;
		}
		m->counter++;
		return 0;
	}

	static uint32_t __cdecl RootTask(TaskNode *n)
	{
		RootNode *r = (RootNode *)n;
		if (r->arena)
		{
			PacketCursor() = TexBase() + 0xC454;
			r->arena = 0;
		}
		else
		{
			PacketCursor() = TexBase() + 0x1C454;
			r->arena = 1;
		}
		FlashLevel() = 0;
		*(uint16_t *)Entity(0) &= 0xF7FF;
		*(uint16_t *)Entity(1) &= 0xF7FF;
		*(uint16_t *)Entity(2) &= 0xF7FF;
		if (r->counter == 1 && !r->started)
		{
			r->started = 1;
			InitTaskQueuePool(QMain(), (void *)(TexBase() + 0x22A0), 0x14, 1);
			InitTaskQueuePool(QArcs(), (void *)(TexBase() + 0x13A0), 0x3C, 0x40);
			InitTaskQueuePool(QCrawlers(), (void *)(TexBase() + 0x9A0), 0x28, 0x40);
			InitTaskQueuePool(QSparks(), (void *)(TexBase() + 0x6A0), 0x18, 0x20);
			InitTaskQueuePool(QLayouts(), (void *)TexBase(), 0xD4, 8);
			Memset32((void *)(TexBase() + CRAWL_POINTS), 0, 0x1400);
			Memset32((void *)(TexBase() + ARC_POINTS), 0, 0x1400);
			for (int i = 0; i < 0x200; i++) ((Point *)(TexBase() + CRAWL_POINTS))[i].a[0] = -1;
			for (int i = 0; i < 0x200; i++) ((Point *)(TexBase() + ARC_POINTS))[i].b = -1;
			MainNode *m = (MainNode *)AddTaskToQueue(QMain(), game_addr(ORIG_MainTask));
			Memset32(&m->counter, 0, 2);
			var<Point *>(TexBase() + ARC_FREE) = nullptr;
			var<Point *>(TexBase() + CRAWL_FREE) = nullptr;
		}
		int e1 = 0, e2 = 0, e3 = 0, e4 = 0, e5 = 0;
		if (r->started)
		{
			e1 = ExecuteTaskQueue(QMain());
			GteSetRotMatrix(&Camera());
			GteSetTransVector(&Camera());
			e2 = ExecuteTaskQueue(QArcs());
			ComputeBonesWorldMatrices(Caster() + 0x60, Caster() + 0x40);
			g_crawl_first = true;
			e3 = ExecuteTaskQueue(QCrawlers());
			BuildBoneMatricesFromPose(Caster() + 0x60);
			e4 = ExecuteTaskQueue(QSparks());
			e5 = ExecuteTaskQueue(QLayouts());
		}
		SetScreenFlash(FlashLevel(), 0);
		if (r->started && e1 == 0 && e2 == 0 && e3 == 0 && e4 == 0 && e5 == 0) return TASK_END;
		r->counter++;
		return 0;
	}
}

	void register_mag126_electric_discharge()
	{
		register_port(disch126::ORIG_RootTask, (void *)disch126::RootTask, "D126 RootTask", 126);
		register_port(disch126::ORIG_MainTask, (void *)disch126::MainTask, "D126 MainTask", 126);
		register_port(disch126::ORIG_ArcTask, (void *)disch126::ArcTask, "D126 ArcTask", 126);
		register_port(disch126::ORIG_CrawlerTask, (void *)disch126::CrawlerTask, "D126 CrawlerTask", 126);
		register_port(disch126::ORIG_SparkTask, (void *)disch126::SparkTask, "D126 SparkTask", 126);
		register_port(disch126::ORIG_LayoutTask, (void *)disch126::LayoutTask, "D126 LayoutTask", 126);
	}
}

