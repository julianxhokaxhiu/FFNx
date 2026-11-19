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

// Effect 207: Maelstrom

#include "mag_common.h"

namespace ff8fx
{
namespace maelstrom207
{
	using namespace eng;
	using namespace magc;

	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::MAG_207_Maelstrom_MagicBufferBase)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::MAG_207_Maelstrom_CastContext)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_207_Maelstrom_PacketCursor)); }
	inline TaskQueue *QParticles() { return (TaskQueue *)game_addr(game::MAG_207_Maelstrom_ParticlesQueue); }
	inline TaskQueue *QMain() { return (TaskQueue *)game_addr(game::MAG_207_Maelstrom_MainQueue); }
	inline Mat4x3 &WarpFrame() { return var<Mat4x3>(game_addr(game::MAG_207_Maelstrom_WarpFrame)); }
	inline Mat4x3 &ModuleMatrix() { return var<Mat4x3>(game_addr(game::MAG_207_Maelstrom_ModuleMatrix)); }
	inline uint32_t OTBase() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)); }

	static constexpr game::Id ORIG_MasterTask = game::MAG_207_Maelstrom_MasterTask;
	static constexpr game::Id ORIG_MainTask = game::MAG_207_Maelstrom_MainTask;
	static constexpr game::Id ORIG_ParticleTask = game::MAG_207_Maelstrom_ParticleTask;
	static const uint32_t MAIN = 0x800;
	static const uint32_t MORPH = 0xCE8;
	static const uint32_t VBUF = 0xEE0;
	static const uint32_t VBUF_BYTES = 0x1A2C - VBUF;
	static const uint32_t MORPH_BYTES = VBUF - MORPH;
	static const uint32_t ARENA_A = 0x1A2C;
	static const uint32_t ARENA_B = 0x11A2C;
	static uint8_t *MESH() { return (uint8_t *)game_addr(game::MAG_207_Maelstrom_DataCopy); }
	static const uint16_t *SQRT_TABLE() { return (const uint16_t *)game_addr(game::unk_11B3164); }
	static const int16_t *LENS_TABLE() { return (const int16_t *)game_addr(game::unk_11B3364); }
	static constexpr game::Id MODEL_Main = game::MAG_207_Maelstrom_ModelMain;
	static constexpr game::Id KINDS = game::a00900901;
	static constexpr game::Id SEQ_Particle = game::MAG_207_Maelstrom_SeqParticle;
	static const void *SOUND_Maelstrom() { return (const void *)game_addr(game::MAG_207_Maelstrom_SoundMaelstrom); }
	static const uint32_t COPY_X = 0x240, COPY_Y = 0x100;

	inline int32_t ComputeCos(int32_t a) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(a); }
	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void RotationFromAnglesB(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::MAG_063_RotationFromAnglesYXZ))(angles, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
	inline uint32_t RenderPrimSet(void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::MAG_063_RenderPrimSet))(h, ot, mode, cursor); }
	inline uint32_t RenderPrimSetCopy(void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::MAG_207_Maelstrom_RenderPrimSetCopy))(h, ot, mode, cursor); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void MatrixMultiply3(Mat4x3 *a, const Mat4x3 *b) { fn<void (__cdecl *)(Mat4x3 *, const Mat4x3 *)>(game_addr(game::MatrixMultiply3x3))(a, b); }
	inline void OrthonormalizeCamera(const void *cam, Mat4x3 *out) { fn<void (__cdecl *)(const void *, Mat4x3 *)>(game_addr(game::MAG_069_OrthonormalizeCameraMatrix))(cam, out); }
	inline void GteMVMVA_RotV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0))(); }
	inline void GteWriteData2(int reg, uint32_t value) { fn<void (__cdecl *)(int, uint32_t)>(game_addr(game::GTE_WriteDataRegAt))(reg, value); }
	inline void GteSetRGBC(const void *rgbc) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadRGBC))(rgbc); }
	inline void GteDPCS() { fn<void (__cdecl *)()>(game_addr(game::GTE_DPCS))(); }
	inline void GteStoreRGB2(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreRGB2))(dst); }
	inline void GteSetFarColor(int32_t r, int32_t g, int32_t b) { fn<void (__cdecl *)(int32_t, int32_t, int32_t)>(game_addr(game::GTE_SetFarColor))(r, g, b); }
	inline void InsertPrimAltViewport(uint32_t ot, void *packet) { fn<void (__cdecl *)(uint32_t, void *)>(game_addr(game::SSIGPU_InsertPrimAltViewport))(ot, packet); }
	inline void GetDrawEnv(void *env) { fn<void (__cdecl *)(void *)>(game_addr(game::GPU_GetDrawEnv))(env); }
	inline void ClearImage(const int16_t *rect, int32_t r, int32_t g, int32_t b) { fn<void (__cdecl *)(const int16_t *, int32_t, int32_t, int32_t)>(game_addr(game::isDravingOverlayImage))(rect, r, g, b); }
	inline void RequestDrawOffset(int32_t x, int32_t y) { fn<void (__cdecl *)(int32_t, int32_t)>(game_addr(game::Gpu_RequestNextPassDrawOffset))(x, y); }

#pragma pack(push, 1)
	struct MasterNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t pad0E;
		uint8_t started;
		uint32_t arena;
	};
	struct MainNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t count;
		uint8_t restored;
		uint8_t pad11[3];
		uint8_t *ents[5];
		int16_t avg[4];
		uint8_t layout[0x4B8];
	};
	struct ParticleNode
	{
		TaskNode hdr;
		int16_t pos[3];
		int16_t counter;
		int16_t vel[3];
		int8_t acc[3];
		uint8_t pad1D[3];
	};
	struct PrimArg
	{
		int16_t pos[4];
		uint32_t kinds;
		int16_t depth;
		int16_t scroll;
		uint32_t morph;
	};
#pragma pack(pop)
	static_assert(sizeof(MasterNode) == 0x14 && sizeof(MainNode) == 0x4E8 && sizeof(ParticleNode) == 0x20, "Maelstrom nodes");
	static_assert(sizeof(PrimArg) == 0x14, "Maelstrom prim block");

	enum DrawKind { K_PLAY = 1, K_WARP = 2, K_PARTICLE = 3 };
}
}

namespace ff8fx
{
namespace maelstrom207
{
	static void __cdecl PrimCallback(prim::Layout *l, prim::Record *r, int arg_)
	{
		const PrimArg *arg = (const PrimArg *)arg_;
		if (((int32_t)r->scale[2] | *(const int32_t *)r->scale) == 0) return;
		if (r->a >= 0x1000 && *(const uint32_t *)r->rgb == 0) return;
		uint8_t *h = (uint8_t *)FieldAlloc(0x6C);
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
		const uint32_t flags = r->flags;
		int16_t off[4] = { r->pos[0], r->pos[1], r->pos[2], 0 };
		if (flags & 0x1000)
		{
			GteSetRotMatrixCtrl(&Camera());
			GteLoadV0(arg->pos);
			GteMVMVA_RotV0();
			GteReadMAC123(m.t);
			GteSetRotMatrixCtrl(&ModuleMatrix());
			GteLoadV0(off);
			GteMVMVA_RotV0();
			int32_t v[3];
			GteReadMAC123(v);
			m.t[0] = (int32_t)((uint32_t)m.t[0] + (uint32_t)v[0]);
			m.t[1] = (int32_t)((uint32_t)m.t[1] + (uint32_t)v[1]);
			m.t[2] = (int32_t)((uint32_t)m.t[2] + (uint32_t)v[2]);
			MatrixMultiply(&ModuleMatrix(), &m);
		}
		else if (flags & 0x200)
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
		m.t[0] = (int32_t)((uint32_t)m.t[0] + (uint32_t)Camera().t[0]);
		m.t[1] = (int32_t)((uint32_t)m.t[1] + (uint32_t)Camera().t[1]);
		m.t[2] = (int32_t)((uint32_t)m.t[2] + (uint32_t)Camera().t[2]);
		if (!(*(const uint32_t *)r->scale == 0x10001000 && r->scale[2] == 0x1000))
		{
			if (flags & 0x100)
			{
				Mat4x3 d = {};
				d.m[0][0] = r->scale[0];
				d.m[1][1] = r->scale[1];
				d.m[2][2] = r->scale[2];
				MatrixMultiply3(&m, &d);
			}
			else
			{
				const int32_t v[3] = { r->scale[0], r->scale[1], r->scale[2] };
				Scale3DMatrix(&m, v);
			}
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
		*(int32_t *)(h + 0x18) = arg->depth;
		*(uint16_t *)(h + 0x22) = 0;
		*(uint16_t *)(h + 0x20) = 0;
		*(uint16_t *)(h + 0x26) = 0;
		*(uint16_t *)(h + 0x24) = 0;
		*(uint16_t *)(h + 0x2A) = 0x100;
		*(uint16_t *)(h + 0x28) = 0x100;
		const char kind = *(const char *)(arg->kinds + object);
		if (kind == '9') PacketCursor() = RenderPrimSetCopy(h, RenderOT(), 2, PacketCursor());
		else
		{
			if (kind == '1')
			{
				*(uint16_t *)(h + 0x2C) = 0;
				*(uint16_t *)(h + 0x2E) = 0;
				*(uint16_t *)(h + 0x32) = 0x80;
				*(uint16_t *)(h + 0x30) = 0x100;
				*(uint16_t *)(h + 0x22) = (uint16_t)((uint8_t)arg->scroll & 0x7F);
			}
			PacketCursor() = RenderPrimSet(h, RenderOT(), 2, PacketCursor());
		}
		FieldFree(0x6C);
	}

	static void NormaliseMesh(uint8_t *mesh)
	{
		uint32_t n = *(const uint32_t *)(mesh + 4);
		uint8_t *v = mesh + 8;
		do
		{
			const int32_t x = *(const int16_t *)v, y = *(const int16_t *)(v + 2);
			int32_t sq = (int32_t)((uint32_t)mul32(y, y) + (uint32_t)mul32(x, x));
			if (sq == 0) *(uint32_t *)(v + 4) = 0;
			else
			{
				GteSetLZCS(sq);
				int32_t lz;
				GteReadLZCR(&lz);
				const int32_t e = lz & ~1;
				const int32_t sh = (0x1F - e) >> 1;
				const int32_t c = e - 0x18;
				uint32_t idx;
				if (c > 0) idx = (uint32_t)sq << (c & 31);
				else idx = (uint32_t)(sq >> ((0x18 - e) & 31));
				*(int16_t *)(v + 6) = (int16_t)((int32_t)((uint32_t)SQRT_TABLE()[idx] << (sh & 31)) >> 17);
			}
			v += 8;
		} while (--n);
	}

	static void PrepareMesh(uint8_t *mesh)
	{
		NormaliseMesh(mesh);
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		*(uint8_t **)h = mesh;
		*(uint8_t **)(h + 4) = mesh + 8;
		*(uint8_t **)(h + 0x20) = mesh + *(const uint32_t *)mesh + 0x10;
		uint8_t *p = *(uint8_t **)(h + 0x20);
		uint32_t n = *(const uint32_t *)p;
		p += 4;
		*(uint8_t **)(h + 0x20) = p;
		if (n)
		{
			do
			{
				const uint32_t *verts = *(const uint32_t *const *)(h + 4);
				const uint16_t i0 = *(const uint16_t *)(p + 4), i1 = *(const uint16_t *)(p + 6), i2 = *(const uint16_t *)(p + 8);
				*(uint32_t *)(h + 0x34) = verts[i0];
				*(uint32_t *)(h + 0x38) = verts[i0 + 1];
				*(uint32_t *)(h + 0x3C) = verts[i1];
				*(uint32_t *)(h + 0x40) = verts[i1 + 1];
				*(uint32_t *)(h + 0x44) = verts[i2];
				*(uint32_t *)(h + 0x48) = verts[i2 + 1];
				int32_t lo = *(const int16_t *)(h + 0x34);
				bool second = false;
				if ((int32_t)*(const int16_t *)(h + 0x3C) < lo)
				{
					lo = *(const int16_t *)(h + 0x3C);
					second = true;
				}
				uint16_t *w = (uint16_t *)(p + 4);
				uint32_t *c = (uint32_t *)p;
				if ((int32_t)*(const int16_t *)(h + 0x44) < lo)
				{
					const uint16_t a = w[0], b = w[1], d = w[2];
					w[0] = d; w[1] = a; w[2] = b;
					const uint32_t c0 = c[0], c1 = c[3], c2 = c[4];
					c[0] = c2; c[4] = c1; c[3] = c0;
				}
				else if (second)
				{
					const uint16_t a = w[0], b = w[1], d = w[2];
					w[0] = b; w[2] = a; w[1] = d;
					const uint32_t c0 = c[0], c1 = c[3], c2 = c[4];
					c[0] = c1; c[3] = c2; c[4] = c0;
				}
				for (int k = 0; k < 3; k++) w[k] = (uint16_t)(w[k] + (uint16_t)(w[k] >> 1));
				p += 0x14;
			} while (--n);
			*(uint8_t **)(h + 0x20) = p;
		}
		p = *(uint8_t **)(h + 0x20);
		n = *(const uint32_t *)p;
		p += 4;
		*(uint8_t **)(h + 0x20) = p;
		if (n)
		{
			do
			{
				const uint32_t *verts = *(const uint32_t *const *)(h + 4);
				uint16_t *w = (uint16_t *)(p + 4);
				uint32_t *c = (uint32_t *)p;
				*(uint32_t *)(h + 0x34) = verts[w[0]];
				*(uint32_t *)(h + 0x38) = verts[w[0] + 1];
				*(uint32_t *)(h + 0x3C) = verts[w[1]];
				*(uint32_t *)(h + 0x40) = verts[w[1] + 1];
				*(uint32_t *)(h + 0x44) = verts[w[2]];
				*(uint32_t *)(h + 0x48) = verts[w[2] + 1];
				*(uint32_t *)(h + 0x4C) = verts[w[3]];
				*(uint32_t *)(h + 0x50) = verts[w[3] + 1];
				int32_t lo = *(const int16_t *)(h + 0x34);
				int which = 0;
				if ((int32_t)*(const int16_t *)(h + 0x3C) < lo) { lo = *(const int16_t *)(h + 0x3C); which = 1; }
				if ((int32_t)*(const int16_t *)(h + 0x44) < lo) { lo = *(const int16_t *)(h + 0x44); which = 2; }
				if ((int32_t)*(const int16_t *)(h + 0x4C) < lo)
				{
					const uint16_t a = w[0], b = w[1], d = w[2], e = w[3];
					w[0] = e; w[3] = a; w[1] = d; w[2] = b;
					const uint32_t c0 = c[0], c1 = c[3], c2 = c[4], c3 = c[5];
					c[0] = c3; c[5] = c0; c[3] = c2; c[4] = c1;
				}
				else if (which == 1)
				{
					const uint16_t a = w[0], b = w[1], d = w[2], e = w[3];
					w[0] = b; w[1] = e; w[3] = d; w[2] = a;
					const uint32_t c0 = c[0], c1 = c[3], c2 = c[4], c3 = c[5];
					c[0] = c1; c[3] = c3; c[5] = c2; c[4] = c0;
				}
				else if (which == 2)
				{
					const uint16_t a = w[0], b = w[1], d = w[2], e = w[3];
					w[0] = d; w[2] = e; w[3] = b; w[1] = a;
					const uint32_t c0 = c[0], c1 = c[3], c2 = c[4], c3 = c[5];
					c[0] = c2; c[4] = c3; c[5] = c1; c[3] = c0;
				}
				for (int k = 0; k < 4; k++) w[k] = (uint16_t)(w[k] + (uint16_t)(w[k] >> 1));
				p += 0x18;
			} while (--n);
			*(uint8_t **)(h + 0x20) = p;
		}
		FieldFree(0x58);
	}

	static int32_t ExpandBox(int16_t *box, const int16_t *p, int32_t *minz)
	{
		if (p[2] <= 0) return 0;
		if ((int32_t)p[2] < *minz) *minz = p[2];
		if (box[0] > p[0]) box[0] = p[0];
		if (box[2] < p[0]) box[2] = p[0];
		if (box[1] > p[1]) box[1] = p[1];
		if (box[3] < p[1]) box[3] = p[1];
		return 1;
	}

	static int32_t HeadBox(int16_t *box, const uint8_t *e, int32_t *minz)
	{
		const int16_t x = *(const int16_t *)(e + 0x1C), y = *(const int16_t *)(e + 0x1E), z = *(const int16_t *)(e + 0x20);
		const int16_t size = (int16_t)(*(const int16_t *)(e + 0x26) + 0x100);
		const int16_t hy = (int16_t)(*(const int16_t *)(e + 0x36) - 0x200);
		int16_t pts[2][4] = { { x, y, z, 0 }, { x, hy, z, 0 } };
		int32_t any = 0, w = size;
		for (int i = 1; i >= 0; i--)
		{
			GteLoadV0(pts[i]);
			GteRTPS();
			int16_t q[4];
			GteReadSXY2(q);
			GteReadOTZ(q + 2);
			q[0] = (int16_t)((int32_t)q[0] / 8);
			q[1] = (int16_t)((int32_t)q[1] / 8);
			if (i == 1)
			{
				const int32_t d = q[2] ? (int32_t)q[2] : 1;
				w = mul32((int32_t)size, 54) / d;
			}
			q[0] = (int16_t)(q[0] - (int16_t)w);
			any |= ExpandBox(box, q, minz);
			q[0] = (int16_t)(q[0] + (int16_t)(w + w));
			any |= ExpandBox(box, q, minz);
		}
		return any;
	}

	static void DrawTriangles(uint8_t *h)
	{
		uint8_t *t = *(uint8_t **)(h + 0x10);
		uint32_t n = *(const uint32_t *)t;
		t += 4;
		*(uint8_t **)(h + 0x10) = t;
		uint8_t *p = (uint8_t *)PacketCursor();
		if (n)
		{
			do
			{
				const uint8_t *v = *(const uint8_t **)(h + 4);
				ff8fx::copy<(0xC)>(h + 0x18, v + *(const uint16_t *)(t + 4) * 4);
				ff8fx::copy<(0xC)>(h + 0x24, v + *(const uint16_t *)(t + 6) * 4);
				ff8fx::copy<(0xC)>(h + 0x30, v + *(const uint16_t *)(t + 8) * 4);
				*(uint32_t *)(p + 8) = *(const uint32_t *)(h + 0x1C);
				*(uint32_t *)(p + 0x10) = *(const uint32_t *)(h + 0x28);
				*(uint32_t *)(p + 0x18) = *(const uint32_t *)(h + 0x34);
				GteSetRGBC(t);
				GteDPCS();
				GteStoreRGB2(p + 4);
				GteSetRGBC(t + 0xC);
				GteDPCS();
				GteStoreRGB2(p + 0xC);
				GteSetRGBC(t + 0x10);
				GteDPCS();
				GteStoreRGB2(p + 0x14);
				*(uint32_t *)p = 0x6000000;
				p[7] = 0x32;
				InsertPrimAltViewport(*(const uint32_t *)(h + 0x14), p);
				p += 0x1C;
				t += 0x14;
			} while (--n);
		}
		*(uint8_t **)(h + 0x10) = t;
		PacketCursor() = (uint32_t)p;
	}

	static void DrawQuads(uint8_t *h)
	{
		uint8_t *t = *(uint8_t **)(h + 0x10);
		uint32_t n = *(const uint32_t *)t;
		t += 4;
		*(uint8_t **)(h + 0x10) = t;
		uint8_t *p = (uint8_t *)PacketCursor();
		if (n)
		{
			do
			{
				const uint8_t *v = *(const uint8_t **)(h + 4);
				ff8fx::copy<(0xC)>(h + 0x18, v + *(const uint16_t *)(t + 4) * 4);
				ff8fx::copy<(0xC)>(h + 0x24, v + *(const uint16_t *)(t + 6) * 4);
				ff8fx::copy<(0xC)>(h + 0x30, v + *(const uint16_t *)(t + 8) * 4);
				ff8fx::copy<(0xC)>(h + 0x3C, v + *(const uint16_t *)(t + 0xA) * 4);
				*(uint32_t *)(p + 8) = *(const uint32_t *)(h + 0x1C);
				*(uint32_t *)(p + 0x10) = *(const uint32_t *)(h + 0x28);
				*(uint32_t *)(p + 0x18) = *(const uint32_t *)(h + 0x34);
				*(uint32_t *)(p + 0x20) = *(const uint32_t *)(h + 0x40);
				GteSetRGBC(t);
				GteDPCS();
				GteStoreRGB2(p + 4);
				GteSetRGBC(t + 0xC);
				GteDPCS();
				GteStoreRGB2(p + 0xC);
				GteSetRGBC(t + 0x10);
				GteDPCS();
				GteStoreRGB2(p + 0x14);
				GteSetRGBC(t + 0x14);
				GteDPCS();
				GteStoreRGB2(p + 0x1C);
				*(uint32_t *)p = 0x8000000;
				p[7] = 0x3A;
				InsertPrimAltViewport(*(const uint32_t *)(h + 0x14), p);
				p += 0x24;
				t += 0x18;
			} while (--n);
		}
		*(uint8_t **)(h + 0x10) = t;
		PacketCursor() = (uint32_t)p;
	}

	static uint16_t CopyTPage(const uint8_t *h, int16_t x)
	{
		const uint32_t ox = *(const uint32_t *)(h + 8);
		const int32_t oy = *(const int32_t *)(h + 0xC);
		return (uint16_t)((((ox + (uint32_t)(int32_t)x) >> 6) & 0xF) | ((((uint32_t)oy & 0x200) | 0x40) << 2) | ((uint32_t)(oy >> 4) & 0x10));
	}

	static void DrawCopyTriangles(uint8_t *h)
	{
		uint8_t *t = *(uint8_t **)(h + 0x10);
		uint32_t n = *(const uint32_t *)t;
		t += 4;
		*(uint8_t **)(h + 0x10) = t;
		uint8_t *p = (uint8_t *)PacketCursor();
		if (n)
		{
			do
			{
				const uint8_t *v = *(const uint8_t **)(h + 4);
				ff8fx::copy<(0xC)>(h + 0x18, v + *(const uint16_t *)(t + 4) * 4);
				ff8fx::copy<(0xC)>(h + 0x24, v + *(const uint16_t *)(t + 6) * 4);
				ff8fx::copy<(0xC)>(h + 0x30, v + *(const uint16_t *)(t + 8) * 4);
				if ((h[0x20] & h[0x2C] & h[0x38]) == 0)
				{
					const int16_t x0 = *(const int16_t *)(h + 0x1C);
					*(uint16_t *)(p + 0x16) = CopyTPage(h, x0);
					const uint8_t a = (uint8_t)(x0 & 0xC0);
					p[0xC] = (uint8_t)(h[0x1C] - a);
					p[0xD] = h[0x1E];
					p[0x14] = (uint8_t)(h[0x28] - a);
					p[0x15] = h[0x2A];
					p[0x1C] = (uint8_t)(h[0x34] - a);
					p[0x1D] = h[0x36];
					*(uint32_t *)(p + 8) = *(const uint32_t *)(h + 0x18);
					*(uint32_t *)(p + 0x10) = *(const uint32_t *)(h + 0x24);
					*(uint32_t *)(p + 4) = 0x808080;
					*(uint32_t *)(p + 0x18) = *(const uint32_t *)(h + 0x30);
					*(uint32_t *)p = 0x7000000;
					p[7] = 0x26;
					InsertPrimAltViewport(*(const uint32_t *)(h + 0x14), p);
					p += 0x20;
				}
				t += 0x14;
			} while (--n);
		}
		*(uint8_t **)(h + 0x10) = t;
		PacketCursor() = (uint32_t)p;
	}

	static void DrawCopyQuads(uint8_t *h)
	{
		uint8_t *t = *(uint8_t **)(h + 0x10);
		uint32_t n = *(const uint32_t *)t;
		t += 4;
		*(uint8_t **)(h + 0x10) = t;
		uint8_t *p = (uint8_t *)PacketCursor();
		if (n)
		{
			do
			{
				const uint8_t *v = *(const uint8_t **)(h + 4);
				ff8fx::copy<(0xC)>(h + 0x18, v + *(const uint16_t *)(t + 4) * 4);
				ff8fx::copy<(0xC)>(h + 0x24, v + *(const uint16_t *)(t + 6) * 4);
				ff8fx::copy<(0xC)>(h + 0x30, v + *(const uint16_t *)(t + 8) * 4);
				ff8fx::copy<(0xC)>(h + 0x3C, v + *(const uint16_t *)(t + 0xA) * 4);
				if ((h[0x44] & h[0x2C] & h[0x20] & h[0x38]) == 0)
				{
					const int16_t x0 = *(const int16_t *)(h + 0x1C);
					*(uint16_t *)(p + 0x16) = CopyTPage(h, x0);
					const uint8_t a = (uint8_t)(x0 & 0xC0);
					p[0xC] = (uint8_t)(h[0x1C] - a);
					p[0xD] = h[0x1E];
					p[0x14] = (uint8_t)(h[0x28] - a);
					p[0x15] = h[0x2A];
					p[0x1C] = (uint8_t)(h[0x34] - a);
					p[0x1D] = h[0x36];
					p[0x24] = (uint8_t)(h[0x40] - a);
					p[0x25] = h[0x42];
					*(uint32_t *)(p + 8) = *(const uint32_t *)(h + 0x18);
					*(uint32_t *)(p + 0x10) = *(const uint32_t *)(h + 0x24);
					*(uint32_t *)(p + 0x18) = *(const uint32_t *)(h + 0x30);
					*(uint32_t *)(p + 4) = 0x808080;
					*(uint32_t *)(p + 0x20) = *(const uint32_t *)(h + 0x3C);
					*(uint32_t *)p = 0x9000000;
					p[7] = 0x2E;
					InsertPrimAltViewport(*(const uint32_t *)(h + 0x14), p);
					p += 0x28;
				}
				t += 0x18;
			} while (--n);
		}
		*(uint8_t **)(h + 0x10) = t;
		PacketCursor() = (uint32_t)p;
	}

	static void Warp(uint8_t *mesh, const uint8_t *verts, int32_t ot, int32_t fade, const int16_t *row, uint8_t *vbuf)
	{
		uint8_t env[0x5C];
		GetDrawEnv(env);
		uint32_t n = *(const uint32_t *)(mesh + 4);
		uint8_t *o = vbuf;
		const uint8_t *v = verts;
		do
		{
			const int32_t k = row[*(const int16_t *)(v + 6)];
			int16_t moved[2];
			moved[0] = (int16_t)(mul32(*(const int16_t *)v, k) >> 12);
			moved[1] = (int16_t)(mul32(*(const int16_t *)(v + 2), k) >> 12);
			GteWriteData2(0, *(const uint32_t *)moved);
			GteMVMVA_RotV0Tr();
			int32_t ir1, ir2;
			GteReadDataRegTo(&ir1, 9);
			GteReadDataRegTo(&ir2, 10);
			*(int16_t *)o = (int16_t)ir1;
			*(int16_t *)(o + 2) = (int16_t)ir2;
			GteWriteData2(0, *(const uint32_t *)v);
			GteMVMVA_RotV0Tr();
			if (*(const int16_t *)o < 0) o[8] |= 1;
			else if (*(const int16_t *)o >= 0x140) o[8] |= 2;
			else *(uint16_t *)(o + 8) = 0;
			const int16_t sy = *(const int16_t *)(o + 2);
			if (sy < 0) o[8] |= 4;
			else if (sy >= 0xD8) o[8] |= 8;
			GteReadDataRegTo(&ir1, 9);
			GteReadDataRegTo(&ir2, 10);
			const int16_t u = (int16_t)ir1;
			*(int16_t *)(o + 4) = u;
			*(int16_t *)(o + 6) = (int16_t)ir2;
			if (u < 0) *(int16_t *)(o + 4) = 0;
			else if (u >= 0x140) *(int16_t *)(o + 4) = 0x13F;
			else if (*(const int16_t *)(o + 6) < 0) *(int16_t *)(o + 6) = 0;
			else if (*(const int16_t *)(o + 6) >= 0xD8) *(int16_t *)(o + 6) = 0xD7;
			o += 0xC;
			v += 8;
		} while (--n);
		uint8_t *h = (uint8_t *)FieldAlloc(0x48);
		*(uint8_t **)h = mesh;
		*(uint32_t *)(h + 8) = COPY_X;
		*(uint8_t **)(h + 4) = vbuf;
		*(uint32_t *)(h + 0xC) = COPY_Y;
		*(uint32_t *)(h + 0x14) = OTBase() + (uint32_t)ot * 4;
		uint32_t *p = (uint32_t *)PacketCursor();
		PacketCursor() += 8;
		p[0] = 0x1000000;
		p[1] = 0xE1000220;
		InsertPrimAutoDepth(*(const uint32_t *)(h + 0x14), p);
		GteSetIR0(fade);
		GteSetFarColor(0, 0, 0);
		*(uint8_t **)(h + 0x10) = mesh + *(const uint32_t *)mesh + 0x10;
		DrawTriangles(h);
		DrawQuads(h);
		p = (uint32_t *)PacketCursor();
		PacketCursor() += 8;
		p[0] = 0x1000000;
		p[1] = 0xE1000240;
		InsertPrimAutoDepth(*(const uint32_t *)(h + 0x14), p);
		*(uint8_t **)(h + 0x10) = mesh + *(const uint32_t *)mesh + 0x10;
		DrawCopyTriangles(h);
		DrawCopyQuads(h);
		FieldFree(0x48);
	}

	static bool WarpBox(const MainNode *w, Mat4x3 &frame, int32_t *z_out)
	{
		int16_t box[4] = { 0x7FFF, 0x7FFF, (int16_t)0x8001, (int16_t)0x8001 };
		int32_t any = 0, minz = 0x10000;
		GteSetRotMatrix(&Camera());
		GteSetTransVector(&Camera());
		for (int32_t i = 0; i < w->count; i++) any |= HeadBox(box, w->ents[i], &minz);
		const int32_t z = minz - 0x80;
		if (!any || z <= 0) return false;
		int16_t bw = (int16_t)(box[2] - box[0]);
		const int16_t cx = (int16_t)(box[0] + (int16_t)((int32_t)bw / 2));
		int16_t bh = (int16_t)(box[3] - box[1]);
		const int16_t cy = (int16_t)(box[1] + (int16_t)((int32_t)bh / 2));
		GteLoadV0(w->avg);
		GteRTPS();
		int16_t sxy[2];
		GteReadSXY2(sxy);
		int32_t otz;
		GteReadOTZ(&otz);
		const int16_t s = (int16_t)(0x18000 / (int32_t)(int16_t)otz);
		if (bw < s) bw = s;
		if (bh < s) bh = s;
		frame.m[1][1] = bh;
		frame.m[0][0] = bw;
		frame.t[1] = cy;
		frame.t[0] = cx;
		*z_out = z;
		return true;
	}

	static int32_t WarpFade(int32_t b) { return 0x1000 - ComputeSin((int32_t)(((uint32_t)b << 11) / 48)); }

	static int32_t WarpRow(int32_t b)
	{
		int32_t r;
		if ((uint32_t)b < 0xC) r = 0x10 - (int32_t)(((uint32_t)b << 4) / 12);
		else if ((uint32_t)b < 0x18) r = (int32_t)((((uint32_t)b - 0xC) << 4) / 12);
		else
		{
			r = (int32_t)((((uint32_t)b - 0x18) << 4) / 12) + 0x10;
			if (r <= 0x1F) return r;
			r = 0x40 - r;
		}
		return r > 0x1F ? 0x1F : r;
	}

	static void ParticleDraw(int16_t *pos, int16_t frame)
	{
		TransformCameraByShadowRotation(pos, 0x1000, (int32_t)0xFFFFFC00);
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		*(uint32_t *)h = game_addr(SEQ_Particle);
		*(int16_t *)(h + 4) = frame;
		*(uint16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static void DirectionFromPitchYaw(int32_t a, int32_t b, int16_t *out)
	{
		const int32_t c = ComputeCos(a);
		out[1] = (int16_t)-ComputeSin(a);
		out[0] = (int16_t)(mul32(ComputeSin(b), c) >> 12);
		out[2] = (int16_t)((int32_t)(0u - (uint32_t)mul32(ComputeCos(b), c)) >> 12);
	}

	static uint32_t __cdecl ParticleTask(TaskNode *n)
	{
		ParticleNode *p = (ParticleNode *)n;
		ParticleDraw(p->pos, p->counter);
		p->vel[0] = (int16_t)(p->vel[0] + (int16_t)p->acc[0]);
		p->vel[1] = (int16_t)(p->vel[1] + (int16_t)p->acc[1]);
		p->vel[2] = (int16_t)(p->vel[2] + (int16_t)p->acc[2]);
		p->pos[0] = (int16_t)(p->pos[0] + p->vel[0]);
		p->pos[1] = (int16_t)(p->pos[1] + p->vel[1]);
		p->pos[2] = (int16_t)(p->pos[2] + p->vel[2]);
		p->counter++;
		return p->counter < 0x10 ? 0 : TASK_END;
	}

	static uint32_t __cdecl MainTask(TaskNode *n)
	{
		MainNode *w = (MainNode *)n;
		if ((uint32_t)(int32_t)w->counter < 0x50)
		{
			PrimArg arg;
			ff8fx::copy<(8)>(arg.pos, w->avg);
			arg.kinds = game_addr(KINDS);
			arg.depth = -0x80;
			arg.scroll = (int16_t)(w->counter << 5);
			arg.morph = TexBase() + MORPH;
			prim::play((prim::Layout *)w->layout, PrimCallback, (int)&arg, 0);
		}
		const int32_t b = (int32_t)w->counter - 0x20;
		if ((uint32_t)b < 0x30)
		{
			int32_t z;
			if (WarpBox(w, WarpFrame(), &z))
			{
				const int32_t fade = WarpFade(b);
				if (b < 8)
				{
					WarpFrame().m[0][0] = (int16_t)((uint32_t)mul32(WarpFrame().m[0][0], b) >> 3);
					WarpFrame().m[1][1] = (int16_t)((uint32_t)mul32(WarpFrame().m[1][1], b) >> 3);
				}
				GteSetRotMatrix(&WarpFrame());
				GteSetTransVector(&WarpFrame());
				Warp(MESH(), MESH() + 8, (z >> 2) + 0x11, fade, LENS_TABLE() + WarpRow(b) * 0x80, (uint8_t *)(TexBase() + VBUF));
			}
		}
		if (w->counter == 0) BdPlaySE(SOUND_Maelstrom(), 0, 0x80);
		if ((uint32_t)((int32_t)w->counter - 0x30) < 0x10)
		{
			for (int i = 0; i < 4; i++)
			{
				ParticleNode *p = (ParticleNode *)AddTaskToQueue(QParticles(), game_addr(ORIG_ParticleTask));
				if (!p) break;
				p->counter = 0;
				const int32_t r1 = CrtRand();
				const int32_t r2 = CrtRand();
				DirectionFromPitchYaw(r2, r1, p->pos);
				const int32_t dist = (CrtRand() & 0x7FF) + 0x800;
				const int32_t dx = mul32(p->pos[0], dist) >> 12;
				const int32_t dy = mul32(p->pos[1], dist) >> 12;
				const int32_t dz = mul32(p->pos[2], dist) >> 12;
				p->vel[2] = 0;
				p->vel[1] = 0;
				p->vel[0] = 0;
				p->acc[0] = (int8_t)((int32_t)(0u - (uint32_t)dx) >> 7);
				p->acc[1] = (int8_t)((int32_t)(0u - (uint32_t)dy) >> 7);
				p->acc[2] = (int8_t)((int32_t)(0u - (uint32_t)dz) >> 7);
				p->pos[0] = (int16_t)(w->avg[0] + (int16_t)dx);
				p->pos[1] = (int16_t)((int16_t)dy + w->avg[1]);
				p->pos[2] = (int16_t)((int16_t)dz + w->avg[2]);
			}
		}
		if (w->counter < 0x50)
		{
			w->counter++;
			return 0;
		}
		if (w->counter == 0x50)
		{
			ApplyActionResultToTargets(Ctx()->actions[0].targets, Ctx()->actions[0].target_count);
			TextureRestoreTask((void *)(TexBase() + ARENA_A), &w->restored);
			w->counter++;
		}
		return w->restored ? TASK_END : 0;
	}

	static uint32_t __cdecl MasterTask(TaskNode *n)
	{
		MasterNode *m = (MasterNode *)n;
		if (m->arena)
		{
			PacketCursor() = TexBase() + ARENA_A;
			m->arena = 0;
		}
		else
		{
			PacketCursor() = TexBase() + ARENA_B;
			m->arena = 1;
		}
		if (m->counter == 1)
		{
			const int16_t rect[4] = { (int16_t)COPY_X, (int16_t)COPY_Y, 0x140, 0x100 };
			ClearImage(rect, 0, 0, 0);
			RequestDrawOffset(COPY_X, COPY_Y);
			if (!m->started)
			{
				PrepareMesh(MESH());
				m->started = 1;
				InitTaskQueuePool(QMain(), (void *)(TexBase() + MAIN), 0x4E8, 1);
				InitTaskQueuePool(QParticles(), (void *)TexBase(), 0x20, 0x40);
				MainNode *w = (MainNode *)AddTaskToQueue(QMain(), game_addr(ORIG_MainTask));
				Memset32(&w->counter, 0, 0x137);
				DecodeModelPrimLayout(game_addr(MODEL_Main), w->layout, 0x4B8);
				w->count = (int16_t)Ctx()->actions[0].target_count;
				w->avg[2] = 0;
				w->avg[1] = 0;
				w->avg[0] = 0;
				for (int32_t i = 0; i < w->count; i++)
				{
					w->ents[i] = Entity(Ctx()->actions[0].targets[i * TARGET_STRIDE]);
					int16_t pos[4];
					GetDefaultEffectPosition(w->ents[i], pos);
					const int16_t y = (int16_t)(*(const int16_t *)(w->ents[i] + 0x36) - 0x200);
					w->avg[0] = (int16_t)(w->avg[0] + pos[0]);
					w->avg[2] = (int16_t)(w->avg[2] + pos[2]);
					w->avg[1] = (int16_t)(w->avg[1] + y);
					pos[1] = y;
				}
				w->avg[0] = (int16_t)((int32_t)w->avg[0] / (int32_t)w->count);
				w->avg[1] = (int16_t)((int32_t)w->avg[1] / (int32_t)w->count);
				w->avg[2] = (int16_t)((int32_t)w->avg[2] / (int32_t)w->count);
			}
		}
		int a0;
		if (m->started)
		{
			OrthonormalizeCamera((const void *)game_addr(game::unk_1D97778), &ModuleMatrix());
			a0 = ExecuteTaskQueue(QMain());
			ExecuteTaskQueue(QParticles());
		}
		else a0 = (int)n;
		if (m->started && a0 == 0) return TASK_END;
		m->counter++;
		return 0;
	}
}

	void register_mag207_maelstrom()
	{
		register_port(maelstrom207::ORIG_MasterTask, (void *)maelstrom207::MasterTask, "207 MasterTask", 207);
		register_port(maelstrom207::ORIG_MainTask, (void *)maelstrom207::MainTask, "207 MainTask", 207);
		register_port(maelstrom207::ORIG_ParticleTask, (void *)maelstrom207::ParticleTask, "207 ParticleTask", 207);
	}
}

