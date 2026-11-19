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

// Effect 217: Energy Bomber

#include "mag_common.h"

namespace ff8fx
{
namespace bomber217
{
	using namespace eng;
	using namespace magc;

	inline uint32_t &TexFile() { return var<uint32_t>(game_addr(game::MAG_217_EnergyBomber_TextureFile)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::MAG_217_EnergyBomber_CastContext)); }
	inline uint8_t *&Caster() { return var<uint8_t *>(game_addr(game::MAG_217_EnergyBomber_CasterEntity)); }
	inline uint8_t *&Target() { return var<uint8_t *>(game_addr(game::MAG_217_EnergyBomber_Target)); }
	inline TaskQueue *QSparks() { return (TaskQueue *)game_addr(game::MAG_217_EnergyBomber_SparksQueue); }
	inline TaskQueue *QMain() { return (TaskQueue *)game_addr(game::MAG_217_EnergyBomber_MainQueue); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_217_EnergyBomber_PacketCursor)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::MAG_217_EnergyBomber_MagicBufferBase)); }

	static constexpr game::Id ORIG_RootTask = game::MAG_217_EnergyBomber_RootTask;
	static constexpr game::Id ORIG_MainTask = game::MAG_217_EnergyBomber_MainTask;
	static constexpr game::Id ORIG_SparkTask = game::MAG_217_EnergyBomber_SparkTask;
	static constexpr game::Id ORIG_PrimCallback = game::MAG_217_EnergyBomber_PrimCallback;
	static const uint32_t POOL_Main = 0x800;
	static const uint32_t MORPH = 0xE84;
	static const uint32_t ARENA_A = 0x1244;
	static const uint32_t ARENA_B = 0x13244;
	static constexpr game::Id LAYOUT_Ball = game::MAG_217_EnergyBomber_LayoutBall;
	static constexpr game::Id LAYOUT_Burst = game::MAG_217_EnergyBomber_LayoutBurst;
	static constexpr game::Id MODEL_Trail = game::MAG_217_EnergyBomber_ModelTrail;
	static constexpr game::Id SEQ_Spark = game::MAG_217_EnergyBomber_SeqSpark;
	static const void *SOUND_Start() { return (const void *)game_addr(game::MAG_217_EnergyBomber_SoundStart); }
	static const uint32_t FLASH = 0x8FC;

	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t flag, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, flag, out); }
	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void RotationFromAnglesYXZ(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::MAG_063_RotationFromAnglesYXZ))(angles, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void GteMVMVA_RotV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0))(); }
	inline uint32_t RenderPrim2(void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::MAG_070_RenderPrimModel))(h, ot, mode, cursor); }
	inline void BlendVectors(const int16_t *a, const int16_t *b, int32_t wa, int32_t wb, int16_t *out) { fn<void (__cdecl *)(const int16_t *, const int16_t *, int32_t, int32_t, int16_t *)>(game_addr(game::VecLerp))(a, b, wa, wb, out); }
	inline void Apply3D(const Mat4x3 *m, const int16_t *in, int32_t *out) { fn<void (__cdecl *)(const Mat4x3 *, const int16_t *, int32_t *)>(game_addr(game::apply3DTransform))(m, in, out); }
	inline void DirectionFromAngles(int32_t pitch, int32_t yaw, int16_t *out) { fn<void (__cdecl *)(int32_t, int32_t, int16_t *)>(game_addr(game::Effect_DirectionFromPitchYaw))(pitch, yaw, out); }
	inline uint32_t OT() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)); }

#pragma pack(push, 1)
	struct RootNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t pad0E;
		uint8_t started;
		uint32_t arena;
	};
	struct MainNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		int16_t pos[4];
		int16_t vel[3];
		int16_t pad1E;
		int16_t hop[4];
		int16_t target[4];
		int16_t trail[2][4];
		int16_t count;
		int16_t index;
		uint8_t layout_ball[0x398];
		uint8_t layout_burst[0x2A8];
	};
	struct SparkNode
	{
		TaskNode hdr;
		int16_t pos[3];
		int16_t counter;
		int16_t vel[3];
		int16_t drag[3];
	};
	struct PrimArg
	{
		int16_t anchor[4];
		int32_t depth;
		uint32_t morph;
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x14 && sizeof(MainNode) == 0x684 && sizeof(SparkNode) == 0x20, "217 nodes");
	static_assert(sizeof(PrimArg) == 0x10, "217 prim block");

	static int16_t PointPad(int32_t c) { return c == 0 ? (int16_t)((uint32_t)Target() >> 16) : 0; }

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
		if (r->flags & 0x200)
		{
			GteSetRotMatrixCtrl(&Camera());
			GteLoadV0(arg->anchor);
			GteMVMVA_RotV0();
			GteReadMAC123(m.t);
			m.t[0] = (int32_t)((uint32_t)m.t[0] + (uint32_t)(int32_t)r->pos[0]);
			m.t[1] = (int32_t)((uint32_t)m.t[1] + (uint32_t)(int32_t)r->pos[1]);
			m.t[2] = (int32_t)((uint32_t)m.t[2] + (uint32_t)(int32_t)r->pos[2]);
		}
		else
		{
			const int16_t off[4] = { (int16_t)(r->pos[0] + arg->anchor[0]), (int16_t)(r->pos[1] + arg->anchor[1]), (int16_t)(r->pos[2] + arg->anchor[2]), 0 };
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
			const int32_t sv[3] = { r->scale[0], r->scale[1], r->scale[2] };
			Scale3DMatrix(&m, sv);
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
		PacketCursor() = RenderPrim2(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static void Play(uint8_t *layout, const int16_t *point)
	{
		PrimArg arg;
		ff8fx::copy<(8)>(arg.anchor, point);
		arg.depth = -0x80;
		arg.morph = TexBase() + MORPH;
		prim::play((prim::Layout *)layout, PrimCallback, (int)&arg, 0);
	}

	static int32_t HopWeight(int32_t k)
	{
		int32_t s = ComputeSin((int32_t)((((uint32_t)k << 12) >> 2) + 0x200) >> 2);
		s = ComputeSin(s >> 2);
		s = ComputeSin(s >> 2);
		return ComputeSin(s >> 2);
	}

	static bool BallPoint(MainNode *m, int32_t c, int16_t *P, bool rand_hop)
	{
		if (c < 0x2F)
		{
			GetEffectSpawnPosition(Caster(), 0x27, 0, P);
			P[1] = (int16_t)(P[1] - 0x200);
			if (c == 0x2E)
			{
				m->vel[0] = (int16_t)(P[0] - m->pos[0]);
				m->vel[1] = (int16_t)(P[1] - m->pos[1]);
				m->vel[2] = (int16_t)(P[2] - m->pos[2]);
			}
			ff8fx::copy<(8)>(m->pos, P);
			return true;
		}
		if (c < 0x37)
		{
			m->pos[0] = (int16_t)(m->pos[0] + m->vel[0]);
			m->pos[1] = (int16_t)(m->pos[1] + m->vel[1]);
			m->pos[2] = (int16_t)(m->pos[2] + m->vel[2]);
			ff8fx::copy<(8)>(P, m->pos);
			return true;
		}
		int32_t s;
		if (c < 0x3B)
		{
			s = HopWeight(c - 0x37);
			if (c == 0x37)
			{
				if (!rand_hop) return false;
				const int32_t r = CrtRand();
				m->hop[1] = (int16_t)(m->target[1] - 0x7D0);
				m->hop[0] = (int16_t)((int16_t)((r & 0xFFF) + m->target[0]) - 0x800);
				m->hop[2] = (int16_t)(((int32_t)m->pos[2] - m->target[2]) * 3 / 4 + m->target[2]);
			}
		}
		else if (c < 0x3F)
		{
			s = HopWeight(c - 0x3B);
			if (c == 0x3B)
			{
				ff8fx::copy<(8)>(m->pos, m->hop);
				m->hop[0] = (int16_t)-m->hop[0];
				m->hop[1] = m->target[1];
				m->hop[2] = (int16_t)(((int32_t)m->pos[2] - m->target[2]) * 3 / 4 + m->target[2]);
			}
		}
		else
		{
			s = HopWeight(c - 0x3F);
			if (c == 0x3F)
			{
				ff8fx::copy<(8)>(m->pos, m->hop);
				m->hop[0] = m->target[0];
				m->hop[1] = m->target[1];
				m->hop[2] = m->target[2];
			}
		}
		BlendVectors(m->pos, m->hop, 0x1000 - s, s, P);
		return true;
	}

	static void TrailDraw(uint8_t *h, const int16_t *point, int32_t i)
	{
		Mat4x3 *m = (Mat4x3 *)(h + 0x58);
		Apply3D(&Camera(), point, m->t);
		m->t[0] = (int32_t)((uint32_t)m->t[0] + (uint32_t)Camera().t[0]);
		m->t[1] = (int32_t)((uint32_t)m->t[1] + (uint32_t)Camera().t[1]);
		m->t[2] = (int32_t)((uint32_t)m->t[2] + (uint32_t)Camera().t[2]);
		m->m[2][2] = 0x1000;
		m->m[1][1] = 0x1000;
		m->m[0][0] = 0x1000;
		m->m[2][1] = 0;
		m->m[2][0] = 0;
		m->m[1][2] = 0;
		m->m[1][0] = 0;
		m->m[0][2] = 0;
		m->m[0][1] = 0;
		GteSetRotMatrix(m);
		GteSetTransVector(m);
		*(uint32_t *)h = game_addr(MODEL_Trail);
		*(uint32_t *)(h + 0x1C) = 0x30;
		*(uint32_t *)(h + 8) = 0;
		*(int32_t *)(h + 0xC) = shl32(i, 10) / 2 + 0xC00;
		PacketCursor() = RenderPrimModel(h, OT() + 0x44, 2, PacketCursor());
	}

	static void SparkDraw(int16_t *pos, int16_t frame)
	{
		TransformCameraByShadowRotation(pos, 0x800, -0x200);
		uint8_t *h = AllocHeader(0xB4);
		*(uint32_t *)h = game_addr(SEQ_Spark);
		*(int16_t *)(h + 4) = frame;
		*(uint16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, OT() + 0x44, 2, PacketCursor());
		FieldFree(0xB4);
	}

	static int32_t FlashLevel(int32_t c)
	{
		if (c < 0xC) return mul32(c, FLASH) / 12;
		if (c >= 0x4A) return mul32(0x56 - c, FLASH) / 12;
		return FLASH;
	}
}
}

namespace ff8fx
{
namespace bomber217
{
	static uint32_t __cdecl SparkTask(TaskNode *n)
	{
		SparkNode *s = (SparkNode *)n;
		SparkDraw(s->pos, s->counter);
		s->vel[0] = (int16_t)(s->vel[0] + s->drag[0]);
		s->vel[1] = (int16_t)(s->vel[1] + s->drag[1]);
		s->vel[2] = (int16_t)(s->vel[2] + s->drag[2]);
		s->pos[0] = (int16_t)(s->pos[0] + s->vel[0]);
		s->pos[1] = (int16_t)(s->pos[1] + s->vel[1]);
		s->pos[2] = (int16_t)(s->pos[2] + s->vel[2]);
		s->counter++;
		return s->counter < 0xC ? 0 : TASK_END;
	}

	static uint32_t __cdecl MainTask(TaskNode *n)
	{
		MainNode *m = (MainNode *)n;
		SetScreenFlash(FlashLevel(m->counter), 0);
		const int32_t c = m->counter;
		if ((uint32_t)c < 0x43)
		{
			int16_t P[4];
			P[3] = PointPad(c);
			BallPoint(m, c, P, true);
			Play(m->layout_ball, P);
			if (m->count > 0)
			{
				uint8_t *h = (uint8_t *)FieldAlloc(0x78);
				for (int32_t i = m->index; i < m->count; i++) TrailDraw(h, m->trail[i], i);
				FieldFree(0x78);
				m->index++;
			}
			if ((uint32_t)c >= 0x37)
			{
				ff8fx::copy<(8)>(m->trail[1], m->trail[0]);
				m->count++;
				ff8fx::copy<(8)>(m->trail[0], P);
				if (m->count > 2) m->count = 2;
				m->index = 0;
			}
		}
		const int32_t k = m->counter - 0x3F;
		if ((uint32_t)k < 0x17)
		{
			if (k == 0) DecodeModelPrimLayout(game_addr(LAYOUT_Burst), m->layout_burst, 0x2A8);
			else if (k < 5)
			{
				SparkNode *s = (SparkNode *)AddTaskToQueue(QSparks(), game_addr(ORIG_SparkTask));
				for (int left = 0x10; s; )
				{
					s->counter = 0;
					s->pos[0] = m->target[0];
					s->pos[1] = m->target[1];
					s->pos[2] = m->target[2];
					s->counter = 0;
					const int32_t yaw = CrtRand();
					const int32_t pitch = CrtRand();
					DirectionFromAngles(pitch, yaw, s->vel);
					const int32_t speed = (CrtRand() & 0x1FF) + 0x200;
					s->vel[0] = (int16_t)(mul32(s->vel[0], speed) >> 12);
					s->vel[1] = (int16_t)(mul32(s->vel[1], speed) >> 12);
					s->vel[2] = (int16_t)(mul32(s->vel[2], speed) >> 12);
					s->drag[0] = (int16_t)(-(int32_t)s->vel[0] >> 4);
					s->drag[1] = (int16_t)(-(int32_t)s->vel[1] >> 4);
					s->drag[2] = (int16_t)(-(int32_t)s->vel[2] >> 4);
					if (--left == 0) break;
					s = (SparkNode *)AddTaskToQueue(QSparks(), game_addr(ORIG_SparkTask));
				}
			}
			Play(m->layout_burst, m->target);
		}
		if (m->counter == 0) BdPlaySE(SOUND_Start(), 0, 0x80);
		if (m->counter == 0x4E) ApplyActionResultToTarget(Ctx()->actions->targets);
		if (m->counter >= 0x56)
		{
			if (m->counter == 0x56)
			{
				SetScreenFlash(0, 0);
				m->counter++;
			}
			return TASK_END;
		}
		m->counter++;
		return 0;
	}

	static uint32_t __cdecl RootTask(TaskNode *n)
	{
		RootNode *rt = (RootNode *)n;
		if (rt->arena)
		{
			PacketCursor() = TexBase() + ARENA_A;
			rt->arena = 0;
		}
		else
		{
			PacketCursor() = TexBase() + ARENA_B;
			rt->arena = 1;
		}
		if (rt->counter == 1 && !rt->started)
		{
			rt->started = 1;
			InitTaskQueuePool(QMain(), (void *)(TexBase() + POOL_Main), 0x684, 1);
			InitTaskQueuePool(QSparks(), (void *)TexBase(), 0x20, 0x40);
			MainNode *m = (MainNode *)AddTaskToQueue(QMain(), game_addr(ORIG_MainTask));
			Memset32(&m->counter, 0, 0x19E);
			DecodeModelPrimLayout(game_addr(LAYOUT_Ball), m->layout_ball, 0x398);
			GetDefaultEffectPosition(Target(), m->target);
		}
		int main = (int)n;
		if (rt->started)
		{
			main = ExecuteTaskQueue(QMain());
			ExecuteTaskQueue(QSparks());
		}
		if (rt->started && main == 0)
		{
			return TASK_END;
		}
		rt->counter++;
		return 0;
	}
}

	void register_mag217_energy_bomber()
	{
		register_port(bomber217::ORIG_RootTask, (void *)bomber217::RootTask, "217 RootTask", 217);
		register_port(bomber217::ORIG_MainTask, (void *)bomber217::MainTask, "217 MainTask", 217);
		register_port(bomber217::ORIG_SparkTask, (void *)bomber217::SparkTask, "217 SparkTask", 217);
	}
}

