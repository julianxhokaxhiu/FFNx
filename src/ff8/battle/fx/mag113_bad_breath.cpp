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

// Effect 113: Bad Breath

#include "mag_common.h"

namespace ff8fx
{
namespace badbreath113
{
	using namespace eng;
	using namespace magc;

	inline uint8_t *&Caster() { return var<uint8_t *>(game_addr(game::g_BadBreath_CasterEntity)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::g_BadBreath_MagicBuffer)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::g_BadBreath_CastContext)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::g_BadBreath_PacketCursor)); }
	inline TaskQueue *QTargets() { return (TaskQueue *)game_addr(game::g_BadBreath_TargetQueue); }
	inline TaskQueue *QParticles() { return (TaskQueue *)game_addr(game::g_BadBreath_ParticleQueue); }
	inline TaskQueue *QMain() { return (TaskQueue *)game_addr(game::g_BadBreath_MainQueue); }
	inline uint32_t &FrameCursor() { return var<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)); }
	inline uint32_t &BoneWorkspace() { return var<uint32_t>(game_addr(game::g_BattleVertexWorkspace)); }
	inline uint32_t RenderOT(uint32_t off) { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + off; }

	static constexpr game::Id ORIG_RootTask = game::MAG_113_BadBreath_RootTask;
	static constexpr game::Id ORIG_MainTask = game::MAG_113_BadBreath_MainTask;
	static constexpr game::Id ORIG_ParticleTask = game::MAG_113_BadBreath_ParticleTask;
	static constexpr game::Id ORIG_TargetTask = game::MAG_113_BadBreath_TargetTask;
	static constexpr game::Id MODEL_Cloud = game::g_BadBreath_CloudLayout;
	static constexpr game::Id PART_Flags = game::g_BadBreath_PartFlags;
	static constexpr game::Id SEQ_Stream = game::unk_12C118C;
	static constexpr game::Id SEQ_Spray = game::unk_12C1418;
	static constexpr game::Id SEQ_Rise = game::unk_12C1014;
	static constexpr game::Id ZERO_Vector = game::g_BadBreath_ZeroVector;
	static constexpr game::Id WAVE_Table = game::g_BadBreath_WaveTable;
	static const void *SOUND_BadBreath() { return (const void *)game_addr(game::g_BadBreath_Sound); }

	inline int32_t ComputeCos(int32_t angle) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(angle); }
	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t flag, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, flag, out); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
	inline uint32_t DrawShadow(uint8_t *entity, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(uint8_t *, uint32_t, int32_t, uint32_t)>(game_addr(game::BattleEntity_DrawShadow))(entity, ot, mode, cursor); }
	inline void GteLoadV0u(const void *v) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadV0_u16))(v); }
	inline void GteSetSXY012(uint32_t a, uint32_t b, uint32_t c) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_SetSXY012))(a, b, c); }
	inline void GteSetSZ123(uint32_t a, uint32_t b, uint32_t c) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_SetSZ123))(a, b, c); }
	inline void GteSetSZ0123(uint32_t a, uint32_t b, uint32_t c, uint32_t d) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_SetSZ0123))(a, b, c, d); }
	inline void GteNCLIP() { fn<void (__cdecl *)()>(game_addr(game::GTE_NCLIP))(); }
	inline void GteReadMAC0(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadMAC0))(dst); }
	inline void GteAVSZ3() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ3))(); }
	inline void GteAVSZ4() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ4))(); }
	inline void GteReadOTZWord(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadOTZ))(dst); }

#pragma pack(push, 1)
	struct RootNode
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
		uint8_t mask;
		uint8_t slot;
		int16_t pos[3];
		int16_t phase;
		uint8_t pad18[0xEC];
	};
	struct ParticleNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t end;
		uint32_t seq;
		int16_t pos[3];
		int16_t scale;
		int16_t vel[3];
		int16_t rot;
		int16_t acc[3];
		int16_t pad2A;
	};
	struct TargetNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t slot;
		uint8_t cloud[0xEC];
	};
	struct PrimArg
	{
		int16_t pos[4];
		uint32_t flags;
		uint32_t morph;
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x14 && sizeof(MainNode) == 0x104 && sizeof(ParticleNode) == 0x2C && sizeof(TargetNode) == 0xFC, "Bad Breath nodes");
	static_assert(sizeof(PrimArg) == 0x10, "Bad Breath prim block");

	template<typename T> static inline T &F(uint8_t *p, uint32_t off) { return *(T *)(p + off); }

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
		m.t[0] = (int32_t)r->pos[0] + arg->pos[0];
		m.t[1] = (int32_t)r->pos[1] + arg->pos[1];
		m.t[2] = (int32_t)r->pos[2] + arg->pos[2];
		const uint32_t *flags = (const uint32_t *)arg->flags;
		if (flags[(int16_t)r->index] & 0x10000000)
		{
			TransformVectorBy3x3Matrix(&Camera(), m.t, m.t);
			m.t[1] = (int32_t)((uint32_t)m.t[1] + (uint32_t)Camera().t[1]);
			m.t[0] = (int32_t)((uint32_t)m.t[0] + (uint32_t)Camera().t[0]);
			m.t[2] = (int32_t)((uint32_t)m.t[2] + (uint32_t)Camera().t[2]);
		}
		else ComposeAffineTransform(&Camera(), &m, &m);
		if (!(*(const uint32_t *)r->scale == 0x10001000 && r->scale[2] == 0x1000))
		{
			int32_t v[3] = { r->scale[0], r->scale[1], r->scale[2] };
			Scale3DMatrix(&m, v);
		}
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		const uint32_t mode = (flags[(int16_t)r->index] & 0xDFFF) | 0x2000;
		*(uint32_t *)(h + 0x1C) = mode;
		*(int32_t *)(h + 0xC) = r->a;
		if (r->a != 0)
		{
			*(uint32_t *)(h + 0x1C) = mode | 0xC0;
			*(uint32_t *)(h + 8) = *(const uint32_t *)r->rgb;
		}
		PacketCursor() = RenderPrimModel(h, RenderOT(0x44), 2, PacketCursor());
		FieldFree(0x58);
	}

	static void SwayVertices(uint8_t **objp, uint8_t **outp, uint8_t *s)
	{
		uint8_t *p = *objp;
		uint8_t *out = *outp;
		const int32_t n = *(const int16_t *)p;
		p += 2;
		if (n == 0)
		{
			*objp = p;
			*outp = out;
			return;
		}
		uint32_t left = (uint32_t)n;
		do
		{
			GteLoadV0u(p);
			GteRTPS();
			GteReadSXY2(s + 0x24);
			p += 6;
			GteReadOTZ(s + 0x28);
			if (F<uint16_t>(s, 0x28) == 0)
				*(uint16_t *)(out + 6) = 0x10;
			else
			{
				GteStoreIR123(s + 0x2C);
				const uint32_t idx = ((uint32_t)F<uint16_t>(s, 0x64) + (uint32_t)F<uint16_t>(s, 0x2E) * 4) & 0xFFF;
				const int16_t sx = F<int16_t>(s, 0x24);
				int32_t d = mul32(*(const int16_t *)(game_addr(WAVE_Table) + idx * 2), F<int16_t>(s, 0x66)) >> 12;
				d = mul32(d, (int32_t)sx - (int32_t)F<int16_t>(s, 0x5C)) >> 11;
				const int16_t nx = (int16_t)(sx + d);
				F<int16_t>(s, 0x24) = nx;
				if ((int32_t)nx < shl32(F<int16_t>(s, 0x14), 3)) s[0x2A] |= 1;
				else if ((int32_t)nx >= shl32(F<int16_t>(s, 0x18), 3)) s[0x2A] |= 2;
				const int32_t sy = F<int16_t>(s, 0x26);
				if (sy < shl32(F<int16_t>(s, 0x16), 3)) s[0x2A] |= 4;
				else if (sy >= shl32(F<int16_t>(s, 0x1A), 3)) s[0x2A] |= 8;
				*(uint32_t *)out = F<uint32_t>(s, 0x24);
				*(uint32_t *)(out + 4) = F<uint32_t>(s, 0x28);
			}
			out += 8;
		} while (--left != 0);
		*objp = p;
		*outp = out;
	}

	static inline void CopyVertex(uint8_t *s, uint32_t off, const uint8_t *verts, uint16_t index)
	{
		const uint8_t *v = verts + (uint32_t)(index & 0xFFF) * 8;
		F<uint32_t>(s, off) = *(const uint32_t *)v;
		F<uint32_t>(s, off + 4) = *(const uint32_t *)(v + 4);
	}

	static uint32_t FlatPolygons(uint8_t *s, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		uint8_t *pk = (uint8_t *)cursor;
		const uint8_t *verts = F<uint8_t *>(s, 4);
		const uint8_t *p = F<uint8_t *>(s, 0);
		uint16_t k = F<uint16_t>(s, 8);
		F<uint16_t>(s, 8) = (uint16_t)(k - 1);
		if (k != 0)
		{
			do
			{
				CopyVertex(s, 0x24, verts, *(const uint16_t *)p);
				CopyVertex(s, 0x2C, verts, *(const uint16_t *)(p + 2));
				CopyVertex(s, 0x34, verts, *(const uint16_t *)(p + 4));
				const uint8_t a = s[0x2A], b = s[0x32], c = s[0x3A];
				if ((uint8_t)(a | b | c) < 0x10 && !(c & (a & b)))
				{
					GteSetSXY012(F<uint32_t>(s, 0x24), F<uint32_t>(s, 0x2C), F<uint32_t>(s, 0x34));
					GteNCLIP();
					GteReadMAC0(s + 0x44);
					if (F<int32_t>(s, 0x44) >= 0)
					{
						GteSetSZ123(F<uint16_t>(s, 0x28), F<uint16_t>(s, 0x30), F<uint16_t>(s, 0x38));
						GteAVSZ3();
						*(uint32_t *)pk = 0x7000000;
						*(uint32_t *)(pk + 4) = ((((uint32_t)*(const uint16_t *)(p + 0xE) & 0x200) | 0x2400) << 16) | F<uint32_t>(s, 0x1C);
						*(uint32_t *)(pk + 8) = F<uint32_t>(s, 0x24);
						*(uint32_t *)(pk + 0x10) = F<uint32_t>(s, 0x2C);
						*(uint32_t *)(pk + 0x18) = F<uint32_t>(s, 0x34);
						*(uint32_t *)(pk + 0xC) = *(const uint32_t *)(p + 8);
						*(uint32_t *)(pk + 0x14) = *(const uint32_t *)(p + 0xC);
						*(uint16_t *)(pk + 0x16) &= 0xFDFF;
						*(uint16_t *)(pk + 0x1C) = *(const uint16_t *)(p + 6);
						GteReadOTZWord(s + 0x4C);
						InsertPrimAutoDepth(ot + (uint32_t)(F<int32_t>(s, 0x4C) >> shift) * 4, pk);
						pk += 0x20;
					}
				}
				k = F<uint16_t>(s, 8);
				p += 0x10;
				F<uint16_t>(s, 8) = (uint16_t)(k - 1);
			} while (k != 0);
		}
		k = F<uint16_t>(s, 0xA);
		F<uint16_t>(s, 0xA) = (uint16_t)(k - 1);
		if (k != 0)
		{
			p += 4;
			do
			{
				CopyVertex(s, 0x24, verts, *(const uint16_t *)(p - 4));
				CopyVertex(s, 0x2C, verts, *(const uint16_t *)(p - 2));
				CopyVertex(s, 0x34, verts, *(const uint16_t *)p);
				CopyVertex(s, 0x3C, verts, *(const uint16_t *)(p + 2));
				const uint8_t d = s[0x42], a = s[0x2A], b = s[0x32], c = s[0x3A];
				if ((uint8_t)(d | a | b | c) < 0x10 && !(c & (d & a & b)))
				{
					GteSetSXY012(F<uint32_t>(s, 0x24), F<uint32_t>(s, 0x2C), F<uint32_t>(s, 0x34));
					GteNCLIP();
					GteReadMAC0(s + 0x44);
					if (F<int32_t>(s, 0x44) >= 0)
					{
						GteSetSZ0123(F<uint16_t>(s, 0x28), F<uint16_t>(s, 0x30), F<uint16_t>(s, 0x38), F<uint16_t>(s, 0x40));
						GteAVSZ4();
						*(uint32_t *)pk = 0x9000000;
						*(uint32_t *)(pk + 4) = ((((uint32_t)*(const uint16_t *)(p + 0xA) & 0x200) | 0x2C00) << 16) | F<uint32_t>(s, 0x1C);
						*(uint32_t *)(pk + 8) = F<uint32_t>(s, 0x24);
						*(uint32_t *)(pk + 0x10) = F<uint32_t>(s, 0x2C);
						*(uint32_t *)(pk + 0x18) = F<uint32_t>(s, 0x34);
						*(uint32_t *)(pk + 0x20) = F<uint32_t>(s, 0x3C);
						*(uint32_t *)(pk + 0xC) = *(const uint32_t *)(p + 4);
						*(uint32_t *)(pk + 0x14) = *(const uint32_t *)(p + 8);
						*(uint16_t *)(pk + 0x1C) = *(const uint16_t *)(p + 0xC);
						*(uint16_t *)(pk + 0x16) &= 0xFDFF;
						*(uint16_t *)(pk + 0x24) = *(const uint16_t *)(p + 0xE);
						GteReadOTZWord(s + 0x4C);
						InsertPrimAutoDepth(ot + (uint32_t)(F<int32_t>(s, 0x4C) >> shift) * 4, pk);
						pk += 0x28;
					}
				}
				k = F<uint16_t>(s, 0xA);
				p += 0x14;
				F<uint16_t>(s, 0xA) = (uint16_t)(k - 1);
			} while (k != 0);
		}
		return (uint32_t)pk;
	}

	static uint32_t SwayModel(const uint8_t *model, uint8_t *s, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		const uint8_t *table = *(const uint8_t *const *)(model + 4);
		const uint8_t *bones = *(const uint8_t *const *)model + 0x10;
		const int32_t count = *(const int32_t *)table;
		const uint8_t *offs = table + 4;
		if (count <= 0) return cursor;
		for (int32_t k = 0; k < count; k++)
		{
			uint8_t *obj = (uint8_t *)(*(const uint8_t *const *)(model + 4) + *(const uint32_t *)offs);
			offs += 4;
			if (!(F<uint32_t>(s, 0x20) & (1u << (k & 31)))) continue;
			uint8_t *out = F<uint8_t *>(s, 4);
			const int32_t groups = *(const int16_t *)obj;
			obj += 2;
			for (int32_t g = groups; g > 0; g--)
			{
				const int32_t bone = *(const int16_t *)obj;
				obj += 2;
				const Mat4x3 *mtx = (const Mat4x3 *)(bones + (uint32_t)mul32(bone, 0x30) + 0x10);
				GteSetRotMatrixCtrl(mtx);
				GteSetTransVectorCtrl(mtx);
				SwayVertices(&obj, &out, s);
			}
			obj = (uint8_t *)(((uint32_t)obj + 3) & ~3u);
			F<uint16_t>(s, 8) = *(const uint16_t *)obj;
			obj += 2;
			F<uint16_t>(s, 0xA) = *(const uint16_t *)obj;
			obj += 0xA;
			F<uint8_t *>(s, 0) = obj;
			cursor = FlatPolygons(s, ot, shift, cursor);
		}
		return cursor;
	}

	static void DrawOverlay(uint8_t *entity, int32_t sinv, int32_t phase)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x8C);
		F<int16_t>(h, 0x86) = (int16_t)(sinv >> 2);
		F<int16_t>(h, 0x84) = (int16_t)phase;
		Mat4x3 *m = (Mat4x3 *)h;
		ComposeAffineTransform(&Camera(), (const Mat4x3 *)(entity + 0x40), m);
		GteSetRotMatrix(m);
		GteSetTransVector(m);
		GteLoadV0((const void *)game_addr(ZERO_Vector));
		GteRTPS();
		GteReadSXY2(h + 0x7C);
		GteReadOTZ(h + 0x88);
		if (F<int32_t>(h, 0x88) > 0)
		{
			FrameCursor() = DrawShadow(entity, RenderOT(0x4040), 0x10, FrameCursor());
			ComputeBonesWorldMatrices(entity + 0x60, m);
			F<int16_t>(h, 0x38) = 0x140;
			F<uint32_t>(h, 0x24) = BoneWorkspace();
			F<int16_t>(h, 0x34) = 0;
			F<int16_t>(h, 0x36) = 0;
			F<int16_t>(h, 0x3A) = 0xD8;
			F<uint32_t>(h, 0x3C) = *(const uint32_t *)(entity + 0x28);
			F<uint32_t>(h, 0x40) = *(const uint32_t *)(entity + 0x7C);
			FrameCursor() = SwayModel(*(const uint8_t *const *)(entity + 0x64), h + 0x20, RenderOT(0x44), 0, FrameCursor());
			BuildBoneMatricesFromPose(entity + 0x60);
			uint8_t *second = *(uint8_t *const *)(entity + 0x78);
			if (second)
			{
				F<uint32_t>(h, 0x40) = 0xFFFFFFFF;
				ComputeBonesWorldMatrices(second, m);
				FrameCursor() = SwayModel(*(const uint8_t *const *)(second + 4), h + 0x20, RenderOT(0x44), 0, FrameCursor());
				BuildBoneMatricesFromPose(second);
			}
		}
		FieldFree(0x8C);
	}

	static int32_t OverlaySin(int32_t e)
	{
		const uint32_t x = (uint32_t)e << 11;
		const uint32_t hi = (uint32_t)(((uint64_t)x * 0x24924925u) >> 32);
		return ComputeSin((int32_t)((((x - hi) >> 1) + hi) >> 4));
	}

	static void AngleDir(const int16_t *a, int16_t *out)
	{
		const int32_t c0 = ComputeCos(a[0]);
		const int32_t s0 = -ComputeSin(a[0]);
		const int32_t s2 = ComputeSin(a[2]);
		const int32_t t = (int32_t)(0u - (uint32_t)mul32(s2, s0)) >> 12;
		const int32_t c2 = ComputeCos(a[2]);
		out[1] = (int16_t)(mul32(c2, s0) >> 12);
		const int32_t s1 = -ComputeSin(a[1]);
		const int32_t c1 = -ComputeCos(a[1]);
		const int32_t x = (int32_t)((uint32_t)mul32(c1, t) + (uint32_t)mul32(s1, c0));
		const int32_t z = (int32_t)((uint32_t)mul32(c1, c0) - (uint32_t)mul32(s1, t));
		out[0] = (int16_t)(x >> 12);
		out[2] = (int16_t)(z >> 12);
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

	static void ParticleDraw(const ParticleNode *p)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		if (p->seq == game_addr(SEQ_Stream) && p->counter < 4) TransformCameraByShadowRotation(p->pos, p->scale, 0);
		else TransformCameraByShadowRotation(p->pos, p->scale, -((int32_t)p->rot >> 2));
		F<uint32_t>(h, 0) = p->seq;
		F<int16_t>(h, 4) = p->counter;
		F<int16_t>(h, 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(0x44), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static void ParticleStep(ParticleNode *p)
	{
		p->vel[1] = (int16_t)(p->vel[1] + p->acc[1]);
		p->vel[0] = (int16_t)(p->vel[0] + p->acc[0]);
		p->vel[2] = (int16_t)(p->vel[2] + p->acc[2]);
		p->pos[1] = (int16_t)(p->pos[1] + p->vel[1]);
		p->pos[2] = (int16_t)(p->pos[2] + p->vel[2]);
		p->pos[0] = (int16_t)(p->pos[0] + p->vel[0]);
	}

	static ParticleNode *NewParticle()
	{
		ParticleNode *p = (ParticleNode *)AddTaskToQueue(QParticles(), game_addr(ORIG_ParticleTask));
		if (p) Memset32(&p->counter, 0, 8);
		return p;
	}
}
}

namespace ff8fx
{
namespace badbreath113
{
	static uint32_t __cdecl ParticleTask(TaskNode *n)
	{
		ParticleNode *p = (ParticleNode *)n;
		ParticleDraw(p);
		p->counter++;
		if (p->counter >= p->end) return TASK_END;
		ParticleStep(p);
		return 0;
	}

	static uint32_t __cdecl TargetTask(TaskNode *n)
	{
		TargetNode *t = (TargetNode *)n;
		uint8_t *entity = Entity(t->slot);
		if ((uint16_t)t->counter <= 0xA)
		{
			ParticleNode *p = NewParticle();
			if (p)
			{
				GetDefaultEffectPosition(entity, p->pos);
				p->vel[0] = (int16_t)((CrtRand() & 0x40) - 0x20);
				p->vel[2] = (int16_t)((CrtRand() & 0x40) - 0x20);
				const int32_t r = CrtRand();
				p->pos[0] = (int16_t)(p->pos[0] + (int16_t)(p->vel[0] << 2));
				p->vel[1] = (int16_t)(-0x20 - (r & 0x1F));
				const int32_t r2 = CrtRand();
				p->seq = game_addr(SEQ_Rise);
				const int32_t prod = mul32(r2, p->pos[1]);
				int32_t q = (int32_t)(((int64_t)prod * 0x57619F1) >> 32) >> 9;
				q += (int32_t)((uint32_t)q >> 31);
				p->end = 0xE;
				p->acc[2] = 0;
				p->pos[1] = (int16_t)q;
				p->pos[2] = (int16_t)(p->pos[2] + (int16_t)(p->vel[2] << 2));
				p->acc[1] = 0;
				p->acc[0] = 0;
				const int16_t s = (int16_t)((CrtRand() >> 4) + 0x800);
				p->scale = s;
				p->rot = s;
			}
		}
		const int32_t e = t->counter - 0xA;
		if ((uint32_t)e < 0x1C)
		{
			const int32_t sinv = OverlaySin(e);
			DrawOverlay(entity, sinv, e << 8);
			*entity |= 4;
			PrimArg arg;
			GetEffectSpawnPosition(entity, 0xF1, 0, arg.pos);
			arg.pos[1] = *(const int16_t *)(entity + 0x24);
			arg.flags = game_addr(PART_Flags);
			arg.morph = TexBase() + 0x11B8;
			prim::play((prim::Layout *)t->cloud, PartCallback, (int)&arg, 0);
		}
		else *(uint16_t *)entity &= 0xFFFB;
		if (t->counter == 0x1B) DamageEntity(entity);
		t->counter++;
		return t->counter <= 0x26 ? 0 : TASK_END;
	}

	static uint32_t __cdecl MainTask(TaskNode *n)
	{
		MainNode *m = (MainNode *)n;
		int16_t sp[3];
		GetEffectSpawnPosition(Caster(), 9, 0, sp);
		const int32_t c = m->counter;
		if ((uint32_t)c <= 0x2C)
		{
			if (c == 0)
			{
				uint32_t mask = *(const uint16_t *)&Ctx()->target_mask;
				int32_t count = 0;
				m->pos[2] = 0;
				m->pos[1] = 0;
				m->pos[0] = 0;
				for (int k = 0; k < 3; k++)
				{
					if (mask & 1)
					{
						int16_t a[4];
						GetDefaultEffectPosition(Entity(k), a);
						m->pos[0] = (int16_t)(m->pos[0] + a[0]);
						m->pos[1] = (int16_t)(m->pos[1] + a[1]);
						m->pos[2] = (int16_t)(m->pos[2] + a[2]);
						count++;
					}
					mask >>= 1;
				}
				m->phase = (int16_t)0xF000;
				m->pos[0] = (int16_t)(m->pos[0] / count);
				m->pos[1] = (int16_t)(m->pos[1] / count);
				m->pos[2] = (int16_t)(m->pos[2] / count + 0x7D0);
			}
			else if (c == 0xA || c == 0x10 || c == 0x1A)
			{
				if (m->mask & 1)
				{
					TargetNode *t = (TargetNode *)AddTaskToQueue(QTargets(), game_addr(ORIG_TargetTask));
					if (t)
					{
						Memset32(&t->counter, 0, 0x3C);
						DecodeModelPrimLayout(game_addr(MODEL_Cloud), t->cloud, 0xEC);
						t->slot = (int16_t)(uint16_t)m->slot;
					}
				}
				m->mask = (uint8_t)(m->mask >> 1);
				m->slot++;
			}
			ParticleNode *p = NewParticle();
			if (p)
			{
				ff8fx::copy<(sizeof(sp))>(p->pos, sp);
				p->vel[0] = (int16_t)(mul32(ComputeSin((int32_t)m->phase >> 2), 2000) >> 12);
				m->phase = (int16_t)(m->phase + 0x174);
				const int32_t r0 = CrtRand();
				p->vel[0] = (int16_t)((r0 & 0x3F) + ((int32_t)p->vel[0] - sp[0]) / 16 - 0x20);
				const int32_t r1 = CrtRand();
				p->vel[1] = (int16_t)((r1 & 0x3F) + ((int32_t)m->pos[1] - sp[1]) / 16 - 0x20);
				const int32_t r2 = CrtRand();
				const int32_t vz = (r2 & 0x3F) + ((int32_t)m->pos[2] - sp[2]) / 16 - 0x20;
				p->seq = game_addr(SEQ_Stream);
				p->end = 0x14;
				p->rot = 0x800;
				p->vel[2] = (int16_t)vz;
				p->acc[0] = (int16_t)(-(int32_t)p->vel[0] >> 5);
				p->acc[1] = (int16_t)(-(int32_t)p->vel[1] >> 5);
				p->acc[2] = (int16_t)(-(int32_t)(int16_t)vz >> 5);
				p->scale = (int16_t)((CrtRand() >> 4) + 0xC00);
			}
			int16_t a[3];
			a[0] = (int16_t)((CrtRand() & 0x7F) + 0x258);
			a[1] = *(const int16_t *)(Caster() + 0xE);
			a[2] = (int16_t)CrtRand();
			for (int k = 0; k < 2; k++)
			{
				ParticleNode *q = NewParticle();
				if (!q) continue;
				ff8fx::copy<(sizeof(sp))>(q->pos, sp);
				AngleDir(a, q->vel);
				q->vel[0] = (int16_t)(q->vel[0] >> 6);
				q->vel[2] = (int16_t)(q->vel[2] >> 6);
				q->vel[1] = (int16_t)(q->vel[1] >> 6);
				q->acc[0] = (int16_t)(-(int32_t)q->vel[0] >> 4);
				q->acc[2] = (int16_t)(-(int32_t)q->vel[2] >> 4);
				q->acc[1] = (int16_t)(-(int32_t)q->vel[1] >> 4);
				q->seq = game_addr(SEQ_Spray);
				q->end = 0xE;
				const int32_t r = CrtRand();
				q->rot = 0;
				q->scale = (int16_t)((r >> 4) + 0x800);
				a[2] = (int16_t)(a[2] + 0x800);
			}
		}
		if ((uint32_t)(c - 0x2C) <= 0xF)
		{
			ParticleNode *p = NewParticle();
			if (p)
			{
				p->vel[0] = (int16_t)((CrtRand() & 0x1F) - 0x10);
				p->vel[2] = (int16_t)((CrtRand() & 0x1F) - 0x10);
				p->vel[1] = (int16_t)(-0x10 - (CrtRand() & 0xF));
				p->pos[0] = (int16_t)((uint16_t)(p->vel[0] << 4) + (uint16_t)sp[0]);
				p->pos[2] = (int16_t)((uint16_t)(p->vel[2] << 4) + (uint16_t)sp[2]);
				p->pos[1] = sp[1];
				p->acc[2] = 0;
				p->acc[1] = 0;
				p->acc[0] = 0;
				p->seq = game_addr(SEQ_Spray);
				p->end = 0xE;
				const int16_t s = (int16_t)((CrtRand() >> 4) + 0x800);
				p->scale = s;
				p->rot = s;
			}
		}
		if (m->counter == 0)
		{
			int16_t pos[4];
			GetDefaultEffectPosition(Caster(), pos);
			BdPlaySE3D(SOUND_BadBreath(), 0x100, pos);
		}
		if (m->counter >= 0x3B) return TASK_END;
		m->counter++;
		return 0;
	}

	static uint32_t __cdecl RootTask(TaskNode *n)
	{
		RootNode *r = (RootNode *)n;
		if (r->arena)
		{
			PacketCursor() = TexBase() + 0x14C8;
			r->arena = 0;
		}
		else
		{
			PacketCursor() = TexBase() + 0x114C8;
			r->arena = 1;
		}
		if (r->counter == 1)
		{
			r->started = 1;
			InitTaskQueuePool(QMain(), (void *)(TexBase() + 0x10B4), 0x104, 1);
			InitTaskQueuePool(QParticles(), (void *)(TexBase() + 0x2F4), 0x2C, 0x50);
			InitTaskQueuePool(QTargets(), (void *)TexBase(), 0xFC, 3);
			MainNode *m = (MainNode *)AddTaskToQueue(QMain(), game_addr(ORIG_MainTask));
			if (m)
			{
				Memset32(&m->counter, 0, 0x3E);
				m->mask = *(const uint8_t *)&Ctx()->target_mask;
			}
		}
		int a, b, c;
		if (r->started)
		{
			a = ExecuteTaskQueue(QMain());
			b = ExecuteTaskQueue(QParticles());
			c = ExecuteTaskQueue(QTargets());
		}
		else a = b = c = (int)n;
		if (r->started && a == 0 && b == 0 && c == 0) return TASK_END;
		r->counter++;
		return 0;
	}
}

	void register_mag113_bad_breath()
	{
		register_port(badbreath113::ORIG_RootTask, (void *)badbreath113::RootTask, "113 RootTask", 113);
		register_port(badbreath113::ORIG_MainTask, (void *)badbreath113::MainTask, "113 MainTask", 113);
		register_port(badbreath113::ORIG_ParticleTask, (void *)badbreath113::ParticleTask, "113 ParticleTask", 113);
		register_port(badbreath113::ORIG_TargetTask, (void *)badbreath113::TargetTask, "113 TargetTask", 113);
	}
}

