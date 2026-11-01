// =====================================================================
//  MEMBER 3 - waves_combat.cpp
//
//  TODO (Member 3) - waves:
//   [ ] Wave 1: 3 easy | Wave 2: 5 easy + medium | Wave 3: 10 of all types
//       (WAVE_SIZE in config.h). Random lanes, spawned a little apart in time.
//   [ ] Next wave only starts when the screen is clear of raiders.
//
//  TODO (Member 3) - combat:
//   [ ] Player and raider in the same lane touching -> both STATE_FIGHTING,
//       stop moving, hit each other on a cooldown until one dies
//   [ ] Projectiles: move by vx * dt, hit the first opponent in the same lane
//         ATTACK_HIGH misses a player that isDucking
//         ATTACK_LOW  misses a player that isJumping
//         enemy shots never hurt a player that isShielded (they hit the defense)
//       on hit: target.hp -= damage, attack.alive = false
//   [ ] hp <= 0 -> alive = false (raider: also spawnCoin)
//   [ ] Several attacks at the same time must all work
//
//  Test alone: push dummy guards into g_players from here (or press 1/2/3,
//  which uses Member 2's placeholder spawn), and set flags by hand, e.g.
//      g_players.back().isDucking = true;
// =====================================================================
#include "../core/config.h"
#include "../core/game_state.h"
#include "../core/input.h"
#include "../core/interfaces.h"

static int  s_wave = 1;
static bool s_allCleared = false;

int  currentWave()     { return s_wave; }
bool allWavesCleared() { return s_allCleared; }

void waves_update(float dt) {
    // TODO
}

void combat_update(float dt) {
    // Move every projectile (arrows, stones, fire pots).
    // Reference: LearnOpenGL - 2D Game > Ball: Position += Velocity * dt;
    for (Attack& a : g_attacks) {
        if (!a.alive) continue;
        a.x += a.vx * dt;
        a.animTime += dt;
        if (a.x < 0 || a.x > SCREEN_W) a.alive = false;   // off screen -> removed by cleanupDead()
    }

    // TODO: collisions + hits

    // TEST KEY (remove before submitting): K = everyone loses 1 hp
    // (lets Members 2 & 3 check red tint + health bars before combat exists)
    if (keyPressed(GLFW_KEY_K)) {
        for (Entity& p : g_players) if (--p.hp <= 0) p.alive = false;
        for (Entity& e : g_enemies) if (--e.hp <= 0) { e.alive = false; spawnCoin(e.lane, e.x); }
    }
}
