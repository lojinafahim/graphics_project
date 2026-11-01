#include "core/game_state.h"
#include "core/config.h"

Screen g_screen = SCREEN_MAIN_MENU;
int    g_coins  = START_COINS;

void setScreen(Screen s) {
    g_screen = s;
}

bool canAfford(int cost) {
    return g_coins >= cost;
}

bool spendCoins(int cost) {
    if (!canAfford(cost)) return false;
    g_coins -= cost;
    return true;
}

void addCoins(int amount) {
    g_coins += amount;
}
