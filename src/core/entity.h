// =====================================================================
//  entity.h  -  SHARED data types used by every member.
//  Do NOT change without telling the group.
// =====================================================================
#pragma once

enum EntityType {
    // players (Member 2)
    PLAYER_MELEE,     // khopesh soldier
    PLAYER_RANGED,    // archer
    PLAYER_DEFENSE,   // shield-bearer / statue
    // enemies (Member 3)
    ENEMY_EASY,       // slingshot raider  - attacks HIGH (players duck)
    ENEMY_MEDIUM,     // fire-pot raider   - attacks LOW  (players jump)
    ENEMY_HARD        // axe brute         - MELEE, 2 damage, runs
};

enum EntityState {
    STATE_WALKING,
    STATE_IDLE,       // standing still (e.g. defense player)
    STATE_FIGHTING,   // stopped and fighting someone
    STATE_DEAD
};

// One player or enemy on the field.
struct Entity {
    EntityType  type;
    EntityState state = STATE_WALKING;
    int   lane = 0;
    float x = 0;            // centre x of the character's feet (world pixels)
    float y = 0;            // feet y (normally laneBottomY(lane) + a few px)
    float width = 50;       // collision box width
    float height = 80;      // collision box height
    int   hp = 1;
    int   maxHp = 1;
    bool  alive = true;

    // Player flags that the enemy side reads (Member 3 checks these)
    bool  isJumping  = false;
    bool  isDucking  = false;
    bool  isShielded = false;   // true while behind a living defense player

    // Free for the owner to use (timers, animation, etc.)
    float actionTimer = 0;      // jump / duck countdown
    float attackCooldown = 0;
    float animTime = 0;
    int   targetIndex = -1;     // who this entity is fighting (index), -1 = none

    bool isPlayer() const { return type <= PLAYER_DEFENSE; }
    bool isEnemy()  const { return type >= ENEMY_EASY; }
    // Damage amount from 0 (full health) to 1 (almost dead) - use for red tint
    float damageAmount() const { return maxHp > 0 ? 1.0f - (float)hp / maxHp : 0.0f; }
};

enum AttackHeight {
    ATTACK_HIGH,    // flies at head height  -> dodged by DUCKING
    ATTACK_LOW,     // rolls on the ground   -> dodged by JUMPING
    ATTACK_MID      // arrows / normal shots -> hits unless blocked
};

// A projectile in flight (arrow, sling stone, fire pot).
struct Attack {
    int   lane = 0;
    float x = 0, y = 0;
    float vx = 0;              // + moves right (player shots), - moves left (enemy shots)
    int   damage = 1;
    bool  fromPlayer = true;   // true = arrow, false = enemy projectile
    AttackHeight height = ATTACK_MID;
    float animTime = 0;        // e.g. spin the fire pot, arc the stone
    bool  alive = true;
};

// A treasure coin on the ground (Member 4).
struct Collectible {
    int   lane = 0;
    float x = 0, y = 0;
    int   value = 5;
    float animTime = 0;
    bool  alive = true;
};
