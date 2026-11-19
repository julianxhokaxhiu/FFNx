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

// Effect 301: Gargantua's burrow

#include "mag_common.h"

namespace ff8fx
{
namespace burrow301
{
	using namespace eng;
	using namespace magc;

	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::MAG_301_GargantuaBurrow_CastContext)); }
	inline uint8_t *&Caster() { return var<uint8_t *>(game_addr(game::MAG_301_GargantuaBurrow_CasterEntity)); }
	inline TaskQueue *QDust() { return (TaskQueue *)game_addr(game::MAG_301_GargantuaBurrow_DustQueue); }
	inline TaskQueue *QRocks() { return (TaskQueue *)game_addr(game::MAG_301_GargantuaBurrow_RocksQueue); }
	inline TaskQueue *QMaster() { return (TaskQueue *)game_addr(game::MAG_301_GargantuaBurrow_MasterQueue); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_301_GargantuaBurrow_PacketCursor)); }
	inline uint16_t &PartyWord(int i) { return var<uint16_t>(game_addr(game::unk_2507744) + i * 4); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::MAG_301_GargantuaBurrow_MagicBufferBase)); }
	inline uint8_t *&Stream() { return var<uint8_t *>(game_addr(game::MAG_301_GargantuaBurrow_SpawnStream)); }
	inline uint32_t &FrameCursor() { return var<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)); }
	inline uint32_t RenderOT44() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44; }

	static constexpr game::Id ORIG_RootTask = game::MAG_301_GargantuaBurrow_RootTask;
	static constexpr game::Id ORIG_MasterTask = game::MAG_301_GargantuaBurrow_MasterTask;
	static constexpr game::Id ORIG_RockTask = game::MAG_301_GargantuaBurrow_RockTask;
	static constexpr game::Id ORIG_DustTask = game::MAG_301_GargantuaBurrow_DustTask;
	static constexpr game::Id ORIG_PrimCallback = game::MAG_301_GargantuaBurrow_PrimCallback;
	static const uint32_t POOL_Rocks = 0xE14, POOL_Dust = 0x1194;
	static const uint32_t VERTS = 0x1494;
	static const uint32_t ARENA_A = 0x6204, ARENA_B = 0x16204;
	static constexpr game::Id MODEL_Layout = game::MAG_301_GargantuaBurrow_ModelLayout;
	static constexpr game::Id MODEL_Rock = game::MAG_301_GargantuaBurrow_ModelRock;
	static constexpr game::Id SEQ_Dust = game::MAG_301_GargantuaBurrow_SeqDust;
	static constexpr game::Id SEQ_Mark = game::MAG_301_GargantuaBurrow_SeqMark;
	static const void *SOUND_Voice() { return (const void *)game_addr(game::MAG_301_GargantuaBurrow_SoundVoice); }
	static const void *SOUND_Start() { return (const void *)game_addr(game::MAG_301_GargantuaBurrow_SoundStart); }
	static const int32_t GROUND = 0x40;

	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
	inline uint32_t RenderPrimModel70(void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::MAG_070_RenderPrimModel))(h, ot, mode, cursor); }
	inline void ReleaseVoiceSlot(uint32_t slot) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::BdSound_ReleaseVoiceSlot))(slot); }
	inline void RotY(int32_t angle, Mat4x3 *out) { fn<void (__cdecl *)(int32_t, Mat4x3 *)>(game_addr(game::Effect_RotationYMatrix))(angle, out); }
	inline void RotX(int32_t angle, Mat4x3 *out) { fn<void (__cdecl *)(int32_t, Mat4x3 *)>(game_addr(game::MAG_063_RotationXMatrix))(angle, out); }
	inline void MatrixMultiply3x3(Mat4x3 *a, const Mat4x3 *b) { fn<void (__cdecl *)(Mat4x3 *, const Mat4x3 *)>(game_addr(game::MatrixMultiply3x3))(a, b); }
	inline void GteLoadIRBytes(const void *src) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadIR123FromBytes))(src); }
	inline void GteStoreIRBytes(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreIR123LowBytes))(dst); }
	inline void GteWriteDataReg(uint32_t value, int32_t reg) { fn<void (__cdecl *)(uint32_t, int32_t)>(game_addr(game::GTE_WriteDataReg))(value, reg); }
	inline void GteLoadV0u(const void *v) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadV0_u16))(v); }
	inline void GteStoreIR123Minus8(void *end) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreIR123_Minus8))(end); }
	inline void GteSetSXY012(uint32_t a, uint32_t b, uint32_t c) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_SetSXY012))(a, b, c); }
	inline void GteSetSZ123(uint32_t a, uint32_t b, uint32_t c) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_SetSZ123))(a, b, c); }
	inline void GteSetSZ0123(uint32_t a, uint32_t b, uint32_t c, uint32_t d) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_SetSZ0123))(a, b, c, d); }
	inline void GteNCLIP() { fn<void (__cdecl *)()>(game_addr(game::GTE_NCLIP))(); }
	inline void GteReadMAC0(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadMAC0))(dst); }
	inline void GteAVSZ3() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ3))(); }
	inline void GteAVSZ4() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ4))(); }
	inline void GteReadOTZWord(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadOTZ))(dst); }
	inline void GteLoadIRColumn(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadIRFromMatrixColumn))(m); }
	inline void GteStoreIRColumn(void *m) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreIRToMatrixColumn))(m); }
	inline void GteMVMVA_RotIR() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotIR))(); }
	inline void GteLoadV0Dwords(const void *v) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadV0FromDwords))(v); }
	inline void GteLoadV012(const void *a, const void *b, const void *c) { fn<void (__cdecl *)(const void *, const void *, const void *)>(game_addr(game::GTE_LoadV012))(a, b, c); }
	inline void GteRTPT() { fn<void (__cdecl *)()>(game_addr(game::GTE_RTPT))(); }
	inline void GteReadFLAG(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadFLAG))(dst); }
	inline void GteReadSXY012Split(void *a, void *b, void *c) { fn<void (__cdecl *)(void *, void *, void *)>(game_addr(game::GTE_ReadSXY012_Split))(a, b, c); }

#pragma pack(push, 1)
	struct RootNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t pad0E;
		uint8_t started;
		uint32_t arena;
	};
	struct Mark
	{
		int16_t x, z;
		int16_t index;
		int16_t life;
	};
	struct MasterNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		uint32_t voice;
		int16_t pos[4];
		uint8_t layout[0x90];
		Mark marks[0x80];
		uint8_t index[0x960];
		Mark *next;
		uint32_t colour;
	};
	struct RockNode
	{
		TaskNode hdr;
		int16_t pos[3];
		int16_t counter;
		int8_t vel[3];
		int8_t gravity;
		int8_t angle[2];
		int8_t spin[2];
	};
	struct DustNode
	{
		TaskNode hdr;
		int16_t pos[3];
		int16_t counter;
		int8_t vel[3];
		uint8_t pad17;
	};
	struct PrimArg
	{
		int16_t pos[4];
		int32_t depth;
		uint32_t morph;
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x14 && sizeof(MasterNode) == 0xE14 && sizeof(RockNode) == 0x1C && sizeof(DustNode) == 0x18, "Gargantua burrow nodes");
	static_assert(sizeof(Mark) == 8 && sizeof(PrimArg) == 0x10, "Gargantua burrow blocks");

	template<typename T> static inline T &F(uint8_t *s, uint32_t off) { return *(T *)(s + off); }
}
}

namespace ff8fx
{
namespace burrow301
{
	static void __cdecl PrimCallback(prim::Layout *l, prim::Record *r, int arg_)
	{
		const PrimArg *arg = (const PrimArg *)arg_;
		if (((int32_t)r->scale[2] | *(const int32_t *)r->scale) == 0) return;
		if (r->a >= 0x1000 && *(const uint32_t *)r->rgb == 0) return;
		uint8_t *h = AllocHeader(0x58);
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
		RotationFromAngles(r->rot, &m);
		m.t[0] = (int32_t)r->pos[0] + arg->pos[0];
		m.t[1] = (int32_t)r->pos[1] + arg->pos[1];
		m.t[2] = (int32_t)r->pos[2] + arg->pos[2];
		ComposeAffineTransform(&Camera(), &m, &m);
		if (!(*(const uint32_t *)r->scale == 0x10001000 && r->scale[2] == 0x1000))
		{
			const int32_t v[3] = { r->scale[0], r->scale[1], r->scale[2] };
			Scale3DMatrix(&m, v);
		}
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		*(uint32_t *)(h + 0x1C) = 0x2000;
		*(int32_t *)(h + 0xC) = r->a;
		if (r->a != 0)
		{
			*(uint32_t *)(h + 0x1C) = 0x20C0;
			*(uint32_t *)(h + 8) = *(const uint32_t *)r->rgb;
		}
		*(int32_t *)(h + 0x18) = arg->depth;
		PacketCursor() = RenderPrimModel70(h, RenderOT44(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static void MarkAt(int32_t x, int32_t z, int32_t i)
	{
		uint8_t *base = (uint8_t *)TexBase();
		const int16_t index = (int16_t)i;
		const uint8_t slot = base[0x4AC + index];
		Mark *e;
		if (slot != 0xFF) e = (Mark *)(base + slot * 8 + 0xAC);
		else
		{
			e = *(Mark **)(base + 0xE0C);
			if (e->life != 0)
			{
				e = (Mark *)(base + 0xAC);
				if (*(const int16_t *)(base + 0xB2) != 0)
				{
					int32_t n = 0x80;
					do
					{
						e++;
						if (--n == 0) return;
					} while (e->life != 0);
				}
			}
			*(Mark **)(base + 0xE0C) = (uintptr_t)e >= (uintptr_t)(base + 0x4A4) ? (Mark *)(base + 0xAC) : e + 1;
			e->index = index;
		}
		e->x = (int16_t)x;
		e->z = (int16_t)z;
		e->life = 0xC;
	}

	static void BodyVertices(uint8_t *s)
	{
		uint8_t *p = F<uint8_t *>(s, 0x78);
		uint8_t *out = F<uint8_t *>(s, 0x84);
		const int32_t n = *(const int16_t *)p;
		p += 2;
		if (n != 0)
		{
			uint32_t left = (uint32_t)n;
			do
			{
				GteLoadV0u(p);
				GteMVMVA_RotV0Tr();
				out += 0x10;
				p += 6;
				GteStoreIR123Minus8(out);
			} while (--left != 0);
		}
		GteSetRotMatrixCtrl(&Camera());
		GteSetTransVectorCtrl(&Camera());
		out = F<uint8_t *>(s, 0x84);
		if (n != 0)
		{
			uint32_t left = (uint32_t)n;
			do
			{
				GteLoadV0(out + 8);
				GteRTPS();
				const uint8_t side = (uint8_t)(((int32_t)*(const int16_t *)(out + 0xA) > F<int32_t>(s, 0x6C)) + 1);
				GteReadSXY2(s + 0x1C);
				GteReadOTZ(s + 0x20);
				if (F<uint16_t>(s, 0x20) == 0)
					out[6] = 0x10;
				else
				{
					s[0x23] = side;
					const int32_t vx = F<int16_t>(s, 0x1C);
					if (vx < shl32(F<int16_t>(s, 0xC), 3)) s[0x22] |= 1;
					else if (vx >= shl32(F<int16_t>(s, 0x10), 3)) s[0x22] |= 2;
					const int32_t vy = F<int16_t>(s, 0x1E);
					if (vy < shl32(F<int16_t>(s, 0xE), 3)) s[0x22] |= 4;
					else if (vy >= shl32(F<int16_t>(s, 0x12), 3)) s[0x22] |= 8;
					*(uint32_t *)out = F<uint32_t>(s, 0x1C);
					*(uint32_t *)(out + 4) = F<uint32_t>(s, 0x20);
				}
				out += 0x10;
			} while (--left != 0);
		}
		F<uint8_t *>(s, 0x84) = out;
		F<uint8_t *>(s, 0x78) = p;
	}

	static inline void CopyVertex(uint8_t *s, uint32_t off, const uint8_t *verts, uint16_t index)
	{
		const uint8_t *v = verts + (uint32_t)(index & 0xFFF) * 0x10;
		F<uint32_t>(s, off) = *(const uint32_t *)v;
		F<uint32_t>(s, off + 4) = *(const uint32_t *)(v + 4);
		F<uint32_t>(s, off + 8) = *(const uint32_t *)(v + 8);
		F<uint32_t>(s, off + 0xC) = *(const uint32_t *)(v + 0xC);
	}

	static uint32_t BodyPolygons(uint8_t *s, uint32_t cursor)
	{
		uint8_t *pk = (uint8_t *)cursor;
		const uint8_t *verts = F<uint8_t *>(s, 4);
		const uint8_t *p = F<uint8_t *>(s, 0);
		const uint16_t tris = F<uint16_t>(s, 8);
		uint16_t k = tris;
		F<uint16_t>(s, 8) = (uint16_t)(k - 1);
		if (k != 0)
		{
			do
			{
				CopyVertex(s, 0x1C, verts, *(const uint16_t *)p);
				CopyVertex(s, 0x2C, verts, *(const uint16_t *)(p + 2));
				CopyVertex(s, 0x3C, verts, *(const uint16_t *)(p + 4));
				const uint8_t a = s[0x22], b = s[0x32], c = s[0x42];
				if ((uint8_t)(a | b | c) < 0x10 && !(c & (a & b)))
				{
					const uint8_t side = s[0x43] & s[0x33] & s[0x23];
					if (side != 2)
					{
						if (side == 0 && ((F<int32_t>(s, 0x70) + F<uint16_t>(s, 8)) & 7) == 0 && (int32_t)F<int16_t>(s, 0x28) > F<int32_t>(s, 0x74))
						{
							const int32_t z = ((int32_t)F<int16_t>(s, 0x28) + F<int16_t>(s, 0x48) + F<int16_t>(s, 0x38)) / 3;
							const int32_t x = ((int32_t)F<int16_t>(s, 0x44) + F<int16_t>(s, 0x34) + F<int16_t>(s, 0x24)) / 3;
							MarkAt(x, z, F<uint16_t>(s, 8));
						}
						GteSetSXY012(F<uint32_t>(s, 0x1C), F<uint32_t>(s, 0x2C), F<uint32_t>(s, 0x3C));
						GteNCLIP();
						GteReadMAC0(s + 0x5C);
						if (F<int32_t>(s, 0x5C) >= 0)
						{
							GteSetSZ123(F<uint16_t>(s, 0x20), F<uint16_t>(s, 0x30), F<uint16_t>(s, 0x40));
							GteAVSZ3();
							*(uint32_t *)pk = 0x7000000;
							*(uint32_t *)(pk + 4) = ((((uint32_t)*(const uint16_t *)(p + 0xE) & 0x200) | 0x2400) << 16) | F<uint32_t>(s, 0x14);
							*(uint32_t *)(pk + 8) = F<uint32_t>(s, 0x1C);
							*(uint32_t *)(pk + 0x10) = F<uint32_t>(s, 0x2C);
							*(uint32_t *)(pk + 0x18) = F<uint32_t>(s, 0x3C);
							*(uint32_t *)(pk + 0xC) = *(const uint32_t *)(p + 8);
							*(uint32_t *)(pk + 0x14) = *(const uint32_t *)(p + 0xC);
							*(uint16_t *)(pk + 0x16) &= 0xFDFF;
							*(uint16_t *)(pk + 0x1C) = *(const uint16_t *)(p + 6);
							GteReadOTZWord(s + 0x64);
							InsertPrimAutoDepth(F<uint32_t>(s, 0x7C) + (uint32_t)(F<int32_t>(s, 0x64) >> (F<int32_t>(s, 0x80) & 31)) * 4, pk);
							pk += 0x20;
						}
					}
				}
				k = F<uint16_t>(s, 8);
				p += 0x10;
				F<uint16_t>(s, 8) = (uint16_t)(k - 1);
			} while (k != 0);
		}
		k = F<uint16_t>(s, 0xA);
		uint16_t rest = (uint16_t)(k - 1);
		F<uint16_t>(s, 0xA) = rest;
		if (k != 0)
		{
			p += 4;
			do
			{
				CopyVertex(s, 0x1C, verts, *(const uint16_t *)(p - 4));
				CopyVertex(s, 0x2C, verts, *(const uint16_t *)(p - 2));
				CopyVertex(s, 0x3C, verts, *(const uint16_t *)p);
				CopyVertex(s, 0x4C, verts, *(const uint16_t *)(p + 2));
				const uint8_t d = s[0x52], a = s[0x22], b = s[0x32], c = s[0x42];
				if ((uint8_t)(d | a | b | c) < 0x10 && !(c & (d & a & b)))
				{
					const uint8_t side = s[0x53] & s[0x43] & s[0x33] & s[0x23];
					if (side != 2)
					{
						if (side == 0 && ((F<int32_t>(s, 0x70) + rest) & 7) == 0 && (int32_t)F<int16_t>(s, 0x28) > F<int32_t>(s, 0x74))
						{
							const int32_t z = ((int32_t)F<int16_t>(s, 0x48) + F<int16_t>(s, 0x28) + F<int16_t>(s, 0x58) + F<int16_t>(s, 0x38)) / 4;
							const int32_t x = ((int32_t)F<int16_t>(s, 0x54) + F<int16_t>(s, 0x44) + F<int16_t>(s, 0x34) + F<int16_t>(s, 0x24)) / 4;
							MarkAt(x, z, (uint16_t)(rest + tris));
						}
						GteSetSXY012(F<uint32_t>(s, 0x1C), F<uint32_t>(s, 0x2C), F<uint32_t>(s, 0x3C));
						GteNCLIP();
						GteReadMAC0(s + 0x5C);
						if (F<int32_t>(s, 0x5C) >= 0)
						{
							GteSetSZ0123(F<uint16_t>(s, 0x20), F<uint16_t>(s, 0x30), F<uint16_t>(s, 0x40), F<uint16_t>(s, 0x50));
							GteAVSZ4();
							*(uint32_t *)pk = 0x9000000;
							*(uint32_t *)(pk + 4) = ((((uint32_t)*(const uint16_t *)(p + 0xA) & 0x200) | 0x2C00) << 16) | F<uint32_t>(s, 0x14);
							*(uint32_t *)(pk + 8) = F<uint32_t>(s, 0x1C);
							*(uint32_t *)(pk + 0x10) = F<uint32_t>(s, 0x2C);
							*(uint32_t *)(pk + 0x18) = F<uint32_t>(s, 0x3C);
							*(uint32_t *)(pk + 0x20) = F<uint32_t>(s, 0x4C);
							*(uint32_t *)(pk + 0xC) = *(const uint32_t *)(p + 4);
							*(uint32_t *)(pk + 0x14) = *(const uint32_t *)(p + 8);
							*(uint16_t *)(pk + 0x1C) = *(const uint16_t *)(p + 0xC);
							*(uint16_t *)(pk + 0x16) &= 0xFDFF;
							*(uint16_t *)(pk + 0x24) = *(const uint16_t *)(p + 0xE);
							GteReadOTZWord(s + 0x64);
							InsertPrimAutoDepth(F<uint32_t>(s, 0x7C) + (uint32_t)(F<int32_t>(s, 0x64) >> (F<int32_t>(s, 0x80) & 31)) * 4, pk);
							pk += 0x28;
						}
					}
				}
				k = F<uint16_t>(s, 0xA);
				p += 0x14;
				rest = (uint16_t)(k - 1);
				F<uint16_t>(s, 0xA) = rest;
			} while (k != 0);
		}
		return (uint32_t)pk;
	}

	static uint32_t BodyObjects(const uint8_t *model, uint8_t *s, uint32_t cursor)
	{
		const uint8_t *table = *(uint8_t *const *)(model + 4);
		const uint8_t *bones = *(uint8_t *const *)model + 0x10;
		const int32_t count = *(const int32_t *)table;
		const uint32_t *offs = (const uint32_t *)(table + 4);
		for (int32_t i = 0; i < count; i++)
		{
			F<uint8_t *>(s, 0x78) = (uint8_t *)(*(uint8_t *const *)(model + 4) + offs[i]);
			if (!(F<uint32_t>(s, 0x18) & (1u << (i & 31)))) continue;
			F<uint32_t>(s, 0x84) = F<uint32_t>(s, 4);
			uint8_t *obj = F<uint8_t *>(s, 0x78);
			int32_t groups = *(const int16_t *)obj;
			F<uint8_t *>(s, 0x78) = obj + 2;
			if (groups > 0)
			{
				do
				{
					obj = F<uint8_t *>(s, 0x78);
					const int32_t bone = *(const int16_t *)obj;
					F<uint8_t *>(s, 0x78) = obj + 2;
					const Mat4x3 *bm = (const Mat4x3 *)(bones + bone * 0x30 + 0x10);
					GteSetRotMatrixCtrl(bm);
					GteSetTransVectorCtrl(bm);
					BodyVertices(s);
				} while (--groups != 0);
			}
			obj = (uint8_t *)(((uintptr_t)F<uint8_t *>(s, 0x78) + 3) & ~(uintptr_t)3);
			F<uint16_t>(s, 8) = *(const uint16_t *)obj;
			F<uint16_t>(s, 0xA) = *(const uint16_t *)(obj + 2);
			F<uint8_t *>(s, 0x78) = obj + 0xC;
			F<uint8_t *>(s, 0) = obj + 0xC;
			cursor = BodyPolygons(s, cursor);
		}
		return cursor;
	}

	static void DrawBody(uint8_t *entity, int32_t counter, int32_t ground, uint32_t buf)
	{
		uint8_t *s = (uint8_t *)FieldAlloc(0x88);
		F<int32_t>(s, 0x6C) = ground;
		F<int32_t>(s, 0x70) = counter;
		F<int32_t>(s, 0x74) = *(const int32_t *)(entity + 0x5C) - 0x320;
		F<uint32_t>(s, 4) = buf;
		ComputeBonesWorldMatrices(entity + 0x60, entity + 0x40);
		F<int16_t>(s, 0x10) = 0x140;
		F<int16_t>(s, 0xC) = 0;
		F<int16_t>(s, 0xE) = 0;
		F<int16_t>(s, 0x12) = 0xD8;
		F<uint32_t>(s, 0x14) = *(const uint32_t *)(entity + 0x28);
		F<uint32_t>(s, 0x18) = *(const uint32_t *)(entity + 0x7C);
		F<uint32_t>(s, 0x80) = 0;
		F<uint32_t>(s, 0x7C) = RenderOT44();
		FrameCursor() = BodyObjects(*(const uint8_t *const *)(entity + 0x64), s, FrameCursor());
		BuildBoneMatricesFromPose(entity + 0x60);
		FieldFree(0x88);
	}

	static uint32_t MarkSprites(uint8_t *h, uint32_t ot, int32_t mode, uint32_t cursor)
	{
		uint8_t *pk = (uint8_t *)cursor;
		const int32_t count = F<int32_t>(h, 0x30);
		const uint8_t *r = F<uint8_t *>(h, 0x2C);
		int32_t shift = mode;
		if (count > 0)
		{
			uint32_t left = (uint32_t)count;
			do
			{
				F<uint16_t>(h, 0x72) = 0;
				F<uint16_t>(h, 0x70) = 0;
				F<uint16_t>(h, 0x6E) = 0;
				F<uint16_t>(h, 0x6A) = 0;
				F<uint16_t>(h, 0x68) = 0;
				F<uint16_t>(h, 0x66) = 0;
				F<uint16_t>(h, 0x74) = 0x1000;
				F<uint16_t>(h, 0x64) = *(const uint16_t *)(r + 0x10);
				F<uint16_t>(h, 0x6A) = 0;
				F<uint16_t>(h, 0x66) = 0;
				F<uint16_t>(h, 0x6C) = *(const uint16_t *)(r + 0x12);
				const int32_t tpage = *(const int16_t *)(r + 0xE);
				const uint32_t w = r[0], hh = r[2];
				const int32_t w8 = (int32_t)(w << 3), h8 = (int32_t)(hh << 3);
				F<int16_t>(h, 0x54) = (int16_t)-w8;
				F<int16_t>(h, 0x44) = (int16_t)-w8;
				F<int16_t>(h, 0x5C) = (int16_t)w8;
				F<int16_t>(h, 0x4C) = (int16_t)w8;
				F<int16_t>(h, 0x4E) = (int16_t)-h8;
				F<int16_t>(h, 0x46) = (int16_t)-h8;
				F<int16_t>(h, 0x5E) = (int16_t)h8;
				F<int16_t>(h, 0x56) = (int16_t)h8;
				F<int32_t>(h, 0x78) = shl32(*(const int16_t *)(r + 8), 4) + w8;
				F<int32_t>(h, 0x7C) = shl32(*(const int16_t *)(r + 0xA), 4) + h8;
				F<int32_t>(h, 0x80) = 0;
				GteSetRotMatrixCtrl((const Mat4x3 *)(h + 0x84));
				GteLoadIRColumn(h + 0x64);
				GteMVMVA_RotIR();
				*(uint32_t *)pk = 0x9000000;
				*(uint32_t *)(pk + 4) = *(const uint32_t *)r;
				GteStoreIRColumn(h + 0x64);
				GteLoadIRColumn(h + 0x66);
				GteMVMVA_RotIR();
				*(uint32_t *)(pk + 0xC) = *(const uint32_t *)(r + 4);
				GteStoreIRColumn(h + 0x66);
				GteLoadIRColumn(h + 0x68);
				GteMVMVA_RotIR();
				pk[0x1C] = r[4];
				GteStoreIRColumn(h + 0x68);
				GteSetTransVectorCtrl((const Mat4x3 *)(h + 0x84));
				GteLoadV0Dwords(h + 0x78);
				GteMVMVA_RotV0Tr();
				pk[0x15] = r[5];
				GteReadMAC123((int32_t *)(h + 0x78));
				GteSetRotMatrixCtrl((const Mat4x3 *)(h + 0x64));
				GteSetTransVectorCtrl((const Mat4x3 *)(h + 0x64));
				GteLoadV012(h + 0x44, h + 0x4C, h + 0x54);
				GteRTPT();
				uint32_t u = r[4] + w;
				if ((int32_t)u >= 0x100) u = 0xFF;
				pk[0x24] = (uint8_t)u;
				pk[0x14] = (uint8_t)u;
				GteReadFLAG(h + 0x40);
				if (!(F<uint32_t>(h, 0x40) & 0x60000))
				{
					*(uint16_t *)(pk + 0x16) = (uint16_t)(tpage & 0x1FF);
					GteReadSXY012Split(pk + 8, pk + 0x10, pk + 0x18);
					GteLoadV0(h + 0x5C);
					GteRTPS();
					uint32_t v = r[5] + hh;
					if ((int32_t)v >= 0x100) v = 0xFF;
					pk[0x25] = (uint8_t)v;
					pk[0x1D] = (uint8_t)v;
					GteReadSXY2(pk + 0x20);
					GteAVSZ4();
					GteReadOTZWord(h + 0x3C);
					const int32_t z = (F<int32_t>(h, 0x3C) >> (shift & 31)) - 0x40;
					shift = z;
					if (z > 0)
					{
						InsertPrimAutoDepth(ot + (uint32_t)z * 4, pk);
						pk += 0x28;
					}
				}
				r += 0x14;
			} while (--left != 0);
		}
		return (uint32_t)pk;
	}

	static void MarkSprite(uint8_t *h, const Mark *e)
	{
		GteSetRotMatrixCtrl(&Camera());
		GteSetTransVectorCtrl(&Camera());
		GteWriteDataReg((uint16_t)e->x, 0);
		GteWriteDataReg((uint16_t)e->z, 1);
		GteMVMVA_RotV0Tr();
		GteReadMAC123((int32_t *)(h + 0x98));
		*(uint32_t *)h = game_addr(SEQ_Mark);
		F<uint16_t>(h, 0x94) = 0x1000;
		F<uint16_t>(h, 0x8C) = 0x1000;
		F<uint16_t>(h, 0x84) = 0x1000;
		const uint32_t f = (*(const uint32_t *)(game_addr(SEQ_Mark) + 8) & 0xFFFF) - (uint32_t)(int32_t)e->life;
		const uint32_t frame = *(const uint16_t *)(game_addr(SEQ_Mark) + 8 + f * 2) + game_addr(SEQ_Mark);
		F<uint32_t>(h, 0x2C) = frame;
		F<uint32_t>(h, 0x30) = *(const uint32_t *)frame;
		F<uint32_t>(h, 0x2C) = frame + 4;
		PacketCursor() = MarkSprites(h, RenderOT44(), 2, PacketCursor());
	}

	static void MarkDraw(uint8_t *h, Mark *e)
	{
		MarkSprite(h, e);
		e->life--;
		if (e->life == 0) ((uint8_t *)TexBase())[0x4AC + e->index] = 0xFF;
	}

	static void DrawMarks(MasterNode *m)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		F<uint16_t>(h, 0x92) = 0;
		F<uint16_t>(h, 0x90) = 0;
		F<uint16_t>(h, 0x8E) = 0;
		F<uint16_t>(h, 0x8A) = 0;
		F<uint16_t>(h, 0x88) = 0;
		F<uint16_t>(h, 0x86) = 0;
		F<uint16_t>(h, 0x60) = 0;
		F<uint16_t>(h, 0x58) = 0;
		F<uint16_t>(h, 0x50) = 0;
		F<uint16_t>(h, 0x48) = 0;
		Mark *e = m->marks;
		for (int32_t n = 0x80; n != 0; n--, e++)
			if (e->life != 0) MarkDraw(h, e);
		FieldFree(0xB4);
	}

	static void RockDraw(const int16_t pos[3], int16_t ax, int16_t ay)
	{
		uint8_t *h = AllocHeader(0x98);
		RotY(ay, (Mat4x3 *)(h + 0x78));
		RotX(ax, (Mat4x3 *)(h + 0x58));
		MatrixMultiply3x3((Mat4x3 *)(h + 0x58), (const Mat4x3 *)(h + 0x78));
		Mat4x3 *m = (Mat4x3 *)(h + 0x58);
		m->t[0] = pos[0];
		m->t[1] = pos[1];
		m->t[2] = pos[2];
		ComposeAffineTransform(&Camera(), m, m);
		GteSetRotMatrix(m);
		GteSetTransVector(m);
		*(uint32_t *)h = game_addr(MODEL_Rock);
		*(uint32_t *)(h + 0x1C) = 0;
		PacketCursor() = RenderPrimModel(h, RenderOT44(), 2, PacketCursor());
		FieldFree(0x98);
	}

	static uint32_t __cdecl RockTask(TaskNode *n)
	{
		RockNode *r = (RockNode *)n;
		RockDraw(r->pos, (int16_t)(r->angle[0] << 4), (int16_t)(r->angle[1] << 4));
		r->angle[0] = (int8_t)(r->angle[0] + r->spin[0]);
		r->angle[1] = (int8_t)(r->angle[1] + r->spin[1]);
		r->pos[0] = (int16_t)(r->pos[0] + (int16_t)(r->vel[0] << 4));
		r->pos[2] = (int16_t)(r->pos[2] + (int16_t)(r->vel[2] << 4));
		r->pos[1] = (int16_t)(r->pos[1] + (int16_t)(r->vel[1] << 2));
		if (r->pos[1] >= 0) r->vel[1] = (int8_t)(-(int32_t)r->vel[1] >> 1);
		r->vel[1] = (int8_t)(r->vel[1] + r->gravity);
		r->counter++;
		return r->counter < 0x10 ? 0 : TASK_END;
	}

	static void DustDraw(const int16_t *pos, int16_t frame)
	{
		TransformCameraByShadowRotation(pos, 0x1000, -0x400);
		uint8_t *h = AllocHeader(0xB4);
		*(uint32_t *)h = game_addr(SEQ_Dust);
		*(int16_t *)(h + 4) = frame;
		*(uint16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT44(), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static uint32_t __cdecl DustTask(TaskNode *n)
	{
		DustNode *d = (DustNode *)n;
		DustDraw(d->pos, d->counter);
		d->pos[0] = (int16_t)(d->pos[0] + d->vel[0]);
		d->pos[1] = (int16_t)(d->pos[1] + d->vel[1]);
		d->pos[2] = (int16_t)(d->pos[2] + d->vel[2]);
		d->counter++;
		return d->counter < 0x10 ? 0 : TASK_END;
	}

	static void SpawnRocks()
	{
		uint8_t *g = Stream();
		const int32_t count = *(const int16_t *)g;
		Stream() = g + 2;
		if (count == 0) return;
		uint32_t left = (uint32_t)count;
		do
		{
			RockNode *r = (RockNode *)AddTaskToQueue(QRocks(), game_addr(ORIG_RockTask));
			r->pos[0] = (int16_t)(*(const int16_t *)(Caster() + 0x54) + *(const int16_t *)Stream());
			g = Stream() + 2;
			Stream() = g;
			r->pos[1] = (int16_t)(*(const int16_t *)(Caster() + 0x58) + *(const int16_t *)g);
			g = Stream() + 2;
			Stream() = g;
			r->pos[2] = (int16_t)(*(const int16_t *)(Caster() + 0x5C) + *(const int16_t *)g);
			g = Stream() + 2;
			Stream() = g;
			r->counter = 0;
			r->vel[0] = *(const int8_t *)Stream();
			g = Stream() + 2;
			Stream() = g;
			r->vel[1] = *(const int8_t *)g;
			g = Stream() + 2;
			Stream() = g;
			r->vel[2] = *(const int8_t *)g;
			g = Stream() + 2;
			Stream() = g;
			r->gravity = 2;
			r->angle[0] = *(const int8_t *)Stream();
			g = Stream() + 2;
			Stream() = g;
			r->angle[1] = *(const int8_t *)g;
			g = Stream() + 2;
			Stream() = g;
			r->spin[0] = *(const int8_t *)g;
			g = Stream() + 2;
			Stream() = g;
			r->spin[1] = *(const int8_t *)g;
			Stream() = Stream() + 2;
		} while (--left != 0);
	}

	static void SpawnDust()
	{
		uint8_t *g = Stream();
		const int32_t count = *(const int16_t *)g;
		Stream() = g + 2;
		if (count == 0) return;
		uint32_t left = (uint32_t)count;
		do
		{
			DustNode *d = (DustNode *)AddTaskToQueue(QDust(), game_addr(ORIG_DustTask));
			d->pos[0] = (int16_t)(*(const int16_t *)(Caster() + 0x54) + *(const int16_t *)Stream());
			g = Stream() + 2;
			Stream() = g;
			d->pos[1] = (int16_t)(*(const int16_t *)(Caster() + 0x58) + *(const int16_t *)g);
			g = Stream() + 2;
			Stream() = g;
			d->pos[2] = (int16_t)(*(const int16_t *)(Caster() + 0x5C) + *(const int16_t *)g);
			g = Stream() + 2;
			Stream() = g;
			d->counter = 0;
			d->vel[0] = *(const int8_t *)Stream();
			g = Stream() + 2;
			Stream() = g;
			d->vel[1] = *(const int8_t *)g;
			g = Stream() + 2;
			Stream() = g;
			d->vel[2] = *(const int8_t *)g;
			Stream() = Stream() + 2;
			d->pad17 = 2;
		} while (--left != 0);
	}

	static uint32_t __cdecl MasterTask(TaskNode *n)
	{
		MasterNode *m = (MasterNode *)n;
		m->counter++;
		if (m->counter >= 0x8E)
		{
			*(uint16_t *)Caster() &= 0xFFF3;
			return TASK_END;
		}
		*(uint16_t *)Caster() &= 0xFFF7;
		const int32_t c = m->counter;
		if ((uint32_t)c < 0x7A)
		{
			PrimArg arg;
			ff8fx::copy<(sizeof(arg.pos))>(arg.pos, m->pos);
			arg.morph = TexBase() + VERTS;
			arg.depth = 0x100;
			prim::play((prim::Layout *)m->layout, PrimCallback, (int)&arg, 0);
			*(uint8_t *)Caster() |= 4;
			DrawBody(Caster(), c, GROUND, TexBase() + VERTS);
			if ((uint32_t)c < 0x66)
			{
				SpawnRocks();
				SpawnDust();
			}
		}
		else *(uint16_t *)Caster() &= 0xFFF3;
		const int32_t d = m->counter - 0x7A;
		if ((uint32_t)d <= 0x14)
		{
			GteSetIR0(0x1000 - ComputeSin(shl32(0x14 - d, 10) / 20));
			GteLoadIRBytes(&m->colour);
			GteGPF();
			GteStoreIRBytes(Caster() + 0x2C);
			Caster()[0x2F] = 0x32;
			if (d == 0x14) *(uint32_t *)(Caster() + 0x2C) = m->colour;
		}
		DrawMarks(m);
		if (m->counter == 1)
		{
			BdPlaySE(SOUND_Start(), 0, 0x80);
			return 0;
		}
		if (m->counter == 0x8A) ReleaseVoiceSlot(m->voice);
		return 0;
	}

	static uint32_t __cdecl RootTask(TaskNode *n)
	{
		RootNode *rn = (RootNode *)n;
		if (rn->arena)
		{
			PacketCursor() = TexBase() + ARENA_A;
			rn->arena = 0;
		}
		else
		{
			PacketCursor() = TexBase() + ARENA_B;
			rn->arena = 1;
		}
		*(uint8_t *)Caster() |= 0xC;
		if (rn->counter == 1 && !rn->started)
		{
			rn->started = 1;
			InitTaskQueuePool(QMaster(), (void *)TexBase(), 0xE14, 1);
			InitTaskQueuePool(QRocks(), (void *)(TexBase() + POOL_Rocks), 0x1C, 0x20);
			InitTaskQueuePool(QDust(), (void *)(TexBase() + POOL_Dust), 0x18, 0x20);
			MasterNode *m = (MasterNode *)AddTaskToQueue(QMaster(), game_addr(ORIG_MasterTask));
			Memset32(&m->counter, 0, 0x382);
			Memset32(m->index, 0xFFFFFFFF, 0x258);
			m->voice = ClaimVoiceSlot(SOUND_Voice(), 1, 0x80);
			DecodeModelPrimLayout(game_addr(MODEL_Layout), m->layout, 0x90);
			GetDefaultEffectPosition(Caster(), m->pos);
			m->pos[1] = 0;
			m->colour = *(const uint32_t *)(Caster() + 0x2C);
			m->next = m->marks;
		}
		int a0;
		if (rn->started)
		{
			a0 = ExecuteTaskQueue(QMaster());
			ExecuteTaskQueue(QRocks());
			ExecuteTaskQueue(QDust());
		}
		else a0 = (int)n;
		if (rn->started && a0 == 0)
		{
			for (int i = 0; i < 3; i++)
			{
				uint16_t &w = *(uint16_t *)Entity(i);
				w ^= (uint16_t)((PartyWord(i) ^ w) & 4);
			}
			*(uint16_t *)Caster() &= 0xFFF3;
			return TASK_END;
		}
		rn->counter++;
		return 0;
	}
}

	void register_mag301_gargantua_burrow()
	{
		register_port(burrow301::ORIG_RootTask, (void *)burrow301::RootTask, "301 RootTask", 301);
		register_port(burrow301::ORIG_MasterTask, (void *)burrow301::MasterTask, "301 MasterTask", 301);
		register_port(burrow301::ORIG_RockTask, (void *)burrow301::RockTask, "301 RockTask", 301);
		register_port(burrow301::ORIG_DustTask, (void *)burrow301::DustTask, "301 DustTask", 301);
	}
}

