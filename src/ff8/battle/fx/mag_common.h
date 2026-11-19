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

// Helpers shared by the effects

#pragma once

#include "fx_port.h"

namespace ff8fx
{
namespace magc
{
	using namespace eng;

#pragma pack(push, 1)
	struct ActionData
	{
		uint8_t attacker;
		uint8_t command;
		uint8_t pad02[4];
		uint16_t effect_id;
		uint8_t *targets;
		uint8_t pad0C[4];
		uint8_t target_count;
		uint8_t last_action;
		uint8_t pad12[2];
	};
	struct CastContext
	{
		uint8_t attacker;
		uint8_t flags;
		uint16_t target_mask;
		ActionData *actions;
		uint8_t sequence_mode, done, submode, pad;
	};
#pragma pack(pop)
	static_assert(sizeof(ActionData) == 20, "BattleActionTaskData is 20 bytes");
	static_assert(sizeof(CastContext) == 12, "MagicCastContext is 12 bytes");

	const int TARGET_STRIDE = 24;

	inline uint32_t UpdateFlags() { return var<uint32_t>(game_addr(game::battle_to_update_flags_dword_1D96A9C)); }
	inline bool DrawOnly() { return (UpdateFlags() & 0x201) != 0; }

	inline uint8_t *Entity(int slot) { return (uint8_t *)(game_addr(game::BattleEntitySlotData) + 0x9C * slot); }
	inline int16_t EntityHeight(int slot) { return *(int16_t *)(Entity(slot) + 0x24); }
	inline int16_t EntitySize(int slot) { return *(int16_t *)(Entity(slot) + 0x26); }

	inline int32_t CrtRand() { return fn<int32_t (__cdecl *)()>(game_addr(game::_rand))(); }
	inline void GetDefaultEffectPosition(uint8_t *entity, int16_t out[4]) { fn<void (__cdecl *)(uint8_t *, int16_t *)>(game_addr(game::GetDefaultEffectPosition))(entity, out); }
	inline void BdPlaySE(const void *sound, int32_t a, int32_t b) { fn<void (__cdecl *)(const void *, int32_t, int32_t)>(game_addr(game::BdPlaySE))(sound, a, b); }
	inline void BdPlaySE3D(const void *sound, int32_t volume, const int16_t *pos) { fn<void (__cdecl *)(const void *, int32_t, const int16_t *)>(game_addr(game::BdPlaySE3D))(sound, volume, pos); }
	inline void ApplyActionResultToTargets(uint8_t *targets, uint32_t count) { fn<void (__cdecl *)(uint8_t *, uint32_t)>(game_addr(game::ApplyActionResultToTargets))(targets, count); }
	inline void ApplyActionResultToTarget(uint8_t *target) { fn<void (__cdecl *)(uint8_t *)>(game_addr(game::ApplyActionResultToTarget))(target); }
	inline void ScreenFadeTask(int32_t a, int32_t b, int32_t c, int32_t d) { fn<void (__cdecl *)(int32_t, int32_t, int32_t, int32_t)>(game_addr(game::Effect_SpawnScreenFadeTask))(a, b, c, d); }
	inline void TextureRestoreTask(void *tex, void *done) { fn<void (__cdecl *)(void *, void *)>(game_addr(game::Battle_RestoreDefaultEffectTexture))(tex, done); }
	inline void Scale3DMatrix(Mat4x3 *m, const int32_t *v) { fn<void (__cdecl *)(Mat4x3 *, const int32_t *)>(game_addr(game::scale3DMatrix))(m, v); }
	inline void ComposeZYXRotationMatrix(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::ComposeZYXRotationMatrix))(angles, out); }
	inline void NormalizeVector(const int32_t *in, int32_t *out) { fn<void (__cdecl *)(const int32_t *, int32_t *)>(game_addr(game::NormalizeVectorToFixedPoint))(in, out); }
	inline void CrossProduct(const int32_t *a, const int32_t *b, int32_t *out) { fn<void (__cdecl *)(const int32_t *, const int32_t *, int32_t *)>(game_addr(game::FixedPointCrossProduct))(a, b, out); }
	inline void BuildAxisAngleRotationMatrix(int32_t angle, Mat4x3 *out, const int32_t *axis) { fn<void (__cdecl *)(int32_t, Mat4x3 *, const int32_t *)>(game_addr(game::BuildAxisAngleRotationMatrix))(angle, out, axis); }
	inline void TransformVectorBy3x3Matrix(const Mat4x3 *m, const int32_t *in, int32_t *out) { fn<void (__cdecl *)(const Mat4x3 *, const int32_t *, int32_t *)>(game_addr(game::TransformVectorBy3x3Matrix))(m, in, out); }
	inline int32_t RotationBetweenVectors(const int32_t *a, const int32_t *b, int32_t *axis) { return fn<int32_t (__cdecl *)(const int32_t *, const int32_t *, int32_t *)>(game_addr(game::GetRotationBetweenVectors_AxisAngle))(a, b, axis); }
	inline void *EffectSpriteSharedSequence(int id) { return fn<void *(__cdecl *)(int)>(game_addr(game::EffectSprite_GetSharedSequence))(id); }
}
}

namespace ff8fx
{
namespace magc
{
	static inline uint8_t *AllocHeader(int size)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(size);
		return h;
	}
}
}
