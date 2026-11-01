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
// ------------------------------------------------------------ combat helpers
const float MELEE_INTERVAL = 1.0f;   // seconds between blows in a fight

// R22: raider dies -> alive = false + drop a coin where it fell
static void damageEnemy(Entity& e, int dmg) {
    if (!e.alive) return;            // already dead -> no second coin
    e.hp -= dmg;
    if (e.hp <= 0) {
        e.hp = 0;
        e.alive = false;
        e.state = STATE_DEAD;
        spawnCoin(e.lane, e.x);
    }
}

static void damagePlayer(Entity& p, int dmg) {
    if (!p.alive) return;
    p.hp -= dmg;
    if (p.hp <= 0) { p.hp = 0; p.alive = false; p.state = STATE_DEAD; }
}

// Reference: LearnOpenGL - 2D Game > Collisions > "Collision detection" (AABB - AABB):
//   collisionX = one.Position.x + one.Size.x >= two.Position.x &&
//                two.Position.x + two.Size.x >= one.Position.x;
// Our x is the CENTRE of the feet, so left edge = x - width/2.
// The y-axis test becomes "same lane".
static bool touching(const Entity& a, const Entity& b) {
    if (a.lane != b.lane) return false;
    float aLeft = a.x - a.width / 2;
    float bLeft = b.x - b.width / 2;
    return aLeft + a.width >= bLeft && bLeft + b.width >= aLeft;
}

// R20: guard + raider touching -> both stop and fight until one dies
static void meleeUpdate(float dt) {
    // 1. Nobody is fighting until we find a contact this frame
    for (Entity& e : g_enemies)
        if (e.alive && e.state == STATE_FIGHTING) e.state = STATE_WALKING;
    for (Entity& p : g_players)
        if (p.alive && p.state == STATE_FIGHTING)
            p.state = (p.type == PLAYER_DEFENSE) ? STATE_IDLE : STATE_WALKING;

    // 2. Each raider fights the first guard it touches
    for (Entity& e : g_enemies) {
        if (!e.alive) continue;
        for (Entity& p : g_players) {
            if (!p.alive || !touching(e, p)) continue;
            e.state = STATE_FIGHTING;          // both stop moving
            p.state = STATE_FIGHTING;
            // countdown, like LearnOpenGL Particles: p.Life -= dt;
            e.actionTimer -= dt;
            if (e.actionTimer <= 0) {          // starts at 0 -> first blow is immediate
                damagePlayer(p, e.type == ENEMY_HARD ? DMG_BRUTE : DMG_NORMAL);
                damageEnemy(e, DMG_NORMAL);
                e.actionTimer = MELEE_INTERVAL;
            }
            break;                             // one opponent at a time
        }
    }
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

        meleeUpdate(dt);       // R20 + R22

    // TODO: projectile hits (R21)

    // TEST KEY (remove before submitting): K = everyone loses 1 hp
    // (lets Members 2 & 3 check red tint + health bars before combat exists)
    if (keyPressed(GLFW_KEY_K)) {
        for (Entity& p : g_players) if (--p.hp <= 0) p.alive = false;
        for (Entity& e : g_enemies) if (--e.hp <= 0) { e.alive = false; spawnCoin(e.lane, e.x); }
    }
}
