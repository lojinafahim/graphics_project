// =====================================================================
//  MEMBER 3 - enemies.cpp   (raiders: slingshot, fire-pot, axe brute)
//
//  Starter version: BASIC hunched, ragged raider models (clearly
//  different from the upright, polished guards). Member 3 owns them and
//  must add detail + all the behaviour below.
//
//  TODO (Member 3):
//   [ ] More detail on all 3 models (faces, hands, torn cloth, belts, scars...)
//   [ ] Walk left (brute RUNS with SPEED_ENEMY_RUN) + walking animation
//   [ ] Slingshot raider fires stones: g_attacks, fromPlayer=false, ATTACK_HIGH,
//       drawn arcing down from above
//   [ ] Fire-pot raider rolls pots: ATTACK_LOW, rolling along the ground + flame
//   [ ] Axe brute: close-range only, DMG_BRUTE (2), axe swing animation
//   [ ] When a raider dies: alive = false and spawnCoin(e.lane, e.x)
//
//  Test alone: hover the mouse over a lane and press 7 / 8 / 9.
// =====================================================================
#include "../core/config.h"
#include "../core/shapes.h"
#include "../core/healthbar.h"
#include "../core/input.h"
#include "../core/game_state.h"
#include "../core/interfaces.h"
#include <cmath>

void enemies_init() {}
void enemies_reset() {}

void spawnEnemy(EntityType type, int lane) {
    Entity e;
    e.type  = type;
    e.lane  = lane;
    e.x     = ENEMY_SPAWN_X;
    e.y     = laneBottomY(lane) + 8;
    e.maxHp = (type == ENEMY_HARD) ? HP_HARD : HP_EASY;
    e.hp    = e.maxHp;
    e.state = STATE_WALKING;
    e.height = (type == ENEMY_HARD) ? 74 : 56;
    if (type == ENEMY_HARD) e.width = 64;
    g_enemies.push_back(e);
}

// ---------------------------------------------------------------- models
static const Color RAIDER_SKIN = rgb(150, 98, 66);
static const Color RAIDER_WRAP = rgb(150, 60, 40);

// Hunched raider body. Drawn facing RIGHT; drawEnemyModel mirrors it to face left.
// Reference: LearnOpenGL - Getting started > Transformations ("In practice"):
//   trans = glm::translate(...); trans = glm::rotate(trans, time, z-axis);
//   -> translate to the HIP, then rotate, so the leg swings around the hip.
// Reference: LearnOpenGL - Getting started > Shaders ("Uniforms"):
//   sin(timeValue) -> smooth back-and-forth value, used here as the swing angle.
static void drawRaiderBody(float bulk, float walkPhase) {
    float swing = std::sin(walkPhase) * 25.0f;       // -25..+25 degrees

    // back leg: pivot at the hip (-8, 25)
    pushMatrix();
    translate(-8, 25);
    rotate(swing);
    drawLine(0, 0, 6, -12, 6, RAIDER_SKIN);          // thigh
    drawLine(6, -12, 2, -25, 6, RAIDER_SKIN);        // shin
    drawRect(-2, -25, 8, 3, PAL_RAIDER_DARK);        // foot
    popMatrix();

    // front leg: pivot at the hip (4, 25), swings the opposite way
    pushMatrix();
    translate(4, 25);
    rotate(-swing);
    drawLine(0, 0, 7, -12, 6, RAIDER_SKIN);
    drawLine(7, -12, 3, -25, 6, RAIDER_SKIN);
    drawRect(1, -25, 9, 3, PAL_RAIDER_DARK);
    popMatrix();
    // ragged skirt with torn edge
    drawPolygon({ {-13 * bulk, 22}, {13 * bulk, 22}, {12 * bulk, 32}, {-12 * bulk, 32} }, PAL_RAIDER_RAGS);
    for (int i = 0; i < 4; i++) {
        float x0 = -13 * bulk + i * 6.5f * bulk;
        drawTriangle(x0, 22, x0 + 6.5f * bulk, 22, x0 + 3 * bulk, 16, PAL_RAIDER_RAGS);
    }
    drawRect(-12 * bulk, 30, 24 * bulk, 3, PAL_RAIDER_DARK);        // rope belt
    // hunched torso leaning forward
    pushMatrix();
    translate(0, 31);
    rotate(-22);
    drawRect(-9 * bulk, 0, 18 * bulk, 22, RAIDER_SKIN);
    drawPolygon({ {-10 * bulk, 2}, {4 * bulk, 2}, {-2 * bulk, 22}, {-10 * bulk, 22} }, PAL_RAIDER_DARK);  // torn vest
    // head with cloth wrap
    drawCircle(3, 28, 7.5f, RAIDER_SKIN);
    drawEllipse(1, 32, 8.5f, 4.5f, RAIDER_WRAP);
    drawTriangle(-7, 32, -16, 26, -8, 28, RAIDER_WRAP);             // wrap tail
    drawRect(5, 27, 3, 1.5f, PAL_BLACK);                            // eye
    drawRect(4, 23, 5, 1.5f, PAL_RAIDER_DARK);                      // stubble / scowl
    popMatrix();
}

void drawEnemyModel(EntityType type, float x, float y, float s, float animTime) {
    pushMatrix();
    translate(x, y);
    scale(-s, s);      // mirror: raiders face LEFT toward the tomb

    if (type == ENEMY_EASY) {                         // ---- slingshot raider
        drawRaiderBody(1.0f, animTime * 8);
        float spin = animTime * 400;                  // sling whirling overhead
        pushMatrix();
        translate(8, 58);
        rotate(spin);
        drawLine(0, 0, 16, 0, 1.5f, PAL_WOOD);
        drawCircle(17, 0, 3.5f, rgb(120, 115, 105));
        popMatrix();
        drawLine(2, 46, 8, 58, 4, RAIDER_SKIN);       // raised arm
        drawRect(-14, 30, 6, 8, rgb(120, 100, 70));   // stone pouch
    }
    else if (type == ENEMY_MEDIUM) {                  // ---- fire-pot raider
        drawRaiderBody(1.0f, animTime * 8);
        drawLine(4, 42, 14, 24, 4, RAIDER_SKIN);      // arms down to the pot
        drawCircle(18, 16, 9, rgb(120, 70, 40));      // clay pot
        drawRect(12, 22, 12, 4, rgb(90, 50, 30));
        drawRect(10, 14, 16, 2, rgb(160, 100, 60));
        float f = 1 + 0.2f * std::sin(animTime * 14);
        drawEllipse(18, 30, 5 * f, 8 * f, PAL_FIRE);
        drawEllipse(18, 28, 2.5f * f, 4 * f, rgb(255, 220, 90));
    }
    else {                                            // ---- axe brute
        scale(1.35f, 1.35f);
        drawRaiderBody(1.3f, animTime * 14);
        drawEllipse(-2, 52, 14, 7, PAL_RAIDER_DARK);  // fur shoulder
        float raise = std::sin(animTime * 3) * 10;
        pushMatrix();
        translate(6, 48);
        rotate(40 + raise);
        drawRect(0, -2, 34, 4, PAL_WOOD);             // axe handle
        drawPolygon({ {26, 2}, {36, 12}, {40, 0}, {36, -12}, {26, -2} }, rgb(150, 150, 160));   // blade
        drawLine(36, -12, 40, 0, 2, rgb(220, 220, 230));
        drawLine(40, 0, 36, 12, 2, rgb(220, 220, 230));
        popMatrix();
    }
    popMatrix();
}

// --------------------------------------------------------------- update
// Brute runs, the other two walk (speeds are in config.h, pixels per second)
static float enemySpeed(EntityType type) {
    return (type == ENEMY_HARD) ? SPEED_ENEMY_RUN : SPEED_ENEMY_WALK;
}
// R18: ranged raiders fire every FIRE_INTERVAL seconds
const float FIRE_INTERVAL = 2.5f;

// Adds one projectile to the shared list g_attacks
// (like the particle list in LearnOpenGL > 2D Game > Particles)
static void fireProjectile(const Entity& e) {
    Attack a;
    a.lane = e.lane;
    a.fromPlayer = false;
    a.damage = DMG_NORMAL;
    if (e.type == ENEMY_EASY) {          // sling stone - head height, dodged by DUCKING
        a.height = ATTACK_HIGH;
        a.x  = e.x - 15;
        a.y  = e.y + 45;
        a.vx = -SPEED_PROJECTILE;        // minus = flies left
    } else {                             // fire pot - on the ground, dodged by JUMPING
        a.height = ATTACK_LOW;
        a.x  = e.x - 20;
        a.y  = e.y + 8;
        a.vx = -SPEED_PROJECTILE * 0.6f; // rolls a bit slower
    }
    g_attacks.push_back(a);
}

void enemies_update(float dt) {
    for (Entity& e : g_enemies) {
        if (!e.alive) continue;
        e.animTime += dt;
        // LearnOpenGL Camera "Movement speed": position += speed * deltaTime
             if (e.state == STATE_WALKING)
            e.x -= enemySpeed(e.type) * dt;      // minus = move left

        // R18: shoot from the moment they appear (brief 3.1).
        // Countdown timer, like LearnOpenGL Particles: p.Life -= dt;
        // attackCooldown starts at 0, so the first shot is immediate.
        if (e.type != ENEMY_HARD) {
            e.attackCooldown -= dt;
            if (e.attackCooldown <= 0) {
                fireProjectile(e);
                e.attackCooldown = FIRE_INTERVAL;
            }
        }
    }
    // TODO: firing

    // TEST KEYS (remove before submitting): spawn in the lane under the mouse
    int lane = laneAtY(mouseY());
    if (lane < 0) lane = 2;
    if (keyPressed(GLFW_KEY_7)) spawnEnemy(ENEMY_EASY, lane);
    if (keyPressed(GLFW_KEY_8)) spawnEnemy(ENEMY_MEDIUM, lane);
    if (keyPressed(GLFW_KEY_9)) spawnEnemy(ENEMY_HARD, lane);
}
// Sling stone: lobbed up, then drops back to head height (arc from above)
static void drawStone(const Attack& a) {
    float t = std::fmin(a.animTime / 0.6f, 1.0f);    // 0..1 during the lob
    float lift = 40.0f * 4.0f * t * (1.0f - t);      // parabola: 0 -> 40 -> 0 px
    drawCircle(a.x, a.y + lift, 4.5f, rgb(120, 115, 105));
    drawCircle(a.x - 1, a.y + lift + 1, 2, rgb(170, 165, 150));   // highlight
}

// Fire pot: spins while rolling left, flame on top
// Reference: LearnOpenGL Transformations "In practice" - translate, then rotate by time
static void drawFirePot(const Attack& a) {
    float f = 1 + 0.25f * std::sin(a.animTime * 20);              // flicker
    drawEllipse(a.x, a.y + 12, 5 * f, 9 * f, PAL_FIRE);
    drawEllipse(a.x, a.y + 10, 2.5f * f, 5 * f, PAL_FIRE_CORE);
    pushMatrix();
    translate(a.x, a.y);
    rotate(a.animTime * 360);                        // + = counter-clockwise = rolling left
    drawCircle(0, 0, 8, rgb(120, 70, 40));
    drawRect(-8, -1.5f, 16, 3, rgb(160, 100, 60));   // stripe so you can see it spin
    popMatrix();
}

void enemies_draw() {
    for (const Entity& e : g_enemies) {
        drawEllipse(e.x, e.y + 1, e.type == ENEMY_HARD ? 30.0f : 20.0f, 5, rgb(120, 90, 50, 90));
        setTint(e.damageAmount());
        drawEnemyModel(e.type, e.x, e.y, 0.85f, e.animTime);
        resetEffects();
        drawHealthBar(e.x, e.y + e.height + 2, e.hp, e.maxHp);
    }
        // R18: enemy projectiles (arrows are drawn by Member 2)
    for (const Attack& a : g_attacks) {
        if (a.fromPlayer || !a.alive) continue;
        if (a.height == ATTACK_HIGH) drawStone(a);
        else                         drawFirePot(a);
    }
}

// A raider reaching the tomb = LOSE (Member 1 calls this every frame)
bool enemyReachedTomb() {
    for (const Entity& e : g_enemies)
        if (e.alive && e.x - e.width / 2 <= TOMB_RIGHT_X) return true;
    return false;
}
