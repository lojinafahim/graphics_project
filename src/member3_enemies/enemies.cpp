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
static void drawRaiderBody(float bulk) {
    // bent legs
    drawLine(-6, 0, -2, 13, 6, RAIDER_SKIN);
    drawLine(-2, 13, -8, 25, 6, RAIDER_SKIN);
    drawLine(7, 0, 11, 13, 6, RAIDER_SKIN);
    drawLine(11, 13, 4, 25, 6, RAIDER_SKIN);
    drawRect(-10, 0, 8, 3, PAL_RAIDER_DARK);
    drawRect(5, 0, 9, 3, PAL_RAIDER_DARK);
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
        drawRaiderBody(1.0f);
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
        drawRaiderBody(1.0f);
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
        drawRaiderBody(1.3f);
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

void enemies_update(float dt) {
    for (Entity& e : g_enemies) {
        if (!e.alive) continue;
        e.animTime += dt;
        // LearnOpenGL Camera "Movement speed": position += speed * deltaTime
        if (e.state == STATE_WALKING)
            e.x -= enemySpeed(e.type) * dt;      // minus = move left
    }
    // TODO: firing

    // TEST KEYS (remove before submitting): spawn in the lane under the mouse
    int lane = laneAtY(mouseY());
    if (lane < 0) lane = 2;
    if (keyPressed(GLFW_KEY_7)) spawnEnemy(ENEMY_EASY, lane);
    if (keyPressed(GLFW_KEY_8)) spawnEnemy(ENEMY_MEDIUM, lane);
    if (keyPressed(GLFW_KEY_9)) spawnEnemy(ENEMY_HARD, lane);
}

void enemies_draw() {
    for (const Entity& e : g_enemies) {
        drawEllipse(e.x, e.y + 1, e.type == ENEMY_HARD ? 30.0f : 20.0f, 5, rgb(120, 90, 50, 90));
        setTint(e.damageAmount());
        drawEnemyModel(e.type, e.x, e.y, 0.85f, e.animTime);
        resetEffects();
        drawHealthBar(e.x, e.y + e.height + 2, e.hp, e.maxHp);
    }
    // TODO: draw enemy projectiles (g_attacks where fromPlayer == false)
}

// A raider reaching the tomb = LOSE (Member 1 calls this every frame)
bool enemyReachedTomb() {
    for (const Entity& e : g_enemies)
        if (e.alive && e.x - e.width / 2 <= TOMB_RIGHT_X) return true;
    return false;
}
