/****************************************************************************/
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

#include <steamworkssdk/isteamutils.h>
#include <steamworkssdk/isteamuserstats.h>
#include <steamworkssdk/isteamuser.h>

#include "achievement.h"

SteamAchievementsFF7 g_FF7SteamAchievements;
SteamAchievementsFF8 g_FF8SteamAchievements;

void SteamManager::init(achievement *achievements, int nAchievements)
{
    this->appID = SteamUtils()->GetAppID();
    this->isInitialized = false;
    this->nAchievements = nAchievements;
    this->achievementList = achievements;
    this->requestStats();
    if (trace_all || trace_achievement)
        ffnx_trace("%s - Init steam achievements with appID: %d\n", __func__, this->appID);
}

bool SteamManager::requestStats()
{
    // Is Steam loaded?
    if (NULL == SteamUserStats() || NULL == SteamUser())
    {
        return false;
    }
    // Is the user logged on?
    if (!SteamUser()->BLoggedOn())
    {
        return false;
    }
    if (trace_all || trace_achievement)
        ffnx_trace("%s - Request user stats sent\n", __func__);

    return SteamUserStats()->RequestCurrentStats();
}

bool SteamManager::setAchievement(int achID)
{
    if (this->isInitialized)
    {
        if (trace_all || trace_achievement)
            ffnx_trace("%s - Achievement %s set, Store request sent to Steam\n", __func__, this->achievementList[achID].chAchID);

        // TODO reactivate when done all;
        char buffer[128];
        _snprintf(buffer, 128, "Achievement unlocked %s\n", this->achievementList[achID].chAchID);
        
        // RELEASE PHASE OF STEAM ACHIEVEMENTS
        //SteamUserStats()->SetAchievement(this->achievementList[achID].chAchID);
        // bool success = SteamUserStats()->StoreStats();
        //return success;

        // TESTING PHASE OF STEAM ACHIEVEMENTS
        this->achievementList[achID].isAchieved = true;
        bool success = SteamUserStats()->IndicateAchievementProgress(this->achievementList[achID].chAchID, 99, 100);
        return success;
    }

    if (trace_all || trace_achievement)
        ffnx_error("%s - Have not received a callback from Steam, thus, cannot send achievement\n", __func__);

    return false;
}

bool SteamManager::isAchieved(int achID){
    return this->achievementList[achID].isAchieved;
}

const char* SteamManager::getStringAchievementID(int achID){
    return this->achievementList[achID].chAchID;
}

void SteamManager::OnUserStatsReceived(UserStatsReceived_t *pCallback)
{
    if (this->appID == pCallback->m_nGameID)
    {
        if (k_EResultOK == pCallback->m_eResult)
        {
            if (trace_all || trace_achievement)
                ffnx_trace("%s - received stats and achievements from Steam\n", __func__);
            this->isInitialized = true;

            // load achievements
            for (int i = 0; i < this->nAchievements; ++i)
            {
                achievement &ach = this->achievementList[i];

                SteamUserStats()->GetAchievement(ach.chAchID, &ach.isAchieved);
                _snprintf(ach.achName, sizeof(ach.achName), "%s",
                          SteamUserStats()->GetAchievementDisplayAttribute(ach.chAchID,
                                                                           "name"));
                _snprintf(ach.achDescription, sizeof(ach.achDescription), "%s",
                          SteamUserStats()->GetAchievementDisplayAttribute(ach.chAchID,
                                                                           "desc"));
                
                // TESTING PHASE STEAM ACHIEVEMENTS, set all achievement at false
                this->achievementList[i].isAchieved = false;
                
                if (trace_all || trace_achievement)
                    ffnx_trace("%s - achievement data(%s, %s, %s)\n", __func__, ach.chAchID, ach.achDescription, ach.isAchieved ? "true" : "false");

                
            }
        }
        else
        {
            if (trace_all || trace_achievement)
                ffnx_trace("%s - RequestStats - failed, %d\n", __func__, pCallback->m_eResult);
        }
    }
}

void SteamManager::OnUserStatsStored(UserStatsStored_t *pCallback)
{
    if (this->appID == pCallback->m_nGameID)
    {
        if (k_EResultOK == pCallback->m_eResult)
        {
            if (trace_all || trace_achievement)
                ffnx_info("%s - success\n", __func__);
        }
        else
        {
            if (trace_all || trace_achievement)
                ffnx_trace("%s - failed, %d\n", __func__, pCallback->m_eResult);
        }
    }
}

void SteamManager::OnAchievementStored(UserAchievementStored_t *pCallback)
{
    if (this->appID == pCallback->m_nGameID)
    {
        if (trace_all || trace_achievement)
            ffnx_trace("%s - Stored Achievement for Steam\n", __func__);
    }
}

// -------------------------- STEAM ACHIEVEMENTS OF FF7 ---------------------------

void SteamAchievementsFF7::init()
{
    this->steamManager.init(g_AchievementsFF7, FF7_N_ACHIEVEMENTS);

    this->unknownMateriaList = {0x16, 0x26, 0x2D, 0x2E, 0x2F, 0x3F, 0x42, 0x43};
    this->unmasterableMateriaList = {0x11, 0x30, 0x49, 0x5A};
    this->indexToFirstLimitIndex = {0, 1, 5, 3, 7, 6, 8, 2, 4};
    this->limitBreakItemsID = {0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x1FF, 0x5D, 0x5E};
}

void SteamAchievementsFF7::initMateriaMastered(savemap *savemap)
{
    if (trace_all || trace_achievement)
        ffnx_trace("%s - init materia mastered\n", __func__);

    std::fill_n(this->masteredMateria, N_TYPE_MATERIA, false);

    for (int i = 0; i < N_UNKNOWN_MATERIA; i++)
    {
        this->masteredMateria[this->unknownMateriaList[i]] = true;
    }

    for (int i = 0; i < N_MATERIA_SLOT; i++)
    {
        uint32_t materia = savemap->materia[i];
        if (this->isMateriaMastered(materia))
        {
            masteredMateria[materia & 0xFF] = true;
        }
    }
    if(this->isYuffieUnlocked(savemap->yuffie_reg_mask)){
        for (int i = 0; i < N_STOLEN_MATERIA_SLOT; i++)
        {
            uint32_t materia = savemap->stolen_materia[i];
            if (this->isMateriaMastered(materia))
            {
                masteredMateria[materia & 0xFF] = true;
            }
        }
    }

    for (int i = 0; i < N_CHARACTERS; i++)
    {
        for (int j = 0; j < N_EQUIP_MATERIA_PER_CHARACTER; j++)
        {
            uint32_t materia = savemap->chars[i].equipped_materia[j];
            if (this->isMateriaMastered(materia))
            {
                masteredMateria[materia & 0xFF] = true;
            }
        }
    }
}

bool SteamAchievementsFF7::isMateriaMastered(uint32_t materia)
{
    byte materiaId = materia & 0xFF;
    uint32_t materiaAp = materia >> 8;
    if (std::find(this->unmasterableMateriaList.begin(), this->unmasterableMateriaList.end(), materiaId) != this->unmasterableMateriaList.end())
        return true;
    return materiaId != MATERIA_EMPTY_SLOT && materiaAp == MATERIA_AP_MASTERED;
}

bool SteamAchievementsFF7::isAllMateriaMastered(bool *masteredMateria)
{
    bool allTrue = true;
    for (int i = 0; i < N_TYPE_MATERIA; i++)
    {
        if (!masteredMateria[i])
            return false;
    }
    return allTrue;
}

bool SteamAchievementsFF7::isYuffieUnlocked(char yuffieRegular){
    // took from https://github.com/sithlord48/ff7tk/blob/master/src/data/FF7Save.cpp#L4777
    return yuffieRegular & (1 << 0);
}

bool SteamAchievementsFF7::isVincentUnlocked(char vincentRegular){
    // took from https://github.com/sithlord48/ff7tk/blob/master/src/data/FF7Save.cpp#L4799
    return vincentRegular & (1 << 2);
}

void SteamAchievementsFF7::setPreviousLimitUsedNumber(savemap_char *characters)
{
    for (int i = 0; i < N_CHARACTERS; i++)
    {
        this->previousUsedLimitNumber[i] = characters[i].used_n_limit_1_1;
    }
}

void SteamAchievementsFF7::unlockBattleWonAchievement(WORD battleSceneID)
{
    if (trace_all || trace_achievement)
        ffnx_trace("%s - unlock achievement for winning battle (scene ID: %d)\n", __func__, battleSceneID);

    if (!this->steamManager.isAchieved(WON_1ST_BATTLE))
        this->steamManager.setAchievement(WON_1ST_BATTLE);

    if (battleSceneID >= DIAMOND_WEAPON_START && battleSceneID <= DIAMOND_WEAPON_END)
    {
        this->steamManager.setAchievement(BEAT_DIAMOND_WEAPON);
    }

    if (battleSceneID >= RUBY_WEAPON_START && battleSceneID <= RUBY_WEAPON_END)
    {
        this->steamManager.setAchievement(BEAT_RUBY_WEAPON);
    }

    if (battleSceneID >= EMERALD_WEAPON_START && battleSceneID <= EMERALD_WEAPON_END)
    {
        this->steamManager.setAchievement(BEAT_EMERALD_WEAPON);
    }

    if (battleSceneID >= ULTIMATE_WEAPON_START && battleSceneID <= ULTIMATE_WEAPON_END)
    {
        this->steamManager.setAchievement(BEAT_ULTIMATE_WEAPON);
    }
}

void SteamAchievementsFF7::unlockGilAchievement(uint32_t gilAmount)
{
    if (trace_all || trace_achievement)
        ffnx_trace("%s - trying to unlock achievement for gil (amount: %d)\n", __func__, gilAmount);

    if (!this->steamManager.isAchieved(GET_99999999_GILS) && gilAmount >= GIL_ACHIEVEMENT_VALUE)
        this->steamManager.setAchievement(GET_99999999_GILS);
}

void SteamAchievementsFF7::unlockCharacterLevelAchievement(savemap_char *characters)
{
    if (trace_all || trace_achievement)
        ffnx_trace("%s - trying to unlock achievement for character level\n", __func__);

    if (this->steamManager.isAchieved(GET_LEVEL_99_WITH_A_CHAR))
        return;

    for (int i = 0; i < N_CHARACTERS; i++)
    {
        if (characters[i].level == TOP_LEVEL_CHARACTER)
        {
            this->steamManager.setAchievement(GET_LEVEL_99_WITH_A_CHAR);
            return;
        }
    }
}

void SteamAchievementsFF7::unlockBattleSquareAchievement(WORD battleLocationID)
{
    if (trace_all || trace_achievement)
        ffnx_trace("%s - trying to unlock achievement for fighting in battle square (battle location id: 0x%04x)\n", __func__, battleLocationID);

    if (!this->steamManager.isAchieved(FIGHT_IN_BATTLE_SQUARE) && battleLocationID == BATTLE_SQUARE_LOCATION_ID)
    {
        this->steamManager.setAchievement(FIGHT_IN_BATTLE_SQUARE);
    }
}

void SteamAchievementsFF7::unlockGotMateriaAchievement(byte materiaID)
{
    if (trace_all || trace_achievement)
        ffnx_trace("%s - trying to unlock achievement for getting materia (got materia ID: 0x%02x)\n", __func__, materiaID);

    if(std::find(this->unmasterableMateriaList.begin(), this->unmasterableMateriaList.end(), materiaID) != this->unmasterableMateriaList.end()){
        if(!this->steamManager.isAchieved(LEVEL_UP_MATERIA_LVL5))
            this->steamManager.setAchievement(LEVEL_UP_MATERIA_LVL5);

        if (!this->steamManager.isAchieved(MASTER_ALL_MATERIA) && this->isAllMateriaMastered(masteredMateria))
            this->steamManager.setAchievement(MASTER_ALL_MATERIA);
    }

    if (materiaID == BAHAMUT_ZERO_MATERIA_ID)
    {
        this->steamManager.setAchievement(GET_MATERIA_BAHAMUT_ZERO);
    }
    else if (materiaID == KOTR_MATERIA_ID)
    {
        this->steamManager.setAchievement(GET_MATERIA_KOTR);
    }
}

void SteamAchievementsFF7::unlockMasterMateriaAchievement(savemap_char *characters)
{
    if (trace_all || trace_achievement)
        ffnx_trace("%s - trying to unlock achievement for mastering materia\n", __func__);

    for (int i = 0; i < N_CHARACTERS; i++)
    {
        if (characters[i].id == SEPHIROTH_ID || characters[i].id == YOUNG_CLOUD_ID)
            continue;

        for (int j = 0; j < N_EQUIP_MATERIA_PER_CHARACTER; j++)
        {
            uint32_t materia = characters[i].equipped_materia[j];
            if (this->isMateriaMastered(materia))
            {
                if (trace_all || trace_achievement)
                    ffnx_trace("%s - trying to unlock achievement for mastering materia (materia id: 0x%02x)\n", __func__, materia & 0xFF);
                
                masteredMateria[materia & 0xFF] = true;
                if (!this->steamManager.isAchieved(LEVEL_UP_MATERIA_LVL5))
                    this->steamManager.setAchievement(LEVEL_UP_MATERIA_LVL5);
            }
        }
    }

    if (!this->steamManager.isAchieved(MASTER_ALL_MATERIA) && this->isAllMateriaMastered(masteredMateria))
        this->steamManager.setAchievement(MASTER_ALL_MATERIA);
}

void SteamAchievementsFF7::unlockFirstLimitBreakAchievement(savemap_char *characters)
{
    if (trace_all || trace_achievement)
        ffnx_trace("%s - trying to unlock achievement for first limit break achievement\n", __func__);

    for (int i = 0; i < N_CHARACTERS; i++)
    {
        if (!this->steamManager.isAchieved(USE_1ST_LIMIT_CLOUD + this->indexToFirstLimitIndex[i]) && characters[i].used_n_limit_1_1 > this->previousUsedLimitNumber[i])
        {
            this->steamManager.setAchievement(USE_1ST_LIMIT_CLOUD + this->indexToFirstLimitIndex[i]);
        }
    }
}

void SteamAchievementsFF7::unlockCaitSithLastLimitBreakAchievement(savemap_char *characters)
{
    if (trace_all || trace_achievement)
        ffnx_trace("%s - trying to unlock achievement for cait sith last limit break achievement\n", __func__);

    if (!this->steamManager.isAchieved(GET_FOURTH_CAITSITH_LAST_LIMIT) && characters[CAIT_SITH_INDEX].learned_limit_break > FIRST_LIMIT_BREAK_CODE)
    {
        this->steamManager.setAchievement(GET_FOURTH_CAITSITH_LAST_LIMIT);
    }
}

void SteamAchievementsFF7::unlockLastLimitBreakAchievement(WORD usedItemID)
{
    if (trace_all || trace_achievement)
        ffnx_trace("%s - trying to unlock achievement for last limit break achievement (used item: 0x%07x)\n", __func__, usedItemID);

    for (int i = 0; i < N_CHARACTERS; i++)
    {
        if (i == CAIT_SITH_INDEX) //excluding cait sith
            continue;

        if (!this->steamManager.isAchieved(GET_FOURTH_CLOUD_LAST_LIMIT + i) && usedItemID == this->limitBreakItemsID[i])
        {
            this->steamManager.setAchievement(GET_FOURTH_CLOUD_LAST_LIMIT + i);
        }
    }
}

void SteamAchievementsFF7::unlockGoldChocoboAchievement(chocobo_slot *firstFourSlots, chocobo_slot *lastTwoSlots)
{
    if (trace_all || trace_achievement)
        ffnx_trace("%s - trying to unlock gold chocobo achievement\n", __func__);

    if (this->steamManager.isAchieved(GET_GOLD_CHOCOBO))
        return;

    for (int i = 0; i < 4; i++)
    {
        if (firstFourSlots[i].type == GOLD_CHOCOBO_TYPE)
            this->steamManager.setAchievement(GET_GOLD_CHOCOBO);
    }

    for (int i = 0; i < 2; i++)
    {
        if (lastTwoSlots[i].type == GOLD_CHOCOBO_TYPE)
            this->steamManager.setAchievement(GET_GOLD_CHOCOBO);
    }
}

void SteamAchievementsFF7::unlockGameProgressAchievement(int achID)
{
    if (trace_all || trace_achievement)
        ffnx_trace("%s - trying to unlock game progress achievement (%s)\n", __func__, this->steamManager.getStringAchievementID(achID));

    if (!(this->steamManager.isAchieved(achID)) && (achID >= DEATH_OF_AERITH && achID <= END_OF_GAME))
        this->steamManager.setAchievement(achID);
}

void SteamAchievementsFF7::unlockYuffieAndVincentAchievement(savemap *savemap)
{
    if (trace_all || trace_achievement)
        ffnx_trace("%s - trying to unlock yuffie and vincent achievement (yuffie_reg: 0x%02x, vincent_reg: 0x%02x, phs_visi: %d)\n", \
                    __func__, savemap->yuffie_reg_mask, savemap->vincent_reg_mask, savemap->phs_visi2);

    // old method with phs ((phsCharacterVisibility & (WORD)(1 << YUFFIE_INDEX)) != 0) [didn't tested]
    if (!this->steamManager.isAchieved(GET_YUFFIE_IN_TEAM) && isYuffieUnlocked(savemap->yuffie_reg_mask))
        this->steamManager.setAchievement(GET_YUFFIE_IN_TEAM);

    if (!this->steamManager.isAchieved(GET_VINCENT_IN_TEAM) && isVincentUnlocked(savemap->vincent_reg_mask))
        this->steamManager.setAchievement(GET_VINCENT_IN_TEAM);
}

// -------------------------- STEAM ACHIEVEMENTS OF FF8 ---------------------------