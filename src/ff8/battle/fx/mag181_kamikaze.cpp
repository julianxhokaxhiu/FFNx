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

// Effect 181: Kamikaze

#include "mag_common.h"

namespace ff8fx
{
namespace kamikaze181
{
	using namespace eng;
	using namespace magc;

	template<typename T> inline T &AT(uint32_t p, int32_t o) { return *(T *)(p + o); }
	inline int16_t &S16(uint32_t p, int32_t o) { return AT<int16_t>(p, o); }
	inline uint16_t &U16(uint32_t p, int32_t o) { return AT<uint16_t>(p, o); }
	inline int32_t &S32(uint32_t p, int32_t o) { return AT<int32_t>(p, o); }
	inline uint32_t &U32(uint32_t p, int32_t o) { return AT<uint32_t>(p, o); }

	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::MAG_181_Kamikaze_CastContext)); }
	inline uint32_t &CasterSlot() { return var<uint32_t>(game_addr(game::MAG_181_Kamikaze_CasterSlot)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::MAG_181_Kamikaze_MagicBufferBase)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_181_Kamikaze_PacketCursor)); }
	inline TaskQueue *EffectQueue() { return (TaskQueue *)game_addr(game::MAG_181_Kamikaze_EffectQueue); }
	inline uint32_t &FrameCursor() { return var<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)); }
	inline uint32_t &BoneWorkspace() { return var<uint32_t>(game_addr(game::g_BattleVertexWorkspace)); }
	inline uint32_t RenderOT(uint32_t off) { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + off; }
	static constexpr game::Id POOL = game::unk_2306600, POOL_END = game::MAG_181_Kamikaze_EffectQueue;

	static constexpr game::Id ORIG_RootTask = game::au_re_BdlinkTask_8;
	static constexpr game::Id ORIG_Director = game::MAG_181_Kamikaze_Director;
	static constexpr game::Id ORIG_CasterGlow = game::MAG_181_Kamikaze_CasterGlow;
	static constexpr game::Id ORIG_Ring = game::MAG_181_Kamikaze_Ring;
	static constexpr game::Id ORIG_Ring2 = game::MAG_181_Kamikaze_Ring2;
	static constexpr game::Id ORIG_Sparks = game::MAG_181_Kamikaze_Sparks;
	static constexpr game::Id ORIG_Smoke = game::MAG_181_Kamikaze_Smoke;
	static constexpr game::Id ORIG_Flare = game::MAG_181_Kamikaze_Flare;
	static constexpr game::Id MODEL_Ring = game::MAG_181_Kamikaze_ModelRing, MODEL_Ring2 = game::unk_D405FC;
	static constexpr game::Id SEQ_Spark = game::MAG_181_Kamikaze_SeqSpark, SEQ_Smoke = game::unk_D3E1F4, SEQ_Flare = game::unk_D3E500;
	static const void *SOUND_Start() { return (const void *)game_addr(game::MAG_181_Kamikaze_SoundStart); }

	inline void GteSetFarColor(uint32_t r, uint32_t g, uint32_t b) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_SetFarColor))(r, g, b); }
	inline void GteLoadRGBC(void *c) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_LoadRGBC))(c); }
	inline void GteDPCS() { fn<void (__cdecl *)()>(game_addr(game::GTE_DPCS))(); }
	inline void GteStoreRGB2(void *c) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreRGB2))(c); }
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
		int16_t zero;
		int16_t pos[4];
		int16_t spin;
		int16_t spin_speed;
		int16_t scale;
		int16_t scale_speed;
		uint32_t pad20;
	};
	struct FlareNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		int16_t pos[4];
		int16_t vel_x;
		int16_t vel_z;
		int16_t scale;
		int16_t pad1E;
		uint32_t pad20;
	};
	struct Particle
	{
		uint32_t type;
		int16_t frame;
		int16_t scale;
		int16_t pos[4];
		int16_t vel[3];
		int16_t pad16;
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x10 && sizeof(EffectNode) == 0x24, "Kamikaze nodes");
	static_assert(sizeof(Particle) == 0x18 && sizeof(FlareNode) == 0x24, "Kamikaze particle / flare");

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

	static uint32_t RenderObjectPrims(uint32_t h, uint32_t ot, uint32_t shift, uint32_t pkt)
	{
		const uint32_t verts = U32(h, 4);
		uint32_t prim = U32(h, 0);
		for (int32_t i = 0; i < (int32_t)U16(h, 8); i++, prim += 0x10)
		{
			GteLoadV012(verts + (U16(prim, 0) & 0xFFF) * 8, verts + (U16(prim, 2) & 0xFFF) * 8, verts + (U16(prim, 4) & 0xFFF) * 8);
			GteRTPT();
			const uint32_t uv1 = U32(prim, 0xC);
			U32(pkt, 0x14) = uv1 & 0x1FFFFFF;
			U32(pkt, 0) = 0x7000000;
			U32(pkt, 4) = (uv1 & 0x2000000) | U32(h, 0x1C) | 0x24000000;
			U32(pkt, 0xC) = U32(prim, 8);
			GteReadFLAG(h + 0x38);
			if (U32(h, 0x38) & 0x60000) continue;
			GteNCLIP();
			GteReadMAC0(h + 0x2C);
			if (S32(h, 0x2C) <= 0) continue;
			const uint32_t xy0 = pkt + 8, xy1 = pkt + 0x10, xy2 = pkt + 0x18;
			GteReadSXY012(xy0, xy1, xy2);
			GteAVSZ3();
			S32(pkt, 0x1C) = (int32_t)S16(prim, 6);
			GteReadOTZ32(h + 0x34);
			int32_t z = S32(h, 0x34) >> shift;
			InsertPrim(ot + (uint32_t)z * 4, pkt);
			if (z > 0) z--;
			pkt += 0x20;
			U32(pkt, 4) = U32(h, 0x5C) | 0x22000000;
			U32(pkt, 8) = U32(xy0, 0);
			U32(pkt, 0) = 0x4000000;
			U32(pkt, 0xC) = U32(xy1, 0);
			U32(pkt, 0x10) = U32(xy2, 0);
			InsertPrim(ot + (uint32_t)z * 4, pkt);
			pkt += 0x14;
		}
		if (U16(h, 0xA) != 0)
		{
			prim += 2;
			for (int32_t i = 0; i < (int32_t)U16(h, 0xA); i++, prim += 0x14)
			{
				GteLoadV012(verts + (U16(prim, -2) & 0xFFF) * 8, verts + (U16(prim, 0) & 0xFFF) * 8, verts + (U16(prim, 2) & 0xFFF) * 8);
				GteRTPT();
				const uint32_t uv1 = U32(prim, 0xA);
				U32(pkt, 0x14) = uv1 & 0x1FFFFFF;
				U32(pkt, 0) = 0x9000000;
				U32(pkt, 4) = (uv1 & 0x2000000) | U32(h, 0x1C) | 0x2C000000;
				U32(pkt, 0xC) = U32(prim, 6);
				GteReadFLAG(h + 0x38);
				if (U32(h, 0x38) & 0x60000) continue;
				GteNCLIP();
				GteReadMAC0(h + 0x2C);
				if (S32(h, 0x2C) <= 0) continue;
				const uint32_t xy0 = pkt + 8, xy1 = pkt + 0x10, xy2 = pkt + 0x18, xy3 = pkt + 0x20;
				GteReadSXY012(xy0, xy1, xy2);
				GteLoadV0((const void *)(verts + (U16(prim, 4) & 0xFFF) * 8));
				GteRTPS();
				const uint32_t uv23 = U32(prim, 0xE);
				U32(pkt, 0x1C) = uv23;
				S32(pkt, 0x24) = (int32_t)uv23 >> 16;
				GteReadSXY2((void *)xy3);
				GteAVSZ4();
				GteReadOTZ32(h + 0x34);
				int32_t z = S32(h, 0x34) >> shift;
				InsertPrim(ot + (uint32_t)z * 4, pkt);
				if (z > 0) z--;
				pkt += 0x28;
				U32(pkt, 4) = U32(h, 0x5C) | 0x2A000000;
				U32(pkt, 8) = U32(xy0, 0);
				U32(pkt, 0) = 0x5000000;
				U32(pkt, 0xC) = U32(xy1, 0);
				U32(pkt, 0x10) = U32(xy2, 0);
				U32(pkt, 0x14) = U32(xy3, 0);
				InsertPrim(ot + (uint32_t)z * 4, pkt);
				pkt += 0x18;
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
		const uint32_t anim = e + 0x60;
		if (!(entity[0] & 0x20)) FrameCursor() = DrawShadow(entity, RenderOT(0x4064), 0x10, FrameCursor());
		const uint32_t world = e + 0x40;
		ComposeAffineTransform(&Camera(), (const Mat4x3 *)world, (Mat4x3 *)(h + 0x60));
		ComputeBonesWorldMatrices((void *)anim, (void *)world);
		U32(h, 4) = BoneWorkspace();
		U32(h, 0x1C) = U32(e, 0x28);
		U16(h, 0x14) = 0;
		U16(h, 0x16) = 0;
		U32(h, 0x20) = U32(e, 0x7C);
		AT<uint8_t>(h, 0x2A) = entity[7];
		AT<uint8_t>(h, 0x29) = entity[7];
		AT<uint8_t>(h, 0x28) = entity[7];
		U32(h, 0x10) = var<uint32_t>(game_addr(game::BATTLE_PACKET_BUFFER_END));
		U16(h, 0x18) = 0x140;
		U16(h, 0x1A) = 0xD8;
		FrameCursor() = RenderModel(U32(anim, 4), h, RenderOT(0x44), 2, FrameCursor());
		BuildBoneMatricesFromPose((void *)anim);
		const uint32_t second = U32(e, 0x78);
		if (second && !(entity[1] & 2))
		{
			U32(h, 0x20) = 0xFFFFFFFF;
			ComputeBonesWorldMatrices((void *)second, (void *)world);
			FrameCursor() = RenderModel(U32(second, 4), h, RenderOT(0x44), 2, FrameCursor());
			BuildBoneMatricesFromPose((void *)second);
		}
	}

	static int32_t GlowLevel(int16_t counter) { return counter < 0xE ? (int32_t)counter * 0x124 : 0x1000; }

	static void DrawGlow(uint8_t *entity, int32_t level)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x80);
		uint32_t *colour = (uint32_t *)(h + 0x5C);
		*colour = 0;
		GteSetFarColor(0xF0, 0x60, 0x60);
		GteLoadRGBC(colour);
		GteSetIR0(level);
		GteDPCS();
		GteStoreRGB2(colour);
		*(uint16_t *)entity |= 0x804;
		DrawCasterModel((uint32_t)h, entity);
		FieldFree(0x80);
	}

	static int16_t DrawFlare(const FlareNode *f)
	{
		TransformCameraByShadowRotation(f->pos, (uint16_t)f->scale, -((int32_t)f->scale >> 3));
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		*(uint32_t *)(h + 0) = game_addr(SEQ_Flare);
		*(int16_t *)(h + 4) = f->counter;
		*(int16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(0x44), 2, PacketCursor());
		FieldFree(0xB4);
		return *(const int16_t *)(h + 0x28);
	}
	static void DrawSpriteParticle(uint8_t *h, const Particle *p)
	{
		*(int16_t *)(h + 4) = p->frame;
		TransformCameraByShadowRotation(p->pos, (uint16_t)p->scale, -((int32_t)p->scale >> 4));
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(0x44), 2, PacketCursor());
	}

	static void DrawRing(const EffectNode *t, uint32_t model, int16_t fade_start, int32_t fade)
	{
		int16_t angles[4];
		angles[0] = 0;
		angles[1] = t->spin;
		angles[2] = 0;
		angles[3] = 0;
		Mat4x3 m = {};
		ComposeZYXRotationMatrix(angles, &m);
		m.t[0] = t->pos[0];
		m.t[1] = t->pos[1];
		int32_t v[3];
		v[2] = t->scale;
		v[1] = t->scale;
		v[0] = t->scale;
		m.t[2] = t->pos[2];
		Scale3DMatrix(&m, v);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		*(uint32_t *)(h + 0) = model;
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0x1C) = 0x33;
		if (t->counter >= fade_start)
		{
			*(uint32_t *)(h + 0x1C) = 0xF3;
			*(int32_t *)(h + 0xC) = fade;
		}
		PacketCursor() = RenderPrimModel(h, RenderOT(0x44), 2, PacketCursor());
		FieldFree(0x58);
	}

	static void RingUpdate(EffectNode *t)
	{
		const int16_t ss = t->scale_speed;
		t->spin = (int16_t)(t->spin + t->spin_speed);
		t->spin_speed = (int16_t)(t->spin_speed - (int16_t)(t->spin_speed >> 4));
		t->scale = (int16_t)(t->scale + ss);
		t->scale_speed = (int16_t)(ss - (int32_t)ss / 7);
		t->counter++;
	}
	static void Ring2Update(EffectNode *t)
	{
		const int16_t ss = t->scale_speed;
		t->scale = (int16_t)(t->scale + ss);
		t->scale_speed = (int16_t)(ss - (int32_t)ss / 7);
		t->counter++;
	}
}
}

namespace ff8fx
{
namespace kamikaze181
{
	static uint32_t __cdecl CasterGlowTask(TaskNode *n)
	{
		EffectNode *t = (EffectNode *)n;
		if (t->counter < 0xF)
		{
			DrawGlow(Caster(), GlowLevel(t->counter));
		}
		if (DrawOnly()) return 0;
		t->counter++;
		return t->counter > 0xF ? TASK_END : 0;
	}
	static uint32_t __cdecl RingTask(TaskNode *n)
	{
		EffectNode *t = (EffectNode *)n;
		DrawRing(t, game_addr(MODEL_Ring), 8, shl32(t->counter - 8, 9));
		if (DrawOnly()) return 0;
		RingUpdate(t);
		return t->counter >= 0x10 ? TASK_END : 0;
	}

	static uint32_t __cdecl Ring2Task(TaskNode *n)
	{
		EffectNode *t = (EffectNode *)n;
		DrawRing(t, game_addr(MODEL_Ring2), 0xA, shl32(t->counter - 0xA, 9));
		if (DrawOnly()) return 0;
		Ring2Update(t);
		return t->counter >= 0x12 ? TASK_END : 0;
	}

	static uint8_t *SpriteHeader(uint32_t seq)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		*(uint32_t *)(h + 0) = seq;
		*(int16_t *)(h + 0x24) = 0;
		return h;
	}

	static uint32_t __cdecl SparksTask(TaskNode *n)
	{
		EffectNode *t = (EffectNode *)n;
		uint8_t *h = SpriteHeader(game_addr(SEQ_Spark));
		int alive = 0;
		for (Particle *p = Pool(); (uint32_t)p < game_addr(POOL_END); p++)
		{
			if (!(p->type & 2)) continue;
			DrawSpriteParticle(h, p);
			if (DrawOnly()) continue;
			p->frame++;
			if (*(int16_t *)(h + 0x28) < 0)
			{
				p->type = 0;
				continue;
			}
			p->pos[0] = (int16_t)(p->pos[0] + p->vel[0]);
			p->pos[1] = (int16_t)(p->pos[1] + p->vel[1]);
			p->pos[2] = (int16_t)(p->pos[2] + p->vel[2]);
			p->vel[0] = (int16_t)(p->vel[0] - (int16_t)(p->vel[0] >> 2));
			p->vel[1] = (int16_t)(p->vel[1] - (int16_t)(p->vel[1] >> 2));
			p->vel[2] = (int16_t)(p->vel[2] - (int16_t)(p->vel[2] >> 2));
			alive++;
		}
		FieldFree(0xB4);
		if (DrawOnly()) return 0;
		if (t->counter >= 0 && t->counter <= 7)
		{
			for (int k = 0; k < 7; k++)
			{
				const int i = FreeParticle();
				if (i < 0) break;
				Particle *p = &Pool()[i];
				p->type = 2;
				p->frame = 0;
				p->scale = (int16_t)(CrtRand() % 0x1000 + 0x800);
				ff8fx::copy<(8)>(p->pos, t->pos);
				int32_t v[3];
				v[0] = CrtRand() % 0x1000 - 0x800;
				v[1] = CrtRand() % 0x1000 - 0x800;
				v[2] = CrtRand() % 0x1000 - 0x800;
				NormalizeVector(v, v);
				const int32_t d = CrtRand() % 0x258 + 0x190;
				const int32_t dx = mul32(d, v[0]), dy = mul32(d, v[1]), dz = mul32(d, v[2]);
				p->pos[0] = (int16_t)(p->pos[0] + (dx >> 12));
				p->pos[1] = (int16_t)(p->pos[1] + (dy >> 12));
				p->pos[2] = (int16_t)(p->pos[2] + (dz >> 12));
				const int32_t s = CrtRand() % 0xFA + 0x122;
				p->vel[0] = (int16_t)(mul32(s, v[0]) >> 12);
				p->vel[1] = (int16_t)(mul32(s, v[1]) >> 12);
				p->vel[2] = (int16_t)(mul32(s, v[2]) >> 12);
			}
		}
		t->counter++;
		if (t->counter >= 4 && alive == 0) return TASK_END;
		return 0;
	}

	static uint32_t __cdecl SmokeTask(TaskNode *n)
	{
		EffectNode *t = (EffectNode *)n;
		uint8_t *h = SpriteHeader(game_addr(SEQ_Smoke));
		int alive = 0;
		for (Particle *p = Pool(); (uint32_t)p < game_addr(POOL_END); p++)
		{
			if (!(p->type & 4)) continue;
			DrawSpriteParticle(h, p);
			if (DrawOnly()) continue;
			p->frame++;
			if (*(int16_t *)(h + 0x28) < 0) p->type = 0;
			else alive++;
		}
		FieldFree(0xB4);
		if (DrawOnly()) return 0;
		if (t->counter >= 0 && t->counter <= 0xD)
		{
			for (int k = 0; k < 2; k++)
			{
				const int i = FreeParticle();
				if (i < 0) break;
				Particle *p = &Pool()[i];
				p->type = 4;
				p->frame = 0;
				p->scale = (int16_t)(CrtRand() % 0xF00 + 0xA00);
				ff8fx::copy<(8)>(p->pos, t->pos);
				p->pos[0] = (int16_t)(p->pos[0] + (CrtRand() % 0x4B0 - 0x258));
				p->pos[1] = (int16_t)(p->pos[1] + (CrtRand() % 0x4B0 - 0x258));
				p->pos[2] = (int16_t)(p->pos[2] + (CrtRand() % 0x4B0 - 0x258));
			}
		}
		t->counter++;
		if (t->counter >= 4 && alive == 0) return TASK_END;
		return 0;
	}

	static uint32_t __cdecl FlareTask(TaskNode *n)
	{
		FlareNode *f = (FlareNode *)n;
		const int16_t next = DrawFlare(f);
		if (DrawOnly()) return 0;
		f->pos[2] = (int16_t)(f->pos[2] + f->vel_z);
		f->pos[0] = (int16_t)(f->pos[0] + f->vel_x);
		f->pos[1] = (int16_t)(f->pos[1] + 2);
		f->counter++;
		return next < 0 ? TASK_END : 0;
	}
	static uint32_t __cdecl DirectorTask(TaskNode *n)
	{
		if (DrawOnly()) return 0;
		EffectNode *t = (EffectNode *)n;
		if (t->counter == 0) GetEffectSpawnPosition(Caster(), 0xF0, 0xA00, t->pos);
		if (t->counter == 1)
		{
			EffectNode *g = (EffectNode *)AddTaskToQueue(EffectQueue(), game_addr(ORIG_CasterGlow));
			g->counter = 0;
		}
		if (t->counter >= 4 && t->counter <= 5)
		{
			int16_t p[4];
			GetEffectSpawnPosition(Caster(), 0xF0, 0xA00, p);
			int32_t v[3];
			v[1] = 0;
			v[0] = (int32_t)*(const int16_t *)(Caster() + 0x1C) - p[0];
			v[2] = (int32_t)*(const int16_t *)(Caster() + 0x20) - p[2];
			NormalizeVector(v, v);
			for (int k = 0; k < t->zero; k++)
			{
				FlareNode *f = (FlareNode *)AddTaskToQueue(EffectQueue(), game_addr(ORIG_Flare));
				f->counter = 0;
				f->pos[0] = p[0];
				const int32_t r = CrtRand() % 0xA;
				f->pos[1] = (int16_t)(r + t->pos[1] - 5);
				f->pos[2] = p[2];
				const int32_t s = CrtRand() % 0x4B + 0x19;
				f->vel_x = (int16_t)(mul32(s, v[0]) >> 12);
				f->vel_z = (int16_t)(mul32(s, v[2]) >> 12);
				f->scale = (int16_t)(CrtRand() % 0x100 + 0x800);
			}
			t->zero = (int16_t)(t->zero - 1);
		}
		if (t->counter == 0x10)
		{
			EffectNode *r = (EffectNode *)AddTaskToQueue(EffectQueue(), game_addr(ORIG_Ring));
			ff8fx::copy<(8)>(r->pos, t->pos);
			r->counter = 0;
			r->zero = 0;
			r->spin = (int16_t)(CrtRand() % 0x1000);
			const int32_t s = CrtRand() % 0x6E;
			r->scale_speed = 0x1CC;
			r->scale = 0x1CC;
			r->spin_speed = (int16_t)(s + 0x3C);
			r = (EffectNode *)AddTaskToQueue(EffectQueue(), game_addr(ORIG_Ring));
			ff8fx::copy<(8)>(r->pos, t->pos);
			r->counter = 0;
			r->zero = 0;
			r->spin = (int16_t)(CrtRand() % 0x1000);
			const int32_t s2 = CrtRand() % 0x82;
			r->spin_speed = (int16_t)(-0x50 - s2);
			r->scale_speed = 0x366;
			r->scale = 0x366;
		}
		if (t->counter == 0xE) ScreenFadeTask(0, 1, 4, 0xFF);
		if (t->counter == 0xF)
		{
			GetDefaultEffectPosition(Caster(), t->pos);
			EffectNode *r = (EffectNode *)AddTaskToQueue(EffectQueue(), game_addr(ORIG_Ring2));
			ff8fx::copy<(8)>(r->pos, t->pos);
			r->counter = 0;
			r->spin = (int16_t)(CrtRand() % 0x1000);
			r->scale_speed = 0x580;
			r->scale = 0x580;
		}
		if (t->counter == 0xF)
		{
			EffectNode *s = (EffectNode *)AddTaskToQueue(EffectQueue(), game_addr(ORIG_Sparks));
			ff8fx::copy<(8)>(s->pos, t->pos);
			s->counter = 0;
		}
		if (t->counter == 0x10)
		{
			EffectNode *s = (EffectNode *)AddTaskToQueue(EffectQueue(), game_addr(ORIG_Smoke));
			ff8fx::copy<(8)>(s->pos, t->pos);
			s->counter = 0;
		}
		if (t->counter == 0x26) ApplyActionResultToTarget(Ctx()->actions[0].targets);
		if (t->counter == 1) BdPlaySE(SOUND_Start(), 0, 0x80);
		t->counter++;
		return t->counter > 0x32 ? TASK_END : 0;
	}
	static uint32_t __cdecl RootTask(TaskNode *n)
	{
		RootNode *r = (RootNode *)n;
		if (r->counter & 1) PacketCursor() = TexBase() + 0xC000;
		else PacketCursor() = TexBase();
		const int a = ExecuteTaskQueue(EffectQueue());
		r->counter++;
		return a ? 0 : TASK_END;
	}
}

	void register_mag181_kamikaze()
	{
		register_port(kamikaze181::ORIG_RootTask, (void *)kamikaze181::RootTask, "K181 RootTask", 181);
		register_port(kamikaze181::ORIG_Director, (void *)kamikaze181::DirectorTask, "K181 Director", 181);
		register_port(kamikaze181::ORIG_CasterGlow, (void *)kamikaze181::CasterGlowTask, "K181 CasterGlow", 181);
		register_port(kamikaze181::ORIG_Ring, (void *)kamikaze181::RingTask, "K181 Ring", 181);
		register_port(kamikaze181::ORIG_Ring2, (void *)kamikaze181::Ring2Task, "K181 Ring2", 181);
		register_port(kamikaze181::ORIG_Sparks, (void *)kamikaze181::SparksTask, "K181 Sparks", 181);
		register_port(kamikaze181::ORIG_Smoke, (void *)kamikaze181::SmokeTask, "K181 Smoke", 181);
		register_port(kamikaze181::ORIG_Flare, (void *)kamikaze181::FlareTask, "K181 Flare", 181);
	}
}

