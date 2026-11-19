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

#include "fx_port.h"
#include <stddef.h>

namespace ff8fx
{
namespace s209
{
	using namespace eng;

	namespace x
	{
		inline int32_t CrtRand() { return fn<int32_t (__cdecl *)()>(game_addr(game::_rand))(); }
		inline int32_t ComputeCos(int32_t a) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(a); }
		inline int32_t Sqrt(int32_t v) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::Math_SqrtInt))(v); }
		inline void ComposeZYXRotationMatrix(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::ComposeZYXRotationMatrix))(angles, out); }
		inline void Scale3DMatrix(Mat4x3 *m, const int32_t *v) { fn<void (__cdecl *)(Mat4x3 *, const int32_t *)>(game_addr(game::scale3DMatrix))(m, v); }
		inline int32_t GetEffectSpawnPosition(void *entity, int32_t bone, int32_t frac, int16_t *out) { return fn<int32_t (__cdecl *)(void *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, frac, out); }
		inline void GetDefaultEffectPosition(void *entity, int16_t *out) { fn<void (__cdecl *)(void *, int16_t *)>(game_addr(game::GetDefaultEffectPosition))(entity, out); }
		inline void SetProjection(int32_t h) { fn<void (__cdecl *)(int32_t)>(game_addr(game::Battle_SetProjectionH))(h); }
		inline void SetScreenFlash2(int32_t level, int32_t mode) { fn<void (__cdecl *)(int32_t, int32_t)>(game_addr(game::Effect_SetScreenFlash))(level, mode); }
		inline void ScreenFade(int32_t a, int32_t b, int32_t c, int32_t d) { fn<void (__cdecl *)(int32_t, int32_t, int32_t, int32_t)>(game_addr(game::Effect_SpawnScreenFadeTask))(a, b, c, d); }
		inline uint32_t RenderGeometry(void *geom, void *hdr, uint32_t ot, int32_t shift, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, void *, uint32_t, int32_t, uint32_t)>(game_addr(game::RenderGeometry))(geom, hdr, ot, shift, cursor); }
		inline void BdPlaySE(uint32_t se, int32_t a, int32_t b) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t)>(game_addr(game::BdPlaySE))(se, a, b); }
		inline uint32_t ClaimVoiceSlot2(uint32_t sound, int32_t a, int32_t b) { return fn<uint32_t (__cdecl *)(uint32_t, int32_t, int32_t)>(game_addr(game::BdSound_ClaimVoiceSlot))(sound, a, b); }
		inline int32_t VoiceSlotBusy(uint32_t slot) { return fn<int32_t (__cdecl *)(uint32_t)>(game_addr(game::BdSound_IsVoiceSlotActive))(slot); }
		inline void ReleaseVoiceSlot(uint32_t slot) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::BdSound_ReleaseVoiceSlot))(slot); }
		inline int32_t LoadState() { return fn<int32_t (__cdecl *)()>(game_addr(game::BattleFile_GetLoadingState))(); }
		inline uint32_t SummonData() { return fn<uint32_t (__cdecl *)()>(game_addr(game::sub_571B70))(); }
		inline void WitchGetNewStage(uint32_t dst) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::BS_WitchFight_GetNewStage))(dst); }
		inline void ChangeStage(uint32_t src) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::BS_ChangeStage))(src); }
		inline void ApplyActionResultToTarget(uint32_t rec) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::ApplyActionResultToTarget))(rec); }
		inline void RestoreTexture(uint32_t buffer, void *flag) { fn<void (__cdecl *)(uint32_t, void *)>(game_addr(game::Battle_RestoreDefaultEffectTexture))(buffer, flag); }
		inline void ClearImage(const int16_t *rect, int32_t r, int32_t g, int32_t b) { fn<void (__cdecl *)(const int16_t *, int32_t, int32_t, int32_t)>(game_addr(game::isDravingOverlayImage))(rect, r, g, b); }
		inline void GetDrawEnv(void *env) { fn<void (__cdecl *)(void *)>(game_addr(game::GPU_GetDrawEnv))(env); }
		inline void SetDefDrawEnv(void *env, int32_t x, int32_t y, int32_t w, int32_t h) { fn<void (__cdecl *)(void *, int32_t, int32_t, int32_t, int32_t)>(game_addr(game::GPU_SetDefDrawEnv))(env, x, y, w, h); }
		inline void SetDrawEnvPacket(void *packet, const void *env) { fn<void (__cdecl *)(void *, const void *)>(game_addr(game::SSIGPU_SetDrawEnv))(packet, env); }
		inline void InsertPrimAltViewport(uint32_t bucket, void *packet) { fn<void (__cdecl *)(uint32_t, void *)>(game_addr(game::SSIGPU_InsertPrimAltViewport))(bucket, packet); }
		inline void GteSetFarColor(int32_t r, int32_t g, int32_t b) { fn<void (__cdecl *)(int32_t, int32_t, int32_t)>(game_addr(game::GTE_SetFarColor))(r, g, b); }
		inline void GteLoadRGBC(const void *c) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadRGBC))(c); }
		inline void GteDPCS() { fn<void (__cdecl *)()>(game_addr(game::GTE_DPCS))(); }
		inline void GteStoreRGB2(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreRGB2))(dst); }
		inline uint32_t PrimModel2(void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::MAG_166_MegiddoFlame_ColumnRender))(h, ot, mode, cursor); }
		inline uint32_t ScrollPrimModel(void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::MAG_066_RenderScrollPrimModel))(h, ot, mode, cursor); }
	}
	using namespace x;

	inline bool Paused() { return (var<uint32_t>(game_addr(game::battle_to_update_flags_dword_1D96A9C)) & 0x201) != 0; }
	inline TaskQueue &RootQueue() { return var<TaskQueue>(game_addr(game::unk_24F3640)); }
	inline TaskQueue &SubQueue() { return var<TaskQueue>(game_addr(game::unk_24F3658)); }
	inline uint8_t *CastCtx() { return var<uint8_t *>(game_addr(game::unk_24F4478)); }
	inline uint32_t CasterSlot() { return var<uint32_t>(game_addr(game::unk_24F4480)); }
	inline uint8_t *Caster() { return (uint8_t *)(game_addr(game::BattleEntitySlotData) + 156 * CasterSlot()); }
	inline uint32_t &Cursor() { return var<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)); }
	inline uint32_t OTBase() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)); }
	static constexpr game::Id GRID = game::unk_24F4488;
	static constexpr game::Id GRID_END = game::unk_24F7A58;
	inline uint8_t *Cells() { return var<uint8_t *>(game_addr(game::unk_24F4484)); }
	inline uint32_t &ColA() { return var<uint32_t>(game_addr(game::unk_24F7A58)); }
	inline uint32_t &ColB() { return var<uint32_t>(game_addr(game::unk_24F3650)); }
	inline uint32_t &StageColA() { return var<uint32_t>(game_addr(game::unk_24F3618)); }
	inline uint32_t &StageColB() { return var<uint32_t>(game_addr(game::unk_24F447C)); }

	static constexpr game::Id ORIG_Master = game::MAG_209_sub_639100;
	static constexpr game::Id ORIG_EndTask = game::MAG_209_sub_639170;
	static constexpr game::Id ORIG_TimelineA = game::MAG_209_sub_6375D0;
	static constexpr game::Id ORIG_TimelineB = game::MAG_209_sub_638BD0;
	static constexpr game::Id ORIG_ScreenTask = game::MAG_209_sub_637A40;
	static constexpr game::Id ORIG_PrimATask = game::MAG_209_sub_638490;
	static constexpr game::Id ORIG_PrimBTask = game::MAG_209_sub_6385E0;
	static constexpr game::Id ORIG_PrimCTask = game::MAG_209_sub_638770;
	static constexpr game::Id ORIG_PrimDTask = game::MAG_209_sub_638900;
	static constexpr game::Id ORIG_PrimETask = game::MAG_209_sub_638A80;

#pragma pack(push, 1)
	struct Node24
	{
		TaskNode hdr;
		int16_t c;
		int16_t e;
		int16_t p10, p12, p14, p16;
		int16_t a18, s1A;
		int16_t s1C, s1E;
		int16_t s20, s22;
	};
#pragma pack(pop)
	static_assert(sizeof(Node24) == 0x24, "sub-queue node is 0x24 bytes");
}
}

namespace ff8fx
{
namespace s209
{
	static uint32_t __cdecl Master(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		uint32_t arena = var<uint32_t>(game_addr(game::unk_24F7A5C));
		var<uint32_t>(game_addr(game::unk_24F7A60)) = (*(const uint8_t *)&n->c & 1) ? arena + 0x10000 : arena;
		int left = ExecuteTaskQueue(&SubQueue());
		if (!left)
		{
			Node24 *e = (Node24 *)AddTaskToQueue(&RootQueue(), game_addr(ORIG_EndTask));
			e->c = 0;
			e->e = 0;
		}
		n->c++;
		return left ? 0 : TASK_END;
	}

	static uint32_t __cdecl EndTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		if (n->c == 1) RestoreTexture(var<uint32_t>(game_addr(game::unk_24F7A5C)), &n->e);
		int16_t done = n->e;
		n->c++;
		return done ? TASK_END : 0;
	}

	static void HideStage() { for (uint32_t a = game_addr(game::unk_1D98991); a < game_addr(game::unk_1D98A41); a += 0x2C) var<uint8_t>(a) |= 2; }
	static void ShowStage() { for (uint32_t a = game_addr(game::unk_1D98991); a < game_addr(game::unk_1D98A41); a += 0x2C) var<uint8_t>(a) &= 0xFD; }

	static void SetAmbient(uint32_t c)
	{
		var<uint32_t>(game_addr(game::CURRENT_BS_SHADOW_PALETTE)) = c;
		for (uint32_t a = game_addr(game::unk_1D972EC); a < game_addr(game::unk_1D97730); a += 0x9C)
			if (var<uint8_t>(a - 0x2C) & 2) var<uint32_t>(a) = c;
	}

	static void ResetRotation(void *m)
	{
		uint32_t *d = (uint32_t *)m;
		d[3] = 0;
		d[2] = 0;
		d[1] = 0;
		d[0] = 0;
		uint16_t *w = (uint16_t *)m;
		w[8] = 0x1000;
		w[4] = 0x1000;
		w[0] = 0x1000;
	}

	static void DrawEnvAt(int32_t x, int32_t y, int32_t ot)
	{
		uint8_t env[0x5C], cur[0x5C];
		GetDrawEnv(cur);
		uint8_t *pk = (uint8_t *)Cursor();
		Cursor() = (uint32_t)pk + 0x40;
		SetDefDrawEnv(env, x, y, 0x140, 0xD8);
		*(uint16_t *)(env + 0x14) = *(const uint16_t *)(cur + 0x14);
		env[0x17] = cur[0x17];
		env[0x16] = cur[0x16];
		env[0x18] = 1;
		env[0x1B] = 0;
		env[0x1A] = 0;
		env[0x19] = 0;
		SetDrawEnvPacket(pk, env);
		InsertPrimAutoDepth(OTBase() + 4 * (uint32_t)ot, pk);
	}

	static void StageToVram()
	{
		int16_t rect[4] = { 0x240, 0x100, 0x140, 0xD8 };
		ClearImage(rect, 0, 0, 0);
		int32_t lo = 0x1121;
		uint8_t *h = (uint8_t *)FieldAlloc(0x2C);
		*(int16_t *)(h + 0x14) = 0;
		*(int16_t *)(h + 0x16) = 0;
		*(int16_t *)(h + 0x1A) = 0xD8;
		*(uint32_t *)(h + 0x10) = var<uint32_t>(game_addr(game::BATTLE_PACKET_BUFFER_END));
		*(int16_t *)(h + 0x18) = 0x140;
		*(uint32_t *)(h + 4) = var<uint32_t>(game_addr(game::g_BattleVertexWorkspace));
		*(uint32_t *)(h + 0x20) = 0xFFFFFFFF;
		for (int i = 0; i < 4; i++)
		{
			if (!(var<uint8_t>(game_addr(game::unk_1D98991) + 0x2C * i) & 1)) continue;
			int32_t o = var<int32_t>(game_addr(game::unk_E2B4F8) + 8 * i);
			if (o < lo) lo = o;
		}
		DrawEnvAt(0, 0, lo);
		for (int i = 0; i < 4; i++)
		{
			uint8_t *p = (uint8_t *)(game_addr(game::unk_1D989A4) + 0x2C * i);
			const int32_t *tb = (const int32_t *)(game_addr(game::unk_E2B4F8) + 8 * i);
			BuildBoneMatricesFromPose(p);
			ComputeBonesWorldMatrices(p, &Camera());
			if (!(p[-0x13] & 1)) continue;
			*(uint32_t *)(h + 0x1C) = *(const uint32_t *)(p + 0x14);
			*(uint16_t *)(h + 0x24) = *(const uint16_t *)(p - 0x12);
			Cursor() = RenderGeometry(p - 0x10, h, OTBase() + 4 * (uint32_t)tb[0], tb[1], Cursor());
			if (tb[0] < lo) lo = tb[0];
		}
		DrawEnvAt(0x240, 0x100, 0x1121);
		FieldFree(0x2C);
	}

	static void Tiles(uint32_t c)
	{
		uint8_t *p = (uint8_t *)Cursor();
		for (int32_t i = 0; i < 10; i++)
		{
			const uint8_t u = (uint8_t)((i & 1) << 5);
			for (int32_t j = 0; j < 7; j++)
			{
				p[0xC] = u;
				*(uint16_t *)(p + 0x12) = 0x20;
				*(uint16_t *)(p + 0x10) = 0x20;
				*(uint32_t *)p = 0x4000000;
				*(uint32_t *)(p + 4) = c;
				*(int16_t *)(p + 8) = (int16_t)(i << 5);
				*(int16_t *)(p + 0xA) = (int16_t)(j << 5);
				p[0xD] = (uint8_t)(j << 5);
				InsertPrimAltViewport(OTBase() + 0x4068, p);
				p += 0x14;
			}
			*(uint32_t *)p = 0x1000000;
			*(uint32_t *)(p + 4) = (((uint32_t)(i / 2 + 0x139)) & 0x9FF) | 0xE1000200;
			InsertPrimAutoDepth(OTBase() + 0x4068, p);
			p += 8;
		}
		Cursor() = (uint32_t)p;
	}

	static void GridInit()
	{
		uint8_t *g = (uint8_t *)game_addr(GRID);
		for (int32_t row = 0; row < 0x1C; row++)
			for (int32_t col = 0; col < 0x29; col++, g += 0xC)
			{
				*(int16_t *)(g + 4) = (int16_t)(-0xA4 + 8 * col);
				*(int16_t *)(g + 6) = (int16_t)(-0x70 + 8 * row);
				*(int16_t *)(g + 8) = 0;
				*(int16_t *)(g + 0xA) = 0;
				if (row == 0 || row == 0x1B) *(int16_t *)(g + 0xA) = 1;
				if (col == 0 || col == 0x28) *(uint16_t *)(g + 0xA) |= 2;
			}
		uint8_t *cell = Cells();
		uint32_t pt = game_addr(GRID);
		int32_t v0 = 0;
		for (int32_t row = 0; row < 0x1B; row++)
		{
			const int32_t v1 = v0 + 8;
			int32_t u = 0;
			for (int32_t col = 0; col < 0x28; col++, cell += 0x60)
			{
				uint8_t *q = cell + 0x2C;
				*(uint32_t *)(cell + 8) = pt + 0x1EC;
				*(uint32_t *)cell = pt;
				*(uint32_t *)(cell + 0xC) = pt + 0x1F8;
				*(uint32_t *)(cell + 4) = pt + 0xC;
				pt += 0xC;
				const int32_t um = u % 64;
				const int32_t u_old = u;
				u += 8;
				const uint16_t tpage = (uint16_t)((u_old / 64 + 0x19) | 0x120);
				*(uint32_t *)(q - 0x1C) = 0x9000000;
				*(uint32_t *)(q - 0x18) = 0x2C808080;
				q[0] = (uint8_t)um;
				q[-0x10] = (uint8_t)um;
				const int32_t ur = u >= 0x140 ? um + 7 : um + 8;
				q[8] = (uint8_t)ur;
				q[-8] = (uint8_t)ur;
				const int32_t vb = v1 >= 0xD8 ? v1 - 1 : v1;
				q[-7] = (uint8_t)v0;
				q[-0xF] = (uint8_t)v0;
				q[9] = (uint8_t)vb;
				q[1] = (uint8_t)vb;
				*(uint16_t *)(q - 6) = tpage;
				*(uint32_t *)(q + 0xC) = 0x9000000;
				*(uint32_t *)(q + 0x10) = 0x2C808080;
				q[0x28] = (uint8_t)um;
				q[0x18] = (uint8_t)um;
				q[0x21] = (uint8_t)v0;
				q[0x19] = (uint8_t)v0;
				q[0x30] = (uint8_t)ur;
				q[0x20] = (uint8_t)ur;
				*(uint16_t *)(q + 0x22) = tpage;
				q[0x31] = (uint8_t)vb;
				q[0x29] = (uint8_t)vb;
			}
			pt += 0xC;
			v0 = v1;
		}
	}

	static void GridRipple(int32_t i, int32_t n)
	{
		int16_t *s = (int16_t *)FieldAlloc(0xC);
		GetEffectSpawnPosition(Caster(), 0xF1, 0, s);
		GteSetRotMatrixCtrl(&Camera());
		GteSetTransVectorCtrl(&Camera());
		GteLoadV0(s);
		GteRTPS();
		GteReadSXY2(s + 4);
		s[4] = (int16_t)((int16_t)(s[4] >> 3) - 0xA4);
		s[5] = (int16_t)((int16_t)(s[5] >> 3) - 0x70);
		const int32_t phase = 385 * i;
		const int32_t amp = (500 * ComputeSin(shl32(i, 11) / n)) >> 12;
		const int32_t range = 0x258 - 9 * i;
		uint8_t *g = (uint8_t *)game_addr(GRID);
		for (int32_t k = 0; k < 0x1C * 0x29; k++, g += 0xC)
		{
			if (*(uint8_t *)(g + 0xA) & 3) continue;
			const int32_t dx = *(int16_t *)(g + 4) - s[4];
			const int32_t dy = *(int16_t *)(g + 6) - s[5];
			const int32_t d = Sqrt(dy * dy + dx * dx);
			const int32_t c = ComputeCos(96 * d - phase) - 0x1000;
			const int32_t f = (range - d) * amp / range;
			*(int16_t *)(g + 8) = (int16_t)((c * f) >> 13);
		}
		FieldFree(0xC);
	}

	static void GridDraw(int32_t parity, uint32_t c)
	{
		Mat4x3 m;
		ResetRotation(&m);
		m.t[0] = 4;
		m.t[1] = 4;
		m.t[2] = 0x800;
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		SetProjection(0x800);
		for (uint32_t g = game_addr(GRID); g < game_addr(GRID_END); g += 0xC)
		{
			GteLoadV0((const void *)(g + 4));
			GteRTPS();
			GteReadSXY2((void *)g);
			*(int16_t *)g = (int16_t)(*(int16_t *)g >> 3);
			*(int16_t *)(g + 2) = (int16_t)(*(int16_t *)(g + 2) >> 3);
		}
		SetProjection(var<int16_t>(game_addr(game::BATTLE_PROJECTION_DISTANCE)));
		const uint32_t col = c | 0x2C000000;
		uint8_t *cell = Cells();
		for (int32_t k = 0; k < 0x1B * 0x28; k++, cell += 0x60)
		{
			uint32_t *q = (uint32_t *)(cell + 0x18 + 0x28 * parity);
			const uint32_t *const *pt = (const uint32_t *const *)cell;
			q[-1] = col;
			q[0] = *pt[0];
			q[2] = *pt[1];
			q[4] = *pt[2];
			q[6] = *pt[3];
			InsertPrimAltViewport(OTBase() + 0x4068, q - 2);
		}
	}

	static uint32_t ScreenColour(int32_t c) { return c < 8 ? var<uint32_t>(game_addr(game::unk_E2B430) + 4 * c) : var<uint32_t>(game_addr(game::unk_E2B450) + 4 * ((c - 8) % 24)); }

	static void ScreenDraw(int32_t c)
	{
		uint32_t col = ScreenColour(c);
		uint32_t bg, amb;
		bool stage = true;
		if (c < 0x24)
		{
			Tiles(col | 0x64000000);
			bg = ColA();
			amb = ColB();
		}
		else if (c < 0x26)
		{
			Tiles(col | 0x64000000);
			bg = ColA();
			amb = ColB();
			SetScreenFlash2(0x1000, 0);
			stage = false;
		}
		else if (c < 0x52)
		{
			const int32_t i = c - 0x26;
			if (i == 0) GridInit();
			bg = ColA();
			amb = ColB();
			GridRipple(i, 0x3C);
			GridDraw(var<uint8_t>(game_addr(game::g_Battle_DisplayParity)), col);
			SetScreenFlash2(0x1000, 0);
			stage = false;
		}
		else if (c < 0x62)
		{
			const int32_t k = c - 0x52;
			const int32_t w = shl32(k, 12) / 16;
			uint32_t gcol = col | 0x2000000, rgbc;
			GteSetFarColor(0, 0, 0);
			rgbc = gcol;
			GteLoadRGBC(&rgbc);
			GteSetIR0(w);
			GteDPCS();
			GteStoreRGB2(&gcol);
			GteSetFarColor(var<uint8_t>(game_addr(game::unk_24F3618)), var<uint8_t>(game_addr(game::unk_24F3619)), var<uint8_t>(game_addr(game::unk_24F361A)));
			GteLoadRGBC(&ColA());
			GteDPCS();
			GteStoreRGB2(&bg);
			GteSetFarColor(var<uint8_t>(game_addr(game::unk_24F447C)), var<uint8_t>(game_addr(game::unk_24F447D)), var<uint8_t>(game_addr(game::unk_24F447E)));
			GteLoadRGBC(&ColB());
			GteDPCS();
			GteStoreRGB2(&amb);
			GridRipple(c - 0x26, 0x3C);
			GridDraw(var<uint8_t>(game_addr(game::g_Battle_DisplayParity)), gcol);
			SetScreenFlash2(0x1000 - ((5 * w * 512) >> 12), 0);
		}
		else
		{
			bg = StageColA();
			amb = StageColB();
			SetScreenFlash2(0x600, 0);
		}
		if (stage) StageToVram();
		var<uint32_t>(game_addr(game::CURRENT_BS_MODEL_PALETTE)) = bg;
		SetAmbient(amb);
	}

	static uint32_t __cdecl ScreenTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		if (n->c < 0x64)
		{
			ScreenDraw(n->c);
		}
		if (Paused()) return 0;
		if (n->c == 0) ShowStage();
		if (n->c == 0x64) HideStage();
		n->c++;
		return n->c > 0x64 ? TASK_END : 0;
	}

	static void PrimMatrix(const Node24 *n, int16_t ay, int32_t sx, int32_t sy, int32_t ty)
	{
		int16_t v[4] = { 0, ay, 0, 0 };
		Mat4x3 m;
		ComposeZYXRotationMatrix(v, &m);
		m.t[0] = n->p10;
		m.t[1] = ty;
		int32_t sc[3] = { sx, sy, sx };
		m.t[2] = n->p14;
		Scale3DMatrix(&m, sc);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
	}

	static void PrimADraw(const Node24 *n, int32_t s)
	{
		PrimMatrix(n, 0, s, s, n->p12);
		uint8_t *h = (uint8_t *)FieldAlloc(0x60);
		*(uint32_t *)h = game_addr(game::unk_E288F0);
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0x1C) = 0x33;
		*(uint32_t *)(h + 0x58) = 0xE1000240;
		*(uint32_t *)(h + 0x5C) = 0xE1000220;
		if (n->c >= 0x66)
		{
			*(uint32_t *)(h + 0x1C) = 0xF3;
			*(int32_t *)(h + 0xC) = shl32(n->c - 0x66, 9);
		}
		Cursor() = PrimModel2(h, OTBase() + 0x44, 2, Cursor());
		FieldFree(0x60);
	}

	static uint32_t __cdecl PrimATask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		PrimADraw(n, n->s1C);
		if (Paused()) return 0;
		int16_t c = n->c;
		if (c < 0x10)
		{
			int16_t v = n->s1E;
			n->s1C = (int16_t)(n->s1C + v);
			n->s1E = (int16_t)(v - (int16_t)((int32_t)v / 8));
		}
		n->c = (int16_t)(c + 1);
		return n->c >= 0x6E ? TASK_END : 0;
	}

	static int32_t PrimBEnd(const Node24 *n) { return n->p16 ? 0x37 : 0x6E; }

	static void PrimBDraw(const Node24 *n, int16_t a, int32_t sx, int32_t sy)
	{
		PrimMatrix(n, a, sx, sy, n->p12);
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		*(uint32_t *)(h + 8) = 0;
		const int32_t end = PrimBEnd(n);
		*(uint32_t *)h = var<uint32_t>(game_addr(game::unk_E2B400) + 4 * (int32_t)n->e);
		*(uint32_t *)(h + 0x1C) = 0x33;
		const int32_t c = n->c;
		if (c >= end - 8)
		{
			*(uint32_t *)(h + 0x1C) = 0xF3;
			*(int32_t *)(h + 0xC) = shl32(c - (end - 8), 9);
		}
		Cursor() = RenderPrimModel(h, OTBase() + 0x44, 2, Cursor());
		FieldFree(0x58);
	}

	static uint32_t __cdecl PrimBTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		PrimBDraw(n, n->a18, n->s1C, n->s20);
		if (Paused()) return 0;
		const int32_t end = PrimBEnd(n);
		n->a18 = (int16_t)(n->a18 + n->s1A);
		int16_t c = n->c;
		if (c < 0x10)
		{
			int16_t v = n->s1E;
			n->s1C = (int16_t)(n->s1C + v);
			n->s1E = (int16_t)(v - (int16_t)((int32_t)v / 8));
		}
		if (c < 0x20)
		{
			int16_t v = n->s22;
			n->s20 = (int16_t)(n->s20 + v);
			n->s22 = (int16_t)(v - (int16_t)((int32_t)v / 8));
		}
		n->c = (int16_t)(c + 1);
		return n->c >= end ? TASK_END : 0;
	}

	static void PrimCDraw(const Node24 *n, int32_t c)
	{
		int16_t v[4] = { 0, 0, 0, 0 };
		Mat4x3 m;
		ComposeZYXRotationMatrix(v, &m);
		GetEffectSpawnPosition(Caster(), 0xF1, 0, v);
		m.t[0] = n->p10;
		m.t[1] = v[1];
		int32_t s = n->s1C;
		int32_t sc[3] = { s, s, s };
		m.t[2] = n->p14;
		Scale3DMatrix(&m, sc);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		uint8_t *h = (uint8_t *)FieldAlloc(0x90);
		*(uint32_t *)(h + 8) = 0;
		*(int16_t *)(h + 0x18) = 0;
		*(int16_t *)(h + 0x1A) = 0;
		*(uint32_t *)(h + 0x10) = 0;
		*(uint32_t *)h = game_addr(game::unk_E26340);
		*(uint32_t *)(h + 0xC) = 0x600;
		*(uint32_t *)(h + 0x20) = 0xF3;
		*(int32_t *)(h + 0x14) = c * 8;
		*(int16_t *)(h + 0x1C) = 0x40;
		*(int16_t *)(h + 0x1E) = 0x80;
		*(uint32_t *)(h + 0x24) = var<uint32_t>(game_addr(game::unk_E2B4B0) + 4 * (c % 0x12));
		if (c >= 0x66) *(int32_t *)(h + 0xC) = 320 * c - 0x7980;
		Cursor() = ScrollPrimModel(h, OTBase() + 0x44, 2, Cursor());
		FieldFree(0x90);
	}

	static uint32_t __cdecl PrimCTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		PrimCDraw(n, n->c);
		if (Paused()) return 0;
		n->c++;
		return n->c >= 0x6E ? TASK_END : 0;
	}

	static void PrimDDraw(const Node24 *n, int16_t a, int32_t s)
	{
		PrimMatrix(n, a, s, s, n->p12);
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		const int32_t c = n->c;
		*(uint32_t *)h = game_addr(game::unk_E277B8);
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0x1C) = 0x33;
		if (c < 2)
		{
			*(int32_t *)(h + 0xC) = shl32(2 - c, 11);
			*(uint32_t *)(h + 0x1C) = 0xF3;
		}
		else if (c >= 8)
		{
			*(int32_t *)(h + 0xC) = shl32(c - 8, 8);
			*(uint32_t *)(h + 0x1C) = 0xF3;
		}
		Cursor() = RenderPrimModel(h, OTBase() + 0x44, 2, Cursor());
		FieldFree(0x58);
	}

	static uint32_t __cdecl PrimDTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		PrimDDraw(n, n->a18, n->s1C);
		if (Paused()) return 0;
		int16_t v = n->s1E;
		n->a18 = (int16_t)(n->a18 + n->s1A);
		n->s1C = (int16_t)(n->s1C + v);
		n->s1E = (int16_t)(v - (int16_t)((int32_t)v / 16));
		v = n->s22;
		n->s20 = (int16_t)(n->s20 + v);
		n->c++;
		n->s22 = (int16_t)(v - (int16_t)((int32_t)v / 16));
		return n->c >= 0x18 ? TASK_END : 0;
	}

	static void PrimEDraw(const Node24 *n, int16_t a, int32_t s)
	{
		PrimMatrix(n, a, s, s, n->p12);
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		const int32_t c = n->c;
		*(uint32_t *)h = game_addr(game::unk_E290E0);
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0x1C) = 0x33;
		if (c >= 4)
		{
			*(uint32_t *)(h + 0x1C) = 0xF3;
			*(int32_t *)(h + 0xC) = shl32(c - 4, 8);
		}
		Cursor() = RenderPrimModel(h, OTBase() + 0x44, 2, Cursor());
		FieldFree(0x58);
	}

	static uint32_t __cdecl PrimETask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		PrimEDraw(n, n->a18, n->s1C);
		if (Paused()) return 0;
		int16_t a = n->s1A, v = n->s1E;
		n->a18 = (int16_t)(n->a18 + a);
		n->s1A = (int16_t)(a - (int16_t)(a >> 4));
		n->s1C = (int16_t)(n->s1C + v);
		n->c++;
		n->s1E = (int16_t)(v - (int16_t)((int32_t)v / 16));
		return n->c >= 0x14 ? TASK_END : 0;
	}

	static Node24 *Spawn(uint32_t fn_)
	{
		Node24 *n = (Node24 *)AddTaskToQueue(&SubQueue(), fn_);
		return n;
	}

	static void SpawnBursts(const Node24 *t, int16_t p16)
	{
		int32_t r = CrtRand();
		const int16_t a = (int16_t)(r % 4096);
		for (int16_t k = 0; k < 0xC; k++)
		{
			Node24 *n = Spawn(game_addr(ORIG_PrimBTask));
			*(uint32_t *)&n->p10 = *(const uint32_t *)&t->p10;
			*(uint32_t *)&n->p14 = *(const uint32_t *)&t->p14;
			n->c = 0;
			n->p16 = p16;
			n->e = k;
			n->a18 = a;
			n->s1A = 0x1E;
			n->s1E = 0x100;
			n->s1C = 0x100;
			r = CrtRand() % 1024 + 0xC00;
			n->s22 = (int16_t)(r / 6);
			n->s20 = (int16_t)(r / 6);
		}
	}

	static void SpawnD()
	{
		Node24 *n = Spawn(game_addr(ORIG_PrimDTask));
		n->c = 0;
		GetEffectSpawnPosition(Caster(), 0xF1, 0, &n->p10);
		n->p12 = (int16_t)(n->p12 + (int16_t)(CrtRand() % 0x258));
		n->a18 = (int16_t)(CrtRand() % 2048);
		n->s1A = (int16_t)(-0x28 - CrtRand() % 0x78);
		int32_t r = CrtRand() % 2048 + 0xE00;
		n->s1E = (int16_t)(r / 8);
		n->s1C = (int16_t)(r / 8);
		r = CrtRand() % 2048 + 0x600;
		n->s22 = (int16_t)(r / 8);
		n->s20 = (int16_t)(r / 8);
	}

	static void SpawnE(bool b)
	{
		Node24 *n = Spawn(game_addr(ORIG_PrimETask));
		n->c = 0;
		GetEffectSpawnPosition(Caster(), 0xF1, 0, &n->p10);
		n->p12 = 0;
		n->a18 = (int16_t)(CrtRand() % 4096);
		if (b) n->s1A = (int16_t)((CrtRand() % 0x28) * 2 + 0x64);
		else n->s1A = (int16_t)(CrtRand() % 0x28 + 0x32);
		int32_t r = CrtRand() % 1024 + 0x2000;
		if (b) r *= 3;
		n->s1E = (int16_t)(r / 12);
		n->s1C = (int16_t)(r / 12);
	}

	static void Flash(int16_t c)
	{
		if (c < 0x10) SetScreenFlash2(96 * c, 0);
		else if (c >= 0x7A) SetScreenFlash2(192 * (0x82 - c), 0);
	}

	static int32_t TimelineStart(Node24 *t, bool b)
	{
		const uint32_t f = var<uint32_t>(game_addr(game::battle_to_update_flags_dword_1D96A9C));
		if (f & 0x201)
		{
			if (f & 1) return -1;
			if (LoadState() < 0) return -1;
		}
		if (t->c == 0)
		{
			GetDefaultEffectPosition(Caster(), &t->p10);
			uint32_t a = var<uint32_t>(game_addr(game::CURRENT_BS_MODEL_PALETTE)), c = var<uint32_t>(game_addr(game::CURRENT_BS_SHADOW_PALETTE));
			t->p12 = (int16_t)(t->p12 + (int16_t)0xF8F8);
			ColA() = a;
			ColB() = c;
			if (b) var<uint32_t>(game_addr(game::unk_24F7A64)) = ClaimVoiceSlot2(game_addr(game::unk_E2B2C8), 1, 0x80);
		}
		if (t->c == 2) WitchGetNewStage(SummonData());
		if (t->c == 0x34)
		{
			if (LoadState() < 0) return -1;
			ChangeStage(SummonData());
			StageColA() = var<uint32_t>(game_addr(game::CURRENT_BS_MODEL_PALETTE));
			StageColB() = var<uint32_t>(game_addr(game::CURRENT_BS_SHADOW_PALETTE));
		}
		if (t->c == 0x10) Spawn(game_addr(ORIG_ScreenTask))->c = 0;
		if (t->c == 0) Caster()[0] |= 8;
		if (t->c == (b ? 0x41 : 0x14)) *(uint16_t *)Caster() &= 0xFFF7;
		if (t->c == 1)
		{
			Node24 *n = Spawn(game_addr(ORIG_PrimATask));
			*(uint32_t *)&n->p10 = *(const uint32_t *)&t->p10;
			*(uint32_t *)&n->p14 = *(const uint32_t *)&t->p14;
			n->c = 0;
			n->s1E = 0x3AA;
			n->s1C = 0x3AA;
		}
		return 0;
	}

	static uint32_t TimelineEnd(Node24 *t)
	{
		t->c++;
		if (t->c <= 0x82) return 0;
		SetScreenFlash2(0, 0);
		return TASK_END;
	}

	static uint32_t __cdecl TimelineA(TaskNode *tn)
	{
		Node24 *t = (Node24 *)tn;
		if (TimelineStart(t, false) < 0) return 0;
		if (t->c == 1)
		{
			SpawnBursts(t, 0);
			Node24 *n = Spawn(game_addr(ORIG_PrimCTask));
			*(uint32_t *)&n->p10 = *(const uint32_t *)&t->p10;
			n->c = 0;
			*(uint32_t *)&n->p14 = *(const uint32_t *)&t->p14;
			n->s1C = 0x1800;
		}
		int16_t c = t->c;
		if (c >= 0x1E && c <= 0x64 && (int32_t)c % 10 == 1) SpawnD();
		if (t->c == 0x73) SpawnE(false);
		Flash(t->c);
		if (t->c == 0x7C) ApplyActionResultToTarget(*(uint32_t *)(*(uint8_t **)(CastCtx() + 4) + 8));
		if (t->c == 1) BdPlaySE(game_addr(game::unk_E2B3F4), 0, 0x80);
		return TimelineEnd(t);
	}

	static uint32_t __cdecl TimelineB(TaskNode *tn)
	{
		Node24 *t = (Node24 *)tn;
		if (TimelineStart(t, true) < 0) return 0;
		if (t->c == 1) SpawnBursts(t, 1);
		int16_t c = t->c;
		if (c >= 0x46 && c <= 0x5A && (int32_t)c % 6 == 1) SpawnD();
		c = t->c;
		if (c >= 0x5D && c <= 0x6E && (int32_t)c % 6 == 0) SpawnE(true);
		if (t->c == 0x16) ScreenFade(0, 2, 4, 0xFF);
		if (t->c == 0x1C) ScreenFade(0, 2, 4, 0xFF);
		if (t->c == 0x32) ScreenFade(0, 2, 8, 0xFF);
		c = t->c;
		if (c >= 0x5D && c < 0x65)
		{
			int32_t s = ComputeSin(shl32(c - 0x30, 10) / 8 + 0x400);
			int32_t v = (60 * s) >> 12;
			if (!(t->c & 1)) v = -v;
			var<int16_t>(game_addr(game::BATTLE_CAMERA_SHAKE_OFFSET_Y)) = (int16_t)v;
		}
		Flash(t->c);
		if (t->c == 0x7C) ApplyActionResultToTarget(*(uint32_t *)(*(uint8_t **)(CastCtx() + 4) + 8));
		if (t->c == 1) BdPlaySE(game_addr(game::unk_E2B3F8), 0, 0x80);
		if (t->c == 0x7F)
			if (VoiceSlotBusy(var<uint32_t>(game_addr(game::unk_24F7A64)))) ReleaseVoiceSlot(var<uint32_t>(game_addr(game::unk_24F7A64)));
		return TimelineEnd(t);
	}
}

	void register_mag209_sorceress_spawn()
	{
		register_port(s209::ORIG_Master, (void *)s209::Master, "209 Master", 209);
		register_port(s209::ORIG_EndTask, (void *)s209::EndTask, "209 EndTask", 209);
		register_port(s209::ORIG_TimelineA, (void *)s209::TimelineA, "209 TimelineA", 209);
		register_port(s209::ORIG_TimelineB, (void *)s209::TimelineB, "209 TimelineB", 209);
		register_port(s209::ORIG_ScreenTask, (void *)s209::ScreenTask, "209 ScreenTask", 209);
		register_port(s209::ORIG_PrimATask, (void *)s209::PrimATask, "209 PrimATask", 209);
		register_port(s209::ORIG_PrimBTask, (void *)s209::PrimBTask, "209 PrimBTask", 209);
		register_port(s209::ORIG_PrimCTask, (void *)s209::PrimCTask, "209 PrimCTask", 209);
		register_port(s209::ORIG_PrimDTask, (void *)s209::PrimDTask, "209 PrimDTask", 209);
		register_port(s209::ORIG_PrimETask, (void *)s209::PrimETask, "209 PrimETask", 209);
	}
}

