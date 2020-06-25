#pragma once

#include <bgfx/bgfx.h>
#include <imgui/imgui.h>
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
	void UpdateMousePos();
	bool UpdateMouseCursor();
	void Update();
	void Render(ImDrawData* drawData);
public:
	bool init(bgfx::ProgramHandle program, int width, int height);
	void draw();
	void destroy();
	void MouseDown(MouseEventArgs& e);
	void MouseUp(MouseEventArgs& e);
	void MouseWheel(MouseEventArgs& e);
	void MouseMove(MouseEventArgs& e);
	void KeyUp(KeyEventArgs& e);
	void KeyDown(KeyEventArgs& e);
	void KeyPress(KeyPressEventArgs& e);
};
