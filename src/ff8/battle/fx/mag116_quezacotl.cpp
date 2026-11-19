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

// Effect 116: Quezacotl - Thunder Storm

#include "fx_port.h"

namespace ff8fx
{
namespace q116
{
	using namespace eng;

	inline uint32_t &Pause() { return var<uint32_t>(game_addr(game::MAG_116_Pause)); }
	inline TaskQueuePair &QueueDebris() { return var<TaskQueuePair>(game_addr(game::MAG_116_QueueDebris)); }
	inline TaskQueuePair &QueueBolts() { return var<TaskQueuePair>(game_addr(game::MAG_116_QueueBolts)); }
	inline TaskQueuePair &QueueCreature() { return var<TaskQueuePair>(game_addr(game::GF116_QUEZACOTL_queueCreature)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::GF116_QUEZACOTL_packetCursor)); }
	inline Mat4x3 &BoltFrame() { return var<Mat4x3>(game_addr(game::MAG_116_BoltFrame)); }
	inline uint32_t &ScreenFlash() { return var<uint32_t>(game_addr(game::GF116_QUEZACOTL_screenFlashLevel)); }
	inline uint8_t &DoneFlag() { return var<uint8_t>(game_addr(game::GF116_QUEZACOTL_doneFlag)); }
	inline uint8_t *&ModelBuffer() { return var<uint8_t *>(game_addr(game::GF116_QUEZACOTL_modelBuffer)); }
	inline Mat4x3 &EffectCamera() { return var<Mat4x3>(game_addr(game::MAG_116_EffectCamera)); }

	static constexpr game::Id ORIG_SequenceTask = game::GF_116Quezacotl_SequenceTask;
	static constexpr game::Id ORIG_CreatureTask = game::GF_116Quezacotl_CreatureTask;
	static constexpr game::Id ORIG_DebrisTask = game::GF116_Quezacotl_DebrisTask;
	static const void *SOUND_CreatureVoice() { return (const void *)game_addr(game::unk_12AE500); }
	static void *MODEL_Debris() { return (void *)game_addr(game::unk_12AC8B8); }

	struct MasterNode
	{
		TaskNode hdr;
		uint16_t counter;
		uint8_t pad0E;
		uint8_t spawned;
		uint32_t parity;
	};

	static uint32_t __cdecl SequenceTask(TaskNode *n)
	{
		MasterNode *node = (MasterNode *)n;
		if (node->parity)
		{
			PacketCursor() = (uint32_t)(ModelBuffer() + 0x2E50);
			node->parity = 0;
		}
		else
		{
			PacketCursor() = (uint32_t)(ModelBuffer() + 0x15E50);
			node->parity = 1;
		}
		ScreenFlash() = 0;

		if (node->counter == 2 && !Pause() && !node->spawned)
		{
			node->spawned = 1;
			InitTaskQueuePool(&QueueCreature().second, ModelBuffer(), 0x8D0, 1);
			TaskNode *creature = AddTaskToQueue(&QueueCreature().second, game_addr(ORIG_CreatureTask));
			Memset32((uint8_t *)creature + 0x0C, 0, 0x231);
			*(uint32_t *)((uint8_t *)creature + 0x10) = ClaimVoiceSlot(SOUND_CreatureVoice(), 1, 0x80);
			StreamStateInit(ModelBuffer() + 0x28E54);
			*(uint32_t *)(ModelBuffer() + 0x28E50) = 1;
		}

		int creature_left = 1;
		if (node->spawned)
		{
			EffectCameraMatrix(&Camera(), &EffectCamera());
			creature_left = ExecuteTaskQueue(&QueueCreature().second);
			if (QueueCreature().first.head)
			{
				ComputeBonesWorldMatrices(ModelBuffer() + 0x88, ModelBuffer() + 0x68);
				ExecuteTaskQueue(&QueueCreature().first);
				BuildBoneMatricesFromPose(ModelBuffer() + 0x88);
			}
			GteSetRotMatrix(&Camera());
			GteSetTransVector(&Camera());
			ExecuteTaskQueue(&QueueBolts().second);
			GteSetRotMatrix(&BoltFrame());
			GteSetTransVector(&BoltFrame());
			ExecuteTaskQueue(&QueueBolts().first);
			ExecuteTaskQueue(&QueueDebris().second);
			ExecuteTaskQueue(&QueueDebris().first);
		}

		SetScreenFlash(ScreenFlash(), 0);
		if (Pause()) return 0;
		if (node->spawned && creature_left == 0)
			return DoneFlag() ? TASK_END : 0;
		node->counter++;
		return 0;
	}

	struct DebrisNode
	{
		TaskNode hdr;
		int16_t age;
		int16_t pad0E;
		int16_t pos[3];
		int16_t pad16;
		int16_t vel[3];
		int16_t pad1E;
		int16_t rot[3];
		int16_t pad26;
		int8_t spin[3];
		int8_t pad2B;
	};
	static_assert(sizeof(DebrisNode) == 0x2C, "debris node is 0x2C bytes");

	static void DebrisDraw(const int16_t pos[3], const int16_t rot[3])
	{
		uint8_t *header = (uint8_t *)FieldAlloc(0x58);
		Mat4x3 m;
		BuildRotationMatrixFromAngles(rot, &m);
		m.t[0] = pos[0];
		m.t[1] = pos[1];
		m.t[2] = pos[2];
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		*(void **)header = MODEL_Debris();
		*(uint32_t *)(header + 0x1C) = 0;
		PacketCursor() = RenderPrimModel(header, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static void DebrisUpdate(DebrisNode *p)
	{
		p->rot[0] += p->spin[0];
		p->rot[2] += p->spin[2];
		p->vel[1] += 10;
		p->rot[1] += p->spin[1];
		p->pos[0] += p->vel[0];
		p->pos[1] += p->vel[1];
		p->pos[2] += p->vel[2];
		if (p->pos[1] > 0)
		{
			p->pos[1] = 0;
			p->vel[1] = -p->vel[1];
		}
	}

	static uint32_t __cdecl DebrisTask(TaskNode *n)
	{
		DebrisNode *p = (DebrisNode *)n;
		DebrisDraw(p->pos, p->rot);
		if (Pause()) return 0;
		DebrisUpdate(p);
		p->age++;
		return p->age < 30 ? 0 : TASK_END;
	}

	static int32_t Rand()
	{
		uint32_t &seed = var<uint32_t>(game_addr(game::GF116_QUEZACOTL_randSeed));
		seed = (seed * 125 + 14) & 0x7FFF;
		return (int32_t)seed;
	}

	struct BoltSeg
	{
		int16_t vertex;
		int16_t width;
		int16_t sx, sy;
		int16_t otz;
		int16_t pad0A;
		int16_t sx2, sy2;
		int16_t pad10[2];
		int16_t intensity;
		int16_t decay;
		BoltSeg *next;
	};
	static_assert(sizeof(BoltSeg) == 28, "bolt segment is 28 bytes");

	struct BranchNode
	{
		TaskNode hdr;
		int16_t count;
		int16_t width;
		int16_t intensity;
		int16_t decay;
		const int16_t *script;
		BoltSeg *head, *tail;
	};
	static_assert(sizeof(BranchNode) == 0x20, "branch node is 0x20 bytes");

	static constexpr game::Id ORIG_BranchTask = game::GF116_Quezacotl_BranchTask;
	static const uint8_t *BOLT_VERTICES() { return (const uint8_t *)game_addr(game::GF116_BoltVertexTable); }
	static const uint8_t *DIST_TABLE() { return (const uint8_t *)game_addr(game::GF116_DistTable); }

	inline BoltSeg *&SegFreeHint() { return *(BoltSeg **)(ModelBuffer() + 0x43A00); }
	inline BoltSeg *SegPool() { return (BoltSeg *)(ModelBuffer() + 0x40200); }

	static BoltSeg *SegAlloc(int16_t vertex, int16_t width, int16_t intensity, int16_t decay)
	{
		BoltSeg *s = SegFreeHint();
		if (s) SegFreeHint() = nullptr;
		else
		{
			s = SegPool();
			int i = 0;
			while (i < 512 && (uint16_t)s->vertex != 0xFFFF) { s++; i++; }
			if (i == 512) return nullptr;
		}
		s->next = nullptr;
		s->vertex = vertex;
		s->width = width;
		s->intensity = intensity;
		s->decay = decay;
		return s;
	}

	static void SpawnBranch(const int16_t *script, int16_t width)
	{
		int32_t r1 = Rand() & 0x3FF;
		int32_t r2 = Rand() & 0x1FF;
		int16_t intensity = (int16_t)(r1 + r2 + 0x100);
		int16_t decay = (int16_t)-(intensity >> 3);
		BoltSeg *s = SegAlloc(script[0], width, intensity, decay);
		if (!s) return;
		BranchNode *b = (BranchNode *)AddTaskToQueue(&QueueBolts().first, game_addr(ORIG_BranchTask));
		if (!b) return;
		Memset32(&b->count, 0, 5);
		b->tail = s;
		b->head = s;
		b->intensity = intensity;
		b->decay = decay;
		b->script = script + 1;
		b->width = width;
		b->count = 1;
	}

	template<typename Seg, int BIAS>
	static void SegEdges(Seg *s, const Seg *n, const int16_t *prev_dir, int16_t *dir)
	{
		if (s->otz < 0) return;
		int32_t ox = 0, oy = 0;
		if (n)
		{
			if (n->otz < 0) return;
			int32_t dx = ScreenPx(n->sx) - ScreenPx(s->sx);
			int32_t ndy = ScreenPx(s->sy) - ScreenPx(n->sy);
			int32_t adx = dx < 0 ? -dx : dx, ady = ndy < 0 ? -ndy : ndy;
			if (adx >= 0x80) adx = 0x7F;
			if (ady >= 0x80) ady = 0x7F;
			int32_t len = DIST_TABLE()[ady * 128 + adx];
			int32_t px, py;
			if (prev_dir)
			{
				if (len == 0)
				{
					dir[0] = prev_dir[0];
					dir[1] = prev_dir[1];
					px = prev_dir[0];
					py = prev_dir[1];
				}
				else
				{
					int32_t cx = shl32(ndy, 12) / len, cy = shl32(dx, 12) / len;
					int32_t sx = cx + prev_dir[0], sy = cy + prev_dir[1];
					dir[0] = (int16_t)cx;
					dir[1] = (int16_t)cy;
					if (sx == 0 && sy == 0) { sx = cx * 2; sy = cy * 2; }
					int32_t sq = (int32_t)((uint32_t)mul32(sx, sx) + (uint32_t)mul32(sy, sy));
					int32_t k = 0x4000 - (sq >> 13);
					px = mul32(k, sx) >> 13;
					py = mul32(k, sy) >> 13;
				}
			}
			else
			{
				if (len == 0) { s->otz = -1; return; }
				px = shl32(ndy, 12) / len;
				py = shl32(dx, 12) / len;
				dir[0] = (int16_t)px;
				dir[1] = (int16_t)py;
			}
			int32_t w = shl32(n->width, 5) / (s->otz + BIAS);
			ox = ScreenEighths(mul32(w, px) >> 12);
			oy = ScreenEighths(mul32(w, py) >> 12);
		}
		int16_t x = s->sx, y = s->sy;
		s->sx = (int16_t)(x - ox);
		s->sx2 = (int16_t)(x + ox);
		s->sy2 = (int16_t)(y + oy);
		s->sy = (int16_t)(y - oy);
	}

	static int16_t g_dir_branch[2], g_dir_bolt[2], g_dir_arc[2];

	template<typename Seg, int BIAS>
	static void DrawRibbon(Seg *head, int count, int16_t *carry)
	{
		int16_t dir[2] = { carry[0], carry[1] };
		SegEdges<Seg, BIAS>(head, head->next, nullptr, dir);
		Seg *s = head->next;
		for (int i = count - 2; i > 0; i--)
		{
			SegEdges<Seg, BIAS>(s, s->next, dir, dir);
			s = s->next;
		}
		SegEdges<Seg, BIAS>(s, nullptr, dir, nullptr);

		Seg *a = head;
		for (int i = count - 1; i > 0; i--)
		{
			Seg *b = a->next;
			if (a->otz > 0 && b->otz > 0)
			{
				uint32_t *p = (uint32_t *)PacketCursor();
				PacketCursor() += 0x34;
				p[2] = *(uint32_t *)&a->sx;
				p[5] = *(uint32_t *)&a->sx2;
				p[3] = 0x3E94C008;
				p[6] = 0x00B6C038;
				*(uint16_t *)((uint8_t *)p + 0x24) = 0xC008;
				*(uint16_t *)((uint8_t *)p + 0x30) = 0xC038;
				p[8] = *(uint32_t *)&b->sx;
				p[11] = *(uint32_t *)&b->sx2;
				uint32_t ca = (uint32_t)(int32_t)(int16_t)(a->intensity >> 4);
				ca = (((ca << 8) | ca) << 8) | ca;
				p[4] = ca;
				p[1] = ca;
				uint32_t cb = (uint32_t)(int32_t)(int16_t)(b->intensity >> 4);
				cb = (((cb << 8) | cb) << 8) | cb;
				p[0] = 0x0C000000;
				((uint8_t *)p)[7] = 0x3E;
				p[10] = cb;
				p[7] = cb;
				InsertPrimAutoDepth(var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44 + 4 * ((a->otz + b->otz) >> 3), p);
			}
			a = b;
		}
		carry[0] = dir[0];
		carry[1] = dir[1];
	}

	static void BranchDraw(BoltSeg *head, int count)
	{
		for (BoltSeg *s = head; s; s = s->next)
		{
			GteLoadV0(BOLT_VERTICES() + 8 * (int32_t)s->vertex);
			GteRTPS();
			GteReadSXY2(&s->sx);
			GteReadOTZ(&s->otz);
		}
		DrawRibbon<BoltSeg, 0x400>(head, count, g_dir_branch);
	}

	static void BranchFreeAll(BranchNode *b)
	{
		SegFreeHint() = b->head;
		for (BoltSeg *s = b->head; s; s = s->next) s->vertex = -1;
	}

	static uint32_t __cdecl BranchTask(TaskNode *n)
	{
		BranchNode *b = (BranchNode *)n;
		if (!Pause())
		{
			for (int iter = 4; iter; iter--)
			{
				if (!b->script) continue;
				int16_t v = *b->script++;
				while (v < 0)
				{
					SpawnBranch(b->script, b->width);
					g_dir_branch[0] = 2;
					g_dir_branch[1] = 0;
					b->script += -1 - v;
					v = *b->script++;
				}
				const int16_t *peek = b->script;
				int16_t c = *peek;
				if (c < 0) { peek -= c; c = *peek; }
				if (c == 0x7FFF)
				{
					if (b->count <= 3) { BranchFreeAll(b); return TASK_END; }
					b->script = nullptr;
				}
				int32_t w = b->width;
				int16_t intensity = b->intensity, decay = b->decay;
				int16_t width = (int16_t)(w + ((Rand() * w) >> 15));
				g_dir_branch[0] = (int16_t)0x78B8;
				g_dir_branch[1] = 0x006C;
				BoltSeg *s = SegAlloc(v, width, intensity, decay);
				if (!s) { BranchFreeAll(b); return TASK_END; }
				b->tail->next = s;
				b->count++;
				b->tail = s;
			}
			for (BoltSeg *s = b->head; s; s = s->next)
				if (s->intensity > 0)
				{
					s->intensity += s->decay;
					if (s->intensity <= 0) s->intensity = 0;
				}
		}
		if (b->count >= 2) BranchDraw(b->head, b->count);
		if (Pause()) return 0;
		BoltSeg *s = b->head;
		if (s->intensity > 0x20) return 0;
		for (;;)
		{
			if (s->next->intensity > 0x20) return 0;
			SegFreeHint() = s;
			s->vertex = -1;
			s = s->next;
			b->count--;
			b->head = s;
			if (b->count < 2) return TASK_END;
			if (s->intensity > 0x20) return 0;
		}
	}

	struct BoltVert
	{
		int16_t pos[3];
		int16_t width;
		int16_t vel[3];
		int16_t hold;
		int16_t sx, sy;
		int16_t otz;
		int16_t pad16;
		int16_t sx2, sy2;
		int16_t pad1C[2];
		int16_t intensity;
		int16_t decay;
		BoltVert *next;
	};
	static_assert(sizeof(BoltVert) == 0x28, "bolt vertex is 0x28 bytes");

	struct BoltNode
	{
		TaskNode hdr;
		int16_t unk0C;
		int16_t count;
		BoltVert *head, *tail;
		int16_t pos[3];
		int16_t t;
		int16_t target[3];
		int16_t dt;
		int16_t vel[3];
		int16_t budget;
		int16_t shape[3];
		int16_t unk36;
		int16_t width;
		int16_t kind;
	};
	static_assert(sizeof(BoltNode) == 0x3C, "bolt node is 0x3C bytes");

	struct FlashNode
	{
		TaskNode hdr;
		int16_t pos[3];
		int16_t age;
	};
	static_assert(sizeof(FlashNode) == 0x14, "flash node is 0x14 bytes");

	static constexpr game::Id ORIG_BoltTask = game::GF116_Quezacotl_BoltTask;
	static constexpr game::Id ORIG_FlashTask = game::GF116_Quezacotl_FlashTask;
	static const int16_t *SIN_TABLE() { return (const int16_t *)game_addr(game::GF116_SinTable); }
	static void *SEQ_ImpactFlash() { return (void *)game_addr(game::unk_12AE400); }

	inline BoltVert *&VertFreeHint() { return *(BoltVert **)(ModelBuffer() + 0x3F1FC); }
	inline BoltVert *VertPool() { return (BoltVert *)(ModelBuffer() + 0x3C9FC); }

	static BoltVert *VertAlloc(const int16_t *pos, int16_t width)
	{
		BoltVert *v = VertFreeHint();
		if (v) VertFreeHint() = nullptr;
		else
		{
			v = VertPool();
			int i = 0;
			while (i < 256 && v->width != -1) { v++; i++; }
			if (i == 256) return nullptr;
		}
		ff8fx::copy<(8)>(v->pos, pos);
		v->width = width;
		v->hold = 2;
		v->intensity = 0x200;
		v->decay = 0x300;
		v->next = nullptr;
		return v;
	}

	static BoltVert *SpawnBolt(const int16_t *vel, const int16_t *pos, const int16_t *target, const int16_t *shape,
		int16_t unk36, int16_t t, int16_t dt, int16_t width, int16_t kind, int16_t budget)
	{
		BoltVert *v = VertAlloc(pos, width);
		if (!v) return nullptr;
		BoltNode *b = (BoltNode *)AddTaskToQueue(&QueueBolts().second, game_addr(ORIG_BoltTask));
		if (!b) { v->width = -1; return nullptr; }
		b->unk0C = 0;
		ff8fx::copy<(8)>(b->pos, pos);
		ff8fx::copy<(8)>(b->target, target);
		ff8fx::copy<(8)>(b->vel, vel);
		b->t = t;
		b->dt = dt;
		b->budget = budget;
		b->tail = v;
		b->head = v;
		b->count = 1;
		if (!shape)
		{
			v->vel[2] = 0;
			v->vel[1] = 0;
			v->vel[0] = 0;
			b->shape[0] = (int16_t)((Rand() & 0xFF) - 0x80);
			b->shape[1] = (int16_t)((Rand() & 0xFF) - 0x80);
			b->shape[2] = (int16_t)((Rand() & 0xFF) - 0x80);
		}
		else ff8fx::copy<(8)>(b->shape, shape);
		b->unk36 = unk36;
		b->width = width;
		b->kind = kind;
		return v;
	}

	static int16_t ForkOffset(int16_t base)
	{
		int32_t o = Rand() > 0x4000 ? -0x200 - (Rand() & 0x1FF) : (Rand() & 0x1FF) + 0x200;
		return (int16_t)(o + base);
	}

	static uint32_t __cdecl BoltTask(TaskNode *n)
	{
		BoltNode *b = (BoltNode *)n;
		if (!Pause())
		{
			for (int iter = 8; iter; iter--)
			{
				if (b->t >= 0x1000) continue;
				b->pos[0] += b->vel[0];
				b->t = (int16_t)(b->t + b->dt);
				b->pos[1] += b->vel[1];
				b->pos[2] += b->vel[2];
				b->vel[0] += (int16_t)((Rand() & 0x7F) - 0x40);
				b->vel[1] += (int16_t)((Rand() & 0x7F) - 0x40);
				b->vel[2] += (int16_t)((Rand() & 0x7F) - 0x40);
				if (Rand() < 0x1000)
				{
					b->pos[0] += (int16_t)((Rand() & 0x1FF) - 0x100);
					b->pos[1] += (int16_t)((Rand() & 0x1FF) - 0x100);
					b->pos[2] += (int16_t)((Rand() & 0x1FF) - 0x100);
				}
				int16_t at[4];
				if (b->t >= 0x1000)
				{
					ff8fx::copy<(8)>(at, b->target);
					b->t = 0x1000;
					if (b->kind != 0 && *(uint32_t *)(ModelBuffer() + 0x28E50) != 0)
					{
						FlashNode *f = (FlashNode *)AddTaskToQueue(&QueueDebris().second, game_addr(ORIG_FlashTask));
						if (f)
						{
							ff8fx::copy<(8)>(f->pos, at);
							f->age = 0;
						}
						if (b->kind == 1)
							for (int k = 3; k; k--)
							{
								int16_t p[4], v[4];
								p[0] = (int16_t)((Rand() & 0x7FF) + at[0] - 0x400);
								p[1] = (int16_t)(at[1] - (Rand() & 0x3FF) - 0x800);
								p[2] = (int16_t)((Rand() & 0x7FF) + at[2] - 0x400);
								v[0] = (int16_t)((Rand() & 0x7F) - 0x40);
								v[1] = (int16_t)(-0x40 - (Rand() & 0x3F));
								v[2] = (int16_t)((Rand() & 0x7F) - 0x40);
								SpawnBolt(v, at, p, nullptr, 0, 0, 0x100, 0xFA, 0, 0);
							}
					}
				}
				else
				{
					GteSetIR0(0x1000 - b->t);
					GteLoadIR123(b->pos);
					GteGPF();
					GteSetIR0(b->t);
					GteLoadIR123(b->target);
					GteGPL();
					GteStoreIR123(at);
				}
				BoltVert *v = VertAlloc(at, b->width);
				if (!v) { b->t = 0x1000; continue; }
				b->tail->next = v;
				b->tail = v;
				GteSetIR0(SIN_TABLE()[(b->t >> 1) & 0xFFF]);
				GteLoadIR123(b->shape);
				GteGPF();
				b->count++;
				GteStoreIR123(v->vel);
				if (b->t >= 0xC00) continue;
				if (Rand() >= shl32(b->budget, 13)) continue;
				int16_t p[4], nv[4];
				if (b->kind != 0)
				{
					p[0] = (int16_t)((Rand() & 0xFFF) + b->target[0] - 0x800);
					p[1] = (int16_t)(b->target[1] - (Rand() & 0x7FF) - 0x100);
					p[2] = (int16_t)(b->target[2] - (Rand() & 0x1FF) - 0x100);
				}
				else
				{
					p[0] = ForkOffset(at[0]);
					p[1] = ForkOffset(at[1]);
					p[2] = ForkOffset(at[2]);
				}
				nv[0] = (int16_t)((Rand() & 0x1FF) + b->vel[0] - 0x100);
				nv[1] = (int16_t)((Rand() & 0x1FF) + b->vel[1] - 0x100);
				nv[2] = (int16_t)((Rand() & 0x1FF) + b->vel[2] - 0x100);
				BoltVert *c = SpawnBolt(nv, at, p, b->shape, -1, b->t, 0x200, (int16_t)(b->width >> 1), b->kind == 1 ? 2 : 0, 0);
				if (c) ff8fx::copy<(8)>(c->vel, v->vel);
				b->budget--;
			}
			for (BoltVert *v = b->head; v; v = v->next)
			{
				if (v->intensity <= 0) continue;
				v->intensity += v->decay;
				if (v->intensity <= 0) { v->intensity = 0; continue; }
				if (v->hold != 0 && v->decay <= 0) { v->hold--; continue; }
				if (v->decay > -0x180) v->decay -= 0x140;
			}
		}
		for (BoltVert *v = b->head; v; v = v->next)
		{
			GteLoadV0(v->pos);
			GteRTPS();
			GteReadSXY2(&v->sx);
			GteReadOTZ(&v->otz);
		}
		if (b->count >= 2) DrawRibbon<BoltVert, 0x200>(b->head, b->count, g_dir_bolt);
		if (Pause()) return 0;
		for (BoltVert *v = b->head; v; v = v->next)
		{
			v->pos[0] += v->vel[0];
			v->pos[1] += v->vel[1];
			v->pos[2] += v->vel[2];
		}
		BoltVert *v = b->head;
		if (v->intensity != 0) return 0;
		for (;;)
		{
			BoltVert *nx = v->next;
			if (!nx || nx->intensity != 0) return 0;
			VertFreeHint() = v;
			v->width = -1;
			v = nx;
			b->count--;
			b->head = v;
			if (b->count < 2) return TASK_END;
			if (v->intensity != 0) return 0;
		}
	}

	static uint32_t __cdecl FlashTask(TaskNode *n)
	{
		FlashNode *f = (FlashNode *)n;
		TransformCameraByShadowRotation(f->pos, 0x800, -0x200);
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		*(void **)h = SEQ_ImpactFlash();
		*(int16_t *)(h + 4) = f->age;
		*(int16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
		FieldFree(0xB4);
		if (Pause()) return 0;
		f->age++;
		return f->age < 8 ? 0 : TASK_END;
	}

	struct ArcPoint { int16_t width, color, pad; };

	struct ArcNode
	{
		TaskNode hdr;
		int16_t age;
		int16_t progress;
		int16_t speed;
		uint8_t reach_shift;
		uint8_t fade_shift;
		const int16_t *strips;
		const uint8_t *faces;
		uint8_t **bones;
		ArcPoint pts[16];
	};
	static_assert(sizeof(ArcNode) == 0x80, "arc node is 0x80 bytes");

	struct ArcSeg
	{
		int16_t sx, sy;
		int16_t otz;
		int16_t width;
		int16_t sx2, sy2;
		int16_t padC;
		int16_t color;
	};
	static_assert(sizeof(ArcSeg) == 16, "arc point is 16 bytes");

	static constexpr game::Id ORIG_ArcTask = game::GF116_Quezacotl_ArcTask;
	static const uint16_t *SQRT_TABLE() { return (const uint16_t *)game_addr(game::GF116_SqrtTable); }

	static int32_t SqrtLz(int32_t v)
	{
		int32_t lz;
		GteSetLZCS(v);
		GteReadLZCR(&lz);
		int32_t e = lz & ~1;
		int32_t sh = (0x1F - e) >> 1;
		int32_t idx = e - 0x18 > 0 ? shl32(v, e - 0x18) : v >> (0x18 - e);
		return shl32(SQRT_TABLE()[idx], sh);
	}

	static int32_t Div15(int32_t x)
	{
		int32_t hi = (int32_t)(((int64_t)x * (int32_t)0x88888889) >> 32);
		int32_t d = (hi + x) >> 3;
		return d + (int32_t)((uint32_t)d >> 31);
	}

	static void TriangleFrame(const uint8_t *bone_mats, const uint8_t *face, int16_t *pos, int16_t *normal)
	{
		int16_t w[3][4];
		for (int k = 0; k < 3; k++)
		{
			const int16_t *v = (const int16_t *)(face + 8 * k);
			const Mat4x3 *m = (const Mat4x3 *)(bone_mats + 0x10 + 48 * (int32_t)v[3]);
			GteSetRotMatrixCtrl(m);
			GteSetTransVectorCtrl(m);
			GteLoadV0(v);
			GteMVMVA_RotV0Tr();
			GteStoreIR123(w[k]);
		}
		int32_t e1[3] = { w[0][0] - w[1][0], w[0][1] - w[1][1], w[0][2] - w[1][2] };
		int32_t e2[3] = { w[2][0] - w[0][0], w[2][1] - w[0][1], w[2][2] - w[0][2] };
		int32_t n[3];
		GteSetRotDiagonal(e1);
		GteLoadIR123_32(e2);
		GteOP();
		GteReadMAC123(n);
		int32_t m = n[0] < 0 ? -n[0] : n[0];
		int32_t a = n[1] < 0 ? -n[1] : n[1];
		if (a > m) m = a;
		a = n[2] < 0 ? -n[2] : n[2];
		if (a > m) m = a;
		GteSetLZCS(m);
		for (int k = 0; k < 3; k++)
		{
			int32_t c = w[2][k] + w[1][k] + w[0][k];
			pos[k] = (int16_t)(mul32(c, 0x5555) >> 16);
		}
		int32_t lz;
		GteReadLZCR(&lz);
		if (lz < 0x12)
		{
			int sh = 0x12 - lz;
			n[0] >>= sh;
			n[1] >>= sh;
			n[2] >>= sh;
		}
		int32_t len2;
		GteLoadIR123_32b(n);
		GteSQR();
		GteSumMAC123(&len2);
		int32_t len = SqrtLz(len2) >> 12;
		normal[0] = (int16_t)(shl32(n[0], 12) / len);
		normal[1] = (int16_t)(shl32(n[1], 12) / len);
		normal[2] = (int16_t)(shl32(n[2], 12) / len);
	}

	static void StripPoint(const uint8_t *bone_mats, const uint8_t *faces, int32_t n, int32_t progress, int16_t *pos, int16_t *normal)
	{
		int32_t at = mul32(n - 1, progress);
		int32_t frac = at & 0xFFF;
		const uint8_t *face = faces + 24 * (at >> 12);
		if (frac == 0)
		{
			TriangleFrame(bone_mats, face, pos, normal);
			return;
		}
		int16_t p1[4], p2[4], n1[4], n2[4], nl[4];
		TriangleFrame(bone_mats, face, p1, n1);
		TriangleFrame(bone_mats, face + 24, p2, n2);
		int32_t inv = 0x1000 - frac;
		GteSetIR0(inv);
		GteLoadIR123(p1);
		GteGPF();
		GteSetIR0(frac);
		GteLoadIR123(p2);
		GteGPL();
		GteStoreIR123(pos);
		GteSetIR0(inv);
		GteLoadIR123(n1);
		GteGPF();
		GteSetIR0(frac);
		GteLoadIR123(n2);
		GteGPL();
		GteStoreIR123(nl);
		int32_t len2;
		GteLoadIR123(nl);
		GteSQR();
		GteSumMAC123(&len2);
		int32_t len = SqrtLz(len2) >> 12;
		normal[0] = (int16_t)(shl32(nl[0], 12) / len);
		normal[1] = (int16_t)(shl32(nl[1], 12) / len);
		normal[2] = (int16_t)(shl32(nl[2], 12) / len);
	}

	static void ArcDraw(ArcSeg *pts, int16_t *carry)
	{
		GteSetRotMatrixCtrl(&Camera());
		GteSetTransVectorCtrl(&Camera());
		for (int k = 0; k < 16; k++)
		{
			int32_t sz3;
			GteLoadV0(&pts[k]);
			GteRTPS();
			GteReadDataReg(14, &pts[k]);
			GteReadDataRegTo(&sz3, 19);
			pts[k].otz = (int16_t)(sz3 >> 2);
		}
		int16_t dir[2] = { carry[0], carry[1] };
		SegEdges<ArcSeg, 0x400>(&pts[0], &pts[1], nullptr, dir);
		for (int k = 1; k < 15; k++)
			SegEdges<ArcSeg, 0x400>(&pts[k], &pts[k + 1], dir, dir);
		SegEdges<ArcSeg, 0x400>(&pts[15], nullptr, dir, nullptr);
		carry[0] = dir[0];
		carry[1] = dir[1];
		for (int k = 0; k < 15; k++)
		{
			const ArcSeg *a = &pts[k], *b = &pts[k + 1];
			if (a->otz <= 0 || b->otz <= 0) continue;
			uint32_t *p = (uint32_t *)var<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR));
			var<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) += 0x34;
			p[2] = *(const uint32_t *)&a->sx;
			p[5] = *(const uint32_t *)&a->sx2;
			p[3] = 0x3E94C008;
			p[6] = 0x00B6C038;
			*(uint16_t *)((uint8_t *)p + 0x24) = 0xC008;
			*(uint16_t *)((uint8_t *)p + 0x30) = 0xC038;
			p[8] = *(const uint32_t *)&b->sx;
			p[11] = *(const uint32_t *)&b->sx2;
			uint32_t ca = (uint32_t)(int32_t)a->color;
			ca = (((ca << 8) | ca) << 8) | ca;
			p[4] = ca;
			p[1] = ca;
			uint32_t cb = (uint32_t)(int32_t)b->color;
			cb = (((cb << 8) | cb) << 8) | cb;
			p[0] = 0x0C000000;
			((uint8_t *)p)[7] = 0x3E;
			p[10] = cb;
			p[7] = cb;
			InsertPrimAutoDepth(var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44 + 4 * ((b->otz + a->otz) >> 5), p);
		}
	}

	static ArcSeg *ArcCurve(ArcNode *arc, int16_t progress, int32_t *length)
	{
		const uint8_t *bone_mats = *arc->bones + 0x10;
		int16_t p1[4], n1[4], p2[4], n2[4];
		StripPoint(bone_mats, arc->faces + 24 * (int32_t)arc->strips[0], arc->strips[1], progress, p1, n1);
		StripPoint(bone_mats, arc->faces + 24 * (int32_t)arc->strips[2], arc->strips[3], progress, p2, n2);
		int32_t d[3] = { p2[0] - p1[0], p2[1] - p1[1], p2[2] - p1[2] };
		GteLoadIR123_32b(d);
		GteSQR();
		GteReadMAC123(d);
		int32_t r = SqrtLz(d[2] + d[1] + d[0]);
		*length = r >> 12;
		GteSetIR0(r >> arc->reach_shift);
		GteLoadIR123(n1);
		GteGPF();
		GteStoreIR123(n1);
		GteLoadIR123(n2);
		GteGPF();
		GteStoreIR123(n2);
		for (int k = 0; k < 3; k++)
		{
			n1[k] += p1[k];
			n2[k] += p2[k];
		}
		ArcSeg *pts = (ArcSeg *)FieldAlloc(0x100);
		ff8fx::copy<(8)>(&pts[0], p1);
		pts[0].width = arc->pts[0].width;
		pts[0].color = arc->pts[0].color;
		ff8fx::copy<(8)>(&pts[15], p2);
		pts[15].width = arc->pts[15].width;
		pts[15].color = arc->pts[15].color;
		for (int i = 1; i < 15; i++)
		{
			int32_t t = Div15(shl32(i, 12)), u = 0x1000 - t;
			int16_t q1[4], q2[4];
			GteSetIR0(u);
			GteLoadIR123(p1);
			GteGPF();
			GteSetIR0(t);
			GteLoadIR123(n1);
			GteGPL();
			GteStoreIR123(q1);
			GteSetIR0(t);
			GteLoadIR123(p2);
			GteGPF();
			GteSetIR0(u);
			GteLoadIR123(n2);
			GteGPL();
			GteStoreIR123(q2);
			GteSetIR0(u);
			GteLoadIR123(q1);
			GteGPF();
			GteSetIR0(t);
			GteLoadIR123(q2);
			GteGPL();
			GteStoreIR123(&pts[i]);
			pts[i].width = arc->pts[i].width;
			pts[i].color = arc->pts[i].color;
		}
		return pts;
	}

	static void ArcBuildDraw(ArcNode *arc)
	{
		int32_t length;
		ArcSeg *pts = ArcCurve(arc, arc->progress, &length);
		const int16_t *profile = (const int16_t *)(ModelBuffer() + 0x3C25C);
		for (int i = 1; i < 15; i++)
		{
			int32_t amp = mul32(profile[i], length) >> 12;
			int32_t rv[3];
			rv[0] = Rand() - 0x4000;
			pts[i].sx += (int16_t)(mul32(rv[0], amp) >> 18);
			rv[1] = Rand() - 0x4000;
			pts[i].sy += (int16_t)(mul32(rv[1], amp) >> 18);
			rv[2] = Rand() - 0x4000;
			pts[i].otz += (int16_t)(mul32(rv[2], amp) >> 18);
		}
		ArcDraw(pts, g_dir_arc);
		FieldFree(0x100);
	}

	static uint32_t __cdecl ArcTask(TaskNode *n)
	{
		ArcNode *arc = (ArcNode *)n;
		ArcBuildDraw(arc);
		if (Pause()) return 0;
		const int16_t *profile = (const int16_t *)(ModelBuffer() + 0x3C25C);
		arc->progress += arc->speed;
		if (arc->progress <= 0x1000) return 0;
		arc->progress = 0x1000;
		for (int k = 0; k < 16; k++)
		{
			int32_t dec = (0x1400 - profile[k]) >> arc->fade_shift;
			arc->pts[k].color -= (int16_t)dec;
			if (arc->pts[k].color < 0) arc->pts[k].color = 0;
		}
		arc->age++;
		return arc->age < 10 ? 0 : TASK_END;
	}

	namespace cx
	{
		inline void Sub508500() { fn<void (__cdecl *)()>(game_addr(game::BattleFile_GetLoadingState))(); }
		inline void RotMatrixY(int32_t angle, void *m) { fn<void (__cdecl *)(int32_t, void *)>(game_addr(game::Effect_RotationYMatrix))(angle, m); }
		inline void BindModel(void *m, void *sections, void *data) { fn<void (__cdecl *)(void *, void *, void *)>(game_addr(game::Effect_BindModelContainerSections))(m, sections, data); }
		inline void SetAnim(void *m, int id) { fn<void (__cdecl *)(void *, int)>(game_addr(game::au_re_Battle_ReadAnimation_6))(m, id); }
		inline void AdvanceAnim(void *m) { fn<void (__cdecl *)(void *)>(game_addr(game::MAG_089_Angelo_AnimStep))(m); }
		inline void DrawMaterialise(void *m, int32_t a2, const void *a3, int32_t a4, int32_t a5, int32_t a6, const void *a7)
		{ fn<void (__cdecl *)(void *, int32_t, const void *, int32_t, int32_t, int32_t, const void *)>(game_addr(game::MAG_116_DrawCreatureMaterialise))(m, a2, a3, a4, a5, a6, a7); }
		inline void MatVec(const void *m, const void *v, void *out) { fn<void (__cdecl *)(const void *, const void *, void *)>(game_addr(game::matrixMultiplyVector))(m, v, out); }
		inline uint32_t DrawModel(void *m, void *buf, uint32_t cursor, uint32_t a4) { return fn<uint32_t (__cdecl *)(void *, void *, uint32_t, uint32_t)>(game_addr(game::Effect_DrawCreatureModel))(m, buf, cursor, a4); }
		inline void Decode(uint32_t src, void *layout, int n) { fn<void (__cdecl *)(uint32_t, void *, int)>(game_addr(game::Effect_DecodeModelPrimLayout))(src, layout, n); }
		inline void Sub6C6500(int a) { fn<void (__cdecl *)(int)>(game_addr(game::MAG_116_SetStageGroupsBit1))(a); }
		inline void Sub6C6530(uint32_t p) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::MAG_116_DrawFullScreenTile))(p); }
		inline void Sub6C65A0(uint32_t p) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::sub_6C65A0))(p); }
		inline void VertexPos(void *m, int a, int b, void *out) { fn<void (__cdecl *)(void *, int, int, void *)>(game_addr(game::MAG_116_CreatureVertexPos))(m, a, b, out); }
		inline void RotMatrixXY(int a, int b, void *out) { fn<void (__cdecl *)(int, int, void *)>(game_addr(game::MAG_116_RotMatrixXY))(a, b, out); }
		inline void SpawnArcO(uint32_t bones, uint32_t strips, uint32_t faces, int a4, int a5, int a6) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t, int, int, int)>(game_addr(game::GF116_Quezacotl_SpawnArc))(bones, strips, faces, a4, a5, a6); }
		inline void SpawnBoltO(void *vel, void *pos, void *target, int shape, int unk, int t, int dt, int width, int kind, int budget)
		{ fn<void (__cdecl *)(void *, void *, void *, int, int, int, int, int, int, int)>(game_addr(game::GF116_Quezacotl_SpawnBolt))(vel, pos, target, shape, unk, t, dt, width, kind, budget); }
		inline void Sub6C76C0(uint32_t p, int a) { fn<void (__cdecl *)(uint32_t, int)>(game_addr(game::GF116_Quezacotl_SpawnBranchFromTable))(p, a); }
		inline void Sub6C7CC0() { fn<void (__cdecl *)()>(game_addr(game::GF116_Quezacotl_InitBoltVertPool))(); }
		inline void Sub6C7690() { fn<void (__cdecl *)()>(game_addr(game::MAG_116_ResetVertexCache))(); }
		inline void Blend(void *a, void *b, int wa, int wb, void *out) { fn<void (__cdecl *)(void *, void *, int, int, void *)>(game_addr(game::VecLerp))(a, b, wa, wb, out); }
		inline int LoadBusy() { return fn<int (__cdecl *)()>(game_addr(game::Stream_IsBusy))(); }
		inline void Load(int id, uint32_t dst, int n) { fn<void (__cdecl *)(int, uint32_t, int)>(game_addr(game::Stream_QueueLoad))(id, dst, n); }
		inline void LoadPump() { fn<void (__cdecl *)()>(game_addr(game::Stream_Pump))(); }
		inline void ReleaseVoice(uint32_t slot) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::BdSound_ReleaseVoiceSlot))(slot); }
		inline void PlaySE(uint32_t se, int a, int b) { fn<void (__cdecl *)(uint32_t, int, int)>(game_addr(game::BdPlaySE))(se, a, b); }
		inline void PlayStream(int a, int b, int c) { fn<void (__cdecl *)(int, int, int)>(game_addr(game::BdPlaySummonStream))(a, b, c); }
		inline void ApplyResult(uint32_t targets, int n) { fn<void (__cdecl *)(uint32_t, int)>(game_addr(game::ApplyActionResultToTargets))(targets, n); }
		inline uint8_t *SummonData() { return fn<uint8_t *(__cdecl *)()>(game_addr(game::sub_571B70))(); }
		inline void Sub508630(void *a, void *b) { fn<void (__cdecl *)(void *, void *)>(game_addr(game::Battle_RestoreDefaultEffectTexture))(a, b); }
		inline int32_t SharedRand() { return fn<int32_t (__cdecl *)()>(game_addr(game::Effect_SharedLcg_Rand))(); }
		inline void Sub6C5300() { fn<void (__cdecl *)()>(game_addr(game::MAG_116_HideTargets))(); }
		inline void Sub6C5350() { fn<void (__cdecl *)()>(game_addr(game::MAG_116_ShowTargets))(); }
		inline void BuildBoneMatrices(void *header) { fn<void (__cdecl *)(void *)>(game_addr(game::BattleModel_BuildBoneMatricesFromPose))(header); }
	}

	static constexpr game::Id CB_PrimDraw = game::MAG_116_PrimDrawCallback;
	static constexpr game::Id CB_PrimMatrix = game::MAG_116_PrimMatrixCallback;
	inline int16_t &G16(uint32_t a) { return var<int16_t>(a); }
	inline int32_t &G32(uint32_t a) { return var<int32_t>(a); }
	static constexpr game::Id CAM_EYE = game::Battle_Camera_world_XZ_s16, CAM_AT = game::Battle_Camera_LookAt_XZ_s16;
	static constexpr game::Id BASE_Y = game::GF116_QUEZACOTL_targetCentroidX, BASE_Y2 = game::GF116_baseY2, BASE_Y3 = game::GF116_baseY3;

	static uint8_t g_fr[0x90];
	inline int16_t &F16(int o) { return *(int16_t *)(g_fr + o); }
	inline int32_t &F32(int o) { return *(int32_t *)(g_fr + o); }
	inline uint8_t *FP(int o) { return g_fr + o; }

	static int Play(prim::Layout *l, uint32_t cb, void *arg)
	{
		return prim::play(l, (prim::Callback)cb, (int)arg, Pause());
	}

	static uint32_t __cdecl CreatureTask(TaskNode *n)
	{
		uint8_t *nd = (uint8_t *)n;
		auto W16 = [nd](int o) -> int16_t & { return *(int16_t *)(nd + o); };
		auto D32 = [nd](int o) -> int32_t & { return *(int32_t *)(nd + o); };
		uint8_t *model = nd + 0x28;
		prim::Layout *L = (prim::Layout *)(nd + 0xD4);
		uint8_t *MB = ModelBuffer();

		int16_t c = W16(0x0C);
		if (c <= 351)
		{
			if ((int32_t)ScreenFlash() < 2400) ScreenFlash() = 2400;
		}
		else
		{
			int32_t v = ((355 - (int32_t)c) * 2400) >> 2;
			if (v > (int32_t)ScreenFlash()) ScreenFlash() = (uint32_t)v;
		}

		uint32_t v3 = (uint32_t)((int32_t)c - 86);
		if (v3 < 0xAB)
		{
			int32_t v4;
			if (v3 == 0)
			{
				cx::Sub508500();
				cx::RotMatrixY(0, nd + 0x68);
				D32(0x80) = 0; W16(0x46) = 0; D32(0x7C) = 0; W16(0x44) = 0;
				D32(0x84) = G32(game_addr(BASE_Y3));
				W16(0x48) = G16(game_addr(BASE_Y3));
				W16(0x36) = 0; W16(0x38) = 0; W16(0x34) = 0;
				cx::BindModel(model, nd + 0xC4, MB + 0x3169C);
				cx::SetAnim(model, 0);
				D32(0x14) = 0;
				v4 = (int32_t)(-3500 * (int32_t)v3) / 50;
			}
			else if (v3 == 0x20) { cx::SetAnim(model, 1); v4 = (int32_t)(-3500 * (int32_t)v3) / 50; }
			else if (v3 == 0x98) { cx::SetAnim(model, 2); v4 = -4096; }
			else v4 = v3 < 0x32 ? (int32_t)(-3500 * (int32_t)v3) / 50 : -4096;
			F16(0x18) = 0x1C0; F16(0x1A) = 0x180; F16(0x1C) = 0x80; F16(0x1E) = 0x80;
			g_fr[0x10] = g_fr[0x11] = g_fr[0x12] = 0x80; g_fr[0x13] = 0;
			if (!Pause()) cx::AdvanceAnim(model);
			cx::DrawMaterialise(model, v4, FP(0x18), 0x3F54, D32(0x14) >> 6, D32(0x14) >> 5, FP(0x10));
			if (!Pause()) D32(0x14) += (int32_t)v3;
		}

		uint32_t v5 = (uint32_t)((int32_t)W16(0x0C) - 257);
		if (v5 < 0x14)
		{
			if (!Pause())
			{
				F16(0x18) = G16(game_addr(CAM_EYE));
				F16(0x1A) = G16(game_addr(CAM_EYE) + 2);
				F16(0x1C) = (int16_t)(G32(game_addr(CAM_EYE) + 4) - G32(game_addr(BASE_Y)));
				int32_t s = ComputeSin((int32_t)((v5 << 12) / 40));
				cx::RotMatrixY(s >> 7, FP(0x44));
				cx::MatVec(FP(0x44), FP(0x18), (void *)game_addr(CAM_EYE));
				G16(game_addr(game::g_BattleCam_Roll)) += 8;
				G16(game_addr(CAM_EYE) + 4) += G16(game_addr(BASE_Y));
				cx::AdvanceAnim(model);
			}
			var<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = cx::DrawModel(model, MB + 0x8D0, var<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)), var<uint32_t>(game_addr(game::BATTLE_PACKET_BUFFER_END)));
		}

		if ((uint32_t)((int32_t)W16(0x0C) - 1) < 0x4C)
		{
			if (W16(0x0C) == 1)
			{
				cx::Decode(game_addr(game::unk_1298C68), L, 0x7D4);
				cx::Sub6C6500(0);
			}
			F32(0x40) = (int32_t)(MB + 0x8D0);
			F32(0x38) = 0; F16(0x3C) = 0; F16(0x34) = 0; F16(0x30) = 0; F16(0x32) = -5000;
			Play(L, game_addr(CB_PrimDraw), FP(0x30));
		}

		if ((uint32_t)((int32_t)W16(0x0C) - 77) < 0x18)
		{
			if (W16(0x0C) == 77)
			{
				cx::Decode(game_addr(game::unk_12A9664), L, 0x7FC);
				cx::Sub6C6500(1);
			}
			F32(0x8C) = (int32_t)(MB + 0x8D0);
			F32(0x84) = 0; F16(0x88) = 0;
			F32(0x78) = 500; F32(0x7C) = 0; F32(0x80) = G32(game_addr(BASE_Y3));
			cx::RotMatrixY(-2048, FP(0x64));
			ComposeAffineTransform(&Camera(), (Mat4x3 *)FP(0x64), (Mat4x3 *)FP(0x64));
			Play(L, game_addr(CB_PrimMatrix), FP(0x64));
		}

		uint32_t v7 = (uint32_t)((int32_t)W16(0x0C) - 86);
		if (v7 < 0x4C)
		{
			if (v7 == 0)
			{
				for (uint32_t i = 0x3C25C; i < 0x3C27C; i += 2)
				{
					int16_t s = (int16_t)ComputeSin((int32_t)(v7 << 11) / 15);
					v7++;
					*(int16_t *)(ModelBuffer() + i) = s;
				}
				InitTaskQueuePool(&QueueCreature().first, ModelBuffer() + 0x3BA5C, 0x80, 0x10);
				InitTaskQueuePool(&QueueBolts().second, ModelBuffer() + 0x3C27C, 0x3C, 0x20);
				InitTaskQueuePool(&QueueBolts().first, ModelBuffer() + 0x3F200, 0x20, 0x80);
				cx::Sub6C7CC0();
				cx::Sub6C7690();
				D32(0x18) = G32(game_addr(CAM_AT));
				D32(0x1C) = G32(game_addr(CAM_AT) + 4);
				D32(0x20) = G32(game_addr(CAM_EYE));
				D32(0x24) = G32(game_addr(CAM_EYE) + 4);
			}
			else if (!Pause())
			{
				int32_t v10 = (int32_t)((v7 << 12) / 76);
				W16(0x1A) -= 17;
				W16(0x22) -= 5;
				W16(0x24) -= G16(game_addr(BASE_Y3));
				int32_t s = ComputeSin(v10 >> 1);
				cx::RotMatrixY(s / 90, FP(0x44));
				cx::MatVec(FP(0x44), nd + 0x20, nd + 0x20);
				W16(0x24) += G16(game_addr(BASE_Y3));
				if (v7 == 31)
				{
					G16(game_addr(CAM_AT)) = (int16_t)0xEBF9; G16(game_addr(CAM_AT) + 2) = (int16_t)0xF9BA; G16(game_addr(CAM_EYE)) = 0x5FB;
					G16(game_addr(CAM_AT) + 4) = (int16_t)(G32(game_addr(BASE_Y3)) + 0x606);
					G16(game_addr(CAM_EYE) + 2) = (int16_t)0xF665;
					G16(game_addr(CAM_EYE) + 4) = (int16_t)(G32(game_addr(BASE_Y3)) - 0x206);
				}
				else if (v7 > 31 && v7 <= 40)
				{
					F16(0x18) = G16(game_addr(CAM_EYE));
					F16(0x1A) = G16(game_addr(CAM_EYE) + 2);
					F16(0x1C) = (int16_t)(G32(game_addr(CAM_EYE) + 4) - G32(game_addr(BASE_Y3)));
					cx::RotMatrixY(0x40, FP(0x44));
					cx::MatVec(FP(0x44), FP(0x18), (void *)game_addr(CAM_EYE));
					G16(game_addr(CAM_EYE) + 4) += (int16_t)G32(game_addr(BASE_Y3));
					F16(0x18) = G16(game_addr(CAM_AT));
					F16(0x1A) = G16(game_addr(CAM_AT) + 2);
					F16(0x1C) = (int16_t)(G32(game_addr(CAM_AT) + 4) - G32(game_addr(BASE_Y3)));
					cx::MatVec(FP(0x44), FP(0x18), (void *)game_addr(CAM_AT));
					G16(game_addr(CAM_AT) + 4) += G16(game_addr(BASE_Y3));
				}
				else if (v10 < 0x800)
				{
					G32(game_addr(CAM_AT)) = D32(0x18);
					G32(game_addr(CAM_AT) + 4) = D32(0x1C);
					G32(game_addr(CAM_EYE)) = D32(0x20);
					G32(game_addr(CAM_EYE) + 4) = D32(0x24);
				}
				else
				{
					int32_t v12 = 2 * v10 - 0x1000;
					F16(0x2C) = (int16_t)(G32(game_addr(BASE_Y3)) - 0xA14);
					F32(0x10) = 0x1000 - v12;
					F16(0x20) = (int16_t)0xFD69; F16(0x22) = (int16_t)0xF544; F16(0x24) = (int16_t)(G32(game_addr(BASE_Y3)) + 0x6F7);
					F16(0x28) = 0x547; F16(0x2A) = (int16_t)0xF9F4;
					cx::Blend(nd + 0x18, FP(0x20), 0x1000 - v12, v12, (void *)game_addr(CAM_AT));
					cx::Blend(nd + 0x20, FP(0x28), F32(0x10), v12, (void *)game_addr(CAM_EYE));
				}
				cx::SpawnArcO((uint32_t)D32(0x8C), game_addr(game::unk_12AE7F0) + 8 * (v7 % 10), game_addr(game::unk_12AE840), 12, 7, 0x100);
				if (v7 < 15 && (v7 & 1) == 0)
				{
					int32_t r = Rand();
					F16(0x12) = 0;
					F16(0x10) = (int16_t)((int16_t)((r & 0x3FF) + W16(0x44)) - 0x200);
					r = Rand();
					F16(0x14) = (int16_t)((int16_t)((r & 0x3FF) + W16(0x48)) - 0x200);
					r = Rand();
					F16(0x28) = (int16_t)((r & 0x7FF) - 0x400);
					r = Rand();
					F16(0x2A) = (int16_t)(-0x800 - (r & 0x3FF));
					r = Rand();
					F16(0x2C) = (int16_t)((r & 0x7FF) + F32(0x14) - 0x400);
					r = Rand();
					F16(0x20) = (int16_t)((r & 0x7F) - 0x40);
					r = Rand();
					F16(0x22) = (int16_t)(-0x40 - (r & 0x3F));
					r = Rand();
					F16(0x24) = (int16_t)((r & 0x7F) - 0x40);
					cx::SpawnBoltO(FP(0x20), FP(0x10), FP(0x28), 0, 0, 0, 0x100, 0xFA, 0, 3);
				}
			}
		}

		if ((uint32_t)((int32_t)W16(0x0C) - 101) < 0x40)
		{
			if (W16(0x0C) == 101) cx::Decode(game_addr(game::unk_1298C68), L, 0x158);
			F32(0x40) = (int32_t)(MB + 0x8D0);
			F16(0x34) = G16(game_addr(BASE_Y3));
			F32(0x38) = 0; F16(0x3C) = 0; F16(0x32) = 0; F16(0x30) = 0;
			Play(L, game_addr(CB_PrimDraw), FP(0x30));
		}

		{
			uint32_t t = (uint32_t)((int32_t)W16(0x0C) - 157);
			if (t < 0x14 && (t & 1) == 0 && !Pause())
			{
				int32_t r = Rand();
				int32_t vtx = *(int16_t *)(game_addr(game::unk_12AFE40) + 2 * ((uint32_t)(r * 11) >> 15));
				cx::VertexPos(model, 0, vtx, FP(0x18));
				r = Rand();
				F16(0x28) = (int16_t)((r & 0x7FF) + F32(0x18) - 0x400);
				r = Rand();
				F16(0x2A) = (int16_t)(*(int32_t *)FP(0x1A) - (r & 0x3FF) - 0x400);
				r = Rand();
				F16(0x2C) = (int16_t)((r & 0x7FF) + F32(0x1C) - 0x400);
				r = Rand();
				F16(0x20) = (int16_t)((r & 0x7F) - 0x40);
				r = Rand();
				F16(0x22) = (int16_t)(-0x40 - (r & 0x3F));
				r = Rand();
				F16(0x24) = (int16_t)((r & 0x7F) - 0x40);
				cx::SpawnBoltO(FP(0x20), FP(0x18), FP(0x28), 0, 0, 0, 0x100, 0xFA, 0, 3);
			}
		}

		{
			uint32_t t = (uint32_t)((int32_t)W16(0x0C) - 207);
			if (t < 0x14)
			{
				if (t == 0)
				{
					G16(game_addr(CAM_EYE)) = 0x2A5;
					G16(game_addr(CAM_EYE) + 2) = (int16_t)0xF20D;
					G16(game_addr(CAM_EYE) + 4) = (int16_t)(G32(game_addr(BASE_Y3)) - 200);
				}
				cx::VertexPos(ModelBuffer() + 0x28, 0, 0x32, (void *)game_addr(CAM_AT));
				if (!Pause())
					cx::SpawnArcO((uint32_t)D32(0x8C), game_addr(game::unk_12AF800) + 8 * (t % 5), game_addr(game::unk_12AF828), 12, 7, 0x100);
			}
		}

		if ((uint32_t)((int32_t)W16(0x0C) - 212) < 0xF)
		{
			if (W16(0x0C) == 212) cx::Decode(game_addr(game::unk_12A1164), L, 0x1A0);
			F32(0x7C) = G16(game_addr(CAM_AT) + 2);
			F32(0x78) = G16(game_addr(CAM_AT));
			F32(0x8C) = (int32_t)(MB + 0x8D0);
			int32_t yaw = -(int32_t)W16(0x36);
			F32(0x84) = 0; F16(0x88) = 0;
			F32(0x80) = G16(game_addr(CAM_AT) + 4);
			cx::RotMatrixXY(-0x80, yaw, FP(0x64));
			ComposeAffineTransform(&Camera(), (Mat4x3 *)FP(0x64), (Mat4x3 *)FP(0x64));
			Play(L, game_addr(CB_PrimMatrix), FP(0x64));
		}

		if ((uint32_t)((int32_t)W16(0x0C) - 227) < 0x14)
		{
			if (W16(0x0C) == 227) cx::Decode(game_addr(game::unk_12A9664), L, 0x1D8);
			F32(0x8C) = (int32_t)(MB + 0x8D0);
			F32(0x84) = 0; F16(0x88) = 0;
			cx::VertexPos(model, 0, 0x32, FP(0x28));
			F32(0x7C) = F16(0x2A);
			F32(0x78) = F16(0x28);
			F32(0x80) = F16(0x2C);
			cx::RotMatrixXY(-0x80, -(int32_t)W16(0x36), FP(0x64));
			ComposeAffineTransform(&Camera(), (Mat4x3 *)FP(0x64), (Mat4x3 *)FP(0x64));
			Play(L, game_addr(CB_PrimMatrix), FP(0x64));
		}

		{
			uint32_t t = (uint32_t)((int32_t)W16(0x0C) - 237);
			if (t < 0x14 && t >= 0xA)
			{
				if (t == 0xA)
				{
					if (cx::LoadBusy()) return 0;
					cx::Decode(game_addr(game::unk_129BA94), L, 0x128);
				}
				F16(0x74) = 0x1000; F16(0x6C) = 0x1000; F16(0x64) = 0x1000;
				F32(0x8C) = (int32_t)(MB + 0x8D0);
				F32(0x84) = 0; F16(0x88) = 0;
				F16(0x72) = 0; F16(0x70) = 0; F16(0x6E) = 0; F16(0x6A) = 0; F16(0x68) = 0; F16(0x66) = 0;
				F32(0x78) = 0; F32(0x7C) = 2000; F32(0x80) = 4000;
				Play(L, game_addr(CB_PrimMatrix), FP(0x64));
			}
		}

		{
			uint32_t t = (uint32_t)((int32_t)W16(0x0C) - 257);
			if (t < 0x2E)
			{
				if (t == 0) cx::Sub508500();
				if (!Pause())
				{
					for (int k = 0; k < 4; k++)
					{
						int32_t r = Rand();
						cx::Sub6C76C0(game_addr(game::aWe), (r * (int32_t)*(int16_t *)game_addr(game::aWe)) >> 15);
					}
				}
				if (t == 0) cx::Decode(game_addr(game::unk_1299718), L, 0xB4);
				F32(0x40) = (int32_t)(MB + 0x8D0);
				F16(0x32) = G16(game_addr(BASE_Y2));
				F16(0x34) = G16(game_addr(BASE_Y));
				F32(0x38) = -128; F16(0x3C) = 1; F16(0x3E) = (int16_t)t; F16(0x30) = 0;
				Play(L, game_addr(CB_PrimDraw), FP(0x30));
				if (t >= 0x14)
				{
					if (t == 0x14) cx::Decode(game_addr(game::unk_129CD84), (void *)game_addr(game::unk_129F4E8), 0x4BC);
					uint32_t p = Pause();
					if (!p)
					{
						int16_t d = t < 0x26 ? -64 : 0x180;
						G16(game_addr(CAM_EYE) + 2) += d;
						G16(game_addr(CAM_AT) + 2) += d;
					}
					F32(0x38) = 0; F16(0x3C) = 0;
					F32(0x8C) = (int32_t)(MB + 0x8D0);
					F32(0x40) = (int32_t)(MB + 0x8D0);
					F16(0x30) = 0;
					F16(0x34) = G16(game_addr(BASE_Y));
					F16(0x32) = (int16_t)(G32(game_addr(BASE_Y2)) - 3000);
					Play((prim::Layout *)game_addr(game::unk_129F4E8), game_addr(CB_PrimDraw), FP(0x30));
				}
			}
		}

		{
			uint32_t t = (uint32_t)((int32_t)W16(0x0C) - 303);
			if (t < 0x10)
			{
				if (t == 0)
				{
					cx::Decode(game_addr(game::unk_12A9664), L, 0x124);
					InitTaskQueuePool(&QueueDebris().first, (void *)game_addr(game::unk_12A53E0), 0x2C, 0x20);
				}
				for (int k = 0; k < 2; k++)
				{
					uint8_t *d = (uint8_t *)AddTaskToQueue(&QueueDebris().first, game_addr(ORIG_DebrisTask));
					if (!d) continue;
					*(int16_t *)(d + 0x0C) = 0;
					*(int16_t *)(d + 0x10) = (int16_t)((cx::SharedRand() & 0xFFF) - 0x800);
					int32_t r = cx::SharedRand();
					*(int16_t *)(d + 0x12) = 0;
					*(int16_t *)(d + 0x14) = (int16_t)((r & 0xFFF) + G32(game_addr(BASE_Y)) - 0x800);
					*(int16_t *)(d + 0x18) = (int16_t)((cx::SharedRand() & 0x3F) - 0x20);
					*(int16_t *)(d + 0x1C) = (int16_t)((cx::SharedRand() & 0x3F) - 0x20);
					*(int16_t *)(d + 0x1A) = (int16_t)(-0x80 - (cx::SharedRand() & 0x7F));
					*(int16_t *)(d + 0x20) = (int16_t)cx::SharedRand();
					*(int16_t *)(d + 0x22) = (int16_t)cx::SharedRand();
					*(int16_t *)(d + 0x24) = (int16_t)cx::SharedRand();
					d[0x28] = (uint8_t)cx::SharedRand();
					d[0x29] = (uint8_t)cx::SharedRand();
					d[0x2A] = (uint8_t)cx::SharedRand();
				}
				F16(0x34) = G16(game_addr(BASE_Y));
				F32(0x40) = (int32_t)(MB + 0x8D0);
				F32(0x8C) = (int32_t)(MB + 0x8D0);
				F32(0x38) = -256; F16(0x3C) = 0; F16(0x32) = 0; F16(0x30) = 0;
				Play(L, game_addr(CB_PrimDraw), FP(0x30));
			}
		}

		{
			uint32_t t = (uint32_t)((int32_t)W16(0x0C) - 319);
			if (t < 0x24)
			{
				if (t == 0) cx::Decode((uint32_t)(ModelBuffer() + 0x28E9C), L, 0x354);
				F16(0x34) = G16(game_addr(BASE_Y));
				F32(0x40) = (int32_t)(ModelBuffer() + 0x8D0);
				F32(0x8C) = (int32_t)(ModelBuffer() + 0x8D0);
				F32(0x38) = -1024;
				F16(0x3C) = 0; F16(0x32) = 0; F16(0x30) = 0;
				Play(L, game_addr(CB_PrimDraw), FP(0x30));
				if (!Pause())
				{
					int16_t d = (int16_t)(0x80 - (t << 7) / 36);
					G16(game_addr(CAM_EYE)) += d;
					G16(game_addr(CAM_EYE) + 2) -= d;
					G16(game_addr(CAM_EYE) + 4) -= d;
				}
			}
		}

		switch ((uint16_t)W16(0x0C))
		{
		case 0x49: case 0x4F: case 0xF8: case 0xFA: case 0xFD: case 0x102: case 0x13D: cx::Sub6C6530(game_addr(game::unk_12AE7E0)); break;
		case 0x4E: cx::Sub6C65A0(game_addr(game::unk_12AE7E4)); break;
		case 0x50: case 0xE3: case 0x101: case 0x103: case 0x13C: case 0x13E: cx::Sub6C6530(game_addr(game::unk_12AE7E4)); break;
		case 0x51: case 0xE4: case 0x13B: case 0x13F: cx::Sub6C6530(game_addr(game::unk_12AE7E8)); break;
		case 0xE5: cx::Sub6C6530(game_addr(game::unk_12AE7EC)); break;
		default: break;
		}

		{
			uint32_t D = (uint32_t)(ModelBuffer() + 0x28E9C);
			int32_t cc = W16(0x0C);
			switch (cc)
			{
			case 0x00: if (cx::LoadBusy()) return 0; cx::Load(0x1F9, D, 1); cx::Load(0x1FA, (uint32_t)(ModelBuffer() + 0x28E9C), 1); cx::Load(0x1FB, (uint32_t)(ModelBuffer() + 0x28E9C), 2); break;
			case 0x1E: if (cx::LoadBusy()) return 0; cx::Load(0x1FC, D, 2); cx::Load(0x1FD, (uint32_t)(ModelBuffer() + 0x28E9C), 1); cx::Load(0x1FE, (uint32_t)(ModelBuffer() + 0x28E9C), 3); break;
			case 0x3C: if (cx::LoadBusy()) return 0; cx::Load(0x1FF, D, 1); cx::Load(0x200, (uint32_t)(ModelBuffer() + 0x3169C), 5); break;
			case 0x4C: if (cx::LoadBusy()) return 0; cx::Load(0x201, game_addr(game::unk_1298C68), 5); break;
			case 0x55: if (cx::LoadBusy()) return 0; cx::Load(0x202, D, 0); cx::Load(0x203, game_addr(game::unk_12A1164), 5); break;
			case 0xA1:
				if (cx::LoadBusy()) return 0;
				cx::Load(0x204, D, 0); cx::Load(0x205, game_addr(game::unk_12A9664), 5);
				cx::Load(0x206, (uint32_t)(ModelBuffer() + 0x28E9C), 1);
				cx::Load(0x207, (uint32_t)(ModelBuffer() + 0x28E9C), 1);
				cx::Load(0x208, (uint32_t)(ModelBuffer() + 0x28E9C), 3);
				break;
			case 0xD3: if (cx::LoadBusy()) return 0; cx::Load(0x209, game_addr(game::unk_1298C68), 5); cx::Load(0x20A, (uint32_t)(ModelBuffer() + 0x28E9C), 1); break;
			case 0x100: if (cx::LoadBusy()) return 0; cx::Load(0x20B, D, 1); cx::Load(0x20C, (uint32_t)(ModelBuffer() + 0x28E9C), 3); break;
			case 0x114:
				if (cx::LoadBusy()) return 0;
				G32(game_addr(game::unk_25216FC)) = 0; G32(game_addr(game::MAG_116_FlashQueue)) = 0;
				*(int32_t *)(ModelBuffer() + 0x28E50) = 0;
				cx::Load(0x20D, game_addr(game::unk_12A9664), 5); cx::Load(0x20E, (uint32_t)(ModelBuffer() + 0x28E9C), 2);
				break;
			case 0x12E:
				if (cx::LoadBusy()) return 0;
				G32(game_addr(game::unk_252172C)) = 0; G32(game_addr(game::GF116_QUEZACOTL_queueCreature)) = 0; G32(game_addr(game::unk_252171C)) = 0; G32(game_addr(game::MAG_116_BoltQueue)) = 0; G32(game_addr(game::unk_252170C)) = 0; G32(game_addr(game::MAG_116_QueueBolts)) = 0;
				cx::Load(0x20F, D, 0x10);
				break;
			case 0x13E: if (cx::LoadBusy()) return 0; break;
			default: break;
			}
		}
		cx::LoadPump();
		if (W16(0x0C) == 351) cx::ReleaseVoice((uint32_t)D32(0x10));

		{
			uint32_t t = (uint32_t)((int32_t)W16(0x0C) - 232);
			if (t < 0x23)
			{
				if (t == 0) InitTaskQueuePool(&QueueDebris().second, (void *)game_addr(game::unk_12A53E0), 0x14, 0x40);
				cx::VertexPos(model, 0, 0x32, FP(0x18));
				int32_t r = Rand();
				F16(0x2A) = 0;
				F16(0x28) = (int16_t)((r & 0x7FF) - 0x3FF);
				r = Rand();
				F16(0x2C) = (int16_t)((r & 0x3FF) + G32(game_addr(BASE_Y)) - 0x200);
				r = Rand();
				F16(0x20) = (int16_t)((r & 0xFF) - 0x80);
				r = Rand();
				F16(0x22) = (int16_t)(-0x40 - (r & 0x7F));
				r = Rand();
				F16(0x24) = (int16_t)(-0x40 - (r & 0x3F));
				cx::SpawnBoltO(FP(0x20), FP(0x18), FP(0x28), 0, 0, 0, 0x100, 0xFA, 1, 2);
			}
		}

		switch ((uint16_t)W16(0x0C))
		{
		case 0:
			cx::Sub6C5300();
			cx::PlaySE(game_addr(game::unk_12AE4F0), 1, 0x80);
			G16(game_addr(game::g_BattleCam_Roll)) = 0;
			G16(game_addr(game::BATTLE_PROJECTION_DISTANCE)) = 0x120;
			G16(game_addr(CAM_AT)) = 0; G16(game_addr(CAM_AT) + 2) = (int16_t)0xEBE2; G16(game_addr(CAM_AT) + 4) = (int16_t)0xFD2D;
			G16(game_addr(CAM_EYE)) = 0; G16(game_addr(CAM_EYE) + 2) = (int16_t)0xF10D; G16(game_addr(CAM_EYE) + 4) = (int16_t)0xF841;
			break;
		case 0x41: cx::PlaySE(game_addr(game::unk_12AE4F4), 1, 0x80); break;
		case 0x45: cx::Sub6C5350(); cx::PlayStream(0x80, 1, 0x7F); break;
		case 0x4C:
			G16(game_addr(CAM_AT)) = 0; G16(game_addr(CAM_AT) + 4) = (int16_t)G32(game_addr(BASE_Y3)); G16(game_addr(CAM_AT) + 2) = (int16_t)0xFD74;
			G16(game_addr(CAM_EYE)) = (int16_t)0xF8EF; G16(game_addr(CAM_EYE) + 2) = (int16_t)0xFE9C; G16(game_addr(CAM_EYE) + 4) = (int16_t)(G32(game_addr(BASE_Y3)) + 0x8E4);
			break;
		case 0x7E: cx::Sub6C5300(); break;
		case 0xD1: cx::PlaySE(game_addr(game::unk_12AE4F8), 1, 0x80); break;
		case 0xD4: cx::Sub6C5350(); break;
		case 0xF6:
			G16(game_addr(CAM_AT)) = (int16_t)0xFB92; G16(game_addr(CAM_AT) + 2) = (int16_t)0xFD39; G16(game_addr(CAM_EYE)) = 0x10D9;
			G16(game_addr(CAM_AT) + 4) = (int16_t)(G32(game_addr(BASE_Y)) - 0x108);
			G16(game_addr(CAM_EYE) + 2) = (int16_t)0xFC8B; G16(game_addr(CAM_EYE) + 4) = (int16_t)(G32(game_addr(BASE_Y)) + 0xCA4);
			break;
		case 0xE2:
			G16(game_addr(CAM_AT)) = (int16_t)0xFFAD; G16(game_addr(CAM_AT) + 2) = (int16_t)0xF9BC; G16(game_addr(CAM_EYE)) = 0xB29; G16(game_addr(CAM_EYE) + 2) = (int16_t)0xFDD7;
			G16(game_addr(CAM_AT) + 4) = (int16_t)(G32(game_addr(BASE_Y3)) - 0x832);
			G16(game_addr(CAM_EYE) + 4) = (int16_t)(G32(game_addr(BASE_Y3)) + 0xA08);
			cx::PlayStream(0x80, 1, 0x7F);
			break;
		case 0x100:
			G16(game_addr(CAM_AT) + 4) = (int16_t)G32(game_addr(BASE_Y));
			G16(game_addr(CAM_AT)) = 0; G16(game_addr(CAM_AT) + 2) = 0; G16(game_addr(CAM_EYE)) = (int16_t)0xF060; G16(game_addr(CAM_EYE) + 2) = (int16_t)0xF060;
			G16(game_addr(CAM_EYE) + 4) = (int16_t)(G32(game_addr(BASE_Y)) + 0xFA0);
			break;
		case 0x114:
			G32(game_addr(BASE_Y2)) = -4096;
			G16(game_addr(game::g_BattleCam_Roll)) = G16(game_addr(game::Battle_Camera_ReturnView_Roll));
			G16(game_addr(CAM_AT)) = (int16_t)0xFFEF; G16(game_addr(CAM_AT) + 2) = (int16_t)0xEAF5; G16(game_addr(CAM_AT) + 4) = (int16_t)(G32(game_addr(BASE_Y)) + 0x130);
			G16(game_addr(CAM_EYE)) = (int16_t)0xFE52; G16(game_addr(CAM_EYE) + 2) = (int16_t)0xEFA4; G16(game_addr(CAM_EYE) + 4) = (int16_t)(G32(game_addr(BASE_Y)) + 0x498);
			break;
		case 0x115: cx::PlaySE(game_addr(game::unk_12AE4FC), 1, 0x80); break;
		case 0x12E:
			G32(game_addr(BASE_Y2)) = 0;
			G16(game_addr(CAM_AT)) = 0; G16(game_addr(CAM_AT) + 2) = (int16_t)0xFAF5; G16(game_addr(CAM_AT) + 4) = (int16_t)(G32(game_addr(BASE_Y3)) - 0x93A);
			G16(game_addr(CAM_EYE)) = 0; G16(game_addr(CAM_EYE) + 2) = (int16_t)0xF6B6; G16(game_addr(CAM_EYE) + 4) = (int16_t)(G32(game_addr(BASE_Y3)) + 0xDE3);
			break;
		case 0x13E:
			G16(game_addr(CAM_AT)) = 0; G16(game_addr(CAM_AT) + 4) = (int16_t)G32(game_addr(BASE_Y)); G16(game_addr(CAM_AT) + 2) = 0;
			G16(game_addr(CAM_EYE)) = 0x1000; G16(game_addr(CAM_EYE) + 2) = (int16_t)0xF000; G16(game_addr(CAM_EYE) + 4) = (int16_t)(G32(game_addr(BASE_Y)) - 0x1000);
			cx::PlayStream(0x80, 1, 0x7F);
			break;
		default: break;
		}

		if (W16(0x0C) == 345)
		{
			uint8_t *ctx = *(uint8_t **)(*(uint8_t **)game_addr(game::GF116_QUEZACOTL_castCtx) + 4);
			cx::ApplyResult(*(uint32_t *)(ctx + 8), ctx[0x10]);
		}
		if (W16(0x0C) >= 355)
		{
			ff8fx::copy<(0x427F * 4)>((void *)game_addr(game::unk_1298C68), cx::SummonData());
			ff8fx::copy<(0x1367 * 4)>((void *)game_addr(game::unk_12A9664), cx::SummonData() + 0x109FC);
			cx::Sub508630(ModelBuffer() + 0x2E50, &DoneFlag());
			cx::Sub6C6500(1);
			return TASK_END;
		}
		W16(0x0C) = (int16_t)(W16(0x0C) + 1);
		return 0;
	}

	[[maybe_unused]] static void SpawnArc(uint8_t **bones, const int16_t *strips, const uint8_t *faces, uint8_t reach_shift, uint8_t fade_shift, int16_t speed)
	{
		ArcNode *arc = (ArcNode *)AddTaskToQueue(&QueueCreature().first, game_addr(ORIG_ArcTask));
		if (!arc) return;
		arc->reach_shift = reach_shift;
		arc->fade_shift = fade_shift;
		arc->speed = speed;
		arc->strips = strips;
		arc->age = 0;
		arc->progress = 0;
		arc->faces = faces;
		arc->bones = bones;
		for (int i = 0; i < 16; i++)
		{
			int32_t s = ComputeSin(Div15(shl32(i, 11)));
			arc->pts[i].color = 0x80;
			arc->pts[i].width = (int16_t)(shl32(shl32(s, 6) + s, 2) >> 12);
			arc->pts[i].pad = 0;
		}
	}
}

	void register_mag116_quezacotl()
	{
		register_port(q116::ORIG_SequenceTask, (void *)q116::SequenceTask, "Q116 SequenceTask", 116);
		register_port(q116::ORIG_CreatureTask, (void *)q116::CreatureTask, "Q116 CreatureTask", 116);
		register_port(q116::ORIG_DebrisTask, (void *)q116::DebrisTask, "Q116 DebrisTask", 116);
		register_port(q116::ORIG_BranchTask, (void *)q116::BranchTask, "Q116 BranchTask", 116);
		register_port(q116::ORIG_BoltTask, (void *)q116::BoltTask, "Q116 BoltTask", 116);
		register_port(q116::ORIG_FlashTask, (void *)q116::FlashTask, "Q116 FlashTask", 116);
		register_port(q116::ORIG_ArcTask, (void *)q116::ArcTask, "Q116 ArcTask", 116);
	}
}

