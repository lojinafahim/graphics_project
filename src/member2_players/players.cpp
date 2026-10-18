// =====================================================================
//  MEMBER 2 - players.cpp   (guards: khopesh soldier, archer, shield-bearer)
//
//  Starter version: BASIC guard models in the reference style (nemes
//  headdress, white kilt, gold/lapis collar). Member 2 owns them and must
//  add detail + all the behaviour below.
//
//  TODO (Member 2):
//   [ ] More detail on all 3 models (arms, hands, face, armour, belt...)
//   [ ] Melee + ranged walk right; defense stays where placed
//   [ ] Khopesh swing animation (rotate the sword arm using animTime)
//   [ ] Arrows: add to g_attacks (fromPlayer = true, height = ATTACK_MID),
//       draw them flying; they disappear on hit (combat sets alive = false)
//   [ ] playerJump / playerDuck: set isJumping / isDucking for JUMP_DURATION /
//       DUCK_DURATION (defense ignores both); draw the jump / duck pose
//   [ ] isShielded = true for players behind a living defense player
//       in the same lane (false once they walk past it or it dies)
//   [ ] Remove dead players (alive = false; cleanupDead() removes them)
//
//  Test alone: hover the mouse over a lane and press 1 / 2 / 3.
// =====================================================================
#include "../core/config.h"
#include "../core/shapes.h"
#include "../core/healthbar.h"
#include "../core/input.h"
#include "../core/ui_style.h"
#include "../core/game_state.h"
#include "../core/interfaces.h"
#include <cmath>

void players_init() {}
void players_reset() {}

void spawnPlayer(EntityType type, int lane) {
    Entity p;
    p.type  = type;
    p.lane  = lane;
    p.x     = TOMB_RIGHT_X + CELL_WIDTH * 0.5f;   // leftmost cell of the lane
    p.y     = laneBottomY(lane) + 8;
    p.maxHp = (type == PLAYER_DEFENSE) ? HP_DEFENSE : HP_MELEE;
    p.hp    = p.maxHp;
    p.state = (type == PLAYER_DEFENSE) ? STATE_IDLE : STATE_WALKING;
    p.height = 70;
    g_players.push_back(p);
}

void playerJump(int lane) { /* TODO */ }
void playerDuck(int lane) { /* TODO */ }

// ---------------------------------------------------------------- models
// All models: feet at (0,0), facing RIGHT, about 80 units tall.
static void drawGuardBody(bool slim) {
    float w = slim ? 8.0f : 10.0f;
    // legs + sandals
    drawRect(-w, 0, 7, 26, PAL_SKIN);
    drawRect(w - 7, 0, 7, 26, PAL_SKIN);
    drawRect(-w - 2, 0, 10, 3, PAL_WOOD);
    drawRect(w - 7, 0, 10, 3, PAL_WOOD);
    // kilt (shendyt) with front panel
    drawPolygon({ {-w - 3, 22}, {w + 3, 22}, {w + 1, 38}, {-w - 1, 38} }, PAL_WHITE_KILT);
    drawPolygon({ {-3, 22}, {5, 22}, {3, 36}, {-2, 36} }, rgb(220, 214, 196));
    drawRect(-w - 1, 36, 2 * w + 2, 4, PAL_GOLD);                    // belt
    // torso
    drawRect(-w, 40, 2 * w, 20, PAL_SKIN);
    // broad collar (wesekh): lapis + gold bands
    drawEllipse(0, 58, w + 4, 6, PAL_LAPIS);
    drawEllipse(0, 59, w + 2, 4, PAL_GOLD);
    drawEllipse(0, 60, w - 1, 2.5f, PAL_LAPIS);
}

static void drawNemesHead() {
    drawCircle(1, 69, 8.5f, PAL_SKIN);                               // face
    drawPolygon({ {-10, 64}, {9, 64}, {8, 79}, {-9, 80} }, PAL_GOLD);  // headdress
    for (int i = 0; i < 3; i++) drawRect(-9, 67 + i * 4.5f, 17, 2, PAL_LAPIS);
    drawPolygon({ {-10, 64}, {-4, 64}, {-6, 50}, {-12, 52} }, PAL_GOLD);  // side lappet
    drawRect(-11, 56, 6, 2, PAL_LAPIS);
    drawCircle(2, 70, 3.5f, PAL_SKIN);                               // face over cloth
    drawRect(4, 70, 3, 1.6f, PAL_BLACK);                             // kohl eye
    drawRect(6, 69, 2, 1, PAL_BLACK);
}

void drawPlayerModel(EntityType type, float x, float y, float s, float animTime) {
    pushMatrix();
    translate(x, y);
    scale(s, s);

    if (type == PLAYER_MELEE) {                       // ---- khopesh soldier
        drawGuardBody(false);
        drawNemesHead();
        drawRect(-14, 44, 6, 14, PAL_SKIN);           // back arm
        drawCircle(-11, 44, 4, rgb(40, 60, 120));      // arm band
        float swing = std::sin(animTime * 4) * 25;    // placeholder swing
        pushMatrix();
        translate(9, 54);
        rotate(-35 + swing);
        drawRect(0, -3, 14, 6, PAL_SKIN);             // sword arm
        drawRect(12, -2, 8, 4, PAL_WOOD);             // grip
        drawRect(19, -1.5f, 12, 3, PAL_BRONZE);       // straight part of blade
        drawArc(38, 4, 9, -100, 120, 4, PAL_BRONZE, 14);   // curved khopesh blade
        popMatrix();
    }
    else if (type == PLAYER_RANGED) {                 // ---- archer
        drawPolygon({ {-16, 38}, {-10, 36}, {-6, 66}, {-12, 68} }, PAL_WOOD);   // quiver
        for (int i = 0; i < 3; i++) drawLine(-12 + i * 2, 66, -10 + i * 3, 74, 1.5f, PAL_WHITE);
        drawGuardBody(true);
        drawCircle(1, 69, 8.5f, PAL_SKIN);
        drawEllipse(-2, 73, 9, 7, PAL_BLACK);         // hair
        drawRect(-9, 73, 18, 3, PAL_LAPIS);           // headband
        drawCircle(7, 74, 2, PAL_GOLD);
        drawRect(4, 69, 3, 1.6f, PAL_BLACK);
        drawRect(6, 48, 16, 5, PAL_SKIN);             // arm holding the bow
        float pull = 4 + std::sin(animTime * 3) * 2;
        drawArc(18, 50, 22, -70, 70, 3, PAL_WOOD, 16);              // bow
        drawLine(18 + 22 * 0.342f, 50 - 22 * 0.94f, 18 - pull, 50, 1, PAL_WHITE);  // string
        drawLine(18 + 22 * 0.342f, 50 + 22 * 0.94f, 18 - pull, 50, 1, PAL_WHITE);
        drawLine(18 - pull, 50, 44, 50, 1.5f, PAL_WOOD);            // arrow
        drawTriangle(44, 47, 44, 53, 50, 50, PAL_BRONZE);
    }
    else {                                            // ---- shield-bearer (defense)
        scale(1.12f, 1.05f);
        drawLine(-12, 2, -6, 92, 3, PAL_WOOD);         // spear behind
        drawTriangle(-9, 90, -3, 91, -5, 102, PAL_BRONZE);
        drawGuardBody(false);
        drawCircle(1, 69, 8.5f, PAL_SKIN);
        drawEllipse(0, 74, 11, 9, PAL_BRONZE);        // helmet
        drawRect(-11, 70, 22, 4, PAL_GOLD);
        drawPolygon({ {-3, 82}, {3, 82}, {0, 92} }, PAL_LAPIS);     // crest
        drawRect(4, 69, 3, 1.6f, PAL_BLACK);
        // tall shield in front with an ankh
        drawRoundedRect(6, 8, 26, 64, 6, PAL_UI_GOLD_DK);
        drawRoundedRect(8, 10, 22, 60, 5, PAL_GOLD);
        drawRoundedRect(11, 13, 16, 54, 4, PAL_LAPIS);
        drawAnkh(19, 42, 30, PAL_GOLD);
    }
    popMatrix();
}

// --------------------------------------------------------------- update
void players_update(float dt) {
    for (Entity& p : g_players) p.animTime += dt;
    // TODO: movement, attacking, timers, shields

    // TEST KEYS (remove before submitting): spawn in the lane under the mouse
    int lane = laneAtY(mouseY());
    if (lane < 0) lane = 2;
    if (keyPressed(GLFW_KEY_1)) spawnPlayer(PLAYER_MELEE, lane);
    if (keyPressed(GLFW_KEY_2)) spawnPlayer(PLAYER_RANGED, lane);
    if (keyPressed(GLFW_KEY_3)) spawnPlayer(PLAYER_DEFENSE, lane);
}

void players_draw() {
    for (const Entity& p : g_players) {
        drawEllipse(p.x, p.y + 1, 22, 5, rgb(120, 90, 50, 90));      // shadow
        setTint(p.damageAmount());
        drawPlayerModel(p.type, p.x, p.y, 0.85f, p.animTime);
        resetEffects();
        drawHealthBar(p.x, p.y + p.height + 2, p.hp, p.maxHp);
    }
    // TODO: draw arrows (g_attacks where fromPlayer == true)
}
