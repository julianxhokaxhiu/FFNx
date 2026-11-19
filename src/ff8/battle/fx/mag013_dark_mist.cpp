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

// Effect 13: Dark Mist / Poison Mist

#include "mag_common.h"

namespace ff8fx
{
namespace mist013
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
		uint8_t pad2E[2];
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
	struct ModelNode
	{
		FxNode h;
		int32_t scale[3];
		int32_t pad3C;
		uint8_t rgb[4];
		int16_t rot[3];
		int16_t pad4A;
		uint32_t model;
		int16_t fade;
		int16_t pad52;
		int16_t fade_a;
		int16_t fade_b;
		int16_t angle;
		int16_t length;
		int16_t grow;
		int16_t fade_step;
	};
	struct Particle
	{
		FxNode h;
		uint8_t pad30[0x18];
		uint8_t rgb[4];
		uint32_t seq;
		int16_t frame;
		int16_t last;
		int16_t shade;
		int16_t pad56;
		int16_t vel[3];
		int16_t pad5E;
		int16_t life;
		int16_t variant;
		int16_t base_y;
		int16_t bob;
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
		int16_t wobble_phase;
		int16_t wobble_amp;
		int16_t padD0;
		int16_t mode;
		uint8_t padD4[4];
		int16_t anim;
		int16_t slot;
		int16_t light_pos[3];
		uint8_t padE2[0x10];
		int16_t glow;
	};
#pragma pack(pop)
	static_assert(sizeof(FxNode) == 0x30 && sizeof(RootNode) == 0x64 && sizeof(EmitterNode) == 0x58, "Dark Mist nodes");
	static_assert(sizeof(ModelNode) == 0x60 && sizeof(Particle) == 0x68 && sizeof(OverlayCtx) == 0xF4, "Dark Mist nodes");

	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::g_Mag013_PacketCursor)); }
	inline uint32_t &ArenaA() { return var<uint32_t>(game_addr(game::MAG_013_ArenaBase1)); }
	inline uint32_t &ArenaB() { return var<uint32_t>(game_addr(game::MAG_013_ArenaBase0)); }
	inline TaskQueue *QEmitters() { return (TaskQueue *)game_addr(game::MAG_013_EmitterQueue); }
	inline TaskQueue *QControl() { return (TaskQueue *)game_addr(game::MAG_013_ControlQueue); }
	inline TaskQueue *QParticles() { return (TaskQueue *)game_addr(game::MAG_013_ParticleQueue); }
	inline TaskQueue *QModels() { return (TaskQueue *)game_addr(game::MAG_013_ModelQueue); }
	inline OverlayCtx &Overlay() { return var<OverlayCtx>(game_addr(game::g_Mag013_OverlayCtx)); }
	inline uint32_t RenderOT(uint32_t off) { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + off; }

	static constexpr game::Id ORIG_Root = game::MAG_013_DarkMist_RootTask;
	static constexpr game::Id ORIG_Emitter = game::MAG_013_DarkMist_EmitterTask;
	static constexpr game::Id ORIG_Swirl = game::MAG_013_DarkMist_SwirlTask;
	static constexpr game::Id ORIG_Control = game::MAG_013_DarkMist_MistControlTask;
	static constexpr game::Id ORIG_Cloud = game::MAG_013_DarkMist_CloudTask;
	static constexpr game::Id ORIG_MistA = game::MAG_013_DarkMist_MistATask;
	static constexpr game::Id ORIG_MistB = game::MAG_013_DarkMist_MistBTask;
	static constexpr game::Id ORIG_SpawnerC = game::MAG_013_DarkMist_PuffCSpawnerTask;
	static constexpr game::Id ORIG_PuffC = game::MAG_013_DarkMist_PuffCTask;
	static constexpr game::Id ORIG_SpawnerD = game::MAG_013_DarkMist_PuffDSpawnerTask;
	static constexpr game::Id ORIG_PuffD = game::MAG_013_DarkMist_PuffDTask;
	static constexpr game::Id ORIG_SpawnerE = game::MAG_013_DarkMist_StreamSpawnerTask;
	static constexpr game::Id ORIG_Stream = game::MAG_013_DarkMist_StreamTask;
	static constexpr game::Id ORIG_Overlay = game::MAG_013_DarkMist_OverlayTask;

	static const void *SOUND_Mist() { return (const void *)game_addr(game::unk_16309F8); }
	static constexpr game::Id MODEL_Swirl = game::unk_163257C, MODEL_Cloud = game::unk_1633C24;
	static constexpr game::Id RING_Table = game::Mag013_RingTable;
	static constexpr game::Id STREAM_Vectors = game::Mag013_StreamVectors;
	static constexpr game::Id SEQ_Mist = game::unk_16309FC, SEQ_MistB = game::unk_1630E94, SEQ_PuffC = game::unk_1630C78, SEQ_PuffD = game::unk_1630D84;
	static constexpr game::Id OVERLAY_A00 = game::unk_27437C0, OVERLAY_SPRITE = game::MAG_013_OverlaySprite, OVERLAY_VERTS = game::MAG_013_OverlayVertexBuffer;

	inline FxNode *AddEffectTask(TaskQueue *q, uint32_t task, int32_t size, void *parent) { return fn<FxNode *(__cdecl *)(TaskQueue *, uint32_t, int32_t, void *)>(game_addr(game::Effect_AddTaskAndInitFromCtx))(q, task, size, parent); }
	inline void ReleaseLinkedTask(void *n) { fn<void (__cdecl *)(void *)>(game_addr(game::Effect_ReleaseLinkedTask))(n); }
	inline void EmitterUpdatePos(void *n) { fn<void (__cdecl *)(void *)>(game_addr(game::MAG_001_CURE_Emitter_UpdatePos))(n); }
	inline void UpdateTargetPosFromBones(void *n) { fn<void (__cdecl *)(void *)>(game_addr(game::Effect_UpdateTargetPosFromBones))(n); }
	inline void ComputeModelBounds(void *n) { fn<void (__cdecl *)(void *)>(game_addr(game::MAG_001_CURE_Emitter_ComputeModelBounds))(n); }
	inline void CopyAnchor(void *n, void *out) { fn<void (__cdecl *)(void *, void *)>(game_addr(game::Effect_CopyAnchorXZFromSource))(n, out); }
	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t length, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, length, out); }
	inline void IdentityMatrix(Mat4x3 *m) { fn<void (__cdecl *)(Mat4x3 *)>(game_addr(game::Mtx_Identity))(m); }
	inline void RotateX(Mat4x3 *m, int32_t a) { fn<void (__cdecl *)(Mat4x3 *, int32_t)>(game_addr(game::Mtx_RotX))(m, a); }
	inline void RotateY(Mat4x3 *m, int32_t a) { fn<void (__cdecl *)(Mat4x3 *, int32_t)>(game_addr(game::Mtx_RotY))(m, a); }
	inline void RotateZ(Mat4x3 *m, int32_t a) { fn<void (__cdecl *)(Mat4x3 *, int32_t)>(game_addr(game::Mtx_RotZ))(m, a); }
	inline void MatrixMulVector(const Mat4x3 *m, const int16_t *v, int16_t *out) { fn<void (__cdecl *)(const Mat4x3 *, const int16_t *, int16_t *)>(game_addr(game::matrixMultiplyVector))(m, v, out); }
	inline int32_t ComputeCos(int32_t a) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(a); }
	inline int32_t Sqrt(int32_t v) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::Math_SqrtInt))(v); }
	inline void NormalizeVec3(const int32_t *in, int16_t *out) { fn<void (__cdecl *)(const int32_t *, int16_t *)>(game_addr(game::Math_NormalizeVec3_Q12))(in, out); }
	inline void BuildMatrixFromDirAndUp(void *out, const void *dir, const void *up) { fn<void (__cdecl *)(void *, const void *, const void *)>(game_addr(game::Effect_BuildMatrixFromDirAndUp))(out, dir, up); }
	inline void UnpackRotationMatrix(const void *in, void *out) { fn<void (__cdecl *)(const void *, void *)>(game_addr(game::UnpackRotationMatrix))(in, out); }
	inline void GteLoadV012(const void *a, const void *b, const void *c) { fn<void (__cdecl *)(const void *, const void *, const void *)>(game_addr(game::GTE_LoadV012))(a, b, c); }
	inline void GteLoadV0Dwords(const void *v) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadV0FromDwords))(v); }
	inline void GteRTPT() { fn<void (__cdecl *)()>(game_addr(game::GTE_RTPT))(); }
	inline void GteNCLIP() { fn<void (__cdecl *)()>(game_addr(game::GTE_NCLIP))(); }
	inline void GteAVSZ3() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ3))(); }
	inline void GteAVSZ4() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ4))(); }
	inline void GteReadMAC0(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadMAC0))(dst); }
	inline void GteReadOTZWord(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadOTZ))(dst); }
	inline void GteStoreSXY012_FT3(void *p) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreSXY012_PolyFT3))(p); }
	inline void GteStoreSXY012_FT4(void *p) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreSXY012_PolyFT3_2))(p); }
	inline void GteSetLightMatrix(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_SetLightMatrix))(m); }
	inline void GteSetBackground(int32_t x, int32_t y, int32_t z) { fn<void (__cdecl *)(int32_t, int32_t, int32_t)>(game_addr(game::GTE_SetBackgroundVector))(x, y, z); }
	inline void GteMVMVA_LightV0Bk() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_LightV0_Bk))(); }
	inline void GteMVMVA_RotIR() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotIR))(); }
	inline void GteLoadIRFromColumn(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadIRFromMatrixColumn))(m); }
	inline void GteStoreIRToColumn(void *m) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreIRToMatrixColumn))(m); }
	inline int32_t BattleReadAnimation(void *hdr, void *cmd) { return fn<int32_t (__cdecl *)(void *, void *)>(game_addr(game::Battle_ReadAnimation))(hdr, cmd); }
	inline void PreBattleReadAnimation(void *hdr, void *cmd, int32_t anim) { fn<void (__cdecl *)(void *, void *, int32_t)>(game_addr(game::pre_Battle_ReadAnimation))(hdr, cmd, anim); }
	inline uint32_t DrawShadow(uint8_t *entity, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(uint8_t *, uint32_t, int32_t, uint32_t)>(game_addr(game::BattleEntity_DrawShadow))(entity, ot, mode, cursor); }

	static uint8_t *CasterEntity(const FxNode *n) { return Entity(n->ctx->attacker); }

	static int32_t Div2p18(int32_t x) { return (x + ((x >> 31) & 0x3FFFF)) >> 18; }

	static uint32_t Finish(FxNode *n)
	{
		n->counter++;
		if ((n->flags & 1) && n->children == 0)
		{
			ReleaseLinkedTask(n);
			return TASK_END;
		}
		return 0;
	}

	static int AdvanceFrame(Particle *p)
	{
		p->frame++;
		if (p->frame > p->last)
		{
			p->h.flags |= 4;
			p->frame = p->last;
			return 1;
		}
		return 0;
	}

	static void LoopFrame(Particle *p)
	{
		p->frame++;
		if (p->frame > p->last) p->frame = 0;
	}

	static void ScrollV(uint32_t model)
	{
		const uint8_t *m = (const uint8_t *)model;
		int32_t vb = *(const int32_t *)m / 4;
		int32_t count = *(const int32_t *)(m + vb * 4 + 0x1C);
		if (count <= 0) return;
		uint32_t *w = (uint32_t *)(m + vb * 4 + 0x1C + 4 + 0x10);
		for (int32_t k = count; k != 0; k--)
		{
			uint32_t d0 = w[-1], d1 = w[0], d2 = w[1];
			uint32_t v0 = (d0 & 0xFF00) - 0x400;
			uint32_t v1 = (d1 & 0xFF00) - 0x400;
			uint32_t v2 = (d2 & 0xFF00) - 0x400;
			uint32_t v3 = ((d2 >> 24) << 8) - 0x400;
			if (v0 > 0xFF00 || v1 > 0xFF00 || v2 > 0xFF00 || v3 > 0xFF00)
			{
				v0 += 0x4000;
				v1 += 0x4000;
				v2 += 0x4000;
				v3 += 0x4000;
			}
			w[0] = (d1 & 0xFFFF00FF) | (v1 & 0x7F00);
			w[-1] = (d0 & 0xFFFF00FF) | (v0 & 0x7F00);
			w[1] = (d2 & 0x00FF00FF) | (v2 & 0x7F00) | ((v3 & 0x7F00) << 16);
			w += 9;
		}
	}

	static void WarpRing(uint32_t ring, uint32_t model, int32_t angle)
	{
		int32_t count = *(const int32_t *)(ring + 4);
		const int16_t *src = (const int16_t *)(ring + 8);
		int16_t *dst = (int16_t *)(model + 8);
		for (int32_t k = count; k > 0; k--)
		{
			int32_t a = (int32_t)((uint32_t)(uint16_t)(src[2] + (int16_t)angle) & 0xFFF);
			int32_t c = ComputeCos(a);
			int32_t x = src[0];
			if (x < 0) x = -x;
			dst[0] = (int16_t)(mul32(x, c) / 4096 + src[0]);
			int32_t s = ComputeSin(a);
			int32_t y = src[1];
			if (y < 0) y = -y;
			dst[1] = (int16_t)(mul32(y, s) / 4096 + src[1]);
			src += 4;
			dst += 4;
		}
	}

	static void DrawModel(ModelNode *n)
	{
		if (n->h.flags & 4) return;
		Mat4x3 m = {};
		IdentityMatrix(&m);
		RotateY(&m, n->rot[1]);
		RotateX(&m, n->rot[0]);
		RotateZ(&m, n->rot[2]);
		m.t[0] = n->h.pos[0];
		m.t[1] = n->h.pos[1];
		m.t[2] = n->h.pos[2];
		Scale3DMatrix(&m, n->scale);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		*(uint32_t *)h = n->model;
		*(uint32_t *)(h + 8) = *(const uint32_t *)n->rgb;
		*(int32_t *)(h + 0xC) = n->fade;
		*(uint32_t *)(h + 0x1C) = 0xF0;
		PacketCursor() = RenderPrimModel(h, RenderOT(0x44), 2, PacketCursor());
		FieldFree(0x58);
	}

	static void DrawSprite(Particle *p)
	{
		if (p->h.flags & 4) return;
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		TransformCameraByShadowRotation(p->h.pos, 0x1000, p->shade);
		*(uint32_t *)h = p->seq;
		*(int16_t *)(h + 4) = p->frame;
		*(uint16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(0x44), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static void DrawSpriteColoured(Particle *p)
	{
		if (p->h.flags & 4) return;
		TransformCameraByShadowRotation(p->h.pos, 0x1000, p->shade);
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		*(uint32_t *)h = p->seq;
		*(int16_t *)(h + 4) = p->frame;
		*(uint32_t *)(h + 0x1C) = *(const uint32_t *)p->rgb;
		*(uint16_t *)(h + 0x24) = 4;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(0x44), 2, PacketCursor());
		FieldFree(0xB4);
	}
}
}

namespace ff8fx
{
namespace mist013
{
	static void MistAInit(Particle *p)
	{
		p->seq = game_addr(SEQ_Mist);
		p->last = 0x17;
		p->shade = 0;
		CopyAnchor(p, p->vel);
		const int16_t x0 = p->h.pos[0], z0 = p->h.pos[2];
		const int32_t vx = (int16_t)(p->vel[0] - x0) / 12;
		p->vel[0] = (int16_t)vx;
		const int16_t y0 = p->h.pos[1];
		const int32_t r = CrtRand();
		const int32_t vy = (int16_t)((uint16_t)p->vel[1] - (uint32_t)(r & 0xFF) - (uint16_t)y0 - 0xC0) / 12;
		p->bob = 0;
		p->base_y = y0;
		const int32_t vz = (int16_t)(p->vel[2] - z0) / 12;
		p->vel[1] = (int16_t)vy;
		p->vel[2] = (int16_t)vz;
		if (p->variant != 0)
		{
			p->h.pos[0] = (int16_t)((int16_t)vx / 2 + x0);
			p->base_y = (int16_t)((int16_t)vy / 2 + y0);
			p->h.pos[2] = (int16_t)((int16_t)vz / 2 + z0);
			const int32_t s = ComputeSin(0);
			p->bob = (int16_t)((uint16_t)(p->bob + 0x155) & 0xFFF);
			p->h.pos[1] = (int16_t)(s / 16 + p->base_y);
		}
		p->h.phase++;
	}

	static void MistAMove(Particle *p)
	{
		p->h.pos[0] = (int16_t)(p->h.pos[0] + p->vel[0]);
		p->shade = (int16_t)(p->shade - 0x20);
		p->base_y = (int16_t)(p->base_y + p->vel[1]);
		p->h.pos[2] = (int16_t)(p->h.pos[2] + p->vel[2]);
		const int32_t s = ComputeSin(p->bob);
		p->bob = (int16_t)((uint16_t)(p->bob + 0x155) & 0xFFF);
		p->h.pos[1] = (int16_t)(s / 16 + p->base_y);
		AdvanceFrame(p);
		if (p->h.counter >= 0xC)
		{
			p->h.flags |= 1;
			p->h.phase++;
		}
	}

	static uint32_t __cdecl MistATask(TaskNode *n)
	{
		Particle *p = (Particle *)n;
		switch (p->h.phase)
		{
		case 0: MistAInit(p); break;
		case 1: MistAMove(p); break;
		default: break;
		}
		DrawSprite(p);
		return Finish(&p->h);
	}

	static void MistBInit(Particle *p)
	{
		p->seq = game_addr(SEQ_MistB);
		p->rgb[0] = 0x80;
		p->rgb[1] = 0x80;
		p->rgb[2] = 0x80;
		p->last = 1;
		p->shade = 0;
		CopyAnchor(p, p->vel);
		int32_t r = CrtRand();
		p->vel[1] = (int16_t)(p->vel[1] + (int16_t)(-0xC0 - (r & 0xFF)));
		r = CrtRand();
		p->vel[0] = (int16_t)(p->vel[0] + (int16_t)(0x100 - (r & 0x1FF)));
		r = CrtRand();
		const int16_t x0 = p->h.pos[0], z0 = p->h.pos[2];
		p->vel[1] = (int16_t)(p->vel[1] + (int16_t)(0x100 - (r & 0x1FF)));
		const int32_t vx = (int16_t)(p->vel[0] - x0) / 12;
		const int16_t y0 = p->h.pos[1];
		p->vel[0] = (int16_t)vx;
		const int32_t vy = (int16_t)(p->vel[1] - y0) / 12;
		p->vel[1] = (int16_t)vy;
		r = CrtRand();
		const int32_t vz = (int16_t)((uint16_t)p->vel[2] - (uint32_t)(r & 0x1FF) - (uint16_t)z0 + 0x100) / 12;
		p->bob = 0;
		p->base_y = y0;
		p->vel[2] = (int16_t)vz;
		if (p->variant != 0)
		{
			p->h.pos[0] = (int16_t)((int16_t)vx / 2 + x0);
			p->base_y = (int16_t)((int16_t)vy / 2 + y0);
			p->h.pos[2] = (int16_t)((int16_t)vz / 2 + z0);
			const int32_t s = ComputeSin(0);
			p->bob = (int16_t)((uint16_t)(p->bob + 0x15) & 0xFFF);
			p->h.pos[1] = (int16_t)(s / 16 + p->base_y);
		}
		p->h.phase++;
	}

	static void MistBMove(Particle *p)
	{
		p->h.pos[0] = (int16_t)(p->h.pos[0] + p->vel[0]);
		p->shade = (int16_t)(p->shade - 0x30);
		p->base_y = (int16_t)(p->base_y + p->vel[1]);
		p->h.pos[2] = (int16_t)(p->h.pos[2] + p->vel[2]);
		const int32_t s = ComputeSin(p->bob);
		p->bob = (int16_t)((uint16_t)(p->bob + 0x155) & 0xFFF);
		p->h.pos[1] = (int16_t)(s / 16 + p->base_y);
		LoopFrame(p);
		if (p->h.counter >= 4)
		{
			const uint8_t r = (uint8_t)(p->rgb[0] + 0xF0);
			p->rgb[1] = (uint8_t)(p->rgb[1] + 0xF0);
			p->rgb[0] = r;
			p->rgb[2] = (uint8_t)(p->rgb[2] + 0xF0);
			if (r == 0)
			{
				p->rgb[0] = 0x10;
				p->rgb[1] = 0x10;
				p->rgb[2] = 0x10;
			}
		}
		if (p->h.counter >= 0xC)
		{
			p->h.flags |= 1;
			p->h.phase++;
		}
	}

	static uint32_t __cdecl MistBTask(TaskNode *n)
	{
		Particle *p = (Particle *)n;
		switch (p->h.phase)
		{
		case 0: MistBInit(p); break;
		case 1: MistBMove(p); break;
		default: break;
		}
		DrawSpriteColoured(p);
		return Finish(&p->h);
	}

	static void PuffCInit(Particle *p)
	{
		p->seq = game_addr(SEQ_PuffC);
		p->last = 8;
		p->shade = 0;
		CopyAnchor(p, p->h.pos);
		p->h.pos[0] = (int16_t)(p->h.pos[0] + (int16_t)(CrtRand() % 0x300 - 0x180));
		p->h.pos[2] = (int16_t)(p->h.pos[2] + (int16_t)(CrtRand() % 0x300 - 0x180));
		p->h.pos[1] = (int16_t)(p->h.pos[1] + (int16_t)(CrtRand() % 0x300 - 0x380));
		p->h.phase++;
	}

	static void PuffDInit(Particle *p)
	{
		p->seq = game_addr(SEQ_PuffD);
		p->last = 9;
		p->shade = 0;
		CopyAnchor(p, p->h.pos);
		p->h.pos[0] = (int16_t)(p->h.pos[0] + (int16_t)(CrtRand() % 0x200 - 0x100));
		p->h.pos[2] = (int16_t)(p->h.pos[2] + (int16_t)(CrtRand() % 0x200 - 0x100));
		p->h.pos[1] = (int16_t)(p->h.pos[1] + (int16_t)(CrtRand() % 0x200 - 0x300));
		p->h.phase++;
	}

	static void PuffPlay(Particle *p)
	{
		if (AdvanceFrame(p))
		{
			p->h.flags |= 1;
			p->h.phase++;
		}
	}

	static uint32_t __cdecl PuffCTask(TaskNode *n)
	{
		Particle *p = (Particle *)n;
		switch (p->h.phase)
		{
		case 0: PuffCInit(p); break;
		case 1: PuffPlay(p); break;
		default: break;
		}
		DrawSprite(p);
		return Finish(&p->h);
	}

	static uint32_t __cdecl PuffDTask(TaskNode *n)
	{
		Particle *p = (Particle *)n;
		switch (p->h.phase)
		{
		case 0: PuffDInit(p); break;
		case 1: PuffPlay(p); break;
		default: break;
		}
		DrawSprite(p);
		return Finish(&p->h);
	}

	static uint32_t __cdecl SpawnerCTask(TaskNode *n)
	{
		FxNode *s = (FxNode *)n;
		switch (s->phase)
		{
		case 0:
			if (s->counter <= 0x10)
			{
				Particle *p = (Particle *)AddEffectTask(QParticles(), game_addr(ORIG_PuffC), 0x68, s);
				p->variant = s->counter;
			}
			else s->phase++;
			break;
		case 1:
			if (s->counter > 0x10)
			{
				s->flags |= 1;
				s->phase++;
			}
			break;
		default: break;
		}
		return Finish(s);
	}

	static uint32_t __cdecl SpawnerDTask(TaskNode *n)
	{
		FxNode *s = (FxNode *)n;
		switch (s->phase)
		{
		case 0:
			if (s->counter <= 8)
			{
				AddEffectTask(QParticles(), game_addr(ORIG_PuffD), 0x68, s);
				Particle *p = (Particle *)AddEffectTask(QParticles(), game_addr(ORIG_PuffD), 0x68, s);
				p->variant = s->counter;
			}
			else s->phase++;
			break;
		case 1:
			if (s->counter > 0x10)
			{
				s->flags |= 1;
				s->phase++;
			}
			break;
		default: break;
		}
		return Finish(s);
	}

	static void StreamInit(Particle *p)
	{
		const int16_t life = (int16_t)(0x14 - p->variant);
		p->seq = game_addr(SEQ_Mist);
		p->life = life;
		p->frame = (int16_t)(0x17 - life);
		p->last = 0x17;
		p->shade = (int16_t)0xFE00;
		CopyAnchor(p, p->h.pos);
		const int16_t *v = (const int16_t *)(game_addr(STREAM_Vectors) + (int32_t)p->variant * 8);
		const int32_t d = p->life;
		p->vel[0] = (int16_t)(v[0] / d);
		p->vel[1] = (int16_t)(v[1] / d);
		p->vel[2] = (int16_t)(v[2] / d);
		p->h.phase++;
	}

	static void StreamMove(Particle *p)
	{
		p->h.pos[0] = (int16_t)(p->h.pos[0] + p->vel[0]);
		p->h.pos[1] = (int16_t)(p->h.pos[1] + p->vel[1]);
		p->h.pos[2] = (int16_t)(p->h.pos[2] + p->vel[2]);
		AdvanceFrame(p);
		if (p->h.counter > p->life)
		{
			p->h.flags |= 1;
			p->h.phase++;
		}
	}

	static uint32_t __cdecl StreamTask(TaskNode *n)
	{
		Particle *p = (Particle *)n;
		switch (p->h.phase)
		{
		case 0: StreamInit(p); break;
		case 1: StreamMove(p); break;
		default: break;
		}
		DrawSprite(p);
		return Finish(&p->h);
	}

	static uint32_t __cdecl SpawnerETask(TaskNode *n)
	{
		FxNode *s = (FxNode *)n;
		GetEffectSpawnPosition(CasterEntity(s), 0xF0, 0x400, s->pos);
		switch (s->phase)
		{
		case 0:
			if (s->counter <= 9)
			{
				Particle *p = (Particle *)AddEffectTask(QParticles(), game_addr(ORIG_Stream), 0x68, s);
				p->variant = s->counter;
			}
			else
			{
				s->flags |= 1;
				s->phase++;
			}
			break;
		default: break;
		}
		return Finish(s);
	}

	static void CloudInit(ModelNode *c)
	{
		uint8_t *entity = CasterEntity(&c->h);
		c->rot[0] = 0x100;
		c->length = 0x400;
		c->fade_step = 0x2C0;
		c->grow = 0x800;
		c->model = game_addr(MODEL_Cloud);
		c->rot[1] = *(const int16_t *)(entity + 0xE);
		c->fade = 0;
		c->rgb[0] = 0;
		c->rgb[1] = 0;
		c->rgb[2] = 0;
		c->scale[0] = 0;
		c->scale[1] = 0;
		c->scale[2] = 0;
		GetEffectSpawnPosition(entity, 0xF0, 0x400, c->h.pos);
		c->h.phase++;
	}

	static void CloudFade(ModelNode *c)
	{
		const int16_t step = c->fade_step;
		c->fade = (int16_t)(c->fade + step);
		if (c->fade >= 0x1000)
		{
			c->h.flags |= 1;
			c->fade = 0x1000;
			c->h.phase++;
		}
		const int16_t s = (int16_t)(step - 0x40);
		c->fade_step = s;
		if (s < 0x40) c->fade_step = 0x40;
	}

	static void CloudUpdate(ModelNode *c, uint8_t *entity)
	{
		const int16_t g = c->grow;
		c->scale[0] += g;
		c->scale[1] += g;
		c->scale[2] += g;
		const int16_t g2 = (int16_t)(g - 0x80);
		c->grow = g2;
		if (g2 < 0) c->grow = 0;
		c->length = (int16_t)(c->length + 0x400);
		GetEffectSpawnPosition(entity, 0xF0, c->length, c->h.pos);
	}

	static uint32_t __cdecl CloudTask(TaskNode *n)
	{
		ModelNode *c = (ModelNode *)n;
		uint8_t *entity = CasterEntity(&c->h);
		switch (c->h.phase)
		{
		case 0: CloudInit(c); break;
		case 1: CloudFade(c); break;
		default: break;
		}
		CloudUpdate(c, entity);
		DrawModel(c);
		return Finish(&c->h);
	}

	static void SwirlPhase(ModelNode *w)
	{
		switch (w->h.phase)
		{
		case 0:
			w->fade_a = 0x1000;
			w->fade_b = 0x1000;
			w->scale[0] = 0x1000;
			w->scale[1] = 0x1000;
			w->scale[2] = 0x1000;
			w->rgb[0] = 0;
			w->rgb[1] = 0;
			w->rgb[2] = 0;
			w->model = game_addr(MODEL_Swirl);
			w->h.phase++;
			break;
		case 1:
			w->fade_a = (int16_t)(w->fade_a - 0x400);
			w->fade_b = (int16_t)(w->fade_b - 0x200);
			if (w->fade_a <= 0)
			{
				w->fade_a = 0;
				w->fade_b = 0x800;
				w->h.phase++;
			}
			break;
		case 2:
			if (w->h.counter >= 0xC) w->h.phase++;
			break;
		case 3:
			w->fade_a = (int16_t)(w->fade_a + 0x200);
			w->fade_b = (int16_t)(w->fade_b + 0x100);
			if (w->fade_a >= 0x1000)
			{
				w->h.flags |= 1;
				w->fade_a = 0x1000;
				w->fade_b = 0x1000;
				w->h.phase++;
			}
			break;
		default: break;
		}
	}

	static uint32_t __cdecl SwirlTask(TaskNode *n)
	{
		ModelNode *w = (ModelNode *)n;
		uint8_t *entity = CasterEntity(&w->h);
		w->rot[0] = 0x100;
		w->rot[1] = *(const int16_t *)(entity + 0xE);
		GetEffectSpawnPosition(entity, 0xF0, 0x400, w->h.pos);
		SwirlPhase(w);
		w->rot[2] = (int16_t)((uint16_t)(w->rot[2] - 0x20) & 0xFFF);
		ScrollV(w->model);
		w->angle = (int16_t)((uint16_t)(w->angle + 0x80) & 0xFFF);
		WarpRing(game_addr(RING_Table), w->model, w->angle);
		w->fade = w->fade_a;
		w->scale[0] = 0x1000;
		w->scale[1] = 0x1000;
		w->scale[2] = 0x1000;
		DrawModel(w);
		w->fade = w->fade_b;
		w->scale[0] = 0x2000;
		w->scale[1] = 0x2000;
		w->scale[2] = 0xC00;
		DrawModel(w);
		return Finish(&w->h);
	}

	static void InitOverlay(OverlayCtx *mc, uint32_t a00, uint8_t *sprite, uint32_t verts, uint8_t *entity, int32_t slot)
	{
		mc->entity = entity;
		mc->slot = (int16_t)slot;
		mc->a00 = a00;
		mc->sprite = sprite;
		mc->frame_cursor = (uint32_t *)game_addr(game::BATTLE_PACKET_WRITE_CURSOR);
		mc->packets = (uint32_t *)game_addr(game::g_Mag013_PacketCursor);
		mc->offset[0] = 0;
		mc->offset[1] = 0;
		mc->offset[2] = 0;
		mc->rgb[0] = 0x80;
		mc->rgb[1] = 0x80;
		mc->rgb[2] = 0x80;
		mc->light_dist = 0x2000;
		mc->mode = 1;
		mc->wobble_amp = 0;
		mc->glow = 0;
		mc->a4[0] = 0;
		mc->a4[1] = 0;
		mc->a4[2] = 0;
		mc->ac[0] = 0;
		mc->ac[1] = 0;
		mc->ac[2] = 0;
		mc->bc[0] = 0;
		mc->bc[1] = 0;
		mc->bc[2] = 0;
		sprite[0x1C] = 0x80;
		sprite[0x1D] = 0x80;
		sprite[0x1E] = 0x80;
		*(uint32_t *)(sprite + 4) = verts;
		*(uint16_t *)(sprite + 0x14) = 0;
		*(uint16_t *)(sprite + 0x16) = 0;
		*(uint16_t *)(sprite + 0x18) = 0x140;
		*(uint16_t *)(sprite + 0x1A) = 0;
		*(uint32_t *)(sprite + 0x20) = 0xFFFFFFFF;
		*(uint16_t *)(sprite + 0x24) = 0;
	}

	static void CopyWorld(OverlayCtx *mc, const uint8_t *entity)
	{
		ff8fx::copy<(0x20)>(&mc->world, entity + 0x40);
		ff8fx::copy<(0x20)>(&mc->world2, entity + 0x40);
		int16_t mode = mc->mode;
		if (mode == 0)
		{
			int16_t out[4];
			MatrixMulVector(&mc->world2, mc->offset, out);
			mc->world2.t[0] += out[0];
			mc->world2.t[1] += out[1];
			mc->world2.t[2] += out[2];
		}
		else if (mode == 1)
		{
			mc->world2.t[0] += mc->offset[0];
			mc->world2.t[1] += mc->offset[1];
			mc->world2.t[2] += mc->offset[2];
		}
	}

	static void BuildLightMatrix(OverlayCtx *mc)
	{
		uint8_t *s = (uint8_t *)FieldAlloc(0x70);
		int16_t x = (int16_t)mc->world2.t[0];
		int16_t y = (int16_t)mc->world2.t[1];
		int16_t z = (int16_t)mc->world2.t[2];
		*(int16_t *)(s + 0x68) = x;
		int32_t dx = x - mc->light_pos[0];
		*(int16_t *)(s + 0x6A) = y;
		int32_t dy = y - mc->light_pos[1];
		*(int16_t *)(s + 0x6C) = z;
		int32_t dz = z - mc->light_pos[2];
		*(int32_t *)(s + 4) = dy;
		*(int32_t *)(s + 8) = dz;
		*(int32_t *)s = dx;
		int32_t d2 = mul32(dz, dz) + mul32(dy, dy) + mul32(dx, dx);
		*(int32_t *)(s + 0x60) = d2;
		*(int32_t *)(s + 0x64) = Sqrt(d2);
		*(int16_t *)(s + 0x12) = 0x1000;
		*(int16_t *)(s + 0x10) = 0;
		*(int16_t *)(s + 0x14) = 0;
		NormalizeVec3((const int32_t *)s, (int16_t *)(s + 0x18));
		BuildMatrixFromDirAndUp(s + 0x20, s + 0x18, s + 0x10);
		UnpackRotationMatrix(s + 0x20, s + 0x40);
		*(int32_t *)(s + 0x5C) = mc->light_dist;
		ff8fx::copy<(0x20)>(s + 0x20, &mc->world);
		*(int32_t *)(s + 0x54) = 0;
		*(int32_t *)(s + 0x58) = 0;
		*(int32_t *)(s + 0x38) = mc->world.t[1] - mc->world2.t[1];
		*(int32_t *)(s + 0x34) = mc->world.t[0] - mc->world2.t[0];
		*(int32_t *)(s + 0x3C) = mc->world.t[2] - mc->world2.t[2];
		GteSetRotMatrixCtrl((const Mat4x3 *)(s + 0x40));
		uint8_t *light = (uint8_t *)&mc->light;
		for (int c = 0; c < 3; c++)
		{
			GteLoadIRFromColumn(s + 0x20 + 2 * c);
			GteMVMVA_RotIR();
			GteStoreIRToColumn(light + 2 * c);
		}
		GteSetTransVectorCtrl((const Mat4x3 *)(s + 0x40));
		GteLoadV0Dwords(s + 0x34);
		GteMVMVA_RotV0Tr();
		GteReadMAC123(mc->light.t);
		FieldFree(0x70);
	}

	static void DrawOverlayObject(const uint8_t *obj, uint32_t ot, int32_t mode, OverlayCtx *mc)
	{
		(void)mode;
		uint8_t *entity = mc->entity;
		uint32_t cursor = *mc->frame_cursor;
		uint8_t *verts = *(uint8_t **)(mc->sprite + 4);
		const uint8_t *bones = *(uint8_t *const *)obj + 0x10;
		const uint8_t *table = *(uint8_t *const *)(obj + 4);
		int32_t count = *(const int32_t *)table;
		uint8_t *s = (uint8_t *)FieldAlloc(0x74);
		const uint32_t *offs = (const uint32_t *)(table + 4);
		*(uint32_t *)(s + 0x70) = *(const uint32_t *)(entity + 0x7C);
		uint32_t col = *(const uint32_t *)(entity + 0x28) & 0xFFFFFF;
		*(uint32_t *)(s + 0x68) = col | 0x24000000;
		*(uint32_t *)(s + 0x6C) = col | 0x2C000000;
		*(int16_t *)(s + 0x64) = mc->wobble_amp;
		*(int16_t *)(s + 0x66) = mc->wobble_phase;
		GteSetRotMatrixCtrl(&mc->view);
		GteSetTransVectorCtrl(&mc->view);
		for (int32_t i = 0; i < count; i++)
		{
			const uint8_t *p = table + offs[i];
			if (!((*(const uint32_t *)(s + 0x70) >> (i & 31)) & 1)) continue;
			uint8_t *v = verts;
			int32_t groups = *(const int16_t *)p;
			p += 2;
			for (int32_t g = groups; g > 0; g--)
			{
				int32_t bone = *(const int16_t *)p;
				p += 2;
				const uint8_t *bm = bones + bone * 0x30 + 0x10;
				GteSetLightMatrix(bm);
				GteSetBackground(*(const int32_t *)(bm + 0x14), *(const int32_t *)(bm + 0x18), *(const int32_t *)(bm + 0x1C));
				int32_t nv = *(const int16_t *)p;
				p += 2;
				for (int32_t k = nv; k > 0; k--)
				{
					*(int16_t *)(s + 0x30) = *(const int16_t *)p;
					*(int16_t *)(s + 0x32) = *(const int16_t *)(p + 2);
					*(int16_t *)(s + 0x34) = *(const int16_t *)(p + 4);
					p += 6;
					GteLoadV0(s + 0x30);
					GteMVMVA_LightV0Bk();
					GteStoreIR123(s + 0x30);
					int32_t a = (int32_t)(((uint32_t)*(const uint16_t *)(s + 0x66) + 12u * *(const uint16_t *)(s + 0x32)) & 0xFFF);
					*(int32_t *)(s + 0x48) = a;
					*(int32_t *)(s + 0x40) = Div2p18(mul32(ComputeSin(a), *(const int16_t *)(s + 0x64)));
					*(int32_t *)(s + 0x44) = Div2p18(mul32(ComputeCos(*(const int32_t *)(s + 0x48)), *(const int16_t *)(s + 0x64)));
					*(int16_t *)(v + 6) = *(const int16_t *)(s + 0x32);
					*(int16_t *)(v + 4) = (int16_t)(*(const int16_t *)(s + 0x30) + (int16_t)*(const int32_t *)(s + 0x40));
					*(int16_t *)(v + 8) = (int16_t)(*(const int16_t *)(s + 0x34) + (int16_t)*(const int32_t *)(s + 0x44));
					v += 0x10;
				}
			}
			p = (const uint8_t *)(((uintptr_t)p + 3) & ~(uintptr_t)3);
			int32_t tris = *(const int16_t *)p;
			int32_t quads = *(const int16_t *)(p + 2);
			p += 0xC;
			uint8_t *pk = (uint8_t *)cursor;
			for (int32_t k = tris; k > 0; k--)
			{
				uint32_t i2 = *(const uint16_t *)(p + 4) & 0xFFF;
				uint32_t i1 = *(const uint16_t *)(p + 2) & 0xFFF;
				uint32_t i0 = *(const uint16_t *)p & 0xFFF;
				*(uint32_t *)(s + 0x5C) = i2;
				*(uint32_t *)(s + 0x58) = i1;
				*(uint32_t *)(s + 0x54) = i0;
				GteLoadV012(verts + i0 * 16 + 4, verts + i1 * 16 + 4, verts + i2 * 16 + 4);
				GteRTPT();
				GteNCLIP();
				GteReadMAC0(s + 0x4C);
				if (*(const int32_t *)(s + 0x4C) > 0)
				{
					GteStoreSXY012_FT3(pk);
					GteAVSZ3();
					GteReadOTZWord(s + 0x50);
					uint32_t z = *(const uint32_t *)(s + 0x50) >> 2;
					*(uint32_t *)(pk + 0xC) = *(const uint32_t *)(p + 8);
					*(uint32_t *)(s + 0x50) = z;
					*(uint32_t *)pk = 0x7000000;
					*(uint32_t *)(pk + 0x14) = *(const uint32_t *)(p + 0xC);
					*(uint16_t *)(pk + 0x1C) = *(const uint16_t *)(p + 6);
					*(uint32_t *)(pk + 4) = *(const uint32_t *)(s + 0x68);
					if (p[0xF] & 2) pk[7] |= 2;
					InsertPrimAutoDepth(ot + z * 4, pk);
					pk += 0x20;
				}
				p += 0x10;
			}
			for (int32_t k = quads; k > 0; k--)
			{
				uint32_t i2 = *(const uint16_t *)(p + 4) & 0xFFF;
				uint32_t i1 = *(const uint16_t *)(p + 2) & 0xFFF;
				uint32_t i0 = *(const uint16_t *)p & 0xFFF;
				*(uint32_t *)(s + 0x5C) = i2;
				*(uint32_t *)(s + 0x58) = i1;
				*(uint32_t *)(s + 0x54) = i0;
				GteLoadV012(verts + i0 * 16 + 4, verts + i1 * 16 + 4, verts + i2 * 16 + 4);
				GteRTPT();
				GteNCLIP();
				GteReadMAC0(s + 0x4C);
				if (*(const int32_t *)(s + 0x4C) > 0)
				{
					GteStoreSXY012_FT4(pk);
					uint32_t i3 = *(const uint16_t *)(p + 6) & 0xFFF;
					*(uint32_t *)(s + 0x60) = i3;
					GteLoadV0(verts + i3 * 16 + 4);
					GteRTPS();
					GteReadSXY2(pk + 0x20);
					GteAVSZ4();
					GteReadOTZWord(s + 0x50);
					uint32_t z = *(const uint32_t *)(s + 0x50) >> 2;
					*(uint32_t *)(pk + 0xC) = *(const uint32_t *)(p + 8);
					*(uint32_t *)(pk + 0x14) = *(const uint32_t *)(p + 0xC);
					*(uint32_t *)(s + 0x50) = z;
					*(uint32_t *)pk = 0x9000000;
					*(uint16_t *)(pk + 0x1C) = *(const uint16_t *)(p + 0x10);
					*(uint16_t *)(pk + 0x24) = *(const uint16_t *)(p + 0x12);
					*(uint32_t *)(pk + 4) = *(const uint32_t *)(s + 0x6C);
					if (p[0xF] & 2) pk[7] |= 2;
					InsertPrimAutoDepth(ot + z * 4, pk);
					pk += 0x28;
				}
				p += 0x14;
			}
			cursor = (uint32_t)pk;
		}
		*mc->frame_cursor = cursor;
		FieldFree(0x74);
	}

	static void DrawOverlay(OverlayCtx *mc)
	{
		uint8_t *entity = mc->entity;
		CopyWorld(mc, entity);
		if (mc->slot != 0xFF) BuildBoneMatricesFromPose(entity + 0x60);
		else if (BattleReadAnimation(entity + 0x60, entity + 0x6C)) PreBattleReadAnimation(entity + 0x60, entity + 0x6C, (uint16_t)mc->anim);
		ComposeAffineTransform(&Camera(), &mc->world, &mc->view);
		if (!(entity[0] & 0x20)) *mc->frame_cursor = DrawShadow(entity, RenderOT(0x4040), 0x10, *mc->frame_cursor);
		BuildLightMatrix(mc);
		DrawOverlayObject(*(const uint8_t *const *)(entity + 0x64), RenderOT(0x44), 4, mc);
		const uint8_t *weapon = *(const uint8_t *const *)(entity + 0x78);
		if (weapon) DrawOverlayObject(*(const uint8_t *const *)(weapon + 4), RenderOT(0x44), 4, mc);
		if (mc->slot != 0xFF) BuildBoneMatricesFromPose(entity + 0x60);
	}

	static uint32_t __cdecl OverlayTask(TaskNode *n)
	{
		FxNode *o = (FxNode *)n;
		OverlayCtx &mc = Overlay();
		switch (o->phase)
		{
		case 0:
		{
			uint8_t slot = o->slot;
			uint8_t *entity = Entity(slot);
			InitOverlay(&mc, game_addr(OVERLAY_A00), (uint8_t *)game_addr(OVERLAY_SPRITE), game_addr(OVERLAY_VERTS), entity, slot);
			o->flags |= 8;
			entity[0] |= 4;
			o->phase++;
			break;
		}
		case 1:
			mc.wobble_amp = (int16_t)(mc.wobble_amp + 0x200);
			if (mc.wobble_amp >= 0x1000)
			{
				mc.wobble_amp = 0x1000;
				o->phase++;
			}
			break;
		case 2:
			if (o->counter >= 0x1C) o->phase++;
			break;
		case 3:
		{
			int16_t a = (int16_t)(mc.wobble_amp - 0x100);
			mc.wobble_amp = a;
			if (a <= 0)
			{
				*(uint16_t *)Entity(o->slot) &= 0xFFFB;
				((RootNode *)o->root)->busy = 0;
				o->flags = (uint16_t)((o->flags & ~8u) | 1);
				o->phase++;
			}
			break;
		}
		default: break;
		}
		mc.wobble_phase = (int16_t)((uint16_t)(mc.wobble_phase + 0xA0) & 0xFFF);
		if (o->flags & 8)
		{
			DrawOverlay(&mc);
		}
		return Finish(o);
	}

	static uint32_t __cdecl ControlTask(TaskNode *n)
	{
		FxNode *c = (FxNode *)n;
		GetEffectSpawnPosition(CasterEntity(c), 0xF0, 0x400, c->pos);
		switch (c->phase)
		{
		case 0: c->phase++; break;
		case 1:
			if (c->counter >= 0x10)
			{
				AddEffectTask(QControl(), game_addr(ORIG_Overlay), 0x30, c);
				c->phase++;
			}
			break;
		case 2:
			if (c->counter > 0x1E)
			{
				c->flags |= 1;
				c->phase++;
			}
			break;
		default: break;
		}
		if (c->counter == 0) AddEffectTask(QModels(), game_addr(ORIG_Cloud), 0x60, c);
		if (c->counter <= 0xC)
		{
			Particle *p = (Particle *)AddEffectTask(QParticles(), game_addr(ORIG_MistA), 0x68, c);
			p->variant = 0;
			p = (Particle *)AddEffectTask(QParticles(), game_addr(ORIG_MistA), 0x68, c);
			p->variant = 1;
			p = (Particle *)AddEffectTask(QParticles(), game_addr(ORIG_MistB), 0x68, c);
			p->variant = 0;
			p = (Particle *)AddEffectTask(QParticles(), game_addr(ORIG_MistB), 0x68, c);
			p->variant = 1;
		}
		if (c->counter == 0x10 || c->counter == 0x17) AddEffectTask(QControl(), game_addr(ORIG_SpawnerE), 0x30, c);
		if (c->counter == 0x10) AddEffectTask(QControl(), game_addr(ORIG_SpawnerC), 0x30, c);
		if (c->counter == 0x1E) AddEffectTask(QControl(), game_addr(ORIG_SpawnerD), 0x30, c);
		return Finish(c);
	}

	static uint32_t __cdecl EmitterTask(TaskNode *n)
	{
		EmitterNode *e = (EmitterNode *)n;
		EmitterUpdatePos(e);
		ComputeModelBounds(e);
		switch (e->h.phase)
		{
		case 0:
			AddEffectTask(QModels(), game_addr(ORIG_Swirl), 0x60, e);
			e->h.phase++;
			break;
		case 1:
			if (e->h.counter >= 0x19)
			{
				AddEffectTask(QControl(), game_addr(ORIG_Control), 0x30, e);
				e->h.phase++;
			}
			break;
		case 2:
			if (e->h.counter >= 0x46)
			{
				ApplyActionResultToTarget(e->h.ctx->actions[e->h.action].targets);
				e->h.flags |= 1;
				e->h.phase++;
			}
			break;
		default: break;
		}
		if (e->h.counter == 0) BdPlaySE(SOUND_Mist(), 0, 0x80);
		return Finish(&e->h);
	}

	static void RootPhase(RootNode *r)
	{
		switch (r->h.phase)
		{
		case 0: r->h.phase++; break;
		case 1:
			r->busy = 1;
			AddEffectTask(QEmitters(), game_addr(ORIG_Emitter), 0x58, r);
			r->h.phase++;
			break;
		case 2:
			if (r->busy == 0)
			{
				int8_t a = r->h.action;
				if ((int16_t)a < r->last_action)
				{
					r->h.action = (int8_t)(a + 1);
					r->h.phase--;
				}
				else r->h.phase++;
			}
			break;
		case 3:
			if (r->alive == 0)
			{
				r->h.flags |= 1;
				r->h.phase++;
			}
			break;
		default: break;
		}
	}

	static uint32_t __cdecl RootTask(TaskNode *n)
	{
		RootNode *r = (RootNode *)n;
		if (r->tick & 1) PacketCursor() = ArenaA();
		else PacketCursor() = ArenaB();
		UpdateTargetPosFromBones(r);
		RootPhase(r);
		r->alive = (int16_t)ExecuteTaskQueue(QEmitters());
		r->alive = (int16_t)(r->alive + ExecuteTaskQueue(QControl()));
		r->alive = (int16_t)(r->alive + ExecuteTaskQueue(QParticles()));
		r->alive = (int16_t)(r->alive + ExecuteTaskQueue(QModels()));
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

	void register_mag013_dark_mist()
	{
		register_port(mist013::ORIG_Root, (void *)mist013::RootTask, "M013 RootTask", 13);
		register_port(mist013::ORIG_Emitter, (void *)mist013::EmitterTask, "M013 EmitterTask", 13);
		register_port(mist013::ORIG_Swirl, (void *)mist013::SwirlTask, "M013 SwirlTask", 13);
		register_port(mist013::ORIG_Control, (void *)mist013::ControlTask, "M013 ControlTask", 13);
		register_port(mist013::ORIG_Cloud, (void *)mist013::CloudTask, "M013 CloudTask", 13);
		register_port(mist013::ORIG_MistA, (void *)mist013::MistATask, "M013 MistATask", 13);
		register_port(mist013::ORIG_MistB, (void *)mist013::MistBTask, "M013 MistBTask", 13);
		register_port(mist013::ORIG_SpawnerC, (void *)mist013::SpawnerCTask, "M013 SpawnerCTask", 13);
		register_port(mist013::ORIG_PuffC, (void *)mist013::PuffCTask, "M013 PuffCTask", 13);
		register_port(mist013::ORIG_SpawnerD, (void *)mist013::SpawnerDTask, "M013 SpawnerDTask", 13);
		register_port(mist013::ORIG_PuffD, (void *)mist013::PuffDTask, "M013 PuffDTask", 13);
		register_port(mist013::ORIG_SpawnerE, (void *)mist013::SpawnerETask, "M013 SpawnerETask", 13);
		register_port(mist013::ORIG_Stream, (void *)mist013::StreamTask, "M013 StreamTask", 13);
		register_port(mist013::ORIG_Overlay, (void *)mist013::OverlayTask, "M013 OverlayTask", 13);
	}
}

