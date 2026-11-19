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
#include <stddef.h>

namespace ff8fx
{
namespace destruct298
{
	using namespace eng;
	using namespace magc;

	inline uint8_t *&Caster() { return var<uint8_t *>(game_addr(game::MAG_298_Bgh251f2Destruction_CasterEntity)); }
	inline TaskQueue *QScript() { return (TaskQueue *)game_addr(game::MAG_298_Bgh251f2Destruction_ScriptQueue); }
	inline TaskQueue *QSprite() { return (TaskQueue *)game_addr(game::MAG_298_Bgh251f2Destruction_SpriteQueue); }
	inline TaskQueue *QLayout() { return (TaskQueue *)game_addr(game::MAG_298_Bgh251f2Destruction_LayoutQueue); }
	inline TaskQueue *QMaster() { return (TaskQueue *)game_addr(game::MAG_298_Bgh251f2Destruction_MasterQueue); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_298_Bgh251f2Destruction_PacketCursor)); }
	inline uint32_t &LcgSeed() { return var<uint32_t>(game_addr(game::MAG_298_Bgh251f2Destruction_LcgSeed)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::MAG_298_Bgh251f2Destruction_MagicBufferBase)); }
	inline uint32_t &FrameCursor() { return var<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)); }
	inline uint32_t &BoneWorkspace() { return var<uint32_t>(game_addr(game::g_BattleVertexWorkspace)); }
	inline uint32_t RenderOT(uint32_t off) { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + off; }

	static constexpr game::Id ORIG_RootTask = game::MAG_298_Bgh251f2Destruction_RootTask;
	static constexpr game::Id ORIG_MasterTask = game::MAG_298_Bgh251f2Destruction_MasterTask;
	static constexpr game::Id ORIG_ScriptTask = game::MAG_298_Bgh251f2Destruction_ScriptTask;
	static constexpr game::Id ORIG_LayoutTask = game::MAG_298_Bgh251f2Destruction_LayoutTask;
	static constexpr game::Id ORIG_SpriteTask = game::MAG_298_Bgh251f2Destruction_SpriteTask;
	static constexpr game::Id ORIG_PrimCallback = game::MAG_298_Bgh251f2Destruction_PrimCallback;
	static const uint32_t POOL_Sprite = 0x654, POOL_Layout = 0xD54, POOL_Master = 0x1794;
	static const uint32_t POSE = 0x17B0;
	static const uint32_t MORPH = 0x261D0;
	static const uint32_t STREAM_State = 0xA188, STREAM_Dest = 0xA1D0, STREAM_File = 0x168;
	static const uint32_t ARENA_A = 0xA1D0, ARENA_B = 0x181D0;
	static constexpr game::Id LAYOUTS = game::unk_1052278;
	static constexpr game::Id SEQUENCES = game::unk_104ED24;
	static constexpr game::Id SEQ_Mark0 = game::MAG_298_Bgh251f2Destruction_SeqMark0, SEQ_Mark1 = game::unk_104E74C;
	static constexpr game::Id FRAG_Models[3] = { game::unk_104E464, game::unk_104E55C, game::unk_104E654 };
	static constexpr game::Id SIN_COS = game::unk_10523E0;
	static constexpr game::Id SQRT_Table = game::unk_1056360;
	static const void *SOUND_Voice() { return (const void *)game_addr(game::MAG_298_Bgh251f2Destruction_SoundVoice); }
	static const void *SOUND_Start() { return (const void *)game_addr(game::MAG_298_Bgh251f2Destruction_SoundStart); }
	static constexpr game::Id SCRIPTS[3] = { game::unk_1056754, game::unk_10567E8, game::unk_1056560 };

	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline void RotY(int32_t angle, Mat4x3 *out) { fn<void (__cdecl *)(int32_t, Mat4x3 *)>(game_addr(game::Effect_RotationYMatrix))(angle, out); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void RotationFromAnglesYXZ(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::MAG_063_RotationFromAnglesYXZ))(angles, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
	inline void MatrixMultiply3x3(Mat4x3 *a, const void *b) { fn<void (__cdecl *)(Mat4x3 *, const void *)>(game_addr(game::MatrixMultiply3x3))(a, b); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void Matrix3x3MultiplyPSX(const void *a, const void *b, void *out) { fn<void (__cdecl *)(const void *, const void *, void *)>(game_addr(game::Matrix3x3_MultiplyPSX))(a, b, out); }
	inline void GteMVMVA_RotV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0))(); }
	inline uint32_t RenderPrimModel70(void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::MAG_070_RenderPrimModel))(h, ot, mode, cursor); }
	inline void PoseBones(uint8_t *anim_header, const void *pose) { fn<void (__cdecl *)(uint8_t *, const void *)>(game_addr(game::Battle_BuildBoneMatricesWithPoseOffsets))(anim_header, pose); }
	inline uint32_t DrawShadow(uint8_t *entity, uint32_t ot, int32_t shift, uint32_t cursor) { return fn<uint32_t (__cdecl *)(uint8_t *, uint32_t, int32_t, uint32_t)>(game_addr(game::BattleEntity_DrawShadow))(entity, ot, shift, cursor); }
	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t flag, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, flag, out); }
	inline void ReleaseVoiceSlot(uint32_t slot) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::BdSound_ReleaseVoiceSlot))(slot); }
	inline void BlendColour(const void *a, const void *b, int32_t wa, int32_t wb, void *out) { fn<void (__cdecl *)(const void *, const void *, int32_t, int32_t, void *)>(game_addr(game::ColourLerp))(a, b, wa, wb, out); }
	inline int32_t SharedLcg() { return fn<int32_t (__cdecl *)()>(game_addr(game::Effect_SharedLcg_Rand))(); }
	inline void MusicVolume(int32_t v) { fn<void (__cdecl *)(int32_t)>(game_addr(game::Battle_SetMusicVolumeUnlessEncounterFlag2))(v); }
	inline void StreamLoad(int32_t id, uint32_t dst, int32_t n) { fn<void (__cdecl *)(int32_t, uint32_t, int32_t)>(game_addr(game::Stream_QueueLoad))(id, dst, n); }
	inline void StreamPump() { fn<void (__cdecl *)()>(game_addr(game::Stream_Pump))(); }
	inline int32_t StreamDone(int32_t a) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::BattleFile_PreLoad_IsLoaded))(a); }
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
	inline void GteLoadIRBytes(const void *src) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadIR123FromBytes))(src); }
	inline void GteStoreIRBytes(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreIR123LowBytes))(dst); }
	inline void GteSetRotDiag32(const void *v3) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_SetRotDiagonal_v32))(v3); }
	inline void GteLoadIR32_45DBF0(const void *v3) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadIR123_32b))(v3); }
	inline void GteLoadIR32_45E090(const void *v3) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadIR123_32))(v3); }
	inline void GteOP() { fn<void (__cdecl *)()>(game_addr(game::GTE_OP_v2))(); }
	inline void GteSQR() { fn<void (__cdecl *)()>(game_addr(game::GTE_SQR))(); }
	inline void GteSumMAC123(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_SumMAC123))(dst); }
	inline void GteSetLZCS(int32_t v) { fn<void (__cdecl *)(int32_t)>(game_addr(game::GTE_SetLZCS))(v); }
	inline void GteReadLZCR(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadLZCR))(dst); }

#pragma pack(push, 1)
	struct PoseEntry
	{
		int16_t pos[3];
		int16_t vis;
		int16_t rot[3];
		int16_t pad;
	};
	struct Fragment
	{
		int16_t pos[3];
		int16_t life;
		int16_t vel[3];
		int16_t scale;
		int16_t angle[2];
		int16_t spin[2];
		uint32_t model;
		uint8_t uv[12][0xC];
	};
	struct Mark
	{
		int16_t pos[3];
		int16_t life;
		int8_t vel[3];
		uint8_t type;
	};
	struct RootNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t pad0E;
		uint8_t started;
		uint32_t arena;
	};
	struct MasterNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		uint32_t voice;
		uint32_t at[2];
		PoseEntry pose[45];
		Fragment frags[0xC0];
		Mark marks[0x80];
		Fragment *next_frag;
		Mark *next_mark;
	};
	struct ScriptNode
	{
		TaskNode hdr;
		PoseEntry *cur;
		const int16_t *ip;
		int16_t vel[3];
		int16_t pad1A;
		int16_t avel[3];
		int16_t pad22;
	};
	struct LayoutNode
	{
		TaskNode hdr;
		int16_t pos[3];
		int16_t scale;
		int16_t turn;
		int16_t depth;
		uint8_t layout[0x130];
	};
	struct SpriteNode
	{
		TaskNode hdr;
		int16_t pos[3];
		int16_t size;
		uint8_t frame;
		uint8_t seq;
		int8_t vel[3];
		int8_t acc[2];
		int8_t pad1B;
	};
	struct PrimArg
	{
		Mat4x3 m;
		int32_t scale[3];
		int32_t scaled;
		int32_t depth;
		uint32_t morph;
	};
#pragma pack(pop)
	static_assert(sizeof(PoseEntry) == 0x10 && sizeof(Fragment) == 0xAC && sizeof(Mark) == 0xC, "BGH251F2 destruction records");
	static_assert(sizeof(RootNode) == 0x14 && sizeof(MasterNode) == 0x89F4 && sizeof(ScriptNode) == 0x24, "BGH251F2 destruction nodes");
	static_assert(sizeof(LayoutNode) == 0x148 && sizeof(SpriteNode) == 0x1C && sizeof(PrimArg) == 0x38, "BGH251F2 destruction nodes");
	static_assert(offsetof(MasterNode, pose) == 0x1C && offsetof(MasterNode, frags) == 0x2EC && offsetof(MasterNode, marks) == 0x83EC, "BGH251F2 destruction master");

	template<typename T> static inline T &F(uint8_t *s, uint32_t off) { return *(T *)(s + off); }
}
}

namespace ff8fx
{
namespace destruct298
{
	static int32_t ModLcg()
	{
		const uint32_t v = (LcgSeed() * 125 + 0xE) & 0x7FFF;
		LcgSeed() = v;
		return (int32_t)v;
	}

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
		if (r->flags & 0x40000) RotationFromAnglesYXZ(r->rot, &m);
		else RotationFromAngles(r->rot, &m);
		int16_t off[4];
		if (arg->scaled != 0)
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
		off[3] = 0;
		if (arg->scaled != 0) Scale3DMatrix(&m, arg->scale);
		int32_t t0, t1, t2;
		if (r->flags & 0x200)
		{
			t0 = off[0];
			t1 = off[1];
			t2 = off[2];
		}
		else
		{
			GteSetRotMatrixCtrl(&arg->m);
			GteLoadV0(off);
			GteMVMVA_RotV0();
			GteReadMAC123(m.t);
			MatrixMultiply(&arg->m, &m);
			t0 = m.t[0];
			t1 = m.t[1];
			t2 = m.t[2];
		}
		m.t[0] = (int32_t)((uint32_t)t0 + (uint32_t)arg->m.t[0]);
		m.t[1] = (int32_t)((uint32_t)t1 + (uint32_t)arg->m.t[1]);
		m.t[2] = (int32_t)((uint32_t)t2 + (uint32_t)arg->m.t[2]);
		if (!(*(const uint32_t *)r->scale == 0x10001000 && r->scale[2] == 0x1000))
		{
			if (r->flags & 0x100)
			{
				const int16_t s[9] = { r->scale[0], 0, 0, 0, r->scale[1], 0, 0, 0, r->scale[2] };
				MatrixMultiply3x3(&m, s);
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
		*(int32_t *)(h + 0xC) = r->a;
		if (r->a != 0)
		{
			*(uint32_t *)(h + 0x1C) = 0x20F0;
			*(uint32_t *)(h + 8) = *(const uint32_t *)r->rgb;
		}
		*(int32_t *)(h + 0x18) = arg->depth;
		PacketCursor() = RenderPrimModel70(h, RenderOT(0x44), 2, PacketCursor());
		FieldFree(0x58);
	}

	static void ProjectGroup(const uint8_t **pp, uint8_t **outp, uint8_t *h, uint8_t key)
	{
		const uint8_t *p = *pp;
		uint8_t *out = *outp;
		const int32_t n = *(const int16_t *)p;
		p += 2;
		if (n != 0)
		{
			uint32_t left = (uint32_t)n;
			do
			{
				GteLoadV0u(p);
				GteRTPS();
				p += 6;
				GteReadSXY2(h + 0x1C);
				GteReadOTZ(h + 0x20);
				if (F<int16_t>(h, 0x20) <= 0)
					out[6] = 0x10;
				else
				{
					h[0x22] = 0;
					h[0x23] = key;
					const int32_t vx = F<int16_t>(h, 0x1C);
					if (vx < shl32(F<int16_t>(h, 0xC), 3)) h[0x22] = 1;
					else if (vx >= shl32(F<int16_t>(h, 0x10), 3)) h[0x22] = 2;
					const int32_t vy = F<int16_t>(h, 0x1E);
					if (vy < shl32(F<int16_t>(h, 0xE), 3)) h[0x22] |= 4;
					else if (vy >= shl32(F<int16_t>(h, 0x12), 3)) h[0x22] |= 8;
					*(uint32_t *)out = F<uint32_t>(h, 0x1C);
					*(uint32_t *)(out + 4) = F<uint32_t>(h, 0x20);
				}
				out += 8;
			} while (--left != 0);
		}
		*pp = p;
		*outp = out;
	}

	static uint32_t DrawPolys(uint8_t *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		uint8_t *pk = (uint8_t *)cursor;
		const uint8_t *verts = *(const uint8_t *const *)(h + 4);
		const uint8_t *p = *(const uint8_t *const *)h;
		int16_t c = *(int16_t *)(h + 8);
		*(int16_t *)(h + 8) = (int16_t)(c - 1);
		while (c != 0)
		{
			const uint32_t i0 = *(const uint16_t *)p & 0xFFF, i1 = *(const uint16_t *)(p + 2) & 0xFFF, i2 = *(const uint16_t *)(p + 4) & 0xFFF;
			*(uint32_t *)(h + 0x1C) = *(const uint32_t *)(verts + i0 * 8);
			*(uint32_t *)(h + 0x20) = *(const uint32_t *)(verts + i0 * 8 + 4);
			*(uint32_t *)(h + 0x24) = *(const uint32_t *)(verts + i1 * 8);
			*(uint32_t *)(h + 0x28) = *(const uint32_t *)(verts + i1 * 8 + 4);
			*(uint32_t *)(h + 0x2C) = *(const uint32_t *)(verts + i2 * 8);
			*(uint32_t *)(h + 0x30) = *(const uint32_t *)(verts + i2 * 8 + 4);
			const uint8_t f0 = h[0x22], f1 = h[0x2A], f2 = h[0x32];
			if ((uint8_t)(f0 | f1 | f2) < 0x10 && !(f2 & (uint8_t)(f0 & f1)))
			{
				GteSetSXY012(*(const uint32_t *)(h + 0x1C), *(const uint32_t *)(h + 0x24), *(const uint32_t *)(h + 0x2C));
				GteNCLIP();
				GteReadMAC0(h + 0x3C);
				if (*(const int32_t *)(h + 0x3C) >= 0)
				{
					GteSetSZ123(*(const int16_t *)(h + 0x20), *(const int16_t *)(h + 0x28), *(const int16_t *)(h + 0x30));
					GteAVSZ3();
					*(uint32_t *)pk = 0x7000000;
					*(uint32_t *)(pk + 4) = ((uint32_t)((*(const uint16_t *)(p + 0xE) & 0x200) | 0x2400) << 16) | *(const uint32_t *)(h + 0x14);
					*(uint32_t *)(pk + 8) = *(const uint32_t *)(h + 0x1C);
					*(uint32_t *)(pk + 0x10) = *(const uint32_t *)(h + 0x24);
					*(uint32_t *)(pk + 0x18) = *(const uint32_t *)(h + 0x2C);
					*(uint32_t *)(pk + 0xC) = *(const uint32_t *)(p + 8);
					*(uint32_t *)(pk + 0x14) = *(const uint32_t *)(p + 0xC);
					*(uint16_t *)(pk + 0x16) &= 0xFDFF;
					*(uint16_t *)(pk + 0x1C) = *(const uint16_t *)(p + 6);
					GteReadOTZWord(h + 0x44);
					InsertPrimAutoDepth(ot + (uint32_t)(*(const int32_t *)(h + 0x44) >> (shift & 31)) * 4, pk);
					pk += 0x20;
				}
			}
			p += 0x10;
			c = *(int16_t *)(h + 8);
			*(int16_t *)(h + 8) = (int16_t)(c - 1);
		}
		c = *(int16_t *)(h + 0xA);
		*(int16_t *)(h + 0xA) = (int16_t)(c - 1);
		while (c != 0)
		{
			const uint32_t i0 = *(const uint16_t *)p & 0xFFF, i1 = *(const uint16_t *)(p + 2) & 0xFFF;
			const uint32_t i2 = *(const uint16_t *)(p + 4) & 0xFFF, i3 = *(const uint16_t *)(p + 6) & 0xFFF;
			*(uint32_t *)(h + 0x1C) = *(const uint32_t *)(verts + i0 * 8);
			*(uint32_t *)(h + 0x20) = *(const uint32_t *)(verts + i0 * 8 + 4);
			*(uint32_t *)(h + 0x24) = *(const uint32_t *)(verts + i1 * 8);
			*(uint32_t *)(h + 0x28) = *(const uint32_t *)(verts + i1 * 8 + 4);
			*(uint32_t *)(h + 0x2C) = *(const uint32_t *)(verts + i2 * 8);
			*(uint32_t *)(h + 0x30) = *(const uint32_t *)(verts + i2 * 8 + 4);
			*(uint32_t *)(h + 0x34) = *(const uint32_t *)(verts + i3 * 8);
			*(uint32_t *)(h + 0x38) = *(const uint32_t *)(verts + i3 * 8 + 4);
			const uint8_t f0 = h[0x22], f1 = h[0x2A], f2 = h[0x32], f3 = h[0x3A];
			if ((uint8_t)(f3 | f0 | f1 | f2) < 0x10 && !(f2 & (uint8_t)(f3 & f0 & f1)))
			{
				GteSetSXY012(*(const uint32_t *)(h + 0x1C), *(const uint32_t *)(h + 0x24), *(const uint32_t *)(h + 0x2C));
				GteNCLIP();
				GteReadMAC0(h + 0x3C);
				if (*(const int32_t *)(h + 0x3C) >= 0)
				{
					GteSetSZ0123(*(const int16_t *)(h + 0x20), *(const int16_t *)(h + 0x28), *(const int16_t *)(h + 0x30), *(const int16_t *)(h + 0x38));
					GteAVSZ4();
					*(uint32_t *)pk = 0x9000000;
					*(uint32_t *)(pk + 4) = ((uint32_t)((*(const uint16_t *)(p + 0xE) & 0x200) | 0x2C00) << 16) | *(const uint32_t *)(h + 0x14);
					*(uint32_t *)(pk + 8) = *(const uint32_t *)(h + 0x1C);
					*(uint32_t *)(pk + 0x10) = *(const uint32_t *)(h + 0x24);
					*(uint32_t *)(pk + 0x18) = *(const uint32_t *)(h + 0x2C);
					*(uint32_t *)(pk + 0x20) = *(const uint32_t *)(h + 0x34);
					*(uint32_t *)(pk + 0xC) = *(const uint32_t *)(p + 8);
					*(uint32_t *)(pk + 0x14) = *(const uint32_t *)(p + 0xC);
					*(uint16_t *)(pk + 0x1C) = *(const uint16_t *)(p + 0x10);
					*(uint16_t *)(pk + 0x16) &= 0xFDFF;
					*(uint16_t *)(pk + 0x24) = *(const uint16_t *)(p + 0x12);
					GteReadOTZWord(h + 0x44);
					InsertPrimAutoDepth(ot + (uint32_t)(*(const int32_t *)(h + 0x44) >> (shift & 31)) * 4, pk);
					pk += 0x28;
				}
			}
			p += 0x14;
			c = *(int16_t *)(h + 0xA);
			*(int16_t *)(h + 0xA) = (int16_t)(c - 1);
		}
		return (uint32_t)pk;
	}

	static uint32_t DrawModel(const uint8_t *model, uint8_t *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		const uint32_t *objs = *(const uint32_t *const *)(model + 4);
		const uint8_t *bones = *(const uint8_t *const *)model + 0x10;
		const int32_t count = (int32_t)objs[0];
		if (count <= 0) return cursor;
		for (int32_t i = 0; i < count; i++)
		{
			const uint8_t *p = *(const uint8_t *const *)(model + 4) + objs[1 + i];
			if (!(*(const uint32_t *)(h + 0x18) & (1u << (i & 31)))) continue;
			uint8_t *out = *(uint8_t **)(h + 4);
			const int32_t groups = *(const int16_t *)p;
			p += 2;
			if (groups > 0)
			{
				for (int32_t g = groups; g != 0; g--)
				{
					const int32_t bone = *(const int16_t *)p;
					p += 2;
					const PoseEntry *pe = *(const PoseEntry *const *)(h + 0x4C) + bone;
					if (pe->vis < 2)
					{
						const Mat4x3 *bm = (const Mat4x3 *)(bones + mul32(bone, 0x30) + 0x10);
						GteSetRotMatrixCtrl(bm);
						GteSetTransVectorCtrl(bm);
						ProjectGroup(&p, &out, h, *(const uint8_t *)&pe->vis);
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
			*(uint16_t *)(h + 8) = *(const uint16_t *)p;
			*(uint16_t *)(h + 0xA) = *(const uint16_t *)(p + 2);
			*(const uint8_t **)h = p + 0xC;
			cursor = DrawPolys(h, ot, shift, cursor);
		}
		return cursor;
	}

	static void CasterDraw(uint8_t *entity, PoseEntry *pose)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x98);
		uint8_t *anim = entity + 0x60;
		FrameCursor() = DrawShadow(entity, RenderOT(0x4040), 0x10, FrameCursor());
		PoseBones(anim, pose);
		uint8_t *skel = **(uint8_t ***)(entity + 0x64);
		if (skel[0] != 0)
		{
			for (int32_t i = 0; i < skel[0]; i++)
			{
				Mat4x3 *bm = (Mat4x3 *)(skel + 0x20 + i * 0x30);
				ComposeAffineTransform((const Mat4x3 *)(entity + 0x40), bm, bm);
				bm->t[0] = (int32_t)((uint32_t)bm->t[0] + (uint32_t)(int32_t)pose[i].pos[0]);
				bm->t[1] = (int32_t)((uint32_t)bm->t[1] + (uint32_t)(int32_t)pose[i].pos[1]);
				bm->t[2] = (int32_t)((uint32_t)bm->t[2] + (uint32_t)(int32_t)pose[i].pos[2]);
				ComposeAffineTransform(&Camera(), bm, bm);
			}
		}
		*(PoseEntry **)(h + 0x4C) = pose;
		*(uint32_t *)(h + 4) = BoneWorkspace();
		*(uint16_t *)(h + 0xC) = 0;
		*(uint16_t *)(h + 0xE) = 0;
		*(uint16_t *)(h + 0x10) = 0x140;
		*(uint16_t *)(h + 0x12) = 0xD8;
		*(uint32_t *)(h + 0x14) = *(const uint32_t *)(entity + 0x28);
		*(uint32_t *)(h + 0x18) = *(const uint32_t *)(entity + 0x7C);
		FrameCursor() = DrawModel(*(const uint8_t *const *)(anim + 4), h, RenderOT(0x44), 0, FrameCursor());
		BuildBoneMatricesFromPose(anim);
		FieldFree(0x98);
	}

	static void WorldGroup(const uint8_t **pp, uint8_t **outp)
	{
		const uint8_t *p = *pp;
		uint8_t *out = *outp;
		const int32_t n = *(const int16_t *)p;
		p += 2;
		if (n != 0)
		{
			uint32_t left = (uint32_t)n;
			do
			{
				GteLoadV0u(p);
				GteMVMVA_RotV0Tr();
				*(uint16_t *)(out + 6) = 0;
				out += 8;
				p += 6;
				GteStoreIR123Minus8(out);
			} while (--left != 0);
		}
		*pp = p;
		*outp = out;
	}

	static Fragment *NewFragment(MasterNode *m)
	{
		Fragment *f = m->next_frag;
		if (f->life != 0)
		{
			f = m->frags;
			if (m->frags[0].life != 0)
			{
				int32_t n = 0xC0;
				do
				{
					f++;
					if (--n == 0) return nullptr;
				} while (f->life != 0);
			}
		}
		m->next_frag = (uintptr_t)f >= (uintptr_t)m + 0x8340 ? m->frags : f + 1;
		const int32_t k = (ModLcg() * 3) >> 15;
		if ((uint32_t)k <= 2) f->model = game_addr(FRAG_Models[k]);
		return f;
	}

	static Mark *NewMark(MasterNode *m)
	{
		Mark *k = m->next_mark;
		if (k->life != 0)
		{
			k = m->marks;
			if (m->marks[0].life != 0)
			{
				int32_t n = 0x80;
				do
				{
					k++;
					if (--n == 0) return nullptr;
				} while (k->life != 0);
			}
		}
		m->next_mark = (uintptr_t)k >= (uintptr_t)m + 0x89E0 ? m->marks : k + 1;
		return k;
	}

	static void FragmentInit(uint8_t *s, Fragment *f)
	{
		*(uint32_t *)f->pos = F<uint32_t>(s, 0x34);
		*(uint32_t *)&f->pos[2] = F<uint32_t>(s, 0x38);
		f->life = (int16_t)((SharedLcg() >> 0xC) + 0x10);
		const int32_t x0 = F<int16_t>(s, 0x34), y0 = F<int16_t>(s, 0x36), z0 = F<int16_t>(s, 0x38);
		F<int32_t>(s, 0x64) = x0 - F<int16_t>(s, 0x3C);
		F<int32_t>(s, 0x68) = y0 - F<int16_t>(s, 0x3E);
		F<int32_t>(s, 0x6C) = z0 - F<int16_t>(s, 0x40);
		F<int32_t>(s, 0x74) = F<int16_t>(s, 0x44) - x0;
		F<int32_t>(s, 0x78) = F<int16_t>(s, 0x46) - y0;
		F<int32_t>(s, 0x7C) = F<int16_t>(s, 0x48) - F<int16_t>(s, 0x38);
		GteSetRotDiag32(s + 0x64);
		GteLoadIR32_45DBF0(s + 0x74);
		GteOP();
		GteReadMAC123((int32_t *)(s + 0x84));
		int32_t big = F<int32_t>(s, 0x84);
		if (big < 0) big = -big;
		int32_t a = F<int32_t>(s, 0x88);
		if (a < 0) a = -a;
		if (a > big) big = a;
		a = F<int32_t>(s, 0x8C);
		if (a < 0) a = -a;
		if (a > big) big = a;
		GteSetLZCS(big);
		f->angle[0] = (int16_t)ModLcg();
		f->angle[1] = (int16_t)ModLcg();
		f->spin[0] = (int16_t)((ModLcg() >> 5) - 0x200);
		f->spin[1] = (int16_t)((ModLcg() >> 5) - 0x200);
		int32_t lz;
		GteReadLZCR(&lz);
		if (lz < 0x12)
		{
			const int32_t sh = 0x12 - lz;
			F<int32_t>(s, 0x84) >>= sh;
			F<int32_t>(s, 0x88) >>= sh;
			F<int32_t>(s, 0x8C) >>= sh;
		}
		GteLoadIR32_45E090(s + 0x84);
		GteSQR();
		int32_t sum;
		GteSumMAC123(&sum);
		GteSetLZCS(sum);
		int32_t lz2;
		GteReadLZCR(&lz2);
		lz2 &= ~1;
		const int32_t shift = (0x1F - lz2) >> 1;
		const int32_t e = lz2 - 0x18;
		const int32_t idx = e > 0 ? shl32(sum, e) : sum >> ((0x18 - lz2) & 31);
		const int32_t norm = (int32_t)((uint32_t)*(const uint16_t *)(game_addr(SQRT_Table) + idx * 2) << (shift & 31)) >> 0xC;
		const int32_t speed = F<int32_t>(s, 0x1C);
		f->vel[0] = (int16_t)(mul32(speed, F<int32_t>(s, 0x84)) / norm);
		f->vel[2] = (int16_t)(mul32(speed, F<int32_t>(s, 0x8C)) / norm);
		f->vel[1] = (int16_t)(mul32(F<int32_t>(s, 0x88), speed) / norm);
		Mark *k = NewMark(F<MasterNode *>(s, 0x30));
		if (k)
		{
			*(uint32_t *)k->pos = F<uint32_t>(s, 0x34);
			*(uint32_t *)&k->pos[2] = F<uint32_t>(s, 0x38);
			if (ModLcg() > 0x2EE0)
			{
				k->type = 0;
				k->life = 0xC;
				k->vel[0] = (int8_t)((uint16_t)f->vel[0] >> 2);
				k->vel[1] = (int8_t)((uint16_t)f->vel[1] >> 2);
				k->vel[2] = (int8_t)((uint16_t)f->vel[2] >> 2);
			}
			else
			{
				k->type = 1;
				k->life = 0xB;
				k->vel[0] = (int8_t)f->vel[0];
				k->vel[1] = (int8_t)f->vel[1];
				k->vel[2] = (int8_t)f->vel[2];
			}
		}
		f->vel[1] = (int16_t)(f->vel[1] - (int16_t)(F<uint16_t>(s, 0x1C) << 1));
		f->scale = (int16_t)((mul32(SharedLcg(), F<int32_t>(s, 0x20)) >> 0xF) + F<int16_t>(s, 0x20));
	}

	enum { POLY_NEXT = 0, POLY_STOP = 1 };
	static int ShatterPoly(uint8_t *s, const uint8_t *uv0, const uint8_t *uv1, const uint8_t *word)
	{
		if (F<int32_t>(s, 0x18) == 0)
		{
			if (SharedLcg() > F<int32_t>(s, 0x24)) return POLY_NEXT;
			const int32_t left = F<int32_t>(s, 0x14);
			F<int32_t>(s, 0x14) = left - 1;
			if (left <= 0) return POLY_STOP;
			F<int32_t>(s, 0x18) = 0xC;
			Fragment *f = NewFragment(F<MasterNode *>(s, 0x30));
			F<Fragment *>(s, 0x28) = f;
			if (!f)
			{
				F<int32_t>(s, 0x14) = 0;
				return POLY_STOP;
			}
			F<uint8_t *>(s, 0x2C) = f->uv[0];
			FragmentInit(s, f);
		}
		uint8_t *w = F<uint8_t *>(s, 0x2C);
		F<int32_t>(s, 0x18)--;
		F<uint8_t *>(s, 0x2C) = w + 0xC;
		*(uint32_t *)w = *(const uint32_t *)uv0;
		*(uint32_t *)(w + 4) = *(const uint32_t *)uv1;
		*(uint16_t *)(w + 8) = *(const uint16_t *)word;
		return POLY_NEXT;
	}

	static inline void CopyWorldVertex(uint8_t *s, uint32_t off, const uint8_t *verts, uint16_t index)
	{
		const uint8_t *v = verts + (uint32_t)(index & 0xFFF) * 8;
		F<uint32_t>(s, off) = *(const uint32_t *)v;
		F<uint32_t>(s, off + 4) = *(const uint32_t *)(v + 4);
	}

	static void ShatterPolys(uint8_t *s)
	{
		const uint8_t *verts = F<uint8_t *>(s, 4);
		const uint8_t *p = F<uint8_t *>(s, 0);
		uint16_t k = F<uint16_t>(s, 8);
		F<uint16_t>(s, 8) = (uint16_t)(k - 1);
		if (k != 0)
		{
			do
			{
				CopyWorldVertex(s, 0x34, verts, *(const uint16_t *)p);
				CopyWorldVertex(s, 0x3C, verts, *(const uint16_t *)(p + 2));
				CopyWorldVertex(s, 0x44, verts, *(const uint16_t *)(p + 4));
				if ((uint8_t)(s[0x42] | s[0x4A] | s[0x3A]) == 0)
					if (ShatterPoly(s, p + 8, p + 0xC, p + 6) == POLY_STOP) return;
				k = F<uint16_t>(s, 8);
				p += 0x10;
				F<uint16_t>(s, 8) = (uint16_t)(k - 1);
			} while (k != 0);
		}
		k = F<uint16_t>(s, 0xA);
		F<uint16_t>(s, 0xA) = (uint16_t)(k - 1);
		if (k == 0) return;
		p += 4;
		do
		{
			CopyWorldVertex(s, 0x34, verts, *(const uint16_t *)(p - 4));
			CopyWorldVertex(s, 0x3C, verts, *(const uint16_t *)(p - 2));
			CopyWorldVertex(s, 0x44, verts, *(const uint16_t *)p);
			CopyWorldVertex(s, 0x4C, verts, *(const uint16_t *)(p + 2));
			if ((uint8_t)(s[0x42] | s[0x4A] | s[0x3A] | s[0x52]) == 0)
				if (ShatterPoly(s, p + 4, p + 8, p + 0xC) == POLY_STOP) return;
			k = F<uint16_t>(s, 0xA);
			p += 0x14;
			F<uint16_t>(s, 0xA) = (uint16_t)(k - 1);
		} while (k != 0);
	}

	static void ShatterModel(const uint8_t *model, uint8_t *s)
	{
		const uint32_t *table = *(const uint32_t *const *)(model + 4);
		const uint8_t *bones = *(const uint8_t *const *)model + 0x10;
		const int32_t count = (int32_t)table[0];
		const uint32_t *offs = table + 1;
		if (count <= 0) return;
		for (int32_t i = 0; i < count; i++)
		{
			const uint8_t *p = *(const uint8_t *const *)(model + 4) + *offs++;
			if (!(F<uint32_t>(s, 0x10) & (1u << (i & 31)))) continue;
			for (;;)
			{
				uint8_t *out = F<uint8_t *>(s, 4);
				int32_t any = 0;
				const int32_t groups = *(const int16_t *)p;
				p += 2;
				if (groups <= 0) break;
				for (int32_t g = groups; g != 0; g--)
				{
					const int32_t bone = *(const int16_t *)p;
					p += 2;
					const uint8_t *b = bones + bone * 0x30;
					if (*(const int16_t *)(b + 0x22) != 0)
					{
						const Mat4x3 *bm = (const Mat4x3 *)(b + 0x10);
						GteSetRotMatrixCtrl(bm);
						GteSetTransVectorCtrl(bm);
						WorldGroup(&p, &out);
						any = 1;
					}
					else
					{
						const int32_t n = *(const int16_t *)p;
						p = p + n * 6 + 2;
						for (int32_t k = n; k != 0; k--)
						{
							*(uint16_t *)(out + 6) = 1;
							out += 8;
						}
					}
				}
				if (!any) break;
				p = (const uint8_t *)(((uint32_t)p + 3) & ~3u);
				F<uint16_t>(s, 8) = *(const uint16_t *)p;
				F<uint16_t>(s, 0xA) = *(const uint16_t *)(p + 2);
				p += 0xC;
				F<const uint8_t *>(s, 0) = p;
				ShatterPolys(s);
				if (F<int32_t>(s, 0x14) <= 0) return;
			}
		}
	}

	static const int16_t *Shatter(const int16_t *ip, uint8_t *entity, PoseEntry *pose)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		F<int32_t>(h, 0x38) = 0;
		F<uint32_t>(h, 0x50) = TexBase() + POOL_Master;
		uint8_t *anim = entity + 0x60;
		F<uint32_t>(h, 0x24) = BoneWorkspace();
		F<uint32_t>(h, 0x2C) = *(const uint32_t *)(entity + 0x28);
		F<uint32_t>(h, 0x30) = *(const uint32_t *)(entity + 0x7C);
		PoseBones(anim, pose);
		uint8_t *skel = **(uint8_t ***)(anim + 4);
		if (skel[0] != 0)
		{
			for (int32_t i = 0; i < skel[0]; i++)
			{
				Mat4x3 *bm = (Mat4x3 *)(skel + 0x20 + i * 0x30);
				ComposeAffineTransform((const Mat4x3 *)(entity + 0x40), bm, bm);
				if (pose[i].pad != 0)
				{
					BuildRotationMatrixFromAngles(pose[i].rot, (Mat4x3 *)h);
					Matrix3x3MultiplyPSX(h, bm, bm);
				}
				bm->t[0] = (int32_t)((uint32_t)bm->t[0] + (uint32_t)(int32_t)pose[i].pos[0]);
				bm->t[1] = (int32_t)((uint32_t)bm->t[1] + (uint32_t)(int32_t)pose[i].pos[1]);
				bm->t[2] = (int32_t)((uint32_t)bm->t[2] + (uint32_t)(int32_t)pose[i].pos[2]);
			}
		}
		skel = **(uint8_t ***)(anim + 4);
		for (uint32_t n = skel[0], k = 0; n != 0; n--, k++) *(int16_t *)(skel + 0x32 + k * 0x30) = 0;
		int32_t id = *ip++;
		while (id != -1)
		{
			*(int16_t *)(skel + id * 0x30 + 0x32) = 1;
			ip++;
			id = ip[-1];
		}
		F<int32_t>(h, 0x34) = ip[0];
		F<int32_t>(h, 0x3C) = ip[1];
		F<int32_t>(h, 0x40) = ip[2];
		F<int32_t>(h, 0x44) = ip[3];
		ip += 4;
		ShatterModel(*(const uint8_t *const *)(anim + 4), h + 0x20);
		BuildBoneMatricesFromPose(anim);
		if (F<int32_t>(h, 0x38) > 0)
		{
			F<Fragment *>(h, 0x48)->life = 0;
			F<MasterNode *>(h, 0x50)->next_frag = F<Fragment *>(h, 0x48);
		}
		FieldFree(0xB4);
		return ip;
	}

	static void RotXScaled(int32_t a, int32_t s, uint8_t *out)
	{
		*(uint32_t *)out = 0;
		*(uint32_t *)(out + 4) = 0;
		*(uint32_t *)(out + 8) = 0;
		*(uint32_t *)(out + 0xC) = 0;
		*(uint32_t *)(out + 0x10) = 0;
		const int16_t *e = (const int16_t *)(game_addr(SIN_COS) + (a & 0xFFF) * 4);
		const int32_t s16 = (int16_t)s;
		const int32_t c = mul32(e[0], s16) >> 0xC;
		const int32_t d = mul32(e[1], s16) >> 0xC;
		*(int16_t *)out = (int16_t)s;
		*(int16_t *)(out + 0xE) = (int16_t)-c;
		*(int16_t *)(out + 8) = (int16_t)d;
		*(int16_t *)(out + 0xA) = (int16_t)c;
		*(int16_t *)(out + 0x10) = (int16_t)d;
	}

	static void RotYTable(int32_t a, uint8_t *out)
	{
		*(uint32_t *)out = 0;
		*(uint32_t *)(out + 4) = 0;
		*(uint32_t *)(out + 8) = 0;
		*(uint32_t *)(out + 0xC) = 0;
		*(uint32_t *)(out + 0x10) = 0;
		const int16_t *e = (const int16_t *)(game_addr(SIN_COS) + (a & 0xFFF) * 4);
		const int16_t c = e[0], d = e[1];
		*(int16_t *)(out + 0xC) = c;
		*(int16_t *)out = d;
		*(int16_t *)(out + 8) = 0x1000;
		*(int16_t *)(out + 4) = (int16_t)-c;
		*(int16_t *)(out + 0x10) = d;
	}

	static void FragRotation(int32_t ax, int32_t ay, int32_t s, uint8_t *out)
	{
		uint8_t ry[0x20], rx[0x20];
		RotXScaled(ax, s, rx);
		RotYTable(ay, ry);
		GteSetRotMatrixCtrl((const Mat4x3 *)rx);
		GteLoadIRColumn(ry);
		GteMVMVA_RotIR();
		GteStoreIRColumn(out);
		GteLoadIRColumn(ry + 2);
		GteMVMVA_RotIR();
		GteStoreIRColumn(out + 2);
		GteLoadIRColumn(ry + 4);
		GteMVMVA_RotIR();
		GteStoreIRColumn(out + 4);
	}

	static uint32_t FragPolys(uint8_t *s, uint32_t ot, int32_t mode, uint32_t cursor)
	{
		uint8_t *pk = (uint8_t *)cursor;
		const uint8_t *p = F<uint8_t *>(s, 0xC);
		const int32_t count = *(const int32_t *)p;
		p += 4;
		const uint8_t *verts = F<uint8_t *>(s, 4);
		F<const uint8_t *>(s, 0xC) = p;
		if (count > 0)
		{
			int32_t left = count;
			do
			{
				GteLoadV012(verts + *(const uint16_t *)(p + 4) * 4, verts + *(const uint16_t *)(p + 6) * 4, verts + *(const uint16_t *)(p + 8) * 4);
				GteRTPT();
				*(uint32_t *)(pk + 4) = F<uint32_t>(s, 0x40);
				GteReadFLAG(s + 0x1C);
				if (!(F<uint32_t>(s, 0x1C) & 0x60000))
				{
					GteNCLIP();
					const uint8_t *w = F<uint8_t *>(s, 0x44);
					*(uint32_t *)(pk + 0xC) = *(const uint32_t *)w;
					*(uint32_t *)(pk + 0x14) = *(const uint32_t *)(w + 4);
					*(uint16_t *)(pk + 0x1C) = *(const uint16_t *)(w + 8);
					GteReadMAC0(s + 0x10);
					if (F<int32_t>(s, 0x10) >= 0)
					{
						GteReadSXY012Split(pk + 8, pk + 0x10, pk + 0x18);
						GteAVSZ3();
						uint32_t clip = 0;
						int16_t v = *(const int16_t *)(pk + 8);
						if (v < 0 || v > 0xA00) clip = 1;
						v = *(const int16_t *)(pk + 0x10);
						if (v < 0 || v > 0xA00) clip |= 2;
						v = *(const int16_t *)(pk + 0x18);
						if (v < 0 || v > 0xA00) clip |= 4;
						v = *(const int16_t *)(pk + 0xA);
						if (v < 0 || v > 0x6C0) clip |= 0x10;
						v = *(const int16_t *)(pk + 0x12);
						if (v < 0 || v > 0x6C0) clip |= 0x20;
						v = *(const int16_t *)(pk + 0x1A);
						if (v < 0 || v > 0x6C0) clip |= 0x40;
						if ((clip & 7) != 7 && (clip & 0x70) != 0x70)
						{
							GteReadOTZWord(s + 0x18);
							*(uint32_t *)pk = 0x7000000;
							const uint32_t bucket = ot + (uint32_t)(F<int32_t>(s, 0x18) >> (mode & 31)) * 4;
							if (F<int32_t>(s, 8) == 0x1000)
							{
								pk[7] = 0x24;
								InsertPrimAutoDepth(bucket, pk);
								pk += 0x20;
							}
							else
							{
								pk[7] = 0x26;
								InsertPrimAutoDepth(bucket, pk);
								uint8_t *flat = pk + 0x20;
								GteReadSXY012Split(flat + 8, flat + 0xC, flat + 0x10);
								*(uint32_t *)(flat + 4) = F<uint32_t>(s, 0x40);
								*(uint32_t *)flat = 0x4000000;
								flat[7] = 0x22;
								InsertPrimAutoDepth(bucket, flat);
								uint8_t *dm = pk + 0x34;
								*(uint32_t *)dm = 0x1000000;
								*(uint32_t *)(dm + 4) = 0xE1000240;
								InsertPrimAutoDepth(bucket, dm);
								pk += 0x3C;
							}
						}
					}
				}
				p += 0xC;
				F<uint8_t *>(s, 0x44) += 0xC;
			} while (--left != 0);
		}
		F<const uint8_t *>(s, 0xC) = p;
		return (uint32_t)pk;
	}

	static uint32_t FragDraw(uint8_t *s, uint32_t ot, int32_t mode, uint32_t cursor)
	{
		const uint8_t *model = F<uint8_t *>(s, 0);
		F<const uint8_t *>(s, 4) = model + 8;
		F<const uint8_t *>(s, 0xC) = model + *(const uint32_t *)model;
		s[0x42] = 0x80;
		s[0x41] = 0x80;
		s[0x40] = 0x80;
		GteSetIR0(F<int32_t>(s, 8));
		GteLoadIRBytes(s + 0x40);
		GteGPF();
		GteStoreIRBytes(s + 0x40);
		return FragPolys(s, ot, mode, cursor);
	}

	static int32_t FragFade(int16_t life) { return life < 8 ? shl32(life, 9) : 0x1000; }

	static void FragmentDraw(uint8_t *h, const Fragment *f, int32_t fade)
	{
		FragRotation(f->angle[0], f->angle[1], f->scale, h);
		F<int32_t>(h, 0x14) = f->pos[0];
		F<int32_t>(h, 0x18) = f->pos[1];
		F<int32_t>(h, 0x1C) = f->pos[2];
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
		F<uint32_t>(h, 0x20) = f->model;
		F<int32_t>(h, 0x28) = fade;
		F<const uint8_t *>(h, 0x64) = f->uv[0];
		PacketCursor() = FragDraw(h + 0x20, RenderOT(0x44), 2, PacketCursor());
	}

	static void DrawFragments(MasterNode *m)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x68);
		const Fragment *f = m->frags;
		for (int32_t n = 0xC0; n != 0; n--, f++)
			if (f->life != 0) FragmentDraw(h, f, FragFade(f->life));
		FieldFree(0x68);
	}

	static void MoveFragments(MasterNode *m)
	{
		Fragment *f = m->frags;
		for (int32_t n = 0xC0; n != 0; n--, f++)
		{
			if (f->life == 0) continue;
			f->life--;
			if (f->life == 0)
			{
				m->next_frag = f;
				continue;
			}
			f->angle[0] = (int16_t)(f->angle[0] + f->spin[0]);
			f->angle[1] = (int16_t)(f->angle[1] + f->spin[1]);
			f->pos[0] = (int16_t)(f->pos[0] + f->vel[0]);
			f->pos[2] = (int16_t)(f->pos[2] + f->vel[2]);
			f->pos[1] = (int16_t)(f->pos[1] + f->vel[1]);
			f->vel[1] = (int16_t)(f->vel[1] + 0x14);
			if (f->pos[1] > 0)
			{
				f->vel[1] = (int16_t)(-(int32_t)f->vel[1] >> 1);
				f->spin[0] = (int16_t)-f->spin[0];
				f->spin[1] = (int16_t)-f->spin[1];
			}
		}
	}

	static uint32_t MarkSprites(uint8_t *h, uint32_t ot, int32_t slot, uint32_t cursor)
	{
		uint8_t *pk = (uint8_t *)cursor;
		const int32_t count = F<int32_t>(h, 0x30);
		const uint8_t *r = F<uint8_t *>(h, 0x2C);
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
					InsertPrimAutoDepth(ot + (uint32_t)slot * 4, pk);
					pk += 0x28;
				}
				r += 0x14;
			} while (--left != 0);
		}
		return (uint32_t)pk;
	}

	static void MarkSprite(uint8_t *h, const Mark *k)
	{
		GteSetRotMatrixCtrl(&Camera());
		GteSetTransVectorCtrl(&Camera());
		GteLoadV0(k->pos);
		GteMVMVA_RotV0Tr();
		GteReadMAC123((int32_t *)(h + 0x98));
		F<uint16_t>(h, 0x94) = 0x1000;
		F<uint16_t>(h, 0x8C) = 0x1000;
		F<uint16_t>(h, 0x84) = 0x1000;
		const uint32_t seq = k->type ? game_addr(SEQ_Mark1) : game_addr(SEQ_Mark0);
		F<uint32_t>(h, 0) = seq;
		const uint32_t f = (uint32_t)*(const uint16_t *)(seq + 8) - (uint32_t)(int32_t)k->life;
		const uint32_t frame = seq + *(const uint16_t *)(seq + f * 2 + 8);
		F<uint32_t>(h, 0x2C) = frame;
		F<uint32_t>(h, 0x30) = *(const uint32_t *)frame;
		F<uint32_t>(h, 0x2C) = frame + 4;
		PacketCursor() = MarkSprites(h, RenderOT(0x44), 2, PacketCursor());
	}

	static void MarkDraw(uint8_t *h, Mark *k)
	{
		MarkSprite(h, k);
		k->life--;
		k->pos[0] = (int16_t)(k->pos[0] + (int16_t)(k->vel[0] << 2));
		k->pos[1] = (int16_t)(k->pos[1] + (int16_t)(k->vel[1] << 2));
		k->pos[2] = (int16_t)(k->pos[2] + (int16_t)(k->vel[2] << 2));
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
		Mark *k = m->marks;
		for (int32_t n = 0x80; n != 0; n--, k++)
			if (k->life != 0) MarkDraw(h, k);
		FieldFree(0xB4);
	}

	static void SpriteDraw(const SpriteNode *s)
	{
		TransformCameraByShadowRotation(s->pos, s->size, -0x400);
		uint8_t *h = AllocHeader(0xB4);
		const uint8_t *seq = *(const uint8_t *const *)(game_addr(SEQUENCES) + s->seq * 4);
		*(const uint8_t **)h = seq;
		*(uint16_t *)(h + 4) = s->frame;
		*(uint16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(0x44), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static int32_t SpriteFrames(const SpriteNode *s) { return *(const int16_t *)(*(const uint8_t *const *)(game_addr(SEQUENCES) + s->seq * 4) + 8); }

	static uint32_t __cdecl SpriteTask(TaskNode *n)
	{
		SpriteNode *s = (SpriteNode *)n;
		const int32_t frames = SpriteFrames(s);
		SpriteDraw(s);
		const int8_t vx = s->vel[0];
		s->pos[0] = (int16_t)(s->pos[0] + (int16_t)(vx * 4));
		const int8_t vy = s->vel[1];
		s->pos[1] = (int16_t)(s->pos[1] + (int16_t)(vy * 4));
		s->pos[2] = (int16_t)(s->pos[2] + (int16_t)(s->vel[2] * 4));
		s->vel[0] = (int8_t)(s->acc[0] + vx);
		s->vel[1] = (int8_t)(s->acc[1] * 2 + vy);
		s->frame++;
		return s->frame < frames ? 0 : TASK_END;
	}

	static uint32_t __cdecl LayoutTask(TaskNode *n)
	{
		LayoutNode *L = (LayoutNode *)n;
		PrimArg arg;
		arg.morph = TexBase() + MORPH;
		RotY(L->turn, &arg.m);
		arg.m.t[0] = L->pos[0];
		arg.m.t[1] = L->pos[1];
		arg.m.t[2] = L->pos[2];
		ComposeAffineTransform(&Camera(), &arg.m, &arg.m);
		arg.scale[2] = L->scale;
		arg.scale[1] = L->scale;
		arg.scale[0] = L->scale;
		arg.scaled = 1;
		arg.depth = L->depth;
		return prim::play((prim::Layout *)L->layout, PrimCallback, (int)&arg, 0) ? 0 : TASK_END;
	}

	static const int16_t *SpawnLayout(const int16_t *ip, uint8_t *entity, PoseEntry *pose, int32_t ground)
	{
		const int32_t bone = ip[0];
		const int32_t flag = ip[1];
		const int32_t layout = ip[2];
		const int16_t scale = ip[3];
		const int16_t turn = ip[4];
		const int16_t depth = ip[5];
		ip += 6;
		LayoutNode *L = (LayoutNode *)AddTaskToQueue(QLayout(), game_addr(ORIG_LayoutTask));
		if (!L) return ip;
		uint8_t *anim = entity + 0x60;
		PoseBones(anim, pose);
		GetEffectSpawnPosition(entity, bone, flag, L->pos);
		const PoseEntry &pe = pose[bone];
		L->pos[0] = (int16_t)(L->pos[0] + pe.pos[0]);
		if (ground != 0) L->pos[1] = 0;
		else L->pos[1] = (int16_t)(L->pos[1] + pe.pos[1]);
		L->pos[2] = (int16_t)(L->pos[2] + pe.pos[2]);
		L->scale = scale;
		L->turn = turn;
		L->depth = depth;
		BuildBoneMatricesFromPose(anim);
		DecodeModelPrimLayout(*(const uint32_t *)(game_addr(LAYOUTS) + layout * 8), L->layout, *(const int32_t *)(game_addr(LAYOUTS) + layout * 8 + 4));
		return ip;
	}

	static const int16_t *SpawnSprite(const int16_t *ip, uint8_t *entity, PoseEntry *pose)
	{
		const int32_t mode = *ip++;
		SpriteNode *s;
		if (mode == 0) s = (SpriteNode *)AddTaskToQueue(QSprite(), game_addr(ORIG_SpriteTask));
		else s = (SpriteNode *)entity;
		if (!s) return ip + 10;
		PoseBones(entity + 0x60, pose);
		const int32_t bone = ip[0];
		GetEffectSpawnPosition(entity, bone, ip[1], s->pos);
		const PoseEntry *pe = pose + bone;
		s->pos[0] = (int16_t)(s->pos[0] + pe->pos[0]);
		s->pos[1] = (int16_t)(s->pos[1] + pe->pos[1]);
		s->pos[2] = (int16_t)(s->pos[2] + pe->pos[2]);
		s->frame = 0;
		s->seq = (uint8_t)ip[2];
		s->size = ip[3];
		s->vel[0] = (int8_t)ip[4];
		s->vel[1] = (int8_t)ip[5];
		s->vel[2] = (int8_t)ip[6];
		s->acc[0] = (int8_t)ip[7];
		s->acc[1] = (int8_t)ip[8];
		s->pad1B = (int8_t)ip[9];
		return ip + 10;
	}

	static int32_t Spread(int32_t range, int32_t base)
	{
		if (range == 0) return base;
		return (mul32(ModLcg(), range) >> 15) + base;
	}

	static const int16_t *SpawnSprites(const int16_t *ip, uint8_t *entity, PoseEntry *pose)
	{
		const int32_t mode = ip[0];
		const int32_t bone = ip[1];
		const int32_t flag = ip[2];
		const int32_t seq = ip[3];
		const int32_t size = ip[4];
		int32_t count = ip[5];
		int32_t r[18];
		for (int k = 0; k < 18; k++) r[k] = ip[6 + k];
		ip += 24;
		PoseBones(entity + 0x60, pose);
		int16_t at[4] = {};
		GetEffectSpawnPosition(entity, bone, flag, at);
		const PoseEntry *pe = pose + bone;
		at[0] = (int16_t)(at[0] + pe->pos[0]);
		at[1] = (int16_t)(at[1] + pe->pos[1]);
		at[2] = (int16_t)(at[2] + pe->pos[2]);
		while (count-- != 0)
		{
			SpriteNode *s = (SpriteNode *)entity;
			if (mode == 0) s = (SpriteNode *)AddTaskToQueue(QSprite(), game_addr(ORIG_SpriteTask));
			if (!s) break;
			s->pos[0] = (int16_t)(Spread(r[0], r[1]) + at[0]);
			s->pos[1] = (int16_t)(Spread(r[2], r[3]) + at[1]);
			s->pos[2] = (int16_t)(Spread(r[4], r[5]) + at[2]);
			s->seq = (uint8_t)seq;
			s->frame = 0;
			s->size = (int16_t)size;
			s->vel[0] = (int8_t)Spread(r[6], r[7]);
			s->vel[1] = (int8_t)Spread(r[8], r[9]);
			s->vel[2] = (int8_t)Spread(r[10], r[11]);
			s->acc[0] = (int8_t)Spread(r[12], r[13]);
			s->acc[1] = (int8_t)Spread(r[14], r[15]);
			s->pad1B = (int8_t)Spread(r[16], r[17]);
		}
		return ip;
	}

	static uint32_t __cdecl ScriptTask(TaskNode *n)
	{
		ScriptNode *s = (ScriptNode *)n;
		const int16_t *ip = s->ip;
		PoseEntry *e = s->cur;
		PoseEntry *const pose = (PoseEntry *)(TexBase() + POSE);
		for (;;)
		{
			const int32_t op = *ip++;
			switch (op)
			{
			case 0x1000: e->pos[0] = ip[0]; e->pos[1] = ip[1]; e->pos[2] = ip[2]; ip += 3; break;
			case 0x1001:
				e->pos[0] = (int16_t)(e->pos[0] + ip[0]);
				e->pos[1] = (int16_t)(e->pos[1] + ip[1]);
				e->pos[2] = (int16_t)(e->pos[2] + ip[2]);
				ip += 3;
				break;
			case 0x1002: s->vel[0] = ip[0]; s->vel[1] = ip[1]; s->vel[2] = ip[2]; ip += 3; break;
			case 0x1003:
				s->vel[0] = (int16_t)(s->vel[0] + ip[0]);
				s->vel[1] = (int16_t)(s->vel[1] + ip[1]);
				s->vel[2] = (int16_t)(s->vel[2] + ip[2]);
				ip += 3;
				break;
			case 0x1004:
				e->pos[0] = (int16_t)(e->pos[0] + s->vel[0]);
				e->pos[1] = (int16_t)(e->pos[1] + s->vel[1]);
				e->pos[2] = (int16_t)(e->pos[2] + s->vel[2]);
				if (e->pos[1] > 0)
				{
					e->pos[1] = 0;
					s->vel[1] = (int16_t)((int16_t)(s->vel[1] >> 2) - s->vel[1]);
				}
				break;
			case 0x1010: e->rot[0] = ip[0]; e->rot[1] = ip[1]; e->rot[2] = ip[2]; ip += 3; break;
			case 0x1011:
				e->rot[0] = (int16_t)(e->rot[0] + ip[0]);
				e->rot[1] = (int16_t)(e->rot[1] + ip[1]);
				e->rot[2] = (int16_t)(e->rot[2] + ip[2]);
				ip += 3;
				break;
			case 0x1012: s->avel[0] = ip[0]; s->avel[1] = ip[1]; s->avel[2] = ip[2]; ip += 3; break;
			case 0x1013:
				s->avel[0] = (int16_t)(s->avel[0] + ip[0]);
				s->avel[1] = (int16_t)(s->avel[1] + ip[1]);
				s->avel[2] = (int16_t)(s->avel[2] + ip[2]);
				ip += 3;
				break;
			case 0x1014:
				e->rot[0] = (int16_t)(e->rot[0] + s->avel[0]);
				e->rot[1] = (int16_t)(e->rot[1] + s->avel[1]);
				e->rot[2] = (int16_t)(e->rot[2] + s->avel[2]);
				break;
			case 0x1080:
				*e = *(const PoseEntry *)(TexBase() + POSE + shl32(ip[0], 4));
				ip++;
				break;
			case 0x1081:
			{
				const uint32_t *src = (const uint32_t *)(TexBase() + POSE + shl32(ip[0], 4));
				ip++;
				((uint32_t *)e)[0] = src[0];
				((uint32_t *)e)[1] = src[1];
				break;
			}
			case 0x1082:
			{
				const uint32_t *src = (const uint32_t *)(TexBase() + POSE + 8 + shl32(ip[0], 4));
				ip++;
				((uint32_t *)e)[2] = src[0];
				((uint32_t *)e)[3] = src[1];
				break;
			}
			case 0x1090: ip = Shatter(ip, Caster(), pose); break;
			case 0x10A0: ip = SpawnLayout(ip, Caster(), pose, 0); break;
			case 0x10A1: ip = SpawnLayout(ip, Caster(), pose, 1); break;
			case 0x10A8: ip = SpawnSprite(ip, Caster(), pose); break;
			case 0x10A9: ip = SpawnSprites(ip, Caster(), pose); break;
			case 0x10F0:
			{
				int16_t *c = (int16_t *)ip;
				if (*c != 0)
				{
					*c = (int16_t)(*c - 1);
					ip++;
					ip = ip + ip[1] - 2;
				}
				else
				{
					*c = ip[1];
					ip += 3;
				}
				break;
			}
			case 0x1100: return TASK_END;
			default:
				s->ip = ip;
				return 0;
			}
		}
	}

	static void StartScript(uint32_t script)
	{
		ScriptNode *s = (ScriptNode *)AddTaskToQueue(QScript(), game_addr(ORIG_ScriptTask));
		Memset32(&s->cur, 0, 6);
		const int16_t *p = (const int16_t *)script;
		s->ip = p + 1;
		s->cur = (PoseEntry *)(TexBase() + shl32(p[0] + 0x17B, 4));
	}

	static uint32_t __cdecl MasterTask(TaskNode *n)
	{
		MasterNode *m = (MasterNode *)n;
		if (m->counter >= 0x73) return TASK_END;
		if (m->counter >= 0)
		{
			CasterDraw(Caster(), m->pose);
			*(uint8_t *)Caster() |= 4;
		}
		DrawFragments(m);
		DrawMarks(m);
		if (m->counter == 1) BdPlaySE(SOUND_Start(), 0, 0x80);
		else if (m->counter == 0x6F) ReleaseVoiceSlot(m->voice);
		if (m->counter >= 0x5A) *(int16_t *)(Caster() + 0x20) = (int16_t)(*(int16_t *)(Caster() + 0x20) - 0x80);
		MoveFragments(m);
		m->counter++;
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
		if (rn->counter == 1 && !rn->started)
		{
			rn->started = 1;
			MusicVolume(0xF0);
			InitTaskQueuePool(QMaster(), (void *)(TexBase() + POOL_Master), 0x89F4, 1);
			InitTaskQueuePool(QLayout(), (void *)(TexBase() + POOL_Layout), 0x148, 8);
			InitTaskQueuePool(QSprite(), (void *)(TexBase() + POOL_Sprite), 0x1C, 0x40);
			InitTaskQueuePool(QScript(), (void *)TexBase(), 0x24, 0x2D);
			MasterNode *m = (MasterNode *)AddTaskToQueue(QMaster(), game_addr(ORIG_MasterTask));
			Memset32(&m->counter, 0, 0x227A);
			m->voice = ClaimVoiceSlot(SOUND_Voice(), 1, 0x80);
			m->next_frag = m->frags;
			m->next_mark = m->marks;
			m->at[0] = *(const uint32_t *)(Caster() + 0x1C);
			m->at[1] = *(const uint32_t *)(Caster() + 0x20);
			for (int i = 0; i < 3; i++) StartScript(game_addr(SCRIPTS[i]));
			StreamStateInit((void *)(TexBase() + STREAM_State));
		}
		if (rn->started && rn->counter < 0x75)
		{
			ExecuteTaskQueue(QMaster());
			ExecuteTaskQueue(QScript());
			ExecuteTaskQueue(QLayout());
			ExecuteTaskQueue(QSprite());
		}
		if (rn->counter == 0x76) StreamLoad(STREAM_File, TexBase() + STREAM_Dest, 0);
		if (rn->started) StreamPump();
		if (rn->counter > 0x76) return StreamDone(0) ? TASK_END : 0;
		rn->counter++;
		return 0;
	}
}

	void register_mag298_bgh251f2_destruction()
	{
		register_port(destruct298::ORIG_RootTask, (void *)destruct298::RootTask, "298 RootTask", 298);
		register_port(destruct298::ORIG_MasterTask, (void *)destruct298::MasterTask, "298 MasterTask", 298);
		register_port(destruct298::ORIG_ScriptTask, (void *)destruct298::ScriptTask, "298 ScriptTask", 298);
		register_port(destruct298::ORIG_LayoutTask, (void *)destruct298::LayoutTask, "298 LayoutTask", 298);
		register_port(destruct298::ORIG_SpriteTask, (void *)destruct298::SpriteTask, "298 SpriteTask", 298);
	}
}

