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
namespace elvoret017
{
	using namespace eng;
	using namespace magc;

	inline uint8_t *&Caster() { return var<uint8_t *>(game_addr(game::MAG_017_g_CasterEntity)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::MAG_017_g_TexBase)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::MAG_017_g_CastContext)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_017_g_PacketCursor)); }
	inline TaskQueue *QParticles() { return (TaskQueue *)game_addr(game::MAG_017_g_ParticleQueue); }
	inline TaskQueue *QMain() { return (TaskQueue *)game_addr(game::MAG_017_g_MainQueue); }
	inline uint32_t RenderOT(uint32_t off) { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + off; }

	static constexpr game::Id ORIG_RootTask = game::MAG_017_ElvoretEntrance_RootTask;
	static constexpr game::Id ORIG_MainTask = game::MAG_017_ElvoretEntrance_MainTask;
	static constexpr game::Id ORIG_ParticleTask = game::MAG_017_ElvoretEntrance_ParticleTask;
	static constexpr game::Id ORIG_PartCallback = game::MAG_017_ElvoretEntrance_PartCallback;
	static constexpr game::Id MODEL_Layout = game::MAG_017_LayoutData;
	static constexpr game::Id PART_Flags = game::MAG_017_LayoutFlags;
	static constexpr game::Id SEQ_Particle = game::MAG_017_ParticleSequence;
	static constexpr game::Id COLOUR_Black = game::MAG_017_g_BlackColour;
	static constexpr game::Id AMBIENT = game::CURRENT_BS_SHADOW_PALETTE;
	static const void *SOUND_Voice() { return (const void *)game_addr(game::MAG_017_SoundVoice); }
	static const void *SOUND_Wings() { return (const void *)game_addr(game::MAG_017_SoundWings); }
	static const void *SOUND_Land() { return (const void *)game_addr(game::MAG_017_SoundLand); }

	inline int32_t ComputeCos(int32_t angle) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(angle); }
	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t flag, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, flag, out); }
	inline void ReleaseVoiceSlot(uint32_t slot) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::BdSound_ReleaseVoiceSlot))(slot); }
	inline void BdPlayActorSE(uint8_t *entity, int32_t a, int32_t b, int32_t c) { fn<void (__cdecl *)(uint8_t *, int32_t, int32_t, int32_t)>(game_addr(game::BdPlayActorSE))(entity, a, b, c); }
	inline void QueueChainTransformation(uint8_t *entity, int32_t chain) { fn<void (__cdecl *)(uint8_t *, int32_t)>(game_addr(game::QueueChainTransformation))(entity, chain); }
	inline void ColourLerp(uint32_t out, uint32_t a, int32_t wa, int32_t wb, const void *b) { fn<void (__cdecl *)(uint32_t, uint32_t, int32_t, int32_t, const void *)>(game_addr(game::ColourLerp))(out, a, wa, wb, b); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }

#pragma pack(push, 1)
	struct RootNode
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
		int16_t pad0E;
		uint32_t voice;
		uint8_t layout[0x1E8];
	};
	struct ParticleNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		int16_t pos[3];
		int16_t scale;
		int16_t vel[3];
		int16_t pad1E;
	};
	struct PrimArg
	{
		int16_t pos[4];
		uint32_t flags;
		uint32_t morph;
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x14 && sizeof(MainNode) == 0x1FC && sizeof(ParticleNode) == 0x20, "Elvoret entrance nodes");
	static_assert(sizeof(PrimArg) == 0x10, "Elvoret entrance prim block");

	static void __cdecl PartCallback(prim::Layout *l, prim::Record *r, int arg_)
	{
		const PrimArg *arg = (const PrimArg *)arg_;
		if (((int32_t)r->scale[2] | *(const int32_t *)r->scale) == 0) return;
		if (r->a >= 0x1000 && *(const uint32_t *)r->rgb == 0) return;
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
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
		m.t[0] = (int32_t)r->pos[0] + arg->pos[0];
		m.t[1] = (int32_t)r->pos[1] + arg->pos[1];
		m.t[2] = (int32_t)r->pos[2] + arg->pos[2];
		const uint32_t *flags = (const uint32_t *)arg->flags;
		if (flags[(int16_t)r->index] & 0x10000000)
		{
			TransformVectorBy3x3Matrix(&Camera(), m.t, m.t);
			m.t[1] = (int32_t)((uint32_t)m.t[1] + (uint32_t)Camera().t[1]);
			m.t[0] = (int32_t)((uint32_t)m.t[0] + (uint32_t)Camera().t[0]);
			m.t[2] = (int32_t)((uint32_t)m.t[2] + (uint32_t)Camera().t[2]);
		}
		else ComposeAffineTransform(&Camera(), &m, &m);
		if (!(*(const uint32_t *)r->scale == 0x10001000 && r->scale[2] == 0x1000))
		{
			int32_t v[3] = { r->scale[0], r->scale[1], r->scale[2] };
			Scale3DMatrix(&m, v);
		}
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		const uint32_t mode = (flags[(int16_t)r->index] & 0xDFFF) | 0x2000;
		*(uint32_t *)(h + 0x1C) = mode;
		*(int32_t *)(h + 0xC) = r->a;
		if (r->a != 0)
		{
			*(uint32_t *)(h + 0x1C) = mode | 0xC0;
			*(uint32_t *)(h + 8) = *(const uint32_t *)r->rgb;
		}
		PacketCursor() = RenderPrimModel(h, RenderOT(0x44), 2, PacketCursor());
		FieldFree(0x58);
	}

	static void ParticleDraw(const ParticleNode *p)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		TransformCameraByShadowRotation(p->pos, p->scale, -((int32_t)p->scale >> 2));
		*(uint32_t *)h = game_addr(SEQ_Particle);
		*(int16_t *)(h + 4) = p->counter;
		*(int16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(0x44), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static void ParticleStep(ParticleNode *p)
	{
		p->pos[0] = (int16_t)(p->pos[0] + p->vel[0]);
		p->pos[1] = (int16_t)(p->pos[1] + p->vel[1]);
		p->pos[2] = (int16_t)(p->pos[2] + p->vel[2]);
		p->counter++;
	}

	static int32_t BurstCount(int16_t counter)
	{
		const uint32_t x = (uint32_t)((int32_t)counter - 0x50) << 12;
		const uint32_t a = (uint32_t)(((uint64_t)x * 0xCCCCCCCDu) >> 32) >> 5;
		return (ComputeSin((int32_t)a) >> 9) + 4;
	}
}
}

namespace ff8fx
{
namespace elvoret017
{
	static uint32_t __cdecl ParticleTask(TaskNode *n)
	{
		ParticleNode *p = (ParticleNode *)n;
		ParticleDraw(p);
		ParticleStep(p);
		return p->counter < 0xD ? 0 : TASK_END;
	}

	static uint32_t __cdecl MainTask(TaskNode *n)
	{
		MainNode *m = (MainNode *)n;
		if (m->counter == 0x14 || m->counter == 0x28) BdPlayActorSE(Caster(), 0, 0, 0x80);
		if ((uint32_t)(int32_t)m->counter < 0x55)
		{
			if ((uint32_t)(int32_t)m->counter <= 0x20)
			{
				const int32_t t = (int32_t)(((uint32_t)(int32_t)m->counter & 0xFFFFF) << 7);
				ColourLerp(game_addr(AMBIENT), game_addr(COLOUR_Black), t, 0x1000 - t, Caster() + 0x2C);
			}
			*(int16_t *)(Caster() + 0x1E) = (int16_t)(*(const int16_t *)(Caster() + 0x1E) + 0x40);
		}
		if ((uint32_t)((int32_t)m->counter - 0x50) < 0x14)
		{
			int32_t count = BurstCount(m->counter);
			int32_t angle = CrtRand();
			const int32_t step = 0x1000 / count;
			do
			{
				ParticleNode *p = (ParticleNode *)AddTaskToQueue(QParticles(), game_addr(ORIG_ParticleTask));
				if (p)
				{
					Memset32(&p->counter, 0, 5);
					const int16_t c = (int16_t)(ComputeCos(angle) >> 2);
					p->vel[0] = c;
					p->pos[0] = c;
					const int16_t s = (int16_t)(ComputeSin(angle) >> 2);
					p->vel[2] = s;
					p->pos[2] = s;
					p->pos[0] = (int16_t)(p->pos[0] + *(const int16_t *)(Caster() + 0x1C));
					p->pos[2] = (int16_t)(*(const int16_t *)(Caster() + 0x20) + s);
					p->pos[1] = *(const int16_t *)(Caster() + 0x24);
					const int32_t r = CrtRand() & 0xFFF;
					const int32_t f = r + (0x400 - (r >> 2));
					p->vel[0] = (int16_t)(mul32(p->vel[0], f) >> 14);
					p->vel[2] = (int16_t)(mul32(p->vel[2], f) >> 14);
					p->scale = (int16_t)((CrtRand() & 0xFFF) + 0x800);
				}
				angle += step;
			} while (--count != 0);
		}
		if ((uint32_t)(int32_t)m->counter < 0x55)
		{
			const uint32_t phase = (uint32_t)(int32_t)m->counter % 0x19;
			int16_t sp[4];
			sp[3] = 0;
			GetEffectSpawnPosition(Caster(), 0x1D, 0x400, sp);
			if (phase == 0) BdPlaySE3D(SOUND_Wings(), 0, sp);
			for (int k = 0; k < 2; k++)
			{
				const uint32_t chance = (uint32_t)(mul32(CrtRand(), 20) >> 15);
				if (chance <= phase) continue;
				ParticleNode *p = (ParticleNode *)AddTaskToQueue(QParticles(), game_addr(ORIG_ParticleTask));
				if (!p) continue;
				Memset32(&p->counter, 0, 5);
				ff8fx::copy<(8)>(p->pos, sp);
				p->scale = (int16_t)((CrtRand() & 0x3FF) + 0x800);
				p->vel[0] = (int16_t)((CrtRand() & 0x3F) - 0x20);
				p->vel[1] = (int16_t)((CrtRand() & 0x3F) - 0x20);
				p->vel[2] = (int16_t)((CrtRand() & 0x3F) - 0x20);
			}
		}
		if (m->counter == 0x55)
		{
			for (int s = 0; s < 3; s++)
			{
				uint8_t *e = Entity(s);
				if ((e[0] & 2) && !(e[8] & 3)) QueueChainTransformation(e, 4);
			}
		}
		if (m->counter >= 0x32)
		{
			if (m->counter == 0x50) BdPlaySE3D(SOUND_Land(), 0, (const int16_t *)(Caster() + 0x1C));
			PrimArg arg;
			arg.pos[0] = *(const int16_t *)(Caster() + 0x1C);
			arg.pos[2] = *(const int16_t *)(Caster() + 0x20);
			arg.pos[1] = *(const int16_t *)(Caster() + 0x24);
			arg.pos[3] = 0;
			arg.flags = game_addr(PART_Flags);
			arg.morph = TexBase() + 0x1B9C;
			prim::play((prim::Layout *)m->layout, PartCallback, (int)&arg, 0);
		}
		if (m->counter == 0x76) ReleaseVoiceSlot(m->voice);
		const int16_t c = m->counter;
		if (c >= 0x7A) return TASK_END;
		m->counter = (int16_t)(c + 1);
		return 0;
	}

	static uint32_t __cdecl RootTask(TaskNode *n)
	{
		RootNode *r = (RootNode *)n;
		if (r->arena)
		{
			PacketCursor() = TexBase() + 0x1BA4;
			r->arena = 0;
		}
		else
		{
			PacketCursor() = TexBase() + 0x11BA4;
			r->arena = 1;
		}
		Caster()[1] |= 0x10;
		if (r->counter == 1)
		{
			if (!r->started)
			{
				*(uint16_t *)Caster() &= 0xFFFB;
				r->started = 1;
				InitTaskQueuePool(QMain(), (void *)(TexBase() + 0x1000), 0x1FC, 1);
				InitTaskQueuePool(QParticles(), (void *)TexBase(), 0x20, 0x80);
			}
			MainNode *m = (MainNode *)AddTaskToQueue(QMain(), game_addr(ORIG_MainTask));
			if (m)
			{
				Memset32(&m->counter, 0, 0x7C);
				m->voice = ClaimVoiceSlot(SOUND_Voice(), 1, 0x80);
				DecodeModelPrimLayout(game_addr(MODEL_Layout), m->layout, 0x1E8);
			}
		}
		int a, b;
		if (r->started)
		{
			a = ExecuteTaskQueue(QMain());
			b = ExecuteTaskQueue(QParticles());
		}
		else a = b = (int)n;
		if (r->started && a == 0 && b == 0)
		{
			*(uint16_t *)Caster() &= 0xEFFF;
			return TASK_END;
		}
		r->counter++;
		if (r->counter >= 10) r->counter = 10;
		return 0;
	}
}

	void register_mag017_elvoret_entrance()
	{
		register_port(elvoret017::ORIG_RootTask, (void *)elvoret017::RootTask, "017 RootTask", 17);
		register_port(elvoret017::ORIG_MainTask, (void *)elvoret017::MainTask, "017 MainTask", 17);
		register_port(elvoret017::ORIG_ParticleTask, (void *)elvoret017::ParticleTask, "017 ParticleTask", 17);
	}
}

