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

// Effect 210: Bloodfest

#include "mag_common.h"

namespace ff8fx
{
namespace blood210
{
	using namespace eng;
	using namespace magc;

#pragma pack(push, 1)
	struct Flame
	{
		int16_t pos[3];
		int16_t ground;
		uint8_t pad08[4];
		int16_t scale;
		uint8_t pad0E[6];
	};
	struct Ember
	{
		int16_t pos[3];
		int16_t vel_y;
		uint8_t pad08[4];
		int16_t scale;
		uint8_t pad0E[6];
	};
	struct FireSpark
	{
		int16_t pos[3];
		int16_t pad06;
		int16_t vel[3];
		int16_t pad0E;
		int16_t scale;
		int16_t pad12;
	};
#pragma pack(pop)
	static_assert(sizeof(Flame) == 0x14 && sizeof(Ember) == 0x14 && sizeof(FireSpark) == 0x14, "fire pieces");

	struct Roll
	{
		int32_t base, mod, sign;
		int32_t of(int32_t random) const { return base + sign * (random % mod); }
	};

	struct FlameRolls
	{
		int32_t delay_mod, delay_step;
		int32_t spread_num, spread_max;
		int32_t low;
		Roll lift;
		Roll scale;
	};
	inline int32_t FlameSpread(int16_t size, const FlameRolls &r)
	{
		const int32_t s = (size * r.spread_num) >> 12;
		return s > r.spread_max ? r.spread_max : s;
	}
	inline void FlameStart(Flame *f, int16_t *delay, const int16_t *pos, int16_t height, int32_t spread, int32_t k, const FlameRolls &r)
	{
		const int32_t half = spread >> 1;
		*delay = (int16_t)(CrtRand() % r.delay_mod + r.delay_step * k);
		ff8fx::copy<(8)>(f->pos, pos);
		f->pos[0] += (int16_t)(CrtRand() % spread - half);
		f->pos[1] += (int16_t)(CrtRand() % spread - half);
		f->pos[2] += (int16_t)(CrtRand() % spread - half);
		f->ground = height;
		if (f->pos[1] > height - r.low) f->pos[1] += (int16_t)r.lift.of(CrtRand());
		f->scale = (int16_t)r.scale.of(CrtRand());
	}

	struct FlameLook
	{
		game::Id seq, glow_seq;
		int32_t shade_shift;
		int32_t tilt, glow_mode, high, span, base;
	};
	inline void FlameDraw(const Flame *p, int16_t frame, const FlameLook &l, uint32_t &cursor)
	{
		TransformCameraByShadowRotation(p->pos, p->scale, -(p->scale >> l.shade_shift));
		uint8_t *h = AllocHeader(0xB4);
		*(uint32_t *)h = game_addr(l.seq);
		*(uint16_t *)(h + 4) = (uint16_t)frame;
		*(uint16_t *)(h + 0x24) = 0;
		cursor = InitEffectSequenceFromData(h, RenderOT(), 2, cursor);
		int16_t angles[4] = { (int16_t)l.tilt, 0, 0, 0 };
		Mat4x3 m = {};
		BuildRotationMatrixFromAngles(angles, &m);
		m.t[0] = p->pos[0];
		m.t[1] = p->ground;
		m.t[2] = p->pos[2];
		int32_t sv[4] = { p->scale, p->scale, 0x1000, 0 };
		Scale3DMatrix(&m, sv);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		int32_t y = p->pos[1];
		if (y < l.high) y = l.high;
		const int32_t q = shl32(y, 12) / -l.high;
		const uint8_t level = (uint8_t)((q * l.span) / 4096 + l.base);
		*(uint32_t *)h = game_addr(l.glow_seq);
		*(uint16_t *)(h + 0x24) = (uint16_t)l.glow_mode;
		*(uint16_t *)(h + 4) = (uint16_t)frame;
		h[0x1E] = level;
		h[0x1D] = level;
		h[0x1C] = level;
		cursor = InitEffectSequenceFromData(h, RenderOT(), 2, cursor);
		FieldFree(0xB4);
	}

	struct EmberRolls
	{
		int32_t delay_mod, delay_step;
		Roll x, y, z, rise, scale;
	};
	inline void EmberStart(Ember *e, int16_t *delay, const int16_t *flame_pos, int32_t k, const EmberRolls &r)
	{
		*delay = (int16_t)(CrtRand() % r.delay_mod + r.delay_step * k);
		ff8fx::copy<(8)>(e->pos, flame_pos);
		e->pos[0] += (int16_t)r.x.of(CrtRand());
		e->pos[1] += (int16_t)r.y.of(CrtRand());
		e->pos[2] += (int16_t)r.z.of(CrtRand());
		e->vel_y = (int16_t)r.rise.of(CrtRand());
		e->scale = (int16_t)r.scale.of(CrtRand());
	}
	inline void EmberDraw(const Ember *p, int16_t frame, uint32_t seq, int32_t shade_shift, uint32_t &cursor)
	{
		TransformCameraByShadowRotation(p->pos, p->scale, -(p->scale >> shade_shift));
		uint8_t *h = AllocHeader(0xB4);
		*(uint16_t *)(h + 4) = (uint16_t)frame;
		*(uint32_t *)h = seq;
		*(uint16_t *)(h + 0x24) = 0;
		cursor = InitEffectSequenceFromData(h, RenderOT(), 2, cursor);
		FieldFree(0xB4);
	}
	inline void EmberRise(Ember *p, int32_t accel, int32_t shrink)
	{
		const int16_t v = p->vel_y;
		p->pos[1] = (int16_t)(p->pos[1] + v);
		p->vel_y = (int16_t)(v + (int16_t)(v >> accel));
		const int16_t s = p->scale;
		p->scale = (int16_t)(s - (int16_t)(s >> shrink));
	}

	struct SparkRolls
	{
		Roll life;
		int32_t delay_mod, delay_step;
		Roll x, y, z, vx, vy, vz, scale;
	};
	inline void FireSparkStart(FireSpark *s, int16_t *life, int16_t *delay, const int16_t *flame_pos, int32_t k, const SparkRolls &r)
	{
		*life = (int16_t)r.life.of(CrtRand());
		*delay = (int16_t)(CrtRand() % r.delay_mod + r.delay_step * k);
		ff8fx::copy<(8)>(s->pos, flame_pos);
		s->pos[0] += (int16_t)r.x.of(CrtRand());
		s->pos[1] += (int16_t)r.y.of(CrtRand());
		s->pos[2] += (int16_t)r.z.of(CrtRand());
		s->vel[0] = (int16_t)r.vx.of(CrtRand());
		s->vel[1] = (int16_t)r.vy.of(CrtRand());
		s->vel[2] = (int16_t)r.vz.of(CrtRand());
		s->scale = (int16_t)r.scale.of(CrtRand());
	}
	struct SparkLook
	{
		int32_t sprite, last, flag20, mode, shade_shift;
	};
	inline void FireSparkDraw(const FireSpark *p, int16_t life, const SparkLook &l, uint32_t &cursor)
	{
		TransformCameraByShadowRotation(p->pos, p->scale, -(p->scale >> l.shade_shift));
		uint8_t *h = AllocHeader(0xB4);
		*(void **)h = EffectSpriteSharedSequence(l.sprite);
		*(uint16_t *)(h + 4) = (uint16_t)(l.last - life);
		h[0x20] = (uint8_t)l.flag20;
		*(uint16_t *)(h + 0x24) = (uint16_t)l.mode;
		cursor = InitEffectSequenceFromData(h, RenderOT(), 2, cursor);
		FieldFree(0xB4);
	}
	inline void FireSparkMove(FireSpark *p, int16_t *life, int32_t drag, int32_t shrink)
	{
		int16_t v = p->vel[0];
		p->scale = (int16_t)(p->scale - shrink);
		p->pos[0] = (int16_t)(p->pos[0] + v);
		p->vel[0] = (int16_t)(v - (int16_t)(v >> drag));
		v = p->vel[1];
		p->pos[1] = (int16_t)(p->pos[1] + v);
		p->vel[1] = (int16_t)(v + (int16_t)(v >> drag));
		v = p->vel[2];
		p->pos[2] = (int16_t)(p->pos[2] + v);
		p->vel[2] = (int16_t)(v - (int16_t)(v >> drag));
		(*life)--;
	}

	inline uint32_t &Pause() { return var<uint32_t>(game_addr(game::unk_2556478)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::unk_255647C)); }
	inline uint32_t &CaptureCursor() { return var<uint32_t>(game_addr(game::unk_2556480)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::unk_2556484)); }
	inline uint8_t *&Caster() { return var<uint8_t *>(game_addr(game::unk_2556488)); }
	inline TaskQueue *QLayouts() { return (TaskQueue *)game_addr(game::unk_2556490); }
	inline TaskQueue *QTrail() { return (TaskQueue *)game_addr(game::unk_25564A0); }
	inline TaskQueue *QRocks() { return (TaskQueue *)game_addr(game::unk_25564B0); }
	inline TaskQueue *QSparkles() { return (TaskQueue *)game_addr(game::unk_25564C0); }
	inline TaskQueue *QMain() { return (TaskQueue *)game_addr(game::unk_25564D0); }
	inline TaskQueue *QFlames() { return (TaskQueue *)game_addr(game::unk_2556548); }
	inline uint32_t *ZeroAngles() { return (uint32_t *)game_addr(game::unk_2556520); }
	inline uint32_t *CasterPos() { return (uint32_t *)game_addr(game::unk_2556530); }
	inline int16_t *Centre() { return (int16_t *)game_addr(game::unk_2556538); }
	inline int32_t &FlashLevel() { return var<int32_t>(game_addr(game::unk_2556540)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::unk_2556558)); }
	inline Mat4x3 *OrthoView() { return (Mat4x3 *)game_addr(game::unk_2556560); }
	inline uint32_t OTBase() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)); }
	inline uint32_t &FramePackets() { return var<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)); }

	static constexpr game::Id ORIG_MasterTask = game::MAG_210_sub_6F6890;
	static constexpr game::Id ORIG_FlameSpawner = game::MAG_210_sub_6F6C00;
	static constexpr game::Id ORIG_FlameTask = game::MAG_210_sub_6F6DA0;
	static constexpr game::Id ORIG_SparkTask = game::sub_6F7130;
	static constexpr game::Id ORIG_EmberTask = game::sub_6F7230;
	static constexpr game::Id ORIG_MainTask = game::MAG_210_sub_6F7310;
	static constexpr game::Id ORIG_TrailTask = game::MAG_210_sub_6F82D0;
	static constexpr game::Id ORIG_LayoutTask = game::MAG_210_sub_6F9AA0;
	static constexpr game::Id ORIG_RockTask = game::MAG_210_sub_6F9B60;
	static constexpr game::Id ORIG_SparkleTask = game::MAG_210_sub_6F9C60;

	static const uint32_t STREAM_STATE = 0xA708;
	static const uint32_t POOL_Main = 0xA750;
	static const uint32_t POOL_Sparkles = 0x153C;
	static const uint32_t POOL_Rocks = 0x183C;
	static const uint32_t POOL_Trail = 0x1BBC;
	static const uint32_t STREAM_B = 0x1F08;
	static const uint32_t MORPH = 0xAB10;
	static const uint32_t ARENA_A = 0xB1B8;
	static const uint32_t ARENA_B = 0x1B1B8;
	static const uint32_t ARENA_C = 0x2B1B8;
	static constexpr game::Id LAYOUT_Rise = game::unk_146D8A4;
	static constexpr game::Id LAYOUT_Burn = game::unk_1462010;
	static constexpr game::Id LAYOUT_Drop = game::unk_146A668;
	static constexpr game::Id LAYOUT_End = game::unk_146ED2C;
	static constexpr game::Id KIND_Plain = game::a00000000000000_4;
	static constexpr game::Id KIND_Burn = game::a3220999;
	static constexpr game::Id MODEL_Bone = game::unk_145D6AC;
	static constexpr game::Id MODEL_Rock = game::unk_1461D98;
	static constexpr game::Id MODEL_Part = game::unk_145E1C4;
	static constexpr game::Id MODEL_Ring = game::unk_14603F4;
	static constexpr game::Id MODEL_PartVerts = game::unk_145E1CC;
	static constexpr game::Id SEQ_Flame = game::unk_14708C0;
	static constexpr game::Id SEQ_FlameGlow = game::unk_1470A6C;
	static constexpr game::Id SEQ_Ember = game::unk_1470C18;
	static constexpr game::Id SEQ_Sparkle = game::unk_145D4EC;
	static const void *SOUND_Start() { return (const void *)game_addr(game::unk_145D4DC); }
	static const void *SOUND_Ghost() { return (const void *)game_addr(game::unk_145D4E0); }
	static const void *SOUND_Lift() { return (const void *)game_addr(game::unk_145D4E4); }
	static const void *SOUND_Fall() { return (const void *)game_addr(game::unk_145D4E8); }
	static uint8_t *TRAIL_ENTITY() { return (uint8_t *)game_addr(game::unk_1D97530); }

	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t flag, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, flag, out); }
	inline int32_t ComputeCos(int32_t angle) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(angle); }
	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline void RotationY(int32_t a, Mat4x3 *out) { fn<void (__cdecl *)(int32_t, Mat4x3 *)>(game_addr(game::Effect_RotationYMatrix))(a, out); }
	inline void RotationX(int32_t a, Mat4x3 *out) { fn<void (__cdecl *)(int32_t, Mat4x3 *)>(game_addr(game::MAG_063_RotationXMatrix))(a, out); }
	inline void RotationZ(int32_t a, Mat4x3 *out) { fn<void (__cdecl *)(int32_t, Mat4x3 *)>(game_addr(game::MAG_065_RotationZMatrix))(a, out); }
	inline void ScaledRotY(int32_t a, int32_t s, Mat4x3 *out) { fn<void (__cdecl *)(int32_t, int32_t, Mat4x3 *)>(game_addr(game::GF_338Moomba_ScaledRotY))(a, s, out); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void MatrixMultiply3x3(Mat4x3 *a, const void *b) { fn<void (__cdecl *)(Mat4x3 *, const void *)>(game_addr(game::MatrixMultiply3x3))(a, b); }
	inline void MatVec(const Mat4x3 *m, const int16_t *in, int16_t *out) { fn<void (__cdecl *)(const Mat4x3 *, const int16_t *, int16_t *)>(game_addr(game::matrixMultiplyVector))(m, in, out); }
	inline void Apply3DTransform(const Mat4x3 *m, const int16_t *in, int32_t *out) { fn<void (__cdecl *)(const Mat4x3 *, const int16_t *, int32_t *)>(game_addr(game::apply3DTransform))(m, in, out); }
	inline void GteMVMVA_RotV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0))(); }
	inline void GteMVMVA_RotIR() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotIR))(); }
	inline void GteLoadIRFromColumn(const void *p) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadIRFromMatrixColumn))(p); }
	inline void GteStoreIRToColumn(void *p) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreIRToMatrixColumn))(p); }
	inline void GteLoadV0FromDwords(const void *p) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadV0FromDwords))(p); }
	inline void GteLoadIR123FromBytes(const void *p) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadIR123FromBytes))(p); }
	inline void GteStoreIR123LowBytes(void *p) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreIR123LowBytes))(p); }
	inline uint32_t RenderPrimSet(void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::MAG_063_RenderPrimSet))(h, ot, mode, cursor); }
	inline uint32_t RenderGeometry(uint32_t model, void *ctx, uint32_t ot, int32_t a, uint32_t cursor) { return fn<uint32_t (__cdecl *)(uint32_t, void *, uint32_t, int32_t, uint32_t)>(game_addr(game::RenderGeometry))(model, ctx, ot, a, cursor); }
	inline uint32_t SetDrawEnv(void *prim, uint32_t env) { return fn<uint32_t (__cdecl *)(void *, uint32_t)>(game_addr(game::SSIGPU_SetDrawEnv))(prim, env); }
	inline void SetDrawAreaPrim(void *prim, const int16_t *rect) { fn<void (__cdecl *)(void *, const int16_t *)>(game_addr(game::GPU_SetDrawAreaPrim))(prim, rect); }
	inline void SetDrawOffsetPrim(void *prim, const int16_t *xy) { fn<void (__cdecl *)(void *, const int16_t *)>(game_addr(game::SSIGPU_SetDrawOffsetPrim))(prim, xy); }
	inline void GetScreenOffset(int32_t *x, int32_t *y) { fn<void (__cdecl *)(int32_t *, int32_t *)>(game_addr(game::GTE_ReadScreenOffset))(x, y); }
	inline void SetScreenOffset(int32_t x, int32_t y) { fn<void (__cdecl *)(int32_t, int32_t)>(game_addr(game::Call_Bs_ParseCamera))(x, y); }
	inline void InsertPrimAltViewport(uint32_t bucket, void *packet) { fn<void (__cdecl *)(uint32_t, void *)>(game_addr(game::SSIGPU_InsertPrimAltViewport))(bucket, packet); }
	inline uint32_t QueueChainTransformation(uint8_t *entity, uint32_t id) { return fn<uint32_t (__cdecl *)(uint8_t *, uint32_t)>(game_addr(game::QueueChainTransformation))(entity, id); }
	inline uint32_t FadeTileInHoldOut(int32_t t, int32_t r, int32_t g, int32_t b, int32_t in, int32_t hold, int32_t out, uint32_t cursor) { return fn<uint32_t (__cdecl *)(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_FadeTileInHoldOut))(t, r, g, b, in, hold, out, cursor); }
	inline void SplineSetup(int32_t n, const void *points, void *work) { fn<void (__cdecl *)(int32_t, const void *, void *)>(game_addr(game::Effect_SplineSetup))(n, points, work); }
	inline void SplineEval(int32_t n, void *work, void *out, int32_t t) { fn<void (__cdecl *)(int32_t, void *, void *, int32_t)>(game_addr(game::Effect_SplineEval))(n, work, out, t); }
	inline void GteSetFarColor2(int32_t r, int32_t g, int32_t b) { fn<void (__cdecl *)(int32_t, int32_t, int32_t)>(game_addr(game::GTE_SetFarColorRGB))(r, g, b); }
	inline void GteLoadRGBC(const void *c) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadRGBC))(c); }
	inline void GteDPCS() { fn<void (__cdecl *)()>(game_addr(game::GTE_DPCS))(); }
	inline void GteStoreRGB2(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreRGB2))(dst); }
	inline void GteLoadV012(const void *v0, const void *v1, const void *v2) { fn<void (__cdecl *)(const void *, const void *, const void *)>(game_addr(game::GTE_LoadV012))(v0, v1, v2); }
	inline void GteRTPT() { fn<void (__cdecl *)()>(game_addr(game::GTE_RTPT))(); }
	inline void GteReadFLAG(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadFLAG))(dst); }
	inline void GteReadSXY012Split(void *a, void *b, void *c) { fn<void (__cdecl *)(void *, void *, void *)>(game_addr(game::GTE_ReadSXY012_Split))(a, b, c); }
	inline void GteAVSZ4() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ4))(); }
	inline void GteReadOTZ32(int32_t *p) { fn<void (__cdecl *)(int32_t *)>(game_addr(game::GTE_ReadOTZ))(p); }
	inline void GteLoadV0u(const void *v) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadV0_u16))(v); }
	inline void GteReadIR2(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadIR2Word))(dst); }
	inline void StreamQueueLoad(int32_t id, uint32_t dst, int32_t kind) { fn<void (__cdecl *)(int32_t, uint32_t, int32_t)>(game_addr(game::Stream_QueueLoad))(id, dst, kind); }
	inline void StreamPump() { fn<void (__cdecl *)()>(game_addr(game::Stream_Pump))(); }
	inline int32_t StreamBusy() { return fn<int32_t (__cdecl *)()>(game_addr(game::Stream_IsBusy))(); }
	inline int32_t StreamPreloadPending(int32_t n) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::au_re_BattleFile_preLoad_1))(n); }
	inline void GhostInit(uint8_t *g, int32_t n, int32_t bone) { fn<void (__cdecl *)(uint8_t *, int32_t, int32_t)>(game_addr(game::MAG_212_BURNINGRAVE_GhostInit))(g, n, bone); }
	inline void GhostVertices(uint8_t **cur, uint8_t **out, void *ctx) { fn<void (__cdecl *)(uint8_t **, uint8_t **, void *)>(game_addr(game::MAG_212_BURNINGRAVE_GhostVertices))(cur, out, ctx); }
	inline uint32_t GhostPolys(void *ctx, uint32_t ot, int32_t shift, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::MAG_274_DUEL_GhostPolys))(ctx, ot, shift, cursor); }
	inline void ModelBox(uint32_t model, int16_t *out) { fn<void (__cdecl *)(uint32_t, int16_t *)>(game_addr(game::Effect_ModelVertexBox))(model, out); }
	inline void RingPoint(int32_t angle, int32_t radius, int16_t *out) { fn<void (__cdecl *)(int32_t, int32_t, int16_t *)>(game_addr(game::MAG_210_sub_67BB70))(angle, radius, out); }
	inline void StageGroupsBit1Off() { fn<void (__cdecl *)()>(game_addr(game::MAG_065_GreatAttractor_ModelsFlag2Off))(); }

#pragma pack(push, 1)
	struct MasterNode { TaskNode hdr; int16_t counter; uint8_t running, parity; };
	struct SpawnerNode { TaskNode hdr; int16_t counter, w0E; uint8_t pad10[0x10]; int16_t slot, w22; };
	struct FlameNode { TaskNode hdr; int16_t frame, delay; Flame f; };
	struct SparkNode { TaskNode hdr; int16_t life, delay; FireSpark s; };
	struct EmberNode { TaskNode hdr; int16_t frame, delay; Ember e; };
	struct TargetRec
	{
		uint8_t *e;
		uint32_t pos01;
		uint32_t pos23;
		int16_t ang[4];
		int16_t height[2];
	};
	struct MainNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t ntargets;
		TargetRec recs[3];
		int16_t centre[4];
		uint8_t ghosts[0x108];
		uint8_t layout[0x258];
	};
	struct RingEntry { uint32_t alive; int16_t base[4]; int16_t tip[4]; };
	struct TrailNode
	{
		TaskNode hdr;
		int16_t counter, pad0E;
		RingEntry ring[7];
		int16_t pts_a[8][4], pts_b[8][4];
		int16_t spl_a[32][4], spl_b[32][4];
		int16_t first_b[4], first_a[4];
		int32_t dir_b[4], dir_ab[4];
	};
	struct TrailWork { uint8_t pad00[8]; int32_t otz; uint32_t flag, col0, col1; uint8_t pad18[4]; uint32_t base_col; int16_t base[4], tip[4]; int32_t dir[4]; };
	struct LayoutNode { TaskNode hdr; int16_t pos[4]; uint8_t layout[0x700]; };
	struct RockNode
	{
		TaskNode hdr;
		int16_t pos[3];
		int16_t vy;
		int16_t life;
		int8_t ax, ay;
		int8_t dax, day;
		int16_t scale;
	};
	struct SparkleNode { TaskNode hdr; int16_t pos[3]; int16_t frame; int16_t vy; int16_t pad16; };
	struct PlayArg
	{
		Mat4x3 m;
		uint32_t kind;
		int16_t depth;
		uint16_t scroll;
		uint16_t w28;
		uint16_t pad2A;
		uint32_t morph;
	};
	struct QuadHeader
	{
		uint32_t data;
		uint32_t verts;
		uint8_t pad08[0xA];
		uint16_t vscroll;
		uint16_t win_a[4];
		uint16_t win_b[4];
		uint32_t list;
		uint8_t pad28[8];
		int32_t otz;
		uint32_t flag;
		int32_t depth;
		uint32_t v[4][2];
		uint32_t col[4];
		uint32_t uv01[2];
		uint16_t uv2, uv3;
		uint8_t pad78[0x20];
	};
#pragma pack(pop)
	static_assert(sizeof(MainNode) == 0x3C0 && sizeof(TargetRec) == 0x18 && sizeof(TrailNode) == 0x34C && sizeof(LayoutNode) == 0x714, "Bloodfest nodes");
	static_assert(sizeof(RockNode) == 0x1C && sizeof(SparkleNode) == 0x18 && sizeof(PlayArg) == 0x30 && sizeof(QuadHeader) == 0x98 && sizeof(TrailWork) == 0x40, "Bloodfest blocks");

	static const FlameRolls FLAME_ROLLS = { 6, 2, 2800, 1000, 0x226, { -500, 300, -1 }, { 0xD00, 0x600, 1 } };
	static const FlameLook FLAME_LOOK = { SEQ_Flame, SEQ_FlameGlow, 4, 0x400, 0xC, -2000, 160, 0xA0 };
	static const EmberRolls EMBER_ROLLS = { 6, 1, { -150, 300, 1 }, { 0, 100, -1 }, { -150, 300, 1 }, { -20, 30, -1 }, { 0xA00, 0x500, 1 } };
	static const SparkRolls SPARK_ROLLS = { { 5, 4, 1 }, 4, 1, { -200, 400, 1 }, { 0, 200, -1 }, { -200, 400, 1 }, { -40, 80, 1 }, { -20, 50, -1 }, { -40, 80, 1 }, { 0x900, 0x600, 1 } };
	static const SparkLook SPARK_LOOK = { 5, 9, 4, 0x10, 3 };
}
}

namespace ff8fx
{
namespace blood210
{
	using namespace eng;
	using namespace magc;

	inline int32_t SlotOf(uint8_t *e) { return (int32_t)((e - (uint8_t *)game_addr(game::BattleEntitySlotData)) / 0x9C); }

	static void StartFlameSpawner(uint8_t *e)
	{
		SpawnerNode *s = (SpawnerNode *)AddTaskToQueue(QFlames(), game_addr(ORIG_FlameSpawner));
		if (!s) return;
		s->counter = 0;
		s->w0E = 0;
		s->w22 = 0;
		s->slot = (int16_t)SlotOf(e);
	}

	static void MasterSetup()
	{
		StreamStateInit((void *)(TexBase() + STREAM_STATE));
		InitTaskQueuePool(QMain(), (void *)(TexBase() + POOL_Main), 0x3C0, 1);
		InitTaskQueuePool(QFlames(), (void *)TexBase(), 0x24, 0x96);
		MainNode *m = (MainNode *)AddTaskToQueue(QMain(), game_addr(ORIG_MainTask));
		Memset32(&m->counter, 0, 0xED);
		int16_t minx = 0x7FFF, miny = 0x7FFF, minz = 0x7FFF, maxx = -0x7FFF, maxy = -0x7FFF, maxz = -0x7FFF;
		const ActionData *ad = Ctx()->actions;
		m->ntargets = (int16_t)ad->target_count;
		for (int32_t k = 0; k < m->ntargets; k++)
		{
			TargetRec *r = &m->recs[k];
			uint8_t *e = Entity(Ctx()->actions->targets[k * TARGET_STRIDE]);
			r->e = e;
			r->pos01 = *(const uint32_t *)(e + 0x1C);
			r->pos23 = *(const uint32_t *)(e + 0x20);
			ff8fx::copy<(8)>(r->ang, ZeroAngles());
			int16_t p[4];
			GetDefaultEffectPosition(e, p);
			if (p[0] < minx) minx = p[0];
			if (p[0] > maxx) maxx = p[0];
			if (p[1] < miny) miny = p[1];
			if (p[1] > maxy) maxy = p[1];
			if (p[2] < minz) minz = p[2];
			if (p[2] > maxz) maxz = p[2];
			StartFlameSpawner(e);
		}
		Centre()[0] = m->centre[0] = (int16_t)((maxx + minx) / 2);
		Centre()[1] = m->centre[1] = (int16_t)((maxy + miny) / 2);
		Centre()[2] = m->centre[2] = (int16_t)((maxz + minz) / 2);
		CasterPos()[0] = *(const uint32_t *)(Caster() + 0x1C);
		CasterPos()[1] = *(const uint32_t *)(Caster() + 0x20);
	}

	static uint32_t __cdecl MasterTask(TaskNode *n)
	{
		MasterNode *ms = (MasterNode *)n;
		if (ms->parity)
		{
			PacketCursor() = TexBase() + ARENA_A;
			CaptureCursor() = TexBase() + ARENA_B;
			ms->parity = 0;
		}
		else
		{
			PacketCursor() = TexBase() + ARENA_B;
			CaptureCursor() = TexBase() + ARENA_C;
			ms->parity = 1;
		}
		FlashLevel() = 0;
		if (ms->counter == 1 && Pause() == 0 && !ms->running)
		{
			ms->running = 1;
			MasterSetup();
		}
		int main_alive = 1, tasks_alive = 1;
		if (ms->running)
		{
			EffectCameraMatrix(&Camera(), OrthoView());
			main_alive = ExecuteTaskQueue(QMain());
			ExecuteTaskQueue(QFlames());
			ExecuteTaskQueue(QSparkles());
			ExecuteTaskQueue(QRocks());
			ExecuteTaskQueue(QTrail());
			tasks_alive = ExecuteTaskQueue(QLayouts());
		}
		SetScreenFlash((uint32_t)FlashLevel(), 0);
		if (Pause() != 0) return 0;
		if (ms->running && main_alive == 0 && tasks_alive == 0)
		{
			SetScreenFlash(0, 0);
			return TASK_END;
		}
		ms->counter++;
		return 0;
	}

	static uint32_t __cdecl FlameSpawner(TaskNode *n)
	{
		if (Pause() != 0) return 0;
		SpawnerNode *s = (SpawnerNode *)n;
		if (s->counter == 1)
		{
			const int32_t spread = FlameSpread(EntitySize(s->slot), FLAME_ROLLS);
			int16_t pos[4];
			GetDefaultEffectPosition(Entity(s->slot), pos);
			for (int32_t k = 0; k < 4; k++)
			{
				FlameNode *f = (FlameNode *)AddTaskToQueue(QFlames(), game_addr(ORIG_FlameTask));
				if (!f) continue;
				f->frame = 0;
				FlameStart(&f->f, &f->delay, pos, EntityHeight(s->slot), spread, k, FLAME_ROLLS);
			}
		}
		if (s->counter == 6)
		{
			int16_t pos[4];
			GetDefaultEffectPosition(Entity(s->slot), pos);
		}
		s->counter++;
		return s->counter >= 0x16 ? TASK_END : 0;
	}

	static uint32_t __cdecl FlameTask(TaskNode *n)
	{
		FlameNode *p = (FlameNode *)n;
		if (p->delay > 0)
		{
			if (Pause() != 0) return 0;
			p->delay--;
			return 0;
		}
		FlameDraw(&p->f, p->frame, FLAME_LOOK, PacketCursor());
		if (Pause() != 0) return 0;
		if (p->frame == 0)
		{
			for (int32_t k = 0; k < 4; k++)
			{
				EmberNode *m = (EmberNode *)AddTaskToQueue(QFlames(), game_addr(ORIG_EmberTask));
				if (!m) continue;
				m->frame = 0;
				EmberStart(&m->e, &m->delay, p->f.pos, k, EMBER_ROLLS);
			}
			for (int32_t k = 0; k < 10; k++)
			{
				SparkNode *m = (SparkNode *)AddTaskToQueue(QFlames(), game_addr(ORIG_SparkTask));
				if (!m) continue;
				FireSparkStart(&m->s, &m->life, &m->delay, p->f.pos, k, SPARK_ROLLS);
			}
		}
		p->frame++;
		return p->frame >= 0x10 ? TASK_END : 0;
	}

	static uint32_t __cdecl SparkTask(TaskNode *n)
	{
		SparkNode *p = (SparkNode *)n;
		if (p->delay > 0)
		{
			if (Pause() != 0) return 0;
			p->delay--;
			return 0;
		}
		FireSparkDraw(&p->s, p->life, SPARK_LOOK, PacketCursor());
		if (Pause() != 0) return 0;
		FireSparkMove(&p->s, &p->life, 4, 0x40);
		return p->life < 0 ? TASK_END : 0;
	}

	static uint32_t __cdecl EmberTask(TaskNode *n)
	{
		EmberNode *p = (EmberNode *)n;
		if (p->delay > 0)
		{
			if (Pause() != 0) return 0;
			p->delay--;
			return 0;
		}
		EmberDraw(&p->e, p->frame, game_addr(SEQ_Ember), 3, PacketCursor());
		if (Pause() != 0) return 0;
		EmberRise(&p->e, 5, 6);
		p->frame++;
		return p->frame >= 0x10 ? TASK_END : 0;
	}

	static int16_t g_off_pad = 0;
	static void __cdecl PrimCallback(prim::Layout *l, prim::Record *r, int arg_)
	{
		const PlayArg *arg = (const PlayArg *)arg_;
		if (((int32_t)r->scale[2] | *(const int32_t *)r->scale) == 0) return;
		if (r->a >= 0x1000 && *(const uint32_t *)r->rgb == 0) return;
		uint8_t *h = (uint8_t *)FieldAlloc(0x6C);
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
		Mat4x3 m;
		RotationFromAngles(r->rot, &m);
		const int16_t off[4] = { r->pos[0], r->pos[1], r->pos[2], g_off_pad };
		if (r->flags & 0x1000)
		{
			GteSetRotMatrixCtrl(OrthoView());
			GteLoadV0(off);
			GteMVMVA_RotV0();
			int32_t t[3];
			GteReadMAC123(t);
			m.t[0] = t[0];
			m.t[1] = t[1];
			m.t[2] = t[2];
			MatrixMultiply(OrthoView(), &m);
		}
		else
		{
			GteSetRotMatrixCtrl(&arg->m);
			GteLoadV0(off);
			GteMVMVA_RotV0();
			GteReadMAC123(m.t);
			MatrixMultiply(&arg->m, &m);
		}
		m.t[0] = (int32_t)((uint32_t)m.t[0] + (uint32_t)arg->m.t[0]);
		m.t[1] = (int32_t)((uint32_t)m.t[1] + (uint32_t)arg->m.t[1]);
		m.t[2] = (int32_t)((uint32_t)m.t[2] + (uint32_t)arg->m.t[2]);
		if (!(*(const uint32_t *)r->scale == 0x10001000 && r->scale[2] == 0x1000))
		{
			if (r->flags & 0x100)
			{
				Mat4x3 s;
				s.m[1][1] = r->scale[1];
				s.m[0][0] = r->scale[0];
				s.m[1][2] = 0;
				s.m[0][2] = 0;
				s.m[2][1] = 0;
				s.m[0][1] = 0;
				s.m[2][0] = 0;
				s.m[1][0] = 0;
				s.m[2][2] = r->scale[2];
				MatrixMultiply3x3(&m, &s);
			}
			else
			{
				const int32_t v[3] = { r->scale[0], r->scale[1], r->scale[2] };
				Scale3DMatrix(&m, v);
			}
		}
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		*(uint32_t *)(h + 0x1C) = (r->flags & 0x4000) ? 0x2000 : 0x2030;
		if (r->flags & 0x2000) *(uint8_t *)(h + 0x1C) |= 0xC;
		*(int32_t *)(h + 0xC) = r->a;
		if (r->a != 0)
		{
			*(uint8_t *)(h + 0x1C) |= 0xC0;
			*(uint32_t *)(h + 8) = *(const uint32_t *)r->rgb;
		}
		*(int32_t *)(h + 0x18) = arg->depth;
		*(uint16_t *)(h + 0x22) = 0;
		*(uint16_t *)(h + 0x20) = 0;
		*(uint16_t *)(h + 0x26) = 0;
		*(uint16_t *)(h + 0x24) = 0;
		*(uint16_t *)(h + 0x2A) = 0x100;
		*(uint16_t *)(h + 0x28) = 0x100;
		const uint8_t kind = ((const uint8_t *)arg->kind)[object];
		if (kind == 0x31)
		{
			*(uint16_t *)(h + 0x2C) = 0;
			*(uint16_t *)(h + 0x2E) = 0;
			*(uint16_t *)(h + 0x32) = 0x80;
			*(uint16_t *)(h + 0x30) = 0x100;
			*(uint16_t *)(h + 0x22) = (uint16_t)(arg->scroll & 0x7F);
		}
		else if (kind == 0x32)
		{
			*(uint16_t *)(h + 0x2C) = 0;
			*(uint16_t *)(h + 0x2E) = 0x80;
			*(uint16_t *)(h + 0x32) = 0x80;
			*(uint16_t *)(h + 0x30) = 0x100;
			*(uint16_t *)(h + 0x22) = (uint16_t)(arg->scroll & 0x7F);
		}
		else if (kind == 0x33)
		{
			*(uint16_t *)(h + 0x2C) = 0;
			*(uint16_t *)(h + 0x2E) = 0;
			*(uint16_t *)(h + 0x32) = 0x100;
			*(uint16_t *)(h + 0x30) = 0x80;
			*(uint16_t *)(h + 0x20) = (uint16_t)(arg->w28 & 0x7F);
		}
		else if (kind == 0x39) *(int32_t *)(h + 0x18) = 0x80;
		PacketCursor() = RenderPrimSet(h, OTBase() + 0x44, 2, PacketCursor());
		FieldFree(0x6C);
	}

	static void Play(MainNode *mn, int site, uint8_t *layout, PlayArg *arg, int32_t x, int32_t z)
	{
		RotationY(0, &arg->m);
		arg->m.t[0] = x;
		arg->m.t[1] = 0;
		arg->m.t[2] = z;
		ComposeAffineTransform(&Camera(), &arg->m, &arg->m);
		arg->morph = TexBase() + MORPH;
		prim::play((prim::Layout *)layout, PrimCallback, (int)arg, (int)Pause());
	}

	static void GhostRecord(uint8_t *entity, uint8_t *g)
	{
		g[0] = 1;
		const uint8_t s = g[2];
		g[3] = s;
		uint8_t *dst = g + 0x88 + (uint32_t)s * 0x40;
		g[2] = (uint8_t)(s + 1);
		if ((uint8_t)(s + 1) >= 2) g[2] = 0;
		g[1] = (uint8_t)(g[1] + 1);
		if (g[1] > 2) g[1] = 2;
		uint8_t *skel = **(uint8_t ***)(entity + 0x64);
		for (uint32_t b = 0; b < skel[0]; b++)
		{
			const int32_t k = (int8_t)g[5 + b];
			if (k >= 0) ff8fx::copy<(0x20)>(dst + k * 0x20, skel + 0x20 + b * 0x30);
		}
	}

#pragma pack(push, 1)
	struct GhostCtx
	{
		uint32_t polys;
		uint32_t verts;
		int16_t ntri, nquad;
		int16_t clip[4];
		uint32_t colour;
		uint32_t visible;
		uint32_t v[8];
		int32_t mac0;
		uint32_t pad40;
		int32_t otz;
		uint8_t pad48[4];
		uint8_t *ghosts;
		uint32_t slot;
	};
#pragma pack(pop)
	static_assert(sizeof(GhostCtx) == 0x54, "afterimage context");

	static void GhostModel(uint8_t *model, GhostCtx *c, uint32_t ot, int32_t shift)
	{
		const uint8_t *geom = *(uint8_t **)(model + 4);
		const int32_t nobj = *(const int32_t *)geom;
		const uint32_t *offs = (const uint32_t *)(geom + 4);
		for (int32_t i = 0; i < nobj; i++)
		{
			uint8_t *obj = (uint8_t *)(offs[i] + *(uint32_t *)(model + 4));
			if (!(c->visible & (1u << i))) continue;
			uint8_t *out = (uint8_t *)c->verts;
			uint8_t *cur = obj + 2;
			int32_t groups = *(const int16_t *)obj;
			for (; groups > 0; groups--)
			{
				const int32_t bone = *(const int16_t *)cur;
				cur += 2;
				const int32_t k = (int8_t)c->ghosts[bone + 5];
				if (k >= 0)
				{
					const Mat4x3 *m = (const Mat4x3 *)(c->slot + (uint32_t)(k + 1) * 0x20);
					GteSetRotMatrixCtrl(m);
					GteSetTransVectorCtrl(m);
					GhostVertices(&cur, &out, c);
				}
				else
				{
					const int32_t nv = *(const int16_t *)cur;
					cur += nv * 6 + 2;
					for (int32_t v = nv; v != 0; v--)
					{
						*(uint16_t *)(out + 6) = 0x10;
						out += 8;
					}
				}
			}
			cur = (uint8_t *)(((uint32_t)cur + 3) & ~3u);
			c->ntri = *(const int16_t *)cur;
			cur += 2;
			c->nquad = *(const int16_t *)cur;
			cur += 0xA;
			c->polys = (uint32_t)cur;
			PacketCursor() = GhostPolys(c, ot, shift, PacketCursor());
		}
	}

	static void GhostDraw(uint8_t *entity, uint8_t *g)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x74);
		GhostCtx *c = (GhostCtx *)(h + 0x20);
		c->verts = var<uint32_t>(game_addr(game::g_BattleVertexWorkspace));
		c->clip[0] = 0;
		c->clip[1] = 0;
		c->clip[2] = 0x140;
		c->clip[3] = 0xD8;
		c->visible = *(const uint32_t *)(entity + 0x7C);
		ComposeAffineTransform(&Camera(), (const Mat4x3 *)(entity + 0x40), (Mat4x3 *)h);
		c->ghosts = g;
		int32_t w = 0x800;
		for (int32_t i = 0; i < g[1]; i++)
		{
			GteSetIR0((w >> 1) + w);
			GteLoadIR123FromBytes(entity + 0x28);
			GteGPF();
			GteStoreIR123LowBytes(&c->colour);
			*((uint8_t *)c + 0x17) = 0;
			c->slot = (uint32_t)g + 0x88 + (((uint32_t)g[3] - (uint32_t)i) & 1) * 0x40;
			for (int32_t b = 0; b < g[4]; b++)
			{
				const uint32_t r = c->slot + (uint32_t)b * 0x20;
				GteSetRotMatrixCtrl((const Mat4x3 *)h);
				GteLoadIRFromColumn((const void *)(r + 0));
				GteMVMVA_RotIR();
				GteStoreIRToColumn((void *)(r + 0x20));
				GteLoadIRFromColumn((const void *)(r + 2));
				GteMVMVA_RotIR();
				GteStoreIRToColumn((void *)(r + 0x22));
				GteLoadIRFromColumn((const void *)(r + 4));
				GteMVMVA_RotIR();
				GteStoreIRToColumn((void *)(r + 0x24));
				GteSetTransVectorCtrl((const Mat4x3 *)h);
				GteLoadV0FromDwords((const void *)(r + 0x14));
				GteMVMVA_RotV0Tr();
				GteReadMAC123((int32_t *)(r + 0x34));
			}
			GhostModel(*(uint8_t **)(entity + 0x64), c, OTBase() + 0x44, 0);
			w -= 0x400;
		}
		FieldFree(0x74);
		if (Pause() != 0) return;
		if (g[0] == 0 && g[1] != 0) g[1]--;
		g[0] = 0;
	}

	static void MeasureHeight(uint8_t *e, int16_t *out)
	{
		ComputeBonesWorldMatrices(e + 0x60, e + 0x40);
		uint8_t *h = (uint8_t *)FieldAlloc(0x18);
		*(uint32_t *)(h + 4) = 0xFFFF0000;
		*(uint32_t *)h = 0x10000;
		*(uint32_t *)(h + 0x14) = *(const uint32_t *)(e + 0x7C);
		const uint8_t *model = *(uint8_t *const *)(e + 0x64);
		const uint8_t *bones = *(const uint8_t *const *)model + 0x10;
		*(uint32_t *)(h + 8) = 0;
		*(uint16_t *)(h + 0xC) = 0x1000;
		const uint8_t *objs = *(const uint8_t *const *)(model + 4);
		const int32_t count = *(const int32_t *)objs;
		const uint32_t *offs = (const uint32_t *)(objs + 4);
		for (int32_t i = 0; i < count; i++)
		{
			const uint8_t *p = *(const uint8_t *const *)(model + 4) + offs[i];
			if (!(*(const uint32_t *)(h + 0x14) & (1u << (i & 31)))) continue;
			int32_t ng = *(const int16_t *)p;
			p += 2;
			if (ng <= 0) continue;
			do
			{
				const int32_t bone = *(const int16_t *)p;
				p += 2;
				const Mat4x3 *bm = (const Mat4x3 *)(bones + (uint32_t)bone * 0x30 + 0x10);
				GteSetRotMatrixCtrl(bm);
				GteSetTransVectorCtrl(bm);
				int32_t nv = *(const int16_t *)p;
				p += 2;
				for (; nv != 0; nv--)
				{
					GteLoadV0u(p);
					GteRTPS();
					p += 6;
					GteReadIR2(h + 0x10);
					const int32_t v = *(const int16_t *)(h + 0x10);
					if (*(const int32_t *)h > v) *(int32_t *)h = v;
					if (*(const int32_t *)(h + 4) < v) *(int32_t *)(h + 4) = v;
				}
			} while (--ng != 0);
		}
		out[0] = *(const int16_t *)(h + 4);
		out[1] = *(const int16_t *)h;
		FieldFree(0x18);
		BuildBoneMatricesFromPose(e + 0x60);
	}

	static void DrawEntity(uint8_t *e, const int16_t *angles, const int16_t *off)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x5C);
		Mat4x3 *m = (Mat4x3 *)h;
		ComposeZYXRotationMatrix(angles, m);
		m->t[0] = off[0];
		m->t[1] = off[1];
		m->t[2] = off[2];
		ComposeAffineTransform((const Mat4x3 *)(e + 0x40), m, m);
		ComposeAffineTransform(&Camera(), m, m);
		Apply3DTransform(m, off, (int32_t *)(h + 0x20));
		m->t[1] = (int32_t)((uint32_t)m->t[1] - *(const uint32_t *)(h + 0x24));
		m->t[0] = (int32_t)((uint32_t)m->t[0] - *(const uint32_t *)(h + 0x20));
		m->t[2] = (int32_t)((uint32_t)m->t[2] - *(const uint32_t *)(h + 0x28));
		ComputeBonesWorldMatrices(e + 0x60, m);
		*(uint32_t *)(h + 0x34) = var<uint32_t>(game_addr(game::g_BattleVertexWorkspace));
		*(uint16_t *)(h + 0x44) = 0;
		*(uint16_t *)(h + 0x46) = 0;
		*(uint16_t *)(h + 0x48) = 0x140;
		*(uint16_t *)(h + 0x4A) = 0xD8;
		*(uint32_t *)(h + 0x4C) = *(const uint32_t *)(e + 0x28);
		*(uint32_t *)(h + 0x58) = 0;
		*(uint32_t *)(h + 0x50) = *(const uint32_t *)(e + 0x7C);
		*(uint32_t *)(h + 0x40) = var<uint32_t>(game_addr(game::BATTLE_PACKET_BUFFER_END));
		FramePackets() = RenderGeometry(*(const uint32_t *)(e + 0x64), h + 0x30, OTBase() + 0x44, 4, FramePackets());
		BuildBoneMatricesFromPose(e + 0x60);
		uint8_t *w = *(uint8_t **)(e + 0x78);
		if (w)
		{
			*(uint32_t *)(h + 0x50) = 0xFFFFFFFF;
			ComputeBonesWorldMatrices(w, m);
			FramePackets() = RenderGeometry(*(const uint32_t *)(w + 4), h + 0x30, OTBase() + 0x44, 4, FramePackets());
			BuildBoneMatricesFromPose(w);
		}
		FieldFree(0x5C);
	}

	static void CaptureStage(const int16_t *a, const int16_t *b)
	{
		const int32_t b0 = -(int32_t)b[0];
		const int32_t b3 = b[3];
		const int32_t nb2 = -(int32_t)b[2];
		const int32_t b5 = b[5];
		const int32_t w = b3 + b0;
		const int32_t dz = b5 + nb2;
		SetScreenFlash(0x8FC, 0);
		int16_t rect[4] = { 0x2C0, 0x100, 0xC0, 0x100 };
		int32_t sx = w / 0xC0;
		int32_t sz = dz / 0x100;
		if (sx % 0xC0 != 0) sx++;
		if (sz % 0x100 != 0) sz++;
		Mat4x3 m, m2;
		m.t[1] = 0;
		m.t[0] = -((int32_t)a[0] / sx);
		m.t[2] = (int32_t)a[2] / sz;
		RotationY(0, &m);
		TransformVectorBy3x3Matrix(&m, m.t, m.t);
		RotationX(-0x400, &m2);
		MatrixMultiply3x3(&m2, &m);
		int32_t s3[3];
		s3[1] = 0;
		m2.t[0] = (b0 - b3) / 2 / sx + m.t[0];
		m2.t[2] = (int32_t)var<int16_t>(game_addr(game::BATTLE_PROJECTION_DISTANCE));
		m2.t[1] = m.t[2] - ((nb2 - b5) / 2 / sz);
		s3[0] = 0x1000000 / (shl32(w, 12) / rect[2]);
		s3[2] = 0x1000000 / (shl32(dz, 12) / rect[3]);
		Scale3DMatrix(&m2, s3);
		uint8_t *env = (uint8_t *)PacketCursor();
		PacketCursor() += 0x40;
		SetDrawEnv(env, game_addr(game::g_Battle_DrawEnv) + (uint32_t)(var<uint8_t>(game_addr(game::g_Battle_DisplayParity)) ^ 1) * 0x5C);
		InsertPrimAutoDepth(OTBase() + 0x44, env);
		int32_t ox, oy;
		GetScreenOffset(&ox, &oy);
		SetScreenOffset(rect[2] / 2, rect[3] / 2);
		GteSetRotMatrix(&m2);
		GteSetTransVector(&m2);
		if (var<uint8_t>(game_addr(game::unk_1D989BD)) & 1)
		{
			uint8_t *h = (uint8_t *)FieldAlloc(0x2C);
			*(uint16_t *)(h + 0x14) = 0;
			*(uint16_t *)(h + 0x16) = 0;
			*(uint16_t *)(h + 0x18) = 0x140;
			*(uint16_t *)(h + 0x1A) = 0xD8;
			*(uint16_t *)(h + 0x24) = var<uint16_t>(game_addr(game::unk_1D989BE));
			*(uint32_t *)(h + 0x1C) = var<uint32_t>(game_addr(game::unk_1D989E4));
			*(uint32_t *)(h + 4) = var<uint32_t>(game_addr(game::g_BattleVertexWorkspace));
			*(uint32_t *)(h + 0x20) = 0xFFFFFFFF;
			*(uint32_t *)(h + 0x28) = 0;
			*(uint32_t *)(h + 0x10) = CaptureCursor();
			BuildBoneMatricesFromPose((void *)game_addr(game::unk_1D989D0));
			ComputeBonesWorldMatrices((void *)game_addr(game::unk_1D989D0), &m2);
			PacketCursor() = RenderGeometry(game_addr(game::unk_1D989C0), h, OTBase() + 0x44, 0x10, PacketCursor());
			FieldFree(0x2C);
		}
		uint8_t *q = (uint8_t *)PacketCursor();
		PacketCursor() += 0x18;
		*(uint16_t *)(q + 8) = 0;
		*(uint16_t *)(q + 0xA) = 0;
		*(uint16_t *)(q + 0xC) = 0xC0;
		*(uint16_t *)(q + 0xE) = 0;
		*(uint16_t *)(q + 0x10) = 0;
		*(uint16_t *)(q + 0x12) = 0x100;
		*(uint16_t *)(q + 0x14) = 0xC0;
		*(uint16_t *)(q + 0x16) = 0x100;
		q[4] = 0;
		q[5] = 0;
		q[6] = 0;
		*(uint32_t *)q = 0x05000000;
		q[7] = 0x28;
		InsertPrimAltViewport(OTBase() + 0x44, q);
		SetScreenOffset(ox, oy);
		uint8_t *area = (uint8_t *)PacketCursor();
		PacketCursor() += 0xC;
		SetDrawAreaPrim(area, rect);
		InsertPrimAutoDepth(OTBase() + 0x44, area);
		int16_t xy[2] = { rect[0], rect[1] };
		uint8_t *offp = (uint8_t *)PacketCursor();
		PacketCursor() += 0xC;
		SetDrawOffsetPrim(offp, xy);
		InsertPrimAutoDepth(OTBase() + 0x44, offp);
	}

	static uint32_t TexWindow(const uint16_t *w)
	{
		uint32_t e = ((uint32_t)(uint8_t)w[1] & 0xF8) | 0xFFFE2000u;
		e = (e << 5) | ((uint32_t)(uint8_t)w[0] & 0xF8);
		e = (e << 5) | (~((uint32_t)w[3] - 1) & 0xF8);
		e = (e << 2) | (((int32_t)~((uint32_t)w[2] - 1) >> 3) & 0x1F);
		return e;
	}
	static void QuadColour(QuadHeader *h, int k, int32_t z, uint32_t col, uint32_t none)
	{
		const int32_t d = h->depth + z;
		if (d <= 0)
		{
			h->col[k] = none;
			return;
		}
		h->col[k] = col;
		const int32_t f = shl32(d, 4);
		if (f < 0x1000)
		{
			GteSetIR0(f);
			GteLoadIR123FromBytes(&h->col[k]);
			GteGPF();
			GteStoreIR123LowBytes(&h->col[k]);
		}
	}
	static uint32_t RenderQuads(QuadHeader *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		const uint32_t *list = (const uint32_t *)h->list;
		int32_t n = (int32_t)list[0];
		const uint8_t *rec = (const uint8_t *)(list + 1);
		h->list = (uint32_t)rec;
		const uint8_t *verts = (const uint8_t *)h->verts;
		uint8_t *pk = (uint8_t *)cursor;
		for (; n > 0; n--, rec += 0x24)
		{
			for (int k = 0; k < 4; k++)
			{
				const uint8_t *v = verts + (uint32_t)*(const uint16_t *)(rec + 4 + 2 * k) * 4;
				h->v[k][0] = *(const uint32_t *)v;
				h->v[k][1] = *(const uint32_t *)(v + 4);
				const uint32_t col = k == 0 ? *(const uint32_t *)rec : *(const uint32_t *)(rec + 0x14 + 4 * k);
				QuadColour(h, k, *(const int16_t *)&h->v[k][1], col, k == 0 ? 0x3E000000u : 0u);
			}
			h->uv01[0] = *(const uint32_t *)(rec + 0xC);
			h->uv01[1] = *(const uint32_t *)(rec + 0x10);
			h->uv2 = *(const uint16_t *)(rec + 0x14);
			const uint32_t vs = h->vscroll;
			uint32_t v2 = (uint32_t)((const uint8_t *)&h->uv2)[1] + vs;
			uint32_t v0 = (uint32_t)((const uint8_t *)h->uv01)[1] + vs;
			h->uv3 = *(const uint16_t *)(rec + 0x16);
			uint32_t v3 = (uint32_t)((const uint8_t *)&h->uv3)[1] + vs;
			uint32_t v1 = (uint32_t)((const uint8_t *)&h->uv01[1])[1] + vs;
			if ((int32_t)(v3 | v2 | v1 | v0) > 0xFF)
			{
				const uint8_t wrap = (uint8_t)h->win_b[2];
				v0 = (uint8_t)(v0 - wrap);
				v1 = (uint8_t)(v1 - wrap);
				v2 = (uint8_t)(v2 - wrap);
				v3 = (uint8_t)(v3 - wrap);
			}
			((uint8_t *)h->uv01)[1] = (uint8_t)v0;
			((uint8_t *)&h->uv01[1])[1] = (uint8_t)v1;
			((uint8_t *)&h->uv2)[1] = (uint8_t)v2;
			((uint8_t *)&h->uv3)[1] = (uint8_t)v3;
			GteLoadV012(h->v[0], h->v[1], h->v[2]);
			GteRTPT();
			*(uint32_t *)pk = 0x0C000000;
			*(uint32_t *)(pk + 0xC) = h->uv01[0];
			*(uint32_t *)(pk + 0x18) = h->uv01[1];
			const uint32_t uv23 = (uint32_t)h->uv2 | ((uint32_t)h->uv3 << 16);
			*(uint32_t *)(pk + 0x24) = uv23;
			*(uint32_t *)(pk + 0x30) = uv23 >> 16;
			GteReadFLAG(&h->flag);
			if (h->flag & 0x60000) continue;
			GteReadSXY012Split(pk + 8, pk + 0x14, pk + 0x20);
			GteLoadV0(h->v[3]);
			GteRTPS();
			uint32_t clip = 0;
			int16_t c = *(const int16_t *)(pk + 8);
			if (c < 0 || c > 0xA00) clip = 1;
			c = *(const int16_t *)(pk + 0x14);
			if (c < 0 || c > 0xA00) clip |= 2;
			c = *(const int16_t *)(pk + 0x20);
			if (c < 0 || c > 0xA00) clip |= 4;
			c = *(const int16_t *)(pk + 0xA);
			if (c < 0 || c > 0x6C0) clip |= 0x10;
			c = *(const int16_t *)(pk + 0x16);
			if (c < 0 || c > 0x6C0) clip |= 0x20;
			c = *(const int16_t *)(pk + 0x22);
			if (c < 0 || c > 0x6C0) clip |= 0x40;
			GteReadSXY2(pk + 0x2C);
			GteAVSZ4();
			c = *(const int16_t *)(pk + 0x2C);
			if (c < 0 || c > 0xA00) clip |= 8;
			c = *(const int16_t *)(pk + 0x2E);
			if (c < 0 || c > 0x6C0) clip |= 0x80;
			if ((clip & 0xF) == 0xF) continue;
			if ((clip & 0xF0) == 0xF0) continue;
			GteReadOTZ32(&h->otz);
			*(uint32_t *)(pk + 4) = h->col[0];
			*(uint32_t *)(pk + 0x10) = h->col[1];
			*(uint32_t *)(pk + 0x1C) = h->col[2];
			*(uint32_t *)(pk + 0x28) = h->col[3];
			const uint32_t bucket = ot + 4 * (uint32_t)(h->otz >> shift);
			uint8_t *poly = pk;
			uint8_t *wa = pk + 0x34;
			uint8_t *wb = pk + 0x40;
			pk += 0x4C;
			*(uint32_t *)wa = 0x02000000;
			*(uint32_t *)(wa + 4) = TexWindow(h->win_a);
			*(uint32_t *)(wa + 8) = 0;
			InsertPrimAutoDepth(bucket, wa);
			InsertPrimAutoDepth(bucket, poly);
			*(uint32_t *)wb = 0x02000000;
			*(uint32_t *)(wb + 4) = TexWindow(h->win_b);
			*(uint32_t *)(wb + 8) = 0;
			InsertPrimAutoDepth(bucket, wb);
		}
		h->list = (uint32_t)rec;
		return (uint32_t)pk;
	}

	static void ThreePart(const int16_t *c, int32_t a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6)
	{
		Mat4x3 m;
		if (a6 > 0)
		{
			uint8_t *h = (uint8_t *)FieldAlloc(0x6C);
			ScaledRotY(a2, 0x2000, &m);
			m.t[1] = 0;
			m.t[0] = c[0];
			m.t[2] = c[2];
			ComposeAffineTransform(&Camera(), &m, &m);
			GteSetRotMatrix(&m);
			GteSetTransVector(&m);
			*(uint16_t *)(h + 0x26) = 0;
			*(uint16_t *)(h + 0x24) = 0;
			*(uint16_t *)(h + 0x2A) = 0x100;
			*(uint16_t *)(h + 0x28) = 0x100;
			*(uint16_t *)(h + 0x2C) = 0;
			*(uint16_t *)(h + 0x2E) = 0x80;
			*(uint16_t *)(h + 0x30) = 0x100;
			*(uint16_t *)(h + 0x32) = 0x80;
			*(uint16_t *)(h + 0x20) = 0;
			*(uint16_t *)(h + 0x22) = (uint16_t)(a5 & 0x7F);
			*(uint32_t *)h = game_addr(MODEL_Part);
			*(int32_t *)(h + 0xC) = a6;
			*(uint32_t *)(h + 8) = 0;
			*(uint32_t *)(h + 0x18) = 0;
			*(uint32_t *)(h + 0x1C) = 0xF0;
			PacketCursor() = RenderPrimSet(h, OTBase() + 0x44, 2, PacketCursor());
			FieldFree(0x6C);
			return;
		}
		if (a4 < 0x1000)
		{
			uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
			Mat4x3 *rm = (Mat4x3 *)(h + 0x6C);
			*(uint32_t *)(h + 0x1C) = 0xF0;
			RotationY(a2, rm);
			*(uint32_t *)(h + 8) = 0;
			int16_t *pv = (int16_t *)(h + 0xAC);
			pv[1] = 0;
			pv[0] = 0;
			pv[2] = (int16_t)((0x800 - a4) * 2);
			Apply3DTransform(rm, pv, rm->t);
			rm->t[0] = (int32_t)((uint32_t)rm->t[0] + (uint32_t)c[0]);
			rm->t[2] = (int32_t)((uint32_t)rm->t[2] + (uint32_t)c[2]);
			if (a3 != 0)
			{
				RotationZ(a3, (Mat4x3 *)(h + 0x8C));
				MatrixMultiply3x3(rm, h + 0x8C);
			}
			ComposeAffineTransform(&Camera(), rm, rm);
			GteSetRotMatrix(rm);
			GteSetTransVector(rm);
			*(uint32_t *)(h + 0x18) = 0;
			*(uint16_t *)(h + 0x26) = 0;
			*(uint16_t *)(h + 0x24) = 0;
			*(uint16_t *)(h + 0x2A) = 0x100;
			*(uint16_t *)(h + 0x28) = 0x100;
			*(uint16_t *)(h + 0x2C) = 0;
			*(uint16_t *)(h + 0x2E) = 0;
			*(uint16_t *)(h + 0x32) = 0x100;
			*(uint16_t *)(h + 0x30) = 0x40;
			*(uint16_t *)(h + 0x20) = (uint16_t)(a5 & 0x3F);
			*(uint16_t *)(h + 0x22) = 0;
			*(uint32_t *)h = game_addr(MODEL_Ring);
			*(int32_t *)(h + 0xC) = a4;
			PacketCursor() = RenderPrimSet(h, OTBase() + 0x44, 2, PacketCursor());
			FieldFree(0xB4);
		}
		QuadHeader *q = (QuadHeader *)FieldAlloc(0x98);
		ScaledRotY(a2, 0x2000, &m);
		m.t[1] = 0;
		m.t[0] = c[0];
		m.t[2] = c[2];
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		q->depth = a4 - 0x800;
		q->win_a[1] = 0;
		q->win_a[0] = 0;
		q->win_a[3] = 0x100;
		q->win_a[2] = 0x100;
		q->win_b[0] = 0;
		q->win_b[1] = 0x80;
		q->win_b[2] = 0x100;
		q->win_b[3] = 0x80;
		q->vscroll = (uint16_t)(a5 & 0x7F);
		q->data = game_addr(MODEL_Part);
		q->verts = game_addr(MODEL_PartVerts);
		q->list = game_addr(MODEL_Part) + *(const uint32_t *)game_addr(MODEL_Part) + 0x1C;
		PacketCursor() = RenderQuads(q, OTBase() + 0x44, 2, PacketCursor());
		FieldFree(0x98);
	}

	static void SpawnBurst(const MainNode *mn)
	{
		int32_t count = 8;
		SparkleNode *s = (SparkleNode *)AddTaskToQueue(QSparkles(), game_addr(ORIG_SparkleTask));
		while (s)
		{
			s->frame = 0;
			const int32_t r = (CrtRand() & 0x7FF) + 0x800;
			RingPoint(CrtRand(), r, s->pos);
			s->pos[0] = (int16_t)(s->pos[0] + mn->centre[0]);
			s->pos[2] = (int16_t)(s->pos[2] + mn->centre[2]);
			s->pos[1] = mn->centre[1];
			s->vy = (int16_t)(-0x80 - (CrtRand() & 0x7F));
			if (--count == 0) break;
			s = (SparkleNode *)AddTaskToQueue(QSparkles(), game_addr(ORIG_SparkleTask));
		}
		count = 8;
		RockNode *k = (RockNode *)AddTaskToQueue(QRocks(), game_addr(ORIG_RockTask));
		while (k)
		{
			k->life = 0;
			const int32_t r = (CrtRand() & 0x7FF) + 0x800;
			RingPoint(CrtRand(), r, k->pos);
			k->pos[0] = (int16_t)(k->pos[0] + mn->centre[0]);
			k->pos[2] = (int16_t)(k->pos[2] + mn->centre[2]);
			k->pos[1] = mn->centre[1];
			k->vy = (int16_t)(-0x80 - (CrtRand() & 0x7F));
			k->scale = (int16_t)((shl32(CrtRand(), 11) >> 15) + 0x800);
			k->ax = (int8_t)CrtRand();
			k->ay = (int8_t)CrtRand();
			k->dax = (int8_t)CrtRand();
			k->day = (int8_t)CrtRand();
			if (--count == 0) break;
			k = (RockNode *)AddTaskToQueue(QRocks(), game_addr(ORIG_RockTask));
		}
	}

	static int32_t FlashOf(int32_t c)
	{
		if (c < 0xC) return c * 2300 / 12;
		if (c >= 0xE7) return (0xF3 - c) * 2300 / 12;
		return 0x8FC;
	}

	static int16_t LiftHeight(int32_t u, int16_t y0)
	{
		if (u < 0x800) return (int16_t)(y0 - (int16_t)(ComputeSin(u >> 1) << 1));
		int32_t v = 0x1000 - ComputeSin(u >> 1);
		v = mul32(v, v) >> 12;
		v = mul32(v, v) >> 12;
		return (int16_t)((int16_t)(v + v - 0x1000) + y0);
	}

	static uint32_t __cdecl MainTask(TaskNode *n)
	{
		MainNode *mn = (MainNode *)n;
		FlashLevel() = FlashOf(mn->counter);
		uint32_t i = (uint32_t)(mn->counter - 0x37);
		if (i < 7)
		{
			if (i == 0)
			{
				GhostInit(mn->ghosts, 1, 1);
				InitTaskQueuePool(QTrail(), (void *)(TexBase() + POOL_Trail), 0x34C, 1);
				if (Pause() == 0)
				{
					TrailNode *t = (TrailNode *)AddTaskToQueue(QTrail(), game_addr(ORIG_TrailTask));
					t->counter = 0;
					for (int32_t k = 0; k < 7; k++) t->ring[k].alive = 0;
				}
			}
			GhostDraw(Caster() + 0x9C, mn->ghosts);
			if (i < 5 && Pause() == 0) GhostRecord(Caster() + 0x9C, mn->ghosts);
		}
		PlayArg arg = {};
		i = (uint32_t)(mn->counter - 0x40);
		if (i < 0xF)
		{
			if (i == 0) DecodeModelPrimLayout(game_addr(LAYOUT_Rise), mn->layout, 0xE0);
			int16_t p[4];
			GetDefaultEffectPosition(Caster(), p);
			arg.depth = 0;
			arg.kind = game_addr(KIND_Plain);
			Play(mn, 0, mn->layout, &arg, p[0], p[2]);
		}
		i = (uint32_t)(mn->counter - 0x41);
		if (i < 8)
		{
			uint8_t *h = (uint8_t *)FieldAlloc(0x80);
			int16_t *sp = (int16_t *)(h + 0x78);
			GetEffectSpawnPosition(Caster(), 0x25, 0, sp);
			Mat4x3 *m = (Mat4x3 *)(h + 0x58);
			RotationY((int32_t)(i << 8), m);
			m->m[1][1] = (int16_t)-sp[1];
			m->t[0] = sp[0];
			m->t[1] = 0;
			m->t[2] = sp[2];
			ComposeAffineTransform(&Camera(), m, m);
			GteSetRotMatrix(m);
			GteSetTransVector(m);
			*(uint32_t *)h = game_addr(MODEL_Bone);
			*(uint32_t *)(h + 0x1C) = 0xF0;
			*(uint32_t *)(h + 0xC) = (i << 12) >> 3;
			*(uint32_t *)(h + 8) = 0;
			PacketCursor() = RenderPrimModel(h, OTBase() + 0x44, 2, PacketCursor());
			FieldFree(0x80);
		}
		i = (uint32_t)(mn->counter - 0x48);
		if (i < 0x1C)
		{
			if (i == 0) GhostInit(mn->ghosts, 1, 1);
			GhostDraw(Caster() + 0x9C, mn->ghosts);
			if (i < 0x1A && Pause() == 0) GhostRecord(Caster() + 0x9C, mn->ghosts);
		}
		i = (uint32_t)(mn->counter - 0x55);
		if (i < 0x1E)
		{
			int32_t fade = 0;
			if (i == 0)
			{
				const uint32_t model = game_addr(LAYOUT_Burn) + *(const uint32_t *)(game_addr(LAYOUT_Burn) + 0x18);
				int16_t box[6];
				ModelBox(model, box);
				ModelBox(model, box);
				CaptureStage(mn->centre, box);
			}
			else if (i >= 0x16) fade = shl32((int32_t)i - 0x16, 12) / 8;
			ThreePart(mn->centre, 0x800, 0, (int32_t)((i << 12) >> 3), (int32_t)(i << 5), fade);
			if (i >= 0xB) ThreePart(mn->centre, -0x400, 0x200, (int32_t)(((i - 0xB) << 12) >> 3), (int32_t)(i << 5), fade);
		}
		int16_t off[4];
		i = (uint32_t)(mn->counter - 0x73);
		if (i < 0x50)
		{
			const int32_t u = (int32_t)((i << 12) / 80) + 0x19;
			if (i == 0)
			{
				for (int32_t k = 0; k < mn->ntargets; k++)
				{
					MeasureHeight(mn->recs[k].e, mn->recs[k].height);
					*mn->recs[k].e |= 0xC;
				}
				DecodeModelPrimLayout(game_addr(LAYOUT_Burn), mn->layout, 0x244);
				InitTaskQueuePool(QSparkles(), (void *)(TexBase() + POOL_Sparkles), 0x18, 0x20);
				InitTaskQueuePool(QRocks(), (void *)(TexBase() + POOL_Rocks), 0x1C, 0x20);
				InitTaskQueuePool(QLayouts(), (void *)TexBase(), 0x714, 3);
				SpawnBurst(mn);
			}
			else if (i < 4) SpawnBurst(mn);
			off[2] = 0;
			off[0] = 0;
			for (int32_t k = 0; k < mn->ntargets; k++)
			{
				TargetRec *r = &mn->recs[k];
				uint8_t *e = r->e;
				if (Pause() == 0) *(int16_t *)(e + 0x1E) = LiftHeight(u, (int16_t)(r->pos01 >> 16));
				*(int32_t *)(e + 0x58) = *(const int16_t *)(e + 0x1E);
				r->ang[0] = (int16_t)(r->ang[0] + ((k & 1) ? 0x100 : -0x100));
				off[1] = (int16_t)((r->height[1] + r->height[0]) >> 1);
				DrawEntity(e, r->ang, off);
			}
			const uint32_t d = i - 0x1A;
			arg.depth = -0x80;
			arg.kind = game_addr(KIND_Burn);
			arg.scroll = (uint16_t)(i << 4);
			arg.w28 = d < 0x64 ? (uint16_t)((d * d) >> 4) : 0;
			Play(mn, 1, mn->layout, &arg, mn->centre[0], mn->centre[2]);
		}
		i = (uint32_t)(mn->counter - 0xC3);
		if (i < 0x28)
		{
			const int32_t w = (int32_t)((i << 12) / 40) + 0x33;
			if (i == 0)
			{
				InitTaskQueuePool(QRocks(), (void *)(TexBase() + POOL_Rocks), 0x1C, 0x20);
				StageGroupsBit1Off();
				DecodeModelPrimLayout(game_addr(LAYOUT_Drop), mn->layout, 0x258);
				*Caster() |= 4;
				*(Caster() + 0x9C) |= 4;
			}
			off[2] = 0;
			off[0] = 0;
			for (int32_t k = 0; k < mn->ntargets; k++)
			{
				TargetRec *r = &mn->recs[k];
				uint8_t *e = r->e;
				const int16_t y = (int16_t)((int16_t)(r->pos01 >> 16) + (int16_t)(w * 4 - 0x2000));
				*(int16_t *)(e + 0x1E) = y;
				*(int32_t *)(e + 0x58) = y;
				r->ang[0] = (int16_t)(r->ang[0] + ((k & 1) ? 0x100 : -0x100));
				off[1] = (int16_t)((r->height[0] + r->height[1]) >> 1);
				DrawEntity(e, r->ang, off);
			}
			if (Pause() == 0)
			{
				int32_t count = 2;
				RockNode *k = (RockNode *)AddTaskToQueue(QRocks(), game_addr(ORIG_RockTask));
				while (k)
				{
					k->life = 0xA;
					const int32_t r = (CrtRand() & 0x7FF) + 0x800;
					RingPoint(CrtRand(), r, k->pos);
					k->pos[0] = (int16_t)(k->pos[0] + mn->centre[0]);
					k->pos[2] = (int16_t)(k->pos[2] + mn->centre[2]);
					k->pos[1] = (int16_t)(mn->centre[1] - 0x800);
					k->vy = (int16_t)((CrtRand() & 0x7F) + 0x100);
					k->scale = (int16_t)((shl32(CrtRand(), 11) >> 15) + 0x800);
					k->ax = (int8_t)CrtRand();
					k->ay = (int8_t)CrtRand();
					k->dax = (int8_t)CrtRand();
					k->day = (int8_t)CrtRand();
					if (--count == 0) break;
					k = (RockNode *)AddTaskToQueue(QRocks(), game_addr(ORIG_RockTask));
				}
			}
			arg.depth = 0x100;
			arg.kind = game_addr(KIND_Plain);
			Play(mn, 2, mn->layout, &arg, mn->centre[0], mn->centre[2]);
		}
		else if (i == 0x28)
		{
			for (int32_t k = 0; k < mn->ntargets; k++)
			{
				TargetRec *r = &mn->recs[k];
				*(uint16_t *)r->e &= 0xFFF3;
				*(uint32_t *)(r->e + 0x1C) = r->pos01;
				*(uint32_t *)(r->e + 0x20) = r->pos23;
				*(int16_t *)(r->e + 0x1E) = (int16_t)(*(int16_t *)(r->e + 0x1E) - 0x1000);
			}
			*(uint16_t *)Caster() &= 0xFFFB;
			*(uint16_t *)(Caster() + 0x9C) &= 0xFFFB;
			for (uint8_t *gp = (uint8_t *)game_addr(game::unk_1D98991); gp < (uint8_t *)game_addr(game::unk_1D98A41); gp += 0x2C) *gp |= 2;
			QRocks()->tail = nullptr;
			QRocks()->head = nullptr;
		}
		else if (i <= 0x30 && Pause() == 0)
		{
			for (int32_t k = 0; k < mn->ntargets; k++)
			{
				uint8_t *e = mn->recs[k].e;
				*(int16_t *)(e + 0x1E) = (int16_t)(*(int16_t *)(e + 0x1E) + 0x200);
				if (i == 0x30)
				{
					LayoutNode *t = (LayoutNode *)AddTaskToQueue(QLayouts(), game_addr(ORIG_LayoutTask));
					GetDefaultEffectPosition(e, t->pos);
					t->pos[1] = 0;
					DecodeModelPrimLayout(game_addr(LAYOUT_End), t->layout, 0x1C0);
				}
			}
			if (i == 0x2E) BdPlaySE(SOUND_Fall(), 0, 0x80);
			else if (i == 0x30) ApplyActionResultToTargets(Ctx()->actions[0].targets, Ctx()->actions[0].target_count);
		}
		i = (uint32_t)(mn->counter - 0xE3);
		if (i < 0x10)
		{
			PacketCursor() = FadeTileInHoldOut((int32_t)i, 0xF0, 0xF0, 0xF0, 6, 4, 6, PacketCursor());
		}
		if (mn->counter == 0x21)
		{
			QFlames()->tail = nullptr;
			QFlames()->head = nullptr;
		}
		else if (mn->counter == 0x23) StreamQueueLoad(0x2C0, TexBase() + ARENA_A, 0);
		else if (mn->counter == 0x36 && StreamBusy() != 0) return 0;
		if (mn->counter == 0xEB)
		{
			StreamQueueLoad(0x168, TexBase() + STREAM_B, 1);
			StreamQueueLoad(0x169, TexBase() + STREAM_B, 1);
			StreamQueueLoad(0x16A, TexBase() + STREAM_B, 0);
		}
		StreamPump();
		if (mn->counter == 0) BdPlaySE(SOUND_Start(), 0, 0x80);
		else if (mn->counter == 0x37) BdPlaySE(SOUND_Ghost(), 0, 0x80);
		else if (mn->counter == 0x73) BdPlaySE(SOUND_Lift(), 0, 0x80);
		if (mn->counter >= 0xF3)
		{
			if (StreamPreloadPending(2) != 0) return TASK_END;
			mn->counter = 0x133;
			return 0;
		}
		mn->counter++;
		return 0;
	}

	static void BoneFrame(uint8_t *anim, int32_t bone, int32_t *dir, int16_t *point)
	{
		const uint8_t *skel = **(uint8_t ***)(anim + 4);
		if (bone >= 0xF0) bone = skel[bone - 0xEC];
		const uint8_t *b = skel + 0x10 + bone * 0x30;
		dir[1] = 0;
		dir[0] = 0;
		dir[2] = -0x1000;
		TransformVectorBy3x3Matrix((const Mat4x3 *)(b + 0x10), dir, dir);
		NormalizeVector(dir, dir);
		point[0] = *(const int16_t *)(b + 0x24);
		point[1] = *(const int16_t *)(b + 0x28);
		point[2] = *(const int16_t *)(b + 0x2C);
	}
	static void WeaponBone(uint8_t *e, int32_t bone, int32_t *dir, int16_t *point)
	{
		BoneFrame(e + 0x60, bone, dir, point);
		TransformVectorBy3x3Matrix((const Mat4x3 *)(e + 0x40), dir, dir);
		NormalizeVector(dir, dir);
		MatVec((const Mat4x3 *)(e + 0x40), point, point);
		point[0] = (int16_t)(point[0] + *(const int16_t *)(e + 0x54));
		point[1] = (int16_t)(point[1] + *(const int16_t *)(e + 0x58));
		point[2] = (int16_t)(point[2] + *(const int16_t *)(e + 0x5C));
	}
	static void TrailDraw(const int16_t (*sa)[4], const int16_t (*sb)[4], TrailWork *w)
	{
		GteSetRotMatrix(&Camera());
		GteSetTransVector(&Camera());
		GteSetFarColor2(0, 0, 0);
		uint32_t *pk = (uint32_t *)PacketCursor();
		uint32_t *pk2 = pk + 9;
		w->col1 = 0x3A185820;
		w->base_col = 0x3A185820;
		for (int32_t k = 0, tacc = 0; tacc < 0x1E000; k++, tacc += 0x1000)
		{
			w->col0 = w->col1;
			pk[0] = 0x8000000;
			GteLoadV012(sa[k], sa[k + 1], sb[k]);
			GteRTPT();
			GteReadFLAG(&w->flag);
			if (w->flag & 0x60000) continue;
			GteReadSXY012Split(pk + 2, pk + 4, pk + 6);
			GteLoadV0(sb[k + 1]);
			GteRTPS();
			GteReadSXY2(pk + 8);
			GteAVSZ4();
			GteReadOTZ32(&w->otz);
			GteSetIR0(tacc / 30);
			GteLoadRGBC(&w->base_col);
			GteDPCS();
			GteStoreRGB2(&w->col1);
			pk[5] = pk[1] = w->col0;
			pk[7] = pk[3] = w->col1;
			InsertPrimAutoDepth(OTBase() + 4 * (uint32_t)(w->otz >> 2) + 0x44, pk);
			pk2[0] = 0x8000000;
			pk2[3] = 0x3A000000;
			pk2[1] = 0x3A000000;
			pk2[5] = w->col0;
			pk2[7] = w->col1;
			pk2[2] = pk[2];
			pk2[4] = pk[4];
			pk2[6] = pk[6];
			pk2[8] = pk[8];
			InsertPrimAutoDepth(OTBase() + 4 * (uint32_t)(w->otz >> 2) + 0x44, pk2);
			pk += 0x12;
			pk2 += 0x12;
		}
		PacketCursor() = (uint32_t)pk2;
	}

	static uint32_t __cdecl TrailTask(TaskNode *n)
	{
		TrailNode *t = (TrailNode *)n;
		TrailWork *w = (TrailWork *)FieldAlloc(0x40);
		WeaponBone(TRAIL_ENTITY(), 1, w->dir, w->base);
		w->tip[0] = (int16_t)((int16_t)(mul32(w->dir[0], 777) >> 12) + w->base[0]);
		w->tip[1] = (int16_t)((int16_t)(mul32(w->dir[1], 777) >> 12) + w->base[1]);
		w->tip[2] = (int16_t)((int16_t)(mul32(w->dir[2], 777) >> 12) + w->base[2]);
		int32_t idx = t->counter % 7;
		RingEntry *e = &t->ring[idx];
		e->alive = 1;
		ff8fx::copy<(8)>(e->base, w->base);
		ff8fx::copy<(8)>(e->tip, w->tip);
		const int32_t want = t->counter >= 0x35 ? 0x3C - t->counter : 7;
		int32_t cnt = 0;
		while (cnt < want)
		{
			const RingEntry *r = &t->ring[idx];
			if (!r->alive) break;
			idx--;
			ff8fx::copy<(8)>(t->pts_a[cnt], r->base);
			ff8fx::copy<(8)>(t->pts_b[cnt], r->tip);
			if (idx < 0) idx = 6;
			cnt++;
		}
		if (cnt > 1)
		{
			uint8_t *wk = (uint8_t *)FieldAlloc(0x190);
			SplineSetup(cnt, t->pts_a, wk);
			for (int32_t k = 0, u = 0; k < 32; k++, u += 0x1000) SplineEval(cnt, wk, t->spl_a[k], u / 31);
			SplineSetup(cnt, t->pts_b, wk);
			for (int32_t k = 0, u = 0; k < 32; k++, u += 0x1000) SplineEval(cnt, wk, t->spl_b[k], u / 31);
			FieldFree(0x190);
			ff8fx::copy<(8)>(t->first_b, t->spl_b[0]);
			t->dir_b[0] = (int32_t)t->spl_b[0][0] - t->spl_b[1][0];
			t->dir_b[1] = (int32_t)t->spl_b[0][1] - t->spl_b[1][1];
			t->dir_b[2] = (int32_t)t->spl_b[0][2] - t->spl_b[1][2];
			NormalizeVector(t->dir_b, t->dir_b);
			ff8fx::copy<(8)>(t->first_a, t->spl_a[0]);
			t->dir_ab[0] = (int32_t)t->first_b[0] - t->first_a[0];
			t->dir_ab[1] = (int32_t)t->first_b[1] - t->first_a[1];
			t->dir_ab[2] = (int32_t)t->first_b[2] - t->first_a[2];
			NormalizeVector(t->dir_ab, t->dir_ab);
			TrailDraw(t->spl_a, t->spl_b, w);
		}
		FieldFree(0x40);
		if (Pause() != 0) return 0;
		t->counter++;
		return t->counter >= 0x3B ? TASK_END : 0;
	}

	static uint32_t __cdecl LayoutTask(TaskNode *n)
	{
		LayoutNode *t = (LayoutNode *)n;
		PlayArg arg = {};
		arg.m.m[2][2] = 0x1000;
		arg.m.m[1][1] = 0x1000;
		arg.m.m[0][0] = 0x1000;
		arg.m.t[1] = t->pos[1];
		arg.m.t[2] = t->pos[2];
		arg.m.t[0] = t->pos[0];
		ComposeAffineTransform(&Camera(), &arg.m, &arg.m);
		arg.depth = -0x80;
		arg.kind = game_addr(KIND_Plain);
		arg.morph = TexBase() + MORPH;
		const int left = prim::play((prim::Layout *)t->layout, PrimCallback, (int)&arg, (int)Pause());
		return left ? 0 : TASK_END;
	}

	static void RockDrawAngles(const RockNode *k, int16_t ax, int16_t ay)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x98);
		ScaledRotY(ay, k->scale, (Mat4x3 *)(h + 0x78));
		RotationX(ax, (Mat4x3 *)(h + 0x58));
		MatrixMultiply3x3((Mat4x3 *)(h + 0x58), h + 0x78);
		Mat4x3 *m = (Mat4x3 *)(h + 0x58);
		m->t[0] = k->pos[0];
		m->t[1] = k->pos[1];
		m->t[2] = k->pos[2];
		ComposeAffineTransform(&Camera(), m, m);
		GteSetRotMatrix(m);
		GteSetTransVector(m);
		*(uint32_t *)h = game_addr(MODEL_Rock);
		*(uint32_t *)(h + 0x1C) = 0;
		PacketCursor() = RenderPrimModel(h, OTBase() + 0x44, 2, PacketCursor());
		FieldFree(0x98);
	}
	static void RockDraw(const RockNode *k) { RockDrawAngles(k, (int16_t)((int16_t)k->ax << 4), (int16_t)((int16_t)k->ay << 4)); }
	static uint32_t __cdecl RockTask(TaskNode *n)
	{
		RockNode *k = (RockNode *)n;
		RockDraw(k);
		if (Pause() != 0) return 0;
		k->life--;
		if (k->life == 0) return TASK_END;
		k->ax = (int8_t)(k->ax + k->dax);
		k->ay = (int8_t)(k->ay + k->day);
		k->pos[1] = (int16_t)(k->pos[1] + k->vy);
		k->vy = (int16_t)(k->vy + 0x14);
		return 0;
	}

	static void SparkleDraw(const int16_t *pos, int16_t frame)
	{
		TransformCameraByShadowRotation(pos, 0x1000, -0x400);
		uint8_t *h = AllocHeader(0xB4);
		*(uint32_t *)h = game_addr(SEQ_Sparkle);
		*(uint16_t *)(h + 4) = (uint16_t)frame;
		*(uint16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
		FieldFree(0xB4);
	}
	static uint32_t __cdecl SparkleTask(TaskNode *n)
	{
		SparkleNode *s = (SparkleNode *)n;
		SparkleDraw(s->pos, s->frame);
		if (Pause() != 0) return 0;
		s->pos[1] = (int16_t)(s->pos[1] + s->vy);
		s->frame++;
		return s->frame < 0x10 ? 0 : TASK_END;
	}
}

	void register_mag210_bloodfest()
	{
		register_port(blood210::ORIG_MasterTask, (void *)blood210::MasterTask, "210 MasterTask", 210);
		register_port(blood210::ORIG_FlameSpawner, (void *)blood210::FlameSpawner, "210 FlameSpawner", 210);
		register_port(blood210::ORIG_FlameTask, (void *)blood210::FlameTask, "210 FlameTask", 210);
		register_port(blood210::ORIG_SparkTask, (void *)blood210::SparkTask, "210 SparkTask", 210);
		register_port(blood210::ORIG_EmberTask, (void *)blood210::EmberTask, "210 EmberTask", 210);
		register_port(blood210::ORIG_MainTask, (void *)blood210::MainTask, "210 MainTask", 210);
		register_port(blood210::ORIG_TrailTask, (void *)blood210::TrailTask, "210 TrailTask", 210);
		register_port(blood210::ORIG_LayoutTask, (void *)blood210::LayoutTask, "210 LayoutTask", 210);
		register_port(blood210::ORIG_RockTask, (void *)blood210::RockTask, "210 RockTask", 210);
		register_port(blood210::ORIG_SparkleTask, (void *)blood210::SparkleTask, "210 SparkleTask", 210);
	}
}

