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