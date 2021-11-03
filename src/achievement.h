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

#include <steamworkssdk/steam_api_common.h>
#include <steamworkssdk/steam_api.h>
#include "log.h"

#define _ACH_ID(id)           \
    {                         \
        id, #id, "", "", 0, 0 \
    }

struct achievement
{
    int achID;
    const char *chAchID;
    char achName[128];
    char achDescription[256];
    bool isAchieved;
    int iconImage;
};

class SteamManager
{
private:
    int appID;                    // Our current AppID
    achievement *achievementList; // Achievements data
    int nAchievements;            // The number of Achievements
    bool isInitialized;           // Have we called Request stats and received the callback?

public:
    SteamManager() : appID(0),
                          isInitialized(false),
                          callbackUserStatsReceived(this, &SteamManager::OnUserStatsReceived),
                          callbackUserStatsStored(this, &SteamManager::OnUserStatsStored),
                          callbackAchievementStored(this, &SteamManager::OnAchievementStored) {}

    void init(achievement *achievements, int nAchievements);

    bool requestStats();
    bool setAchievement(int achID);
    bool isAchieved(int achID);
    const char* getStringAchievementID(int achID);

    STEAM_CALLBACK(SteamManager, OnUserStatsReceived, UserStatsReceived_t, callbackUserStatsReceived);
    STEAM_CALLBACK(SteamManager, OnUserStatsStored, UserStatsStored_t, callbackUserStatsStored);
    STEAM_CALLBACK(SteamManager, OnAchievementStored, UserAchievementStored_t, callbackAchievementStored);
};

class SteamAchievementsFF7 
{
private:
    static inline const int N_CHARACTERS = 9;
    static inline const int N_TYPE_MATERIA = 91;
    static inline const int N_UNKNOWN_MATERIA = 8;

    static inline const int N_MATERIA_SLOT = 200;
    static inline const int N_STOLEN_MATERIA_SLOT = 48;
    static inline const int N_EQUIP_MATERIA_PER_CHARACTER = 16;
    static inline const WORD BATTLE_SQUARE_LOCATION_ID = 0x0025;
    static inline const WORD FIRST_LIMIT_BREAK_CODE = 0x0001;
    static inline const WORD FOURTH_LIMIT_BREAK_CODE = 0x0200;
    static inline const int GIL_ACHIEVEMENT_VALUE = 99999999;
    static inline const int TOP_LEVEL_CHARACTER = 99;

    static inline const int YUFFIE_INDEX = 5;
    static inline const int CAIT_SITH_INDEX = 6;
    static inline const int VINCENT_INDEX = 7;
    static inline const byte YOUNG_CLOUD_ID = 0x09;
    static inline const byte SEPHIROTH_ID = 0x0A;

    static inline const byte MATERIA_EMPTY_SLOT = 0xFF;
    static inline const int MATERIA_AP_MASTERED = 0xFFFFFF;
    static inline const byte BAHAMUT_ZERO_MATERIA_ID = 0x58;
    static inline const byte KOTR_MATERIA_ID = 0x59;

    static inline const WORD DIAMOND_WEAPON_START = 980;
    static inline const WORD DIAMOND_WEAPON_END = 981;
    static inline const WORD RUBY_WEAPON_START = 982;
    static inline const WORD RUBY_WEAPON_END = 983;
    static inline const WORD EMERALD_WEAPON_START = 984;
    static inline const WORD EMERALD_WEAPON_END = 987;
    static inline const WORD ULTIMATE_WEAPON_START = 988;
    static inline const WORD ULTIMATE_WEAPON_END = 991;

    static inline const byte GOLD_CHOCOBO_TYPE = 0x04;

    SteamManager steamManager;

    std::vector<int> indexToFirstLimitIndex;
    std::vector<int> unknownMateriaList;
    std::vector<int> unmasterableMateriaList;
    std::vector<int> limitBreakItemsID;
    
    WORD previousUsedLimitNumber[N_CHARACTERS];
    bool equipMasteredMateriaCharacter[N_CHARACTERS][N_EQUIP_MATERIA_PER_CHARACTER];
    bool masteredMateria[N_TYPE_MATERIA];
    bool yuffieUnlocked;
    bool vincentUnlocked;
    bool caitsithLastLimitUnlocked;

    bool isYuffieUnlocked(char yuffieRegular);
    bool isVincentUnlocked(char vincentRegular);

public:
    void init();
    void initStatsFromSaveFile(savemap *savemap);
    void initCharStatsBeforeBattle(savemap_char *characters);
    void initMateriaMastered(savemap *savemap);
    bool isMateriaMastered(uint32_t materia);
    bool isAllMateriaMastered(bool* masteredMateria);
    void unlockBattleWonAchievement(WORD battleSceneID);
    void unlockGilAchievement(uint32_t gilAmount);
    void unlockCharacterLevelAchievement(savemap_char *characters);
    void unlockBattleSquareAchievement(WORD battle_location_id);
    void unlockGotMateriaAchievement(byte materia_id);
    void unlockMasterMateriaAchievement(savemap_char *characters);
    void unlockFirstLimitBreakAchievement(savemap_char *characters);
    void unlockLastLimitBreakAchievement(WORD item_id);
    void unlockCaitSithLastLimitBreakAchievement(savemap_char *characters);
    void unlockGoldChocoboAchievement(chocobo_slot *firstFourSlots, chocobo_slot *lastTwoSlots);
    void unlockGameProgressAchievement(int achID);
    void unlockYuffieAndVincentAchievement(savemap *savemap);
};

class SteamAchievementsFF8
{
public:
    void init() {}
};

enum Achievements
{
    DEATH_OF_AERITH = 0,
    SHINRA_ANNIHILATED = 1,
    END_OF_GAME = 2,
    GET_99999999_GILS = 3,
    GET_LEVEL_99_WITH_A_CHAR = 4,
    GET_FOURTH_CLOUD_LAST_LIMIT = 5,
    GET_FOURTH_BARRET_LAST_LIMIT = 6,
    GET_FOURTH_TIFA_LAST_LIMIT = 7,
    GET_FOURTH_AERITH_LAST_LIMIT = 8,
    GET_FOURTH_REDXIII_LAST_LIMIT = 9,
    GET_FOURTH_YUFFIE_LAST_LIMIT = 10,
    GET_FOURTH_CAITSITH_LAST_LIMIT = 11,
    GET_FOURTH_VINCENT_LAST_LIMIT = 12,
    GET_FOURTH_CID_LAST_LIMIT = 13,
    GET_MATERIA_KOTR = 14,
    LEVEL_UP_MATERIA_LVL5 = 15,
    GET_MATERIA_BAHAMUT_ZERO = 16,
    BEAT_ULTIMATE_WEAPON = 17,
    BEAT_DIAMOND_WEAPON = 18,
    BEAT_RUBY_WEAPON = 19,
    BEAT_EMERALD_WEAPON = 20,
    GET_VINCENT_IN_TEAM = 21,
    GET_YUFFIE_IN_TEAM = 22,
    MASTER_ALL_MATERIA = 23,
    GET_GOLD_CHOCOBO = 24,
    WON_1ST_BATTLE = 25,
    USE_1ST_LIMIT_CLOUD = 26,
    USE_1ST_LIMIT_BARRET = 27,
    USE_1ST_LIMIT_VINCENT = 28,
    USE_1ST_LIMIT_AERITH = 29,
    USE_1ST_LIMIT_CID = 30,
    USE_1ST_LIMIT_TIFA = 31,
    USE_1ST_LIMIT_YUFFIE = 32,
    USE_1ST_LIMIT_REDXIII = 33,
    USE_1ST_LIMIT_CAITSITH = 34,
    FIGHT_IN_BATTLE_SQUARE = 35
};

enum AchievementsFF8
{
    UNLOCK_GF_QUEZACOTL = 0,
    UNLOCK_GF_SHIVA = 1,
    UNLOCK_GF_IFRIT = 2,
    UNLOCK_GF_SIREN = 3,
    UNLOCK_GF_BROTHERS = 4,
    UNLOCK_GF_DIABLOS = 5,
    UNLOCK_GF_CARBUNCLE = 6,
    UNLOCK_GF_LEVIATHAN = 7,
    UNLOCK_GF_PANDEMONA = 8,
    UNLOCK_GF_CERBERUS = 9,
    UNLOCK_GF_ALEXANDER = 10,
    UNLOCK_GF_DOOMTRAIN = 11,
    UNLOCK_GF_BAHAMUT = 12,
    UNLOCK_GF_CACTUAR = 13,
    UNLOCK_GF_TONBERRY = 14,
    UNLOCK_GF_EDEN = 15,
    DRAW_100_MAGIC = 16,
    REACH_SEED_RANK_A = 17,
    CARDGAME_FIRST_TIME = 18,
    COLLECT_ALL_CARDS = 19,
    UPGRADE_WEAPON_FIRST_TIME = 20,
    REACH_MAX_HP = 21,
    REACH_MAX_GIL = 22,
    REACH_LEVEL_100 = 23,
    FINISH_THE_GAME = 24,
    FINISH_THE_GAME_INITIAL_LEVEL = 25,
    CAPTURE_CHOCOBO_FIRST_TIME = 26,
    FOUND_RAGNAROK = 27,
    SEED_FIRST_SALARY = 28,
    CHOCORPG_FIRST_ITEM = 29,
    BEAT_OMEGA_WEAPON = 30,
    OBEL_LAKE_SECRET = 31,
    UFO = 32,
    CARDS_CLUB_MASTER = 33,
    LOSER = 34,
    PROFESSIONAL = 35,
    TIMBER_MANIACS = 36,
    MAGAZINES_ADDICT = 37,
    MAGIC_FINDER = 38,
    CHICOBO_TOP_LEVEL = 39,
    BLUE_MAGICS = 40,
    DOG_TRICKS = 41,
    TOTAL_KILLS_100 = 42,
    TOTAL_KILLS_1000 = 43,
    TOTAL_KILLS_10000 = 44
};

achievement g_AchievementsFF7[] = {
    _ACH_ID(DEATH_OF_AERITH),
    _ACH_ID(SHINRA_ANNIHILATED),
    _ACH_ID(END_OF_GAME),
    _ACH_ID(GET_99999999_GILS),
    _ACH_ID(GET_LEVEL_99_WITH_A_CHAR),
    _ACH_ID(GET_FOURTH_CLOUD_LAST_LIMIT),
    _ACH_ID(GET_FOURTH_BARRET_LAST_LIMIT),
    _ACH_ID(GET_FOURTH_TIFA_LAST_LIMIT),
    _ACH_ID(GET_FOURTH_AERITH_LAST_LIMIT),
    _ACH_ID(GET_FOURTH_REDXIII_LAST_LIMIT),
    _ACH_ID(GET_FOURTH_YUFFIE_LAST_LIMIT),
    _ACH_ID(GET_FOURTH_CAITSITH_LAST_LIMIT),
    _ACH_ID(GET_FOURTH_VINCENT_LAST_LIMIT),
    _ACH_ID(GET_FOURTH_CID_LAST_LIMIT),
    _ACH_ID(GET_MATERIA_KOTR),
    _ACH_ID(LEVEL_UP_MATERIA_LVL5),
    _ACH_ID(GET_MATERIA_BAHAMUT_ZERO),
    _ACH_ID(BEAT_ULTIMATE_WEAPON),
    _ACH_ID(BEAT_DIAMOND_WEAPON),
    _ACH_ID(BEAT_RUBY_WEAPON),
    _ACH_ID(BEAT_EMERALD_WEAPON),
    _ACH_ID(GET_VINCENT_IN_TEAM),
    _ACH_ID(GET_YUFFIE_IN_TEAM),
    _ACH_ID(MASTER_ALL_MATERIA),
    _ACH_ID(GET_GOLD_CHOCOBO),
    _ACH_ID(WON_1ST_BATTLE),
    _ACH_ID(USE_1ST_LIMIT_CLOUD),
    _ACH_ID(USE_1ST_LIMIT_BARRET),
    _ACH_ID(USE_1ST_LIMIT_VINCENT),
    _ACH_ID(USE_1ST_LIMIT_AERITH),
    _ACH_ID(USE_1ST_LIMIT_CID),
    _ACH_ID(USE_1ST_LIMIT_TIFA),
    _ACH_ID(USE_1ST_LIMIT_YUFFIE),
    _ACH_ID(USE_1ST_LIMIT_REDXIII),
    _ACH_ID(USE_1ST_LIMIT_CAITSITH),
    _ACH_ID(FIGHT_IN_BATTLE_SQUARE)};

achievement g_AchievementsFF8[] = {
    _ACH_ID(UNLOCK_GF_QUEZACOTL),
    _ACH_ID(UNLOCK_GF_SHIVA),
    _ACH_ID(UNLOCK_GF_IFRIT),
    _ACH_ID(UNLOCK_GF_SIREN),
    _ACH_ID(UNLOCK_GF_BROTHERS),
    _ACH_ID(UNLOCK_GF_DIABLOS),
    _ACH_ID(UNLOCK_GF_CARBUNCLE),
    _ACH_ID(UNLOCK_GF_LEVIATHAN),
    _ACH_ID(UNLOCK_GF_PANDEMONA),
    _ACH_ID(UNLOCK_GF_CERBERUS),
    _ACH_ID(UNLOCK_GF_ALEXANDER),
    _ACH_ID(UNLOCK_GF_DOOMTRAIN),
    _ACH_ID(UNLOCK_GF_BAHAMUT),
    _ACH_ID(UNLOCK_GF_CACTUAR),
    _ACH_ID(UNLOCK_GF_TONBERRY),
    _ACH_ID(UNLOCK_GF_EDEN),
    _ACH_ID(DRAW_100_MAGIC),
    _ACH_ID(REACH_SEED_RANK_A),
    _ACH_ID(CARDGAME_FIRST_TIME),
    _ACH_ID(COLLECT_ALL_CARDS),
    _ACH_ID(UPGRADE_WEAPON_FIRST_TIME),
    _ACH_ID(REACH_MAX_HP),
    _ACH_ID(REACH_MAX_GIL),
    _ACH_ID(REACH_LEVEL_100),
    _ACH_ID(FINISH_THE_GAME),
    _ACH_ID(FINISH_THE_GAME_INITIAL_LEVEL),
    _ACH_ID(CAPTURE_CHOCOBO_FIRST_TIME),
    _ACH_ID(FOUND_RAGNAROK),
    _ACH_ID(SEED_FIRST_SALARY),
    _ACH_ID(CHOCORPG_FIRST_ITEM),
    _ACH_ID(BEAT_OMEGA_WEAPON),
    _ACH_ID(OBEL_LAKE_SECRET),
    _ACH_ID(UFO),
    _ACH_ID(CARDS_CLUB_MASTER),
    _ACH_ID(LOSER),
    _ACH_ID(PROFESSIONAL),
    _ACH_ID(TIMBER_MANIACS),
    _ACH_ID(MAGAZINES_ADDICT),
    _ACH_ID(MAGIC_FINDER),
    _ACH_ID(CHICOBO_TOP_LEVEL),
    _ACH_ID(BLUE_MAGICS),
    _ACH_ID(DOG_TRICKS),
    _ACH_ID(TOTAL_KILLS_100),
    _ACH_ID(TOTAL_KILLS_1000),
    _ACH_ID(TOTAL_KILLS_10000)};

// Global, access to Achievements object
extern SteamAchievementsFF7 g_FF7SteamAchievements;
extern SteamAchievementsFF8 g_FF8SteamAchievements;