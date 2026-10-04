/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2026 Julian Xhokaxhiu                                   //
//    Copyright (C) 2023 Cosmos                                             //
//    Copyright (C) 2023 Tang-Tang Zhou                                     //
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

#include "external_mesh.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <filesystem>

#include "cfg.h"
#include "log.h"
#include "utils.h"

#define CGLTF_IMPLEMENTATION
#include "cgltf.h"

// A number from a glTF "extras" JSON object (e.g. {"spring_drag": 0.4}); value is left alone when absent
static bool readExtrasFloat(const std::string& extras, const char* key, float& value)
{
    size_t position = extras.find(std::string("\"") + key + "\"");
    if (position == std::string::npos) return false;

    position = extras.find(':', position);
    if (position == std::string::npos) return false;

    const char* start = extras.c_str() + position + 1;
    char* end = nullptr;
    float number = std::strtof(start, &end);
    if (end == start) return false;

    value = number;
    return true;
}

// Spring settings from a model config table (stiffness, drag, gravity, radius); values it lacks are left alone
static bool readConfigSpringSettings(toml::node_view<toml::node> table, Joint& joint)
{
    bool found = false;
    if (auto value = table["stiffness"].value<double>()) { joint.springStiffness = static_cast<float>(*value); found = true; }
    if (auto value = table["drag"].value<double>()) { joint.springDrag = static_cast<float>(*value); found = true; }
    if (auto value = table["gravity"].value<double>()) { joint.springGravity = static_cast<float>(*value); found = true; }
    if (auto value = table["radius"].value<double>()) { joint.springRadius = static_cast<float>(*value); found = true; }
    return found;
}

// Name used to find an image's DDS files: its file name without extension
// (e.g. "textures/cloud_0.png" -> "cloud_0"), or its name when it has no file
static std::string getImageTextureName(const cgltf_image* image)
{
    if (image == nullptr) return "";

    std::string path = image->uri != nullptr ? image->uri : (image->name != nullptr ? image->name : "");
    std::string filename = path.substr(path.find_last_of("/") + 1);
    return filename.substr(0, filename.find_last_of("."));
}

void createJointHierarchy(Skin* pSkin, int parentIndex, cgltf_node* pJointNode, int* curIndex)
{
    Joint outJoint;

    outJoint.rotation.x = pJointNode->rotation[0];
    outJoint.rotation.y = pJointNode->rotation[1];
    outJoint.rotation.z = pJointNode->rotation[2];
    outJoint.rotation.w = pJointNode->rotation[3];

    outJoint.translation.x = pJointNode->translation[0];
    outJoint.translation.y = pJointNode->translation[1];
    outJoint.translation.z = pJointNode->translation[2];

    outJoint.name = pJointNode->name;
    outJoint.extras = pJointNode->extras.data != nullptr ? pJointNode->extras.data : "";

    outJoint.parentJointIndex = parentIndex;

    pSkin->joints[*curIndex] = outJoint;
    auto newParentIndex = *curIndex;
    (*curIndex)++;

    for (int i = 0; i < pJointNode->children_count; ++i)
    {
        createJointHierarchy(pSkin, newParentIndex, pJointNode->children[i], curIndex);
    }    
}

bool ExternalMesh::importExternalMeshGltfFile(char* file_path, char* tex_path, bool isZUp)
{
	cgltf_options options = {0};
	cgltf_data* data = NULL;
	cgltf_result result = cgltf_parse_file(&options, file_path, &data);
	if (result != cgltf_result_success)
	{
		ffnx_error("External mesh: could not parse %s (cgltf error %d)\n", file_path, result);
		return false;
	}

	result = cgltf_load_buffers(&options, data, file_path);
	if (result != cgltf_result_success)
	{
		ffnx_error("External mesh: could not load buffers for %s (cgltf error %d)\n", file_path, result);
		cgltf_free(data);
		return false;
	}

    std::string modelPath = file_path;
    std::string modelFolder = modelPath.substr(0, modelPath.find_last_of("/") + 1);
    std::string modelFilename =  modelPath.substr(modelPath.find_last_of("/") + 1);
    std::string modelFilenameWithoutExt =  modelFilename.substr(0, modelFilename.find_last_of("."));
    std::string configPath = modelFolder + modelFilenameWithoutExt + "_config.toml";
    loadConfig(configPath);

	for (size_t i = 0; i < data->textures_count; i++)
	{
		auto texture = data->textures[i];
		std::string name = getImageTextureName(texture.image);
		if (name.empty()) continue;

		std::string texFullPath = tex_path + name + ".dds";

        std::string modPath = !override_mod_path.empty() ? override_mod_path : mod_path;

        Material candidateMaterial;

        auto pair = materials.emplace(name, candidateMaterial);
        if(pair.second)
        {
            auto& material = (*pair.first).second;
            uint32_t width, height, mipCount = 0;

            char full_tex_path[512];

            int texCount = getTextureCount(name);
            material.frameInterval = getFrameInterval(name);
            for (int texIndex = 0; texIndex < texCount; ++texIndex)
            {
                if (texIndex != 0)
                {
                    auto nameWithoutNumber = name.substr(0, name.length() - 1);
                    _snprintf(full_tex_path, sizeof(full_tex_path), "%s/%s/world/%s%s_00.dds", basedir, modPath.c_str(), nameWithoutNumber.data(), std::to_string(texIndex + 1).data());
                }
                else
                    _snprintf(full_tex_path, sizeof(full_tex_path), "%s/%s/world/%s_00.dds", basedir, modPath.c_str(), name.data());

                auto textureHandle = newRenderer.createTextureHandle(full_tex_path, &width, &height, &mipCount);
                if (!textureHandle.idx)
                {
                    if (texIndex != 0)
                        _snprintf(full_tex_path, sizeof(full_tex_path), "%s/%s/world/%s%s.dds", basedir, modPath.c_str(), name.data(), std::to_string(texIndex + 1).data());
                    else
                        _snprintf(full_tex_path, sizeof(full_tex_path), "%s/%s/world/%s.dds", basedir, modPath.c_str(), name.data());

                    textureHandle = newRenderer.createTextureHandle(texFullPath.data(), &width, &height, &mipCount);
                    if (!textureHandle.idx) textureHandle = BGFX_INVALID_HANDLE;
                }

                if(bgfx::isValid(textureHandle))
                {
                    material.baseColorTexHandles.push_back(textureHandle);
                }
            }

            std::string nmlTexFullPath = tex_path + name + "_nml.dds";
            auto nmlTextureHandle = newRenderer.createTextureHandle(nmlTexFullPath.data(), &width, &height, &mipCount, false);
            if (!nmlTextureHandle.idx) nmlTextureHandle = BGFX_INVALID_HANDLE;
            if(bgfx::isValid(nmlTextureHandle))
            {
                material.normalTexHandles.push_back(nmlTextureHandle);
            }

            std::string pbrTexFullPath = tex_path + name + "_pbr.dds";
            auto pbrTextureHandle = newRenderer.createTextureHandle(pbrTexFullPath.data(), &width, &height, &mipCount, false);
            if (!pbrTextureHandle.idx) pbrTextureHandle = BGFX_INVALID_HANDLE;
            if(bgfx::isValid(pbrTextureHandle))
            {
                material.pbrTexHandles.push_back(pbrTextureHandle);
            }
        }
	}

	for (size_t i = 0; i < data->meshes_count; i++)
	{
		cgltf_mesh mesh = data->meshes[i];

		for (size_t j = 0; j < mesh.primitives_count; j++)
		{
			Shape outShape;

			cgltf_primitive primitive = mesh.primitives[j];
			auto indexCount = primitive.indices->count;
			auto vertexCount = 0;

			float* posBuffer = nullptr;
			float* normalBuffer = nullptr;
			float* uvBuffer = nullptr;
			float* colorBuffer = nullptr;
            byte* jointsBuffer = nullptr;
            float * weightsBuffer = nullptr;
			for (size_t k = 0; k < primitive.attributes_count; k++)
			{
				cgltf_attribute attr = primitive.attributes[k];

				if(strcmp(attr.name, "POSITION") == 0)
				{
					vertexCount = attr.data->count;
					posBuffer = (float*)((char*)attr.data->buffer_view->buffer->data + attr.data->buffer_view->offset);
					outShape.min.x = attr.data->min[0];
					outShape.min.y = attr.data->min[1];
					outShape.min.z = attr.data->min[2];
					outShape.max.x = attr.data->max[0];
					outShape.max.y = attr.data->max[1];
					outShape.max.z = attr.data->max[2];
				}
				else if(strcmp(attr.name, "NORMAL") == 0)
				{
					normalBuffer = (float*)((char*)attr.data->buffer_view->buffer->data + attr.data->buffer_view->offset);
				}
				else if(strcmp(attr.name, "TEXCOORD_0") == 0)
				{
					uvBuffer = (float*)((char*)attr.data->buffer_view->buffer->data + attr.data->buffer_view->offset);
				}
				else if(strcmp(attr.name, "COLOR_0") == 0)
				{
					colorBuffer = (float*)((char*)attr.data->buffer_view->buffer->data + attr.data->buffer_view->offset);
				}
                else if(strcmp(attr.name, "JOINTS_0") == 0)
				{
					jointsBuffer = (byte*)((char*)attr.data->buffer_view->buffer->data + attr.data->buffer_view->offset);
				}
                else if(strcmp(attr.name, "WEIGHTS_0") == 0)
				{
					weightsBuffer = (float*)((char*)attr.data->buffer_view->buffer->data + attr.data->buffer_view->offset);
				}
			}

            auto material = primitive.material;
            outShape.isDoubleSided = material != nullptr && material->double_sided;
            if (mesh.name != nullptr)
            {
                outShape.name = mesh.name;
                std::transform(outShape.name.begin(), outShape.name.end(), outShape.name.begin(), [](unsigned char c) { return std::toupper(c); });
            }
            if (material != nullptr && material->alpha_mode == cgltf_alpha_mode_mask) outShape.alphaMode = ShapeAlphaMode::MASK_MODE;
            else if (material != nullptr && material->alpha_mode == cgltf_alpha_mode_blend) outShape.alphaMode = ShapeAlphaMode::BLEND_MODE;
            if (material != nullptr) outShape.alphaCutoff = material->alpha_cutoff;

			// Look the texture up by the same name its DDS files were loaded under
			auto texture = material != nullptr ? material->pbr_metallic_roughness.base_color_texture.texture : nullptr;
			if(texture != nullptr)
			{
				std::string texName = getImageTextureName(texture->image);
				if(materials.contains(texName)) outShape.pMaterial = &materials[texName];
			}

			// Parts without a material are drawn white
			const cgltf_float defaultBaseColorFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
			const cgltf_float* baseColorFactor = material != nullptr ? material->pbr_metallic_roughness.base_color_factor : defaultBaseColorFactor;
			for (int vertexIndex = 0; vertexIndex < vertexCount; vertexIndex++)
			{
				struct nvertex vertex;
				vertex._.x = posBuffer[3 * vertexIndex];
                if (isZUp)
                {
                    vertex._.y = posBuffer[3 * vertexIndex + 1];
                    vertex._.z = posBuffer[3 * vertexIndex + 2];
                }
                else
                {
                    vertex._.y = posBuffer[3 * vertexIndex + 2];
                    vertex._.z = posBuffer[3 * vertexIndex + 1];
                }

				vertex.color.w = 1.0f;
				vertex.color.r = static_cast<char>(baseColorFactor[0] * 255);
				vertex.color.g = static_cast<char>(baseColorFactor[1] * 255);
				vertex.color.b = static_cast<char>(baseColorFactor[2] * 255);
				vertex.color.a = static_cast<char>(baseColorFactor[3] * 255);

                if (uvBuffer != nullptr)
                {
				    vertex.u = uvBuffer[2 * vertexIndex];
				    vertex.v = uvBuffer[2 * vertexIndex + 1];
                }
                else
                {
                    vertex.u = 0.0f;
				    vertex.v = 0.0f;
                }

				outShape.vertices.push_back(vertex);

				struct vector3<float> normal;

				normal.x = normalBuffer[3 * vertexIndex];
                if (isZUp)
                {
                    normal.y = normalBuffer[3 * vertexIndex + 1];
				    normal.z = normalBuffer[3 * vertexIndex + 2];
                }
                else
                {
                    normal.y = normalBuffer[3 * vertexIndex + 2];
				    normal.z = normalBuffer[3 * vertexIndex + 1];
                }

				outShape.normals.push_back(normal);

                if (jointsBuffer != nullptr)
                {
                    struct vector4<float> joints;
         
                    joints.x = jointsBuffer[4 * vertexIndex];
                    joints.y = jointsBuffer[4 * vertexIndex + 1];
                    joints.z = jointsBuffer[4 * vertexIndex + 2];
                    joints.w = jointsBuffer[4 * vertexIndex + 3];

                    outShape.joints.push_back(joints);
                }

                if (weightsBuffer != nullptr)
                {
                    struct vector4<float> weights;

                    weights.x = weightsBuffer[4 * vertexIndex];
                    weights.y = weightsBuffer[4 * vertexIndex + 1];
                    weights.z = weightsBuffer[4 * vertexIndex + 2];
                    weights.w = weightsBuffer[4 * vertexIndex + 3];

                    outShape.weights.push_back(weights);
                }
			}

			if(primitive.indices->component_type == cgltf_component_type_r_16u)
			{
				auto indexBuffer = (unsigned short*)((char*)primitive.indices->buffer_view->buffer->data + primitive.indices->buffer_view->offset);

				for (int id = 0; id < indexCount; ++id)
				{
					outShape.indices.push_back(indexBuffer[id]);
				}
			}else if(primitive.indices->component_type == cgltf_component_type_r_32u)
			{
				auto indexBuffer = (unsigned int*)((char*)primitive.indices->buffer_view->buffer->data + primitive.indices->buffer_view->offset);
				for (int id = 0; id < indexCount; ++id)
				{
					outShape.indices.push_back(indexBuffer[id]);
				}
			}

            fillExternalMeshVertexBuffer(outShape.vertices.data(), outShape.normals.data(), outShape.joints.data(), outShape.weights.data(), outShape.vertices.size());
            fillExternalMeshIndexBuffer(outShape.indices.data(), outShape.indices.size());

            shapes.push_back(outShape);
		}
	}

    for (size_t i = 0; i < data->skins_count; i++)
	{
        Skin outSkin;

        cgltf_skin skin = data->skins[i];

        outSkin.joints.resize(skin.joints_count);
        int curIndex = 0;
        while (curIndex != skin.joints_count)
        {
            createJointHierarchy(&outSkin, -1, skin.joints[curIndex], &curIndex);
        }

        auto joint_count = outSkin.joints.size();
        for (size_t j = 0; j < joint_count; j++)
		{
            auto inverseBindPoseMatrixBuffer = (float*)((char*)skin.inverse_bind_matrices->buffer_view->buffer->data + skin.inverse_bind_matrices->buffer_view->offset);
            memcpy(outSkin.joints[j].inverseBindPoseMatrix, &inverseBindPoseMatrixBuffer[16 * j], sizeof(float) * 16);
        }
		/*for (size_t j = 0; j < skin.joints_count; j++)
		{
			Joint outJoint;

            auto joint = skin.joints[j];

            outJoint.rotation.x = joint->rotation[0];
            outJoint.rotation.y = joint->rotation[1];
            outJoint.rotation.z = joint->rotation[2];
            outJoint.rotation.w = joint->rotation[3];

            outJoint.translation.x = joint->translation[0];
            outJoint.translation.y = joint->translation[1];
            outJoint.translation.z = joint->translation[2];

            outSkin.joints.push_back(outJoint);
        }*/

        skins.push_back(outSkin);
    }

    // Joints named after the game's bones (bone_00, bone_01, ...: KimeraCS battle exports)
    if (!skins.empty())
    {
        for (size_t j = 0; j < skins[0].joints.size(); j++)
        {
            const std::string& name = skins[0].joints[j].name;
            if (name.size() < 6 || _strnicmp(name.c_str(), "bone_", 5) != 0) continue;

            char* end = nullptr;
            long boneIndex = strtol(name.c_str() + 5, &end, 10);
            if (end == name.c_str() + 5 || *end != 0 || boneIndex < 0 || boneIndex >= 512) continue;

            if (gameBoneJoints.size() <= static_cast<size_t>(boneIndex)) gameBoneJoints.resize(boneIndex + 1, -1);
            gameBoneJoints[boneIndex] = static_cast<int>(j);
        }
    }

    // Battle weapons: parts skinned only to a joint named "weapon" (all weapons share it; one is equipped)
    if (!skins.empty())
    {
        int weaponJoint = -1;
        for (size_t j = 0; j < skins[0].joints.size(); j++)
            if (!_stricmp(skins[0].joints[j].name.c_str(), "weapon")) weaponJoint = static_cast<int>(j);

        if (weaponJoint >= 0)
        {
            for (auto& shape : shapes)
            {
                bool onlyWeapon = !shape.joints.empty() && shape.joints.size() == shape.weights.size();
                for (size_t v = 0; v < shape.joints.size() && onlyWeapon; v++)
                {
                    const auto& j = shape.joints[v];
                    const auto& w = shape.weights[v];
                    float indices[4] = { j.x, j.y, j.z, j.w }, weights[4] = { w.x, w.y, w.z, w.w };
                    for (int k = 0; k < 4; k++)
                        if (weights[k] > 0.0f && static_cast<int>(indices[k]) != weaponJoint) onlyWeapon = false;
                }
                shape.isWeapon = onlyWeapon;
            }
        }
    }

    // Spring bones: joints whose name contains "spring" (any case), and every joint below them
    for (auto& skin : skins)
    {
        for (size_t j = 0; j < skin.joints.size(); j++)
        {
            Joint& joint = skin.joints[j];

            std::string lowerName = joint.name;
            std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), [](unsigned char c) { return std::tolower(c); });
            joint.isSpring = lowerName.find("spring") != std::string::npos
                || (joint.parentJointIndex >= 0 && skin.joints[joint.parentJointIndex].isSpring);
            if (!joint.isSpring) continue;

            // The bone points at its first child, or (at the end of a chain) its own length further on
            joint.springTail = joint.translation;
            for (size_t c = j + 1; c < skin.joints.size(); c++)
            {
                if (skin.joints[c].parentJointIndex == static_cast<int>(j))
                {
                    joint.springTail = skin.joints[c].translation;
                    break;
                }
            }

            float length = std::sqrt(joint.springTail.x * joint.springTail.x + joint.springTail.y * joint.springTail.y + joint.springTail.z * joint.springTail.z);
            if (length <= 0.0f) joint.isSpring = false;
            else hasSpringBones = true;

            // Settings, later sources winning (see Joint): defaults or the parent's, custom properties, config
            bool chainStart = joint.parentJointIndex < 0 || !skin.joints[joint.parentJointIndex].isSpring;
            if (chainStart)
            {
                joint.springStiffness = SPRING_BONE_STIFFNESS;
                joint.springDrag = SPRING_BONE_DRAG;
                joint.springGravity = SPRING_BONE_GRAVITY_FACTOR;
                joint.springRadius = SPRING_BONE_RADIUS;
                readConfigSpringSettings(config["spring_bones"], joint);
            }
            else
            {
                const Joint& parent = skin.joints[joint.parentJointIndex];
                joint.springStiffness = parent.springStiffness;
                joint.springDrag = parent.springDrag;
                joint.springGravity = parent.springGravity;
                joint.springRadius = parent.springRadius;
            }

            bool fromProperties = readExtrasFloat(joint.extras, "spring_stiffness", joint.springStiffness);
            fromProperties |= readExtrasFloat(joint.extras, "spring_drag", joint.springDrag);
            fromProperties |= readExtrasFloat(joint.extras, "spring_gravity", joint.springGravity);
            fromProperties |= readExtrasFloat(joint.extras, "spring_radius", joint.springRadius);
            bool fromConfig = readConfigSpringSettings(config["spring_bones"][joint.name], joint);
            joint.springStiffness = std::clamp(joint.springStiffness, 0.0f, 1.0f);
            joint.springDrag = std::clamp(joint.springDrag, 0.0f, 1.0f);
            joint.springRadius = std::max(joint.springRadius, 0.0f);

            if ((trace_all || trace_loaders) && joint.isSpring && (chainStart || fromProperties || fromConfig))
                ffnx_trace("External mesh: spring bone %s: stiffness %.3f, drag %.3f, gravity %.3f, radius %.3f%s%s\n", joint.name.c_str(),
                    joint.springStiffness, joint.springDrag, joint.springGravity, joint.springRadius,
                    fromProperties ? " (custom properties)" : "", fromConfig ? " (config)" : "");
        }
    }

    if (hasSpringBones) setupSpringColliders();

    // The skeleton's root node is the non-joint parent of its top joint. Its channels carry the root motion
    // (the height that stands the model on the floor, moves like jumps)
    cgltf_node* rootNode = nullptr;
    if (data->skins_count > 0)
    {
        const cgltf_skin& skin = data->skins[0];
        for (size_t j = 0; j < skin.joints_count && rootNode == nullptr; j++)
        {
            cgltf_node* parent = skin.joints[j]->parent;
            if (parent != nullptr && std::find(skin.joints, skin.joints + skin.joints_count, parent) == skin.joints + skin.joints_count)
                rootNode = parent;
        }
    }

    for (size_t i = 0; i < data->animations_count; i++)
    {
        cgltf_animation anim = data->animations[i];
        if (anim.name == nullptr) continue;

        // Matched case-insensitively against the game's animation names: the 4 letters of a field .a file, or
        // the whole name of a battle animation (ANIM_NN: its number in the model's list; <PACK>_NN: a limit
        // break animation from that pack, e.g. BLAVER_02)
        std::string animName = anim.name;
        std::transform(animName.begin(), animName.end(), animName.begin(), [](unsigned char c) { return std::toupper(c); });
        if (animName.find('_') == std::string::npos) animName = animName.substr(0, 4);

        Animation outAnim;
        std::map<const cgltf_accessor*, std::vector<float>> timelines;
        
        int jointCount = skins[0].joints.size();
        outAnim.keyFrames.resize(jointCount);
        for (size_t j = 0; j < anim.channels_count; j++)
        {
            auto channel = anim.channels[j];

            int targetJointIndex = -1;
            for (int jointIndex = 0; jointIndex < jointCount; ++jointIndex)
            {
                const auto& joint = skins[0].joints[jointIndex];
                if(channel.target_node->name == joint.name)
                {
                    targetJointIndex = jointIndex;
                    break;
                }
            }

            float* samplerBuffer = (float*)((char*)channel.sampler->output->buffer_view->buffer->data + channel.sampler->output->buffer_view->offset);

            // Key times, used to stretch the animation when its key count differs from the game's frame count.
            // Channels usually share one timeline, so each is read once.
            auto cachedTimes = timelines.find(channel.sampler->input);
            if (cachedTimes == timelines.end())
            {
                std::vector<float> times(channel.sampler->input->count);
                cgltf_accessor_unpack_floats(channel.sampler->input, times.data(), times.size());
                cachedTimes = timelines.emplace(channel.sampler->input, std::move(times)).first;
            }
            const std::vector<float>& keyTimes = cachedTimes->second;

            if (!keyTimes.empty())
            {
                if (outAnim.keyCount == 0 || keyTimes.front() < outAnim.startTime) outAnim.startTime = keyTimes.front();
                if (outAnim.keyCount == 0 || keyTimes.back() > outAnim.endTime) outAnim.endTime = keyTimes.back();
                outAnim.keyCount = std::max(outAnim.keyCount, keyTimes.size());
            }

            if(targetJointIndex == -1)
            {
                if (rootNode != nullptr && channel.target_node == rootNode)
                {
                    if (channel.target_path == cgltf_animation_path_type_translation)
                    {
                        for (int k = 0; k < channel.sampler->output->count; ++k)
                            outAnim.rootTranslation.push_back({ samplerBuffer[k * 3 + 0], samplerBuffer[k * 3 + 1], samplerBuffer[k * 3 + 2] });
                        outAnim.rootTranslationTimes = keyTimes;
                    }
                    else if (channel.target_path == cgltf_animation_path_type_rotation)
                    {
                        for (int k = 0; k < channel.sampler->output->count; ++k)
                            outAnim.rootRotation.push_back({ samplerBuffer[k * 4 + 0], samplerBuffer[k * 4 + 1], samplerBuffer[k * 4 + 2], samplerBuffer[k * 4 + 3] });
                        outAnim.rootRotationTimes = keyTimes;
                    }
                }

                continue;
            }
        
            KeyFrame& outKeyFrame = outAnim.keyFrames[targetJointIndex];
            if (channel.target_path == cgltf_animation_path_type_translation)
            {     
                for (int k = 0; k < channel.sampler->output->count; ++k)
                {
                    vector3<float> translation;
                    translation.x = samplerBuffer[k * 3 + 0];
                    translation.y = samplerBuffer[k * 3 + 1];
                    translation.z = samplerBuffer[k * 3 + 2];
                    outKeyFrame.translation.push_back(translation);  
                }
                outKeyFrame.translationTimes = keyTimes;
                outKeyFrame.targetJointIndex = targetJointIndex;
            }
            else if (channel.target_path == cgltf_animation_path_type_rotation)
            {
                //KeyFrame outKeyFrame;
                for (int k = 0; k < channel.sampler->output->count; ++k)
                {
                    vector4<float> rotation;
                    rotation.x = samplerBuffer[k * 4 + 0];
                    rotation.y = samplerBuffer[k * 4 + 1];
                    rotation.z = samplerBuffer[k * 4 + 2];
                    rotation.w = samplerBuffer[k * 4 + 3];
                    outKeyFrame.rotation.push_back(rotation);  
                }
                outKeyFrame.rotationTimes = keyTimes;
            }        
        }

        animations[animName] = std::move(outAnim);
    }

    updateExternalMeshBuffers();

    cgltf_free(data);

	return true;
}

uint32_t ExternalMesh::fillExternalMeshVertexBuffer(struct nvertex* inVertex, struct vector3<float>* normals, struct vector4<float>* joints, struct vector4<float>* weights, uint32_t inCount)
{
    if (!bgfx::isValid(vertexBufferHandle)) vertexBufferHandle = bgfx::createDynamicVertexBuffer(inCount, newRenderer.GetVertexLayout(), BGFX_BUFFER_ALLOW_RESIZE);

    uint32_t currentOffset = vertexBufferData.size();

    for (uint32_t idx = 0; idx < inCount; idx++)
    {
        vertexBufferData.push_back(Vertex());

        vertexBufferData[currentOffset + idx].x = inVertex[idx]._.x;
        vertexBufferData[currentOffset + idx].y = inVertex[idx]._.y;
        vertexBufferData[currentOffset + idx].z = inVertex[idx]._.z;
        vertexBufferData[currentOffset + idx].w = (::isinf(inVertex[idx].color.w) ? 1.0f : inVertex[idx].color.w);
        vertexBufferData[currentOffset + idx].bgra = inVertex[idx].color.color;
        vertexBufferData[currentOffset + idx].u = inVertex[idx].u;
        vertexBufferData[currentOffset + idx].v = inVertex[idx].v;

        if (normals)
        {
            vertexBufferData[currentOffset + idx].nx = normals[idx].x;
            vertexBufferData[currentOffset + idx].ny = normals[idx].y;
            vertexBufferData[currentOffset + idx].nz = normals[idx].z;
        }

        if (joints)
        {
            vertexBufferData[currentOffset + idx].bone_indices[0] = static_cast<uint8_t>(joints[idx].x);
            vertexBufferData[currentOffset + idx].bone_indices[1] = static_cast<uint8_t>(joints[idx].y);
            vertexBufferData[currentOffset + idx].bone_indices[2] = static_cast<uint8_t>(joints[idx].z);
            vertexBufferData[currentOffset + idx].bone_indices[3] = static_cast<uint8_t>(joints[idx].w);
        }

        if (weights)
        {
            vertexBufferData[currentOffset + idx].bone_weights[0] = weights[idx].x;
            vertexBufferData[currentOffset + idx].bone_weights[1] = weights[idx].y;
            vertexBufferData[currentOffset + idx].bone_weights[2] = weights[idx].z;
            vertexBufferData[currentOffset + idx].bone_weights[3] = weights[idx].w;
        }

        if (vertex_log && idx == 0) ffnx_trace("%s: %u [XYZW(%f, %f, %f, %f), BGRA(%08x), UV(%f, %f)]\n", __func__, idx, vertexBufferData[currentOffset + idx].x, vertexBufferData[currentOffset + idx].y, vertexBufferData[currentOffset + idx].z, vertexBufferData[currentOffset + idx].w, vertexBufferData[currentOffset + idx].bgra, vertexBufferData[currentOffset + idx].u, vertexBufferData[currentOffset + idx].v);
        if (vertex_log && idx == 1) ffnx_trace("%s: See the rest on RenderDoc.\n", __func__);
    }

    return currentOffset;
};

uint32_t ExternalMesh::fillExternalMeshIndexBuffer(uint32_t* inIndex, uint32_t inCount)
{
    if (!bgfx::isValid(indexBufferHandle)) indexBufferHandle = bgfx::createDynamicIndexBuffer(inCount, BGFX_BUFFER_ALLOW_RESIZE | BGFX_BUFFER_INDEX32);

    uint32_t currentOffset = indexBufferData.size();

    for (uint32_t idx = 0; idx < inCount; idx++)
    {
        indexBufferData.push_back(inIndex[idx]);
    }

    return currentOffset;
};

void ExternalMesh::updateExternalMeshBuffers()
{
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
}

void ExternalMesh::bindField3dVertexBuffer(uint32_t offset, uint32_t inCount)
{
    bgfx::setVertexBuffer(0, vertexBufferHandle, offset, inCount);
}

void ExternalMesh::bindField3dIndexBuffer(uint32_t offset, uint32_t inCount)
{
    bgfx::setIndexBuffer(indexBufferHandle, offset, inCount);
}

void ExternalMesh::clearExternalMesh3dBuffers()
{
    vertexBufferData.clear();
    vertexBufferData.shrink_to_fit();

    indexBufferData.clear();
    indexBufferData.shrink_to_fit();
}

void ExternalMesh::unloadExternalMesh()
{
    for (const auto& mat : materials)
    {
        for (const auto& tex : mat.second.baseColorTexHandles)
        {
            if (bgfx::isValid(tex))
                bgfx::destroy(tex);
        }
        for (const auto& tex : mat.second.normalTexHandles)
        {
            if (bgfx::isValid(tex))
                bgfx::destroy(tex);
        }
        for (const auto& tex : mat.second.pbrTexHandles)
        {
            if (bgfx::isValid(tex))
                bgfx::destroy(tex);
        }
    }
    shapes.clear();
    materials.clear();
    clearExternalMesh3dBuffers();
}

void ExternalMesh::loadConfig(const std::string& path)
{
    try
    {
        config = toml::parse_file(path);
    }
    catch (const toml::parse_error &err)
    {
        config = toml::parse("");
    }
}

int ExternalMesh::getTextureCount(std::string tex_name)
{
    auto node = config[tex_name];
    if(node)
    {
        if (auto sub_node = node["num_textures"]) return sub_node.value_or(0);
    }

    return 1;
}

int ExternalMesh::getFrameInterval(std::string tex_name)
{
    auto node = config[tex_name];
    if(node)
    {
        if (auto sub_node = node["frame_interval"]) return sub_node.value_or(0);
    }

    return 0;
}

// The gltf root node's keys for this point of an animation. Returns false when the animation doesn't animate the
// root node, so the game's own root motion is used.
bool ExternalMesh::getRootMotionSample(const std::string& animName, const AnimationPosition& position, vector3<float>& translation, vector4<float>& rotation) const
{
    auto it = animations.find(animName);
    if (it == animations.end()) return false;

    const Animation& anim = it->second;
    if (anim.rootTranslation.empty() || anim.rootRotation.empty()) return false;

    translation = sampleTranslation(anim.rootTranslationTimes, anim.rootTranslation, position, anim.rootTranslation.front());
    rotation = sampleRotation(anim.rootRotationTimes, anim.rootRotation, position, anim.rootRotation.front());
    return true;
}

std::string ExternalMesh::animationForFrameCount(int frameCount) const
{
    // One key per game frame, allowing the up to three loop-closing keys KimeraCS's 60 fps exports add
    std::string match;
    int matches = 0;
    for (const auto& [name, anim] : animations)
    {
        if (frameCount > 0 && anim.keyCount >= (size_t)frameCount && anim.keyCount <= (size_t)frameCount + 3)
        {
            match = name;
            matches++;
        }
    }
    if (matches == 1) return match;

    if (animations.size() == 1) return animations.begin()->first;

    return "";
}

// Builds the game's root matrix (as its root animation would) from a gltf root translation and rotation.
// translationScale converts gltf units to the game's, normally the scale the mesh is drawn with.
void buildRootMatrix(const vector3<float>& t, const vector4<float>& q, float translationScale, struct matrix* outMatrix)
{
    // glTF rotation as a column-vector matrix
    float r[3][3] = {
        { 1 - 2 * (q.y * q.y + q.z * q.z), 2 * (q.x * q.y - q.z * q.w), 2 * (q.x * q.z + q.y * q.w) },
        { 2 * (q.x * q.y + q.z * q.w), 1 - 2 * (q.x * q.x + q.z * q.z), 2 * (q.y * q.z - q.x * q.w) },
        { 2 * (q.x * q.z - q.y * q.w), 2 * (q.y * q.z + q.x * q.w), 1 - 2 * (q.x * q.x + q.y * q.y) }
    };

    // glTF -> game: a 180 degree turn around Z (X and Y mirrored) for both the rotation and the translation.
    // The game's matrices are row-vector, so the rotation is stored transposed.
    const float flip[3] = { -1.0f, -1.0f, 1.0f };
    for (int row = 0; row < 3; ++row)
        for (int col = 0; col < 3; ++col)
            outMatrix->m[col][row] = flip[row] * r[row][col];

    outMatrix->_14 = 0.0f;
    outMatrix->_24 = 0.0f;
    outMatrix->_34 = 0.0f;
    outMatrix->_41 = -t.x * translationScale;
    outMatrix->_42 = -t.y * translationScale;
    outMatrix->_43 = t.z * translationScale;
    outMatrix->_44 = 1.0f;
}

AnimationPosition getAnimationPosition(const Animation& anim, int frame, int frameCount, float clockSeconds)
{
    AnimationPosition position;

    // The game holds a single frame (e.g. an idle) but the gltf animates: loop it on its own clock
    if (frameCount <= 1 && anim.keyCount > 1 && anim.endTime > anim.startTime)
    {
        position.useKeyIndex = false;
        position.time = anim.startTime + std::fmod(std::max(clockSeconds, 0.0f), anim.endTime - anim.startTime);
        return position;
    }

    // One key per game frame (or a single held game frame): show the key of the game's frame, as before.
    // Up to three extra keys also count: KimeraCS's 60 fps exports end with loop-closing in-between keys after the
    // last frame (one for fields at 2x, three for battle at 4x), which stretching would wrongly blend into the end
    // of one-shot animations and shift against the game's root motion (feet sliding).
    size_t frames = static_cast<size_t>(std::max(frameCount, 0));
    if (frameCount <= 1 || (anim.keyCount >= frames && anim.keyCount <= frames + 3) || anim.endTime <= anim.startTime)
    {
        position.keyIndex = frame;
        return position;
    }

    // Otherwise stretch the gltf timeline over the game animation, first frame to first key, last to last
    float progress = std::clamp(static_cast<float>(frame) / static_cast<float>(frameCount - 1), 0.0f, 1.0f);

    position.useKeyIndex = false;
    position.time = anim.startTime + progress * (anim.endTime - anim.startTime);
    return position;
}

// The two keys around a time, and how far the time is between them (0 = first, 1 = second)
static void findKeys(const std::vector<float>& times, size_t valueCount, float time, size_t& first, size_t& second, float& blend)
{
    size_t count = std::min(times.size(), valueCount);

    second = std::upper_bound(times.begin(), times.begin() + count, time) - times.begin();
    if (second == 0)
    {
        first = second = 0;
        blend = 0.0f;
        return;
    }
    if (second >= count)
    {
        first = second = count - 1;
        blend = 0.0f;
        return;
    }

    first = second - 1;
    float span = times[second] - times[first];
    blend = span > 0.0f ? (time - times[first]) / span : 0.0f;
}

vector3<float> sampleTranslation(const std::vector<float>& times, const std::vector<vector3<float>>& values, const AnimationPosition& position, const vector3<float>& fallback)
{
    if (values.empty()) return fallback;

    if (position.useKeyIndex || times.empty())
        return values[std::clamp(position.keyIndex, 0, static_cast<int>(values.size()) - 1)];

    size_t first, second;
    float blend;
    findKeys(times, values.size(), position.time, first, second, blend);

    return lerpTranslation(values[first], values[second], blend);
}

vector4<float> sampleRotation(const std::vector<float>& times, const std::vector<vector4<float>>& values, const AnimationPosition& position, const vector4<float>& fallback)
{
    if (values.empty()) return fallback;

    if (position.useKeyIndex || times.empty())
        return values[std::clamp(position.keyIndex, 0, static_cast<int>(values.size()) - 1)];

    size_t first, second;
    float blend;
    findKeys(times, values.size(), position.time, first, second, blend);

    return slerpRotation(values[first], values[second], blend);
}

vector3<float> lerpTranslation(const vector3<float>& a, const vector3<float>& b, float blend)
{
    return { a.x + (b.x - a.x) * blend, a.y + (b.y - a.y) * blend, a.z + (b.z - a.z) * blend };
}

vector4<float> slerpRotation(const vector4<float>& a, vector4<float> b, float blend)
{
    // Spherical interpolation along the shorter way around
    float cosAngle = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    if (cosAngle < 0.0f)
    {
        b = { -b.x, -b.y, -b.z, -b.w };
        cosAngle = -cosAngle;
    }

    float weightA = 1.0f - blend, weightB = blend;
    if (cosAngle < 0.9995f)
    {
        float angle = std::acos(cosAngle);
        float sinAngle = std::sin(angle);
        weightA = std::sin((1.0f - blend) * angle) / sinAngle;
        weightB = std::sin(blend * angle) / sinAngle;
    }

    vector4<float> result = { a.x * weightA + b.x * weightB, a.y * weightA + b.y * weightB, a.z * weightA + b.z * weightB, a.w * weightA + b.w * weightB };
    float length = std::sqrt(result.x * result.x + result.y * result.y + result.z * result.z + result.w * result.w);
    if (length > 0.0f)
    {
        result.x /= length;
        result.y /= length;
        result.z /= length;
        result.w /= length;
    }
    return result;
}

// How far a switch between animations has blended from the previous pose (0) to the new animation (1)
float getSwitchBlendWeight(float clockSeconds)
{
    if (clockSeconds >= EXTERNAL_MESH_SWITCH_BLEND_SECONDS) return 1.0f;

    float progress = std::max(clockSeconds, 0.0f) / EXTERNAL_MESH_SWITCH_BLEND_SECONDS;
    return progress * progress * (3.0f - 2.0f * progress); // Eases in and out
}

static vector3<float> transformPoint(const vector3<float>& point, const float* matrix)
{
    return {
        point.x * matrix[0] + point.y * matrix[4] + point.z * matrix[8] + matrix[12],
        point.x * matrix[1] + point.y * matrix[5] + point.z * matrix[9] + matrix[13],
        point.x * matrix[2] + point.y * matrix[6] + point.z * matrix[10] + matrix[14]
    };
}

static float vectorLength(const vector3<float>& v)
{
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

// Moves point along the line from head so it is length away from head
static void keepLength(vector3<float>& point, const vector3<float>& head, float length)
{
    vector3<float> direction = { point.x - head.x, point.y - head.y, point.z - head.z };
    float directionLength = vectorLength(direction);
    if (directionLength <= 0.0f) return;

    point = { head.x + direction.x * length / directionLength, head.y + direction.y * length / directionLength, head.z + direction.z * length / directionLength };
}

// The point on the segment start-end closest to point
static vector3<float> closestPointOnSegment(const vector3<float>& point, const vector3<float>& start, const vector3<float>& end)
{
    vector3<float> segment = { end.x - start.x, end.y - start.y, end.z - start.z };
    float lengthSquared = segment.x * segment.x + segment.y * segment.y + segment.z * segment.z;
    float along = 0.0f;
    if (lengthSquared > 0.0f)
    {
        along = ((point.x - start.x) * segment.x + (point.y - start.y) * segment.y + (point.z - start.z) * segment.z) / lengthSquared;
        along = std::clamp(along, 0.0f, 1.0f);
    }
    return { start.x + segment.x * along, start.y + segment.y * along, start.z + segment.z * along };
}

// Pushes a point (a sphere of pointRadius) out of any capsule it is inside of; returns whether it moved
static bool pushOutOfColliders(vector3<float>& point, const std::vector<SpringCollider>& colliders, float pointRadius)
{
    bool moved = false;
    for (const auto& collider : colliders)
    {
        vector3<float> closest = closestPointOnSegment(point, collider.start, collider.end);
        vector3<float> away = { point.x - closest.x, point.y - closest.y, point.z - closest.z };
        float distance = vectorLength(away);
        float radius = collider.radius + pointRadius;
        if (distance >= radius || distance <= 0.0f) continue;

        point = { closest.x + away.x * radius / distance, closest.y + away.y * radius / distance, closest.z + away.z * radius / distance };
        moved = true;
    }
    return moved;
}

// Fits a capsule to every body joint (not a spring bone) from the vertices it mainly drives: from the joint to
// its first body child (or to the middle of its vertices at the end of a chain), as thick as its vertices'
// typical distance from that line, a bit smaller.
void ExternalMesh::setupSpringColliders()
{
    if (skins.empty()) return;
    Skin& skin = skins[0];
    size_t jointCount = skin.joints.size();

    // Joint positions in the bind pose (the mesh's space)
    std::vector<vector3<float>> bindPositions(jointCount);
    for (size_t j = 0; j < jointCount; j++)
    {
        float bindMatrix[16];
        bx::mtxInverse(bindMatrix, skin.joints[j].inverseBindPoseMatrix);
        bindPositions[j] = { bindMatrix[12], bindMatrix[13], bindMatrix[14] };
    }

    // Vertices by the joint with the largest weight on them
    std::vector<std::vector<vector3<float>>> jointVertices(jointCount);
    for (const auto& shape : shapes)
    {
        if (shape.joints.size() != shape.vertices.size() || shape.weights.size() != shape.vertices.size()) continue;

        for (size_t v = 0; v < shape.vertices.size(); v++)
        {
            const auto& joints = shape.joints[v];
            const auto& weights = shape.weights[v];
            float jointIndices[4] = { joints.x, joints.y, joints.z, joints.w };
            float jointWeights[4] = { weights.x, weights.y, weights.z, weights.w };

            int best = 0;
            for (int k = 1; k < 4; k++) if (jointWeights[k] > jointWeights[best]) best = k;

            size_t jointIndex = static_cast<size_t>(jointIndices[best]);
            if (jointWeights[best] > 0.0f && jointIndex < jointCount) jointVertices[jointIndex].push_back(shape.vertices[v]._);
        }
    }

    for (size_t j = 0; j < jointCount; j++)
    {
        Joint& joint = skin.joints[j];
        const auto& vertices = jointVertices[j];
        if (joint.isSpring || vertices.size() < 8) continue;

        vector3<float> start = bindPositions[j];
        vector3<float> end = start;
        bool hasChild = false;
        for (size_t c = j + 1; c < jointCount; c++)
        {
            if (skin.joints[c].parentJointIndex == static_cast<int>(j) && !skin.joints[c].isSpring)
            {
                end = bindPositions[c];
                hasChild = true;
                break;
            }
        }
        if (!hasChild)
        {
            vector3<float> center = {};
            for (const auto& vertex : vertices) { center.x += vertex.x; center.y += vertex.y; center.z += vertex.z; }
            end = { center.x / vertices.size(), center.y / vertices.size(), center.z / vertices.size() };
        }

        std::vector<float> distances;
        distances.reserve(vertices.size());
        for (const auto& vertex : vertices)
        {
            vector3<float> closest = closestPointOnSegment(vertex, start, end);
            distances.push_back(vectorLength({ vertex.x - closest.x, vertex.y - closest.y, vertex.z - closest.z }));
        }
        std::nth_element(distances.begin(), distances.begin() + distances.size() / 2, distances.end());
        float radius = distances[distances.size() / 2] * SPRING_BONE_COLLIDER_RADIUS_SCALE;
        if (radius <= 0.0f) continue;

        // Stored in the joint's own space so it follows the animated joint
        joint.colliderStart = transformPoint(start, joint.inverseBindPoseMatrix);
        joint.colliderEnd = transformPoint(end, joint.inverseBindPoseMatrix);
        joint.colliderRadius = radius;
        joint.hasCollider = true;
    }
}

// Rough size of the model's data in memory (for the field model cache's budget)
size_t ExternalMesh::estimateMemory() const
{
    size_t bytes = vertexBufferData.capacity() * sizeof(Vertex) + indexBufferData.capacity() * sizeof(uint32_t);

    for (const auto& shape : shapes)
    {
        bytes += shape.vertices.capacity() * sizeof(nvertex) + shape.normals.capacity() * sizeof(vector3<float>)
            + shape.joints.capacity() * sizeof(vector4<float>) + shape.weights.capacity() * sizeof(vector4<float>)
            + shape.indices.capacity() * sizeof(uint32_t);
    }

    for (const auto& [name, anim] : animations)
    {
        for (const auto& keyFrame : anim.keyFrames)
        {
            bytes += keyFrame.rotation.capacity() * sizeof(vector4<float>) + keyFrame.translation.capacity() * sizeof(vector3<float>)
                + (keyFrame.rotationTimes.capacity() + keyFrame.translationTimes.capacity()) * sizeof(float) + sizeof(KeyFrame);
        }
        bytes += anim.rootRotation.capacity() * sizeof(vector4<float>) + anim.rootTranslation.capacity() * sizeof(vector3<float>)
            + (anim.rootRotationTimes.capacity() + anim.rootTranslationTimes.capacity()) * sizeof(float) + sizeof(Animation);
    }

    return bytes;
}

// Frees everything a field model holds on the graphics card (textures and its vertex and index buffers)
void ExternalMesh::destroyFieldResources()
{
    unloadExternalMesh();

    if (bgfx::isValid(vertexBufferHandle)) bgfx::destroy(vertexBufferHandle);
    if (bgfx::isValid(indexBufferHandle)) bgfx::destroy(indexBufferHandle);
    vertexBufferHandle = BGFX_INVALID_HANDLE;
    indexBufferHandle = BGFX_INVALID_HANDLE;
}

// ---------------------------------------------------------------------------------------------------------
// One character drawn with a shared field model

ExternalMeshInstance::ExternalMeshInstance(std::shared_ptr<ExternalMesh> sharedMesh) : mesh(std::move(sharedMesh))
{
    if (!mesh->skins.empty()) joints.resize(mesh->skins[0].joints.size());
}

// The state for one character drawn with this model (see ExternalMeshInstance::variants)
ExternalMeshInstance* ExternalMeshInstance::variantFor(const void* key)
{
    auto& variant = variants[key];
    if (!variant)
    {
        variant = std::make_unique<ExternalMeshInstance>(mesh);
        variant->equippedWeapon = equippedWeapon;
        variant->hidden = hidden;
        if (trace_all || trace_loaders) ffnx_trace("External mesh: character %p gets its own state for %p (%u in total)\n", this, key, (unsigned)variants.size());
    }

    activeVariant = variant.get();
    return activeVariant;
}

// Seconds since the game switched this character to animName (restarts on every switch)
float ExternalMeshInstance::getAnimationClock(const std::string& animName)
{
    auto now = std::chrono::steady_clock::now();

    if (animName != clockAnim)
    {
        // Blend from the pose shown last (the previous animation's, or a blend still in progress)
        bool hadAnim = !clockAnim.empty();
        blendFromTranslation = hadAnim ? lastTranslation : std::vector<vector3<float>>();
        blendFromRotation = hadAnim ? lastRotation : std::vector<vector4<float>>();
        blendFromHasRoot = hadAnim && lastHasRoot;
        blendFromRootTranslation = lastRootTranslation;
        blendFromRootRotation = lastRootRotation;
        lastHasRoot = false;

        clockAnim = animName;
        clockStart = now;
    }

    return std::chrono::duration<float>(now - clockStart).count();
}

// The game's root matrix for this frame from the gltf root node's keys (blended after a switch). Returns false
// when the animation doesn't animate the root node, so the game's own root motion is used.
bool ExternalMeshInstance::getRootMotionMatrix(const std::string& animName, int frame, int frameCount, float clockSeconds, float translationScale, struct matrix* outMatrix)
{
    auto it = mesh->animations.find(animName);
    if (it == mesh->animations.end()) return false;

    vector3<float> t;
    vector4<float> q;
    if (!mesh->getRootMotionSample(animName, getAnimationPosition(it->second, frame, frameCount, clockSeconds), t, q)) return false;

    float blendWeight = getSwitchBlendWeight(clockSeconds);
    if (blendWeight < 1.0f && blendFromHasRoot)
    {
        t = lerpTranslation(blendFromRootTranslation, t, blendWeight);
        q = slerpRotation(blendFromRootRotation, q, blendWeight);
    }
    lastRootTranslation = t;
    lastRootRotation = q;
    lastHasRoot = true;

    buildRootMatrix(t, q, translationScale, outMatrix);
    return true;
}

// Blends a joint's sampled pose with the pose shown before an animation switch, and remembers the result
void ExternalMeshInstance::blendJointPose(size_t jointIndex, size_t jointCount, float clockSeconds, vector3<float>& translation, vector4<float>& rotation)
{
    float blendWeight = getSwitchBlendWeight(clockSeconds);
    if (blendWeight < 1.0f && jointIndex < blendFromTranslation.size() && jointIndex < blendFromRotation.size())
    {
        translation = lerpTranslation(blendFromTranslation[jointIndex], translation, blendWeight);
        rotation = slerpRotation(blendFromRotation[jointIndex], rotation, blendWeight);
    }

    if (lastTranslation.size() != jointCount) lastTranslation.resize(jointCount);
    if (lastRotation.size() != jointCount) lastRotation.resize(jointCount);
    lastTranslation[jointIndex] = translation;
    lastRotation[jointIndex] = rotation;
}

// How many fixed physics steps spring bones should advance since they were last drawn
int ExternalMeshInstance::getSpringSteps()
{
    auto now = std::chrono::steady_clock::now();

    if (!springTimeStarted)
    {
        springTimeStarted = true;
        springLastTime = now;
        return 0;
    }

    // Long pauses (menus, loading) don't fast-forward the physics
    float elapsed = std::min(std::chrono::duration<float>(now - springLastTime).count(), 0.1f);
    springLastTime = now;

    springTimeAccumulator += elapsed;
    int steps = static_cast<int>(springTimeAccumulator / SPRING_BONE_STEP_SECONDS);
    springTimeAccumulator -= steps * SPRING_BONE_STEP_SECONDS;

    return steps;
}

// Places the body capsules in field space for this frame (body joints must already be posed)
void ExternalMeshInstance::updateSpringColliders(size_t jointCount)
{
    springColliders.clear();
    if (!hasSpringWorldMatrix || mesh->skins.empty()) return;

    const auto& skinJoints = mesh->skins[0].joints;
    for (size_t j = 0; j < jointCount; j++)
    {
        const Joint& joint = skinJoints[j];
        if (!joint.hasCollider) continue;

        float worldGlobalMatrix[16];
        bx::mtxMul(worldGlobalMatrix, joints[j].calculatedMatrix, springWorldMatrix.m[0]);

        // The joint's scale (model scale and the game's) applies to the radius too
        float scale = vectorLength({ worldGlobalMatrix[0], worldGlobalMatrix[1], worldGlobalMatrix[2] });

        springColliders.push_back({ transformPoint(joint.colliderStart, worldGlobalMatrix), transformPoint(joint.colliderEnd, worldGlobalMatrix), joint.colliderRadius * scale });
    }
}

// Swings a spring bone: simulates where its tail is in the field (inertia, gravity, pull back to the animated
// direction) and turns the bone's animated matrix (row-vector, model space) to point there.
void ExternalMeshInstance::simulateSpringBone(const Joint& joint, JointState& state, int steps, float modelScale)
{
    if (!hasSpringWorldMatrix) return;

    float* globalMatrix = state.calculatedMatrix;
    float worldMatrix[16], inverseWorldMatrix[16], worldGlobalMatrix[16];
    memcpy(worldMatrix, springWorldMatrix.m, sizeof(worldMatrix));
    bx::mtxInverse(inverseWorldMatrix, worldMatrix);
    bx::mtxMul(worldGlobalMatrix, globalMatrix, worldMatrix);

    vector3<float> head = { worldGlobalMatrix[12], worldGlobalMatrix[13], worldGlobalMatrix[14] };
    vector3<float> animatedTail = transformPoint(joint.springTail, worldGlobalMatrix);
    vector3<float> animatedDirection = { animatedTail.x - head.x, animatedTail.y - head.y, animatedTail.z - head.z };
    float length = vectorLength(animatedDirection);
    if (length <= 0.0f) return;

    // Start (or restart after a jump across the field) from the animated pose
    vector3<float> offset = { state.springTailPosition.x - head.x, state.springTailPosition.y - head.y, state.springTailPosition.z - head.z };
    if (!state.springStarted || vectorLength(offset) > 3.0f * length)
    {
        state.springTailPosition = state.springTailPrevious = animatedTail;
        state.springStarted = true;
    }

    // The game's field space has Y pointing down
    float gravity = joint.springGravity * SPRING_BONE_GRAVITY * modelScale * SPRING_BONE_STEP_SECONDS * SPRING_BONE_STEP_SECONDS;

    // The tail's own thickness, scaled like the bone (model scale and the game's)
    float tailRadius = joint.springRadius * vectorLength({ worldGlobalMatrix[0], worldGlobalMatrix[1], worldGlobalMatrix[2] });

    for (int step = 0; step < steps; step++)
    {
        vector3<float>& tail = state.springTailPosition;
        vector3<float>& previous = state.springTailPrevious;

        vector3<float> next = {
            tail.x + (tail.x - previous.x) * (1.0f - joint.springDrag) + (animatedTail.x - tail.x) * joint.springStiffness,
            tail.y + (tail.y - previous.y) * (1.0f - joint.springDrag) + (animatedTail.y - tail.y) * joint.springStiffness + gravity,
            tail.z + (tail.z - previous.z) * (1.0f - joint.springDrag) + (animatedTail.z - tail.z) * joint.springStiffness
        };

        // Keep the bone's length, push the tail out of the body, then keep the length again
        keepLength(next, head, length);
        if (pushOutOfColliders(next, springColliders, tailRadius)) keepLength(next, head, length);

        previous = tail;
        tail = next;
    }

    // Turn the bone from its animated direction to the simulated one (around its head)
    vector3<float> from = { animatedDirection.x / length, animatedDirection.y / length, animatedDirection.z / length };
    vector3<float> to = { state.springTailPosition.x - head.x, state.springTailPosition.y - head.y, state.springTailPosition.z - head.z };
    float toLength = vectorLength(to);
    if (toLength <= 0.0f) return;
    to = { to.x / toLength, to.y / toLength, to.z / toLength };

    vector3<float> axis = { from.y * to.z - from.z * to.y, from.z * to.x - from.x * to.z, from.x * to.y - from.y * to.x };
    float sinAngle = vectorLength(axis);
    float cosAngle = from.x * to.x + from.y * to.y + from.z * to.z;
    if (sinAngle < 1e-6f) return;
    axis = { axis.x / sinAngle, axis.y / sinAngle, axis.z / sinAngle };

    // Rotation matrix (column-vector form) around axis by the angle between from and to
    float c = cosAngle, s = sinAngle, t = 1.0f - cosAngle;
    float rotation[3][3] = {
        { t * axis.x * axis.x + c, t * axis.x * axis.y - s * axis.z, t * axis.x * axis.z + s * axis.y },
        { t * axis.x * axis.y + s * axis.z, t * axis.y * axis.y + c, t * axis.y * axis.z - s * axis.x },
        { t * axis.x * axis.z - s * axis.y, t * axis.y * axis.z + s * axis.x, t * axis.z * axis.z + c }
    };

    // Row-vector matrices keep each local axis in a row: turn the three axes, keep the head where it is
    for (int row = 0; row < 3; row++)
    {
        float x = worldGlobalMatrix[row * 4 + 0], y = worldGlobalMatrix[row * 4 + 1], z = worldGlobalMatrix[row * 4 + 2];
        worldGlobalMatrix[row * 4 + 0] = rotation[0][0] * x + rotation[0][1] * y + rotation[0][2] * z;
        worldGlobalMatrix[row * 4 + 1] = rotation[1][0] * x + rotation[1][1] * y + rotation[1][2] * z;
        worldGlobalMatrix[row * 4 + 2] = rotation[2][0] * x + rotation[2][1] * y + rotation[2][2] * z;
    }

    bx::mtxMul(globalMatrix, worldGlobalMatrix, inverseWorldMatrix);
}

// ---------------------------------------------------------------------------------------------------------
// Field model cache: each gltf is loaded once and shared by every character using it. Models no field uses any
// more stay loaded (up to a memory budget, least recently used dropped first) so later fields can reuse them.
// A changed .gltf, .bin or _config.toml is loaded again.

namespace
{
    struct FieldMeshCacheEntry
    {
        std::shared_ptr<ExternalMesh> mesh;
        std::vector<std::filesystem::file_time_type> fileTimes;
        size_t bytes = 0;
        uint64_t lastUsed = 0;
    };

    // Never destroyed: models must not be freed after the renderer shuts down at exit
    std::map<std::string, FieldMeshCacheEntry>& fieldMeshCache()
    {
        static auto* cache = new std::map<std::string, FieldMeshCacheEntry>();
        return *cache;
    }

    uint64_t fieldMeshCacheClock = 0;

    // The files a model is loaded from (missing ones count too, so adding a config file reloads the model)
    std::vector<std::filesystem::file_time_type> getModelFileTimes(const std::string& gltfPath)
    {
        std::string withoutExtension = gltfPath.substr(0, gltfPath.find_last_of("."));
        std::vector<std::filesystem::file_time_type> times;
        for (const auto& path : { gltfPath, withoutExtension + ".bin", withoutExtension + "_config.toml" })
        {
            std::error_code error;
            auto time = std::filesystem::last_write_time(path, error);
            times.push_back(error ? std::filesystem::file_time_type::min() : time);
        }
        return times;
    }

    void trimFieldMeshCache()
    {
        auto& cache = fieldMeshCache();

        while (true)
        {
            size_t unusedBytes = 0, unusedCount = 0;
            auto oldest = cache.end();
            for (auto it = cache.begin(); it != cache.end(); ++it)
            {
                if (it->second.mesh.use_count() > 1) continue; // Still drawn by some character
                unusedBytes += it->second.bytes;
                unusedCount++;
                if (oldest == cache.end() || it->second.lastUsed < oldest->second.lastUsed) oldest = it;
            }

            if (oldest == cache.end() || (unusedBytes <= FIELD_MESH_CACHE_BUDGET_BYTES && unusedCount <= FIELD_MESH_CACHE_MAX_UNUSED)) break;

            if (trace_all || trace_loaders) ffnx_trace("External mesh: dropped %s from the cache\n", oldest->first.c_str());
            cache.erase(oldest);
        }
    }
}

std::shared_ptr<ExternalMesh> acquireFieldExternalMesh(char* file_path, char* tex_path, const char** outSource)
{
    auto& cache = fieldMeshCache();
    std::string key = file_path;
    auto fileTimes = getModelFileTimes(key);

    auto it = cache.find(key);
    if (it != cache.end() && it->second.fileTimes == fileTimes)
    {
        *outSource = it->second.mesh.use_count() > 1 ? "shared" : "reused from the cache";
        it->second.lastUsed = ++fieldMeshCacheClock;
        return it->second.mesh;
    }

    // Characters still drawn with an older version keep it until they're freed
    auto mesh = std::shared_ptr<ExternalMesh>(new ExternalMesh(), [](ExternalMesh* oldMesh) { oldMesh->destroyFieldResources(); delete oldMesh; });
    if (!mesh->importExternalMeshGltfFile(file_path, tex_path, true)) return nullptr;

    *outSource = "loaded";
    cache[key] = { mesh, fileTimes, mesh->estimateMemory(), ++fieldMeshCacheClock };
    trimFieldMeshCache();
    return mesh;
}

// The battle character loaded last (its weapon file follows its parts)
static ExternalMeshInstance* lastBattleCharacter = nullptr;

// The gltf character drawn last for each battle actor (the base instance owning it, for releasing)
static std::map<const void*, std::pair<ExternalMeshInstance*, ExternalMeshInstance*>> battleActorCharacters;

// Characters freed while lighting is on may still have a draw queued for the end of the frame (deferred
// draws), so they are deleted once the queue has been drawn
static std::vector<ExternalMeshInstance*>& pendingFieldMeshReleases()
{
    static auto* pending = new std::vector<ExternalMeshInstance*>();
    return *pending;
}

void releaseFieldExternalMesh(ExternalMeshInstance* instance)
{
    if (instance == lastBattleCharacter) lastBattleCharacter = nullptr;
    for (auto it = battleActorCharacters.begin(); it != battleActorCharacters.end();)
    {
        if (it->second.second == instance) it = battleActorCharacters.erase(it);
        else ++it;
    }

    if (trace_all || trace_loaders) ffnx_trace("External mesh: character freed (%p)\n", instance);

    if (enable_lighting && !ff8)
    {
        pendingFieldMeshReleases().push_back(instance);
        return;
    }

    delete instance;
    trimFieldMeshCache();
}

void flushReleasedFieldExternalMeshes()
{
    auto& pending = pendingFieldMeshReleases();
    if (pending.empty()) return;

    for (auto* instance : pending) delete instance;
    pending.clear();
    trimFieldMeshCache();
}

// ---------------------------------------------------------------------------------------------------------
// Battle weapons

void setLastBattleCharacter(ExternalMeshInstance* instance)
{
    lastBattleCharacter = instance;
}

ExternalMeshInstance* getLastBattleCharacter()
{
    return lastBattleCharacter;
}

bool hasWeaponMesh(const ExternalMesh& mesh, const std::string& name)
{
    for (const auto& shape : mesh.shapes)
        if (shape.isWeapon && shape.name == name) return true;

    return false;
}

// ---------------------------------------------------------------------------------------------------------
// Posing

int ExternalMesh::jointForGameBone(uint32_t boneIndex) const
{
    return boneIndex < gameBoneJoints.size() ? gameBoneJoints[boneIndex] : -1;
}

void ExternalMeshInstance::updatePose(float scale)
{
    poseReady = true;
    if (mesh->skins.empty()) return;

    const auto& skin = mesh->skins[0];

    // Joints past the bone limit are ignored instead of overflowing the bone matrices
    size_t jointCount = std::min(skin.joints.size(), static_cast<size_t>(MAX_BONE_MATRICES));
    if (joints.size() < jointCount) return;

    // An animation the gltf doesn't have shows the skeleton's rest pose (the raw bind pose would include the
    // export's root node, e.g. KimeraCS's 180 degree turn, and look upside down)
    static const Animation restPose;
    auto found = mesh->animations.find(current_anim);
    const auto& anim = found != mesh->animations.end() ? found->second : restPose;
    AnimationPosition position = getAnimationPosition(anim, current_frame, current_frame_count, current_clock);
    int springSteps = mesh->hasSpringBones ? getSpringSteps() : 0;

    // Places a joint from its local transform and its parent (parents always come before their children)
    auto poseJoint = [&](size_t i)
    {
        const auto& joint = skin.joints[i];
        auto& state = joints[i];

        float parentMatrix[16];
        bx::mtxScale(parentMatrix, scale);

        if (joint.parentJointIndex != -1)
        {
            memcpy(parentMatrix, joints[joint.parentJointIndex].calculatedMatrix, sizeof(float) * 16);
        }

        bx::mtxMul(state.calculatedMatrix, state.localMatrix, parentMatrix);
    };

    // The animation's pose; spring bones (and everything below them) wait until the body is posed
    for (size_t i = 0; i < jointCount; ++i)
    {
        const auto& joint = skin.joints[i];
        static const KeyFrame noKeys;
        const auto& keyFrame = i < anim.keyFrames.size() ? anim.keyFrames[i] : noKeys;

        auto currentTranslation = sampleTranslation(keyFrame.translationTimes, keyFrame.translation, position, joint.translation);
        auto currentRotation = sampleRotation(keyFrame.rotationTimes, keyFrame.rotation, position, joint.rotation);
        blendJointPose(i, jointCount, current_clock, currentTranslation, currentRotation);

        float currentTranslationMatrix[16];
        bx::mtxTranslate(currentTranslationMatrix, currentTranslation.x, currentTranslation.y, currentTranslation.z);

        float currentRotationMatrix[16];
        bx::Quaternion rotationQuaternion = { currentRotation.x, currentRotation.y, currentRotation.z, -currentRotation.w };
        bx::mtxFromQuaternion(currentRotationMatrix, rotationQuaternion);

        bx::mtxMul(joints[i].localMatrix, currentRotationMatrix, currentTranslationMatrix);

        if (!joint.isSpring) poseJoint(i);
    }

    // Spring bones swing after the animation, colliding with this frame's body; children follow the swung bone
    if (mesh->hasSpringBones)
    {
        updateSpringColliders(jointCount);

        for (size_t i = 0; i < jointCount; ++i)
        {
            if (!skin.joints[i].isSpring) continue;

            poseJoint(i);
            simulateSpringBone(skin.joints[i], joints[i], springSteps, scale);
        }
    }
}

void setBattleActorCharacter(const void* actorKey, ExternalMeshInstance* character, ExternalMeshInstance* owner)
{
    battleActorCharacters[actorKey] = { character, owner };
}

ExternalMeshInstance* getBattleActorCharacter(const void* actorKey)
{
    auto it = battleActorCharacters.find(actorKey);
    return it != battleActorCharacters.end() ? it->second.first : nullptr;
}

