// =====================================================================
//  MEMBER 2 - players.cpp   (guards: khopesh soldier, archer, shield-bearer)
//
//  Done:
//   [x] More detail on all 3 models (hands, face, armour, belt, sandals...)
//   [x] Melee + ranged walk right; defense stays where placed
//   [x] Walking animation (legs swing) + khopesh swing (faster when fighting)
//   [x] Arrows: added to g_attacks (fromPlayer = true, height = ATTACK_MID),
//       they fly right and get drawn here
//   [x] playerJump / playerDuck with JUMP_DURATION / DUCK_DURATION,
//       jump + duck poses (defense ignores both)
//   [x] isShielded = true for guards behind a living shield-bearer
//   [x] Dead guards get alive = false (cleanupDead() removes them)
//
//  Test keys (remove before submitting) - mouse over a lane, then:
//   1 / 2 / 3 = spawn soldier / archer / shield-bearer
//   J = jump, U = duck, K = hit the first guard in that lane for 1 damage
// =====================================================================
#include "../core/config.h"
#include "../core/shapes.h"
#include "../core/healthbar.h"
#include "../core/input.h"
#include "../core/ui_style.h"
#include "../core/game_state.h"
#include "../core/interfaces.h"
#include <cmath>
#include <algorithm>

// how often the archer shoots (seconds between arrows)
const float ARCHER_COOLDOWN = 1.5f;
// guards stop walking here (the middle of the last cell)
const float PLAYER_STOP_X = GRID_RIGHT_X - CELL_WIDTH * 0.5f;
// size we draw the guards at in the lanes
const float GUARD_SCALE = 0.85f;
// how high a jump goes (pixels)
const float JUMP_HEIGHT = 45.0f;

// drawPlayerModel can't take more parameters (its name is shared with the
// other members), so players_draw sets these before calling it.
// card portraits + cutscene don't touch them, so their guards stand still
static bool s_walking  = false;   // swing the legs
static bool s_fighting = false;   // swing the sword faster

void players_init() {}

// called on restart - remove all guards and their arrows
void players_reset() {
    g_players.clear();
    g_attacks.erase(std::remove_if(g_attacks.begin(), g_attacks.end(),
                                   [](const Attack& a) { return a.fromPlayer; }),
                    g_attacks.end());
}

void spawnPlayer(EntityType type, int lane) {
    Entity p;
    p.type  = type;
    p.lane  = lane;
    p.x     = TOMB_RIGHT_X + CELL_WIDTH * 0.5f;   // leftmost cell of the lane
    p.y     = laneBottomY(lane) + 8;
    if (type == PLAYER_MELEE)       p.maxHp = HP_MELEE;
    else if (type == PLAYER_RANGED) p.maxHp = HP_RANGED;
    else                            p.maxHp = HP_DEFENSE;
    p.hp    = p.maxHp;
    p.state = (type == PLAYER_DEFENSE) ? STATE_IDLE : STATE_WALKING;
    p.height = 70;
    p.attackCooldown = 0.3f;   // archer shoots almost right away
    g_players.push_back(p);
}

// every guard in the lane jumps (shield-bearer doesn't)
void playerJump(int lane) {
    for (Entity& p : g_players) {
        if (!p.alive || p.lane != lane || p.type == PLAYER_DEFENSE) continue;
        p.isJumping = true;
        p.isDucking = false;
        p.actionTimer = JUMP_DURATION;
    }
}

// same thing but ducking
void playerDuck(int lane) {
    for (Entity& p : g_players) {
        if (!p.alive || p.lane != lane || p.type == PLAYER_DEFENSE) continue;
        p.isDucking = true;
        p.isJumping = false;
        p.actionTimer = DUCK_DURATION;
    }
}

// ---------------------------------------------------------------- models
// All models: feet at (0,0), facing RIGHT, about 80 units tall.

// one leg + sandal, rotated at the hip so it can swing while walking
static void drawLeg(float hipX, float angle) {
    pushMatrix();
    translate(hipX, 26);
    rotate(angle);
    drawRect(-3.5f, -26, 7, 26, PAL_SKIN);
    drawRect(-3.5f, -14, 7, 2, rgb(200, 150, 100));      // knee line
    drawRect(-5.5f, -26, 10, 3, PAL_WOOD);              // sandal
    drawRect(-1, -23, 2, 4, PAL_WOOD);                  // sandal strap
    popMatrix();
}

static void drawGuardBody(bool slim, float animTime) {
    float w = slim ? 8.0f : 10.0f;

    // legs swing in opposite directions when walking
    float legSwing = s_walking ? std::sin(animTime * 8) * 22 : 0;
    drawLeg(-w + 3.5f, legSwing);
    drawLeg(w - 3.5f, -legSwing);

    // kilt (shendyt) with front panel
    drawPolygon({ {-w - 3, 22}, {w + 3, 22}, {w + 1, 38}, {-w - 1, 38} }, PAL_WHITE_KILT);
    drawPolygon({ {-3, 22}, {5, 22}, {3, 36}, {-2, 36} }, rgb(220, 214, 196));
    for (int i = 0; i < 3; i++)                          // pleats
        drawLine(-w + 1 + i * 4, 37, -w + i * 4, 24, 1, rgb(205, 198, 178));
    drawRect(-w - 1, 36, 2 * w + 2, 4, PAL_GOLD);        // belt
    drawRect(-2, 35, 5, 6, PAL_LAPIS);                   // belt buckle

    // torso + a leather chest strap
    drawRect(-w, 40, 2 * w, 20, PAL_SKIN);
    drawLine(-w + 1, 41, w - 1, 57, 3, PAL_WOOD);

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
    drawRect(4, 72.5f, 3, 0.8f, PAL_BLACK);                          // eyebrow
    drawRect(5, 65, 3, 1, rgb(150, 80, 60));                         // mouth
    drawCircle(9, 68, 1.3f, PAL_SKIN);                               // nose
    drawRect(1, 59, 3, 5, PAL_LAPIS);                                // small beard
}

// small fist
static void drawHand(float x, float y) {
    drawCircle(x, y, 3.2f, PAL_SKIN);
    drawRect(x - 2, y - 1, 4, 1, rgb(200, 150, 100));                // finger line
}

void drawPlayerModel(EntityType type, float x, float y, float s, float animTime) {
    pushMatrix();
    translate(x, y);
    scale(s, s);

    if (type == PLAYER_MELEE) {                       // ---- khopesh soldier
        drawRect(-14, 44, 6, 14, PAL_SKIN);           // back arm
        drawCircle(-11, 44, 4, rgb(40, 60, 120));     // arm band
        drawHand(-11, 43);
        drawGuardBody(false, animTime);
        // bronze scale armour on the chest
        for (int r = 0; r < 3; r++)
            for (int c = 0; c < 4; c++)
                drawCircle(-7 + c * 5, 44 + r * 4, 2.2f, PAL_BRONZE);
        drawNemesHead();

        // sword arm - keeps swinging from the moment he's placed,
        // a lot faster once he's fighting a raider
        float speed = s_fighting ? 10.0f : 4.0f;
        float swing = std::sin(animTime * speed) * 30;
        pushMatrix();
        translate(9, 54);
        rotate(-35 + swing);
        drawRect(0, -3, 14, 6, PAL_SKIN);             // sword arm
        drawRect(4, -3.5f, 3, 7, PAL_GOLD);           // bracelet
        drawHand(14, 0);
        drawRect(12, -2, 8, 4, PAL_WOOD);             // grip
        drawRect(19, -1.5f, 12, 3, PAL_BRONZE);       // straight part of blade
        drawArc(38, 4, 9, -100, 120, 4, PAL_BRONZE, 14);   // curved khopesh blade
        popMatrix();
    }
    else if (type == PLAYER_RANGED) {                 // ---- archer
        drawPolygon({ {-16, 38}, {-10, 36}, {-6, 66}, {-12, 68} }, PAL_WOOD);   // quiver
        drawRect(-15, 50, 8, 2, PAL_GOLD);            // quiver band
        for (int i = 0; i < 3; i++) drawLine(-12 + i * 2, 66, -10 + i * 3, 74, 1.5f, PAL_WHITE);
        drawGuardBody(true, animTime);
        drawCircle(1, 69, 8.5f, PAL_SKIN);
        drawEllipse(-2, 73, 9, 7, PAL_BLACK);         // hair
        drawRect(-11, 62, 5, 10, PAL_BLACK);          // hair at the back
        drawRect(-9, 73, 18, 3, PAL_LAPIS);           // headband
        drawCircle(7, 74, 2, PAL_GOLD);
        drawRect(4, 69, 3, 1.6f, PAL_BLACK);          // eye
        drawRect(4, 71.5f, 3, 0.8f, PAL_BLACK);       // eyebrow
        drawRect(5, 64.5f, 3, 1, rgb(150, 80, 60));   // mouth
        drawRect(-12, 63, 2, 4, PAL_GOLD);            // earring

        drawRect(6, 48, 16, 5, PAL_SKIN);             // arm holding the bow
        drawRect(10, 47.5f, 3, 6, PAL_GOLD);          // bracelet
        float pull = 4 + std::sin(animTime * 3) * 2;  // drawing the string back
        drawArc(18, 50, 22, -70, 70, 3, PAL_WOOD, 16);              // bow
        drawLine(18 + 22 * 0.342f, 50 - 22 * 0.94f, 18 - pull, 50, 1, PAL_WHITE);  // string
        drawLine(18 + 22 * 0.342f, 50 + 22 * 0.94f, 18 - pull, 50, 1, PAL_WHITE);
        drawLine(18 - pull, 50, 44, 50, 1.5f, PAL_WOOD);            // arrow
        drawTriangle(44, 47, 44, 53, 50, 50, PAL_BRONZE);
        drawHand(20, 50);                             // hand on the bow
        drawHand(18 - pull, 50);                      // hand on the string
    }
    else {                                            // ---- shield-bearer (defense)
        scale(1.12f, 1.05f);
        drawLine(-12, 2, -6, 92, 3, PAL_WOOD);        // spear behind
        drawTriangle(-9, 90, -3, 91, -5, 102, PAL_BRONZE);
        drawRect(-10, 86, 6, 2, PAL_GOLD);            // spear ring
        drawGuardBody(false, animTime);
        // leather armour bands on the chest
        for (int i = 0; i < 3; i++) drawRect(-10, 43 + i * 5, 20, 2, PAL_WOOD);
        drawCircle(1, 69, 8.5f, PAL_SKIN);
        drawEllipse(0, 74, 11, 9, PAL_BRONZE);        // helmet
        drawRect(-11, 70, 22, 4, PAL_GOLD);
        drawRect(-11, 62, 5, 10, PAL_BRONZE);         // cheek guard
        drawPolygon({ {-3, 82}, {3, 82}, {0, 92} }, PAL_LAPIS);     // crest
        drawRect(4, 69, 3, 1.6f, PAL_BLACK);          // eye
        drawRect(5, 64.5f, 3, 1, rgb(150, 80, 60));   // mouth
        // tall shield in front with an ankh
        drawRoundedRect(6, 8, 26, 64, 6, PAL_UI_GOLD_DK);
        drawRoundedRect(8, 10, 22, 60, 5, PAL_GOLD);
        drawRoundedRect(11, 13, 16, 54, 4, PAL_LAPIS);
        for (int i = 0; i < 4; i++)                   // studs on the rim
            drawCircle(19, 15 + i * 17, 1.5f, PAL_UI_GOLD_DK);
        drawAnkh(19, 42, 30, PAL_GOLD);
        drawHand(8, 45);                              // hand gripping the shield
    }
    popMatrix();
}

// --------------------------------------------------------------- update
void players_update(float dt) {
    for (Entity& p : g_players) {
        if (!p.alive) continue;
        p.animTime += dt;

        // death - cleanupDead() takes it out of the list later
        if (p.hp <= 0) {
            p.alive = false;
            p.state = STATE_DEAD;
            playSound("death");
            continue;
        }

        // movement - only while walking (combat sets STATE_FIGHTING to stop them)
        if (p.type != PLAYER_DEFENSE && p.state == STATE_WALKING) {
            p.x += SPEED_PLAYER_WALK * dt;
            if (p.x >= PLAYER_STOP_X) {      // reached the end of the lane
                p.x = PLAYER_STOP_X;
                p.state = STATE_IDLE;
            }
        }

        // jump / duck timer
        if (p.isJumping || p.isDucking) {
            p.actionTimer -= dt;
            if (p.actionTimer <= 0) {
                p.isJumping = false;
                p.isDucking = false;
                p.actionTimer = 0;
            }
        }

        // archer keeps shooting from the moment it's placed
        if (p.type == PLAYER_RANGED) {
            p.attackCooldown -= dt;
            if (p.attackCooldown <= 0) {
                Attack a;
                a.lane = p.lane;
                a.x = p.x + 44 * GUARD_SCALE;            // tip of the bow
                a.y = p.y + 50 * GUARD_SCALE;            // chest height
                a.vx = SPEED_PROJECTILE;
                a.damage = DMG_NORMAL;
                a.fromPlayer = true;
                a.height = ATTACK_MID;
                g_attacks.push_back(a);
                p.attackCooldown = ARCHER_COOLDOWN;
                playSound("attack");
            }
        }
    }

    // shield - a guard is safe if a living shield-bearer is in the same
    // lane further right (between the guard and the raiders).
    // checked every frame, so it turns off once he walks past or it dies
    for (Entity& p : g_players) {
        p.isShielded = false;
        if (!p.alive || p.type == PLAYER_DEFENSE) continue;
        for (const Entity& d : g_players) {
            if (d.alive && d.type == PLAYER_DEFENSE && d.lane == p.lane && d.x > p.x) {
                p.isShielded = true;
                break;
            }
        }
    }

    // move our arrows to the right, kill them once they leave the screen
    // (combat sets alive = false when one hits a raider)
    for (Attack& a : g_attacks) {
        if (!a.alive || !a.fromPlayer) continue;
        a.x += a.vx * dt;
        a.animTime += dt;
        if (a.x > SCREEN_W + 40) a.alive = false;
    }

    // TEST KEYS (remove before submitting): act on the lane under the mouse
    int lane = laneAtY(mouseY());
    if (lane < 0) lane = 2;
    if (keyPressed(GLFW_KEY_1)) spawnPlayer(PLAYER_MELEE, lane);
    if (keyPressed(GLFW_KEY_2)) spawnPlayer(PLAYER_RANGED, lane);
    if (keyPressed(GLFW_KEY_3)) spawnPlayer(PLAYER_DEFENSE, lane);
    if (keyPressed(GLFW_KEY_J)) playerJump(lane);
    if (keyPressed(GLFW_KEY_U)) playerDuck(lane);
    if (keyPressed(GLFW_KEY_K)) {
        for (Entity& p : g_players) {
            if (p.alive && p.lane == lane) { p.hp -= 1; break; }
        }
    }
}

// one arrow flying right
static void drawArrow(const Attack& a) {
    drawLine(a.x - 22, a.y, a.x, a.y, 2, PAL_WOOD);                     // shaft
    drawTriangle(a.x, a.y - 4, a.x, a.y + 4, a.x + 8, a.y, PAL_BRONZE); // tip
    drawTriangle(a.x - 22, a.y, a.x - 27, a.y + 4, a.x - 18, a.y, PAL_WHITE);  // feathers
    drawTriangle(a.x - 22, a.y, a.x - 27, a.y - 4, a.x - 18, a.y, PAL_WHITE);
}

void players_draw() {
    for (const Entity& p : g_players) {
        if (!p.alive) continue;

        // jump goes up and comes back down smoothly (half a sine wave)
        float lift = 0;
        if (p.isJumping) {
            float t = 1.0f - p.actionTimer / JUMP_DURATION;   // 0 -> 1
            lift = std::sin(t * 3.14159f) * JUMP_HEIGHT;
        }

        // shadow stays on the ground, gets smaller while in the air
        float shadowW = 22 - lift * 0.2f;
        drawEllipse(p.x, p.y + 1, shadowW, 5, rgb(120, 90, 50, 90));

        s_walking  = (p.state == STATE_WALKING && !p.isJumping && !p.isDucking);
        s_fighting = (p.state == STATE_FIGHTING);

        setTint(p.damageAmount());
        pushMatrix();
        translate(p.x, p.y + lift);
        if (p.isDucking) scale(1.1f, 0.6f);   // squash down to duck
        drawPlayerModel(p.type, 0, 0, GUARD_SCALE, p.animTime);
        popMatrix();
        resetEffects();

        s_walking = false;
        s_fighting = false;

        float barY = p.y + lift + (p.isDucking ? p.height * 0.6f : p.height) + 2;
        drawHealthBar(p.x, barY, p.hp, p.maxHp);
    }

    // our arrows
    for (const Attack& a : g_attacks) {
        if (a.alive && a.fromPlayer) drawArrow(a);
    }
}

