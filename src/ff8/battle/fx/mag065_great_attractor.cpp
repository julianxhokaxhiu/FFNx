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
namespace ga065
{
	using namespace eng;
	using namespace magc;

	inline int32_t &Scale() { return var<int32_t>(game_addr(game::g_GreatAttractor_OverlayScale)); }
	inline int32_t &Pause() { return var<int32_t>(game_addr(game::g_GreatAttractor_DebugPause)); }
	inline uint8_t *&Ctx() { return var<uint8_t *>(game_addr(game::g_GreatAttractor_CastContext)); }
	inline uint8_t *&Caster() { return var<uint8_t *>(game_addr(game::g_GreatAttractor_Caster)); }
	inline TaskQueue *QLayouts() { return (TaskQueue *)game_addr(game::g_GreatAttractor_LayoutQueue); }
	inline TaskQueue *QMaster() { return (TaskQueue *)game_addr(game::g_GreatAttractor_MasterQueue); }
	inline uint8_t *&SegCursor() { return var<uint8_t *>(game_addr(game::unk_2544FBC)); }
	inline uint8_t *&ChainCursor() { return var<uint8_t *>(game_addr(game::unk_2544FC0)); }
	inline uint8_t *&PModelCursor() { return var<uint8_t *>(game_addr(game::unk_2544FC4)); }
	inline uint8_t *&QuadCursor() { return var<uint8_t *>(game_addr(game::unk_2544FC8)); }
	inline uint8_t *&SpriteCursor() { return var<uint8_t *>(game_addr(game::unk_2544FCC)); }
	inline uint8_t *Flags() { return (uint8_t *)game_addr(game::g_GreatAttractor_ParticleFlags); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::g_GreatAttractor_PacketCursor)); }
	inline int16_t *CasterPos() { return (int16_t *)game_addr(game::g_GreatAttractor_CasterPos); }
	inline int16_t *Centre() { return (int16_t *)game_addr(game::g_GreatAttractor_TargetsCentre); }
	inline int16_t &NoiseShift() { return var<int16_t>(game_addr(game::unk_2544FE8)); }
	inline int16_t &NoiseHalf() { return var<int16_t>(game_addr(game::unk_2544FEA)); }
	inline int16_t &NoiseKeep() { return var<int16_t>(game_addr(game::unk_2544FEC)); }
	inline int16_t &RibbonDepth() { return var<int16_t>(game_addr(game::unk_2544FEE)); }
	inline int32_t &Lcg() { return var<int32_t>(game_addr(game::g_GreatAttractor_Lcg)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::g_GreatAttractor_TexBase)); }
	inline uint32_t *SavedFlags() { return (uint32_t *)game_addr(game::g_GreatAttractor_SavedEntityFlags); }
	inline uint32_t &FrameArena() { return var<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)); }
	inline int16_t &CameraZoom() { return var<int16_t>(game_addr(game::g_BattleCam_Roll)); }
	inline uint32_t OT(uint32_t off) { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + off; }

	static constexpr game::Id ORIG_RootTask = game::MAG_065_GreatAttractor_RootTask;
	static constexpr game::Id ORIG_MasterTask = game::MAG_065_GreatAttractor_MasterTask;
	static constexpr game::Id ORIG_LayoutTask = game::MAG_065_GreatAttractor_LayoutTask;
	static constexpr game::Id SINCOS = game::unk_133ED20;
	static constexpr game::Id DIST_TABLE = game::unk_133AD20;
	static constexpr game::Id OBJS0 = game::a00000000000000_2;
	static constexpr game::Id OBJS23 = game::a23000000000000;
	static constexpr game::Id STARS = game::unk_133ABC8;
	static constexpr game::Id BONE_R = game::unk_133ABB0;
	static constexpr game::Id BONE_L = game::unk_133ABB8;
	static constexpr game::Id ENTITY_SCALE = game::unk_133AB88;
	static constexpr game::Id SPRITE_SEQ = game::unk_1334F0C;
	static const void *SOUND_Voice() { return (const void *)game_addr(game::unk_1334D70); }

	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline void RotY(int32_t angle, void *out) { fn<void (__cdecl *)(int32_t, void *)>(game_addr(game::Effect_RotationYMatrix))(angle, out); }
	inline void RotX(int32_t angle, void *out) { fn<void (__cdecl *)(int32_t, void *)>(game_addr(game::MAG_063_RotationXMatrix))(angle, out); }
	inline void RotZ(int32_t angle, void *out) { fn<void (__cdecl *)(int32_t, void *)>(game_addr(game::MAG_065_RotationZMatrix))(angle, out); }
	inline void RotationFromAngles(const int16_t *angles, void *out) { fn<void (__cdecl *)(const int16_t *, void *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void RotationFromAnglesB(const int16_t *angles, void *out) { fn<void (__cdecl *)(const int16_t *, void *)>(game_addr(game::MAG_063_RotationFromAnglesYXZ))(angles, out); }
	inline void RotationFromAnglesC(const int16_t *angles, void *out) { fn<void (__cdecl *)(const int16_t *, void *)>(game_addr(game::MAG_065_RotationFromAnglesXZY))(angles, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
	inline void MatMulScaleDiag(void *a, const int16_t *b) { fn<void (__cdecl *)(void *, const int16_t *)>(game_addr(game::MatrixMultiply3x3))(a, b); }
	inline void MatrixMultiply(const void *a, void *b) { fn<void (__cdecl *)(const void *, void *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void MatMul3(const void *a, const void *b, void *out) { fn<void (__cdecl *)(const void *, const void *, void *)>(game_addr(game::Matrix3x3_MultiplyPSX))(a, b, out); }
	inline void MatVec(const void *m, const int16_t *in, int16_t *out) { fn<void (__cdecl *)(const void *, const int16_t *, int16_t *)>(game_addr(game::matrixMultiplyVector))(m, in, out); }
	inline void Apply3D(const void *m, const int16_t *in, int32_t *out) { fn<void (__cdecl *)(const void *, const int16_t *, int32_t *)>(game_addr(game::apply3DTransform))(m, in, out); }
	inline void TransformVec3(const void *m, const int32_t *in, int32_t *out) { fn<void (__cdecl *)(const void *, const int32_t *, int32_t *)>(game_addr(game::TransformVectorBy3x3Matrix))(m, in, out); }
	inline int32_t OrthoBasis(const int32_t *v, void *m) { return fn<int32_t (__cdecl *)(const int32_t *, void *)>(game_addr(game::BuildOrthonormalBasis))(v, m); }
	inline int32_t ISqrt(int32_t v) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::Math_SqrtInt))(v); }
	inline int32_t ComputeCos(int32_t angle) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(angle); }
	inline void DirectionFromAngles(int32_t pitch, int32_t yaw, int16_t *out) { fn<void (__cdecl *)(int32_t, int32_t, int16_t *)>(game_addr(game::Effect_DirectionFromPitchYaw))(pitch, yaw, out); }
	inline uint32_t RenderPrimSet(void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::MAG_063_RenderPrimSet))(h, ot, mode, cursor); }
	inline uint32_t Tile(int32_t r, int32_t g, int32_t b, int32_t level, uint32_t cursor) { return fn<uint32_t (__cdecl *)(int32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_FullScreenTile))(r, g, b, level, cursor); }
	inline uint32_t Flipbook(void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::MAG_065_RenderFlipbook))(h, ot, mode, cursor); }
	inline void CameraKeys(const int16_t *k0, const int16_t *k1, int32_t za, int32_t zb, int32_t i, int32_t n) { fn<void (__cdecl *)(const int16_t *, const int16_t *, int32_t, int32_t, int32_t, int32_t)>(game_addr(game::Effect_CameraKeysLerp))(k0, k1, za, zb, i, n); }
	inline void PartyShadowOn() { fn<void (__cdecl *)()>(game_addr(game::MAG_070_ShockwavePulsar_EntitiesFlag4On))(); }
	inline void EnemyShadowOn() { fn<void (__cdecl *)()>(game_addr(game::MAG_065_SetEnemyEntitiesFlag4))(); }
	inline void ModelsFlag2On() { fn<void (__cdecl *)()>(game_addr(game::MAG_065_SetStageGroupsBit1))(); }
	inline void RequestScreenFeedback(int32_t a, int32_t b, int32_t c, int32_t d, int32_t e) { fn<void (__cdecl *)(int32_t, int32_t, int32_t, int32_t, int32_t)>(game_addr(game::Battle_RequestScreenFeedback))(a, b, c, d, e); }
	inline void QueueChainTransformation(uint8_t *entity, int32_t a) { fn<void (__cdecl *)(uint8_t *, int32_t)>(game_addr(game::QueueChainTransformation))(entity, a); }
	inline void CameraArmReturn() { fn<void (__cdecl *)()>(game_addr(game::BS_Camera_ArmReturnAfterEffect))(); }
	inline void ReleaseVoice(uint32_t slot) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::BdSound_ReleaseVoiceSlot))(slot); }
	inline void StreamLoad(int32_t id, uint32_t dst, int32_t kind) { fn<void (__cdecl *)(int32_t, uint32_t, int32_t)>(game_addr(game::Stream_QueueLoad))(id, dst, kind); }
	inline int32_t StreamBusy() { return fn<int32_t (__cdecl *)()>(game_addr(game::Stream_IsBusy))(); }
	inline void StreamPump() { fn<void (__cdecl *)()>(game_addr(game::Stream_Pump))(); }
	inline int32_t StreamPending(int32_t a) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::au_re_BattleFile_preLoad_1))(a); }
	inline void InsertPrimAltViewport(uint32_t bucket, void *packet) { fn<void (__cdecl *)(uint32_t, void *)>(game_addr(game::SSIGPU_InsertPrimAltViewport))(bucket, packet); }
	inline void GteLoadIRBytes(const void *src) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadIR123FromBytes))(src); }
	inline void GteStoreIRBytes(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreIR123LowBytes))(dst); }
	inline void GteLoadIRColumn(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadIRFromMatrixColumn))(m); }
	inline void GteStoreIRColumn(void *m) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreIRToMatrixColumn))(m); }
	inline void GteMVMVA_RotIR() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotIR))(); }
	inline void GteMVMVA_RotV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0))(); }
	inline void GteLoadV0Dwords(const void *v) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadV0FromDwords))(v); }
	inline void GteLoadV012(const void *a, const void *b, const void *c) { fn<void (__cdecl *)(const void *, const void *, const void *)>(game_addr(game::GTE_LoadV012))(a, b, c); }
	inline void GteRTPT() { fn<void (__cdecl *)()>(game_addr(game::GTE_RTPT))(); }
	inline void GteReadFLAG(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadFLAG))(dst); }
	inline void GteNCLIP() { fn<void (__cdecl *)()>(game_addr(game::GTE_NCLIP))(); }
	inline void GteReadMAC0(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadMAC0))(dst); }
	inline void GteReadSXY012(void *a, void *b, void *c) { fn<void (__cdecl *)(void *, void *, void *)>(game_addr(game::GTE_ReadSXY012_Split))(a, b, c); }
	inline void GteAVSZ3() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ3))(); }
	inline void GteAVSZ4() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ4))(); }
	inline void GteReadOTZWord(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadOTZ))(dst); }

	inline uint32_t mulhi_u(uint32_t a, uint32_t b) { return (uint32_t)(((uint64_t)a * b) >> 32); }
	inline int32_t mulhi_s(int32_t a, int32_t b) { return (int32_t)(((int64_t)a * b) >> 32); }
	inline int32_t sdiv_pow2(int32_t v, int n) { return (int32_t)((uint32_t)v + ((uint32_t)(v >> 31) & ((1u << n) - 1))) >> n; }
	inline int16_t SinTab(int32_t i) { return *(const int16_t *)(game_addr(SINCOS) + (uint32_t)i * 4); }
	inline int16_t CosTab(int32_t i) { return *(const int16_t *)(game_addr(SINCOS) + (uint32_t)i * 4 + 2); }

#pragma pack(push, 1)
	struct RootNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t pad0E;
		uint8_t started;
		uint32_t parity;
	};
	struct Target
	{
		uint8_t *entity;
		uint32_t pos_xy;
		uint32_t pos_z;
	};
	struct MasterNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t ntargets;
		uint32_t voice;
		uint32_t caster_xy;
		uint32_t caster_z;
		int16_t centre[3];
		int16_t pad22;
		Target targets[3];
		uint8_t layout_a[0x9C4];
		uint8_t layout_b[0x3AC];
	};
	struct LayoutNode
	{
		TaskNode hdr;
		int16_t pos[3];
		int16_t depth;
		int32_t scale;
		uint8_t layout[0x1A0];
	};
	struct PrimArg
	{
		Mat4x3 m;
		uint32_t objs;
		uint32_t cursor;
		int16_t depth;
		int16_t scroll;
		uint32_t morph;
	};
	struct Star
	{
		int16_t pos[3];
		int16_t dist;
		uint8_t rgb[3];
		uint8_t level;
		uint8_t rgb2[3];
		uint8_t speed;
	};
	struct PModel
	{
		int16_t pos[3];
		uint8_t life;
		int8_t model;
		int16_t vel[3];
		int16_t scale;
		int16_t rot[2];
		int8_t drot[2];
	};
	struct QuadP
	{
		int16_t pos[3];
		int16_t life;
		int16_t m[9];
		int16_t vel[3];
	};
	struct SpriteP
	{
		int16_t pos[3];
		int16_t life;
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x14 && sizeof(MasterNode) == 0xDB8 && sizeof(LayoutNode) == 0x1B8, "Attractor 065 nodes");
	static_assert(sizeof(PrimArg) == 0x30 && sizeof(Star) == 0x10, "Attractor 065 blocks");
	static_assert(sizeof(PModel) == 0x16 && sizeof(QuadP) == 0x20 && sizeof(SpriteP) == 8, "Attractor 065 particles");
}
}

namespace ff8fx
{
namespace ga065
{
	using namespace eng;
	using namespace magc;

	static int32_t NextLcg()
	{
		const int32_t v = (int32_t)(((uint32_t)Lcg() * 125u + 0xEu) & 0x7FFF);
		Lcg() = v;
		return v;
	}

	static void Polar(int32_t angle, int32_t radius, int16_t *out)
	{
		out[0] = (int16_t)(mul32(ComputeSin(angle), radius) >> 12);
		out[2] = (int16_t)(mul32(ComputeCos(angle), radius) >> 12);
	}

	static void ModelsFlag2Off()
	{
		for (uint32_t a = game_addr(game::unk_1D98991); a < game_addr(game::unk_1D98A41); a += 0x2C) *(uint8_t *)a &= 0xFD;
	}

	static void SaveEntityFlags()
	{
		uint32_t *dst = SavedFlags();
		for (uint32_t e = game_addr(game::BattleEntitySlotData); e < game_addr(game::CAMERA_FLAG_RELATED); e += 0x9C) *dst++ = *(const uint16_t *)e;
	}

	static void RestoreEntityFlag4()
	{
		const uint32_t *src = SavedFlags();
		for (uint32_t e = game_addr(game::BattleEntitySlotData); src < (const uint32_t *)game_addr(game::unk_2545014); e += 0x9C, src++)
		{
			const uint16_t f = *(const uint16_t *)e;
			if (f & 2) *(uint16_t *)e = (uint16_t)((((uint8_t)*src ^ (uint32_t)f) & 4) ^ f);
		}
	}

	static void Feedback() { RequestScreenFeedback(0, 0xFF, 0xFF, 0xFF, 0x3F); }

	static int32_t FadeLevel(int32_t t, int32_t in, int32_t hold, int32_t out)
	{
		if (t < in) return shl32(t + 1, 12) / in;
		if (t < in + hold) return 0x1000;
		return (shl32(in - t + hold, 12) - 0x1000) / out + 0x1000;
	}

	static void FadeTile(int32_t t, int32_t r, int32_t g, int32_t b, int32_t in, int32_t hold, int32_t out)
	{
		FrameArena() = Tile(r, g, b, FadeLevel(t, in, hold, out), FrameArena());
	}

	static void RotYScaled(int32_t angle, int32_t s, Mat4x3 *m)
	{
		uint32_t *d = (uint32_t *)m;
		const int32_t a = (int16_t)angle, sc = (int16_t)s;
		d[0] = d[1] = d[2] = d[3] = d[4] = 0;
		const int32_t sn = mul32((int16_t)ComputeSin(a), sc) >> 12;
		const int32_t cs = mul32((int16_t)ComputeCos(a), sc) >> 12;
		m->m[2][0] = (int16_t)sn;
		m->m[1][1] = (int16_t)s;
		m->m[0][0] = (int16_t)cs;
		m->m[0][2] = (int16_t)-sn;
		m->m[2][2] = (int16_t)cs;
	}

	static void RotXScaledTab(int32_t angle, int32_t s, Mat4x3 *m)
	{
		uint32_t *d = (uint32_t *)m;
		d[0] = d[1] = d[2] = d[3] = d[4] = 0;
		const int32_t i = angle & 0xFFF, sc = (int16_t)s;
		const int32_t sn = mul32(SinTab(i), sc) >> 12;
		const int32_t cs = mul32(CosTab(i), sc) >> 12;
		m->m[0][0] = (int16_t)s;
		m->m[2][1] = (int16_t)-sn;
		m->m[1][1] = (int16_t)cs;
		m->m[1][2] = (int16_t)sn;
		m->m[2][2] = (int16_t)cs;
	}

	static void RotYTab(int32_t angle, Mat4x3 *m)
	{
		uint32_t *d = (uint32_t *)m;
		d[0] = d[1] = d[2] = d[3] = d[4] = 0;
		const int32_t i = angle & 0xFFF;
		const int16_t sn = SinTab(i), cs = CosTab(i);
		m->m[2][0] = sn;
		m->m[0][0] = cs;
		m->m[1][1] = 0x1000;
		m->m[0][2] = (int16_t)-sn;
		m->m[2][2] = cs;
	}

	static Mat4x3 *BoneMatrix(uint16_t bone)
	{
		uint8_t *skel = **(uint8_t ***)(Caster() + 0x64);
		return (Mat4x3 *)(skel + 0x20 + mul32((int16_t)bone, 0x30));
	}

	static void BonePoint32(const int16_t *v, int32_t *out)
	{
		Mat4x3 m;
		BuildBoneMatricesFromPose(Caster() + 0x60);
		ComposeAffineTransform((const Mat4x3 *)(Caster() + 0x40), BoneMatrix(v[3]), &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		GteLoadV0(v);
		GteMVMVA_RotV0Tr();
		GteReadMAC123(out);
	}

	static void BonePoint16(const int16_t *v, int16_t *out)
	{
		Mat4x3 m;
		BuildBoneMatricesFromPose(Caster() + 0x60);
		ComposeAffineTransform((const Mat4x3 *)(Caster() + 0x40), BoneMatrix(v[3]), &m);
		GteSetRotMatrixCtrl(&m);
		GteSetTransVectorCtrl(&m);
		GteLoadV0(v);
		GteMVMVA_RotV0Tr();
		GteStoreIR123(out);
	}

	static void CameraBlend(const int16_t *k0, const int16_t *k1, int32_t za, int32_t zb, int32_t t)
	{
		CameraZoom() = (int16_t)(sdiv_pow2(mul32(zb - za, t), 12) + za);
		const int32_t u = 0x1000 - t;
		GteSetIR0(u);
		GteLoadIR123(k0);
		GteGPF();
		GteSetIR0(t);
		GteLoadIR123(k1);
		GteGPL();
		GteStoreIR123((void *)game_addr(game::Battle_Camera_LookAt_XZ_s16));
		GteSetIR0(u);
		GteLoadIR123(k0 + 3);
		GteGPF();
		GteSetIR0(t);
		GteLoadIR123(k1 + 3);
		GteGPL();
		GteStoreIR123((void *)game_addr(game::Battle_Camera_world_XZ_s16));
	}

	static uint32_t DrawTris(uint8_t *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		uint8_t *pk = (uint8_t *)cursor;
		const uint8_t *verts = *(const uint8_t *const *)h;
		const uint8_t *list = *(const uint8_t *const *)(h + 4);
		const int32_t count = *(const int32_t *)list;
		const uint8_t *p = list + 4;
		*(const uint8_t **)(h + 4) = p;
		if (count > 0)
		{
			for (int32_t n = count; n != 0; n--, p += 0x1C)
			{
				GteLoadV012(verts + *(const uint16_t *)(p + 4) * 4, verts + *(const uint16_t *)(p + 6) * 4, verts + *(const uint16_t *)(p + 8) * 4);
				GteRTPT();
				*(uint32_t *)pk = 0x9000000;
				*(uint32_t *)(pk + 4) = *(const uint32_t *)p;
				*(uint32_t *)(pk + 0xC) = *(const uint32_t *)(p + 0xC);
				*(uint32_t *)(pk + 0x18) = *(const uint32_t *)(p + 0x10);
				*(uint32_t *)(pk + 0x24) = *(const uint32_t *)(p + 8) >> 16;
				GteReadFLAG(h + 0x14);
				if (*(const uint32_t *)(h + 0x14) & 0x60000) continue;
				GteNCLIP();
				GteReadMAC0(h + 8);
				if (*(const int32_t *)(h + 8) < 0) continue;
				GteReadSXY012(pk + 8, pk + 0x14, pk + 0x20);
				GteAVSZ3();
				uint32_t c = 0;
				int16_t v = *(const int16_t *)(pk + 8);
				if (v < 0 || v > 0xA00) c = 1;
				v = *(const int16_t *)(pk + 0x14);
				if (v < 0 || v > 0xA00) c |= 2;
				v = *(const int16_t *)(pk + 0x20);
				if (v < 0 || v > 0xA00) c |= 4;
				v = *(const int16_t *)(pk + 0xA);
				if (v < 0 || v > 0x6C0) c |= 0x10;
				v = *(const int16_t *)(pk + 0x16);
				if (v < 0 || v > 0x6C0) c |= 0x20;
				v = *(const int16_t *)(pk + 0x22);
				if (v < 0 || v > 0x6C0) c |= 0x40;
				if ((c & 7) == 7 || (c & 0x70) == 0x70) continue;
				GteReadOTZWord(h + 0x10);
				*(uint32_t *)(pk + 0x10) = *(const uint32_t *)(p + 0x14);
				*(uint32_t *)(pk + 0x1C) = *(const uint32_t *)(p + 0x18);
				InsertPrimAutoDepth(ot + (uint32_t)(*(const int32_t *)(h + 0x10) >> (shift & 31)) * 4, pk);
				pk += 0x28;
			}
		}
		*(const uint8_t **)(h + 4) = p;
		return (uint32_t)pk;
	}

	static uint32_t DrawQuads(uint8_t *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		uint8_t *pk = (uint8_t *)cursor;
		const uint8_t *verts = *(const uint8_t *const *)h;
		const uint8_t *list = *(const uint8_t *const *)(h + 4);
		const int32_t count = *(const int32_t *)list;
		const uint8_t *p = list + 4;
		*(const uint8_t **)(h + 4) = p;
		if (count > 0)
		{
			for (int32_t n = count; n != 0; n--, p += 0x24)
			{
				GteLoadV012(verts + *(const uint16_t *)(p + 4) * 4, verts + *(const uint16_t *)(p + 6) * 4, verts + *(const uint16_t *)(p + 8) * 4);
				GteRTPT();
				*(uint32_t *)pk = 0xC000000;
				*(uint32_t *)(pk + 4) = *(const uint32_t *)p;
				*(uint32_t *)(pk + 0xC) = *(const uint32_t *)(p + 0xC);
				*(uint32_t *)(pk + 0x18) = *(const uint32_t *)(p + 0x10);
				*(uint32_t *)(pk + 0x24) = *(const uint32_t *)(p + 0x14);
				*(uint32_t *)(pk + 0x30) = *(const uint32_t *)(p + 0x14) >> 16;
				GteReadFLAG(h + 0x14);
				if (*(const uint32_t *)(h + 0x14) & 0x60000) continue;
				GteReadSXY012(pk + 8, pk + 0x14, pk + 0x20);
				GteLoadV0(verts + *(const uint16_t *)(p + 0xA) * 4);
				GteRTPS();
				uint32_t c = 0;
				int16_t v = *(const int16_t *)(pk + 8);
				if (v < 0 || v > 0xA00) c = 1;
				v = *(const int16_t *)(pk + 0x14);
				if (v < 0 || v > 0xA00) c |= 2;
				v = *(const int16_t *)(pk + 0x20);
				if (v < 0 || v > 0xA00) c |= 4;
				v = *(const int16_t *)(pk + 0xA);
				if (v < 0 || v > 0x6C0) c |= 0x10;
				v = *(const int16_t *)(pk + 0x16);
				if (v < 0 || v > 0x6C0) c |= 0x20;
				v = *(const int16_t *)(pk + 0x22);
				if (v < 0 || v > 0x6C0) c |= 0x40;
				GteReadSXY2(pk + 0x2C);
				GteAVSZ4();
				v = *(const int16_t *)(pk + 0x2C);
				if (v < 0 || v > 0xA00) c |= 8;
				v = *(const int16_t *)(pk + 0x2E);
				if (v < 0 || v > 0x6C0) c |= 0x80;
				if ((c & 0xF) == 0xF || (c & 0xF0) == 0xF0) continue;
				GteReadOTZWord(h + 0x10);
				*(uint32_t *)(pk + 0x10) = *(const uint32_t *)(p + 0x18);
				*(uint32_t *)(pk + 0x1C) = *(const uint32_t *)(p + 0x1C);
				*(uint32_t *)(pk + 0x28) = *(const uint32_t *)(p + 0x20);
				InsertPrimAutoDepth(ot + (uint32_t)(*(const int32_t *)(h + 0x10) >> (shift & 31)) * 4, pk);
				pk += 0x34;
			}
		}
		*(const uint8_t **)(h + 4) = p;
		return (uint32_t)pk;
	}

	static void __cdecl PartCallback(prim::Layout *l, prim::Record *r, int arg_)
	{
		PrimArg *arg = (PrimArg *)arg_;
		if (((int32_t)r->scale[2] | *(const int32_t *)r->scale) == 0) return;
		if (r->a >= 0x1000 && *(const uint32_t *)r->rgb == 0) return;
		uint8_t *h = (uint8_t *)FieldAlloc(0x6C);
		const int32_t object = (int16_t)r->flags_lo;
		uint8_t *model = l->data + *(const int32_t *)(l->data + object * 4 + 8);
		*(uint8_t **)h = model;
		const int16_t f0 = r->b0, f1 = r->b1;
		if (f0 == f1)
		{
			if (f0 == 0) *(uint8_t **)(h + 4) = model + 0xC;
			else *(uint8_t **)(h + 4) = model + (uint32_t)mul32(*(const int32_t *)(model + 4), f0) * 8 + 0xC;
		}
		else
		{
			const int16_t t = r->b;
			if (t == 0)
			{
				if (f0 == 0) *(uint8_t **)(h + 4) = model + 0xC;
				else *(uint8_t **)(h + 4) = model + (uint32_t)mul32(*(const int32_t *)(model + 4), f0) * 8 + 0xC;
			}
			else if (t == 0x1000)
			{
				if (f1 == 0) *(uint8_t **)(h + 4) = model + 0xC;
				else *(uint8_t **)(h + 4) = model + (uint32_t)mul32(*(const int32_t *)(model + 4), f1) * 8 + 0xC;
			}
			else
			{
				BlendVertexFrames((uint32_t)model, f0, f1, t, arg->morph);
				*(uint32_t *)(h + 4) = arg->morph;
			}
		}
		Mat4x3 m;
		if (r->flags & 0x20000) RotationFromAnglesC(r->rot, &m);
		else if (r->flags & 0x40000) RotationFromAnglesB(r->rot, &m);
		else RotationFromAngles(r->rot, &m);
		int16_t off[4] = { r->pos[0], r->pos[1], r->pos[2], 0 };
		if (r->flags & 0x800) MatVec(&m, off, off);
		int32_t t0, t1, t2;
		if (r->flags & 0x200)
		{
			t0 = off[0];
			t1 = off[1];
			t2 = off[2];
		}
		else
		{
			GteSetRotMatrixCtrl(&arg->m);
			GteLoadV0(off);
			GteMVMVA_RotV0();
			GteReadMAC123(m.t);
			MatrixMultiply(&arg->m, &m);
			t0 = m.t[0];
			t1 = m.t[1];
			t2 = m.t[2];
		}
		m.t[0] = (int32_t)((uint32_t)t0 + (uint32_t)arg->m.t[0]);
		m.t[1] = (int32_t)((uint32_t)t1 + (uint32_t)arg->m.t[1]);
		m.t[2] = (int32_t)((uint32_t)t2 + (uint32_t)arg->m.t[2]);
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
		*(uint32_t *)(h + 0x1C) = (r->flags & 0x4000) ? 0x2000 : 0x2030;
		if (r->flags & 0x2000) *(uint32_t *)(h + 0x1C) |= 0xC;
		*(int32_t *)(h + 0xC) = r->a;
		if (r->a != 0)
		{
			*(uint32_t *)(h + 0x1C) |= 0xC0;
			*(uint32_t *)(h + 8) = *(const uint32_t *)r->rgb;
		}
		*(int32_t *)(h + 0x18) = arg->depth;
		*(uint16_t *)(h + 0x22) = 0;
		*(uint16_t *)(h + 0x20) = 0;
		*(uint16_t *)(h + 0x26) = 0;
		*(uint16_t *)(h + 0x24) = 0;
		*(uint16_t *)(h + 0x2A) = 0x100;
		*(uint16_t *)(h + 0x28) = 0x100;
		switch (*(const uint8_t *)(arg->objs + object))
		{
		case '1':
			*(uint16_t *)(h + 0x2C) = 0;
			*(uint16_t *)(h + 0x2E) = 0x80;
			*(uint16_t *)(h + 0x32) = 0x80;
			*(uint16_t *)(h + 0x30) = 0x100;
			*(uint16_t *)(h + 0x22) = (uint16_t)(*(const uint8_t *)&arg->scroll & 0x7F);
			break;
		case '2':
			*(uint16_t *)(h + 0x2E) = 0;
			*(uint16_t *)(h + 0x2C) = 0;
			*(uint16_t *)(h + 0x32) = 0x80;
			*(uint16_t *)(h + 0x30) = 0x100;
			*(uint16_t *)(h + 0x22) = (uint16_t)(*(const uint8_t *)&arg->scroll & 0x7F);
			break;
		case '3':
			*(uint16_t *)(h + 0x2E) = 0;
			*(uint16_t *)(h + 0x2C) = 0;
			*(uint16_t *)(h + 0x32) = 0x100;
			*(uint16_t *)(h + 0x30) = 0x40;
			*(uint16_t *)(h + 0x20) = (uint16_t)(*(const uint8_t *)&arg->scroll & 0x3F);
			break;
		default:
			break;
		}
		arg->cursor = RenderPrimSet(h, OT(0x44), 2, arg->cursor);
		FieldFree(0x6C);
	}

	static void __cdecl ScaledPartCallback(prim::Layout *l, prim::Record *r, int arg)
	{
		r->scale[0] = (int16_t)(mul32(r->scale[0], Scale()) >> 12);
		r->scale[1] = (int16_t)(mul32(r->scale[1], Scale()) >> 12);
		r->scale[2] = (int16_t)(mul32(r->scale[2], Scale()) >> 12);
		PartCallback(l, r, arg);
	}

	inline uint8_t *PModels() { return (uint8_t *)(TexBase() + 0x3B530); }

	static void PModelsInit()
	{
		for (uint32_t off = 0xAEA;; off -= 0x16)
		{
			*(uint8_t *)(TexBase() + 0x3B536 + off) = 0;
			if (off == 0) break;
		}
		PModelCursor() = PModels();
	}

	static PModel *PModelAlloc()
	{
		uint8_t *first = PModels();
		uint8_t *p = PModelCursor();
		if (p[6] != 0)
		{
			p = first;
			if (first[6] != 0)
			{
				int32_t n = 0x80;
				for (;;)
				{
					p += 0x16;
					if (--n == 0) return nullptr;
					if (p[6] == 0) break;
				}
			}
		}
		PModelCursor() = p >= first + 0xAEA ? first : p + 0x16;
		PModel *m = (PModel *)p;
		m->rot[0] = (int16_t)CrtRand();
		m->rot[1] = (int16_t)CrtRand();
		m->drot[0] = (int8_t)CrtRand();
		m->drot[1] = (int8_t)CrtRand();
		return m;
	}

	static void PModelsUpdate()
	{
		PModel *p = (PModel *)PModels();
		for (int i = 0; i < 0x80; i++, p = (PModel *)((uint8_t *)p + 0x16))
		{
			if (p->life == 0) continue;
			if (--p->life == 0)
			{
				PModelCursor() = (uint8_t *)p;
				continue;
			}
			p->rot[0] = (int16_t)(p->rot[0] + (int16_t)p->drot[0]);
			p->rot[1] = (int16_t)(p->rot[1] + (int16_t)p->drot[1]);
			p->pos[0] = (int16_t)(p->pos[0] + p->vel[0]);
			p->pos[2] = (int16_t)(p->pos[2] + p->vel[2]);
			p->pos[1] = (int16_t)(p->pos[1] + p->vel[1]);
		}
	}

	static void PModelsDraw()
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x78);
		static const uint32_t MESH[8] = { 0x39E00, 0x3A0B8, 0x3A370, 0x3A628, 0x3A8E0, 0x3AB98, 0x3AE50, 0x3B108 };
		for (int k = 0; k < 8; k++) *(uint32_t *)(h + 0x40 + 4 * k) = TexBase() + MESH[k];
		Mat4x3 *m = (Mat4x3 *)h;
		uint8_t *p = PModels();
		for (int n = 0x80; n != 0; n--, p += 0x16)
		{
			const PModel *q = (const PModel *)p;
			if (q->life == 0) continue;
			RotXScaledTab(q->rot[0], q->scale, m);
			RotYTab(q->rot[1], (Mat4x3 *)(h + 0x20));
			GteSetRotMatrixCtrl(m);
			GteLoadIRColumn(h + 0x20);
			GteMVMVA_RotIR();
			GteStoreIRColumn(h);
			GteLoadIRColumn(h + 0x22);
			GteMVMVA_RotIR();
			GteStoreIRColumn(h + 2);
			GteLoadIRColumn(h + 0x24);
			GteMVMVA_RotIR();
			GteStoreIRColumn(h + 4);
			*(int32_t *)(h + 0x14) = q->pos[0];
			*(int32_t *)(h + 0x18) = q->pos[1];
			*(int32_t *)(h + 0x1C) = q->pos[2];
			GteSetRotMatrixCtrl(&Camera());
			GteLoadIRColumn(h);
			GteMVMVA_RotIR();
			GteStoreIRColumn(h);
			GteLoadIRColumn(h + 2);
			GteMVMVA_RotIR();
			GteStoreIRColumn(h + 2);
			GteLoadIRColumn(h + 4);
			GteMVMVA_RotIR();
			GteStoreIRColumn(h + 4);
			GteSetTransVectorCtrl(&Camera());
			GteLoadV0Dwords(h + 0x14);
			GteMVMVA_RotV0Tr();
			GteReadMAC123((int32_t *)(h + 0x14));
			GteSetRotMatrixCtrl(m);
			GteSetTransVectorCtrl(m);
			const uint32_t mesh = *(const uint32_t *)(h + 0x40 + (int32_t)q->model * 4);
			*(uint32_t *)(h + 0x64) = *(const uint32_t *)mesh + mesh + 0x18;
			*(uint32_t *)(h + 0x60) = mesh + 8;
			FrameArena() = DrawTris(h + 0x60, OT(0x44), 2, FrameArena());
		}
		FieldFree(0x78);
	}

	inline uint8_t *Sprites() { return (uint8_t *)(TexBase() + 0x3E1F0); }

	static void SpritesInit()
	{
		uint8_t *p = Sprites() + 6;
		for (int n = 0x80; n != 0; n--, p += 8) *(uint16_t *)p = 0;
		SpriteCursor() = Sprites();
	}

	static SpriteP *SpriteAlloc()
	{
		uint8_t *first = Sprites();
		uint8_t *p = SpriteCursor();
		if (*(const int16_t *)(p + 6) != 0)
		{
			p = first;
			if (*(const int16_t *)(first + 6) != 0)
			{
				int32_t n = 0x80;
				for (;;)
				{
					p += 8;
					if (--n == 0) return nullptr;
					if (*(const int16_t *)(p + 6) == 0) break;
				}
			}
		}
		SpriteCursor() = p >= first + 0x3F8 ? first : p + 8;
		return (SpriteP *)p;
	}

	static void SpritesUpdate()
	{
		uint8_t *p = Sprites() + 6;
		for (int n = 0x80; n != 0; n--, p += 8)
		{
			const int16_t v = *(const int16_t *)p;
			if (v != 0) *(int16_t *)p = (int16_t)(v - 1);
		}
	}

	static void SpritesDraw()
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		*(uint16_t *)(h + 0x60) = 0;
		*(uint16_t *)(h + 0x58) = 0;
		*(uint16_t *)(h + 0x50) = 0;
		*(uint16_t *)(h + 0x48) = 0;
		uint8_t *p = Sprites();
		for (int n = 0x80; n != 0; n--, p += 8)
		{
			const int16_t life = *(const int16_t *)(p + 6);
			if (life == 0) continue;
			GteSetRotMatrixCtrl(&Camera());
			GteSetTransVectorCtrl(&Camera());
			GteLoadV0(p);
			GteMVMVA_RotV0Tr();
			GteReadMAC123((int32_t *)(h + 0x98));
			uint32_t *z = (uint32_t *)(h + 0x84);
			z[0] = z[1] = z[2] = z[3] = z[4] = 0;
			*(uint16_t *)(h + 0x94) = 0x800;
			*(uint16_t *)(h + 0x8C) = 0x800;
			*(uint16_t *)(h + 0x84) = 0x800;
			*(uint32_t *)h = game_addr(SPRITE_SEQ);
			const int32_t frames = *(const int16_t *)(game_addr(SPRITE_SEQ) + 8);
			const int32_t idx = frames - (life - 1) % frames;
			uint32_t at = game_addr(SPRITE_SEQ) + *(const int16_t *)(game_addr(SPRITE_SEQ) + 8 + idx * 2);
			*(uint32_t *)(h + 0x2C) = at;
			int32_t w = *(const int32_t *)at;
			*(int32_t *)(h + 0x30) = w;
			if (w < 0)
			{
				w &= 0x7FFFFFFF;
				h[0x25] |= 1;
				*(int32_t *)(h + 0x30) = w;
			}
			*(uint32_t *)(h + 0x2C) += 4;
			PacketCursor() = Flipbook(h, OT(0x44), 2, PacketCursor());
		}
		FieldFree(0xB4);
	}

	inline uint8_t *Quads() { return (uint8_t *)(TexBase() + 0x27690); }

	static void QuadsInit()
	{
		for (uint32_t off = 0x7E0;; off -= 0x20)
		{
			*(uint16_t *)(TexBase() + 0x27696 + off) = 0;
			if (off == 0) break;
		}
		QuadCursor() = Quads();
	}

	static QuadP *QuadAlloc()
	{
		uint8_t *first = Quads();
		uint8_t *p = QuadCursor();
		if (*(const int16_t *)(p + 6) != 0)
		{
			p = first;
			if (*(const int16_t *)(first + 6) != 0)
			{
				int32_t n = 0x40;
				for (;;)
				{
					p += 0x20;
					if (--n == 0) return nullptr;
					if (*(const int16_t *)(p + 6) == 0) break;
				}
			}
		}
		QuadCursor() = p >= first + 0x7E0 ? first : p + 0x20;
		return (QuadP *)p;
	}

	static void QuadsUpdate()
	{
		QuadP *q = (QuadP *)Quads();
		for (int n = 0x40; n != 0; n--, q++)
		{
			int16_t v = q->life;
			if (v == 0) continue;
			v = (int16_t)(v - 1);
			q->life = v;
			if (v == 0)
			{
				QuadCursor() = (uint8_t *)q;
				continue;
			}
			q->pos[0] = (int16_t)(q->pos[0] + q->vel[0]);
			q->pos[2] = (int16_t)(q->pos[2] + q->vel[2]);
			q->pos[1] = (int16_t)(q->pos[1] + q->vel[1]);
		}
	}

	static void QuadsDraw()
	{
		const uint8_t *mesh = (const uint8_t *)(TexBase() + 0x3B3C0);
		uint8_t *h = (uint8_t *)FieldAlloc(0x28);
		QuadP *q = (QuadP *)Quads();
		for (int n = 0x40; n != 0; n--, q++)
		{
			if (q->life == 0) continue;
			GteSetRotMatrixCtrl((const Mat4x3 *)q->m);
			*(int32_t *)h = q->pos[0];
			*(int32_t *)(h + 4) = q->pos[1];
			*(int32_t *)(h + 8) = q->pos[2];
			GteSetTransVectorCtrl((const Mat4x3 *)(h - 0x14));
			*(const uint8_t **)(h + 0x10) = mesh + 8;
			*(const uint8_t **)(h + 0x14) = mesh + *(const uint32_t *)mesh + 0x1C;
			PacketCursor() = DrawQuads(h + 0x10, OT(0x44), 4, PacketCursor());
		}
		FieldFree(0x28);
	}

	inline uint8_t *Segs() { return (uint8_t *)(TexBase() + 0x3B530); }
	inline uint8_t *Chains() { return (uint8_t *)(TexBase() + 0x3CD30); }
	template<typename T> inline T &F(uint8_t *p, uint32_t off) { return *(T *)(p + off); }

	static void RibbonsInit(int32_t shift, int32_t keep, int32_t depth)
	{
		NoiseShift() = (int16_t)(15 - shift);
		NoiseKeep() = (int16_t)keep;
		const int32_t one = (int32_t)(1u << (shift & 31));
		NoiseHalf() = (int16_t)((one - (one >> 31)) >> 1);
		uint8_t *p = Segs();
		RibbonDepth() = (int16_t)depth;
		SegCursor() = p;
		for (int n = 0x100; n != 0; n--) { p += 6; *(int16_t *)p = -1; p += 0x12; }
		uint8_t *c = Chains();
		ChainCursor() = c;
		for (int n = 0x20; n != 0; n--, c += 0x38) *(uint32_t *)c = 0;
	}

	static uint8_t *SegAlloc()
	{
		uint8_t *first = Segs();
		uint8_t *p = SegCursor();
		if (F<int16_t>(p, 6) != -1)
		{
			p = first;
			if (F<int16_t>(first, 6) != -1)
			{
				int32_t n = 0x100;
				for (;;)
				{
					p += 0x18;
					if (--n == 0) return nullptr;
					if (F<int16_t>(p, 6) == -1) break;
				}
			}
		}
		SegCursor() = p >= first + 0x17E8 ? first : p + 0x18;
		return p;
	}

	static void ChainNew(const uint8_t *rec)
	{
		uint8_t *first = Chains();
		uint8_t *c = ChainCursor();
		if (F<uint32_t>(c, 0) != 0)
		{
			c = first;
			if (F<uint32_t>(first, 0) != 0)
			{
				int32_t n = 0x20;
				for (;;)
				{
					c += 0x38;
					if (--n == 0) return;
					if (F<uint32_t>(c, 0) == 0) break;
				}
			}
		}
		ChainCursor() = c >= first + 0x6C8 ? first : c + 0x38;
		uint8_t *s = SegAlloc();
		if (!s) return;
		F<uint32_t>(s, 0x14) = 0;
		F<uint32_t>(s, 0) = *(const uint32_t *)(rec + 0x10);
		F<uint32_t>(s, 4) = *(const uint32_t *)(rec + 0x14);
		F<uint32_t>(s, 8) = 0;
		F<uint32_t>(s, 0xC) = 0;
		F<uint32_t>(s, 0x10) = *(const uint32_t *)(rec + 0x30);
		F<int16_t>(c, 0x34) = 1;
		F<uint8_t *>(c, 4) = s;
		F<uint8_t *>(c, 0) = s;
		F<uint32_t>(c, 8) = *(const uint32_t *)(rec + 8);
		F<uint32_t>(c, 0xC) = *(const uint32_t *)(rec + 0xC);
		F<uint32_t>(c, 0x30) = *(const uint32_t *)(rec + 0x30);
		for (uint32_t o = 0x10; o < 0x30; o += 4) F<uint32_t>(c, o) = *(const uint32_t *)(rec + o);
		F<uint16_t>(c, 0x36) = *(const uint16_t *)(rec + 0x36);
	}

	static void RibbonsUpdate()
	{
		uint8_t *c = Chains();
		for (int n = 0x20; n != 0; n--, c += 0x38)
		{
			if (F<uint32_t>(c, 0) == 0) continue;
			F<int16_t>(c, 0x36) = (int16_t)(F<int16_t>(c, 0x36) - 0x100);
			if (F<int16_t>(c, 0x36) <= 0)
			{
				uint8_t *s = F<uint8_t *>(c, 0);
				SegCursor() = s;
				do
				{
					F<int16_t>(s, 6) = -1;
					s = F<uint8_t *>(s, 0x14);
				} while (s);
				F<uint32_t>(c, 0) = 0;
				ChainCursor() = c;
				continue;
			}
			if (F<int16_t>(c, 0x1E) < 0x1000)
			{
				F<int16_t>(c, 0x1E) = (int16_t)(F<int16_t>(c, 0x1E) + F<int16_t>(c, 0x26));
				F<int16_t>(c, 0x10) = (int16_t)(F<int16_t>(c, 0x10) + F<int16_t>(c, 0x20));
				F<int16_t>(c, 0x12) = (int16_t)(F<int16_t>(c, 0x12) + F<int16_t>(c, 0x22));
				F<int16_t>(c, 0x14) = (int16_t)(F<int16_t>(c, 0x14) + F<int16_t>(c, 0x24));
				for (uint32_t o = 0x20; o <= 0x24; o += 2)
				{
					const int32_t r = NextLcg();
					F<int16_t>(c, o) = (int16_t)(F<int16_t>(c, o) + (int16_t)((r >> (*(const uint8_t *)&NoiseShift() & 31)) - NoiseHalf()));
				}
				if (NextLcg() < NoiseKeep())
				{
					for (uint32_t o = 0x10; o <= 0x14; o += 2)
					{
						const int32_t r = NextLcg();
						const int32_t sh = (*(const int32_t *)&NoiseShift() - 2) & 31;
						F<int16_t>(c, o) = (int16_t)(F<int16_t>(c, o) + (int16_t)((r >> sh) - (int16_t)(NoiseHalf() << 2)));
					}
				}
				int16_t at[4];
				if (F<int16_t>(c, 0x1E) >= 0x1000)
				{
					F<int16_t>(c, 0x1E) = 0x1000;
					*(uint32_t *)&at[0] = F<uint32_t>(c, 0x18);
					*(uint32_t *)&at[2] = F<uint32_t>(c, 0x1C);
				}
				else
				{
					GteSetIR0(0x1000 - F<int16_t>(c, 0x1E));
					GteLoadIR123(c + 0x10);
					GteGPF();
					GteSetIR0(F<int16_t>(c, 0x1E));
					GteLoadIR123(c + 0x18);
					GteGPL();
					GteStoreIR123(at);
					at[3] = 0;
				}
				uint8_t *s = SegAlloc();
				if (!s)
				{
					F<int16_t>(c, 0x1E) = 0x1000;
					return;
				}
				F<uint32_t>(s, 0x14) = 0;
				F<uint32_t>(s, 0) = *(const uint32_t *)&at[0];
				F<uint32_t>(s, 4) = *(const uint32_t *)&at[2];
				F<int16_t>(s, 6) = F<int16_t>(c, 0x16);
				F<uint32_t>(s, 0x10) = F<uint32_t>(c, 0x30);
				F<uint8_t *>(F<uint8_t *>(c, 4), 0x14) = s;
				F<uint8_t *>(c, 4) = s;
				GteSetIR0(SinTab(F<int16_t>(c, 0x1E) >> 1));
				GteLoadIR123(c + 0x28);
				GteGPF();
				F<int16_t>(c, 0x34) = (int16_t)(F<int16_t>(c, 0x34) + 1);
				GteStoreIR123(s + 8);
				F<int16_t>(s, 0xE) = F<int16_t>(c, 0x1E);
			}
			for (uint8_t *s = F<uint8_t *>(c, 0); s; s = F<uint8_t *>(s, 0x14))
			{
				F<int16_t>(s, 0x10) = (int16_t)(F<int16_t>(s, 0x10) + F<int16_t>(s, 0x12));
				if (F<int16_t>(s, 0x10) < 0) F<int16_t>(s, 0x10) = 0;
				F<int16_t>(s, 0) = (int16_t)(F<int16_t>(s, 0) + F<int16_t>(s, 8));
				F<int16_t>(s, 2) = (int16_t)(F<int16_t>(s, 2) + F<int16_t>(s, 0xA));
				F<int16_t>(s, 4) = (int16_t)(F<int16_t>(s, 4) + F<int16_t>(s, 0xC));
			}
			uint8_t *s = F<uint8_t *>(c, 0);
			if (F<int16_t>(s, 0x10) > 0x400) continue;
			for (;;)
			{
				if (F<int16_t>(F<uint8_t *>(s, 0x14), 0x10) > 0x400) break;
				SegCursor() = s;
				F<int16_t>(s, 6) = -1;
				s = F<uint8_t *>(s, 0x14);
				F<int16_t>(c, 0x34) = (int16_t)(F<int16_t>(c, 0x34) - 1);
				F<uint8_t *>(c, 0) = s;
				if (F<int16_t>(c, 0x34) < 2)
				{
					F<uint32_t>(c, 0) = 0;
					ChainCursor() = c;
					break;
				}
				if (F<int16_t>(s, 0x10) > 0x400) break;
			}
		}
	}

	static void SegEdges(uint8_t *s, uint8_t *n, const int16_t *prev, int16_t *dir)
	{
		if (F<int16_t>(s, 4) < 0) return;
		int32_t ox = 0, oy = 0;
		if (n)
		{
			if (F<int16_t>(n, 4) < 0) return;
			const int32_t dx = F<int16_t>(n, 0) - F<int16_t>(s, 0);
			const int32_t ndy = F<int16_t>(s, 2) - F<int16_t>(n, 2);
			int32_t adx = dx < 0 ? -dx : dx, ady = ndy < 0 ? -ndy : ndy;
			if (adx >= 0x80) adx = 0x7F;
			if (ady >= 0x80) ady = 0x7F;
			const int32_t len = *(const uint8_t *)(game_addr(DIST_TABLE) + ady * 128 + adx);
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
					if (sx == 0 && sy == 0) { sx = cx * 2; sy = cy * 2; }
					const int32_t sq = (int32_t)((uint32_t)mul32(sx, sx) + (uint32_t)mul32(sy, sy));
					const int32_t k = 0x4000 - (sq >> 13);
					px = mul32(k, sx) >> 13;
					py = mul32(k, sy) >> 13;
				}
			}
			else
			{
				if (len == 0) { F<int16_t>(s, 4) = -1; return; }
				px = shl32(ndy, 12) / len;
				py = shl32(dx, 12) / len;
				dir[0] = (int16_t)px;
				dir[1] = (int16_t)py;
			}
			const int32_t w = (int32_t)F<int16_t>(n, 6) / (F<int16_t>(s, 4) + 0x80);
			ox = mul32(w, px) >> 12;
			oy = mul32(w, py) >> 12;
		}
		const int16_t x = F<int16_t>(s, 0), y = F<int16_t>(s, 2);
		F<int16_t>(s, 0) = (int16_t)(x - ox);
		F<int16_t>(s, 8) = (int16_t)(x + ox);
		F<int16_t>(s, 0xA) = (int16_t)(y + oy);
		F<int16_t>(s, 2) = (int16_t)(y - oy);
	}

	static void RibbonDraw(uint8_t *h, uint8_t *seg, int32_t count)
	{
		uint8_t *o = h + 0x50;
		const int32_t step = 0x800 / count * 4;
		uint32_t wave = game_addr(SINCOS);
		do
		{
			const int32_t t = F<int16_t>(seg, 0xE), u = 0x1000 - t;
			GteSetIR0(t);
			GteLoadIR123(h + 0xC);
			GteGPF();
			GteSetIR0(u);
			GteLoadIR123(seg);
			GteGPL();
			GteStoreIR123(h + 0x1C);
			GteSetIR0(u);
			GteLoadIR123(h + 0x14);
			GteGPF();
			GteSetIR0(t);
			GteLoadIR123(seg);
			GteGPL();
			GteStoreIR123(h + 0x24);
			GteSetIR0(t);
			GteLoadIR123(h + 0x1C);
			GteGPF();
			GteSetIR0(u);
			GteLoadIR123(h + 0x24);
			GteGPL();
			GteStoreIR123(h + 0x1C);
			GteLoadV0(h + 0x1C);
			GteRTPS();
			GteReadSXY2(o);
			GteReadOTZ(o + 4);
			F<int16_t>(o, 0) = (int16_t)sdiv_pow2(F<int16_t>(o, 0), 3);
			F<int16_t>(o, 2) = (int16_t)sdiv_pow2(F<int16_t>(o, 2), 3);
			F<int16_t>(o, 6) = (int16_t)(mul32(F<int16_t>(seg, 6), *(const int16_t *)wave) >> 12);
			wave += step;
			const int32_t g = F<int16_t>(seg, 0x10), f = g & 0xFFF, i = g >> 12;
			GteSetIR0(0x1000 - f);
			GteLoadIRBytes(h + i * 4 + 0x3C);
			GteGPF();
			GteSetIR0(f);
			GteLoadIRBytes(h + i * 4 + 0x40);
			GteGPL();
			GteStoreIRBytes(o + 0x10);
			seg = F<uint8_t *>(seg, 0x14);
			o += 0x14;
		} while (seg);
		int16_t *dir = (int16_t *)(h + 4);
		uint8_t *s = h + 0x50;
		SegEdges(s, s + 0x14, nullptr, dir);
		s += 0x14;
		for (int32_t k = count - 2; k != 0; k--, s += 0x14) SegEdges(s, s + 0x14, dir, dir);
		SegEdges(s, nullptr, dir, nullptr);
		uint8_t *a = h + 0x50;
		for (int32_t k = count - 1; k != 0; k--, a += 0x14)
		{
			uint8_t *b = a + 0x14;
			if (F<int16_t>(a, 4) <= 0 || F<int16_t>(b, 4) <= 0) continue;
			uint8_t *p = (uint8_t *)PacketCursor();
			PacketCursor() += 0x34;
			F<uint32_t>(p, 8) = F<uint32_t>(a, 0);
			F<uint32_t>(p, 0x14) = F<uint32_t>(a, 8);
			F<uint32_t>(p, 0xC) = 0x3F54A048;
			F<uint32_t>(p, 0x18) = 0xB6A078;
			F<uint16_t>(p, 0x24) = 0xA048;
			F<uint16_t>(p, 0x30) = 0xA078;
			F<uint32_t>(p, 0x20) = F<uint32_t>(b, 0);
			F<uint32_t>(p, 0x2C) = F<uint32_t>(b, 8);
			const uint32_t ca = F<uint32_t>(a, 0x10);
			F<uint32_t>(p, 0x10) = ca;
			F<uint32_t>(p, 4) = ca;
			const uint32_t cb = F<uint32_t>(b, 0x10);
			F<uint32_t>(p, 0) = 0xC000000;
			F<uint32_t>(p, 0x28) = cb;
			F<uint32_t>(p, 0x1C) = cb;
			p[7] = 0x3E;
			int32_t d = ((F<int16_t>(a, 4) + F<int16_t>(b, 4)) >> 3) + F<int32_t>(h, 0x4C);
			if (d < 0) d = 0;
			InsertPrimAltViewport(F<uint32_t>(h, 0) + (uint32_t)d * 4, p);
		}
	}

	static void RibbonsDraw()
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x190);
		F<uint32_t>(h, 0) = OT(0x44);
		F<uint32_t>(h, 0x38) = 0x80F0F0;
		F<uint32_t>(h, 0x34) = 0x40F0;
		F<uint32_t>(h, 0x30) = 0x80F0F0;
		F<uint32_t>(h, 0x2C) = 0;
		F<int32_t>(h, 0x4C) = RibbonDepth();
		GteSetRotMatrixCtrl(&Camera());
		GteSetTransVectorCtrl(&Camera());
		uint8_t *c = Chains();
		for (int n = 0x20; n != 0; n--, c += 0x38)
		{
			if (F<uint32_t>(c, 0) == 0 || F<int16_t>(c, 0x34) < 2) continue;
			const int16_t fade = F<int16_t>(c, 0x36);
			if (fade < 0x1000)
			{
				GteSetIR0(fade);
				for (uint32_t o = 0x48; o >= 0x3C; o -= 4)
				{
					GteLoadIRBytes(h + o - 0x10);
					GteGPF();
					GteStoreIRBytes(h + o);
				}
			}
			else
			{
				for (uint32_t o = 0x48; o >= 0x3C; o -= 4) F<uint32_t>(h, o) = F<uint32_t>(h, o - 0x10);
			}
			const uint32_t *a = F<const uint32_t *>(c, 8);
			F<uint32_t>(h, 0xC) = a[0];
			F<uint32_t>(h, 0x10) = a[1];
			const uint32_t *b = F<const uint32_t *>(c, 0xC);
			F<uint32_t>(h, 0x14) = b[0];
			F<uint32_t>(h, 0x18) = b[1];
			RibbonDraw(h, F<uint8_t *>(c, 0), F<int16_t>(c, 0x34));
		}
		FieldFree(0x190);
	}

	static uint32_t Billboard(int32_t x, int32_t y, int32_t z, int32_t angle, int32_t scale, int32_t scroll, int32_t flags, int32_t depth, uint32_t model, uint32_t cursor)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x8C);
		Mat4x3 *m = (Mat4x3 *)h;
		RotYScaled(angle, scale, m);
		m->t[0] = x;
		m->t[1] = y;
		m->t[2] = z;
		if (flags & 0x8000)
		{
			TransformVec3(&Camera(), m->t, m->t);
			m->t[0] = (int32_t)((uint32_t)m->t[0] + (uint32_t)Camera().t[0]);
			m->t[1] = (int32_t)((uint32_t)m->t[1] + (uint32_t)Camera().t[1]);
			m->t[2] = (int32_t)((uint32_t)m->t[2] + (uint32_t)Camera().t[2]);
		}
		else ComposeAffineTransform(&Camera(), m, m);
		GteSetRotMatrix(m);
		GteSetTransVector(m);
		uint8_t *p = h + 0x20;
		F<int32_t>(p, 0x1C) = flags;
		F<uint32_t>(p, 0) = model;
		F<uint32_t>(p, 0x20) = 0;
		if (scroll != 0)
		{
			F<uint16_t>(p, 0x26) = 0;
			F<uint16_t>(p, 0x24) = 0;
			F<uint16_t>(p, 0x2A) = 0x100;
			F<uint16_t>(p, 0x28) = 0x100;
			F<uint16_t>(p, 0x2C) = 0;
			F<uint16_t>(p, 0x2E) = 0;
			F<uint16_t>(p, 0x32) = 0x80;
			F<uint16_t>(p, 0x30) = 0x100;
			F<uint16_t>(p, 0x22) = (uint16_t)(scroll & 0x7F);
		}
		F<int32_t>(p, 0x18) = depth;
		cursor = RenderPrimSet(p, OT(0x44), 2, cursor);
		FieldFree(0x8C);
		return cursor;
	}

	static uint32_t Stars(int32_t depth, int32_t x, int32_t y, int32_t z, int32_t limit, uint32_t cursor)
	{
		Star *st = (Star *)(TexBase() + 0xDB8);
		uint8_t *pk = (uint8_t *)cursor;
		uint8_t *h = (uint8_t *)FieldAlloc(0x64);
		const uint32_t *cam = (const uint32_t *)&Camera();
		F<uint32_t>(h, 0) = cam[0];
		F<uint32_t>(h, 4) = cam[1];
		F<uint32_t>(h, 8) = cam[2];
		F<uint32_t>(h, 0xC) = cam[3];
		F<uint16_t>(h, 0x10) = *(const uint16_t *)&cam[4];
		Mat4x3 *v = (Mat4x3 *)h;
		v->t[0] = x;
		v->t[1] = y;
		v->t[2] = z;
		TransformVec3(v, v->t, v->t);
		v->t[0] = (int32_t)((uint32_t)v->t[0] + (uint32_t)Camera().t[0]);
		v->t[1] = (int32_t)((uint32_t)v->t[1] + (uint32_t)Camera().t[1]);
		v->t[2] = (int32_t)((uint32_t)v->t[2] + (uint32_t)Camera().t[2]);
		for (uint32_t g = game_addr(STARS) + 6; g < game_addr(game::unk_133AC4E); g += 0x10)
		{
			RotX(*(const int16_t *)(g - 2), h + 0x40);
			RotY(*(const int16_t *)g, h + 0x20);
			MatMul3(h + 0x20, h + 0x40, h + 0x40);
			MatMul3(h, h + 0x40, h + 0x40);
			GteSetRotMatrix((const Mat4x3 *)(h + 0x40));
			GteSetTransVector(v);
			for (int n = 0x80; n != 0; n--, st++)
			{
				if (st->dist >= limit) continue;
				GteLoadV0(st);
				GteRTPS();
				GteReadSXY2(pk + 8);
				GteReadOTZ(h + 0x60);
				if (F<int32_t>(h, 0x60) <= 0) continue;
				if (st->level != 0)
				{
					const int32_t l = (uint16_t)((uint16_t)st->level << 4);
					GteSetIR0(l);
					GteLoadIRBytes(st->rgb);
					GteGPF();
					GteSetIR0(0x1000 - l);
					GteLoadIRBytes(st->rgb2);
					GteGPL();
					GteStoreIRBytes(pk + 4);
				}
				else F<uint32_t>(pk, 4) = *(const uint32_t *)st->rgb;
				F<uint32_t>(pk, 0) = 0x2000000;
				pk[7] = 0x68;
				int32_t d = (F<int32_t>(h, 0x60) >> 2) + depth;
				F<int32_t>(h, 0x60) = d;
				if (d < 0) F<int32_t>(h, 0x60) = 0;
				InsertPrimAutoDepth(var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + (uint32_t)F<int32_t>(h, 0x60) * 4 + 0x44, pk);
				pk += 0xC;
			}
		}
		FieldFree(0x64);
		return (uint32_t)pk;
	}

	static void StarsTwinkle()
	{
		uint8_t *p = (uint8_t *)(TexBase() + 0xDC3);
		for (int n = 0x400; n != 0; n--, p += 0x10)
		{
			const uint16_t v = (uint16_t)((int16_t)(int8_t)p[4] + (uint16_t)p[0]);
			if (v > 0xFF)
			{
				p[0] = (uint8_t)~(uint8_t)v;
				p[4] = (uint8_t)-(int8_t)p[4];
			}
			else p[0] = (uint8_t)v;
		}
		for (uint32_t g = game_addr(STARS) + 4; g < game_addr(game::unk_133AC4C); g += 0x10)
		{
			*(int16_t *)g = (int16_t)(*(int16_t *)g + *(const int16_t *)(g - 4));
			*(int16_t *)(g + 2) = (int16_t)(*(int16_t *)(g + 2) + *(const int16_t *)(g - 2));
		}
	}

	static void CasterLayout(MasterNode *m, int32_t step, bool scroll)
	{
		PrimArg arg;
		RotY(0, &arg.m);
		arg.m.t[0] = CasterPos()[0];
		arg.m.t[1] = 0;
		arg.m.t[2] = CasterPos()[2];
		ComposeAffineTransform(&Camera(), &arg.m, &arg.m);
		arg.scroll = scroll ? (int16_t)shl32(step, 5) : 0;
		arg.morph = TexBase() + 0x6CB8;
		arg.cursor = PacketCursor();
		arg.objs = scroll ? game_addr(OBJS23) : game_addr(OBJS0);
		arg.depth = 0;
		prim::play((prim::Layout *)m->layout_a, PartCallback, (int)&arg, Pause());
		PacketCursor() = arg.cursor;
	}

	static void BendBeam(const uint8_t *model, uint8_t *out, int32_t a, int32_t b)
	{
		const int16_t *v = (const int16_t *)(model + 8);
		int16_t *o = (int16_t *)out;
		for (int32_t n = *(const int32_t *)(model + 4); n != 0; n--, v += 4, o += 4)
		{
			o[0] = (int16_t)((mul32(*(const int16_t *)(game_addr(SINCOS) - (uint32_t)(v[2] & ~1) * 2), a) >> 12) + (int16_t)(v[0] << 1));
			o[1] = (int16_t)((mul32(*(const int16_t *)(game_addr(SINCOS) - (uint32_t)(v[2] & ~1) * 2), b) >> 12) + (int16_t)(v[1] << 1));
			o[2] = (int16_t)(v[2] << 3);
		}
	}

	static void Beam(const int16_t *p0, const int16_t *p1, int32_t a, int32_t b)
	{
		BendBeam((const uint8_t *)game_addr(game::unk_133822C), (uint8_t *)(TexBase() + 0x6CB8), a, b);
		uint8_t *h = (uint8_t *)FieldAlloc(0x9C);
		int32_t *d = (int32_t *)h;
		d[0] = p1[0] - p0[0];
		d[1] = p1[1] - p0[1];
		d[2] = p0[2] - p1[2];
		Mat4x3 *m = (Mat4x3 *)(h + 0x10);
		OrthoBasis(d, m);
		m->t[0] = p0[0];
		m->t[1] = p0[1];
		m->t[2] = p0[2];
		ComposeAffineTransform(&Camera(), m, m);
		GteSetRotMatrix(m);
		GteSetTransVector(m);
		uint8_t *p = h + 0x30;
		F<uint32_t>(p, 0) = game_addr(game::unk_133822C);
		F<uint32_t>(p, 0x1C) = 0x2030;
		F<uint32_t>(p, 4) = TexBase() + 0x6CB8;
		F<uint32_t>(p, 0x20) = 0;
		F<int32_t>(p, 0x18) = -0x800;
		PacketCursor() = RenderPrimSet(p, OT(0x44), 2, PacketCursor());
		FieldFree(0x9C);
	}

	static void TwistRay(const uint8_t *model, uint8_t *out, int32_t a, int32_t b)
	{
		const int16_t *v = (const int16_t *)(model + 8);
		int16_t *o = (int16_t *)out;
		for (int32_t n = *(const int32_t *)(model + 4); n != 0; n--, v += 4, o += 4)
		{
			const int32_t i = (mul32(v[2], b) >> 12) & 0xFFF;
			o[0] = (int16_t)((mul32(SinTab(i), a) >> 12) + v[0]);
			o[1] = v[1];
			o[2] = v[2];
		}
	}

	static void RayDraw(const int16_t *from, const int16_t *centre, const uint8_t *tab, int32_t t, int32_t u, int32_t sign, uint32_t model, int16_t spin)
	{
		int32_t fade = -0x800;
		uint8_t *h = (uint8_t *)FieldAlloc(0xDC);
		const int32_t uu = 0x1000 - u;
		const int32_t tt = 0x1000 - t;
		const int32_t ex = sdiv_pow2(mul32(*(const int16_t *)tab, tt), 12) + centre[0];
		const int32_t ey = sdiv_pow2(mul32(*(const int16_t *)(tab + 2), tt), 12) + centre[1];
		const int32_t ez = sdiv_pow2(mul32(*(const int16_t *)(tab + 4), tt), 12) + centre[2];
		int32_t *d = (int32_t *)h;
		d[0] = ex - from[0];
		d[1] = ey - from[1];
		d[2] = from[2] - ez;
		Mat4x3 *basis = (Mat4x3 *)(h + 0x10);
		const int32_t len = OrthoBasis(d, basis);
		int32_t bend = *(const int32_t *)(tab + 0xC) - len;
		int32_t length = len;
		Mat4x3 *m = (Mat4x3 *)(h + 0x30);
		if (uu < 0x800)
		{
			fade = uu * 2 - 0x1800;
			const int32_t c = 0x1000 - ComputeCos((uu - (uu >> 31)) >> 1);
			bend = mul32(bend, c) >> 12;
			length = sdiv_pow2(mul32(len, c), 12);
			RotY(spin, h + 0x50);
			MatMul3(basis, h + 0x50, m);
		}
		else
		{
			F<uint32_t>(h, 0x30) = F<uint32_t>(h, 0x10);
			F<uint32_t>(h, 0x34) = F<uint32_t>(h, 0x14);
			F<uint32_t>(h, 0x38) = F<uint32_t>(h, 0x18);
			F<uint32_t>(h, 0x3C) = F<uint32_t>(h, 0x1C);
			F<uint16_t>(h, 0x40) = F<uint16_t>(h, 0x20);
		}
		m->t[0] = ex;
		m->t[1] = ey;
		m->t[2] = ez;
		ComposeAffineTransform(&Camera(), m, m);
		GteSetRotMatrix(m);
		GteSetTransVector(m);
		uint8_t *p = h + 0x70;
		F<uint32_t>(p, 0x20) = 0;
		F<uint32_t>(p, 0x1C) = 0;
		F<uint32_t>(p, 0) = model;
		F<int32_t>(p, 0x18) = -0x800;
		PacketCursor() = RenderPrimSet(p, OT(0x44), 2, PacketCursor());
		TwistRay((const uint8_t *)game_addr(game::unk_133A0FC), (uint8_t *)(TexBase() + 0x6CB8), sdiv_pow2(mul32(bend, sign), 2), fade);
		basis->t[0] = from[0];
		basis->t[1] = from[1];
		basis->t[2] = from[2];
		d[1] = 0x2000;
		d[0] = 0x2000;
		d[2] = length;
		Scale3DMatrix(basis, d);
		ComposeAffineTransform(&Camera(), basis, m);
		GteSetRotMatrix(m);
		GteSetTransVector(m);
		F<uint32_t>(p, 0) = game_addr(game::unk_133A0FC);
		F<uint32_t>(p, 0x1C) = 0x2030;
		F<uint32_t>(p, 4) = TexBase() + 0x6CB8;
		F<int32_t>(p, 0x18) = -0x400;
		PacketCursor() = RenderPrimSet(p, OT(0x44), 2, PacketCursor());
		FieldFree(0xDC);
	}

	static void Ray(const int16_t *from, const int16_t *centre, uint8_t *tab, int32_t t, int32_t u, int32_t sign, uint32_t model)
	{
		if (0x1000 - u < 0x800 && Pause() == 0) *(int16_t *)(tab + 0x12) = (int16_t)(*(int16_t *)(tab + 0x12) + *(const int16_t *)(tab + 0x10));
		RayDraw(from, centre, tab, t, u, sign, model, *(const int16_t *)(tab + 0x12));
	}

	static void RingSpawn(const int16_t *pos, int32_t angle, int32_t phase, int32_t size, int32_t a, int32_t b, uint32_t from, uint32_t to)
	{
		const int32_t ab = a + b;
		if (size <= (ab - (ab >> 31)) >> 1) return;
		if (sdiv_pow2(mul32(NextLcg(), size), 14) >= ab) return;
		uint8_t rec[0x38];
		int16_t *p0 = (int16_t *)(rec + 0x10), *p1 = (int16_t *)(rec + 0x18);
		Polar(angle, size + a, p0);
		p0[0] = (int16_t)(p0[0] + pos[0]);
		p0[2] = (int16_t)(p0[2] + pos[2]);
		p0[1] = pos[1];
		Polar(angle + phase, size + b, p1);
		p1[0] = (int16_t)(p1[0] + pos[0]);
		p1[2] = (int16_t)(p1[2] + pos[2]);
		p1[1] = pos[1];
		int16_t *w = (int16_t *)rec;
		w[0x20 / 2] = (int16_t)((mul32(NextLcg(), 1000) >> 15) - 0x1F4);
		w[0x22 / 2] = (int16_t)((mul32(NextLcg(), 1000) >> 15) - 0x1F4);
		w[0x16 / 2] = 0x2710;
		w[0x26 / 2] = 0x200;
		w[0x30 / 2] = 0x3000;
		w[0x32 / 2] = (int16_t)0xFE00;
		w[0x24 / 2] = (int16_t)((mul32(NextLcg(), 1000) >> 15) - 0x1F4);
		w[0x2C / 2] = 0;
		w[0x2A / 2] = 0;
		w[0x28 / 2] = 0;
		w[0x1E / 2] = 0;
		w[0x2E / 2] = 0;
		w[0x28 / 2] = (int16_t)((mul32(NextLcg(), 1000) >> 15) - 0x1F4);
		w[0x2A / 2] = (int16_t)((mul32(NextLcg(), 1000) >> 15) - 0x1F4);
		w[0x36 / 2] = 0x1000;
		w[0x2C / 2] = (int16_t)((mul32(NextLcg(), 1000) >> 15) - 0x1F4);
		*(uint32_t *)(rec + 8) = from;
		*(uint32_t *)(rec + 0xC) = to;
		ChainNew(rec);
	}

	static void RingDraw(uint8_t *rec, const int16_t *pos, int32_t size, uint32_t model, int16_t spin);
	static void Ring(uint8_t *rec, const int16_t *pos, int32_t size, uint32_t model, int32_t spawn)
	{
		if (Pause() == 0)
		{
			if (spawn != 0)
			{
				int16_t *rpos = (int16_t *)(rec + 0x1C);
				const int32_t angle = *(const int16_t *)rec, a = *(const int16_t *)(rec + 4);
				RingSpawn(pos, angle, 0x400, size, a, *(const int16_t *)(rec + 6), (uint32_t)rpos, *(const uint32_t *)(rec + 0x10));
				RingSpawn(pos, angle, 0x800, sdiv_pow2(mul32(size, 0x16A1), 12), a, *(const int16_t *)(rec + 8), (uint32_t)rpos, *(const uint32_t *)(rec + 0x14));
				RingSpawn(pos, angle, 0xC00, size, a, *(const int16_t *)(rec + 0xA), (uint32_t)rpos, *(const uint32_t *)(rec + 0x18));
			}
			*(int16_t *)(rec + 0xC) = (int16_t)(*(int16_t *)(rec + 0xC) + *(const int16_t *)(rec + 0xE));
		}
		RingDraw(rec, pos, size, model, *(const int16_t *)(rec + 0xC));
	}

	static void RingDraw(uint8_t *rec, const int16_t *pos, int32_t size, uint32_t model, int16_t spin)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x94);
		Mat4x3 *m = (Mat4x3 *)(h + 8);
		RotY(*(const int16_t *)rec, m);
		int16_t *z = (int16_t *)h;
		z[1] = 0;
		z[0] = 0;
		z[2] = 0x1000;
		int32_t *o = (int32_t *)(h + 0x1C);
		Apply3D(m, z, o);
		const int32_t r = *(const int16_t *)(rec + 4) + size;
		o[0] = sdiv_pow2(mul32(o[0], r), 12) + pos[0];
		*(int16_t *)(rec + 0x1C) = (int16_t)o[0];
		o[1] = sdiv_pow2(mul32(o[1], r), 12) + pos[1];
		*(int16_t *)(rec + 0x1E) = (int16_t)o[1];
		o[2] = sdiv_pow2(mul32(o[2], r), 12) + pos[2];
		*(int16_t *)(rec + 0x20) = (int16_t)o[2];
		RotY(spin, m);
		ComposeAffineTransform(&Camera(), m, m);
		GteSetRotMatrix(m);
		GteSetTransVector(m);
		uint8_t *p = h + 0x28;
		F<uint32_t>(p, 0x20) = 0;
		F<uint32_t>(p, 0) = model;
		F<uint32_t>(p, 0x1C) = 0;
		F<int32_t>(p, 0x18) = *(const int16_t *)(rec + 2);
		PacketCursor() = RenderPrimSet(p, OT(0x44), 2, PacketCursor());
		FieldFree(0x94);
	}

	inline uint32_t LayoutData(uint32_t slot) { return TexBase() + 0x27690 + *(const uint32_t *)(TexBase() + slot); }

	static int32_t BackdropSize(uint32_t s)
	{
		const uint32_t x = s << 11, h = mulhi_u(x, 0x8421085u);
		return (int32_t)((((x - h) >> 1) + h) >> 4);
	}

	static void BeamBends(uint32_t s, int32_t *pa, int32_t *pb)
	{
		int32_t a, b;
		if (s < 0x17)
		{
			const int32_t q = sdiv_pow2((int32_t)(mulhi_u(s << 12, 0xB21642C9u) >> 4), 2);
			b = shl32(ComputeSin(q), 1);
			a = (b - (b >> 31)) >> 1;
		}
		else if (s - 0x17 < 2)
		{
			const int32_t e = (int32_t)(s - 0x17);
			b = 0x1556 - e * 0x111 * 10;
			a = (b - (b >> 31)) >> 1;
		}
		else
		{
			const uint32_t e = (s - 0x17 + 0xFFFFEu) << 12;
			const int32_t q = sdiv_pow2((int32_t)(mulhi_u(e, 0xAAAAAAABu) >> 5), 2);
			const int32_t c1 = ComputeCos(q);
			const int32_t c2 = ComputeCos(sdiv_pow2(0x1000 - c1, 2));
			a = 0;
			b = shl32(ComputeSin(sdiv_pow2(0x1000 - c2, 2)), 1);
		}
		*pa = a;
		*pb = b;
	}

	static void RayParams(uint32_t s, int32_t *t, int32_t *u)
	{
		*t = (int32_t)(mulhi_u(s * 3000u, 0x88888889u) >> 5);
		*u = (int32_t)(mulhi_u(s << 12, 0x88888889u) >> 5);
	}

	static int32_t RingSize(uint32_t s)
	{
		const int32_t g = (int32_t)(mulhi_u(s << 12, 0xF0F0F0F1u) >> 6);
		int32_t v;
		if (g < 0x5DC) v = ComputeSin(g >> 2);
		else
		{
			const int32_t b = ComputeSin(0x177);
			const int32_t q = mul32(0x1000 - b, g - 0x5DC);
			int32_t d = mulhi_s(q, 0x64FADF43) >> 10;
			d += (int32_t)((uint32_t)d >> 31);
			v = d + b;
		}
		return sdiv_pow2(mul32(0x1000 - v, 5000), 12) + 0x138;
	}

	static int32_t OverlayScale(uint32_t s)
	{
		const uint32_t x = s << 10, h = mulhi_u(x, 0xD41D41D5u);
		const int32_t a0 = (int32_t)((((x - h) >> 1) + h) >> 6);
		const int32_t c1 = ComputeCos(a0);
		const int32_t c2 = ComputeCos((0x1000 - c1) >> 2);
		const int32_t c3 = ComputeCos((0x1000 - c2) >> 2);
		const int32_t c4 = ComputeCos((0x1000 - c3) >> 2);
		return 0x1200 - c4;
	}

	static void MirroredLayouts(MasterNode *m, bool roty)
	{
		PrimArg arg;
		if (roty) RotY(0x320, &arg.m);
		else RotYScaled(-0x320, 0x2000, &arg.m);
		BonePoint32((const int16_t *)game_addr(BONE_L), arg.m.t);
		ComposeAffineTransform(&Camera(), &arg.m, &arg.m);
		arg.morph = TexBase() + 0x6CB8;
		arg.cursor = PacketCursor();
		arg.objs = game_addr(OBJS0);
		arg.depth = 0;
		arg.scroll = 0;
		prim::play((prim::Layout *)m->layout_a, PartCallback, (int)&arg, Pause());
		PacketCursor() = arg.cursor;
		if (roty) RotY(0x320, &arg.m);
		else RotYScaled(-0x320, 0x2000, &arg.m);
		arg.m.m[0][0] = (int16_t)-arg.m.m[0][0];
		arg.m.m[0][1] = (int16_t)-arg.m.m[0][1];
		arg.m.m[0][2] = (int16_t)-arg.m.m[0][2];
		BonePoint32((const int16_t *)game_addr(BONE_R), arg.m.t);
		ComposeAffineTransform(&Camera(), &arg.m, &arg.m);
		arg.cursor = PacketCursor();
		prim::play((prim::Layout *)m->layout_b, PartCallback, (int)&arg, Pause());
		PacketCursor() = arg.cursor;
	}

	static void SetKeys(int16_t *k, int32_t a0, int32_t a1, int32_t a2, int32_t a3, int32_t a4, int32_t a5)
	{
		k[0] = (int16_t)a0; k[1] = (int16_t)a1; k[2] = (int16_t)a2; k[3] = (int16_t)a3; k[4] = (int16_t)a4; k[5] = (int16_t)a5;
	}

	static void SetCamera(int32_t lx, int32_t ly, int32_t lz, int32_t ex, int32_t ey, int32_t ez)
	{
		int16_t *look = (int16_t *)game_addr(game::Battle_Camera_LookAt_XZ_s16), *eye = (int16_t *)game_addr(game::Battle_Camera_world_XZ_s16);
		look[0] = (int16_t)lx; look[1] = (int16_t)ly; look[2] = (int16_t)lz;
		eye[0] = (int16_t)ex; eye[1] = (int16_t)ey; eye[2] = (int16_t)ez;
	}

	static void MasterCamera(int32_t c)
	{
		int16_t k0[6], k1[6];
		if ((uint32_t)c < 0x1F)
		{
			if (c == 0)
			{
				CameraArmReturn();
				CameraZoom() = 0;
			}
			const int32_t x = CasterPos()[0], z = CasterPos()[2];
			SetKeys(k0, x - 0x11, 0xEF5D, z - 0xDAB, x + 0x31E, 0xFDD1, z + 0x1C93);
			SetKeys(k1, x + 0x24, 0xFC11, z + 0x1A7F, x - 0x53, 0xFA78, z + 0x365F);
			CameraBlend(k0, k1, 0, 0, ComputeSin((int32_t)(mulhi_u((uint32_t)c << 12, 0x88888889u) >> 6)));
		}
		else if (c == 0x1F)
		{
			const int32_t x = CasterPos()[0], z = CasterPos()[2];
			SetCamera(x - 0x28, 0xF1B9, z + 0x51E, x - 0x109, 0xEBF8, z + 0x1B23);
		}
		const int32_t x = CasterPos()[0], z = CasterPos()[2];
		if (c == 0x36) SetCamera(x, 0xFC1C, z + 0x1924, x, 0xEC22, z - 0x62BA);
		if ((uint32_t)(c - 0x42) < 0x1E)
		{
			SetKeys(k0, x + 0x30E, 0x26A, z - 0x99, x + 0x349, 0xFD53, z + 0x132B);
			SetKeys(k1, x + 0x2C, 0xF, z - 0xE2, x + 0xFD, 0xFF83, z + 0x8A3);
			CameraKeys(k0, k1, 0, 0, c - 0x42, 0x1E);
		}
		if ((uint32_t)(c - 0x60) < 0x1A)
		{
			SetKeys(k0, x - 0x5FA, 0x9D, z + 0x444, x + 0x628, 0xFF54, z + 0xBB3);
			SetKeys(k1, x - 0x2E0, 0x1F, z + 0x105, x + 0x252, 0x1E, z + 0x8E0);
			CameraKeys(k0, k1, -0x40, 0x120, c - 0x60, 0x1A);
		}
		if ((uint32_t)(c - 0x7A) < 0x16)
		{
			SetKeys(k0, x - 0x2E0, 0x1F, z + 0x105, x + 0x252, 0x1E, z + 0x8E0);
			SetKeys(k1, x - 0x205, 0xE6, z + 0x96, x + 0x24A, 0xFE57, z + 0x718);
			CameraKeys(k0, k1, -0x4A0, -0x560, c - 0x7A, 0x16);
		}
		if ((uint32_t)(c - 0x90) < 0x16)
		{
			SetKeys(k0, x - 0x98, 0xFFA6, z + 0x146, x + 0xDC, 0x35, z + 0x4A0);
			SetKeys(k1, x + 0x46, 0xFEA2, z + 0x147, x + 0x3CD, 0xAA, z + 0xBA7);
			CameraKeys(k0, k1, -0x9C0, -0x9C0, c - 0x90, 0x16);
		}
		if (c == 0xA6)
		{
			SetCamera(x + 0x17C, 0xF4C9, z + 0xFD7, x - 0x1464, 0xD6CE, z + 0x29FB);
			CameraZoom() = 0;
		}
		if ((uint32_t)(c - 0xC5) < 0x28)
		{
			SetKeys(k0, x + 0x179, 0xF745, z + 0xEB0, x + 0x1CE, 0xF495, z + 0x3392);
			SetKeys(k1, x + 0x149, 0x10A, z - 0x21E, x + 0x193, 0xE164, z + 0x2D52);
			CameraKeys(k0, k1, 0, 0, c - 0xC5, 0x28);
		}
		if (c == 0xED)
		{
			SetCamera(x + 0x10C, 0xFF65, z - 0x32, x - 0x11B, 0x13F, z + 0x5A7);
			CameraZoom() = 0x6C0;
		}
		else if (c == 0x107)
		{
			SetCamera(x - 0xC0, 0xFF99, z + 0x46, x + 0x209, 0x83, z + 0x59E);
			CameraZoom() = 0x4E0;
		}
		else if (c == 0x121)
		{
			SetCamera(x - 0xC0, 0xFF99, z + 0x46, x + 0x204, 0x7D, z + 0x599);
			CameraZoom() = 0x1000;
		}
		else if (c == 0x13B)
		{
			SetCamera(x + 0x141, 0x4B, z - 0x65, x + 0x26, 0x8D, z + 0x6A0);
			CameraZoom() = 0xEA0;
		}
		const int32_t cx = Centre()[0], cz = Centre()[2];
		if ((uint32_t)(c - 0x155) < 0x3C)
		{
			SetKeys(k0, cx - 0x20, 0xA2A, cz - 0x2E7, cx + 0x1A0, 0xD7D3, cz - 0x431D);
			SetKeys(k1, cx - 0x64, 0xFB94, cz + 0x66E, cx + 0xED, 0xE95A, cz - 0x308A);
			CameraKeys(k0, k1, 0, 0, c - 0x155, 0x3C);
		}
		if ((uint32_t)(c - 0x191) < 0x44)
		{
			SetKeys(k0, cx - 0x4B, 0xFF2C, cz + 0xBFA, cx + 0x2A, 0xFB39, cz + 0x36A);
			SetKeys(k1, cx - 0x389, 0xCB, cz + 0xA90, cx + 0x8F, 0xDEB4, cz - 0x13F9);
			const int32_t a = (int32_t)(mulhi_u((uint32_t)(c - 0x191) << 10, 0xF0F0F0F1u) >> 6);
			CameraBlend(k0, k1, 0, 0, 0x1000 - ComputeCos(a));
		}
		if (c == 0x1D7) SetCamera(cx, 0, cz + 0xA99, cx, 0xE83F, cz + 0x38);
	}

	static uint32_t __cdecl MasterTask(TaskNode *n)
	{
		MasterNode *m = (MasterNode *)n;
		{
			const uint32_t s = (uint32_t)(m->counter - 1);
			if (s < 0x1F)
			{
				const int32_t size = BackdropSize(s);
				if (s < 10) *Caster() |= 8;
				else *(uint16_t *)Caster() &= 0xFFF7;
				PacketCursor() = Billboard(CasterPos()[0], -1000, CasterPos()[2] - 1000, 0, size, 0, 0x800C, 0x800, game_addr(game::unk_1334FFC), PacketCursor());
				PacketCursor() = Billboard(CasterPos()[0], -1000, CasterPos()[2] - 1000, 0, size, shl32((int32_t)s, 5), 0x8030, -0x200, game_addr(game::unk_1336924), PacketCursor());
				PacketCursor() = Stars(0, CasterPos()[0], 0, CasterPos()[2], sdiv_pow2(size, 2), PacketCursor());
				if (Pause() == 0) *(int16_t *)(Caster() + 0x20) = (int16_t)(*(int16_t *)(Caster() + 0x20) - 0x142);
			}
		}
		if ((uint32_t)(m->counter - 0x20) < 0x1FE)
		{
			*(uint8_t *)game_addr(game::unk_1D98A15) &= 0xFD;
			FrameArena() = Billboard(CasterPos()[0], 0, CasterPos()[2], 0, 0x1000, 0, 0xC, 0, game_addr(game::unk_1334FFC), FrameArena());
			FrameArena() = Stars(0, CasterPos()[0], 0, CasterPos()[2], 0x10000, FrameArena());
		}
		{
			const uint32_t s = (uint32_t)(m->counter - 0x2A);
			if (s < 0xD)
			{
				if (s == 0)
				{
					DecodeModelPrimLayout(LayoutData(0x27694), m->layout_a, 0x3AC);
					DecodeModelPrimLayout(LayoutData(0x27694), m->layout_b, 0x3AC);
				}
				MirroredLayouts(m, false);
			}
		}
		{
			const uint32_t s = (uint32_t)(m->counter - 0x37);
			if (s < 0xC)
			{
				if (s == 0)
				{
					DecodeModelPrimLayout(LayoutData(0x27698), m->layout_a, 0x1AC);
					DecodeModelPrimLayout(LayoutData(0x27698), m->layout_b, 0x1AC);
				}
				MirroredLayouts(m, true);
			}
		}
		{
			static const struct { int16_t start, len; uint32_t slot, size; } P[4] = {
				{ 0x43, 0x1E, 0x2769C, 0x280 }, { 0x61, 0x1A, 0x276A0, 0x280 }, { 0x7B, 0x16, 0x276A4, 0x280 }, { 0x91, 0x16, 0x276A8, 0x9C4 } };
			for (int k = 0; k < 4; k++)
			{
				const int32_t s = m->counter - P[k].start;
				if ((uint32_t)s >= (uint32_t)P[k].len) continue;
				if (s == 0)
				{
					DecodeModelPrimLayout(LayoutData(P[k].slot), m->layout_a, (int32_t)P[k].size);
					if (k == 0)
					{
						*Caster() |= 8;
						ModelsFlag2Off();
						PartyShadowOn();
					}
				}
				CasterLayout(m, s, true);
			}
		}
		{
			uint32_t s = (uint32_t)(m->counter - 0xA7);
			if (s < 0x47)
			{
				if (s == 0) *(uint16_t *)Caster() &= 0xFFF3;
				if (Pause() == 0)
				{
					uint32_t e = s - 0xD;
					int16_t *z = (int16_t *)(Caster() + 0x20);
					if (e < 0x10)
					{
						if (e >= 8)
						{
							const int32_t amp = ComputeSin(0x400) / 6;
							const int32_t s1 = ComputeSin((int32_t)((uint32_t)((e + 0x3FFFF8u) << 10) >> 3));
							const int32_t s2 = ComputeSin(s1 >> 2);
							*z = (int16_t)(*z + (int16_t)sdiv_pow2(mul32(0x1000 - s2, amp), 12));
						}
						else *z = (int16_t)(*z + (int16_t)(ComputeSin((int32_t)((e << 11) >> 4)) / 6));
					}
					else
					{
						e -= 0x10;
						if (e < 0x10)
						{
							const int32_t v = ComputeSin((int32_t)(mulhi_u(e << 11, 0xCCCCCCCDu) >> 4));
							int32_t d = mulhi_s(v, (int32_t)0xC71C71C7) >> 2;
							d += (int32_t)((uint32_t)d >> 31);
							*z = (int16_t)(*z + (int16_t)d);
						}
						else if (e < 0x100) *z = (int16_t)(*z - 0xAA);
					}
				}
				int32_t a, b;
				BeamBends(s, &a, &b);
				int16_t r0[3], l0[3], p[3];
				BonePoint16((const int16_t *)game_addr(BONE_R), r0);
				const int16_t *cp = CasterPos();
				p[0] = (int16_t)(cp[0] + 0x2710); p[1] = (int16_t)(cp[1] - 0x2710); p[2] = (int16_t)(cp[2] - 0x7D0);
				Beam(r0, p, a, b);
				p[0] = (int16_t)(cp[0] + 0x2710); p[1] = (int16_t)(cp[1] - 0x4E20); p[2] = (int16_t)(cp[2] - 0x1388);
				Beam(r0, p, a, b);
				BonePoint16((const int16_t *)game_addr(BONE_L), l0);
				a = -a;
				p[0] = (int16_t)(cp[0] - 0x2710); p[1] = (int16_t)(cp[1] - 0x2710); p[2] = (int16_t)(cp[2] - 0x7D0);
				Beam(l0, p, a, b);
				p[0] = (int16_t)(cp[0] - 0x2710); p[1] = (int16_t)(cp[1] - 0x4E20); p[2] = (int16_t)(cp[2] - 0x1388);
				Beam(l0, p, a, b);
			}
		}
		{
			static const struct { int16_t start; uint32_t slot, size; } P[4] = {
				{ 0xEE, 0x27694, 0x120 }, { 0x108, 0x27698, 0x120 }, { 0x122, 0x2769C, 0x120 }, { 0x13C, 0x276A0, 0x85C } };
			for (int k = 0; k < 4; k++)
			{
				const int32_t s = m->counter - P[k].start;
				if ((uint32_t)s >= 0x1A) continue;
				if (s == 0)
				{
					DecodeModelPrimLayout(LayoutData(P[k].slot), m->layout_a, (int32_t)P[k].size);
					if (k == 0)
					{
						*Caster() |= 8;
						PartyShadowOn();
					}
				}
				CasterLayout(m, s, false);
			}
		}
		{
			const uint32_t s = (uint32_t)(m->counter - 0x156);
			if (s < 0x3C)
			{
				int32_t t, u;
				RayParams(s, &t, &u);
				if (s == 0)
				{
					for (int32_t i = 0; i < m->ntargets; i++)
					{
						Target &tg = m->targets[i];
						const int16_t *tp = (const int16_t *)&tg.pos_xy;
						const int32_t y = sdiv_pow2(shl32(tp[1] - m->centre[1], 11), 12);
						const int32_t z = sdiv_pow2(shl32(*(const int16_t *)&tg.pos_z - m->centre[2], 11), 12);
						const int32_t x = sdiv_pow2(shl32(tp[0] - m->centre[0], 11), 12);
						*(int16_t *)(tg.entity + 0x1C) = (int16_t)(x + m->centre[0]);
						*(int16_t *)(tg.entity + 0x1E) = (int16_t)(y + m->centre[1]);
						*(int16_t *)(tg.entity + 0x20) = (int16_t)(z + m->centre[2] + 0x7D0);
						*(uint32_t *)(tg.entity + 0x30) = game_addr(ENTITY_SCALE);
					}
					const int16_t *cxy = (const int16_t *)&m->caster_xy;
					const int32_t y = sdiv_pow2(shl32(cxy[1] - m->centre[1], 11), 12);
					const int32_t z = sdiv_pow2(shl32(*(const int16_t *)&m->caster_z - m->centre[2], 11), 12);
					const int32_t x = sdiv_pow2(shl32(cxy[0] - m->centre[0], 11), 12);
					*(int16_t *)(Caster() + 0x1C) = (int16_t)(x + m->centre[0]);
					*(int32_t *)(Caster() + 0x54) = *(const int16_t *)(Caster() + 0x1C);
					*(int16_t *)(Caster() + 0x1E) = (int16_t)(y + m->centre[1]);
					*(int32_t *)(Caster() + 0x58) = *(const int16_t *)(Caster() + 0x1E);
					*(int16_t *)(Caster() + 0x20) = (int16_t)(z + m->centre[2] - 0x7D0);
					*(int32_t *)(Caster() + 0x5C) = *(const int16_t *)(Caster() + 0x20);
					*(uint32_t *)(Caster() + 0x30) = game_addr(ENTITY_SCALE);
					RestoreEntityFlag4();
					EnemyShadowOn();
					*(uint16_t *)Caster() &= 0xFFF3;
				}
				if (Pause() == 0) *(int16_t *)(Caster() + 0x20) = (int16_t)(*(int16_t *)(Caster() + 0x20) - 0xAA);
				int16_t c[3] = { m->centre[0], m->centre[1], (int16_t)(m->centre[2] + 0x7D0) };
				int16_t r0[3], l0[3];
				BonePoint16((const int16_t *)game_addr(BONE_R), r0);
				Ray(r0, c, (uint8_t *)game_addr(game::unk_133AC40), t, u, -1, TexBase() + 0x36620);
				Ray(r0, c, (uint8_t *)game_addr(game::unk_133AC54), t, u, 1, TexBase() + 0x393C8);
				BonePoint16((const int16_t *)game_addr(BONE_L), l0);
				Ray(l0, c, (uint8_t *)game_addr(game::unk_133AC68), t, u, 1, TexBase() + 0x37558);
				Ray(l0, c, (uint8_t *)game_addr(game::unk_133AC7C), t, u, -1, TexBase() + 0x38490);
			}
		}
		{
			const uint32_t s = (uint32_t)(m->counter - 0x192);
			if (s < 0x44)
			{
				Feedback();
				if (s == 0)
				{
					Flags()[0] = 1;
					Flags()[1] = 1;
					RibbonsInit(7, 0x1000, -0x800);
					SpritesInit();
					InitTaskQueuePool(QLayouts(), (void *)(TexBase() + 0x3D430), 0x1B8, 8);
				}
				int16_t c[3] = { m->centre[0], m->centre[1], (int16_t)(m->centre[2] + 0x7D0) };
				const int32_t size = RingSize(s);
				const int32_t spawn = s >= 6 ? 1 : 0;
				Ring((uint8_t *)game_addr(game::unk_133AC90), c, size, TexBase() + 0x36620, spawn);
				Ring((uint8_t *)game_addr(game::unk_133ACB4), c, size, TexBase() + 0x393C8, spawn);
				Ring((uint8_t *)game_addr(game::unk_133ACD8), c, size, TexBase() + 0x38490, spawn);
				Ring((uint8_t *)game_addr(game::unk_133ACFC), c, size, TexBase() + 0x37558, spawn);
				if (Pause() == 0 && size < 0x682)
				{
					if (s == 0x3A)
					{
						LayoutNode *L = (LayoutNode *)AddTaskToQueue(QLayouts(), game_addr(ORIG_LayoutTask));
						if (L)
						{
							L->depth = -0x800;
							L->pos[0] = c[0];
							L->pos[1] = c[1];
							L->pos[2] = c[2];
							L->scale = 0x2000;
							DecodeModelPrimLayout(LayoutData(0x276A8), L->layout, 0x1A0);
						}
					}
					if (size < 0x341)
					{
						LayoutNode *L = (LayoutNode *)AddTaskToQueue(QLayouts(), game_addr(ORIG_LayoutTask));
						if (L)
						{
							L->depth = -0x800;
							L->pos[0] = (int16_t)(sdiv_pow2(mul32(CrtRand(), 10000), 15) + c[0] - 0x1388);
							L->pos[1] = (int16_t)(sdiv_pow2(mul32(CrtRand(), 10000), 15) + c[1] - 0x1388);
							L->pos[2] = (int16_t)(sdiv_pow2(mul32(CrtRand(), 10000), 15) + c[2] - 0x1388);
							if (s & 1)
							{
								L->scale = (NextLcg() >> 4) + 0xC00;
								DecodeModelPrimLayout(LayoutData(0x276A8), L->layout, 0x1A0);
							}
							else
							{
								L->scale = (NextLcg() >> 4) + 0x800;
								DecodeModelPrimLayout(LayoutData(0x276A4), L->layout, 0x78);
							}
						}
					}
					else
					{
						SpriteP *p = SpriteAlloc();
						const int32_t angle = shl32((int32_t)s, 10);
						for (int k = 8; p; )
						{
							const int32_t r = sdiv_pow2(mul32(CrtRand() - 0x4000, 0x7D0), 15) + 0x7D0;
							Polar(angle, r, p->pos);
							p->pos[0] = (int16_t)(p->pos[0] + (int16_t)((CrtRand() & 0x1FF) + c[0] - 0x100));
							p->pos[1] = (int16_t)((CrtRand() & 0x1FF) + c[1] - 0x100);
							const int32_t rz = CrtRand();
							p->life = 8;
							p->pos[2] = (int16_t)(p->pos[2] + (int16_t)((rz & 0x1FF) + c[2] - 0x100));
							if (--k == 0) break;
							p = SpriteAlloc();
						}
					}
				}
			}
		}
		if ((uint32_t)(m->counter - 0x1D2) < 0x1F)
		{
			FadeTile(m->counter - 0x1D2, 0xFF, 0xFF, 0xFF, 4, 2, 0x19);
		}
		{
			const int32_t s = m->counter - 0x1D8;
			if ((uint32_t)s < 0x46)
			{
				Feedback();
				if (s == 0)
				{
					DecodeModelPrimLayout(LayoutData(0x276AC), m->layout_a, 0xC8);
					QLayouts()->tail = nullptr;
					QLayouts()->head = nullptr;
					PartyShadowOn();
					PModelsInit();
					QuadsInit();
					Flags()[0] = 0;
					Flags()[1] = 0;
					Flags()[2] = 1;
					Flags()[3] = 1;
					*Caster() |= 4;
					const int16_t c[3] = { m->centre[0], m->centre[1], (int16_t)(m->centre[2] + 0x7D0) };
					for (int32_t k = 0x80; k != 0; k--)
					{
						PModel *p = PModelAlloc();
						p->pos[0] = c[0];
						p->pos[1] = c[1];
						p->pos[2] = c[2];
						const int32_t pitch = sdiv_pow2(mul32(CrtRand(), 0x640), 15) - 0x320;
						const int32_t yaw = (CrtRand() & 0xFFF) - 0x800;
						DirectionFromAngles(pitch, yaw, p->vel);
						const int32_t sp = (CrtRand() & 0x3F) + 0x20;
						p->life = (uint8_t)(0x8000 / sp);
						const int16_t vx = p->vel[0];
						p->pos[0] = (int16_t)(p->pos[0] + (int16_t)(vx >> 4));
						const int16_t vy = p->vel[1];
						p->vel[0] = (int16_t)(mul32(vx, sp) >> 12);
						const int32_t vz32 = mul32(p->vel[2], sp);
						const int32_t vy32 = mul32(vy, sp);
						p->pos[1] = (int16_t)(p->pos[1] + (int16_t)(vy >> 4));
						p->pos[2] = (int16_t)(p->pos[2] + (int16_t)(p->vel[2] >> 4));
						p->vel[1] = (int16_t)(vy32 >> 12);
						p->vel[2] = (int16_t)(vz32 >> 12);
						p->scale = (int16_t)((CrtRand() & 0x3FFF) + 0x400);
						p->rot[0] = (int16_t)CrtRand();
						p->rot[1] = (int16_t)CrtRand();
						p->drot[0] = (int8_t)((CrtRand() & 0x7F) - 0x40);
						p->drot[1] = (int8_t)((CrtRand() & 0x7F) - 0x40);
						p->model = (int8_t)(CrtRand() & 7);
					}
				}
				if (Pause() == 0)
				{
					QuadP *q = QuadAlloc();
					for (int k = 2; q; )
					{
						const int32_t rx = sdiv_pow2(mul32(CrtRand(), 300), 15) + 0xC8;
						const int32_t rz = CrtRand();
						Mat4x3 mx, mz;
						RotX(rx, &mx);
						RotZ(rz, &mz);
						MatMul3(&mz, &mx, q->m);
						q->vel[0] = (int16_t)(-(int32_t)q->m[2] >> 5);
						q->vel[1] = (int16_t)(-(int32_t)q->m[5] >> 5);
						q->vel[2] = (int16_t)(-(int32_t)q->m[8] >> 5);
						*(uint32_t *)q->pos = 0;
						q->pos[2] = 0x1000;
						q->life = 0x20;
						if (--k == 0) break;
						q = QuadAlloc();
					}
				}
				PrimArg arg;
				RotY(0, &arg.m);
				arg.m.t[0] = 0;
				arg.morph = TexBase() + 0x6CB8;
				arg.m.t[1] = 0;
				arg.m.t[2] = 0x1000;
				arg.objs = game_addr(OBJS0);
				arg.depth = -0x400;
				arg.scroll = 0;
				Scale() = OverlayScale(s);
				arg.cursor = FrameArena();
				prim::play((prim::Layout *)m->layout_a, ScaledPartCallback, (int)&arg, Pause());
				FrameArena() = arg.cursor;
			}
		}
		int32_t zero = 0;
		{
			const int32_t s = m->counter - 0x214;
			if ((uint32_t)s < 0x14)
			{
				FadeTile(s, 0xFF, 0xFF, 0xFF, 8, 4, 8);
				if (s == 0xA)
				{
					*(uint32_t *)game_addr(game::Battle_Camera_LookAt_XZ_s16) = *(const uint32_t *)game_addr(game::Battle_Camera_ReturnView_LookAt_XZ_s16);
					*(uint32_t *)game_addr(game::Battle_Camera_LookAt_Y) = *(const uint32_t *)game_addr(game::Battle_Camera_ReturnView_LookAt_Y);
					*(uint32_t *)game_addr(game::Battle_Camera_world_XZ_s16) = *(const uint32_t *)game_addr(game::Battle_Camera_ReturnView_world_XZ_s16);
					Flags()[2] = 0;
					Flags()[3] = 0;
					*(uint32_t *)game_addr(game::Battle_Camera_world_Y) = *(const uint32_t *)game_addr(game::Battle_Camera_ReturnView_world_Y);
					CameraZoom() = *(const int16_t *)game_addr(game::Battle_Camera_ReturnView_Roll);
					for (int32_t i = 0; i < m->ntargets; i++)
					{
						uint8_t *e = m->targets[i].entity;
						*(uint32_t *)(e + 0x1C) = m->targets[i].pos_xy;
						*(uint32_t *)(e + 0x20) = m->targets[i].pos_z;
						*(uint32_t *)(e + 0x30) = 0;
					}
					*(uint16_t *)Caster() &= 0xFFF7;
					*(uint32_t *)(Caster() + 0x1C) = m->caster_xy;
					*(uint32_t *)(Caster() + 0x20) = m->caster_z;
					*(uint32_t *)(Caster() + 0x30) = 0;
					ModelsFlag2On();
					RestoreEntityFlag4();
					QueueChainTransformation(Caster(), 1);
				}
			}
		}
		if (Flags()[0])
		{
			RibbonsDraw();
		}
		if (Flags()[1])
		{
			SpritesDraw();
		}
		else
		{
			if (Flags()[2])
			{
				PModelsDraw();
			}
			if (Flags()[3])
			{
				QuadsDraw();
			}
		}
		const int16_t c = m->counter;
		if (c == 0)
		{
			StreamLoad(0x187, TexBase() + 0x27690, 2);
			StreamLoad(0x188, TexBase() + 0x27690, 2);
			StreamLoad(0x189, TexBase() + 0x27690, 5);
		}
		else if (c == 0x1F)
		{
			if (StreamBusy()) return 0;
		}
		else if (c == 0xA6) StreamLoad(0x18A, TexBase() + 0x27690, 5);
		else if (c == 0xED)
		{
			if (StreamBusy()) return 0;
		}
		else if (c == 0x21F)
		{
			StreamLoad(0x168, TexBase() + 0x7648, 1);
			StreamLoad(0x169, TexBase() + 0x7648, 1);
			StreamLoad(0x16A, TexBase() + 0x7648, zero);
		}
		StreamPump();
		if (Flags()[0]) RibbonsUpdate();
		if (Flags()[1]) SpritesUpdate();
		else
		{
			if (Flags()[2]) PModelsUpdate();
			if (Flags()[3]) QuadsUpdate();
		}
		StarsTwinkle();
		if (m->counter == 0x224) ReleaseVoice(m->voice);
		switch (m->counter)
		{
		case 0: BdPlaySE((const void *)game_addr(game::unk_133AB6C), 0, 0x80); break;
		case 0x61: BdPlaySE((const void *)game_addr(game::unk_133AB70), 0, 0x80); break;
		case 0xA7: BdPlaySE((const void *)game_addr(game::unk_133AB74), 0, 0x80); break;
		case 0xEE: BdPlaySE((const void *)game_addr(game::unk_133AB78), 0, 0x80); break;
		case 0x156: BdPlaySE((const void *)game_addr(game::unk_133AB7C), 0, 0x80); break;
		case 0x1C9: BdPlaySE((const void *)game_addr(game::unk_133AB80), 0, 0x80); break;
		default: break;
		}
		if (m->counter == 0x20)
		{
			for (int32_t i = 0; i < m->ntargets; i++)
			{
				uint8_t *e = m->targets[i].entity;
				*(uint32_t *)(e + 0x1C) = m->targets[i].pos_xy;
				*(uint32_t *)(e + 0x20) = m->targets[i].pos_z;
				*(int16_t *)(e + 0x20) = (int16_t)(*(int16_t *)(e + 0x20) + 0x2710);
			}
			*(uint32_t *)(Caster() + 0x1C) = m->caster_xy;
			*(uint32_t *)(Caster() + 0x20) = m->caster_z;
		}
		MasterCamera(m->counter);
		if (m->counter == 0x224)
		{
			const ActionData *act = *(const ActionData *const *)(Ctx() + 4);
			ApplyActionResultToTargets(*(uint8_t *const *)((const uint8_t *)act + 8), *((const uint8_t *)act + 0x10));
		}
		m->counter++;
		if (m->counter >= 0x228)
		{
			if (StreamPending(2)) return TASK_END;
			m->counter = 0x232;
		}
		return 0;
	}

	static uint32_t __cdecl LayoutTask(TaskNode *n)
	{
		LayoutNode *L = (LayoutNode *)n;
		PrimArg arg;
		arg.morph = TexBase() + 0x6CB8;
		ff8fx::fill<(sizeof(arg.m.m))>(&arg.m, 0);
		arg.m.m[0][0] = arg.m.m[1][1] = arg.m.m[2][2] = 0x1000;
		arg.m.pad = 0;
		arg.m.t[0] = L->pos[0];
		arg.m.t[1] = L->pos[1];
		arg.m.t[2] = L->pos[2];
		ComposeAffineTransform(&Camera(), &arg.m, &arg.m);
		Scale() = L->scale;
		arg.depth = L->depth;
		arg.objs = game_addr(OBJS0);
		arg.cursor = FrameArena();
		arg.scroll = 0;
		const int left = prim::play((prim::Layout *)L->layout, ScaledPartCallback, (int)&arg, Pause());
		FrameArena() = arg.cursor;
		return left != 0 ? 0 : TASK_END;
	}

	static uint32_t __cdecl RootTask(TaskNode *n)
	{
		RootNode *r = (RootNode *)n;
		if (r->parity)
		{
			PacketCursor() = TexBase() + 0x7648;
			r->parity = 0;
		}
		else
		{
			PacketCursor() = TexBase() + 0x17648;
			r->parity = 1;
		}
		if (r->counter == 1 && Pause() == 0 && !r->started)
		{
			r->started = 1;
			SaveEntityFlags();
			InitTaskQueuePool(QMaster(), (void *)TexBase(), 0xDB8, 1);
			MasterNode *m = (MasterNode *)AddTaskToQueue(QMaster(), game_addr(ORIG_MasterTask));
			Memset32(&m->counter, 0, 0x36B);
			m->voice = ClaimVoiceSlot(SOUND_Voice(), 1, 0x80);
			const uint8_t *act = *(const uint8_t *const *)(Ctx() + 4);
			int16_t minx = 0x7FFF, miny = 0x7FFF, minz = 0x7FFF;
			int16_t maxx = (int16_t)0x8001, maxy = (int16_t)0x8001, maxz = (int16_t)0x8001;
			m->ntargets = act[0x10];
			uint32_t idx = 0;
			for (int32_t i = 0; i < m->ntargets; i++, idx += 0x18)
			{
				const uint8_t slot = (*(const uint8_t *const *)(*(const uint8_t *const *)(Ctx() + 4) + 8))[idx];
				uint8_t *e = Entity(slot);
				m->targets[i].entity = e;
				m->targets[i].pos_xy = *(const uint32_t *)(e + 0x1C);
				m->targets[i].pos_z = *(const uint32_t *)(e + 0x20);
				int16_t p[4];
				GetDefaultEffectPosition(e, p);
				if (p[0] < minx) minx = p[0];
				if (p[0] > maxx) maxx = p[0];
				if (p[1] < miny) miny = p[1];
				if (p[1] > maxy) maxy = p[1];
				if (p[2] < minz) minz = p[2];
				if (p[2] > maxz) maxz = p[2];
			}
			int32_t v = maxx + minx;
			Centre()[0] = (int16_t)((v - (v >> 31)) >> 1);
			m->centre[0] = Centre()[0];
			v = maxy + miny;
			Centre()[1] = (int16_t)((v - (v >> 31)) >> 1);
			m->centre[1] = Centre()[1];
			v = maxz + minz;
			Centre()[2] = (int16_t)((v - (v >> 31)) >> 1);
			m->centre[2] = Centre()[2];
			m->caster_xy = *(const uint32_t *)(Caster() + 0x1C);
			m->caster_z = *(const uint32_t *)(Caster() + 0x20);
			*(uint32_t *)CasterPos() = *(const uint32_t *)(Caster() + 0x1C);
			*(uint32_t *)(CasterPos() + 2) = *(const uint32_t *)(Caster() + 0x20);
			StreamStateInit((void *)(TexBase() + 0x27648));
			Star *st = (Star *)(TexBase() + 0xDB8);
			for (uint32_t g = game_addr(STARS); g < game_addr(game::unk_133AC48); g += 0x10)
			{
				*(int16_t *)(g + 2) = 0;
				*(int16_t *)g = 0;
				for (int k = 0x80; k != 0; k--, st++)
				{
					const int32_t pitch = sdiv_pow2(mul32(CrtRand(), 1600), 15) - 0x320;
					const int32_t yaw = (CrtRand() & 0xFFF) - 0x800;
					DirectionFromAngles(pitch, yaw, st->pos);
					st->dist = (int16_t)ISqrt(mul32(yaw, yaw) + mul32(pitch, pitch));
					const int32_t rr = CrtRand();
					const int32_t d = ((rr - (rr >> 31)) >> 1) + 0x2EE0;
					st->pos[0] = (int16_t)(mul32(st->pos[0], d) >> 12);
					st->pos[1] = (int16_t)(mul32(st->pos[1], d) >> 12);
					st->pos[2] = (int16_t)(mul32(st->pos[2], d) >> 12);
					*(uint32_t *)st->rgb = *(const uint32_t *)(g - 8);
					st->level = (uint8_t)CrtRand();
					*(uint32_t *)st->rgb2 = *(const uint32_t *)(g - 4);
					const uint8_t sp = st->speed;
					st->speed = (uint8_t)((mul32(CrtRand(), sp) >> 17) + sp);
				}
			}
		}
		int a = (int)n;
		if (r->started)
		{
			a = ExecuteTaskQueue(QMaster());
			ExecuteTaskQueue(QLayouts());
		}
		if (Pause() != 0) return 0;
		if (r->started && a == 0) return TASK_END;
		r->counter++;
		return 0;
	}
}

	void register_mag065_great_attractor()
	{
		register_port(ga065::ORIG_RootTask, (void *)ga065::RootTask, "A065 RootTask", 65);
		register_port(ga065::ORIG_MasterTask, (void *)ga065::MasterTask, "A065 MasterTask", 65);
		register_port(ga065::ORIG_LayoutTask, (void *)ga065::LayoutTask, "A065 LayoutTask", 65);
	}
}

