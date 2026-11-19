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

// Effect 324: Tonberry King's death: the King fades out in a swarm of rising sparks while

#include "mag_common.h"
#include <stddef.h>

namespace ff8fx
{
namespace tonk324
{
	using namespace eng;
	using namespace magc;

	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::MAG_324_TonberryKingDeath_CastContext)); }
	inline int32_t &CasterSlot() { return var<int32_t>(game_addr(game::MAG_324_TonberryKingDeath_CasterSlot)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::unk_221BC60)); }
	inline uint32_t &Voice() { return var<uint32_t>(game_addr(game::unk_221BC64)); }
	inline int16_t *BonePoint() { return (int16_t *)game_addr(game::world_position); }
	inline TaskQueue *QTasks() { return (TaskQueue *)game_addr(game::MAG_324_TonberryKingDeath_TasksQueue); }
	inline uint8_t *Caster() { return Entity(CasterSlot()); }
	static constexpr game::Id KEPT_MATRIX = game::MAG_324_TonberryKingDeath_KeptMatrix, KEPT_BONES = game::MAG_324_TonberryKingDeath_KeptBones, CUR_BONES = game::MAG_324_TonberryKingDeath_CurrentBones;
	static constexpr game::Id ARENA_A = game::unk_2213C60, ARENA_B = game::unk_220BC60;

	static constexpr game::Id ORIG_RootTask = game::MAG_324_TonberryKingDeath_RootTask;
	static constexpr game::Id ORIG_CueTask = game::MAG_324_TonberryKingDeath_CueTask;
	static constexpr game::Id ORIG_EmitterA = game::MAG_324_TonberryKingDeath_EmitterA;
	static constexpr game::Id ORIG_EmitterB = game::MAG_324_TonberryKingDeath_EmitterB;
	static constexpr game::Id ORIG_TintTask = game::MAG_324_TonberryKingDeath_TintTask;
	static constexpr game::Id ORIG_GhostTask = game::MAG_324_TonberryKingDeath_GhostTask;
	static constexpr game::Id ORIG_SpinTask = game::MAG_324_TonberryKingDeath_SpinTask;
	static constexpr game::Id SEQ_SparkA = game::MAG_324_TonberryKingDeath_SeqSparkA, SEQ_SparkB = game::unk_CD0D00, SEQ_Spin = game::unk_CD0B2C;
	static constexpr game::Id TINTS = game::unk_CD1218;
	static const void *VOICE() { return (const void *)game_addr(game::unk_CD1104); }
	static const void *SOUND() { return (const void *)game_addr(game::unk_CD1210); }
	static const int CELLS = 0x64;

	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t flag, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, flag, out); }
	inline int32_t VoiceActive(uint32_t v) { return fn<int32_t (__cdecl *)(uint32_t)>(game_addr(game::BdSound_IsVoiceSlotActive))(v); }
	inline void ReleaseVoiceSlot(uint32_t v) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::BdSound_ReleaseVoiceSlot))(v); }
	inline void GteSetFarColor(int32_t r, int32_t g, int32_t b) { fn<void (__cdecl *)(int32_t, int32_t, int32_t)>(game_addr(game::GTE_SetFarColor))(r, g, b); }
	inline void GteLoadRGBC(const void *c) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadRGBC))(c); }
	inline void GteDPCS() { fn<void (__cdecl *)()>(game_addr(game::GTE_DPCS))(); }
	inline void GteStoreRGB2(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreRGB2))(dst); }
	inline void GteSetLightMatrix(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_SetLightMatrix))(m); }
	inline void GteSetBackFromTrans(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_SetBackColorFromTrans))(m); }
	inline void GteMVMVA_LightV0_Bk() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_LightV0_Bk))(); }
	inline void GteSetRotScale(int32_t s) { fn<void (__cdecl *)(int32_t)>(game_addr(game::GTE_SetRotScale))(s); }
	inline void GteSetTransFromVec32(const int32_t *v) { fn<void (__cdecl *)(const int32_t *)>(game_addr(game::GTE_SetTransFromVec32))(v); }
	inline uint32_t RenderGeometry(uint32_t group, void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(uint32_t, void *, uint32_t, int32_t, uint32_t)>(game_addr(game::RenderGeometry))(group, h, ot, mode, cursor); }
	inline Mat4x3 *ShadowCamera(const int16_t *pos, int32_t a, int32_t b) { return fn<Mat4x3 *(__cdecl *)(const int16_t *, int32_t, int32_t)>(game_addr(game::TransformCameraByShadowRotation))(pos, a, b); }
	inline uint32_t &FrameArena() { return var<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)); }

#pragma pack(push, 1)
	struct RootNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
	};
	struct TaskBody
	{
		TaskNode hdr;
		int16_t counter;
		int16_t word0E;
		uint8_t pad10[0xC];
		int16_t word1C;
		uint8_t pad1E[6];
	};
	struct Cell
	{
		uint32_t flag;
		int16_t counter;
		int16_t size;
		int16_t pos[3];
		int16_t pad0E[2];
		int16_t vy;
		int16_t pad14[2];
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x10 && sizeof(TaskBody) == 0x24 && sizeof(Cell) == 0x18, "Tonberry King death nodes");
	inline Cell *Cells() { return (Cell *)game_addr(game::MAG_324_TonberryKingDeath_Cells); }

	static void BonesOut(uint8_t *anim, uint8_t *dst)
	{
		uint8_t *sk = **(uint8_t ***)(anim + 4);
		int32_t n = sk[0];
		if (n <= 0) return;
		uint8_t *src = sk + 0x20;
		for (; n != 0; n--, src += 0x30, dst += 0x20) ff8fx::copy<(0x20)>(dst, src);
	}

	static void BonesIn(uint8_t *anim, const uint8_t *src)
	{
		uint8_t *sk = **(uint8_t ***)(anim + 4);
		if (sk[0] == 0) return;
		uint8_t *dst = sk + 0x20;
		for (int32_t i = 0; i < sk[0]; i++, src += 0x20, dst += 0x30) ff8fx::copy<(0x20)>(dst, src);
	}

	static void ModelDraw(uint8_t *ent)
	{
		uint8_t *w = (uint8_t *)FieldAlloc(0x4C);
		ComposeAffineTransform(&Camera(), (const Mat4x3 *)(ent + 0x40), (Mat4x3 *)w);
		ComputeBonesWorldMatrices(ent + 0x60, w);
		*(uint32_t *)(w + 0x24) = var<uint32_t>(game_addr(game::g_BattleVertexWorkspace));
		*(int16_t *)(w + 0x34) = 0;
		*(int16_t *)(w + 0x36) = 0;
		*(uint32_t *)(w + 0x3C) = *(const uint32_t *)(ent + 0x28);
		*(uint32_t *)(w + 0x30) = var<uint32_t>(game_addr(game::BATTLE_PACKET_BUFFER_END));
		w[0x4A] = ent[7];
		w[0x49] = ent[7];
		w[0x48] = ent[7];
		*(int16_t *)(w + 0x38) = 0x140;
		*(int16_t *)(w + 0x3A) = 0xD8;
		*(uint32_t *)(w + 0x40) = *(const uint32_t *)(ent + 0x7C);
		FrameArena() = RenderGeometry(*(const uint32_t *)(ent + 0x64), w + 0x20, RenderOT(), 4, FrameArena());
		FieldFree(0x4C);
	}

	static int32_t SparkDraw(uint8_t *w, Cell *cells, uint32_t bit, uint32_t seq, bool rise)
	{
		uint8_t *h = AllocHeader(0xB4);
		*(int16_t *)(h + 0x24) = 0;
		int32_t live = 0;
		ff8fx::copy<(0x20)>(w + 8, &Camera());
		*(uint32_t *)h = seq;
		GteSetLightMatrix(w + 8);
		GteSetBackFromTrans(w + 8);
		for (int i = 0; i < CELLS; i++)
		{
			Cell &c = cells[i];
			if (!((uint8_t)c.flag & bit)) continue;
			GteLoadV0(c.pos);
			GteMVMVA_LightV0_Bk();
			GteSetRotScale(c.size);
			*(int16_t *)(h + 4) = c.counter;
			int32_t *mac = (int32_t *)(w + 0x38), *dir = (int32_t *)(w + 0x28);
			GteReadMAC123(mac);
			NormalizeVector(mac, dir);
			const int32_t k = 0 - (c.size >> 3);
			mac[0] += mul32(dir[0], k) >> 12;
			mac[1] += mul32(dir[1], k) >> 12;
			mac[2] += mul32(dir[2], k) >> 12;
			GteSetTransFromVec32(mac);
			PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
			if (DrawOnly()) continue;
			if (*(const int16_t *)(h + 0x28) < 0) c.flag = 0;
			else
			{
				c.counter++;
				if (rise) c.pos[1] = (int16_t)(c.pos[1] + c.vy);
				live++;
			}
		}
		FieldFree(0xB4);
		return live;
	}

	static const uint8_t *SpinDraw(const TaskBody *t, const int16_t *point)
	{
		int16_t ang[3] = { 0, (int16_t)(t->word0E << 11), 0 };
		Mat4x3 m;
		ComposeZYXRotationMatrix(ang, &m);
		const int32_t sc[3] = { t->word1C, t->word1C, t->word1C };
		Scale3DMatrix(&m, sc);
		const Mat4x3 *p = ShadowCamera(point, t->word1C, 0 - (t->word1C >> 4));
		m.t[0] = (t->word0E ? 0xC8 : -0xC8) + p->t[0];
		m.t[1] = p->t[1] - 0x3C;
		m.t[2] = p->t[2];
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		uint8_t *h = AllocHeader(0xB4);
		*(uint32_t *)h = game_addr(SEQ_Spin);
		*(int16_t *)(h + 4) = t->counter;
		*(int16_t *)(h + 0x24) = 8;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
		FieldFree(0xB4);
		return h;
	}
}
}

namespace ff8fx
{
namespace tonk324
{
	static uint32_t __cdecl SpinTask(TaskNode *n)
	{
		TaskBody *t = (TaskBody *)n;
		const uint8_t *h = SpinDraw(t, BonePoint());
		if (DrawOnly()) return 0;
		t->counter++;
		return *(const int16_t *)(h + 0x28) < 0 ? TASK_END : 0;
	}

	static uint32_t __cdecl GhostTask(TaskNode *n)
	{
		TaskBody *t = (TaskBody *)n;
		if (t->counter == 0)
		{
			BonesOut(Caster() + 0x60, (uint8_t *)game_addr(KEPT_BONES));
			uint8_t *e = Caster();
			*(uint32_t *)(e + 0x7C) &= ~2u;
			ff8fx::copy<(0x20)>((void *)game_addr(KEPT_MATRIX), e + 0x40);
		}
		BonesOut(Caster() + 0x60, (uint8_t *)game_addr(CUR_BONES));
		uint8_t *e = Caster();
		const uint32_t tint = *(const uint32_t *)(e + 0x28);
		const uint32_t mask = *(const uint32_t *)(e + 0x7C);
		uint8_t live[0x20];
		if (t->counter > 6)
		{
			ff8fx::copy<(0x20)>(live, e + 0x40);
			ff8fx::copy<(0x20)>(e + 0x40, (const void *)game_addr(KEPT_MATRIX));
		}
		*(uint32_t *)(e + 0x7C) = 2;
		BonesIn(e + 0x60, (const uint8_t *)game_addr(KEPT_BONES));
		if (t->counter >= 0x10)
		{
			GteSetFarColor(0, 0, 0);
			GteLoadRGBC(e + 0x28);
			GteSetIR0((t->counter - 0x10) << 8);
			GteDPCS();
			GteStoreRGB2(e + 0x28);
			e[0x2B] = 2;
			e[7] = 0;
		}
		ModelDraw(e);
		BonesIn(Caster() + 0x60, (const uint8_t *)game_addr(CUR_BONES));
		e = Caster();
		const int16_t c = t->counter;
		*(uint32_t *)(e + 0x7C) = mask;
		*(uint32_t *)(e + 0x28) = tint;
		if (c > 6) ff8fx::copy<(0x20)>(e + 0x40, live);
		if (DrawOnly()) return 0;
		t->counter = (int16_t)(c + 1);
		return t->counter > 0x20 ? TASK_END : 0;
	}

	static void TintColour(TaskBody *t, uint8_t *e, int16_t c)
	{
		if (c < 4)
		{
			const uint32_t far = var<uint32_t>(game_addr(game::CURRENT_BS_MODEL_PALETTE));
			GteSetFarColor(far & 0xFF, (far >> 8) & 0xFF, var<uint8_t>(game_addr(game::unk_B8B7DA)));
			GteLoadRGBC((const void *)game_addr(TINTS));
			GteSetIR0((4 - c) << 10);
			GteDPCS();
			GteStoreRGB2(e + 0x28);
		}
		else if (c >= 0x2A)
		{
			GteSetFarColor(0, 0, 0);
			GteLoadRGBC((const void *)(game_addr(TINTS) + t->word1C * 4));
			GteSetIR0((c - 0x2A) << 9);
			GteDPCS();
			GteStoreRGB2(e + 0x28);
		}
		else
		{
			const int32_t idx = (c - 4) % 0x18;
			t->word1C = (int16_t)idx;
			*(uint32_t *)(e + 0x28) = *(const uint32_t *)(game_addr(TINTS) + idx * 4);
		}
	}

	static uint32_t __cdecl TintTask(TaskNode *n)
	{
		TaskBody *t = (TaskBody *)n;
		const int16_t c = t->counter;
		uint8_t *e = Entity(t->word0E);
		TintColour(t, e, c);
		e[1] |= 8;
		const uint16_t flags = *(const uint16_t *)e;
		if (DrawOnly()) return 0;
		t->counter++;
		if (t->counter < 0x32) return 0;
		*(uint16_t *)e = (uint16_t)(flags & 0xF7FF);
		return TASK_END;
	}

	static uint32_t __cdecl EmitterA(TaskNode *n)
	{
		TaskBody *t = (TaskBody *)n;
		uint8_t *w = (uint8_t *)FieldAlloc(0x48);
		if (!DrawOnly() && t->counter <= 0x2E)
		{
			const int32_t nb = **(const uint8_t *const *)*(uint8_t *const *)(Caster() + 0x64);
			for (int i = 0; i < 2; i++)
			{
				int k = 0;
				while (k < CELLS && Cells()[k].flag != 0) k++;
				if (k >= CELLS) break;
				Cell &c = Cells()[k];
				c.flag = 1;
				c.counter = 0;
				const int32_t r0 = CrtRand() % 0x600;
				c.size = (int16_t)(r0 - (int16_t)(t->counter * 0x28) + 0x800);
				const int32_t bone = nb ? CrtRand() % nb : 0;
				GetEffectSpawnPosition(Caster(), bone, CrtRand() % 0x1000, c.pos);
				const int32_t s = 0xFA - t->counter * 3, range = s + s;
				c.pos[0] = (int16_t)(c.pos[0] + (CrtRand() % range - s));
				c.pos[1] = (int16_t)(c.pos[1] + (CrtRand() % range - s));
				c.pos[2] = (int16_t)(c.pos[2] + (CrtRand() % range - s));
				c.vy = (int16_t)(-5 - CrtRand() % 0x19);
			}
		}
		const int32_t live = SparkDraw(w, Cells(), 1, game_addr(SEQ_SparkA), true);
		FieldFree(0x48);
		if (DrawOnly()) return 0;
		t->counter++;
		return live ? 0 : TASK_END;
	}

	static uint32_t __cdecl EmitterB(TaskNode *n)
	{
		TaskBody *t = (TaskBody *)n;
		uint8_t *w = (uint8_t *)FieldAlloc(0x48);
		if (!DrawOnly() && t->counter <= 0x2E)
		{
			const int32_t nb = **(const uint8_t *const *)*(uint8_t *const *)(Caster() + 0x64);
			for (int i = 0; i < 2; i++)
			{
				int k = 0;
				while (k < CELLS && Cells()[k].flag != 0) k++;
				if (k >= CELLS) break;
				Cell &c = Cells()[k];
				c.flag = 2;
				c.counter = 0;
				const int32_t r0 = CrtRand() % 0x600;
				c.size = (int16_t)(r0 + (int16_t)((int16_t)(0x18 - t->counter) << 5));
				const int32_t bone = nb ? CrtRand() % nb : 0;
				GetEffectSpawnPosition(Caster(), bone, CrtRand() % 0x1000, c.pos);
				const int32_t s = (0x64 - t->counter) * 3, range = s + s;
				c.pos[0] = (int16_t)(c.pos[0] + (CrtRand() % range - s));
				c.pos[1] = (int16_t)(c.pos[1] + (CrtRand() % range - s));
				c.pos[2] = (int16_t)(c.pos[2] + (CrtRand() % range - s));
			}
		}
		const int32_t live = SparkDraw(w, Cells(), 2, game_addr(SEQ_SparkB), false);
		FieldFree(0x48);
		if (DrawOnly()) return 0;
		t->counter++;
		return live ? 0 : TASK_END;
	}

	static TaskBody *AddTask(uint32_t fn) { return (TaskBody *)AddTaskToQueue(QTasks(), fn); }

	static uint32_t __cdecl CueTask(TaskNode *n)
	{
		TaskBody *t = (TaskBody *)n;
		GetEffectSpawnPosition(Caster(), 0xF0, 0x1000, BonePoint());
		if (DrawOnly()) return 0;
		if (t->counter == 0) Voice() = ClaimVoiceSlot(VOICE(), 1, 0x80);
		if (t->counter == 0xC)
		{
			AddTask(game_addr(ORIG_EmitterA))->counter = 0;
			t->word0E = 0;
			if (t->counter == 0xC) AddTask(game_addr(ORIG_EmitterB))->counter = 0;
		}
		if (t->counter == 8)
		{
			TaskBody *a = AddTask(game_addr(ORIG_TintTask));
			a->counter = 0;
			a->word0E = (int16_t)CasterSlot();
		}
		if (t->counter == 0x12) AddTask(game_addr(ORIG_GhostTask))->counter = 0;
		if (t->counter >= 0x7D && t->counter <= 0x87 && t->counter % 2 == 1)
		{
			TaskBody *a = AddTask(game_addr(ORIG_SpinTask));
			const int16_t side = t->word0E;
			a->counter = 0;
			a->word0E = (int16_t)(side & 1);
			a->word1C = 0x800;
			t->word0E = (int16_t)(side + 1);
		}
		if (t->counter == 0x82) ApplyActionResultToTarget(Ctx()->actions[0].targets);
		if (t->counter == 1) BdPlaySE3D(SOUND(), 0, BonePoint());
		if (t->counter == 0xC5 && VoiceActive(Voice())) ReleaseVoiceSlot(Voice());
		t->counter++;
		return t->counter > 0xC8 ? TASK_END : 0;
	}

	static uint32_t __cdecl RootTask(TaskNode *n)
	{
		RootNode *r = (RootNode *)n;
		PacketCursor() = game_addr(ARENA_A);
		if (!(*(const uint8_t *)&r->counter & 1)) PacketCursor() = game_addr(ARENA_B);
		const int a = ExecuteTaskQueue(QTasks());
		r->counter++;
		return a ? 0 : TASK_END;
	}
}

	void register_mag324_tonberry_king_death()
	{
		register_port(tonk324::ORIG_RootTask, (void *)tonk324::RootTask, "324 RootTask", 324);
		register_port(tonk324::ORIG_CueTask, (void *)tonk324::CueTask, "324 CueTask", 324);
		register_port(tonk324::ORIG_EmitterA, (void *)tonk324::EmitterA, "324 EmitterA", 324);
		register_port(tonk324::ORIG_EmitterB, (void *)tonk324::EmitterB, "324 EmitterB", 324);
		register_port(tonk324::ORIG_TintTask, (void *)tonk324::TintTask, "324 TintTask", 324);
		register_port(tonk324::ORIG_GhostTask, (void *)tonk324::GhostTask, "324 GhostTask", 324);
		register_port(tonk324::ORIG_SpinTask, (void *)tonk324::SpinTask, "324 SpinTask", 324);
	}
}

