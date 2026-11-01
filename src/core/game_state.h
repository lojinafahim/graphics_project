// =====================================================================
//  game_state.h  -  SHARED game data everyone can read.
// =====================================================================
#pragma once

enum Screen {
    SCREEN_MAIN_MENU, SCREEN_SOUND_SETTINGS, SCREEN_CUTSCENE,
    SCREEN_PLAYING, SCREEN_PAUSED, SCREEN_WIN, SCREEN_LOSE, SCREEN_QUIT
};

extern Screen g_screen;     // which screen is showing right now
extern int    g_coins;      // the player's coins

void setScreen(Screen s);

bool canAfford(int cost);   // enough coins?
bool spendCoins(int cost);  // returns false (and spends nothing) if not enough
void addCoins(int amount);
