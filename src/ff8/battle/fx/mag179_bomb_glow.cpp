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
namespace bombglow179
{
	using namespace eng;
	using namespace magc;

	template<typename T> inline T &AT(uint32_t p, int32_t o) { return *(T *)(p + o); }
	inline int16_t &S16(uint32_t p, int32_t o) { return AT<int16_t>(p, o); }
	inline uint16_t &U16(uint32_t p, int32_t o) { return AT<uint16_t>(p, o); }
	inline int32_t &S32(uint32_t p, int32_t o) { return AT<int32_t>(p, o); }
	inline uint32_t &U32(uint32_t p, int32_t o) { return AT<uint32_t>(p, o); }

	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::g_mag179_castContext)); }
	inline uint32_t &CasterSlot() { return var<uint32_t>(game_addr(game::g_mag179_casterSlot)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::g_mag179_packetCursor)); }
	inline TaskQueue *EffectQueue() { return (TaskQueue *)game_addr(game::g_mag179_effectQueue); }
	inline uint32_t &FrameCursor() { return var<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)); }
	inline uint32_t &BoneWorkspace() { return var<uint32_t>(game_addr(game::g_BattleVertexWorkspace)); }
	inline uint32_t RenderOT(uint32_t off) { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + off; }
	static constexpr game::Id ARENA0 = game::unk_230AD0C, ARENA1 = game::unk_2312D0C; static const uint32_t ARENA_SIZE = 0x8000;
	static constexpr game::Id POOL = game::g_mag179_particlePool, POOL_END = game::g_mag179_effectQueue;

	static constexpr game::Id ORIG_RootTask = game::MAG_179_BombGlow_RootTask;
	static constexpr game::Id ORIG_Director = game::MAG_179_BombGlow_DirectorTask;
	static constexpr game::Id ORIG_CasterGlow = game::MAG_179_BombGlow_CasterGlowTask;
	static constexpr game::Id ORIG_FuseSparks = game::MAG_179_BombGlow_FuseSparksTask;
	static constexpr game::Id SEQ_Spark = game::MAG_179_SparkSeq;
	static constexpr game::Id GLOW_COLOURS = game::MAG_179_GlowColours;
	static const void *SOUND_Glow() { return (const void *)game_addr(game::unk_D44968); }

	inline void GteSetFarColor(uint32_t r, uint32_t g, uint32_t b) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_SetFarColor))(r, g, b); }
	inline void GteLoadRGBC(uint32_t c) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::GTE_LoadRGBC))(c); }
	inline void GteDPCS() { fn<void (__cdecl *)()>(game_addr(game::GTE_DPCS))(); }
	inline void GteStoreRGB2(uint32_t c) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::GTE_StoreRGB2))(c); }
	inline void GteLoadV012(uint32_t a, uint32_t b, uint32_t c) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_LoadV012))(a, b, c); }
	inline void GteRTPT() { fn<void (__cdecl *)()>(game_addr(game::GTE_RTPT))(); }
	inline void GteReadFLAG(uint32_t dst) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::GTE_ReadFLAG))(dst); }
	inline void GteNCLIP() { fn<void (__cdecl *)()>(game_addr(game::GTE_NCLIP))(); }
	inline void GteReadMAC0(uint32_t dst) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::GTE_ReadMAC0))(dst); }
	inline void GteReadSXY012(uint32_t a, uint32_t b, uint32_t c) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_ReadSXY012_Split))(a, b, c); }
	inline void GteAVSZ3() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ3))(); }
	inline void GteAVSZ4() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ4))(); }
	inline void GteReadOTZ32(uint32_t dst) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::GTE_ReadOTZ))(dst); }
	inline void GteMVMVA_RotV0Tr2() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0_Tr_2))(); }
	inline void InsertPrim(uint32_t bucket, uint32_t pkt) { fn<void (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::SSIGPU_InsertPrimAutoDepth))(bucket, pkt); }
	inline void GteLoadV0Words(uint32_t v) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::GTE_LoadV0FromSVec3))(v); }
	inline void GteStoreIRVertex(uint32_t out) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::GTE_StoreIR123ToSVec))(out); }
	inline uint32_t DrawShadow(uint8_t *entity, uint32_t ot, uint32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(uint8_t *, uint32_t, uint32_t, uint32_t)>(game_addr(game::BattleEntity_DrawShadow))(entity, ot, mode, cursor); }
	inline uint32_t RenderGeometry(uint32_t model, uint32_t h, uint32_t ot, uint32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t)>(game_addr(game::RenderGeometry))(model, h, ot, mode, cursor); }
	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t flag, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, flag, out); }

#pragma pack(push, 1)
	struct RootNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad;
	};
	struct EffectNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t pad0E[0xC];
		uint16_t hidden;
		uint8_t pad1C[8];
	};
	struct Particle
	{
		uint32_t type;
		int16_t frame;
		int16_t scale;
		int16_t pos[4];
		int16_t mirror[2];
		uint32_t pad14;
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x10 && sizeof(EffectNode) == 0x24, "Bomb glow nodes");
	static_assert(sizeof(Particle) == 0x18, "Bomb glow particle");

	inline Particle *Pool() { return (Particle *)game_addr(POOL); }
	static const int POOL_COUNT = 0x64;
	inline uint8_t *Caster() { return Entity((int)CasterSlot()); }

	static int FreeParticle()
	{
		int i = 0;
		for (uint32_t p = game_addr(POOL); ; p += 0x18, i++)
		{
			if (U32(p, 0) == 0) break;
			if (p + 0x18 >= game_addr(POOL_END)) return -1;
		}
		if (i >= 0x64) return -1;
		return i;
	}

	static int32_t ScrollV(int16_t y, int32_t scroll) { return (y + scroll) / 3; }
	static uint16_t RebaseUV(uint32_t h, int32_t o, int32_t q, int32_t base)
	{
		return (uint16_t)(((uint32_t)(q - base) << 8) ^ AT<uint8_t>(h, o));
	}

	static uint32_t RenderObjectPrims(uint32_t h, uint32_t ot, uint32_t shift, uint32_t pkt)
	{
		const int32_t base = S16(h, 0x3A);
		const uint32_t verts = U32(h, 4);
		uint32_t prim = U32(h, 0);
		for (int32_t i = 0; i < (int32_t)U16(h, 8); i++, prim += 0x10)
		{
			GteLoadV012(verts + (U16(prim, 0) & 0xFFF) * 8, verts + (U16(prim, 2) & 0xFFF) * 8, verts + (U16(prim, 4) & 0xFFF) * 8);
			GteRTPT();
			U32(pkt, 0x14) = U32(prim, 0xC);
			U32(pkt, 0) = 0x7000000;
			U32(pkt, 4) = U32(h, 0x1C) | 0x24000000;
			U32(pkt, 0xC) = U32(prim, 8);
			GteReadFLAG(h + 0x48);
			if (U32(h, 0x48) & 0x60000) continue;
			GteNCLIP();
			GteReadMAC0(h + 0x3C);
			if (S32(h, 0x3C) <= 0) continue;
			const uint32_t xy0 = pkt + 8, xy1 = pkt + 0x10, xy2 = pkt + 0x18;
			GteReadSXY012(xy0, xy1, xy2);
			GteAVSZ3();
			S32(pkt, 0x1C) = S32(prim, 4) >> 16;
			GteReadOTZ32(h + 0x44);
			int32_t z = S32(h, 0x44) >> shift;
			InsertPrim(ot + (uint32_t)z * 4, pkt);
			const uint32_t v0 = verts + (U16(prim, 0) & 0xFFF) * 8, v1 = verts + (U16(prim, 2) & 0xFFF) * 8, v2 = verts + (U16(prim, 4) & 0xFFF) * 8;
			U32(h, 0x4C) = U32(v0, 0);
			U32(h, 0x50) = U32(v0, 4);
			U32(h, 0x54) = U32(v1, 0);
			U32(h, 0x58) = U32(v1, 4);
			U32(h, 0x5C) = U32(v2, 0);
			U32(h, 0x60) = U32(v2, 4);
			const int32_t scroll = S32(h, 0x30);
			const int32_t q0 = ScrollV(S16(h, 0x4E), scroll);
			S32(h, 0x70) = q0;
			const int32_t q1 = ScrollV(S16(h, 0x56), scroll);
			S32(h, 0x74) = q1;
			const int32_t q2 = ScrollV(S16(h, 0x5E), scroll);
			S32(h, 0x78) = q2;
			const uint16_t p0 = (uint16_t)q0 & 0xFF00, p1 = (uint16_t)q1 & 0xFF00, p2 = (uint16_t)q2 & 0xFF00;
			U16(h, 0x50) = p0;
			U16(h, 0x58) = p1;
			U16(h, 0x60) = p2;
			if (p0 != p1 || p0 != p2)
			{
				U16(h, 0x52) = RebaseUV(h, 0x52, q0, base);
				U16(h, 0x5A) = RebaseUV(h, 0x5A, q1, base);
				U16(h, 0x62) = RebaseUV(h, 0x62, q2, base);
			}
			if (z > 0) z--;
			pkt += 0x20;
			U32(pkt, 8) = U32(h, 0x6C) | 0x26000000;
			U16(pkt, 0x10) = U16(h, 0x52);
			U16(pkt, 0x18) = U16(h, 0x5A);
			U16(pkt, 0x20) = U16(h, 0x62);
			U32(pkt, 0) = 0x9000000;
			U32(pkt, 0xC) = U32(xy0, 0);
			U32(pkt, 0x14) = U32(xy1, 0);
			U16(pkt, 0x12) = 0x3F54;
			U32(pkt, 0x1C) = U32(xy2, 0);
			U32(pkt, 4) = U32(h, 0x88);
			U16(pkt, 0x1A) = 0xBA;
			U32(pkt, 0x24) = U32(h, 0x8C);
			InsertPrim(ot + (uint32_t)z * 4, pkt);
			pkt += 0x28;
		}
		if (U16(h, 0xA) != 0)
		{
			for (int32_t i = 0; i < (int32_t)U16(h, 0xA); i++, prim += 0x14)
			{
				GteLoadV012(verts + (U16(prim, 0) & 0xFFF) * 8, verts + (U16(prim, 2) & 0xFFF) * 8, verts + (U16(prim, 4) & 0xFFF) * 8);
				GteRTPT();
				U32(pkt, 0xC) = U32(prim, 8);
				U32(pkt, 0) = 0x9000000;
				U32(pkt, 4) = U32(h, 0x1C) | 0x2C000000;
				U32(pkt, 0x14) = U32(prim, 0xC);
				GteReadFLAG(h + 0x48);
				if (U32(h, 0x48) & 0x60000) continue;
				GteNCLIP();
				GteReadMAC0(h + 0x3C);
				if (S32(h, 0x3C) <= 0) continue;
				const uint32_t xy0 = pkt + 8, xy1 = pkt + 0x10, xy2 = pkt + 0x18, xy3 = pkt + 0x20;
				GteReadSXY012(xy0, xy1, xy2);
				GteLoadV0((const void *)(verts + (U16(prim, 6) & 0xFFF) * 8));
				GteRTPS();
				U32(pkt, 0x1C) = U32(prim, 0x10);
				S32(pkt, 0x24) = S32(prim, 0x10) >> 16;
				GteReadSXY2((void *)xy3);
				GteAVSZ4();
				GteReadOTZ32(h + 0x44);
				int32_t z = S32(h, 0x44) >> shift;
				InsertPrim(ot + (uint32_t)z * 4, pkt);
				const uint32_t v0 = verts + (U16(prim, 0) & 0xFFF) * 8, v1 = verts + (U16(prim, 2) & 0xFFF) * 8;
				const uint32_t v2 = verts + (U16(prim, 4) & 0xFFF) * 8, v3 = verts + (U16(prim, 6) & 0xFFF) * 8;
				U32(h, 0x4C) = U32(v0, 0);
				U32(h, 0x50) = U32(v0, 4);
				U32(h, 0x54) = U32(v1, 0);
				U32(h, 0x58) = U32(v1, 4);
				U32(h, 0x5C) = U32(v2, 0);
				U32(h, 0x60) = U32(v2, 4);
				U32(h, 0x64) = U32(v3, 0);
				U32(h, 0x68) = U32(v3, 4);
				const int32_t scroll = S32(h, 0x30);
				const int32_t q0 = ScrollV(S16(h, 0x4E), scroll);
				S32(h, 0x70) = q0;
				const int32_t q1 = ScrollV(S16(h, 0x56), scroll);
				S32(h, 0x74) = q1;
				const int32_t q2 = ScrollV(S16(h, 0x5E), scroll);
				S32(h, 0x78) = q2;
				const int32_t q3 = ScrollV(S16(h, 0x66), scroll);
				S32(h, 0x7C) = q3;
				const uint16_t p0 = (uint16_t)q0 & 0xFF00, p1 = (uint16_t)q1 & 0xFF00, p2 = (uint16_t)q2 & 0xFF00, p3 = (uint16_t)q3 & 0xFF00;
				U16(h, 0x60) = p2;
				U16(h, 0x50) = p0;
				U16(h, 0x58) = p1;
				U16(h, 0x68) = p3;
				if (p0 != p1 || p0 != p2 || p0 != p3)
				{
					U16(h, 0x52) = RebaseUV(h, 0x52, q0, base);
					U16(h, 0x5A) = RebaseUV(h, 0x5A, q1, base);
					U16(h, 0x62) = RebaseUV(h, 0x62, q2, base);
					U16(h, 0x6A) = RebaseUV(h, 0x6A, q3, base);
				}
				if (z > 0) z--;
				pkt += 0x28;
				U32(pkt, 8) = U32(h, 0x6C) | 0x2E000000;
				U16(pkt, 0x10) = U16(h, 0x52);
				U16(pkt, 0x18) = U16(h, 0x5A);
				U16(pkt, 0x20) = U16(h, 0x62);
				U16(pkt, 0x28) = U16(h, 0x6A);
				U32(pkt, 0) = 0xB000000;
				U32(pkt, 0xC) = U32(xy0, 0);
				U32(pkt, 0x14) = U32(xy1, 0);
				U16(pkt, 0x12) = 0x3F54;
				U32(pkt, 0x1C) = U32(xy2, 0);
				U16(pkt, 0x1A) = 0xBA;
				U32(pkt, 0x24) = U32(xy3, 0);
				U32(pkt, 4) = U32(h, 0x88);
				U32(pkt, 0x2C) = U32(h, 0x8C);
				InsertPrim(ot + (uint32_t)z * 4, pkt);
				pkt += 0x30;
			}
		}
		return pkt;
	}

	static uint32_t RenderModel(uint32_t model, uint32_t h, uint32_t ot, uint32_t shift, uint32_t pkt)
	{
		const uint32_t bones = U32(model, 0) + 0x10;
		uint32_t table = U32(model, 4);
		const int32_t count = S32(table, 0);
		table += 4;
		for (int32_t i = 0; i < count; i++)
		{
			uint32_t obj = U32(model, 4) + U32(table, 0);
			table += 4;
			if (!(U32(h, 0x20) & (1u << (i & 31)))) continue;
			int32_t groups = S16(obj, 0);
			uint32_t out = U32(h, 4);
			obj += 2;
			for (; groups > 0; groups--)
			{
				const uint32_t bone = bones + (uint32_t)((int32_t)S16(obj, 0) * 0x30) + 0x10;
				obj += 2;
				GteSetRotMatrixCtrl((const Mat4x3 *)bone);
				GteSetTransVectorCtrl((const Mat4x3 *)bone);
				int32_t n = S16(obj, 0);
				obj += 2;
				for (; n > 0; n--)
				{
					GteLoadV0Words(obj);
					obj += 6;
					GteMVMVA_RotV0Tr2();
					GteStoreIRVertex(out);
					const int32_t q = ScrollV(S16(out, 2), S32(h, 0x30));
					U16(out, 6) = (uint16_t)(AT<uint8_t>(out, 0) | ((uint32_t)(uint8_t)q << 8));
					out += 8;
				}
			}
			const uint32_t a = (obj + 3) & 0xFFFFFFFC;
			U16(h, 8) = U16(a, 0);
			U16(h, 0xA) = U16(a, 2);
			U16(h, 0xC) = U16(a, 4);
			U16(h, 0xE) = U16(a, 6);
			U32(h, 0) = a + 0xC;
			GteSetRotMatrixCtrl(&Camera());
			GteSetTransVectorCtrl(&Camera());
			pkt = RenderObjectPrims(h, ot, shift, pkt);
		}
		return pkt;
	}

	static void DrawCasterModel(uint32_t h, uint8_t *entity)
	{
		const uint32_t e = (uint32_t)entity;
		const uint32_t ox = AT<uint8_t>(h, 0x34), oy = AT<uint8_t>(h, 0x36);
		const uint32_t mw = U16(h, 0x38), mh = U16(h, 0x3A);
		uint32_t win = ((oy & 0xF8) | 0xFFFE2000) << 5;
		win = (win | (ox & 0xF8)) << 5;
		win = (win | (~(mh - 1) & 0xF8)) << 2;
		win |= (uint32_t)((int32_t)~(mw - 1) >> 3) & 0x1F;
		U32(h, 0x88) = win;
		U32(h, 0x8C) = 0xE2000000;
		const uint32_t anim = e + 0x60;
		if (!(entity[0] & 0x20)) FrameCursor() = DrawShadow(entity, RenderOT(0x4064), 0x10, FrameCursor());
		const uint32_t world = e + 0x40;
		ComposeAffineTransform(&Camera(), (const Mat4x3 *)world, (Mat4x3 *)(h + 0x90));
		ComputeBonesWorldMatrices((void *)anim, (void *)world);
		U32(h, 0x1C) = U32(e, 0x28);
		U32(h, 0x20) = U32(e, 0x7C);
		AT<uint8_t>(h, 0x2A) = entity[7];
		AT<uint8_t>(h, 0x29) = entity[7];
		AT<uint8_t>(h, 0x28) = entity[7];
		U32(h, 4) = BoneWorkspace();
		U32(h, 0x10) = var<uint32_t>(game_addr(game::BATTLE_PACKET_BUFFER_END));
		U16(h, 0x14) = 0;
		U16(h, 0x16) = 0;
		U16(h, 0x18) = 0x140;
		U16(h, 0x1A) = 0xD8;
		FrameCursor() = RenderModel(U32(anim, 4), h, RenderOT(0x44), 2, FrameCursor());
		BuildBoneMatricesFromPose((void *)anim);
		const uint32_t second = U32(e, 0x78);
		if (second && !(entity[1] & 2))
		{
			U32(h, 0x20) = 0xFFFFFFFF;
			ComputeBonesWorldMatrices((void *)second, (void *)(h + 0x90));
			FrameCursor() = RenderGeometry(U32(second, 4), h, RenderOT(0x44), 4, FrameCursor());
			BuildBoneMatricesFromPose((void *)second);
		}
	}

	static int32_t GlowLevel(int16_t counter)
	{
		if (counter < 4) return shl32(4 - counter, 10);
		if (counter >= 0x16) return shl32(counter - 0x16, 9);
		return 0;
	}

	static void GlowColour(uint32_t h, int16_t counter, int32_t level)
	{
		GteSetFarColor(0, 0, 0);
		GteLoadRGBC(game_addr(GLOW_COLOURS) + (uint32_t)((int32_t)counter % 8) * 4);
		GteSetIR0(level);
		GteDPCS();
		GteStoreRGB2(h + 0x6C);
	}

	static void GlowBlock(uint32_t h, int32_t scroll)
	{
		U16(h, 0x34) = 0;
		U16(h, 0x36) = 0x80;
		U16(h, 0x3A) = 0x80;
		U16(h, 0x38) = 0x40;
		U32(h, 0x2C) = 0;
		S32(h, 0x30) = scroll;
	}
	static int32_t GlowScroll(int16_t counter) { return (int32_t)counter * 72; }

	static void DrawFuseParticle(uint8_t *h, const Particle *p)
	{
		*(int16_t *)(h + 4) = p->frame;
		*(int32_t *)(h + 0xC) = p->mirror[0];
		*(int32_t *)(h + 0x10) = p->mirror[1];
		TransformCameraByShadowRotation(p->pos, (uint16_t)p->scale, -((int32_t)p->scale >> 4));
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(0x44), 2, PacketCursor());
	}
	static uint8_t *SparkHeader()
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		*(uint32_t *)(h + 0) = game_addr(SEQ_Spark);
		*(int16_t *)(h + 0x24) = 2;
		*(int32_t *)(h + 0x14) = 0x1000;
		return h;
	}
}
}

namespace ff8fx
{
namespace bombglow179
{
	static uint32_t __cdecl CasterGlowTask(TaskNode *n)
	{
		EffectNode *t = (EffectNode *)n;
		if (t->counter < 0x1E)
		{
			const int32_t level = GlowLevel(t->counter);
			const uint32_t h = (uint32_t)FieldAlloc(0xB0);
			GlowColour(h, t->counter, level);
			GlowBlock(h, GlowScroll(t->counter));
			uint8_t *entity = Caster();
			entity[0] |= 4;
			DrawCasterModel(h, entity);
			FieldFree(0xB0);
		}
		if (DrawOnly()) return 0;
		t->counter++;
		if (t->counter <= 0x1E) return 0;
		uint16_t &flags = *(uint16_t *)Caster();
		flags = (uint16_t)((flags & 0xFFFB) | t->hidden);
		return TASK_END;
	}

	static uint32_t __cdecl FuseSparksTask(TaskNode *n)
	{
		EffectNode *t = (EffectNode *)n;
		uint8_t *h = SparkHeader();
		int alive = 0;
		for (Particle *p = Pool(); (uint32_t)p < game_addr(POOL_END); p++)
		{
			if (!(p->type & 1)) continue;
			DrawFuseParticle(h, p);
			if (DrawOnly()) continue;
			p->frame++;
			if (*(int16_t *)(h + 0x28) < 0) p->type = 0;
			else alive++;
		}
		FieldFree(0xB4);
		if (DrawOnly()) return 0;
		if (t->counter >= 0 && t->counter <= 0x16)
		{
			const int32_t bones = **(const uint8_t *const *)*(const uint32_t *)(Caster() + 0x64);
			for (int k = 0; k < 3; k++)
			{
				const int i = FreeParticle();
				if (i < 0) break;
				Particle *p = &Pool()[i];
				p->type = 1;
				p->frame = 0;
				const int32_t r = CrtRand() % 0x6A0;
				p->scale = (int16_t)(r + (int32_t)(uint16_t)(t->counter + 0x40) * 8);
				const int32_t flag = CrtRand() % 0x1000;
				const int32_t bone = CrtRand() % bones;
				GetEffectSpawnPosition(Caster(), bone, flag, p->pos);
				p->pos[0] = (int16_t)(p->pos[0] + (CrtRand() % 0x1B8 - 0xDC));
				p->pos[1] = (int16_t)(p->pos[1] + (CrtRand() % 0x1B8 - 0xDC));
				p->pos[2] = (int16_t)(p->pos[2] + (CrtRand() % 0x1B8 - 0xDC));
				p->mirror[0] = (p->pos[0] & 1) ? 0x1000 : -0x1000;
				p->mirror[1] = (p->pos[1] & 1) ? 0x1000 : -0x1000;
			}
		}
		t->counter++;
		if (t->counter >= 4 && alive == 0) return TASK_END;
		return 0;
	}

	static uint32_t __cdecl DirectorTask(TaskNode *n)
	{
		if (DrawOnly()) return 0;
		EffectNode *t = (EffectNode *)n;
		if (t->counter == 1)
		{
			EffectNode *g = (EffectNode *)AddTaskToQueue(EffectQueue(), game_addr(ORIG_CasterGlow));
			g->counter = 0;
			g->hidden = (uint16_t)(Caster()[0] & 4);
		}
		if (t->counter == 1)
		{
			EffectNode *f = (EffectNode *)AddTaskToQueue(EffectQueue(), game_addr(ORIG_FuseSparks));
			f->counter = 0;
		}
		if (t->counter == 0x23) ApplyActionResultToTarget(Ctx()->actions[0].targets);
		if (t->counter == 1)
		{
			int16_t pos[4];
			GetDefaultEffectPosition(Caster(), pos);
			BdPlaySE3D(SOUND_Glow(), 0, pos);
		}
		t->counter++;
		return t->counter > 0x28 ? TASK_END : 0;
	}

	static uint32_t __cdecl RootTask(TaskNode *n)
	{
		RootNode *r = (RootNode *)n;
		if (r->counter & 1) PacketCursor() = game_addr(ARENA1);
		else PacketCursor() = game_addr(ARENA0);
		const int a = ExecuteTaskQueue(EffectQueue());
		r->counter++;
		return a ? 0 : TASK_END;
	}
}

	void register_mag179_bomb_glow()
	{
		register_port(bombglow179::ORIG_RootTask, (void *)bombglow179::RootTask, "B179 RootTask", 179);
		register_port(bombglow179::ORIG_Director, (void *)bombglow179::DirectorTask, "B179 Director", 179);
		register_port(bombglow179::ORIG_CasterGlow, (void *)bombglow179::CasterGlowTask, "B179 CasterGlow", 179);
		register_port(bombglow179::ORIG_FuseSparks, (void *)bombglow179::FuseSparksTask, "B179 FuseSparks", 179);
	}
}

