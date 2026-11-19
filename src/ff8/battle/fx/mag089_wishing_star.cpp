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
#include <intrin.h>

namespace ff8fx
{
namespace ws089
{
	using namespace eng;
	using namespace magc;

	inline uint8_t *&Ctx() { return var<uint8_t *>(game_addr(game::g_MAG_089_CastContext)); }
	inline uint8_t *&Caster() { return var<uint8_t *>(game_addr(game::g_MAG_089_CasterEntity)); }
	inline uint8_t &SpritesOn() { return var<uint8_t>(game_addr(game::g_MAG_089_SpritesOn)); }
	inline uint8_t &ModelsOn() { return var<uint8_t>(game_addr(game::g_MAG_089_PModelsOn)); }
	inline uint8_t *&SpriteCursor() { return var<uint8_t *>(game_addr(game::g_MAG_089_SpriteAllocCursor)); }
	inline uint8_t *&PartCursor() { return var<uint8_t *>(game_addr(game::g_MAG_089_PModelAllocCursor)); }
	inline TaskQueue *QC() { return (TaskQueue *)game_addr(game::g_MAG_089_QueueDives); }
	inline TaskQueue *QB() { return (TaskQueue *)game_addr(game::g_MAG_089_QueueFlashDebris); }
	inline TaskQueue *QA() { return (TaskQueue *)game_addr(game::g_MAG_089_QueueDirector); }
	inline uint32_t &Cursor() { return var<uint32_t>(game_addr(game::g_MAG_089_PacketCursor)); }
	inline uint32_t &CursorEnd() { return var<uint32_t>(game_addr(game::g_MAG_089_PacketEnd)); }
	inline int16_t *Centre() { return (int16_t *)game_addr(game::g_MAG_089_TargetsCentre); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::g_MAG_089_MagicBuffer)); }
	inline uint32_t *SavedFlags() { return (uint32_t *)game_addr(game::g_MAG_089_SavedEntityFlags); }
	inline uint8_t *&TargetCursor() { return var<uint8_t *>(game_addr(game::g_MAG_089_TargetRecordCursor)); }
	inline uint32_t &FrameArena() { return var<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)); }
	inline uint32_t OTBase() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)); }

	static constexpr game::Id ORIG_RootTask = game::MAG_089_RootTask;
	static constexpr game::Id ORIG_Director = game::MAG_089_DirectorTask;
	static constexpr game::Id ORIG_FlashTask = game::MAG_089_FlashTask;
	static constexpr game::Id ORIG_DebrisTask = game::MAG_089_DebrisTask;
	static constexpr game::Id ORIG_DiveTask = game::MAG_089_DiveTask;
	static constexpr game::Id ORIG_LastDiveTask = game::MAG_089_LastDiveTask;
	static constexpr game::Id ORIG_PartCallback = game::MAG_089_PrimPartCallback;

	static constexpr game::Id STAR_GROUPS = game::MAG_089_StarGroups;
	static constexpr game::Id PART_KINDS = game::MAG_089_PModelKinds;
	static constexpr game::Id SQRT_TABLE = game::MAG_089_SqrtTable;
	static constexpr game::Id CAPTURE_MODELS = game::MAG_089_CaptureStageModels;
	static constexpr game::Id SEQ_Spark = game::MAG_089_Seq_Spark;
	static constexpr game::Id SEQ_Sparkle = game::MAG_089_Seq_Sparkle;
	static constexpr game::Id SEQ_Flash = game::MAG_089_Seq_Flash;
	static constexpr game::Id MDL_Debris = game::MAG_089_Model_Debris;
	static constexpr game::Id MDL_Trail = game::MAG_089_Model_Trail;
	static constexpr game::Id MDL_Shards = game::MAG_089_Model_Shards;
	static constexpr game::Id LAYOUT_Dive = game::MAG_089_Layout_Dive;

	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline void RotY(int32_t angle, void *out) { fn<void (__cdecl *)(int32_t, void *)>(game_addr(game::Effect_RotationYMatrix))(angle, out); }
	inline void RotX(int32_t angle, void *out) { fn<void (__cdecl *)(int32_t, void *)>(game_addr(game::MAG_063_RotationXMatrix))(angle, out); }
	inline void RotZ(int32_t angle, void *out) { fn<void (__cdecl *)(int32_t, void *)>(game_addr(game::MAG_065_RotationZMatrix))(angle, out); }
	inline void RotationFromAngles(const int16_t *angles, void *out) { fn<void (__cdecl *)(const int16_t *, void *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
	inline void MatrixMultiply(const void *a, void *b) { fn<void (__cdecl *)(const void *, void *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void MatMul3(const void *a, const void *b, void *out) { fn<void (__cdecl *)(const void *, const void *, void *)>(game_addr(game::Matrix3x3_MultiplyPSX))(a, b, out); }
	inline void MatVec(const void *m, const int16_t *in, int16_t *out) { fn<void (__cdecl *)(const void *, const int16_t *, int16_t *)>(game_addr(game::matrixMultiplyVector))(m, in, out); }
	inline void TransformVec3(const void *m, const int32_t *in, int32_t *out) { fn<void (__cdecl *)(const void *, const int32_t *, int32_t *)>(game_addr(game::TransformVectorBy3x3Matrix))(m, in, out); }
	inline int32_t ISqrt(int32_t v) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::Math_SqrtInt))(v); }
	inline int32_t ComputeCos(int32_t angle) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(angle); }
	inline void DirectionFromAngles(int32_t pitch, int32_t yaw, int16_t *out) { fn<void (__cdecl *)(int32_t, int32_t, int16_t *)>(game_addr(game::Effect_DirectionFromPitchYaw))(pitch, yaw, out); }
	inline uint32_t RenderPrimSet(void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::MAG_063_RenderPrimSet))(h, ot, mode, cursor); }
	inline uint32_t Tile(int32_t r, int32_t g, int32_t b, int32_t level, uint32_t cursor) { return fn<uint32_t (__cdecl *)(int32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_FullScreenTile))(r, g, b, level, cursor); }
	inline void QueueChainTransformation(uint8_t *entity, int32_t a) { fn<void (__cdecl *)(uint8_t *, int32_t)>(game_addr(game::QueueChainTransformation))(entity, a); }
	inline uint32_t BindExternalAnimSet(uint8_t *entity, void *out, uint32_t data) { return fn<uint32_t (__cdecl *)(uint8_t *, void *, uint32_t)>(game_addr(game::BattleEntity_BindExternalAnimSet))(entity, out, data); }
	inline void BindModelContainer(void *c, void *sections, uint32_t data) { fn<void (__cdecl *)(void *, void *, uint32_t)>(game_addr(game::Effect_BindModelContainerSections))(c, sections, data); }
	inline void SetAnim(void *c, int32_t id) { fn<void (__cdecl *)(void *, int32_t)>(game_addr(game::au_re_Battle_ReadAnimation_6))(c, id); }
	inline uint32_t DrawCreatureModel(void *c, uint32_t buf, uint32_t cursor, uint32_t end) { return fn<uint32_t (__cdecl *)(void *, uint32_t, uint32_t, uint32_t)>(game_addr(game::Effect_DrawCreatureModel))(c, buf, cursor, end); }
	inline void GetEffectSpawnPosition(void *c, int32_t bone, int32_t a, int16_t *out) { fn<void (__cdecl *)(void *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(c, bone, a, out); }
	inline void CameraArmReturn() { fn<void (__cdecl *)()>(game_addr(game::BS_Camera_ArmReturnAfterEffect))(); }
	inline void StreamLoad(int32_t id, uint32_t dst, int32_t kind) { fn<void (__cdecl *)(int32_t, uint32_t, int32_t)>(game_addr(game::Stream_QueueLoad))(id, dst, kind); }
	inline int32_t StreamBusy() { return fn<int32_t (__cdecl *)()>(game_addr(game::Stream_IsBusy))(); }
	inline void StreamPump() { fn<void (__cdecl *)()>(game_addr(game::Stream_Pump))(); }
	inline int32_t StreamPending(int32_t a) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::au_re_BattleFile_preLoad_1))(a); }
	inline void QueueTIMUpload(uint32_t tim) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::Battle_QueueTIMUpload_GetEOF))(tim); }
	inline void InsertPrimAltViewport(uint32_t bucket, void *packet) { fn<void (__cdecl *)(uint32_t, void *)>(game_addr(game::SSIGPU_InsertPrimAltViewport))(bucket, packet); }
	inline void ClearOrderingTable(uint32_t ot, int32_t n) { fn<void (__cdecl *)(uint32_t, int32_t)>(game_addr(game::SSIGPU_ClearOrderingTable))(ot, n); }
	inline void GetDrawEnv(void *env) { fn<void (__cdecl *)(void *)>(game_addr(game::GPU_GetDrawEnv))(env); }
	inline void SetDefDrawEnv(void *env, int32_t x, int32_t y, int32_t w, int32_t h) { fn<void (__cdecl *)(void *, int32_t, int32_t, int32_t, int32_t)>(game_addr(game::GPU_SetDefDrawEnv))(env, x, y, w, h); }
	inline void SetDrawEnv(void *packet, const void *env) { fn<void (__cdecl *)(void *, const void *)>(game_addr(game::SSIGPU_SetDrawEnv))(packet, env); }
	inline void SetDrawMode(void *packet, int32_t dfe, int32_t dtd, int32_t tpage, const void *rect) { fn<void (__cdecl *)(void *, int32_t, int32_t, int32_t, const void *)>(game_addr(game::GPU_SetDrawMode))(packet, dfe, dtd, tpage, rect); }
	inline uint32_t RenderGeometry(uint32_t group, void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(uint32_t, void *, uint32_t, int32_t, uint32_t)>(game_addr(game::RenderGeometry))(group, h, ot, mode, cursor); }
	inline uint32_t DrawShadow(uint8_t *entity, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(uint8_t *, uint32_t, int32_t, uint32_t)>(game_addr(game::BattleEntity_DrawShadow))(entity, ot, mode, cursor); }
	inline int32_t BattleReadAnimation(void *hdr, void *cmd) { return fn<int32_t (__cdecl *)(void *, void *)>(game_addr(game::Battle_ReadAnimation))(hdr, cmd); }
	inline void PreBattleReadAnimation(void *hdr, void *cmd, uint32_t anim) { fn<void (__cdecl *)(void *, void *, uint32_t)>(game_addr(game::pre_Battle_ReadAnimation))(hdr, cmd, anim); }
	inline void ScreenFeedback() { fn<void (__cdecl *)()>(game_addr(game::MAG_065_GreatAttractor_ScreenFeedback))(); }
	inline void CameraBlend(const int16_t *k0, const int16_t *k1, int32_t za, int32_t zb, int32_t t) { fn<void (__cdecl *)(const int16_t *, const int16_t *, int32_t, int32_t, int32_t)>(game_addr(game::MAG_065_GreatAttractor_CameraBlend))(k0, k1, za, zb, t); }
	inline void GteLoadIRBytes(const void *src) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadIR123FromBytes))(src); }
	inline void GteStoreIRBytes(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreIR123LowBytes))(dst); }
	inline void GteLoadIRColumn(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadIRFromMatrixColumn))(m); }
	inline void GteStoreIRColumn(void *m) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreIRToMatrixColumn))(m); }
	inline void GteMVMVA_RotIR() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotIR))(); }
	inline void GteMVMVA_RotV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0))(); }
	inline void GteLoadV0Dwords(const void *v) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadV0FromDwords))(v); }
	inline void GteLoadV0u16(const void *v) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadV0_u16))(v); }
	inline void GteLoadV012(const void *a, const void *b, const void *c) { fn<void (__cdecl *)(const void *, const void *, const void *)>(game_addr(game::GTE_LoadV012))(a, b, c); }
	inline void GteRTPT() { fn<void (__cdecl *)()>(game_addr(game::GTE_RTPT))(); }
	inline void GteReadFLAG(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadFLAG))(dst); }
	inline void GteReadSXY012(void *a, void *b, void *c) { fn<void (__cdecl *)(void *, void *, void *)>(game_addr(game::GTE_ReadSXY012_Split))(a, b, c); }
	inline void GteAVSZ4() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ4))(); }
	inline void GteReadOTZWord(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadOTZ))(dst); }
	inline void GteSetFarColor(int32_t r, int32_t g, int32_t b) { fn<void (__cdecl *)(int32_t, int32_t, int32_t)>(game_addr(game::GTE_SetFarColor))(r, g, b); }
	inline void GteLoadRGBC(const void *src) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadRGBC))(src); }
	inline void GteDPCS() { fn<void (__cdecl *)()>(game_addr(game::GTE_DPCS))(); }
	inline void GteStoreRGB2(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreRGB2))(dst); }

	inline uint32_t mulhi_u(uint32_t a, uint32_t b) { return (uint32_t)(((uint64_t)a * b) >> 32); }
	inline int32_t mulhi_s(int32_t a, int32_t b) { return (int32_t)(((int64_t)a * b) >> 32); }
	inline int32_t sdiv_pow2(int32_t v, int n) { return (int32_t)((uint32_t)v + ((uint32_t)(v >> 31) & ((1u << n) - 1))) >> n; }
	inline int32_t div6(int32_t v) { const int32_t h = mulhi_s(v, 0x2AAAAAAB); return h + (int32_t)((uint32_t)h >> 31); }
	inline int32_t div80(int32_t v) { const int32_t h = mulhi_s(v, 0x66666667) >> 5; return h + (int32_t)((uint32_t)h >> 31); }
	inline int32_t div3(int32_t v) { const int32_t h = mulhi_s(v, 0x55555556); return h + (int32_t)((uint32_t)h >> 31); }
	template<typename T> static inline T &F(void *p, uint32_t off) { return *(T *)((uint8_t *)p + off); }

#pragma pack(push, 1)
	struct RootNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t pad0E;
		uint8_t started;
		uint32_t parity;
	};
	struct DirectorNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t pad0E[6];
		uint8_t targets[0x20];
		uint8_t angelo[0x9C];
		uint32_t rinoa_xy;
		uint32_t rinoa_z;
		int16_t centre[3];
		int16_t padDE;
		uint32_t rinoa_ang0;
		uint32_t rinoa_ang1;
		uint8_t sections[0x10];
		uint8_t animset[0x20];
		uint8_t layout[0x218];
		uint8_t layout2[0xEC];
	};
	struct SmallNode
	{
		TaskNode hdr;
		int16_t pos[3];
		int16_t size;
		int16_t vel[3];
		int8_t ang[2];
		int8_t gravity;
		int8_t dang[2];
		int8_t frame;
		uint8_t pad20[0x10];
	};
	struct DiveNode
	{
		TaskNode hdr;
		Mat4x3 m;
		int16_t counter;
		int16_t pad2E;
		uint8_t layout[0x158];
	};
	struct PrimArg
	{
		Mat4x3 m;
		uint32_t cursor;
		int16_t depth;
		int16_t pad26;
		uint32_t morph;
	};
	struct Sprite
	{
		uint32_t seq;
		int16_t pos[3];
		int16_t life;
		int16_t vel[3];
		int16_t scale;
		int8_t acc[3];
		uint8_t pad17;
	};
	struct PModel
	{
		uint8_t life;
		uint8_t kind;
		int16_t fade;
		int16_t scale[3];
		int16_t vel[3];
		int16_t m[9];
		int16_t pad22;
		int32_t pos[3];
	};
	struct Shard
	{
		int16_t pos[3];
		int16_t life;
		int16_t vel[3];
		int16_t len;
		uint32_t rot;
		int16_t v[3][3];
		uint8_t pad26[2];
		uint8_t uv0[2];
		uint16_t clut;
		uint8_t uv1[2];
		uint16_t tpage;
		uint8_t uv2[2];
		uint8_t pad32[2];
	};
	struct DrawEnv
	{
		int16_t clip[4];
		int16_t ofs[2];
		int16_t tw[4];
		uint16_t tpage;
		uint8_t dtd, dfe;
		uint8_t isbg, r0, g0, b0;
		uint8_t dr_env[0x40];
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x14 && sizeof(DirectorNode) == 0x41C && sizeof(SmallNode) == 0x30 && sizeof(DiveNode) == 0x188, "Wishing Star 089 nodes");
	static_assert(sizeof(PrimArg) == 0x2C && sizeof(Sprite) == 0x18 && sizeof(PModel) == 0x30 && sizeof(Shard) == 0x34 && sizeof(DrawEnv) == 0x5C, "Wishing Star 089 blocks");
}
}

namespace ff8fx
{
namespace ws089
{
	using namespace eng;
	using namespace magc;

	inline Sprite *Sprites() { return (Sprite *)(TexBase() + 0x1240); }
	inline PModel *PModels() { return (PModel *)(TexBase() + 0x4240); }
	inline Shard *Shards() { return (Shard *)(TexBase() + 0x5EA4); }

	static void SaveEntityFlags()
	{
		uint32_t *dst = SavedFlags();
		for (uint32_t e = game_addr(game::BattleEntitySlotData); e < game_addr(game::CAMERA_FLAG_RELATED); e += 0x9C) *dst++ = *(const uint16_t *)e;
	}

	static void PartyFlag4On()
	{
		for (uint32_t e = game_addr(game::BattleEntitySlotData); e < game_addr(game::unk_1D97494); e += 0x9C)
		{
			const uint16_t f = *(const uint16_t *)e;
			if (f & 2) *(uint16_t *)e = (uint16_t)(f | 4);
		}
	}

	static void EnemyFlag4On()
	{
		for (uint32_t e = game_addr(game::unk_1D97494); e < game_addr(game::CAMERA_FLAG_RELATED); e += 0x9C)
		{
			const uint16_t f = *(const uint16_t *)e;
			if (f & 2) *(uint16_t *)e = (uint16_t)(f | 4);
		}
	}

	static void RestoreFlag4(uint32_t e, const uint32_t *src, const uint32_t *end)
	{
		for (; src < end; e += 0x9C, src++)
		{
			const uint16_t f = *(const uint16_t *)e;
			if (f & 2) *(uint16_t *)e = (uint16_t)((((uint8_t)*src ^ (uint32_t)f) & 4) ^ f);
		}
	}
	static void RestoreEnemyFlags() { RestoreFlag4(game_addr(game::unk_1D97494), SavedFlags() + 3, SavedFlags() + 7); }
	static void RestorePartyFlags() { RestoreFlag4(game_addr(game::BattleEntitySlotData), SavedFlags(), SavedFlags() + 3); }

	static void StageModelsBack()
	{
		for (uint32_t m = game_addr(game::unk_1D98991); m < game_addr(game::unk_1D98A41); m += 0x2C)
		{
			ComputeBonesWorldMatrices((void *)(m + 0x13), &Camera());
			const uint8_t b = *(const uint8_t *)m;
			if (b & 1) *(uint8_t *)m = (uint8_t)(b | 2);
		}
		RestoreFlag4(game_addr(game::unk_1D97494), SavedFlags() + 3, SavedFlags() + 7);
	}

	static void PModelsInit()
	{
		uint8_t *p = (uint8_t *)PModels();
		for (int n = 0x80; n != 0; n--, p += 0x30) *p = 0;
		PartCursor() = (uint8_t *)PModels();
	}

	static void SpritesInit()
	{
		uint8_t *p = (uint8_t *)Sprites() + 0xA;
		for (int n = 0x200; n != 0; n--, p += 0x18) *(int16_t *)p = 0;
		SpriteCursor() = (uint8_t *)Sprites();
	}

	static void ShardsReset()
	{
		uint8_t *p = (uint8_t *)Shards() + 6;
		for (int n = 0x78; n != 0; n--, p += 0x34) *(int16_t *)p = 0;
		uint8_t *m = (uint8_t *)(TexBase() + 0x7704);
		for (int n = 0x40; n != 0; n--, m += 0x14)
		{
			int16_t a[4];
			a[0] = (int16_t)((CrtRand() & 0x1FF) - 0x100);
			a[1] = (int16_t)((CrtRand() & 0x1FF) - 0x100);
			a[2] = (int16_t)((CrtRand() & 0x1FF) - 0x100);
			BuildRotationMatrixFromAngles(a, (Mat4x3 *)m);
			const int32_t s = 0xF00 - (CrtRand() & 0x1FF);
			int32_t v[3] = { s, s, s };
			Scale3DMatrix((Mat4x3 *)m, v);
		}
	}

	static Sprite *SpriteAlloc()
	{
		uint8_t *p = SpriteCursor();
		const uint32_t tex = TexBase();
		if (*(int16_t *)(p + 0xA) != 0)
		{
			p = (uint8_t *)(tex + 0x1240);
			if (*(int16_t *)(tex + 0x124A) != 0)
			{
				int n = 0x200;
				for (;;)
				{
					p += 0x18;
					if (--n == 0) return nullptr;
					if (*(int16_t *)(p + 0xA) == 0) break;
				}
			}
		}
		if ((uint32_t)p >= tex + 0x4228) SpriteCursor() = (uint8_t *)(tex + 0x1240);
		else SpriteCursor() = p + 0x18;
		return (Sprite *)p;
	}

	static PModel *PModelAlloc()
	{
		uint8_t *p = PartCursor();
		const uint32_t tex = TexBase();
		if (*p != 0)
		{
			p = (uint8_t *)(tex + 0x4240);
			if (*p != 0)
			{
				int n = 0x80;
				for (;;)
				{
					p += 0x30;
					if (--n == 0) return nullptr;
					if (*p == 0) break;
				}
			}
		}
		if ((uint32_t)p >= tex + 0x5A10) PartCursor() = (uint8_t *)(tex + 0x4240);
		else PartCursor() = p + 0x30;
		return (PModel *)p;
	}

	static void ShardsInit(uint32_t model, const int16_t *off)
	{
		uint8_t *rec = (uint8_t *)Shards();
		uint8_t *h = (uint8_t *)FieldAlloc(0x24);
		F<uint32_t>(h, 0) = *(const uint32_t *)model + model + 8;
		F<uint32_t>(h, 0x1C) = *(const uint32_t *)off;
		F<uint32_t>(h, 0x20) = *(const uint32_t *)(off + 2);
		uint32_t p = F<uint32_t>(h, 0);
		const int32_t count = *(const int32_t *)p;
		F<uint32_t>(h, 0) = p + 4;
		if (count > 0)
		{
			uint32_t t = p + 4 + 6;
			int16_t *v0 = &F<int16_t>(h, 4);
			for (int32_t left = count; left != 0; left--, t += 0x14)
			{
				uint32_t i0 = *(const uint16_t *)(t - 2), i1 = *(const uint16_t *)t, i2 = *(const uint16_t *)(t + 2);
				F<uint32_t>(h, 4) = *(const uint32_t *)(model + i0 * 4 + 8);
				F<uint32_t>(h, 8) = *(const uint32_t *)(model + i0 * 4 + 0xC);
				F<uint32_t>(h, 0xC) = *(const uint32_t *)(model + i1 * 4 + 8);
				F<uint32_t>(h, 0x10) = *(const uint32_t *)(model + i1 * 4 + 0xC);
				F<uint32_t>(h, 0x14) = *(const uint32_t *)(model + i2 * 4 + 8);
				F<uint32_t>(h, 0x18) = *(const uint32_t *)(model + i2 * 4 + 0xC);
				int32_t cx = div3(F<int16_t>(h, 0xC) + F<int16_t>(h, 0x14) + v0[0]);
				F<int16_t>(rec, 0) = (int16_t)cx;
				int32_t cy = div3(F<int16_t>(h, 0x16) + F<int16_t>(h, 0xE) + F<int16_t>(h, 6));
				F<int16_t>(rec, 2) = (int16_t)cy;
				int32_t cz = div3(F<int16_t>(h, 8) + F<int16_t>(h, 0x18) + F<int16_t>(h, 0x10));
				F<int16_t>(rec, 4) = (int16_t)cz;
				int32_t minx = v0[0];
				int flag = 0;
				if (F<int16_t>(h, 0xC) < minx) { minx = F<int16_t>(h, 0xC); flag = 1; }
				const int16_t ox = v0[0];
				if (F<int16_t>(h, 0x14) < minx)
				{
					const int16_t y2 = F<int16_t>(h, 0x16);
					v0[0] = F<int16_t>(h, 0x14);
					F<int16_t>(h, 0x14) = F<int16_t>(h, 0xC);
					F<int16_t>(h, 0xC) = ox;
					const int16_t oy = F<int16_t>(h, 6);
					F<int16_t>(h, 6) = y2;
					F<int16_t>(h, 0x16) = F<int16_t>(h, 0xE);
					const int16_t z2 = F<int16_t>(h, 0x18);
					F<int16_t>(h, 0xE) = oy;
					const int16_t oz = F<int16_t>(h, 8);
					F<int16_t>(h, 8) = z2;
					F<int16_t>(h, 0x18) = F<int16_t>(h, 0x10);
					F<int16_t>(h, 0x10) = oz;
				}
				else if (flag == 1)
				{
					const int16_t y1 = F<int16_t>(h, 0xE);
					v0[0] = F<int16_t>(h, 0xC);
					F<int16_t>(h, 0xC) = F<int16_t>(h, 0x14);
					F<int16_t>(h, 0x14) = ox;
					const int16_t oy = F<int16_t>(h, 6);
					F<int16_t>(h, 6) = y1;
					F<int16_t>(h, 0xE) = F<int16_t>(h, 0x16);
					const int16_t z1 = F<int16_t>(h, 0x10);
					F<int16_t>(h, 0x16) = oy;
					const int16_t oz = F<int16_t>(h, 8);
					F<int16_t>(h, 8) = z1;
					F<int16_t>(h, 0x10) = F<int16_t>(h, 0x18);
					F<int16_t>(h, 0x18) = oz;
				}
				F<int16_t>(rec, 0x14) = (int16_t)(v0[0] - cx);
				F<int16_t>(rec, 0x16) = (int16_t)(F<int16_t>(h, 6) - cy);
				F<int16_t>(rec, 0x18) = (int16_t)(F<int16_t>(h, 8) - cz);
				F<int16_t>(rec, 0x1A) = (int16_t)(F<int16_t>(h, 0xC) - cx);
				F<int16_t>(rec, 0x1C) = (int16_t)(F<int16_t>(h, 0xE) - cy);
				F<int16_t>(rec, 0x1E) = (int16_t)(F<int16_t>(h, 0x10) - cz);
				F<int16_t>(rec, 0x20) = (int16_t)(F<int16_t>(h, 0x14) - cx);
				F<int16_t>(rec, 0x22) = (int16_t)(F<int16_t>(h, 0x16) - cy);
				F<int16_t>(rec, 0x2A) = 0;
				F<int16_t>(rec, 0x24) = (int16_t)(F<int16_t>(h, 0x18) - cz);
				const int32_t x0 = v0[0];
				const int32_t base = (int32_t)(((uint32_t)x0 & 0xFFFFFF00u) | ((uint32_t)x0 & 0xC0u));
				F<uint16_t>(rec, 0x2E) = (uint16_t)((((base + 0x180) >> 6) & 0xF) | 0x130);
				F<uint8_t>(rec, 0x28) = (uint8_t)((uint8_t)x0 - (uint8_t)base);
				F<uint8_t>(rec, 0x29) = (uint8_t)F<int16_t>(h, 6);
				F<uint8_t>(rec, 0x2C) = (uint8_t)((uint8_t)F<int16_t>(h, 0xC) - (uint8_t)base);
				F<uint8_t>(rec, 0x2D) = (uint8_t)F<int16_t>(h, 0xE);
				F<uint8_t>(rec, 0x30) = (uint8_t)((uint8_t)F<int16_t>(h, 0x14) - (uint8_t)base);
				F<uint8_t>(rec, 0x31) = (uint8_t)F<int16_t>(h, 0x16);
				v0[0] = (int16_t)(cx - F<int16_t>(h, 0x1C));
				F<int16_t>(h, 6) = (int16_t)(cy - F<int16_t>(h, 0x1E));
				F<int16_t>(h, 8) = (int16_t)(cz - F<int16_t>(h, 0x20));
				GteLoadIR123(v0);
				GteSQR();
				int32_t sq, lz;
				GteSumMAC123(&sq);
				GteSetLZCS(sq);
				GteReadLZCR(&lz);
				const int32_t z = lz & ~1;
				const int32_t sh = (0x1F - z) >> 1;
				const int32_t idx = (z - 0x18 > 0) ? shl32(sq, (z - 0x18) & 31) : (sq >> ((0x18 - z) & 31));
				int32_t len = shl32((int32_t)*(const uint16_t *)(game_addr(SQRT_TABLE) + (uint32_t)idx * 2), sh & 31) >> 12;
				F<int16_t>(rec, 0xE) = (int16_t)len;
				F<int16_t>(rec, 8) = (int16_t)(shl32(v0[0], 2) / len);
				F<int16_t>(rec, 0xA) = (int16_t)(shl32(F<int16_t>(h, 6), 2) / len);
				F<int16_t>(rec, 0xC) = (int16_t)(shl32(F<int16_t>(h, 8), 2) / len);
				rec += 0x34;
			}
		}
		FieldFree(0x24);
		if (count > 0)
		{
			uint8_t *s = (uint8_t *)Shards();
			for (int32_t left = count; left != 0; left--, s += 0x34)
			{
				F<int16_t>(s, 6) = 0x10;
				const int32_t r = CrtRand() & 0x3F;
				F<uint32_t>(s, 0x10) = TexBase() + (uint32_t)r * 0x14 + 0x7704;
			}
		}
	}

	static void ShardsUpdate(int32_t limit)
	{
		uint8_t *rec = (uint8_t *)Shards();
		for (int n = 0x78; n != 0; n--, rec += 0x34)
		{
			if (F<int16_t>(rec, 6) == 0) continue;
			if (F<int16_t>(rec, 0xE) >= limit) continue;
			GteSetRotMatrixCtrl((const Mat4x3 *)F<uint32_t>(rec, 0x10));
			GteLoadV0(rec + 0x14);
			GteMVMVA_RotV0();
			GteStoreIR123(rec + 0x14);
			GteLoadV0u16(rec + 0x1A);
			GteMVMVA_RotV0();
			if (F<int32_t>(rec, 0x14) == 0 && F<int16_t>(rec, 0x18) == 0)
			{
				F<int16_t>(rec, 6) = 0;
				continue;
			}
			GteStoreIR123(rec + 0x1A);
			GteLoadV0(rec + 0x20);
			GteMVMVA_RotV0();
			GteStoreIR123(rec + 0x20);
			F<int16_t>(rec, 0) = (int16_t)(F<int16_t>(rec, 0) + F<int16_t>(rec, 8));
			F<int16_t>(rec, 2) = (int16_t)(F<int16_t>(rec, 2) + F<int16_t>(rec, 0xA));
			F<int16_t>(rec, 4) = (int16_t)(F<int16_t>(rec, 4) + F<int16_t>(rec, 0xC));
		}
	}

	static void ShardsDraw()
	{
		uint8_t *pk = (uint8_t *)FrameArena();
		GteSetRotMatrix(&Camera());
		GteSetTransVector(&Camera());
		const uint8_t *rec = (const uint8_t *)Shards();
		for (int n = 0x78; n != 0; n--, rec += 0x34)
		{
			if (F<int16_t>((void *)rec, 6) == 0) continue;
			F<uint32_t>(pk, 0xC) = *(const uint32_t *)(rec + 0x28);
			F<uint32_t>(pk, 0x14) = *(const uint32_t *)(rec + 0x2C);
			F<uint16_t>(pk, 0x1C) = *(const uint16_t *)(rec + 0x30);
			const int16_t cx = *(const int16_t *)rec, cy = *(const int16_t *)(rec + 2);
			F<int16_t>(pk, 8) = (int16_t)(*(const int16_t *)(rec + 0x14) + cx);
			F<int16_t>(pk, 0xA) = (int16_t)(*(const int16_t *)(rec + 0x16) + cy);
			F<int16_t>(pk, 0x10) = (int16_t)(*(const int16_t *)(rec + 0x1A) + cx);
			F<int16_t>(pk, 0x12) = (int16_t)(*(const int16_t *)(rec + 0x1C) + cy);
			F<int16_t>(pk, 0x18) = (int16_t)(*(const int16_t *)(rec + 0x20) + cx);
			F<int16_t>(pk, 0x1A) = (int16_t)(*(const int16_t *)(rec + 0x22) + cy);
			uint32_t c = 0;
			if (F<uint16_t>(pk, 8) > 0xA00) c = 1;
			if (F<uint16_t>(pk, 0x10) > 0xA00) c |= 2;
			if (F<uint16_t>(pk, 0x18) > 0xA00) c |= 4;
			if (F<uint16_t>(pk, 0xA) > 0x6C0) c |= 0x10;
			if (F<uint16_t>(pk, 0x12) > 0x6C0) c |= 0x20;
			if (F<uint16_t>(pk, 0x1A) > 0x6C0) c |= 0x40;
			if ((c & 7) == 7 || (c & 0x70) == 0x70) continue;
			F<uint32_t>(pk, 0) = 0x7000000;
			F<uint32_t>(pk, 4) = 0x24808080;
			InsertPrimAltViewport(OTBase() + 0x1000, pk);
			pk += 0x20;
		}
		FrameArena() = (uint32_t)pk;
	}

	static void CaptureModels(uint32_t ot)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x4C);
		F<int16_t>(h, 0x34) = 0;
		F<int16_t>(h, 0x36) = 0;
		F<int16_t>(h, 0x38) = 0x140;
		F<int16_t>(h, 0x3A) = 0xD8;
		F<int32_t>(h, 0x40) = -1;
		F<uint32_t>(h, 0x24) = var<uint32_t>(game_addr(game::g_BattleVertexWorkspace));
		F<uint32_t>(h, 0x30) = var<uint32_t>(game_addr(game::BATTLE_PACKET_BUFFER_END));
		const uint32_t *tbl = (const uint32_t *)game_addr(CAPTURE_MODELS);
		for (uint32_t m = game_addr(game::unk_1D989A4); tbl < (const uint32_t *)game_addr(game::MAG_089_TIM_0); tbl += 2, m += 0x2C)
		{
			if (!(*(const uint8_t *)(m - 0x13) & 1)) continue;
			BuildBoneMatricesFromPose((void *)m);
			ComputeBonesWorldMatrices((void *)m, &Camera());
			F<uint32_t>(h, 0x3C) = *(const uint32_t *)(m + 0x14);
			F<uint16_t>(h, 0x44) = *(const uint16_t *)(m - 0x12);
			FrameArena() = RenderGeometry(m - 0x10, h + 0x20, ot + tbl[0] * 4, (int32_t)tbl[1], FrameArena());
			*(uint8_t *)(m - 0x13) &= 0xFD;
		}
		for (uint32_t e = game_addr(game::unk_1D97494); e < game_addr(game::CAMERA_FLAG_RELATED); e += 0x9C)
		{
			const uint16_t f = *(const uint16_t *)e;
			if (!(f & 2)) continue;
			*(uint16_t *)e = (uint16_t)(f | 4);
			ComposeAffineTransform(&Camera(), (const Mat4x3 *)(e + 0x40), (Mat4x3 *)h);
			FrameArena() = DrawShadow((uint8_t *)e, ot + 0x4040, 0x10, FrameArena());
			ComputeBonesWorldMatrices((void *)(e + 0x60), h);
			F<uint32_t>(h, 0x24) = var<uint32_t>(game_addr(game::g_BattleVertexWorkspace));
			F<int16_t>(h, 0x34) = 0;
			F<int16_t>(h, 0x36) = 0;
			F<int16_t>(h, 0x38) = 0x140;
			F<int16_t>(h, 0x3A) = 0xD8;
			F<uint32_t>(h, 0x3C) = *(const uint32_t *)(e + 0x28);
			F<uint32_t>(h, 0x48) = 0;
			F<uint32_t>(h, 0x40) = *(const uint32_t *)(e + 0x7C);
			FrameArena() = RenderGeometry(*(const uint32_t *)(e + 0x64), h + 0x20, ot + 0x44, 4, FrameArena());
			BuildBoneMatricesFromPose((void *)(e + 0x60));
			const uint32_t w = *(const uint32_t *)(e + 0x78);
			if (w == 0) continue;
			F<int32_t>(h, 0x40) = -1;
			ComputeBonesWorldMatrices((void *)w, h);
			FrameArena() = RenderGeometry(*(const uint32_t *)(w + 4), h + 0x20, ot + 0x44, 4, FrameArena());
			BuildBoneMatricesFromPose((void *)w);
		}
		FieldFree(0x4C);
	}

	static int32_t Capture(uint32_t ot)
	{
		ClearOrderingTable(ot, 0x1122);
		DrawEnv d1;
		GetDrawEnv(&d1);
		int16_t rect[4];
		rect[3] = 0x100;
		rect[2] = 0x100;
		uint8_t *p = (uint8_t *)FrameArena();
		const int16_t other = d1.ofs[0] != 0 ? 0 : 0x140;
		FrameArena() += 0xC;
		const uint32_t tpage = *(const uint32_t *)&d1.tpage & 0xFFFF;
		d1.ofs[0] = other;
		const uint32_t dtd = d1.dtd, dfe = d1.dfe;
		rect[1] = 0;
		rect[0] = 0;
		SetDrawMode(p, (int32_t)dfe, (int32_t)dtd, (int32_t)tpage, rect);
		InsertPrimAutoDepth(ot + 4, p);
		DrawEnv d2;
		p = (uint8_t *)FrameArena();
		FrameArena() += 0x40;
		SetDefDrawEnv(&d2, d1.ofs[0], d1.ofs[1], 0x140, 0xD8);
		d2.tpage = d1.tpage;
		d2.dtd = 0;
		d2.dfe = d1.dfe;
		d2.isbg = 0;
		d2.b0 = 0;
		d2.g0 = 0;
		d2.r0 = 0;
		SetDrawEnv(p, &d2);
		InsertPrimAutoDepth(ot + 4, p);
		CaptureModels(ot);
		p = (uint8_t *)FrameArena();
		FrameArena() += 0x40;
		SetDefDrawEnv(&d2, 0x180, 0x100, 0x140, 0xD8);
		d2.tpage = d1.tpage;
		d2.dtd = d1.dtd;
		d2.dfe = d1.dfe;
		d2.isbg = 1;
		d2.b0 = 0;
		d2.g0 = 0;
		d2.r0 = 0;
		SetDrawEnv(p, &d2);
		InsertPrimAutoDepth(ot + 0x4484, p);
		return 0;
	}

	static int32_t FadeLevel(int32_t t, int32_t in, int32_t hold, int32_t out)
	{
		const int32_t din = in != 0 ? in : 1;
		const int32_t dout = out != 0 ? out : 1;
		if (t < in) return shl32(t + 1, 12) / din;
		if (t < in + hold) return 0x1000;
		return (shl32(in - t + hold, 12) - 0x1000) / dout + 0x1000;
	}
	static void FadeTile(int32_t t, int32_t r, int32_t g, int32_t b, int32_t in, int32_t hold, int32_t out)
	{
		const int32_t level = FadeLevel(t, in, hold, out);
		FrameArena() = Tile(r, g, b, level, FrameArena());
	}

	static void CameraKeys(const int16_t *k0, const int16_t *k1, int32_t za, int32_t zb, int32_t i, int32_t n)
	{
		const int32_t d = n - 1;
		var<int16_t>(game_addr(game::g_BattleCam_Roll)) = (int16_t)(mul32(zb - za, i) / d + za);
		const int32_t t = shl32(i, 12) / d;
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

	static void CameraOrbit(int32_t pitch, int32_t yaw, int32_t s)
	{
		uint8_t frame[0x48];
		int16_t *v = (int16_t *)frame;
		Mat4x3 *m = (Mat4x3 *)(frame + 8);
		RotY(yaw, m);
		if (pitch != 0)
		{
			Mat4x3 *rx = (Mat4x3 *)(frame + 0x28);
			RotX(pitch, rx);
			MatMul3(rx, m, m);
		}
		const int16_t ly = var<int16_t>(game_addr(game::unk_B8B7FA));
		var<int16_t>(game_addr(game::unk_B8B7F2)) = (int16_t)((mul32(var<int16_t>(game_addr(game::unk_B8B7F2)) - ly, s) >> 12) + ly);
		v[1] = (int16_t)(mul32(var<int16_t>(game_addr(game::unk_B8B7F2)) - ly, s) >> 12);
		const int32_t lz = var<int16_t>(game_addr(game::Battle_Camera_LookAt_Y)), lx = var<int16_t>(game_addr(game::Battle_Camera_LookAt_XZ_s16));
		v[2] = (int16_t)(mul32(var<int16_t>(game_addr(game::Battle_Camera_world_Y)) - lz, s) >> 12);
		v[0] = (int16_t)(mul32(var<int16_t>(game_addr(game::Battle_Camera_world_XZ_s16)) - lx, s) >> 12);
		MatVec(m, v, v);
		var<int16_t>(game_addr(game::Battle_Camera_world_XZ_s16)) = (int16_t)(var<int16_t>(game_addr(game::Battle_Camera_LookAt_XZ_s16)) + v[0]);
		var<int16_t>(game_addr(game::unk_B8B7F2)) = (int16_t)(var<int16_t>(game_addr(game::unk_B8B7FA)) + v[1]);
		var<int16_t>(game_addr(game::Battle_Camera_world_Y)) = (int16_t)(var<int16_t>(game_addr(game::Battle_Camera_LookAt_Y)) + v[2]);
	}

	static void DamageWave()
	{
		const uint32_t n = (uint32_t)((ActionData *)F<uint8_t *>(Ctx(), 4))->target_count >> 3;
		for (uint32_t k = n; k != 0; k--)
		{
			ApplyActionResultToTarget(TargetCursor());
			TargetCursor() += 0x18;
		}
	}

	static void BonePoint(uint8_t *c, const int16_t *pt, int16_t *out)
	{
		Mat4x3 m;
		uint8_t *skel = **(uint8_t ***)(c + 0x64);
		BuildBoneMatricesFromPose(c + 0x60);
		ComposeAffineTransform((const Mat4x3 *)(c + 0x40), (const Mat4x3 *)(skel + 0x20 + mul32(pt[3], 0x30)), &m);
		GteSetRotMatrixCtrl(&m);
		GteSetTransVectorCtrl(&m);
		GteLoadV0(pt);
		GteMVMVA_RotV0Tr();
		GteStoreIR123(out);
	}

	static void FlashAt(uint8_t *c, const int16_t *pt, int32_t n)
	{
		int16_t v[4];
		BonePoint(c, pt, v);
		SmallNode *t = (SmallNode *)AddTaskToQueue(QB(), game_addr(ORIG_FlashTask));
		if (t)
		{
			t->frame = 0;
			t->pos[0] = v[0];
			t->pos[1] = v[1];
			t->pos[2] = v[2];
			t->size = 0x1000;
		}
		while (n-- != 0)
		{
			t = (SmallNode *)AddTaskToQueue(QB(), game_addr(ORIG_DebrisTask));
			if (!t) break;
			t->frame = 0xF;
			t->pos[0] = v[0];
			t->pos[1] = v[1];
			t->pos[2] = v[2];
			t->size = 0x1000;
			t->vel[0] = (int16_t)((CrtRand() & 0x7F) - 0x40);
			t->vel[1] = (int16_t)(0x80 - (CrtRand() & 0x7F));
			t->vel[2] = (int16_t)((CrtRand() & 0x7F) + 0x7F);
			t->ang[0] = (int8_t)CrtRand();
			t->ang[1] = (int8_t)CrtRand();
			t->dang[0] = (int8_t)((CrtRand() & 0x1F) - 0x10);
			t->dang[1] = (int8_t)((CrtRand() & 0x1F) - 0x10);
			t->gravity = (int8_t)0xEC;
		}
	}

	static DiveNode *DiveSpawn(uint32_t task, int32_t a, int32_t b, int32_t k)
	{
		DiveNode *t = (DiveNode *)AddTaskToQueue(QC(), task);
		if (!t) return nullptr;
		DecodeModelPrimLayout(game_addr(LAYOUT_Dive), t->layout, 0x158);
		int16_t ang[4];
		ang[0] = (int16_t)a;
		ang[1] = (int16_t)b;
		ang[2] = 0;
		BuildRotationMatrixFromAngles(ang, &t->m);
		t->m.t[0] = Centre()[0] + mul32(t->m.m[0][2], k);
		t->m.t[1] = Centre()[1] + mul32(t->m.m[1][2], k);
		t->counter = 0;
		t->m.t[2] = Centre()[2] + mul32(t->m.m[2][2], k);
		return t;
	}

	static void __cdecl PartCallback(prim::Layout *l, prim::Record *r, int arg_)
	{
		PrimArg *blk = (PrimArg *)arg_;
		if (((int32_t)r->scale[2] | *(const int32_t *)r->scale) == 0) return;
		if (r->a >= 0x1000 && *(const uint32_t *)r->rgb == 0) return;
		uint8_t frame[0x38];
		int16_t *v = (int16_t *)frame;
		int32_t *s = (int32_t *)(frame + 8);
		Mat4x3 *m = (Mat4x3 *)(frame + 0x18);
		uint8_t *h = (uint8_t *)FieldAlloc(0x6C);
		const uint32_t data = (uint32_t)l->data;
		const uint32_t mdl = data + *(const uint32_t *)(data + (int32_t)(int16_t)r->flags_lo * 4 + 8);
		F<uint32_t>(h, 0) = mdl;
		const int16_t b0 = r->b0, b1 = r->b1;
		const uint32_t nv = *(const uint32_t *)(mdl + 4);
		if (b0 == b1 || r->b == 0)
		{
			if (b0 == 0) F<uint32_t>(h, 4) = mdl + 0xC;
			else F<uint32_t>(h, 4) = mdl + (uint32_t)mul32((int32_t)nv, b0) * 8 + 0xC;
		}
		else if (r->b == 0x1000)
		{
			if (b1 == 0) F<uint32_t>(h, 4) = mdl + 0xC;
			else F<uint32_t>(h, 4) = mdl + (uint32_t)mul32((int32_t)nv, b1) * 8 + 0xC;
		}
		else
		{
			BlendVertexFrames(mdl, b0, b1, r->b, blk->morph);
			F<uint32_t>(h, 4) = blk->morph;
		}
		RotationFromAngles(r->rot, m);
		const uint32_t flags = r->flags;
		v[0] = r->pos[0];
		v[1] = r->pos[1];
		v[2] = r->pos[2];
		int32_t x, y, z;
		if (flags & 0x200)
		{
			x = v[0];
			y = v[1];
			z = v[2];
		}
		else
		{
			GteSetRotMatrixCtrl(&blk->m);
			GteLoadV0(v);
			GteMVMVA_RotV0();
			GteReadMAC123(m->t);
			MatrixMultiply(&blk->m, m);
			x = m->t[0];
			y = m->t[1];
			z = m->t[2];
		}
		m->t[0] = x + blk->m.t[0];
		m->t[1] = y + blk->m.t[1];
		m->t[2] = z + blk->m.t[2];
		if (*(const uint32_t *)r->scale != 0x10001000 || r->scale[2] != 0x1000)
		{
			s[0] = r->scale[0];
			s[1] = r->scale[1];
			s[2] = r->scale[2];
			Scale3DMatrix(m, s);
		}
		GteSetRotMatrix(m);
		GteSetTransVector(m);
		F<uint32_t>(h, 0x1C) = (r->flags & 0x4000) ? 0x2000 : 0x2030;
		F<int32_t>(h, 0xC) = r->a;
		if (r->a != 0)
		{
			F<uint32_t>(h, 0x1C) |= 0xC0;
			F<uint32_t>(h, 8) = *(const uint32_t *)r->rgb;
		}
		F<int32_t>(h, 0x18) = blk->depth;
		F<uint32_t>(h, 0x20) = 0;
		blk->cursor = RenderPrimSet(h, OTBase() + 0x44, 2, blk->cursor);
		FieldFree(0x6C);
	}

	static int Play(uint8_t *layout, int32_t x, int32_t y, int32_t z, int16_t depth, uint32_t *cursor)
	{
		PrimArg a;
		RotY(0, &a.m);
		a.m.t[0] = x;
		a.m.t[1] = y;
		a.m.t[2] = z;
		ComposeAffineTransform(&Camera(), &a.m, &a.m);
		a.morph = TexBase() + 0x1888C;
		a.depth = depth;
		a.cursor = *cursor;
		const int left = prim::play((prim::Layout *)layout, PartCallback, (int)&a, 0);
		*cursor = a.cursor;
		return left;
	}

	static void SpritesUpdate()
	{
		uint8_t *p = (uint8_t *)Sprites() + 0xA;
		for (int n = 0x200; n != 0; n--, p += 0x18)
		{
			if (*(int16_t *)p == 0) continue;
			const int32_t ax = (int8_t)p[0xA];
			const int16_t vx = *(int16_t *)(p + 2), vy = *(int16_t *)(p + 4);
			*(int16_t *)(p - 6) = (int16_t)(*(int16_t *)(p - 6) + vx);
			*(int16_t *)(p - 4) = (int16_t)(*(int16_t *)(p - 4) + vy);
			(*(int16_t *)p)--;
			*(int16_t *)(p + 2) = (int16_t)(vx + ax * 4);
			const int32_t ay = (int8_t)p[0xB];
			const int16_t dz = (int16_t)((int16_t)((int8_t)p[0xC] * 4) + *(int16_t *)(p + 6));
			*(int16_t *)(p + 4) = (int16_t)(vy + ay * 4);
			*(int16_t *)(p - 2) = (int16_t)(*(int16_t *)(p - 2) + dz);
		}
	}

	static uint32_t Flipbook(uint8_t *h, uint32_t ot, int32_t mode, uint32_t cursor)
	{
		uint8_t *pk = (uint8_t *)cursor;
		int32_t count = F<int32_t>(h, 0x30);
		const uint8_t *rec = F<const uint8_t *>(h, 0x2C);
		if (count <= 0) return (uint32_t)pk;
		for (; count != 0; count--, rec += 0x14)
		{
			F<int16_t>(h, 0x72) = 0;
			F<int16_t>(h, 0x70) = 0;
			F<int16_t>(h, 0x6E) = 0;
			F<int16_t>(h, 0x6A) = 0;
			F<int16_t>(h, 0x68) = 0;
			F<int16_t>(h, 0x66) = 0;
			F<int16_t>(h, 0x74) = 0x1000;
			const int32_t tp = *(const int16_t *)(rec + 0xE);
			F<int16_t>(h, 0x64) = *(const int16_t *)(rec + 0x10);
			F<int16_t>(h, 0x6A) = 0;
			F<int16_t>(h, 0x66) = 0;
			F<int16_t>(h, 0x6C) = *(const int16_t *)(rec + 0x12);
			const uint32_t w = rec[0], hh = rec[2];
			const int32_t w8 = (int32_t)(w << 3), h8 = (int32_t)(hh << 3);
			F<int16_t>(h, 0x54) = (int16_t)-w8;
			F<int16_t>(h, 0x44) = (int16_t)-w8;
			F<int16_t>(h, 0x5C) = (int16_t)w8;
			F<int16_t>(h, 0x4C) = (int16_t)w8;
			F<int16_t>(h, 0x4E) = (int16_t)-h8;
			F<int16_t>(h, 0x46) = (int16_t)-h8;
			F<int16_t>(h, 0x5E) = (int16_t)h8;
			F<int16_t>(h, 0x56) = (int16_t)h8;
			F<int32_t>(h, 0x78) = shl32(*(const int16_t *)(rec + 8), 4) + w8;
			F<int32_t>(h, 0x80) = 0;
			F<int32_t>(h, 0x7C) = shl32(*(const int16_t *)(rec + 0xA), 4) + h8;
			GteSetRotMatrixCtrl((const Mat4x3 *)(h + 0x84));
			GteLoadIRColumn(h + 0x64);
			GteMVMVA_RotIR();
			F<uint32_t>(pk, 0) = 0x9000000;
			pk[4] = rec[1];
			pk[5] = rec[1];
			pk[6] = rec[1];
			pk[7] = rec[3];
			GteStoreIRColumn(h + 0x64);
			GteLoadIRColumn(h + 0x66);
			GteMVMVA_RotIR();
			F<uint32_t>(pk, 0xC) = *(const uint32_t *)(rec + 4);
			GteStoreIRColumn(h + 0x66);
			GteLoadIRColumn(h + 0x68);
			GteMVMVA_RotIR();
			pk[0x1C] = rec[4];
			GteStoreIRColumn(h + 0x68);
			GteSetTransVectorCtrl((const Mat4x3 *)(h + 0x84));
			GteLoadV0Dwords(h + 0x78);
			GteMVMVA_RotV0Tr();
			pk[0x15] = rec[5];
			GteReadMAC123((int32_t *)(h + 0x78));
			GteSetRotMatrixCtrl((const Mat4x3 *)(h + 0x64));
			GteSetTransVectorCtrl((const Mat4x3 *)(h + 0x64));
			GteLoadV012(h + 0x44, h + 0x4C, h + 0x54);
			GteRTPT();
			uint32_t u = rec[4] + w;
			if (u >= 0x100) u = 0xFF;
			pk[0x24] = (uint8_t)u;
			pk[0x14] = (uint8_t)u;
			GteReadFLAG(h + 0x40);
			if (F<uint32_t>(h, 0x40) & 0x60000) continue;
			F<uint16_t>(pk, 0x16) = (uint16_t)(tp & 0x1FF);
			GteReadSXY012(pk + 8, pk + 0x10, pk + 0x18);
			GteLoadV0(h + 0x5C);
			GteRTPS();
			uint32_t vv = rec[5] + hh;
			if (vv >= 0x100) vv = 0xFF;
			pk[0x25] = (uint8_t)vv;
			pk[0x1D] = (uint8_t)vv;
			GteReadSXY2(pk + 0x20);
			if (h[0x25] & 1)
			{
				uint8_t *p2 = pk + 0x28;
				const uint32_t slot = ot + (uint32_t)mode * 4;
				F<uint32_t>(p2, 0) = 0x1000000;
				F<uint32_t>(p2, 4) = 0xE1000220;
				InsertPrimAutoDepth(slot, p2);
				InsertPrimAutoDepth(slot, pk);
				pk = p2 + 8;
			}
			else
			{
				InsertPrimAutoDepth(ot + (uint32_t)mode * 4, pk);
				pk += 0x28;
			}
		}
		return (uint32_t)pk;
	}

	static void SpritesDraw()
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		F<int16_t>(h, 0x60) = 0;
		F<int16_t>(h, 0x58) = 0;
		F<int16_t>(h, 0x50) = 0;
		F<int16_t>(h, 0x48) = 0;
		uint8_t *s = (uint8_t *)Sprites();
		for (int n = 0x200; n != 0; n--, s += 0x18)
		{
			if (F<int16_t>(s, 0xA) == 0) continue;
			GteSetRotMatrixCtrl(&Camera());
			GteSetTransVectorCtrl(&Camera());
			GteLoadV0(s + 4);
			GteMVMVA_RotV0Tr();
			GteReadMAC123((int32_t *)(h + 0x98));
			F<uint32_t>(h, 0x84) = 0;
			F<uint32_t>(h, 0x88) = 0;
			F<uint32_t>(h, 0x8C) = 0;
			F<uint32_t>(h, 0x90) = 0;
			F<uint32_t>(h, 0x94) = 0;
			const int16_t sc = F<int16_t>(s, 0x12);
			F<int16_t>(h, 0x94) = sc;
			F<int16_t>(h, 0x8C) = sc;
			F<int16_t>(h, 0x84) = sc;
			const uint32_t seq = F<uint32_t>(s, 0);
			F<uint32_t>(h, 0) = seq;
			const int32_t cnt = *(const int16_t *)(seq + 8);
			const int32_t rem = (F<int16_t>(s, 0xA) - 1) % cnt;
			const uint32_t fr = seq + (int32_t)*(const int16_t *)(seq + (uint32_t)(cnt - rem) * 2 + 8);
			F<uint32_t>(h, 0x2C) = fr;
			int32_t c = *(const int32_t *)fr;
			F<int32_t>(h, 0x30) = c;
			if (c < 0)
			{
				c &= 0x7FFFFFFF;
				h[0x25] |= 1;
				F<int32_t>(h, 0x30) = c;
			}
			F<uint32_t>(h, 0x2C) += 4;
			Cursor() = Flipbook(h, OTBase() + 0x44, 2, Cursor());
		}
		FieldFree(0xB4);
	}

	static void PModelsUpdate()
	{
		uint8_t *p = (uint8_t *)PModels();
		for (int n = 0x80; n != 0; n--, p += 0x30)
		{
			if (p[0] == 0) continue;
			const uint8_t *k = (const uint8_t *)(game_addr(PART_KINDS) + ((uint32_t)p[1] << 4));
			if (--p[0] == 0)
			{
				if (k[7] == 0) continue;
				Sprite *s = SpriteAlloc();
				if (!s) continue;
				s->pos[0] = F<int16_t>(p, 0x24);
				s->pos[1] = F<int16_t>(p, 0x28);
				s->pos[2] = F<int16_t>(p, 0x2C);
				s->scale = 0x800;
				s->life = 0x10;
				s->seq = game_addr(SEQ_Sparkle);
				s->vel[0] = (int16_t)(F<int16_t>(p, 0x12) >> 7);
				s->vel[1] = (int16_t)(F<int16_t>(p, 0x18) >> 7);
				s->vel[2] = (int16_t)(F<int16_t>(p, 0x1E) >> 7);
				F<uint32_t>(s, 0x14) = 0;
				continue;
			}
			const int16_t vy = F<int16_t>(p, 0xC);
			F<int16_t>(p, 2) = (int16_t)(F<int16_t>(p, 2) + *(const int16_t *)(k + 8));
			F<int16_t>(p, 4) = (int16_t)(F<int16_t>(p, 4) + *(const int16_t *)(k + 0xA));
			F<int16_t>(p, 6) = (int16_t)(F<int16_t>(p, 6) + *(const int16_t *)(k + 0xC));
			const int16_t vx = F<int16_t>(p, 0xA);
			F<int16_t>(p, 8) = (int16_t)(F<int16_t>(p, 8) + *(const int16_t *)(k + 0xE));
			F<int32_t>(p, 0x24) += vx;
			F<int32_t>(p, 0x28) += vy;
			const int16_t vz = F<int16_t>(p, 0xE);
			F<int32_t>(p, 0x2C) += vz;
			F<int16_t>(p, 0xA) = (int16_t)(vx - div80(F<int16_t>(p, 0x12)));
			F<int16_t>(p, 0xC) = (int16_t)(vy - div80(F<int16_t>(p, 0x18)));
			F<int16_t>(p, 0xE) = (int16_t)(vz - div80(F<int16_t>(p, 0x1E)));
		}
	}

	static void PModelsDraw()
	{
		const uint32_t ot = OTBase() + 0x44;
		uint8_t *pk = (uint8_t *)Cursor();
		uint8_t *h = (uint8_t *)FieldAlloc(0x4C);
		RotY(0, h + 0x20);
		uint8_t *p = (uint8_t *)PModels();
		for (int n = 0x80; n != 0; n--, p += 0x30)
		{
			if (p[0] == 0) continue;
			const uint8_t *k = (const uint8_t *)(game_addr(PART_KINDS) + ((uint32_t)p[1] << 4));
			const uint32_t model = *(const uint32_t *)k;
			F<int16_t>(h, 0x20) = F<int16_t>(p, 4);
			const uint8_t *prim = (const uint8_t *)(model + *(const uint32_t *)model + 0x10);
			F<int16_t>(h, 0x28) = F<int16_t>(p, 6);
			F<int16_t>(h, 0x30) = F<int16_t>(p, 8);
			MatMul3(p + 0x10, h + 0x20, h);
			F<int32_t>(h, 0x14) = F<int32_t>(p, 0x24);
			F<int32_t>(h, 0x18) = F<int32_t>(p, 0x28);
			F<int32_t>(h, 0x1C) = F<int32_t>(p, 0x2C);
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
			GteSetRotMatrixCtrl((const Mat4x3 *)h);
			GteSetTransVectorCtrl((const Mat4x3 *)h);
			GteSetFarColor(k[4], k[5], k[6]);
			GteLoadV012((const void *)(model + (uint32_t)*(const uint16_t *)(prim + 4) * 4 + 8), (const void *)(model + (uint32_t)*(const uint16_t *)(prim + 6) * 4 + 8),
				(const void *)(model + (uint32_t)*(const uint16_t *)(prim + 8) * 4 + 8));
			GteRTPT();
			F<uint32_t>(pk, 0) = 0x9000000;
			F<uint32_t>(pk, 4) = *(const uint32_t *)prim;
			F<uint32_t>(pk, 0xC) = *(const uint32_t *)(prim + 0xC);
			F<uint32_t>(pk, 0x14) = *(const uint32_t *)(prim + 0x10);
			const int32_t uv3 = *(const int32_t *)(prim + 0x14);
			F<int32_t>(pk, 0x1C) = uv3;
			F<int32_t>(pk, 0x24) = uv3 >> 16;
			GteReadFLAG(h + 0x40);
			if (F<uint32_t>(h, 0x40) & 0x60000) continue;
			GteReadSXY012(pk + 8, pk + 0x10, pk + 0x18);
			GteLoadV0((const void *)(model + (uint32_t)*(const uint16_t *)(prim + 0xA) * 4 + 8));
			GteRTPS();
			GteReadSXY2(pk + 0x20);
			GteAVSZ4();
			GteReadOTZWord(h + 0x44);
			GteLoadRGBC(pk + 4);
			const int32_t fade = F<int16_t>(p, 2);
			F<int32_t>(h, 0x48) = fade;
			GteSetIR0(fade);
			GteDPCS();
			GteStoreRGB2(pk + 4);
			InsertPrimAutoDepth(ot + (uint32_t)(F<int32_t>(h, 0x44) >> 2) * 4, pk);
			pk += 0x28;
		}
		FieldFree(0x4C);
		Cursor() = (uint32_t)pk;
	}

	static uint32_t Stars(int32_t depth, int32_t x, int32_t y, int32_t z, int32_t limit, uint32_t cursor)
	{
		const uint8_t *st = (const uint8_t *)(TexBase() + 0x7C04);
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
		for (uint32_t g = game_addr(STAR_GROUPS) + 0xE; g < game_addr(game::unk_14AF51E); g += 0x10)
		{
			RotX(*(const int16_t *)(g - 2), h + 0x40);
			RotY(*(const int16_t *)g, h + 0x20);
			MatMul3(h + 0x20, h + 0x40, h + 0x40);
			MatMul3(h, h + 0x40, h + 0x40);
			GteSetRotMatrix((const Mat4x3 *)(h + 0x40));
			GteSetTransVector(v);
			for (int n = 0x80; n != 0; n--, st += 0x10)
			{
				if (*(const int16_t *)(st + 6) >= limit) continue;
				GteLoadV0(st);
				GteRTPS();
				GteReadSXY2(pk + 8);
				GteReadOTZ(h + 0x60);
				if (F<int32_t>(h, 0x60) <= 0) continue;
				if (st[0xB] != 0)
				{
					const int32_t l = (uint16_t)((uint16_t)st[0xB] << 4);
					GteSetIR0(l);
					GteLoadIRBytes(st + 8);
					GteGPF();
					GteSetIR0(0x1000 - l);
					GteLoadIRBytes(st + 0xC);
					GteGPL();
					GteStoreIRBytes(pk + 4);
				}
				else F<uint32_t>(pk, 4) = *(const uint32_t *)(st + 8);
				F<uint32_t>(pk, 0) = 0x2000000;
				pk[7] = 0x68;
				int32_t d = (F<int32_t>(h, 0x60) >> 2) + depth;
				F<int32_t>(h, 0x60) = d;
				if (d < 0) F<int32_t>(h, 0x60) = 0;
				InsertPrimAutoDepth(OTBase() + (uint32_t)F<int32_t>(h, 0x60) * 4 + 0x44, pk);
				pk += 0xC;
			}
		}
		FieldFree(0x64);
		return (uint32_t)pk;
	}

	static void StarsTwinkle()
	{
		uint8_t *p = (uint8_t *)(TexBase() + 0x7C0F);
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
		for (uint32_t g = game_addr(STAR_GROUPS) + 0xC; g < game_addr(game::unk_14AF51C); g += 0x10)
		{
			*(int16_t *)g = (int16_t)(*(int16_t *)g + *(const int16_t *)(g - 4));
			*(int16_t *)(g + 2) = (int16_t)(*(int16_t *)(g + 2) + *(const int16_t *)(g - 2));
		}
	}

	static void FlashDraw(const SmallNode *t)
	{
		TransformCameraByShadowRotation(t->pos, t->size, -(t->size >> 2));
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		F<uint32_t>(h, 0) = game_addr(SEQ_Flash);
		F<int16_t>(h, 4) = (int16_t)t->frame;
		F<int16_t>(h, 0x24) = 0;
		Cursor() = InitEffectSequenceFromData(h, OTBase() + 0x44, 2, Cursor());
		FieldFree(0xB4);
	}

	static uint32_t __cdecl FlashTask(TaskNode *n)
	{
		SmallNode *t = (SmallNode *)n;
		FlashDraw(t);
		t->frame = (int8_t)(t->frame + 1);
		return t->frame < 0xC ? 0 : TASK_END;
	}

	static void DebrisDraw(const int16_t *pos, int16_t ax, int16_t ay)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x80);
		F<int16_t>(h, 0) = ax;
		Mat4x3 *m = (Mat4x3 *)(h + 8);
		F<int16_t>(h, 2) = ay;
		F<int16_t>(h, 4) = 0;
		BuildRotationMatrixFromAngles((const int16_t *)h, m);
		m->t[0] = pos[0];
		m->t[1] = pos[1];
		m->t[2] = pos[2];
		ComposeAffineTransform(&Camera(), m, m);
		GteSetRotMatrix(m);
		GteSetTransVector(m);
		F<uint32_t>(h, 0x44) = 0;
		F<uint32_t>(h, 0x28) = game_addr(MDL_Debris);
		Cursor() = RenderPrimModel(h + 0x28, OTBase() + 0x44, 2, Cursor());
		FieldFree(0x80);
	}

	static uint32_t __cdecl DebrisTask(TaskNode *n)
	{
		SmallNode *t = (SmallNode *)n;
		DebrisDraw(t->pos, (int16_t)((int16_t)t->ang[0] << 4), (int16_t)((int16_t)t->ang[1] << 4));
		if (--t->frame == 0) return TASK_END;
		const int16_t vx = t->vel[0], vy = t->vel[1];
		t->pos[0] = (int16_t)(t->pos[0] + vx);
		t->pos[1] = (int16_t)(t->pos[1] + vy);
		if (t->pos[1] >= 0) t->vel[1] = (int16_t)-vy;
		else t->vel[1] = (int16_t)((int16_t)t->gravity + vy);
		t->ang[0] = (int8_t)(t->ang[0] + t->dang[0]);
		t->pos[2] = (int16_t)(t->pos[2] + t->vel[2]);
		t->ang[1] = (int8_t)(t->ang[1] + t->dang[1]);
		return 0;
	}

	static void RandRotZMul(int16_t *m, const void *dir)
	{
		RotZ(CrtRand(), m);
		MatMul3(dir, m, m);
	}

	static void RandTiltMul(int16_t *out, const void *dir)
	{
		Mat4x3 rz, rx;
		RotZ(CrtRand(), &rz);
		RotX(-0x200, &rx);
		MatMul3(&rz, &rx, &rx);
		MatMul3(dir, &rx, out);
	}

	static void TrailDraw(const Mat4x3 *m, uint32_t *cursor)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x78);
		ComposeAffineTransform(&Camera(), m, (Mat4x3 *)h);
		GteSetRotMatrix((const Mat4x3 *)h);
		GteSetTransVector((const Mat4x3 *)h);
		F<uint32_t>(h, 0x3C) = 0x30;
		F<uint32_t>(h, 0x20) = game_addr(MDL_Trail);
		*cursor = RenderPrimModel(h + 0x20, OTBase() + 0x44, 2, *cursor);
		FieldFree(0x78);
	}

	static void PModelBurst(const Mat4x3 *dir, const int32_t *p, int kind, bool tilt, int count)
	{
		for (int left = count;;)
		{
			PModel *s = PModelAlloc();
			if (!s) return;
			if (!tilt)
			{
				RandRotZMul(s->m, dir);
			}
			else RandTiltMul(s->m, dir);
			s->pad22 = 0;
			s->pos[0] = p[0];
			s->pos[1] = p[1];
			s->pos[2] = p[2];
			s->vel[0] = (int16_t)div6(s->m[1]);
			s->vel[1] = (int16_t)div6(s->m[4]);
			s->kind = (uint8_t)kind;
			s->vel[2] = (int16_t)div6(s->m[7]);
			s->life = 0xD;
			s->fade = 0;
			s->scale[2] = 0x800;
			s->scale[0] = 0x800;
			s->scale[1] = 0x2000;
			if (--left == 0) return;
		}
	}

	static void SparkleBurst(const void *m, const int32_t *p, int32_t count, int16_t z)
	{
		for (Sprite *s = SpriteAlloc(); s;)
		{
			s->pos[0] = (int16_t)p[0];
			s->pos[1] = (int16_t)p[1];
			s->pos[2] = (int16_t)p[2];
			s->scale = 0x800;
			s->life = 0x10;
			s->seq = game_addr(SEQ_Sparkle);
			const int32_t r = CrtRand();
			s->vel[0] = (int16_t)ComputeCos(r);
			s->vel[1] = (int16_t)ComputeSin(r);
			s->vel[2] = z;
			MatVec(m, s->vel, s->vel);
			const int32_t sp = (CrtRand() & 0x7F) + 0x80;
			F<uint32_t>(s, 0x14) = 0;
			s->vel[0] = (int16_t)(mul32(s->vel[0], sp) >> 12);
			s->vel[1] = (int16_t)(mul32(s->vel[1], sp) >> 12);
			s->vel[2] = (int16_t)(mul32(s->vel[2], sp) >> 12);
			if (--count == 0) break;
			s = SpriteAlloc();
		}
	}

	static uint32_t __cdecl DiveTask(TaskNode *n)
	{
		DiveNode *d = (DiveNode *)n;
		if (d->counter < 0xC)
		{
			TrailDraw(&d->m, &FrameArena());
			const int16_t c = d->counter;
			if (c > 3 && c < 8)
			{
				int left = 8;
				for (Sprite *s = SpriteAlloc(); s;)
				{
					s->pos[0] = (int16_t)d->m.t[0];
					s->pos[1] = (int16_t)d->m.t[1];
					s->pos[2] = (int16_t)d->m.t[2];
					s->scale = 0x800;
					s->life = 0x10;
					s->seq = game_addr(SEQ_Sparkle);
					const int32_t r = CrtRand();
					s->vel[0] = (int16_t)ComputeCos(r);
					s->vel[1] = (int16_t)ComputeSin(r);
					s->vel[2] = 0;
					MatVec(&d->m, s->vel, s->vel);
					const int32_t sp = (CrtRand() & 0x200) + 0x200;
					const int32_t fw = CrtRand() & 0xFFF;
					const int16_t vx = s->vel[0];
					s->pos[0] = (int16_t)(s->pos[0] + (int16_t)((mul32(vx, sp) >> 12) + (mul32(d->m.m[0][2], fw) >> 12)));
					const int16_t vy = s->vel[1];
					s->pos[1] = (int16_t)(s->pos[1] + (int16_t)((mul32(d->m.m[1][2], fw) >> 12) + (mul32(vy, sp) >> 12)));
					const int16_t vz = s->vel[2];
					const int32_t dz = (mul32(vz, sp) >> 12) + (mul32(d->m.m[2][2], fw) >> 12);
					F<uint32_t>(s, 0x14) = 0;
					s->vel[0] = (int16_t)(vx >> 6);
					s->vel[1] = (int16_t)(vy >> 6);
					s->pos[2] = (int16_t)(s->pos[2] + (int16_t)dz);
					s->vel[2] = (int16_t)(vz >> 6);
					if (--left == 0) break;
					s = SpriteAlloc();
				}
			}
			d->m.t[1] -= d->m.m[1][2];
			d->m.t[0] -= d->m.m[0][2];
			d->m.t[2] -= d->m.m[2][2];
		}
		const int16_t c = d->counter;
		if (c < 6)
		{
			d->counter++;
			return 0;
		}
		PrimArg a;
		F<uint32_t>(&a, 0) = F<uint32_t>(d, 0xC);
		F<uint32_t>(&a, 8) = F<uint32_t>(d, 0x14);
		F<uint32_t>(&a, 0xC) = F<uint32_t>(d, 0x18);
		F<uint32_t>(&a, 4) = F<uint32_t>(d, 0x10);
		F<uint16_t>(&a, 0x10) = F<uint16_t>(d, 0x1C);
		a.m.t[0] = sdiv_pow2(d->m.m[0][2], 2) + Centre()[0];
		a.m.t[1] = sdiv_pow2(d->m.m[1][2], 2) + Centre()[1];
		a.m.t[2] = sdiv_pow2(d->m.m[2][2], 2) + Centre()[2];
		if (c <= 8 && !(c & 1))
		{
			PModelBurst(&d->m, a.m.t, 0, false, 4);
			PModelBurst(&d->m, a.m.t, 1, true, 4);
		}
		if (d->counter == 6)
		{
			SparkleBurst(&d->m, a.m.t, 0x18, 0x2000);
			DamageWave();
		}
		ComposeAffineTransform(&Camera(), &a.m, &a.m);
		a.morph = TexBase() + 0x1888C;
		a.depth = 0;
		a.cursor = FrameArena();
		const int left = prim::play((prim::Layout *)d->layout, PartCallback, (int)&a, 0);
		FrameArena() = a.cursor;
		if (left == 0) return TASK_END;
		d->counter++;
		return 0;
	}

	static uint32_t __cdecl LastDiveTask(TaskNode *n)
	{
		DiveNode *d = (DiveNode *)n;
		if (d->counter < 0xC)
		{
			TrailDraw(&d->m, &Cursor());
			d->m.t[0] += -((d->m.m[0][2] - (d->m.m[0][2] >> 15)) >> 1);
			d->m.t[1] += -((d->m.m[1][2] - (d->m.m[1][2] >> 15)) >> 1);
			d->m.t[2] += -((d->m.m[2][2] - (d->m.m[2][2] >> 15)) >> 1);
		}
		if (d->counter >= 3)
		{
			Mat4x3 l;
			RotY(0, &l);
			l.t[2] = Centre()[2];
			const int16_t c = d->counter;
			const uint32_t ph = (uint32_t)((int32_t)c - 3);
			l.t[0] = Centre()[0];
			l.t[1] = Centre()[1];
			if (ph <= 9 && !(c & 3))
			{
				const int kind = (int)((ph >> 2) + 2);
				for (int left = 0x30;;)
				{
					PModel *s = PModelAlloc();
					if (!s) break;
					RandRotZMul(s->m, &l);
					s->pad22 = 0;
					const int32_t spread = (CrtRand() & 0x7FF) - 0x400;
					const int32_t mx = s->m[1];
					s->pos[0] = sdiv_pow2(mul32(mx, spread), 12) + l.t[0];
					const int32_t my = s->m[4];
					s->pos[1] = sdiv_pow2(mul32(my, spread), 12) + l.t[1];
					const int32_t mz = s->m[7];
					s->life = 0xD;
					s->fade = 0;
					s->pos[2] = sdiv_pow2(mul32(mz, spread), 12) + l.t[2];
					s->kind = (uint8_t)kind;
					s->scale[1] = 0x2000;
					s->vel[0] = (int16_t)div6(mx);
					s->vel[1] = (int16_t)div6(my);
					s->scale[2] = 0x800;
					s->vel[2] = (int16_t)div6(mz);
					s->scale[0] = 0x800;
					if (--left == 0) break;
				}
			}
			if (d->counter == 3)
			{
				PModelBurst(&l, l.t, 1, true, 0x10);
				SparkleBurst(&l, l.t, 0x10, 0x2000);
				RotX(0x400, &d->m);
			}
		}
		const int16_t c = d->counter;
		d->counter = (int16_t)(c + 1);
		return c < 0xC ? 0 : TASK_END;
	}

	static void SetKeys(int16_t *k, int32_t a0, int32_t a1, int32_t a2, int32_t a3, int32_t a4, int32_t a5)
	{
		k[0] = (int16_t)a0; k[1] = (int16_t)a1; k[2] = (int16_t)a2; k[3] = (int16_t)a3; k[4] = (int16_t)a4; k[5] = (int16_t)a5;
	}

	static void AnimStep(uint8_t *c)
	{
		if (BattleReadAnimation(c + 0x60, c + 0x6C) == 1)
		{
			PreBattleReadAnimation(c + 0x60, c + 0x6C, c[0x6C]);
		}
	}

	static void RinoaHeight(DirectorNode *d, int32_t v, int32_t base)
	{
		*(int16_t *)(Caster() + 0x20) = (int16_t)(d->centre[2] - (int16_t)v + base);
		*(int32_t *)(Caster() + 0x5C) = *(const int16_t *)(Caster() + 0x20);
		F<int32_t>(d, 0x90) = *(const int32_t *)(Caster() + 0x5C);
	}

	static void GrowDraw(const DirectorNode *d, int32_t g)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x9C);
		Mat4x3 *m = (Mat4x3 *)(h + 0x10);
		RotY(0, m);
		F<int32_t>(h, 8) = g;
		F<int32_t>(h, 4) = g;
		F<int32_t>(h, 0) = g;
		Scale3DMatrix(m, (const int32_t *)h);
		m->t[0] = d->centre[0];
		m->t[1] = d->centre[1];
		m->t[2] = d->centre[2] - sdiv_pow2(g * 13500, 12) + 0x3A98;
		ComposeAffineTransform(&Camera(), m, m);
		GteSetRotMatrix(m);
		GteSetTransVector(m);
		F<uint32_t>(h, 0x50) = 0;
		F<uint32_t>(h, 0x48) = 0;
		F<uint32_t>(h, 0x30) = game_addr(MDL_Trail);
		F<uint32_t>(h, 0x4C) = 0;
		Cursor() = RenderPrimSet(h + 0x30, OTBase() + 0x44, 2, Cursor());
		FieldFree(0x9C);
	}

	static void LandingDraw(uint32_t lt, const int16_t *p, int32_t angle, int32_t fade, int32_t g)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x8C);
		Mat4x3 *m = (Mat4x3 *)h;
		RotY(0, m);
		m->t[0] = p[0];
		m->t[1] = p[1];
		m->t[2] = p[2];
		ComposeAffineTransform(&Camera(), m, m);
		GteSetRotMatrix(m);
		GteSetTransVector(m);
		uint8_t *pr = h + 0x20;
		F<int16_t>(h, 0x4A) = 0x100;
		F<int16_t>(h, 0x48) = 0x100;
		F<int16_t>(h, 0x50) = 0x100;
		F<int16_t>(h, 0x46) = 0;
		F<int16_t>(h, 0x44) = 0;
		F<int16_t>(h, 0x4C) = 0;
		F<int16_t>(h, 0x40) = 0;
		F<uint32_t>(h, 0x28) = 0;
		F<uint32_t>(pr, 0) = game_addr(game::MAG_089_Model_Landing0);
		F<int16_t>(h, 0x4E) = 0x80;
		F<int16_t>(h, 0x52) = 0x80;
		F<int16_t>(h, 0x42) = (int16_t)((lt & 3) << 5);
		F<uint32_t>(h, 0x3C) = 0x30;
		if (lt - 0xB < 4)
		{
			F<uint32_t>(h, 0x3C) = 0xF0;
			F<int32_t>(h, 0x2C) = fade;
		}
		F<int32_t>(h, 0x38) = -0x40;
		Cursor() = RenderPrimSet(pr, OTBase() + 0x44, 2, Cursor());
		RotZ(angle, m);
		m->t[0] = p[0];
		m->t[1] = p[1];
		m->t[2] = p[2];
		ComposeAffineTransform(&Camera(), m, m);
		GteSetRotMatrix(m);
		GteSetTransVector(m);
		F<uint32_t>(h, 0x40) = 0;
		F<uint32_t>(pr, 0) = game_addr(game::MAG_089_Model_Landing1);
		F<int32_t>(h, 0x38) = -0x40;
		F<uint32_t>(h, 0x3C) = 0x30;
		Cursor() = RenderPrimSet(pr, OTBase() + 0x44, 2, Cursor());
		F<uint32_t>(h, 0x3C) = 0xF0;
		F<int32_t>(h, 0x2C) = g;
		F<uint32_t>(pr, 0) = game_addr(game::MAG_089_Model_Landing2);
		F<int32_t>(h, 0x38) = -0x40;
		Cursor() = RenderPrimSet(pr, OTBase() + 0x44, 2, Cursor());
		FieldFree(0x8C);
	}

	static void DirectorCamera(int32_t counter)
	{
		const int16_t *ctr = Centre();
		const uint32_t cs = (uint32_t)counter;
		int16_t ka[6], kb[6];
		if (cs < 4)
		{
			if (cs == 0)
			{
				CameraArmReturn();
				var<int16_t>(game_addr(game::BATTLE_PROJECTION_DISTANCE)) = 0x120;
				var<int16_t>(game_addr(game::g_BattleCam_Roll)) = 0;
			}
			SetKeys(ka, ctr[0] - 0x169, -0x2C1, ctr[2] + 0x12C5, ctr[0] + 0x45A, -0x5AF, ctr[2] - 0x34F);
			SetKeys(kb, ctr[0] - 0xA3A, 0x4F1, ctr[2] + 0xBB5, ctr[0] + 0x45A, -0x5AF, ctr[2] - 0x34F);
			CameraKeys(ka, kb, 0, 0, (int32_t)cs, 0x16);
		}
		else if (cs < 0x15)
		{
			const uint8_t *e = Caster();
			var<uint32_t>(game_addr(game::Battle_Camera_LookAt_XZ_s16)) = *(const uint32_t *)(e + 0x1C);
			const uint32_t z = *(const uint32_t *)(e + 0x20);
			var<uint32_t>(game_addr(game::Battle_Camera_LookAt_Y)) = z;
			var<uint32_t>(game_addr(game::Battle_Camera_world_Y)) = z;
			var<int16_t>(game_addr(game::unk_B8B7FA)) = (int16_t)(var<int16_t>(game_addr(game::unk_B8B7FA)) - 0x2A3);
			var<int16_t>(game_addr(game::Battle_Camera_LookAt_Y)) = (int16_t)(var<int16_t>(game_addr(game::Battle_Camera_LookAt_Y)) - 0x1F4);
			var<int16_t>(game_addr(game::Battle_Camera_world_Y)) = (int16_t)(var<int16_t>(game_addr(game::Battle_Camera_world_Y)) - 0x1F4);
			var<uint32_t>(game_addr(game::Battle_Camera_world_XZ_s16)) = *(const uint32_t *)(e + 0x1C);
			const uint32_t q = mulhi_u((cs - 4) << 12, 0xBA2E8BA3) >> 4;
			var<int16_t>(game_addr(game::unk_B8B7F2)) = (int16_t)(var<int16_t>(game_addr(game::unk_B8B7F2)) - 0x20B);
			var<int16_t>(game_addr(game::Battle_Camera_world_XZ_s16)) = (int16_t)(var<int16_t>(game_addr(game::Battle_Camera_world_XZ_s16)) + sdiv_pow2((int32_t)q, 2) + 1000);
		}
		uint32_t cc = (uint32_t)(counter - 0x15);
		if (cc < 0xE)
		{
			SetKeys(ka, ctr[0] - 0xBD, -0x6F8, ctr[2] + 0x187, ctr[0] - 0x54, -0xC4, ctr[2] + 0x17B5);
			SetKeys(kb, ctr[0] - 0x86, -0x295, ctr[2] + 0x276, ctr[0] + 0xE2, -0x155, ctr[2] + 0xC40);
			CameraKeys(ka, kb, 0, 0, (int32_t)cc, 0xE);
		}
		cc = (uint32_t)(counter - 0x31);
		if (cc < 0x19)
		{
			SetKeys(ka, ctr[0] - 0x317, ctr[1] - 0x1E0, ctr[2] - 0x6C3, ctr[0] - 0x96C, ctr[1] - 0xA27, ctr[2] - 0x1569);
			SetKeys(kb, ctr[0] + 0xCC, ctr[1] - 0x491, ctr[2] - 0xFC4, ctr[0] + 0x11F, ctr[1] - 0x77F, ctr[2] - 0x182D);
			CameraKeys(ka, kb, 0, 0, (int32_t)cc, 0x19);
		}
		cc = (uint32_t)(counter - 0x5F);
		if (cc < 0xF)
		{
			SetKeys(ka, ctr[0] + 0xCC, ctr[1] - 0x491, ctr[2] - 0xFC4, ctr[0] + 0x11F, ctr[1] - 0x77F, ctr[2] - 0x182D);
			SetKeys(kb, ctr[0] - 0x529, ctr[1] - 0xB7, ctr[2] - 0x51E, ctr[0] + 0x1AB4, ctr[1] - 0x1AE, ctr[2] + 0x767);
			CameraKeys(ka, kb, 0, 0, (int32_t)cc, 0xF);
		}
		cc = (uint32_t)(counter - 0x65);
		if (cc < 0x3C)
		{
			const int32_t t = (int32_t)(mulhi_u(cc << 12, 0x88888889) >> 5);
			const int32_t sn = ComputeSin(t);
			CameraOrbit(mul32(sn, -40) >> 12, -0x30, 0x1000);
		}
		if (counter == 0xA1)
		{
			var<int16_t>(game_addr(game::Battle_Camera_LookAt_XZ_s16)) = (int16_t)(ctr[0] - 0x158);
			var<int16_t>(game_addr(game::unk_B8B7FA)) = (int16_t)(ctr[1] - 0x5D);
			var<int16_t>(game_addr(game::Battle_Camera_LookAt_Y)) = (int16_t)(ctr[2] + 0xCF1);
			var<int16_t>(game_addr(game::Battle_Camera_world_XZ_s16)) = (int16_t)(ctr[0] - 0x196);
			var<int16_t>(game_addr(game::unk_B8B7F2)) = (int16_t)(ctr[1] - 0x57);
			var<int16_t>(game_addr(game::Battle_Camera_world_Y)) = (int16_t)(ctr[2] + 0x51C);
		}
		cc = (uint32_t)(counter - 0xB0);
		if (cc < 0x32)
		{
			const uint32_t e = cc - 6;
			const int32_t t = e > 0x3E8 ? 0 : (int32_t)(mulhi_u(e << 10, 0xBA2E8BA3) >> 5);
			SetKeys(ka, ctr[0] + 0x3F1, ctr[1] - 0x2B9, ctr[2] + 0x8EF, ctr[0] + 0x188F, ctr[1] - 0x498, ctr[2] + 0x1B1B);
			SetKeys(kb, ctr[0] - 0x10A8, ctr[1] - 0xE1B, ctr[2] - 0xC50, ctr[0] + 0x1C7B, ctr[1] + 0x128C, ctr[2] + 0x167D);
			CameraBlend(ka, kb, 0, 0, ComputeSin(ComputeSin(t) >> 2));
		}
	}

	static uint32_t __cdecl Director(TaskNode *n)
	{
		DirectorNode *d = (DirectorNode *)n;
		uint8_t *angelo = d->angelo;
		int16_t c = d->counter;
		if (c == 0x1E) QueueChainTransformation(Caster(), 2);
		else if (c == 0x20) QueueChainTransformation(Caster(), 3);
		else if (c == 0xDE) QueueChainTransformation(Caster(), 4);
		c = d->counter;
		if (c == 7) SetAnim(angelo, 1);
		else if (c == 0x1E) SetAnim(angelo, 2);
		else if (c == 0x20) SetAnim(angelo, 3);
		uint32_t s = (uint32_t)(int32_t)d->counter;
		if (s < 0x16)
		{
			const uint32_t q = mulhi_u(s << 12, 0xBA2E8BA3) >> 4;
			if (s == 0) EnemyFlag4On();
			RinoaHeight(d, sdiv_pow2((int32_t)(q * 5000), 12), 0x1770);
			if (s >= 3)
			{
				switch (s % 6)
				{
				case 0: FlashAt(angelo, (const int16_t *)game_addr(game::MAG_089_FlashPoint_Start), 0); break;
				case 1: FlashAt(angelo, (const int16_t *)game_addr(game::MAG_089_FlashPoint_1), 0); break;
				case 3: FlashAt(angelo, (const int16_t *)game_addr(game::MAG_089_FlashPoint_2), 4); break;
				case 4: FlashAt(angelo, (const int16_t *)game_addr(game::MAG_089_FlashPoint_3), 4); break;
				default: break;
				}
				if (s % 7 == 0) FlashAt(Caster(), (const int16_t *)(game_addr(game::MAG_089_FlashPoints_Rinoa) + ((s / 7) & 1) * 8), 0);
			}
		}
		s = (uint32_t)((int32_t)d->counter - 0x1E);
		if (s <= 3)
		{
			*(int16_t *)(Caster() + 0x1E) = (int16_t)mul32((int32_t)s + 1, -275);
			*(int32_t *)(Caster() + 0x58) = *(const int16_t *)(Caster() + 0x1E);
			F<int32_t>(d, 0x8C) = *(const int32_t *)(Caster() + 0x58);
		}
		s = (uint32_t)((int32_t)d->counter - 0x16);
		if (s < 0x1C)
		{
			const uint32_t q = (s << 12) / 28;
			if (s == 0)
			{
				InitTaskQueuePool(QB(), (void *)(TexBase() + 0xC40), 0x30, 0x20);
				RestoreEnemyFlags();
			}
			RinoaHeight(d, sdiv_pow2((int32_t)(q * 12000), 12), 0x1770);
		}
		s = (uint32_t)((int32_t)d->counter - 0x4B);
		if (s < 0x14)
		{
			if (s == 0) DecodeModelPrimLayout(game_addr(game::MAG_089_Layout_Rise), d->layout, 0x68);
			Play(d->layout, d->centre[0], d->centre[1], d->centre[2] + 0x3A98, 0, &Cursor());
		}
		s = (uint32_t)((int32_t)d->counter - 0x57);
		if (s < 0xF)
		{
			const int32_t g = (int32_t)(mulhi_u(s << 12, 0x88888889) >> 3);
			GrowDraw(d, g);
		}
		s = (uint32_t)((int32_t)d->counter - 0x66);
		if (s < 0x3C && s <= 0x2D)
		{
			switch (*(const uint8_t *)(game_addr(game::sub_6FBB50) + s))
			{
			case 0: ModelsOn() = 1; SpritesOn() = 1; DiveSpawn(game_addr(ORIG_DiveTask), 0, 0, 6); break;
			case 1: DiveSpawn(game_addr(ORIG_DiveTask), 0x600, 0x600, 6); break;
			case 2: DiveSpawn(game_addr(ORIG_DiveTask), 0xE00, 0xE00, 6); break;
			case 3: DiveSpawn(game_addr(ORIG_DiveTask), 0xE00, 0x600, 6); break;
			case 4: DiveSpawn(game_addr(ORIG_DiveTask), 0x200, 0xA00, 6); break;
			case 5: DiveSpawn(game_addr(ORIG_DiveTask), 0xA00, 0, 6); break;
			default: break;
			}
		}
		s = (uint32_t)((int32_t)d->counter - 0x20);
		if (s < 4)
		{
			if (s == 0) DecodeModelPrimLayout(game_addr(game::MAG_089_Layout_BoneBurst), d->layout, 0x70);
			int16_t p[4];
			GetEffectSpawnPosition(angelo, 0x13, 0, p);
			Play(d->layout, p[0], p[1], p[2], -0x200, &Cursor());
		}
		const uint32_t lt = (uint32_t)((int32_t)d->counter - 0xA2);
		if (lt < 0xF)
		{
			const int32_t g = (int32_t)(mulhi_u(lt << 12, 0x88888889) >> 3);
			if (lt == 0)
			{
				*(int16_t *)(Caster() + 0x1E) = d->centre[1];
				*(int32_t *)(Caster() + 0x58) = *(const int16_t *)(Caster() + 0x1E);
				F<int32_t>(d, 0x8C) = *(const int32_t *)(Caster() + 0x58);
				*(uint16_t *)Caster() &= 0xFFF3;
				*(uint16_t *)angelo &= 0xFFF3;
				ModelsOn() = 0;
				SpritesOn() = 0;
			}
			RinoaHeight(d, sdiv_pow2(g * 13500, 12), 0x3A98);
			int16_t p[4];
			GetEffectSpawnPosition(angelo, 0x13, 0, p);
			LandingDraw(lt, p, (int32_t)((lt & 0xFF) << 8), shl32((int32_t)(lt - 0xB) + 1, 10), g);
		}
		s = (uint32_t)((int32_t)d->counter - 0xB1);
		if (s < 0x32)
		{
			if (s == 0)
			{
				*(uint8_t *)Caster() |= 0xC;
				angelo[0] |= 0xC;
				DecodeModelPrimLayout(game_addr(game::MAG_089_Layout_LastDive), d->layout, 0x218);
				SpritesInit();
				ModelsOn() = 1;
				SpritesOn() = 1;
				DiveSpawn(game_addr(ORIG_LastDiveTask), 0, 0, 2);
			}
			else if (s >= 3)
			{
				if (s == 3) DamageWave();
				Play(d->layout, d->centre[0], d->centre[1], d->centre[2], 0, &Cursor());
			}
		}
		s = (uint32_t)((int32_t)d->counter - 0x25);
		if (s < 0xBD)
		{
			if (s == 0) DecodeModelPrimLayout(game_addr(game::MAG_089_Layout_Sky), d->layout2, 0xEC);
			Play(d->layout2, d->centre[0], 0, d->centre[2], 0, &FrameArena());
			FrameArena() = Stars(0, Centre()[0], 0, Centre()[2], 0x10000, FrameArena());
		}
		s = (uint32_t)((int32_t)d->counter - 0x24);
		if (s < 0x27)
		{
			if (s == 0)
			{
				Capture(TexBase() + 0x14404);
				FadeTile(0, 0xFF, 0xFF, 0xFF, 0, 1, 7);
			}
			else if (s < 8) FadeTile((int32_t)s, 0xFF, 0xFF, 0xFF, 0, 1, 7);
			ShardsDraw();
		}
		if ((uint32_t)((int32_t)d->counter - 0x24) < 0xBE) ScreenFeedback();
		if ((uint16_t)d->counter < 0x10F)
		{
			if (!(angelo[0] & 4))
			{
				Cursor() = DrawCreatureModel(angelo, TexBase() + 0x1888C, Cursor(), CursorEnd());
			}
			if (!(angelo[0] & 8)) AnimStep(angelo);
		}
		s = (uint32_t)((int32_t)d->counter - 0xC5);
		if (s < 0x3E)
		{
			FadeTile((int32_t)s, 0xFF, 0xFF, 0xFF, 0x1E, 1, 0x1F);
			if (s == 0x1E)
			{
				var<uint16_t>(game_addr(game::BATTLE_PROJECTION_DISTANCE)) = var<uint16_t>(game_addr(game::Battle_Camera_ReturnView_Param1D8E038));
				var<uint32_t>(game_addr(game::Battle_Camera_LookAt_XZ_s16)) = var<uint32_t>(game_addr(game::Battle_Camera_ReturnView_LookAt_XZ_s16));
				var<uint32_t>(game_addr(game::Battle_Camera_world_XZ_s16)) = var<uint32_t>(game_addr(game::Battle_Camera_ReturnView_world_XZ_s16));
				var<uint32_t>(game_addr(game::Battle_Camera_LookAt_Y)) = var<uint32_t>(game_addr(game::Battle_Camera_ReturnView_LookAt_Y));
				var<uint32_t>(game_addr(game::Battle_Camera_world_Y)) = var<uint32_t>(game_addr(game::Battle_Camera_ReturnView_world_Y));
				*(uint32_t *)(Caster() + 0x1C) = d->rinoa_xy;
				*(uint32_t *)(Caster() + 0x20) = d->rinoa_z;
				*(uint16_t *)Caster() &= 0xFFF3;
				RestorePartyFlags();
				StageModelsBack();
			}
		}
		s = (uint32_t)((int32_t)d->counter - 0xEB);
		if (s < 0x18)
		{
			uint32_t cnt;
			if (s == 0)
			{
				SpritesInit();
				cnt = 1u << 7;
			}
			else if (s >= 0x16) cnt = 4;
			else if (s < 0x10) cnt = 1u << ((7 - (s >> 1)) & 31);
			else cnt = 0;
			while (cnt-- != 0)
			{
				Sprite *p = SpriteAlloc();
				if (!p) break;
				p->pos[0] = (int16_t)(sdiv_pow2(mul32(CrtRand(), 8000), 15) - 0xFA0);
				p->pos[1] = (int16_t)sdiv_pow2(mul32(CrtRand(), -4000), 15);
				const int32_t r = CrtRand();
				p->scale = 0x400;
				p->life = 0xE;
				p->seq = game_addr(SEQ_Spark);
				p->pos[2] = (int16_t)(sdiv_pow2(mul32(r, 10000), 15) - 0x1388);
				p->vel[0] = (int16_t)((CrtRand() & 0x3F) - 0x20);
				p->vel[1] = (int16_t)((CrtRand() & 0x3F) - 0x20);
				const int32_t vz = (CrtRand() & 0x3F) - 0x20;
				p->vel[2] = (int16_t)vz;
				p->acc[2] = (int8_t)(-vz >> 6);
				p->acc[0] = (int8_t)(-(int32_t)p->vel[0] >> 6);
				p->acc[1] = (int8_t)(-(int32_t)p->vel[1] >> 6);
			}
		}
		if (ModelsOn())
		{
			PModelsDraw();
		}
		if (SpritesOn())
		{
			SpritesDraw();
		}
		SpritesUpdate();
		PModelsUpdate();
		const uint32_t buf = TexBase() + 0xBC04;
		c = d->counter;
		if (c == 0)
		{
			StreamLoad(0x2F6, buf, 1);
			StreamLoad(0x2F7, TexBase() + 0xBC04, 1);
			StreamLoad(0x2F8, TexBase() + 0xBC04, 5);
		}
		else if (c == 0x24)
		{
			if (StreamBusy()) return 0;
		}
		else if (c == 0x4A)
		{
			QueueTIMUpload(buf);
			QueueTIMUpload(TexBase() + 0xDE24);
			QueueTIMUpload(TexBase() + 0x10044);
			QueueTIMUpload(TexBase() + 0x11264);
		}
		else if (c == 0x4C)
		{
			StreamLoad(0x2F9, buf, 1);
			StreamLoad(0x2FA, TexBase() + 0xBC04, 1);
			StreamLoad(0x2FB, TexBase() + 0xBC04, 1);
		}
		else if (c == 0x65)
		{
			if (StreamBusy()) return 0;
			StreamLoad(0x2FC, buf, 5);
		}
		else if (c == 0xAD)
		{
			if (StreamBusy()) return 0;
			QueueTIMUpload(buf);
			QueueTIMUpload(TexBase() + 0x10024);
		}
		else if (c == 0xE3)
		{
			StreamLoad(0x168, buf, 1);
			StreamLoad(0x169, TexBase() + 0xBC04, 1);
			StreamLoad(0x16A, TexBase() + 0xBC04, 1);
		}
		StreamPump();
		c = d->counter;
		if (c == 0) BdPlaySE((const void *)game_addr(game::MAG_089_SE_Start), 1, 0x80);
		else if (c == 0x32) BdPlaySE((const void *)game_addr(game::MAG_089_SE_Counter32), 1, 0x80);
		else if (c == 0xA2) BdPlaySE((const void *)game_addr(game::MAG_089_SE_Landing), 1, 0x80);
		if (d->counter == 0x32)
		{
			RestoreEnemyFlags();
			*(uint8_t *)Caster() |= 0xC;
			angelo[0] |= 0xC;
		}
		s = (uint32_t)((int32_t)d->counter - 0x24);
		if (s < 0x27) ShardsUpdate(s < 0xA ? (int32_t)(mulhi_u((s * 5) << 9, 0xCCCCCCCD) >> 3) : 0xA00);
		if ((uint32_t)((int32_t)d->counter - 0x25) < 0xBD) StarsTwinkle();
		DirectorCamera(d->counter);
		d->counter++;
		if (d->counter >= 0x10F)
		{
			*(uint32_t *)(Caster() + 0xC) = d->rinoa_ang0;
			*(uint32_t *)(Caster() + 0x10) = d->rinoa_ang1;
			if (StreamPending(2)) return TASK_END;
			d->counter = 0x11F;
		}
		return 0;
	}

	static void DirectorInit(DirectorNode *d)
	{
		Memset32(&d->counter, 0, 0x104);
		const uint32_t tex = TexBase();
		int16_t maxx = -0x7FFF, maxy = -0x7FFF, maxz = -0x7FFF, minx = 0x7FFF, miny = 0x7FFF, minz = 0x7FFF;
		ActionData *act = F<ActionData *>(Ctx(), 4);
		const uint32_t count = (uint32_t)act->target_count >> 3;
		uint8_t **tg = (uint8_t **)d->targets;
		for (uint32_t k = 0, off = 0; k < count; k++, off += 0x18, tg += 2)
		{
			act = F<ActionData *>(Ctx(), 4);
			uint8_t *e = Entity(act->targets[off]);
			*tg = e;
			int16_t p[4];
			GetDefaultEffectPosition(e, p);
			if (p[0] < minx) minx = p[0];
			if (p[0] > maxx) maxx = p[0];
			if (p[1] < miny) miny = p[1];
			if (p[1] > maxy) maxy = p[1];
			if (p[2] < minz) minz = p[2];
			if (p[2] > maxz) maxz = p[2];
		}
		act = F<ActionData *>(Ctx(), 4);
		TargetCursor() = act->targets;
		d->centre[0] = (int16_t)((maxx + minx) / 2);
		d->centre[1] = (int16_t)((miny + maxy) / 2);
		d->centre[2] = maxz;
		uint8_t *e = Caster();
		d->rinoa_xy = *(const uint32_t *)(e + 0x1C);
		d->rinoa_z = *(const uint32_t *)(e + 0x20);
		d->rinoa_ang0 = *(const uint32_t *)(Caster() + 0xC);
		d->rinoa_ang1 = *(const uint32_t *)(Caster() + 0x10);
		*(int16_t *)(Caster() + 0xE) = 0;
		Centre()[0] = d->centre[0];
		Centre()[1] = d->centre[1];
		Centre()[2] = d->centre[2];
		PartyFlag4On();
		*(uint16_t *)Caster() &= 0xFFFB;
		BindExternalAnimSet(Caster(), d->animset, game_addr(game::MAG_089_RinoaAnimSet));
		QueueChainTransformation(Caster(), 1);
		uint8_t *angelo = d->angelo;
		BindModelContainer(angelo, d->sections, game_addr(game::MAG_089_AngeloSections));
		SetAnim(angelo, 0);
		RotY(0, angelo + 0x40);
		const int32_t cx = Centre()[0];
		F<int32_t>(d, 0x88) = cx;
		F<int16_t>(d, 0x50) = (int16_t)cx;
		*(int16_t *)(Caster() + 0x1C) = (int16_t)cx;
		F<int32_t>(d, 0x8C) = 0;
		F<int16_t>(d, 0x52) = 0;
		*(int16_t *)(Caster() + 0x1E) = 0;
		const int32_t cz = Centre()[2];
		F<int32_t>(d, 0x90) = cz;
		F<int16_t>(d, 0x54) = (int16_t)cz;
		*(int16_t *)(Caster() + 0x20) = (int16_t)cz;
		PModelsInit();
		SpritesInit();
		ShardsReset();
		int16_t off[4];
		off[0] = 0xA0;
		off[1] = 0x6C;
		off[2] = 0;
		ShardsInit(game_addr(MDL_Shards), off);
		StreamStateInit((void *)(TexBase() + 0x5E5C));
		uint8_t *st = (uint8_t *)(TexBase() + 0x7C04);
		for (uint32_t g = game_addr(STAR_GROUPS) + 8; g < game_addr(game::unk_14AF518); g += 0x10)
		{
			*(int16_t *)(g + 2) = 0;
			*(int16_t *)g = 0;
			for (int k = 0x80; k != 0; k--, st += 0x10)
			{
				const int32_t pitch = sdiv_pow2(mul32(CrtRand(), 1600), 15) - 0x320;
				const int32_t yaw = (CrtRand() & 0xFFF) - 0x800;
				DirectionFromAngles(pitch, yaw, (int16_t *)st);
				F<int16_t>(st, 6) = (int16_t)ISqrt(mul32(yaw, yaw) + mul32(pitch, pitch));
				const int32_t rr = CrtRand();
				const int32_t dd = ((rr - (rr >> 31)) >> 1) + 0x2EE0;
				F<int16_t>(st, 0) = (int16_t)(mul32(F<int16_t>(st, 0), dd) >> 12);
				F<int16_t>(st, 2) = (int16_t)(mul32(F<int16_t>(st, 2), dd) >> 12);
				F<int16_t>(st, 4) = (int16_t)(mul32(F<int16_t>(st, 4), dd) >> 12);
				F<uint32_t>(st, 8) = *(const uint32_t *)(g - 8);
				st[0xB] = (uint8_t)CrtRand();
				F<uint32_t>(st, 0xC) = *(const uint32_t *)(g - 4);
				const uint8_t sp = st[0xF];
				st[0xF] = (uint8_t)((mul32(CrtRand(), sp) >> 17) + sp);
			}
		}
		(void)tex;
	}

	static uint32_t __cdecl RootTask(TaskNode *n)
	{
		RootNode *r = (RootNode *)n;
		const uint32_t tex = TexBase();
		if (r->parity != 0)
		{
			Cursor() = tex + 0x1A88C;
			CursorEnd() = tex + 0x2A88C;
			r->parity = 0;
		}
		else
		{
			Cursor() = tex + 0x2A88C;
			CursorEnd() = tex + 0x3A88C;
			r->parity = 1;
		}
		if (r->counter == 2)
		{
			SaveEntityFlags();
			if (!r->started)
			{
				r->started = 1;
				InitTaskQueuePool(QA(), (void *)(TexBase() + 0x5A40), 0x41C, 1);
				InitTaskQueuePool(QB(), (void *)(TexBase() + 0xC40), 0x30, 0x20);
				InitTaskQueuePool(QC(), (void *)TexBase(), 0x188, 8);
			}
			DirectorNode *d = (DirectorNode *)AddTaskToQueue(QA(), game_addr(ORIG_Director));
			if (d) DirectorInit(d);
		}
		int a = (int)n;
		if (r->started)
		{
			a = ExecuteTaskQueue(QA());
			ExecuteTaskQueue(QB());
			ExecuteTaskQueue(QC());
		}
		if (r->started && a == 0) return TASK_END;
		r->counter++;
		return 0;
	}
}

	void register_mag089_wishing_star()
	{
		register_port(ws089::ORIG_RootTask, (void *)ws089::RootTask, "089 RootTask", 89);
		register_port(ws089::ORIG_Director, (void *)ws089::Director, "089 Director", 89);
		register_port(ws089::ORIG_FlashTask, (void *)ws089::FlashTask, "089 FlashTask", 89);
		register_port(ws089::ORIG_DebrisTask, (void *)ws089::DebrisTask, "089 DebrisTask", 89);
		register_port(ws089::ORIG_DiveTask, (void *)ws089::DiveTask, "089 DiveTask", 89);
		register_port(ws089::ORIG_LastDiveTask, (void *)ws089::LastDiveTask, "089 LastDiveTask", 89);
	}
}

