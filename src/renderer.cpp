#include "renderer.h"

Renderer newRenderer;

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
    if (uniform_log) trace("%s: VSFlags XYZW(isTLVertex %f, blendMode %f, isFBTexture %f, isTexture %f)\n", __func__, internalState.VSFlags[0], internalState.VSFlags[1], internalState.VSFlags[2], internalState.VSFlags[3]);

    internalState.FSAlphaFlags = {
        (float)internalState.alphaRef,
        (float)internalState.alphaFunc,
        (float)internalState.bDoAlphaTest,
        NULL
    };
    if (uniform_log) trace("%s: FSAlphaFlags XYZW(inAlphaRef %f, inAlphaFunc %f, bDoAlphaTest %f, NULL)\n", __func__, internalState.FSAlphaFlags[0], internalState.FSAlphaFlags[1], internalState.FSAlphaFlags[2]);

    internalState.FSMiscFlags = {
        (float)internalState.bIsMovieFullRange,
        (float)internalState.bIsMovieYUV,
        (float)internalState.bModulateAlpha,
        (float)internalState.bIsMovie
    };
    if (uniform_log) trace("%s: FSMiscFlags XYZW(isMovieFullRange %f, isMovieYUV %f, modulateAlpha %f, isMovie %f)\n", __func__, internalState.FSMiscFlags[0], internalState.FSMiscFlags[1], internalState.FSMiscFlags[2], internalState.FSMiscFlags[3]);

    setUniform("VSFlags", bgfx::UniformType::Vec4, internalState.VSFlags.data());
    setUniform("FSAlphaFlags", bgfx::UniformType::Vec4, internalState.FSAlphaFlags.data());
    setUniform("FSMiscFlags", bgfx::UniformType::Vec4, internalState.FSMiscFlags.data());

    setUniform("d3dViewport", bgfx::UniformType::Mat4, internalState.d3dViewMatrix);
    setUniform("d3dProjection", bgfx::UniformType::Mat4, internalState.d3dProjectionMatrix);
    setUniform("worldView", bgfx::UniformType::Mat4, internalState.worldViewMatrix);
}

bgfx::RendererType::Enum Renderer::getRendererType() {
    std::string backend(renderer_backend);
    bgfx::RendererType::Enum ret;

    std::string shaderSuffix;

    if (backend == "Vulkan") {
        ret = bgfx::RendererType::Vulkan;
        shaderSuffix = ".vk";
    }
    else if (backend == "Direct3D12") {
        ret = bgfx::RendererType::Direct3D12;
        shaderSuffix = ".d3d12";
    }
    else if (backend == "Direct3D11") {
        ret = bgfx::RendererType::Direct3D11;
        shaderSuffix = ".d3d11";
    }
    else if (backend == "OpenGL") {
        ret = bgfx::RendererType::OpenGL;
        shaderSuffix = ".gl";
    }
    else {
        ret = bgfx::RendererType::Noop;
    }

    vertexPath += shaderSuffix + ".vert";
    fragmentPath += shaderSuffix + ".frag";
    vertexPostPath += shaderSuffix + ".vert";
    fragmentPostPath += shaderSuffix + ".frag";

    return ret;
}

// Via https://dev.to/pperon/hello-bgfx-4dka
bgfx::ShaderHandle Renderer::getShader(const char* filePath)
{
    bgfx::ShaderHandle handle = BGFX_INVALID_HANDLE;

    FILE* file = fopen(filePath, "rb");

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

    bgfx::destroy(emptyTexture);

    bgfx::destroy(vertexBufferHandle);

    bgfx::destroy(indexBufferHandle);

    bgfx::destroy(backendFrameBuffer);

    for (auto handle : backendProgramHandles)
    {
        if (bgfx::isValid(handle))
            bgfx::destroy(handle);
    }
};

void Renderer::reset()
{
    setBackgroundColor();

    doAlphaTest();
    doDepthTest();
    doDepthWrite();
    doScissorTest();
    setCullMode();
    setBlendMode();
    setAlphaRef();
    isTLVertex();
    isYUV();
    isFullRange();
    isFBTexture();
    isTexture();
    doModulateAlpha();
    doTextureFiltering();
    isExternalTexture();
    useFancyTransparency();
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
    word indices[] = {
        0, 1, 2,
        1, 3, 2
    };

    backendProgram = RendererProgram::POSTPROCESSING;
    backendViewId++;
    {
        if (internalState.bHasDrawBeenDone)
            useTexture(
                bgfx::getTexture(backendFrameBuffer).idx
            );
        else
            useTexture(emptyTexture.idx);

        setClearFlags(true, true);

        bindVertexBuffer(vertices, 4);
        bindIndexBuffer(indices, 6);

        setBlendMode(RendererBlendMode::BLEND_DISABLED);
        setPrimitiveType();

        draw();

        setBlendMode();
    }
    backendProgram = RendererProgram::FRAMEBUFFER;
};

void Renderer::printMatrix(char* name, float* mat)
{
    trace("%s: 0 [%f, %f, %f, %f]\n", name, mat[0], mat[1], mat[2], mat[3]);
    trace("%s: 1 [%f, %f, %f, %f]\n", name, mat[4], mat[5], mat[6], mat[7]);
    trace("%s: 2 [%f, %f, %f, %f]\n", name, mat[8], mat[9], mat[10], mat[11]);
    trace("%s: 3 [%f, %f, %f, %f]\n", name, mat[12], mat[13], mat[14], mat[15]);
};

bool Renderer::doesItFitInMemory(size_t size)
{
    return size < last_ram_state.ullAvailVirtual;
}

// PUBLIC

void Renderer::init()
{
    viewWidth = window_size_x;
    viewHeight = window_size_y;

    // aspect correction
    if (preserve_aspect && viewWidth * 3 != viewHeight * 4)
    {
        if (viewHeight * 4 > viewWidth * 3)
        {
            viewOffsetY = viewHeight - (viewWidth * 3) / 4;
            viewHeight = (viewWidth * 3) / 4;
        }
        else if (viewWidth * 3 > viewHeight * 4)
        {
            viewOffsetX = (viewWidth - (viewHeight * 4) / 3) / 2;
            viewWidth = (viewHeight * 4) / 3;
        }
    }

    // In order to prevent weird glitches while rendering we need to use the closest resolution to native's game one
    framebufferWidth = (viewWidth % game_width) ? (viewWidth / game_width + 1) * game_width * (internal_resolution_scale - 1) : viewWidth;
    framebufferHeight = (viewHeight % game_height) ? (viewHeight / game_height + 1) * game_height * (internal_resolution_scale - 1) : viewHeight;

    framebufferVertexWidth = (viewWidth * game_width) / window_size_x;
    framebufferVertexOffsetX = (game_width - framebufferVertexWidth) / 2;

    // Init renderer
    bgfx::Init bgfxInit;
    bgfxInit.platformData.nwh = hwnd;
    bgfxInit.type = getRendererType();
    bgfxInit.resolution.width = window_size_x;
    bgfxInit.resolution.height = window_size_y;
    bgfxInit.resolution.reset = BGFX_RESET_NONE;

    if (enable_anisotropic)
        bgfxInit.resolution.reset |= BGFX_RESET_MAXANISOTROPY;

    if (enable_vsync)
        bgfxInit.resolution.reset |= BGFX_RESET_VSYNC;

    bgfxInit.debug = renderer_debug;
    bgfxInit.callback = &bgfxCallbacks;

    if (!bgfx::init(bgfxInit)) exit(1);

    bx::mtxOrtho(internalState.backendProjMatrix, 0.0f, game_width, game_height, 0.0f, bgfxInit.type == bgfx::RendererType::OpenGL ? -1.0f : 0.0f, 1.0f, 0.0, bgfx::getCaps()->homogeneousDepth);

    // Create an empty texture
    emptyTexture = bgfx::createTexture2D(1, 1, false, 1, bgfx::TextureFormat::BGRA8);

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

    backendFrameBufferRT = {
        bgfx::createTexture2D(
            framebufferWidth,
            framebufferHeight,
            false,
            1,
            bgfx::TextureFormat::RGBA8,
            fbFlags
        ),
        bgfx::createTexture2D(
            framebufferWidth,
            framebufferHeight,
            false,
            1,
            bgfx::TextureFormat::D24S8,
            fbFlags
        )
    };

    backendFrameBuffer = bgfx::createFrameBuffer(
        backendFrameBufferRT.size(),
        backendFrameBufferRT.data(),
        true
    );

    // Create Program
    backendProgramHandles[RendererProgram::POSTPROCESSING] = bgfx::createProgram(
        getShader(vertexPostPath.c_str()),
        getShader(fragmentPostPath.c_str()),
        true
    );

    backendProgramHandles[RendererProgram::FRAMEBUFFER] = bgfx::createProgram(
        getShader(vertexPath.c_str()),
        getShader(fragmentPath.c_str()),
        true
    );

    vertexLayout
        .begin()
        .add(bgfx::Attrib::Position, 4, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .end();

    if (fullscreen) bgfx::setDebug(BGFX_DEBUG_TEXT);

    bgfx::frame();

    // Set defaults
    show();
};

void Renderer::shutdown()
{
    destroyAll();

    bgfx::shutdown();
}

void Renderer::draw()
{
    if (trace_all) trace("Renderer::%s\n", __func__);

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

    setCommonUniforms();

    // Bind texture
    {
        uint idxMax = 3;

        for (uint idx = 0; idx < idxMax; idx++)
        {
            uint16_t rt = internalState.texHandlers[idx];

            bgfx::TextureHandle handle = { rt };

            if (!internalState.bIsMovie && idx > 0) handle = emptyTexture;

            if (bgfx::isValid(handle))
            {
                uint32_t flags = 0;

                if (backendProgram == RendererProgram::POSTPROCESSING)
                {
                    flags = BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_W_CLAMP | BGFX_SAMPLER_MIN_ANISOTROPIC | BGFX_SAMPLER_MAG_ANISOTROPIC;
                }
                else
                {
                    if (internalState.bIsMovie) flags = BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_W_CLAMP;

                    if (!internalState.bDoTextureFiltering) flags |= BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIP_POINT;
                }

                bgfx::setTexture(idx, getUniform(shaderTextureBindings[idx], bgfx::UniformType::Sampler), handle, flags);
            }
        }
    }

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
            if (internalState.bUseFancyTransparency) internalState.state |= BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA);
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
};

void Renderer::show()
{
    // Reset internal state
    reset();

    renderFrame();

    bgfx::frame();

    bgfx::dbgTextClear();

    backendViewId = 1;

    bgfx::setViewMode(backendViewId, bgfx::ViewMode::Sequential);
}

void Renderer::printText(uint16_t x, uint16_t y, uint color, const char* text)
{
    bgfx::dbgTextPrintf(
        x,
        y,
        color,
        text
    );
}

const bgfx::Caps* Renderer::getCaps()
{
    return bgfx::getCaps();
};

void Renderer::bindVertexBuffer(struct nvertex* inVertex, uint inCount)
{
    if (bgfx::isValid(vertexBufferHandle)) bgfx::destroy(vertexBufferHandle);

    Vertex* vertices = new Vertex[inCount];

    for (uint idx = 0; idx < inCount; idx++)
    {
        vertices[idx].x = inVertex[idx]._.x;
        vertices[idx].y = inVertex[idx]._.y;
        vertices[idx].z = inVertex[idx]._.z;
        vertices[idx].w = ( std::isinf(inVertex[idx].color.w) ? 1.0f : inVertex[idx].color.w );
        vertices[idx].bgra = inVertex[idx].color.color;
        vertices[idx].u = inVertex[idx].u;
        vertices[idx].v = inVertex[idx].v;

        if (vertex_log && idx == 0) trace("%s: %u [XYZW(%f, %f, %f, %f), BGRA(%08x), UV(%f, %f)]\n", __func__, idx, vertices[idx].x, vertices[idx].y, vertices[idx].z, vertices[idx].w, vertices[idx].bgra, vertices[idx].u, vertices[idx].v);
        if (vertex_log && idx == 1) trace("%s: See the rest on RenderDoc.\n", __func__);
    }

    vertexBufferHandle = bgfx::createVertexBuffer(
        bgfx::copy(
            vertices,
            sizeof(Vertex) * inCount
        ),
        vertexLayout
    );

    bgfx::setVertexBuffer(0, vertexBufferHandle);

    delete[] vertices;
};

void Renderer::bindIndexBuffer(word* inIndex, uint inCount)
{
    if (bgfx::isValid(indexBufferHandle)) bgfx::destroy(indexBufferHandle);

    indexBufferHandle = bgfx::createIndexBuffer(
        bgfx::copy(
            inIndex,
            sizeof(word) * inCount
        )
    );

    bgfx::setIndexBuffer(indexBufferHandle);
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

uint Renderer::createTexture(uint8_t* data, size_t width, size_t height, int stride, RendererTextureType type, bool generateMips)
{
    bgfx::TextureHandle ret;

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
    if (!doesItFitInMemory(texInfo.storageSize))
    {
        ret = bgfx::createTexture2D(1, 1, false, 1, bgfx::TextureFormat::BGRA8);
    }
    else
    {
        const bgfx::Memory* mem = bgfx::copy(data, texInfo.storageSize);

        ret = bgfx::createTexture2D(
            width,
            height,
            false,
            1,
            texFormat,
            BGFX_TEXTURE_NONE,
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
    }

    return ret.idx;
};

uint Renderer::createTexture(char* filename, uint* width, uint* height)
{
    bgfx::TextureHandle ret = { 0 };

    FILE* file = fopen(filename, "rb");
        
    if (file)
    {
        size_t filesize = 0;
        bimg::ImageContainer imgSize;
        bimg::ImageContainer* img = nullptr;
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

        // ==================================

        if (buffer != nullptr)
        {
            bimg::imageParse(imgSize, buffer, filesize + 1);

            if (doesItFitInMemory(imgSize.m_size))
            {
                img = bimg::imageParse(&defaultAllocator, buffer, filesize + 1);
            }

            driver_free(buffer);
        }

        if (img != nullptr)
        {
            if (gl_check_texture_dimensions(img->m_width, img->m_height, filename))
            {
                const bgfx::Memory* mem = bgfx::copy(img->m_data, img->m_size);

                ret = bgfx::createTexture2D(
                    img->m_width,
                    img->m_height,
                    1 < img->m_numMips,
                    img->m_numLayers,
                    bgfx::TextureFormat::Enum(img->m_format),
                    BGFX_TEXTURE_NONE,
                    mem
                );

                *width = img->m_width;
                *height = img->m_height;
            }
            else
            {
                ret = bgfx::createTexture2D(1, 1, false, 1, bgfx::TextureFormat::BGRA8);
            }

            bimg::imageFree(img);
        }
        else
        {
            ret = bgfx::createTexture2D(1, 1, false, 1, bgfx::TextureFormat::BGRA8);
        }
    }

    return ret.idx;
}

bool Renderer::saveTexture(char* filename, uint width, uint height, void* data)
{
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
        }
    }
};

void Renderer::useTexture(uint rt, uint slot)
{
    if (rt > 0)
    {
        internalState.texHandlers[slot] = rt;
        isTexture(true);
    }
    else
    {
        internalState.texHandlers[slot] = 0;
        isTexture(false);
    }
};

uint Renderer::blitTexture(uint x, uint y, uint width, uint height)
{
    uint16_t newX = getInternalCoordX(x);
    uint16_t newY = getInternalCoordY(y);
    uint16_t newWidth = getInternalCoordX(width);
    uint16_t newHeight = getInternalCoordY(height);

    uint16_t texWidth = framebufferWidth > newWidth ? newWidth : framebufferWidth;
    uint16_t texHeight = framebufferHeight > newHeight ? newHeight : framebufferHeight;
    
    bgfx::TextureHandle ret = bgfx::createTexture2D(texWidth, texHeight, false, 1, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_BLIT_DST);

    if (getCaps()->originBottomLeft) newY = framebufferHeight - (newY + texHeight);
    
    backendViewId++;

    bgfx::blit(backendViewId, ret, 0, 0, bgfx::getTexture(backendFrameBuffer), newX, newY, newWidth, newHeight);
    bgfx::touch(backendViewId);
    
    backendViewId++;

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

void Renderer::useFancyTransparency(bool flag)
{
    internalState.bUseFancyTransparency = flag;
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

void Renderer::setPrimitiveType(RendererPrimitiveType type)
{
    if (trace_all) trace("%s: %u\n", __func__, type);

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

void Renderer::setWorldView(struct matrix *matrix)
{
    ::memcpy(internalState.worldViewMatrix, &matrix->m[0][0], sizeof(matrix->m));

    if (uniform_log) printMatrix(__func__, internalState.worldViewMatrix);
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
