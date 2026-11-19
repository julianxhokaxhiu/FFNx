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
namespace sp070
{
	using namespace eng;
	using namespace magc;

	inline uint8_t *&SparkCursor() { return var<uint8_t *>(game_addr(game::g_Pulsar_SparkCursor)); }
	inline uint8_t *&ShardCursor() { return var<uint8_t *>(game_addr(game::g_Pulsar_ShardCursor)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::g_Pulsar_PacketCursor)); }
	inline uint32_t &ArenaEnd() { return var<uint32_t>(game_addr(game::g_Pulsar_ArenaEnd)); }
	inline uint8_t *&Ctx() { return var<uint8_t *>(game_addr(game::g_Pulsar_CastContext)); }
	inline TaskQueue *QSub() { return (TaskQueue *)game_addr(game::g_Pulsar_SubQueue); }
	inline TaskQueue *QMaster() { return (TaskQueue *)game_addr(game::g_Pulsar_MasterQueue); }
	inline uint32_t &InitAngles0() { return var<uint32_t>(game_addr(game::g_Pulsar_InitAngles)); }
	inline uint32_t &InitAngles1() { return var<uint32_t>(game_addr(game::unk_2544EFC)); }
	inline uint32_t &CentreX() { return var<uint32_t>(game_addr(game::g_Pulsar_TargetsCentre)); }
	inline int16_t &CentreY() { return var<int16_t>(game_addr(game::unk_2544F0A)); }
	inline uint32_t &CentreZ() { return var<uint32_t>(game_addr(game::unk_2544F0C)); }
	inline int32_t &Flash() { return var<int32_t>(game_addr(game::g_Pulsar_FlashLevel)); }
	inline int32_t &FlashPrev() { return var<int32_t>(game_addr(game::g_Pulsar_FlashPrev)); }
	inline int32_t &Side() { return var<int32_t>(game_addr(game::g_Pulsar_Side)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::g_Pulsar_TexBase)); }
	inline Mat4x3 &EffCam() { return var<Mat4x3>(game_addr(game::g_Pulsar_EffectCamera)); }
	inline uint32_t *SavedFlags() { return (uint32_t *)game_addr(game::g_Pulsar_SavedEntityFlags); }
	inline uint32_t OT() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)); }
	inline int16_t &ViewWord() { return var<int16_t>(game_addr(game::BATTLE_PROJECTION_DISTANCE)); }
	inline int16_t &CameraZoom() { return var<int16_t>(game_addr(game::g_BattleCam_Roll)); }
	inline int16_t *CamEye() { return (int16_t *)game_addr(game::Battle_Camera_world_XZ_s16); }
	inline int16_t *CamLookAt() { return (int16_t *)game_addr(game::Battle_Camera_LookAt_XZ_s16); }

	static constexpr game::Id ORIG_RootTask = game::MAG_070_ShockwavePulsar_RootTask;
	static constexpr game::Id ORIG_MasterTask = game::MAG_070_ShockwavePulsar_MasterTask;
	static constexpr game::Id ORIG_LiftTask = game::MAG_070_ShockwavePulsar_LiftTask;
	static constexpr game::Id ORIG_RiseTask = game::MAG_070_ShockwavePulsar_RiseTask;
	static constexpr game::Id ORIG_RingTask = game::MAG_070_ShockwavePulsar_RingTask;
	static constexpr game::Id ORIG_Callback = game::MAG_070_ShockwavePulsar_PrimPartCallback;
	static constexpr game::Id LAYOUT_CENTRE = game::unk_1332540;
	static constexpr game::Id LAYOUT_WAVE = game::unk_1327D28;
	static constexpr game::Id LAYOUT_LIFT = game::unk_1326C68;
	static constexpr game::Id LAYOUT_RISE = game::unk_1325A88;
	static constexpr game::Id LAYOUT_RING = game::unk_1324FF0;
	static constexpr game::Id OBJS_A = game::a00000000000000_1;
	static constexpr game::Id OBJS_WAVE = game::MAG_260_SHOCKWAVE_PULSAR_QUISTIS_WaveObjectTypes;
	static constexpr game::Id SPARK_SEQ = game::unk_1324B98;
	static constexpr game::Id BURST_SEQ = game::unk_13249EC;
	static constexpr game::Id SHARD_MODEL = game::unk_1324ED8;
	static const void *SOUND_Voice() { return (const void *)game_addr(game::unk_1324DAC); }
	static const void *SOUND_0() { return (const void *)game_addr(game::unk_1324ECC); }
	static const void *SOUND_50() { return (const void *)game_addr(game::unk_1324ED0); }
	static const void *SOUND_AA() { return (const void *)game_addr(game::unk_1324ED4); }

	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void RotationFromAnglesB(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::MAG_063_RotationFromAnglesYXZ))(angles, out); }
	inline void RotationFromAnglesC(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::MAG_RotationFromAnglesXYZ))(angles, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void MatrixMultiply3(Mat4x3 *a, const Mat4x3 *b) { fn<void (__cdecl *)(Mat4x3 *, const Mat4x3 *)>(game_addr(game::MatrixMultiply3x3))(a, b); }
	inline uint32_t RenderPrimModel2(void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::MAG_070_RenderPrimModel))(h, ot, mode, cursor); }
	inline uint32_t Tile(int32_t r, int32_t g, int32_t b, int32_t level, uint32_t cursor) { return fn<uint32_t (__cdecl *)(int32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_FullScreenTile))(r, g, b, level, cursor); }
	inline uint32_t Flipbook(void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::MAG_065_RenderFlipbook))(h, ot, mode, cursor); }
	inline void CamLerp(const int16_t *a, const int16_t *b, int32_t t) { fn<void (__cdecl *)(const int16_t *, const int16_t *, int32_t)>(game_addr(game::MAG_325_CameraLerp))(a, b, t); }
	inline void DrawTargetModel(uint8_t *e, const int16_t *angles, const int16_t *offset, int32_t scale) { fn<void (__cdecl *)(uint8_t *, const int16_t *, const int16_t *, int32_t)>(game_addr(game::MAG_260_SHOCKWAVE_PULSAR_QUISTIS_DrawEntityModelTransformed))(e, angles, offset, scale); }
	inline void ShardMatrix(int32_t angle, int32_t sy, int32_t sxz, void *out) { fn<void (__cdecl *)(int32_t, int32_t, int32_t, void *)>(game_addr(game::MAG_260_SHOCKWAVE_PULSAR_QUISTIS_ShardMatrix))(angle, sy, sxz, out); }
	typedef uint32_t (__cdecl *PrimList)(uint8_t *h, uint32_t ot, int32_t shift, uint32_t cursor);
	static constexpr game::Id LIST_FLAT_TRI = game::MAG_260_SHOCKWAVE_PULSAR_QUISTIS_FlatTris, LIST_1 = game::MAG_260_SHOCKWAVE_PULSAR_QUISTIS_TexTris, LIST_2 = game::MAG_260_SHOCKWAVE_PULSAR_QUISTIS_TexQuads, LIST_3 = game::MAG_260_SHOCKWAVE_PULSAR_QUISTIS_GouraudTris, LIST_4 = game::MAG_260_SHOCKWAVE_PULSAR_QUISTIS_GouraudTexTris;
	inline uint32_t CallList(uint32_t addr, uint8_t *h, uint32_t ot, int32_t shift, uint32_t cursor) { return fn<PrimList>(addr)(h, ot, shift, cursor); }
	inline int32_t ComputeCos(int32_t angle) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(angle); }
	inline void CameraArmReturn() { fn<void (__cdecl *)()>(game_addr(game::BS_Camera_ArmReturnAfterEffect))(); }
	inline void ReleaseVoice(uint32_t slot) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::BdSound_ReleaseVoiceSlot))(slot); }
	inline void StreamLoad(int32_t id, uint32_t dst, int32_t kind) { fn<void (__cdecl *)(int32_t, uint32_t, int32_t)>(game_addr(game::Stream_QueueLoad))(id, dst, kind); }
	inline void StreamPump() { fn<void (__cdecl *)()>(game_addr(game::Stream_Pump))(); }
	inline int32_t StreamPending(int32_t a) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::BattleFile_PreLoad_IsLoaded))(a); }
	inline void TransformVec3(const void *m, const int32_t *in, int32_t *out) { fn<void (__cdecl *)(const void *, const int32_t *, int32_t *)>(game_addr(game::TransformVectorBy3x3Matrix))(m, in, out); }
	inline void GteMVMVA_RotV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0))(); }
	inline void GteLoadV0u(const void *v) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadV0_u16))(v); }
	inline void GteReadIR2(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadIR2Word))(dst); }
	inline void GteLoadV012(const void *a, const void *b, const void *c) { fn<void (__cdecl *)(const void *, const void *, const void *)>(game_addr(game::GTE_LoadV012))(a, b, c); }
	inline void GteRTPT() { fn<void (__cdecl *)()>(game_addr(game::GTE_RTPT))(); }
	inline void GteReadFLAG(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadFLAG))(dst); }
	inline void GteReadSXY012(void *a, void *b, void *c) { fn<void (__cdecl *)(void *, void *, void *)>(game_addr(game::GTE_ReadSXY012_Split))(a, b, c); }
	inline void GteNCLIP() { fn<void (__cdecl *)()>(game_addr(game::GTE_NCLIP))(); }
	inline void GteReadMAC0(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadMAC0))(dst); }
	inline void GteAVSZ4() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ4))(); }
	inline void GteReadOTZWord(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadOTZ))(dst); }
	inline void GteSetFarColor(uint32_t r, uint32_t g, uint32_t b) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_SetFarColor))(r, g, b); }
	inline void GteLoadRGBC(const void *c) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadRGBC))(c); }
	inline void GteLoadRGB012(const void *a, const void *b, const void *c) { fn<void (__cdecl *)(const void *, const void *, const void *)>(game_addr(game::GTE_LoadRGB012))(a, b, c); }
	inline void GteDPCS() { fn<void (__cdecl *)()>(game_addr(game::GTE_DPCS))(); }
	inline void GteDPCT() { fn<void (__cdecl *)()>(game_addr(game::GTE_DPCT))(); }
	inline void GteStoreRGB2(void *c) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreRGB2))(c); }
	inline void GteStoreRGB012(void *a, void *b, void *c) { fn<void (__cdecl *)(void *, void *, void *)>(game_addr(game::GTE_StoreRGB012))(a, b, c); }

	inline uint32_t mulhi_u(uint32_t a, uint32_t b) { return (uint32_t)(((uint64_t)a * b) >> 32); }
	inline int32_t mulhi_s(int32_t a, int32_t b) { return (int32_t)(((int64_t)a * b) >> 32); }
	inline int32_t sdiv_pow2(int32_t v, int n) { return (int32_t)((uint32_t)v + ((uint32_t)(v >> 31) & ((1u << n) - 1))) >> n; }
	template<typename T> static inline T &F(uint8_t *p, uint32_t off) { return *(T *)(p + off); }
	inline int16_t &E16(uint8_t *e, uint32_t off) { return *(int16_t *)(e + off); }
	inline int32_t SlotOf(const uint8_t *e)
	{
		const int32_t d = (int32_t)((uint32_t)e - game_addr(game::BattleEntitySlotData));
		int32_t q = (mulhi_s(d, (int32_t)0xD20D20D3) + d) >> 7;
		return q + (int32_t)((uint32_t)q >> 31);
	}

#pragma pack(push, 1)
	struct RootNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t parity;
		uint8_t started;
	};
	struct Target
	{
		uint8_t *entity;
		uint32_t pos_xy;
		uint32_t pos_z;
		int16_t ang[3];
		int16_t pad12;
		int16_t y_max;
		int16_t y_min;
	};
	struct MasterNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t ntargets;
		uint32_t voice;
		Target targets[3];
		uint8_t layout[0x1100];
	};
	struct SubNode
	{
		TaskNode hdr;
		union
		{
			struct { Target *rec; int16_t counter; int16_t delay; } t;
			int16_t pos[4];
		};
		uint8_t layout[0x100];
	};
	struct Spark
	{
		const uint8_t *seq;
		int16_t pos[3];
		int16_t scale;
		int8_t vel[3];
		uint8_t life;
	};
	struct Shard
	{
		int16_t life;
		int16_t bone;
		int16_t sy, sxz;
		int16_t dsy, dsxz;
		Target *rec;
	};
	struct PrimArg
	{
		int16_t pos[3];
		int16_t pad06;
		int32_t scale[3];
		int32_t scaled;
		uint32_t objs;
		int16_t depth;
		int16_t pad1E;
		uint32_t morph;
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x10 && sizeof(MasterNode) == 0x115C && sizeof(SubNode) == 0x114, "Pulsar 070 nodes");
	static_assert(sizeof(Target) == 0x18 && sizeof(Spark) == 0x10 && sizeof(Shard) == 0x10 && sizeof(PrimArg) == 0x24, "Pulsar 070 blocks");
}
}

namespace ff8fx
{
namespace sp070
{
	static inline bool OutX(const uint8_t *xy) { const int16_t v = *(const int16_t *)xy; return v < 0 || v > 0xA00; }
	static inline bool OutY(const uint8_t *xy) { const int16_t v = *(const int16_t *)(xy + 2); return v < 0 || v > 0x6C0; }

	static uint32_t __cdecl FlatQuads(uint8_t *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		uint8_t *pk = (uint8_t *)cursor;
		const uint8_t *list = F<uint8_t *>(h, 0x20);
		const int32_t count = *(const int32_t *)list;
		const uint8_t *rec = list + 4;
		F<const uint8_t *>(h, 0x20) = rec;
		const uint8_t *verts = F<uint8_t *>(h, 4);
		if (count <= 0)
		{
			F<const uint8_t *>(h, 0x20) = rec;
			return (uint32_t)pk;
		}
		for (int32_t k = count; k != 0; k--, rec += 0xC)
		{
			GteLoadV012(verts + (uint32_t)*(const uint16_t *)(rec + 4) * 4, verts + (uint32_t)*(const uint16_t *)(rec + 6) * 4, verts + (uint32_t)*(const uint16_t *)(rec + 8) * 4);
			GteRTPT();
			F<uint32_t>(pk, 0) = 0x5000000;
			const uint32_t code = *(const uint32_t *)rec;
			F<uint32_t>(pk, 4) = code;
			if (F<uint8_t>(h, 0x1C) & 1) F<uint32_t>(pk, 4) = code | 0x2000000;
			if (F<uint8_t>(h, 0x1C) & 4) F<uint32_t>(pk, 4) &= 0xFDFFFFFF;
			GteReadFLAG(h + 0x30);
			if (F<uint32_t>(h, 0x30) & 0x60000) continue;
			GteNCLIP();
			uint32_t clip = 0;
			GteReadMAC0(h + 0x24);
			if (F<int32_t>(h, 0x24) < 0 && !(F<uint8_t>(h, 0x1C) & 0x10)) continue;
			GteReadSXY012(pk + 8, pk + 0xC, pk + 0x10);
			GteLoadV0(verts + (uint32_t)*(const uint16_t *)(rec + 0xA) * 4);
			GteRTPS();
			if (OutX(pk + 8)) clip = 1;
			if (OutX(pk + 0xC)) clip |= 2;
			if (OutX(pk + 0x10)) clip |= 4;
			if (OutY(pk + 8)) clip |= 0x10;
			if (OutY(pk + 0xC)) clip |= 0x20;
			if (OutY(pk + 0x10)) clip |= 0x40;
			GteReadSXY2(pk + 0x14);
			GteAVSZ4();
			if (OutX(pk + 0x14)) clip |= 8;
			if (OutY(pk + 0x14)) clip |= 0x80;
			if ((clip & 0xF) == 0xF) continue;
			if ((clip & 0xF0) == 0xF0) continue;
			GteReadOTZWord(h + 0x2C);
			if (F<uint8_t>(h, 0x1C) & 0x40)
			{
				GteLoadRGBC(pk + 4);
				GteSetIR0(F<int32_t>(h, 0xC));
				GteDPCS();
				GteStoreRGB2(pk + 4);
			}
			int32_t z = (int32_t)((uint32_t)F<int32_t>(h, 0x2C) + (uint32_t)F<int32_t>(h, 0x18));
			F<int32_t>(h, 0x2C) = z;
			if (z < 0) F<int32_t>(h, 0x2C) = 0;
			const uint32_t bucket = ot + (uint32_t)(F<int32_t>(h, 0x2C) >> shift) * 4;
			uint8_t *poly = pk;
			uint8_t *mode1 = pk + 0x18;
			uint8_t *mode2 = pk + 0x20;
			pk += 0x28;
			F<uint32_t>(mode1, 0) = 0x1000000;
			F<uint32_t>(mode1, 4) = 0xE1000220;
			InsertPrimAutoDepth(bucket, mode1);
			InsertPrimAutoDepth(bucket, poly);
			F<uint32_t>(mode2, 0) = 0x1000000;
			F<uint32_t>(mode2, 4) = 0xE1000240;
			InsertPrimAutoDepth(bucket, mode2);
		}
		F<const uint8_t *>(h, 0x20) = rec;
		return (uint32_t)pk;
	}

	static uint32_t __cdecl GouraudQuads(uint8_t *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		uint8_t *pk = (uint8_t *)cursor;
		const uint8_t *list = F<uint8_t *>(h, 0x20);
		const int32_t count = *(const int32_t *)list;
		const uint8_t *rec = list + 4;
		const uint8_t *verts = F<uint8_t *>(h, 4);
		F<const uint8_t *>(h, 0x20) = rec;
		if (count <= 0)
		{
			F<const uint8_t *>(h, 0x20) = rec;
			return (uint32_t)pk;
		}
		for (int32_t k = count; k != 0; k--, rec += 0x18)
		{
			GteLoadV012(verts + (uint32_t)*(const uint16_t *)(rec + 4) * 4, verts + (uint32_t)*(const uint16_t *)(rec + 6) * 4, verts + (uint32_t)*(const uint16_t *)(rec + 8) * 4);
			GteRTPT();
			F<uint32_t>(pk, 0) = 0x8000000;
			const uint32_t code = *(const uint32_t *)rec;
			F<uint32_t>(pk, 4) = code;
			if (F<uint8_t>(h, 0x1C) & 2) F<uint32_t>(pk, 4) = code | 0x2000000;
			if (F<uint8_t>(h, 0x1C) & 8) F<uint32_t>(pk, 4) &= 0xFDFFFFFF;
			GteReadFLAG(h + 0x30);
			if (F<uint32_t>(h, 0x30) & 0x60000) continue;
			GteNCLIP();
			uint32_t clip = 0;
			GteReadMAC0(h + 0x24);
			if (F<int32_t>(h, 0x24) < 0 && !(F<uint8_t>(h, 0x1C) & 0x20)) continue;
			GteReadSXY012(pk + 8, pk + 0x10, pk + 0x18);
			GteLoadV0(verts + (uint32_t)*(const uint16_t *)(rec + 0xA) * 4);
			GteRTPS();
			if (OutX(pk + 8)) clip = 1;
			if (OutX(pk + 0x10)) clip |= 2;
			if (OutX(pk + 0x18)) clip |= 4;
			if (OutY(pk + 8)) clip |= 0x10;
			if (OutY(pk + 0x10)) clip |= 0x20;
			if (OutY(pk + 0x18)) clip |= 0x40;
			GteReadSXY2(pk + 0x20);
			GteAVSZ4();
			if (OutX(pk + 0x20)) clip |= 8;
			if (OutY(pk + 0x20)) clip |= 0x80;
			if ((clip & 0xF) == 0xF) continue;
			if ((clip & 0xF0) == 0xF0) continue;
			GteReadOTZWord(h + 0x2C);
			if (F<uint8_t>(h, 0x1C) & 0x80)
			{
				GteLoadRGB012(rec + 0xC, rec + 0x10, rec + 0x14);
				GteSetIR0(F<int32_t>(h, 0xC));
				GteDPCT();
				GteStoreRGB012(pk + 0xC, pk + 0x14, pk + 0x1C);
				GteLoadRGBC(pk + 4);
				GteDPCS();
				GteStoreRGB2(pk + 4);
			}
			else
			{
				F<uint32_t>(pk, 0xC) = *(const uint32_t *)(rec + 0xC);
				F<uint32_t>(pk, 0x14) = *(const uint32_t *)(rec + 0x10);
				F<uint32_t>(pk, 0x1C) = *(const uint32_t *)(rec + 0x14);
			}
			int32_t z = (int32_t)((uint32_t)F<int32_t>(h, 0x2C) + (uint32_t)F<int32_t>(h, 0x18));
			F<int32_t>(h, 0x2C) = z;
			if (z < 0) F<int32_t>(h, 0x2C) = 0;
			const uint32_t bucket = ot + (uint32_t)(F<int32_t>(h, 0x2C) >> shift) * 4;
			uint8_t *poly = pk;
			uint8_t *mode1 = pk + 0x24;
			uint8_t *mode2 = pk + 0x2C;
			pk += 0x58;
			F<uint32_t>(mode1, 0) = 0x1000000;
			F<uint32_t>(mode1, 4) = 0xE1000220;
			InsertPrimAutoDepth(bucket, mode1);
			InsertPrimAutoDepth(bucket, poly);
			F<uint32_t>(mode2, 0) = 0x1000000;
			F<uint32_t>(mode2, 4) = 0xE1000240;
			InsertPrimAutoDepth(bucket, mode2);
		}
		F<const uint8_t *>(h, 0x20) = rec;
		return (uint32_t)pk;
	}

	static uint32_t __cdecl GouraudTexQuads(uint8_t *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		uint8_t *pk = (uint8_t *)cursor;
		const uint8_t *list = F<uint8_t *>(h, 0x20);
		const int32_t count = *(const int32_t *)list;
		const uint8_t *rec = list + 4;
		const uint8_t *verts = F<uint8_t *>(h, 4);
		F<const uint8_t *>(h, 0x20) = rec;
		if (count <= 0)
		{
			F<const uint8_t *>(h, 0x20) = rec;
			return (uint32_t)pk;
		}
		for (int32_t k = count; k != 0; k--, rec += 0x24)
		{
			GteLoadV012(verts + (uint32_t)*(const uint16_t *)(rec + 4) * 4, verts + (uint32_t)*(const uint16_t *)(rec + 6) * 4, verts + (uint32_t)*(const uint16_t *)(rec + 8) * 4);
			GteRTPT();
			F<uint32_t>(pk, 0) = 0xC000000;
			const uint32_t code = *(const uint32_t *)rec;
			F<uint32_t>(pk, 4) = code;
			if (F<uint8_t>(h, 0x1C) & 2) F<uint32_t>(pk, 4) = code | 0x2000000;
			if (F<uint8_t>(h, 0x1C) & 8) F<uint32_t>(pk, 4) &= 0xFDFFFFFF;
			F<uint32_t>(pk, 0xC) = *(const uint32_t *)(rec + 0xC);
			F<uint32_t>(pk, 0x18) = *(const uint32_t *)(rec + 0x10);
			const uint32_t uv = *(const uint32_t *)(rec + 0x14);
			F<uint32_t>(pk, 0x24) = uv;
			F<uint32_t>(pk, 0x30) = uv >> 16;
			GteReadFLAG(h + 0x30);
			if (F<uint32_t>(h, 0x30) & 0x60000) continue;
			GteNCLIP();
			uint32_t clip = 0;
			GteReadMAC0(h + 0x24);
			if (F<int32_t>(h, 0x24) < 0 && !(F<uint8_t>(h, 0x1C) & 0x20)) continue;
			GteReadSXY012(pk + 8, pk + 0x14, pk + 0x20);
			GteLoadV0(verts + (uint32_t)*(const uint16_t *)(rec + 0xA) * 4);
			GteRTPS();
			if (OutX(pk + 8)) clip = 1;
			if (OutX(pk + 0x14)) clip |= 2;
			if (OutX(pk + 0x20)) clip |= 4;
			if (OutY(pk + 8)) clip |= 0x10;
			if (OutY(pk + 0x14)) clip |= 0x20;
			if (OutY(pk + 0x20)) clip |= 0x40;
			GteReadSXY2(pk + 0x2C);
			GteAVSZ4();
			if (OutX(pk + 0x2C)) clip |= 8;
			if (OutY(pk + 0x2C)) clip |= 0x80;
			if ((clip & 0xF) == 0xF) continue;
			if ((clip & 0xF0) == 0xF0) continue;
			GteReadOTZWord(h + 0x2C);
			if (F<uint8_t>(h, 0x1C) & 0x80)
			{
				GteLoadRGB012(rec + 0x18, rec + 0x1C, rec + 0x20);
				GteSetIR0(F<int32_t>(h, 0xC));
				GteDPCT();
				GteStoreRGB012(pk + 0x10, pk + 0x1C, pk + 0x28);
				GteLoadRGBC(pk + 4);
				GteDPCS();
				GteStoreRGB2(pk + 4);
			}
			else
			{
				F<uint32_t>(pk, 0x10) = *(const uint32_t *)(rec + 0x18);
				F<uint32_t>(pk, 0x1C) = *(const uint32_t *)(rec + 0x1C);
				F<uint32_t>(pk, 0x28) = *(const uint32_t *)(rec + 0x20);
			}
			int32_t z = (int32_t)((uint32_t)F<int32_t>(h, 0x2C) + (uint32_t)F<int32_t>(h, 0x18));
			F<int32_t>(h, 0x2C) = z;
			if (z < 0) F<int32_t>(h, 0x2C) = 0;
			const uint32_t bucket = ot + (uint32_t)(F<int32_t>(h, 0x2C) >> shift) * 4;
			uint8_t *poly = pk;
			uint8_t *mode = pk + 0x34;
			pk = mode + 8;
			F<uint32_t>(mode, 0) = 0x1000000;
			F<uint32_t>(mode, 4) = 0xE1000220;
			InsertPrimAutoDepth(bucket, mode);
			InsertPrimAutoDepth(bucket, poly);
		}
		F<const uint8_t *>(h, 0x20) = rec;
		return (uint32_t)pk;
	}

	static uint32_t __cdecl RenderModel(uint8_t *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		if (!(F<uint32_t>(h, 0x1C) & 0x2000))
			F<uint32_t>(h, 4) = F<uint32_t>(h, 0) + 8;
		{
			const uint8_t *model = F<uint8_t *>(h, 0);
			const uint32_t b = F<uint8_t>(h, 0xA);
			F<const uint8_t *>(h, 0x20) = model + *(const uint32_t *)model;
			const uint32_t g = F<uint8_t>(h, 9);
			const uint32_t r = F<uint8_t>(h, 8);
			GteSetFarColor(r, g, b);
		}
		uint32_t c = cursor;
		for (int i = 0; i < 8; i++)
		{
			const uint8_t *list = F<uint8_t *>(h, 0x20);
			if (*(const uint32_t *)list == 0)
			{
				F<const uint8_t *>(h, 0x20) = list + 4;
				continue;
			}
			switch (i)
			{
				case 0: c = CallList(game_addr(LIST_FLAT_TRI), h, ot, shift, c); break;
				case 1: c = FlatQuads(h, ot, shift, c); break;
				case 2: c = CallList(game_addr(LIST_1), h, ot, shift, c); break;
				case 3: c = CallList(game_addr(LIST_2), h, ot, shift, c); break;
				case 4: c = CallList(game_addr(LIST_3), h, ot, shift, c); break;
				case 5: c = GouraudQuads(h, ot, shift, c); break;
				case 6: c = CallList(game_addr(LIST_4), h, ot, shift, c); break;
				default: c = GouraudTexQuads(h, ot, shift, c); break;
			}
		}
		return c;
	}

	static void __cdecl PrimPart(prim::Layout *l, prim::Record *r, int arg_)
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
		if (r->flags & 0x40000) RotationFromAnglesB(r->rot, &m);
		else if (r->flags & 0x80000) RotationFromAnglesC(r->rot, &m);
		else RotationFromAngles(r->rot, &m);
		int16_t off[4] = {};
		const int32_t scaled = arg->scaled;
		if (scaled)
		{
			off[0] = (int16_t)(mul32(r->pos[0], arg->scale[0]) >> 12);
			off[1] = (int16_t)(mul32(r->pos[1], arg->scale[1]) >> 12);
			off[2] = (int16_t)(mul32(r->pos[2], arg->scale[2]) >> 12);
		}
		else
		{
			off[0] = r->pos[0];
			off[1] = r->pos[1];
			off[2] = r->pos[2];
		}
		if (scaled) Scale3DMatrix(&m, arg->scale);
		const uint32_t flags = r->flags;
		if (flags & 0x1000)
		{
			GteSetRotMatrixCtrl(&Camera());
			GteLoadV0(arg->pos);
			GteMVMVA_RotV0();
			GteReadMAC123(m.t);
			GteSetRotMatrixCtrl(&EffCam());
			GteLoadV0(off);
			GteMVMVA_RotV0();
			int32_t mac[3];
			GteReadMAC123(mac);
			m.t[0] = (int32_t)((uint32_t)m.t[0] + (uint32_t)mac[0]);
			m.t[1] = (int32_t)((uint32_t)m.t[1] + (uint32_t)mac[1]);
			m.t[2] = (int32_t)((uint32_t)m.t[2] + (uint32_t)mac[2]);
			MatrixMultiply(&EffCam(), &m);
		}
		else if (flags & 0x200)
		{
			GteSetRotMatrixCtrl(&Camera());
			GteLoadV0(arg->pos);
			GteMVMVA_RotV0();
			GteReadMAC123(m.t);
			m.t[0] = (int32_t)((uint32_t)m.t[0] + (uint32_t)(int32_t)off[0]);
			m.t[1] = (int32_t)((uint32_t)m.t[1] + (uint32_t)(int32_t)off[1]);
			m.t[2] = (int32_t)((uint32_t)m.t[2] + (uint32_t)(int32_t)off[2]);
		}
		else
		{
			off[0] = (int16_t)(off[0] + arg->pos[0]);
			off[1] = (int16_t)(off[1] + arg->pos[1]);
			off[2] = (int16_t)(off[2] + arg->pos[2]);
			GteSetRotMatrixCtrl(&Camera());
			GteLoadV0(off);
			GteMVMVA_RotV0();
			GteReadMAC123(m.t);
			MatrixMultiply(&Camera(), &m);
		}
		m.t[0] = (int32_t)((uint32_t)m.t[0] + (uint32_t)Camera().t[0]);
		m.t[1] = (int32_t)((uint32_t)m.t[1] + (uint32_t)Camera().t[1]);
		m.t[2] = (int32_t)((uint32_t)m.t[2] + (uint32_t)Camera().t[2]);
		if (!(*(const uint32_t *)r->scale == 0x10001000 && r->scale[2] == 0x1000))
		{
			if (flags & 0x100)
			{
				Mat4x3 d = {};
				d.m[0][0] = r->scale[0];
				d.m[1][1] = r->scale[1];
				d.m[2][2] = r->scale[2];
				MatrixMultiply3(&m, &d);
			}
			else
			{
				int32_t v[3] = { r->scale[0], r->scale[1], r->scale[2] };
				Scale3DMatrix(&m, v);
			}
		}
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		*(uint32_t *)(h + 0x1C) = (r->flags & 0x4000) ? 0x2000 : 0x2030;
		*(int32_t *)(h + 0xC) = r->a;
		if (r->a != 0)
		{
			*(uint32_t *)(h + 0x1C) |= 0xC0;
			*(uint32_t *)(h + 8) = *(const uint32_t *)r->rgb;
		}
		*(int32_t *)(h + 0x18) = arg->depth;
		if (((const char *)arg->objs)[(int16_t)r->flags_lo] == '1')
			PacketCursor() = RenderModel(h, OT() + 0x44, 2, PacketCursor());
		else
			PacketCursor() = RenderPrimModel2(h, OT() + 0x44, 2, PacketCursor());
		FieldFree(0x58);
	}

	static void SaveEntityFlags()
	{
		for (int k = 0; k < 7; k++) SavedFlags()[k] = *(const uint16_t *)Entity(k);
	}
	static void RestoreEntityFlags()
	{
		for (int k = 0; k < 7; k++)
		{
			uint8_t *e = Entity(k);
			const uint16_t w = *(const uint16_t *)e;
			if (w & 2) *(uint16_t *)e = (uint16_t)((((uint32_t)(uint8_t)SavedFlags()[k] ^ w) & 4) ^ w);
		}
	}
	static void EntitiesFlag4On()
	{
		for (int k = 0; k < 7; k++)
		{
			uint8_t *e = Entity(k);
			const uint16_t w = *(const uint16_t *)e;
			if (w & 2) *(uint16_t *)e = (uint16_t)(w | 4);
		}
	}
	static void ClearSparks()
	{
		uint8_t *p = (uint8_t *)TexBase() + 0x182B;
		for (int k = 0x80; k != 0; k--, p += 0x10) *p = 0;
		SparkCursor() = (uint8_t *)TexBase() + 0x181C;
	}
	static void ClearShards()
	{
		uint8_t *p = (uint8_t *)TexBase() + 0x201C;
		for (int k = 0x40; k != 0; k--, p += 0x10) *(int16_t *)p = 0;
		ShardCursor() = (uint8_t *)TexBase() + 0x201C;
	}
	static Spark *SparkAlloc()
	{
		uint8_t *p = SparkCursor();
		const uint32_t tex = TexBase();
		if (p[0xF] != 0)
		{
			p = (uint8_t *)tex + 0x181C;
			if (p[0xF] != 0)
			{
				int32_t n = 0x80;
				do
				{
					p += 0x10;
					if (--n == 0) return nullptr;
				} while (p[0xF] != 0);
			}
		}
		if ((uint32_t)p >= tex + 0x200C) SparkCursor() = (uint8_t *)tex + 0x181C;
		else SparkCursor() = p + 0x10;
		return (Spark *)p;
	}
	static Shard *ShardAlloc()
	{
		uint8_t *p = ShardCursor();
		const uint32_t tex = TexBase();
		if (*(const int16_t *)p != 0)
		{
			p = (uint8_t *)tex + 0x201C;
			if (*(const int16_t *)p != 0)
			{
				int32_t n = 0x40;
				do
				{
					p += 0x10;
					if (--n == 0) return nullptr;
				} while (*(const int16_t *)p != 0);
			}
		}
		if ((uint32_t)p >= tex + 0x240C) ShardCursor() = (uint8_t *)tex + 0x201C;
		else ShardCursor() = p + 0x10;
		return (Shard *)p;
	}
	static void SparksUpdate()
	{
		Spark *s = (Spark *)(TexBase() + 0x181C);
		for (int k = 0x80; k != 0; k--, s++)
		{
			if (s->life == 0) continue;
			s->life--;
			s->pos[0] = (int16_t)(s->pos[0] + (int16_t)((int16_t)s->vel[0] << 2));
			s->pos[1] = (int16_t)(s->pos[1] + (int16_t)((int16_t)s->vel[1] << 2));
			s->pos[2] = (int16_t)(s->pos[2] + (int16_t)((int16_t)s->vel[2] << 2));
		}
	}
	static void ShardsUpdate()
	{
		Shard *s = (Shard *)(TexBase() + 0x201C);
		for (int k = 0x40; k != 0; k--, s++)
		{
			if (s->life == 0) continue;
			const int16_t d = s->dsy;
			s->life--;
			s->sy = (int16_t)(s->sy + d);
			s->sxz = (int16_t)(s->sxz + s->dsxz);
		}
	}

	static void SparksDraw(const Spark *s)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		F<uint16_t>(h, 0x60) = 0;
		F<uint16_t>(h, 0x58) = 0;
		F<uint16_t>(h, 0x50) = 0;
		F<uint16_t>(h, 0x48) = 0;
		for (int n = 0x80; n != 0; n--, s++)
		{
			if (s->life == 0) continue;
			GteSetRotMatrixCtrl(&Camera());
			GteSetTransVectorCtrl(&Camera());
			GteLoadV0(s->pos);
			GteMVMVA_RotV0Tr();
			GteReadMAC123((int32_t *)(h + 0x98));
			ff8fx::fill<(0x14)>(h + 0x84, 0);
			F<uint16_t>(h, 0x94) = (uint16_t)s->scale;
			F<uint16_t>(h, 0x8C) = (uint16_t)s->scale;
			F<uint16_t>(h, 0x84) = (uint16_t)s->scale;
			const uint8_t *seq = s->seq;
			*(const uint8_t **)h = seq;
			const int32_t frames = *(const int16_t *)(seq + 8);
			const int32_t k = frames - ((int32_t)(int8_t)s->life - 1) % frames;
			const uint8_t *frame = seq + *(const int16_t *)(seq + 8 + 2 * k);
			*(const uint8_t **)(h + 0x2C) = frame;
			const int32_t word = *(const int32_t *)frame;
			F<int32_t>(h, 0x30) = word;
			if (word < 0)
			{
				h[0x25] |= 1;
				F<uint32_t>(h, 0x30) = (uint32_t)word & 0x7FFFFFFF;
			}
			*(const uint8_t **)(h + 0x2C) = frame + 4;
			PacketCursor() = Flipbook(h, OT() + 0x44, 2, PacketCursor());
		}
		FieldFree(0xB4);
	}

	static int32_t BonePosition(uint8_t *e, int32_t bone, int32_t *out)
	{
		const uint8_t *skel = **(uint8_t *const *const *)(e + 0x64);
		if (bone >= 0xF0) bone = skel[bone - 0xEC];
		const uint8_t *b = skel + 0x10 + (uint32_t)bone * 0x30;
		if (*(const int16_t *)b < 0) return 1;
		TransformVec3(e + 0x40, (const int32_t *)(b + 0x24), out);
		out[0] = (int32_t)((uint32_t)((out[0] - (out[0] >> 31)) >> 1) + (uint32_t)F<int32_t>(e, 0x54));
		out[1] = (int32_t)((uint32_t)((out[1] - (out[1] >> 31)) >> 1) + (uint32_t)F<int32_t>(e, 0x58));
		out[2] = (int32_t)((uint32_t)((out[2] - (out[2] >> 31)) >> 1) + (uint32_t)F<int32_t>(e, 0x5C));
		return 0;
	}

	static void ShardsDraw(const Shard *s)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x78);
		const int32_t angle = (int32_t)(((Side() <= 0 ? 0u : 0xFFFFFFFFu) & 0xFFFFF800u) + 0x800u);
		F<uint32_t>(h, 0x38) = 0xFFFFFF00;
		F<uint32_t>(h, 0x3C) = 0x10;
		for (int n = 0x40; n != 0; n--, s++)
		{
			if (s->life == 0) continue;
			ShardMatrix(angle, s->sy, s->sxz, h);
			BonePosition(s->rec->entity, s->bone, (int32_t *)(h + 0x14));
			F<int32_t>(h, 0x18) = (int32_t)((uint32_t)F<int32_t>(h, 0x18) + (uint32_t)sdiv_pow2(s->rec->y_min + s->rec->y_max, 2));
			ComposeAffineTransform(&Camera(), (Mat4x3 *)h, (Mat4x3 *)h);
			GteSetRotMatrix((Mat4x3 *)h);
			GteSetTransVector((Mat4x3 *)h);
			F<uint32_t>(h, 0x20) = game_addr(SHARD_MODEL);
			PacketCursor() = RenderModel(h + 0x20, OT() + 0x44, 2, PacketCursor());
		}
		FieldFree(0x78);
	}

	static int32_t MeasureHeight(uint8_t *e, int16_t *out)
	{
		ComputeBonesWorldMatrices(e + 0x60, e + 0x40);
		uint8_t *h = (uint8_t *)FieldAlloc(0x18);
		F<uint32_t>(h, 4) = 0xFFFF0000;
		F<uint32_t>(h, 0) = 0x10000;
		F<uint32_t>(h, 0x14) = F<uint32_t>(e, 0x7C);
		const uint8_t *model = F<uint8_t *>(e, 0x64);
		const uint8_t *bones = *(const uint8_t *const *)model + 0x10;
		F<uint32_t>(h, 8) = 0;
		F<uint16_t>(h, 0xC) = 0x1000;
		const uint8_t *objs = *(const uint8_t *const *)(model + 4);
		const int32_t count = *(const int32_t *)objs;
		const uint32_t *offs = (const uint32_t *)(objs + 4);
		for (int32_t i = 0; i < count; i++)
		{
			const uint8_t *p = *(const uint8_t *const *)(model + 4) + offs[i];
			if (!(F<uint32_t>(h, 0x14) & (1u << (i & 31)))) continue;
			int32_t ng = *(const int16_t *)p;
			p += 2;
			if (ng <= 0) continue;
			do
			{
				const int32_t bone = *(const int16_t *)p;
				p += 2;
				const Mat4x3 *bm = (const Mat4x3 *)(bones + (uint32_t)bone * 0x30 + 0x10);
				GteSetRotMatrixCtrl(bm);
				GteSetTransVectorCtrl(bm);
				int32_t nv = *(const int16_t *)p;
				p += 2;
				for (; nv != 0; nv--)
				{
					GteLoadV0u(p);
					GteRTPS();
					p += 6;
					GteReadIR2(h + 0x10);
					const int32_t v = F<int16_t>(h, 0x10);
					if (F<int32_t>(h, 0) > v) F<int32_t>(h, 0) = v;
					if (F<int32_t>(h, 4) < v) F<int32_t>(h, 4) = v;
				}
			} while (--ng != 0);
		}
		out[0] = F<int16_t>(h, 4);
		out[1] = F<int16_t>(h, 0);
		FieldFree(0x18);
		BuildBoneMatricesFromPose(e + 0x60);
		return out[1];
	}

	static void TargetArg(PrimArg &arg, const Target *rec, uint8_t *e)
	{
		arg.pos[0] = E16(e, 0x1C);
		arg.pos[2] = E16(e, 0x20);
		const int32_t v = rec->y_max + rec->y_min;
		arg.pos[1] = (int16_t)((int16_t)((v - (v >> 31)) >> 1) + E16(e, 0x1E));
		arg.morph = TexBase() + 0x241C;
		const int32_t d = rec->y_min - rec->y_max;
		arg.scaled = 1;
		arg.scale[2] = d;
		arg.scale[1] = d;
		arg.scale[0] = d;
		arg.objs = game_addr(OBJS_A);
		arg.depth = (int16_t)0xFF00;
	}

	static uint32_t __cdecl LiftTask(TaskNode *n)
	{
		SubNode *s = (SubNode *)n;
		const int32_t n12 = shl32(s->t.counter, 12);
		int32_t q = (mulhi_s(n12, (int32_t)0x8D3DCB09) + n12) >> 4;
		q += (int32_t)((uint32_t)q >> 31);
		Target *rec = s->t.rec;
		uint8_t *e = rec->entity;
		const int32_t slot = SlotOf(e);
		PrimArg arg;
		TargetArg(arg, rec, e);
		prim::play((prim::Layout *)s->layout, PrimPart, (int)&arg, 0);
		if (s->t.counter == 5)
		{
			Spark *p = SparkAlloc();
			if (p)
			{
				int32_t ang = 0x1000;
				do
				{
					const int32_t r = CrtRand();
					p->pos[0] = E16(e, 0x1C);
					p->pos[1] = E16(e, 0x1E);
					p->pos[2] = E16(e, 0x20);
					const int32_t speed = (r & 0xF) + 0x10;
					p->scale = (int16_t)((CrtRand() & 0x800) + 0xC00);
					p->vel[0] = (int8_t)(mul32(ComputeSin(ang), speed) >> 12);
					p->vel[2] = (int8_t)(mul32(ComputeCos(ang), speed) >> 12);
					p->vel[1] = 0;
					p->seq = (const uint8_t *)game_addr(BURST_SEQ);
					p->life = 0x10;
					ang -= 0x100;
					if (ang == 0) break;
					p = SparkAlloc();
				} while (p);
			}
		}
		const int32_t rest = 0x1000 - q;
		if (slot & 1)
		{
			const int16_t d = (int16_t)sdiv_pow2(rest, 3);
			rec->ang[0] = (int16_t)(rec->ang[0] + d);
			rec->ang[1] = (int16_t)(rec->ang[1] + d);
		}
		else
		{
			const int16_t d = (int16_t)sdiv_pow2(rest, 3);
			rec->ang[0] = (int16_t)(rec->ang[0] - d);
			rec->ang[1] = (int16_t)(rec->ang[1] - d);
		}
		E16(e, 0x1E) = (int16_t)sdiv_pow2(shl32(mul32(rest, rest) >> 12, 12), 12);
		s->t.counter++;
		if (s->t.counter >= 0x1E)
		{
			*(uint16_t *)e &= 0xFFF7;
			return TASK_END;
		}
		return 0;
	}

	static uint32_t __cdecl RingTask(TaskNode *n)
	{
		SubNode *s = (SubNode *)n;
		PrimArg arg;
		arg.scaled = 0;
		arg.objs = game_addr(OBJS_A);
		arg.pos[0] = s->pos[0];
		arg.pos[2] = s->pos[2];
		arg.pos[1] = s->pos[1];
		arg.morph = TexBase() + 0x241C;
		arg.depth = (int16_t)0xFF80;
		const int left = prim::play((prim::Layout *)s->layout, PrimPart, (int)&arg, 0);
		return left != 0 ? 0 : TASK_END;
	}

	static uint32_t __cdecl RiseTask(TaskNode *n)
	{
		SubNode *s = (SubNode *)n;
		Target *rec = s->t.rec;
		uint8_t *e = rec->entity;
		PrimArg arg;
		TargetArg(arg, rec, e);
		prim::play((prim::Layout *)s->layout, PrimPart, (int)&arg, 0);
		const uint32_t i = (uint32_t)((int32_t)s->t.counter - 0xD);
		const uint32_t len = (uint32_t)(int32_t)s->t.delay;
		if (i < len)
		{
			const int32_t t = (int32_t)((i << 12) / len);
			const int32_t slot = SlotOf(e);
			if (i == 0)
			{
				SubNode *ring = (SubNode *)AddTaskToQueue(QSub(), game_addr(ORIG_RingTask));
				ring->pos[0] = E16(e, 0x1C);
				ring->pos[2] = E16(e, 0x20);
				ring->pos[1] = 0;
				DecodeModelPrimLayout(game_addr(LAYOUT_RING), ring->layout, 0xD8);
			}
			const int16_t d = (int16_t)sdiv_pow2(t, 3);
			if (slot & 1)
			{
				rec->ang[0] = (int16_t)(rec->ang[0] + d);
				rec->ang[1] = (int16_t)(rec->ang[1] + d);
			}
			else
			{
				rec->ang[0] = (int16_t)(rec->ang[0] - d);
				rec->ang[1] = (int16_t)(rec->ang[1] - d);
			}
			int32_t v = shl32(mul32(t, t) >> 12, 14);
			v = (v - (v >> 31)) >> 1;
			v = sdiv_pow2(v, 12);
			E16(e, 0x1E) = (int16_t)(*(const int16_t *)((const uint8_t *)rec + 6) - (int16_t)v);
			int16_t vec[3];
			vec[2] = 0;
			vec[0] = 0;
			vec[1] = (int16_t)((rec->y_min + rec->y_max) >> 1);
			DrawTargetModel(e, rec->ang, vec, 0x1000);
			*e |= 4;
			s->t.counter++;
			return 0;
		}
		if (i < 0xC8)
		{
			if (i == len) E16(e, 0x1E) = (int16_t)0xD000;
			E16(e, 0x1E) = (int16_t)(E16(e, 0x1E) + (int16_t)0xFF00);
		}
		s->t.counter++;
		return 0;
	}

	static uint32_t __cdecl MasterTask(TaskNode *n)
	{
		MasterNode *m = (MasterNode *)n;
		if (m->counter < 0xC)
		{
			const int32_t v = (int32_t)m->counter * 2300;
			int32_t q = mulhi_s(v, 0x2AAAAAAB) >> 1;
			Flash() = q + (int32_t)((uint32_t)q >> 31);
		}
		else if (m->counter >= 0xFA)
		{
			const int32_t v = (0x106 - (int32_t)m->counter) * 2300;
			int32_t q = mulhi_s(v, 0x2AAAAAAB) >> 1;
			Flash() = q + (int32_t)((uint32_t)q >> 31);
		}
		else
			Flash() = 0x8FC;

		if ((uint32_t)((int32_t)m->counter - 0x32) < 0x1E)
		{
			if (m->counter == 0x32) DecodeModelPrimLayout(game_addr(LAYOUT_CENTRE), m->layout, 0x218);
			PrimArg arg;
			arg.pos[0] = (int16_t)CentreX();
			arg.pos[2] = (int16_t)CentreZ();
			arg.pos[1] = (int16_t)0xC000;
			arg.morph = TexBase() + 0x241C;
			arg.scaled = 0;
			arg.objs = game_addr(OBJS_A);
			arg.depth = (int16_t)0xFF80;
			prim::play((prim::Layout *)m->layout, PrimPart, (int)&arg, 0);
		}

		const int32_t s = (int32_t)m->counter - 0x50;
		if ((uint32_t)s < 0xB6)
		{
			const uint32_t w = (uint32_t)(s - 0x7E);
			if (w < 8)
			{
				int32_t level;
				if ((int32_t)w < 7)
				{
					const int32_t v = shl32((int32_t)w + 1, 12);
					int32_t q = (mulhi_s(v, (int32_t)0x92492493) + v) >> 2;
					level = q + (int32_t)((uint32_t)q >> 31);
				}
				else if ((int32_t)w < 8) level = 0x1000;
				else level = shl32(8 - (int32_t)w, 12);
				PacketCursor() = Tile(0xFF, 0xFF, 0xFF, level, PacketCursor());
			}
			bool behind = true;
			if (s == 0)
			{
				InitTaskQueuePool(QSub(), (void *)TexBase(), 0x114, 6);
				EntitiesFlag4On();
				DecodeModelPrimLayout(game_addr(LAYOUT_WAVE), m->layout, 0x1100);
				for (int32_t k = 0; k < m->ntargets; k++)
				{
					SubNode *t = (SubNode *)AddTaskToQueue(QSub(), game_addr(ORIG_LiftTask));
					Target *rec = &m->targets[k];
					*(int16_t *)(rec->entity + 0x1E) = 0x1000;
					if (SlotOf(rec->entity) & 1)
					{
						rec->ang[1] = (int16_t)0xE20B;
						rec->ang[0] = (int16_t)0xE20B;
					}
					else
					{
						rec->ang[1] = 0x1DF5;
						rec->ang[0] = 0x1DF5;
					}
					rec->ang[2] = 0;
					t->t.counter = 0;
					DecodeModelPrimLayout(game_addr(LAYOUT_LIFT), t->layout, 0xBC);
					t->t.rec = rec;
				}
			}
			else
			{
				if ((uint32_t)s >= 0xD && (uint32_t)s <= 0x28)
				{
					int32_t k = 4;
					Spark *p = SparkAlloc();
					while (p)
					{
						CrtRand();
						if (k & 1)
						{
							p->pos[1] = 0x1000;
							p->vel[1] = (int8_t)0xF4;
						}
						else
						{
							p->pos[1] = (int16_t)0xF000;
							p->vel[1] = 0xC;
						}
						const int32_t a = CrtRand();
						p->pos[0] = (int16_t)((uint32_t)(ComputeSin(a) >> 3) + CentreX());
						p->pos[2] = (int16_t)((uint32_t)((ComputeCos(a) >> 3) - shl32(Side(), 11)) + CentreZ());
						p->scale = 0x200;
						CrtRand();
						p->vel[2] = 0;
						p->vel[0] = 0;
						p->seq = (const uint8_t *)game_addr(SPARK_SEQ);
						p->life = 0x50;
						if (--k == 0) break;
						p = SparkAlloc();
					}
				}
				else if ((uint32_t)s >= 0x73 && (uint32_t)s < 0x83)
				{
					for (int32_t k = 0; k < m->ntargets; k++)
					{
						Target *rec = &m->targets[k];
						const int32_t height = rec->y_max - rec->y_min;
						uint8_t *e = rec->entity;
						int32_t j = 2;
						Shard *p = ShardAlloc();
						while (p)
						{
							p->life = 6;
							const int32_t nb = ***(const uint8_t *const *const *)(e + 0x64);
							p->bone = (int16_t)(mul32(CrtRand(), nb) >> 15);
							const int32_t r = CrtRand();
							p->rec = rec;
							const int32_t v = sdiv_pow2(mul32(r, height), 16) + height;
							p->sxz = (int16_t)v;
							const int16_t a = (int16_t)((int16_t)v >> 1);
							p->sy = a;
							p->dsy = (int16_t)(a >> 4);
							p->dsxz = (int16_t)(a >> 2);
							if (--j == 0) break;
							p = ShardAlloc();
						}
					}
				}
				if ((uint32_t)s >= 0x86)
				{
					const int32_t u = s - 0x86;
					if (u == 0)
					{
						for (int32_t k = 0; k < m->ntargets; k++)
						{
							Target *rec = &m->targets[k];
							*(uint16_t *)rec->entity &= 0xFFF3;
							*(uint32_t *)(rec->entity + 0x1C) = rec->pos_xy;
							*(uint32_t *)(rec->entity + 0x20) = rec->pos_z;
						}
						RestoreEntityFlags();
					}
					if (Flash() >= 0x8FC && u < 0x20) Flash() = 0x1000 - sdiv_pow2(u * 1796, 5);
					behind = false;
				}
			}
			int32_t zoff = 0;
			int16_t depth = 0;
			if (behind)
			{
				zoff = shl32(-Side(), 11);
				Flash() = 0x1000;
				int16_t vec[3];
				vec[2] = 0;
				vec[0] = 0;
				for (int32_t k = 0; k < m->ntargets; k++)
				{
					Target *rec = &m->targets[k];
					vec[1] = (int16_t)((rec->y_max + rec->y_min) >> 1);
					DrawTargetModel(rec->entity, rec->ang, vec, 0x800);
					*rec->entity |= 4;
				}
			}
			else
				depth = (int16_t)0xFC00;
			PrimArg arg;
			arg.pos[0] = (int16_t)CentreX();
			arg.morph = TexBase() + 0x241C;
			arg.pos[1] = 0;
			arg.scaled = 0;
			arg.pos[2] = (int16_t)(zoff + (int32_t)CentreZ());
			arg.objs = game_addr(OBJS_WAVE);
			arg.depth = depth;
			prim::play((prim::Layout *)m->layout, PrimPart, (int)&arg, 0);
		}

		SparksDraw((const Spark *)(TexBase() + 0x181C));
		ShardsDraw((const Shard *)(TexBase() + 0x201C));
		SparksUpdate();
		ShardsUpdate();

		if (m->counter == 0) BdPlaySE(SOUND_0(), 0, 0x80);
		else if (m->counter == 0x50) BdPlaySE(SOUND_50(), 0, 0x80);
		else if (m->counter == 0xAA) BdPlaySE(SOUND_AA(), 0, 0x80);
		if (m->counter == 0x102) ReleaseVoice(m->voice);

		{
			const uint32_t c1 = (uint32_t)((int32_t)m->counter - 1);
			if (c1 < 0x18 && (c1 & 7) == 0 && (int32_t)(c1 >> 3) < m->ntargets)
			{
				Target *rec = &m->targets[c1 >> 3];
				SubNode *t = (SubNode *)AddTaskToQueue(QSub(), game_addr(ORIG_RiseTask));
				*rec->entity |= 8;
				MeasureHeight(rec->entity, &rec->y_max);
				t->t.counter = 0;
				t->t.delay = (int16_t)(0x32 - (int32_t)c1);
				DecodeModelPrimLayout(game_addr(LAYOUT_RISE), t->layout, 0x100);
				t->t.rec = rec;
			}
		}

		const int32_t X = (int32_t)CentreX(), Z = (int32_t)CentreZ(), sd = Side();
		if (m->counter == 0)
		{
			CameraArmReturn();
			ViewWord() = 0x120;
			CameraZoom() = 0;
			CamLookAt()[1] = (int16_t)0xFCD9;
			CamLookAt()[0] = (int16_t)(X + 8);
			CamEye()[1] = (int16_t)0xF491;
			CamLookAt()[2] = (int16_t)(Z + sd * 340);
			CamEye()[0] = (int16_t)(X + 0x5A2);
			CamEye()[2] = (int16_t)(Z + sd * 3431);
		}
		if ((uint32_t)((int32_t)m->counter - 0x14) < 0x27)
		{
			const uint32_t a = (uint32_t)((int32_t)m->counter - 0x14) << 12;
			int16_t k0[6], k1[6];
			k1[0] = (int16_t)(X + 8);
			k1[1] = (int16_t)0xFCD9;
			k1[2] = (int16_t)(Z + sd * 340);
			k1[3] = (int16_t)(X + 0x5A2);
			k1[4] = (int16_t)0xF491;
			k1[5] = (int16_t)(Z + sd * 3431);
			k0[0] = (int16_t)(X - 0x66);
			k0[1] = (int16_t)0xDF3F;
			k0[2] = (int16_t)(Z + sd * 2438);
			k0[3] = (int16_t)(X + 0x1A2);
			k0[4] = 0x6CD;
			k0[5] = (int16_t)(Z + sd * 10573);
			const uint32_t hi = mulhi_u(a, 0xAF286BCB);
			const uint32_t angle = ((((a - hi) >> 1) + hi) >> 5) >> 2;
			CamLerp(k1, k0, 0x1000 - ComputeCos((int32_t)angle));
		}
		if (m->counter == 0x3B)
		{
			CamLookAt()[1] = (int16_t)0xBFC3;
			CamLookAt()[0] = (int16_t)(X - 0x16D);
			CamEye()[1] = (int16_t)0xCAB7;
			CamLookAt()[2] = (int16_t)(Z + sd * 538);
			CamEye()[0] = (int16_t)(X - 0x169);
			CamEye()[2] = (int16_t)(Z + sd * 8304);
		}
		if ((uint32_t)((int32_t)m->counter - 0x4F) < 0x28)
		{
			const uint32_t a = (uint32_t)((int32_t)m->counter - 0x4F) << 12;
			int16_t k0[6], k1[6];
			ViewWord() = 0xC8;
			k0[0] = (int16_t)(X - 0x1D);
			k0[1] = 0xBB7;
			k0[2] = (int16_t)(Z - sd * 291);
			k0[3] = (int16_t)(X - 0x18D5);
			k0[4] = (int16_t)0xE9DA;
			k0[5] = (int16_t)(Z + sd * 4421);
			k1[0] = (int16_t)(X - 0xCC);
			k1[1] = (int16_t)0xFFF3;
			k1[2] = (int16_t)(Z - sd * 2255);
			k1[3] = (int16_t)(X + 0x1B);
			k1[4] = (int16_t)0xFBCF;
			k1[5] = (int16_t)(Z + sd * 4566);
			const uint32_t hi = mulhi_u(a, 0xA41A41A5);
			const uint32_t t = (((a - hi) >> 1) + hi) >> 5;
			CamLerp(k0, k1, (int32_t)t);
		}
		if ((uint32_t)((int32_t)m->counter - 0x77) < 0x3C)
		{
			const uint32_t a = (uint32_t)((int32_t)m->counter - 0x77) << 12;
			int16_t k0[6], k1[6];
			k0[0] = (int16_t)(X - 0xCC);
			k0[1] = (int16_t)0xFFF3;
			k0[2] = (int16_t)(Z - sd * 2255);
			k0[3] = (int16_t)(X + 0x1B);
			k0[4] = (int16_t)0xFBCF;
			k0[5] = (int16_t)(Z + sd * 4566);
			k1[0] = (int16_t)(X - 0xCD);
			k1[1] = (int16_t)0xFFF2;
			k1[2] = (int16_t)(Z - sd * 2256);
			k1[3] = (int16_t)(X + 0x22BE);
			k1[4] = (int16_t)0xFB3E;
			k1[5] = (int16_t)(Z - sd * 2368);
			const uint32_t t = mulhi_u(a, 0x22B63CBF) >> 3;
			CamLerp(k0, k1, (int32_t)t);
		}
		if (m->counter == 0xD5)
		{
			ViewWord() = var<int16_t>(game_addr(game::Battle_Camera_ReturnView_Param1D8E038));
			CamLookAt()[0] = (int16_t)(X + 0x2BE);
			CamEye()[0] = (int16_t)(X + 0x1AA2);
			CamLookAt()[1] = 0x63;
			CamEye()[1] = (int16_t)0xE1DE;
			CamLookAt()[2] = (int16_t)(Z + sd * 459);
			CamEye()[2] = (int16_t)(Z + sd * 445);
			StreamLoad(0x168, TexBase() + 0x269C, 1);
			StreamLoad(0x169, TexBase() + 0x269C, 0);
		}
		StreamPump();
		if (m->counter == 0xFC)
		{
			const ActionData *act = ((const CastContext *)Ctx())->actions;
			ApplyActionResultToTargets(act->targets, act->target_count);
		}
		if (m->counter >= 0x106)
		{
			RestoreEntityFlags();
			Flash() = 0;
			ViewWord() = var<int16_t>(game_addr(game::Battle_Camera_ReturnView_Param1D8E038));
			QSub()->tail = nullptr;
			QSub()->head = nullptr;
			return StreamPending(1) != 0 ? TASK_END : 0;
		}
		m->counter++;
		return 0;
	}

	static uint32_t __cdecl RootTask(TaskNode *n)
	{
		RootNode *r = (RootNode *)n;
		if (r->parity)
		{
			PacketCursor() = TexBase() + 0xAE9C;
			ArenaEnd() = TexBase() + 0x1AE9C;
			r->parity = 0;
		}
		else
		{
			PacketCursor() = TexBase() + 0x1AE9C;
			ArenaEnd() = TexBase() + 0x2AE9C;
			r->parity = 1;
		}
		Flash() = 0;
		if (r->counter == 1 && !r->started)
		{
			r->started = 1;
			SaveEntityFlags();
			ClearSparks();
			ClearShards();
			StreamStateInit((void *)(TexBase() + 0x678));
			InitTaskQueuePool(QMaster(), (void *)(TexBase() + 0x6C0), 0x115C, 1);
			InitTaskQueuePool(QSub(), (void *)TexBase(), 0x114, 6);
			MasterNode *m = (MasterNode *)AddTaskToQueue(QMaster(), game_addr(ORIG_MasterTask));
			Memset32(&m->counter, 0, 0x454);
			m->voice = ClaimVoiceSlot(SOUND_Voice(), 1, 0x80);
			const ActionData *act = ((const CastContext *)Ctx())->actions;
			int16_t minx = 0x7FFF, miny = 0x7FFF, minz = 0x7FFF;
			int16_t maxx = (int16_t)0x8001, maxy = (int16_t)0x8001, maxz = (int16_t)0x8001;
			m->ntargets = act->target_count;
			for (int32_t i = 0; i < m->ntargets; i++)
			{
				Target *rec = &m->targets[i];
				uint8_t *e = Entity(((const CastContext *)Ctx())->actions->targets[i * TARGET_STRIDE]);
				rec->entity = e;
				rec->pos_xy = *(const uint32_t *)(e + 0x1C);
				rec->pos_z = *(const uint32_t *)(e + 0x20);
				*(uint32_t *)rec->ang = InitAngles0();
				*(uint32_t *)&rec->ang[2] = InitAngles1();
				int16_t p[4];
				GetDefaultEffectPosition(e, p);
				if (p[0] < minx) minx = p[0];
				if (p[0] > maxx) maxx = p[0];
				if (p[1] < miny) miny = p[1];
				if (p[1] > maxy) maxy = p[1];
				if (p[2] < minz) minz = p[2];
				if (p[2] > maxz) maxz = p[2];
				Side() = SlotOf(e) >= 3 ? -1 : 1;
			}
			int32_t v = maxx + minx;
			*(int16_t *)&CentreX() = (int16_t)((v - (v >> 31)) >> 1);
			v = maxy + miny;
			CentreY() = (int16_t)((v - (v >> 31)) >> 1);
			v = maxz + minz;
			*(int16_t *)&CentreZ() = (int16_t)((v - (v >> 31)) >> 1);
		}
		int a = (int)n;
		if (r->started)
		{
			EffectCameraMatrix(&Camera(), &EffCam());
			a = ExecuteTaskQueue(QMaster());
			ExecuteTaskQueue(QSub());
		}
		if (Flash() != FlashPrev())
		{
			SetScreenFlash((uint32_t)Flash(), 0);
			const int32_t f = Flash();
			if (f >= 0x1000)
			{
				for (uint32_t b = game_addr(game::unk_1D98991); b < game_addr(game::unk_1D98A41); b += 0x2C) *(uint8_t *)b &= 0xFD;
			}
			else if (FlashPrev() >= 0x1000)
			{
				for (uint32_t b = game_addr(game::unk_1D98991); b < game_addr(game::unk_1D98A41); b += 0x2C) *(uint8_t *)b |= 2;
			}
			FlashPrev() = f;
		}
		if (r->started && a == 0)
		{
			SetScreenFlash(0, 0);
			return TASK_END;
		}
		r->counter++;
		return 0;
	}
}

	void register_mag070_shockwave_pulsar()
	{
		register_port(sp070::ORIG_RootTask, (void *)sp070::RootTask, "P070 RootTask", 70);
		register_port(sp070::ORIG_MasterTask, (void *)sp070::MasterTask, "P070 MasterTask", 70);
		register_port(sp070::ORIG_LiftTask, (void *)sp070::LiftTask, "P070 LiftTask", 70);
		register_port(sp070::ORIG_RiseTask, (void *)sp070::RiseTask, "P070 RiseTask", 70);
		register_port(sp070::ORIG_RingTask, (void *)sp070::RingTask, "P070 RingTask", 70);
	}
}

