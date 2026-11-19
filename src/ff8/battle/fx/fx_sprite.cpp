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

// Effect sprites of the hit-effect queue

#include "mag_common.h"

namespace ff8fx
{
namespace sprite
{
	using namespace eng;
	using namespace magc;

	static constexpr game::Id ORIG_Sprite = game::BattleFx_TickSpriteEffect;
	static constexpr game::Id ORIG_SharedSprite = game::EffectSprite_SharedSequenceTask;

#pragma pack(push, 1)
	struct SpriteNode
	{
		TaskNode hdr;
		int16_t frame;
		int16_t pad0E;
		int16_t pos[3];
		int16_t shade;
		uint32_t source;
		uint8_t id;
		uint8_t rot;
		uint8_t scale;
		uint8_t palette;
		const uint8_t *seq;
	};
#pragma pack(pop)
	static_assert(sizeof(SpriteNode) == 0x24, "sprite node");

	inline bool Paused() { return (UpdateFlags() & 1) != 0; }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)); }

	static const uint8_t *SharedSequence()
	{
		const uint8_t *s = var<const uint8_t *>(game_addr(game::EFFECT_SELECTION_LIST) + 5 * 4);
		if (*(const uint32_t *)s & 0xFF000000) s = var<const uint8_t *>(game_addr(game::EFFECT_SELECTION_LIST));
		return s;
	}

	static int16_t Draw(const SpriteNode *s, const void *seq, int16_t frame, bool shared)
	{
		const int32_t size = (int32_t)(s->scale & 0x7F) << 8;
		int32_t shade = s->shade;
		if (shade == 0) shade = size >> 4;
		TransformCameraByShadowRotation(s->pos, size, -shade);
		uint8_t *h = AllocHeader(0xB4);
		*(const void **)h = seq;
		*(int16_t *)(h + 4) = frame;
		*(int32_t *)(h + 8) = (int32_t)s->rot << 4;
		*(int32_t *)(h + 0xC) = (s->scale & 0x80) ? -0x1000 : 0x1000;
		*(int32_t *)(h + 0x10) = 0x1000;
		if (shared)
		{
			h[0x20] = 0x11;
			*(uint16_t *)(h + 0x24) = 0x13;
		}
		else
		{
			*(uint16_t *)(h + 0x24) = 3;
			if (s->palette & 0x80)
			{
				*(uint16_t *)(h + 0x24) = 0x13;
				h[0x20] = (uint8_t)(s->palette & 0x7F);
			}
		}
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
		const int16_t next = *(const int16_t *)(h + 0x28);
		FieldFree(0xB4);
		return next;
	}
}
}

namespace ff8fx
{
namespace sprite
{
	static uint32_t Tick(TaskNode *n, bool shared)
	{
		SpriteNode *s = (SpriteNode *)n;
		const int16_t next = Draw(s, shared ? SharedSequence() : s->seq, s->frame, shared);
		if (Paused()) return 0;
		s->frame++;
		return next < 0 ? TASK_END : 0;
	}

	static uint32_t __cdecl SpriteTask(TaskNode *n) { return Tick(n, false); }

	static uint32_t __cdecl SharedSpriteTask(TaskNode *n) { return Tick(n, true); }
}

	void register_fx_sprite()
	{
		register_port(sprite::ORIG_Sprite, (void *)sprite::SpriteTask, "sprite 570BC0 BattleFx_TickSpriteEffect", 0);
		register_port(sprite::ORIG_SharedSprite, (void *)sprite::SharedSpriteTask, "sprite 570AC0 BattleFx_TickSharedSpriteEffect", 0);
	}
}

