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

// Effect 22: Bio

#include "mag_common.h"

namespace ff8fx
{
namespace bio022
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
		Mat4x3 spin_m;
		int16_t center[4];
		int16_t acid_done;
		int16_t spin;
		int16_t radius;
		int16_t scale;
		int16_t big;
		int16_t pad8A;
	};
	struct WobbleNode
	{
		FxNode h;
		int16_t pad30;
		int16_t from;
		int16_t to;
		int16_t t;
	};
	struct Particle
	{
		FxNode h;
		int32_t scale[3];
		int32_t pad3C;
		int16_t rot[3];
		int16_t pad46;
		uint8_t pad48[4];
		uint32_t seq;
		int16_t frame;
		int16_t last;
		int16_t shade;
		int16_t pad56;
		int16_t vel[3];
		int16_t pad5E;
		int16_t acc[3];
		int16_t pad66;
		int16_t variant;
		int16_t angle;
		int16_t speed;
		int16_t pad6E;
	};
	struct MeshNode
	{
		FxNode h;
		int32_t scale[3];
		uint8_t pad3C[8];
		int16_t rot[3];
		int16_t pad4A;
		uint8_t *mesh;
		int16_t frame;
		int16_t pad52;
		int16_t offs[3];
		int16_t pad5A;
		int16_t offs_to[3];
		int16_t pad62;
		int16_t wob[3];
		int16_t pad6A;
		const uint8_t *src;
		const uint8_t *dst;
		int16_t key_a;
		int16_t key_b;
		int16_t spin;
		uint8_t pad7A[8];
		int16_t spin_d;
		int16_t t;
		uint8_t pad86[4];
		int16_t cycles;
		int16_t variant;
		int16_t step;
		int16_t steps;
		int16_t frac;
		int16_t lit;
		int16_t scale_d;
	};
	struct FlashNode
	{
		FxNode h;
		const uint32_t *script;
		uint32_t colour;
		int16_t idx;
		int16_t pad3A;
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
	static_assert(sizeof(FxNode) == 0x30 && sizeof(RootNode) == 0x64 && sizeof(EmitterNode) == 0x8C, "Bio nodes");
	static_assert(sizeof(WobbleNode) == 0x38 && sizeof(Particle) == 0x70 && sizeof(MeshNode) == 0x98, "Bio nodes");
	static_assert(sizeof(FlashNode) == 0x3C && sizeof(OverlayCtx) == 0xF4, "Bio nodes");

	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_022_PacketCursor)); }
	inline uint32_t &ArenaA() { return var<uint32_t>(game_addr(game::MAG_022_ArenaBase1)); }
	inline uint32_t &ArenaB() { return var<uint32_t>(game_addr(game::MAG_022_ArenaBase0)); }
	inline TaskQueue *QEmitters() { return (TaskQueue *)game_addr(game::MAG_022_EmitterQueue); }
	inline TaskQueue *QControl() { return (TaskQueue *)game_addr(game::MAG_022_WobbleQueue); }
	inline TaskQueue *QParticles() { return (TaskQueue *)game_addr(game::MAG_022_ParticleQueue); }
	inline TaskQueue *QMeshes() { return (TaskQueue *)game_addr(game::MAG_022_MeshQueue); }
	inline TaskQueue *QFlash() { return (TaskQueue *)game_addr(game::MAG_022_FlashQueue); }
	inline OverlayCtx &Overlay() { return var<OverlayCtx>(game_addr(game::MAG_022_OverlayCtx)); }
	inline uint32_t RenderOT(uint32_t off) { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + off; }

	static constexpr game::Id ORIG_Root = game::MAG_022_BIO_Tick;
	static constexpr game::Id ORIG_Emitter = game::MAG_022_EmitterTask;
	static constexpr game::Id ORIG_Pool = game::MAG_022_PoolTask;
	static constexpr game::Id ORIG_Wobble = game::MAG_022_WobbleTask;
	static constexpr game::Id ORIG_BubbleSpawner = game::MAG_022_BubbleSpawnerTask;
	static constexpr game::Id ORIG_Splash = game::MAG_022_SplashTask;
	static constexpr game::Id ORIG_Bubble = game::MAG_022_BubbleTask;
	static constexpr game::Id ORIG_Blob = game::MAG_022_BlobTask;
	static constexpr game::Id ORIG_Flash = game::MAG_022_Flash;
	static constexpr game::Id ORIG_Droplet = game::MAG_022_DropletTask;
	static constexpr game::Id ORIG_Palette = game::MAG_022_PaletteTask;
	static constexpr game::Id ORIG_MistSpawnerA = game::MAG_022_MistSpawnerATask;
	static constexpr game::Id ORIG_MistA = game::MAG_022_MistATask;
	static constexpr game::Id ORIG_MistSpawnerB = game::MAG_022_MistSpawnerBTask;
	static constexpr game::Id ORIG_MistB = game::MAG_022_MistBTask;
	static constexpr game::Id ORIG_Overlay = game::MAG_022_OverlayTask;

	static const void *SOUND_Bio() { return (const void *)game_addr(game::unk_16146E4); }
	static constexpr game::Id STAGE_WOBBLE = game::unk_1D98990;
	static constexpr game::Id PALETTE_TIM = game::unk_16146E8;
	static constexpr game::Id PALETTES = game::unk_1614F68;
	static constexpr game::Id POOL_KEYS = game::unk_16189CC;
	static constexpr game::Id BLOB_KEYS = game::unk_1626000;
	static constexpr game::Id MESH_Pool = game::unk_1615CE4, MESH_Splat = game::unk_1617E3C, MESH_Blob = game::unk_1619888, MESH_BlobBoil = game::unk_161F750;
	static constexpr game::Id KEY_BlobA = game::unk_16189E8, KEY_BlobB = game::unk_161E8B0;
	static constexpr game::Id SEQ_SplashA = game::unk_1614F74, SEQ_SplashB = game::unk_1615120;
	static constexpr game::Id SEQ_BubbleA = game::unk_16152CC, SEQ_BubbleB = game::unk_1615440;
	static constexpr game::Id SEQ_Droplet = game::unk_16156F8, SEQ_MistA = game::unk_1615858, SEQ_MistB = game::unk_16155B4;
	static constexpr game::Id FLASH_Script = game::unk_162693C;
	static constexpr game::Id COUNT_Bubbles = game::unk_162694C, COUNT_Splashes = game::unk_162696C, COUNT_MistA = game::unk_162698C, COUNT_MistB = game::unk_16269AC;
	static constexpr game::Id OVERLAY_A00 = game::unk_2705160, OVERLAY_SPRITE = game::unk_2714DE8, OVERLAY_VERTS = game::MAG_022_OverlayVertexBuffer;

	inline FxNode *AddEffectTask(TaskQueue *q, uint32_t task, int32_t size, void *parent) { return fn<FxNode *(__cdecl *)(TaskQueue *, uint32_t, int32_t, void *)>(game_addr(game::Effect_AddTaskAndInitFromCtx))(q, task, size, parent); }
	inline void ReleaseLinkedTask(void *n) { fn<void (__cdecl *)(void *)>(game_addr(game::Effect_ReleaseLinkedTask))(n); }
	inline void EmitterUpdatePos(void *n) { fn<void (__cdecl *)(void *)>(game_addr(game::MAG_001_CURE_Emitter_UpdatePos))(n); }
	inline void UpdateTargetPosFromBones(void *n) { fn<void (__cdecl *)(void *)>(game_addr(game::Effect_UpdateTargetPosFromBones))(n); }
	inline void ComputeModelBounds(void *n) { fn<void (__cdecl *)(void *)>(game_addr(game::MAG_001_CURE_Emitter_ComputeModelBounds))(n); }
	inline void CopyMidpointFromSource(void *n, int16_t *out) { fn<void (__cdecl *)(void *, int16_t *)>(game_addr(game::Effect_CopyMidpointFromSource))(n, out); }
	inline int32_t BoundsCenterY(void *n) { return fn<int32_t (__cdecl *)(void *)>(game_addr(game::Effect_GetTargetBoundsCenterWorldY))(n); }
	inline int32_t BoundsHalfHeight(void *n) { return fn<int32_t (__cdecl *)(void *)>(game_addr(game::MAG_022_BoundsHalfHeight))(n); }
	inline int32_t BoundsBaseY(void *n) { return fn<int32_t (__cdecl *)(void *)>(game_addr(game::sub_8DCAF0))(n); }
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
	inline void GteLoadV012Block(const void *v) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadV012Dwords))(v); }
	inline void GteLoadV012(const void *a, const void *b, const void *c) { fn<void (__cdecl *)(const void *, const void *, const void *)>(game_addr(game::GTE_LoadV012))(a, b, c); }
	inline void GteLoadV0Dwords(const void *v) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadV0FromDwords))(v); }
	inline void GteRTPT() { fn<void (__cdecl *)()>(game_addr(game::GTE_RTPT))(); }
	inline void GteNCLIP() { fn<void (__cdecl *)()>(game_addr(game::GTE_NCLIP))(); }
	inline void GteAVSZ3() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ3))(); }
	inline void GteAVSZ4() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ4))(); }
	inline void GteReadMAC0(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadMAC0))(dst); }
	inline void GteReadOTZWord(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadOTZ))(dst); }
	inline void GteStoreSXY012_GT3(void *p) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreSXY012_PolyGT3))(p); }
	inline void GteStoreSXY012_FT3(void *p) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreSXY012_PolyFT3))(p); }
	inline void GteStoreSXY012_FT4(void *p) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreSXY012_PolyFT3_2))(p); }
	inline void GteSetLightMatrix(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_SetLightMatrix))(m); }
	inline void GteSetBackground(int32_t x, int32_t y, int32_t z) { fn<void (__cdecl *)(int32_t, int32_t, int32_t)>(game_addr(game::GTE_SetBackgroundVector))(x, y, z); }
	inline void GteMVMVA_LightV0Bk() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_LightV0_Bk))(); }
	inline void GteMVMVA_RotV0Tr2() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0_Tr_2))(); }
	inline void GteMVMVA_RotIR() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotIR))(); }
	inline void GteLoadIRFromColumn(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadIRFromMatrixColumn))(m); }
	inline void GteStoreIRToColumn(void *m) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreIRToMatrixColumn))(m); }
	inline void GteLoadIRBytes(const void *rgb) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadIR123FromBytes))(rgb); }
	inline void GteStoreIRBytes(void *rgb) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreIR123LowBytes))(rgb); }
	inline void InsertPrimAltViewport(uint32_t bucket, void *p) { fn<void (__cdecl *)(uint32_t, void *)>(game_addr(game::SSIGPU_InsertPrimAltViewport))(bucket, p); }
	inline uint32_t GetTPage(int32_t tp, int32_t abr, int32_t x, int32_t y) { return fn<uint32_t (__cdecl *)(int32_t, int32_t, int32_t, int32_t)>(game_addr(game::sub_45C690))(tp, abr, x, y); }
	inline void SetDrawMode(void *p, int32_t dfe, int32_t dtd, uint32_t tpage, const void *tw) { fn<void (__cdecl *)(void *, int32_t, int32_t, uint32_t, const void *)>(game_addr(game::GPU_SetDrawMode))(p, dfe, dtd, tpage, tw); }
	inline void QueueTIMUpload(uint32_t tim) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::Battle_QueueTIMUpload_GetEOF))(tim); }
	inline int32_t BattleReadAnimation(void *hdr, void *cmd) { return fn<int32_t (__cdecl *)(void *, void *)>(game_addr(game::Battle_ReadAnimation))(hdr, cmd); }
	inline void PreBattleReadAnimation(void *hdr, void *cmd, int32_t anim) { fn<void (__cdecl *)(void *, void *, int32_t)>(game_addr(game::pre_Battle_ReadAnimation))(hdr, cmd, anim); }
	inline uint32_t DrawShadow(uint8_t *entity, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(uint8_t *, uint32_t, int32_t, uint32_t)>(game_addr(game::BattleEntity_DrawShadow))(entity, ot, mode, cursor); }

	static int16_t Div40960(int32_t x)
	{
		int32_t q = x / 4096;
		int32_t hi = (int32_t)(((int64_t)q * 0x66666667) >> 32) >> 2;
		hi += (int32_t)((uint32_t)hi >> 31);
		return (int16_t)hi;
	}

	static int16_t DivNeg12(int32_t x)
	{
		int32_t hi = (int32_t)(((int64_t)x * (int32_t)0xD5555555) >> 32) >> 1;
		hi += (int32_t)((uint32_t)hi >> 31);
		return (int16_t)hi;
	}

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
}
}

namespace ff8fx
{
namespace bio022
{
	static void Morph(const uint8_t *src, const uint8_t *dst, uint8_t *out, int32_t t)
	{
		const uint8_t *a = src + 8;
		int32_t count = *(const int32_t *)(src + 4);
		const uint8_t *b = dst + 8;
		uint8_t *o = out + 8;
		int32_t *s = (int32_t *)FieldAlloc(8);
		int32_t t16 = (int16_t)t;
		s[1] = t16;
		s[0] = 0x1000 - t16;
		for (int32_t k = count; k > 0; k--)
		{
			GteSetIR0(s[0]);
			GteLoadIR123(a);
			GteGPF();
			a += 8;
			GteSetIR0(s[1]);
			GteLoadIR123(b);
			GteGPL();
			b += 8;
			GteStoreIR123(o);
			o += 8;
		}
		FieldFree(8);
	}

	static const uint8_t *MeshPolys(const uint8_t *mesh, int32_t *count)
	{
		int32_t vb = *(const int32_t *)mesh / 4;
		*count = *(const int32_t *)(mesh + vb * 4 + 0x18);
		return mesh + vb * 4 + 0x1C;
	}

	static uint8_t *MeshTriangle(const uint8_t *poly, uint8_t *pkt, uint32_t ot, uint8_t *otz)
	{
		GteStoreSXY012_GT3(pkt);
		*(uint32_t *)(pkt + 4) = *(const uint32_t *)poly;
		*(uint32_t *)(pkt + 0x10) = *(const uint32_t *)(poly + 0x14);
		*(uint32_t *)(pkt + 0x1C) = *(const uint32_t *)(poly + 0x18);
		pkt[7] = poly[3];
		*(uint32_t *)pkt = 0x9000000;
		*(uint32_t *)(pkt + 0xC) = *(const uint32_t *)(poly + 0xC);
		*(uint32_t *)(pkt + 0x18) = *(const uint32_t *)(poly + 0x10);
		*(uint16_t *)(pkt + 0x24) = *(const uint16_t *)(poly + 0xA);
		GteAVSZ3();
		GteReadOTZWord(otz);
		int32_t z = *(int32_t *)otz >> 2;
		*(int32_t *)otz = z;
		InsertPrimAutoDepth(ot + z * 4, pkt);
		return pkt + 0x28;
	}

	static void DrawMesh(MeshNode *m)
	{
		uint32_t ot = RenderOT(0x44);
		uint8_t *pkt = (uint8_t *)PacketCursor();
		if (m->h.flags & 4) return;
		uint8_t *s = (uint8_t *)FieldAlloc(0x3C);
		Mat4x3 *mat = (Mat4x3 *)s;
		BuildRotationMatrixFromAngles(m->rot, mat);
		mat->t[0] = m->h.pos[0];
		mat->t[1] = m->h.pos[1];
		mat->t[2] = m->h.pos[2];
		Scale3DMatrix(mat, m->scale);
		ComposeAffineTransform(&Camera(), mat, mat);
		GteSetRotMatrix(mat);
		GteSetTransVector(mat);
		const uint8_t *mesh = m->mesh;
		const uint8_t *verts = mesh + 8;
		int32_t count;
		const uint8_t *poly = MeshPolys(mesh, &count);
		int32_t otz;
		for (int32_t k = count; k > 0; k--)
		{
			ff8fx::copy<(8)>(s + 0x20, verts + (*(const uint16_t *)(poly + 4) >> 1) * 8);
			ff8fx::copy<(8)>(s + 0x28, verts + (*(const uint16_t *)(poly + 6) >> 1) * 8);
			ff8fx::copy<(8)>(s + 0x30, verts + (*(const uint16_t *)(poly + 8) >> 1) * 8);
			GteLoadV012Block(s + 0x20);
			GteRTPT();
			GteNCLIP();
			GteReadMAC0(s + 0x38);
			if (*(int32_t *)(s + 0x38) > 0) pkt = MeshTriangle(poly, pkt, ot, (uint8_t *)&otz);
			poly += 0x1C;
		}
		PacketCursor() = (uint32_t)pkt;
		FieldFree(0x3C);
	}

	static void Sag(int16_t *v, const MeshNode *m)
	{
		int16_t y = v[1];
		if (y <= -0x100) return;
		if (y > 0x200) v[1] = 0x200;
		int32_t f = v[1] + 0x100;
		int16_t x = v[0];
		int32_t d = mul32((int32_t)x - m->h.pos[0], f);
		v[0] = (int16_t)(((d + ((d >> 31) & 0x1FF)) >> 9) + x);
		int16_t z = v[2];
		d = mul32((int32_t)z - m->h.pos[2], f);
		v[2] = (int16_t)(((d + ((d >> 31) & 0x1FF)) >> 9) + z);
		if (v[1] > 0) v[1] = 0;
	}

	static void DrawMeshPuddle(MeshNode *m)
	{
		uint8_t *pkt = (uint8_t *)PacketCursor();
		uint32_t ot = RenderOT(0x44);
		if (m->h.flags & 4) return;
		uint8_t *s = (uint8_t *)FieldAlloc(0x3C);
		Mat4x3 *mat = (Mat4x3 *)s;
		BuildRotationMatrixFromAngles(m->rot, mat);
		mat->t[0] = m->h.pos[0];
		mat->t[1] = m->h.pos[1];
		mat->t[2] = m->h.pos[2];
		Scale3DMatrix(mat, m->scale);
		const uint8_t *mesh = m->mesh;
		const uint8_t *verts = mesh + 8;
		int32_t count;
		const uint8_t *poly = MeshPolys(mesh, &count);
		GteSetLightMatrix(mat);
		GteSetBackground(mat->t[0], mat->t[1], mat->t[2]);
		GteSetRotMatrix(&Camera());
		GteSetTransVector(&Camera());
		int32_t otz;
		for (int32_t k = count; k > 0; k--)
		{
			ff8fx::copy<(8)>(s + 0x20, verts + (*(const uint16_t *)(poly + 4) >> 1) * 8);
			ff8fx::copy<(8)>(s + 0x28, verts + (*(const uint16_t *)(poly + 6) >> 1) * 8);
			ff8fx::copy<(8)>(s + 0x30, verts + (*(const uint16_t *)(poly + 8) >> 1) * 8);
			for (int i = 0; i < 3; i++)
			{
				GteLoadV0(s + 0x20 + 8 * i);
				GteMVMVA_LightV0Bk();
				GteStoreIR123(s + 0x20 + 8 * i);
			}
			for (int i = 0; i < 3; i++) Sag((int16_t *)(s + 0x20 + 8 * i), m);
			GteLoadV012(s + 0x20, s + 0x28, s + 0x30);
			GteRTPT();
			GteNCLIP();
			GteReadMAC0(s + 0x38);
			if (*(int32_t *)(s + 0x38) > 0) pkt = MeshTriangle(poly, pkt, ot, (uint8_t *)&otz);
			poly += 0x1C;
		}
		PacketCursor() = (uint32_t)pkt;
		FieldFree(0x3C);
	}

	static void PoolUpdatePos(MeshNode *m)
	{
		int16_t v[4];
		ff8fx::copy<(8)>(v, m->offs_to);
		int32_t frac = m->frac;
		FxNode *e = m->h.emitter;
		v[0] = (int16_t)(v[0] + (int16_t)(mul32((int32_t)m->offs[0] - m->offs_to[0], frac) / 4096));
		v[1] = (int16_t)(v[1] + (int16_t)(mul32((int32_t)m->offs[1] - m->offs_to[1], frac) / 4096));
		v[2] = (int16_t)(v[2] + (int16_t)(mul32((int32_t)m->offs[2] - m->offs_to[2], frac) / 4096));
		Mat4x3 r;
		int16_t out[4];
		IdentityMatrix(&r);
		RotateY(&r, m->spin);
		MatrixMulVector(&r, v, out);
		ff8fx::copy<(8)>(m->h.pos, ((EmitterNode *)e)->center);
		m->h.pos[0] = (int16_t)(m->h.pos[0] + out[0]);
		m->h.pos[1] = (int16_t)(m->h.pos[1] + out[1]);
		m->h.pos[2] = (int16_t)(m->h.pos[2] + out[2]);
	}

	static void PoolInit(MeshNode *m)
	{
		EmitterNode *e = (EmitterNode *)m->h.emitter;
		int32_t s = e->scale;
		int32_t half = s / 2;
		m->scale[2] = half;
		m->scale[1] = half;
		m->scale[0] = half;
		switch ((uint32_t)(int32_t)m->variant)
		{
		case 0:
			m->rot[1] = 0x800;
			m->offs[0] = Div40960(mul32(s, -4240));
			m->offs[1] = Div40960(mul32(s, 2720));
			m->offs[2] = Div40960(mul32(s, -1200));
			break;
		case 1:
			m->rot[1] = 0x800;
			m->offs[0] = Div40960(mul32(s, -240));
			m->offs[1] = Div40960(mul32(s, 4400));
			m->offs[2] = Div40960(mul32(s, -80));
			break;
		case 2:
			m->rot[1] = 0xA00;
			m->offs[0] = Div40960(mul32(s, 1760));
			m->offs[1] = Div40960(mul32(s, -5200));
			m->offs[2] = Div40960(mul32(s, 80));
			break;
		case 3:
			m->rot[1] = 0x100;
			m->offs[0] = Div40960(mul32(s, -400));
			m->offs[1] = Div40960(mul32(s, 80));
			m->offs[2] = Div40960(mul32(s, -5200));
			break;
		case 4:
			m->offs[0] = Div40960(mul32(s, -1760));
			m->offs[1] = Div40960(mul32(s, -160));
			m->rot[1] = 0xC00;
			m->offs[2] = Div40960(mul32(s, 4400));
			break;
		case 5:
			m->offs[0] = Div40960(mul32(s, 5200));
			m->offs[1] = Div40960(mul32(s, 1440));
			m->rot[1] = 0xC00;
			m->offs[2] = Div40960(mul32(s, 640));
			break;
		default:
			break;
		}
		m->offs_to[0] = (int16_t)(m->offs[0] * 2);
		int32_t lift = mul32(s, 640) / 2 / 4096;
		m->key_b = 1;
		m->mesh = (uint8_t *)game_addr(MESH_Pool);
		m->offs_to[1] = (int16_t)((int16_t)-e->center[1] - lift);
		m->spin = e->spin;
		m->key_a = 0;
		m->frame = 0;
		m->offs_to[2] = (int16_t)(m->offs[2] * 2);
		m->src = *(const uint8_t *const *)game_addr(POOL_KEYS);
		m->dst = *(const uint8_t *const *)(game_addr(POOL_KEYS) + 4);
		m->h.phase++;
	}

	static void PoolBubble(MeshNode *m)
	{
		m->t = (int16_t)(m->t + 0x800);
		int16_t t = m->t;
		if (t <= 0x1000) return;
		t = (int16_t)(t - 0x1000);
		m->key_a++;
		m->key_b++;
		m->t = t;
		if (m->key_a >= 4)
		{
			EmitterNode *e = (EmitterNode *)m->h.emitter;
			int16_t steps = (int16_t)(0x1E - e->h.counter);
			int16_t sd = (int16_t)((int16_t)m->scale[0] - e->scale);
			m->mesh = (uint8_t *)game_addr(MESH_Splat);
			m->key_a = 5;
			m->steps = steps;
			uint16_t sp = (uint16_t)((uint16_t)(m->spin - e->spin) & 0xFFF);
			m->scale_d = sd;
			m->key_b = 6;
			m->spin_d = (int16_t)sp;
			if ((int16_t)sp >= 0x800) m->spin_d = (int16_t)(sp - 0x1000);
			m->lit = 1;
			m->h.phase++;
		}
		m->src = *(const uint8_t *const *)(game_addr(POOL_KEYS) + 4 * m->key_a);
		m->dst = *(const uint8_t *const *)(game_addr(POOL_KEYS) + 4 * m->key_b);
	}

	static void PoolFollow(MeshNode *m, int32_t rem, int32_t steps)
	{
		EmitterNode *e = (EmitterNode *)m->h.emitter;
		m->spin = (int16_t)((int16_t)(mul32(m->spin_d, rem) / steps) + e->spin);
		int32_t sc = mul32(m->scale_d, rem) / steps + e->scale;
		m->scale[2] = sc;
		m->scale[1] = sc;
		m->scale[0] = sc;
	}

	static void PoolCollapse(MeshNode *m)
	{
		int32_t steps = m->steps;
		int16_t step = m->step;
		int32_t rem = steps - step;
		m->t = (int16_t)(m->t + 0x400);
		PoolFollow(m, rem, steps);
		int16_t ns = (int16_t)(step + 1);
		m->step = ns;
		m->frac = (int16_t)(shl32(ns, 12) / steps);
		if (m->t >= 0x1000)
		{
			m->t = 0x1000;
			m->h.phase++;
		}
	}

	static void PoolSettle(MeshNode *m)
	{
		int16_t steps = m->steps;
		int32_t rem = (int32_t)steps - m->step;
		PoolFollow(m, rem, steps);
		int16_t ns = (int16_t)(m->step + 1);
		m->step = ns;
		m->frac = (int16_t)(shl32(ns, 12) / steps);
		if (ns >= steps)
		{
			m->h.flags |= 1;
			m->h.phase++;
		}
	}

	static void PoolWaitEnd(MeshNode *m)
	{
		if (m->h.emitter->counter >= 0x1E)
		{
			m->h.flags |= 1;
			m->h.phase++;
		}
	}

	static void PoolPhase(MeshNode *m)
	{
		switch (m->h.phase)
		{
		case 0: PoolInit(m); break;
		case 1: PoolBubble(m); break;
		case 2: PoolCollapse(m); break;
		case 3: PoolSettle(m); break;
		case 4: PoolWaitEnd(m); break;
		default: break;
		}
	}

	static uint32_t __cdecl PoolTask(TaskNode *n)
	{
		MeshNode *m = (MeshNode *)n;
		PoolPhase(m);
		PoolUpdatePos(m);
		Morph(m->src, m->dst, m->mesh, m->t);
		if (m->lit == 0) DrawMesh(m);
		else DrawMeshPuddle(m);
		return Finish(&m->h);
	}

	static void BlobShow(MeshNode *m)
	{
		EmitterNode *e = (EmitterNode *)m->h.emitter;
		Morph(m->src, m->dst, m->mesh, m->t);
		if (e->big == 1) DrawMeshPuddle(m);
		else DrawMesh(m);
	}

	static void BlobBurst(MeshNode *m, int32_t k)
	{
		uint8_t *s = (uint8_t *)FieldAlloc(0x30);
		ff8fx::copy<(8)>(s + 8, m->h.pos);
		Mat4x3 *mat = (Mat4x3 *)(s + 0x10);
		BuildRotationMatrixFromAngles(m->rot, mat);
		mat->t[0] = *(const int16_t *)(s + 8);
		mat->t[1] = *(const int16_t *)(s + 0xA);
		mat->t[2] = *(const int16_t *)(s + 0xC);
		Scale3DMatrix(mat, m->scale);
		GteSetRotMatrix(mat);
		GteSetTransVector(mat);
		const uint8_t *v = m->mesh + 8 + (int16_t)k * 16;
		int32_t n = *(const int32_t *)(m->mesh + 4) / 8 - 1;
		for (; n > 0; n--)
		{
			Particle *p = (Particle *)AddEffectTask(QParticles(), game_addr(ORIG_Droplet), 0x70, m);
			GteLoadV0(v);
			GteMVMVA_RotV0Tr2();
			GteStoreIR123(p->h.pos);
			int16_t vx = (int16_t)((int16_t)(p->h.pos[0] - *(const int16_t *)(s + 8)) / 8);
			int16_t vy = (int16_t)((int16_t)(p->h.pos[1] - *(const int16_t *)(s + 0xA)) / 8);
			int16_t vz = (int16_t)((int16_t)(p->h.pos[2] - *(const int16_t *)(s + 0xC)) / 8);
			p->vel[0] = vx;
			p->vel[1] = vy;
			p->acc[0] = (int16_t)-(vx / 16);
			v += 0x40;
			p->acc[1] = (int16_t)-(vy / 16);
			p->vel[2] = vz;
			p->acc[2] = (int16_t)-(vz / 16);
		}
		FieldFree(0x30);
	}

	static void BlobInit(MeshNode *m)
	{
		EmitterNode *e = (EmitterNode *)m->h.emitter;
		ff8fx::copy<(8)>(m->h.pos, e->center);
		m->rot[1] = e->spin;
		m->mesh = (uint8_t *)game_addr(MESH_Blob);
		m->t = 0x400;
		m->src = (const uint8_t *)game_addr(KEY_BlobA);
		m->dst = (const uint8_t *)game_addr(KEY_BlobB);
		m->frame = 0;
		int32_t sc = e->scale;
		m->scale[2] = sc;
		m->scale[1] = sc;
		m->scale[0] = sc;
		BlobShow(m);
		m->h.phase++;
	}

	static void BlobGrow(MeshNode *m)
	{
		m->t = (int16_t)(m->t + 0x400);
		if (m->t >= 0x1000)
		{
			const uint8_t *a = *(const uint8_t *const *)(game_addr(BLOB_KEYS) + 0x10);
			const uint8_t *b = *(const uint8_t *const *)game_addr(BLOB_KEYS);
			m->key_a = 4;
			m->key_b = 0;
			m->t = 0;
			m->mesh = (uint8_t *)game_addr(MESH_BlobBoil);
			m->h.phase++;
			m->src = a;
			m->dst = b;
		}
		BlobShow(m);
	}

	static void BlobBoil(MeshNode *m)
	{
		EmitterNode *e = (EmitterNode *)m->h.emitter;
		m->t = (int16_t)(m->t + 0x400);
		if (m->t >= 0x1000)
		{
			m->key_a = m->key_b;
			int32_t r = CrtRand();
			m->key_b = (int16_t)(m->key_b + (int16_t)(r % 3 + 1));
			while (m->key_b >= 4) m->key_b = (int16_t)(m->key_b - 4);
			m->src = *(const uint8_t *const *)(game_addr(BLOB_KEYS) + 4 * m->key_a);
			m->cycles++;
			m->dst = *(const uint8_t *const *)(game_addr(BLOB_KEYS) + 4 * m->key_b);
			m->t = 0;
		}
		BlobShow(m);
		if (m->cycles >= 5)
		{
			BlobBurst(m, 0);
			e->acid_done = 1;
			FlashNode *f = (FlashNode *)AddEffectTask(QFlash(), game_addr(ORIG_Flash), 0x3C, m);
			f->script = (const uint32_t *)game_addr(FLASH_Script);
			m->h.phase++;
		}
	}

	static void BlobPhase(MeshNode *m)
	{
		switch (m->h.phase)
		{
		case 0: BlobInit(m); break;
		case 1: BlobGrow(m); break;
		case 2: BlobBoil(m); break;
		case 3: BlobBurst(m, 1); m->h.phase++; break;
		case 4: BlobBurst(m, 2); m->h.phase++; break;
		case 5: m->h.flags |= 1; m->h.phase++; break;
		default: break;
		}
	}

	static uint32_t __cdecl BlobTask(TaskNode *n)
	{
		MeshNode *m = (MeshNode *)n;
		BlobPhase(m);
		EmitterNode *e = (EmitterNode *)m->h.emitter;
		int32_t r = CrtRand();
		m->wob[0] = (int16_t)(m->wob[0] + (r & 0x7F) + 0x80);
		r = CrtRand();
		m->wob[0] &= 0x7FF;
		int16_t w0 = m->wob[0];
		m->wob[1] = (int16_t)(m->wob[1] + (r & 0x3F) + 0x40);
		m->wob[1] &= 0x7FF;
		r = CrtRand();
		m->wob[2] = (int16_t)(((uint32_t)(uint16_t)((r & 0x3F) + (uint16_t)m->wob[2]) + 0x10) & 0x7FF);
		int32_t sc = e->scale;
		m->scale[2] = sc;
		m->scale[1] = sc;
		m->scale[0] = sc;
		int32_t sn = ComputeSin(w0);
		m->scale[0] += mul32(e->scale / 4, sn) / 4096;
		sn = ComputeSin(m->wob[1]);
		m->scale[1] += mul32(e->scale / 4, sn) / 4096;
		sn = ComputeSin(m->wob[2]);
		m->scale[2] += mul32(e->scale / 4, sn) / 4096;
		r = CrtRand();
		m->rot[0] = (int16_t)(m->rot[0] + (r & 0x1F) + 0x20);
		m->rot[0] &= 0xFFF;
		r = CrtRand();
		int32_t ry = (int32_t)(uint16_t)((r & 0x1F) + (uint16_t)m->rot[1]) + 0x20;
		m->h.counter++;
		m->rot[1] = (int16_t)(ry & 0xFFF);
		if ((m->h.flags & 1) && m->h.children == 0)
		{
			ReleaseLinkedTask(m);
			return TASK_END;
		}
		return 0;
	}

	static void DrawSprite(Particle *p, int32_t size)
	{
		if (p->h.flags & 4) return;
		TransformCameraByShadowRotation(p->h.pos, size, p->shade);
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		*(uint32_t *)h = p->seq;
		*(int16_t *)(h + 4) = p->frame;
		*(uint16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(0x44), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static void DrawOrientedSprite(Particle *p)
	{
		if (p->h.flags & 4) return;
		Mat4x3 m = {};
		IdentityMatrix(&m);
		RotateY(&m, p->rot[1]);
		RotateZ(&m, p->rot[2]);
		RotateX(&m, p->rot[0]);
		Scale3DMatrix(&m, p->scale);
		m.t[0] = p->h.pos[0];
		m.t[1] = p->h.pos[1];
		m.t[2] = p->h.pos[2];
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		*(uint32_t *)h = p->seq;
		*(int16_t *)(h + 4) = p->frame;
		*(uint16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(0x44), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static void SplashInit(Particle *p)
	{
		if (p->variant == 0) p->seq = game_addr(SEQ_SplashA);
		else p->seq = game_addr(SEQ_SplashB);
		p->last = 0xF;
		p->vel[1] = -0x40;
		p->angle = (int16_t)(CrtRand() & 0xFFF);
		p->speed = (int16_t)((CrtRand() & 0x80) + 0x1C0);
		p->h.phase++;
	}

	static void SplashMove(Particle *p)
	{
		p->angle = (int16_t)((uint16_t)(p->speed + p->angle) & 0xFFF);
		int32_t sn = ComputeSin(p->angle);
		p->vel[0] = (int16_t)(sn / 128);
		int32_t cs = ComputeCos(p->angle);
		p->h.pos[1] = (int16_t)(p->h.pos[1] + p->vel[1]);
		p->h.pos[0] = (int16_t)(p->h.pos[0] + p->vel[0]);
		int16_t vz = (int16_t)(cs / 128);
		p->h.pos[2] = (int16_t)(p->h.pos[2] + vz);
		p->vel[2] = vz;
		if (AdvanceFrame(p))
		{
			p->h.flags |= 1;
			p->h.phase++;
		}
	}

	static uint32_t __cdecl SplashTask(TaskNode *n)
	{
		Particle *p = (Particle *)n;
		switch (p->h.phase)
		{
		case 0: SplashInit(p); break;
		case 1: SplashMove(p); break;
		default: break;
		}
		DrawSprite(p, (int16_t)p->scale[0]);
		return Finish(&p->h);
	}

	static void BubbleInit(Particle *p)
	{
		p->rot[0] = 0x400;
		if (p->variant == 0)
		{
			p->seq = game_addr(SEQ_BubbleA);
			p->last = 0xC;
		}
		else
		{
			p->seq = game_addr(SEQ_BubbleB);
			p->last = 0xC;
		}
		p->h.phase++;
	}

	static void BubblePop(Particle *p)
	{
		if (!AdvanceFrame(p)) return;
		Particle *q = (Particle *)AddEffectTask(QParticles(), game_addr(ORIG_Splash), 0x70, p);
		q->variant = p->variant;
		p->h.flags |= 1;
		ff8fx::copy<(0x10)>(q->scale, p->scale);
		p->h.phase++;
	}

	static uint32_t __cdecl BubbleTask(TaskNode *n)
	{
		Particle *p = (Particle *)n;
		switch (p->h.phase)
		{
		case 0: BubbleInit(p); break;
		case 1: BubblePop(p); break;
		default: break;
		}
		DrawOrientedSprite(p);
		return Finish(&p->h);
	}

	static void DropletInit(Particle *p)
	{
		EmitterNode *e = (EmitterNode *)p->h.emitter;
		int32_t r = CrtRand();
		p->seq = game_addr(SEQ_Droplet);
		p->last = 0xC;
		int32_t sc = mul32((r & 7) + 1, e->scale) / 8;
		p->scale[2] = sc;
		p->scale[1] = sc;
		p->scale[0] = sc;
		p->h.phase++;
	}

	static void DropletFall(Particle *p)
	{
		int16_t ay = p->acc[1];
		p->vel[0] = (int16_t)(p->vel[0] + p->acc[0]);
		p->vel[2] = (int16_t)(p->vel[2] + p->acc[2]);
		p->h.pos[0] = (int16_t)(p->h.pos[0] + p->vel[0]);
		p->h.pos[2] = (int16_t)(p->h.pos[2] + p->vel[2]);
		p->vel[1] = (int16_t)(p->vel[1] + (int16_t)(ay - 6));
		p->h.pos[1] = (int16_t)(p->h.pos[1] + p->vel[1]);
		if (AdvanceFrame(p))
		{
			p->h.flags |= 1;
			p->h.phase++;
		}
	}

	static void BelowGround(Particle *p)
	{
		if (p->h.pos[1] > 0) p->h.flags |= 5;
	}

	static uint32_t __cdecl DropletTask(TaskNode *n)
	{
		Particle *p = (Particle *)n;
		switch (p->h.phase)
		{
		case 0: DropletInit(p); break;
		case 1: DropletFall(p); break;
		default: break;
		}
		BelowGround(p);
		DrawSprite(p, (int16_t)p->scale[0]);
		return Finish(&p->h);
	}

	static void RandomOffsetInBall(Particle *p, int32_t r)
	{
		int32_t rr = r;
		if ((int16_t)rr == 0) rr = 1;
		int32_t a = CrtRand() & 0xFFF;
		int32_t b = CrtRand() & 0xFFF;
		Mat4x3 m = {};
		IdentityMatrix(&m);
		RotateY(&m, (int16_t)a);
		RotateX(&m, (int16_t)b);
		int16_t v[4] = {};
		int16_t out[4];
		int32_t d = CrtRand() % (int16_t)rr;
		v[2] = (int16_t)d;
		MatrixMulVector(&m, v, out);
		p->h.pos[0] = (int16_t)(p->h.pos[0] + out[0]);
		p->h.pos[1] = (int16_t)(p->h.pos[1] + out[1]);
		p->h.pos[2] = (int16_t)(p->h.pos[2] + out[2]);
	}

	static void RandomOffsetOnSphere(Particle *p, int32_t r)
	{
		int32_t rr = r;
		if ((int16_t)rr == 0) rr = 1;
		int32_t a = CrtRand() & 0xFFF;
		int32_t b = CrtRand() & 0xFFF;
		Mat4x3 m = {};
		IdentityMatrix(&m);
		RotateY(&m, (int16_t)a);
		RotateX(&m, (int16_t)b);
		int16_t v[4] = {};
		int16_t out[4];
		v[2] = (int16_t)rr;
		MatrixMulVector(&m, v, out);
		p->h.pos[0] = (int16_t)(p->h.pos[0] + out[0]);
		p->h.pos[1] = (int16_t)(p->h.pos[1] + out[1]);
		p->h.pos[2] = (int16_t)(p->h.pos[2] + out[2]);
	}

	static void MistAInit(Particle *p)
	{
		EmitterNode *e = (EmitterNode *)p->h.emitter;
		ff8fx::copy<(8)>(p->h.pos, e->center);
		RandomOffsetInBall(p, mul32(e->scale * 3, 256) / 4096);
		p->seq = game_addr(SEQ_MistA);
		p->last = 0xF;
		p->h.phase++;
	}

	static uint32_t __cdecl MistATask(TaskNode *n)
	{
		Particle *p = (Particle *)n;
		switch (p->h.phase)
		{
		case 0: MistAInit(p); break;
		case 1:
			if (AdvanceFrame(p))
			{
				p->h.flags |= 1;
				p->h.phase++;
			}
			break;
		default: break;
		}
		BelowGround(p);
		DrawSprite(p, (int16_t)p->scale[0]);
		return Finish(&p->h);
	}

	static void MistBInit(Particle *p)
	{
		EmitterNode *e = (EmitterNode *)p->h.emitter;
		ff8fx::copy<(8)>(p->h.pos, e->center);
		RandomOffsetOnSphere(p, mul32(e->scale * 5, 256) / 4096);
		p->seq = game_addr(SEQ_MistB);
		int32_t r = CrtRand();
		p->last = 0xB;
		int32_t d = (int16_t)((r & 7) + 0xC);
		int32_t vx = ((int32_t)p->h.pos[0] - e->center[0]) / d;
		int32_t vy = ((int32_t)p->h.pos[1] - e->center[1]) / d;
		p->vel[0] = (int16_t)vx;
		int32_t vz = ((int32_t)p->h.pos[2] - e->center[2]) / d;
		p->vel[1] = (int16_t)vy;
		p->vel[2] = (int16_t)vz;
		p->acc[0] = DivNeg12((int16_t)vx);
		p->acc[1] = DivNeg12((int16_t)vy);
		p->acc[2] = DivNeg12((int16_t)vz);
		p->h.phase++;
	}

	static void MistBMove(Particle *p)
	{
		p->vel[0] = (int16_t)(p->vel[0] + p->acc[0]);
		p->vel[1] = (int16_t)(p->vel[1] + p->acc[1]);
		p->vel[2] = (int16_t)(p->vel[2] + p->acc[2]);
		p->h.pos[0] = (int16_t)(p->h.pos[0] + p->vel[0]);
		p->h.pos[1] = (int16_t)(p->h.pos[1] + p->vel[1]);
		p->h.pos[2] = (int16_t)(p->h.pos[2] + p->vel[2]);
		if (AdvanceFrame(p))
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
		BelowGround(p);
		DrawSprite(p, (int16_t)p->scale[0]);
		return Finish(&p->h);
	}

	static void SpawnerAtCenter(FxNode *n)
	{
		ff8fx::copy<(8)>(n->pos, ((EmitterNode *)n->emitter)->center);
		n->phase++;
	}

	static void BubbleSpawn(FxNode *n)
	{
		if (n->counter > 0x14)
		{
			n->flags |= 1;
			n->phase++;
			return;
		}
		EmitterNode *e = (EmitterNode *)n->emitter;
		int32_t count = *(const uint8_t *)(game_addr(COUNT_Bubbles) + n->counter);
		for (int32_t k = count; k > 0; k--)
		{
			int32_t a = CrtRand() & 0xFFF;
			int32_t r = CrtRand() % 0x180;
			int32_t rad = mul32(r + 0x380, e->scale) / 4096;
			Particle *p = (Particle *)AddEffectTask(QParticles(), game_addr(ORIG_Bubble), 0x70, n);
			p->variant = (int16_t)(CrtRand() & 1);
			int32_t r16 = (int16_t)rad;
			int32_t sn = ComputeSin((int16_t)a);
			p->h.pos[0] = (int16_t)(p->h.pos[0] + (int16_t)(mul32(sn, r16) / 4096));
			int32_t cs = ComputeCos((int16_t)a);
			p->h.pos[1] = 0;
			p->h.pos[2] = (int16_t)(p->h.pos[2] + (int16_t)(mul32(cs, r16) / 4096));
			int32_t sc = e->scale;
			p->scale[2] = sc;
			p->scale[1] = sc;
			p->scale[0] = sc;
		}
		count = *(const uint8_t *)(game_addr(COUNT_Splashes) + n->counter);
		for (int32_t k = count; k > 0; k--)
		{
			int32_t a = CrtRand() & 0xFFF;
			int32_t r = CrtRand() % 0x180;
			int32_t rad = mul32(r + 0x380, e->scale) / 4096;
			Particle *p = (Particle *)AddEffectTask(QParticles(), game_addr(ORIG_Splash), 0x70, n);
			p->variant = (int16_t)(CrtRand() & 1);
			int32_t r16 = (int16_t)rad;
			int32_t sn = ComputeSin((int16_t)a);
			p->h.pos[0] = (int16_t)(p->h.pos[0] + (int16_t)(mul32(sn, r16) / 4096));
			int32_t cs = ComputeCos((int16_t)a);
			p->h.pos[2] = (int16_t)(p->h.pos[2] + (int16_t)(mul32(cs, r16) / 4096));
			int32_t base = (int16_t)BoundsBaseY(n);
			int32_t ry = CrtRand() & 0xFFF;
			p->h.pos[1] = (int16_t)(mul32(ry, base) / 4096);
			int32_t sc = e->scale;
			int32_t rs = CrtRand() & 0x7FF;
			int32_t v = sc - mul32(rs, sc) / 4096;
			p->scale[2] = v;
			p->scale[1] = v;
			p->scale[0] = v;
		}
	}

	static uint32_t __cdecl BubbleSpawnerTask(TaskNode *n)
	{
		FxNode *s = (FxNode *)n;
		switch (s->phase)
		{
		case 0: SpawnerAtCenter(s); break;
		case 1: BubbleSpawn(s); break;
		default: break;
		}
		return Finish(s);
	}

	static void MistSpawn(FxNode *n, int16_t last_tick, uint32_t counts, uint32_t task)
	{
		if (n->counter > last_tick)
		{
			n->flags |= 1;
			n->phase++;
			return;
		}
		EmitterNode *e = (EmitterNode *)n->emitter;
		int32_t count = *(const uint8_t *)(counts + n->counter);
		for (int32_t k = count; k > 0; k--)
		{
			Particle *p = (Particle *)AddEffectTask(QParticles(), task, 0x70, n);
			int32_t r = CrtRand();
			int32_t sc = e->scale;
			if (!(r & 1)) sc /= 2;
			p->scale[2] = sc;
			p->scale[1] = sc;
			p->scale[0] = sc;
		}
	}

	static uint32_t __cdecl MistSpawnerATask(TaskNode *n)
	{
		FxNode *s = (FxNode *)n;
		switch (s->phase)
		{
		case 0: SpawnerAtCenter(s); break;
		case 1: MistSpawn(s, 0x18, game_addr(COUNT_MistA), game_addr(ORIG_MistA)); break;
		default: break;
		}
		return Finish(s);
	}

	static uint32_t __cdecl MistSpawnerBTask(TaskNode *n)
	{
		FxNode *s = (FxNode *)n;
		switch (s->phase)
		{
		case 0: SpawnerAtCenter(s); break;
		case 1: MistSpawn(s, 0x14, game_addr(COUNT_MistB), game_addr(ORIG_MistB)); break;
		default: break;
		}
		return Finish(s);
	}

	static void SetStageWobble(int16_t amp)
	{
		for (int k = 0; k < 4; k++) *(int16_t *)(game_addr(STAGE_WOBBLE) + 2 + 0x2C * k) = amp;
	}

	static uint32_t __cdecl WobbleTask(TaskNode *n)
	{
		WobbleNode *w = (WobbleNode *)n;
		switch (w->h.phase)
		{
		case 0:
			w->h.pos[0] = 0;
			for (int k = 0; k < 4; k++)
			{
				*(int16_t *)(game_addr(STAGE_WOBBLE) + 2 + 0x2C * k) = 0;
				*(uint8_t *)(game_addr(STAGE_WOBBLE) + 0x2A + 0x2C * k) = 0;
				*(uint8_t *)(game_addr(STAGE_WOBBLE) + 0x29 + 0x2C * k) = 0;
				*(uint8_t *)(game_addr(STAGE_WOBBLE) + 0x28 + 0x2C * k) = 0;
			}
			w->h.phase++;
			break;
		case 1:
			w->h.pos[0] = (int16_t)(w->h.pos[0] + 0x80);
			if (w->h.pos[0] >= 0x400)
			{
				w->h.phase++;
				w->h.pos[0] = 0x400;
			}
			SetStageWobble(w->h.pos[0]);
			break;
		case 2:
			if (((EmitterNode *)w->h.emitter)->acid_done != 0)
			{
				w->h.pos[1] = 0;
				w->h.phase++;
			}
			break;
		case 3:
			w->h.pos[1]++;
			if (w->h.pos[1] >= 4) w->h.phase++;
			break;
		case 4:
			w->h.pos[0] = (int16_t)(w->h.pos[0] - 0x80);
			if (w->h.pos[0] <= 0)
			{
				w->h.flags |= 1;
				w->h.phase++;
				w->h.pos[0] = 0;
			}
			SetStageWobble(w->h.pos[0]);
			break;
		default: break;
		}
		return Finish(&w->h);
	}

	static void LerpClut(uint32_t a, uint32_t b, uint32_t out, int32_t t, int32_t n)
	{
		const uint8_t *pa = (const uint8_t *)a + 0x14;
		const uint8_t *pb = (const uint8_t *)b + 0x14;
		uint8_t *po = (uint8_t *)out + 0x14;
		uint8_t *s = (uint8_t *)FieldAlloc(0x18);
		int32_t t16 = (int16_t)t;
		*(int32_t *)(s + 4) = t16;
		int32_t count = (int16_t)n;
		*(int32_t *)s = 0x1000 - t16;
		intptr_t delta = pb - pa;
		for (int32_t k = count; k > 0; k--)
		{
			uint16_t ca = (uint16_t)(*(const uint16_t *)pa & 0x7FFF);
			*(uint16_t *)(s + 0x14) = ca;
			s[8] = (uint8_t)(ca & 0x1F);
			s[9] = (uint8_t)((ca >> 5) & 0x1F);
			s[10] = (uint8_t)((ca >> 10) & 0x1F);
			uint16_t cb = (uint16_t)(*(const uint16_t *)(pa + delta) & 0x7FFF);
			*(uint16_t *)(s + 0x16) = cb;
			s[0xC] = (uint8_t)(cb & 0x1F);
			s[0xD] = (uint8_t)((cb >> 5) & 0x1F);
			s[0xE] = (uint8_t)((cb >> 10) & 0x1F);
			GteSetIR0(*(const int32_t *)s);
			GteLoadIRBytes(s + 8);
			GteGPF();
			pa += 2;
			GteSetIR0(*(const int32_t *)(s + 4));
			GteLoadIRBytes(s + 0xC);
			GteGPL();
			GteStoreIRBytes(s + 0x10);
			uint32_t c = ((uint32_t)(s[0x12] & 0x1F) << 10) + ((uint32_t)(s[0x11] & 0x1F) << 5) + (uint32_t)(s[0x10] & 0x1F);
			*(uint16_t *)po = (uint16_t)(c | 0x8000);
			po += 2;
		}
		FieldFree(0x18);
	}

	static uint32_t __cdecl PaletteTask(TaskNode *n)
	{
		WobbleNode *c = (WobbleNode *)n;
		switch (c->h.phase)
		{
		case 0:
			c->from = 0;
			c->h.phase++;
			c->to = 1;
			break;
		case 1:
		{
			c->t = (int16_t)(c->t + 0x800);
			EmitterNode *e = (EmitterNode *)c->h.emitter;
			if (c->t >= 0x1000)
			{
				c->t = 0;
				c->from = c->to;
				int32_t r = CrtRand();
				c->to = (int16_t)(c->to + (int16_t)((r & 1) + 1));
				while (c->to > 2) c->to = (int16_t)(c->to - 3);
			}
			LerpClut(*(const uint32_t *)(game_addr(PALETTES) + 4 * c->from), *(const uint32_t *)(game_addr(PALETTES) + 4 * c->to), game_addr(PALETTE_TIM), c->t, 0x80);
			QueueTIMUpload(game_addr(PALETTE_TIM));
			if (e->acid_done != 0)
			{
				c->h.flags |= 1;
				c->h.phase++;
			}
			break;
		}
		default: break;
		}
		return Finish(&c->h);
	}

	static void FlashDraw(const FlashNode *f)
	{
		uint32_t bucket = RenderOT(0x1C);
		uint8_t *p = (uint8_t *)PacketCursor();
		*(uint32_t *)(p + 4) = f->colour;
		*(uint32_t *)p = 0x5000000;
		p[7] = 0x2A;
		*(uint32_t *)(p + 8) = 0;
		*(uint32_t *)(p + 0xC) = 0x140;
		*(uint32_t *)(p + 0x10) = 0xD80000;
		*(uint32_t *)(p + 0x14) = 0xD80140;
		InsertPrimAltViewport(bucket, p);
		uint8_t *q = p + 0x18;
		uint32_t tpage = GetTPage(0, 1, 0x280, 0) & 0xFFFF;
		SetDrawMode(q, 0, 0, tpage, nullptr);
		InsertPrimAutoDepth(bucket, q);
		PacketCursor() = (uint32_t)(q + 0xC);
	}

	static uint32_t __cdecl FlashTask(TaskNode *n)
	{
		FlashNode *f = (FlashNode *)n;
		int16_t idx = f->idx;
		uint32_t w = f->script[idx];
		uint8_t op = (uint8_t)(w >> 24);
		if (op == 0xFF) f->h.flags |= 1;
		else
		{
			if (op == 0xFE)
			{
				if (f->h.counter >= (int16_t)(uint8_t)(w >> 16))
				{
					idx++;
					f->idx = idx;
					f->colour = f->script[idx];
				}
			}
			else
			{
				idx++;
				f->colour = w;
				f->idx = idx;
			}
			FlashDraw(f);
		}
		return Finish(&f->h);
	}

	static void InitOverlay(OverlayCtx *mc, uint32_t a00, uint8_t *sprite, uint32_t verts, uint8_t *entity, int32_t slot)
	{
		mc->entity = entity;
		mc->slot = (int16_t)slot;
		mc->a00 = a00;
		mc->sprite = sprite;
		mc->frame_cursor = (uint32_t *)game_addr(game::BATTLE_PACKET_WRITE_CURSOR);
		mc->packets = (uint32_t *)game_addr(game::MAG_022_PacketCursor);
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

	static void OverlayInit(FxNode *o)
	{
		uint8_t slot = o->slot;
		uint8_t *entity = Entity(slot);
		InitOverlay(&Overlay(), game_addr(OVERLAY_A00), (uint8_t *)game_addr(OVERLAY_SPRITE), game_addr(OVERLAY_VERTS), entity, slot);
		o->flags |= 8;
		entity[0] |= 4;
		o->phase++;
	}

	static void OverlayGlow(OverlayCtx &mc)
	{
		mc.glow = (int16_t)(mc.glow + 0x40);
		if (mc.glow > 0x300) mc.glow = 0x300;
	}

	static uint32_t __cdecl OverlayTask(TaskNode *n)
	{
		FxNode *o = (FxNode *)n;
		OverlayCtx &mc = Overlay();
		switch (o->phase)
		{
		case 0: OverlayInit(o); break;
		case 1:
			OverlayGlow(mc);
			mc.wobble_amp = (int16_t)(mc.wobble_amp + 0x200);
			if (mc.wobble_amp >= 0x1000)
			{
				mc.wobble_amp = 0x1000;
				o->phase++;
			}
			break;
		case 2:
			OverlayGlow(mc);
			if (o->counter >= 0x1A) o->phase++;
			break;
		case 3:
		{
			mc.glow = (int16_t)(mc.glow - 0x40);
			if (mc.glow < 0) mc.glow = 0;
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

	static void EmitterInit(EmitterNode *e)
	{
		CopyMidpointFromSource(e, e->center);
		e->center[1] = (int16_t)BoundsCenterY(e);
		int16_t hh = (int16_t)BoundsHalfHeight(e);
		e->radius = hh;
		if (hh > 0xA00) e->radius = 0xA00;
		int32_t rad = e->radius;
		int32_t hi = (int32_t)(((int64_t)shl32(rad, 12) * 0x4BDA12F7) >> 32) >> 7;
		hi += (int32_t)((uint32_t)hi >> 31);
		int16_t s = (int16_t)hi;
		e->scale = s;
		if (s < 0x1000) e->scale = 0x1000;
		else if (s > 0x4000) e->scale = 0x4000;
		e->scale = (int16_t)(shl32(e->scale * 11, 8) / 4096);
		int32_t cy = e->center[1];
		if (cy + rad > -0x200)
		{
			e->big = 1;
			e->center[1] = (int16_t)(shl32(cy * 3, 10) / 4096);
		}
		AddEffectTask(QControl(), game_addr(ORIG_Wobble), 0x38, e);
		MeshNode *m = (MeshNode *)AddEffectTask(QMeshes(), game_addr(ORIG_Pool), 0x98, e);
		m->variant = 0;
		e->h.phase++;
	}

	static void EmitterPool(EmitterNode *e, int16_t tick, int16_t variant)
	{
		if (e->h.counter < tick) return;
		MeshNode *m = (MeshNode *)AddEffectTask(QMeshes(), game_addr(ORIG_Pool), 0x98, e);
		m->variant = variant;
		e->h.phase++;
	}

	static void EmitterPhase(EmitterNode *e)
	{
		switch (e->h.phase)
		{
		case 0: EmitterInit(e); break;
		case 1:
			if (e->h.counter >= 2)
			{
				AddEffectTask(QControl(), game_addr(ORIG_BubbleSpawner), 0x38, e);
				EmitterPool(e, 2, 1);
			}
			break;
		case 2: EmitterPool(e, 4, 2); break;
		case 3: EmitterPool(e, 6, 3); break;
		case 4: EmitterPool(e, 8, 4); break;
		case 5: EmitterPool(e, 10, 5); break;
		case 6:
			if (e->h.counter >= 0x1E)
			{
				AddEffectTask(QControl(), game_addr(ORIG_Palette), 0x38, e);
				AddEffectTask(QMeshes(), game_addr(ORIG_Blob), 0x98, e);
				e->h.phase++;
			}
			break;
		case 7:
			if (e->h.counter >= 0x20)
			{
				AddEffectTask(QControl(), game_addr(ORIG_MistSpawnerA), 0x38, e);
				AddEffectTask(QControl(), game_addr(ORIG_MistSpawnerB), 0x38, e);
				e->h.phase++;
			}
			break;
		case 8:
			if (e->h.counter >= 0x22)
			{
				AddEffectTask(QControl(), game_addr(ORIG_Overlay), 0x38, e);
				e->h.phase++;
			}
			break;
		case 9:
			if (e->acid_done == 1)
			{
				ApplyActionResultToTarget(e->h.ctx->actions[e->h.action].targets);
				e->h.flags |= 1;
				e->h.phase++;
			}
			break;
		default: break;
		}
	}

	static uint32_t __cdecl EmitterTask(TaskNode *n)
	{
		EmitterNode *e = (EmitterNode *)n;
		EmitterUpdatePos(e);
		ComputeModelBounds(e);
		EmitterPhase(e);
		e->spin = (int16_t)((uint16_t)(e->spin - 0x40) & 0xFFF);
		IdentityMatrix(&e->spin_m);
		RotateY(&e->spin_m, e->spin);
		if (e->h.counter == 0) BdPlaySE(SOUND_Bio(), 0, 0x80);
		return Finish(&e->h);
	}

	static void RootPhase(RootNode *r)
	{
		switch (r->h.phase)
		{
		case 0: r->h.phase++; break;
		case 1:
			r->busy = 1;
			AddEffectTask(QEmitters(), game_addr(ORIG_Emitter), 0x8C, r);
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
		r->alive = (int16_t)(r->alive + ExecuteTaskQueue(QMeshes()));
		r->alive = (int16_t)(r->alive + ExecuteTaskQueue(QFlash()));
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

	void register_mag022_bio()
	{
		register_port(bio022::ORIG_Root, (void *)bio022::RootTask, "B022 RootTask", 22);
		register_port(bio022::ORIG_Emitter, (void *)bio022::EmitterTask, "B022 EmitterTask", 22);
		register_port(bio022::ORIG_Pool, (void *)bio022::PoolTask, "B022 PoolTask", 22);
		register_port(bio022::ORIG_Wobble, (void *)bio022::WobbleTask, "B022 WobbleTask", 22);
		register_port(bio022::ORIG_BubbleSpawner, (void *)bio022::BubbleSpawnerTask, "B022 BubbleSpawnerTask", 22);
		register_port(bio022::ORIG_Splash, (void *)bio022::SplashTask, "B022 SplashTask", 22);
		register_port(bio022::ORIG_Bubble, (void *)bio022::BubbleTask, "B022 BubbleTask", 22);
		register_port(bio022::ORIG_Blob, (void *)bio022::BlobTask, "B022 BlobTask", 22);
		register_port(bio022::ORIG_Flash, (void *)bio022::FlashTask, "B022 FlashTask", 22);
		register_port(bio022::ORIG_Droplet, (void *)bio022::DropletTask, "B022 DropletTask", 22);
		register_port(bio022::ORIG_Palette, (void *)bio022::PaletteTask, "B022 PaletteTask", 22);
		register_port(bio022::ORIG_MistSpawnerA, (void *)bio022::MistSpawnerATask, "B022 MistSpawnerATask", 22);
		register_port(bio022::ORIG_MistA, (void *)bio022::MistATask, "B022 MistATask", 22);
		register_port(bio022::ORIG_MistSpawnerB, (void *)bio022::MistSpawnerBTask, "B022 MistSpawnerBTask", 22);
		register_port(bio022::ORIG_MistB, (void *)bio022::MistBTask, "B022 MistBTask", 22);
		register_port(bio022::ORIG_Overlay, (void *)bio022::OverlayTask, "B022 OverlayTask", 22);
	}
}

