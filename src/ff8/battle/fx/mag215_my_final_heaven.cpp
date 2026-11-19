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

// Effect 215: My Final Heaven

#include "mag_common.h"

namespace ff8fx
{
namespace heaven215
{
	using namespace eng;
	using namespace magc;

	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::MAG_215_MYFINALHEAVEN_Ctx)); }
	inline uint8_t *&Caster() { return var<uint8_t *>(game_addr(game::MAG_215_MYFINALHEAVEN_Caster)); }
	inline TaskQueue *QRings() { return (TaskQueue *)game_addr(game::MAG_215_MYFINALHEAVEN_QRings); }
	inline TaskQueue *QDirector() { return (TaskQueue *)game_addr(game::MAG_215_MYFINALHEAVEN_QDirector); }
	inline TaskQueue *QRoot() { return (TaskQueue *)game_addr(game::MAG_215_MYFINALHEAVEN_QRoot); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_215_MYFINALHEAVEN_PacketCursor)); }
	inline uint32_t &PacketCursor2() { return var<uint32_t>(game_addr(game::MAG_215_MYFINALHEAVEN_PacketCursor2)); }
	inline const int16_t *ZeroAngles() { return (const int16_t *)game_addr(game::MAG_215_MYFINALHEAVEN_ZeroAngles); }
	inline uint32_t *CamAnchor() { return (uint32_t *)game_addr(game::MAG_215_MYFINALHEAVEN_CamAnchor); }
	inline uint32_t *CentreWords() { return (uint32_t *)game_addr(game::MAG_215_MYFINALHEAVEN_CentreWords); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::MAG_215_MYFINALHEAVEN_MagicBuffer)); }
	inline uint32_t OTBase() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)); }
	inline uint32_t &FramePackets() { return var<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)); }
	inline int16_t *CamEye() { return (int16_t *)game_addr(game::Battle_Camera_world_XZ_s16); }
	inline int16_t *CamLookAt() { return (int16_t *)game_addr(game::Battle_Camera_LookAt_XZ_s16); }
	inline int16_t &CamRoll() { return var<int16_t>(game_addr(game::g_BattleCam_Roll)); }
	inline int16_t &ViewWord() { return var<int16_t>(game_addr(game::BATTLE_PROJECTION_DISTANCE)); }

	static constexpr game::Id ORIG_RootTask = game::MAG_215_MYFINALHEAVEN_RootTask;
	static constexpr game::Id ORIG_DirectorTask = game::MAG_215_MYFINALHEAVEN_DirectorTask;
	static constexpr game::Id ORIG_RingTask = game::MAG_215_MYFINALHEAVEN_RingTask;
	static constexpr game::Id ORIG_CallbackA = game::MAG_215_MYFINALHEAVEN_PartCallback;
	static constexpr game::Id ORIG_CallbackB = game::MAG_215_MYFINALHEAVEN_PartCallbackCameraAligned;

	static constexpr game::Id COM_Anim = game::MAG_215_MY_FINAL_HEAVEN_AnimSet;
	static constexpr game::Id LAYOUT_Anchor = game::unk_11550A8, LAYOUT_Back = game::unk_1168A5C, LAYOUT_Run = game::unk_1160BB0, LAYOUT_Screen = game::unk_11625E0;
	static constexpr game::Id LAYOUT_Far = game::unk_1169844, LAYOUT_Jump = game::unk_116703C, LAYOUT_JumpBone = game::unk_115DCCC, LAYOUT_Crater = game::unk_116AD0C;
	static constexpr game::Id LAYOUT_Bone = game::unk_115A910, LAYOUT_Ring = game::unk_115C4C8, LAYOUT_TargetRing = game::unk_115CA08;
	static constexpr game::Id KIND_Anchor = game::a0100, KIND_Walk = game::a1, KIND_Screen = game::a0900, KIND_Plain = game::a000;
	static constexpr game::Id KIND_Jump = game::a12, KIND_Crater = game::a100, KIND_Ring = game::a0;
	static constexpr game::Id MODEL_Debris = game::unk_1154F58;
	static const int16_t *SINCOS() { return (const int16_t *)game_addr(game::unk_116EB1C); }
	static const void *SOUND_A() { return (const void *)game_addr(game::unk_1154F48); }
	static const void *SOUND_B() { return (const void *)game_addr(game::unk_1154F4C); }

	inline int32_t ComputeCos(int32_t a) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(a); }
	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t flag, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, flag, out); }
	inline uint32_t BindExternalAnimSet(uint8_t *entity, void *out, uint32_t data) { return fn<uint32_t (__cdecl *)(uint8_t *, void *, uint32_t)>(game_addr(game::BattleEntity_BindExternalAnimSet))(entity, out, data); }
	inline uint32_t QueueChainTransformation(uint8_t *entity, uint32_t id) { return fn<uint32_t (__cdecl *)(uint8_t *, uint32_t)>(game_addr(game::QueueChainTransformation))(entity, id); }
	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline void RotationY(int32_t a, Mat4x3 *out) { fn<void (__cdecl *)(int32_t, Mat4x3 *)>(game_addr(game::Effect_RotationYMatrix))(a, out); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void RotationFromAnglesB(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::MAG_063_RotationFromAnglesYXZ))(angles, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void MatrixMultiply3x3(Mat4x3 *a, const Mat4x3 *b) { fn<void (__cdecl *)(Mat4x3 *, const Mat4x3 *)>(game_addr(game::MatrixMultiply3x3))(a, b); }
	inline void Matrix3x3MultiplyPSX(const Mat4x3 *a, const Mat4x3 *b, Mat4x3 *out) { fn<void (__cdecl *)(const Mat4x3 *, const Mat4x3 *, Mat4x3 *)>(game_addr(game::Matrix3x3_MultiplyPSX))(a, b, out); }
	inline void Apply3DTransform(const Mat4x3 *m, const int16_t *in, int32_t *out) { fn<void (__cdecl *)(const Mat4x3 *, const int16_t *, int32_t *)>(game_addr(game::apply3DTransform))(m, in, out); }
	inline uint32_t RenderPrimSet(void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::MAG_063_RenderPrimSet))(h, ot, mode, cursor); }
	inline uint32_t DrawShadow(uint8_t *e, uint32_t ot, int32_t a, uint32_t cursor) { return fn<uint32_t (__cdecl *)(uint8_t *, uint32_t, int32_t, uint32_t)>(game_addr(game::BattleEntity_DrawShadow))(e, ot, a, cursor); }
	inline uint32_t RenderGeometry(uint32_t model, void *ctx, uint32_t ot, int32_t a, uint32_t cursor) { return fn<uint32_t (__cdecl *)(uint32_t, void *, uint32_t, int32_t, uint32_t)>(game_addr(game::RenderGeometry))(model, ctx, ot, a, cursor); }
	inline int32_t SqrtScaled(int32_t v) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::sub_56BED0))(v); }
	inline void CamLerp(const int16_t *a, const int16_t *b, int32_t t) { fn<void (__cdecl *)(const int16_t *, const int16_t *, int32_t)>(game_addr(game::MAG_325_CameraLerp))(a, b, t); }
	inline void CameraArmReturn() { fn<void (__cdecl *)()>(game_addr(game::BS_Camera_ArmReturnAfterEffect))(); }
	inline void StreamQueueLoad(int32_t id, uint32_t dst, int32_t kind) { fn<void (__cdecl *)(int32_t, uint32_t, int32_t)>(game_addr(game::Stream_QueueLoad))(id, dst, kind); }
	inline void StreamPump() { fn<void (__cdecl *)()>(game_addr(game::Stream_Pump))(); }
	inline int32_t StreamPreload(int32_t a) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::BattleFile_PreLoad_IsLoaded))(a); }
	inline void GteMVMVA_RotV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0))(); }
	inline void GteMVMVA_RotIR() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotIR))(); }
	inline void GteLoadIRFromColumn(const void *p) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadIRFromMatrixColumn))(p); }
	inline void GteStoreIRToColumn(void *p) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreIRToMatrixColumn))(p); }
	inline void GteLoadV0FromDwords(const void *p) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadV0FromDwords))(p); }
	inline void GteLoadV012(const void *a, const void *b, const void *c) { fn<void (__cdecl *)(const void *, const void *, const void *)>(game_addr(game::GTE_LoadV012))(a, b, c); }
	inline void GteRTPT() { fn<void (__cdecl *)()>(game_addr(game::GTE_RTPT))(); }
	inline void GteReadFLAG(void *p) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadFLAG))(p); }
	inline void GteNCLIP() { fn<void (__cdecl *)()>(game_addr(game::GTE_NCLIP))(); }
	inline void GteReadSXY012Split(void *a, void *b, void *c) { fn<void (__cdecl *)(void *, void *, void *)>(game_addr(game::GTE_ReadSXY012_Split))(a, b, c); }
	inline void GteReadMAC0(void *p) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadMAC0))(p); }
	inline void GteAVSZ3() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ3))(); }
	inline void GteReadOTZ32(void *p) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadOTZ))(p); }
	inline void GteLoadV0u(const void *v) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadV0_u16))(v); }
	inline void GteReadIR2Word(void *p) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadIR2Word))(p); }

	enum : int { SITE_ANCHOR = 1, SITE_BACK, SITE_RUN, SITE_SCREEN, SITE_FAR, SITE_JUMP, SITE_JUMPBONE, SITE_CRATER, SITE_BONE };

#pragma pack(push, 1)
	struct RootNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t state;
		uint8_t parity;
	};
	struct TargetRec
	{
		uint8_t *e;
		uint32_t pos01;
		uint32_t pos23;
		uint32_t rot01;
		uint32_t rot23;
		int16_t ang[4];
		int16_t height[2];
	};
	struct Debris
	{
		int16_t pos[3];
		int16_t life;
		int16_t vel[3];
		int16_t scale;
		int16_t rx, ry;
		int8_t spin[2];
	};
	struct DirectorNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		uint8_t bind[0x20];
		uint32_t saved_pos[2];
		uint32_t saved_rot[2];
		TargetRec recs[10];
		int16_t centre[3];
		int16_t pad186;
		uint8_t layout_b[0x188];
		uint8_t layout_c[0x27C];
		uint8_t layout_a[0x370];
		Debris debris[0x80];
		Debris *cursor;
	};
	struct ArgA
	{
		Mat4x3 m;
		const uint8_t *kind;
		int16_t depth;
		int16_t scroll;
		int16_t fade;
		int16_t pad2A;
		uint32_t morph;
	};
	struct ArgB
	{
		int16_t pos[3];
		int16_t pad06;
		const uint8_t *kind;
		int16_t depth;
		int16_t scroll;
		int16_t fade;
		int16_t pad12;
		uint32_t morph;
	};
	struct RingNode
	{
		TaskNode hdr;
		int16_t pos[3];
		int16_t scale;
		int16_t angle;
		int16_t pad16;
		uint8_t layout[0x27C];
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x10 && sizeof(DirectorNode) == 0x1400 && sizeof(TargetRec) == 0x20 && sizeof(Debris) == 0x16, "My Final Heaven nodes");
	static_assert(sizeof(ArgA) == 0x30 && sizeof(ArgB) == 0x18 && sizeof(RingNode) == 0x294, "My Final Heaven blocks");
}
}

namespace ff8fx
{
namespace heaven215
{
	using namespace eng;
	using namespace magc;

	inline int16_t &W16(void *p, int off) { return *(int16_t *)((uint8_t *)p + off); }
	inline uint16_t &U16(void *p, int off) { return *(uint16_t *)((uint8_t *)p + off); }
	inline int32_t &W32(void *p, int off) { return *(int32_t *)((uint8_t *)p + off); }
	inline uint32_t &U32(void *p, int off) { return *(uint32_t *)((uint8_t *)p + off); }
	inline uint8_t &B8(void *p, int off) { return *((uint8_t *)p + off); }
	inline int32_t sdiv4096(int32_t v) { return (int32_t)((uint32_t)v + ((uint32_t)(v >> 31) & 0xFFFu)) >> 12; }

	inline uint8_t *Targets() { return Ctx()->actions[0].targets; }
	inline int32_t TargetCount() { return Ctx()->actions[0].target_count; }

	static void ScaledRotY(int32_t angle, int32_t scale, Mat4x3 *out)
	{
		ff8fx::fill<(0x14)>(out, 0);
		const int32_t a = -(int32_t)(int16_t)angle;
		const int32_t r = (int16_t)scale;
		const int32_t s = mul32(ComputeSin(a), r) >> 12;
		const int32_t c = mul32(ComputeCos(a), r) >> 12;
		out->m[2][0] = (int16_t)s;
		out->m[1][1] = (int16_t)scale;
		out->m[0][0] = (int16_t)c;
		out->m[0][2] = (int16_t)-s;
		out->m[2][2] = (int16_t)c;
	}

	static void ScaledRotX(int32_t angle, int32_t scale, Mat4x3 *out)
	{
		ff8fx::fill<(0x14)>(out, 0);
		const int16_t *sc = SINCOS() + (angle & 0xFFF) * 2;
		const int32_t s = (int16_t)scale;
		const int32_t a = mul32(sc[0], s) >> 12;
		const int32_t b = mul32(sc[1], s) >> 12;
		out->m[0][0] = (int16_t)scale;
		out->m[2][1] = (int16_t)-a;
		out->m[1][1] = (int16_t)b;
		out->m[1][2] = (int16_t)a;
		out->m[2][2] = (int16_t)b;
	}

	static void RotYTable(int32_t angle, Mat4x3 *out)
	{
		ff8fx::fill<(0x14)>(out, 0);
		const int16_t *sc = SINCOS() + (angle & 0xFFF) * 2;
		out->m[2][0] = sc[0];
		out->m[0][0] = sc[1];
		out->m[1][1] = 0x1000;
		out->m[0][2] = (int16_t)-sc[0];
		out->m[2][2] = sc[1];
	}

	static void WaveVertices(uint32_t model, uint8_t *out, int32_t s)
	{
		const int32_t n = *(const int32_t *)(model + 4);
		const int16_t *v = (const int16_t *)(model + 0xC);
		const int32_t k = mul32(n, s) >> 12;
		int16_t *o = (int16_t *)out;
		for (int32_t i = 0; i < n; i++, v += 4, o += 4)
		{
			if (i > k)
			{
				*(uint32_t *)o = 0;
				o[2] = 0x4000;
				continue;
			}
			if ((uint32_t)(i - k + 0x40) < 0x40)
			{
				const int32_t w = (int32_t)((uint32_t)(-0x40 * i) + (uint32_t)(0x40 * k));
				const int16_t *u = v + (2 - (i & 7)) * 4;
				o[0] = (int16_t)(sdiv4096(mul32((int32_t)v[0] - (int32_t)u[0], w)) + u[0]);
				o[1] = (int16_t)(sdiv4096(mul32((int32_t)v[1] - (int32_t)u[1], w)) + u[1]);
				o[2] = v[2];
				continue;
			}
			ff8fx::copy<(8)>(o, v);
		}
	}

	static uint8_t *PartModel(prim::Layout *l, prim::Record *r, uint8_t *h, uint32_t morph)
	{
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
				BlendVertexFrames((uint32_t)model, f0, f1, t, morph);
				*(uint32_t *)(h + 4) = morph;
			}
		}
		return model;
	}

	static void PartHeader(prim::Record *r, uint8_t *h, int16_t depth, const uint8_t *kind, int16_t scroll, int16_t fade, uint32_t morph)
	{
		*(uint32_t *)(h + 0x1C) = 0x2030;
		*(int32_t *)(h + 0xC) = r->a;
		if (r->a != 0)
		{
			*(uint32_t *)(h + 0x1C) = 0x20F0;
			*(uint32_t *)(h + 8) = *(const uint32_t *)r->rgb;
		}
		*(int32_t *)(h + 0x18) = depth;
		U16(h, 0x22) = 0;
		U16(h, 0x20) = 0;
		U16(h, 0x26) = 0;
		U16(h, 0x24) = 0;
		U16(h, 0x2A) = 0x100;
		U16(h, 0x28) = 0x100;
		switch (kind[(int16_t)r->flags_lo])
		{
		case 0x31:
			U16(h, 0x2C) = 0;
			U16(h, 0x2E) = 0;
			U16(h, 0x32) = 0x80;
			U16(h, 0x30) = 0x100;
			U16(h, 0x22) = (uint16_t)((uint8_t)scroll & 0x7F);
			break;
		case 0x32:
			U16(h, 0x2C) = 0;
			U16(h, 0x2E) = 0;
			U16(h, 0x32) = 0x80;
			U16(h, 0x30) = 0x100;
			U16(h, 0x22) = (uint16_t)((uint8_t)(-(uint8_t)scroll) & 0x7F);
			break;
		case 0x33:
			U16(h, 0x2C) = 0;
			U16(h, 0x2E) = 0x80;
			U16(h, 0x32) = 0x80;
			U16(h, 0x30) = 0x100;
			U16(h, 0x22) = (uint16_t)((uint8_t)scroll & 0x7F);
			break;
		case 0x39:
			WaveVertices(*(const uint32_t *)h, (uint8_t *)morph, scroll);
			*(uint32_t *)(h + 4) = morph;
			break;
		}
		if (fade != 0)
		{
			*(uint32_t *)(h + 8) = 0;
			*(int32_t *)(h + 0xC) = fade;
			B8(h, 0x1C) |= 0xC0;
		}
		PacketCursor() = RenderPrimSet(h, OTBase() + 0x44, 2, PacketCursor());
		FieldFree(0x6C);
	}

	static int16_t g_off_pad = 0;

	static void __cdecl CallbackA(prim::Layout *l, prim::Record *r, int arg_)
	{
		const ArgA *arg = (const ArgA *)arg_;
		if (((int32_t)r->scale[2] | *(const int32_t *)r->scale) == 0) return;
		if (r->a >= 0x1000 && *(const uint32_t *)r->rgb == 0) return;
		uint8_t *h = (uint8_t *)FieldAlloc(0x6C);
		PartModel(l, r, h, arg->morph);
		Mat4x3 m;
		if (r->flags & 0x40000) RotationFromAnglesB(r->rot, &m);
		else RotationFromAngles(r->rot, &m);
		const int16_t off[4] = { r->pos[0], r->pos[1], r->pos[2], g_off_pad };
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
			MatrixMultiply(&arg->m, &m);
		}
		m.t[0] = (int32_t)((uint32_t)m.t[0] + (uint32_t)arg->m.t[0]);
		m.t[1] = (int32_t)((uint32_t)m.t[1] + (uint32_t)arg->m.t[1]);
		m.t[2] = (int32_t)((uint32_t)m.t[2] + (uint32_t)arg->m.t[2]);
		if (!(*(const uint32_t *)r->scale == 0x10001000 && r->scale[2] == 0x1000))
		{
			if (r->flags & 0x100)
			{
				Mat4x3 s;
				s.m[0][0] = r->scale[0];
				s.m[1][1] = r->scale[1];
				s.m[2][2] = r->scale[2];
				s.m[1][2] = 0;
				s.m[0][2] = 0;
				s.m[2][1] = 0;
				s.m[0][1] = 0;
				s.m[2][0] = 0;
				s.m[1][0] = 0;
				MatrixMultiply3x3(&m, &s);
			}
			else
			{
				const int32_t v[3] = { r->scale[0], r->scale[1], r->scale[2] };
				Scale3DMatrix(&m, v);
			}
		}
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		PartHeader(r, h, arg->depth, arg->kind, arg->scroll, arg->fade, arg->morph);
	}

	static int16_t g_m2_pad = 0;

	static void __cdecl CallbackB(prim::Layout *l, prim::Record *r, int arg_)
	{
		const ArgB *arg = (const ArgB *)arg_;
		if (((int32_t)r->scale[2] | *(const int32_t *)r->scale) == 0) return;
		if (r->a >= 0x1000 && *(const uint32_t *)r->rgb == 0) return;
		uint8_t *h = (uint8_t *)FieldAlloc(0x6C);
		PartModel(l, r, h, arg->morph);
		Mat4x3 m;
		if (r->flags & 0x40000) RotationFromAnglesB(r->rot, &m);
		else RotationFromAngles(r->rot, &m);
		const int16_t off[4] = { r->pos[0], r->pos[1], r->pos[2], g_off_pad };
		int32_t o32[3];
		if (r->flags & 0x200)
		{
			o32[0] = off[0];
			o32[1] = off[1];
			o32[2] = off[2];
		}
		else Apply3DTransform(&Camera(), off, o32);
		if (!(*(const uint32_t *)r->scale == 0x10001000 && r->scale[2] == 0x1000))
		{
			if (r->flags & 0x100)
			{
				Mat4x3 s;
				s.m[0][0] = r->scale[0];
				s.m[1][1] = r->scale[1];
				s.m[2][2] = r->scale[2];
				s.m[1][2] = 0;
				s.m[0][2] = 0;
				s.m[2][1] = 0;
				s.m[0][1] = 0;
				s.m[2][0] = 0;
				s.m[1][0] = 0;
				MatrixMultiply3x3(&m, &s);
			}
			else
			{
				for (int i = 0; i < 3; i++)
					for (int j = 0; j < 3; j++) m.m[i][j] = (int16_t)(mul32(m.m[i][j], r->scale[i]) >> 12);
			}
		}
		Mat4x3 m2;
		m2.pad = g_m2_pad;
		if (r->flags & 0x200)
		{
			ff8fx::copy<(0x12)>(&m2, &m);
			m2.pad = g_m2_pad;
		}
		else Matrix3x3MultiplyPSX(&Camera(), &m, &m2);
		Apply3DTransform(&Camera(), arg->pos, m2.t);
		m2.t[0] = (int32_t)((uint32_t)m2.t[0] + (uint32_t)Camera().t[0] + (uint32_t)o32[0]);
		m2.t[1] = (int32_t)((uint32_t)m2.t[1] + (uint32_t)Camera().t[1] + (uint32_t)o32[1]);
		m2.t[2] = (int32_t)((uint32_t)m2.t[2] + (uint32_t)Camera().t[2] + (uint32_t)o32[2]);
		GteSetRotMatrix(&m2);
		GteSetTransVector(&m2);
		PartHeader(r, h, arg->depth, arg->kind, arg->scroll, arg->fade, arg->morph);
	}

	static void DebrisInit(DirectorNode *d)
	{
		for (int i = 0x7F; i >= 0; i--) d->debris[i].life = 0;
		d->cursor = &d->debris[0];
	}

	static Debris *DebrisAlloc(DirectorNode *d)
	{
		Debris *k = d->cursor;
		if (k->life != 0)
		{
			k = &d->debris[0];
			if (k->life != 0)
			{
				int n = 0x80;
				do
				{
					k++;
					if (--n == 0) return nullptr;
				} while (k->life != 0);
			}
		}
		if (k >= &d->debris[0x7F]) d->cursor = &d->debris[0];
		else d->cursor = k + 1;
		k->rx = (int16_t)CrtRand();
		k->ry = (int16_t)CrtRand();
		k->spin[0] = (int8_t)CrtRand();
		k->spin[1] = (int8_t)CrtRand();
		return k;
	}

	static uint32_t TriRender(uint8_t *h, uint32_t ot, int32_t shift, uint32_t pk)
	{
		const uint8_t *v = *(const uint8_t **)h;
		const uint8_t *t = *(const uint8_t **)(h + 4);
		int32_t count = *(const int32_t *)t;
		t += 4;
		*(const uint8_t **)(h + 4) = t;
		if (count <= 0) return pk;
		uint8_t *p = (uint8_t *)pk;
		for (; count != 0; count--, t += 0x14)
		{
			GteLoadV012(v + (uint32_t)U16((void *)t, 4) * 4, v + (uint32_t)U16((void *)t, 6) * 4, v + (uint32_t)U16((void *)t, 8) * 4);
			GteRTPT();
			U32(p, 0) = 0x6000000;
			U32(p, 4) = *(const uint32_t *)t;
			GteReadFLAG(h + 0x14);
			if (U32(h, 0x14) & 0x60000) continue;
			GteNCLIP();
			uint32_t clip = 0;
			GteReadSXY012Split(p + 8, p + 0x10, p + 0x18);
			U32(p, 0xC) = *(const uint32_t *)(t + 0xC);
			U32(p, 0x14) = *(const uint32_t *)(t + 0x10);
			GteReadMAC0(h + 8);
			if (W32(h, 8) < 0) continue;
			GteAVSZ3();
			const int16_t x0 = W16(p, 8), x1 = W16(p, 0x10), x2 = W16(p, 0x18);
			const int16_t y0 = W16(p, 0xA), y1 = W16(p, 0x12), y2 = W16(p, 0x1A);
			clip = (x0 < 0 || x0 > 0xA00) ? 1 : 0;
			if (x1 < 0 || x1 > 0xA00) clip |= 2;
			if (x2 < 0 || x2 > 0xA00) clip |= 4;
			if (y0 < 0 || y0 > 0x6C0) clip |= 0x10;
			if (y1 < 0 || y1 > 0x6C0) clip |= 0x20;
			if (y2 < 0 || y2 > 0x6C0) clip |= 0x40;
			if ((clip & 7) == 7 || (clip & 0x70) == 0x70) continue;
			GteReadOTZ32(h + 0x10);
			InsertPrimAutoDepth(ot + (uint32_t)(W32(h, 0x10) >> shift) * 4, p);
			p += 0x1C;
		}
		*(const uint8_t **)(h + 4) = t;
		return (uint32_t)p;
	}

	static void DebrisDrawOne(const Debris *k, uint8_t *h)
	{
		Mat4x3 *m = (Mat4x3 *)h;
		ScaledRotX(k->rx, k->scale, m);
		RotYTable(k->ry, (Mat4x3 *)(h + 0x20));
		GteSetRotMatrixCtrl(m);
		GteLoadIRFromColumn(h + 0x20);
		GteMVMVA_RotIR();
		GteStoreIRToColumn(h);
		GteLoadIRFromColumn(h + 0x22);
		GteMVMVA_RotIR();
		GteStoreIRToColumn(h + 2);
		GteLoadIRFromColumn(h + 0x24);
		GteMVMVA_RotIR();
		GteStoreIRToColumn(h + 4);
		m->t[0] = k->pos[0];
		m->t[1] = k->pos[1];
		m->t[2] = k->pos[2];
		GteSetRotMatrixCtrl(&Camera());
		GteLoadIRFromColumn(h);
		GteMVMVA_RotIR();
		GteStoreIRToColumn(h);
		GteLoadIRFromColumn(h + 2);
		GteMVMVA_RotIR();
		GteStoreIRToColumn(h + 2);
		GteLoadIRFromColumn(h + 4);
		GteMVMVA_RotIR();
		GteStoreIRToColumn(h + 4);
		GteSetTransVectorCtrl(&Camera());
		GteLoadV0FromDwords(m->t);
		GteMVMVA_RotV0Tr();
		GteReadMAC123(m->t);
		GteSetRotMatrixCtrl(m);
		GteSetTransVectorCtrl(m);
		U32(h, 0x40) = game_addr(MODEL_Debris);
		U32(h, 0x44) = *(const uint32_t *)game_addr(game::unk_1154F50) + game_addr(game::unk_1154F60);
		FramePackets() = TriRender(h + 0x40, OTBase() + 0x44, 2, FramePackets());
	}

	static void DebrisDraw(DirectorNode *d)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		for (int i = 0; i < 0x80; i++)
			if (d->debris[i].life != 0) DebrisDrawOne(&d->debris[i], h);
		FieldFree(0x58);
	}

	static void DebrisStep(Debris *k)
	{
		k->rx = (int16_t)(k->rx + ((int32_t)k->spin[0] << 2));
		k->pos[0] = (int16_t)(k->pos[0] + k->vel[0]);
		k->pos[1] = (int16_t)(k->pos[1] + k->vel[1]);
		int16_t vy = (int16_t)(k->vel[1] - 0xA);
		k->ry = (int16_t)(k->ry + ((int32_t)k->spin[1] << 2));
		k->pos[2] = (int16_t)(k->pos[2] + k->vel[2]);
		if (k->pos[1] >= 0) vy = (int16_t)-((int32_t)vy / 2);
		k->vel[1] = vy;
	}

	static void DebrisMove(DirectorNode *d)
	{
		for (int i = 0; i < 0x80; i++)
		{
			Debris *k = &d->debris[i];
			if (k->life == 0) continue;
			if (--k->life == 0)
			{
				d->cursor = k;
				continue;
			}
			DebrisStep(k);
		}
	}

	static void DrawEntityWorld(uint8_t *e)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x4C);
		FramePackets() = DrawShadow(e, OTBase() + 0x84, 6, FramePackets());
		ComposeAffineTransform(&Camera(), (const Mat4x3 *)(e + 0x40), (Mat4x3 *)h);
		ComputeBonesWorldMatrices(e + 0x60, h);
		U16(h, 0x38) = 0x140;
		U32(h, 0x24) = var<uint32_t>(game_addr(game::g_BattleVertexWorkspace));
		U16(h, 0x34) = 0;
		U16(h, 0x36) = 0;
		U16(h, 0x3A) = 0xD8;
		U32(h, 0x3C) = U32(e, 0x28);
		U32(h, 0x48) = 0;
		U32(h, 0x40) = U32(e, 0x7C);
		U32(h, 0x30) = var<uint32_t>(game_addr(game::BATTLE_PACKET_BUFFER_END));
		FramePackets() = RenderGeometry(U32(e, 0x64), h + 0x20, OTBase() + 0x44, 6, FramePackets());
		BuildBoneMatricesFromPose(e + 0x60);
		uint8_t *w = *(uint8_t **)(e + 0x78);
		if (w)
		{
			U32(h, 0x40) = 0xFFFFFFFF;
			ComputeBonesWorldMatrices(w, h);
			FramePackets() = RenderGeometry(U32(w, 4), h + 0x20, OTBase() + 0x44, 6, FramePackets());
			BuildBoneMatricesFromPose(w);
		}
		FieldFree(0x4C);
	}

	static void DrawEntity(uint8_t *e, const int16_t *angles, const int16_t *off)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x5C);
		Mat4x3 *m = (Mat4x3 *)h;
		FramePackets() = DrawShadow(e, OTBase() + 0x84, 4, FramePackets());
		ComposeZYXRotationMatrix(angles, m);
		m->t[0] = off[0];
		m->t[1] = off[1];
		m->t[2] = off[2];
		ComposeAffineTransform((const Mat4x3 *)(e + 0x40), m, m);
		ComposeAffineTransform(&Camera(), m, m);
		Apply3DTransform(m, off, (int32_t *)(h + 0x20));
		m->t[0] = (int32_t)((uint32_t)m->t[0] - U32(h, 0x20));
		m->t[1] = (int32_t)((uint32_t)m->t[1] - U32(h, 0x24));
		m->t[2] = (int32_t)((uint32_t)m->t[2] - U32(h, 0x28));
		ComputeBonesWorldMatrices(e + 0x60, m);
		U32(h, 0x34) = var<uint32_t>(game_addr(game::g_BattleVertexWorkspace));
		U16(h, 0x44) = 0;
		U16(h, 0x46) = 0;
		U16(h, 0x48) = 0x140;
		U16(h, 0x4A) = 0xD8;
		U32(h, 0x4C) = U32(e, 0x28);
		U32(h, 0x58) = 0;
		U32(h, 0x50) = U32(e, 0x7C);
		U32(h, 0x40) = var<uint32_t>(game_addr(game::BATTLE_PACKET_BUFFER_END));
		FramePackets() = RenderGeometry(U32(e, 0x64), h + 0x30, OTBase() + 0x44, 4, FramePackets());
		BuildBoneMatricesFromPose(e + 0x60);
		uint8_t *w = *(uint8_t **)(e + 0x78);
		if (w)
		{
			U32(h, 0x50) = 0xFFFFFFFF;
			ComputeBonesWorldMatrices(w, m);
			FramePackets() = RenderGeometry(U32(w, 4), h + 0x30, OTBase() + 0x44, 4, FramePackets());
			BuildBoneMatricesFromPose(w);
		}
		FieldFree(0x5C);
	}

	static int32_t MeasureHeight(uint8_t *e, int16_t *out)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x24);
		U32(h, 8) = U32(e, 0x54);
		U32(h, 0xC) = U32(e, 0x58);
		U32(h, 0x10) = U32(e, 0x5C);
		W32(e, 0x54) = W16(e, 0x1C);
		W32(e, 0x58) = W16(e, 0x1E);
		W32(e, 0x5C) = W16(e, 0x20);
		ComputeBonesWorldMatrices(e + 0x60, e + 0x40);
		U32(e, 0x54) = U32(h, 8);
		U32(e, 0x58) = U32(h, 0xC);
		U32(e, 0x5C) = U32(h, 0x10);
		W32(h, 4) = (int32_t)0xFFFF0000;
		W32(h, 0) = 0x10000;
		U32(h, 0x20) = U32(e, 0x7C);
		uint8_t *skel = **(uint8_t ***)(e + 0x64);
		U32(h, 0x14) = 0;
		U16(h, 0x18) = 0x1000;
		uint8_t *model = *(uint8_t **)(e + 0x64);
		const uint8_t *geom = *(uint8_t **)(model + 4);
		const int32_t nobj = *(const int32_t *)geom;
		const uint32_t *offs = (const uint32_t *)(geom + 4);
		for (int32_t i = 0; i < nobj; i++)
		{
			const uint8_t *obj = (const uint8_t *)(offs[i] + *(uint32_t *)(model + 4));
			if (!(U32(h, 0x20) & (1u << i))) continue;
			int32_t groups = *(const int16_t *)obj;
			obj += 2;
			for (; groups > 0; groups--)
			{
				const int32_t bone = *(const int16_t *)obj;
				obj += 2;
				const Mat4x3 *bm = (const Mat4x3 *)(skel + 0x20 + bone * 0x30);
				GteSetRotMatrixCtrl(bm);
				GteSetTransVectorCtrl(bm);
				int32_t nv = *(const int16_t *)obj;
				obj += 2;
				for (; nv != 0; nv--)
				{
					GteLoadV0u(obj);
					GteRTPS();
					obj += 6;
					GteReadIR2Word(h + 0x1C);
					const int32_t y = W16(h, 0x1C);
					if (W32(h, 0) > y) W32(h, 0) = y;
					if (W32(h, 4) < y) W32(h, 4) = y;
				}
			}
		}
		out[0] = W16(h, 4);
		out[1] = W16(h, 0);
		FieldFree(0x24);
		BuildBoneMatricesFromPose(e + 0x60);
		return out[1];
	}

	static int32_t ClearX(int32_t x, int32_t size, int32_t height)
	{
		const int32_t half = size >> 1;
		for (;;)
		{
			const int32_t hi = x + half, lo = x - half;
			bool hit = false;
			for (int32_t i = 0; i < TargetCount() && !hit; i++)
			{
				uint8_t *e = Entity(Targets()[i * 0x18]);
				if (W16(e, 0x3C) <= height) continue;
				const int32_t ex = W16(e, 0x1C);
				const int32_t a = (W16(e, 0x34) >> 1) + ex;
				if (a > lo)
				{
					if (a < hi) hit = true;
				}
				else if ((W16(e, 0x3A) >> 1) + ex > lo) hit = true;
			}
			if (!hit) return x;
			x += 0x80;
		}
	}

	static uint32_t __cdecl RingTask(TaskNode *n)
	{
		RingNode *k = (RingNode *)n;
		ArgA arg;
		ScaledRotY(k->angle, k->scale, &arg.m);
		arg.m.t[0] = k->pos[0];
		arg.m.t[1] = k->pos[1];
		arg.m.t[2] = k->pos[2];
		ComposeAffineTransform(&Camera(), &arg.m, &arg.m);
		arg.morph = TexBase() + 0xF918;
		arg.depth = -0x80;
		arg.kind = (const uint8_t *)game_addr(KIND_Ring);
		arg.fade = 0;
		arg.scroll = 0;
		const int r = prim::play((prim::Layout *)k->layout, CallbackA, (int)&arg, 0);
		return r != 0 ? 0 : TASK_END;
	}

	static void SpawnBoneRing(int32_t bone, int32_t flag, int32_t scale)
	{
		RingNode *k = (RingNode *)AddTaskToQueue(QRings(), game_addr(ORIG_RingTask));
		if (!k) return;
		GetEffectSpawnPosition(Caster(), bone, flag, k->pos);
		k->pos[1] = 0;
		k->scale = (int16_t)scale;
		*(uint32_t *)&k->angle = 0;
		DecodeModelPrimLayout(game_addr(LAYOUT_Ring), k->layout, 0x40);
	}

	static void PlayA(DirectorNode *d, int site, uint8_t *layout, ArgA *arg, bool view)
	{
		if (view) ComposeAffineTransform(&Camera(), &arg->m, &arg->m);
		prim::play((prim::Layout *)layout, CallbackA, (int)arg, 0);
	}
	static void PlayB(DirectorNode *d, int site, uint8_t *layout, ArgB *arg)
	{
		prim::play((prim::Layout *)layout, CallbackB, (int)arg, 0);
	}

	static int32_t FlashLevel(int32_t c)
	{
		if (c < 0xC) return c * 2300 / 12;
		if (c >= 0x10E) return (0x11A - c) * 2300 / 12;
		return 0x8FC;
	}

	static int32_t JumpFlash(int32_t b)
	{
		return 0x1000 - (shl32(mul32(b - 0xA, 449), 10) >> 12);
	}

	static int32_t OpeningRoll(int32_t t)
	{
		const int32_t v = 0x1000 - t;
		int32_t q = (int32_t)(((int64_t)v * 0x6DB6DB6D) >> 32) - v;
		q >>= 3;
		return q + (int32_t)((uint32_t)q >> 31);
	}

	static void DirectorCamera(int32_t c, bool engine)
	{
		if (c == 0)
		{
			if (engine) CameraArmReturn();
			ViewWord() = 0x120;
			CamRoll() = 0;
		}
		const int16_t X = (int16_t)CamAnchor()[0], Z = (int16_t)CamAnchor()[1];
		if ((uint32_t)c < 0x18)
		{
			const int32_t t = (int32_t)(((uint32_t)c << 12) / 23);
			CamRoll() = (int16_t)OpeningRoll(t);
			const int16_t a[6] = { (int16_t)(X - 0x76), (int16_t)0xFDB1, (int16_t)(Z - 0x300), (int16_t)(X + 0x18D), (int16_t)0xFDC4, (int16_t)(Z - 0x1418) };
			const int16_t b[6] = { (int16_t)(X - 0x89), (int16_t)0xFCE6, (int16_t)(Z - 0x88), (int16_t)(X - 0x79), (int16_t)0xFD01, (int16_t)(Z - 0x2F3) };
			CamLerp(a, b, t);
		}
		const int32_t s = c - 0x3D;
		if ((uint32_t)s < 0xE)
		{
			const int32_t t = (int32_t)(((uint32_t)s << 12) / 14);
			if (s == 0)
			{
				CamLookAt()[1] = (int16_t)0xFCB0;
				CamEye()[1] = (int16_t)0xFBFD;
				CamLookAt()[0] = (int16_t)(X - 0x76);
				CamEye()[0] = (int16_t)(X - 0x94);
				CamLookAt()[2] = (int16_t)(Z - 0x318);
				CamEye()[2] = (int16_t)(Z + 0x507);
			}
			CamRoll() = (int16_t)sdiv4096(mul32(t, 1365));
		}
		const int16_t CX = (int16_t)CentreWords()[0], CZ = (int16_t)CentreWords()[1];
		if (c == 0x4B)
		{
			CamRoll() = 0;
			CamLookAt()[0] = (int16_t)(CX - 0x2F);
			CamLookAt()[2] = (int16_t)(CZ - 0x2B6);
			CamEye()[0] = (int16_t)(CX + 0xB53);
			CamLookAt()[1] = (int16_t)0xFE39;
			CamEye()[1] = (int16_t)0xF4DE;
			CamEye()[2] = (int16_t)(CZ + 0x3AB);
		}
		if (c == 0xA3)
		{
			CamRoll() = 0x200;
			CamLookAt()[0] = (int16_t)(CX - 0x105);
			CamLookAt()[2] = (int16_t)(CZ + 0x59C);
			CamEye()[0] = (int16_t)(CX + 0x1E);
			CamLookAt()[1] = (int16_t)0xF955;
			CamEye()[1] = (int16_t)0xFDE1;
			CamEye()[2] = (int16_t)(CZ - 0x1265);
		}
		const int32_t e = c - 0xDD;
		if ((uint32_t)e < 4)
		{
			const int16_t a[6] = { (int16_t)(CX - 0x105), (int16_t)0xF955, (int16_t)(CZ + 0x59C), (int16_t)(CX + 0x1E), (int16_t)0xFDE1, (int16_t)(CZ - 0x1265) };
			const int16_t b[6] = { (int16_t)(CX + 0xCE), (int16_t)0xFC16, (int16_t)(CZ - 0xC49), (int16_t)(CX + 0x35B), 0x3C, (int16_t)(CZ - 0x15D5) };
			const int32_t t = (int32_t)(((uint32_t)e << 12) / 3);
			CamRoll() = (int16_t)(CamRoll() - 0x80);
			CamLerp(a, b, t);
		}
	}

	static void SpawnSideRing(int32_t b)
	{
		RingNode *k = (RingNode *)AddTaskToQueue(QRings(), game_addr(ORIG_RingTask));
		if (!k) return;
		const int16_t x = W16(Caster(), 0x1C);
		k->pos[0] = x;
		k->pos[0] = (int16_t)((b & 1) ? x + 0xC8 : x - 0xC8);
		k->pos[1] = 0;
		k->scale = 0x2000;
		*(uint32_t *)&k->angle = 0;
		k->pos[2] = (int16_t)(W16(Caster(), 0x20) + 0x400);
		DecodeModelPrimLayout(game_addr(LAYOUT_Ring), k->layout, 0x40);
	}

	static uint32_t __cdecl DirectorTask(TaskNode *n)
	{
		DirectorNode *d = (DirectorNode *)n;
		const int16_t c = d->counter;
		SetScreenFlash((uint32_t)FlashLevel(c), 0);
		ArgA arga;
		ArgB argb;
		ff8fx::fill<(sizeof(arga))>(&arga, 0);
		ff8fx::fill<(sizeof(argb))>(&argb, 0);
		const uint32_t morph = TexBase() + 0xF918;
		uint8_t *z = Caster();

		int32_t s = c - 1;
		if ((uint32_t)s < 0x3D)
		{
			if (s == 0) DecodeModelPrimLayout(game_addr(LAYOUT_Anchor), d->layout_a, 0x370);
			GetDefaultEffectPosition(z, argb.pos);
			argb.pos[1] = 0;
			argb.morph = morph;
			argb.depth = -0x80;
			argb.kind = (const uint8_t *)game_addr(KIND_Anchor);
			argb.scroll = (int16_t)(s << 5);
			argb.fade = 0;
			PlayB(d, SITE_ANCHOR, d->layout_a, &argb);
		}
		s = c - 0x3E;
		if ((uint32_t)s < 0xE)
		{
			if (s == 0) DecodeModelPrimLayout(game_addr(LAYOUT_Back), d->layout_b, 0xB0);
			W16(z, 0x20) = (int16_t)(W16(z, 0x20) - 0x100);
			RotationY(0, &arga.m);
			arga.m.t[0] = W16(z, 0x1C);
			arga.m.t[1] = 0;
			arga.m.t[2] = W16(z, 0x20);
			arga.morph = morph;
			arga.depth = -0x80;
			arga.kind = (const uint8_t *)game_addr(KIND_Walk);
			arga.scroll = (int16_t)(s << 5);
			arga.fade = 0;
			PlayA(d, SITE_BACK, d->layout_b, &arga, true);
			const uint32_t r = (uint32_t)s % 6;
			if (r == 2) SpawnBoneRing(0x14, 0x1000, 0x1000);
			else if (r == 5) SpawnBoneRing(0x13, 0x1000, 0x1000);
		}
		s = c - 0x4C;
		if ((uint32_t)s < 0x28)
		{
			if (s == 0)
			{
				QRings()->tail = nullptr;
				QRings()->head = nullptr;
				DecodeModelPrimLayout(game_addr(LAYOUT_Run), d->layout_b, 0x188);
				W16(z, 0x20) = (int16_t)(d->centre[2] + 0x1800);
				W32(z, 0x5C) = W16(z, 0x20);
				W16(z, 0x20) = (int16_t)(W16(z, 0x20) - 0x4CC);
			}
			else if ((uint32_t)s < 0xA)
			{
				W16(z, 0x20) = (int16_t)(W16(z, 0x20) - 0x4CC);
			}
			else
			{
				B8(z, 0) |= 8;
			}
			if ((uint32_t)s >= 0x14)
			{
				for (int32_t i = 0; i < TargetCount(); i++)
				{
					uint8_t *e = d->recs[i].e;
					const int32_t dx = (W16(e, 0x1C) - W16(z, 0x1C)) >> 2;
					if (dx > -0x400 && dx < 0x400)
					{
						if (dx > 0) W16(e, 0xE) = (int16_t)(W16(e, 0xE) + dx - 0x400);
						else W16(e, 0xE) = (int16_t)(W16(e, 0xE) + dx + 0x400);
					}
				}
			}
			RotationY(0, &arga.m);
			arga.m.t[0] = W16(z, 0x1C);
			arga.m.t[1] = 0;
			arga.m.t[2] = W16(z, 0x20);
			arga.morph = morph;
			arga.depth = -0x80;
			arga.kind = (const uint8_t *)game_addr(KIND_Walk);
			arga.scroll = (int16_t)(s << 6);
			arga.fade = 0;
			PlayA(d, SITE_RUN, d->layout_b, &arga, true);
		}
		s = c - 0x74;
		if ((uint32_t)s < 0x30)
		{
			if (s == 0)
			{
				for (int k = 0; k < 4; k++) var<uint8_t>(game_addr(game::unk_1D98991) + k * 0x2C) &= 0xFD;
				for (int32_t i = 0; i < TargetCount(); i++)
				{
					TargetRec &r = d->recs[i];
					B8(r.e, 0) |= 4;
					U32(r.e, 0xC) = r.rot01;
					U32(r.e, 0x10) = r.rot23;
				}
				DecodeModelPrimLayout(game_addr(LAYOUT_Screen), d->layout_b, 0x94);
			}
			RotationY(0, &arga.m);
			arga.morph = morph;
			const int32_t k = (int32_t)(((uint32_t)s << 12) / 48);
			arga.m.t[0] = 0;
			arga.m.t[1] = 0;
			arga.m.t[2] = 0xAF0;
			arga.depth = 0;
			arga.kind = (const uint8_t *)game_addr(KIND_Screen);
			arga.scroll = (int16_t)(k + 0x4D);
			arga.fade = (uint32_t)s > 0x2C ? (int16_t)((s << 10) + 0x5200) : (int16_t)0;
			PlayA(d, SITE_SCREEN, d->layout_b, &arga, false);
		}
		s = c - 0xA4;
		if ((uint32_t)s < 0x12)
		{
			if (s == 0) DecodeModelPrimLayout(game_addr(LAYOUT_Far), d->layout_b, 0x154);
			RotationY(0, &arga.m);
			arga.m.t[0] = W16(z, 0x1C);
			arga.m.t[1] = 0;
			arga.m.t[2] = (int32_t)d->centre[2] + 0x5DC0;
			arga.morph = morph;
			arga.depth = 0;
			arga.kind = (const uint8_t *)game_addr(KIND_Plain);
			arga.fade = 0;
			PlayA(d, SITE_FAR, d->layout_b, &arga, true);
		}
		const int32_t b = c - 0xB6;
		if ((uint32_t)b < 0x28)
		{
			arga.depth = 0;
			if ((uint32_t)b < 8) arga.fade = (int16_t)((8 - b) << 9);
			else if ((uint32_t)b > 0x16) arga.fade = (int16_t)((b - 0x16) << 9);
			else arga.fade = 0;
			if (b == 0)
			{
				W16(z, 0x20) = (int16_t)(d->centre[2] + 0x5AA0);
				DecodeModelPrimLayout(game_addr(LAYOUT_Jump), d->layout_b, 0xBC);
				DecodeModelPrimLayout(game_addr(LAYOUT_JumpBone), d->layout_c, 0x27C);
				InitTaskQueuePool(QRings(), (void *)(TexBase() + 0x1400), 0x294, 0x24);
			}
			if ((uint32_t)b < 0x1E)
			{
				DrawEntityWorld(z);
				B8(z, 0) |= 4;
				arga.depth = (int16_t)0xF800;
				if ((uint32_t)b < 0x14) W16(z, 0x20) = (int16_t)(W16(z, 0x20) - 0x320);
				else if ((uint32_t)b < 0x19) W16(z, 0x20) = (int16_t)(W16(z, 0x20) - 0x640);
				else if ((uint32_t)b < 0x1D)
				{
					if (b == 0x19) W16(z, 0x20) = (int16_t)(W16(z, 0x20) - 0x400);
					int32_t v = shl32(b - 0x19, 12);
					v = (int32_t)((uint32_t)v + ((uint32_t)(v >> 31) & 3u)) >> 2;
					W16(z, 0x20) = (int16_t)(W16(z, 0x20) - (mul32(0xE00 - v, 600) >> 12));
				}
				if (b == 0x1A)
				{
					for (int32_t i = 0; i < TargetCount(); i++)
					{
						uint8_t *e = Entity(Targets()[i * 0x18]);
						RingNode *k = (RingNode *)AddTaskToQueue(QRings(), game_addr(ORIG_RingTask));
						if (!k) break;
						GetDefaultEffectPosition(e, k->pos);
						*(uint32_t *)&k->angle = 0x800;
						k->scale = (int16_t)(((int32_t)W16(e, 0x26) / 2) * 2 + 0x800);
						DecodeModelPrimLayout(game_addr(LAYOUT_TargetRing), k->layout, 0x160);
					}
				}
				else if ((uint32_t)b < 0x19)
				{
					int32_t count = 4;
					Debris *k = DebrisAlloc(d);
					while (k)
					{
						k->pos[0] = W16(z, 0x1C);
						k->pos[1] = 0;
						k->life = 0xF;
						k->pos[2] = (int16_t)(W16(z, 0x20) + 0x400);
						int32_t vx = (CrtRand() & 0x3F) + 0x3F;
						k->vel[0] = (int16_t)vx;
						if (count & 1) k->vel[0] = (int16_t)-vx;
						k->vel[1] = (int16_t)(-0x50 - (CrtRand() & 0x3F));
						k->vel[2] = (int16_t)((CrtRand() & 0x3F) + 0x3F);
						k->scale = (int16_t)((CrtRand() & 0xFFF) + 0x400);
						if (--count == 0) break;
						k = DebrisAlloc(d);
					}
					if (b == 0xA) *(uint16_t *)z &= 0xFFF7;
				}
				if ((uint32_t)b < 0x1D) SpawnSideRing(b);
				if (b == 0xA)
					for (int k = 0; k < 4; k++) var<uint8_t>(game_addr(game::unk_1D98991) + k * 0x2C) |= 2;
				if ((uint32_t)b >= 0xA && (uint32_t)b <= 0x1A) SetScreenFlash((uint32_t)JumpFlash(b), 0);
			}
			else if (b == 0x1E)
			{
				*(uint16_t *)z &= 0xFFFB;
			}
			else
			{
				arga.depth = (int16_t)0xF800;
			}
			if (arga.fade < 0x1000)
			{
				RotationY(0, &arga.m);
				arga.m.t[0] = W16(z, 0x1C);
				arga.m.t[1] = 0;
				arga.m.t[2] = W16(z, 0x20);
				arga.morph = morph;
				arga.kind = (const uint8_t *)game_addr(KIND_Jump);
				arga.scroll = (int16_t)(b << 5);
				PlayA(d, SITE_JUMP, d->layout_b, &arga, true);
				if ((uint32_t)b >= 0xA)
				{
					int16_t p[4];
					GetEffectSpawnPosition(z, 0x16, 0, p);
					RotationY(0, &arga.m);
					arga.m.t[0] = p[0];
					arga.m.t[1] = p[1];
					arga.m.t[2] = p[2];
					arga.kind = (const uint8_t *)game_addr(KIND_Plain);
					PlayA(d, SITE_JUMPBONE, d->layout_c, &arga, true);
				}
			}
		}
		s = c - 0xDE;
		if ((uint32_t)s < 0x20)
		{
			if (s == 0) DecodeModelPrimLayout(game_addr(LAYOUT_Crater), d->layout_a, 0x154);
			RotationY(0x800, &arga.m);
			arga.m.t[0] = d->centre[0];
			arga.m.t[1] = 0;
			arga.m.t[2] = (int32_t)d->centre[2] + 0x400;
			arga.morph = morph;
			arga.depth = -0x80;
			arga.kind = (const uint8_t *)game_addr(KIND_Crater);
			arga.scroll = (int16_t)(s << 5);
			arga.fade = 0;
			PlayA(d, SITE_CRATER, d->layout_a, &arga, true);
		}
		else if (s == 0x20)
		{
			*(uint16_t *)z &= 0xFFFB;
		}
		const int32_t t = c - 0xDE;
		if ((uint32_t)t < 0x28)
		{
			const int32_t k = (int32_t)(((uint32_t)t << 12) / 40);
			int32_t y;
			if (k < 0xC00) y = SqrtScaled(ComputeSin(k >> 1));
			else
			{
				const int32_t v = mul32(0x1000 - k, 3444);
				y = (int32_t)((uint32_t)v + ((uint32_t)(v >> 31) & 0x3FFu)) >> 10;
			}
			y = shl32(-y, 1);
			int16_t off[3];
			off[2] = 0;
			off[0] = 0;
			if (t == 0)
			{
				DecodeModelPrimLayout(game_addr(LAYOUT_Crater), d->layout_a, 0x154);
				for (int32_t i = 0; i < TargetCount(); i++)
				{
					uint8_t *e = d->recs[i].e;
					MeasureHeight(e, d->recs[i].height);
					B8(e, 0) |= 8;
				}
			}
			for (int32_t i = 0; i < TargetCount(); i++)
			{
				TargetRec &r = d->recs[i];
				const int16_t ry = (int16_t)(r.pos01 >> 16);
				W16(r.e, 0x1E) = (int16_t)(ry - (int16_t)sdiv4096(mul32(r.height[0], k)) + y);
				off[1] = (int16_t)(((int32_t)r.height[1] + (int32_t)r.height[0]) >> 1);
				DrawEntity(r.e, r.ang, off);
				r.ang[0] = (int16_t)(r.ang[0] + ((i & 1) ? 0xCC : -0xCC));
				B8(r.e, 0) |= 4;
			}
		}
		else
		{
			if ((uint32_t)t < 0x3B)
			{
				for (int32_t i = 0; i < TargetCount(); i++)
				{
					TargetRec &r = d->recs[i];
					const int16_t ry = (int16_t)(r.pos01 >> 16);
					W16(r.e, 0x1E) = ry;
					MeasureHeight(r.e, r.height);
					W16(r.e, 0x1E) = (int16_t)(ry - r.height[0]);
					DrawEntity(r.e, ZeroAngles(), ZeroAngles());
					B8(r.e, 0) |= 4;
				}
			}
			if (t == 0x28)
			{
				for (int32_t i = 0; i < TargetCount(); i++) *(uint16_t *)d->recs[i].e &= 0xFFF7;
				StreamQueueLoad(0x168, TexBase() + 0x7118, 1);
				StreamQueueLoad(0x169, TexBase() + 0x7118, 1);
				StreamQueueLoad(0x16A, TexBase() + 0x7118, 0);
			}
		}
		W32(z, 0x54) = W16(z, 0x1C);
		W32(z, 0x58) = W16(z, 0x1E);
		W32(z, 0x5C) = W16(z, 0x20);
		s = c - 0x18;
		if ((uint32_t)s < 0x5C)
		{
			if (s == 0) DecodeModelPrimLayout(game_addr(LAYOUT_Bone), d->layout_c, 0x1B4);
			GetEffectSpawnPosition(z, 0x16, 0, argb.pos);
			argb.morph = morph;
			argb.depth = -0x80;
			argb.kind = (const uint8_t *)game_addr(KIND_Plain);
			argb.fade = 0;
			PlayB(d, SITE_BONE, d->layout_c, &argb);
		}
		DebrisDraw(d);
		DebrisMove(d);
		if (c == 5) BdPlaySE(SOUND_A(), 0, 0x80);
		else if (c == 0xA2) BdPlaySE(SOUND_B(), 0, 0x80);
		else if (c == 0x108) ApplyActionResultToTargets(Targets(), (uint32_t)TargetCount());
		if (c == 0xA3)
		{
			const int16_t mz = (int16_t)-d->centre[2];
			d->centre[0] = 0;
			for (int32_t i = 0; i < TargetCount(); i++)
			{
				uint8_t *e = d->recs[i].e;
				*(uint16_t *)e &= 0xFFFB;
				W16(e, 0x20) = (int16_t)(W16(e, 0x20) + mz);
			}
		}
		DirectorCamera(c, true);
		if (++d->counter >= 0x11A)
		{
			for (int32_t i = 0; i < TargetCount(); i++) *(uint16_t *)d->recs[i].e &= 0xFFF3;
			*(uint16_t *)Entity(0) &= 0xFFFB;
			*(uint16_t *)Entity(1) &= 0xFFFB;
			*(uint16_t *)Entity(2) &= 0xFFFB;
			SetScreenFlash(0, 0);
			return TASK_END;
		}
		return 0;
	}

	static void StartDirector(DirectorNode *d)
	{
		Memset32(&d->counter, 0, 0x4FD);
		int16_t minx = 0x7FFF, minz = 0x7FFF, maxx = (int16_t)0x8001, maxz = (int16_t)0x8001;
		for (int32_t i = 0; i < TargetCount(); i++)
		{
			TargetRec &r = d->recs[i];
			r.e = Entity(Targets()[i * 0x18]);
			r.pos01 = U32(r.e, 0x1C);
			r.pos23 = U32(r.e, 0x20);
			r.rot01 = U32(r.e, 0xC);
			r.rot23 = U32(r.e, 0x10);
			*(uint32_t *)&r.ang[0] = *(const uint32_t *)ZeroAngles();
			*(uint32_t *)&r.ang[2] = *(const uint32_t *)(ZeroAngles() + 2);
			int16_t p[4];
			GetDefaultEffectPosition(r.e, p);
			if (p[0] < minx) minx = p[0];
			if (p[0] > maxx) maxx = p[0];
			if (p[2] < minz) minz = p[2];
			if (p[2] > maxz) maxz = p[2];
		}
		(void)minz;
		const int16_t cx = (int16_t)(((int32_t)maxx + (int32_t)minx) / 2);
		d->centre[0] = cx;
		d->centre[1] = 0;
		d->centre[2] = maxz;
		uint8_t *z = Caster();
		const int16_t x = (int16_t)ClearX(cx, W16(z, 0x26), W16(z, 0x92));
		d->centre[0] = x;
		d->saved_pos[0] = U32(z, 0x1C);
		d->saved_pos[1] = U32(z, 0x20);
		d->saved_rot[0] = U32(z, 0xC);
		d->saved_rot[1] = U32(z, 0x10);
		W16(z, 0x1C) = x;
		W16(z, 0x20) = (int16_t)(d->centre[2] + 0x1000);
		W16(z, 0xE) = 0;
		CamAnchor()[0] = U32(z, 0x1C);
		CamAnchor()[1] = U32(z, 0x20);
		CentreWords()[0] = *(const uint32_t *)&d->centre[0];
		CentreWords()[1] = *(const uint32_t *)&d->centre[2];
		DebrisInit(d);
		BindExternalAnimSet(z, d->bind, game_addr(COM_Anim));
		QueueChainTransformation(z, 1);
		StreamStateInit((void *)(TexBase() + 0x70D0));
		*(uint16_t *)Entity(0) |= 4;
		*(uint16_t *)Entity(1) |= 4;
		*(uint16_t *)Entity(2) |= 4;
		*(uint16_t *)z &= 0xFFFB;
	}

	static uint32_t __cdecl RootTask(TaskNode *n)
	{
		RootNode *r = (RootNode *)n;
		if (r->parity)
		{
			PacketCursor() = TexBase() + 0x10518;
			PacketCursor2() = TexBase() + 0x22518;
			r->parity = 0;
		}
		else
		{
			PacketCursor() = TexBase() + 0x22518;
			PacketCursor2() = TexBase() + 0x34518;
			r->parity = 1;
		}
		if (r->counter == 1)
		{
			if (r->state == 0)
			{
				r->state = 1;
				InitTaskQueuePool(QDirector(), (void *)TexBase(), 0x1400, 1);
				InitTaskQueuePool(QRings(), (void *)(TexBase() + 0x1400), 0x294, 0x24);
			}
			DirectorNode *d = (DirectorNode *)AddTaskToQueue(QDirector(), game_addr(ORIG_DirectorTask));
			if (d) StartDirector(d);
		}
		int a = 1;
		if (r->state)
		{
			a = ExecuteTaskQueue(QDirector());
			ExecuteTaskQueue(QRings());
		}
		if (r->state)
		{
			StreamPump();
			if (a == 0)
			{
				if (StreamPreload(2) == 0) return 0;
				const DirectorNode *d = (const DirectorNode *)TexBase();
				for (int32_t i = 0; i < TargetCount(); i++)
				{
					const TargetRec &t = d->recs[i];
					U32(t.e, 0x1C) = t.pos01;
					U32(t.e, 0x20) = t.pos23;
					U32(t.e, 0xC) = t.rot01;
					U32(t.e, 0x10) = t.rot23;
				}
				QueueChainTransformation(Caster(), 2);
				U32(Caster(), 0x1C) = d->saved_pos[0];
				U32(Caster(), 0x20) = d->saved_pos[1];
				U32(Caster(), 0xC) = d->saved_rot[0];
				U32(Caster(), 0x10) = d->saved_rot[1];
				*(uint16_t *)Caster() &= 0xFFF7;
				ff8fx::copy<(0x10)>((void *)game_addr(game::Battle_Camera_world_XZ_s16), (const void *)game_addr(game::Battle_Camera_ReturnView_world_XZ_s16));
				ViewWord() = var<int16_t>(game_addr(game::Battle_Camera_ReturnView_Param1D8E038));
				return TASK_END;
			}
		}
		const int16_t old = r->counter;
		r->counter = (int16_t)(old + 1);
		if (old > 0x100) r->counter = 0x100;
		return 0;
	}
}

	void register_mag215_my_final_heaven()
	{
		register_port(heaven215::ORIG_RootTask, (void *)heaven215::RootTask, "215 RootTask", 215);
		register_port(heaven215::ORIG_DirectorTask, (void *)heaven215::DirectorTask, "215 DirectorTask", 215);
		register_port(heaven215::ORIG_RingTask, (void *)heaven215::RingTask, "215 RingTask", 215);
	}
}

