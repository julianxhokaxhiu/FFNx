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

// Verifier of the native battle effects

#include "fx_port.h"
#include "fx_modules.h"
#include "../../../patch.h"
#include "../../../log.h"
#include "../../../cfg.h"

#include <windows.h>
#include <dbghelp.h>
#include <float.h>
#include <stdio.h>
#include <utility>

namespace ff8fx
{
namespace
{
	const uint32_t MISMATCH_LOG_MAX = 60;
	const uint32_t FAULTS_MAX = 3;

	int (__cdecl *g_tick_orig)(TaskQueue *) = nullptr;
	bool g_installed = false;

	enum Kind : uint8_t { K_REGION, K_OT, K_ARENA, K_QHEADER, K_QPOOL };
	enum Phase : uint8_t { PH_PRE, PH_A, PH_B };
	struct Entry
	{
		uint32_t addr, size, off;
		Kind kind;
		Phase phase;
		const char *name;
		TaskQueue *q;
		uint32_t node_size;
	};
	const int ENTRIES_MAX = 160;
	const uint32_t BUF_SIZE = 0x580000;
	const uint32_t POOL_MAX = 0x20000;
	const uint32_t POOLS_TOTAL_MAX = 0x80000;
	const uint32_t FRAME_WINDOW_MAX = 0x40000;
	Entry g_ent[ENTRIES_MAX];
	int g_nent = 0;
	uint32_t g_used = 0, g_pools_used = 0;
	uint8_t g_snap[BUF_SIZE], g_post[BUF_SIZE], g_scan[BUF_SIZE];
	bool g_incomplete = false;
	char g_incomplete_msg[192];

	Phase g_phase = PH_PRE;
	bool g_tracking = false;

	TaskQueue *g_known[32];
	int g_nknown = 0;

	void set_incomplete(const char *fmt, uint32_t a, uint32_t b)
	{
		if (g_incomplete) return;
		g_incomplete = true;
		_snprintf_s(g_incomplete_msg, sizeof(g_incomplete_msg), _TRUNCATE, fmt, a, b);
	}

	bool add_entry(uint32_t addr, uint32_t size, Kind kind, const char *name, TaskQueue *q = nullptr, uint32_t node_size = 0)
	{
		if (!size) return true;
		if (g_nent >= ENTRIES_MAX || g_used + size > BUF_SIZE)
		{
			set_incomplete("tracked memory full (range %08X, %X bytes)", addr, size);
			return false;
		}
		Entry &e = g_ent[g_nent++];
		e = { addr, size, g_used, kind, g_phase, name, q, node_size };
		copy_bytes(g_snap + g_used, (const void *)addr, size);
		g_used += size;
		return true;
	}

	bool has_entry(uint32_t addr, Kind kind)
	{
		for (int i = 0; i < g_nent; i++)
			if (g_ent[i].addr == addr && g_ent[i].kind == kind) return true;
		return false;
	}

	void note_known(TaskQueue *q)
	{
		if (g_phase == PH_B) return;
		for (int i = 0; i < g_nknown; i++) if (g_known[i] == q) return;
		if (g_nknown < 32) g_known[g_nknown++] = q;
	}

	inline uint32_t pool_size(const TaskQueue *q) { return (uint32_t)(uint16_t)q->node_size * (uint16_t)q->capacity; }

	void track_queue(TaskQueue *q, void *pool = nullptr, uint32_t size = 0, uint32_t node_size = 0)
	{
		if (!q) return;
		if (!has_entry((uint32_t)q, K_QHEADER))
		{
			if (IsBadReadPtr(q, sizeof(TaskQueue))) { set_incomplete("task queue %08X unreadable%.0u", (uint32_t)q, 0); return; }
			add_entry((uint32_t)q, sizeof(TaskQueue), K_QHEADER, "task queue header", q);
		}
		note_known(q);
		if (!pool) { pool = q->pool; size = pool_size(q); node_size = (uint16_t)q->node_size; }
		if (!pool || !size || has_entry((uint32_t)pool, K_QPOOL)) return;
		if (size > POOL_MAX || g_pools_used + size > POOLS_TOTAL_MAX || IsBadReadPtr(pool, size))
		{
			set_incomplete("task pool %08X (%X bytes) not trackable", (uint32_t)pool, size);
			return;
		}
		if (add_entry((uint32_t)pool, size, K_QPOOL, "task pool", q, node_size)) g_pools_used += size;
	}

	uint32_t *crt_seed() { return (uint32_t *)(((uint8_t *(__cdecl *)())game_addr(mod::CRT_TLS_FN))() + 0x14); }

	const game::Id MAGIC_BUFFER_STATIC = game::MAGIC_TEXTURE_BUFFER_BASE;
	uint32_t magic_buffer() { return (uint32_t)((uint8_t *(__cdecl *)())game_addr(game::Magic_TextureOFF_ToEAX1))(); }
	uint32_t magic_buffer_size() { return ((uint32_t(__cdecl *)())game_addr(game::sub_571B60))(); }

	bool build_entries(const mod::Module &m)
	{
		g_nent = 0; g_used = 0; g_pools_used = 0;
		g_incomplete = false; g_incomplete_msg[0] = 0;
		g_phase = PH_PRE;
		for (int i = 0; i < mod::engine_region_count; i++)
		{
			uint32_t addr = game_addr(mod::engine_regions[i].addr), size = mod::engine_regions[i].size;
			if (addr == game_addr(MAGIC_BUFFER_STATIC)) { addr = magic_buffer(); size = magic_buffer_size(); }
			add_entry(addr, size, K_REGION, mod::engine_regions[i].name);
		}
		add_entry(game_addr(m.data_lo), game_addr(m.data_hi) - game_addr(m.data_lo), K_REGION, "module globals");
		if (m.extra) add_entry(game_addr(m.extra), m.extra_size, K_REGION, "module extra cell");
		for (int i = 0; i < m.nstreams; i++) add_entry(game_addr(m.streams[i].addr), m.streams[i].size, K_REGION, "module stream/cell");
		uint32_t ot = *(uint32_t *)game_addr(mod::RENDER_LIST_BASE_PTR);
		if (ot) add_entry(ot, mod::OT_SPAN_WORDS * 4, K_OT, "ordering table");
		add_entry(game_addr(mod::SSIGPU_ARENA), mod::SSIGPU_ARENA_SIZE, K_ARENA, "SSIGPU execution arena");
		uint32_t fcur = *(uint32_t *)game_addr(mod::FRAME_PACKET_CUR_PTR), fend = *(uint32_t *)game_addr(mod::FRAME_PACKET_END_PTR);
		if (fcur && fend > fcur && fend - fcur <= FRAME_WINDOW_MAX && !IsBadReadPtr((void *)fcur, fend - fcur))
			add_entry(fcur, fend - fcur, K_REGION, "frame packet arena");
		uint32_t scratch = *(uint32_t *)game_addr(mod::FIELD_ALLOC_PTR);
		if (scratch && !IsBadReadPtr((void *)scratch, mod::FIELD_ALLOC_SCRATCH))
			add_entry(scratch, mod::FIELD_ALLOC_SCRATCH, K_REGION, "Field_Alloc scratch");
		add_entry((uint32_t)crt_seed(), 4, K_REGION, "CRT rand seed");
		for (int i = 0; i < g_nknown; i++) track_queue(g_known[i]);
		return !g_incomplete;
	}

	void restore_snapshot()
	{
		for (int i = g_nent - 1; i >= 0; i--) copy_bytes((void *)g_ent[i].addr, g_snap + g_ent[i].off, g_ent[i].size);
	}

	void save_post()
	{
		for (int i = 0; i < g_nent; i++) copy_bytes(g_post + g_ent[i].off, (const void *)g_ent[i].addr, g_ent[i].size);
	}

	inline const uint8_t *reference(const Entry &e) { return (e.phase == PH_B ? g_snap : g_post) + e.off; }

	void put_back_A()
	{
		for (int i = 0; i < g_nent; i++) if (g_ent[i].phase == PH_B) copy_bytes((void *)g_ent[i].addr, g_snap + g_ent[i].off, g_ent[i].size);
		for (int i = 0; i < g_nent; i++) if (g_ent[i].phase != PH_B) copy_bytes((void *)g_ent[i].addr, g_post + g_ent[i].off, g_ent[i].size);
	}

	inline unsigned int x87_cw()
	{
		unsigned int cw = 0;
		__control87_2(0, 0, &cw, nullptr);
		return cw;
	}
	inline void x87_set_cw(unsigned int cw)
	{
		__control87_2(cw, _MCW_EM | _MCW_IC | _MCW_RC | _MCW_PC, nullptr, nullptr);
	}
	inline void x87_reset()
	{
		_fpreset();
	}

	__declspec(noinline) void scrub_stack()
	{
		volatile uint8_t buf[0x10000];
		SecureZeroMemory((void *)buf, sizeof(buf));
	}

	enum { EXT_OFF = 0, EXT_RECORD, EXT_REPLAY };
	enum { OBS_NONE = 0, OBS_ADD_TASK, OBS_INIT_POOL };
	struct Site { uint32_t addr; int arity; const char *name; int observe; void *stub; uint8_t saved[5]; bool patched; };
	struct Call { int site; uint32_t args[6]; uint32_t ret; uint32_t w_off, w_len; unsigned int cw_after; };
	const int SITES_MAX = 128;
	const int CALLS_MAX = 256;
	const uint32_t WLOG_SIZE = 0x100000;
	Site g_sites[SITES_MAX];
	int g_nsites = 0;
	int g_mode = EXT_OFF, g_depth = 0;
	Call g_calls[CALLS_MAX];
	int g_ncalls = 0, g_replay_i = 0;
	bool g_calls_overflow = false;
	uint8_t g_wlog[WLOG_SIZE];
	uint32_t g_wlog_used = 0;
	bool g_wlog_overflow = false;
	char g_call_msg[256];

	uint32_t site_call(int site, uint32_t *args);
	template<int I> uint32_t __cdecl site_stub(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5)
	{
		uint32_t a[6] = { a0, a1, a2, a3, a4, a5 };
		return site_call(I, a);
	}
	template<int... I> void fill_stubs(void **out, std::integer_sequence<int, I...>)
	{
		((out[I] = (void *)&site_stub<I>), ...);
	}

	void patch_site(Site &s, bool on)
	{
		if (s.patched == on) return;
		uint8_t *code = (uint8_t *)s.addr;
		DWORD old;
		if (!VirtualProtect(code, 5, PAGE_EXECUTE_READWRITE, &old)) return;
		if (on)
		{
			ff8fx::copy<(5)>(s.saved, code);
			code[0] = 0xE9;
			*(int32_t *)(code + 1) = (int32_t)((uint8_t *)s.stub - (code + 5));
		}
		else ff8fx::copy<(5)>(code, s.saved);
		VirtualProtect(code, 5, old, &old);
		FlushInstructionCache(GetCurrentProcess(), code, 5);
		s.patched = on;
	}

	void patch_all(bool on)
	{
		for (int i = 0; i < g_nsites; i++) patch_site(g_sites[i], on);
	}

	uint32_t call_through(int site, const uint32_t *a)
	{
		Site &s = g_sites[site];
		patch_site(s, false);
		uint32_t r = ((uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t))s.addr)(a[0], a[1], a[2], a[3], a[4], a[5]);
		patch_site(s, true);
		return r;
	}

	void scan_copy()
	{
		for (int i = 0; i < g_nent; i++) copy_bytes(g_scan + g_ent[i].off, (const void *)g_ent[i].addr, g_ent[i].size);
	}

	uint32_t diff_to_wlog()
	{
		uint32_t start = g_wlog_used;
		for (int r = 0; r < g_nent; r++)
		{
			const uint8_t *cur = (const uint8_t *)g_ent[r].addr, *pre = g_scan + g_ent[r].off;
			uint32_t size = g_ent[r].size;
			for (uint32_t i = 0; i < size;)
			{
				uint32_t blk = size - i < 64 ? size - i : 64;
				if (ff8fx::same_bytes(cur + i, pre + i, blk)) { i += blk; continue; }
				uint32_t a = i;
				while (a < size && cur[a] == pre[a]) a++;
				uint32_t b = a;
				while (b < size && (cur[b] != pre[b] || (b + 1 < size && cur[b + 1] != pre[b + 1]))) b++;
				if (g_wlog_used + 8 + (b - a) > WLOG_SIZE) { g_wlog_overflow = true; return g_wlog_used - start; }
				*(uint32_t *)(g_wlog + g_wlog_used) = g_ent[r].addr + a;
				*(uint32_t *)(g_wlog + g_wlog_used + 4) = b - a;
				copy_bytes(g_wlog + g_wlog_used + 8, cur + a, b - a);
				g_wlog_used += 8 + (b - a);
				i = b;
			}
		}
		return g_wlog_used - start;
	}

	uint32_t site_call(int site, uint32_t *args)
	{
		const Site &s = g_sites[site];
		if (s.observe)
		{
			if (g_tracking && !(g_mode == EXT_RECORD && g_depth > 0))
			{
				TaskQueue *q = (TaskQueue *)args[0];
				if (s.observe == OBS_ADD_TASK) track_queue(q);
				else track_queue(q, (void *)args[1], args[2] * args[3], args[2]);
			}
			return call_through(site, args);
		}
		if (g_mode == EXT_RECORD)
		{
			if (g_depth > 0) return call_through(site, args);
			if (g_ncalls >= CALLS_MAX) { g_calls_overflow = true; return call_through(site, args); }
			g_depth++;
			scan_copy();
			uint32_t r = call_through(site, args);
			Call &c = g_calls[g_ncalls++];
			c.site = site;
			ff8fx::copy<(sizeof(c.args))>(c.args, args);
			c.ret = r;
			c.w_off = g_wlog_used;
			c.w_len = diff_to_wlog();
			c.cw_after = x87_cw();
			g_depth--;
			return r;
		}
		if (g_mode == EXT_REPLAY)
		{
			if (g_replay_i >= g_ncalls)
			{
				if (!g_call_msg[0])
					_snprintf_s(g_call_msg, sizeof(g_call_msg), _TRUNCATE, "extra call %s(%08X, %08X, %08X) #%d", s.name, args[0], args[1], args[2], g_replay_i);
				g_replay_i++;
				return 0;
			}
			const Call &c = g_calls[g_replay_i++];
			bool same = c.site == site;
			for (int i = 0; same && i < s.arity; i++) same = c.args[i] == args[i];
			if (!same && !g_call_msg[0])
				_snprintf_s(g_call_msg, sizeof(g_call_msg), _TRUNCATE, "call #%d: original %s(%08X, %08X, %08X), port %s(%08X, %08X, %08X)",
					g_replay_i - 1, g_sites[c.site].name, c.args[0], c.args[1], c.args[2], s.name, args[0], args[1], args[2]);
			x87_set_cw(c.cw_after);
			for (uint32_t o = c.w_off; o < c.w_off + c.w_len;)
			{
				uint32_t addr = *(uint32_t *)(g_wlog + o), len = *(uint32_t *)(g_wlog + o + 4);
				copy_bytes((void *)addr, g_wlog + o + 8, len);
				o += 8 + len;
			}
			return c.ret;
		}
		return call_through(site, args);
	}

	void on_queue_seen(TaskQueue *q)
	{
		if (!g_tracking || (g_mode == EXT_RECORD && g_depth > 0)) return;
		track_queue(q);
	}

	int describe_memory_diff(char *msg, size_t n)
	{
		int first = -1, ndiff_entries = 0;
		uint32_t i = 0;
		for (int r = 0; r < g_nent; r++)
		{
			const Entry &e = g_ent[r];
			const uint8_t *cur = (const uint8_t *)e.addr, *ref = reference(e);
			if (ff8fx::same_bytes(cur, ref, e.size)) continue;
			ndiff_entries++;
			if (first >= 0) continue;
			first = r;
			while (cur[i] == ref[i]) i++;
		}
		if (first < 0) return 0;
		const Entry &e = g_ent[first];
		const uint8_t *cur = (const uint8_t *)e.addr, *ref = reference(e);
		uint32_t ndiff = 0;
		for (uint32_t k = i; k < e.size; k++) if (cur[k] != ref[k]) ndiff++;
		uint32_t w0 = i & ~3u, addr = e.addr + i;
		bool word = w0 + 4 <= e.size;
		int o = _snprintf_s(msg, n, _TRUNCATE, "%s %08X+%X = %08X: original %08X, port %08X (%u bytes differ in it, %d ranges differ)",
			e.name, e.addr, i, addr, word ? *(const uint32_t *)(ref + w0) : ref[i], word ? *(const uint32_t *)(cur + w0) : cur[i], ndiff, ndiff_entries);
		if (o < 0) return (int)strlen(msg);
		int k = 0;
		if (e.kind == K_OT)
			k = _snprintf_s(msg + o, n - o, _TRUNCATE, " [bucket %d]", (int)(i / 4) - 17);
		else if (e.kind == K_ARENA)
			k = _snprintf_s(msg + o, n - o, _TRUNCATE, " [OT node %u +%u]", i / 24, i % 24);
		else if (e.kind == K_QHEADER)
			k = _snprintf_s(msg + o, n - o, _TRUNCATE, " [queue %p: original head %08X tail %08X, port head %08X tail %08X]", e.q,
				*(const uint32_t *)ref, *(const uint32_t *)(ref + 4), *(const uint32_t *)cur, *(const uint32_t *)(cur + 4));
		if (k < 0) return (int)strlen(msg);
		o += k;
		for (int r = 0; r < g_nent; r++)
		{
			const Entry &p = g_ent[r];
			if (p.kind != K_QPOOL || !p.node_size || addr < p.addr || addr >= p.addr + p.size) continue;
			uint32_t node = (addr - p.addr) / p.node_size, noff = (addr - p.addr) % p.node_size;
			uint32_t fn = p.node_size >= 12 ? *(const uint32_t *)(p.addr + node * p.node_size + 8) : 0;
			const char *pn = port_name(fn);
			k = _snprintf_s(msg + o, n - o, _TRUNCATE, " [queue %p node %u +%X, task %08X%s%s]", p.q, node, noff, fn, pn ? " = " : "", pn ? pn : "");
			if (k > 0) o += k;
			break;
		}
		return o;
	}

	void adopt_unread_garbage()
	{
		static constexpr game::Id words[] = { game::unk_1CA928E, game::unk_1CA8A16, game::unk_1CA8A1E, game::unk_1CA8A26, game::unk_B8B7F6, game::unk_B8B7FE };
		for (game::Id w : words)
			for (int i = 0; i < g_nent; i++)
			{
				const uint32_t a = game_addr(w);
				const Entry &e = g_ent[i];
				if (a >= e.addr && a + 2 <= e.addr + e.size)
				{
					*(uint16_t *)a = *(const uint16_t *)(reference(e) + (a - e.addr));
					break;
				}
			}
	}

	const char *compare(int ra, int rb)
	{
		static char msg[1024];
		if (ra != rb) { _snprintf_s(msg, sizeof(msg), _TRUNCATE, "queue return: original %d, port %d", ra, rb); return msg; }
		if (g_call_msg[0]) { _snprintf_s(msg, sizeof(msg), _TRUNCATE, "external %s", g_call_msg); return msg; }
		if (g_replay_i != g_ncalls)
		{
			_snprintf_s(msg, sizeof(msg), _TRUNCATE, "external calls: original made %d, port %d (next expected %s)", g_ncalls, g_replay_i,
				g_replay_i < g_ncalls ? g_sites[g_calls[g_replay_i].site].name : "-");
			return msg;
		}
		if (g_incomplete) { _snprintf_s(msg, sizeof(msg), _TRUNCATE, "port run touched untracked memory: %s", g_incomplete_msg); return msg; }
		adopt_unread_garbage();
		return describe_memory_diff(msg, sizeof(msg)) > 0 ? msg : nullptr;
	}

	uint32_t g_fault_code = 0, g_fault_addr = 0;
	int fault_filter(EXCEPTION_POINTERS *ep)
	{
		g_fault_code = ep->ExceptionRecord->ExceptionCode;
		g_fault_addr = (uint32_t)ep->ExceptionRecord->ExceptionAddress;
		return EXCEPTION_EXECUTE_HANDLER;
	}
	bool run_guarded(TaskQueue *q, int *r)
	{
		__try { *r = g_tick_orig(q); }
		__except (fault_filter(GetExceptionInformation())) { return false; }
		return true;
	}

	void log_all_diffs(const char *mname, uint32_t tick)
	{
		int shown = 0;
		for (int r = 0; r < g_nent && shown < 8; r++)
		{
			const Entry &e = g_ent[r];
			const uint8_t *cur = (const uint8_t *)e.addr, *ref = reference(e);
			if (ff8fx::same_bytes(cur, ref, e.size)) continue;
			uint32_t i = 0, n = 0;
			while (cur[i] == ref[i]) i++;
			for (uint32_t k = i; k < e.size; k++) if (cur[k] != ref[k]) n++;
			char a[64], b[64];
			int ao = 0, bo = 0;
			for (uint32_t k = i & ~3u; k < (i & ~3u) + 16 && k < e.size; k++)
			{
				ao += _snprintf_s(a + ao, sizeof(a) - ao, _TRUNCATE, "%02X", ref[k]);
				bo += _snprintf_s(b + bo, sizeof(b) - bo, _TRUNCATE, "%02X", cur[k]);
			}
			ffnx_error("FF8 battle fx: %s tick %u   differs: %s %08X+%X at +%X (%08X), %u bytes | original %s | port %s\n",
				mname, tick, e.name, e.addr, e.size, i, e.addr + i, n, a, b);
			shown++;
		}
	}

	const int FULL_DUMPS_MAX = 3;
	int g_full_dumps = 0;

	void write_state_dump(const mod::Module &m, int effect_id, uint32_t tick, TaskQueue *q)
	{
		CreateDirectoryA("ffnx_fx_dumps", nullptr);
		char base[MAX_PATH], path[MAX_PATH];
		_snprintf_s(base, sizeof(base), _TRUNCATE, "ffnx_fx_dumps\\%s_e%d_t%u", m.name, effect_id, tick);
		for (char *c = base + 14; *c; c++) if (*c == ' ' || *c == '(' || *c == ')') *c = '_';
		_snprintf_s(path, sizeof(path), _TRUNCATE, "%s.bin", base);
		FILE *f = nullptr;
		if (fopen_s(&f, path, "wb") == 0 && f)
		{
			uint32_t hdr[4] = { 0x31445846, (uint32_t)g_nent, (uint32_t)(uintptr_t)q, tick };
			fwrite(hdr, sizeof(hdr), 1, f);
			for (int i = 0; i < g_nent; i++)
			{
				const Entry &e = g_ent[i];
				uint32_t w[4] = { e.addr, e.size, (uint32_t)e.phase, (uint32_t)e.kind };
				char nm[32] = {};
				strncpy_s(nm, e.name ? e.name : "", _TRUNCATE);
				fwrite(w, sizeof(w), 1, f);
				fwrite(nm, sizeof(nm), 1, f);
			}
			for (int i = 0; i < g_nent; i++) fwrite(g_snap + g_ent[i].off, g_ent[i].size, 1, f);
			for (int i = 0; i < g_nent; i++) fwrite(reference(g_ent[i]), g_ent[i].size, 1, f);
			for (int i = 0; i < g_nent; i++) fwrite((const void *)g_ent[i].addr, g_ent[i].size, 1, f);
			fclose(f);
			ffnx_error("FF8 battle fx: %s tick %u: state written to %s\n", m.name, tick, path);
		}
		if (g_full_dumps >= FULL_DUMPS_MAX) return;
		g_full_dumps++;
		restore_snapshot();
		_snprintf_s(path, sizeof(path), _TRUNCATE, "%s.dmp", base);
		HANDLE h = CreateFileA(path, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (h != INVALID_HANDLE_VALUE)
		{
			BOOL ok = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), h,
				(MINIDUMP_TYPE)(MiniDumpWithFullMemory | MiniDumpWithFullMemoryInfo), nullptr, nullptr, nullptr);
			CloseHandle(h);
			ffnx_error("FF8 battle fx: %s tick %u: pre-tick memory dump %s%s\n", m.name, tick, path, ok ? "" : " FAILED");
		}
	}

	struct Summon
	{
		const mod::Module *m;
		TaskQueue *q;
		int effect_id;
		uint32_t ticks, verified, matched, mismatched, first_bad, skipped, faults, logged, original_only, detailed;
		bool refused, dumped;
	};
	Summon g_s = {};

	void summon_end()
	{
		if (g_s.m && g_s.ticks)
			ffnx_info("FF8 battle fx: %s (effect %d) ended: ticks=%u verified=%u matched=%u mismatched=%u (first at tick %u) skipped=%u faults=%u original-only=%u\n",
				g_s.m->name, g_s.effect_id, g_s.ticks, g_s.verified, g_s.matched, g_s.mismatched, g_s.first_bad, g_s.skipped, g_s.faults, g_s.original_only);
		g_s = {};
		g_nknown = 0;
	}

	void summon_begin(const mod::Module *m, TaskQueue *q, int effect_id)
	{
		g_s = {};
		g_s.m = m; g_s.q = q; g_s.effect_id = effect_id;
		g_nknown = 0;
		g_s.refused = !mod::fits(*m);
		if (g_s.refused)
			ffnx_error("FF8 battle fx: %s (effect %d) does not fit the snapshot (globals %X bytes, streams %X bytes): original code only\n",
				m->name, effect_id, game_addr(m->data_hi) - game_addr(m->data_lo), mod::streams_bytes(*m));
		else
			ffnx_info("FF8 battle fx: verifying %s (effect %d), queue %p\n", m->name, effect_id, q);
	}

	int run_original_only(TaskQueue *q)
	{
		bool act = g_active;
		set_ports(false);
		int r = g_tick_orig(q);
		set_ports(act);
		return r;
	}

	int verified_tick(TaskQueue *q, const mod::Module &m)
	{
		g_s.verified++;
		uint32_t tick = g_s.ticks + 1;
		if (!build_entries(m))
		{
			g_s.skipped++;
			if (g_s.logged < MISMATCH_LOG_MAX) { g_s.logged++; ffnx_error("FF8 battle fx: %s tick %u not verifiable: %s\n", m.name, tick, g_incomplete_msg); }
			return run_original_only(q);
		}

		g_ncalls = 0; g_calls_overflow = false;
		g_wlog_used = 0; g_wlog_overflow = false;
		g_phase = PH_A;
		g_tracking = true;
		g_queue_seen = on_queue_seen;
		g_mode = EXT_RECORD;
		g_depth = 0;
		bool act = g_active;
		set_ports(false);
		patch_all(true);
		scrub_stack();
		unsigned int cw_a0 = x87_cw();
		int ra = g_tick_orig(q);
		unsigned int cw_a1 = x87_cw();
		patch_all(false);
		g_mode = EXT_OFF;
		g_queue_seen = nullptr;
		g_tracking = false;
		set_ports(act);
		save_post();
		if (g_wlog_overflow || g_calls_overflow || g_incomplete)
		{
			g_s.skipped++;
			if (g_s.logged < MISMATCH_LOG_MAX)
			{
				g_s.logged++;
				ffnx_error("FF8 battle fx: %s tick %u not verifiable: %s\n", m.name, tick,
					g_incomplete ? g_incomplete_msg : g_wlog_overflow ? "external call write log full" : "too many external calls");
			}
			g_phase = PH_PRE;
			return ra;
		}

		restore_snapshot();
		g_phase = PH_B;
		g_replay_i = 0;
		g_call_msg[0] = 0;
		g_tracking = true;
		g_queue_seen = on_queue_seen;
		g_mode = EXT_REPLAY;
		set_ports(true);
		patch_all(true);
		scrub_stack();
		x87_set_cw(cw_a0);
		int rb = 0;
		bool ok = run_guarded(q, &rb);
		if (!ok) x87_reset();
		unsigned int cw_b1 = x87_cw();
		x87_set_cw(cw_a1);
		patch_all(false);
		g_mode = EXT_OFF;
		g_queue_seen = nullptr;
		g_tracking = false;
		set_ports(act);
		g_phase = PH_PRE;

		const char *diff = ok ? compare(ra, rb) : "port FAULTED";
		if (!ok)
		{
			g_s.faults++;
			ffnx_error("FF8 battle fx: %s tick %u: port fault %08X at %08X (fault %u)%s\n", m.name, tick, g_fault_code, g_fault_addr, g_s.faults,
				g_s.faults >= FAULTS_MAX ? " - original code only for the rest of this summon" : "");
		}
		if (!diff)
		{
			g_s.matched++;
			return rb;
		}
		g_s.mismatched++;
		if (!g_s.first_bad) g_s.first_bad = tick;
		if (ok && g_s.logged < MISMATCH_LOG_MAX)
		{
			g_s.logged++;
			char calls[200];
			int co = 0;
			calls[0] = 0;
			for (int i = 0; i < g_ncalls && co >= 0 && co < 180; i++)
				co += _snprintf_s(calls + co, sizeof(calls) - co, _TRUNCATE, " %s", g_sites[g_calls[i].site].name);
			ffnx_error("FF8 battle fx: %s tick %u MISMATCH: %s | return %d/%d, x87 cw A %04X->%04X B end %04X, calls:%s\n",
				m.name, tick, diff, ra, rb, cw_a0, cw_a1, cw_b1, calls[0] ? calls : " none");
			if (g_s.detailed < 3) { g_s.detailed++; log_all_diffs(m.name, tick); }
		}
		if (ok && ff8_battle_fx_dump && !g_s.dumped) { g_s.dumped = true; write_state_dump(m, g_s.effect_id, tick, q); }
		put_back_A();
		return ra;
	}

	int __cdecl effect_tick(TaskQueue *q)
	{
		int id = *(int *)game_addr(mod::EFFECT_ID_PTR) + 1;
		const mod::Module *m = mod::find_module(id);
		if (!m || !module_ported(id))
		{
			if (g_s.m) summon_end();
			return g_tick_orig(q);
		}
		if (g_s.m != m || g_s.q != q) { summon_end(); summon_begin(m, q, id); }
		int r;
		if (g_s.refused || g_s.faults >= FAULTS_MAX)
		{
			r = run_original_only(q);
			g_s.original_only++;
		}
		else r = verified_tick(q, *m);
		g_s.ticks++;
		if (r == 0) summon_end();
		return r;
	}
}

	void verify_install()
	{
		if (g_installed) return;
		g_installed = true;
		void *stubs[SITES_MAX];
		fill_stubs(stubs, std::make_integer_sequence<int, SITES_MAX>{});
		g_nsites = 0;
		g_sites[g_nsites++] = { game_addr(game::AddTaskToQueue), 0, "AddTaskToQueue", OBS_ADD_TASK };
		g_sites[g_nsites++] = { game_addr(game::BdLink_InitTaskQueuePool), 0, "InitTaskQueuePool", OBS_INIT_POOL };
		for (int i = 0; i < mod::ext_site_count && g_nsites < SITES_MAX; i++)
			g_sites[g_nsites++] = { game_addr(mod::ext_sites[i].addr), mod::ext_sites[i].arity, mod::ext_sites[i].name, OBS_NONE };
		if (g_nsites - 2 < mod::ext_site_count)
			ffnx_error("FF8 battle fx: the verifier has room for %d external sites, %d listed\n", SITES_MAX - 2, mod::ext_site_count);
		for (int i = 0; i < g_nsites; i++) g_sites[i].stub = stubs[i];

		g_tick_orig = (int (__cdecl *)(TaskQueue *))get_relative_call(game_addr(game::sub_50093A), 0);
		replace_call(game_addr(game::sub_50093A), (void *)effect_tick);
		ffnx_info("FF8 battle fx: differential verifier installed (%d modules, %d external sites)\n", mod::module_count, mod::ext_site_count);
	}
}
