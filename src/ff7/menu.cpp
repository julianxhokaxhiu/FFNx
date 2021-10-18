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

#include "../ff7.h"
#include "../log.h"
#include "../achievement.h"

void ff7_menu_battle_end_sub_6C9543()
{
    ((void (*)())ff7_externals.menu_battle_end_sub_6C9543)();

    if(*ff7_externals.menu_battle_end_mode == 1){
        if (trace_all || trace_achievement)
            ffnx_trace("%s - trying to unlock achievement for first limit, cait sith, character level, and master materia\n", __func__);
        // this could be achieved when the limit break is used, instead in this way it is achieved only after battle ending
        g_FF7SteamAchievements.unlockFirstLimitBreakAchievement(ff7_externals.savemap->chars);

        g_FF7SteamAchievements.unlockCaitSithLastLimitBreakAchievement(ff7_externals.savemap->chars);
        g_FF7SteamAchievements.unlockCharacterLevelAchievement(ff7_externals.savemap->chars);
        g_FF7SteamAchievements.unlockMasterMateriaAchievement(ff7_externals.savemap->chars);
    }
    if(*ff7_externals.menu_battle_end_mode == 3){
        if (trace_all || trace_achievement)
            ffnx_trace("%s - trying to unlock achievement for gil\n", __func__);
        g_FF7SteamAchievements.unlockGilAchievement(ff7_externals.savemap->gil);
    }
}

int ff7_get_materia_gil(uint32_t materia)
{
    int materiaGil = ((int (*)(uint32_t))ff7_externals.get_materia_gil)(materia);

    if (trace_all || trace_achievement)
        ffnx_trace("%s - trying to unlock achievement for gil\n", __func__);

    g_FF7SteamAchievements.unlockGilAchievement(ff7_externals.savemap->gil + materiaGil);
    return materiaGil;
}

void ff7_menu_sub_6CBCB9(int gilObtained)
{ // NOT TESTED
    if (ff7_externals.savemap->gil + gilObtained < ff7_externals.savemap->gil)
        ff7_externals.savemap->gil = -1;
    else
        ff7_externals.savemap->gil = ff7_externals.savemap->gil + gilObtained;

    if (trace_all || trace_achievement)
        ffnx_trace("%s - trying to unlock achievement for gil\n", __func__);

    g_FF7SteamAchievements.unlockGilAchievement(ff7_externals.savemap->gil);
};

byte ff7_menu_sub_6CBCF3(uint32_t materia_id)
{
    byte returnValue = ((byte(*)(uint32_t))ff7_externals.menu_sub_6CBCF3)(materia_id);

    if (trace_all || trace_achievement)
        ffnx_trace("%s - trying to unlock achievement for getting materia\n", __func__);

    g_FF7SteamAchievements.unlockGotMateriaAchievement(materia_id);

    return returnValue;
}

void ff7_menu_sub_6CC17F(uint32_t materia)
{
    ((void (*)(uint32_t))ff7_externals.menu_sub_6CC17F)(materia);

    if (trace_all || trace_achievement)
        ffnx_trace("%s - trying to unlock achievement for getting materia\n", __func__);

    g_FF7SteamAchievements.unlockGotMateriaAchievement(materia & 0xFF);
}

uint32_t ff7_menu_decrease_item_quantity(uint32_t item_used)
{
    uint32_t item_id;
    uint32_t prevIndex;
    uint32_t item_quantity;
    WORD local_c;
    int index=0;

    WORD *party_item_slots = ff7_externals.savemap->items;
    item_id = item_used & 0x1FF;
    item_quantity = (item_used & 0xFFFF) >> 9;
    prevIndex = item_id;
    
    while (!(((WORD)party_item_slots[index] != -1) &&
            (prevIndex = (WORD)party_item_slots[index] & 0x1ff, item_id == prevIndex)))
    {
        if (index >= 320)
            return prevIndex & 0xFFFF0000 | 0xFFFF;
        
        prevIndex = index;
        index = index + 1;
    }
    if (item_quantity < (party_item_slots[index] >> 9))
    {
        local_c = (WORD)(item_used | (item_quantity << 9));
        item_id = ((party_item_slots[index] >> 9) - item_quantity) * 512 | item_id;
        party_item_slots[index] = (WORD)item_id;
    }
    else
    {
        local_c = party_item_slots[index];
        item_id = 0;
        party_item_slots[index] = 0xFFFF;

        g_FF7SteamAchievements.unlockLastLimitBreakAchievement(item_used & 0x1FF);
    }
    return item_id & 0xFFFF0000 | (uint32_t)local_c;
}

// Called when finished writing a name of a chocobo/characters
void ff7_menu_sub_6CDC09(DWORD param_1){ // TO BE TESTED if chocobo is put in farm before naming or after naming
    ((void (*)(DWORD))ff7_externals.menu_sub_6CDC09)(param_1);

    g_FF7SteamAchievements.unlockGoldChocoboAchievement(ff7_externals.savemap->chocobo_slots_first, ff7_externals.savemap->chocobo_slots_last);
}