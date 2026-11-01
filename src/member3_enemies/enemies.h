// =====================================================================
//  enemies.h  -  raider models.
//  Implemented by Friend 3 in member3_enemies/enemies.cpp.
// =====================================================================
#pragma once
#include "core/entity.h"

// Draws one raider (facing left) with its feet at (x, y). s = size.
void drawEnemyModel(EntityType type, float x, float y, float s, float animTime = 0);
