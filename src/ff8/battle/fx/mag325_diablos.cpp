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

// Effect 325: Diablos - Dark Messenger

#include "fx_port.h"

namespace ff8fx
{
namespace d325
{
	using namespace eng;

	inline uint32_t &Pause() { return var<uint32_t>(game_addr(game::MAG_325_Pause)); }
	inline uint8_t *&CastCtx() { return var<uint8_t *>(game_addr(game::MAG_325_CastCtx)); }
	inline TaskQueuePair &QueueMain() { return var<TaskQueuePair>(game_addr(game::MAG_325_QueueMain)); }
	inline TaskQueue &QueueParticles() { return QueueMain().first; }
	inline TaskQueue &QueueTimeline() { return QueueMain().second; }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_325_PacketCursor)); }
	inline int32_t &CenterZ() { return var<int32_t>(game_addr(game::g_Diablos_TargetsCenterZ)); }
	inline int32_t &CenterX() { return var<int32_t>(game_addr(game::g_Diablos_TargetsCenterX)); }
	inline int32_t &FarZ() { return var<int32_t>(game_addr(game::MAG_325_FarZ)); }
	inline int32_t &MidZ() { return var<int32_t>(game_addr(game::MAG_325_MidZ)); }
	inline uint32_t &Flash() { return var<uint32_t>(game_addr(game::MAG_325_Flash)); }
	inline uint32_t &FlashPrev() { return var<uint32_t>(game_addr(game::MAG_325_FlashPrev)); }
	inline uint8_t &Mode() { return var<uint8_t>(game_addr(game::g_Diablos_ParticleMode)); }
	inline uint8_t *&ModelBuffer() { return var<uint8_t *>(game_addr(game::MAG_325_ModelBuffer)); }
	inline Mat4x3 &EffectCamera() { return var<Mat4x3>(game_addr(game::MAG_325_EffectCamera)); }
	inline uint32_t &FrameCursor() { return var<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)); }
	inline uint32_t OTBase() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)); }
	inline uint32_t ModelDrawMode() { return var<uint32_t>(game_addr(game::BATTLE_PACKET_BUFFER_END)); }
	inline int16_t &CamWorld(int k) { return var<int16_t>(game_addr(game::Battle_Camera_world_XZ_s16) + 2 * k); }
	inline int16_t &CamLookAt(int k) { return var<int16_t>(game_addr(game::Battle_Camera_LookAt_XZ_s16) + 2 * k); }

	static constexpr game::Id ORIG_SequenceTask = game::GF_325Diablos_SequenceTask;
	static constexpr game::Id ORIG_TimelineTask = game::GF_325Diablos_TimelineTask;
	static constexpr game::Id ORIG_BurstTask = game::MAG_325_BonePrimBurstTask;
	static constexpr game::Id ORIG_RisingTask = game::MAG_325_RisingPrimTask;
	static constexpr game::Id CB_PrimObject = game::MAG_325_PrimObjectCallback;

	namespace x
	{
		inline int32_t Rand() { return fn<int32_t (__cdecl *)()>(game_addr(game::_rand))(); }
		inline int32_t Cos(int32_t a) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(a); }
		inline uint32_t RenderPrimSet(void *hdr, uint32_t ot, int mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int, uint32_t)>(game_addr(game::MAG_063_RenderPrimSet))(hdr, ot, mode, cursor); }
		inline void RotX(int32_t angle, void *out) { fn<void (__cdecl *)(int32_t, void *)>(game_addr(game::MAG_063_RotationXMatrix))(angle, out); }
		inline void Decode(uint32_t src, void *layout, int size) { fn<void (__cdecl *)(uint32_t, void *, int)>(game_addr(game::Effect_DecodeModelPrimLayout))(src, layout, size); }
		inline void MorphPair(int x, int y, int z, uint32_t hdr, void *tab_a, int na, int ta, void *tab_b, int nb, int tb)
		{ fn<void (__cdecl *)(int, int, int, uint32_t, void *, int, int, void *, int, int)>(game_addr(game::MAG_325_DrawMorphPair))(x, y, z, hdr, tab_a, na, ta, tab_b, nb, tb); }
		inline void DrawPrimAt(int x, int y, int z, uint32_t model) { fn<void (__cdecl *)(int, int, int, uint32_t)>(game_addr(game::MAG_325_DrawPrimAt))(x, y, z, model); }
		inline void SpawnRising(int x, int y, int z, int shade, uint32_t model) { fn<void (__cdecl *)(int, int, int, int, uint32_t)>(game_addr(game::MAG_325_SpawnRisingPrim))(x, y, z, shade, model); }
		inline void DrawEntityScaled(void *entity, const int32_t *scale) { fn<void (__cdecl *)(void *, const int32_t *)>(game_addr(game::MAG_325_DrawEntityScaled))(entity, scale); }
		inline void DrawWell(int x, int y, int z, int scroll) { fn<void (__cdecl *)(int, int, int, int)>(game_addr(game::MAG_325_DrawGravityWell))(x, y, z, scroll); }
		inline uint32_t FlashTile(int i, int r, int g, int b, int up, int down, uint32_t cursor) { return fn<uint32_t (__cdecl *)(int, int, int, int, int, int, uint32_t)>(game_addr(game::Effect_FlashTileUpDown))(i, r, g, b, up, down, cursor); }
		inline uint32_t Tile(int r, int g, int b, int level, uint32_t cursor) { return fn<uint32_t (__cdecl *)(int, int, int, int, uint32_t)>(game_addr(game::Effect_FullScreenTile))(r, g, b, level, cursor); }
		inline void SetAnim(void *e, int id) { fn<void (__cdecl *)(void *, int)>(game_addr(game::au_re_Battle_ReadAnimation_6))(e, id); }
		inline void AdvanceAnim(void *e) { fn<void (__cdecl *)(void *)>(game_addr(game::MAG_089_Angelo_AnimStep))(e); }
		inline void BindModel(void *e, void *sections, uint32_t data) { fn<void (__cdecl *)(void *, void *, uint32_t)>(game_addr(game::Effect_BindModelContainerSections))(e, sections, data); }
		inline void DrawStylised(void *e, int y, int a, int mode, void *buf) { fn<void (__cdecl *)(void *, int, int, int, void *)>(game_addr(game::MAG_325_DrawModelStylised))(e, y, a, mode, buf); }
		inline uint32_t DrawModel(void *e, void *buf, uint32_t cursor, uint32_t mode) { return fn<uint32_t (__cdecl *)(void *, void *, uint32_t, uint32_t)>(game_addr(game::Effect_DrawCreatureModel))(e, buf, cursor, mode); }
		inline void ResetPuffs() { fn<void (__cdecl *)()>(game_addr(game::MAG_325_ResetPuffs))(); }
		inline void BoltAdvance() { fn<void (__cdecl *)()>(game_addr(game::MAG_325_AdvanceBolts))(); }
		inline void BoltDraw() { fn<void (__cdecl *)()>(game_addr(game::MAG_325_DrawBolts))(); }
		inline void PuffDraw() { fn<void (__cdecl *)()>(game_addr(game::MAG_325_DrawPuffs))(); }
		inline uint8_t *PuffAlloc() { return fn<uint8_t *(__cdecl *)()>(game_addr(game::MAG_325_AllocPuff))(); }
		inline void PuffMove() { fn<void (__cdecl *)()>(game_addr(game::MAG_325_MovePuffs))(); }
		inline void PuffMoveRise() { fn<void (__cdecl *)()>(game_addr(game::MAG_325_MovePuffsRise))(); }
		inline void PuffMoveShrink() { fn<void (__cdecl *)()>(game_addr(game::MAG_325_MovePuffsShrink))(); }
		inline void PuffSpawn(int n, int y, int flag) { fn<void (__cdecl *)(int, int, int)>(game_addr(game::MAG_325_SpawnPuffs))(n, y, flag); }
		inline void PuffsFromModel(void *e) { fn<void (__cdecl *)(void *)>(game_addr(game::MAG_325_PuffsFromModel))(e); }
		inline void BoltParams(int a, int b, int c) { fn<void (__cdecl *)(int, int, int)>(game_addr(game::MAG_325_InitBolts))(a, b, c); }
		inline void BoltSpawn(void *s) { fn<void (__cdecl *)(void *)>(game_addr(game::MAG_325_SpawnBolt))(s); }
		inline void CamLerp(const void *a, const void *b, int t) { fn<void (__cdecl *)(const void *, const void *, int)>(game_addr(game::MAG_325_CameraLerp))(a, b, t); }
		inline void TargetHeights(void *entity, void *out) { fn<void (__cdecl *)(void *, void *)>(game_addr(game::MAG_070_ShockwavePulsar_MeasureModelHeight))(entity, out); }
		inline void VertexPos(void *e, uint32_t vertex, void *out) { fn<void (__cdecl *)(void *, uint32_t, void *)>(game_addr(game::MAG_291_CreatureBonePoint))(e, vertex, out); }
		inline void SpawnPosition(void *e, int bone, int a, void *out) { fn<void (__cdecl *)(void *, int, int, void *)>(game_addr(game::GetEffectSpawnPosition))(e, bone, a, out); }
		inline void Load(int id, uint32_t dst, int n) { fn<void (__cdecl *)(int, uint32_t, int)>(game_addr(game::Stream_QueueLoad))(id, dst, n); }
		inline int LoadBusy() { return fn<int (__cdecl *)()>(game_addr(game::Stream_IsBusy))(); }
		inline int PreLoad0(int a) { return fn<int (__cdecl *)(int)>(game_addr(game::BattleFile_PreLoad_IsLoaded))(a); }
		inline void PreLoad() { fn<void (__cdecl *)()>(game_addr(game::Stream_Pump))(); }
		inline int PreLoad1(int a) { return fn<int (__cdecl *)(int)>(game_addr(game::au_re_BattleFile_preLoad_1))(a); }
		inline void PlayStream(uint32_t a, int b, uint32_t c) { fn<void (__cdecl *)(uint32_t, int, uint32_t)>(game_addr(game::BdPlaySummonStream))(a, b, c); }
		inline void TransStream(uint32_t dst, void *flag) { fn<void (__cdecl *)(uint32_t, void *)>(game_addr(game::BdTransSummonStream))(dst, flag); }
		inline void PlaySE(uint32_t se, int a, int b) { fn<void (__cdecl *)(uint32_t, int, int)>(game_addr(game::BdPlaySE))(se, a, b); }
		inline void TimUpload(uint32_t tim) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::Battle_QueueTIMUpload_GetEOF))(tim); }
		inline void ReleaseVoice(uint32_t slot) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::BdSound_ReleaseVoiceSlot))(slot); }
		inline void ApplyResult(void *targets, int n) { fn<void (__cdecl *)(void *, int)>(game_addr(game::ApplyActionResultToTargets))(targets, n); }
	}

#pragma pack(push, 1)
	struct MasterNode
	{
		TaskNode hdr;
		uint16_t counter;
		uint8_t parity;
		uint8_t spawned;
	};

	struct Target
	{
		uint8_t *entity;
		int16_t h0, h1;
		int16_t z0, one;
	};

	struct TimelineNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t pad0E;
		uint8_t stream_ready;
		uint32_t voice;
		uint32_t flip[16];
		uint32_t morph_b[16];
		Target targets[10];
		uint8_t e1[0x9C];
		uint8_t e2[0x9C];
		uint8_t sections1[0x10];
		uint8_t sections2[0x10];
		uint8_t prim_a[0xE0];
		uint8_t prim_b[0x468];
		uint8_t prim_c[0x1E8];
	};

	struct PrimCtx
	{
		int16_t pos[3];
		int16_t pad06;
		int32_t scale[3];
		int32_t use_scale;
		uint32_t cursor;
		uint32_t scroll_sel;
		uint32_t draw_sel;
		int16_t shade;
		int16_t scroll;
		int16_t fade;
		int16_t pad2A;
		uint8_t *morph;
	};

	struct RisingNode
	{
		TaskNode hdr;
		int16_t pos[3];
		int16_t shade;
		int32_t unused14;
		uint8_t layout[0xD0];
	};

	struct BurstNode
	{
		TaskNode hdr;
		int16_t pos[3];
		int16_t shade;
		int32_t scale;
		uint8_t layout[0xD0];
	};

	struct BoltSpawnRec
	{
		uint8_t pad0[8];
		int16_t pos[3];
		int16_t width;
		int16_t target[3];
		int16_t t;
		int16_t vel[3];
		int16_t dt;
		int16_t shape[3];
		int16_t budget;
		int16_t intensity;
		int16_t decay;
		int16_t unused2C;
		int16_t fade;
	};
#pragma pack(pop)
	static_assert(sizeof(MasterNode) == 0x10, "master node is 0x10 bytes");
	static_assert(sizeof(Target) == 12, "target record is 12 bytes");
	static_assert(sizeof(TimelineNode) == 0x994, "timeline node is 0x994 bytes");
	static_assert(sizeof(PrimCtx) == 0x30, "prim context is 0x30 bytes");
	static_assert(sizeof(RisingNode) == 0xE8, "rising node is 0xE8 bytes");
	static_assert(sizeof(BurstNode) == 0xE8, "burst node is 0xE8 bytes");
	static_assert(sizeof(BoltSpawnRec) == 0x30, "bolt spawn record is 0x30 bytes");

	static const uint32_t MB_MORPH = 0xD94;
	static const uint32_t MB_STREAM_STATE = 0x4C14;
	static const uint32_t MB_PARTICLES = 0x4C5C;
	static const uint32_t MB_PUFFS_END = 0x565C;
	static const uint32_t MB_CHAINS = 0x7C5C;
	static const uint32_t MB_CHAINS_END = 0x882C;
	static const uint32_t MB_ARENA_A = 0x885C, MB_ARENA_B = 0xF85C;
	static const uint32_t MB_STREAM = 0x1685C;

	inline uint8_t *E1(TimelineNode *t) { return t->e1; }
	inline uint8_t *E2(TimelineNode *t) { return t->e2; }
	inline Mat4x3 &Root(uint8_t *e) { return *(Mat4x3 *)(e + 0x40); }
	inline uint8_t *Morph() { return ModelBuffer() + MB_MORPH; }
	inline uint8_t *ActionData() { return *(uint8_t **)(CastCtx() + 4); }
	inline int TargetCount() { return ActionData()[0x10]; }
}
}

namespace ff8fx
{
namespace d325
{
	using namespace eng;

	static uint32_t __cdecl SequenceTask(TaskNode *n)
	{
		MasterNode *node = (MasterNode *)n;
		if (node->parity)
		{
			PacketCursor() = (uint32_t)(ModelBuffer() + MB_ARENA_A);
			node->parity = 0;
		}
		else
		{
			PacketCursor() = (uint32_t)(ModelBuffer() + MB_ARENA_B);
			node->parity = 1;
		}
		Flash() = 0;

		if (node->counter == 2 && !Pause() && !node->spawned)
		{
			node->spawned = 1;
			x::ResetPuffs();
			InitTaskQueuePool(&QueueTimeline(), ModelBuffer(), 0x994, 1);
			TimelineNode *t = (TimelineNode *)AddTaskToQueue(&QueueTimeline(), game_addr(ORIG_TimelineTask));
			Memset32(&t->counter, 0, 0x262);
			t->voice = ClaimVoiceSlot((const void *)game_addr(game::unk_FD6658), 1, 0x80);
			x::Decode(game_addr(game::unk_FD8584), t->prim_a, 0xE0);
			for (int k = 0; k < TargetCount(); k++)
			{
				uint32_t id = (*(uint8_t **)(ActionData() + 8))[0x18 * k];
				t->targets[k].z0 = 0;
				t->targets[k].one = 0x1000;
				t->targets[k].entity = (uint8_t *)(game_addr(game::BattleEntitySlotData) + 0x9C * id);
			}
			StreamStateInit(ModelBuffer() + MB_STREAM_STATE);
			int16_t *tab = (int16_t *)(ModelBuffer() + 0x994);
			for (int32_t a = 0; a < 0x1000; a += 0x10)
			{
				*tab++ = (int16_t)ComputeSin(a);
				*tab++ = (int16_t)x::Cos(a);
			}
		}

		uint32_t left = (uint32_t)n;
		if (node->spawned)
		{
			left = (uint32_t)ExecuteTaskQueue(&QueueTimeline());
			if (QueueParticles().head)
			{
				EffectCameraMatrix(&Camera(), &EffectCamera());
				ExecuteTaskQueue(&QueueParticles());
			}
		}

		uint32_t f = Flash();
		if (f != FlashPrev())
		{
			SetScreenFlash(f, 0);
			f = Flash();
			if ((int32_t)f >= 0x1000)
			{
				for (uint32_t a = game_addr(game::unk_1D98991); a < game_addr(game::unk_1D98A41); a += 0x2C) var<uint8_t>(a) &= 0xFD;
			}
			else if ((int32_t)FlashPrev() >= 0x1000)
			{
				for (uint32_t a = game_addr(game::unk_1D98991); a < game_addr(game::unk_1D98A41); a += 0x2C) var<uint8_t>(a) |= 2;
			}
			FlashPrev() = f;
		}

		if (Pause()) return 0;
		if (node->spawned && left == 0)
		{
			SetScreenFlash(0, 0);
			return TASK_END;
		}
		node->counter++;
		return 0;
	}

	static int16_t FadeA(uint32_t v)
	{
		if (v < 0x10) return (int16_t)(((0x10000u - (v << 12)) >> 3) + 0x100);
		if (v >= 0x201) return (int16_t)((((v + 0xFFDFFu) << 12) >> 3) + 0x100);
		return 0;
	}

	static void CtxA(PrimCtx &c, int16_t fade)
	{
		ff8fx::fill<(sizeof(c))>(&c, 0);
		c.pos[2] = (int16_t)MidZ();
		c.morph = Morph();
		c.scroll_sel = game_addr(game::a0000);
		c.draw_sel = game_addr(game::a0010);
		c.shade = 0x200;
		c.fade = fade;
	}

	static void CtxC(PrimCtx &c)
	{
		ff8fx::fill<(sizeof(c))>(&c, 0);
		c.pos[0] = 0x5A;
		c.pos[1] = (int16_t)0xDC7C;
		c.pos[2] = (int16_t)(FarZ() - 0xA0);
		c.morph = Morph();
		c.scroll_sel = c.draw_sel = game_addr(game::a0);
		c.shade = -0x40;
	}

	static void CtxB298(PrimCtx &c)
	{
		ff8fx::fill<(sizeof(c))>(&c, 0);
		c.pos[0] = 0x4A;
		c.pos[1] = (int16_t)0xD736;
		c.pos[2] = (int16_t)(FarZ() - 0xF2);
		c.morph = Morph();
		c.scroll_sel = game_addr(game::a00000);
		c.draw_sel = game_addr(game::a01000);
	}

	static void CtxB406(PrimCtx &c, int16_t scroll)
	{
		ff8fx::fill<(sizeof(c))>(&c, 0);
		c.pos[0] = (int16_t)CenterX();
		c.pos[2] = (int16_t)CenterZ();
		c.morph = Morph();
		c.scroll = scroll;
		c.scroll_sel = game_addr(game::a0010000000000);
		c.draw_sel = game_addr(game::a1000000100000);
	}

	static int Play(void *layout, PrimCtx &c)
	{
		c.cursor = FrameCursor();
		int r = prim::play((prim::Layout *)layout, (prim::Callback)game_addr(CB_PrimObject), (int)&c, (int)Pause());
		FrameCursor() = c.cursor;
		return r;
	}

	static void DrawGrowing(int32_t diag, int32_t ty)
	{
		uint8_t *w = (uint8_t *)FieldAlloc(0x8C);
		*(int16_t *)(w + 0xE) = 0;
		*(int16_t *)(w + 0xC) = 0;
		*(int16_t *)(w + 0xA) = 0;
		*(int16_t *)(w + 0x10) = (int16_t)diag;
		*(int16_t *)(w + 8) = (int16_t)diag;
		*(int16_t *)(w + 0) = (int16_t)diag;
		*(int16_t *)(w + 6) = 0;
		*(int16_t *)(w + 4) = 0;
		*(int16_t *)(w + 2) = 0;
		*(int32_t *)(w + 0x14) = 0;
		*(int32_t *)(w + 0x18) = ty;
		*(int32_t *)(w + 0x1C) = CenterZ();
		ComposeAffineTransform(&Camera(), (Mat4x3 *)w, (Mat4x3 *)w);
		GteSetRotMatrix((Mat4x3 *)w);
		GteSetTransVector((Mat4x3 *)w);
		*(int32_t *)(w + 0x3C) = 0;
		*(int32_t *)(w + 0x40) = 0;
		*(int32_t *)(w + 0x38) = 0;
		*(uint32_t *)(w + 0x20) = game_addr(game::unk_FD6B6C);
		PacketCursor() = x::RenderPrimSet(w + 0x20, OTBase() + 0x44, 2, PacketCursor());
		FieldFree(0x8C);
	}
	static int32_t GrowDiag(uint32_t e) { return (int32_t)((e << 12) / 60u) + 0x22; }

	static void DrawSinking(int32_t k, int32_t sub)
	{
		uint8_t *w = (uint8_t *)FieldAlloc(0x78);
		x::RotX(0x200, w);
		*(int32_t *)(w + 0x14) = 0;
		*(int32_t *)(w + 0x18) = shl32(k - 0xE, 9) + sub;
		*(int32_t *)(w + 0x1C) = FarZ() - shl32(k + 4, 9) - sub;
		ComposeAffineTransform(&Camera(), (Mat4x3 *)w, (Mat4x3 *)w);
		GteSetRotMatrix((Mat4x3 *)w);
		GteSetTransVector((Mat4x3 *)w);
		*(int32_t *)(w + 0x3C) = 0x30;
		*(uint32_t *)(w + 0x20) = (uint32_t)(ModelBuffer() + 0x35F7C);
		PacketCursor() = RenderPrimModel(w + 0x20, OTBase() + 0x44, 2, PacketCursor());
		FieldFree(0x78);
	}

	static int32_t Ease(int32_t x)
	{
		if (x >= 0x555) return ((x - 0x555) * 426 >> 11) + 0xE56;
		int32_t s = ComputeSin(x >> 2);
		s = ComputeSin(s >> 2);
		return ComputeSin(s >> 2);
	}
	static int32_t WellDepth(uint32_t e) { return Ease((int32_t)((e << 12) / 38u)) * 4 - 0x4100; }

	static void SquashTarget(Target &t, int32_t depth, int32_t scale[3])
	{
		uint8_t *ent = t.entity;
		int32_t d = (*(int32_t *)(ent + 0x54) - CenterX()) >> 2;
		if (d < 0) d = -d;
		if (d > 0x400) d = 0x400;
		int32_t co = x::Cos(d);
		int32_t h0 = t.h0;
		int32_t h = mul32((int32_t)t.h1 - h0, co) >> 12;
		if (h < depth)
		{
			int32_t q = shl32(depth, 12) / h;
			scale[1] = q;
			*(int32_t *)(ent + 0x58) = (int32_t)*(int16_t *)(ent + 0x1E) - mul32(q, h0) / 4096;
		}
		else
		{
			scale[1] = 0x1000;
			*(int32_t *)(ent + 0x58) = (int32_t)*(int16_t *)(ent + 0x1E) - h0;
		}
		scale[2] = 0x1000;
		scale[0] = 0x1000;
	}

	static int32_t FlashTileLevel(int32_t i) { return i < 4 ? shl32(i + 1, 12) / 4 : shl32(4 - i - 1, 12) / 8 + 0x1000; }

	static void SetCam(int16_t lx, int16_t ly, int16_t lz, int16_t wx, int16_t wy, int16_t wz)
	{
		CamLookAt(0) = lx; CamLookAt(1) = ly; CamLookAt(2) = lz;
		CamWorld(0) = wx; CamWorld(1) = wy; CamWorld(2) = wz;
	}

	static void Lerp6(int16_t a0, int16_t a1, int16_t a2, int16_t a3, int16_t a4, int16_t a5,
		int16_t b0, int16_t b1, int16_t b2, int16_t b3, int16_t b4, int16_t b5, int32_t t)
	{
		int16_t a[6] = { a0, a1, a2, a3, a4, a5 }, b[6] = { b0, b1, b2, b3, b4, b5 };
		x::CamLerp(a, b, t);
	}

	static int CameraTick(int32_t c, bool real)
	{
		int r = 0;
		const int32_t D4 = CenterZ(), D8 = CenterX();
		if ((uint32_t)c < 0x1E)
		{
			if (c == 0)
			{
				if (real)
				{
					x::ResetPuffs();
					var<int16_t>(game_addr(game::BATTLE_PROJECTION_DISTANCE)) = 0x120;
					var<int16_t>(game_addr(game::g_BattleCam_Roll)) = 0;
				}
				SetCam(0, (int16_t)0xF3A2, (int16_t)(D4 - 0x4BF), 0, (int16_t)0xF8FC, (int16_t)(D4 + 0x1E57));
				r = 3;
			}
			else if ((uint32_t)c > 8 && real)
			{
				x::PuffSpawn(4, -0xC00, 0);
				x::PuffMoveRise();
			}
		}
		if ((uint32_t)(c - 0x1E) < 0x1E)
		{
			if (c == 0x1E)
			{
				if (real) x::ResetPuffs();
				SetCam(0, (int16_t)0xC000, (int16_t)D4, 0, (int16_t)0xB8C0, (int16_t)(D4 + 0x13E8));
				r = 3;
				if (real)
					for (int k = 0x13; k; k--)
					{
						x::PuffSpawn(4, -0x4000, 1);
						x::PuffMoveShrink();
					}
			}
			if (real)
			{
				x::PuffSpawn(4, -0x4000, 1);
				x::PuffMoveShrink();
			}
		}
		{
			uint32_t e = (uint32_t)(c - 0x3C);
			if (e < 0x1E)
			{
				int32_t t = (int32_t)((e << 12) / 30u) + 0x44;
				if (real)
				{
					if (e == 0)
					{
						uint8_t *p = ModelBuffer() + MB_PARTICLES + 6;
						for (int k = 0xA0; k; k--, p += 0x10)
							if (p[9]) *(int16_t *)p += 0x2000;
					}
					x::PuffMoveShrink();
				}
				Lerp6(0x8FE, (int16_t)0xF9B0, (int16_t)(D4 - 0x837), (int16_t)0xFE53, (int16_t)0xD59D, (int16_t)(D4 + 0x6B6),
					(int16_t)0xFAA0, (int16_t)0xF78B, (int16_t)(D4 - 0xCAE), 0x6EB, (int16_t)0xD563, (int16_t)(D4 + 0x4EF), t);
				r = e == 0 ? 3 : 1;
			}
		}
		int32_t DC = FarZ();
		if (c == 0x5A)
		{
			if (real) x::ResetPuffs();
			DC = FarZ();
			SetCam(0, (int16_t)0xDFED, (int16_t)(DC - 0xD6), 0, (int16_t)0xE105, (int16_t)(DC - 0x62C));
			r = 3;
		}
		{
			uint32_t e = (uint32_t)(c - 0x7B);
			if (e < 0x1E)
			{
				int32_t t = (int32_t)((e << 12) / 30u) + 0x44;
				Lerp6(0, (int16_t)0xDFED, (int16_t)(DC - 0xD6), 0, (int16_t)0xE105, (int16_t)(DC - 0x62C),
					0, (int16_t)0xE3B4, (int16_t)(DC + 0x12C), 0, (int16_t)0xE9D2, (int16_t)(DC - 0xC52), t);
				DC = FarZ();
				r |= 1;
			}
		}
		if (c == 0x99)
		{
			SetCam(0, (int16_t)0xE532, (int16_t)(DC + 0xD9), 0, (int16_t)0xE565, (int16_t)(DC - 0x4AA));
			r = 3;
		}
		{
			uint32_t e = (uint32_t)(c - 0xAF);
			if (e < 0xF)
			{
				int32_t t = ComputeSin((int32_t)((e << 12) / 15u + 0x88) >> 2);
				DC = FarZ();
				Lerp6(0, (int16_t)0xE532, (int16_t)(DC + 0xD9), 0, (int16_t)0xE565, (int16_t)(DC - 0x4AA),
					0, (int16_t)0xE2FD, (int16_t)(DC + 0xD1), 0, (int16_t)0xE41A, (int16_t)(DC - 0xCE8), t);
				DC = FarZ();
				r |= 1;
			}
		}
		if (c == 0xCA)
		{
			if (real) x::ResetPuffs();
			DC = FarZ();
			SetCam((int16_t)0xFF51, (int16_t)0xE051, (int16_t)(DC - 0xF0), 0x177, (int16_t)0xE016, (int16_t)(DC - 0x121));
			r = 3;
		}
		{
			uint32_t e = (uint32_t)(c - 0xD9);
			if (e < 0x14)
			{
				int32_t co = x::Cos((int32_t)((e << 12) / 20u + 0x66) / 2);
				int32_t t = 0x800 - co / 2;
				DC = FarZ();
				Lerp6((int16_t)0xFF50, (int16_t)0xE053, (int16_t)(DC - 0xF1), (int16_t)0xFE29, (int16_t)0xD9DB, (int16_t)(DC + 0xB8),
					(int16_t)0xFF50, (int16_t)0xE053, (int16_t)(DC - 0xF1), 0x1CB, (int16_t)0xDA33, (int16_t)(DC + 0x7C), t);
				DC = FarZ();
				r = e == 0 ? 3 : (r | 1);
			}
		}
		if (c == 0xED)
		{
			SetCam(0x27, (int16_t)0xDED0, (int16_t)(DC - 0xEB), (int16_t)0xFCBA, (int16_t)0xDF3B, (int16_t)(DC - 0x48C));
			r = 3;
		}
		{
			uint32_t e = (uint32_t)(c - 0x104);
			if (e <= 0x10)
			{
				int32_t t = (int32_t)((e << 12) >> 4);
				Lerp6(0x27, (int16_t)0xDED0, (int16_t)(DC - 0xEB), (int16_t)0xFCBA, (int16_t)0xDF3B, (int16_t)(DC - 0x48C),
					0, (int16_t)0xDD59, (int16_t)(DC - 0x115), (int16_t)0xFEE7, (int16_t)0xDF0B, (int16_t)(DC - 0x228), t);
				DC = FarZ();
				r |= 1;
			}
		}
		if (c == 0x129)
		{
			if (real) var<int16_t>(game_addr(game::g_BattleCam_Roll)) = (int16_t)0xFF00;
			SetCam((int16_t)0xFFF9, (int16_t)0xDD51, (int16_t)(DC - 0x109), (int16_t)0xFF0D, (int16_t)0xE5DC, (int16_t)(DC - 0xEDF));
			r = 3;
		}
		{
			uint32_t e = (uint32_t)(c - 0x149);
			if (e < 0x28)
			{
				if (real) var<int16_t>(game_addr(game::g_BattleCam_Roll)) = 0;
				int32_t t = (int32_t)((e << 12) / 40u) + 0x66;
				Lerp6(0x3D, (int16_t)0xDE1E, (int16_t)(DC - 0x97), 0x1ED, (int16_t)0xDE91, (int16_t)(DC - 0x70),
					0xC9, (int16_t)0xE35F, (int16_t)(DC - 0x6C7), 0x651, (int16_t)0xE711, (int16_t)(DC + 0x48C), t);
				DC = FarZ();
				r = e == 0 ? 3 : (r | 1);
			}
		}
		{
			uint32_t e = (uint32_t)(c - 0x16F);
			if (e < 0x26)
			{
				int32_t t = (int32_t)((e << 12) / 38u) + 0x35;
				int32_t s = Ease(t);
				DC = FarZ();
				int16_t x8 = (int16_t)D8;
				Lerp6(x8, (int16_t)(s * 4 - 0x4100), (int16_t)(D4 + 0xEE4), x8, (int16_t)0xFDF2, (int16_t)(D4 + 0x2BBB),
					x8, (int16_t)0xFBE3, (int16_t)(DC - 0x1A9), x8, (int16_t)0xFCCF, (int16_t)(DC + 0xA30), t);
				DC = FarZ();
				r = e == 0 ? 3 : (r | 1);
			}
		}
		if (c == 0x195)
		{
			SetCam((int16_t)(D8 - 0x3EC), (int16_t)0xF22F, (int16_t)(D4 + 0x27B), (int16_t)(D8 - 0x1A91), (int16_t)0xC008, (int16_t)(D4 + 0x2287));
			r = 3;
		}
		if (c == 0x1C7)
		{
			SetCam(0, (int16_t)0xE16E, (int16_t)DC, 0xEB6, (int16_t)0xE1D0, (int16_t)DC);
			r = 3;
		}
		{
			uint32_t e = (uint32_t)(c - 0x1D4);
			if (e < 0x1E)
			{
				int32_t co = x::Cos((int32_t)((e << 12) / 30u + 0x44) / 2);
				int32_t t = 0x800 - co / 2;
				DC = FarZ();
				Lerp6(0, (int16_t)0xE16E, (int16_t)DC, 0xEB6, (int16_t)0xE1D0, (int16_t)DC,
					0, (int16_t)0xE16D, (int16_t)(DC - 1), 0x24C4, (int16_t)0xE7DF, (int16_t)(DC - 0x18), t);
				r |= 1;
			}
		}
		if (c == 0x1F2)
		{
			SetCam((int16_t)D8, (int16_t)0xFCF4, (int16_t)(D4 - 0x470), (int16_t)D8, (int16_t)0xFA2F, (int16_t)(D4 + 0x2178));
			r = 3;
		}
		return r;
	}

	static uint32_t __cdecl TimelineTask(TaskNode *n)
	{
		TimelineNode *t = (TimelineNode *)n;
		uint8_t *MB = ModelBuffer();
		uint8_t *e1 = E1(t), *e2 = E2(t);

		int32_t c = t->counter;

		{
			uint32_t v = (uint32_t)(c - 1);
			if (v < 0x211)
			{
				if (v < 8) Flash() = ((v << 12) >> 3) + 0x100;
				else if (v >= 0x209) Flash() = ((0x210000u - (v << 12)) >> 3) + 0x100;
				else
				{
					Flash() = 0x1000;
					PrimCtx ctx;
					CtxA(ctx, FadeA(v));
					Play(t->prim_a, ctx);
				}
			}
		}

		{
			uint32_t e = (uint32_t)(c - 0x1F);
			if (e < 0x3C)
			{
				DrawGrowing(GrowDiag(e), e < 0x1E ? -0x4000 : -0x2000);
			}
		}

		{
			uint32_t e = (uint32_t)(c - 0x5B);
			if (e < 0xC)
			{
				int32_t tt = (int32_t)((e << 12) / 12u);
				if (e == 0)
				{
					t->flip[0] = (uint32_t)(MB + 0x3505C);
					t->flip[1] = (uint32_t)(MB + 0x3704C);
					t->flip[2] = (uint32_t)(MB + 0x3505C);
					t->morph_b[0] = (uint32_t)(MB + 0x3505C);
					t->morph_b[1] = (uint32_t)(MB + 0x3704C);
					t->morph_b[2] = (uint32_t)(MB + 0x3505C);
				}
				x::MorphPair(0, -0x2000, (int16_t)FarZ(), (uint32_t)(ModelBuffer() + 0x4C244), t->flip, 2, tt, t->morph_b, 2, tt);
			}
		}

		{
			uint32_t e = (uint32_t)(c - 0x67);
			if (e < 0x64)
			{
				uint32_t model;
				if (e == 0)
				{
					if (!Pause())
					{
						InitTaskQueuePool(&QueueParticles(), ModelBuffer() + 0x3505C, 0xE8, 0x10);
						x::SpawnRising(-500, -0x212C, FarZ(), -0x80, (uint32_t)(ModelBuffer() + 0x4AFAC));
						x::SpawnRising(-200, -0x212C, FarZ(), -0x80, (uint32_t)(ModelBuffer() + 0x4BBC4));
						x::SpawnRising(0, -0x212C, FarZ(), -0x80, (uint32_t)(ModelBuffer() + 0x4B558));
						x::SpawnRising(200, -0x212C, FarZ(), -0x80, (uint32_t)(ModelBuffer() + 0x4BBC4));
						x::SpawnRising(500, -0x212C, FarZ(), -0x80, (uint32_t)(ModelBuffer() + 0x4AFAC));
						for (int k = 0; k < 9; k++) t->flip[k] = (uint32_t)(ModelBuffer() + 0x3903C + 0x1FF0 * k);
					}
					model = t->flip[e % 9];
				}
				else if (e < 0x46) model = t->flip[e % 9];
				else model = t->flip[0];
				x::DrawPrimAt(0, -0x2000, (int16_t)FarZ(), model);
			}
		}

		{
			uint32_t e = (uint32_t)(c - 0xAD);
			if (e < 0x1E)
			{
				if (e == 0) x::SetAnim(e1, 1);
				else if (e == 0x1C) x::SetAnim(e1, 2);
			}
		}
		if (t->counter == 0xCB && !Pause()) Root(e1).t[1] = -0x1880;

		{
			uint32_t e = (uint32_t)((int32_t)t->counter - 0xEE);
			if (e < 0x3C)
			{
				if (e == 0)
				{
					if (!Pause())
					{
						x::SetAnim(e1, 3);
						for (int k = 0; k < 13; k++) t->flip[k] = (uint32_t)(ModelBuffer() + 0x3505C + 0x1FF0 * k);
					}
				}
				else if (e == 8 && !Pause()) x::SetAnim(e1, 4);
				uint32_t model = t->flip[e % 13];
				x::DrawPrimAt(0x5A, -0x2258, FarZ() - 0xA0, model);
			}
		}

		{
			uint32_t e = (uint32_t)((int32_t)t->counter - 0x10C);
			if (e < 0x1E && !Pause()) Root(e1).t[1] += -10;
			if (e < 0x3C)
			{
				if (e == 0) x::Decode((uint32_t)(ModelBuffer() + 0x51D40), t->prim_c, 0x1E8);
				PrimCtx ctx;
				CtxC(ctx);
				int r = Play(t->prim_c, ctx);
				if (r == 0)
				{
					x::Decode((uint32_t)(ModelBuffer() + 0x51D40), t->prim_c, 0x1E8);
				}
			}
		}

		{
			uint32_t e = (uint32_t)((int32_t)t->counter - 0x12A);
			if (e < 0x1E)
			{
				if (e == 0) x::Decode((uint32_t)(ModelBuffer() + 0x4EF8C), t->prim_b, 0x3C8);
				PrimCtx ctx;
				CtxB298(ctx);
				Play(t->prim_b, ctx);
			}
		}

		{
			uint32_t e = (uint32_t)((int32_t)t->counter - 0x148);
			if (e < 0x28)
			{
				if (e == 0)
				{
					x::SetAnim(e1, 5);
					x::SetAnim(e2, 1);
				}
				else if (e >= 0x20)
				{
					DrawSinking((int32_t)e - 0x20, 0);
				}
			}
		}

		{
			uint32_t e = (uint32_t)((int32_t)t->counter - 0x170);
			if (e < 0x26)
			{
				int32_t depth = WellDepth(e);
				if (e == 0)
					for (int k = 0; k < TargetCount(); k++)
					{
						uint8_t *ent = t->targets[k].entity;
						x::TargetHeights(ent, &t->targets[k].h0);
						ent[0] |= 8;
					}
				for (int k = 0; k < TargetCount(); k++)
				{
					int32_t scale[3];
					SquashTarget(t->targets[k], depth, scale);
					x::DrawEntityScaled(t->targets[k].entity, scale);
					t->targets[k].entity[0] |= 4;
				}
				x::DrawWell(CenterX(), depth - 0x1000, CenterZ(), (int32_t)(e << 4));
			}
		}

		{
			uint32_t e = (uint32_t)((int32_t)t->counter - 0x192);
			if (e < 0xC)
			{
				PacketCursor() = x::FlashTile((int32_t)e, 0xFF, 0xFF, 0xFF, 4, 8, PacketCursor());
			}
		}

		{
			uint32_t e = (uint32_t)((int32_t)t->counter - 0x196);
			if (e < 0x32)
			{
				if (e == 0) x::Decode((uint32_t)(ModelBuffer() + 0x3B4E4), t->prim_b, 0x468);
				PrimCtx ctx;
				CtxB406(ctx, (int16_t)(e * 4));
				Play(t->prim_b, ctx);
			}
		}
		if (t->counter == 0x1C8) x::SetAnim(e1, 6);

		{
			uint32_t e = (uint32_t)((int32_t)t->counter - 0x5C);
			if (e < 0x212)
			{
				if (e == 0)
				{
					Mat4x3 &r = Root(e1);
					r.m[2][1] = 0;
					r.m[2][2] = 0x1000;
					r.m[1][1] = 0x1000;
					r.m[0][0] = 0x1000;
					r.m[2][0] = 0;
					r.m[1][2] = 0;
					r.m[1][0] = 0;
					r.m[0][2] = 0;
					r.m[0][1] = 0;
					r.t[0] = 0;
					r.t[1] = -0x2000;
					r.t[2] = FarZ();
					x::BindModel(e1, t->sections1, (uint32_t)(ModelBuffer() + 0x2185C));
					x::SetAnim(e1, 0);
					*(uint32_t *)(e1 + 0x28) = 0x808080;
				}
				if (!Pause()) x::AdvanceAnim(e1);
				int kind = 0;
				if (e < 0x4B) kind = 1;
				else if (e >= 0xB0 && e < 0xEC) kind = 2;
				else if (e <= 0x114 || (e >= 0x16C && e < 0x17A)) kind = 3;
				if (kind == 1) x::DrawStylised(e1, -0x2000, 0x80, 2, ModelBuffer() + MB_MORPH);
				else if (kind == 2) x::DrawStylised(e1, -0x22BC, 0x80, 0, ModelBuffer() + MB_MORPH);
				else if (kind == 3) FrameCursor() = x::DrawModel(e1, ModelBuffer() + MB_MORPH, FrameCursor(), ModelDrawMode());
			}
		}

		{
			uint32_t e = (uint32_t)((int32_t)t->counter - 0x12A);
			if (e < 0x46)
			{
				if (e == 0)
				{
					ff8fx::copy<(0x20)>(e2 + 0x40, e1 + 0x40);
					x::BindModel(e2, t->sections2, (uint32_t)(ModelBuffer() + 0x31B5C));
					x::SetAnim(e2, 0);
				}
				if (!Pause()) x::AdvanceAnim(e2);
				FrameCursor() = x::DrawModel(e2, ModelBuffer() + MB_MORPH, FrameCursor(), ModelDrawMode());
			}
		}

		{
			uint8_t m;
			bool bolts = false;
			if (!Pause())
			{
				m = Mode();
				if (m == 2)
				{
					x::BoltAdvance();
					m = Mode();
					bolts = m == 2;
				}
			}
			else
			{
				m = Mode();
				bolts = m == 2;
			}
			if (bolts)
			{
				x::BoltDraw();
			}
			else if (m == 3)
			{
				x::PuffDraw();
			}
		}

		{
			switch ((uint16_t)t->counter)
			{
			case 0:
				x::Load(0x2D7, (uint32_t)(ModelBuffer() + MB_STREAM), 3);
				x::Load(0x2D8, (uint32_t)(ModelBuffer() + MB_STREAM), 2);
				x::Load(0x2D9, (uint32_t)(ModelBuffer() + 0x2185C), 5);
				x::Load(0x2DA, (uint32_t)(ModelBuffer() + 0x3505C), 5);
				x::Load(0x2DB, (uint32_t)(ModelBuffer() + MB_STREAM), 5);
				break;
			case 0x20:
				if (!x::PreLoad0(0)) return 0;
				x::PlayStream(0x80, 1, 0x7F);
				break;
			case 0x57:
			case 0xA9:
			case 0x1D1:
				if (x::LoadBusy()) return 0;
				t->stream_ready = 0;
				x::TransStream((uint32_t)(ModelBuffer() + MB_STREAM), &t->stream_ready);
				break;
			case 0xD6:
				if (!x::PreLoad0(0)) return 0;
				t->stream_ready = 0;
				x::TransStream((uint32_t)(ModelBuffer() + MB_STREAM), &t->stream_ready);
				break;
			case 0x5B:
				if (!t->stream_ready) return 0;
				x::PlayStream(0x80, 1, 0x7F);
				x::Load(0x2DC, (uint32_t)(ModelBuffer() + MB_STREAM), 5);
				break;
			case 0xAD:
				if (!t->stream_ready) return 0;
				x::PlayStream(0x80, 1, 0x7F);
				x::Load(0x2DD, (uint32_t)(ModelBuffer() + MB_STREAM), 5);
				break;
			case 0xCB:
				QueueParticles().tail = nullptr;
				QueueParticles().head = nullptr;
				x::Load(0x2DE, (uint32_t)(ModelBuffer() + 0x3505C), 5);
				break;
			case 0xDA:
			case 0x1D5:
				if (!t->stream_ready) return 0;
				x::PlayStream(0x80, 1, 0x7F);
				break;
			case 0xED:
				if (x::LoadBusy()) return 0;
				break;
			case 0x147:
				x::Load(0x2DF, (uint32_t)(ModelBuffer() + 0x3505C), 5);
				break;
			case 0x167:
				if (x::LoadBusy()) return 0;
				x::TimUpload((uint32_t)(ModelBuffer() + 0x42E54));
				x::Load(0x2E0, (uint32_t)(ModelBuffer() + MB_STREAM), 3);
				break;
			case 0x1B4:
				if (x::LoadBusy()) return 0;
				x::PlayStream(0x80, 1, 0x7F);
				x::Load(0x2D7, (uint32_t)(ModelBuffer() + MB_STREAM), 5);
				break;
			case 0x1F4:
				x::Load(0x168, (uint32_t)(ModelBuffer() + MB_STREAM), 1);
				x::Load(0x169, (uint32_t)(ModelBuffer() + MB_STREAM), 1);
				x::Load(0x16A, (uint32_t)(ModelBuffer() + MB_STREAM), 0);
				break;
			default:
				break;
			}
		}
		x::PreLoad();

		switch ((uint16_t)t->counter)
		{
		case 0: x::PlaySE(game_addr(game::unk_FD6640), 1, 0x80); break;
		case 0x5B: x::PlaySE(game_addr(game::unk_FD6644), 1, 0x80); break;
		case 0xEE: x::PlaySE(game_addr(game::unk_FD6648), 1, 0x80); break;
		case 0x14D: x::PlaySE(game_addr(game::unk_FD664C), 1, 0x80); break;
		case 0x192: x::PlaySE(game_addr(game::unk_FD6650), 1, 0x80); break;
		case 0x1C8: x::PlaySE(game_addr(game::unk_FD6654), 1, 0x80); break;
		default: break;
		}

		{
			uint32_t e = (uint32_t)((int32_t)t->counter - 0x5C);
			if (e < 0x4B && !Pause())
			{
				if ((e & 7) == 7)
				{
					int16_t v[4];
					int32_t r = x::Rand();
					x::VertexPos(e1, game_addr(game::unk_FDBDA8) + 8 * ((uint32_t)(r * 10) >> 15), v);
					r = x::Rand();
					int32_t k = (r * 3) >> 15;
					uint32_t model;
					if (k == 0) model = (uint32_t)(ModelBuffer() + 0x4AFAC);
					else if (k == 1) model = (uint32_t)(ModelBuffer() + 0x4B558);
					else if (k == 2) model = (uint32_t)(ModelBuffer() + 0x4BBC4);
					else model = *(uint32_t *)v;
					if (e != 7) x::SpawnRising(v[0], v[1], v[2], -0x80, model);
				}
				Root(e1).t[1] += 0x20;
			}
		}

		{
			uint32_t e = (uint32_t)((int32_t)t->counter - 0xAD);
			if (e < 0x1E)
			{
				if (e >= 4 && e < 6)
				{
					for (int32_t cnt = 0x1F;; cnt--)
					{
						uint8_t *p = x::PuffAlloc();
						if (!p) break;
						p[0xF] = 0x10;
						*(uint32_t *)p = (cnt & 1) ? game_addr(game::unk_FD69C0) : game_addr(game::unk_FD6814);
						int32_t ang = x::Rand() & 0x7FF;
						int32_t rad = (x::Rand() & 0x1FF) + 0x200;
						int32_t cx = mul32(x::Cos(ang), rad) >> 12;
						*(int16_t *)(p + 4) = (int16_t)cx;
						int32_t sn = mul32(ComputeSin(ang), rad);
						int32_t q = rad / 4;
						p[0xC] = (uint8_t)(*(int16_t *)(p + 4) / 64);
						sn >>= 12;
						int32_t sy = (int16_t)sn;
						*(int16_t *)(p + 6) = (int16_t)(sn - 0x2000);
						p[0xD] = (uint8_t)(sy / 64);
						q = -q;
						*(int16_t *)(p + 8) = (int16_t)q;
						p[0xE] = (uint8_t)((int16_t)q / 16);
						*(int16_t *)(p + 8) = (int16_t)(q + FarZ());
						p[0xB] = (uint8_t)((x::Rand() & 0x3F) + 0x10);
						p[0xA] = (uint8_t)(ang / 16);
						if (cnt == 0) break;
					}
				}
				x::PuffMove();
			}
		}

		{
			uint32_t e = (uint32_t)((int32_t)t->counter - 0xEE);
			if (e < 0x3C)
			{
				if (e == 0) x::BoltParams(5, 0, -0x20);
				else if (e >= 8)
				{
					int16_t P[4];
					x::SpawnPosition(e1, 0x21, 0, P);
					BoltSpawnRec S;
					int32_t rad = (x::Rand() & 0xFFF) + 0x400;
					int32_t ang = x::Rand();
					int32_t cx = mul32(x::Cos(ang), rad) >> 12;
					int32_t sn = mul32(ComputeSin(ang), rad) >> 12;
					int32_t h7 = cx >> 7;
					S.pos[0] = (int16_t)(P[0] + h7);
					int32_t r = x::Rand();
					S.pos[1] = (int16_t)((r & 0xFF) + P[1] - 0x80);
					int32_t s7 = sn >> 7;
					S.pos[2] = (int16_t)(P[2] + s7);
					S.vel[1] = -100;
					S.target[0] = (int16_t)(P[0] + (cx >> 3));
					S.target[1] = (int16_t)(P[1] - rad / 8);
					S.shape[2] = 0;
					S.shape[1] = 0;
					S.target[2] = (int16_t)((sn >> 3) + P[2]);
					S.shape[0] = 0;
					S.vel[0] = (int16_t)-h7;
					S.vel[2] = (int16_t)-s7;
					S.width = 0x5DC;
					S.t = 0;
					S.dt = 0x100;
					S.budget = 0;
					S.intensity = 0x3000;
					S.decay = -0x200;
					S.shape[0] = (int16_t)(((x::Rand() * 100) >> 15) - 0x32);
					S.shape[1] = (int16_t)(((x::Rand() * 100) >> 15) - 0x32);
					S.fade = 0x1000;
					S.shape[2] = (int16_t)(((x::Rand() * 100) >> 15) - 0x32);
					x::BoltSpawn(&S);
				}
			}
		}

		{
			uint32_t e = (uint32_t)((int32_t)t->counter - 0x12A);
			if (e < 0x14)
			{
				if (e == 0)
				{
					var<int16_t>(game_addr(game::unk_2505200)) = 9;
					var<int16_t>(game_addr(game::unk_2505202)) = 0x20;
					var<int16_t>(game_addr(game::unk_2505204)) = 0x800;
					var<int16_t>(game_addr(game::unk_2505206)) = -4;
				}
				int16_t P[4];
				x::SpawnPosition(e1, 0x21, 0, P);
				int32_t rad = (x::Rand() & 0x3FF) + 0xBB8;
				int32_t half = rad / 2;
				for (int k = 2; k; k--)
				{
					BoltSpawnRec S;
					int32_t ang = x::Rand();
					int32_t cx = mul32(x::Cos(ang), rad) >> 12;
					int32_t sn = mul32(ComputeSin(ang), rad) >> 12;
					S.vel[1] = 200;
					S.pos[0] = (int16_t)((cx >> 8) + P[0]);
					S.pos[2] = (int16_t)((sn >> 8) + P[2]);
					S.target[0] = (int16_t)((cx >> 4) + P[0] + cx);
					S.pos[1] = P[1];
					S.target[1] = (int16_t)(P[1] - half);
					S.shape[2] = 0;
					S.shape[1] = 0;
					S.shape[0] = 0;
					S.target[2] = (int16_t)((sn >> 4) + P[2] + sn);
					S.vel[0] = (int16_t)(cx >> 6);
					S.vel[2] = (int16_t)(sn >> 6);
					S.width = 0xBB8;
					S.t = 0;
					S.dt = 0x200;
					S.budget = 2;
					S.intensity = 0x3000;
					S.decay = -0x200;
					S.shape[0] = (int16_t)(((x::Rand() * 300) >> 15) - 0x96);
					S.shape[1] = (int16_t)(((x::Rand() * 300) >> 15) - 0x96);
					S.fade = 0x1000;
					S.shape[2] = (int16_t)(((x::Rand() * 300) >> 15) - 0x96);
					x::BoltSpawn(&S);
				}
			}
		}

		{
			uint32_t e = (uint32_t)((int32_t)t->counter - 0x178);
			if (e < 0x1E)
			{
				int32_t a4 = (int32_t)(((e + 8) << 12) / 38u);
				if (e == 0) x::BoltParams(7, 0x1000, -0x100);
				for (int k = 2; k; k--)
				{
					BoltSpawnRec S;
					int32_t ang = x::Rand();
					int32_t rad = (x::Rand() & 0x3FF) + 0x400;
					int32_t cx = mul32(x::Cos(ang), rad) / 4096;
					int32_t sn = mul32(ComputeSin(ang), rad) / 4096;
					S.pos[1] = 0;
					S.pos[0] = (int16_t)(CenterX() + cx);
					S.pos[2] = (int16_t)(sn + CenterZ());
					S.target[0] = (int16_t)((cx >> 1) + cx + CenterX());
					int32_t s = Ease(a4);
					int32_t w = ComputeSin(rad / 4);
					S.target[1] = (int16_t)(s * 4 - 0x4200 - w);
					S.target[2] = (int16_t)((sn >> 1) + sn + CenterZ());
					int32_t kk = mul32(x::Rand(), a4) >> 15;
					S.vel[0] = (int16_t)(mul32(cx, kk) >> 12);
					S.vel[1] = 0;
					S.vel[2] = (int16_t)(mul32(sn, kk) >> 12);
					S.shape[2] = 0;
					S.shape[1] = 0;
					S.shape[0] = 0;
					S.width = 0x1770;
					S.t = 0;
					S.dt = 0x200;
					S.budget = 2;
					S.intensity = 0x3000;
					S.decay = -0x200;
					S.shape[0] = (int16_t)((x::Rand() & 0xFF) - 0x80);
					S.shape[1] = (int16_t)((x::Rand() & 0xFF) - 0x80);
					S.fade = 0x1000;
					S.shape[2] = (int16_t)((x::Rand() & 0xFF) - 0x80);
					x::BoltSpawn(&S);
				}
			}
		}

		{
			uint32_t e = (uint32_t)((int32_t)t->counter - 0x196);
			if (e < 0x10)
			{
				int32_t q = (int32_t)((e << 12) / 30u);
				if (e == 0) x::BoltParams(7, 0x1000, 0);
				int32_t rad = q / 4 + 0xBB8;
				int16_t ty = (int16_t)shl32(-rad, 2);
				for (int k = 2; k; k--)
				{
					BoltSpawnRec S;
					int32_t ang = x::Rand();
					int32_t cx = mul32(x::Cos(ang), rad) >> 12;
					int32_t sn = mul32(ComputeSin(ang), rad) >> 12;
					S.pos[1] = 0;
					S.pos[0] = (int16_t)(cx + CenterX());
					S.pos[2] = (int16_t)(CenterZ() + sn);
					S.target[0] = (int16_t)((cx >> 1) + cx + CenterX());
					S.target[1] = ty;
					S.target[2] = (int16_t)((sn >> 1) + sn + CenterZ());
					S.vel[0] = (int16_t)(mul32(x::Rand(), cx) >> 22);
					S.vel[1] = 0;
					S.vel[2] = (int16_t)(mul32(x::Rand(), sn) >> 22);
					S.shape[2] = 0;
					S.shape[1] = 0;
					S.shape[0] = 0;
					S.width = 0x1770;
					S.t = 0;
					S.dt = 0x200;
					S.budget = 2;
					S.intensity = 0x3000;
					S.decay = -0x200;
					S.shape[0] = (int16_t)((x::Rand() & 0xFF) - 0x80);
					S.shape[1] = (int16_t)((x::Rand() & 0xFF) - 0x80);
					S.fade = 0x1000;
					S.shape[2] = (int16_t)((x::Rand() & 0xFF) - 0x80);
					x::BoltSpawn(&S);
				}
			}
		}

		{
			uint32_t e = (uint32_t)((int32_t)t->counter - 0x1D2);
			if (e < 8)
			{
				if (e == 0) InitTaskQueuePool(&QueueParticles(), ModelBuffer() + 0x3505C, 0xE8, 0x10);
				for (int k = 2; k; k--)
				{
					BurstNode *b = (BurstNode *)AddTaskToQueue(&QueueParticles(), game_addr(ORIG_BurstTask));
					if (!b) continue;
					int32_t r = x::Rand();
					uint32_t nb = ***(uint8_t ***)(e1 + 0x64);
					x::SpawnPosition(e1, (r * (int32_t)nb) >> 15, 0, b->pos);
					b->shade = -0x100;
					b->scale = (x::Rand() & 0x7FF) + 0x800;
					x::Decode((uint32_t)(ModelBuffer() + 0x42620), b->layout, 0xB8);
				}
				uint8_t g = (uint8_t)(0x70 - (uint8_t)((uint8_t)e << 4));
				e1[0x2A] = g;
				e1[0x29] = g;
				e1[0x28] = g;
			}
		}

		{
			uint32_t e = (uint32_t)((int32_t)t->counter - 0x1D5);
			if (e < 0x1E)
			{
				if (e == 0)
				{
					x::ResetPuffs();
					x::PuffsFromModel(e1);
				}
				x::PuffMove();
			}
		}

		if (t->counter == 0x1F3)
		{
			for (int k = 0; k < TargetCount(); k++) *(uint16_t *)t->targets[k].entity &= 0xFFF3;
			Mode() = 0;
			uint8_t *a = ActionData();
			x::ApplyResult(*(void **)(a + 8), a[0x10]);
		}

		CameraTick(t->counter, true);

		if (t->counter == 0x20E) x::ReleaseVoice(t->voice);
		t->counter++;
		if (t->counter < 0x212) return 0;
		if (t->counter == 0x212)
			for (int k = 0; k < TargetCount(); k++) *(uint16_t *)t->targets[k].entity &= 0xFFF3;
		if (x::PreLoad1(2)) return TASK_END;
		t->counter = 0x222;
		return 0;
	}

	static void CtxRising(PrimCtx &c, const int16_t pos[3], int16_t shade)
	{
		ff8fx::fill<(sizeof(c))>(&c, 0);
		c.pos[0] = pos[0];
		c.pos[1] = pos[1];
		c.pos[2] = pos[2];
		c.pad06 = shade;
		c.morph = Morph();
		c.shade = shade;
		c.scroll_sel = game_addr(game::a0);
		c.draw_sel = game_addr(game::a1);
	}

	static uint32_t __cdecl RisingTask(TaskNode *n)
	{
		RisingNode *p = (RisingNode *)n;
		PrimCtx c;
		CtxRising(c, p->pos, p->shade);
		if (!Play(p->layout, c)) return TASK_END;
		if (!Pause()) p->pos[1] += 0x18;
		return 0;
	}

	static void CtxBurst(PrimCtx &c, const BurstNode *p)
	{
		ff8fx::fill<(sizeof(c))>(&c, 0);
		c.pos[0] = p->pos[0];
		c.pos[1] = p->pos[1];
		c.pos[2] = p->pos[2];
		c.pad06 = p->shade;
		c.scale[0] = c.scale[1] = c.scale[2] = p->scale;
		c.use_scale = 1;
		c.morph = Morph();
		c.shade = p->shade;
		c.scroll_sel = c.draw_sel = game_addr(game::a0);
	}

	static uint32_t __cdecl BurstTask(TaskNode *n)
	{
		BurstNode *p = (BurstNode *)n;
		PrimCtx c;
		CtxBurst(c, p);
		return Play(p->layout, c) ? 0 : TASK_END;
	}
}

	void register_mag325_diablos()
	{
		register_port(d325::ORIG_SequenceTask, (void *)d325::SequenceTask, "D325 SequenceTask", 325);
		register_port(d325::ORIG_TimelineTask, (void *)d325::TimelineTask, "D325 TimelineTask", 325);
		register_port(d325::ORIG_RisingTask, (void *)d325::RisingTask, "D325 RisingTask", 325);
		register_port(d325::ORIG_BurstTask, (void *)d325::BurstTask, "D325 BurstTask", 325);
	}
}

