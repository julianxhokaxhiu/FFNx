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

// Effect 133: Hypnotize

#include "mag_common.h"
#include "fx_params.h"

namespace ff8fx
{
namespace hypno133
{
	using namespace eng;
	using namespace magc;

	inline uint8_t *&Caster() { return var<uint8_t *>(game_addr(game::g_mag133_caster)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::g_mag133_magicBuffer)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::g_mag133_ctx)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::g_mag133_packetCursor)); }
	inline TaskQueue *QSprites() { return (TaskQueue *)game_addr(game::g_mag133_qSprites); }
	inline TaskQueue *QTargets() { return (TaskQueue *)game_addr(game::g_mag133_qTargetLayouts); }
	inline TaskQueue *QRings() { return (TaskQueue *)game_addr(game::g_mag133_qRings); }
	inline TaskQueue *QMain() { return (TaskQueue *)game_addr(game::g_mag133_qMain); }
	inline int16_t ProjectionH() { return var<int16_t>(game_addr(game::BATTLE_PROJECTION_DISTANCE)); }

	static constexpr game::Id ORIG_MasterTask = game::MAG_133_Hypnotize_MasterTask;
	static constexpr game::Id ORIG_MainTask = game::MAG_133_Hypnotize_MainTask;
	static constexpr game::Id ORIG_SpriteTask = game::MAG_133_Hypnotize_SpriteTask;
	static constexpr game::Id ORIG_RingTask = game::MAG_133_Hypnotize_RingTask;
	static constexpr game::Id ORIG_TargetLayoutTask = game::MAG_133_Hypnotize_TargetLayoutTask;
	static const uint32_t MORPH = 0xE04;
	static const uint32_t ARENA_A = 0xE0C;
	static const uint32_t ARENA_B = 0x10E0C;
	static constexpr game::Id MODEL_Main = game::unk_1207958;
	static const uint32_t *MODEL_Rings() { return (const uint32_t *)game_addr(game::unk_1212B20); }
	static const uint32_t *MODEL_Targets() { return (const uint32_t *)game_addr(game::unk_1212B40); }
	static const uint32_t *RING_TABLE() { return (const uint32_t *)game_addr(game::unk_1212AE0); }
	static constexpr game::Id SEQ_Sprite = game::unk_12077B8;
	static const void *SOUND_Hypnotize() { return (const void *)game_addr(game::unk_1212ADC); }

	struct HypnotizeParams
	{
		struct Timing
		{
			int16_t sound_tick;
			int16_t damage_tick;
			int16_t end_tick;
		} timing;
		struct Main
		{
			int16_t last_tick;
			int16_t basis_h;
			int16_t depth_h;
			int16_t offset_shift;
			int16_t pixels;
		} main;
		struct Spawned
		{
			fxp::Window window;
			int16_t every;
		};
		Spawned rings;
		struct Targets
		{
			Spawned spawn;
			fxp::Range spread_x;
			fxp::Range scale;
			int16_t depth;
		} targets;
		struct Sprites
		{
			Spawned spawn;
			int16_t bone;
			fxp::Range spread_x, spread_y, spread_z;
			int16_t frames;
		} sprites;
	};
	static const HypnotizeParams DEFAULTS = {
		{ 0, 0x22, 0x2A },
		{ 0x1D, 12, 16, 4, 1 },
		{ { 0, 0xF }, 2 },
		{ { { 2, 0x16 }, 2 }, { -0x400, 0x3FF }, { 0x800, 0xFFF }, -0x80 },
		{ { { 0x12, 0x29 }, 2 }, 0xF0, { -0x100, 0xFF }, { -0x180, 0x7F }, { -0x100, 0xFF }, 0xE },
	};
	static HypnotizeParams g_params = DEFAULTS;
	static inline const HypnotizeParams &P() { return g_params; }
	enum PartId { PART_MAIN = 1, PART_RINGS = 2, PART_TARGETS = 3, PART_SPRITES = 4 };
	static inline bool Spawns(const HypnotizeParams::Spawned &s, int32_t c)
	{
		return s.window.has(c) && s.every > 0 && (c - s.window.first) % s.every == 0;
	}

	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t flag, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, flag, out); }
	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void GteMVMVA_RotV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0))(); }
	inline uint32_t RenderPrim2(void *h, uint32_t ot, int mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int, uint32_t)>(game_addr(game::MAG_070_RenderPrimModel))(h, ot, mode, cursor); }
	inline int32_t BuildOrthonormalBasis(const int32_t *v, Mat4x3 *m) { return fn<int32_t (__cdecl *)(const int32_t *, Mat4x3 *)>(game_addr(game::BuildOrthonormalBasis))(v, m); }

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
		int16_t count;
		uint8_t *ents[3];
		uint8_t layout[0xA8];
	};
	struct RingNode
	{
		TaskNode hdr;
		int16_t point[4];
		uint8_t layout[0x9C];
	};
	struct TargetNode
	{
		TaskNode hdr;
		int16_t pos[3];
		int16_t scale;
		uint8_t layout[0x40];
	};
	struct SpriteNode
	{
		TaskNode hdr;
		int16_t pos[3];
		int16_t counter;
	};
	struct PrimArg
	{
		Mat4x3 m;
		int32_t depth;
		uint32_t morph;
	};
#pragma pack(pop)
	static_assert(sizeof(MasterNode) == 0x14 && sizeof(MainNode) == 0xC4 && sizeof(RingNode) == 0xB0, "Hypnotize nodes");
	static_assert(sizeof(TargetNode) == 0x54 && sizeof(SpriteNode) == 0x14, "Hypnotize nodes");
	static_assert(sizeof(PrimArg) == 0x28, "Hypnotize prim block");

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
		RotationFromAngles(r->rot, &m);
		int16_t off[4] = { r->pos[0], r->pos[1], r->pos[2], 0 };
		int32_t t[3];
		if (r->flags & 0x200)
		{
			t[0] = off[0];
			t[1] = off[1];
			t[2] = off[2];
		}
		else
		{
			GteSetRotMatrixCtrl(&arg->m);
			GteLoadV0(off);
			GteMVMVA_RotV0();
			GteReadMAC123(m.t);
			MatrixMultiply(&arg->m, &m);
			t[0] = m.t[0];
			t[1] = m.t[1];
			t[2] = m.t[2];
		}
		m.t[0] = (int32_t)((uint32_t)t[0] + (uint32_t)arg->m.t[0]);
		m.t[1] = (int32_t)((uint32_t)t[1] + (uint32_t)arg->m.t[1]);
		m.t[2] = (int32_t)((uint32_t)t[2] + (uint32_t)arg->m.t[2]);
		if (!(*(const uint32_t *)r->scale == 0x10001000 && r->scale[2] == 0x1000))
		{
			const int32_t v[3] = { r->scale[0], r->scale[1], r->scale[2] };
			Scale3DMatrix(&m, v);
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

	static void BonePoint(const int16_t *entry, int16_t *out)
	{
		uint8_t *c = Caster();
		Mat4x3 m = {};
		ComposeAffineTransform((const Mat4x3 *)(c + 0x40), (const Mat4x3 *)(**(uint8_t ***)(c + 0x64) + 0x20 + (int32_t)entry[3] * 0x30), &m);
		GteSetRotMatrixCtrl(&m);
		GteSetTransVectorCtrl(&m);
		GteLoadV0(entry);
		GteMVMVA_RotV0Tr();
		GteStoreIR123(out);
	}

	static void MainFrame(const int16_t *pos, PrimArg *arg)
	{
		const HypnotizeParams::Main &mp = P().main;
		GteSetRotMatrix(&Camera());
		GteSetTransVector(&Camera());
		GteLoadV0(pos);
		GteRTPS();
		int16_t sxy[2];
		GteReadSXY2(sxy);
		if (mp.pixels)
		{
			sxy[0] = (int16_t)(sxy[0] >> 3);
			sxy[1] = (int16_t)(sxy[1] >> 3);
		}
		int32_t *v = arg->m.t;
		v[0] = shl32(0xA0 - (int32_t)sxy[0], mp.offset_shift);
		v[1] = shl32(0x78 - (int32_t)sxy[1], mp.offset_shift);
		const int32_t h = ProjectionH();
		v[2] = h * mp.basis_h;
		BuildOrthonormalBasis(v, &arg->m);
		v[1] = (int32_t)(0u - (uint32_t)v[1]);
		v[0] = (int32_t)(0u - (uint32_t)v[0]);
		v[2] = h * mp.depth_h;
	}

	static void ScaledCamera(int32_t s, Mat4x3 *m)
	{
		GteSetIR0(s);
		GteLoadIR123(&Camera().m[0][0]);
		GteGPF();
		GteStoreIR123(&m->m[0][0]);
		GteLoadIR123(&Camera().m[1][0]);
		GteGPF();
		GteStoreIR123(&m->m[1][0]);
		GteLoadIR123(&Camera().m[2][0]);
		GteGPF();
		GteStoreIR123(&m->m[2][0]);
	}

	static void CameraAnchor(const int16_t *pos, Mat4x3 *m)
	{
		GteSetRotMatrixCtrl(&Camera());
		GteLoadV0(pos);
		GteMVMVA_RotV0();
		GteReadMAC123(m->t);
		m->t[0] = (int32_t)((uint32_t)m->t[0] + (uint32_t)Camera().t[0]);
		m->t[1] = (int32_t)((uint32_t)m->t[1] + (uint32_t)Camera().t[1]);
		m->t[2] = (int32_t)((uint32_t)m->t[2] + (uint32_t)Camera().t[2]);
	}

	static void SpriteDraw(const SpriteNode *s)
	{
		uint8_t *h = AllocHeader(0xB4);
		TransformCameraByShadowRotation(s->pos, 0x400, -0x100);
		*(uint32_t *)h = game_addr(SEQ_Sprite);
		*(int16_t *)(h + 4) = s->counter;
		*(uint16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
		FieldFree(0xB4);
	}
}
}

namespace ff8fx
{
namespace hypno133
{
	static uint32_t __cdecl SpriteTask(TaskNode *n)
	{
		SpriteNode *s = (SpriteNode *)n;
		FX_PART(PART_SPRITES);
		SpriteDraw(s);
		s->counter++;
		return s->counter < P().sprites.frames ? 0 : TASK_END;
	}

	static uint32_t __cdecl RingTask(TaskNode *n)
	{
		RingNode *g = (RingNode *)n;
		FX_PART(PART_RINGS);
		int16_t pos[4];
		BonePoint(g->point, pos);
		PrimArg arg;
		ff8fx::copy<(sizeof(arg.m.m))>(&arg.m.m, &Camera().m);
		CameraAnchor(pos, &arg.m);
		arg.depth = -0x80;
		arg.morph = TexBase() + MORPH;
		return prim::play((prim::Layout *)g->layout, PrimCallback, (int)&arg, 0) ? 0 : TASK_END;
	}

	static uint32_t __cdecl TargetLayoutTask(TaskNode *n)
	{
		TargetNode *p = (TargetNode *)n;
		FX_PART(PART_TARGETS);
		PrimArg arg;
		ScaledCamera(p->scale, &arg.m);
		CameraAnchor(p->pos, &arg.m);
		arg.depth = P().targets.depth;
		arg.morph = TexBase() + MORPH;
		return prim::play((prim::Layout *)p->layout, PrimCallback, (int)&arg, 0) ? 0 : TASK_END;
	}

	static uint8_t *RandomTarget(const MainNode *t)
	{
		const int32_t i = mul32(CrtRand(), (int32_t)t->count) >> 15;
		return t->ents[i];
	}

	static uint32_t __cdecl MainTask(TaskNode *n)
	{
		MainNode *t = (MainNode *)n;
		const HypnotizeParams &pp = P();
		if ((uint16_t)t->counter <= (uint16_t)pp.main.last_tick)
		{
			FX_PART(PART_MAIN);
			int16_t pos[4];
			GetDefaultEffectPosition(Caster(), pos);
			PrimArg arg;
			MainFrame(pos, &arg);
			arg.depth = 0;
			arg.morph = TexBase() + MORPH;
			prim::play((prim::Layout *)t->layout, PrimCallback, (int)&arg, 0);
		}
		if (t->counter == pp.timing.sound_tick) BdPlaySE(SOUND_Hypnotize(), 0, 0x80);
		const int32_t c = t->counter;
		if (Spawns(pp.rings, c))
		{
			RingNode *g = (RingNode *)AddTaskToQueue(QRings(), game_addr(ORIG_RingTask));
			if (g)
			{
				const uint32_t i = (uint32_t)((c - pp.rings.window.first) / pp.rings.every) & 7;
				((uint32_t *)g->point)[0] = RING_TABLE()[2 * i];
				((uint32_t *)g->point)[1] = RING_TABLE()[2 * i + 1];
				DecodeModelPrimLayout(MODEL_Rings()[i], g->layout, 0x9C);
			}
		}
		if (Spawns(pp.targets.spawn, c))
		{
			const uint32_t e = (uint32_t)((c - pp.targets.spawn.window.first) / pp.targets.spawn.every);
			TargetNode *p = (TargetNode *)AddTaskToQueue(QTargets(), game_addr(ORIG_TargetLayoutTask));
			if (p)
			{
				uint8_t *ent = RandomTarget(t);
				const int32_t flag = CrtRand() >> 3;
				const int32_t bone = mul32(CrtRand(), (int32_t)***(uint8_t ***)(ent + 0x64)) >> 15;
				GetEffectSpawnPosition(ent, bone, flag, p->pos);
				p->pos[0] = (int16_t)(p->pos[0] + pp.targets.spread_x.roll(CrtRand()));
				p->scale = (int16_t)pp.targets.scale.roll(CrtRand() >> 4);
				DecodeModelPrimLayout(MODEL_Targets()[e & 7], p->layout, 0x40);
			}
		}
		if (Spawns(pp.sprites.spawn, c))
		{
			SpriteNode *s = (SpriteNode *)AddTaskToQueue(QSprites(), game_addr(ORIG_SpriteTask));
			if (s)
			{
				uint8_t *ent = RandomTarget(t);
				GetEffectSpawnPosition(ent, pp.sprites.bone, 0, s->pos);
				s->pos[0] = (int16_t)(s->pos[0] + pp.sprites.spread_x.roll(CrtRand()));
				s->pos[1] = (int16_t)(s->pos[1] + pp.sprites.spread_y.roll(CrtRand()));
				const int32_t rz = CrtRand();
				s->counter = 0;
				s->pos[2] = (int16_t)(s->pos[2] + pp.sprites.spread_z.roll(rz));
			}
		}
		if (t->counter == pp.timing.damage_tick) ApplyActionResultToTargets(Ctx()->actions[0].targets, Ctx()->actions[0].target_count);
		t->counter++;
		return t->counter < pp.timing.end_tick ? 0 : TASK_END;
	}

	static uint32_t __cdecl MasterTask(TaskNode *n)
	{
		MasterNode *m = (MasterNode *)n;
		if (m->arena)
		{
			PacketCursor() = TexBase() + ARENA_A;
			m->arena = 0;
		}
		else
		{
			PacketCursor() = TexBase() + ARENA_B;
			m->arena = 1;
		}
		if (m->counter == 1 && !m->started)
		{
			m->started = 1;
			InitTaskQueuePool(QMain(), (void *)(TexBase() + 0xD40), 0xC4, 1);
			InitTaskQueuePool(QRings(), (void *)(TexBase() + 0x7C0), 0xB0, 8);
			InitTaskQueuePool(QTargets(), (void *)(TexBase() + 0x280), 0x54, 0x10);
			InitTaskQueuePool(QSprites(), (void *)TexBase(), 0x14, 0x20);
			MainNode *t = (MainNode *)AddTaskToQueue(QMain(), game_addr(ORIG_MainTask));
			Memset32(&t->counter, 0, 0x2E);
			t->count = (int16_t)Ctx()->actions[0].target_count;
			for (int32_t i = 0; i < t->count; i++)
				t->ents[i] = Entity(Ctx()->actions[0].targets[i * TARGET_STRIDE]);
			DecodeModelPrimLayout(game_addr(MODEL_Main), t->layout, 0xA8);
		}
		int a0, a1, a2, a3;
		if (m->started)
		{
			a0 = ExecuteTaskQueue(QMain());
			a1 = ExecuteTaskQueue(QRings());
			a2 = ExecuteTaskQueue(QTargets());
			a3 = ExecuteTaskQueue(QSprites());
			FX_PART(0);
		}
		else a0 = a1 = a2 = a3 = (int)n;
		if (m->started && a0 == 0 && a1 == 0 && a2 == 0 && a3 == 0) return TASK_END;
		m->counter++;
		return 0;
	}
}

	namespace hypno133
	{
		using fxp::Field;
#define HYPNO_FIELD(part, member, label, doc, type, lo, hi) \
		{ part, #member, label, doc, (uint32_t)offsetof(HypnotizeParams, member), fxp::type, lo, hi }
		static const fxp::Part PARTS[] = {
			{ 0, "timing", "Timing", "The main task's clock: when the sound plays, when the targets take the result, when it ends (draws nothing)" },
			{ PART_MAIN, "main", "Main layout", "The layout in front of the caster, framed in view space on its screen position" },
			{ PART_RINGS, "rings", "Rings", "Eight layouts at points of the caster's bones, one every other tick" },
			{ PART_TARGETS, "targets", "Target layouts", "Layouts at random bones of random targets, at random sizes" },
			{ PART_SPRITES, "sprites", "Sprites", "A flipbook at random points around a target's bone" },
		};
		static const Field FIELDS[] = {
			HYPNO_FIELD("timing", timing.sound_tick, "Sound tick", "The sound plays at this tick", I16, 0, 200),
			HYPNO_FIELD("timing", timing.damage_tick, "Damage tick", "The targets take the result at this tick", I16, 0, 200),
			HYPNO_FIELD("timing", timing.end_tick, "End tick", "The main task ends at this tick", I16, 1, 200),
			HYPNO_FIELD("main", main.last_tick, "Last tick", "Drawn up to this tick", I16, -1, 200),
			HYPNO_FIELD("main", main.basis_h, "Basis depth (x H)", "Its orientation: the caster's screen offset seen at this many projection distances", I16, 1, 64),
			HYPNO_FIELD("main", main.depth_h, "Depth (x H)", "Its distance: this many projection distances", I16, 1, 64),
			HYPNO_FIELD("main", main.offset_shift, "Offset scale (2^n)", "The screen offset times 2^n", I16, 0, 8),
			HYPNO_FIELD("main", main.pixels, "PSX screen units", "1: the caster's screen position in pixels as on PSX; 0: in the PC's eighths, as the PC had it (the layout lands far off screen)", I16, 0, 1),
			HYPNO_FIELD("rings", rings.window.first, "From tick", "Spawned from this tick...", I16, 0, 200),
			HYPNO_FIELD("rings", rings.window.last, "To tick", "...to this one...", I16, 0, 200),
			HYPNO_FIELD("rings", rings.every, "Every", "...every this many ticks (8 layouts, then they repeat)", I16, 1, 50),
			HYPNO_FIELD("targets", targets.spawn.window.first, "From tick", "Spawned from this tick...", I16, 0, 200),
			HYPNO_FIELD("targets", targets.spawn.window.last, "To tick", "...to this one...", I16, 0, 200),
			HYPNO_FIELD("targets", targets.spawn.every, "Every", "...every this many ticks", I16, 1, 50),
			HYPNO_FIELD("targets", targets.spread_x.min, "Spread x min", "Added to the bone point's x, at random in [min, max]", I16, -0x2000, 0x2000),
			HYPNO_FIELD("targets", targets.spread_x.max, "Spread x max", "", I16, -0x2000, 0x2000),
			HYPNO_FIELD("targets", targets.scale.min, "Size min", "4096 = its own size", I16, 0, 0x7FFF),
			HYPNO_FIELD("targets", targets.scale.max, "Size max", "", I16, 0, 0x7FFF),
			HYPNO_FIELD("targets", targets.depth, "Depth", "Drawing depth offset", I16, -0x1000, 0x1000),
			HYPNO_FIELD("sprites", sprites.spawn.window.first, "From tick", "Spawned from this tick...", I16, 0, 200),
			HYPNO_FIELD("sprites", sprites.spawn.window.last, "To tick", "...to this one...", I16, 0, 200),
			HYPNO_FIELD("sprites", sprites.spawn.every, "Every", "...every this many ticks", I16, 1, 50),
			HYPNO_FIELD("sprites", sprites.bone, "Bone", "The target's bone point they start from", I16, 0, 0xFF),
			HYPNO_FIELD("sprites", sprites.spread_x.min, "Spread x min", "Added to the bone point, at random in [min, max]", I16, -0x2000, 0x2000),
			HYPNO_FIELD("sprites", sprites.spread_x.max, "Spread x max", "", I16, -0x2000, 0x2000),
			HYPNO_FIELD("sprites", sprites.spread_y.min, "Spread y min", "y goes down", I16, -0x2000, 0x2000),
			HYPNO_FIELD("sprites", sprites.spread_y.max, "Spread y max", "", I16, -0x2000, 0x2000),
			HYPNO_FIELD("sprites", sprites.spread_z.min, "Spread z min", "", I16, -0x2000, 0x2000),
			HYPNO_FIELD("sprites", sprites.spread_z.max, "Spread z max", "", I16, -0x2000, 0x2000),
			HYPNO_FIELD("sprites", sprites.frames, "Frames", "Flipbook frames: lifetime", I16, 1, 64),
		};
#undef HYPNO_FIELD
	}

	void register_mag133_hypnotize()
	{
		fxp::register_params({ 133, "Hypnotize", &hypno133::g_params, &hypno133::DEFAULTS, sizeof(hypno133::HypnotizeParams),
		                       hypno133::PARTS, (int)(sizeof(hypno133::PARTS) / sizeof(hypno133::PARTS[0])),
		                       hypno133::FIELDS, (int)(sizeof(hypno133::FIELDS) / sizeof(hypno133::FIELDS[0])) });
		register_port(hypno133::ORIG_MasterTask, (void *)hypno133::MasterTask, "133 MasterTask", 133);
		register_port(hypno133::ORIG_MainTask, (void *)hypno133::MainTask, "133 MainTask", 133);
		register_port(hypno133::ORIG_RingTask, (void *)hypno133::RingTask, "133 RingTask", 133);
		register_port(hypno133::ORIG_TargetLayoutTask, (void *)hypno133::TargetLayoutTask, "133 TargetLayoutTask", 133);
		register_port(hypno133::ORIG_SpriteTask, (void *)hypno133::SpriteTask, "133 SpriteTask", 133);
	}
	FX_LAB_MODULE(register_mag133_hypnotize);
}

