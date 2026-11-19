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

// Effect 303: Remove mask: the mask flies off in four pieces that burst into

#include "mag_common.h"
#include <stddef.h>

namespace ff8fx
{
namespace mask303
{
	using namespace eng;
	using namespace magc;

	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::unk_25075D4)); }
	inline uint8_t *&Caster() { return var<uint8_t *>(game_addr(game::unk_25075D8)); }
	inline uint8_t *&Unmasked() { return var<uint8_t *>(game_addr(game::unk_25075DC)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::unk_2507644)); }
	inline uint32_t &Lcg() { return var<uint32_t>(game_addr(game::unk_2507648)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::unk_2507658)); }
	inline TaskQueue *QMain() { return (TaskQueue *)game_addr(game::unk_25075F0); }
	inline TaskQueue *QLayouts() { return (TaskQueue *)game_addr(game::unk_2507600); }
	inline TaskQueue *QSparks() { return (TaskQueue *)game_addr(game::unk_2507610); }
	inline TaskQueue *QSprites() { return (TaskQueue *)game_addr(game::unk_2507620); }
	inline uint32_t OTBase() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)); }
	inline uint32_t &FrameCursor() { return var<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)); }

	static constexpr game::Id ORIG_RootTask = game::MAG_303_sub_66A920;
	static constexpr game::Id ORIG_MainTask = game::MAG_303_sub_66AAF0;
	static constexpr game::Id ORIG_LayoutTask = game::MAG_303_sub_66C220;
	static constexpr game::Id ORIG_SparkTask = game::MAG_303_sub_66C100;
	static constexpr game::Id ORIG_SpriteTask = game::MAG_303_sub_66C080;
	static const uint32_t POOL_Layouts = 0x27C0, POOL_Sparks = 0x2CA0, POOL_Sprites = 0x35A0;
	static const uint32_t STREAM_STATE = 0x3640;
	static const uint32_t MORPH = 0x3688;
	static const uint32_t ARENA_A = 0x5688, ARENA_B = 0x15688;
	static constexpr game::Id MODEL_Main = game::unk_1033114;
	static constexpr game::Id MODEL_Piece = game::unk_10325F4;
	static constexpr game::Id SEQ_Spark = game::unk_1031440, SEQ_Sprite = game::unk_1031550;
	static const void *SOUND_Voice() { return (const void *)game_addr(game::unk_103411C); }
	static const void *SOUND_Start() { return (const void *)game_addr(game::unk_10340A8); }
	static const int16_t *POINT_Main() { return (const int16_t *)game_addr(game::unk_1034250); }
	static const int16_t *POINT_Sprites() { return (const int16_t *)game_addr(game::unk_1034258); }
	static const struct { game::Id vel, model; int16_t scale; } PIECES[4] = {
		{ game::unk_1034230, game::unk_1031694, 0x1388 }, { game::unk_1034238, game::unk_1031E5C, 0x1000 }, { game::unk_1034240, game::unk_103218C, 0x1000 }, { game::unk_1034248, game::unk_10324BC, 0xBB8 } };
	static const int32_t REDRAW_Clut = 0x3DD4;

	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline void RotY(int32_t angle, Mat4x3 *out) { fn<void (__cdecl *)(int32_t, Mat4x3 *)>(game_addr(game::Effect_RotationYMatrix))(angle, out); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void RotationFromAnglesB(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::MAG_063_RotationFromAnglesYXZ))(angles, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void GteMVMVA_RotV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0))(); }
	inline uint32_t RenderPrim2(void *h, uint32_t ot, int32_t shift, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::MAG_070_RenderPrimModel))(h, ot, shift, cursor); }
	inline void StreamLoad(int32_t id, uint32_t dst, int32_t kind) { fn<void (__cdecl *)(int32_t, uint32_t, int32_t)>(game_addr(game::Stream_QueueLoad))(id, dst, kind); }
	inline void StreamPump() { fn<void (__cdecl *)()>(game_addr(game::Stream_Pump))(); }
	inline int32_t StreamDone(int32_t a) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::BattleFile_PreLoad_IsLoaded))(a); }
	inline void ReleaseVoiceSlot(uint32_t slot) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::BdSound_ReleaseVoiceSlot))(slot); }
	inline void SetMonsterPresence(int32_t slot) { fn<void (__cdecl *)(int32_t)>(game_addr(game::setMonsterPresence))(slot); }
	inline uint32_t DrawShadow(uint8_t *ent, uint32_t ot, int32_t a, uint32_t cursor) { return fn<uint32_t (__cdecl *)(uint8_t *, uint32_t, int32_t, uint32_t)>(game_addr(game::BattleEntity_DrawShadow))(ent, ot, a, cursor); }
	inline void GteLoadV1(const void *v) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadV1))(v); }
	inline void GteLoadV2(const void *v) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadV2))(v); }
	inline void GteLoadV0u(const void *v) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadV0_u16))(v); }
	inline void GteRTPT() { fn<void (__cdecl *)()>(game_addr(game::GTE_RTPT))(); }
	inline void GteNCLIP() { fn<void (__cdecl *)()>(game_addr(game::GTE_NCLIP))(); }
	inline void GteReadMAC0(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadMAC0))(dst); }
	inline void GteReadSXY012Split(void *a, void *b, void *c) { fn<void (__cdecl *)(void *, void *, void *)>(game_addr(game::GTE_ReadSXY012_Split))(a, b, c); }
	inline void GteReadSXY2(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadSXY2))(dst); }
	inline void GteAVSZ3() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ3))(); }
	inline void GteAVSZ4() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ4))(); }
	inline void GteReadOTZ32(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadOTZ))(dst); }
	inline void GteSetSXY012(uint32_t a, uint32_t b, uint32_t c) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_SetSXY012))(a, b, c); }
	inline void GteSetSZ123(int32_t a, int32_t b, int32_t c) { fn<void (__cdecl *)(int32_t, int32_t, int32_t)>(game_addr(game::GTE_SetSZ123))(a, b, c); }
	inline void GteSetSZ0123(int32_t a, int32_t b, int32_t c, int32_t d) { fn<void (__cdecl *)(int32_t, int32_t, int32_t, int32_t)>(game_addr(game::GTE_SetSZ0123))(a, b, c, d); }

#pragma pack(push, 1)
	struct RootNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t pad0E;
		uint8_t started;
		uint32_t arena;
	};
	struct Shard
	{
		int16_t pos[3];
		int16_t kind;
		int16_t vel[3];
		int16_t life;
		uint32_t code;
		uint8_t spin[0x12];
		uint8_t pad26[2];
		int16_t corner[4][3];
		uint32_t uv0;
		uint32_t uv1;
		uint16_t uv2, uv3;
	};
	struct MainNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		uint32_t voice;
		int16_t pt[4];
		Shard *cursor;
		Shard shards[0x80];
		uint8_t layout[0x1A0];
	};
	struct LayoutNode
	{
		TaskNode hdr;
		uint32_t model;
		int16_t pos[3];
		int16_t scale;
		int16_t vel[3];
		int16_t life;
		uint8_t layout[0x118];
	};
	struct SparkNode
	{
		TaskNode hdr;
		int16_t pos[3];
		int16_t frame;
		int16_t vel[3];
		int16_t pad1A;
		int16_t acc[3];
		int16_t pad22;
	};
	struct SpriteNode
	{
		TaskNode hdr;
		int16_t pos[3];
		int16_t frame;
	};
	struct PrimArg
	{
		int16_t pos[4];
		int32_t scale[3];
		int32_t scaled;
		int32_t depth;
		uint32_t morph;
	};
	struct ShatterHdr
	{
		uint32_t model;
		MainNode *main;
		int32_t shift;
		uint8_t *verts;
		uint8_t *list;
		int16_t v[4][4];
		int16_t centre[4];
		uint8_t *spins;
		int16_t angles[4];
		int32_t scale[3];
		uint8_t pad54[4];
	};
	struct RedrawHdr
	{
		uint8_t *faces;
		uint8_t *vb;
		uint16_t ntri, nquad;
		int16_t clip[4];
		uint32_t colour;
		uint32_t shown;
		uint8_t e[4][0xC];
		int32_t mac0;
		uint8_t pad50[4];
		int32_t otz;
		uint8_t pad58[4];
		uint32_t rgb;
		uint16_t clut, page;
		int32_t voff, uoff;
		int32_t uwrap, vwrap;
		uint32_t win_a[3];
		uint32_t win_b[3];
		uint8_t *list;
		uint8_t *vcur;
		uint32_t frame_cursor;
		uint32_t cursor;
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x14 && sizeof(Shard) == 0x4C && sizeof(MainNode) == 0x27C0, "Remove mask nodes");
	static_assert(sizeof(LayoutNode) == 0x138 && sizeof(SparkNode) == 0x24 && sizeof(SpriteNode) == 0x14 && sizeof(PrimArg) == 0x20, "Remove mask nodes");
	static_assert(sizeof(ShatterHdr) == 0x58 && sizeof(RedrawHdr) == 0x9C && offsetof(MainNode, layout) == 0x2620, "Remove mask headers");
	static_assert(offsetof(RedrawHdr, mac0) == 0x4C && offsetof(RedrawHdr, rgb) == 0x5C && offsetof(RedrawHdr, win_a) == 0x74 && offsetof(RedrawHdr, cursor) == 0x98, "Remove mask redraw header");

	static uint32_t Rand15()
	{
		Lcg() = (Lcg() * 125 + 0xE) & 0x7FFF;
		return Lcg();
	}

	static int16_t AccelOf(int16_t v)
	{
		int32_t q = (int32_t)(((int64_t)v * (int32_t)0xD5555555) >> 32);
		q >>= 1;
		q += (int32_t)((uint32_t)q >> 31);
		return (int16_t)q;
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
		if (r->flags & 0x40000) RotationFromAnglesB(r->rot, &m);
		else RotationFromAngles(r->rot, &m);
		int16_t off[4] = { r->pos[0], r->pos[1], r->pos[2], 0 };
		if (arg->scaled)
		{
			off[0] = (int16_t)(mul32(r->pos[0], arg->scale[0]) >> 12);
			off[1] = (int16_t)(mul32(r->pos[1], arg->scale[1]) >> 12);
			off[2] = (int16_t)(mul32(r->pos[2], arg->scale[2]) >> 12);
			Scale3DMatrix(&m, arg->scale);
		}
		if (r->flags & 0x200)
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
			const int32_t s[3] = { r->scale[0], r->scale[1], r->scale[2] };
			Scale3DMatrix(&m, s);
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
		PacketCursor() = RenderPrim2(h, OTBase() + 0x44, 2, PacketCursor());
		FieldFree(0x58);
	}

	static void BonePoint(const int16_t *v, int16_t *out)
	{
		uint8_t *c = Caster();
		const uint32_t bones = **(const uint32_t *const *)(c + 0x64) + 0x10;
		BuildBoneMatricesFromPose(c + 0x60);
		Mat4x3 m = {};
		ComposeAffineTransform((const Mat4x3 *)(Caster() + 0x40), (const Mat4x3 *)(bones + (uint32_t)(int32_t)v[3] * 0x30 + 0x10), &m);
		GteSetRotMatrixCtrl(&m);
		GteSetTransVectorCtrl(&m);
		GteLoadV0(v);
		GteMVMVA_RotV0Tr();
		GteStoreIR123(out);
	}

	static inline bool Off16(int16_t v, uint16_t lim) { return (uint16_t)v > lim; }

	static void ShardsDraw(Shard *shards)
	{
		GteSetRotMatrix(&Camera());
		GteSetTransVector(&Camera());
		int16_t *v = (int16_t *)FieldAlloc(8);
		for (int k = 0; k < 0x80; k++)
		{
			const Shard *s = &shards[k];
			if (s->kind == 0) continue;
			uint8_t *P = (uint8_t *)PacketCursor();
			for (int j = 0; j < 3; j++) v[j] = (int16_t)(s->corner[0][j] + s->pos[j]);
			GteLoadV0(v);
			for (int j = 0; j < 3; j++) v[j] = (int16_t)(s->corner[1][j] + s->pos[j]);
			GteLoadV1(v);
			for (int j = 0; j < 3; j++) v[j] = (int16_t)(s->corner[2][j] + s->pos[j]);
			GteLoadV2(v);
			GteRTPT();
			*(uint32_t *)(P + 0xC) = s->uv0;
			*(uint32_t *)(P + 0x14) = s->uv1;
			*(uint16_t *)(P + 0x1C) = s->uv2;
			GteReadSXY012Split(P + 8, P + 0x10, P + 0x18);
			uint32_t clip = 0;
			if (Off16(*(int16_t *)(P + 8), 0xA00)) clip = 1;
			if (Off16(*(int16_t *)(P + 0x10), 0xA00)) clip |= 2;
			if (Off16(*(int16_t *)(P + 0x18), 0xA00)) clip |= 4;
			if (Off16(*(int16_t *)(P + 0xA), 0x6C0)) clip |= 0x10;
			if (Off16(*(int16_t *)(P + 0x12), 0x6C0)) clip |= 0x20;
			if (Off16(*(int16_t *)(P + 0x1A), 0x6C0)) clip |= 0x40;
			uint32_t next;
			if (s->kind == 2)
			{
				for (int j = 0; j < 3; j++) v[j] = (int16_t)(s->corner[3][j] + s->pos[j]);
				GteLoadV0(v);
				GteRTPS();
				*(uint16_t *)(P + 0x24) = s->uv3;
				GteReadSXY2(P + 0x20);
				GteAVSZ4();
				if (Off16(*(int16_t *)(P + 0x20), 0xA00)) clip |= 8;
				if (Off16(*(int16_t *)(P + 0x22), 0x6C0)) clip |= 0x80;
				if ((clip & 0xF) == 0xF || (clip & 0xF0) == 0xF0) continue;
				GteReadOTZ32(v);
				if (v[0] <= 0) continue;
				*(uint32_t *)P = 0x9000000;
				*(uint32_t *)(P + 4) = s->code;
				next = PacketCursor() + 0x28;
			}
			else
			{
				GteAVSZ3();
				if ((clip & 7) == 7 || (clip & 0x70) == 0x70) continue;
				*(uint32_t *)P = 0x7000000;
				*(uint32_t *)(P + 4) = s->code;
				GteReadOTZ32(v);
				if (v[0] <= 0) continue;
				next = PacketCursor() + 0x20;
			}
			PacketCursor() = next;
			InsertPrimAutoDepth(OTBase() + (uint32_t)((int32_t)v[0] >> 2) * 4, P);
		}
		FieldFree(8);
	}

	static bool ShardMove(Shard *s, Shard **cursor)
	{
		s->life--;
		if (s->life != 0)
		{
			GteSetRotMatrixCtrl((const Mat4x3 *)s->spin);
			GteLoadV0(s->corner[0]);
			GteMVMVA_RotV0();
			GteStoreIR123(s->corner[0]);
			GteLoadV0u(s->corner[1]);
			GteMVMVA_RotV0();
			if (!(*(const uint32_t *)s->corner[0] == 0 && s->corner[0][2] == 0))
			{
				GteStoreIR123(s->corner[1]);
				GteLoadV0(s->corner[2]);
				GteMVMVA_RotV0();
				GteStoreIR123(s->corner[2]);
				GteLoadV0u(s->corner[3]);
				GteMVMVA_RotV0();
				GteStoreIR123(s->corner[3]);
				s->pos[0] = (int16_t)(s->pos[0] + s->vel[0]);
				s->pos[1] = (int16_t)(s->pos[1] + s->vel[1]);
				s->pos[2] = (int16_t)(s->pos[2] + s->vel[2]);
				s->vel[1] = (int16_t)(s->vel[1] + 0x1E);
				if (s->pos[1] >= 0) s->vel[1] = (int16_t)(-(int32_t)s->vel[1] >> 1);
				return true;
			}
		}
		s->kind = 0;
		*cursor = s;
		return false;
	}

	static void ShardsMove(MainNode *m)
	{
		for (int k = 0; k < 0x80; k++)
			if (m->shards[k].kind != 0) ShardMove(&m->shards[k], &m->cursor);
	}

	static Shard *AllocShard(MainNode *m)
	{
		Shard *s = m->cursor;
		if (s->kind != 0)
		{
			s = &m->shards[0];
			if (s->kind != 0)
			{
				int k = 1;
				for (; k < 0x80; k++)
					if (m->shards[k].kind == 0) break;
				if (k == 0x80) return nullptr;
				s = &m->shards[k];
			}
		}
		m->cursor = (uint8_t *)s >= (uint8_t *)&m->shards[0x7F] ? &m->shards[0] : s + 1;
		s->life = (int16_t)((Rand15() & 7) + 0xC);
		return s;
	}

	static void MakeShard(ShatterHdr *h, const uint8_t *f, int n)
	{
		Shard *s = AllocShard(h->main);
		if (!s) return;
		s->code = *(const uint32_t *)f;
		for (int i = 0; i < n; i++) ff8fx::copy<(8)>(h->v[i], h->verts + (uint32_t)*(const uint16_t *)(f + 4 + 2 * i) * 4);
		int16_t c[3];
		for (int j = 0; j < 3; j++)
		{
			int32_t sum = 0;
			for (int i = 0; i < n; i++) sum += h->v[i][j];
			c[j] = (int16_t)(n == 3 ? sum / 3 : sum / 4);
		}
		s->pos[0] = c[0];
		s->pos[1] = c[1];
		s->pos[2] = c[2];
		s->kind = (int16_t)(n == 3 ? 1 : 2);
		for (int i = 0; i < n; i++)
			for (int j = 0; j < 3; j++) s->corner[i][j] = (int16_t)(h->v[i][j] - c[j]);
		for (int j = 0; j < 3; j++) s->vel[j] = (int16_t)(((int32_t)c[j] - h->centre[j]) >> (h->shift & 0x1F));
		s->uv0 = *(const uint32_t *)(f + 0xC);
		s->uv1 = *(const uint32_t *)(f + 0x10);
		if (n == 3) s->uv2 = *(const uint16_t *)(f + 0xA);
		else *(uint32_t *)&s->uv2 = *(const uint32_t *)(f + 0x14);
		ff8fx::copy<(0x12)>(s->spin, h->spins + (Rand15() & 0x3F) * 0x20);
	}

	static void ShardList(ShatterHdr *h, int n, uint32_t stride)
	{
		int32_t count = *(const int32_t *)h->list;
		uint8_t *f = h->list + 4;
		for (; count > 0; count--, f += stride) MakeShard(h, f, n);
		h->list = f;
	}

	static void Shatter(ShatterHdr *h)
	{
		uint8_t *out = (uint8_t *)PacketCursor();
		int16_t mx = -0x7FFF, my = -0x7FFF, mz = -0x7FFF, nx = 0x7FFF, ny = 0x7FFF, nz = 0x7FFF;
		h->verts = out;
		const uint8_t *v = (const uint8_t *)h->model + 8;
		int32_t n = *(const int32_t *)(h->model + 4);
		do
		{
			GteLoadV0(v);
			GteMVMVA_RotV0Tr();
			v += 8;
			GteStoreIR123(h->v[0]);
			const int16_t x = h->v[0][0], y = h->v[0][1], z = h->v[0][2];
			if (x < nx) nx = x;
			if (x > mx) mx = x;
			if (y < ny) ny = y;
			if (y > my) my = y;
			if (z < nz) nz = z;
			if (z > mz) mz = z;
			ff8fx::copy<(8)>(out, h->v[0]);
			out += 8;
		} while (--n);
		h->spins = out;
		h->centre[0] = (int16_t)(((int32_t)mx + nx) / 2);
		h->centre[1] = (int16_t)(((int32_t)my + ny) / 2);
		h->centre[2] = (int16_t)(((int32_t)mz + nz) / 2);
		for (int k = 0x40; k != 0; k--)
		{
			h->angles[0] = (int16_t)((CrtRand() & 0x1FF) - 0x100);
			h->angles[1] = (int16_t)((CrtRand() & 0x1FF) - 0x100);
			h->angles[2] = (int16_t)((CrtRand() & 0x1FF) - 0x100);
			BuildRotationMatrixFromAngles(h->angles, (Mat4x3 *)out);
			const int32_t s = 0xF81 - (CrtRand() & 0xFF);
			h->scale[2] = s;
			h->scale[1] = s;
			h->scale[0] = s;
			Scale3DMatrix((Mat4x3 *)out, h->scale);
			out += 0x20;
		}
		h->list = (uint8_t *)h->model + *(const uint32_t *)h->model + 8;
		if (*(const uint32_t *)h->list != 0) ShardList(h, 3, 0x14);
		else h->list += 4;
		if (*(const uint32_t *)h->list != 0) ShardList(h, 4, 0x18);
	}

	static void ProjectGroup(RedrawHdr *hh)
	{
		uint8_t *p = hh->list;
		uint8_t *vb = hh->vcur;
		int32_t n = *(const int16_t *)p;
		p += 2;
		for (; n != 0; n--)
		{
			GteLoadV0u(p);
			GteRTPS();
			p += 6;
			uint8_t *e = hh->e[0];
			GteReadSXY2(e);
			GteReadOTZ(e + 4);
			if (*(const int16_t *)(e + 4) <= 0)
			{
				vb[6] = 0x10;
			}
			else
			{
				const int32_t sx = *(const int16_t *)e, sy = *(const int16_t *)(e + 2);
				if (sx < shl32(hh->clip[0], 3)) e[6] |= 1;
				else if (sx >= shl32(hh->clip[2], 3)) e[6] |= 2;
				if (sy < shl32(hh->clip[1], 3)) e[6] |= 4;
				else if (sy >= shl32(hh->clip[3], 3)) e[6] |= 8;
				int32_t ir1, ir2;
				GteReadDataRegTo(&ir1, 9);
				GteReadDataRegTo(&ir2, 10);
				e[8] = (uint8_t)(ir2 >> 3);
				e[7] = (uint8_t)(ir1 >> 3);
				ff8fx::copy<(0xC)>(vb, e);
			}
			vb += 0xC;
		}
		hh->list = p;
		hh->vcur = vb;
	}

	static void EnvUV(const RedrawHdr *hh, int n, int o, int32_t off, uint8_t wrap, uint8_t *q)
	{
		int32_t c[4];
		for (int i = 0; i < n; i++) c[i] = hh->e[i][o];
		int32_t mn = c[0];
		for (int i = 1; i < n; i++)
			if (mn > c[i]) mn = c[i];
		int32_t span = 0;
		for (int i = 0; i < n; i++) span |= c[i] - mn;
		if (span > 0x80)
			for (int i = 0; i < n; i++) c[i] = (c[i] - 0x80) & 0xFF;
		int32_t any = 0;
		for (int i = 0; i < n; i++) any |= (c[i] = (int32_t)((uint32_t)c[i] + (uint32_t)off));
		for (int i = 0; i < n; i++) q[8 * i] = any > 0xFF ? (uint8_t)((uint8_t)c[i] - wrap) : (uint8_t)c[i];
	}

	static void EnvPolys(RedrawHdr *hh, uint32_t ot, int32_t shift)
	{
		uint8_t *Q = (uint8_t *)hh->frame_cursor;
		const uint8_t *vb = hh->vb;
		uint8_t *f = hh->faces;
		for (uint32_t i = 0; i < hh->ntri; i++, f += 0x10)
		{
			for (int j = 0; j < 3; j++) ff8fx::copy<(0xC)>(hh->e[j], vb + (uint32_t)(*(const uint16_t *)(f + 2 * j) & 0xFFF) * 0xC);
			if ((uint8_t)(hh->e[0][6] | hh->e[1][6] | hh->e[2][6]) >= 0x10) continue;
			if (hh->e[2][6] & (uint8_t)(hh->e[0][6] & hh->e[1][6])) continue;
			GteSetSXY012(*(const uint32_t *)hh->e[0], *(const uint32_t *)hh->e[1], *(const uint32_t *)hh->e[2]);
			GteNCLIP();
			GteReadMAC0(&hh->mac0);
			if (hh->mac0 < 0) continue;
			GteSetSZ123(*(const int16_t *)(hh->e[0] + 4), *(const int16_t *)(hh->e[1] + 4), *(const int16_t *)(hh->e[2] + 4));
			GteAVSZ3();
			uint8_t *P1 = (uint8_t *)hh->cursor;
			hh->cursor += 0xC;
			ff8fx::copy<(0xC)>(P1, hh->win_a);
			GteReadOTZ32(&hh->otz);
			const uint32_t bucket = ot + (uint32_t)(hh->otz >> (shift & 0x1F)) * 4;
			InsertPrimAutoDepth(bucket, P1);
			*(uint32_t *)Q = 0x7000000;
			*(uint32_t *)(Q + 4) = hh->rgb | 0x26000000;
			*(uint32_t *)(Q + 8) = *(const uint32_t *)hh->e[0];
			*(uint32_t *)(Q + 0x10) = *(const uint32_t *)hh->e[1];
			*(uint32_t *)(Q + 0x18) = *(const uint32_t *)hh->e[2];
			EnvUV(hh, 3, 7, hh->uoff, (uint8_t)hh->uwrap, Q + 0xC);
			EnvUV(hh, 3, 8, hh->voff, (uint8_t)hh->vwrap, Q + 0xD);
			*(uint16_t *)(Q + 0xE) = hh->clut;
			*(uint16_t *)(Q + 0x16) = hh->page;
			InsertPrimAutoDepth(bucket, Q);
			uint8_t *P2 = (uint8_t *)hh->cursor;
			hh->cursor += 0xC;
			ff8fx::copy<(0xC)>(P2, hh->win_b);
			Q += 0x20;
			InsertPrimAutoDepth(bucket, P2);
			uint8_t *P3 = (uint8_t *)hh->cursor;
			hh->cursor += 0x20;
			*(uint32_t *)P3 = 0x7000000;
			*(uint32_t *)(P3 + 4) = hh->colour | 0x24000000;
			*(uint32_t *)(P3 + 8) = *(const uint32_t *)hh->e[0];
			*(uint32_t *)(P3 + 0x10) = *(const uint32_t *)hh->e[1];
			*(uint32_t *)(P3 + 0x18) = *(const uint32_t *)hh->e[2];
			*(uint32_t *)(P3 + 0xC) = *(const uint32_t *)(f + 8);
			*(uint32_t *)(P3 + 0x14) = *(const uint32_t *)(f + 0xC);
			*(uint16_t *)(P3 + 0x1C) = *(const uint16_t *)(f + 6);
			InsertPrimAutoDepth(bucket, P3);
		}
		for (uint32_t i = 0; i < hh->nquad; i++, f += 0x14)
		{
			for (int j = 0; j < 4; j++) ff8fx::copy<(0xC)>(hh->e[j], vb + (uint32_t)(*(const uint16_t *)(f + 2 * j) & 0xFFF) * 0xC);
			if ((uint8_t)(hh->e[0][6] | hh->e[1][6] | hh->e[2][6] | hh->e[3][6]) >= 0x10) continue;
			if (hh->e[3][6] & (uint8_t)(hh->e[0][6] & hh->e[1][6] & hh->e[2][6])) continue;
			GteSetSXY012(*(const uint32_t *)hh->e[0], *(const uint32_t *)hh->e[1], *(const uint32_t *)hh->e[2]);
			GteNCLIP();
			GteReadMAC0(&hh->mac0);
			if (hh->mac0 < 0) continue;
			GteSetSZ0123(*(const int16_t *)(hh->e[0] + 4), *(const int16_t *)(hh->e[1] + 4), *(const int16_t *)(hh->e[2] + 4), *(const int16_t *)(hh->e[3] + 4));
			GteAVSZ4();
			uint8_t *P1 = (uint8_t *)hh->cursor;
			ff8fx::copy<(0xC)>(P1, hh->win_a);
			GteReadOTZ32(&hh->otz);
			const uint32_t bucket = ot + (uint32_t)(hh->otz >> (shift & 0x1F)) * 4;
			InsertPrimAutoDepth(bucket, P1);
			hh->cursor += 0xC;
			*(uint32_t *)Q = 0x9000000;
			*(uint32_t *)(Q + 4) = hh->rgb | 0x2E000000;
			*(uint32_t *)(Q + 8) = *(const uint32_t *)hh->e[0];
			*(uint32_t *)(Q + 0x10) = *(const uint32_t *)hh->e[1];
			*(uint32_t *)(Q + 0x18) = *(const uint32_t *)hh->e[2];
			*(uint32_t *)(Q + 0x20) = *(const uint32_t *)hh->e[3];
			EnvUV(hh, 4, 7, hh->uoff, (uint8_t)hh->uwrap, Q + 0xC);
			EnvUV(hh, 4, 8, hh->voff, (uint8_t)hh->vwrap, Q + 0xD);
			*(uint16_t *)(Q + 0xE) = hh->clut;
			*(uint16_t *)(Q + 0x16) = hh->page;
			InsertPrimAutoDepth(bucket, Q);
			uint8_t *P2 = (uint8_t *)hh->cursor;
			ff8fx::copy<(0xC)>(P2, hh->win_b);
			Q += 0x28;
			InsertPrimAutoDepth(bucket, P2);
			uint8_t *P3 = (uint8_t *)hh->cursor + 0xC;
			hh->cursor = (uint32_t)P3 + 0x28;
			*(uint32_t *)P3 = 0x9000000;
			*(uint32_t *)(P3 + 4) = hh->colour | 0x2C000000;
			*(uint32_t *)(P3 + 8) = *(const uint32_t *)hh->e[0];
			*(uint32_t *)(P3 + 0x10) = *(const uint32_t *)hh->e[1];
			*(uint32_t *)(P3 + 0x18) = *(const uint32_t *)hh->e[2];
			*(uint32_t *)(P3 + 0x20) = *(const uint32_t *)hh->e[3];
			*(uint32_t *)(P3 + 0xC) = *(const uint32_t *)(f + 8);
			*(uint32_t *)(P3 + 0x14) = *(const uint32_t *)(f + 0xC);
			*(uint16_t *)(P3 + 0x1C) = *(const uint16_t *)(f + 0x10);
			*(uint16_t *)(P3 + 0x24) = *(const uint16_t *)(f + 0x12);
			InsertPrimAutoDepth(bucket, P3);
		}
		hh->frame_cursor = (uint32_t)Q;
	}

	static void EnvModel(const uint32_t *skel, RedrawHdr *hh, uint32_t ot, int32_t shift)
	{
		const uint8_t *bones = (const uint8_t *)skel[0] + 0x10;
		const uint32_t *objs = (const uint32_t *)skel[1];
		const int32_t n = (int32_t)*objs++;
		for (int32_t i = 0; i < n; i++)
		{
			uint8_t *o = (uint8_t *)(*objs++ + skel[1]);
			hh->list = o;
			if (!(hh->shown & shl32(1, i & 0x1F))) continue;
			hh->vcur = hh->vb;
			int32_t groups = *(const int16_t *)hh->list;
			hh->list += 2;
			for (; groups > 0; groups--)
			{
				const int32_t bone = *(const int16_t *)hh->list;
				hh->list += 2;
				const Mat4x3 *m = (const Mat4x3 *)(bones + (uint32_t)bone * 0x30 + 0x10);
				GteSetRotMatrixCtrl(m);
				GteSetTransVectorCtrl(m);
				ProjectGroup(hh);
			}
			uint8_t *p = (uint8_t *)(((uint32_t)hh->list + 3) & ~3u);
			hh->list = p;
			hh->ntri = *(const uint16_t *)p;
			p += 2;
			hh->list = p;
			hh->nquad = *(const uint16_t *)p;
			p += 0xA;
			hh->list = p;
			hh->faces = p;
			EnvPolys(hh, ot, shift);
		}
	}

	static void Redraw(uint8_t *ent, const int16_t *rect, int32_t clut, int32_t uoff, int32_t voff, const uint8_t *rgb, uint32_t vb)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0xBC);
		FrameCursor() = DrawShadow(ent, OTBase() + 0x4040, 0x10, FrameCursor());
		ComposeAffineTransform(&Camera(), (const Mat4x3 *)(ent + 0x40), (Mat4x3 *)h);
		ComputeBonesWorldMatrices(ent + 0x60, h);
		RedrawHdr *hh = (RedrawHdr *)(h + 0x20);
		hh->vb = (uint8_t *)vb;
		hh->clip[0] = 0;
		hh->clip[1] = 0;
		hh->clip[2] = 0x140;
		hh->clip[3] = 0xD8;
		hh->colour = *(const uint32_t *)(ent + 0x28);
		hh->shown = *(const uint32_t *)(ent + 0x7C);
		hh->win_a[0] = 0x2000000;
		hh->win_a[1] = 0xE2000000;
		hh->win_b[0] = 0x2000000;
		hh->win_b[1] = 0xE2000210;
		hh->uoff = uoff & 0x7F;
		hh->voff = voff & 0x7F;
		hh->win_a[2] = 0;
		hh->win_b[2] = 0;
		hh->vwrap = 0x80;
		hh->uwrap = 0x80;
		const int32_t y = rect[1];
		uint16_t x = (uint16_t)((rect[0] & 0x3C0) | 0x2800);
		hh->page = (uint16_t)((uint16_t)((int16_t)x >> 6) | ((y >> 4) & 0x10) | ((y & 0x200) << 2));
		hh->clut = (uint16_t)clut;
		hh->rgb = *(const uint32_t *)rgb;
		hh->frame_cursor = FrameCursor();
		hh->cursor = PacketCursor();
		EnvModel(*(const uint32_t *const *)(ent + 0x64), hh, OTBase() + 0x44, 0);
		FrameCursor() = hh->frame_cursor;
		PacketCursor() = hh->cursor;
		BuildBoneMatricesFromPose(ent + 0x60);
		FieldFree(0xBC);
	}

	static void MainRedraw(int32_t c, uint32_t vb)
	{
		const int16_t rect[4] = { 0x2C0, 0x100, 0x80, 0x80 };
		const int32_t g = ComputeSin(shl32((c - 0x18) & 0x1FFFFF, 7)) >> 6;
		const uint8_t rgb[4] = { (uint8_t)g, (uint8_t)g, (uint8_t)g, 0 };
		Redraw(Unmasked(), rect, REDRAW_Clut, c * 2, c * 4, rgb, vb);
	}

	static void SpriteDraw(const int16_t *pos, int16_t frame, uint32_t seq)
	{
		uint8_t *h = AllocHeader(0xB4);
		TransformCameraByShadowRotation(pos, 0xC00, -0x300);
		*(uint32_t *)h = seq;
		*(int16_t *)(h + 4) = frame;
		*(uint16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, OTBase() + 0x44, 2, PacketCursor());
		FieldFree(0xB4);
	}

	static void PieceFrame(const int16_t *pos, Mat4x3 *m)
	{
		RotY(-*(const int16_t *)(Caster() + 0xE), m);
		m->t[0] = pos[0];
		m->t[1] = pos[1];
		m->t[2] = pos[2];
	}

	static void PieceDraw(uint32_t model, const int16_t *pos)
	{
		uint8_t *h = AllocHeader(0x58);
		Mat4x3 m = {};
		PieceFrame(pos, &m);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		*(uint32_t *)(h + 0x1C) = 0;
		*(uint32_t *)h = model;
		PacketCursor() = RenderPrimModel(h, OTBase() + 0x44, 2, PacketCursor());
		FieldFree(0x58);
	}

	static void PieceArg(const LayoutNode *L, PrimArg *a)
	{
		ff8fx::copy<(8)>(a->pos, L->pos);
		a->scale[0] = a->scale[1] = a->scale[2] = L->scale;
		a->scaled = 1;
		a->depth = 0;
		a->morph = TexBase() + MORPH;
	}
}
}

namespace ff8fx
{
namespace mask303
{
	static uint32_t __cdecl SpriteTask(TaskNode *n)
	{
		SpriteNode *s = (SpriteNode *)n;
		SpriteDraw(s->pos, s->frame, game_addr(SEQ_Sprite));
		s->frame++;
		return s->frame < 0xC ? 0 : TASK_END;
	}

	static uint32_t __cdecl SparkTask(TaskNode *n)
	{
		SparkNode *s = (SparkNode *)n;
		SpriteDraw(s->pos, s->frame, game_addr(SEQ_Spark));
		s->frame++;
		if (s->frame >= 0xA) return TASK_END;
		s->pos[0] = (int16_t)(s->pos[0] + s->vel[0]);
		s->pos[1] = (int16_t)(s->pos[1] + s->vel[1]);
		s->pos[2] = (int16_t)(s->pos[2] + s->vel[2]);
		s->vel[0] = (int16_t)(s->acc[0] + s->vel[0]);
		s->vel[1] = (int16_t)(s->acc[1] + s->vel[1]);
		s->vel[2] = (int16_t)(s->acc[2] + s->vel[2]);
		return 0;
	}

	static uint32_t __cdecl LayoutTask(TaskNode *n)
	{
		LayoutNode *L = (LayoutNode *)n;
		if (L->life != 0)
		{
			PieceDraw(L->model, L->pos);
			L->pos[1] = (int16_t)(L->pos[1] + L->vel[1]);
			L->pos[0] = (int16_t)(L->pos[0] + L->vel[0]);
			L->pos[2] = (int16_t)(L->pos[2] + L->vel[2]);
			L->vel[1] = (int16_t)(L->vel[1] + 0xA);
			L->life--;
			if (L->life > 0) return 0;
			Mat4x3 m = {};
			PieceFrame(L->pos, &m);
			GteSetRotMatrix(&m);
			GteSetTransVector(&m);
			ShatterHdr *h = (ShatterHdr *)FieldAlloc(0x58);
			h->model = L->model;
			h->main = (MainNode *)TexBase();
			h->shift = 2;
			Shatter(h);
			L->pos[0] = h->centre[0];
			L->pos[1] = h->centre[1];
			L->pos[2] = h->centre[2];
			FieldFree(0x58);
		}
		PrimArg a;
		PieceArg(L, &a);
		return prim::play((prim::Layout *)L->layout, PrimCallback, (int)&a, 0) != 0 ? 0 : TASK_END;
	}

	static void SpawnPiece(const int16_t *vel, uint32_t model, int16_t scale)
	{
		LayoutNode *L = (LayoutNode *)AddTaskToQueue(QLayouts(), game_addr(ORIG_LayoutTask));
		DecodeModelPrimLayout(game_addr(MODEL_Piece), L->layout, 0x118);
		L->model = model;
		ff8fx::copy<(8)>(L->pos, Caster() + 0x1C);
		L->scale = scale;
		ff8fx::copy<(8)>(L->vel, vel);
	}

	static uint32_t __cdecl MainTask(TaskNode *n)
	{
		MainNode *m = (MainNode *)n;
		if (m->counter >= 0x29)
		{
			StreamLoad(0x168, PacketCursor(), 1);
			return TASK_END;
		}
		if (m->counter == 0)
		{
			*(uint16_t *)Unmasked() &= 0xFFFB;
			*Caster() |= 4;
		}
		ShardsDraw(m->shards);
		const int32_t c = m->counter;
		if ((uint32_t)c < 0x16)
		{
			if (c == 0)
			{
				BonePoint(POINT_Main(), m->pt);
				m->pt[3] = 0;
			}
			PrimArg a;
			ff8fx::copy<(8)>(a.pos, m->pt);
			a.scaled = 0;
			a.depth = -0x80;
			a.morph = TexBase() + MORPH;
			prim::play((prim::Layout *)m->layout, PrimCallback, (int)&a, 0);
			if ((uint32_t)c < 2)
			{
				for (int k = 0x20; k != 0; k--)
				{
					SparkNode *s = (SparkNode *)AddTaskToQueue(QSparks(), game_addr(ORIG_SparkTask));
					if (!s) continue;
					ff8fx::copy<(8)>(s->pos, m->pt);
					s->vel[0] = (int16_t)((CrtRand() & 0x1FF) - 0x100);
					s->vel[1] = (int16_t)((CrtRand() & 0x1FF) - 0x100);
					s->vel[2] = (int16_t)((CrtRand() & 0x1FF) - 0x100);
					s->acc[0] = AccelOf(s->vel[0]);
					s->acc[1] = AccelOf(s->vel[1]);
					s->acc[2] = AccelOf(s->vel[2]);
				}
			}
		}
		const uint32_t e = (uint32_t)(m->counter - 0x18);
		if (e < 0x10)
		{
			MainRedraw(m->counter, TexBase() + MORPH);
			*Unmasked() |= 4;
		}
		else if (e < 0x29) *(uint16_t *)Unmasked() &= 0xFFFB;
		ShardsMove(m);
		if (m->counter == 0) BdPlaySE(SOUND_Start(), 0, 0x80);
		else if (m->counter == 0x25) ReleaseVoiceSlot(m->voice);
		if (m->counter == 6)
		{
			int16_t p[4];
			BonePoint(POINT_Sprites(), p);
			for (int k = 8; k != 0; k--)
			{
				SpriteNode *s = (SpriteNode *)AddTaskToQueue(QSprites(), game_addr(ORIG_SpriteTask));
				if (!s) continue;
				s->pos[0] = (int16_t)((CrtRand() & 0x1FF) + p[0] - 0x100);
				s->pos[1] = (int16_t)(0 - (CrtRand() & 0x1FF));
				const int32_t rz = CrtRand() & 0x1FF;
				s->frame = 0;
				s->pos[2] = (int16_t)(rz + p[2] - 0x100);
			}
		}
		if (m->counter == 0)
			for (const auto &pc : PIECES) SpawnPiece((const int16_t *)game_addr(pc.vel), game_addr(pc.model), pc.scale);
		m->counter++;
		return 0;
	}

	static uint32_t __cdecl RootTask(TaskNode *n)
	{
		RootNode *r = (RootNode *)n;
		if (r->arena)
		{
			PacketCursor() = TexBase() + ARENA_A;
			r->arena = 0;
		}
		else
		{
			PacketCursor() = TexBase() + ARENA_B;
			r->arena = 1;
		}
		if (r->counter == 1 && !r->started)
		{
			r->started = 1;
			InitTaskQueuePool(QMain(), (void *)TexBase(), 0x27C0, 1);
			InitTaskQueuePool(QLayouts(), (void *)(TexBase() + POOL_Layouts), 0x138, 4);
			InitTaskQueuePool(QSparks(), (void *)(TexBase() + POOL_Sparks), 0x24, 0x40);
			InitTaskQueuePool(QSprites(), (void *)(TexBase() + POOL_Sprites), 0x14, 8);
			MainNode *m = (MainNode *)AddTaskToQueue(QMain(), game_addr(ORIG_MainTask));
			Memset32(&m->counter, 0, 0x9ED);
			m->voice = ClaimVoiceSlot(SOUND_Voice(), 1, 0x80);
			m->cursor = m->shards;
			DecodeModelPrimLayout(game_addr(MODEL_Main), m->layout, 0x1A0);
			StreamStateInit((void *)(TexBase() + STREAM_STATE));
		}
		if (r->started)
		{
			const int a0 = ExecuteTaskQueue(QMain());
			ExecuteTaskQueue(QLayouts());
			ExecuteTaskQueue(QSparks());
			ExecuteTaskQueue(QSprites());
			if (a0 == 0)
			{
				StreamPump();
				if (StreamDone(a0) == 0) return 0;
				SetMonsterPresence((int32_t)(Unmasked() - Entity(0)) / 0x9C);
				*(uint16_t *)Unmasked() &= 0xFFFB;
				return TASK_END;
			}
		}
		r->counter++;
		return 0;
	}
}

	void register_mag303_remove_mask()
	{
		register_port(mask303::ORIG_RootTask, (void *)mask303::RootTask, "303 RootTask", 303);
		register_port(mask303::ORIG_MainTask, (void *)mask303::MainTask, "303 MainTask", 303);
		register_port(mask303::ORIG_LayoutTask, (void *)mask303::LayoutTask, "303 LayoutTask", 303);
		register_port(mask303::ORIG_SparkTask, (void *)mask303::SparkTask, "303 SparkTask", 303);
		register_port(mask303::ORIG_SpriteTask, (void *)mask303::SpriteTask, "303 SpriteTask", 303);
	}
}

