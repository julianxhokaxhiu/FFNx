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

// Effect parameters

#pragma once

#include <stddef.h>
#include <stdint.h>

namespace ff8fx
{
namespace fxp
{
	struct Window
	{
		int16_t first, last;
		bool has(int32_t tick) const { return tick >= first && tick <= last; }
	};

	struct Schedule
	{
		int16_t from[4];
		int16_t count[4];
		int32_t at(int32_t tick) const
		{
			int32_t n = 0;
			for (int k = 0; k < 4; k++)
				if (tick >= from[k]) n = count[k];
			return n;
		}
	};

	struct Range
	{
		int16_t min, max;
		int32_t roll(int32_t random) const { return min + random % (max - min + 1); }
	};

	struct Fraction
	{
		int16_t num, den;
		int32_t of(int32_t value) const { return value * num / den; }
	};

	struct GlintStyle
	{
		int16_t ticks;
		int16_t grow_w, grow_h;
		int16_t shrink_w, shrink_h;
		uint8_t centre_step[3];
		uint8_t tip_step[3];
		int16_t spin;
	};

	enum FieldType : uint8_t { I16 = 0, U8 = 1, I32 = 2 };
	struct Field
	{
		const char *part;
		const char *key;
		const char *label;
		const char *doc;
		uint32_t offset;
		FieldType type;
		int32_t min, max;
	};
	struct Part
	{
		int id;
		const char *key;
		const char *title;
		const char *doc;
	};
	struct Module
	{
		int effect_id;
		const char *name;
		void *params;
		const void *defaults;
		uint32_t size;
		const Part *parts;
		int part_count;
		const Field *fields;
		int field_count;
	};

	inline Module g_modules[64];
	inline int g_module_count = 0;
	inline void register_params(const Module &m)
	{
		for (int k = 0; k < g_module_count; k++)
			if (g_modules[k].effect_id == m.effect_id) return;
		if (g_module_count < 64) g_modules[g_module_count++] = m;
	}
	inline const Module *find_params(int effect_id)
	{
		for (int k = 0; k < g_module_count; k++)
			if (g_modules[k].effect_id == effect_id) return &g_modules[k];
		return nullptr;
	}
}
}

#define FX_PART(id) ((void)0)
#define FX_LAB_MODULE(fn) static_assert(true, "")
