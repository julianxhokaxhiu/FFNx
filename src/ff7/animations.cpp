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

#include "../ff7.h"
#include "../log.h"
#include "../patch.h"
#include "../globals.h"
#include "defs.h"

byte y_pos_offset_display_damage_30[] = {0, 1, 2, 3, 4, 5, 6, 6, 7, 7, 8, 8, 8, 8, 7, 7, 6, 6, 5, 4, 3, 2};
byte y_pos_offset_display_damage_60[] = {0, 1, 2, 3, 4, 5, 6, 6, 7, 7, 7, 8, 8, 8, 8, 8, 7, 7, 7, 6, 6, 5, 4, 3, 2, 1, 0, 0, 1, 2, 3, 4, 4, 4, 3, 2, 1, 0, 0, 1, 1, 0, 0, 0};
WORD ff7_odin_steel_frames_AEEC14;

const std::unordered_map<byte, int> numArgsOpCode = {
    {0x8E, 0},
    {0x8F, 0},
    {0x90, 3},
    {0x91, 1}, // Function added
    {0x92, 0},
    {0x93, 0}, // Function added
    {0x94, 5}, // Function added
    {0x95, 0},
    {0x96, 2}, // effect60 fn added (barret gun)
    {0x97, 2}, // Run enemy death animations
    {0x98, 1}, // Display action text
    {0x99, 6},
    {0x9A, 4},
    {0x9B, 0},
    {0x9C, 0},
    {0x9D, 1}, // Dispatches Tifa limit breaks
    {0x9E, 0xFF},
    {0x9F, 0},
    {0xA0, 1},
    {0xA1, 2},
    {0xA2, 1},
    {0xA3, 1},
    {0xA4, 0}, // Spell aura related
    {0xA5, 0}, // Spell aura related
    {0xA6, 0},
    {0xA7, 1},
    {0xA8, 2}, // Move actor to resting position
    {0xA9, 2},
    {0xAA, 0},
    {0xAB, 4},
    {0xAC, 1}, // Vincent related
    {0xAD, 5}, // Barret related machine gun effect
    {0xAE, 0}, // Resting position related
    {0xAF, 1}, // Resting position reset
    {0xB0, 0}, // Resting position related
    {0xB1, 0}, // Resting position related
    {0xB2, 0}, // nop
    {0xB3, 0xFF},
    {0xB4, 0}, // Y rotation
    {0xB5, 11},
    {0xB6, 1}, // running animation related
    {0xB7, 0}, // death effects
    {0xB8, 0},
    {0xB9, 1}, // setup animation camera data
    {0xBA, 2}, // resting Y rotation
    {0xBC, 1}, // Idle camera index
    {0xBD, 4}, // rotate to target animation
    {0xBE, 1}, // not Tifa stuff
    {0xBF, 2},
    {0xC1, 0xFF},
    {0xC2, 1}, // display damage
    {0xC3, 0}, // some effects
    {0xC4, 3}, // resting Y rotation (inverting direction)
    {0xC5, 0}, // set frames to wait to 0xBFD0F0
    {0xC6, 1}, // set 0xBFD0F0 frames to wait
    {0xC7, 3}, // enemy animation thing
    {0xC8, 5}, // effects thing
    {0xC9, 0}, // nop
    {0xCA, 0xFF},
    {0xCB, 8},
    {0xCC, 1}, // move effects thing
    {0xCE, 1},
    {0xCF, 8}, // 3d move effects
    {0xD0, 3}, // move effects
    {0xD1, 5}, // move effects
    {0xD2, 0},
    {0xD3, 0},
    {0xD4, 3}, // move effects
    {0xD5, 8}, // move effects
    {0xD6, 1}, // effects thing
    {0xD7, 2}, // effects thing
    {0xD8, 3}, // effects thing
    {0xDA, 1},
    {0xDB, 4}, // effect machine gun
    {0xDC, 3}, // rotation stuff
    {0xDD, 2}, // machine gun effects
    {0xDE, 2}, // machine gun stuff
    {0xDF, 0}, // resting Y rotation
    {0xE0, 0}, // spell aura related
    {0xE1, 0}, // appear model
    {0xE2, 0}, // vanish model
    {0xE3, 0}, // position actor
    {0xE4, 0}, // resting stuff
    {0xE5, 0}, // rotation stuff
    {0xE6, 0}, // spell aura stuff
    {0xE7, 1},
    {0xE8, 0},
    {0xE9, 3}, // move effects
    {0xEA, 0}, // display action string effect
    {0xEB, 0xFF},
    {0xEC, 0xFF},
    {0xED, 0}, // resting stuff
    {0xEE, 0xFF},
    {0xF0, 0}, // foot dust effect
    {0xF1, -1},
    {0xF2, 0}, // nop
    {0xF3, 0xFF},
    {0xF4, 0xFF},
    {0xF5, 1}, // game init enemies
    {0xF6, 0}, // Run normal enemy death animation effects
    {0xF7, 1}, // delay damage display effect
    {0xF8, 1}, // effect stuff
    {0xF9, 0}, // resets actor orientation
    {0xFA, 0},
    {0xFB, 4},
    {0xFC, 0}, // setting orientation for target-all action
    {0xFD, 6}, // set resting position
    {0xFE, 0xFF},
    {0xFF, 0xFF},
};

const std::unordered_set<byte> endingOpCode{{0xA2, 0xA7, 0xA9, 0xB6, 0xF1}};
std::unordered_set<uint32_t> patchedAddress{};

enum class interpolation_trick
{
    NONE = 0,
    PAUSE_TRICK,
    PREVIOUS_VALUE_TRICK,
    ONLY_ONECALL_TRICK,
};
struct aux_effect_data
{
    bool isFirstTimeRunning;
    interpolation_trick interpolationTrick;
    int frameCounter;
};

std::array<aux_effect_data, 100> aux_effect100_data;
std::array<aux_effect_data, 60> aux_effect60_data;
std::array<aux_effect_data, 10> aux_effect10_data;

bool isAddFunctionDisabled = false;

void patchAnimationScriptArg(byte *scriptPointer, byte position)
{
    if (!patchedAddress.contains((DWORD)(scriptPointer + position)))
    {
        byte beforeValue = scriptPointer[position];
        scriptPointer[position] = scriptPointer[position] * frame_multiplier;

        if (beforeValue >= 0x100 / frame_multiplier)
            ffnx_error("Script arg multiplication out of bound at 0x%x: before is %d, after is %d\n", (DWORD)(scriptPointer + position),
                       beforeValue, scriptPointer[position]);
    }

    patchedAddress.insert((DWORD)(scriptPointer + position));
}

byte getActorIdleAnimScript(byte actorID)
{
    return ff7_externals.g_actor_idle_scripts[actorID];
}

battle_model_state *getBattleModelState(byte actorID)
{
    return &(ff7_externals.g_battle_model_state[actorID]);
}

battle_model_state_small *getSmallBattleModelState(byte actorID)
{
    return &(ff7_externals.g_small_battle_model_state[actorID]);
}

byte *getAnimScriptPointer(byte **ptrToScriptTable, battle_model_state &ownerModelState)
{
    byte *scriptPtr = ptrToScriptTable[ownerModelState.animScriptIndex];
    if (ownerModelState.animScriptIndex >= 0x2E && ownerModelState.animScriptIndex <= 0x3B && ownerModelState.animScriptIndex != 0x33)
    {
        if (ownerModelState.animScriptIndex == 0x39)
            ownerModelState.field_25 |= 0x80u;
        scriptPtr = ff7_externals.animation_script_pointers[ownerModelState.animScriptIndex - 0x2E];
    }
    return scriptPtr;
}

void ff7_run_animation_script(byte actorID, byte **ptrToScriptTable)
{
    // creates copy of model states
    battle_model_state ownerModelState = *getBattleModelState(actorID);
    battle_model_state_small smallModelState = *getSmallBattleModelState(actorID);

    byte script_wait_frames = *ff7_externals.g_script_wait_frames;

    if (!*ff7_externals.g_is_battle_paused)
    {
        bool isScriptActive = true;
        byte *scriptPtr = getAnimScriptPointer(ptrToScriptTable, ownerModelState);

        if (ownerModelState.modelEffectFlags & 1)
        {
            ownerModelState.isScriptExecuting = 1;
            ownerModelState.currentScriptPosition = 0;
            ownerModelState.waitFrames = 0;
        }
        if (ownerModelState.isScriptExecuting)
        {
            ownerModelState.playedAnimFrames = 0;
            while (isScriptActive)
            {
                byte currentOpCode = scriptPtr[ownerModelState.currentScriptPosition++];

                switch (currentOpCode)
                {
                case 0xC5:
                    ownerModelState.waitFrames = script_wait_frames;
                    break;
                case 0xC6:
                    patchAnimationScriptArg(scriptPtr, ownerModelState.currentScriptPosition);
                    script_wait_frames = scriptPtr[ownerModelState.currentScriptPosition++];
                    break;
                case 0x9E:
                    if (actorID == *ff7_externals.special_actor_id)
                    {
                        if (*ff7_externals.effect100_counter != 0)
                            ownerModelState.currentScriptPosition--;
                    }
                    else if (!(getBattleModelState(*ff7_externals.special_actor_id)->actorIsNotActing == 1 && *ff7_externals.effect100_counter == 0))
                    {
                        ownerModelState.currentScriptPosition--;
                    }
                    isScriptActive = false;
                    break;
                case 0xB3:
                    if ((smallModelState.field_0 & 0x1000) != 0)
                    {
                        byte position;
                        do
                            position = scriptPtr[ownerModelState.currentScriptPosition++];
                        while (position != 0xB2);
                    }
                    break;
                case 0xC1:
                    ownerModelState.currentScriptPosition = 0;
                    byte position;
                    do
                        position = scriptPtr[ownerModelState.currentScriptPosition++];
                    while (position != 0xC9);
                    break;
                case 0xCA:
                    if (*ff7_externals.g_is_effect_loading)
                    {
                        ownerModelState.currentScriptPosition = 0;
                        byte position;
                        do
                            position = scriptPtr[ownerModelState.currentScriptPosition++];
                        while (position != 0xC9);
                    }
                    break;
                case 0xCE:
                    if (actorID >= 4)
                    {
                        byte position;
                        do
                            position = scriptPtr[ownerModelState.currentScriptPosition++];
                        while (position != 0xCD);
                    }
                    break;
                case 0xEB:
                case 0xEC:
                    if (*ff7_externals.g_is_effect_loading)
                    {
                        ownerModelState.currentScriptPosition--;
                        isScriptActive = false;
                    }
                    break;
                case 0xF3:
                    if (ownerModelState.waitFrames != 0)
                    {
                        ownerModelState.waitFrames--;
                        ownerModelState.currentScriptPosition--;
                        isScriptActive = false;
                    }
                    break;
                case 0xF4:
                    patchAnimationScriptArg(scriptPtr, ownerModelState.currentScriptPosition);
                    ownerModelState.waitFrames = scriptPtr[ownerModelState.currentScriptPosition++];
                    break;
                case 0xFE:
                    if (ownerModelState.waitFrames == 0)
                    {
                        currentOpCode = scriptPtr[ownerModelState.currentScriptPosition];
                        if (currentOpCode == 0xC0)
                        {
                            ownerModelState.currentScriptPosition = 0;
                            ownerModelState.waitFrames = 0;
                            ownerModelState.isScriptExecuting = 0;
                            ownerModelState.runningAnimIdx = *scriptPtr;
                            ownerModelState.animScriptIndex = getActorIdleAnimScript(actorID);
                            scriptPtr = ptrToScriptTable[ownerModelState.animScriptIndex];
                        }
                    }
                    break;
                case 0xEE:
                case 0xFF:
                    ownerModelState.actorIsNotActing = 1;
                    ownerModelState.currentScriptPosition = 0;
                    ownerModelState.isScriptExecuting = 0;
                    ownerModelState.waitFrames = 0;
                    ownerModelState.animScriptIndex = getActorIdleAnimScript(actorID);
                    scriptPtr = ptrToScriptTable[ownerModelState.animScriptIndex];
                    break;
                default:
                    if (numArgsOpCode.contains(currentOpCode))
                    {
                        ownerModelState.currentScriptPosition += numArgsOpCode.at(currentOpCode);
                        if (endingOpCode.contains(currentOpCode))
                            isScriptActive = false;
                    }
                    else
                    {
                        isScriptActive = false;
                    }
                    break;
                }
            }
        }
    }

    // execute original run animation script
    ((void (*)(byte, byte **))ff7_externals.run_animation_script)(actorID, ptrToScriptTable);

    if (ownerModelState.currentScriptPosition != getBattleModelState(actorID)->currentScriptPosition)
        ffnx_error("%s - Animation script pointer simulation wrong! Final position does not match (simulation: %d != real: %d)\n", __func__,
                   ownerModelState.currentScriptPosition, getBattleModelState(actorID)->currentScriptPosition);

    if (ownerModelState.waitFrames != getBattleModelState(actorID)->waitFrames)
        ffnx_error("%s - Camera script pointer simulation wrong! Final frames to wait does not match (simulation: %d != real: %d)\n", __func__,
                   ownerModelState.waitFrames, getBattleModelState(actorID)->waitFrames);
}

int ff7_add_fn_to_effect100_fn(uint32_t function)
{
    int idx;
    for (idx = 0; idx < 100; idx++)
    {
        if (ff7_externals.effect100_array_fn[idx] == 0 && *ff7_externals.effect100_array_idx <= idx)
            break;
    }
    if (idx >= 100)
        return 0xFFFF;
    if (isAddFunctionDisabled)
        return idx;

    ff7_externals.effect100_array_fn[idx] = function;
    ff7_externals.effect100_array_data[idx].field_0 = *ff7_externals.effect100_array_idx;
    *ff7_externals.effect100_counter = *ff7_externals.effect100_counter + 1;

    aux_effect100_data[idx].frameCounter = 0;
    aux_effect100_data[idx].interpolationTrick = interpolation_trick::NONE;
    aux_effect100_data[idx].isFirstTimeRunning = true;
    return idx;
}

void ff7_add_kotr_camera_fn_to_effect100_fn(DWORD param_1, DWORD param_2, WORD param_3)
{
    ff7_externals.add_kotr_camera_fn_to_effect100_fn_476AAB(param_1, param_2, param_3);
    
    constexpr int kotr_camera_idx = 99;
    aux_effect100_data[kotr_camera_idx].frameCounter = 0;
    aux_effect100_data[kotr_camera_idx].interpolationTrick = interpolation_trick::NONE;
    aux_effect100_data[kotr_camera_idx].isFirstTimeRunning = true;
}

void ff7_execute_effect100_fn()
{
    uint16_t &fn_index = *ff7_externals.effect100_array_idx;
    for (fn_index = 0; fn_index < 100; fn_index++)
    {
        if(ff7_externals.effect100_array_fn[fn_index] && *ff7_externals.g_is_battle_running_9AD1AC)
        {
            if (aux_effect100_data[fn_index].isFirstTimeRunning) 
            {
                if (ff7_externals.effect100_array_fn[fn_index] == ff7_externals.display_battle_action_text_42782A)
                {
                    ff7_externals.effect100_array_data[fn_index].field_6 *= frame_multiplier;
                }
                else if (ff7_externals.effect100_array_fn[fn_index] == ff7_externals.battle_sub_425D29)
                {
                    ff7_externals.effect100_array_data[fn_index].n_frames *= frame_multiplier;
                }
                else if (ff7_externals.effect100_array_fn[fn_index] == ff7_externals.battle_sub_5BDA0F)
                {
                    ff7_externals.effect100_array_data[fn_index].field_2 /= frame_multiplier;
                    ff7_externals.effect100_array_data[fn_index].n_frames *= frame_multiplier;
                }
                else if (ff7_externals.effect100_array_fn[fn_index] == ff7_externals.tifa_limit_1_2_sub_4E3D51 ||
                         ff7_externals.effect100_array_fn[fn_index] == ff7_externals.tifa_limit_2_1_sub_4E48D4)
                {
                    ff7_externals.effect100_array_data[fn_index].field_1A *= frame_multiplier;
                }
                else if(ff7_externals.effect100_array_fn[fn_index] == ff7_externals.run_summon_odin_steel_sub_4A9908)
                {
                    ff7_odin_steel_frames_AEEC14 = *ff7_externals.field_odin_frames_AEEC14 * frame_multiplier;
                }
                else if (ff7_externals.effect100_array_fn[fn_index] == ff7_externals.battle_enemy_death_5BBD24 ||
                         ff7_externals.effect100_array_fn[fn_index] == ff7_externals.battle_iainuki_death_5BCAAA ||
                         ff7_externals.effect100_array_fn[fn_index] == ff7_externals.battle_boss_death_5BC48C ||
                         ff7_externals.effect100_array_fn[fn_index] == ff7_externals.battle_melting_death_5BC21F ||
                         ff7_externals.effect100_array_fn[fn_index] == ff7_externals.battle_disintegrate_2_death_5BBA82 ||
                         ff7_externals.effect100_array_fn[fn_index] == ff7_externals.battle_morph_death_5BC812 ||
                         ff7_externals.effect100_array_fn[fn_index] == ff7_externals.run_summon_animations_5C0E4B ||
                         ff7_externals.effect100_array_fn[fn_index] == ff7_externals.vincent_limit_fade_effect_sub_5D4240)
                {
                    aux_effect100_data[fn_index].interpolationTrick = interpolation_trick::NONE;
                }
                else if(ff7_externals.effect100_array_fn[fn_index] == ff7_externals.run_bahamut_zero_main_loop_484A16)
                {
                    aux_effect100_data[fn_index].interpolationTrick = interpolation_trick::ONLY_ONECALL_TRICK;
                }
                else
                {
                    aux_effect100_data[fn_index].interpolationTrick = interpolation_trick::PAUSE_TRICK;
                }

                if (trace_all || trace_battle_animation)
                    ffnx_trace("%s - begin function[%d]: 0x%x (actor_id: %d,last command: 0x%02X, 0x%04X)\n", __func__, fn_index,
                               ff7_externals.effect100_array_fn[fn_index], ff7_externals.anim_event_queue[0].attackerID,
                               ff7_externals.battle_context->lastCommandIdx, ff7_externals.battle_context->lastActionIdx);

                aux_effect100_data[fn_index].isFirstTimeRunning = false;
            }

            if (aux_effect100_data[fn_index].interpolationTrick != interpolation_trick::NONE)
            {
                if (aux_effect100_data[fn_index].frameCounter % frame_multiplier == 0)
                {
                    ((void (*)())ff7_externals.effect100_array_fn[fn_index])();
                }
                else
                {
                    byte was_paused = *ff7_externals.g_is_battle_paused;
                    auto data_prev = ff7_externals.effect100_array_data[fn_index];

                    switch (aux_effect100_data[fn_index].interpolationTrick)
                    {
                    case interpolation_trick::PREVIOUS_VALUE_TRICK:
                        isAddFunctionDisabled = true;
                        ((void (*)())ff7_externals.effect100_array_fn[fn_index])();
                        ff7_externals.effect100_array_data[fn_index].field_2 = data_prev.field_2;
                        isAddFunctionDisabled = false;
                        break;
                    case interpolation_trick::PAUSE_TRICK:
                        *ff7_externals.g_is_battle_paused = 1;
                        ((void (*)())ff7_externals.effect100_array_fn[fn_index])();
                        *ff7_externals.g_is_battle_paused = was_paused;
                        break;
                    default:
                        break;
                    }
                }
                aux_effect100_data[fn_index].frameCounter++;
            }
            else
            {
                ((void (*)())ff7_externals.effect100_array_fn[fn_index])();
            }

            if (ff7_externals.effect100_array_data[fn_index].field_0 == (uint16_t)-1)
            {
                if (trace_all || trace_battle_animation)
                    ffnx_trace("%s - end function[%d]: 0x%x (actor_id: %d,last command: 0x%02X, 0x%04X)\n", __func__, fn_index,
                               ff7_externals.effect100_array_fn[fn_index], ff7_externals.anim_event_queue[0].attackerID,
                               ff7_externals.battle_context->lastCommandIdx, ff7_externals.battle_context->lastActionIdx);

                ff7_externals.effect100_array_data[fn_index].field_0 = 0;
                ff7_externals.effect100_array_data[fn_index].field_2 = 0;
                ff7_externals.effect100_array_fn[fn_index] = 0;
                *ff7_externals.effect100_counter = *ff7_externals.effect100_counter - 1;
            }
        }
        else if(ff7_externals.effect100_array_fn[fn_index] == ff7_externals.display_battle_action_text_42782A)
        {
            if (aux_effect100_data[fn_index].isFirstTimeRunning)
            {
                ff7_externals.effect100_array_data[fn_index].field_6 *= frame_multiplier;
                aux_effect100_data[fn_index].isFirstTimeRunning = false;
            }

            ((void (*)())ff7_externals.effect100_array_fn[fn_index])();
        }
    }
    fn_index = 0;
}

int ff7_add_fn_to_effect10_fn(uint32_t function)
{
    int idx;
    for (idx = 0; idx < 10; idx++)
    {
        if (ff7_externals.effect10_array_fn[idx] == 0 && *ff7_externals.effect10_array_idx <= idx)
            break;
    }
    if (idx >= 10)
        return 0xFFFF;
    if (isAddFunctionDisabled)
        return idx;

    ff7_externals.effect10_array_fn[idx] = function;
    ff7_externals.effect10_array_data[idx].field_0 = *ff7_externals.effect10_array_idx;
    *ff7_externals.effect10_counter = *ff7_externals.effect10_counter + 1;

    aux_effect10_data[idx].isFirstTimeRunning = true;
    return idx;
}

void ff7_execute_effect10_fn()
{
    uint16_t &fn_index = *ff7_externals.effect10_array_idx;
    for (fn_index = 0; fn_index < 10; fn_index++)
    {
        auto &effect10_data = ff7_externals.effect10_array_data[fn_index];
        if (ff7_externals.effect10_array_fn[fn_index] != 0)
        {
            if (aux_effect10_data[fn_index].isFirstTimeRunning)
            {
                if (ff7_externals.effect10_array_fn[fn_index] == ff7_externals.battle_sub_426DE3)
                {
                    // Related to resting positions
                    effect10_data.n_frames *= frame_multiplier;
                    effect10_data.field_18 *= frame_multiplier;
                    effect10_data.field_C /= frame_multiplier;
                    effect10_data.field_E /= frame_multiplier;
                    effect10_data.field_6 /= frame_multiplier;
                }
                else if (ff7_externals.effect10_array_fn[fn_index] == ff7_externals.battle_sub_426941)
                {
                    // Related to resting positions
                    effect10_data.n_frames *= frame_multiplier;
                    effect10_data.field_A /= frame_multiplier;
                    effect10_data.field_C /= frame_multiplier;
                }
                else if (ff7_externals.effect10_array_fn[fn_index] == ff7_externals.battle_sub_426899)
                {
                    // Related to resting Y rotation
                    effect10_data.n_frames *= frame_multiplier;
                    effect10_data.field_E /= frame_multiplier;
                }
                else if (ff7_externals.effect10_array_fn[fn_index] == ff7_externals.battle_sub_4267F1)
                {
                    // Related to resting Y position
                    effect10_data.n_frames *= frame_multiplier;
                    effect10_data.field_A /= frame_multiplier;
                }
                else if (ff7_externals.effect10_array_fn[fn_index] == ff7_externals.battle_move_character_sub_426A26)
                {
                    // Animation of moving characters from attacker to attacked
                    effect10_data.n_frames *= frame_multiplier;
                    effect10_data.field_18 *= frame_multiplier;
                    effect10_data.field_C /= frame_multiplier;
                    effect10_data.field_E /= frame_multiplier;
                }
                else if (ff7_externals.effect10_array_fn[fn_index] == ff7_externals.battle_move_character_sub_42739D)
                {
                    // Animation of moving characters from attacker to attacked
                    effect10_data.n_frames *= frame_multiplier;
                    effect10_data.field_19 *= frame_multiplier;
                    effect10_data.field_1A *= frame_multiplier;
                    effect10_data.field_C /= frame_multiplier;
                    effect10_data.field_E /= frame_multiplier;
                }
                else if (ff7_externals.effect10_array_fn[fn_index] == ff7_externals.battle_move_character_sub_426F58)
                {
                    effect10_data.n_frames *= frame_multiplier;
                    // Do not modify the others, already done elsewhere
                }
                else if (ff7_externals.effect10_array_fn[fn_index] == ff7_externals.battle_move_character_sub_4270DE)
                {
                    // Animation of moving characters for some limit breaks from attacker to attacked
                    effect10_data.n_frames *= frame_multiplier;
                    effect10_data.field_19 *= frame_multiplier;
                    effect10_data.field_1A *= frame_multiplier;
                    effect10_data.field_C /= frame_multiplier;
                    effect10_data.field_E /= frame_multiplier;
                    effect10_data.field_14 /= frame_multiplier;
                }
                if (trace_all || trace_battle_animation)
                    ffnx_trace("%s - begin function[%d]: 0x%x (actor_id: %d,last command: 0x%02X, 0x%04X)\n", __func__, fn_index,
                               ff7_externals.effect10_array_fn[fn_index], ff7_externals.anim_event_queue[0].attackerID,
                               ff7_externals.battle_context->lastCommandIdx, ff7_externals.battle_context->lastActionIdx);

                aux_effect10_data[fn_index].isFirstTimeRunning = false;
            }

            ((void (*)())ff7_externals.effect10_array_fn[fn_index])();

            if (effect10_data.field_0 == (uint16_t)-1)
            {
                if (trace_all || trace_battle_animation)
                    ffnx_trace("%s - end function[%d]: 0x%x (actor_id: %d,last command: 0x%02X, 0x%04X)\n", __func__, fn_index,
                               ff7_externals.effect10_array_fn[fn_index], ff7_externals.anim_event_queue[0].attackerID,
                               ff7_externals.battle_context->lastCommandIdx, ff7_externals.battle_context->lastActionIdx);

                effect10_data.field_0 = 0;
                effect10_data.field_2 = 0;
                ff7_externals.effect10_array_fn[fn_index] = 0;
                *ff7_externals.effect10_counter = *ff7_externals.effect10_counter - 1;
            }
        }
    }
    fn_index = 0;
}

int ff7_add_fn_to_effect60_fn(uint32_t function)
{
    int idx;
    for (idx = 0; idx < 60; idx++)
    {
        if (ff7_externals.effect60_array_fn[idx] == 0 && *ff7_externals.effect60_array_idx <= idx)
            break;
    }
    if (idx >= 60)
        return 0xFFFF;
    if (isAddFunctionDisabled)
        return idx;

    ff7_externals.effect60_array_fn[idx] = function;
    ff7_externals.effect60_array_data[idx].field_0 = *ff7_externals.effect60_array_idx;
    *ff7_externals.effect60_counter = *ff7_externals.effect60_counter + 1;

    aux_effect60_data[idx].frameCounter = 0;
    aux_effect60_data[idx].interpolationTrick = interpolation_trick::NONE;
    aux_effect60_data[idx].isFirstTimeRunning = true;
    return idx;
}

void ff7_execute_effect60_fn()
{
    uint16_t &fn_index = *ff7_externals.effect60_array_idx;
    for (fn_index = 0; fn_index < 60; fn_index++)
    {
        if (ff7_externals.effect60_array_fn[fn_index] != 0)
        {
            if (aux_effect60_data[fn_index].isFirstTimeRunning)
            {
                if (ff7_externals.effect60_array_fn[fn_index] == ff7_externals.battle_sub_4276B6 ||
                    ff7_externals.effect60_array_fn[fn_index] == ff7_externals.battle_sub_4255B7 ||
                    ff7_externals.effect60_array_fn[fn_index] == ff7_externals.battle_sub_427737 ||
                    ff7_externals.effect60_array_fn[fn_index] == ff7_externals.battle_sub_425AAD ||
                    ff7_externals.effect60_array_fn[fn_index] == ff7_externals.battle_sub_427AF1 ||
                    ff7_externals.effect60_array_fn[fn_index] == ff7_externals.battle_sub_4277B1)
                {
                    ff7_externals.effect60_array_data[fn_index].n_frames *= frame_multiplier;
                }
                else if (ff7_externals.effect60_array_fn[fn_index] == ff7_externals.battle_sub_5BD96D)
                {
                    ff7_externals.effect60_array_data[fn_index].n_frames *= frame_multiplier;
                    ff7_externals.effect60_array_data[fn_index].field_2 /= frame_multiplier;
                }
                else if (ff7_externals.effect60_array_fn[fn_index] == ff7_externals.battle_sub_425E5F ||
                         ff7_externals.effect60_array_fn[fn_index] == ff7_externals.battle_sub_5C1C8F ||
                         ff7_externals.effect60_array_fn[fn_index] == ff7_externals.battle_sub_5BCF9D ||
                         ff7_externals.effect60_array_fn[fn_index] == ff7_externals.battle_sub_425520 ||
                         ff7_externals.effect60_array_fn[fn_index] == ff7_externals.battle_boss_death_sub_5BC5EC ||
                         ff7_externals.effect60_array_fn[fn_index] == ff7_externals.battle_sub_5BCD42 ||
                         ff7_externals.effect60_array_fn[fn_index] == ff7_externals.display_battle_damage_5BB410 ||
                         ff7_externals.effect60_array_fn[fn_index] == ff7_externals.magic_aura_effects_5C0300 ||
                         ff7_externals.effect60_array_fn[fn_index] == ff7_externals.limit_break_aura_effects_5C0572 ||
                         ff7_externals.effect60_array_fn[fn_index] == ff7_externals.enemy_skill_aura_effects_5C06BF ||
                         ff7_externals.effect60_array_fn[fn_index] == ff7_externals.summon_aura_effects_5C0953 ||
                         ff7_externals.effect60_array_fn[fn_index] == ff7_externals.battle_sub_5C18BC)
                {
                    aux_effect60_data[fn_index].interpolationTrick = interpolation_trick::NONE;
                }
                else if(ff7_externals.effect60_array_fn[fn_index] == ff7_externals.battle_smoke_move_handler_5BE4E2)
                {
                    aux_effect60_data[fn_index].interpolationTrick = interpolation_trick::ONLY_ONECALL_TRICK;
                }
                else
                {
                    aux_effect60_data[fn_index].interpolationTrick = interpolation_trick::PAUSE_TRICK;
                }

                if (trace_all || trace_battle_animation)
                    ffnx_trace("%s - begin function[%d]: 0x%x (actor_id: %d,last command: 0x%02X, 0x%04X)\n", __func__, fn_index,
                               ff7_externals.effect60_array_fn[fn_index], ff7_externals.anim_event_queue[0].attackerID,
                               ff7_externals.battle_context->lastCommandIdx, ff7_externals.battle_context->lastActionIdx);

                aux_effect60_data[fn_index].isFirstTimeRunning = false;
            }

            if (aux_effect60_data[fn_index].interpolationTrick != interpolation_trick::NONE)
            {
                if (aux_effect60_data[fn_index].frameCounter % frame_multiplier == 0)
                {
                    ((void (*)())ff7_externals.effect60_array_fn[fn_index])();
                }
                else
                {
                    byte was_paused = *ff7_externals.g_is_battle_paused;
                    auto data_prev = ff7_externals.effect60_array_data[fn_index];

                    switch (aux_effect60_data[fn_index].interpolationTrick)
                    {
                    case interpolation_trick::PREVIOUS_VALUE_TRICK:
                        isAddFunctionDisabled = true;
                        ((void (*)())ff7_externals.effect60_array_fn[fn_index])();
                        ff7_externals.effect60_array_data[fn_index].field_2 = data_prev.field_2;
                        isAddFunctionDisabled = false;
                        break;
                    case interpolation_trick::PAUSE_TRICK:
                        *ff7_externals.g_is_battle_paused = 1;
                        ((void (*)())ff7_externals.effect60_array_fn[fn_index])();
                        *ff7_externals.g_is_battle_paused = was_paused;
                        break;
                    default:
                        break;
                    }
                }

                aux_effect60_data[fn_index].frameCounter++;
            }
            else
            {
                ((void (*)())ff7_externals.effect60_array_fn[fn_index])();
            }

            if (ff7_externals.effect60_array_data[fn_index].field_0 == (uint16_t)-1)
            {
                if (trace_all || trace_battle_animation)
                    ffnx_trace("%s - end function[%d]: 0x%x (actor_id: %d,last command: 0x%02X, 0x%04X)\n", __func__, fn_index,
                               ff7_externals.effect60_array_fn[fn_index], ff7_externals.anim_event_queue[0].attackerID,
                               ff7_externals.battle_context->lastCommandIdx, ff7_externals.battle_context->lastActionIdx);

                ff7_externals.effect60_array_data[fn_index].field_0 = 0;
                ff7_externals.effect60_array_data[fn_index].field_2 = 0;
                ff7_externals.effect60_array_fn[fn_index] = 0;
                *ff7_externals.effect60_counter = *ff7_externals.effect60_counter - 1;
            }
        }
    }
    fn_index = 0;
}

void ff7_boss_death_animation_5BC5EC()
{
    auto &fn_data = ff7_externals.effect60_array_data[*ff7_externals.effect60_array_idx];
    if (fn_data.n_frames == 0)
    {
        fn_data.field_0 = 0xFFFF;
    }
    else
    {
        if (fn_data.n_frames == (58 * frame_multiplier) || fn_data.n_frames == (64 * frame_multiplier))
            *ff7_externals.field_battle_BFB2E0 = ((uint32_t(*)(byte, byte, byte))ff7_externals.battle_boss_death_call_5BD436)(0xFA, 0xFA, 0xFA);

        std::array<short, 8> offset_z_position{64, 32, 0, -32, -64, -32, 0, 32};
        int index = ((fn_data.n_frames * (4 / frame_multiplier)) % offset_z_position.size());
        getBattleModelState(fn_data.field_8)->restingZPosition = fn_data.field_A + offset_z_position[index];
        fn_data.n_frames--;
    }
}

int ff7_get_n_frames_display_action_string()
{
    int shiftValue = 2 - frame_multiplier / 2;
    return ((int)*ff7_externals.field_byte_DC0E11 >> shiftValue) + 4 * frame_multiplier;
}

void ff7_battle_disintegrate_1_death_sub_5BC04D(byte effect10_array_idx)
{
    auto &fn_data = ff7_externals.effect10_array_data[effect10_array_idx];
    auto &battle_model_state = *getBattleModelState(fn_data.field_8);
    if (fn_data.n_frames == 0)
    {
        fn_data.field_0 = 0xFFFF;
        battle_model_state.field_25 &= 0x7F;
        battle_model_state.field_C &= 0xFFDF;
        battle_model_state.actorIsNotActing = 1;
        ((void (*)(byte))ff7_externals.battle_sub_42C0A7)(fn_data.field_8);
    }
    else
    {
        battle_model_state.field_14 += 0x80 / frame_multiplier;
        if (battle_model_state.field_28 > 0)
            battle_model_state.field_28 -= 0x10 / frame_multiplier;

        battle_model_state.field_1AC8 = 1;

        // effect10_array_data_8FE1F6 is a short array of 15 size. To avoid overflowing, divide also the index
        battle_model_state.field_1ADC += (float)(int)ff7_externals.effect10_array_data_8FE1F6[(int)(fn_data.n_frames / frame_multiplier)] / frame_multiplier;
        battle_model_state.field_1ACC += 22.5f / frame_multiplier;
        battle_model_state.field_1AD0 += 22.5f / frame_multiplier;
        battle_model_state.field_1AD4 += 22.5f / frame_multiplier;
        fn_data.n_frames--;
    }
}

void ff7_battle_move_character_sub_426F58()
{
    auto &fn_data = ff7_externals.effect10_array_data[*ff7_externals.effect10_array_idx];
    if (fn_data.n_frames == 0)
    {
        fn_data.field_0 = 0xFFFF;
    }
    else
    {
        *ff7_externals.g_script_args[2] = fn_data.field_A;
        *ff7_externals.g_script_args[3] = fn_data.field_8;
        *ff7_externals.g_script_args[4] = fn_data.field_10;
        getBattleModelState(*ff7_externals.g_script_args[3])->restingXPosition += fn_data.field_C / frame_multiplier;
        getBattleModelState(*ff7_externals.g_script_args[3])->restingZPosition += fn_data.field_E / frame_multiplier;

        int index = (int)(fn_data.field_18 / frame_multiplier) + *ff7_externals.g_script_args[4] * 8; // to avoid overflow
        getBattleModelState(*ff7_externals.g_script_args[3])->restingYPosition += ff7_externals.resting_Y_array_data[index] / frame_multiplier;
        fn_data.field_18++;
        fn_data.n_frames--;
    }
}

void ff7_battle_animations_hook_init()
{
    replace_call_function(ff7_externals.battle_sub_42A5EB + 0xB8, ff7_run_animation_script);
    replace_call_function(ff7_externals.battle_sub_42E275 + 0xB2, ff7_run_animation_script);
    replace_call_function(ff7_externals.battle_sub_42E34A + 0x76, ff7_run_animation_script);
    replace_call_function(ff7_externals.battle_sub_5BD5E9 + 0x22F, ff7_run_animation_script);
    replace_call_function(ff7_externals.battle_sub_5C1D9A + 0x4A, ff7_run_animation_script);
    replace_function(ff7_externals.add_fn_to_effect100_fn, ff7_add_fn_to_effect100_fn);
    replace_function(ff7_externals.add_fn_to_effect10_fn, ff7_add_fn_to_effect10_fn);
    replace_function(ff7_externals.add_fn_to_effect60_fn, ff7_add_fn_to_effect60_fn);
    replace_function(ff7_externals.execute_effect100_fn, ff7_execute_effect100_fn);
    replace_function(ff7_externals.execute_effect10_fn, ff7_execute_effect10_fn);
    replace_function(ff7_externals.execute_effect60_fn, ff7_execute_effect60_fn);

    // Normal enemy death
    patch_multiply_code<WORD>(ff7_externals.battle_enemy_death_5BBD24 + 0x40, frame_multiplier);
    patch_divide_code<WORD>(ff7_externals.battle_enemy_death_sub_5BBE32 + 0xA8, frame_multiplier);
    patch_divide_code<byte>(ff7_externals.battle_enemy_death_sub_5BBE32 + 0xCB, frame_multiplier);

    // Enemy death - iainuki
    patch_multiply_code<WORD>(ff7_externals.battle_iainuki_death_5BCAAA + 0x40, frame_multiplier);
    patch_divide_code<WORD>(ff7_externals.battle_iainuki_death_sub_5BCBB8 + 0x9F, frame_multiplier);
    patch_divide_code<byte>(ff7_externals.battle_iainuki_death_sub_5BCBB8 + 0xC2, frame_multiplier);
    patch_divide_code<byte>(ff7_externals.battle_iainuki_death_sub_5BCBB8 + 0xE3, frame_multiplier);
    patch_divide_code<byte>(ff7_externals.battle_iainuki_death_sub_5BCBB8 + 0x104, frame_multiplier);
    patch_divide_code<WORD>(ff7_externals.battle_iainuki_death_sub_5BCBB8 + 0x154, frame_multiplier);

    // Boss death
    patch_multiply_code<WORD>(ff7_externals.battle_boss_death_5BC48C + 0x40, frame_multiplier);
    patch_multiply_code<WORD>(ff7_externals.battle_boss_death_5BC48C + 0xCF, frame_multiplier);
    patch_divide_code<byte>(ff7_externals.battle_boss_death_sub_5BC6ED + 0xCF, frame_multiplier);
    patch_divide_code<byte>(ff7_externals.battle_boss_death_sub_5BC6ED + 0xF1, frame_multiplier);
    replace_function(ff7_externals.battle_boss_death_sub_5BC5EC, ff7_boss_death_animation_5BC5EC);

    // Enemy death - melting
    patch_multiply_code<WORD>(ff7_externals.battle_melting_death_5BC21F + 0x40, frame_multiplier);
    patch_divide_code<WORD>(ff7_externals.battle_melting_death_sub_5BC32D + 0xAB, frame_multiplier);
    patch_divide_code<byte>(ff7_externals.battle_melting_death_sub_5BC32D + 0xCE, frame_multiplier);
    patch_divide_code<WORD>(ff7_externals.battle_melting_death_sub_5BC32D + 0xEE, frame_multiplier);
    patch_divide_code<WORD>(ff7_externals.battle_melting_death_sub_5BC32D + 0x10D, frame_multiplier);
    patch_divide_code<WORD>(ff7_externals.battle_melting_death_sub_5BC32D + 0x12C, frame_multiplier);

    // Enemy death - disintegrate 2
    patch_multiply_code<WORD>(ff7_externals.battle_disintegrate_2_death_5BBA82 + 0x40, frame_multiplier);
    patch_divide_code<WORD>(ff7_externals.battle_disintegrate_2_death_sub_5BBBDE + 0xB9, frame_multiplier);
    patch_divide_code<byte>(ff7_externals.battle_disintegrate_2_death_sub_5BBBDE + 0xDB, frame_multiplier);
    patch_divide_code<float>((uint32_t)ff7_externals.field_float_battle_7B7680, frame_multiplier); // float value used also elsewhere

    // Enemy death - morph
    patch_multiply_code<WORD>(ff7_externals.battle_morph_death_5BC812 + 0x40, frame_multiplier);
    patch_divide_code<WORD>(ff7_externals.battle_morph_death_sub_5BC920 + 0x9F, frame_multiplier);
    patch_divide_code<byte>(ff7_externals.battle_morph_death_sub_5BC920 + 0xC2, frame_multiplier);
    patch_divide_code<byte>(ff7_externals.battle_morph_death_sub_5BC920 + 0xE3, frame_multiplier);
    patch_divide_code<byte>(ff7_externals.battle_morph_death_sub_5BC920 + 0x104, frame_multiplier);
    patch_divide_code<WORD>(ff7_externals.battle_morph_death_sub_5BC920 + 0x154, frame_multiplier);

    // Enemy death - disintegrate 1 
    patch_multiply_code<WORD>(ff7_externals.battle_disintegrate_1_death_5BBF31 + 0x40, frame_multiplier);
    replace_function(ff7_externals.battle_disintegrate_1_death_sub_5BC04D, ff7_battle_disintegrate_1_death_sub_5BC04D);

    // Display string related
    replace_function(ff7_externals.get_n_frames_display_action_string, ff7_get_n_frames_display_action_string);

    // Character movement (e.g. movement animation for attacks)
    replace_function(ff7_externals.battle_move_character_sub_426F58, ff7_battle_move_character_sub_426F58);

    // Character fade in/out (i.e. multiply g_script_wait_frames and other things)
    patch_multiply_code<byte>(ff7_externals.battle_sub_42A72D + 0x11A, frame_multiplier);
    patch_multiply_code<WORD>(ff7_externals.vincent_limit_fade_effect_sub_5D4240 + 0x24, frame_multiplier);
    patch_multiply_code<WORD>(ff7_externals.vincent_limit_fade_effect_sub_5D4240 + 0x57, frame_multiplier);
    patch_multiply_code<byte>(ff7_externals.vincent_limit_fade_effect_sub_5D4240 + 0x6E, frame_multiplier);
    patch_multiply_code<WORD>(ff7_externals.battle_sub_5C18BC + 0xDC, frame_multiplier);
    patch_multiply_code<byte>(ff7_externals.battle_sub_5C18BC + 0xE4, frame_multiplier);
    patch_multiply_code<WORD>(ff7_externals.battle_sub_5C1C8F + 0x55, frame_multiplier);
    patch_multiply_code<byte>(ff7_externals.battle_sub_5C1C8F + 0x5D, frame_multiplier);

    // Summons
    patch_multiply_code<byte>(ff7_externals.run_summon_animations_5C0E4B + 0x75, frame_multiplier);
    patch_multiply_code<byte>(ff7_externals.run_summon_animations_5C0E4B + 0x6D, frame_multiplier);
    patch_multiply_code<byte>(ff7_externals.run_summon_animations_5C0E4B + 0x15B, frame_multiplier);
    memset_code(ff7_externals.run_shiva_camera_58E60D + 0xC2D, 0x90, 6);
    patch_code_dword(ff7_externals.run_ramuh_camera_597206 + 0x44, 0x000B9585);
    memset_code(ff7_externals.run_odin_gunge_camera_4A0F52 + 0xC0C, 0x90, 6);
    patch_multiply_code<DWORD>(ff7_externals.run_summon_odin_steel_sub_4A9908 + 0x5E, frame_multiplier);
    patch_multiply_code<DWORD>(ff7_externals.run_summon_odin_steel_sub_4A9908 + 0x25D, frame_multiplier);
    patch_multiply_code<byte>(ff7_externals.run_summon_odin_steel_sub_4A9908 + 0x265, frame_multiplier);
    patch_code_dword(ff7_externals.run_summon_odin_steel_sub_4A9908 + 0x316, (DWORD)&ff7_odin_steel_frames_AEEC14);
    replace_call_function(ff7_externals.run_summon_kotr_sub_476857 + 0x1C6, ff7_add_kotr_camera_fn_to_effect100_fn);

    // Show Damage
    patch_multiply_code<WORD>(ff7_externals.display_battle_damage_5BB410 + 0x54, frame_multiplier);
    if(frame_multiplier == 2)
    {
        patch_code_dword(ff7_externals.display_battle_damage_5BB410 + 0x1E2, (DWORD)y_pos_offset_display_damage_30);
        patch_code_dword(ff7_externals.display_battle_damage_5BB410 + 0x2D7, (DWORD)y_pos_offset_display_damage_30);
    }
    else if(frame_multiplier == 4)
    {
        patch_code_dword(ff7_externals.display_battle_damage_5BB410 + 0x1E2, (DWORD)y_pos_offset_display_damage_60);
        patch_code_dword(ff7_externals.display_battle_damage_5BB410 + 0x2D7, (DWORD)y_pos_offset_display_damage_60);
    }

    // Aura animation (magic, limit break, enemy skill, summon)
    patch_code_byte(ff7_externals.magic_aura_effects_5C0300 + 0x4C, 0x7 - frame_multiplier / 2);
    patch_code_byte(ff7_externals.magic_aura_effects_5C0300 + 0x6A, 0xA - frame_multiplier / 2);
    patch_multiply_code<byte>(ff7_externals.magic_aura_effects_5C0300 + 0x88, frame_multiplier);
    patch_code_byte(ff7_externals.magic_aura_effects_5C0300 + 0xA2, 0xC - frame_multiplier / 2);
    patch_multiply_code<byte>(ff7_externals.magic_aura_effects_5C0300 + 0x138, frame_multiplier);
    patch_divide_code<DWORD>(ff7_externals.limit_break_aura_effects_5C0572 + 0x4C, frame_multiplier);
    patch_multiply_code<byte>(ff7_externals.limit_break_aura_effects_5C0572 + 0x6E, frame_multiplier);
    patch_divide_code<DWORD>(ff7_externals.limit_break_aura_effects_5C0572 + 0x7A, frame_multiplier);
    patch_multiply_code<byte>(ff7_externals.limit_break_aura_effects_5C0572 + 0x98, frame_multiplier);
    patch_multiply_code<byte>(ff7_externals.limit_break_aura_effects_5C0572 + 0xAD, frame_multiplier);
    patch_code_byte(ff7_externals.limit_break_aura_effects_5C0572 + 0xB0, 0x9 - frame_multiplier / 2);
    patch_multiply_code<byte>(ff7_externals.limit_break_aura_effects_5C0572 + 0x13E, frame_multiplier);
    patch_multiply_code<DWORD>(ff7_externals.enemy_skill_aura_effects_5C06BF + 0x5C, frame_multiplier);
    patch_code_byte(ff7_externals.enemy_skill_aura_effects_5C06BF + 0x64, 0x7 - frame_multiplier / 2);
    patch_multiply_code<DWORD>(ff7_externals.enemy_skill_aura_effects_5C06BF + 0x81, frame_multiplier);
    patch_code_byte(ff7_externals.enemy_skill_aura_effects_5C06BF + 0x89, 0xA - frame_multiplier / 2);
    patch_multiply_code<byte>(ff7_externals.enemy_skill_aura_effects_5C06BF + 0xA7, frame_multiplier);
    patch_divide_code<int>(ff7_externals.enemy_skill_aura_effects_5C06BF + 0xB3, frame_multiplier);
    patch_multiply_code<byte>(ff7_externals.enemy_skill_aura_effects_5C06BF + 0xD6, frame_multiplier);
    patch_code_byte(ff7_externals.enemy_skill_aura_effects_5C06BF + 0xD9, 0xC - frame_multiplier / 2);
    patch_multiply_code<byte>(ff7_externals.enemy_skill_aura_effects_5C06BF + 0x182, frame_multiplier);
    patch_code_byte(ff7_externals.summon_aura_effects_5C0953 + 0x4D, 0xC - frame_multiplier / 2);
    patch_multiply_code<byte>(ff7_externals.summon_aura_effects_5C0953 + 0x19D, frame_multiplier);

    // Limit break effects
    patch_multiply_code<byte>(ff7_externals.tifa_limit_2_1_sub_4E48D4 + 0x1FE, frame_multiplier);
    memset_code(ff7_externals.aerith_limit_2_1_sub_45B0CF + 0xCE, 0x90, 6);
    patch_multiply_code<byte>(ff7_externals.aerith_limit_2_1_sub_45B0CF + 0xE2, frame_multiplier);

    // Effect60 related
    patch_multiply_code<WORD>(ff7_externals.battle_sub_425E5F + 0x3A, frame_multiplier);

    patch_multiply_code<WORD>(ff7_externals.battle_sub_5BCF9D + 0x3A, frame_multiplier);
    patch_code_byte(ff7_externals.battle_sub_5BD050 + 0x1DC, 0x2 - frame_multiplier / 2);
    patch_code_byte(ff7_externals.battle_sub_5BD050 + 0x203, 0x2 - frame_multiplier / 2);

    patch_multiply_code<WORD>(ff7_externals.battle_sub_5BCD42 + 0x5B, frame_multiplier); 
    patch_divide_code<WORD>(ff7_externals.battle_sub_5BCD42 + 0x6E, frame_multiplier);

    // Tifa slots speed patch (bitwise and with 0x7 changed to 0x3)
    patch_code_byte(ff7_externals.battle_sub_6E3135 + 0x168, 0x3);
    patch_code_byte(ff7_externals.battle_sub_6E3135 + 0x16B, 0xCA);
}