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

// Effect 103: Blizzara

#include "mag_ice_common.h"

namespace ff8fx
{
namespace blizzara103
{
	using namespace eng;
	using namespace magc;
	using namespace icec;

	inline uint8_t *&TexBase() { return var<uint8_t *>(game_addr(game::MAG103_BLIZZARA_arenaBase)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::MAG103_BLIZZARA_castCtx)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_103_BLIZZARA_PacketCursor)); }
	inline TaskQueue *Queue(int i) { return (TaskQueue *)(game_addr(game::MAG103_BLIZZARA_Queues) + 0x10 * i); }

	static constexpr game::Id ORIG_Director = game::MAG_103_BLIZZARA_Director;
	static constexpr game::Id ORIG_IceBlockTask = game::MAG_103_BLIZZARA_IceBlockTask;
	static constexpr game::Id ORIG_PillarTask = game::MAG_103_BLIZZARA_PillarTask;
	static constexpr game::Id ORIG_BurstTask = game::MAG_103_BLIZZARA_BurstTask;
	static constexpr game::Id ORIG_ChunkTask = game::MAG_103_BLIZZARA_ChunkTask;
	static constexpr game::Id ORIG_MistTask = game::MAG_103_BLIZZARA_MistTask;
	static const uint8_t *MODEL_Block() { return (const uint8_t *)game_addr(game::unk_131ABB0); }
	static const uint8_t *MODEL_BlockUV() { return (const uint8_t *)game_addr(game::unk_131C86C); }
	static const void *MODEL_Pillar() { return (const void *)game_addr(game::unk_1319990); }
	static const void *MODEL_Burst() { return (const void *)game_addr(game::unk_131A678); }
	static const void *SEQ_Ice() { return (const void *)game_addr(game::unk_13197E4); }
	static const void *SOUND_Blizzara() { return (const void *)game_addr(game::unk_131E750); }
	static const uint32_t MODEL_OFFSET[3] = { 0x10BE4, 0x13A34, 0x16884 };
	static const uint32_t MODEL_UV_OFFSET[3] = { 0x128A0, 0x156F0, 0x18540 };
	static const int SHARDS = 0xDF;

	inline uint32_t RenderOT44() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44; }
	inline int16_t EntityHeight2(const uint8_t *entity) { return *(const int16_t *)(entity + 0x24); }

#pragma pack(push, 1)
	struct DirectorNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t action;
		uint8_t pools;
		uint32_t arena;
	};
	struct Shard
	{
		int16_t pos[3];
		int16_t active;
		int16_t vel[3];
		int16_t pad0E;
		int16_t angle[3];
		int16_t pad16;
		int16_t spin[3];
		int16_t pad1E;
		int16_t v[3][4];
		uint8_t fade[4][4];
	};
	struct IceBlockNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t action;
		uint8_t *entity;
		uint8_t *model;
		uint8_t *model2;
		int16_t pos[3];
		uint8_t pad22[4];
		int16_t clip_y;
		uint8_t used[4];
		Shard shards[SHARDS];
		int16_t verts[SHARDS * 3][4];
	};
	struct PillarNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		IceBlockNode *block;
		int16_t pos[3];
		int16_t rise;
		int16_t yaw;
		int16_t yaw_vel;
		int16_t pad20;
		int16_t scale;
		int16_t rise_acc;
		uint8_t stopped;
		uint8_t pad27;
	};
	struct BurstNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		int16_t pos[3];
		uint8_t pad16[4];
		int16_t scale;
		int16_t vel;
		int16_t acc;
	};
	struct ChunkNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		uint8_t *entity;
		int16_t pos[3];
		int16_t scale;
		int16_t vel[3];
		int16_t bounce;
	};
	struct MistNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		int16_t pos[3];
		int16_t scale;
		int16_t vel[3];
		int16_t pad1E;
		int16_t acc_x;
		int16_t pad22;
		int16_t acc_z;
		int16_t pad26;
	};
#pragma pack(pop)
	static_assert(sizeof(DirectorNode) == 0x14 && sizeof(Shard) == 0x48 && sizeof(IceBlockNode) == 0x53CC && sizeof(PillarNode) == 0x28
		&& sizeof(BurstNode) == 0x20 && sizeof(ChunkNode) == 0x24 && sizeof(MistNode) == 0x28, "Blizzara node layouts");
}
}

namespace ff8fx
{
namespace blizzara103
{
	static uint32_t __cdecl DirectorTask(TaskNode *n)
	{
		DirectorNode *d = (DirectorNode *)n;
		if (d->arena)
		{
			PacketCursor() = (uint32_t)(TexBase() + 0x196D4);
			d->arena = 0;
		}
		else
		{
			PacketCursor() = (uint32_t)(TexBase() + 0x296D4);
			d->arena = 1;
		}
		if (d->counter == 1)
		{
			if (!d->pools)
			{
				d->pools = 1;
				InitTaskQueuePool(Queue(0), TexBase(), 0x53CC, 3);
				InitTaskQueuePool(Queue(1), TexBase() + 0xFB64, 0x28, 0xC);
				InitTaskQueuePool(Queue(2), TexBase() + 0xFD44, 0x20, 1);
				InitTaskQueuePool(Queue(3), TexBase() + 0xFD64, 0x24, 0x20);
				InitTaskQueuePool(Queue(4), TexBase() + 0x101E4, 0x28, 0x40);
			}
			ActionData *acts = Ctx()->actions;
			if (d->action <= acts[0].last_action)
			{
				uint8_t *entity = Entity(acts[d->action].targets[0]);
				bool busy = false;
				uint8_t *slot = TexBase();
				for (int i = 0; i < 3; i++, slot += 0x53CC)
				{
					IceBlockNode *b = (IceBlockNode *)slot;
					if ((b->hdr.flags & 1) && b->entity == entity && b->counter <= 0x3C)
					{
						busy = true;
						break;
					}
				}
				if (busy) d->counter = 0;
				else
				{
					IceBlockNode *b = (IceBlockNode *)AddTaskToQueue(Queue(0), game_addr(ORIG_IceBlockTask));
					FillDwords(&b->counter, 0, 0x14F0);
					int16_t action = (int16_t)(uint16_t)d->action;
					b->action = action;
					b->entity = entity;
					b->clip_y = 0x7FFF;
					if (action >= 0 && action < 3)
					{
						CopyDwords(TexBase() + MODEL_OFFSET[action], MODEL_Block(), 0x72F);
						CopyDwords(TexBase() + MODEL_UV_OFFSET[action], MODEL_BlockUV(), 0x465);
						b->model = TexBase() + MODEL_OFFSET[action];
						b->model2 = TexBase() + MODEL_UV_OFFSET[action];
					}
					uint8_t *prim = ModelList(6, b->model, nullptr);
					uint8_t *uv = ModelList(2, b->model2, nullptr);
					for (int i = 0; i < SHARDS; i++, prim += 0x1C, uv += 0x14)
					{
						b->shards[i].active = 0;
						prim[0x17] = 0;
						prim[0x1B] = 0;
						uv[2] = 0xFF;
						uv[1] = 0xFF;
						uv[0] = 0xFF;
					}
					uint8_t *h = (uint8_t *)FieldAlloc(0x58);
					*(uint32_t *)(h + 0x1C) = 0xC;
					Cursor(h, 0) = b->model;
					PrepareModel(h, b->model, b->verts[0]);
					FieldFree(0x58);
					d->action++;
				}
			}
		}
		int r0, r1, r2, r3, r4;
		if (d->pools)
		{
			r0 = ExecuteTaskQueue(Queue(0));
			r1 = ExecuteTaskQueue(Queue(1));
			r2 = ExecuteTaskQueue(Queue(2));
			r3 = ExecuteTaskQueue(Queue(3));
			r4 = ExecuteTaskQueue(Queue(4));
		}
		else r0 = r1 = r2 = r3 = r4 = (int)(uint32_t)n;
		if (d->pools && !r0 && !r1 && !r2 && !r3 && !r4) return TASK_END;
		d->counter++;
		if (d->counter >= 0x14) d->counter = 0;
		return 0;
	}

	static uint32_t RenderPolys103(uint8_t *h, uint32_t ot, int shift, uint32_t cursor) { return RenderPolys(h, ot, shift, cursor, true); }
	static uint32_t RenderBlock(uint8_t *h, uint32_t ot, int shift, uint32_t cursor) { return RenderModel(h, ot, shift, cursor, RenderPolys103); }

	static bool FadeColor(uint8_t *c, const uint8_t *d)
	{
		bool gone = true;
		for (int k = 0; k < 3; k++)
		{
			int32_t v = (int32_t)c[k] - (int32_t)d[k];
			if (v > 0)
			{
				c[k] = (uint8_t)v;
				gone = false;
			}
			else c[k] = 0;
		}
		return gone;
	}

	static void PoseShard(const Shard *s, int16_t (*vbuf)[4], const uint8_t *prim)
	{
		Mat4x3 m = {};
		BuildRotationMatrixFromAngles(s->angle, &m);
		m.t[0] = s->pos[0];
		m.t[1] = s->pos[1];
		m.t[2] = s->pos[2];
		GteSetRotMatrixCtrl(&m);
		GteSetTransVectorCtrl(&m);
		for (int k = 0; k < 3; k++)
		{
			int16_t *v = vbuf[*(const uint16_t *)(prim + 4 + 2 * k) >> 1];
			GteLoadV0(s->v[k]);
			GteMVMVA_RotV0Tr();
			GteStoreIR123(v);
		}
	}

	static void UpdateShards(Shard *s, int16_t (*vbuf)[4], uint8_t *prim, uint8_t *ext)
	{
		for (int i = 0; i < SHARDS; i++, s++, prim += 0x1C, ext += 0x14)
		{
			if (!s->active) continue;
			if (FadeColor(prim, s->fade[0]) && FadeColor(prim + 0x14, s->fade[1]) && FadeColor(prim + 0x18, s->fade[2]))
			{
				s->active = 0;
				continue;
			}
			FadeColor(ext, s->fade[3]);
			int32_t g = (int32_t)prim[0x17] - 0x20;
			prim[0x17] = (uint8_t)(g > 0 ? g : 0);
			s->vel[1] = (int16_t)(s->vel[1] - 5);
			s->pos[2] = (int16_t)(s->pos[2] + s->vel[2]);
			s->pos[1] = (int16_t)(s->pos[1] + s->vel[1]);
			s->pos[0] = (int16_t)(s->pos[0] + s->vel[0]);
			s->angle[1] = (int16_t)(s->angle[1] + s->spin[1]);
			s->angle[0] = (int16_t)(s->angle[0] + s->spin[0]);
			s->angle[2] = (int16_t)(s->angle[2] + s->spin[2]);
			PoseShard(s, vbuf, prim);
		}
	}

	static void BlockMatrix(const IceBlockNode *b, int16_t scale_y)
	{
		Mat4x3 m = {};
		m.m[0][0] = -0x1000;
		m.m[1][1] = scale_y;
		m.m[2][2] = -0x1000;
		m.t[0] = b->pos[0];
		m.t[1] = b->pos[1];
		m.t[2] = b->pos[2];
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
	}

	static void BlockHeader(uint8_t *h, const IceBlockNode *b, int16_t counter, uint32_t flags)
	{
		Cursor(h, 0) = b->model;
		Cursor(h, 4) = b->model2;
		*(const void **)(h + 8) = b->verts;
		*(uint16_t *)(h + 0x1C) = (uint16_t)((counter & 0x1F) << 1);
		*(uint16_t *)(h + 0x2A) = 0;
		*(uint16_t *)(h + 0x1E) = (uint16_t)((counter & 0xF) << 3);
		*(uint16_t *)(h + 0x28) = 0;
		*(uint16_t *)(h + 0x22) = 0;
		*(uint16_t *)(h + 0x20) = 0;
		*(uint16_t *)(h + 0x26) = 0x100;
		*(uint16_t *)(h + 0x24) = 0x100;
		*(uint16_t *)(h + 0x2C) = 0x40;
		*(uint16_t *)(h + 0x2E) = 0x80;
		*(uint32_t *)(h + 0x18) = flags;
	}

	static int32_t Div3(int32_t x)
	{
		int32_t hi = (int32_t)(((int64_t)x * 0x55555556) >> 32);
		return hi + (int32_t)((uint32_t)hi >> 31);
	}

	static uint32_t __cdecl IceBlockTask(TaskNode *n)
	{
		IceBlockNode *b = (IceBlockNode *)n;
		if (b->counter == 0)
		{
			GetEffectSpawnPosition(b->entity, 0xF1, 0, b->pos);
			b->pos[1] = EntityHeight2(b->entity);
		}
		if (b->counter >= 0 && b->counter <= 0xC)
		{
			int32_t c = b->counter;
			if (c == 6) BdPlaySE3D(SOUND_Blizzara(), 0x101, b->pos);
			if (!(c & 3))
			{
				PillarNode *p = (PillarNode *)AddTaskToQueue(Queue(1), game_addr(ORIG_PillarTask));
				FillDwords(&p->counter, 0, 7);
				p->block = b;
				GetEffectSpawnPosition(b->entity, 0xF1, 0, p->pos);
				int32_t i = shl32(CrtRand(), 2) >> 15;
				while (b->used[i]) i = shl32(CrtRand(), 2) >> 15;
				b->used[i] = 1;
				p->yaw_vel = (i & 1) ? 0x1F : (int16_t)0xFFE1;
				p->pos[1] = (int16_t)(p->pos[1] + (int16_t)(((i << 10) - 0x5FF) >> 1));
				int32_t a = i < 0 ? -i : i;
				int32_t s = ComputeSin((a << 11) / 4 + 0x100);
				int32_t x = shl32(s, 10);
				int32_t hi = (int32_t)(((int64_t)x * 0x5F533929) >> 32) >> 12;
				hi += (int32_t)((uint32_t)hi >> 31);
				p->rise_acc = 0;
				p->scale = (int16_t)hi;
				p->rise = 6;
			}
		}
		if (b->counter == 0x14)
		{
			BurstNode *u = (BurstNode *)AddTaskToQueue(Queue(2), game_addr(ORIG_BurstTask));
			FillDwords(&u->counter, 0, 5);
			GetEffectSpawnPosition(b->entity, 0xF1, 0, u->pos);
			u->scale = 0x64;
			u->pos[1] = EntityHeight2(b->entity);
			u->vel = 0xC3;
			u->acc = (int16_t)0xFFF0;
		}
		int32_t grow;
		if (b->counter >= 0x14 && b->counter <= 0x1E)
		{
			int32_t t = b->counter - 0x14;
			uint8_t *h = (uint8_t *)FieldAlloc(0x64);
			int16_t sy;
			if (t < 3)
			{
				sy = (int16_t)Div3(t << 12);
				grow = sy;
			}
			else
			{
				sy = 0x1000;
				grow = 0x1000;
			}
			BlockMatrix(b, sy);
			BlockHeader(h, b, b->counter, 0x6600C);
			PacketCursor() = RenderBlock(h, RenderOT44(), 2, PacketCursor());
			FieldFree(0x64);
			int32_t x = grow * 3400;
			int32_t q = (int32_t)(((int64_t)x * 0x66666667) >> 32) >> 2;
			q += (int32_t)((uint32_t)q >> 31);
			b->clip_y = (int16_t)(EntityHeight2(b->entity) - (int16_t)q);
		}
		else grow = 0x1000;
		if (b->counter >= 0x1E && b->counter <= 0x46)
		{
			int32_t t = b->counter - 0x1E;
			uint8_t *prims = ModelList(6, b->model, nullptr);
			uint8_t *uvs = ModelList(2, b->model2, nullptr);
			if (t <= 0x14)
			{
				int32_t x = t * 3400;
				int32_t q = (int32_t)(((int64_t)x * 0x66666667) >> 32) >> 3;
				int32_t level = q + (int32_t)((uint32_t)q >> 31) - 3400;
				Shard *s = b->shards;
				uint8_t *prim = prims, *uv = uvs;
				for (int i = 0; i < SHARDS; i++, s++, prim += 0x1C, uv += 0x14)
				{
					if (s->active) continue;
					int16_t *va = b->verts[*(uint16_t *)(prim + 4) >> 1];
					int16_t *vb = b->verts[*(uint16_t *)(prim + 6) >> 1];
					int16_t *vc = b->verts[*(uint16_t *)(prim + 8) >> 1];
					if (va[1] > level || vb[1] > level || vc[1] > level) continue;
					int16_t cx = (int16_t)Div3(va[0] + vc[0] + vb[0]);
					s->pos[0] = cx;
					int16_t cy = (int16_t)Div3(vc[1] + vb[1] + va[1]);
					s->pos[1] = cy;
					int16_t cz = (int16_t)Div3(va[2] + vc[2] + vb[2]);
					s->active = 1;
					s->pos[2] = cz;
					s->vel[2] = (int16_t)(-(cz * cy) >> 15);
					s->vel[0] = (int16_t)(-(cy * cx) >> 15);
					s->vel[1] = 0;
					s->angle[2] = 0;
					s->angle[1] = 0;
					s->angle[0] = 0;
					s->spin[0] = (int16_t)((CrtRand() - 0x4000) >> 7);
					s->spin[1] = (int16_t)((CrtRand() - 0x4000) >> 7);
					s->spin[2] = (int16_t)((CrtRand() - 0x4000) >> 7);
					*(uint32_t *)prim |= 0x02000000;
					prim[0x17] = 0xFF;
					prim[0x1B] = 0;
					s->fade[0][0] = (uint8_t)((prim[0] + 15) >> 3);
					s->fade[0][1] = (uint8_t)((prim[1] + 15) >> 3);
					s->fade[0][2] = (uint8_t)((prim[2] + 15) >> 3);
					s->fade[1][0] = (uint8_t)((prim[0x14] + 15) >> 3);
					s->fade[1][1] = (uint8_t)((prim[0x15] + 15) >> 3);
					s->fade[1][2] = (uint8_t)((prim[0x16] + 15) >> 3);
					s->fade[2][0] = (uint8_t)((prim[0x18] + 15) >> 3);
					s->fade[2][1] = (uint8_t)((prim[0x19] + 15) >> 3);
					s->fade[2][2] = (uint8_t)((prim[0x1A] + 15) >> 3);
					s->fade[3][0] = (uint8_t)((uv[0] + 15) >> 2);
					s->fade[3][1] = (uint8_t)((uv[1] + 15) >> 2);
					s->fade[3][2] = (uint8_t)((uv[2] + 15) >> 2);
					s->v[0][0] = (int16_t)(va[0] - cx);
					s->v[0][1] = (int16_t)(va[1] - cy);
					s->v[0][2] = (int16_t)(va[2] - cz);
					s->v[1][0] = (int16_t)(vb[0] - cx);
					s->v[1][1] = (int16_t)(vb[1] - cy);
					s->v[1][2] = (int16_t)(vb[2] - cz);
					s->v[2][0] = (int16_t)(vc[0] - cx);
					s->v[2][1] = (int16_t)(vc[1] - cy);
					s->v[2][2] = (int16_t)(vc[2] - cz);
				}
			}
			UpdateShards(b->shards, b->verts, prims, uvs);
			uint8_t *h = (uint8_t *)FieldAlloc(0x64);
			BlockMatrix(b, 0x1000);
			BlockHeader(h, b, b->counter, 0x660C0);
			PacketCursor() = RenderBlock(h, RenderOT44(), 2, PacketCursor());
			FieldFree(0x64);
		}
		if (b->counter >= 0x14 && b->counter <= 0x1E)
		{
			int32_t angle = CrtRand();
			for (int k = 0; k < 2; k++)
			{
				ChunkNode *c = (ChunkNode *)AddTaskToQueue(Queue(3), game_addr(ORIG_ChunkTask));
				if (!c) continue;
				FillDwords(&c->counter, 0, 6);
				c->entity = b->entity;
				int32_t r1 = CrtRand();
				int32_t r2 = CrtRand();
				int32_t a = ((r1 + r2 - 0x8000) >> 8) + angle;
				angle += 0x800;
				c->pos[0] = (int16_t)(ComputeCos(a) >> 3);
				c->pos[2] = (int16_t)(ComputeSin(a) >> 3);
				c->pos[1] = 0;
				c->scale = (int16_t)(0x1000 - (CrtRand() >> 4));
				NormalizeVector16(c->pos, c->vel);
				c->pos[0] = (int16_t)(c->pos[0] + b->pos[0]);
				c->pos[2] = (int16_t)(c->pos[2] + b->pos[2]);
				int16_t y = (int16_t)((CrtRand() >> 3) - (grow >> 1) - 0x400);
				c->pos[1] = y;
				if (y > -1000) c->pos[1] = -1000;
				c->vel[1] = 0x20;
				c->pos[1] = (int16_t)(c->pos[1] + EntityHeight2(b->entity));
				c->bounce = -1;
				c->vel[0] = (int16_t)(c->vel[0] >> 6);
				c->vel[2] = (int16_t)(c->vel[2] >> 6);
			}
		}
		if (b->counter >= 0x14 && b->counter <= 0x28)
		{
			int32_t angle = CrtRand();
			for (int k = 0; k < 4; k++)
			{
				MistNode *m = (MistNode *)AddTaskToQueue(Queue(4), game_addr(ORIG_MistTask));
				if (!m) continue;
				FillDwords(&m->counter, 0, 7);
				int32_t r1 = CrtRand();
				int32_t r2 = CrtRand();
				int32_t a = ((r1 + r2 - 0x8000) >> 8) + angle;
				angle += 0x400;
				m->pos[0] = (int16_t)(ComputeCos(a) >> 4);
				m->pos[2] = (int16_t)(ComputeSin(a) >> 4);
				m->pos[1] = 0;
				m->scale = (int16_t)(0x1000 - (CrtRand() >> 4));
				NormalizeVector16(m->pos, m->vel);
				m->pos[0] = (int16_t)(m->pos[0] + b->pos[0]);
				m->pos[2] = (int16_t)(m->pos[2] + b->pos[2]);
				m->pos[1] = b->pos[1];
				int32_t x = m->vel[0];
				int32_t q = (int32_t)(((int64_t)x * 0x2AAAAAAB) >> 32) >> 2;
				int16_t vx = (int16_t)(q + (int32_t)((uint32_t)q >> 31));
				m->vel[0] = vx;
				x = m->vel[2];
				q = (int32_t)(((int64_t)x * 0x2AAAAAAB) >> 32) >> 2;
				int16_t vz = (int16_t)(q + (int32_t)((uint32_t)q >> 31));
				m->vel[2] = vz;
				x = vx;
				q = ((int32_t)(((int64_t)x * 0x6DB6DB6D) >> 32) - x) >> 3;
				m->acc_x = (int16_t)(q + (int32_t)((uint32_t)q >> 31));
				x = vz;
				q = ((int32_t)(((int64_t)x * 0x6DB6DB6D) >> 32) - x) >> 3;
				m->acc_z = (int16_t)(q + (int32_t)((uint32_t)q >> 31));
			}
		}
		if (b->counter == 0x3C) ApplyActionResultToTarget(Ctx()->actions[b->action].targets);
		if (b->counter >= 0x46) return TASK_END;
		b->counter++;
		return 0;
	}

	static int32_t Div6(int32_t x)
	{
		int32_t hi = (int32_t)(((int64_t)x * 0x2AAAAAAB) >> 32);
		return hi + (int32_t)((uint32_t)hi >> 31);
	}

	static void YawScaleMatrix(int16_t yaw, Mat4x3 *m, int16_t s)
	{
		int32_t sn = mul32(ComputeSin(yaw), s) >> 12;
		int32_t cs = mul32(ComputeCos(yaw), s) >> 12;
		m->m[0][2] = (int16_t)sn;
		m->m[2][0] = (int16_t)-sn;
		m->m[1][1] = s;
		m->m[0][0] = (int16_t)cs;
		m->m[0][1] = 0;
		m->m[1][0] = 0;
		m->m[1][2] = 0;
		m->m[2][1] = 0;
		m->m[2][2] = (int16_t)cs;
	}

	static int32_t PillarFade(int16_t c)
	{
		if (c < 6) return 0x1000 - ComputeSin(Div6(c << 10));
		if (c >= 0x12)
		{
			int32_t q = (int32_t)(((int64_t)((c - 0x12) << 10) * 0x2AAAAAAB) >> 32) >> 1;
			return ComputeSin(q + (int32_t)((uint32_t)q >> 31));
		}
		return 0;
	}

	static void PillarDraw(const PillarNode *p, uint8_t *h)
	{
		Mat4x3 m = {};
		YawScaleMatrix(p->yaw, &m, p->scale);
		m.t[0] = p->pos[0];
		m.t[1] = p->pos[1];
		m.t[2] = p->pos[2];
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		*(const void **)h = MODEL_Pillar();
		h[0xA] = 0;
		h[9] = 0;
		h[8] = 0;
		*(uint32_t *)(h + 0x1C) = 0xF3;
		PacketCursor() = RenderPrimModel(h, RenderOT44(), 2, PacketCursor());
	}

	static void PillarUpdate(PillarNode *p)
	{
		if (!p->stopped && p->block->clip_y <= p->pos[1])
		{
			p->rise = 0xF0;
			p->rise_acc = (int16_t)0xFF80;
			p->stopped = 1;
		}
		p->rise_acc = (int16_t)(p->rise_acc + 8);
		if (p->rise_acc > 0) p->rise_acc = 0;
		p->rise = (int16_t)(p->rise + p->rise_acc);
		if (p->rise < 5) p->rise = 5;
		p->scale = (int16_t)(p->scale + p->rise);
	}

	static uint32_t __cdecl PillarTask(TaskNode *n)
	{
		PillarNode *p = (PillarNode *)n;
		uint8_t *h = AllocHeader(0x58);
		*(int32_t *)(h + 0xC) = 0;
		*(int32_t *)(h + 0xC) = PillarFade(p->counter);
		if (*(int32_t *)(h + 0xC) < 0x1000) PillarDraw(p, h);
		FieldFree(0x58);
		p->yaw = (int16_t)(p->yaw + p->yaw_vel);
		p->counter++;
		if (p->counter >= 0x1E) return TASK_END;
		PillarUpdate(p);
		return 0;
	}

	static int32_t BurstFade(int16_t c)
	{
		if (c < 6) return 0x1000 - ComputeSin(Div6(c << 10));
		if (c >= 0xE) return ComputeSin(Div6((c - 0xE) << 10));
		return 0;
	}

	static void BurstDraw(const BurstNode *u, uint8_t *h)
	{
		Mat4x3 m = {};
		m.m[0][0] = u->scale;
		m.m[1][1] = u->scale;
		m.m[2][2] = u->scale;
		m.t[0] = u->pos[0];
		m.t[1] = u->pos[1];
		m.t[2] = u->pos[2];
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		*(const void **)h = MODEL_Burst();
		h[0xA] = 0;
		h[9] = 0;
		h[8] = 0;
		*(uint32_t *)(h + 0x1C) = 0xF3;
		PacketCursor() = RenderPrimModel(h, RenderOT44(), 2, PacketCursor());
	}

	static void BurstUpdate(BurstNode *u)
	{
		u->vel = (int16_t)(u->vel + u->acc);
		if (u->vel < 0x1E) u->vel = 0x1E;
		u->scale = (int16_t)(u->scale + u->vel);
	}

	static uint32_t __cdecl BurstTask(TaskNode *n)
	{
		BurstNode *u = (BurstNode *)n;
		uint8_t *h = AllocHeader(0x58);
		*(int32_t *)(h + 0xC) = 0;
		*(int32_t *)(h + 0xC) = BurstFade(u->counter);
		if (*(int32_t *)(h + 0xC) < 0x1000) BurstDraw(u, h);
		FieldFree(0x58);
		u->counter++;
		if (u->counter >= 0x14) return TASK_END;
		BurstUpdate(u);
		return 0;
	}

	static void ChunkDraw(const ChunkNode *c)
	{
		uint8_t *h = AllocHeader(0xB4);
		TransformCameraByShadowRotation(c->pos, c->scale, -(c->scale >> 2));
		*(uint16_t *)(h + 0x24) = 0;
		*(const void **)h = SEQ_Ice();
		*(uint16_t *)(h + 4) = (uint16_t)(c->counter & 0xF);
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT44(), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static void ChunkUpdate(ChunkNode *c)
	{
		int16_t vy = c->vel[1];
		if (c->pos[1] < EntityHeight2(c->entity) - vy * 4) c->vel[1] = (int16_t)(vy + 4);
		else if (vy != 0)
		{
			if (c->bounce == -1) c->bounce = (int16_t)Div6(vy);
			int16_t d[3] = { c->vel[0], 0, c->vel[2] };
			c->vel[1] = 0;
			NormalizeVector16(d, d);
			c->vel[0] = (int16_t)(c->vel[0] + (int16_t)((d[0] * c->bounce) >> 10));
			c->vel[2] = (int16_t)(c->vel[2] + (int16_t)((d[2] * c->bounce) >> 10));
		}
		c->pos[1] = (int16_t)(c->pos[1] + c->vel[1]);
		c->pos[2] = (int16_t)(c->pos[2] + (int16_t)(c->vel[2] >> 2));
		c->pos[0] = (int16_t)(c->pos[0] + (int16_t)(c->vel[0] >> 2));
	}

	static uint32_t __cdecl ChunkTask(TaskNode *n)
	{
		ChunkNode *c = (ChunkNode *)n;
		ChunkDraw(c);
		c->counter++;
		if (c->counter >= 0x10) return TASK_END;
		ChunkUpdate(c);
		return 0;
	}

	static void MistDraw(const MistNode *m)
	{
		uint8_t *h = AllocHeader(0xB4);
		TransformCameraByShadowRotation(m->pos, m->scale, -(m->scale >> 2));
		*(uint16_t *)(h + 0x24) = 0;
		*(const void **)h = SEQ_Ice();
		*(uint16_t *)(h + 4) = (uint16_t)(m->counter & 0xF);
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT44(), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static void MistUpdate(MistNode *m)
	{
		m->vel[2] = (int16_t)(m->vel[2] + m->acc_z);
		m->vel[0] = (int16_t)(m->vel[0] + m->acc_x);
		m->pos[2] = (int16_t)(m->pos[2] + m->vel[2]);
		m->pos[0] = (int16_t)(m->pos[0] + m->vel[0]);
	}

	static uint32_t __cdecl MistTask(TaskNode *n)
	{
		MistNode *m = (MistNode *)n;
		MistDraw(m);
		m->counter++;
		if (m->counter >= 0x10) return TASK_END;
		MistUpdate(m);
		return 0;
	}
}

	void register_mag103_blizzara()
	{
		register_port(blizzara103::ORIG_Director, (void *)blizzara103::DirectorTask, "B103 Director", 103);
		register_port(blizzara103::ORIG_IceBlockTask, (void *)blizzara103::IceBlockTask, "B103 IceBlockTask", 103);
		register_port(blizzara103::ORIG_PillarTask, (void *)blizzara103::PillarTask, "B103 PillarTask", 103);
		register_port(blizzara103::ORIG_BurstTask, (void *)blizzara103::BurstTask, "B103 BurstTask", 103);
		register_port(blizzara103::ORIG_ChunkTask, (void *)blizzara103::ChunkTask, "B103 ChunkTask", 103);
		register_port(blizzara103::ORIG_MistTask, (void *)blizzara103::MistTask, "B103 MistTask", 103);
	}
}

