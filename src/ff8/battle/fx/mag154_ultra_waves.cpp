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

// Effect 154: Ultra Waves

#include "mag_common.h"

namespace ff8fx
{
namespace ultra154
{
	using namespace eng;
	using namespace magc;

	inline int32_t &TargetSlot() { return var<int32_t>(game_addr(game::g_UW154_TargetSlot)); }
	inline TaskQueue *QRoot() { return (TaskQueue *)game_addr(game::g_UW154_RootQueue); }
	inline TaskQueue *QEffects() { return (TaskQueue *)game_addr(game::g_UW154_EffectQueue); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::g_UW154_Ctx)); }
	inline int32_t &CasterSlot() { return var<int32_t>(game_addr(game::g_UW154_CasterSlot)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::g_UW154_PacketCursor)); }
	inline uint32_t &FrameArena() { return var<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)); }
	inline uint32_t OTBase() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)); }
	inline uint8_t DisplayBuffer() { return var<uint8_t>(game_addr(game::g_Battle_DisplayParity)); }

	static constexpr game::Id ARENA_EVEN = game::g_UW154_PacketArenaA, ARENA_ODD = game::g_UW154_PacketArenaB;
	static constexpr game::Id GRID = game::g_UW154_Grid;
	static constexpr game::Id GRID_END = game::g_UW154_DrawOffsetXY;
	static uint32_t GridBytes() { return game_addr(GRID_END) - game_addr(GRID); }
	static constexpr game::Id QUADS = game::g_UW154_GridQuads;
	static const uint32_t QUADS_BYTES = 27 * 40 * 0x60;
	static constexpr game::Id OFFSET_XY = game::g_UW154_DrawOffsetXY;
	static constexpr game::Id PKT_Capture = game::UW154_CapturePackets;
	static constexpr game::Id DRAWENV = game::g_Battle_DrawEnv;
	static constexpr game::Id STAGE_Anim0 = game::unk_1D989A4;
	static constexpr game::Id PARTS = game::g_UW154_Particles;
	static const int PART_COUNT = 100;
	static const int32_t WARP_TICKS = 0x23;

	static constexpr game::Id ORIG_RootTask = game::MAG_154_UltraWaves_RootTask;
	static constexpr game::Id ORIG_WaitTexRestore = game::MAG_154_UltraWaves_WaitTexRestoreTask;
	static constexpr game::Id ORIG_Controller = game::MAG_154_UltraWaves_ControllerTask;
	static constexpr game::Id ORIG_Emitter = game::MAG_154_UltraWaves_EmitterTask;
	static constexpr game::Id ORIG_ScreenWarp = game::MAG_154_UltraWaves_ScreenWarpTask;
	static constexpr game::Id MODEL_Ring = game::UW154_RingModel;
	static const uint32_t *WarpColours() { return (const uint32_t *)game_addr(game::UW154_WarpColours); }
	static constexpr game::Id EMITTER_Vectors = game::UW154_EmitterVectors;
	static const uint32_t *RingColours() { return (const uint32_t *)game_addr(game::UW154_RingColours); }
	static const int32_t *StageDraw() { return (const int32_t *)game_addr(game::UW154_StageDrawTable); }
	static const void *SOUND_Waves() { return (const void *)game_addr(game::UW154_Sound); }
	static const void *CAMERA_Anim() { return (const void *)game_addr(game::MAG154_ULTRA_WAVES_cameraAnim); }

	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t flag, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, flag, out); }
	inline void MatVec(const void *m, const int16_t *in, int16_t *out) { fn<void (__cdecl *)(const void *, const int16_t *, int16_t *)>(game_addr(game::matrixMultiplyVector))(m, in, out); }
	inline void NormalizeSVector(const int16_t *in, int16_t *out) { fn<void (__cdecl *)(const int16_t *, int16_t *)>(game_addr(game::VectorNormalS16))(in, out); }
	inline void MulMatrixInPlace(Mat4x3 *m, const Mat4x3 *n) { fn<void (__cdecl *)(Mat4x3 *, const Mat4x3 *)>(game_addr(game::MatrixMultiply3x3))(m, n); }
	inline int32_t Cos(int32_t a) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(a); }
	inline void BuildDrawEnvPacket(uint32_t packet, uint32_t env) { fn<void (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::SSIGPU_SetDrawEnv))(packet, env); }
	inline void BuildDrawAreaPacket(uint32_t packet, const int16_t *rect) { fn<void (__cdecl *)(uint32_t, const int16_t *)>(game_addr(game::GPU_SetDrawAreaPrim))(packet, rect); }
	inline void BuildDrawOffsetPacket(uint32_t packet, uint32_t xy) { fn<void (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::SSIGPU_SetDrawOffsetPrim))(packet, xy); }
	inline void InsertPrimAltViewport(uint32_t bucket, uint32_t packet) { fn<void (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::SSIGPU_InsertPrimAltViewport))(bucket, packet); }
	inline void GetScreenOffset(int32_t *x, int32_t *y) { fn<void (__cdecl *)(int32_t *, int32_t *)>(game_addr(game::GTE_ReadScreenOffset))(x, y); }
	inline void SetScreenOffset(int32_t x, int32_t y) { fn<void (__cdecl *)(int32_t, int32_t)>(game_addr(game::Call_Bs_ParseCamera))(x, y); }
	inline uint32_t RenderGeometry(uint32_t group, uint32_t header, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t, int32_t, uint32_t)>(game_addr(game::RenderGeometry))(group, header, ot, mode, cursor); }
	inline void GteSetFarColour(uint32_t r, uint32_t g, uint32_t b) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_SetFarColor))(r, g, b); }
	inline void GteLoadV012(uint32_t a, uint32_t b, uint32_t c) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_LoadV012))(a, b, c); }
	inline void GteRTPT() { fn<void (__cdecl *)()>(game_addr(game::GTE_RTPT))(); }
	inline void GteReadFLAG(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadFLAG))(dst); }
	inline void GteNCLIP() { fn<void (__cdecl *)()>(game_addr(game::GTE_NCLIP))(); }
	inline void GteReadMAC0(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadMAC0))(dst); }
	inline void GteReadSXY012Split(void *a, void *b, void *c) { fn<void (__cdecl *)(void *, void *, void *)>(game_addr(game::GTE_ReadSXY012_Split))(a, b, c); }
	inline void GteAVSZ3() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ3))(); }
	inline void GteAVSZ4() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ4))(); }
	inline void GteStoreOTZ(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadOTZ))(dst); }
	inline void GteLoadRGBC(const void *c) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadRGBC))(c); }
	inline void GteDPCS() { fn<void (__cdecl *)()>(game_addr(game::GTE_DPCS))(); }
	inline void GteStoreRGB2(void *c) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreRGB2))(c); }
	inline void GteLoadRGB012(const void *a, const void *b, const void *c) { fn<void (__cdecl *)(const void *, const void *, const void *)>(game_addr(game::GTE_LoadRGB012))(a, b, c); }
	inline void GteDPCT() { fn<void (__cdecl *)()>(game_addr(game::GTE_DPCT))(); }
	inline void GteStoreRGB012(void *a, void *b, void *c) { fn<void (__cdecl *)(void *, void *, void *)>(game_addr(game::GTE_StoreRGB012))(a, b, c); }

#pragma pack(push, 1)
	struct RootNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t done;
	};
	struct ControllerNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		int16_t anchor[4];
		uint8_t pad18[0xC];
	};
	struct EmitterNode
	{
		TaskNode hdr;
		int16_t counter;
		uint16_t mask;
		int16_t pos[3];
		int16_t pad16;
		int16_t dir[3];
		int16_t pad1E;
		int16_t angle;
		int16_t colour;
	};
	struct WarpNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t pad0E[0x16];
	};
	struct Particle
	{
		uint16_t mask;
		int16_t colour;
		int16_t life;
		int16_t scale;
		int16_t pos[3];
		int16_t angle;
		int16_t vel[3];
		int16_t radius;
	};
	struct RenderHeader
	{
		uint32_t model;
		uint32_t verts;
		uint8_t far_rgb[4];
		int32_t fade;
		uint16_t u_off;
		uint16_t pad12;
		uint16_t v_off;
		uint16_t pad16;
		int32_t depth;
		uint32_t flags;
		uint32_t colour;
		uint32_t list;
		int32_t mac0;
		uint32_t pad2C;
		int32_t otz;
		uint32_t gte_flag;
		uint8_t pad38[0x20];
		uint32_t scale_rgb;
		uint32_t c[4];
	};
	struct EmitterWork
	{
		int16_t offs[4];
		Mat4x3 m;
		Mat4x3 spin;
		int32_t scale[4];
		int32_t from[4];
		int32_t to[4];
		int32_t axis[4];
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x10, "Ultra Waves root nodes");
	static_assert(sizeof(ControllerNode) == 0x24 && sizeof(EmitterNode) == 0x24 && sizeof(WarpNode) == 0x24, "Ultra Waves nodes");
	static_assert(sizeof(Particle) == 0x18 && sizeof(RenderHeader) == 0x6C && sizeof(EmitterWork) == 0x88, "Ultra Waves particle / render header / work");

	inline Particle *Parts() { return (Particle *)game_addr(PARTS); }
}
}

namespace ff8fx
{
namespace ultra154
{
	template<typename T> static inline T &At(uint32_t base, uint32_t off) { return *(T *)(base + off); }

	static inline bool OutX(uint32_t xy) { int16_t v = *(const int16_t *)xy; return v < 0 || v > 0xA00; }
	static inline bool OutY(uint32_t xy) { int16_t v = *(const int16_t *)(xy + 2); return v < 0 || v > 0x6C0; }

	static inline void UvOffsets(const RenderHeader *h, uint32_t pkt, uint32_t u_word, uint32_t v_word)
	{
		const uint32_t fl = h->flags;
		if (fl & 0x400) At<uint16_t>(pkt, u_word) = (uint16_t)(At<uint16_t>(pkt, u_word) + h->u_off);
		else if (fl & 0x100) At<uint16_t>(pkt, u_word) = h->u_off;
		if (fl & 0x800) At<uint16_t>(pkt, v_word) = (uint16_t)(At<uint16_t>(pkt, v_word) + h->v_off);
		else if (fl & 0x200) At<uint16_t>(pkt, v_word) = h->v_off;
	}

	static inline void ScaleColours(RenderHeader *h, int n)
	{
		h->scale_rgb = h->colour;
		uint8_t *s = (uint8_t *)&h->scale_rgb;
		for (int k = 0; k < n; k++)
		{
			uint8_t *c = (uint8_t *)&h->c[k];
			c[0] = (uint8_t)((uint32_t)c[0] * s[0] >> 7);
			c[1] = (uint8_t)((uint32_t)c[1] * s[1] >> 7);
			c[2] = (uint8_t)((uint32_t)c[2] * s[2] >> 7);
		}
	}

	static inline uint32_t Bucket(uint32_t ot, const RenderHeader *h, int32_t shift) { return ot + (uint32_t)(h->otz >> shift) * 4; }

	static uint32_t ListFT3(RenderHeader *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		const uint32_t list = h->list;
		int32_t count = At<int32_t>(list, 0);
		uint32_t rec = list + 4;
		h->list = rec;
		const uint32_t vb = h->verts;
		if (count <= 0)
		{
			h->list = rec;
			return cursor;
		}
		uint32_t pkt = cursor;
		do
		{
			GteLoadV012(vb + (uint32_t)At<uint16_t>(rec, 4) * 4, vb + (uint32_t)At<uint16_t>(rec, 6) * 4, vb + (uint32_t)At<uint16_t>(rec, 8) * 4);
			GteRTPT();
			uint32_t code = h->colour | 0x24000000;
			At<uint32_t>(pkt, 0) = 0x7000000;
			At<uint32_t>(pkt, 4) = code;
			if (h->flags & 1) At<uint32_t>(pkt, 4) = code | 0x2000000;
			if (h->flags & 4) At<uint32_t>(pkt, 4) &= 0xFDFFFFFF;
			const uint32_t d = (uint32_t)h->depth;
			At<uint32_t>(pkt, 0xC) = At<uint32_t>(rec, 0xC) + d;
			At<uint32_t>(pkt, 0x14) = At<uint32_t>(rec, 0x10) + d;
			At<uint32_t>(pkt, 0x1C) = (At<uint32_t>(rec, 8) >> 16) + d;
			GteReadFLAG(&h->gte_flag);
			if (!(h->gte_flag & 0x60000))
			{
				GteNCLIP();
				UvOffsets(h, pkt, 0x16, 0xE);
				GteReadMAC0(&h->mac0);
				const int32_t mac0 = h->mac0;
				if (mac0 != 0 && (mac0 > 0 || (h->flags & 0x10)))
				{
					GteReadSXY012Split((void *)(pkt + 8), (void *)(pkt + 0x10), (void *)(pkt + 0x18));
					GteAVSZ3();
					uint32_t clip = OutX(pkt + 8) ? 1 : 0;
					if (OutX(pkt + 0x10)) clip |= 2;
					if (OutX(pkt + 0x18)) clip |= 4;
					if (OutY(pkt + 8)) clip |= 0x10;
					if (OutY(pkt + 0x10)) clip |= 0x20;
					if (OutY(pkt + 0x18)) clip |= 0x40;
					if ((clip & 7) != 7 && (clip & 0x70) != 0x70)
					{
						GteStoreOTZ(&h->otz);
						if (h->flags & 0x40)
						{
							GteLoadRGBC((void *)(pkt + 4));
							GteSetIR0(h->fade);
							GteDPCS();
							GteStoreRGB2((void *)(pkt + 4));
						}
						InsertPrimAutoDepth(Bucket(ot, h, shift), (void *)pkt);
						pkt += 0x20;
					}
				}
			}
			rec += 0x14;
		} while (--count);
		h->list = rec;
		return pkt;
	}

	static uint32_t ListFT4(RenderHeader *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		const uint32_t list = h->list;
		int32_t count = At<int32_t>(list, 0);
		uint32_t rec = list + 4;
		h->list = rec;
		const uint32_t vb = h->verts;
		if (count <= 0)
		{
			h->list = rec;
			return cursor;
		}
		uint32_t pkt = cursor;
		do
		{
			GteLoadV012(vb + (uint32_t)At<uint16_t>(rec, 4) * 4, vb + (uint32_t)At<uint16_t>(rec, 6) * 4, vb + (uint32_t)At<uint16_t>(rec, 8) * 4);
			GteRTPT();
			uint32_t code = h->colour | 0x2C000000;
			At<uint32_t>(pkt, 0) = 0x9000000;
			At<uint32_t>(pkt, 4) = code;
			if (h->flags & 1) At<uint32_t>(pkt, 4) = code | 0x2000000;
			if (h->flags & 4) At<uint32_t>(pkt, 4) &= 0xFDFFFFFF;
			const uint32_t d = (uint32_t)h->depth;
			At<uint32_t>(pkt, 0xC) = At<uint32_t>(rec, 0xC) + d;
			const uint32_t uv23 = (d << 16) + d + At<uint32_t>(rec, 0x14);
			At<uint32_t>(pkt, 0x1C) = uv23;
			At<uint32_t>(pkt, 0x14) = At<uint32_t>(rec, 0x10) + d;
			At<uint32_t>(pkt, 0x24) = uv23 >> 16;
			GteReadFLAG(&h->gte_flag);
			if (!(h->gte_flag & 0x60000))
			{
				GteNCLIP();
				UvOffsets(h, pkt, 0x16, 0xE);
				GteReadMAC0(&h->mac0);
				const int32_t mac0 = h->mac0;
				if (mac0 != 0 && (mac0 > 0 || (h->flags & 0x10)))
				{
					GteReadSXY012Split((void *)(pkt + 8), (void *)(pkt + 0x10), (void *)(pkt + 0x18));
					GteLoadV0((const void *)(vb + (uint32_t)At<uint16_t>(rec, 0xA) * 4));
					GteRTPS();
					uint32_t clip = OutX(pkt + 8) ? 1 : 0;
					if (OutX(pkt + 0x10)) clip |= 2;
					if (OutX(pkt + 0x18)) clip |= 4;
					if (OutY(pkt + 8)) clip |= 0x10;
					if (OutY(pkt + 0x10)) clip |= 0x20;
					if (OutY(pkt + 0x18)) clip |= 0x40;
					GteReadSXY2((void *)(pkt + 0x20));
					GteAVSZ4();
					if (OutX(pkt + 0x20)) clip |= 8;
					if (OutY(pkt + 0x20)) clip |= 0x80;
					if ((clip & 0xF) != 0xF && (clip & 0xF0) != 0xF0)
					{
						GteStoreOTZ(&h->otz);
						if (h->flags & 0x40)
						{
							GteLoadRGBC((void *)(pkt + 4));
							GteSetIR0(h->fade);
							GteDPCS();
							GteStoreRGB2((void *)(pkt + 4));
						}
						InsertPrimAutoDepth(Bucket(ot, h, shift), (void *)pkt);
						pkt += 0x28;
					}
				}
			}
			rec += 0x18;
		} while (--count);
		h->list = rec;
		return pkt;
	}

	static uint32_t ListGT3(RenderHeader *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		const uint32_t list = h->list;
		int32_t count = At<int32_t>(list, 0);
		uint32_t rec = list + 4;
		h->list = rec;
		const uint32_t vb = h->verts;
		if (count <= 0)
		{
			h->list = rec;
			return cursor;
		}
		uint32_t pkt = cursor;
		do
		{
			GteLoadV012(vb + (uint32_t)At<uint16_t>(rec, 4) * 4, vb + (uint32_t)At<uint16_t>(rec, 6) * 4, vb + (uint32_t)At<uint16_t>(rec, 8) * 4);
			GteRTPT();
			const uint32_t code = At<uint32_t>(rec, 0);
			At<uint32_t>(pkt, 0) = 0x9000000;
			h->c[0] = code;
			if (h->flags & 2) h->c[0] = code | 0x2000000;
			if (h->flags & 8) h->c[0] &= 0xFDFFFFFF;
			const uint32_t d = (uint32_t)h->depth;
			At<uint32_t>(pkt, 0xC) = At<uint32_t>(rec, 0xC) + d;
			At<uint32_t>(pkt, 0x18) = At<uint32_t>(rec, 0x10) + d;
			At<uint32_t>(pkt, 0x24) = (At<uint32_t>(rec, 8) >> 16) + d;
			h->c[1] = At<uint32_t>(rec, 0x14);
			h->c[2] = At<uint32_t>(rec, 0x18);
			GteReadFLAG(&h->gte_flag);
			if (!(h->gte_flag & 0x60000))
			{
				GteNCLIP();
				UvOffsets(h, pkt, 0x1A, 0xE);
				GteReadMAC0(&h->mac0);
				const int32_t mac0 = h->mac0;
				if (mac0 != 0 && (mac0 > 0 || (h->flags & 0x20)))
				{
					GteReadSXY012Split((void *)(pkt + 8), (void *)(pkt + 0x14), (void *)(pkt + 0x20));
					GteAVSZ3();
					uint32_t clip = OutX(pkt + 8) ? 1 : 0;
					if (OutX(pkt + 0x14)) clip |= 2;
					if (OutX(pkt + 0x20)) clip |= 4;
					if (OutY(pkt + 8)) clip |= 0x10;
					if (OutY(pkt + 0x14)) clip |= 0x20;
					if (OutY(pkt + 0x20)) clip |= 0x40;
					if ((clip & 7) != 7 && (clip & 0x70) != 0x70)
					{
						GteStoreOTZ(&h->otz);
						ScaleColours(h, 3);
						if (h->flags & 0x80)
						{
							GteLoadRGB012(&h->c[1], &h->c[2], &h->c[0]);
							GteSetIR0(h->fade);
							GteDPCT();
							GteStoreRGB012((void *)(pkt + 0x10), (void *)(pkt + 0x1C), (void *)(pkt + 4));
						}
						else
						{
							At<uint32_t>(pkt, 4) = h->c[0];
							At<uint32_t>(pkt, 0x10) = h->c[1];
							At<uint32_t>(pkt, 0x1C) = h->c[2];
						}
						InsertPrimAutoDepth(Bucket(ot, h, shift), (void *)pkt);
						pkt += 0x28;
					}
				}
			}
			rec += 0x1C;
		} while (--count);
		h->list = rec;
		return pkt;
	}

	static uint32_t ListGT4(RenderHeader *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		const uint32_t list = h->list;
		int32_t count = At<int32_t>(list, 0);
		uint32_t rec = list + 4;
		h->list = rec;
		const uint32_t vb = h->verts;
		if (count <= 0)
		{
			h->list = rec;
			return cursor;
		}
		uint32_t pkt = cursor;
		do
		{
			GteLoadV012(vb + (uint32_t)At<uint16_t>(rec, 4) * 4, vb + (uint32_t)At<uint16_t>(rec, 6) * 4, vb + (uint32_t)At<uint16_t>(rec, 8) * 4);
			GteRTPT();
			const uint32_t code = At<uint32_t>(rec, 0);
			At<uint32_t>(pkt, 0) = 0xC000000;
			h->c[0] = code;
			if (h->flags & 2) h->c[0] = code | 0x2000000;
			if (h->flags & 8) h->c[0] &= 0xFDFFFFFF;
			const uint32_t d = (uint32_t)h->depth;
			At<uint32_t>(pkt, 0xC) = At<uint32_t>(rec, 0xC) + d;
			const uint32_t uv23 = (d << 16) + d + At<uint32_t>(rec, 0x14);
			At<uint32_t>(pkt, 0x18) = At<uint32_t>(rec, 0x10) + d;
			At<uint32_t>(pkt, 0x24) = uv23;
			At<uint32_t>(pkt, 0x30) = uv23 >> 16;
			h->c[1] = At<uint32_t>(rec, 0x18);
			h->c[2] = At<uint32_t>(rec, 0x1C);
			h->c[3] = At<uint32_t>(rec, 0x20);
			GteReadFLAG(&h->gte_flag);
			if (!(h->gte_flag & 0x60000))
			{
				GteNCLIP();
				UvOffsets(h, pkt, 0x1A, 0xE);
				GteReadMAC0(&h->mac0);
				const int32_t mac0 = h->mac0;
				if (mac0 != 0 && (mac0 > 0 || (h->flags & 0x20)))
				{
					GteReadSXY012Split((void *)(pkt + 8), (void *)(pkt + 0x14), (void *)(pkt + 0x20));
					GteLoadV0((const void *)(vb + (uint32_t)At<uint16_t>(rec, 0xA) * 4));
					GteRTPS();
					uint32_t clip = OutX(pkt + 8) ? 1 : 0;
					if (OutX(pkt + 0x14)) clip |= 2;
					if (OutX(pkt + 0x20)) clip |= 4;
					if (OutY(pkt + 8)) clip |= 0x10;
					if (OutY(pkt + 0x14)) clip |= 0x20;
					if (OutY(pkt + 0x20)) clip |= 0x40;
					GteReadSXY2((void *)(pkt + 0x2C));
					GteAVSZ4();
					if (OutX(pkt + 0x2C)) clip |= 8;
					if (OutY(pkt + 0x2C)) clip |= 0x80;
					if ((clip & 0xF) != 0xF && (clip & 0xF0) != 0xF0)
					{
						GteStoreOTZ(&h->otz);
						ScaleColours(h, 4);
						if (h->flags & 0x80)
						{
							GteLoadRGB012(&h->c[1], &h->c[2], &h->c[3]);
							GteSetIR0(h->fade);
							GteDPCT();
							GteStoreRGB012((void *)(pkt + 0x10), (void *)(pkt + 0x1C), (void *)(pkt + 0x28));
							GteLoadRGBC(&h->c[0]);
							GteDPCS();
							GteStoreRGB2((void *)(pkt + 4));
						}
						else
						{
							At<uint32_t>(pkt, 4) = h->c[0];
							At<uint32_t>(pkt, 0x10) = h->c[1];
							At<uint32_t>(pkt, 0x1C) = h->c[2];
							At<uint32_t>(pkt, 0x28) = h->c[3];
						}
						InsertPrimAutoDepth(Bucket(ot, h, shift), (void *)pkt);
						pkt += 0x34;
					}
				}
			}
			rec += 0x24;
		} while (--count);
		h->list = rec;
		return pkt;
	}

	static uint32_t RenderModel(RenderHeader *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		const uint32_t fl = h->flags;
		if (!(fl & 0x2000)) h->verts = h->model + 8;
		h->list = At<uint32_t>(h->model, 0) + h->model;
		if (!(fl & 0x1000)) h->depth = 0;
		GteSetFarColour(h->far_rgb[0], h->far_rgb[1], h->far_rgb[2]);
		h->list += 8;
		if (At<uint32_t>(h->list, 0) != 0) cursor = ListFT3(h, ot, shift, cursor);
		else h->list += 4;
		if (At<uint32_t>(h->list, 0) != 0) cursor = ListFT4(h, ot, shift, cursor);
		else h->list += 4;
		h->list += 8;
		if (At<uint32_t>(h->list, 0) != 0) cursor = ListGT3(h, ot, shift, cursor);
		else h->list += 4;
		if (At<uint32_t>(h->list, 0) != 0) cursor = ListGT4(h, ot, shift, cursor);
		else h->list += 4;
		return cursor;
	}

	static uint32_t __cdecl RootTask(TaskNode *n)
	{
		RootNode *r = (RootNode *)n;
		PacketCursor() = game_addr(ARENA_ODD);
		if (!(r->counter & 1)) PacketCursor() = game_addr(ARENA_EVEN);
		int alive = ExecuteTaskQueue(QEffects());
		if (alive == 0)
		{
			RootNode *w = (RootNode *)AddTaskToQueue(QRoot(), game_addr(ORIG_WaitTexRestore));
			w->counter = 0;
			w->done = 0;
		}
		r->counter++;
		return alive ? 0 : TASK_END;
	}

	static uint32_t __cdecl WaitTexRestore(TaskNode *n)
	{
		RootNode *w = (RootNode *)n;
		if (w->counter == 1) TextureRestoreTask((void *)game_addr(ARENA_EVEN), &w->done);
		int16_t done = w->done;
		w->counter++;
		return done ? TASK_END : 0;
	}

	static void GridInit()
	{
		uint32_t v = game_addr(GRID);
		for (int32_t row = 0; row < 0x1C; row++)
			for (int32_t col = 0; col < 0x29; col++, v += 0xC)
			{
				At<int16_t>(v, 4) = (int16_t)(-0xA4 + 8 * col);
				At<int16_t>(v, 6) = (int16_t)(-0x70 + 8 * row);
				At<int16_t>(v, 8) = 0;
				At<int16_t>(v, 0xA) = 0;
				if (row == 0 || row == 0x1B) At<int16_t>(v, 0xA) = 1;
				if (col == 0 || col == 0x28) At<int16_t>(v, 0xA) |= 2;
			}
		uint32_t vtx = game_addr(GRID);
		uint32_t rec = game_addr(QUADS);
		for (int32_t row = 0; row < 0x1B; row++)
		{
			const int32_t v0 = 8 * row;
			int32_t v1 = v0 + 8;
			if (v1 >= 0xD8) v1--;
			for (int32_t col = 0; col < 0x28; col++, rec += 0x60)
			{
				const int32_t x = 8 * col;
				At<uint32_t>(rec, 0) = vtx;
				At<uint32_t>(rec, 4) = vtx + 0xC;
				At<uint32_t>(rec, 8) = vtx + 0x1EC;
				At<uint32_t>(rec, 0xC) = vtx + 0x1F8;
				vtx += 0xC;
				const uint8_t u0 = (uint8_t)(x % 64);
				const uint16_t tpage = (uint16_t)((x / 64 + 0x16) | 0x120);
				const uint8_t u1 = (uint8_t)(x + 8 >= 0x140 ? u0 + 7 : u0 + 8);
				for (uint32_t p = rec + 0x10; p < rec + 0x60; p += 0x28)
				{
					At<uint32_t>(p, 0) = 0x9000000;
					At<uint32_t>(p, 4) = 0x2C808080;
					At<uint8_t>(p, 0xC) = u0;
					At<uint8_t>(p, 0xD) = (uint8_t)v0;
					At<uint8_t>(p, 0x14) = u1;
					At<uint8_t>(p, 0x15) = (uint8_t)v0;
					At<uint16_t>(p, 0x16) = tpage;
					At<uint8_t>(p, 0x1C) = u0;
					At<uint8_t>(p, 0x1D) = (uint8_t)v1;
					At<uint8_t>(p, 0x24) = u1;
					At<uint8_t>(p, 0x25) = (uint8_t)v1;
				}
			}
			vtx += 0xC;
		}
	}

	static void StageHide()
	{
		for (uint32_t a = game_addr(game::unk_1D98991); a < game_addr(game::unk_1D98A41); a += 0x2C) At<uint8_t>(a, 0) &= 0xFD;
	}
	static void StageShow()
	{
		for (uint32_t a = game_addr(game::unk_1D98991); a < game_addr(game::unk_1D98A41); a += 0x2C) At<uint8_t>(a, 0) |= 2;
	}

	static void Capture(uint32_t capture_packets, uint32_t offset_xy)
	{
		int16_t rect[4] = { 0x180, 0x100, 0x140, 0xD8 };
		uint32_t pkt = PacketCursor();
		BuildDrawEnvPacket(pkt, game_addr(DRAWENV) + 0x5C * ((uint32_t)DisplayBuffer() ^ 1));
		InsertPrimAutoDepth(OTBase() + 0x406C, (void *)pkt);
		pkt += 0x40;
		PacketCursor() = pkt;
		int32_t ofx, ofy;
		GetScreenOffset(&ofx, &ofy);
		SetScreenOffset(rect[2] / 2, rect[3] / 2);
		const uint32_t h = (uint32_t)FieldAlloc(0x54);
		At<int16_t>(h, 0x3C) = 0;
		At<int16_t>(h, 0x3E) = 0;
		At<int16_t>(h, 0x40) = 0x140;
		At<int16_t>(h, 0x42) = 0xD8;
		At<uint32_t>(h, 0x2C) = var<uint32_t>(game_addr(game::g_BattleVertexWorkspace));
		At<uint32_t>(h, 0x38) = var<uint32_t>(game_addr(game::BATTLE_PACKET_BUFFER_END));
		At<int32_t>(h, 0x48) = -1;
		for (int k = 0; k < 4; k++)
		{
			const uint32_t anim = game_addr(STAGE_Anim0) + 0x2C * (uint32_t)k;
			BuildBoneMatricesFromPose((void *)anim);
			ComputeBonesWorldMatrices((void *)anim, &Camera());
			if (At<uint8_t>(anim, (uint32_t)-0x13) & 1)
			{
				At<uint32_t>(h, 0x44) = At<uint32_t>(anim, 0x14);
				At<uint16_t>(h, 0x4C) = At<uint16_t>(anim, (uint32_t)-0x12);
				FrameArena() = RenderGeometry(anim - 0x10, h + 0x28, OTBase() + (uint32_t)StageDraw()[2 * k] * 4, StageDraw()[2 * k + 1], FrameArena());
			}
		}
		FieldFree(0x54);
		SetScreenOffset(ofx, ofy);
		InsertPrimAltViewport(OTBase() + 0x4484, capture_packets + 0x18 * (uint32_t)DisplayBuffer());
		pkt = PacketCursor();
		BuildDrawAreaPacket(pkt, rect);
		InsertPrimAutoDepth(OTBase() + 0x4484, (void *)pkt);
		pkt += 0xC;
		At<int16_t>(offset_xy, 0) = rect[0];
		At<int16_t>(offset_xy, 2) = rect[1];
		BuildDrawOffsetPacket(pkt, offset_xy);
		InsertPrimAutoDepth(OTBase() + 0x4484, (void *)pkt);
		pkt += 0xC;
		PacketCursor() = pkt;
	}

	static inline int32_t Div41(int32_t x) { int32_t h = (int32_t)(((int64_t)x * 0x63E7063F) >> 32) >> 4; return h + (int32_t)((uint32_t)h >> 31); }
	static inline int32_t Div5(int32_t x) { int32_t h = (int32_t)(((int64_t)x * 0x66666667) >> 32) >> 1; return h + (int32_t)((uint32_t)h >> 31); }

	static void GridWave(int32_t c, int32_t n)
	{
		const int32_t phase = c * ((c >> 1) + 0xD0);
		int32_t s = ComputeSin(shl32(c, 10) / n);
		s = shl32(mul32(s, n) >> 12, 12) / n;
		const int32_t amp = mul32(ComputeSin(s), 48) >> 12;
		uint32_t v = game_addr(GRID);
		for (int32_t rp = 0; rp < 0x7000; rp += 0x400)
			for (int32_t ca = 0; ca < 0x3D800; ca += 0x1800, v += 0xC)
			{
				if (At<uint16_t>(v, 0xA) & 3) continue;
				const int32_t a = Div41(ca);
				const int32_t q = ComputeSin(shl32(mul32(Cos(a), 3), 9) >> 12);
				int32_t e = (shl32(mul32(q, 7), 7) >> 12) + rp;
				e = Div5(mul32(e, 3));
				e += shl32(mul32(ComputeSin(a), 11), 7) >> 12;
				e += phase;
				At<int16_t>(v, 8) = (int16_t)(mul32(ComputeSin(e), amp) >> 12);
			}
	}

	static void IdentityRotation(Mat4x3 *m)
	{
		uint32_t *w = (uint32_t *)m;
		w[3] = 0;
		w[2] = 0;
		w[1] = 0;
		w[0] = 0;
		m->m[2][2] = 0x1000;
		m->m[1][1] = 0x1000;
		m->m[0][0] = 0x1000;
	}

	static void GridDraw(uint32_t quads, uint32_t buffer, uint32_t colour)
	{
		Mat4x3 m;
		m.pad = 0;
		IdentityRotation(&m);
		m.t[0] = 4;
		m.t[1] = 4;
		m.t[2] = var<int16_t>(game_addr(game::BATTLE_PROJECTION_DISTANCE));
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		for (uint32_t v = game_addr(GRID); v < game_addr(GRID_END); v += 0xC)
		{
			GteLoadV0((const void *)(v + 4));
			GteRTPS();
			GteReadSXY2((void *)v);
		}
		const uint32_t code = colour | 0x2C000000;
		uint32_t rec = quads;
		for (int row = 0; row < 0x1B; row++)
			for (int col = 0; col < 0x28; col++, rec += 0x60)
			{
				const uint32_t p = rec + 0x18 + buffer * 0x28;
				At<uint32_t>(p, (uint32_t)-4) = code;
				At<uint32_t>(p, 0) = At<uint32_t>(At<uint32_t>(rec, 0), 0);
				At<uint32_t>(p, 8) = At<uint32_t>(At<uint32_t>(rec, 4), 0);
				At<uint32_t>(p, 0x10) = At<uint32_t>(At<uint32_t>(rec, 8), 0);
				At<uint32_t>(p, 0x18) = At<uint32_t>(At<uint32_t>(rec, 0xC), 0);
				InsertPrimAutoDepth(OTBase() + 0x4068, (void *)(p - 8));
			}
	}

	static uint32_t __cdecl Controller(TaskNode *n)
	{
		if (DrawOnly()) return 0;
		ControllerNode *c = (ControllerNode *)n;
		if (c->counter == 0) GridInit();
		if (c->counter == 1)
		{
			for (int k = 0; k < 4; k++)
			{
				const uint32_t vec = game_addr(EMITTER_Vectors) + 0x10 * (uint32_t)k;
				EmitterNode *e = (EmitterNode *)AddTaskToQueue(QEffects(), game_addr(ORIG_Emitter));
				e->counter = 0;
				e->mask = (uint16_t)(1u << (2 * k));
				GetEffectSpawnPosition(Entity(CasterSlot()), 0xF1, 0x1000, c->anchor);
				int16_t v[4];
				MatVec(Entity(CasterSlot()) + 0x40, (const int16_t *)vec, v);
				e->pos[0] = (int16_t)(c->anchor[0] + v[0]);
				e->pos[1] = (int16_t)(v[1] + c->anchor[1]);
				e->pos[2] = (int16_t)(c->anchor[2] + v[2]);
				MatVec(Entity(CasterSlot()) + 0x40, (const int16_t *)(vec + 8), v);
				e->dir[0] = (int16_t)(c->anchor[0] - e->pos[0] + v[0]);
				e->dir[1] = (int16_t)(v[1] - e->pos[1] + c->anchor[1]);
				e->dir[2] = (int16_t)(c->anchor[2] - e->pos[2] + v[2]);
				NormalizeSVector(e->dir, e->dir);
				e->angle = (int16_t)(CrtRand() % 0x1000);
				e->colour = 0;
			}
		}
		if (c->counter == 8)
		{
			WarpNode *w = (WarpNode *)AddTaskToQueue(QEffects(), game_addr(ORIG_ScreenWarp));
			w->counter = 0;
		}
		if (c->counter == 0x24) ApplyActionResultToTargets(Ctx()->actions->targets, Ctx()->actions->target_count);
		if (c->counter == 1) BdPlaySE(SOUND_Waves(), 0, 0x80);
		c->counter++;
		return c->counter > 0x28 ? TASK_END : 0;
	}

	static void EmitterBegin(RenderHeader *h, EmitterWork *w, const EmitterNode *e)
	{
		h->far_rgb[0] = h->far_rgb[1] = h->far_rgb[2] = h->far_rgb[3] = 0;
		w->from[0] = 0;
		w->from[1] = 0;
		h->model = game_addr(MODEL_Ring);
		h->flags = 0x33;
		w->from[2] = -0x1000;
		w->to[0] = e->dir[0];
		w->to[1] = e->dir[1];
		w->to[2] = e->dir[2];
	}

	static void ParticleDraw(RenderHeader *h, EmitterWork *w, const Particle &p, uint32_t colour, int32_t fade)
	{
		w->m.t[2] = p.pos[2];
		w->m.t[0] = p.pos[0];
		w->m.t[1] = p.pos[1];
		const int32_t angle = RotationBetweenVectors(w->from, w->to, w->axis);
		BuildAxisAngleRotationMatrix(angle, &w->m, w->axis);
		BuildAxisAngleRotationMatrix(p.angle, &w->spin, w->to);
		MulMatrixInPlace(&w->spin, &w->m);
		w->offs[1] = 0;
		w->offs[2] = 0;
		w->offs[0] = p.radius;
		MatVec(&w->spin, w->offs, w->offs);
		w->m.t[0] = (int32_t)((uint32_t)w->m.t[0] + (uint32_t)(int32_t)w->offs[0]);
		w->m.t[1] = (int32_t)((uint32_t)w->m.t[1] + (uint32_t)(int32_t)w->offs[1]);
		w->m.t[2] = (int32_t)((uint32_t)w->m.t[2] + (uint32_t)(int32_t)w->offs[2]);
		w->scale[2] = p.scale;
		w->scale[1] = p.scale;
		w->scale[0] = p.scale;
		Scale3DMatrix(&w->m, w->scale);
		ComposeAffineTransform(&Camera(), &w->m, &w->m);
		GteSetRotMatrix(&w->m);
		GteSetTransVector(&w->m);
		h->colour = colour;
		if (p.life >= 6)
		{
			h->fade = fade;
			h->flags |= 0xC0;
		}
		PacketCursor() = RenderModel(h, OTBase() + 0x44, 2, PacketCursor());
	}

	static inline int32_t ParticleFade(int32_t life) { return (life - 6) * 682; }

	static bool ParticleUpdate(Particle &p)
	{
		p.life++;
		if (p.life >= 0xC)
		{
			p.mask = 0;
			return false;
		}
		p.colour = (int16_t)(((uint8_t)p.colour + 1) & 0xF);
		p.scale = (int16_t)(p.scale + p.scale / 6);
		p.pos[0] = (int16_t)(p.pos[0] + p.vel[0]);
		p.pos[1] = (int16_t)(p.pos[1] + p.vel[1]);
		p.pos[2] = (int16_t)(p.pos[2] + p.vel[2]);
		p.angle = (int16_t)(p.angle + 0x200);
		p.radius = (int16_t)(p.radius + 0xA);
		return true;
	}

	static void EmitterSpawn(EmitterNode *e, Particle *parts)
	{
		if (e->counter < 0 || e->counter > 0x14) return;
		for (int i = 0; i < PART_COUNT; i++)
		{
			Particle &p = parts[i];
			if (p.mask != 0) continue;
			p.mask = e->mask;
			p.colour = e->colour;
			ff8fx::copy<(4)>(&p.pos[0], &e->pos[0]);
			p.life = 0;
			p.scale = 0x400;
			ff8fx::copy<(4)>(&p.pos[2], &e->pos[2]);
			p.angle = e->angle;
			p.vel[0] = (int16_t)(mul32(e->dir[0], 150) >> 12);
			p.vel[1] = (int16_t)(mul32(e->dir[1], 150) >> 12);
			p.vel[2] = (int16_t)(mul32(e->dir[2], 150) >> 12);
			p.radius = 100;
			return;
		}
	}

	static void EmitterAdvance(EmitterNode *e)
	{
		e->angle = (int16_t)(e->angle + 0x100);
		e->colour = (int16_t)(((uint8_t)e->colour - 1) & 0xF);
	}

	static uint32_t __cdecl Emitter(TaskNode *n)
	{
		EmitterNode *e = (EmitterNode *)n;
		RenderHeader *h = (RenderHeader *)FieldAlloc(0x6C);
		EmitterWork *w = (EmitterWork *)FieldAlloc(0x88);
		int alive = 0;
		EmitterBegin(h, w, e);
		for (int i = 0; i < PART_COUNT; i++)
		{
			Particle &p = Parts()[i];
			if (!(e->mask & p.mask)) continue;
			ParticleDraw(h, w, p, RingColours()[p.colour], ParticleFade(p.life));
			if (DrawOnly()) continue;
			if (ParticleUpdate(p)) alive++;
		}
		FieldFree(0x88);
		FieldFree(0x6C);
		if (DrawOnly()) return 0;
		EmitterSpawn(e, Parts());
		EmitterAdvance(e);
		e->counter++;
		if (e->counter < 0xC) return 0;
		return alive == 0 ? TASK_END : 0;
	}

	static void WarpDraw(int32_t c, uint32_t capture_packets, uint32_t offset_xy, uint32_t quads)
	{
		Capture(capture_packets, offset_xy);
		GridWave(c, WARP_TICKS);
		GridDraw(quads, DisplayBuffer(), WarpColours()[c]);
	}

	static uint32_t __cdecl ScreenWarp(TaskNode *n)
	{
		WarpNode *w = (WarpNode *)n;
		if (w->counter < WARP_TICKS)
		{
			WarpDraw(w->counter, game_addr(PKT_Capture), game_addr(OFFSET_XY), game_addr(QUADS));
		}
		if (DrawOnly()) return 0;
		if (w->counter == 0) StageHide();
		if (w->counter == WARP_TICKS) StageShow();
		w->counter++;
		return w->counter > WARP_TICKS ? TASK_END : 0;
	}
}

	void register_mag154_ultra_waves()
	{
		register_port(ultra154::ORIG_RootTask, (void *)ultra154::RootTask, "U154 RootTask", 154);
		register_port(ultra154::ORIG_WaitTexRestore, (void *)ultra154::WaitTexRestore, "U154 WaitTexRestore", 154);
		register_port(ultra154::ORIG_Controller, (void *)ultra154::Controller, "U154 Controller", 154);
		register_port(ultra154::ORIG_Emitter, (void *)ultra154::Emitter, "U154 Emitter", 154);
		register_port(ultra154::ORIG_ScreenWarp, (void *)ultra154::ScreenWarp, "U154 ScreenWarp", 154);
	}
}

