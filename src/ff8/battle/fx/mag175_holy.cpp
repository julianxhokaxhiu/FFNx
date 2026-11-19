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

// Effect 175: Holy

#include "mag_common.h"

namespace ff8fx
{
namespace holy175
{
	using namespace eng;
	using namespace magc;

	inline uint32_t &TexFile() { return var<uint32_t>(game_addr(game::MAG_175_TexFile)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::MAG_175_TexBase)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_175_PacketCursor)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::MAG_175_Ctx)); }
	inline int32_t &FadeSide() { return var<int32_t>(game_addr(game::MAG_175_FadeSide)); }
	inline int32_t &GridTextured() { return var<int32_t>(game_addr(game::MAG_175_GridTextured)); }
	inline int32_t &GridActive() { return var<int32_t>(game_addr(game::MAG_175_GridActive)); }
	inline TaskQueuePair &RootQueues() { return var<TaskQueuePair>(game_addr(game::MAG_175_RootQueues)); }
	inline TaskQueue &Tasks() { return var<TaskQueue>(game_addr(game::MAG_175_Tasks)); }
	inline uint32_t &FadeFlag(int slot) { return var<uint32_t>(game_addr(game::unk_2377698) + 0x18 * slot); }
	inline Mat4x3 *OrbitMatrix(int index) { return (Mat4x3 *)(game_addr(game::unk_23745B0) + 0x20 * index); }
	inline Mat4x3 *SpriteMatrix() { return (Mat4x3 *)game_addr(game::unk_23941E8); }
	inline uint32_t OTBase() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)); }
	inline uint32_t &FrameArena() { return var<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)); }
	inline int16_t ProjH() { return var<int16_t>(game_addr(game::BATTLE_PROJECTION_DISTANCE)); }
	inline uint8_t DisplayBuffer() { return var<uint8_t>(game_addr(game::g_Battle_DisplayParity)); }

	static constexpr game::Id ORIG_RootTask = game::MAG_175_RootTask;
	static constexpr game::Id ORIG_RestoreTask = game::MAG_175_RestoreTask;
	static constexpr game::Id ORIG_DirectorTask = game::MAG_175_HOLY_Director_Tick;
	static constexpr game::Id ORIG_ShatterTask = game::MAG_175_ShatterTask;
	static constexpr game::Id ORIG_FadeInTask = game::MAG_175_FadeInTask;
	static constexpr game::Id ORIG_FadeOutTask = game::MAG_175_FadeOutTask;
	static constexpr game::Id ORIG_SparkleTask = game::MAG_175_SparkleTask;
	static constexpr game::Id ORIG_OrbitCtrlTask = game::MAG_175_OrbitCtrlTask;
	static constexpr game::Id ORIG_OrbitTask = game::MAG_175_OrbitTask;
	static constexpr game::Id ORIG_MoteTask = game::MAG_175_MoteTask;
	static constexpr game::Id ORIG_RingTask = game::MAG_175_RingTask;
	static constexpr game::Id ORIG_WaveTask = game::MAG_175_WaveTask;
	static constexpr game::Id ORIG_ExpandTask = game::MAG_175_ExpandTask;
	static constexpr game::Id ORIG_PillarTask = game::MAG_175_PillarTask;
	static constexpr game::Id ORIG_BeamTask = game::MAG_175_BeamTask;
	static constexpr game::Id ORIG_DiskTask = game::MAG_175_DiskTask;
	static constexpr game::Id ORIG_FlashTask = game::MAG_175_FlashTask;

	static const void *SOUND_Holy() { return (const void *)game_addr(game::unk_D59180); }
	static const void *TIM_Upload() { return (const void *)game_addr(game::unk_D45EAC); }
	static constexpr game::Id PILLAR_Delays = game::unk_D59188;
	static constexpr game::Id BEAM_Sprites = game::unk_D591D0;
	static constexpr game::Id WAVE_Textures = game::unk_D59210;
	static constexpr game::Id STAGE_Buckets = game::unk_D59250;
	static constexpr game::Id STAGE_EnvRestore = game::unk_D592C0;
	static constexpr game::Id DRAWENV = game::g_Battle_DrawEnv;
	static constexpr game::Id SEQ_Sparkle = game::unk_D524C4;
	static constexpr game::Id SEQ_OrbitSpark = game::unk_D523E8;
	static constexpr game::Id SEQ_Mote = game::unk_D52374;
	static constexpr game::Id SEQ_Beam = game::unk_D522CC;
	static constexpr game::Id MODEL_Ring = game::unk_D54588;
	static constexpr game::Id MODEL_Wave = game::unk_D56C58;
	static constexpr game::Id MODEL_Expand = game::unk_D53BA0;
	static constexpr game::Id MODEL_Pillar = game::unk_D53688;
	static constexpr game::Id MODEL_Disk = game::unk_D52DD8;
	static constexpr game::Id MODEL_Flash = game::unk_D527F8;

	static constexpr game::Id GRID_Vertices = game::MAG_175_GRID_Vertices, GRID_VerticesEnd = game::MAG_175_GRID_VerticesEnd;
	static constexpr game::Id GRID_Centres = game::MAG_175_GRID_Centres, GRID_CentresEnd = game::MAG_175_Tasks;
	static constexpr game::Id GRID_Triangles = game::MAG175_HOLY_lightPillarTable;
	static constexpr game::Id GRID_Packets = game::MAG_175_GRID_Packets;
	static constexpr game::Id GRID_CellPointers = game::MAG_175_GRID_CellPointers;
	static const int GRID_TriangleCount = 14 * 20 * 4;
	static constexpr game::Id SPARKLES = game::MAG_175_Sparkles, SPARKLES_End = game::MAG_175_GRID_Centres;
	static constexpr game::Id MOTES = game::MAG_175_Motes, MOTES_End = game::MAG_175_Sparkles;
	static constexpr game::Id STAGE_AnimHeaders = game::unk_1D989A4;

	inline int32_t ComputeCos(int32_t angle) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(angle); }
	inline int32_t Sqrt(int32_t v) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::Math_SqrtInt))(v); }
	inline void QueueTimUpload(const void *tim) { fn<void (__cdecl *)(const void *)>(game_addr(game::Battle_QueueTIMUpload_GetEOF))(tim); }
	inline void MatrixMultiplyVector(const Mat4x3 *m, const int16_t *in, int16_t *out) { fn<void (__cdecl *)(const Mat4x3 *, const int16_t *, int16_t *)>(game_addr(game::matrixMultiplyVector))(m, in, out); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline Mat4x3 *ShadowRotation(const void *pos, int32_t a, int32_t b) { return fn<Mat4x3 *(__cdecl *)(const void *, int32_t, int32_t)>(game_addr(game::TransformCameraByShadowRotation))(pos, a, b); }
	inline uint32_t RenderWaveModel(void *header, uint32_t ot, int mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int, uint32_t)>(game_addr(game::MAG_066_RenderScrollPrimModel))(header, ot, mode, cursor); }
	inline void InsertPrimAltViewport(uint32_t bucket, void *packet) { fn<void (__cdecl *)(uint32_t, void *)>(game_addr(game::SSIGPU_InsertPrimAltViewport))(bucket, packet); }
	inline uint32_t RenderGeometry(void *group, void *header, uint32_t ot, uint32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, void *, uint32_t, uint32_t, uint32_t)>(game_addr(game::RenderGeometry))(group, header, ot, mode, cursor); }
	inline void BuildDrawEnvPacket(void *packet, uint32_t env) { fn<void (__cdecl *)(void *, uint32_t)>(game_addr(game::SSIGPU_SetDrawEnv))(packet, env); }
	inline void BuildDrawAreaPacket(void *packet, const int16_t *rect) { fn<void (__cdecl *)(void *, const int16_t *)>(game_addr(game::GPU_SetDrawAreaPrim))(packet, rect); }
	inline void BuildDrawOffsetPacket(void *packet, const void *xy) { fn<void (__cdecl *)(void *, const void *)>(game_addr(game::SSIGPU_SetDrawOffsetPrim))(packet, xy); }
	inline void GetScreenOffset(int32_t *x, int32_t *y) { fn<void (__cdecl *)(int32_t *, int32_t *)>(game_addr(game::GTE_ReadScreenOffset))(x, y); }
	inline void SetScreenOffset(int32_t x, int32_t y) { fn<void (__cdecl *)(int32_t, int32_t)>(game_addr(game::Call_Bs_ParseCamera))(x, y); }
	inline void GteSetFarColor(int32_t r, int32_t g, int32_t b) { fn<void (__cdecl *)(int32_t, int32_t, int32_t)>(game_addr(game::GTE_SetFarColor))(r, g, b); }
	inline void GteSetRGB(const void *rgb) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadRGBC))(rgb); }
	inline void GteDPCS() { fn<void (__cdecl *)()>(game_addr(game::GTE_DPCS))(); }
	inline void GteReadRGB2(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreRGB2))(dst); }
	inline void GteLoadV012(const void *v0, const void *v1, const void *v2) { fn<void (__cdecl *)(const void *, const void *, const void *)>(game_addr(game::GTE_LoadV012))(v0, v1, v2); }
	inline void GteRTPT() { fn<void (__cdecl *)()>(game_addr(game::GTE_RTPT))(); }
	inline void GteReadSXY012(void *s0, void *s1, void *s2) { fn<void (__cdecl *)(void *, void *, void *)>(game_addr(game::GTE_ReadSXY012_Split))(s0, s1, s2); }

#pragma pack(push, 1)
	struct RootNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
	};
	struct RestoreNode
	{
		TaskNode hdr;
		int16_t counter;
		uint16_t done;
	};
	struct DirectorNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t action;
		int16_t pos[4];
		uint8_t pad18[8];
		int16_t target;
		int16_t pad22;
	};
	struct ShatterNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t pad0E[0x16];
	};
	struct FadeNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t slot;
		uint8_t pad10[8];
		int16_t duration;
		uint16_t saved;
		uint8_t pad1C[8];
	};
	struct SparkleNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		int16_t pos[4];
		uint8_t pad18[0xC];
	};
	struct OrbitCtrlNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t index;
		int16_t pos[4];
		int16_t pad18;
		int16_t target;
		int16_t ax;
		int16_t dax;
		int16_t az;
		int16_t daz;
	};
	struct OrbitNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t index;
		uint8_t pad10[6];
		int16_t base;
		int16_t angle;
		int16_t phase;
		int16_t size;
		int16_t k;
		int16_t radius;
		int16_t dradius;
	};
	struct EmitterNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t pad0E[0x16];
	};
	struct RingNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t delay;
		int16_t pos[4];
		int16_t angle;
		int16_t spin;
		int16_t scale;
		int16_t dscale;
		int16_t w20;
		int16_t w22;
	};
	struct Sparkle
	{
		uint32_t active;
		uint16_t frame;
		int16_t size;
		int16_t radius;
		int16_t y;
		int16_t angle;
		int16_t pad0E;
		int16_t dradius;
		int16_t dy;
		int16_t dangle;
		int16_t pad16;
	};
	struct Mote
	{
		uint32_t active;
		uint16_t frame;
		int16_t scale;
		int16_t x, y, z;
		int16_t age;
		int16_t pad10;
		int16_t speed;
		int16_t pad14;
		int16_t life;
	};
	struct Vertex
	{
		int16_t sxy[2];
		int16_t x, y, z;
		uint16_t flags;
		int16_t u, v;
	};
	struct Triangle
	{
		Vertex *v[3];
		int16_t cx, cy;
		int16_t zero10;
		int16_t delay;
		int16_t offx, offy;
		int16_t zero18;
		int16_t scale;
		int16_t vx, vy;
		int16_t ay;
		int16_t dscale;
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x10 && sizeof(RestoreNode) == 0x10, "Holy root nodes");
	static_assert(sizeof(DirectorNode) == 0x24 && sizeof(ShatterNode) == 0x24 && sizeof(FadeNode) == 0x24 && sizeof(SparkleNode) == 0x24, "Holy nodes");
	static_assert(sizeof(OrbitCtrlNode) == 0x24 && sizeof(OrbitNode) == 0x24 && sizeof(EmitterNode) == 0x24 && sizeof(RingNode) == 0x24, "Holy nodes");
	static_assert(sizeof(Sparkle) == 0x18 && sizeof(Mote) == 0x18 && sizeof(Vertex) == 0x10 && sizeof(Triangle) == 0x24, "Holy pools");

	inline TaskNode *AddTask(uint32_t fn) { return AddTaskToQueue(&Tasks(), fn); }
	inline uint16_t &EntityFlags(int slot) { return *(uint16_t *)Entity(slot); }

	static int32_t Div3(int32_t x) { int32_t hi = (int32_t)(((int64_t)x * 0x55555556) >> 32); return hi + (int32_t)((uint32_t)hi >> 31); }
	static int32_t Div6(int32_t x) { int32_t hi = (int32_t)(((int64_t)x * 0x2AAAAAAB) >> 32); return hi + (int32_t)((uint32_t)hi >> 31); }
	static int32_t Div20(int32_t x) { int32_t hi = (int32_t)(((int64_t)x * 0x66666667) >> 32) >> 3; return hi + (int32_t)((uint32_t)hi >> 31); }
	static int32_t Div350(int32_t x) { int32_t hi = (int32_t)(((int64_t)x * 0x5D9F7391) >> 32) >> 7; return hi + (int32_t)((uint32_t)hi >> 31); }
	static int32_t Div12(int32_t x) { int32_t hi = (int32_t)(((int64_t)x * 0x2AAAAAAB) >> 32) >> 1; return hi + (int32_t)((uint32_t)hi >> 31); }
}
}

namespace ff8fx
{
namespace holy175
{
	static uint32_t __cdecl RootTask(TaskNode *n)
	{
		RootNode *r = (RootNode *)n;
		if (r->counter & 1) PacketCursor() = TexBase() + 0xC000;
		else PacketCursor() = TexBase();
		int left = ExecuteTaskQueue(&Tasks());
		if (!left && !(Ctx()->flags & 2))
		{
			RestoreNode *t = (RestoreNode *)AddTaskToQueue(&RootQueues().first, game_addr(ORIG_RestoreTask));
			t->counter = (int16_t)left;
			t->done = (uint16_t)left;
		}
		r->counter++;
		return left ? 0 : TASK_END;
	}

	static uint32_t __cdecl RestoreTask(TaskNode *n)
	{
		RestoreNode *p = (RestoreNode *)n;
		if (p->counter == 1) TextureRestoreTask((void *)TexBase(), &p->done);
		uint16_t done = p->done;
		p->counter++;
		return done ? TASK_END : 0;
	}

	static uint32_t CellUV(const Vertex *v, uint8_t du, uint16_t tpage)
	{
		uint8_t u = (uint8_t)((uint8_t)v->u + du);
		uint8_t w = (uint8_t)v->v;
		return (uint32_t)u | ((uint32_t)w << 8) | ((uint32_t)tpage << 16);
	}

	static void BuildGrid()
	{
		Vertex *e = (Vertex *)game_addr(GRID_Vertices);
		int32_t y = -0x78;
		for (int row = 0; row < 15; row++, y += 0x10)
		{
			int32_t x = -0xA8;
			for (int col = 0; col < 21; col++, x += 0x10, e++)
			{
				e->x = (int16_t)x;
				e->y = (int16_t)y;
				e->z = 0;
				e->flags = 0;
				if (row == 0 || row == 14)
				{
					e->v = 0;
					e->flags |= 1;
				}
				else
				{
					int16_t r = (int16_t)(CrtRand() % 12 - 6);
					e->v = r;
					e->y = (int16_t)(e->y + r);
				}
				if (col == 0 || col == 20)
				{
					e->u = 0;
					e->flags |= 2;
				}
				else
				{
					int16_t r = (int16_t)(CrtRand() % 14 - 7);
					e->u = r;
					e->x = (int16_t)(e->x + r);
				}
				e->u = (int16_t)(e->u + (int16_t)(x + 0xA8));
				e->v = (int16_t)(e->v + (int16_t)(y + 0x78));
			}
		}
		Vertex *c = (Vertex *)game_addr(GRID_Centres);
		int32_t yb = -0x78;
		for (int row = 0; row < 14; row++, yb += 0x10)
		{
			int32_t xb = -0xA8;
			for (int col = 0; col < 20; col++, xb += 0x10, c++)
			{
				c->x = (int16_t)(xb + 8);
				c->y = (int16_t)(yb + 8);
				c->z = 0;
				c->flags = 0;
				int16_t r = (int16_t)(CrtRand() % 6 - 3);
				c->v = r;
				c->y = (int16_t)(c->y + r);
				r = (int16_t)(CrtRand() % 6 - 3);
				c->u = r;
				c->x = (int16_t)(c->x + r);
				c->u = (int16_t)(c->u + (int16_t)(xb + 0xB0));
				c->v = (int16_t)(c->v + (int16_t)(yb + 0x80));
			}
		}
		Vertex *a = (Vertex *)game_addr(GRID_Vertices);
		Vertex *b = (Vertex *)game_addr(GRID_Centres);
		uint8_t *pk = (uint8_t *)game_addr(GRID_Packets);
		Triangle *t = (Triangle *)game_addr(GRID_Triangles);
		Vertex **cell = (Vertex **)game_addr(GRID_CellPointers);
		for (int row = 0; row < 14; row++, a++)
		{
			for (int col = 0; col < 20; col++, a++, b++, pk += 0x100, t += 4)
			{
				Vertex *tl = a, *tr = a + 1, *bl = a + 21, *br = a + 22, *cc = b;
				cell[2] = bl;
				cell[0] = tl;
				cell[1] = tr;
				cell[3] = br;
				cell[4] = cc;
				uint16_t tpage;
				uint8_t du;
				if (tl->u > 0xA0)
				{
					tpage = 0x1B | 0x120;
					du = 0x80;
				}
				else
				{
					tpage = 0x19 | 0x120;
					du = 0;
				}
				uint32_t uvTL = CellUV(tl, du, tpage), uvTR = CellUV(tr, du, tpage), uvBL = CellUV(bl, du, tpage);
				uint32_t uvBR = CellUV(br, du, tpage), uvC = CellUV(cc, du, tpage);
				for (int j = 7; j >= 0; j--) *(uint32_t *)(pk + 0x20 * j) = 0x7000000;
				for (int j = 7; j >= 0; j--) *(uint32_t *)(pk + 0x20 * j + 4) = 0x26808080;
				t[0].v[0] = tl; t[0].v[1] = tr; t[0].v[2] = cc;
				t[1].v[0] = tl; t[1].v[1] = cc; t[1].v[2] = bl;
				t[2].v[0] = tr; t[2].v[1] = br; t[2].v[2] = cc;
				t[3].v[0] = cc; t[3].v[1] = br; t[3].v[2] = bl;
				const uint32_t uv[4][3] = { { uvTL, uvTR, uvC }, { uvTL, uvC, uvBL }, { uvTR, uvBR, uvC }, { uvC, uvBR, uvBL } };
				for (int k = 0; k < 4; k++)
					for (int buf = 0; buf < 2; buf++)
						for (int q = 0; q < 3; q++) *(uint32_t *)(pk + 0x40 * k + 0x20 * buf + 0xC + 8 * q) = uv[k][q];
			}
		}
	}

	static void InitTriangles()
	{
		Triangle *t = (Triangle *)game_addr(GRID_Triangles);
		for (int i = 0; i < GRID_TriangleCount; i++, t++)
		{
			const Vertex *v0 = t->v[0], *v1 = t->v[1], *v2 = t->v[2];
			int32_t cy = Div3(v0->y + v1->y + v2->y);
			int32_t cx = Div3(v0->x + v1->x + v2->x);
			t->cx = (int16_t)cx;
			t->cy = (int16_t)cy;
			t->zero10 = 0;
			const Vertex *w = t->v[0];
			int32_t u = w->u, v = w->v;
			int32_t d = Sqrt(u * u + v * v);
			t->delay = (int16_t)Div350(mul32(d, 10));
			t->zero18 = 0;
			t->offy = 0;
			t->offx = 0;
			t->scale = 0x1000;
			t->vx = (int16_t)(CrtRand() % 6 + 1);
			t->vy = (int16_t)(CrtRand() % 25 + 4);
			t->ay = (int16_t)(-2 - CrtRand() % 8);
			t->dscale = (int16_t)(CrtRand() % 0x160 + 0x140);
		}
	}

	static void FadeSlots(int32_t &lo, int32_t &hi)
	{
		if (FadeSide() >= 3)
		{
			lo = 0;
			hi = 3;
		}
		else
		{
			lo = 3;
			hi = 7;
		}
	}

	static void SpawnFadeOuts(int16_t duration)
	{
		int32_t lo, hi;
		FadeSlots(lo, hi);
		for (int32_t s = lo; s < hi; s++)
		{
			uint16_t f = EntityFlags(s);
			if ((f & 2) && !(f & 4))
			{
				FadeFlag(s) = 1;
				FadeNode *t = (FadeNode *)AddTask(game_addr(ORIG_FadeOutTask));
				t->counter = 0;
				t->slot = (int16_t)s;
				t->duration = duration;
				t->saved = (uint16_t)(EntityFlags(s) & 0x800);
			}
			else FadeFlag(s) = 0;
		}
	}

	static void SpawnFadeIns(int16_t duration)
	{
		int32_t lo, hi;
		FadeSlots(lo, hi);
		for (int32_t s = lo; s < hi; s++)
		{
			if (!FadeFlag(s)) continue;
			FadeNode *t = (FadeNode *)AddTask(game_addr(ORIG_FadeInTask));
			t->counter = 0;
			t->slot = (int16_t)s;
			t->duration = duration;
			t->saved = (uint16_t)(EntityFlags(s) & 0x800);
		}
	}

	static void FadeColours(uint8_t *e, int32_t ir0)
	{
		uint32_t rgb = 0x32000000;
		GteSetFarColor(var<uint8_t>(game_addr(game::CURRENT_BS_SHADOW_PALETTE)), var<uint8_t>(game_addr(game::unk_B8B9A9)), var<uint8_t>(game_addr(game::unk_B8B9AA)));
		GteSetRGB(&rgb);
		GteSetIR0(ir0);
		GteDPCS();
		GteReadRGB2(e + 0x2C);
		GteSetFarColor(var<uint8_t>(game_addr(game::CURRENT_BS_MODEL_PALETTE)) >> 1, var<uint8_t>(game_addr(game::unk_B8B7D9)) >> 1, var<uint8_t>(game_addr(game::unk_B8B7DA)) >> 1);
		rgb = 0;
		GteSetRGB(&rgb);
		GteDPCS();
		GteReadRGB2(e + 0x28);
	}

	static uint32_t __cdecl FadeInTask(TaskNode *n)
	{
		FadeNode *p = (FadeNode *)n;
		int32_t t = shl32(p->counter, 12) / p->duration;
		uint8_t *e = Entity(p->slot);
		FadeColours(e, t);
		e[0x2B] = 2;
		if (DrawOnly()) return 0;
		if (p->counter == 0) EntityFlags(p->slot) = (uint16_t)((EntityFlags(p->slot) & 0xFFFB) | 0x800);
		p->counter++;
		if (p->counter < p->duration) return 0;
		uint32_t colour = var<uint32_t>(game_addr(game::CURRENT_BS_MODEL_PALETTE));
		e[7] = 0;
		*(uint32_t *)(e + 0x28) = colour;
		EntityFlags(p->slot) = (uint16_t)((EntityFlags(p->slot) & 0xF7FF) | p->saved);
		return TASK_END;
	}

	static uint32_t __cdecl FadeOutTask(TaskNode *n)
	{
		FadeNode *p = (FadeNode *)n;
		int32_t t = shl32(p->counter, 12) / p->duration;
		uint8_t *e = Entity(p->slot);
		e[7] = 0;
		FadeColours(e, 0x1000 - t);
		e[0x2B] = 2;
		if (DrawOnly()) return 0;
		if (p->counter == 0) e[1] |= 8;
		p->counter++;
		if (p->counter < p->duration) return 0;
		EntityFlags(p->slot) = (uint16_t)((EntityFlags(p->slot) & 0xF7FF) | p->saved | 4);
		return TASK_END;
	}

	static void StageGroupsFlag(bool set)
	{
		for (uint32_t a = game_addr(game::unk_1D98991); a < game_addr(game::unk_1D98A41); a += 0x2C)
		{
			if (set) *(uint8_t *)a |= 2;
			else *(uint8_t *)a &= 0xFD;
		}
	}

	static void IdentityRotation(Mat4x3 *m)
	{
		uint32_t *d = (uint32_t *)m;
		d[3] = 0;
		d[2] = 0;
		d[1] = 0;
		d[0] = 0;
		m->m[2][2] = 0x1000;
		m->m[1][1] = 0x1000;
		m->m[0][0] = 0x1000;
	}

	static void StageCapture()
	{
		uint32_t other = DisplayBuffer() ^ 1u;
		uint8_t *pk = (uint8_t *)FrameArena();
		int16_t rect[4] = { 0x240, 0x100, 0x140, 0xE8 };
		BuildDrawEnvPacket(pk, game_addr(DRAWENV) + 0x5C * other);
		InsertPrimAutoDepth(OTBase() + 0x406C, pk);
		pk += 0x40;
		FrameArena() = (uint32_t)pk;
		int32_t ofy, ofx;
		GetScreenOffset(&ofx, &ofy);
		SetScreenOffset(rect[2] / 2, rect[3] / 2);
		uint8_t *h = (uint8_t *)FieldAlloc(0x54);
		*(uint16_t *)(h + 0x3C) = 0;
		*(uint16_t *)(h + 0x3E) = 0;
		*(uint16_t *)(h + 0x40) = 0x140;
		*(uint16_t *)(h + 0x42) = 0xD8;
		*(uint32_t *)(h + 0x2C) = var<uint32_t>(game_addr(game::g_BattleVertexWorkspace));
		*(uint32_t *)(h + 0x38) = var<uint32_t>(game_addr(game::BATTLE_PACKET_BUFFER_END));
		*(uint32_t *)(h + 0x48) = 0xFFFFFFFF;
		for (int k = 0; k < 4; k++)
		{
			uint8_t *anim = (uint8_t *)(game_addr(STAGE_AnimHeaders) + 0x2C * k);
			const uint32_t *bucket = (const uint32_t *)(game_addr(STAGE_Buckets) + 8 * k);
			BuildBoneMatricesFromPose(anim);
			ComputeBonesWorldMatrices(anim, &Camera());
			if (anim[-0x13] & 1)
			{
				*(uint32_t *)(h + 0x44) = *(const uint32_t *)(anim + 0x14);
				*(uint16_t *)(h + 0x4C) = *(const uint16_t *)(anim - 0x12);
				FrameArena() = RenderGeometry(anim - 0x10, h + 0x28, OTBase() + bucket[0] * 4, bucket[1], FrameArena());
			}
		}
		FieldFree(0x54);
		SetScreenOffset(ofx, ofy);
		uint8_t *env = (uint8_t *)(game_addr(STAGE_EnvRestore) + 0x18 * DisplayBuffer());
		InsertPrimAltViewport(OTBase() + 0x4484, env);
		pk = (uint8_t *)FrameArena();
		BuildDrawAreaPacket(pk, rect);
		InsertPrimAutoDepth(OTBase() + 0x4484, pk);
		pk += 0xC;
		var<int16_t>(game_addr(game::unk_23941B8)) = rect[0];
		var<int16_t>(game_addr(game::unk_23941BA)) = rect[1];
		BuildDrawOffsetPacket(pk, (const void *)game_addr(game::unk_23941B8));
		InsertPrimAutoDepth(OTBase() + 0x4484, pk);
		pk += 0xC;
		FrameArena() = (uint32_t)pk;
	}

	static void ProjectGrid(uint8_t *h)
	{
		Mat4x3 *m = (Mat4x3 *)(h + 0x84);
		IdentityRotation(m);
		m->t[0] = 8;
		m->t[1] = 4;
		m->t[2] = ProjH();
		GteSetRotMatrix(m);
		GteSetTransVector(m);
		for (Vertex *v = (Vertex *)game_addr(GRID_Vertices); v < (Vertex *)game_addr(GRID_VerticesEnd); v++)
		{
			GteLoadV0(&v->x);
			GteRTPS();
			GteReadSXY2(v->sxy);
		}
		for (Vertex *v = (Vertex *)game_addr(GRID_Centres); v < (Vertex *)game_addr(GRID_CentresEnd); v++)
		{
			GteLoadV0(&v->x);
			GteRTPS();
			GteReadSXY2(v->sxy);
		}
	}

	static void DrawStaticTriangle(const Triangle *t, uint8_t *pkt, uint32_t colour)
	{
		*(uint32_t *)(pkt + 4) = colour | 0x24000000;
		*(uint32_t *)(pkt + 8) = *(const uint32_t *)t->v[0]->sxy;
		*(uint32_t *)(pkt + 0x10) = *(const uint32_t *)t->v[1]->sxy;
		*(uint32_t *)(pkt + 0x18) = *(const uint32_t *)t->v[2]->sxy;
		InsertPrimAutoDepth(OTBase() + 0x4068, pkt);
	}

	static void UpdateTriangle(Triangle *t)
	{
		int16_t ox = t->offx;
		t->offx = (int16_t)(t->vx + ox);
		int16_t vx = t->vx;
		t->vx = (int16_t)(vx + (int16_t)(vx >> 2));
		t->offy = (int16_t)(t->offy + t->vy);
		t->vy = (int16_t)(t->vy + (int16_t)(t->ay >> 1));
		t->scale = (int16_t)(t->scale - t->dscale);
		if (t->scale < 0x180) t->scale = 0x180;
	}

	static void ProjectTriangle(uint8_t *h, const Triangle *t)
	{
		const uint8_t *v0 = (const uint8_t *)t->v[0];
		*(uint32_t *)(h + 0x44) = *(const uint32_t *)(v0 + 4);
		*(uint32_t *)(h + 0x48) = *(const uint32_t *)(v0 + 8);
		*(int16_t *)(h + 0x46) = (int16_t)(*(int16_t *)(h + 0x46) - t->cy);
		*(int16_t *)(h + 0x44) = (int16_t)(*(int16_t *)(h + 0x44) - t->cx);
		const uint8_t *v1 = (const uint8_t *)t->v[1];
		*(uint32_t *)(h + 0x4C) = *(const uint32_t *)(v1 + 4);
		*(uint32_t *)(h + 0x50) = *(const uint32_t *)(v1 + 8);
		*(int16_t *)(h + 0x4C) = (int16_t)(*(int16_t *)(h + 0x4C) - t->cx);
		*(int16_t *)(h + 0x4E) = (int16_t)(*(int16_t *)(h + 0x4E) - t->cy);
		const uint8_t *v2 = (const uint8_t *)t->v[2];
		*(uint32_t *)(h + 0x54) = *(const uint32_t *)(v2 + 4);
		*(uint32_t *)(h + 0x58) = *(const uint32_t *)(v2 + 8);
		*(int16_t *)(h + 0x54) = (int16_t)(*(int16_t *)(h + 0x54) - t->cx);
		*(int16_t *)(h + 0x56) = (int16_t)(*(int16_t *)(h + 0x56) - t->cy);
		Mat4x3 *m = (Mat4x3 *)(h + 0x64);
		IdentityRotation(m);
		m->m[1][1] = t->scale;
		m->m[0][0] = t->scale;
		GteSetRotMatrixCtrl(m);
		GteSetTransVectorCtrl(m);
		GteLoadV012(h + 0x44, h + 0x4C, h + 0x54);
		GteRTPT();
	}

	static void FlatTriangle(uint8_t *pkt)
	{
		*(uint32_t *)pkt = 0x4000000;
		*(uint32_t *)(pkt + 4) = 0x20FFFFFF;
		GteReadSXY012(pkt + 8, pkt + 0xC, pkt + 0x10);
		InsertPrimAutoDepth(OTBase() + 0x4064, pkt);
	}

	static void EmitMovingTriangle(const Triangle *t, uint8_t *pkt, uint32_t colour)
	{
		if (t->scale == 0x180)
		{
			FlatTriangle(pkt);
			return;
		}
		*(uint32_t *)(pkt + 4) = colour | 0x26000000;
		GteReadSXY012(pkt + 8, pkt + 0x10, pkt + 0x18);
		InsertPrimAutoDepth(OTBase() + 0x4064, pkt);
	}

	static void GridDraw(int32_t counter, int32_t buf, uint32_t colour)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		ProjectGrid(h);
		*(int32_t *)(h + 0x30) = ProjH();
		int32_t drawn = 0;
		uint8_t *pk = (uint8_t *)game_addr(GRID_Packets);
		Triangle *t = (Triangle *)game_addr(GRID_Triangles);
		for (int i = 0; i < GRID_TriangleCount; i++, t++, pk += 0x40)
		{
			if (t->delay < 0) continue;
			if (t->delay >= counter)
			{
				DrawStaticTriangle(t, pk + buf * 0x20, colour);
				drawn++;
				continue;
			}
			int32_t x = t->offx + t->cx;
			int32_t y = t->offy + t->cy;
			*(int32_t *)(h + 0x78) = x;
			*(int32_t *)(h + 0x7C) = y;
			if (x > 0xA8 || y < -0x74)
			{
				t->delay = -1;
				continue;
			}
			*(int32_t *)(h + 0x78) = x + 8;
			*(int32_t *)(h + 0x80) = *(int32_t *)(h + 0x30);
			*(int32_t *)(h + 0x7C) = y + 4;
			if (!DrawOnly()) UpdateTriangle(t);
			ProjectTriangle(h, t);
			if (GridTextured()) EmitMovingTriangle(t, pk + buf * 0x20, colour);
			else if (t->scale == 0x180)
			{
				FlatTriangle((uint8_t *)PacketCursor());
				PacketCursor() += 0x20;
			}
			drawn++;
		}
		if (!drawn) GridActive() = 0;
		FieldFree(0xB4);
	}

	static uint32_t GridColour(int32_t c)
	{
		int32_t level = c * 12 + 0x80;
		if (level > 0xFF) level = 0xFF;
		return (uint32_t)level | ((uint32_t)level << 8) | ((uint32_t)level << 16);
	}

	static uint32_t __cdecl ShatterTask(TaskNode *n)
	{
		ShatterNode *p = (ShatterNode *)n;
		if (p->counter < 0x32 && GridActive())
		{
			if (p->counter < 10) StageCapture();
			int32_t c = p->counter;
			GridDraw(c, DisplayBuffer(), GridColour(c));
		}
		if (DrawOnly()) return 0;
		if (p->counter == 0) StageGroupsFlag(false);
		if (p->counter == 6) SpawnFadeOuts(6);
		p->counter++;
		return p->counter > 0x32 ? TASK_END : 0;
	}

	static void SparkleDraw(uint8_t *h, int16_t *s, const Sparkle *e)
	{
		int32_t a = e->angle;
		*(uint16_t *)(h + 4) = e->frame;
		int32_t r = e->radius;
		s[4] = (int16_t)((int16_t)(mul32(ComputeCos(a), r) >> 12) + s[0]);
		s[5] = e->y;
		s[6] = (int16_t)((int16_t)(mul32(ComputeSin(a), r) >> 12) + s[2]);
		ShadowRotation(s + 4, e->size, -(e->size >> 4));
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
	}

	static uint32_t __cdecl SparkleTask(TaskNode *n)
	{
		SparkleNode *p = (SparkleNode *)n;
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		int16_t *s = (int16_t *)FieldAlloc(0x10);
		*(uint32_t *)s = *(const uint32_t *)&p->pos[0];
		*(uint32_t *)h = game_addr(SEQ_Sparkle);
		*(uint16_t *)(h + 0x24) = 0;
		*(uint32_t *)(s + 2) = *(const uint32_t *)&p->pos[2];
		int32_t alive = 0;
		for (Sparkle *e = (Sparkle *)game_addr(SPARKLES); e < (Sparkle *)game_addr(SPARKLES_End); e++)
		{
			if (!(e->active & 1)) continue;
			SparkleDraw(h, s, e);
			if (DrawOnly()) continue;
			e->frame++;
			if (*(int16_t *)(h + 0x28) < 0) e->active = 0;
			else
			{
				e->radius = (int16_t)(e->radius - e->dradius);
				e->y = (int16_t)(e->y + e->dy);
				e->angle = (int16_t)(e->angle + e->dangle);
				alive++;
			}
		}
		FieldFree(0x10);
		FieldFree(0xB4);
		if (DrawOnly()) return 0;
		if (p->counter >= 0 && p->counter <= 0x42)
		{
			for (int j = 0; j < 2; j++)
			{
				Sparkle *e = (Sparkle *)game_addr(SPARKLES);
				int idx = 0;
				while (e->active)
				{
					e++;
					idx++;
					if (e >= (Sparkle *)game_addr(SPARKLES_End)) goto spawned;
				}
				if (idx >= 100) goto spawned;
				e->active = 1;
				e->frame = 0;
				e->size = (int16_t)(CrtRand() % 0x300 + 0x180);
				e->radius = (int16_t)(CrtRand() % 0x1F40 + 0x258);
				e->y = (int16_t)(0xC8 - CrtRand() % 0xBB8);
				e->angle = (int16_t)(CrtRand() % 0x1000);
				int32_t d = CrtRand() % 14 + 0x1F;
				e->dradius = (int16_t)(e->radius / d);
				e->dy = (int16_t)((p->pos[1] - e->y) / d);
				e->dangle = (int16_t)(-10 - CrtRand() % 30);
			}
		}
	spawned:
		p->counter++;
		if (p->counter >= 0x1F && !alive) return TASK_END;
		return 0;
	}

	static void OrbitCtrlMatrix(const OrbitCtrlNode *p, Mat4x3 *m)
	{
		int16_t ang[3] = { p->ax, 0, p->az };
		BuildRotationMatrixFromAngles(ang, m);
		m->t[0] = p->pos[0];
		m->t[1] = p->pos[1];
		m->t[2] = p->pos[2];
	}

	static void OrbitCtrlUpdate(OrbitCtrlNode *p)
	{
		if (p->counter >= 6 && p->counter - 6 < 0x34)
		{
			p->ax = (int16_t)(p->ax + p->dax);
			p->az = (int16_t)(p->az + p->daz);
		}
	}

	static uint32_t __cdecl OrbitCtrlTask(TaskNode *n)
	{
		OrbitCtrlNode *p = (OrbitCtrlNode *)n;
		OrbitCtrlMatrix(p, OrbitMatrix(p->index));
		if (DrawOnly()) return 0;
		if (p->counter == 0)
		{
			int32_t r = CrtRand() % 0x1000;
			for (int k = 0; k < 8; k++)
			{
				OrbitNode *o = (OrbitNode *)AddTask(game_addr(ORIG_OrbitTask));
				o->index = p->index;
				o->k = (int16_t)k;
				o->counter = 0;
				o->phase = 0x800;
				o->size = 0xA00;
				o->radius = 0xFA0;
				o->dradius = 0x258;
				int32_t v = (int16_t)((EntitySize(p->target) * 2200) >> 12);
				o->base = (int16_t)((v * 3) / 4);
				o->angle = (int16_t)((k * 0x200 + r) & 0xFFF);
			}
		}
		OrbitCtrlUpdate(p);
		p->counter++;
		return p->counter >= 0x4B ? TASK_END : 0;
	}

	static void OrbitDraw(const OrbitNode *p, const Mat4x3 *m)
	{
		int32_t r = p->radius + p->base;
		int16_t v[4];
		v[0] = (int16_t)(mul32(ComputeCos(p->angle), r) >> 12);
		v[1] = 0;
		v[2] = (int16_t)(mul32(ComputeSin(p->angle), r) >> 12);
		v[3] = 0;
		MatrixMultiplyVector(m, v, v);
		v[0] = (int16_t)(v[0] + (int16_t)m->t[0]);
		v[2] = (int16_t)(v[2] + (int16_t)m->t[2]);
		v[1] = (int16_t)(v[1] + (int16_t)m->t[1]);
		ShadowRotation(v, p->size, -(p->size >> 4));
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		*(uint16_t *)(h + 4) = (uint16_t)(*(const uint8_t *)&p->counter & 7);
		*(uint32_t *)h = game_addr(SEQ_OrbitSpark);
		*(uint16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static void OrbitUpdate(OrbitNode *p)
	{
		int32_t c = p->counter;
		if (c < 6)
		{
			int16_t d = p->dradius;
			p->radius = (int16_t)(p->radius - d);
			p->dradius = (int16_t)(d - Div20(d));
		}
		else if (c - 6 < 0x34)
		{
			if (c == 6) p->dradius = p->radius;
			p->phase = (int16_t)(p->phase + 0x78);
			int32_t s = ComputeSin(p->phase);
			p->angle = (int16_t)(p->angle + 0x3C);
			p->radius = (int16_t)((int16_t)((s * 400) >> 12) + p->dradius);
		}
		else if (c - 0x3A < 0x10)
		{
			int32_t d = c - 0x3A;
			if (d == 0)
			{
				p->dradius = p->radius;
				p->phase = (int16_t)((p->radius + p->base) / 8);
			}
			int32_t a = (d << 11) / 16;
			if (d >= 8) p->dradius = (int16_t)(p->dradius - p->phase);
			int32_t s = ComputeSin(a);
			p->radius = (int16_t)((int16_t)((s * 3000) >> 12) + p->dradius);
			p->angle = (int16_t)(p->angle + 0x1E);
		}
		else if (c - 0x4A < 1) p->radius = (int16_t)-p->base;
	}

	static uint32_t __cdecl OrbitTask(TaskNode *n)
	{
		OrbitNode *p = (OrbitNode *)n;
		OrbitDraw(p, OrbitMatrix(p->index));
		if (DrawOnly()) return 0;
		OrbitUpdate(p);
		p->counter++;
		return p->counter >= 0x4B ? TASK_END : 0;
	}

	static void MoteDraw(uint8_t *h, uint8_t *m, const Mote *e)
	{
		int32_t level = e->age * 21;
		if (level > 0x80) level = 0x80;
		uint32_t colour = (((uint32_t)level << 8 | (uint32_t)level) << 8) | (uint32_t)level;
		*(int32_t *)(m + 0x34) = 0;
		*(int32_t *)(m + 0x30) = 0;
		*(int32_t *)(m + 0x2C) = 0;
		*(int32_t *)(m + 0x3C) = e->x;
		*(uint32_t *)(h + 0x1C) = colour;
		*(uint16_t *)(h + 4) = e->frame;
		*(int32_t *)(m + 0x28) = 0;
		*(int16_t *)(m + 0x38) = e->scale;
		*(int16_t *)(m + 0x30) = e->scale;
		*(int16_t *)(m + 0x28) = e->scale;
		*(int32_t *)(m + 0x40) = e->y;
		*(int32_t *)(m + 0x44) = e->z;
		Mat4x3 *w = (Mat4x3 *)(m + 0x28);
		ComposeAffineTransform((const Mat4x3 *)(m + 8), w, w);
		GteSetRotMatrix(w);
		GteSetTransVector(w);
		FrameArena() = InitEffectSequenceFromData(h, OTBase() + 0x4064, 0xE, FrameArena());
	}

	static uint32_t __cdecl MoteTask(TaskNode *n)
	{
		EmitterNode *p = (EmitterNode *)n;
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		uint8_t *m = (uint8_t *)FieldAlloc(0x48);
		int32_t alive = 0;
		*(uint32_t *)h = game_addr(SEQ_Mote);
		*(uint16_t *)(h + 0x24) = 0xC;
		int16_t *ang = (int16_t *)m;
		ang[0] = 0;
		ang[1] = 0;
		ang[2] = 0x1C2;
		BuildRotationMatrixFromAngles(ang, (Mat4x3 *)(m + 8));
		*(int32_t *)(m + 0x1C) = 0;
		*(int32_t *)(m + 0x20) = 0;
		*(int32_t *)(m + 0x24) = ProjH();
		for (Mote *e = (Mote *)game_addr(MOTES); e < (Mote *)game_addr(MOTES_End); e++)
		{
			if (!(e->active & 1)) continue;
			MoteDraw(h, m, e);
			if (DrawOnly()) continue;
			e->age++;
			if (e->age < e->life)
			{
				e->y = (int16_t)(e->y + e->speed);
				e->speed = (int16_t)(e->speed + (int16_t)(e->speed >> 3));
				alive++;
			}
			else e->active = 0;
		}
		FieldFree(0x48);
		FieldFree(0xB4);
		if (DrawOnly()) return 0;
		if (p->counter >= 0 && p->counter <= 0x3A)
		{
			for (int j = 0; j < 2; j++)
			{
				Mote *e = (Mote *)game_addr(MOTES);
				int idx = 0;
				while (e->active)
				{
					e++;
					idx++;
					if (e >= (Mote *)game_addr(MOTES_End)) goto spawned;
				}
				if (idx >= 0x50) goto spawned;
				e->active = 1;
				e->frame = (uint16_t)(CrtRand() & 3);
				e->scale = (int16_t)(CrtRand() % 0x90 + 0x80);
				e->x = (int16_t)(CrtRand() % 0x154 - 0xAA);
				e->y = (int16_t)(CrtRand() % 0x15E - 0x32);
				e->z = 0;
				e->age = 0;
				e->speed = (int16_t)(-1 - CrtRand() % 10);
				e->life = (int16_t)(CrtRand() % 10 + 10);
			}
		}
	spawned:
		p->counter++;
		if (p->counter >= 4 && !alive) return TASK_END;
		return 0;
	}

	static void ModelGte(const int16_t ang[3], const int16_t *pos, int32_t scale)
	{
		Mat4x3 m = {};
		ComposeZYXRotationMatrix(ang, &m);
		m.t[0] = pos[0];
		m.t[1] = pos[1];
		m.t[2] = pos[2];
		int32_t s[3] = { scale, scale, scale };
		Scale3DMatrix(&m, s);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
	}

	static uint8_t *ModelHeader(uint32_t model)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		*(uint32_t *)h = model;
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0x1C) = 0x33;
		return h;
	}

	static void ModelFade(uint8_t *h, int32_t fade)
	{
		*(uint32_t *)(h + 0x1C) = 0xF3;
		*(int32_t *)(h + 0xC) = fade;
	}

	static void RingDraw(const RingNode *p)
	{
		int16_t ang[3] = { 0, p->angle, 0 };
		ModelGte(ang, p->pos, p->scale);
		uint8_t *h = ModelHeader(game_addr(MODEL_Ring));
		if (p->counter < 8)
		{
			int32_t fade = (8 - p->counter) << 9;
			ModelFade(h, fade);
		}
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static uint32_t __cdecl RingTask(TaskNode *n)
	{
		RingNode *p = (RingNode *)n;
		RingDraw(p);
		if (DrawOnly()) return 0;
		p->angle = (int16_t)(p->angle + p->spin);
		p->counter++;
		return p->counter >= 0x3A ? TASK_END : 0;
	}

	static void WaveDraw(const RingNode *p)
	{
		int16_t ang[3] = { 0, 0, 0 };
		ModelGte(ang, p->pos, p->scale);
		uint8_t *h = (uint8_t *)FieldAlloc(0x90);
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0x10) = 0;
		*(uint16_t *)(h + 0x18) = 0;
		*(uint32_t *)(h + 0x24) = *(const uint32_t *)(game_addr(WAVE_Textures) + 4 * (*(const uint8_t *)&p->w22 & 0xF));
		int32_t c = p->counter;
		int32_t scroll = p->w20 * c;
		*(uint32_t *)h = game_addr(MODEL_Wave);
		*(uint32_t *)(h + 0x20) = 0x33;
		*(int32_t *)(h + 0x14) = scroll;
		*(uint16_t *)(h + 0x1A) = 0x80;
		*(uint16_t *)(h + 0x1C) = 0x40;
		*(uint16_t *)(h + 0x1E) = 0x80;
		if (c < 0x38)
		{
			int32_t fade = 0x1000 - c * 73;
			*(uint32_t *)(h + 0x20) = 0xF3;
			*(int32_t *)(h + 0xC) = fade;
		}
		FrameArena() = RenderWaveModel(h, RenderOT(), 2, FrameArena());
		FieldFree(0x90);
	}

	static uint32_t __cdecl WaveTask(TaskNode *n)
	{
		RingNode *p = (RingNode *)n;
		WaveDraw(p);
		if (DrawOnly()) return 0;
		p->w22++;
		p->counter++;
		return p->counter >= 0x3A ? TASK_END : 0;
	}

	static void ExpandDraw(const RingNode *p)
	{
		int16_t ang[3] = { 0, 0, 0 };
		ModelGte(ang, p->pos, p->scale);
		uint8_t *h = ModelHeader(game_addr(MODEL_Expand));
		if (p->counter >= 8)
		{
			int32_t fade = (p->counter - 8) << 9;
			ModelFade(h, fade);
		}
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static void ExpandUpdate(RingNode *p)
	{
		int16_t d = p->dscale;
		p->scale = (int16_t)(p->scale + d);
		p->dscale = (int16_t)(d - d / 32);
	}

	static uint32_t __cdecl ExpandTask(TaskNode *n)
	{
		RingNode *p = (RingNode *)n;
		ExpandDraw(p);
		if (DrawOnly()) return 0;
		ExpandUpdate(p);
		p->counter++;
		return p->counter >= 0x10 ? TASK_END : 0;
	}

	static void PillarDraw(const RingNode *p)
	{
		int16_t ang[3] = { p->angle, p->pos[3], 0 };
		ModelGte(ang, p->pos, p->scale);
		uint8_t *h = ModelHeader(game_addr(MODEL_Pillar));
		int32_t c = p->counter;
		if (c < 8)
		{
			int32_t fade = (8 - c) << 9;
			ModelFade(h, fade);
		}
		else if (c >= 0x10)
		{
			int32_t fade = (c - 0x10) * 409;
			ModelFade(h, fade);
		}
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static void PillarUpdate(RingNode *p)
	{
		if (p->counter < 0xC) return;
		int16_t v = p->spin;
		p->angle = (int16_t)(p->angle + v);
		p->spin = (int16_t)(v + (int16_t)(v >> 4));
	}

	static uint32_t __cdecl PillarTask(TaskNode *n)
	{
		RingNode *p = (RingNode *)n;
		if (p->delay > 0)
		{
			if (DrawOnly()) return 0;
			p->delay--;
			return 0;
		}
		PillarDraw(p);
		if (DrawOnly()) return 0;
		PillarUpdate(p);
		p->counter++;
		return p->counter >= 0x1A ? TASK_END : 0;
	}

	static Mat4x3 *ScreenSpriteMatrix(const int16_t *pos, int16_t size, int32_t depth)
	{
		Mat4x3 *m = SpriteMatrix();
		m->m[2][2] = size;
		m->m[1][1] = size;
		m->m[0][0] = size;
		GteSetRotMatrix(&Camera());
		GteSetTransVector(&Camera());
		GteLoadV0(pos);
		GteRTPS();
		int16_t sxy[2];
		GteReadSXY2(sxy);
		int16_t sx = (int16_t)(sxy[0] / 8);
		int16_t sy = (int16_t)(sxy[1] / 8);
		m->t[0] = sx - 0xA0;
		m->t[1] = sy - 0x6C;
		m->t[2] = ProjH() + depth;
		GteSetRotMatrix(m);
		GteSetTransVector(m);
		return m;
	}

	static void BeamDraw(const RingNode *p)
	{
		int32_t a[3] = { 0, 0, -0x1000 };
		int32_t b[3];
		b[0] = var<int16_t>(game_addr(game::Battle_Camera_world_XZ_s16)) - p->pos[0];
		b[1] = var<int16_t>(game_addr(game::unk_B8B7F2)) - p->pos[1];
		b[2] = var<int16_t>(game_addr(game::Battle_Camera_world_Y)) - p->pos[2];
		NormalizeVector(b, b);
		int32_t axis[3];
		int32_t angle = RotationBetweenVectors(a, b, axis);
		Mat4x3 r = {};
		BuildAxisAngleRotationMatrix(angle, &r, axis);
		r.t[0] = p->pos[0];
		r.t[1] = p->pos[1];
		r.t[2] = p->pos[2];
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		int32_t c = p->counter;
		*(uint32_t *)h = game_addr(SEQ_Beam);
		*(uint16_t *)(h + 0x24) = 0;
		bool lit = false;
		int32_t level = 0;
		if (c < 2)
		{
			level = c << 6;
			lit = true;
		}
		else if (c >= 0x10)
		{
			level = (0x18 - c) << 4;
			lit = true;
		}
		if (lit)
		{
			*(uint16_t *)(h + 0x24) = 4;
			*(uint32_t *)(h + 0x1C) = (((uint32_t)level << 8) | (uint32_t)level) << 8;
		}
		int32_t d[3] = { -0x200 - c * 300, -0x100 - c * 30, c * 25 - 0xE00 };
		NormalizeVector(d, d);
		for (uint32_t k = 0; k < 8; k++)
		{
			const int16_t *e = (const int16_t *)(game_addr(BEAM_Sprites) + 8 * k);
			int32_t dist = (e[2] * 2000) >> 12;
			int16_t v[4];
			v[0] = (int16_t)(mul32(dist, d[0]) >> 12);
			v[1] = (int16_t)(mul32(dist, d[1]) >> 12);
			v[2] = (int16_t)(mul32(dist, d[2]) >> 12);
			v[3] = 0;
			MatrixMultiplyVector(&r, v, v);
			v[0] = (int16_t)(v[0] + (int16_t)r.t[0]);
			v[1] = (int16_t)(v[1] + (int16_t)r.t[1]);
			v[2] = (int16_t)(v[2] + (int16_t)r.t[2]);
			ScreenSpriteMatrix(v, e[1], 0);
			*(uint16_t *)(h + 4) = (uint16_t)e[0];
			PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
		}
		FieldFree(0xB4);
	}

	static uint32_t __cdecl BeamTask(TaskNode *n)
	{
		RingNode *p = (RingNode *)n;
		BeamDraw(p);
		if (DrawOnly()) return 0;
		p->counter++;
		return p->counter >= 0x18 ? TASK_END : 0;
	}

	static void DiskDraw(const RingNode *p)
	{
		Mat4x3 *s = ShadowRotation(p->pos, p->scale, 0);
		int16_t ang[3] = { 0, 0, p->angle };
		Mat4x3 m = {};
		ComposeZYXRotationMatrix(ang, &m);
		MatrixMultiply(s, &m);
		GteSetRotMatrix(&m);
		uint8_t *h = ModelHeader(game_addr(MODEL_Disk));
		PacketCursor() = RenderPrimModel(h, RenderOT(), 0xD, PacketCursor());
		FieldFree(0x58);
	}

	static void DiskUpdate(RingNode *p)
	{
		int16_t spin = p->spin, d = p->dscale;
		p->angle = (int16_t)(p->angle + spin);
		p->scale = (int16_t)(p->scale + d);
		if (p->scale < 0) p->scale = 0x7FFF;
		p->dscale = (int16_t)(d / 8 + d);
	}

	static uint32_t __cdecl DiskTask(TaskNode *n)
	{
		RingNode *p = (RingNode *)n;
		DiskDraw(p);
		if (DrawOnly()) return 0;
		DiskUpdate(p);
		p->counter++;
		return p->counter >= 0x18 ? TASK_END : 0;
	}

	static void FlashDraw(const RingNode *p)
	{
		Mat4x3 *m = ScreenSpriteMatrix(p->pos, p->scale, 0);
		m->m[0][0] = p->scale;
		m->m[1][1] = 0x1000;
		GteSetRotMatrix(m);
		uint8_t *h = ModelHeader(game_addr(MODEL_Flash));
		int32_t c = p->counter;
		if (c < 4)
		{
			int32_t fade = (4 - c) << 10;
			ModelFade(h, fade);
		}
		else if (c >= 0x12)
		{
			int32_t fade = (c - 0x12) * 682;
			ModelFade(h, fade);
		}
		FrameArena() = RenderPrimModel(h, RenderOT(), 0xE, FrameArena());
		FieldFree(0x58);
	}

	static void FlashUpdate(RingNode *p)
	{
		if (p->counter < 8) return;
		int16_t d = p->dscale;
		p->scale = (int16_t)(p->scale - d);
		p->dscale = (int16_t)(d - Div6(d));
	}

	static uint32_t __cdecl FlashTask(TaskNode *n)
	{
		RingNode *p = (RingNode *)n;
		FlashDraw(p);
		if (DrawOnly()) return 0;
		FlashUpdate(p);
		p->counter++;
		return p->counter > 0x18 ? TASK_END : 0;
	}

	static void CopyAnchor(int16_t *dst, const DirectorNode *p)
	{
		*(uint32_t *)&dst[0] = *(const uint32_t *)&p->pos[0];
		*(uint32_t *)&dst[2] = *(const uint32_t *)&p->pos[2];
	}

	static uint32_t __cdecl DirectorTask(TaskNode *n)
	{
		if (DrawOnly()) return 0;
		DirectorNode *p = (DirectorNode *)n;
		if (p->counter == 0)
		{
			GetDefaultEffectPosition(Entity(p->target), p->pos);
			GridTextured() = 1;
			GridActive() = 1;
		}
		if (p->counter == 3) BuildGrid();
		if (p->counter == 4) InitTriangles();
		if (p->counter == 5)
		{
			ShatterNode *t = (ShatterNode *)AddTask(game_addr(ORIG_ShatterTask));
			t->counter = 0;
			FadeSide() = p->target;
		}
		if (p->counter == 1)
		{
			SparkleNode *t = (SparkleNode *)AddTask(game_addr(ORIG_SparkleTask));
			CopyAnchor(t->pos, p);
			t->counter = 0;
		}
		if (p->counter == 6)
		{
			OrbitCtrlNode *a = (OrbitCtrlNode *)AddTask(game_addr(ORIG_OrbitCtrlTask));
			CopyAnchor(a->pos, p);
			a->counter = 0;
			a->index = 0;
			a->target = p->target;
			a->ax = 0;
			a->az = 0;
			a->dax = (int16_t)(CrtRand() % 50 + 0x3C);
			a->daz = (int16_t)(CrtRand() % 40 + 0x1E);
			OrbitCtrlNode *b = (OrbitCtrlNode *)AddTask(game_addr(ORIG_OrbitCtrlTask));
			CopyAnchor(b->pos, p);
			b->target = p->target;
			b->counter = 0;
			b->index = 1;
			b->ax = 0;
			b->dax = (int16_t)-a->dax;
			b->az = 0;
			b->daz = (int16_t)-a->daz;
		}
		if (p->counter == 0x10)
		{
			EmitterNode *t = (EmitterNode *)AddTask(game_addr(ORIG_MoteTask));
			t->counter = 0;
		}
		if (p->counter == 0x1E)
		{
			GridTextured() = 0;
			QueueTimUpload(TIM_Upload());
		}
		if (p->counter == 0x1F)
		{
			RingNode *r = (RingNode *)AddTask(game_addr(ORIG_RingTask));
			CopyAnchor(r->pos, p);
			r->pos[1] = (int16_t)(r->pos[1] + 0x1F4);
			r->counter = 0;
			r->scale = 0x1000;
			r->angle = (int16_t)(CrtRand() % 0x1000);
			r->spin = (int16_t)(CrtRand() % 20 + 0x1E);
			RingNode *w = (RingNode *)AddTask(game_addr(ORIG_WaveTask));
			CopyAnchor(w->pos, p);
			w->pos[1] = (int16_t)(w->pos[1] + 0x258);
			w->counter = 0;
			w->scale = 0x1000;
			w->w20 = 0xC;
			w->w22 = 0;
			w = (RingNode *)AddTask(game_addr(ORIG_WaveTask));
			CopyAnchor(w->pos, p);
			w->pos[1] = (int16_t)(w->pos[1] + 0x258);
			w->counter = 0;
			w->scale = 0xF00;
			w->w20 = 4;
			w->w22 = 8;
		}
		if (p->counter >= 0x1F && p->counter <= 0x49 && p->counter % 6 == 1)
		{
			RingNode *e = (RingNode *)AddTask(game_addr(ORIG_ExpandTask));
			CopyAnchor(e->pos, p);
			e->pos[1] = (int16_t)(e->pos[1] + 0x5DC);
			e->counter = 0;
			e->dscale = 0x2AA;
			e->scale = 0x2AA;
		}
		if (p->counter == 0x2D)
		{
			int32_t r = CrtRand() % 0x1000;
			int32_t acc = 0;
			for (int k = 0; k < 12; k++, acc += 0x1000)
			{
				RingNode *q = (RingNode *)AddTask(game_addr(ORIG_PillarTask));
				q->delay = (int16_t)(*(const int16_t *)(game_addr(PILLAR_Delays) + 4 * k) * 3);
				q->counter = 0;
				int32_t ang = (Div12(acc) + r) & 0xFFF;
				int32_t cs = ComputeCos(ang);
				q->pos[1] = (int16_t)(p->pos[1] + 0x578);
				q->pos[0] = (int16_t)((int16_t)((cs * 2100) >> 12) + p->pos[0]);
				int32_t sn = ComputeSin(ang);
				q->angle = 0;
				q->pos[3] = (int16_t)(0xC00 - ang);
				q->pos[2] = (int16_t)((int16_t)((sn * 2100) >> 12) + p->pos[2]);
				q->spin = (int16_t)(CrtRand() % 30 + 0x1E);
				q->scale = 0x1000;
			}
		}
		if (p->counter == 0x42)
		{
			RingNode *b = (RingNode *)AddTask(game_addr(ORIG_BeamTask));
			CopyAnchor(b->pos, p);
			b->counter = 0;
		}
		if (p->counter == 0x4E)
		{
			RingNode *d = (RingNode *)AddTask(game_addr(ORIG_DiskTask));
			CopyAnchor(d->pos, p);
			d->counter = 0;
			d->angle = (int16_t)(CrtRand() % 0x1000);
			d->spin = 0x28;
			d->scale = 0;
			d->dscale = 0x140;
			d = (RingNode *)AddTask(game_addr(ORIG_DiskTask));
			CopyAnchor(d->pos, p);
			d->counter = 0;
			d->angle = (int16_t)(CrtRand() % 0x1000);
			d->spin = -0x28;
			d->scale = 0;
			d->dscale = 0x280;
		}
		if (p->counter == 0x5A)
		{
			RingNode *f = (RingNode *)AddTask(game_addr(ORIG_FlashTask));
			CopyAnchor(f->pos, p);
			f->counter = 0;
			f->scale = 0x3C00;
			f->dscale = 0xA00;
			f->w20 = 0x3C00;
		}
		if (p->counter >= 0x67)
		{
			if (p->counter == 0x67)
			{
				StageGroupsFlag(true);
				SpawnFadeIns(8);
			}
			SetScreenFlash((uint32_t)((0x6F - p->counter) << 9), 0);
		}
		if (p->counter == 0x6A) ApplyActionResultToTarget(Ctx()->actions[p->action].targets);
		if (p->counter == 0x6F)
		{
			int32_t next = p->action + 1;
			if (next <= Ctx()->actions[0].last_action)
			{
				DirectorNode *d = (DirectorNode *)AddTask(game_addr(ORIG_DirectorTask));
				d->counter = 0;
				d->action = (int16_t)next;
				d->target = Ctx()->actions[next].targets[0];
			}
		}
		if (p->counter == 1)
		{
			int16_t pos[4];
			GetDefaultEffectPosition(Entity(p->target), pos);
			BdPlaySE3D(SOUND_Holy(), 1, pos);
		}
		p->counter++;
		if (p->counter <= 0x6F) return 0;
		if ((uint16_t)p->action == (uint16_t)Ctx()->actions[0].last_action) SetScreenFlash(0, 0);
		return TASK_END;
	}
}

	void register_mag175_holy()
	{
		register_port(holy175::ORIG_RootTask, (void *)holy175::RootTask, "H175 RootTask", 175);
		register_port(holy175::ORIG_RestoreTask, (void *)holy175::RestoreTask, "H175 RestoreTask", 175);
		register_port(holy175::ORIG_DirectorTask, (void *)holy175::DirectorTask, "H175 DirectorTask", 175);
		register_port(holy175::ORIG_ShatterTask, (void *)holy175::ShatterTask, "H175 ShatterTask", 175);
		register_port(holy175::ORIG_FadeInTask, (void *)holy175::FadeInTask, "H175 FadeInTask", 175);
		register_port(holy175::ORIG_FadeOutTask, (void *)holy175::FadeOutTask, "H175 FadeOutTask", 175);
		register_port(holy175::ORIG_SparkleTask, (void *)holy175::SparkleTask, "H175 SparkleTask", 175);
		register_port(holy175::ORIG_OrbitCtrlTask, (void *)holy175::OrbitCtrlTask, "H175 OrbitCtrlTask", 175);
		register_port(holy175::ORIG_OrbitTask, (void *)holy175::OrbitTask, "H175 OrbitTask", 175);
		register_port(holy175::ORIG_MoteTask, (void *)holy175::MoteTask, "H175 MoteTask", 175);
		register_port(holy175::ORIG_RingTask, (void *)holy175::RingTask, "H175 RingTask", 175);
		register_port(holy175::ORIG_WaveTask, (void *)holy175::WaveTask, "H175 WaveTask", 175);
		register_port(holy175::ORIG_ExpandTask, (void *)holy175::ExpandTask, "H175 ExpandTask", 175);
		register_port(holy175::ORIG_PillarTask, (void *)holy175::PillarTask, "H175 PillarTask", 175);
		register_port(holy175::ORIG_BeamTask, (void *)holy175::BeamTask, "H175 BeamTask", 175);
		register_port(holy175::ORIG_DiskTask, (void *)holy175::DiskTask, "H175 DiskTask", 175);
		register_port(holy175::ORIG_FlashTask, (void *)holy175::FlashTask, "H175 FlashTask", 175);
	}
}

