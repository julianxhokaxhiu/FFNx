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

// Effect 129: Breath

#include "mag_common.h"
#include "mag_ice_common.h"

namespace ff8fx
{
namespace breath129
{
	using namespace eng;
	using namespace magc;
	using namespace icec;

	inline Mat4x3 &BranchFrame() { return var<Mat4x3>(game_addr(game::g129_Breath_BranchFrame)); }
	inline int32_t &FlashLevel() { return var<int32_t>(game_addr(game::g129_Breath_FlashLevel)); }
	inline uint8_t *&Caster() { return var<uint8_t *>(game_addr(game::g129_Breath_Caster)); }
	inline Mat4x3 &BranchRot() { return var<Mat4x3>(game_addr(game::g129_Breath_BranchRot)); }
	inline uint8_t *&TexBase() { return var<uint8_t *>(game_addr(game::g129_Breath_MagicBuffer)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::g129_Breath_CastCtx)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::g129_Breath_PacketCursor)); }
	inline TaskQueue *QSprites() { return (TaskQueue *)game_addr(game::g129_Breath_QSprites); }
	inline TaskQueue *QTargets() { return (TaskQueue *)game_addr(game::g129_Breath_QTargets); }
	inline TaskQueue *QArcs() { return (TaskQueue *)game_addr(game::g129_Breath_QArcs); }
	inline TaskQueue *QBranches() { return (TaskQueue *)game_addr(game::g129_Breath_QBranches); }
	inline TaskQueue *QMain() { return (TaskQueue *)game_addr(game::g129_Breath_QMain); }

	static constexpr game::Id ORIG_MasterTask = game::MAG_129_Breath_MasterTask;
	static constexpr game::Id ORIG_MainTask = game::MAG_129_Breath_MainTask;
	static constexpr game::Id ORIG_TargetTask = game::MAG_129_Breath_TargetTask;
	static constexpr game::Id ORIG_SpriteTask = game::MAG_129_Breath_SparkSpriteTask;
	static constexpr game::Id ORIG_BranchTask = game::MAG_129_Breath_BranchTask;
	static constexpr game::Id ORIG_ArcTask = game::MAG_129_Breath_ArcTask;
	static constexpr game::Id MODEL_Mouth1 = game::MAG_129_Breath_ModelMouth1;
	static constexpr game::Id MODEL_Mouth2 = game::MAG_129_Breath_ModelMouth2;
	static constexpr game::Id MODEL_Mouth3 = game::MAG_129_Breath_ModelMouth3;
	static constexpr game::Id MODEL_Target = game::MAG_129_Breath_ModelTarget;
	static uint8_t *MESH() { return (uint8_t *)game_addr(game::g129_Breath_MeshModel); }
	static uint8_t *BRANCH_VERTS() { return (uint8_t *)game_addr(game::g129_Breath_BranchVerts); }
	static constexpr game::Id SEQ_Spark = game::MAG_129_Breath_SeqSpark;
	static const int16_t *BRANCH_SCRIPT() { return (const int16_t *)game_addr(game::g129_Breath_BranchScripts); }
	static constexpr game::Id ARC_STRIPS_CASTER = game::g129_Breath_ArcStripsCaster;
	static constexpr game::Id ARC_FACES_CASTER = game::g129_Breath_ArcFacesCaster;
	static constexpr game::Id ARC_STRIPS_TARGET = game::g129_Breath_ArcStripsTarget;
	static const void *SOUND_Breath() { return (const void *)game_addr(game::MAG_129_Breath_SoundBreath); }
	static const uint8_t *DIST_TABLE() { return (const uint8_t *)game_addr(game::unk_1229F28); }
	static const uint16_t *SQRT_TABLE() { return (const uint16_t *)game_addr(game::unk_122DEA8); }
	static const int16_t *SWIRL_TABLE() { return (const int16_t *)game_addr(game::g129_Breath_SwirlTable); }
	static const uint32_t SEGS = 0x19C0;
	static const uint32_t PROFILE = 0x35C0;
	static const uint32_t HIT = 0x35E0;
	static const uint32_t VERTS = 0x35F0;

	inline int32_t CartesianToGameAngle(int32_t dx, int32_t dz) { return fn<int32_t (__cdecl *)(int32_t, int32_t)>(game_addr(game::CartesianToGameAngle))(dx, dz); }
	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void RotationFromAngles2(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::MAG_063_RotationFromAnglesYXZ))(angles, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void MulMatrix3(Mat4x3 *a, const void *b) { fn<void (__cdecl *)(Mat4x3 *, const void *)>(game_addr(game::MatrixMultiply3x3))(a, b); }
	inline void GteMVMVA_RotV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0))(); }
	inline void RotY(int32_t angle, Mat4x3 *out) { fn<void (__cdecl *)(int32_t, Mat4x3 *)>(game_addr(game::Effect_RotationYMatrix))(angle, out); }
	inline void RotZ(int32_t angle, Mat4x3 *out) { fn<void (__cdecl *)(int32_t, Mat4x3 *)>(game_addr(game::MAG_065_RotationZMatrix))(angle, out); }
	inline void GteZeroTrans() { fn<void (__cdecl *)()>(game_addr(game::GTE_ZeroTranslation))(); }
	inline uint32_t RenderPrim2(void *h, uint32_t ot, int mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int, uint32_t)>(game_addr(game::MAG_070_RenderPrimModel))(h, ot, mode, cursor); }
	inline int32_t Sqrt(int32_t v) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::Math_SqrtInt))(v); }
	inline int32_t BuildOrthonormalBasis(const int32_t *v, Mat4x3 *m) { return fn<int32_t (__cdecl *)(const int32_t *, Mat4x3 *)>(game_addr(game::BuildOrthonormalBasis))(v, m); }
	inline void GteReadOTZ4(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadOTZ_SZ3div4))(dst); }
	inline void GteLoadRGBC(const void *c) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadRGBC))(c); }
	inline void GteDPCS() { fn<void (__cdecl *)()>(game_addr(game::GTE_DPCS))(); }
	inline void GteStoreRGB2(void *c) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreRGB2))(c); }

#pragma pack(push, 1)
	struct MasterNode
	{
		TaskNode hdr;
		uint16_t counter;
		uint8_t pad0E;
		uint8_t started;
		uint32_t arena;
	};
	struct MainNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t count;
		uint8_t *ent[3];
		uint8_t slot[3];
		uint8_t pad1F;
		int16_t cur;
		int16_t next;
		int16_t walk;
		int16_t step;
		int16_t angle1;
		int16_t angle2;
		int32_t length;
		int16_t pos[4];
		uint8_t mouth1[0x108];
		uint8_t mouth2[0x68];
		uint8_t mouth3[0x188];
	};
	struct TargetNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t action;
		uint8_t slot;
		int16_t pos[3];
		int16_t angle;
		uint8_t layout[0x110];
		uint8_t faces[0xC0];
	};
	struct SpriteNode
	{
		TaskNode hdr;
		uint16_t counter;
		uint16_t pad0E;
		int16_t pos[3];
		int16_t size;
	};
	struct BoltSeg
	{
		int16_t vertex;
		int16_t width;
		int16_t sx, sy;
		int16_t otz;
		int16_t pad0A;
		int16_t sx2, sy2;
		int16_t pad10[2];
		int16_t intensity;
		int16_t decay;
		BoltSeg *next;
	};
	struct BranchNode
	{
		TaskNode hdr;
		int16_t count;
		int16_t width;
		int16_t intensity;
		int16_t decay;
		const int16_t *script;
		BoltSeg *head, *tail;
	};
	struct ArcPoint { int16_t width, color, pad; };
	struct ArcNode
	{
		TaskNode hdr;
		int16_t age;
		int16_t progress;
		int16_t speed;
		uint8_t reach_shift;
		uint8_t fade_shift;
		const int16_t *strips;
		const uint8_t *faces;
		uint8_t **bones;
		ArcPoint pts[16];
	};
	struct ArcSeg
	{
		int16_t sx, sy;
		int16_t otz;
		int16_t width;
		int16_t sx2, sy2;
		int16_t padC;
		int16_t color;
	};
	struct PrimArg
	{
		Mat4x3 m;
		int32_t depth;
		uint32_t morph;
	};
	struct Sampler
	{
		uint8_t *arena;
		uint8_t **model;
		int32_t pad08;
		int16_t unk0C;
		int16_t pad0E;
		int16_t tris;
		int16_t quads;
		uint8_t *cursor;
		int16_t object;
		int16_t base;
		int16_t verts;
		int16_t min[3];
		int16_t max[3];
		int16_t extent;
		int16_t polys;
		int16_t pad2E;
		uint8_t *out;
	};
#pragma pack(pop)
	static_assert(sizeof(MasterNode) == 0x14 && sizeof(MainNode) == 0x330 && sizeof(TargetNode) == 0x1E8, "Breath nodes");
	static_assert(sizeof(SpriteNode) == 0x18 && sizeof(BoltSeg) == 28 && sizeof(BranchNode) == 0x20, "Breath nodes");
	static_assert(sizeof(ArcNode) == 0x80 && sizeof(ArcSeg) == 16 && sizeof(PrimArg) == 0x28 && sizeof(Sampler) == 0x34, "Breath nodes");
}
}

namespace ff8fx
{
namespace breath129
{
	using namespace eng;
	using namespace magc;
	using namespace icec;

	inline BoltSeg *Segs() { return (BoltSeg *)(TexBase() + SEGS); }
	inline int16_t *Profile() { return (int16_t *)(TexBase() + PROFILE); }
	inline int32_t &HitCount() { return *(int32_t *)(TexBase() + HIT); }
	inline uint8_t *&HitEntity(int i) { return *(uint8_t **)(TexBase() + HIT + 4 + 4 * i); }
	inline uint32_t Verts() { return (uint32_t)(TexBase() + VERTS); }

	static int32_t Div15(int32_t x)
	{
		int32_t hi = (int32_t)(((int64_t)x * (int32_t)0x88888889) >> 32);
		int32_t d = (hi + x) >> 3;
		return d + (int32_t)((uint32_t)d >> 31);
	}

	static int32_t SqrtLz(int32_t v)
	{
		int32_t lz;
		GteSetLZCS(v);
		GteReadLZCR(&lz);
		int32_t e = lz & ~1;
		int32_t sh = (0x1F - e) >> 1;
		int32_t idx = e - 0x18 > 0 ? shl32(v, e - 0x18) : v >> (0x18 - e);
		return shl32(SQRT_TABLE()[idx], sh);
	}

	static void NormalizeDepth(uint8_t *model)
	{
		const int32_t count = *(const int32_t *)(model + 4);
		int32_t mn = *(const int16_t *)(model + 0x1C), mx = mn;
		const uint8_t *z = model + 0x1C;
		for (int32_t i = count - 1; i; i--, z += 8)
		{
			const int32_t v = *(const int16_t *)z;
			if (mn > v) mn = v;
			else if (mx < v) mx = v;
		}
		const int32_t range = mx - mn;
		uint8_t *p = model + 0xE;
		for (int32_t i = count; i; i--, p += 8)
			*(int16_t *)p = (int16_t)(shl32(*(const int16_t *)(p - 2), 12) / range);
	}

	static void SwirlVertices(uint8_t *model, int32_t phase, int16_t *out)
	{
		int32_t count = *(const int32_t *)(model + 4);
		const int16_t *v = (const int16_t *)(model + 8);
		do
		{
			GteLoadV0(v);
			GteMVMVA_RotV0Tr();
			GteStoreIR123(out);
			const int32_t w = v[3];
			const int32_t a = (w + phase) & 0xFFF;
			const int32_t amp = (int32_t)SWIRL_TABLE()[w & 0xFFE] >> 4;
			out[1] = (int16_t)(out[1] - (int16_t)(mul32(SWIRL_TABLE()[a * 2], amp) >> 12));
			out[0] = (int16_t)(out[0] + (int16_t)(mul32(SWIRL_TABLE()[a * 2 + 1], amp) >> 12));
			out += 4;
			v += 4;
		} while (--count);
	}

	static uint32_t MeshQuads(uint8_t *h, uint32_t ot, int shift, uint32_t cursor)
	{
		uint8_t *verts = Cursor(h, 0x20);
		uint8_t *prim = Cursor(h, 0x24);
		int32_t count = *(int32_t *)prim;
		prim += 4;
		Cursor(h, 0x24) = prim;
		uint8_t *p = (uint8_t *)cursor;
		if (count <= 0) return cursor;
		for (int32_t left = count; left; left--, prim += 0x24)
		{
			ff8fx::copy<(8)>(h + 0x38, verts + 4 * *(uint16_t *)(prim + 4));
			ff8fx::copy<(8)>(h + 0x40, verts + 4 * *(uint16_t *)(prim + 6));
			ff8fx::copy<(8)>(h + 0x48, verts + 4 * *(uint16_t *)(prim + 8));
			ff8fx::copy<(8)>(h + 0x50, verts + 4 * *(uint16_t *)(prim + 0xA));
			GteLoadV012(h + 0x38, h + 0x40, h + 0x48);
			GteRTPT();
			*(uint32_t *)p = 0x0C000000;
			*(uint32_t *)(p + 4) = *(uint32_t *)prim;
			*(uint32_t *)(p + 0xC) = *(uint32_t *)(prim + 0xC);
			*(uint32_t *)(p + 0x18) = *(uint32_t *)(prim + 0x10);
			uint32_t uv = *(uint32_t *)(prim + 0x14);
			*(uint32_t *)(p + 0x24) = uv;
			*(uint32_t *)(p + 0x30) = uv >> 16;
			GteReadFLAG(h + 0x34);
			if (*(uint32_t *)(h + 0x34) & 0x60000) continue;
			GteReadSXY012(p + 8, p + 0x14, p + 0x20);
			GteLoadV0(h + 0x50);
			GteRTPS();
			uint32_t clip = 0;
			auto offx = [](int16_t x) { return x < 0 || x > 0xA00; };
			auto offy = [](int16_t y) { return y < 0 || y > 0x6C0; };
			if (offx(*(int16_t *)(p + 8))) clip = 1;
			if (offx(*(int16_t *)(p + 0x14))) clip |= 2;
			if (offx(*(int16_t *)(p + 0x20))) clip |= 4;
			if (offy(*(int16_t *)(p + 0xA))) clip |= 0x10;
			if (offy(*(int16_t *)(p + 0x16))) clip |= 0x20;
			if (offy(*(int16_t *)(p + 0x22))) clip |= 0x40;
			GteReadSXY2(p + 0x2C);
			GteAVSZ4();
			if (offx(*(int16_t *)(p + 0x2C))) clip |= 8;
			if (offy(*(int16_t *)(p + 0x2E))) clip |= 0x80;
			if ((clip & 0xF) == 0xF || (clip & 0xF0) == 0xF0) continue;
			GteReadOTZReg(h + 0x30);
			if (*(int32_t *)(h + 8))
			{
				GteSetFarColor(h[4], h[5], h[6]);
				GteLoadRGB012(prim + 0x18, prim + 0x1C, prim + 0x20);
				GteSetIR0(*(int32_t *)(h + 8));
				GteDPCT();
				GteStoreRGB012(p + 0x10, p + 0x1C, p + 0x28);
				GteLoadRGBC(p + 4);
				GteDPCS();
				GteStoreRGB2(p + 4);
			}
			else
			{
				*(uint32_t *)(p + 0x10) = *(uint32_t *)(prim + 0x18);
				*(uint32_t *)(p + 0x1C) = *(uint32_t *)(prim + 0x1C);
				*(uint32_t *)(p + 0x28) = *(uint32_t *)(prim + 0x20);
			}
			{
				static const int vs[4] = { 0xD, 0x19, 0x25, 0x31 };
				const uint16_t add = *(uint16_t *)(h + 0xE);
				uint32_t v[4], o = 0;
				for (int k = 0; k < 4; k++) { v[k] = p[vs[k]] + (uint32_t)add; o |= v[k]; }
				for (int k = 0; k < 4; k++) p[vs[k]] = (uint8_t)(o > 0xFF ? v[k] - h[0x1C] : v[k]);
			}
			uint32_t otp = ot + 4 * (*(int32_t *)(h + 0x30) >> shift);
			uint8_t *t1 = p + 0x34;
			*(uint32_t *)t1 = 0x02000000;
			*(uint32_t *)(t1 + 4) = TextureWindow(h + 0x10);
			*(uint32_t *)(t1 + 8) = 0;
			InsertPrimAutoDepth(otp, t1);
			InsertPrimAutoDepth(otp, p);
			uint8_t *t2 = p + 0x40;
			*(uint32_t *)t2 = 0x02000000;
			*(uint32_t *)(t2 + 4) = TextureWindow(h + 0x18);
			*(uint32_t *)(t2 + 8) = 0;
			InsertPrimAutoDepth(otp, t2);
			p += 0x4C;
		}
		Cursor(h, 0x24) = prim;
		return (uint32_t)p;
	}

	static uint32_t RenderMesh(uint8_t *h, uint32_t ot, int shift, uint32_t cursor)
	{
		uint8_t *m = Cursor(h, 0);
		Cursor(h, 0x24) = m + *(uint32_t *)m + 0x1C;
		return MeshQuads(h, ot, shift, cursor);
	}

	static void MeshSwirl(int16_t angle1, int16_t angle2, int16_t *out)
	{
		Mat4x3 rz;
		RotZ(angle1, &rz);
		GteSetRotMatrix(&rz);
		GteZeroTrans();
		SwirlVertices(MESH(), angle2, out);
	}

	static int32_t MeshDraw(uint8_t *h, const int16_t *pos, int16_t x, int16_t y, int16_t z, uint16_t scroll, Mat4x3 *frame, uint32_t verts)
	{
		const int32_t dx = x - pos[0], dy = y - pos[1], dz = z - pos[2];
		const int32_t length = Sqrt((int32_t)((uint32_t)mul32(dz, dz) + (uint32_t)mul32(dy, dy) + (uint32_t)mul32(dx, dx))) + 0x200;
		int16_t sc[9] = { 0x1000, 0, 0, 0, 0x1000, 0, 0, 0, (int16_t)length };
		const int32_t v[3] = { dx, -dy, -dz };
		Mat4x3 basis;
		BuildOrthonormalBasis(v, &basis);
		MulMatrix3(&basis, sc);
		basis.t[0] = pos[0];
		basis.t[1] = pos[1];
		basis.t[2] = pos[2];
		ComposeAffineTransform(&Camera(), &basis, frame);
		GteSetRotMatrix(frame);
		GteSetTransVector(frame);
		*(uint16_t *)(h + 0x12) = 0;
		*(uint16_t *)(h + 0x10) = 0;
		*(uint16_t *)(h + 0x16) = 0x100;
		*(uint16_t *)(h + 0x14) = 0x100;
		*(uint16_t *)(h + 0x1A) = 0;
		*(uint16_t *)(h + 0x18) = 0;
		*(uint16_t *)(h + 0x1C) = 0x100;
		*(uint16_t *)(h + 0x1E) = 0x80;
		*(uint16_t *)(h + 0xE) = scroll;
		*(uint8_t **)h = MESH();
		*(uint32_t *)(h + 0x20) = verts;
		PacketCursor() = RenderMesh(h, RenderOT(), 2, PacketCursor());
		return length;
	}

	static void MeshFade(uint8_t *h, int32_t c1e)
	{
		if ((uint32_t)c1e < 8)
		{
			*(int32_t *)(h + 8) = 0x1000 - ComputeSin(shl32(c1e & 0x3FFFFF, 7));
			*(int32_t *)(h + 4) = 0;
		}
		else if ((uint32_t)c1e > 0x23)
		{
			*(int32_t *)(h + 8) = ComputeSin((shl32(c1e, 7) - 0x1101) & 0x1FFFFF80);
			*(int32_t *)(h + 4) = 0;
		}
		else *(int32_t *)(h + 8) = 0;
	}

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
			if (r->flags & 0x100)
			{
				int16_t s[9] = { r->scale[0], 0, 0, 0, r->scale[1], 0, 0, 0, r->scale[2] };
				MulMatrix3(&m, s);
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
		PacketCursor() = RenderPrim2(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static BoltSeg *SegAlloc(int16_t vertex, int16_t width, int16_t intensity, int16_t decay)
	{
		BoltSeg *s = Segs();
		int i = 0x100;
		while ((uint16_t)s->vertex != 0xFFFF)
		{
			s++;
			if (--i == 0) return nullptr;
		}
		s->vertex = vertex;
		s->width = width;
		s->intensity = intensity;
		s->decay = decay;
		s->next = nullptr;
		return s;
	}

	static void SpawnBranch(const int16_t *script, int32_t width)
	{
		int32_t r1 = CrtRand() & 0x3FF;
		int32_t r2 = CrtRand() & 0x1FF;
		int16_t intensity = (int16_t)(r1 + r2 + 0x100);
		int16_t decay = (int16_t)-(intensity >> 3);
		BoltSeg *s = SegAlloc(script[0], (int16_t)width, intensity, decay);
		if (!s) return;
		BranchNode *b = (BranchNode *)AddTaskToQueue(QBranches(), game_addr(ORIG_BranchTask));
		if (!b) return;
		Memset32(&b->count, 0, 5);
		b->tail = s;
		b->head = s;
		b->intensity = intensity;
		b->decay = decay;
		b->script = script + 1;
		b->width = (int16_t)width;
		b->count = 1;
	}

	static void SpawnBranchAt(const int16_t *table, int32_t i)
	{
		SpawnBranch(table + table[i + 1], 0x140);
	}

	template<typename Seg>
	static void SegEdges(Seg *s, const Seg *n, const int16_t *prev_dir, int16_t *dir)
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
			int32_t w = shl32(n->width, 5) / (s->otz + 0x400);
			ox = ScreenEighths(mul32(w, px) >> 12);
			oy = ScreenEighths(mul32(w, py) >> 12);
		}
		int16_t x = s->sx, y = s->sy;
		s->sx = (int16_t)(x - ox);
		s->sx2 = (int16_t)(x + ox);
		s->sy2 = (int16_t)(y + oy);
		s->sy = (int16_t)(y - oy);
	}

	static uint32_t g_branch_slot;

	static uint32_t BranchDraw(BoltSeg *head, int32_t phase, int32_t count, uint32_t slot)
	{
		for (BoltSeg *s = head; s; s = s->next)
		{
			GteSetRotMatrixCtrl(&BranchRot());
			GteZeroTrans();
			const int16_t *v = (const int16_t *)(BRANCH_VERTS() + 8 + 8 * (int32_t)s->vertex);
			int16_t w[4];
			GteLoadV0(v);
			GteMVMVA_RotV0Tr();
			GteStoreIR123(w);
			const int32_t a = (v[3] + phase) & 0xFFF;
			const int32_t amp = (int16_t)(SWIRL_TABLE()[v[3] & 0xFFE] >> 4);
			w[1] = (int16_t)(w[1] - (int16_t)(mul32(SWIRL_TABLE()[a * 2], amp) >> 12));
			w[0] = (int16_t)(w[0] + (int16_t)(mul32(SWIRL_TABLE()[a * 2 + 1], amp) >> 12));
			GteSetRotMatrixCtrl(&BranchFrame());
			GteSetTransVectorCtrl(&BranchFrame());
			GteLoadV0(w);
			GteRTPS();
			GteReadSXY2(&s->sx);
			GteReadOTZ4(&s->otz);
			uint32_t flag;
			GteReadFLAG(&flag);
			if (flag & 0x60000) s->otz = 0;
		}
		int16_t dir[2] = { (int16_t)slot, (int16_t)(slot >> 16) };
		SegEdges<BoltSeg>(head, head->next, nullptr, dir);
		BoltSeg *s = head->next;
		for (int32_t i = count - 2; i; i--)
		{
			SegEdges<BoltSeg>(s, s->next, dir, dir);
			s = s->next;
		}
		SegEdges<BoltSeg>(s, nullptr, dir, nullptr);
		slot = (uint16_t)dir[0] | (uint32_t)(uint16_t)dir[1] << 16;

		BoltSeg *a = head;
		for (int32_t i = count - 1; i; i--)
		{
			BoltSeg *b = a->next;
			if (a->otz > 0 && b->otz > 0)
			{
				uint32_t *p = (uint32_t *)PacketCursor();
				PacketCursor() += 0x34;
				p[2] = *(uint32_t *)&a->sx;
				p[5] = *(uint32_t *)&a->sx2;
				p[3] = 0x3CD4E048;
				p[6] = 0x00B8E078;
				*(uint16_t *)((uint8_t *)p + 0x24) = 0xE048;
				*(uint16_t *)((uint8_t *)p + 0x30) = 0xE078;
				p[8] = *(uint32_t *)&b->sx;
				p[11] = *(uint32_t *)&b->sx2;
				uint32_t ca = (uint32_t)(int32_t)(int16_t)(a->intensity >> 4);
				ca = (((ca << 8) | ca) << 8) | ca;
				p[4] = ca;
				p[1] = ca;
				uint32_t cb = (uint32_t)(int32_t)(int16_t)(b->intensity >> 4);
				cb = (((cb << 8) | cb) << 8) | cb;
				p[0] = 0x0C000000;
				((uint8_t *)p)[7] = 0x3E;
				p[10] = cb;
				p[7] = cb;
				InsertPrimAutoDepth(var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44 + 4 * ((b->otz + a->otz) >> 5), p);
			}
			a = b;
		}
		return slot;
	}

	static uint32_t BranchFreeAll(BranchNode *b, uint32_t slot)
	{
		for (BoltSeg *s = b->head; s; s = s->next) s->vertex = -1;
		g_branch_slot = slot;
		return TASK_END;
	}

	static uint32_t __cdecl BranchTask(TaskNode *n)
	{
		BranchNode *b = (BranchNode *)n;
		uint32_t slot = g_branch_slot;
		uint32_t edi = 0;
		for (TaskNode *q = QBranches()->head; q && q != n; q = q->next) edi = (uint32_t)q;
		for (int iter = 4; iter; iter--)
		{
			if (!b->script) continue;
			int16_t v = *b->script++;
			edi = (edi & 0xFFFF0000u) | (uint16_t)v;
			while (v < 0)
			{
				SpawnBranch(b->script, b->width);
				slot = 0;
				edi = (uint32_t)b->script;
				b->script += -1 - v;
				v = *b->script++;
				edi = (edi & 0xFFFF0000u) | (uint16_t)v;
			}
			const int16_t *peek = b->script;
			int16_t c = *peek;
			if (c < 0) { peek -= c; c = *peek; }
			if (c == 0x7FFF)
			{
				if (b->count <= 3) return BranchFreeAll(b, slot);
				b->script = nullptr;
			}
			int32_t w = b->width;
			int16_t intensity = b->intensity, decay = b->decay;
			int16_t width = (int16_t)(((CrtRand() * w) >> 15) + w);
			slot = edi;
			BoltSeg *s = SegAlloc(v, width, intensity, decay);
			if (!s) return BranchFreeAll(b, slot);
			b->tail->next = s;
			b->count++;
			b->tail = s;
		}
		for (BoltSeg *s = b->head; s; s = s->next)
			if (s->intensity > 0)
			{
				s->intensity += s->decay;
				if (s->intensity <= 0) s->intensity = 0;
			}
		if (b->count >= 2) slot = BranchDraw(b->head, ((MainNode *)(TexBase() + 0x1690))->angle2, b->count, slot);
		g_branch_slot = slot;
		BoltSeg *s = b->head;
		if (s->intensity > 0x10) return 0;
		for (;;)
		{
			BoltSeg *nx = s->next;
			if (nx->intensity > 0x10) return 0;
			s->vertex = -1;
			b->count--;
			b->head = nx;
			if (b->count < 2) return TASK_END;
			s = nx;
			if (s->intensity > 0x10) return 0;
		}
	}

	static void TriangleFrame(const uint8_t *bone_mats, const uint8_t *face, int16_t *pos, int16_t *normal)
	{
		int16_t w[3][4];
		for (int k = 0; k < 3; k++)
		{
			const int16_t *v = (const int16_t *)(face + 8 * k);
			const Mat4x3 *m = (const Mat4x3 *)(bone_mats + 0x10 + 48 * (int32_t)v[3]);
			GteSetRotMatrixCtrl(m);
			GteSetTransVectorCtrl(m);
			GteLoadV0(v);
			GteMVMVA_RotV0Tr();
			GteStoreIR123(w[k]);
		}
		int32_t e1[3] = { w[0][0] - w[1][0], w[0][1] - w[1][1], w[0][2] - w[1][2] };
		int32_t e2[3] = { w[2][0] - w[0][0], w[2][1] - w[0][1], w[2][2] - w[0][2] };
		int32_t n[3];
		GteSetRotDiagonal(e1);
		GteLoadIR123_32(e2);
		GteOP();
		GteReadMAC123(n);
		int32_t m = n[0] < 0 ? -n[0] : n[0];
		int32_t a = n[1] < 0 ? -n[1] : n[1];
		if (a > m) m = a;
		a = n[2] < 0 ? -n[2] : n[2];
		if (a > m) m = a;
		GteSetLZCS(m);
		for (int k = 0; k < 3; k++)
		{
			int32_t c = w[2][k] + w[1][k] + w[0][k];
			pos[k] = (int16_t)(mul32(c, 0x5555) >> 16);
		}
		int32_t lz;
		GteReadLZCR(&lz);
		if (lz < 0x12)
		{
			int sh = 0x12 - lz;
			n[0] >>= sh;
			n[1] >>= sh;
			n[2] >>= sh;
		}
		int32_t len2;
		GteLoadIR123_32b(n);
		GteSQR();
		GteSumMAC123(&len2);
		int32_t len = SqrtLz(len2) >> 12;
		normal[0] = (int16_t)(shl32(n[0], 12) / len);
		normal[1] = (int16_t)(shl32(n[1], 12) / len);
		normal[2] = (int16_t)(shl32(n[2], 12) / len);
	}

	static void StripPoint(const uint8_t *bone_mats, const uint8_t *faces, int32_t n, int32_t progress, int16_t *pos, int16_t *normal)
	{
		int32_t at = mul32(n - 1, progress);
		int32_t frac = at & 0xFFF;
		const uint8_t *face = faces + 24 * (at >> 12);
		if (frac == 0)
		{
			TriangleFrame(bone_mats, face, pos, normal);
			return;
		}
		int16_t p1[4], p2[4], n1[4], n2[4], nl[4];
		TriangleFrame(bone_mats, face, p1, n1);
		TriangleFrame(bone_mats, face + 24, p2, n2);
		int32_t inv = 0x1000 - frac;
		GteSetIR0(inv);
		GteLoadIR123(p1);
		GteGPF();
		GteSetIR0(frac);
		GteLoadIR123(p2);
		GteGPL();
		GteStoreIR123(pos);
		GteSetIR0(inv);
		GteLoadIR123(n1);
		GteGPF();
		GteSetIR0(frac);
		GteLoadIR123(n2);
		GteGPL();
		GteStoreIR123(nl);
		int32_t len2;
		GteLoadIR123(nl);
		GteSQR();
		GteSumMAC123(&len2);
		int32_t len = SqrtLz(len2) >> 12;
		normal[0] = (int16_t)(shl32(nl[0], 12) / len);
		normal[1] = (int16_t)(shl32(nl[1], 12) / len);
		normal[2] = (int16_t)(shl32(nl[2], 12) / len);
	}

	static void ArcDraw(ArcSeg *pts, const int16_t *dir0)
	{
		GteSetRotMatrixCtrl(&Camera());
		GteSetTransVectorCtrl(&Camera());
		for (int k = 0; k < 16; k++)
		{
			int32_t sz3;
			GteLoadV0(&pts[k]);
			GteRTPS();
			GteReadDataReg(14, &pts[k]);
			GteReadDataRegTo(&sz3, 19);
			pts[k].otz = (int16_t)(sz3 >> 2);
		}
		int16_t dir[2] = { dir0[0], dir0[1] };
		SegEdges<ArcSeg>(&pts[0], &pts[1], nullptr, dir);
		for (int k = 1; k < 15; k++)
			SegEdges<ArcSeg>(&pts[k], &pts[k + 1], dir, dir);
		SegEdges<ArcSeg>(&pts[15], nullptr, dir, nullptr);
		for (int k = 0; k < 15; k++)
		{
			const ArcSeg *a = &pts[k], *b = &pts[k + 1];
			if (a->otz <= 0 || b->otz <= 0) continue;
			uint32_t *p = (uint32_t *)PacketCursor();
			PacketCursor() += 0x34;
			p[2] = *(const uint32_t *)&a->sx;
			p[5] = *(const uint32_t *)&a->sx2;
			p[3] = 0x3CD4E048;
			p[6] = 0x00B8E078;
			*(uint16_t *)((uint8_t *)p + 0x24) = 0xE048;
			*(uint16_t *)((uint8_t *)p + 0x30) = 0xE078;
			p[8] = *(const uint32_t *)&b->sx;
			p[11] = *(const uint32_t *)&b->sx2;
			uint32_t ca = (uint32_t)(int32_t)a->color;
			ca = (((ca << 8) | ca) << 8) | ca;
			p[4] = ca;
			p[1] = ca;
			uint32_t cb = (uint32_t)(int32_t)b->color;
			cb = (((cb << 8) | cb) << 8) | cb;
			p[0] = 0x0C000000;
			((uint8_t *)p)[7] = 0x3E;
			p[10] = cb;
			p[7] = cb;
			InsertPrimAutoDepth(var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44 + 4 * ((b->otz + a->otz) >> 4), p);
		}
	}

	static ArcSeg *ArcCurve(ArcNode *arc, int16_t progress, int32_t *length)
	{
		const uint8_t *bone_mats = *arc->bones + 0x10;
		int16_t p1[4], n1[4], p2[4], n2[4];
		StripPoint(bone_mats, arc->faces + 24 * (int32_t)arc->strips[0], arc->strips[1], progress, p1, n1);
		StripPoint(bone_mats, arc->faces + 24 * (int32_t)arc->strips[2], arc->strips[3], progress, p2, n2);
		int32_t d[3] = { p2[0] - p1[0], p2[1] - p1[1], p2[2] - p1[2] };
		GteLoadIR123_32b(d);
		GteSQR();
		GteReadMAC123(d);
		int32_t r = SqrtLz(d[2] + d[1] + d[0]);
		*length = r >> 12;
		GteSetIR0(r >> arc->reach_shift);
		GteLoadIR123(n1);
		GteGPF();
		GteStoreIR123(n1);
		GteLoadIR123(n2);
		GteGPF();
		GteStoreIR123(n2);
		for (int k = 0; k < 3; k++)
		{
			n1[k] += p1[k];
			n2[k] += p2[k];
		}
		ArcSeg *pts = (ArcSeg *)FieldAlloc(0x100);
		ff8fx::copy<(8)>(&pts[0], p1);
		pts[0].width = arc->pts[0].width;
		pts[0].color = arc->pts[0].color;
		ff8fx::copy<(8)>(&pts[15], p2);
		pts[15].width = arc->pts[15].width;
		pts[15].color = arc->pts[15].color;
		for (int i = 1; i < 15; i++)
		{
			int32_t t = Div15(shl32(i, 12)), u = 0x1000 - t;
			int16_t q1[4], q2[4];
			GteSetIR0(u);
			GteLoadIR123(p1);
			GteGPF();
			GteSetIR0(t);
			GteLoadIR123(n1);
			GteGPL();
			GteStoreIR123(q1);
			GteSetIR0(t);
			GteLoadIR123(p2);
			GteGPF();
			GteSetIR0(u);
			GteLoadIR123(n2);
			GteGPL();
			GteStoreIR123(q2);
			GteSetIR0(u);
			GteLoadIR123(q1);
			GteGPF();
			GteSetIR0(t);
			GteLoadIR123(q2);
			GteGPL();
			GteStoreIR123(&pts[i]);
			pts[i].width = arc->pts[i].width;
			pts[i].color = arc->pts[i].color;
		}
		return pts;
	}

	static uint32_t __cdecl ArcTask(TaskNode *n)
	{
		ArcNode *arc = (ArcNode *)n;
		int32_t length;
		ArcSeg *pts = ArcCurve(arc, arc->progress, &length);
		for (int i = 1; i < 15; i++)
		{
			int32_t amp = mul32(Profile()[i], length) >> 12;
			int32_t rv[3];
			rv[0] = CrtRand() - 0x4000;
			pts[i].sx += (int16_t)(mul32(rv[0], amp) >> 20);
			rv[1] = CrtRand() - 0x4000;
			pts[i].sy += (int16_t)(mul32(rv[1], amp) >> 20);
			rv[2] = CrtRand() - 0x4000;
			pts[i].otz += (int16_t)(mul32(rv[2], amp) >> 20);
		}
		const int16_t dir0[2] = { (int16_t)(uint32_t)pts, (int16_t)((uint32_t)pts >> 16) };
		ArcDraw(pts, dir0);
		FieldFree(0x100);
		arc->progress += arc->speed;
		if (arc->progress <= 0x1000) return 0;
		arc->progress = 0x1000;
		for (int k = 0; k < 16; k++)
		{
			int32_t dec = (0x1400 - Profile()[k]) >> arc->fade_shift;
			arc->pts[k].color -= (int16_t)dec;
			if (arc->pts[k].color < 0) arc->pts[k].color = 0;
		}
		arc->age++;
		return arc->age < 10 ? 0 : TASK_END;
	}

	static void SpawnArc(uint8_t **bones, const int16_t *strips, const uint8_t *faces, uint8_t reach_shift, uint8_t fade_shift, int16_t speed)
	{
		ArcNode *arc = (ArcNode *)AddTaskToQueue(QArcs(), game_addr(ORIG_ArcTask));
		if (!arc) return;
		arc->reach_shift = reach_shift;
		arc->fade_shift = fade_shift;
		arc->speed = speed;
		arc->strips = strips;
		arc->age = 0;
		arc->progress = 0;
		arc->faces = faces;
		arc->bones = bones;
		for (int i = 0; i < 16; i++)
		{
			int32_t s = ComputeSin(Div15(shl32(i, 11)));
			arc->pts[i].color = 0x80;
			arc->pts[i].width = (int16_t)(shl32(shl32(s, 6) + s, 2) >> 12);
			arc->pts[i].pad = 0;
		}
	}

	static void Centroid(Sampler *s, int32_t a, int32_t b, int32_t c, int32_t d)
	{
		uint8_t *ar = s->arena;
		int16_t x = (int16_t)(*(int16_t *)(ar + 16 * a + 8) + *(int16_t *)(ar + 16 * b + 8));
		int16_t y = (int16_t)(*(int16_t *)(ar + 16 * a + 0xA) + *(int16_t *)(ar + 16 * b + 0xA));
		int16_t z = (int16_t)(*(int16_t *)(ar + 16 * a + 0xC) + *(int16_t *)(ar + 16 * b + 0xC));
		x = (int16_t)(x + *(int16_t *)(ar + 16 * c + 8));
		y = (int16_t)(y + *(int16_t *)(ar + 16 * c + 0xA));
		z = (int16_t)(z + *(int16_t *)(ar + 16 * c + 0xC));
		int16_t rx, ry, rz;
		if (d < 0)
		{
			auto third = [](int16_t v) { int32_t hi = (int32_t)(((int64_t)v * 0x55555556) >> 32); return (int16_t)(hi + (int32_t)((uint32_t)hi >> 31)); };
			rx = third(x);
			ry = third(y);
			rz = third(z);
		}
		else
		{
			x = (int16_t)(x + *(int16_t *)(ar + 16 * d + 8));
			y = (int16_t)(y + *(int16_t *)(ar + 16 * d + 0xA));
			z = (int16_t)(z + *(int16_t *)(ar + 16 * d + 0xC));
			rx = (int16_t)((int32_t)x / 4);
			ry = (int16_t)((int32_t)y / 4);
			rz = (int16_t)((int32_t)z / 4);
		}
		int16_t *o = (int16_t *)s->cursor;
		o[0] = rx;
		o[1] = ry;
		o[2] = rz;
		o[3] = (int16_t)a;
		o[4] = (int16_t)b;
		o[5] = (int16_t)c;
		s->cursor += 12;
	}

	static void Centroids(Sampler *s)
	{
		int16_t *table = (int16_t *)(s->arena + 0x8000);
		int32_t n = 0;
		s->cursor = s->arena + 2 * (int32_t)s->polys + 0x8002;
		const uint32_t *objects = (const uint32_t *)s->model[1];
		const int32_t count = (int32_t)*objects++;
		s->object = 0;
		s->verts = 0;
		if (count > 0)
		{
			do
			{
				s->base = s->verts;
				const uint8_t *o = s->model[1] + *objects++;
				int32_t groups = *(const int16_t *)o;
				o += 2;
				for (; groups > 0; groups--)
				{
					const int16_t nv = *(const int16_t *)(o + 2);
					s->verts = (int16_t)(s->verts + nv);
					o += 2 + 6 * (int32_t)nv + 2;
				}
				o = (const uint8_t *)(((uint32_t)o + 3) & ~3u);
				s->tris = *(const int16_t *)o;
				s->quads = *(const int16_t *)(o + 2);
				const uint8_t *poly = o + 0xC;
				for (;;)
				{
					const int16_t c = s->tris;
					s->tris = (int16_t)(c - 1);
					if (c == 0) break;
					const int32_t b = (uint16_t)s->base;
					const int32_t i0 = (uint16_t)((*(const uint16_t *)poly & 0xFFF) + b);
					const int32_t i1 = (uint16_t)((*(const uint16_t *)(poly + 2) & 0xFFF) + b);
					const int32_t i2 = (uint16_t)((*(const uint16_t *)(poly + 4) & 0xFFF) + b);
					table[n] = (int16_t)((int32_t)(s->cursor - s->arena - 0x8000) >> 1);
					Centroid(s, i0, i1, i2, -1);
					n++;
					poly += 0x10;
				}
				for (;;)
				{
					const int16_t c = s->quads;
					s->quads = (int16_t)(c - 1);
					if (c == 0) break;
					const int32_t b = (uint16_t)s->base;
					const int32_t i0 = (uint16_t)((*(const uint16_t *)poly & 0xFFF) + b);
					const int32_t i1 = (uint16_t)((*(const uint16_t *)(poly + 2) & 0xFFF) + b);
					const int32_t i2 = (uint16_t)((*(const uint16_t *)(poly + 4) & 0xFFF) + b);
					const int32_t i3 = (uint16_t)((*(const uint16_t *)(poly + 6) & 0xFFF) + b);
					table[n] = (int16_t)((int32_t)(s->cursor - s->arena - 0x8000) >> 1);
					Centroid(s, i0, i1, i2, i3);
					n++;
					poly += 0x14;
				}
				s->object++;
			} while (s->object < count);
		}
		table[n] = (int16_t)((int32_t)(s->cursor - s->arena - 0x8000) >> 1);
	}

	static void SampleVertices(Sampler *s)
	{
		uint8_t *v = s->arena;
		const uint8_t *bone_mats = s->model[0] + 0x10;
		const uint32_t *objects = (const uint32_t *)s->model[1];
		const int32_t count = (int32_t)*objects++;
		s->min[0] = s->min[1] = s->min[2] = 0x7FFF;
		s->max[0] = s->max[1] = s->max[2] = (int16_t)0x8000;
		s->object = 0;
		s->polys = 0;
		if (count > 0)
		{
			do
			{
				const uint8_t *o = s->model[1] + *objects++;
				int32_t groups = *(const int16_t *)o;
				o += 2;
				for (; groups > 0; groups--)
				{
					const int16_t bone = *(const int16_t *)o;
					const Mat4x3 *m = (const Mat4x3 *)(bone_mats + 0x10 + 48 * (int32_t)bone);
					o += 2;
					GteSetRotMatrixCtrl(m);
					GteSetTransVectorCtrl(m);
					int32_t nv = *(const int16_t *)o;
					o += 2;
					for (; nv; nv--)
					{
						int16_t *e = (int16_t *)v;
						e[0] = *(const int16_t *)o;
						e[1] = *(const int16_t *)(o + 2);
						e[2] = *(const int16_t *)(o + 4);
						e[3] = bone;
						o += 6;
						GteLoadV0(e);
						GteMVMVA_RotV0Tr();
						GteStoreIR123(e + 4);
						for (int k = 0; k < 3; k++)
						{
							if (s->min[k] > e[4 + k]) s->min[k] = e[4 + k];
							else if (s->max[k] < e[4 + k]) s->max[k] = e[4 + k];
						}
						v += 0x10;
					}
				}
				o = (const uint8_t *)(((uint32_t)o + 3) & ~3u);
				s->polys = (int16_t)(s->polys + *(const int16_t *)o);
				s->polys = (int16_t)(s->polys + *(const int16_t *)(o + 2));
				s->object++;
			} while (s->object < count);
		}
		for (int k = 0; k < 3; k++) s->max[k] = (int16_t)(s->max[k] - s->min[k]);
		s->extent = s->max[0];
		if (s->max[1] > s->max[0]) s->extent = s->max[1];
		if (s->max[2] > s->extent) s->extent = s->max[2];
		Centroids(s);
	}

	static int16_t ClosestAt(Sampler *s, int32_t i, int16_t len)
	{
		const int16_t *table = (const int16_t *)(s->arena + 0x8000);
		int32_t best = 0x7FFF, found = i;
		const int16_t *c = table + table[i];
		const int16_t *t = table;
		for (int32_t k = 0; k < s->polys; k++, t++)
		{
			const int16_t *o = table + *t;
			int16_t d[3];
			d[0] = (int16_t)(c[0] - o[0]);
			d[1] = (int16_t)(c[1] - o[1]);
			d[2] = (int16_t)(c[2] - o[2]);
			ff8fx::copy<(6)>((uint8_t *)s + 8, d);
			int32_t sum;
			GteLoadIR123((uint8_t *)s + 8);
			GteSQR();
			GteSumMAC123(&sum);
			int32_t lz;
			GteSetLZCS(sum);
			GteReadLZCR(&lz);
			const int32_t e = lz & ~1;
			const int32_t sh = (0x1F - e) >> 1;
			const int32_t idx = e - 0x18 > 0 ? shl32(sum, e - 0x18) : sum >> (0x18 - e);
			int32_t diff = (shl32(SQRT_TABLE()[idx], sh) >> 12) - len;
			if (diff < 0) diff = -diff;
			if ((int16_t)best > diff)
			{
				best = diff;
				found = k;
			}
		}
		return (int16_t)found;
	}

	static void EmitFace(Sampler *s, int32_t k)
	{
		const int16_t *table = (const int16_t *)(s->arena + 0x8000);
		const int16_t *rec = table + table[k];
		for (int j = 0; j < 3; j++)
			ff8fx::copy<(8)>(s->out + 8 * j, s->arena + 16 * (int32_t)rec[3 + j]);
		s->out += 0x18;
	}

	static void SampleFaces(uint8_t *anim, uint8_t *out, int32_t n)
	{
		Sampler *s = (Sampler *)FieldAlloc(0x34);
		s->arena = (uint8_t *)PacketCursor();
		s->model = *(uint8_t ***)(anim + 4);
		BuildBoneMatricesFromPose(anim);
		SampleVertices(s);
		s->out = out;
		for (; n; n--)
		{
			int16_t a, b;
			do
			{
				a = (int16_t)((CrtRand() * (int32_t)s->polys) >> 15);
				const int16_t ext = s->extent;
				const int32_t r = (CrtRand() * (int32_t)ext) >> 16;
				const int16_t len = (int16_t)(r + (int16_t)(ext >> 2) + (int16_t)(ext >> 1));
				b = ClosestAt(s, a, len);
			} while (a == b);
			EmitFace(s, a);
			EmitFace(s, b);
		}
		FieldFree(0x34);
	}

	static void SpriteDraw(const SpriteNode *p)
	{
		uint8_t *h = AllocHeader(0xB4);
		TransformCameraByShadowRotation(p->pos, p->size, -((int32_t)p->size >> 2));
		*(uint32_t *)h = game_addr(SEQ_Spark);
		*(uint16_t *)(h + 4) = p->counter;
		*(uint16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static uint32_t __cdecl SpriteTask(TaskNode *n)
	{
		SpriteNode *p = (SpriteNode *)n;
		SpriteDraw(p);
		p->counter++;
		return (int16_t)p->counter < 9 ? 0 : TASK_END;
	}

	static uint32_t __cdecl TargetTask(TaskNode *n)
	{
		TargetNode *t = (TargetNode *)n;
		uint8_t *entity = Entity(t->slot);
		PrimArg arg;
		RotY(t->angle, &arg.m);
		arg.m.t[0] = t->pos[0];
		arg.m.t[1] = t->pos[1];
		arg.m.t[2] = t->pos[2];
		ComposeAffineTransform(&Camera(), &arg.m, &arg.m);
		arg.morph = Verts();
		arg.depth = 0;
		prim::play((prim::Layout *)t->layout, PrimCallback, (int)&arg, 0);
		if (t->counter <= 8 && !(t->counter & 1))
		{
			SpriteNode *p = (SpriteNode *)AddTaskToQueue(QSprites(), game_addr(ORIG_SpriteTask));
			if (p)
			{
				const int32_t r = CrtRand();
				GetEffectSpawnPosition(entity, (r * (int32_t)***(uint8_t ***)(entity + 0x64)) >> 15, 0, p->pos);
				p->counter = 0;
				p->size = (int16_t)((CrtRand() & 0x1FF) + 0x400);
			}
		}
		if (t->counter == 0x10) ApplyActionResultToTarget(Ctx()->actions[0].targets + 24 * t->action);
		if (t->counter >= 0xC && !(t->counter & 3))
			SpawnArc(*(uint8_t ***)(entity + 0x64), (const int16_t *)(game_addr(ARC_STRIPS_TARGET) + 8 * ((t->counter - 0xC) >> 2)), t->faces, 0xC, 5, 0x800);
		t->counter++;
		return t->counter < 0x18 ? 0 : TASK_END;
	}

	static uint32_t __cdecl MainTask(TaskNode *n)
	{
		MainNode *m = (MainNode *)n;
		{
			const int16_t c = m->counter;
			if (c < 4) { if (shl32(c, 9) > FlashLevel()) FlashLevel() = shl32(c, 9); }
			else if (c <= 0x45) { if (FlashLevel() < 0x800) FlashLevel() = 0x800; }
			else { const int32_t v = shl32(0x49 - c, 9); if (v > FlashLevel()) FlashLevel() = v; }
		}
		if (m->counter < 0x14 && !(m->counter & 1))
			SpawnArc(*(uint8_t ***)(Caster() + 0x64), (const int16_t *)(game_addr(ARC_STRIPS_CASTER) + 8 * ((m->counter >> 1) & 3)), (const uint8_t *)game_addr(ARC_FACES_CASTER), 0xC, 7, 0x100);
		GetEffectSpawnPosition(Caster(), 9, 0x2000, m->pos);
		if (*Caster() & 2) g_branch_slot = 0x43300000;
		const int32_t c1c = m->counter - 0x1C;
		if ((uint32_t)c1c < 0x2D)
		{
			PrimArg a1;
			a1.m.m[0][0] = 0x1000; a1.m.m[0][1] = 0; a1.m.m[0][2] = 0;
			a1.m.m[1][0] = 0; a1.m.m[1][1] = 0x1000; a1.m.m[1][2] = 0;
			a1.m.m[2][0] = 0; a1.m.m[2][1] = 0; a1.m.m[2][2] = 0x1000;
			a1.m.pad = 0;
			a1.m.t[0] = m->pos[0];
			a1.m.t[1] = m->pos[1];
			a1.m.t[2] = m->pos[2];
			ComposeAffineTransform(&Camera(), &a1.m, &a1.m);
			a1.morph = Verts();
			a1.depth = -0x80;
			prim::play((prim::Layout *)m->mouth1, PrimCallback, (int)&a1, 0);
			if (prim::play((prim::Layout *)m->mouth2, PrimCallback, (int)&a1, 0) == 0)
			{
				DecodeModelPrimLayout(game_addr(MODEL_Mouth2), m->mouth2, 0x68);
			}
			if (c1c <= 0x20)
			{
				PrimArg a2;
				RotY(*(int16_t *)(Caster() + 0xE), &a2.m);
				a2.m.t[0] = m->pos[0];
				a2.m.t[1] = m->pos[1];
				a2.m.t[2] = m->pos[2];
				ComposeAffineTransform(&Camera(), &a2.m, &a2.m);
				a2.morph = Verts();
				a2.depth = 0;
				prim::play((prim::Layout *)m->mouth3, PrimCallback, (int)&a2, 0);
			}
		}
		const int32_t c1e = m->counter - 0x1E;
		if ((uint32_t)c1e < 0x2B)
		{
			uint8_t *h = AllocHeader(0x5C);
			MeshSwirl(m->angle1, m->angle2, (int16_t *)Verts());
			int16_t from[4], to[4];
			GetEffectSpawnPosition(m->ent[m->cur], 0xF1, 0, from);
			GetEffectSpawnPosition(m->ent[m->next], 0xF1, 0, to);
			const int16_t x = (int16_t)(from[0] + ((((int32_t)to[0] - from[0]) * m->walk) >> 12));
			const int16_t y = (int16_t)(from[1] + ((((int32_t)to[1] - from[1]) * m->walk) >> 12));
			const int16_t z = (int16_t)(from[2] + ((((int32_t)to[2] - from[2]) * m->walk) >> 12));
			if ((uint32_t)c1e >= 5)
			{
				m->walk = (int16_t)(m->walk + m->step);
				if (m->walk >= 0x1000)
				{
					TargetNode *t = (TargetNode *)AddTaskToQueue(QTargets(), game_addr(ORIG_TargetTask));
					if (t)
					{
						t->counter = 0;
						t->action = m->slot[m->cur];
						uint8_t *e = m->ent[m->cur];
						t->slot = (uint8_t)(((int32_t)(e - Entity(0))) / 0x9C);
						GetEffectSpawnPosition(e, 0xF1, 0, t->pos);
						t->pos[1] = *(int16_t *)(e + 0x24);
						t->angle = (int16_t)CartesianToGameAngle((int32_t)from[0] - m->pos[0], (int32_t)from[2] - m->pos[2]);
						SampleFaces(e + 0x60, t->faces, 4);
						HitEntity(HitCount()) = e;
						HitCount()++;
						DecodeModelPrimLayout(game_addr(MODEL_Target), t->layout, 0x110);
					}
					m->walk = (int16_t)(m->walk - 0x1000);
					m->cur = m->next;
					m->next = (int16_t)(m->next + 1);
					if (m->next >= m->count) m->next = (int16_t)(m->count - 1);
				}
			}
			MeshFade(h, c1e);
			const uint8_t c8 = (uint8_t)c1e;
			m->length = MeshDraw(h, m->pos, x, y, z, (uint16_t)((c8 & 7) << 4), &BranchFrame(), Verts());
			if (*(const int32_t *)(MESH() + *(const uint32_t *)MESH() + 0x1C) > 0) g_branch_slot = 0;
			FieldFree(0x5C);
			if (!(c8 & 1))
			{
				const int32_t r = CrtRand();
				SpawnBranchAt(BRANCH_SCRIPT(), (r * (int32_t)BRANCH_SCRIPT()[0]) >> 15);
			}
		}
		RotZ((int16_t)-m->angle1, &BranchRot());
		const int left = ExecuteTaskQueue(QBranches());
		if (m->counter == 0) BdPlaySE(SOUND_Breath(), 0, 0x80);
		m->angle1 = (int16_t)(m->angle1 - 100);
		m->angle2 = (int16_t)(m->angle2 + 200);
		if (m->counter >= 0x49 && left == 0) return TASK_END;
		m->counter++;
		return 0;
	}

	static uint32_t __cdecl MasterTask(TaskNode *n)
	{
		MasterNode *r = (MasterNode *)n;
		if (r->arena)
		{
			PacketCursor() = (uint32_t)(TexBase() + 0x3B70);
			r->arena = 0;
		}
		else
		{
			PacketCursor() = (uint32_t)(TexBase() + 0x1BB70);
			r->arena = 1;
		}
		FlashLevel() = 0;
		if (r->counter == 1 && !r->started)
		{
			r->started = 1;
			InitTaskQueuePool(QMain(), TexBase() + 0x1690, 0x330, 1);
			InitTaskQueuePool(QBranches(), TexBase() + 0xE90, 0x20, 0x40);
			InitTaskQueuePool(QArcs(), TexBase() + 0x690, 0x80, 0x10);
			InitTaskQueuePool(QTargets(), TexBase() + 0xD8, 0x1E8, 3);
			InitTaskQueuePool(QSprites(), TexBase(), 0x18, 9);
			Memset32(TexBase() + SEGS, 0, 0x700);
			for (int i = 0; i < 0x100; i++) Segs()[i].vertex = -1;
			for (int i = 0; i < 16; i++) Profile()[i] = (int16_t)ComputeSin(Div15(shl32(i, 11)));
			MainNode *mn = (MainNode *)AddTaskToQueue(QMain(), game_addr(ORIG_MainTask));
			Memset32(&mn->counter, 0, 0xC9);
			DecodeModelPrimLayout(game_addr(MODEL_Mouth1), mn->mouth1, 0x108);
			DecodeModelPrimLayout(game_addr(MODEL_Mouth2), mn->mouth2, 0x68);
			DecodeModelPrimLayout(game_addr(MODEL_Mouth3), mn->mouth3, 0x188);
			mn->count = (int16_t)Ctx()->actions[0].target_count;
			for (int i = 0; i < mn->count; i++)
			{
				mn->slot[i] = (uint8_t)i;
				mn->ent[i] = Entity(Ctx()->actions[0].targets[i * TARGET_STRIDE]);
			}
			for (int i = 0; i < mn->count - 1; i++)
				for (int j = i + 1; j < mn->count; j++)
					if (*(int16_t *)(mn->ent[i] + 0x1C) > *(int16_t *)(mn->ent[j] + 0x1C))
					{
						uint8_t *e = mn->ent[i];
						mn->ent[i] = mn->ent[j];
						mn->ent[j] = e;
						const uint8_t s = mn->slot[i];
						mn->slot[i] = mn->slot[j];
						mn->slot[j] = s;
					}
			mn->next = mn->count > 1 ? 1 : 0;
			{
				const int32_t x = shl32(mn->count, 12);
				const int32_t hi = (int32_t)(((int64_t)x * 0x3E0F83E1) >> 32) >> 3;
				mn->step = (int16_t)(hi + (int32_t)((uint32_t)hi >> 31));
			}
			NormalizeDepth(MESH());
			NormalizeDepth(BRANCH_VERTS());
			HitCount() = 0;
			HitEntity(2) = nullptr;
			HitEntity(1) = nullptr;
			HitEntity(0) = nullptr;
		}
		int a, b, c, d;
		if (r->started)
		{
			a = ExecuteTaskQueue(QMain());
			for (int i = 0; i < 3; i++)
				if (HitEntity(i)) ComputeBonesWorldMatrices(HitEntity(i) + 0x60, HitEntity(i) + 0x40);
			ComputeBonesWorldMatrices(Caster() + 0x60, Caster() + 0x40);
			b = ExecuteTaskQueue(QArcs());
			for (int i = 0; i < 3; i++)
				if (HitEntity(i)) BuildBoneMatricesFromPose(HitEntity(i) + 0x60);
			BuildBoneMatricesFromPose(Caster() + 0x60);
			c = ExecuteTaskQueue(QTargets());
			d = ExecuteTaskQueue(QSprites());
		}
		else a = b = c = d = (int)n;
		SetScreenFlash((uint32_t)FlashLevel(), 0);
		if (r->started && a == 0 && b == 0 && c == 0 && d == 0) return TASK_END;
		r->counter++;
		return 0;
	}
}

	void register_mag129_breath()
	{
		register_port(breath129::ORIG_MasterTask, (void *)breath129::MasterTask, "129 MasterTask", 129);
		register_port(breath129::ORIG_MainTask, (void *)breath129::MainTask, "129 MainTask", 129);
		register_port(breath129::ORIG_TargetTask, (void *)breath129::TargetTask, "129 TargetTask", 129);
		register_port(breath129::ORIG_SpriteTask, (void *)breath129::SpriteTask, "129 SpriteTask", 129);
		register_port(breath129::ORIG_BranchTask, (void *)breath129::BranchTask, "129 BranchTask", 129);
		register_port(breath129::ORIG_ArcTask, (void *)breath129::ArcTask, "129 ArcTask", 129);
	}
}

