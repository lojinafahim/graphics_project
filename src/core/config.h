#pragma once

const int SCREEN_W = 1280;
const int SCREEN_H = 720;
const char* const GAME_TITLE = "Guardians of the Tomb";
const int   NUM_LANES   = 5;
const float GRID_BOTTOM = 120.0f;
const float LANE_HEIGHT = 88.0f;
const float GRID_TOP    = GRID_BOTTOM + NUM_LANES * LANE_HEIGHT;
const float TOMB_RIGHT_X = 160.0f;
inline float laneBottomY(int lane) { return GRID_BOTTOM + lane * LANE_HEIGHT; }
inline float laneCenterY(int lane) { return laneBottomY(lane) + LANE_HEIGHT * 0.5f; }

// ---------- Economy (brief: ranged = 2x melee, defense = 3x, start = 3 melee)
const int COST_MELEE   = 10;
const int COST_RANGED  = 20;
const int COST_DEFENSE = 30;
const int START_COINS  = 30;

// ---------- Health (from the brief)
const int HP_MELEE = 5, HP_RANGED = 5, HP_DEFENSE = 7;
const int HP_EASY  = 5, HP_MEDIUM = 5, HP_HARD    = 7;

// ---------- More layout (lanes, columns, HUD)
const int   NUM_COLS      = 9;
const float HUD_TOP       = 110.0f;   // HUD bar is y = 0 .. HUD_TOP
const float CELL_WIDTH    = 120.0f;   // 9 columns -> grid ends at x = 1240
const float GRID_RIGHT_X  = TOMB_RIGHT_X + NUM_COLS * CELL_WIDTH;
const float ENEMY_SPAWN_X = SCREEN_W - 50.0f;   // enemies appear at the right edge

inline float colLeftX(int col)   { return TOMB_RIGHT_X + col * CELL_WIDTH; }
inline float colCenterX(int col) { return colLeftX(col) + CELL_WIDTH * 0.5f; }
// Returns the lane under a y position, or -1 if outside the grid.
inline int laneAtY(float y) {
    if (y < GRID_BOTTOM || y >= GRID_TOP) return -1;
    return (int)((y - GRID_BOTTOM) / LANE_HEIGHT);
}

const int COIN_DROP_VALUE = 5;   // value of one treasure coin
const int DMG_NORMAL = 1;
const int DMG_BRUTE  = 2;

// ---------- Speeds (pixels per second)
const float SPEED_PLAYER_WALK = 40.0f;
const float SPEED_ENEMY_WALK  = 35.0f;
const float SPEED_ENEMY_RUN   = 90.0f;   // axe brute
const float SPEED_PROJECTILE  = 260.0f;
const float JUMP_DURATION     = 0.6f;    // seconds in the air
const float DUCK_DURATION     = 0.6f;    // seconds ducking

// ---------- Waves
const int NUM_WAVES = 3;
const int WAVE_SIZE[NUM_WAVES] = { 3, 5, 10 };

// ---------- Testing: change LOCALLY to start on another screen (don't commit)
#define START_SCREEN SCREEN_MAIN_MENU
