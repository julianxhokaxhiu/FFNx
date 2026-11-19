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

// Effect 291: Pandemona - Tornado Zone

#include "fx_port.h"

namespace ff8fx
{
namespace p291
{
	using namespace eng;

	inline uint32_t &Pause() { return var<uint32_t>(game_addr(game::g_P291_DebugPause)); }
	inline uint8_t *CastCtx() { return var<uint8_t *>(game_addr(game::g_P291_CastCtx)); }
	inline TaskQueue &QLand() { return var<TaskQueue>(game_addr(game::g_P291_QLandPrim)); }
	inline TaskQueue &QFall() { return var<TaskQueue>(game_addr(game::g_P291_QFallPrim)); }
	inline TaskQueue &QTrail() { return var<TaskQueue>(game_addr(game::g_P291_QTrail)); }
	inline TaskQueue &QFlip() { return var<TaskQueue>(game_addr(game::g_P291_QFlipbook)); }
	inline TaskQueue &QCreature() { return var<TaskQueue>(game_addr(game::g_P291_QCreature)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::g_P291_PacketCursor)); }
	inline int32_t &Ground() { return var<int32_t>(game_addr(game::g_P291_GroundZ)); }
	inline int32_t &Center() { return var<int32_t>(game_addr(game::g_P291_CenterZ)); }
	inline int32_t &Flash() { return var<int32_t>(game_addr(game::g_P291_ScreenFlash)); }
	inline uint8_t *MB() { return var<uint8_t *>(game_addr(game::g_P291_ModelBuffer)); }
	inline int32_t &LoadState() { return var<int32_t>(game_addr(game::g_P291_TexLoadState)); }
	inline uint32_t &FrameCursor() { return var<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)); }
	inline int16_t &CamW(int i) { return var<int16_t>(game_addr(game::Battle_Camera_world_XZ_s16) + 2 * i); }
	inline int16_t &CamL(int i) { return var<int16_t>(game_addr(game::Battle_Camera_LookAt_XZ_s16) + 2 * i); }
	inline int16_t &ShakeY() { return var<int16_t>(game_addr(game::BATTLE_CAMERA_SHAKE_OFFSET_Y)); }
	inline int16_t &Roll() { return var<int16_t>(game_addr(game::g_BattleCam_Roll)); }
	inline int16_t &W1D8E038() { return var<int16_t>(game_addr(game::BATTLE_PROJECTION_DISTANCE)); }
	inline int16_t SinT(int i) { return *(const int16_t *)(game_addr(game::unk_13B7BB8) + 4 * i); }
	inline int16_t CosT(int i) { return *(const int16_t *)(game_addr(game::unk_13B7BBA) + 4 * i); }

	inline int Count() { return *(uint8_t *)(*(uint32_t *)(CastCtx() + 4) + 0x10); }
	inline uint8_t *TargetIds() { return *(uint8_t **)(*(uint32_t *)(CastCtx() + 4) + 8); }

	template<typename T> inline T &At(void *p, int o) { return *(T *)((uint8_t *)p + o); }
	template<typename T> inline const T &At(const void *p, int o) { return *(const T *)((const uint8_t *)p + o); }

	static constexpr game::Id ORIG_SequenceTask = game::GF_291Pandemona_SequenceTask;
	static constexpr game::Id ORIG_TimelineTask = game::GF_291Pandemona_TimelineTask;
	static constexpr game::Id ORIG_LandPrimTask = game::MAG_291_LandPrimTask;
	static constexpr game::Id ORIG_FallPrimTask = game::MAG_291_FallPrimTask;
	static constexpr game::Id ORIG_SwirlFlipTask = game::MAG_291_SwirlFlipbookTask;
	static constexpr game::Id ORIG_RiseFlipTask = game::MAG_291_RiseFlipbookTask;
	static constexpr game::Id ORIG_TrailTask = game::MAG_291_TrailTask;
	static constexpr game::Id CB_Prim = game::MAG_291_PrimPlayer_DrawObject;

	namespace w
	{
		inline int32_t Rand() { return fn<int32_t (__cdecl *)()>(game_addr(game::_rand))(); }
		inline int32_t Cos(int32_t a) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(a); }
		inline void Decode(uint32_t src, void *layout, int n) { fn<void (__cdecl *)(uint32_t, void *, int)>(game_addr(game::Effect_DecodeModelPrimLayout))(src, layout, n); }
		inline void RotY(int32_t a, void *m) { fn<void (__cdecl *)(int32_t, void *)>(game_addr(game::Effect_RotationYMatrix))(a, m); }
		inline void RotX(int32_t a, void *m) { fn<void (__cdecl *)(int32_t, void *)>(game_addr(game::MAG_063_RotationXMatrix))(a, m); }
		inline void Scale3D(void *m, const void *v) { fn<void (__cdecl *)(void *, const void *)>(game_addr(game::scale3DMatrix))(m, v); }
		inline void Basis(const void *v, void *m) { fn<void (__cdecl *)(const void *, void *)>(game_addr(game::BuildOrthonormalBasis))(v, m); }
		inline void Billboard(void *m) { fn<void (__cdecl *)(void *)>(game_addr(game::MAG_297_WHITE_WIND_QUISTIS_CameraBillboardMatrix))(m); }
		inline void VecLerp(const void *a, const void *b, int32_t wa, int32_t wb, void *out) { fn<void (__cdecl *)(const void *, const void *, int32_t, int32_t, void *)>(game_addr(game::VecLerp))(a, b, wa, wb, out); }
		inline uint32_t PrimSet(void *hdr, uint32_t ot, int mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int, uint32_t)>(game_addr(game::MAG_063_RenderPrimSet))(hdr, ot, mode, cursor); }
		inline void CamRotate(int32_t angle, int32_t scale) { fn<void (__cdecl *)(int32_t, int32_t)>(game_addr(game::MAG_278_CameraSwingAboutLookAt))(angle, scale); }
		inline void DefaultPos(void *ent, void *out) { fn<void (__cdecl *)(void *, void *)>(game_addr(game::GetDefaultEffectPosition))(ent, out); }
		inline void Load(int id, uint32_t dst, int kind) { fn<void (__cdecl *)(int, uint32_t, int)>(game_addr(game::Stream_QueueLoad))(id, dst, kind); }
		inline int LoadBusy() { return fn<int (__cdecl *)()>(game_addr(game::Stream_IsBusy))(); }
		inline void LoadPump() { fn<void (__cdecl *)()>(game_addr(game::Stream_Pump))(); }
		inline int PreLoad1(int a) { return fn<int (__cdecl *)(int)>(game_addr(game::au_re_BattleFile_preLoad_1))(a); }
		inline void LoadFile(int id, uint32_t dst, int a, int b) { fn<void (__cdecl *)(int, uint32_t, int, int)>(game_addr(game::pre_LoadBattleFile))(id, dst, a, b); }
		inline void TimUpload(uint32_t p) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::Battle_QueueTIMUpload_GetEOF))(p); }
		inline void PlaySE(uint32_t snd) { fn<void (__cdecl *)(uint32_t, int, int)>(game_addr(game::BdPlaySE))(snd, 1, 0x80); }
		inline void PlayStream() { fn<void (__cdecl *)(int, int, int)>(game_addr(game::BdPlaySummonStream))(0x80, 1, 0x7F); }
		inline void ReleaseVoice(uint32_t slot) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::BdSound_ReleaseVoiceSlot))(slot); }
		inline void StreamStateInit(void *s) { fn<void (__cdecl *)(void *)>(game_addr(game::BattleFile_StreamState_Init))(s); }
		inline void ReadAnim6(void *e, int id) { fn<void (__cdecl *)(void *, int)>(game_addr(game::au_re_Battle_ReadAnimation_6))(e, id); }
		inline void Advance(void *e) { fn<void (__cdecl *)(void *)>(game_addr(game::MAG_089_Angelo_AnimStep))(e); }
		inline void Pivot(void *ent, void *out) { fn<void (__cdecl *)(void *, void *)>(game_addr(game::MAG_291_Entity_BoneYRange))(ent, out); }
		inline void Bind(void *e, void *sec, uint32_t data) { fn<void (__cdecl *)(void *, void *, uint32_t)>(game_addr(game::MAG_291_BindCreatureModel))(e, sec, data); }
		inline uint32_t DrawCreature(void *e, int a2, uint32_t a3, uint32_t cursor, uint32_t a5) { return fn<uint32_t (__cdecl *)(void *, int, uint32_t, uint32_t, uint32_t)>(game_addr(game::MAG_291_DrawCreatureModel))(e, a2, a3, cursor, a5); }
		inline void BonePos(void *e, uint32_t data, void *out) { fn<void (__cdecl *)(void *, uint32_t, void *)>(game_addr(game::MAG_291_CreatureBonePoint))(e, data, out); }
		inline void HitTarget(void *ent) { fn<void (__cdecl *)(void *)>(game_addr(game::MAG_291_ApplyResultToEntity))(ent); }
		inline void Flatten(uint32_t mesh) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::MAG_291_Mesh_SetWFromY))(mesh); }
		inline void Flatten2(uint32_t mesh) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::MAG_291_Mesh_SetWFromZ))(mesh); }
		inline void PartReset(void *nd) { fn<void (__cdecl *)(void *)>(game_addr(game::MAG_291_Particles_Reset))(nd); }
		inline int16_t *PartAlloc(void *nd) { return fn<int16_t *(__cdecl *)(void *)>(game_addr(game::MAG_291_Particles_Alloc))(nd); }
		inline void PartFall(void *nd) { fn<void (__cdecl *)(void *)>(game_addr(game::MAG_291_Particles_UpdateFall))(nd); }
		inline void PartOrbit(void *nd, int32_t a, int32_t b) { fn<void (__cdecl *)(void *, int32_t, int32_t)>(game_addr(game::MAG_291_Particles_UpdateOrbit))(nd, a, b); }
		inline void PartBounce(void *nd) { fn<void (__cdecl *)(void *)>(game_addr(game::MAG_291_Particles_UpdateBounce))(nd); }
		inline void PartDrift(void *nd) { fn<void (__cdecl *)(void *)>(game_addr(game::MAG_291_Particles_UpdateDrift))(nd); }
		inline void PartDraw(void *nd) { fn<void (__cdecl *)(void *)>(game_addr(game::MAG_291_Particles_Draw))(nd); }
		inline void Scale3(int32_t a, int32_t s, void *out) { fn<void (__cdecl *)(int32_t, int32_t, void *)>(game_addr(game::MAG_291_ScaledRotX))(a, s, out); }
		inline void MatY(int32_t a, int32_t s, int32_t sy, void *m) { fn<void (__cdecl *)(int32_t, int32_t, int32_t, void *)>(game_addr(game::MAG_291_ScaledRotY))(a, s, sy, m); }
		inline void Morph(uint32_t a, uint32_t b, int32_t t) { fn<void (__cdecl *)(uint32_t, uint32_t, int32_t)>(game_addr(game::MAG_291_MorphMeshToScratch))(a, b, t); }
		typedef void (__cdecl *Fn12)(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t);
		inline void Tornado(const int32_t *a) { fn<Fn12>(game_addr(game::MAG_291_DrawTornadoLayer))(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10], a[11]); }
		typedef void (__cdecl *Fn10)(void *, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t);
		inline void Ribbon(void *m, const int32_t *a) { fn<Fn10>(game_addr(game::MAG_291_DrawRibbonLayer))(m, a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9]); }
		inline void EntDraw(void *ent, void *ang, void *pivot, void *scale) { fn<void (__cdecl *)(void *, void *, void *, void *)>(game_addr(game::MAG_274_DUEL_DrawEntity))(ent, ang, pivot, scale); }
		inline int TrailPush(int max, int count, void *entries, void *a, void *b, int c0, int c1, int c2) { return fn<int (__cdecl *)(int, int, void *, void *, void *, int, int, int)>(game_addr(game::MAG_291_Trail_PushHead))(max, count, entries, a, b, c0, c1, c2); }
		inline uint32_t TrailDraw(int32_t *count, void *entries, int d, uint32_t ot, int mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(int32_t *, void *, int, uint32_t, int, uint32_t)>(game_addr(game::MAG_291_Trail_FadeDraw))(count, entries, d, ot, mode, cursor); }
	}
}
}

namespace ff8fx
{
namespace p291
{
	using namespace eng;

	static uint32_t __cdecl SequenceTask(TaskNode *n)
	{
		uint8_t *nd = (uint8_t *)n;
		if (At<uint32_t>(nd, 0x10))
		{
			PacketCursor() = (uint32_t)(MB() + 0x71D4);
			At<uint32_t>(nd, 0x10) = 0;
		}
		else
		{
			PacketCursor() = (uint32_t)(MB() + 0x151D4);
			At<uint32_t>(nd, 0x10) = 1;
		}
		Flash() = 0;
		if (At<uint16_t>(nd, 0xC) == 2 && !Pause() && !nd[0xF])
		{
			nd[0xF] = 1;
			InitTaskQueuePool(&QCreature(), MB() + 0x21A4, 0x1030, 1);
			InitTaskQueuePool(&QFlip(), MB() + 0x1AA4, 0x1C, 0x40);
			InitTaskQueuePool(&QTrail(), MB() + 0xD90, 0x45C, 3);
			InitTaskQueuePool(&QFall(), MB() + 0x640, 0x9C, 0xC);
			InitTaskQueuePool(&QLand(), MB(), 0x190, 4);
			uint8_t *cr = (uint8_t *)AddTaskToQueue(&QCreature(), game_addr(ORIG_TimelineTask));
			Memset32(cr + 0xC, 0, 0x409);
			At<uint32_t>(cr, 0x10) = ClaimVoiceSlot((const void *)game_addr(game::unk_13B7A5C), 1, 0x80);
			At<uint32_t>(cr, 0x102C) = (uint32_t)(cr + 0x42C);
			for (int i = 0; i < Count(); i++)
			{
				uint8_t *rec = cr + 0x14 + 0x20 * i;
				uint8_t *ent = (uint8_t *)(game_addr(game::BattleEntitySlotData) + 0x9C * (int32_t)TargetIds()[0x18 * i]);
				At<uint8_t *>(rec, 0) = ent;
				At<uint32_t>(rec, 0xC) = At<uint32_t>(ent, 0x1C);
				At<uint32_t>(rec, 0x10) = At<uint32_t>(ent, 0x20);
				At<uint32_t>(rec, 0x14) = At<uint32_t>(ent, 0xC);
				At<uint32_t>(rec, 0x18) = At<uint32_t>(ent, 0x10);
				At<int16_t>(rec, 0x16) += 0x800;
				rec[0x1C] = 0;
				rec[0x1D] = 1;
				At<int16_t>(rec, 0xA) = 0x1000;
				int16_t size = At<int16_t>(ent, 0x26);
				At<int16_t>(rec, 8) = size == 0 ? (int16_t)0x10 : (int16_t)(0x10000 / ((int32_t)size + 0x400));
				w::Pivot(ent, rec + 4);
			}
			if (Count() - 1 > 0)
			{
				for (int p = 1; p - 1 < Count() - 1; p++)
				{
					for (int j = p; j < Count(); j++)
					{
						uint8_t *a = cr + 0x14 + 0x20 * (p - 1), *b = cr + 0x14 + 0x20 * j;
						if (At<int16_t>(At<uint8_t *>(a, 0), 0x1C) < At<int16_t>(At<uint8_t *>(b, 0), 0x1C))
						{
							uint8_t t[0x20];
							ff8fx::copy<(0x20)>(t, a);
							ff8fx::copy<(0x20)>(a, b);
							ff8fx::copy<(0x20)>(b, t);
						}
					}
				}
			}
			for (int i = 0; i < Count(); i++)
			{
				uint8_t *rec = cr + 0x14 + 0x20 * i;
				uint8_t *ent = At<uint8_t *>(rec, 0);
				rec[0x1E] = 0;
				uint8_t *e = At<uint8_t *>(ent, 0x8C);
				bool found = false;
				if (e && e != ent)
				{
					while (e)
					{
						if (e[0] & 2)
						{
							int k = 0, nt = Count();
							for (; k < nt; k++)
								if (At<uint8_t *>(cr + 0x14 + 0x20 * k, 0) == e) break;
							if (k < nt) { found = true; break; }
						}
						e = At<uint8_t *>(e, 0x8C);
						if (e == ent) break;
					}
				}
				if (!found) rec[0x1E] = 1;
			}
			w::Flatten(game_addr(game::unk_13A0E58));
			w::Flatten(game_addr(game::unk_13A4F88));
			w::Flatten(game_addr(game::unk_13A6508));
			w::Flatten2(game_addr(game::unk_13A8508));
			w::Flatten2(game_addr(game::unk_13AB5C0));
			w::Flatten2(game_addr(game::unk_13AC1B8));
			w::Flatten2(game_addr(game::unk_13ACDB0));
			w::StreamStateInit(MB() + 0x231D4);
		}
		uint32_t left = (uint32_t)nd;
		if (nd[0xF])
		{
			left = (uint32_t)ExecuteTaskQueue(&QCreature());
			ExecuteTaskQueue(&QFlip());
			ExecuteTaskQueue(&QTrail());
			ExecuteTaskQueue(&QFall());
			ExecuteTaskQueue(&QLand());
		}
		SetScreenFlash((uint32_t)Flash(), 0);
		if (Pause()) return 0;
		if (nd[0xF] && left == 0) return TASK_END;
		At<uint16_t>(nd, 0xC)++;
		return 0;
	}

	static uint8_t g_fr[0xB0];
	inline int16_t &F16(uint8_t *fr, int o) { return *(int16_t *)(fr + o); }
	inline int32_t &F32(uint8_t *fr, int o) { return *(int32_t *)(fr + o); }

	struct Pass
	{
		uint8_t *nd, *fr;
		int32_t c;
	};

	inline uint8_t *E(uint8_t *nd) { return nd + 0x154; }
	inline uint8_t *Rec(uint8_t *nd, int i) { return nd + 0x14 + 0x20 * i; }
	inline uint8_t *Ent(uint8_t *nd, int i) { return At<uint8_t *>(Rec(nd, i), 0); }

	static void Play(const Pass &p, uint8_t *ctx)
	{
		prim::Layout *l = (prim::Layout *)(p.nd + 0x1F0);
		prim::play(l, (prim::Callback)game_addr(CB_Prim), (int)ctx, Pause());
	}

	static void DrawA(const Pass &p, int32_t v)
	{
		uint8_t *fr = p.fr;
		int32_t scroll = (int32_t)shl32(v, 3);
		F32(fr, 0x58) = 0;
		F32(fr, 0x5C) = 0;
		F32(fr, 0x60) = Center();
		w::RotY(0x800, fr + 0x44);
		ComposeAffineTransform(&Camera(), (Mat4x3 *)(fr + 0x44), (Mat4x3 *)(fr + 0x44));
		F16(fr, 0x66) = (int16_t)scroll;
		F32(fr, 0x6C) = (int32_t)(MB() + 0x31D4);
		F32(fr, 0x68) = game_addr(game::unk_13EF440);
		F16(fr, 0x64) = 0;
		Play(p, fr + 0x44);
	}

	struct ParB { int32_t t[3][12]; };
	static void GenB(int32_t v, ParB &r)
	{
		int32_t e = (int32_t)((uint32_t)shl32(v, 12) / 14u);
		int32_t y = Center() + 0x7D0;
		int32_t a[3][12] = {
			{ y, (int32_t)game_addr(game::unk_13A0E58), (int32_t)game_addr(game::unk_13A4280), shl32(v, 6), 0, 0, 0, e, e + 0x1000, 0, shl32(v, 5), 0x1000 - (e >> 1) },
			{ y, (int32_t)game_addr(game::unk_13A4F88), (int32_t)game_addr(game::unk_13A5EF0), shl32(v, 6), 0, 0, 0, e * 2, e + 0x1000, 1, shl32(v, 5), 0x1000 - (e >> 2) },
			{ y, (int32_t)game_addr(game::unk_13A6508), (int32_t)game_addr(game::unk_13A7BF0), shl32(v, 6), 0, 0, 0, e * 2, e + 0x1000, 1, v * 40, 0x1000 - (e >> 2) } };
		ff8fx::copy<(sizeof(a))>(r.t, a);
	}
	static void DrawB(const Pass &p, int32_t v)
	{
		ParB r;
		GenB(v, r);
		for (int k = 0; k < 3; k++) w::Tornado(r.t[k]);
	}

	struct ParC { int32_t t[3][12]; int32_t e, ang2, fade, third; };
	static void GenC(int32_t v, ParC &r)
	{
		int32_t e = (int32_t)((uint32_t)shl32(v, 12) / 60u);
		int32_t y = Center();
		int32_t a[3][12] = {
			{ y, (int32_t)game_addr(game::unk_13A0E58), (int32_t)game_addr(game::unk_13A4280), shl32(v, 6), e, shl32(v, 7), e >> 2, shl32(v + 0x20, 7), 0x2000, 0, shl32(v, 5), 0 },
			{ y, (int32_t)game_addr(game::unk_13A4F88), (int32_t)game_addr(game::unk_13A5EF0), shl32(v, 6), e, shl32(v + 2, 7), e >> 1, shl32(v + 0x10, 8), 0x2000, 1, shl32(v, 5), 0 },
			{ y, (int32_t)game_addr(game::unk_13A6508), (int32_t)game_addr(game::unk_13A7BF0), shl32(v, 6), e, shl32(v + 4, 7), e >> 1, shl32(v + 0x10, 8), 0x2000, 1, v * 40, 0 } };
		ff8fx::copy<(sizeof(a))>(r.t, a);
		r.e = e;
		r.ang2 = v * 100;
		r.fade = shl32(0x3C - v, 8);
		r.third = (uint32_t)v >= 0x2C;
	}
	static void DrawC(const Pass &p, int32_t v)
	{
		ParC r;
		GenC(v, r);
		for (int k = 0; k < 3; k++) w::Tornado(r.t[k]);
		uint8_t *ws = (uint8_t *)FieldAlloc(0x8C);
		w::Morph(game_addr(game::unk_139FFE8), game_addr(game::unk_13A0790), r.e);
		w::MatY(r.t[0][3], 0x2000, 0x1000, ws);
		At<int32_t>(ws, 0x14) = 0;
		At<int32_t>(ws, 0x18) = -0x2000;
		At<int32_t>(ws, 0x1C) = Center();
		ComposeAffineTransform(&Camera(), (Mat4x3 *)ws, (Mat4x3 *)ws);
		GteSetRotMatrix((Mat4x3 *)ws);
		GteSetTransVector((Mat4x3 *)ws);
		At<int32_t>(ws, 0x3C) = 0x2030;
		At<int16_t>(ws, 0x42) = 0;
		At<int16_t>(ws, 0x40) = 0;
		At<int32_t>(ws, 0x38) = 0x100;
		At<uint32_t>(ws, 0x24) = (uint32_t)(MB() + 0x31D4);
		At<uint32_t>(ws, 0x20) = game_addr(game::unk_139FFE8);
		PacketCursor() = w::PrimSet(ws + 0x20, RenderOT(), 2, PacketCursor());
		w::MatY(r.ang2, 0x1388, 0x1000, ws);
		At<int32_t>(ws, 0x14) = 0;
		At<int32_t>(ws, 0x18) = -0x2000;
		At<int32_t>(ws, 0x1C) = Center();
		ComposeAffineTransform(&Camera(), (Mat4x3 *)ws, (Mat4x3 *)ws);
		GteSetRotMatrix((Mat4x3 *)ws);
		GteSetTransVector((Mat4x3 *)ws);
		At<int32_t>(ws, 0x38) = 0x80;
		At<uint32_t>(ws, 0x20) = game_addr(game::unk_139FFE8);
		At<uint32_t>(ws, 0x24) = (uint32_t)(MB() + 0x31D4);
		PacketCursor() = w::PrimSet(ws + 0x20, RenderOT(), 2, PacketCursor());
		if (r.third)
		{
			At<int32_t>(ws, 0x14) = 0;
			At<int32_t>(ws, 0x18) = -0x2000;
			At<int32_t>(ws, 0x1C) = Center();
			w::Billboard(ws);
			GteSetRotMatrix((Mat4x3 *)ws);
			GteSetTransVector((Mat4x3 *)ws);
			At<int32_t>(ws, 0x3C) = 0xF0;
			At<int32_t>(ws, 0x28) = 0;
			At<int32_t>(ws, 0x2C) = r.fade;
			At<int32_t>(ws, 0x38) = 0;
			At<uint32_t>(ws, 0x20) = game_addr(game::unk_13A0978);
			PacketCursor() = w::PrimSet(ws + 0x20, RenderOT(), 2, PacketCursor());
		}
		FieldFree(0x8C);
	}

	struct ParD { int32_t on, more; int32_t t[3][12]; int32_t v36, ang2; };
	static void GenD(int32_t v, ParD &r)
	{
		ff8fx::fill<(sizeof(r))>(&r, 0);
		int32_t v35 = (uint32_t)v < 0x18 ? 0 : (int32_t)((uint32_t)shl32(v - 0x18, 12) / 20u);
		if (v35 > 0x1000) return;
		r.on = 1;
		int32_t y = Center();
		int32_t a0[12] = { y, (int32_t)game_addr(game::unk_13A0E58), (int32_t)game_addr(game::unk_13A4280), shl32(v, 6), 0x1000, 0, 0, 0x2E00, 0x2000, 0, shl32(v, 5), v35 };
		ff8fx::copy<(sizeof(a0))>(r.t[0], a0);
		int32_t v36 = (uint32_t)v < 0x18 ? 0 : (int32_t)((uint32_t)shl32(v - 0x18, 12) / 30u);
		if (v36 > 0x1000) return;
		r.more = 1;
		int32_t a1[12] = { y, (int32_t)game_addr(game::unk_13A4F88), (int32_t)game_addr(game::unk_13A5EF0), shl32(v, 6), 0x1000, 0, 0, (int32_t)0xFFFF8800, 0x2000, 1, shl32(v, 5), v36 };
		int32_t a2[12] = { y, (int32_t)game_addr(game::unk_13A6508), (int32_t)game_addr(game::unk_13A7BF0), shl32(v, 6), 0x1000, 0, 0, (int32_t)0xFFFF8800, 0x2000, 1, v * 40, v36 };
		ff8fx::copy<(sizeof(a1))>(r.t[1], a1);
		ff8fx::copy<(sizeof(a2))>(r.t[2], a2);
		r.v36 = v36;
		r.ang2 = v * 100;
	}
	static void DrawD(const Pass &p, int32_t v)
	{
		ParD r;
		GenD(v, r);
		if (!r.on) return;
		w::Tornado(r.t[0]);
		if (!r.more) return;
		w::Tornado(r.t[1]);
		w::Tornado(r.t[2]);
		uint8_t *ws = (uint8_t *)FieldAlloc(0x8C);
		if (r.v36)
		{
			At<int32_t>(ws, 0x3C) = 0x20F0;
			At<int32_t>(ws, 0x2C) = r.v36;
			At<int32_t>(ws, 0x28) = 0;
		}
		else At<int32_t>(ws, 0x3C) = 0x2030;
		At<int16_t>(ws, 0x42) = 0;
		At<int16_t>(ws, 0x40) = 0;
		w::MatY(r.t[0][3], 0x2000, 0x1000, ws);
		At<int32_t>(ws, 0x14) = 0;
		At<int32_t>(ws, 0x18) = -0x2000;
		At<int32_t>(ws, 0x1C) = Center();
		ComposeAffineTransform(&Camera(), (Mat4x3 *)ws, (Mat4x3 *)ws);
		GteSetRotMatrix((Mat4x3 *)ws);
		GteSetTransVector((Mat4x3 *)ws);
		At<int32_t>(ws, 0x38) = 0x100;
		At<uint32_t>(ws, 0x24) = game_addr(game::unk_13A0798);
		At<uint32_t>(ws, 0x20) = game_addr(game::unk_139FFE8);
		PacketCursor() = w::PrimSet(ws + 0x20, RenderOT(), 2, PacketCursor());
		w::MatY(r.ang2, 0x1388, 0x1000, ws);
		At<int32_t>(ws, 0x14) = 0;
		At<int32_t>(ws, 0x18) = -0x2000;
		At<int32_t>(ws, 0x1C) = Center();
		ComposeAffineTransform(&Camera(), (Mat4x3 *)ws, (Mat4x3 *)ws);
		GteSetRotMatrix((Mat4x3 *)ws);
		GteSetTransVector((Mat4x3 *)ws);
		At<int32_t>(ws, 0x38) = 0x80;
		At<uint32_t>(ws, 0x24) = game_addr(game::unk_13A0798);
		At<uint32_t>(ws, 0x20) = game_addr(game::unk_139FFE8);
		PacketCursor() = w::PrimSet(ws + 0x20, RenderOT(), 2, PacketCursor());
		At<int32_t>(ws, 0x14) = 0;
		At<int32_t>(ws, 0x18) = -0x2000;
		At<int32_t>(ws, 0x1C) = Center();
		w::Billboard(ws);
		GteSetRotMatrix((Mat4x3 *)ws);
		GteSetTransVector((Mat4x3 *)ws);
		At<uint32_t>(ws, 0x3C) &= ~0x2000u;
		At<int32_t>(ws, 0x38) = 0;
		At<uint32_t>(ws, 0x20) = game_addr(game::unk_13A0978);
		PacketCursor() = w::PrimSet(ws + 0x20, RenderOT(), 2, PacketCursor());
		FieldFree(0x8C);
	}

	static void DrawE(const Pass &p)
	{
		uint8_t *fr = p.fr;
		F32(fr, 0x58) = 0;
		F32(fr, 0x5C) = 0;
		F32(fr, 0x60) = Center();
		w::RotY(0x800, fr + 0x44);
		ComposeAffineTransform(&Camera(), (Mat4x3 *)(fr + 0x44), (Mat4x3 *)(fr + 0x44));
		F32(fr, 0x6C) = (int32_t)(MB() + 0x31D4);
		F32(fr, 0x68) = 0;
		F16(fr, 0x64) = -128;
		Play(p, fr + 0x44);
	}

	static void DrawBonePrim(const Pass &p, int bo, int32_t scroll, int32_t scroll1, bool next_in, int16_t bias, uint32_t table)
	{
		uint8_t *fr = p.fr;
		w::BonePos(E(p.nd), game_addr(game::unk_13B7BB0), fr + bo);
		F32(fr, 0x58) = F16(fr, bo);
		F32(fr, 0x5C) = F16(fr, bo + 2);
		F32(fr, 0x60) = F16(fr, bo + 4);
		w::Billboard(fr + 0x44);
		F16(fr, 0x66) = (int16_t)scroll;
		F32(fr, 0x6C) = (int32_t)(MB() + 0x31D4);
		F32(fr, 0x68) = (int32_t)table;
		F16(fr, 0x64) = bias;
		Play(p, fr + 0x44);
	}

	static void DrawG(const Pass &p, int32_t v)
	{
		uint8_t *fr = p.fr;
		F32(fr, 0x58) = 0;
		F32(fr, 0x5C) = 0;
		F32(fr, 0x60) = Ground() - 0x400;
		w::RotY(0x800, fr + 0x44);
		F32(fr, 0x20) = 0x1000;
		F32(fr, 0x1C) = 0x1000;
		F32(fr, 0x24) = 0x1400;
		w::Scale3D(fr + 0x44, fr + 0x1C);
		ComposeAffineTransform(&Camera(), (Mat4x3 *)(fr + 0x44), (Mat4x3 *)(fr + 0x44));
		int32_t s = shl32(v, 5);
		F32(fr, 0x2C) = s;
		F16(fr, 0x66) = (int16_t)s;
		F32(fr, 0x68) = game_addr(game::unk_13EF430);
		F16(fr, 0x64) = -128;
		F32(fr, 0x6C) = (int32_t)(MB() + 0x31D4);
		Play(p, fr + 0x44);
	}

	static void DrawRing(int32_t scroll, bool fading, int32_t fade)
	{
		uint8_t *ws = (uint8_t *)FieldAlloc(0x8C);
		if (fading) At<int32_t>(ws, 0x2C) = fade;
		if (!fading || fade < 0x1000)
		{
			w::MatY(0x800, 0x1000, 0x1000, ws);
			At<int32_t>(ws, 0x14) = -500;
			At<int32_t>(ws, 0x18) = 0;
			At<int32_t>(ws, 0x1C) = Ground();
			ComposeAffineTransform(&Camera(), (Mat4x3 *)ws, (Mat4x3 *)ws);
			GteSetRotMatrix((Mat4x3 *)ws);
			GteSetTransVector((Mat4x3 *)ws);
			if (fading)
			{
				At<int32_t>(ws, 0x3C) = 0x20F0;
				At<int32_t>(ws, 0x28) = 0;
			}
			else At<int32_t>(ws, 0x3C) = 0x2030;
			At<int16_t>(ws, 0x46) = 0;
			At<int16_t>(ws, 0x44) = 0;
			At<int16_t>(ws, 0x4A) = 0x100;
			At<int16_t>(ws, 0x48) = 0x100;
			At<int16_t>(ws, 0x4C) = 0;
			At<int16_t>(ws, 0x4E) = 0x80;
			At<int16_t>(ws, 0x50) = 0x100;
			At<int16_t>(ws, 0x52) = 0x80;
			At<int16_t>(ws, 0x40) = 0;
			At<int16_t>(ws, 0x42) = (int16_t)(scroll & 0x7F);
			At<int32_t>(ws, 0x38) = 0;
			At<uint32_t>(ws, 0x24) = game_addr(game::unk_13AEBC8);
			At<uint32_t>(ws, 0x20) = game_addr(game::unk_13AEBC0);
			PacketCursor() = w::PrimSet(ws + 0x20, RenderOT(), 2, PacketCursor());
		}
		FieldFree(0x8C);
	}

	struct ParJ { int32_t r[4][10]; };
	static void GenJ(int32_t v, ParJ &p)
	{
		int32_t q = (int32_t)((uint32_t)shl32(v, 12) / 42u) >> 2;
		int32_t f = v * 372;
		if (f > 0x2000) f = 0x2000;
		int32_t g = v * 93 + 0x1000, s = shl32(v, 5), b = shl32(v + 4, 9);
		int32_t a[4][10] = {
			{ 0, (int32_t)game_addr(game::unk_13A8508), shl32(v, 6), 0x1400, q, b, 0x2000, 2, s, f },
			{ 0, (int32_t)game_addr(game::unk_13AB5C0), shl32(v, 6), shl32(v, 7), q, b, 0x2000, 0, s, g },
			{ 0, (int32_t)game_addr(game::unk_13AC1B8), shl32(-v, 6), shl32(v, 7), q, b, 0x2000, 1, s, g },
			{ 0, (int32_t)game_addr(game::unk_13ACDB0), shl32(v, 6), shl32(v, 7), q, b, 0x2000, 2, s, g } };
		ff8fx::copy<(sizeof(a))>(p.r, a);
	}
	static void DrawJ(const Pass &p, int32_t v)
	{
		uint8_t *fr = p.fr;
		F32(fr, 0x94) = -(int32_t)F16(fr, 0x1E);
		F32(fr, 0x90) = 0;
		F32(fr, 0x98) = Center() - Ground();
		w::Basis(fr + 0x90, fr + 0x70);
		F32(fr, 0x84) = F16(fr, 0x1C);
		F32(fr, 0x88) = F16(fr, 0x1E);
		F32(fr, 0x8C) = F16(fr, 0x20);
		ComposeAffineTransform(&Camera(), (Mat4x3 *)(fr + 0x70), (Mat4x3 *)(fr + 0x44));
		F32(fr, 0x68) = 0;
		F16(fr, 0x64) = -128;
		F32(fr, 0x6C) = (int32_t)(MB() + 0x31D4);
		Play(p, fr + 0x44);
		ParJ r;
		GenJ(v, r);
		for (int k = 0; k < 4; k++) w::Ribbon(fr + 0x70, r.r[k]);
	}

	struct ParK
	{
		int32_t r[4][10];
		struct Tb { int32_t vis, flags, fade, ang, scale, yscale; } tb[4];
	};
	static void GenK(int32_t v, ParK &p)
	{
		ff8fx::fill<(sizeof(p))>(&p, 0);
		int32_t f = (uint32_t)v <= 0x18 ? 0x2000 : shl32(v - 8, 9);
		int32_t s = shl32(v, 5);
		int32_t a[4][10] = {
			{ 0, (int32_t)game_addr(game::unk_13A8508), shl32(v, 6), shl32(v, 7), 0x400, 0x5800, 0x2000, 2, s, f },
			{ 0, (int32_t)game_addr(game::unk_13AB5C0), shl32(v, 6), shl32(v, 7), 0x400, 0x5800, 0x2000, 2, s, f },
			{ 0, (int32_t)game_addr(game::unk_13AC1B8), shl32(-v, 6), shl32(v, 7), 0x400, 0x5800, 0x2000, 2, s, f },
			{ 0, (int32_t)game_addr(game::unk_13ACDB0), shl32(v, 6), shl32(v, 7), 0x400, 0x5800, 0x2000, 2, s, f } };
		ff8fx::copy<(sizeof(a))>(p.r, a);
		for (int k = 0; k < 4; k++)
		{
			const int16_t *t = (const int16_t *)(game_addr(game::unk_13BBBE4) - 0xC * k);
			int32_t start = t[0], end = t[1];
			ParK::Tb &b = p.tb[k];
			if ((uint32_t)v <= (uint32_t)start || (uint32_t)v >= (uint32_t)(end + 8)) continue;
			b.vis = 1;
			if ((uint32_t)v < (uint32_t)(start + 8)) { b.flags = 0xF0; b.fade = shl32(start + 8, 9) - shl32(v, 9); }
			else if ((uint32_t)v > (uint32_t)end) { b.flags = 0xF0; b.fade = shl32(v - end, 9); }
			else b.flags = 0x30;
			b.ang = shl32(v, 6);
			b.scale = (int16_t)((int16_t)((int16_t)v - t[0]) * t[-3] + t[-4]);
			b.yscale = t[-2];
		}
	}
	static void DrawK(const Pass &p, int32_t v)
	{
		uint8_t *fr = p.fr;
		ParK r;
		GenK(v, r);
		w::RotX(0x400, fr + 0x90);
		F32(fr, 0xA4) = 0;
		F32(fr, 0xA8) = 0;
		F32(fr, 0xAC) = Ground();
		for (int k = 0; k < 4; k++) w::Ribbon(fr + 0x90, r.r[k]);
		uint8_t *ws = (uint8_t *)FieldAlloc(0x8C);
		int32_t y = -0x15C8;
		At<int16_t>(ws, 0x42) = 0;
		At<int16_t>(ws, 0x40) = 0;
		At<int32_t>(ws, 0x28) = 0;
		for (int k = 0; k < 4; k++, y += 0x898)
		{
			const ParK::Tb &b = r.tb[k];
			if (!b.vis) continue;
			At<int32_t>(ws, 0x3C) = b.flags;
			if (b.flags == 0xF0) At<int32_t>(ws, 0x2C) = b.fade;
			w::MatY(b.ang, b.scale, b.yscale, ws);
			At<int32_t>(ws, 0x14) = 0;
			At<int32_t>(ws, 0x18) = y;
			At<int32_t>(ws, 0x1C) = Ground();
			ComposeAffineTransform(&Camera(), (Mat4x3 *)ws, (Mat4x3 *)ws);
			GteSetRotMatrix((Mat4x3 *)ws);
			GteSetTransVector((Mat4x3 *)ws);
			At<int32_t>(ws, 0x38) = 0;
			At<uint32_t>(ws, 0x20) = game_addr(game::unk_13AE0F8);
			PacketCursor() = w::PrimSet(ws + 0x20, RenderOT(), 2, PacketCursor());
		}
		FieldFree(0x8C);
	}

	static void DrawTargets(const Pass &p)
	{
		uint8_t *nd = p.nd, *fr = p.fr;
		F16(fr, 0x20) = 0;
		F16(fr, 0x1C) = 0;
		F32(fr, 0x3C) = 0;
		for (int i = 0; i < Count(); i++)
		{
			uint8_t *rec = Rec(nd, i), *ent = Ent(nd, i);
			if (rec[0x1C]) continue;
			if (rec[0x1D] && (ent[0] & 2))
			{
				F16(fr, 0x1E) = (int16_t)(((int32_t)At<int16_t>(rec, 4) + At<int16_t>(rec, 6)) >> 1);
				int32_t s = At<int16_t>(rec, 0xA);
				F32(fr, 0x34) = s;
				F32(fr, 0x30) = s;
				F32(fr, 0x2C) = s;
				At<int32_t>(ent, 0x54) = At<int16_t>(ent, 0x1C);
				At<int32_t>(ent, 0x58) = At<int16_t>(ent, 0x1E);
				At<int32_t>(ent, 0x5C) = At<int16_t>(ent, 0x20);
				if (rec[0x1E])
				{
					uint8_t *e = ent;
					do
					{
						if (e[0] & 2) w::EntDraw(e, rec + 0x14, fr + 0x1C, fr + 0x2C);
						e = At<uint8_t *>(e, 0x8C);
					} while (e != ent && e);
				}
				else w::EntDraw(ent, rec + 0x14, fr + 0x1C, fr + 0x2C);
			}
			if (rec[0x1E])
			{
				uint8_t *e = ent;
				do
				{
					if (At<uint16_t>(e, 0) & 2) At<uint16_t>(e, 0) |= 4;
					e = At<uint8_t *>(e, 0x8C);
				} while (e != ent && e);
			}
			else ent[0] |= 4;
		}
	}

	static bool CreatureDrawn(int32_t c) { return c >= 100 && c < 377 && (c < 224 || c >= 254); }

	static void MotionSink(uint8_t *nd, int32_t v)
	{
		for (int i = 0; i < Count(); i++)
		{
			uint8_t *ent = Ent(nd, i);
			At<int16_t>(ent, 0x20) += (int16_t)v;
			if ((uint32_t)v >= (uint32_t)(15 + 2 * i))
			{
				At<int16_t>(ent, 0x20) += (int16_t)shl32(v, 5);
				At<int16_t>(ent, 0x1E) -= (int16_t)shl32(v, 3);
			}
		}
	}

	static void MotionLift(uint8_t *nd, uint8_t *fr, int32_t v)
	{
		int32_t wgt = (int32_t)((uint32_t)shl32(v, 12) / 20u);
		for (int i = 0; i < Count(); i++)
		{
			uint8_t *rec = Rec(nd, i), *ent = Ent(nd, i);
			wgt -= 0x100;
			if (wgt > 0x1100) { rec[0x1D] = 0; continue; }
			F16(fr, 0x3C) = At<int16_t>(rec, 0xC);
			F16(fr, 0x3E) = (int16_t)(At<int16_t>(rec, 0xE) - 0x3E8);
			F16(fr, 0x40) = At<int16_t>(rec, 0x10);
			F16(fr, 0x1C) = F16(fr, 0x2C);
			int32_t avg = ((int32_t)At<int16_t>(rec, 4) + At<int16_t>(rec, 6)) / 2;
			F16(fr, 0x1E) = (int16_t)(F32(fr, 0x2E) - avg);
			F16(fr, 0x20) = F16(fr, 0x30);
			if (i & 1)
			{
				At<int16_t>(rec, 0x18) += 0x200;
				At<int16_t>(rec, 0x16) += (int16_t)shl32(v, 3);
			}
			else
			{
				At<int16_t>(rec, 0x18) += (int16_t)0xFE00;
				At<int16_t>(rec, 0x16) += (int16_t)shl32(-v, 3);
			}
			w::VecLerp(fr + 0x3C, fr + 0x1C, 0x1000 - wgt, wgt, ent + 0x1C);
			wgt = 0x1000 - w::Cos(wgt >> 2);
			if (wgt < 0) wgt = 0;
			At<int16_t>(rec, 0xA) = (int16_t)(mul32(At<int16_t>(rec, 8), wgt) / 4096 - wgt + 0x1000);
		}
	}

	static void MotionSpin(uint8_t *nd, uint8_t *fr, int32_t v)
	{
		int32_t wgt = (int32_t)((uint32_t)shl32(v, 12) >> 5);
		for (int i = 0; i < Count(); i++)
		{
			uint8_t *rec = Rec(nd, i), *ent = Ent(nd, i);
			wgt -= 0x100;
			if (wgt < 0) continue;
			int32_t d;
			if (i & 1) { At<int16_t>(rec, 0x18) += 0x200; d = shl32(v, 3); }
			else { At<int16_t>(rec, 0x18) += (int16_t)0xFE00; d = shl32(-v, 3); }
			At<int16_t>(rec, 0x16) += (int16_t)d;
			if (wgt >= 0xC00)
			{
				At<int16_t>(ent, 0x1E) += (int16_t)0xFE00;
				At<int16_t>(rec, 0xA) = 0x1000;
				continue;
			}
			F16(fr, 0x10) = (int16_t)(At<int16_t>(rec, 0xC) >> 1);
			F16(fr, 0x12) = At<int16_t>(rec, 0xE);
			F16(fr, 0x14) = At<int16_t>(rec, 0x10);
			F16(fr, 0x3C) = F16(fr, 0x1C);
			int32_t avg = ((int32_t)At<int16_t>(rec, 4) + At<int16_t>(rec, 6)) / 2;
			F16(fr, 0x3E) = (int16_t)(F32(fr, 0x1E) - avg);
			F16(fr, 0x40) = F16(fr, 0x20);
			w::VecLerp(fr + 0x10, fr + 0x3C, wgt, 0x1000 - wgt, ent + 0x1C);
			wgt = ComputeSin(wgt >> 2);
			rec[0x1D] = 1;
			At<int16_t>(rec, 0xA) = (int16_t)(mul32(At<int16_t>(rec, 8), 0x1000 - wgt) / 4096 + wgt);
		}
	}

	static void OrbitReset(uint8_t *nd)
	{
		for (int i = 0; i < Count(); i++)
		{
			uint8_t *rec = Rec(nd, i), *ent = Ent(nd, i);
			At<uint32_t>(ent, 0x1C) = At<uint32_t>(rec, 0xC);
			At<uint32_t>(ent, 0x20) = At<uint32_t>(rec, 0x10);
			At<uint32_t>(rec, 0x14) = At<uint32_t>(ent, 0xC);
			At<uint32_t>(rec, 0x18) = At<uint32_t>(ent, 0x10);
			At<int16_t>(rec, 0x18) = (int16_t)((i & 1) ? 0x100 : -0x100);
		}
	}

	static void MotionOrbit(uint8_t *nd, int32_t v)
	{
		int32_t base = shl32(v, 7);
		for (int i = 0; i < Count(); i++)
		{
			uint8_t *rec = Rec(nd, i), *ent = Ent(nd, i);
			At<int16_t>(rec, 0x16) += (int16_t)((i & 1) ? 0x100 : -0x100);
			int32_t a = (ent[4] == 0x8E ? mul32(v, -800) : mul32(v, -350)) - shl32(i, 11);
			At<int16_t>(ent, 0x1E) = (int16_t)a;
			int32_t a2 = (int16_t)(a << 1);
			int32_t b = (base + a2) & 0xFFF;
			int32_t r = (int16_t)(*(const int16_t *)(game_addr(game::unk_13B7BB8) + 2 * (a2 & 0xFFE)) >> 2);
			At<int16_t>(ent, 0x1C) = (int16_t)(mul32(SinT(b), r) >> 12);
			At<int16_t>(ent, 0x20) = (int16_t)((mul32(CosT(b), r) >> 12) + Ground());
		}
	}

	static void DropReset(uint8_t *nd)
	{
		for (int i = 0; i < Count(); i++)
		{
			uint8_t *rec = Rec(nd, i), *ent = Ent(nd, i);
			int16_t x = At<int16_t>(rec, 0xC);
			At<int16_t>(rec, 0x18) += 0x800;
			At<int16_t>(ent, 0x1C) = x;
			At<int16_t>(ent, 0x20) = At<int16_t>(rec, 0x10);
			At<int16_t>(ent, 0x1E) -= At<int16_t>(rec, 4);
		}
	}

	static void MotionDrop(uint8_t *nd, int32_t v, bool real)
	{
		for (int i = 0; i < Count(); i++)
		{
			uint8_t *rec = Rec(nd, i), *ent = Ent(nd, i);
			uint8_t landed = rec[0x1C];
			if (landed) { rec[0x1C] = landed + 1; continue; }
			At<int16_t>(rec, 0x16) += (int16_t)((i & 1) ? 0x100 : -0x100);
			At<int16_t>(ent, 0x1E) += 0x400;
			if ((int32_t)At<int16_t>(ent, 0x1E) < -(int32_t)At<int16_t>(rec, 4))
			{
				if (real && (v & 3) == 0)
				{
					uint8_t *t = (uint8_t *)AddTaskToQueue(&QFall(), game_addr(ORIG_FallPrimTask));
					if (t)
					{
						At<uint8_t *>(t, 0xC) = rec;
						w::Decode((uint32_t)(MB() + 0x30760), t + 0x10, 0x8C);
					}
				}
				continue;
			}
			if (real)
			{
				uint8_t *t = (uint8_t *)AddTaskToQueue(&QLand(), game_addr(ORIG_LandPrimTask));
				if (t)
				{
					At<uint8_t *>(t, 0xC) = rec;
					w::Decode((uint32_t)(MB() + 0x2EAA0), t + 0x10, 0x180);
				}
			}
			At<int16_t>(ent, 0x1E) = (int16_t)-At<int16_t>(rec, 4);
			At<int16_t>(rec, 0x18) = At<int16_t>(ent, 0x10);
			At<int16_t>(rec, 0x16) = (int16_t)(At<int16_t>(ent, 0xE) + 0x800);
			if (real)
			{
				w::HitTarget(ent);
				if (rec[0x1E])
				{
					uint8_t *e = ent;
					do
					{
						if (At<uint16_t>(e, 0) & 2) At<uint16_t>(e, 0) &= 0xFFFB;
						e = At<uint8_t *>(e, 0x8C);
					} while (e != ent && e);
				}
				else At<uint16_t>(ent, 0) &= 0xFFFB;
			}
			rec[0x1C] = 1;
			if (real && i == 0) ShakeY() += 0x3E8;
		}
	}

	static void SpawnFloorParticles(uint8_t *nd)
	{
		int k = 4;
		for (int16_t *q = w::PartAlloc(nd); q; q = w::PartAlloc(nd))
		{
			int32_t r = w::Rand() & 0xFFF;
			q[4] = (int16_t)(SinT(r) >> 3);
			q[6] = (int16_t)(CosT(r) >> 3);
			q[5] = 0;
			q[3] = 0xC;
			q[7] = 0x1000;
			if (--k == 0) break;
		}
	}

	static void SpawnHandParticles(uint8_t *nd, uint8_t *fr, int bo, int32_t n, bool half)
	{
		for (int32_t k = n; k != 0; k--)
		{
			int16_t *q = w::PartAlloc(nd);
			if (!q) break;
			int32_t r = w::Rand() & 0xFFF;
			int16_t sx = half ? (int16_t)(SinT(r) >> 1) : SinT(r);
			q[4] = sx;
			q[5] = half ? (int16_t)(CosT(r) >> 1) : CosT(r);
			int16_t cz = q[5];
			q[0] = (int16_t)(sx + F32(fr, bo));
			q[1] = (int16_t)(F32(fr, bo + 2) + cz);
			q[2] = (int16_t)(F32(fr, bo + 4) - 0x1000);
			q[6] = 0x200;
			q[4] = (int16_t)(-(int32_t)sx >> 3);
			q[5] = (int16_t)(-(int32_t)cz >> 3);
			q[3] = 8;
			q[7] = (int16_t)(half ? 0x800 : 0x1000);
		}
	}

	static void SpawnSwirls(int32_t n, int16_t r, int16_t y, int16_t va, int16_t vb, int16_t da)
	{
		for (int32_t k = n; k != 0; k--)
		{
			uint8_t *t = (uint8_t *)AddTaskToQueue(&QFlip(), game_addr(ORIG_SwirlFlipTask));
			if (!t) break;
			At<int16_t>(t, 0xC) = 0;
			At<int16_t>(t, 0x16) = va;
			At<int16_t>(t, 0x18) = vb;
			At<int16_t>(t, 0x10) = (int16_t)w::Rand();
			At<int16_t>(t, 0x12) = da;
			At<int16_t>(t, 0x14) = y;
			At<int16_t>(t, 0xE) = r;
		}
	}

	static bool CamStep(uint8_t *nd, int32_t c, bool real)
	{
		bool cut = false;
		int32_t y8 = Center();
		if ((uint32_t)c < 0x28)
		{
			if (c == 0)
			{
				cut = true;
				if (real) w::PlaySE(game_addr(game::unk_13B7A48));
				Roll() = 0;
				W1D8E038() = 0xF0;
				CamL(0) = 0;
				CamL(1) = (int16_t)0xFC7C;
				CamL(2) = (int16_t)(y8 - 0x7D0);
				CamW(0) = 0;
				CamW(1) = (int16_t)0xFB50;
				CamW(2) = (int16_t)(y8 - 200);
			}
			w::CamRotate(0, shl32(0x228 - c, 3));
			CamW(1) = (int16_t)0xFB50;
		}
		if (c == 0x27)
		{
			cut = true;
			W1D8E038() = 0x120;
			CamL(0) = 0;
			CamL(1) = (int16_t)0xF458;
			CamL(2) = (int16_t)(y8 - 0x840);
			CamW(0) = (int16_t)0xF000;
			CamW(1) = (int16_t)0xFB48;
			CamW(2) = (int16_t)(y8 - 0x2396);
		}
		if ((uint32_t)(c - 0x28) < 0x3C) w::CamRotate(0x10, 0x1000);
		if (c == 0x63)
		{
			cut = true;
			if (real)
			{
				uint8_t *e = E(nd);
				At<int16_t>(nd, 0x1A2) = 0;
				At<int16_t>(nd, 0x1A4) = 0x1800;
				At<int16_t>(nd, 0x19C) = 0x1800;
				At<int16_t>(nd, 0x194) = 0x1800;
				At<int16_t>(nd, 0x1A0) = 0;
				At<int16_t>(nd, 0x19E) = 0;
				At<int16_t>(nd, 0x19A) = 0;
				At<int16_t>(nd, 0x198) = 0;
				At<int16_t>(nd, 0x196) = 0;
				At<int32_t>(nd, 0x1A8) = 0;
				At<int16_t>(nd, 0x170) = 0;
				At<int32_t>(nd, 0x1AC) = -0x2000;
				At<int16_t>(nd, 0x172) = (int16_t)0xE000;
				At<int32_t>(nd, 0x1B0) = Center();
				At<int16_t>(nd, 0x174) = (int16_t)Center();
				w::Bind(e, nd + 0x41C, (uint32_t)(MB() + 0x31220));
				w::ReadAnim6(e, 0);
			}
			CamL(2) = (int16_t)Center();
			CamL(0) = 0;
			CamL(1) = (int16_t)0xEE61;
			CamW(0) = 0;
			CamW(1) = (int16_t)0xF976;
			CamW(2) = (int16_t)(Center() - 0x936);
			if (real) w::PlaySE(game_addr(game::unk_13B7A4C));
		}
		y8 = Center();
		if (c == 0x6B && real) w::PlayStream();
		if (c == 0x7B)
		{
			cut = true;
			CamL(0) = (int16_t)0xFB6E;
			CamL(1) = (int16_t)0xEA57;
			CamL(2) = (int16_t)(y8 - 0x484);
			CamW(0) = 0x5F9;
			CamW(1) = (int16_t)0xEEEA;
			CamW(2) = (int16_t)(y8 + 0xA08);
		}
		{
			uint32_t v = (uint32_t)(c - 0x7C);
			if (v < 0x1E)
			{
				w::CamRotate(0, 0x1054);
				CamL(1) += 0x82;
				uint32_t s = v - 0x14;
				if (s < 6)
				{
					int32_t k = (int32_t)(s * 0x155 + 0xAA);
					int16_t sh = (int16_t)(mul32(w::Cos(k), 100) >> 12);
					ShakeY() = sh;
					if (s & 1) ShakeY() = (int16_t)-sh;
				}
			}
		}
		if (c == 0x99)
		{
			cut = true;
			CamL(0) = (int16_t)0xFDEC;
			CamL(1) = (int16_t)0xF768;
			CamL(2) = (int16_t)(y8 - 0xEA);
			CamW(0) = 0x76B;
			CamW(1) = (int16_t)0xF7B7;
			CamW(2) = (int16_t)(y8 - 0x557);
		}
		if ((uint32_t)(c - 0x9A) < 0xA) w::CamRotate(-0x15, 0xFA0);
		else if (c - 0x9A == 0xA)
		{
			cut = true;
			CamL(0) = (int16_t)0xFE25;
			CamL(1) = (int16_t)0xF624;
			CamL(2) = (int16_t)(y8 - 0x171);
			CamW(0) = (int16_t)0xFE95;
			CamW(1) = (int16_t)0xFA8B;
			CamW(2) = (int16_t)(y8 - 0x1CCD);
		}
		{
			uint32_t v = (uint32_t)(c - 0xB7);
			if (v < 0x29)
			{
				int32_t s = (int32_t)(shl32(v, 12) / 41u);
				if (v == 0 && real) w::PlayStream();
				int32_t k1 = w::Cos(s >> 2);
				int32_t k2 = w::Cos((0x1000 - k1) >> 2);
				int32_t wb = 0x1000 - k2, wa = 0x1000 - wb;
				int16_t a[3] = { (int16_t)0xFE25, (int16_t)0xF624, (int16_t)(Center() - 0x171) };
				int16_t b[3] = { (int16_t)0xFE64, (int16_t)0xF31C, (int16_t)(Center() + 0x4E4) };
				w::VecLerp(a, b, wa, wb, &CamL(0));
				int16_t a2[3] = { (int16_t)0xFE95, (int16_t)0xFA8B, (int16_t)(Center() - 0x1CCD) };
				int16_t b2[3] = { (int16_t)0xFEA9, (int16_t)0xF769, (int16_t)(Center() - 0x25D) };
				w::VecLerp(a2, b2, wa, wb, &CamW(0));
			}
		}
		if (c == 0xDF)
		{
			cut = true;
			if (real) w::PlaySE(game_addr(game::unk_13B7A50));
			int32_t g = Ground();
			CamL(0) = 0x34F;
			CamL(1) = (int16_t)0xF9F4;
			CamL(2) = (int16_t)(g + 0x250);
			CamW(0) = 0x14E2;
			CamW(1) = (int16_t)0xFE1B;
			CamW(2) = (int16_t)(g + 0x83C);
		}
		if ((uint32_t)(c - 0x9A) < 0x1E) w::CamRotate(0, 0x1000);
		y8 = Center();
		if (c == 0xFD)
		{
			cut = true;
			CamL(0) = 0;
			CamL(1) = (int16_t)0xF4EE;
			CamL(2) = (int16_t)y8;
			CamW(0) = (int16_t)0xFAEA;
			CamW(1) = (int16_t)0xFEB1;
			CamW(2) = (int16_t)(y8 + 0x148C);
		}
		{
			uint32_t v = (uint32_t)(c - 0xFE);
			if (v < 0x1E)
			{
				int32_t a = (int32_t)((shl32(v, 12) / 30u) >> 1);
				w::CamRotate(ComputeSin(a) >> 8, 0xFA0);
				CamW(1) += (int16_t)0xFF38;
			}
		}
		if (c == 0x11B)
		{
			cut = true;
			int32_t g = Ground();
			CamL(0) = (int16_t)0xFEC5;
			CamL(1) = (int16_t)0xF904;
			CamW(0) = (int16_t)0xFA97;
			CamL(2) = (int16_t)(g + 0x1395);
			CamW(1) = (int16_t)0xFD39;
			CamW(2) = (int16_t)(g - 0xA62);
		}
		y8 = Center();
		if (c == 0x13F)
		{
			cut = true;
			CamL(0) = (int16_t)0xFF8D;
			CamL(1) = (int16_t)0xF556;
			CamL(2) = (int16_t)(y8 + 0x279);
			CamW(0) = 0x159;
			CamW(1) = (int16_t)0xF07F;
			CamW(2) = (int16_t)(y8 - 0x1EB);
		}
		{
			uint32_t v = (uint32_t)(c - 0x14E);
			if (v < 0x2B)
			{
				int32_t s = (int32_t)(shl32(v, 12) / 43u);
				if (v == 0 && real) w::PlayStream();
				int32_t k = ComputeSin(s >> 2);
				int16_t a[3] = { (int16_t)0xFF8D, (int16_t)0xF556, (int16_t)(Center() + 0x279) };
				int16_t b[3] = { (int16_t)0xFD15, (int16_t)0xF982, (int16_t)(Center() - 0x795) };
				w::VecLerp(a, b, 0x1000 - k, k, &CamL(0));
				int16_t a2[3] = { 0x159, (int16_t)0xF07F, (int16_t)(Center() - 0x1EB) };
				int16_t b2[3] = { 0xD7E, (int16_t)0xFEA4, (int16_t)(Center() - 0x1EAB) };
				w::VecLerp(a2, b2, 0x1000 - k, k, &CamW(0));
			}
		}
		if (c == 0x178)
		{
			cut = true;
			if (real) w::PlaySE(game_addr(game::unk_13B7A54));
			CamL(0) = 0;
			uint8_t *ent = Ent(nd, 0);
			int32_t g = Ground();
			CamL(2) = (int16_t)g;
			CamL(1) = (int16_t)((((int32_t)At<int16_t>(ent, 0x1E) + 0x1000) >> 2) - 0x1000);
			CamW(0) = (int16_t)0xFC88;
			CamW(1) = (int16_t)0xF1A7;
			CamW(2) = (int16_t)(g - 0x1DD7);
		}
		if ((uint32_t)(c - 0x179) < 0x28)
		{
			uint8_t *ent = Ent(nd, 0);
			CamL(1) = (int16_t)((((int32_t)At<int16_t>(ent, 0x1E) + 0x1000) >> 2) - 0x1000);
			w::CamRotate(0x20, 0x1000);
		}
		if ((uint32_t)(c - 0x1B8) < 0x28)
		{
			if (c - 0x1B8 == 8 && real) w::PlaySE(game_addr(game::unk_13B7A58));
			if (Rec(nd, 0)[0x1C] <= 2)
			{
				uint8_t *ent = Ent(nd, 0);
				w::DefaultPos(ent, &CamL(0));
				int16_t half = (int16_t)(At<int16_t>(ent, 0x26) >> 1);
				CamW(0) = (int16_t)(half + CamL(0) + 0xBB8);
				int16_t lz = CamL(1);
				CamW(2) = CamL(2);
				CamW(1) = (int16_t)((int16_t)(lz >> 2) - 0x3E8);
				CamL(1) = (int16_t)(lz >> 1);
			}
		}
		if (c == 0x1DC && real) w::ReleaseVoice(At<uint32_t>(nd, 0x10));
		return cut;
	}

	static uint32_t __cdecl TimelineTask(TaskNode *n)
	{
		uint8_t *nd = (uint8_t *)n;
		uint8_t *fr = g_fr;
		int32_t c = At<int16_t>(nd, 0xC);

		Pass p = { nd, fr, c };

		if (c < 8) Flash() = (2800 * c) >> 3;
		else if (c <= 0x1D8) Flash() = 0xAF0;
		else Flash() = (2800 * (480 - c)) >> 3;

		{
			uint32_t v = (uint32_t)(c - 10);
			if (v < 0x1E)
			{
				if (v == 0) w::Decode(game_addr(game::unk_13AFF48), nd + 0x1F0, 0x12C);
				else if (v >= 8) DrawA(p, (int32_t)v);
				if (!Pause())
				{
					int k = 4;
					for (int16_t *q = w::PartAlloc(nd); q; q = w::PartAlloc(nd))
					{
						int32_t r = w::Rand();
						q[1] = 0x3E8;
						q[0] = (int16_t)((r & 0xFFF) - 0x800);
						q[2] = (int16_t)(Center() + 0x3E8);
						q[3] = 0x14;
						q[4] = 0;
						q[5] = -100;
						q[6] = -400;
						q[7] = 0x1000;
						if (--k == 0) break;
					}
					w::PartFall(nd);
				}
				if (v > 0xE)
				{
					int32_t v6 = (int32_t)v - 0xE;
					DrawB(p, v6);
					if (!Pause())
					{
						int32_t e = (int32_t)((uint32_t)shl32(v6, 12) / 14u);
						SpawnSwirls(v6 >> 1, (int16_t)(e >> 2), (int16_t)(Center() + 0x7D0), 0x1000, 0x800, 0x80);
					}
				}
			}
		}

		{
			uint32_t v = (uint32_t)(c - 0x28);
			if (v < 0x3C)
			{
				int32_t e = (int32_t)((uint32_t)shl32(v, 12) / 60u);
				if (v == 0) w::PartReset(nd);
				if (!Pause())
				{
					SpawnFloorParticles(nd);
					SpawnSwirls((int32_t)(v >> 1), (int16_t)shl32(v, 7), (int16_t)Center(), 0x2000, 0x1000, 0);
					w::MatY(shl32(v, 6), shl32(v + 0x40, 6), 0x2000, fr + 0x70);
					F32(fr, 0x88) = 0;
					F32(fr, 0x84) = 0;
					F32(fr, 0x8C) = Center();
					GteSetRotMatrix((Mat4x3 *)(fr + 0x70));
					GteSetTransVector((Mat4x3 *)(fr + 0x70));
					w::PartOrbit(nd, shl32(v, 7), e >> 1);
				}
				DrawC(p, (int32_t)v);
			}
		}

		{
			uint32_t v = (uint32_t)(c - 0x64);
			if (v < 0x36)
			{
				if (!Pause())
				{
					if (v <= 0x18)
					{
						At<int32_t>(nd, 0x1AC) = (int32_t)(shl32(v, 13) / 24) - 0x2000;
						SpawnFloorParticles(nd);
						w::MatY(shl32(v, 6), shl32(v + 0x40, 6), 0x2000, fr + 0x70);
						F32(fr, 0x88) = 0;
						F32(fr, 0x84) = 0;
						F32(fr, 0x8C) = Center();
						GteSetRotMatrix((Mat4x3 *)(fr + 0x70));
						GteSetTransVector((Mat4x3 *)(fr + 0x70));
						w::PartOrbit(nd, 0x3C, 0x800);
						if (v == 0x18)
						{
							int16_t *q = (int16_t *)(nd + 0x42C);
							for (int k = 0; k < 0x80; k++, q += 12)
							{
								if (q[3] == 0) continue;
								if (q[5] < -0x400)
								{
									q[3] += 0x18;
									q[4] = (int16_t)((int32_t)q[0] / 256);
									q[6] = (int16_t)(((int32_t)q[2] - Center()) / 256);
									q[5] = (int16_t)((w::Rand() & 0x7F) + 0x40);
								}
								else
								{
									q[3] = 0;
									At<uint32_t>(nd, 0x102C) = (uint32_t)q;
								}
							}
						}
						else if (v == 0xE)
						{
							for (int32_t k = 2;; k--)
							{
								uint8_t *t = (uint8_t *)AddTaskToQueue(&QTrail(), game_addr(ORIG_TrailTask));
								At<int16_t>(t, 0xC) = 0;
								At<int32_t>(t, 0x20) = 0;
								At<int16_t>(t, 0x10) = (int16_t)(shl32(k, 11) - 0x1770);
								At<int16_t>(t, 0x1A) = 0x1000;
								At<int16_t>(t, 0x1E) = 0;
								At<int16_t>(t, 0x1C) = 0;
								At<int16_t>(t, 0x14) = 0xD2;
								At<int16_t>(t, 0x18) = 0x40;
								At<int16_t>(t, 0x12) = (int16_t)(k * 0x555);
								At<int16_t>(t, 0x16) = (int16_t)(k * 0x555);
								if (k & 1)
								{
									At<int16_t>(t, 0x14) = (int16_t)0xFF2E;
									At<int16_t>(t, 0x18) = (int16_t)0xFFC0;
								}
								if (k == 0) break;
							}
						}
					}
					else w::PartBounce(nd);
				}
				DrawD(p, (int32_t)v);
			}
		}

		{
			uint32_t v = (uint32_t)(c - 0x90);
			if (v < 0x1E)
			{
				if (v == 0) w::Decode(game_addr(game::unk_13B0C9C), nd + 0x1F0, 0xE0);
				DrawE(p);
			}
		}
		if (c == 0x9A) w::PartReset(nd);

		{
			uint32_t v = (uint32_t)(c - 0xB8);
			if (v < 0x28)
			{
				if (v == 0) w::Decode((uint32_t)(MB() + 0x23220), nd + 0x1F0, 0x22C);
				DrawBonePrim(p, 0x1C, (int32_t)(v * v), (int32_t)((v + 1) * (v + 1)), (uint32_t)(c + 1 - 0xB8) < 0x28, -128, game_addr(game::unk_13EF438));
				if (!Pause())
				{
					SpawnHandParticles(nd, fr, 0x1C, (int32_t)(v >> 3), true);
					w::PartDrift(nd);
				}
			}
		}

		{
			uint32_t v = (uint32_t)(c - 0xE0);
			if (v < 0x1E)
			{
				if (v == 0)
				{
					w::PartReset(nd);
					w::Decode(game_addr(game::unk_13B17AC), nd + 0x1F0, 0x1D8);
				}
				DrawG(p, (int32_t)v);
				if (!Pause())
				{
					for (int32_t k = 3;; k--)
					{
						int16_t *q = w::PartAlloc(nd);
						if (!q) break;
						q[0] = (int16_t)((w::Rand() & 0xFFF) - 0x800);
						q[1] = (int16_t)-(w::Rand() & 0x7FF);
						q[2] = (int16_t)(Ground() - 0x1000);
						q[3] = 0xC;
						q[4] = 0;
						q[5] = 0;
						q[6] = 0x2AA;
						q[7] = 0x1000;
						if (k == 0) break;
					}
					w::PartDrift(nd);
					MotionSink(nd, (int32_t)v);
				}
			}
		}

		{
			uint32_t v = (uint32_t)(c - 0xFE);
			if (v < 0x1E)
			{
				if (v == 0)
				{
					w::PartReset(nd);
					w::Decode((uint32_t)(MB() + 0x23220), nd + 0x1F0, 0x8C);
				}
				bool nx = (uint32_t)(c + 1 - 0xFE) < 0x1E;
				DrawBonePrim(p, 0x2C, shl32(v, 5), shl32(v + 1, 5), nx, 0x40, game_addr(game::unk_13EF42C));
				DrawRing(shl32(v, 5), false, 0);
				if (!Pause())
				{
					SpawnHandParticles(nd, fr, 0x2C, (int32_t)(v >> 2), false);
					for (int32_t k = 3;; k--)
					{
						uint8_t *t = (uint8_t *)AddTaskToQueue(&QFlip(), game_addr(ORIG_RiseFlipTask));
						if (!t) break;
						At<int16_t>(t, 0x12) = 0;
						At<int16_t>(t, 0x18) = 0x2000;
						At<int16_t>(t, 0x1A) = 0x1000;
						int32_t x = (w::Rand() & 0x1FFF) - 0x1000;
						int32_t lo = (int8_t)(uint8_t)x;
						At<int16_t>(t, 0xC) = (int16_t)x;
						int32_t q = (int32_t)(((int64_t)lo * (int32_t)0xD5555555) >> 32) >> 5;
						t[0x14] = (uint8_t)(q + (int32_t)((uint32_t)q >> 31));
						At<int16_t>(t, 0x10) = (int16_t)Ground();
						t[0x16] = 0x14;
						At<int16_t>(t, 0xE) = 0;
						t[0x15] = 0;
						if (k == 0) break;
					}
					w::PartDrift(nd);
					MotionLift(nd, fr, (int32_t)v);
				}
			}
		}

		{
			uint32_t v = (uint32_t)(c - 0x11C);
			if (v < 0x24)
			{
				if (v == 0)
				{
					int16_t *q = (int16_t *)(nd + 0x42C);
					for (int k = 0; k < 0x80; k++, q += 12)
					{
						if (q[3] == 0) continue;
						q[4] >>= 4;
						q[6] >>= 4;
						q[3] = 0x24;
						q[7] = 0x800;
					}
					w::Decode((uint32_t)(MB() + 0x2786C), nd + 0x1F0, 0x11C);
				}
				bool nx = (uint32_t)(c + 1 - 0x11C) < 0x24;
				DrawBonePrim(p, 0x1C, shl32(v, 5), shl32(v + 1, 5), nx, 0x80, game_addr(game::unk_13EF42C));
				DrawRing(shl32(v, 5), true, (int32_t)((uint32_t)shl32(v, 12) >> 4));
				if (!Pause()) w::PartBounce(nd);
			}
		}

		{
			uint32_t v = (uint32_t)(c - 0x14F);
			if (v < 0x2A)
			{
				if (v == 0)
				{
					w::PartReset(nd);
					w::Decode((uint32_t)(MB() + 0x2C074), nd + 0x1F0, 0x1C0);
				}
				w::BonePos(E(nd), game_addr(game::unk_13B7BB0), fr + 0x1C);
				if (!Pause())
				{
					int k = 4;
					for (int16_t *q = w::PartAlloc(nd); q; q = w::PartAlloc(nd))
					{
						q[0] = (int16_t)((w::Rand() & 0xFFF) - 0x800);
						q[1] = 0;
						q[3] = 0xC;
						q[2] = (int16_t)(Center() - 0x3E8);
						q[4] = 0;
						q[5] = (int16_t)(-0x20 - (w::Rand() & 0x40));
						q[6] = -400;
						q[7] = 0x1000;
						if (--k == 0) break;
					}
					w::PartFall(nd);
					MotionSpin(nd, fr, (int32_t)v);
				}
				DrawJ(p, (int32_t)v);
			}
		}

		{
			uint32_t v = (uint32_t)(c - 0x179);
			if (v < 0x28)
			{
				if (v == 0)
				{
					OrbitReset(nd);
					w::PartReset(nd);
				}
				if (!Pause())
				{
					SpawnFloorParticles(nd);
					w::MatY(shl32(v, 6), 0x5800, 0x2000, fr + 0x90);
					F32(fr, 0xA8) = 0;
					F32(fr, 0xA4) = 0;
					F32(fr, 0xAC) = Ground();
					GteSetRotMatrix((Mat4x3 *)(fr + 0x90));
					GteSetTransVector((Mat4x3 *)(fr + 0x90));
					w::PartOrbit(nd, 0x28, 0x800);
					MotionOrbit(nd, (int32_t)v);
				}
				DrawK(p, (int32_t)v);
			}
		}

		{
			uint32_t v = (uint32_t)(c - 0x1A1);
			if (v < 0x17)
			{
				if (v == 0 && !Pause())
				{
					int16_t *q = (int16_t *)(nd + 0x42C);
					for (int k = 0; k < 0x80; k++, q += 12)
					{
						if (q[3] == 0) continue;
						q[3] += 0x28;
						q[4] >>= 2;
						q[5] >>= 2;
						q[6] >>= 2;
					}
				}
				w::PartBounce(nd);
			}
		}

		uint32_t vdrop = (uint32_t)(c - 0x1B8);
		if (vdrop < 0x28)
		{
			if (vdrop == 0) DropReset(nd);
			if (!Pause())
			{
				MotionDrop(nd, (int32_t)vdrop, true);
				w::PartBounce(nd);
			}
		}
		if (vdrop < 0x5C8)
		{
			for (int i = 0; i < Count(); i++)
			{
				uint8_t *rec = Rec(nd, i), *ent = Ent(nd, i);
				if (!rec[0x1C]) continue;
				if (*(uint8_t *)(At<uint32_t>(ent, 0x74) + 0x2C) & 0x40) continue;
				if (!(ent[0] & 2)) continue;
				w::Pivot(ent, rec + 4);
				int16_t y = (int16_t)-At<int16_t>(rec, 4);
				At<int16_t>(ent, 0x1E) = y;
				At<int32_t>(ent, 0x58) = y;
			}
		}

		w::PartDraw(nd);
		if (c >= 0xE0) DrawTargets(p);
		if (CreatureDrawn(c))
			FrameCursor() = w::DrawCreature(E(nd), 0x2000, (uint32_t)(MB() + 0x31D4), FrameCursor(), var<uint32_t>(game_addr(game::BATTLE_PACKET_BUFFER_END)));

		{
			uint32_t D = (uint32_t)(MB() + 0x2321C);
			switch ((uint16_t)c)
			{
			case 0:
				w::Load(0x2CF, D, 2);
				w::Load(0x2D0, (uint32_t)(MB() + 0x2321C), 1);
				w::Load(0x2D1, (uint32_t)(MB() + 0x2321C), 3);
				w::Load(0x2D2, (uint32_t)(MB() + 0x31220), 5);
				break;
			case 0x63:
				if (w::LoadBusy()) return 0;
				w::Load(0x2D3, D, 3);
				w::Load(0x2D4, (uint32_t)(MB() + 0x23220), 5);
				break;
			case 0xB7:
				if (w::LoadBusy()) return 0;
				break;
			case 0xE0:
				w::Load(0x2D5, D, 3);
				w::Load(0x2D6, (uint32_t)(MB() + 0x23220), 5);
				break;
			case 0xFD:
				if (w::LoadBusy()) return 0;
				break;
			case 0x1E0:
				if (QLand().head) return 0;
				w::Load(0x168, (uint32_t)(MB() + 0x31220), 1);
				w::Load(0x169, (uint32_t)(MB() + 0x31220), 1);
				w::Load(0x16A, (uint32_t)(MB() + 0x31220), 1);
				LoadState() = 0;
				break;
			default: break;
			}
		}
		w::LoadPump();
		switch (LoadState())
		{
		case 0:
			w::LoadFile(0x168, (uint32_t)(MB() + 0x31220), 0, 0);
			w::TimUpload((uint32_t)(MB() + 0x31220));
			LoadState() = 1;
			break;
		case 1:
			w::LoadFile(0x169, (uint32_t)(MB() + 0x31220), 0, 0);
			w::TimUpload((uint32_t)(MB() + 0x31220));
			LoadState() = 2;
			break;
		case 2:
			w::LoadFile(0x16A, (uint32_t)(MB() + 0x31220), 0, 0);
			w::TimUpload((uint32_t)(MB() + 0x31220));
			LoadState() = 3;
			break;
		default: break;
		}
		if (c >= 0x82 && c < 0x179 && (c < 0xE0 || c >= 0xFE)) w::Advance(E(nd));

		CamStep(nd, c, true);

		At<int16_t>(nd, 0xC) = (int16_t)(At<int16_t>(nd, 0xC) + 1);
		if (At<int16_t>(nd, 0xC) <= 0x1E0) return 0;
		Flash() = 0;
		if (At<int16_t>(nd, 0xC) < 0x1FE)
		{
			int cnt = Count();
			for (int i = 0; i < cnt; i++)
				if (!(*(uint8_t *)(At<uint32_t>(Ent(nd, i), 0x74) + 0x2C) & 0x40)) return 0;
		}
		if (!w::PreLoad1(2))
		{
			At<int16_t>(nd, 0xC) = 0x208;
			return 0;
		}
		for (int i = 0; i < Count(); i++)
		{
			uint8_t *rec = Rec(nd, i), *ent = Ent(nd, i);
			At<int16_t>(ent, 0x1C) = At<int16_t>(rec, 0xC);
			At<int16_t>(ent, 0x1E) = At<int16_t>(rec, 0xE);
			At<int16_t>(ent, 0x20) = At<int16_t>(rec, 0x10);
		}
		return TASK_END;
	}

	static void TargetPrimCtx(uint8_t *ctx, uint8_t *rec, const int16_t *pos)
	{
		uint8_t *ent = At<uint8_t *>(rec, 0);
		At<int32_t>(ctx, 0x14) = pos[0];
		At<int32_t>(ctx, 0x18) = (int32_t)pos[1] + At<int16_t>(rec, 4);
		At<int32_t>(ctx, 0x1C) = pos[2];
		w::Scale3(0, (int16_t)(At<int16_t>(ent, 0x26) + 0x400), ctx);
		ComposeAffineTransform(&Camera(), (Mat4x3 *)ctx, (Mat4x3 *)ctx);
		At<int32_t>(ctx, 0x24) = 0;
		At<int16_t>(ctx, 0x20) = -128;
		At<uint32_t>(ctx, 0x28) = (uint32_t)(MB() + 0x31D4);
	}

	static uint32_t __cdecl TargetPrimTask(TaskNode *n)
	{
		uint8_t *nd = (uint8_t *)n;
		uint8_t *rec = At<uint8_t *>(nd, 0xC);
		uint8_t ctx[0x2C];
		TargetPrimCtx(ctx, rec, &At<int16_t>(At<uint8_t *>(rec, 0), 0x1C));
		int left = prim::play((prim::Layout *)(nd + 0x10), (prim::Callback)game_addr(CB_Prim), (int)ctx, Pause());
		return left != 0 ? 0 : TASK_END;
	}

	static void SwirlDraw(uint8_t *nd, int16_t angle, int16_t frame)
	{
		int32_t i = angle & 0xFFF;
		int32_t r = At<int16_t>(nd, 0xE);
		int16_t pos[3];
		pos[0] = (int16_t)(mul32(SinT(i), r) >> 12);
		pos[1] = 0;
		pos[2] = (int16_t)((int16_t)(mul32(CosT(i), r) >> 12) + At<int16_t>(nd, 0x14));
		TransformCameraByShadowRotation(pos, At<int16_t>(nd, 0x16), -(At<int16_t>(nd, 0x18) >> 2));
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		*(uint32_t *)h = game_addr(game::unk_13AFCC0);
		*(int16_t *)(h + 4) = frame;
		*(int16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static uint32_t __cdecl SwirlFlipTask(TaskNode *n)
	{
		uint8_t *nd = (uint8_t *)n;
		SwirlDraw(nd, At<int16_t>(nd, 0x10), At<int16_t>(nd, 0xC));
		if (Pause()) return 0;
		At<int16_t>(nd, 0xC)++;
		if (At<int16_t>(nd, 0xC) >= 0xC) return TASK_END;
		At<int16_t>(nd, 0x10) += At<int16_t>(nd, 0x12);
		return 0;
	}

	static void RiseDraw(uint8_t *nd, const int16_t *pos, int16_t frame)
	{
		TransformCameraByShadowRotation(pos, At<int16_t>(nd, 0x18), -(At<int16_t>(nd, 0x1A) >> 2));
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		*(uint32_t *)h = game_addr(game::unk_13AFE04);
		*(int16_t *)(h + 4) = frame;
		*(int16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static uint32_t __cdecl RiseFlipTask(TaskNode *n)
	{
		uint8_t *nd = (uint8_t *)n;
		RiseDraw(nd, &At<int16_t>(nd, 0xC), At<int16_t>(nd, 0x12));
		if (Pause()) return 0;
		At<int16_t>(nd, 0x12)++;
		if (At<int16_t>(nd, 0x12) >= 0xC) return TASK_END;
		for (int k = 0; k < 3; k++) At<int16_t>(nd, 0xC + 2 * k) += (int16_t)shl32((int8_t)nd[0x14 + k], 4);
		return 0;
	}

	static int16_t g_trail_pts[8];

	static void TrailMatrix(uint8_t *m, int16_t spin, int16_t scale)
	{
		w::MatY(spin, scale, (int16_t)((scale >> 2) + 0xC00), m);
		At<int32_t>(m, 0x18) = 0;
		At<int32_t>(m, 0x14) = 0;
		At<int32_t>(m, 0x1C) = Center();
		ComposeAffineTransform(&Camera(), (Mat4x3 *)m, (Mat4x3 *)m);
		GteSetRotMatrix((Mat4x3 *)m);
		GteSetTransVector((Mat4x3 *)m);
	}

	static uint32_t __cdecl TrailTask(TaskNode *n)
	{
		uint8_t *nd = (uint8_t *)n;
		int16_t age = At<int16_t>(nd, 0xC);
		if (age <= 0x14 && !Pause())
		{
			int32_t t = (int32_t)(((int64_t)shl32(age, 12) * 0x66666667) >> 32) >> 3;
			t += (int32_t)((uint32_t)t >> 31);
			int32_t k = (ComputeSin(t + t) >> 3) + 0x1000;
			int32_t s = mul32(ComputeSin(At<int16_t>(nd, 0x12)), k) >> 14;
			g_trail_pts[0] = (int16_t)s;
			g_trail_pts[4] = (int16_t)s;
			int32_t cz = mul32(w::Cos(At<int16_t>(nd, 0x12)), k);
			At<int16_t>(nd, 0x12) += At<int16_t>(nd, 0x14);
			cz >>= 14;
			g_trail_pts[2] = (int16_t)cz;
			g_trail_pts[6] = (int16_t)cz;
			g_trail_pts[1] = (int16_t)(At<int16_t>(nd, 0x10) + (int16_t)t);
			int16_t y2;
			if (age < 8) y2 = (int16_t)(g_trail_pts[1] - (shl32(age, 9) >> 3) - 0x20);
			else if (age > 0xC) y2 = (int16_t)(g_trail_pts[1] - (shl32(0x14 - age, 9) >> 3) - 0x20);
			else y2 = (int16_t)(g_trail_pts[1] - 0x200);
			g_trail_pts[5] = y2;
			At<int32_t>(nd, 0x20) = w::TrailPush(0x1E, At<int32_t>(nd, 0x20), nd + 0x24, g_trail_pts + 4, g_trail_pts, 0x10, 0x58, (3 - (age & 3)) << 5);
		}
		uint8_t mat[0x20];
		TrailMatrix(mat, At<int16_t>(nd, 0x16), At<int16_t>(nd, 0x1A));
		PacketCursor() = w::TrailDraw(&At<int32_t>(nd, 0x20), nd + 0x24, -4, RenderOT(), 2, PacketCursor());
		if (Pause()) return 0;
		At<int16_t>(nd, 0x16) += At<int16_t>(nd, 0x18);
		int16_t a = At<int16_t>(nd, 0xC);
		if (a >= 0x14)
		{
			if (a == 0x14)
			{
				At<int16_t>(nd, 0x1A) = 0x1000;
				At<int16_t>(nd, 0x1C) = 0x400;
				At<int16_t>(nd, 0x1E) = (int16_t)0xFFC0;
			}
			int16_t d = At<int16_t>(nd, 0x1C);
			At<int16_t>(nd, 0x1A) += d;
			int16_t d2 = (int16_t)(At<int16_t>(nd, 0x1E) + d);
			At<int16_t>(nd, 0x1C) = d2;
			if (d2 < 0x40) At<int16_t>(nd, 0x1C) = 0x40;
		}
		int16_t na = (int16_t)(a + 1);
		At<int16_t>(nd, 0xC) = na;
		if (na > 8 && At<int32_t>(nd, 0x20) < 2) return TASK_END;
		return 0;
	}
}

	void register_mag291_pandemona()
	{
		register_port(p291::ORIG_SequenceTask, (void *)p291::SequenceTask, "P291 SequenceTask", 291);
		register_port(p291::ORIG_TimelineTask, (void *)p291::TimelineTask, "P291 TimelineTask", 291);
		register_port(p291::ORIG_LandPrimTask, (void *)p291::TargetPrimTask, "P291 LandPrimTask", 291);
		register_port(p291::ORIG_FallPrimTask, (void *)p291::TargetPrimTask, "P291 FallPrimTask", 291);
		register_port(p291::ORIG_SwirlFlipTask, (void *)p291::SwirlFlipTask, "P291 SwirlFlipTask", 291);
		register_port(p291::ORIG_RiseFlipTask, (void *)p291::RiseFlipTask, "P291 RiseFlipTask", 291);
		register_port(p291::ORIG_TrailTask, (void *)p291::TrailTask, "P291 TrailTask", 291);
	}
}

