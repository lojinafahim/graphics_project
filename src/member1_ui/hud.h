// =====================================================================
//  hud.h  -  lane grid + HUD (cards, coin counter, wave panel).
//  Implemented by Friend 2 in member1_ui/grid_hud.cpp.
// =====================================================================
#pragma once

void grid_update(float dt);       // hover + click to toggle lanes, W/S keys
void grid_draw();                 // lane highlights
bool isLaneSelected(int lane);

void hud_update(float dt);        // player cards (lock/unlock, buy)
void hud_draw();                  // cards, coin counter, wave panel
