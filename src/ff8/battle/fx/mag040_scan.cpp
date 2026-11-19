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

// Effect 40: Scan

#include "mag_common.h"

namespace ff8fx
{
namespace scan040
{
	using namespace eng;
	using namespace magc;

#pragma pack(push, 1)
	struct FxNode
	{
		TaskNode hdr;
		CastContext *ctx;
		FxNode *root;
		FxNode *emitter;
		FxNode *parent;
		int16_t pos[4];
		int16_t counter;
		uint16_t flags;
		uint8_t children;
		int8_t phase;
		int8_t action;
		int8_t target;
		uint8_t attacker;
		uint8_t slot;
		uint8_t line;
		uint8_t last;
	};
	struct RootNode
	{
		FxNode h;
		uint8_t bones[0x28];
		int16_t last_action;
		int16_t target_count;
		int16_t tick;
		int16_t alive;
		uint8_t pad60[3];
		uint8_t busy;
	};
	struct EmitterNode
	{
		FxNode h;
		uint8_t anchor[0x18];
		int16_t bmin[4];
		int16_t bmax[4];
	};
	struct CtlNode
	{
		FxNode h;
		int32_t scale[3];
		uint32_t pad3C;
		int16_t rot[3];
		int16_t pad46;
		int16_t dx, dy;
		uint32_t pad4C;
		uint32_t shape;
		int16_t offx;
		int16_t pad56;
		int16_t offz;
		int16_t pad5A;
		uint32_t pad;
		uint32_t prev;
		int16_t size;
		int16_t timer;
		int16_t mode;
		int16_t lines;
		int16_t line;
		int16_t fade;
		int16_t moved;
		int16_t idle;
		int16_t index;
		int16_t pad76;
	};
	struct ModelNode
	{
		FxNode h;
		int32_t scale[3];
		uint32_t pad3C;
		uint8_t rgb[4];
		int16_t angle[3];
		int16_t pad4A;
		uint32_t model;
		int16_t fade;
		int16_t pad52;
		uint32_t morph_a;
		uint32_t morph_b;
		int16_t fade1;
		int16_t fade2;
		int16_t table;
		int16_t which;
		int16_t max;
		int16_t timer;
		int16_t wave;
		int16_t blend;
	};
	struct SpriteNode
	{
		FxNode h;
		uint8_t pad30[0x1C];
		uint32_t seq;
		int16_t frame;
		int16_t last;
		uint32_t pad54;
	};
	struct CamState
	{
		int16_t eye[4];
		int16_t look[4];
		int16_t look0[4];
		int16_t look1[4];
		uint32_t angles[2];
		uint32_t place[2];
		int16_t t;
		int16_t step;
		int16_t yaw, yaw0, yaw1;
		int16_t dist, dist0, dist1;
		int16_t h, h0, h1;
		int16_t proj, proj0, proj1;
		int16_t cls;
	};
#pragma pack(pop)
	static_assert(sizeof(FxNode) == 0x30 && sizeof(RootNode) == 0x64 && sizeof(EmitterNode) == 0x58, "Scan nodes");
	static_assert(sizeof(CtlNode) == 0x78 && sizeof(ModelNode) == 0x6C && sizeof(SpriteNode) == 0x58, "Scan nodes");
	static_assert(sizeof(CamState) == 0x4E, "Scan camera state");

	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::g_Scan_PacketCursor)); }
	inline TaskQueue *QEmitters() { return (TaskQueue *)game_addr(game::MAG_040_EmitterQueue); }
	inline TaskQueue *QControls() { return (TaskQueue *)game_addr(game::MAG_040_ControlQueue); }
	inline TaskQueue *QSprites() { return (TaskQueue *)game_addr(game::MAG_040_SpriteQueue); }
	inline TaskQueue *QModels() { return (TaskQueue *)game_addr(game::MAG_040_ModelQueue); }
	inline CamState &Cam() { return **(CamState **)game_addr(game::unk_15CDDE8); }
	inline int32_t &Scanned(int k) { return ((int32_t *)game_addr(game::g_Scan_ScannedEntity))[k]; }
	inline uint32_t &SavedWord(int k) { return ((uint32_t *)game_addr(game::g_Scan_SavedEntityWord))[k]; }
	inline uint32_t &SavedColour(int k) { return ((uint32_t *)game_addr(game::g_Scan_SavedEntityColour))[k]; }
	inline int16_t &HudActive() { return var<int16_t>(game_addr(game::g_Scan_HudActive)); }
	inline int16_t &Closing() { return var<int16_t>(game_addr(game::g_Scan_Closing)); }
	inline int16_t &Confirmed() { return var<int16_t>(game_addr(game::g_Scan_Confirmed)); }
	inline int16_t &TextDone() { return var<int16_t>(game_addr(game::g_Scan_TextDone)); }
	inline uint8_t *MainEntity() { return var<uint8_t *>(game_addr(game::g_Scan_MainEntity)); }
	inline int16_t &InW(uint32_t addr) { return var<int16_t>(addr); }
	inline uint32_t LinesOT() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x14; }

	static constexpr game::Id ORIG_Root = game::MAG_040_SCAN_Tick, ORIG_Emitter = game::MAG_040_SCAN_Emitter_Tick, ORIG_Scanner = game::MAG_040_SCAN_Scanner_Tick, ORIG_AltScanner = game::MAG_040_SCAN_ShortScanner_Tick;
	static constexpr game::Id ORIG_Reticle = game::MAG_040_SCAN_Reticle_Tick, ORIG_Pair = game::MAG_040_SCAN_ModelPair_Tick, ORIG_Morph = game::MAG_040_SCAN_MorphModel_Tick, ORIG_Corner = game::MAG_040_SCAN_CornerBracket_Tick;
	static constexpr game::Id ORIG_Bracket = game::MAG_040_SCAN_SlidingBracket_Tick, ORIG_Ring = game::MAG_040_SCAN_Ring_Tick, ORIG_Sprite = game::MAG_040_SCAN_Sprite_Tick, ORIG_Camera = game::MAG_040_SCAN_Camera_Tick;
	static constexpr game::Id ORIG_Dimmer = game::MAG_040_SCAN_Dimmer_Tick, ORIG_Bars = game::MAG_040_SCAN_ScanBars_Tick, ORIG_Spinner = game::MAG_040_SCAN_Spinner_Tick, ORIG_Board = game::MAG_040_SCAN_StatBoard_Tick;
	static constexpr game::Id ORIG_StatBar = game::MAG_040_SCAN_StatBar_Tick, ORIG_Viewer = game::MAG_040_SCAN_Viewer_Tick, ORIG_Backdrop = game::MAG_040_SCAN_Backdrop_Tick, ORIG_Text = game::MAG_040_SCAN_TextPager_Tick;
	static constexpr game::Id ORIG_Restorer = game::MAG_040_SCAN_Restorer_Tick, ORIG_CamRestore = game::MAG_040_SCAN_CameraReturn_Tick, ORIG_AltSpinner = game::MAG_040_SCAN_ShortSpinner_Tick;
	static constexpr game::Id ORIG_AltBoard = game::MAG_040_SCAN_ShortBoard_Tick, ORIG_AltText = game::MAG_040_SCAN_ShortTextPager_Tick;

	static const void *SOUND_Scan() { return (const void *)game_addr(game::unk_15D0B3C); }
	static constexpr game::Id INFO_TABLE = game::g_Scan_CameraRows;
	static constexpr game::Id RETICLE_PATH = game::g_Scan_ReticlePath;

	inline FxNode *AddEffectTask(TaskQueue *q, uint32_t task, int32_t size, void *parent) { return fn<FxNode *(__cdecl *)(TaskQueue *, uint32_t, int32_t, void *)>(game_addr(game::Effect_AddTaskAndInitFromCtx))(q, task, size, parent); }
	inline void ReleaseLinkedTask(void *n) { fn<void (__cdecl *)(void *)>(game_addr(game::Effect_ReleaseLinkedTask))(n); }
	inline void EmitterUpdatePos(void *n) { fn<void (__cdecl *)(void *)>(game_addr(game::MAG_001_CURE_Emitter_UpdatePos))(n); }
	inline void ComputeModelBounds(void *n) { fn<void (__cdecl *)(void *)>(game_addr(game::MAG_001_CURE_Emitter_ComputeModelBounds))(n); }
	inline void UpdateTargetPosFromBones(void *n) { fn<void (__cdecl *)(void *)>(game_addr(game::Effect_UpdateTargetPosFromBones))(n); }
	inline void CopyMidpointFromSource(void *n, int16_t *out) { fn<void (__cdecl *)(void *, int16_t *)>(game_addr(game::Effect_CopyMidpointFromSource))(n, out); }
	inline void IdentityMatrix(Mat4x3 *m) { fn<void (__cdecl *)(Mat4x3 *)>(game_addr(game::Mtx_Identity))(m); }
	inline void RotateX(Mat4x3 *m, int32_t a) { fn<void (__cdecl *)(Mat4x3 *, int32_t)>(game_addr(game::Mtx_RotX))(m, a); }
	inline void RotateY(Mat4x3 *m, int32_t a) { fn<void (__cdecl *)(Mat4x3 *, int32_t)>(game_addr(game::Mtx_RotY))(m, a); }
	inline void RotateZ(Mat4x3 *m, int32_t a) { fn<void (__cdecl *)(Mat4x3 *, int32_t)>(game_addr(game::Mtx_RotZ))(m, a); }
	inline void MatrixMulVector(const Mat4x3 *m, const int16_t *v, int16_t *out) { fn<void (__cdecl *)(const Mat4x3 *, const int16_t *, int16_t *)>(game_addr(game::matrixMultiplyVector))(m, v, out); }
	inline int32_t ComputeCos(int32_t a) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(a); }
	inline int32_t Sqrt(int32_t v) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::Math_SqrtInt))(v); }
	inline int32_t CartesianToGameAngle(int32_t a, int32_t b) { return fn<int32_t (__cdecl *)(int32_t, int32_t)>(game_addr(game::CartesianToGameAngle))(a, b); }
	inline void GetEffectSpawnPosition(void *entity, int32_t bone, int32_t a, int16_t *out) { fn<void (__cdecl *)(void *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, a, out); }
	inline uint8_t *MainPartOf(uint8_t *entity) { return fn<uint8_t *(__cdecl *)(uint8_t *)>(game_addr(game::Battle_EntityChainLowest))(entity); }
	inline uint32_t RenderPrimModelB(void *header, uint32_t ot, int mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int, uint32_t)>(game_addr(game::Effect_RenderPrimModelGlow))(header, ot, mode, cursor); }
	inline void InsertPrimAltViewport(uint32_t ot, void *prim) { fn<void (__cdecl *)(uint32_t, void *)>(game_addr(game::SSIGPU_InsertPrimAltViewport))(ot, prim); }
	inline void CameraArmReturn() { fn<void (__cdecl *)()>(game_addr(game::BS_Camera_ArmReturnAfterEffect))(); }
	inline void BattleMenuShow(int32_t on) { fn<void (__cdecl *)(int32_t)>(game_addr(game::BattleMenu_SetVisible))(on); }
	inline uint32_t ReadPadHeldRaw(int32_t a, int32_t b) { return fn<uint32_t (__cdecl *)(int32_t, int32_t)>(game_addr(game::read_pad_held_raw))(a, b); }
	inline uint32_t RemapPadInput(uint32_t raw) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::remap_pad_input))(raw); }
	inline int32_t ReadAnalog(int32_t port, int32_t axis, int32_t c) { return fn<int32_t (__cdecl *)(int32_t, int32_t, int32_t)>(game_addr(game::sub_49F0A0))(port, axis, c); }
	inline void TextSetLayerText(int32_t layer, const void *text) { fn<void (__cdecl *)(int32_t, const void *)>(game_addr(game::Text_SetLayerText))(layer, text); }
	inline void TextLayer4A0700(int32_t layer, int32_t a) { fn<void (__cdecl *)(int32_t, int32_t)>(game_addr(game::Text_SetWindowFlags))(layer, a); }
	inline void TextSetPosition(int32_t layer, const int16_t *rect) { fn<void (__cdecl *)(int32_t, const int16_t *)>(game_addr(game::setTextPosition))(layer, rect); }
	inline void TextLayer4A0640(int32_t layer) { fn<void (__cdecl *)(int32_t)>(game_addr(game::Text_OpenWindowInstant))(layer); }
	inline void TextLayer4A0680(int32_t layer) { fn<void (__cdecl *)(int32_t)>(game_addr(game::Text_CloseWindowInstant))(layer); }
	inline void TextLayer49FBC0(int32_t layer, int32_t a) { fn<void (__cdecl *)(int32_t, int32_t)>(game_addr(game::sub_49FBC0))(layer, a); }
	inline uint8_t ScanTextLines(uint32_t slot) { return (uint8_t)fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::manageScanText_Dup))(slot); }
	inline const void *ScanTextLine(uint32_t slot, uint32_t line) { return fn<const void *(__cdecl *)(uint32_t, uint32_t)>(game_addr(game::ScanText_GetLine_Dup))(slot, line); }
	inline const void *ScanTextInfo(uint32_t slot) { return fn<const void *(__cdecl *)(uint32_t)>(game_addr(game::Scan_GetText))(slot); }
	inline const void *CharacterName(uint32_t slot) { return fn<const void *(__cdecl *)(uint32_t)>(game_addr(game::getAddressCharacterName))(slot); }
	inline const void *MonsterName(uint32_t slot) { return fn<const void *(__cdecl *)(uint32_t)>(game_addr(game::BattleText_GetMonsterName))(slot); }

	template<typename T> static inline T &F(uint8_t *p, uint32_t off) { return *(T *)(p + off); }
	template<typename T> static inline const T &F(const uint8_t *p, uint32_t off) { return *(const T *)(p + off); }

	static inline int32_t Div4096(int32_t x) { return x / 4096; }
	static inline int16_t Div96(int32_t x) { int32_t hi = (int32_t)(((int64_t)x * 0x2AAAAAAB) >> 32); hi >>= 4; return (int16_t)(hi + (int32_t)((uint32_t)hi >> 31)); }

	static int16_t EntityClass(int16_t slot)
	{
		uint8_t *e = Entity(slot);
		const int16_t t = (int16_t)e[4];
		if (t == 0x24 || t == 0x25) return F<int32_t>(e, 0x7C) == -1 ? (int16_t)0xA0 : t;
		if (t == 0x2B) return *F<uint8_t *>(e, 0x74) == 1 ? t : (int16_t)0xA1;
		if (t == 0x77) return *F<uint8_t *>(e, 0x74) == 1 ? t : (int16_t)0xA2;
		if (t == 0x3B)
		{
			const int16_t w = F<int16_t>(e, 0x9A);
			if (w == 0x1800) return (int16_t)0xA3;
			if (w == 0x2000) return (int16_t)0xA4;
			return t;
		}
		if (t == 0x4B) return *F<uint8_t *>(e, 0x74) == 1 ? t : (int16_t)0xA5;
		return t;
	}

	static const uint8_t *InfoOf(const FxNode *n)
	{
		const int16_t c = EntityClass((int16_t)(uint16_t)n->slot);
		Cam().cls = c;
		return (const uint8_t *)(game_addr(INFO_TABLE) + (int32_t)c * 16);
	}

	static void FindScannedEntities(int16_t slot)
	{
		uint8_t *e = Entity(slot);
		var<uint8_t *>(game_addr(game::g_Scan_MainEntity)) = MainPartOf(e);
		uint8_t *list[8];
		if (EntityClass(slot) == 0x54)
		{
			int16_t n = 0;
			for (int k = 0; k < 7; k++)
			{
				const uint8_t b = Entity(k)[4];
				if (b == 0x54 || b == 0x60 || b == 0x61) list[n++] = Entity(k);
			}
			list[n] = nullptr;
		}
		else
		{
			int16_t n = 0;
			uint8_t *p = e;
			do
			{
				if (p[0] & 2) list[n++] = p;
				p = F<uint8_t *>(p, 0x8C);
			} while (p != e && p != nullptr);
			list[n] = nullptr;
		}
		for (int k = 0; k < 7; k++)
		{
			Scanned(k) = 0;
			if (!list[0]) continue;
			for (int j = 0; list[j]; j++)
				if (Entity(k) == list[j]) Scanned(k) = 1;
		}
	}

	static void Line(const int16_t *a, const int16_t *b)
	{
		uint8_t *p = (uint8_t *)PacketCursor();
		p[4] = 0;
		p[6] = 0;
		F<uint32_t>(p, 0) = 0x3000000;
		p[7] = 0x40;
		F<int16_t>(p, 0xA) = a[1];
		F<int16_t>(p, 8) = a[0];
		p[5] = 0xFF;
		F<int16_t>(p, 0xC) = b[0];
		F<int16_t>(p, 0xE) = b[1];
		InsertPrimAltViewport(LinesOT(), p);
		PacketCursor() = (uint32_t)(p + 0x10);
	}

	static void Fan(const FxNode *n, int16_t r, int16_t a0, int16_t count, int16_t *out)
	{
		const uint32_t ot = LinesOT();
		uint8_t *p = (uint8_t *)PacketCursor();
		int32_t a = a0;
		int32_t x0 = Div4096(mul32(ComputeCos(a), r));
		int32_t y0 = Div4096(mul32(ComputeSin(a), r));
		for (int32_t c = count; c > 0; c--)
		{
			a = (a + 0x80) & 0xFFF;
			const int32_t x1 = Div4096(mul32(ComputeCos(a), r));
			const int32_t y1 = Div4096(mul32(ComputeSin(a), r));
			F<uint32_t>(p, 0) = 0x3000000;
			p[7] = 0x40;
			F<int16_t>(p, 8) = (int16_t)(n->pos[0] + x0 + 0xA0);
			F<int16_t>(p, 0xC) = (int16_t)(n->pos[0] + x1 + 0xA0);
			p[4] = 0;
			F<int16_t>(p, 0xA) = (int16_t)(n->pos[1] + y0 + 0x6C);
			p[5] = 0xFF;
			p[6] = 0;
			out[0] = (int16_t)(n->pos[0] + x1 + 0xA0);
			F<int16_t>(p, 0xE) = (int16_t)(n->pos[1] + y1 + 0x6C);
			out[1] = (int16_t)(n->pos[1] + y1 + 0x6C);
			InsertPrimAltViewport(ot, p);
			p += 0x10;
			x0 = x1;
			y0 = y1;
		}
		PacketCursor() = (uint32_t)p;
	}

	static void CornerLines(const FxNode *n)
	{
		int16_t o[2] = { (int16_t)(n->pos[0] + 0xA0), (int16_t)(n->pos[1] + 0x6C) };
		int16_t c[2] = { 0x10, 0xC };
		Line(o, c);
		c[0] = 0x130; c[1] = 0xC;
		Line(o, c);
		c[0] = 0x10; c[1] = 0xCC;
		Line(o, c);
		c[0] = 0x130; c[1] = 0xCC;
		Line(o, c);
	}

	static void BracketLines(const CtlNode *n)
	{
		const uint32_t ot = LinesOT();
		uint8_t *p = (uint8_t *)PacketCursor();
		const uint8_t *s = (const uint8_t *)n->shape;
		while (F<int16_t>(s, 0) != 0xFF)
		{
			p[4] = s[0xC];
			p[6] = s[0xE];
			const int32_t k = n->size;
			p[5] = s[0xD];
			F<uint32_t>(p, 0) = 0x3000000;
			p[7] = 0x40;
			F<int16_t>(p, 8) = (int16_t)(Div4096(F<int16_t>(s, 4) * k) + n->h.pos[0] + 0xA0);
			F<int16_t>(p, 0xA) = (int16_t)(Div4096(F<int16_t>(s, 6) * k) + n->h.pos[1] + 0x6C);
			F<int16_t>(p, 0xC) = (int16_t)(Div4096(F<int16_t>(s, 8) * k) + n->h.pos[0] + 0xA0);
			F<int16_t>(p, 0xE) = (int16_t)(Div4096(F<int16_t>(s, 0xA) * k) + n->h.pos[1] + 0x6C);
			InsertPrimAutoDepth(ot, p);
			s += 0x10;
			p += 0x10;
		}
		PacketCursor() = (uint32_t)p;
	}

	static void DrawHudModel(const ModelNode *n, bool alt)
	{
		if (n->h.flags & 4) return;
		Mat4x3 m;
		IdentityMatrix(&m);
		RotateY(&m, n->angle[1]);
		RotateX(&m, n->angle[0]);
		RotateZ(&m, n->angle[2]);
		m.t[0] = n->h.pos[0];
		m.t[1] = n->h.pos[1];
		m.t[2] = n->h.pos[2];
		Scale3DMatrix(&m, n->scale);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		F<uint32_t>(h, 0) = n->model;
		F<uint32_t>(h, 8) = *(const uint32_t *)n->rgb;
		F<int32_t>(h, 0xC) = n->fade;
		F<uint32_t>(h, 0x1C) = 0xF0;
		if (alt) PacketCursor() = RenderPrimModelB(h, RenderOT(), 2, PacketCursor());
		else PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static void DrawHudSprite(const SpriteNode *n, int16_t s)
	{
		if (n->h.flags & 4) return;
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		int32_t sv[3] = { s, s, s };
		Mat4x3 m;
		IdentityMatrix(&m);
		Scale3DMatrix(&m, sv);
		m.t[0] = n->h.pos[0];
		m.t[1] = n->h.pos[1];
		m.t[2] = n->h.pos[2];
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		F<uint32_t>(h, 0) = n->seq;
		F<int16_t>(h, 4) = n->frame;
		F<int16_t>(h, 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static void MorphVertices(const uint8_t *a, const uint8_t *b, uint8_t *out, int16_t t)
	{
		const uint8_t *pa = a + 8;
		const uint8_t *pb = b + 8;
		uint8_t *po = out + 8;
		int32_t count = *(const int32_t *)(a + 4);
		int32_t *w = (int32_t *)FieldAlloc(8);
		w[1] = t;
		w[0] = 0x1000 - t;
		for (; count > 0; count--)
		{
			GteSetIR0(w[0]);
			GteLoadIR123(pa);
			GteGPF();
			GteSetIR0(w[1]);
			GteLoadIR123(pb);
			GteGPL();
			GteStoreIR123(po);
			pa += 8;
			pb += 8;
			po += 8;
		}
		FieldFree(8);
	}

	static void ScanBars(int16_t tick, uint8_t c)
	{
		const uint32_t ot = var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x4474;
		uint8_t *p = (uint8_t *)PacketCursor();
		int16_t y = (int16_t)((int32_t)((uint8_t)tick & 0x3F) - 0x40);
		for (int row = 5; row > 0; row--)
		{
			const int16_t y1 = (int16_t)(y + 0x40);
			int16_t x = 0;
			for (int col = 3; col > 0; col--)
			{
				const int16_t x1 = (int16_t)(x + 0x80);
				F<int16_t>(p, 8) = x;
				F<int16_t>(p, 0x18) = x;
				F<uint32_t>(p, 0) = 0x9000000;
				p[7] = 0x2C;
				F<uint16_t>(p, 0x16) = 0xBA;
				F<uint16_t>(p, 0xE) = 0x3C94;
				p[0xC] = 0x80;
				p[0xD] = 0;
				p[0x14] = 0;
				p[0x15] = 0;
				p[0x1C] = 0x80;
				p[0x1D] = 0x40;
				p[0x24] = 0;
				p[0x25] = 0x40;
				F<int16_t>(p, 0xA) = y;
				F<int16_t>(p, 0x10) = x1;
				F<int16_t>(p, 0x12) = y;
				F<int16_t>(p, 0x1A) = y1;
				F<int16_t>(p, 0x20) = x1;
				F<int16_t>(p, 0x22) = y1;
				p[6] = c;
				p[5] = c;
				p[4] = c;
				InsertPrimAltViewport(ot, p);
				p += 0x28;
				x = x1;
			}
			y = y1;
		}
		PacketCursor() = (uint32_t)p;
	}

	static int32_t CameraBlend(CamState &c)
	{
		int32_t arrived = 0;
		c.t = (int16_t)(c.t + c.step);
		if (c.t >= 0x1000)
		{
			c.t = 0x1000;
			arrived = 1;
		}
		auto lerp = [&](int16_t a, int16_t b) { return (int16_t)(Div4096(((int32_t)b - (int32_t)a) * c.t) + a); };
		c.yaw = lerp(c.yaw0, c.yaw1);
		c.dist = lerp(c.dist0, c.dist1);
		c.h = lerp(c.h0, c.h1);
		c.proj = lerp(c.proj0, c.proj1);
		c.look[0] = lerp(c.look0[0], c.look1[0]);
		c.look[1] = lerp(c.look0[1], c.look1[1]);
		c.look[2] = lerp(c.look0[2], c.look1[2]);
		return arrived;
	}

	static void CameraEye(CamState &c)
	{
		Mat4x3 m;
		IdentityMatrix(&m);
		RotateY(&m, c.yaw);
		int16_t v[4];
		int16_t out[4];
		v[0] = 0;
		v[1] = c.h;
		v[2] = c.dist;
		MatrixMulVector(&m, v, out);
		c.eye[0] = (int16_t)(c.look[0] + out[0]);
		c.eye[1] = (int16_t)(c.look[1] + out[1]);
		c.eye[2] = (int16_t)(c.look[2] + out[2]);
	}

	static void CameraApply()
	{
		CamState &c = Cam();
		CameraEye(c);
		var<uint32_t>(game_addr(game::Battle_Camera_LookAt_XZ_s16)) = *(const uint32_t *)&c.look[0];
		var<uint32_t>(game_addr(game::Battle_Camera_LookAt_Y)) = *(const uint32_t *)&c.look[2];
		var<uint32_t>(game_addr(game::Battle_Camera_world_XZ_s16)) = *(const uint32_t *)&c.eye[0];
		var<uint32_t>(game_addr(game::Battle_Camera_world_Y)) = *(const uint32_t *)&c.eye[2];
		var<int16_t>(game_addr(game::BATTLE_PROJECTION_DISTANCE)) = c.proj;
	}

	static void CameraSetup(CtlNode *n)
	{
		const uint8_t *info = InfoOf(&n->h);
		uint8_t *ent = Entity(n->h.slot);
		CamState &c = Cam();
		c.t = 0;
		switch (n->mode)
		{
		case 0:
		case 2:
			c.dist1 = (int16_t)((int32_t)F<int16_t>(info, 0) * 3 / 4);
			c.h1 = (int16_t)(c.dist1 / 2);
			c.step = 0x100;
			break;
		case 1:
			c.dist1 = F<int16_t>(info, 0);
			c.h1 = 0;
			c.step = 0x2AA;
			break;
		case 3:
			c.dist1 = (int16_t)((int32_t)F<int16_t>(info, 0) * 3 / 4);
			c.h1 = (int16_t)(c.dist1 / 2);
			c.step = 0x200;
			break;
		default: break;
		}
		c.proj0 = var<int16_t>(game_addr(game::BATTLE_PROJECTION_DISTANCE));
		c.proj1 = 0x240;
		const int32_t ex = (int32_t)var<int16_t>(game_addr(game::Battle_Camera_world_XZ_s16)) - var<int16_t>(game_addr(game::Battle_Camera_LookAt_XZ_s16));
		const int32_t ez = (int32_t)var<int16_t>(game_addr(game::Battle_Camera_world_Y)) - var<int16_t>(game_addr(game::Battle_Camera_LookAt_Y));
		const int32_t d = Sqrt((int32_t)((uint32_t)mul32(ez, ez) + (uint32_t)mul32(ex, ex)));
		c.dist0 = (int16_t)-d;
		c.h0 = (int16_t)(var<int16_t>(game_addr(game::unk_B8B7F2)) - var<int16_t>(game_addr(game::unk_B8B7FA)));
		c.yaw1 = F<int16_t>(MainEntity(), 0xE);
		c.yaw0 = (int16_t)((CartesianToGameAngle(var<int16_t>(game_addr(game::g_BattleCam_ViewMatrixM22)), var<int16_t>(game_addr(game::unk_1D9777C))) - 0x400) & 0xFFF);
		if ((uint32_t)(int32_t)n->mode <= 3)
		{
			int16_t v[4];
			if (n->mode == 1)
			{
				GetEffectSpawnPosition(MainEntity(), 0xF1, 0, c.look1);
				if (F<int16_t>(info, 8) == 0)
				{
					ff8fx::copy<(4)>(&c.look1[0], MainEntity() + 0x1C);
					ff8fx::copy<(4)>(&c.look1[2], MainEntity() + 0x20);
				}
				const int16_t x = F<int16_t>(info, 2);
				c.look1[1] = 0;
				v[0] = x;
			}
			else
			{
				GetEffectSpawnPosition(ent, 0xF1, 0, c.look1);
				if (F<int16_t>(info, 8) == 0)
				{
					ff8fx::copy<(4)>(&c.look1[0], ent + 0x1C);
					ff8fx::copy<(4)>(&c.look1[2], ent + 0x20);
				}
				c.look1[1] = 0;
				v[0] = 0;
			}
			v[1] = F<int16_t>(info, 4);
			v[2] = 0;
			Mat4x3 m;
			IdentityMatrix(&m);
			RotateY(&m, F<int16_t>(MainEntity(), 0xE));
			int16_t out[4];
			MatrixMulVector(&m, v, out);
			c.look1[0] = (int16_t)(c.look1[0] + out[0]);
			c.look1[1] = (int16_t)(c.look1[1] + out[1]);
			c.look1[2] = (int16_t)(c.look1[2] + out[2]);
		}
		ff8fx::copy<(4)>(&c.look0[0], (const void *)game_addr(game::Battle_Camera_LookAt_XZ_s16));
		ff8fx::copy<(4)>(&c.look0[2], (const void *)game_addr(game::Battle_Camera_LookAt_Y));
		n->h.phase++;
	}
}
}

namespace ff8fx
{
namespace scan040
{
	static inline uint32_t TaskEnd(FxNode *n)
	{
		n->counter++;
		if ((n->flags & 1) && n->children == 0)
		{
			ReleaseLinkedTask(n);
			return TASK_END;
		}
		return 0;
	}

	static uint32_t __cdecl CameraTask(TaskNode *t)
	{
		CtlNode *n = (CtlNode *)t;
		switch (n->h.phase)
		{
		case 0:
			CameraSetup(n);
			break;
		case 1:
			if (CameraBlend(Cam()) == 1)
			{
				*(uint8_t *)&n->h.flags |= 1;
				n->h.phase++;
			}
			CameraApply();
			break;
		default: break;
		}
		return TaskEnd(&n->h);
	}

	static uint32_t __cdecl CamRestoreTask(TaskNode *t)
	{
		CtlNode *n = (CtlNode *)t;
		switch (n->h.phase)
		{
		case 0:
			var<uint32_t>(game_addr(game::Battle_Camera_LookAt_XZ_s16)) = var<uint32_t>(game_addr(game::Battle_Camera_ReturnView_LookAt_XZ_s16));
			var<uint32_t>(game_addr(game::Battle_Camera_LookAt_Y)) = var<uint32_t>(game_addr(game::Battle_Camera_ReturnView_LookAt_Y));
			var<uint32_t>(game_addr(game::Battle_Camera_world_Y)) = var<uint32_t>(game_addr(game::Battle_Camera_ReturnView_world_Y));
			var<int16_t>(game_addr(game::BATTLE_PROJECTION_DISTANCE)) = var<int16_t>(game_addr(game::Battle_Camera_ReturnView_Param1D8E038));
			var<uint32_t>(game_addr(game::Battle_Camera_world_XZ_s16)) = var<uint32_t>(game_addr(game::Battle_Camera_ReturnView_world_XZ_s16));
			n->timer = 1;
			n->h.phase++;
			break;
		case 1:
			n->timer--;
			if (n->timer <= 0)
			{
				*(uint8_t *)&n->h.flags |= 1;
				n->h.phase++;
			}
			break;
		default: break;
		}
		return TaskEnd(&n->h);
	}

	static uint32_t __cdecl CornerTask(TaskNode *t)
	{
		CtlNode *n = (CtlNode *)t;
		switch (n->h.phase)
		{
		case 0:
			switch (n->index)
			{
			case 0: n->shape = game_addr(game::unk_15D1718); n->h.pos[0] = (int16_t)-0x90; n->h.pos[1] = (int16_t)-0x60; break;
			case 1: n->shape = game_addr(game::unk_15D1748); n->h.pos[0] = 0x90; n->h.pos[1] = (int16_t)-0x60; break;
			case 2: n->shape = game_addr(game::unk_15D1778); n->h.pos[0] = (int16_t)-0x90; n->h.pos[1] = 0x60; break;
			case 3: n->shape = game_addr(game::unk_15D17A8); n->h.pos[0] = 0x90; n->h.pos[1] = 0x60; break;
			default: break;
			}
			n->size = 0x1000;
			n->h.phase++;
			break;
		case 1:
			BracketLines(n);
			if (HudActive() == 0)
			{
				*(uint8_t *)&n->h.flags |= 1;
				n->h.phase++;
			}
			break;
		default: break;
		}
		return TaskEnd(&n->h);
	}

	static uint32_t __cdecl BracketTask(TaskNode *t)
	{
		CtlNode *n = (CtlNode *)t;
		switch (n->h.phase)
		{
		case 0:
			ff8fx::copy<(8)>(n->h.pos, n->h.parent->pos);
			switch (n->index)
			{
			case 0: n->shape = game_addr(game::unk_15D17D8); n->h.pos[0] = 0; n->h.pos[1] = 0; n->dx = -1; n->dy = -1; break;
			case 1: n->shape = game_addr(game::unk_15D1808); n->h.pos[0] = 0; n->h.pos[1] = 0; n->dx = 1; n->dy = -1; break;
			case 2: n->shape = game_addr(game::unk_15D1838); n->h.pos[0] = 0; n->h.pos[1] = 0; n->dx = -1; n->dy = 1; break;
			case 3: n->shape = game_addr(game::unk_15D1868); n->h.pos[0] = 0; n->h.pos[1] = 0; n->dx = 1; n->dy = 1; break;
			default: break;
			}
			n->size = 0x1000;
			n->timer = 0x1E;
			n->h.phase++;
			break;
		case 1:
			ff8fx::copy<(8)>(n->h.pos, n->h.parent->pos);
			BracketLines(n);
			n->timer--;
			if (n->timer < 0)
			{
				n->timer = 4;
				n->h.phase++;
			}
			break;
		case 2:
			n->h.pos[0] = (int16_t)(n->h.pos[0] + n->dx);
			n->h.pos[1] = (int16_t)(n->h.pos[1] + n->dy);
			n->size = (int16_t)(n->size + 0x400);
			n->timer--;
			if (n->timer < 0) n->h.phase++;
			BracketLines(n);
			break;
		case 3:
			BracketLines(n);
			if (HudActive() == 0)
			{
				*(uint8_t *)&n->h.flags |= 1;
				n->h.phase++;
			}
			break;
		default: break;
		}
		return TaskEnd(&n->h);
	}

	static uint32_t __cdecl RingTask(TaskNode *t)
	{
		CtlNode *n = (CtlNode *)t;
		int16_t out[2];
		switch (n->h.phase)
		{
		case 0:
			n->timer = 0;
			n->h.phase++;
			break;
		case 1:
			n->timer = (int16_t)(n->timer + 4);
			if (n->timer >= 0x18)
			{
				n->timer = 0x18;
				n->h.phase++;
			}
			Fan(&n->h, 0x30, 0x600, n->timer, out);
			break;
		case 2:
		{
			Fan(&n->h, 0x30, 0x600, n->timer, out);
			const int16_t p[2] = { (int16_t)(out[0] + 0x18), (int16_t)(out[1] + 0x18) };
			Line(out, p);
			AddEffectTask(QSprites(), game_addr(ORIG_Sprite), 0x58, n);
			n->h.phase++;
			break;
		}
		case 3:
		{
			Fan(&n->h, 0x30, 0x600, n->timer, out);
			const int16_t p[2] = { (int16_t)(out[0] + 0x18), (int16_t)(out[1] + 0x18) };
			Line(out, p);
			const int16_t q[2] = { (int16_t)(p[0] + 0x58), p[1] };
			Line(p, q);
			if (HudActive() == 0)
			{
				*(uint8_t *)&n->h.flags |= 1;
				n->h.phase++;
			}
			break;
		}
		default: break;
		}
		return TaskEnd(&n->h);
	}

	static uint32_t __cdecl ReticleTask(TaskNode *t)
	{
		CtlNode *n = (CtlNode *)t;
		int16_t out[2];
		switch (n->h.phase)
		{
		case 0:
		{
			const int16_t *path = (const int16_t *)game_addr(RETICLE_PATH) + 2 * n->h.counter;
			n->h.pos[0] = path[0];
			n->h.pos[1] = path[1];
			for (int16_t i = 0; i < 4; i++) ((CtlNode *)AddEffectTask(QControls(), game_addr(ORIG_Bracket), 0x78, n))->index = i;
			for (int16_t i = 0; i < 4; i++) ((CtlNode *)AddEffectTask(QControls(), game_addr(ORIG_Corner), 0x78, n))->index = i;
			AddEffectTask(QModels(), game_addr(ORIG_Morph), 0x6C, n);
			AddEffectTask(QModels(), game_addr(ORIG_Pair), 0x6C, n);
			Fan(&n->h, 8, 0, 0x20, out);
			CornerLines(&n->h);
			n->h.phase++;
			break;
		}
		case 1:
		{
			const int16_t *path = (const int16_t *)game_addr(RETICLE_PATH) + 2 * n->h.counter;
			if (path[0] == 0x7FFF)
			{
				n->timer = 0;
				n->h.phase++;
			}
			else
			{
				n->h.pos[0] = path[0];
				n->h.pos[1] = path[1];
			}
			Fan(&n->h, 8, 0, 0x20, out);
			CornerLines(&n->h);
			n->timer = 8;
			break;
		}
		case 2:
			n->timer = (int16_t)(n->timer - 2);
			if (n->timer > 0)
			{
				Fan(&n->h, n->timer, 0, 0x20, out);
				CornerLines(&n->h);
			}
			else n->h.phase++;
			break;
		case 3:
			AddEffectTask(QControls(), game_addr(ORIG_Ring), 0x78, n);
			n->h.phase++;
			break;
		case 4:
			if (HudActive() == 0)
			{
				*(uint8_t *)&n->h.flags |= 1;
				n->h.phase++;
			}
			break;
		default: break;
		}
		return TaskEnd(&n->h);
	}

	static uint32_t __cdecl BarsTask(TaskNode *t)
	{
		CtlNode *n = (CtlNode *)t;
		switch (n->h.phase)
		{
		case 0: n->h.phase++; break;
		case 1:
			n->fade = (int16_t)(n->fade + 8);
			if (n->fade >= 0x80)
			{
				n->fade = 0x80;
				n->h.phase++;
			}
			ScanBars(n->h.counter, (uint8_t)n->fade);
			break;
		case 2:
			ScanBars(n->h.counter, (uint8_t)n->fade);
			if (Confirmed() == 1) n->h.phase++;
			break;
		case 3:
			n->fade = (int16_t)(n->fade - 8);
			if (n->fade <= 0)
			{
				*(uint8_t *)&n->h.flags |= 1;
				n->fade = 0;
				n->h.phase++;
			}
			ScanBars(n->h.counter, (uint8_t)n->fade);
			break;
		default: break;
		}
		return TaskEnd(&n->h);
	}

	static void DimEntities(const CtlNode *n)
	{
		const int16_t v = n->h.pos[0];
		for (int k = 0; k < 4; k++) var<int16_t>(game_addr(game::unk_1D98992) + 0x2C * k) = v;
		const uint32_t b = var<uint32_t>(game_addr(game::CURRENT_BS_MODEL_PALETTE));
		for (int k = 0; k < 7; k++)
		{
			if (Scanned(k) != 0) continue;
			uint8_t *e = Entity(k);
			const int32_t s = v;
			e[0x28] = (uint8_t)((uint8_t)b - (uint8_t)Div4096((int32_t)(b & 0xFF) * s));
			e[0x29] = (uint8_t)((uint8_t)(b >> 8) - (uint8_t)Div4096((int32_t)((b >> 8) & 0xFF) * s));
			const uint8_t z = var<uint8_t>(game_addr(game::unk_B8B7DA));
			e[0x2A] = (uint8_t)(z - (uint8_t)Div4096((int32_t)z * s));
		}
	}

	static uint32_t __cdecl DimmerTask(TaskNode *t)
	{
		CtlNode *n = (CtlNode *)t;
		switch (n->h.phase)
		{
		case 0:
			n->h.pos[0] = 0;
			for (int k = 0; k < 4; k++)
			{
				var<int16_t>(game_addr(game::unk_1D98992) + 0x2C * k) = 0;
				var<uint8_t>(game_addr(game::unk_1D989BA) + 0x2C * k) = 0;
				var<uint8_t>(game_addr(game::unk_1D989B9) + 0x2C * k) = 0;
				var<uint8_t>(game_addr(game::unk_1D989B8) + 0x2C * k) = 0;
			}
			for (int k = 0; k < 7; k++)
			{
				uint8_t *e = Entity(k);
				const uint16_t w = F<uint16_t>(e, 0);
				SavedWord(k) = w;
				SavedColour(k) = F<uint32_t>(e, 0x28);
				if (Scanned(k) == 0) F<uint16_t>(e, 0) = (uint16_t)(w | 0x800);
			}
			n->h.phase++;
			break;
		case 1:
			n->h.pos[0] = (int16_t)(n->h.pos[0] + 0x100);
			if (n->h.pos[0] >= 0x1000)
			{
				n->h.pos[0] = 0x1000;
				n->h.phase++;
			}
			DimEntities(n);
			break;
		case 2:
			for (int k = 0; k < 4; k++) var<uint8_t>(game_addr(game::unk_1D98991) + 0x2C * k) &= 0xFD;
			for (int k = 0; k < 7; k++)
			{
				uint8_t *e = Entity(k);
				if (Scanned(k) == 1)
				{
					e[0x2A] = 0x80;
					e[0x29] = 0x80;
					e[0x28] = 0x80;
					e[0x2B] = 0;
					e[0] |= 0x20;
				}
				else e[0] |= 0x24;
			}
			*(uint8_t *)&n->h.flags |= 1;
			n->h.phase++;
			break;
		default: break;
		}
		return TaskEnd(&n->h);
	}

	static uint32_t __cdecl RestorerTask(TaskNode *t)
	{
		CtlNode *n = (CtlNode *)t;
		switch (n->h.phase)
		{
		case 0:
			n->h.pos[0] = 0x1000;
			for (int k = 0; k < 4; k++)
			{
				const uint8_t a = var<uint8_t>(game_addr(game::unk_1D98991) + 0x2C * k);
				var<uint8_t>(game_addr(game::unk_1D98991) + 0x2C * k) = (uint8_t)(((a >> 1) & 2) | (a & 0xFD));
			}
			for (int k = 0; k < 7; k++)
			{
				uint8_t *e = Entity(k);
				if (Scanned(k) == 0) F<uint16_t>(e, 0) ^= (uint16_t)((uint8_t)(e[0] ^ (uint8_t)SavedWord(k)) & 0x24);
				else
				{
					F<uint16_t>(e, 0) ^= (uint16_t)((uint8_t)((uint8_t)SavedWord(k) ^ e[0]) & 0x20);
					F<uint32_t>(e, 0x28) = SavedColour(k);
				}
			}
			n->h.phase++;
			break;
		case 1:
			n->h.pos[0] = (int16_t)(n->h.pos[0] - 0x100);
			if (n->h.pos[0] <= 0)
			{
				n->h.pos[0] = 0;
				n->h.phase++;
			}
			DimEntities(n);
			break;
		case 2:
		{
			const uint32_t b = var<uint32_t>(game_addr(game::CURRENT_BS_MODEL_PALETTE));
			for (int k = 0; k < 7; k++)
			{
				uint8_t *e = Entity(k);
				if (Scanned(k) == 0)
				{
					F<uint16_t>(e, 0) ^= (uint16_t)(((uint16_t)SavedWord(k) ^ F<uint16_t>(e, 0)) & 0x800);
					e[0x28] = (uint8_t)b;
					e[0x29] = (uint8_t)(b >> 8);
					e[0x2A] = var<uint8_t>(game_addr(game::unk_B8B7DA));
				}
				else F<uint16_t>(e, 0) ^= (uint16_t)((uint8_t)((uint8_t)SavedWord(k) ^ e[0]) & 0x20);
			}
			*(uint8_t *)&n->h.flags |= 1;
			n->h.phase++;
			break;
		}
		default: break;
		}
		return TaskEnd(&n->h);
	}

	static void PairPhase(ModelNode *n)
	{
		switch (n->h.phase)
		{
		case 0:
			*(uint8_t *)&n->h.flags |= 4;
			n->h.pos[2] = 0x480;
			n->scale[2] = 0x1000;
			n->angle[2] = 0xA00;
			n->h.phase++;
			break;
		case 1:
			if (n->h.counter >= 3) n->h.phase++;
			break;
		case 2:
		{
			const int16_t k = n->h.counter;
			*(uint8_t *)&n->h.flags &= 0xFB;
			n->h.pos[0] = (int16_t)(((const int16_t *)game_addr(game::unk_15D15E8))[2 * k] - 0x3C);
			n->h.pos[1] = (int16_t)(((const int16_t *)game_addr(game::unk_15D15E8))[2 * k + 1] + 0x3C);
			n->scale[0] = ((const int16_t *)game_addr(game::unk_15D1630))[2 * k];
			n->scale[1] = ((const int16_t *)game_addr(game::unk_15D1630))[2 * k + 1];
			n->fade1 = ((const int16_t *)game_addr(game::unk_15D15A0))[k];
			n->fade2 = ((const int16_t *)game_addr(game::unk_15D15C4))[k];
			if (k >= 0x11) n->h.phase++;
			break;
		}
		case 3:
			n->wave = (int16_t)((n->wave + 0x200) & 0xFFF);
			n->fade2 = (int16_t)((int16_t)(ComputeSin(n->wave) + 0x1000) / 4);
			if (HudActive() == 0)
			{
				*(uint8_t *)&n->h.flags |= 1;
				n->h.phase++;
			}
			break;
		default: break;
		}
	}

	static void PairDraw(ModelNode *n)
	{
		n->fade = n->fade1;
		n->model = game_addr(game::unk_15CE93C);
		DrawHudModel(n, false);
		n->fade = n->fade2;
		n->model = game_addr(game::unk_15CE9B8);
		DrawHudModel(n, false);
	}

	static uint32_t __cdecl PairTask(TaskNode *t)
	{
		ModelNode *n = (ModelNode *)t;
		PairPhase(n);
		PairDraw(n);
		return TaskEnd(&n->h);
	}

	static void MorphPhase(ModelNode *n)
	{
		switch (n->h.phase)
		{
		case 0:
			n->h.pos[0] = 0;
			n->h.pos[1] = 0;
			n->h.pos[2] = 0x480;
			n->scale[2] = 0x3FF8;
			n->scale[1] = 0x3FF8;
			n->scale[0] = 0x3FF8;
			n->angle[0] = 0x5DC;
			n->angle[1] = 0x2A0;
			n->angle[2] = (int16_t)0xF808;
			n->model = game_addr(game::unk_15CDDEC);
			n->morph_a = game_addr(game::unk_15CFAB4);
			n->morph_b = game_addr(game::unk_15CFC3C);
			n->fade = 0x1000;
			n->h.phase++;
			break;
		case 1:
			n->fade = (int16_t)(n->fade - 0x155);
			if (n->fade < 0) n->fade = 0;
			n->angle[0] = (int16_t)((n->angle[0] - 0x7D) & 0xFFF);
			n->angle[1] = (int16_t)((n->angle[1] - 0x38) & 0xFFF);
			n->angle[2] = (int16_t)((n->angle[2] + 0xAA) & 0xFFF);
			n->scale[0] += -0x4AA;
			if (n->scale[0] <= 0x800)
			{
				n->scale[0] = 0x800;
				n->timer = 0xC;
				n->h.phase++;
			}
			n->scale[2] = n->scale[0];
			n->scale[1] = n->scale[0];
			break;
		case 2:
			n->angle[2] = (int16_t)((n->angle[2] + 0xAA) & 0xFFF);
			n->timer--;
			if (n->timer <= 0) n->h.phase++;
			break;
		case 3:
			n->blend = (int16_t)(n->blend + 0x200);
			n->angle[2] = (int16_t)((n->angle[2] + 0xAA) & 0xFFF);
			if (n->blend >= 0x1000)
			{
				n->blend = 0;
				n->model = game_addr(game::unk_15CE1D4);
				n->morph_a = game_addr(game::unk_15CFDC4);
				n->morph_b = game_addr(game::unk_15D02CC);
				n->h.phase++;
			}
			break;
		case 4:
			n->blend = (int16_t)(n->blend + 0x400);
			if (n->blend >= 0x1000)
			{
				n->blend = 0x1000;
				n->h.phase++;
			}
			break;
		case 5:
			if ((int8_t)((uint8_t)n->h.counter & 7) < 4) n->angle[2] = (int16_t)((n->angle[2] + 0x2A) & 0xFFF);
			if (HudActive() == 0)
			{
				*(uint8_t *)&n->h.flags |= 1;
				n->h.phase++;
			}
			break;
		default: break;
		}
	}

	static uint32_t __cdecl MorphTask(TaskNode *t)
	{
		ModelNode *n = (ModelNode *)t;
		MorphPhase(n);
		MorphVertices((const uint8_t *)n->morph_a, (const uint8_t *)n->morph_b, (uint8_t *)n->model, n->blend);
		DrawHudModel(n, false);
		return TaskEnd(&n->h);
	}

	static void SpinnerPhase(ModelNode *n)
	{
		switch (n->h.phase)
		{
		case 0:
			n->scale[0] = 0x1000;
			n->scale[1] = 0x1000;
			n->scale[2] = 0x1000;
			n->fade = 0x1000;
			n->h.pos[0] = (int16_t)-0x80;
			n->h.pos[1] = (int16_t)-0xA0;
			n->h.pos[2] = 0x480;
			n->model = game_addr(game::g_Scan_SpinnerModel);
			n->h.phase++;
			break;
		case 1:
			n->fade = (int16_t)(n->fade - 0x100);
			if (n->fade <= 0)
			{
				n->fade = 0;
				n->h.phase++;
			}
			break;
		case 2:
			if (Closing() == 1)
			{
				n->h.flags |= 1;
				n->h.phase++;
			}
			break;
		default: break;
		}
	}

	static void SpinnerTurn(ModelNode *n)
	{
		n->angle[1] = (int16_t)((n->angle[1] + 0xC0) & 0xFFF);
		n->angle[0] = (int16_t)((n->angle[0] + 0x60) & 0xFFF);
	}

	static uint32_t __cdecl SpinnerTask(TaskNode *t)
	{
		ModelNode *n = (ModelNode *)t;
		SpinnerPhase(n);
		SpinnerTurn(n);
		DrawHudModel(n, false);
		return TaskEnd(&n->h);
	}

	static int16_t BarWidth(uint8_t *stats, int which)
	{
		int32_t d;
		if (which <= 1)
		{
			int32_t v = F<int32_t>(stats, 0);
			if (v > 0x3E7) v = 0x3E7;
			const int32_t s = shl32(v, 0xE);
			d = (int32_t)(((int64_t)s * (int32_t)0x83340521) >> 32) + s;
			d >>= 9;
		}
		else if (which <= 6)
		{
			int32_t v = stats[0xA5 + (which - 2)];
			if (v > 0x7F) v = 0x7F;
			const int32_t s = shl32(v, 0xE);
			d = (int32_t)(((int64_t)s * (int32_t)0x81020409) >> 32) + s;
			d >>= 6;
		}
		else
		{
			int32_t v = stats[0xAB];
			if (v > 0x19) v = 0x19;
			const int32_t s = shl32(v, 0xE);
			d = (int32_t)(((int64_t)s * 0x51EB851F) >> 32);
			d >>= 3;
		}
		return (int16_t)(d + (int32_t)((uint32_t)d >> 31));
	}

	static void StatBarPhase(ModelNode *n)
	{
		switch (n->h.phase)
		{
		case 0:
		{
			const int16_t k = n->which;
			const FxNode *parent = n->h.parent;
			const int16_t *tbl = (const int16_t *)(n->table == 0 ? game_addr(game::g_Scan_StatBarLayout) : game_addr(game::g_Scan_ShortStatBarLayout)) + 5 * k;
			*(uint8_t *)&n->h.flags |= 4;
			if ((uint32_t)(int32_t)k <= 7)
			{
				static constexpr game::Id MODELS[8] = { game::g_Scan_StatBarModel_HP, game::g_Scan_StatBarModel_HP, game::g_Scan_StatBarModel_STR, game::g_Scan_StatBarModel_VIT, game::g_Scan_StatBarModel_MAG, game::g_Scan_StatBarModel_SPR, game::g_Scan_StatBarModel_SPD, game::g_Scan_StatBarModel_EVA };
				n->model = game_addr(MODELS[k]);
				n->max = BarWidth((uint8_t *)game_addr(game::unk_1D27B28) + 0xD0 * n->h.slot, k);
			}
			n->scale[0] = 0;
			n->scale[1] = 0x1000;
			n->scale[2] = 0x1000;
			n->h.pos[0] = (int16_t)(tbl[0] / 2 + parent->pos[0]);
			n->h.pos[2] = parent->pos[2];
			n->h.pos[1] = (int16_t)(tbl[1] / 2 + parent->pos[1]);
			n->angle[2] = tbl[2];
			n->wave = tbl[4];
			n->timer = tbl[3];
			n->h.phase++;
			break;
		}
		case 1:
			n->timer--;
			if (n->timer < 0)
			{
				*(uint8_t *)&n->h.flags &= 0xFB;
				n->h.phase++;
			}
			break;
		case 2:
			n->scale[0] += 0x300;
			if (n->scale[0] > n->max) n->scale[0] = n->max;
			if (Closing() == 1)
			{
				n->h.flags |= 1;
				n->h.phase++;
			}
			break;
		default: break;
		}
	}

	static void StatBarPulse(ModelNode *n)
	{
		n->wave = (int16_t)((n->wave + 0x100) & 0xFFF);
		const int32_t v = (int16_t)(ComputeSin(n->wave) + 0x1000);
		const int32_t hi = (int32_t)(((int64_t)v * 0x2AAAAAAB) >> 32);
		n->fade = (int16_t)(hi + (int32_t)((uint32_t)hi >> 31));
	}

	static uint32_t __cdecl StatBarTask(TaskNode *t)
	{
		ModelNode *n = (ModelNode *)t;
		StatBarPhase(n);
		StatBarPulse(n);
		DrawHudModel(n, false);
		return TaskEnd(&n->h);
	}

	static uint32_t __cdecl BoardTask(TaskNode *t)
	{
		ModelNode *n = (ModelNode *)t;
		switch (n->h.phase)
		{
		case 0:
			n->scale[0] = 0x1000;
			n->scale[1] = 0x1000;
			n->scale[2] = 0x1000;
			n->h.pos[0] = (int16_t)-0x40;
			n->h.pos[1] = (int16_t)-0x58;
			n->h.pos[2] = 0x240;
			n->model = game_addr(game::g_Scan_StatBoardModel);
			n->h.phase++;
			break;
		case 1:
			for (int16_t i = 0; i < 8; i++)
			{
				ModelNode *b = (ModelNode *)AddEffectTask(QModels(), game_addr(ORIG_StatBar), 0x6C, n);
				b->which = i;
				b->table = 0;
			}
			n->h.phase++;
			break;
		case 2:
			if (Closing() == 1)
			{
				n->h.flags |= 1;
				n->h.phase++;
			}
			break;
		default: break;
		}
		DrawHudModel(n, false);
		return TaskEnd(&n->h);
	}

	static uint32_t __cdecl AltBoardTask(TaskNode *t)
	{
		ModelNode *n = (ModelNode *)t;
		switch (n->h.phase)
		{
		case 0:
			n->scale[0] = 0x1000;
			n->scale[1] = 0x1000;
			n->scale[2] = 0x1000;
			n->h.pos[0] = (int16_t)-0x88;
			n->h.pos[1] = (int16_t)-0x3C;
			n->h.pos[2] = 0x240;
			n->model = game_addr(game::unk_15CF614);
			n->h.phase++;
			break;
		case 1:
			for (int16_t i = 1; i < 8; i++)
			{
				ModelNode *b = (ModelNode *)AddEffectTask(QModels(), game_addr(ORIG_StatBar), 0x6C, n);
				b->which = i;
				b->table = 1;
			}
			n->h.phase++;
			break;
		case 2:
			if (Closing() == 1)
			{
				n->h.flags |= 1;
				n->h.phase++;
			}
			break;
		default: break;
		}
		DrawHudModel(n, false);
		return TaskEnd(&n->h);
	}

	static void BackdropPhase(ModelNode *n)
	{
		switch (n->h.phase)
		{
		case 0:
			n->scale[0] = 0x1200;
			n->scale[1] = 0x1200;
			n->scale[2] = 0x1200;
			n->h.pos[0] = (int16_t)0xF080;
			n->h.pos[1] = 0xF00;
			n->h.pos[2] = 0x7FFF;
			n->model = game_addr(game::g_Scan_BackdropModel);
			n->fade = 0x1000;
			n->h.phase++;
			break;
		case 1:
			n->fade = (int16_t)(n->fade - 0x200);
			if (n->fade <= 0)
			{
				n->fade = 0;
				n->h.phase++;
			}
			break;
		case 2:
			if (Confirmed() == 1)
			{
				n->h.flags |= 1;
				n->h.phase++;
			}
			break;
		default: break;
		}
	}

	static uint32_t __cdecl BackdropTask(TaskNode *t)
	{
		ModelNode *n = (ModelNode *)t;
		BackdropPhase(n);
		DrawHudModel(n, false);
		return TaskEnd(&n->h);
	}

	static void AltSpinnerPhase(ModelNode *n)
	{
		switch (n->h.phase)
		{
		case 0:
			n->scale[0] = 0x1000;
			n->scale[1] = 0x1000;
			n->scale[2] = 0x1000;
			n->h.pos[0] = (int16_t)-0x200;
			n->h.pos[1] = 0;
			n->h.pos[2] = 0x6C0;
			n->model = game_addr(game::unk_15CF674);
			n->angle[1] = 0x400;
			n->h.phase++;
			break;
		case 1:
			n->angle[1] = (int16_t)(n->angle[1] - 0x80);
			if (n->angle[1] <= 0)
			{
				n->angle[1] = 0;
				n->h.phase++;
			}
			break;
		case 2:
			if (Closing() == 1)
			{
				n->h.flags |= 1;
				n->h.phase++;
			}
			break;
		default: break;
		}
	}

	static uint32_t __cdecl AltSpinnerTask(TaskNode *t)
	{
		ModelNode *n = (ModelNode *)t;
		AltSpinnerPhase(n);
		DrawHudModel(n, true);
		return TaskEnd(&n->h);
	}

	static uint32_t __cdecl SpriteTask(TaskNode *t)
	{
		SpriteNode *n = (SpriteNode *)t;
		switch (n->h.phase)
		{
		case 0:
			n->h.pos[0] = 0x190;
			n->h.pos[1] = 0xC0;
			n->h.pos[2] = 0x900;
			n->seq = game_addr(game::unk_15D07D4);
			n->last = 0x1F;
			n->h.phase++;
			break;
		case 1:
			n->frame++;
			if (n->frame > n->last)
			{
				*(uint8_t *)&n->h.flags |= 4;
				n->frame = n->last;
				*(uint8_t *)&n->h.flags &= 0xFB;
				n->seq = game_addr(game::unk_15D0AF8);
				n->frame = 0;
				n->last = 3;
				n->h.phase++;
			}
			break;
		case 2:
			n->frame++;
			if (n->frame > n->last) n->frame = 0;
			if (HudActive() == 0)
			{
				*(uint8_t *)&n->h.flags |= 1;
				n->h.phase++;
			}
			break;
		default: break;
		}
		DrawHudSprite(n, 0x400);
		return TaskEnd(&n->h);
	}

	static void TextOpen(CtlNode *n)
	{
		TextDone() = 0;
		n->lines = (int16_t)(uint16_t)ScanTextLines(n->h.slot);
		n->h.phase++;
	}

	static void TextShowLine(CtlNode *n)
	{
		int16_t rect[4] = { 0x10, (int16_t)0xB4, 0x128, 0x1C };
		const void *s = ScanTextLine(n->h.slot, (uint8_t)n->line);
		TextSetLayerText(1, s);
		TextLayer4A0700(1, 0);
		TextSetPosition(1, rect);
		TextLayer49FBC0(1, 0);
		TextLayer4A0640(1);
		n->timer = 0x2D;
		n->h.phase++;
	}

	static void TextClose(CtlNode *n)
	{
		TextLayer4A0680(1);
		TextSetLayerText(1, nullptr);
		TextDone() = 1;
		*(uint8_t *)&n->h.flags |= 1;
		n->h.phase++;
	}

	static uint32_t __cdecl TextTask(TaskNode *t)
	{
		CtlNode *n = (CtlNode *)t;
		n->prev = n->pad;
		n->pad = RemapPadInput(ReadPadHeldRaw(0, 0)) & 0xFFFF;
		switch (n->h.phase)
		{
		case 0: TextOpen(n); break;
		case 1: TextShowLine(n); break;
		case 2:
			if (n->pad & 0x40)
			{
				n->h.phase++;
				break;
			}
			if (!(n->prev & 0x20) && (n->pad & 0x20)) { }
			else
			{
				n->timer--;
				if (n->timer > 0) break;
			}
			n->line++;
			if (n->line >= n->lines) n->line = 0;
			n->h.phase--;
			break;
		case 3: TextClose(n); break;
		default: break;
		}
		return TaskEnd(&n->h);
	}

	static uint32_t __cdecl AltTextTask(TaskNode *t)
	{
		CtlNode *n = (CtlNode *)t;
		n->prev = n->pad;
		n->pad = RemapPadInput(ReadPadHeldRaw(0, 0)) & 0xFFFF;
		switch (n->h.phase)
		{
		case 0: TextOpen(n); break;
		case 1: TextShowLine(n); break;
		case 2:
			if (n->pad & 0x40)
			{
				n->h.phase++;
				break;
			}
			if (!(n->prev & 0x20) && (n->pad & 0x20)) { }
			else
			{
				n->timer--;
				if (n->timer > 0) break;
			}
			n->line++;
			if (n->line < n->lines) n->h.phase--;
			else n->h.phase++;
			break;
		case 3: TextClose(n); break;
		default: break;
		}
		return TaskEnd(&n->h);
	}

	static void ReadViewInput()
	{
		static constexpr game::Id CLEAR[] = { game::unk_269AABA, game::unk_269AAB8, game::unk_269AAB6, game::unk_269AAB4, game::unk_269AAC2, game::unk_269AAC0, game::unk_269AABE, game::unk_269AABC, game::unk_269AACE, game::unk_269AACC, game::unk_269AACA, game::unk_269AAC8, game::unk_269AAD6, game::unk_269AAD4, game::unk_269AAD2, game::unk_269AAD0, game::unk_269AAC6, game::unk_269AAC4 };
		for (game::Id w : CLEAR) InW(game_addr(w)) = 0;
		const uint32_t pad = RemapPadInput(ReadPadHeldRaw(0, 0)) & 0xFFFF;
		var<uint32_t>(game_addr(game::g_Scan_PadBits)) = pad;
		if (pad & 0xF000)
		{
			const uint8_t m = (uint8_t)pad;
			if (m & 3)
			{
				if (pad & 0x1000) InW(game_addr(game::unk_269AAD4)) = 8;
				else if (pad & 0x4000) InW(game_addr(game::unk_269AAD6)) = 8;
				if (pad & 0x8000) InW(game_addr(game::unk_269AAD0)) = 8;
				else if (pad & 0x2000) InW(game_addr(game::unk_269AAD2)) = 8;
			}
			else if (m & 0xC)
			{
				if (pad & 0x1000) InW(game_addr(game::unk_269AAC6)) = 8;
				else if (pad & 0x4000) InW(game_addr(game::unk_269AAC4)) = 8;
				if (pad & 0x8000) InW(game_addr(game::unk_269AAD0)) = 8;
				else if (pad & 0x2000) InW(game_addr(game::unk_269AAD2)) = 8;
			}
			else
			{
				if (pad & 0x1000) InW(game_addr(game::unk_269AACC)) = 8;
				else if (pad & 0x4000) InW(game_addr(game::unk_269AACE)) = 8;
				if (pad & 0x8000) InW(game_addr(game::unk_269AAC8)) = 8;
				else if (pad & 0x2000) InW(game_addr(game::unk_269AACA)) = 8;
			}
			return;
		}
		InW(game_addr(game::unk_269AAAC)) = (int16_t)ReadAnalog(0, 2, 0);
		if (InW(game_addr(game::unk_269AAAC)) < 0)
		{
			InW(game_addr(game::unk_269AAB2)) = 0x80;
			InW(game_addr(game::unk_269AAB0)) = 0x80;
			InW(game_addr(game::unk_269AAAE)) = 0x80;
			InW(game_addr(game::unk_269AAAC)) = 0x80;
		}
		else
		{
			InW(game_addr(game::unk_269AAAE)) = (int16_t)ReadAnalog(0, 3, 0);
			InW(game_addr(game::unk_269AAB0)) = (int16_t)ReadAnalog(0, 0, 0);
			InW(game_addr(game::unk_269AAB2)) = (int16_t)ReadAnalog(0, 1, 0);
			auto axis = [](uint32_t in, uint32_t lo, uint32_t hi)
			{
				const int16_t v = InW(in);
				if (v <= 0x60) InW(lo) = Div96((int32_t)(int16_t)(0x60 - v) << 4);
				else if (v >= 0x9F) InW(hi) = Div96((int32_t)(int16_t)(v - 0x9F) << 4);
			};
			axis(game_addr(game::unk_269AAAC), game_addr(game::unk_269AAB4), game_addr(game::unk_269AAB6));
			axis(game_addr(game::unk_269AAAE), game_addr(game::unk_269AAB8), game_addr(game::unk_269AABA));
			axis(game_addr(game::unk_269AAB0), game_addr(game::unk_269AABC), game_addr(game::unk_269AABE));
			axis(game_addr(game::unk_269AAB2), game_addr(game::unk_269AAC0), game_addr(game::unk_269AAC2));
		}
		const uint8_t m = var<uint8_t>(game_addr(game::g_Scan_PadBits));
		if (m & 3)
		{
			InW(game_addr(game::unk_269AAD0)) = InW(game_addr(game::unk_269AAB4));
			InW(game_addr(game::unk_269AAD2)) = InW(game_addr(game::unk_269AAB6));
			InW(game_addr(game::unk_269AAD4)) = InW(game_addr(game::unk_269AAB8));
			InW(game_addr(game::unk_269AAD6)) = InW(game_addr(game::unk_269AABA));
		}
		else if (m & 0xC)
		{
			InW(game_addr(game::unk_269AAC6)) = InW(game_addr(game::unk_269AAB8));
			InW(game_addr(game::unk_269AAC4)) = InW(game_addr(game::unk_269AABA));
			InW(game_addr(game::unk_269AAD0)) = InW(game_addr(game::unk_269AAB4));
			InW(game_addr(game::unk_269AAD2)) = InW(game_addr(game::unk_269AAB6));
		}
		else
		{
			InW(game_addr(game::unk_269AAC8)) = InW(game_addr(game::unk_269AAB4));
			InW(game_addr(game::unk_269AACA)) = InW(game_addr(game::unk_269AAB6));
			InW(game_addr(game::unk_269AACE)) = InW(game_addr(game::unk_269AABA));
			InW(game_addr(game::unk_269AACC)) = InW(game_addr(game::unk_269AAB8));
		}
	}

	static void ResetView(CtlNode *n)
	{
		uint8_t *ent = MainEntity();
		CamState &c = Cam();
		F<uint32_t>(ent, 0xC) = c.angles[0];
		F<uint32_t>(ent, 0x10) = c.angles[1];
		n->moved = 0;
		n->rot[0] = 0;
		n->rot[1] = 0;
		n->rot[2] = 0;
		n->scale[0] = 0x1000;
		n->scale[1] = 0x1000;
		n->scale[2] = 0x1000;
	}

	static void ViewerSetup(CtlNode *n)
	{
		uint8_t *ent = MainEntity();
		CamState &c = Cam();
		c.angles[0] = F<uint32_t>(ent, 0xC);
		c.angles[1] = F<uint32_t>(ent, 0x10);
		c.place[0] = F<uint32_t>(ent, 0x1C);
		c.place[1] = F<uint32_t>(ent, 0x20);
		F<int32_t *>(ent, 0x30) = n->scale;
		n->scale[0] = 0x1000;
		int16_t centre[4];
		n->scale[2] = 0x1000;
		n->scale[1] = 0x1000;
		GetEffectSpawnPosition(ent, 0xF1, 0, centre);
		const uint8_t *info = InfoOf(&n->h);
		ent = MainEntity();
		if (F<int16_t>(info, 8) == 0)
		{
			ff8fx::copy<(4)>(&centre[0], ent + 0x1C);
			ff8fx::copy<(4)>(&centre[2], ent + 0x20);
			centre[1] = 0;
		}
		n->offx = (int16_t)(F<int16_t>(ent, 0x1C) - centre[0]);
		n->offz = (int16_t)(F<int16_t>(ent, 0x20) - centre[2]);
	}

	static void ViewerTick(CtlNode *n)
	{
		const uint8_t *info = InfoOf(&n->h);
		ReadViewInput();
		const uint8_t m = var<uint8_t>(game_addr(game::g_Scan_PadBits));
		if ((m & 4) && (m & 1)) ResetView(n);
		uint8_t *ent;
		if (n->moved == 0)
		{
			n->idle = 0x4B;
			ent = MainEntity();
			if (info[0xD] == 1) F<int16_t>(ent, 0xE) = (int16_t)(F<int16_t>(ent, 0xE) + 0x40);
		}
		else
		{
			n->idle--;
			if (n->idle <= 0) ResetView(n);
			ent = MainEntity();
		}
		int16_t v = InW(game_addr(game::unk_269AAC8));
		if (v > 0)
		{
			n->moved = 1;
			n->idle = 0x4B;
			if (info[0xD] == 1) F<int16_t>(ent, 0xE) = (int16_t)(F<int16_t>(ent, 0xE) + (int16_t)(v << 4));
		}
		v = InW(game_addr(game::unk_269AACA));
		if (v > 0)
		{
			n->moved = 1;
			n->idle = 0x4B;
			if (info[0xD] == 1) F<int16_t>(ent, 0xE) = (int16_t)(F<int16_t>(ent, 0xE) + (int16_t)(v * -0x10));
		}
		v = InW(game_addr(game::unk_269AACC));
		if (v > 0)
		{
			n->moved = 1;
			n->idle = 0x4B;
			if (info[0xC] == 1) F<int16_t>(ent, 0xC) = (int16_t)(F<int16_t>(ent, 0xC) + (int16_t)shl32(-v, 4));
		}
		v = InW(game_addr(game::unk_269AACE));
		if (v > 0)
		{
			n->moved = 1;
			n->idle = 0x4B;
			if (info[0xC] == 1) F<int16_t>(ent, 0xC) = (int16_t)(F<int16_t>(ent, 0xC) + (int16_t)(v << 4));
		}
		v = InW(game_addr(game::unk_269AAC4));
		if (v > 0)
		{
			n->moved = 1;
			n->scale[0] += (int32_t)v << 4;
			n->idle = 0x4B;
			if (n->scale[0] > 0x2000) n->scale[0] = 0x2000;
			n->scale[2] = n->scale[0];
			n->scale[1] = n->scale[0];
		}
		else
		{
			v = InW(game_addr(game::unk_269AAC6));
			if (v > 0)
			{
				n->moved = 1;
				n->scale[0] += -((int32_t)v << 4);
				n->idle = 0x4B;
				if (n->scale[0] < 0x800) n->scale[0] = 0x800;
				n->scale[2] = n->scale[0];
				n->scale[1] = n->scale[0];
			}
		}
		v = InW(game_addr(game::unk_269AAD0));
		if (v > 0)
		{
			n->rot[0] = (int16_t)(n->rot[0] + (int16_t)shl32(-v, 3));
			n->moved = 1;
			n->idle = 0x4B;
		}
		v = InW(game_addr(game::unk_269AAD2));
		if (v > 0)
		{
			n->rot[0] = (int16_t)(n->rot[0] + (int16_t)(v << 3));
			n->moved = 1;
			n->idle = 0x4B;
		}
		v = InW(game_addr(game::unk_269AAD4));
		if (v > 0)
		{
			n->rot[1] = (int16_t)(n->rot[1] + (int16_t)shl32(-v, 3));
			n->moved = 1;
			n->idle = 0x4B;
		}
		v = InW(game_addr(game::unk_269AAD6));
		if (v > 0)
		{
			n->rot[1] = (int16_t)(n->rot[1] + (int16_t)(v << 3));
			n->moved = 1;
			n->idle = 0x4B;
		}
		F<uint16_t>(ent, 0xC) &= 0xFFF;
		F<uint16_t>(ent, 0xE) &= 0xFFF;
		F<uint16_t>(ent, 0x10) &= 0xFFF;
		CamState &c = Cam();
		F<uint32_t>(ent, 0x1C) = c.place[0];
		F<uint32_t>(ent, 0x20) = c.place[1];
		Mat4x3 m1;
		ComposeZYXRotationMatrix((const int16_t *)(ent + 0xC), &m1);
		Scale3DMatrix(&m1, n->scale);
		int16_t up[4];
		int16_t out[4];
		up[1] = F<int16_t>(info, 6);
		up[0] = 0;
		up[2] = 0;
		MatrixMulVector(&m1, up, out);
		ent = MainEntity();
		F<int16_t>(ent, 0x1C) = (int16_t)(F<int16_t>(ent, 0x1C) + out[0]);
		F<int16_t>(ent, 0x1E) = (int16_t)(F<int16_t>(ent, 0x1E) + out[1]);
		F<int16_t>(ent, 0x20) = (int16_t)(F<int16_t>(ent, 0x20) + out[2]);
		F<int16_t>(ent, 0x1E) = (int16_t)(F<int16_t>(ent, 0x1E) - F<int16_t>(info, 6));
		if (n->moved == 1 && F<int16_t>(info, 8) == 0)
		{
			F<int32_t>(ent, 0x54) = F<int16_t>(ent, 0x1C);
			F<int32_t>(ent, 0x58) = F<int16_t>(ent, 0x1E);
			F<int32_t>(ent, 0x5C) = F<int16_t>(ent, 0x20);
			int16_t p[4];
			GetEffectSpawnPosition(ent, 0xF1, 0, p);
			ent = MainEntity();
			F<int16_t>(ent, 0x1C) = (int16_t)(F<int16_t>(ent, 0x1C) + (int16_t)(var<int16_t>(game_addr(game::Battle_Camera_LookAt_XZ_s16)) - p[0]));
			F<int16_t>(ent, 0x1E) = (int16_t)(F<int16_t>(ent, 0x1E) + (int16_t)(var<int16_t>(game_addr(game::unk_B8B7FA)) - p[1]));
			F<int16_t>(ent, 0x20) = (int16_t)(F<int16_t>(ent, 0x20) + (int16_t)(var<int16_t>(game_addr(game::Battle_Camera_LookAt_Y)) - p[2]));
			F<int32_t>(ent, 0x54) = F<int16_t>(ent, 0x1C);
			F<int32_t>(ent, 0x58) = F<int16_t>(ent, 0x1E);
			F<int32_t>(ent, 0x5C) = F<int16_t>(ent, 0x20);
			Mat4x3 m3;
			ComposeZYXRotationMatrix((const int16_t *)c.angles, &m3);
			int16_t w[4];
			w[0] = (int16_t)-F<int16_t>(info, 2);
			w[1] = (int16_t)-F<int16_t>(info, 0xA);
			w[2] = 0;
			MatrixMulVector(&m3, w, out);
			ent = MainEntity();
			F<int16_t>(ent, 0x1C) = (int16_t)(F<int16_t>(ent, 0x1C) + out[0]);
			F<int16_t>(ent, 0x1E) = (int16_t)(F<int16_t>(ent, 0x1E) + out[1]);
			F<int16_t>(ent, 0x20) = (int16_t)(F<int16_t>(ent, 0x20) + out[2]);
			F<int32_t>(ent, 0x54) = F<int16_t>(ent, 0x1C);
			F<int32_t>(ent, 0x58) = F<int16_t>(ent, 0x1E);
			F<int32_t>(ent, 0x5C) = F<int16_t>(ent, 0x20);
			ComposeZYXRotationMatrix((const int16_t *)c.angles, &m3);
			MatrixMulVector(&m3, n->rot, out);
		}
		else
		{
			Mat4x3 m3;
			ComposeZYXRotationMatrix((const int16_t *)c.angles, &m3);
			MatrixMulVector(&m3, n->rot, out);
		}
		ent = MainEntity();
		F<int16_t>(ent, 0x1C) = (int16_t)(F<int16_t>(ent, 0x1C) + out[0]);
		F<int16_t>(ent, 0x1E) = (int16_t)(F<int16_t>(ent, 0x1E) + out[1]);
		F<int16_t>(ent, 0x20) = (int16_t)(F<int16_t>(ent, 0x20) + out[2]);
		F<int32_t>(ent, 0x54) = F<int16_t>(ent, 0x1C);
		F<int32_t>(ent, 0x58) = F<int16_t>(ent, 0x1E);
		F<int32_t>(ent, 0x5C) = F<int16_t>(ent, 0x20);
		for (int k = 0; k < 7; k++)
		{
			if (Scanned(k) != 1) continue;
			uint8_t *e = Entity(k);
			F<uint32_t>(e, 0x1C) = F<uint32_t>(ent, 0x1C);
			F<uint32_t>(e, 0x20) = F<uint32_t>(ent, 0x20);
			ff8fx::copy<(0x20)>(e + 0x40, ent + 0x40);
		}
	}

	static void RestoreScanned()
	{
		const CamState &c = Cam();
		for (int k = 0; k < 7; k++)
		{
			if (Scanned(k) != 1) continue;
			uint8_t *e = Entity(k);
			F<uint32_t>(e, 0xC) = c.angles[0];
			F<uint32_t>(e, 0x10) = c.angles[1];
			F<uint32_t>(e, 0x1C) = c.place[0];
			F<uint32_t>(e, 0x20) = c.place[1];
			F<uint32_t>(e, 0x30) = 0;
		}
	}

	static uint32_t __cdecl ViewerTask(TaskNode *t)
	{
		CtlNode *n = (CtlNode *)t;
		switch (n->h.phase)
		{
		case 0:
		{
			const void *name = n->h.slot <= 2 ? CharacterName(n->h.slot) : MonsterName(n->h.slot);
			ff8fx::copy<(0x18)>((void *)game_addr(game::unk_269AA78), name);
			int16_t rect[4] = { 0xAA, 0x1E, 0x88, 0x1C };
			TextSetLayerText(2, (const void *)game_addr(game::unk_269AA78));
			TextLayer4A0700(2, 0);
			TextSetPosition(2, rect);
			TextLayer4A0640(2);
			const void *info = ScanTextInfo(n->h.slot);
			rect[0] = 0xAA; rect[1] = 0x34; rect[2] = 0x88; rect[3] = 0x80;
			TextSetLayerText(0, info);
			TextLayer4A0700(0, 0);
			TextSetPosition(0, rect);
			TextLayer4A0640(0);
			((CtlNode *)AddEffectTask(QControls(), game_addr(ORIG_Text), 0x78, n))->timer = 0;
			AddEffectTask(QModels(), game_addr(ORIG_Backdrop), 0x6C, n);
			ViewerSetup(n);
			n->h.phase++;
			break;
		}
		case 1:
			ViewerTick(n);
			if (TextDone() == 1) n->h.phase++;
			break;
		case 2:
			ViewerTick(n);
			if ((uint8_t)RemapPadInput(ReadPadHeldRaw(0, 0)) & 0x40)
			{
				n->timer = 0x12;
				Confirmed() = 1;
				Closing() = 1;
				n->h.phase++;
			}
			break;
		case 3:
			ViewerTick(n);
			n->timer--;
			if (n->timer <= 0) n->h.phase++;
			break;
		case 4:
			RestoreScanned();
			TextLayer4A0680(2);
			TextSetLayerText(2, nullptr);
			TextLayer4A0680(0);
			TextSetLayerText(0, nullptr);
			*(uint8_t *)&n->h.flags |= 1;
			n->h.phase++;
			break;
		default: break;
		}
		return TaskEnd(&n->h);
	}

	static void MarkTarget(CtlNode *n)
	{
		Entity(n->h.slot)[1] |= 0x78;
	}

	static uint32_t __cdecl ScannerTask(TaskNode *t)
	{
		CtlNode *n = (CtlNode *)t;
		switch (n->h.phase)
		{
		case 0:
			MarkTarget(n);
			((CtlNode *)AddEffectTask(QControls(), game_addr(ORIG_Camera), 0x78, n))->mode = 0;
			HudActive() = 1;
			AddEffectTask(QControls(), game_addr(ORIG_Reticle), 0x78, n);
			n->timer = 0xF;
			n->h.phase++;
			break;
		case 1:
			n->timer--;
			if (n->timer < 0)
			{
				AddEffectTask(QControls(), game_addr(ORIG_Dimmer), 0x78, n);
				n->timer = 0x10;
				n->h.phase++;
			}
			break;
		case 2:
			n->timer--;
			if (n->timer < 0)
			{
				AddEffectTask(QControls(), game_addr(ORIG_Bars), 0x78, n);
				n->timer = 0x20;
				n->h.phase++;
			}
			break;
		case 3:
			n->timer--;
			if (n->timer < 0)
			{
				HudActive() = 0;
				n->h.phase++;
			}
			break;
		case 4:
			((CtlNode *)AddEffectTask(QControls(), game_addr(ORIG_Camera), 0x78, n))->mode = 1;
			n->timer = 8;
			n->h.phase++;
			break;
		case 5:
			n->timer--;
			if (n->timer < 0)
			{
				AddEffectTask(QControls(), game_addr(ORIG_Viewer), 0x78, n);
				AddEffectTask(QModels(), game_addr(ORIG_Spinner), 0x6C, n);
				AddEffectTask(QModels(), game_addr(ORIG_Board), 0x6C, n);
				n->h.phase++;
			}
			break;
		case 6:
			if (n->h.children == 0)
			{
				uint8_t *e = Entity(n->h.slot);
				if ((int16_t)n->h.action == ((RootNode *)n->h.root)->last_action) AddEffectTask(QControls(), game_addr(ORIG_CamRestore), 0x78, n);
				else if (Cam().dist <= (int16_t)0xE000) ((CtlNode *)AddEffectTask(QControls(), game_addr(ORIG_Camera), 0x78, n))->mode = 3;
				AddEffectTask(QControls(), game_addr(ORIG_Restorer), 0x78, n);
				F<uint16_t>(e, 0) &= 0x87FF;
				n->h.phase++;
			}
			break;
		case 7:
			if (n->h.children == 0)
			{
				*(uint8_t *)&n->h.flags |= 1;
				n->h.phase++;
			}
			break;
		default: break;
		}
		return TaskEnd(&n->h);
	}

	static uint32_t __cdecl AltScannerTask(TaskNode *t)
	{
		CtlNode *n = (CtlNode *)t;
		switch (n->h.phase)
		{
		case 0:
			MarkTarget(n);
			((CtlNode *)AddEffectTask(QControls(), game_addr(ORIG_Camera), 0x78, n))->mode = 2;
			HudActive() = 1;
			AddEffectTask(QControls(), game_addr(ORIG_Reticle), 0x78, n);
			n->timer = 0x2D;
			n->h.phase++;
			break;
		case 1:
			n->timer--;
			if (n->timer < 0)
			{
				AddEffectTask(QModels(), game_addr(ORIG_AltSpinner), 0x6C, n);
				n->timer = 8;
				n->h.phase++;
			}
			break;
		case 2:
			n->timer--;
			if (n->timer < 0)
			{
				AddEffectTask(QControls(), game_addr(ORIG_AltText), 0x78, n);
				AddEffectTask(QModels(), game_addr(ORIG_AltBoard), 0x6C, n);
				n->h.phase++;
			}
			break;
		case 3:
		{
			const uint8_t *info = InfoOf(&n->h);
			CamState &c = Cam();
			if (info[0xE] == 1) c.yaw = (int16_t)((c.yaw + 0x40) & 0xFFF);
			if (F<int16_t>(info, 4) < (int16_t)0xFE80)
			{
				c.h = (int16_t)(c.h - c.h1 / 32);
				if (c.h > 0) c.h = 0;
			}
			CameraApply();
			if (TextDone() == 1)
			{
				HudActive() = 0;
				Closing() = 1;
				n->h.phase++;
			}
			break;
		}
		case 4:
			if (n->h.children == 0)
			{
				uint8_t *e = Entity(n->h.slot);
				if ((int16_t)n->h.action == ((RootNode *)n->h.root)->last_action)
				{
					AddEffectTask(QControls(), game_addr(ORIG_CamRestore), 0x78, n);
					F<uint16_t>(e, 0) &= 0x87FF;
					n->h.phase++;
					break;
				}
				if (Cam().dist <= (int16_t)0xE000) ((CtlNode *)AddEffectTask(QControls(), game_addr(ORIG_Camera), 0x78, n))->mode = 3;
				F<uint16_t>(e, 0) &= 0x87FF;
				n->h.phase++;
			}
			break;
		case 5:
			if (n->h.children == 0)
			{
				*(uint8_t *)&n->h.flags |= 1;
				n->h.phase++;
			}
			break;
		default: break;
		}
		return TaskEnd(&n->h);
	}

	static uint32_t __cdecl EmitterTask(TaskNode *t)
	{
		EmitterNode *e = (EmitterNode *)t;
		if (e->h.counter == 0)
		{
			int16_t mid[4];
			CopyMidpointFromSource(e, mid);
			BdPlaySE(SOUND_Scan(), 0, 0x80);
		}
		EmitterUpdatePos(e);
		ComputeModelBounds(e);
		switch (e->h.phase)
		{
		case 0:
		{
			Confirmed() = 0;
			Closing() = 0;
			FindScannedEntities((int16_t)(uint16_t)e->h.slot);
			const uint8_t *rec = e->h.ctx->actions[0].targets + 24 * (int32_t)e->h.action;
			AddEffectTask(QControls(), (rec[2] & 2) ? game_addr(ORIG_AltScanner) : game_addr(ORIG_Scanner), 0x78, e);
			e->h.phase++;
			break;
		}
		case 1:
			if (e->h.children == 0)
			{
				((RootNode *)e->h.root)->busy = 0;
				ApplyActionResultToTarget(e->h.ctx->actions[e->h.action].targets);
				*(uint8_t *)&e->h.flags |= 1;
				e->h.phase++;
			}
			break;
		default: break;
		}
		return TaskEnd(&e->h);
	}

	static void RootPhase(RootNode *r)
	{
		switch (r->h.phase)
		{
		case 0:
			r->h.phase++;
			break;
		case 1:
			r->busy = 1;
			AddEffectTask(QEmitters(), game_addr(ORIG_Emitter), 0x58, r);
			CameraArmReturn();
			BattleMenuShow(0);
			r->h.phase++;
			break;
		case 2:
			if (r->busy == 0)
			{
				const int8_t a = r->h.action;
				if ((int16_t)a < r->last_action)
				{
					r->h.action = (int8_t)(a + 1);
					r->h.phase--;
				}
				else r->h.phase++;
			}
			break;
		case 3:
			if (r->alive == 0)
			{
				BattleMenuShow(1);
				*(uint8_t *)&r->h.flags |= 1;
				r->h.phase++;
			}
			break;
		default: break;
		}
	}

	static uint32_t __cdecl RootTask(TaskNode *t)
	{
		RootNode *r = (RootNode *)t;
		if (r->tick & 1) PacketCursor() = var<uint32_t>(game_addr(game::unk_269CFC4));
		else PacketCursor() = var<uint32_t>(game_addr(game::unk_269CFC0));
		UpdateTargetPosFromBones(r);
		RootPhase(r);
		r->alive = (int16_t)ExecuteTaskQueue(QEmitters());
		r->alive = (int16_t)(r->alive + ExecuteTaskQueue(QControls()));
		r->alive = (int16_t)(r->alive + ExecuteTaskQueue(QSprites()));
		r->alive = (int16_t)(r->alive + ExecuteTaskQueue(QModels()));
		r->tick++;
		return TaskEnd(&r->h);
	}
}

	void register_mag040_scan()
	{
		using namespace scan040;
		register_port(ORIG_Root, (void *)RootTask, "040 Root", 40);
		register_port(ORIG_Emitter, (void *)EmitterTask, "040 Emitter", 40);
		register_port(ORIG_Scanner, (void *)ScannerTask, "040 Scanner", 40);
		register_port(ORIG_AltScanner, (void *)AltScannerTask, "040 AltScanner", 40);
		register_port(ORIG_Reticle, (void *)ReticleTask, "040 Reticle", 40);
		register_port(ORIG_Pair, (void *)PairTask, "040 Pair", 40);
		register_port(ORIG_Morph, (void *)MorphTask, "040 Morph", 40);
		register_port(ORIG_Corner, (void *)CornerTask, "040 Corner", 40);
		register_port(ORIG_Bracket, (void *)BracketTask, "040 Bracket", 40);
		register_port(ORIG_Ring, (void *)RingTask, "040 Ring", 40);
		register_port(ORIG_Sprite, (void *)SpriteTask, "040 Sprite", 40);
		register_port(ORIG_Camera, (void *)CameraTask, "040 Camera", 40);
		register_port(ORIG_Dimmer, (void *)DimmerTask, "040 Dimmer", 40);
		register_port(ORIG_Bars, (void *)BarsTask, "040 Bars", 40);
		register_port(ORIG_Spinner, (void *)SpinnerTask, "040 Spinner", 40);
		register_port(ORIG_Board, (void *)BoardTask, "040 Board", 40);
		register_port(ORIG_StatBar, (void *)StatBarTask, "040 StatBar", 40);
		register_port(ORIG_Viewer, (void *)ViewerTask, "040 Viewer", 40);
		register_port(ORIG_Backdrop, (void *)BackdropTask, "040 Backdrop", 40);
		register_port(ORIG_Text, (void *)TextTask, "040 Text", 40);
		register_port(ORIG_Restorer, (void *)RestorerTask, "040 Restorer", 40);
		register_port(ORIG_CamRestore, (void *)CamRestoreTask, "040 CamRestore", 40);
		register_port(ORIG_AltSpinner, (void *)AltSpinnerTask, "040 AltSpinner", 40);
		register_port(ORIG_AltBoard, (void *)AltBoardTask, "040 AltBoard", 40);
		register_port(ORIG_AltText, (void *)AltTextTask, "040 AltText", 40);
	}
}

