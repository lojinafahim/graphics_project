// =====================================================================
//  interfaces.h  -  The agreed function names for every member.
//
//  Every function below ALREADY EXISTS as an empty stub in its owner's
//  file, so the project always compiles. You fill in YOUR OWN functions
//  and call other members' functions by these names - even before they
//  have written them.
//
//  Do NOT rename / change these without telling the group.
// =====================================================================
#pragma once
#include "entity.h"

// ---------------------------------------------------------------------
//  MEMBER 1 - engine, menus, lane grid, HUD, player cards, win/lose
//  files: src/member1_ui/
// ---------------------------------------------------------------------
void ui_init();
void ui_reset();

void mainMenu_update(float dt);     // Start / Exit / Sound settings
void mainMenu_draw();
void pauseMenu_update(float dt);    // Continue / Restart / Main menu
void pauseMenu_draw();
void endMenu_update(float dt);      // Win / Lose: Restart / Main menu
void endMenu_draw();

void grid_update(float dt);         // hover + click to toggle lanes, W/S keys
void grid_draw();
bool isLaneSelected(int lane);

void hud_update(float dt);          // player cards (lock/unlock, buy), coin counter
void hud_draw();

void winLose_update(float dt);      // checks allWavesCleared() / enemyReachedTomb()

// ---------------------------------------------------------------------
//  MEMBER 2 - players (guards)
//  files: src/member2_players/
// ---------------------------------------------------------------------
void players_init();
void players_reset();
void players_update(float dt);      // walking, attacking, jump/duck timers, shields
void players_draw();                // also draws arrows (g_attacks with fromPlayer)
void spawnPlayer(EntityType type, int lane);   // place at the left of that lane
void playerJump(int lane);          // every player in that lane jumps (not defense)
void playerDuck(int lane);          // every player in that lane ducks (not defense)
// Draws one guard model with feet at (x, y), size s (1 = normal).
// Used by players_draw, and by Member 1 (card portraits) and Member 4 (cutscene).
void drawPlayerModel(EntityType type, float x, float y, float s, float animTime = 0);

// ---------------------------------------------------------------------
//  MEMBER 3 - enemies (raiders), waves, combat
//  files: src/member3_enemies/
// ---------------------------------------------------------------------
void enemies_init();
void enemies_reset();
void enemies_update(float dt);      // movement + firing stones / fire pots
void enemies_draw();                // also draws enemy projectiles
void spawnEnemy(EntityType type, int lane);
// Draws one raider model (facing left) with feet at (x, y), size s.
// Used by enemies_draw and by Member 4's cutscene.
void drawEnemyModel(EntityType type, float x, float y, float s, float animTime = 0);

void waves_update(float dt);        // spawns waves, next wave after screen is clear
int  currentWave();                 // 1..NUM_WAVES
bool allWavesCleared();             // -> WIN
bool enemyReachedTomb();            // -> LOSE

void combat_update(float dt);       // collisions, projectile hits, damage

// ---------------------------------------------------------------------
//  MEMBER 4 - world, tomb, environment, collectibles, cutscene, sound
//  files: src/member4_world/
// ---------------------------------------------------------------------
void world_init();
void world_drawBackground();        // sky, desert, environment, tomb (behind lanes)
void world_drawForeground();        // anything drawn ON TOP of characters (optional)
void world_update(float dt);        // torch flicker, etc.

void cutscene_start();
void cutscene_update(float dt);
void cutscene_draw();
bool cutscene_finished();

void spawnCoin(int lane, float x);  // Member 3 calls this when a raider dies
void coins_reset();
void coins_update(float dt);        // pickup when a player touches a coin -> addCoins
void coins_draw();

// Sound (bonus) - safe to call even if sound is not done yet
void sound_init();
void playSound(const char* name);   // e.g. "attack", "death", "coin", "place", "select", "win", "lose"
void playMusic(const char* name);
void stopMusic();
void setVolume(float v);            // 0..1
float getVolume();
void toggleMute();
void soundSettings_update(float dt);   // the Sound Settings screen
void soundSettings_draw();
