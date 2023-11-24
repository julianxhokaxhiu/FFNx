/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
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

#include <vector>
#include <map>
#include <string>

#include "common.h"
#include "globals.h"
#include "renderer.h"

struct Material
{
    std::vector<bgfx::TextureHandle> baseColorTexHandles;
    std::vector<bgfx::TextureHandle> normalTexHandles;
    std::vector<bgfx::TextureHandle> pbrTexHandles;
    int texIndex = 0;
    int frameInterval = 0;
};

struct Shape
{
    std::vector<nvertex> vertices;
    std::vector<vector3<float>> normals;
    std::vector<uint32_t> indices;
    vector3<float> min;
    vector3<float> max;
    Material* pMaterial = nullptr;
    bool isDoubleSided = false;
};

class ExternalMesh
{
public:
    bool importExternalMeshGltfFile(char* file_path, char* tex_path);
    uint32_t fillExternalMeshVertexBuffer(struct nvertex* inVertex, struct vector3<float>* normals, uint32_t inCount);
    uint32_t fillExternalMeshIndexBuffer(uint32_t* inIndex, uint32_t inCount);
    void updateExternalMeshBuffers();
    void bindField3dVertexBuffer(uint32_t offset, uint32_t inCount);
    void bindField3dIndexBuffer(uint32_t offset, uint32_t inCount);
    void clearExternalMesh3dBuffers();
    void unloadExternalMesh();
    
    std::vector<Shape> shapes;
	std::map<std::string, Material> materials;
private:
    void loadConfig(const std::string& path);

    int getTextureCount(std::string tex_name);
    int getFrameInterval(std::string tex_name);

private:
    // Config
    toml::parse_result config;

    std::vector<Vertex> vertexBufferData;
    bgfx::DynamicVertexBufferHandle vertexBufferHandle = BGFX_INVALID_HANDLE;

    std::vector<uint32_t> indexBufferData;
    bgfx::DynamicIndexBufferHandle indexBufferHandle = BGFX_INVALID_HANDLE;   
};
