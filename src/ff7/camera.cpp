/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2021 Julian Xhokaxhiu                                   //
//    Copyright (C) 2021 Tang-Tang Zhou                                     //
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

#include <unordered_set>
#include <unordered_map>
#include <utility>

#include "../ff7.h"
#include "../log.h"

enum class PatchType
{
    NONE = 0,
    FIELD_1,
    FIELD_3,
    FIELD_4
};

PatchType patchTypeCameraFunction[16];
const int frameMultiplier = ((ff7_fps_limiter == FF7_LIMITER_30FPS) ? 2 : 4);

void patchOpCodeF5(battle_camera_position* camera_position, char variationIndex)
{
    camera_position[variationIndex].frames_to_wait = (camera_position[variationIndex].frames_to_wait + 1) * frameMultiplier;
    camera_position[variationIndex].frames_to_wait--; // After 0xF5, there should "always" be an opcode 0xF4 
}

byte *getCameraScriptPointer(char variationIndex, short cameraScriptIdx, bool isSub5C3FD5)
{
    int internalOffset = isSub5C3FD5 ? 4 : 0;
    if (cameraScriptIdx == -1)
        return isSub5C3FD5 ? ff7_externals.battle_camera_scripts_8FEE30 : ff7_externals.battle_camera_scripts_8FEE2C;
    else if (cameraScriptIdx == -2)
        return isSub5C3FD5 ? (byte *)ff7_externals.battle_camera_scripts_901270[*ff7_externals.battle_camera_script_index] : (byte *)ff7_externals.battle_camera_scripts_9010D0[*ff7_externals.battle_camera_script_index];
    else if (cameraScriptIdx == -3)
    {
        int outerOffset = variationIndex * 4 + *(int *)(*ff7_externals.battle_camera_scripts_9A13BC + 0x8 + internalOffset) - *ff7_externals.battle_camera_script_offset;
        int finalOffset = *(int *)(*ff7_externals.battle_camera_scripts_9A13BC + outerOffset) - *ff7_externals.battle_camera_script_offset;
        return (byte *)(*ff7_externals.battle_camera_scripts_9A13BC + finalOffset);
    }
    int outerOffset = (3 * cameraScriptIdx + variationIndex) * 4 + *(int *)(*ff7_externals.battle_camera_scripts_9A13BC + internalOffset) - *ff7_externals.battle_camera_script_offset;
    int finalOffset = *(int *)(*ff7_externals.battle_camera_scripts_9A13BC + outerOffset) - *ff7_externals.battle_camera_script_offset;
    return (byte *)(*ff7_externals.battle_camera_scripts_9A13BC + finalOffset);
}

bool simulateCameraScript(byte *scriptPtr, uint16_t &currentPosition, uint16_t &framesToWait, std::unordered_map<byte, int> &numArgsOpCode, std::unordered_set<byte> &deactiveOpCodes)
{
    if (trace_all || trace_battle_camera)
        ffnx_trace("%s - START LIST OF CAMERA SCRIPT OPCODE AND ARGS\n", __func__);

    bool executedOpCodeF5 = false;
    bool isScriptActive = true;
    while (isScriptActive)
    {
        byte currentOpCode = scriptPtr[currentPosition++];

        if (trace_all || trace_battle_camera)
            ffnx_trace("opcode: 0x%0x\n", currentOpCode);

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
            executedOpCodeF5 = true;
            framesToWait = scriptPtr[currentPosition++] * frameMultiplier;
            break;
        case 0xFE:
            if (framesToWait == 0)
            {
                currentOpCode = scriptPtr[currentPosition];

                if (trace_all || trace_battle_camera)
                    ffnx_trace("0xFE case: opcode 0x%0x\n", currentOpCode);

                if (currentOpCode == 192)
                {
                    framesToWait = 0;
                    currentPosition = 0;
                }
            }
            break;
        default:
            if (numArgsOpCode.find(currentOpCode) != numArgsOpCode.end())
            {
                currentPosition += numArgsOpCode[currentOpCode];

                if (deactiveOpCodes.find(currentOpCode) != deactiveOpCodes.end())
                    isScriptActive = false;
            }
            else
            {
                if (trace_all || trace_battle_camera)
                    ffnx_error("%s - Strange OpCode 0x%0x in camera script\n", __func__, currentOpCode);
                isScriptActive = false;
            }
            break;
        }
    }
    if (trace_all || trace_battle_camera)
        ffnx_trace("%s - END LIST OF CAMERA SCRIPT OPCODE AND ARGS\n", __func__);

    return executedOpCodeF5;
}

// For the other opcodes
int ff7_add_fn_to_camera_fn_for_field_1(uint32_t function)
{
    int fnIdx = ((int (*)(uint32_t))ff7_externals.add_fn_to_camera_fn_array)(function);
    patchTypeCameraFunction[fnIdx] = PatchType::FIELD_1;
    return fnIdx;
}

// For 0xEB opcode of battle_camera_sub_5C23D1
int ff7_add_fn_to_camera_fn_for_field_3(uint32_t function)
{
    int fnIdx = ((int (*)(uint32_t))ff7_externals.add_fn_to_camera_fn_array)(function);
    patchTypeCameraFunction[fnIdx] = PatchType::FIELD_3;
    return fnIdx;
}

// For 0xE7 and 0xE9 opcode of battle_camera_sub_5C23D1
int ff7_add_fn_to_camera_fn_for_field_4(uint32_t function)
{
    int fnIdx = ((int (*)(uint32_t))ff7_externals.add_fn_to_camera_fn_array)(function);
    patchTypeCameraFunction[fnIdx] = PatchType::FIELD_4;
    return fnIdx;
}

// For battle_camera_sub_5C52F8 and battle_camera_sub_5C3E6F
int ff7_add_fn_to_camera_fn_special_multiply(uint32_t function)
{   
    DWORD *battle_data_C05FF4_pointer = (DWORD *)*ff7_externals.battle_data_C05FF4;
    *battle_data_C05FF4_pointer *= frameMultiplier;
    return ((int (*)(uint32_t))ff7_externals.add_fn_to_camera_fn_array)(function);
}

void ff7_execute_camera_functions()
{   
    for (int index = 0; index < 16; index++)
    {
        if (ff7_externals.camera_fn_array[index] != 0)
        {
            if(patchTypeCameraFunction[index] != PatchType::NONE)
                if (trace_all || trace_battle_camera)
                    ffnx_trace("%s - function started 0x%x\n", __func__, ff7_externals.camera_fn_array[index]);

            if(patchTypeCameraFunction[index] == PatchType::FIELD_1)
                ff7_externals.battle_camera_data[index].field_1 *= frameMultiplier;
            else if (patchTypeCameraFunction[index] == PatchType::FIELD_3)
                ff7_externals.battle_camera_data[index].field_3 *= frameMultiplier;
            else if (patchTypeCameraFunction[index] == PatchType::FIELD_4)
                ff7_externals.battle_camera_data[index].field_4 *= frameMultiplier;
            
            patchTypeCameraFunction[index] = PatchType::NONE;
        }
    }
    ((void (*)())ff7_externals.execute_camera_functions)();
}

void ff7_battle_camera_sub_5C3FD5(char variationIndex, DWORD param_2, short cameraScriptIdx)
{
    if (trace_all || trace_battle_camera)
        ffnx_trace("%s - Parameters: %d, %d, %d\n", __func__, variationIndex, param_2, cameraScriptIdx);

    battle_camera_position *cameraPosition = ff7_externals.battle_camera_position_BE1130;

    byte *scriptPtr = getCameraScriptPointer(variationIndex, cameraScriptIdx, true);
    uint16_t currentPosition = (cameraPosition[variationIndex].current_position == 255) ? 0 : cameraPosition[variationIndex].current_position;
    uint16_t framesToWait = (cameraPosition[variationIndex].current_position == 255) ? 0 : cameraPosition[variationIndex].frames_to_wait;

    std::unordered_map<byte, int> numArgsOpCode{{0xD8, 9}, {0xD9, 0}, {0xDB, 0}, {0xDC, 0}, {0xDD, 1}, {0xDE, 1}, {0xDF, 0}, {0xE0, 2}, {0xE1, 0}, 
                                                {0xE2, 1}, {0xE3, 9}, {0xE4, 8}, {0xE5, 8}, {0xE6, 7}, {0xE8, 8}, {0xEA, 8}, {0xEC, 9}, {0xF0, 8}, 
                                                {0xF4, -1}, {0xF5, 1}, {0xF8, 7}, {0xF9, 7}, {0xFA, 6}, {0xFE, 0}, {0xFF, -1}};
    std::unordered_set<byte> deactiveOpCodes{0xF0, 0xF8, 0xF9, 0xFF};

    bool executedOpCodeF5 = simulateCameraScript(scriptPtr, currentPosition, framesToWait, numArgsOpCode, deactiveOpCodes);

    ((void (*)(char, DWORD, short))ff7_externals.battle_camera_sub_5C3FD5)(variationIndex, param_2, cameraScriptIdx);

    if(executedOpCodeF5)
        patchOpCodeF5(cameraPosition, variationIndex);

    if (currentPosition != cameraPosition[variationIndex].current_position)
        ffnx_error("%s - Camera script pointer simulation wrong! Battle camera final position does not match (simulation: %d != real: %d)\n", __func__,
                   currentPosition, cameraPosition[variationIndex].current_position);

    if (framesToWait != cameraPosition[variationIndex].frames_to_wait)
        ffnx_error("%s - Camera script pointer simulation wrong! Battle camera final frames to wait does not match (simulation: %d != real: %d)\n", __func__,
                   framesToWait, cameraPosition[variationIndex].frames_to_wait);
}

void ff7_battle_camera_sub_5C23D1(char variationIndex, DWORD param_2, short cameraScriptIdx)
{
    if (trace_all || trace_battle_camera)
        ffnx_trace("%s - Parameters: %d, %d, %d\n", __func__, variationIndex, param_2, cameraScriptIdx);

    battle_camera_position *cameraPosition = ff7_externals.battle_camera_position_BE10F0;

    byte *scriptPtr = getCameraScriptPointer(variationIndex, cameraScriptIdx, false);
    uint16_t currentPosition = (cameraPosition[variationIndex].current_position == 255) ? 0 : cameraPosition[variationIndex].current_position;
    uint16_t framesToWait = (cameraPosition[variationIndex].current_position == 255) ? 0 : cameraPosition[variationIndex].frames_to_wait;

    std::unordered_map<byte, int> numArgsOpCode{{0xD5, 2}, {0xD6, 0}, {0xD7, 2}, {0xD8, 9}, {0xD9, 0}, {0xDA, 0}, {0xDB, 0}, {0xDC, 0}, {0xDD, 1},
                                                {0xDE, 1}, {0xDF, 0}, {0xE0, 2}, {0xE1, 0}, {0xE2, 1}, {0xE3, 9}, {0xE4, 8}, {0xE5, 8}, {0xE6, 7}, 
                                                {0xE7, 8}, {0xE9, 8}, {0xEB, 9}, {0xEF, 8}, {0xF0, 7}, {0xF1, 0}, {0xF2, 5}, {0xF3, 5}, {0xF4, -1},
                                                {0xF5, 1}, {0xF7, 7}, {0xF8, 12}, {0xF9, 6}, {0xFE, 0}, {0xFF, -1}};
    std::unordered_set<byte> deactiveOpCodes{0xEF, 0xF0, 0xF7, 0xFF};

    bool executedOpCodeF5 = simulateCameraScript(scriptPtr, currentPosition, framesToWait, numArgsOpCode, deactiveOpCodes);

    ((void (*)(char, DWORD, short))ff7_externals.battle_camera_sub_5C23D1)(variationIndex, param_2, cameraScriptIdx);

    if(executedOpCodeF5)
        patchOpCodeF5(cameraPosition, variationIndex);

    if (currentPosition != cameraPosition[variationIndex].current_position)
        ffnx_error("%s - Camera script pointer simulation wrong! Battle camera final position does not match (simulation: %d != real: %d)\n", __func__,
                   currentPosition, cameraPosition[variationIndex].current_position);

    if (framesToWait != cameraPosition[variationIndex].frames_to_wait)
        ffnx_error("%s - Camera script pointer simulation wrong! Battle camera final frames to wait does not match (simulation: %d != real: %d)\n", __func__,
                   framesToWait, cameraPosition[variationIndex].frames_to_wait);
}
