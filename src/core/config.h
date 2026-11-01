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
