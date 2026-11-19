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

#include "mag_common.h"
#include <stddef.h>

namespace ff8fx
{
namespace strong318
{
	using namespace eng;
	using namespace magc;

	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_318_StrongCounterAttack_PacketCursor)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::MAG_318_StrongCounterAttack_CastContext)); }
	inline uint8_t *&Caster() { return var<uint8_t *>(game_addr(game::MAG_318_StrongCounterAttack_CasterEntity)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::MAG_318_StrongCounterAttack_MagicBufferBase)); }
	inline TaskQueue *QSprites() { return (TaskQueue *)game_addr(game::MAG_318_StrongCounterAttack_SpritesQueue); }
	inline TaskQueue *QBones() { return (TaskQueue *)game_addr(game::MAG_318_StrongCounterAttack_BonesQueue); }
	inline TaskQueue *QMain() { return (TaskQueue *)game_addr(game::MAG_318_StrongCounterAttack_MainQueue); }
	static const int16_t *ZERO_VEC() { return (const int16_t *)game_addr(game::MAG_318_StrongCounterAttack_SZeroVector); }

	static constexpr game::Id ORIG_RootTask = game::MAG_318_StrongCounterAttack_RootTask;
	static constexpr game::Id ORIG_MainTask = game::MAG_318_StrongCounterAttack_MainTask;
	static constexpr game::Id ORIG_SpriteTask = game::MAG_318_StrongCounterAttack_SpriteTask;
	static constexpr game::Id ORIG_BoneScaleTask = game::MAG_318_StrongCounterAttack_BoneScaleTask;
	static const uint32_t POOL_Bones = 0x1304, POOL_Sprites = 0x1390;
	static const uint32_t STREAM_STATE = 0x1990;
	static const uint32_t MORPH = 0x19D8;
	static const uint32_t ARENA_A = 0x1DE0;
	static const uint32_t ARENA_B = 0x11DE0;
	static const uint32_t STREAM_DEST = 0x21DE0;
	static constexpr game::Id MODEL_A = game::MAG_318_StrongCounterAttack_ModelA;
	static constexpr game::Id MODEL_B = game::MAG_318_StrongCounterAttack_ModelB;
	static constexpr game::Id MODEL_C = game::MAG_318_StrongCounterAttack_ModelC;
	static constexpr game::Id PRIM_Spike = game::unk_FF7CB0;
	static constexpr game::Id SEQ_Sprite = game::MAG_318_StrongCounterAttack_SeqSprite;
	static constexpr game::Id SEQ_Particle = game::MAG_318_StrongCounterAttack_SeqParticle;
	static const void *SOUND() { return (const void *)game_addr(game::unk_FF77A8); }
	static const int16_t *OFFSETS() { return (const int16_t *)game_addr(game::unk_FFE688); }
	static const int PARTICLES = 0x80;

	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t flag, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, flag, out); }
	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void MulMatrix3(Mat4x3 *a, const void *b) { fn<void (__cdecl *)(Mat4x3 *, const void *)>(game_addr(game::MatrixMultiply3x3))(a, b); }
	inline void GteMVMVA_RotV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0))(); }
	inline uint32_t RenderPrim2(void *h, uint32_t ot, int mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int, uint32_t)>(game_addr(game::MAG_070_RenderPrimModel))(h, ot, mode, cursor); }
	inline int32_t BuildOrthonormalBasis(const int32_t *v, Mat4x3 *m) { return fn<int32_t (__cdecl *)(const int32_t *, Mat4x3 *)>(game_addr(game::BuildOrthonormalBasis))(v, m); }
	inline void MatrixMulVector(const Mat4x3 *m, const int16_t *v, int16_t *out) { fn<void (__cdecl *)(const Mat4x3 *, const int16_t *, int16_t *)>(game_addr(game::matrixMultiplyVector))(m, v, out); }
	inline void VecLerp(const void *a, const void *b, int32_t wa, int32_t wb, void *out) { fn<void (__cdecl *)(const void *, const void *, int32_t, int32_t, void *)>(game_addr(game::VecLerp))(a, b, wa, wb, out); }
	inline int32_t ComputeCos(int32_t a) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(a); }
	inline int32_t CartesianToGameAngle(int32_t x, int32_t y) { return fn<int32_t (__cdecl *)(int32_t, int32_t)>(game_addr(game::CartesianToGameAngle))(x, y); }
	inline void GteLoadIRFromColumn(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadIRFromMatrixColumn))(m); }
	inline void GteMVMVA_RotIR() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotIR))(); }
	inline void GteStoreIRToColumn(void *m) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreIRToMatrixColumn))(m); }
	inline void GteLoadV0Dwords(const void *v) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadV0FromDwords))(v); }
	inline void GteLoadV012(const void *a, const void *b, const void *c) { fn<void (__cdecl *)(const void *, const void *, const void *)>(game_addr(game::GTE_LoadV012))(a, b, c); }
	inline void GteRTPT() { fn<void (__cdecl *)()>(game_addr(game::GTE_RTPT))(); }
	inline void GteReadFLAG(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadFLAG))(dst); }
	inline void GteReadSXY012(void *a, void *b, void *c) { fn<void (__cdecl *)(void *, void *, void *)>(game_addr(game::GTE_ReadSXY012_Split))(a, b, c); }
	inline void StreamLoad(int32_t id, uint32_t dst, int32_t kind) { fn<void (__cdecl *)(int32_t, uint32_t, int32_t)>(game_addr(game::Stream_QueueLoad))(id, dst, kind); }
	inline void StreamPump() { fn<void (__cdecl *)()>(game_addr(game::Stream_Pump))(); }
	inline int32_t StreamDone(int32_t a) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::BattleFile_PreLoad_IsLoaded))(a); }

#pragma pack(push, 1)
	struct RootNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t started;
		uint8_t pad0F;
		uint32_t arena;
	};
	struct Particle
	{
		int16_t pos[3];
		int16_t life;
		int16_t size;
		int16_t pad0A;
		int8_t vel[3];
		uint8_t pad0F;
	};
	struct MainNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t pad0E[0x4A];
		uint8_t *ent;
		int16_t S[4];
		int16_t T[4];
		int16_t off[3][4];
		Mat4x3 M;
		Mat4x3 M2;
		Mat4x3 M3;
		uint8_t layout_a[0xEC];
		uint8_t layout_b[0x5E8];
		uint8_t layout_c[0x348];
		Particle parts[PARTICLES];
		Particle *cursor;
	};
	struct BoneNode
	{
		TaskNode hdr;
		uint8_t *bone;
		int16_t delay;
		int16_t sy;
		int16_t dy;
		int16_t sx;
		int16_t dx;
		int16_t pad1A;
	};
	struct SpriteNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		int16_t pos[4];
		int16_t vel[3];
		int16_t pad1E;
		int16_t drag[3];
		int16_t pad26;
		int16_t vel0[4];
	};
	struct PrimArg
	{
		Mat4x3 m;
		int32_t depth;
		uint32_t morph;
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x14 && sizeof(MainNode) == 0x1304 && sizeof(BoneNode) == 0x1C && sizeof(Particle) == 0x10, "Strong counter-attack nodes");
	static_assert(sizeof(SpriteNode) == 0x30 && sizeof(PrimArg) == 0x28, "Strong counter-attack nodes");
	static_assert(offsetof(MainNode, ent) == 0x58 && offsetof(MainNode, M3) == 0xC4 && offsetof(MainNode, layout_c) == 0x7B8 && offsetof(MainNode, parts) == 0xB00, "Strong counter-attack main node");

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
		RotationFromAngles(r->rot, &m);
		int16_t off[4] = { r->pos[0], r->pos[1], r->pos[2], 0 };
		if (r->flags & 0x200)
		{
			m.t[0] = off[0];
			m.t[1] = off[1];
			m.t[2] = off[2];
		}
		else
		{
			GteSetRotMatrixCtrl(&arg->m);
			GteLoadV0(off);
			GteMVMVA_RotV0();
			GteReadMAC123(m.t);
			if (!(r->flags & 0x8000)) MatrixMultiply(&arg->m, &m);
		}
		m.t[0] = (int32_t)((uint32_t)m.t[0] + (uint32_t)arg->m.t[0]);
		m.t[1] = (int32_t)((uint32_t)m.t[1] + (uint32_t)arg->m.t[1]);
		m.t[2] = (int32_t)((uint32_t)m.t[2] + (uint32_t)arg->m.t[2]);
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

	static int32_t Ease4(int32_t x)
	{
		int32_t e = mul32(x, x) >> 12;
		return mul32(e, e) >> 12;
	}

	static void SpikePoint(const MainNode *m, int32_t e, int16_t *L)
	{
		VecLerp(m->T, m->S, e, 0x1000 - e, L);
	}

	static void SpikeDraw(Mat4x3 *M, const int16_t *L, PrimArg *arg)
	{
		M->t[0] = L[0];
		M->t[1] = L[1];
		M->t[2] = L[2];
		ComposeAffineTransform(&Camera(), M, &arg->m);
		GteSetRotMatrix(&arg->m);
		GteSetTransVector(&arg->m);
		uint8_t *h = AllocHeader(0x58);
		*(uint32_t *)h = game_addr(PRIM_Spike);
		*(uint32_t *)(h + 0x1C) = 0;
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static void AngleToDir(const int16_t *a, int16_t *out)
	{
		out[2] = (int16_t)ComputeCos(a[0]);
		const int32_t s = 0 - ComputeSin(a[0]);
		out[0] = (int16_t)(mul32(ComputeSin(a[2]), s) >> 12);
		out[1] = (int16_t)((0 - mul32(ComputeCos(a[2]), s)) >> 12);
	}

	static void AngleToUnitVector(const int16_t *a, int16_t *out)
	{
		const int32_t c = ComputeCos(a[0]);
		out[1] = (int16_t)(0 - ComputeSin(a[0]));
		out[0] = (int16_t)(mul32(ComputeSin(a[1]), c) >> 12);
		out[2] = (int16_t)((0 - mul32(ComputeCos(a[1]), c)) >> 12);
	}

	static void SpawnParticle(MainNode *m, const int16_t *pos, const int16_t *ang)
	{
		Particle *p = m->cursor;
		if (p->life != 0)
		{
			p = &m->parts[0];
			if (p->life != 0)
			{
				int k = PARTICLES;
				for (;;)
				{
					p++;
					if (--k == 0) return;
					if (p->life == 0) break;
				}
			}
		}
		if ((uint8_t *)p >= (uint8_t *)m + 0x12F0) m->cursor = &m->parts[0];
		else m->cursor = p + 1;
		*(uint32_t *)&p->pos[0] = *(const uint32_t *)&pos[0];
		*(uint32_t *)&p->pos[2] = *(const uint32_t *)&pos[2];
		p->life = 0xB;
		p->size = (int16_t)((CrtRand() >> 4) + 0x800);
		int16_t d[4];
		AngleToDir(ang, d);
		GteLoadV0(d);
		GteMVMVA_RotV0();
		GteStoreIR123(d);
		p->vel[0] = (int8_t)(d[0] >> 8);
		p->vel[1] = (int8_t)(d[1] >> 8);
		p->vel[2] = (int8_t)(d[2] >> 8);
	}

	static uint32_t RenderQuads(uint8_t *h, uint32_t ot, int mode, uint32_t cursor)
	{
		uint8_t *pk = (uint8_t *)cursor;
		const uint8_t *q = *(const uint8_t *const *)(h + 0x2C);
		int32_t count = *(const int32_t *)(h + 0x30);
		if (count <= 0) return cursor;
		for (; count != 0; count--, q += 0x14)
		{
			*(int16_t *)(h + 0x72) = 0;
			*(int16_t *)(h + 0x70) = 0;
			*(int16_t *)(h + 0x6E) = 0;
			*(int16_t *)(h + 0x6A) = 0;
			*(int16_t *)(h + 0x68) = 0;
			*(int16_t *)(h + 0x66) = 0;
			*(int16_t *)(h + 0x74) = 0x1000;
			*(int16_t *)(h + 0x64) = *(const int16_t *)(q + 0x10);
			const int32_t tpage = *(const int16_t *)(q + 0xE);
			*(int16_t *)(h + 0x6C) = *(const int16_t *)(q + 0x12);
			const int32_t uw = q[0], vh = q[2];
			const int32_t w = uw << 3, hh = vh << 3;
			*(int16_t *)(h + 0x54) = (int16_t)-w;
			*(int16_t *)(h + 0x44) = (int16_t)-w;
			*(int16_t *)(h + 0x5C) = (int16_t)w;
			*(int16_t *)(h + 0x4C) = (int16_t)w;
			*(int16_t *)(h + 0x4E) = (int16_t)-hh;
			*(int16_t *)(h + 0x46) = (int16_t)-hh;
			*(int16_t *)(h + 0x5E) = (int16_t)hh;
			*(int16_t *)(h + 0x56) = (int16_t)hh;
			*(int32_t *)(h + 0x78) = (*(const int16_t *)(q + 8) << 4) + w;
			*(int32_t *)(h + 0x7C) = (*(const int16_t *)(q + 0xA) << 4) + hh;
			*(int32_t *)(h + 0x80) = 0;
			GteSetRotMatrixCtrl((const Mat4x3 *)(h + 0x84));
			GteLoadIRFromColumn(h + 0x64);
			GteMVMVA_RotIR();
			*(uint32_t *)pk = 0x9000000;
			pk[4] = q[1];
			pk[5] = q[1];
			pk[6] = q[1];
			pk[7] = q[3];
			GteStoreIRToColumn(h + 0x64);
			GteLoadIRFromColumn(h + 0x66);
			GteMVMVA_RotIR();
			*(uint32_t *)(pk + 0xC) = *(const uint32_t *)(q + 4);
			GteStoreIRToColumn(h + 0x66);
			GteLoadIRFromColumn(h + 0x68);
			GteMVMVA_RotIR();
			pk[0x1C] = q[4];
			GteStoreIRToColumn(h + 0x68);
			GteSetTransVectorCtrl((const Mat4x3 *)(h + 0x84));
			GteLoadV0Dwords(h + 0x78);
			GteMVMVA_RotV0Tr();
			pk[0x15] = q[5];
			GteReadMAC123((int32_t *)(h + 0x78));
			GteSetRotMatrixCtrl((const Mat4x3 *)(h + 0x64));
			GteSetTransVectorCtrl((const Mat4x3 *)(h + 0x64));
			GteLoadV012(h + 0x44, h + 0x4C, h + 0x54);
			GteRTPT();
			int32_t u = q[4] + uw;
			if (u >= 0x100) u = 0xFF;
			pk[0x24] = (uint8_t)u;
			pk[0x14] = (uint8_t)u;
			GteReadFLAG(h + 0x40);
			if (*(const uint32_t *)(h + 0x40) & 0x60000) continue;
			*(uint16_t *)(pk + 0x16) = (uint16_t)(tpage & 0x1FF);
			GteReadSXY012(pk + 8, pk + 0x10, pk + 0x18);
			GteLoadV0(h + 0x5C);
			GteRTPS();
			int32_t v = q[5] + vh;
			if (v >= 0x100) v = 0xFF;
			pk[0x25] = (uint8_t)v;
			pk[0x1D] = (uint8_t)v;
			GteReadSXY2(pk + 0x20);
			InsertPrimAutoDepth(ot + mode * 4, pk);
			pk += 0x28;
		}
		return (uint32_t)pk;
	}

	static void ParticleDraw(uint8_t *h, Particle *p)
	{
		GteSetRotMatrixCtrl(&Camera());
		GteSetTransVectorCtrl(&Camera());
		GteLoadV0(p->pos);
		GteMVMVA_RotV0Tr();
		GteReadMAC123((int32_t *)(h + 0x98));
		*(uint32_t *)h = game_addr(SEQ_Particle);
		*(int16_t *)(h + 0x94) = p->size;
		*(int16_t *)(h + 0x8C) = p->size;
		*(int16_t *)(h + 0x84) = p->size;
		const uint32_t idx = (*(const uint32_t *)(game_addr(SEQ_Particle) + 8) & 0xFFFF) - (int32_t)p->life;
		const uint32_t f = *(const uint16_t *)(game_addr(SEQ_Particle) + 8 + idx * 2) + game_addr(SEQ_Particle);
		*(uint32_t *)(h + 0x30) = *(const uint32_t *)f;
		*(uint32_t *)(h + 0x2C) = f + 4;
		PacketCursor() = RenderQuads(h, RenderOT(), 2, PacketCursor());
		p->life--;
		p->pos[0] = (int16_t)(p->pos[0] + (int16_t)(p->vel[0] << 2));
		p->pos[1] = (int16_t)(p->pos[1] + (int16_t)(p->vel[1] << 2));
		p->pos[2] = (int16_t)(p->pos[2] + (int16_t)(p->vel[2] << 2));
	}

	static void ParticleWalk(Particle *cells)
	{
		uint8_t *h = AllocHeader(0xB4);
		*(int16_t *)(h + 0x92) = 0;
		*(int16_t *)(h + 0x90) = 0;
		*(int16_t *)(h + 0x8E) = 0;
		*(int16_t *)(h + 0x8A) = 0;
		*(int16_t *)(h + 0x88) = 0;
		*(int16_t *)(h + 0x86) = 0;
		*(int16_t *)(h + 0x60) = 0;
		*(int16_t *)(h + 0x58) = 0;
		*(int16_t *)(h + 0x50) = 0;
		*(int16_t *)(h + 0x48) = 0;
		for (int i = 0; i < PARTICLES; i++)
			if (cells[i].life != 0) ParticleDraw(h, &cells[i]);
		FieldFree(0xB4);
	}

	static void SpriteDraw(const int16_t *pos, const int16_t *vel0, int16_t frame)
	{
		uint8_t *h = AllocHeader(0xB4);
		int16_t d[4];
		d[0] = (int16_t)(pos[0] - vel0[0]);
		d[1] = (int16_t)(pos[1] - vel0[1]);
		d[2] = (int16_t)(pos[2] - vel0[2]);
		d[3] = 0;
		TransformCameraByShadowRotation(d, 0x400, -0x100);
		GteLoadV0(ZERO_VEC());
		GteMVMVA_RotV0Tr();
		int16_t a[4], b[4];
		GteStoreIR123(a);
		TransformCameraByShadowRotation(pos, 0x400, -0x100);
		GteLoadV0(vel0);
		GteMVMVA_RotV0Tr();
		GteStoreIR123(b);
		*(int32_t *)(h + 8) = 0 - CartesianToGameAngle((int32_t)a[1] - b[1], (int32_t)b[0] - a[0]);
		*(uint32_t *)h = game_addr(SEQ_Sprite);
		*(int16_t *)(h + 0x24) = 1;
		*(int16_t *)(h + 4) = frame;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
		FieldFree(0xB4);
	}
}
}

namespace ff8fx
{
namespace strong318
{
	static uint32_t __cdecl SpriteTask(TaskNode *n)
	{
		SpriteNode *s = (SpriteNode *)n;
		SpriteDraw(s->pos, s->vel0, s->counter);
		s->counter++;
		if (s->counter >= 0xC) return TASK_END;
		s->vel[1] = (int16_t)(s->vel[1] + s->drag[1]);
		s->vel[0] = (int16_t)(s->vel[0] + s->drag[0]);
		s->vel[2] = (int16_t)(s->vel[2] + s->drag[2]);
		s->pos[1] = (int16_t)(s->pos[1] + s->vel[1]);
		s->pos[2] = (int16_t)(s->pos[2] + s->vel[2]);
		s->pos[0] = (int16_t)(s->pos[0] + s->vel[0]);
		return 0;
	}

	static uint32_t __cdecl BoneScaleTask(TaskNode *n)
	{
		BoneNode *b = (BoneNode *)n;
		b->delay--;
		if (b->delay <= 0)
		{
			b->sy = (int16_t)(b->sy + b->dy);
			b->delay = 0;
			if (b->sy > 0x1000) b->sy = 0x1000;
			b->sx = (int16_t)(b->sx + b->dx);
			if (b->sx > 0x1000) b->sx = 0x1000;
			if (b->sx >= 0x1000 && b->sy >= 0x1000) return TASK_END;
		}
		const int32_t v[3] = { b->sx, b->sx, b->sy };
		Scale3DMatrix((Mat4x3 *)(b->bone + 0x10), v);
		return 0;
	}

	static void StartBoneScale(int32_t bone, int16_t delay, int16_t dx, int16_t dy)
	{
		BoneNode *b = (BoneNode *)AddTaskToQueue(QBones(), game_addr(ORIG_BoneScaleTask));
		b->bone = **(uint8_t ***)(Caster() + 0x64) + bone * 0x30 + 0x10;
		b->delay = delay;
		b->dx = dx;
		b->dy = dy;
		b->sy = 0;
		b->sx = 0;
	}

	static uint32_t __cdecl MainTask(TaskNode *n)
	{
		MainNode *m = (MainNode *)n;
		PrimArg arg;
		if (m->counter == 2)
		{
			GetEffectSpawnPosition(Caster(), 0x1D, 0x800, m->S);
			int32_t v[3] = { (int32_t)m->T[0] - m->S[0], (int32_t)m->S[1] - m->T[1], (int32_t)m->S[2] - m->T[2] };
			BuildOrthonormalBasis(v, &m->M);
			MatrixMulVector(&m->M, OFFSETS(), m->off[0]);
			MatrixMulVector(&m->M, OFFSETS() + 4, m->off[1]);
			MatrixMulVector(&m->M, OFFSETS() + 8, m->off[2]);
			BuildOrthonormalBasis(v, &m->M2);
			v[0] = 0 - v[0];
			v[2] = 0 - v[2];
			BuildOrthonormalBasis(v, &m->M3);
		}
		if ((uint32_t)(m->counter - 2) < 0x16)
		{
			m->M2.t[0] = (int32_t)m->S[0] - m->off[0][0];
			m->M2.t[1] = (int32_t)m->S[1] - m->off[0][1];
			m->M2.t[2] = (int32_t)m->S[2] - m->off[0][2];
			ComposeAffineTransform(&Camera(), &m->M2, &arg.m);
			arg.morph = TexBase() + MORPH;
			arg.depth = 0;
			prim::play((prim::Layout *)m->layout_a, PrimCallback, (int)&arg, 0);
		}
		const int32_t e4 = m->counter - 4;
		if ((uint32_t)e4 < 0x10)
		{
			if (e4 == 0) *(uint32_t *)(Caster() + 0x7C) &= ~8u;
			int16_t L[4];
			SpikePoint(m, Ease4((int32_t)((uint32_t)(e4 << 12) >> 4)), L);
			m->M.t[0] = (int32_t)m->off[0][0] + L[0];
			m->M.t[1] = (int32_t)m->off[0][1] + L[1];
			m->M.t[2] = (int32_t)m->off[0][2] + L[2];
			ComposeAffineTransform(&Camera(), &m->M, &arg.m);
			arg.morph = TexBase() + MORPH;
			arg.depth = -0x80;
			prim::play((prim::Layout *)m->layout_b, PrimCallback, (int)&arg, 0);
			SpikeDraw(&m->M, L, &arg);
		}
		if ((uint32_t)e4 < 0x11)
		{
			GteSetRotMatrix(&m->M);
			int32_t x = (e4 << 12) / 16;
			for (int i = 0; i < 4; i++)
			{
				int16_t L[4];
				SpikePoint(m, Ease4(x), L);
				x -= 0x40;
				int16_t ang[3], pos[4];
				ang[0] = 0x400;
				ang[2] = (int16_t)CrtRand();
				pos[0] = (int16_t)(m->off[1][0] + L[0]);
				pos[1] = (int16_t)(m->off[1][1] + L[1]);
				pos[2] = (int16_t)(m->off[1][2] + L[2]);
				pos[3] = 0;
				for (int j = 2; j != 0; j--)
				{
					SpawnParticle(m, pos, ang);
					ang[2] = (int16_t)(ang[2] + 0x800);
				}
				if ((uint32_t)e4 >= 4)
				{
					ang[0] = 0x400;
					ang[2] = (int16_t)CrtRand();
					pos[0] = (int16_t)(m->off[2][0] + L[0]);
					pos[1] = (int16_t)(m->off[2][1] + L[1]);
					pos[2] = (int16_t)(m->off[2][2] + L[2]);
					for (int j = 2; j != 0; j--)
					{
						SpawnParticle(m, pos, ang);
						ang[2] = (int16_t)(ang[2] + 0x800);
					}
				}
			}
		}
		const int32_t c13 = m->counter - 0x13;
		if ((uint32_t)c13 < 0x20)
		{
			m->M3.t[0] = m->T[0];
			m->M3.t[1] = m->T[1];
			m->M3.t[2] = m->T[2];
			ComposeAffineTransform(&Camera(), &m->M3, &arg.m);
			arg.morph = TexBase() + MORPH;
			arg.depth = -0x80;
			prim::play((prim::Layout *)m->layout_c, PrimCallback, (int)&arg, 0);
			if ((uint32_t)c13 < 4)
			{
				int16_t ang[2];
				ang[1] = (int16_t)CrtRand();
				for (int k = 8; k != 0; k--)
				{
					SpriteNode *s = (SpriteNode *)AddTaskToQueue(QSprites(), game_addr(ORIG_SpriteTask));
					if (s)
					{
						s->counter = 0;
						*(uint32_t *)&s->pos[0] = *(const uint32_t *)&m->T[0];
						*(uint32_t *)&s->pos[2] = *(const uint32_t *)&m->T[2];
						ang[0] = (int16_t)((mul32(CrtRand(), 1500) >> 15) - 0x2EE);
						AngleToUnitVector(ang, s->vel);
						GteSetIR0((CrtRand() & 0xFF) + 0x80);
						GteLoadIR123(s->vel);
						GteGPF();
						GteStoreIR123(s->vel);
						s->drag[0] = (int16_t)(s->vel[0] / -12);
						s->drag[1] = (int16_t)(s->vel[1] / -12);
						s->drag[2] = (int16_t)(s->vel[2] / -12);
						*(uint32_t *)&s->vel0[0] = *(const uint32_t *)&s->vel[0];
						*(uint32_t *)&s->vel0[2] = *(const uint32_t *)&s->vel[2];
					}
					ang[1] = (int16_t)(ang[1] + 0x200);
				}
			}
		}
		ParticleWalk(m->parts);
		if (m->counter == 8) BdPlaySE(SOUND(), 0, 0x80);
		if (m->counter == 0x29) StreamLoad(0x168, TexBase() + STREAM_DEST, 0);
		StreamPump();
		if (m->counter == 0x18) ApplyActionResultToTarget(Ctx()->actions[0].targets);
		if (m->counter == 0x27)
		{
			*(uint32_t *)(Caster() + 0x7C) |= 8u;
			StartBoneScale(0x16, 0, 0x200, 0x400);
			StartBoneScale(0x1D, 8, 0x200, 0x400);
			StartBoneScale(0x21, 0x10, 0x200, 0x400);
			StartBoneScale(0x22, 0x10, 0x200, 0x400);
			StartBoneScale(0x23, 0x10, 0x200, 0x400);
		}
		m->counter++;
		if (m->counter >= 0x33)
		{
			if (StreamDone(0) != 0) return TASK_END;
			m->counter = 0x33;
		}
		return 0;
	}

	static uint32_t __cdecl RootTask(TaskNode *n)
	{
		RootNode *r = (RootNode *)n;
		if (r->arena)
		{
			PacketCursor() = TexBase() + ARENA_A;
			r->arena = 0;
		}
		else
		{
			PacketCursor() = TexBase() + ARENA_B;
			r->arena = 1;
		}
		if (r->counter == 1)
		{
			if (!r->started)
			{
				r->started = 1;
				InitTaskQueuePool(QMain(), (void *)TexBase(), 0x1304, 1);
				InitTaskQueuePool(QBones(), (void *)(TexBase() + POOL_Bones), 0x1C, 5);
				InitTaskQueuePool(QSprites(), (void *)(TexBase() + POOL_Sprites), 0x30, 0x20);
			}
			MainNode *m = (MainNode *)AddTaskToQueue(QMain(), game_addr(ORIG_MainTask));
			Memset32(&m->counter, 0, 0x4BE);
			m->ent = Entity(Ctx()->actions[0].targets[0]);
			GetDefaultEffectPosition(m->ent, m->T);
			m->cursor = m->parts;
			DecodeModelPrimLayout(game_addr(MODEL_A), m->layout_a, 0xEC);
			DecodeModelPrimLayout(game_addr(MODEL_B), m->layout_b, 0x5E8);
			DecodeModelPrimLayout(game_addr(MODEL_C), m->layout_c, 0x348);
			StreamStateInit((void *)(TexBase() + STREAM_STATE));
		}
		int a0, a1, a2;
		if (r->started)
		{
			a0 = ExecuteTaskQueue(QMain());
			a1 = ExecuteTaskQueue(QBones());
			a2 = ExecuteTaskQueue(QSprites());
		}
		else a0 = a1 = a2 = (int)n;
		if (r->started)
		{
			if (a0 == 0 && a1 == 0 && a2 == 0) return TASK_END;
			r->counter = 2;
			return 0;
		}
		r->counter++;
		return 0;
	}
}

	void register_mag318_strong_counter_attack()
	{
		register_port(strong318::ORIG_RootTask, (void *)strong318::RootTask, "318 RootTask", 318);
		register_port(strong318::ORIG_MainTask, (void *)strong318::MainTask, "318 MainTask", 318);
		register_port(strong318::ORIG_SpriteTask, (void *)strong318::SpriteTask, "318 SpriteTask", 318);
		register_port(strong318::ORIG_BoneScaleTask, (void *)strong318::BoneScaleTask, "318 BoneScaleTask", 318);
	}
}

