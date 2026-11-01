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
#include <cstdlib>

// ------------------------------------------------------------ R19: waves
// Reference: LearnOpenGL - 2D Game > Levels: a list of levels + current index,
//   and GameLevel::IsCompleted() -> "is anything still alive?"
// Reference: LearnOpenGL - 2D Game > Particles: rand() for random values,
//   and Life -= dt as a countdown timer.
const float SPAWN_GAP  = 2.5f;   // seconds between raiders inside a wave
const float WAVE_PAUSE = 3.0f;   // breather before a wave starts

static int   s_wave       = 1;          // 1..NUM_WAVES
static int   s_spawned    = 0;          // raiders spawned so far in this wave
static float s_spawnTimer = WAVE_PAUSE; // countdown to the next spawn
static bool  s_allCleared = false;

int  currentWave()     { return s_wave; }
bool allWavesCleared() { return s_allCleared; }

// Called from enemies_reset() (Start / Restart)
void waves_reset() {
    s_wave = 1;
    s_spawned = 0;
    s_spawnTimer = WAVE_PAUSE;
    s_allCleared = false;
}

// like GameLevel::IsCompleted(), but returns true while something is alive
static bool anyRaiderAlive() {
    for (const Entity& e : g_enemies)
        if (e.alive) return true;
    return false;
}

// Wave 1: easy | Wave 2: easy + medium | Wave 3: easy + medium + hard.
// The first spawns of a wave cycle through every allowed type, so each type
// is guaranteed to appear. The rest are random.
static EntityType pickType(int wave, int index) {
    int kinds = wave;                                  // 1, 2 or 3 types allowed
    int k = (index < kinds) ? index : std::rand() % kinds;
    return (EntityType)(ENEMY_EASY + k);
}

void waves_update(float dt) {
    if (s_allCleared) return;
    int size = WAVE_SIZE[s_wave - 1];

    // 1. Still spawning this wave: one raider every SPAWN_GAP seconds
    if (s_spawned < size) {
        s_spawnTimer -= dt;
        if (s_spawnTimer <= 0) {
            spawnEnemy(pickType(s_wave, s_spawned), std::rand() % NUM_LANES);
            s_spawned++;
            s_spawnTimer = SPAWN_GAP;
        }
        return;
    }

    // 2. Last raider has spawned -> the wave is over.
    //    Wait until every raider on screen is defeated.
    if (anyRaiderAlive()) return;

    // 3. Screen is clear
    if (s_wave == NUM_WAVES) {
        s_allCleared = true;               // Member 1 shows WIN
        return;
    }
    s_wave++;                              // next wave (the HUD badge updates by itself)
    s_spawned = 0;
    s_spawnTimer = WAVE_PAUSE;
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
// ------------------------------------------------------------ R21: projectile hits
// Reference: LearnOpenGL - 2D Game > Collisions:
//   CheckCollision (AABB) + DoCollisions (loop over every brick, skip destroyed ones)
const float PROJECTILE_SIZE = 10.0f;     // small box around a stone / pot / arrow

static bool hits(const Attack& a, const Entity& t) {
    if (!t.alive || a.lane != t.lane) return false;
    float aLeft = a.x - PROJECTILE_SIZE / 2;
    float tLeft = t.x - t.width / 2;
    return aLeft + PROJECTILE_SIZE >= tLeft && tLeft + t.width >= aLeft;
}

// HIGH (stone) flies over a ducking guard, LOW (pot) rolls under a jumping guard
static bool dodges(const Attack& a, const Entity& p) {
    if (a.height == ATTACK_HIGH && p.isDucking) return true;
    if (a.height == ATTACK_LOW  && p.isJumping) return true;
    return false;
}

// a living shield-bearer in this lane, or nullptr
static Entity* findShield(int lane) {
    for (Entity& p : g_players)
        if (p.alive && p.lane == lane && p.type == PLAYER_DEFENSE) return &p;
    return nullptr;
}

static void projectileHits() {
    for (Attack& a : g_attacks) {
        if (!a.alive) continue;

        if (a.fromPlayer) {
            // ARROW: first raider it reaches (flies right -> smallest x is first)
            // Only g_enemies is checked, so arrows can never hurt guards.
            Entity* target = nullptr;
            for (Entity& e : g_enemies)
                if (hits(a, e) && (!target || e.x < target->x)) target = &e;
            if (target) {
                damageEnemy(*target, a.damage);    // also drops the coin if it dies
                a.alive = false;
            }
        } else {
            // STONE / POT: first guard it reaches (flies left -> largest x is first)
            Entity* target = nullptr;
            for (Entity& p : g_players)
                if (hits(a, p) && !dodges(a, p) && (!target || p.x > target->x)) target = &p;
            if (!target) continue;                 // missed / dodged -> keeps flying

            // guard behind a shield-bearer: the shield absorbs the hit
            if (target->isShielded && target->type != PLAYER_DEFENSE) {
                Entity* shield = findShield(a.lane);
                if (shield) target = shield;
            }
            damagePlayer(*target, a.damage);
            a.alive = false;
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

        projectileHits();      // R21

    // TEST KEY (remove before submitting): K = everyone loses 1 hp
    // (lets Members 2 & 3 check red tint + health bars before combat exists)
        // TEST KEYS (remove before submitting): set guard flags by hand,
    // because Member 2's jump / duck / shield aren't merged yet
    if (keyPressed(GLFW_KEY_4)) for (Entity& p : g_players) p.isDucking  = !p.isDucking;
    if (keyPressed(GLFW_KEY_5)) for (Entity& p : g_players) p.isJumping  = !p.isJumping;
    if (keyPressed(GLFW_KEY_6)) for (Entity& p : g_players) p.isShielded = !p.isShielded;  
    if (keyPressed(GLFW_KEY_K)) {
        for (Entity& p : g_players) if (--p.hp <= 0) p.alive = false;
        for (Entity& e : g_enemies) if (--e.hp <= 0) { e.alive = false; spawnCoin(e.lane, e.x); }
    }
}
