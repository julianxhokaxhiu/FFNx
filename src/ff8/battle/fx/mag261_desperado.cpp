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

#include "gfc_engine.h"

namespace ff8fx
{
namespace gfc
{
namespace desperado261
{
	static Clone g_c = {};
}
}
}

namespace ff8fx
{
namespace gfc
{
	void apply_brothers(Clone &c);

namespace desperado261
{
	static void describe(Clone &c)
	{
		c.name = "261 Desperado";
		c.effect_id = 261;
		c.vm_table = game_addr(game::MAG261_VmOpcodeTable);
		c.bone_table = game_addr(game::MAG261_BoneHandlerTable);
		c.prim_table = game_addr(game::funcs_95CA74);
		c.prim_size = game_addr(game::unk_1858F44);
		c.spr_table = game_addr(game::unk_1858F58);
		c.ptype_table = game_addr(game::unk_1858F84);
		c.pop_table = game_addr(game::funcs_95E877);
		c.draw_table = game_addr(game::MAG261_DrawHandlerTable);
		c.attr_16A = game_addr(game::unk_1859162);
		c.attr_184 = game_addr(game::unk_185917C);
		c.attr_186 = game_addr(game::unk_185917E);
		c.attr_194 = game_addr(game::unk_185918C);
		c.attr_19C = game_addr(game::unk_1859194);
		c.desc = game_addr(game::MAG261_ResourceDesc);
		c.ptr_block = game_addr(game::MAG261_PtrBlock);
		c.queue = game_addr(game::g_MAG261_RootQueue);
		c.file00 = game_addr(game::g_MAG261_File00);
		c.file01 = game_addr(game::g_MAG261_File01);
		c.tick = game_addr(game::MAG_261_SequenceTick);
		c.load_cb = game_addr(game::MAG_261_LoadCb_ClearBusy);
		c.lit_dispatcher = false;
		c.bone_count = 18;
	}

	static constexpr game::Id COMDATA = game::MAG261_LimitAnimSetComData;
	static constexpr game::Id RING = game::g_MAG261_TrailRing;
	static constexpr game::Id PTS_BASE = game::g_MAG261_TrailPtsBase;
	static constexpr game::Id PTS_TIP = game::g_MAG261_TrailPtsTip;
	static constexpr game::Id SPL_BASE = game::g_MAG261_TrailCurveBase;
	static constexpr game::Id SPL_TIP = game::g_MAG261_TrailCurveTip;
	static constexpr game::Id TAIL_BASE = game::g_MAG261_TrailTailBase;
	static constexpr game::Id DIR_TIP = game::g_MAG261_TrailDirTip;
	static constexpr game::Id HEAD_BASE = game::g_MAG261_TrailHeadBase;
	static constexpr game::Id DIR_BASE = game::g_MAG261_TrailDirBase;

	template<typename F> static inline F fx(uint32_t a) { return (F)a; }
	static inline uint8_t *A(uint32_t a) { return (uint8_t *)a; }
	static inline void xl_BindEntityModelFromComData(void *ent, void *out, uint32_t data) { fx<void (__cdecl *)(void *, void *, uint32_t)>(game_addr(game::BattleEntity_BindExternalAnimSet))(ent, out, data); }
	static inline void xl_QueueChainTransformation(void *ent, int32_t id) { fx<void (__cdecl *)(void *, int32_t)>(game_addr(game::QueueChainTransformation))(ent, id); }
	static inline uint8_t *xl_JointMatrix_B66B80(void *hdr, int32_t joint, int32_t scale, void *m) { return fx<uint8_t *(__cdecl *)(void *, int32_t, int32_t, void *)>(game_addr(game::GfCinematic_ModelJointMatrix))(hdr, joint, scale, m); }
	static inline void xl_SplineSetup(int32_t n, const void *pts, void *work) { fx<void (__cdecl *)(int32_t, const void *, void *)>(game_addr(game::Effect_SplineSetup))(n, pts, work); }
	static inline void xl_SplineEval(int32_t n, const void *work, void *out, int32_t t) { fx<void (__cdecl *)(int32_t, const void *, void *, int32_t)>(game_addr(game::Effect_SplineEval))(n, work, out, t); }
	static inline void xl_Normalize(const void *in, void *out) { fx<void (__cdecl *)(const void *, void *)>(game_addr(game::NormalizeVectorToFixedPoint))(in, out); }
	static inline void xl_MulVec3x3(const void *m, const void *in, void *out) { fx<void (__cdecl *)(const void *, const void *, void *)>(game_addr(game::TransformVectorBy3x3Matrix))(m, in, out); }
	static inline void xl_GteReadFLAG(void *dst) { fx<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadFLAG))(dst); }
	static inline void xl_GteAVSZ4() { fx<void (__cdecl *)()>(game_addr(game::GTE_AVSZ4))(); }
	static inline void xl_GteSetIR0(int32_t v) { fx<void (__cdecl *)(int32_t)>(game_addr(game::GTE_SetIR0))(v); }
	static inline void xl_GteLoadRGBC(const void *c) { fx<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadRGBC))(c); }
	static inline void xl_GteDPCS() { fx<void (__cdecl *)()>(game_addr(game::GTE_DPCS))(); }
	static inline void xl_GteStoreRGB2(void *dst) { fx<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreRGB2))(dst); }
	static inline void xl_InsertPrimAutoDepth(uint32_t ot, void *prim) { fx<void (__cdecl *)(uint32_t, void *)>(game_addr(game::SSIGPU_InsertPrimAutoDepth))(ot, prim); }

	static inline uint8_t *SlotEntity() { return PTR(SCENE(), 0x60 + 4 * (int32_t)U8(CUR(), 0x1B)); }

	static void __cdecl op_0AB_BindLimitAnimSet()
	{
		uint8_t *ent = SlotEntity();
		uint8_t *blk = blob::ArenaAlloc(0x20);
		xl_BindEntityModelFromComData(ent, blk, game_addr(COMDATA));
		xl_QueueChainTransformation(ent, S16(STREAM(), 2));
		STREAM() += 4;
	}

	static void __cdecl op_0AD_PosFromPosedEntityJoint()
	{
		uint8_t *ent = SlotEntity();
		uint8_t *hdr = (U8(RT(), 0x4B) & 0x80) ? PTR(ent, 0x78) : ent + 0x60;
		x::BuildBoneMatricesFromPose(hdr);
		uint8_t *m = xl_JointMatrix_B66B80(hdr, S16(STREAM(), 2), 0x1000, ent + 0x40);
		x::SetRotMatrix(m);
		x::SetTransVector(m);
		uint8_t *w = WS();
		x::TransformToCamera(CUR() + 0x94, w + 0xF0, w + 0xFC);
		S32(CUR(), 0x5C) = shl32(S32(WS(), 0xF0), 16);
		S32(CUR(), 0x60) = shl32(S32(WS(), 0xF4), 16);
		S32(CUR(), 0x64) = shl32(S32(WS(), 0xF8), 16);
		C().bone[U8(CUR(), 0x18)]();
		STREAM() += 4;
	}

	static void __cdecl op_0B4_PosFromRandomJoint()
	{
		uint8_t *ent = SlotEntity();
		uint8_t *hdr = ent + 0x60;
		x::BuildBoneMatricesFromPose(hdr);
		const int32_t n = U8(PTR(PTR(hdr, 4), 0), 0);
		int32_t j = blob::Rand74(n);
		if (j >= n) j = n - 1;
		xl_JointMatrix_B66B80(hdr, j, 0x1000, ent + 0x40);
		S32(CUR(), 0x5C) = shl32(S32(WS(), 0xF0), 16);
		S32(CUR(), 0x60) = shl32(S32(WS(), 0xF4), 16);
		S32(CUR(), 0x64) = shl32(S32(WS(), 0xF8), 16);
		C().bone[U8(CUR(), 0x18)]();
		STREAM() += 2;
	}

	struct TrailWork
	{
		int32_t pad0[2];
		int32_t otz;
		int32_t flag;
		uint32_t c_prev;
		uint32_t c_cur;
		uint32_t pad18;
		uint32_t c_base;
		int16_t base[4];
		int16_t tip[4];
		int32_t dir[4];
	};
	static_assert(sizeof(TrailWork) == 0x40, "TrailWork");

	static void JointFrame(uint8_t *hdr, int32_t joint, int32_t *dir, int16_t *pos)
	{
		uint8_t *sk = PTR(PTR(hdr, 4), 0);
		uint8_t *bones = sk + 0x10;
		if (joint >= 0xF0) joint = U8(sk, joint - 0xEC);
		uint8_t *b = bones + 0x30 * joint;
		dir[1] = 0;
		dir[0] = 0;
		dir[2] = (int32_t)0xFFFFF000;
		xl_MulVec3x3(b + 0x10, dir, dir);
		xl_Normalize(dir, dir);
		pos[0] = S16(b, 0x24);
		pos[1] = S16(b, 0x28);
		pos[2] = S16(b, 0x2C);
	}

	static void TipFrom(TrailWork *w, int32_t len)
	{
		for (int k = 0; k < 3; k++) w->tip[k] = (int16_t)((mul32(len, w->dir[k]) >> 12) + w->base[k]);
	}

	static void TrailDraw(const uint8_t *sa, const uint8_t *sb, uint32_t c, TrailWork *w)
	{
		x::SetRotMatrix(NODEMAT());
		x::SetTransVector(NODEMAT());
		x::GteSetBackColor3(0, 0, 0);
		uint8_t *p = PTR(CTX(), 0x7C);
		const uint32_t col = (c & 0xFFFFFF) | 0x3A000000;
		uint8_t *q = p + 0x24;
		w->c_cur = col;
		w->c_base = col;
		int32_t t = 0;
		for (int32_t off = 0; off < 0xF0; off += 8, t += 0x1000)
		{
			w->c_prev = w->c_cur;
			U32(p, 0) = 0x8000000;
			x::GteLoadV012(sa + off, sa + off + 8, sb + off);
			x::GteRTPT();
			xl_GteReadFLAG(&w->flag);
			if (w->flag & 0x60000) continue;
			x::GteReadSXY012(p + 8, p + 0x10, p + 0x18);
			x::GteLoadV0(sb + off + 8);
			x::GteRTPS();
			x::GteReadSXY2(p + 0x20);
			xl_GteAVSZ4();
			x::GteReadOTZ(&w->otz);
			xl_GteSetIR0(t / 30);
			xl_GteLoadRGBC(&w->c_base);
			xl_GteDPCS();
			xl_GteStoreRGB2(&w->c_cur);
			U32(p, 0x14) = w->c_prev;
			U32(p, 4) = w->c_prev;
			U32(p, 0x1C) = w->c_cur;
			U32(p, 0xC) = w->c_cur;
			xl_InsertPrimAutoDepth(FRAMERL() + 4 * (uint32_t)(w->otz >> 2) + 0x44, p);
			U32(q, 0) = 0x8000000;
			U32(q, 0xC) = 0x3A000000;
			U32(q, 4) = 0x3A000000;
			U32(q, 0x14) = w->c_prev;
			U32(q, 0x1C) = w->c_cur;
			U32(q, 8) = U32(p, 8);
			U32(q, 0x10) = U32(p, 0x10);
			U32(q, 0x18) = U32(p, 0x18);
			U32(q, 0x20) = U32(p, 0x20);
			xl_InsertPrimAutoDepth(FRAMERL() + 4 * (uint32_t)(w->otz >> 2) + 0x44, q);
			p += 0x48;
			q += 0x48;
		}
		PTR(CTX(), 0x7C) = q;
	}

	static int32_t TrailGather(const uint8_t *ring, int32_t size, int32_t counter, int32_t s, uint8_t *pa, uint8_t *pb)
	{
		int32_t n = size;
		const int32_t lim = 0x64 - size;
		if (counter >= lim) n = n - counter + lim;
		int32_t i = 0;
		for (; i < n; i++)
		{
			const uint8_t *e = ring + 0x14 * s;
			if (U32(e, 0) == 0) break;
			s--;
			U32(pa, 8 * i) = U32(e, 4);
			U32(pa, 8 * i + 4) = U32(e, 8);
			U32(pb, 8 * i) = U32(e, 0xC);
			U32(pb, 8 * i + 4) = U32(e, 0x10);
			if (s < 0) s = size - 1;
		}
		return i;
	}

	static void TrailCurve(int32_t n, const uint8_t *pts, uint8_t *work, uint8_t *out)
	{
		xl_SplineSetup(n, pts, work);
		int32_t k = 0;
		for (uint8_t *o = out; o < out + 0x100; o += 8, k += 0x1000)
			xl_SplineEval(n, work, o, k / 31);
	}

	static void TrailSample(uint8_t *data, uint8_t *blk)
	{
		TrailWork *w = (TrailWork *)x::FieldAlloc(0x40);
		JointFrame(SlotEntity() + 0x60, U16(data, 0), w->dir, w->base);
		TipFrom(w, S16(CUR(), 0x94));
		const int32_t size = U16(data, 2);
		const int32_t slot = (int32_t)((uint32_t)U16(blk, 0) % (uint32_t)size);
		uint8_t *e = A(game_addr(RING)) + 0x14 * slot;
		U32(e, 0) = 1;
		U32(e, 4) = U32(w->base, 0);
		U32(e, 8) = U32(w->base, 4);
		U32(e, 0xC) = U32(w->tip, 0);
		U32(e, 0x10) = U32(w->tip, 4);
		const int32_t n = TrailGather(A(game_addr(RING)), size, U16(blk, 0), slot, A(game_addr(PTS_BASE)), A(game_addr(PTS_TIP)));
		if (n > 1)
		{
			uint8_t *work = (uint8_t *)x::FieldAlloc(0x190);
			TrailCurve(n, A(game_addr(PTS_BASE)), work, A(game_addr(SPL_BASE)));
			TrailCurve(n, A(game_addr(PTS_TIP)), work, A(game_addr(SPL_TIP)));
			x::FieldFree(0x190);
			U32(A(game_addr(TAIL_BASE)), 0) = U32(A(game_addr(SPL_TIP)), 0);
			S32(A(game_addr(DIR_TIP)), 0) = S16(A(game_addr(SPL_TIP)), 0) - S16(A(game_addr(SPL_TIP)), 8);
			U32(A(game_addr(TAIL_BASE)), 4) = U32(A(game_addr(SPL_TIP)), 4);
			S32(A(game_addr(DIR_TIP)), 4) = S16(A(game_addr(SPL_TIP)), 2) - S16(A(game_addr(SPL_TIP)), 0xA);
			S32(A(game_addr(DIR_TIP)), 8) = S16(A(game_addr(SPL_TIP)), 4) - S16(A(game_addr(SPL_TIP)), 0xC);
			xl_Normalize(A(game_addr(DIR_TIP)), A(game_addr(DIR_TIP)));
			U32(A(game_addr(HEAD_BASE)), 0) = U32(A(game_addr(SPL_BASE)), 0);
			U32(A(game_addr(HEAD_BASE)), 4) = U32(A(game_addr(SPL_BASE)), 4);
			S32(A(game_addr(DIR_BASE)), 0) = S16(A(game_addr(TAIL_BASE)), 0) - S16(A(game_addr(SPL_BASE)), 0);
			S32(A(game_addr(DIR_BASE)), 4) = S16(A(game_addr(TAIL_BASE)), 2) - S16(A(game_addr(SPL_BASE)), 2);
			S32(A(game_addr(DIR_BASE)), 8) = S16(A(game_addr(TAIL_BASE)), 4) - S16(A(game_addr(SPL_BASE)), 4);
			xl_Normalize(A(game_addr(DIR_BASE)), A(game_addr(DIR_BASE)));
			TrailDraw(A(game_addr(SPL_BASE)), A(game_addr(SPL_TIP)), U32(CUR(), 0xCC), w);
		}
		x::FieldFree(0x40);
		U16(blk, 0) = (uint16_t)(U16(blk, 0) + 1);
	}

	static void __cdecl dh_71_JointTrail()
	{
		uint8_t *b = CUR();
		if (PTR(b, 0xBC) == nullptr)
		{
			uint8_t *data = PTR(b, 0xB8);
			uint8_t *blk = blob::ArenaAlloc(0x10);
			PTR(CUR(), 0xBC) = blk;
			U16(blk, 0) = 0;
			for (int32_t i = 0; i < (int32_t)U16(data, 2); i++) U32(A(game_addr(RING)), 0x14 * i) = 0;
			b = CUR();
		}
		uint8_t *data = PTR(b, 0xB8);
		uint8_t *blk = PTR(b, 0xBC);
		uint8_t *ent = PTR(SCENE(), 0x60 + 4 * (int32_t)U8(b, 0x1B));
		if (S16(b, 0x96) != 0) return;
		x::ComputeBonesWorldMatrices(ent + 0x60, ent + 0x40);
		TrailSample(data, blk);
		x::BuildBoneMatricesFromPose(ent + 0x60);
	}

	static VoidFn g_save_vm[0x200], g_save_big[256], g_save_draw[256];

	static void apply_slots(Clone &c)
	{
		ff8fx::copy<(sizeof(g_save_vm))>(g_save_vm, c.vm);
		ff8fx::copy<(sizeof(g_save_big))>(g_save_big, c.bigtab);
		ff8fx::copy<(sizeof(g_save_draw))>(g_save_draw, c.draw);
		apply_brothers(c);
		const VoidFn d38 = c.draw[38];
		ff8fx::copy<(sizeof(g_save_vm))>(c.vm, g_save_vm);
		ff8fx::copy<(sizeof(g_save_big))>(c.bigtab, g_save_big);
		ff8fx::copy<(sizeof(g_save_draw))>(c.draw, g_save_draw);
		apply_shared_misc(c);
		c.draw[38] = d38;
		c.vm[0x0AB] = op_0AB_BindLimitAnimSet;
		c.vm[0x0AD] = op_0AD_PosFromPosedEntityJoint;
		c.vm[0x0B4] = op_0B4_PosFromRandomJoint;
		c.draw[71] = dh_71_JointTrail;
	}

	static uint32_t __cdecl SequenceTask(TaskNode *)
	{
		return SequenceTick(g_c);
	}
}
}

	void register_mag261_desperado()
	{
		using namespace gfc;
		desperado261::describe(desperado261::g_c);
		init_clone(desperado261::g_c, nullptr, 0);
		desperado261::apply_slots(desperado261::g_c);
		register_port(game::MAG_261_SequenceTick, (void *)desperado261::SequenceTask, "MAG261 Desperado SequenceTick", 261);
	}
}

