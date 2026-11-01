// =====================================================================
//  players.h  -  guard models.
//  Implemented by Friend 3 in member2_players/players.cpp.
// =====================================================================
#pragma once
#include "core/entity.h"

// Draws one guard with its feet at (x, y). s = size (1 = normal).
// animTime drives walking / attack animation (0 = standing still).
void drawPlayerModel(EntityType type, float x, float y, float s, float animTime = 0);
