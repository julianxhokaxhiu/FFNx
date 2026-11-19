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

namespace ff8fx
{
namespace helix063
{
	using namespace eng;
	using namespace magc;

	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::MAG_063_CastCtx)); }
	inline uint8_t *&Caster() { return var<uint8_t *>(game_addr(game::MAG_063_CasterEntity)); }
	inline uint8_t *&Target() { return var<uint8_t *>(game_addr(game::MAG_063_TargetEntity)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_063_PacketCursor)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::MAG_063_MagicBuffer)); }
	inline TaskQueue *QMaster() { return (TaskQueue *)game_addr(game::MAG_063_QMaster); }
	inline TaskQueue *QParticles() { return (TaskQueue *)game_addr(game::MAG_063_QParticles); }
	inline uint32_t RenderOT(uint32_t off) { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + off; }

	static constexpr game::Id ORIG_RootTask = game::MAG_063_RootTask;
	static constexpr game::Id ORIG_MasterTask = game::MAG_063_MasterTask;
	static constexpr game::Id ORIG_ParticleTask = game::MAG_063_ParticleTask;
	static constexpr game::Id MODEL_Ring = game::unk_134F87C;
	static constexpr game::Id MODEL_Core = game::unk_134E154;
	static constexpr game::Id MODEL_Impact = game::unk_1350048;
	static constexpr game::Id MODEL_Beam = game::unk_134C2FC;
	static constexpr game::Id SEQ_Particle = game::unk_134C1B8;
	static constexpr game::Id POINT_A = game::unk_13549F8;
	static constexpr game::Id POINT_B = game::unk_1354A00;
	static constexpr game::Id OBJS_Plain = game::a00000000000000;
	static constexpr game::Id OBJS_Impact = game::a100000;
	static const void *SOUND_Start() { return (const void *)game_addr(game::unk_13549F4); }

	inline int32_t ComputeCos(int32_t a) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(a); }
	inline int32_t CartesianToGameAngle(int32_t x, int32_t y) { return fn<int32_t (__cdecl *)(int32_t, int32_t)>(game_addr(game::CartesianToGameAngle))(x, y); }
	inline int32_t Sqrt(int32_t v) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::Math_SqrtInt))(v); }
	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline void RotX(int32_t angle, Mat4x3 *out) { fn<void (__cdecl *)(int32_t, Mat4x3 *)>(game_addr(game::MAG_063_RotationXMatrix))(angle, out); }
	inline void RotY(int32_t angle, Mat4x3 *out) { fn<void (__cdecl *)(int32_t, Mat4x3 *)>(game_addr(game::Effect_RotationYMatrix))(angle, out); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void RotationFromAnglesB(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::MAG_063_RotationFromAnglesYXZ))(angles, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
	inline void MatMulScaleDiag(Mat4x3 *a, const int16_t *b) { fn<void (__cdecl *)(Mat4x3 *, const int16_t *)>(game_addr(game::MatrixMultiply3x3))(a, b); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void GteMVMVA_RotV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0))(); }
	inline uint32_t RenderPrimSet(void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::MAG_063_RenderPrimSet))(h, ot, mode, cursor); }
	inline uint32_t Tile(int32_t r, int32_t g, int32_t b, int32_t level, uint32_t cursor) { return fn<uint32_t (__cdecl *)(int32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_FullScreenTile))(r, g, b, level, cursor); }
	inline int32_t BuildOrthonormalBasis(const int32_t *v, Mat4x3 *m) { return fn<int32_t (__cdecl *)(const int32_t *, Mat4x3 *)>(game_addr(game::BuildOrthonormalBasis))(v, m); }
	inline void DirectionFromAngles(int32_t pitch, int32_t yaw, int16_t *out) { fn<void (__cdecl *)(int32_t, int32_t, int16_t *)>(game_addr(game::Effect_DirectionFromPitchYaw))(pitch, yaw, out); }

#pragma pack(push, 1)
	struct RootNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t started;
		uint8_t arena;
	};
	struct MasterNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		int16_t pos[4];
		uint8_t ring1[0xE8];
		uint8_t ring2[0xE8];
		uint8_t core[0x6A0];
		uint8_t impact[0x580];
	};
	struct ParticleNode
	{
		TaskNode hdr;
		int16_t pos[3];
		int16_t vel[3];
		int8_t acc[3];
		int8_t frame;
	};
	struct PrimArg
	{
		Mat4x3 m;
		uint32_t objs;
		int16_t depth;
		int16_t scroll;
		uint8_t pad28[8];
		uint32_t morph;
	};
	struct Points
	{
		int16_t p1[3];
		int16_t p2[3];
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x10 && sizeof(MasterNode) == 0xE08 && sizeof(ParticleNode) == 0x1C, "Helix nodes");
	static_assert(sizeof(PrimArg) == 0x34, "Helix prim block");

	static void __cdecl PartCallback(prim::Layout *l, prim::Record *r, int arg_)
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
		int16_t off[4] = { r->pos[0], r->pos[1], r->pos[2], 0 };
		GteSetRotMatrixCtrl(&arg->m);
		GteLoadV0(off);
		GteMVMVA_RotV0();
		GteReadMAC123(m.t);
		if (!(r->flags & 0x8000)) MatrixMultiply(&arg->m, &m);
		m.t[0] = (int32_t)((uint32_t)m.t[0] + (uint32_t)arg->m.t[0]);
		m.t[1] = (int32_t)((uint32_t)m.t[1] + (uint32_t)arg->m.t[1]);
		m.t[2] = (int32_t)((uint32_t)m.t[2] + (uint32_t)arg->m.t[2]);
		if (!(*(const uint32_t *)r->scale == 0x10001000 && r->scale[2] == 0x1000))
		{
			if (r->flags & 0x100)
			{
				const int16_t s[9] = { r->scale[0], 0, 0, 0, r->scale[1], 0, 0, 0, r->scale[2] };
				MatMulScaleDiag(&m, s);
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
		if (*(const uint8_t *)(arg->objs + object) == '1')
		{
			*(uint16_t *)(h + 0x2E) = 0;
			*(uint16_t *)(h + 0x2C) = 0;
			*(uint16_t *)(h + 0x32) = 0x80;
			*(uint16_t *)(h + 0x30) = 0x100;
			*(uint16_t *)(h + 0x22) = (uint16_t)(*(const uint8_t *)&arg->scroll & 0x7F);
		}
		PacketCursor() = RenderPrimSet(h, RenderOT(0x44), 2, PacketCursor());
		FieldFree(0x6C);
	}

	static void CasterPoint(uint32_t data, int16_t *out)
	{
		uint8_t *caster = Caster();
		const uint32_t bones = **(const uint32_t *const *)(caster + 0x64) + 0x10;
		BuildBoneMatricesFromPose(caster + 0x60);
		Mat4x3 m;
		const int32_t bone = *(const int16_t *)(data + 6);
		ComposeAffineTransform((const Mat4x3 *)(Caster() + 0x40), (const Mat4x3 *)(bones + (uint32_t)bone * 0x30 + 0x10), &m);
		GteSetRotMatrixCtrl(&m);
		GteSetTransVectorCtrl(&m);
		GteLoadV0((const void *)data);
		GteMVMVA_RotV0Tr();
		GteStoreIR123(out);
	}

	static void BeamAngles(int32_t dx, int32_t dy, int32_t dz, int16_t *ang)
	{
		const int32_t yaw = CartesianToGameAngle(dx, dz);
		ang[1] = (int16_t)-yaw;
		const int32_t h = Sqrt((int32_t)((uint32_t)mul32(dx, dx) + (uint32_t)mul32(dz, dz)));
		ang[0] = (int16_t)(CartesianToGameAngle(dy, h) & 0xFFF);
		ang[2] = 0;
	}

	static void Beam(const int16_t *from, const int16_t *to, int32_t scroll, int32_t tilt, int32_t turn, int32_t len, int32_t fade)
	{
		const int32_t dx = (int32_t)to[0] - from[0];
		const int32_t dz = (int32_t)to[2] - from[2];
		const int32_t dy = (int32_t)to[1] - from[1];
		int16_t ang[4];
		BeamAngles(dx, dy, dz, ang);
		const int32_t d = Sqrt((int32_t)((uint32_t)mul32(dz, dz) + (uint32_t)mul32(dy, dy) + (uint32_t)mul32(dx, dx)));
		uint8_t *b = (uint8_t *)FieldAlloc(0xBC);
		int32_t *scale = (int32_t *)b;
		scale[1] = 0x1000;
		scale[0] = 0x1000;
		scale[2] = mul32(d, len) >> 12;
		Mat4x3 *rx = (Mat4x3 *)(b + 0x10);
		RotX((int32_t)((uint32_t)tilt - *(const uint32_t *)&ang[0]), rx);
		Scale3DMatrix(rx, scale);
		Mat4x3 *m = (Mat4x3 *)(b + 0x30);
		RotY((int32_t)((uint32_t)turn - *(const uint32_t *)&ang[1] + 0x800), m);
		MatMulScaleDiag(m, (const int16_t *)rx);
		m->t[0] = from[0];
		m->t[1] = from[1];
		m->t[2] = from[2];
		ComposeAffineTransform(&Camera(), m, m);
		GteSetRotMatrix(m);
		GteSetTransVector(m);
		uint8_t *h = b + 0x50;
		*(uint32_t *)(h + 0x1C) = 0x30;
		*(uint32_t *)h = game_addr(MODEL_Beam);
		*(uint16_t *)(h + 0x2A) = 0x100;
		*(uint16_t *)(h + 0x28) = 0x100;
		*(uint16_t *)(h + 0x30) = 0x100;
		*(uint16_t *)(h + 0x22) = (uint16_t)(scroll & 0x7F);
		*(uint16_t *)(h + 0x26) = 0;
		*(uint16_t *)(h + 0x24) = 0;
		*(uint16_t *)(h + 0x2C) = 0;
		*(uint16_t *)(h + 0x2E) = 0x80;
		*(uint16_t *)(h + 0x32) = 0x80;
		*(uint16_t *)(h + 0x20) = 0;
		if (fade != 0)
		{
			*(uint32_t *)(h + 0x1C) = 0xF0;
			*(uint32_t *)(h + 8) = 0;
			*(int32_t *)(h + 0xC) = fade;
		}
		*(int32_t *)(h + 0x18) = -0x80;
		PacketCursor() = RenderPrimSet(h, RenderOT(0x44), 2, PacketCursor());
		FieldFree(0xBC);
	}

	static int32_t TileLevel(int32_t i, int32_t in, int32_t hold, int32_t out)
	{
		const int32_t dout = out ? out : 1;
		const int32_t din = in ? in : 1;
		if (i < in) return shl32(i + 1, 12) / din;
		if (i < in + hold) return 0x1000;
		return (shl32(in - i + hold, 12) - 0x1000) / dout + 0x1000;
	}
	static void FadeTile(int32_t i, int32_t r, int32_t g, int32_t b, int32_t in, int32_t hold, int32_t out)
	{
		PacketCursor() = Tile(r, g, b, TileLevel(i, in, hold, out), PacketCursor());
	}

	struct BeamCurve { int32_t tilt, turn, len, fade; };
	static BeamCurve Curve(int32_t k)
	{
		BeamCurve c;
		const uint32_t x = (uint32_t)k << 12;
		const int32_t a = (int32_t)((uint32_t)(((uint64_t)x * 0xCCCCCCCDull) >> 32) >> 4);
		c.fade = (uint32_t)k < 8 ? (int32_t)(0x1000 - (x >> 3)) : 0;
		const int32_t q = a >> 2;
		ComputeCos(q);
		const int32_t cs = ComputeCos(q);
		const int32_t s1 = ComputeSin(a);
		c.tilt = mul32(s1, cs) >> 0x11;
		const int32_t s2 = ComputeSin(a);
		c.turn = mul32(s2, cs) >> 0xF;
		const int32_t s3 = ComputeSin(a / 2);
		c.len = (int32_t)((uint32_t)a + (uint32_t)s3 * 4);
		return c;
	}

	static void ParticleDraw(ParticleNode *p)
	{
		TransformCameraByShadowRotation(p->pos, 0x1000, -0x400);
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		*(uint32_t *)h = game_addr(SEQ_Particle);
		*(int16_t *)(h + 4) = p->frame;
		*(uint16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(0x44), 2, PacketCursor());
		FieldFree(0xB4);
	}
}
}

namespace ff8fx
{
namespace helix063
{
	static uint32_t __cdecl ParticleTask(TaskNode *n)
	{
		ParticleNode *p = (ParticleNode *)n;
		ParticleDraw(p);
		p->frame++;
		if (p->frame >= 0xC) return TASK_END;
		p->pos[0] = (int16_t)(p->pos[0] + p->vel[0]);
		p->pos[1] = (int16_t)(p->pos[1] + p->vel[1]);
		p->pos[2] = (int16_t)(p->pos[2] + p->vel[2]);
		p->vel[0] = (int16_t)(p->vel[0] + p->acc[0]);
		p->vel[1] = (int16_t)(p->vel[1] + p->acc[1]);
		p->vel[2] = (int16_t)(p->vel[2] + p->acc[2]);
		return 0;
	}

	static int8_t Drag(int16_t v)
	{
		int32_t hi = (int32_t)(((int64_t)v * (int64_t)(int32_t)0xD5555555) >> 32);
		hi >>= 1;
		hi += (int32_t)((uint32_t)hi >> 31);
		return (int8_t)hi;
	}

	static void SpawnParticles(MasterNode *m)
	{
		int32_t count = 10;
		ParticleNode *p = (ParticleNode *)AddTaskToQueue(QParticles(), game_addr(ORIG_ParticleTask));
		while (p)
		{
			p->pos[0] = m->pos[0];
			p->pos[1] = m->pos[1];
			p->pos[2] = m->pos[2];
			const int32_t yaw = CrtRand();
			const int32_t pitch = (CrtRand() & 0x3FF) - 0x200;
			DirectionFromAngles(pitch, yaw, p->vel);
			const int32_t s = (CrtRand() & 0xFF) + 0x100;
			p->frame = 0;
			const int32_t x = mul32(p->vel[0], s) / 4096;
			const int32_t y = mul32(p->vel[1], s) / 4096;
			const int32_t z = mul32(p->vel[2], s) / 4096;
			p->vel[0] = (int16_t)x;
			p->acc[0] = Drag((int16_t)x);
			p->vel[1] = (int16_t)y;
			p->acc[1] = Drag((int16_t)y);
			p->vel[2] = (int16_t)z;
			p->acc[2] = Drag((int16_t)z);
			if (--count == 0) break;
			p = (ParticleNode *)AddTaskToQueue(QParticles(), game_addr(ORIG_ParticleTask));
		}
	}

	static uint32_t __cdecl MasterTask(TaskNode *n)
	{
		MasterNode *m = (MasterNode *)n;
		const int32_t c = m->counter;
		uint32_t flash;
		if ((uint32_t)(c - 0x10) < 4) flash = (uint32_t)mul32(c - 0x10, 0x23F);
		else if (m->counter >= 0x34) flash = (uint32_t)(mul32(0x40 - c, 0x8FC) / 12);
		else flash = m->counter >= 0x10 ? 0x8FC : 0;
		SetScreenFlash(flash, 0);
		Points pts;
		CasterPoint(game_addr(POINT_A), pts.p1);
		CasterPoint(game_addr(POINT_B), pts.p2);
		int16_t mid[3];
		mid[0] = (int16_t)(((int32_t)pts.p2[0] + pts.p1[0]) / 2);
		mid[1] = (int16_t)(((int32_t)pts.p2[1] + pts.p1[1]) / 2);
		mid[2] = (int16_t)(((int32_t)pts.p2[2] + pts.p1[2]) / 2);
		PrimArg arg;
		arg.scroll = 0;
		ff8fx::fill<(sizeof(arg.pad28))>(arg.pad28, 0);
		if ((uint16_t)m->counter < 0x2D)
		{
			RotY(0, &arg.m);
			arg.m.t[0] = mid[0];
			arg.m.t[1] = mid[1];
			arg.m.t[2] = mid[2];
			ComposeAffineTransform(&Camera(), &arg.m, &arg.m);
			arg.depth = (int16_t)0xFF80;
			arg.objs = game_addr(OBJS_Plain);
			arg.morph = TexBase() + 0x21150;
			prim::play((prim::Layout *)m->core, PartCallback, (int)&arg, 0);
		}
		if ((uint16_t)m->counter < 0x30)
		{
			RotY(0, &arg.m);
			arg.m.t[0] = pts.p1[0];
			arg.m.t[1] = pts.p1[1];
			arg.m.t[2] = pts.p1[2];
			ComposeAffineTransform(&Camera(), &arg.m, &arg.m);
			arg.depth = (int16_t)0xFF80;
			arg.objs = game_addr(OBJS_Plain);
			arg.morph = TexBase() + 0x21150;
			prim::play((prim::Layout *)m->ring1, PartCallback, (int)&arg, 0);
			RotY(0, &arg.m);
			arg.m.t[0] = pts.p2[0];
			arg.m.t[1] = pts.p2[1];
			arg.m.t[2] = pts.p2[2];
			ComposeAffineTransform(&Camera(), &arg.m, &arg.m);
			prim::play((prim::Layout *)m->ring2, PartCallback, (int)&arg, 0);
		}
		const int32_t k = m->counter - 0x10;
		if ((uint32_t)k < 0x14)
		{
			const BeamCurve cv = Curve(k);
			const int32_t scroll = shl32(k, 5);
			Beam(pts.p1, m->pos, scroll, -cv.tilt, cv.turn, cv.len, cv.fade);
			Beam(pts.p2, m->pos, scroll, cv.tilt, -cv.turn, cv.len, cv.fade);
		}
		const int32_t e = m->counter - 0x24;
		if ((uint32_t)e < 0x1C)
		{
			if ((uint32_t)(e - 1) < 4)
			{
				FadeTile(e - 1, 0xFF, 0xFF, 0xFF, 0, 1, 3);
			}
			if ((uint32_t)e < 8)
			{
				const int32_t fade = (int32_t)((uint32_t)shl32(e, 12) >> 3);
				const int32_t scroll = shl32(e, 5);
				Beam(pts.p1, m->pos, scroll, 0, 0, 0x1000, fade);
				Beam(pts.p2, m->pos, scroll, 0, 0, 0x1000, fade);
			}
			if ((uint32_t)e < 3) SpawnParticles(m);
			uint8_t *tgt = Target();
			int32_t v[3];
			v[0] = (int32_t)mid[0] - *(const int16_t *)(tgt + 0x1C);
			v[2] = (int32_t)*(const int16_t *)(tgt + 0x20) - mid[2];
			v[1] = 0;
			const int32_t len = BuildOrthonormalBasis(v, &arg.m);
			arg.m.t[0] = mul32(v[0], 500) / len + m->pos[0];
			arg.m.t[1] = m->pos[1];
			arg.m.t[2] = m->pos[2] - mul32(v[2], 500) / len;
			ComposeAffineTransform(&Camera(), &arg.m, &arg.m);
			arg.depth = (int16_t)0xFF80;
			arg.objs = game_addr(OBJS_Impact);
			arg.scroll = (int16_t)shl32(e, 5);
			arg.morph = TexBase() + 0x21150;
			prim::play((prim::Layout *)m->impact, PartCallback, (int)&arg, 0);
		}
		if (m->counter == 0) BdPlaySE(SOUND_Start(), 0, 0x80);
		if (m->counter == 0x36) ApplyActionResultToTarget(Ctx()->actions[0].targets);
		if (m->counter >= 0x40)
		{
			SetScreenFlash(0, 0);
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
			PacketCursor() = TexBase() + 0x1150;
			r->arena = 0;
		}
		else
		{
			PacketCursor() = TexBase() + 0x11150;
			r->arena = 1;
		}
		if (r->counter == 1 && !r->started)
		{
			r->started = 1;
			InitTaskQueuePool(QMaster(), (void *)(TexBase() + 0x348), 0xE08, 1);
			InitTaskQueuePool(QParticles(), (void *)TexBase(), 0x1C, 0x1E);
			MasterNode *m = (MasterNode *)AddTaskToQueue(QMaster(), game_addr(ORIG_MasterTask));
			Memset32(&m->counter, 0, 0x37F);
			DecodeModelPrimLayout(game_addr(MODEL_Ring), m->ring1, 0xE8);
			DecodeModelPrimLayout(game_addr(MODEL_Ring), m->ring2, 0xE8);
			DecodeModelPrimLayout(game_addr(MODEL_Core), m->core, 0x6A0);
			DecodeModelPrimLayout(game_addr(MODEL_Impact), m->impact, 0x580);
			GetDefaultEffectPosition(Target(), m->pos);
		}
		int a;
		if (r->started)
		{
			a = ExecuteTaskQueue(QMaster());
			ExecuteTaskQueue(QParticles());
		}
		else a = (int)n;
		if (r->started && a == 0) return TASK_END;
		r->counter++;
		return 0;
	}
}

	void register_mag063_helix_attack()
	{
		register_port(helix063::ORIG_RootTask, (void *)helix063::RootTask, "H063 RootTask", 63);
		register_port(helix063::ORIG_MasterTask, (void *)helix063::MasterTask, "H063 MasterTask", 63);
		register_port(helix063::ORIG_ParticleTask, (void *)helix063::ParticleTask, "H063 ParticleTask", 63);
	}
}

