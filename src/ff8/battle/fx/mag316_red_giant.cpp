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

namespace ff8fx
{
namespace redgiant316
{
	using namespace eng;
	using namespace magc;
	using namespace msweep;

	inline uint32_t &TexFile() { return var<uint32_t>(game_addr(game::MAG_316_RedGiant_TextureFile)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::MAG_316_RedGiant_CastContext)); }
	inline uint8_t *&Caster() { return var<uint8_t *>(game_addr(game::MAG_316_RedGiant_CasterEntity)); }
	inline TaskQueue *QMain() { return (TaskQueue *)game_addr(game::MAG_316_RedGiant_MainQueue); }
	inline TaskQueue *QRoot() { return (TaskQueue *)game_addr(game::MAG_316_RedGiant_RootQueue); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_316_RedGiant_PacketCursor)); }
	inline uint32_t &Lcg() { return var<uint32_t>(game_addr(game::MAG_316_RedGiant_Lcg)); }
	inline uint32_t &Flash() { return var<uint32_t>(game_addr(game::MAG_316_RedGiant_Flash)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::MAG_316_RedGiant_MagicBufferBase)); }

	static constexpr game::Id ORIG_Setup = game::MAG_316_RedGiant_Setup, ORIG_FileLoader = game::MAG_316_RedGiant_FileLoader;
	static constexpr game::Id ORIG_RootTask = game::MAG_316_RedGiant_RootTask;
	static constexpr game::Id ORIG_MainTask = game::MAG_316_RedGiant_MainTask;
	static constexpr game::Id TEXTURE_NAME = game::aMag315Tim;
	static constexpr game::Id TIM_Exe = game::unk_1005E64;
	static constexpr game::Id CAMERA = game::MAG316_cameraAnim;
	static constexpr game::Id SEQ_Particle = game::MAG_316_RedGiant_SeqParticle;
	static constexpr game::Id COLOUR_Target = game::MAG_316_RedGiant_ColourLerpTarget;
	static const void *SOUND_Voice() { return (const void *)game_addr(game::MAG_316_RedGiant_SoundVoice); }
	static const void *SOUND_Start() { return (const void *)game_addr(game::MAG_316_RedGiant_SoundStart); }
	static const uint32_t VERTS_B = 0x81C;
	static const uint32_t ARENA_A = 0x881C, ARENA_B = 0x1881C;
	static const int16_t PAGE_MOVE = 0xB, CLUT_MOVE = 0x414;

	inline void FillDwords(void *dst, uint32_t value, uint32_t dwords) { fn<void (__cdecl *)(void *, uint32_t, uint32_t)>(game_addr(game::Effect_FillDwords))(dst, value, dwords); }
	inline uint32_t MagicTextureOff() { return fn<uint32_t (__cdecl *)()>(game_addr(game::Magic_TextureOFF_ToEAX1))(); }

#pragma pack(push, 1)
	struct RootNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t pad0E;
		uint8_t started;
		uint32_t parity;
	};
	struct Particle
	{
		int16_t pos[3];
		int16_t life;
		int16_t size;
		int16_t pad0A;
		int8_t vel[3];
		int8_t pad0F;
	};
	struct MainNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		uint32_t voice;
		uint32_t colour;
		Particle parts[0x80];
		Particle *next;
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x14 && sizeof(Particle) == 0x10 && sizeof(MainNode) == 0x81C, "316 nodes");
}
}

namespace ff8fx
{
namespace redgiant316
{
	using namespace eng;
	using namespace magc;
	using namespace msweep;

	static int32_t Rand15()
	{
		Lcg() = (Lcg() * 125 + 0xE) & 0x7FFF;
		return (int32_t)Lcg();
	}

	static void SpawnParticle(uint8_t *h)
	{
		MainNode *m = F<MainNode *>(h, 0x84);
		Particle *s = m->next;
		if (s->life != 0)
		{
			s = &m->parts[0];
			if (s->life != 0)
			{
				int k = 0x80;
				do
				{
					s++;
					if (--k == 0) return;
				} while (s->life != 0);
			}
		}
		m->next = s < &m->parts[0x7F] ? s + 1 : &m->parts[0];
		*(uint32_t *)&s->pos[0] = F<uint32_t>(h, 0x7C);
		*(uint32_t *)&s->pos[2] = F<uint32_t>(h, 0x80);
		s->life = 0xB;
		s->size = (int16_t)((Rand15() >> 0xC) + 0x400);
		s->vel[0] = (int8_t)((Rand15() >> 0xD) - 2);
		s->vel[1] = (int8_t)(0xFD - (Rand15() >> 0xB));
		s->vel[2] = (int8_t)((Rand15() >> 0xD) - 2);
	}

	static void SparkTri(uint8_t *h)
	{
		if (Rand15() >= 0x800) return;
		F<int16_t>(h, 0x7C) = (int16_t)(((int32_t)F<int16_t>(h, 0x44) + F<int16_t>(h, 0x34) + F<int16_t>(h, 0x24)) / 3);
		F<int16_t>(h, 0x7E) = (int16_t)(((int32_t)F<int16_t>(h, 0x46) + F<int16_t>(h, 0x36) + F<int16_t>(h, 0x26)) / 3);
		F<int16_t>(h, 0x80) = (int16_t)(((int32_t)F<int16_t>(h, 0x48) + F<int16_t>(h, 0x38) + F<int16_t>(h, 0x28)) / 3);
		SpawnParticle(h);
	}

	static void SparkQuad(uint8_t *h)
	{
		if (Rand15() >= 0x800) return;
		F<int16_t>(h, 0x7C) = (int16_t)(((int32_t)F<int16_t>(h, 0x54) + F<int16_t>(h, 0x44) + F<int16_t>(h, 0x34) + F<int16_t>(h, 0x24)) / 4);
		F<int16_t>(h, 0x7E) = (int16_t)(((int32_t)F<int16_t>(h, 0x56) + F<int16_t>(h, 0x46) + F<int16_t>(h, 0x36) + F<int16_t>(h, 0x26)) / 4);
		F<int16_t>(h, 0x80) = (int16_t)(((int32_t)F<int16_t>(h, 0x58) + F<int16_t>(h, 0x48) + F<int16_t>(h, 0x38) + F<int16_t>(h, 0x28)) / 4);
		SpawnParticle(h);
	}

	static void ProjectB(uint8_t *h)
	{
		const uint8_t *data = F<uint8_t *>(h, 0x74);
		uint8_t *out = F<uint8_t *>(h, 0x78);
		const int32_t n = *(const int16_t *)data;
		data += 2;
		if (n != 0)
		{
			for (int32_t k = n; k != 0; k--)
			{
				GteLoadV0u16(data);
				GteMVMVA_RotV0Tr();
				out += 0x10;
				data += 6;
				GteStoreIR123Minus8(out);
			}
		}
		GteSetRotMatrixCtrl(&Camera());
		GteSetTransVectorCtrl(&Camera());
		out = F<uint8_t *>(h, 0x78);
		if (n != 0)
		{
			for (int32_t k = n; k != 0; k--)
			{
				GteLoadV0(out + 8);
				GteRTPS();
				const uint8_t g = GreyOf(*(const int16_t *)(out + 0xA), F<int16_t>(h, 0x6C));
				GteReadSXY2(h + 0x1C);
				GteReadOTZ(h + 0x20);
				if (F<int16_t>(h, 0x20) > 0)
				{
					out[6] = g;
					*(uint32_t *)out = F<uint32_t>(h, 0x1C);
					*(uint16_t *)(out + 4) = F<uint16_t>(h, 0x20);
				}
				else *(uint16_t *)(out + 4) = 0;
				out += 0x10;
			}
		}
		F<const uint8_t *>(h, 0x74) = data;
		F<uint8_t *>(h, 0x78) = out;
	}

	static inline void Copy16(uint8_t *dst, const uint8_t *src) { ff8fx::copy<(0x10)>(dst, src); }

	static void PolysB(uint8_t *h, uint32_t ot, int32_t shift)
	{
		const uint8_t *verts = F<const uint8_t *>(h, 4);
		const uint8_t *p = F<const uint8_t *>(h, 0);
		uint8_t *pk = F<uint8_t *>(h, 0x88);
		for (uint32_t i = 0; i < F<uint16_t>(h, 8); i++, p += 0x10)
		{
			Copy16(h + 0x1C, verts + ((*(const uint16_t *)p & 0xFFF) << 4));
			Copy16(h + 0x2C, verts + ((*(const uint16_t *)(p + 2) & 0xFFF) << 4));
			Copy16(h + 0x3C, verts + ((*(const uint16_t *)(p + 4) & 0xFFF) << 4));
			if ((int16_t)(F<uint16_t>(h, 0x30) | F<uint16_t>(h, 0x40) | F<uint16_t>(h, 0x20)) <= 0) continue;
			if ((uint8_t)(h[0x32] | h[0x42] | h[0x22]) == 0) continue;
			GteSetSXY012(F<uint32_t>(h, 0x1C), F<uint32_t>(h, 0x2C), F<uint32_t>(h, 0x3C));
			GteNCLIP();
			GteReadMAC0(h + 0x5C);
			if (F<int32_t>(h, 0x5C) < 0) continue;
			GteSetSZ123(F<int16_t>(h, 0x20), F<int16_t>(h, 0x30), F<int16_t>(h, 0x40));
			GteAVSZ3();
			GteReadOTZWord(h + 0x64);
			const uint32_t bucket = ot + (uint32_t)(F<int32_t>(h, 0x64) >> (shift & 31)) * 4;
			if ((uint8_t)(h[0x32] & h[0x42] & h[0x22]) == 0x80)
			{
				F<uint32_t>(pk, 0) = 0x7000000;
				F<uint32_t>(pk, 4) = F<uint32_t>(h, 0x14) | 0x24000000;
				F<uint32_t>(pk, 8) = F<uint32_t>(h, 0x1C);
				F<uint32_t>(pk, 0x10) = F<uint32_t>(h, 0x2C);
				F<uint32_t>(pk, 0x18) = F<uint32_t>(h, 0x3C);
				F<uint32_t>(pk, 0xC) = *(const uint32_t *)(p + 8);
				F<uint32_t>(pk, 0x14) = *(const uint32_t *)(p + 0xC);
				F<uint16_t>(pk, 0x1C) = *(const uint16_t *)(p + 6);
				F<int16_t>(pk, 0x16) = (int16_t)(F<int16_t>(pk, 0x16) + F<int16_t>(h, 0x6E));
				F<int16_t>(pk, 0xE) = (int16_t)(F<int16_t>(pk, 0xE) + F<int16_t>(h, 0x70));
				InsertPrimAutoDepth(bucket, pk);
				pk += 0x20;
				continue;
			}
			uint8_t *q = F<uint8_t *>(h, 0x8C);
			uint8_t *g = pk;
			pk += 0x28;
			F<uint8_t *>(h, 0x8C) = q + 0x1C;
			F<uint32_t>(g, 0) = 0x9000000;
			F<uint32_t>(q, 0) = 0x6000000;
			F<uint32_t>(q, 8) = F<uint32_t>(g, 8) = F<uint32_t>(h, 0x1C);
			F<uint32_t>(q, 0x10) = F<uint32_t>(g, 0x14) = F<uint32_t>(h, 0x2C);
			F<uint32_t>(q, 0x18) = F<uint32_t>(g, 0x20) = F<uint32_t>(h, 0x3C);
			F<uint32_t>(g, 0xC) = *(const uint32_t *)(p + 8);
			F<uint32_t>(g, 0x18) = *(const uint32_t *)(p + 0xC);
			F<uint16_t>(g, 0x24) = *(const uint16_t *)(p + 6);
			F<uint32_t>(g, 4) = Grey(h[0x22]) | 0x36000000;
			F<uint32_t>(q, 4) = Grey(h[0x22]) | 0x32000000;
			F<uint32_t>(q, 0xC) = F<uint32_t>(g, 0x10) = Grey(h[0x32]);
			F<uint32_t>(q, 0x14) = F<uint32_t>(g, 0x1C) = Grey(h[0x42]);
			F<int16_t>(g, 0x1A) = (int16_t)(F<int16_t>(g, 0x1A) + F<int16_t>(h, 0x6E));
			F<int16_t>(g, 0xE) = (int16_t)(F<int16_t>(g, 0xE) + F<int16_t>(h, 0x70));
			InsertPrimAutoDepth(bucket, g);
			InsertPrimAutoDepth(bucket, q);
			uint8_t *d = F<uint8_t *>(h, 0x8C);
			F<uint8_t *>(h, 0x8C) = d + 8;
			F<uint32_t>(d, 0) = 0x1000000;
			F<uint32_t>(d, 4) = 0xE10002C0;
			InsertPrimAutoDepth(bucket, d);
			SparkTri(h);
		}
		p += 4;
		for (uint32_t i = 0; i < F<uint16_t>(h, 0xA); i++, p += 0x14)
		{
			Copy16(h + 0x1C, verts + ((*(const uint16_t *)(p - 4) & 0xFFF) << 4));
			Copy16(h + 0x2C, verts + ((*(const uint16_t *)(p - 2) & 0xFFF) << 4));
			Copy16(h + 0x3C, verts + ((*(const uint16_t *)p & 0xFFF) << 4));
			Copy16(h + 0x4C, verts + ((*(const uint16_t *)(p + 2) & 0xFFF) << 4));
			if ((int16_t)(F<uint16_t>(h, 0x30) | F<uint16_t>(h, 0x50) | F<uint16_t>(h, 0x40) | F<uint16_t>(h, 0x20)) <= 0) continue;
			if ((uint8_t)(h[0x32] | h[0x42] | h[0x52] | h[0x22]) == 0) continue;
			GteSetSXY012(F<uint32_t>(h, 0x1C), F<uint32_t>(h, 0x2C), F<uint32_t>(h, 0x3C));
			GteNCLIP();
			GteReadMAC0(h + 0x5C);
			if (F<int32_t>(h, 0x5C) < 0) continue;
			GteSetSZ0123(F<int16_t>(h, 0x20), F<int16_t>(h, 0x30), F<int16_t>(h, 0x40), F<int16_t>(h, 0x50));
			GteAVSZ4();
			GteReadOTZWord(h + 0x64);
			const uint32_t bucket = ot + (uint32_t)(F<int32_t>(h, 0x64) >> (shift & 31)) * 4;
			if ((uint8_t)(h[0x32] & h[0x42] & h[0x52] & h[0x22]) == 0x80)
			{
				F<uint32_t>(pk, 0) = 0x9000000;
				F<uint32_t>(pk, 4) = F<uint32_t>(h, 0x14) | 0x2C000000;
				F<uint32_t>(pk, 8) = F<uint32_t>(h, 0x1C);
				F<uint32_t>(pk, 0x10) = F<uint32_t>(h, 0x2C);
				F<uint32_t>(pk, 0x18) = F<uint32_t>(h, 0x3C);
				F<uint32_t>(pk, 0x20) = F<uint32_t>(h, 0x4C);
				F<uint32_t>(pk, 0xC) = *(const uint32_t *)(p + 4);
				F<uint32_t>(pk, 0x14) = *(const uint32_t *)(p + 8);
				F<uint16_t>(pk, 0x1C) = *(const uint16_t *)(p + 0xC);
				F<uint16_t>(pk, 0x24) = *(const uint16_t *)(p + 0xE);
				F<int16_t>(pk, 0x16) = (int16_t)(F<int16_t>(pk, 0x16) + F<int16_t>(h, 0x6E));
				F<int16_t>(pk, 0xE) = (int16_t)(F<int16_t>(pk, 0xE) + F<int16_t>(h, 0x70));
				InsertPrimAutoDepth(bucket, pk);
				pk += 0x28;
				continue;
			}
			uint8_t *q = F<uint8_t *>(h, 0x8C);
			uint8_t *g = pk;
			pk += 0x34;
			F<uint8_t *>(h, 0x8C) = q + 0x24;
			F<uint32_t>(g, 0) = 0xC000000;
			F<uint32_t>(q, 0) = 0x8000000;
			F<uint32_t>(q, 8) = F<uint32_t>(g, 8) = F<uint32_t>(h, 0x1C);
			F<uint32_t>(q, 0x10) = F<uint32_t>(g, 0x14) = F<uint32_t>(h, 0x2C);
			F<uint32_t>(q, 0x18) = F<uint32_t>(g, 0x20) = F<uint32_t>(h, 0x3C);
			F<uint32_t>(q, 0x20) = F<uint32_t>(g, 0x2C) = F<uint32_t>(h, 0x4C);
			F<uint32_t>(g, 0xC) = *(const uint32_t *)(p + 4);
			F<uint32_t>(g, 0x18) = *(const uint32_t *)(p + 8);
			F<uint16_t>(g, 0x24) = *(const uint16_t *)(p + 0xC);
			F<uint16_t>(g, 0x30) = *(const uint16_t *)(p + 0xE);
			F<uint32_t>(g, 4) = Grey(h[0x22]) | 0x3E000000;
			F<uint32_t>(q, 4) = Grey(h[0x22]) | 0x3A000000;
			F<uint32_t>(q, 0xC) = F<uint32_t>(g, 0x10) = Grey(h[0x32]);
			F<uint32_t>(q, 0x14) = F<uint32_t>(g, 0x1C) = Grey(h[0x42]);
			F<uint32_t>(q, 0x1C) = F<uint32_t>(g, 0x28) = Grey(h[0x52]);
			F<int16_t>(g, 0x1A) = (int16_t)(F<int16_t>(g, 0x1A) + F<int16_t>(h, 0x6E));
			F<int16_t>(g, 0xE) = (int16_t)(F<int16_t>(g, 0xE) + F<int16_t>(h, 0x70));
			InsertPrimAutoDepth(bucket, g);
			InsertPrimAutoDepth(bucket, q);
			uint8_t *d = F<uint8_t *>(h, 0x8C);
			F<uint8_t *>(h, 0x8C) = d + 8;
			F<uint32_t>(d, 0) = 0x1000000;
			F<uint32_t>(d, 4) = 0xE10002C0;
			InsertPrimAutoDepth(bucket, d);
			SparkQuad(h);
		}
		F<uint8_t *>(h, 0x88) = pk;
	}

	static void ModelB(const uint8_t *model, uint8_t *h, uint32_t ot, int32_t shift)
	{
		const uint32_t *objs = *(const uint32_t *const *)(model + 4);
		const uint8_t *bones = *(const uint8_t *const *)model + 0x10;
		const int32_t count = (int32_t)objs[0];
		for (int32_t i = 0; i < count; i++)
		{
			F<const uint8_t *>(h, 0x74) = *(const uint8_t *const *)(model + 4) + objs[1 + i];
			if (!(F<uint32_t>(h, 0x18) & (1u << (i & 31)))) continue;
			F<uint32_t>(h, 0x78) = F<uint32_t>(h, 4);
			const uint8_t *p = F<const uint8_t *>(h, 0x74);
			const int32_t groups = *(const int16_t *)p;
			F<const uint8_t *>(h, 0x74) = p + 2;
			if (groups > 0)
			{
				for (int32_t g = groups; g != 0; g--)
				{
					const uint8_t *q = F<const uint8_t *>(h, 0x74);
					const int32_t bone = *(const int16_t *)q;
					F<const uint8_t *>(h, 0x74) = q + 2;
					const Mat4x3 *bm = (const Mat4x3 *)(bones + mul32(bone, 0x30) + 0x10);
					GteSetRotMatrixCtrl(bm);
					GteSetTransVectorCtrl(bm);
					ProjectB(h);
				}
			}
			p = (const uint8_t *)((F<uint32_t>(h, 0x74) + 3) & ~3u);
			F<uint16_t>(h, 8) = *(const uint16_t *)p;
			F<uint16_t>(h, 0xA) = *(const uint16_t *)(p + 2);
			F<const uint8_t *>(h, 0x74) = p + 0xC;
			F<const uint8_t *>(h, 0) = p + 0xC;
			PolysB(h, ot, shift);
		}
	}

	static void SweepB(MainNode *m, uint8_t *ent, uint32_t verts, int16_t level, int16_t page, int16_t clut)
	{
		FrameArena() = DrawShadow(ent, OT(0x4040), 0x10, FrameArena());
		uint8_t *b = (uint8_t *)FieldAlloc(0xB0);
		uint8_t *anim = ent + 0x60;
		F<int16_t>(b, 0x8E) = page;
		F<int16_t>(b, 0x8C) = level;
		F<int16_t>(b, 0x90) = clut;
		F<MainNode *>(b, 0xA4) = m;
		ComputeBonesWorldMatrices(anim, ent + 0x40);
		F<uint32_t>(b, 0x24) = verts;
		F<int16_t>(b, 0x2C) = 0;
		F<int16_t>(b, 0x2E) = 0;
		F<int16_t>(b, 0x30) = 0x140;
		F<int16_t>(b, 0x32) = 0xD8;
		F<uint32_t>(b, 0x34) = F<uint32_t>(ent, 0x28);
		F<uint32_t>(b, 0x38) = F<uint32_t>(ent, 0x7C);
		F<uint32_t>(b, 0xA8) = FrameArena();
		F<uint32_t>(b, 0xAC) = PacketCursor();
		ModelB(F<const uint8_t *>(anim, 4), b + 0x20, OT(0x44), 0);
		FrameArena() = F<uint32_t>(b, 0xA8);
		PacketCursor() = F<uint32_t>(b, 0xAC);
		BuildBoneMatricesFromPose(anim);
		FieldFree(0xB0);
	}

	static void ParticleDraw(uint8_t *h, Particle *p)
	{
		GteSetRotMatrixCtrl(&Camera());
		GteSetTransVectorCtrl(&Camera());
		GteLoadV0(p);
		GteMVMVA_RotV0Tr();
		GteReadMAC123((int32_t *)(h + 0x98));
		const int16_t size = p->size;
		F<uint32_t>(h, 0) = game_addr(SEQ_Particle);
		F<int16_t>(h, 0x94) = size;
		F<int16_t>(h, 0x8C) = size;
		F<int16_t>(h, 0x84) = size;
		const uint32_t idx = (var<uint32_t>(game_addr(SEQ_Particle) + 8) & 0xFFFF) - (uint32_t)(int32_t)p->life;
		const uint32_t rec = (uint32_t)var<uint16_t>(game_addr(SEQ_Particle) + 8 + idx * 2) + game_addr(SEQ_Particle);
		F<uint32_t>(h, 0x2C) = rec;
		F<uint32_t>(h, 0x30) = var<uint32_t>(rec);
		F<uint32_t>(h, 0x2C) = rec + 4;
		PacketCursor() = DrawSprites(h, OT(0x44), 2, PacketCursor(), false);
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

	static int16_t LevelA(int32_t c)
	{
		const uint32_t t = (uint32_t)(c - 0x14);
		return t < 0x64 ? (int16_t)(0x200 - (int32_t)(t * 4000u / 100u)) : (int16_t)-0xDA0;
	}
	static int16_t LevelB(int32_t c)
	{
		const uint32_t t = (uint32_t)(c - 0x96);
		return (int16_t)(0x200 - (int32_t)((600000u - t * 4000u) / 150u));
	}

	static uint32_t __cdecl MainTask(TaskNode *n)
	{
		MainNode *m = (MainNode *)n;
		const int16_t c = m->counter;
		if (c >= 0x138)
		{
			*(uint16_t *)Caster() &= 0xFFF7;
			*(uint32_t *)(Caster() + 0x2C) = m->colour;
			*(uint16_t *)Entity(0) &= 0xFFFB;
			*(uint16_t *)Entity(1) &= 0xFFFB;
			*(uint16_t *)Entity(2) &= 0xFFFB;
			return TASK_END;
		}
		if (c < 8) Flash() = (uint32_t)(mul32(c, 1000) >> 3);
		else if (c > 0x130) Flash() = (uint32_t)(mul32(0x138 - c, 1000) >> 3);
		else Flash() = 0x3E8;
		if ((uint32_t)(c - 0x14) < 0x82)
		{
			SweepA(Caster(), LevelA(c), PAGE_MOVE, CLUT_MOVE, PacketCursor());
			*Caster() |= 4;
		}
		if ((uint32_t)(c - 0x96) < 0x96)
		{
			SweepB(m, Caster(), TexBase() + VERTS_B, LevelB(c), PAGE_MOVE, CLUT_MOVE);
			*Caster() |= 4;
		}
		const uint32_t t3 = (uint32_t)(m->counter - 0x96);
		if (t3 < 0x8C)
		{
			const int32_t w = (int32_t)((t3 << 12) / 140u);
			ColourLerp(&m->colour, game_addr(COLOUR_Target), 0x1000 - w, w, Caster() + 0x2C);
			Caster()[0x2F] = 0x32;
		}
		if (m->counter >= 0x3C) *Caster() |= 8;
		Particles(m);
		if (m->counter == 0x2D)
		{
			BdPlaySE(SOUND_Start(), 0, 0x80);
			m->counter++;
			return 0;
		}
		if (m->counter == 0x134) ReleaseVoice(m->voice);
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
		if (r->counter == 2 && !r->started)
		{
			r->started = 1;
			MusicVolumeFade(0xF0);
			InitTaskQueuePool(QMain(), (void *)TexBase(), 0x81C, 1);
			MainNode *m = (MainNode *)AddTaskToQueue(QMain(), game_addr(ORIG_MainTask));
			FillDwords(&m->counter, 0, 0x204);
			m->voice = ClaimVoiceSlot(SOUND_Voice(), 1, 0x80);
			m->next = &m->parts[0];
			m->colour = *(const uint32_t *)(Caster() + 0x2C);
			ShowAllPolys(Caster());
		}
		const int left = r->started ? ExecuteTaskQueue(QMain()) : (int)n;
		SetScreenFlash(Flash(), 0);
		if (r->started && left == 0) return TASK_END;
		r->counter++;
		return 0;
	}
}

	void register_mag316_red_giant()
	{
		register_port(redgiant316::ORIG_RootTask, (void *)redgiant316::RootTask, "316 RootTask", 316);
		register_port(redgiant316::ORIG_MainTask, (void *)redgiant316::MainTask, "316 MainTask", 316);
	}
}

