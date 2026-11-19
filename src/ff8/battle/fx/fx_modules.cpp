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

// Memory owned by each effect, for the verifier

#include "fx_modules.h"

namespace ff8fx::mod
{
	static const Region streams_q116[] = { { game::unk_1298C68, 0x109FC }, { game::unk_12A9664, 0x4D9C } };
	static const Region streams_c199[] = { { game::unk_CF3564, 0x30 }, { game::g_CamScript_OtherTrack, 0x162B } };
	static const Region streams_p291[] = { { game::g_P291_TexLoadState, 4 } };
	static const Region streams_o187[] = { { game::unk_FA94A8, 0x4000 }, { game::unk_E41E50, 0xC }, { game::unk_E41E98, 4 }, { game::unk_209FAB8, 0x40000 }, { game::g_CamScript_OtherTrack, 0x162B } };
	static const Region streams_d191[] = { { game::g191_PoolA, 0x18 }, { game::unk_209FAB8, 0x40000 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::g_BattleModel_ObjPacketCursor, 8 } };
	static const Region streams_g327[] = { { game::unk_E808D0, 0x10C50 }, { game::unk_209FAB8, 0x40000 }, { game::g327_PoolB, 0xC }, { game::g_CamScript_OtherTrack, 0x162B } };
	static const Region streams_o326[] = { { game::unk_EECC2C, 0x8004 }, { game::unk_E19668, 0xC }, { game::unk_E196B0, 4 }, { game::unk_209FAB8, 0x40000 }, { game::g_CamScript_OtherTrack, 0x162B } };
	static const Region streams_gfc[] = { { game::g_GfCinematic_MeshDepthScale, 4 }, { game::unk_209AB64, 0x10 }, { game::unk_C78BF0, 0x10 }, { game::GF_CINEMATIC_PARTY_HINT, 4 }, { game::unk_2798C40, 0x35C } };
	static const Region streams_ps[] = { { game::g_GfCinematic_MeshDepthScale, 4 }, { game::unk_209AB64, 0x10 }, { game::unk_C78BF0, 0x10 }, { game::GF_CINEMATIC_PARTY_HINT, 4 }, { game::unk_2798C40, 0x35C }, { game::unk_B8BA00, 8 } };
	static const Region streams_s302[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC } };
	static const Region streams_c318[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC } };
	static const Region streams_t324[] = { { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC }, { game::g_BattleModel_ObjPacketCursor, 8 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_c319[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC } };
	static const Region streams_a095[] = { { game::unk_2793DA4, 0xD4 }, { game::unk_153386C, 0x154 }, { game::unk_16A40EC, 0x1284 }, { game::unk_2795110, 0x68 }, { game::unk_19D1018, 0x23600 }, { game::g_BattleModel_ObjPacketCursor, 8 } };
	static const Region streams_a090[] = { { game::unk_15474EC, 0x530 }, { game::unk_2793DA4, 0xD4 }, { game::unk_2795960, 0x218 }, { game::g_BattleModel_ObjPacketCursor, 8 } };
	static const Region streams_a097[] = { { game::unk_152B94C, 0xD4 }, { game::unk_168356C, 0xC340 }, { game::unk_2793DA4, 0xD4 }, { game::g_BattleModel_ObjPacketCursor, 8 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::unk_19D1018, 0x23600 } };
	static const Region streams_a098[] = { { game::unk_152B2C4, 0xF4 }, { game::unk_167484C, 0xC340 }, { game::unk_2793DA4, 0xD4 }, { game::g_BattleModel_ObjPacketCursor, 8 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::unk_19D1018, 0x23600 } };
	static const Region streams_a099[] = { { game::unk_152AABC, 0x174 }, { game::unk_1665B28, 0x8120 }, { game::unk_2793DA4, 0xD4 }, { game::g_BattleModel_ObjPacketCursor, 8 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::unk_19D1018, 0x23600 }, { game::unk_2795113, 0x68 } };
	static const Region streams_a100[] = { { game::unk_1529B8C, 0x134 }, { game::unk_1656E04, 0xC340 }, { game::unk_2793DA4, 0xD4 }, { game::g_BattleModel_ObjPacketCursor, 8 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::unk_19D1018, 0x23600 } };
	static const Region streams_f002[] = { { game::MAG002_FIRE_slotFree, 0x1C }, { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_f142[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_f143[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_t003[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_t102[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_t105[] = { { game::unk_130E55C, 0x3040 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_i144[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_i103[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_i104[] = { { game::unk_1315774, 0x1900 }, { game::unk_13197C4, 0xC }, { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_b022[] = { { game::unk_16146FC, 0x100 }, { game::unk_1615CEC, 0x2D8 }, { game::unk_1617E44, 0x150 }, { game::unk_1619890, 0xE98 }, { game::unk_161F758, 0x810 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_q038[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_u149[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_h175[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::unk_D592C0, 0x30 } };
	static const Region streams_m148[] = { { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC }, { game::g_BattleModel_ObjPacketCursor, 8 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_p034[] = { { game::Effect_SharedCamMatrixSnapshot, 0x20 } };
	static const Region streams_d039[] = { { game::unk_2793DA4, 0xD4 }, { game::unk_15D1F18, 0x140 } };
	static const Region streams_c036[] = { { game::unk_15D9E0C, 0x590 }, { game::unk_15DBAE4, 0x590 }, { game::unk_2793DA4, 0xD4 } };
	static const Region streams_f124[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_s145[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_t146[] = { { game::unk_DD674C, 0xC }, { game::unk_DD6780, 4 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::g_BattleModel_ObjPacketCursor, 8 } };
	static const Region streams_d107[] = { { game::unk_12F9978, 0x20 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC } };
	static const Region streams_d109[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_a111[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_b117[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_b108[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_r147[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_e024[] = { { game::unk_2793DA4, 0xD4 }, { game::unk_17E48E4, 0x9C }, { game::unk_17E9DA8, 0x738 } };
	static const Region streams_p032[] = { { game::Effect_SharedCamMatrixSnapshot, 0x20 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_s033[] = { { game::unk_2793DA4, 0xD4 } };
	static const Region streams_l035[] = { { game::Effect_GlintPacketCursorPtr, 4 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_f027[] = { { game::unk_17DE100, 0x104 }, { game::unk_17E398C, 0xF48 }, { game::unk_2793DA4, 0xD4 } };
	static const Region streams_c028[] = { { game::unk_17D5F4C, 0xD4 }, { game::unk_17DCC48, 0x14A8 }, { game::unk_2793DA4, 0xD4 } };
	static const Region streams_c025[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_c001[] = { { game::Effect_GlintPacketCursorPtr, 4 }, { game::Effect_SharedCamMatrixSnapshot, 0x20 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_c004[] = { { game::unk_163FFD8, 0x25E }, { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_c021[] = { { game::unk_1627978, 0x27E }, { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_c040[] = { { game::unk_15CDDF4, 0x180 }, { game::unk_15CE1DC, 0x500 }, { game::NB_MESSAGE_TO_PRINT, 1 }, { game::TEXT_LAYER, 0xA4 }, { game::unk_1D2B558, 0x32 }, { game::unk_2795BD0, 4 }, { game::SCAN_ELEMENT_INDEX_LIST_DUP, 0x2C } };
	static const Region streams_e029[] = { { game::unk_17D27DC, 0x98 }, { game::unk_17D56AC, 0x890 }, { game::unk_2793DA4, 0xD4 } };
	static const Region streams_c026[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_r010[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_s011[] = { { game::unk_1365728, 0x9A0 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_s155[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_g168[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_u154[] = { { game::g_BattleModel_ObjPacketCursor, 8 }, { game::UW154_CapturePackets, 0x30 } };
	static const Region streams_s180[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_g169[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC } };
	static const Region streams_a126[] = { { game::Mag126_ScreenFadeChance, 4 } };
	static const Region streams_w153[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_m013[] = { { game::unk_163257C, 0x169C }, { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::unk_1D9770C, 2 } };
	static const Region streams_d232[] = { { game::g_GfCinematic_MeshDepthScale, 4 }, { game::unk_209AB64, 0x10 }, { game::unk_C78BF0, 0x10 }, { game::GF_CINEMATIC_PARTY_HINT, 4 }, { game::unk_2798C40, 0x478 }, { game::g_BattleModel_ObjPacketCursor, 8 } };
	static const Region streams_f234[] = { { game::g_GfCinematic_MeshDepthScale, 4 }, { game::unk_209AB64, 0x10 }, { game::unk_C78BF0, 0x10 }, { game::GF_CINEMATIC_PARTY_HINT, 4 }, { game::unk_2798C40, 0x478 }, { game::g_BattleModel_ObjPacketCursor, 8 } };
	static const Region streams_l030[] = { { game::unk_17CC72C, 0x60A0 }, { game::unk_2793DA4, 0xD4 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_d041[] = { { game::unk_15CAAB0, 0x2FE }, { game::unk_2793DA4, 0xD4 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_c190[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_m048[] = { { game::unk_15AE4FC, 0x9790 }, { game::unk_2793DA4, 0xD4 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_m008[] = { { game::unk_1636C94, 0x5BC }, { game::unk_1818B78, 0x12BC }, { game::unk_181CB14, 0x488 }, { game::unk_19EB018, 0x530C }, { game::unk_2793DA4, 0xD4 }, { game::unk_2795110, 0x68 }, { game::g_BattleModel_ObjPacketCursor, 8 } };
	static const Region streams_d009[] = { { game::unk_2793DA4, 0xD4 }, { game::unk_2795110, 0x68 }, { game::g_BattleModel_ObjPacketCursor, 8 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC } };
	static const Region streams_b012[] = { { game::unk_1362E44, 0x188 }, { game::unk_1363734, 0x188 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_d015[] = { { game::unk_17FC68C, 0x5944 }, { game::unk_2793DA4, 0xD4 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_d037[] = { { game::unk_15D62F4, 0x8A0 }, { game::unk_17C8420, 0x4300 }, { game::unk_2793DA4, 0xD4 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_d019[] = { { game::unk_17F37BC, 0x4750 }, { game::unk_2793DA4, 0xD4 }, { game::unk_2795110, 0x68 } };
	static const Region streams_d020[] = { { game::Effect_SharedCamMatrixSnapshot, 0x20 }, { game::unk_2795110, 0x68 }, { game::g_BattleModel_ObjPacketCursor, 8 } };
	static const Region streams_d023[] = { { game::unk_17EA4F0, 0x3D64 }, { game::unk_2793DA4, 0xD4 }, { game::unk_2795110, 0x68 } };
	static const Region streams_h031[] = { { game::unk_2793DA4, 0xD4 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_s042[] = { { game::unk_2793DA4, 0xD4 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_b043[] = { { game::unk_2793DA4, 0xD4 } };
	static const Region streams_j044[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_CAMERA_SHAKE_OFFSET_Y, 2 }, { game::unk_1DCD6E4, 0xC } };
	static const Region streams_s045[] = { { game::Effect_SharedCamMatrixSnapshot, 0x20 } };
	static const Region streams_s046[] = { { game::unk_15C0FE8, 0x1370 }, { game::unk_2793DA4, 0xD4 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_c047[] = { { game::unk_15B824C, 0x24 }, { game::unk_17B97A4, 0x1980 }, { game::unk_2793DA4, 0xD4 } };
	static const Region streams_r049[] = { { game::Effect_SharedCamMatrixSnapshot, 0x20 } };
	static const Region streams_g054[] = { { game::unk_2793DA4, 0xD4 }, { game::g_EveryonesGrudge_ActorData, 0x4C }, { game::unk_17AAF00, 0x80 }, { game::unk_17AB058, 0x80 } };
	static const Region streams_a055[] = { { game::unk_15A4170, 0x9C0 }, { game::Effect_SharedCamMatrixSnapshot, 0x20 } };
	static const Region streams_r060[] = { { game::unk_1797CC0, 0x9034 }, { game::unk_2793DA4, 0xD4 } };
	static const Region streams_h063[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_t064[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_a065[] = { { game::unk_133ABC8, 0x158 } };
	static const Region streams_g066[] = { { game::g066_Pool4Ptr, 8 }, { game::g_CamScript_OtherTrack, 0x162B }, { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::g_BattleModel_ObjPacketCursor, 8 } };
	static const Region streams_s068[] = { { game::unk_1591C4C, 0x180 }, { game::unk_1592034, 0x500 }, { game::NB_MESSAGE_TO_PRINT, 1 }, { game::TEXT_LAYER, 0xA4 }, { game::unk_1D2B558, 0x32 }, { game::unk_2795BD0, 4 }, { game::SCAN_TEXT_LINES, 0x30 } };
	static const Region streams_g075[] = { { game::g_GfCinematic_MeshDepthScale, 4 }, { game::unk_209AB64, 0x10 }, { game::unk_C78BF0, 0x10 }, { game::GF_CINEMATIC_PARTY_HINT, 4 }, { game::unk_2798C40, 0x518 } };
	static const Region streams_p070[] = { { game::g_BattleModel_ObjPacketCursor, 8 } };
	static const Region streams_u076[] = { { game::source, 0x19 }, { game::CURRENT_BS_SHADOW_PALETTE, 4 }, { game::g_CamScript_OtherTrack, 0x162B } };
	static const Region streams_m078[] = { { game::unk_2793DA4, 0xD4 }, { game::unk_176E194, 0x94 }, { game::unk_1771814, 0x80 }, { game::unk_177196C, 0x80 }, { game::unk_1771AC4, 0x80 }, { game::unk_1771C1C, 0x80 }, { game::unk_1771D74, 0x80 }, { game::unk_1771ECC, 0x80 } };
	static const Region streams_f077[] = { { game::g_GfCinematic_MeshDepthScale, 4 }, { game::unk_209AB64, 0x10 }, { game::unk_C78BF0, 0x10 }, { game::GF_CINEMATIC_PARTY_HINT, 4 }, { game::unk_2798C40, 0x478 } };
	static const Region streams_u079[] = { { game::unk_2793DA4, 0xD4 }, { game::unk_1581F34, 0x44 }, { game::unk_1762E40, 0xB214 }, { game::unk_19F1018, 0x5034 }, { game::unk_1D99AB0, 0x12 } };
	static const Region streams_u080[] = { { game::unk_2793DA4, 0xD4 }, { game::unk_1749C64, 0xAEB8 }, { game::unk_175AC20, 0x3BD4 } };
	static const Region streams_u081[] = { { game::unk_2793DA4, 0xD4 }, { game::unk_1744A1C, 0x1644 }, { game::unk_17492B4, 0x890 }, { game::unk_2795110, 0x68 } };
	static const Region streams_u082[] = { { game::unk_2793DA4, 0xD4 }, { game::unk_1741028, 0x93C }, { game::unk_174417C, 0x890 }, { game::unk_2795110, 0x68 } };
	static const Region streams_u083[] = { { game::unk_2793DA4, 0xD4 }, { game::MAG_083_ActorSequence, 0xCC }, { game::unk_172D278, 0x2A4 }, { game::unk_1737B58, 0x10A0 }, { game::unk_15786F4, 0x518 }, { game::unk_209FAB8, 0x7D00 }, { game::g_BattleModel_ObjPacketCursor, 8 } };
	static const Region streams_b113[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC } };
	static const Region streams_g128[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC } };
	static const Region streams_b129[] = { { game::unk_12272E4, 0x580 }, { game::unk_1228F0C, 0x2C0 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC } };
	static const Region streams_b131[] = { { game::BadBreath_Mesh1, 0x540 }, { game::BadBreath_Mesh2, 0x540 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC } };
	static const Region streams_d132[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC } };
	static const Region streams_h133[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC } };
	static const Region streams_b134[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC } };
	static const Region streams_r135[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC } };
	static const Region streams_o136[] = { { game::Mag136_OilMesh, 0x368 }, { game::unk_11F81E0, 2 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC } };
	static const Region streams_o137[] = { { game::MAG_137_MeshScaleMatrix, 0x12 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC } };
	static const Region streams_s138[] = { { game::unk_11DF764, 0x368 }, { game::unk_11E3148, 2 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC } };
	static const Region streams_b179[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_x276[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_f277[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC } };
	static const Region streams_p315[] = { { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC }, { game::g_BattleModel_ObjPacketCursor, 8 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_g289[] = { { game::mag289_TubeMeshA, 0x528 }, { game::mag289_TubeMeshB, 0x528 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC }, { game::g_BattleModel_ObjPacketCursor, 8 } };
	static const Region streams_s281[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC }, { game::g_BattleModel_ObjPacketCursor, 8 } };
	static const Region streams_b304[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC }, { game::g_BattleModel_ObjPacketCursor, 8 } };
	static const Region streams_u227[] = { { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC }, { game::g_BattleModel_ObjPacketCursor, 8 } };
	static const Region streams_g311[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC }, { game::g_BattleModel_ObjPacketCursor, 8 } };
	static const Region streams_r316[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_a314[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::unk_1DCD6E4, 0xC } };
	static const Region streams_m308[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::unk_1DCD6E4, 0xC } };
	static const Region streams_d320[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC }, { game::unk_FF12A8, 2 } };
	static const Region streams_r303[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC }, { game::g_BattleModel_ObjPacketCursor, 8 } };
	static const Region streams_s209[] = { { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC }, { game::g_BattleModel_ObjPacketCursor, 8 }, { game::unk_209FAB8, 0x40000 }, { game::BATTLE_EFFECT_BONE_OFFSET, 0x20 }, { game::initialize_psxvram_is_done, 4 }, { game::CURRENT_BS_MODEL_PALETTE, 4 }, { game::CURRENT_BS_SHADOW_PALETTE, 4 } };
	static const Region streams_z167[] = { { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC }, { game::g_BattleModel_ObjPacketCursor, 8 } };
	static const Region streams_g218[] = { { game::unk_F1DD54, 0xBCC }, { game::unk_209FAB8, 0x40000 }, { game::g_CamScript_OtherTrack, 0x162B } };
	static const Region streams_d251[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC }, { game::g_BattleModel_ObjPacketCursor, 8 }, { game::unk_1434430, 0x3D58 } };
	static const Region streams_b210[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC }, { game::g_BattleModel_ObjPacketCursor, 8 } };
	static const Region streams_i173[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_g301[] = { { game::MAG_301_GargantuaBurrow_SpawnStream, 4 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC }, { game::g_BattleModel_ObjPacketCursor, 8 } };
	static const Region streams_b298[] = { { game::unk_1056560, 0x31C }, { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC }, { game::g_BattleModel_ObjPacketCursor, 8 } };
	static const Region streams_t290[] = { { game::unk_106E648, 0x24C }, { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC }, { game::g_BattleModel_ObjPacketCursor, 8 } };
	static const Region streams_m207[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC }, { game::g_BattleModel_ObjPacketCursor, 8 }, { game::unk_1CA8850, 4 }, { game::unk_1CA89AC, 4 }, { game::unk_1CA89B4, 4 }, { game::initialize_psxvram_is_done, 4 } };
	static const Region streams_g214[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC }, { game::g_BattleModel_ObjPacketCursor, 8 }, { game::unk_1CA8850, 4 }, { game::unk_1CA89AC, 4 }, { game::unk_1CA89B4, 4 }, { game::initialize_psxvram_is_done, 4 } };
	static const Region streams_s176[] = { { game::unk_D4516C, 0xC }, { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_b157[] = { { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC }, { game::g_BattleModel_ObjPacketCursor, 8 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_m158[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_s139[] = { { game::unk_11D7900, 0x12F0 } };
	static const Region streams_i050[] = { { game::Effect_GlintPacketCursorPtr, 4 }, { game::Effect_SharedCamMatrixSnapshot, 0x20 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_i051[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_i053[] = { { game::unk_15A78B8, 0x168 }, { game::unk_15A7DE0, 0x48 }, { game::unk_15A7EB0, 0x48 }, { game::unk_2793DA4, 0xD4 } };
	static const Region streams_i058[] = { { game::unk_159B100, 0x280 }, { game::unk_159BA38, 0x20 }, { game::unk_159BA98, 0x48 }, { game::unk_159BB68, 0x280 }, { game::unk_159C4A0, 0x20 }, { game::unk_2793DA4, 0xD4 } };
	static const Region streams_i059[] = { { game::unk_1597B40, 0x280 }, { game::unk_1598478, 0x20 }, { game::unk_15984D8, 0x48 }, { game::unk_15985A8, 0x280 }, { game::unk_1598EE0, 0x20 }, { game::unk_2793DA4, 0xD4 } };
	static const Region streams_i052[] = { { game::MAG_052_XPOTION_ActorData, 0xD4 }, { game::unk_17B1DE4, 0x14A8 }, { game::unk_2793DA4, 0xD4 } };
	static const Region streams_i005[] = { { game::Effect_GlintPacketCursorPtr, 4 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_i007[] = { { game::unk_163B1E0, 0x578 }, { game::unk_163C6BC, 0x48 }, { game::unk_163C78C, 0x48 }, { game::unk_2793DA4, 0xD4 } };
	static const Region streams_i067[] = { { game::unk_2793DA4, 0xD4 }, { game::unk_178F3F4, 0x9C }, { game::unk_17948B8, 0x738 } };
	static const Region streams_f162[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::unk_DA2654, 0x1110 } };
	static const Region streams_b164[] = { { game::g_CamScript_OtherTrack, 0x162B }, { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_m166[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_m312[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_d174[] = { { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC }, { game::g_BattleModel_ObjPacketCursor, 8 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_s213[] = { { game::unk_117ADE0, 0x9A0 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_e217[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC }, { game::g_BattleModel_ObjPacketCursor, 8 } };
	static const Region streams_l345[] = { {game::unk_2793DA4,0xD4}, {game::g_BattleModel_ObjPacketCursor,8}, {game::unk_1525774,0x548}, {game::unk_1643698,0x6FD0} };
	static const Region streams_g150[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_k181[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_r313[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_l165[] = { { game::g_CamScript_OtherTrack, 0x162B }, { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_c182[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_t057[] = { { game::unk_2793DA4, 0xD4 }, { game::unk_17A3DD0, 0xA0 }, { game::unk_17A9A64, 0x890 } };
	static const Region streams_a056[] = { { game::unk_2793DA4, 0xD4 }, { game::unk_159F510, 0x140 } };
	static const Region streams_r016[] = { { game::MAG_016_RECOVER_ActorData, 0xA4 }, { game::unk_17FBC94, 0x9E8 }, { game::unk_2793DA4, 0xD4 } };
	static const Region streams_r061[] = { { game::MAG_061_REVIVE_ActorData, 0x90 }, { game::unk_1797578, 0x738 }, { game::unk_2793DA4, 0xD4 } };
	static const Region streams_m273[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC } };
	static const Region streams_d062[] = { { game::Effect_SharedCamMatrixSnapshot, 0x20 } };
	static const Region streams_q184[] = { { game::g_BattleModel_ObjPacketCursor, 8 }, { game::MAG_184_ULTRA_WAVES_QUISTIS_CapturePackets, 0x30 } };
	static const Region streams_q074[] = { {game::unk_2793DA4,0xD4}, {game::g_BattleModel_ObjPacketCursor,8}, {game::unk_158235C,0x548}, {game::MAG_074_LVDEATH_QUISTIS_AuraActorData,0x6FD0} };
	static const Region streams_q073[] = { {game::unk_2793DA4,0xD4}, {game::MAG_073_MIGHTY_GUARD_QUISTIS_ActorData,0x94}, {game::unk_1788BFC,0x80}, {game::unk_1788D54,0x80}, {game::unk_1788EAC,0x80}, {game::unk_1789004,0x80}, {game::unk_178915C,0x80}, {game::unk_17892B4,0x80} };
	static const Region streams_q072[] = { { game::unk_158A4C4, 0x9C0 }, { game::Effect_SharedCamMatrixSnapshot, 0x20 } };
	static const Region streams_q306[] = { { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC }, { game::g_BattleModel_ObjPacketCursor, 8 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_q307[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_q305[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_q071[] = { { game::MAG_071_LASER_EYE_QUISTIS_ActorData, 0x60A0 }, { game::unk_2793DA4, 0xD4 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_q317[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC } };
	static const Region streams_q300[] = { { game::MAG_300_BAD_BREATH_QUISTIS_Mesh1, 0x530 }, { game::MAG_300_BAD_BREATH_QUISTIS_Mesh2, 0x530 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC } };
	static const Region streams_q299[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC }, { game::g_BattleModel_ObjPacketCursor, 8 } };
	static const Region streams_q275[] = { { game::MAG_275_ELECTROCUTE_QUISTIS_FadeChance, 4 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC } };
	static const Region streams_q260[] = { { game::g_BattleModel_ObjPacketCursor, 8 } };
	static const Region streams_s280[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC } };
	static const Region streams_s288[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC } };
	static const Region streams_z274[] = { { game::unk_10E9538, 0x268 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC }, { game::g_BattleModel_ObjPacketCursor, 8 } };
	static const Region streams_s189[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC }, { game::g_BattleModel_ObjPacketCursor, 8 }, { game::unk_D1B490, 0x18 } };
	static const Region streams_s085[] = { { game::unk_2793DA4, 0xD4 } };
	static const Region streams_r084[] = { { game::unk_1572BC8, 0x430 }, { game::unk_2793DA4, 0xD4 }, { game::g_BattleModel_ObjPacketCursor, 8 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC } };
	static const Region streams_z112[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC } };
	static const Region streams_z212[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC }, { game::g_BattleModel_ObjPacketCursor, 8 } };
	static const Region streams_r093[] = { { game::unk_1538B58, 0x670 }, { game::unk_16B62B8, 0x90 }, { game::unk_16B7E90, 0x5E0 }, { game::unk_2793DA4, 0xD4 }, { game::g_BattleModel_ObjPacketCursor, 8 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC } };
	static const Region streams_r094[] = { { game::unk_153409C, 0x670 }, { game::unk_16AAD80, 0x90 }, { game::unk_16AD238, 0x738 }, { game::unk_2793DA4, 0xD4 }, { game::g_BattleModel_ObjPacketCursor, 8 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC } };
	static const Region streams_z215[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC }, { game::g_BattleModel_ObjPacketCursor, 8 } };
	static const Region streams_r091[] = { { game::unk_15438A0, 0x670 }, { game::unk_16CB3FC, 0x90 }, { game::unk_16CBFAC, 0x330 }, { game::unk_2793DA4, 0xD4 }, { game::g_BattleModel_ObjPacketCursor, 8 }, { game::unk_2795110, 0x68 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC } };
	static const Region streams_z216[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC }, { game::g_BattleModel_ObjPacketCursor, 8 } };
	static const Region streams_r092[] = { { game::unk_153D5BC, 0x670 }, { game::unk_2793DA4, 0xD4 }, { game::g_BattleModel_ObjPacketCursor, 8 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC } };
	static const Region streams_r088[] = { { game::unk_154F93C, 0x670 }, { game::unk_155565C, 0x670 }, { game::unk_2793DA4, 0xD4 }, { game::g_BattleModel_ObjPacketCursor, 8 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC } };
	static const Region streams_i188[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC }, { game::g_BattleModel_ObjPacketCursor, 8 } };
	static const Region streams_i196[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC }, { game::g_BattleModel_ObjPacketCursor, 8 } };
	static const Region streams_i192[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC }, { game::g_BattleModel_ObjPacketCursor, 8 } };
	static const Region streams_i194[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC }, { game::g_BattleModel_ObjPacketCursor, 8 } };
	static const Region streams_r086[] = { { game::MAG_086_ANGELO_CANNON_AngeloPoseBlock, 0x670 }, { game::unk_2793DA4, 0xD4 }, { game::g_BattleModel_ObjPacketCursor, 8 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC } };
	static const Region streams_i195[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC }, { game::g_BattleModel_ObjPacketCursor, 8 } };
	static const Region streams_i193[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC }, { game::g_BattleModel_ObjPacketCursor, 8 } };
	static const Region streams_i197[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC }, { game::g_BattleModel_ObjPacketCursor, 8 } };
	static const Region streams_r087[] = { { game::MAG_087_ANGELO_STRIKE_AngeloPoseBlock, 0x670 }, { game::unk_2793DA4, 0xD4 }, { game::g_BattleModel_ObjPacketCursor, 8 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC } };
	static const Region streams_i198[] = { { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC }, { game::g_BattleModel_ObjPacketCursor, 8 } };
	static const Region streams_s200[] = { { game::g_CamScript_OtherTrack, 0x162B }, { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_r089[] = { { game::unk_1491414, 0x670 }, { game::MAG_089_StarGroups, 0x80 }, { game::g_BattleModel_ObjPacketCursor, 8 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::BATTLE_FILE_LOADING_STATE, 0x80 }, { game::unk_B8B9E4, 0xC } };
	static const Region streams_e017[] = { { game::CURRENT_BS_SHADOW_PALETTE, 4 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_b110[] = { { game::unk_12E0658, 0x9A0 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_r161[] = { { game::MAG161_AnimBindBlock, 0x30 }, { game::BATTLE_CAMERA_SHAKE_OFFSET_Y, 2 } };
	static const Region streams_r159[] = { { game::MAG159_AnimBindBlock, 0x30 } };
	static const Region streams_r141[] = { { game::MAG141_AnimBindBlock, 0x20 } };
	static const Region streams_r259[] = { { game::MAG259_AnimBindBlock, 0x20 } };
	static const Region streams_r332[] = { { game::MAG332_AnimBindBlock, 0x20 } };
	static const Region streams_r333[] = { { game::MAG333_AnimBindBlock, 0x20 } };
	static const Region streams_r334[] = { { game::unk_24FEAD8, 0x30 } };
	static const Region streams_r335[] = { { game::unk_24FEAA8, 0x30 } };
	static const Region streams_r336[] = { { game::unk_24FEA78, 0x30 }, { game::BATTLE_CAMERA_SHAKE_OFFSET_Y, 2 } };
	static const Region streams_r337[] = { { game::unk_24FEA48, 0x30 }, { game::BATTLE_CAMERA_SHAKE_OFFSET_Y, 2 } };
	static const Region streams_r339[] = { { game::unk_24FEA08, 0x40 } };
	static const Region streams_r340[] = { { game::unk_24FE9D8, 0x30 } };
	static const Region streams_r341[] = { { game::unk_24FE9A8, 0x30 }, { game::BATTLE_CAMERA_SHAKE_OFFSET_Y, 2 } };
	static const Region streams_r342[] = { { game::unk_24FE978, 0x30 } };
	static const Region streams_r343[] = { { game::unk_24FE948, 0x30 } };
	static const Region streams_d014[] = { { game::unk_2793DA4, 0xD4 }, { game::g_BattleModel_ObjPacketCursor, 8 }, { game::unk_162C97C, 0x548 }, { game::unk_1801FD0, 0x6FD0 } };
	static const Region streams_a096[] = { { game::unk_2793DA4, 0xD4 }, { game::unk_152BBBC, 0x60E }, { game::unk_169227C, 0xA4 }, { game::unk_1696140, 0xBCA4 }, { game::g_BattleModel_ObjPacketCursor, 8 } };
	static const Region streams_s185[] = { { game::g185_GlowRingNode, 4 }, { game::unk_209FAB8, 0x40000 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 }, { game::g_CamScript_OtherTrack, 0x162B }, { game::g_BattleModel_ObjPacketCursor, 8 } };
	static const Region streams_p140[] = { { game::unk_11B539C, 0xB17 }, { game::g_BattleModel_ObjPacketCursor, 8 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_c278[] = { { game::unk_10B6FB4, 0x5A9 }, { game::g_BattleModel_ObjPacketCursor, 8 }, { game::SOME_CAMERA_Z_FACTOR, 0x20 } };
	static const Region streams_m338[] = { { game::unk_136BB50, 0x549 }, { game::g_BattleModel_ObjPacketCursor, 8 } };
	static const Region streams_r160[] = { { game::MAG_160_RENZOKUKEN_AnimBindBlock, 0x20 } };

	const Module modules[] = {
		{ 116, game::GF116_QUEZACOTL_timBuffer, game::unk_25217D0, {}, 0, true, "Quezacotl", streams_q116, 2 },
		{ 325, game::unk_250517C, game::MAG_323_Metsu_TextureFile, {}, 0, false, "Diablos" },
		{ 278, game::unk_2508110, game::unk_25081FC, {}, 0, false, "Carbuncle", streams_c278, 3 },
		{ 291, game::g_P291_DebugPause, game::MAG_287_MegaPulseCannon_CastContext, {}, 0, false, "Pandemona", streams_p291, 1 },
		{ 140, game::g140_EmberPool, game::MAG_139_CasterEntity, {}, 0, false, "Phoenix", streams_p140, 3 },
		{ 338, game::unk_25561C8, game::unk_2556254, {}, 0, false, "Moomba", streams_m338, 2 },
		{ 69,  game::g069_SparkHint, game::unk_2556F98, {}, 0, false, "Griever" },
		{ 185, game::unk_22BC128, game::MAG_184_ULTRA_WAVES_QUISTIS_TargetSlot, {}, 0, false, "Shiva", streams_s185, 5 },
		{ 199, game::unk_2259950, game::unk_225A8E4, game::unk_CF3A68, 4, false, "Cactuar", streams_c199, 2 },
		{ 187, game::unk_24FD458, game::unk_24FE910, {}, 0, false, "Odin", streams_o187, 5 },
		{ 326, game::MAG_326_HistA, game::unk_24F2308, {}, 0, false, "Odin (reverse)", streams_o326, 5 },
		{ 191, game::unk_24FBD68, game::unk_24FD458, {}, 0, false, "Doomtrain", streams_d191, 4 },
		{ 327, game::g327_VoiceSlot, game::MAG_324_TonberryKingDeath_KeptMatrix, {}, 0, false, "Gilgamesh (Zantetsuken)", streams_g327, 4 },
		{ 328, game::g327_VoiceSlot, game::MAG_324_TonberryKingDeath_KeptMatrix, {}, 0, false, "Gilgamesh (Masamune)", streams_g327, 4 },
		{ 329, game::g327_VoiceSlot, game::MAG_324_TonberryKingDeath_KeptMatrix, {}, 0, false, "Gilgamesh (Excalibur)", streams_g327, 4 },
		{ 330, game::g327_VoiceSlot, game::MAG_324_TonberryKingDeath_KeptMatrix, {}, 0, false, "Gilgamesh (Excalipoor)", streams_g327, 4 },
		{ 201, game::unk_2796E00, game::unk_2798C40, {}, 0, false, "Ifrit", streams_gfc, 5 },
		{ 6,   game::unk_2796EF8, game::unk_2798C40, {}, 0, false, "Leviathan", streams_gfc, 5 },
		{ 202, game::unk_2796DE0, game::unk_2798C40, {}, 0, false, "Bahamut", streams_gfc, 5 },
		{ 203, game::unk_2796DA8, game::unk_2798C40, {}, 0, false, "Cerberus", streams_gfc, 5 },
		{ 204, game::unk_2796D70, game::unk_2798C40, {}, 0, false, "Alexander", streams_gfc, 5 },
		{ 205, game::unk_2796D30, game::unk_2798C40, {}, 0, false, "Brothers", streams_gfc, 5 },
		{ 206, game::unk_2796CF8, game::unk_2798C40, {}, 0, false, "Eden", streams_gfc, 5 },
		{ 95,  game::unk_257F8A0, game::unk_258FCF4, {}, 0, false, "Siren", streams_a095, 6 },
		{ 96,  game::unk_257B740, game::unk_257F8A0, {}, 0, false, "MiniMog", streams_a096, 5 },
		{ 90,  game::MAG_090_PacketCursor, game::unk_25A4E00, {}, 0, false, "Tonberry", streams_a090, 4 },
		{ 97,  game::unk_2575268, game::unk_257B740, {}, 0, false, "Boko ChocoFire", streams_a097, 6 },
		{ 98,  game::unk_256E358, game::unk_2575268, {}, 0, false, "Boko ChocoFlare", streams_a098, 6 },
		{ 99,  game::unk_2565A30, game::unk_256E358, {}, 0, false, "Boko ChocoMeteor", streams_a099, 7 },
		{ 100, game::unk_255EAC8, game::unk_2565A30, {}, 0, false, "Boko ChocoBocle", streams_a100, 6 },
		{ 2,   game::unk_24CA818, game::MAG_344_Griever368_FirstTargetSlot, {}, 0, false, "Fire", streams_f002, 2 },
		{ 142, game::unk_24BFB48, game::unk_24C10B4, {}, 0, false, "Fira", streams_f142, 1 },
		{ 143, game::unk_24BECC8, game::unk_24BFB40, {}, 0, false, "Firaga", streams_f143, 1 },
		{ 3,   game::unk_2557000, game::g_Effect_SharedLcgSeed, {}, 0, false, "Thunder", streams_t003, 1 },
		{ 102, game::MAG102_THUNDARA_boltPool, game::MAG_101_TexFile, {}, 0, false, "Thundara", streams_t102, 1 },
		{ 105, game::unk_2543CF0, game::MAG_104_BLIZZAGA_FlashLevel, {}, 0, false, "Thundaga", streams_t105, 2 },
		{ 144, game::MAG_144_BLIZZARD_FirstTargetSlot, game::unk_24BECC4, {}, 0, false, "Blizzard", streams_i144, 1 },
		{ 103, game::MAG_103_BLIZZARA_File, game::MAG102_THUNDARA_boltPool, {}, 0, false, "Blizzara", streams_i103, 1 },
		{ 104, game::MAG_104_BLIZZAGA_FlashLevel, game::MAG_103_BLIZZARA_File, {}, 0, false, "Blizzaga", streams_i104, 3 },
		{ 22,  game::MAG_022_FlashQueue, game::unk_2714F24, {}, 0, false, "Bio", streams_b022, 6 },
		{ 38,  game::MAG_038_PacketCursor, game::unk_2792E60, {}, 0, false, "Quake", streams_q038, 1 },
		{ 149, game::g_Ultima_RingScale, game::unk_2464BA8, {}, 0, false, "Ultima", streams_u149, 1 },
		{ 175, game::MAG_175_FadeSide, game::MAG_174_Degenerator_TargetSlot, game::g_BattleModel_ObjPacketCursor, 4, false, "Holy", streams_h175, 2 },
		{ 148, game::unk_2464BA8, game::g_Regen_FirstTargetSlot, {}, 0, false, "Meltdown", streams_m148, 4 },
		{ 34,  game::MAG_034_FilePtr, game::MAG_033_FilePtr, {}, 0, false, "Pain", streams_p034, 1 },
		{ 39,  game::MAG_039_ScreenFlashQueue, game::unk_269E764, {}, 0, false, "Drain", streams_d039, 2 },
		{ 14,  game::MAG_014_FilePtr, game::unk_27428A0, {}, 0, false, "Death", streams_d014, 4 },
		{ 36,  game::MAG_036_ScreenOverlayQueue, game::unk_26A9BC4, {}, 0, false, "Confuse", streams_c036, 3 },
		{ 124, game::g_Flare_FlashLevel, game::unk_2521448, {}, 0, false, "Flare", streams_f124, 1 },
		{ 145, game::unk_24AA630, game::MAG_144_BLIZZARD_FirstTargetSlot, game::g_BattleModel_ObjPacketCursor, 8, false, "Sleep", streams_s145, 1 },
		{ 146, game::MAG_146_Grid, game::unk_24AA62C, {}, 0, false, "Tornado", streams_t146, 4 },
		{ 107, game::unk_2521B30, game::unk_2521BB8, {}, 0, false, "Demi", streams_d107, 4 },
		{ 109, game::unk_2521A68, game::unk_2521AC0, {}, 0, false, "Dispel", streams_d109, 1 },
		{ 111, game::unk_2521970, game::MAG_110_g_TimFile, {}, 0, false, "Aura", streams_a111, 1 },
		{ 118, game::unk_25215D8, game::unk_2521650, {}, 0, false, "Aero", nullptr, 0 },
		{ 123, game::unk_2521448, game::unk_25214B0, {}, 0, false, "Slow", nullptr, 0 },
		{ 119, game::unk_2521590, game::unk_25215D8, {}, 0, false, "Stop", nullptr, 0 },
		{ 125, game::unk_2521368, game::g_Flare_FlashLevel, {}, 0, false, "Haste", nullptr, 0 },
		{ 115, game::unk_25217D0, game::unk_2521818, {}, 0, false, "Float", nullptr, 0 },
		{ 114, game::unk_2521818, game::unk_252186C, {}, 0, false, "Zombie", nullptr, 0 },
		{ 117, game::unk_2521650, game::GF116_QUEZACOTL_timBuffer, {}, 0, false, "Break", streams_b117, 1 },
		{ 108, game::unk_2521AC0, game::unk_2521B30, {}, 0, false, "Berserk", streams_b108, 1 },
		{ 122, game::unk_25214B0, game::g_Mag121_FlashLevel, {}, 0, false, "Silence", nullptr, 0 },
		{ 121, game::g_Mag121_FlashLevel, game::g_PetrifyStare_FlashLevel, {}, 0, false, "Blind", nullptr, 0 },
		{ 147, game::g_Regen_FirstTargetSlot, game::unk_2491BBC, {}, 0, false, "Regen", streams_r147, 1 },
		{ 24, game::MAG024_ESUNA_timBuffer, game::unk_27011B4, {}, 0, false, "Esuna", streams_e024, 3 },
		{ 32, game::g_Protect_FlashQueue, game::MAG_031_ScreenFlashQueue, {}, 0, false, "Protect", streams_p032, 2 },
		{ 33, game::MAG_033_FilePtr, game::unk_26C30EC, {}, 0, false, "Shell", streams_s033, 1 },
		{ 35, game::MAG_035_FilePtr, game::unk_26BA9C8, {}, 0, false, "Life", streams_l035, 2 },
		{ 27, game::MAG_027_FilePtr, game::unk_26D904C, {}, 0, false, "Full-life", streams_f027, 3 },
		{ 28, game::MAG_028_ScreenFlashQueue, game::unk_26D8304, {}, 0, false, "Curaga", streams_c028, 3 },
		{ 106, game::unk_2521BB8, game::unk_2543CF0, {}, 0, false, "Reflect", nullptr, 0 },
		{ 25, game::MAG025_CURA_timBuffer, game::MAG024_ESUNA_timBuffer, {}, 0, false, "Cura", streams_c025, 1 },
		{ 1, game::MAG001_CURE_timBuffer, game::MAG_038_PacketCursor, {}, 0, false, "Cure", streams_c001, 3 },
		{ 4, game::g_Double_TexFile, game::unk_277AEA8, {}, 0, false, "Double", streams_c004, 2 },
		{ 21, game::g_Triple_TexFile, game::unk_2721580, {}, 0, false, "Triple", streams_c021, 2 },
		{ 40, game::unk_269A160, game::unk_269D93C, {}, 0, false, "Scan", streams_c040, 7 },
		{ 222, game::g_GF222Water_TaskQueue, game::unk_2798C40, {}, 0, false, "Water", streams_gfc, 5 },
		{ 223, game::g_GF223Meteor_TaskQueue, game::unk_2798C40, {}, 0, false, "Meteor", streams_gfc, 5 },
		{ 120, game::g_PetrifyStare_FlashLevel, game::unk_2521590, {}, 0, false, "Petrify Stare", nullptr, 0 },
		{ 152, game::g_mag152_TargetSlot, game::unk_244D670, {}, 0, false, "Ultrasonic Waves", nullptr, 0 },
		{ 127, game::g_Mag127_FlashLevel, game::g_Mag126_FlashLevel, {}, 0, false, "Petrify Stare (Cockatrice)", nullptr, 0 },
		{ 29, game::MAG_029_FilePtr, game::unk_26D74F4, {}, 0, false, "Wind Blast", streams_e029, 3 },
		{ 235, game::unk_27969B0, game::unk_2798C40, {}, 0, false, "Breath", streams_gfc, 5 },
		{ 231, game::unk_2796A90, game::unk_2798C40, {}, 0, false, "Disease Breath", streams_gfc, 5 },
		{ 233, game::g_GF233Earthquake_TaskQueue, game::unk_2798C40, {}, 0, false, "Earthquake", streams_gfc, 5 },
		{ 26, game::MAG_026_FlashQueue, game::unk_26E58F0, {}, 0, false, "Clash", streams_c026, 1 },
		{ 10, game::unk_24C91D8, game::unk_24CA810, {}, 0, false, "Ray-Bomb", streams_r010, 1 },
		{ 11, game::g_StormBreath_Caster, game::unk_25561C8, {}, 0, false, "Storm Breath", streams_s011, 2 },
		{ 155, game::unk_23F07F0, game::unk_2404234, {}, 0, false, "Sand Storm", streams_s155, 1 },
		{ 168, game::unk_23B3C10, game::unk_23BD350, {}, 0, false, "Sleeping Gas", streams_g168, 1 },
		{ 154, game::g_UW154_TargetSlot, game::g_Web153_SplinePointsA, {}, 0, false, "Ultra Waves", streams_u154, 2 },
		{ 180, game::g_Suicide_TargetSlot, game::unk_2309554, {}, 0, false, "Suicide", streams_s180, 1 },
		{ 169, game::g_Mag169_TargetSlot, game::unk_23B3C10, game::g_BattleModel_ObjPacketCursor, 8, false, "Gastric Juice", streams_g169, 3 },
		{ 126, game::g_Mag126_FlashLevel, game::unk_2521368, {}, 0, false, "Electric Discharge", streams_a126, 1 },
		{ 153, game::g_Web153_SplinePointsA, game::g_mag152_TargetSlot, {}, 0, false, "Sticky Web", streams_w153, 1 },
		{ 13, game::MAG_013_FilePtr, game::unk_274EFE4, {}, 0, false, "Dark Mist", streams_m013, 3 },
		{ 232, game::GF_232BreathOfDeath_Queue, game::unk_2798C40, {}, 0, false, "Breath of Death", streams_d232, 6 },
		{ 234, game::unk_27969E8, game::unk_2798C40, {}, 0, false, "Fart", streams_f234, 6 },
		{ 30, game::MAG_030_FilePtr, game::unk_26D6A3C, {}, 0, false, "Counter Laser-Eye", streams_l030, 3 },
		{ 41, game::g_Dribble_File, game::unk_2698AB8, {}, 0, false, "Dribble", streams_d041, 3 },
		{ 190, game::g_ChainGun_TargetSlot, game::unk_2293F44, {}, 0, false, "Chain Gun", streams_c190, 1 },
		{ 48, game::MAG_048_ScreenFlashQueue, game::unk_26495EC, {}, 0, false, "Magma Breath", streams_m048, 3 },
		{ 8, game::MAG_008_FilePtr, game::MAG_007_MEGAPHOENIX_FileData, {}, 0, false, "Doom", streams_m008, 7 },
		{ 9, game::MAG_009_DoomActivation_TextureFile, game::unk_27584F8, {}, 0, false, "Doom Activation (a Doom countdown at 0)", streams_d009, 6 },
		{ 12, game::unk_2545188, game::g_StormBreath_Caster, {}, 0, false, "Blade Shot", streams_b012, 3 },
		{ 15, game::MAG_015_FilePtr, game::unk_273AE7C, {}, 0, false, "Draw", streams_d015, 3 },
		{ 37, game::MAG_037_FilePtr, game::unk_26A8DB4, {}, 0, false, "Drink Magic", streams_d037, 4 },
		{ 19, game::g_RaldoThrow_File, game::unk_2730498, {}, 0, false, "Raldo Throw", streams_d019, 3 },
		{ 20, game::MAG_020_FilePtr, game::g_RaldoThrow_File, {}, 0, false, "NORG Pod opening", streams_d020, 3 },
		{ 23, game::g_PsychoBlast_FilePtr, game::MAG_022_FlashQueue, {}, 0, false, "Psycho Blast", streams_d023, 3 },
		{ 31, game::MAG_031_ScreenFlashQueue, game::unk_26D33E4, {}, 0, false, "Heartbreak", streams_h031, 2 },
		{ 42, game::MAG_042_FilePtr, game::unk_267B024, {}, 0, false, "Shoot", streams_s042, 2 },
		{ 43, game::g_MeltingBubble_File, game::unk_265622C, {}, 0, false, "Melting Bubble", streams_b043, 1 },
		{ 44, game::g_Junk_TexFile, game::unk_2545184, {}, 0, false, "Junk", streams_j044, 3 },
		{ 45, game::g_Stare_File, game::g_MeltingBubble_File, {}, 0, false, "Stare", streams_s045, 1 },
		{ 46, game::g_Sigh_FilePtr, game::unk_26527B4, {}, 0, false, "Sigh", streams_s046, 3 },
		{ 47, game::MAG_047_FilePtr, game::g_Sigh_FilePtr, {}, 0, false, "Curse", streams_c047, 3 },
		{ 49, game::MAG_049_FilePtr, game::MAG_048_ScreenFlashQueue, {}, 0, false, "Resonance", streams_r049, 1 },
		{ 54, game::g_EveryonesGrudge_File, game::unk_2617164, {}, 0, false, "Everyone's Grudge", streams_g054, 4 },
		{ 55, game::g_AquaBreath_File, game::g_EveryonesGrudge_File, {}, 0, false, "Aqua Breath", streams_a055, 2 },
		{ 60, game::g_RaijinStrike_File, game::unk_260705C, {}, 0, false, "Raijin attack 294", streams_r060, 2 },
		{ 63, game::MAG_063_TexFile, game::g_Junk_TexFile, {}, 0, false, "Helix attack 361", streams_h063, 1 },
		{ 64, game::g_Mag064_File, game::MAG_063_TexFile, {}, 0, false, "Ultimecia attack 353", streams_t064, 1 },
		{ 65, game::unk_2544F60, game::g_Mag064_File, {}, 0, false, "Great Attractor", streams_a065, 1 },
		{ 66, game::g066_Pool3Cursor, game::unk_24C91D0, {}, 0, false, "Griever death (Ultimecia)", streams_g066, 4 },
		{ 68, game::unk_25FCDD8, game::unk_26005B4, {}, 0, false, "Scan (enemy)", streams_s068, 7 },
		{ 75, game::GF_075HellsJudgement_Queue, game::unk_2798C40, {}, 0, false, "Hell's Judgement", streams_g075, 5 },
		{ 70, game::g_Pulsar_TexFile, game::unk_2544F60, {}, 0, false, "Shockwave Pulsar", streams_p070, 1 },
		{ 76, game::unk_24C3270, game::unk_24C66AC, {}, 0, false, "Ultimecia final form (attack 355)", streams_u076, 3 },
		{ 78, game::unk_25EB530, game::unk_25EC514, {}, 0, false, "Mighty Guard", streams_m078, 8 },
		{ 77, game::MAG077_TaskQueue, game::unk_2798C40, {}, 0, false, "Ultimecia final form death (attack 356)", streams_f077, 5 },
		{ 79, game::MAG_079_FilePtr, game::unk_25EB530, {}, 0, false, "Ultimecia final form death (attack 351)", streams_u079, 5 },
		{ 80, game::unk_25D4938, game::MAG_079_FilePtr, {}, 0, false, "Ultimecia junctions to Griever (attack 352)", streams_u080, 3 },
		{ 81, game::unk_25D21E0, game::unk_25D4938, {}, 0, false, "Ultimecia attack 335", streams_u081, 4 },
		{ 82, game::g_mag082_file, game::unk_25D21E0, {}, 0, false, "Ultimecia attack 337", streams_u082, 4 },
		{ 83, game::unk_25C9F70, game::g_mag082_file, {}, 0, false, "Ultimecia attack 336", streams_u083, 7 },
		{ 101, game::MAG_101_TexFile, game::g_Pulsar_TexFile, {}, 0, false, "Trauma attack 290", nullptr, 0 },
		{ 113, game::g_BadBreath_CasterEntity, game::MAG_112_METEOR_BARRET_Caster, {}, 0, false, "Bad Breath", streams_b113, 3 },
		{ 128, game::g128_CasterEntity, game::g_Mag127_FlashLevel, {}, 0, false, "Gastric Juice (attack 77)", streams_g128, 3 },
		{ 129, game::g129_Breath_BranchFrame, game::g128_CasterEntity, {}, 0, false, "Breath (attack 102)", streams_b129, 5 },
		{ 130, game::g130_CasterEntity, game::g129_Breath_BranchFrame, {}, 0, false, "Eerie Sound Wave (attack 185)", nullptr, 0 },
		{ 131, game::g131_MeshFrame, game::g130_CasterEntity, {}, 0, false, "Bad Breath (attack 184)", streams_b131, 5 },
		{ 132, game::g_Mag132_Caster, game::g131_MeshFrame, {}, 0, false, "Dissolving Acid (attack 183)", streams_d132, 3 },
		{ 133, game::g_mag133_caster, game::g_Mag132_Caster, {}, 0, false, "Hypnotize (attack 76)", streams_h133, 3 },
		{ 134, game::g_134_CasterEntity, game::g_mag133_caster, {}, 0, false, "Beam Laser (attack 176)", streams_b134, 3 },
		{ 135, game::g_ReflectBeam_Caster, game::g_134_CasterEntity, {}, 0, false, "Reflect Beam (attack 177)", streams_r135, 3 },
		{ 136, game::g_Mag136_CasterEntity, game::g_ReflectBeam_Caster, {}, 0, false, "Oil Shot (attack 180)", streams_o136, 5 },
		{ 137, game::g_mag137_CasterEntity, game::g_Mag136_CasterEntity, {}, 0, false, "Oil Blast (attack 181)", streams_o137, 4 },
		{ 138, game::MAG_138_Saliva_CasterEntity, game::g_mag137_CasterEntity, {}, 0, false, "Saliva (attack 175)", streams_s138, 5 },
		{ 179, game::unk_2309558, game::unk_231AD10, {}, 0, false, "Bomb glow (attacks 82 / 83)", streams_b179, 1 },
		{ 276, game::MAG_276_CasterEntity, game::g_MAG_275_ELECTROCUTE_QUISTIS_FlashLevel, {}, 0, false, "X-ATM092 break-down (attack 145)", streams_x276, 1 },
		{ 277, game::g277_Caster, game::MAG_276_CasterEntity, {}, 0, false, "Funguar laser (attack 115)", streams_f277, 3 },
		{ 315, game::unk_2239530, game::unk_223B660, {}, 0, false, "Fake President death (attack 151)", streams_p315, 4 },
		{ 289, game::g_mag289_TextureFile, game::unk_2507C3C, {}, 0, false, "Brrawghh! (attack 131)", streams_g289, 6 },
		{ 239, game::unk_27968D0, game::unk_2798C40, {}, 0, false, "Ochu Dance (attack 192)", streams_gfc, 5 },
		{ 240, game::unk_2796898, game::unk_2798C40, {}, 0, false, "Earthquake (attack 193, Ochu)", streams_gfc, 5 },
		{ 281, game::g281_TextureFile, game::unk_2507FD4, {}, 0, false, "Shotgun (attack 61)", streams_s281, 4 },
		{ 304, game::g304_CasterEntity, game::unk_25075D0, {}, 0, false, "Boomerang Sword (attack 159)", streams_b304, 4 },
		{ 322, game::MAG_322_Zan_TextureFile, game::unk_2505344, {}, 0, false, "Zan (attack 214, Fujin c0m119 / c0m136)", streams_b304, 4 },
		{ 323, game::MAG_323_Metsu_TextureFile, game::unk_25052AC, {}, 0, false, "Metsu (attack 215, Fujin c0m119 / c0m136)", streams_b304, 4 },
		{ 311, game::g_mag311_Caster, game::unk_2507428, {}, 0, false, "Gravija (attack 257)", streams_g311, 4 },
		{ 258, game::MAG_258_TaskQueue, game::unk_2798C40, {}, 0, false, "Mad Cow Special (attack 213, Sacred / Minotaur)", streams_gfc, 5 },
		{ 256, game::MAG_256_File01, game::unk_2798C40, {}, 0, false, "Sand Shake (attack 174, Abyss Worm)", streams_gfc, 5 },
		{ 244, game::unk_27967B8, game::unk_2798C40, {}, 0, false, "BGH251F2 turret explosion (attacks 141 / 148, BGH251F2)", streams_gfc, 5 },
		{ 220, game::unk_2796C18, game::unk_2798C40, {}, 0, false, "Light Pillar (attack 308, Omega Weapon / Ultima Weapon)", streams_gfc, 5 },
		{ 253, game::unk_27965F0, game::unk_2798C40, {}, 0, false, "Twin Homing Laser (attack 283, Mobile Type 8)", streams_gfc, 5 },
		{ 254, game::unk_27965B8, game::unk_2798C40, {}, 0, false, "Homing Laser (attack 281, Right Probe)", streams_gfc, 5 },
		{ 255, game::unk_2796580, game::unk_2798C40, {}, 0, false, "Homing Laser (attack 282, Left Probe)", streams_gfc, 5 },
		{ 249, game::unk_27966A0, game::unk_2798C40, {}, 0, false, "Attack (attack 117, BGH251F2)", streams_gfc, 5 },
		{ 250, game::unk_2796660, game::unk_2798C40, {}, 0, false, "Beam Cannon (attack 118, BGH251F2)", streams_gfc, 5 },
		{ 248, game::unk_27966D8, game::unk_2798C40, {}, 0, false, "effect 248 (no kernel.bin enemy attack)", streams_gfc, 5 },
		{ 266, game::unk_2795CF8, game::unk_2798C40, {}, 0, false, "Tiamat death (attack 306, Tiamat)", streams_gfc, 5 },
		{ 268, game::unk_2795C88, game::unk_2798C40, {}, 0, false, "Tri-Point death (attack 260, Tri-Point)", streams_gfc, 5 },
		{ 270, game::unk_2795C18, game::unk_2798C40, {}, 0, false, "Ultima Weapon Death (attack 312, Omega Weapon / Ultima Weapon)", streams_gfc, 5 },
		{ 228, game::unk_2796B38, game::unk_2798C40, {}, 0, false, "Ice Strike (Edea temporary limit break)", streams_gfc, 5 },
		{ 166, game::unk_23BF038, game::MAG_165_LION_HEART_TrailPointsBase, {}, 0, false, "Megiddo Flame (attack 310, Omega Weapon)", streams_m166, 1 },
		{ 312, game::unk_223CCA0, game::unk_2240EE0, {}, 0, false, "Megiddo Flame (attack 285, Mobile Type 8)", streams_m312, 1 },
		{ 174, game::MAG_174_Degenerator_TargetSlot, game::unk_2397274, {}, 0, false, "Degenerator (attack 72, Gesper)", streams_d174, 4 },
		{ 213, game::MAG_213_StormBreath_CasterEntity, game::unk_25101F8, {}, 0, false, "Storm Breath (attack 233, Elnoyle)", streams_s213, 2 },
		{ 217, game::MAG_217_EnergyBomber_TextureFile, game::MAG_216_DIFFERENTBEAT_File, {}, 0, false, "Energy Bomber (attack 316, Adel c0m112)", streams_e217, 4 },
		{ 345, game::MAG_345_Lv5Death_TextureFile, game::unk_255EAC0, {}, 0, false, "LV5 Death (attack 124)", streams_l345, 4 },
		{ 150, game::MAG_150_GatlingGun_TargetSlot, game::unk_2461E84, {}, 0, false, "Gatling Gun (attack 68)", streams_g150, 1 },
		{ 181, game::unk_23065D8, game::unk_2307D94, {}, 0, false, "Kamikaze", streams_k181, 1 },
		{ 313, game::unk_223B668, game::unk_223CCA0, {}, 0, false, "Ray-Bomb (attack 200)", streams_r313, 1 },
		{ 284, game::unk_2507DB0, game::unk_2507E1C, {}, 0, false, "Micro Missiles (attack 201, GIM52A)", streams_q299, 4 },
		{ 282, game::unk_2507E90, game::g281_TextureFile, {}, 0, false, "Evil-Eye (attack 152, Gargantua)", streams_q299, 4 },
		{ 285, game::unk_2507D08, game::unk_2507DAC, {}, 0, false, "Thunder Summon (attack 242, Catoblepas)", streams_q299, 4 },
		{ 286, game::unk_2507CA8, game::unk_2507D04, {}, 0, false, "Mini Pulse Cannon (attack 293, Droma)", streams_q299, 4 },
		{ 279, game::MAG_279_MegaSpark_TextureFile, game::unk_2508110, {}, 0, false, "Mega Spark (attack 106, Tri-Point)", streams_q299, 4 },
		{ 287, game::MAG_287_MegaPulseCannon_CastContext, game::unk_2556384, {}, 0, false, "Mega Pulse Cannon (attack 292, Trauma)", streams_q299, 4 },
		{ 214, game::MAG_214_OmegaGravija_CasterEntity, game::unk_2510168, {}, 0, false, "Gravija (attack 309, Omega Weapon / Ultima Weapon)", streams_g214, 8 },
		{ 207, game::MAG_207_Maelstrom_CasterEntity, game::g140_EmberPool, {}, 0, false, "Maelstrom (Edea)", streams_m207, 8 },
		{ 172, game::unk_239A8E0, game::unk_23AD700, {}, 0, false, "Morph (attack 53, Jelleye)", streams_q299, 4 },
		{ 171, game::unk_23AD700, game::unk_23AF81C, {}, 0, false, "Poison Gas (attacks 32-34, Tri-Face)", streams_q299, 4 },
		{ 170, game::MAG_170_Acid_TargetSlot, game::g_Mag169_TargetSlot, {}, 0, false, "Acid (attack 35, Tri-Face)", streams_q299, 4 },
		{ 156, game::unk_23ECE40, game::unk_23F07F0, {}, 0, false, "Wild Cannon Blow (attack 70, SAM08G)", streams_q299, 4 },
		{ 151, game::unk_244D670, game::MAG_150_GatlingGun_TargetSlot, {}, 0, false, "Cannon Blow (attack 69, SAM08G)", streams_q299, 4 },
		{ 186, game::MAG_186_Blaster_TargetSlot, game::unk_22BC128, {}, 0, false, "Blaster (attacks 122-123, Torama)", streams_q299, 4 },
		{ 321, game::unk_221BC68, game::unk_2239530, {}, 0, false, "Ker Plunk (attack 231, Jumbo Cactuar)", streams_q299, 4 },
		{ 301, game::MAG_301_GargantuaBurrow_TextureFile, game::MAG_300_BAD_BREATH_QUISTIS_MeshFrame, {}, 0, false, "Gargantua burrow (attack 265, Gargantua)", streams_g301, 5 },
		{ 298, game::MAG_298_Bgh251f2Destruction_TextureFile, game::unk_2507904, {}, 0, false, "BGH251F2 destruction (attack 119, BGH251F2 second fight)", streams_b298, 5 },
		{ 290, game::MAG_290_TraumaShatter_TextureFile, game::unk_2507BCC, {}, 0, false, "Trauma shatter (attack 291, Trauma)", streams_t290, 5 },
		{ 283, game::MAG_283_MagicSummon_TextureFile, game::unk_2507E90, {}, 0, false, "Magic Summon (attack 261, Sphinxara)", streams_q299, 4 },
		{ 241, game::unk_2796860, game::unk_2798C40, {}, 0, false, "Gatling Gun (attack 138, BGH251F2)", streams_gfc, 5 },
		{ 242, game::unk_2796828, game::unk_2798C40, {}, 0, false, "Beam Cannon (attack 139, BGH251F2)", streams_gfc, 5 },
		{ 243, game::unk_27967F0, game::unk_2798C40, {}, 0, false, "1st turret exploding (attacks 140 / 147, BGH251F2)", streams_gfc, 5 },
		{ 245, game::unk_2796780, game::unk_2798C40, {}, 0, false, "3rd turret exploding (attacks 142 / 149, BGH251F2)", streams_gfc, 5 },
		{ 246, game::unk_2796748, game::unk_2798C40, {}, 0, false, "4th turret exploding (attacks 143 / 150, BGH251F2)", streams_gfc, 5 },
		{ 247, game::unk_2796710, game::unk_2798C40, {}, 0, false, "BGH251F2 Death (attack 144)", streams_gfc, 5 },
		{ 238, game::unk_2796908, game::unk_2798C40, {}, 0, false, "Breath (attack 114, Ruby Dragon)", streams_gfc, 5 },
		{ 252, game::unk_2796628, game::unk_2798C40, {}, 0, false, "Corona (attack 284, Mobile Type 8)", streams_gfc, 5 },
		{ 257, game::unk_2796508, game::unk_2798C40, {}, 0, false, "Mega Flare (attack 234, Bahamut)", streams_gfc, 5 },
		{ 267, game::unk_2795CC0, game::unk_2798C40, {}, 0, false, "Catoblepas death (attack 241, Catoblepas)", streams_gfc, 5 },
		{ 331, game::unk_2795BE0, game::unk_2798C40, {}, 0, false, "effect 331 (attack 313, c0m118)", streams_gfc, 5 },
		{ 269, game::unk_2795C50, game::unk_2798C40, {}, 0, false, "effect 269 (attack 266, Gargantua)", streams_ps, 6 },
		{ 236, game::unk_2796978, game::unk_2798C40, {}, 0, false, "Gas (attack 127, Death Claw)", streams_gfc, 5 },
		{ 237, game::unk_2796940, game::unk_2798C40, {}, 0, false, "Explosion (attacks 128 / 129 / 130, Death Claw)", streams_gfc, 5 },
		{ 211, game::unk_2796C88, game::unk_2798C40, {}, 0, false, "Adel Death (attack 317, Adel)", streams_ps, 6 },
		{ 219, game::unk_2796C50, game::unk_2798C40, {}, 0, false, "Terra Break (attack 311, Omega Weapon)", streams_gfc, 5 },
		{ 265, game::unk_2795D30, game::unk_2798C40, {}, 0, false, "effect 265 (attack 259, Sphinxara)", streams_gfc, 5 },
		{ 208, game::unk_2796CC0, game::unk_2798C40, {}, 0, false, "Final Sorceress Death (attack 328, Sorceress (Wormlike))", streams_ps, 6 },
		{ 264, game::unk_2795D68, game::unk_2798C40, {}, 0, false, "effect 264 (attack 277, Krysta)", streams_gfc, 5 },
		{ 221, game::unk_2796BE0, game::unk_2798C40, {}, 0, false, "Apocalypse (magic; attack 373, Ultimecia final form)", streams_gfc, 5 },
		{ 344, game::MAG_344_Griever368_FirstTargetSlot, game::unk_24F0BCC, {}, 0, false, "effect 344 (attack 368, Griever)", nullptr, 0 },
		{ 302, game::MAG_302_SnipeLaser_CasterEntity, game::MAG_301_GargantuaBurrow_TextureFile, {}, 0, false, "Snipe Laser (attack 155, GIM47N)", streams_s302, 3 },
		{ 318, game::MAG_318_StrongCounterAttack_TextureFile, game::MAG_317_ACID_QUISTIS_g_Caster, {}, 0, false, "Strong counter-attack (Krysta)", streams_c318, 3 },
		{ 324, game::MAG_324_TonberryKingDeath_KeptMatrix, game::unk_221BC68, {}, 0, false, "Tonberry King death (Tonberry King)", streams_t324, 4 },
		{ 319, game::MAG_319_CounterAttack_TextureFile, game::MAG_318_StrongCounterAttack_TextureFile, {}, 0, false, "Counter-attack (Krysta)", streams_c319, 3 },
		{ 316, game::MAG_316_RedGiant_TextureFile, game::MAG_314_Abadon_TextureFile, {}, 0, false, "effect 316 (attack 203, Red Giant)", streams_r316, 1 },
		{ 314, game::MAG_314_Abadon_TextureFile, game::g_mag311_Caster, {}, 0, false, "effect 314 (attack 191, Abadon)", streams_a314, 2 },
		{ 308, game::unk_25074D0, game::g304_CasterEntity, {}, 0, false, "effect 308 (attack 286, Mobile Type 8)", streams_m308, 2 },
		{ 320, game::unk_2505348, game::MAG_319_CounterAttack_TextureFile, {}, 0, false, "Dark Flare (Tiamat)", streams_d320, 4 },
		{ 303, game::unk_25075D0, game::MAG_302_SnipeLaser_CasterEntity, {}, 0, false, "Remove mask (Sphinxaur)", streams_r303, 4 },
		{ 209, game::unk_24F3618, game::MAG_200_NOMERCY_g_TrailBasePoints, {}, 0, false, "Sorceress spawn (attacks 319 / 327, c0m115 / c0m116 / c0m117)", streams_s209, 8 },
		{ 167, game::unk_23BD350, game::unk_23BF034, {}, 0, false, "Zantetsuken (attack 274, Odin c0m070)", streams_z167, 3 },
		{ 218, game::unk_24F2340, game::unk_24F3618, {}, 0, false, "Gilgamesh kills Seifer (attack 330, Seifer c0m128)", streams_g218, 3 },
		{ 251, game::unk_2556388, game::unk_2556478, {}, 0, false, "Demon Slice (attack 314, Seifer c0m086)", streams_d251, 5 },
		{ 210, game::unk_2556478, game::unk_2556580, {}, 0, false, "Bloodfest (attack 315, Seifer c0m128)", streams_b210, 4 },
		{ 173, game::MAG_173_FloorLength, game::unk_239A8E0, {}, 0, false, "Ice Breath (attack 38, Snow Lion)", streams_i173, 1 },
		{ 176, game::MAG_176_g_curFunnelRings, game::unk_2374574, {}, 0, false, "Sand Storm (attack 80, Adamantoise)", streams_s176, 2 },
		{ 157, game::unk_23EB5C0, game::unk_23ECE40, {}, 0, false, "Breath (attack 37, Hexadragon)", streams_b157, 4 },
		{ 158, game::unk_23D7B08, game::unk_23EB5C0, {}, 0, false, "Melt-Eye (attack 36, Hexadragon)", streams_m158, 1 },
		{ 178, game::unk_231AD10, game::unk_2341948, {}, 0, false, "10,000 Needles (attack 230, Jumbo Cactuar)", nullptr, 0 },
		{ 177, game::unk_2341948, game::MAG_176_g_curFunnelRings, {}, 0, false, "1000 Needles (attack 25, Cactuar)", nullptr, 0 },
		{ 226, game::MAG_226_WhiteWind_TextureFile, game::MAG_217_EnergyBomber_TextureFile, {}, 0, false, "White Wind (attack 276, Adamantoise)", nullptr, 0 },
		{ 227, game::MAG_227_UltimeciaFirstDeath_TextureFile, game::unk_250876C, {}, 0, false, "Ultimecia First Death (attack 339, Ultimecia c0m121)", streams_u227, 3 },
		{ 139, game::MAG_139_CasterEntity, game::MAG_138_Saliva_CasterEntity, {}, 0, false, "Sonic Wave (attack 182)", streams_s139, 1 },
		{ 50, game::MAG_050_POTION_TextureFile, game::MAG_049_FilePtr, {}, 0, false, "Potion (items Potion / Potion+)", streams_i050, 3 },
		{ 51, game::MAG_051_HIPOTION_FilePtr, game::MAG_050_POTION_TextureFile, {}, 0, false, "Hi-Potion (items Hi-Potion / Hi-Potion+)", streams_i051, 1 },
		{ 53, game::MAG_053_MEGAPOTION_FileData, game::unk_261814C, {}, 0, false, "Mega-Potion (item)", streams_i053, 4 },
		{ 58, game::MAG_058_ELIXIR_FileData, game::unk_260B30C, {}, 0, false, "Elixir (item)", streams_i058, 6 },
		{ 59, game::MAG_059_MEGALIXIR_FileData, game::unk_2608044, {}, 0, false, "Megalixir (item)", streams_i059, 6 },
		{ 52, game::MAG_052_XPOTION_FlashQueue, game::unk_2619F4C, {}, 0, false, "X-Potion (item)", streams_i052, 3 },
		{ 5, game::MAGICEFFECT05_TEX_FILEHANDLE, game::unk_276FAE8, {}, 0, false, "Phoenix Down (item)", streams_i005, 2 },
		{ 7, game::MAG_007_MEGAPHOENIX_FileData, game::unk_2760B64, {}, 0, false, "Mega Phoenix (item)", streams_i007, 4 },
		{ 292, game::MAG_292_SOFT_TextureFile, game::unk_2507B34, {}, 0, false, "Soft (item)", nullptr, 0 },
		{ 293, game::MAG_293_EYEDROPS_TextureFile, game::unk_2507AE4, {}, 0, false, "Eye Drops (item)", nullptr, 0 },
		{ 294, game::MAG_294_ANTIDOTE_TextureFile, game::unk_2507A94, {}, 0, false, "Antidote (item)", nullptr, 0 },
		{ 295, game::MAG_295_ECHOSCREEN_TextureFile, game::MAG_294_ANTIDOTE_TextureFile, {}, 0, false, "Echo Screen (item)", nullptr, 0 },
		{ 296, game::MAG_296_HOLYWATER_TextureFile, game::unk_25079D4, {}, 0, false, "Holy Water (item)", nullptr, 0 },
		{ 67, game::MAG067_REMEDY_timBuffer, game::unk_2601314, {}, 0, false, "Remedy (items Remedy / Remedy+)", streams_i067, 3 },
		{ 309, game::MAG_309_HERO_TextureFile, game::unk_25074CC, {}, 0, false, "Hero (items Hero-trial / Hero)", nullptr, 0 },
		{ 310, game::MAG_310_HOLYWAR_TextureFile, game::unk_250747C, {}, 0, false, "Holy War (items Holy War-trial / Holy War)", nullptr, 0 },
		{ 160, game::MAG_160_RENZOKUKEN_TrailPointsBase, game::unk_23D5644, {}, 0, false, "Renzokuken (Squall)", streams_r160, 1 },
		{ 163, game::MAG_163_ROUGH_DIVIDE_TrailPointsBase, game::MAG_162_FATED_CIRCLE_TrailPointsBase, {}, 0, false, "Rough Divide (Squall's Renzokuken finisher)", nullptr, 0 },
		{ 162, game::MAG_162_FATED_CIRCLE_TrailPointsBase, game::unk_23D0D8C, {}, 0, false, "Fated Circle (Squall's Renzokuken finisher)", streams_f162, 2 },
		{ 164, game::MAG_164_BLASTING_ZONE_TargetSlot, game::MAG_163_ROUGH_DIVIDE_TrailPointsBase, {}, 0, false, "Blasting Zone (Squall's Renzokuken finisher)", streams_b164, 2 },
		{ 165, game::MAG_165_LION_HEART_TrailPointsBase, game::MAG_164_BLASTING_ZONE_TargetSlot, {}, 0, false, "Lion Heart (Squall's Renzokuken finisher)", streams_l165, 2 },
		{ 183, game::MAG_183_DEFEND_TargetSlot, game::MAG_182_CARD_TargetSlot, {}, 0, false, "Defend (command ability)", nullptr, 0 },
		{ 182, game::MAG_182_CARD_TargetSlot, game::unk_23065D4, {}, 0, false, "Card (command ability)", streams_c182, 1 },
		{ 57, game::MAG057_TREATMENT_timBuffer, game::unk_260E334, {}, 0, false, "Treatment (command ability)", streams_t057, 3 },
		{ 56, game::MAG_056_ScreenFlashQueue, game::unk_260F144, {}, 0, false, "Absorb (command ability)", streams_a056, 2 },
		{ 16, game::MAG_016_RECOVER_FilePtr, game::unk_2732BF0, {}, 0, false, "Recover (command ability)", streams_r016, 3 },
		{ 61, game::MAG_061_REVIVE_FilePtr, game::unk_2604070, {}, 0, false, "Revive (command ability)", streams_r061, 3 },
		{ 272, game::MAG_272_LV_DOWN_CasterEntity, game::MAG_271_LV_UP_CasterEntity, {}, 0, false, "LV Down (command ability)", nullptr, 0 },
		{ 271, game::MAG_271_LV_UP_CasterEntity, game::MAG_260_SHOCKWAVE_PULSAR_QUISTIS_TextureFile, {}, 0, false, "LV Up (command ability)", nullptr, 0 },
		{ 273, game::MAG_273_MAD_RUSH_CasterEntity, game::MAG_272_LV_DOWN_CasterEntity, {}, 0, false, "Mad Rush (command ability)", streams_m273, 3 },
		{ 62, game::MAG_062_DEVOUR_TimFile, game::MAG_061_REVIVE_FilePtr, {}, 0, false, "Devour (command ability)", streams_d062, 1 },
		{ 184, game::MAG_184_ULTRA_WAVES_QUISTIS_TargetSlot, game::MAG_183_DEFEND_TargetSlot, {}, 0, false, "Ultra Waves (Quistis blue magic)", streams_q184, 2 },
		{ 74, game::MAG_074_LVDEATH_QUISTIS_g_FileData, game::unk_25F3F58, {}, 0, false, "LV?Death (Quistis blue magic)", streams_q074, 4 },
		{ 73, game::MAG_073_MIGHTY_GUARD_QUISTIS_g_FileData, game::MAG_072_AQUA_BREATH_QUISTIS_FilePtr, {}, 0, false, "Mighty Guard (Quistis blue magic)", streams_q073, 8 },
		{ 72, game::MAG_072_AQUA_BREATH_QUISTIS_FilePtr, game::unk_25FA400, {}, 0, false, "Aqua Breath (Quistis blue magic)", streams_q072, 2 },
		{ 297, game::MAG_297_WHITE_WIND_QUISTIS_FilePtr, game::MAG_296_HOLYWATER_TextureFile, {}, 0, false, "White Wind (Quistis blue magic)", nullptr, 0 },
		{ 230, game::unk_2796AC8, game::unk_2798C40, {}, 0, false, "Fire Breath (Quistis blue magic)", streams_gfc, 5 },
		{ 229, game::unk_2796B00, game::unk_2798C40, {}, 0, false, "Homing Laser (Quistis blue magic)", streams_gfc, 5 },
		{ 306, game::MAG_306_DEGENERATOR_QUISTIS_TargetSlot, game::unk_224564C, {}, 0, false, "Degenerator (Quistis blue magic)", streams_q306, 4 },
		{ 307, game::MAG_307_RAY_BOMB_QUISTIS_RootPool, game::MAG_306_DEGENERATOR_QUISTIS_TargetSlot, {}, 0, false, "Ray-Bomb (Quistis blue magic)", streams_q307, 1 },
		{ 305, game::MAG_305_GATLING_GUN_QUISTIS_TargetSlot, game::unk_2257794, {}, 0, false, "Gatling Gun (Quistis blue magic)", streams_q305, 1 },
		{ 71, game::MAG_071_LASER_EYE_QUISTIS_g_FileData, game::unk_25FCDD8, {}, 0, false, "Laser Eye (Quistis blue magic)", streams_q071, 3 },
		{ 317, game::MAG_317_ACID_QUISTIS_g_Caster, game::MAG_316_RedGiant_TextureFile, {}, 0, false, "Acid (Quistis blue magic)", streams_q317, 3 },
		{ 300, game::MAG_300_BAD_BREATH_QUISTIS_MeshFrame, game::MAG_299_MICRO_MISSILES_QUISTIS_TextureFile, {}, 0, false, "Bad Breath (Quistis blue magic)", streams_q300, 5 },
		{ 299, game::MAG_299_MICRO_MISSILES_QUISTIS_TextureFile, game::unk_250787C, {}, 0, false, "Micro Missiles (Quistis blue magic)", streams_q299, 4 },
		{ 275, game::g_MAG_275_ELECTROCUTE_QUISTIS_FlashLevel, game::unk_2508410, {}, 0, false, "Electrocute (Quistis blue magic)", streams_q275, 4 },
		{ 260, game::MAG_260_SHOCKWAVE_PULSAR_QUISTIS_TextureFile, game::MAG_227_UltimeciaFirstDeath_TextureFile, {}, 0, false, "Shockwave Pulsar (Quistis blue magic)", streams_q260, 1 },
		{ 280, game::unk_2507FD8, game::MAG_279_MegaSpark_TextureFile, {}, 0, false, "Full-cure (Selphie slot)", streams_s280, 3 },
		{ 288, game::unk_2507C40, game::unk_2507CA8, {}, 0, false, "Rapture (Selphie slot)", streams_s288, 3 },
		{ 274, game::unk_2508410, game::MAG_273_MAD_RUSH_CasterEntity, {}, 0, false, "Duel (Zell limit break)", streams_z274, 5 },
		{ 189, game::unk_2293F4C, game::unk_22B2A34, {}, 0, false, "Wall (Selphie slot)", streams_s189, 5 },
		{ 85, game::MAG085_THE_END_timBuffer, game::MAG_084_ANGEL_WING_pTextureFile, {}, 0, false, "The End (Selphie slot)", streams_s085, 1 },
		{ 84, game::MAG_084_ANGEL_WING_pTextureFile, game::unk_25C9F70, {}, 0, false, "Angel Wing (Rinoa limit break)", streams_r084, 6 },
		{ 112, game::MAG_112_METEOR_BARRET_Caster, game::unk_2521970, {}, 0, false, "Meteor Barret (Zell finisher)", streams_z112, 3 },
		{ 212, game::unk_25101F8, game::MAG_207_Maelstrom_CasterEntity, {}, 0, false, "Burning Rave (Zell finisher)", streams_z212, 4 },
		{ 93, game::unk_2592548, game::unk_2594D98, {}, 0, false, "Angelo Recover (Rinoa Angelo command)", streams_r093, 8 },
		{ 94, game::unk_258FCF8, game::unk_2592548, {}, 0, false, "Angelo Reverse (Rinoa Angelo command)", streams_r094, 8 },
		{ 215, game::MAG_215_MYFINALHEAVEN_File, game::unk_25089AC, {}, 0, false, "My Final Heaven (Zell finisher)", streams_z215, 4 },
		{ 91, game::unk_2598F28, game::MAG_090_PacketCursor, {}, 0, false, "Angelo Rush (Rinoa Angelo command)", streams_r091, 9 },
		{ 216, game::MAG_216_DIFFERENTBEAT_File, game::unk_2508930, {}, 0, false, "Different Beat (Zell finisher)", streams_z216, 4 },
		{ 92, game::unk_2594D98, game::unk_2598F20, {}, 0, false, "Angelo Search (Rinoa Angelo command)", streams_r092, 6 },
		{ 88, game::unk_25A4D98, game::unk_25ACEAC, {}, 0, false, "Invincible Moon (Rinoa Combine limit break)", streams_r088, 7 },
		{ 188, game::MAG_188_NORMALSHOT_AfterimageEndTick, game::unk_22B75F4, {}, 0, false, "Normal Shot (Irvine limit break)", streams_i188, 4 },
		{ 196, game::MAG_196_QUICKSHOT_AfterimageEndTick, game::unk_226A96C, {}, 0, false, "Quick Shot (Irvine limit break)", streams_i196, 4 },
		{ 192, game::MAG_192_SCATTERSHOT_AfterimageEndTick, game::unk_2281B14, {}, 0, false, "Scatter Shot (Irvine limit break)", streams_i192, 4 },
		{ 194, game::MAG_194_FLAMESHOT_AfterimageEndTick, game::MAG_193_DARKSHOT_AfterimageEndTick, {}, 0, false, "Flame Shot (Irvine limit break)", streams_i194, 4 },
		{ 86, game::MAG_086_ANGELO_CANNON_pFile, game::unk_25BC38C, {}, 0, false, "Angelo Cannon (Rinoa Combine limit break)", streams_r086, 6 },
		{ 195, game::MAG_195_CANISTERSHOT_AfterimageEndTick, game::unk_227146C, {}, 0, false, "Canister Shot (Irvine limit break)", streams_i195, 4 },
		{ 193, game::MAG_193_DARKSHOT_AfterimageEndTick, game::unk_227B334, {}, 0, false, "Dark Shot (Irvine limit break)", streams_i193, 4 },
		{ 197, game::MAG_197_ARMORSHOT_AfterimageEndTick, game::MAG_196_QUICKSHOT_AfterimageEndTick, {}, 0, false, "Armor Shot (Irvine limit break)", streams_i197, 4 },
		{ 87, game::MAG_087_ANGELO_STRIKE_pFile, game::unk_25B4CC4, {}, 0, false, "Angelo Strike (Rinoa Combine limit break)", streams_r087, 6 },
		{ 198, game::MAG_198_HYPERSHOT_AfterimageEndTick, game::unk_2260464, {}, 0, false, "Hyper Shot (Irvine limit break)", streams_i198, 4 },
		{ 200, game::MAG_200_NOMERCY_g_TrailBasePoints, game::unk_24FBD68, {}, 0, false, "No Mercy (Seifer limit break)", streams_s200, 2 },
		{ 89, game::g_MAG_089_CastContext, game::unk_2556624, {}, 0, false, "Wishing Star (Rinoa Combine limit break)", streams_r089, 6 },
		{ 261, game::g_MAG261_RootQueue, game::unk_2798C40, {}, 0, false, "Desperado (Laguna limit break)", streams_gfc, 5 },
		{ 263, game::g_MAG263_File01, game::unk_2798C40, {}, 0, false, "Massive Anchor (Ward limit break)", streams_gfc, 5 },
		{ 262, game::g_MAG262_File01, game::unk_2798C40, {}, 0, false, "Blood Pain (Kiros limit break)", streams_gfc, 5 },
		{ 17, game::MAG_017_g_CasterEntity, game::unk_2557000, {}, 0, false, "Elvoret entrance (Elvoret c0m091 AI init: attackAnimation 17)", streams_e017, 2 },
		{ 110, game::MAG_110_g_TimFile, game::unk_2521A64, {}, 0, false, "Biggs & Wedge 1st death (Biggs c0m074 / Wedge c0m073 death script: attackAnimation 110)", streams_b110, 2 },
		{ 18, game::MAG_018_ElvoretDeath_g_TaskQueue, game::unk_2798C40, {}, 0, false, "Elvoret death (Elvoret c0m091 death script: attackAnimation 18)", streams_gfc, 5 },
		{ 161, game::MAG161_TrailPointsBase, game::unk_23D34A4, {}, 0, false, "Renzokuken vs Elvoret / Elnoyle (Squall; target c0m091 / c0m131)", streams_r161, 2 },
		{ 159, game::MAG159_TrailPointsBase, game::unk_23D7B04, {}, 0, false, "Renzokuken vs X-ATM092 (Squall; target c0m092)", streams_r159, 1 },
		{ 141, game::MAG141_TrailPointsBase, game::unk_24C3270, {}, 0, false, "Renzokuken 5 hits (Squall; crisis level 1 / 2)", streams_r141, 1 },
		{ 259, game::MAG259_TrailPointsBase, game::unk_225994C, {}, 0, false, "Renzokuken 6 hits (Squall; crisis level 2 / 3)", streams_r259, 1 },
		{ 332, game::MAG332_TrailPointsBase, game::unk_21FF2A4, {}, 0, false, "Renzokuken 7 hits (Squall; crisis level 3 / 4)", streams_r332, 1 },
		{ 333, game::MAG333_TrailPointsBase, game::unk_21FC2BC, {}, 0, false, "Renzokuken 8 hits (Squall; crisis level 4)", streams_r333, 1 },
		{ 334, game::unk_21F7250, game::unk_21FA0C8, {}, 0, false, "Renzokuken vs Bahamut (Squall; target c0m067)", streams_r334, 1 },
		{ 335, game::unk_21F4418, game::unk_21F724C, {}, 0, false, "Renzokuken vs NORG (Squall; target c0m077)", streams_r335, 1 },
		{ 336, game::unk_21F15E0, game::unk_21F4414, {}, 0, false, "Renzokuken vs Ultima Weapon (Squall; target c0m090)", streams_r336, 2 },
		{ 337, game::unk_21EE748, game::unk_21F15C0, {}, 0, false, "Renzokuken vs the Sorceress, wormlike (Squall; target c0m117)", streams_r337, 2 },
		{ 339, game::unk_21EB8B0, game::unk_21EE728, {}, 0, false, "Renzokuken vs Adel (Squall; target c0m112)", streams_r339, 1 },
		{ 340, game::unk_21E8A78, game::unk_21EB8AC, {}, 0, false, "Renzokuken vs Ultimecia, final form (Squall; target c0m126)", streams_r340, 1 },
		{ 341, game::unk_21E5BE0, game::unk_21E8A58, {}, 0, false, "Renzokuken vs Jumbo Cactuar (Squall; target c0m084)", streams_r341, 2 },
		{ 342, game::unk_21E2DA8, game::unk_21E5BDC, {}, 0, false, "Renzokuken vs Ultimecia + Griever (Squall; target c0m124)", streams_r342, 1 },
		{ 343, game::unk_21DFEF0, game::unk_21E2D88, {}, 0, false, "Renzokuken vs Griever (Squall; target c0m122)", streams_r343, 1 },
	};
	const int module_count = (int)(sizeof(modules) / sizeof(modules[0]));

	const Region engine_regions[] = {
		{ game::MAGIC_TEXTURE_BUFFER_BASE, 0x100000, "magic buffer" },
		{ game::unk_21DFAB8, 4, "magic buffer cursor" },
		{ game::MAGIC_TEXTURE_BUFFER_PTR, 4, "MAGIC_TEXTURE_BUFFER_PTR" },
		{ game::Battle_Camera_world_XZ_s16, 0x20, "battle camera" },
		{ game::unk_1D97700, 0xB0, "camera state" },
		{ game::BATTLE_PROJECTION_DISTANCE, 0x20, "render list globals" },
		{ game::GTE_VXY0, 0x70, "GTE data registers" },
		{ game::CAMERA_ROT_X, 0xF0, "GTE control registers" },
		{ game::BATTLE_EFFECT_TASK_POOL, 0x2A40, "hit-effect queue pool + header" },
		{ game::BATTLE_STAGE_RENDER_GROUPS, 0xDC, "battle entity array" },
		{ game::BattleEntitySlotData, 0x440, "BattleEntitySlotData" },
		{ game::BattleStateEntity, 0x1C0, "battle state controllers" },
		{ game::unk_1D971F0, 0xD0, "weapon animation slots" },
		{ game::unk_1D99AB0, 0x20, "effect light/position" },
		{ game::unk_1DCD6E0, 0x10, "streamed-file loader" },
		{ game::GLOBAL_MEMORY_POOL, 4, "Field_Alloc pointer" },
		{ game::g_Effect_SharedLcgSeed, 4, "effect LCG seed" },
		{ game::g_Battle_ScreenFeedbackRequest, 4, "screen feedback request" },
		{ game::BATTLE_VRAM_COMMANDS, 0x204, "VRAM upload queue" },
		{ game::ssigpu_execution_cur, 4, "ssigpu_execution_cur" },
	};
	const int engine_region_count = (int)(sizeof(engine_regions) / sizeof(engine_regions[0]));

	const ExtSite ext_sites[] = {
		{ game::BdPlaySE, 3, "BdPlaySE" },
		{ game::BdPlaySy, 3, "BdPlaySy" },
		{ game::setMonsterPresence, 1, "setMonsterPresence" },
		{ game::BdPlaySummonStream, 3, "BdPlaySummonStream" },
		{ game::BdTransSummonStream, 2, "BdTransSummonStream" },
		{ game::BdSound_ClaimVoiceSlot, 3, "BdSound_ClaimVoiceSlot" },
		{ game::BdSound_ReleaseVoiceSlot, 1, "BdSound_ReleaseVoiceSlot" },
		{ game::pre_LoadBattleFile, 4, "pre_LoadBattleFile" },
		{ game::BattleFile_CharacterLoad, 0, "BattleFile_CharacterLoad" },
		{ game::ApplyActionResultToTargets, 2, "ApplyActionResultToTargets" },
		{ game::ApplyActionResultToTarget, 0, "ApplyActionResultToTarget" },
		{ game::QueueChainTransformation, 0, "QueueChainTransformation" },
		{ game::AddTaskToQueueAnimSeq, 1, "AddTaskToQueueAnimSeq" },
		{ game::BattleEntity_BindExternalAnimSet, 3, "BS_BindEntityModelFromComData" },
		{ game::loadBS_36Or37, 2, "loadBS_36Or37" },
		{ game::BS_Stage142_ReplaceGeometry, 2, "BS_SwapModel_512AC0" },
		{ game::sub_46B3A0, 0, "Sound_46B3A0" },
		{ game::sub_46B3E0, 0, "Sound_46B3E0" },
		{ game::sub_46B450, 0, "Sound_46B450" },
		{ game::Music_SetVolumeTrans, 0, "Sound_46BBC0" },
		{ game::Battle_SetMusicVolumeUnlessEncounterFlag2, 0, "Music_47E3C0" },
		{ game::GfCinematic_ClutTintUpload, 0, "GfCinematic_ClutUpload" },
		{ game::BattleMenu_SetVisible, 0, "Battle_4A8480" },
		{ game::BattleFade_QueueWhiteOverlay, 0, "Battle_501E40" },
		{ game::BattleFade_ReleaseWhiteOverlay, 0, "Battle_501F30" },
		{ game::BattleAnim_SetSequencePause, 0, "Battle_504270" },
		{ game::Battle_PlayCameraAnimation, 0, "Battle_5099A0" },
		{ game::BS_Camera_ArmReturnAfterEffect, 0, "Battle_50A730" },
		{ game::GfCinematic_RenderPartyModelsOffscreen, 0, "GfCinematic_RenderPartyModelsOffscreen" },
		{ game::GfCinematic_RenderBattleStageOffscreen, 0, "GfCinematic_RenderBattleStageOffscreen" },
		{ game::GfCinematic_RenderBattleModelOffscreen, 0, "GfCinematic_RenderBattleModelOffscreen" },
		{ game::GfCinematic_RenderStageGroundOffscreen, 0, "GfCinematic_RenderStageGroundOffscreen" },
		{ game::QueueBlitCommand, 0, "QueueBlitCommand" },
		{ game::SdStreamingVolumeTranslation, 0, "SdStreamingVolumeTranslation" },
		{ game::Music_StopChannelOrAll, 1, "Music_StopChannelOrAll" },
		{ game::Sfx_SetAllChannelsVolumeTrans, 2, "Sfx_SetAllChannelsVolumeTrans" },
		{ game::printMonsterRelatedText, 3, "printMonsterRelatedText" },
		{ game::BS_R0win_QueueVictoryFanfare, 2, "BS_R0win_QueueVictoryFanfare" },
		{ game::BS_Music_SetChannelVolumeCommand, 2, "BS_Music_SetChannelVolumeCommand" },
		{ game::Music_WavStream_Play, 1, "Music_WavStream_Play" },
		{ game::pre_updateEntityStatusFlag, 2, "pre_updateEntityStatusFlag" },
		{ game::BattleEntity_SelectStatusBaseSeq, 1, "updateEntityAnimation" },
		{ game::Battle_StopCombatAndClearPartySummons, 0, "linkedToSummonGF" },
		{ game::Battle_RequestScreenFeedback, 0, "Battle_RequestScreenFeedback" },
		{ game::Music_SetVolumeImmediate, 0, "Music_SetVolumeImmediate" },
		{ game::AnimSeq_StartSeqIfFightOrDamage, 0, "QueueChainTransformationConditional" },
		{ game::BdPlayStream, 0, "BdPlayStream" },
		{ game::Battle_QueueTIMUpload_GetEOF, 0, "Battle_QueueTIMUpload_GetEOF" },
		{ game::Battle_QueueVramUpload_Type0_RectData, 0, "Battle_QueueVramUpload" },
		{ game::Battle_RestoreDefaultEffectTexture, 0, "Battle_RestoreDefaultEffectTexture" },
		{ game::BdSound_IsVoiceSlotActive, 0, "Engine_4A2900" },
		{ game::OutputDebugString_1, 0, "OutputDebugString_1" },
		{ game::Battle_QueueVramReadback_Type2, 0, "Battle_QueueVramReadback_Type2" },
		{ game::BdPlaySE3D, 2, "BdPlaySE3D" },
		{ game::BS_WitchFight_GetNewStage, 1, "BS_WitchGetNewStage" },
		{ game::BS_ChangeStage, 1, "BS_ChangeStage" },
		{ game::read_pad_held_raw, 2, "read_pad_held_raw" },
		{ game::remap_pad_input, 1, "remap_pad_input" },
		{ game::sub_49F0A0, 3, "ReadAnalog_49F0A0" },
		{ game::Text_SetLayerText, 1, "Text_SetLayerText" },
		{ game::Text_SetWindowFlags, 0, "Text_4A0700" },
		{ game::setTextPosition, 0, "setTextPosition" },
		{ game::Text_OpenWindowInstant, 0, "Text_4A0640" },
		{ game::Text_CloseWindowInstant, 0, "Text_4A0680" },
		{ game::sub_49FBC0, 0, "Text_49FBC0" },
		{ game::manageScanText_Dup, 1, "manageScanText_Dup" },
		{ game::ScanText_GetLine_Dup, 0, "ScanText_B68810" },
		{ game::manageScanText, 1, "manageScanText" },
		{ game::ScanText_GetLine, 0, "ScanText_GetLine" },
		{ game::Scan_GetText, 0, "ScanText_B687C0" },
		{ game::getAddressCharacterName, 1, "CharacterName_47EAF0" },
		{ game::BattleText_GetMonsterName, 1, "MonsterName_495100" },
		{ game::BdPlayActorSE, 4, "BdPlayActorSE" },
		{ game::printMonsterRelatedText0, 2, "printMonsterRelatedText0" },
		{ game::HitEffect_QueueFunctionTask, 2, "AddTaskToQueueAnimSeq_99_B0_B1_B4_A5" },
		{ game::BattleEntity_DrawShadow, 4, "BattleEntity_DrawShadow" },
		{ game::BattleFile_StreamState_Init, 1, "BattleFile_StreamState_Init" },
		{ game::Stream_QueueLoad, 3, "Stream_QueueLoad" },
		{ game::Stream_Pump, 0, "Stream_Pump" },
		{ game::BattleFile_PreLoad_IsLoaded, 1, "BattleFile_PreLoad_IsLoaded" },
	};
	const int ext_site_count = (int)(sizeof(ext_sites) / sizeof(ext_sites[0]));

	const Module *find_module(int effect_id)
	{
		for (int m = 0; m < module_count; m++)
			if (modules[m].effect_id == effect_id) return &modules[m];
		return nullptr;
	}

	uint32_t streams_bytes(const Module &m)
	{
		uint32_t n = 0;
		for (int i = 0; i < m.nstreams; i++) n += m.streams[i].size;
		return n;
	}
}
