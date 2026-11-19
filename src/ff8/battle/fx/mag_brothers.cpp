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

// Effect 205: Brothers

#include "gfc_engine.h"

namespace ff8fx
{
namespace gfc
{
namespace part_brothers
{
	static constexpr game::Id BROTHERS_FLAG_2796D40 = game::g_GF205Brothers_NorgTargetFixupActive;
	static constexpr game::Id COMBAT_SCENE_ID_1CFF6E0 = game::COMBAT_SCENE_ID;
	static constexpr game::Id MODEL_SLOT0_FILEID_1D98990 = game::unk_1D98990;
	static constexpr game::Id DWORD_1D98A4C = game::unk_1D98A4C;
	static constexpr game::Id WORD_1D974B0 = game::unk_1D974B0;
	static constexpr game::Id WORD_1D97494 = game::unk_1D97494;
	static inline uint8_t *ZEROVEC() { return (uint8_t *)game_addr(game::unk_27971E4); }

	static inline void *xl_ComposeZYXRotation(const void *angles, void *out) { return x::f<void *(__cdecl *)(const void *, void *)>(game_addr(game::ComposeZYXRotationMatrix))(angles, out); }
	static inline void xl_Apply3DTransform(const void *m, const void *v, void *out) { x::f<void (__cdecl *)(const void *, const void *, void *)>(game_addr(game::apply3DTransform))(m, v, out); }
	static inline void xl_RotTransIR(const void *v, void *ir, void *flag) { x::f<void (__cdecl *)(const void *, void *, void *)>(game_addr(game::GTE_RotTrans))(v, ir, flag); }
	static inline int32_t xl_RotTransPers(const void *v, void *sxy, void *p, void *flag) { return x::f<int32_t (__cdecl *)(const void *, void *, void *, void *)>(game_addr(game::TransformWorldCoordinateToProjectedSpace))(v, sxy, p, flag); }
	static inline int32_t xl_NormalClip(int32_t a, int32_t b, int32_t c) { return x::f<int32_t (__cdecl *)(int32_t, int32_t, int32_t)>(game_addr(game::GTE_NormalClip3))(a, b, c); }
	static inline void xl_ModelShadow_B67040(void *hdr, void *root, int32_t ot, int32_t y, int32_t level)
	{
		x::f<int32_t (__cdecl *)(void *, void *, int32_t, int32_t, int32_t)>(game_addr(game::GfCinematic_DrawModelShadow))(hdr, root, ot, y, level);
	}
	static inline void xl_OffscreenModelRender_B65F30(void *desc, const void *pos, const void *angles)
	{
		x::f<int32_t (__cdecl *)(void *, const void *, const void *)>(game_addr(game::GfCinematic_RenderBattleModelOffscreen))(desc, pos, angles);
	}

	static void __cdecl dh_03_AnimatedModelWithShadow()
	{
		h_B269E0();
		h_B26AD0(PTR(CUR(), 0xBC));
		uint8_t *blk = PTR(CUR(), 0xBC);
		U32(blk, 0x44) = U32(CTX(), 0x74);
		uint32_t cursor = U32(CTX(), 0x7C);
		uint32_t ot = U32(RT(), 0x4C);
		U32(CTX(), 0x7C) = x::RenderGeometry(blk + 0x30, blk + 0x40, ot, 4, cursor);
		if (!(U8(blk, 2) & 4)) return;
		uint8_t *par = blob::GetBone((int32_t)U16(CUR(), 0x9C));
		blob::RotMatrixOrder(par + 0x8C, 0);
		blob::CopyMatrix(WS() + 0x80, WS() + 0xE0);
		S32(WS(), 0x60) = shl32(S16(CUR(), 0x8C), 4);
		S32(WS(), 0x64) = shl32(S16(CUR(), 0x8E), 4);
		S32(WS(), 0x68) = shl32(S16(CUR(), 0x90), 4);
		x::ScaleMatrix(WS() + 0x80, WS() + 0x60);
		uint8_t *hdr = blk + 0x10;
		S32(WS(), 0x94) = S16(par, 0x94);
		S32(WS(), 0x98) = S16(par, 0x96);
		S32(WS(), 0x9C) = S16(par, 0x98);
		uint8_t *sp = PTR(hdr, 0x6C);
		int32_t y = S16(sp, 2);
		int32_t otspec = S16(sp, 0);
		xl_ModelShadow_B67040(hdr, WS() + 0x80, otspec, y, 0x80);
	}

	static void EntityShadow_AF5D00(uint8_t *ent, uint8_t *root, uint8_t *pos)
	{
		uint8_t *w = WS();
		U8(ent, 0) |= 0x20;
		uint8_t *hdr = ent + 0x60;
		x::BuildBoneMatricesFromPose(hdr);
		x::ComputeBonesWorldMatrices(hdr, root);
		uint8_t *sk = PTR(PTR(ent, 0x64), 0);
		int32_t i = 1;
		int32_t maxx = S32(sk, 0x34);
		int32_t minx = maxx - 0x1F4;
		maxx += 0x1F4;
		int32_t zz = S32(sk, 0x3C);
		int32_t minz = zz - 0x1F4;
		int32_t maxz = zz + 0x1F4;
		U32(w, 0xD0) = 0;
		if (U8(sk, 0) > 1)
		{
			uint8_t *m = sk + 0x50;
			do
			{
				x::SetRotMatrix(m);
				x::SetTransVector(m);
				U16(w, 0xD4) = U16(m, -0xE);
				xl_RotTransIR(w + 0xD0, w + 0xD8, w + 0xFC);
				int32_t v = S16(w, 0xD8);
				if (v < minx) minx = v;
				else if (v > maxx) maxx = v;
				v = S16(w, 0xDC);
				if (v < minz) minz = v;
				else if (v > maxz) maxz = v;
				m += 0x30;
				i++;
			} while (i < (int32_t)U8(sk, 0));
		}
		int32_t hx = add32(minx, maxx) / 2;
		U16(w, 0xD0) = (uint16_t)(U16(pos, 0) + (uint16_t)hx);
		maxx = sub32(maxx, hx);
		uint16_t pz = U16(pos, 4);
		U16(w, 0xD2) = 0;
		int32_t hz = add32(minz, maxz) / 2;
		maxz = sub32(maxz, hz);
		U16(w, 0xD4) = (uint16_t)(pz + (uint16_t)hz);
		x::SetRotMatrix(NODEMAT());
		x::SetTransVector(NODEMAT());
		uint8_t *flag = w + 0xFC;
		xl_RotTransIR(w + 0xD0, w + 0xD8, flag);
		int32_t tx = S16(w, 0xD8);
		int32_t ty = S16(w, 0xDA);
		int32_t tz = S16(w, 0xDC);
		x::GteWriteCtrl(tx, 5);
		x::GteWriteCtrl(ty, 6);
		x::GteWriteCtrl(tz, 7);
		uint8_t *q = w + 0xB0;
		int32_t cs = x::Cos(0x200);
		int32_t dz = maxz;
		int32_t dx = maxx;
		int32_t a = mul32(cs, dz) >> 12;
		int32_t c = mul32(cs, dx) >> 12;
		U16(q, 0x04) = (uint16_t)dz;
		U16(q, 0x0C) = (uint16_t)a;
		U16(q, 0x08) = (uint16_t)c;
		U16(q, 0x18) = (uint16_t)c;
		int32_t na = -a, ndz = -dz, nc = -c;
		U16(q, 0x00) = 0;
		U16(q, 0x02) = 0;
		U16(q, 0x0A) = 0;
		U16(q, 0x10) = (uint16_t)dx;
		U16(q, 0x12) = 0;
		U16(q, 0x14) = 0;
		U16(q, 0x1A) = 0;
		U16(q, 0x1C) = (uint16_t)na;
		U16(q, 0x20) = 0;
		U16(q, 0x22) = 0;
		U16(q, 0x24) = (uint16_t)ndz;
		U16(q, 0x28) = (uint16_t)nc;
		int32_t ndx = -dx;
		U16(q, 0x38) = (uint16_t)nc;
		U16(q, 0x30) = (uint16_t)ndx;
		U16(q, 0x2A) = 0;
		U16(q, 0x2C) = (uint16_t)na;
		U16(q, 0x32) = 0;
		U16(q, 0x34) = 0;
		U16(q, 0x3A) = 0;
		U16(q, 0x3C) = (uint16_t)a;
		U16(q, 0x40) = 0;
		U16(q, 0x42) = 0;
		U16(q, 0x44) = 0;
		uint8_t *v = q;
		for (int32_t n = 9; n != 0; n--)
		{
			xl_RotTransPers(v, v, flag, flag);
			v += 8;
		}
		if (xl_NormalClip(S32(q, 0), S32(q, 0x10), S32(q, 0x20)) > 0)
		{
			uint32_t ot = FRAMERL() + 0x4064;
			uint8_t *pkt = PTR(CTX(), 0x7C);
			uint32_t code = U32(ent, 0x2C);
			int32_t k = 8;
			do
			{
				U8(pkt, 3) = 8;
				U32(pkt, 8) = 0;
				U32(pkt, 4) = 0xE1000040;
				U32(pkt, 0xC) = code;
				U32(pkt, 0x14) = 0;
				U32(pkt, 0x1C) = 0;
				U32(pkt, 0x10) = U32(q, 0x40);
				U32(pkt, 0x18) = U32(q, k - 8);
				U32(pkt, 0x20) = U32(q, k & 0x38);
				x::InsertPrimDepthKeys(ot, pkt, 0, 0, 0, 0);
				k += 8;
				pkt += 0x24;
			} while (k - 8 < 0x40);
			PTR(CTX(), 0x7C) = pkt;
		}
		x::BuildBoneMatricesFromPose(hdr);
	}

	static void __cdecl dh_38_AttachBattleEntity()
	{
		uint8_t *bone = CUR();
		uint8_t *ent = PTR(SCENE(), 0x60 + 4 * (int32_t)U8(bone, 0x1B));
		if (U32(bone, 0xC4) == 0)
		{
			U32(bone, 0xC4) = 0xFFFFFFFF;
			uint16_t d = U16(ent, 0x54);
			U8(ent, 1) |= 0x10;
			d = (uint16_t)(d - U16(ent, 0x1C));
			U16(WS(), 0xC0) = d;
			U16(WS(), 0xC2) = (uint16_t)(U16(ent, 0x58) - U16(ent, 0x1E));
			U16(WS(), 0xC4) = (uint16_t)(U16(ent, 0x5C) - U16(ent, 0x20));
			uint8_t *ang = SCENE() + 0xD8 + 8 * (int32_t)U8(CUR(), 0x1B);
			U16(WS(), 0xD0) = (uint16_t)(0 - U16(ang, 0));
			U16(WS(), 0xD2) = (uint16_t)(0 - U16(ang, 2));
			U16(WS(), 0xD4) = (uint16_t)(0 - U16(ang, 4));
			xl_ComposeZYXRotation(WS() + 0xD0, WS() + 0x60);
			x::SetRotMatrix(WS() + 0x60);
			x::SetTransVector(ZEROVEC());
			xl_RotTransIR(WS() + 0xC0, CUR() + 0xBC, WS() + 0xFC);
			bone = CUR();
		}
		uint8_t *off = bone + 0xBC;
		uint8_t *ang = SCENE() + 0xD8 + 8 * (int32_t)U8(bone, 0x1B);
		xl_ComposeZYXRotation(ang, WS() + 0x60);
		xl_Apply3DTransform(WS() + 0x60, off, WS() + 0x74);
		uint8_t *hdr = ent + 0x60;
		x::BuildBoneMatricesFromPose(hdr);
		blob::Blob_B66B80(hdr, 0xF1, 0x1000, WS() + 0x60);
		GFC_TAINT(WS() + 0xE2, nullptr, 2);
		uint8_t *w = WS();
		int32_t oF0 = S32(w, 0xF0);
		int32_t oF4 = S32(w, 0xF4);
		int32_t oF8 = S32(w, 0xF8);
		U16(w, 0xD0) = (uint16_t)(U16(CUR(), 0x8C) + U16(ang, 0));
		U16(ent, 0x0C) = U16(WS(), 0xD0);
		U16(WS(), 0xD2) = (uint16_t)(U16(CUR(), 0x8E) + U16(ang, 2));
		U16(ent, 0x0E) = U16(WS(), 0xD2);
		U16(WS(), 0xD4) = (uint16_t)(U16(CUR(), 0x90) + U16(ang, 4));
		U16(ent, 0x10) = U16(WS(), 0xD4);
		xl_ComposeZYXRotation(WS() + 0xD0, WS() + 0x60);
		xl_Apply3DTransform(WS() + 0x60, off, WS() + 0x74);
		blob::Blob_B66B80(hdr, 0xF1, 0x1000, WS() + 0x60);
		GFC_TAINT(WS() + 0xE2, nullptr, 2);
		w = WS();
		int32_t dz = sub32(S32(w, 0xF8), oF8);
		int32_t dy = sub32(S32(w, 0xF4), oF4);
		uint8_t *base = SCENE() + 0x84 + 8 * (int32_t)U8(CUR(), 0x1B);
		uint8_t *p = w + 0x50;
		uint16_t bx = (uint16_t)(U16(CUR(), 0x94) - (uint16_t)S32(w, 0xF0));
		bx = (uint16_t)(bx + U16(base, 0));
		uint16_t ax = (uint16_t)((uint16_t)oF0 + bx);
		U16(p, 0) = ax;
		U16(ent, 0x1C) = ax;
		ax = (uint16_t)(U16(CUR(), 0x96) + U16(base, 2));
		ax = (uint16_t)(ax - (uint16_t)dy);
		U16(p, 2) = ax;
		U16(ent, 0x1E) = ax;
		ax = (uint16_t)(U16(CUR(), 0x98) + U16(base, 4));
		ax = (uint16_t)(ax - (uint16_t)dz);
		U16(p, 4) = ax;
		U16(ent, 0x20) = ax;
		EntityShadow_AF5D00(ent, WS() + 0x60, p);
	}

	static void SyncEntityModelCopy_AF6310(uint8_t *blk)
	{
		uint8_t *src = PTR(blk, 0xC) + 0x60;
		uint8_t *d = blk + 0x10;
		uint8_t *d2 = blk + 0x30;
		U32(d, 0) = U32(src, 0);
		U32(d, 4) = U32(src, 4);
		U32(d, 8) = U32(src, 8);
		uint8_t *m = PTR(src, 4);
		U32(d2, 0) = U32(m, 0);
		U32(d2, 4) = U32(m, 4);
		U32(d2, 8) = U32(m, 8);
		U32(d2, 0xC) = U32(m, 0xC);
	}

	static uint8_t *CreateEntityModelCopy_AF62B0(uint8_t *ent)
	{
		uint8_t *blk = blob::ArenaAlloc(0x80);
		PTR(blk, 0xC) = ent;
		SyncEntityModelCopy_AF6310(blk);
		U32(blk, 0x54) = 0;
		U32(blk, 0x58) = 0xD00140;
		uint32_t t = U32(ent, 0x28);
		U32(blk, 0x78) = t;
		U32(blk, 0x5C) = t;
		t = U32(ent, 0x7C);
		U16(blk, 0x64) = 0;
		U32(blk, 0x60) = t;
		U32(blk, 0x68) = 0;
		U32(blk, 0x50) = 0xFFFFFFFF;
		U16(blk, 0) = 0;
		U16(blk, 2) = 0;
		U8(blk, 0x21) = 0;
		return blk;
	}

	static void __cdecl dh_48_DrawBattleEntityCopy()
	{
		uint8_t *bone = CUR();
		uint8_t *ent = PTR(SCENE(), 0x60 + 4 * (int32_t)U8(bone, 0x1B));
		if (U32(bone, 0xBC) == 0)
		{
			U8(ent, 0) |= 4;
			uint8_t *r = blob::GetBone(S16(PTR(CUR(), 0xB8), 0));
			uint8_t *c = CUR() + 0xC0;
			U32(c, 0) = U32(r, 0x94);
			U16(c, 4) = U16(r, 0x98);
			c = CUR() + 0xA8;
			U32(c, 0) = U32(r, 0x8C);
			U16(c, 4) = U16(r, 0x98);
			uint8_t *blk = CreateEntityModelCopy_AF62B0(ent);
			PTR(CUR(), 0xBC) = blk;
		}
		uint8_t *r = blob::GetBone(S16(PTR(CUR(), 0xB8), 0));
		uint8_t *snap = CUR() + 0xA8;
		uint8_t *a = WS() + 0x80;
		uint8_t *ang = SCENE() + 0xD8 + 8 * (int32_t)U8(CUR(), 0x1B);
		U16(a, 0) = (uint16_t)(U16(ang, 0) - U16(snap, 0) + U16(r, 0x8C));
		U16(a, 2) = (uint16_t)(U16(r, 0x8E) - U16(snap, 2) + U16(ang, 2));
		U16(a, 4) = (uint16_t)(U16(r, 0x98) - U16(snap, 4) + U16(ang, 4));
		blob::RotMatrixOrder(a, 1);
		uint8_t *sp = CUR() + 0xC0;
		uint8_t *base = SCENE() + 0x84 + 8 * (int32_t)U8(CUR(), 0x1B);
		x::RotMatrixFromAngles(r + 0x8C, WS() + 0xA0);
		int32_t tx = S16(r, 0x94);
		int32_t ty = S16(r, 0x96);
		int32_t tz = S16(r, 0x98);
		x::GteWriteCtrl(tx, 5);
		x::GteWriteCtrl(ty, 6);
		x::GteWriteCtrl(tz, 7);
		x::SetRotMatrix(WS() + 0xA0);
		U16(WS(), 0xC0) = (uint16_t)(U16(base, 0) - U16(sp, 0));
		U16(WS(), 0xC2) = (uint16_t)(U16(base, 2) - U16(sp, 2));
		U16(WS(), 0xC4) = (uint16_t)(U16(base, 4) - U16(sp, 4));
		uint8_t *w = WS();
		x::TransformToCamera(w + 0xC0, w + 0xF4, w + 0x9C);
		w = WS();
		x::ComposeAffine(NODEMAT(), w + 0xE0, w + 0xE0);
		GFC_TAINT(w + 0xF2, nullptr, 2);
		uint8_t *blk = PTR(CUR(), 0xBC);
		uint8_t *hdr = blk + 0x10;
		uint8_t *desc = blk + 0x40;
		SyncEntityModelCopy_AF6310(blk);
		x::ComputeBonesWorldMatrices(hdr, WS() + 0xE0);
		uint32_t ot = U32(RT(), 0x4C);
		uint32_t cursor = U32(CTX(), 0x7C);
		U32(desc, 4) = U32(CTX(), 0x74);
		U32(CTX(), 0x7C) = x::RenderGeometry(blk + 0x30, desc, ot, 4, cursor);
		x::BuildBoneMatricesFromPose(hdr);
	}

	static void __cdecl op_09D_RecenterModelOffsetBone()
	{
		uint8_t *obj = blob::ObjectPtr(S16(STREAM(), 2));
		int32_t minz = 0x7FFF;
		uint8_t *v = obj + U32(obj, 0x14);
		int32_t n = S32(obj, 0x18);
		int32_t miny = 0x7FFF;
		int32_t minx = 0x7FFF;
		int32_t maxz = -0x8000;
		int32_t maxy = -0x8000;
		int32_t maxx = -0x8000;
		if (n > 0)
		{
			for (int32_t k = n; k != 0; k--)
			{
				int32_t t = S16(v, 0);
				if (t < minx) minx = t;
				if (t > maxx) maxx = t;
				t = S16(v, 2);
				if (t < miny) miny = t;
				if (t > maxy) maxy = t;
				t = S16(v, 4);
				if (t < minz) minz = t;
				if (t > maxz) maxz = t;
				v += 8;
			}
		}
		int32_t cz = minz;
		minx = add32(minx, sub32(maxx, minx) / 2);
		int32_t p = mul32(S16(STREAM(), 4), minx);
		S32(CUR(), 0x5C) = add32(S32(CUR(), 0x5C), shl32(p / 256, 16));
		miny = add32(miny, sub32(maxy, miny) / 2);
		p = mul32(S16(STREAM(), 6), miny);
		S32(CUR(), 0x60) = add32(S32(CUR(), 0x60), shl32(p / 256, 16));
		cz = add32(cz, sub32(maxz, cz) / 2);
		p = mul32(S16(STREAM(), 8), cz);
		{
			uint8_t *b = CUR();
			S32(b, 0x64) = add32(S32(b, 0x64), shl32(p / 256, 16));
		}
		(&C().bone[0])[U8(CUR(), 0x18)]();
		v = obj + U32(obj, 0x14);
		n = S32(obj, 0x18);
		if (n > 0)
		{
			for (; n != 0; n--)
			{
				U16(v, 0) = (uint16_t)(U16(v, 0) - (uint16_t)minx);
				U16(v, 2) = (uint16_t)(U16(v, 2) - (uint16_t)miny);
				U16(v, 4) = (uint16_t)(U16(v, 4) - (uint16_t)cz);
				v += 8;
			}
		}
		STREAM() += 0xA;
	}

	static void __cdecl op_0C5_JumpIfBattleStage()
	{
		uint8_t *s = STREAM();
		int32_t stage = (int32_t)(MEM<uint32_t>(game_addr(COMBAT_SCENE_ID_1CFF6E0)) & 0xFFFF);
		if (stage == (int32_t)S16(s, 2))
			STREAM() = s + S16(s, 4);
		else
			STREAM() = s + 6;
	}

	static void __cdecl op_0D1_JumpIfModelSlot0FileId()
	{
		uint16_t id = MEM<uint8_t>(game_addr(MODEL_SLOT0_FILEID_1D98990));
		uint8_t *s = STREAM();
		if (id == U16(s, 2))
			STREAM() = s + S16(s, 4);
		else
			STREAM() = s + 6;
	}

	static void __cdecl op_0D5_NorgBossTargetFixup()
	{
		uint8_t &flag = MEM<uint8_t>(game_addr(BROTHERS_FLAG_2796D40));
		if (MEM<uint16_t>(game_addr(COMBAT_SCENE_ID_1CFF6E0)) != 0x3F)
		{
			STREAM() += 4;
			return;
		}
		uint32_t sub = (uint32_t)U16(RT(), 0x4A) >> 12;
		if (sub == 0)
		{
			uint8_t *sc = SCENE();
			if (U8(sc, 0x48) == 3)
			{
				flag = 0;
				STREAM() += 4;
				return;
			}
			flag = 1;
			U8(sc, 0x41) = 4;
			U8(SCENE(), 0x4B) = 3;
			uint8_t *p = SCENE() + 0x9C;
			U16(p, 0) = MEM<uint16_t>(game_addr(WORD_1D974B0));
			U16(p, 2) = MEM<uint16_t>(game_addr(WORD_1D974B0) + 2);
			U16(p, 4) = MEM<uint16_t>(game_addr(WORD_1D974B0) + 4);
			U16(SCENE(), 0x54) = MEM<uint16_t>(game_addr(WORD_1D97494));
			U32(SCENE(), 0x6C) = game_addr(WORD_1D97494);
			STREAM() += 4;
			return;
		}
		if (sub == 1)
		{
			if (flag != 0 && U8(CUR(), 0x1B) == 3)
			{
				uint8_t *s = STREAM();
				STREAM() = s + S16(s, 2);
			}
			else
				STREAM() += 4;
		}
	}

	static void __cdecl op_0DA_RenderEntityToVram()
	{
		if (U8(RT(), 0x4B) & 0x10)
		{
			uint32_t lv = MEM<uint32_t>(game_addr(DWORD_1D98A4C)) & 0xFF;
			uint8_t *b = CUR();
			int32_t d = (int32_t)(lv + lv - 0xA0u);
			int32_t hi = (int32_t)(((int64_t)d * (int64_t)0x55555556) >> 32);
			uint32_t q = (uint32_t)hi + ((uint32_t)hi >> 31) + 0x7Fu;
			q |= U32(b, 0xCC) & 0x2000000;
			q |= 0x808000;
			U32(b, 0xCC) = q;
			STREAM() += 2;
			return;
		}
		if (U16(RCTX(), 0x30) == 0)
		{
			U16(WS(), 0xD0) = U16(STREAM(), 2);
			U16(WS(), 0xD2) = U16(STREAM(), 4);
			U16(WS(), 0xD4) = U16(STREAM(), 6);
			U16(WS(), 0xD6) = U16(STREAM(), 8);
			uint8_t *d = PTR(CTX(), 0x74);
			U32(d, 0) = 0;
			U32(d, 4) = 0;
			U32(d, 0) = U32(CTX(), 0x74) + 0xEC;
			U32(d, 8) = U32(CTX(), 0x74) + 0x10EC;
			U32(d, 0xC) = 0x5BEC;
			U32(CUR(), 0xC4) = 0x5BEC;
			blob::ArenaAlloc(S32(d, 0xC));
			U32(d, 0x14) = U32(CTX(), 0x74);
			uint8_t *b = CUR();
			xl_OffscreenModelRender_B65F30(d, b + 0x94, b + 0x8C);
		}
		STREAM() += 0xA;
	}
}

	void apply_brothers(Clone &c)
	{
		c.draw[3] = part_brothers::dh_03_AnimatedModelWithShadow;
		c.draw[38] = part_brothers::dh_38_AttachBattleEntity;
		c.draw[48] = part_brothers::dh_48_DrawBattleEntityCopy;
		c.vm[0x09D] = part_brothers::op_09D_RecenterModelOffsetBone;
		c.vm[0x0C5] = part_brothers::op_0C5_JumpIfBattleStage;
		c.vm[0x0D1] = part_brothers::op_0D1_JumpIfModelSlot0FileId;
		c.vm[0x0D5] = part_brothers::op_0D5_NorgBossTargetFixup;
		c.vm[0x0DA] = part_brothers::op_0DA_RenderEntityToVram;
	}
}
}

namespace ff8fx
{
namespace gfc
{
namespace brothers
{
	static Clone g_c = {};

	static void describe(Clone &c)
	{
		c.name = "205 Brothers";
		c.effect_id = 205;
		c.vm_table = game_addr(game::GF_205Brothers_VmOpcodeTable);
		c.bone_table = game_addr(game::GF_205Brothers_BoneHandlerTable);
		c.prim_table = game_addr(game::funcs_AF74E4);
		c.prim_size = game_addr(game::unk_1871BF4);
		c.spr_table = game_addr(game::unk_1871C08);
		c.ptype_table = game_addr(game::unk_1871C34);
		c.pop_table = game_addr(game::funcs_AF92F7);
		c.draw_table = game_addr(game::GF_205Brothers_DrawHandlerTable);
		c.attr_16A = game_addr(game::unk_1871E12);
		c.attr_184 = game_addr(game::unk_1871E2C);
		c.attr_186 = game_addr(game::unk_1871E2E);
		c.attr_194 = game_addr(game::unk_1871E3C);
		c.attr_19C = game_addr(game::unk_1871E44);
		c.desc = game_addr(game::unk_1871878);
		c.ptr_block = game_addr(game::unk_1871A94);
		c.queue = game_addr(game::unk_2796D30);
		c.file00 = game_addr(game::unk_2796D6C);
		c.file01 = game_addr(game::unk_2796D68);
		c.tick = game_addr(game::GF_205Brothers_SequenceTick);
		c.load_cb = game_addr(game::MAG_205_sub_AFAD70);
		c.lit_dispatcher = false;
		c.bone_count = 18;
	}

	static uint32_t __cdecl SequenceTask(TaskNode *)
	{
		return SequenceTick(g_c);
	}
}
}

	void register_gfc_brothers()
	{
		using namespace gfc;
		brothers::describe(brothers::g_c);
		init_clone(brothers::g_c, nullptr, 0);
		apply_brothers(brothers::g_c);
		apply_shared_misc(brothers::g_c);
		register_port(game::GF_205Brothers_SequenceTick, (void *)brothers::SequenceTask, "GFC205 Brothers SequenceTick", 205);
	}
}

