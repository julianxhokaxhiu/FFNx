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

// Effect 152: Ultrasonic Waves

#include "mag_common.h"

namespace ff8fx
{
namespace ultrasonic152
{
	using namespace eng;
	using namespace magc;

	inline int32_t &TargetSlot() { return var<int32_t>(game_addr(game::g_mag152_TargetSlot)); }
	inline TaskQueue *QRoot() { return (TaskQueue *)game_addr(game::g_mag152_RootQueue); }
	inline TaskQueue *QEffects() { return (TaskQueue *)game_addr(game::g_mag152_EffectQueue); }
	inline int16_t *TargetPos() { return (int16_t *)game_addr(game::g_mag152_TargetPos); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::g_mag152_CastContext)); }
	inline int32_t &CasterSlot() { return var<int32_t>(game_addr(game::g_mag152_CasterSlot)); }
	inline Mat4x3 &Rot() { return var<Mat4x3>(game_addr(game::g_mag152_FlightRotation)); }
	inline int16_t *Anchor() { return (int16_t *)game_addr(game::g_mag152_CasterAnchor); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::g_mag152_MagicBuffer)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::g_mag152_PacketCursor)); }
	inline uint32_t RenderOT44() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44; }

	static constexpr game::Id ORIG_RootTask = game::MAG_152_UltrasonicWaves_RootTask;
	static constexpr game::Id ORIG_WaitTexRestore = game::MAG_152_UltrasonicWaves_WaitTexRestoreTask;
	static constexpr game::Id ORIG_Controller = game::MAG_152_UltrasonicWaves_ControllerTask;
	static constexpr game::Id ORIG_WaveTask = game::MAG_152_UltrasonicWaves_WaveTask;
	static constexpr game::Id ORIG_RingTask = game::MAG_152_UltrasonicWaves_RingTask;
	static constexpr game::Id ORIG_BurstA = game::MAG_152_UltrasonicWaves_BurstTaskA;
	static constexpr game::Id ORIG_BurstB = game::MAG_152_UltrasonicWaves_BurstTaskB;
	static constexpr game::Id ORIG_BurstC = game::MAG_152_UltrasonicWaves_BurstTaskC;
	static constexpr game::Id SEQ_Wave = game::unk_DBE5CC;
	static constexpr game::Id SEQ_BurstA = game::unk_DBE420;
	static constexpr game::Id SEQ_BurstB = game::unk_DBE814;
	static constexpr game::Id SEQ_BurstC = game::unk_DBE904;
	static constexpr game::Id MODEL_Ring = game::unk_DBE9E0;
	static const uint32_t *RingColours() { return (const uint32_t *)game_addr(game::MAG_152_RingColourCycle); }
	static const void *SOUND_Waves() { return (const void *)game_addr(game::unk_DBEC50); }

	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t flag, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, flag, out); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
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
		uint8_t pad0E[0x16];
	};
	struct WaveNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		int16_t pos[3];
		int16_t pad16;
		int16_t roll;
		int16_t pad1A;
		int16_t scale;
		uint8_t pad1E[6];
	};
	struct RingNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		int16_t pos[3];
		int16_t angle;
		int16_t vel[3];
		int16_t spin;
		int16_t scale;
		int16_t grow;
	};
	struct BurstNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		int16_t pos[4];
		uint8_t pad18[0xC];
	};
	struct Particle
	{
		uint32_t type;
		int16_t frame;
		int16_t scale;
		int16_t pos[4];
		uint8_t pad10[8];
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
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x10, "Ultrasonic Waves root nodes");
	static_assert(sizeof(ControllerNode) == 0x24 && sizeof(WaveNode) == 0x24 && sizeof(RingNode) == 0x24 && sizeof(BurstNode) == 0x24, "Ultrasonic Waves nodes");
	static_assert(sizeof(Particle) == 0x18 && sizeof(RenderHeader) == 0x6C, "Ultrasonic Waves particle / render header");

	inline Particle *Parts() { return (Particle *)game_addr(game::g_mag152_Particles); }
	static const int PART_COUNT = 100;
}
}

namespace ff8fx
{
namespace ultrasonic152
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
		if (r->counter & 1) PacketCursor() = TexBase() + 0x8000;
		else PacketCursor() = TexBase();
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
		if (w->counter == 1) TextureRestoreTask((void *)TexBase(), &w->done);
		int16_t done = w->done;
		w->counter++;
		return done ? TASK_END : 0;
	}

	static uint32_t __cdecl Controller(TaskNode *n)
	{
		int32_t dir[3];
		dir[0] = TargetPos()[0] - Anchor()[0];
		dir[1] = TargetPos()[1] - Anchor()[1];
		dir[2] = TargetPos()[2] - Anchor()[2];
		int32_t down[3] = { 0, 0, -0x1000 };
		NormalizeVector(dir, dir);
		int32_t axis[3];
		const int32_t angle = RotationBetweenVectors(down, dir, axis);
		BuildAxisAngleRotationMatrix(angle, &Rot(), axis);
		if (DrawOnly()) return 0;
		ControllerNode *c = (ControllerNode *)n;
		if (c->counter == 0)
		{
			GetEffectSpawnPosition(Entity(CasterSlot()), 0xF0, 0x1000, Anchor());
			GetDefaultEffectPosition(Entity(TargetSlot()), TargetPos());
		}
		if (c->counter >= 1 && c->counter <= 0xE && c->counter % 5 == 1)
		{
			WaveNode *w = (WaveNode *)AddTaskToQueue(QEffects(), game_addr(ORIG_WaveTask));
			ff8fx::copy<(4)>(&w->pos[0], &Anchor()[0]);
			w->counter = 0;
			ff8fx::copy<(4)>(&w->pos[2], &Anchor()[2]);
			w->pos[0] = (int16_t)(w->pos[0] + (int16_t)(CrtRand() % 0x6E - 0x37));
			w->pos[1] = (int16_t)(w->pos[1] + (int16_t)(CrtRand() % 0x6E - 0x37));
			w->pos[2] = (int16_t)(w->pos[2] + (int16_t)(CrtRand() % 0x6E - 0x37));
			w->roll = (int16_t)(CrtRand() % 0x1000);
			w->scale = (int16_t)(CrtRand() % 0x400 + 0x900);
		}
		if (c->counter >= 5 && c->counter <= 0x14)
		{
			RingNode *g = (RingNode *)AddTaskToQueue(QEffects(), game_addr(ORIG_RingTask));
			ff8fx::copy<(4)>(&g->pos[0], &Anchor()[0]);
			g->counter = 0;
			ff8fx::copy<(4)>(&g->pos[2], &Anchor()[2]);
			g->angle = (int16_t)(CrtRand() % 0x1000);
			g->vel[0] = (int16_t)((TargetPos()[0] - Anchor()[0]) / 18);
			g->vel[1] = (int16_t)((TargetPos()[1] - Anchor()[1]) / 18);
			g->vel[2] = (int16_t)((TargetPos()[2] - Anchor()[2]) / 18);
			const int32_t r = CrtRand() % 0x28;
			g->grow = 0x97;
			g->scale = 0x97;
			g->spin = (int16_t)(r + 0xA);
		}
		if (c->counter == 0x19)
		{
			BurstNode *b = (BurstNode *)AddTaskToQueue(QEffects(), game_addr(ORIG_BurstA));
			ff8fx::copy<(4)>(&b->pos[0], &TargetPos()[0]);
			b->counter = 0;
			ff8fx::copy<(4)>(&b->pos[2], &TargetPos()[2]);
		}
		if (c->counter == 0x1A)
		{
			BurstNode *b = (BurstNode *)AddTaskToQueue(QEffects(), game_addr(ORIG_BurstB));
			ff8fx::copy<(4)>(&b->pos[0], &TargetPos()[0]);
			b->counter = 0;
			ff8fx::copy<(4)>(&b->pos[2], &TargetPos()[2]);
		}
		if (c->counter == 0x1A)
		{
			BurstNode *b = (BurstNode *)AddTaskToQueue(QEffects(), game_addr(ORIG_BurstC));
			ff8fx::copy<(4)>(&b->pos[0], &TargetPos()[0]);
			b->counter = 0;
			ff8fx::copy<(4)>(&b->pos[2], &TargetPos()[2]);
		}
		if (c->counter == 0x23) ApplyActionResultToTarget(Ctx()->actions[0].targets);
		if (c->counter == 1) BdPlaySE(SOUND_Waves(), 0, 0x80);
		c->counter++;
		return c->counter > 0x28 ? TASK_END : 0;
	}

	static int16_t WaveDraw(const WaveNode *p, int32_t level)
	{
		TransformCameraByShadowRotation(p->pos, p->scale, -(p->scale >> 3));
		uint8_t *h = AllocHeader(0xB4);
		*(uint32_t *)h = game_addr(SEQ_Wave);
		*(uint16_t *)(h + 4) = (uint16_t)p->counter;
		*(int32_t *)(h + 8) = p->roll;
		*(uint16_t *)(h + 0x24) = 9;
		if (p->counter >= 8)
		{
			*(uint16_t *)(h + 0x24) = 0xD;
			const uint32_t c = (uint32_t)level;
			*(uint32_t *)(h + 0x1C) = (((c << 8) | c) << 8) | c;
		}
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT44(), 2, PacketCursor());
		FieldFree(0xB4);
		return *(const int16_t *)(h + 0x28);
	}

	static uint32_t __cdecl WaveTask(TaskNode *n)
	{
		WaveNode *p = (WaveNode *)n;
		const int16_t next = WaveDraw(p, shl32(0xC - p->counter, 5));
		if (DrawOnly()) return 0;
		p->counter++;
		return next < 0 ? TASK_END : 0;
	}

	static void RingDraw(const RingNode *p, uint32_t colour, int32_t fade)
	{
		int16_t angles[4] = { 0, 0, p->angle, 0 };
		Mat4x3 m = {};
		BuildRotationMatrixFromAngles(angles, &m);
		m.t[0] = p->pos[0];
		m.t[1] = p->pos[1];
		m.t[2] = p->pos[2];
		int32_t v[3] = { p->scale, p->scale, p->scale };
		Scale3DMatrix(&m, v);
		MatrixMultiply(&Rot(), &m);
		MatrixMultiply(&Camera(), &m);
		TransformVectorBy3x3Matrix(&Camera(), m.t, m.t);
		m.t[0] = (int32_t)((uint32_t)m.t[0] + (uint32_t)Camera().t[0]);
		m.t[1] = (int32_t)((uint32_t)m.t[1] + (uint32_t)Camera().t[1]);
		m.t[2] = (int32_t)((uint32_t)m.t[2] + (uint32_t)Camera().t[2]);
		NormalizeVector(m.t, v);
		const int32_t k = -(p->scale >> 4);
		m.t[0] = (int32_t)((uint32_t)m.t[0] + (uint32_t)(mul32(k, v[0]) >> 12));
		m.t[1] = (int32_t)((uint32_t)m.t[1] + (uint32_t)(mul32(k, v[1]) >> 12));
		m.t[2] = (int32_t)((uint32_t)m.t[2] + (uint32_t)(mul32(k, v[2]) >> 12));
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		RenderHeader *h = (RenderHeader *)AllocHeader(0x6C);
		h->far_rgb[0] = h->far_rgb[1] = h->far_rgb[2] = h->far_rgb[3] = 0;
		h->model = game_addr(MODEL_Ring);
		h->flags = 0x33;
		h->colour = colour;
		if (p->counter >= 0x10)
		{
			h->flags = 0xF3;
			h->fade = fade;
		}
		PacketCursor() = RenderModel(h, RenderOT44(), 2, PacketCursor());
		FieldFree(0x6C);
	}

	static void RingUpdate(RingNode *p)
	{
		p->pos[0] = (int16_t)(p->pos[0] + p->vel[0]);
		p->pos[1] = (int16_t)(p->pos[1] + p->vel[1]);
		p->pos[2] = (int16_t)(p->pos[2] + p->vel[2]);
		p->angle = (int16_t)(p->angle + p->spin);
		p->scale = (int16_t)(p->scale + p->grow);
	}

	static uint32_t __cdecl RingTask(TaskNode *n)
	{
		RingNode *p = (RingNode *)n;
		RingDraw(p, RingColours()[p->counter % 10], shl32(p->counter - 0x10, 10));
		if (DrawOnly()) return 0;
		RingUpdate(p);
		p->counter++;
		return p->counter > 0x14 ? TASK_END : 0;
	}

	static int BurstDraw(Particle *parts, uint32_t seq, uint16_t mode, uint32_t type)
	{
		uint8_t *h = AllocHeader(0xB4);
		*(uint32_t *)h = seq;
		*(uint16_t *)(h + 0x24) = mode;
		int alive = 0;
		for (int i = 0; i < PART_COUNT; i++)
		{
			Particle &e = parts[i];
			if (!((uint8_t)e.type & type)) continue;
			*(uint16_t *)(h + 4) = (uint16_t)e.frame;
			TransformCameraByShadowRotation(e.pos, e.scale, -(e.scale >> 4));
			PacketCursor() = InitEffectSequenceFromData(h, RenderOT44(), 2, PacketCursor());
			if (DrawOnly()) continue;
			e.frame++;
			if (*(const int16_t *)(h + 0x28) < 0) e.type = 0;
			else alive++;
		}
		FieldFree(0xB4);
		return alive;
	}

	static Particle *FreeParticle()
	{
		for (int i = 0; i < PART_COUNT; i++)
			if (Parts()[i].type == 0) return &Parts()[i];
		return nullptr;
	}

	static uint32_t __cdecl BurstA(TaskNode *n)
	{
		BurstNode *b = (BurstNode *)n;
		const int alive = BurstDraw(Parts(), game_addr(SEQ_BurstA), 8, 1);
		if (DrawOnly()) return 0;
		if (b->counter >= 0 && b->counter <= 8)
		{
			for (int k = 0; k < 2; k++)
			{
				Particle *e = FreeParticle();
				if (!e) break;
				e->type = 1;
				e->frame = 0;
				e->scale = (int16_t)(CrtRand() % 0x700 + 0x300);
				ff8fx::copy<(8)>(&e->pos[0], &b->pos[0]);
				e->pos[0] = (int16_t)(e->pos[0] + (int16_t)(CrtRand() % 0x320 - 0x190));
				e->pos[1] = (int16_t)(e->pos[1] + (int16_t)(CrtRand() % 0x320 - 0x190));
				e->pos[2] = (int16_t)(e->pos[2] + (int16_t)(CrtRand() % 0x320 - 0x190));
			}
		}
		b->counter++;
		return (b->counter >= 4 && alive == 0) ? TASK_END : 0;
	}

	static uint32_t __cdecl BurstB(TaskNode *n)
	{
		BurstNode *b = (BurstNode *)n;
		const int alive = BurstDraw(Parts(), game_addr(SEQ_BurstB), 0, 2);
		if (DrawOnly()) return 0;
		if (b->counter >= 0 && b->counter <= 0xD)
		{
			Particle *e = FreeParticle();
			if (e)
			{
				e->type = 2;
				e->frame = 0;
				e->scale = (int16_t)(CrtRand() % 0x400 + 0x300);
				ff8fx::copy<(8)>(&e->pos[0], &b->pos[0]);
				e->pos[0] = (int16_t)(e->pos[0] + (int16_t)(CrtRand() % 0x384 - 0x1C2));
				e->pos[1] = (int16_t)(e->pos[1] + (int16_t)(CrtRand() % 0x384 - 0x1C2));
				e->pos[2] = (int16_t)(e->pos[2] + (int16_t)(CrtRand() % 0x384 - 0x1C2));
			}
		}
		b->counter++;
		return (b->counter >= 4 && alive == 0) ? TASK_END : 0;
	}

	static uint32_t __cdecl BurstC(TaskNode *n)
	{
		BurstNode *b = (BurstNode *)n;
		const int alive = BurstDraw(Parts(), game_addr(SEQ_BurstC), 0, 4);
		if (DrawOnly()) return 0;
		if (b->counter >= 0 && b->counter <= 0xD)
		{
			Particle *e = FreeParticle();
			if (e)
			{
				e->type = 4;
				e->frame = 0;
				e->scale = (int16_t)(CrtRand() % 0x500 + 0x500);
				ff8fx::copy<(8)>(&e->pos[0], &b->pos[0]);
				e->pos[0] = (int16_t)(e->pos[0] + (int16_t)(CrtRand() % 0x384 - 0x1C2));
				e->pos[1] = (int16_t)(e->pos[1] + (int16_t)(CrtRand() % 0x384 - 0x1C2));
				e->pos[2] = (int16_t)(e->pos[2] + (int16_t)(CrtRand() % 0x384 - 0x1C2));
			}
		}
		b->counter++;
		return (b->counter >= 4 && alive == 0) ? TASK_END : 0;
	}
}

	void register_mag152_ultrasonic_waves()
	{
		register_port(ultrasonic152::ORIG_RootTask, (void *)ultrasonic152::RootTask, "U152 RootTask", 152);
		register_port(ultrasonic152::ORIG_WaitTexRestore, (void *)ultrasonic152::WaitTexRestore, "U152 WaitTexRestore", 152);
		register_port(ultrasonic152::ORIG_Controller, (void *)ultrasonic152::Controller, "U152 Controller", 152);
		register_port(ultrasonic152::ORIG_WaveTask, (void *)ultrasonic152::WaveTask, "U152 WaveTask", 152);
		register_port(ultrasonic152::ORIG_RingTask, (void *)ultrasonic152::RingTask, "U152 RingTask", 152);
		register_port(ultrasonic152::ORIG_BurstA, (void *)ultrasonic152::BurstA, "U152 BurstA", 152);
		register_port(ultrasonic152::ORIG_BurstB, (void *)ultrasonic152::BurstB, "U152 BurstB", 152);
		register_port(ultrasonic152::ORIG_BurstC, (void *)ultrasonic152::BurstC, "U152 BurstC", 152);
	}
}

