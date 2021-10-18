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

#define N_MATERIA_SLOT 200
#define N_STOLEN_MATERIA_SLOT 48
#define N_EQUIP_MATERIA_PER_CHARACTER 16
#define BATTLE_SQUARE_LOCATION_ID 0x0025
#define FIRST_LIMIT_BREAK_CODE 0x0001
#define FOURTH_LIMIT_BREAK_CODE 0x0200
#define GIL_ACHIEVEMENT_VALUE 99999999
#define TOP_LEVEL_CHARACTER 99

#define YUFFIE_INDEX 5
#define CAIT_SITH_INDEX 6
#define VINCENT_INDEX 7
#define YOUNG_CLOUD_ID 0x09
#define SEPHIROTH_ID 0x0A

#define MATERIA_EMPTY_SLOT 0xFF
#define MATERIA_AP_MASTERED 0xFFFFFF
#define BAHAMUT_ZERO_MATERIA_ID 0x58
#define KOTR_MATERIA_ID 0x59

#define DIAMOND_WEAPON_START 980
#define DIAMOND_WEAPON_END 981
#define RUBY_WEAPON_START 982
#define RUBY_WEAPON_END 983
#define EMERALD_WEAPON_START 984
#define EMERALD_WEAPON_END 987
#define ULTIMATE_WEAPON_START 988
#define ULTIMATE_WEAPON_END 991

// gold chocobo from https://gamefaqs.gamespot.com/pc/130791-final-fantasy-vii/faqs/13970
#define GOLD_CHOCOBO_TYPE 0x04

#include "achievement.h"

SteamAchievementsFF7 g_FF7SteamAchievements;
SteamAchievementsFF8 g_FF8SteamAchievements;

void SteamAchievements::init(achievement *achievements, int nAchievements)
{
    this->appID = SteamUtils()->GetAppID();
    this->isInitialized = false;
    this->nAchievements = nAchievements;
    this->achievementList = achievements;
    this->requestStats();
    if (trace_all || trace_achievement)
        ffnx_trace("%s - Init steam achievements with appID: %d\n", __func__, this->appID);
}

bool SteamAchievements::requestStats()
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

bool SteamAchievements::setAchievement(int achID)
{
    if (this->isInitialized)
    {
        if (trace_all || trace_achievement)
            ffnx_trace("%s - Achievement %s set, Store request sent to Steam\n", __func__, this->achievementList[achID].chAchID);

        // TODO reactivate when done all;
        char buffer[128];
        _snprintf(buffer, 128, "Achievement unlocked %s\n", this->achievementList[achID].chAchID);
        //SteamUserStats()->SetAchievement(this->achievementList[achID].chAchID);
        // bool success = SteamUserStats()->StoreStats();
        if (true) //success)
            this->achievementList[achID].isAchieved = true;
        //return success;

        // TESTING PHASE OF STEAM ACHIEVEMENTS
        if (trace_all || trace_achievement)
            MessageBoxA(gameHwnd, buffer, "Testing achievement unlocked", 0);
        return true;
    }

    if (trace_all || trace_achievement)
        ffnx_error("%s - Have not received a callback from Steam, thus, cannot send achievement\n", __func__);

    return false;
}

void SteamAchievements::OnUserStatsReceived(UserStatsReceived_t *pCallback)
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

void SteamAchievements::OnUserStatsStored(UserStatsStored_t *pCallback)
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

void SteamAchievements::OnAchievementStored(UserAchievementStored_t *pCallback)
{
    if (this->appID == pCallback->m_nGameID)
    {
        if (trace_all || trace_achievement)
            ffnx_trace("%s - Stored Achievement for Steam\n", __func__);
    }
}

// -------------------------- STEAM ACHIEVEMENTS OF FF7 ---------------------------

void SteamAchievementsFF7::init(achievement *achievements, int nAchievements)
{
    SteamAchievements::init(achievements, nAchievements);

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

    for (int i = 0; i < N_STOLEN_MATERIA_SLOT; i++)
    {
        uint32_t materia = savemap->stolen_materia[i];
        if (this->isMateriaMastered(materia))
        {
            masteredMateria[materia & 0xFF] = true;
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

    if (!this->achievementList[WON_1ST_BATTLE].isAchieved)
        this->setAchievement(WON_1ST_BATTLE);

    if (battleSceneID >= DIAMOND_WEAPON_START && battleSceneID <= DIAMOND_WEAPON_END)
    {
        this->setAchievement(BEAT_DIAMOND_WEAPON);
    }

    if (battleSceneID >= RUBY_WEAPON_START && battleSceneID <= RUBY_WEAPON_END)
    {
        this->setAchievement(BEAT_RUBY_WEAPON);
    }

    if (battleSceneID >= EMERALD_WEAPON_START && battleSceneID <= EMERALD_WEAPON_END)
    {
        this->setAchievement(BEAT_EMERALD_WEAPON);
    }

    if (battleSceneID >= ULTIMATE_WEAPON_START && battleSceneID <= ULTIMATE_WEAPON_END)
    {
        this->setAchievement(BEAT_ULTIMATE_WEAPON);
    }
}

void SteamAchievementsFF7::unlockGilAchievement(uint32_t gilAmount)
{
    if (trace_all || trace_achievement)
        ffnx_trace("%s - trying to unlock achievement for gil (amount: %d)\n", __func__, gilAmount);

    if (!this->achievementList[GET_99999999_GILS].isAchieved && gilAmount >= GIL_ACHIEVEMENT_VALUE)
        this->setAchievement(GET_99999999_GILS);
}

void SteamAchievementsFF7::unlockCharacterLevelAchievement(savemap_char *characters)
{
    if (trace_all || trace_achievement)
        ffnx_trace("%s - trying to unlock achievement for character level\n", __func__);

    if (this->achievementList[GET_LEVEL_99_WITH_A_CHAR].isAchieved)
        return;

    for (int i = 0; i < N_CHARACTERS; i++)
    {
        if (characters[i].level == TOP_LEVEL_CHARACTER)
        {
            this->setAchievement(GET_LEVEL_99_WITH_A_CHAR);
            return;
        }
    }
}

void SteamAchievementsFF7::unlockBattleSquareAchievement(WORD battleLocationID)
{
    if (trace_all || trace_achievement)
        ffnx_trace("%s - trying to unlock achievement for fighting in battle square (battle location id: 0x%04x)\n", __func__, battleLocationID);

    if (!this->achievementList[FIGHT_IN_BATTLE_SQUARE].isAchieved && battleLocationID == BATTLE_SQUARE_LOCATION_ID)
    {
        this->setAchievement(FIGHT_IN_BATTLE_SQUARE);
    }
}

void SteamAchievementsFF7::unlockGotMateriaAchievement(byte materiaID)
{
    if (trace_all || trace_achievement)
        ffnx_trace("%s - trying to unlock achievement for getting materia (got materia ID: 0x%02x)\n", __func__, materiaID);

    if (materiaID == BAHAMUT_ZERO_MATERIA_ID)
    {
        this->setAchievement(GET_MATERIA_BAHAMUT_ZERO);
    }
    else if (materiaID == KOTR_MATERIA_ID)
    {
        this->setAchievement(GET_MATERIA_KOTR);
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

                if (!this->achievementList[LEVEL_UP_MATERIA_LVL5].isAchieved)
                    this->setAchievement(LEVEL_UP_MATERIA_LVL5);
            }
        }
    }

    if (!this->achievementList[MASTER_ALL_MATERIA].isAchieved && this->isAllMateriaMastered(masteredMateria))
        this->setAchievement(MASTER_ALL_MATERIA);
}

void SteamAchievementsFF7::unlockFirstLimitBreakAchievement(savemap_char *characters)
{
    if (trace_all || trace_achievement)
        ffnx_trace("%s - trying to unlock achievement for first limit break achievement\n", __func__);

    for (int i = 0; i < N_CHARACTERS; i++)
    {
        if (!this->achievementList[USE_1ST_LIMIT_CLOUD + this->indexToFirstLimitIndex[i]].isAchieved && characters[i].used_n_limit_1_1 > this->previousUsedLimitNumber[i])
        {
            this->setAchievement(USE_1ST_LIMIT_CLOUD + this->indexToFirstLimitIndex[i]);
        }
    }
}

void SteamAchievementsFF7::unlockCaitSithLastLimitBreakAchievement(savemap_char *characters)
{
    if (trace_all || trace_achievement)
        ffnx_trace("%s - trying to unlock achievement for cait sith last limit break achievement\n", __func__);

    if (!this->achievementList[GET_FOURTH_CAITSITH_LAST_LIMIT].isAchieved && characters[CAIT_SITH_INDEX].learned_limit_break > FIRST_LIMIT_BREAK_CODE)
    {
        this->setAchievement(GET_FOURTH_CAITSITH_LAST_LIMIT);
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

        if (!this->achievementList[GET_FOURTH_CLOUD_LAST_LIMIT + i].isAchieved && usedItemID == this->limitBreakItemsID[i])
        {
            this->setAchievement(GET_FOURTH_CLOUD_LAST_LIMIT + i);
        }
    }
}

void SteamAchievementsFF7::unlockGoldChocoboAchievement(chocobo_slot *firstFourSlots, chocobo_slot *lastTwoSlots)
{
    if (trace_all || trace_achievement)
        ffnx_trace("%s - trying to unlock gold chocobo achievement\n", __func__);

    if (this->achievementList[GET_GOLD_CHOCOBO].isAchieved)
        return;

    for (int i = 0; i < 4; i++)
    {
        if (firstFourSlots[i].type == GOLD_CHOCOBO_TYPE)
            this->setAchievement(GET_GOLD_CHOCOBO);
    }

    for (int i = 0; i < 2; i++)
    {
        if (lastTwoSlots[i].type == GOLD_CHOCOBO_TYPE)
            this->setAchievement(GET_GOLD_CHOCOBO);
    }
}

void SteamAchievementsFF7::unlockGameProgressAchievement(int achID)
{
    if (trace_all || trace_achievement)
        ffnx_trace("%s - trying to unlock game progress achievement (%s)\n", __func__, this->achievementList[achID].chAchID);

    if (!(this->achievementList[achID].isAchieved) && (achID >= DEATH_OF_AERITH && achID <= END_OF_GAME))
        this->setAchievement(achID);
}

void SteamAchievementsFF7::unlockYuffieAndVincentAchievement(WORD phsCharacterVisibility)
{
    if (trace_all || trace_achievement)
        ffnx_trace("%s - trying to unlock yuffie and vincent achievement (phs visibility: %d)\n", __func__, phsCharacterVisibility);

    if (!this->achievementList[GET_YUFFIE_IN_TEAM].isAchieved && ((phsCharacterVisibility & (WORD)(1 << YUFFIE_INDEX)) != 0))
    {
        this->setAchievement(GET_YUFFIE_IN_TEAM);
    }

    if (!this->achievementList[GET_VINCENT_IN_TEAM].isAchieved && ((phsCharacterVisibility & (WORD)(1 << VINCENT_INDEX)) != 0))
    {
        this->setAchievement(GET_VINCENT_IN_TEAM);
    }
}

// -------------------------- STEAM ACHIEVEMENTS OF FF8 ---------------------------