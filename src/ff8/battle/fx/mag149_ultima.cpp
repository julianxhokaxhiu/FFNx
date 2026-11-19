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

// Effect 149: Ultima

#include "mag_common.h"

namespace ff8fx
{
namespace ultima149
{
	using namespace eng;
	using namespace magc;

	inline int32_t &RingScale() { return var<int32_t>(game_addr(game::g_Ultima_RingScale)); }
	inline TaskQueue &RootQueue() { return var<TaskQueue>(game_addr(game::MAG_149_RootQueue)); }
	inline TaskQueue &FxQueue() { return var<TaskQueue>(game_addr(game::MAG_149_FxQueue)); }
	inline uint32_t *SideFlags() { return (uint32_t *)game_addr(game::MAG_149_SlotFadeFlags); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::MAG_149_Ctx)); }
	inline uint8_t *CoreVertices() { return (uint8_t *)game_addr(game::g_Ultima_CoreVertices); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::MAG_149_TexBase)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_149_PacketCursor)); }
	inline uint32_t &VoiceSlot() { return var<uint32_t>(game_addr(game::MAG_149_VoiceSlot)); }

	static constexpr game::Id ORIG_Director = game::MAG_149_ULTIMA_Director_Tick;
	static constexpr game::Id ORIG_Orbit = game::MAG_149_Orbit;
	static constexpr game::Id ORIG_Streaks = game::MAG_149_Streaks;
	static constexpr game::Id ORIG_Shards = game::MAG_149_Shards;
	static constexpr game::Id ORIG_Energy = game::MAG_149_Energy;
	static constexpr game::Id ORIG_Ring = game::MAG_149_Ring;
	static constexpr game::Id ORIG_Core = game::MAG_149_ULTIMA_CoreMorphMesh_Tick;
	static constexpr game::Id ORIG_Dust = game::MAG_149_Dust;
	static constexpr game::Id ORIG_Sparks = game::MAG_149_Sparks;
	static constexpr game::Id ORIG_SideFadeIn = game::MAG_149_SideFadeIn;
	static constexpr game::Id ORIG_SideFadeOut = game::MAG_149_SideFadeOut;
	static constexpr game::Id ORIG_Root = game::MAG_149_RootTask;
	static constexpr game::Id ORIG_WaitTexRestore = game::MAG_149_WaitTexRestore;

	static constexpr game::Id ARRAY_A = game::MAG_149_ArrayA, ARRAY_B = game::MAG_149_ArrayB;
	static const int ARRAY_COUNT = 0x64;
	static const void *SOUND_Voice() { return (const void *)game_addr(game::unk_DC9ABC); }
	static const void *SOUND_Start() { return (const void *)game_addr(game::unk_DC9BD0); }
	static const void *SOUND_Blast() { return (const void *)game_addr(game::unk_DC9BD4); }
	static constexpr game::Id SEQ_Orbit = game::unk_DC333C;
	static constexpr game::Id SEQ_Dust = game::unk_DC312C;
	static constexpr game::Id SEQ_Spark = game::unk_DC35B8;
	static constexpr game::Id MODEL_Streak = game::unk_DC8BC4;
	static constexpr game::Id MODEL_Shard = game::unk_DC8804;
	static constexpr game::Id MODEL_Energy = game::unk_DC7F54;
	static constexpr game::Id MODEL_Ring = game::unk_DC3784;
	static constexpr game::Id MODEL_Dome = game::unk_DC5254;
	static constexpr game::Id MODEL_Core = game::unk_DC6EA4;
	static constexpr game::Id CORE_Shape1 = game::unk_DC7B94;
	static const int CORE_VERTS = 0x78;
	static const int32_t *RING_Fades() { return (const int32_t *)game_addr(game::unk_DC9BDC); }

	inline uint32_t RenderOT44() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44; }

	inline int32_t ComputeCos(int32_t angle) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(angle); }
	inline Mat4x3 *ShadowCamera(const void *pos, int32_t scale, int32_t c) { return fn<Mat4x3 *(__cdecl *)(const void *, int32_t, int32_t)>(game_addr(game::TransformCameraByShadowRotation))(pos, scale, c); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void MatrixMultiplyVector(const void *m, const void *in, void *out) { fn<void (__cdecl *)(const void *, const void *, void *)>(game_addr(game::matrixMultiplyVector))(m, in, out); }
	inline void NormalizeSVector(const int16_t *in, int16_t *out) { fn<void (__cdecl *)(const int16_t *, int16_t *)>(game_addr(game::VectorNormalS16))(in, out); }
	inline uint32_t VoiceSlotBusy(uint32_t slot) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::BdSound_IsVoiceSlotActive))(slot); }
	inline void ReleaseVoiceSlot(uint32_t slot) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::BdSound_ReleaseVoiceSlot))(slot); }
	inline void GteLoadV012(const void *a, const void *b, const void *c) { fn<void (__cdecl *)(const void *, const void *, const void *)>(game_addr(game::GTE_LoadV012))(a, b, c); }
	inline void GteRTPT() { fn<void (__cdecl *)()>(game_addr(game::GTE_RTPT))(); }
	inline void GteNCLIP() { fn<void (__cdecl *)()>(game_addr(game::GTE_NCLIP))(); }
	inline void GteReadMAC0(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadMAC0))(dst); }
	inline void GteReadFLAG(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadFLAG))(dst); }
	inline void GteReadSXY012Split(void *a, void *b, void *c) { fn<void (__cdecl *)(void *, void *, void *)>(game_addr(game::GTE_ReadSXY012_Split))(a, b, c); }
	inline void GteAVSZ4() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ4))(); }
	inline void GteStoreOTZ(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadOTZ))(dst); }
	inline void GteSetFarColour(uint32_t r, uint32_t g, uint32_t b) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_SetFarColor))(r, g, b); }
	inline void GteLoadRGBC(const void *c) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadRGBC))(c); }
	inline void GteLoadRGB012(const void *a, const void *b, const void *c) { fn<void (__cdecl *)(const void *, const void *, const void *)>(game_addr(game::GTE_LoadRGB012))(a, b, c); }
	inline void GteDPCS() { fn<void (__cdecl *)()>(game_addr(game::GTE_DPCS))(); }
	inline void GteDPCT() { fn<void (__cdecl *)()>(game_addr(game::GTE_DPCT))(); }
	inline void GteStoreRGB2(void *c) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreRGB2))(c); }
	inline void GteStoreRGB012(void *a, void *b, void *c) { fn<void (__cdecl *)(void *, void *, void *)>(game_addr(game::GTE_StoreRGB012))(a, b, c); }

#pragma pack(push, 1)
	struct RootNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t done;
	};
	struct DirectorNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t action;
		int16_t pos[4];
		uint8_t pad18[8];
		int16_t target;
		int16_t pad22;
	};
	struct PartNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t index;
		int16_t pos[4];
		int16_t rot;
		int16_t rotvel;
		int16_t scale;
		int16_t dscale;
		int16_t pad20[2];
	};
	struct Particle
	{
		uint32_t type;
		int16_t frame;
		int16_t scale;
		int16_t pos[3];
		int16_t angle;
		int16_t vel[3];
		int16_t spin;
	};
	struct StreakWork
	{
		uint8_t pad00[8];
		Mat4x3 m;
		Mat4x3 rot;
		int32_t scale[4];
		int32_t up[4];
		int32_t dir[4];
		int32_t axis[4];
	};
	struct ShardWork
	{
		int16_t sv[4];
		int16_t pos[4];
		Mat4x3 m;
		Mat4x3 rot;
		int32_t scale[4];
		int32_t ref[4];
		int32_t dir[4];
		int32_t axis[4];
		int32_t view[4];
	};
	struct MeshHeader
	{
		uint32_t model;
		uint8_t *verts;
		uint8_t far_rgb[4];
		int32_t fade;
		int32_t uscroll;
		int32_t vscroll;
		uint16_t win_w;
		uint16_t win_h;
		uint16_t u_wrap;
		uint16_t v_wrap;
		uint32_t flags;
		uint8_t *cursor;
		int32_t mac0;
		uint32_t pad2C;
		int32_t otz;
		uint32_t gte_flag;
		uint8_t pad38[0x24];
		int32_t uv[4];
		int32_t du, dv;
		uint32_t texwin;
		uint32_t texwin_off;
		uint32_t drawmode;
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x10 && sizeof(DirectorNode) == 0x24 && sizeof(PartNode) == 0x24, "Ultima nodes");
	static_assert(sizeof(Particle) == 0x18 && sizeof(StreakWork) == 0x88 && sizeof(ShardWork) == 0xA0 && sizeof(MeshHeader) == 0x80, "Ultima structs");

	inline Particle *ArrayA() { return (Particle *)game_addr(ARRAY_A); }
	inline Particle *ArrayB() { return (Particle *)game_addr(ARRAY_B); }

	static Particle *FreeParticle(Particle *a)
	{
		for (int i = 0; i < ARRAY_COUNT; i++)
			if (a[i].type == 0) return &a[i];
		return nullptr;
	}

	inline uint32_t &H32(uint8_t *h, int off) { return *(uint32_t *)(h + off); }

	inline int32_t Hi(int32_t x, int32_t m) { return (int32_t)(((int64_t)x * m) >> 32); }
	inline int32_t Div3(int32_t x) { int32_t h = Hi(x, 0x55555556); return h + (int32_t)((uint32_t)h >> 31); }
	inline int32_t Div6(int32_t x) { int32_t h = Hi(x, 0x2AAAAAAB); return h + (int32_t)((uint32_t)h >> 31); }
	inline int32_t Div12(int32_t x) { int32_t h = Hi(x, 0x2AAAAAAB) >> 1; return h + (int32_t)((uint32_t)h >> 31); }
	inline int32_t Div20(int32_t x) { int32_t h = Hi(x, 0x66666667) >> 3; return h + (int32_t)((uint32_t)h >> 31); }
	inline int32_t Div40(int32_t x) { int32_t h = Hi(x, 0x66666667) >> 4; return h + (int32_t)((uint32_t)h >> 31); }
	inline int32_t Div14(int32_t x) { int32_t h = (Hi(x, (int32_t)0x92492493) + x) >> 3; return h + (int32_t)((uint32_t)h >> 31); }
	inline int32_t Neg(int32_t x) { return (int32_t)(0u - (uint32_t)x); }

	inline uint8_t *EntityOf(int slot) { return Entity(slot); }
}
}

namespace ff8fx
{
namespace ultima149
{
	static void Identity3x3(Mat4x3 *m)
	{
		uint32_t *d = (uint32_t *)m;
		d[3] = 0;
		d[2] = 0;
		d[1] = 0;
		d[0] = 0;
		m->m[2][2] = 0x1000;
		m->m[1][1] = 0x1000;
		m->m[0][0] = 0x1000;
	}

	static void AverageTargetPosition(const uint8_t *targets, int32_t count, int16_t *out)
	{
		int16_t sx = 0, sy = 0, sz = 0;
		int16_t a[4] = { 0, 0, 0, 0 };
		for (int32_t left = count; left > 0; left--)
		{
			GetDefaultEffectPosition(EntityOf(targets[0]), a);
			sx = (int16_t)(sx + a[0]);
			sy = (int16_t)(sy + a[1]);
			sz = (int16_t)(sz + a[2]);
			targets += TARGET_STRIDE;
		}
		a[0] = (int16_t)(sx / count);
		a[1] = (int16_t)(sy / count);
		a[2] = (int16_t)(sz / count);
		ff8fx::copy<(8)>(out, a);
	}

	static void FlashLayersSetBit()
	{
		for (uint8_t *p = (uint8_t *)game_addr(game::unk_1D98991); p < (uint8_t *)game_addr(game::unk_1D98A41); p += 0x2C) *p |= 2;
	}
	static void FlashLayersClearBit()
	{
		for (uint8_t *p = (uint8_t *)game_addr(game::unk_1D98991); p < (uint8_t *)game_addr(game::unk_1D98A41); p += 0x2C) *p &= 0xFD;
	}

	static void SideRange(int32_t slot, int *first, int *end)
	{
		if (slot >= 3) { *first = 0; *end = 3; }
		else { *first = 3; *end = 7; }
	}

	static void SpawnSideFadeIn(int32_t duration, int32_t slot)
	{
		int i, end;
		SideRange(slot, &i, &end);
		for (; i < end; i++)
		{
			if (SideFlags()[i * 6] == 0) continue;
			PartNode *n = (PartNode *)AddTaskToQueue(&FxQueue(), game_addr(ORIG_SideFadeIn));
			n->counter = 0;
			n->index = (int16_t)i;
			n->rot = (int16_t)duration;
			n->rotvel = (int16_t)(*(uint16_t *)EntityOf(i) & 0x800);
		}
	}

	static void SpawnSideFadeOut(int32_t duration, int32_t slot)
	{
		int i, end;
		SideRange(slot, &i, &end);
		for (; i < end; i++)
		{
			uint16_t f = *(uint16_t *)EntityOf(i);
			if ((f & 2) && !(f & 4))
			{
				SideFlags()[i * 6] = 1;
				PartNode *n = (PartNode *)AddTaskToQueue(&FxQueue(), game_addr(ORIG_SideFadeOut));
				n->counter = 0;
				n->index = (int16_t)i;
				n->rot = (int16_t)duration;
				n->rotvel = (int16_t)(*(uint16_t *)EntityOf(i) & 0x800);
			}
			else SideFlags()[i * 6] = 0;
		}
	}

	static void SideColours(uint8_t *e, int32_t level, uint32_t r, uint32_t g, uint32_t b)
	{
		uint32_t col = 0x32000000;
		GteSetFarColour(var<uint8_t>(game_addr(game::CURRENT_BS_SHADOW_PALETTE)), var<uint8_t>(game_addr(game::unk_B8B9A9)), var<uint8_t>(game_addr(game::unk_B8B9AA)));
		GteLoadRGBC(&col);
		GteSetIR0(level);
		GteDPCS();
		GteStoreRGB2(e + 0x2C);
		GteSetFarColour(r, g, b);
		col = 0;
		GteLoadRGBC(&col);
		GteDPCS();
		GteStoreRGB2(e + 0x28);
	}

	static uint32_t __cdecl SideFadeInTask(TaskNode *nd)
	{
		PartNode *n = (PartNode *)nd;
		int32_t level = shl32(n->counter, 12) / n->rot;
		uint8_t *e = EntityOf(n->index);
		SideColours(e, level, var<uint8_t>(game_addr(game::CURRENT_BS_MODEL_PALETTE)), var<uint8_t>(game_addr(game::unk_B8B7D9)), var<uint8_t>(game_addr(game::unk_B8B7DA)));
		e[0x2B] = 2;
		if (DrawOnly()) return 0;
		if (n->counter == 0) *(uint16_t *)e = (uint16_t)((*(uint16_t *)e & 0xFFFB) | 0x800);
		n->counter++;
		if (n->counter < n->rot) return 0;
		*(uint32_t *)(e + 0x28) = var<uint32_t>(game_addr(game::CURRENT_BS_MODEL_PALETTE));
		*(uint16_t *)e = (uint16_t)((*(uint16_t *)e & 0xF7FF) | (uint16_t)n->rotvel);
		return TASK_END;
	}

	static uint32_t __cdecl SideFadeOutTask(TaskNode *nd)
	{
		PartNode *n = (PartNode *)nd;
		int32_t level = 0x1000 - shl32(n->counter, 12) / n->rot;
		uint8_t *e = EntityOf(n->index);
		e[7] = 0;
		SideColours(e, level, var<uint8_t>(game_addr(game::CURRENT_BS_MODEL_PALETTE)) >> 1, var<uint8_t>(game_addr(game::unk_B8B7D9)) >> 1, var<uint8_t>(game_addr(game::unk_B8B7DA)) >> 1);
		e[0x2B] = 2;
		if (DrawOnly()) return 0;
		if (n->counter == 0) e[1] |= 8;
		n->counter++;
		if (n->counter < n->rot) return 0;
		*(uint16_t *)e = (uint16_t)((*(uint16_t *)e & 0xF7FF) | (uint16_t)n->rotvel | 4);
		return TASK_END;
	}

	static PartNode *SpawnPart(uint32_t task, const DirectorNode *d, bool swap)
	{
		PartNode *n = (PartNode *)AddTaskToQueue(&FxQueue(), task);
		ff8fx::copy<(8)>(n->pos, d->pos);
		if (swap)
		{
			int16_t t = n->pos[1];
			n->pos[1] = n->pos[3];
			n->pos[3] = t;
		}
		n->counter = 0;
		return n;
	}

	static uint32_t __cdecl DirectorTask(TaskNode *nd)
	{
		if (DrawOnly()) return 0;
		DirectorNode *d = (DirectorNode *)nd;
		if (d->counter == 0)
		{
			ActionData *a = &Ctx()->actions[d->action];
			AverageTargetPosition(a->targets, a->target_count, d->pos);
			d->pos[1] = 0;
			d->pos[3] = (int16_t)0xF95C;
			d->target = (int16_t)Ctx()->actions[d->action].targets[0];
			d->pad22 = 0;
			VoiceSlot() = ClaimVoiceSlot(SOUND_Voice(), 1, 0x80);
		}
		if (d->counter == 1) SpawnPart(game_addr(ORIG_Orbit), d, false);
		if (d->counter == 0xA) SpawnPart(game_addr(ORIG_Streaks), d, true);
		if (d->counter == 0x23) SpawnPart(game_addr(ORIG_Shards), d, true);
		if (d->counter == 1)
		{
			PartNode *n = SpawnPart(game_addr(ORIG_Energy), d, true);
			n->rot = (int16_t)(CrtRand() % 0x1000);
			n->rotvel = 0x28;
			n->scale = 0;
			n->dscale = 0xC;
			n = SpawnPart(game_addr(ORIG_Energy), d, true);
			n->rot = (int16_t)(CrtRand() % 0x1000);
			n->rotvel = -0x28;
			n->scale = 0;
			n->dscale = 0x19;
		}
		if (d->counter == 0x5E)
		{
			PartNode *n = SpawnPart(game_addr(ORIG_Ring), d, false);
			n->rot = (int16_t)(CrtRand() % 0x1000);
			n->scale = 0x1000;
			n->dscale = 0x249;
		}
		if (d->counter == 0x64)
		{
			PartNode *n = SpawnPart(game_addr(ORIG_Core), d, false);
			n->index = 0;
			n->rot = (int16_t)(CrtRand() % 0x1000);
			n->rotvel = 0xC8;
		}
		if (d->counter == 0x5E) SpawnPart(game_addr(ORIG_Dust), d, false);
		if (d->counter == 0x5F) SpawnPart(game_addr(ORIG_Sparks), d, false);
		if (d->counter == 0) SpawnSideFadeOut(0x18, d->target);
		if (d->counter == 0xA0) SpawnSideFadeIn(8, d->target);
		if (d->action == 0 && d->counter <= 0x18)
		{
			if (d->counter == 0x18) FlashLayersClearBit();
			else SetScreenFlash((uint32_t)(d->counter * 0xAA), 0);
		}
		else if (d->action == (int16_t)(uint16_t)Ctx()->actions[0].last_action && d->counter >= 0xA0)
		{
			if (d->counter == 0xA0) FlashLayersSetBit();
			else SetScreenFlash((uint32_t)shl32(0xA8 - d->counter, 9), 0);
		}
		if (d->counter == 0x5B) ScreenFadeTask(3, 1, 8, 0xFF);
		if (d->counter == 0x8A) ScreenFadeTask(0xF, 8, 0xA, 0xFF);
		if (d->counter == 0xA6)
		{
			ActionData *a = &Ctx()->actions[d->action];
			ApplyActionResultToTargets(a->targets, a->target_count);
		}
		if (d->counter == 0xA7)
		{
			int32_t next = d->action + 1;
			if (next <= (int32_t)Ctx()->actions[0].last_action)
			{
				DirectorNode *n = (DirectorNode *)AddTaskToQueue(&FxQueue(), game_addr(ORIG_Director));
				n->counter = 0;
				n->action = (int16_t)next;
			}
		}
		int16_t at[4];
		if (d->counter == 1)
		{
			ActionData *a = &Ctx()->actions[d->action];
			AverageTargetPosition(a->targets, a->target_count, at);
			BdPlaySE3D(SOUND_Start(), 1, at);
		}
		if (d->counter == 0x5E)
		{
			ActionData *a = &Ctx()->actions[d->action];
			AverageTargetPosition(a->targets, a->target_count, at);
			BdPlaySE3D(SOUND_Blast(), 1, at);
		}
		if (d->counter == 0xA5)
		{
			if (VoiceSlotBusy(VoiceSlot())) ReleaseVoiceSlot(VoiceSlot());
		}
		d->counter++;
		if (d->counter <= 0xA8) return 0;
		if (d->action == (int16_t)(uint16_t)Ctx()->actions[0].last_action) SetScreenFlash(0, 0);
		return TASK_END;
	}

	static void OrbitDraw(uint8_t *h, int16_t *p, const Particle *e)
	{
		*(uint16_t *)(h + 4) = (uint16_t)Div3(e->frame);
		int32_t angle = e->vel[0];
		int32_t radius = e->pos[0];
		p[4] = (int16_t)((mul32(ComputeCos(angle), radius) >> 12) + p[0]);
		p[5] = (int16_t)((mul32(ComputeSin(2 * angle), e->pos[2]) >> 12) + e->pos[1]);
		p[6] = (int16_t)((mul32(ComputeSin(angle), radius) >> 12) + p[2]);
		ShadowCamera(p + 4, e->scale, Neg(e->scale >> 4));
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT44(), 2, PacketCursor());
	}

	static void OrbitSpawn()
	{
		for (int k = 0; k < 2; k++)
		{
			Particle *e = FreeParticle(ArrayB());
			if (!e) return;
			e->type = 1;
			e->frame = 0;
			e->scale = (int16_t)(CrtRand() % 0x500 + 0x300);
			e->pos[0] = (int16_t)(CrtRand() % 0x1388 + 0x384);
			e->pos[1] = (int16_t)(-300 - CrtRand() % 0x7D0);
			e->pos[2] = (int16_t)(CrtRand() % 0x12C + 0x1E);
			e->vel[0] = (int16_t)(CrtRand() % 0x1000);
			e->vel[1] = 0;
			e->vel[2] = (int16_t)(CrtRand() % 0x1E + 0xA);
		}
	}

	static uint8_t *OrbitBegin(const PartNode *n, int16_t **pos)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		int16_t *p = (int16_t *)FieldAlloc(0x10);
		ff8fx::copy<(8)>(p, n->pos);
		H32(h, 0) = game_addr(SEQ_Orbit);
		*(uint16_t *)(h + 0x24) = 0;
		*pos = p;
		return h;
	}

	static uint32_t __cdecl OrbitTask(TaskNode *nd)
	{
		PartNode *n = (PartNode *)nd;
		int16_t *p;
		uint8_t *h = OrbitBegin(n, &p);
		int alive = 0;
		Particle *e = ArrayB();
		for (int i = 0; i < ARRAY_COUNT; i++, e++)
		{
			if (!(*(uint8_t *)&e->type & 1)) continue;
			OrbitDraw(h, p, e);
			if (DrawOnly()) continue;
			e->frame++;
			if (*(int16_t *)(h + 0x28) < 0) e->type = 0;
			else
			{
				e->vel[0] = (int16_t)(e->vel[0] + e->vel[2]);
				alive++;
			}
		}
		FieldFree(0x10);
		FieldFree(0xB4);
		if (DrawOnly()) return 0;
		if (n->counter >= 0 && n->counter <= 0x14) OrbitSpawn();
		n->counter++;
		if (n->counter >= 0x18 && alive == 0) return TASK_END;
		return 0;
	}

	static void StreakDraw(uint8_t *h, StreakWork *w, const Particle *e)
	{
		Identity3x3(&w->m);
		w->m.t[0] = e->pos[0];
		w->m.t[1] = e->pos[1];
		w->m.t[2] = e->pos[2];
		w->dir[0] = e->vel[0];
		w->dir[1] = e->vel[1];
		w->dir[2] = e->vel[2];
		NormalizeVector(w->dir, w->dir);
		int32_t angle = RotationBetweenVectors(w->up, w->dir, w->axis);
		BuildAxisAngleRotationMatrix(angle, &w->rot, w->axis);
		MatrixMultiply(&w->rot, &w->m);
		w->scale[2] = 0x1000;
		w->scale[0] = 0x1000;
		w->scale[1] = e->scale;
		Scale3DMatrix(&w->m, w->scale);
		ComposeAffineTransform(&Camera(), &w->m, &w->m);
		GteSetRotMatrix(&w->m);
		GteSetTransVector(&w->m);
		if (e->frame >= 4)
		{
			int32_t fade = shl32(e->frame - 4, 10);
			*(int32_t *)(h + 0xC) = fade;
			H32(h, 0x1C) |= 0xC0;
		}
		PacketCursor() = RenderPrimModel(h, RenderOT44(), 2, PacketCursor());
	}

	static void StreakSpawn(const PartNode *n)
	{
		int32_t count = n->counter / 16 + 1;
		for (int32_t k = 0; k < count; k++)
		{
			Particle *e = FreeParticle(ArrayB());
			if (!e) return;
			e->type = 2;
			e->frame = 0;
			e->scale = (int16_t)(CrtRand() % 0xE00 + 0x1000);
			ff8fx::copy<(4)>(&e->pos[0], &n->pos[0]);
			ff8fx::copy<(4)>(&e->pos[2], &n->pos[2]);
			int32_t v[3];
			v[0] = CrtRand() % 0x1000 - 0x800;
			v[1] = CrtRand() % 0x1000 - 0x800;
			v[2] = CrtRand() % 0x1000 - 0x800;
			NormalizeVector(v, v);
			int32_t r = CrtRand() % 0x1388 + 0x7D0;
			e->pos[0] = (int16_t)(e->pos[0] + (int16_t)(mul32(r, v[0]) >> 12));
			e->pos[1] = (int16_t)(e->pos[1] + (int16_t)(mul32(r, v[1]) >> 12));
			e->pos[2] = (int16_t)(e->pos[2] + (int16_t)(mul32(r, v[2]) >> 12));
			int32_t s = CrtRand() % 0xD2 + 0x78;
			e->vel[0] = (int16_t)(Neg(mul32(s, v[0])) >> 12);
			e->vel[1] = (int16_t)(Neg(mul32(s, v[1])) >> 12);
			e->vel[2] = (int16_t)(Neg(mul32(s, v[2])) >> 12);
		}
	}

	static void StreakMove(Particle *e)
	{
		e->scale = (int16_t)(e->scale - (int16_t)(e->scale >> 3));
		e->pos[0] = (int16_t)(e->pos[0] + e->vel[0]);
		e->pos[1] = (int16_t)(e->pos[1] + e->vel[1]);
		e->pos[2] = (int16_t)(e->pos[2] + e->vel[2]);
		e->vel[0] = (int16_t)(e->vel[0] - (int16_t)(e->vel[0] >> 3));
		e->vel[1] = (int16_t)(e->vel[1] - (int16_t)(e->vel[1] >> 3));
		e->vel[2] = (int16_t)(e->vel[2] - (int16_t)(e->vel[2] >> 3));
	}

	static uint8_t *StreakBegin(StreakWork **work)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		StreakWork *w = (StreakWork *)FieldAlloc(0x88);
		H32(h, 0) = game_addr(MODEL_Streak);
		H32(h, 8) = 0;
		H32(h, 0x1C) = 0x33;
		w->up[0] = 0;
		w->up[1] = -0x1000;
		w->up[2] = 0;
		*work = w;
		return h;
	}

	static uint32_t __cdecl StreaksTask(TaskNode *nd)
	{
		PartNode *n = (PartNode *)nd;
		StreakWork *w;
		uint8_t *h = StreakBegin(&w);
		int alive = 0;
		Particle *e = ArrayB();
		for (int i = 0; i < ARRAY_COUNT; i++, e++)
		{
			if (!(*(uint8_t *)&e->type & 2)) continue;
			StreakDraw(h, w, e);
			if (DrawOnly()) continue;
			e->frame++;
			if (e->frame < 8)
			{
				StreakMove(e);
				alive++;
			}
			else e->type = 0;
		}
		FieldFree(0x88);
		FieldFree(0x58);
		if (DrawOnly()) return 0;
		if (n->counter >= 0 && n->counter <= 0x46) StreakSpawn(n);
		n->counter++;
		if (n->counter >= 8 && alive == 0) return TASK_END;
		return 0;
	}

	static void ShardDraw(uint8_t *h, ShardWork *w, const Particle *e)
	{
		Identity3x3(&w->m);
		w->m.t[1] = w->pos[1];
		w->m.t[0] = w->pos[0];
		w->m.t[2] = w->pos[2];
		w->dir[0] = e->pos[0];
		w->dir[1] = e->pos[1];
		w->dir[2] = e->pos[2];
		int32_t angle = RotationBetweenVectors(w->ref, w->dir, w->axis);
		BuildAxisAngleRotationMatrix(angle, &w->rot, w->axis);
		MatrixMultiply(&w->rot, &w->m);
		BuildAxisAngleRotationMatrix(e->angle, &w->rot, w->view);
		MatrixMultiply(&w->rot, &w->m);
		w->scale[1] = e->scale;
		w->scale[2] = e->scale;
		w->scale[0] = e->scale;
		Scale3DMatrix(&w->m, w->scale);
		ComposeAffineTransform(&Camera(), &w->m, &w->m);
		GteSetRotMatrix(&w->m);
		GteSetTransVector(&w->m);
		int16_t f = e->frame;
		H32(h, 0x1C) = 0x33;
		if (f < 6 || f >= 10)
		{
			int32_t fade = f < 6 ? 0x1000 - f * 0x2AA : (f - 10) * 0x2AA;
			*(int32_t *)(h + 0xC) = fade;
			H32(h, 0x1C) = 0xF3;
		}
		PacketCursor() = RenderPrimModel(h, RenderOT44(), 2, PacketCursor());
	}

	static uint8_t *ShardBegin(const PartNode *n, ShardWork **work)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		ShardWork *w = (ShardWork *)FieldAlloc(0xA0);
		H32(h, 0) = game_addr(MODEL_Shard);
		ff8fx::copy<(4)>(&w->pos[0], &n->pos[0]);
		H32(h, 8) = 0;
		ff8fx::copy<(4)>(&w->pos[2], &n->pos[2]);
		w->ref[0] = 0;
		w->ref[2] = 0;
		w->view[0] = var<int16_t>(game_addr(game::Battle_Camera_world_XZ_s16)) - w->pos[0];
		w->view[1] = var<int16_t>(game_addr(game::unk_B8B7F2)) - w->pos[1];
		w->ref[1] = -0x1000;
		w->view[2] = var<int16_t>(game_addr(game::Battle_Camera_world_Y)) - w->pos[2];
		NormalizeVector(w->view, w->view);
		*work = w;
		return h;
	}

	static uint32_t __cdecl ShardsTask(TaskNode *nd)
	{
		PartNode *n = (PartNode *)nd;
		ShardWork *w;
		uint8_t *h = ShardBegin(n, &w);
		int alive = 0;
		Particle *e = ArrayB();
		for (int i = 0; i < ARRAY_COUNT; i++, e++)
		{
			if (!(*(uint8_t *)&e->type & 4)) continue;
			ShardDraw(h, w, e);
			if (DrawOnly()) continue;
			e->frame++;
			if (e->frame < 0x10)
			{
				e->scale = (int16_t)(e->scale + Div6(e->scale));
				e->angle = (int16_t)(e->angle + e->spin);
				alive++;
			}
			else e->type = 0;
		}
		FieldFree(0xA0);
		FieldFree(0x58);
		if (DrawOnly()) return 0;
		w->ref[0] = 0;
		w->ref[1] = 0;
		w->ref[2] = -0x1000;
		int32_t angle = RotationBetweenVectors(w->ref, w->view, w->axis);
		BuildAxisAngleRotationMatrix(angle, &w->m, w->axis);
		if (n->counter >= 0 && n->counter <= 0x28)
		{
			Particle *e2 = FreeParticle(ArrayB());
			if (e2)
			{
				e2->type = 4;
				e2->frame = 0;
				e2->scale = (int16_t)(CrtRand() % 0x400 + 0x500);
				w->sv[0] = (int16_t)(CrtRand() % 0x1000 - 0x800);
				w->sv[1] = (int16_t)(CrtRand() % 0x1000 - 0x800);
				w->sv[2] = (int16_t)(CrtRand() % 0x200 - 0x100);
				NormalizeSVector(w->sv, w->sv);
				MatrixMultiplyVector(&w->m, w->sv, e2->pos);
				e2->angle = (int16_t)(CrtRand() % 0x1000);
				int32_t spin = CrtRand() % 0x1E + 0xA;
				e2->spin = (int16_t)spin;
				if (*(uint8_t *)&e2->angle & 1) e2->spin = (int16_t)-spin;
			}
		}
		n->counter++;
		if (n->counter >= 0x10 && alive == 0) return TASK_END;
		return 0;
	}

	static void EnergyDraw(const PartNode *n)
	{
		Mat4x3 *shadow = ShadowCamera(n->pos, n->scale, 0);
		int16_t angles[4];
		angles[0] = 0;
		angles[1] = 0;
		angles[2] = n->rot;
		Mat4x3 m;
		ComposeZYXRotationMatrix(angles, &m);
		MatrixMultiply(shadow, &m);
		GteSetRotMatrix(&m);
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		H32(h, 0) = game_addr(MODEL_Energy);
		H32(h, 8) = 0;
		H32(h, 0x1C) = 0x33;
		if (n->counter >= 0x5B)
		{
			H32(h, 0x1C) = 0xF3;
			int32_t fade = shl32(n->counter - 0x5B, 10);
			*(int32_t *)(h + 0xC) = fade;
		}
		PacketCursor() = RenderPrimModel(h, RenderOT44(), 0xD, PacketCursor());
		FieldFree(0x58);
	}

	static void EnergyMove(PartNode *n)
	{
		int16_t ds = n->dscale;
		n->rot = (int16_t)(n->rot + n->rotvel);
		n->scale = (int16_t)(n->scale + ds);
		if (n->scale < 0) n->scale = 0x7FFF;
		if (n->counter < 0x22) n->dscale = (int16_t)(ds + (int16_t)(ds >> 5));
		else n->dscale = (int16_t)(ds + Div12(ds));
	}

	static uint32_t __cdecl EnergyTask(TaskNode *nd)
	{
		PartNode *n = (PartNode *)nd;
		EnergyDraw(n);
		if (DrawOnly()) return 0;
		EnergyMove(n);
		n->counter++;
		return n->counter >= 0x5F ? TASK_END : 0;
	}

	static void ScrollUV(MeshHeader *g, uint8_t *b0, uint8_t *b1, uint8_t *b2, uint8_t *b3, int32_t d, int32_t wrap)
	{
		int32_t a = *b0 + d;
		int32_t c = *b1 + d;
		int32_t b = *b2 + d;
		int32_t e = *b3 + d;
		g->uv[0] = a;
		g->uv[1] = c;
		g->uv[2] = b;
		g->uv[3] = e;
		if (a >= 0x100 || c >= 0x100 || b >= 0x100 || e >= 0x100)
		{
			g->uv[0] = a - wrap;
			g->uv[1] = c - wrap;
			g->uv[2] = b - wrap;
			g->uv[3] = e - wrap;
		}
		else if (a < 0 || c < 0 || b < 0 || e < 0)
		{
			g->uv[0] = a + wrap;
			g->uv[1] = c + wrap;
			g->uv[2] = b + wrap;
			g->uv[3] = e + wrap;
		}
		*b0 = (uint8_t)g->uv[0];
		*b1 = (uint8_t)g->uv[1];
		*b2 = (uint8_t)g->uv[2];
		*b3 = (uint8_t)g->uv[3];
	}

	static uint32_t ClipX(const uint8_t *xy, uint32_t bit) { int16_t x = *(const int16_t *)xy; return (x < 0 || x > 0xA00) ? bit : 0; }
	static uint32_t ClipY(const uint8_t *xy, uint32_t bit) { int16_t y = *(const int16_t *)(xy + 2); return (y < 0 || y > 0x6C0) ? bit : 0; }

	static uint32_t MeshFT4(MeshHeader *g, uint32_t ot, int32_t mode, uint32_t cursor)
	{
		int32_t uwrap = (int16_t)g->u_wrap, vwrap = (int16_t)g->v_wrap;
		uint8_t *r = g->cursor;
		int32_t n = *(int32_t *)r;
		r += 4;
		g->cursor = r;
		uint8_t *verts = g->verts;
		uint8_t *p = (uint8_t *)cursor;
		for (int32_t left = n; left > 0; left--, r += 0x18)
		{
			GteLoadV012(verts + *(uint16_t *)(r + 4) * 4, verts + *(uint16_t *)(r + 6) * 4, verts + *(uint16_t *)(r + 8) * 4);
			GteRTPT();
			*(uint32_t *)(p + 8) = *(uint32_t *)r | 0x2000000;
			*(uint32_t *)(p + 0x20) = *(uint32_t *)(r + 0x14);
			*(uint32_t *)p = 0xC000000;
			*(uint32_t *)(p + 0x10) = *(uint32_t *)(r + 0xC);
			*(uint32_t *)(p + 0x18) = *(uint32_t *)(r + 0x10);
			*(uint32_t *)(p + 0x28) = *(uint32_t *)(r + 0x14) >> 16;
			GteReadFLAG(&g->gte_flag);
			if (g->gte_flag & 0x60000) continue;
			GteNCLIP();
			if (g->du) ScrollUV(g, p + 0x10, p + 0x18, p + 0x20, p + 0x28, g->du, uwrap);
			GteReadMAC0(&g->mac0);
			if (g->mac0 < 0 && !(*(uint8_t *)&g->flags & 0x10)) continue;
			GteReadSXY012Split(p + 0xC, p + 0x14, p + 0x1C);
			GteLoadV0(verts + *(uint16_t *)(r + 0xA) * 4);
			GteRTPS();
			uint32_t clip = ClipX(p + 0xC, 1) | ClipX(p + 0x14, 2) | ClipX(p + 0x1C, 4)
				| ClipY(p + 0xC, 0x10) | ClipY(p + 0x14, 0x20) | ClipY(p + 0x1C, 0x40);
			if (g->dv) ScrollUV(g, p + 0x11, p + 0x19, p + 0x21, p + 0x29, g->dv, vwrap);
			GteReadSXY2(p + 0x24);
			GteAVSZ4();
			clip |= ClipX(p + 0x24, 8) | ClipY(p + 0x24, 0x80);
			if ((clip & 0xF) == 0xF || (clip & 0xF0) == 0xF0) continue;
			GteStoreOTZ(&g->otz);
			if (*(uint8_t *)&g->flags & 0x40)
			{
				GteLoadRGBC(p + 8);
				GteSetIR0(g->fade);
				GteDPCS();
				GteStoreRGB2(p + 8);
			}
			*(uint32_t *)(p + 0x2C) = g->texwin_off;
			*(uint32_t *)(p + 0x30) = g->drawmode;
			*(uint32_t *)(p + 4) = g->texwin;
			int32_t z = (g->otz >> mode) - 1;
			if (z < 0) z = 0;
			InsertPrimAutoDepth(ot + (uint32_t)z * 4, p);
			p += 0x34;
		}
		g->cursor = r;
		return (uint32_t)p;
	}

	static uint32_t MeshGT4(MeshHeader *g, uint32_t ot, int32_t mode, uint32_t cursor)
	{
		int32_t uwrap = (int16_t)g->u_wrap, vwrap = (int16_t)g->v_wrap;
		uint8_t *r = g->cursor;
		int32_t n = *(int32_t *)r;
		r += 4;
		g->cursor = r;
		uint8_t *verts = g->verts;
		uint8_t *p = (uint8_t *)cursor;
		for (int32_t left = n; left > 0; left--, r += 0x24)
		{
			GteLoadV012(verts + *(uint16_t *)(r + 4) * 4, verts + *(uint16_t *)(r + 6) * 4, verts + *(uint16_t *)(r + 8) * 4);
			GteRTPT();
			*(uint32_t *)(p + 8) = *(uint32_t *)r | 0x2000000;
			*(uint32_t *)(p + 0x28) = *(uint32_t *)(r + 0x14);
			*(uint32_t *)p = 0xF000000;
			*(uint32_t *)(p + 0x10) = *(uint32_t *)(r + 0xC);
			*(uint32_t *)(p + 0x1C) = *(uint32_t *)(r + 0x10);
			*(uint32_t *)(p + 0x34) = *(uint32_t *)(r + 0x14) >> 16;
			GteReadFLAG(&g->gte_flag);
			if (g->gte_flag & 0x60000) continue;
			GteNCLIP();
			if (g->du) ScrollUV(g, p + 0x10, p + 0x1C, p + 0x28, p + 0x34, g->du, uwrap);
			GteReadMAC0(&g->mac0);
			if (g->mac0 < 0 && !(*(uint8_t *)&g->flags & 0x20)) continue;
			GteReadSXY012Split(p + 0xC, p + 0x18, p + 0x24);
			GteLoadV0(verts + *(uint16_t *)(r + 0xA) * 4);
			GteRTPS();
			uint32_t clip = ClipX(p + 0xC, 1) | ClipX(p + 0x18, 2) | ClipX(p + 0x24, 4)
				| ClipY(p + 0xC, 0x10) | ClipY(p + 0x18, 0x20) | ClipY(p + 0x24, 0x40);
			if (g->dv) ScrollUV(g, p + 0x11, p + 0x1D, p + 0x29, p + 0x35, g->dv, vwrap);
			GteReadSXY2(p + 0x30);
			GteAVSZ4();
			clip |= ClipX(p + 0x30, 8) | ClipY(p + 0x30, 0x80);
			if ((clip & 0xF) == 0xF || (clip & 0xF0) == 0xF0) continue;
			GteStoreOTZ(&g->otz);
			if (*(uint8_t *)&g->flags & 0x80)
			{
				GteLoadRGB012(r + 0x18, r + 0x1C, r + 0x20);
				GteSetIR0(g->fade);
				GteDPCT();
				GteStoreRGB012(p + 0x14, p + 0x20, p + 0x2C);
				GteLoadRGBC(p + 8);
				GteDPCS();
				GteStoreRGB2(p + 8);
			}
			else
			{
				*(uint32_t *)(p + 0x14) = *(uint32_t *)(r + 0x18);
				*(uint32_t *)(p + 0x20) = *(uint32_t *)(r + 0x1C);
				*(uint32_t *)(p + 0x2C) = *(uint32_t *)(r + 0x20);
			}
			*(uint32_t *)(p + 0x38) = g->texwin_off;
			*(uint32_t *)(p + 0x3C) = g->drawmode;
			*(uint32_t *)(p + 4) = g->texwin;
			int32_t z = (g->otz >> mode) - 1;
			if (z < 0) z = 0;
			InsertPrimAutoDepth(ot + (uint32_t)z * 4, p);
			p += 0x40;
		}
		g->cursor = r;
		return (uint32_t)p;
	}

	static void MeshSkip(MeshHeader *g, uint32_t record)
	{
		uint8_t *s = g->cursor;
		g->cursor = s + *(uint32_t *)s * record + 4;
	}

	static uint32_t MeshRender(MeshHeader *g, uint32_t ot, int32_t mode, uint32_t cursor)
	{
		uint8_t *model = (uint8_t *)g->model;
		g->verts = model + 8;
		g->cursor = model + *(uint32_t *)model;
		uint32_t tw = ((uint32_t)*(uint8_t *)&g->win_h & 0xF8) | 0xFFFE2000;
		tw <<= 5;
		tw |= (uint32_t)*(uint8_t *)&g->win_w & 0xF8;
		tw <<= 5;
		tw |= (0u - (uint32_t)g->v_wrap) & 0xF8;
		tw <<= 2;
		tw |= (uint32_t)((int32_t)(0u - (uint32_t)g->u_wrap) >> 3) & 0x1F;
		g->texwin = tw;
		g->drawmode = 0xE1000220;
		g->texwin_off = 0xE2000000;
		g->du = g->uscroll % (int32_t)(int16_t)g->u_wrap;
		g->dv = g->vscroll % (int32_t)(int16_t)g->v_wrap;
		GteSetFarColour(g->far_rgb[0], g->far_rgb[1], g->far_rgb[2]);
		MeshSkip(g, 12);
		MeshSkip(g, 12);
		g->cursor += 4;
		cursor = MeshFT4(g, ot, mode, cursor);
		g->cursor += 4;
		MeshSkip(g, 24);
		g->cursor += 4;
		return MeshGT4(g, ot, mode, cursor);
	}

	static void RingDraw(const PartNode *n)
	{
		int16_t angles[4];
		angles[0] = 0;
		angles[1] = n->rot;
		angles[2] = 0;
		Mat4x3 m;
		ComposeZYXRotationMatrix(angles, &m);
		m.t[0] = n->pos[0];
		m.t[1] = n->pos[1];
		int32_t s[3];
		s[2] = n->scale;
		s[1] = n->scale;
		s[0] = n->scale;
		RingScale() = n->scale;
		m.t[2] = n->pos[2];
		Scale3DMatrix(&m, s);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		H32(h, 0) = game_addr(MODEL_Ring);
		H32(h, 8) = 0xCEE8CE;
		H32(h, 0x1C) = 0xC0;
		*(int32_t *)(h + 0xC) = RING_Fades()[n->counter & 0xF];
		PacketCursor() = RenderPrimModel(h, RenderOT44(), 2, PacketCursor());
		FieldFree(0x58);
		MeshHeader *g = (MeshHeader *)FieldAlloc(0x80);
		g->model = game_addr(MODEL_Dome);
		*(uint32_t *)g->far_rgb = 0;
		g->flags = 0;
		if (n->counter >= 0x36)
		{
			g->flags = 0xC0;
			int32_t fade = shl32(n->counter - 0x36, 9);
			g->fade = fade;
		}
		g->win_h = 0x80;
		g->win_w = 0x40;
		g->u_wrap = 0x40;
		int32_t v = shl32(n->counter, 3);
		g->vscroll = v;
		g->v_wrap = 0x80;
		g->uscroll = 0;
		PacketCursor() = MeshRender(g, RenderOT44(), 2, PacketCursor());
		FieldFree(0x80);
	}

	static uint32_t __cdecl RingTask(TaskNode *nd)
	{
		PartNode *n = (PartNode *)nd;
		RingDraw(n);
		if (DrawOnly()) return 0;
		int16_t ds = n->dscale;
		n->scale = (int16_t)(n->scale + ds);
		n->counter++;
		n->dscale = (int16_t)(ds - Div20(ds));
		return n->counter >= 0x3E ? TASK_END : 0;
	}

	static void CoreDraw(const PartNode *n)
	{
		int16_t angles[4];
		angles[0] = 0;
		angles[1] = n->rot;
		angles[2] = 0;
		Mat4x3 m;
		ComposeZYXRotationMatrix(angles, &m);
		m.t[0] = n->pos[0];
		int32_t s[3];
		s[2] = RingScale();
		s[1] = RingScale();
		s[0] = RingScale();
		m.t[1] = n->pos[1];
		m.t[2] = n->pos[2];
		Scale3DMatrix(&m, s);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		int16_t c = n->counter;
		H32(h, 0) = game_addr(MODEL_Core);
		H32(h, 8) = 0;
		H32(h, 0x1C) = 0x2033;
		if (c < 4 || c >= 0x20)
		{
			int32_t fade = c < 4 ? shl32(4 - c, 10) : shl32(c - 0x20, 9);
			*(int32_t *)(h + 0xC) = fade;
			H32(h, 0x1C) = 0x20F3;
		}
		int32_t t = Div40(shl32(c, 10));
		int32_t w = ComputeSin(t);
		const int16_t *v0 = (const int16_t *)(game_addr(MODEL_Core) + 8);
		const int16_t *v1 = (const int16_t *)game_addr(CORE_Shape1);
		int16_t *out = (int16_t *)(CoreVertices() + n->index * 0x3C0);
		for (int k = 0; k < CORE_VERTS; k++, v0 += 4, v1 += 4, out += 4)
		{
			out[0] = (int16_t)((mul32((int32_t)v1[0] - v0[0], w) >> 12) + v0[0]);
			out[1] = (int16_t)((mul32((int32_t)v1[1] - v0[1], w) >> 12) + v0[1]);
			out[2] = (int16_t)((mul32((int32_t)v1[2] - v0[2], w) >> 12) + v0[2]);
		}
		H32(h, 4) = (uint32_t)(CoreVertices() + n->index * 0x3C0);
		PacketCursor() = RenderPrimModel(h, RenderOT44(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static uint32_t __cdecl CoreTask(TaskNode *nd)
	{
		PartNode *n = (PartNode *)nd;
		CoreDraw(n);
		if (DrawOnly()) return 0;
		int16_t spin = n->rotvel;
		n->rot = (int16_t)(n->rot + spin);
		n->counter++;
		n->rotvel = (int16_t)(spin - (int16_t)(spin >> 4));
		return n->counter >= 0x28 ? TASK_END : 0;
	}

	static void SpriteDraw(uint8_t *h, Particle *e)
	{
		ShadowCamera(e->pos, e->scale, Neg(e->scale >> 4));
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT44(), 2, PacketCursor());
	}

	static void DustDraw(uint8_t *h, Particle *e)
	{
		*(uint16_t *)(h + 4) = (uint16_t)e->frame;
		SpriteDraw(h, e);
	}

	static void SparkDraw(uint8_t *h, Particle *e)
	{
		*(uint16_t *)(h + 4) = (uint16_t)(e->frame << 1);
		SpriteDraw(h, e);
	}

	static void DustMove(Particle *e)
	{
		e->pos[0] = (int16_t)(e->pos[0] + e->vel[0]);
		e->pos[1] = (int16_t)(e->pos[1] + e->vel[1]);
		e->pos[2] = (int16_t)(e->pos[2] + e->vel[2]);
		e->vel[0] = (int16_t)(e->vel[0] - (int16_t)(e->vel[0] >> 4));
		e->vel[1] = (int16_t)(e->vel[1] - (int16_t)(e->vel[1] >> 3));
		e->vel[2] = (int16_t)(e->vel[2] - (int16_t)(e->vel[2] >> 4));
	}

	static void SparkMove(Particle *e)
	{
		e->pos[0] = (int16_t)(e->pos[0] + e->vel[0]);
		e->pos[1] = (int16_t)(e->pos[1] + e->vel[1]);
		e->pos[2] = (int16_t)(e->pos[2] + e->vel[2]);
		e->vel[0] = (int16_t)(e->vel[0] - (int16_t)(e->vel[0] >> 2));
		e->vel[1] = (int16_t)(e->vel[1] - (int16_t)(e->vel[1] >> 3));
		e->vel[2] = (int16_t)(e->vel[2] - (int16_t)(e->vel[2] >> 2));
	}

	static uint8_t *DustBegin()
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		H32(h, 0) = game_addr(SEQ_Dust);
		*(uint16_t *)(h + 0x24) = 0;
		return h;
	}

	static uint8_t *SparkBegin()
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		H32(h, 0) = game_addr(SEQ_Spark);
		h[0x21] = 0;
		*(uint16_t *)(h + 0x24) = 0x20;
		return h;
	}

	static uint32_t __cdecl DustTask(TaskNode *nd)
	{
		PartNode *n = (PartNode *)nd;
		uint8_t *h = DustBegin();
		int alive = 0;
		Particle *e = ArrayB();
		for (int i = 0; i < ARRAY_COUNT; i++, e++)
		{
			if (!(*(uint8_t *)&e->type & 8)) continue;
			DustDraw(h, e);
			if (DrawOnly()) continue;
			e->frame++;
			if (*(int16_t *)(h + 0x28) < 0) e->type = 0;
			else
			{
				DustMove(e);
				alive++;
			}
		}
		FieldFree(0xB4);
		if (DrawOnly()) return 0;
		if (n->counter >= 0 && n->counter <= 0x32)
		{
			int32_t ring = RingScale();
			ring = (ring + shl32(ring, 10)) >> 12;
			int32_t count = Div14(n->counter) + 5;
			for (int32_t k = 0; k < count; k++)
			{
				Particle *s = FreeParticle(ArrayB());
				if (!s) break;
				s->type = 8;
				s->frame = 0;
				s->scale = (int16_t)(CrtRand() % 0x800 + shl32((uint16_t)(n->counter + 0x40), 5));
				int32_t a = CrtRand() % 0x1000;
				int32_t cs = ComputeCos(a);
				int32_t sn = ComputeSin(a);
				int32_t r = CrtRand() % 0x190 + ring - 0xC8;
				ff8fx::copy<(4)>(&s->pos[0], &n->pos[0]);
				ff8fx::copy<(4)>(&s->pos[2], &n->pos[2]);
				s->pos[0] = (int16_t)(s->pos[0] + (int16_t)(mul32(cs, r) >> 12));
				s->pos[2] = (int16_t)(s->pos[2] + (int16_t)(mul32(sn, r) >> 12));
				int32_t v = CrtRand() % 0x64 + 0x50;
				s->vel[0] = (int16_t)(mul32(cs, v) >> 12);
				s->vel[1] = (int16_t)(-0x32 - CrtRand() % 0x50);
				s->vel[2] = (int16_t)(mul32(sn, v) >> 12);
			}
		}
		n->counter++;
		if (n->counter >= 0x10 && alive == 0) return TASK_END;
		return 0;
	}

	static uint32_t __cdecl SparksTask(TaskNode *nd)
	{
		PartNode *n = (PartNode *)nd;
		uint8_t *h = SparkBegin();
		int alive = 0;
		Particle *e = ArrayA();
		for (int i = 0; i < ARRAY_COUNT; i++, e++)
		{
			if (!(*(uint8_t *)&e->type & 1)) continue;
			SparkDraw(h, e);
			if (DrawOnly()) continue;
			e->frame++;
			if (e->frame >= 0x10) e->type = 0;
			else
			{
				SparkMove(e);
				alive++;
			}
		}
		FieldFree(0xB4);
		if (DrawOnly()) return 0;
		if (n->counter >= 0 && n->counter <= 0x33)
		{
			int32_t ring = RingScale();
			ring = (ring + shl32(ring, 10)) >> 12;
			for (int k = 0; k < 7; k++)
			{
				Particle *s = FreeParticle(ArrayA());
				if (!s) break;
				s->type = 1;
				s->frame = 0;
				s->scale = (int16_t)(CrtRand() % 0x800 + shl32((uint16_t)(n->counter + 0x280), 2));
				int32_t a = CrtRand() % 0x1000;
				int32_t cs = ComputeCos(a);
				int32_t sn = ComputeSin(a);
				int32_t r = CrtRand() % 0x258 + ring - 0x96;
				ff8fx::copy<(4)>(&s->pos[0], &n->pos[0]);
				ff8fx::copy<(4)>(&s->pos[2], &n->pos[2]);
				s->pos[0] = (int16_t)(s->pos[0] + (int16_t)(mul32(cs, r) >> 12));
				s->pos[2] = (int16_t)(s->pos[2] + (int16_t)(mul32(sn, r) >> 12));
				int32_t v = CrtRand() % 0x168 + 0x136;
				s->vel[0] = (int16_t)(mul32(cs, v) >> 12);
				s->vel[1] = (int16_t)(-0xA0 - CrtRand() % 0xAA);
				s->vel[2] = (int16_t)(mul32(sn, v) >> 12);
			}
		}
		n->counter++;
		if (n->counter >= 0x10 && alive == 0) return TASK_END;
		return 0;
	}

	static uint32_t __cdecl RootTask(TaskNode *nd)
	{
		RootNode *r = (RootNode *)nd;
		if (r->counter & 1) PacketCursor() = TexBase() + 0xC000;
		else PacketCursor() = TexBase();
		int running = ExecuteTaskQueue(&FxQueue());
		if (!running)
		{
			RootNode *w = (RootNode *)AddTaskToQueue(&RootQueue(), game_addr(ORIG_WaitTexRestore));
			w->counter = 0;
			w->done = 0;
		}
		r->counter++;
		return running ? 0 : TASK_END;
	}

	static uint32_t __cdecl WaitTexRestoreTask(TaskNode *nd)
	{
		RootNode *w = (RootNode *)nd;
		if (w->counter == 1) TextureRestoreTask((void *)TexBase(), &w->done);
		int16_t done = w->done;
		w->counter++;
		return done ? TASK_END : 0;
	}
}

	void register_mag149_ultima()
	{
		register_port(ultima149::ORIG_Root, (void *)ultima149::RootTask, "U149 RootTask", 149);
		register_port(ultima149::ORIG_WaitTexRestore, (void *)ultima149::WaitTexRestoreTask, "U149 WaitTexRestore", 149);
		register_port(ultima149::ORIG_Director, (void *)ultima149::DirectorTask, "U149 Director", 149);
		register_port(ultima149::ORIG_Orbit, (void *)ultima149::OrbitTask, "U149 Orbit", 149);
		register_port(ultima149::ORIG_Streaks, (void *)ultima149::StreaksTask, "U149 Streaks", 149);
		register_port(ultima149::ORIG_Shards, (void *)ultima149::ShardsTask, "U149 Shards", 149);
		register_port(ultima149::ORIG_Energy, (void *)ultima149::EnergyTask, "U149 Energy", 149);
		register_port(ultima149::ORIG_Ring, (void *)ultima149::RingTask, "U149 Ring", 149);
		register_port(ultima149::ORIG_Core, (void *)ultima149::CoreTask, "U149 Core", 149);
		register_port(ultima149::ORIG_Dust, (void *)ultima149::DustTask, "U149 Dust", 149);
		register_port(ultima149::ORIG_Sparks, (void *)ultima149::SparksTask, "U149 Sparks", 149);
		register_port(ultima149::ORIG_SideFadeIn, (void *)ultima149::SideFadeInTask, "U149 SideFadeIn", 149);
		register_port(ultima149::ORIG_SideFadeOut, (void *)ultima149::SideFadeOutTask, "U149 SideFadeOut", 149);
	}
}

