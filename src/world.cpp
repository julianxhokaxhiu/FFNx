/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2020 Marcin Gomulak                                     //
//    Copyright (C) 2026 Julian Xhokaxhiu                                   //
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

#include <imgui.h>
#include <stdint.h>

#include "globals.h"
#include "common.h"
#include "patch.h"

#include "world.h"


//https://stackoverflow.com/a/19885112/4509036
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')


WORD* cameraZoom;
DWORD* playerPosX; //203EE80
DWORD* playerPosZ; //+1
DWORD* playerPosY; //+1; it's dword, but later in game it's &0xFFFF
BYTE* collisionTriangleGroundType; //0x2035D2E
BYTE* collisionTriangleTextureFlag; //+1

DWORD* currentCollideTriangle; //ESI+0x1C / 53CEE9
DWORD* bCollisionEnabled;

void world_init()
{
	if (ff8)
	{
		cameraZoom = ff8_externals.camera_zoom_dword_1CA92E4;
		playerPosX = (DWORD*)get_absolute_value(ff8_externals.sub_53BB90, 0x4C);
		playerPosZ = playerPosX+1;
		playerPosY = playerPosX+2;
		BYTE *wm_struct;
		if(FF8_US_VERSION)
		{
			wm_struct = (BYTE*)get_absolute_value(ff8_externals.sub_53C750, 0x633);
			bCollisionEnabled = (DWORD*)get_absolute_value(ff8_externals.sub_53E6B0, 0x4);
		}
		else
		{
			wm_struct = (BYTE*)get_absolute_value(ff8_externals.sub_53C750, 0x649);
			bCollisionEnabled = (DWORD*)get_absolute_value(ff8_externals.sub_53E6B0, 0x1);
		}
		collisionTriangleGroundType = wm_struct + 0x26;
		collisionTriangleTextureFlag = collisionTriangleGroundType + 1;

		currentCollideTriangle = (DWORD*)(wm_struct + 0x1C);
	}
}

void world_debug(bool* isOpen)
{
	if (!ImGui::Begin("World Debug", isOpen, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::End();
		return;
	}

	if (getmode_cached()->driver_mode != MODE_WORLDMAP)
	{
		ImGui::Text("Not currently on a worldmap.");
		ImGui::End();
		return;
	}

	ImGui::SliderInt("Camera zoom ", (int*)cameraZoom, 0, 9000);
	ImGui::Text("	Player position:"); //didn't use InputInt3 because it's XZ-Y not XYZ
	ImGui::InputInt("X", (int*)playerPosX, 10);
	ImGui::SameLine();
	ImGui::InputInt("Y", (int*)playerPosY, 10);
	ImGui::SameLine();
	ImGui::InputInt("Z", (int*)playerPosZ, 10);
	ImGui::Checkbox("Enable collision", (bool*)bCollisionEnabled);
	ImGui::Text("	Current collide triangle:");
	ImGui::Text("Ground type: %d, texture flag: 0b%c%c%c%c%c%c%c%c", //this is cool stuff
		*collisionTriangleGroundType,
		BYTE_TO_BINARY(*collisionTriangleTextureFlag)
		);
	ImGui::Text("triangle pointer: %08X:", *currentCollideTriangle);

	if (*currentCollideTriangle != 0) //this is to prevent nullptr crashes
	{
		ImU8 u8step = 1;
		ImU8 u8stepTen = 10;
		int tPage, clut, texFlags, vertFlags;
		bool texFlagsBool[8];
		bool vertFlagsBool[8];
		tPage = (*((BYTE*)*currentCollideTriangle + 12) >> 4) & 0x0F;
		clut = *((BYTE*)*currentCollideTriangle + 12) &0x0F;
		texFlags = *((BYTE*)*currentCollideTriangle + 14);
		vertFlags = *((BYTE*)*currentCollideTriangle + 15);

		for (int i = 0; i < 8; i++)
		{
			texFlagsBool[i] = ((texFlags >> 7 - i) & 0b1) == 1;
			vertFlagsBool[i] = ((vertFlags >> 7 - i) & 0b1) == 1;
		}

		ImGui::InputScalar("F1", ImGuiDataType_U8 ,(BYTE*)*currentCollideTriangle, &u8step, &u8stepTen, "%d");
		ImGui::SameLine();
		ImGui::InputScalar("F2", ImGuiDataType_U8, (BYTE*)*currentCollideTriangle + 1, &u8step, &u8stepTen, "%d");
		ImGui::SameLine();
		ImGui::InputScalar("F3", ImGuiDataType_U8, (BYTE*)*currentCollideTriangle + 2, &u8step, &u8stepTen, "%d");

		ImGui::InputScalar("N1", ImGuiDataType_U8, (BYTE*)*currentCollideTriangle + 3, &u8step, &u8stepTen, "%d");
		ImGui::SameLine();
		ImGui::InputScalar("N2", ImGuiDataType_U8, (BYTE*)*currentCollideTriangle + 4, &u8step, &u8stepTen, "%d");
		ImGui::SameLine();
		ImGui::InputScalar("N3", ImGuiDataType_U8, (BYTE*)*currentCollideTriangle + 5, &u8step, &u8stepTen, "%d");

		ImGui::InputScalar("U1", ImGuiDataType_U8, (BYTE*)*currentCollideTriangle + 6, &u8step, &u8stepTen, "%d");
		ImGui::SameLine();
		ImGui::InputScalar("V1", ImGuiDataType_U8, (BYTE*)*currentCollideTriangle + 7, &u8step, &u8stepTen, "%d");
		ImGui::InputScalar("U2", ImGuiDataType_U8, (BYTE*)*currentCollideTriangle + 8, &u8step, &u8stepTen, "%d");
		ImGui::SameLine();
		ImGui::InputScalar("V2", ImGuiDataType_U8, (BYTE*)*currentCollideTriangle + 9, &u8step, &u8stepTen, "%d");
		ImGui::InputScalar("U3", ImGuiDataType_U8, (BYTE*)*currentCollideTriangle + 10, &u8step, &u8stepTen, "%d");
		ImGui::SameLine();
		ImGui::InputScalar("V3", ImGuiDataType_U8, (BYTE*)*currentCollideTriangle + 11, &u8step, &u8stepTen, "%d");

		ImGui::InputInt("TPage", &tPage);
		ImGui::InputInt("Clut", &clut);
		ImGui::InputScalar("Ground type", ImGuiDataType_U8, (BYTE*)*currentCollideTriangle + 13, &u8step, &u8stepTen, "%d");

		ImGui::Text("TEXFLAGS:");
		ImGui::Checkbox("TEXFLAGS_MISC", &texFlagsBool[0]);
		ImGui::Checkbox("TEXFLAGS_WATER", &texFlagsBool[1]);
		ImGui::Checkbox("TEXFLAGS_ROAD", &texFlagsBool[2]);
		ImGui::Checkbox("TEXFLAGS_TRANSPARENT", &texFlagsBool[3]);
		ImGui::Checkbox("TEXFLAGS_ENTERABLE", &texFlagsBool[4]);
		ImGui::Checkbox("TEXFLAGS_UNK", &texFlagsBool[5]);
		ImGui::Checkbox("TEXFLAGS_SHADOW1", &texFlagsBool[6]);
		ImGui::Checkbox("TEXFLAGS_SHADOW2", &texFlagsBool[7]);
		texFlags = 0;
		texFlags |= texFlagsBool[0] ? 0x80 : 00;
		texFlags |= texFlagsBool[1] ? 0x40 : 00;
		texFlags |= texFlagsBool[2] ? 0x20 : 00;
		texFlags |= texFlagsBool[3] ? 0x10 : 00;
		texFlags |= texFlagsBool[4] ? 0x08 : 00;
		texFlags |= texFlagsBool[5] ? 0x04 : 00;
		texFlags |= texFlagsBool[6] ? 0x02 : 00;
		texFlags |= texFlagsBool[7] ? 0x01 : 00;

		ImGui::Text("VERTFLAGS:");
		ImGui::Checkbox("VERTFLAGS_WALKABLE", &vertFlagsBool[0]);
		ImGui::Checkbox("VERTFLAGS_UNK", &vertFlagsBool[1]);
		ImGui::Checkbox("VERTFLAGS_UNK2", &vertFlagsBool[2]);
		ImGui::Checkbox("VERTFLAGS_WALKABLECHOCOBO?", &vertFlagsBool[3]);
		ImGui::Checkbox("VERTFLAGS_UNK3", &vertFlagsBool[4]);
		ImGui::Checkbox("VERTFLAGS_UNK4", &vertFlagsBool[5]);
		ImGui::Checkbox("VERTFLAGS_UNK5", &vertFlagsBool[6]);
		ImGui::Checkbox("VERTFLAGS_UNK6", &vertFlagsBool[7]);
		vertFlags = 0;
		vertFlags |= vertFlagsBool[0] ? 0x80 : 00;
		vertFlags |= vertFlagsBool[1] ? 0x40 : 00;
		vertFlags |= vertFlagsBool[2] ? 0x20 : 00;
		vertFlags |= vertFlagsBool[3] ? 0x10 : 00;
		vertFlags |= vertFlagsBool[4] ? 0x08 : 00;
		vertFlags |= vertFlagsBool[5] ? 0x04 : 00;
		vertFlags |= vertFlagsBool[6] ? 0x02 : 00;
		vertFlags |= vertFlagsBool[7] ? 0x01 : 00;

		*((BYTE*)*currentCollideTriangle + 12) = (BYTE)(tPage<<4 | clut);
		*((BYTE*)*currentCollideTriangle + 14) = (BYTE)texFlags;
		*((BYTE*)*currentCollideTriangle + 15) = (BYTE)vertFlags;
	}


	ImGui::End();
}
