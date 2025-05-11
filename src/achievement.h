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

#pragma once

#include <memory>
#include <optional>
#include <steamworkssdk/steam_api.h>
#include <array>
#include <unordered_map>
#include <vector>
#include <string>
#include <windows.h>

#include "ff7.h"
#include "ff8/save_data.h"

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
    int appID;                                          // Our current AppID
    std::vector<achievement> achievementList;           // Achievements data
    int nAchievements;                                  // The number of Achievements
    bool isInitialized;                                 // Have we called Request stats and received the callback?
    std::unordered_map<std::string, int> stats;         // Steam stats used by the game

public:
    SteamManager(const achievement *achievements, int nAchievements, std::vector<std::string> statsNameVec = {});
    ~SteamManager() = default;

    bool requestStats();
    bool showAchievementProgress(int achID, int progressValue, int maxValue);
    bool setAchievement(int achID);
    bool isAchieved(int achID);
    const char *getStringAchievementID(int achID);
    std::optional<int> getUserStat(std::string statName);
    bool updateUserStat(std::string statName, int value);

    STEAM_CALLBACK(SteamManager, OnUserStatsReceived, UserStatsReceived_t, callbackUserStatsReceived);
    STEAM_CALLBACK(SteamManager, OnUserStatsStored, UserStatsStored_t, callbackUserStatsStored);
    STEAM_CALLBACK(SteamManager, OnAchievementStored, UserAchievementStored_t, callbackAchievementStored);
};

class SteamAchievementsFF7
{
private:
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
        USE_1ST_LIMIT_TIFA = 28,
        USE_1ST_LIMIT_AERITH = 29,
        USE_1ST_LIMIT_REDXIII = 30,
        USE_1ST_LIMIT_YUFFIE = 31,
        USE_1ST_LIMIT_CAITSITH = 32,
        USE_1ST_LIMIT_VINCENT = 33,
        USE_1ST_LIMIT_CID = 34,
        FIGHT_IN_BATTLE_SQUARE = 35
    };

    static inline const achievement ACHIEVEMENTS[] = {
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
        _ACH_ID(USE_1ST_LIMIT_TIFA),
        _ACH_ID(USE_1ST_LIMIT_AERITH),
        _ACH_ID(USE_1ST_LIMIT_REDXIII),
        _ACH_ID(USE_1ST_LIMIT_YUFFIE),
        _ACH_ID(USE_1ST_LIMIT_CAITSITH),
        _ACH_ID(USE_1ST_LIMIT_VINCENT),
        _ACH_ID(USE_1ST_LIMIT_CID),
        _ACH_ID(FIGHT_IN_BATTLE_SQUARE),
    };

    static inline constexpr int FF7_N_ACHIEVEMENTS = 36;

    static inline constexpr int N_CHARACTERS = 9;
    static inline constexpr int N_TYPE_MATERIA = 91;
    static inline constexpr int N_UNKNOWN_MATERIA = 8;

    static inline constexpr int N_MATERIA_SLOT = 200;
    static inline constexpr int N_STOLEN_MATERIA_SLOT = 48;
    static inline constexpr int N_EQUIP_MATERIA_PER_CHARACTER = 16;
    static inline constexpr int GIL_ACHIEVEMENT_VALUE = 99999999;
    static inline constexpr int TOP_LEVEL_CHARACTER = 99;

    static inline constexpr int YUFFIE_INDEX = 5;
    static inline constexpr int CAIT_SITH_INDEX = 6;
    static inline constexpr int VINCENT_INDEX = 7;
    static inline constexpr byte YOUNG_CLOUD_ID = 0x09;
    static inline constexpr byte SEPHIROTH_ID = 0x0A;

    static inline constexpr byte MATERIA_EMPTY_SLOT = 0xFF;
    static inline constexpr int MATERIA_AP_MASTERED = 0xFFFFFF;
    static inline constexpr byte BAHAMUT_ZERO_MATERIA_ID = 0x58;
    static inline constexpr byte KOTR_MATERIA_ID = 0x59;

    // took from here https://finalfantasy.fandom.com/wiki/Diamond_Weapon_(Final_Fantasy_VII_boss)#Formations
    static inline constexpr WORD DIAMOND_WEAPON_FORMATION_ID = 980;
    static inline constexpr std::array<WORD, 2> RUBY_WEAPON_FORMATION_ID = {982, 983};
    static inline constexpr std::array<WORD, 3> EMERALD_WEAPON_FORMATION_ID = {984, 985, 986};
    static inline constexpr WORD ULTIMATE_WEAPON_FORMATION_ID = 287;
    static inline constexpr WORD BATTLE_SQUARE_LOCATION_ID = 0x0025;

    static inline constexpr byte GOLD_CHOCOBO_TYPE = 0x04;
    static inline constexpr int N_GOLD_CHOCOBO_FIRST_SLOTS = 4;
    static inline constexpr int N_GOLD_CHOCOBO_LAST_SLOTS = 2;

    static inline const std::string DEATH_OF_AERITH_MOVIE_NAME = "funeral";
    static inline const std::string SHINRA_ANNIHILATED_MOVIE_NAME = "hwindjet";
    static inline const std::string END_OF_GAME_MOVIE_NAME = "ending2";
    static inline const std::string INVALID_MOVIE_NAME = "";

    static inline constexpr std::array<byte, N_UNKNOWN_MATERIA> unknownMateriaList = {0x16, 0x26, 0x2D, 0x2E, 0x2F, 0x3F, 0x42, 0x43};
    static inline constexpr std::array<byte, 4> unmasterableMateriaList = {0x11, 0x30, 0x49, 0x5A};

    static inline constexpr byte LIMIT_COMMAND_INDEX = 0x14;
    static inline constexpr std::array<byte, N_CHARACTERS> firstLimitBreakActionID = {0x00, 0x07, 0x62, 0x0E, 0x23, 0x31, 0x2A, 0x2D, 0x1C};
    static inline constexpr std::array<WORD, N_CHARACTERS> limitBreakItemsID = {0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0xFFFF, 0x5D, 0x5E};

    std::unique_ptr<SteamManager> steamManager;

    std::array<WORD, N_CHARACTERS> previousUsedLimitNumber;
    std::array<std::array<bool, N_EQUIP_MATERIA_PER_CHARACTER>, N_CHARACTERS> equipMasteredMateriaCharacter;
    std::array<bool, N_TYPE_MATERIA> masteredMateria;
    std::array<bool, N_GOLD_CHOCOBO_FIRST_SLOTS + N_GOLD_CHOCOBO_LAST_SLOTS> isGoldChocoboSlot;
    bool yuffieUnlocked;
    bool vincentUnlocked;
    int caitsithNumKills;
    std::string lastSeenMovieName;

    bool isYuffieUnlocked(char yuffieRegular);
    bool isVincentUnlocked(char vincentRegular);
    void initMateriaMastered(const savemap &savemap);
    bool isMateriaMastered(uint32_t materia);
    bool isAllMateriaMastered(const std::array<bool, N_TYPE_MATERIA> masteredMateriaList);

public:
    SteamAchievementsFF7();
    ~SteamAchievementsFF7() = default;

    void initStatsFromSaveFile(const savemap &savemap);
    void initCharStatsBeforeBattle(const savemap_char characters[]);
    void initMovieStats(const std::string movieName);

    bool isEndingMovie();

    void unlockBattleWonAchievement(WORD formationID);
    void unlockGilAchievement(uint32_t gilAmount);
    void unlockCharacterLevelAchievement(const savemap_char characters[]);
    void unlockBattleSquareAchievement(WORD battleLocationID);
    void unlockGotMateriaAchievement(byte materiaID);
    void unlockMasterMateriaAchievement(const savemap_char characters[]);
    void unlockFirstLimitBreakAchievement(short characterIndex, short actionIndex);
    void unlockLastLimitBreakAchievement(WORD itemID);
    void unlockCaitSithLastLimitBreakAchievement(const savemap_char characters[]);
    void unlockGoldChocoboAchievement(const chocobo_slot firstFourSlots[], const chocobo_slot lastTwoSlots[]);
    void unlockGameProgressAchievement();
    void unlockYuffieAndVincentAchievement(unsigned char yuffieRegMask, unsigned char vincentRegMask);
};

class SteamAchievementsFF8
{
private:
    enum Achievements
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

    static inline const achievement ACHIEVEMENTS[] = {
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
        _ACH_ID(TOTAL_KILLS_10000),
    };

    struct upgrade_data {
        byte char_id = 0xFF;
        byte prev_weapon_id = 0xFF;
    };

    static inline const Achievements gfIndexToAchMap[16] = {
        UNLOCK_GF_QUEZACOTL,
        UNLOCK_GF_SHIVA,
        UNLOCK_GF_IFRIT,
        UNLOCK_GF_SIREN,
        UNLOCK_GF_BROTHERS,
        UNLOCK_GF_DIABLOS,
        UNLOCK_GF_CARBUNCLE,
        UNLOCK_GF_LEVIATHAN,
        UNLOCK_GF_PANDEMONA,
        UNLOCK_GF_CERBERUS,
        UNLOCK_GF_ALEXANDER,
        UNLOCK_GF_DOOMTRAIN,
        UNLOCK_GF_BAHAMUT,
        UNLOCK_GF_CACTUAR,
        UNLOCK_GF_TONBERRY,
        UNLOCK_GF_EDEN,
    };

    static inline constexpr int FF8_N_ACHIEVEMENTS = 45;

    static inline const std::string ENEMY_KILLED_STAT_NAME = "nmy_kill";
    static inline const std::string DRAW_MAGIC_STAT_NAME = "mag_draw";
    static inline const std::string STOCK_MAGIC_STAT_NAME = "mag_stck";
    static inline const std::string WON_CARDGAME_STAT_NAME = "won_card";

    static inline constexpr int N_CARDS = 77;
    static inline constexpr int N_RARE_CARDS = 33;
    static inline constexpr byte SQUALL_CARD_LOCATION = 0xF0;

    static inline constexpr WORD MAX_SEED_EXP = 3100;
    static inline constexpr int MAX_HP = 9999;
    static inline constexpr uint32_t MAX_GIL = 99999999;
    static inline constexpr int MAX_LEVEL = 100;

    std::unique_ptr<SteamManager> steamManager;
    std::array<bool, N_RARE_CARDS> prevOwnedRareCards;
    upgrade_data prevWeaponUpgradeData;
    byte statCharId = 0xFF;

    // steam stats
    int magicStocked = 0;
    int magicDrawn = 0;

public:
    SteamAchievementsFF8();
    ~SteamAchievementsFF8() = default;

    void initOwnedTripleTriadRareCards(const savemap_ff8_triple_triad &triple_triad);
    void initPreviousWeaponIdBeforeUpgrade(byte charId, byte weaponId);
    void initStatCharIdUnderStatCompute(byte statCharId);

    byte getStatCharIdUnderStatCompute();

    void unlockPlayTripleTriadAchievement();
    void unlockLoserTripleTriadAchievement(const savemap_ff8_triple_triad &triple_triad);
    void unlockCollectorTripleTriadAchievement(const savemap_ff8_triple_triad &triple_triad);
    void increaseCardWinsAndUnlockProfessionalAchievement();
    void unlockGuardianForceAchievement(int gf_idx);
    void unlockTopSeedRankAchievement(WORD seed_exp);
    void unlockUpgradeWeaponAchievement(const savemap_ff8 &savemap);
    void unlockMaxHpAchievement(int max_hp);
    void unlockMaxGilAchievement(uint32_t gil);
    void unlockTopLevelAchievement(int level);
    void increaseKillsAndTryUnlockAchievement();

};

// Global, access to Achievements object
extern std::unique_ptr<SteamAchievementsFF7> g_FF7SteamAchievements;
extern std::unique_ptr<SteamAchievementsFF8> g_FF8SteamAchievements;
