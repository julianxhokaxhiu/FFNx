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

// Effect 139: Sonic Wave

#include "mag_common.h"

namespace ff8fx
{
namespace sonic139
{
	using namespace eng;
	using namespace magc;

	inline uint8_t *&Caster() { return var<uint8_t *>(game_addr(game::MAG_139_CasterEntity)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::MAG_139_MagicBufferBase)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::MAG_139_Context)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_139_PacketCursor)); }
	inline TaskQueue *QTargets() { return (TaskQueue *)game_addr(game::MAG_139_TargetQueue); }
	inline TaskQueue *QMain() { return (TaskQueue *)game_addr(game::MAG_139_MainQueue); }
	inline Mat4x3 &WarpFrame() { return var<Mat4x3>(game_addr(game::MAG_139_WarpFrame)); }
	inline uint32_t OTBase() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)); }

	static constexpr game::Id ORIG_MasterTask = game::MAG_139_MasterTask;
	static constexpr game::Id ORIG_MainTask = game::MAG_139_MainTask;
	static constexpr game::Id ORIG_TargetTask = game::MAG_139_TargetTask;
	static const uint32_t VERTS = 0xF3C;
	static const uint32_t VBUF = 0x1584;
	static const uint32_t VBUF_BYTES = 0x1EF0 - VBUF;
	static const uint32_t VERTS_BYTES = VBUF - VERTS;
	static const uint32_t ARENA_A = 0x1EF0;
	static const uint32_t ARENA_B = 0x11EF0;
	static uint8_t *MESH() { return (uint8_t *)game_addr(game::MAG_139_DataCopy); }
	static constexpr game::Id FRAMES = game::MAG_139_WaveFrames;
	static const uint32_t FRAME_BYTES = 0x650;
	static const uint16_t *SQRT_TABLE() { return (const uint16_t *)game_addr(game::MAG_139_SqrtTable); }
	static const int16_t *WAVE_TABLE() { return (const int16_t *)game_addr(game::MAG_139_WaveTable); }
	static const int16_t *BONE_ENTRY() { return (const int16_t *)game_addr(game::MAG_139_BonePointEntry); }
	static constexpr game::Id MODEL_Main = game::MAG_139_LayoutMain;
	static constexpr game::Id MODEL_Target = game::MAG_139_LayoutTarget;
	static const uint8_t *COLOURS() { return (const uint8_t *)game_addr(game::MAG_139_TintColours); }
	static const void *SOUND_Wave() { return (const void *)game_addr(game::MAG_139_SoundWave); }
	static const uint32_t OT_CAPTURE = 0x4484;

	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void RotationFromAngles2(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::MAG_063_RotationFromAnglesYXZ))(angles, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void GteMVMVA_RotV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0))(); }
	inline uint32_t RenderPrim2(void *h, uint32_t ot, int mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int, uint32_t)>(game_addr(game::MAG_070_RenderPrimModel))(h, ot, mode, cursor); }
	inline void RotY(int32_t angle, Mat4x3 *out) { fn<void (__cdecl *)(int32_t, Mat4x3 *)>(game_addr(game::Effect_RotationYMatrix))(angle, out); }
	inline void BlendVectors(const int16_t *a, const int16_t *b, int32_t wa, int32_t wb, int16_t *out) { fn<void (__cdecl *)(const int16_t *, const int16_t *, int32_t, int32_t, int16_t *)>(game_addr(game::VecLerp))(a, b, wa, wb, out); }
	inline void BlendColour(const void *a, const void *b, int32_t wa, int32_t wb, void *out) { fn<void (__cdecl *)(const void *, const void *, int32_t, int32_t, void *)>(game_addr(game::ColourLerp))(a, b, wa, wb, out); }
	inline void GteWriteData(int32_t value, int reg) { fn<void (__cdecl *)(int32_t, int)>(game_addr(game::GTE_WriteDataReg))(value, reg); }
	inline void GteWriteData2(int reg, uint32_t value) { fn<void (__cdecl *)(int, uint32_t)>(game_addr(game::GTE_WriteDataRegAt))(reg, value); }
	inline void GteSetRGBC(const void *rgbc) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadRGBC))(rgbc); }
	inline void GteDPCS() { fn<void (__cdecl *)()>(game_addr(game::GTE_DPCS))(); }
	inline void GteStoreRGB2(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreRGB2))(dst); }
	inline void GteSetFarColor(int32_t r, int32_t g, int32_t b) { fn<void (__cdecl *)(int32_t, int32_t, int32_t)>(game_addr(game::GTE_SetFarColor))(r, g, b); }
	inline void InsertPrimAltViewport(uint32_t ot, void *packet) { fn<void (__cdecl *)(uint32_t, void *)>(game_addr(game::SSIGPU_InsertPrimAltViewport))(ot, packet); }

#pragma pack(push, 1)
	struct DrawEnv
	{
		int16_t clip[4];
		int16_t ofs[2];
		int16_t tw[4];
		uint16_t tpage;
		uint8_t dtd, dfe;
		uint8_t isbg, r0, g0, b0;
		uint8_t dr_env[0x40];
	};
#pragma pack(pop)
	static_assert(sizeof(DrawEnv) == 0x5C, "DRAWENV");
	inline void GetDrawEnv(DrawEnv *env) { fn<void (__cdecl *)(DrawEnv *)>(game_addr(game::GPU_GetDrawEnv))(env); }
	inline void SetDefDrawEnv(DrawEnv *env, int32_t x, int32_t y, int32_t w, int32_t h) { fn<void (__cdecl *)(DrawEnv *, int32_t, int32_t, int32_t, int32_t)>(game_addr(game::GPU_SetDefDrawEnv))(env, x, y, w, h); }
	inline void SetDrawEnv(void *packet, const DrawEnv *env) { fn<void (__cdecl *)(void *, const DrawEnv *)>(game_addr(game::SSIGPU_SetDrawEnv))(packet, env); }
	inline void SetDrawMode(void *packet, int32_t dfe, int32_t dtd, int32_t tpage, const void *rect) { fn<void (__cdecl *)(void *, int32_t, int32_t, int32_t, const void *)>(game_addr(game::GPU_SetDrawMode))(packet, dfe, dtd, tpage, rect); }

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
		uint8_t index[3];
		uint8_t restored;
		uint8_t *ents[3];
		int16_t start[4];
		int16_t avg[4];
		uint8_t layout[0x4AC];
	};
	struct TargetNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t index;
		uint8_t *entity;
		uint32_t colour;
		int16_t pos[4];
		uint8_t layout[0x278];
	};
	struct PrimArg
	{
		Mat4x3 m;
		int32_t depth;
		uint32_t morph;
	};
#pragma pack(pop)
	static_assert(sizeof(MasterNode) == 0x14 && sizeof(MainNode) == 0x4DC && sizeof(TargetNode) == 0x298, "Sonic Wave nodes");
	static_assert(sizeof(PrimArg) == 0x28, "Sonic Wave prim block");

	enum PlayKind { PLAY_MAIN = 1, PLAY_TARGET = 2, WARP_A = 3, WARP_B = 4 };
}
}

namespace ff8fx
{
namespace sonic139
{
	static void __cdecl PrimCallback(prim::Layout *l, prim::Record *r, int arg_)
	{
		const PrimArg *arg = (const PrimArg *)arg_;
		if (((int32_t)r->scale[2] | *(const int32_t *)r->scale) == 0) return;
		if (r->a >= 0x1000 && *(const uint32_t *)r->rgb == 0) return;
		uint8_t *h = AllocHeader(0x58);
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
		if (r->flags & 0x40000) RotationFromAngles2(r->rot, &m);
		else RotationFromAngles(r->rot, &m);
		int16_t off[4] = { r->pos[0], r->pos[1], r->pos[2], 0 };
		int32_t t[3];
		if (r->flags & 0x200)
		{
			t[0] = off[0];
			t[1] = off[1];
			t[2] = off[2];
		}
		else
		{
			GteSetRotMatrixCtrl(&arg->m);
			GteLoadV0(off);
			GteMVMVA_RotV0();
			GteReadMAC123(m.t);
			MatrixMultiply(&arg->m, &m);
			t[0] = m.t[0];
			t[1] = m.t[1];
			t[2] = m.t[2];
		}
		m.t[0] = (int32_t)((uint32_t)t[0] + (uint32_t)arg->m.t[0]);
		m.t[1] = (int32_t)((uint32_t)t[1] + (uint32_t)arg->m.t[1]);
		m.t[2] = (int32_t)((uint32_t)t[2] + (uint32_t)arg->m.t[2]);
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
		*(int32_t *)(h + 0x18) = arg->depth;
		PacketCursor() = RenderPrim2(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
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

	static void DamageEntity(uint8_t *entity)
	{
		const int32_t slot = (int32_t)((uint32_t)entity - game_addr(game::BattleEntitySlotData)) / 0x9C;
		for (uint32_t i = 0; i < Ctx()->actions[0].target_count; i++)
		{
			uint8_t *t = Ctx()->actions[0].targets + i * TARGET_STRIDE;
			if ((int32_t)t[0] == slot) ApplyActionResultToTarget(t);
		}
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
		int16_t pts[2][4] = { { x, y, z, 0 }, { x, (int16_t)(y - size), z, 0 } };
		int32_t any = 0, w = size;
		for (int i = 1; i >= 0; i--)
		{
			GteLoadV0(pts[i]);
			GteRTPS();
			int16_t q[4];
			GteReadSXY2(q);
			int32_t otz;
			GteReadOTZ(&otz);
			q[2] = (int16_t)otz;
			q[0] = (int16_t)(q[0] >> 3);
			q[1] = (int16_t)(q[1] >> 3);
			if (i == 1) w = mul32((int32_t)size, 54) / (int32_t)q[2];
			q[0] = (int16_t)(q[0] - (int16_t)w);
			any |= ExpandBox(box, q, minz);
			q[0] = (int16_t)(q[0] + (int16_t)(w + w));
			any |= ExpandBox(box, q, minz);
		}
		return any;
	}

	static void BlendFrames(const uint8_t *a, const uint8_t *b, uint8_t *out, int32_t t)
	{
		uint32_t n = *(const uint32_t *)(a + 4);
		GteSetIR0(t);
		const uint8_t *pa = a + 8, *pb = b + 8;
		uint8_t *o = out;
		do
		{
			const uint32_t ax = *(const uint16_t *)pa, bx = *(const uint16_t *)pb;
			GteWriteData((int32_t)(bx - ax), 9);
			const uint32_t ay = *(const uint16_t *)(pa + 2), by = *(const uint16_t *)(pb + 2);
			GteWriteData((int32_t)(by - ay), 10);
			GteGPF();
			int32_t ir;
			GteReadDataRegTo(&ir, 9);
			*(uint16_t *)o = (uint16_t)((uint32_t)ir + ax);
			GteReadDataRegTo(&ir, 10);
			*(uint16_t *)(o + 2) = (uint16_t)((uint32_t)ir + ay);
			pa += 8;
			*(uint32_t *)(o + 4) = *(const uint32_t *)(pa + 4);
			pb += 8;
			o += 8;
		} while (--n);
	}

	static void NormaliseMesh(uint8_t *mesh)
	{
		uint32_t n = *(const uint32_t *)(mesh + 4);
		uint8_t *v = mesh + 8;
		do
		{
			const int32_t x = *(const int16_t *)v, y = *(const int16_t *)(v + 2);
			int32_t sq = (int32_t)((uint32_t)mul32(x, x) + (uint32_t)mul32(y, y));
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
				int32_t len = (int32_t)((uint32_t)SQRT_TABLE()[idx] << (sh & 31)) >> 12;
				*(uint32_t *)(v + 4) = ((uint32_t)(len & ~1) << 19) | (*(const uint32_t *)(v + 4) & 0xFFFFF);
				int32_t a = 0x400 - (len >> 2);
				if (a < 0) a = 0;
				else if (a > 0x3FF) a = 0x3FF;
				const int32_t s = WAVE_TABLE()[a] >> 2;
				if (len == 0) len = 1;
				const int32_t dx = mul32(x, s) / len;
				const int32_t dy = mul32(y, s) / len;
				const uint32_t p = *(const uint32_t *)(v + 4);
				*(uint32_t *)(v + 4) = (((p & ~0x3FFu) | ((uint32_t)dx & 0x3FF)) & 0xFFF003FFu) | (((uint32_t)dy & 0x3FF) << 10);
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
		uint8_t *p = mesh + *(const uint32_t *)mesh + 0x10;
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
		FieldFree(0x58);
	}

	static void DrawRestTriangles(uint8_t *h)
	{
		uint8_t *t = *(uint8_t **)(h + 0x14);
		uint32_t n = *(const uint32_t *)t;
		t += 4;
		*(uint8_t **)(h + 0x14) = t;
		uint8_t *p = (uint8_t *)PacketCursor();
		if (n)
		{
			do
			{
				const uint8_t *v = *(const uint8_t **)(h + 4);
				ff8fx::copy<(0xC)>(h + 0x1C, v + *(const uint16_t *)(t + 4) * 4);
				ff8fx::copy<(0xC)>(h + 0x28, v + *(const uint16_t *)(t + 6) * 4);
				ff8fx::copy<(0xC)>(h + 0x34, v + *(const uint16_t *)(t + 8) * 4);
				*(uint32_t *)(p + 8) = *(const uint32_t *)(h + 0x20);
				*(uint32_t *)(p + 0x10) = *(const uint32_t *)(h + 0x2C);
				*(uint32_t *)(p + 0x18) = *(const uint32_t *)(h + 0x38);
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
				InsertPrimAltViewport(*(const uint32_t *)(h + 0x18), p);
				p += 0x1C;
				t += 0x14;
			} while (--n);
			*(uint8_t **)(h + 0x14) = t;
		}
		PacketCursor() = (uint32_t)p;
	}

	static void DrawWarpTriangles(uint8_t *h)
	{
		uint8_t *t = *(uint8_t **)(h + 0x14);
		uint32_t n = *(const uint32_t *)t;
		t += 4;
		*(uint8_t **)(h + 0x14) = t;
		uint8_t *p = (uint8_t *)PacketCursor();
		if (n)
		{
			do
			{
				const uint8_t *v = *(const uint8_t **)(h + 4);
				ff8fx::copy<(0xC)>(h + 0x1C, v + *(const uint16_t *)(t + 4) * 4);
				ff8fx::copy<(0xC)>(h + 0x28, v + *(const uint16_t *)(t + 6) * 4);
				ff8fx::copy<(0xC)>(h + 0x34, v + *(const uint16_t *)(t + 8) * 4);
				if (!(h[0x24] & h[0x30] & h[0x3C]))
				{
					const uint16_t u0 = *(const uint16_t *)(h + 0x20);
					const uint32_t ty = *(const uint32_t *)(h + 0x10);
					const uint32_t tp = (((*(const uint32_t *)(h + 0xC) + u0) >> 6) & 0xF) | (((ty & 0x200) | 0x40) << 2) | (((int32_t)ty >> 4) & 0x10);
					*(uint16_t *)(p + 0x16) = (uint16_t)tp;
					const uint8_t base = (uint8_t)(u0 & 0xC0);
					p[0xC] = (uint8_t)(h[0x20] - base);
					p[0xD] = h[0x22];
					p[0x14] = (uint8_t)(h[0x2C] - base);
					p[0x15] = h[0x2E];
					p[0x1C] = (uint8_t)(h[0x38] - base);
					p[0x1D] = h[0x3A];
					*(uint32_t *)(p + 8) = *(const uint32_t *)(h + 0x1C);
					*(uint32_t *)(p + 0x10) = *(const uint32_t *)(h + 0x28);
					*(uint32_t *)(p + 4) = 0x808080;
					*(uint32_t *)(p + 0x18) = *(const uint32_t *)(h + 0x34);
					*(uint32_t *)p = 0x7000000;
					p[7] = 0x26;
					InsertPrimAltViewport(*(const uint32_t *)(h + 0x18), p);
					p += 0x20;
				}
				t += 0x14;
			} while (--n);
			*(uint8_t **)(h + 0x14) = t;
		}
		PacketCursor() = (uint32_t)p;
	}

	static void ScreenWarp(uint8_t *mesh, const uint8_t *verts, int32_t ot_index, int32_t fade, int32_t amp, int32_t phase, uint8_t *vbuf)
	{
		const uint32_t ot = OTBase() + (uint32_t)ot_index * 4;
		DrawEnv env;
		GetDrawEnv(&env);
		env.ofs[0] = env.ofs[0] != 0 ? 0 : 0x140;
		const int16_t rect[4] = { 0, 0, 0x100, 0x100 };
		uint8_t *p = (uint8_t *)PacketCursor();
		PacketCursor() += 0xC;
		SetDrawMode(p, env.dfe, env.dtd, env.tpage, rect);
		InsertPrimAutoDepth(ot, p);
		uint32_t n = *(const uint32_t *)(mesh + 4);
		uint8_t *o = vbuf;
		const uint8_t *v = verts;
		do
		{
			const uint32_t pk = *(const uint32_t *)(v + 4);
			const int32_t idx = ((((int32_t)pk >> 18) & ~3) + phase) & 0xFFF;
			const int32_t a = mul32(WAVE_TABLE()[idx], amp) >> 12;
			const int32_t dx = (int32_t)(pk << 22) >> 22, dy = (int32_t)(pk << 12) >> 22;
			const uint16_t vx = (uint16_t)((mul32(dx, a) >> 12) + *(const int16_t *)v);
			const uint16_t vy = (uint16_t)((mul32(dy, a) >> 12) + *(const int16_t *)(v + 2));
			GteWriteData2(0, (uint32_t)vx | ((uint32_t)vy << 16));
			GteMVMVA_RotV0Tr();
			int32_t ir1, ir2;
			GteReadDataRegTo(&ir1, 9);
			GteReadDataRegTo(&ir2, 10);
			*(int16_t *)o = (int16_t)ir1;
			*(int16_t *)(o + 2) = (int16_t)ir2;
			GteWriteData2(0, *(const uint32_t *)v);
			GteMVMVA_RotV0Tr();
			const int16_t sx = *(const int16_t *)o, sy = *(const int16_t *)(o + 2);
			if (sx < 0) o[8] |= 1;
			else if (sx >= 0x140) o[8] |= 2;
			else *(uint16_t *)(o + 8) = 0;
			if (sy < 0) o[8] |= 4;
			else if (sy >= 0xD8) o[8] |= 8;
			GteReadDataRegTo(&ir1, 9);
			GteReadDataRegTo(&ir2, 10);
			int16_t &u = *(int16_t *)(o + 4), &w = *(int16_t *)(o + 6);
			u = (int16_t)ir1;
			w = (int16_t)ir2;
			if (u < 0) u = 0;
			else if (u >= 0x140) u = 0x13F;
			else if (w < 0) w = 0;
			else if (w >= 0xD8) w = 0xD7;
			o += 0xC;
			v += 8;
		} while (--n);
		uint8_t *h = (uint8_t *)FieldAlloc(0x4C);
		*(uint8_t **)h = mesh;
		*(uint8_t **)(h + 4) = vbuf;
		*(uint32_t *)(h + 0xC) = 0x240;
		*(uint32_t *)(h + 0x10) = 0x100;
		*(uint32_t *)(h + 0x18) = OTBase() + (uint32_t)ot_index * 4;
		GteSetIR0(fade);
		GteSetFarColor(0, 0, 0);
		*(uint8_t **)(h + 0x14) = mesh + *(const uint32_t *)mesh + 0x10;
		DrawRestTriangles(h);
		p = (uint8_t *)PacketCursor();
		PacketCursor() += 8;
		*(uint32_t *)p = 0x1000000;
		*(uint32_t *)(p + 4) = 0xE1000220;
		InsertPrimAutoDepth(*(const uint32_t *)(h + 0x18), p);
		*(uint8_t **)(h + 0x14) = mesh + *(const uint32_t *)mesh + 0x10;
		DrawWarpTriangles(h);
		FieldFree(0x4C);
		p = (uint8_t *)PacketCursor();
		PacketCursor() += 0x40;
		DrawEnv e;
		SetDefDrawEnv(&e, env.ofs[0], env.ofs[1], 0x140, 0xD8);
		e.tpage = env.tpage;
		e.dtd = 0;
		e.dfe = env.dfe;
		e.isbg = 0;
		e.b0 = e.g0 = e.r0 = 0;
		SetDrawEnv(p, &e);
		InsertPrimAutoDepth(ot, p);
		p = (uint8_t *)PacketCursor();
		PacketCursor() += 0x40;
		SetDefDrawEnv(&e, 0x240, 0x100, 0x140, 0xD8);
		e.tpage = env.tpage;
		e.dtd = env.dtd;
		e.dfe = env.dfe;
		e.isbg = 1;
		e.b0 = e.g0 = e.r0 = 0;
		SetDrawEnv(p, &e);
		InsertPrimAutoDepth(OTBase() + OT_CAPTURE, p);
	}

	static bool WarpAFrame(const MainNode *w, int32_t t, int16_t pad, Mat4x3 &frame, int16_t *otz_out)
	{
		int16_t q[4];
		BlendVectors(w->start, w->avg, 0x1000 - t, t, q);
		q[3] = pad;
		GteSetRotMatrix(&Camera());
		GteSetTransVector(&Camera());
		GteLoadV0(q);
		GteRTPS();
		int16_t sxy[2];
		GteReadSXY2(sxy);
		int32_t otz;
		GteReadOTZ(&otz);
		sxy[0] = (int16_t)(sxy[0] >> 3);
		sxy[1] = (int16_t)(sxy[1] >> 3);
		const int16_t z = (int16_t)otz;
		if (z <= 0) return false;
		const int16_t s = (int16_t)(0x18000 / (int32_t)z);
		frame.t[0] = sxy[0];
		frame.m[1][1] = s;
		frame.m[0][0] = s;
		frame.t[1] = sxy[1];
		GteSetRotMatrix(&frame);
		GteSetTransVector(&frame);
		*otz_out = z;
		return true;
	}

	static bool WarpBFrame(const MainNode *w, Mat4x3 &frame, int32_t *z_out)
	{
		int16_t box[4] = { 0x7FFF, 0x7FFF, (int16_t)0x8001, (int16_t)0x8001 };
		int32_t any = 0, minz = 0x10000;
		GteSetRotMatrix(&Camera());
		GteSetTransVector(&Camera());
		for (int32_t i = 0; i < w->count; i++) any |= HeadBox(box, w->ents[i], &minz);
		const int32_t z = minz - 0x200;
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
		frame.t[0] = cx;
		frame.t[1] = cy;
		GteSetRotMatrix(&frame);
		GteSetTransVector(&frame);
		*z_out = z;
		return true;
	}

	static void MainBlock(const int16_t *pos, PrimArg *arg)
	{
		RotY((int16_t)-*(const int16_t *)(Caster() + 0xE), &arg->m);
		arg->m.t[0] = pos[0];
		arg->m.t[1] = pos[1];
		arg->m.t[2] = pos[2];
		ComposeAffineTransform(&Camera(), &arg->m, &arg->m);
	}

	static void TargetBlock(const int16_t *pos, PrimArg *arg)
	{
		ff8fx::copy<(sizeof(arg->m.m))>(&arg->m.m, &Camera().m);
		GteSetRotMatrixCtrl(&Camera());
		GteLoadV0(pos);
		GteMVMVA_RotV0();
		GteReadMAC123(arg->m.t);
		arg->m.t[0] = (int32_t)((uint32_t)arg->m.t[0] + (uint32_t)Camera().t[0]);
		arg->m.t[1] = (int32_t)((uint32_t)arg->m.t[1] + (uint32_t)Camera().t[1]);
		arg->m.t[2] = (int32_t)((uint32_t)arg->m.t[2] + (uint32_t)Camera().t[2]);
	}

	static uint32_t __cdecl TargetTask(TaskNode *n)
	{
		TargetNode *p = (TargetNode *)n;
		PrimArg arg;
		TargetBlock(p->pos, &arg);
		arg.depth = 0;
		arg.morph = TexBase() + VERTS;
		if (!prim::play((prim::Layout *)p->layout, PrimCallback, (int)&arg, 0))
		{
			*(uint32_t *)(p->entity + 0x28) = p->colour;
			*(uint16_t *)p->entity &= 0xF7FF;
			DamageEntity(p->entity);
			return TASK_END;
		}
		p->entity[1] |= 8;
		const int32_t u = mul32((int32_t)p->counter * 3, 0x1000) / 26;
		const int32_t s = ComputeSin(u / 6);
		const int32_t f = u & 0xFFF;
		const int32_t k = u >> 12;
		uint8_t c[4];
		BlendColour(COLOURS() + k * 4, COLOURS() + k * 4 + 4, 0x1000 - f, f, c);
		BlendColour(&p->colour, c, 0x1000 - s, s, p->entity + 0x28);
		p->counter++;
		return 0;
	}

	static int16_t g_start_residue = 0x0252;
	static int16_t StartResidue() { return g_start_residue; }

	static uint32_t __cdecl MainTask(TaskNode *n)
	{
		MainNode *w = (MainNode *)n;
		if ((uint32_t)(int32_t)w->counter < 0x24)
		{
			int16_t pos[4];
			pos[3] = StartResidue();
			BonePoint(BONE_ENTRY(), pos);
			if (w->counter == 0) ff8fx::copy<(8)>(w->start, pos);
			PrimArg arg;
			MainBlock(pos, &arg);
			arg.depth = -0x80;
			arg.morph = TexBase() + VERTS;
			prim::play((prim::Layout *)w->layout, PrimCallback, (int)&arg, 0);
		}
		const int32_t c = w->counter;
		if ((uint32_t)c < 0x1E)
		{
			const int32_t t = (int32_t)((uint32_t)(c << 12) / 30);
			int16_t z;
			if (WarpAFrame(w, t, c == 0 ? StartResidue() : 0, WarpFrame(), &z))
			{
				const int32_t half = t >> 1;
				const int32_t s = ComputeSin(half);
				const uint32_t phase = (uint32_t)c << 9;
				const uint32_t k = (phase >> 12) & 3;
				BlendFrames((const uint8_t *)(game_addr(FRAMES) + k * FRAME_BYTES), (const uint8_t *)(game_addr(FRAMES) + ((k + 1) & 3) * FRAME_BYTES), (uint8_t *)(TexBase() + VERTS), (int32_t)(phase & 0xFFF));
				const int32_t s2 = ComputeSin(half);
				ScreenWarp(MESH(), (const uint8_t *)(TexBase() + VERTS), (((int16_t)(z - 0x200)) >> 2) + 0x11, 0x1000 - s2, s + s, t, (uint8_t *)(TexBase() + VBUF));
			}
		}
		const uint32_t b = (uint32_t)(c - 0x1E);
		if (b < 0x28)
		{
			int32_t z;
			if (WarpBFrame(w, WarpFrame(), &z))
			{
				const int32_t t = (int32_t)((b << 12) / 40);
				const int32_t half = t >> 1;
				const int32_t s = ComputeSin(half);
				const int32_t s2 = ComputeSin(half);
				ScreenWarp(MESH(), MESH() + 8, (z >> 2) + 0x11, 0x1000 - s2, s, t, (uint8_t *)(TexBase() + VBUF));
			}
		}
		if (w->counter == 0) BdPlaySE(SOUND_Wave(), 0, 0x80);
		const uint32_t e = (uint32_t)(w->counter - 0x1E);
		if (e < 0x10 && !(e & 3) && (e >> 2) < (uint32_t)(int32_t)w->count)
		{
			TargetNode *p = (TargetNode *)AddTaskToQueue(QTargets(), game_addr(ORIG_TargetTask));
			if (p)
			{
				const uint32_t i = e >> 2;
				p->counter = 0;
				p->index = w->index[i];
				p->entity = w->ents[i];
				p->colour = *(const uint32_t *)(p->entity + 0x28);
				GetDefaultEffectPosition(p->entity, p->pos);
				DecodeModelPrimLayout(game_addr(MODEL_Target), p->layout, 0x278);
			}
		}
		if (w->counter < 0x46)
		{
			w->counter++;
			return 0;
		}
		if (QTargets()->head) return 0;
		if (w->counter == 0x46)
		{
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
		if (m->counter == 1 && !m->started)
		{
			PrepareMesh(MESH());
			for (uint32_t f = game_addr(FRAMES) + 3 * FRAME_BYTES; f != game_addr(FRAMES); f -= FRAME_BYTES) NormaliseMesh((uint8_t *)f);
			m->started = 1;
			InitTaskQueuePool(QMain(), (void *)(TexBase() + 0xA60), 0x4DC, 1);
			InitTaskQueuePool(QTargets(), (void *)TexBase(), 0x298, 4);
			MainNode *w = (MainNode *)AddTaskToQueue(QMain(), game_addr(ORIG_MainTask));
			Memset32(&w->counter, 0, 0x134);
			DecodeModelPrimLayout(game_addr(MODEL_Main), w->layout, 0x4AC);
			w->count = (int16_t)Ctx()->actions[0].target_count;
			w->avg[2] = 0;
			w->avg[1] = 0;
			w->avg[0] = 0;
			g_start_residue = 0x0252;
			if (w->count > 0)
			{
				for (int32_t i = 0; i < w->count; i++)
				{
					w->index[i] = (uint8_t)i;
					w->ents[i] = Entity(Ctx()->actions[0].targets[i * TARGET_STRIDE]);
					int16_t pos[4];
					GetDefaultEffectPosition(w->ents[i], pos);
					if (w->ents[i][0] & 2) g_start_residue = 0x001A;
					w->avg[0] = (int16_t)(w->avg[0] + pos[0]);
					w->avg[1] = (int16_t)(w->avg[1] + pos[1]);
					w->avg[2] = (int16_t)(w->avg[2] + pos[2]);
				}
			}
			w->avg[0] = (int16_t)((int32_t)w->avg[0] / (int32_t)w->count);
			w->avg[1] = (int16_t)((int32_t)w->avg[1] / (int32_t)w->count);
			w->avg[2] = (int16_t)((int32_t)w->avg[2] / (int32_t)w->count);
		}
		int a0;
		if (m->started)
		{
			a0 = ExecuteTaskQueue(QMain());
			ExecuteTaskQueue(QTargets());
		}
		else a0 = (int)n;
		if (m->started && a0 == 0) return TASK_END;
		m->counter++;
		return 0;
	}
}

	void register_mag139_sonic_wave()
	{
		register_port(sonic139::ORIG_MasterTask, (void *)sonic139::MasterTask, "139 MasterTask", 139);
		register_port(sonic139::ORIG_MainTask, (void *)sonic139::MainTask, "139 MainTask", 139);
		register_port(sonic139::ORIG_TargetTask, (void *)sonic139::TargetTask, "139 TargetTask", 139);
	}
}

