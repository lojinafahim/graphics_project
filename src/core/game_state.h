// =====================================================================
//  game_state.h  -  SHARED game data everyone can read.
//
//  Ownership (who is allowed to ADD / REMOVE items):
//    g_players     -> Member 2   (others only read or change hp/flags)
//    g_enemies     -> Member 3
//    g_attacks     -> Member 2 adds arrows, Member 3 adds enemy shots,
//                     Member 3 (combat) removes them when they hit
//    g_collectibles-> Member 4
//    g_screen      -> Member 1   (others call setScreen only where noted)
//
//  Testing alone: push dummy entities into these lists from your own
//  file (see the TEST KEYS sections in each member's file).
// =====================================================================
#pragma once
#include <vector>
#include "entity.h"

enum Screen {
    SCREEN_MAIN_MENU,
    SCREEN_SOUND_SETTINGS,
    SCREEN_CUTSCENE,
    SCREEN_PLAYING,
    SCREEN_PAUSED,
    SCREEN_WIN,
    SCREEN_LOSE,
    SCREEN_QUIT
};

extern Screen g_screen;
extern int    g_coins;

extern std::vector<Entity>      g_players;
extern std::vector<Entity>      g_enemies;
extern std::vector<Attack>      g_attacks;
extern std::vector<Collectible> g_collectibles;

void setScreen(Screen s);

// Coins
bool canAfford(int cost);
bool spendCoins(int cost);     // returns false (and spends nothing) if not enough
void addCoins(int amount);

// Clears everything and calls every module's reset function.
// Called on Start / Restart.
void resetGame();

// Removes entries with alive == false from all lists. Called once per frame.
void cleanupDead();
