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

// Cinematic GF engine: palette shift

	static inline void xl_PaletteToHsv_B67710(const void *src, void *dst, int32_t n) { x::f<void (__cdecl *)(const void *, void *, int32_t)>(game_addr(game::MAG_006_PaletteToHsv))(src, dst, n); }
	static inline void xl_HsvToPalette_B67810(const void *hsv, void *dst, int32_t n) { x::f<void (__cdecl *)(const void *, void *, int32_t)>(game_addr(game::MAG_006_HsvToPalette))(hsv, dst, n); }

	static inline void xl_QueueVramReadback(const void *rect, uint32_t dst) { x::f<void (__cdecl *)(const void *, uint32_t)>(game_addr(game::Battle_QueueVramReadback_Type2))(rect, dst); }

	static constexpr game::Id PS_RECTRING = game::g_GfCinematic_BlitRectRing;
	static constexpr game::Id PS_RECTRING_IDX = game::g_GfCinematic_BlitRectRingIdx;

	static void StoreRect_B66F90(const uint8_t *src, uint32_t dst)
	{
		uint8_t *slot = (uint8_t *)(game_addr(PS_RECTRING) + 8u * (MEM<uint8_t>(game_addr(PS_RECTRING_IDX)) & 0xFu));
		U16(slot, 0) = U16(src, 0);
		U16(slot, 2) = U16(src, 2);
		U16(slot, 4) = U16(src, 4);
		U16(slot, 6) = U16(src, 6);
		xl_QueueVramReadback(slot, dst);
		MEM<uint8_t>(game_addr(PS_RECTRING_IDX)) = (uint8_t)(MEM<uint8_t>(game_addr(PS_RECTRING_IDX)) + 1);
	}

	static void __cdecl op_035_VramReadback()
	{
		uint8_t *s = STREAM();
		if (U8(RT(), 0x4B) & 0x80)
		{
			int32_t slot = S16(s, 4);
			uint8_t *src = s + S16(s, 2);
			uint32_t dst = MEM<uint32_t>(game_addr(game::MAGIC_TEXTURE_BUFFER_PTR)) + ((uint32_t)slot << 4);
			STREAM() = s + 6;
			StoreRect_B66F90(src, dst);
			return;
		}
		uint8_t *src = s + 2;
		int32_t n = shl32(mul32(S16(s, 8), S16(s, 6)), 1);
		uint8_t *blk = blob::ArenaAlloc(n);
		PTR(CUR(), 0xC4) = blk;
		uint32_t dst = U32(CUR(), 0xC4);
		STREAM() = STREAM() + 10;
		StoreRect_B66F90(src, dst);
	}

	static void __cdecl dh_47_PaletteHsvShift()
	{
		uint8_t *cur = CUR();
		if (U32(cur, 0xBC) == 0)
		{
			const uint8_t *r = PTR(cur, 0xB8);
			const int32_t n = mul32(S16(r, 6), S16(r, 4));
			PTR(CUR(), 0xBC) = blob::ArenaAlloc(shl32(n, 3));
			xl_PaletteToHsv_B67710(PTR(CUR(), 0xC4), PTR(CUR(), 0xBC), n);
			cur = CUR();
		}
		const uint8_t *rect = PTR(cur, 0xB8);
		const int32_t n = mul32(S16(rect, 6), S16(rect, 4));
		S32(WS(), 0xF0) = S16(cur, 0x94);
		S32(WS(), 0xF8) = S16(CUR(), 0x96);
		S32(WS(), 0xFC) = S16(CUR(), 0x98);
		S32(WS(), 0xF4) = S16(CUR(), 0x8C);
		xl_HsvToPalette_B67810(PTR(CUR(), 0xBC), PTR(CUR(), 0xC4), n);
		const uint32_t c = AT<uint16_t>(PTR(CUR(), 0xC4), 2 * (S16(rect, 4) - 1));
		const uint8_t red = (uint8_t)(c << 3), green = (uint8_t)((c >> 5) << 3), blue = (uint8_t)((c >> 10) << 3);
		for (uint32_t i = 0; i < 2; i++)
		{
			uint8_t *w = (uint8_t *)(game_addr(game::unk_B8BA00) + 4 * i);
			U8(w, 0) = red;
			U8(w, 1) = green;
			U8(w, 2) = blue;
			U8(w, 3) = (uint8_t)(3 + i);
			uint8_t *ent = (uint8_t *)(game_addr(game::unk_1D974BC) + 0x9C * i);
			U32(ent, 0) = U32(w, 0);
			U8(ent, 3) = 0;
		}
	}
