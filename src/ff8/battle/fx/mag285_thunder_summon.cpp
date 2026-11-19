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

// Effect 285: Thunder Summon

#include "mag_common.h"

namespace ff8fx
{
namespace thunder285
{
	using namespace eng;
	using namespace magc;

	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::unk_2507D0C)); }
	inline uint8_t *&Caster() { return var<uint8_t *>(game_addr(game::unk_2507D10)); }
	inline TaskQueue *QSparks() { return (TaskQueue *)game_addr(game::unk_2507D18); }
	inline TaskQueue *QMain() { return (TaskQueue *)game_addr(game::unk_2507D28); }
	inline int16_t &MeanZ() { return var<int16_t>(game_addr(game::unk_2507D5C)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::unk_2507D60)); }
	inline uint32_t &FlashLevel() { return var<uint32_t>(game_addr(game::unk_2507D64)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::unk_2507D68)); }
	inline Mat4x3 &EffCam() { return var<Mat4x3>(game_addr(game::unk_2507D70)); }
	inline uint32_t *KeptWords() { return (uint32_t *)game_addr(game::unk_2507D90); }
	inline int16_t *Eye() { return (int16_t *)game_addr(game::Battle_Camera_world_XZ_s16); }
	inline int16_t *LookAt() { return (int16_t *)game_addr(game::Battle_Camera_LookAt_XZ_s16); }
	inline uint32_t RenderOT44() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44; }

	static constexpr game::Id ORIG_MasterTask = game::MAG_285_sub_67A900;
	static constexpr game::Id ORIG_MainTask = game::MAG_285_sub_67AC00;
	static constexpr game::Id ORIG_SparkTask = game::MAG_285_sub_67B720;
	static constexpr game::Id ORIG_PrimCallback = game::MAG_285_sub_67B190;
	static const uint32_t POOL_Main = 0x500;
	static const uint32_t STREAM = 0x1E38;
	static const uint32_t MORPH = 0x1AB8;
	static const uint32_t ARENA_A = 0x1E80, ARENA_B = 0x11E80;
	static const uint32_t LOAD_Next = 0x21E80;
	static constexpr game::Id MODEL_Bolt = game::unk_1084BBC;
	static constexpr game::Id MODEL_Ground = game::unk_1089B00;
	static constexpr game::Id SEQ_Spark = game::unk_1084AF8;
	static constexpr game::Id POINT_A = game::unk_108FAB8, POINT_B = game::unk_108FAB0;
	static const void *SOUND_Start() { return (const void *)game_addr(game::unk_108FAA8); }
	static const void *SOUND_Bolt() { return (const void *)game_addr(game::unk_108FAAC); }

	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void MatrixMultiply3x3(Mat4x3 *a, const void *b) { fn<void (__cdecl *)(Mat4x3 *, const void *)>(game_addr(game::MatrixMultiply3x3))(a, b); }
	inline void MatrixMulVector(const Mat4x3 *m, const int16_t *v, int16_t *out) { fn<void (__cdecl *)(const Mat4x3 *, const int16_t *, int16_t *)>(game_addr(game::matrixMultiplyVector))(m, v, out); }
	inline void GteMVMVA_RotV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0))(); }
	inline uint32_t RenderPrimSet(void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::MAG_063_RenderPrimSet))(h, ot, mode, cursor); }
	inline void RotY(int32_t angle, Mat4x3 *out) { fn<void (__cdecl *)(int32_t, Mat4x3 *)>(game_addr(game::Effect_RotationYMatrix))(angle, out); }
	inline void RotX(int32_t angle, Mat4x3 *out) { fn<void (__cdecl *)(int32_t, Mat4x3 *)>(game_addr(game::MAG_063_RotationXMatrix))(angle, out); }
	inline void VecLerp(const int16_t *a, const int16_t *b, int32_t wa, int32_t wb, int16_t *out) { fn<void (__cdecl *)(const int16_t *, const int16_t *, int32_t, int32_t, int16_t *)>(game_addr(game::VecLerp))(a, b, wa, wb, out); }
	inline uint32_t Tile(int32_t r, int32_t g, int32_t b, int32_t level, uint32_t cursor) { return fn<uint32_t (__cdecl *)(int32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_FullScreenTile))(r, g, b, level, cursor); }
	inline void CameraArmReturn() { fn<void (__cdecl *)()>(game_addr(game::BS_Camera_ArmReturnAfterEffect))(); }
	inline void StreamLoad(int32_t id, uint32_t dst, int32_t kind) { fn<void (__cdecl *)(int32_t, uint32_t, int32_t)>(game_addr(game::Stream_QueueLoad))(id, dst, kind); }
	inline void StreamPump() { fn<void (__cdecl *)()>(game_addr(game::Stream_Pump))(); }
	inline int32_t StreamDone(int32_t a) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::BattleFile_PreLoad_IsLoaded))(a); }

#pragma pack(push, 1)
	struct MasterNode
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
		uint8_t bolt[0x944];
		uint8_t ground[0xC64];
	};
	struct SparkNode
	{
		TaskNode hdr;
		int16_t pos[3];
		int16_t counter;
	};
	struct PrimArg
	{
		int16_t pos[4];
		int16_t depth;
		int16_t scroll;
		uint32_t morph;
	};
#pragma pack(pop)
	static_assert(sizeof(MasterNode) == 0x14 && sizeof(MainNode) == 0x15B8 && sizeof(SparkNode) == 0x14, "Thunder Summon nodes");
	static_assert(sizeof(PrimArg) == 0x10, "Thunder Summon prim block");

	static void OrthonormalizeCamera(const Mat4x3 *in, Mat4x3 *out)
	{
		const int16_t *r = (const int16_t *)in;
		int16_t *o = (int16_t *)out;
		int32_t v[3] = { r[1], r[4], r[7] };
		NormalizeVector(v, v);
		o[1] = (int16_t)v[0];
		o[4] = (int16_t)v[1];
		int32_t w[3] = { v[1], (int32_t)(0u - (uint32_t)v[0]), 0 };
		o[7] = (int16_t)v[2];
		NormalizeVector(w, w);
		o[0] = (int16_t)w[0];
		o[3] = (int16_t)w[1];
		o[6] = (int16_t)w[2];
		int32_t c[3];
		CrossProduct(w, v, c);
		NormalizeVector(c, c);
		o[2] = (int16_t)c[0];
		o[5] = (int16_t)c[1];
		o[8] = (int16_t)c[2];
		out->t[0] = in->t[0];
		out->t[1] = in->t[1];
		out->t[2] = in->t[2];
	}

	static void BonePoint(const int16_t *v, int16_t *out)
	{
		uint8_t *c = Caster();
		Mat4x3 m;
		ComposeAffineTransform((const Mat4x3 *)(c + 0x40), (const Mat4x3 *)(**(uint8_t ***)(c + 0x64) + 0x20 + (int32_t)v[3] * 0x30), &m);
		GteSetRotMatrixCtrl(&m);
		GteSetTransVectorCtrl(&m);
		GteLoadV0(v);
		GteMVMVA_RotV0Tr();
		GteStoreIR123(out);
	}

	static void CamOrbit(int32_t a, int32_t b, int32_t s)
	{
		Mat4x3 m;
		RotY(b, &m);
		if (a)
		{
			Mat4x3 x;
			RotX(a, &x);
			MatrixMultiply3x3(&m, &x);
		}
		int16_t *eye = Eye(), *la = LookAt();
		eye[1] = (int16_t)(la[1] + (int16_t)(mul32((int32_t)eye[1] - la[1], s) >> 12));
		int16_t v[4];
		v[1] = (int16_t)(mul32((int32_t)eye[1] - la[1], s) >> 12);
		v[0] = (int16_t)(mul32((int32_t)eye[0] - la[0], s) >> 12);
		v[2] = (int16_t)(mul32((int32_t)eye[2] - la[2], s) >> 12);
		v[3] = 0;
		MatrixMulVector(&m, v, v);
		eye[0] = (int16_t)(la[0] + v[0]);
		eye[1] = (int16_t)(la[1] + v[1]);
		eye[2] = (int16_t)(la[2] + v[2]);
	}

	static void SlotFlags(int32_t on)
	{
		for (uint8_t *p = (uint8_t *)game_addr(game::unk_1D98A15); ; p -= 0x2C)
		{
			*p = (uint8_t)((*p & 0xFD) | ((on & 1) << 1));
			if (p == (uint8_t *)game_addr(game::unk_1D98991)) break;
		}
		uint16_t *w = (uint16_t *)game_addr(game::unk_1D97668);
		for (int i = 6; i >= 0; i--, w = (uint16_t *)((uint8_t *)w - 0x9C))
		{
			if (on) *w = (uint16_t)(((KeptWords()[i] ^ *w) & 4) ^ *w);
			else *(uint8_t *)w |= 4;
		}
	}

	static void SparkDraw(const SparkNode *s)
	{
		TransformCameraByShadowRotation(s->pos, 0x800, -0x200);
		uint8_t *h = AllocHeader(0xB4);
		*(uint32_t *)h = game_addr(SEQ_Spark);
		*(int16_t *)(h + 4) = s->counter;
		*(uint16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT44(), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static void BoltArg(PrimArg *arg)
	{
		arg->pos[0] = 0;
		arg->pos[1] = -0x2710;
		arg->pos[2] = 0;
		arg->depth = 0;
		arg->scroll = 0;
		arg->morph = TexBase() + MORPH;
	}

	static void GroundArg(int32_t c, PrimArg *arg)
	{
		arg->pos[0] = 0;
		arg->pos[1] = 0;
		arg->pos[2] = MeanZ();
		arg->depth = -0x100;
		arg->scroll = (int16_t)(((uint8_t)(0 - (uint8_t)(c - 0x56)) & 3) << 5);
		arg->morph = TexBase() + MORPH;
	}
}
}

namespace ff8fx
{
namespace thunder285
{
	static void __cdecl PrimCallback(prim::Layout *l, prim::Record *r, int arg_)
	{
		const PrimArg *arg = (const PrimArg *)arg_;
		if (((int32_t)r->scale[2] | *(const int32_t *)r->scale) == 0) return;
		if (r->a >= 0x1000 && *(const uint32_t *)r->rgb == 0) return;
		uint8_t *h = AllocHeader(0x6C);
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
		int16_t off[4] = { r->pos[0], r->pos[1], r->pos[2], 0 };
		if (r->flags & 0x1000)
		{
			int32_t a[3], w[3];
			GteSetRotMatrixCtrl(&Camera());
			GteLoadV0(arg->pos);
			GteMVMVA_RotV0();
			GteReadMAC123(a);
			GteSetRotMatrixCtrl(&EffCam());
			GteLoadV0(off);
			GteMVMVA_RotV0();
			GteReadMAC123(w);
			m.t[0] = (int32_t)((uint32_t)a[0] + (uint32_t)w[0]);
			m.t[1] = (int32_t)((uint32_t)a[1] + (uint32_t)w[1]);
			m.t[2] = (int32_t)((uint32_t)a[2] + (uint32_t)w[2]);
			if (!(r->flags & 0x8000)) MatrixMultiply(&EffCam(), &m);
		}
		else if (r->flags & 0x200)
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
			if (!(r->flags & 0x8000)) MatrixMultiply(&Camera(), &m);
		}
		m.t[0] = (int32_t)((uint32_t)m.t[0] + (uint32_t)Camera().t[0]);
		m.t[1] = (int32_t)((uint32_t)m.t[1] + (uint32_t)Camera().t[1]);
		m.t[2] = (int32_t)((uint32_t)m.t[2] + (uint32_t)Camera().t[2]);
		if (!(*(const uint32_t *)r->scale == 0x10001000 && r->scale[2] == 0x1000))
		{
			if (r->flags & 0x100)
			{
				int16_t s[10] = { r->scale[0], 0, 0, 0, r->scale[1], 0, 0, 0, r->scale[2], 0 };
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
		if (arg->scroll != 0 && (int16_t)r->flags_lo == 0)
		{
			*(uint16_t *)(h + 0x26) = 0;
			*(uint16_t *)(h + 0x24) = 0;
			*(uint16_t *)(h + 0x2A) = 0x100;
			*(uint16_t *)(h + 0x28) = 0x100;
			*(uint16_t *)(h + 0x2E) = 0x80;
			*(uint16_t *)(h + 0x2C) = 0x80;
			*(uint16_t *)(h + 0x32) = 0x80;
			*(uint16_t *)(h + 0x30) = 0x100;
			*(int16_t *)(h + 0x22) = arg->scroll;
		}
		else *(int16_t *)(h + 0x22) = 0;
		*(int32_t *)(h + 0x18) = arg->depth;
		*(uint16_t *)(h + 0x20) = 0;
		PacketCursor() = RenderPrimSet(h, RenderOT44(), 2, PacketCursor());
		FieldFree(0x6C);
	}

	static uint32_t __cdecl SparkTask(TaskNode *n)
	{
		SparkNode *s = (SparkNode *)n;
		SparkDraw(s);
		s->counter++;
		return s->counter < 7 ? 0 : TASK_END;
	}

	static void SpawnSparks(int32_t c)
	{
		for (uint32_t point = game_addr(POINT_A); ; point -= 8)
		{
			SparkNode *s = (SparkNode *)AddTaskToQueue(QSparks(), game_addr(ORIG_SparkTask));
			if (s)
			{
				BonePoint((const int16_t *)point, s->pos);
				s->counter = 0;
				if ((uint32_t)c >= 0x10 && (uint32_t)c <= 0x12)
				{
					for (int k = 4; k; k--)
					{
						SparkNode *e = (SparkNode *)AddTaskToQueue(QSparks(), game_addr(ORIG_SparkTask));
						if (!e) break;
						e->pos[0] = (int16_t)((CrtRand() & 0x1FF) + s->pos[0] - 0x100);
						e->pos[1] = (int16_t)((CrtRand() & 0x1FF) + s->pos[1] - 0x100);
						const int32_t z = (CrtRand() & 0x1FF) + s->pos[2];
						e->counter = 0;
						e->pos[2] = (int16_t)(z - 0x100);
					}
				}
			}
			if (point == game_addr(POINT_B)) break;
		}
	}

	static void CameraTrack(int32_t c)
	{
		const uint8_t *e = Caster();
		const int16_t x = *(const int16_t *)(e + 0x1C), y = *(const int16_t *)(e + 0x1E), z = *(const int16_t *)(e + 0x20);
		const int32_t t = (int32_t)((uint32_t)shl32(c, 12) >> 5);
		int16_t a[4] = { (int16_t)(x - 0x2C0), (int16_t)(y - 0x6EF), (int16_t)(z + 0x8ED), 0 };
		int16_t b[4] = { (int16_t)(x + 0x6E), (int16_t)(y - 0xB52), (int16_t)(z + 0x953), 0 };
		VecLerp(a, b, 0x1000 - t, t, LookAt());
		int16_t p[4] = { (int16_t)(x - 0x4F6), (int16_t)(y - 0x11), (int16_t)(z + 0x13A4), 0 };
		int16_t q[4] = { (int16_t)(x + 0x27C), (int16_t)(y + 0x952), (int16_t)(z + 0x1A5D), 0 };
		VecLerp(p, q, 0x1000 - t, t, Eye());
	}

	static uint32_t __cdecl MainTask(TaskNode *n)
	{
		MainNode *m = (MainNode *)n;
		const int32_t c = m->counter;
		if (c < 8) FlashLevel() = (uint32_t)((int32_t)((uint32_t)mul32(c, 0x4B) << 5) >> 3);
		else if (c > 0x7F) FlashLevel() = (uint32_t)((int32_t)((uint32_t)mul32(0x87 - c, 0x4B) << 5) >> 3);
		else FlashLevel() = 0x960;
		if ((uint32_t)(c - 0x30) < 0x27)
		{
			PrimArg arg;
			BoltArg(&arg);
			prim::play((prim::Layout *)m->bolt, PrimCallback, (int)&arg, 0);
			if (c - 0x30 == 0x22)
			{
				PacketCursor() = Tile(0x80, 0x80, 0x80, 0x1000, PacketCursor());
			}
		}
		if ((uint32_t)(c - 0x56) < 0x30)
		{
			PrimArg arg;
			GroundArg(c, &arg);
			prim::play((prim::Layout *)m->ground, PrimCallback, (int)&arg, 0);
		}
		if (m->counter == 0) BdPlaySE(SOUND_Start(), 0, 0x80);
		else if (m->counter == 0x30) BdPlaySE(SOUND_Bolt(), 0, 0x80);
		const int32_t c1 = m->counter;
		if ((uint32_t)c1 < 0x20)
		{
			if ((uint32_t)c1 < 0x18) SpawnSparks(c1);
			if (c1 == 0)
			{
				CameraArmReturn();
				var<uint16_t>(game_addr(game::BATTLE_PROJECTION_DISTANCE)) = 0x120;
				var<uint16_t>(game_addr(game::BattleEntitySlotData)) |= 4;
				var<uint16_t>(game_addr(game::unk_1D9735C)) |= 4;
				var<uint16_t>(game_addr(game::unk_1D973F8)) |= 4;
				var<uint16_t>(game_addr(game::g_BattleCam_Roll)) = 0;
			}
			CameraTrack(c1);
		}
		const int32_t c2 = m->counter - 0x10;
		if ((uint32_t)c2 < 0x20)
		{
			const int32_t k = shl32(c2 & 0xFFFFF, 7);
			CamOrbit(0, k >> 7, 0x1000);
			LookAt()[1] = (int16_t)(LookAt()[1] - (int16_t)(k >> 3));
			Eye()[1] = (int16_t)(Eye()[1] - (int16_t)(k >> 5));
		}
		if (m->counter == 0x30)
		{
			SlotFlags(0);
			LookAt()[0] = 0;
			LookAt()[1] = (int16_t)0xD8F0;
			LookAt()[2] = 0;
			Eye()[0] = 0x138;
			Eye()[1] = (int16_t)0xDAF7;
			Eye()[2] = (int16_t)0xFE6D;
		}
		const int32_t c3 = m->counter - 0x56;
		if ((uint32_t)c3 < 0x30)
		{
			const int32_t a = (int32_t)(uint32_t)(((uint64_t)(uint32_t)shl32(c3, 12) * 0xAAAAAAABull) >> 37);
			if (c3 == 0)
			{
				SlotFlags(1);
				const uint32_t mz = var<uint32_t>(game_addr(game::unk_2507D5C));
				LookAt()[2] = (int16_t)mz;
				Eye()[2] = (int16_t)(mz + 0x1518);
				PacketCursor() = Tile(0x80, 0x80, 0x80, 0x1000, PacketCursor());
				LookAt()[0] = 0;
				LookAt()[1] = (int16_t)0xFC93;
				Eye()[0] = (int16_t)0xFA24;
				Eye()[1] = (int16_t)0xFA24;
			}
			else if (c3 == 1) ScreenFadeTask(0, 1, 1, 0x80);
			else if (c3 == 8) ScreenFadeTask(0, 1, 0, 0x80);
			else if (c3 == 0xA) ScreenFadeTask(0, 1, 0, 0x60);
			CamOrbit(0, ComputeSin(a >> 1) >> 7, 0xFC8);
		}
		if (m->counter == 0x6E) ApplyActionResultToTargets(Ctx()->actions[0].targets, Ctx()->actions[0].target_count);
		m->counter++;
		if (m->counter >= 0x87)
		{
			var<uint32_t>(game_addr(game::Battle_Camera_LookAt_Y)) = var<uint32_t>(game_addr(game::Battle_Camera_ReturnView_LookAt_Y));
			var<uint16_t>(game_addr(game::BATTLE_PROJECTION_DISTANCE)) = var<uint16_t>(game_addr(game::Battle_Camera_ReturnView_Param1D8E038));
			var<uint32_t>(game_addr(game::Battle_Camera_LookAt_XZ_s16)) = var<uint32_t>(game_addr(game::Battle_Camera_ReturnView_LookAt_XZ_s16));
			var<uint32_t>(game_addr(game::Battle_Camera_world_XZ_s16)) = var<uint32_t>(game_addr(game::Battle_Camera_ReturnView_world_XZ_s16));
			var<uint32_t>(game_addr(game::Battle_Camera_world_Y)) = var<uint32_t>(game_addr(game::Battle_Camera_ReturnView_world_Y));
			StreamLoad(0x168, TexBase() + LOAD_Next, 0);
			return TASK_END;
		}
		return 0;
	}

	static uint32_t __cdecl MasterTask(TaskNode *n)
	{
		MasterNode *mn = (MasterNode *)n;
		if (mn->arena)
		{
			PacketCursor() = TexBase() + ARENA_A;
			mn->arena = 0;
		}
		else
		{
			PacketCursor() = TexBase() + ARENA_B;
			mn->arena = 1;
		}
		FlashLevel() = 0;
		if (mn->counter == 1 && !mn->started)
		{
			mn->started = 1;
			for (int i = 0; i < 7; i++) KeptWords()[i] = *(const uint16_t *)Entity(i);
			InitTaskQueuePool(QMain(), (void *)(TexBase() + POOL_Main), 0x15B8, 1);
			InitTaskQueuePool(QSparks(), (void *)TexBase(), 0x14, 0x40);
			MainNode *m = (MainNode *)AddTaskToQueue(QMain(), game_addr(ORIG_MainTask));
			Memset32(&m->counter, 0, 0x56B);
			const int32_t nt = Ctx()->actions[0].target_count;
			int16_t sum = 0;
			MeanZ() = 0;
			for (int i = 0; i < nt; i++)
			{
				sum = (int16_t)(sum + *(const int16_t *)(Entity(Ctx()->actions[0].targets[i * TARGET_STRIDE]) + 0x20));
				MeanZ() = sum;
			}
			MeanZ() = (int16_t)((int32_t)sum / nt);
			DecodeModelPrimLayout(game_addr(MODEL_Bolt), m->bolt, 0x944);
			DecodeModelPrimLayout(game_addr(MODEL_Ground), m->ground, 0xC64);
			StreamStateInit((void *)(TexBase() + STREAM));
		}
		int a0;
		if (mn->started)
		{
			OrthonormalizeCamera(&Camera(), &EffCam());
			a0 = ExecuteTaskQueue(QMain());
			ExecuteTaskQueue(QSparks());
		}
		else a0 = (int)n;
		SetScreenFlash(FlashLevel(), 0);
		if (mn->started && !a0)
		{
			StreamPump();
			SetScreenFlash(0, 0);
			return StreamDone(0) ? TASK_END : 0;
		}
		mn->counter++;
		return 0;
	}
}

	void register_mag285_thunder_summon()
	{
		register_port(thunder285::ORIG_MasterTask, (void *)thunder285::MasterTask, "285 MasterTask", 285);
		register_port(thunder285::ORIG_MainTask, (void *)thunder285::MainTask, "285 MainTask", 285);
		register_port(thunder285::ORIG_SparkTask, (void *)thunder285::SparkTask, "285 SparkTask", 285);
	}
}

