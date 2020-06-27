/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 Julian Xhokaxhiu                                   //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
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

#include "renderer.h"
#include "overlay.h"
#include <bx/file.h>
#include "cfg.h"
#include "field.h"

#define IMGUI_VIEW_ID 255

// Updates the mouse position
void Overlay::UpdateMousePos()
{
    ImGuiIO& io = ImGui::GetIO();

    // Set OS mouse position if requested (rarely used, only when ImGuiConfigFlags_NavEnableSetMousePos is enabled by user)
    if (io.WantSetMousePos)
    {
        POINT pos = { (int)io.MousePos.x, (int)io.MousePos.y };
        ::ClientToScreen(gameHwnd, &pos);
        ::SetCursorPos(pos.x, pos.y);
    }

    // Set mouse position
    io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
    POINT pos;
    if (HWND active_window = ::GetForegroundWindow())
        if (active_window == gameHwnd || ::IsChild(active_window, gameHwnd))
            if (::GetCursorPos(&pos) && ::ScreenToClient(gameHwnd, &pos))
                io.MousePos = ImVec2((float)pos.x, (float)pos.y);
}

// Sets the cursor icon
bool Overlay::UpdateMouseCursor()
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
        return false;

    ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
    if (imgui_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
    {
        // Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
        ::SetCursor(NULL);
    }
    else
    {
        // Show OS mouse cursor
        LPTSTR win32_cursor = IDC_ARROW;
        switch (imgui_cursor)
        {
        case ImGuiMouseCursor_Arrow:        win32_cursor = IDC_ARROW; break;
        case ImGuiMouseCursor_TextInput:    win32_cursor = IDC_IBEAM; break;
        case ImGuiMouseCursor_ResizeAll:    win32_cursor = IDC_SIZEALL; break;
        case ImGuiMouseCursor_ResizeEW:     win32_cursor = IDC_SIZEWE; break;
        case ImGuiMouseCursor_ResizeNS:     win32_cursor = IDC_SIZENS; break;
        case ImGuiMouseCursor_ResizeNESW:   win32_cursor = IDC_SIZENESW; break;
        case ImGuiMouseCursor_ResizeNWSE:   win32_cursor = IDC_SIZENWSE; break;
        case ImGuiMouseCursor_Hand:         win32_cursor = IDC_HAND; break;
        case ImGuiMouseCursor_NotAllowed:   win32_cursor = IDC_NO; break;
        }
        ::SetCursor(::LoadCursor(NULL, win32_cursor));
    }
    return true;
}

void Overlay::Update()
{
    ImGuiIO& io = ImGui::GetIO();
    IM_ASSERT(io.Fonts->IsBuilt() && "Font atlas not built! It is generally built by the renderer back-end. Missing call to renderer _NewFrame() function? e.g. ImGui_ImplOpenGL3_NewFrame().");

    // Block keys from reaching the game engine if the debug windows need input
    SetBlockKeysFromGame(io.WantCaptureKeyboard);

    // Setup display size (every frame to accommodate for window resizing)
    RECT rect;
    ::GetClientRect(gameHwnd, &rect);
    io.DisplaySize = ImVec2((float)(rect.right - rect.left), (float)(rect.bottom - rect.top));

    // Setup time step
    INT64 current_time;
    ::QueryPerformanceCounter((LARGE_INTEGER*)&current_time);
    io.DeltaTime = (float)(current_time - g_Time) / g_TicksPerSecond;
    g_Time = current_time;

    // Read keyboard modifiers inputs
    io.KeyCtrl = (::GetKeyState(VK_CONTROL) & 0x8000) != 0;
    io.KeyShift = (::GetKeyState(VK_SHIFT) & 0x8000) != 0;
    io.KeyAlt = (::GetKeyState(VK_MENU) & 0x8000) != 0;
    io.KeySuper = false;

    // Update OS mouse position
    UpdateMousePos();

    // Update OS mouse cursor with the cursor requested by imgui
    ImGuiMouseCursor mouse_cursor = io.MouseDrawCursor ? ImGuiMouseCursor_None : ImGui::GetMouseCursor();
    if (g_LastMouseCursor != mouse_cursor)
    {
        g_LastMouseCursor = mouse_cursor;
        UpdateMouseCursor();
    }
}

void Overlay::Render(ImDrawData* drawData)
{
    const ImGuiIO& io = ImGui::GetIO();
    const float width = io.DisplaySize.x;
    const float height = io.DisplaySize.y;

    bgfx::setViewName(IMGUI_VIEW_ID, "ImGui");
    bgfx::setViewMode(IMGUI_VIEW_ID, bgfx::ViewMode::Sequential);

    const bgfx::Caps* caps = bgfx::getCaps();
    {
        float ortho[16];
        bx::mtxOrtho(ortho, 0.0f, width, height, 0.0f, 0.0f, 1000.0f, 0.0f, caps->homogeneousDepth);
        bgfx::setViewTransform(IMGUI_VIEW_ID, NULL, ortho);
        bgfx::setViewRect(IMGUI_VIEW_ID, 0, 0, uint16_t(width), uint16_t(height));
    }

    for (int32_t ii = 0, num = drawData->CmdListsCount; ii < num; ++ii)
    {
        const ImDrawList* drawList = drawData->CmdLists[ii];
        uint32_t numVertices = (uint32_t)drawList->VtxBuffer.size();
        uint32_t numIndices = (uint32_t)drawList->IdxBuffer.size();

        bgfx::TransientVertexBuffer vb;
        if (bgfx::getAvailTransientVertexBuffer(drawList->VtxBuffer.Size, imguiVertexLayout)) {
            bgfx::allocTransientVertexBuffer(&vb, drawList->VtxBuffer.Size, imguiVertexLayout);
            memcpy(vb.data, drawList->VtxBuffer.Data, drawList->VtxBuffer.Size * sizeof(ImDrawVert));
        }

        bgfx::TransientIndexBuffer ib;
        if (bgfx::getAvailTransientIndexBuffer(drawList->IdxBuffer.Size)) {
            bgfx::allocTransientIndexBuffer(&ib, drawList->IdxBuffer.Size);
            memcpy(ib.data, drawList->IdxBuffer.Data, drawList->IdxBuffer.Size * sizeof(ImDrawIdx));
        }

        uint32_t offset = 0;
        for (ImDrawCmd cmd : drawList->CmdBuffer)
        {
            if (cmd.UserCallback)
            {
                cmd.UserCallback(drawList, &cmd);
            }
            else if (0 != cmd.ElemCount)
            {
                uint64_t state = 0
                    | BGFX_STATE_WRITE_RGB
                    | BGFX_STATE_WRITE_A
                    | BGFX_STATE_MSAA
                    ;

                bgfx::TextureHandle th = m_texture;
                bgfx::ProgramHandle program = m_program;

                if (NULL != cmd.TextureId)
                {
                    union { ImTextureID ptr; struct { bgfx::TextureHandle handle; uint8_t flags; uint8_t mip; } s; } texture = { cmd.TextureId };
                    state |= 0 != (texture.s.flags)
                        ? BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA)
                        : BGFX_STATE_NONE
                        ;
                    th = texture.s.handle;
                }
                else
                {
                    state |= BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA);
                }

                const uint16_t xx = uint16_t(bx::max(cmd.ClipRect.x, 0.0f));
                const uint16_t yy = uint16_t(bx::max(cmd.ClipRect.y, 0.0f));
                bgfx::setScissor(xx, yy
                    , uint16_t(bx::min(cmd.ClipRect.z, 65535.0f) - xx)
                    , uint16_t(bx::min(cmd.ClipRect.w, 65535.0f) - yy)
                );

                bgfx::setState(state);
                bgfx::setTexture(0, s_tex, th);
                bgfx::setVertexBuffer(0, &vb, 0, numVertices);
                bgfx::setIndexBuffer(&ib, offset, cmd.ElemCount);
                bgfx::submit(IMGUI_VIEW_ID, m_program);
            }

            offset += cmd.ElemCount;
        }
    }
}

bool Overlay::init(bgfx::ProgramHandle program, int width, int height)
{
    if (!IMGUI_CHECKVERSION())
        return false;
    if (!::QueryPerformanceFrequency((LARGE_INTEGER*)&g_TicksPerSecond))
        return false;
    if (!::QueryPerformanceCounter((LARGE_INTEGER*)&g_Time))
        return false;

    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui::SetNextWindowBgAlpha(0.3f);

    // Setup back-end capabilities flags
    ImGuiIO& io = ImGui::GetIO();
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)
    io.BackendPlatformName = "imgui_impl_win32";
    io.ImeWindowHandle = gameHwnd;

    // Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array that we will update during the application lifetime.
    io.KeyMap[ImGuiKey_Tab] = VK_TAB;
    io.KeyMap[ImGuiKey_LeftArrow] = VK_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = VK_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = VK_UP;
    io.KeyMap[ImGuiKey_DownArrow] = VK_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = VK_PRIOR;
    io.KeyMap[ImGuiKey_PageDown] = VK_NEXT;
    io.KeyMap[ImGuiKey_Home] = VK_HOME;
    io.KeyMap[ImGuiKey_End] = VK_END;
    io.KeyMap[ImGuiKey_Insert] = VK_INSERT;
    io.KeyMap[ImGuiKey_Delete] = VK_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = VK_BACK;
    io.KeyMap[ImGuiKey_Space] = VK_SPACE;
    io.KeyMap[ImGuiKey_Enter] = VK_RETURN;
    io.KeyMap[ImGuiKey_Escape] = VK_ESCAPE;
    io.KeyMap[ImGuiKey_KeyPadEnter] = VK_RETURN;
    io.KeyMap[ImGuiKey_A] = 'A';
    io.KeyMap[ImGuiKey_C] = 'C';
    io.KeyMap[ImGuiKey_V] = 'V';
    io.KeyMap[ImGuiKey_X] = 'X';
    io.KeyMap[ImGuiKey_Y] = 'Y';
    io.KeyMap[ImGuiKey_Z] = 'Z';

    io.DisplaySize = ImVec2(width, height);
    io.DeltaTime = 1.0f / 60.0f;
    io.IniFilename = NULL;

    bgfx::RendererType::Enum type = bgfx::getRendererType();
    m_program = program;

    s_tex = bgfx::createUniform("s_tex", bgfx::UniformType::Sampler);

    // Load fonts
    // Build texture atlas
    unsigned char* pixels;
    int twidth, theight;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &twidth, &theight);

    const bgfx::Memory* mem = bgfx::makeRef(pixels, twidth * theight * 4);
    m_texture = bgfx::createTexture2D(
        twidth
        , theight
        , false
        , 1
        , bgfx::TextureFormat::BGRA8
        , 0
        , mem
    );

    imguiVertexLayout
        .begin()
        .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
        .end();

    RegisterMouseListener(this);
    RegisterKeyListener(this);

    return true;
}

void Overlay::drawMainWindow() {
    if (!ImGui::Begin("Debugging Tools", &visible, ImGuiWindowFlags_::ImGuiWindowFlags_MenuBar))
    {
        ImGui::End();
        return;
    }

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Tools"))
        {
            ImGui::MenuItem("Field Debug", NULL, &field_debug_open);
            ImGui::MenuItem("ImGui Demo", NULL, &demo_open);
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    ImGui::Text("Select a debug tool from the menu.");
    ImGui::End();
}

void Overlay::draw()
{
    Update();
    ImGui::NewFrame();
    
    // This is a placeholder UI
    if (visible)
    {
        drawMainWindow();        
        if (field_debug_open) field_debug(&field_debug_open);
        if (demo_open) ImGui::ShowDemoWindow(&demo_open); // Useful to keep as GUI guide for now
    }
    
    // Due to ShowCursor having an internal counter instead of actually using the flag set, we need to make sure that the cursor actually changes state
    if (fullscreen)
    {
        CURSORINFO cursor{ sizeof(CURSORINFO) };
        GetCursorInfo(&cursor);
        bool cursorVisible = cursor.flags & CURSOR_SHOWING > 0;

        if (visible && !cursorVisible)
            while (ShowCursor(true) < 1);
        else if (!visible && cursorVisible)
            while (ShowCursor(false) > 0);
    }

    ImGui::Render();
    Render(ImGui::GetDrawData());
}

void Overlay::destroy()
{
    bgfx::destroy(s_tex);
    bgfx::destroy(m_texture);

    ImGui::DestroyContext();
}

void Overlay::MouseDown(MouseEventArgs& e)
{
    if (!ImGui::IsAnyMouseDown() && ::GetCapture() == NULL)
        ::SetCapture(gameHwnd);
    ImGui::GetIO().MouseDown[e.button - 1] = true;
}

void Overlay::MouseUp(MouseEventArgs& e)
{
    ImGui::GetIO().MouseDown[e.button - 1] = false;
    if (!ImGui::IsAnyMouseDown() && ::GetCapture() == gameHwnd)
        ::ReleaseCapture();
}

void Overlay::MouseWheel(MouseEventArgs& e)
{
    ImGui::GetIO().MouseWheel += e.delta;
}

void Overlay::MouseMove(MouseEventArgs& e)
{
    UpdateMouseCursor();
}

void Overlay::KeyUp(KeyEventArgs& e)
{
    if (e.keyValue == debug_ui_hotkey)
        visible = !visible;

    if (e.keyValue < 256)
        ImGui::GetIO().KeysDown[e.keyValue] = 0;
}

void Overlay::KeyDown(KeyEventArgs& e)
{
    if (e.keyValue < 256)
        ImGui::GetIO().KeysDown[e.keyValue] = 1;
}

void Overlay::KeyPress(KeyPressEventArgs& e)
{
    if (e.keyChar > 0 && e.keyChar < 0x10000)
        ImGui::GetIO().AddInputCharacterUTF16((unsigned short)e.keyChar);
}
