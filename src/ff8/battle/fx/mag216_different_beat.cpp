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

// Effect 216: Different Beat

#include "mag_common.h"

namespace ff8fx
{
namespace beat216
{
	using namespace eng;
	using namespace magc;

	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_216_DIFFERENTBEAT_PacketCursor)); }
	inline uint32_t &CaptureCursor() { return var<uint32_t>(game_addr(game::MAG_216_DIFFERENTBEAT_CaptureCursor)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::MAG_216_DIFFERENTBEAT_Ctx)); }
	inline uint8_t *&Caster() { return var<uint8_t *>(game_addr(game::MAG_216_DIFFERENTBEAT_Caster)); }
	inline uint8_t *&Target() { return var<uint8_t *>(game_addr(game::MAG_216_DIFFERENTBEAT_Target)); }
	inline TaskQueue *QSprites() { return (TaskQueue *)game_addr(game::MAG_216_DIFFERENTBEAT_QSprites); }
	inline TaskQueue *QRocks() { return (TaskQueue *)game_addr(game::MAG_216_DIFFERENTBEAT_QRocks); }
	inline TaskQueue *QRings() { return (TaskQueue *)game_addr(game::MAG_216_DIFFERENTBEAT_QRings); }
	inline TaskQueue *QDirector() { return (TaskQueue *)game_addr(game::MAG_216_DIFFERENTBEAT_QDirector); }
	inline TaskQueue *QRoot() { return (TaskQueue *)game_addr(game::MAG_216_DIFFERENTBEAT_QRoot); }
	inline int32_t &Flash() { return var<int32_t>(game_addr(game::MAG_216_DIFFERENTBEAT_FlashLevel)); }
	inline int32_t &FlashLast() { return var<int32_t>(game_addr(game::MAG_216_DIFFERENTBEAT_FlashLevelLast)); }
	inline const int16_t *ZeroWords() { return (const int16_t *)game_addr(game::MAG_216_DIFFERENTBEAT_ZeroWords); }
	inline uint32_t *CamAnchor() { return (uint32_t *)game_addr(game::MAG_216_DIFFERENTBEAT_CamAnchor); }
	inline uint32_t *Anchor() { return (uint32_t *)game_addr(game::MAG_216_DIFFERENTBEAT_Place); }
	inline int32_t &Height() { return var<int32_t>(game_addr(game::MAG_216_DIFFERENTBEAT_TargetHeight)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::MAG_216_DIFFERENTBEAT_MagicBuffer)); }
	inline Mat4x3 *View() { return (Mat4x3 *)game_addr(game::MAG_216_DIFFERENTBEAT_View); }
	inline uint32_t OTBase() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)); }
	inline uint32_t &FramePackets() { return var<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)); }
	inline int16_t *CamEye() { return (int16_t *)game_addr(game::Battle_Camera_world_XZ_s16); }
	inline int16_t *CamLookAt() { return (int16_t *)game_addr(game::Battle_Camera_LookAt_XZ_s16); }
	inline int16_t &ViewWord() { return var<int16_t>(game_addr(game::BATTLE_PROJECTION_DISTANCE)); }
	inline int16_t &CamRoll() { return var<int16_t>(game_addr(game::g_BattleCam_Roll)); }

	static constexpr game::Id ORIG_RootTask = game::MAG_216_DIFFERENTBEAT_RootTask;
	static constexpr game::Id ORIG_DirectorTask = game::MAG_216_DIFFERENTBEAT_DirectorTask;
	static constexpr game::Id ORIG_RingTask = game::MAG_216_DIFFERENTBEAT_RingTask;
	static constexpr game::Id ORIG_RockATask = game::MAG_216_DIFFERENTBEAT_RockATask;
	static constexpr game::Id ORIG_RockBTask = game::MAG_216_DIFFERENTBEAT_RockBTask;
	static constexpr game::Id ORIG_SparkTask = game::MAG_216_DIFFERENTBEAT_SparkTask;
	static constexpr game::Id ORIG_EmberTask = game::MAG_216_DIFFERENTBEAT_EmberTask;
	static constexpr game::Id ORIG_Callback = game::MAG_216_DIFFERENTBEAT_PartCallback;

	static constexpr game::Id COM_Anim = game::MAG_216_DIFFERENT_BEAT_AnimSet;
	static constexpr game::Id LAYOUT_Rise = game::unk_1140C5C, LAYOUT_Fly = game::unk_11431D4, LAYOUT_Drop = game::unk_11484D0, LAYOUT_Impact = game::unk_1141444, LAYOUT_Crater = game::unk_114A6DC, LAYOUT_Pillar = game::unk_11512C8, LAYOUT_Ring = game::unk_115049C;
	static constexpr game::Id KIND_Rise = game::a1, KIND_Fly = game::a100, KIND_Impact = game::a00_1, KIND_Crater = game::a090000, KIND_Ring = game::a000;
	static constexpr game::Id MODEL_Twist = game::unk_113C3CC, MODEL_Rock = game::unk_1140AA4;
	static constexpr game::Id SEQ_Spark = game::unk_113C1AC, SEQ_Ember = game::unk_113C2F0;
	static const void *SOUND_Start() { return (const void *)game_addr(game::unk_113C1A8); }
	static const uint32_t MORPH = 0x2CD8;

	inline int32_t ComputeCos(int32_t a) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(a); }
	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t flag, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, flag, out); }
	inline uint32_t BindExternalAnimSet(uint8_t *entity, void *out, uint32_t data) { return fn<uint32_t (__cdecl *)(uint8_t *, void *, uint32_t)>(game_addr(game::BattleEntity_BindExternalAnimSet))(entity, out, data); }
	inline uint32_t QueueChainTransformation(uint8_t *entity, uint32_t id) { return fn<uint32_t (__cdecl *)(uint8_t *, uint32_t)>(game_addr(game::QueueChainTransformation))(entity, id); }
	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline void RotationX(int32_t a, Mat4x3 *out) { fn<void (__cdecl *)(int32_t, Mat4x3 *)>(game_addr(game::MAG_063_RotationXMatrix))(a, out); }
	inline void RotationY(int32_t a, Mat4x3 *out) { fn<void (__cdecl *)(int32_t, Mat4x3 *)>(game_addr(game::Effect_RotationYMatrix))(a, out); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void RotationFromAnglesB(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::MAG_063_RotationFromAnglesYXZ))(angles, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void MatrixMultiply3x3(Mat4x3 *a, const Mat4x3 *b) { fn<void (__cdecl *)(Mat4x3 *, const Mat4x3 *)>(game_addr(game::MatrixMultiply3x3))(a, b); }
	inline void Apply3DTransform(const Mat4x3 *m, const int16_t *in, int32_t *out) { fn<void (__cdecl *)(const Mat4x3 *, const int16_t *, int32_t *)>(game_addr(game::apply3DTransform))(m, in, out); }
	inline uint32_t RenderPrimSet(void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::MAG_063_RenderPrimSet))(h, ot, mode, cursor); }
	inline uint32_t DrawShadow(uint8_t *e, uint32_t ot, int32_t a, uint32_t cursor) { return fn<uint32_t (__cdecl *)(uint8_t *, uint32_t, int32_t, uint32_t)>(game_addr(game::BattleEntity_DrawShadow))(e, ot, a, cursor); }
	inline uint32_t RenderGeometry(uint32_t model, void *ctx, uint32_t ot, int32_t a, uint32_t cursor) { return fn<uint32_t (__cdecl *)(uint32_t, void *, uint32_t, int32_t, uint32_t)>(game_addr(game::RenderGeometry))(model, ctx, ot, a, cursor); }
	inline void CamLerp(const int16_t *a, const int16_t *b, int32_t t) { fn<void (__cdecl *)(const int16_t *, const int16_t *, int32_t)>(game_addr(game::MAG_325_CameraLerp))(a, b, t); }
	inline void CameraArmReturn() { fn<void (__cdecl *)()>(game_addr(game::BS_Camera_ArmReturnAfterEffect))(); }
	inline void OrthonormalizeCamera(const Mat4x3 *in, Mat4x3 *out) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::MAG_069_OrthonormalizeCameraMatrix))(in, out); }
	inline void TransformVectorBy3x3(const Mat4x3 *m, const int32_t *in, int32_t *out) { fn<void (__cdecl *)(const Mat4x3 *, const int32_t *, int32_t *)>(game_addr(game::TransformVectorBy3x3Matrix))(m, in, out); }
	inline uint32_t SetDrawEnv(void *prim, uint32_t env) { return fn<uint32_t (__cdecl *)(void *, uint32_t)>(game_addr(game::SSIGPU_SetDrawEnv))(prim, env); }
	inline void SetDrawAreaPrim(void *prim, const int16_t *rect) { fn<void (__cdecl *)(void *, const int16_t *)>(game_addr(game::GPU_SetDrawAreaPrim))(prim, rect); }
	inline void SetDrawOffsetPrim(void *prim, const int16_t *xy) { fn<void (__cdecl *)(void *, const int16_t *)>(game_addr(game::SSIGPU_SetDrawOffsetPrim))(prim, xy); }
	inline void SetDrawStp(void *prim, int32_t on) { fn<void (__cdecl *)(void *, int32_t)>(game_addr(game::SetDrawStp))(prim, on); }
	inline void GetScreenOffset(int32_t *x, int32_t *y) { fn<void (__cdecl *)(int32_t *, int32_t *)>(game_addr(game::GTE_ReadScreenOffset))(x, y); }
	inline void SetScreenOffset(int32_t x, int32_t y) { fn<void (__cdecl *)(int32_t, int32_t)>(game_addr(game::Call_Bs_ParseCamera))(x, y); }
	inline void InsertPrimAltViewport(uint32_t bucket, void *packet) { fn<void (__cdecl *)(uint32_t, void *)>(game_addr(game::SSIGPU_InsertPrimAltViewport))(bucket, packet); }
	inline uint32_t FullScreenTile(int32_t r, int32_t g, int32_t b, int32_t level, uint32_t cursor) { return fn<uint32_t (__cdecl *)(int32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_FullScreenTile))(r, g, b, level, cursor); }
	inline void StreamStateInit(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::BattleFile_StreamState_Init))(dst); }
	inline void StreamQueueLoad(int32_t id, uint32_t dst, int32_t kind) { fn<void (__cdecl *)(int32_t, uint32_t, int32_t)>(game_addr(game::Stream_QueueLoad))(id, dst, kind); }
	inline void StreamPump() { fn<void (__cdecl *)()>(game_addr(game::Stream_Pump))(); }
	inline int32_t StreamPreload(int32_t a) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::BattleFile_PreLoad_IsLoaded))(a); }
	inline void GteMVMVA_RotV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0))(); }
	inline void GteMVMVA_RotIR() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotIR))(); }
	inline void GteLoadIRFromColumn(const void *p) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadIRFromMatrixColumn))(p); }
	inline void GteStoreIRToColumn(void *p) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreIRToMatrixColumn))(p); }
	inline void GteLoadV0FromDwords(const void *p) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadV0FromDwords))(p); }
	inline void GteLoadIR123FromBytes(const void *p) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadIR123FromBytes))(p); }
	inline void GteStoreIR123LowBytes(void *p) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreIR123LowBytes))(p); }
	inline void GteSetSXY012(uint32_t a, uint32_t b, uint32_t c) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_SetSXY012))(a, b, c); }
	inline void GteNCLIP() { fn<void (__cdecl *)()>(game_addr(game::GTE_NCLIP))(); }
	inline void GteReadMAC0(void *p) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadMAC0))(p); }
	inline void GteSetSZ123(uint32_t a, uint32_t b, uint32_t c) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_SetSZ123))(a, b, c); }
	inline void GteSetSZ0123(uint32_t a, uint32_t b, uint32_t c, uint32_t d) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_SetSZ0123))(a, b, c, d); }
	inline void GteAVSZ3() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ3))(); }
	inline void GteAVSZ4() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ4))(); }
	inline void GteReadOTZWord(void *p) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadOTZ))(p); }
	inline void GteReadOTZdiv4(void *p) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadOTZ_SZ3div4))(p); }
	inline void GteLoadV0u(const void *v) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadV0_u16))(v); }
	inline void GteReadIR2(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadIR2Word))(dst); }
	inline void GteLoadV012(const void *a, const void *b, const void *c) { fn<void (__cdecl *)(const void *, const void *, const void *)>(game_addr(game::GTE_LoadV012))(a, b, c); }
	inline void GteRTPT() { fn<void (__cdecl *)()>(game_addr(game::GTE_RTPT))(); }
	inline void GteReadFLAG(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadFLAG))(dst); }
	inline void GteReadSXY012(void *a, void *b, void *c) { fn<void (__cdecl *)(void *, void *, void *)>(game_addr(game::GTE_ReadSXY012_Split))(a, b, c); }
	inline void GteSetFarColor(uint32_t r, uint32_t g, uint32_t b) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_SetFarColor))(r, g, b); }
	inline void GteLoadRGBC(const void *c) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadRGBC))(c); }
	inline void GteLoadRGB012(const void *a, const void *b, const void *c) { fn<void (__cdecl *)(const void *, const void *, const void *)>(game_addr(game::GTE_LoadRGB012))(a, b, c); }
	inline void GteDPCS() { fn<void (__cdecl *)()>(game_addr(game::GTE_DPCS))(); }
	inline void GteDPCT() { fn<void (__cdecl *)()>(game_addr(game::GTE_DPCT))(); }
	inline void GteStoreRGB2(void *c) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreRGB2))(c); }
	inline void GteStoreRGB012(void *a, void *b, void *c) { fn<void (__cdecl *)(void *, void *, void *)>(game_addr(game::GTE_StoreRGB012))(a, b, c); }
	inline void GteSetRotMatrixRaw(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_SetRotMatrix))(m); }
	inline void GteSetTransVectorRaw(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_SetTransVector))(m); }

	inline int16_t &W16(void *p, int off) { return *(int16_t *)((uint8_t *)p + off); }
	inline uint16_t &U16(void *p, int off) { return *(uint16_t *)((uint8_t *)p + off); }
	inline int32_t &W32(void *p, int off) { return *(int32_t *)((uint8_t *)p + off); }
	inline uint32_t &U32(void *p, int off) { return *(uint32_t *)((uint8_t *)p + off); }
	inline uint8_t &B8(void *p, int off) { return *((uint8_t *)p + off); }
	inline int32_t sdiv_pow2(int32_t v, int n) { return (int32_t)((uint32_t)v + ((uint32_t)(v >> 31) & ((1u << n) - 1))) >> n; }

	enum : int { SITE_RISE = 1, SITE_FLY, SITE_DROP_A, SITE_DROP_B, SITE_IMPACT, SITE_CRATER, SITE_PILLAR };

#pragma pack(push, 1)
	struct RootNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t state;
		uint8_t parity;
	};
	struct DirectorNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		uint8_t bind[0x20];
		int16_t place[4];
		int16_t anchor[4];
		uint32_t saved_pos[2];
		uint32_t saved_rot[2];
		int16_t scale;
		int16_t lift;
		int16_t sway_y;
		int16_t pad56;
		uint8_t layout_a[0x484];
		uint8_t layout_b[0x160];
		uint8_t layout_c[0xA8];
		uint8_t layout_d[0x684];
		uint8_t layout_e[0xEC];
		uint8_t ghosts[0x988];
		int16_t height[2];
		int16_t angles[3];
		uint8_t pad17E6[2];
		uint32_t target_pos[2];
	};
	struct PlayArg
	{
		Mat4x3 m;
		const uint8_t *kind;
		int16_t depth;
		int16_t scroll;
		int16_t fade;
		int16_t pad2A;
		uint32_t morph;
	};
	struct RingNode
	{
		TaskNode hdr;
		int16_t pos[3];
		int16_t scale;
		int32_t angle;
		uint8_t layout[0xEC];
	};
	struct RockNode
	{
		TaskNode hdr;
		int16_t pos[3];
		int16_t vel[3];
		int8_t acc[3];
		uint8_t life;
		uint8_t rx, ry;
		int8_t spin[2];
		int16_t scale;
		int16_t pad22;
	};
	struct SpriteNode
	{
		TaskNode hdr;
		int16_t pos[3];
		int16_t size;
		int8_t vel[3];
		uint8_t frame;
		int8_t acc[3];
		uint8_t pad1B;
	};
	struct GhostCtx
	{
		uint32_t polys;
		uint32_t verts;
		int16_t ntri, nquad;
		int16_t clip[4];
		uint32_t colour;
		uint32_t visible;
		uint32_t v[8];
		int32_t mac0;
		uint32_t pad40;
		int32_t otz;
		uint8_t pad48[4];
		uint8_t *ghosts;
		uint32_t slot;
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x10 && sizeof(DirectorNode) == 0x17F0, "Different Beat nodes");
	static_assert(sizeof(PlayArg) == 0x30 && sizeof(RingNode) == 0x104 && sizeof(RockNode) == 0x24 && sizeof(SpriteNode) == 0x1C && sizeof(GhostCtx) == 0x54, "Different Beat blocks");
}
}

namespace ff8fx
{
namespace beat216
{
	using namespace eng;
	using namespace magc;

	static inline bool OutX(const uint8_t *xy) { const int16_t v = *(const int16_t *)xy; return v < 0 || v > 0xA00; }
	static inline bool OutY(const uint8_t *xy) { const int16_t v = *(const int16_t *)xy; return v < 0 || v > 0x6C0; }
	static inline uint32_t Bucket(uint8_t *h, uint32_t ot, int32_t shift)
	{
		GteReadOTZWord(h + 0x2C);
		W32(h, 0x2C) = (int32_t)((uint32_t)W32(h, 0x2C) + (uint32_t)W32(h, 0x18));
		if (W32(h, 0x2C) < 0) W32(h, 0x2C) = 0;
		return ot + (uint32_t)(W32(h, 0x2C) >> shift) * 4;
	}
	static inline void ModePrim(uint8_t *p, uint32_t word)
	{
		U32(p, 0) = 0x1000000;
		U32(p, 4) = word;
	}
	static inline void Code(uint8_t *h, uint8_t *pk, uint32_t code, uint8_t on, uint8_t off)
	{
		U32(pk, 4) = code;
		if (B8(h, 0x1C) & on) U32(pk, 4) = code | 0x2000000;
		if (B8(h, 0x1C) & off) U32(pk, 4) &= 0xFDFFFFFF;
	}
	static inline void Overlay(uint8_t *h, uint8_t *p, uint32_t tag, uint8_t code)
	{
		const uint8_t g = (uint8_t)(0x80 - (uint8_t)(W32(h, 0xC) >> 5));
		U32(p, 0) = tag;
		B8(p, 7) = code;
		B8(p, 6) = g;
		B8(p, 5) = g;
		B8(p, 4) = g;
	}
	static inline const uint8_t *Vert(const uint8_t *verts, const uint8_t *rec, int off) { return verts + (uint32_t)*(const uint16_t *)(rec + off) * 4; }
	static inline bool Clip3(const uint8_t *a, const uint8_t *b, const uint8_t *c)
	{
		uint32_t clip = OutX(a) ? 1 : 0;
		if (OutX(b)) clip |= 2;
		if (OutX(c)) clip |= 4;
		if (OutY(a + 2)) clip |= 0x10;
		if (OutY(b + 2)) clip |= 0x20;
		if (OutY(c + 2)) clip |= 0x40;
		return (clip & 7) == 7 || (clip & 0x70) == 0x70;
	}
	static inline uint32_t Clip3Bits(const uint8_t *a, const uint8_t *b, const uint8_t *c)
	{
		uint32_t clip = OutX(a) ? 1 : 0;
		if (OutX(b)) clip |= 2;
		if (OutX(c)) clip |= 4;
		if (OutY(a + 2)) clip |= 0x10;
		if (OutY(b + 2)) clip |= 0x20;
		if (OutY(c + 2)) clip |= 0x40;
		return clip;
	}
	struct ListHead { int32_t count; const uint8_t *rec; const uint8_t *verts; };
	static inline ListHead Head(uint8_t *h)
	{
		const uint8_t *list = *(const uint8_t **)(h + 0x20);
		ListHead l = { *(const int32_t *)list, list + 4, *(const uint8_t **)(h + 4) };
		*(const uint8_t **)(h + 0x20) = l.rec;
		return l;
	}

	static uint32_t FlatTris(uint8_t *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		ListHead l = Head(h);
		uint8_t *pk = (uint8_t *)cursor;
		if (l.count <= 0) return (uint32_t)pk;
		const uint8_t *rec = l.rec;
		for (int32_t k = l.count; k != 0; k--, rec += 0xC)
		{
			GteLoadV012(Vert(l.verts, rec, 4), Vert(l.verts, rec, 6), Vert(l.verts, rec, 8));
			GteRTPT();
			U32(pk, 0) = 0x4000000;
			Code(h, pk, *(const uint32_t *)rec, 1, 4);
			GteReadFLAG(h + 0x30);
			if (U32(h, 0x30) & 0x60000) continue;
			GteNCLIP();
			GteReadMAC0(h + 0x24);
			if (W32(h, 0x24) < 0 && !(B8(h, 0x1C) & 0x10)) continue;
			GteReadSXY012(pk + 8, pk + 0xC, pk + 0x10);
			GteAVSZ3();
			if (Clip3(pk + 8, pk + 0xC, pk + 0x10)) continue;
			const uint32_t bucket = Bucket(h, ot, shift);
			if (B8(h, 0x1C) & 0x40)
			{
				GteLoadRGBC(pk + 4);
				GteSetIR0(W32(h, 0xC));
				GteDPCS();
				GteStoreRGB2(pk + 4);
				B8(pk, 7) |= 2;
				InsertPrimAutoDepth(bucket, pk);
				uint8_t *m1 = pk + 0x14, *p2 = pk + 0x1C, *m2 = pk + 0x30;
				pk += 0x38;
				ModePrim(m1, 0xE1000220);
				InsertPrimAutoDepth(bucket, m1);
				GteReadSXY012(p2 + 8, p2 + 0xC, p2 + 0x10);
				Overlay(h, p2, 0x4000000, 0x22);
				InsertPrimAutoDepth(bucket, p2);
				ModePrim(m2, 0xE1000240);
				InsertPrimAutoDepth(bucket, m2);
			}
			else
			{
				InsertPrimAutoDepth(bucket, pk);
				pk += 0x14;
			}
		}
		*(const uint8_t **)(h + 0x20) = rec;
		return (uint32_t)pk;
	}

	static uint32_t FlatQuads(uint8_t *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		ListHead l = Head(h);
		uint8_t *pk = (uint8_t *)cursor;
		if (l.count <= 0) return (uint32_t)pk;
		const uint8_t *rec = l.rec;
		for (int32_t k = l.count; k != 0; k--, rec += 0xC)
		{
			GteLoadV012(Vert(l.verts, rec, 4), Vert(l.verts, rec, 6), Vert(l.verts, rec, 8));
			GteRTPT();
			U32(pk, 0) = 0x5000000;
			Code(h, pk, *(const uint32_t *)rec, 1, 4);
			GteReadFLAG(h + 0x30);
			if (U32(h, 0x30) & 0x60000) continue;
			GteNCLIP();
			GteReadMAC0(h + 0x24);
			if (W32(h, 0x24) < 0 && !(B8(h, 0x1C) & 0x10)) continue;
			GteReadSXY012(pk + 8, pk + 0xC, pk + 0x10);
			GteLoadV0(Vert(l.verts, rec, 0xA));
			GteRTPS();
			uint32_t clip = Clip3Bits(pk + 8, pk + 0xC, pk + 0x10);
			GteReadSXY2(pk + 0x14);
			GteAVSZ4();
			if (OutX(pk + 0x14)) clip |= 8;
			if (OutY(pk + 0x16)) clip |= 0x80;
			if ((clip & 0xF) == 0xF || (clip & 0xF0) == 0xF0) continue;
			const uint32_t bucket = Bucket(h, ot, shift);
			if (B8(h, 0x1C) & 0x40)
			{
				GteLoadRGBC(pk + 4);
				GteSetIR0(W32(h, 0xC));
				GteDPCS();
				GteStoreRGB2(pk + 4);
				U32(h, 0x34) = U32(pk, 0x10);
				B8(pk, 7) |= 2;
				InsertPrimAutoDepth(bucket, pk);
				uint8_t *m1 = pk + 0x18, *p2 = pk + 0x20, *m2 = pk + 0x38;
				pk += 0x40;
				ModePrim(m1, 0xE1000220);
				InsertPrimAutoDepth(bucket, m1);
				U32(p2, 0x10) = U32(h, 0x34);
				GteReadSXY012(p2 + 8, p2 + 0xC, p2 + 0x14);
				Overlay(h, p2, 0x5000000, 0x2A);
				InsertPrimAutoDepth(bucket, p2);
				ModePrim(m2, 0xE1000240);
				InsertPrimAutoDepth(bucket, m2);
			}
			else
			{
				InsertPrimAutoDepth(bucket, pk);
				pk += 0x18;
			}
		}
		*(const uint8_t **)(h + 0x20) = rec;
		return (uint32_t)pk;
	}

	static uint32_t TexTris(uint8_t *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		ListHead l = Head(h);
		uint8_t *pk = (uint8_t *)cursor;
		if (l.count <= 0) return (uint32_t)pk;
		const uint8_t *rec = l.rec;
		for (int32_t k = l.count; k != 0; k--, rec += 0x14)
		{
			GteLoadV012(Vert(l.verts, rec, 4), Vert(l.verts, rec, 6), Vert(l.verts, rec, 8));
			GteRTPT();
			U32(pk, 0) = 0x7000000;
			Code(h, pk, *(const uint32_t *)rec, 1, 4);
			U32(pk, 0xC) = *(const uint32_t *)(rec + 0xC);
			U32(pk, 0x14) = *(const uint32_t *)(rec + 0x10);
			U32(pk, 0x1C) = *(const uint32_t *)(rec + 8) >> 16;
			GteReadFLAG(h + 0x30);
			if (U32(h, 0x30) & 0x60000) continue;
			GteNCLIP();
			GteReadMAC0(h + 0x24);
			if (W32(h, 0x24) < 0 && !(B8(h, 0x1C) & 0x10)) continue;
			GteReadSXY012(pk + 8, pk + 0x10, pk + 0x18);
			GteAVSZ3();
			if (Clip3(pk + 8, pk + 0x10, pk + 0x18)) continue;
			const uint32_t bucket = Bucket(h, ot, shift);
			if (B8(h, 0x1C) & 0x40)
			{
				GteLoadRGBC(pk + 4);
				GteSetIR0(W32(h, 0xC));
				GteDPCS();
				GteStoreRGB2(pk + 4);
				B8(pk, 7) |= 2;
				InsertPrimAutoDepth(bucket, pk);
				uint8_t *p2 = pk + 0x20, *m2 = pk + 0x34;
				pk += 0x3C;
				GteReadSXY012(p2 + 8, p2 + 0xC, p2 + 0x10);
				Overlay(h, p2, 0x4000000, 0x22);
				InsertPrimAutoDepth(bucket, p2);
				ModePrim(m2, 0xE1000240);
				InsertPrimAutoDepth(bucket, m2);
			}
			else
			{
				InsertPrimAutoDepth(bucket, pk);
				pk += 0x20;
			}
		}
		*(const uint8_t **)(h + 0x20) = rec;
		return (uint32_t)pk;
	}

	static uint32_t TexQuads(uint8_t *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		ListHead l = Head(h);
		uint8_t *pk = (uint8_t *)cursor;
		if (l.count <= 0) return (uint32_t)pk;
		const uint8_t *rec = l.rec;
		for (int32_t k = l.count; k != 0; k--, rec += 0x18)
		{
			GteLoadV012(Vert(l.verts, rec, 4), Vert(l.verts, rec, 6), Vert(l.verts, rec, 8));
			GteRTPT();
			U32(pk, 0) = 0x9000000;
			Code(h, pk, *(const uint32_t *)rec, 1, 4);
			U32(pk, 0xC) = *(const uint32_t *)(rec + 0xC);
			U32(pk, 0x14) = *(const uint32_t *)(rec + 0x10);
			U32(pk, 0x1C) = *(const uint32_t *)(rec + 0x14);
			U32(pk, 0x24) = *(const uint32_t *)(rec + 0x14) >> 16;
			GteReadFLAG(h + 0x30);
			if (U32(h, 0x30) & 0x60000) continue;
			GteNCLIP();
			GteReadMAC0(h + 0x24);
			if (W32(h, 0x24) < 0 && !(B8(h, 0x1C) & 0x10)) continue;
			GteReadSXY012(pk + 8, pk + 0x10, pk + 0x18);
			GteLoadV0(Vert(l.verts, rec, 0xA));
			GteRTPS();
			uint32_t clip = Clip3Bits(pk + 8, pk + 0x10, pk + 0x18);
			GteReadSXY2(pk + 0x20);
			GteAVSZ4();
			if (OutX(pk + 0x20)) clip |= 8;
			if (OutY(pk + 0x22)) clip |= 0x80;
			if ((clip & 0xF) == 0xF || (clip & 0xF0) == 0xF0) continue;
			const uint32_t bucket = Bucket(h, ot, shift);
			if (B8(h, 0x1C) & 0x40)
			{
				GteLoadRGBC(pk + 4);
				GteSetIR0(W32(h, 0xC));
				GteDPCS();
				GteStoreRGB2(pk + 4);
				U32(h, 0x34) = U32(pk, 0x18);
				B8(pk, 7) |= 2;
				InsertPrimAutoDepth(bucket, pk);
				uint8_t *p2 = pk + 0x28, *m2 = pk + 0x40;
				pk += 0x48;
				U32(p2, 0x10) = U32(h, 0x34);
				GteReadSXY012(p2 + 8, p2 + 0xC, p2 + 0x14);
				Overlay(h, p2, 0x5000000, 0x2A);
				InsertPrimAutoDepth(bucket, p2);
				ModePrim(m2, 0xE1000240);
				InsertPrimAutoDepth(bucket, m2);
			}
			else
			{
				InsertPrimAutoDepth(bucket, pk);
				pk += 0x28;
			}
		}
		*(const uint8_t **)(h + 0x20) = rec;
		return (uint32_t)pk;
	}

	static uint32_t GouraudTris(uint8_t *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		ListHead l = Head(h);
		uint8_t *pk = (uint8_t *)cursor;
		if (l.count <= 0) return (uint32_t)pk;
		const uint8_t *rec = l.rec;
		for (int32_t k = l.count; k != 0; k--, rec += 0x14)
		{
			GteLoadV012(Vert(l.verts, rec, 4), Vert(l.verts, rec, 6), Vert(l.verts, rec, 8));
			GteRTPT();
			U32(pk, 0) = 0x6000000;
			Code(h, pk, *(const uint32_t *)rec, 2, 8);
			GteReadFLAG(h + 0x30);
			if (U32(h, 0x30) & 0x60000) continue;
			GteNCLIP();
			GteReadMAC0(h + 0x24);
			if (W32(h, 0x24) < 0 && !(B8(h, 0x1C) & 0x20)) continue;
			GteReadSXY012(pk + 8, pk + 0x10, pk + 0x18);
			GteAVSZ3();
			if (Clip3(pk + 8, pk + 0x10, pk + 0x18)) continue;
			const uint32_t bucket = Bucket(h, ot, shift);
			if (B8(h, 0x1C) & 0x80)
			{
				GteLoadRGB012(rec + 0xC, rec + 0x10, pk + 4);
				GteSetIR0(W32(h, 0xC));
				GteDPCT();
				GteStoreRGB012(pk + 0xC, pk + 0x14, pk + 4);
				B8(pk, 7) |= 2;
				InsertPrimAutoDepth(bucket, pk);
				uint8_t *m1 = pk + 0x1C, *p2 = pk + 0x24, *m2 = pk + 0x38;
				pk += 0x40;
				ModePrim(m1, 0xE1000220);
				InsertPrimAutoDepth(bucket, m1);
				GteReadSXY012(p2 + 8, p2 + 0xC, p2 + 0x10);
				Overlay(h, p2, 0x4000000, 0x22);
				InsertPrimAutoDepth(bucket, p2);
				ModePrim(m2, 0xE1000240);
				InsertPrimAutoDepth(bucket, m2);
			}
			else
			{
				U32(pk, 0xC) = *(const uint32_t *)(rec + 0xC);
				U32(pk, 0x14) = *(const uint32_t *)(rec + 0x10);
				InsertPrimAutoDepth(bucket, pk);
				pk += 0x1C;
			}
		}
		*(const uint8_t **)(h + 0x20) = rec;
		return (uint32_t)pk;
	}

	static uint32_t GouraudQuads(uint8_t *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		ListHead l = Head(h);
		uint8_t *pk = (uint8_t *)cursor;
		if (l.count <= 0) return (uint32_t)pk;
		const uint8_t *rec = l.rec;
		for (int32_t k = l.count; k != 0; k--, rec += 0x18)
		{
			GteLoadV012(Vert(l.verts, rec, 4), Vert(l.verts, rec, 6), Vert(l.verts, rec, 8));
			GteRTPT();
			U32(pk, 0) = 0x8000000;
			Code(h, pk, *(const uint32_t *)rec, 2, 8);
			GteReadFLAG(h + 0x30);
			if (U32(h, 0x30) & 0x60000) continue;
			GteNCLIP();
			GteReadMAC0(h + 0x24);
			if (W32(h, 0x24) < 0 && !(B8(h, 0x1C) & 0x20)) continue;
			GteReadSXY012(pk + 8, pk + 0x10, pk + 0x18);
			GteLoadV0(Vert(l.verts, rec, 0xA));
			GteRTPS();
			uint32_t clip = Clip3Bits(pk + 8, pk + 0x10, pk + 0x18);
			GteReadSXY2(pk + 0x20);
			GteAVSZ4();
			if (OutX(pk + 0x20)) clip |= 8;
			if (OutY(pk + 0x22)) clip |= 0x80;
			if ((clip & 0xF) == 0xF || (clip & 0xF0) == 0xF0) continue;
			const uint32_t bucket = Bucket(h, ot, shift);
			if (B8(h, 0x1C) & 0x80)
			{
				GteLoadRGB012(rec + 0xC, rec + 0x10, rec + 0x14);
				GteSetIR0(W32(h, 0xC));
				GteDPCT();
				GteStoreRGB012(pk + 0xC, pk + 0x14, pk + 0x1C);
				GteLoadRGBC(pk + 4);
				GteDPCS();
				GteStoreRGB2(pk + 4);
				U32(h, 0x34) = U32(pk, 0x18);
				B8(pk, 7) |= 2;
				InsertPrimAutoDepth(bucket, pk);
				uint8_t *m1 = pk + 0x24, *p2 = pk + 0x2C, *m2 = pk + 0x44;
				pk += 0x4C;
				ModePrim(m1, 0xE1000220);
				InsertPrimAutoDepth(bucket, m1);
				U32(p2, 0x10) = U32(h, 0x34);
				GteReadSXY012(p2 + 8, p2 + 0xC, p2 + 0x14);
				Overlay(h, p2, 0x5000000, 0x2A);
				InsertPrimAutoDepth(bucket, p2);
				ModePrim(m2, 0xE1000240);
				InsertPrimAutoDepth(bucket, m2);
			}
			else
			{
				U32(pk, 0xC) = *(const uint32_t *)(rec + 0xC);
				U32(pk, 0x14) = *(const uint32_t *)(rec + 0x10);
				U32(pk, 0x1C) = *(const uint32_t *)(rec + 0x14);
				InsertPrimAutoDepth(bucket, pk);
				pk += 0x24;
			}
		}
		*(const uint8_t **)(h + 0x20) = rec;
		return (uint32_t)pk;
	}

	static uint32_t GouraudTexTris(uint8_t *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		ListHead l = Head(h);
		uint8_t *pk = (uint8_t *)cursor;
		if (l.count <= 0) return (uint32_t)pk;
		const uint8_t *rec = l.rec;
		for (int32_t k = l.count; k != 0; k--, rec += 0x1C)
		{
			GteLoadV012(Vert(l.verts, rec, 4), Vert(l.verts, rec, 6), Vert(l.verts, rec, 8));
			GteRTPT();
			U32(pk, 0) = 0x9000000;
			Code(h, pk, *(const uint32_t *)rec, 2, 8);
			U32(pk, 0xC) = *(const uint32_t *)(rec + 0xC);
			U32(pk, 0x18) = *(const uint32_t *)(rec + 0x10);
			U32(pk, 0x24) = *(const uint32_t *)(rec + 8) >> 16;
			GteReadFLAG(h + 0x30);
			if (U32(h, 0x30) & 0x60000) continue;
			GteNCLIP();
			GteReadMAC0(h + 0x24);
			if (W32(h, 0x24) < 0 && !(B8(h, 0x1C) & 0x20)) continue;
			GteReadSXY012(pk + 8, pk + 0x14, pk + 0x20);
			GteAVSZ3();
			if (Clip3(pk + 8, pk + 0x14, pk + 0x20)) continue;
			const uint32_t bucket = Bucket(h, ot, shift);
			if (B8(h, 0x1C) & 0x80)
			{
				GteLoadRGB012(rec + 0x14, rec + 0x18, pk + 4);
				GteSetIR0(W32(h, 0xC));
				GteDPCT();
				GteStoreRGB012(pk + 0x10, pk + 0x1C, pk + 4);
				B8(pk, 7) |= 2;
				InsertPrimAutoDepth(bucket, pk);
				uint8_t *p2 = pk + 0x28, *m2 = pk + 0x3C;
				pk += 0x44;
				GteReadSXY012(p2 + 8, p2 + 0xC, p2 + 0x10);
				Overlay(h, p2, 0x4000000, 0x22);
				InsertPrimAutoDepth(bucket, p2);
				ModePrim(m2, 0xE1000240);
				InsertPrimAutoDepth(bucket, m2);
			}
			else
			{
				U32(pk, 0x10) = *(const uint32_t *)(rec + 0x14);
				U32(pk, 0x1C) = *(const uint32_t *)(rec + 0x18);
				InsertPrimAutoDepth(bucket, pk);
				pk += 0x28;
			}
		}
		*(const uint8_t **)(h + 0x20) = rec;
		return (uint32_t)pk;
	}

	static uint32_t GouraudTexQuads(uint8_t *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		ListHead l = Head(h);
		uint8_t *pk = (uint8_t *)cursor;
		if (l.count <= 0) return (uint32_t)pk;
		const uint8_t *rec = l.rec;
		for (int32_t k = l.count; k != 0; k--, rec += 0x24)
		{
			GteLoadV012(Vert(l.verts, rec, 4), Vert(l.verts, rec, 6), Vert(l.verts, rec, 8));
			GteRTPT();
			U32(pk, 0) = 0xC000000;
			Code(h, pk, *(const uint32_t *)rec, 2, 8);
			U32(pk, 0xC) = *(const uint32_t *)(rec + 0xC);
			U32(pk, 0x18) = *(const uint32_t *)(rec + 0x10);
			U32(pk, 0x24) = *(const uint32_t *)(rec + 0x14);
			U32(pk, 0x30) = *(const uint32_t *)(rec + 0x14) >> 16;
			GteReadFLAG(h + 0x30);
			if (U32(h, 0x30) & 0x60000) continue;
			GteNCLIP();
			GteReadMAC0(h + 0x24);
			if (W32(h, 0x24) < 0 && !(B8(h, 0x1C) & 0x20)) continue;
			GteReadSXY012(pk + 8, pk + 0x14, pk + 0x20);
			GteLoadV0(Vert(l.verts, rec, 0xA));
			GteRTPS();
			uint32_t clip = Clip3Bits(pk + 8, pk + 0x14, pk + 0x20);
			GteReadSXY2(pk + 0x2C);
			GteAVSZ4();
			if (OutX(pk + 0x2C)) clip |= 8;
			if (OutY(pk + 0x2E)) clip |= 0x80;
			if ((clip & 0xF) == 0xF || (clip & 0xF0) == 0xF0) continue;
			const uint32_t bucket = Bucket(h, ot, shift);
			if (B8(h, 0x1C) & 0x80)
			{
				GteLoadRGB012(rec + 0x18, rec + 0x1C, rec + 0x20);
				GteSetIR0(W32(h, 0xC));
				GteDPCT();
				GteStoreRGB012(pk + 0x10, pk + 0x1C, pk + 0x28);
				GteLoadRGBC(pk + 4);
				GteDPCS();
				GteStoreRGB2(pk + 4);
				U32(h, 0x34) = U32(pk, 0x20);
				B8(pk, 7) |= 2;
				InsertPrimAutoDepth(bucket, pk);
				uint8_t *p2 = pk + 0x34, *m2 = pk + 0x4C;
				pk += 0x54;
				U32(p2, 0x10) = U32(h, 0x34);
				GteReadSXY012(p2 + 8, p2 + 0xC, p2 + 0x14);
				Overlay(h, p2, 0x5000000, 0x2A);
				InsertPrimAutoDepth(bucket, p2);
				ModePrim(m2, 0xE1000240);
				InsertPrimAutoDepth(bucket, m2);
			}
			else
			{
				U32(pk, 0x10) = *(const uint32_t *)(rec + 0x18);
				U32(pk, 0x1C) = *(const uint32_t *)(rec + 0x1C);
				U32(pk, 0x28) = *(const uint32_t *)(rec + 0x20);
				InsertPrimAutoDepth(bucket, pk);
				pk += 0x34;
			}
		}
		*(const uint8_t **)(h + 0x20) = rec;
		return (uint32_t)pk;
	}

	static uint32_t RenderModel(uint8_t *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		if (!(U32(h, 0x1C) & 0x2000)) U32(h, 4) = U32(h, 0) + 8;
		const uint8_t *model = *(const uint8_t **)h;
		*(const uint8_t **)(h + 0x20) = model + *(const uint32_t *)model;
		GteSetFarColor(B8(h, 8), B8(h, 9), B8(h, 0xA));
		uint32_t c = cursor;
		for (int i = 0; i < 8; i++)
		{
			const uint8_t *list = *(const uint8_t **)(h + 0x20);
			if (*(const uint32_t *)list == 0)
			{
				*(const uint8_t **)(h + 0x20) = list + 4;
				continue;
			}
			switch (i)
			{
			case 0: c = FlatTris(h, ot, shift, c); break;
			case 1: c = FlatQuads(h, ot, shift, c); break;
			case 2: c = TexTris(h, ot, shift, c); break;
			case 3: c = TexQuads(h, ot, shift, c); break;
			case 4: c = GouraudTris(h, ot, shift, c); break;
			case 5: c = GouraudQuads(h, ot, shift, c); break;
			case 6: c = GouraudTexTris(h, ot, shift, c); break;
			default: c = GouraudTexQuads(h, ot, shift, c); break;
			}
		}
		return c;
	}

	inline uint8_t *Targets() { return Ctx()->actions[0].targets; }

	static int16_t g_off_pad = 0;

	static void __cdecl Callback(prim::Layout *l, prim::Record *r, int arg_)
	{
		const PlayArg *arg = (const PlayArg *)arg_;
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
		Mat4x3 m;
		if (r->flags & 0x40000) RotationFromAnglesB(r->rot, &m);
		else RotationFromAngles(r->rot, &m);
		const int16_t off[4] = { r->pos[0], r->pos[1], r->pos[2], g_off_pad };
		if (r->flags & 0x1000)
		{
			GteSetRotMatrixRaw(View());
			GteLoadV0(off);
			GteMVMVA_RotV0();
			int32_t t[3];
			GteReadMAC123(t);
			m.t[0] = t[0];
			m.t[1] = t[1];
			m.t[2] = t[2];
			if (!(r->flags & 0x8000)) MatrixMultiply(View(), &m);
		}
		else if (r->flags & 0x200)
		{
			m.t[0] = off[0];
			m.t[1] = off[1];
			m.t[2] = off[2];
		}
		else
		{
			GteSetRotMatrixRaw(&arg->m);
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
		U32(h, 0x1C) = (r->flags & 0x4000) ? 0x2000 : 0x2030;
		if (r->flags & 0x2000) U32(h, 0x1C) |= 0xC;
		W32(h, 0xC) = r->a;
		if (r->a != 0)
		{
			U32(h, 0x1C) |= 0xC0;
			U32(h, 8) = *(const uint32_t *)r->rgb;
		}
		W32(h, 0x18) = arg->depth;
		U16(h, 0x22) = 0;
		U16(h, 0x20) = 0;
		U16(h, 0x26) = 0;
		U16(h, 0x24) = 0;
		U16(h, 0x2A) = 0x100;
		U16(h, 0x28) = 0x100;
		switch ((uint32_t)arg->kind[object] - 0x31)
		{
		case 0:
			U16(h, 0x2C) = 0;
			U16(h, 0x2E) = 0;
			U16(h, 0x32) = 0x80;
			U16(h, 0x30) = 0x100;
			U16(h, 0x22) = (uint16_t)(*(const uint8_t *)&arg->scroll & 0x7F);
			break;
		case 1:
			U16(h, 0x2C) = 0;
			U16(h, 0x2E) = 0;
			U16(h, 0x32) = 0x80;
			U16(h, 0x30) = 0x100;
			U16(h, 0x22) = (uint16_t)((uint8_t)-*(const uint8_t *)&arg->scroll & 0x7F);
			break;
		case 2:
			U16(h, 0x2C) = 0;
			U16(h, 0x2E) = 0x80;
			U16(h, 0x32) = 0x80;
			U16(h, 0x30) = 0x100;
			U16(h, 0x22) = (uint16_t)(*(const uint8_t *)&arg->scroll & 0x7F);
			break;
		case 8:
			if (W32(h, 0xC) != 0) B8(h, 0x1C) &= 0xF3;
			W32(h, 0x18) += 0x100;
			PacketCursor() = RenderModel(h, OTBase() + 0x44, 2, PacketCursor());
			FieldFree(0x6C);
			return;
		}
		if (arg->fade != 0)
		{
			U32(h, 8) = 0;
			W32(h, 0xC) = arg->fade;
			U32(h, 0x1C) |= 0xC0;
		}
		PacketCursor() = RenderPrimSet(h, OTBase() + 0x44, 2, PacketCursor());
		FieldFree(0x6C);
	}

	static void GhostInit(uint8_t *g, int32_t n, const int32_t *bones)
	{
		g[3] = 0;
		g[2] = 0;
		g[1] = 0;
		g[0] = 0;
		g[4] = (uint8_t)n;
		ff8fx::fill<(0x80)>(g + 5, 0xFF);
		for (int32_t i = 0; i < n; i++) g[5 + bones[i]] = (uint8_t)i;
	}

	static void GhostRecord(uint8_t *entity, uint8_t *g)
	{
		g[0] = 1;
		const uint8_t s = g[2];
		g[3] = s;
		uint8_t *dst = g + 0x88 + (uint32_t)s * 0x480;
		g[2] = (uint8_t)(s + 1);
		if ((uint8_t)(s + 1) >= 2) g[2] = 0;
		g[1] = (uint8_t)(g[1] + 1);
		if (g[1] > 2) g[1] = 2;
		uint8_t *skel = **(uint8_t ***)(entity + 0x64);
		for (uint32_t b = 0; b < skel[0]; b++)
		{
			const int32_t k = (int8_t)g[5 + b];
			if (k >= 0) ff8fx::copy<(0x20)>(dst + k * 0x20, skel + 0x20 + b * 0x30);
		}
	}

	static void GhostVertices(uint8_t **cursor, uint8_t **out, GhostCtx *c)
	{
		uint8_t *src = *cursor;
		int32_t count = *(const int16_t *)src;
		uint8_t *o = *out;
		src += 2;
		for (; count != 0; count--)
		{
			GteLoadV0u(src);
			GteRTPS();
			GteReadSXY2(&c->v[0]);
			GteReadOTZdiv4(&c->v[1]);
			src += 6;
			if (U16(c, 0x20) == 0)
			{
				U16(o, 6) = 0x10;
			}
			else
			{
				U16(c, 0x22) = 0;
				const int32_t x = W16(c, 0x1C);
				if (x < (int32_t)c->clip[0] * 8) U16(c, 0x22) = 1;
				else if (x >= (int32_t)c->clip[2] * 8) U16(c, 0x22) = 2;
				const int32_t y = W16(c, 0x1E);
				if (y < (int32_t)c->clip[1] * 8) B8(c, 0x22) |= 4;
				else if (y >= (int32_t)c->clip[3] * 8) B8(c, 0x22) |= 8;
				U32(o, 0) = c->v[0];
				U32(o, 4) = c->v[1];
			}
			o += 8;
		}
		*cursor = src;
		*out = o;
	}

	static uint32_t GhostPolys(GhostCtx *c, uint32_t ot, int32_t shift, uint32_t pk)
	{
		uint8_t *vb = (uint8_t *)c->verts;
		uint8_t *p = (uint8_t *)c->polys;
		int16_t n = c->ntri;
		c->ntri = (int16_t)(n - 1);
		if (n != 0)
		{
			do
			{
				c->v[0] = U32(vb, (U16(p, 0) & 0xFFF) * 8);
				c->v[1] = U32(vb, (U16(p, 0) & 0xFFF) * 8 + 4);
				if (U16(c, 0x22) < 0x10)
				{
					c->v[2] = U32(vb, (U16(p, 2) & 0xFFF) * 8);
					c->v[3] = U32(vb, (U16(p, 2) & 0xFFF) * 8 + 4);
					if (U16(c, 0x2A) < 0x10)
					{
						c->v[4] = U32(vb, (U16(p, 4) & 0xFFF) * 8);
						c->v[5] = U32(vb, (U16(p, 4) & 0xFFF) * 8 + 4);
						if (U16(c, 0x32) < 0x10 && (B8(c, 0x22) & (uint8_t)(B8(c, 0x2A) & B8(c, 0x32))) == 0)
						{
							GteSetSXY012(c->v[0], c->v[2], c->v[4]);
							GteNCLIP();
							GteReadMAC0(&c->mac0);
							if (c->mac0 >= 0)
							{
								GteSetSZ123(U16(c, 0x20), U16(c, 0x28), U16(c, 0x30));
								GteAVSZ3();
								U32((void *)pk, 0) = 0x7000000;
								U32((void *)pk, 4) = c->colour | 0x26000000;
								U32((void *)pk, 8) = c->v[0];
								U32((void *)pk, 0x10) = c->v[2];
								U32((void *)pk, 0x18) = c->v[4];
								U32((void *)pk, 0xC) = U32(p, 8);
								U32((void *)pk, 0x14) = U32(p, 0xC);
								U16((void *)pk, 0x16) &= 0xFDFF;
								U16((void *)pk, 0x1C) = U16(p, 6);
								GteReadOTZWord(&c->otz);
								InsertPrimAutoDepth(ot + (uint32_t)(c->otz >> shift) * 4, (void *)pk);
								pk += 0x20;
							}
						}
					}
				}
				n = c->ntri;
				p += 0x10;
				c->ntri = (int16_t)(n - 1);
			} while (n != 0);
		}
		n = c->nquad;
		c->nquad = (int16_t)(n - 1);
		if (n != 0)
		{
			p += 4;
			do
			{
				c->v[0] = U32(vb, (U16(p, -4) & 0xFFF) * 8);
				c->v[1] = U32(vb, (U16(p, -4) & 0xFFF) * 8 + 4);
				if (U16(c, 0x22) < 0x10)
				{
					c->v[2] = U32(vb, (U16(p, -2) & 0xFFF) * 8);
					c->v[3] = U32(vb, (U16(p, -2) & 0xFFF) * 8 + 4);
					if (U16(c, 0x2A) < 0x10)
					{
						c->v[4] = U32(vb, (U16(p, 0) & 0xFFF) * 8);
						c->v[5] = U32(vb, (U16(p, 0) & 0xFFF) * 8 + 4);
						if (U16(c, 0x32) < 0x10)
						{
							c->v[6] = U32(vb, (U16(p, 2) & 0xFFF) * 8);
							c->v[7] = U32(vb, (U16(p, 2) & 0xFFF) * 8 + 4);
							if (U16(c, 0x3A) < 0x10 && (B8(c, 0x3A) & (uint8_t)(B8(c, 0x22) & (uint8_t)(B8(c, 0x2A) & B8(c, 0x32)))) == 0)
							{
								GteSetSXY012(c->v[0], c->v[2], c->v[4]);
								GteNCLIP();
								GteReadMAC0(&c->mac0);
								if (c->mac0 >= 0)
								{
									GteSetSZ0123(U16(c, 0x20), U16(c, 0x28), U16(c, 0x30), U16(c, 0x38));
									GteAVSZ4();
									U32((void *)pk, 0) = 0x9000000;
									U32((void *)pk, 4) = c->colour | 0x2E000000;
									U32((void *)pk, 8) = c->v[0];
									U32((void *)pk, 0x10) = c->v[2];
									U32((void *)pk, 0x18) = c->v[4];
									U32((void *)pk, 0x20) = c->v[6];
									U32((void *)pk, 0xC) = U32(p, 4);
									U32((void *)pk, 0x14) = U32(p, 8);
									U16((void *)pk, 0x1C) = U16(p, 0xC);
									U16((void *)pk, 0x16) &= 0xFDFF;
									U16((void *)pk, 0x24) = U16(p, 0xE);
									GteReadOTZWord(&c->otz);
									InsertPrimAutoDepth(ot + (uint32_t)(c->otz >> shift) * 4, (void *)pk);
									pk += 0x28;
								}
							}
						}
					}
				}
				n = c->nquad;
				p += 0x14;
				c->nquad = (int16_t)(n - 1);
			} while (n != 0);
		}
		return pk;
	}

	static void GhostModel(uint8_t *model, GhostCtx *c, uint32_t ot, int32_t shift)
	{
		const uint8_t *geom = *(uint8_t **)(model + 4);
		const int32_t nobj = *(const int32_t *)geom;
		const uint32_t *offs = (const uint32_t *)(geom + 4);
		for (int32_t i = 0; i < nobj; i++)
		{
			uint8_t *obj = (uint8_t *)(offs[i] + *(uint32_t *)(model + 4));
			if (!(c->visible & (1u << i))) continue;
			uint8_t *out = (uint8_t *)c->verts;
			uint8_t *cur = obj + 2;
			int32_t groups = *(const int16_t *)obj;
			for (; groups > 0; groups--)
			{
				const int32_t bone = *(const int16_t *)cur;
				cur += 2;
				const int32_t k = (int8_t)c->ghosts[bone + 5];
				if (k >= 0)
				{
					const Mat4x3 *m = (const Mat4x3 *)(c->slot + (uint32_t)(k + 0x12) * 0x20);
					GteSetRotMatrixRaw(m);
					GteSetTransVectorRaw(m);
					GhostVertices(&cur, &out, c);
				}
				else
				{
					const int32_t nv = *(const int16_t *)cur;
					cur += nv * 6 + 2;
					for (int32_t v = nv; v != 0; v--)
					{
						U16(out, 6) = 0x10;
						out += 8;
					}
				}
			}
			cur = (uint8_t *)(((uint32_t)cur + 3) & ~3u);
			c->ntri = *(const int16_t *)cur;
			cur += 2;
			c->nquad = *(const int16_t *)cur;
			cur += 0xA;
			c->polys = (uint32_t)cur;
			PacketCursor() = GhostPolys(c, ot, shift, PacketCursor());
		}
	}

	static void GhostDraw(uint8_t *entity, uint8_t *g)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x74);
		GhostCtx *c = (GhostCtx *)(h + 0x20);
		c->verts = var<uint32_t>(game_addr(game::g_BattleVertexWorkspace));
		c->clip[0] = 0;
		c->clip[1] = 0;
		c->clip[2] = 0x140;
		c->clip[3] = 0xD8;
		c->visible = *(const uint32_t *)(entity + 0x7C);
		ComposeAffineTransform(&Camera(), (const Mat4x3 *)(entity + 0x40), (Mat4x3 *)h);
		c->ghosts = g;
		int32_t w = 0x800;
		for (int32_t i = 0; i < g[1]; i++)
		{
			GteSetIR0((w >> 1) + w);
			GteLoadIR123FromBytes(entity + 0x28);
			GteGPF();
			GteStoreIR123LowBytes(&c->colour);
			B8(c, 0x17) = 0;
			c->slot = (uint32_t)g + 0x88 + (((uint32_t)g[3] - (uint32_t)i) & 1) * 0x480;
			for (int32_t b = 0; b < g[4]; b++)
			{
				const uint32_t r = c->slot + (uint32_t)b * 0x20;
				GteSetRotMatrixRaw(h);
				GteLoadIRFromColumn((const void *)(r + 0));
				GteMVMVA_RotIR();
				GteStoreIRToColumn((void *)(r + 0x240));
				GteLoadIRFromColumn((const void *)(r + 2));
				GteMVMVA_RotIR();
				GteStoreIRToColumn((void *)(r + 0x242));
				GteLoadIRFromColumn((const void *)(r + 4));
				GteMVMVA_RotIR();
				GteStoreIRToColumn((void *)(r + 0x244));
				GteSetTransVectorRaw(h);
				GteLoadV0FromDwords((const void *)(r + 0x14));
				GteMVMVA_RotV0Tr();
				GteReadMAC123((int32_t *)(r + 0x254));
			}
			GhostModel(*(uint8_t **)(entity + 0x64), c, OTBase() + 0x44, 0);
			w -= 0x400;
		}
		FieldFree(0x74);
		if (g[0] == 0 && g[1] != 0) g[1]--;
		g[0] = 0;
	}

	static void ModelBox(uint32_t model, int16_t *out)
	{
		const uint8_t *m = (const uint8_t *)model + 4;
		int32_t n = *(const int32_t *)m;
		const int16_t *v = (const int16_t *)(m + 8);
		out[3] = out[0] = v[0];
		out[4] = out[1] = v[1];
		out[5] = out[2] = v[2];
		n--;
		do
		{
			v += 4;
			if (out[0] > v[0]) out[0] = v[0];
			else if (out[3] < v[0]) out[3] = v[0];
			if (out[1] > v[1]) out[1] = v[1];
			else if (out[4] < v[1]) out[4] = v[1];
			if (out[2] > v[2]) out[2] = v[2];
			else if (out[5] < v[2]) out[5] = v[2];
		} while (--n != 0);
	}

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

	static void TwistVertices(uint32_t model, uint8_t *out, int32_t scale, int32_t angle)
	{
		const int16_t *v = (const int16_t *)(model + 8);
		int32_t n = *(const int32_t *)(model + 4);
		do
		{
			const int32_t a = (v[1] >> 2) + angle;
			const int32_t s = ComputeSin(a);
			const int32_t c = ComputeCos(a);
			const int32_t y = mul32(v[1], scale) >> 12;
			W16(out, 0) = (int16_t)((mul32(v[0], c) - mul32(y, s)) >> 12);
			W16(out, 2) = (int16_t)((mul32(v[0], s) + mul32(y, c)) >> 12);
			W16(out, 4) = v[2];
			out += 8;
			v += 4;
		} while (--n != 0);
	}

	static uint32_t FadeTile(int32_t f, int32_t r, int32_t g, int32_t b, int32_t in, int32_t hold, int32_t out, uint32_t cursor)
	{
		const int32_t din = in ? in : 1;
		const int32_t dout = out ? out : 1;
		if (f < in) return FullScreenTile(r, g, b, shl32(f + 1, 12) / din, cursor);
		if (f < in + hold) return FullScreenTile(r, g, b, 0x1000, cursor);
		return FullScreenTile(r, g, b, (shl32(in - f + hold, 12) - 0x1000) / dout + 0x1000, cursor);
	}

	static int32_t MeasureHeight(uint8_t *e, int16_t *out)
	{
		W32(e, 0x58) = W16(e, 0x1E);
		W32(e, 0x54) = W16(e, 0x1C);
		W32(e, 0x5C) = W16(e, 0x20);
		ComputeBonesWorldMatrices(e + 0x60, e + 0x40);
		uint8_t *h = (uint8_t *)FieldAlloc(0x18);
		U32(h, 4) = 0xFFFF0000;
		U32(h, 0) = 0x10000;
		U32(h, 0x14) = U32(e, 0x7C);
		U32(h, 8) = 0;
		U16(h, 0xC) = 0x1000;
		uint8_t *bones = **(uint8_t ***)(e + 0x64) + 0x10;
		uint8_t *model = *(uint8_t **)(e + 0x64);
		const uint8_t *geom = *(uint8_t **)(model + 4);
		const int32_t nobj = *(const int32_t *)geom;
		for (int32_t i = 0; i < nobj; i++)
		{
			const uint8_t *src = *(uint8_t **)(model + 4) + ((const uint32_t *)(geom + 4))[i];
			if (!(U32(h, 0x14) & (1u << i))) continue;
			int32_t groups = *(const int16_t *)src;
			src += 2;
			for (; groups > 0; groups--)
			{
				const int32_t bone = *(const int16_t *)src;
				src += 2;
				const Mat4x3 *m = (const Mat4x3 *)(bones + 0x10 + bone * 0x30);
				GteSetRotMatrixRaw(m);
				GteSetTransVectorRaw(m);
				int32_t nv = *(const int16_t *)src;
				src += 2;
				for (; nv != 0; nv--)
				{
					GteLoadV0u(src);
					GteRTPS();
					GteReadIR2(h + 0x10);
					src += 6;
					const int32_t y = W16(h, 0x10);
					if (W32(h, 0) > y) W32(h, 0) = y;
					if (W32(h, 4) < y) W32(h, 4) = y;
				}
			}
		}
		out[0] = W16(h, 4);
		out[1] = W16(h, 0);
		FieldFree(0x18);
		BuildBoneMatricesFromPose(e + 0x60);
		return out[1];
	}

	static void DrawEntity(uint8_t *e, const int16_t *angles, const int16_t *off, const int32_t *scale)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x5C);
		Mat4x3 *m = (Mat4x3 *)h;
		FramePackets() = DrawShadow(e, OTBase() + 0x4040, 0x10, FramePackets());
		ComposeZYXRotationMatrix(angles, m);
		m->t[0] = off[0];
		m->t[1] = off[1];
		m->t[2] = off[2];
		ComposeAffineTransform((const Mat4x3 *)(e + 0x40), m, m);
		Scale3DMatrix(m, scale);
		ComposeAffineTransform(&Camera(), m, m);
		Apply3DTransform(m, off, (int32_t *)(h + 0x20));
		m->t[1] = (int32_t)((uint32_t)m->t[1] - U32(h, 0x24));
		m->t[0] = (int32_t)((uint32_t)m->t[0] - U32(h, 0x20));
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

	template<typename F> static void ForChain(F f)
	{
		uint8_t *e = Target();
		do
		{
			f(e);
			e = *(uint8_t **)(e + 0x8C);
		} while (e != Target() && e != nullptr);
	}

	static void CaptureStage(const int16_t *pos, const int16_t *b)
	{
		const int32_t b0 = -(int32_t)b[0];
		const int32_t b3 = b[3];
		const int32_t w = b3 + b0;
		const int32_t nb2 = -(int32_t)b[2];
		const int32_t b5 = b[5];
		const int32_t d = b5 + nb2;
		SetScreenFlash(0, 0);
		int16_t rect[4] = { 0x280, 0x180, 0x80, 0x80 };
		int32_t sx = sdiv_pow2(w, 7);
		if (sx % 0x80 != 0) sx++;
		int32_t sz = sdiv_pow2(d, 7);
		if (sz % 0x80 != 0) sz++;
		Mat4x3 m, m2;
		m.t[1] = 0;
		m.t[0] = -((int32_t)pos[0] / sx);
		m.t[2] = (int32_t)pos[2] / sz;
		RotationY(0, &m);
		TransformVectorBy3x3(&m, m.t, m.t);
		RotationX(-0x400, &m2);
		MatrixMultiply3x3(&m2, &m);
		int32_t s3[3];
		s3[1] = 0;
		m2.t[0] = (b0 - b3) / 2 / sx + m.t[0];
		m2.t[2] = (int32_t)var<int16_t>(game_addr(game::BATTLE_PROJECTION_DISTANCE));
		m2.t[1] = m.t[2] - ((nb2 - b5) / 2 / sz);
		s3[0] = 0x1000000 / (shl32(w, 12) / rect[2]);
		s3[2] = 0x1000000 / (shl32(d, 12) / rect[3]);
		Scale3DMatrix(&m2, s3);
		uint8_t *env = (uint8_t *)PacketCursor();
		PacketCursor() += 0x40;
		SetDrawEnv(env, game_addr(game::g_Battle_DrawEnv) + (uint32_t)(var<uint8_t>(game_addr(game::g_Battle_DisplayParity)) ^ 1) * 0x5C);
		InsertPrimAutoDepth(OTBase() + 0x44, env);
		int32_t ox, oy;
		GetScreenOffset(&ox, &oy);
		SetScreenOffset(rect[2] / 2, rect[3] / 2);
		GteSetRotMatrix(&m2);
		GteSetTransVector(&m2);
		if (var<uint8_t>(game_addr(game::unk_1D989BD)) & 1)
		{
			uint8_t *h = (uint8_t *)FieldAlloc(0x2C);
			U16(h, 0x14) = 0;
			U16(h, 0x16) = 0;
			U16(h, 0x18) = 0x140;
			U16(h, 0x1A) = 0xD8;
			U16(h, 0x24) = var<uint16_t>(game_addr(game::unk_1D989BE));
			U32(h, 0x1C) = var<uint32_t>(game_addr(game::unk_1D989E4));
			U32(h, 4) = var<uint32_t>(game_addr(game::g_BattleVertexWorkspace));
			U32(h, 0x20) = 0xFFFFFFFF;
			U32(h, 0x28) = 0;
			U32(h, 0x10) = CaptureCursor();
			BuildBoneMatricesFromPose((void *)game_addr(game::unk_1D989D0));
			ComputeBonesWorldMatrices((void *)game_addr(game::unk_1D989D0), &m2);
			PacketCursor() = RenderGeometry(game_addr(game::unk_1D989C0), h, OTBase() + 0x44, 0x10, PacketCursor());
			FieldFree(0x2C);
		}
		uint8_t *q = (uint8_t *)PacketCursor();
		PacketCursor() += 0x18;
		U16(q, 8) = 0;
		U16(q, 0xA) = 0;
		U16(q, 0xC) = 0x80;
		U16(q, 0xE) = 0;
		U16(q, 0x10) = 0;
		U16(q, 0x12) = 0x80;
		U16(q, 0x14) = 0x80;
		U16(q, 0x16) = 0x80;
		q[4] = 0;
		q[5] = 0;
		q[6] = 0;
		U32(q, 0) = 0x05000000;
		q[7] = 0x28;
		InsertPrimAltViewport(OTBase() + 0x44, q);
		uint8_t *stp = (uint8_t *)PacketCursor();
		PacketCursor() += 0xC;
		SetDrawStp(stp, 1);
		InsertPrimAutoDepth(OTBase() + 0x44, stp);
		SetScreenOffset(ox, oy);
		uint8_t *area = (uint8_t *)PacketCursor();
		PacketCursor() += 0xC;
		SetDrawAreaPrim(area, rect);
		InsertPrimAutoDepth(OTBase() + 0x44, area);
		int16_t xy[2] = { rect[0], rect[1] };
		uint8_t *offp = (uint8_t *)PacketCursor();
		PacketCursor() += 0xC;
		SetDrawOffsetPrim(offp, xy);
		InsertPrimAutoDepth(OTBase() + 0x44, offp);
	}

	static uint32_t __cdecl RingTask(TaskNode *n)
	{
		RingNode *k = (RingNode *)n;
		PlayArg arg;
		ScaledRotY(k->angle, k->scale, &arg.m);
		arg.m.t[0] = k->pos[0];
		arg.m.t[1] = k->pos[1];
		arg.m.t[2] = k->pos[2];
		ComposeAffineTransform(&Camera(), &arg.m, &arg.m);
		arg.morph = TexBase() + MORPH;
		arg.depth = -0x80;
		arg.fade = 0;
		arg.kind = (const uint8_t *)game_addr(KIND_Ring);
		arg.scroll = 0;
		arg.pad2A = 0;
		return prim::play((prim::Layout *)k->layout, Callback, (int)&arg, 0) != 0 ? 0 : 2;
	}

	static void SpawnRing(int32_t x, int32_t y, int32_t z, int32_t scale)
	{
		RingNode *k = (RingNode *)AddTaskToQueue(QRings(), game_addr(ORIG_RingTask));
		if (!k) return;
		k->scale = (int16_t)scale;
		k->pos[0] = (int16_t)(W16(Caster(), 0x1C) + x);
		k->pos[1] = (int16_t)(W16(Caster(), 0x1E) + y);
		k->angle = 0;
		k->pos[2] = (int16_t)(W16(Caster(), 0x20) + z);
		DecodeModelPrimLayout(game_addr(LAYOUT_Ring), k->layout, 0xEC);
	}

	static void RockDrawAt(const int16_t *pos, int32_t ax, int32_t ay, int16_t scale)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x98);
		Mat4x3 *mb = (Mat4x3 *)(h + 0x78), *ma = (Mat4x3 *)(h + 0x58);
		ScaledRotY(ay, scale, mb);
		RotationX(ax, ma);
		MatrixMultiply3x3(ma, mb);
		ma->t[0] = pos[0];
		ma->t[1] = pos[1];
		ma->t[2] = pos[2];
		ComposeAffineTransform(&Camera(), ma, ma);
		GteSetRotMatrix(ma);
		GteSetTransVector(ma);
		U32(h, 0x1C) = 0;
		U32(h, 0) = game_addr(MODEL_Rock);
		PacketCursor() = RenderPrimModel(h, OTBase() + 0x44, 2, PacketCursor());
		FieldFree(0x98);
	}
	static void RockDraw(const RockNode *k) { RockDrawAt(k->pos, (int32_t)(int8_t)k->rx << 4, (int32_t)(int8_t)k->ry << 4, k->scale); }
	static void RockMove(RockNode *k)
	{
		k->rx = (uint8_t)(k->rx + k->spin[0]);
		k->pos[0] = (int16_t)(k->pos[0] + k->vel[0]);
		const int16_t vx = (int16_t)(k->acc[0] + k->vel[0]);
		k->ry = (uint8_t)(k->ry + k->spin[1]);
		k->pos[1] = (int16_t)(k->pos[1] + k->vel[1]);
		k->pos[2] = (int16_t)(k->pos[2] + k->vel[2]);
		const int16_t vy = (int16_t)(k->acc[1] + k->vel[1]);
		k->vel[0] = vx;
		const int16_t vz = (int16_t)(k->acc[2] + k->vel[2]);
		k->vel[1] = vy;
		k->vel[2] = vz;
	}
	static uint32_t RockStep(RockNode *k)
	{
		RockDraw(k);
		if (--k->life == 0) return 2;
		RockMove(k);
		return 0;
	}
	static uint32_t __cdecl RockATask(TaskNode *n)
	{
		return RockStep((RockNode *)n);
	}
	static uint32_t __cdecl RockBTask(TaskNode *n)
	{
		return RockStep((RockNode *)n);
	}

	static void SpawnRocks(int32_t n, int32_t x, int32_t y, int32_t z, int32_t l0, int32_t l1, int32_t d0, int32_t d1, int32_t s0, int32_t s1)
	{
		for (;;)
		{
			const uint32_t task = (CrtRand() & 1) ? game_addr(ORIG_RockATask) : game_addr(ORIG_RockBTask);
			RockNode *k = (RockNode *)AddTaskToQueue(QRocks(), task);
			if (!k) return;
			U32(k, 0x18) = 0;
			k->acc[1] = 0x14;
			k->scale = (int16_t)((mul32(CrtRand(), s0) >> 15) + s1);
			int32_t dist = (mul32(CrtRand(), d0) >> 15) + d1;
			const int32_t a = CrtRand();
			int32_t sx = mul32(ComputeSin(a), dist) >> 12;
			int32_t cz = mul32(ComputeCos(a), dist) >> 12;
			k->pos[0] = (int16_t)(x + sx);
			k->pos[1] = (int16_t)y;
			k->pos[2] = (int16_t)(cz + z);
			k->life = (uint8_t)((uint8_t)(mul32(CrtRand(), l0) >> 15) + (uint8_t)l1);
			sx = mul32(sx, d1) >> 14;
			cz = mul32(cz, d1) >> 14;
			dist = -dist >> 2;
			k->vel[0] = (int16_t)sx;
			k->vel[2] = (int16_t)cz;
			k->vel[1] = (int16_t)dist;
			k->rx = (uint8_t)CrtRand();
			k->ry = (uint8_t)CrtRand();
			k->spin[0] = (int8_t)CrtRand();
			k->spin[1] = (int8_t)CrtRand();
			if (n-- == 0) return;
		}
	}

	static void SpriteDraw(const SpriteNode *s, uint32_t seq)
	{
		TransformCameraByShadowRotation(s->pos, s->size, -(s->size >> 2));
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		U32(h, 0) = seq;
		W16(h, 4) = (int8_t)s->frame;
		U16(h, 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, OTBase() + 0x44, 2, PacketCursor());
		FieldFree(0xB4);
	}
	static uint32_t __cdecl SparkTask(TaskNode *n)
	{
		SpriteNode *s = (SpriteNode *)n;
		SpriteDraw(s, game_addr(SEQ_Spark));
		s->pos[0] = (int16_t)(s->pos[0] + (int16_t)(s->vel[0] << 2));
		s->pos[1] = (int16_t)(s->pos[1] + (int16_t)(s->vel[1] << 2));
		s->pos[2] = (int16_t)(s->pos[2] + (int16_t)(s->vel[2] << 2));
		s->frame++;
		return (int8_t)s->frame < 0xC ? 0 : 2;
	}
	static uint32_t __cdecl EmberTask(TaskNode *n)
	{
		SpriteNode *s = (SpriteNode *)n;
		SpriteDraw(s, game_addr(SEQ_Ember));
		s->pos[0] = (int16_t)(s->pos[0] + (int16_t)(s->vel[0] << 2));
		s->pos[1] = (int16_t)(s->pos[1] + (int16_t)(s->vel[1] << 2));
		s->pos[2] = (int16_t)(s->pos[2] + (int16_t)(s->vel[2] << 2));
		s->vel[0] = (int8_t)(s->acc[0] + s->vel[0]);
		s->vel[1] = (int8_t)(s->acc[1] + s->vel[1]);
		s->vel[2] = (int8_t)(s->acc[2] + s->vel[2]);
		s->frame++;
		return (int8_t)s->frame < 8 ? 0 : 2;
	}

	static void SpawnSparks(int32_t n, int32_t x, int32_t y, int32_t z, int32_t d0, int32_t d1, int32_t s0, int32_t s1)
	{
		for (;;)
		{
			SpriteNode *s = (SpriteNode *)AddTaskToQueue(QSprites(), game_addr(ORIG_SparkTask));
			if (!s) return;
			s->size = (int16_t)((mul32(CrtRand(), s0) >> 15) + s1);
			const int32_t dist = (mul32(CrtRand(), d0) >> 15) + d1;
			const int32_t a = CrtRand();
			const int32_t sx = mul32(ComputeSin(a), dist) >> 12;
			const int32_t cz = mul32(ComputeCos(a), dist) >> 12;
			s->pos[0] = (int16_t)(x + sx);
			s->pos[1] = (int16_t)y;
			s->frame = 0;
			s->pos[2] = (int16_t)(cz + z);
			s->vel[2] = (int8_t)(mul32(cz, d1) >> 16);
			s->vel[0] = (int8_t)(mul32(sx, d1) >> 16);
			s->vel[1] = 0;
			if (n-- == 0) return;
		}
	}

	static void SpawnEmbers(int32_t n, int32_t x, int32_t y, int32_t z)
	{
		for (;;)
		{
			SpriteNode *s = (SpriteNode *)AddTaskToQueue(QSprites(), game_addr(ORIG_EmberTask));
			if (!s) return;
			s->size = 0x800;
			s->pos[0] = (int16_t)(W16(Caster(), 0x1C) + x);
			s->pos[1] = (int16_t)(W16(Caster(), 0x1E) + y);
			s->frame = 0;
			s->pos[2] = (int16_t)(W16(Caster(), 0x20) + z);
			s->vel[0] = (int8_t)((uint8_t)CrtRand() + 0x80);
			s->vel[1] = (int8_t)(0x80 - (uint8_t)CrtRand());
			const uint8_t vz = (uint8_t)((uint8_t)CrtRand() + 0x80);
			s->vel[2] = (int8_t)vz;
			s->acc[0] = (int8_t)-sdiv_pow2(s->vel[0], 3);
			s->acc[1] = (int8_t)-sdiv_pow2(s->vel[1], 3);
			s->acc[2] = (int8_t)-sdiv_pow2((int8_t)vz, 3);
			if (n-- == 0) return;
		}
	}

	static void Play(int site, uint8_t *layout, PlayArg *arg, const Mat4x3 *frame)
	{
		(void)frame;
		prim::play((prim::Layout *)layout, Callback, (int)arg, 0);
	}

	static void DrawTwist(int32_t scale, int32_t angle, bool last, const int16_t *point, uint32_t morph)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x94);
		Mat4x3 *m = (Mat4x3 *)(h + 0x6C);
		ScaledRotY(0x400, 0x800, m);
		int16_t p[4];
		if (point) ff8fx::copy<(6)>(h + 0x8C, point);
		else GetEffectSpawnPosition(Caster(), 0x14, 0, (int16_t *)(h + 0x8C));
		ff8fx::copy<(6)>(p, h + 0x8C);
		m->t[0] = p[0];
		m->t[1] = p[1];
		m->t[2] = p[2];
		ComposeAffineTransform(&Camera(), m, m);
		GteSetRotMatrix(m);
		GteSetTransVector(m);
		U32(h, 0) = game_addr(MODEL_Twist);
		U32(h, 4) = morph;
		TwistVertices(game_addr(MODEL_Twist), (uint8_t *)morph, scale, angle);
		U32(h, 0x1C) = 0x2030;
		if (last)
		{
			U32(h, 0x1C) = 0x20F0;
			U32(h, 0xC) = 0x800;
			U32(h, 8) = 0;
		}
		W32(h, 0x18) = -0x80;
		U16(h, 0x22) = 0;
		U16(h, 0x20) = 0;
		PacketCursor() = RenderPrimSet(h, OTBase() + 0x44, 2, PacketCursor());
		FieldFree(0x94);
	}

	static void DrawTargets(const int16_t *angles, const int16_t *off, const int32_t *scale)
	{
		ForChain([&](uint8_t *e)
		{
			if (B8(e, 0) & 2)
			{
				DrawEntity(e, angles, off, scale);
				B8(e, 0) |= 4;
			}
		});
	}

	static int32_t FlashLevel(int32_t c)
	{
		if (c < 0xC) return mul32(c, 2700) / 12;
		if (c >= 0x7F) return mul32(0x8B - c, 2700) / 12;
		return 0xA8C;
	}
	static int32_t FlashLevel2(int32_t h)
	{
		int32_t v;
		if ((uint32_t)h < 0xC) v = (int32_t)(((uint32_t)h << 12) / 12) + 0xAA;
		else if ((uint32_t)h >= 0x54) v = (int32_t)((uint32_t)(0x5F000 - ((uint32_t)h << 12)) / 12) + 0xAA;
		else v = 0x1000;
		return (mul32(v, 1396) >> 12) + 0xA8C;
	}

	static void CamSet(int16_t ax, int16_t ay, int16_t az, int16_t ex, int16_t ey, int16_t ez)
	{
		CamLookAt()[0] = ax;
		CamLookAt()[1] = ay;
		CamLookAt()[2] = az;
		CamEye()[0] = ex;
		CamEye()[1] = ey;
		CamEye()[2] = ez;
	}
	static void DirectorCamera(int32_t c, bool engine)
	{
		if (c == 0)
		{
			if (engine) CameraArmReturn();
			ViewWord() = 0x120;
			CamRoll() = 0;
			const int16_t x = (int16_t)CamAnchor()[0], z = (int16_t)CamAnchor()[1];
			CamSet((int16_t)(x - 0x88), (int16_t)0xFDE2, (int16_t)(z - 0xD0), (int16_t)(x - 0x270), (int16_t)0xFD50, (int16_t)(z - 0x750));
		}
		const int16_t px = (int16_t)Anchor()[0], py = (int16_t)(Anchor()[0] >> 16), pz = (int16_t)Anchor()[1];
		const int16_t hy = (int16_t)Height();
		if (c == 8) CamSet((int16_t)(px - 0x2D), (int16_t)0xFE1D, (int16_t)(pz + 0x1CA), (int16_t)(px + 0x3EB), (int16_t)0xFDB7, (int16_t)(pz + 0x9D3));
		if (c == 0x10) CamSet((int16_t)(px - 0x62), (int16_t)0xFC86, (int16_t)(pz - 0x99), (int16_t)(px + 0x9CC), (int16_t)0xFADD, (int16_t)(pz + 0x1B1));
		int32_t s = c - 0x16;
		if ((uint32_t)s < 0x30)
		{
			const int16_t a[6] = { (int16_t)(px + 0xC1), (int16_t)(py - 0xC2F), (int16_t)(pz - 0x191), (int16_t)(px + 0x3D2), (int16_t)(py - 0x988), (int16_t)(pz - 0xF9) };
			const int16_t b[6] = { (int16_t)(px + 0x1D), (int16_t)(py - 0x2414), (int16_t)(pz - 0x292), (int16_t)(px + 0xBB3), (int16_t)(py - 0x131C), (int16_t)(pz + 0x3F) };
			CamLerp(a, b, (int32_t)(((uint32_t)s << 12) / 47));
		}
		s = c - 0x46;
		if ((uint32_t)s < 0x20)
		{
			const int16_t a[6] = { (int16_t)(px - 0x19), (int16_t)(hy - 0x3B8A), (int16_t)(pz + 0x13D4), (int16_t)(px - 0x81), (int16_t)(hy - 0x3AC6), (int16_t)(pz + 0x1BC0) };
			const int16_t b[6] = { (int16_t)(px + 0x42), (int16_t)(hy - 0x391D), (int16_t)(pz + 0x11CF), (int16_t)(px + 0x1512), (int16_t)(hy - 0x3848), (int16_t)(pz + 0x12AC) };
			CamLerp(a, b, (int32_t)(((uint32_t)s << 12) / 31));
		}
		s = c - 0x66;
		if ((uint32_t)s < 0xA)
		{
			const int16_t a[6] = { (int16_t)(px + 0x42), (int16_t)(hy - 0x391D), (int16_t)(pz + 0x11CF), (int16_t)(px + 0x1512), (int16_t)(hy - 0x3848), (int16_t)(pz + 0x12AC) };
			const int16_t b[6] = { (int16_t)(px + 0x12), (int16_t)(hy - 0xAB6), (int16_t)(pz - 0x27), (int16_t)(px + 0x381), (int16_t)(hy - 0x16C3), (int16_t)(pz + 0x1164) };
			CamLerp(a, b, (int32_t)(((uint32_t)s << 12) / 9));
		}
		if (c == 0x70) CamSet((int16_t)(px + 0x211), (int16_t)(py - 0x50C), (int16_t)(pz - 0x2CA), (int16_t)(px + 0x905), (int16_t)(py - 0x7C0), (int16_t)(pz - 0xA44));
	}

	static uint32_t __cdecl DirectorTask(TaskNode *n)
	{
		DirectorNode *d = (DirectorNode *)n;
		uint8_t *z = Caster();
		const int32_t c = d->counter;
		Flash() = FlashLevel(c);
		PlayArg arg;
		ff8fx::fill<(sizeof(arg))>(&arg, 0);
		Mat4x3 frame;
		const uint32_t morph = TexBase() + MORPH;
		int32_t s = c - 9;
		if ((uint32_t)s < 0xE)
		{
			if (s == 0)
			{
				W16(z, 0xE) = 0;
				U32(z, 0x1C) = *(const uint32_t *)&d->place[0];
				U32(z, 0x20) = *(const uint32_t *)&d->place[2];
				DecodeModelPrimLayout(game_addr(LAYOUT_Rise), d->layout_a, 0x40);
				d->lift = 0xC40;
				d->ghosts[3] = 0;
				d->ghosts[2] = 0;
				d->ghosts[1] = 0;
				d->ghosts[0] = 0;
			}
			RotationY(0, &arg.m);
			int16_t p[4];
			GetDefaultEffectPosition(z, p);
			arg.m.t[0] = p[0];
			arg.m.t[1] = 0;
			arg.m.t[2] = p[2];
			frame = arg.m;
			ComposeAffineTransform(&Camera(), &arg.m, &arg.m);
			arg.morph = morph;
			arg.depth = 0;
			arg.fade = 0;
			arg.kind = (const uint8_t *)game_addr(KIND_Rise);
			arg.scroll = (int16_t)(s << 5);
			Play(SITE_RISE, d->layout_a, &arg, &frame);
			if ((uint32_t)s >= 8 && (uint32_t)s < 0xD)
			{
				const int32_t t = s - 8;
				DrawTwist(shl32(t, 12) / 5 / 2 + 0x800, 0x400 - shl32(t, 10), s == 0xC, nullptr, TexBase() + MORPH);
			}
			if (s == 9)
			{
				SpawnRing(0, -0x1F4, -0x3E8, 0x1000);
				SpawnEmbers(0x10, 0, -0x1F4, -0x3E8);
			}
			if ((uint32_t)s >= 9) W16(Target(), 0x1E) = (int16_t)(W16(Target(), 0x1E) - 0x280);
		}
		s = c - 0x17;
		if ((uint32_t)s < 0x30)
		{
			const int32_t r = (int32_t)((uint32_t)s % 6);
			if (s == 0)
			{
				W16(z, 0x1E) = (int16_t)(W16(z, 0x1E) - 0xA00);
				ForChain([](uint8_t *e) { if (B8(e, 0) & 2) B8(e, 0) |= 8; });
			}
			W16(z, 0x1E) = (int16_t)(W16(z, 0x1E) - 0xD0);
			if (r >= 2 && r <= 4) W16(Target(), 0x1E) = (int16_t)(W16(Target(), 0x1E) - 0x1A0);
			bool twist = r <= 3;
			if (r == 0)
			{
				SpawnRing(0, -0x1F4, -0x3E8, 0x1000);
				SpawnEmbers(0x10, 0, -0x1F4, -0x3E8);
				ForChain([](uint8_t *e) { if (B8(e, 0) & 2) U16(e, 0) &= 0xFFF7; });
				QueueChainTransformation(Target(), 4);
				twist = true;
			}
			if (twist) DrawTwist(shl32(r, 12) / 3 / 2 + 0x800, 0x400 - shl32(r, 10), r == 3, nullptr, TexBase() + MORPH);
			if (((uint32_t)s / 6) & 1) d->angles[0] = (int16_t)-d->angles[0];
			const int32_t one[3] = { 0x1000, 0x1000, 0x1000 };
			DrawTargets(ZeroWords(), ZeroWords(), one);
		}
		s = c - 0x5D;
		if ((uint32_t)s < 0xA)
		{
			if (s == 0)
			{
				DecodeModelPrimLayout(game_addr(LAYOUT_Fly), d->layout_a, 0x484);
				ForChain([](uint8_t *e) { if (B8(e, 0) & 2) U16(e, 0) = (uint16_t)((U16(e, 0) & 0xFFFB) | 8); });
				MeasureHeight(Target(), d->height);
				d->height[0] = (int16_t)(d->height[0] - W16(Target(), 0x1E));
				d->height[1] = (int16_t)(d->height[1] - W16(Target(), 0x1E));
			}
			RotationX(-0x200, &arg.m);
			int16_t p[4];
			GetEffectSpawnPosition(z, 0x14, 0, p);
			arg.m.t[0] = p[0];
			arg.m.t[1] = p[1];
			arg.m.t[2] = p[2];
			frame = arg.m;
			ComposeAffineTransform(&Camera(), &arg.m, &arg.m);
			arg.morph = morph;
			arg.kind = (const uint8_t *)game_addr(KIND_Fly);
			arg.scroll = (int16_t)(s << 5);
			arg.depth = -0x80;
			arg.fade = 0;
			Play(SITE_FLY, d->layout_a, &arg, &frame);
			if ((uint32_t)s >= 8)
			{
				W16(z, 0x1E) = (int16_t)(W16(z, 0x1E) + 0x200);
				W16(z, 0x20) = (int16_t)(W16(z, 0x20) - 0x200);
			}
		}
		s = c - 0x47;
		if ((uint32_t)s < 0x16 && s == 0)
		{
			W16(z, 0x1E) = (int16_t)(W16(z, 0x1E) + (int16_t)(d->height[1] - d->height[0] - 0x600));
			W16(z, 0x20) = (int16_t)(W16(z, 0x20) + (int16_t)(0x1788 - (W16(z, 0x26) >> 1)));
			W16(Target(), 0x20) = (int16_t)(W16(Target(), 0x20) + 0x1388);
			d->angles[0] = 0;
			d->lift = (int16_t)(Height() - 0x1055);
			d->sway_y = (int16_t)(W16(Target(), 0x1E) + 0x600);
			QRings()->tail = nullptr;
			QRings()->head = nullptr;
		}
		if ((uint32_t)s < 0x20)
		{
			const int32_t a = (int32_t)(((uint32_t)s << 12) >> 5);
			d->lift = (int16_t)(d->lift + (int16_t)(ComputeCos(a >> 2) / 12));
			W16(Target(), 0x1E) = (int16_t)(d->sway_y - (int16_t)(ComputeSin(a >> 2) >> 1));
			d->angles[0] = (int16_t)(ComputeSin(a * 2) >> 4);
			const int16_t off[3] = { 0, (int16_t)((d->height[1] + d->height[0]) >> 1), 0 };
			const int32_t one[3] = { 0x1000, 0x1000, 0x1000 };
			DrawTargets(d->angles, off, one);
		}
		else if (s == 0x20)
		{
			ForChain([](uint8_t *e) { if (B8(e, 0) & 2) U16(e, 0) &= 0xFFFB; });
		}
		s = c - 0x67;
		if ((uint32_t)s < 0x12)
		{
			if (s == 0) DecodeModelPrimLayout(game_addr(LAYOUT_Drop), d->layout_b, 0x160);
			if ((uint32_t)s < 0xA)
			{
				RotationX(-0x200, &arg.m);
				arg.fade = 0;
			}
			else
			{
				RotationX(-0x400, &arg.m);
				arg.fade = (int16_t)((s - 0xA) << 9);
			}
			int16_t p[4];
			GetEffectSpawnPosition(z, 0x14, 0, p);
			arg.m.t[0] = p[0];
			arg.m.t[1] = p[1];
			arg.m.t[2] = p[2];
			frame = arg.m;
			ComposeAffineTransform(&Camera(), &arg.m, &arg.m);
			arg.morph = morph;
			arg.kind = (const uint8_t *)game_addr(KIND_Fly);
			arg.scroll = (int16_t)(s << 5);
			arg.depth = -0x80;
			Play(SITE_DROP_A, d->layout_a, &arg, &frame);
			Play(SITE_DROP_B, d->layout_b, &arg, &frame);
			if ((uint32_t)s < 0xA)
			{
				W16(z, 0x1E) = (int16_t)(W16(z, 0x1E) + 0x4E6);
				W16(Target(), 0x1E) = (int16_t)(W16(Target(), 0x1E) + 0x4E6);
				W16(z, 0x20) = (int16_t)(W16(z, 0x20) - 0x1F4);
				W16(Target(), 0x20) = (int16_t)(W16(Target(), 0x20) - 0x1F4);
			}
			if (s == 9) W16((void *)Anchor(), 4) = d->anchor[2];
		}
		s = c - 0x71;
		if ((uint32_t)s < 0x19)
		{
			const int32_t e24 = (int32_t)(((uint32_t)s << 12) / 24);
			if (s == 0)
			{
				DecodeModelPrimLayout(game_addr(LAYOUT_Impact), d->layout_c, 0xA8);
				DecodeModelPrimLayout(game_addr(LAYOUT_Crater), d->layout_d, 0x684);
				W16(Target(), 0x1E) = (int16_t)(W16(&d->target_pos[0], 2) - d->height[0]);
				U32(z, 0xC) = d->saved_rot[0];
				U32(z, 0x10) = d->saved_rot[1];
				W16(z, 0x1E) = (int16_t)(d->height[1] - d->height[0]);
				W16(z, 0x1C) = d->anchor[0];
				W16(z, 0x20) = d->anchor[2];
			}
			else if ((uint32_t)s - 1 <= 1)
			{
				PacketCursor() = FadeTile(s - 1, 0x80, 0x80, 0x80, 0, 1, 1, PacketCursor());
			}
			if ((uint32_t)s < 4)
			{
				const int32_t sc = d->scale >> 1;
				SpawnRocks(0x10, d->anchor[0], 0, d->anchor[2], 5, 0xF, sc, sc, 0x3E8, 0x3E8);
				const int32_t y = -(CrtRand() & 0x3FF);
				SpawnSparks(8, d->anchor[0], y, d->anchor[2], sc, sc, 0x7D0, 0x7D0);
			}
			int32_t v = 0x1000 - ComputeSin(e24 >> 1);
			v = mul32(v, v) >> 12;
			v = mul32(v, v) >> 12;
			v = mul32(v, v) >> 12;
			const int32_t k = (v >> 2) + (v >> 1) + 0x400;
			W16(Target(), 0x1E) = (int16_t)(W16(&d->target_pos[0], 2) - (int16_t)(mul32(d->height[0], k) / 0x1000));
			{
				const int32_t sq[3] = { 0x1000, k, 0x1000 };
				DrawTargets(ZeroWords(), ZeroWords(), sq);
			}
			if ((uint32_t)s < 0xC)
			{
				W16(z, 0x1E) = (int16_t)(mul32(d->height[1] - d->height[0], k) / 0x1000);
				RotationY(0, &arg.m);
				arg.m.t[0] = W16(z, 0x1C);
				arg.m.t[1] = W16(z, 0x1E);
				arg.m.t[2] = W16(z, 0x20) + 0x1F4;
				frame = arg.m;
				ComposeAffineTransform(&Camera(), &arg.m, &arg.m);
				arg.morph = morph;
				arg.depth = 0;
				arg.fade = 0;
				arg.kind = (const uint8_t *)game_addr(KIND_Impact);
				Play(SITE_IMPACT, d->layout_c, &arg, &frame);
			}
			ff8fx::fill<(sizeof(Mat4x3))>(&arg.m, 0);
			arg.m.m[0][0] = d->scale;
			arg.m.m[1][1] = d->scale;
			arg.m.m[2][2] = d->scale;
			arg.m.t[0] = d->anchor[0];
			arg.m.t[1] = 0;
			arg.m.t[2] = d->anchor[2];
			frame = arg.m;
			ComposeAffineTransform(&Camera(), &arg.m, &arg.m);
			arg.morph = morph;
			arg.depth = 0;
			arg.fade = 0;
			arg.kind = (const uint8_t *)game_addr(KIND_Crater);
			Play(SITE_CRATER, d->layout_d, &arg, &frame);
			if ((uint32_t)s >= 0xD && (uint32_t)s <= 0x11)
			{
				const int32_t t = 0x11 - s;
				if (s == 0xE) W16(z, 0xE) = W16(&d->saved_rot[0], 2);
				W16(z, 0x1E) = (int16_t)(W16(&d->saved_pos[0], 2) - (int16_t)(t << 8));
				const int32_t dist = (t << 8) + 0xBB8;
				const int16_t yaw = W16(&d->saved_rot[0], 2);
				W16(z, 0x1C) = (int16_t)(W16(&d->saved_pos[0], 0) - (int16_t)(mul32(ComputeSin(yaw), dist) / 0x1000));
				W16(z, 0x20) = (int16_t)(W16(&d->saved_pos[1], 0) - (int16_t)(mul32(ComputeCos(yaw), dist) / 0x1000));
			}
			else if (s == 0x12)
			{
				U32(z, 0x1C) = d->saved_pos[0];
				U32(z, 0x20) = d->saved_pos[1];
			}
		}
		else if (s == 0x19)
		{
			ForChain([](uint8_t *e) { if (B8(e, 0) & 2) U16(e, 0) &= 0xFFF3; });
			ApplyActionResultToTarget(Targets());
		}
		s = c - 0x1D;
		if ((uint32_t)s < 0x60) Flash() = FlashLevel2(s);
		s = c - 0x29;
		if ((uint32_t)s < 0x48)
		{
			if (s == 0) DecodeModelPrimLayout(game_addr(LAYOUT_Pillar), d->layout_e, 0xEC);
			RotationY(0, &arg.m);
			arg.m.t[0] = d->place[0];
			arg.m.t[1] = d->lift - 0x3A80;
			arg.m.t[2] = d->place[2];
			frame = arg.m;
			ComposeAffineTransform(&Camera(), &arg.m, &arg.m);
			RotationY(0, &arg.m);
			arg.m.m[0][1] = (int16_t)(arg.m.m[0][1] + (int16_t)sdiv_pow2(arg.m.m[0][1], 3));
			arg.kind = (const uint8_t *)game_addr(KIND_Impact);
			arg.depth = -0x200;
			arg.m.m[1][1] = (int16_t)(arg.m.m[1][1] + (int16_t)sdiv_pow2(arg.m.m[1][1], 3));
			arg.m.m[2][1] = (int16_t)(arg.m.m[2][1] + (int16_t)sdiv_pow2(arg.m.m[2][1], 3));
			arg.morph = morph;
			if ((uint32_t)s < 0xC) arg.fade = (int16_t)((uint32_t)(0xC000 - ((uint32_t)s << 12)) / 12 + 0xAA);
			else if ((uint32_t)s >= 0x3C) arg.fade = (int16_t)(((uint32_t)(s - 0x3C) << 12) / 12 + 0xAA);
			else arg.fade = 0;
			Play(SITE_PILLAR, d->layout_e, &arg, &frame);
		}
		if ((c > 0 && c <= 0xC) || (c > 0x7B && c <= 0x82) || (c > 0x17 && c < 0x47)) GhostDraw(z, d->ghosts);
		if ((c >= 0 && c <= 0xA) || (c >= 0x7B && c <= 0x80) || (c >= 0x17 && c <= 0x23)) GhostRecord(z, d->ghosts);
		if (c == 0) BdPlaySE(SOUND_Start(), 0, 0x80);
		DirectorCamera(c, true);
		d->counter = (int16_t)(c + 1);
		if (d->counter < 0x8B) return 0;
		StreamQueueLoad(0x168, TexBase() + 0x3198, 1);
		StreamQueueLoad(0x169, TexBase() + 0x3198, 1);
		StreamQueueLoad(0x16A, TexBase() + 0x3198, 0);
		SetScreenFlash(0, 0);
		return TASK_END;
	}

	static void StartDirector(DirectorNode *d)
	{
		Memset32(&d->counter, 0, 0x5F9);
		uint8_t *z = Caster();
		d->saved_pos[0] = U32(z, 0x1C);
		d->saved_pos[1] = U32(z, 0x20);
		d->saved_rot[0] = U32(z, 0xC);
		d->saved_rot[1] = U32(z, 0x10);
		GetDefaultEffectPosition(Target(), d->place);
		ff8fx::copy<(8)>(d->anchor, d->place);
		d->place[1] = 0;
		d->scale = (int16_t)((W16(Target(), 0x26) >> 1) + 0x400);
		int16_t box[6];
		ModelBox(game_addr(LAYOUT_Crater) + *(const uint32_t *)(game_addr(LAYOUT_Crater) + 0xC), box);
		box[0] = (int16_t)(mul32(box[0], d->scale) >> 12);
		box[3] = (int16_t)(mul32(box[3], d->scale) >> 12);
		box[1] = (int16_t)(mul32(box[1], d->scale) >> 12);
		box[4] = (int16_t)(mul32(box[4], d->scale) >> 12);
		box[2] = (int16_t)(mul32(box[2], d->scale) >> 12);
		box[5] = (int16_t)(mul32(box[5], d->scale) >> 12);
		CaptureStage(d->place, box);
		d->place[2] = (int16_t)(d->place[2] + (int16_t)((W16(Target(), 0x26) >> 1) + (W16(z, 0x26) >> 1)));
		CamAnchor()[0] = U32(z, 0x1C);
		CamAnchor()[1] = U32(z, 0x20);
		Anchor()[0] = *(const uint32_t *)&d->place[0];
		Anchor()[1] = *(const uint32_t *)&d->place[2];
		BindExternalAnimSet(z, d->bind, game_addr(COM_Anim));
		QueueChainTransformation(z, 1);
		StreamStateInit((void *)(TexBase() + 0x2C90));
		U16(z, 0) &= 0xFFFB;
		MeasureHeight(Target(), d->height);
		Height() = d->height[1] - d->height[0] + 0x4E1;
		d->target_pos[0] = U32(Target(), 0x1C);
		d->target_pos[1] = U32(Target(), 0x20);
		W16(Target(), 0x1E) = (int16_t)(W16(Target(), 0x1E) - d->height[0]);
		ForChain([](uint8_t *e) { if (B8(e, 0) & 2) B8(e, 0) |= 8; });
		static const int32_t bones[0x12] = { 1, 2, 4, 8, 9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16 };
		GhostInit(d->ghosts, 0x12, bones);
	}

	static uint32_t __cdecl RootTask(TaskNode *n)
	{
		RootNode *r = (RootNode *)n;
		if (r->parity)
		{
			PacketCursor() = TexBase() + 0x3198;
			CaptureCursor() = TexBase() + 0x1D198;
			r->parity = 0;
		}
		else
		{
			PacketCursor() = TexBase() + 0x1D198;
			CaptureCursor() = TexBase() + 0x37198;
			r->parity = 1;
		}
		Flash() = 0;
		if (r->counter == 1)
		{
			if (r->state == 0)
			{
				r->state = 1;
				InitTaskQueuePool(QDirector(), (void *)TexBase(), 0x17F0, 1);
				InitTaskQueuePool(QRings(), (void *)(TexBase() + 0x17F0), 0x104, 8);
				InitTaskQueuePool(QRocks(), (void *)(TexBase() + 0x2010), 0x24, 0x40);
				InitTaskQueuePool(QSprites(), (void *)(TexBase() + 0x2910), 0x1C, 0x20);
			}
			DirectorNode *d = (DirectorNode *)AddTaskToQueue(QDirector(), game_addr(ORIG_DirectorTask));
			if (d) StartDirector(d);
		}
		OrthonormalizeCamera(&Camera(), View());
		int a = 1;
		if (r->state)
		{
			a = ExecuteTaskQueue(QDirector());
			ExecuteTaskQueue(QRings());
			ExecuteTaskQueue(QRocks());
			ExecuteTaskQueue(QSprites());
		}
		const int32_t f = Flash();
		if (f != FlashLast())
		{
			SetScreenFlash((uint32_t)f, 0);
			if (Flash() >= 0x1000)
			{
				for (uint32_t p = game_addr(game::unk_1D98991); p < game_addr(game::unk_1D98A41); p += 0x2C) var<uint8_t>(p) &= 0xFD;
			}
			else if (FlashLast() >= 0x1000)
			{
				for (uint32_t p = game_addr(game::unk_1D98991); p < game_addr(game::unk_1D98A41); p += 0x2C) var<uint8_t>(p) |= 2;
			}
			FlashLast() = Flash();
		}
		if (r->state && a == 0)
		{
			StreamPump();
			if (StreamPreload(1) == 0) return 0;
			const DirectorNode *d = (const DirectorNode *)TexBase();
			U32(Target(), 0x1C) = d->target_pos[0];
			U32(Target(), 0x20) = d->target_pos[1];
			ForChain([](uint8_t *e) { if (B8(e, 0) & 2) U16(e, 0) &= 0xFFF7; });
			U32(Caster(), 0x1C) = d->saved_pos[0];
			U32(Caster(), 0x20) = d->saved_pos[1];
			U32(Caster(), 0xC) = d->saved_rot[0];
			U32(Caster(), 0x10) = d->saved_rot[1];
			ViewWord() = var<int16_t>(game_addr(game::Battle_Camera_ReturnView_Param1D8E038));
			return TASK_END;
		}
		r->counter++;
		return 0;
	}
}

	void register_mag216_different_beat()
	{
		register_port(beat216::ORIG_RootTask, (void *)beat216::RootTask, "216 RootTask", 216);
		register_port(beat216::ORIG_DirectorTask, (void *)beat216::DirectorTask, "216 DirectorTask", 216);
		register_port(beat216::ORIG_RingTask, (void *)beat216::RingTask, "216 RingTask", 216);
		register_port(beat216::ORIG_RockATask, (void *)beat216::RockATask, "216 RockATask", 216);
		register_port(beat216::ORIG_RockBTask, (void *)beat216::RockBTask, "216 RockBTask", 216);
		register_port(beat216::ORIG_SparkTask, (void *)beat216::SparkTask, "216 SparkTask", 216);
		register_port(beat216::ORIG_EmberTask, (void *)beat216::EmberTask, "216 EmberTask", 216);
	}
}

