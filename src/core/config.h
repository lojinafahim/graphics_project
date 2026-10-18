// =====================================================================
//  config.h  -  SHARED. Agreed by the whole team.
//  Do NOT change values here without telling the group first.
// =====================================================================
#pragma once

// ---------- Screen ----------------------------------------------------
// World coordinates: (0,0) is the BOTTOM-LEFT corner, 1280 x 720.
// Every draw function uses these pixel coordinates.
const int SCREEN_W = 1280;
const int SCREEN_H = 720;

// ---------- Layout (matches the reference picture) ----------------------
//   y 0   .. 110  : bottom HUD bar (player cards left, coin counter right)
//   y 120 .. 560  : the 5 lanes (grid)
//   y 560 .. 720  : sky / skyline (obelisks, palms, pyramids)
//   x 0   .. 160  : the tomb (home base) on the left
// Lane 0 is the bottom lane, lane NUM_LANES-1 is the top lane.
const char* const GAME_TITLE    = "Guardians of the Pharaoh";
const int   NUM_LANES     = 5;
const int   NUM_COLS      = 9;
const float HUD_TOP       = 110.0f;   // HUD bar is y = 0 .. HUD_TOP
const float GRID_BOTTOM   = 120.0f;   // y where lane 0 starts
const float LANE_HEIGHT   = 88.0f;    // 5 lanes -> grid goes up to y = 560
const float GRID_TOP      = GRID_BOTTOM + NUM_LANES * LANE_HEIGHT;
const float TOMB_RIGHT_X  = 160.0f;   // grid starts right after the tomb
const float CELL_WIDTH    = 120.0f;   // 9 columns -> grid ends at x = 1240
const float GRID_RIGHT_X  = TOMB_RIGHT_X + NUM_COLS * CELL_WIDTH;
const float ENEMY_SPAWN_X = SCREEN_W - 50.0f;   // enemies appear at the right edge

inline float laneBottomY(int lane) { return GRID_BOTTOM + lane * LANE_HEIGHT; }
inline float laneCenterY(int lane) { return laneBottomY(lane) + LANE_HEIGHT * 0.5f; }
inline float colLeftX(int col)     { return TOMB_RIGHT_X + col * CELL_WIDTH; }
inline float colCenterX(int col)   { return colLeftX(col) + CELL_WIDTH * 0.5f; }
// Returns the lane under a y position, or -1 if outside the grid.
inline int laneAtY(float y) {
    if (y < GRID_BOTTOM || y >= GRID_TOP) return -1;
    return (int)((y - GRID_BOTTOM) / LANE_HEIGHT);
}

// ---------- Economy ---------------------------------------------------
const int COST_MELEE     = 10;
const int COST_RANGED    = 20;
const int COST_DEFENSE   = 30;
const int START_COINS    = 30;
const int COIN_DROP_VALUE = 5;   // value of one treasure coin

// ---------- Health / damage (from the project brief) ------------------
const int HP_MELEE   = 5;
const int HP_RANGED  = 5;
const int HP_DEFENSE = 7;
const int HP_EASY    = 5;   // slingshot raider
const int HP_MEDIUM  = 5;   // fire-pot raider
const int HP_HARD    = 7;   // axe brute
const int DMG_NORMAL = 1;
const int DMG_BRUTE  = 2;

// ---------- Speeds (pixels per second) - tweak freely together ---------
const float SPEED_PLAYER_WALK = 40.0f;
const float SPEED_ENEMY_WALK  = 35.0f;
const float SPEED_ENEMY_RUN   = 90.0f;   // axe brute
const float SPEED_PROJECTILE  = 260.0f;
const float JUMP_DURATION     = 0.6f;    // seconds in the air
const float DUCK_DURATION     = 0.6f;    // seconds ducking

// ---------- Waves -----------------------------------------------------
const int NUM_WAVES = 3;
const int WAVE_SIZE[NUM_WAVES] = { 3, 5, 10 };

// ---------- Testing ---------------------------------------------------
// Change this LOCALLY to jump straight to the screen you are working on.
// (Don't commit your change.)  Options are in game_state.h -> Screen.
#define START_SCREEN SCREEN_MAIN_MENU
