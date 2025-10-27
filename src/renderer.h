/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2025 Julian Xhokaxhiu                                   //
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

#include "common.h"
#include "overlay.h"

#include <cmrc/cmrc.hpp>
#include <vector>
#include <array>
#include <string>
#include <math.h>
#include <bx/math.h>
#include <bx/bx.h>
#include <bx/allocator.h>
#include <bx/file.h>
#include <bimg/bimg.h>
#include <bimg/decode.h>
#include <bimg/encode.h>
#include <bgfx/platform.h>
#include <bgfx/bgfx.h>

#define FFNX_RENDERER_INVALID_HANDLE { 0 }

enum RendererInterpolationQualifier {
    FLAT = 0,
    SMOOTH
};

enum RendererBlendMode {
    BLEND_AVG = 0,
    BLEND_ADD,
    BLEND_SUB,
    BLEND_25P,
    BLEND_NONE,
    BLEND_DISABLED = 999
};

enum RendererCullMode {
    DISABLED = 0,
    FRONT,
    BACK
};

enum RendererAlphaFunc {
    NEVER = 0,
    LESS,
    EQUAL,
    LEQUAL,
    GREATER,
    NOTEQUAL,
    GEQUAL,
    ALWAYS
};

enum RendererPrimitiveType
{
    PT_POINTS = 0,
    PT_LINES,
    PT_LINE_LOOP,
    PT_LINE_STRIP,
    PT_TRIANGLES,
    PT_TRIANGLE_STRIP,
    PT_TRIANGLE_FAN,
    PT_QUADS,
    PT_QUAD_STRIP
};

enum RendererTextureType
{
    BGRA = 0,
    YUV
};

enum RendererUniform
{
    VS_FLAGS = 0,
    FS_ALPHA_FLAGS,
    FS_MISC_FLAGS,
    FS_HDR_FLAGS,
    FS_TEX_FLAGS,
    FS_MOVIE_FLAGS,
    WM_FLAGS,
    TIME_COLOR,
    TIME_DATA,
    D3D_VIEWPORT,
    D3D_PROJECTION,
    WORLD_VIEW,
    NORMAL_MATRIX,
    VIEW_MATRIX,
    INV_VIEW_MATRIX,
    GAME_LIGHTING_FLAGS,
    GAME_GLOBAL_LIGHT_COLOR,
    GAME_LIGHT_COLOR1,
    GAME_LIGHT_COLOR2,
    GAME_LIGHT_COLOR3,
    GAME_LIGHT_DIR1,
    GAME_LIGHT_DIR2,
    GAME_LIGHT_DIR3,
    GAME_SCRIPTED_LIGHT_COLOR,

    LIGHTING_SETTINGS,
    LIGHT_DIR_DATA,
    LIGHT_DATA,
    AMBIENT_LIGHT_DATA,
    SHADOW_DATA,
    FIELD_SHADOW_DATA,
    MATERIAL_DATA,
    MATERIAL_SCALE_DATA,
    LIGHTING_DEBUG_DATA,
    IBL_DATA,
    LIGHT_VIEW_PROJ_MATRIX,
    LIGHT_VIEW_PROJ_TEX_MATRIX,
    LIGHT_INV_VIEW_PROJ_TEX_MATRIX,
    VIEW_OFFSET_MATRIX,
    INV_VIEW_OFFSET_MATRIX,

    COUNT,
};

enum ColorMatrixType{
    COLORMATRIX_BT601 = 0,
    COLORMATRIX_BT709 = 1,
    COLORMATRIX_BGR24 = 2
};

enum ColorGamutType{
    COLORGAMUT_SRGB = 0,
    COLORGAMUT_NTSCJ = 1,
    COLORGAMUT_SMPTEC = 2,
    COLORGAMUT_EBU = 3
};

enum InverseGammaFunctionType{
    GAMMAFUNCTION_SRGB = 0,
    GAMMAFUNCTION_TWO_PT_TWO = 1,
    GAMMAFUNCTION_SMPTE170M = 2,
    GAMMAFUNCTION_BT1886_APPX1 = 3,
    GAMMAFUNCTION_TWO_PT_EIGHT = 4
};

namespace RendererTextureSlot {
    enum RendererTextureSlot
    {
        TEX_Y = 0,
        TEX_U,
        TEX_V,
        TEX_S,
        TEX_D,
        TEX_NML,
        TEX_PBR,
        TEX_IBL_SPEC,
        TEX_IBL_DIFF,
        TEX_BRDF,
        TEX_G_LUT,
        COUNT
    };
};

enum GamutLUTIndexType{
    INDEX_LUT_NTSCJ_TO_SRGB,
    INDEX_LUT_SMPTEC_TO_SRGB,
    INDEX_LUT_EBU_TO_SRGB,
    INDEX_LUT_INVERSE_NTSCJ_TO_SRGB,
    INDEX_LUT_INVERSE_NTSCJ_TO_SMPTEC,
    INDEX_LUT_INVERSE_NTSCJ_TO_EBU
};

static void RendererReleaseImageContainer(void* _ptr, void* _userData)
{
    BX_UNUSED(_ptr);
    bimg::ImageContainer* imageContainer = (bimg::ImageContainer*)_userData;
    bimg::imageFree(imageContainer);
}

static void RendererReleaseData(void* _ptr, void* _userData)
{
    BX_UNUSED(_ptr);
    driver_free(_userData);
}

struct RendererCallbacks : public bgfx::CallbackI {
    std::string cachePath = R"(shaders\cache)";

    virtual ~RendererCallbacks() {};
    virtual void fatal(const char* _filePath, uint16_t _line, bgfx::Fatal::Enum _code, const char* _str) override;
    virtual void traceVargs(const char* _filePath, uint16_t _line, const char* _format, va_list _argList) override;
    virtual void profilerBegin(const char* _name, uint32_t _abgr, const char* _filePath, uint16_t _line) override {};
    virtual void profilerBeginLiteral(const char* _name, uint32_t _abgr, const char* _filePath, uint16_t _line) override {};
    virtual void profilerEnd() override {};
    virtual uint32_t cacheReadSize(uint64_t _id) override;
    virtual bool cacheRead(uint64_t _id, void* _data, uint32_t _size) override;
    virtual void cacheWrite(uint64_t _id, const void* _data, uint32_t _size) override;
    virtual void screenShot(const char* _filePath, uint32_t _width, uint32_t _height, uint32_t _pitch, const void* _data, uint32_t _size, bool _yflip) override {};
    virtual void captureBegin(uint32_t _width, uint32_t _height, uint32_t _pitch, bgfx::TextureFormat::Enum _format, bool _yflip) override {};
    virtual void captureEnd() override {};
    virtual void captureFrame(const void* _data, uint32_t _size) override {};
};

// Vertex data structure
struct Vertex
{
    float x;
    float y;
    float z;
    float w;
    uint32_t bgra;
    float u;
    float v;
    float nx;
    float ny;
    float nz;
};

class Renderer {
private:
    friend class Lighting;

    // Current renderer view
    enum RendererProgram {
        FLAT = 0,
        SMOOTH,
        SHADOW_MAP,
        LIGHTING_FLAT,
        LIGHTING_SMOOTH,
        FIELD_SHADOW,
        POSTPROCESSING,
        OVERLAY,
        BLIT,
        COUNT
    };

    struct RendererState
    {
        std::vector<bgfx::TextureHandle> texHandlers;
        bool bTexturesBound = false;

        bool bHasDrawBeenDone = false;

        bool bDoAlphaTest = false;
        float alphaRef = 0.0f;
        RendererAlphaFunc alphaFunc;

        bool bDoDepthTest = false;
        bool bDoDepthWrite = false;
        bool bDoScissorTest = false;

        bool bIsTLVertex = false;
        bool bIsFBTexture = false;
        bool bIsTexture = false;
        bool bDoTextureFiltering = false;
        bool bDoMirrorTextureWrap = false;
        bool bModulateAlpha = false;
        bool bIsMovie = false;
        bool bIsMovieFullRange = false;
        bool bIsMovieYUV = false;
        bool bIsExternalTexture = false;
        bool bIsHDR = false;
        bool bIsFogEnabled = false;
        ColorMatrixType bIsMovieColorMatrix = COLORMATRIX_BT601;
        ColorGamutType bIsMovieColorGamut = COLORGAMUT_SRGB;
        ColorGamutType bIsOverallColorGamut = COLORGAMUT_SRGB;
        InverseGammaFunctionType bIsMovieGammaType = GAMMAFUNCTION_SRGB;

        float backendProjMatrix[16];
        float postprocessingProjMatrix[16];

        std::vector<float> VSFlags;
        std::vector<float> FSAlphaFlags;
        std::vector<float> FSMiscFlags;
        std::vector<float> FSHDRFlags;
        std::vector<float> FSTexFlags;
        std::vector<float> WMFlags;
        std::vector<float> FSMovieFlags;

        std::array<float, 4> TimeColor;
        std::array<float, 4> TimeData;

        std::array<float, 4> gameLightingFlags;
        float gameGlobalLightColor[4] = { 0.0, 0.0, 0.0, 0.0 };
        float gameLightDir1[4] = { 0.0, 0.0, 0.0, 0.0 };
        float gameLightColor1[4] = { 0.0, 0.0, 0.0, 0.0 };
        float gameLightDir2[4] = { 0.0, 0.0, 0.0, 0.0 };
        float gameLightColor2[4] = { 0.0, 0.0, 0.0, 0.0 };
        float gameLightDir3[4] = { 0.0, 0.0, 0.0, 0.0 };
        float gameLightColor3[4] = { 0.0, 0.0, 0.0, 0.0 };
        float gameScriptedLightColor[4] = { 0.0, 0.0, 0.0, 0.0 };

        float d3dViewMatrix[16];
        float d3dProjectionMatrix[16];
        float viewMatrix[16];
        float invViewMatrix[16];
        float worldViewMatrix[16];
        float normalMatrix[16];

        float sphericalWorldRate = 0.0f;

        uint32_t clearColorValue;

        RendererCullMode cullMode = RendererCullMode::DISABLED;
        RendererBlendMode blendMode = RendererBlendMode::BLEND_NONE;
        RendererPrimitiveType primitiveType = RendererPrimitiveType::PT_TRIANGLES;

        uint64_t state = BGFX_STATE_MSAA;

        bool isViewMatrixSet = false;
    };

    std::string vertexPathFlat = "shaders/FFNx";
    std::string fragmentPathFlat = "shaders/FFNx";
    std::string vertexPathSmooth = "shaders/FFNx";
    std::string fragmentPathSmooth = "shaders/FFNx";
    std::string vertexPostPath = "shaders/FFNx.post";
    std::string fragmentPostPath = "shaders/FFNx.post";
    std::string vertexOverlayPath = "shaders/FFNx.overlay";
    std::string fragmentOverlayPath = "shaders/FFNx.overlay";
    std::string vertexLightingPathFlat = "shaders/FFNx.lighting";
    std::string fragmentLightingPathFlat = "shaders/FFNx.lighting";
    std::string vertexLightingPathSmooth = "shaders/FFNx.lighting";
    std::string fragmentLightingPathSmooth = "shaders/FFNx.lighting";
    std::string vertexShadowMapPath = "shaders/FFNx.shadowmap";
    std::string fragmentShadowMapPath = "shaders/FFNx.shadowmap";
    std::string vertexFieldShadowPath = "shaders/FFNx.field.shadow";
    std::string fragmentFieldShadowPath = "shaders/FFNx.field.shadow";
    std::string vertexBlitPath = "shaders/FFNx.blit";
    std::string fragmentBlitPath = "shaders/FFNx.blit";

    bgfx::ViewId backendViewId = 1;
    RendererProgram backendProgram = RendererProgram::SMOOTH;

    std::vector<bgfx::ProgramHandle> backendProgramHandles = std::vector<bgfx::ProgramHandle>(RendererProgram::COUNT, BGFX_INVALID_HANDLE);

    std::vector<bgfx::TextureHandle> backendFrameBufferRT = { BGFX_INVALID_HANDLE, BGFX_INVALID_HANDLE };
    bgfx::FrameBufferHandle backendFrameBuffer = BGFX_INVALID_HANDLE;

    bgfx::TextureHandle shadowMapTexture = BGFX_INVALID_HANDLE;
    bgfx::FrameBufferHandle shadowMapFrameBuffer = BGFX_INVALID_HANDLE;

    bgfx::TextureHandle specularIblTexture = BGFX_INVALID_HANDLE;
    bgfx::TextureHandle diffuseIblTexture = BGFX_INVALID_HANDLE;
    bgfx::TextureHandle envBrdfTexture = BGFX_INVALID_HANDLE;

    std::vector<Vertex> vertexBufferData;
    bgfx::DynamicVertexBufferHandle vertexBufferHandle = BGFX_INVALID_HANDLE;

    std::vector<WORD> indexBufferData;
    bgfx::DynamicIndexBufferHandle indexBufferHandle = BGFX_INVALID_HANDLE;

    bgfx::TextureHandle FFNxLogoHandle = BGFX_INVALID_HANDLE;

    bgfx::TextureHandle GLUTHandleNTSCJtoSRGB = BGFX_INVALID_HANDLE;
    bgfx::TextureHandle GLUTHandleSMPTECtoSRGB = BGFX_INVALID_HANDLE;
    bgfx::TextureHandle GLUTHandleEBUtoSRGB = BGFX_INVALID_HANDLE;
    bgfx::TextureHandle GLUTHandleInverseNTSCJtoSRGB = BGFX_INVALID_HANDLE;
    bgfx::TextureHandle GLUTHandleInverseNTSCJtoSMPTEC = BGFX_INVALID_HANDLE;
    bgfx::TextureHandle GLUTHandleInverseNTSCJtoEBU = BGFX_INVALID_HANDLE;

    bgfx::VertexLayout vertexLayout;

    std::array<bgfx::UniformHandle, RendererUniform::COUNT> bgfxUniformHandles;
    std::array<bgfx::UniformHandle, RendererTextureSlot::COUNT> bgfxTexUniformHandles;

    RendererState internalState;

    uint16_t viewOffsetX = 0;
    uint16_t viewOffsetY = 0;
    uint16_t viewWidth = 0;
    uint16_t viewHeight = 0;

    uint16_t framebufferWidth = 0;
    uint16_t framebufferHeight = 0;

    uint16_t scissorOffsetX = 0;
    uint16_t scissorOffsetY = 0;
    uint16_t scissorWidth = 0;
    uint16_t scissorHeight = 0;

    uint16_t framebufferVertexOffsetX = 0;
    uint16_t framebufferVertexWidth = 0;

    uint16_t scalingFactor = 0;

    uint32_t createBGRA(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    bgfx::RendererType::Enum getUserChosenRenderer();
    void updateRendererShaderPaths();
    bgfx::ShaderHandle getShader(const char* filePath);

    bgfx::UniformHandle createUniform(std::string uniformName, bgfx::UniformType::Enum uniformType);

    void destroyUniforms();
    void destroyAll();

    void resetState();

    void renderFrame();

    void printMatrix(char* name, float* mat);

    void recalcInternals();
    void calcBackendProjMatrix();
    void prepareFramebuffer();

    void AssignGamutLUT();

    bx::DefaultAllocator defaultAllocator;
    bx::FileWriter defaultWriter;
    Overlay overlay;

    bool doCaptureFrame = false;

    bgfx::Init bgfxInit;

public:
    std::string currentRenderer;

    // ---

    void init();
    void reset();
    void prepareFFNxLogo();
    void prepareShadowMap();
    void prepareSpecularIbl(char* fullpath = nullptr);
    void prepareDiffuseIbl(char* fullpath = nullptr);
    void prepareEnvBrdf();
    void prepareGamutLUTs();
    void LoadGamutLUT(GamutLUTIndexType whichLUT);
    void shutdown();

    void clearShadowMap();
    void drawToShadowMap(bool uniformsAlreadyAttached = false, bool texturesAlreadyAttached = false);
    void drawWithLighting(bool uniformsAlreadyAttached = false, bool texturesAlreadyAttached = false, bool keepBindings = false);
    void drawFieldShadow();
    void draw(bool uniformsAlreadyAttached = false, bool texturesAlreadyAttached = false, bool keepBindings = false);
    void discardAllBindings();
    void drawOverlay();
    void drawFFNxLogo(float fade);
    void show();

    void printText(uint16_t x, uint16_t y, uint32_t attr, const char* text);
    void toggleCaptureFrame();

    // ---

    const bgfx::Caps* getCaps();
    const bgfx::Stats* getStats();
    const bgfx::VertexLayout& GetVertexLayout();

    void bindVertexBuffer(struct nvertex* inVertex, vector3<float>* normals, uint32_t inCount);
    void bindIndexBuffer(WORD* inIndex, uint32_t inCount);

    bgfx::UniformHandle setUniform(RendererUniform uniform, const void* uniformValue);
    void setCommonUniforms();
    void setLightingUniforms();
    void bindTextures();

    void setScissor(uint16_t x, uint16_t y, uint16_t width, uint16_t height);
    void setClearFlags(bool doClearColor = false, bool doClearDepth = false);
    void setBackgroundColor(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 0.0f);

    uint32_t createTexture(uint8_t* data, size_t width, size_t height, int stride = 0, RendererTextureType type = RendererTextureType::BGRA, bool isSrgb = false, bool copyData = true);
    uint32_t createTexture(char* filename, uint32_t* width, uint32_t* height, uint32_t* mipCount, bool isSrgb = false);
    bimg::ImageContainer* createImageContainer(const char* filename, bimg::TextureFormat::Enum targetFormat = bimg::TextureFormat::Enum::Count);
    bimg::ImageContainer* createImageContainer(cmrc::file* file, bimg::TextureFormat::Enum targetFormat = bimg::TextureFormat::Enum::Count);
    bgfx::TextureHandle createTextureHandle(char* filename, uint32_t* width, uint32_t* height, uint32_t* mipCount, bool isSrgb = false);
    bgfx::TextureHandle createTextureHandle(cmrc::file* file, char* filename, uint32_t* width, uint32_t* height, uint32_t* mipCount, bool isSrgb = false);
    uint32_t createTextureLibPng(char* filename, uint32_t* width, uint32_t* height, bool isSrgb = false);
    bool saveTexture(const char* filename, uint32_t width, uint32_t height, const void* data);
    void deleteTexture(uint16_t texId);
    void useTexture(uint16_t texId, uint32_t slot = 0);
    uint32_t createBlitTexture(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
    void blitTexture(uint16_t dest, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
    void zoomBackendFrameBuffer(int x, int y, int width, int height);
    void clearDepthBuffer();

    void isMovie(bool flag = false);
    void isTLVertex(bool flag = false);
    void setBlendMode(RendererBlendMode mode = RendererBlendMode::BLEND_NONE);
    void isTexture(bool flag = false);
    void isFBTexture(bool flag = false);
    void isFullRange(bool flag = false);
    void isYUV(bool flag = false);
    void doModulateAlpha(bool flag = false);
    void doTextureFiltering(bool flag = false);
    void doMirrorTextureWrap(bool flag = false);
    void isExternalTexture(bool flag = false);
    bool isHDR();
    void setColorMatrix(ColorMatrixType cmtype = COLORMATRIX_BT601);
    void setColorGamut(ColorGamutType cgtype = COLORGAMUT_SRGB);
    void setOverallColorGamut(ColorGamutType cgtype = COLORGAMUT_SRGB);
    void setGammaType(InverseGammaFunctionType gtype = GAMMAFUNCTION_SRGB);

    // Alpha mode emulation
    void setAlphaRef(RendererAlphaFunc func = RendererAlphaFunc::ALWAYS, float ref = 0.0f);
    void doAlphaTest(bool flag = false);

    // Internal states
    void setInterpolationQualifier(RendererInterpolationQualifier qualifier = RendererInterpolationQualifier::SMOOTH);
    void setPrimitiveType(RendererPrimitiveType type = RendererPrimitiveType::PT_TRIANGLES);
    void setCullMode(RendererCullMode mode = RendererCullMode::DISABLED);
    void doDepthTest(bool flag = false);
    void doDepthWrite(bool flag = false);

    // Scissor test
    void doScissorTest(bool flag = false);

    // Wireframe mode
    void setWireframeMode(bool flag = false);

    // Viewport
    void setViewMatrix(struct matrix* matrix);
    float* getViewMatrix();
    float* getInvViewMatrix();
    bool isViewMatrixSet();
    void resetViewMatrixFlag();
    void setWorldViewMatrix(struct matrix* matrix, bool calculateNormalMatrix = true);
    float* getWorldViewMatrix();
    float* getNormalMatrix();
    void setD3DViweport(struct matrix* matrix);
    void setD3DProjection(struct matrix* matrix);

    // Internal coord calculation
    uint16_t getInternalCoordX(uint16_t inX);
    uint16_t getInternalCoordY(uint16_t inY);

    // Internal scaling factor
    uint16_t getScalingFactor();

    // Day-night time cycle
    void setTimeColor(bx::Vec3 color);
    void setTimeEnabled(bool flag = false);
    void setTimeFilterEnabled(bool flag = false);
    bool isTimeFilterEnabled();

    // Worldmap
    void setSphericalWorldRate(float value = 0.0f);
    void setFogEnabled(bool flag = false);
    bool isFogEnabled();

    // Game lighting
    void setGameLightData(light_data* lightdata = nullptr);

    static bool doesItFitInMemory(size_t size);
};

extern Renderer newRenderer;
