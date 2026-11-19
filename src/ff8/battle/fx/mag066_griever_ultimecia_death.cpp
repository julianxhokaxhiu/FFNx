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
namespace gu066
{
	using namespace eng;
	using namespace magc;

	template<typename T> static inline T &At(uint32_t base, int32_t off) { return *(T *)(base + off); }

	inline int32_t &Pool3Cursor() { return var<int32_t>(game_addr(game::g066_Pool3Cursor)); }
	inline TaskQueue *QRoot() { return (TaskQueue *)game_addr(game::g066_RootQueue); }
	inline TaskQueue *QMaster() { return (TaskQueue *)game_addr(game::g066_MasterQueue); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::g066_CastContext)); }
	inline int32_t &CasterSlot() { return var<int32_t>(game_addr(game::SLOT_ID_LINKED_GRIEVER)); }
	inline int16_t *Centre() { return (int16_t *)game_addr(game::g066_Centre); }
	inline int16_t &Push() { return var<int16_t>(game_addr(game::g066_Push)); }
	inline uint32_t &TexFile() { return var<uint32_t>(game_addr(game::g066_TexFile)); }
	inline uint32_t &ShardTable() { return var<uint32_t>(game_addr(game::g066_ShardTable)); }
	inline int32_t &ShardCursor() { return var<int32_t>(game_addr(game::g066_ShardCursor)); }
	inline uint32_t &FaceStates() { return var<uint32_t>(game_addr(game::g066_FaceStates)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::g066_TexBase)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::g066_PacketCursor)); }
	inline uint32_t &Voice() { return var<uint32_t>(game_addr(game::g066_VoiceSlot)); }
	inline uint32_t &Pool4() { return var<uint32_t>(game_addr(game::g066_Pool4Ptr)); }
	inline uint32_t &Pool3() { return var<uint32_t>(game_addr(game::g066_Pool3Ptr)); }
	inline uint32_t &FrameArena() { return var<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)); }
	inline uint32_t OT44() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44; }
	inline uint8_t *Caster() { return Entity(CasterSlot()); }

	static constexpr game::Id POOL1 = game::g066_Pool1, POOL1_END = game::g066_Pool2;
	static constexpr game::Id POOL2 = game::g066_Pool2, POOL2_END = game::g066_MasterQueue;
	static constexpr game::Id CASTER_FRAME = game::g066_CasterFrame;
	static constexpr game::Id CASTER_VIEW = game::g066_CasterView;
	static constexpr game::Id BILLBOARD = game::g066_AnchorMatrix;
	static constexpr game::Id SPAWN_BONES = game::a2;
	static constexpr game::Id SPARK_COLOURS = game::g066_SparkColourCycle;
	static constexpr game::Id GLOW_COLOURS = game::g066_GlowColourCycle;
	static constexpr game::Id RAIN_COLOURS = game::g066_RainColourCycle;

	static constexpr game::Id ORIG_RootTask = game::MAG_066_GrieverDeath_RootTask;
	static constexpr game::Id ORIG_Master = game::MAG_066_GrieverDeath_MasterTask;
	static constexpr game::Id ORIG_FadeTask = game::MAG_066_GrieverDeath_FadeTask;
	static constexpr game::Id ORIG_FeedbackA = game::MAG_066_GrieverDeath_FeedbackTaskA;
	static constexpr game::Id ORIG_FeedbackB = game::MAG_066_GrieverDeath_FeedbackTaskB;
	static constexpr game::Id ORIG_SparkTask = game::MAG_066_GrieverDeath_SparkTask;
	static constexpr game::Id ORIG_SpawnerTask = game::MAG_066_GrieverDeath_SpawnerTask;
	static constexpr game::Id ORIG_RingTask = game::MAG_066_GrieverDeath_RingTask;
	static constexpr game::Id ORIG_PuffTask = game::MAG_066_GrieverDeath_PuffTask;
	static constexpr game::Id ORIG_DustTask = game::MAG_066_GrieverDeath_DustTask;
	static constexpr game::Id ORIG_SmokeTask = game::MAG_066_GrieverDeath_SmokeTask;
	static constexpr game::Id ORIG_MeltTask = game::MAG_066_GrieverDeath_MeltTask;
	static constexpr game::Id ORIG_GlowTask = game::MAG_066_GrieverDeath_GlowTask;
	static constexpr game::Id ORIG_WaveTask = game::MAG_066_GrieverDeath_WaveTask;
	static constexpr game::Id ORIG_SwirlTask = game::MAG_066_GrieverDeath_SwirlTask;
	static constexpr game::Id ORIG_FlareTask = game::MAG_066_GrieverDeath_FlareTask;
	static constexpr game::Id ORIG_ShardSpin = game::MAG_066_GrieverDeath_ShardSpinTask;
	static constexpr game::Id ORIG_SparkRain = game::MAG_066_GrieverDeath_SparkRainTask;
	static constexpr game::Id ORIG_Billboard = game::MAG_066_GrieverDeath_BillboardTask;
	static constexpr game::Id ORIG_Billboard2 = game::MAG_066_GrieverDeath_Billboard2Task;
	static constexpr game::Id ORIG_DustFall2 = game::MAG_066_GrieverDeath_DustFall2Task;
	static constexpr game::Id ORIG_DustFall = game::MAG_066_GrieverDeath_DustFallTask;
	static constexpr game::Id ORIG_StageTask = game::MAG_066_GrieverDeath_StageTask;

	inline int32_t PauseQuery() { return fn<int32_t (__cdecl *)()>(game_addr(game::BattleFile_GetLoadingState))(); }
	inline void CameraScriptStart(uint32_t script, uint32_t frame, void *entity, TaskQueue *q, int32_t n) { fn<void (__cdecl *)(uint32_t, uint32_t, void *, TaskQueue *, int32_t)>(game_addr(game::MAG066_CamScript_Start))(script, frame, entity, q, n); }
	inline void ArmCameraReturn() { fn<void (__cdecl *)()>(game_addr(game::BS_Camera_ArmReturnAfterEffect))(); }
	inline void Sub4A8480(int32_t a) { fn<void (__cdecl *)(int32_t)>(game_addr(game::BattleMenu_SetVisible))(a); }
	inline int32_t VoiceBusy(uint32_t v) { return fn<int32_t (__cdecl *)(uint32_t)>(game_addr(game::BdSound_IsVoiceSlotActive))(v); }
	inline void VoiceRelease(uint32_t v) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::BdSound_ReleaseVoiceSlot))(v); }
	inline void ChainTransformation(void *entity, int32_t id) { fn<void (__cdecl *)(void *, int32_t)>(game_addr(game::QueueChainTransformation))(entity, id); }
	inline void MusicVolumeTrans(int32_t a, int32_t b, int32_t c) { fn<void (__cdecl *)(int32_t, int32_t, int32_t)>(game_addr(game::Music_SetVolumeTrans))(a, b, c); }
	inline void MusicStop(int32_t a) { fn<void (__cdecl *)(int32_t)>(game_addr(game::Music_StopChannelOrAll))(a); }
	inline void SfxVolumeTrans(int32_t a, int32_t b) { fn<void (__cdecl *)(int32_t, int32_t)>(game_addr(game::Sfx_SetAllChannelsVolumeTrans))(a, b); }
	inline void Sub46B450(int32_t a, int32_t b) { fn<void (__cdecl *)(int32_t, int32_t)>(game_addr(game::sub_46B450))(a, b); }
	inline void QueueTIMUpload(uint32_t tim) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::Battle_QueueTIMUpload_GetEOF))(tim); }
	inline void BattleMessage(int32_t slot, int32_t a, int32_t b) { fn<void (__cdecl *)(int32_t, int32_t, int32_t)>(game_addr(game::printMonsterRelatedText))(slot, a, b); }
	inline int32_t GetEffectSpawnPosition(void *entity, int32_t bone, int32_t flag, int16_t *out) { return fn<int32_t (__cdecl *)(void *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, flag, out); }
	inline void ScreenFeedback(int32_t a, int32_t r, int32_t g, int32_t b, int32_t c) { fn<void (__cdecl *)(int32_t, int32_t, int32_t, int32_t, int32_t)>(game_addr(game::Battle_RequestScreenFeedback))(a, r, g, b, c); }
	inline void ScreenFadeColour(int32_t r, int32_t g, int32_t b) { fn<void (__cdecl *)(int32_t, int32_t, int32_t)>(game_addr(game::Effect_ScreenFadeColour))(r, g, b); }
	inline void NormalizeSVector(const int16_t *in, int16_t *out) { fn<void (__cdecl *)(const int16_t *, int16_t *)>(game_addr(game::VectorNormalS16))(in, out); }
	inline uint32_t GlowRender(void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::MAG_066_RenderScrollPrimModel))(h, ot, mode, cursor); }
	inline uint32_t RenderGeometry(uint32_t group, void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(uint32_t, void *, uint32_t, int32_t, uint32_t)>(game_addr(game::RenderGeometry))(group, h, ot, mode, cursor); }
	inline void GteSetFarColour(uint32_t r, uint32_t g, uint32_t b) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_SetFarColor))(r, g, b); }
	inline void GteLoadV012(uint32_t a, uint32_t b, uint32_t c) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_LoadV012))(a, b, c); }
	inline void GteRTPT() { fn<void (__cdecl *)()>(game_addr(game::GTE_RTPT))(); }
	inline void GteReadFLAG(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadFLAG))(dst); }
	inline void GteNCLIP() { fn<void (__cdecl *)()>(game_addr(game::GTE_NCLIP))(); }
	inline void GteReadMAC0(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadMAC0))(dst); }
	inline void GteReadSXY012Split(uint32_t a, uint32_t b, uint32_t c) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_ReadSXY012_Split))(a, b, c); }
	inline void GteAVSZ3() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ3))(); }
	inline void GteAVSZ4() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ4))(); }
	inline void GteStoreOTZ(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadOTZ))(dst); }
	inline void GteLoadRGBC(uint32_t c) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::GTE_LoadRGBC))(c); }
	inline void GteDPCS() { fn<void (__cdecl *)()>(game_addr(game::GTE_DPCS))(); }
	inline void GteStoreRGB2(uint32_t c) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::GTE_StoreRGB2))(c); }
	inline void GteLoadRGB012(uint32_t a, uint32_t b, uint32_t c) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_LoadRGB012))(a, b, c); }
	inline void GteDPCT() { fn<void (__cdecl *)()>(game_addr(game::GTE_DPCT))(); }
	inline void GteStoreRGB012(uint32_t a, uint32_t b, uint32_t c) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_StoreRGB012))(a, b, c); }
	inline void GteSetLightMatrix(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_SetLightMatrix))(m); }
	inline void GteMVMVA_LightV0Bk() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_LightV0_Bk))(); }
	inline void GteMVMVA_RotV0Tr2() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0_Tr_2))(); }
	inline void GteLoadV0Words(uint32_t v) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::GTE_LoadV0FromSVec3))(v); }
	inline void GteStoreIRVertex(uint32_t out) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::GTE_StoreIR123ToSVec))(out); }
	inline void GteSetTransFromVec32(const int32_t *v) { fn<void (__cdecl *)(const int32_t *)>(game_addr(game::GTE_SetTransFromVec32))(v); }
	inline void GteSetBackFromTrans(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_SetBackColorFromTrans))(m); }
	inline void GteSetRotScale(int32_t s) { fn<void (__cdecl *)(int32_t)>(game_addr(game::GTE_SetRotScale))(s); }
	inline void GteTransFromMAC() { fn<void (__cdecl *)()>(game_addr(game::GTE_TransFromMAC))(); }

#pragma pack(push, 1)
	struct RenderHeader
	{
		uint32_t model;
		uint32_t verts;
		uint8_t far_rgb[4];
		int32_t fade;
		uint16_t u_off;
		uint16_t pad12;
		uint16_t v_off;
		uint16_t pad16;
		int32_t depth;
		uint32_t flags;
		uint32_t colour;
		uint32_t list;
		int32_t mac0;
		uint32_t pad2C;
		int32_t otz;
		uint32_t gte_flag;
		uint8_t pad38[0x20];
		uint32_t scale_rgb;
		uint32_t c[4];
	};
#pragma pack(pop)
	static_assert(sizeof(RenderHeader) == 0x6C, "Griever death render header");

	static uint16_t g_frame_pad = 0;
	static inline uint16_t SpawnPadWord() { return g_frame_pad; }
	static inline void NotePad(uint32_t esp4_word) { g_frame_pad = (uint16_t)(esp4_word >> 16); }
}
}

namespace ff8fx
{
namespace gu066
{
	static inline bool OutX(uint32_t xy) { int16_t v = *(const int16_t *)xy; return v < 0 || v > 0xA00; }
	static inline bool OutY(uint32_t xy) { int16_t v = *(const int16_t *)(xy + 2); return v < 0 || v > 0x6C0; }

	static inline void UvOffsets(const RenderHeader *h, uint32_t pkt, uint32_t u_word, uint32_t v_word)
	{
		const uint32_t fl = h->flags;
		if (fl & 0x400) At<uint16_t>(pkt, u_word) = (uint16_t)(At<uint16_t>(pkt, u_word) + h->u_off);
		else if (fl & 0x100) At<uint16_t>(pkt, u_word) = h->u_off;
		if (fl & 0x800) At<uint16_t>(pkt, v_word) = (uint16_t)(At<uint16_t>(pkt, v_word) + h->v_off);
		else if (fl & 0x200) At<uint16_t>(pkt, v_word) = h->v_off;
	}

	static inline void ScaleColours(RenderHeader *h, int n)
	{
		h->scale_rgb = h->colour;
		uint8_t *s = (uint8_t *)&h->scale_rgb;
		for (int k = 0; k < n; k++)
		{
			uint8_t *c = (uint8_t *)&h->c[k];
			c[0] = (uint8_t)((uint32_t)c[0] * s[0] >> 7);
			c[1] = (uint8_t)((uint32_t)c[1] * s[1] >> 7);
			c[2] = (uint8_t)((uint32_t)c[2] * s[2] >> 7);
		}
	}

	static inline uint32_t Bucket(uint32_t ot, const RenderHeader *h, int32_t shift) { return ot + (uint32_t)(h->otz >> shift) * 4; }

	static uint32_t ListF3(RenderHeader *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		const uint32_t list = h->list;
		int32_t count = At<int32_t>(list, 0);
		uint32_t rec = list + 4;
		h->list = rec;
		const uint32_t vb = h->verts;
		if (count <= 0)
		{
			h->list = rec;
			return cursor;
		}
		uint32_t pkt = cursor;
		do
		{
			GteLoadV012(vb + (uint32_t)At<uint16_t>(rec, 4) * 4, vb + (uint32_t)At<uint16_t>(rec, 6) * 4, vb + (uint32_t)At<uint16_t>(rec, 8) * 4);
			GteRTPT();
			const uint32_t code = h->colour | 0x20000000;
			const uint32_t fl = h->flags;
			At<uint32_t>(pkt, 0) = 0x4000000;
			At<uint32_t>(pkt, 4) = code;
			if (fl & 1) At<uint32_t>(pkt, 4) = code | 0x2000000;
			if (fl & 4) At<uint32_t>(pkt, 4) &= 0xFDFFFFFF;
			GteReadFLAG(&h->gte_flag);
			if (!(h->gte_flag & 0x60000))
			{
				GteNCLIP();
				GteReadMAC0(&h->mac0);
				const int32_t mac0 = h->mac0;
				if (mac0 != 0 && (mac0 > 0 || (h->flags & 0x10)))
				{
					GteReadSXY012Split(pkt + 8, pkt + 0xC, pkt + 0x10);
					GteAVSZ3();
					uint32_t clip = OutX(pkt + 8) ? 1 : 0;
					if (OutX(pkt + 0xC)) clip |= 2;
					if (OutX(pkt + 0x10)) clip |= 4;
					if (OutY(pkt + 8)) clip |= 0x10;
					if (OutY(pkt + 0xC)) clip |= 0x20;
					if (OutY(pkt + 0x10)) clip |= 0x40;
					if ((clip & 7) != 7 && (clip & 0x70) != 0x70)
					{
						GteStoreOTZ(&h->otz);
						if (h->flags & 0x40)
						{
							GteLoadRGBC(pkt + 4);
							GteSetIR0(h->fade);
							GteDPCS();
							GteStoreRGB2(pkt + 4);
						}
						InsertPrimAutoDepth(Bucket(ot, h, shift), (void *)pkt);
						pkt += 0x14;
					}
				}
			}
			rec += 0xC;
		} while (--count);
		h->list = rec;
		return pkt;
	}

	static uint32_t ListF4(RenderHeader *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		const uint32_t list = h->list;
		int32_t count = At<int32_t>(list, 0);
		uint32_t rec = list + 4;
		h->list = rec;
		const uint32_t vb = h->verts;
		if (count <= 0)
		{
			h->list = rec;
			return cursor;
		}
		uint32_t pkt = cursor;
		do
		{
			GteLoadV012(vb + (uint32_t)At<uint16_t>(rec, 4) * 4, vb + (uint32_t)At<uint16_t>(rec, 6) * 4, vb + (uint32_t)At<uint16_t>(rec, 8) * 4);
			GteRTPT();
			const uint32_t code = h->colour | 0x28000000;
			const uint32_t fl = h->flags;
			At<uint32_t>(pkt, 0) = 0x5000000;
			At<uint32_t>(pkt, 4) = code;
			if (fl & 1) At<uint32_t>(pkt, 4) = code | 0x2000000;
			if (fl & 4) At<uint32_t>(pkt, 4) &= 0xFDFFFFFF;
			GteReadFLAG(&h->gte_flag);
			if (!(h->gte_flag & 0x60000))
			{
				GteNCLIP();
				GteReadMAC0(&h->mac0);
				const int32_t mac0 = h->mac0;
				if (mac0 != 0 && (mac0 > 0 || (h->flags & 0x10)))
				{
					GteReadSXY012Split(pkt + 8, pkt + 0xC, pkt + 0x10);
					GteLoadV0((const void *)(vb + (uint32_t)At<uint16_t>(rec, 0xA) * 4));
					GteRTPS();
					uint32_t clip = OutX(pkt + 8) ? 1 : 0;
					if (OutX(pkt + 0xC)) clip |= 2;
					if (OutX(pkt + 0x10)) clip |= 4;
					if (OutY(pkt + 8)) clip |= 0x10;
					if (OutY(pkt + 0xC)) clip |= 0x20;
					if (OutY(pkt + 0x10)) clip |= 0x40;
					GteReadSXY2((void *)(pkt + 0x14));
					GteAVSZ4();
					if (OutX(pkt + 0x14)) clip |= 8;
					if (OutY(pkt + 0x14)) clip |= 0x80;
					if ((clip & 0xF) != 0xF && (clip & 0xF0) != 0xF0)
					{
						GteStoreOTZ(&h->otz);
						if (h->flags & 0x40)
						{
							GteLoadRGBC(pkt + 4);
							GteSetIR0(h->fade);
							GteDPCS();
							GteStoreRGB2(pkt + 4);
						}
						InsertPrimAutoDepth(Bucket(ot, h, shift), (void *)pkt);
						pkt += 0x18;
					}
				}
			}
			rec += 0xC;
		} while (--count);
		h->list = rec;
		return pkt;
	}

	static uint32_t ListFT3(RenderHeader *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		const uint32_t list = h->list;
		int32_t count = At<int32_t>(list, 0);
		uint32_t rec = list + 4;
		h->list = rec;
		const uint32_t vb = h->verts;
		if (count <= 0)
		{
			h->list = rec;
			return cursor;
		}
		uint32_t pkt = cursor;
		do
		{
			GteLoadV012(vb + (uint32_t)At<uint16_t>(rec, 4) * 4, vb + (uint32_t)At<uint16_t>(rec, 6) * 4, vb + (uint32_t)At<uint16_t>(rec, 8) * 4);
			GteRTPT();
			uint32_t code = h->colour | 0x24000000;
			At<uint32_t>(pkt, 0) = 0x7000000;
			At<uint32_t>(pkt, 4) = code;
			if (h->flags & 1) At<uint32_t>(pkt, 4) = code | 0x2000000;
			if (h->flags & 4) At<uint32_t>(pkt, 4) &= 0xFDFFFFFF;
			const uint32_t d = (uint32_t)h->depth;
			At<uint32_t>(pkt, 0xC) = At<uint32_t>(rec, 0xC) + d;
			At<uint32_t>(pkt, 0x14) = At<uint32_t>(rec, 0x10) + d;
			At<uint32_t>(pkt, 0x1C) = (At<uint32_t>(rec, 8) >> 16) + d;
			GteReadFLAG(&h->gte_flag);
			if (!(h->gte_flag & 0x60000))
			{
				GteNCLIP();
				UvOffsets(h, pkt, 0x16, 0xE);
				GteReadMAC0(&h->mac0);
				const int32_t mac0 = h->mac0;
				if (mac0 != 0 && (mac0 > 0 || (h->flags & 0x10)))
				{
					GteReadSXY012Split(pkt + 8, pkt + 0x10, pkt + 0x18);
					GteAVSZ3();
					uint32_t clip = OutX(pkt + 8) ? 1 : 0;
					if (OutX(pkt + 0x10)) clip |= 2;
					if (OutX(pkt + 0x18)) clip |= 4;
					if (OutY(pkt + 8)) clip |= 0x10;
					if (OutY(pkt + 0x10)) clip |= 0x20;
					if (OutY(pkt + 0x18)) clip |= 0x40;
					if ((clip & 7) != 7 && (clip & 0x70) != 0x70)
					{
						GteStoreOTZ(&h->otz);
						if (h->flags & 0x40)
						{
							GteLoadRGBC(pkt + 4);
							GteSetIR0(h->fade);
							GteDPCS();
							GteStoreRGB2(pkt + 4);
						}
						InsertPrimAutoDepth(Bucket(ot, h, shift), (void *)pkt);
						pkt += 0x20;
					}
				}
			}
			rec += 0x14;
		} while (--count);
		h->list = rec;
		return pkt;
	}

	static uint32_t ListFT4(RenderHeader *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		const uint32_t list = h->list;
		int32_t count = At<int32_t>(list, 0);
		uint32_t rec = list + 4;
		h->list = rec;
		const uint32_t vb = h->verts;
		if (count <= 0)
		{
			h->list = rec;
			return cursor;
		}
		uint32_t pkt = cursor;
		do
		{
			GteLoadV012(vb + (uint32_t)At<uint16_t>(rec, 4) * 4, vb + (uint32_t)At<uint16_t>(rec, 6) * 4, vb + (uint32_t)At<uint16_t>(rec, 8) * 4);
			GteRTPT();
			uint32_t code = h->colour | 0x2C000000;
			At<uint32_t>(pkt, 0) = 0x9000000;
			At<uint32_t>(pkt, 4) = code;
			if (h->flags & 1) At<uint32_t>(pkt, 4) = code | 0x2000000;
			if (h->flags & 4) At<uint32_t>(pkt, 4) &= 0xFDFFFFFF;
			const uint32_t d = (uint32_t)h->depth;
			At<uint32_t>(pkt, 0xC) = At<uint32_t>(rec, 0xC) + d;
			const uint32_t uv23 = (d << 16) + d + At<uint32_t>(rec, 0x14);
			At<uint32_t>(pkt, 0x1C) = uv23;
			At<uint32_t>(pkt, 0x14) = At<uint32_t>(rec, 0x10) + d;
			At<uint32_t>(pkt, 0x24) = uv23 >> 16;
			GteReadFLAG(&h->gte_flag);
			if (!(h->gte_flag & 0x60000))
			{
				GteNCLIP();
				UvOffsets(h, pkt, 0x16, 0xE);
				GteReadMAC0(&h->mac0);
				const int32_t mac0 = h->mac0;
				if (mac0 != 0 && (mac0 > 0 || (h->flags & 0x10)))
				{
					GteReadSXY012Split(pkt + 8, pkt + 0x10, pkt + 0x18);
					GteLoadV0((const void *)(vb + (uint32_t)At<uint16_t>(rec, 0xA) * 4));
					GteRTPS();
					uint32_t clip = OutX(pkt + 8) ? 1 : 0;
					if (OutX(pkt + 0x10)) clip |= 2;
					if (OutX(pkt + 0x18)) clip |= 4;
					if (OutY(pkt + 8)) clip |= 0x10;
					if (OutY(pkt + 0x10)) clip |= 0x20;
					if (OutY(pkt + 0x18)) clip |= 0x40;
					GteReadSXY2((void *)(pkt + 0x20));
					GteAVSZ4();
					if (OutX(pkt + 0x20)) clip |= 8;
					if (OutY(pkt + 0x20)) clip |= 0x80;
					if ((clip & 0xF) != 0xF && (clip & 0xF0) != 0xF0)
					{
						GteStoreOTZ(&h->otz);
						if (h->flags & 0x40)
						{
							GteLoadRGBC(pkt + 4);
							GteSetIR0(h->fade);
							GteDPCS();
							GteStoreRGB2(pkt + 4);
						}
						InsertPrimAutoDepth(Bucket(ot, h, shift), (void *)pkt);
						pkt += 0x28;
					}
				}
			}
			rec += 0x18;
		} while (--count);
		h->list = rec;
		return pkt;
	}

	static uint32_t ListG3(RenderHeader *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		const uint32_t list = h->list;
		int32_t count = At<int32_t>(list, 0);
		uint32_t rec = list + 4;
		h->list = rec;
		const uint32_t vb = h->verts;
		if (count <= 0)
		{
			h->list = rec;
			return cursor;
		}
		uint32_t pkt = cursor;
		do
		{
			GteLoadV012(vb + (uint32_t)At<uint16_t>(rec, 4) * 4, vb + (uint32_t)At<uint16_t>(rec, 6) * 4, vb + (uint32_t)At<uint16_t>(rec, 8) * 4);
			GteRTPT();
			const uint32_t fl = h->flags;
			const uint32_t code = At<uint32_t>(rec, 0);
			At<uint32_t>(pkt, 0) = 0x6000000;
			h->c[0] = code;
			if (fl & 2) h->c[0] = code | 0x2000000;
			if (fl & 8) h->c[0] &= 0xFDFFFFFF;
			h->c[1] = At<uint32_t>(rec, 0xC);
			h->c[2] = At<uint32_t>(rec, 0x10);
			GteReadFLAG(&h->gte_flag);
			if (!(h->gte_flag & 0x60000))
			{
				GteNCLIP();
				GteReadMAC0(&h->mac0);
				const int32_t mac0 = h->mac0;
				if (mac0 != 0 && (mac0 > 0 || (h->flags & 0x20)))
				{
					GteReadSXY012Split(pkt + 8, pkt + 0x10, pkt + 0x18);
					GteAVSZ3();
					uint32_t clip = OutX(pkt + 8) ? 1 : 0;
					if (OutX(pkt + 0x10)) clip |= 2;
					if (OutX(pkt + 0x18)) clip |= 4;
					if (OutY(pkt + 8)) clip |= 0x10;
					if (OutY(pkt + 0x10)) clip |= 0x20;
					if (OutY(pkt + 0x18)) clip |= 0x40;
					if ((clip & 7) != 7 && (clip & 0x70) != 0x70)
					{
						GteStoreOTZ(&h->otz);
						ScaleColours(h, 3);
						if (h->flags & 0x80)
						{
							GteLoadRGB012((uint32_t)&h->c[1], (uint32_t)&h->c[2], (uint32_t)&h->c[0]);
							GteSetIR0(h->fade);
							GteDPCT();
							GteStoreRGB012(pkt + 0xC, pkt + 0x14, pkt + 4);
						}
						else
						{
							At<uint32_t>(pkt, 4) = h->c[0];
							At<uint32_t>(pkt, 0xC) = h->c[1];
							At<uint32_t>(pkt, 0x14) = h->c[2];
						}
						InsertPrimAutoDepth(Bucket(ot, h, shift), (void *)pkt);
						pkt += 0x1C;
					}
				}
			}
			rec += 0x14;
		} while (--count);
		h->list = rec;
		return pkt;
	}

	static uint32_t ListG4(RenderHeader *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		const uint32_t list = h->list;
		int32_t count = At<int32_t>(list, 0);
		uint32_t rec = list + 4;
		h->list = rec;
		const uint32_t vb = h->verts;
		if (count <= 0)
		{
			h->list = rec;
			return cursor;
		}
		uint32_t pkt = cursor;
		do
		{
			GteLoadV012(vb + (uint32_t)At<uint16_t>(rec, 4) * 4, vb + (uint32_t)At<uint16_t>(rec, 6) * 4, vb + (uint32_t)At<uint16_t>(rec, 8) * 4);
			GteRTPT();
			const uint32_t fl = h->flags;
			const uint32_t code = At<uint32_t>(rec, 0);
			At<uint32_t>(pkt, 0) = 0x8000000;
			h->c[0] = code;
			if (fl & 2) h->c[0] = code | 0x2000000;
			if (fl & 8) h->c[0] &= 0xFDFFFFFF;
			h->c[1] = At<uint32_t>(rec, 0xC);
			h->c[2] = At<uint32_t>(rec, 0x10);
			h->c[3] = At<uint32_t>(rec, 0x14);
			GteReadFLAG(&h->gte_flag);
			if (!(h->gte_flag & 0x60000))
			{
				GteNCLIP();
				GteReadMAC0(&h->mac0);
				const int32_t mac0 = h->mac0;
				if (mac0 != 0 && (mac0 > 0 || (h->flags & 0x20)))
				{
					GteReadSXY012Split(pkt + 8, pkt + 0x10, pkt + 0x18);
					GteLoadV0((const void *)(vb + (uint32_t)At<uint16_t>(rec, 0xA) * 4));
					GteRTPS();
					uint32_t clip = OutX(pkt + 8) ? 1 : 0;
					if (OutX(pkt + 0x10)) clip |= 2;
					if (OutX(pkt + 0x18)) clip |= 4;
					if (OutY(pkt + 8)) clip |= 0x10;
					if (OutY(pkt + 0x10)) clip |= 0x20;
					if (OutY(pkt + 0x18)) clip |= 0x40;
					GteReadSXY2((void *)(pkt + 0x20));
					GteAVSZ4();
					if (OutX(pkt + 0x20)) clip |= 8;
					if (OutY(pkt + 0x20)) clip |= 0x80;
					if ((clip & 0xF) != 0xF && (clip & 0xF0) != 0xF0)
					{
						GteStoreOTZ(&h->otz);
						ScaleColours(h, 4);
						if (h->flags & 0x80)
						{
							GteLoadRGB012((uint32_t)&h->c[1], (uint32_t)&h->c[2], (uint32_t)&h->c[3]);
							GteSetIR0(h->fade);
							GteDPCT();
							GteStoreRGB012(pkt + 0xC, pkt + 0x14, pkt + 0x1C);
							GteLoadRGBC((uint32_t)&h->c[0]);
							GteDPCS();
							GteStoreRGB2(pkt + 4);
						}
						else
						{
							At<uint32_t>(pkt, 4) = h->c[0];
							At<uint32_t>(pkt, 0xC) = h->c[1];
							At<uint32_t>(pkt, 0x14) = h->c[2];
							At<uint32_t>(pkt, 0x1C) = h->c[3];
						}
						InsertPrimAutoDepth(Bucket(ot, h, shift), (void *)pkt);
						pkt += 0x24;
					}
				}
			}
			rec += 0x18;
		} while (--count);
		h->list = rec;
		return pkt;
	}

	static uint32_t ListGT3(RenderHeader *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		const uint32_t list = h->list;
		int32_t count = At<int32_t>(list, 0);
		uint32_t rec = list + 4;
		h->list = rec;
		const uint32_t vb = h->verts;
		if (count <= 0)
		{
			h->list = rec;
			return cursor;
		}
		uint32_t pkt = cursor;
		do
		{
			GteLoadV012(vb + (uint32_t)At<uint16_t>(rec, 4) * 4, vb + (uint32_t)At<uint16_t>(rec, 6) * 4, vb + (uint32_t)At<uint16_t>(rec, 8) * 4);
			GteRTPT();
			const uint32_t code = At<uint32_t>(rec, 0);
			At<uint32_t>(pkt, 0) = 0x9000000;
			h->c[0] = code;
			if (h->flags & 2) h->c[0] = code | 0x2000000;
			if (h->flags & 8) h->c[0] &= 0xFDFFFFFF;
			const uint32_t d = (uint32_t)h->depth;
			At<uint32_t>(pkt, 0xC) = At<uint32_t>(rec, 0xC) + d;
			At<uint32_t>(pkt, 0x18) = At<uint32_t>(rec, 0x10) + d;
			At<uint32_t>(pkt, 0x24) = (At<uint32_t>(rec, 8) >> 16) + d;
			h->c[1] = At<uint32_t>(rec, 0x14);
			h->c[2] = At<uint32_t>(rec, 0x18);
			GteReadFLAG(&h->gte_flag);
			if (!(h->gte_flag & 0x60000))
			{
				GteNCLIP();
				UvOffsets(h, pkt, 0x1A, 0xE);
				GteReadMAC0(&h->mac0);
				const int32_t mac0 = h->mac0;
				if (mac0 != 0 && (mac0 > 0 || (h->flags & 0x20)))
				{
					GteReadSXY012Split(pkt + 8, pkt + 0x14, pkt + 0x20);
					GteAVSZ3();
					uint32_t clip = OutX(pkt + 8) ? 1 : 0;
					if (OutX(pkt + 0x14)) clip |= 2;
					if (OutX(pkt + 0x20)) clip |= 4;
					if (OutY(pkt + 8)) clip |= 0x10;
					if (OutY(pkt + 0x14)) clip |= 0x20;
					if (OutY(pkt + 0x20)) clip |= 0x40;
					if ((clip & 7) != 7 && (clip & 0x70) != 0x70)
					{
						GteStoreOTZ(&h->otz);
						ScaleColours(h, 3);
						if (h->flags & 0x80)
						{
							GteLoadRGB012((uint32_t)&h->c[1], (uint32_t)&h->c[2], (uint32_t)&h->c[0]);
							GteSetIR0(h->fade);
							GteDPCT();
							GteStoreRGB012(pkt + 0x10, pkt + 0x1C, pkt + 4);
						}
						else
						{
							At<uint32_t>(pkt, 4) = h->c[0];
							At<uint32_t>(pkt, 0x10) = h->c[1];
							At<uint32_t>(pkt, 0x1C) = h->c[2];
						}
						InsertPrimAutoDepth(Bucket(ot, h, shift), (void *)pkt);
						pkt += 0x28;
					}
				}
			}
			rec += 0x1C;
		} while (--count);
		h->list = rec;
		return pkt;
	}

	static uint32_t ListGT4(RenderHeader *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		const uint32_t list = h->list;
		int32_t count = At<int32_t>(list, 0);
		uint32_t rec = list + 4;
		h->list = rec;
		const uint32_t vb = h->verts;
		if (count <= 0)
		{
			h->list = rec;
			return cursor;
		}
		uint32_t pkt = cursor;
		do
		{
			GteLoadV012(vb + (uint32_t)At<uint16_t>(rec, 4) * 4, vb + (uint32_t)At<uint16_t>(rec, 6) * 4, vb + (uint32_t)At<uint16_t>(rec, 8) * 4);
			GteRTPT();
			const uint32_t code = At<uint32_t>(rec, 0);
			At<uint32_t>(pkt, 0) = 0xC000000;
			h->c[0] = code;
			if (h->flags & 2) h->c[0] = code | 0x2000000;
			if (h->flags & 8) h->c[0] &= 0xFDFFFFFF;
			const uint32_t d = (uint32_t)h->depth;
			At<uint32_t>(pkt, 0xC) = At<uint32_t>(rec, 0xC) + d;
			const uint32_t uv23 = (d << 16) + d + At<uint32_t>(rec, 0x14);
			At<uint32_t>(pkt, 0x18) = At<uint32_t>(rec, 0x10) + d;
			At<uint32_t>(pkt, 0x24) = uv23;
			At<uint32_t>(pkt, 0x30) = uv23 >> 16;
			h->c[1] = At<uint32_t>(rec, 0x18);
			h->c[2] = At<uint32_t>(rec, 0x1C);
			h->c[3] = At<uint32_t>(rec, 0x20);
			GteReadFLAG(&h->gte_flag);
			if (!(h->gte_flag & 0x60000))
			{
				GteNCLIP();
				UvOffsets(h, pkt, 0x1A, 0xE);
				GteReadMAC0(&h->mac0);
				const int32_t mac0 = h->mac0;
				if (mac0 != 0 && (mac0 > 0 || (h->flags & 0x20)))
				{
					GteReadSXY012Split(pkt + 8, pkt + 0x14, pkt + 0x20);
					GteLoadV0((const void *)(vb + (uint32_t)At<uint16_t>(rec, 0xA) * 4));
					GteRTPS();
					uint32_t clip = OutX(pkt + 8) ? 1 : 0;
					if (OutX(pkt + 0x14)) clip |= 2;
					if (OutX(pkt + 0x20)) clip |= 4;
					if (OutY(pkt + 8)) clip |= 0x10;
					if (OutY(pkt + 0x14)) clip |= 0x20;
					if (OutY(pkt + 0x20)) clip |= 0x40;
					GteReadSXY2((void *)(pkt + 0x2C));
					GteAVSZ4();
					if (OutX(pkt + 0x2C)) clip |= 8;
					if (OutY(pkt + 0x2C)) clip |= 0x80;
					if ((clip & 0xF) != 0xF && (clip & 0xF0) != 0xF0)
					{
						GteStoreOTZ(&h->otz);
						ScaleColours(h, 4);
						if (h->flags & 0x80)
						{
							GteLoadRGB012((uint32_t)&h->c[1], (uint32_t)&h->c[2], (uint32_t)&h->c[3]);
							GteSetIR0(h->fade);
							GteDPCT();
							GteStoreRGB012(pkt + 0x10, pkt + 0x1C, pkt + 0x28);
							GteLoadRGBC((uint32_t)&h->c[0]);
							GteDPCS();
							GteStoreRGB2(pkt + 4);
						}
						else
						{
							At<uint32_t>(pkt, 4) = h->c[0];
							At<uint32_t>(pkt, 0x10) = h->c[1];
							At<uint32_t>(pkt, 0x1C) = h->c[2];
							At<uint32_t>(pkt, 0x28) = h->c[3];
						}
						InsertPrimAutoDepth(Bucket(ot, h, shift), (void *)pkt);
						pkt += 0x34;
					}
				}
			}
			rec += 0x24;
		} while (--count);
		h->list = rec;
		return pkt;
	}

	static uint32_t RenderModel(RenderHeader *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		const uint32_t fl = h->flags;
		if (!(fl & 0x2000)) h->verts = h->model + 8;
		h->list = At<uint32_t>(h->model, 0) + h->model;
		if (!(fl & 0x1000)) h->depth = 0;
		GteSetFarColour(h->far_rgb[0], h->far_rgb[1], h->far_rgb[2]);
		typedef uint32_t (*List)(RenderHeader *, uint32_t, int32_t, uint32_t);
		static const List lists[8] = { ListF3, ListF4, ListFT3, ListFT4, ListG3, ListG4, ListGT3, ListGT4 };
		for (int i = 0; i < 8; i++)
		{
			if (At<uint32_t>(h->list, 0) != 0) cursor = lists[i](h, ot, shift, cursor);
			else h->list += 4;
		}
		return cursor;
	}
}
}

namespace ff8fx
{
namespace gu066
{
	static inline int16_t &Counter(uint32_t n) { return At<int16_t>(n, 0xC); }

	static uint32_t AddMasterTask(uint32_t fn)
	{
		const uint32_t n = (uint32_t)AddTaskToQueue(QMaster(), fn);
		At<int16_t>(n, 0xC) = 0;
		At<int16_t>(n, 0xE) = 0;
		return n;
	}

	static void AddFadeTask(int16_t in, int16_t hold, int16_t out, int16_t level)
	{
		const uint32_t n = (uint32_t)AddTaskToQueue(QMaster(), game_addr(ORIG_FadeTask));
		At<int16_t>(n, 0x10) = in;
		At<int16_t>(n, 0x12) = hold;
		At<int16_t>(n, 0xC) = 0;
		At<int16_t>(n, 0x14) = out;
		At<int16_t>(n, 0x1C) = level;
	}

	static void AddFeedbackTask(int32_t kind, int16_t ticks)
	{
		const uint32_t n = (uint32_t)AddTaskToQueue(QMaster(), kind ? game_addr(ORIG_FeedbackB) : game_addr(ORIG_FeedbackA));
		At<int16_t>(n, 0xC) = 0;
		At<int16_t>(n, 0xE) = ticks;
	}

	static uint32_t __cdecl FadeTask(TaskNode *node)
	{
		const uint32_t n = (uint32_t)node;
		const int16_t c = Counter(n);
		const int32_t in = At<int16_t>(n, 0x10);
		const int32_t level = At<int16_t>(n, 0x1C);
		const int32_t t = c;
		int32_t v;
		if (t < in)
			v = (level / in) * t;
		else
		{
			const int32_t hold = At<int16_t>(n, 0x12);
			if (t - in < hold)
				v = level;
			else
			{
				const int32_t out = At<int16_t>(n, 0x14);
				const int32_t u = t - in - hold;
				if (u < out)
					v = level - (level / out) * u;
				else
					v = -1;
			}
		}
		if (DrawOnly()) return 0;
		Counter(n) = (int16_t)(c + 1);
		if (v < 0) return TASK_END;
		ScreenFadeColour(v, v, v);
		return 0;
	}

	static uint32_t __cdecl FeedbackTask(TaskNode *node)
	{
		ScreenFeedback(0, 0xFF, 0xFF, 0xFF, 0x3F);
		if (DrawOnly()) return 0;
		const uint32_t n = (uint32_t)node;
		Counter(n) = (int16_t)(Counter(n) + 1);
		return Counter(n) >= At<int16_t>(n, 0xE) ? TASK_END : 0;
	}

	static void SparkDraw(uint32_t w, RenderHeader *h, uint32_t e)
	{
		At<int32_t>(w, 0x54) = At<int16_t>(e, 0x12);
		At<int32_t>(w, 0x50) = At<int16_t>(e, 0x10);
		At<int32_t>(w, 0x58) = At<int16_t>(e, 0x14);
		const int32_t angle = RotationBetweenVectors((const int32_t *)(w + 0x70), (const int32_t *)(w + 0x50), (int32_t *)(w + 0x60));
		BuildAxisAngleRotationMatrix(angle, (Mat4x3 *)(w + 0x30), (const int32_t *)(w + 0x60));
		At<int32_t>(w, 0x44) = At<int16_t>(e, 8);
		At<int32_t>(w, 0x48) = At<int16_t>(e, 0xA);
		At<int32_t>(w, 0x4C) = At<int16_t>(e, 0xC);
		const int32_t sx = At<int16_t>(e, 0xE);
		At<int32_t>(w, 0x58) = sx;
		At<int32_t>(w, 0x50) = sx;
		At<int32_t>(w, 0x54) = At<int16_t>(e, 0x16);
		Scale3DMatrix((Mat4x3 *)(w + 0x30), (const int32_t *)(w + 0x50));
		ComposeAffineTransform((const Mat4x3 *)(w + 0x10), (const Mat4x3 *)(w + 0x30), (Mat4x3 *)(w + 0x30));
		GteSetRotMatrix((const Mat4x3 *)(w + 0x30));
		GteSetTransVector((const Mat4x3 *)(w + 0x30));
		const int16_t c = At<int16_t>(e, 4);
		h->flags = 0x33;
		if (c < 6)
		{
			h->fade = 0x1000 - c * 682;
			h->flags = 0xF3;
		}
		else if (c >= 0x12)
		{
			h->fade = (c - 0x12) * 682;
			h->flags = 0xF3;
		}
		h->colour = At<uint32_t>(game_addr(SPARK_COLOURS), (c % 0x12) * 4);
		PacketCursor() = RenderModel(h, OT44(), 2, PacketCursor());
	}

	static uint32_t __cdecl SparkTask(TaskNode *node)
	{
		const uint32_t n = (uint32_t)node;
		const uint32_t w = (uint32_t)FieldAlloc(0x80);
		if (!DrawOnly() && Counter(n) <= 0x46 && Counter(n) % 3 == 0)
		{
			uint32_t e = game_addr(POOL1);
			int32_t i = 0;
			bool found = true;
			while (At<int32_t>(e, 0) != 0)
			{
				e += 0x18;
				i++;
				if (e >= game_addr(POOL1_END)) { found = false; break; }
			}
			if (found && i < 100)
			{
				At<int32_t>(e, 0) = 1;
				At<int16_t>(e, 4) = 0;
				const int32_t bone = CrtRand() % 0x1D;
				const int32_t flag = CrtRand() % 0x1000;
				GetEffectSpawnPosition(Caster(), At<int32_t>(game_addr(SPAWN_BONES), bone * 4), flag, (int16_t *)(w + 8));
				const int32_t rx = CrtRand() % 0x258;
				At<int32_t>(w, 0x50) = rx - Centre()[0] + At<int16_t>(w, 8) - 0x12C;
				const int32_t ry = CrtRand() % 0x258;
				At<int32_t>(w, 0x54) = ry + At<int16_t>(w, 0xA) - Centre()[1] - 0x12C;
				const int32_t rz = CrtRand() % 0x258;
				At<int32_t>(w, 0x58) = rz + At<int16_t>(w, 0xC) - Centre()[2] - 0x12C;
				NormalizeVector((const int32_t *)(w + 0x50), (int32_t *)(w + 0x50));
				At<uint32_t>(e, 8) = At<uint32_t>(w, 8);
				At<uint32_t>(e, 0xC) = At<uint32_t>(w, 0xC);
				At<int16_t>(e, 0x10) = (int16_t)At<int32_t>(w, 0x50);
				At<int16_t>(e, 0x12) = (int16_t)At<int32_t>(w, 0x54);
				At<int16_t>(e, 0x14) = (int16_t)At<int32_t>(w, 0x58);
				At<int16_t>(e, 0xE) = (int16_t)(CrtRand() % 0x1200 + 0x2800);
				At<int16_t>(e, 0x16) = (int16_t)(CrtRand() % 0x2000 + 0x1800);
			}
		}
		RenderHeader *h = (RenderHeader *)FieldAlloc(0x6C);
		NotePad((uint32_t)h);
		ff8fx::copy<(0x20)>((void *)(w + 0x10), (const void *)&Camera());
		h->model = game_addr(game::unk_DE90CC);
		At<uint32_t>((uint32_t)h, 8) = 0;
		int32_t count = 0;
		At<int32_t>(w, 0x70) = 0;
		At<int32_t>(w, 0x74) = -0x1000;
		At<int32_t>(w, 0x78) = 0;
		for (uint32_t e = game_addr(POOL1); e < game_addr(POOL1_END); e += 0x18)
		{
			if (At<int32_t>(e, 0) != 1) continue;
			SparkDraw(w, h, e);
			if (DrawOnly()) continue;
			const int16_t c = At<int16_t>(e, 4);
			if (c >= 0x18)
				At<int32_t>(e, 0) = 0;
			else
			{
				At<int16_t>(e, 4) = (int16_t)(c + 1);
				count++;
			}
		}
		FieldFree(0x6C);
		FieldFree(0x80);
		if (DrawOnly()) return 0;
		Counter(n) = (int16_t)(Counter(n) + 1);
		return count ? 0 : TASK_END;
	}

	static uint32_t __cdecl SpawnerTask(TaskNode *node)
	{
		if (DrawOnly()) return 0;
		const uint32_t n = (uint32_t)node;
		Counter(n) = (int16_t)(Counter(n) + 1);
		const int16_t c = Counter(n);
		if (c > 0x68) return TASK_END;
		if (c % 3 != 1) return 0;
		int16_t pos[4];
		pos[3] = (int16_t)SpawnPadWord();
		const int32_t bone = CrtRand() % 0x1D;
		const int32_t flag = CrtRand() % 0x1000;
		GetEffectSpawnPosition(Caster(), At<int32_t>(game_addr(SPAWN_BONES), bone * 4), flag, pos);
		const uint32_t lo = *(const uint32_t *)&pos[0], hi = *(const uint32_t *)&pos[2];
		uint32_t t = (uint32_t)AddTaskToQueue(QMaster(), game_addr(ORIG_RingTask));
		At<int16_t>(t, 0xC) = 0;
		At<uint32_t>(t, 0x10) = lo;
		At<uint32_t>(t, 0x14) = hi;
		At<int16_t>(t, 0x18) = (int16_t)(CrtRand() % 0x200 + 0x80);
		const int32_t size = (CrtRand() % 0x1600 + 0xC00) / 3;
		At<int16_t>(t, 0x1E) = (int16_t)size;
		At<int16_t>(t, 0x1C) = (int16_t)size;
		t = (uint32_t)AddTaskToQueue(QMaster(), game_addr(ORIG_PuffTask));
		At<int16_t>(t, 0xC) = 0;
		At<int16_t>(t, 0xE) = (int16_t)(Counter(n) + 0x64);
		At<uint32_t>(t, 0x10) = lo;
		At<uint32_t>(t, 0x14) = hi;
		t = (uint32_t)AddTaskToQueue(QMaster(), game_addr(ORIG_DustTask));
		At<int16_t>(t, 0xC) = 0;
		At<int16_t>(t, 0xE) = (int16_t)(Counter(n) + 0xC8);
		At<uint32_t>(t, 0x10) = lo;
		At<uint32_t>(t, 0x14) = hi;
		t = (uint32_t)AddTaskToQueue(QMaster(), game_addr(ORIG_SmokeTask));
		At<int16_t>(t, 0xC) = 0;
		At<int16_t>(n, 0xE) = (int16_t)(At<int16_t>(n, 0xE) + 1);
		At<int16_t>(t, 0xE) = (int16_t)(Counter(n) + 0x12C);
		At<uint32_t>(t, 0x10) = lo;
		At<uint32_t>(t, 0x14) = hi;
		if (At<int16_t>(n, 0xE) % 6 == 3 && CrtRand() % 100 < 0x3C)
			ScreenFadeTask(0, 2, 6, 0xFF);
		return 0;
	}

	static void RingDraw(uint32_t n)
	{
		int16_t angles[3] = { 0, At<int16_t>(n, 0x18), 0 };
		Mat4x3 m;
		ComposeZYXRotationMatrix(angles, &m);
		m.t[0] = At<int16_t>(n, 0x10);
		const int32_t s = At<int16_t>(n, 0x1C);
		const int32_t scale[3] = { s, s, s };
		m.t[1] = At<int16_t>(n, 0x12);
		m.t[2] = At<int16_t>(n, 0x14);
		Scale3DMatrix(&m, scale);
		TransformCameraByShadowRotation((const void *)(n + 0x10), At<int16_t>(n, 0x1C), -(int32_t)Push());
		GteSetRotMatrix(&m);
		RenderHeader *h = (RenderHeader *)FieldAlloc(0x58);
		const int16_t c = Counter(n);
		h->model = game_addr(game::unk_DE4A54);
		At<uint32_t>((uint32_t)h, 8) = 0;
		h->flags = 0x33;
		if (c >= 2)
		{
			h->flags = 0xF3;
			h->fade = (c - 2) << 9;
		}
		PacketCursor() = RenderPrimModel(h, OT44(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static uint32_t __cdecl RingTask(TaskNode *node)
	{
		const uint32_t n = (uint32_t)node;
		RingDraw(n);
		if (DrawOnly()) return 0;
		const int16_t d = At<int16_t>(n, 0x1E);
		At<int16_t>(n, 0x1C) = (int16_t)(At<int16_t>(n, 0x1C) + d);
		At<int16_t>(n, 0x1E) = (int16_t)(d - d / 4);
		Counter(n) = (int16_t)(Counter(n) + 1);
		return Counter(n) >= 0xA ? TASK_END : 0;
	}

	static void SpriteBegin(uint32_t w, uint8_t *h, uint32_t model)
	{
		At<int16_t>((uint32_t)h, 0x24) = 8;
		ff8fx::copy<(0x20)>((void *)(w + 8), (const void *)&Camera());
		At<uint32_t>((uint32_t)h, 0) = model;
		GteSetLightMatrix((const void *)(w + 8));
		GteSetBackFromTrans((const void *)(w + 8));
	}

	static void SpritePushed(uint32_t w, uint8_t *h, uint32_t e, int32_t push, int16_t frame)
	{
		GteLoadV0((const void *)(e + 8));
		GteMVMVA_LightV0Bk();
		GteSetRotScale(At<int16_t>(e, 6));
		At<int16_t>((uint32_t)h, 4) = frame;
		GteReadMAC123((int32_t *)(w + 0x38));
		NormalizeVector((const int32_t *)(w + 0x38), (int32_t *)(w + 0x28));
		At<int32_t>(w, 0x38) = At<int32_t>(w, 0x38) + (mul32(At<int32_t>(w, 0x28), push) >> 12);
		At<int32_t>(w, 0x3C) = At<int32_t>(w, 0x3C) + (mul32(At<int32_t>(w, 0x2C), push) >> 12);
		At<int32_t>(w, 0x40) = At<int32_t>(w, 0x40) + (mul32(At<int32_t>(w, 0x30), push) >> 12);
		GteSetTransFromVec32((const int32_t *)(w + 0x38));
		FrameArena() = InitEffectSequenceFromData(h, OT44(), 2, FrameArena());
	}

	static uint32_t PoolFree(uint32_t pool, uint32_t end, int32_t cap)
	{
		uint32_t e = pool;
		int32_t i = 0;
		while (At<int32_t>(e, 0) != 0)
		{
			e += 0x18;
			i++;
			if (e >= end) return 0;
		}
		return i < cap ? e : 0;
	}

	static uint32_t __cdecl PuffTask(TaskNode *node)
	{
		const uint32_t n = (uint32_t)node;
		const uint32_t w = (uint32_t)FieldAlloc(0x48);
		if (!DrawOnly() && Counter(n) <= 4)
		{
			At<uint32_t>(w, 0) = At<uint32_t>(n, 0x10);
			At<uint32_t>(w, 4) = At<uint32_t>(n, 0x14);
			for (int k = 0; k < 1; k++)
			{
				const uint32_t e = PoolFree(game_addr(POOL1), game_addr(POOL1_END), 100);
				if (!e) break;
				At<int32_t>(e, 0) = At<int16_t>(n, 0xE);
				At<int16_t>(e, 4) = 0;
				if (Counter(n) == 0)
					At<int16_t>(e, 6) = 0x1200;
				else
					At<int16_t>(e, 6) = (int16_t)(CrtRand() % 0xC00 + 0x600);
				At<int16_t>(e, 8) = (int16_t)(CrtRand() % 0x320 + At<int16_t>(w, 0) - 0x190);
				At<int16_t>(e, 0xA) = (int16_t)(CrtRand() % 0x320 + At<int16_t>(w, 2) - 0x190);
				At<int16_t>(e, 0xC) = (int16_t)(CrtRand() % 0x320 + At<int16_t>(w, 4) - 0x190);
			}
		}
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		SpriteBegin(w, h, game_addr(game::unk_DE43E8));
		int32_t count = 0;
		const int32_t push = -(int32_t)Push();
		const int32_t tag = At<int16_t>(n, 0xE);
		for (uint32_t e = game_addr(POOL1); e < game_addr(POOL1_END); e += 0x18)
		{
			if (At<int32_t>(e, 0) != tag) continue;
			SpritePushed(w, h, e, push, At<int16_t>(e, 4));
			if (DrawOnly()) continue;
			if (At<int16_t>((uint32_t)h, 0x28) < 0)
				At<int32_t>(e, 0) = 0;
			else
			{
				At<int16_t>(e, 4) = (int16_t)(At<int16_t>(e, 4) + 1);
				count++;
			}
		}
		FieldFree(0xB4);
		FieldFree(0x48);
		if (DrawOnly()) return 0;
		Counter(n) = (int16_t)(Counter(n) + 1);
		return count ? 0 : TASK_END;
	}

	static void DustMove(uint32_t e)
	{
		At<int16_t>(e, 8) = (int16_t)(At<int16_t>(e, 8) + At<int16_t>(e, 0x10));
		At<int16_t>(e, 0xA) = (int16_t)(At<int16_t>(e, 0xA) + At<int16_t>(e, 0x12));
		At<int16_t>(e, 0xC) = (int16_t)(At<int16_t>(e, 0xC) + At<int16_t>(e, 0x14));
		for (int k = 0x10; k <= 0x14; k += 2)
		{
			const int16_t v = At<int16_t>(e, k);
			At<int16_t>(e, k) = (int16_t)(v - (int16_t)(v >> 3));
		}
	}

	static uint32_t __cdecl DustTask(TaskNode *node)
	{
		const uint32_t n = (uint32_t)node;
		const uint32_t w = (uint32_t)FieldAlloc(0x48);
		if (!DrawOnly() && Counter(n) <= 1)
		{
			At<uint32_t>(w, 0) = At<uint32_t>(n, 0x10);
			At<uint32_t>(w, 4) = At<uint32_t>(n, 0x14);
			for (int k = 0; k < 5; k++)
			{
				const uint32_t e = PoolFree(game_addr(POOL2), game_addr(POOL2_END), 200);
				if (!e) break;
				At<int32_t>(e, 0) = At<int16_t>(n, 0xE);
				At<int16_t>(e, 4) = 0;
				At<int16_t>(e, 6) = (int16_t)(CrtRand() % 0x400 + 0x300);
				At<int16_t>(e, 8) = (int16_t)(CrtRand() % 0x3E8 + At<int16_t>(w, 0) - 0x1F4);
				At<int16_t>(e, 0xA) = (int16_t)(CrtRand() % 0x3E8 + At<int16_t>(w, 2) - 0x1F4);
				const int16_t z = (int16_t)(CrtRand() % 0x3E8 + At<int16_t>(w, 4) - 0x1F4);
				At<int16_t>(e, 0xC) = z;
				At<int32_t>(w, 0x28) = At<int16_t>(e, 8) - Centre()[0];
				At<int32_t>(w, 0x2C) = At<int16_t>(e, 0xA) - Centre()[1];
				At<int32_t>(w, 0x30) = z - Centre()[2];
				NormalizeVector((const int32_t *)(w + 0x28), (int32_t *)(w + 0x28));
				const int32_t speed = CrtRand() % 0x12C + 0x96;
				At<int16_t>(e, 0x10) = (int16_t)(mul32(speed, At<int32_t>(w, 0x28)) >> 12);
				At<int16_t>(e, 0x12) = (int16_t)(mul32(At<int32_t>(w, 0x2C), speed) >> 12);
				At<int16_t>(e, 0x14) = (int16_t)(mul32(speed, At<int32_t>(w, 0x30)) >> 12);
			}
		}
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		SpriteBegin(w, h, game_addr(game::unk_DE4910));
		int32_t count = 0;
		const int32_t push = -(int32_t)Push();
		const int32_t tag = At<int16_t>(n, 0xE);
		for (uint32_t e = game_addr(POOL2); e < game_addr(POOL2_END); e += 0x18)
		{
			if (At<int32_t>(e, 0) != tag) continue;
			SpritePushed(w, h, e, push, At<int16_t>(e, 4));
			if (DrawOnly()) continue;
			if (At<int16_t>((uint32_t)h, 0x28) < 0)
				At<int32_t>(e, 0) = 0;
			else
			{
				At<int16_t>(e, 4) = (int16_t)(At<int16_t>(e, 4) + 1);
				DustMove(e);
				count++;
			}
		}
		FieldFree(0xB4);
		FieldFree(0x48);
		if (DrawOnly()) return 0;
		Counter(n) = (int16_t)(Counter(n) + 1);
		return count ? 0 : TASK_END;
	}

	static uint32_t __cdecl SmokeTask(TaskNode *node)
	{
		const uint32_t n = (uint32_t)node;
		const uint32_t w = (uint32_t)FieldAlloc(0x48);
		if (!DrawOnly() && Counter(n) <= 8)
		{
			At<uint32_t>(w, 0) = At<uint32_t>(n, 0x10);
			At<uint32_t>(w, 4) = At<uint32_t>(n, 0x14);
			for (int k = 0; k < 1; k++)
			{
				const uint32_t e = PoolFree(game_addr(POOL2), game_addr(POOL2_END), 200);
				if (!e) break;
				At<int32_t>(e, 0) = At<int16_t>(n, 0xE);
				At<int16_t>(e, 4) = 0;
				At<int16_t>(e, 6) = (int16_t)(CrtRand() % 0x880 + 0x600);
				At<uint32_t>(e, 8) = At<uint32_t>(w, 0);
				At<uint32_t>(e, 0xC) = At<uint32_t>(w, 4);
				At<int32_t>(w, 0x28) = At<int16_t>(e, 8) - Centre()[0];
				At<int32_t>(w, 0x2C) = At<int16_t>(e, 0xA) - Centre()[1];
				At<int32_t>(w, 0x30) = At<int16_t>(e, 0xC) - Centre()[2];
				NormalizeVector((const int32_t *)(w + 0x28), (int32_t *)(w + 0x28));
				const int32_t speed = CrtRand() % 0xFA + 0x82;
				At<int16_t>(e, 0x10) = (int16_t)(mul32(At<int32_t>(w, 0x28), speed) >> 12);
				At<int16_t>(e, 0x12) = (int16_t)(mul32(At<int32_t>(w, 0x2C), speed) >> 12);
				At<int16_t>(e, 0x14) = (int16_t)(mul32(At<int32_t>(w, 0x30), speed) >> 12);
			}
		}
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		SpriteBegin(w, h, game_addr(game::unk_DE423C));
		int32_t count = 0;
		const int32_t push = -(int32_t)Push();
		const int32_t tag = At<int16_t>(n, 0xE);
		for (uint32_t e = game_addr(POOL2); e < game_addr(POOL2_END); e += 0x18)
		{
			if (At<int32_t>(e, 0) != tag) continue;
			SpritePushed(w, h, e, push, At<int16_t>(e, 4));
			if (DrawOnly()) continue;
			if (At<int16_t>((uint32_t)h, 0x28) < 0)
				At<int32_t>(e, 0) = 0;
			else
			{
				At<int16_t>(e, 4) = (int16_t)(At<int16_t>(e, 4) + 1);
				const int16_t s = At<int16_t>(e, 6);
				At<int16_t>(e, 6) = (int16_t)(s + (int16_t)(s >> 5));
				DustMove(e);
				count++;
			}
		}
		FieldFree(0xB4);
		FieldFree(0x48);
		if (DrawOnly()) return 0;
		Counter(n) = (int16_t)(Counter(n) + 1);
		return count ? 0 : TASK_END;
	}

	static inline int32_t Div6000Q(int32_t x)
	{
		int32_t d = (int32_t)(((int64_t)x * 0x57619F1) >> 32) >> 7;
		return d + (int32_t)((uint32_t)d >> 31);
	}

	static void MeltStartTicks(uint32_t h)
	{
		uint32_t rec = At<uint32_t>(h, 0);
		uint32_t st = At<uint32_t>(h, 0x2C);
		const uint32_t vb = At<uint32_t>(h, 4);
		int32_t n = At<uint16_t>(h, 8);
		if (n > 0)
		{
			do
			{
				const uint32_t idx = At<uint16_t>(rec, 2) & 0xFFF;
				rec += 0x10;
				st += 2;
				At<uint32_t>(h, 0x60) = At<uint32_t>(vb, idx * 8);
				At<uint32_t>(h, 0x64) = At<uint32_t>(vb, idx * 8 + 4);
				const int32_t y = At<int16_t>(h, 0x62);
				At<uint16_t>(st, -2) |= (uint16_t)(Div6000Q(y * 320) + 0x140);
			} while (--n);
		}
		n = At<uint16_t>(h, 0xA);
		if (n > 0)
		{
			rec += 2;
			do
			{
				const uint32_t idx = At<uint16_t>(rec, 0) & 0xFFF;
				rec += 0x14;
				st += 2;
				At<uint32_t>(h, 0x60) = At<uint32_t>(vb, idx * 8);
				At<uint32_t>(h, 0x64) = At<uint32_t>(vb, idx * 8 + 4);
				const int32_t y = At<int16_t>(h, 0x62);
				At<uint16_t>(st, -2) |= (uint16_t)(Div6000Q(y * 320) + 0x140);
			} while (--n);
		}
		At<uint32_t>(h, 0x2C) = st;
	}

	static int32_t ShardFree()
	{
		int32_t idx = ShardCursor();
		const uint32_t tab = ShardTable();
		for (int32_t tries = 0; tries < 1000; tries++, idx++)
		{
			if (idx >= 1000) idx = 0;
			if (At<int16_t>(tab, idx * 0x44) == 0) return idx;
		}
		return -1;
	}

	static void ShardDirection(uint32_t h, int32_t vtx, uint32_t out)
	{
		At<int16_t>(h, 0x80) = (int16_t)(At<int16_t>(h, 0x78) - At<int16_t>(h, vtx));
		At<int16_t>(h, 0x82) = (int16_t)(At<int16_t>(h, 0x7A) - At<int16_t>(h, vtx + 2));
		At<int16_t>(h, 0x84) = (int16_t)(At<int16_t>(h, 0x7C) - At<int16_t>(h, vtx + 4));
		NormalizeSVector((const int16_t *)(h + 0x80), (int16_t *)out);
	}

	static uint32_t ShardSpawn(uint32_t h, uint32_t st, int32_t idx, int nv)
	{
		At<uint16_t>(st, 0) = (uint16_t)idx;
		ShardCursor() = idx & 0xFFFF;
		const uint32_t e = ShardTable() + idx * 0x44;
		At<int16_t>(e, 0) = 1;
		At<int16_t>(e, 2) = (int16_t)(CrtRand() % 0x18 + 0x10);
		for (int k = 0; k < nv * 2; k++)
			At<uint32_t>(e, 4 + k * 4) = At<uint32_t>(h, 0x58 + k * 4);
		At<uint32_t>(h, 0x78) = At<uint32_t>(h, 0x60);
		At<uint32_t>(h, 0x7C) = At<uint32_t>(h, 0x64);
		At<uint16_t>(h, 0x7A) = (uint16_t)(At<uint16_t>(h, 0x7A) + 0xF060);
		for (int k = 0; k < nv; k++)
			ShardDirection(h, 0x58 + k * 8, e + 0x24 + k * 8);
		return e;
	}

	static void ShardFly(uint32_t h, uint32_t st, uint32_t e, int nv)
	{
		for (int k = 0; k < nv * 2; k++)
			At<uint32_t>(h, 0x58 + k * 4) = At<uint32_t>(e, 4 + k * 4);
		const int16_t age = At<int16_t>(e, 0);
		if (age < 0x30)
		{
			At<uint32_t>(h, 0x38) = 0;
			const int32_t fade = (age << 12) / 48;
			GteLoadRGBC(h + 0x38);
			GteSetIR0(fade);
			GteDPCS();
			GteStoreRGB2(h + 0x38);
		}
		else
			At<uint32_t>(h, 0x38) = 0xFFFFFF;
		if (DrawOnly()) return;
		At<int16_t>(e, 0) = (int16_t)(At<int16_t>(e, 0) + 1);
		if (At<int16_t>(e, 0) >= 0x40)
		{
			At<int16_t>(e, 0) = 0;
			At<uint16_t>(st, 0) = 0x8000;
		}
		const int32_t sp = At<int16_t>(e, 2);
		int32_t f[4];
		if (nv == 3)
		{
			f[0] = (sp * 3 << 10) >> 12;
			f[1] = sp;
			f[2] = (sp * 7 << 8) >> 12;
		}
		else
		{
			f[0] = (sp * 5 << 9) >> 12;
			f[1] = sp;
			f[2] = (sp * 3 << 10) >> 12;
			f[3] = (sp * 7 << 8) >> 12;
		}
		for (int k = 0; k < nv; k++)
			for (int j = 0; j < 3; j++)
				At<int16_t>(e, 4 + k * 8 + j * 2) = (int16_t)(At<int16_t>(e, 4 + k * 8 + j * 2) + ((At<int16_t>(e, 0x24 + k * 8 + j * 2) * f[k]) >> 12));
		At<int16_t>(e, 2) = (int16_t)(sp + (int16_t)((int16_t)sp >> 4));
	}

	static uint32_t FaceDraw(uint32_t h, uint32_t rec, int nv, bool overlay, uint32_t ot, int32_t shift, uint32_t pkt)
	{
		GteLoadV012(h + 0x58, h + 0x60, h + 0x68);
		GteRTPT();
		const uint32_t w0c = At<uint32_t>(rec, 0xC);
		At<uint32_t>(pkt, 0) = nv == 3 ? 0x7000000 : 0x9000000;
		At<uint32_t>(pkt, 0xC) = At<uint32_t>(rec, 8);
		At<uint32_t>(pkt, 4) = (w0c & 0x2000000) | At<uint32_t>(h, 0x1C) | (nv == 3 ? 0x24000000 : 0x2C000000);
		At<uint32_t>(pkt, 0x14) = w0c & 0x1FFFFFF;
		GteReadFLAG((void *)(h + 0x54));
		if (At<uint32_t>(h, 0x54) & 0x60000) return pkt;
		GteNCLIP();
		GteReadMAC0((void *)(h + 0x48));
		if (At<int32_t>(h, 0x48) <= 0) return pkt;
		GteReadSXY012Split(pkt + 8, pkt + 0x10, pkt + 0x18);
		if (nv == 3)
		{
			GteAVSZ3();
			At<int32_t>(pkt, 0x1C) = At<int32_t>(rec, 4) >> 16;
		}
		else
		{
			GteLoadV0((const void *)(h + 0x70));
			GteRTPS();
			const uint32_t uv23 = At<uint32_t>(rec, 0x10);
			At<uint32_t>(pkt, 0x1C) = uv23;
			At<int32_t>(pkt, 0x24) = (int32_t)uv23 >> 16;
			GteReadSXY2((void *)(pkt + 0x20));
			GteAVSZ4();
		}
		GteStoreOTZ((void *)(h + 0x50));
		int32_t z = At<int32_t>(h, 0x50) >> shift;
		InsertPrimAutoDepth(ot + z * 4, (void *)pkt);
		const uint32_t size = nv == 3 ? 0x20 : 0x28;
		if (!overlay) return pkt + size;
		const uint32_t q = pkt + size;
		At<uint32_t>(q, 4) = At<uint32_t>(h, 0x38) | (nv == 3 ? 0x22000000 : 0x2A000000);
		At<uint32_t>(q, 8) = At<uint32_t>(pkt, 8);
		At<uint32_t>(q, 0) = nv == 3 ? 0x4000000 : 0x5000000;
		At<uint32_t>(q, 0xC) = At<uint32_t>(pkt, 0x10);
		At<uint32_t>(q, 0x10) = At<uint32_t>(pkt, 0x18);
		if (nv == 4) At<uint32_t>(q, 0x14) = At<uint32_t>(pkt, 0x20);
		if (--z < 0) z = 0;
		InsertPrimAutoDepth(ot + z * 4, (void *)q);
		return q + (nv == 3 ? 0x14 : 0x18);
	}

	static uint32_t MeltFaces(uint32_t h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		int32_t spawn_ok = 1;
		uint32_t pkt = cursor;
		const uint32_t vb = At<uint32_t>(h, 4);
		uint32_t st = At<uint32_t>(h, 0x2C);
		uint32_t rec = At<uint32_t>(h, 0);
		uint32_t i = 0;
		if (At<uint16_t>(h, 8) != 0)
		{
			do
			{
				const uint32_t s = At<uint16_t>(st, 0);
				if (!(s & 0x8000))
				{
					int32_t shard;
					if (s & 0x4000)
					{
						for (int k = 0; k < 3; k++)
						{
							const uint32_t idx = At<uint16_t>(rec, k * 2) & 0xFFF;
							At<uint32_t>(h, 0x58 + k * 8) = At<uint32_t>(vb, idx * 8);
							At<uint32_t>(h, 0x5C + k * 8) = At<uint32_t>(vb, idx * 8 + 4);
						}
						shard = 0;
						if (At<int32_t>(h, 0x44) > (int32_t)(s & 0x3FFF) && spawn_ok)
						{
							const int32_t idx = ShardFree();
							if (idx < 0)
								spawn_ok = 0;
							else
							{
								ShardSpawn(h, st, idx, 3);
								const uint32_t p3 = Pool3();
								int32_t c3 = Pool3Cursor();
								for (int32_t tries = 0; tries < 800; tries++, c3++)
								{
									if (c3 >= 800) c3 = 0;
									const uint32_t q = p3 + c3 * 0x18;
									if (At<int32_t>(q, 0) != 0) continue;
									Pool3Cursor() = c3;
									At<int32_t>(q, 0) = 1;
									At<int16_t>(q, 4) = 0;
									At<int16_t>(q, 6) = (int16_t)(CrtRand() % 0x880 + 0x880);
									At<uint32_t>(q, 8) = At<uint32_t>(h, 0x58);
									At<uint32_t>(q, 0xC) = At<uint32_t>(h, 0x5C);
									At<int16_t>(q, 0x12) = (int16_t)(-0x19 - CrtRand() % 0x41);
									break;
								}
							}
						}
					}
					else
					{
						shard = 1;
						ShardFly(h, st, ShardTable() + s * 0x44, 3);
					}
					pkt = FaceDraw(h, rec, 3, shard != 0, ot, shift, pkt);
				}
				i++;
				rec += 0x10;
				st += 2;
			} while (i < At<uint16_t>(h, 8));
		}
		i = 0;
		if (At<uint16_t>(h, 0xA) != 0)
		{
			do
			{
				const uint32_t s = At<uint16_t>(st, 0);
				if (!(s & 0x8000))
				{
					int32_t shard;
					if (s & 0x4000)
					{
						for (int k = 0; k < 4; k++)
						{
							const uint32_t idx = At<uint16_t>(rec, k * 2) & 0xFFF;
							At<uint32_t>(h, 0x58 + k * 8) = At<uint32_t>(vb, idx * 8);
							At<uint32_t>(h, 0x5C + k * 8) = At<uint32_t>(vb, idx * 8 + 4);
						}
						shard = 0;
						if (At<int32_t>(h, 0x44) > (int32_t)(s & 0x3FFF) && spawn_ok)
						{
							const int32_t idx = ShardFree();
							if (idx < 0)
								spawn_ok = 0;
							else
								ShardSpawn(h, st, idx, 4);
						}
					}
					else
					{
						shard = 1;
						ShardFly(h, st, ShardTable() + s * 0x44, 4);
					}
					pkt = FaceDraw(h, rec, 4, shard != 0, ot, shift, pkt);
				}
				i++;
				rec += 0x14;
				st += 2;
			} while (i < At<uint16_t>(h, 0xA));
		}
		At<uint32_t>(h, 0x2C) = st;
		return pkt;
	}

	static uint32_t MeltModel(uint32_t model, uint32_t h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		const uint32_t bones = At<uint32_t>(model, 0) + 0x10;
		uint32_t offs = At<uint32_t>(model, 4);
		const int32_t count = At<int32_t>(offs, 0);
		offs += 4;
		if (count <= 0) return cursor;
		for (int32_t i = 0; i < count; i++)
		{
			const uint32_t obj0 = At<uint32_t>(model, 4) + At<uint32_t>(offs, 0);
			offs += 4;
			if (!(At<uint32_t>(h, 0x20) & (1u << (i & 31)))) continue;
			uint32_t obj = obj0;
			int32_t groups = At<int16_t>(obj, 0);
			uint32_t out = At<uint32_t>(h, 4);
			obj += 2;
			if (groups > 0)
			{
				do
				{
					const int32_t bone = At<int16_t>(obj, 0);
					obj += 2;
					const Mat4x3 *m = (const Mat4x3 *)(bones + bone * 0x30 + 0x10);
					GteSetRotMatrixCtrl(m);
					GteSetTransVectorCtrl(m);
					int32_t nv = At<int16_t>(obj, 0);
					obj += 2;
					if (nv > 0)
					{
						do
						{
							GteLoadV0Words(obj);
							obj += 6;
							GteMVMVA_RotV0Tr2();
							GteStoreIRVertex(out);
							out += 8;
						} while (--nv);
					}
				} while (--groups);
			}
			const uint32_t a = (obj + 3) & ~3u;
			At<uint16_t>(h, 8) = At<uint16_t>(a, 0);
			At<uint16_t>(h, 0xA) = At<uint16_t>(a, 2);
			At<uint16_t>(h, 0xC) = At<uint16_t>(a, 4);
			At<uint16_t>(h, 0xE) = At<uint16_t>(a, 6);
			At<uint32_t>(h, 0) = a + 0xC;
			GteSetFarColour(0xFF, 0xFF, 0xFF);
			GteSetRotMatrixCtrl(&Camera());
			GteSetTransVectorCtrl(&Camera());
			if (At<int32_t>(h, 0x44) != 0)
				cursor = MeltFaces(h, ot, shift, cursor);
			else
				MeltStartTicks(h);
		}
		return cursor;
	}

	static void MeltDraw(uint32_t h, uint8_t *entity)
	{
		ComputeBonesWorldMatrices(entity + 0x60, entity + 0x40);
		At<uint32_t>(h, 4) = var<uint32_t>(game_addr(game::g_BattleVertexWorkspace));
		At<int16_t>(h, 0x14) = 0;
		At<int16_t>(h, 0x16) = 0;
		At<uint32_t>(h, 0x1C) = *(const uint32_t *)(entity + 0x28);
		At<uint32_t>(h, 0x20) = *(const uint32_t *)(entity + 0x7C);
		At<uint32_t>(h, 0x10) = var<uint32_t>(game_addr(game::BATTLE_PACKET_BUFFER_END));
		const uint8_t b = entity[7];
		At<uint8_t>(h, 0x2A) = b;
		At<uint8_t>(h, 0x29) = b;
		At<uint8_t>(h, 0x28) = b;
		At<int16_t>(h, 0x18) = 0x140;
		At<int16_t>(h, 0x1A) = 0xD8;
		FrameArena() = MeltModel(*(const uint32_t *)(entity + 0x64), h, OT44(), 2, FrameArena());
		BuildBoneMatricesFromPose(entity + 0x60);
	}

	static uint32_t __cdecl MeltTask(TaskNode *node)
	{
		const uint32_t n = (uint32_t)node;
		if (Counter(n) < 0xA0)
		{
			const uint32_t h = (uint32_t)FieldAlloc(0xB0);
			At<uint32_t>(h, 0x2C) = FaceStates();
			const int32_t c = Counter(n);
			At<int32_t>(h, 0x44) = c;
			if (c != 0)
				Caster()[0] |= 4;
			MeltDraw(h, Caster());
			FieldFree(0xB0);
		}
		if (DrawOnly()) return 0;
		Counter(n) = (int16_t)(Counter(n) + 1);
		return Counter(n) > 0xA0 ? TASK_END : 0;
	}

	static void StartMelt()
	{
		uint32_t n = (uint32_t)AddTaskToQueue(QMaster(), game_addr(ORIG_MeltTask));
		At<int16_t>(n, 0xC) = 0;
		const uint32_t base = TexBase();
		uint32_t d = base + 0x28000;
		FaceStates() = d;
		for (int k = 0; k < 0x461; k++, d += 4)
			At<uint32_t>(d, 0) = 0x40004000;
		ShardTable() = base + 0x29184;
		ShardCursor() = 0;
		uint32_t e = base + 0x29184;
		for (int k = 0; k < 1000; k++, e += 0x44)
			At<int16_t>(e, 0) = 0;
	}

	static void GlowDraw(uint32_t w, uint32_t h, uint32_t e)
	{
		At<int16_t>(w, 2) = At<int16_t>(e, 0xE);
		ComposeZYXRotationMatrix((const int16_t *)w, (Mat4x3 *)(w + 0x28));
		At<int32_t>(w, 0x3C) = At<int16_t>(e, 8);
		At<int32_t>(w, 0x40) = At<int16_t>(e, 0xA);
		At<int32_t>(w, 0x44) = At<int16_t>(e, 0xC);
		const int32_t s = At<int16_t>(e, 6);
		At<int32_t>(w, 0x50) = s;
		At<int32_t>(w, 0x48) = s;
		Scale3DMatrix((Mat4x3 *)(w + 0x28), (const int32_t *)(w + 0x48));
		ComposeAffineTransform((const Mat4x3 *)(w + 8), (const Mat4x3 *)(w + 0x28), (Mat4x3 *)(w + 0x28));
		GteSetRotMatrix((const Mat4x3 *)(w + 0x28));
		GteSetTransVector((const Mat4x3 *)(w + 0x28));
		const int16_t c = At<int16_t>(e, 4);
		At<uint32_t>(h, 0x20) = 0x33;
		if (c < 0x10)
		{
			At<uint32_t>(h, 0x20) = 0xF3;
			At<int32_t>(h, 0xC) = (0x10 - c) << 8;
		}
		At<int32_t>(h, 0x14) = At<int16_t>(e, 0x12);
		At<uint32_t>(h, 0x24) = At<uint32_t>(game_addr(GLOW_COLOURS), At<int16_t>(e, 0x10) * 4);
		FrameArena() = GlowRender((void *)h, OT44(), 2, FrameArena());
	}

	static uint32_t __cdecl GlowTask(TaskNode *node)
	{
		const uint32_t n = (uint32_t)node;
		const uint32_t w = (uint32_t)FieldAlloc(0x68);
		if (!DrawOnly() && Counter(n) <= 0x78 && Counter(n) % 16 == 0)
		{
			const uint32_t e = PoolFree(game_addr(POOL1), game_addr(POOL1_END), 100);
			if (e)
			{
				At<int32_t>(e, 0) = 2;
				At<int16_t>(e, 4) = 0;
				At<int16_t>(e, 6) = (int16_t)(CrtRand() % 0x1400 + 0x600);
				At<int16_t>(e, 8) = (int16_t)(CrtRand() % 0x1770 + var<int32_t>(game_addr(game::g066_Centre)) - 0xBB8);
				At<int16_t>(e, 0xA) = 0;
				At<int16_t>(e, 0xC) = (int16_t)(CrtRand() % 0x1770 + var<int32_t>(game_addr(game::unk_24C9174)) - 0xBB8);
				At<int16_t>(e, 0xE) = (int16_t)(CrtRand() % 0x1000);
				At<int16_t>(e, 0x10) = 0;
				const int16_t v = (int16_t)(CrtRand() % 0x10 + 4);
				At<int16_t>(e, 0x16) = v;
				At<int16_t>(e, 0x12) = v;
			}
		}
		const uint32_t h = (uint32_t)FieldAlloc(0x90);
		At<uint32_t>(h, 8) = 0;
		At<int16_t>(h, 0x18) = 0;
		At<uint32_t>(h, 0x10) = 0;
		ff8fx::copy<(0x20)>((void *)(w + 8), (const void *)&Camera());
		At<uint32_t>(h, 0) = game_addr(game::unk_DE97A4);
		At<int16_t>(h, 0x1A) = 0x80;
		At<int16_t>(h, 0x1C) = 0x40;
		At<int16_t>(h, 0x1E) = 0x80;
		At<int32_t>(w, 0x4C) = 0x3000;
		At<int16_t>(w, 4) = 0;
		At<int16_t>(w, 0) = 0;
		for (uint32_t e = game_addr(POOL1); e < game_addr(POOL1_END); e += 0x18)
		{
			if (!(At<uint8_t>(e, 0) & 2)) continue;
			GlowDraw(w, h, e);
			if (DrawOnly()) continue;
			At<int16_t>(e, 0x10) = (int16_t)(At<int16_t>(e, 0x10) + 1);
			if (At<int16_t>(e, 0x10) >= 0xC)
				At<int16_t>(e, 0x10) = 0;
			At<int16_t>(e, 0x12) = (int16_t)(At<int16_t>(e, 0x12) + At<int16_t>(e, 0x16));
			At<int16_t>(e, 4) = (int16_t)(At<int16_t>(e, 4) + 1);
		}
		FieldFree(0x90);
		FieldFree(0x68);
		if (DrawOnly()) return 0;
		Counter(n) = (int16_t)(Counter(n) + 1);
		if (Counter(n) < 0x8C) return 0;
		for (uint32_t e = game_addr(POOL1); e < game_addr(POOL1_END); e += 0x18)
			if (At<uint8_t>(e, 0) & 2) At<int32_t>(e, 0) = 0;
		return TASK_END;
	}

	static void WaveDraw(uint32_t n)
	{
		int16_t angles[3] = { 0, At<int16_t>(n, 0x18), 0 };
		Mat4x3 m;
		ComposeZYXRotationMatrix(angles, &m);
		m.t[0] = At<int16_t>(n, 0x10);
		m.t[1] = At<int16_t>(n, 0x12);
		const int32_t s = At<int16_t>(n, 0x1C);
		int32_t scale[3];
		scale[2] = s;
		scale[0] = s;
		m.t[2] = At<int16_t>(n, 0x14);
		scale[1] = At<int16_t>(n, 0x20);
		Scale3DMatrix(&m, scale);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		RenderHeader *h = (RenderHeader *)FieldAlloc(0x58);
		h->model = game_addr(game::unk_DE562C);
		At<uint32_t>((uint32_t)h, 8) = 0;
		h->flags = 0x33;
		PacketCursor() = RenderPrimModel(h, OT44(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static uint32_t __cdecl WaveTask(TaskNode *node)
	{
		const uint32_t n = (uint32_t)node;
		WaveDraw(n);
		if (DrawOnly()) return 0;
		const int16_t c = Counter(n);
		At<int16_t>(n, 0x18) = (int16_t)(At<int16_t>(n, 0x18) + At<int16_t>(n, 0x1A));
		if (c < 0x6E)
		{
			const int16_t d = At<int16_t>(n, 0x1E);
			At<int16_t>(n, 0x1C) = (int16_t)(At<int16_t>(n, 0x1C) - d);
			At<int16_t>(n, 0x1E) = (int16_t)(d - d / 32);
		}
		Counter(n) = (int16_t)(c + 1);
		return Counter(n) >= 0x78 ? TASK_END : 0;
	}

	static void StartWave()
	{
		Centre()[1] = (int16_t)(Centre()[1] + 0x7D0);
		const uint32_t n = (uint32_t)AddTaskToQueue(QMaster(), game_addr(ORIG_WaveTask));
		At<int16_t>(n, 0x10) = Centre()[0];
		At<int16_t>(n, 0x12) = Centre()[1];
		At<int16_t>(n, 0xC) = 0;
		At<int16_t>(n, 0x14) = Centre()[2];
		At<int16_t>(n, 0x18) = 0;
		At<int16_t>(n, 0x1A) = 0x1E;
		At<int16_t>(n, 0x1C) = 0x4000;
		At<int16_t>(n, 0x1E) = 0xD7;
		At<int16_t>(n, 0x20) = 0x2000;
	}

	static void SwirlDraw(uint32_t n)
	{
		int16_t angles[3] = { 0, At<int16_t>(n, 0x18), 0 };
		Mat4x3 m;
		ComposeZYXRotationMatrix(angles, &m);
		m.t[0] = At<int16_t>(n, 0x10);
		m.t[1] = At<int16_t>(n, 0x12);
		const int32_t s = At<int16_t>(n, 0x1C);
		const int32_t scale[3] = { s, s, s };
		m.t[2] = At<int16_t>(n, 0x14);
		Scale3DMatrix(&m, scale);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		RenderHeader *h = (RenderHeader *)FieldAlloc(0x58);
		const int32_t out = At<int16_t>(n, 0x22);
		const int16_t in = At<int16_t>(n, 0x16);
		At<uint32_t>((uint32_t)h, 8) = 0;
		const int16_t c = Counter(n);
		const int32_t start_out = At<int16_t>(n, 0xE) - out;
		h->model = game_addr(game::unk_DE7F54);
		h->flags = 0x33;
		if (c < in)
		{
			h->fade = 0x1000 - (0x1000 / in) * c;
			h->flags = 0xF3;
		}
		else if (c >= start_out)
		{
			h->fade = (c - start_out) * (0x1000 / out);
			h->flags = 0xF3;
		}
		PacketCursor() = RenderPrimModel(h, OT44(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static uint32_t __cdecl SwirlTask(TaskNode *node)
	{
		const uint32_t n = (uint32_t)node;
		SwirlDraw(n);
		if (DrawOnly()) return 0;
		At<int16_t>(n, 0x18) = (int16_t)(At<int16_t>(n, 0x18) + At<int16_t>(n, 0x1A));
		At<int16_t>(n, 0x1C) = (int16_t)(At<int16_t>(n, 0x1C) + At<int16_t>(n, 0x1E));
		Counter(n) = (int16_t)(Counter(n) + 1);
		return Counter(n) >= At<int16_t>(n, 0xE) ? TASK_END : 0;
	}

	static void StartSwirls()
	{
		uint32_t n = (uint32_t)AddTaskToQueue(QMaster(), game_addr(ORIG_SwirlTask));
		At<uint32_t>(n, 0x10) = var<uint32_t>(game_addr(game::g066_Centre));
		At<uint32_t>(n, 0x14) = var<uint32_t>(game_addr(game::unk_24C9174));
		At<int16_t>(n, 0x12) = (int16_t)(At<int16_t>(n, 0x12) + (int16_t)0xFC18);
		At<int16_t>(n, 0xC) = 0;
		At<int16_t>(n, 0xE) = 0x22;
		At<int16_t>(n, 0x16) = 8;
		At<int16_t>(n, 0x18) = (int16_t)(CrtRand() % 0x800);
		At<int16_t>(n, 0x1A) = 0x1E;
		At<int16_t>(n, 0x1C) = 0x1800;
		At<int16_t>(n, 0x1E) = 1;
		At<int16_t>(n, 0x20) = 0x1000;
		At<int16_t>(n, 0x22) = 0xC;
		n = (uint32_t)AddTaskToQueue(QMaster(), game_addr(ORIG_SwirlTask));
		At<uint32_t>(n, 0x10) = var<uint32_t>(game_addr(game::g066_Centre));
		At<uint32_t>(n, 0x14) = var<uint32_t>(game_addr(game::unk_24C9174));
		At<int16_t>(n, 0xC) = 0;
		At<int16_t>(n, 0xE) = 0x24;
		At<int16_t>(n, 0x16) = 0xC;
		At<int16_t>(n, 0x18) = (int16_t)(CrtRand() % 0x800);
		At<int16_t>(n, 0x1A) = 0x37;
		At<int16_t>(n, 0x1C) = 0x1800;
		At<int16_t>(n, 0x1E) = 1;
		At<int16_t>(n, 0x20) = 0xE00;
		At<int16_t>(n, 0x22) = 0xC;
		n = (uint32_t)AddTaskToQueue(QMaster(), game_addr(ORIG_SwirlTask));
		At<uint32_t>(n, 0x10) = var<uint32_t>(game_addr(game::g066_Centre));
		At<uint32_t>(n, 0x14) = var<uint32_t>(game_addr(game::unk_24C9174));
		At<int16_t>(n, 0x12) = (int16_t)(At<int16_t>(n, 0x12) + 0x4B0);
		At<int16_t>(n, 0xC) = 0;
		At<int16_t>(n, 0xE) = 0x20;
		At<int16_t>(n, 0x16) = 0xA;
		At<int16_t>(n, 0x22) = 0xA;
		At<int16_t>(n, 0x18) = (int16_t)(CrtRand() % 0x800);
		At<int16_t>(n, 0x1A) = 0x28;
		At<int16_t>(n, 0x1C) = 0x1000;
		At<int16_t>(n, 0x1E) = 1;
		At<int16_t>(n, 0x20) = 0xA00;
	}

	static void FlareDraw(uint32_t n)
	{
		int16_t angles[3] = { 0, At<int16_t>(n, 0x18), 0 };
		Mat4x3 m;
		ComposeZYXRotationMatrix(angles, &m);
		m.t[0] = At<int16_t>(n, 0x10);
		m.t[1] = At<int16_t>(n, 0x12);
		const int32_t s = At<int16_t>(n, 0x1C);
		const int32_t scale[3] = { s, s, s };
		m.t[2] = At<int16_t>(n, 0x14);
		Scale3DMatrix(&m, scale);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		RenderHeader *h = (RenderHeader *)FieldAlloc(0x58);
		const int16_t c = Counter(n);
		h->model = game_addr(game::unk_DE98E4);
		At<uint32_t>((uint32_t)h, 8) = 0;
		h->flags = 0x33;
		if (c < 6)
		{
			h->fade = 0x1000 - c * 682;
			h->flags = 0xF3;
		}
		else if (c >= 0x22)
		{
			h->fade = (c - 0x22) << 8;
			h->flags = 0xF3;
		}
		PacketCursor() = RenderPrimModel(h, OT44(), 3, PacketCursor());
		FieldFree(0x58);
	}

	static uint32_t __cdecl FlareTask(TaskNode *node)
	{
		const uint32_t n = (uint32_t)node;
		FlareDraw(n);
		if (DrawOnly()) return 0;
		const int16_t d = At<int16_t>(n, 0x1E);
		At<int16_t>(n, 0x1C) = (int16_t)(At<int16_t>(n, 0x1C) + d);
		At<int16_t>(n, 0x1E) = (int16_t)(d - d / 64);
		Counter(n) = (int16_t)(Counter(n) + 1);
		return Counter(n) >= 0x32 ? TASK_END : 0;
	}

	static void StartFlare()
	{
		const uint32_t n = (uint32_t)AddTaskToQueue(QMaster(), game_addr(ORIG_FlareTask));
		At<uint32_t>(n, 0x10) = var<uint32_t>(game_addr(game::g066_Centre));
		At<uint32_t>(n, 0x14) = var<uint32_t>(game_addr(game::unk_24C9174));
		At<int16_t>(n, 0x12) = (int16_t)(At<int16_t>(n, 0x12) + 0x118);
		At<int16_t>(n, 0xC) = 0;
		At<int16_t>(n, 0x18) = (int16_t)(CrtRand() % 0x1000);
		At<int16_t>(n, 0x1A) = 0x50;
		At<int16_t>(n, 0x1C) = 0x2000;
		At<int16_t>(n, 0x1E) = 0x200;
	}

	static uint32_t __cdecl ShardSpin(TaskNode *node)
	{
		const uint32_t n = (uint32_t)node;
		const uint32_t w = (uint32_t)FieldAlloc(0x48);
		if (!DrawOnly() && Counter(n) <= 0x6E)
		{
			At<uint32_t>(w, 0) = var<uint32_t>(game_addr(game::g066_Centre));
			At<uint32_t>(w, 4) = var<uint32_t>(game_addr(game::unk_24C9174));
			for (int k = 0; k < 2; k++)
			{
				const uint32_t e = PoolFree(game_addr(POOL2), game_addr(POOL2_END), 200);
				if (!e) break;
				At<int32_t>(e, 0) = 1;
				At<int16_t>(e, 4) = 0;
				At<int16_t>(e, 6) = (int16_t)(CrtRand() % 0x2000 + 0x1600);
				At<uint32_t>(e, 8) = At<uint32_t>(w, 0);
				At<uint32_t>(e, 0xC) = At<uint32_t>(w, 4);
				At<int16_t>(e, 8) = (int16_t)(At<int16_t>(e, 8) + CrtRand() % 0x1DB0 - 0xED8);
				At<int16_t>(e, 0xA) = (int16_t)(At<int16_t>(e, 0xA) + CrtRand() % 0x1DB0 - 0xED8);
				At<int16_t>(e, 0xC) = (int16_t)(At<int16_t>(e, 0xC) + CrtRand() % 0x1DB0 - 0xED8);
			}
		}
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		SpriteBegin(w, h, game_addr(game::unk_DE45D0));
		for (uint32_t e = game_addr(POOL2); e < game_addr(POOL2_END); e += 0x18)
		{
			if (!(At<uint8_t>(e, 0) & 1)) continue;
			SpritePushed(w, h, e, -(At<int16_t>(e, 6) >> 3), At<int16_t>(e, 4));
			if (DrawOnly()) continue;
			if (At<int16_t>((uint32_t)h, 0x28) < 0)
				At<int32_t>(e, 0) = 0;
			else
				At<int16_t>(e, 4) = (int16_t)(At<int16_t>(e, 4) + 1);
		}
		FieldFree(0xB4);
		FieldFree(0x48);
		if (DrawOnly()) return 0;
		Counter(n) = (int16_t)(Counter(n) + 1);
		if (Counter(n) < 0x78) return 0;
		for (uint32_t e = game_addr(POOL2); e < game_addr(POOL2_END); e += 0x18)
			if (At<uint8_t>(e, 0) & 1) At<int32_t>(e, 0) = 0;
		return TASK_END;
	}

	static void SparkRainDraw(uint32_t w, RenderHeader *h, uint32_t e)
	{
		At<int16_t>(w, 4) = At<int16_t>(e, 0xC);
		ComposeZYXRotationMatrix((const int16_t *)w, (Mat4x3 *)(w + 0x28));
		At<int32_t>(w, 0x48) = At<int16_t>(e, 6);
		Scale3DMatrix((Mat4x3 *)(w + 0x28), (const int32_t *)(w + 0x48));
		GteSetRotMatrix((const Mat4x3 *)(w + 0x28));
		const int16_t c = At<int16_t>(e, 4);
		h->flags = 0x33;
		if (c < 6)
		{
			h->fade = 0x1000 - c * 682;
			h->flags = 0xF3;
		}
		else if (c >= 0xA)
		{
			h->fade = (c - 0xA) * 682;
			h->flags = 0xF3;
		}
		h->colour = At<uint32_t>(game_addr(RAIN_COLOURS), At<int16_t>(e, 8) * 4);
		PacketCursor() = RenderModel(h, OT44(), 2, PacketCursor());
	}

	static uint32_t __cdecl SparkRain(TaskNode *node)
	{
		const uint32_t n = (uint32_t)node;
		const uint32_t w = (uint32_t)FieldAlloc(0x68);
		if (!DrawOnly() && Counter(n) <= 0x6E)
		{
			for (int k = 0; k < 1; k++)
			{
				const uint32_t e = PoolFree(game_addr(POOL1), game_addr(POOL1_END), 100);
				if (!e) break;
				At<int32_t>(e, 0) = 1;
				At<int16_t>(e, 4) = 0;
				At<int16_t>(e, 6) = (int16_t)(CrtRand() % 0x2800 + 0x3000);
				At<int16_t>(e, 8) = (int16_t)(CrtRand() % 0xC);
				At<int16_t>(e, 0xA) = 0;
				At<int16_t>(e, 0xC) = (int16_t)(CrtRand() % 0x1000);
				At<int16_t>(e, 0x12) = (int16_t)(-8 - CrtRand() % 0x18);
				int16_t v = (int16_t)(CrtRand() % 0x14 + 5);
				At<int16_t>(e, 0x14) = v;
				if (At<uint8_t>(e, 0xC) & 1) At<int16_t>(e, 0x14) = (int16_t)-v;
			}
		}
		RenderHeader *h = (RenderHeader *)FieldAlloc(0x6C);
		h->model = game_addr(game::unk_DE90CC);
		At<uint32_t>((uint32_t)h, 8) = 0;
		TransformCameraByShadowRotation((const void *)game_addr(game::g066_Centre), 0x1000, -1000);
		At<int16_t>(w, 2) = 0;
		At<int16_t>(w, 0) = 0;
		At<int32_t>(w, 0x50) = 0x2800;
		At<int32_t>(w, 0x4C) = 0x2800;
		for (uint32_t e = game_addr(POOL1); e < game_addr(POOL1_END); e += 0x18)
		{
			if (!(At<uint8_t>(e, 0) & 1)) continue;
			SparkRainDraw(w, h, e);
			if (DrawOnly()) continue;
			const int16_t c = At<int16_t>(e, 4);
			if (c >= 0x10)
			{
				At<int32_t>(e, 0) = 0;
				continue;
			}
			At<int16_t>(e, 4) = (int16_t)(c + 1);
			At<int16_t>(e, 8) = (int16_t)(At<int16_t>(e, 8) + 1);
			if (At<int16_t>(e, 8) >= 0xC)
				At<int16_t>(e, 8) = 0;
			At<int16_t>(e, 0xA) = (int16_t)(At<int16_t>(e, 0xA) + At<int16_t>(e, 0x12));
			At<int16_t>(e, 0xC) = (int16_t)(At<int16_t>(e, 0xC) + At<int16_t>(e, 0x14));
		}
		FieldFree(0x6C);
		FieldFree(0x68);
		if (DrawOnly()) return 0;
		Counter(n) = (int16_t)(Counter(n) + 1);
		if (Counter(n) < 0x78) return 0;
		for (uint32_t e = game_addr(POOL1); e < game_addr(POOL1_END); e += 0x18)
			if (At<uint8_t>(e, 0) & 1) At<int32_t>(e, 0) = 0;
		return TASK_END;
	}

	static void ScreenAnchor(const int16_t *pos, int16_t scale, int32_t zoff)
	{
		At<int16_t>(game_addr(BILLBOARD), 0x10) = scale;
		At<int16_t>(game_addr(BILLBOARD), 8) = scale;
		At<int16_t>(game_addr(BILLBOARD), 0) = scale;
		GteSetRotMatrix(&Camera());
		GteSetTransVector(&Camera());
		GteLoadV0(pos);
		GteRTPS();
		int16_t xy[2];
		GteReadSXY2(xy);
		const int16_t sx = (int16_t)(xy[0] / 8);
		const int16_t sy = (int16_t)(xy[1] / 8);
		At<int32_t>(game_addr(BILLBOARD), 0x14) = sx - 0xA0;
		At<int32_t>(game_addr(BILLBOARD), 0x18) = sy - 0x6C;
		At<int32_t>(game_addr(BILLBOARD), 0x1C) = var<int16_t>(game_addr(game::BATTLE_PROJECTION_DISTANCE)) + zoff;
		GteSetRotMatrix((const Mat4x3 *)game_addr(BILLBOARD));
		GteSetTransVector((const Mat4x3 *)game_addr(BILLBOARD));
	}

	static void BillboardDraw(uint32_t n)
	{
		int16_t angles[3] = { 0, 0, 0 };
		Mat4x3 m;
		ComposeZYXRotationMatrix(angles, &m);
		const int32_t scale[3] = { At<int16_t>(n, 0x1C), At<int16_t>(n, 0x20), 0 };
		Scale3DMatrix(&m, scale);
		ScreenAnchor((const int16_t *)(n + 0x10), 0x1000, 0);
		GteSetRotMatrix(&m);
		RenderHeader *h = (RenderHeader *)FieldAlloc(0x58);
		const int16_t c = Counter(n);
		h->model = game_addr(game::unk_DE87FC);
		At<uint32_t>((uint32_t)h, 8) = 0;
		h->flags = 0x33;
		if (c >= 0x16)
		{
			h->flags = 0xF3;
			h->fade = (c - 0x16) << 10;
		}
		PacketCursor() = RenderPrimModel(h, OT44(), 0xE, PacketCursor());
		FieldFree(0x58);
	}

	static uint32_t __cdecl Billboard(TaskNode *node)
	{
		const uint32_t n = (uint32_t)node;
		BillboardDraw(n);
		if (DrawOnly()) return 0;
		const int16_t d = At<int16_t>(n, 0x1E);
		At<int16_t>(n, 0x1C) = (int16_t)(At<int16_t>(n, 0x1C) + d);
		At<int16_t>(n, 0x1E) = (int16_t)(d - d / 6);
		Counter(n) = (int16_t)(Counter(n) + 1);
		return Counter(n) >= 0x1A ? TASK_END : 0;
	}

	static void StartBillboard()
	{
		const uint32_t n = (uint32_t)AddTaskToQueue(QMaster(), game_addr(ORIG_Billboard));
		At<uint32_t>(n, 0x10) = var<uint32_t>(game_addr(game::g066_Centre));
		At<int16_t>(n, 0xC) = 0;
		At<uint32_t>(n, 0x14) = var<uint32_t>(game_addr(game::unk_24C9174));
		At<int16_t>(n, 0x1E) = 0x600;
		At<int16_t>(n, 0x1C) = 0x600;
		At<int16_t>(n, 0x20) = 0x20;
	}

	static void Billboard2Draw(uint32_t n)
	{
		int16_t angles[3] = { 0, 0, 0 };
		Mat4x3 m;
		ComposeZYXRotationMatrix(angles, &m);
		const int32_t s = At<int16_t>(n, 0x1C);
		const int32_t scale[3] = { s, s, 0 };
		m.t[0] = 0;
		m.t[1] = 0;
		m.t[2] = var<int16_t>(game_addr(game::BATTLE_PROJECTION_DISTANCE));
		Scale3DMatrix(&m, scale);
		ScreenAnchor((const int16_t *)(n + 0x10), 0x1000, 0);
		GteSetRotMatrix(&m);
		RenderHeader *h = (RenderHeader *)FieldAlloc(0x58);
		h->model = game_addr(game::unk_DE87FC);
		At<uint32_t>((uint32_t)h, 8) = 0;
		h->flags = 0x33;
		PacketCursor() = RenderPrimModel(h, OT44(), 0xE, PacketCursor());
		FieldFree(0x58);
	}

	static uint32_t __cdecl Billboard2(TaskNode *node)
	{
		const uint32_t n = (uint32_t)node;
		Billboard2Draw(n);
		if (DrawOnly()) return 0;
		const int16_t c = Counter(n);
		if (c < 0x11)
		{
			const int16_t d = At<int16_t>(n, 0x1E);
			At<int16_t>(n, 0x1C) = (int16_t)(At<int16_t>(n, 0x1C) + d);
			At<int16_t>(n, 0x1E) = (int16_t)(d - d / 16);
		}
		Counter(n) = (int16_t)(c + 1);
		return Counter(n) >= 0x11 ? TASK_END : 0;
	}

	static void StartBillboard2()
	{
		const uint32_t n = (uint32_t)AddTaskToQueue(QMaster(), game_addr(ORIG_Billboard2));
		At<uint32_t>(n, 0x10) = var<uint32_t>(game_addr(game::g066_Centre));
		At<int16_t>(n, 0xC) = 0;
		At<uint32_t>(n, 0x14) = var<uint32_t>(game_addr(game::unk_24C9174));
		At<int16_t>(n, 0x1E) = 0x1D5;
		At<int16_t>(n, 0x1C) = 0x1D5;
	}

	static void SpriteAtMAC(uint8_t *h, uint32_t e, int16_t frame)
	{
		GteLoadV0((const void *)(e + 8));
		GteMVMVA_LightV0Bk();
		GteSetRotScale(At<int16_t>(e, 6));
		At<int16_t>((uint32_t)h, 4) = frame;
		GteTransFromMAC();
		FrameArena() = InitEffectSequenceFromData(h, OT44(), 2, FrameArena());
	}

	static uint32_t __cdecl DustFall2(TaskNode *node)
	{
		const uint32_t n = (uint32_t)node;
		const uint32_t pool = Pool4();
		const uint32_t w = (uint32_t)FieldAlloc(0x48);
		if (!DrawOnly() && Counter(n) <= 0x134)
		{
			At<uint32_t>(w, 0) = At<uint32_t>(n, 0x10);
			At<uint32_t>(w, 4) = At<uint32_t>(n, 0x14);
			for (int k = 0; k < 8; k++)
			{
				int32_t i = 0;
				uint32_t e = pool;
				while (At<int32_t>(e, 0) != 0)
				{
					i++;
					e += 0x18;
					if (i >= 400) break;
				}
				if (i >= 400) break;
				e = pool + i * 0x18;
				At<int32_t>(e, 0) = 1;
				At<int16_t>(e, 4) = 0;
				At<int16_t>(e, 6) = (int16_t)(CrtRand() % 0xC00 + 0x100);
				At<uint32_t>(e, 8) = At<uint32_t>(w, 0);
				At<uint32_t>(e, 0xC) = At<uint32_t>(w, 4);
				At<int16_t>(e, 8) = (int16_t)(At<int16_t>(e, 8) + CrtRand() % 0x2710 - 0x1388);
				At<int16_t>(e, 0xA) = (int16_t)(-2000 - CrtRand() % 0x708);
				At<int16_t>(e, 0xC) = (int16_t)(At<int16_t>(e, 0xC) + CrtRand() % 0x2710 - 0x1388);
				At<int32_t>(w, 0x28) = CrtRand() % 0x600 + 0x300;
				At<int32_t>(w, 0x2C) = CrtRand() % 0x500 + 0x400;
				At<int32_t>(w, 0x30) = CrtRand() % 0x700 + 0x200;
				NormalizeVector((const int32_t *)(w + 0x28), (int32_t *)(w + 0x28));
				const int32_t speed = CrtRand() % 0x37 + 0xA;
				At<int16_t>(e, 0x10) = (int16_t)(mul32(At<int32_t>(w, 0x28), speed) >> 12);
				At<int16_t>(e, 0x12) = (int16_t)(mul32(At<int32_t>(w, 0x2C), speed) >> 12);
				At<int16_t>(e, 0x14) = (int16_t)(mul32(At<int32_t>(w, 0x30), speed) >> 12);
			}
		}
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		At<int16_t>((uint32_t)h, 0x24) = 8;
		ff8fx::copy<(0x20)>((void *)(w + 8), (const void *)&Camera());
		At<uint32_t>((uint32_t)h, 0) = game_addr(game::unk_DE477C);
		GteSetLightMatrix((const void *)(w + 8));
		GteSetBackFromTrans((const void *)(w + 8));
		uint32_t e = pool;
		for (int k = 0; k < 400; k++, e += 0x18)
		{
			if (!(At<uint8_t>(e, 0) & 1)) continue;
			SpriteAtMAC(h, e, (int16_t)(At<int16_t>(e, 4) >> 2));
			if (DrawOnly()) continue;
			if (At<int16_t>((uint32_t)h, 0x28) < 0)
				At<int32_t>(e, 0) = 0;
			else
			{
				At<int16_t>(e, 4) = (int16_t)(At<int16_t>(e, 4) + 1);
				At<int16_t>(e, 8) = (int16_t)(At<int16_t>(e, 8) + At<int16_t>(e, 0x10));
				At<int16_t>(e, 0xA) = (int16_t)(At<int16_t>(e, 0xA) + At<int16_t>(e, 0x12));
				At<int16_t>(e, 0xC) = (int16_t)(At<int16_t>(e, 0xC) + At<int16_t>(e, 0x14));
			}
		}
		FieldFree(0xB4);
		FieldFree(0x48);
		if (DrawOnly()) return 0;
		Counter(n) = (int16_t)(Counter(n) + 1);
		if (Counter(n) < 0x134) return 0;
		e = pool;
		for (int k = 0; k < 400; k++, e += 0x18)
			if (At<uint8_t>(e, 0) & 1) At<int32_t>(e, 0) = 0;
		return TASK_END;
	}

	static void StartDustFall2()
	{
		const uint32_t n = (uint32_t)AddTaskToQueue(QMaster(), game_addr(ORIG_DustFall2));
		At<int16_t>(n, 0x10) = Centre()[0];
		At<int16_t>(n, 0xC) = 0;
		At<int16_t>(n, 0x12) = 0;
		At<int16_t>(n, 0x14) = (int16_t)(var<int32_t>(game_addr(game::unk_24C9174)) + 0x1DB0);
		uint32_t e = ShardTable();
		Pool4() = e;
		for (int k = 0; k < 400; k++, e += 0x18)
			At<int32_t>(e, 0) = 0;
	}

	static uint32_t __cdecl DustFall(TaskNode *node)
	{
		const uint32_t n = (uint32_t)node;
		const uint32_t pool = Pool3();
		const uint32_t w = (uint32_t)FieldAlloc(0x48);
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		At<int16_t>((uint32_t)h, 0x24) = 8;
		ff8fx::copy<(0x20)>((void *)(w + 8), (const void *)&Camera());
		At<uint32_t>((uint32_t)h, 0) = game_addr(game::unk_DE477C);
		GteSetLightMatrix((const void *)(w + 8));
		GteSetBackFromTrans((const void *)(w + 8));
		uint32_t e = pool;
		for (int k = 0; k < 800; k++, e += 0x18)
		{
			if (!(At<uint8_t>(e, 0) & 1)) continue;
			SpriteAtMAC(h, e, At<int16_t>(e, 4));
			if (DrawOnly()) continue;
			if (At<int16_t>((uint32_t)h, 0x28) < 0)
				At<int32_t>(e, 0) = 0;
			else
			{
				At<int16_t>(e, 4) = (int16_t)(At<int16_t>(e, 4) + 1);
				At<int16_t>(e, 0xA) = (int16_t)(At<int16_t>(e, 0xA) + At<int16_t>(e, 0x12));
			}
		}
		FieldFree(0xB4);
		FieldFree(0x48);
		if (DrawOnly()) return 0;
		Counter(n) = (int16_t)(Counter(n) + 1);
		return Counter(n) >= 0x98 ? TASK_END : 0;
	}

	static void StartDustFall()
	{
		const uint32_t n = (uint32_t)AddTaskToQueue(QMaster(), game_addr(ORIG_DustFall));
		At<int16_t>(n, 0xC) = 0;
		uint32_t e = TexBase();
		Pool3() = e;
		Pool3Cursor() = 0;
		for (int k = 0; k < 800; k++, e += 0x18)
			At<int32_t>(e, 0) = 0;
	}

	static uint32_t __cdecl StageTask(TaskNode *node)
	{
		const uint32_t n = (uint32_t)node;
		if (Counter(n) < 0x78)
		{
			var<uint8_t>(game_addr(game::unk_1D989BD)) &= 0xFD;
			const uint32_t h = (uint32_t)FieldAlloc(0x2C);
			At<int16_t>(h, 0x14) = 0;
			At<int16_t>(h, 0x16) = 0;
			At<int16_t>(h, 0x18) = 0x140;
			At<int16_t>(h, 0x1A) = 0xD8;
			At<uint32_t>(h, 4) = var<uint32_t>(game_addr(game::g_BattleVertexWorkspace));
			At<uint32_t>(h, 0x10) = var<uint32_t>(game_addr(game::BATTLE_PACKET_BUFFER_END));
			At<int32_t>(h, 0x20) = -1;
			BuildBoneMatricesFromPose((void *)game_addr(game::unk_1D989D0));
			ComputeBonesWorldMatrices((void *)game_addr(game::unk_1D989D0), (void *)&Camera());
			if (var<uint8_t>(game_addr(game::unk_1D989BD)) & 1)
			{
				At<uint32_t>(h, 0x1C) = var<uint32_t>(game_addr(game::unk_1D989E4));
				At<int16_t>(h, 0x24) = var<int16_t>(game_addr(game::unk_1D989BE));
				FrameArena() = RenderGeometry(game_addr(game::unk_1D989C0), (void *)h, OT44(), 4, FrameArena());
			}
			FieldFree(0x2C);
		}
		if (DrawOnly()) return 0;
		Counter(n) = (int16_t)(Counter(n) + 1);
		if (Counter(n) <= 0x78) return 0;
		var<uint8_t>(game_addr(game::unk_1D989BD)) |= 2;
		return TASK_END;
	}

	static uint32_t __cdecl RootTask(TaskNode *node)
	{
		const uint32_t n = (uint32_t)node;
		g_frame_pad = 0;
		if (At<uint8_t>(n, 0xC) & 1)
			PacketCursor() = TexBase() + 0x10000;
		else
			PacketCursor() = TexBase();
		const int alive = ExecuteTaskQueue(QMaster());
		Counter(n) = (int16_t)(Counter(n) + 1);
		return alive ? 0 : TASK_END;
	}

	static uint32_t __cdecl Master(TaskNode *node)
	{
		const uint32_t n = (uint32_t)node;
		ComposeAffineTransform(&Camera(), (const Mat4x3 *)game_addr(CASTER_FRAME), (Mat4x3 *)game_addr(CASTER_VIEW));
		const uint32_t uf = UpdateFlags();
		if (uf & 0x201)
		{
			if (uf & 1) return 0;
			if (PauseQuery() < 0) return 0;
		}
		if (Counter(n) == 0)
		{
			CameraScriptStart(game_addr(game::unk_DEA484), game_addr(CASTER_FRAME), Caster(), QMaster(), 0);
			ArmCameraReturn();
			*(int32_t *)(Caster() + 0x7C) = 3;
			Voice() = ClaimVoiceSlot((const void *)game_addr(game::unk_DEA20C), 1, 0x80);
			Sub4A8480(0);
			var<uint32_t>(game_addr(game::battle_to_update_flags_dword_1D96A9C)) = UpdateFlags() | 0x400;
		}
		int32_t c = Counter(n);
		if (c < 0x35)
		{
			if (c == 1)
				BdPlaySE((const void *)game_addr(game::unk_DEA344), 1, 0x80);
			else if (c == 0)
			{
				ChainTransformation(Caster(), 0x12);
				AddFeedbackTask(0, 0x35);
				MusicVolumeTrans(0, 0x34, 0);
			}
		}
		else if ((c -= 0x35) < 0x80)
		{
			if (c == 0)
			{
				ChainTransformation(Caster(), 0x13);
				int16_t p1[4], p2[4];
				GetEffectSpawnPosition(Caster(), 1, 0x800, p1);
				GetEffectSpawnPosition(Caster(), 0xA, 0x800, p2);
				Centre()[0] = (int16_t)((p2[0] + p1[0]) / 2);
				Centre()[1] = (int16_t)((p2[1] + p1[1]) / 2);
				Centre()[2] = (int16_t)((p2[2] + p1[2]) / 2);
				const int32_t size = *(const int16_t *)(Caster() + 0x26);
				Push() = (int16_t)((size * 1450) >> 13);
				MusicStop(0);
			}
			else if (c == 1)
				QueueTIMUpload(TexFile());
			else if (c == 2)
				QueueTIMUpload(game_addr(game::unk_ECC40C));
			else if (c == 3)
			{
				AddMasterTask(game_addr(ORIG_SparkTask));
				AddMasterTask(game_addr(ORIG_SpawnerTask));
			}
		}
		else if ((c -= 0x80) < 0x24)
		{
			if (c == 0)
				ChainTransformation(Caster(), 0x14);
			else if (c == 0xA)
				BattleMessage(CasterSlot(), 5, 0x28);
		}
		else if ((c -= 0x24) < 5)
		{
			if (c == 1)
				BdPlaySE((const void *)game_addr(game::unk_DEA348), 1, 0x80);
			else if (c == 0)
			{
				ChainTransformation(Caster(), 0x15);
				AddFeedbackTask(1, 0xAC);
				StartDustFall();
			}
		}
		else if ((c -= 5) < 0xA0)
		{
			if (c == 0x9F)
				BdPlaySE((const void *)game_addr(game::unk_DEA34C), 1, 0x80);
			else if (c == 0)
			{
				ChainTransformation(Caster(), 0x16);
				StartMelt();
			}
			else if (c == 0x14)
				AddMasterTask(game_addr(ORIG_GlowTask));
			else if (c == 0x93)
				ScreenFadeTask(8, 0xB, 0xF, 0xFF);
		}
		else if ((c -= 0xA0) < 0x78)
		{
			if (c == 0x3C)
			{
				BdPlaySE((const void *)game_addr(game::unk_DEA354), 1, 0x80);
				StartFlare();
			}
			else if (c == 0x66)
			{
				BdPlaySE((const void *)game_addr(game::unk_DEA358), 1, 0x80);
				StartBillboard2();
			}
			else if (c == 0)
			{
				AddMasterTask(game_addr(ORIG_StageTask));
				StartWave();
			}
			else if (c == 2)
				StartSwirls();
			else if (c == 0xA)
			{
				AddMasterTask(game_addr(ORIG_ShardSpin));
				AddMasterTask(game_addr(ORIG_SparkRain));
			}
			else if (c == 0x60)
				StartBillboard();
			else if (c == 0x70)
				AddFadeTask(7, 0x14, 0x20, 0xFF);
		}
		else if ((c -= 0x78) < 0xE0)
		{
			if (c == 0x14)
				BdPlaySE((const void *)game_addr(game::unk_DEA350), 1, 0x80);
			else if (c == 0x16)
				StartDustFall2();
			else if (c == 0x1E)
				AddFeedbackTask(0, 0x12C);
		}
		else if ((c -= 0xE0) < 0x6A)
		{
			if (c == 0x4C)
				SfxVolumeTrans(0x78, 0);
		}
		const int16_t t = Counter(n);
		if (t <= 0x20)
			SetScreenFlash((uint32_t)(t << 6), 0);
		else if (t >= 0x281 && t < 0x291)
			SetScreenFlash((uint32_t)((0x291 - t) << 7), 0);
		if (Counter(n) == 0x19)
			ApplyActionResultToTarget(Ctx()->actions->targets);
		if (Counter(n) == 0x33D && VoiceBusy(Voice()))
			VoiceRelease(Voice());
		Counter(n) = (int16_t)(Counter(n) + 1);
		if (Counter(n) <= 0x340) return 0;
		SetScreenFlash(0x1000, 0);
		var<uint32_t>(game_addr(game::Battle_Camera_ReturnView_world_XZ_s16)) = var<uint32_t>(game_addr(game::Battle_Camera_world_XZ_s16));
		var<uint32_t>(game_addr(game::Battle_Camera_ReturnView_world_Y)) = var<uint32_t>(game_addr(game::Battle_Camera_world_Y));
		var<uint32_t>(game_addr(game::Battle_Camera_ReturnView_LookAt_XZ_s16)) = var<uint32_t>(game_addr(game::Battle_Camera_LookAt_XZ_s16));
		var<uint32_t>(game_addr(game::Battle_Camera_ReturnView_LookAt_Y)) = var<uint32_t>(game_addr(game::Battle_Camera_LookAt_Y));
		var<int16_t>(game_addr(game::Battle_Camera_ReturnView_Param1D8E038)) = var<int16_t>(game_addr(game::BATTLE_PROJECTION_DISTANCE));
		Sub46B450(-1, 0);
		ChainTransformation(Caster(), 1);
		return TASK_END;
	}
}

	void register_mag066_griever_ultimecia_death()
	{
		register_port(gu066::ORIG_RootTask, (void *)gu066::RootTask, "G066 RootTask", 66);
		register_port(gu066::ORIG_Master, (void *)gu066::Master, "G066 Master", 66);
		register_port(gu066::ORIG_FadeTask, (void *)gu066::FadeTask, "G066 FadeTask", 66);
		register_port(gu066::ORIG_FeedbackA, (void *)gu066::FeedbackTask, "G066 FeedbackA", 66);
		register_port(gu066::ORIG_FeedbackB, (void *)gu066::FeedbackTask, "G066 FeedbackB", 66);
		register_port(gu066::ORIG_SparkTask, (void *)gu066::SparkTask, "G066 SparkTask", 66);
		register_port(gu066::ORIG_SpawnerTask, (void *)gu066::SpawnerTask, "G066 SpawnerTask", 66);
		register_port(gu066::ORIG_RingTask, (void *)gu066::RingTask, "G066 RingTask", 66);
		register_port(gu066::ORIG_PuffTask, (void *)gu066::PuffTask, "G066 PuffTask", 66);
		register_port(gu066::ORIG_DustTask, (void *)gu066::DustTask, "G066 DustTask", 66);
		register_port(gu066::ORIG_SmokeTask, (void *)gu066::SmokeTask, "G066 SmokeTask", 66);
		register_port(gu066::ORIG_MeltTask, (void *)gu066::MeltTask, "G066 MeltTask", 66);
		register_port(gu066::ORIG_GlowTask, (void *)gu066::GlowTask, "G066 GlowTask", 66);
		register_port(gu066::ORIG_WaveTask, (void *)gu066::WaveTask, "G066 WaveTask", 66);
		register_port(gu066::ORIG_SwirlTask, (void *)gu066::SwirlTask, "G066 SwirlTask", 66);
		register_port(gu066::ORIG_FlareTask, (void *)gu066::FlareTask, "G066 FlareTask", 66);
		register_port(gu066::ORIG_ShardSpin, (void *)gu066::ShardSpin, "G066 ShardSpin", 66);
		register_port(gu066::ORIG_SparkRain, (void *)gu066::SparkRain, "G066 SparkRain", 66);
		register_port(gu066::ORIG_Billboard, (void *)gu066::Billboard, "G066 Billboard", 66);
		register_port(gu066::ORIG_Billboard2, (void *)gu066::Billboard2, "G066 Billboard2", 66);
		register_port(gu066::ORIG_DustFall2, (void *)gu066::DustFall2, "G066 DustFall2", 66);
		register_port(gu066::ORIG_DustFall, (void *)gu066::DustFall, "G066 DustFall", 66);
		register_port(gu066::ORIG_StageTask, (void *)gu066::StageTask, "G066 StageTask", 66);
	}
}

