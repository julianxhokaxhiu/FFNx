/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2026 Julian Xhokaxhiu                                   //
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
#include <unordered_set>

#include "achievement.h"
#include "log.h"
#include "cfg.h"

#define ach_trace(x, ...) if (trace_all || trace_achievement) ffnx_trace((x), ##__VA_ARGS__)
#define get_ach_id(flag_2013, ach_2013, ach_re_release) (flag_2013) ? std::static_cast<int>(ach_2013) : std::static_cast<int>(ach_re_release)

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
    ach_trace("%s - Init steam achievements with appID: %d\n", __func__, this->appID);
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
    ach_trace("%s - Request user stats sent\n", __func__);

    return SteamUserStats()->RequestCurrentStats();
}

bool SteamManager::setAchievement(int achID)
{
    if (this->isInitialized)
    {
        if (this->isAchieved(achID)) {
            ach_trace("%s - Achievement %s already achieved, skip sending request to Steam\n", __func__, this->getStringAchievementID(achID));
            return true;
        }

        ach_trace("%s - Achievement %s set, Store request sent to Steam\n", __func__, this->getStringAchievementID(achID));

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
        ach_trace("%s - Show achievement progress for achievement %s (%d/%d)\n", __func__, this->getStringAchievementID(achID), progressValue, maxValue);

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

std::optional<int> SteamManager::getUserStat(const std::string &statName) {
    if (!this->stats.contains(statName)) {
       return {};
    }
    return {this->stats.at(statName)};
}

bool SteamManager::updateUserStat(const std::string &statName, int value) {
    if (!this->stats.contains(statName)) {
       return false;
    }
    ach_trace("%s - Updating steam user stat '%s' to %d\n", __func__, statName.c_str(), value);
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
            ach_trace("%s - received stats and achievements from Steam\n", __func__);
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

                ach_trace("%s - achievement data(%s, %s, %s)\n", __func__, ach.chAchID, ach.achDescription, ach.isAchieved ? "true" : "false");
            }
        }
        else
        {
            ach_trace("%s - RequestStats - failed, %d\n", __func__, pCallback->m_eResult);
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
            ach_trace("%s - failed, %d\n", __func__, pCallback->m_eResult);
        }
    }
}

void SteamManager::OnAchievementStored(UserAchievementStored_t *pCallback)
{
    if (this->appID == pCallback->m_nGameID)
    {
        ach_trace("%s - Stored Achievement %s for Steam\n", __func__, pCallback->m_rgchAchievementName);
    }
}

// -------------------------- STEAM ACHIEVEMENTS OF FF7 ---------------------------

SteamAchievementsFF7::SteamAchievementsFF7(boolean isFF72013Release)
{
    this->isFF72013Release = isFF72013Release;
    if (isFF72013Release) {
        this->steamManager = std::make_unique<SteamManager>(SteamAchievementsFF7::ACHIEVEMENTS_2013, std::size(SteamAchievementsFF7::ACHIEVEMENTS_2013));
    } else {
        this->steamManager = std::make_unique<SteamManager>(SteamAchievementsFF7::ACHIEVEMENTS_RE_RELEASE, std::size(SteamAchievementsFF7::ACHIEVEMENTS_RE_RELEASE));
    }
    this->lastSeenMovieName = INVALID_MOVIE_NAME;
}

int SteamAchievementsFF7::getAchievementIdByVersion(Achievements2013 ach_2013, AchievementsReRelease ach_rerelease)
{
    if (this->isFF72013Release) {
        return ach_2013;
    } else {
        return ach_rerelease;
    }
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
    ach_trace("%s - initialization of character stats before battle\n", __func__);

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
    ach_trace("%s - the movie name initialized is: %s\n", __func__, movieName.c_str());

    this->lastSeenMovieName = std::move(movieName);
}

void SteamAchievementsFF7::initMateriaMastered(const savemap &savemap)
{
    // Called only when loading a save file
    // Assumption: there is no mastered materia obtained through new character joining the team

    ach_trace("%s - init materia mastered\n", __func__);

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
    ach_trace("%s - unlock achievement for winning battle (formation ID: %d)\n", __func__, formationID);
    int won_1st_battle_ach = getAchievementIdByVersion(WON_1ST_BATTLE, A01_FirstBattle);
    int beat_diamond_weapon_ach = getAchievementIdByVersion(BEAT_DIAMOND_WEAPON, A29_DiamondWeapon);
    int beat_ruby_weapon_ach = getAchievementIdByVersion(BEAT_RUBY_WEAPON, A30_RubyWeapon);
    int beat_emerald_weapon_ach = getAchievementIdByVersion(BEAT_EMERALD_WEAPON, A31_EmeraldWeapon);

    this->steamManager->setAchievement(won_1st_battle_ach);

    if (formationID == DIAMOND_WEAPON_FORMATION_ID)
    {
        this->steamManager->setAchievement(beat_diamond_weapon_ach);
    }

    if (find(begin(RUBY_WEAPON_FORMATION_ID), end(RUBY_WEAPON_FORMATION_ID), formationID) != end(RUBY_WEAPON_FORMATION_ID))
    {
        this->steamManager->setAchievement(beat_ruby_weapon_ach);
    }

    if (find(begin(EMERALD_WEAPON_FORMATION_ID), end(EMERALD_WEAPON_FORMATION_ID), formationID) != end(EMERALD_WEAPON_FORMATION_ID))
    {
        this->steamManager->setAchievement(beat_emerald_weapon_ach);
    }

    if (this->isFF72013Release) {
        if (formationID == ULTIMATE_WEAPON_FORMATION_ID)
        {
            this->steamManager->setAchievement(BEAT_ULTIMATE_WEAPON);
        }
    }
}

void SteamAchievementsFF7::unlockGilAchievement(uint32_t gilAmount)
{
    ach_trace("%s - trying to unlock achievement for gil (amount: %d)\n", __func__, gilAmount);
    int gil_ach = getAchievementIdByVersion(GET_99999999_GILS, A14_MasterOfGil);

    if (gilAmount >= GIL_ACHIEVEMENT_VALUE)
        this->steamManager->setAchievement(gil_ach);
}

void SteamAchievementsFF7::unlockCharacterLevelAchievement(const savemap_char characters[])
{
    ach_trace("%s - trying to unlock achievement for character level\n", __func__);
    int char_lvl_ach = getAchievementIdByVersion(GET_LEVEL_99_WITH_A_CHAR, A15_TopLevel);

    if (this->steamManager->isAchieved(char_lvl_ach))
        return;

    for (int i = 0; i < N_CHARACTERS; i++)
    {
        if (characters[i].level == TOP_LEVEL_CHARACTER)
        {
            this->steamManager->setAchievement(char_lvl_ach);
            return;
        }
    }
}

void SteamAchievementsFF7::unlockGotMateriaAchievement(byte materiaID)
{
    using namespace std;
    ach_trace("%s - trying to unlock achievement for getting materia (got materia ID: 0x%02x)\n", __func__, materiaID);
    int lvl_up_materia_ach = getAchievementIdByVersion(LEVEL_UP_MATERIA_LVL5, A05_MasterMateria);

    if (find(begin(unmasterableMateriaList), end(unmasterableMateriaList), materiaID) != end(unmasterableMateriaList))
    {
        this->steamManager->setAchievement(lvl_up_materia_ach);

        int nMateriaMastered = accumulate(begin(masteredMateria), end(masteredMateria), 0);
        boolean wasNotMastered = !this->masteredMateria[materiaID];
        this->masteredMateria[materiaID] = true;

        if (this->isFF72013Release) {
            if (wasNotMastered)
                this->steamManager->showAchievementProgress(MASTER_ALL_MATERIA, nMateriaMastered + 1 - sizeof(unknownMateriaList),
                                                            N_TYPE_MATERIA - sizeof(unknownMateriaList));

            if (!this->steamManager->isAchieved(MASTER_ALL_MATERIA) && this->isAllMateriaMastered(masteredMateria))
                this->steamManager->setAchievement(MASTER_ALL_MATERIA);
        }
    }

    if (materiaID == BAHAMUT_ZERO_MATERIA_ID)
    {
        this->steamManager->setAchievement(getAchievementIdByVersion(GET_MATERIA_KOTR, A17_KnightsOfTheRound));
    }
    else if (materiaID == KOTR_MATERIA_ID)
    {
        this->steamManager->setAchievement(getAchievementIdByVersion(GET_MATERIA_BAHAMUT_ZERO, A16_BahamutZero));
    }

    if (!this->isFF72013Release) {
        if (materiaID == BAHAMUT_MATERIA_ID) {
            this->steamManager->setAchievement(A12_Bahamut);
        } else if (materiaID == LEVIATHAN_MATERIA_ID) {
            this->steamManager->setAchievement(A11_Leviathan);
        } else if (materiaID == SUMMON_MATERIA_ID) {
            this->steamManager->setAchievement(A28_MasterInvocation);
        }
    }
}

void SteamAchievementsFF7::unlockMasterMateriaAchievement(const savemap_char characters[])
{
    using namespace std;
    ach_trace("%s - trying to unlock achievement for mastering materia\n", __func__);
    int lvl_up_materia_ach = getAchievementIdByVersion(LEVEL_UP_MATERIA_LVL5, A05_MasterMateria);

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
                ach_trace("%s - trying to unlock achievement for mastering materia (materia id: 0x%02x)\n", __func__, materia & 0xFF);

                nNewMateriaMastered += (!this->masteredMateria[materia & 0xFF]) ? 1 : 0;
                this->masteredMateria[materia & 0xFF] = true;
                this->equipMasteredMateriaCharacter[i][j] = true;
                this->steamManager->setAchievement(lvl_up_materia_ach);
            }
        }
    }
    if (this->isFF72013Release && nNewMateriaMastered > 0)
    {
        this->steamManager->showAchievementProgress(MASTER_ALL_MATERIA, nMateriaMastered + nNewMateriaMastered - sizeof(unknownMateriaList),
                                                    N_TYPE_MATERIA - sizeof(unknownMateriaList));
        if (!this->steamManager->isAchieved(MASTER_ALL_MATERIA) && this->isAllMateriaMastered(masteredMateria))
            this->steamManager->setAchievement(MASTER_ALL_MATERIA);
    }
}

void SteamAchievementsFF7::unlockAchievementByBattleCommandAndAction(short commandID, short actionID)
{
    ach_trace("%s - trying to unlock achievement by battle command and action (command_id: 0x%02x, action_id: 0x%02x)\n", __func__, commandID, actionID);

    if (commandID == LIMIT_COMMAND_INDEX)
    {
        auto item = find(firstLimitBreakActionID.begin(), firstLimitBreakActionID.end(), actionID);
        if (item != firstLimitBreakActionID.end())
        {
            if (this->isFF72013Release) {
                this->steamManager->setAchievement(USE_1ST_LIMIT_CLOUD + distance(firstLimitBreakActionID.cbegin(), item));
            } else {
                this->steamManager->setAchievement(A02_Braver);
            }
        }
    }

    if (!this->isFF72013Release) {
        if (commandID == SUMMON_COMMAND_INDEX) {
            this->steamManager->setAchievement(A04_Summon);
        }
    }
}

void SteamAchievementsFF7::unlockCaitSithLastLimitBreakAchievement(const savemap_char characters[])
{
    ach_trace("%s - trying to unlock achievement for cait sith last limit break achievement (num_kills: %d)\n", __func__, characters[CAIT_SITH_INDEX].num_kills);
    int cait_sith_last_limit_ach = getAchievementIdByVersion(GET_FOURTH_CAITSITH_LAST_LIMIT, A24_Slots);

    if (this->caitsithNumKills >= 0 && this->caitsithNumKills < 40 && characters[CAIT_SITH_INDEX].num_kills >= 40) {
        this->steamManager->setAchievement(cait_sith_last_limit_ach);
    }
    this->caitsithNumKills = -1;
}

void SteamAchievementsFF7::unlockLastLimitBreakAchievement(WORD usedItemID)
{
    using namespace std;
    ach_trace("%s - trying to unlock achievement for last limit break achievement (used item: 0x%07x)\n", __func__, usedItemID);
    int cloud_last_limit_ach = getAchievementIdByVersion(GET_FOURTH_CLOUD_LAST_LIMIT, A18_Omnislash);

    auto item = find(limitBreakItemsID.begin(), limitBreakItemsID.end(), usedItemID);
    if (item != limitBreakItemsID.end()) {
        this->steamManager->setAchievement(cloud_last_limit_ach + distance(limitBreakItemsID.cbegin(), item));
    }
}

void SteamAchievementsFF7::unlockGameProgressAchievement()
{
    ach_trace("%s - trying to unlock game progress achievement (movieName: %s)\n", __func__, this->lastSeenMovieName.c_str());

    if (this->isFF72013Release) {
        if (this->lastSeenMovieName == DEATH_OF_AERITH_MOVIE_NAME)
            this->steamManager->setAchievement(DEATH_OF_AERITH);

        if (this->lastSeenMovieName == SHINRA_ANNIHILATED_MOVIE_NAME)
            this->steamManager->setAchievement(SHINRA_ANNIHILATED);
    }

    if (this->lastSeenMovieName == END_OF_GAME_MOVIE_NAME)
        this->steamManager->setAchievement(getAchievementIdByVersion(END_OF_GAME, A27_FinishFF7));
}

void SteamAchievementsFF7::unlockYuffieAndVincentAchievement(unsigned char yuffieRegMask, unsigned char vincentRegMask)
{
    ach_trace("%s - trying to unlock yuffie and vincent achievement (yuffie_reg: 0x%02x, vincent_reg: 0x%02x)\n",
                   __func__, yuffieRegMask, vincentRegMask);
    int yuffie_ach = getAchievementIdByVersion(GET_YUFFIE_IN_TEAM, A09_Yuffie);
    int vincent_ach = getAchievementIdByVersion(GET_VINCENT_IN_TEAM, A10_Vincent);

    if (!this->steamManager->isAchieved(yuffie_ach) && !this->yuffieUnlocked && this->isYuffieUnlocked(yuffieRegMask))
    {
        this->steamManager->setAchievement(yuffie_ach);
    }

    if (!this->steamManager->isAchieved(vincent_ach) && !this->vincentUnlocked && this->isVincentUnlocked(vincentRegMask))
    {
        this->steamManager->setAchievement(vincent_ach);
    }
}

// Achievements only in 2013 edition
void SteamAchievementsFF7::unlockGoldChocoboAchievement(const chocobo_slot firstFourSlots[], const chocobo_slot lastTwoSlots[])
{
    ach_trace("%s - trying to unlock gold chocobo achievement\n", __func__);
    if (!this->isFF72013Release)
        return;

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

void SteamAchievementsFF7::unlockBattleSquareAchievement(WORD battleLocationID)
{
    if (this->isFF72013Release) {
        ach_trace("%s - trying to unlock achievement for fighting in battle square (battle location id: 0x%04x)\n", __func__, battleLocationID);
        if (battleLocationID == BATTLE_SQUARE_LOCATION_ID)
        {
            this->steamManager->setAchievement(FIGHT_IN_BATTLE_SQUARE);
        }
    }
}

// Achievements only in 2026 edition
void SteamAchievementsFF7::unlockFallInBattleAchievement()
{
    if (!this->isFF72013Release) {
        ach_trace("%s - trying to unlock fall in battle achievement\n", __func__);
        this->steamManager->setAchievement(A13_FaillureIsAnOption);
    }
}

void SteamAchievementsFF7::unlockAchievementByDialogEvent(WORD fieldMapId, int textId)
{
    if (!this->isFF72013Release) {
        ach_trace("%s - trying to unlock achievement by dialog event text (field_id: %d, text_id: %d)\n", __func__, fieldMapId, textId);

        // Don Corneo choosing Cloud
        if (fieldMapId == 210 && textId == 11) {
            this->steamManager->setAchievement(A03_DonCorneo);
        }
        // Go on a date with Barret in the Gold Saucer
        else if (fieldMapId == 489 && textId == 56) {
            this->steamManager->setAchievement(A06_Bromance);
        }
    }
}

void SteamAchievementsFF7::unlockWinChocoboMinigameAchievement(int racePosition)
{
    if (!this->isFF72013Release) {
        ach_trace("%s - trying to unlock win chocobo minigame achievement (race position: %d)\n", __func__, racePosition);

        // 0 is first position
        if (racePosition == 0) {
            this->steamManager->setAchievement(A07_Chocochampion);
        }
    }
}

void SteamAchievementsFF7::unlockBikeHighscoreAchievement(int score)
{
    if (!this->isFF72013Release) {
        ach_trace("%s - trying to unlock bike highscore achievement (score: %d)\n", __func__, score);

        if (score >= 10050) {
            this->steamManager->setAchievement(A08_CorelAngel);
        }
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
    this->increaseUserStatAndTryUnlockAchievement(PROFESSIONAL, WON_CARDGAME_STAT_NAME, 100);
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
    this->steamManager->setAchievement(gfIndexToAchMap[gf_idx]);
}

void SteamAchievementsFF8::unlockTopSeedRankAchievement(WORD seed_exp)
{
    ach_trace("%s - trying to unlock seed rank A achivement (seed exp: %d)\n", __func__, seed_exp);
    if (seed_exp >= MAX_SEED_EXP) {
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
        this->steamManager->setAchievement(UPGRADE_WEAPON_FIRST_TIME);
    }

    this->prevWeaponUpgradeData.char_id = 0xFF;
    this->prevWeaponUpgradeData.prev_weapon_id = 0xFF;
}

void SteamAchievementsFF8::unlockMaxHpAchievement(int max_hp)
{
    ach_trace("%s - trying to unlock maximum HP achivement (max hp: %d)\n", __func__, max_hp);

    if (max_hp >= MAX_HP) {
        this->steamManager->setAchievement(REACH_MAX_HP);
    }
    this->statCharId = 0xFF;
}

void SteamAchievementsFF8::unlockMaxGilAchievement(uint32_t gil)
{
    ach_trace("%s - trying to unlock maximum gil achivement (gil: %d)\n", __func__, gil);

    if (gil >= MAX_GIL) {
        this->steamManager->setAchievement(REACH_MAX_GIL);
    }
}

void SteamAchievementsFF8::unlockTopLevelAchievement(int level)
{
    ach_trace("%s - trying to unlock top level achivement (level: %d)\n", __func__, level);

    if (level == MAX_LEVEL) {
        this->steamManager->setAchievement(REACH_LEVEL_100);
    }
}

void SteamAchievementsFF8::increaseKillsAndTryUnlockAchievement()
{
    if (this->steamManager->isAchieved(TOTAL_KILLS_10000))
    {
        return;
    }

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
        this->steamManager->setAchievement(TOTAL_KILLS_100);
    }
    else if (new_kills % 10 == 0) {
        this->steamManager->showAchievementProgress(TOTAL_KILLS_100, new_kills, 100);
    }

    if (new_kills >= 1000)
    {
        this->steamManager->setAchievement(TOTAL_KILLS_1000);
    }
    else if (new_kills > 100 && new_kills % 100 == 0) {
        this->steamManager->showAchievementProgress(TOTAL_KILLS_1000, new_kills, 1000);
    }

    if (new_kills >= 10000)
    {
        this->steamManager->setAchievement(TOTAL_KILLS_10000);
    }
}

void SteamAchievementsFF8::increaseMagicStockAndTryUnlockAchievement()
{
    this->increaseUserStatAndTryUnlockAchievement(DRAW_100_MAGIC, STOCK_MAGIC_STAT_NAME, 100, true);
}

void SteamAchievementsFF8::increaseMagicDrawsAndTryUnlockAchievement()
{
    this->increaseUserStatAndTryUnlockAchievement(MAGIC_FINDER, DRAW_MAGIC_STAT_NAME, 100, true);
}

void SteamAchievementsFF8::unlockTimberManiacsAchievement(WORD timber_maniacs_bitmap)
{
    ach_trace("%s - trying to unlock timber maniacs achivement (timber maniacs: 0x%x)\n", __func__, timber_maniacs_bitmap);
    if ((timber_maniacs_bitmap & 0x3FFF) == 0x3FFE || (timber_maniacs_bitmap & 0x3FFF) == 0x3FFD) {
        this->steamManager->setAchievement(TIMBER_MANIACS);
    }
}

void SteamAchievementsFF8::unlockFirstSalaryAchievement()
{
    ach_trace("%s - trying to unlock first salary achivement\n", __func__);

    this->steamManager->setAchievement(SEED_FIRST_SALARY);
}

void SteamAchievementsFF8::unlockQuistisLimitBreaksAchievement(WORD quistis_lb_bitmap)
{
    ach_trace("%s - trying to unlock quistis limit breaks achivement (quistis lb: 0x%x)\n", __func__, quistis_lb_bitmap);

    if (quistis_lb_bitmap == 0xFFFF) {
        this->steamManager->setAchievement(BLUE_MAGICS);
    }
}

void SteamAchievementsFF8::unlockRinoaLimitBreaksAchievement(byte rinoa_completed_lb)
{
    ach_trace("%s - trying to unlock rinoa limit breaks achivement (completed lb: 0x%x)\n", __func__, rinoa_completed_lb);

    if (rinoa_completed_lb == 0xFF) {
        this->steamManager->setAchievement(DOG_TRICKS);
    }
}

void SteamAchievementsFF8::unlockOmegaDestroyedAchievement()
{
    ach_trace("%s - trying to unlock omega destroyed achivement\n", __func__);

    this->steamManager->setAchievement(BEAT_OMEGA_WEAPON);
}

void SteamAchievementsFF8::unlockPupuQuestAchievement(byte pupu_encounter_bitmap)
{
    ach_trace("%s - trying to unlock UFO achivement (pupu encounter var: 0x%x)\n", __func__, pupu_encounter_bitmap);

    if ((pupu_encounter_bitmap & 0xFC) == 0xFC) {
        this->steamManager->setAchievement(UFO);
    }
}

void SteamAchievementsFF8::unlockChocoLootAchievement()
{
    ach_trace("%s - trying to unlock choco loot achivement\n", __func__);

    this->steamManager->setAchievement(CHOCORPG_FIRST_ITEM);
}

void SteamAchievementsFF8::unlockTopLevelBokoAchievement(byte boko_lvl)
{
    ach_trace("%s - trying to unlock top level boko achivement (boko lvl: %d)\n", __func__, boko_lvl);

    if (boko_lvl >= 100) {
        this->steamManager->setAchievement(CHICOBO_TOP_LEVEL);
    }
}

void SteamAchievementsFF8::unlockChocoboAchievement()
{
    ach_trace("%s - trying to unlock chocobo achivement\n", __func__);

    this->steamManager->setAchievement(CAPTURE_CHOCOBO_FIRST_TIME);
}

void SteamAchievementsFF8::unlockCardClubMasterAchievement(const savemap_ff8_field &savemap_field)
{
    bool cc_king_unlocked = savemap_field.tt_cc_quest_1 & 0b10000;
    ach_trace("%s - trying to unlock card club master achivement (cc king unlocked: %d)\n", __func__, cc_king_unlocked);

    if (cc_king_unlocked) {
        this->steamManager->setAchievement(CARDS_CLUB_MASTER);
    }
}

void SteamAchievementsFF8::unlockObelLakeQuestAchievement()
{
    ach_trace("%s - trying to unlock obel lake quest achivement\n", __func__);

    this->steamManager->setAchievement(OBEL_LAKE_SECRET);
}

void SteamAchievementsFF8::unlockRagnarokAchievement()
{
    ach_trace("%s - trying to unlock ragnarok achivement\n", __func__);

    this->steamManager->setAchievement(FOUND_RAGNAROK);
}

void SteamAchievementsFF8::unlockEndOfGameAchievement(int squall_lvl)
{
    ach_trace("%s - trying to unlock end of game achivement (squall_lvl: %d)\n", __func__, squall_lvl);

    this->steamManager->setAchievement(FINISH_THE_GAME);

    if (squall_lvl == 7) {
        this->steamManager->setAchievement(FINISH_THE_GAME_INITIAL_LEVEL);
    }
}

void SteamAchievementsFF8::unlockMagazineAddictAchievement(const savemap_ff8_items &items)
{
    std::unordered_set<uint8_t> magazines_found = {};
    for (int i = 0; i < ITEM_SLOTS; i++) {
        if (itemIsMagazine(items.items[i].item_id) && items.items[i].item_quantity > 0) {
            magazines_found.insert(items.items[i].item_id);
        }
    }
    ach_trace("%s - trying to unlock magazine addict achivement (magazines found: %d)\n", __func__, magazines_found.size());
 
    if (magazines_found.size() >= MAGAZINES_TO_COLLECT) {
        this->steamManager->setAchievement(MAGAZINES_ADDICT);
    }
}

bool SteamAchievementsFF8::itemIsMagazine(uint8_t item_id) {
    return item_id >= 0xB1 && item_id <= 0xC6;
}

// Private methods
void SteamAchievementsFF8::increaseUserStatAndTryUnlockAchievement(Achievements achId, const std::string &statName, int achValue, bool showAchievementProgress)
{
    if (this->steamManager->isAchieved(achId))
    {
        return;
    }

    auto opt_stat_value = this->steamManager->getUserStat(statName);
    if (!opt_stat_value.has_value()) {
        ffnx_error("%s - failed to get %s stat\n", __func__, statName.c_str());
        return;
    }

    int new_stat_value = opt_stat_value.value() + 1;
    ach_trace("%s - trying to unlock %s achivement (stat value: %d)\n", __func__, this->ACHIEVEMENTS[achId].chAchID, new_stat_value);
    this->steamManager->updateUserStat(statName, new_stat_value);

    if (new_stat_value >= achValue)
    {
        this->steamManager->setAchievement(achId);
    }
    else if (showAchievementProgress && new_stat_value % (achValue / 10) == 0)
    {
        this->steamManager->showAchievementProgress(achId, new_stat_value, achValue);
    }
}

