// =====================================================================
//  entity.h  -  SHARED. The kinds of characters in the game.
//  Friend 2 adds the Entity struct (lane, x, hp, ...) to this file later.
// =====================================================================
#pragma once

enum EntityType {
    PLAYER_MELEE, PLAYER_RANGED, PLAYER_DEFENSE,
    ENEMY_EASY, ENEMY_MEDIUM, ENEMY_HARD
};
