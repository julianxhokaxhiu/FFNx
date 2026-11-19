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

// Effect 157: Breath

#include "mag_common.h"

namespace ff8fx
{
namespace breath157
{
	using namespace eng;
	using namespace magc;

	inline Mat4x3 &FrameB() { return var<Mat4x3>(game_addr(game::unk_23EB5C0)); }
	inline int32_t *DirB() { return (int32_t *)game_addr(game::unk_23EB5E0); }
	inline uint32_t &TargetSlot() { return var<uint32_t>(game_addr(game::unk_23EB5F0)); }
	inline int32_t &LenB() { return var<int32_t>(game_addr(game::unk_23EB5F4)); }
	inline TaskQueue *RootQueue() { return (TaskQueue *)game_addr(game::unk_23EB618); }
	inline int16_t *MouthB() { return (int16_t *)game_addr(game::unk_23EBF88); }
	inline TaskQueue *EffectQueue() { return (TaskQueue *)game_addr(game::unk_23EBF90); }
	inline Mat4x3 &FrameA() { return var<Mat4x3>(game_addr(game::unk_23ECDB0)); }
	inline int16_t *TargetPos() { return (int16_t *)game_addr(game::unk_23ECDD0); }
	inline int32_t *DirA() { return (int32_t *)game_addr(game::unk_23ECDD8); }
	inline int32_t &LenA() { return var<int32_t>(game_addr(game::unk_23ECDE8)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::unk_23ECDEC)); }
	inline uint32_t &CasterSlot() { return var<uint32_t>(game_addr(game::unk_23ECDF0)); }
	inline int16_t *MouthA() { return (int16_t *)game_addr(game::unk_23ECDF8); }
	inline int16_t *CloudPos(int16_t i) { return (int16_t *)(game_addr(game::unk_23ECE00) + (uint32_t)(int32_t)i * 8); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::unk_23ECE18)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::unk_23ECE1C)); }
	inline Mat4x3 &SpriteFrame() { return var<Mat4x3>(game_addr(game::unk_23ECE20)); }
	static constexpr game::Id PARTS = game::unk_23EB628, PARTS_END = game::unk_23EBF88;
	static const int PART_COUNT = 100;
	static const uint32_t ARENA_SIZE = 0xC000;

	static constexpr game::Id ORIG_RootTask = game::MAG_157_sub_605460;
	static constexpr game::Id ORIG_TexRestore = game::MAG_157_sub_6054D0;
	static constexpr game::Id ORIG_Director = game::MAG_157_sub_603F50;
	static constexpr game::Id ORIG_Swirl = game::MAG_157_sub_6043A0;
	static constexpr game::Id ORIG_BreathParticles = game::MAG_157_sub_6044D0;
	static constexpr game::Id ORIG_Cloud = game::MAG_157_sub_604780;
	static constexpr game::Id ORIG_Burst = game::MAG_157_sub_6049F0;
	static constexpr game::Id ORIG_Puff = game::MAG_157_sub_604B30;
	static constexpr game::Id ORIG_PuffParticles = game::MAG_157_sub_604C80;
	static constexpr game::Id ORIG_CloudModels = game::MAG_157_sub_604E90;
	static constexpr game::Id ORIG_Rings = game::MAG_157_sub_6050A0;
	static constexpr game::Id ORIG_Splatter = game::MAG_157_sub_6052A0;
	static constexpr game::Id REF_DIR = game::unk_DB4CB8;
	static constexpr game::Id MODEL_Swirl = game::unk_DB0DBC, MODEL_Burst = game::unk_DB42C4, MODEL_Cloud1 = game::unk_DB20DC, MODEL_Cloud2 = game::unk_DB2F54, MODEL_Ring = game::unk_DB3BDC;
	static constexpr game::Id SEQ_Breath = game::unk_DB0B34, SEQ_Puff = game::unk_DB0AC0, SEQ_PuffPart = game::unk_DB0CE0, SEQ_Splatter = game::unk_DB0914;
	static const void *SOUND_Start() { return (const void *)game_addr(game::unk_DB4CAC); }
	static const void *SOUND_Cloud() { return (const void *)game_addr(game::unk_DB4CB0); }

	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t frac, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, frac, out); }
	inline int32_t NormalizeVectorLen(const int32_t *in, int32_t *out) { return fn<int32_t (__cdecl *)(const int32_t *, int32_t *)>(game_addr(game::NormalizeVectorToFixedPoint))(in, out); }
	inline int32_t SqrtInt(int32_t v) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::Math_SqrtInt))(v); }
	inline int32_t ComputeCos(int32_t a) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(a); }
	inline int32_t ComputeSin(int32_t a) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeSin))(a); }
	inline void GteSetLightMatrix(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_SetLightMatrix))(m); }
	inline void GteSetBackColorFromTrans(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_SetBackColorFromTrans))(m); }
	inline void GteMVMVA_LightV0Bk() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_LightV0_Bk))(); }
	inline void GteSetRotDiag(int32_t s) { fn<void (__cdecl *)(int32_t)>(game_addr(game::GTE_SetRotDiag))(s); }
	inline void GteTransFromIR() { fn<void (__cdecl *)()>(game_addr(game::GTE_TransFromIR))(); }
	inline void RestoreDefaultEffectTexture(uint32_t tex, void *done) { fn<void (__cdecl *)(uint32_t, void *)>(game_addr(game::Battle_RestoreDefaultEffectTexture))(tex, done); }

#pragma pack(push, 1)
	struct RootNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t done;
	};
	struct EffectNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t w0E;
		int16_t w10, w12, w14, w16, w18, w1A, w1C, w1E, w20, w22;
	};
	struct Part
	{
		uint32_t type;
		int16_t frame;
		int16_t size;
		int16_t v[8];
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x10 && sizeof(EffectNode) == 0x24, "Breath nodes");
	static_assert(sizeof(Part) == 0x18, "Breath particle");

	inline Part *Parts() { return (Part *)game_addr(PARTS); }
	inline uint8_t *Caster() { return Entity((int)CasterSlot()); }
	inline uint8_t *Target() { return Entity((int)TargetSlot()); }

	static int FreePart()
	{
		int i = 0;
		for (uint32_t a = game_addr(PARTS); a < game_addr(PARTS_END); a += sizeof(Part), i++)
			if (*(uint32_t *)a == 0) return i < PART_COUNT ? i : -1;
		return -1;
	}

	static uint8_t *ModelHeader(uint32_t model, uint32_t flags)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		*(uint32_t *)h = model;
		*(int32_t *)(h + 8) = 0;
		*(int32_t *)(h + 0x1C) = (int32_t)flags;
		return h;
	}
	static void Render(uint8_t *h)
	{
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
	}
	static void ModelMatrix(Mat4x3 &m, const int16_t ang[3], int32_t s, const Mat4x3 *f)
	{
		ComposeZYXRotationMatrix(ang, &m);
		int32_t sc[3];
		sc[2] = s;
		sc[1] = s;
		sc[0] = s;
		m.t[2] = 0;
		m.t[1] = 0;
		m.t[0] = 0;
		Scale3DMatrix(&m, sc);
		ComposeAffineTransform(f, &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
	}

	static void SwirlMatrix(const EffectNode *t)
	{
		int16_t ang[3];
		ang[0] = 0;
		ang[1] = 0;
		ang[2] = t->w18;
		Mat4x3 m;
		ModelMatrix(m, ang, t->w1C, &FrameA());
	}
	static uint8_t *SwirlHeader(int16_t c)
	{
		uint8_t *h = ModelHeader(game_addr(MODEL_Swirl), 0x33);
		if (c < 4)
		{
			*(int32_t *)(h + 0xC) = (4 - (int32_t)c) << 10;
			*(int32_t *)(h + 0x1C) = 0xF3;
		}
		else if (c >= 0x16)
		{
			*(int32_t *)(h + 0xC) = ((int32_t)c - 0x16) << 9;
			*(int32_t *)(h + 0x1C) = 0xF3;
		}
		return h;
	}
	static void SwirlDraw(const EffectNode *t)
	{
		SwirlMatrix(t);
		Render(SwirlHeader(t->counter));
		FieldFree(0x58);
	}

	static void BurstMatrix(const EffectNode *t)
	{
		int16_t ang[3] = { 0, 0, 0 };
		Mat4x3 m;
		ModelMatrix(m, ang, t->w1C, &FrameB());
	}
	static uint8_t *BurstHeader(int16_t c)
	{
		uint8_t *h = ModelHeader(game_addr(MODEL_Burst), 0x33);
		if (c < 8)
		{
			*(int32_t *)(h + 0x1C) = 0xF3;
			*(int32_t *)(h + 0xC) = (8 - (int32_t)c) << 9;
		}
		return h;
	}
	static void BurstDraw(const EffectNode *t)
	{
		BurstMatrix(t);
		Render(BurstHeader(t->counter));
		FieldFree(0x58);
	}

	static void CloudModel1Matrix(const EffectNode *t, Mat4x3 &m)
	{
		const int16_t *c = CloudPos(t->w0E);
		int16_t ang[3];
		ang[0] = t->w10;
		ang[1] = 0;
		ang[2] = t->w18;
		ComposeZYXRotationMatrix(ang, &m);
		m.t[0] = c[0];
		m.t[2] = c[2];
		m.t[1] = c[1];
		int32_t sc[3];
		sc[2] = t->w1C;
		sc[1] = t->w1C;
		sc[0] = t->w1C;
		Scale3DMatrix(&m, sc);
		ComposeAffineTransform(&FrameB(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
	}
	static uint8_t *CloudHeader(int16_t c)
	{
		uint8_t *h = ModelHeader(game_addr(MODEL_Cloud1), 3);
		if (c >= 8)
		{
			*(int32_t *)(h + 0x1C) = 0xC3;
			*(int32_t *)(h + 0xC) = ((int32_t)c - 8) << 9;
		}
		return h;
	}
	static void CloudModel2(const EffectNode *t, Mat4x3 &m, uint8_t *h)
	{
		int16_t ang[3];
		ang[0] = 0;
		ang[1] = t->w10;
		ang[2] = 0;
		ComposeZYXRotationMatrix(ang, &m);
		const int32_t s2 = t->w1C + 0x2A0;
		int32_t sc[3];
		sc[2] = s2;
		sc[1] = s2;
		sc[0] = s2;
		Scale3DMatrix(&m, sc);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		*(uint32_t *)h = game_addr(MODEL_Cloud2);
		Render(h);
	}
	static void CloudModelsDraw(const EffectNode *t)
	{
		Mat4x3 m;
		CloudModel1Matrix(t, m);
		uint8_t *h = CloudHeader(t->counter);
		Render(h);
		FieldFree(0x58);
		CloudModel2(t, m, h);
	}

	static void Ring1Matrix(const EffectNode *t, Mat4x3 &m)
	{
		const int16_t *c = CloudPos(t->w0E);
		int16_t ang[3];
		ang[0] = t->w20;
		ang[1] = 0;
		ang[2] = t->w18;
		ComposeZYXRotationMatrix(ang, &m);
		m.t[2] = c[2];
		m.t[0] = c[0];
		m.t[1] = c[1];
		const int32_t s = t->w1C + 0xA00;
		int32_t sc[3];
		sc[2] = s;
		sc[1] = s;
		sc[0] = s;
		Scale3DMatrix(&m, sc);
		ComposeAffineTransform(&FrameB(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
	}
	static uint8_t *RingHeader(int16_t c)
	{
		uint8_t *h = ModelHeader(game_addr(MODEL_Ring), 0x33);
		if (c >= 8)
		{
			*(int32_t *)(h + 0x1C) = 0xF3;
			*(int32_t *)(h + 0xC) = ((int32_t)c - 8) * 409;
		}
		return h;
	}
	static void Ring2(const EffectNode *t, Mat4x3 &m, uint8_t *h)
	{
		int16_t ang[3];
		ang[0] = (int16_t)(t->w20 + 0x400);
		ang[1] = 0;
		ang[2] = t->w18;
		ComposeZYXRotationMatrix(ang, &m);
		const int32_t s = t->w1C + 0xA00;
		int32_t sc[3];
		sc[2] = s;
		sc[1] = s;
		sc[0] = s;
		Scale3DMatrix(&m, sc);
		ComposeAffineTransform(&FrameA(), &m, &m);
		GteSetRotMatrix(&m);
		Render(h);
	}
	static void RingsDraw(const EffectNode *t)
	{
		Mat4x3 m;
		Ring1Matrix(t, m);
		uint8_t *h = RingHeader(t->counter);
		Render(h);
		FieldFree(0x58);
		Ring2(t, m, h);
	}

	static void LitSpriteSetup(const Mat4x3 *frame)
	{
		GteSetRotMatrixCtrl(&SpriteFrame());
		GteSetLightMatrix(frame);
		GteSetBackColorFromTrans(frame);
	}
	static void LitSpriteAt(const int16_t *v, int32_t s)
	{
		GteLoadV0(v);
		GteMVMVA_LightV0Bk();
		GteSetRotDiag(s);
		GteReadMAC123(SpriteFrame().t);
	}
	static void LitSpritePull(int32_t s)
	{
		int32_t n[3];
		NormalizeVector(SpriteFrame().t, n);
		const int32_t k = -(s >> 3);
		SpriteFrame().t[0] += mul32(n[0], k) >> 12;
		SpriteFrame().t[1] += mul32(n[1], k) >> 12;
		SpriteFrame().t[2] += mul32(n[2], k) >> 12;
		GteSetTransVectorCtrl(&SpriteFrame());
	}
	static void SeqDraw(uint8_t *h)
	{
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
	}
	static uint8_t *SeqHeader(uint32_t seq)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		*(uint32_t *)h = seq;
		*(int16_t *)(h + 0x24) = 0;
		return h;
	}

	static void BreathPartDraw(uint8_t *h, const Part *p)
	{
		int16_t v[4];
		v[0] = (int16_t)(mul32(ComputeCos(p->v[0]), p->v[3]) >> 12);
		v[1] = (int16_t)(mul32(ComputeSin(p->v[0]), p->v[3]) >> 12);
		v[2] = p->v[2];
		LitSpriteAt(v, p->size);
		*(int16_t *)(h + 4) = p->frame;
		LitSpritePull(p->size);
		SeqDraw(h);
	}
	static void PuffDraw(const EffectNode *t)
	{
		LitSpriteSetup(&FrameB());
		LitSpriteAt(CloudPos(t->w0E), t->w1C);
		LitSpritePull(t->w1C);
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		*(int16_t *)(h + 4) = (int16_t)(t->counter & 3);
		*(uint32_t *)h = game_addr(SEQ_Puff);
		*(int16_t *)(h + 0x24) = 0;
		SeqDraw(h);
		FieldFree(0xB4);
	}
	static void PuffPartDraw(uint8_t *h, const Part *p)
	{
		GteLoadV0(p->v);
		GteMVMVA_LightV0Bk();
		GteSetRotDiag(p->size);
		*(int16_t *)(h + 4) = p->frame;
		GteReadMAC123(SpriteFrame().t);
		GteTransFromIR();
		SeqDraw(h);
	}
	static void SplatterDraw(uint8_t *h, const Part *p)
	{
		*(int16_t *)(h + 4) = p->frame;
		TransformCameraByShadowRotation(p->v, p->size, -((int32_t)p->size >> 4));
		SeqDraw(h);
	}
}
}

namespace ff8fx
{
namespace breath157
{
	static uint32_t __cdecl SwirlTask(TaskNode *nd)
	{
		EffectNode *t = (EffectNode *)nd;
		SwirlDraw(t);
		if (DrawOnly()) return 0;
		t->w18 = (int16_t)(t->w18 + t->w1A);
		t->counter++;
		return t->counter >= 0x1E ? TASK_END : 0;
	}

	static uint32_t __cdecl BurstTask(TaskNode *nd)
	{
		EffectNode *t = (EffectNode *)nd;
		BurstDraw(t);
		if (DrawOnly()) return 0;
		const int16_t d = t->w1E;
		t->w1C = (int16_t)(t->w1C - d);
		t->w1E = (int16_t)(d - (int16_t)(d / 24));
		t->counter++;
		return t->counter >= 0xA ? TASK_END : 0;
	}

	static uint32_t __cdecl PuffTask(TaskNode *nd)
	{
		EffectNode *t = (EffectNode *)nd;
		PuffDraw(t);
		if (DrawOnly()) return 0;
		int16_t *c = CloudPos(t->w0E);
		c[2] = (int16_t)(c[2] - t->w14);
		t->counter++;
		return t->counter >= 5 ? TASK_END : 0;
	}

	static uint32_t __cdecl CloudModelsTask(TaskNode *nd)
	{
		EffectNode *t = (EffectNode *)nd;
		CloudModelsDraw(t);
		if (DrawOnly()) return 0;
		int16_t *c = CloudPos(t->w0E);
		const int16_t v = t->w14;
		c[2] = (int16_t)(c[2] - v);
		t->w14 = (int16_t)(v - (int16_t)(v / 6));
		t->w10 = (int16_t)(t->w10 + t->w12);
		const int16_t g = t->w1E;
		t->w18 = (int16_t)(t->w18 + t->w1A);
		t->w1C = (int16_t)(t->w1C + g);
		t->w1E = (int16_t)(g - (int16_t)(g / 8));
		t->counter++;
		return t->counter >= 0x10 ? TASK_END : 0;
	}

	static uint32_t __cdecl RingsTask(TaskNode *nd)
	{
		EffectNode *t = (EffectNode *)nd;
		RingsDraw(t);
		if (DrawOnly()) return 0;
		const int16_t a = t->w1A;
		t->w18 = (int16_t)(t->w18 + a);
		t->w1A = (int16_t)(a - (int16_t)(a >> 4));
		const int16_t g = t->w1E;
		t->w1C = (int16_t)(t->w1C + g);
		t->w1E = (int16_t)(g - (int16_t)(g / 8));
		t->counter++;
		return t->counter >= 0x12 ? TASK_END : 0;
	}

	static uint32_t __cdecl BreathParticlesTask(TaskNode *nd)
	{
		EffectNode *t = (EffectNode *)nd;
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		int alive = 0;
		*(uint32_t *)h = game_addr(SEQ_Breath);
		*(int16_t *)(h + 0x24) = 0;
		LitSpriteSetup(&FrameA());
		for (Part *p = Parts(); (uint32_t)p < game_addr(PARTS_END); p++)
		{
			if (!(p->type & 1)) continue;
			BreathPartDraw(h, p);
			if (DrawOnly()) continue;
			p->frame++;
			if (*(const int16_t *)(h + 0x28) < 0)
			{
				p->type = 0;
				continue;
			}
			p->v[0] = (int16_t)(p->v[0] + p->v[1]);
			alive++;
		}
		FieldFree(0xB4);
		if (DrawOnly()) return 0;
		if (t->counter >= 0 && t->counter <= 0x1E)
		{
			for (int k = 0; k < 3; k++)
			{
				const int i = FreePart();
				if (i < 0) break;
				Part *p = &Parts()[i];
				p->type = 1;
				p->frame = 0;
				p->size = (int16_t)(CrtRand() % 0x400 + 0x300);
				p->v[0] = (int16_t)(CrtRand() % 0x1000);
				p->v[1] = (int16_t)(CrtRand() % 0x32 + 0xA);
				p->v[2] = (int16_t)(-10 - CrtRand() % 0x258);
				p->v[3] = (int16_t)(CrtRand() % 0x514 + 0x12C);
			}
		}
		t->counter++;
		if (t->counter >= 4 && alive == 0) return TASK_END;
		return 0;
	}

	static uint32_t __cdecl PuffParticlesTask(TaskNode *nd)
	{
		EffectNode *t = (EffectNode *)nd;
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		int alive = 0;
		*(uint32_t *)h = game_addr(SEQ_PuffPart);
		*(int16_t *)(h + 0x24) = 0;
		LitSpriteSetup(&FrameB());
		for (Part *p = Parts(); (uint32_t)p < game_addr(PARTS_END); p++)
		{
			if (!(p->type & 2)) continue;
			PuffPartDraw(h, p);
			if (DrawOnly()) continue;
			p->frame++;
			if (*(const int16_t *)(h + 0x28) < 0) p->type = 0;
			else alive++;
		}
		FieldFree(0xB4);
		if (DrawOnly()) return 0;
		if (t->counter >= 0 && t->counter <= 4)
		{
			const int32_t spread = (int32_t)t->counter * 30 + 0x15E;
			const int16_t *c = CloudPos(t->w0E);
			for (int k = 0; k < 6; k++)
			{
				const int i = FreePart();
				if (i < 0) break;
				Part *p = &Parts()[i];
				p->type = 2;
				p->frame = 0;
				const int32_t half = spread >> 1;
				p->size = (int16_t)(CrtRand() % 0x380 + 0x200);
				*(uint32_t *)&p->v[0] = *(const uint32_t *)&c[0];
				*(uint32_t *)&p->v[2] = *(const uint32_t *)&c[2];
				p->v[0] = (int16_t)(p->v[0] + (CrtRand() % spread - half));
				p->v[1] = (int16_t)(p->v[1] + (CrtRand() % spread - half));
				p->v[2] = (int16_t)(p->v[2] + CrtRand() % (int32_t)t->w14);
			}
		}
		t->counter++;
		if (t->counter >= 4 && alive == 0) return TASK_END;
		return 0;
	}

	static uint32_t __cdecl SplatterTask(TaskNode *nd)
	{
		EffectNode *t = (EffectNode *)nd;
		uint8_t *h = SeqHeader(game_addr(SEQ_Splatter));
		int alive = 0;
		for (Part *p = Parts(); (uint32_t)p < game_addr(PARTS_END); p++)
		{
			if (!(p->type & 4)) continue;
			SplatterDraw(h, p);
			if (DrawOnly()) continue;
			p->frame++;
			if (*(const int16_t *)(h + 0x28) < 0) p->type = 0;
			else alive++;
		}
		FieldFree(0xB4);
		if (DrawOnly()) return 0;
		if (t->counter >= 0 && t->counter <= 4)
		{
			for (int k = 0; k < 6; k++)
			{
				const int i = FreePart();
				if (i < 0) break;
				Part *p = &Parts()[i];
				p->type = 4;
				p->frame = 0;
				p->size = (int16_t)(CrtRand() % 0xB00 + 0x600);
				*(uint32_t *)&p->v[0] = *(const uint32_t *)&TargetPos()[0];
				*(uint32_t *)&p->v[2] = *(const uint32_t *)&TargetPos()[2];
				p->v[0] = (int16_t)(p->v[0] + (CrtRand() % 0x4B0 - 0x258));
				p->v[1] = (int16_t)(p->v[1] + (CrtRand() % 0x4B0 - 0x258));
				p->v[2] = (int16_t)(p->v[2] + (CrtRand() % 0x4B0 - 0x258));
			}
		}
		t->counter++;
		if (t->counter >= 4 && alive == 0) return TASK_END;
		return 0;
	}

	static uint32_t __cdecl CloudTask(TaskNode *nd)
	{
		if (DrawOnly()) return 0;
		EffectNode *t = (EffectNode *)nd;
		if (t->counter == 0)
		{
			int16_t *c = CloudPos(t->w0E);
			c[2] = 0;
			c[1] = 0;
			c[0] = 0;
			EffectNode *n = (EffectNode *)AddTaskToQueue(EffectQueue(), game_addr(ORIG_Burst));
			n->counter = 0;
			n->w1C = 0x4000;
			n->w1E = 0x800;
		}
		if (t->counter == 8)
		{
			EffectNode *n = (EffectNode *)AddTaskToQueue(EffectQueue(), game_addr(ORIG_Puff));
			n->w0E = t->w0E;
			n->counter = 0;
			n->w1C = 0xD00;
			const int16_t v = (int16_t)((shl32(mul32(LenB(), 11), 8) >> 12) / 4);
			n->w14 = v;
			t->w14 = v;
		}
		if (t->counter == 9)
		{
			EffectNode *n = (EffectNode *)AddTaskToQueue(EffectQueue(), game_addr(ORIG_PuffParticles));
			n->counter = 0;
			n->w0E = t->w0E;
			n->w14 = t->w14;
		}
		if (t->counter == 0xC)
		{
			EffectNode *n = (EffectNode *)AddTaskToQueue(EffectQueue(), game_addr(ORIG_CloudModels));
			n->counter = 0;
			n->w0E = t->w0E;
			n->w10 = (int16_t)(CrtRand() % 0x1000);
			n->w12 = (int16_t)(CrtRand() % 0x46 + 0x19);
			const int32_t r = CrtRand() % 0x258;
			n->w14 = (int16_t)((shl32(mul32(r + LenB() - 0x12C, 5), 8) >> 12) / 5);
			n->w18 = (int16_t)(CrtRand() % 0x1000);
			n->w1A = (int16_t)(CrtRand() % 0x46 + 0x19);
			const int16_t g = (int16_t)((CrtRand() % 0x600 + 0x700) / 8);
			n->w1E = g;
			n->w1C = g;
			EffectNode *m = (EffectNode *)AddTaskToQueue(EffectQueue(), game_addr(ORIG_Rings));
			m->counter = 0;
			m->w0E = t->w0E;
			m->w18 = (int16_t)(CrtRand() % 0x1000);
			const int32_t spin = CrtRand() % 0x32;
			m->w1E = n->w1E;
			m->w1C = n->w1E;
			m->w1A = (int16_t)(spin + 0x14);
			m->w20 = (int16_t)(CrtRand() % 0x1000);
		}
		if (t->counter == 0xF)
		{
			EffectNode *n = (EffectNode *)AddTaskToQueue(EffectQueue(), game_addr(ORIG_Splatter));
			n->counter = 0;
		}
		t->counter++;
		return t->counter >= 0x16 ? TASK_END : 0;
	}

	static void Mouth(int16_t *mouth, int32_t *dir, int32_t &len, int32_t off_y, int32_t off_z)
	{
		GetEffectSpawnPosition(Caster(), 0xF0, 0x1000, mouth);
		int32_t v[3];
		v[0] = 0;
		v[1] = off_y;
		v[2] = off_z;
		TransformVectorBy3x3Matrix((const Mat4x3 *)(Caster() + 0x40), v, v);
		mouth[0] = (int16_t)(mouth[0] + v[0]);
		mouth[1] = (int16_t)(mouth[1] + v[1]);
		mouth[2] = (int16_t)(mouth[2] + v[2]);
		dir[1] = TargetPos()[1] - mouth[1];
		dir[0] = TargetPos()[0] - mouth[0];
		dir[2] = TargetPos()[2] - mouth[2];
		len = SqrtInt(NormalizeVectorLen(dir, dir));
	}
	static void BreathFrame(Mat4x3 &f, const int32_t *dir, const int16_t *mouth)
	{
		int32_t axis[3];
		const int32_t angle = RotationBetweenVectors((const int32_t *)game_addr(REF_DIR), dir, axis);
		BuildAxisAngleRotationMatrix(angle, &f, axis);
		f.t[0] = mouth[0];
		f.t[1] = mouth[1];
		f.t[2] = mouth[2];
		ComposeAffineTransform(&Camera(), &f, &f);
	}

	static uint32_t __cdecl DirectorTask(TaskNode *nd)
	{
		BreathFrame(FrameA(), DirA(), MouthA());
		BreathFrame(FrameB(), DirB(), MouthB());
		if (DrawOnly()) return 0;
		EffectNode *t = (EffectNode *)nd;
		if (t->counter == 0)
		{
			GetDefaultEffectPosition(Target(), TargetPos());
			Mouth(MouthA(), DirA(), LenA(), 0x226, -0x280);
		}
		if (t->counter == 1)
		{
			EffectNode *n = (EffectNode *)AddTaskToQueue(EffectQueue(), game_addr(ORIG_Swirl));
			n->counter = 0;
			n->w18 = (int16_t)(CrtRand() % 0x1000);
			const int32_t r = CrtRand() % 0x32;
			n->w1C = 0x1400;
			n->w1A = (int16_t)(r + 0x1E);
			n = (EffectNode *)AddTaskToQueue(EffectQueue(), game_addr(ORIG_Swirl));
			n->counter = 0;
			n->w18 = (int16_t)(CrtRand() % 0x1000);
			const int32_t r2 = CrtRand() % 0x3C;
			n->w1C = 0xF00;
			n->w1A = (int16_t)(-0x14 - r2);
		}
		if (t->counter == 2)
		{
			EffectNode *n = (EffectNode *)AddTaskToQueue(EffectQueue(), game_addr(ORIG_BreathParticles));
			n->counter = 0;
		}
		if (t->counter == 0x13) Mouth(MouthB(), DirB(), LenB(), 0x384, -0x546);
		if (t->counter == 0x14 || t->counter == 0x20 || t->counter == 0x2B)
		{
			EffectNode *n = (EffectNode *)AddTaskToQueue(EffectQueue(), game_addr(ORIG_Cloud));
			n->w0E = t->w0E;
			n->counter = 0;
			t->w0E++;
		}
		if (t->counter == 0x41) ApplyActionResultToTarget(Ctx()->actions[0].targets);
		if (t->counter == 1) BdPlaySE(SOUND_Start(), 0, 0x80);
		if (t->counter == 0x14) BdPlaySE(SOUND_Cloud(), 0, 0x80);
		t->counter++;
		return t->counter > 0x46 ? TASK_END : 0;
	}

	static uint32_t __cdecl RootTask(TaskNode *nd)
	{
		RootNode *r = (RootNode *)nd;
		if (r->counter & 1) PacketCursor() = TexBase() + ARENA_SIZE;
		else PacketCursor() = TexBase();
		const int a = ExecuteTaskQueue(EffectQueue());
		if (!a)
		{
			RootNode *w = (RootNode *)AddTaskToQueue(RootQueue(), game_addr(ORIG_TexRestore));
			w->counter = 0;
			w->done = 0;
		}
		r->counter++;
		return a ? 0 : TASK_END;
	}

	static uint32_t __cdecl TexRestoreTask(TaskNode *nd)
	{
		RootNode *r = (RootNode *)nd;
		if (r->counter == 1) RestoreDefaultEffectTexture(TexBase(), &r->done);
		const int16_t done = r->done;
		r->counter++;
		return done ? TASK_END : 0;
	}
}

	void register_mag157_breath()
	{
		register_port(breath157::ORIG_RootTask, (void *)breath157::RootTask, "B157 RootTask", 157);
		register_port(breath157::ORIG_TexRestore, (void *)breath157::TexRestoreTask, "B157 TexRestore", 157);
		register_port(breath157::ORIG_Director, (void *)breath157::DirectorTask, "B157 Director", 157);
		register_port(breath157::ORIG_Swirl, (void *)breath157::SwirlTask, "B157 Swirl", 157);
		register_port(breath157::ORIG_BreathParticles, (void *)breath157::BreathParticlesTask, "B157 BreathParticles", 157);
		register_port(breath157::ORIG_Cloud, (void *)breath157::CloudTask, "B157 Cloud", 157);
		register_port(breath157::ORIG_Burst, (void *)breath157::BurstTask, "B157 Burst", 157);
		register_port(breath157::ORIG_Puff, (void *)breath157::PuffTask, "B157 Puff", 157);
		register_port(breath157::ORIG_PuffParticles, (void *)breath157::PuffParticlesTask, "B157 PuffParticles", 157);
		register_port(breath157::ORIG_CloudModels, (void *)breath157::CloudModelsTask, "B157 CloudModels", 157);
		register_port(breath157::ORIG_Rings, (void *)breath157::RingsTask, "B157 Rings", 157);
		register_port(breath157::ORIG_Splatter, (void *)breath157::SplatterTask, "B157 Splatter", 157);
	}
}

