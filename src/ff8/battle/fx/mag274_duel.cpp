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

// Effect 274: Duel: the effect of the whole Duel and of its six

#include "mag_common.h"

namespace ff8fx
{
namespace duel274
{
	using namespace eng;
	using namespace magc;

	inline uint32_t *SavedFlags() { return (uint32_t *)game_addr(game::MAG_274_DUEL_SavedEntityFlags); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::MAG_274_DUEL_TexBase)); }
	inline uint32_t &Alloc() { return var<uint32_t>(game_addr(game::MAG_274_DUEL_FileBuffer)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_274_DUEL_PacketCursor)); }
	inline Mat4x3 *CamCopy() { return (Mat4x3 *)game_addr(game::MAG_274_DUEL_CameraCopy); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::MAG_274_DUEL_CastContext)); }
	inline uint8_t *&Caster() { return var<uint8_t *>(game_addr(game::MAG_274_DUEL_Caster)); }
	inline TaskQueue *QRocks() { return (TaskQueue *)game_addr(game::MAG_274_DUEL_RockQueue); }
	inline TaskQueue *QFlashes() { return (TaskQueue *)game_addr(game::MAG_274_DUEL_HitFlashQueue); }
	inline TaskQueue *QSparks() { return (TaskQueue *)game_addr(game::MAG_274_DUEL_SparkQueue); }
	inline TaskQueue *QMoves() { return (TaskQueue *)game_addr(game::MAG_274_DUEL_MoveQueue); }
	inline uint8_t &NewMove() { return var<uint8_t>(game_addr(game::MAG_274_DUEL_NewMoveFlag)); }
	inline uint8_t &PostedMode() { return var<uint8_t>(game_addr(game::MAG_274_DUEL_PostedMode)); }
	inline int16_t *SavedPartyY() { return (int16_t *)game_addr(game::MAG_274_DUEL_SavedPartyY); }
	inline uint32_t OTBase() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)); }
	inline uint32_t &FramePackets() { return var<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)); }

	static constexpr game::Id ORIG_RootTask = game::MAG_274_DUEL_RootTask;
	static constexpr game::Id ORIG_MoveTask = game::MAG_274_DUEL_MoveTask;
	static constexpr game::Id ORIG_SparkTask = game::MAG_274_DUEL_SparkTask;
	static constexpr game::Id ORIG_FlashTask = game::MAG_274_DUEL_HitFlashTask;
	static constexpr game::Id ORIG_RockTask = game::MAG_274_DUEL_RockTask;

	static constexpr game::Id COM_Anim = game::MAG_274_DUEL_AnimSet;
	static constexpr game::Id SEQ_Flash = game::unk_10E7868;
	static constexpr game::Id MODEL_Wave = game::unk_10E7E18;
	static constexpr game::Id MODEL_Rock = game::unk_10E9360;
	static constexpr game::Id MODEL_Kick = game::unk_10E9530;
	static constexpr game::Id LAYOUT_Punch = game::unk_10EA030, LAYOUT_PunchB = game::unk_10EC7D4, LAYOUT_Booya = game::unk_10ED700;
	static constexpr game::Id LAYOUT_Kick = game::unk_10F10F8, LAYOUT_Heel = game::unk_10F3808, LAYOUT_HeelB = game::unk_10F6BE0;
	static constexpr game::Id LAYOUT_Meteor = game::unk_10F73B0, LAYOUT_MeteorB = game::unk_10F89F4, LAYOUT_Dolphin = game::unk_10FE310, LAYOUT_DolphinB = game::unk_110198C;
	static constexpr game::Id CAMERA_Data = game::unk_110BE70;
	static const int16_t *BEND_Table() { return (const int16_t *)game_addr(game::unk_110D3B0); }
	static const uint8_t *SPARK_Table() { return (const uint8_t *)game_addr(game::unk_110E3B8); }
	static const uint8_t *LENGTH_Table() { return (const uint8_t *)game_addr(game::MAG274_DUEL_moveDurations); }
	static const int16_t *THROW_Table() { return (const int16_t *)game_addr(game::unk_110E3F0); }
	static const void *SOUND_Punch() { return (const void *)game_addr(game::unk_110BE58); }
	static const void *SOUND_Booya() { return (const void *)game_addr(game::unk_110BE5C); }
	static const void *SOUND_Heel() { return (const void *)game_addr(game::unk_110BE60); }
	static const void *SOUND_Kick() { return (const void *)game_addr(game::unk_110BE64); }
	static const void *SOUND_Dolphin() { return (const void *)game_addr(game::unk_110BE68); }
	static const void *SOUND_Meteor() { return (const void *)game_addr(game::unk_110BE6C); }

	inline int32_t Rand() { return fn<int32_t (__cdecl *)()>(game_addr(game::Effect_SharedLcg_Rand))(); }
	inline int32_t ComputeCos(int32_t a) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(a); }
	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t flag, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, flag, out); }
	inline uint32_t BindExternalAnimSet(uint8_t *entity, uint32_t out, uint32_t data) { return fn<uint32_t (__cdecl *)(uint8_t *, uint32_t, uint32_t)>(game_addr(game::BattleEntity_BindExternalAnimSet))(entity, out, data); }
	inline uint32_t QueueChainTransformation(uint8_t *entity, uint32_t id) { return fn<uint32_t (__cdecl *)(uint8_t *, uint32_t)>(game_addr(game::QueueChainTransformation))(entity, id); }
	inline void CameraAnimation(uint32_t data, uint32_t id) { fn<void (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::BS_GetCameraAnimationPointer))(data, id); }
	inline uint8_t *EntityChainLowest(uint8_t *e) { return fn<uint8_t *(__cdecl *)(uint8_t *)>(game_addr(game::Battle_EntityChainLowest))(e); }
	inline void MeasureModelHeight(uint8_t *e, int16_t *out) { fn<void (__cdecl *)(uint8_t *, int16_t *)>(game_addr(game::MAG_070_ShockwavePulsar_MeasureModelHeight))(e, out); }
	inline void StreamQueueLoad(int32_t id, uint32_t dst, int32_t kind) { fn<void (__cdecl *)(int32_t, uint32_t, int32_t)>(game_addr(game::Stream_QueueLoad))(id, dst, kind); }
	inline void StreamPump() { fn<void (__cdecl *)()>(game_addr(game::Stream_Pump))(); }
	inline int32_t StreamIsBusy() { return fn<int32_t (__cdecl *)()>(game_addr(game::Stream_IsBusy))(); }
	inline void DecodeModelPrimLayout(uint32_t data, uint32_t layout, int32_t size) { fn<void (__cdecl *)(uint32_t, uint32_t, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline void RotationY(int32_t a, Mat4x3 *out) { fn<void (__cdecl *)(int32_t, Mat4x3 *)>(game_addr(game::Effect_RotationYMatrix))(a, out); }
	inline void RotationZ(int32_t a, Mat4x3 *out) { fn<void (__cdecl *)(int32_t, Mat4x3 *)>(game_addr(game::MAG_065_RotationZMatrix))(a, out); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void RotationFromAnglesB(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::MAG_063_RotationFromAnglesYXZ))(angles, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
	inline void MultiplyPSX(const void *a, const void *b, Mat4x3 *out) { fn<void (__cdecl *)(const void *, const void *, Mat4x3 *)>(game_addr(game::Matrix3x3_MultiplyPSX))(a, b, out); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void MatrixMultiply3x3(Mat4x3 *a, const Mat4x3 *b) { fn<void (__cdecl *)(Mat4x3 *, const Mat4x3 *)>(game_addr(game::MatrixMultiply3x3))(a, b); }
	inline void Apply3DTransform(const Mat4x3 *m, const int16_t *in, int32_t *out) { fn<void (__cdecl *)(const Mat4x3 *, const int16_t *, int32_t *)>(game_addr(game::apply3DTransform))(m, in, out); }
	inline void BuildOrthonormalBasis(const int32_t *dir, Mat4x3 *out) { fn<void (__cdecl *)(const int32_t *, Mat4x3 *)>(game_addr(game::BuildOrthonormalBasis))(dir, out); }
	inline void CopyDwords(uint32_t dst, uint32_t src, int32_t n) { fn<void (__cdecl *)(uint32_t, uint32_t, int32_t)>(game_addr(game::MAG_104_CopyDwords))(dst, src, n); }
	inline uint32_t RenderPrimModel070(void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::MAG_070_RenderPrimModel))(h, ot, mode, cursor); }
	inline uint32_t DrawShadow(uint8_t *e, uint32_t ot, int32_t a, uint32_t cursor) { return fn<uint32_t (__cdecl *)(uint8_t *, uint32_t, int32_t, uint32_t)>(game_addr(game::BattleEntity_DrawShadow))(e, ot, a, cursor); }
	inline uint32_t RenderGeometry(uint32_t model, void *ctx, uint32_t ot, int32_t a, uint32_t cursor) { return fn<uint32_t (__cdecl *)(uint32_t, void *, uint32_t, int32_t, uint32_t)>(game_addr(game::RenderGeometry))(model, ctx, ot, a, cursor); }
	inline void ScreenFadeTask(int32_t a, int32_t b, int32_t c, int32_t d) { fn<void (__cdecl *)(int32_t, int32_t, int32_t, int32_t)>(game_addr(game::Effect_SpawnScreenFadeTask))(a, b, c, d); }
	inline void GteMVMVA_RotV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0))(); }
	inline void GteMVMVA_RotIR() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotIR))(); }
	inline void GteLoadIRFromColumn(const void *p) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadIRFromMatrixColumn))(p); }
	inline void GteStoreIRToColumn(void *p) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreIRToMatrixColumn))(p); }
	inline void GteLoadV0FromDwords(const void *p) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadV0FromDwords))(p); }
	inline void GteLoadIR123FromBytes(const void *p) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadIR123FromBytes))(p); }
	inline void GteStoreIR123LowBytes(void *p) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreIR123LowBytes))(p); }
	inline void GteSetSXY012(uint32_t a, uint32_t b, uint32_t c) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_SetSXY012))(a, b, c); }
	inline void GteNCLIP() { fn<void (__cdecl *)()>(game_addr(game::GTE_NCLIP))(); }
	inline void GteReadMAC0(int32_t *p) { fn<void (__cdecl *)(int32_t *)>(game_addr(game::GTE_ReadMAC0))(p); }
	inline void GteSetSZ123(uint32_t a, uint32_t b, uint32_t c) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_SetSZ123))(a, b, c); }
	inline void GteSetSZ0123(uint32_t a, uint32_t b, uint32_t c, uint32_t d) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_SetSZ0123))(a, b, c, d); }
	inline void GteAVSZ3() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ3))(); }
	inline void GteAVSZ4() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ4))(); }
	inline void GteReadOTZ32(int32_t *p) { fn<void (__cdecl *)(int32_t *)>(game_addr(game::GTE_ReadOTZ))(p); }
	inline void GteReadOTZdiv4(void *p) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadOTZ_SZ3div4))(p); }
	inline void GteLoadV012(const void *a, const void *b, const void *c) { fn<void (__cdecl *)(const void *, const void *, const void *)>(game_addr(game::GTE_LoadV012))(a, b, c); }
	inline void GteRTPT() { fn<void (__cdecl *)()>(game_addr(game::GTE_RTPT))(); }
	inline void GteReadFLAG(uint32_t *p) { fn<void (__cdecl *)(uint32_t *)>(game_addr(game::GTE_ReadFLAG))(p); }
	inline void GteReadSXY012(void *a, void *b, void *c) { fn<void (__cdecl *)(void *, void *, void *)>(game_addr(game::GTE_ReadSXY012_Split))(a, b, c); }
	inline void GteLoadRGBC(const void *p) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadRGBC))(p); }
	inline void GteDPCS() { fn<void (__cdecl *)()>(game_addr(game::GTE_DPCS))(); }
	inline void GteStoreRGB2(void *p) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreRGB2))(p); }
	inline void GteLoadV0u(const void *v) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadV0_u16))(v); }
	inline void GteStoreIR123s(void *p) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreIR123))(p); }

#pragma pack(push, 1)
	struct RootNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t action;
		uint8_t started;
		uint8_t new_move;
		uint8_t mode;
		uint16_t pad12;
		uint32_t arena;
	};
	struct MoveNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t action;
		uint8_t mode;
		uint8_t first;
		uint8_t length;
		uint8_t wait;
		uint8_t level;
		uint8_t *target;
		uint32_t caster_pos[2];
		uint32_t caster_rot[2];
		int16_t anchor[4];
		uint32_t target_pos[2];
		uint8_t bind[0x10];
		uint32_t layout_a;
		uint32_t layout_b;
		uint32_t ghosts;
		int16_t p54[4];
		int16_t height[2];
		uint8_t w60[0x14];
	};
	struct PlayArg
	{
		Mat4x3 m;
		int32_t depth;
		int16_t mode;
		int16_t scroll;
		uint32_t morph;
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
	static_assert(sizeof(RootNode) == 0x18 && sizeof(MoveNode) == 0x74, "Duel nodes");
	static_assert(sizeof(PlayArg) == 0x2C && sizeof(GhostCtx) == 0x54, "Duel blocks");
	#pragma pack(push, 1)
	struct SparkNode
	{
		TaskNode hdr;
		int16_t pos[3];
		int16_t frame;
		int16_t vel[3];
		int16_t seq;
		int16_t acc[3];
		int16_t pad22;
	};
	struct FlashNode
	{
		TaskNode hdr;
		int16_t pos[3];
		int16_t frame;
	};
	struct RockNode
	{
		TaskNode hdr;
		int16_t pos[3];
		int16_t life;
		int16_t vel[3];
		int16_t pad1a;
		int16_t ang[3];
		int16_t pad22;
		int16_t spin[3];
		int16_t pad2a;
	};
#pragma pack(pop)
	static_assert(sizeof(SparkNode) == 0x24 && sizeof(FlashNode) == 0x14 && sizeof(RockNode) == 0x2C, "Duel particle nodes");
}
}

namespace ff8fx
{
namespace duel274
{
	using namespace eng;
	using namespace magc;

	inline int16_t &W16(void *p, int off) { return *(int16_t *)((uint8_t *)p + off); }
	inline uint16_t &U16(void *p, int off) { return *(uint16_t *)((uint8_t *)p + off); }
	inline int32_t &W32(void *p, int off) { return *(int32_t *)((uint8_t *)p + off); }
	inline uint32_t &U32(void *p, int off) { return *(uint32_t *)((uint8_t *)p + off); }
	inline uint8_t &B8(void *p, int off) { return *((uint8_t *)p + off); }

	inline int SlotOf(const uint8_t *e) { return (int)((e - (const uint8_t *)game_addr(game::BattleEntitySlotData)) / 0x9C); }
	inline uint8_t *ActionTargets(int i) { return Ctx()->actions[i].targets; }

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
		uint8_t *dst = g + 0x88 + (uint32_t)s * 0x200;
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
								GteReadOTZ32(&c->otz);
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
									GteReadOTZ32(&c->otz);
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
					const Mat4x3 *m = (const Mat4x3 *)(c->slot + (uint32_t)(k + 8) * 0x20);
					GteSetRotMatrixCtrl(m);
					GteSetTransVectorCtrl(m);
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
			FramePackets() = GhostPolys(c, ot, shift, FramePackets());
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
			c->slot = (uint32_t)g + 0x88 + (((uint32_t)g[3] - (uint32_t)i) & 1) * 0x200;
			for (int32_t b = 0; b < g[4]; b++)
			{
				const uint32_t r = c->slot + (uint32_t)b * 0x20;
				GteSetRotMatrixCtrl((const Mat4x3 *)h);
				GteLoadIRFromColumn((const void *)(r + 0));
				GteMVMVA_RotIR();
				GteStoreIRToColumn((void *)(r + 0x100));
				GteLoadIRFromColumn((const void *)(r + 2));
				GteMVMVA_RotIR();
				GteStoreIRToColumn((void *)(r + 0x102));
				GteLoadIRFromColumn((const void *)(r + 4));
				GteMVMVA_RotIR();
				GteStoreIRToColumn((void *)(r + 0x104));
				GteSetTransVectorCtrl((const Mat4x3 *)h);
				GteLoadV0FromDwords((const void *)(r + 0x14));
				GteMVMVA_RotV0Tr();
				GteReadMAC123((int32_t *)(r + 0x114));
			}
			GhostModel(*(uint8_t **)(entity + 0x64), c, OTBase() + 0x44, 0);
			w -= 0x400;
		}
		FieldFree(0x74);
		if (g[0] == 0 && g[1] != 0) g[1]--;
		g[0] = 0;
	}

	static uint32_t RenderTris(uint8_t *h, uint32_t ot, int32_t shift, uint32_t pk)
	{
		uint8_t *list = *(uint8_t **)(h + 0x20);
		int32_t count = *(const int32_t *)list;
		uint8_t *p = list + 4;
		const uint32_t va = U32(h, 4);
		*(uint8_t **)(h + 0x20) = p;
		if (count <= 0)
		{
			*(uint8_t **)(h + 0x20) = p;
			return pk;
		}
		for (; count != 0; count--, p += 0x14)
		{
			GteLoadV012((const void *)(va + U16(p, 4) * 4), (const void *)(va + U16(p, 6) * 4), (const void *)(va + U16(p, 8) * 4));
			GteRTPT();
			U32((void *)pk, 0) = 0x6000000;
			U32((void *)pk, 4) = U32(p, 0);
			GteReadFLAG((uint32_t *)(h + 0x30));
			if (U32(h, 0x30) & 0x60000) continue;
			GteNCLIP();
			uint32_t clip = 0;
			GteReadMAC0((int32_t *)(h + 0x24));
			if (W32(h, 0x24) < 0 && !(B8(h, 0x1C) & 0x20)) continue;
			GteReadSXY012((void *)(pk + 8), (void *)(pk + 0x10), (void *)(pk + 0x18));
			GteAVSZ3();
			int16_t x = W16((void *)pk, 8);
			if (x < 0 || x > 0xA00) clip = 1;
			x = W16((void *)pk, 0x10);
			if (x < 0 || x > 0xA00) clip |= 2;
			x = W16((void *)pk, 0x18);
			if (x < 0 || x > 0xA00) clip |= 4;
			x = W16((void *)pk, 0xA);
			if (x < 0 || x > 0x6C0) clip |= 0x10;
			x = W16((void *)pk, 0x12);
			if (x < 0 || x > 0x6C0) clip |= 0x20;
			x = W16((void *)pk, 0x1A);
			if (x < 0 || x > 0x6C0) clip |= 0x40;
			if ((clip & 7) == 7 || (clip & 0x70) == 0x70) continue;
			GteReadOTZ32((int32_t *)(h + 0x2C));
			U32((void *)pk, 0xC) = U32(p, 0xC);
			U32((void *)pk, 0x14) = U32(p, 0x10);
			const int32_t z = W32(h, 0x2C) + W32(h, 0xC);
			W32(h, 0x2C) = z;
			if (z < 0) continue;
			const uint32_t slot = ot + (uint32_t)(z >> shift) * 4;
			const uint32_t tri = pk;
			pk += 0x1C;
			U32((void *)pk, 0) = 0x1000000;
			U32((void *)pk, 4) = 0xE1000220;
			InsertPrimAutoDepth(slot, (void *)pk);
			pk += 8;
			InsertPrimAutoDepth(slot, (void *)tri);
			U32((void *)pk, 0) = 0x1000000;
			U32((void *)pk, 4) = 0xE1000240;
			InsertPrimAutoDepth(slot, (void *)pk);
			pk += 8;
		}
		*(uint8_t **)(h + 0x20) = p;
		return pk;
	}

	static uint32_t RenderQuads(uint8_t *h, uint32_t ot, int32_t shift, uint32_t pk)
	{
		uint8_t *list = *(uint8_t **)(h + 0x20);
		int32_t count = *(const int32_t *)list;
		uint8_t *p = list + 4;
		const uint32_t va = U32(h, 4);
		*(uint8_t **)(h + 0x20) = p;
		if (count <= 0)
		{
			*(uint8_t **)(h + 0x20) = p;
			return pk;
		}
		for (; count != 0; count--, p += 0x18)
		{
			GteLoadV012((const void *)(va + U16(p, 4) * 4), (const void *)(va + U16(p, 6) * 4), (const void *)(va + U16(p, 8) * 4));
			GteRTPT();
			U32((void *)pk, 0) = 0x8000000;
			U32((void *)pk, 4) = U32(p, 0);
			GteReadFLAG((uint32_t *)(h + 0x30));
			if (U32(h, 0x30) & 0x60000) continue;
			GteNCLIP();
			uint32_t clip = 0;
			GteReadMAC0((int32_t *)(h + 0x24));
			if (W32(h, 0x24) < 0 && !(B8(h, 0x1C) & 0x20)) continue;
			GteReadSXY012((void *)(pk + 8), (void *)(pk + 0x10), (void *)(pk + 0x18));
			GteLoadV0((const void *)(va + U16(p, 0xA) * 4));
			GteRTPS();
			int16_t x = W16((void *)pk, 8);
			if (x < 0 || x > 0xA00) clip = 1;
			x = W16((void *)pk, 0x10);
			if (x < 0 || x > 0xA00) clip |= 2;
			x = W16((void *)pk, 0x18);
			if (x < 0 || x > 0xA00) clip |= 4;
			x = W16((void *)pk, 0xA);
			if (x < 0 || x > 0x6C0) clip |= 0x10;
			x = W16((void *)pk, 0x12);
			if (x < 0 || x > 0x6C0) clip |= 0x20;
			x = W16((void *)pk, 0x1A);
			if (x < 0 || x > 0x6C0) clip |= 0x40;
			GteReadSXY2((void *)(pk + 0x20));
			GteAVSZ4();
			x = W16((void *)pk, 0x20);
			if (x < 0 || x > 0xA00) clip |= 8;
			x = W16((void *)pk, 0x22);
			if (x < 0 || x > 0x6C0) clip |= 0x80;
			if ((clip & 0xF) == 0xF || (clip & 0xF0) == 0xF0) continue;
			GteReadOTZ32((int32_t *)(h + 0x2C));
			U32((void *)pk, 0xC) = U32(p, 0xC);
			U32((void *)pk, 0x14) = U32(p, 0x10);
			U32((void *)pk, 0x1C) = U32(p, 0x14);
			const int32_t z = W32(h, 0x2C) + W32(h, 0xC);
			W32(h, 0x2C) = z;
			if (z < 0) continue;
			const uint32_t slot = ot + (uint32_t)(z >> shift) * 4;
			const uint32_t quad = pk;
			pk += 0x24;
			U32((void *)pk, 0) = 0x1000000;
			U32((void *)pk, 4) = 0xE1000220;
			InsertPrimAutoDepth(slot, (void *)pk);
			pk += 8;
			InsertPrimAutoDepth(slot, (void *)quad);
			U32((void *)pk, 0) = 0x1000000;
			U32((void *)pk, 4) = 0xE1000240;
			InsertPrimAutoDepth(slot, (void *)pk);
			pk += 8;
		}
		*(uint8_t **)(h + 0x20) = p;
		return pk;
	}

	static uint32_t RenderModelA(uint8_t *h, uint32_t ot, int32_t shift, uint32_t pk)
	{
		uint8_t *model = *(uint8_t **)h;
		uint8_t *list = model + *(const uint32_t *)model + 0x10;
		*(uint8_t **)(h + 0x20) = list;
		if (*(const uint32_t *)list != 0) pk = RenderTris(h, ot, shift, pk);
		else *(uint8_t **)(h + 0x20) = list + 4;
		uint8_t *q = *(uint8_t **)(h + 0x20);
		if (*(const uint32_t *)q != 0) return RenderQuads(h, ot, shift, pk);
		*(uint8_t **)(h + 0x20) = q + 4;
		return pk;
	}

	static uint32_t TexWindow(uint8_t a, uint8_t b, uint16_t w, uint16_t hh)
	{
		uint32_t d = ((uint32_t)a & 0xF8) | 0xFFFE2000u;
		d <<= 5;
		d |= (uint32_t)b & 0xF8;
		d <<= 5;
		d |= ~((uint32_t)w - 1) & 0xF8;
		d <<= 2;
		return (((uint32_t)(~((uint32_t)hh - 1)) >> 3) & 0x1F) | d;
	}

	static uint32_t RenderScroll(uint8_t *h, uint32_t ot, int32_t shift, uint32_t pk)
	{
		uint8_t *list = *(uint8_t **)(h + 0x24);
		int32_t count = *(const int32_t *)list;
		uint8_t *p = list + 4;
		const uint32_t va = U32(h, 4);
		*(uint8_t **)(h + 0x24) = p;
		if (count <= 0)
		{
			*(uint8_t **)(h + 0x24) = p;
			return pk;
		}
		for (; count != 0; count--, p += 0x14)
		{
			GteLoadV012((const void *)(va + U16(p, 4) * 4), (const void *)(va + U16(p, 6) * 4), (const void *)(va + U16(p, 8) * 4));
			GteRTPT();
			U32((void *)pk, 0) = 0x7000000;
			U32((void *)pk, 4) = U32(p, 0);
			U32((void *)pk, 0xC) = U32(p, 0xC);
			U32((void *)pk, 0x14) = U32(p, 0x10);
			U32((void *)pk, 0x1C) = U32(p, 8) >> 16;
			GteReadFLAG((uint32_t *)(h + 0x34));
			if (U32(h, 0x34) & 0x60000) continue;
			GteNCLIP();
			uint32_t clip = 0;
			GteReadMAC0((int32_t *)(h + 0x28));
			if (W32(h, 0x28) < 0) continue;
			GteReadSXY012((void *)(pk + 8), (void *)(pk + 0x10), (void *)(pk + 0x18));
			GteAVSZ3();
			int16_t x = W16((void *)pk, 8);
			if (x < 0 || x > 0xA00) clip = 1;
			x = W16((void *)pk, 0x10);
			if (x < 0 || x > 0xA00) clip |= 2;
			x = W16((void *)pk, 0x18);
			if (x < 0 || x > 0xA00) clip |= 4;
			x = W16((void *)pk, 0xA);
			if (x < 0 || x > 0x6C0) clip |= 0x10;
			x = W16((void *)pk, 0x12);
			if (x < 0 || x > 0x6C0) clip |= 0x20;
			x = W16((void *)pk, 0x1A);
			if (x < 0 || x > 0x6C0) clip |= 0x40;
			if ((clip & 7) == 7 || (clip & 0x70) == 0x70) continue;
			GteReadOTZ32((int32_t *)(h + 0x30));
			if (W32(h, 0xC) != 0)
			{
				GteLoadRGBC((const void *)(pk + 4));
				GteSetIR0(W32(h, 0xC));
				GteDPCS();
				GteStoreRGB2((void *)(pk + 4));
				B8((void *)pk, 7) = 0x26;
			}
			const uint32_t off = U16(h, 0x12);
			uint32_t v2 = (uint32_t)B8((void *)pk, 0x1D) + off;
			const uint32_t v0 = (uint32_t)B8((void *)pk, 0xD) + off;
			const uint32_t v1 = (uint32_t)B8((void *)pk, 0x15) + off;
			if ((int32_t)(v2 | v1 | v0) > 0xFF)
			{
				B8((void *)pk, 0xD) = (uint8_t)((uint8_t)v0 - B8(h, 0x22));
				B8((void *)pk, 0x15) = (uint8_t)((uint8_t)v1 - B8(h, 0x22));
				v2 = (uint8_t)((uint8_t)v2 - B8(h, 0x22));
			}
			else
			{
				B8((void *)pk, 0xD) = (uint8_t)v0;
				B8((void *)pk, 0x15) = (uint8_t)v1;
			}
			B8((void *)pk, 0x1D) = (uint8_t)v2;
			const uint32_t slot = ot + (uint32_t)(W32(h, 0x30) >> shift) * 4;
			const uint32_t tri = pk;
			pk += 0x20;
			U32((void *)pk, 0) = 0x2000000;
			U32((void *)pk, 4) = TexWindow(B8(h, 0x16), B8(h, 0x14), U16(h, 0x1A), U16(h, 0x18));
			U32((void *)pk, 8) = 0;
			InsertPrimAutoDepth(slot, (void *)pk);
			InsertPrimAutoDepth(slot, (void *)tri);
			pk += 0xC;
			U32((void *)pk, 0) = 0x2000000;
			U32((void *)pk, 4) = TexWindow(B8(h, 0x1E), B8(h, 0x1C), U16(h, 0x22), U16(h, 0x20));
			U32((void *)pk, 8) = 0;
			InsertPrimAutoDepth(slot, (void *)pk);
			pk += 0xC;
			if (W32(h, 0xC) == 0) continue;
			U32((void *)pk, 0) = 0x4000000;
			B8((void *)pk, 7) = 0x22;
			U32((void *)pk, 8) = U32((void *)tri, 8);
			U32((void *)pk, 0xC) = U32((void *)tri, 0x10);
			U32((void *)pk, 0x10) = U32((void *)tri, 0x18);
			const uint8_t grey = (uint8_t)(0x40 - (uint8_t)(W32(h, 0xC) >> 6));
			B8((void *)pk, 6) = grey;
			B8((void *)pk, 5) = grey;
			B8((void *)pk, 4) = grey;
			InsertPrimAutoDepth(slot, (void *)pk);
			pk += 0x14;
			U32((void *)pk, 0) = 0x1000000;
			U32((void *)pk, 4) = 0xE1000240;
			InsertPrimAutoDepth(slot, (void *)pk);
			pk += 8;
		}
		*(uint8_t **)(h + 0x24) = p;
		return pk;
	}

	static uint32_t RenderModelB(uint8_t *h, uint32_t ot, int32_t shift, uint32_t pk)
	{
		uint8_t *model = *(uint8_t **)h;
		*(uint8_t **)(h + 0x24) = model + *(const uint32_t *)model + 8;
		return RenderScroll(h, ot, shift, pk);
	}

	static int16_t g_off_pad = 0;
	static void __cdecl PartCallback(prim::Layout *l, prim::Record *r, int arg_)
	{
		const PlayArg *arg = (const PlayArg *)arg_;
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
		Mat4x3 m;
		if (r->flags & 0x40000) RotationFromAnglesB(r->rot, &m);
		else RotationFromAngles(r->rot, &m);
		const int16_t off[4] = { r->pos[0], r->pos[1], r->pos[2], g_off_pad };
		if (r->flags & 0x1000)
		{
			GteSetRotMatrixCtrl(CamCopy());
			GteLoadV0(off);
			GteMVMVA_RotV0();
			int32_t t[3];
			GteReadMAC123(t);
			m.t[0] = t[0];
			m.t[1] = t[1];
			m.t[2] = t[2];
			if (!(r->flags & 0x8000)) MatrixMultiply(CamCopy(), &m);
		}
		else if (r->flags & 0x200)
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
		*(uint32_t *)(h + 0x1C) = 0x2030;
		if (r->flags & 0x2000) *(uint32_t *)(h + 0x1C) = 0x203C;
		*(int32_t *)(h + 0xC) = r->a;
		if (r->a != 0)
		{
			B8(h, 0x1C) |= 0xC0;
			*(uint32_t *)(h + 8) = *(const uint32_t *)r->rgb;
		}
		uint32_t pk;
		if (arg->mode == 1)
		{
			pk = RenderModelA(h, OTBase() + 0x44, 2, PacketCursor());
		}
		else if (arg->mode == 2 && r->flags_lo == 0)
		{
			U16(h, 0x16) = 0;
			U16(h, 0x14) = 0;
			U16(h, 0x1A) = 0x100;
			U16(h, 0x18) = 0x100;
			U16(h, 0x1E) = 0;
			U16(h, 0x1C) = 0;
			U16(h, 0x20) = 0x100;
			U16(h, 0x22) = 0x80;
			W16(h, 0x12) = arg->scroll;
			W32(h, 0xC) = 0;
			pk = RenderModelB(h, OTBase() + 0x44, 2, PacketCursor());
		}
		else
		{
			*(int32_t *)(h + 0x18) = arg->depth;
			pk = RenderPrimModel070(h, OTBase() + 0x44, 2, PacketCursor());
		}
		PacketCursor() = pk;
		FieldFree(0x58);
	}

	static void SparkDraw(SparkNode *s)
	{
		const uint8_t *e = SPARK_Table() + s->seq * 8;
		const int16_t scale = *(const int16_t *)(e + 6);
		const uint32_t a = ((uint32_t)(int32_t)s->seq & 0xFFFF0000u) | (uint16_t)scale;
		TransformCameraByShadowRotation(s->pos, (int32_t)a, -((int32_t)scale >> 2));
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		*(uint32_t *)h = *(const uint32_t *)e;
		*(int16_t *)(h + 4) = s->frame;
		*(int16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, OTBase() + 0x44, 2, PacketCursor());
		FieldFree(0xB4);
	}
	static uint32_t __cdecl SparkTask(TaskNode *n)
	{
		SparkNode *s = (SparkNode *)n;
		SparkDraw(s);
		s->frame++;
		if (s->frame >= *(const int16_t *)(SPARK_Table() + s->seq * 8 + 4)) return TASK_END;
		s->pos[0] = (int16_t)(s->pos[0] + s->vel[0]);
		s->pos[1] = (int16_t)(s->pos[1] + s->vel[1]);
		s->pos[2] = (int16_t)(s->pos[2] + s->vel[2]);
		s->vel[0] = (int16_t)(s->acc[0] + s->vel[0]);
		s->vel[1] = (int16_t)(s->acc[1] + s->vel[1]);
		s->vel[2] = (int16_t)(s->acc[2] + s->vel[2]);
		return 0;
	}

	static void SparkBurst(int32_t bone, int32_t n, int32_t kind, int32_t seq)
	{
		int16_t pos[4];
		GetEffectSpawnPosition(Caster(), bone, 0, pos);
		pos[3] = 0;
		SparkNode *s = (SparkNode *)AddTaskToQueue(QSparks(), game_addr(ORIG_SparkTask));
		while (s)
		{
			ff8fx::copy<(8)>(s->pos, pos);
			bool decel = true;
			switch (kind)
			{
			case 0:
				s->vel[0] = (int16_t)((Rand() & 0xFF) - 0x80);
				s->vel[1] = (int16_t)((Rand() & 0xFF) - 0x80);
				s->vel[2] = (int16_t)(-0x80 - (Rand() & 0x7F));
				break;
			case 1:
				s->vel[0] = (int16_t)((Rand() & 0xFF) - 0x80);
				s->vel[1] = (int16_t)(-0x80 - (Rand() & 0x7F));
				s->vel[2] = (int16_t)((Rand() & 0xFF) - 0x80);
				break;
			case 2:
			{
				const int32_t a = Rand();
				const int32_t r = (Rand() & 0xFF) + 0x100;
				s->vel[0] = (int16_t)(mul32(ComputeSin(a), r) >> 12);
				s->vel[1] = (int16_t)(mul32(ComputeCos(a), r) >> 12);
				s->vel[2] = (int16_t)((Rand() & 0x3F) - 0x20);
				break;
			}
			case 3:
			{
				const int32_t a = Rand();
				const int32_t r = (Rand() & 0x7F) + 0x80;
				s->vel[0] = (int16_t)(mul32(ComputeSin(a), r) >> 12);
				s->vel[2] = (int16_t)(mul32(ComputeCos(a), r) >> 12);
				const int32_t v = -0x80 - (Rand() & 0x7F);
				s->acc[2] = 0;
				s->vel[1] = (int16_t)v;
				s->acc[0] = 0;
				s->acc[1] = 0x14;
				decel = false;
				break;
			}
			case 4:
			{
				const int32_t a = Rand();
				const int32_t r = Rand() & 0x3FF;
				s->pos[0] = (int16_t)(s->pos[0] + (int16_t)(mul32(ComputeSin(a), r) >> 11));
				s->pos[2] = (int16_t)(s->pos[2] + (int16_t)(mul32(ComputeCos(a), r) >> 12));
				const int32_t up = Rand() & 0x7FF;
				s->vel[1] = 0;
				s->pos[1] = (int16_t)(s->pos[1] - up);
				s->vel[2] = 0;
				s->vel[0] = 0;
				s->acc[1] = -0x14;
				s->acc[2] = 0;
				s->acc[0] = 0;
				decel = false;
				break;
			}
			default:
				break;
			}
			if (decel)
			{
				s->acc[0] = (int16_t)(-(int32_t)s->vel[0] >> 3);
				s->acc[1] = (int16_t)(-(int32_t)s->vel[1] >> 3);
				s->acc[2] = (int16_t)(-(int32_t)s->vel[2] >> 3);
			}
			s->seq = (int16_t)seq;
			if (--n == 0) break;
			s = (SparkNode *)AddTaskToQueue(QSparks(), game_addr(ORIG_SparkTask));
		}
	}

	static void SparkRing(uint8_t *entity, int32_t bone)
	{
		int16_t pos[4];
		GetEffectSpawnPosition(entity, bone, 0, pos);
		int32_t n = 2;
		SparkNode *s = (SparkNode *)AddTaskToQueue(QSparks(), game_addr(ORIG_SparkTask));
		while (s)
		{
			const int32_t r = ((int32_t)*(const int16_t *)(entity + 0x26) + 0x1000) >> 4;
			const int32_t a = Rand();
			const int32_t b = Rand();
			const int32_t c = mul32(ComputeCos(a), r) >> 12;
			s->pos[1] = (int16_t)(pos[1] - (int16_t)(mul32(ComputeSin(a), r) >> 12));
			s->pos[0] = (int16_t)(pos[0] - (int16_t)(mul32(ComputeSin(b), c) >> 12));
			s->frame = 0;
			s->pos[2] = (int16_t)((mul32(ComputeCos(b), c) >> 12) + pos[2]);
			s->seq = 2;
			s->acc[2] = 0;
			s->acc[1] = 0;
			s->acc[0] = 0;
			s->vel[2] = 0;
			s->vel[1] = 0;
			s->vel[0] = 0;
			if (--n == 0) break;
			s = (SparkNode *)AddTaskToQueue(QSparks(), game_addr(ORIG_SparkTask));
		}
	}

	static void FlashDraw(FlashNode *f)
	{
		TransformCameraByShadowRotation(f->pos, 0x800, -0x200);
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		*(uint32_t *)h = game_addr(SEQ_Flash);
		*(int16_t *)(h + 4) = f->frame;
		*(int16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, OTBase() + 0x44, 2, PacketCursor());
		FieldFree(0xB4);
	}
	static uint32_t __cdecl FlashTask(TaskNode *n)
	{
		FlashNode *f = (FlashNode *)n;
		FlashDraw(f);
		f->frame++;
		return f->frame < 8 ? 0 : TASK_END;
	}

	static void SpawnFlash(int32_t bone, int32_t far_)
	{
		int16_t pos[4];
		GetEffectSpawnPosition(Caster(), bone, 0, pos);
		pos[3] = 0;
		FlashNode *f = (FlashNode *)AddTaskToQueue(QFlashes(), game_addr(ORIG_FlashTask));
		if (!f) return;
		ff8fx::copy<(8)>(f->pos, pos);
		if (far_ == 0) f->pos[2] = (int16_t)(f->pos[2] - 0x80);
	}

	static void RockDraw(RockNode *k)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		Mat4x3 m;
		BuildRotationMatrixFromAngles(k->ang, &m);
		m.t[0] = k->pos[0];
		m.t[1] = k->pos[1];
		m.t[2] = k->pos[2];
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		*(uint32_t *)h = game_addr(MODEL_Rock);
		*(uint32_t *)(h + 0x1C) = 0;
		PacketCursor() = RenderPrimModel(h, OTBase() + 0x44, 2, PacketCursor());
		FieldFree(0x58);
	}
	static uint32_t __cdecl RockTask(TaskNode *n)
	{
		RockNode *k = (RockNode *)n;
		RockDraw(k);
		k->ang[0] = (int16_t)(k->ang[0] + k->spin[0]);
		k->ang[2] = (int16_t)(k->ang[2] + k->spin[2]);
		k->vel[1] = (int16_t)(k->vel[1] + 0xF);
		k->ang[1] = (int16_t)(k->ang[1] + k->spin[1]);
		k->pos[0] = (int16_t)(k->pos[0] + k->vel[0]);
		k->pos[1] = (int16_t)(k->pos[1] + k->vel[1]);
		k->pos[2] = (int16_t)(k->pos[2] + k->vel[2]);
		if (k->pos[1] > 0)
		{
			k->pos[1] = 0;
			k->vel[1] = (int16_t)-k->vel[1];
		}
		k->life++;
		return k->life < 0x14 ? 0 : TASK_END;
	}

	static void PlaceBefore(int32_t angle, int32_t dist, const int16_t *from, int16_t *out)
	{
		out[0] = (int16_t)(from[0] - (int16_t)(mul32(ComputeSin(angle), dist) >> 12));
		out[2] = (int16_t)(from[2] - (int16_t)(mul32(ComputeCos(angle), dist) >> 12));
	}

	static const Mat4x3 *BoneMatrix(uint8_t *entity, int32_t bone)
	{
		return (const Mat4x3 *)(**(uint8_t ***)(entity + 0x64) + bone * 0x30 + 0x20);
	}

	static void BendWave(uint32_t model, const uint8_t *src, int32_t grow, int32_t roll)
	{
		Mat4x3 m;
		RotationZ(roll, &m);
		GteSetRotMatrix(&m);
		int32_t count = *(const int32_t *)(model + 4);
		int16_t *v = (int16_t *)(model + 8);
		do
		{
			GteLoadV0(src);
			GteMVMVA_RotV0();
			src += 8;
			GteStoreIR123s(v);
			const int32_t z = v[2];
			const int32_t bend = mul32(BEND_Table()[z >> 1], grow) >> 13;
			v[0] = (int16_t)(v[0] - (int16_t)bend);
			v[2] = (int16_t)(mul32(z, grow) >> 10);
			v += 4;
		} while (--count != 0);
	}

	static void ViewAt(Mat4x3 *out)
	{
		TransformVectorBy3x3Matrix(&Camera(), out->t, out->t);
		out->t[0] = (int32_t)((uint32_t)out->t[0] + (uint32_t)Camera().t[0]);
		out->t[1] = (int32_t)((uint32_t)out->t[1] + (uint32_t)Camera().t[1]);
		out->t[2] = (int32_t)((uint32_t)out->t[2] + (uint32_t)Camera().t[2]);
		ff8fx::copy<(0x12)>(out, &Camera());
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

	static void DrawBars(int32_t level)
	{
		const int32_t s = mul32(ComputeSin(shl32(level, 4)) * 5, 16) >> 12;
		const uint32_t slot = OTBase() + 0x20;
		uint8_t *pk = (uint8_t *)PacketCursor();
		PacketCursor() += 8;
		U32(pk, 0) = 0x1000000;
		U32(pk, 4) = 0xE1000220;
		InsertPrimAutoDepth(slot, pk);
		pk = (uint8_t *)PacketCursor();
		PacketCursor() += 0x24;
		W16(pk, 0xA) = (int16_t)(s * 8 - 0x280);
		W16(pk, 0x12) = (int16_t)(s * 8 - 0x280);
		U32(pk, 4) = 0x404040;
		W16(pk, 8) = 0;
		W16(pk, 0x10) = 0xA00;
		W16(pk, 0x18) = 0;
		W16(pk, 0x1A) = (int16_t)(s * 8);
		W16(pk, 0x20) = 0xA00;
		W16(pk, 0x22) = (int16_t)(s * 8);
		U32(pk, 0xC) = 0x404040;
		U32(pk, 0x1C) = 0;
		U32(pk, 0x14) = 0;
		U32(pk, 0) = 0x8000000;
		B8(pk, 7) = 0x3A;
		InsertPrimAutoDepth(slot, pk);
		pk = (uint8_t *)PacketCursor();
		PacketCursor() += 0x24;
		W16(pk, 0xA) = (int16_t)((0xD8 - s) * 8);
		W16(pk, 0x12) = (int16_t)((0xD8 - s) * 8);
		U32(pk, 4) = 0;
		W16(pk, 8) = 0;
		W16(pk, 0x10) = 0xA00;
		W16(pk, 0x18) = 0;
		W16(pk, 0x1A) = (int16_t)((0x128 - s) * 8);
		W16(pk, 0x20) = 0xA00;
		W16(pk, 0x22) = (int16_t)((0x128 - s) * 8);
		U32(pk, 0xC) = 0;
		U32(pk, 0x1C) = 0x404040;
		U32(pk, 0x14) = 0x404040;
		U32(pk, 0) = 0x8000000;
		B8(pk, 7) = 0x3A;
		InsertPrimAutoDepth(slot, pk);
		pk = (uint8_t *)PacketCursor();
		PacketCursor() += 8;
		U32(pk, 0) = 0x1000000;
		U32(pk, 4) = 0xE1000240;
		InsertPrimAutoDepth(slot, pk);
	}

	static void PauseEntities()
	{
		for (uint8_t *e = (uint8_t *)game_addr(game::BattleEntitySlotData); e < (uint8_t *)game_addr(game::CAMERA_FLAG_RELATED); e += 0x9C)
			if (e[0] & 2) U16(e, 0) |= 8;
	}
	static void ResumeEntities()
	{
		for (uint8_t *e = (uint8_t *)game_addr(game::BattleEntitySlotData); e < (uint8_t *)game_addr(game::CAMERA_FLAG_RELATED); e += 0x9C)
			if (e[0] & 2) U16(e, 0) &= 0xFFF7;
	}

	static void RestoreChainFlags(uint8_t *target, uint16_t mask)
	{
		uint8_t *e = target;
		do
		{
			const uint16_t f = U16(e, 0);
			if (f & 2) U16(e, 0) = (uint16_t)(f ^ ((f ^ (uint16_t)SavedFlags()[SlotOf(e)]) & mask));
			e = *(uint8_t **)(e + 0x8C);
		} while (e != target && e != nullptr);
	}

	enum : int { SITE_PUNCH_B = 1, SITE_PUNCH_A, SITE_BOOYA, SITE_HEEL_A, SITE_HEEL_B, SITE_KICK, SITE_DOLPHIN_A, SITE_DOLPHIN_B, SITE_METEOR_A, SITE_METEOR_B };
	static void BoneFrame(int site, Mat4x3 *f, int16_t *pos)
	{
		uint8_t *c = Caster();
		if (site == SITE_METEOR_A)
		{
			GetEffectSpawnPosition(c, 0x13, 0, pos);
			f->t[0] = pos[0];
			f->t[1] = 0;
			f->t[2] = pos[2];
			return;
		}
		GetEffectSpawnPosition(c, 0x16, 0, pos);
		if (site == SITE_PUNCH_B)
		{
			MultiplyPSX((const void *)game_addr(game::unk_1D97300), BoneMatrix(c, 0x12), f);
			f->t[0] = pos[0];
			f->t[1] = pos[1];
			f->t[2] = pos[2];
		}
		else
		{
			RotationY(-0x400, f);
			f->t[0] = pos[0];
			f->t[1] = pos[1];
			f->t[2] = pos[2] - 0xC8;
		}
	}
	static int PlayLayout(MoveNode *m, int site, uint32_t layout, PlayArg *arg)
	{
		(void)m;
		if (site == SITE_METEOR_A) ViewAt(&arg->m);
		else ComposeAffineTransform(&Camera(), &arg->m, &arg->m);
		return prim::play((prim::Layout *)layout, PartCallback, (int)arg, 0);
	}
	static int16_t g_pad_booya = 0, g_pad_meteor = 0;
	static void Damage(const MoveNode *m)
	{
		ApplyActionResultToTarget(ActionTargets(m->action));
	}
	static void PlaceCaster(MoveNode *m, int32_t dist)
	{
		uint8_t *c = Caster();
		PlaceBefore((int32_t)W16(c, 0xE) + 0x800, dist, m->anchor, (int16_t *)(c + 0x1C));
	}
	static int32_t Radius(uint8_t *e) { return W16(e, 0x26); }

	static void MovePunchRush(MoveNode *m)
	{
		uint8_t *c = Caster();
		if (m->counter == 0)
		{
			const int32_t r = Radius(c);
			PlaceCaster(m, (r >> 1) + (r + (Radius(m->target) >> 1)));
			InitTaskQueuePool(QSparks(), (void *)Alloc(), 0x24, 0x20);
			Alloc() += 0x480;
			InitTaskQueuePool(QFlashes(), (void *)Alloc(), 0x14, 2);
			Alloc() += 0x28;
			m->layout_a = Alloc();
			Alloc() += 0x14C;
			m->layout_b = Alloc();
			Alloc() += 0x64;
			m->ghosts = Alloc();
			Alloc() += 0x488;
			DecodeModelPrimLayout(game_addr(LAYOUT_Punch), m->layout_a, 0x14C);
			DecodeModelPrimLayout(game_addr(LAYOUT_PunchB), m->layout_b, 0x64);
			static const int32_t bones[6] = { 0xD, 0x11, 0x15, 0xE, 0x12, 0x16 };
			GhostInit((uint8_t *)m->ghosts, 6, bones);
		}
		W16(m->target, 0xE) = (int16_t)(W16(c, 0xE) + 0x800);
		ExecuteTaskQueue(QSparks());
		ExecuteTaskQueue(QFlashes());
		PlayArg arg;
		if ((uint32_t)(m->counter - 0x1B) < 0xF)
		{
			int16_t pos[4];
			BoneFrame(SITE_PUNCH_B, &arg.m, pos);
			arg.morph = TexBase() + 0x74;
			arg.depth = -0x80;
			arg.mode = 0;
			PlayLayout(m, SITE_PUNCH_B, m->layout_b, &arg);
		}
		if ((uint32_t)(m->counter - 0x21) < 0xE)
		{
			int16_t pos[4];
			BoneFrame(SITE_PUNCH_A, &arg.m, pos);
			arg.morph = TexBase() + 0x74;
			arg.depth = -0x100;
			arg.mode = 0;
			PlayLayout(m, SITE_PUNCH_A, m->layout_a, &arg);
		}
		GhostDraw(c, (uint8_t *)m->ghosts);
		if (m->counter <= 0x1A) GhostRecord(c, (uint8_t *)m->ghosts);
		switch (m->counter)
		{
		case 8:
			SparkBurst(0x15, 0x10, 0, 0);
			SpawnFlash(0x15, 0);
			W16(m->target, 0x20) -= 0x32;
			break;
		case 9:
		case 0xE:
		case 0x17:
			W16(m->target, 0x20) += 0x32;
			break;
		case 0xD:
			SparkBurst(0x16, 0x10, 0, 0);
			SpawnFlash(0x16, 0);
			W16(m->target, 0x20) -= 0x32;
			break;
		case 0x16:
			SparkBurst(0x15, 0x10, 1, 0);
			SpawnFlash(0x15, 1);
			W16(m->target, 0x20) -= 0x32;
			break;
		case 0x20:
		case 0x21:
			if (m->counter == 0x20) Damage(m);
			if (m->counter == 0x21)
			{
				ScreenFadeTask(0, 1, 1, 0x80);
				SparkBurst(0x16, 0x20, 0, 0);
			}
			W16(m->target, 0x20) -= 0xC8;
			break;
		default:
			break;
		}
		if (m->counter == 8) BdPlaySE(SOUND_Punch(), 0, 0x80);
	}

	static void MoveBooya(MoveNode *m)
	{
		uint8_t *c = Caster();
		if (m->counter == 0)
		{
			PlaceCaster(m, (Radius(m->target) >> 1) + (Radius(c) >> 1) + 0x80);
			InitTaskQueuePool(QSparks(), (void *)Alloc(), 0x24, 0x40);
			Alloc() += 0x900;
			m->layout_a = Alloc();
			Alloc() += 0x5C8;
			m->ghosts = Alloc();
			Alloc() += 0x488;
			DecodeModelPrimLayout(game_addr(LAYOUT_Booya), m->layout_a, 0x5C8);
			static const int32_t bones[1] = { 9 };
			GhostInit((uint8_t *)m->ghosts, 1, bones);
		}
		GhostDraw(c, (uint8_t *)m->ghosts);
		ExecuteTaskQueue(QSparks());
		W16(m->target, 0xE) = (int16_t)(W16(c, 0xE) + 0x800);
		if (m->counter >= 0x1F && m->counter < 0x2D)
		{
			if (m->counter == 0x1F) GetEffectSpawnPosition(c, 9, 0, m->p54);
			PlayArg arg;
			RotationY(-0x400, &arg.m);
			arg.m.t[0] = m->p54[0];
			arg.m.t[1] = m->p54[1];
			arg.m.t[2] = m->p54[2];
			arg.morph = TexBase() + 0x74;
			arg.depth = 0;
			arg.mode = 0;
			PlayLayout(m, SITE_BOOYA, m->layout_a, &arg);
		}
		int16_t *fist = (int16_t *)m->w60;
		if (m->counter == 9) GetEffectSpawnPosition(c, 0x16, 0, fist);
		if (m->counter >= 0xA && m->counter < 0x20)
		{
			int16_t pos[4];
			GetEffectSpawnPosition(c, 0x16, 0, pos);
			pos[3] = g_pad_booya;
			W16(m->target, 0x1C) += (int16_t)(pos[0] - fist[0]);
			W16(m->target, 0x1E) += (int16_t)(pos[1] - fist[1]);
			W16(m->target, 0x20) += (int16_t)(pos[2] - fist[2]);
			ff8fx::copy<(8)>(fist, pos);
		}
		if (m->counter >= 0x19 && m->counter <= 0x20) GhostRecord(c, (uint8_t *)m->ghosts);
		if (m->counter == 0x1F) SparkBurst(0x16, 0x20, 2, 1);
		if (m->counter == 0x20)
		{
			U32(m->target, 0x1C) = m->target_pos[0];
			U32(m->target, 0x20) = m->target_pos[1];
			Damage(m);
		}
		if (m->counter == 0x21) ScreenFadeTask(0, 1, 1, 0x80);
		if (m->counter >= 0x23 && m->counter <= 0x26)
		{
			SparkRing(c, 9);
			SparkRing(m->target, 0xF0);
		}
		if (m->counter == 0xD) BdPlaySE(SOUND_Booya(), 0, 0x80);
	}

	static void MoveHeelDrop(MoveNode *m)
	{
		uint8_t *c = Caster();
		EffectCameraMatrix(&Camera(), CamCopy());
		if (m->counter == 0)
		{
			PlaceCaster(m, (Radius(m->target) >> 1) + Radius(c) + 0x100);
			m->layout_a = Alloc();
			Alloc() += 0x360;
			m->layout_b = Alloc();
			Alloc() += 0x64;
			m->ghosts = Alloc();
			Alloc() += 0x488;
			DecodeModelPrimLayout(game_addr(LAYOUT_Heel), m->layout_a, 0x360);
			DecodeModelPrimLayout(game_addr(LAYOUT_HeelB), m->layout_b, 0x64);
			static const int32_t bones[3] = { 0xC, 0x10, 0x14 };
			GhostInit((uint8_t *)m->ghosts, 3, bones);
		}
		GhostDraw(c, (uint8_t *)m->ghosts);
		if (m->counter >= 0x20 && m->counter < 0x2E)
		{
			if (m->counter == 0x20) GetEffectSpawnPosition(c, 0x14, 0, m->p54);
			PlayArg arg;
			RotationY(0, &arg.m);
			arg.m.t[0] = m->p54[0];
			arg.m.t[1] = m->p54[1];
			arg.m.t[2] = m->p54[2];
			arg.morph = TexBase() + 0x74;
			arg.depth = -0x100;
			arg.mode = 0;
			PlayLayout(m, SITE_HEEL_A, m->layout_a, &arg);
			RotationY(0, &arg.m);
			arg.m.t[0] = m->p54[0];
			arg.m.t[1] = 0;
			arg.m.t[2] = m->p54[2];
			PlayLayout(m, SITE_HEEL_B, m->layout_b, &arg);
		}
		if (m->counter >= 0x19 && m->counter <= 0x20) GhostRecord(c, (uint8_t *)m->ghosts);
		if (m->counter == 0x20) Damage(m);
		if (m->counter == 0x21) ScreenFadeTask(0, 1, 1, 0x80);
		if (m->counter == 0x1E) BdPlaySE(SOUND_Heel(), 0, 0x80);
	}

	static void MoveMachKick(MoveNode *m)
	{
		uint8_t *c = Caster();
		uint32_t &backup = U32(m->w60, 0);
		int16_t *from = (int16_t *)(m->w60 + 4);
		int16_t *to = (int16_t *)(m->w60 + 0xC);
		if (m->counter == 0)
		{
			const int32_t r = Radius(c);
			PlaceCaster(m, (r >> 1) + (r + (Radius(m->target) >> 1)));
			backup = Alloc();
			Alloc() += 0x268;
			m->layout_a = Alloc();
			Alloc() += 0x23C;
			m->ghosts = Alloc();
			Alloc() += 0x488;
			CopyDwords(backup, game_addr(MODEL_Kick) + 8, 0x9A);
			DecodeModelPrimLayout(game_addr(LAYOUT_Kick), m->layout_a, 0x23C);
			static const int32_t bones[3] = { 0xC, 0x10, 0x14 };
			GhostInit((uint8_t *)m->ghosts, 3, bones);
		}
		GhostDraw(c, (uint8_t *)m->ghosts);
		if (m->counter >= 0x20 && m->counter < 0x2E)
		{
			if (m->counter == 0x20) GetEffectSpawnPosition(c, 0x14, 0, m->p54);
			PlayArg arg;
			RotationY(0, &arg.m);
			arg.m.t[0] = m->p54[0];
			arg.m.t[1] = m->p54[1];
			arg.m.t[2] = m->p54[2];
			arg.morph = TexBase() + 0x74;
			arg.depth = -0x100;
			arg.mode = 0;
			PlayLayout(m, SITE_KICK, m->layout_a, &arg);
		}
		const uint32_t v = (uint32_t)(m->counter - 0x1D);
		if (v < 8)
		{
			if (v < 4) GetEffectSpawnPosition(c, 0x14, 0, to);
			int32_t d[3] = { to[0] - from[0], to[1] - from[1], from[2] - to[2] };
			Mat4x3 b;
			BuildOrthonormalBasis(d, &b);
			d[1] = 0x200;
			d[0] = 0x200;
			d[2] = 0x200;
			Scale3DMatrix(&b, d);
			b.t[0] = to[0];
			b.t[1] = to[1];
			b.t[2] = to[2];
			ComposeAffineTransform(&Camera(), &b, &b);
			GteSetRotMatrix(&b);
			GteSetTransVector(&b);
			uint8_t *h = (uint8_t *)FieldAlloc(0x58);
			*(uint32_t *)h = game_addr(MODEL_Kick);
			if (v >= 4)
			{
				*(uint32_t *)(h + 0x1C) = 0xF0;
				*(uint32_t *)(h + 8) = 0;
				*(uint32_t *)(h + 0xC) = (v - 4) << 10;
			}
			else
			{
				*(uint32_t *)(h + 0x1C) = 0x30;
			}
			PacketCursor() = RenderPrimModel(h, OTBase() + 0x44, 2, PacketCursor());
			FieldFree(0x58);
		}
		const uint32_t w = (uint32_t)(m->counter - 0x1C);
		if (w < 4)
		{
			if (m->counter == 0x1C) GetEffectSpawnPosition(c, 0x14, 0, from);
			BendWave(game_addr(MODEL_Kick), (const uint8_t *)backup, (int32_t)(w << 10) + 0x100, (int32_t)(w << 10));
		}
		if (m->counter == 0x25) CopyDwords(game_addr(MODEL_Kick) + 8, backup, 0x9A);
		if (m->counter >= 0x19 && m->counter <= 0x20) GhostRecord(c, (uint8_t *)m->ghosts);
		if (m->counter == 0x20)
		{
			Damage(m);
			if (m->counter == 0x20) ScreenFadeTask(0, 1, 1, 0x80);
		}
		if (m->counter == 0x1B) BdPlaySE(SOUND_Kick(), 0, 0x80);
	}

	static void MoveDolphinBlow(MoveNode *m)
	{
		uint8_t *c = Caster();
		EffectCameraMatrix(&Camera(), CamCopy());
		if (m->counter == 0)
		{
			const int32_t r = Radius(c);
			PlaceCaster(m, (r >> 1) + (r + (Radius(m->target) >> 1)));
			ff8fx::copy<(8)>(m->p54, c + 0x1C);
			m->layout_a = Alloc();
			Alloc() += 0xE0;
			m->layout_b = Alloc();
			Alloc() += 0x388;
			InitTaskQueuePool(QSparks(), (void *)Alloc(), 0x24, 0x40);
			Alloc() += 0x900;
			DecodeModelPrimLayout(game_addr(LAYOUT_Dolphin), m->layout_a, 0xE0);
			DecodeModelPrimLayout(game_addr(LAYOUT_DolphinB), m->layout_b, 0x388);
		}
		if (m->counter < 0x10)
		{
			PlayArg arg;
			RotationY(0x800, &arg.m);
			arg.m.t[0] = m->p54[0];
			arg.m.t[1] = m->p54[1];
			arg.m.t[2] = m->p54[2] - 0x1F4;
			arg.morph = TexBase() + 0x74;
			arg.depth = -0x100;
			arg.mode = 1;
			PlayLayout(m, SITE_DOLPHIN_A, m->layout_a, &arg);
		}
		const uint32_t v = (uint32_t)(m->counter - 0x14);
		if (v < 0x32)
		{
			PlayArg arg;
			RotationY(0x800, &arg.m);
			arg.m.t[0] = m->p54[0];
			arg.m.t[1] = m->p54[1];
			arg.m.t[2] = m->p54[2] - 0x3E8;
			arg.morph = TexBase() + 0x74;
			arg.depth = 0;
			arg.mode = 2;
			arg.scroll = (int16_t)((v & 0xF) << 3);
			PlayLayout(m, SITE_DOLPHIN_B, m->layout_b, &arg);
		}
		ExecuteTaskQueue(QSparks());
		if (m->counter == 0x1E) SparkBurst(0xF1, 0x10, 3, 3);
		if (m->counter >= 0x20 && m->counter < 0x28) SparkBurst(0xF1, 8, 4, 4);
		if (m->counter == 0x1F) ScreenFadeTask(0, 1, 0, 0x80);
		if (m->counter >= 0x20 && m->counter <= 0x21)
		{
			if (m->counter == 0x21) Damage(m);
			W16(m->target, 0x20) -= 0xC8;
		}
		if (m->counter == 0x18) BdPlaySE(SOUND_Dolphin(), 0, 0x80);
	}

	static void DrawPillar(Mat4x3 *w, const int16_t *pos, int16_t counter)
	{
		w->t[1] = 0;
		w->m[1][1] = (int16_t)(-2 * pos[1]);
		w->t[0] = pos[0];
		w->t[2] = pos[2];
		ComposeAffineTransform(&Camera(), w, w);
		GteSetRotMatrix(w);
		GteSetTransVector(w);
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		*(uint32_t *)h = game_addr(MODEL_Wave);
		if (counter < 0x2C)
		{
			*(uint32_t *)(h + 0x1C) = 0x30;
		}
		else
		{
			*(uint32_t *)(h + 8) = 0;
			*(uint32_t *)(h + 0x1C) = 0xF0;
			*(uint32_t *)(h + 0xC) = (uint32_t)(counter - 0x2C) << 9;
		}
		PacketCursor() = RenderPrimModel(h, OTBase() + 0x44, 2, PacketCursor());
		FieldFree(0x58);
	}

	static void MoveMeteorStrike(MoveNode *m)
	{
		uint8_t *c = Caster();
		uint8_t *t = m->target;
		int16_t *hip = (int16_t *)m->w60;
		int32_t &tilt = W32(m->w60, 8);
		if (m->counter == 0)
		{
			U32(t, 0x1C) = U32(m->anchor, 0);
			U32(t, 0x20) = U32(m->anchor, 4);
			PlaceCaster(m, Radius(t) >> 1);
			GetEffectSpawnPosition(c, 0x15, 0, hip);
			MeasureModelHeight(t, m->height);
			tilt = 0;
			m->layout_a = Alloc();
			Alloc() += 0x128;
			m->layout_b = Alloc();
			Alloc() += 0x9E4;
			InitTaskQueuePool(QRocks(), (void *)Alloc(), 0x2C, 0x20);
			Alloc() += 0x580;
			DecodeModelPrimLayout(game_addr(LAYOUT_Meteor), m->layout_a, 0x128);
			DecodeModelPrimLayout(game_addr(LAYOUT_MeteorB), m->layout_b, 0x9E4);
		}
		int16_t pos[4];
		int16_t ang[4];
		if (m->counter >= 0x2C && m->counter < 0x3E)
		{
			PlayArg arg;
			BoneFrame(SITE_METEOR_A, &arg.m, pos);
			arg.morph = TexBase() + 0x74;
			arg.depth = 0;
			arg.mode = 0;
			const int left = PlayLayout(m, SITE_METEOR_A, m->layout_a, &arg);
			if (m->counter < 0x35)
			{
				Mat4x3 w;
				const int32_t angle = (int32_t)shl32((int32_t)(((uint32_t)left & 0xFFFF0000u) | (uint16_t)m->counter), 7);
				RotationY(angle, &w);
				GetEffectSpawnPosition(c, 0x13, 0, pos);
				DrawPillar(&w, pos, m->counter);
				if (m->counter < 0x32)
				{
					int32_t n = 2;
					RockNode *k = (RockNode *)AddTaskToQueue(QRocks(), game_addr(ORIG_RockTask));
					while (k)
					{
						ff8fx::copy<(8)>(k->pos, pos);
						k->life = 0;
						k->pos[0] = (int16_t)(k->pos[0] + (Rand() & 0x1FF) - 0x100);
						k->pos[2] = (int16_t)(k->pos[2] + (Rand() & 0x1FF) - 0x100);
						k->pos[1] = (int16_t)(k->pos[1] + (Rand() & 0xFF));
						k->vel[0] = (int16_t)((Rand() & 0x7F) - 0x40);
						k->vel[2] = (int16_t)((Rand() & 0x7F) - 0x40);
						k->vel[1] = (int16_t)(Rand() & 0x1F);
						k->ang[0] = (int16_t)Rand();
						k->ang[1] = (int16_t)Rand();
						k->ang[2] = (int16_t)Rand();
						k->spin[0] = (int16_t)((Rand() & 0xFF) - 0x80);
						k->spin[1] = (int16_t)((Rand() & 0xFF) - 0x80);
						k->spin[2] = (int16_t)((Rand() & 0xFF) - 0x80);
						if (--n == 0) break;
						k = (RockNode *)AddTaskToQueue(QRocks(), game_addr(ORIG_RockTask));
					}
				}
			}
		}
		const uint32_t u = (uint32_t)(m->counter - 0x3C);
		if (u < 0x10)
		{
			PlayArg arg;
			const int16_t s = (int16_t)(((int32_t)W16(t, 0x26) + 0x1000) >> 1);
			arg.m.m[2][2] = s;
			arg.m.m[2][1] = 0;
			arg.m.m[1][1] = s;
			arg.m.m[0][0] = s;
			arg.m.m[2][0] = 0;
			arg.m.m[1][2] = 0;
			arg.m.m[1][0] = 0;
			arg.m.m[0][2] = 0;
			arg.m.m[0][1] = 0;
			arg.m.t[0] = W16(t, 0x1C);
			arg.m.t[1] = 0;
			arg.m.t[2] = W16(t, 0x20);
			arg.morph = TexBase() + 0x74;
			arg.depth = ((int32_t)W16(t, 0x26) - 0x800) >> 3;
			arg.mode = 0;
			PlayLayout(m, SITE_METEOR_B, m->layout_b, &arg);
			if (u < 4)
			{
				int32_t n = 4;
				RockNode *k = (RockNode *)AddTaskToQueue(QRocks(), game_addr(ORIG_RockTask));
				while (k)
				{
					U32(k->pos, 0) = U32(t, 0x1C);
					U32(k->pos, 4) = U32(t, 0x20);
					k->life = 0;
					k->pos[1] = 0;
					k->vel[0] = (int16_t)((Rand() & 0x1FF) - 0x100);
					k->vel[2] = (int16_t)((Rand() & 0x1FF) - 0x100);
					k->vel[1] = (int16_t)(-0x40 - (Rand() & 0x7F));
					k->ang[0] = (int16_t)Rand();
					k->ang[1] = (int16_t)Rand();
					k->ang[2] = (int16_t)Rand();
					k->spin[0] = (int16_t)((Rand() & 0xFF) - 0x80);
					k->spin[1] = (int16_t)((Rand() & 0xFF) - 0x80);
					k->spin[2] = (int16_t)((Rand() & 0xFF) - 0x80);
					if (--n == 0) break;
					k = (RockNode *)AddTaskToQueue(QRocks(), game_addr(ORIG_RockTask));
				}
			}
		}
		if ((uint16_t)m->counter <= 0x3B)
		{
			GetEffectSpawnPosition(c, 0x15, 0, ang);
			ang[3] = g_pad_meteor;
			const int16_t *path = THROW_Table() + m->counter * 4;
			int16_t x = (int16_t)(m->anchor[0] + path[0]);
			W16(t, 0x1C) = x;
			W32(t, 0x54) = x;
			x = (int16_t)((int16_t)(path[1] - m->height[0]) + m->anchor[1]);
			W16(t, 0x1E) = x;
			W32(t, 0x58) = x;
			x = (int16_t)(path[2] + m->anchor[2]);
			W16(t, 0x20) = x;
			W32(t, 0x5C) = x;
			ff8fx::copy<(8)>(hip, ang);
			B8(t, 1) |= 0x10;
			if (m->counter == 0x1C)
			{
				GetDefaultEffectPosition(t, pos);
				hip[0] = (int16_t)(hip[0] + (int16_t)(pos[0] - W16(c, 0x1C)));
				hip[2] = (int16_t)(hip[2] + (int16_t)(pos[2] - W16(c, 0x20)));
				W16(c, 0x1C) = pos[0];
				W16(c, 0x20) = pos[2];
			}
		}
		if (m->counter >= 0x3A && m->counter <= 0x43) W16(c, 0x20) += (int16_t)shl32(m->counter - 0x39, 7);
		if (m->counter == 0x3D) ScreenFadeTask(0, 1, 1, 0x80);
		if (m->counter == 0x46) Damage(m);
		const uint32_t k = (uint32_t)(m->counter - 0x37);
		if (k < 0x100)
		{
			pos[2] = 0;
			pos[0] = 0;
			if (k < 4)
			{
				pos[1] = (int16_t)((uint32_t)mul32((int32_t)m->height[1] - (int32_t)m->height[0], (int32_t)k) >> 3) + m->height[0];
				tilt -= 0x200;
				if (tilt < -0x800) tilt = -0x800;
			}
			else
			{
				MeasureModelHeight(t, m->height);
				pos[1] = (int16_t)(((int32_t)m->height[1] + (int32_t)m->height[0]) >> 1);
			}
			int32_t scale[3];
			scale[2] = 0x1000;
			scale[0] = 0x1000;
			if (k >= 4 && k < 0xC)
			{
				const int32_t s = 0x1000 - (ComputeSin((int32_t)((k - 4) << 8)) >> 1);
				scale[1] = s;
				pos[1] = (int16_t)(mul32(pos[1], s) >> 12);
			}
			else
			{
				scale[1] = 0x1000;
			}
			ang[2] = 0;
			ang[0] = (int16_t)tilt;
			ang[1] = 0;
			pos[2] = 0;
			pos[0] = 0;
			uint8_t *e = t;
			do
			{
				if (e[0] & 2)
				{
					DrawEntity(e, ang, pos, scale);
					U16(e, 0) |= 0x1004;
				}
				e = *(uint8_t **)(e + 0x8C);
			} while (e != t && e != nullptr);
		}
		ExecuteTaskQueue(QRocks());
		if (m->counter == 0x2B) BdPlaySE(SOUND_Meteor(), 0, 0x80);
	}

	static uint32_t __cdecl MoveTask(TaskNode *n)
	{
		MoveNode *m = (MoveNode *)n;
		g_pad_meteor = 0;
		m->mode = PostedMode();
		RestoreChainFlags(m->target, 4);
		if (m->mode == 0)
		{
			if (m->counter == 0) Ctx()->done = 1;
			m->counter = 1;
			return 0;
		}
		if (m->wait != 0)
		{
			bool bars = true;
			if (m->wait < 4)
			{
				bars = false;
			}
			else if (NewMove())
			{
				m->level = (uint8_t)(m->level - 0x10);
				if (m->level > 0x80)
				{
					ResumeEntities();
					SetScreenFlash(0, 0);
					m->level = 0;
					m->wait = 0;
					goto start;
				}
			}
			else
			{
				PauseEntities();
				m->level = (uint8_t)(m->level + 0x10);
				if (m->level > 0x40) m->level = 0x40;
			}
			if (bars)
			{
				SetScreenFlash(0x800, 0);
				DrawBars(m->level);
			}
			m->wait++;
			if (m->wait > 4) m->wait = 4;
			return 0;
		}
	start:
		if (NewMove())
		{
			Alloc() = TexBase() + 0x934;
			if (m->first)
			{
				uint8_t *c = Caster();
				m->caster_pos[0] = U32(c, 0x1C);
				m->caster_pos[1] = U32(c, 0x20);
				m->caster_rot[0] = U32(c, 0xC);
				m->caster_rot[1] = U32(c, 0x10);
				m->target_pos[0] = U32(m->target, 0x1C);
				m->target_pos[1] = U32(m->target, 0x20);
				GetDefaultEffectPosition(m->target, m->anchor);
				if (m->target[0] & 2) g_pad_meteor = 0x01D9;
				if (m->target[4] == 0x8C)
				{
					m->anchor[2] = (int16_t)(m->anchor[2] - 0xC80);
				}
				else if (m->target[4] == 0x54)
				{
					SavedPartyY()[0] = W16(Entity(0), 0x20);
					SavedPartyY()[1] = W16(Entity(1), 0x20);
					SavedPartyY()[2] = W16(Entity(2), 0x20);
					W16(Entity(0), 0x20) = (int16_t)(SavedPartyY()[0] + 1000);
					W16(Entity(1), 0x20) = (int16_t)(SavedPartyY()[1] + 1000);
					W16(Entity(2), 0x20) = (int16_t)(SavedPartyY()[2] + 1000);
					m->anchor[2] = (int16_t)(m->anchor[2] - 0x7D0);
				}
				m->anchor[1] = W16(m->target_pos, 2);
				BindExternalAnimSet(c, (uint32_t)m->bind, game_addr(COM_Anim));
				StreamStateInit((void *)Alloc());
				Alloc() += 0x18;
				StreamQueueLoad(0x2CB, Alloc(), 1);
				StreamQueueLoad(0x2CC, Alloc(), 0x10);
				Alloc() += 0x8800;
			}
			U32((void *)TexBase(), 0x92C) = Alloc();
			const uint32_t arena = m->mode == 6 ? 0x14000 : 0xE000;
			Alloc() += arena;
			U32((void *)TexBase(), 0x930) = Alloc();
			Alloc() += arena;
			W16(Caster(), 0xE) = 0;
			U32(m->target, 0x1C) = m->target_pos[0];
			U32(m->target, 0x20) = m->target_pos[1];
			RestoreChainFlags(m->target, 0x1004);
			QueueChainTransformation(Caster(), m->mode);
			CameraAnimation(game_addr(CAMERA_Data) + var<uint16_t>(game_addr(game::unk_110BE74)), (((uint32_t)m->mode - 2) << 4) + m->target[0x91]);
			m->length = LENGTH_Table()[m->mode];
			NewMove() = 0;
			m->counter = 0;
		}
		if (m->first) StreamPump();
		if (m->counter <= (int16_t)m->length)
		{
			switch (m->mode)
			{
			case 2: MovePunchRush(m); break;
			case 3: MoveBooya(m); break;
			case 4: MoveHeelDrop(m); break;
			case 5: MoveMachKick(m); break;
			case 6: MoveDolphinBlow(m); break;
			case 7: MoveMeteorStrike(m); break;
			case 8:
			{
				uint8_t *c = Caster();
				if (m->counter == 0)
				{
					U32(c, 0x1C) = m->caster_pos[0];
					U32(c, 0x20) = m->caster_pos[1];
					U32(c, 0xC) = m->caster_rot[0];
					U32(c, 0x10) = m->caster_rot[1];
					U32(m->target, 0x1C) = m->target_pos[0];
					U32(m->target, 0x20) = m->target_pos[1];
					RestoreChainFlags(m->target, 0x1004);
					if (m->target[4] == 0x54)
					{
						W16(Entity(0), 0x20) = SavedPartyY()[0];
						W16(Entity(1), 0x20) = SavedPartyY()[1];
						W16(Entity(2), 0x20) = SavedPartyY()[2];
					}
				}
				if (Ctx()->submode != 0)
				{
					Ctx()->done = 1;
					return TASK_END;
				}
				if (m->counter == 0)
				{
					Damage(m);
					m->w60[0] = 0;
					TextureRestoreTask((void *)U32((void *)TexBase(), 0x92C), &m->w60[0]);
				}
				if (m->w60[0] == 0 || m->counter < (int16_t)m->length)
				{
					m->counter++;
					return 0;
				}
				Ctx()->done = 1;
				return TASK_END;
			}
			default:
				break;
			}
		}
		m->counter++;
		if (m->counter == m->length)
		{
			if (m->first && StreamIsBusy())
			{
				m->counter--;
				return 0;
			}
			m->first = 0;
			m->wait = 1;
			m->level = 0;
			Ctx()->done = 1;
		}
		if (m->counter > (int16_t)m->length) m->counter = m->length;
		return 0;
	}

	static uint32_t __cdecl RootTask(TaskNode *n)
	{
		RootNode *r = (RootNode *)n;
		if (r->arena)
		{
			PacketCursor() = U32((void *)TexBase(), 0x92C);
			r->arena = 0;
		}
		else
		{
			PacketCursor() = U32((void *)TexBase(), 0x930);
			r->arena = 1;
		}
		if (r->counter == 1)
		{
			if (!r->started)
			{
				r->started = 1;
				InitTaskQueuePool(QMoves(), (void *)TexBase(), 0x74, 1);
			}
			MoveNode *m = (MoveNode *)AddTaskToQueue(QMoves(), game_addr(ORIG_MoveTask));
			if (m)
			{
				Memset32(&m->counter, 0, 0x1A);
				m->mode = 0;
				m->action = r->action;
				m->first = 1;
				m->target = Entity(ActionTargets(r->action)[0]);
				m->target = EntityChainLowest(m->target);
				MeasureModelHeight(m->target, m->height);
				r->action++;
			}
		}
		if (r->started && ExecuteTaskQueue(QMoves()) == 0) return TASK_END;
		r->counter++;
		if (r->counter >= 10) r->counter = 10;
		return 0;
	}
}

	void register_mag274_duel()
	{
		register_port(duel274::ORIG_RootTask, (void *)duel274::RootTask, "274 RootTask", 274);
		register_port(duel274::ORIG_MoveTask, (void *)duel274::MoveTask, "274 MoveTask", 274);
		register_port(duel274::ORIG_SparkTask, (void *)duel274::SparkTask, "274 SparkTask", 274);
		register_port(duel274::ORIG_FlashTask, (void *)duel274::FlashTask, "274 FlashTask", 274);
		register_port(duel274::ORIG_RockTask, (void *)duel274::RockTask, "274 RockTask", 274);
	}
}

