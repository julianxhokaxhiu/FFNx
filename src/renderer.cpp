/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2022 Julian Xhokaxhiu                                   //
//    Copyright (C) 2022 Cosmos                                             //
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
#include "lighting.h"

Renderer newRenderer;
RendererCallbacks bgfxCallbacks;

// BGFX CALLBACKS
void RendererCallbacks::fatal(const char* _filePath, uint16_t _line, bgfx::Fatal::Enum _code, const char* _str)
{
    std::string error;

    switch (_code) {
    case bgfx::Fatal::Enum::DebugCheck: error = "Debug Check";
    case bgfx::Fatal::Enum::InvalidShader: error = "Invalid Shader";
    case bgfx::Fatal::Enum::UnableToInitialize: error = "Unable To Initialize";
    case bgfx::Fatal::Enum::UnableToCreateTexture: error = "Unable To Create Texture";
    case bgfx::Fatal::Enum::DeviceLost: error = "Device Lost";
    }

    ffnx_error("[%s] %s\n", error.c_str(), _str);
}

void RendererCallbacks::traceVargs(const char* _filePath, uint16_t _line, const char* _format, va_list _argList)
{
    if (renderer_debug)
    {
        char buffer[16 * 1024];

        va_list argListCopy;
        va_copy(argListCopy, _argList);
        vsnprintf(buffer, sizeof(buffer), _format, argListCopy);
        va_end(argListCopy);

        ffnx_trace("%s", buffer);
    }
}

uint32_t RendererCallbacks::cacheReadSize(uint64_t _id)
{
    // Return 0 if shader is not found.
    return 0;
}

bool RendererCallbacks::cacheRead(uint64_t _id, void* _data, uint32_t _size)
{
    // Shader is not found in cache, needs to be rebuilt.
    return false;
}

void RendererCallbacks::cacheWrite(uint64_t _id, const void* _data, uint32_t _size)
{
}

// PRIVATE

// Via https://stackoverflow.com/a/14375308
uint32_t Renderer::createBGRA(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    return ((b & 0xff) << 24) + ((g & 0xff) << 16) + ((r & 0xff) << 8) + (a & 0xff);
}

void Renderer::setCommonUniforms()
{
    internalState.VSFlags = {
        (float)internalState.bIsTLVertex,
        (float)internalState.blendMode,
        (float)internalState.bIsFBTexture,
        (float)internalState.bIsTexture
    };
    if (uniform_log) ffnx_trace("%s: VSFlags XYZW(isTLVertex %f, blendMode %f, isFBTexture %f, isTexture %f)\n", __func__, internalState.VSFlags[0], internalState.VSFlags[1], internalState.VSFlags[2], internalState.VSFlags[3]);

    internalState.FSAlphaFlags = {
        (float)internalState.alphaRef,
        (float)internalState.alphaFunc,
        (float)internalState.bDoAlphaTest,
        NULL
    };
    if (uniform_log) ffnx_trace("%s: FSAlphaFlags XYZW(inAlphaRef %f, inAlphaFunc %f, bDoAlphaTest %f, NULL)\n", __func__, internalState.FSAlphaFlags[0], internalState.FSAlphaFlags[1], internalState.FSAlphaFlags[2]);

    internalState.FSMiscFlags = {
        (float)internalState.bIsMovieFullRange,
        (float)internalState.bIsMovieYUV,
        (float)internalState.bModulateAlpha,
        (float)internalState.bIsMovie
    };
    if (uniform_log) ffnx_trace("%s: FSMiscFlags XYZW(isMovieFullRange %f, isMovieYUV %f, modulateAlpha %f, isMovie %f)\n", __func__, internalState.FSMiscFlags[0], internalState.FSMiscFlags[1], internalState.FSMiscFlags[2], internalState.FSMiscFlags[3]);

    internalState.FSHDRFlags = {
        (float)internalState.bIsHDR,
        (float)hdr_max_nits,
        NULL,
        NULL
    };
    if (uniform_log) ffnx_trace("%s: FSMiscFlags XYZW(isHDR %f, monitorNits %f, NULL, NULL)\n", __func__, internalState.FSHDRFlags[0], internalState.FSHDRFlags[1]);

    internalState.FSTexFlags = {
        (float)(internalState.texHandlers[RendererTextureSlot::TEX_NML].idx != bgfx::kInvalidHandle),
        (float)(internalState.texHandlers[RendererTextureSlot::TEX_PBR].idx != bgfx::kInvalidHandle),
        (float)(specularIblTexture.idx != bgfx::kInvalidHandle && diffuseIblTexture.idx != bgfx::kInvalidHandle && envBrdfTexture.idx != bgfx::kInvalidHandle),
        NULL
    };
    if (uniform_log) ffnx_trace("%s: FSTexFlags XYZW(isNmlTextureLoaded %f, isPbrTextureLoaded %f, isIblTextureLoaded %f, NULL)\n", __func__, internalState.FSTexFlags[0], internalState.FSTexFlags[1], internalState.FSTexFlags[2]);

    setUniform("VSFlags", bgfx::UniformType::Vec4, internalState.VSFlags.data());
    setUniform("FSAlphaFlags", bgfx::UniformType::Vec4, internalState.FSAlphaFlags.data());
    setUniform("FSMiscFlags", bgfx::UniformType::Vec4, internalState.FSMiscFlags.data());
    setUniform("FSHDRFlags", bgfx::UniformType::Vec4, internalState.FSHDRFlags.data());
    setUniform("FSTexFlags", bgfx::UniformType::Vec4, internalState.FSTexFlags.data());

    setUniform("d3dViewport", bgfx::UniformType::Mat4, internalState.d3dViewMatrix);
    setUniform("d3dProjection", bgfx::UniformType::Mat4, internalState.d3dProjectionMatrix);
    setUniform("worldView", bgfx::UniformType::Mat4, internalState.worldViewMatrix);
    setUniform("normalMatrix", bgfx::UniformType::Mat4, internalState.normalMatrix);
    setUniform("viewMatrix", bgfx::UniformType::Mat4, internalState.viewMatrix);
    setUniform("invViewMatrix", bgfx::UniformType::Mat4, internalState.invViewMatrix);
}

void Renderer::setLightingUniforms()
{
    auto lightingState = lighting.getLightingState();

    setUniform("lightingSettings", bgfx::UniformType::Vec4, lightingState.lightingSettings);
    setUniform("lightDirData", bgfx::UniformType::Vec4, lightingState.lightDirData);
    setUniform("lightData", bgfx::UniformType::Vec4, lightingState.lightData);
    setUniform("ambientLightData", bgfx::UniformType::Vec4, lightingState.ambientLightData);
    setUniform("shadowData", bgfx::UniformType::Vec4, lightingState.shadowData);
    setUniform("fieldShadowData", bgfx::UniformType::Vec4, lightingState.fieldShadowData);
    setUniform("materialData", bgfx::UniformType::Vec4, lightingState.materialData);
    setUniform("materialScaleData", bgfx::UniformType::Vec4, lightingState.materialScaleData);
    setUniform("lightingDebugData", bgfx::UniformType::Vec4, lightingState.lightingDebugData);
    setUniform("iblData", bgfx::UniformType::Vec4, lightingState.iblData);

    setUniform("lightViewProjMatrix", bgfx::UniformType::Mat4, lightingState.lightViewProjMatrix);
    setUniform("lightViewProjTexMatrix", bgfx::UniformType::Mat4, lightingState.lightViewProjTexMatrix);
    setUniform("lightInvViewProjTexMatrix", bgfx::UniformType::Mat4, lightingState.lightInvViewProjTexMatrix);
}

bgfx::RendererType::Enum Renderer::getUserChosenRenderer() {
    bgfx::RendererType::Enum ret;

    switch (renderer_backend)
    {
    case RENDERER_BACKEND_AUTO:
        ret = bgfx::RendererType::Count;
        break;
    case RENDERER_BACKEND_OPENGL:
        ret = bgfx::RendererType::OpenGL;
        break;
    case RENDERER_BACKEND_DIRECT3D9:
        ret = bgfx::RendererType::Direct3D9;
        break;
    case RENDERER_BACKEND_DIRECT3D11:
        ret = bgfx::RendererType::Direct3D11;
        break;
    case RENDERER_BACKEND_DIRECT3D12:
        ret = bgfx::RendererType::Direct3D12;
        break;
    case RENDERER_BACKEND_VULKAN:
        ret = bgfx::RendererType::Vulkan;
        break;
    default:
        ret = bgfx::RendererType::Noop;
        break;
    }

    return ret;
}

void Renderer::updateRendererShaderPaths()
{
    std::string shaderSuffix;

    switch (getCaps()->rendererType)
    {
    case bgfx::RendererType::OpenGL:
        currentRenderer = "OpenGL";
        shaderSuffix = ".gl";
        break;
    case bgfx::RendererType::Direct3D9:
        currentRenderer = "Direct3D9";
        shaderSuffix = ".d3d9";
        break;
    case bgfx::RendererType::Direct3D11:
        currentRenderer = "Direct3D11";
        shaderSuffix = ".d3d11";
        break;
    case bgfx::RendererType::Direct3D12:
        currentRenderer = "Direct3D12";
        shaderSuffix = ".d3d12";
        break;
    case bgfx::RendererType::Vulkan:
        currentRenderer = "Vulkan";
        shaderSuffix = ".vk";
        break;
    }

    vertexPathFlat += ".flat" + shaderSuffix + ".vert";
    fragmentPathFlat += ".flat" + shaderSuffix + ".frag";
    vertexPathSmooth += ".smooth" + shaderSuffix + ".vert";
    fragmentPathSmooth += ".smooth" + shaderSuffix + ".frag";
    vertexPostPath += shaderSuffix + ".vert";
    fragmentPostPath += shaderSuffix + ".frag";
    vertexOverlayPath += shaderSuffix + ".vert";
    fragmentOverlayPath += shaderSuffix + ".frag";
    vertexLightingPathFlat += ".flat" + shaderSuffix + ".vert";
    fragmentLightingPathFlat += ".flat" + shaderSuffix + ".frag";
    vertexLightingPathSmooth += ".smooth" + shaderSuffix + ".vert";
    fragmentLightingPathSmooth += ".smooth" + shaderSuffix + ".frag";
    vertexShadowMapPath += shaderSuffix + ".vert";
    fragmentShadowMapPath += shaderSuffix + ".frag";
    vertexFieldShadowPath += shaderSuffix + ".vert";
    fragmentFieldShadowPath += shaderSuffix + ".frag";
}

// Via https://dev.to/pperon/hello-bgfx-4dka
bgfx::ShaderHandle Renderer::getShader(const char* filePath)
{
    bgfx::ShaderHandle handle = BGFX_INVALID_HANDLE;

    FILE* file = fopen(filePath, "rb");

    if (file == NULL)
    {
        char tmp[1024]{ 0 };

        sprintf(tmp, "Oops! Something very bad happened.\n\nCould not find shader file:\n%s\n\nMake sure all the provided files are installed correctly.", filePath);

        MessageBoxA(gameHwnd, tmp, "Error", MB_ICONERROR | MB_OK);

        exit(1);
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    const bgfx::Memory* mem = bgfx::alloc(fileSize);
    fread(mem->data, 1, fileSize, file);
    fclose(file);

    handle = bgfx::createShader(mem);

    if (bgfx::isValid(handle))
    {
        bgfx::setName(handle, filePath);
    }

    return handle;
}

bgfx::UniformHandle Renderer::getUniform(std::string uniformName, bgfx::UniformType::Enum uniformType)
{
    bgfx::UniformHandle handle;
    auto ret = bgfxUniformHandles.find(uniformName);

    if (ret != bgfxUniformHandles.end())
    {
        handle = { (uint16_t)ret->second };
    }
    else
    {
        handle = bgfx::createUniform(uniformName.c_str(), uniformType);
        bgfxUniformHandles[uniformName] = handle.idx;
    }

    return handle;
}

bgfx::UniformHandle Renderer::setUniform(const char* uniformName, bgfx::UniformType::Enum uniformType, const void* uniformValue)
{
    bgfx::UniformHandle handle = getUniform(std::string(uniformName), uniformType);

    if (bgfx::isValid(handle))
    {
        bgfx::setUniform(handle, uniformValue);
    }

    return handle;
}

void Renderer::destroyUniforms()
{
    for (const auto& item : bgfxUniformHandles)
    {
        bgfx::UniformHandle handle = { item.second };

        if (bgfx::isValid(handle))
            bgfx::destroy(handle);
    }

    bgfxUniformHandles.clear();
}

void Renderer::destroyAll()
{
    destroyUniforms();

    for (auto& handle : internalState.texHandlers)
    {
        if (bgfx::isValid(handle))
            bgfx::destroy(handle);
    }

    bgfx::destroy(vertexBufferHandle);

    bgfx::destroy(indexBufferHandle);

    bgfx::destroy(backendFrameBuffer);

    bgfx::destroy(shadowMapFrameBuffer);

    for (auto& handle : backendProgramHandles)
    {
        if (bgfx::isValid(handle))
            bgfx::destroy(handle);
    }

    if (enable_devtools)
        overlay.destroy();
};

void Renderer::resetState()
{
    setBackgroundColor();

    doAlphaTest();
    doDepthTest();
    doDepthWrite();
    doScissorTest();
    setCullMode();
    setBlendMode();
    setAlphaRef();
    setInterpolationQualifier();
    isTLVertex();
    isYUV();
    isFullRange();
    isFBTexture();
    isTexture();
    doModulateAlpha();
    doTextureFiltering();
    isExternalTexture();
};

void Renderer::renderFrame()
{
    /*  y0    y2
     x0 +-----+ x2
        |    /|
        |   / |
        |  /  |
        | /   |
        |/    |
     x1 +-----+ x3
        y1    y3
    */

    // 0
    float x0 = preserve_aspect ? framebufferVertexOffsetX : 0.0f;
    float y0 = 0.0f;
    float u0 = 0.0f;
    float v0 = getCaps()->originBottomLeft ? 1.0f : 0.0f;
    // 1
    float x1 = x0;
    float y1 = game_height;
    float u1 = u0;
    float v1 = getCaps()->originBottomLeft ? 0.0f : 1.0f;
    // 2
    float x2 = x0 + (preserve_aspect ? framebufferVertexWidth : game_width);
    float y2 = y0;
    float u2 = 1.0f;
    float v2 = v0;
    // 3
    float x3 = x2;
    float y3 = y1;
    float u3 = u2;
    float v3 = v1;

    struct nvertex vertices[] = {
        {x0, y0, 1.0f, 1.0f, 0xff000000, 0, u0, v0},
        {x1, y1, 1.0f, 1.0f, 0xff000000, 0, u1, v1},
        {x2, y2, 1.0f, 1.0f, 0xff000000, 0, u2, v2},
        {x3, y3, 1.0f, 1.0f, 0xff000000, 0, u3, v3},
    };
    WORD indices[] = {
        0, 1, 2,
        1, 3, 2
    };

    backendProgram = RendererProgram::POSTPROCESSING;
    backendViewId++;
    {
        bool needsToDraw = internalState.bHasDrawBeenDone;

        if (internalState.bHasDrawBeenDone)
            useTexture(
                bgfx::getTexture(backendFrameBuffer).idx
            );
        else
            useTexture(0);

        setClearFlags(true, true);

        bindVertexBuffer(vertices, 0, 4);
        bindIndexBuffer(indices, 6);

        setBlendMode(RendererBlendMode::BLEND_DISABLED);
        setPrimitiveType();

        if (needsToDraw) draw();

        setBlendMode();
    }
};

void Renderer::printMatrix(char* name, float* mat)
{
    ffnx_trace("%s: 0 [%f, %f, %f, %f]\n", name, mat[0], mat[1], mat[2], mat[3]);
    ffnx_trace("%s: 1 [%f, %f, %f, %f]\n", name, mat[4], mat[5], mat[6], mat[7]);
    ffnx_trace("%s: 2 [%f, %f, %f, %f]\n", name, mat[8], mat[9], mat[10], mat[11]);
    ffnx_trace("%s: 3 [%f, %f, %f, %f]\n", name, mat[12], mat[13], mat[14], mat[15]);
};

bool Renderer::doesItFitInMemory(size_t size)
{
    if (size <= 0) ffnx_glitch("Unexpected texture size while checking if it fits in memory.\n");

    // We need to check this value as much as in real time as possible, to avoid possible crashes
    GlobalMemoryStatusEx(&last_ram_state);

    return size < last_ram_state.ullAvailVirtual;
}

void Renderer::recalcInternals()
{
    long scale_factor = internal_resolution_scale;

    viewWidth = window_size_x;
    viewHeight = window_size_y;

    // aspect correction
    if (preserve_aspect && viewWidth * 3 != viewHeight * 4)
    {
        if (viewHeight * 4 > viewWidth * 3)
        {
            viewOffsetY = viewHeight - (viewWidth * 3) / 4;
            viewHeight = (viewWidth * 3) / 4;

            y_offset = viewOffsetY;
        }
        else if (viewWidth * 3 > viewHeight * 4)
        {
            viewOffsetX = (viewWidth - (viewHeight * 4) / 3) / 2;
            viewWidth = (viewHeight * 4) / 3;

            x_offset = viewOffsetX;
        }
    }

    // If internal_resolution_scale from settings is less than one, calculate the closest fit for the output resolution, otherwise use the value directly
    if (internal_resolution_scale < 1)
    {
        long scaleW = ::round(viewWidth / (float)game_width);
        long scaleH = ::round(viewHeight / (float)game_height);

        if (scaleH > scaleW) scaleW = scaleH;
        if (scaleW > internal_resolution_scale) scale_factor = scaleW;
        if (scale_factor < 1) scale_factor = 1;
    }

    // Use the set or calculated scaling factor to determine the width and height of the framebuffer according to the original resolution
    framebufferWidth = game_width * scale_factor;
    framebufferHeight = game_height * scale_factor;

    framebufferVertexWidth = (viewWidth * game_width) / window_size_x;
    framebufferVertexOffsetX = (game_width - framebufferVertexWidth) / 2;

    // Let the user know about chosen resolutions
    ffnx_info("Original resolution %ix%i, Scaling factor %i, Internal resolution %ix%i, Output resolution %ix%i\n", game_width, game_height, scale_factor, framebufferWidth, framebufferHeight, window_size_x, window_size_y);
}

void Renderer::prepareFramebuffer()
{
    // If already existing, destroy
    if (bgfx::isValid(backendFrameBuffer))
        bgfx::destroy(backendFrameBuffer);

    uint64_t fbFlags = BGFX_TEXTURE_RT;

    if (enable_antialiasing > 0)
    {
        if (enable_antialiasing <= 2)
            fbFlags = BGFX_TEXTURE_RT_MSAA_X2;
        else if (enable_antialiasing <= 4)
            fbFlags = BGFX_TEXTURE_RT_MSAA_X4;
        else if (enable_antialiasing <= 8)
            fbFlags = BGFX_TEXTURE_RT_MSAA_X8;
        else if (enable_antialiasing <= 16)
            fbFlags = BGFX_TEXTURE_RT_MSAA_X16;
    }

    backendFrameBufferRT[0] = bgfx::createTexture2D(
        framebufferWidth,
        framebufferHeight,
        false,
        1,
        internalState.bIsHDR ? bgfx::TextureFormat::RGB10A2 : bgfx::TextureFormat::RGBA16,
        fbFlags | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP
    );

    backendFrameBufferRT[1] = bgfx::createTexture2D(
        framebufferWidth,
        framebufferHeight,
        false,
        1,
        bgfx::TextureFormat::D32F,
        fbFlags | BGFX_TEXTURE_RT_WRITE_ONLY | BGFX_TEXTURE_BLIT_DST
    );

    backendFrameBuffer = bgfx::createFrameBuffer(
        backendFrameBufferRT.size(),
        backendFrameBufferRT.data(),
        true
    );

    backupDepthTexture = bgfx::createTexture2D(
        framebufferWidth,
        framebufferHeight,
        false,
        1,
        bgfx::TextureFormat::D32F,
        fbFlags | BGFX_TEXTURE_RT_WRITE_ONLY | BGFX_TEXTURE_BLIT_DST
    );
}

void Renderer::bindTextures()
{
    if (!internalState.bTexturesBound)
    {
        for (uint32_t idx = RendererTextureSlot::TEX_Y; idx < RendererTextureSlot::COUNT; idx++)
        {
            bgfx::TextureHandle handle = internalState.texHandlers[idx];

            if (bgfx::isValid(handle))
            {
                uint32_t flags = 0;

                switch(idx)
                {
                    case RendererTextureSlot::TEX_Y:
                    case RendererTextureSlot::TEX_U:
                    case RendererTextureSlot::TEX_V:
                        if (!internalState.bIsMovie && idx > RendererTextureSlot::TEX_Y) handle = BGFX_INVALID_HANDLE;

                        if (backendProgram == RendererProgram::POSTPROCESSING)
                        {
                            flags = BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_W_CLAMP | BGFX_SAMPLER_MIN_ANISOTROPIC | BGFX_SAMPLER_MAG_ANISOTROPIC;
                        }
                        else
                        {
                            if (internalState.bIsMovie) flags |= BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_W_CLAMP;

                            if (!internalState.bDoTextureFiltering || !internalState.bIsExternalTexture) flags |= BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIP_POINT;
                        }
                        break;
                    case RendererTextureSlot::TEX_S:
                    case RendererTextureSlot::TEX_D:
                        // Specially handled, move on
                        continue;
                    default:
                        break;
                }

                if (flags == 0) flags = UINT32_MAX;

                bgfx::setTexture(idx, getUniform("tex_" + std::to_string(idx), bgfx::UniformType::Sampler), handle, flags);
            }
        }

        internalState.bTexturesBound = true;
    }
}

// PUBLIC

void Renderer::init()
{
    recalcInternals();

    // Init renderer
    bgfxInit.platformData.nwh = gameHwnd;
    bgfxInit.type = getUserChosenRenderer();
    bgfxInit.resolution.width = window_size_x;
    bgfxInit.resolution.height = window_size_y;

    if (enable_anisotropic)
        bgfxInit.resolution.reset |= BGFX_RESET_MAXANISOTROPY;

    if (enable_vsync)
        bgfxInit.resolution.reset |= BGFX_RESET_VSYNC;

    bgfxInit.debug = renderer_debug;
    bgfxInit.callback = &bgfxCallbacks;

    if (!bgfx::init(bgfxInit)) exit(1);

    // If HDR support is present, make use of it
    if (getCaps()->supported & BGFX_CAPS_HDR10)
    {
        internalState.bIsHDR = true;

        bgfxInit.resolution.reset |= BGFX_RESET_HDR10;
        bgfxInit.resolution.format = bgfx::TextureFormat::RGB10A2;

        if (hdr_max_nits <= 0) {
            short idx = 0;
            while (!getCaps()->outDeviceInfo[idx].isHDR10) idx++;
            hdr_max_nits = getCaps()->outDeviceInfo[idx].maxFullFrameLuminance;
        }

        bgfx::reset(window_size_x, window_size_y, bgfxInit.resolution.reset, bgfxInit.resolution.format);
    }

    internalState.texHandlers.resize(RendererTextureSlot::COUNT, BGFX_INVALID_HANDLE);

    updateRendererShaderPaths();

    bx::mtxOrtho(
        internalState.backendProjMatrix,
        0.0f,
        game_width,
        game_height,
        0.0f,
        getCaps()->homogeneousDepth ? -1.0f : 0.0f,
        1.0f,
        0.0,
        getCaps()->homogeneousDepth
    );

    prepareFramebuffer();

    prepareShadowMap();

    // Create Program
    backendProgramHandles[RendererProgram::POSTPROCESSING] = bgfx::createProgram(
        getShader(vertexPostPath.c_str()),
        getShader(fragmentPostPath.c_str()),
        true
    );

    backendProgramHandles[RendererProgram::FLAT] = bgfx::createProgram(
        getShader(vertexPathFlat.c_str()),
        getShader(fragmentPathFlat.c_str()),
        true
    );

    backendProgramHandles[RendererProgram::SMOOTH] = bgfx::createProgram(
        getShader(vertexPathSmooth.c_str()),
        getShader(fragmentPathSmooth.c_str()),
        true
    );

    backendProgramHandles[RendererProgram::LIGHTING_FLAT] = bgfx::createProgram(
        getShader(vertexLightingPathFlat.c_str()),
        getShader(fragmentLightingPathFlat.c_str()),
        true
    );

    backendProgramHandles[RendererProgram::LIGHTING_SMOOTH] = bgfx::createProgram(
        getShader(vertexLightingPathSmooth.c_str()),
        getShader(fragmentLightingPathSmooth.c_str()),
        true
    );

    backendProgramHandles[RendererProgram::FIELD_SHADOW] = bgfx::createProgram(
        getShader(vertexFieldShadowPath.c_str()),
        getShader(fragmentFieldShadowPath.c_str()),
        true
    );

    backendProgramHandles[RendererProgram::SHADOW_MAP] = bgfx::createProgram(
        getShader(vertexShadowMapPath.c_str()),
        getShader(fragmentShadowMapPath.c_str()),
        true
    );

    vertexLayout
        .begin()
        .add(bgfx::Attrib::Position, 4, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
        .end();

    bgfx::setDebug(BGFX_DEBUG_TEXT);

    bgfx::frame();

    if (enable_devtools)
    {
        backendProgramHandles[RendererProgram::OVERLAY] = bgfx::createProgram(
            getShader(vertexOverlayPath.c_str()),
            getShader(fragmentOverlayPath.c_str()),
            true
        );
        overlay.init(backendProgramHandles[RendererProgram::OVERLAY], window_size_x, window_size_y);
    }

    // Init Lighting
    lighting.init();

    // Set defaults
    show();
};

void Renderer::reset()
{
    recalcInternals();

    prepareFramebuffer();

    if(!ff8 && enable_lighting) prepareShadowMap();

    bgfx::reset(window_size_x, window_size_y, bgfxInit.resolution.reset, bgfxInit.resolution.format);
}

void Renderer::prepareShadowMap()
{
    if (bgfx::isValid(shadowMapFrameBuffer))
        bgfx::destroy(shadowMapFrameBuffer);

    auto shadowMapResolution = lighting.getShadowMapResolution();
    shadowMapTexture = bgfx::createTexture2D(
        shadowMapResolution,
        shadowMapResolution,
        false,
        1,
        bgfx::TextureFormat::D32F,
        BGFX_TEXTURE_RT | BGFX_SAMPLER_COMPARE_LEQUAL | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_W_CLAMP
    );

    shadowMapFrameBuffer = bgfx::createFrameBuffer(
        1,
        &shadowMapTexture,
        true
    );
}

void Renderer::prepareSpecularIbl(char* fullpath)
{
    if (bgfx::isValid(specularIblTexture))
        bgfx::destroy(specularIblTexture);

    uint32_t width, height, mipCount = 0;
    specularIblTexture = createTextureHandle(fullpath, &width, &height, &mipCount, false);
    lighting.setIblMipCount(mipCount);
}

void Renderer::prepareDiffuseIbl(char* fullpath)
{
    if (bgfx::isValid(diffuseIblTexture))
        bgfx::destroy(diffuseIblTexture);

    uint32_t width, height, mipCount = 0;
    diffuseIblTexture = createTextureHandle(fullpath, &width, &height, &mipCount, false);
}

void Renderer::prepareEnvBrdf()
{
    static char fullpath[MAX_PATH];

    if (bgfx::isValid(envBrdfTexture))
        bgfx::destroy(envBrdfTexture);

    sprintf(fullpath, "%s/%s/ibl/envBrdf.dds", basedir, external_lighting_path.c_str());

    uint32_t width, height, mipCount = 0;
    envBrdfTexture = createTextureHandle(fullpath, &width, &height, &mipCount, false);
}

void Renderer::shutdown()
{
    destroyAll();

    bgfx::shutdown();
}

void Renderer::clearShadowMap()
{
    bgfx::setViewClear(0, BGFX_CLEAR_DEPTH, internalState.clearColorValue, 1.0f, 0);
    bgfx::touch(0);
}

void Renderer::drawToShadowMap()
{
    if (trace_all || trace_renderer) ffnx_trace("Renderer::%s with backendProgram %d\n", __func__, backendProgram);

    // Lighting state
    auto lightingState = lighting.getLightingState();

    // Set view to render in the framebuffer
    bgfx::setViewFrameBuffer(0, shadowMapFrameBuffer);

    // Set current view rect
    int shadowMapResolution = lighting.getShadowMapResolution();
    bgfx::setViewRect(0, 0, 0, shadowMapResolution, shadowMapResolution);

    // Set current view transform
    bgfx::setViewTransform(0, lightingState.lightViewMatrix, lightingState.lightProjMatrix);

    // Set uniforms
    setLightingUniforms();
    setCommonUniforms();

    // Bind textures in pipeline
    bindTextures();

    // Set state
    internalState.state = BGFX_STATE_DEPTH_TEST_LEQUAL | BGFX_STATE_WRITE_Z;

    // Face culling
    if (lighting.isShadowFaceCullingEnabled())
    {
        switch (internalState.cullMode)
        {
        case RendererCullMode::FRONT: internalState.state |= BGFX_STATE_CULL_CCW;
        case RendererCullMode::BACK: internalState.state |= BGFX_STATE_CULL_CW;
        }
    }
    bgfx::setState(internalState.state);

    bgfx::submit(0, backendProgramHandles[RendererProgram::SHADOW_MAP], 0, BGFX_DISCARD_NONE);
};

void Renderer::drawWithLighting(bool isCastShadow)
{
    if (trace_all || trace_renderer) ffnx_trace("Renderer::%s with backendProgram %d\n", __func__, backendProgram);

    // Draw to shadowmap
    if (isCastShadow) newRenderer.drawToShadowMap();

    // Set lighting program
    backendProgram = backendProgram == SMOOTH ? LIGHTING_SMOOTH : LIGHTING_FLAT;

    // Re-Bind shadow map with comparison sampler
    bgfx::setTexture(RendererTextureSlot::TEX_S, getUniform("tex_" + std::to_string(RendererTextureSlot::TEX_S), bgfx::UniformType::Sampler), bgfx::getTexture(shadowMapFrameBuffer));

    // Bind specular IBL cubemap
    if (bgfx::isValid(specularIblTexture))
    {
        bgfx::setTexture(RendererTextureSlot::TEX_IBL_SPEC, getUniform("tex_" + std::to_string(RendererTextureSlot::TEX_IBL_SPEC), bgfx::UniformType::Sampler), specularIblTexture);
    }

    // Bind diffuse IBL cubemap
    if (bgfx::isValid(diffuseIblTexture))
    {
        bgfx::setTexture(RendererTextureSlot::TEX_IBL_DIFF, getUniform("tex_" + std::to_string(RendererTextureSlot::TEX_IBL_DIFF), bgfx::UniformType::Sampler), diffuseIblTexture);
    }

    // Bind environment BRDF texture
    if (bgfx::isValid(envBrdfTexture))
    {
        bgfx::setTexture(RendererTextureSlot::TEX_BRDF, getUniform("tex_" + std::to_string(RendererTextureSlot::TEX_BRDF), bgfx::UniformType::Sampler), envBrdfTexture, BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);
    }

    // Draw with lighting
    draw(isCastShadow);
}

void Renderer::backupDepthBuffer()
{
    backendViewId++;
    bgfx::setViewClear(backendViewId, BGFX_CLEAR_NONE, internalState.clearColorValue, 1.0f);
    bgfx::touch(backendViewId);
    bgfx::blit(backendViewId, backupDepthTexture, 0, 0, bgfx::getTexture(backendFrameBuffer, 1), 0, 0, framebufferWidth, framebufferHeight);
    backendViewId++;
    bgfx::setViewClear(backendViewId, BGFX_CLEAR_NONE, internalState.clearColorValue, 1.0f);
    bgfx::touch(backendViewId);
}

void Renderer::recoverDepthBuffer()
{
    backendViewId++;
    bgfx::setViewClear(backendViewId, BGFX_CLEAR_NONE, internalState.clearColorValue, 1.0f);
    bgfx::touch(backendViewId);
    bgfx::blit(backendViewId, bgfx::getTexture(backendFrameBuffer, 1), 0, 0, backupDepthTexture, 0, 0, framebufferWidth, framebufferHeight);
    backendViewId++;
    bgfx::setViewClear(backendViewId, BGFX_CLEAR_NONE, internalState.clearColorValue, 1.0f);
    bgfx::touch(backendViewId);
}

void Renderer::drawFieldShadow()
{
    backendProgram = RendererProgram::FIELD_SHADOW;

    // Re-Bind shadow map with comparison sampler
    bgfx::setTexture(RendererTextureSlot::TEX_S, getUniform("tex_" + std::to_string(RendererTextureSlot::TEX_S), bgfx::UniformType::Sampler), bgfx::getTexture(shadowMapFrameBuffer));

    // Re-Bind shadow map for direct depth sampling
    bgfx::setTexture(RendererTextureSlot::TEX_D, getUniform("tex_" + std::to_string(RendererTextureSlot::TEX_D), bgfx::UniformType::Sampler), bgfx::getTexture(shadowMapFrameBuffer), BGFX_TEXTURE_RT | BGFX_SAMPLER_POINT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_W_CLAMP);

    draw();
}

void Renderer::draw(bool uniformsAlreadyAttached)
{
    if (trace_all || trace_renderer) ffnx_trace("Renderer::%s with backendProgram %d\n", __func__, backendProgram);

    // Set current view rect
    if (backendProgram == RendererProgram::POSTPROCESSING)
        bgfx::setViewRect(backendViewId, 0, 0, window_size_x, window_size_y);
    else {
        // Set view to render in the framebuffer
        bgfx::setViewFrameBuffer(backendViewId, backendFrameBuffer);

        bgfx::setViewRect(backendViewId, 0, 0, framebufferWidth, framebufferHeight);

        if (internalState.bDoScissorTest) bgfx::setScissor(scissorOffsetX, scissorOffsetY, scissorWidth, scissorHeight);
    }

    // Set current view transform
    bgfx::setViewTransform(backendViewId, NULL, internalState.backendProjMatrix);

    // Skip uniform attachment as it has been done already
    if (!uniformsAlreadyAttached)
    {
        setCommonUniforms();
        setLightingUniforms();
    }

    // Bind textures in pipeline
    bindTextures();

    // Set state
    {
        internalState.state = BGFX_STATE_LINEAA | BGFX_STATE_MSAA | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A;

        switch (internalState.cullMode)
        {
        case RendererCullMode::FRONT: internalState.state |= BGFX_STATE_CULL_CW;
        case RendererCullMode::BACK: internalState.state |= BGFX_STATE_CULL_CCW;
        }

        switch (internalState.blendMode)
        {
        case RendererBlendMode::BLEND_AVG:
            internalState.state |= BGFX_STATE_BLEND_EQUATION(BGFX_STATE_BLEND_EQUATION_ADD);
            internalState.state |= BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA);
            break;
        case RendererBlendMode::BLEND_ADD:
            internalState.state |= BGFX_STATE_BLEND_EQUATION(BGFX_STATE_BLEND_EQUATION_ADD);
            internalState.state |= BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_ONE);
            break;
        case RendererBlendMode::BLEND_SUB:
            internalState.state |= BGFX_STATE_BLEND_EQUATION(BGFX_STATE_BLEND_EQUATION_REVSUB);
            internalState.state |= BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_ONE);
            break;
        case RendererBlendMode::BLEND_25P:
            internalState.state |= BGFX_STATE_BLEND_EQUATION(BGFX_STATE_BLEND_EQUATION_ADD);
            internalState.state |= BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_ONE);
            break;
        case RendererBlendMode::BLEND_NONE:
            internalState.state |= BGFX_STATE_BLEND_EQUATION(BGFX_STATE_BLEND_EQUATION_ADD);
            if (internalState.bIsExternalTexture && !ff8) internalState.state |= BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA);
            else internalState.state |= BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_ZERO);
            break;
        }

        switch (internalState.primitiveType)
        {
        case RendererPrimitiveType::PT_LINES:
            internalState.state |= BGFX_STATE_PT_LINES;
            break;
        case RendererPrimitiveType::PT_POINTS:
            internalState.state |= BGFX_STATE_PT_POINTS;
            break;
        }

        if (internalState.bDoDepthTest) internalState.state |= BGFX_STATE_DEPTH_TEST_LEQUAL;

        if (internalState.bDoDepthWrite) internalState.state |= BGFX_STATE_WRITE_Z;
    }
    bgfx::setState(internalState.state);

    bgfx::submit(backendViewId, backendProgramHandles[backendProgram]);

    internalState.bHasDrawBeenDone = true;
    internalState.bTexturesBound = false;
};

void Renderer::drawOverlay()
{
    if (enable_devtools)
        overlay.draw();
}

void Renderer::show()
{
    // Reset internal state
    resetState();

    renderFrame();

    bgfx::update(
        vertexBufferHandle,
        0,
        bgfx::copy(
            vertexBufferData.data(),
            vectorSizeOf(vertexBufferData)
        )
    );

    bgfx::update(
        indexBufferHandle,
        0,
        bgfx::copy(
            indexBufferData.data(),
            vectorSizeOf(indexBufferData)
        )
    );

    bgfx::frame(doCaptureFrame);

    if (trace_all || trace_renderer) ffnx_trace("Renderer::%s\n", __func__);

    bgfx::dbgTextClear();

    backendViewId = 1;

    vertexBufferData.clear();
    vertexBufferData.shrink_to_fit();

    indexBufferData.clear();
    indexBufferData.shrink_to_fit();

    bgfx::setViewMode(backendViewId, bgfx::ViewMode::Sequential);
}

void Renderer::printText(uint16_t x, uint16_t y, uint32_t color, const char* text)
{
    bgfx::dbgTextPrintf(
        x,
        y,
        color,
        text
    );
}

void Renderer::toggleCaptureFrame()
{
    doCaptureFrame = !doCaptureFrame;
}

const bgfx::Caps* Renderer::getCaps()
{
    return bgfx::getCaps();
};

const bgfx::Stats* Renderer::getStats()
{
    return bgfx::getStats();
}

void Renderer::bindVertexBuffer(struct nvertex* inVertex, vector3<float>* normals, uint32_t inCount)
{
    if (!bgfx::isValid(vertexBufferHandle)) vertexBufferHandle = bgfx::createDynamicVertexBuffer(inCount, vertexLayout, BGFX_BUFFER_ALLOW_RESIZE);

    uint32_t currentOffset = vertexBufferData.size();

    for (uint32_t idx = 0; idx < inCount; idx++)
    {
        vertexBufferData.push_back(Vertex());

        vertexBufferData[currentOffset + idx].x = inVertex[idx]._.x;
        vertexBufferData[currentOffset + idx].y = inVertex[idx]._.y;
        vertexBufferData[currentOffset + idx].z = inVertex[idx]._.z;
        vertexBufferData[currentOffset + idx].w = ( ::isinf(inVertex[idx].color.w) ? 1.0f : inVertex[idx].color.w );
        vertexBufferData[currentOffset + idx].bgra = inVertex[idx].color.color;
        vertexBufferData[currentOffset + idx].u = inVertex[idx].u;
        vertexBufferData[currentOffset + idx].v = inVertex[idx].v;

        if (normals)
        {
            vertexBufferData[currentOffset + idx].nx = normals[idx].x;
            vertexBufferData[currentOffset + idx].ny = normals[idx].y;
            vertexBufferData[currentOffset + idx].nz = normals[idx].z;
        }

        if (vertex_log && idx == 0) ffnx_trace("%s: %u [XYZW(%f, %f, %f, %f), BGRA(%08x), UV(%f, %f)]\n", __func__, idx, vertexBufferData[currentOffset + idx].x, vertexBufferData[currentOffset + idx].y, vertexBufferData[currentOffset + idx].z, vertexBufferData[currentOffset + idx].w, vertexBufferData[currentOffset + idx].bgra, vertexBufferData[currentOffset + idx].u, vertexBufferData[currentOffset + idx].v);
        if (vertex_log && idx == 1) ffnx_trace("%s: See the rest on RenderDoc.\n", __func__);
    }

    bgfx::setVertexBuffer(0, vertexBufferHandle, currentOffset, inCount);
};

void Renderer::bindIndexBuffer(WORD* inIndex, uint32_t inCount)
{
    if (!bgfx::isValid(indexBufferHandle)) indexBufferHandle = bgfx::createDynamicIndexBuffer(inCount, BGFX_BUFFER_ALLOW_RESIZE);

    uint32_t currentOffset = indexBufferData.size();

    for (uint32_t idx = 0; idx < inCount; idx++)
    {
        indexBufferData.push_back(inIndex[idx]);
    }

    bgfx::setIndexBuffer(indexBufferHandle, currentOffset, inCount);
};

void Renderer::setScissor(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    scissorOffsetX = getInternalCoordX(x);
    scissorOffsetY = getInternalCoordY(y);
    scissorWidth = getInternalCoordX(width);
    scissorHeight = getInternalCoordY(height);
}

void Renderer::setClearFlags(bool doClearColor, bool doClearDepth)
{
    if (trace_all || trace_renderer) ffnx_trace("Renderer::%s clearColor=%d,clearDepth=%d\n", __func__, doClearColor, doClearDepth);

    uint16_t clearFlags = BGFX_CLEAR_NONE;

    if (doClearColor)
        clearFlags |= BGFX_CLEAR_COLOR;

    if (doClearDepth)
        clearFlags |= BGFX_CLEAR_DEPTH;

    bgfx::setViewClear(backendViewId, clearFlags, internalState.clearColorValue, 1.0f);
    bgfx::touch(backendViewId);

    internalState.bHasDrawBeenDone = false;
}

void Renderer::setBackgroundColor(float r, float g, float b, float a)
{
    internalState.clearColorValue = createBGRA(r * 255, g * 255, b * 255, a * 255);
}

uint32_t Renderer::createTexture(uint8_t* data, size_t width, size_t height, int stride, RendererTextureType type, bool isSrgb)
{
    bgfx::TextureHandle ret = FFNX_RENDERER_INVALID_HANDLE;

    bgfx::TextureFormat::Enum texFormat = bgfx::TextureFormat::R8;
    bimg::TextureFormat::Enum imgFormat = bimg::TextureFormat::R8;

    if (type == RendererTextureType::BGRA)
    {
        texFormat = bgfx::TextureFormat::BGRA8;
        imgFormat = bimg::TextureFormat::BGRA8;
    }

    bimg::TextureInfo texInfo;
    bimg::imageGetSize(&texInfo, width, height, 0, false, false, 1, imgFormat);

    // If the texture we are going to create does not fit in memory, return an empty one.
    // Will prevent the game from crashing, while allowing the player to not loose its progress.
    if (doesItFitInMemory(texInfo.storageSize) && (data != NULL))
    {
        const bgfx::Memory* mem = bgfx::copy(data, texInfo.storageSize);

        uint64_t flags = BGFX_SAMPLER_NONE;

        if (isSrgb) flags |= BGFX_TEXTURE_SRGB;
        else flags |= BGFX_TEXTURE_NONE;

        ret = bgfx::createTexture2D(
            width,
            height,
            false,
            1,
            texFormat,
            flags,
            stride > 0 ? NULL : mem
        );

        if (stride > 0)
            bgfx::updateTexture2D(
                ret,
                0,
                0,
                0,
                0,
                width,
                height,
                mem,
                stride
            );

        if (trace_all || trace_renderer) ffnx_trace("Renderer::%s: %u => %ux%u from data with stride %u\n", __func__, ret.idx, width, height, stride);
    }

    return ret.idx;
};

uint32_t Renderer::createTexture(char* filename, uint32_t* width, uint32_t* height, uint32_t* mipCount, bool isSrgb)
{
    bgfx::TextureHandle handle = createTextureHandle(filename, width, height, mipCount, isSrgb);
    return handle.idx;
}

bimg::ImageContainer* Renderer::createImageContainer(const char* filename, bimg::TextureFormat::Enum targetFormat)
{
    FILE* file = fopen(filename, "rb");
    bimg::ImageContainer* img = nullptr;

    if (file)
    {
        size_t filesize = 0;
        char* buffer = nullptr;

        fseek(file, 0, SEEK_END);
        filesize = ftell(file);

        if (doesItFitInMemory(filesize + 1))
        {
            buffer = (char*)driver_malloc(filesize + 1);
            fseek(file, 0, SEEK_SET);
            fread(buffer, filesize, 1, file);
        }

        fclose(file);

        if (buffer != nullptr)
        {
            img = bimg::imageParse(&defaultAllocator, buffer, filesize + 1);

            driver_free(buffer);
        }
    }

    if (img && targetFormat != bimg::TextureFormat::Enum::UnknownDepth && targetFormat != img->m_format)
    {
        if (trace_all || trace_renderer) ffnx_trace("Renderer::%s: convert image to format %d\n", __func__, targetFormat);

        bimg::ImageContainer* converted = bimg::imageConvert(&defaultAllocator, targetFormat, *img);

        bimg::imageFree(img);

        img = converted;
    }

    return img;
}

bgfx::TextureHandle Renderer::createTextureHandle(char* filename, uint32_t* width, uint32_t* height, uint32_t* mipCount, bool isSrgb)
{
    bgfx::TextureHandle ret = BGFX_INVALID_HANDLE;
    bimg::ImageContainer* img = createImageContainer(filename);

    if (img != nullptr)
    {
        if (gl_check_texture_dimensions(img->m_width, img->m_height, filename) && doesItFitInMemory(img->m_size))
        {
            uint64_t flags = BGFX_SAMPLER_NONE;

            if (isSrgb) flags |= BGFX_TEXTURE_SRGB;
            else flags |= BGFX_TEXTURE_NONE;

            const bgfx::Memory* mem = bgfx::makeRef(img->m_data, img->m_size, RendererReleaseImageContainer, img);
            if (img->m_cubeMap)
            {
                ret = bgfx::createTextureCube(
                    img->m_width,
                    1 < img->m_numMips,
                    img->m_numLayers,
                    bgfx::TextureFormat::Enum(img->m_format),
                    flags,
                    mem
                );
            }
            else
            {

                ret = bgfx::createTexture2D(
                    img->m_width,
                    img->m_height,
                    1 < img->m_numMips,
                    img->m_numLayers,
                    bgfx::TextureFormat::Enum(img->m_format),
                    flags,
                    mem
                );
            }

            *width = img->m_width;
            *height = img->m_height;
            *mipCount = img->m_numMips;

            if (trace_all || trace_renderer) ffnx_trace("Renderer::%s: %u => %ux%u from filename %s\n", __func__, ret.idx, width, height, filename);
        }
    }

    return ret;
}

uint32_t Renderer::createTextureLibPng(char* filename, uint32_t* width, uint32_t* height, bool isSrgb)
{
    bgfx::TextureHandle ret = FFNX_RENDERER_INVALID_HANDLE;

    FILE* file = fopen(filename, "rb");

    if (file)
    {
        png_infop info_ptr = nullptr;
        png_structp png_ptr = nullptr;

        png_uint_32 _width = 0, _height = 0;
        png_byte color_type = 0, bit_depth = 0;

        png_bytepp rowptrs = nullptr;
        size_t rowbytes = 0;

        uint8_t* data = nullptr;
        size_t datasize = 0;

        fseek(file, 0, SEEK_END);
        datasize = ftell(file);
        fseek(file, 0, SEEK_SET);

        png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, (png_voidp)0, RendererLibPngErrorCb, RendererLibPngWarningCb);

        if (!png_ptr)
        {
            fclose(file);

            return ret.idx;
        }

        info_ptr = png_create_info_struct(png_ptr);

        if (!info_ptr)
        {
            png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);

            fclose(file);

            return ret.idx;
        }

        if (setjmp(png_jmpbuf(png_ptr)))
        {
            png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

            fclose(file);

            return ret.idx;
        }

        png_init_io(png_ptr, file);

        png_set_filter(png_ptr, 0, PNG_FILTER_NONE);

        if (!doesItFitInMemory(datasize))
        {
            png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

            fclose(file);

            return ret.idx;
        }

        png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, NULL);

        color_type = png_get_color_type(png_ptr, info_ptr);
        bit_depth = png_get_bit_depth(png_ptr, info_ptr);
        _width = png_get_image_width(png_ptr, info_ptr);
        _height = png_get_image_height(png_ptr, info_ptr);

        rowptrs = png_get_rows(png_ptr, info_ptr);
        rowbytes = png_get_rowbytes(png_ptr, info_ptr);

        datasize = rowbytes * _height;

        if (!doesItFitInMemory(datasize))
        {
            png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

            fclose(file);

            return ret.idx;
        }

        data = (uint8_t*)driver_calloc(datasize, sizeof(uint8_t));

        for (png_uint_32 y = 0; y < _height; y++) memcpy(data + (rowbytes * y), rowptrs[y], rowbytes);

        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

        fclose(file);

        // ------------------------------------------------------------

        bgfx::TextureFormat::Enum texFmt = bgfx::TextureFormat::Unknown;

        switch (bit_depth)
        {
        case 8:
        {
            switch (color_type)
            {
            case PNG_COLOR_TYPE_GRAY:
                texFmt = bgfx::TextureFormat::R8;
                break;
            case PNG_COLOR_TYPE_GRAY_ALPHA:
                texFmt = bgfx::TextureFormat::RG8;
                break;
            case PNG_COLOR_TYPE_RGB:
                texFmt = bgfx::TextureFormat::RGB8;
                break;
            case PNG_COLOR_TYPE_RGBA:
            case PNG_COLOR_TYPE_PALETTE:
                texFmt = bgfx::TextureFormat::RGBA8;
                break;
            }
            break;
        }
        case 16:
        {
            switch (color_type)
            {
            case PNG_COLOR_TYPE_GRAY:
                texFmt = bgfx::TextureFormat::R16;
                break;
            case PNG_COLOR_TYPE_GRAY_ALPHA:
                texFmt = bgfx::TextureFormat::RG16;
                break;
            case PNG_COLOR_TYPE_RGB:
            case PNG_COLOR_TYPE_RGBA:
                texFmt = bgfx::TextureFormat::RGBA16;
                break;
            case PNG_COLOR_TYPE_PALETTE:
                break;
            }
            break;
        }
        default:
            break;
        }

        if (texFmt != bgfx::TextureFormat::Unknown)
        {
            const bgfx::Memory* mem = bgfx::makeRef(data, datasize, RendererReleaseData, data);

            uint64_t flags = BGFX_SAMPLER_NONE;

            if (isSrgb) flags |= BGFX_TEXTURE_SRGB;
            else flags |= BGFX_TEXTURE_NONE;

            ret = bgfx::createTexture2D(
                _width,
                _height,
                false,
                1,
                texFmt,
                flags,
                mem
            );

            *width = _width;
            *height = _height;
        }
        else
            driver_free(data);

        if (trace_all || trace_renderer) ffnx_trace("Renderer::%s: %u => %ux%u from filename %s\n", __func__, ret.idx, width, height, filename);
    }

    return ret.idx;
}

bool Renderer::saveTexture(const char* filename, uint32_t width, uint32_t height, const void* data)
{
    if (trace_all || trace_renderer) ffnx_trace("Renderer::%s: %ux%u with filename %s\n", __func__, width, height, filename);

    if (bx::open(&defaultWriter, filename, false))
    {
        bimg::imageWritePng(
            &defaultWriter,
            width,
            height,
            width * 4,
            data,
            bimg::TextureFormat::BGRA8,
            false
        );

        bx::close(&defaultWriter);

        return true;
    }

    return false;
}

void Renderer::deleteTexture(uint16_t rt)
{
    if (rt > 0)
    {
        bgfx::TextureHandle handle = { rt };

        if (bgfx::isValid(handle)) {
            bgfx::destroy(handle);

            if (trace_all || trace_renderer) ffnx_trace("Renderer::%s: %u Texture was valid and is now destroyed!\n", __func__, rt);
        }
    }
};

void Renderer::useTexture(uint16_t rt, uint32_t slot)
{
    if (trace_all || trace_renderer) ffnx_trace("Renderer::%s: [%u] => %u\n", __func__, slot, rt);

    if (rt > 0)
    {
        internalState.texHandlers[slot] = { rt };
        if (slot == RendererTextureSlot::TEX_Y) isTexture(true);
    }
    else
    {
        internalState.texHandlers[slot] = BGFX_INVALID_HANDLE;
        if (slot == RendererTextureSlot::TEX_Y) isTexture(false);
    }
};

uint32_t Renderer::blitTexture(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    uint16_t newX = getInternalCoordX(x);
    uint16_t newY = getInternalCoordY(y);
    uint16_t newWidth = getInternalCoordX(width);
    uint16_t newHeight = getInternalCoordY(height);

    uint16_t dstY = 0;

    bgfx::TextureHandle ret = bgfx::createTexture2D(newWidth, newHeight, false, 1, internalState.bIsHDR ? bgfx::TextureFormat::RGB10A2 : bgfx::TextureFormat::RGBA16, BGFX_TEXTURE_BLIT_DST);

    if (trace_all || trace_renderer) ffnx_trace("Renderer::%s: %u => XY(%u,%u) WH(%u,%u)\n", __func__, ret.idx, newX, newY, newWidth, newHeight);

    if (getCaps()->originBottomLeft)
    {
        int _newY = framebufferHeight - (newY + newHeight);

        // If the new Y is a positive value, we can use it as it is
        if (_newY > 0)
        {
            newY = _newY;
            dstY = 0;
        }
        // Otherwise, it means we have to copy the whole source texture
        // but shift the result on the Y axis of the dest texture of the absolute negative difference
        else
        {
            newY = 0;
            dstY = ::abs(_newY);
        }
    }

    backendViewId++;

    bgfx::blit(backendViewId, ret, 0, dstY, bgfx::getTexture(backendFrameBuffer), newX, newY, newWidth, newHeight);
    bgfx::touch(backendViewId);
    setClearFlags(false, false);

    backendViewId++;
    setClearFlags(false, false);

    return ret.idx;
};

void Renderer::isMovie(bool flag)
{
    internalState.bIsMovie = flag;
};

void Renderer::isTLVertex(bool flag)
{
    internalState.bIsTLVertex = flag;
};

void Renderer::setBlendMode(RendererBlendMode mode)
{
    internalState.blendMode = mode;
};

void Renderer::isTexture(bool flag)
{
    internalState.bIsTexture = flag;
};

void Renderer::isFBTexture(bool flag)
{
    internalState.bIsFBTexture = flag;
};

void Renderer::isFullRange(bool flag)
{
    internalState.bIsMovieFullRange = flag;
};

void Renderer::isYUV(bool flag)
{
    internalState.bIsMovieYUV = flag;
};

void Renderer::doModulateAlpha(bool flag)
{
    internalState.bModulateAlpha = flag;
};

void Renderer::doTextureFiltering(bool flag)
{
    internalState.bDoTextureFiltering = flag;
};

void Renderer::isExternalTexture(bool flag)
{
    internalState.bIsExternalTexture = flag;
}

bool Renderer::isHDR()
{
    return internalState.bIsHDR;
}

void Renderer::setAlphaRef(RendererAlphaFunc func, float ref)
{
    internalState.alphaFunc = func;
    internalState.alphaRef = ref;
};

void Renderer::doAlphaTest(bool flag)
{
    internalState.bDoAlphaTest = flag;
};

void Renderer::setInterpolationQualifier(RendererInterpolationQualifier qualifier)
{
    switch (qualifier)
    {
    case RendererInterpolationQualifier::FLAT:
        backendProgram = RendererProgram::FLAT;
        if (trace_all || trace_renderer) ffnx_trace("Renderer::%s: FLAT\n", __func__);
        break;
    case RendererInterpolationQualifier::SMOOTH:
        backendProgram = RendererProgram::SMOOTH;
        if (trace_all || trace_renderer) ffnx_trace("Renderer::%s: SMOOTH\n", __func__);
        break;
    }
}

void Renderer::setPrimitiveType(RendererPrimitiveType type)
{
    if (trace_all || trace_renderer) ffnx_trace("Renderer::%s: %u\n", __func__, type);

    internalState.primitiveType = type;
};

void Renderer::setCullMode(RendererCullMode mode)
{
    internalState.cullMode = mode;
}

void Renderer::doDepthTest(bool flag)
{
    internalState.bDoDepthTest = flag;
}

void Renderer::doDepthWrite(bool flag)
{
    internalState.bDoDepthWrite = flag;
}

void Renderer::doScissorTest(bool flag)
{
    internalState.bDoScissorTest = flag;
}

void Renderer::setWireframeMode(bool flag)
{
    if (flag) bgfx::setDebug(BGFX_DEBUG_WIREFRAME);
};

void Renderer::setViewMatrix(struct matrix* matrix)
{
    ::memcpy(internalState.viewMatrix, &matrix->m[0][0], sizeof(matrix->m));

    bx::mtxInverse(internalState.invViewMatrix, internalState.viewMatrix);

    if (uniform_log) printMatrix(__func__, internalState.viewMatrix);
};

float* Renderer::getViewMatrix()
{
    return internalState.viewMatrix;
}

void Renderer::setWorldViewMatrix(struct matrix *matrix)
{
    ::memcpy(internalState.worldViewMatrix, &matrix->m[0][0], sizeof(matrix->m));

    if (uniform_log) printMatrix(__func__, internalState.worldViewMatrix);

    struct matrix transpose;
    transpose_matrix(matrix, &transpose);
    struct matrix invTranspose;
    inverse_matrix(&transpose, &invTranspose);
    invTranspose._41 = 0.0;
    invTranspose._42 = 0.0;
    invTranspose._43 = 0.0;
    invTranspose._44 = 1.0;

    ::memcpy(internalState.normalMatrix, &invTranspose.m[0][0], sizeof(invTranspose.m));

    if (uniform_log) printMatrix(__func__, internalState.normalMatrix);
};

void Renderer::setD3DViweport(struct matrix* matrix)
{
    ::memcpy(internalState.d3dViewMatrix, &matrix->m[0][0], sizeof(matrix->m));

    if (uniform_log) printMatrix(__func__, internalState.d3dViewMatrix);
};

void Renderer::setD3DProjection(struct matrix* matrix)
{
    ::memcpy(internalState.d3dProjectionMatrix, &matrix->m[0][0], sizeof(matrix->m));

    if (uniform_log) printMatrix(__func__, internalState.d3dProjectionMatrix);
};

uint16_t Renderer::getInternalCoordX(uint16_t inX)
{
    return (inX * framebufferWidth) / game_width;
}

uint16_t Renderer::getInternalCoordY(uint16_t inY)
{
    return (inY * framebufferHeight) / game_height;
}
