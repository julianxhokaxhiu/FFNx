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
#include "fx_params.h"
#include "fx_model_sweep.h"

#include <windows.h>

namespace ff8fx
{
namespace abadon314
{
	using namespace eng;
	using namespace magc;
	using namespace msweep;

	inline uint32_t &TexFile() { return var<uint32_t>(game_addr(game::MAG_314_Abadon_TextureFile)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::MAG_314_Abadon_CastContext)); }
	inline uint8_t *&Caster() { return var<uint8_t *>(game_addr(game::MAG_314_Abadon_CasterEntity)); }
	inline TaskQueue *QScripts() { return (TaskQueue *)game_addr(game::MAG_314_Abadon_ScriptsQueue); }
	inline TaskQueue *QMain() { return (TaskQueue *)game_addr(game::MAG_314_Abadon_MainQueue); }
	inline TaskQueue *QRoot() { return (TaskQueue *)game_addr(game::MAG_314_Abadon_RootQueue); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_314_Abadon_PacketCursor)); }
	inline uint32_t &Lcg() { return var<uint32_t>(game_addr(game::MAG_314_Abadon_Lcg)); }
	inline uint32_t &Flash() { return var<uint32_t>(game_addr(game::MAG_314_Abadon_Flash)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::MAG_314_Abadon_MagicBufferBase)); }

	static constexpr game::Id ORIG_Setup = game::MAG_314_Abadon_Setup, ORIG_FileLoader = game::MAG_314_Abadon_FileLoader;
	static constexpr game::Id ORIG_RootTask = game::MAG_314_Abadon_RootTask;
	static constexpr game::Id ORIG_MainTask = game::MAG_314_Abadon_MainTask;
	static constexpr game::Id ORIG_ScriptTask = game::MAG_314_Abadon_BoneScriptTask;
	static constexpr game::Id TEXTURE_NAME = game::aMag313Tim;
	static constexpr game::Id CAMERA = game::MAG314_cameraAnim;
	static constexpr game::Id SEQUENCES = game::unk_10168C0;
	static constexpr game::Id COLOUR_Target = game::MAG_314_Abadon_ColourLerpTarget;
	static const void *SOUND_Voice() { return (const void *)game_addr(game::MAG_314_Abadon_SoundVoice); }
	static const void *SOUND_Start() { return (const void *)game_addr(game::MAG_314_Abadon_SoundStart); }
	static const uint32_t MAIN = 0xA80;
	static const uint32_t STREAM_STATE = 0x56A8, STREAM_DST = 0x56F0;
	static const uint32_t ARENA_A = 0x56F0, ARENA_B = 0x156F0;
	static const int16_t PAGE_MOVE = 0xD, CLUT_MOVE = 0x314;
	static constexpr game::Id SCRIPTS[29] = { game::g_Mag314_BoneScripts, game::unk_101AAB0, game::unk_101AA7C, game::unk_101AADC, game::unk_101AB08, game::unk_101AB34, game::unk_101AB64, game::unk_101AB94, game::unk_101ABC0, game::unk_101ABEC, game::unk_101AC18, game::unk_101AC38, game::unk_101AC64, game::unk_101AC90, game::unk_101ACB0, game::unk_101ACDC, game::unk_101AD08, game::unk_101AD28, game::unk_101AD58, game::unk_101AD88, game::unk_101ADB4, game::unk_101ADE0, game::unk_101AE0C, game::unk_101AE2C, game::unk_101AE58, game::unk_101AE84, game::unk_101AEA4, game::unk_101AED0, game::unk_101AEFC };

	inline uint32_t MagicTextureOff() { return fn<uint32_t (__cdecl *)()>(game_addr(game::Magic_TextureOFF_ToEAX1))(); }
	inline void StreamLoad(int32_t id, uint32_t dst, int32_t kind) { fn<void (__cdecl *)(int32_t, uint32_t, int32_t)>(game_addr(game::Stream_QueueLoad))(id, dst, kind); }
	inline void StreamPump() { fn<void (__cdecl *)()>(game_addr(game::Stream_Pump))(); }
	inline int32_t StreamPending(int32_t a) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::BattleFile_PreLoad_IsLoaded))(a); }
	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t flag, void *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, void *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, flag, out); }
	inline uint32_t FlashTile(int32_t t, int32_t r, int32_t g, int32_t b, int32_t in, int32_t out, uint32_t cursor) { return fn<uint32_t (__cdecl *)(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_FlashTileUpDown))(t, r, g, b, in, out, cursor); }
	inline void InsertPrimAltViewport(uint32_t bucket, void *packet) { fn<void (__cdecl *)(uint32_t, void *)>(game_addr(game::SSIGPU_InsertPrimAltViewport))(bucket, packet); }
	inline void Apply2DTransform(const void *a, const void *b, void *out) { fn<void (__cdecl *)(const void *, const void *, void *)>(game_addr(game::BattleModel_ScaleMatrixColumns4096))(a, b, out); }
	inline void Apply3DTransform(const void *m, const void *v, void *out) { fn<void (__cdecl *)(const void *, const void *, void *)>(game_addr(game::apply3DTransform))(m, v, out); }
	inline void Matrix3x3Multiply(const void *a, const void *b, void *out) { fn<void (__cdecl *)(const void *, const void *, void *)>(game_addr(game::Matrix3x3_MultiplyPSX))(a, b, out); }
	inline void GteLoadV1(const void *v) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadV1))(v); }
	inline void GteLoadV2(const void *v) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadV2))(v); }
	inline void GteMVMVA_RotV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0))(); }

#pragma pack(push, 1)
	struct RootNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t pad0E;
		uint8_t started;
		uint32_t parity;
	};
	struct PoseEntry
	{
		int16_t pos[3];
		int16_t vis;
		int16_t rot[3];
		int16_t pad;
	};
	struct Particle
	{
		int16_t pos[3];
		int16_t life;
		int16_t size;
		int16_t pad0A;
		int8_t vel[3];
		int8_t seq;
	};
	struct Shard
	{
		int16_t pos[3];
		int16_t kind;
		int16_t vel[3];
		int16_t life;
		uint8_t *spin;
		int16_t c[4][3];
		uint32_t uv0;
		uint32_t uv1;
		uint16_t uv2;
		uint16_t uv3;
	};
	struct MainNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		uint32_t voice;
		uint32_t colour[2];
		uint32_t tint1;
		PoseEntry pose[0x70];
		Particle parts[0x80];
		Particle *next_part;
		Shard shards[0x100];
		Shard *next_shard;
		uint8_t spins[0x40][0x14];
	};
	struct ScriptNode
	{
		TaskNode hdr;
		uint8_t *entity;
		PoseEntry *pose;
		PoseEntry *cur;
		const int16_t *ip;
		int16_t vel[3];
		int16_t pad22;
		int16_t avel[3];
		int16_t pad2A;
		int16_t ground;
		uint8_t bounced;
		uint8_t pad2F;
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x14 && sizeof(PoseEntry) == 0x10 && sizeof(Particle) == 0x10 && sizeof(Shard) == 0x38, "314 nodes");
	static_assert(sizeof(MainNode) == 0x4C28 && sizeof(ScriptNode) == 0x30, "314 nodes");
}
}

namespace ff8fx
{
namespace abadon314
{
	using namespace eng;
	using namespace magc;
	using namespace msweep;

	static int32_t Rand15()
	{
		Lcg() = (Lcg() * 125 + 0xE) & 0x7FFF;
		return (int32_t)Lcg();
	}

	static void PoseBones(uint8_t *anim, const PoseEntry *pose)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x60);
		uint8_t *skel = **(uint8_t ***)(anim + 4);
		const uint8_t count = skel[0];
		if (!(skel[1] & 1))
		{
			for (uint32_t i = 0; i < count; i++)
			{
				uint8_t *rec = skel + 0x10 + i * 0x30;
				const int16_t *pr = pose[i].rot;
				F<int16_t>(h, 0x58) = (int16_t)(F<int16_t>(rec, 4) + pr[0]);
				F<int16_t>(h, 0x5A) = (int16_t)(F<int16_t>(rec, 6) + pr[1]);
				F<int16_t>(h, 0x5C) = (int16_t)(F<int16_t>(rec, 8) + pr[2]);
				ComposeZYXRotationMatrix((const int16_t *)(h + 0x58), (Mat4x3 *)h);
				if (F<int16_t>(rec, 0) == -1)
				{
					const int32_t s = F<int16_t>(skel, 2);
					F<int32_t>(h, 0x48) = s;
					F<int32_t>(h, 0x44) = s;
					F<int32_t>(h, 0x40) = s;
					Scale3DMatrix((Mat4x3 *)h, (const int32_t *)(h + 0x40));
					ff8fx::copy<(0x20)>(rec + 0x10, h);
					F<int32_t>(rec, 0x24) = mul32(F<int16_t>(skel, 8), F<int16_t>(skel, 2)) >> 8;
					F<int32_t>(rec, 0x28) = mul32(F<int16_t>(skel, 0xA), F<int16_t>(skel, 2)) >> 8;
					F<int32_t>(rec, 0x2C) = mul32(F<int16_t>(skel, 0xC), F<int16_t>(skel, 2)) >> 8;
				}
				else
				{
					const uint8_t *prec = skel + 0x10 + mul32(F<int16_t>(rec, 0), 0x30);
					F<int32_t>(h, 0x14) = 0;
					F<int32_t>(h, 0x18) = 0;
					F<int32_t>(h, 0x1C) = F<int16_t>(prec, 2);
					ComposeAffineTransform((const Mat4x3 *)(prec + 0x10), (const Mat4x3 *)h, (Mat4x3 *)(rec + 0x10));
				}
			}
		}
		else if (count != 0)
		{
			static uint8_t rots[0x50][0x20];
			static int32_t scales[0x50][4];
			for (uint32_t i = 0; i < count; i++)
			{
				uint8_t *rec = skel + 0x10 + i * 0x30;
				const int16_t *pr = pose[i].rot;
				F<int16_t>(h, 0x58) = (int16_t)(F<int16_t>(rec, 4) + pr[0]);
				F<int16_t>(h, 0x5A) = (int16_t)(F<int16_t>(rec, 6) + pr[1]);
				F<int16_t>(h, 0x5C) = (int16_t)(F<int16_t>(rec, 8) + pr[2]);
				ComposeZYXRotationMatrix((const int16_t *)(h + 0x58), (Mat4x3 *)h);
				const int32_t parent = F<int16_t>(rec, 0);
				if (parent == -1)
				{
					const int32_t s = F<int16_t>(skel, 2);
					F<int32_t>(h, 0x48) = s;
					F<int32_t>(h, 0x44) = s;
					F<int32_t>(h, 0x40) = s;
					Apply2DTransform(h, h + 0x40, rots[i]);
					scales[i][0] = shl32(F<int16_t>(rec, 0xA), 2);
					scales[i][1] = shl32(F<int16_t>(rec, 0xC), 2);
					scales[i][2] = shl32(F<int16_t>(rec, 0xE), 2);
					Apply2DTransform(rots[i], scales[i], rec + 0x10);
					F<int32_t>(rec, 0x24) = mul32(F<int16_t>(skel, 8), F<int16_t>(skel, 2)) >> 8;
					F<int32_t>(rec, 0x28) = mul32(F<int16_t>(skel, 0xA), F<int16_t>(skel, 2)) >> 8;
					F<int32_t>(rec, 0x2C) = mul32(F<int16_t>(skel, 0xC), F<int16_t>(skel, 2)) >> 8;
				}
				else
				{
					const uint8_t *prec = skel + 0x10 + mul32(parent, 0x30);
					F<int16_t>(h, 0x52) = 0;
					F<int16_t>(h, 0x50) = 0;
					F<int16_t>(h, 0x54) = F<int16_t>(prec, 2);
					Apply3DTransform(prec + 0x10, h + 0x50, h + 0x40);
					F<int32_t>(rec, 0x24) = F<int32_t>(prec, 0x24) + F<int32_t>(h, 0x40);
					F<int32_t>(rec, 0x28) = F<int32_t>(prec, 0x28) + F<int32_t>(h, 0x44);
					F<int32_t>(rec, 0x2C) = F<int32_t>(prec, 0x2C) + F<int32_t>(h, 0x48);
					Matrix3x3Multiply(rots[parent], h, rots[i]);
					scales[i][0] = mul32(scales[parent][0], F<int16_t>(rec, 0xA)) >> 0xA;
					scales[i][1] = mul32(scales[parent][1], F<int16_t>(rec, 0xC)) >> 0xA;
					scales[i][2] = mul32(scales[parent][2], F<int16_t>(rec, 0xE)) >> 0xA;
					Apply2DTransform(rots[i], scales[i], rec + 0x10);
				}
			}
		}
		FieldFree(0x60);
	}

	static void ProjectCaster(const uint8_t **data, uint8_t **out, uint8_t *h, int32_t key)
	{
		const uint8_t *d = *data;
		uint8_t *o = *out;
		const int32_t n = *(const int16_t *)d;
		d += 2;
		for (int32_t k = n; k != 0; k--)
		{
			GteLoadV0u16(d);
			GteRTPS();
			GteReadSXY2(h + 0x20);
			d += 6;
			GteReadOTZ(h + 0x24);
			if (F<int16_t>(h, 0x24) <= 0) o[6] = 0x10;
			else
			{
				h[0x26] = 0;
				h[0x27] = (uint8_t)key;
				const int32_t x = F<int16_t>(h, 0x20);
				if (x < shl32(F<int16_t>(h, 0xC), 3)) h[0x26] = 1;
				else if (x >= shl32(F<int16_t>(h, 0x10), 3)) h[0x26] = 2;
				const int32_t y = F<int16_t>(h, 0x22);
				if (y < shl32(F<int16_t>(h, 0xE), 3)) h[0x26] |= 4;
				else if (y >= shl32(F<int16_t>(h, 0x12), 3)) h[0x26] |= 8;
				*(uint32_t *)o = F<uint32_t>(h, 0x20);
				*(uint32_t *)(o + 4) = F<uint32_t>(h, 0x24);
			}
			o += 8;
		}
		*data = d;
		*out = o;
	}

	static uint32_t CasterPolys(uint8_t *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		uint8_t *pk = (uint8_t *)cursor;
		const uint8_t *verts = F<const uint8_t *>(h, 4);
		const uint8_t *p = F<const uint8_t *>(h, 0);
		int16_t c = F<int16_t>(h, 8);
		F<int16_t>(h, 8) = (int16_t)(c - 1);
		while (c != 0)
		{
			if (!(p[0xF] & 0x80))
			{
				const uint32_t i0 = *(const uint16_t *)p & 0xFFF, i1 = *(const uint16_t *)(p + 2) & 0xFFF, i2 = *(const uint16_t *)(p + 4) & 0xFFF;
				F<uint32_t>(h, 0x20) = *(const uint32_t *)(verts + i0 * 8);
				F<uint32_t>(h, 0x24) = *(const uint32_t *)(verts + i0 * 8 + 4);
				F<uint32_t>(h, 0x28) = *(const uint32_t *)(verts + i1 * 8);
				F<uint32_t>(h, 0x2C) = *(const uint32_t *)(verts + i1 * 8 + 4);
				F<uint32_t>(h, 0x30) = *(const uint32_t *)(verts + i2 * 8);
				F<uint32_t>(h, 0x34) = *(const uint32_t *)(verts + i2 * 8 + 4);
				const uint8_t f0 = h[0x26], f1 = h[0x2E], f2 = h[0x36];
				if ((uint8_t)(f0 | f1 | f2) < 0x10 && !(f2 & (uint8_t)(f0 & f1)) && h[0x27] == h[0x2F] && h[0x27] == h[0x37])
				{
					GteSetSXY012(F<uint32_t>(h, 0x20), F<uint32_t>(h, 0x28), F<uint32_t>(h, 0x30));
					GteNCLIP();
					GteReadMAC0(h + 0x40);
					if (F<int32_t>(h, 0x40) >= 0)
					{
						GteSetSZ123(F<int16_t>(h, 0x24), F<int16_t>(h, 0x2C), F<int16_t>(h, 0x34));
						GteAVSZ3();
						F<uint32_t>(pk, 0) = 0x7000000;
						F<uint32_t>(pk, 4) = ((uint32_t)((*(const uint16_t *)(p + 0xE) & 0x200) | 0x2400) << 16) | F<uint32_t>(h, 0x14);
						F<uint32_t>(pk, 8) = F<uint32_t>(h, 0x20);
						F<uint32_t>(pk, 0x10) = F<uint32_t>(h, 0x28);
						F<uint32_t>(pk, 0x18) = F<uint32_t>(h, 0x30);
						F<uint32_t>(pk, 0xC) = *(const uint32_t *)(p + 8);
						F<uint32_t>(pk, 0x14) = *(const uint32_t *)(p + 0xC);
						const uint16_t page = (uint16_t)(F<uint16_t>(pk, 0x16) & 0xFDFF);
						F<uint16_t>(pk, 0x1C) = *(const uint16_t *)(p + 6);
						F<uint16_t>(pk, 0x16) = (uint16_t)(page + F<uint16_t>(h, 0x18));
						F<uint16_t>(pk, 0xE) = (uint16_t)(F<uint16_t>(pk, 0xE) + F<uint16_t>(h, 0x1A));
						GteReadOTZWord(h + 0x48);
						InsertPrimAutoDepth(ot + (uint32_t)(F<int32_t>(h, 0x48) >> (shift & 31)) * 4, pk);
						pk += 0x20;
					}
				}
			}
			p += 0x10;
			c = F<int16_t>(h, 8);
			F<int16_t>(h, 8) = (int16_t)(c - 1);
		}
		c = F<int16_t>(h, 0xA);
		F<int16_t>(h, 0xA) = (int16_t)(c - 1);
		while (c != 0)
		{
			if (!(p[0xF] & 0x80))
			{
				const uint32_t i0 = *(const uint16_t *)p & 0xFFF, i1 = *(const uint16_t *)(p + 2) & 0xFFF;
				const uint32_t i2 = *(const uint16_t *)(p + 4) & 0xFFF, i3 = *(const uint16_t *)(p + 6) & 0xFFF;
				F<uint32_t>(h, 0x20) = *(const uint32_t *)(verts + i0 * 8);
				F<uint32_t>(h, 0x24) = *(const uint32_t *)(verts + i0 * 8 + 4);
				F<uint32_t>(h, 0x28) = *(const uint32_t *)(verts + i1 * 8);
				F<uint32_t>(h, 0x2C) = *(const uint32_t *)(verts + i1 * 8 + 4);
				F<uint32_t>(h, 0x30) = *(const uint32_t *)(verts + i2 * 8);
				F<uint32_t>(h, 0x34) = *(const uint32_t *)(verts + i2 * 8 + 4);
				F<uint32_t>(h, 0x38) = *(const uint32_t *)(verts + i3 * 8);
				F<uint32_t>(h, 0x3C) = *(const uint32_t *)(verts + i3 * 8 + 4);
				const uint8_t f0 = h[0x26], f1 = h[0x2E], f2 = h[0x36], f3 = h[0x3E];
				if ((uint8_t)(f3 | f0 | f1 | f2) < 0x10 && !(f2 & (uint8_t)(f3 & f0 & f1)) && h[0x27] == h[0x2F] && h[0x27] == h[0x37] && h[0x27] == h[0x3F])
				{
					GteSetSXY012(F<uint32_t>(h, 0x20), F<uint32_t>(h, 0x28), F<uint32_t>(h, 0x30));
					GteNCLIP();
					GteReadMAC0(h + 0x40);
					if (F<int32_t>(h, 0x40) >= 0)
					{
						GteSetSZ0123(F<int16_t>(h, 0x24), F<int16_t>(h, 0x2C), F<int16_t>(h, 0x34), F<int16_t>(h, 0x3C));
						GteAVSZ4();
						F<uint32_t>(pk, 0) = 0x9000000;
						F<uint32_t>(pk, 4) = ((uint32_t)((*(const uint16_t *)(p + 0xE) & 0x200) | 0x2C00) << 16) | F<uint32_t>(h, 0x14);
						F<uint32_t>(pk, 8) = F<uint32_t>(h, 0x20);
						F<uint32_t>(pk, 0x10) = F<uint32_t>(h, 0x28);
						F<uint32_t>(pk, 0x18) = F<uint32_t>(h, 0x30);
						F<uint32_t>(pk, 0x20) = F<uint32_t>(h, 0x38);
						F<uint32_t>(pk, 0xC) = *(const uint32_t *)(p + 8);
						F<uint32_t>(pk, 0x14) = *(const uint32_t *)(p + 0xC);
						F<uint16_t>(pk, 0x1C) = *(const uint16_t *)(p + 0x10);
						const uint16_t page = (uint16_t)(F<uint16_t>(pk, 0x16) & 0xFDFF);
						F<uint16_t>(pk, 0x24) = *(const uint16_t *)(p + 0x12);
						F<uint16_t>(pk, 0x16) = (uint16_t)(page + F<uint16_t>(h, 0x18));
						F<uint16_t>(pk, 0xE) = (uint16_t)(F<uint16_t>(pk, 0xE) + F<uint16_t>(h, 0x1A));
						GteReadOTZWord(h + 0x48);
						InsertPrimAutoDepth(ot + (uint32_t)(F<int32_t>(h, 0x48) >> (shift & 31)) * 4, pk);
						pk += 0x28;
					}
				}
			}
			p += 0x14;
			c = F<int16_t>(h, 0xA);
			F<int16_t>(h, 0xA) = (int16_t)(c - 1);
		}
		return (uint32_t)pk;
	}

	static uint32_t CasterModel(const uint8_t *model, uint8_t *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		const uint32_t *objs = *(const uint32_t *const *)(model + 4);
		const uint8_t *bones = *(const uint8_t *const *)model + 0x10;
		const int32_t count = (int32_t)objs[0];
		if (count <= 0) return cursor;
		for (int32_t i = 0; i < count; i++)
		{
			const uint8_t *p = *(const uint8_t *const *)(model + 4) + objs[1 + i];
			if (!(F<uint32_t>(h, 0x1C) & (1u << (i & 31)))) continue;
			uint8_t *out = F<uint8_t *>(h, 4);
			const int32_t groups = *(const int16_t *)p;
			p += 2;
			if (groups > 0)
			{
				for (int32_t g = groups; g != 0; g--)
				{
					const int32_t bone = *(const int16_t *)p;
					p += 2;
					const PoseEntry *pe = F<const PoseEntry *>(h, 0x50) + bone;
					if (pe->vis < 2)
					{
						const Mat4x3 *bm = (const Mat4x3 *)(bones + mul32(bone, 0x30) + 0x10);
						GteSetRotMatrixCtrl(bm);
						GteSetTransVectorCtrl(bm);
						ProjectCaster(&p, &out, h, pe->vis == 1 ? (bone | 0x80) : 0);
					}
					else
					{
						const int32_t n = *(const int16_t *)p;
						p = p + n * 6 + 2;
						for (int32_t k = n; k != 0; k--)
						{
							out[6] = 0x10;
							out += 8;
						}
					}
				}
			}
			p = (const uint8_t *)(((uint32_t)p + 3) & ~3u);
			F<uint16_t>(h, 8) = *(const uint16_t *)p;
			F<uint16_t>(h, 0xA) = *(const uint16_t *)(p + 2);
			F<const uint8_t *>(h, 0) = p + 0xC;
			cursor = CasterPolys(h, ot, shift, cursor);
		}
		return cursor;
	}

	static void CasterDraw(uint8_t *ent, const PoseEntry *pose, int16_t page, int16_t clut)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x9C);
		uint8_t *anim = ent + 0x60;
		FrameArena() = DrawShadow(ent, OT(0x4040), 0x10, FrameArena());
		PoseBones(anim, pose);
		uint8_t *skel = **(uint8_t ***)(anim + 4);
		for (uint32_t i = 0; i < skel[0]; i++)
		{
			Mat4x3 *bm = (Mat4x3 *)(skel + 0x20 + i * 0x30);
			ComposeAffineTransform((const Mat4x3 *)(ent + 0x40), bm, bm);
			bm->t[0] = (int32_t)((uint32_t)bm->t[0] + (uint32_t)(int32_t)pose[i].pos[0]);
			bm->t[1] = (int32_t)((uint32_t)bm->t[1] + (uint32_t)(int32_t)pose[i].pos[1]);
			bm->t[2] = (int32_t)((uint32_t)bm->t[2] + (uint32_t)(int32_t)pose[i].pos[2]);
			ComposeAffineTransform(&Camera(), bm, bm);
		}
		F<int16_t>(h, 0x18) = page;
		F<int16_t>(h, 0x1A) = clut;
		F<const PoseEntry *>(h, 0x50) = pose;
		F<uint32_t>(h, 4) = var<uint32_t>(game_addr(game::g_BattleVertexWorkspace));
		F<int16_t>(h, 0xC) = 0;
		F<int16_t>(h, 0xE) = 0;
		F<int16_t>(h, 0x10) = 0x140;
		F<int16_t>(h, 0x12) = 0xD8;
		F<uint32_t>(h, 0x14) = F<uint32_t>(ent, 0x28);
		F<uint32_t>(h, 0x1C) = F<uint32_t>(ent, 0x7C);
		FrameArena() = CasterModel(F<const uint8_t *>(anim, 4), h, OT(0x44), 0, FrameArena());
		PoseBones(anim, pose);
		FieldFree(0x9C);
	}

	static Particle *AllocParticle(MainNode *m)
	{
		Particle *s = m->next_part;
		if (s->life != 0)
		{
			s = &m->parts[0];
			if (s->life != 0)
			{
				int k = 0x80;
				do
				{
					s++;
					if (--k == 0) return nullptr;
				} while (s->life != 0);
			}
		}
		m->next_part = s < &m->parts[0x7F] ? s + 1 : &m->parts[0];
		return s;
	}

	static Shard *AllocShard(MainNode *m)
	{
		Shard *s = m->next_shard;
		if (s->kind != 0)
		{
			s = &m->shards[0];
			if (s->kind != 0)
			{
				int k = 0x100;
				do
				{
					s++;
					if (--k == 0) return nullptr;
				} while (s->kind != 0);
			}
		}
		m->next_shard = s < &m->shards[0xFF] ? s + 1 : &m->shards[0];
		return s;
	}

	static void ShardParticle(MainNode *m, const Shard *s)
	{
		Particle *p = AllocParticle(m);
		if (!p) return;
		*(uint32_t *)&p->pos[0] = *(const uint32_t *)&s->pos[0];
		*(uint32_t *)&p->pos[2] = *(const uint32_t *)&s->pos[2];
		*(uint32_t *)&p->vel[0] = 0;
		p->life = 0xB;
		p->size = 0x1000;
		p->vel[0] = 0;
		p->vel[1] = (int8_t)-(int8_t)(Rand15() & 7);
		const uint8_t vz = (uint8_t)(0xC0 - (uint8_t)(Rand15() & 0x1F));
		p->seq = 1;
		p->vel[2] = (int8_t)vz;
	}

	static void ProjectShards(const uint8_t **data, uint8_t **out, uint8_t *h, int32_t key)
	{
		const uint8_t *d = *data;
		uint8_t *o = *out;
		const int32_t n = *(const int16_t *)d;
		d += 2;
		for (int32_t k = n; k != 0; k--)
		{
			GteLoadV0u16(d);
			GteMVMVA_RotV0Tr();
			d += 6;
			*(int16_t *)(o + 6) = (int16_t)key;
			o += 8;
			GteStoreIR123Minus8(o);
		}
		uint8_t *e = *out;
		for (int32_t k = n; k != 0; k--)
		{
			if (*(const int16_t *)(e + 2) <= F<int16_t>(h, 0x14)) *(int16_t *)(e + 6) = (int16_t)(key | 1);
			e += 8;
		}
		*data = d;
		*out = e;
	}

	static void ShardPolys(uint8_t *h)
	{
		uint8_t *p = F<uint8_t *>(h, 0);
		const uint8_t *verts = F<const uint8_t *>(h, 4);
		MainNode *m = F<MainNode *>(h, 0x1C);
		int16_t c = F<int16_t>(h, 8);
		F<int16_t>(h, 8) = (int16_t)(c - 1);
		while (c != 0)
		{
			if (!(p[0xF] & 0x80))
			{
				const uint32_t i0 = *(const uint16_t *)p & 0xFFF, i1 = *(const uint16_t *)(p + 2) & 0xFFF, i2 = *(const uint16_t *)(p + 4) & 0xFFF;
				F<uint32_t>(h, 0x24) = *(const uint32_t *)(verts + i0 * 8);
				F<uint32_t>(h, 0x28) = *(const uint32_t *)(verts + i0 * 8 + 4);
				F<uint32_t>(h, 0x2C) = *(const uint32_t *)(verts + i1 * 8);
				F<uint32_t>(h, 0x30) = *(const uint32_t *)(verts + i1 * 8 + 4);
				F<uint32_t>(h, 0x34) = *(const uint32_t *)(verts + i2 * 8);
				F<uint32_t>(h, 0x38) = *(const uint32_t *)(verts + i2 * 8 + 4);
				if ((h[0x32] & (uint8_t)(h[0x3A] & h[0x2A])) != 0 && F<int16_t>(h, 0x2A) == F<int16_t>(h, 0x32) && F<int16_t>(h, 0x2A) == F<int16_t>(h, 0x3A))
				{
					Shard *s = AllocShard(m);
					if (!s) return;
					const int16_t cx = (int16_t)(((int32_t)F<int16_t>(h, 0x24) + F<int16_t>(h, 0x2C) + F<int16_t>(h, 0x34)) / 3);
					s->pos[0] = cx;
					const int16_t cy = (int16_t)(((int32_t)F<int16_t>(h, 0x26) + F<int16_t>(h, 0x36) + F<int16_t>(h, 0x2E)) / 3);
					s->pos[1] = cy;
					const int16_t cz = (int16_t)(((int32_t)F<int16_t>(h, 0x28) + F<int16_t>(h, 0x38) + F<int16_t>(h, 0x30)) / 3);
					s->kind = 1;
					s->pos[2] = cz;
					for (int k = 0; k < 3; k++)
					{
						s->c[k][0] = (int16_t)(F<int16_t>(h, 0x24 + 8 * k) - cx);
						s->c[k][1] = (int16_t)(F<int16_t>(h, 0x26 + 8 * k) - cy);
						s->c[k][2] = (int16_t)(F<int16_t>(h, 0x28 + 8 * k) - cz);
					}
					const int32_t sp = F<int16_t>(h, 0x16);
					s->vel[0] = (int16_t)(((Rand15() & sp) - (sp >> 1)) >> 2);
					const int32_t r1 = Rand15() & sp;
					s->vel[2] = (int16_t)-(r1 + (int16_t)((int16_t)F<int16_t>(h, 0x16) >> 1));
					s->vel[1] = (int16_t)(-(Rand15() & (int32_t)F<int16_t>(h, 0x16)) >> 1);
					s->life = (int16_t)((Rand15() & 7) + 4);
					s->uv0 = *(const uint32_t *)(p + 8);
					s->uv1 = *(const uint32_t *)(p + 0xC);
					s->uv2 = *(const uint16_t *)(p + 6);
					*((uint16_t *)&s->uv1 + 1) = (uint16_t)(*((uint16_t *)&s->uv1 + 1) + F<uint16_t>(h, 0x18));
					*((uint16_t *)&s->uv0 + 1) = (uint16_t)(*((uint16_t *)&s->uv0 + 1) + F<uint16_t>(h, 0x1A));
					s->spin = F<uint8_t *>(h, 0x54) + (Rand15() & 0x3F) * 0x14;
					p[0xF] |= 0x80;
					if (Rand15() < 0x2000) ShardParticle(m, s);
				}
			}
			p += 0x10;
			c = F<int16_t>(h, 8);
			F<int16_t>(h, 8) = (int16_t)(c - 1);
		}
		c = F<int16_t>(h, 0xA);
		F<int16_t>(h, 0xA) = (int16_t)(c - 1);
		while (c != 0)
		{
			if (!(p[0xF] & 0x80))
			{
				const uint32_t i0 = *(const uint16_t *)p & 0xFFF, i1 = *(const uint16_t *)(p + 2) & 0xFFF;
				const uint32_t i2 = *(const uint16_t *)(p + 4) & 0xFFF, i3 = *(const uint16_t *)(p + 6) & 0xFFF;
				F<uint32_t>(h, 0x24) = *(const uint32_t *)(verts + i0 * 8);
				F<uint32_t>(h, 0x28) = *(const uint32_t *)(verts + i0 * 8 + 4);
				F<uint32_t>(h, 0x2C) = *(const uint32_t *)(verts + i1 * 8);
				F<uint32_t>(h, 0x30) = *(const uint32_t *)(verts + i1 * 8 + 4);
				F<uint32_t>(h, 0x34) = *(const uint32_t *)(verts + i2 * 8);
				F<uint32_t>(h, 0x38) = *(const uint32_t *)(verts + i2 * 8 + 4);
				F<uint32_t>(h, 0x3C) = *(const uint32_t *)(verts + i3 * 8);
				F<uint32_t>(h, 0x40) = *(const uint32_t *)(verts + i3 * 8 + 4);
				if ((h[0x32] & (uint8_t)(h[0x3A] & h[0x2A] & h[0x42])) != 0 && F<int16_t>(h, 0x2A) == F<int16_t>(h, 0x32)
					&& F<int16_t>(h, 0x2A) == F<int16_t>(h, 0x3A) && F<int16_t>(h, 0x2A) == F<int16_t>(h, 0x42))
				{
					Shard *s = AllocShard(m);
					if (!s) return;
					const int16_t cx = (int16_t)(((int32_t)F<int16_t>(h, 0x24) + F<int16_t>(h, 0x2C) + F<int16_t>(h, 0x34) + F<int16_t>(h, 0x3C)) / 4);
					s->pos[0] = cx;
					const int16_t cy = (int16_t)(((int32_t)F<int16_t>(h, 0x26) + F<int16_t>(h, 0x3E) + F<int16_t>(h, 0x36) + F<int16_t>(h, 0x2E)) / 4);
					s->pos[1] = cy;
					const int16_t cz = (int16_t)(((int32_t)F<int16_t>(h, 0x28) + F<int16_t>(h, 0x40) + F<int16_t>(h, 0x38) + F<int16_t>(h, 0x30)) / 4);
					s->kind = 2;
					s->pos[2] = cz;
					for (int k = 0; k < 4; k++)
					{
						s->c[k][0] = (int16_t)(F<int16_t>(h, 0x24 + 8 * k) - cx);
						s->c[k][1] = (int16_t)(F<int16_t>(h, 0x26 + 8 * k) - cy);
						s->c[k][2] = (int16_t)(F<int16_t>(h, 0x28 + 8 * k) - cz);
					}
					const int16_t sp = F<int16_t>(h, 0x16);
					s->vel[0] = (int16_t)((Rand15() & (uint16_t)sp) - (int16_t)(sp >> 1));
					s->vel[2] = (int16_t)-(int32_t)((Rand15() & (uint16_t)sp) + (int16_t)(sp >> 1));
					s->vel[1] = (int16_t)-(int16_t)(Rand15() & (uint16_t)F<int16_t>(h, 0x16));
					s->life = (int16_t)((Rand15() & 7) + 4);
					s->uv0 = *(const uint32_t *)(p + 8);
					s->uv1 = *(const uint32_t *)(p + 0xC);
					s->uv2 = *(const uint16_t *)(p + 0x10);
					s->uv3 = *(const uint16_t *)(p + 0x12);
					*((uint16_t *)&s->uv1 + 1) = (uint16_t)(*((uint16_t *)&s->uv1 + 1) + F<uint16_t>(h, 0x18));
					*((uint16_t *)&s->uv0 + 1) = (uint16_t)(*((uint16_t *)&s->uv0 + 1) + F<uint16_t>(h, 0x1A));
					s->spin = F<uint8_t *>(h, 0x54) + (Rand15() & 0x3F) * 0x14;
					p[0xF] |= 0x80;
					if (Rand15() < 0x2000) ShardParticle(m, s);
				}
			}
			p += 0x14;
			c = F<int16_t>(h, 0xA);
			F<int16_t>(h, 0xA) = (int16_t)(c - 1);
		}
	}

	static void ShardModel(const uint8_t *model, uint8_t *h)
	{
		const uint32_t *objs = *(const uint32_t *const *)(model + 4);
		const uint8_t *bones = *(const uint8_t *const *)model + 0x10;
		const int32_t count = (int32_t)objs[0];
		for (int32_t i = 0; i < count; i++)
		{
			const uint8_t *p = *(const uint8_t *const *)(model + 4) + objs[1 + i];
			if (!(F<uint32_t>(h, 0x10) & (1u << (i & 31)))) continue;
			uint8_t *out = F<uint8_t *>(h, 4);
			const int32_t groups = *(const int16_t *)p;
			p += 2;
			if (groups > 0)
			{
				for (int32_t g = groups; g != 0; g--)
				{
					const int32_t bone = *(const int16_t *)p;
					p += 2;
					const PoseEntry *pe = F<const PoseEntry *>(h, 0x20) + bone;
					if (pe->vis < 2)
					{
						const Mat4x3 *bm = (const Mat4x3 *)(bones + mul32(bone, 0x30) + 0x10);
						GteSetRotMatrixCtrl(bm);
						GteSetTransVectorCtrl(bm);
						ProjectShards(&p, &out, h, pe->vis == 1 ? shl32(bone, 8) : 0);
					}
					else
					{
						const int32_t n = *(const int16_t *)p;
						p = p + n * 6 + 2;
						for (int32_t k = n; k != 0; k--)
						{
							*(int16_t *)(out + 6) = 0;
							out += 8;
						}
					}
				}
			}
			p = (const uint8_t *)(((uint32_t)p + 3) & ~3u);
			F<uint16_t>(h, 8) = *(const uint16_t *)p;
			F<uint16_t>(h, 0xA) = *(const uint16_t *)(p + 2);
			F<const uint8_t *>(h, 0) = p + 0xC;
			ShardPolys(h);
		}
	}

	static void ShardSweep(uint8_t *ent, const PoseEntry *pose, uint8_t *spins, int16_t level, int16_t spread, int16_t page, int16_t clut)
	{
		uint8_t *anim = ent + 0x60;
		PoseBones(anim, pose);
		uint8_t *skel = **(uint8_t ***)(anim + 4);
		for (uint32_t i = 0; i < skel[0]; i++)
		{
			Mat4x3 *bm = (Mat4x3 *)(skel + 0x20 + i * 0x30);
			ComposeAffineTransform((const Mat4x3 *)(ent + 0x40), bm, bm);
			bm->t[0] = (int32_t)((uint32_t)bm->t[0] + (uint32_t)(int32_t)pose[i].pos[0]);
			bm->t[1] = (int32_t)((uint32_t)bm->t[1] + (uint32_t)(int32_t)pose[i].pos[1]);
			bm->t[2] = (int32_t)((uint32_t)bm->t[2] + (uint32_t)(int32_t)pose[i].pos[2]);
		}
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		F<int16_t>(h, 0x16) = spread;
		F<uint32_t>(h, 0x1C) = TexBase() + MAIN;
		F<int16_t>(h, 0x14) = level;
		F<uint8_t *>(h, 0x54) = spins;
		F<const PoseEntry *>(h, 0x20) = pose;
		F<int16_t>(h, 0x18) = page;
		F<int16_t>(h, 0x1A) = clut;
		F<uint32_t>(h, 4) = var<uint32_t>(game_addr(game::g_BattleVertexWorkspace));
		F<uint32_t>(h, 0xC) = F<uint32_t>(ent, 0x28);
		F<uint32_t>(h, 0x10) = F<uint32_t>(ent, 0x7C);
		ShardModel(F<const uint8_t *>(anim, 4), h);
		FieldFree(0x58);
	}

	static void ShardsDraw(MainNode *m)
	{
		GteSetRotMatrix(&Camera());
		GteSetTransVector(&Camera());
		int16_t *v = (int16_t *)FieldAlloc(8);
		for (int k = 0; k < 0x100; k++)
		{
			const Shard *s = &m->shards[k];
			uint8_t *pk = (uint8_t *)PacketCursor();
			if (s->kind == 0) continue;
			for (int a = 0; a < 3; a++) v[a] = (int16_t)(s->c[0][a] + s->pos[a]);
			GteLoadV0(v);
			for (int a = 0; a < 3; a++) v[a] = (int16_t)(s->c[1][a] + s->pos[a]);
			GteLoadV1(v);
			for (int a = 0; a < 3; a++) v[a] = (int16_t)(s->c[2][a] + s->pos[a]);
			GteLoadV2(v);
			GteRTPT();
			F<uint32_t>(pk, 0xC) = s->uv0;
			F<uint32_t>(pk, 0x14) = s->uv1;
			F<uint16_t>(pk, 0x1C) = s->uv2;
			GteReadSXY012Split(pk + 8, pk + 0x10, pk + 0x18);
			uint32_t off = 0;
			if (F<int16_t>(pk, 8) < 0 || F<int16_t>(pk, 8) > 0xA00) off = 1;
			if (F<int16_t>(pk, 0x10) < 0 || F<int16_t>(pk, 0x10) > 0xA00) off |= 2;
			if (F<int16_t>(pk, 0x18) < 0 || F<int16_t>(pk, 0x18) > 0xA00) off |= 4;
			if (F<int16_t>(pk, 0xA) < 0 || F<int16_t>(pk, 0xA) > 0x6C0) off |= 0x10;
			if (F<int16_t>(pk, 0x12) < 0 || F<int16_t>(pk, 0x12) > 0x6C0) off |= 0x20;
			if (F<int16_t>(pk, 0x1A) < 0 || F<int16_t>(pk, 0x1A) > 0x6C0) off |= 0x40;
			uint32_t size;
			if (s->kind == 2)
			{
				for (int a = 0; a < 3; a++) v[a] = (int16_t)(s->c[3][a] + s->pos[a]);
				GteLoadV0(v);
				GteRTPS();
				F<uint16_t>(pk, 0x24) = s->uv3;
				GteReadSXY2(pk + 0x20);
				GteAVSZ4();
				if (F<int16_t>(pk, 0x20) < 0 || F<int16_t>(pk, 0x20) > 0xA00) off |= 8;
				if (F<int16_t>(pk, 0x22) < 0 || F<int16_t>(pk, 0x22) > 0x6C0) off |= 0x80;
				if ((off & 0xF) == 0xF || (off & 0xF0) == 0xF0) continue;
				GteReadOTZWord(v);
				if (v[0] <= 0) continue;
				F<uint32_t>(pk, 0) = 0x9000000;
				F<uint32_t>(pk, 4) = 0x2CF0F0F0;
				size = 0x28;
			}
			else
			{
				GteAVSZ3();
				if ((off & 7) == 7 || (off & 0x70) == 0x70) continue;
				F<uint32_t>(pk, 0) = 0x7000000;
				F<uint32_t>(pk, 4) = 0x24F0F0F0;
				GteReadOTZWord(v);
				if (v[0] <= 0) continue;
				size = 0x20;
			}
			PacketCursor() += size;
			InsertPrimAutoDepth(var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + (uint32_t)(v[0] >> 2) * 4, pk);
		}
		FieldFree(8);
	}

	static void ShardsUpdate(MainNode *m)
	{
		for (int k = 0; k < 0x100; k++)
		{
			Shard *s = &m->shards[k];
			if (s->kind == 0) continue;
			s->life--;
			if (s->life != 0)
			{
				GteSetRotMatrixCtrl((const Mat4x3 *)s->spin);
				GteLoadV0(s->c[0]);
				GteMVMVA_RotV0();
				GteStoreIR123(s->c[0]);
				GteLoadV0u16(s->c[1]);
				GteMVMVA_RotV0();
				if (*(const uint32_t *)&s->c[0][0] != 0 || s->c[0][2] != 0)
				{
					GteStoreIR123(s->c[1]);
					GteLoadV0(s->c[2]);
					GteMVMVA_RotV0();
					GteStoreIR123(s->c[2]);
					GteLoadV0u16(s->c[3]);
					GteMVMVA_RotV0();
					GteStoreIR123(s->c[3]);
					s->pos[0] = (int16_t)(s->pos[0] + s->vel[0]);
					s->pos[1] = (int16_t)(s->pos[1] + s->vel[1]);
					s->pos[2] = (int16_t)(s->pos[2] + s->vel[2]);
					continue;
				}
			}
			s->kind = 0;
			m->next_shard = s;
		}
	}

	static void ParticleDraw(uint8_t *h, Particle *p)
	{
		GteSetRotMatrixCtrl(&Camera());
		GteSetTransVectorCtrl(&Camera());
		GteLoadV0(p);
		GteMVMVA_RotV0Tr();
		GteReadMAC123((int32_t *)(h + 0x98));
		const int16_t size = p->size;
		F<int16_t>(h, 0x94) = size;
		F<int16_t>(h, 0x8C) = size;
		F<int16_t>(h, 0x84) = size;
		const uint32_t seq = var<uint32_t>(game_addr(SEQUENCES) + (int32_t)p->seq * 4);
		F<uint32_t>(h, 0) = seq;
		const uint32_t idx = (uint32_t)var<uint16_t>(seq + 8) - (uint32_t)(int32_t)p->life;
		const uint32_t rec = seq + var<uint16_t>(seq + idx * 2 + 8);
		F<uint32_t>(h, 0x2C) = rec;
		F<uint32_t>(h, 0x30) = var<uint32_t>(rec);
		F<uint32_t>(h, 0x2C) = rec + 4;
		PacketCursor() = DrawSprites(h, OT(0x44), 2, PacketCursor(), true);
		p->life--;
		p->pos[0] = (int16_t)(p->pos[0] + (int16_t)(p->vel[0] << 2));
		p->pos[1] = (int16_t)(p->pos[1] + (int16_t)(p->vel[1] << 2));
		p->pos[2] = (int16_t)(p->pos[2] + (int16_t)(p->vel[2] << 2));
	}

	static void Particles(MainNode *m)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		F<int16_t>(h, 0x92) = 0;
		F<int16_t>(h, 0x90) = 0;
		F<int16_t>(h, 0x8E) = 0;
		F<int16_t>(h, 0x8A) = 0;
		F<int16_t>(h, 0x88) = 0;
		F<int16_t>(h, 0x86) = 0;
		F<int16_t>(h, 0x60) = 0;
		F<int16_t>(h, 0x58) = 0;
		F<int16_t>(h, 0x50) = 0;
		F<int16_t>(h, 0x48) = 0;
		for (int k = 0; k < 0x80; k++)
			if (m->parts[k].life != 0) ParticleDraw(h, &m->parts[k]);
		FieldFree(0xB4);
	}

	static const int16_t *ScriptQuad(const int16_t *ip)
	{
		uint8_t *q = (uint8_t *)PacketCursor();
		PacketCursor() += 0x18;
		F<int16_t>(q, 8) = 0;
		F<int16_t>(q, 0x10) = 0;
		F<int16_t>(q, 0xE) = 0;
		F<int16_t>(q, 0xA) = 0;
		F<int16_t>(q, 0x14) = 0x140;
		F<int16_t>(q, 0xC) = 0x140;
		F<int16_t>(q, 0x16) = 0xD8;
		F<int16_t>(q, 0x12) = 0xD8;
		q[4] = *(const uint8_t *)ip;
		q[5] = *(const uint8_t *)(ip + 1);
		q[6] = *(const uint8_t *)(ip + 2);
		F<uint32_t>(q, 0) = 0x5000000;
		q[7] = 0x2A;
		InsertPrimAltViewport(var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x20, q);
		return ip + 3;
	}

	static uint32_t __cdecl ScriptTask(TaskNode *n)
	{
		ScriptNode *s = (ScriptNode *)n;
		PoseEntry *e = s->cur;
		const int16_t *ip = s->ip;
		for (;;)
		{
			const int32_t op = *ip++;
			switch (op)
			{
			case 0x1000: e->pos[0] = ip[0]; e->pos[1] = ip[1]; e->pos[2] = ip[2]; ip += 3; continue;
			case 0x1001:
				e->pos[0] = (int16_t)(e->pos[0] + ip[0]); e->pos[1] = (int16_t)(e->pos[1] + ip[1]); e->pos[2] = (int16_t)(e->pos[2] + ip[2]);
				ip += 3; continue;
			case 0x1002: s->vel[0] = ip[0]; s->vel[1] = ip[1]; s->vel[2] = ip[2]; ip += 3; continue;
			case 0x1003:
				s->vel[0] = (int16_t)(s->vel[0] + ip[0]); s->vel[1] = (int16_t)(s->vel[1] + ip[1]); s->vel[2] = (int16_t)(s->vel[2] + ip[2]);
				ip += 3; continue;
			case 0x1004:
			{
				e->pos[0] = (int16_t)(e->pos[0] + s->vel[0]);
				e->pos[1] = (int16_t)(e->pos[1] + s->vel[1]);
				e->pos[2] = (int16_t)(e->pos[2] + s->vel[2]);
				const int32_t bone = (int32_t)((uint8_t *)s->cur - (uint8_t *)s->pose) >> 4;
				int32_t w[3];
				TransformVectorBy3x3Matrix((const Mat4x3 *)(s->entity + 0x40), (const int32_t *)(**(uint8_t ***)(s->entity + 0x64) + mul32(bone, 0x30) + 0x34), w);
				const int32_t y = w[1] + e->pos[1];
				if (y <= s->ground) continue;
				s->vel[1] = (int16_t)((int16_t)(s->vel[1] >> 2) - s->vel[1]);
				if (s->bounced) continue;
				s->bounced = 1;
				MainNode *m = (MainNode *)(TexBase() + MAIN);
				Particle *p = AllocParticle(m);
				for (int left = 0x10; p; )
				{
					GetEffectSpawnPosition(Caster(), (int32_t)((uint8_t *)s->cur - (uint8_t *)s->pose) >> 4, CrtRand() & 0xFFF, p);
					p->pos[0] = (int16_t)(p->pos[0] + (int16_t)((CrtRand() & 0x7FF) + e->pos[0] - 0x400));
					p->pos[1] = (int16_t)(p->pos[1] + e->pos[1]);
					const int16_t dz = (int16_t)((CrtRand() & 0x7FF) + e->pos[2]);
					p->life = 0xC;
					p->pos[2] = (int16_t)(p->pos[2] + (int16_t)(dz - 0x400));
					p->size = (int16_t)((CrtRand() & 0x7FF) + 0xC00);
					p->vel[0] = (int8_t)((CrtRand() & 7) - 4);
					p->vel[2] = (int8_t)((CrtRand() & 7) - 4);
					const uint8_t vy = (uint8_t)(0xF8 - (uint8_t)(CrtRand() & 7));
					p->vel[1] = (int8_t)vy;
					p->seq = 0;
					if (--left == 0) break;
					p = AllocParticle(m);
				}
				continue;
			}
			case 0x1008: s->ground = *ip++; continue;
			case 0x100A: e->vis = 1; continue;
			case 0x1010: e->rot[0] = ip[0]; e->rot[1] = ip[1]; e->rot[2] = ip[2]; ip += 3; continue;
			case 0x1011:
				e->rot[0] = (int16_t)(e->rot[0] + ip[0]); e->rot[1] = (int16_t)(e->rot[1] + ip[1]); e->rot[2] = (int16_t)(e->rot[2] + ip[2]);
				ip += 3; continue;
			case 0x1012: s->avel[0] = ip[0]; s->avel[1] = ip[1]; s->avel[2] = ip[2]; ip += 3; continue;
			case 0x1013:
				s->avel[0] = (int16_t)(s->avel[0] + ip[0]); s->avel[1] = (int16_t)(s->avel[1] + ip[1]); s->avel[2] = (int16_t)(s->avel[2] + ip[2]);
				ip += 3; continue;
			case 0x1014:
				e->rot[0] = (int16_t)(e->rot[0] + s->avel[0]); e->rot[1] = (int16_t)(e->rot[1] + s->avel[1]); e->rot[2] = (int16_t)(e->rot[2] + s->avel[2]);
				continue;
			case 0x10B0: ip = ScriptQuad(ip); continue;
			case 0x10F0:
			{
				int16_t *loop = (int16_t *)ip;
				if (loop[0] != 0)
				{
					loop[0] = (int16_t)(loop[0] - 1);
					ip = (const int16_t *)((const uint8_t *)(ip + 1) + mul32(ip[2], 2) - 4);
				}
				else
				{
					loop[0] = loop[1];
					ip += 3;
				}
				continue;
			}
			case 0x1100: return TASK_END;
			default:
				s->ip = ip;
				return 0;
			}
		}
	}

	static void StartScript(uint8_t *ent, PoseEntry *pose, const int16_t *script)
	{
		ScriptNode *s = (ScriptNode *)AddTaskToQueue(QScripts(), game_addr(ORIG_ScriptTask));
		Memset32(&s->cur, 0, 9);
		s->entity = ent;
		s->pose = pose;
		s->cur = pose + script[0];
		s->ip = script + 1;
	}

	static int16_t LevelA(int32_t c) { return (int16_t)(0x200 - (int32_t)((uint32_t)(c - 0x10) * 4200u / 50u)); }
	static int16_t LevelShards(int32_t c) { return (int16_t)(mul32(3 * (c - 0x64) - 0x14A, 1000) / 110 + 0x400); }

	static uint32_t __cdecl MainTask(TaskNode *n)
	{
		MainNode *m = (MainNode *)n;
		const int16_t c = m->counter;
		if (c >= 0xE2)
		{
			*(uint16_t *)Caster() &= 0xFFF7;
			*(uint16_t *)(Caster() + 0x9C) &= 0xFFF7;
			*(uint32_t *)(Caster() + 0x2C) = m->colour[0];
			*(uint32_t *)(Caster() + 0xC8) = m->colour[1];
			ShowAllPolys(Caster());
			ShowAllPolys(Caster() + 0x9C);
			*(uint16_t *)Entity(0) &= 0xFFFB;
			*(uint16_t *)Entity(1) &= 0xFFFB;
			*(uint16_t *)Entity(2) &= 0xFFFB;
			return TASK_END;
		}
		if (c < 8) Flash() = (uint32_t)(mul32(c, 1000) >> 3);
		else if (c > 0xDA) Flash() = (uint32_t)(mul32(0xE2 - c, 1000) >> 3);
		else Flash() = 0x3E8;
		if ((uint32_t)(m->counter - 0x10) < 0x32)
		{
			SweepA(Caster(), LevelA(m->counter), PAGE_MOVE, CLUT_MOVE, PacketCursor());
			*Caster() |= 4;
		}
		const uint32_t t = (uint32_t)(m->counter - 0x51);
		if (t < 0x10)
		{
			const int32_t w = (int32_t)((t & 0xFFFFF) << 8);
			for (int k = 0; k < 2; k++)
			{
				ColourLerp(&m->colour[k], game_addr(COLOUR_Target), 0x1000 - w, w, Caster() + k * 0x9C + 0x2C);
				Caster()[k * 0x9C + 0x2F] = 0x32;
			}
		}
		if (m->counter == 0x42)
			for (int k = 0; k < 29; k++) StartScript(Caster(), m->pose, (const int16_t *)game_addr(SCRIPTS[k]));
		if (m->counter >= 0x42)
		{
			CasterDraw(Caster(), m->pose, PAGE_MOVE, CLUT_MOVE);
			*Caster() |= 4;
		}
		if ((uint32_t)m->counter < 5) PacketCursor() = FlashTile(m->counter, 0xFF, 0x80, 0x80, 1, 4, PacketCursor());
		if ((uint32_t)(m->counter - 0xF) < 5) PacketCursor() = FlashTile(m->counter - 0xF, 0xFF, 0x80, 0x80, 1, 4, PacketCursor());
		if ((uint32_t)(m->counter - 0x13) < 5) PacketCursor() = FlashTile(m->counter - 0x13, 0xFF, 0x80, 0x80, 1, 4, PacketCursor());
		Particles(m);
		ShardsDraw(m);
		if (m->counter == 1) BdPlaySE(SOUND_Start(), 0, 0x80);
		else if (m->counter == 0xDE) ReleaseVoice(m->voice);
		ShardsUpdate(m);
		if ((uint32_t)m->counter <= 8)
		{
			for (int k = 0; k < 2; k++)
			{
				if (m->counter & 1) *(uint16_t *)(Caster() + k * 0x9C) &= 0xFFF7;
				else Caster()[k * 0x9C] |= 8;
			}
		}
		const int32_t c2 = m->counter;
		if ((uint32_t)c2 <= 0x10)
		{
			Particle *p = AllocParticle(m);
			for (int left = 8; p; )
			{
				uint8_t *ent = Caster();
				const int32_t bone = CrtRand() % (int32_t)(uint32_t)***(uint8_t ***)(ent + 0x64);
				GetEffectSpawnPosition(ent, bone, 0, p);
				p->life = 0xF;
				const int32_t r = CrtRand();
				*(uint32_t *)&p->vel[0] = 0;
				p->size = (int16_t)((r & 0x7FF) + 0x200);
				p->seq = (int8_t)((CrtRand() & 1) + 2);
				if (--left == 0) break;
				p = AllocParticle(m);
			}
			const int32_t w = (int32_t)(((uint32_t)c2 << 12) >> 4) + 0x80;
			ColourLerp(&m->tint1, game_addr(COLOUR_Target), 0x1000 - w, w, Caster() + 0xC4);
			Caster()[0xC7] = 2;
			Caster()[0x9D] |= 8;
		}
		if (c2 == 0x10) Caster()[0x9C] |= 4;
		if ((uint32_t)(m->counter - 0x64) <= 0x6E)
			ShardSweep(Caster(), m->pose, &m->spins[0][0], LevelShards(m->counter), 0x7F, PAGE_MOVE, CLUT_MOVE);
		m->counter++;
		return 0;
	}

	static uint32_t __cdecl RootTask(TaskNode *n)
	{
		RootNode *r = (RootNode *)n;
		if (r->parity != 0)
		{
			PacketCursor() = TexBase() + ARENA_A;
			r->parity = 0;
		}
		else
		{
			PacketCursor() = TexBase() + ARENA_B;
			r->parity = 1;
		}
		Flash() = 0;
		if (r->counter == 1 && !r->started)
		{
			r->started = 1;
			MusicVolumeFade(0xF0);
			InitTaskQueuePool(QMain(), (void *)(TexBase() + MAIN), 0x4C28, 1);
			InitTaskQueuePool(QScripts(), (void *)TexBase(), 0x30, 0x38);
			MainNode *m = (MainNode *)AddTaskToQueue(QMain(), game_addr(ORIG_MainTask));
			Memset32(&m->counter, 0, 0x1307);
			m->voice = ClaimVoiceSlot(SOUND_Voice(), 1, 0x80);
			m->colour[0] = *(const uint32_t *)(Caster() + 0x2C);
			m->colour[1] = *(const uint32_t *)(Caster() + 0xC8);
			m->tint1 = *(const uint32_t *)(Caster() + 0xC4);
			m->next_part = &m->parts[0];
			m->next_shard = &m->shards[0];
			StreamStateInit((void *)(TexBase() + STREAM_STATE));
			for (int k = 0; k < 0x40; k++)
			{
				int16_t ang[4];
				ang[0] = (int16_t)((CrtRand() & 0x1FF) - 0x100);
				ang[1] = (int16_t)((CrtRand() & 0x1FF) - 0x100);
				ang[2] = (int16_t)((CrtRand() & 0x1FF) - 0x100);
				BuildRotationMatrixFromAngles(ang, (Mat4x3 *)m->spins[k]);
				const int32_t s = 0xF00 - (CrtRand() & 0x1FF);
				const int32_t sc[3] = { s, s, s };
				Scale3DMatrix((Mat4x3 *)m->spins[k], sc);
			}
		}
		if (r->started && r->counter < 0xE4)
		{
			ExecuteTaskQueue(QMain());
			PoseBones(Caster() + 0x60, (const PoseEntry *)(TexBase() + MAIN + 0x20));
			ExecuteTaskQueue(QScripts());
			BuildBoneMatricesFromPose(Caster() + 0x60);
		}
		SetScreenFlash(Flash(), 0);
		if (r->counter == 0xE4)
		{
			StreamLoad(0x168, TexBase() + STREAM_DST, 1);
			StreamLoad(0x169, TexBase() + STREAM_DST, 0);
		}
		if (r->started) StreamPump();
		if (r->counter > 0xE4) return StreamPending(1) ? TASK_END : 0;
		r->counter++;
		return 0;
	}

	// For the verifier: the shard sweep marks broken polygons (bit 0x80 of byte 0xF) inside the caster's model,
	// so the model's polygon block is effect state. Same walk as ShardModel, every object.
	void ModelPolyRegions(void (*add)(uint32_t addr, uint32_t size, const char *name))
	{
		uint8_t *ent = Caster();
		if (!ent || IsBadReadPtr(ent + 0x64, 4)) return;
		const uint8_t *model = *(const uint8_t *const *)(ent + 0x64);
		if (!model || IsBadReadPtr(model + 4, 4)) return;
		const uint8_t *base = *(const uint8_t *const *)(model + 4);
		if (!base || IsBadReadPtr(base, 4)) return;
		const uint32_t count = *(const uint32_t *)base;
		if (count == 0 || count > 32 || IsBadReadPtr(base, 4 + count * 4)) return;
		uint32_t end = 0;
		for (uint32_t i = 0; i < count; i++)
		{
			const uint8_t *p = base + ((const uint32_t *)base)[1 + i];
			if (IsBadReadPtr(p, 2)) return;
			const int32_t groups = *(const int16_t *)p;
			p += 2;
			for (int32_t g = 0; g < groups; g++)
			{
				if (IsBadReadPtr(p, 4)) return;
				p += 2;
				p += *(const int16_t *)p * 6 + 2;
			}
			p = (const uint8_t *)(((uint32_t)p + 3) & ~3u);
			if (IsBadReadPtr(p, 4)) return;
			const uint32_t e = (uint32_t)(p + 0xC) + *(const uint16_t *)p * 0x10u + *(const uint16_t *)(p + 2) * 0x14u - (uint32_t)base;
			if (e > end) end = e;
		}
		if (end && end <= 0x80000 && !IsBadReadPtr(base, end)) add((uint32_t)base, end, "Abadon model polygons");
	}
}

	void register_mag314_abadon()
	{
		register_port(abadon314::ORIG_RootTask, (void *)abadon314::RootTask, "314 RootTask", 314);
		register_port(abadon314::ORIG_MainTask, (void *)abadon314::MainTask, "314 MainTask", 314);
		register_port(abadon314::ORIG_ScriptTask, (void *)abadon314::ScriptTask, "314 ScriptTask", 314);
	}
}

