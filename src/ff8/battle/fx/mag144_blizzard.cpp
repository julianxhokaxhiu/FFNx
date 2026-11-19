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

// Effect 144: Blizzard

#include "mag_common.h"

namespace ff8fx
{
namespace blizzard144
{
	using namespace eng;
	using namespace magc;

	inline TaskQueuePair &Queues() { return var<TaskQueuePair>(game_addr(game::MAG144_BLIZZARD_queues)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::MAG144_BLIZZARD_castCtx)); }
	inline uint8_t *&TexBase() { return var<uint8_t *>(game_addr(game::MAG_144_BLIZZARD_Texture)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG144_BLIZZARD_pktCursor)); }
	inline uint8_t *ShardBuffer(int action) { return (uint8_t *)(game_addr(game::MAG144_BLIZZARD_shardBuffers) + 0xA00 * action); }
	inline int16_t &CameraShake() { return var<int16_t>(game_addr(game::BATTLE_CAMERA_SHAKE_OFFSET_Y)); }

	static constexpr game::Id ORIG_RootTask = game::MAG_144_BLIZZARD_RootTask;
	static constexpr game::Id ORIG_Director = game::MAG_144_BLIZZARD_Director;
	static constexpr game::Id ORIG_ShockwaveTask = game::MAG_144_BLIZZARD_Shockwave_Tick;
	static constexpr game::Id ORIG_DebrisTask = game::MAG_144_BLIZZARD_Debris_Tick;
	static constexpr game::Id ORIG_FallingIceTask = game::MAG_144_BLIZZARD_FallingIce_Tick;
	static constexpr game::Id ORIG_MistPuffTask = game::MAG_144_BLIZZARD_MistPuff_Tick;
	static constexpr game::Id ORIG_CrystalTask = game::MAG_144_BLIZZARD_Crystal_Tick;
	static const uint8_t *MODEL_Crystal() { return (const uint8_t *)game_addr(game::MAG144_BLIZZARD_crystalMesh); }
	static const uint8_t *MODEL_CrystalUV() { return (const uint8_t *)game_addr(game::MAG144_BLIZZARD_crystalPrims); }
	static const void *MODEL_Shockwave() { return (const void *)game_addr(game::MAG144_BLIZZARD_shockwaveModel); }
	static const void *SEQ_Ice() { return (const void *)game_addr(game::MAG144_BLIZZARD_iceSprite); }
	static const void *SEQ_Mist() { return (const void *)game_addr(game::MAG144_BLIZZARD_mistSprite); }
	static const void *SOUND_Blizzard() { return (const void *)game_addr(game::MAG144_BLIZZARD_soundId); }

	inline uint32_t RenderOT44() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44; }

	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t flag, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, flag, out); }
	inline int32_t NormalizeVector16(int16_t *in, int16_t *out) { return fn<int32_t (__cdecl *)(int16_t *, int16_t *)>(game_addr(game::VectorNormalS16))(in, out); }
	inline int32_t ISqrt(int32_t v) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::Math_SqrtInt))(v); }
	inline int32_t ComputeCos(int32_t angle) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(angle); }
	inline void GteLoadV012(const void *a, const void *b, const void *c) { fn<void (__cdecl *)(const void *, const void *, const void *)>(game_addr(game::GTE_LoadV012))(a, b, c); }
	inline void GteRTPT() { fn<void (__cdecl *)()>(game_addr(game::GTE_RTPT))(); }
	inline void GteReadFLAG(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadFLAG))(dst); }
	inline void GteReadSXY012(void *a, void *b, void *c) { fn<void (__cdecl *)(void *, void *, void *)>(game_addr(game::GTE_ReadSXY012_Split))(a, b, c); }
	inline void GteAVSZ3() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ3))(); }
	inline void GteReadOTZReg(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadOTZ))(dst); }
	inline void GteSetFarColor(int32_t r, int32_t g, int32_t b) { fn<void (__cdecl *)(int32_t, int32_t, int32_t)>(game_addr(game::GTE_SetFarColor))(r, g, b); }
	inline void GteLoadRGBC(const void *c) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadRGBC))(c); }
	inline void GteDPCS() { fn<void (__cdecl *)()>(game_addr(game::GTE_DPCS))(); }
	inline void GteStoreRGB2(void *c) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreRGB2))(c); }

	struct RootNode { TaskNode hdr; uint16_t counter; };
	struct DirectorNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t action;
		int16_t pos[3];
		int16_t height;
		uint8_t pad18[8];
		int16_t slot;
		int16_t pad22;
	};
	struct CrystalNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t action;
		int16_t pos[3];
		int16_t slam;
		int16_t yaw;
		int16_t yaw_vel;
		int16_t scale;
		int16_t pad1E;
		int16_t lift;
		int16_t pad22;
	};
	struct ShockwaveNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		int16_t pos[3];
		uint8_t pad16[6];
		int16_t scale;
		int16_t scale_vel;
		uint8_t pad20[4];
	};
	struct DebrisNode
	{
		TaskNode hdr;
		int16_t frame;
		int16_t pad0E;
		int16_t pos[3];
		int16_t pad16;
		int16_t vel_x;
		int16_t pad1A;
		int16_t vel_z;
		int16_t pad1E;
		int16_t scale;
		int16_t pad22;
	};
	struct FallingIceNode
	{
		TaskNode hdr;
		int16_t frame;
		int16_t pad0E;
		int16_t pos[3];
		int16_t vel_y;
		uint8_t pad18[4];
		int16_t scale;
		uint8_t pad1E[6];
	};
	struct MistNode
	{
		TaskNode hdr;
		int16_t frame;
		int16_t delay;
		int16_t pos[3];
		int16_t pad16;
		uint8_t pad18[4];
		int16_t scale;
		uint8_t pad1E[6];
	};
	static_assert(sizeof(DirectorNode) == 0x24 && sizeof(CrystalNode) == 0x24 && sizeof(ShockwaveNode) == 0x24 && sizeof(DebrisNode) == 0x24
		&& sizeof(FallingIceNode) == 0x24 && sizeof(MistNode) == 0x24, "Blizzard nodes are 0x24 bytes");

#pragma pack(push, 1)
	struct Shard
	{
		int16_t center[3];
		int16_t delay;
		int16_t angle[3];
		int16_t step;
		int16_t angle_vel[3];
		int16_t steps;
		int16_t dir_x;
		int16_t radius_vel;
		int16_t dir_z;
		int16_t radius;
	};
	struct CrystalHeader
	{
		const uint8_t *model;
		const uint8_t *uvmodel;
		uint8_t far_rgb[3];
		uint8_t pad0B;
		int32_t ir0;
		int32_t limit;
		uint32_t pad14;
		uint32_t flags;
		const uint8_t *verts;
		const uint8_t *faces;
		const uint8_t *uvs;
		uint8_t *shards;
		int32_t cos_a;
		uint32_t pad30;
		int32_t sin_otz;
		uint32_t gte_flag;
		int16_t v0[4];
		int16_t v1[4];
		int16_t v2[4];
		uint32_t pad54;
		int16_t center[4];
		int16_t rx, pad62;
		int16_t rz, pad66;
		Mat4x3 base;
		Mat4x3 shard;
		uint16_t tw1[4];
		uint16_t tw0[4];
		int32_t v_scroll[3];
		uint32_t padC4, padC8;
		int32_t scroll;
	};
#pragma pack(pop)
	static_assert(sizeof(Shard) == 0x20 && sizeof(CrystalHeader) == 0xD0, "Blizzard shard / crystal header layouts");
}
}

namespace ff8fx
{
namespace blizzard144
{
	static uint32_t __cdecl RootTask(TaskNode *n)
	{
		RootNode *r = (RootNode *)n;
		if (r->counter & 1) PacketCursor() = (uint32_t)(TexBase() + 0xC000);
		else PacketCursor() = (uint32_t)TexBase();
		int alive = ExecuteTaskQueue(&Queues().second);
		r->counter++;
		return alive ? 0 : TASK_END;
	}

	static int32_t ShardDelayDiv(int32_t sum_y)
	{
		int32_t x = shl32(sum_y, 3) + 0x4780;
		int32_t hi = (int32_t)(((int64_t)x * (int32_t)0x84767DFF) >> 32);
		hi += x;
		hi >>= 11;
		hi += (int32_t)((uint32_t)hi >> 31);
		return hi;
	}

	static void BuildShards(CrystalHeader *h, const uint8_t *verts)
	{
		const uint8_t *f = h->faces;
		int32_t count = *(const int32_t *)f;
		f += 4;
		h->faces = f;
		Shard *s = (Shard *)h->shards;
		for (int32_t left = count; left > 0; left--)
		{
			const int16_t *a = (const int16_t *)(verts + 4 * *(const uint16_t *)(f + 4));
			const int16_t *b = (const int16_t *)(verts + 4 * *(const uint16_t *)(f + 6));
			const int16_t *c = (const int16_t *)(verts + 4 * *(const uint16_t *)(f + 8));
			int32_t sx = a[0] + b[0] + c[0];
			int32_t sy = a[1] + b[1] + c[1];
			int32_t sz = a[2] + b[2] + c[2];
			int32_t r = CrtRand();
			int16_t cx = (int16_t)(sx / 3);
			s->center[0] = cx;
			int16_t cz = (int16_t)(sz / 3);
			s->center[1] = (int16_t)(sy / 3);
			int32_t t = ShardDelayDiv(sy);
			s->center[2] = cz;
			s->delay = (int16_t)(r % 3 - t + 8);
			int32_t vx = (r & 7) << 5;
			s->angle_vel[0] = (int16_t)vx;
			int32_t vz = ((r >> 3) & 7) << 4;
			int32_t ay = r % 1000 + 1000;
			s->angle[1] = (int16_t)ay;
			s->angle_vel[2] = (int16_t)vz;
			s->step = 0;
			if (r & 0x800) { vx = -vx; s->angle_vel[0] = (int16_t)vx; }
			if (r & 0x4080) { vz = -vz; s->angle_vel[2] = (int16_t)vz; }
			int32_t k = r % 8 + 12;
			s->angle[0] = (int16_t)(s->angle_vel[0] * (int16_t)k);
			s->dir_x = cx;
			int16_t day = (int16_t)((int16_t)ay / k);
			s->angle[2] = (int16_t)(s->angle_vel[2] * (int16_t)k);
			s->steps = (int16_t)k;
			s->radius_vel = 0;
			s->dir_z = cz;
			s->angle_vel[1] = day;
			NormalizeVector16(&s->dir_x, &s->dir_x);
			int32_t m = r % 5000 + 0xDAC;
			s->radius = (int16_t)(ISqrt(m) + m);
			f += 0x14;
			s->radius_vel = (int16_t)(m / k);
			s++;
		}
		h->shards = (uint8_t *)s;
		h->faces = f;
	}

	static void ShatterPrep(CrystalHeader *h)
	{
		const uint8_t *model = h->model;
		int32_t size = *(const int32_t *)model;
		h->verts = model + 8;
		h->faces = model + size + 8;
		BuildShards(h, model + 8);
	}

	static uint32_t __cdecl DirectorTask(TaskNode *n)
	{
		DirectorNode *d = (DirectorNode *)n;
		if (DrawOnly()) return 0;
		if (d->counter == 0)
		{
			GetEffectSpawnPosition(Entity(d->slot), 0xF0, 0x1000, d->pos);
			d->height = EntityHeight(d->slot);
			CrystalHeader *h = (CrystalHeader *)FieldAlloc(0xD0);
			h->model = MODEL_Crystal();
			h->flags = 0;
			h->shards = ShardBuffer(d->action);
			ShatterPrep(h);
			FieldFree(0xD0);
		}
		if (d->counter == 1)
		{
			CrystalNode *c = (CrystalNode *)AddTaskToQueue(&Queues().second, game_addr(ORIG_CrystalTask));
			ff8fx::copy<(8)>(c->pos, d->pos);
			c->pos[1] = (int16_t)(c->pos[1] - 900);
			c->counter = 0;
			c->action = d->action;
			c->yaw = (int16_t)(CrtRand() % 4096);
			int32_t r = CrtRand() % 300;
			c->scale = 0x5B6;
			c->lift = 0x50;
			c->yaw_vel = (int16_t)(r + 600);
			int16_t v = (int16_t)(c->pos[1] - d->height + 0xC4);
			c->slam = (int16_t)(v / 2);
		}
		if (d->counter >= 2 && d->counter <= 0x1B && (d->counter & 1))
		{
			MistNode *m = (MistNode *)AddTaskToQueue(&Queues().second, game_addr(ORIG_MistPuffTask));
			m->frame = 0;
			m->delay = (int16_t)(CrtRand() % 4);
			ff8fx::copy<(8)>(m->pos, d->pos);
			m->pos[1] = (int16_t)(m->pos[1] - 1000);
			m->pos[0] += (int16_t)(CrtRand() % 1000 - 500);
			m->pos[1] += (int16_t)(CrtRand() % 1000 - 500);
			m->pos[2] += (int16_t)(CrtRand() % 1000 - 500);
			m->scale = (int16_t)(CrtRand() % 1024 + 0x400);
		}
		if (d->counter >= 0x12 && d->counter <= 0x20)
		{
			int32_t count = d->counter / 16 + 1;
			for (int32_t k = count; k > 0; k--)
			{
				FallingIceNode *f = (FallingIceNode *)AddTaskToQueue(&Queues().second, game_addr(ORIG_FallingIceTask));
				ff8fx::copy<(8)>(f->pos, d->pos);
				f->pos[1] = (int16_t)(f->pos[1] - 600);
				f->frame = 0;
				f->pos[0] += (int16_t)(CrtRand() % 1000 - 500);
				f->pos[1] += (int16_t)(CrtRand() % 1000 - 500);
				f->pos[2] += (int16_t)(CrtRand() % 1000 - 500);
				f->vel_y = (int16_t)(CrtRand() % 30 + 25);
				f->scale = (int16_t)(CrtRand() % 0x900 + 0x600);
			}
		}
		if (d->counter >= 0x25 && d->counter <= 0x27)
		{
			for (int k = 0; k < 20; k++)
			{
				DebrisNode *b = (DebrisNode *)AddTaskToQueue(&Queues().second, game_addr(ORIG_DebrisTask));
				b->frame = 0;
				b->pos[0] = d->pos[0];
				b->pos[1] = d->height;
				b->pos[2] = d->pos[2];
				int32_t v[3];
				v[0] = CrtRand() % 4096 - 0x800;
				v[1] = 0;
				v[2] = CrtRand() % 4096 - 0x800;
				NormalizeVector(v, v);
				int32_t r = CrtRand() % 350 + 400;
				b->pos[0] += (int16_t)((v[0] * r) >> 12);
				b->pos[2] += (int16_t)((v[2] * r) >> 12);
				r = CrtRand() % 40 + 30;
				b->vel_x = (int16_t)((v[0] * r) >> 12);
				b->vel_z = (int16_t)((v[2] * r) >> 12);
				b->scale = (int16_t)(CrtRand() % 0x600 + 0x900);
			}
		}
		if (d->counter == 0x25)
		{
			ShockwaveNode *s = (ShockwaveNode *)AddTaskToQueue(&Queues().second, game_addr(ORIG_ShockwaveTask));
			s->pos[0] = d->pos[0];
			s->pos[1] = d->height;
			s->pos[2] = d->pos[2];
			s->counter = 0;
			s->scale = 0x27D;
			s->scale_vel = 0x27D;
			ApplyActionResultToTarget(Ctx()->actions[d->action].targets);
		}
		if (d->counter == 0x1C)
		{
			int next = d->action + 1;
			if (next <= Ctx()->actions[0].last_action)
			{
				DirectorNode *t = (DirectorNode *)AddTaskToQueue(&Queues().second, game_addr(ORIG_Director));
				t->counter = 0;
				t->action = (int16_t)next;
				t->slot = (int16_t)(uint16_t)Ctx()->actions[next].targets[0];
			}
		}
		if (d->counter == 1)
		{
			int16_t pos[4];
			GetDefaultEffectPosition(Entity(d->slot), pos);
			BdPlaySE3D(SOUND_Blizzard(), 0x100, pos);
		}
		d->counter++;
		return d->counter > 0x2C ? TASK_END : 0;
	}

	static void ShockwaveMatrix(const ShockwaveNode *p)
	{
		int16_t angles[4] = { 0, 0, 0, 0 };
		Mat4x3 m = {};
		BuildRotationMatrixFromAngles(angles, &m);
		m.t[0] = p->pos[0];
		m.t[1] = p->pos[1];
		m.t[2] = p->pos[2];
		int32_t sv[4] = { p->scale, 0x1000, p->scale, 0 };
		Scale3DMatrix(&m, sv);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
	}

	static void ShockwaveHeader(uint8_t *h, int16_t c)
	{
		*(const void **)h = MODEL_Shockwave();
		*(uint32_t *)(h + 0x1C) = 3;
		if (c >= 8)
		{
			h[0xA] = 0;
			h[9] = 0;
			h[8] = 0;
			*(uint32_t *)(h + 0x1C) = 0xC3;
			*(int32_t *)(h + 0xC) = (c - 8) * 682;
		}
	}

	static void ShockwaveUpdate(ShockwaveNode *p)
	{
		int16_t v = p->scale_vel;
		p->scale = (int16_t)(p->scale + v);
		p->counter++;
		p->scale_vel = (int16_t)(v - (int16_t)(v >> 3));
	}

	static uint32_t __cdecl ShockwaveTask(TaskNode *n)
	{
		ShockwaveNode *p = (ShockwaveNode *)n;
		ShockwaveMatrix(p);
		uint8_t *h = AllocHeader(0x58);
		ShockwaveHeader(h, p->counter);
		PacketCursor() = RenderPrimModel(h, RenderOT44(), 2, PacketCursor());
		FieldFree(0x58);
		if (DrawOnly()) return 0;
		ShockwaveUpdate(p);
		return p->counter >= 0xE ? TASK_END : 0;
	}

	static void DebrisDraw(const DebrisNode *p)
	{
		TransformCameraByShadowRotation(p->pos, p->scale, -(p->scale >> 3));
		uint8_t *h = AllocHeader(0xB4);
		*(uint16_t *)(h + 4) = (uint16_t)p->frame;
		*(const void **)h = SEQ_Ice();
		*(uint16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT44(), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static void DebrisUpdate(DebrisNode *p)
	{
		int16_t vx = p->vel_x, vz = p->vel_z;
		p->pos[0] = (int16_t)(p->pos[0] + vx);
		p->pos[2] = (int16_t)(p->pos[2] + vz);
		p->vel_x = (int16_t)(vx - (int16_t)(vx >> 4));
		p->frame++;
		p->vel_z = (int16_t)(vz - (int16_t)(vz >> 4));
	}

	static uint32_t __cdecl DebrisTask(TaskNode *n)
	{
		DebrisNode *p = (DebrisNode *)n;
		DebrisDraw(p);
		if (DrawOnly()) return 0;
		DebrisUpdate(p);
		return p->frame >= 0x10 ? TASK_END : 0;
	}

	static void FallingIceDraw(const FallingIceNode *p)
	{
		TransformCameraByShadowRotation(p->pos, p->scale, -(p->scale >> 3));
		uint8_t *h = AllocHeader(0xB4);
		*(uint16_t *)(h + 4) = (uint16_t)p->frame;
		*(const void **)h = SEQ_Ice();
		*(uint16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT44(), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static void FallingIceUpdate(FallingIceNode *p)
	{
		int16_t v = p->vel_y;
		p->pos[1] = (int16_t)(p->pos[1] + v);
		p->frame++;
		p->vel_y = (int16_t)(v + (int16_t)(v >> 6));
	}

	static uint32_t __cdecl FallingIceTask(TaskNode *n)
	{
		FallingIceNode *p = (FallingIceNode *)n;
		FallingIceDraw(p);
		if (DrawOnly()) return 0;
		FallingIceUpdate(p);
		return p->frame >= 0x10 ? TASK_END : 0;
	}

	static void MistHeader(uint8_t *h, int16_t frame)
	{
		*(const void **)h = SEQ_Mist();
		*(uint16_t *)(h + 4) = (uint16_t)(frame > 4 ? 4 : frame);
		*(uint16_t *)(h + 0x24) = 8;
		if (frame >= 4)
		{
			uint8_t level = (uint8_t)((uint8_t)frame * 0xE0);
			h[0x1E] = level;
			h[0x1D] = level;
			h[0x1C] = level;
			*(uint16_t *)(h + 0x24) = 0xC;
		}
	}

	static uint32_t __cdecl MistPuffTask(TaskNode *n)
	{
		MistNode *p = (MistNode *)n;
		if (p->delay > 0)
		{
			if (DrawOnly()) return 0;
			p->delay--;
			return 0;
		}
		TransformCameraByShadowRotation(p->pos, p->scale, -(p->scale >> 5));
		uint8_t *h = AllocHeader(0xB4);
		MistHeader(h, p->frame);
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT44(), 2, PacketCursor());
		FieldFree(0xB4);
		if (DrawOnly()) return 0;
		p->frame++;
		return p->frame >= 8 ? TASK_END : 0;
	}

	static uint32_t TextureWindow(const uint16_t *w)
	{
		uint32_t c = ((uint32_t)(uint8_t)w[1] & 0xF8) | 0xFFFE2000;
		c = (c << 5) | ((uint32_t)(uint8_t)w[0] & 0xF8);
		c = (c << 5) | ((uint32_t)-(int32_t)w[3] & 0xF8);
		c = (c << 2) | ((uint32_t)(-(int32_t)w[2] >> 3) & 0x1F);
		return c;
	}

	static uint32_t CrystalRenderCore(CrystalHeader *h, uint32_t ot, int shift, uint32_t cursor)
	{
		int32_t limit = h->limit;
		Shard *s = (Shard *)h->shards;
		const uint8_t *f = h->faces;
		int32_t count = *(const int32_t *)f;
		f += 4;
		h->faces = f;
		const uint8_t *uv = h->uvs;
		uv += 4;
		h->uvs = uv;
		int32_t tw_level = (int16_t)h->tw0[3];
		uint8_t *p = (uint8_t *)cursor;
		const uint8_t *verts = h->verts;
		for (int32_t left = count; left > 0; left--, f += 0x14, uv += 0x14, s++)
		{
			if (s->delay > limit) continue;
			const int16_t *a = (const int16_t *)(verts + 4 * *(const uint16_t *)(f + 4));
			const int16_t *b = (const int16_t *)(verts + 4 * *(const uint16_t *)(f + 6));
			const int16_t *c = (const int16_t *)(verts + 4 * *(const uint16_t *)(f + 8));
			bool moving = s->step < s->steps;
			if (moving)
			{
				ff8fx::copy<(8)>(h->center, s->center);
				ff8fx::copy<(8)>(h->v0, a);
				h->v0[0] = (int16_t)(h->v0[0] - h->center[0]);
				h->v0[1] = (int16_t)(h->v0[1] - h->center[1]);
				h->v0[2] = (int16_t)(h->v0[2] - h->center[2]);
				ff8fx::copy<(8)>(h->v1, b);
				h->v1[0] = (int16_t)(h->v1[0] - h->center[0]);
				h->v1[1] = (int16_t)(h->v1[1] - h->center[1]);
				h->v1[2] = (int16_t)(h->v1[2] - h->center[2]);
				ff8fx::copy<(8)>(h->v2, c);
				h->v2[0] = (int16_t)(h->v2[0] - h->center[0]);
				h->v2[1] = (int16_t)(h->v2[1] - h->center[1]);
				h->v2[2] = (int16_t)(h->v2[2] - h->center[2]);
				int32_t cs = ComputeCos(s->angle[1]);
				h->cos_a = cs;
				int32_t sn = ComputeSin(s->angle[1]);
				h->sin_otz = sn;
				int32_t rx = (mul32(cs, s->dir_x) >> 12) + (-mul32(sn, s->dir_z) >> 12);
				h->rx = (int16_t)rx;
				int32_t rz = (mul32(sn, s->dir_x) >> 12) + (mul32(cs, s->dir_z) >> 12);
				int16_t radius = s->radius;
				h->rz = (int16_t)rz;
				h->shard.t[0] = mul32((int16_t)rx, radius) >> 12;
				h->shard.t[2] = mul32((int16_t)rz, radius) >> 12;
				h->shard.t[1] = h->center[1];
				if (!DrawOnly())
				{
					s->angle[0] = (int16_t)(s->angle[0] - s->angle_vel[0]);
					s->angle[1] = (int16_t)(s->angle[1] - s->angle_vel[1]);
					s->angle[2] = (int16_t)(s->angle[2] - s->angle_vel[2]);
					s->step++;
					s->radius = (int16_t)(radius - s->radius_vel);
				}
				BuildRotationMatrixFromAngles(s->angle, &h->shard);
				ComposeAffineTransform(&h->base, &h->shard, &h->shard);
				GteSetRotMatrixCtrl(&h->shard);
				GteSetTransVectorCtrl(&h->shard);
				int32_t level = (0x80 / s->steps) * s->step;
				*(uint32_t *)(p + 4) = (((uint32_t)level | 0x2600) << 16) | ((uint32_t)level << 8) | (uint32_t)level;
			}
			else
			{
				ff8fx::copy<(8)>(h->v0, a);
				ff8fx::copy<(8)>(h->v1, b);
				ff8fx::copy<(8)>(h->v2, c);
				GteSetRotMatrixCtrl(&h->base);
				GteSetTransVectorCtrl(&h->base);
				*(uint32_t *)(p + 4) = 0x24808080;
			}
			GteLoadV012(h->v0, h->v1, h->v2);
			GteRTPT();
			bool semi = (h->flags & 1) != 0;
			*(uint32_t *)p = 0x07000000;
			if (semi) *(uint32_t *)(p + 4) |= 0x02000000;
			*(uint32_t *)(p + 0xC) = *(const uint32_t *)(f + 0xC);
			*(uint32_t *)(p + 0x14) = *(const uint32_t *)(f + 0x10);
			*(uint32_t *)(p + 0x1C) = *(const uint32_t *)(f + 8) >> 16;
			GteReadFLAG(&h->gte_flag);
			if (h->gte_flag & 0x60000) continue;
			GteReadSXY012(p + 8, p + 0x10, p + 0x18);
			GteAVSZ3();
			GteReadOTZReg(&h->sin_otz);
			if (h->flags & 0x40)
			{
				GteSetFarColor(h->far_rgb[0], h->far_rgb[1], h->far_rgb[2]);
				GteLoadRGBC(p + 4);
				GteSetIR0(h->ir0);
				GteDPCS();
				GteStoreRGB2(p + 4);
			}
			int32_t z = h->sin_otz >> shift;
			InsertPrimAutoDepth(ot + 4 * z, p);
			if (s->step < s->steps)
			{
				p += 0x20;
				continue;
			}
			if (z > 0) z--;
			uint8_t *q = p + 0x20;
			*(uint32_t *)(q + 0xC) = *(const uint32_t *)(p + 8);
			*(uint32_t *)(q + 0x14) = *(const uint32_t *)(p + 0x10);
			*(uint32_t *)(q + 0x1C) = *(const uint32_t *)(p + 0x18);
			*(uint32_t *)(q + 8) = *(const uint32_t *)(p + 4) | 0x02000000;
			*(uint32_t *)q = 0x09000000;
			*(uint32_t *)(q + 0x10) = *(const uint32_t *)(uv + 0xC);
			*(uint32_t *)(q + 0x20) = *(const uint32_t *)(uv + 8) >> 16;
			*(uint32_t *)(q + 0x18) = *(const uint32_t *)(uv + 0x10);
			int32_t sc = h->scroll;
			if (sc != 0)
			{
				int32_t v0 = q[0x11] + sc, v1 = q[0x19] + sc, v2 = sc + q[0x21];
				h->v_scroll[0] = v0;
				h->v_scroll[1] = v1;
				h->v_scroll[2] = v2;
				if (v0 >= 0x100 || v1 >= 0x100 || v2 >= 0x100)
				{
					h->v_scroll[0] = v0 - tw_level;
					h->v_scroll[1] = v1 - tw_level;
					h->v_scroll[2] = v2 - tw_level;
				}
				else if (v0 < 0 || v1 < 0 || v2 < 0)
				{
					h->v_scroll[0] = v0 + tw_level;
					h->v_scroll[1] = v1 + tw_level;
					h->v_scroll[2] = v2 + tw_level;
				}
				q[0x11] = (uint8_t)h->v_scroll[0];
				q[0x19] = (uint8_t)h->v_scroll[1];
				q[0x21] = (uint8_t)h->v_scroll[2];
			}
			*(uint32_t *)(q + 4) = TextureWindow(h->tw0);
			*(uint32_t *)(q + 0x24) = TextureWindow(h->tw1);
			InsertPrimAutoDepth(ot + 4 * z, q);
			p = q + 0x28;
		}
		h->shards = (uint8_t *)s;
		h->faces = f;
		return (uint32_t)p;
	}

	static uint32_t CrystalRender(CrystalHeader *h, uint32_t ot, int shift, uint32_t cursor)
	{
		const uint8_t *model = h->model;
		h->tw1[1] = 0;
		h->tw1[0] = 0;
		h->tw1[3] = 0;
		h->tw1[2] = 0;
		h->tw0[0] = 0;
		h->tw0[1] = 0x80;
		h->tw0[2] = 0x80;
		h->tw0[3] = 0x80;
		h->verts = model + 8;
		h->scroll = h->scroll % 128;
		h->faces = model + *(const int32_t *)model + 8;
		h->uvs = h->uvmodel + *(const int32_t *)h->uvmodel + 8;
		return CrystalRenderCore(h, ot, shift, cursor);
	}

	static void CrystalMatrix(const CrystalNode *p, Mat4x3 *m)
	{
		int16_t angles[4] = { 0, p->yaw, 0, 0 };
		BuildRotationMatrixFromAngles(angles, m);
		m->t[0] = p->pos[0];
		m->t[1] = p->pos[1];
		m->t[2] = p->pos[2];
		int32_t sv[4] = { p->scale, p->scale, p->scale, 0 };
		Scale3DMatrix(m, sv);
	}

	static void CrystalHeaderInit(CrystalHeader *h, const CrystalNode *p, const Mat4x3 *m)
	{
		h->limit = p->counter;
		h->model = MODEL_Crystal();
		h->uvmodel = MODEL_CrystalUV();
		h->flags = 0;
		h->shards = ShardBuffer(p->action);
		ComposeAffineTransform(&Camera(), m, &h->base);
		h->scroll = p->counter * 8;
		if (p->counter >= 0x28)
		{
			h->flags |= 0xC3;
			h->far_rgb[2] = 0;
			h->far_rgb[1] = 0;
			h->far_rgb[0] = 0;
			h->ir0 = (p->counter - 0x28) << 9;
		}
	}

	static int16_t Div5(int16_t v)
	{
		int32_t x = v;
		int32_t q = (int32_t)(((int64_t)x * 0x66666667) >> 32) >> 2;
		q += (int32_t)((uint32_t)q >> 31);
		return (int16_t)q;
	}

	static void CrystalUpdate(CrystalNode *p)
	{
		int16_t c = p->counter;
		if (c < 0x1C)
		{
			int16_t v = p->yaw_vel;
			p->yaw = (int16_t)(p->yaw + v);
			p->yaw_vel = (int16_t)(v - Div5(v));
		}
		else if (c < 0x21)
		{
			int16_t v = p->lift;
			p->pos[1] = (int16_t)(p->pos[1] - v);
			p->lift = (int16_t)(v - Div5(v));
		}
		else if (c >= 0x23 && c < 0x25)
			p->pos[1] = (int16_t)(p->pos[1] - p->slam);
	}

	static uint32_t __cdecl CrystalTask(TaskNode *n)
	{
		CrystalNode *p = (CrystalNode *)n;
		Mat4x3 m = {};
		CrystalMatrix(p, &m);
		CrystalHeader *h = (CrystalHeader *)FieldAlloc(0xD0);
		CrystalHeaderInit(h, p, &m);
		PacketCursor() = CrystalRender(h, RenderOT44(), 2, PacketCursor());
		FieldFree(0xD0);
		if (DrawOnly()) return 0;
		CrystalUpdate(p);
		if (p->counter == 0x25) CameraShake() = (int16_t)(CameraShake() + 0x20);
		p->counter++;
		return p->counter >= 0x30 ? TASK_END : 0;
	}
}

	void register_mag144_blizzard()
	{
		register_port(blizzard144::ORIG_RootTask, (void *)blizzard144::RootTask, "B144 RootTask", 144);
		register_port(blizzard144::ORIG_Director, (void *)blizzard144::DirectorTask, "B144 Director", 144);
		register_port(blizzard144::ORIG_ShockwaveTask, (void *)blizzard144::ShockwaveTask, "B144 ShockwaveTask", 144);
		register_port(blizzard144::ORIG_DebrisTask, (void *)blizzard144::DebrisTask, "B144 DebrisTask", 144);
		register_port(blizzard144::ORIG_FallingIceTask, (void *)blizzard144::FallingIceTask, "B144 FallingIceTask", 144);
		register_port(blizzard144::ORIG_MistPuffTask, (void *)blizzard144::MistPuffTask, "B144 MistPuffTask", 144);
		register_port(blizzard144::ORIG_CrystalTask, (void *)blizzard144::CrystalTask, "B144 CrystalTask", 144);
	}
}

