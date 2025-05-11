/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2025 Julian Xhokaxhiu                                   //
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

#include <steamworkssdk/isteamutils.h>
#include <steamworkssdk/isteamuserstats.h>
#include <steamworkssdk/isteamuser.h>

#include <numeric>
#include <algorithm>

#include "achievement.h"
#include "log.h"
#include "cfg.h"

#define ach_trace(x, ...) if (trace_all || trace_achievement) ffnx_trace((x), ##__VA_ARGS__)

using std::string;

std::unique_ptr<SteamAchievementsFF7> g_FF7SteamAchievements = nullptr;
std::unique_ptr<SteamAchievementsFF8> g_FF8SteamAchievements = nullptr;

SteamManager::SteamManager(const achievement *achievements, int nAchievements, std::vector<std::string> statsNameVec):
  isInitialized(false),
  callbackUserStatsReceived(this, &SteamManager::OnUserStatsReceived),
  callbackUserStatsStored(this, &SteamManager::OnUserStatsStored),
  callbackAchievementStored(this, &SteamManager::OnAchievementStored)
{
    this->appID = SteamUtils()->GetAppID();
    this->nAchievements = nAchievements;
    for (std::string statName : statsNameVec) {
        this->stats.insert({statName, 0});
    }
    this->achievementList.insert(this->achievementList.end(), &achievements[0], &achievements[nAchievements]);
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
            ffnx_trace("%s - Achievement %s set, Store request sent to Steam\n", __func__, this->getStringAchievementID(achID));

        this->achievementList[achID].isAchieved = true;
        if (steam_achievements_debug_mode)
        {
            return SteamUserStats()->IndicateAchievementProgress(this->getStringAchievementID(achID), 99, 100);
        }
        else
        {
            SteamUserStats()->SetAchievement(this->getStringAchievementID(achID));
            return SteamUserStats()->StoreStats();
        }
    }

    if (trace_all || trace_achievement)
        ffnx_error("%s - Have not received a callback from Steam, thus, cannot send achievement\n", __func__);

    return false;
}

bool SteamManager::showAchievementProgress(int achID, int progressValue, int maxValue)
{
    if (this->isInitialized)
    {
        if (trace_all || trace_achievement)
            ffnx_trace("%s - Show achievement progress for achievement %s (%d/%d)\n", __func__, this->getStringAchievementID(achID), progressValue, maxValue);

        return SteamUserStats()->IndicateAchievementProgress(this->getStringAchievementID(achID), progressValue, maxValue);
    }

    if (trace_all || trace_achievement)
        ffnx_error("%s - Have not received a callback from Steam, thus, cannot show progress achievement\n", __func__);

    return false;
}

bool SteamManager::isAchieved(int achID)
{
    return this->achievementList[achID].isAchieved;
}

const char *SteamManager::getStringAchievementID(int achID)
{
    return this->achievementList[achID].chAchID;
}

std::optional<int> SteamManager::getUserStat(std::string statName) {
    if (!this->stats.contains(statName)) {
       return {};
    }
    return {this->stats.at(statName)};
}

bool SteamManager::updateUserStat(std::string statName, int value) {
    if (!this->stats.contains(statName)) {
       return false;
    }
    this->stats.insert_or_assign(statName, value);
    SteamUserStats()->SetStat(statName.c_str(), value);
    return SteamUserStats()->StoreStats();
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
 
            // load stats (assume all stats to be integers)
            for (auto statName: this->stats) {
                int statValue;
                if (SteamUserStats()->GetStat(statName.first.c_str(), &statValue)) {
                    this->stats.insert_or_assign(statName.first, statValue);
                }

                ach_trace("%s - user stat(%s, %d)\n", __func__, statName.first.c_str(), statValue);
            }

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
            ffnx_trace("%s - Stored Achievement %s for Steam\n", __func__, pCallback->m_rgchAchievementName);
    }
}

// -------------------------- STEAM ACHIEVEMENTS OF FF7 ---------------------------

SteamAchievementsFF7::SteamAchievementsFF7()
{
    this->steamManager = std::make_unique<SteamManager>(SteamAchievementsFF7::ACHIEVEMENTS, FF7_N_ACHIEVEMENTS);
    this->lastSeenMovieName = INVALID_MOVIE_NAME;
}

void SteamAchievementsFF7::initStatsFromSaveFile(const savemap &savemap)
{
    this->yuffieUnlocked = this->isYuffieUnlocked(savemap.yuffie_reg_mask);
    this->vincentUnlocked = this->isVincentUnlocked(savemap.vincent_reg_mask);
    for (int i = 0; i < N_GOLD_CHOCOBO_FIRST_SLOTS + N_GOLD_CHOCOBO_LAST_SLOTS; i++)
    {
        const chocobo_slot slot = (i < N_GOLD_CHOCOBO_FIRST_SLOTS) ? savemap.chocobo_slots_first[i] : savemap.chocobo_slots_last[i - N_GOLD_CHOCOBO_FIRST_SLOTS];
        this->isGoldChocoboSlot[i] = slot.type == GOLD_CHOCOBO_TYPE;
    }

    this->lastSeenMovieName = INVALID_MOVIE_NAME;
    this->initMateriaMastered(savemap);

    if (trace_all || trace_achievement)
    {
        ffnx_trace("%s - init stats from save file\n", __func__);
        ffnx_trace("yuffie unlocked: %s\n", this->yuffieUnlocked ? "true" : "false");
        ffnx_trace("vincent unlocked: %s\n", this->vincentUnlocked ? "true" : "false");
    }
}

void SteamAchievementsFF7::initCharStatsBeforeBattle(const savemap_char characters[])
{
    if (trace_all || trace_achievement)
        ffnx_trace("%s - initialization of character stats before battle\n", __func__);

    for (int i = 0; i < N_CHARACTERS; i++)
    {
        this->previousUsedLimitNumber[i] = characters[i].used_n_limit_1_1;
        for (int j = 0; j < N_EQUIP_MATERIA_PER_CHARACTER; j++)
        {
            uint32_t materia = characters[i].equipped_materia[j];
            this->equipMasteredMateriaCharacter[i][j] = isMateriaMastered(materia);
        }

        if (i == CAIT_SITH_INDEX)
            this->caitsithNumKills = characters[i].num_kills;
    }
}

void SteamAchievementsFF7::initMovieStats(const string movieName)
{
    if (trace_all || trace_achievement)
        ffnx_trace("%s - the movie name initialized is: %s\n", __func__, movieName.c_str());

    this->lastSeenMovieName = std::move(movieName);
}

void SteamAchievementsFF7::initMateriaMastered(const savemap &savemap)
{
    // Called only when loading a save file
    // Assumption: there is no mastered materia obtained through new character joining the team

    if (trace_all || trace_achievement)
        ffnx_trace("%s - init materia mastered\n", __func__);

    std::fill_n(this->masteredMateria.data(), N_TYPE_MATERIA, false);

    for (int i = 0; i < N_UNKNOWN_MATERIA; i++)
    {
        this->masteredMateria[this->unknownMateriaList[i]] = true;
    }

    for (int i = 0; i < N_MATERIA_SLOT; i++)
    {
        uint32_t materia = savemap.materia[i];
        if (this->isMateriaMastered(materia))
        {
            masteredMateria[materia & 0xFF] = true;
        }
    }
    if (this->isYuffieUnlocked(savemap.yuffie_reg_mask))
    {
        for (int i = 0; i < N_STOLEN_MATERIA_SLOT; i++)
        {
            uint32_t materia = savemap.stolen_materia[i];
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
            uint32_t materia = savemap.chars[i].equipped_materia[j];
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
    if (std::find(unmasterableMateriaList.begin(), unmasterableMateriaList.end(), materiaId) != unmasterableMateriaList.end())
        return true;
    return materiaId != MATERIA_EMPTY_SLOT && materiaAp == MATERIA_AP_MASTERED;
}

bool SteamAchievementsFF7::isAllMateriaMastered(const std::array<bool, N_TYPE_MATERIA> masteredMateriaList)
{
    bool allMastered = true;
    for (int i = 0; i < N_TYPE_MATERIA; i++)
    {
        if (!masteredMateriaList[i])
            return false;
    }
    return allMastered;
}

bool SteamAchievementsFF7::isYuffieUnlocked(char yuffieRegular)
{
    // took from https://github.com/sithlord48/ff7tk/blob/master/src/data/FF7Save.cpp#L4777
    return yuffieRegular & (1 << 0);
}

bool SteamAchievementsFF7::isVincentUnlocked(char vincentRegular)
{
    // took from https://github.com/sithlord48/ff7tk/blob/master/src/data/FF7Save.cpp#L4799
    return vincentRegular & (1 << 2);
}

bool SteamAchievementsFF7::isEndingMovie()
{
    return this->lastSeenMovieName == END_OF_GAME_MOVIE_NAME;
}

void SteamAchievementsFF7::unlockBattleWonAchievement(WORD formationID)
{
    if (trace_all || trace_achievement)
        ffnx_trace("%s - unlock achievement for winning battle (formation ID: %d)\n", __func__, formationID);

    if (!this->steamManager->isAchieved(WON_1ST_BATTLE))
        this->steamManager->setAchievement(WON_1ST_BATTLE);

    if (!this->steamManager->isAchieved(BEAT_DIAMOND_WEAPON) && formationID == DIAMOND_WEAPON_FORMATION_ID)
    {
        this->steamManager->setAchievement(BEAT_DIAMOND_WEAPON);
    }

    if (!this->steamManager->isAchieved(BEAT_RUBY_WEAPON)
        && find(begin(RUBY_WEAPON_FORMATION_ID), end(RUBY_WEAPON_FORMATION_ID), formationID) != end(RUBY_WEAPON_FORMATION_ID))
    {
        this->steamManager->setAchievement(BEAT_RUBY_WEAPON);
    }

    if (!this->steamManager->isAchieved(BEAT_EMERALD_WEAPON)
        && find(begin(EMERALD_WEAPON_FORMATION_ID), end(EMERALD_WEAPON_FORMATION_ID), formationID) != end(EMERALD_WEAPON_FORMATION_ID))
    {
        this->steamManager->setAchievement(BEAT_EMERALD_WEAPON);
    }

    if (!this->steamManager->isAchieved(BEAT_ULTIMATE_WEAPON) && formationID == ULTIMATE_WEAPON_FORMATION_ID)
    {
        this->steamManager->setAchievement(BEAT_ULTIMATE_WEAPON);
    }
}

void SteamAchievementsFF7::unlockGilAchievement(uint32_t gilAmount)
{
    if (trace_all || trace_achievement)
        ffnx_trace("%s - trying to unlock achievement for gil (amount: %d)\n", __func__, gilAmount);

    if (!this->steamManager->isAchieved(GET_99999999_GILS) && gilAmount >= GIL_ACHIEVEMENT_VALUE)
        this->steamManager->setAchievement(GET_99999999_GILS);
}

void SteamAchievementsFF7::unlockCharacterLevelAchievement(const savemap_char characters[])
{
    if (trace_all || trace_achievement)
        ffnx_trace("%s - trying to unlock achievement for character level\n", __func__);

    if (this->steamManager->isAchieved(GET_LEVEL_99_WITH_A_CHAR))
        return;

    for (int i = 0; i < N_CHARACTERS; i++)
    {
        if (characters[i].level == TOP_LEVEL_CHARACTER)
        {
            this->steamManager->setAchievement(GET_LEVEL_99_WITH_A_CHAR);
            return;
        }
    }
}

void SteamAchievementsFF7::unlockBattleSquareAchievement(WORD battleLocationID)
{
    if (trace_all || trace_achievement)
        ffnx_trace("%s - trying to unlock achievement for fighting in battle square (battle location id: 0x%04x)\n", __func__, battleLocationID);

    if (!this->steamManager->isAchieved(FIGHT_IN_BATTLE_SQUARE) && battleLocationID == BATTLE_SQUARE_LOCATION_ID)
    {
        this->steamManager->setAchievement(FIGHT_IN_BATTLE_SQUARE);
    }
}

void SteamAchievementsFF7::unlockGotMateriaAchievement(byte materiaID)
{
    using namespace std;
    if (trace_all || trace_achievement)
        ffnx_trace("%s - trying to unlock achievement for getting materia (got materia ID: 0x%02x)\n", __func__, materiaID);

    if (find(begin(unmasterableMateriaList), end(unmasterableMateriaList), materiaID) != end(unmasterableMateriaList))
    {
        int nMateriaMastered = accumulate(begin(masteredMateria), end(masteredMateria), 0);
        if (!this->masteredMateria[materiaID])
            this->steamManager->showAchievementProgress(MASTER_ALL_MATERIA, nMateriaMastered + 1 - sizeof(unknownMateriaList),
                                                        N_TYPE_MATERIA - sizeof(unknownMateriaList));
        this->masteredMateria[materiaID] = true;

        if (!this->steamManager->isAchieved(LEVEL_UP_MATERIA_LVL5))
            this->steamManager->setAchievement(LEVEL_UP_MATERIA_LVL5);

        if (!this->steamManager->isAchieved(MASTER_ALL_MATERIA) && this->isAllMateriaMastered(masteredMateria))
            this->steamManager->setAchievement(MASTER_ALL_MATERIA);
    }

    if (materiaID == BAHAMUT_ZERO_MATERIA_ID)
    {
        this->steamManager->setAchievement(GET_MATERIA_BAHAMUT_ZERO);
    }
    else if (materiaID == KOTR_MATERIA_ID)
    {
        this->steamManager->setAchievement(GET_MATERIA_KOTR);
    }
}

void SteamAchievementsFF7::unlockMasterMateriaAchievement(const savemap_char characters[])
{
    using namespace std;
    if (trace_all || trace_achievement)
        ffnx_trace("%s - trying to unlock achievement for mastering materia\n", __func__);

    int nMateriaMastered = accumulate(begin(masteredMateria), end(masteredMateria), 0);
    int nNewMateriaMastered = 0;
    for (int i = 0; i < N_CHARACTERS; i++)
    {
        if (characters[i].id == SEPHIROTH_ID || characters[i].id == YOUNG_CLOUD_ID)
            continue;

        for (int j = 0; j < N_EQUIP_MATERIA_PER_CHARACTER; j++)
        {
            uint32_t materia = characters[i].equipped_materia[j];
            if (this->isMateriaMastered(materia) && !this->equipMasteredMateriaCharacter[i][j])
            {
                if (trace_all || trace_achievement)
                    ffnx_trace("%s - trying to unlock achievement for mastering materia (materia id: 0x%02x)\n", __func__, materia & 0xFF);

                nNewMateriaMastered += (!this->masteredMateria[materia & 0xFF]) ? 1 : 0;
                this->masteredMateria[materia & 0xFF] = true;
                this->equipMasteredMateriaCharacter[i][j] = true;
                if (!this->steamManager->isAchieved(LEVEL_UP_MATERIA_LVL5))
                    this->steamManager->setAchievement(LEVEL_UP_MATERIA_LVL5);
            }
        }
    }
    if (nNewMateriaMastered > 0)
    {
        this->steamManager->showAchievementProgress(MASTER_ALL_MATERIA, nMateriaMastered + nNewMateriaMastered - sizeof(unknownMateriaList),
                                                    N_TYPE_MATERIA - sizeof(unknownMateriaList));
        if (!this->steamManager->isAchieved(MASTER_ALL_MATERIA) && this->isAllMateriaMastered(masteredMateria))
            this->steamManager->setAchievement(MASTER_ALL_MATERIA);
    }
}

void SteamAchievementsFF7::unlockFirstLimitBreakAchievement(short commandID, short actionID)
{
    if (trace_all || trace_achievement)
        ffnx_trace("%s - trying to unlock achievement for first limit break achievement(command_id: 0x%02x, action_id: 0x%02x)\n", __func__, commandID, actionID);

    if (commandID == LIMIT_COMMAND_INDEX)
    {
        auto item = find(firstLimitBreakActionID.begin(), firstLimitBreakActionID.end(), actionID);
        if (item != firstLimitBreakActionID.end())
        {
            if (!this->steamManager->isAchieved(USE_1ST_LIMIT_CLOUD + distance(firstLimitBreakActionID.cbegin(), item)))
            {
                this->steamManager->setAchievement(USE_1ST_LIMIT_CLOUD + distance(firstLimitBreakActionID.cbegin(), item));
            }
        }
    }
}

void SteamAchievementsFF7::unlockCaitSithLastLimitBreakAchievement(const savemap_char characters[])
{
    if (trace_all || trace_achievement)
        ffnx_trace("%s - trying to unlock achievement for cait sith last limit break achievement (num_kills: %d)\n", __func__, characters[CAIT_SITH_INDEX].num_kills);

    if (!this->steamManager->isAchieved(GET_FOURTH_CAITSITH_LAST_LIMIT) && this->caitsithNumKills >= 0 && this->caitsithNumKills < 40
        && characters[CAIT_SITH_INDEX].num_kills >= 40)
    {
        this->steamManager->setAchievement(GET_FOURTH_CAITSITH_LAST_LIMIT);
    }
    this->caitsithNumKills = -1;
}

void SteamAchievementsFF7::unlockLastLimitBreakAchievement(WORD usedItemID)
{
    using namespace std;
    if (trace_all || trace_achievement)
        ffnx_trace("%s - trying to unlock achievement for last limit break achievement (used item: 0x%07x)\n", __func__, usedItemID);

    auto item = find(limitBreakItemsID.begin(), limitBreakItemsID.end(), usedItemID);
    if (item != limitBreakItemsID.end())
    {
        if (!this->steamManager->isAchieved(GET_FOURTH_CLOUD_LAST_LIMIT + distance(limitBreakItemsID.cbegin(), item)))
        {
            this->steamManager->setAchievement(GET_FOURTH_CLOUD_LAST_LIMIT + distance(limitBreakItemsID.cbegin(), item));
        }
    }
}

void SteamAchievementsFF7::unlockGoldChocoboAchievement(const chocobo_slot firstFourSlots[], const chocobo_slot lastTwoSlots[])
{
    if (trace_all || trace_achievement)
        ffnx_trace("%s - trying to unlock gold chocobo achievement\n", __func__);

    if (this->steamManager->isAchieved(GET_GOLD_CHOCOBO))
        return;

    for (int i = 0; i < N_GOLD_CHOCOBO_FIRST_SLOTS + N_GOLD_CHOCOBO_LAST_SLOTS; i++)
    {
        const chocobo_slot slot = (i < N_GOLD_CHOCOBO_FIRST_SLOTS) ? firstFourSlots[i] : lastTwoSlots[i - N_GOLD_CHOCOBO_FIRST_SLOTS];
        if (!this->isGoldChocoboSlot[i] && slot.type == GOLD_CHOCOBO_TYPE)
        {
            this->steamManager->setAchievement(GET_GOLD_CHOCOBO);
        }
    }
}

void SteamAchievementsFF7::unlockGameProgressAchievement()
{
    if (trace_all || trace_achievement)
        ffnx_trace("%s - trying to unlock game progress achievement (movieName: %s)\n", __func__, this->lastSeenMovieName.c_str());

    if (!(this->steamManager->isAchieved(DEATH_OF_AERITH)) && this->lastSeenMovieName == DEATH_OF_AERITH_MOVIE_NAME)
        this->steamManager->setAchievement(DEATH_OF_AERITH);

    if (!(this->steamManager->isAchieved(SHINRA_ANNIHILATED)) && this->lastSeenMovieName == SHINRA_ANNIHILATED_MOVIE_NAME)
        this->steamManager->setAchievement(SHINRA_ANNIHILATED);

    if (!(this->steamManager->isAchieved(END_OF_GAME)) && this->lastSeenMovieName == END_OF_GAME_MOVIE_NAME)
        this->steamManager->setAchievement(END_OF_GAME);
}

void SteamAchievementsFF7::unlockYuffieAndVincentAchievement(unsigned char yuffieRegMask, unsigned char vincentRegMask)
{
    if (trace_all || trace_achievement)
        ffnx_trace("%s - trying to unlock yuffie and vincent achievement (yuffie_reg: 0x%02x, vincent_reg: 0x%02x)\n",
                   __func__, yuffieRegMask, vincentRegMask);

    if (!this->steamManager->isAchieved(GET_YUFFIE_IN_TEAM) && !this->yuffieUnlocked && this->isYuffieUnlocked(yuffieRegMask))
    {
        this->steamManager->setAchievement(GET_YUFFIE_IN_TEAM);
    }

    if (!this->steamManager->isAchieved(GET_VINCENT_IN_TEAM) && !this->vincentUnlocked && this->isVincentUnlocked(vincentRegMask))
    {
        this->steamManager->setAchievement(GET_VINCENT_IN_TEAM);
    }
}

// -------------------------- STEAM ACHIEVEMENTS OF FF8 ---------------------------

SteamAchievementsFF8::SteamAchievementsFF8()
{
    std::vector<std::string> statsNameVec = { ENEMY_KILLED_STAT_NAME, DRAW_MAGIC_STAT_NAME, STOCK_MAGIC_STAT_NAME, WON_CARDGAME_STAT_NAME };
    this->steamManager = std::make_unique<SteamManager>(SteamAchievementsFF8::ACHIEVEMENTS, FF8_N_ACHIEVEMENTS, statsNameVec);
}

void SteamAchievementsFF8::initOwnedTripleTriadRareCards(const savemap_ff8_triple_triad &tt_data)
{
    ach_trace("%s - init owned triple triad rare cards\n", __func__);

    for (int i = 0; i < this->prevOwnedRareCards.size(); i++) {
        bool owned = tt_data.card_locations[i] == SQUALL_CARD_LOCATION && (tt_data.cards_rare[i / 8] & (1 << i % 8)) > 0;
        this->prevOwnedRareCards[i] = owned;
    }
}

void SteamAchievementsFF8::initPreviousWeaponIdBeforeUpgrade(byte charId, byte weaponId)
{
    ach_trace("%s - init previous weapon id before upgrade (char id: %d, weapon id: %d)\n", __func__, charId, weaponId);
    this->prevWeaponUpgradeData.char_id = charId;
    this->prevWeaponUpgradeData.prev_weapon_id = weaponId;
}

void SteamAchievementsFF8::initStatCharIdUnderStatCompute(byte statCharId) {
    ach_trace("%s - init stat char id before max hp computation (char id: %d)\n", __func__, statCharId);
    this->statCharId = statCharId;
}

byte SteamAchievementsFF8::getStatCharIdUnderStatCompute() {
    return this->statCharId;
}

void SteamAchievementsFF8::unlockPlayTripleTriadAchievement()
{
    ach_trace("%s - trying to unlock play card game first time achievement\n", __func__);

    if (!(this->steamManager->isAchieved(CARDGAME_FIRST_TIME)))
        this->steamManager->setAchievement(CARDGAME_FIRST_TIME);
}

void SteamAchievementsFF8::unlockLoserTripleTriadAchievement(const savemap_ff8_triple_triad &tt_data)
{
    ach_trace("%s - trying to unlock loser card game achievement\n", __func__);

    for (int i = 0; i < this->prevOwnedRareCards.size(); i++) {
        bool owned = tt_data.card_locations[i] == SQUALL_CARD_LOCATION && (tt_data.cards_rare[i / 8] & (1 << i % 8)) > 0;
        if (!owned && this->prevOwnedRareCards[i]) {
            ach_trace("%s - LOSER achievement unlocked due to card id '%d' lost\n", __func__, i);

            if (!(this->steamManager->isAchieved(LOSER)))
                this->steamManager->setAchievement(LOSER);
        }
    }
}

void SteamAchievementsFF8::increaseCardWinsAndUnlockProfessionalAchievement()
{
    auto opt_wins = this->steamManager->getUserStat(WON_CARDGAME_STAT_NAME);
    if (!opt_wins.has_value()) {
        ffnx_error("%s - failed to get %s stat\n", __func__, WON_CARDGAME_STAT_NAME.c_str());
        return;
    }
    int new_wins = opt_wins.value() + 1;
    ach_trace("%s - trying to unlock professional player card game achievement (wins: %d)\n", __func__, new_wins);
    this->steamManager->updateUserStat(WON_CARDGAME_STAT_NAME, new_wins);

    if (new_wins >= 100)
    {
        if (!(this->steamManager->isAchieved(PROFESSIONAL)))
            this->steamManager->setAchievement(PROFESSIONAL);
    }
}

void SteamAchievementsFF8::unlockCollectorTripleTriadAchievement(const savemap_ff8_triple_triad &tt_data)
{
    ach_trace("%s - trying to unlock collector card game achievement\n", __func__);
    if (this->steamManager->isAchieved(COLLECT_ALL_CARDS)) {
      return;
    }

    int ownedNormalCards = 0;
    for (int i = 0; i < N_CARDS; i++) {
        bool owned = tt_data.cards[i] > 0x80;
        ownedNormalCards += owned;
    }

    int ownedRareCards = 0;
    for (int i = 0; i < N_RARE_CARDS; i++) {
        bool owned = tt_data.card_locations[i] == SQUALL_CARD_LOCATION && (tt_data.cards_rare[i / 8] & (1 << i % 8)) > 0;
        ownedRareCards += owned;
    }

    ach_trace("%s - collector report: normal cards %d/%d, rare cards %d/%d\n", __func__,
              ownedNormalCards, N_CARDS, ownedRareCards, N_RARE_CARDS);

    if (ownedNormalCards + ownedRareCards == N_CARDS + N_RARE_CARDS) {
        this->steamManager->setAchievement(COLLECT_ALL_CARDS);
    }
}

void SteamAchievementsFF8::unlockGuardianForceAchievement(int gf_idx)
{
    if (gf_idx < 0 || gf_idx > 15) {
      ach_trace("%s - invalid guardian force idx (gf id: %d)\n", __func__, gf_idx);
      return;
    }

    ach_trace("%s - trying to unlock guardian force achievement (gf id: %d)\n", __func__, gf_idx);
    if (!(this->steamManager->isAchieved(gfIndexToAchMap[gf_idx])))
        this->steamManager->setAchievement(gfIndexToAchMap[gf_idx]);
}

void SteamAchievementsFF8::unlockTopSeedRankAchievement(WORD seed_exp)
{
    ach_trace("%s - trying to unlock seed rank A achivement (seed exp: %d)\n", __func__, seed_exp);
    if (seed_exp >= MAX_SEED_EXP) {
        if (!(this->steamManager->isAchieved(REACH_SEED_RANK_A)))
            this->steamManager->setAchievement(REACH_SEED_RANK_A);
    }
}

void SteamAchievementsFF8::unlockUpgradeWeaponAchievement(const savemap_ff8 &savemap)
{
    if (this->prevWeaponUpgradeData.char_id == 0xFF || this->prevWeaponUpgradeData.prev_weapon_id == 0xFF) {
        ach_trace("%s - invalid previous weapon id before upgrade (char id: %d, weapon id: %d)\n",
                  __func__, this->prevWeaponUpgradeData.char_id, this->prevWeaponUpgradeData.prev_weapon_id);
    }

    byte weaponId = savemap.chars[this->prevWeaponUpgradeData.char_id].weapon_id;
    ach_trace("%s - trying to unlock handyman achivement (new weapon id: %d, old weapon id: %d)\n",
              __func__, weaponId, this->prevWeaponUpgradeData.prev_weapon_id);
    if (weaponId > this->prevWeaponUpgradeData.prev_weapon_id) {
        if (!(this->steamManager->isAchieved(UPGRADE_WEAPON_FIRST_TIME)))
            this->steamManager->setAchievement(UPGRADE_WEAPON_FIRST_TIME);
    }

    this->prevWeaponUpgradeData.char_id = 0xFF;
    this->prevWeaponUpgradeData.prev_weapon_id = 0xFF;
}

void SteamAchievementsFF8::unlockMaxHpAchievement(int max_hp)
{
    ach_trace("%s - trying to unlock maximum HP achivement (max hp: %d)\n", __func__, max_hp);

    if (max_hp >= MAX_HP)
    {
        if (!(this->steamManager->isAchieved(REACH_MAX_HP)))
            this->steamManager->setAchievement(REACH_MAX_HP);
    }
    this->statCharId = 0xFF;
}

void SteamAchievementsFF8::unlockMaxGilAchievement(uint32_t gil)
{
    ach_trace("%s - trying to unlock maximum gil achivement (gil: %d)\n", __func__, gil);

    if (gil >= MAX_GIL)
    {
        if (!(this->steamManager->isAchieved(REACH_MAX_GIL)))
            this->steamManager->setAchievement(REACH_MAX_GIL);
    }
}

void SteamAchievementsFF8::unlockTopLevelAchievement(int level)
{
    ach_trace("%s - trying to unlock top level achivement (level: %d)\n", __func__, level);

    if (level == MAX_LEVEL)
    {
        if (!(this->steamManager->isAchieved(REACH_LEVEL_100)))
            this->steamManager->setAchievement(REACH_LEVEL_100);
    }
}

void SteamAchievementsFF8::increaseKillsAndTryUnlockAchievement()
{
    auto opt_kills = this->steamManager->getUserStat(ENEMY_KILLED_STAT_NAME);
    if (!opt_kills.has_value()) {
        ffnx_error("%s - failed to get %s stat\n", __func__, ENEMY_KILLED_STAT_NAME.c_str());
        return;
    }
    
    int new_kills = opt_kills.value() + 1;
    ach_trace("%s - trying to unlock kills achivements (kills: %d)\n", __func__, new_kills);
    this->steamManager->updateUserStat(ENEMY_KILLED_STAT_NAME, new_kills);

    if (new_kills >= 100)
    {
        if (!(this->steamManager->isAchieved(TOTAL_KILLS_100)))
            this->steamManager->setAchievement(TOTAL_KILLS_100);
    }
    else if (new_kills % 10 == 0) {
        this->steamManager->showAchievementProgress(TOTAL_KILLS_100, new_kills, 100);
    }
    

    if (new_kills >= 1000)
    {
        if (!(this->steamManager->isAchieved(TOTAL_KILLS_1000)))
            this->steamManager->setAchievement(TOTAL_KILLS_1000);
    }
    else if (new_kills > 100 && new_kills % 100 == 0) {
        this->steamManager->showAchievementProgress(TOTAL_KILLS_1000, new_kills, 1000);
    }
    

    if (new_kills >= 10000)
    {
        if (!(this->steamManager->isAchieved(TOTAL_KILLS_10000)))
            this->steamManager->setAchievement(TOTAL_KILLS_10000);
    }
}

