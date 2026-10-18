// =====================================================================
//  game_state.cpp  -  SHARED.
// =====================================================================
#include "game_state.h"
#include "config.h"
#include "interfaces.h"
#include <algorithm>

Screen g_screen = START_SCREEN;
int    g_coins  = START_COINS;

std::vector<Entity>      g_players;
std::vector<Entity>      g_enemies;
std::vector<Attack>      g_attacks;
std::vector<Collectible> g_collectibles;

void setScreen(Screen s) { g_screen = s; }

bool canAfford(int cost) { return g_coins >= cost; }
bool spendCoins(int cost) {
    if (g_coins < cost) return false;
    g_coins -= cost;
    return true;
}
void addCoins(int amount) { g_coins += amount; }

void resetGame() {
    g_coins = START_COINS;
    g_players.clear();
    g_enemies.clear();
    g_attacks.clear();
    g_collectibles.clear();
    ui_reset();
    players_reset();
    enemies_reset();
    coins_reset();
}

template <typename T>
static void removeDead(std::vector<T>& list) {
    list.erase(std::remove_if(list.begin(), list.end(),
                              [](const T& item) { return !item.alive; }),
               list.end());
}

void cleanupDead() {
    removeDead(g_players);
    removeDead(g_enemies);
    removeDead(g_attacks);
    removeDead(g_collectibles);
}
