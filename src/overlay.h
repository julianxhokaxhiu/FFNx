/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2020 Marcin Gomulak                                     //
//    Copyright (C) 2023 Julian Xhokaxhiu                                   //
//    Copyright (C) 2023 Cosmos                                             //
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

#pragma once

#include <bgfx/bgfx.h>
#include <imgui.h>
#include "input.h"

class Overlay : public MouseListener, public KeyListener {
private:
	INT64 g_Time = 0;
	INT64  g_TicksPerSecond = 0;
	ImGuiMouseCursor g_LastMouseCursor = ImGuiMouseCursor_COUNT;
	bgfx::VertexLayout imguiVertexLayout;
	bgfx::UniformHandle s_tex;
	bgfx::TextureHandle m_texture;
	bgfx::ProgramHandle m_program;
	bool visible = true;
	bool field_debug_open = false;
	bool lighting_debug_open = false;
	bool world_debug_open = false;

	void UpdateMousePos();
	bool UpdateMouseCursor();
	void Update();
	void Render(ImDrawData* drawData);
public:
	bool init(bgfx::ProgramHandle program, int width, int height);
	void drawMainWindow();
	void draw();
	void destroy();
	void MouseDown(MouseEventArgs e);
	void MouseUp(MouseEventArgs e);
	void MouseWheel(MouseEventArgs e);
	void MouseMove(MouseEventArgs e);
	void KeyUp(KeyEventArgs e);
	void KeyDown(KeyEventArgs e);
	void KeyPress(KeyPressEventArgs e);
};
