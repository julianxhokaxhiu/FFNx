/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2023 Julian Xhokaxhiu                                   //
//    Copyright (C) 2023 Tang-Tang Zhou                                     //
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
#include "camera.h"
#include "defs.h"

#include <unordered_set>
#include <unordered_map>
#include <utility>
#include <span>

#include "../../patch.h"
#include "../../log.h"
#include "../../globals.h"

#include <bx/math.h>

namespace ff7::battle
{
    Camera camera;

    const std::unordered_map<byte, int> numArgsPositionOpCode{{0xD5, 2}, {0xD6, 0}, {0xD7, 2}, {0xD8, 9}, {0xD9, 0}, {0xDA, 0}, {0xDB, 0}, {0xDC, 0}, {0xDD, 1}, {0xDE, 1}, {0xDF, 0}, {0xE0, 2}, {0xE1, 0}, {0xE2, 1}, {0xE3, 9}, {0xE4, 8}, {0xE5, 8}, {0xE6, 7}, {0xE7, 8}, {0xE9, 8}, {0xEB, 9}, {0xEF, 8}, {0xF0, 7}, {0xF1, 0}, {0xF2, 5}, {0xF3, 5}, {0xF4, -1}, {0xF5, 1}, {0xF7, 7}, {0xF8, 12}, {0xF9, 6}, {0xFE, 0}, {0xFF, -1}};
    const std::unordered_map<byte, int> numArgsOpCode{{0xD8, 9}, {0xD9, 0}, {0xDB, 0}, {0xDC, 0}, {0xDD, 1}, {0xDE, 1}, {0xDF, 0}, {0xE0, 2}, {0xE1, 0}, {0xE2, 1}, {0xE3, 9}, {0xE4, 8}, {0xE5, 8}, {0xE6, 7}, {0xE8, 8}, {0xEA, 8}, {0xEC, 9}, {0xF0, 8}, {0xF4, -1}, {0xF5, 1}, {0xF8, 7}, {0xF9, 7}, {0xFA, 6}, {0xFE, 0}, {0xFF, -1}};
    const std::unordered_set<byte> endingFocalOpCodes{0xF0, 0xF8, 0xF9, 0xFF};
    const std::unordered_set<byte> endingPositionOpCodes{0xEF, 0xF0, 0xF7, 0xFF};
    constexpr int CAMERA_ARRAY_SIZE = 16;
    std::array<bool, CAMERA_ARRAY_SIZE> isNewCameraFunction{};

    byte *getCameraScriptPointer(char variationIndex, short cameraScriptIdx, bool isCameraFocalPoint)
    {
        int internalOffset = isCameraFocalPoint ? 4 : 0;
        if (cameraScriptIdx == -1)
            return isCameraFocalPoint ? ff7_externals.battle_camera_focal_scripts_8FEE30 : ff7_externals.battle_camera_position_scripts_8FEE2C;
        else if (cameraScriptIdx == -2)
            return isCameraFocalPoint ? (byte *)ff7_externals.battle_camera_focal_scripts_901270[*ff7_externals.battle_camera_script_index] : (byte *)ff7_externals.battle_camera_position_scripts_9010D0[*ff7_externals.battle_camera_script_index];
        else if (cameraScriptIdx == -3)
        {
            int outerOffset = variationIndex * 4 + *(int *)(*ff7_externals.battle_camera_global_scripts_9A13BC + 0x8 + internalOffset) - *ff7_externals.battle_camera_script_offset;
            int finalOffset = *(int *)(*ff7_externals.battle_camera_global_scripts_9A13BC + outerOffset) - *ff7_externals.battle_camera_script_offset;
            return (byte *)(*ff7_externals.battle_camera_global_scripts_9A13BC + finalOffset);
        }
        int outerOffset = (3 * cameraScriptIdx + variationIndex) * 4 + *(int *)(*ff7_externals.battle_camera_global_scripts_9A13BC + internalOffset) - *ff7_externals.battle_camera_script_offset;
        int finalOffset = *(int *)(*ff7_externals.battle_camera_global_scripts_9A13BC + outerOffset) - *ff7_externals.battle_camera_script_offset;
        return (byte *)(*ff7_externals.battle_camera_global_scripts_9A13BC + finalOffset);
    }

    bool simulateCameraScript(byte *scriptPtr, short &currentPosition, short &framesToWait, const std::unordered_map<byte, int> &numArgsOpCode,
                            const std::unordered_set<byte> &endingOpCodes)
    {
        bool executedOpCodeF5 = false;
        bool isScriptActive = true;
        while (isScriptActive)
        {
            byte currentOpCode = scriptPtr[currentPosition++];

            switch (currentOpCode)
            {
            case 0xF4:
                if (framesToWait != 0)
                {
                    framesToWait--;
                    currentPosition--;
                    isScriptActive = false;
                }
                break;
            case 0xF5:
                if(scriptPtr[currentPosition] == 0xFF)
                {
                    framesToWait = -1;
                    currentPosition++;
                }
                else
                {
                    executedOpCodeF5 = true;
                    framesToWait = scriptPtr[currentPosition++] * battle_frame_multiplier;
                }
                break;
            case 0xFE:
                if (framesToWait == 0)
                {
                    currentOpCode = scriptPtr[currentPosition];

                    if (currentOpCode == 192)
                    {
                        framesToWait = 0;
                        currentPosition = 0;
                    }
                }
                break;
            default:
                if (numArgsOpCode.contains(currentOpCode))
                {
                    currentPosition += numArgsOpCode.at(currentOpCode);

                    if (endingOpCodes.contains(currentOpCode))
                        isScriptActive = false;
                }
                else
                {
                    isScriptActive = false;
                }
                break;
            }
        }

        return executedOpCodeF5;
    }

    int add_fn_to_camera_fn(uint32_t function)
    {
        auto element = std::find(ff7_externals.camera_fn_array.begin(), ff7_externals.camera_fn_array.end(), 0);
        if (element != ff7_externals.camera_fn_array.end())
        {
            int index = std::distance(ff7_externals.camera_fn_array.begin(), element);
            ff7_externals.camera_fn_array[index] = function;
            ff7_externals.camera_fn_data[index].field_0 = *ff7_externals.camera_fn_index;
            *ff7_externals.camera_fn_counter = *ff7_externals.camera_fn_counter + 1;

            isNewCameraFunction[index] = true;
            return index;
        }
        return 0xFFFF;
    }

    void execute_camera_functions()
    {
        uint16_t &fn_index = *ff7_externals.camera_fn_index;
        for (fn_index = 0; fn_index < CAMERA_ARRAY_SIZE; fn_index++)
        {
            if (ff7_externals.camera_fn_array[fn_index] != 0)
            {
                if (isNewCameraFunction[fn_index])
                {
                    if (ff7_externals.camera_fn_array[fn_index] == ff7_externals.battle_camera_position_sub_5C5B9C ||
                        ff7_externals.camera_fn_array[fn_index] == ff7_externals.battle_camera_focal_sub_5C5F5E ||
                        ff7_externals.camera_fn_array[fn_index] == ff7_externals.battle_camera_position_sub_5C557D ||
                        ff7_externals.camera_fn_array[fn_index] == ff7_externals.battle_camera_focal_sub_5C5714)
                    {
                        ff7_externals.camera_fn_data[fn_index].n_frames *= battle_frame_multiplier;
                    }
                    else if (ff7_externals.camera_fn_array[fn_index] == ff7_externals.battle_camera_position_sub_5C3D0D)
                    {
                        ff7_externals.camera_fn_data[fn_index].n_frames *= battle_frame_multiplier;
                        ff7_externals.camera_fn_data[fn_index].field_8 /= battle_frame_multiplier;
                        ff7_externals.camera_fn_data[fn_index].field_6 /= battle_frame_multiplier;
                        ff7_externals.camera_fn_data[fn_index].field_E /= battle_frame_multiplier;
                    }

                    isNewCameraFunction[fn_index] = false;
                }

                ((void (*)())ff7_externals.camera_fn_array[fn_index])();
                if (ff7_externals.camera_fn_data[fn_index].field_0 == (uint16_t)-1)
                {
                    ff7_externals.camera_fn_data[fn_index].field_0 = 0;
                    ff7_externals.camera_fn_data[fn_index].field_2 = 0;
                    ff7_externals.camera_fn_array[fn_index] = 0;
                    *ff7_externals.camera_fn_counter = *ff7_externals.camera_fn_counter - 1;
                }
            }
        }
        fn_index = 0;
    }

    void run_camera_focal_position_script(char variationIndex, DWORD param_2, short cameraScriptIdx)
    {
        auto cameraPosition = ff7_externals.battle_camera_focal_point;

        byte *scriptPtr = getCameraScriptPointer(variationIndex, cameraScriptIdx, true);
        short currentPosition = (cameraPosition[variationIndex].current_position == 255) ? 0 : cameraPosition[variationIndex].current_position;
        short framesToWait = (cameraPosition[variationIndex].current_position == 255) ? 0 : cameraPosition[variationIndex].frames_to_wait;

        bool executedOpCodeF5 = simulateCameraScript(scriptPtr, currentPosition, framesToWait, numArgsOpCode, endingFocalOpCodes);

        ((void (*)(char, DWORD, short))ff7_externals.set_camera_focal_position_scripts)(variationIndex, param_2, cameraScriptIdx);

        if (executedOpCodeF5)
            cameraPosition[variationIndex].frames_to_wait = framesToWait;
    }

    void run_camera_position_script(char variationIndex, DWORD param_2, short cameraScriptIdx)
    {
        auto cameraPosition = ff7_externals.battle_camera_position;

        byte *scriptPtr = getCameraScriptPointer(variationIndex, cameraScriptIdx, false);
        short currentPosition = (cameraPosition[variationIndex].current_position == 255) ? 0 : cameraPosition[variationIndex].current_position;
        short framesToWait = (cameraPosition[variationIndex].current_position == 255) ? 0 : cameraPosition[variationIndex].frames_to_wait;

        bool executedOpCodeF5 = simulateCameraScript(scriptPtr, currentPosition, framesToWait, numArgsPositionOpCode, endingPositionOpCodes);

        ((void (*)(char, DWORD, short))ff7_externals.set_camera_position_scripts)(variationIndex, param_2, cameraScriptIdx);

        if (executedOpCodeF5)
            cameraPosition[variationIndex].frames_to_wait = framesToWait;
    }

    void compute_interpolation_to_formation_camera()
    {
        ff7_externals.battle_camera_position[3].point = *ff7_externals.g_battle_camera_position;
        ff7_externals.battle_camera_focal_point[3].point = *ff7_externals.g_battle_camera_focal_point;
        int frame_steps = 2 * battle_frame_multiplier;
        vector3<short> delta_position, delta_focal_point;
        if (*ff7_externals.is_camera_moving_BFB2DC)
        {
            delta_position.x = (ff7_externals.formation_camera[*ff7_externals.curr_formation_camera_idx].position.x - ff7_externals.battle_camera_position[3].point.x) / frame_steps;
            if(delta_position.x == 0)
                delta_position.x = (ff7_externals.formation_camera[*ff7_externals.curr_formation_camera_idx].position.x - ff7_externals.battle_camera_position[3].point.x) % frame_steps;
            ff7_externals.battle_camera_position[3].point.x += delta_position.x;

            delta_position.y = (ff7_externals.formation_camera[*ff7_externals.curr_formation_camera_idx].position.y - ff7_externals.battle_camera_position[3].point.y) / frame_steps;
            if(delta_position.y == 0)
                delta_position.y = (ff7_externals.formation_camera[*ff7_externals.curr_formation_camera_idx].position.y - ff7_externals.battle_camera_position[3].point.y) % frame_steps;
            ff7_externals.battle_camera_position[3].point.y += delta_position.y;

            delta_position.z = (ff7_externals.formation_camera[*ff7_externals.curr_formation_camera_idx].position.z - ff7_externals.battle_camera_position[3].point.z) / frame_steps;
            if(delta_position.z == 0)
                delta_position.z = (ff7_externals.formation_camera[*ff7_externals.curr_formation_camera_idx].position.z - ff7_externals.battle_camera_position[3].point.z) % frame_steps;
            ff7_externals.battle_camera_position[3].point.z += delta_position.z;

            delta_focal_point.x = (ff7_externals.formation_camera[*ff7_externals.curr_formation_camera_idx].focal_point.x - ff7_externals.battle_camera_focal_point[3].point.x) / frame_steps;
            if(delta_focal_point.x == 0)
                delta_focal_point.x = (ff7_externals.formation_camera[*ff7_externals.curr_formation_camera_idx].focal_point.x - ff7_externals.battle_camera_focal_point[3].point.x) % frame_steps;
            ff7_externals.battle_camera_focal_point[3].point.x += delta_focal_point.x;

            delta_focal_point.y = (ff7_externals.formation_camera[*ff7_externals.curr_formation_camera_idx].focal_point.y - ff7_externals.battle_camera_focal_point[3].point.y) / frame_steps;
            if(delta_focal_point.y == 0)
                delta_focal_point.y = (ff7_externals.formation_camera[*ff7_externals.curr_formation_camera_idx].focal_point.y - ff7_externals.battle_camera_focal_point[3].point.y) % frame_steps;
            ff7_externals.battle_camera_focal_point[3].point.y += delta_focal_point.y;

            delta_focal_point.z = (ff7_externals.formation_camera[*ff7_externals.curr_formation_camera_idx].focal_point.z - ff7_externals.battle_camera_focal_point[3].point.z) / frame_steps;
            if(delta_focal_point.z == 0)
                delta_focal_point.z = (ff7_externals.formation_camera[*ff7_externals.curr_formation_camera_idx].focal_point.z - ff7_externals.battle_camera_focal_point[3].point.z) % frame_steps;
            ff7_externals.battle_camera_focal_point[3].point.z += delta_focal_point.z;

            if (!delta_position.x && !delta_position.y && !delta_position.z && !delta_focal_point.x && !delta_focal_point.y && !delta_focal_point.z)
                *ff7_externals.is_camera_moving_BFB2DC = 0;
        }
    }

    void update_battle_camera(short cameraScriptIndex)
    {
        vector3<short>* pGlobalCameraPos = ff7_externals.g_battle_camera_position;
        vector3<short>* pCameraPosition = &ff7_externals.battle_camera_position[*ff7_externals.g_variation_index].point;
        vector3<short>* pFormationCameraPos = &ff7_externals.formation_camera[*ff7_externals.curr_formation_camera_idx].position;

        ((void(*)(short))ff7_externals.update_battle_camera_sub_5C20CE)(cameraScriptIndex);

        byte battle_enter_frames_to_wait = *ff7_externals.battle_enter_frames_to_wait;
        if(cameraScriptIndex == -2 && battle_enter_frames_to_wait > 5) 
        {
            camera.reset();
            camera.setupInitialCamera();
        }

        if((std::abs(pGlobalCameraPos->x - pFormationCameraPos->x) <= 1 &&
            std::abs(pGlobalCameraPos->y - pFormationCameraPos->y) <= 1 &&
            std::abs(pGlobalCameraPos->z - pFormationCameraPos->z) <= 1))
        {
            camera.controlCamera(pGlobalCameraPos);
            *pFormationCameraPos = *pGlobalCameraPos;
            *pCameraPosition = *pGlobalCameraPos;
        }

        if(cameraScriptIndex == -3) camera.reset();
    }

    void camera_hook_init()
    {
        replace_function(ff7_externals.execute_camera_functions, execute_camera_functions);
        replace_function(ff7_externals.add_fn_to_camera_fn_array, add_fn_to_camera_fn);
        replace_call_function(ff7_externals.handle_camera_functions + 0x35, run_camera_focal_position_script);
        replace_call_function(ff7_externals.handle_camera_functions + 0x4B, run_camera_position_script);

        // Battle outro camera frame fix: patch DAT_009AE138 (frames to wait before closing battle mode)
        patch_multiply_code<DWORD>(ff7_externals.battle_sub_430DD0 + 0x3DE, battle_frame_multiplier);
        patch_multiply_code<DWORD>(ff7_externals.battle_sub_430DD0 + 0x361, battle_frame_multiplier);
        patch_multiply_code<DWORD>(ff7_externals.battle_sub_430DD0 + 0x326, battle_frame_multiplier);

        // Battle outro fading speed fix
        patch_multiply_code<byte>(ff7_externals.battle_sub_430DD0 + 0x60E, battle_frame_multiplier);

        // Battle intro camera frame fix: patch DAT_00BFD0F4 (frames to wait before atb starts)
        patch_multiply_code<byte>(ff7_externals.battle_sub_429AC0 + 0x152, battle_frame_multiplier);
        patch_multiply_code<byte>(ff7_externals.battle_sub_429D8A + 0x1D8, battle_frame_multiplier);

        // Move camera back to formation camera FPS fix
        replace_function(ff7_externals.compute_interpolation_to_formation_camera, compute_interpolation_to_formation_camera);
    }

    void Camera::setRotationSpeed(float rotX, float rotY, float rotZ)
    {
        rotationSpeed.x = rotX / static_cast<float>(battle_frame_multiplier);
        rotationSpeed.y = rotY / static_cast<float>(battle_frame_multiplier);
        rotationSpeed.z = rotZ / static_cast<float>(battle_frame_multiplier);
    }

    void Camera::setZoomSpeed(float speed)
    {
        zoomSpeed = speed / static_cast<float>(battle_frame_multiplier);
    }

    void Camera::reset()
    {
        rotationOffset.x = 0.0f;
        rotationOffset.y = 0.0f;
        zoomOffset = 0.0f;
    }

    void  Camera::setupInitialCamera()
    {
        vector3<short>* pFormationCameraPos = &ff7_externals.formation_camera[*ff7_externals.curr_formation_camera_idx].position;
        vector3<short>* pFormationCameraFocusPos = &ff7_externals.formation_camera[*ff7_externals.curr_formation_camera_idx].focal_point;

        initialCameraPos.x = pFormationCameraPos->x;
        initialCameraPos.y = pFormationCameraPos->y;
        initialCameraPos.z = pFormationCameraPos->z;

        initialCameraFocusPos.x = pFormationCameraFocusPos->x;
        initialCameraFocusPos.y = pFormationCameraFocusPos->y;
        initialCameraFocusPos.z = pFormationCameraFocusPos->z;
    }

    void Camera::controlCamera(vector3<short>* cameraPosition)
    {
        static WORD last_battle_id = 0;

        bx::Vec3 cameraPos = {
            static_cast<float>(initialCameraPos.x),
            static_cast<float>(initialCameraPos.y),
            static_cast<float>(initialCameraPos.z)};
        bx::Vec3 cameraFocusPos = {
            static_cast<float>(initialCameraFocusPos.x),
            static_cast<float>(initialCameraFocusPos.y),
            static_cast<float>(initialCameraFocusPos.z)};

        float dist = bx::distance(cameraFocusPos, cameraPos);
        float candidateDist = dist - (zoomOffset + zoomSpeed);
        if(candidateDist < maxZoomDist && candidateDist > minZoomDist)
        {
            zoomOffset = zoomOffset + zoomSpeed;
        }

        bx::Vec3 up = { 0, 1, 0 };
        bx::Vec3 forward =  { cameraFocusPos.x - cameraPos.x,
                            cameraFocusPos.y - cameraPos.y,
                            cameraFocusPos.z - cameraPos.z};
        forward  = bx::normalize(forward);
        bx::Vec3 right = bx::cross(forward, up);

        cameraPos = bx::add(cameraPos, bx::mul(forward, zoomOffset));

        float dot = bx::dot(bx::mul(forward, 1.0), up);
        float angle = 180.0f * std::acosf(dot) / M_PI;
        float candidateAngle = angle + rotationOffset.x + rotationSpeed.x;
        rotationOffset.x += rotationSpeed.x;
        if(candidateAngle < minVerticalAngle)
            rotationOffset.x += minVerticalAngle - candidateAngle;
        if(candidateAngle > maxVerticalAngle)
            rotationOffset.x -= candidateAngle - maxVerticalAngle;

        rotationOffset.y = std::remainder(rotationOffset.y + rotationSpeed.y, 360.0f);

        auto quaternionH = bx::fromAxisAngle(up, M_PI * rotationOffset.y / 180.0f);
        auto quaternionV = bx::fromAxisAngle(right, M_PI * rotationOffset.x / 180.0f);

        auto quaternion = bx::mul(quaternionV, quaternionH);
        quaternion = bx::normalize(quaternion);

        float focusToOriginMatrix[16];
        bx::mtxTranslate(focusToOriginMatrix, -cameraFocusPos.x, -cameraFocusPos.y, -cameraFocusPos.z);

        float originToFocusMatrix[16];
        bx::mtxTranslate(originToFocusMatrix, cameraFocusPos.x, cameraFocusPos.y, cameraFocusPos.z);

        float rotMat[16];
        bx::mtxFromQuaternion(rotMat, quaternion);

        float tmp[16];
        bx::mtxMul(tmp, focusToOriginMatrix, rotMat);

        float tmp2[16];
        bx::mtxMul(tmp2, tmp, originToFocusMatrix);

        // Get new camera pos
        float newCameraPos[4] =  { 0.0f, 0.0f, 0.0f , 1.0f};
        float oldCameraPos[4] =  { cameraPos.x, cameraPos.y, cameraPos.z, 1.0f};
        bx::vec4MulMtx(newCameraPos, oldCameraPos, tmp2);

        cameraPosition->x = newCameraPos[0];
        cameraPosition->y = newCameraPos[1];
        cameraPosition->z = newCameraPos[2];
    }
}