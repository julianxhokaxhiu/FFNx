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

// Effect 50: Potion

#include "mag_common.h"

namespace ff8fx
{
namespace potion050
{
	using namespace eng;
	using namespace magc;

#pragma pack(push, 1)
	struct FxNode
	{
		TaskNode hdr;
		CastContext *ctx;
		FxNode *root;
		FxNode *emitter;
		FxNode *parent;
		int16_t pos[4];
		int16_t counter;
		uint16_t flags;
		uint8_t children;
		int8_t phase;
		int8_t action;
		int8_t target;
		uint8_t attacker;
		uint8_t slot;
		uint8_t line;
		uint8_t last;
	};
	struct RootNode
	{
		FxNode h;
		uint8_t bones[0x28];
		int16_t last_action;
		int16_t target_count;
		int16_t tick;
		int16_t alive;
		uint8_t pad60[3];
		uint8_t busy;
	};
	struct EmitterNode
	{
		FxNode h;
		uint8_t anchor[0x18];
		int16_t bmin[4];
		int16_t bmax[4];
	};
	struct Particle
	{
		FxNode h;
		uint8_t pad30[0x12];
		int16_t angle;
		uint8_t pad44[8];
		uint32_t seq;
		int16_t frame;
		int16_t last;
		int16_t shade;
		int16_t pad56;
		int16_t vel[3];
		int16_t pad5E;
		int16_t center[4];
		int16_t radius;
		int16_t pad6A;
	};
	struct OverlayCtx
	{
		uint32_t a00;
		uint8_t *sprite;
		uint8_t *entity;
		uint32_t *frame_cursor;
		uint32_t *packets;
		Mat4x3 world;
		Mat4x3 view;
		Mat4x3 world2;
		Mat4x3 light;
		uint8_t pad94[0x10];
		int16_t a4[3];
		int16_t padAA;
		int16_t ac[3];
		int16_t padB2;
		int16_t offset[3];
		int16_t padBA;
		int16_t bc[3];
		int16_t padC2;
		uint8_t rgb[3];
		uint8_t padC7;
		int32_t light_dist;
		int16_t cc;
		int16_t ce;
		int16_t padD0;
		int16_t mode;
		uint8_t padD4[4];
		int16_t anim;
		int16_t slot;
		int16_t light_pos[3];
		int16_t padE2;
		int16_t proj_w;
		int16_t proj_h;
		int16_t u_off;
		int16_t v_off;
		uint16_t tpage;
		uint16_t clut;
		int16_t padF0;
		int16_t glow;
		int16_t cull;
		int16_t padF6;
	};
#pragma pack(pop)
	static_assert(sizeof(FxNode) == 0x30 && sizeof(RootNode) == 0x64 && sizeof(EmitterNode) == 0x58, "Potion nodes");
	static_assert(sizeof(Particle) == 0x6C && sizeof(OverlayCtx) == 0xF8, "Potion nodes");

	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_050_POTION_PacketCursor)); }
	inline uint32_t &PacketLimit() { return var<uint32_t>(game_addr(game::MAG_050_POTION_PacketLimit)); }
	inline TaskQueue *QEmitters() { return (TaskQueue *)game_addr(game::MAG_050_POTION_EmitterQueue); }
	inline TaskQueue *QParticles() { return (TaskQueue *)game_addr(game::MAG_050_POTION_ParticleQueue); }
	inline TaskQueue *QGlints() { return (TaskQueue *)game_addr(game::MAG_050_POTION_GlintQueue); }
	inline OverlayCtx &Overlay() { return var<OverlayCtx>(game_addr(game::MAG_050_POTION_OverlayCtx)); }
	inline uint32_t RenderOT(uint32_t off) { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + off; }

	static constexpr game::Id ORIG_Root = game::MAG_050_POTION_Root_Tick;
	static constexpr game::Id ORIG_Emitter = game::MAG_050_POTION_Emitter_Tick;
	static constexpr game::Id ORIG_Ring = game::MAG_050_POTION_Ring_Tick;
	static constexpr game::Id ORIG_Mote = game::MAG_050_POTION_Mote_Tick;
	static constexpr game::Id ORIG_Sparkle = game::MAG_050_POTION_Sparkle_Tick;
	static constexpr game::Id ORIG_Glint = game::Effect_Glint_Tick;

	static const void *SOUND_Potion() { return (const void *)game_addr(game::MAG_050_POTION_Sound); }
	static constexpr game::Id SEQ_Ring = game::MAG_050_POTION_SeqRing, SEQ_RingEnd = game::MAG_050_POTION_SeqRingEnd, SEQ_Mote = game::MAG_050_POTION_SeqMote, SEQ_Sparkle = game::MAG_050_POTION_SeqSparkle;
	static constexpr game::Id SEQ_Glow = game::MAG_050_POTION_SeqGlow;
	static constexpr game::Id CAM_SNAPSHOT = game::Effect_SharedCamMatrixSnapshot;
	static constexpr game::Id ARENA_A = game::MAG_050_POTION_ArenaA, LIMIT_A = game::MAG_050_POTION_ArenaLimitA, ARENA_B = game::MAG_050_POTION_ArenaB, LIMIT_B = game::MAG_050_POTION_ArenaLimitB;
	static constexpr game::Id OVERLAY_A00 = game::MAG050_POTION_overlayBuf0, OVERLAY_SPRITE = game::MAG_050_POTION_OverlaySprite, OVERLAY_VERTS = game::MAG_050_POTION_OverlayVerts;
	static constexpr game::Id GLINT_CURSOR = game::Effect_GlintPacketCursorPtr;

	inline FxNode *AddEffectTask(TaskQueue *q, uint32_t task, int32_t size, void *parent) { return fn<FxNode *(__cdecl *)(TaskQueue *, uint32_t, int32_t, void *)>(game_addr(game::Effect_AddTaskAndInitFromCtx))(q, task, size, parent); }
	inline void ReleaseLinkedTask(void *n) { fn<void (__cdecl *)(void *)>(game_addr(game::Effect_ReleaseLinkedTask))(n); }
	inline void EmitterUpdatePos(void *n) { fn<void (__cdecl *)(void *)>(game_addr(game::MAG_001_CURE_Emitter_UpdatePos))(n); }
	inline void ComputeModelBounds(void *n) { fn<void (__cdecl *)(void *)>(game_addr(game::MAG_001_CURE_Emitter_ComputeModelBounds))(n); }
	inline void UpdateTargetPosFromBones(void *n) { fn<void (__cdecl *)(void *)>(game_addr(game::Effect_UpdateTargetPosFromBones))(n); }
	inline void CopyAnchorXZFromSource(void *n, int16_t *out) { fn<void (__cdecl *)(void *, int16_t *)>(game_addr(game::Effect_CopyAnchorXZFromSource))(n, out); }
	inline int32_t BoundsMaxWorldY(void *n) { return fn<int32_t (__cdecl *)(void *)>(game_addr(game::Effect_GetTargetBoundsMaxWorldY))(n); }
	inline int32_t BoundsRadius(void *n) { return fn<int32_t (__cdecl *)(void *)>(game_addr(game::Effect_GetTargetBoundsRadius))(n); }
	inline int32_t BoundsHeight(void *n) { return fn<int32_t (__cdecl *)(void *)>(game_addr(game::Effect_GetTargetBoundsHeight))(n); }
	inline void IdentityMatrix(Mat4x3 *m) { fn<void (__cdecl *)(Mat4x3 *)>(game_addr(game::Mtx_Identity))(m); }
	inline void RotateX(Mat4x3 *m, int32_t a) { fn<void (__cdecl *)(Mat4x3 *, int32_t)>(game_addr(game::Mtx_RotX))(m, a); }
	inline void BuildMatrixFromDirAndUp(void *out, const void *dir, const void *up) { fn<void (__cdecl *)(void *, const void *, const void *)>(game_addr(game::Effect_BuildMatrixFromDirAndUp))(out, dir, up); }
	inline int32_t ComputeCos(int32_t a) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(a); }
	inline int32_t Sqrt(int32_t v) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::Math_SqrtInt))(v); }
	inline void NormalizeVec3(const int32_t *in, int16_t *out) { fn<void (__cdecl *)(const int32_t *, int16_t *)>(game_addr(game::Math_NormalizeVec3_Q12))(in, out); }
	inline void UnpackRotationMatrix(const void *in, void *out) { fn<void (__cdecl *)(const void *, void *)>(game_addr(game::UnpackRotationMatrix))(in, out); }
	inline void MatrixMulVector(const Mat4x3 *m, const int16_t *v, int16_t *out) { fn<void (__cdecl *)(const Mat4x3 *, const int16_t *, int16_t *)>(game_addr(game::matrixMultiplyVector))(m, v, out); }
	inline void GteLoadV012(const void *a, const void *b, const void *c) { fn<void (__cdecl *)(const void *, const void *, const void *)>(game_addr(game::GTE_LoadV012))(a, b, c); }
	inline void GteLoadV0Dwords(const void *v) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadV0FromDwords))(v); }
	inline void GteRTPT() { fn<void (__cdecl *)()>(game_addr(game::GTE_RTPT))(); }
	inline void GteNCLIP() { fn<void (__cdecl *)()>(game_addr(game::GTE_NCLIP))(); }
	inline void GteAVSZ3() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ3))(); }
	inline void GteAVSZ4() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ4))(); }
	inline void GteReadMAC0(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadMAC0))(dst); }
	inline void GteReadOTZWord(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadOTZ))(dst); }
	inline void GteReadSXY012(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadSXY012))(dst); }
	inline uint32_t GteSZ(int k) { return var<uint32_t>(game_addr(game::GTE_SZ0) + 4 * k); }
	inline void GteStoreSXY012_GT3(void *p) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreSXY012_PolyGT3))(p); }
	inline void GteStoreSXY012_FT3(void *p) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreSXY012_PolyFT3))(p); }
	inline void GteStoreSXY012_GT4(void *p) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreSXY012_PolyGT3_2))(p); }
	inline void GteStoreSXY012_FT4(void *p) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreSXY012_PolyFT3_2))(p); }
	inline void GteSetLightMatrix(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_SetLightMatrix))(m); }
	inline void GteSetBackground(int32_t x, int32_t y, int32_t z) { fn<void (__cdecl *)(int32_t, int32_t, int32_t)>(game_addr(game::GTE_SetBackgroundVector))(x, y, z); }
	inline void GteMVMVA_LightV0Bk() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_LightV0_Bk))(); }
	inline void GteMVMVA_RotIR() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotIR))(); }
	inline void GteLoadIRFromColumn(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadIRFromMatrixColumn))(m); }
	inline void GteStoreIRToColumn(void *m) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreIRToMatrixColumn))(m); }
	inline void InsertPrimDepthKeys(uint32_t ot, void *prim, uint32_t a, uint32_t b, uint32_t c, uint32_t d) { fn<void (__cdecl *)(uint32_t, void *, uint32_t, uint32_t, uint32_t, uint32_t)>(game_addr(game::SSIGPU_InsertPrimDepthKeys))(ot, prim, a, b, c, d); }
	inline uint32_t DrawShadow(uint8_t *entity, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(uint8_t *, uint32_t, int32_t, uint32_t)>(game_addr(game::BattleEntity_DrawShadow))(entity, ot, mode, cursor); }
	inline int32_t BattleReadAnimation(void *hdr, void *cmd) { return fn<int32_t (__cdecl *)(void *, void *)>(game_addr(game::Battle_ReadAnimation))(hdr, cmd); }
	inline void PreBattleReadAnimation(void *hdr, void *cmd, uint32_t anim) { fn<void (__cdecl *)(void *, void *, uint32_t)>(game_addr(game::pre_Battle_ReadAnimation))(hdr, cmd, anim); }

	template<typename T> static inline T &F(uint8_t *p, uint32_t off) { return *(T *)(p + off); }
	template<typename T> static inline const T &F(const uint8_t *p, uint32_t off) { return *(const T *)(p + off); }

	static inline int32_t Div4096(int32_t x) { return x / 4096; }

	static void DrawSprite(const Particle *p)
	{
		if (p->h.flags & 4) return;
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		TransformCameraByShadowRotation(p->h.pos, 0x1000, p->shade);
		F<uint32_t>(h, 0) = p->seq;
		F<int16_t>(h, 4) = p->frame;
		F<int16_t>(h, 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(0x44), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static void DrawAdditiveGlow(const Particle *p, uint32_t seq, const uint32_t *colour)
	{
		if (p->h.flags & 4) return;
		Mat4x3 m;
		IdentityMatrix(&m);
		RotateX(&m, 0x400);
		m.t[0] = p->h.pos[0];
		m.t[1] = 0;
		m.t[2] = p->h.pos[2];
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		F<uint32_t>(h, 0) = seq;
		F<int16_t>(h, 4) = 0;
		F<uint32_t>(h, 0x1C) = *colour;
		F<int16_t>(h, 0x24) = 4;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(0x44), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static void SetOverlayTexturePage(OverlayCtx *c, const int16_t *pos, int32_t x, int32_t y, int32_t cx, int32_t cy, int32_t w, int32_t h)
	{
		c->light_pos[0] = pos[0];
		c->light_pos[1] = pos[1];
		c->light_pos[2] = pos[2];
		const int32_t yy = (int16_t)y;
		uint32_t t = ((uint32_t)x & 0x3C0) | 0x2800;
		t = (uint32_t)(uint16_t)((int16_t)t >> 6);
		t |= (uint32_t)(yy >> 4) & 0x10;
		t |= ((uint32_t)yy & 0x200) << 2;
		c->tpage = (uint16_t)t;
		c->clut = (uint16_t)((uint32_t)((cx >> 4) & 0x3F) | shl32(cy, 6));
		c->proj_w = (int16_t)w;
		c->u_off = (int16_t)(((uint32_t)x & 0x3F) << 1);
		c->v_off = (int16_t)(y & 0xFF);
		c->proj_h = (int16_t)h;
	}

	static void UpdateMatrices(OverlayCtx *c, const uint8_t *entity)
	{
		ff8fx::copy<(0x20)>(&c->world, entity + 0x40);
		ff8fx::copy<(0x20)>(&c->world2, entity + 0x40);
		if (c->mode == 0)
		{
			int16_t out[4];
			MatrixMulVector(&c->world2, c->offset, out);
			c->world2.t[0] = (int32_t)((uint32_t)c->world2.t[0] + (uint32_t)(int32_t)out[0]);
			c->world2.t[1] = (int32_t)((uint32_t)c->world2.t[1] + (uint32_t)(int32_t)out[1]);
			c->world2.t[2] = (int32_t)((uint32_t)c->world2.t[2] + (uint32_t)(int32_t)out[2]);
		}
		else if (c->mode == 1)
		{
			c->world2.t[0] = (int32_t)((uint32_t)c->world2.t[0] + (uint32_t)(int32_t)c->offset[0]);
			c->world2.t[1] = (int32_t)((uint32_t)c->world2.t[1] + (uint32_t)(int32_t)c->offset[1]);
			c->world2.t[2] = (int32_t)((uint32_t)c->world2.t[2] + (uint32_t)(int32_t)c->offset[2]);
		}
	}

	static void BuildProjectorMatrix(OverlayCtx *c)
	{
		uint8_t *s = (uint8_t *)FieldAlloc(0x70);
		const int16_t x = (int16_t)c->world2.t[0];
		const int16_t y = (int16_t)c->world2.t[1];
		const int16_t z = (int16_t)c->world2.t[2];
		F<int16_t>(s, 0x68) = x;
		const int32_t dx = (int32_t)x - c->light_pos[0];
		F<int16_t>(s, 0x6A) = y;
		const int32_t dy = (int32_t)y - c->light_pos[1];
		F<int16_t>(s, 0x6C) = z;
		const int32_t dz = (int32_t)z - c->light_pos[2];
		F<int32_t>(s, 4) = dy;
		F<int32_t>(s, 8) = dz;
		F<int32_t>(s, 0) = dx;
		const int32_t d2 = (int32_t)((uint32_t)mul32(dz, dz) + (uint32_t)mul32(dy, dy) + (uint32_t)mul32(dx, dx));
		F<int32_t>(s, 0x60) = d2;
		F<int32_t>(s, 0x64) = Sqrt(d2);
		F<int16_t>(s, 0x12) = 0x1000;
		F<int16_t>(s, 0x10) = 0;
		F<int16_t>(s, 0x14) = 0;
		NormalizeVec3((const int32_t *)s, (int16_t *)(s + 0x18));
		BuildMatrixFromDirAndUp(s + 0x20, s + 0x18, s + 0x10);
		UnpackRotationMatrix(s + 0x20, s + 0x40);
		F<int32_t>(s, 0x5C) = c->light_dist;
		ff8fx::copy<(0x20)>(s + 0x20, &c->world);
		F<int32_t>(s, 0x54) = 0;
		F<int32_t>(s, 0x58) = 0;
		F<int32_t>(s, 0x38) = (int32_t)((uint32_t)c->world.t[1] - (uint32_t)c->world2.t[1]);
		F<int32_t>(s, 0x34) = (int32_t)((uint32_t)c->world.t[0] - (uint32_t)c->world2.t[0]);
		F<int32_t>(s, 0x3C) = (int32_t)((uint32_t)c->world.t[2] - (uint32_t)c->world2.t[2]);
		GteSetRotMatrixCtrl((const Mat4x3 *)(s + 0x40));
		uint8_t *light = (uint8_t *)&c->light;
		for (int k = 0; k < 3; k++)
		{
			GteLoadIRFromColumn(s + 0x20 + 2 * k);
			GteMVMVA_RotIR();
			GteStoreIRToColumn(light + 2 * k);
		}
		GteSetTransVectorCtrl((const Mat4x3 *)(s + 0x40));
		GteLoadV0Dwords(s + 0x34);
		GteMVMVA_RotV0Tr();
		GteReadMAC123(c->light.t);
		FieldFree(0x70);
	}

	static bool ProjectedUV(uint8_t *s, int count)
	{
		for (int v = 0; v < count; v++)
		{
			const int16_t x = PsxPixels(F<int16_t>(s, 0x40 + 4 * v));
			if (x < F<int16_t>(s, 0xC0) || x > F<int16_t>(s, 0xC2)) return false;
			const int16_t y = PsxPixels(F<int16_t>(s, 0x42 + 4 * v));
			if (y < F<int16_t>(s, 0xC4) || y > F<int16_t>(s, 0xC6)) return false;
			F<int16_t>(s, 0x40 + 4 * v) = (int16_t)(F<int16_t>(s, 0xC8) + x);
			F<int16_t>(s, 0x42 + 4 * v) = (int16_t)(y + F<int16_t>(s, 0xCA));
		}
		return true;
	}

	static void ProjectorPass(uint8_t *s, const uint8_t *verts)
	{
		GteSetRotMatrixCtrl((const Mat4x3 *)(s + 0x20));
		GteSetTransVectorCtrl((const Mat4x3 *)(s + 0x20));
		GteLoadV012(verts + F<uint32_t>(s, 0x94) * 16 + 4, verts + F<uint32_t>(s, 0x98) * 16 + 4, verts + F<uint32_t>(s, 0x9C) * 16 + 4);
		GteRTPT();
		F<int16_t>(s, 0xCC) = 0;
		GteNCLIP();
		GteReadMAC0(s + 0x8C);
		if (F<int16_t>(s, 0xCE) == 1 && F<int32_t>(s, 0x8C) <= 0) F<int16_t>(s, 0xCC) = 1;
	}

	static void RenderModel(const uint8_t *model, uint32_t ot, int32_t mode, OverlayCtx *c)
	{
		(void)mode;
		uint32_t fcur = *c->frame_cursor;
		uint32_t pcur = *c->packets;
		uint8_t *verts = *(uint8_t **)(c->sprite + 4);
		const uint8_t *bones = *(const uint8_t *const *)model + 0x10;
		const uint8_t *table = *(const uint8_t *const *)(model + 4);
		const int32_t count = *(const int32_t *)table;
		const uint8_t *entity = c->entity;
		uint8_t *s = (uint8_t *)FieldAlloc(0xD0);
		F<int16_t>(s, 0xCE) = c->cull;
		F<uint32_t>(s, 0xB8) = *(const uint32_t *)(entity + 0x7C);
		F<uint16_t>(s, 0xBC) = c->tpage;
		F<uint16_t>(s, 0xBE) = c->clut;
		const int32_t hw = (int32_t)c->proj_w / 2;
		const int32_t hh = (int32_t)c->proj_h / 2;
		F<int16_t>(s, 0xC2) = (int16_t)(hw + 0x9F);
		F<int16_t>(s, 0xC0) = (int16_t)(0xA0 - hw);
		F<int16_t>(s, 0xC6) = (int16_t)(hh + 0x77);
		F<int16_t>(s, 0xC4) = (int16_t)(0x78 - hh);
		F<int16_t>(s, 0xCA) = (int16_t)(c->v_off + (int16_t)hh - 0x78);
		F<int16_t>(s, 0xC8) = (int16_t)(c->u_off + (int16_t)hw - 0xA0);
		const uint32_t rgb = *(const uint32_t *)c->rgb & 0xFFFFFF;
		F<uint32_t>(s, 0xAC) = rgb | 0x3E000000;
		F<uint32_t>(s, 0xA8) = rgb | 0x36000000;
		const uint32_t col = *(const uint32_t *)(entity + 0x28) & 0xFFFFFF;
		F<uint32_t>(s, 0xB0) = col | 0x24000000;
		F<uint32_t>(s, 0xB4) = col | 0x2C000000;
		ff8fx::copy<(0x20)>(s, &c->view);
		ff8fx::copy<(0x20)>(s + 0x20, &c->light);
		F<int16_t>(s, 0xA4) = c->ce;
		F<int16_t>(s, 0xA6) = c->cc;
		GteSetRotMatrixCtrl((const Mat4x3 *)s);
		GteSetTransVectorCtrl((const Mat4x3 *)s);
		const uint32_t *offs = (const uint32_t *)(table + 4);
		for (int32_t k = 0; k < count; k++)
		{
			uint8_t *vcur = verts;
			const uint8_t *p = table + offs[k];
			if (!((F<uint32_t>(s, 0xB8) >> (k & 31)) & 1)) continue;
			const int32_t groups = *(const int16_t *)p;
			p += 2;
			for (int32_t g = groups; g > 0; g--)
			{
				const int32_t bone = *(const int16_t *)p;
				p += 2;
				const uint8_t *bm = bones + (uint32_t)mul32(bone, 0x30) + 0x10;
				GteSetLightMatrix(bm);
				GteSetBackground(F<int32_t>(bm, 0x14), F<int32_t>(bm, 0x18), F<int32_t>(bm, 0x1C));
				const int32_t nv = *(const int16_t *)p;
				p += 2;
				if (nv <= 0) continue;
				uint8_t *out = vcur + 4;
				vcur += (uint32_t)nv * 16;
				for (int32_t v = nv; v > 0; v--)
				{
					F<int16_t>(s, 0x70) = *(const int16_t *)p;
					F<int16_t>(s, 0x72) = *(const int16_t *)(p + 2);
					F<int16_t>(s, 0x74) = *(const int16_t *)(p + 4);
					p += 6;
					GteLoadV0(s + 0x70);
					GteMVMVA_LightV0Bk();
					GteStoreIR123(out);
					out += 0x10;
				}
			}
			p = (const uint8_t *)(((uint32_t)p + 3) & ~3u);
			const int32_t tris = *(const int16_t *)p;
			const int32_t quads = *(const int16_t *)(p + 2);
			const uint8_t *poly = p + 0xC;
			uint8_t *gp = (uint8_t *)pcur;
			uint8_t *fp = (uint8_t *)fcur;
			for (int32_t t = tris; t > 0; t--, poly += 0x10)
			{
				uint8_t *light = nullptr;
				const uint32_t i2 = *(const uint16_t *)(poly + 4) & 0xFFF;
				const uint32_t i1 = *(const uint16_t *)(poly + 2) & 0xFFF;
				const uint32_t i0 = *(const uint16_t *)poly & 0xFFF;
				F<uint32_t>(s, 0x9C) = i2;
				F<uint32_t>(s, 0x98) = i1;
				F<uint32_t>(s, 0x94) = i0;
				GteLoadV012(verts + i0 * 16 + 4, verts + i1 * 16 + 4, verts + i2 * 16 + 4);
				GteRTPT();
				const uint32_t z1 = GteSZ(1), z2 = GteSZ(2), z3 = GteSZ(3);
				GteNCLIP();
				GteReadMAC0(s + 0x8C);
				if (F<int32_t>(s, 0x8C) <= 0) continue;
				GteAVSZ3();
				GteReadOTZWord(s + 0x90);
				F<uint32_t>(s, 0x90) >>= 2;
				GteStoreSXY012_GT3(gp);
				GteStoreSXY012_FT3(fp);
				ProjectorPass(s, verts);
				if (F<int16_t>(s, 0xCC) == 0)
				{
					GteReadSXY012(s + 0x40);
					if (!ProjectedUV(s, 3)) F<int16_t>(s, 0xCC) = 1;
					if (F<int16_t>(s, 0xCC) == 0)
					{
						gp[0xC] = s[0x40];
						gp[0xD] = s[0x42];
						gp[0x18] = s[0x44];
						gp[0x19] = s[0x46];
						gp[0x24] = s[0x48];
						gp[0x25] = s[0x4A];
						F<uint16_t>(gp, 0x1A) = F<uint16_t>(s, 0xBC);
						F<uint16_t>(gp, 0xE) = F<uint16_t>(s, 0xBE);
						F<uint32_t>(gp, 0x1C) = F<uint32_t>(s, 0xA8);
						F<uint32_t>(gp, 0x10) = F<uint32_t>(s, 0xA8);
						F<uint32_t>(gp, 4) = F<uint32_t>(s, 0xA8);
						F<uint32_t>(gp, 0) = 0x9000000;
						light = gp;
						gp += 0x28;
					}
				}
				F<uint32_t>(fp, 0xC) = *(const uint32_t *)(poly + 8);
				F<uint32_t>(fp, 0x14) = *(const uint32_t *)(poly + 0xC);
				F<uint16_t>(fp, 0x1C) = *(const uint16_t *)(poly + 6);
				F<uint32_t>(fp, 4) = F<uint32_t>(s, 0xB0);
				F<uint32_t>(fp, 0) = 0x7000000;
				if (poly[0xF] & 2) fp[7] |= 2;
				InsertPrimDepthKeys(ot + F<uint32_t>(s, 0x90) * 4, fp, z1, z2, z3, 0);
				if (light) InsertPrimDepthKeys(ot + F<uint32_t>(s, 0x90) * 4, light, z1, z2, z3, 0);
				fp += 0x20;
				GteSetRotMatrixCtrl((const Mat4x3 *)s);
				GteSetTransVectorCtrl((const Mat4x3 *)s);
			}
			for (int32_t q = quads; q > 0; q--, poly += 0x14)
			{
				uint8_t *light = nullptr;
				const uint32_t i2 = *(const uint16_t *)(poly + 4) & 0xFFF;
				const uint32_t i1 = *(const uint16_t *)(poly + 2) & 0xFFF;
				const uint32_t i0 = *(const uint16_t *)poly & 0xFFF;
				F<uint32_t>(s, 0x9C) = i2;
				F<uint32_t>(s, 0x98) = i1;
				F<uint32_t>(s, 0x94) = i0;
				GteLoadV012(verts + i0 * 16 + 4, verts + i1 * 16 + 4, verts + i2 * 16 + 4);
				GteRTPT();
				const uint32_t z1 = GteSZ(1), z2 = GteSZ(2), z3 = GteSZ(3);
				GteNCLIP();
				GteReadMAC0(s + 0x8C);
				if (F<int32_t>(s, 0x8C) <= 0) continue;
				GteStoreSXY012_GT4(gp);
				GteStoreSXY012_FT4(fp);
				const uint32_t i3 = *(const uint16_t *)(poly + 6) & 0xFFF;
				F<uint32_t>(s, 0xA0) = i3;
				GteLoadV0(verts + i3 * 16 + 4);
				GteRTPS();
				const uint32_t z4 = GteSZ(3);
				GteReadSXY2(gp + 0x2C);
				GteReadSXY2(fp + 0x20);
				GteAVSZ4();
				GteReadOTZWord(s + 0x90);
				F<uint32_t>(s, 0x90) >>= 2;
				ProjectorPass(s, verts);
				if (F<int16_t>(s, 0xCC) == 0)
				{
					GteReadSXY012(s + 0x40);
					GteLoadV0(verts + F<uint32_t>(s, 0xA0) * 16 + 4);
					GteRTPS();
					GteReadSXY2(s + 0x4C);
					if (!ProjectedUV(s, 4)) F<int16_t>(s, 0xCC) = 1;
					if (F<int16_t>(s, 0xCC) == 0)
					{
						gp[0xD] = s[0x42];
						gp[0xC] = s[0x40];
						gp[0x18] = s[0x44];
						gp[0x24] = s[0x48];
						gp[0x19] = s[0x46];
						gp[0x25] = s[0x4A];
						gp[0x31] = s[0x4E];
						gp[0x30] = s[0x4C];
						F<uint16_t>(gp, 0x1A) = F<uint16_t>(s, 0xBC);
						F<uint32_t>(gp, 0x28) = F<uint32_t>(s, 0xAC);
						F<uint32_t>(gp, 0x1C) = F<uint32_t>(s, 0xAC);
						F<uint32_t>(gp, 0x10) = F<uint32_t>(s, 0xAC);
						F<uint32_t>(gp, 4) = F<uint32_t>(s, 0xAC);
						F<uint16_t>(gp, 0xE) = F<uint16_t>(s, 0xBE);
						F<uint32_t>(gp, 0) = 0xC000000;
						light = gp;
						gp += 0x34;
					}
				}
				F<uint32_t>(fp, 0xC) = *(const uint32_t *)(poly + 8);
				F<uint32_t>(fp, 0x14) = *(const uint32_t *)(poly + 0xC);
				F<uint16_t>(fp, 0x1C) = *(const uint16_t *)(poly + 0x10);
				F<uint16_t>(fp, 0x24) = *(const uint16_t *)(poly + 0x12);
				F<uint32_t>(fp, 4) = F<uint32_t>(s, 0xB4);
				F<uint32_t>(fp, 0) = 0x9000000;
				if (poly[0xF] & 2) fp[7] |= 2;
				InsertPrimDepthKeys(ot + F<uint32_t>(s, 0x90) * 4, fp, z1, z2, z3, z4);
				if (light) InsertPrimDepthKeys(ot + F<uint32_t>(s, 0x90) * 4, light, z1, z2, z3, z4);
				fp += 0x28;
				GteSetRotMatrixCtrl((const Mat4x3 *)s);
				GteSetTransVectorCtrl((const Mat4x3 *)s);
			}
			pcur = (uint32_t)gp;
			fcur = (uint32_t)fp;
		}
		*c->frame_cursor = fcur;
		*c->packets = pcur;
		FieldFree(0xD0);
	}

	static void DrawTargetModelWithOverlay(OverlayCtx *c)
	{
		uint8_t *entity = c->entity;
		UpdateMatrices(c, entity);
		if (c->slot != 0xFF) BuildBoneMatricesFromPose(entity + 0x60);
		else
		{
			const int32_t more = BattleReadAnimation(entity + 0x60, entity + 0x6C);
			if (more) PreBattleReadAnimation(entity + 0x60, entity + 0x6C, ((uint32_t)more & 0xFFFF0000u) | (uint16_t)c->anim);
		}
		ComposeAffineTransform(&Camera(), &c->world, &c->view);
		if (!(entity[0] & 0x20)) *c->frame_cursor = DrawShadow(entity, RenderOT(0x4040), 0x10, *c->frame_cursor);
		BuildProjectorMatrix(c);
		RenderModel(*(const uint8_t *const *)(entity + 0x64), RenderOT(0x44), 4, c);
		const uint8_t *weapon = *(const uint8_t *const *)(entity + 0x78);
		if (weapon) RenderModel(*(const uint8_t *const *)(weapon + 4), RenderOT(0x44), 4, c);
		if (c->slot != 0xFF) BuildBoneMatricesFromPose(entity + 0x60);
	}

	static void SetupTargetOverlay(OverlayCtx *c, uint32_t a00, uint8_t *sprite, uint32_t verts, uint8_t *entity, int32_t slot)
	{
		c->entity = entity;
		c->a00 = a00;
		c->slot = (int16_t)slot;
		c->sprite = sprite;
		c->frame_cursor = (uint32_t *)game_addr(game::BATTLE_PACKET_WRITE_CURSOR);
		c->packets = (uint32_t *)game_addr(game::MAG_050_POTION_PacketCursor);
		c->offset[0] = 0;
		c->offset[1] = 0;
		c->offset[2] = 0;
		c->rgb[0] = 0x80;
		c->rgb[1] = 0x80;
		c->rgb[2] = 0x80;
		c->light_dist = 0x2000;
		c->mode = 1;
		c->ce = 0;
		c->glow = 0;
		c->a4[0] = 0;
		c->a4[1] = 0;
		c->a4[2] = 0;
		c->ac[0] = 0;
		c->ac[1] = 0;
		c->ac[2] = 0;
		c->bc[0] = 0;
		c->bc[1] = 0;
		c->bc[2] = 0;
		sprite[0x1C] = 0x80;
		sprite[0x1D] = 0x80;
		sprite[0x1E] = 0x80;
		F<uint32_t>(sprite, 4) = verts;
		F<uint16_t>(sprite, 0x14) = 0;
		F<uint16_t>(sprite, 0x16) = 0;
		F<uint16_t>(sprite, 0x18) = 0x140;
		F<uint16_t>(sprite, 0x1A) = 0;
		F<uint32_t>(sprite, 0x20) = 0xFFFFFFFF;
		F<uint16_t>(sprite, 0x24) = 0;
	}

	static int32_t LifetimeExpired(Particle *p)
	{
		p->frame++;
		if (p->frame > p->last)
		{
			*(uint8_t *)&p->h.flags |= 4;
			p->frame = p->last;
			return 1;
		}
		return 0;
	}

	static void Integrate(Particle *p)
	{
		p->h.pos[0] = (int16_t)(p->h.pos[0] + p->vel[0]);
		p->h.pos[1] = (int16_t)(p->h.pos[1] + p->vel[1]);
		p->h.pos[2] = (int16_t)(p->h.pos[2] + p->vel[2]);
		if (LifetimeExpired(p))
		{
			*(uint8_t *)&p->h.flags |= 1;
			p->h.phase++;
		}
	}

	static void RingPlace(Particle *r, uint32_t step)
	{
		ff8fx::copy<(8)>(r->h.pos, r->center);
		r->h.pos[1] = (int16_t)(r->h.pos[1] + (int16_t)(r->vel[1] * (int16_t)step));
		const int32_t angle = (int32_t)((step << 7) & 0xFFF);
		r->angle = (int16_t)angle;
		r->h.pos[0] = (int16_t)(r->h.pos[0] + Div4096(mul32(ComputeSin((int16_t)angle), r->radius)));
		r->h.pos[2] = (int16_t)(r->h.pos[2] + Div4096(mul32(ComputeCos(r->angle), r->radius)));
	}

	static void SpawnSparkle(Particle *r)
	{
		if (r->h.counter > 0 && r->h.counter < 0x11) AddEffectTask(QParticles(), game_addr(ORIG_Sparkle), 0x6C, r);
	}

	static void SpawnMotes(Particle *r)
	{
		const int16_t c = r->h.counter;
		if (c <= 1 || c >= 0x7FFF) return;
		int32_t n;
		if (c > 10) n = 2;
		else if (c > 8) n = 3;
		else n = (c > 6) + 1;
		for (; n; n--)
		{
			const int32_t a = CrtRand() & 0xFFF;
			Particle *m = (Particle *)AddEffectTask(QParticles(), game_addr(ORIG_Mote), 0x6C, r);
			CopyAnchorXZFromSource(m, m->h.pos);
			m->h.pos[1] = r->h.pos[1];
			int32_t d = (int32_t)r->h.pos[1] - (int32_t)(int16_t)BoundsMaxWorldY(r);
			d = mul32(d, CrtRand() & 0x1F);
			int32_t q = (int32_t)(((int64_t)d * (int32_t)0xD5555555) >> 32) >> 3;
			q += (int32_t)((uint32_t)q >> 31);
			m->h.pos[1] = (int16_t)(m->h.pos[1] + q);
			const int32_t rad = (int32_t)(int16_t)BoundsRadius(r) * 3 / 2;
			m->radius = (int16_t)rad;
			if ((int16_t)rad > 0x400) m->radius = 0x400;
			m->h.pos[0] = (int16_t)(m->h.pos[0] + Div4096(mul32(ComputeSin((int16_t)a), m->radius)));
			m->h.pos[2] = (int16_t)(m->h.pos[2] + Div4096(mul32(ComputeCos((int16_t)a), m->radius)));
			ff8fx::copy<(8)>(m->vel, r->vel);
		}
	}

	static void SpawnGlints(Particle *r)
	{
		const int16_t c = r->h.counter;
		if (c <= 3 || c >= 0xB) return;
		int32_t n;
		if (c > 10) n = 2;
		else if (c > 8) n = 4;
		else n = (c > 6) + 2;
		for (; n; n--)
		{
			var<uint32_t>(game_addr(GLINT_CURSOR)) = game_addr(game::MAG_050_POTION_PacketCursor);
			uint8_t *g = (uint8_t *)AddEffectTask(QGlints(), game_addr(ORIG_Glint), 0x5C, r);
			F<int16_t>(g, 0x56) = 8;
			g[0x4C] = 0x10;
			g[0x4D] = 0x10;
			g[0x4E] = 0x10;
			g[0x50] = 0;
			g[0x51] = 0;
			g[0x52] = 0x10;
			F<int16_t>(g, 0x3C) = 0x100;
			F<int16_t>(g, 0x3E) = 0x10;
			F<int16_t>(g, 0x40) = 0x40;
			F<int16_t>(g, 0x42) = 0x10;
			if (CrtRand() & 1) F<int16_t>(g, 0x58) = 0x40;
			else F<int16_t>(g, 0x58) = (int16_t)0xFFC0;
		}
	}

	static void RingInit(Particle *r)
	{
		*(uint8_t *)&r->h.flags |= 2;
		r->seq = game_addr(SEQ_Ring);
		uint8_t *entity = Entity(r->h.slot);
		CopyAnchorXZFromSource(r, r->center);
		r->center[1] = (int16_t)BoundsMaxWorldY(r);
		const int32_t rad = shl32(BoundsRadius(r), 1);
		r->radius = (int16_t)rad;
		if ((int16_t)rad > 0x400) r->radius = 0x400;
		const int32_t hv = (int32_t)(int16_t)BoundsHeight(r) + 0x100;
		int32_t q = (int32_t)(((int64_t)hv * (int32_t)0x99999999) >> 32) >> 3;
		q += (int32_t)((uint32_t)q >> 31);
		r->vel[1] = (int16_t)((int32_t)(int16_t)q / 4);
		SetupTargetOverlay(&Overlay(), game_addr(OVERLAY_A00), (uint8_t *)game_addr(OVERLAY_SPRITE), game_addr(OVERLAY_VERTS), entity, r->h.slot);
		*(uint8_t *)&r->h.flags |= 8;
		entity[0] |= 4;
		r->h.phase++;
	}

	static void RingEnd(Particle *r)
	{
		if (!LifetimeExpired(r)) return;
		((RootNode *)r->h.root)->busy = 0;
		*(uint16_t *)Entity(r->h.slot) &= 0xFFFB;
		uint16_t f = r->h.flags;
		f = (uint16_t)((f & 0xFF00) | (uint8_t)(((uint8_t)f & 0xF5) | 1));
		r->h.flags = f;
		r->h.phase++;
	}
}
}

namespace ff8fx
{
namespace potion050
{
	static uint32_t __cdecl SparkleTask(TaskNode *n)
	{
		Particle *p = (Particle *)n;
		switch (p->h.phase)
		{
		case 0:
			p->seq = game_addr(SEQ_Sparkle);
			p->last = 8;
			p->vel[0] = (int16_t)((CrtRand() & 0x1F) - 0x10);
			p->vel[2] = (int16_t)((CrtRand() & 0x1F) - 0x10);
			p->vel[1] = (int16_t)((CrtRand() & 0xF) + 0x10);
			p->h.phase++;
			break;
		case 1: Integrate(p); break;
		default: break;
		}
		DrawSprite(p);
		p->h.counter++;
		if ((p->h.flags & 1) && p->h.children == 0)
		{
			ReleaseLinkedTask(p);
			return TASK_END;
		}
		return 0;
	}

	static uint32_t __cdecl MoteTask(TaskNode *n)
	{
		Particle *p = (Particle *)n;
		switch (p->h.phase)
		{
		case 0:
			p->seq = game_addr(SEQ_Mote);
			p->last = 0x10;
			p->vel[1] = (int16_t)(-8 - (CrtRand() & 0xF));
			p->vel[0] = 0;
			p->vel[2] = 0;
			p->h.phase++;
			break;
		case 1: Integrate(p); break;
		default: break;
		}
		DrawSprite(p);
		p->h.counter++;
		if ((p->h.flags & 1) && p->h.children == 0)
		{
			ReleaseLinkedTask(p);
			return TASK_END;
		}
		return 0;
	}

	static uint32_t __cdecl RingTask(TaskNode *n)
	{
		Particle *r = (Particle *)n;
		switch (r->h.phase)
		{
		case 0: RingInit(r); break;
		case 1:
			if (r->h.counter >= 0x14)
			{
				r->seq = game_addr(SEQ_RingEnd);
				r->last = 4;
				r->h.phase++;
			}
			break;
		case 2: RingEnd(r); break;
		default: break;
		}
		for (uint32_t s = 0; s < 4; s++)
		{
			RingPlace(r, (uint32_t)(uint16_t)(r->h.counter << 2) + s);
			if (r->h.flags & 2) SpawnSparkle(r);
		}
		if (r->h.flags & 2)
		{
			SpawnMotes(r);
			SpawnGlints(r);
		}
		if (r->h.flags & 8)
		{
			Overlay().offset[1] = r->h.pos[1];
			SetOverlayTexturePage(&Overlay(), r->h.pos, 0x200, 0x180, 0x140, 0xF1, 0x80, 0x80);
			DrawTargetModelWithOverlay(&Overlay());
		}
		if (r->h.counter < 0x14)
		{
			const uint32_t grey = (uint8_t)(0x3C - (uint8_t)(r->h.counter * 3));
			const uint32_t colour = grey | grey << 8 | grey << 16 | ((uint32_t)r & 0xFF000000u);
			DrawSprite(r);
			DrawAdditiveGlow(r, game_addr(SEQ_Glow), &colour);
		}
		else DrawSprite(r);
		r->h.counter++;
		if ((r->h.flags & 1) && r->h.children == 0)
		{
			ReleaseLinkedTask(r);
			return TASK_END;
		}
		return 0;
	}

	static uint32_t __cdecl EmitterTask(TaskNode *n)
	{
		EmitterNode *e = (EmitterNode *)n;
		EmitterUpdatePos(e);
		ComputeModelBounds(e);
		switch (e->h.phase)
		{
		case 0:
			AddEffectTask(QParticles(), game_addr(ORIG_Ring), 0x6C, e);
			e->h.phase++;
			break;
		case 1: e->h.phase++; break;
		case 2:
			if (e->h.counter >= 0xF)
			{
				ApplyActionResultToTarget(e->h.ctx->actions[e->h.action].targets + e->h.target * TARGET_STRIDE);
				*(uint8_t *)&e->h.flags |= 1;
				e->h.phase++;
			}
			break;
		default: break;
		}
		if (e->h.counter == 0) BdPlaySE(SOUND_Potion(), 0, 0x80);
		e->h.counter++;
		if ((e->h.flags & 1) && e->h.children == 0)
		{
			ReleaseLinkedTask(e);
			return TASK_END;
		}
		return 0;
	}

	static void RootPhase(RootNode *r)
	{
		switch (r->h.phase)
		{
		case 0:
		case 1:
			r->h.phase++;
			break;
		case 2:
			if (r->h.children == 0) r->h.phase++;
			break;
		case 3:
			r->busy = 1;
			AddEffectTask(QEmitters(), game_addr(ORIG_Emitter), 0x58, r);
			r->h.phase++;
			break;
		case 4:
			if (r->busy == 0)
			{
				const int8_t a = r->h.action;
				if ((int16_t)a < r->last_action)
				{
					r->h.action = (int8_t)(a + 1);
					r->h.line++;
					r->h.phase--;
				}
				else r->h.phase++;
			}
			break;
		case 5:
			r->h.phase++;
			break;
		case 6:
			if (r->alive == 0) r->h.phase++;
			break;
		case 7:
		case 8:
			r->h.phase++;
			break;
		case 9:
			*(uint8_t *)&r->h.flags |= 1;
			r->h.phase++;
			break;
		default: break;
		}
	}

	static uint32_t __cdecl RootTask(TaskNode *n)
	{
		RootNode *r = (RootNode *)n;
		ff8fx::copy<(0x20)>((void *)game_addr(CAM_SNAPSHOT), &Camera());
		var<uint32_t>(game_addr(game::MAG050_POTION_camMatrixPtr_a)) = game_addr(CAM_SNAPSHOT);
		var<uint32_t>(game_addr(game::MAG050_POTION_camMatrixPtr_b)) = game_addr(CAM_SNAPSHOT);
		if (r->tick & 1)
		{
			PacketCursor() = var<uint32_t>(game_addr(ARENA_A));
			PacketLimit() = var<uint32_t>(game_addr(LIMIT_A));
		}
		else
		{
			PacketCursor() = var<uint32_t>(game_addr(ARENA_B));
			PacketLimit() = var<uint32_t>(game_addr(LIMIT_B));
		}
		UpdateTargetPosFromBones(r);
		RootPhase(r);
		r->alive = 0;
		r->alive = (int16_t)(r->alive + ExecuteTaskQueue(QEmitters()));
		r->alive = (int16_t)(r->alive + ExecuteTaskQueue(QParticles()));
		r->alive = (int16_t)(r->alive + ExecuteTaskQueue(QGlints()));
		r->tick++;
		r->h.counter++;
		if ((r->h.flags & 1) && r->h.children == 0)
		{
			ReleaseLinkedTask(r);
			return TASK_END;
		}
		return 0;
	}
}

	void register_mag050_potion()
	{
		register_port(potion050::ORIG_Root, (void *)potion050::RootTask, "050 Root", 50);
		register_port(potion050::ORIG_Emitter, (void *)potion050::EmitterTask, "050 Emitter", 50);
		register_port(potion050::ORIG_Ring, (void *)potion050::RingTask, "050 Ring", 50);
		register_port(potion050::ORIG_Mote, (void *)potion050::MoteTask, "050 Mote", 50);
		register_port(potion050::ORIG_Sparkle, (void *)potion050::SparkleTask, "050 Sparkle", 50);
	}
}

