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

// Effect 317: Acid: Gastric Juice 128

#include "mag_common.h"

namespace ff8fx
{
namespace acid_quistis
{
	using namespace eng;
	using namespace magc;

	inline uint8_t *&Caster() { return var<uint8_t *>(game_addr(game::MAG_317_ACID_QUISTIS_g_Caster)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::MAG_317_ACID_QUISTIS_g_MagicBuffer)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::MAG_317_ACID_QUISTIS_g_CastContext)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_317_ACID_QUISTIS_g_PacketCursor)); }
	inline Mat4x3 &EffectCamera() { return var<Mat4x3>(game_addr(game::MAG_317_ACID_QUISTIS_g_EffectCamera)); }
	inline TaskQueue *QParticles() { return (TaskQueue *)game_addr(game::MAG_317_ACID_QUISTIS_g_ParticleQueue); }
	inline TaskQueue *QStreams() { return (TaskQueue *)game_addr(game::MAG_317_ACID_QUISTIS_g_StreamQueue); }
	inline TaskQueue *QTarget() { return (TaskQueue *)game_addr(game::MAG_317_ACID_QUISTIS_g_TargetQueue); }
	inline uint32_t &FrameCursor() { return var<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)); }
	inline uint32_t &BoneWorkspace() { return var<uint32_t>(game_addr(game::g_BattleVertexWorkspace)); }
	inline uint32_t RenderOT(uint32_t off) { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + off; }

	static constexpr game::Id ORIG_RootTask = game::MAG_317_ACID_QUISTIS_RootTask;
	static constexpr game::Id ORIG_TargetTask = game::MAG_317_ACID_QUISTIS_TargetTask;
	static constexpr game::Id ORIG_SprayTask = game::MAG_317_ACID_QUISTIS_SprayTask;
	static constexpr game::Id ORIG_DripTask = game::MAG_317_ACID_QUISTIS_DripTask;
	static constexpr game::Id ORIG_StreamTask = game::MAG_317_ACID_QUISTIS_StreamTask;
	static constexpr game::Id ORIG_SplashTask = game::MAG_317_ACID_QUISTIS_SplashTask;
	static constexpr game::Id MODEL_Layout = game::MAG_317_ACID_QUISTIS_Layout;
	static constexpr game::Id SEQ_Spray = game::MAG_317_ACID_QUISTIS_SeqSpray;
	static constexpr game::Id SEQ_Drip = game::MAG_317_ACID_QUISTIS_SeqDrip;
	static constexpr game::Id SEQ_Splash = game::MAG_317_ACID_QUISTIS_SeqSplash;
	static constexpr game::Id ZERO_Vector = game::MAG_317_ACID_QUISTIS_g_ZeroVector;
	static const void *SOUND_Juice() { return (const void *)game_addr(game::MAG_317_ACID_QUISTIS_SndAcid); }
	static const uint8_t *DIST_TABLE() { return (const uint8_t *)game_addr(game::MAG_317_ACID_QUISTIS_RibbonLengthTable); }
	static const uint32_t POINTS = 0x1708;
	static const uint32_t MELT = 0x3F08;
	static const uint32_t MORPH = 0x1508;
	static const uint32_t BIND = 0x14E8;
	static constexpr game::Id COM_Anim = game::MAG_317_ACID_QUISTIS_ComAnimSet;
	static constexpr game::Id CASTER_Point = game::MAG_317_ACID_QUISTIS_CasterPoint;

	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t flag, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, flag, out); }
	inline uint32_t BindExternalAnimSet(uint8_t *entity, uint32_t out, uint32_t data) { return fn<uint32_t (__cdecl *)(uint8_t *, uint32_t, uint32_t)>(game_addr(game::BattleEntity_BindExternalAnimSet))(entity, out, data); }
	inline uint32_t QueueChainTransformation(uint8_t *entity, uint32_t id) { return fn<uint32_t (__cdecl *)(uint8_t *, uint32_t)>(game_addr(game::QueueChainTransformation))(entity, id); }
	inline int32_t ComputeCos(int32_t a) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(a); }
	inline int32_t CartesianToGameAngle(int32_t dx, int32_t dz) { return fn<int32_t (__cdecl *)(int32_t, int32_t)>(game_addr(game::CartesianToGameAngle))(dx, dz); }
	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void GteMVMVA_RotV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0))(); }
	inline void GteLoadV0u(const void *v) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadV0_u16))(v); }
	inline void BlendVectors(const int16_t *a, const int16_t *b, int32_t wa, int32_t wb, int16_t *out) { fn<void (__cdecl *)(const int16_t *, const int16_t *, int32_t, int32_t, int16_t *)>(game_addr(game::VecLerp))(a, b, wa, wb, out); }
	inline void MatrixMultiplyVector(const Mat4x3 *m, const int16_t *v, int16_t *out) { fn<void (__cdecl *)(const Mat4x3 *, const int16_t *, int16_t *)>(game_addr(game::matrixMultiplyVector))(m, v, out); }
	inline int32_t BuildOrthonormalBasis(const int32_t *v, Mat4x3 *m) { return fn<int32_t (__cdecl *)(const int32_t *, Mat4x3 *)>(game_addr(game::BuildOrthonormalBasis))(v, m); }
	inline uint32_t DrawShadow(uint8_t *entity, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(uint8_t *, uint32_t, int32_t, uint32_t)>(game_addr(game::BattleEntity_DrawShadow))(entity, ot, mode, cursor); }
	inline uint32_t ModelPolygons(uint8_t *s, uint32_t ot, int32_t shift, uint32_t cursor) { return fn<uint32_t (__cdecl *)(uint8_t *, uint32_t, int32_t, uint32_t)>(game_addr(game::MAG_317_ACID_QUISTIS_BattleModelObjectPolygons))(s, ot, shift, cursor); }

#pragma pack(push, 1)
	struct RootNode
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
		int16_t angle;
		int16_t pos[4];
		int16_t pad1E;
		uint8_t layout[0x248];
	};
	struct ParticleNode
	{
		TaskNode hdr;
		int32_t counter;
		int16_t pos[3];
		int16_t size;
		int16_t vel[3];
		int16_t pad1E;
	};
	struct Point
	{
		int16_t pos[3];
		int16_t width;
		int16_t vel[3];
		int16_t hold;
		int16_t sx, sy;
		int16_t otz;
		int16_t pad16;
		int16_t sx2, sy2;
		int16_t pad1C[2];
		int16_t fade;
		int16_t pad22;
		Point *next;
	};
	struct StreamNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t count;
		Point *head, *tail;
		int16_t from[3];
		int16_t t;
		int16_t to[3];
		int16_t dt;
		Mat4x3 basis;
		int16_t slot;
		int16_t angle;
		int16_t width;
		int16_t pad4E;
	};
	struct PrimArg
	{
		int16_t pos[4];
		uint32_t morph;
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x14 && sizeof(TargetNode) == 0x268 && sizeof(ParticleNode) == 0x20, "Acid nodes");
	static_assert(sizeof(Point) == 0x28 && sizeof(StreamNode) == 0x50 && sizeof(PrimArg) == 0xC, "Acid nodes");

	template<typename T> static inline T &F(uint8_t *p, uint32_t off) { return *(T *)(p + off); }
	inline Point *Points() { return (Point *)(TexBase() + POINTS); }
	inline uint8_t BoneCount(const uint8_t *entity) { return **(uint8_t *const *)*(uint8_t *const *)(entity + 0x64); }

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
		if (r->flags & 0x1000)
		{
			GteSetRotMatrixCtrl(&Camera());
			GteLoadV0(arg->pos);
			GteMVMVA_RotV0();
			GteReadMAC123(m.t);
			GteSetRotMatrixCtrl(&EffectCamera());
			GteLoadV0(off);
			GteMVMVA_RotV0();
			int32_t w[3];
			GteReadMAC123(w);
			m.t[0] = (int32_t)((uint32_t)m.t[0] + (uint32_t)w[0]);
			m.t[1] = (int32_t)((uint32_t)m.t[1] + (uint32_t)w[1]);
			m.t[2] = (int32_t)((uint32_t)m.t[2] + (uint32_t)w[2]);
			MatrixMultiply(&EffectCamera(), &m);
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
		PacketCursor() = RenderPrimModel(h, RenderOT(0x44), 2, PacketCursor());
		FieldFree(0x58);
	}

	static void CasterPoint(const int16_t *v, int16_t *out)
	{
		uint8_t *caster = Caster();
		const uint8_t *bones = **(uint8_t *const *const *)(caster + 0x64) + 0x10;
		BuildBoneMatricesFromPose(caster + 0x60);
		Mat4x3 m;
		ComposeAffineTransform((const Mat4x3 *)(caster + 0x40), (const Mat4x3 *)(bones + (uint32_t)mul32(v[3], 0x30) + 0x10), &m);
		GteSetRotMatrixCtrl(&m);
		GteSetTransVectorCtrl(&m);
		GteLoadV0(v);
		GteMVMVA_RotV0Tr();
		GteStoreIR123(out);
	}

	static void MeltGroup(uint8_t *h)
	{
		const uint8_t *v = F<const uint8_t *>(h, 4);
		uint8_t *out = F<uint8_t *>(h, 8);
		const int32_t n = *(const int16_t *)v;
		v += 2;
		if (n != 0)
		{
			uint32_t left = (uint32_t)n;
			do
			{
				GteLoadV0u(v);
				GteMVMVA_RotV0Tr();
				v += 6;
				int32_t y;
				GteReadDataRegTo(&y, 10);
				const int32_t r = CrtRand();
				*out = (uint8_t)(0 - (uint8_t)(mul32((r & 0x1F) + 0x10, y) >> 10));
				out++;
			} while (--left != 0);
		}
		F<uint8_t *>(h, 8) = out;
		F<const uint8_t *>(h, 4) = v;
	}

	static void MeltModel(uint8_t *h, uint8_t *anim)
	{
		const uint8_t *model = *(const uint8_t *const *)(anim + 4);
		const uint8_t *bones = *(const uint8_t *const *)model + 0x10;
		const uint32_t *offs = (const uint32_t *)(*(const uint8_t *const *)(model + 4));
		int32_t count = *(const int32_t *)offs;
		offs++;
		if (count <= 0) return;
		do
		{
			const uint8_t *obj = *(const uint8_t *const *)(model + 4) + *offs++;
			const int32_t groups = *(const int16_t *)obj;
			F<const uint8_t *>(h, 4) = obj + 2;
			if (groups != 0)
			{
				uint32_t left = (uint32_t)groups;
				do
				{
					const uint8_t *p = F<const uint8_t *>(h, 4);
					const int32_t bone = *(const int16_t *)p;
					F<const uint8_t *>(h, 4) = p + 2;
					const Mat4x3 *m = (const Mat4x3 *)(bones + (uint32_t)mul32(bone, 0x30) + 0x10);
					GteSetRotMatrix(m);
					GteSetTransVector(m);
					MeltGroup(h);
				} while (--left != 0);
			}
		} while (--count != 0);
	}

	static void InitMelt(uint8_t *entity)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0xC);
		F<uint32_t>(h, 8) = TexBase() + MELT;
		ComputeBonesWorldMatrices(entity + 0x60, entity + 0x40);
		MeltModel(h, entity + 0x60);
		BuildBoneMatricesFromPose(entity + 0x60);
		uint8_t *second = *(uint8_t *const *)(entity + 0x78);
		if (second)
		{
			ComputeBonesWorldMatrices(second, entity + 0x40);
			MeltModel(h, second);
			BuildBoneMatricesFromPose(second);
		}
		FieldFree(0xC);
	}

	static void WorldVertices(uint8_t *s)
	{
		const uint8_t *v = F<const uint8_t *>(s, 0x10);
		uint8_t *out = F<uint8_t *>(s, 8);
		const int32_t n = *(const int16_t *)v;
		v += 2;
		if (n == 0) return;
		uint32_t left = (uint32_t)n;
		do
		{
			GteLoadV0u(v);
			GteMVMVA_RotV0Tr();
			v += 6;
			GteStoreIR123(out);
			out += 8;
		} while (--left != 0);
	}

	static void MeltVertices(uint8_t *s)
	{
		const uint8_t *v = F<const uint8_t *>(s, 0x10);
		uint8_t *out = F<uint8_t *>(s, 8);
		const int32_t n = *(const int16_t *)v;
		v += 2;
		if (n == 0)
		{
			F<uint8_t *>(s, 8) = out;
			F<const uint8_t *>(s, 0x10) = v;
			return;
		}
		const uint8_t *end = v + (uint32_t)mul32(n, 6);
		uint32_t left = (uint32_t)n;
		do
		{
			const uint8_t *melt = F<const uint8_t *>(s, 0xC);
			*(int16_t *)(out + 2) = (int16_t)(*(const int16_t *)(out + 2) + (int16_t)(mul32(*melt, F<int16_t>(s, 0x60)) >> 10));
			F<const uint8_t *>(s, 0xC) = melt + 1;
			GteLoadV0(out);
			GteRTPS();
			GteReadSXY2(s + 0x30);
			GteReadOTZ(s + 0x34);
			if (F<uint16_t>(s, 0x34) == 0)
				*(uint16_t *)(out + 6) = 0x10;
			else
			{
				const int32_t sx = F<int16_t>(s, 0x30);
				if (sx < shl32(F<int16_t>(s, 0x20), 3)) s[0x36] |= 1;
				else if (sx >= shl32(F<int16_t>(s, 0x24), 3)) s[0x36] |= 2;
				const int32_t sy = F<int16_t>(s, 0x32);
				if (sy < shl32(F<int16_t>(s, 0x22), 3)) s[0x36] |= 4;
				else if (sy >= shl32(F<int16_t>(s, 0x26), 3)) s[0x36] |= 8;
				*(uint32_t *)out = F<uint32_t>(s, 0x30);
				*(uint32_t *)(out + 4) = F<uint32_t>(s, 0x34);
			}
			out += 8;
		} while (--left != 0);
		F<uint8_t *>(s, 8) = out;
		F<const uint8_t *>(s, 0x10) = end;
	}

	static uint32_t MeltModelDraw(const uint8_t *model, uint8_t *s, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		const uint8_t *bones = *(const uint8_t *const *)model + 0x10;
		const uint32_t *offs = (const uint32_t *)(*(const uint8_t *const *)(model + 4));
		const int32_t count = *(const int32_t *)offs;
		offs++;
		if (count <= 0) return cursor;
		for (int32_t k = 0; k < count; k++)
		{
			F<const uint8_t *>(s, 0x10) = *(const uint8_t *const *)(model + 4) + offs[k];
			if (!(F<uint32_t>(s, 0x2C) & (1u << (k & 31)))) continue;
			F<uint32_t>(s, 8) = F<uint32_t>(s, 4);
			const uint8_t *obj = F<const uint8_t *>(s, 0x10);
			int32_t groups = *(const int16_t *)obj;
			F<const uint8_t *>(s, 0x10) = obj + 2;
			if (groups > 0)
			{
				do
				{
					const uint8_t *p = F<const uint8_t *>(s, 0x10);
					const int32_t bone = *(const int16_t *)p;
					F<const uint8_t *>(s, 0x10) = p + 2;
					const Mat4x3 *m = (const Mat4x3 *)(bones + (uint32_t)mul32(bone, 0x30) + 0x10);
					GteSetRotMatrixCtrl(m);
					GteSetTransVectorCtrl(m);
					WorldVertices(s);
					GteSetRotMatrixCtrl(&Camera());
					GteSetTransVectorCtrl(&Camera());
					MeltVertices(s);
				} while (--groups != 0);
			}
			const uint8_t *p = (const uint8_t *)(((uint32_t)F<const uint8_t *>(s, 0x10) + 3) & ~3u);
			for (int w = 0; w < 6; w++)
			{
				F<uint16_t>(s, 0x14 + 2 * w) = *(const uint16_t *)p;
				p += 2;
			}
			F<const uint8_t *>(s, 0x10) = p;
			F<const uint8_t *>(s, 0) = p;
			cursor = ModelPolygons(s, ot, shift, cursor);
		}
		return cursor;
	}

	static void DrawMeltingTarget(uint8_t *entity, int32_t sinv)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x84);
		F<int16_t>(h, 0x80) = (int16_t)sinv;
		F<uint32_t>(h, 0x2C) = TexBase() + MELT;
		FrameCursor() = DrawShadow(entity, RenderOT(0x4040), 0x10, FrameCursor());
		ComputeBonesWorldMatrices(entity + 0x60, entity + 0x40);
		F<int16_t>(h, 0x44) = 0x140;
		F<uint32_t>(h, 0x24) = BoneWorkspace();
		F<int16_t>(h, 0x40) = 0;
		F<int16_t>(h, 0x42) = 0;
		F<int16_t>(h, 0x46) = 0xD8;
		F<uint32_t>(h, 0x48) = *(const uint32_t *)(entity + 0x28);
		F<uint32_t>(h, 0x4C) = *(const uint32_t *)(entity + 0x7C);
		FrameCursor() = MeltModelDraw(*(const uint8_t *const *)(entity + 0x64), h + 0x20, RenderOT(0x44), 0, FrameCursor());
		BuildBoneMatricesFromPose(entity + 0x60);
		uint8_t *second = *(uint8_t *const *)(entity + 0x78);
		if (second)
		{
			F<uint32_t>(h, 0x4C) = 0xFFFFFFFF;
			ComputeBonesWorldMatrices(second, entity + 0x40);
			FrameCursor() = MeltModelDraw(*(const uint8_t *const *)(second + 4), h + 0x20, RenderOT(0x44), 0, FrameCursor());
			BuildBoneMatricesFromPose(second);
		}
		FieldFree(0x84);
	}

	static int32_t MeltSin(int32_t e)
	{
		return ComputeSin((int32_t)(uint32_t)(((uint64_t)((uint32_t)e << 11) * 0xCCCCCCCDu) >> 37));
	}

	static void ParticleDraw(const ParticleNode *p, uint32_t seq)
	{
		TransformCameraByShadowRotation(p->pos, p->size, seq == game_addr(SEQ_Spray) ? 0 : -((int32_t)p->size >> 2));
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		F<uint32_t>(h, 0) = seq;
		F<int16_t>(h, 4) = (int16_t)p->counter;
		F<int16_t>(h, 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(0x44), 2, PacketCursor());
		FieldFree(0xB4);
	}
	static void SprayStep(ParticleNode *p)
	{
		p->pos[1] = (int16_t)(p->pos[1] + p->vel[1]);
		p->pos[0] = (int16_t)(p->pos[0] + p->vel[0]);
		p->pos[2] = (int16_t)(p->pos[2] + p->vel[2]);
		p->vel[1] = (int16_t)(p->vel[1] + 0x10);
	}
	static void DripStep(ParticleNode *p)
	{
		p->pos[0] = (int16_t)(p->pos[0] + p->vel[0]);
		p->pos[2] = (int16_t)(p->pos[2] + p->vel[2]);
		p->vel[1]++;
		p->pos[1] = (int16_t)(p->pos[1] + p->vel[1]);
	}

	static Point *AllocPoint(const int16_t *pos, int16_t width)
	{
		Point *p = Points();
		int n = 0x100;
		while ((uint16_t)p->width != 0xFFFF)
		{
			p++;
			if (--n == 0) return nullptr;
		}
		ff8fx::copy<(8)>(p->pos, pos);
		p->width = width;
		p->hold = 0xA;
		p->fade = 0x800;
		p->next = nullptr;
		return p;
	}

	static Point *SpawnStream(const int16_t *from, const int16_t *to, int32_t slot, int32_t t, int32_t dt, int32_t width, int32_t angle)
	{
		Point *p = AllocPoint(from, (int16_t)width);
		if (!p) return nullptr;
		StreamNode *s = (StreamNode *)AddTaskToQueue(QStreams(), game_addr(ORIG_StreamTask));
		if (!s)
		{
			p->width = -1;
			return nullptr;
		}
		Memset32(&s->counter, 0, 0x11);
		s->counter = 0;
		ff8fx::copy<(8)>(s->from, from);
		ff8fx::copy<(8)>(s->to, to);
		s->t = (int16_t)t;
		s->dt = (int16_t)dt;
		s->angle = (int16_t)angle;
		s->tail = p;
		s->head = p;
		s->count = 1;
		s->slot = (int16_t)slot;
		s->width = (int16_t)width;
		const int32_t d[3] = { (int32_t)to[0] - from[0], (int32_t)to[1] - from[1], (int32_t)to[2] - from[2] };
		BuildOrthonormalBasis(d, &s->basis);
		return p;
	}

	static void RibbonEdges(Point *p, const Point *n, const int16_t *prev, int16_t *dir)
	{
		if (p->otz < 0) return;
		int32_t ox = 0, oy = 0;
		if (n)
		{
			if (n->otz < 0) return;
			const int32_t dx = ScreenPx(n->sx) - ScreenPx(p->sx);
			const int32_t ndy = ScreenPx(p->sy) - ScreenPx(n->sy);
			int32_t adx = dx < 0 ? -dx : dx, ady = ndy < 0 ? -ndy : ndy;
			if (adx >= 0x80) adx = 0x7F;
			if (ady >= 0x80) ady = 0x7F;
			const int32_t len = DIST_TABLE()[ady * 128 + adx];
			int32_t px, py;
			if (prev)
			{
				if (len == 0)
				{
					dir[0] = prev[0];
					dir[1] = prev[1];
					px = prev[0];
					py = prev[1];
				}
				else
				{
					const int32_t cx = shl32(ndy, 12) / len, cy = shl32(dx, 12) / len;
					int32_t sx = cx + prev[0], sy = cy + prev[1];
					dir[0] = (int16_t)cx;
					dir[1] = (int16_t)cy;
					if (sx == 0 && sy == 0)
					{
						sx = cx * 2;
						sy = cy * 2;
					}
					const int32_t sq = (int32_t)((uint32_t)mul32(sx, sx) + (uint32_t)mul32(sy, sy));
					const int32_t k = 0x4000 - (sq >> 13);
					px = mul32(k, sx) >> 13;
					py = mul32(k, sy) >> 13;
				}
			}
			else
			{
				if (len == 0)
				{
					p->otz = -1;
					return;
				}
				px = shl32(ndy, 12) / len;
				py = shl32(dx, 12) / len;
				dir[0] = (int16_t)px;
				dir[1] = (int16_t)py;
			}
			const int32_t w = shl32(n->width, 5) / (p->otz + 0x200);
			ox = ScreenEighths(mul32(w, px) >> 12);
			oy = ScreenEighths(mul32(w, py) >> 12);
		}
		const int16_t x = p->sx, y = p->sy;
		p->sx = (int16_t)(x - ox);
		p->sx2 = (int16_t)(x + ox);
		p->sy2 = (int16_t)(y + oy);
		p->sy = (int16_t)(y - oy);
	}

	static const uint32_t RIBBON_DIR0 = 0x43300000;
	static void DrawRibbon(Point *head, int32_t count)
	{
		int16_t dir[2] = { (int16_t)(RIBBON_DIR0 & 0xFFFF), (int16_t)(RIBBON_DIR0 >> 16) };
		RibbonEdges(head, head->next, nullptr, dir);
		Point *p = head->next;
		for (int32_t i = count - 2; i != 0; i--)
		{
			RibbonEdges(p, p->next, dir, dir);
			p = p->next;
		}
		RibbonEdges(p, nullptr, dir, nullptr);
		Point *a = head;
		for (int32_t i = count - 1; i != 0; i--)
		{
			Point *b = a->next;
			if (a->otz > 0 && b->otz > 0)
			{
				uint32_t *pk = (uint32_t *)PacketCursor();
				PacketCursor() += 0x34;
				pk[2] = *(const uint32_t *)&a->sx;
				pk[5] = *(const uint32_t *)&a->sx2;
				pk[3] = 0x3D14E048;
				pk[6] = 0x00BAE078;
				*(uint16_t *)((uint8_t *)pk + 0x24) = 0xE048;
				*(uint16_t *)((uint8_t *)pk + 0x30) = 0xE078;
				pk[8] = *(const uint32_t *)&b->sx;
				pk[11] = *(const uint32_t *)&b->sx2;
				uint32_t ca = (uint32_t)(int32_t)(int16_t)(a->fade >> 4);
				ca = (((ca << 8) | ca) << 8) | ca;
				pk[4] = ca;
				pk[1] = ca;
				uint32_t cb = (uint32_t)(int32_t)(int16_t)(b->fade >> 4);
				cb = (((cb << 8) | cb) << 8) | cb;
				pk[0] = 0x0C000000;
				((uint8_t *)pk)[7] = 0x3E;
				pk[10] = cb;
				pk[7] = cb;
				InsertPrimAutoDepth(RenderOT(0x44) + 4 * ((a->otz + b->otz) >> 3), pk);
			}
			a = b;
		}
	}

	static void StreamDraw(Point *head, int32_t count)
	{
		GteSetRotMatrix(&Camera());
		GteSetTransVector(&Camera());
		Point *p = head;
		do
		{
			GteLoadV0(p);
			GteRTPS();
			GteReadSXY2(&p->sx);
			GteReadOTZ(&p->otz);
			p = p->next;
		} while (p);
		if (count >= 2) DrawRibbon(head, count);
	}

	static inline int16_t FadeStep(int16_t f)
	{
		if (f <= 0) return f;
		f = (int16_t)(f + (f > 0x400 ? -0x100 : -0x80));
		return f > 0 ? f : 0;
	}
}
}

namespace ff8fx
{
namespace acid_quistis
{
	static uint32_t __cdecl SprayTask(TaskNode *n)
	{
		ParticleNode *p = (ParticleNode *)n;
		ParticleDraw(p, game_addr(SEQ_Spray));
		SprayStep(p);
		p->counter++;
		return p->counter < 9 ? 0 : TASK_END;
	}

	static uint32_t __cdecl DripTask(TaskNode *n)
	{
		ParticleNode *p = (ParticleNode *)n;
		ParticleDraw(p, game_addr(SEQ_Drip));
		DripStep(p);
		p->counter++;
		return p->counter < 0xD ? 0 : TASK_END;
	}

	static uint32_t __cdecl SplashTask(TaskNode *n)
	{
		ParticleNode *p = (ParticleNode *)n;
		ParticleDraw(p, game_addr(SEQ_Splash));
		p->counter++;
		return p->counter < 0x10 ? 0 : TASK_END;
	}

	static uint32_t __cdecl StreamTask(TaskNode *n)
	{
		StreamNode *s = (StreamNode *)n;
		for (int iter = 2; iter != 0; iter--)
		{
			if (s->t < 0x1000)
			{
				s->t = (int16_t)(s->dt + s->t);
				int16_t at[4] = { 0, 0, 0, 0 };
				BlendVectors(s->from, s->to, 0x1000 - s->t, s->t, at);
				const int32_t sn = ComputeSin(s->t >> 1);
				at[1] = (int16_t)(at[1] - (int16_t)((int16_t)sn >> 2));
				const int32_t radius = (int16_t)((int16_t)sn >> 4);
				int16_t v[4];
				v[0] = (int16_t)(mul32(ComputeSin(s->angle), radius) >> 12);
				v[1] = (int16_t)(mul32(ComputeCos(s->angle), radius) >> 12);
				v[2] = 0;
				v[3] = 0;
				MatrixMultiplyVector(&s->basis, v, v);
				s->angle = (int16_t)(s->angle + 0x100);
				at[0] = (int16_t)(at[0] + v[0]);
				at[1] = (int16_t)(at[1] + v[1]);
				at[2] = (int16_t)(at[2] + v[2]);
				if (s->t >= 0x1000 && s->slot >= 0)
				{
					ParticleNode *q = (ParticleNode *)AddTaskToQueue(QParticles(), game_addr(ORIG_SplashTask));
					if (q)
					{
						q->counter = 0;
						ff8fx::copy<(8)>(q->pos, at);
						q->size = (int16_t)(s->width << 4);
					}
				}
				Point *p = AllocPoint(at, s->width);
				if (!p) s->t = 0x1000;
				else
				{
					s->count++;
					s->tail->next = p;
					s->tail = p;
					p->vel[0] = (int16_t)(v[0] >> 3);
					p->vel[1] = (int16_t)(v[1] >> 3);
					p->vel[2] = (int16_t)(v[2] >> 3);
				}
			}
			Point *q = s->head;
			do
			{
				if (q->fade > 0) q->fade = FadeStep(q->fade);
				q = q->next;
			} while (q);
		}
		StreamDraw(s->head, s->count);
		Point *q = s->head;
		do
		{
			q->pos[0] = (int16_t)(q->pos[0] + q->vel[0]);
			q->pos[1] = (int16_t)(q->pos[1] + q->vel[1]);
			q->pos[2] = (int16_t)(q->pos[2] + q->vel[2]);
			q = q->next;
		} while (q);
		Point *h = s->head;
		if (h->fade != 0) return 0;
		for (;;)
		{
			Point *nx = h->next;
			if (nx->fade != 0) return 0;
			h->width = -1;
			s->count--;
			s->head = nx;
			if (s->count < 2) return TASK_END;
			h = nx;
			if (h->fade != 0) return 0;
		}
	}

	static uint32_t __cdecl TargetTask(TaskNode *n)
	{
		TargetNode *t = (TargetNode *)n;
		if (t->counter >= 0x19)
		{
			PrimArg arg;
			ff8fx::copy<(8)>(arg.pos, t->pos);
			arg.morph = TexBase() + MORPH;
			prim::play((prim::Layout *)t->layout, PartCallback, (int)&arg, 0);
		}
		const uint32_t e = (uint32_t)((int32_t)t->counter - 0x21);
		if (e < 0x28)
		{
			t->entity[0] |= 4;
			const int32_t sinv = MeltSin((int32_t)e);
			DrawMeltingTarget(t->entity, sinv);
		}
		else *(uint16_t *)t->entity &= 0xFFFB;
		if (t->counter == 0xA) BdPlaySE(SOUND_Juice(), 0, 0x80);
		if (t->counter == 0x12)
		{
			for (int32_t w = 0xFFF; w != 0; w -= 0x555)
			{
				int16_t from[4], anchor[4], to[4];
				CasterPoint((const int16_t *)game_addr(CASTER_Point), from);
				GetDefaultEffectPosition(Caster(), anchor);
				uint8_t *entity = t->entity;
				const int32_t r = CrtRand();
				GetEffectSpawnPosition(entity, mul32(r, BoneCount(entity)) >> 15, 0, to);
				const int32_t slot = (int32_t)((uint32_t)t->entity - game_addr(game::BattleEntitySlotData)) / 0x9C;
				Point *p = SpawnStream(from, to, slot, 0, 0xA0, 0x96, w);
				if (p)
				{
					*(uint32_t *)p->vel = var<uint32_t>(game_addr(ZERO_Vector));
					*(uint32_t *)&p->vel[2] = var<uint32_t>(game_addr(ZERO_Vector) + 4);
				}
			}
		}
		if ((uint32_t)((int32_t)t->counter - 0x12) < 2)
		{
			for (int k = 8; k != 0; k--)
			{
				ParticleNode *q = (ParticleNode *)AddTaskToQueue(QParticles(), game_addr(ORIG_SprayTask));
				if (!q) continue;
				CasterPoint((const int16_t *)game_addr(CASTER_Point), q->pos);
				q->counter = 0;
				q->size = (int16_t)((CrtRand() & 0x3FF) + 0x400);
				const int16_t a = (int16_t)((uint32_t)(CrtRand() & 0x3FF) + (uint16_t)t->angle - 0x200);
				const int32_t speed = (int16_t)((CrtRand() & 0x3F) + 0x40);
				q->vel[0] = (int16_t)(mul32(ComputeSin(a), speed) >> 12);
				q->vel[2] = (int16_t)(mul32(ComputeCos(a), speed) >> 12);
				q->vel[1] = (int16_t)(-0x20 - (CrtRand() & 0x3F));
			}
		}
		const uint32_t d = (uint32_t)((int32_t)t->counter - 0x21);
		if (d < 0x14 && !(d & 1))
		{
			ParticleNode *q = (ParticleNode *)AddTaskToQueue(QParticles(), game_addr(ORIG_DripTask));
			if (q)
			{
				uint8_t *entity = t->entity;
				const int32_t r = CrtRand();
				GetEffectSpawnPosition(entity, mul32(r, BoneCount(entity)) >> 15, 0, q->pos);
				q->counter = 0;
				q->size = (int16_t)((CrtRand() & 0x7FF) + 0x800);
				if (d >= 0xA) q->size = (int16_t)(uint32_t)(((uint64_t)(uint32_t)mul32(0x1E - (int32_t)d, q->size) * 0xCCCCCCCDu) >> 36);
				const int16_t a = (int16_t)((uint32_t)(CrtRand() & 0x3FF) + (uint16_t)t->angle - 0x200);
				const int32_t speed = (int16_t)(CrtRand() & 0x1F);
				q->vel[0] = (int16_t)(mul32(ComputeSin(a), speed) >> 12);
				q->vel[2] = (int16_t)(mul32(ComputeCos(a), speed) >> 12);
				q->vel[1] = (int16_t)(-0x10 - (CrtRand() & 0x1F));
			}
		}
		if (t->counter == 0x39) ApplyActionResultToTarget(Ctx()->actions[t->action].targets);
		if (t->counter >= 0x49) return TASK_END;
		t->counter++;
		return 0;
	}

	static uint32_t __cdecl RootTask(TaskNode *n)
	{
		RootNode *r = (RootNode *)n;
		if (r->arena)
		{
			PacketCursor() = TexBase() + 0x4708;
			r->arena = 0;
		}
		else
		{
			PacketCursor() = TexBase() + 0x14708;
			r->arena = 1;
		}
		if (r->counter == 1)
		{
			if (!r->started)
			{
				r->started = 1;
				InitTaskQueuePool(QTarget(), (void *)(TexBase() + 0x1280), 0x268, 1);
				InitTaskQueuePool(QStreams(), (void *)(TexBase() + 0x1000), 0x50, 8);
				InitTaskQueuePool(QParticles(), (void *)TexBase(), 0x20, 0x80);
				Point *p = Points();
				for (int i = 0; i < 0x100; i++) p[i].width = -1;
			}
			if (r->action <= Ctx()->actions[0].last_action)
			{
				TargetNode *t = (TargetNode *)AddTaskToQueue(QTarget(), game_addr(ORIG_TargetTask));
				Memset32(&t->counter, 0, 0x97);
				t->action = (int16_t)(uint16_t)r->action;
				uint8_t *entity = Entity(Ctx()->actions[t->action].targets[0]);
				t->entity = entity;
				GetDefaultEffectPosition(entity, t->pos);
				int16_t caster[4];
				GetDefaultEffectPosition(Caster(), caster);
				t->angle = (int16_t)CartesianToGameAngle((int32_t)t->pos[0] - caster[0], (int32_t)t->pos[2] - caster[2]);
				DecodeModelPrimLayout(game_addr(MODEL_Layout), t->layout, 0x248);
				r->action++;
				InitMelt(t->entity);
			}
			BindExternalAnimSet(Caster(), TexBase() + BIND, game_addr(COM_Anim));
			QueueChainTransformation(Caster(), 3);
		}
		int a, b, c;
		if (r->started)
		{
			EffectCameraMatrix(&Camera(), &EffectCamera());
			a = ExecuteTaskQueue(QTarget());
			b = ExecuteTaskQueue(QStreams());
			c = ExecuteTaskQueue(QParticles());
		}
		else a = b = c = (int)n;
		if (r->started && a == 0 && b == 0 && c == 0) return TASK_END;
		r->counter++;
		return 0;
	}
}

	void register_mag317_acid_quistis()
	{
		register_port(acid_quistis::ORIG_RootTask, (void *)acid_quistis::RootTask, "317 RootTask", 317);
		register_port(acid_quistis::ORIG_TargetTask, (void *)acid_quistis::TargetTask, "317 TargetTask", 317);
		register_port(acid_quistis::ORIG_StreamTask, (void *)acid_quistis::StreamTask, "317 StreamTask", 317);
		register_port(acid_quistis::ORIG_SprayTask, (void *)acid_quistis::SprayTask, "317 SprayTask", 317);
		register_port(acid_quistis::ORIG_DripTask, (void *)acid_quistis::DripTask, "317 DripTask", 317);
		register_port(acid_quistis::ORIG_SplashTask, (void *)acid_quistis::SplashTask, "317 SplashTask", 317);
	}
}

