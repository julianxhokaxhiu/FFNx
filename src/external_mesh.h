/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2026 Julian Xhokaxhiu                                   //
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

#include <chrono>
#include <vector>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <toml++/toml.h>

#include "renderer.h"
#include "matrix.h"

struct Material
{
    std::vector<bgfx::TextureHandle> baseColorTexHandles;
    std::vector<bgfx::TextureHandle> normalTexHandles;
    std::vector<bgfx::TextureHandle> pbrTexHandles;
    int texIndex = 0;
    int frameInterval = 0;
};

// The glTF material's alpha mode: opaque, cut out below a cutoff (mask), or alpha blended
enum class ShapeAlphaMode
{
    OPAQUE_MODE,
    MASK_MODE,
    BLEND_MODE
};

struct Shape
{
    std::vector<nvertex> vertices;
    std::vector<vector3<float>> normals;
    std::vector<vector4<float>> joints;
    std::vector<vector4<float>> weights;
    std::vector<uint32_t> indices;
    vector3<float> min;
    vector3<float> max;
    Material* pMaterial = nullptr;
    bool isDoubleSided = false;
    ShapeAlphaMode alphaMode = ShapeAlphaMode::OPAQUE_MODE;
    float alphaCutoff = 0.5f; // MASK: texels below this are cut out
    std::string name; // The glTF mesh's name (battle weapons are named after the game's weapon file, e.g. RTCK)
    bool isWeapon = false; // Skinned only to a joint named "weapon": drawn only while it is the equipped weapon
};

struct Joint
{
    vector4<float> rotation;
    vector3<float> translation;
    std::string name;
    std::string extras; // The node's glTF "extras" (custom properties) as JSON, or empty
    int parentJointIndex = -1;
    float inverseBindPoseMatrix[16];

    // Spring bones: joints named with "spring" (and the joints below them) swing on their own after animation
    bool isSpring = false;
    vector3<float> springTail = {}; // Local point the bone points at (its first child, or its own length again)

    // Spring settings, later sources winning: the driver's defaults (or the model config's [spring_bones]) at the
    // start of a chain, else the parent spring bone's; the bone's custom properties (spring_stiffness,
    // spring_drag, spring_gravity, spring_radius); the model config's [spring_bones.<bone name>]
    float springStiffness = 0.0f;
    float springDrag = 0.0f;
    float springGravity = 0.0f;
    float springRadius = 0.0f;

    // Body joints (not springs) carry a capsule spring bones collide with, in the joint's local space, fitted
    // to the vertices skinned to it
    bool hasCollider = false;
    vector3<float> colliderStart = {};
    vector3<float> colliderEnd = {};
    float colliderRadius = 0.0f;
};

// A joint's state for one character drawing a model
struct JointState
{
    float localMatrix[16]; // This frame's animated local transform
    float calculatedMatrix[16]; // Model-space transform (after spring bones)
    vector3<float> springTailPosition = {}; // Simulated spring tail in field space
    vector3<float> springTailPrevious = {};
    bool springStarted = false;
};

struct Skin
{
    std::vector<Joint> joints;
};

struct KeyFrame
{
    int targetJointIndex = 0;
    std::vector<vector4<float>> rotation;
    std::vector<vector3<float>> translation;
    std::vector<float> rotationTimes;
    std::vector<float> translationTimes;
};

struct Animation
{
    std::vector<KeyFrame> keyFrames;

    // Keys of the skeleton's root node (the non-joint parent of the top joint), if the file animates it
    std::vector<vector3<float>> rootTranslation;
    std::vector<vector4<float>> rootRotation;
    std::vector<float> rootTranslationTimes;
    std::vector<float> rootRotationTimes;

    size_t keyCount = 0; // Most keys of any channel
    float startTime = 0.0f;
    float endTime = 0.0f;
};

// Where to sample an animation for one of the game's frames: a key index when the gltf has one key per game
// frame, otherwise a time on the gltf's own timeline (stretched over the game animation's length, or looping on
// its own clock when the game holds a single frame)
struct AnimationPosition
{
    bool useKeyIndex = true;
    int keyIndex = 0;
    float time = 0.0f;
};

AnimationPosition getAnimationPosition(const Animation& anim, int frame, int frameCount, float clockSeconds);
vector3<float> sampleTranslation(const std::vector<float>& times, const std::vector<vector3<float>>& values, const AnimationPosition& position, const vector3<float>& fallback);
vector4<float> sampleRotation(const std::vector<float>& times, const std::vector<vector4<float>>& values, const AnimationPosition& position, const vector4<float>& fallback);
vector3<float> lerpTranslation(const vector3<float>& a, const vector3<float>& b, float blend);
vector4<float> slerpRotation(const vector4<float>& a, vector4<float> b, float blend);

// Spring bone defaults (per 1/60 second step): how strongly a bone returns to its animated direction (0 loose,
// 1 rigid), how much of its swing it loses (0 keeps swinging, 1 no swing), and how much gravity pulls it (1 =
// SPRING_BONE_GRAVITY mesh units per second squared, scaled by the field model scale). The tail's collision
// radius is in mesh units.
constexpr float SPRING_BONE_STIFFNESS = 0.06f;
constexpr float SPRING_BONE_DRAG = 0.3f;
constexpr float SPRING_BONE_GRAVITY_FACTOR = 1.0f;
constexpr float SPRING_BONE_RADIUS = 0.0f;
constexpr float SPRING_BONE_GRAVITY = 200.0f;
constexpr float SPRING_BONE_STEP_SECONDS = 1.0f / 60.0f;
// Body capsules use this share of the typical distance of their vertices from the bone, so they stay inside
// the surface (hair resting on the scalp isn't pushed off)
constexpr float SPRING_BONE_COLLIDER_RADIUS_SCALE = 0.8f;

struct SpringCollider
{
    vector3<float> start;
    vector3<float> end;
    float radius;
};

// Animation switches blend from the previous pose over this long instead of snapping
constexpr float EXTERNAL_MESH_SWITCH_BLEND_SECONDS = 0.15f;
float getSwitchBlendWeight(float clockSeconds);
void buildRootMatrix(const vector3<float>& translation, const vector4<float>& rotation, float translationScale, struct matrix* outMatrix);

// Field models no field uses any more stay loaded for later fields, up to this much memory or this many models
constexpr size_t FIELD_MESH_CACHE_BUDGET_BYTES = 256 * 1024 * 1024;
constexpr size_t FIELD_MESH_CACHE_MAX_UNUSED = 32;

struct cgltf_data;

class ExternalMesh
{
public:
    bool importExternalMeshGltfFile(char* file_path, char* tex_path, bool isZUp = false);
    uint32_t fillExternalMeshVertexBuffer(struct nvertex* inVertex, struct vector3<float>* normals, struct vector4<float>* joints, struct vector4<float>* weights, uint32_t inCount);
    uint32_t fillExternalMeshIndexBuffer(uint32_t* inIndex, uint32_t inCount);
    void updateExternalMeshBuffers();
    void bindField3dVertexBuffer(uint32_t offset, uint32_t inCount);
    void bindField3dIndexBuffer(uint32_t offset, uint32_t inCount);
    void clearExternalMesh3dBuffers();
    void unloadExternalMesh();
    void destroyFieldResources();
    size_t estimateMemory() const;
    bool getRootMotionSample(const std::string& animName, const AnimationPosition& position, vector3<float>& translation, vector4<float>& rotation) const;
    // The animation for a game animation of this many frames when the game gives no other way to tell (a summon
    // model that isn't a battle actor): the one with one key per frame, or the only one; empty if neither
    std::string animationForFrameCount(int frameCount) const;

    std::vector<Shape> shapes;
	std::map<std::string, Material> materials;
    std::vector<Skin> skins;
    std::map<std::string, Animation> animations;

    // Animations already reported as using the gltf root motion (trace_loaders)
    std::set<std::string> rootMotionChecked;

    bool hasSpringBones = false;

    // The gltf joint standing for the game's bone (battle exports name joints bone_00, bone_01, ...), or -1
    int jointForGameBone(uint32_t boneIndex) const;
    std::vector<int> gameBoneJoints; // Game bone index -> joint index (joints named bone_NN), -1 when none
private:
    void setupSpringColliders();

    // The steps of importExternalMeshGltfFile, in order
    void loadTextures(cgltf_data* data, char* tex_path);
    std::vector<std::vector<int>> loadSkins(cgltf_data* data, const char* file_path);
    void loadMeshes(cgltf_data* data, const std::vector<std::vector<int>>& skinJointRemaps, bool isZUp);
    void findGameBoneJoints();
    void findWeaponShapes();
    void setupSpringBones();
    void loadAnimations(cgltf_data* data);
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

// One character drawn with a (possibly shared) field model: its animation, blending and spring bone state
class ExternalMeshInstance
{
public:
    explicit ExternalMeshInstance(std::shared_ptr<ExternalMesh> sharedMesh);

    float getAnimationClock(const std::string& animName);
    bool getRootMotionMatrix(const std::string& animName, int frame, int frameCount, float clockSeconds, float translationScale, struct matrix* outMatrix);
    void blendJointPose(size_t jointIndex, size_t jointCount, float clockSeconds, vector3<float>& translation, vector4<float>& rotation);
    int getSpringSteps();
    void updateSpringColliders(size_t jointCount);
    void simulateSpringBone(const Joint& joint, JointState& state, int steps, float modelScale);

    std::shared_ptr<ExternalMesh> mesh;
    std::vector<JointState> joints;

    // Battle: the weapon mesh to draw (the game's weapon file, e.g. RTCK); a hidden instance stands in for the
    // game's own weapon model so it isn't drawn as well
    std::string equippedWeapon;
    bool hidden = false;

    // The game's opacity for this character (battle fade in, death fade out), 1 = opaque
    float fadeAlpha = 1.0f;

    // The color the game adds to this character (battle: red while dying, status tints), 0..1 per channel;
    // drawn as an additive pass over the model
    vector3<float> addedColor = { 0.0f, 0.0f, 0.0f };

    // Characters sharing one loaded model (identical enemies share the game's model) each get their own state,
    // keyed by their own placement data; the game draws them one after another, so the active one is drawn
    ExternalMeshInstance* variantFor(const void* key);
    ExternalMeshInstance* activeVariant = nullptr;

    // Poses the skeleton for the current animation, frame and clock (animation, switch blend, spring bones):
    // each joint's calculatedMatrix, in model space with the mesh scale. Done when the game draws the model, so
    // the game can use the gltf's bones; poseReady tells the final draw it doesn't need to do it again.
    void updatePose(float scale);
    bool poseReady = false;

    // Battle: this frame's world positions of the game's own bones and of the gltf joints standing for them
    // (by game bone index), so the game's original weapon can follow the gltf's hand
    std::vector<vector3<float>> gameBonePositions;
    std::vector<vector3<float>> gltfBonePositions;
    std::vector<bool> hasGltfBonePosition;

    std::string current_anim;
    int current_frame = 0;
    int current_frame_count = 0; // Frame count of the game's current animation (.a)
    float current_clock = 0.0f; // Seconds since the game switched to the current animation

    // Spring bones: where the character was placed in the field when last drawn (row-vector, game units)
    struct matrix springWorldMatrix = {};
    bool hasSpringWorldMatrix = false;

private:
    // Animation the own clock is running for, and when the game switched to it
    std::string clockAnim;
    std::chrono::steady_clock::time_point clockStart;

    // Pose shown last (joint-local, plus the gltf root), and the one to blend from after a switch
    std::vector<vector3<float>> lastTranslation, blendFromTranslation;
    std::vector<vector4<float>> lastRotation, blendFromRotation;
    vector3<float> lastRootTranslation = {}, blendFromRootTranslation = {};
    vector4<float> lastRootRotation = {}, blendFromRootRotation = {};
    bool lastHasRoot = false, blendFromHasRoot = false;

    std::chrono::steady_clock::time_point springLastTime;
    bool springTimeStarted = false;
    float springTimeAccumulator = 0.0f;
    std::vector<SpringCollider> springColliders; // Body capsules in field space for the current frame

    std::map<const void*, std::unique_ptr<ExternalMeshInstance>> variants;
};

// Field models: the shared model for a gltf (loaded, or reused from the cache; outSource says which), or nullptr
// when it fails to load. Characters are freed with releaseFieldExternalMesh, which also trims the cache.
std::shared_ptr<ExternalMesh> acquireFieldExternalMesh(char* file_path, char* tex_path, const char** outSource);
void releaseFieldExternalMesh(ExternalMeshInstance* instance);

// Battle weapons: the battle character loaded last (its weapon file is loaded right after its parts), and whether
// its gltf has a weapon mesh with this name
void setLastBattleCharacter(ExternalMeshInstance* instance);

// Battle: the gltf character drawn last for a battle actor (its original weapon is drawn right after it), and
// the instance owning it (freeing that forgets the actor)
void setBattleActorCharacter(const void* actorKey, ExternalMeshInstance* character, ExternalMeshInstance* owner);
ExternalMeshInstance* getBattleActorCharacter(const void* actorKey);
ExternalMeshInstance* getLastBattleCharacter();
bool hasWeaponMesh(const ExternalMesh& mesh, const std::string& name);
void flushReleasedFieldExternalMeshes(); // After the deferred draws, which may still use freed characters
