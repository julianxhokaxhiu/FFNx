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

#include "mag_common.h"

namespace ff8fx
{
namespace ultimecia227
{
	using namespace eng;
	using namespace magc;

	inline uint32_t &TexFile() { return var<uint32_t>(game_addr(game::MAG_227_UltimeciaFirstDeath_TextureFile)); }
	inline uint8_t *&SparkCursor() { return var<uint8_t *>(game_addr(game::MAG_227_UltimeciaFirstDeath_SparkCursor)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_227_UltimeciaFirstDeath_PacketCursor)); }
	inline uint32_t &PacketCursor2() { return var<uint32_t>(game_addr(game::MAG_227_UltimeciaFirstDeath_PacketCursor2)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::MAG_227_UltimeciaFirstDeath_CastContext)); }
	inline uint8_t *&Caster() { return var<uint8_t *>(game_addr(game::MAG_227_UltimeciaFirstDeath_CasterEntity)); }
	inline TaskQueue *QFeathers() { return (TaskQueue *)game_addr(game::MAG_227_UltimeciaFirstDeath_FeathersQueue); }
	inline TaskQueue *QMain() { return (TaskQueue *)game_addr(game::MAG_227_UltimeciaFirstDeath_MainQueue); }
	inline int16_t *CasterPoint() { return (int16_t *)game_addr(game::MAG_227_UltimeciaFirstDeath_CasterPoint); }
	inline int16_t *LayoutPoint() { return (int16_t *)game_addr(game::MAG_227_UltimeciaFirstDeath_LayoutPoint); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::MAG_227_UltimeciaFirstDeath_MagicBufferBase)); }

	static constexpr game::Id ORIG_MasterTask = game::MAG_227_UltimeciaFirstDeath_MasterTask;
	static constexpr game::Id ORIG_MainTask = game::MAG_227_UltimeciaFirstDeath_MainTask;
	static constexpr game::Id ORIG_FeatherTask = game::MAG_227_UltimeciaFirstDeath_FeatherTask;
	static constexpr game::Id ORIG_PrimCallback = game::MAG_227_UltimeciaFirstDeath_PrimCallback;
	static const uint32_t POOL_Main = 0x180;
	static const uint32_t SPARKS = 0x61C;
	static const uint32_t SPARKS_LAST = 0xE0C;
	static const uint32_t MORPH = 0xE1C;
	static const uint32_t ARENA_A = 0xE24;
	static const uint32_t ARENA_B = 0x10E24;
	static const uint32_t ARENA_C = 0x20E24;
	static constexpr game::Id LAYOUT_A = game::MAG_227_UltimeciaFirstDeath_LayoutA;
	static constexpr game::Id LAYOUT_B = game::MAG_227_UltimeciaFirstDeath_LayoutB;
	static constexpr game::Id LAYOUT_C = game::MAG_227_UltimeciaFirstDeath_LayoutC;
	static constexpr game::Id MODEL_Feather = game::MAG_227_UltimeciaFirstDeath_ModelFeather;
	static constexpr game::Id SEQ_Spark = game::MAG_227_UltimeciaFirstDeath_SeqSpark;
	static constexpr game::Id POINT_Caster = game::unk_1132018;
	static constexpr game::Id POINTS_A = game::unk_1131660, POINTS_B = game::unk_1131960, POINTS_C = game::unk_1131C60;
	static const void *SOUND_Start() { return (const void *)game_addr(game::MAG_227_UltimeciaFirstDeath_SoundStart); }
	static const void *SOUND_Fall() { return (const void *)game_addr(game::MAG_227_UltimeciaFirstDeath_SoundFall); }
	static const void *SOUND_Voice() { return (const void *)game_addr(game::MAG_227_UltimeciaFirstDeath_SoundVoice); }

	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void RotationFromAnglesB(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::MAG_063_RotationFromAnglesYXZ))(angles, out); }
	inline void RotY(int32_t angle, Mat4x3 *out) { fn<void (__cdecl *)(int32_t, Mat4x3 *)>(game_addr(game::Effect_RotationYMatrix))(angle, out); }
	inline void RotZ(int32_t angle, Mat4x3 *out) { fn<void (__cdecl *)(int32_t, Mat4x3 *)>(game_addr(game::MAG_065_RotationZMatrix))(angle, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void MulMatrix3(Mat4x3 *a, const void *b) { fn<void (__cdecl *)(Mat4x3 *, const void *)>(game_addr(game::MatrixMultiply3x3))(a, b); }
	inline void BlendVectors(const int16_t *a, const int16_t *b, int32_t wa, int32_t wb, int16_t *out) { fn<void (__cdecl *)(const int16_t *, const int16_t *, int32_t, int32_t, int16_t *)>(game_addr(game::VecLerp))(a, b, wa, wb, out); }
	inline void GteMVMVA_RotV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0))(); }
	inline uint32_t RenderPrim2(void *h, uint32_t ot, int mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int, uint32_t)>(game_addr(game::MAG_070_RenderPrimModel))(h, ot, mode, cursor); }
	inline uint32_t Flipbook(void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::MAG_065_RenderFlipbook))(h, ot, mode, cursor); }
	inline void ReleaseVoiceSlot(uint32_t slot) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::BdSound_ReleaseVoiceSlot))(slot); }
	inline void BattleText(int32_t slot, int32_t line, int32_t ticks) { fn<void (__cdecl *)(int32_t, int32_t, int32_t)>(game_addr(game::printMonsterRelatedText))(slot, line, ticks); }
	inline uint32_t OT() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)); }
	template<typename T> static inline T &F(uint8_t *p, uint32_t off) { return *(T *)(p + off); }

#pragma pack(push, 1)
	struct MasterNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t arena;
		uint8_t started;
	};
	struct MainNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		uint32_t voice;
		int16_t from[4];
		uint8_t layout_a[0x33C];
		uint8_t layout_b[0xC0];
		uint8_t layout_c[0x84];
	};
	struct FeatherNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t rz;
		int16_t ry;
		int16_t dry;
		int16_t drz;
		int16_t pad16;
	};
	struct Spark
	{
		const uint8_t *seq;
		int16_t pos[3];
		int16_t scale;
		int8_t vel[3];
		uint8_t life;
	};
	struct PrimArg
	{
		int16_t pos[4];
		int32_t depth;
		uint32_t morph;
	};
#pragma pack(pop)
	static_assert(sizeof(MasterNode) == 0x10 && sizeof(MainNode) == 0x49C && sizeof(FeatherNode) == 0x18, "227 nodes");
	static_assert(sizeof(Spark) == 0x10 && sizeof(PrimArg) == 0x10, "227 blocks");

	static const int16_t CallbackPad = 0;

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
		if (r->flags & 0x200)
		{
			GteSetRotMatrixCtrl(&Camera());
			GteLoadV0(arg->pos);
			GteMVMVA_RotV0();
			GteReadMAC123(m.t);
			m.t[0] = (int32_t)((uint32_t)m.t[0] + (uint32_t)(int32_t)r->pos[0]);
			m.t[1] = (int32_t)((uint32_t)m.t[1] + (uint32_t)(int32_t)r->pos[1]);
			m.t[2] = (int32_t)((uint32_t)m.t[2] + (uint32_t)(int32_t)r->pos[2]);
		}
		else
		{
			const int16_t off[4] = { (int16_t)(r->pos[0] + arg->pos[0]), (int16_t)(r->pos[1] + arg->pos[1]), (int16_t)(r->pos[2] + arg->pos[2]), 0 };
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
			if (r->flags & 0x100)
			{
				Mat4x3 d = {};
				d.m[0][0] = r->scale[0];
				d.m[1][1] = r->scale[1];
				d.m[2][2] = r->scale[2];
				MulMatrix3(&m, &d);
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
		*(int32_t *)(h + 0x18) = arg->depth;
		PacketCursor() = RenderPrim2(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static void ModelPoint(const int16_t *v, int16_t *out)
	{
		uint8_t *caster = Caster();
		const uint8_t *bones = **(uint8_t *const *const *)(caster + 0x64) + 0x10;
		BuildBoneMatricesFromPose(caster + 0x60);
		Mat4x3 m;
		ComposeAffineTransform((const Mat4x3 *)(caster + 0x40), (const Mat4x3 *)(bones + (uint32_t)mul32(v[3], 0x30) + 0x10), &m);
		GteSetRotMatrixCtrl(&m);
		GteSetTransVectorCtrl(&m);
		GteLoadV0(v);
		GteMVMVA_RotV0Tr();
		GteStoreIR123(out);
	}

	static void ClearSparks()
	{
		uint8_t *p = (uint8_t *)TexBase() + SPARKS + 0xF;
		for (int k = 0x80; k != 0; k--, p += 0x10) *p = 0;
		SparkCursor() = (uint8_t *)TexBase() + SPARKS;
	}
	static Spark *SparkAlloc()
	{
		uint8_t *p = SparkCursor();
		const uint32_t tex = TexBase();
		if (p[0xF] != 0)
		{
			p = (uint8_t *)tex + SPARKS;
			if (p[0xF] != 0)
			{
				int32_t n = 0x80;
				do
				{
					p += 0x10;
					if (--n == 0) return nullptr;
				} while (p[0xF] != 0);
			}
		}
		if ((uint32_t)p >= tex + SPARKS_LAST) SparkCursor() = (uint8_t *)tex + SPARKS;
		else SparkCursor() = p + 0x10;
		return (Spark *)p;
	}
	static void SparkSpawn(const int16_t *points, int32_t count)
	{
		Spark *s = SparkAlloc();
		if (!s) return;
		const int32_t i = mul32(CrtRand(), count) / 0x8000;
		ModelPoint(points + i * 4, s->pos);
		s->seq = (const uint8_t *)game_addr(SEQ_Spark);
		s->vel[0] = (int8_t)((CrtRand() & 3) - 2);
		s->vel[2] = (int8_t)((CrtRand() & 3) - 2);
		const int8_t vy = (int8_t)((CrtRand() & 3) + 3);
		s->scale = 0x200;
		s->life = 0x12;
		s->vel[1] = vy;
	}
	static void SparksUpdate()
	{
		Spark *s = (Spark *)(TexBase() + SPARKS);
		for (int k = 0x80; k != 0; k--, s++)
		{
			if (s->life == 0) continue;
			s->life--;
			s->pos[0] = (int16_t)(s->pos[0] + (int16_t)((int16_t)s->vel[0] << 2));
			s->pos[1] = (int16_t)(s->pos[1] + (int16_t)((int16_t)s->vel[1] << 2));
			s->pos[2] = (int16_t)(s->pos[2] + (int16_t)((int16_t)s->vel[2] << 2));
		}
	}
	static void SparksDraw(const Spark *s)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		F<uint16_t>(h, 0x60) = 0;
		F<uint16_t>(h, 0x58) = 0;
		F<uint16_t>(h, 0x50) = 0;
		F<uint16_t>(h, 0x48) = 0;
		for (int n = 0x80; n != 0; n--, s++)
		{
			if (s->life == 0) continue;
			GteSetRotMatrixCtrl(&Camera());
			GteSetTransVectorCtrl(&Camera());
			GteLoadV0(s->pos);
			GteMVMVA_RotV0Tr();
			GteReadMAC123((int32_t *)(h + 0x98));
			ff8fx::fill<(0x14)>(h + 0x84, 0);
			F<uint16_t>(h, 0x94) = (uint16_t)s->scale;
			F<uint16_t>(h, 0x8C) = (uint16_t)s->scale;
			F<uint16_t>(h, 0x84) = (uint16_t)s->scale;
			const uint8_t *seq = s->seq;
			*(const uint8_t **)h = seq;
			const int32_t frames = *(const int16_t *)(seq + 8);
			const int32_t k = frames - ((int32_t)(int8_t)s->life - 1) % frames;
			const uint8_t *frame = seq + *(const int16_t *)(seq + 8 + 2 * k);
			*(const uint8_t **)(h + 0x2C) = frame;
			const int32_t word = *(const int32_t *)frame;
			F<int32_t>(h, 0x30) = word;
			if (word < 0)
			{
				h[0x25] |= 1;
				F<uint32_t>(h, 0x30) = (uint32_t)word & 0x7FFFFFFF;
			}
			*(const uint8_t **)(h + 0x2C) = frame + 4;
			PacketCursor() = Flipbook(h, OT() + 0x44, 2, PacketCursor());
		}
		FieldFree(0xB4);
	}

	static int32_t FeatherFade(int32_t c)
	{
		if (c > 0xA) return shl32(c - 0xA, 12) / 6;
		if (c < 3) return shl32(3 - c, 10);
		return 0;
	}
	static void FeatherScale(int32_t c, int16_t &sx, int16_t &sy)
	{
		const int32_t s = ComputeSin((shl32(c, 12) / 16) >> 2);
		const int16_t a = (int16_t)(s + 0x1000);
		sx = (int16_t)(a + (int16_t)(a >> 1));
		sy = (int16_t)(s + s);
	}
	static void FeatherDraw(int32_t fade, int16_t sx, int16_t sy, int16_t ry, int16_t rz, const int32_t *t)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		Mat4x3 m, ym, d;
		m.t[0] = t[0];
		m.t[1] = t[1];
		m.t[2] = t[2];
		*(uint32_t *)(h + 0x1C) = 0xF3;
		*(uint32_t *)(h + 8) = 0;
		*(int32_t *)(h + 0xC) = fade;
		ff8fx::fill<(0x12)>(&d, 0);
		d.m[0][0] = sx;
		d.m[1][1] = sy;
		d.m[2][2] = 0x1000;
		RotY(ry, &ym);
		MulMatrix3(&ym, &d);
		RotZ(rz, &m);
		MatrixMultiply(&ym, &m);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		*(uint32_t *)h = game_addr(MODEL_Feather);
		*(int32_t *)(h + 0x18) = -0x80;
		PacketCursor() = RenderPrim2(h, OT() + 0x44, 2, PacketCursor());
		FieldFree(0x58);
	}
	static void FeatherAt(const int16_t *point, int32_t *t)
	{
		t[0] = point[0];
		t[1] = (int32_t)point[1] - 0xC8;
		t[2] = point[2];
	}

	static void LayoutAPos(const MainNode *m, int32_t c, const int16_t *point, int16_t *pos)
	{
		if ((uint32_t)(c - 0xD4) < 0x32)
		{
			const int32_t k = c - 0xD4;
			int32_t dy = -0xC8;
			if (k > 0x2D) dy = (k * 5 - 0xFA) * 5 * 8 / 5;
			pos[0] = point[0];
			pos[1] = (int16_t)(point[1] + dy);
			pos[2] = point[2];
		}
		else
		{
			const int32_t t = (int32_t)(((uint32_t)(c - 0x106) << 12) >> 3) + 0x100;
			BlendVectors(m->from, LayoutPoint(), 0x1000 - t, t, pos);
		}
	}
}
}

namespace ff8fx
{
namespace ultimecia227
{
	static uint32_t __cdecl FeatherTask(TaskNode *n)
	{
		FeatherNode *f = (FeatherNode *)n;
		int32_t t[3];
		FeatherAt(CasterPoint(), t);
		int16_t sx, sy;
		FeatherScale(f->counter, sx, sy);
		FeatherDraw(FeatherFade(f->counter), sx, sy, f->ry, f->rz, t);
		if (f->counter >= 0x10) return TASK_END;
		f->ry = (int16_t)(f->ry + f->dry);
		f->rz = (int16_t)(f->rz + f->drz);
		f->counter++;
		return 0;
	}

	static void Play(MainNode *m, int which, uint8_t *layout, const int16_t *pos, int32_t depth)
	{
		PrimArg arg;
		arg.pos[0] = pos[0];
		arg.pos[1] = pos[1];
		arg.pos[2] = pos[2];
		arg.pos[3] = CallbackPad;
		arg.depth = depth;
		arg.morph = TexBase() + MORPH;
		prim::play((prim::Layout *)layout, PrimCallback, (int)&arg, 0);
	}

	static uint32_t __cdecl MainTask(TaskNode *n)
	{
		MainNode *m = (MainNode *)n;
		const int32_t c = m->counter;
		int16_t pos[3];
		if ((uint32_t)(c - 0xD4) < 0x32)
		{
			if (c == 0xD4) DecodeModelPrimLayout(game_addr(LAYOUT_A), m->layout_a, 0x33C);
			LayoutAPos(m, c, CasterPoint(), pos);
			Play(m, 0, m->layout_a, pos, -0x80);
		}
		if ((uint32_t)(c - 0x106) < 8)
		{
			if (c == 0x106)
			{
				*(uint32_t *)&m->from[0] = *(const uint32_t *)&CasterPoint()[0];
				*(uint32_t *)&m->from[2] = *(const uint32_t *)&CasterPoint()[2];
				const uint8_t *e = Caster();
				LayoutPoint()[1] = 0;
				LayoutPoint()[0] = *(const int16_t *)(e + 0x1C);
				LayoutPoint()[2] = (int16_t)(*(const int16_t *)(e + 0x20) + 0x3E8);
			}
			LayoutAPos(m, c, CasterPoint(), pos);
			Play(m, 0, m->layout_a, pos, -0x80);
		}
		if ((uint32_t)(c - 0x104) < 0x12)
		{
			if (c == 0x104) DecodeModelPrimLayout(game_addr(LAYOUT_B), m->layout_b, 0xC0);
			const uint8_t *e = Caster();
			pos[0] = (int16_t)(*(const int16_t *)(e + 0x1C) + 0x2C);
			pos[1] = (int16_t)(*(const int16_t *)(e + 0x1E) - 0x8EE);
			pos[2] = (int16_t)(*(const int16_t *)(e + 0x20) + 0x19A);
			Play(m, 1, m->layout_b, pos, -0x80);
		}
		if ((uint32_t)(c - 0x10C) < 0x10)
		{
			if (c == 0x10C) DecodeModelPrimLayout(game_addr(LAYOUT_C), m->layout_c, 0x84);
			PrimArg arg;
			ff8fx::copy<(8)>(arg.pos, LayoutPoint());
			arg.depth = 0;
			arg.morph = TexBase() + MORPH;
			prim::play((prim::Layout *)m->layout_c, PrimCallback, (int)&arg, 0);
		}
		SparksDraw((const Spark *)(TexBase() + SPARKS));
		SparksUpdate();
		if (m->counter == 0) BdPlaySE(SOUND_Start(), 0, 0x80);
		else if (m->counter == 0xD4) BdPlaySE(SOUND_Fall(), 0, 0x80);
		if (m->counter == 0x118) ReleaseVoiceSlot(m->voice);
		if ((uint16_t)m->counter < 0x5F)
		{
			for (int i = 2; i != 0; i--)
			{
				SparkSpawn((const int16_t *)game_addr(POINTS_A), 0x60);
				SparkSpawn((const int16_t *)game_addr(POINTS_B), 0x60);
			}
		}
		if ((uint32_t)(m->counter - 0xB4) < 0x16)
		{
			for (int i = 2; i != 0; i--) SparkSpawn((const int16_t *)game_addr(POINTS_C), 0x77);
		}
		if ((uint32_t)(m->counter - 0xDE) < 0x17)
		{
			FeatherNode *f = (FeatherNode *)AddTaskToQueue(QFeathers(), game_addr(ORIG_FeatherTask));
			if (f)
			{
				Memset32(&f->counter, 0, 3);
				f->rz = (int16_t)CrtRand();
				f->ry = (int16_t)CrtRand();
				f->dry = 0x10;
				if (CrtRand() & 1) f->dry = (int16_t)-f->dry;
				f->drz = (int16_t)(-0x25 - (CrtRand() & 0x1F));
			}
		}
		const int32_t slot = (int32_t)((uint32_t)Caster() - game_addr(game::BattleEntitySlotData)) / 0x9C;
		switch (m->counter)
		{
		case 0x26: BattleText(slot, 0, 0x2A); break;
		case 0x55: BattleText(slot, 1, 0x1E); break;
		case 0x73: BattleText(slot, 2, 0x1E); break;
		case 0xA5: BattleText(slot, 3, 0xF); break;
		}
		if (m->counter >= 0x11C)
		{
			*Caster() |= 4;
			return TASK_END;
		}
		m->counter++;
		return 0;
	}

	static uint32_t __cdecl MasterTask(TaskNode *n)
	{
		MasterNode *ms = (MasterNode *)n;
		if (ms->arena)
		{
			PacketCursor() = TexBase() + ARENA_A;
			PacketCursor2() = TexBase() + ARENA_B;
			ms->arena = 0;
		}
		else
		{
			PacketCursor() = TexBase() + ARENA_B;
			PacketCursor2() = TexBase() + ARENA_C;
			ms->arena = 1;
		}
		if (ms->counter == 1 && !ms->started)
		{
			ms->started = 1;
			ClearSparks();
			InitTaskQueuePool(QMain(), (void *)(TexBase() + POOL_Main), 0x49C, 1);
			InitTaskQueuePool(QFeathers(), (void *)TexBase(), 0x18, 0x10);
			MainNode *m = (MainNode *)AddTaskToQueue(QMain(), game_addr(ORIG_MainTask));
			Memset32(&m->counter, 0, 0x124);
			m->voice = ClaimVoiceSlot(SOUND_Voice(), 1, 0x80);
		}
		int main = (int)n;
		if (ms->started)
		{
			ModelPoint((const int16_t *)game_addr(POINT_Caster), CasterPoint());
			main = ExecuteTaskQueue(QMain());
			ExecuteTaskQueue(QFeathers());
		}
		if (ms->started && main == 0)
		{
			return TASK_END;
		}
		ms->counter++;
		return 0;
	}
}

	void register_mag227_ultimecia_first_death()
	{
		register_port(ultimecia227::ORIG_MasterTask, (void *)ultimecia227::MasterTask, "227 MasterTask", 227);
		register_port(ultimecia227::ORIG_MainTask, (void *)ultimecia227::MainTask, "227 MainTask", 227);
		register_port(ultimecia227::ORIG_FeatherTask, (void *)ultimecia227::FeatherTask, "227 FeatherTask", 227);
	}
}

