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

// Effect 251: Demon Slice

#include "mag_common.h"

namespace ff8fx
{
namespace demon251
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

	inline uint32_t &Pause() { return var<uint32_t>(game_addr(game::unk_2556388)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::unk_255638C)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::unk_2556390)); }
	inline uint8_t *&Caster() { return var<uint8_t *>(game_addr(game::unk_2556394)); }
	inline uint8_t *&Target() { return var<uint8_t *>(game_addr(game::unk_2556398)); }
	inline TaskQueue *QLayouts() { return (TaskQueue *)game_addr(game::unk_25563A0); }
	inline TaskQueue *QTrail() { return (TaskQueue *)game_addr(game::unk_25563B0); }
	inline TaskQueue *QSparkles() { return (TaskQueue *)game_addr(game::unk_25563D0); }
	inline TaskQueue *QMain() { return (TaskQueue *)game_addr(game::unk_25563E0); }
	inline TaskQueue *QFlames() { return (TaskQueue *)game_addr(game::unk_2556440); }
	inline int32_t &FlashLevel() { return var<int32_t>(game_addr(game::unk_255643C)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::unk_2556450)); }
	inline Mat4x3 *OrthoView() { return (Mat4x3 *)game_addr(game::unk_2556458); }
	inline uint32_t OTBase() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)); }

	static constexpr game::Id ORIG_MasterTask = game::MAG_251_sub_6F4580;
	static constexpr game::Id ORIG_FlameSpawner = game::MAG_251_sub_6F4890;
	static constexpr game::Id ORIG_FlameTask = game::MAG_251_sub_6F4A30;
	static constexpr game::Id ORIG_SparkTask = game::sub_6F4DC0;
	static constexpr game::Id ORIG_EmberTask = game::sub_6F4EC0;
	static constexpr game::Id ORIG_MainTask = game::MAG_251_sub_6F5000;
	static constexpr game::Id ORIG_TrailTask = game::MAG_251_sub_6F5B20;
	static constexpr game::Id ORIG_LayoutTask = game::MAG_251_sub_6F6530;
	static constexpr game::Id ORIG_SparkleTask = game::MAG_251_sub_6F65F0;
	static constexpr game::Id ORIG_PrimCallback = game::MAG_251_sub_6F60D0;
	static constexpr game::Id ORIG_DrawEntity212 = game::MAG_212_BURNINGRAVE_DrawEntity;

	static const uint32_t STREAM_STATE = 0xA24C;
	static const uint32_t POOL_Main = 0xA294;
	static const uint32_t POOL_Sparkles = 0x1580;
	static const uint32_t POOL_Trail = 0x1700;
	static const uint32_t MORPH = 0xA880;
	static const uint32_t ARENA_A = 0xB300;
	static const uint32_t ARENA_B = 0x1B300;
	static constexpr game::Id LAYOUT_Fade = game::unk_142FB40;
	static constexpr game::Id LAYOUT_Wave = game::unk_1434430;
	static constexpr game::Id LAYOUT_Rise = game::unk_1431F88;
	static constexpr game::Id LAYOUT_Swing = game::unk_1433318;
	static constexpr game::Id LAYOUT_Push = game::unk_1432448;
	static constexpr game::Id LAYOUT_Fall = game::unk_142D8A4;
	static constexpr game::Id LAYOUT_Hit = game::unk_1431418;
	static constexpr game::Id KIND_Plain = game::a00000000000000_3;
	static constexpr game::Id KIND_Wave = game::aAb10;
	static constexpr game::Id KIND_Swing = game::unk_145D2B0;
	static constexpr game::Id SEQ_Flame = game::unk_143878C;
	static constexpr game::Id SEQ_FlameGlow = game::unk_1438938;
	static constexpr game::Id SEQ_Ember = game::unk_1438AE4;
	static constexpr game::Id SEQ_Sparkle = game::unk_142D744;
	static const int16_t *WAVE_TABLE() { return (const int16_t *)game_addr(game::unk_1438C90); }
	static const void *SOUND_Start() { return (const void *)game_addr(game::unk_142D738); }
	static const void *SOUND_Swing() { return (const void *)game_addr(game::unk_142D740); }
	static const void *SOUND_Fall() { return (const void *)game_addr(game::unk_142D73C); }
	static uint8_t *TRAIL_ENTITY() { return (uint8_t *)game_addr(game::unk_1D97530); }

	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t flag, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, flag, out); }
	inline int32_t ComputeCos(int32_t angle) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(angle); }
	inline int32_t ISqrt(int32_t v) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::Math_SqrtInt))(v); }
	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline void RotationY(int32_t a, Mat4x3 *out) { fn<void (__cdecl *)(int32_t, Mat4x3 *)>(game_addr(game::Effect_RotationYMatrix))(a, out); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void RotationFromAnglesB(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::MAG_063_RotationFromAnglesYXZ))(angles, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void MatrixMultiply3x3(Mat4x3 *a, const void *b) { fn<void (__cdecl *)(Mat4x3 *, const void *)>(game_addr(game::MatrixMultiply3x3))(a, b); }
	inline void MatVec(const Mat4x3 *m, const int16_t *in, int16_t *out) { fn<void (__cdecl *)(const Mat4x3 *, const int16_t *, int16_t *)>(game_addr(game::matrixMultiplyVector))(m, in, out); }
	inline void VecLerp(const void *a, const void *b, int32_t wa, int32_t wb, void *out) { fn<void (__cdecl *)(const void *, const void *, int32_t, int32_t, void *)>(game_addr(game::VecLerp))(a, b, wa, wb, out); }
	inline void GteMVMVA_RotV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0))(); }
	inline uint32_t RenderPrimSet(void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::MAG_063_RenderPrimSet))(h, ot, mode, cursor); }
	inline uint32_t QueueChainTransformation(uint8_t *entity, uint32_t id) { return fn<uint32_t (__cdecl *)(uint8_t *, uint32_t)>(game_addr(game::QueueChainTransformation))(entity, id); }
	inline uint32_t Tile(int32_t r, int32_t g, int32_t b, int32_t level, uint32_t cursor) { return fn<uint32_t (__cdecl *)(int32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_FullScreenTile))(r, g, b, level, cursor); }
	inline void DrawEntity212(uint8_t *e, const int16_t *angles, const int16_t *off) { fn<void (__cdecl *)(uint8_t *, const int16_t *, const int16_t *)>(game_addr(ORIG_DrawEntity212))(e, angles, off); }
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

#pragma pack(push, 1)
	struct MasterNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t running;
		uint8_t parity;
	};
	struct SpawnerNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t w0E;
		uint8_t pad10[0x10];
		int16_t slot;
		int16_t w22;
	};
	struct FlameNode { TaskNode hdr; int16_t frame, delay; Flame f; };
	struct SparkNode { TaskNode hdr; int16_t life, delay; FireSpark s; };
	struct EmberNode { TaskNode hdr; int16_t frame, delay; Ember e; };
	struct MainNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		int16_t caster0[4];
		int16_t target0[4];
		int16_t target_fx[4];
		int16_t near_pt[4];
		int16_t far_pt[4];
		int16_t keep[4];
		int16_t step[4];
		uint32_t spawn[2];
		uint8_t layout_a[0x1C8];
		uint8_t layout_b[0x3D4];
	};
	struct RingEntry { uint32_t alive; int16_t base[4]; int16_t tip[4]; };
	struct TrailNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		RingEntry ring[7];
		int16_t pts_a[8][4];
		int16_t pts_b[8][4];
		int16_t spl_a[32][4];
		int16_t spl_b[32][4];
		int16_t first_b[4];
		int16_t first_a[4];
		int32_t dir_b[4];
		int32_t dir_ab[4];
	};
	struct TrailWork
	{
		uint8_t pad00[8];
		int32_t otz;
		uint32_t flag;
		uint32_t col0;
		uint32_t col1;
		uint8_t pad18[4];
		uint32_t base_col;
		int16_t base[4];
		int16_t tip[4];
		int32_t dir[4];
	};
	struct LayoutNode
	{
		TaskNode hdr;
		int16_t pos[4];
		uint8_t layout[0x144];
	};
	struct SparkleNode
	{
		TaskNode hdr;
		int16_t pos[3];
		int16_t frame;
		int8_t vel[3];
		uint8_t pad17;
	};
	struct PlayArg
	{
		Mat4x3 m;
		uint32_t kind;
		int16_t depth;
		uint16_t scroll;
		int16_t fade;
		int16_t wave_phase;
		int16_t wave_amp;
		int16_t wave_xz;
		int16_t wave_y;
		int16_t pad32;
		uint32_t morph;
	};
#pragma pack(pop)
	static_assert(sizeof(MasterNode) == 0x10 && sizeof(SpawnerNode) == 0x24 && sizeof(FlameNode) == 0x24 && sizeof(SparkNode) == 0x24 && sizeof(EmberNode) == 0x24, "Demon Slice nodes");
	static_assert(sizeof(MainNode) == 0x5EC && sizeof(TrailNode) == 0x34C && sizeof(LayoutNode) == 0x158 && sizeof(SparkleNode) == 0x18, "Demon Slice nodes");
	static_assert(sizeof(RingEntry) == 0x14 && sizeof(TrailWork) == 0x40 && sizeof(PlayArg) == 0x38, "Demon Slice blocks");

	static const FlameRolls FLAME_ROLLS = { 6, 2, 2800, 1000, 0x226, { -500, 300, -1 }, { 0xD00, 0x600, 1 } };
	static const FlameLook FLAME_LOOK = { SEQ_Flame, SEQ_FlameGlow, 4, 0x400, 0xC, -2000, 160, 0xA0 };
	static const EmberRolls EMBER_ROLLS = { 6, 1, { -150, 300, 1 }, { 0, 100, -1 }, { -150, 300, 1 }, { -20, 30, -1 }, { 0xA00, 0x500, 1 } };
	static const SparkRolls SPARK_ROLLS = { { 5, 4, 1 }, 4, 1, { -200, 400, 1 }, { 0, 200, -1 }, { -200, 400, 1 }, { -40, 80, 1 }, { -20, 50, -1 }, { -40, 80, 1 }, { 0x900, 0x600, 1 } };
	static const SparkLook SPARK_LOOK = { 5, 9, 4, 0x10, 3 };
}
}

namespace ff8fx
{
namespace demon251
{
	using namespace eng;
	using namespace magc;

	inline int32_t SlotOf(uint8_t *e) { return (int32_t)((e - (uint8_t *)game_addr(game::BattleEntitySlotData)) / 0x9C); }

	static void NormaliseHeights(uint8_t *model)
	{
		const int32_t n = *(const int32_t *)(model + 4);
		int16_t *v = (int16_t *)(model + 0xC);
		int32_t lo = v[1], hi = v[1];
		for (int32_t i = 0; i < n - 1; i++)
		{
			const int32_t y = v[i * 4 + 1];
			if (lo > y) lo = y;
			else if (hi < y) hi = y;
		}
		const int32_t span = hi - lo;
		for (int32_t i = 0; i < n; i++) v[i * 4 + 3] = (int16_t)(shl32(v[i * 4 + 1] - hi, 12) / span);
	}

	static void StartFlameSpawner(uint8_t *e)
	{
		SpawnerNode *s = (SpawnerNode *)AddTaskToQueue(QFlames(), game_addr(ORIG_FlameSpawner));
		if (!s) return;
		s->counter = 0;
		s->w0E = 0;
		s->w22 = 0;
		s->slot = (int16_t)SlotOf(e);
	}

	static int16_t g_task_fade = 0;

	static void MasterSetup()
	{
		StreamStateInit((void *)(TexBase() + STREAM_STATE));
		InitTaskQueuePool(QMain(), (void *)(TexBase() + POOL_Main), 0x5EC, 1);
		InitTaskQueuePool(QFlames(), (void *)TexBase(), 0x24, 0x96);
		MainNode *m = (MainNode *)AddTaskToQueue(QMain(), game_addr(ORIG_MainTask));
		Memset32(&m->counter, 0, 0x178);
		StartFlameSpawner(Target());
		ff8fx::copy<(8)>(m->caster0, Caster() + 0x1C);
		ff8fx::copy<(8)>(m->target0, Target() + 0x1C);
		*(int16_t *)(Target() + 0x1E) = 0;
		GetDefaultEffectPosition(Target(), m->target_fx);
		const int32_t dx = *(const int16_t *)(Caster() + 0x1C) - m->target_fx[0];
		const int32_t dz = *(const int16_t *)(Caster() + 0x20) - m->target_fx[2];
		const int32_t d = ISqrt(dz * dz + dx * dx);
		int32_t s = *(const int16_t *)(Caster() + 0x26) + *(const int16_t *)(Target() + 0x26);
		s += s / 2;
		const int16_t x0 = m->target_fx[0];
		m->near_pt[0] = (int16_t)(s * dx / d + x0);
		m->near_pt[1] = 0;
		m->near_pt[2] = (int16_t)(s * dz / d + m->target_fx[2]);
		s = *(const int16_t *)(Caster() + 0x26) + *(const int16_t *)(Target() + 0x26);
		m->far_pt[1] = 0;
		m->far_pt[0] = (int16_t)(s * dx / d + x0);
		m->far_pt[2] = (int16_t)(s * dz / d + m->target_fx[2]);
		m->step[0] = (int16_t)(shl32(-dx, 9) / d);
		m->step[2] = (int16_t)(shl32(-dz, 9) / d);
		uint8_t *wave = (uint8_t *)game_addr(LAYOUT_Wave);
		NormaliseHeights(wave + *(const int32_t *)(wave + 8));
		NormaliseHeights(wave + *(const int32_t *)(wave + 0xC));
	}

	static uint32_t __cdecl MasterTask(TaskNode *n)
	{
		MasterNode *ms = (MasterNode *)n;
		if (ms->parity)
		{
			PacketCursor() = TexBase() + ARENA_A;
			ms->parity = 0;
		}
		else
		{
			PacketCursor() = TexBase() + ARENA_B;
			ms->parity = 1;
		}
		FlashLevel() = 0;
		if (ms->counter == 1 && Pause() == 0 && !ms->running)
		{
			ms->running = 1;
			MasterSetup();
		}
		int alive = 1;
		if (ms->running)
		{
			g_task_fade = (int16_t)(uint32_t)n;
			EffectCameraMatrix(&Camera(), OrthoView());
			alive = ExecuteTaskQueue(QMain());
			ExecuteTaskQueue(QFlames());
			ExecuteTaskQueue(QSparkles());
			ExecuteTaskQueue(QTrail());
			ExecuteTaskQueue(QLayouts());
		}
		SetScreenFlash((uint32_t)FlashLevel(), 0);
		if (Pause() != 0) return 0;
		if (ms->running && alive == 0)
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

	static void Wave(uint8_t *h, uint32_t morph, int32_t phase, int32_t amp, int32_t xz, int32_t ys)
	{
		const uint8_t *model = *(uint8_t **)h;
		int32_t n = *(const int32_t *)(model + 4);
		const int16_t *v = (const int16_t *)(model + 0xC);
		int16_t *out = (int16_t *)morph;
		do
		{
			const int32_t w = v[3];
			const int32_t a = mul32(WAVE_TABLE()[w & 0xFFE], amp) >> 12;
			const int16_t *dir = WAVE_TABLE() + ((w + phase) & 0xFFF) * 2;
			const int32_t dz = mul32(dir[1], a);
			const int32_t dx = (int16_t)(mul32(dir[0], a) >> 12);
			out[0] = (int16_t)(mul32(dx + v[0], xz) >> 12);
			out[2] = (int16_t)(mul32(v[2] + (int16_t)(dz >> 12), xz) >> 12);
			out[1] = (int16_t)(mul32(v[1], ys) >> 12);
			v += 4;
			out += 4;
		} while (--n != 0);
		*(uint32_t *)(h + 4) = morph;
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
		if (r->flags & 0x40000) RotationFromAnglesB(r->rot, &m);
		else RotationFromAngles(r->rot, &m);
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
			if (!(r->flags & 0x8000)) MatrixMultiply(OrthoView(), &m);
		}
		else if (r->flags & 0x200)
		{
			m.t[0] = off[0];
			m.t[1] = off[1];
			m.t[2] = off[2];
		}
		else
		{
			GteSetRotMatrixCtrl(&arg->m);
			GteLoadV0(off);
			GteMVMVA_RotV0();
			GteReadMAC123(m.t);
			if (!(r->flags & 0x8000)) MatrixMultiply(&arg->m, &m);
		}
		m.t[0] = (int32_t)((uint32_t)m.t[0] + (uint32_t)arg->m.t[0]);
		m.t[1] = (int32_t)((uint32_t)m.t[1] + (uint32_t)arg->m.t[1]);
		m.t[2] = (int32_t)((uint32_t)m.t[2] + (uint32_t)arg->m.t[2]);
		if (!(*(const uint32_t *)r->scale == 0x10001000 && r->scale[2] == 0x1000))
		{
			if (r->flags & 0x100)
			{
				Mat4x3 s;
				s.m[0][0] = r->scale[0];
				s.m[1][1] = r->scale[1];
				s.m[2][2] = r->scale[2];
				s.m[1][2] = 0;
				s.m[0][2] = 0;
				s.m[2][1] = 0;
				s.m[0][1] = 0;
				s.m[2][0] = 0;
				s.m[1][0] = 0;
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
		*(uint32_t *)(h + 0x1C) = 0x2030;
		*(int32_t *)(h + 0xC) = r->a;
		if (r->a != 0)
		{
			*(uint32_t *)(h + 0x1C) = 0x20F0;
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
			*(uint16_t *)(h + 0x32) = 0x100;
			*(uint16_t *)(h + 0x30) = 0x40;
			*(uint16_t *)(h + 0x20) = (uint16_t)(arg->scroll & 0x3F);
		}
		else if (kind == 0x41 || kind == 0x42)
		{
			if (kind == 0x41)
			{
				*(uint16_t *)(h + 0x2C) = 0;
				*(uint16_t *)(h + 0x2E) = 0;
				*(uint16_t *)(h + 0x32) = 0x80;
				*(uint16_t *)(h + 0x30) = 0x100;
				*(uint16_t *)(h + 0x22) = (uint16_t)(arg->scroll & 0x7F);
			}
			Wave(h, arg->morph, arg->wave_phase, arg->wave_amp, arg->wave_xz, arg->wave_y);
		}
		if (arg->fade != 0)
		{
			*(int32_t *)(h + 0xC) = arg->fade;
			*(uint32_t *)(h + 8) = 0;
			*(uint8_t *)(h + 0x1C) |= 0xC0;
		}
		PacketCursor() = RenderPrimSet(h, OTBase() + 0x44, 2, PacketCursor());
		FieldFree(0x6C);
	}

	static void Play(MainNode *mn, int site, uint8_t *layout, PlayArg *arg, int32_t x, int32_t y, int32_t z)
	{
		RotationY(0, &arg->m);
		arg->m.t[0] = x;
		arg->m.t[1] = y;
		arg->m.t[2] = z;
		ComposeAffineTransform(&Camera(), &arg->m, &arg->m);
		g_task_fade = (int16_t)arg->m.t[1];
		arg->morph = TexBase() + MORPH;
		prim::play((prim::Layout *)layout, PrimCallback, (int)arg, (int)Pause());
	}

	static int32_t FadeLevel(int32_t t, int32_t in, int32_t hold, int32_t out)
	{
		const int32_t din = in != 0 ? in : 1;
		const int32_t dout = out != 0 ? out : 1;
		if (t < in) return shl32(t + 1, 12) / din;
		if (t < in + hold) return 0x1000;
		return (shl32(in - t + hold, 12) - 0x1000) / dout + 0x1000;
	}
	static void FadeTile(int32_t t, int32_t r, int32_t g, int32_t b, int32_t in, int32_t hold, int32_t out)
	{
		const int32_t level = FadeLevel(t, in, hold, out);
		PacketCursor() = Tile(r, g, b, level, PacketCursor());
	}

	static void MeasureHeight(uint8_t *e, int16_t *out)
	{
		*(int32_t *)(e + 0x58) = 0;
		ComputeBonesWorldMatrices(e + 0x60, e + 0x40);
		*(int32_t *)(e + 0x58) = *(const int16_t *)(e + 0x1E);
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

	static int32_t FlashOf(int32_t c)
	{
		if (c < 0xC) return c * 2300 / 12;
		if (c >= 0xD0) return (0xDC - c) * 2300 / 12;
		return 0x8FC;
	}

	static int16_t DropHeight(int32_t i)
	{
		const int32_t a = ComputeCos((int32_t)(((uint32_t)shl32(i, 12) >> 3) + 0x100) >> 2);
		const int32_t b = ComputeCos((0x1000 - a) >> 2);
		return (int16_t)(shl32(0x1000 - b, 12) / 4096 - 0x1000);
	}

	static void StartTrail()
	{
		TrailNode *t = (TrailNode *)AddTaskToQueue(QTrail(), game_addr(ORIG_TrailTask));
		t->counter = 0;
		for (int32_t k = 0; k < 7; k++) t->ring[k].alive = 0;
	}

	static int16_t g_spawn_pad = 0;

	static uint32_t __cdecl MainTask(TaskNode *n)
	{
		MainNode *mn = (MainNode *)n;
		int16_t sp[4];
		sp[3] = g_spawn_pad;
		if (mn->counter < 0x71)
		{
			GetEffectSpawnPosition(Caster(), 0, 0, sp);
			mn->spawn[0] = *(const uint32_t *)&sp[0];
			mn->spawn[1] = *(const uint32_t *)&sp[2];
		}
		else
		{
			GetEffectSpawnPosition(Caster(), 0, 0, sp);
			sp[0] = (int16_t)mn->spawn[0];
			sp[2] = (int16_t)mn->spawn[1];
		}
		const int32_t c0 = mn->counter;
		FlashLevel() = FlashOf(c0);
		if (c0 == 0x37)
		{
			InitTaskQueuePool(QTrail(), (void *)(TexBase() + POOL_Trail), 0x34C, 1);
			InitTaskQueuePool(QLayouts(), (void *)TexBase(), 0x158, 0x10);
			InitTaskQueuePool(QSparkles(), (void *)(TexBase() + POOL_Sparkles), 0x18, 0x10);
			if (Pause() == 0) StartTrail();
		}
		PlayArg arg = {};
		uint32_t i = (uint32_t)(mn->counter - 0x49);
		if (i < 0x14)
		{
			if (i == 0) DecodeModelPrimLayout(game_addr(LAYOUT_Fade), mn->layout_a, 0x1C8);
			arg.depth = 0;
			arg.kind = game_addr(KIND_Plain);
			arg.fade = i > 10 ? (int16_t)((uint32_t)shl32(i - 10, 12) / 10) : 0;
			Play(mn, 0, mn->layout_a, &arg, sp[0], 0, sp[2]);
		}
		i = (uint32_t)(mn->counter - 0x53);
		if (i < 0x5F)
		{
			if (i == 0) DecodeModelPrimLayout(game_addr(LAYOUT_Wave), mn->layout_b, 0x24C);
			if (i < 0x53 && i % 6 == 0 && Pause() == 0)
			{
				LayoutNode *t = (LayoutNode *)AddTaskToQueue(QLayouts(), game_addr(ORIG_LayoutTask));
				if (t)
				{
					DecodeModelPrimLayout(game_addr(LAYOUT_Rise), t->layout, 0xC0);
					t->pos[0] = sp[0];
					t->pos[1] = (int16_t)(sp[1] - 0x100);
					t->pos[2] = sp[2];
				}
			}
			arg.depth = 0;
			arg.kind = game_addr(KIND_Wave);
			arg.wave_y = 0x1000;
			arg.wave_xz = 0x1000;
			arg.fade = 0;
			if (i >= 0x50) arg.wave_xz = (int16_t)(ComputeSin((int32_t)((uint32_t)shl32(i - 0x50, 10) / 15)) * 2 + 0x1000);
			arg.wave_amp = 0x100;
			arg.wave_phase = (int16_t)(i << 6);
			arg.scroll = (uint16_t)(i << 5);
			if (mn->counter >= 0x8D)
			{
				int32_t u = shl32(mn->counter - 0x8D, 12) / 27;
				if (u > 0x1000) u = 0x1000;
				arg.wave_y = (int16_t)(u + 0x1000);
				if (mn->counter < 0xA8)
				{
					const int32_t a = ComputeCos(u >> 2);
					const int32_t b = ComputeCos((0x1000 - a) >> 2);
					const int16_t lift = (int16_t)(0x1000 - b);
					*(int16_t *)(Caster() + 0x1E) = (int16_t)(mn->caster0[1] - lift);
					*(int16_t *)(Target() + 0x1E) = (int16_t)(mn->target0[1] - lift);
				}
			}
			Play(mn, 1, mn->layout_b, &arg, sp[0], 0, sp[2]);
		}
		i = (uint32_t)(mn->counter - 0x53);
		if (i < 0x1F)
		{
			const int32_t s = ComputeSin((int32_t)((uint32_t)shl32(i, 12) / 31 + 0x42) / 4);
			VecLerp(mn->caster0, mn->far_pt, 0x1000 - s, s, Caster() + 0x1C);
		}
		i = (uint32_t)(mn->counter - 0x72);
		if (i < 0x19)
		{
			if (i == 0)
			{
				DecodeModelPrimLayout(game_addr(LAYOUT_Swing), mn->layout_a, 0x1A0);
				ff8fx::copy<(8)>(mn->keep, sp);
			}
			arg.depth = 0;
			arg.kind = game_addr(KIND_Swing);
			arg.scroll = (uint16_t)(i << 5);
			arg.fade = 0;
			Play(mn, 2, mn->layout_a, &arg, mn->keep[0], 0, mn->keep[2]);
		}
		i = (uint32_t)(mn->counter - 0xA3);
		if (i < 0xE)
		{
			if (i == 0)
			{
				DecodeModelPrimLayout(game_addr(LAYOUT_Push), mn->layout_a, 0x144);
				GetDefaultEffectPosition(Target(), mn->far_pt);
			}
			arg.depth = 0;
			arg.kind = game_addr(KIND_Plain);
			arg.fade = 0;
			Play(mn, 3, mn->layout_a, &arg, mn->far_pt[0], -0x1000, mn->far_pt[2]);
			if (Pause() == 0)
			{
				*(int16_t *)(Target() + 0x1C) += mn->step[0];
				*(int16_t *)(Target() + 0x20) += mn->step[2];
				if (i == 0xD) *Target() |= 4;
			}
		}
		i = (uint32_t)(mn->counter - 0xC8);
		if (i < 0x14)
		{
			if (i == 0)
			{
				DecodeModelPrimLayout(game_addr(LAYOUT_Fall), mn->layout_b, 0x208);
				if (Pause() == 0) QueueChainTransformation(Target(), 5);
			}
			arg.depth = 0;
			arg.kind = game_addr(KIND_Plain);
			arg.fade = 0;
			Play(mn, 4, mn->layout_b, &arg, mn->target0[0], 0, mn->target0[2]);
		}
		i = (uint32_t)(mn->counter - 0x72);
		if (i < 4) FadeTile((int32_t)i, 0xFF, 0xFF, 0xFF, 0, 1, 3);
		i = (uint32_t)(mn->counter - 0xA3);
		if (i < 4) FadeTile((int32_t)i, 0xFF, 0xFF, 0xFF, 0, 1, 3);
		i = (uint32_t)(mn->counter - 0xC0);
		bool draw_target = false;
		if (i < 8)
		{
			if (i == 0)
			{
				*(int16_t *)(Target() + 0x1C) = mn->target0[0];
				*(int16_t *)(Target() + 0x20) = mn->target0[2];
			}
			if (Pause() == 0)
			{
				*(int16_t *)(Target() + 0x1E) = DropHeight((int32_t)i);
				if (i == 7)
				{
					ff8fx::copy<(4)>(Target() + 0x1C, &mn->target0[0]);
					ff8fx::copy<(4)>(Target() + 0x20, &mn->target0[2]);
				}
			}
			draw_target = true;
		}
		else if (i < 0x1B) draw_target = true;
		else if (i == 0x1B)
		{
			if (Pause() == 0) ApplyActionResultToTarget(Ctx()->actions[0].targets);
			*(uint16_t *)Target() &= 0xFFFB;
		}
		if (draw_target)
		{
			int16_t box[2];
			MeasureHeight(Target(), box);
			int16_t off[4];
			int16_t angles[4];
			off[1] = (int16_t)((box[1] + box[0]) >> 1);
			off[2] = 0;
			off[0] = 0;
			off[3] = 0;
			angles[0] = 0x800;
			angles[2] = 0;
			angles[1] = 0;
			angles[3] = 0;
			DrawEntity212(Target(), angles, off);
			*Target() |= 4;
		}
		if (mn->counter == 0x21)
		{
			QFlames()->tail = nullptr;
			QFlames()->head = nullptr;
		}
		else if (mn->counter == 0x23) StreamQueueLoad(0x2C9, TexBase() + ARENA_A, 0);
		else if (mn->counter == 0x36 && StreamBusy() != 0) return 0;
		StreamPump();
		if (mn->counter == 0) BdPlaySE(SOUND_Start(), 0, 0x80);
		else if (mn->counter == 0x44) BdPlaySE(SOUND_Swing(), 0, 0x80);
		else if (mn->counter == 0xC7) BdPlaySE(SOUND_Fall(), 0, 0x80);
		i = (uint32_t)(mn->counter - 0x72);
		if (i < 0x36)
		{
			const uint32_t k = i % 5;
			if (k == 0)
			{
				LayoutNode *t = (LayoutNode *)AddTaskToQueue(QLayouts(), game_addr(ORIG_LayoutTask));
				if (t)
				{
					DecodeModelPrimLayout(game_addr(LAYOUT_Hit), t->layout, 0x144);
					GetDefaultEffectPosition(Target(), t->pos);
					QueueChainTransformation(Target(), 5);
				}
			}
			if (mn->counter < 0x8D)
			{
				const int32_t s = ComputeSin(shl32((int32_t)k, 12) / 9 / 2);
				VecLerp(mn->far_pt, mn->near_pt, 0x1000 - s, s, Caster() + 0x1C);
			}
		}
		i = (uint32_t)(mn->counter - 0xA8);
		if (i < 0xE)
		{
			const int32_t t = (int32_t)((uint32_t)shl32((int32_t)i + 1, 12) / 14);
			if (i == 0) ff8fx::copy<(8)>(mn->keep, Caster() + 0x1C);
			VecLerp(mn->keep, mn->caster0, 0x1000 - t, t, Caster() + 0x1C);
			if (i == 0xD)
			{
				int32_t count = 0x10;
				int32_t angle = 0;
				SparkleNode *s = (SparkleNode *)AddTaskToQueue(QSparkles(), game_addr(ORIG_SparkleTask));
				while (s)
				{
					const int32_t r = CrtRand() + 0x4000;
					const int32_t sx = mul32(ComputeSin(angle), r) >> 15;
					const int32_t cz = mul32(ComputeCos(angle), r) >> 15;
					s->pos[0] = (int16_t)(sx / 8 + mn->caster0[0]);
					s->pos[2] = (int16_t)(cz / 8 + mn->caster0[2]);
					s->vel[0] = (int8_t)(sx >> 4);
					s->vel[2] = (int8_t)(cz >> 4);
					angle += 0x100;
					s->pos[1] = 0;
					s->vel[1] = (int8_t)(0xF8 - (CrtRand() & 7));
					s->frame = 0;
					if (--count == 0) break;
					s = (SparkleNode *)AddTaskToQueue(QSparkles(), game_addr(ORIG_SparkleTask));
				}
			}
		}
		if (mn->counter >= 0xDC)
		{
			ff8fx::copy<(4)>(Caster() + 0x1C, &mn->caster0[0]);
			ff8fx::copy<(4)>(Caster() + 0x20, &mn->caster0[2]);
			return TASK_END;
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

	static void TrailSplines(TrailNode *t, int32_t cnt)
	{
		uint8_t *wk = (uint8_t *)FieldAlloc(0x190);
		SplineSetup(cnt, t->pts_a, wk);
		for (int32_t k = 0, u = 0; k < 32; k++, u += 0x1000) SplineEval(cnt, wk, t->spl_a[k], u / 31);
		SplineSetup(cnt, t->pts_b, wk);
		for (int32_t k = 0, u = 0; k < 32; k++, u += 0x1000) SplineEval(cnt, wk, t->spl_b[k], u / 31);
		FieldFree(0x190);
	}

	static void TrailEnds(TrailNode *t)
	{
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
		const int32_t want = t->counter >= 0xD ? 0x14 - t->counter : 7;
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
			TrailSplines(t, cnt);
			TrailEnds(t);
			TrailDraw(t->spl_a, t->spl_b, w);
		}
		FieldFree(0x40);
		if (Pause() != 0) return 0;
		t->counter++;
		return t->counter >= 0x13 ? TASK_END : 0;
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
		arg.fade = g_task_fade;
		arg.morph = TexBase() + MORPH;
		const int left = prim::play((prim::Layout *)t->layout, PrimCallback, (int)&arg, (int)Pause());
		return left ? 0 : TASK_END;
	}

	static uint32_t __cdecl SparkleTask(TaskNode *n)
	{
		SparkleNode *s = (SparkleNode *)n;
		TransformCameraByShadowRotation(s->pos, 0x1000, -0x400);
		uint8_t *h = AllocHeader(0xB4);
		*(uint32_t *)h = game_addr(SEQ_Sparkle);
		*(uint16_t *)(h + 4) = (uint16_t)s->frame;
		*(uint16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
		g_task_fade = (int16_t)RenderOT();
		FieldFree(0xB4);
		if (Pause() != 0) return 0;
		s->frame++;
		if (s->frame >= 0xD) return TASK_END;
		s->pos[0] = (int16_t)(s->pos[0] + s->vel[0]);
		s->pos[1] = (int16_t)(s->pos[1] + s->vel[1]);
		s->pos[2] = (int16_t)(s->pos[2] + s->vel[2]);
		return 0;
	}
}

	void register_mag251_demon_slice()
	{
		register_port(demon251::ORIG_MasterTask, (void *)demon251::MasterTask, "251 MasterTask", 251);
		register_port(demon251::ORIG_FlameSpawner, (void *)demon251::FlameSpawner, "251 FlameSpawner", 251);
		register_port(demon251::ORIG_FlameTask, (void *)demon251::FlameTask, "251 FlameTask", 251);
		register_port(demon251::ORIG_SparkTask, (void *)demon251::SparkTask, "251 SparkTask", 251);
		register_port(demon251::ORIG_EmberTask, (void *)demon251::EmberTask, "251 EmberTask", 251);
		register_port(demon251::ORIG_MainTask, (void *)demon251::MainTask, "251 MainTask", 251);
		register_port(demon251::ORIG_TrailTask, (void *)demon251::TrailTask, "251 TrailTask", 251);
		register_port(demon251::ORIG_LayoutTask, (void *)demon251::LayoutTask, "251 LayoutTask", 251);
		register_port(demon251::ORIG_SparkleTask, (void *)demon251::SparkleTask, "251 SparkleTask", 251);
	}
}

