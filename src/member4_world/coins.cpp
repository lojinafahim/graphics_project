// =====================================================================
//  MEMBER 4 - coins.cpp   (treasure collectibles: gold ankh coins)
//
//  TODO (Member 4):
//   [x] More detailed coin model (rim notches, shine that moves)
//   [x] coins_update: if a player in the same lane touches it ->
//         addCoins(c.value), c.alive = false, playSound("coin")
//   [x] Small sparkle + "+5" when collected
//
//  Test alone: hover over a lane and press C to drop a coin there.
// =====================================================================
#include "../core/config.h"
#include "../core/shapes.h"
#include "../core/input.h"
#include "../core/ui_style.h"
#include "../core/game_state.h"
#include "../core/interfaces.h"
#include <cmath>
#include <string>
#include <vector>

static const float COIN_RADIUS     = 13.0f;
static const float SPARKLE_SECONDS = 1.4f;   // how long the collect burst lasts

// A short gold burst shown where a coin was collected.
struct Sparkle {
    float x, y;
    int   value;
    float t = 0;      // seconds since it started
};
static std::vector<Sparkle> s_sparkles;

void spawnCoin(int lane, float x) {
    Collectible c;
    c.lane  = lane;
    c.x     = x;
    c.y     = laneBottomY(lane) + 22;
    c.value = COIN_DROP_VALUE;
    g_collectibles.push_back(c);
}

void coins_reset() {
    s_sparkles.clear();
}

void coins_update(float dt) {
    for (Collectible& c : g_collectibles) c.animTime += dt;

    // Pickup: a guard in the same lane that overlaps the coin collects it
    for (Collectible& c : g_collectibles) {
        if (!c.alive) continue;
        for (const Entity& p : g_players) {
            if (!p.alive || p.lane != c.lane) continue;
            float touchDist = p.width / 2 + COIN_RADIUS;    // 1D AABB overlap
            if (std::fabs(p.x - c.x) < touchDist) {
                addCoins(c.value);
                c.alive = false;                            // cleanupDead() removes it
                playSound("coin");
                s_sparkles.push_back({ c.x, c.y + 64, c.value });   // above the guard's head (coins are drawn behind guards)
                break;                                      // this coin is taken
            }
        }
    }

    // Age the sparkles and drop finished ones
    for (Sparkle& s : s_sparkles) s.t += dt;
    for (size_t i = 0; i < s_sparkles.size(); ) {
        if (s_sparkles[i].t >= SPARKLE_SECONDS) s_sparkles.erase(s_sparkles.begin() + i);
        else i++;
    }

    // TEST KEYS (remove before submitting)
    int lane = laneAtY(mouseY());
    if (keyPressed(GLFW_KEY_C)) spawnCoin(lane < 0 ? 2 : lane, mouseX());
}

// ------------------------------------------------------------ coin model
// Drawn around (0,0). 'shine' (0..1) moves the light streak across the face.
static void drawTreasureCoin(float r, float shine) {
    const Color rimDark  = rgb(150, 104, 30);
    const Color rim      = rgb(214, 160, 46);
    const Color face     = rgb(240, 196, 70);
    const Color faceDeep = rgb(222, 172, 52);

    drawCircle(0, 0, r, rimDark);                       // outer edge
    for (int i = 0; i < 16; i++) {                      // milled notches on the rim
        pushMatrix();
        rotate(i * 22.5f);
        drawRect(r - 2.6f, -0.9f, 2.6f, 1.8f, rim);
        popMatrix();
    }
    drawCircle(0, 0, r * 0.82f, face);                  // gold face
    drawArc(0, 0, r * 0.66f, 0, 360, 1.2f, faceDeep, 28);   // inner ring
    drawAnkh(0, -0.5f, r * 1.0f, rimDark);              // stamped ankh

    // light streak sliding across the face
    float sx = -r * 0.6f + shine * r * 1.2f;
    pushMatrix();
    translate(sx, 0);
    rotate(-25);
    drawEllipse(0, 0, r * 0.12f, r * 0.62f, rgb(255, 245, 200, 150));
    popMatrix();
    drawCircle(-r * 0.35f, r * 0.38f, r * 0.14f, rgb(255, 250, 220, 200));  // small glint
}

static void drawSparkle(const Sparkle& s) {
    float k = s.t / SPARKLE_SECONDS;                    // 0 -> 1 over its life
    float fade = 1.0f - k;
    setAlpha(fade);

    // 8 gold rays shooting outward
    float inner = 6 + 16 * k, outer = 14 + 28 * k;
    for (int i = 0; i < 8; i++) {
        float a = (i * 45.0f + 22.5f) * 3.14159265f / 180.0f;
        float thick = (i % 2 ? 2.5f : 3.5f) * (1.0f - 0.4f * k);
        drawLine(s.x + inner * std::cos(a), s.y + inner * std::sin(a),
                 s.x + outer * std::cos(a), s.y + outer * std::sin(a), thick, PAL_GOLD);
    }
    drawCircle(s.x, s.y, 9 * (1.0f - k), rgb(255, 240, 170));   // flash in the middle

    // "+5" floating up
    std::string label = "+" + std::to_string(s.value);
    float ty = s.y + 34 + 26 * k;
    drawTextCentered(label, s.x + 1.5f, ty - 1.5f, 22, rgb(90, 55, 10));   // shadow
    drawTextCentered(label, s.x, ty, 22, PAL_GOLD);
    resetEffects();
}

void coins_draw() {
    for (const Collectible& c : g_collectibles) {
        float bob   = std::sin(c.animTime * 3) * 3;
        float spin  = std::fabs(std::cos(c.animTime * 2));            // fake 3D spin
        float shine = std::fmod(c.animTime * 0.8f, 1.0f);
        drawEllipse(c.x, c.y - 14, 10 * (0.5f + 0.5f * spin), 3, rgb(120, 90, 50, 90));  // shadow
        pushMatrix();
        translate(c.x, c.y + bob);
        scale(0.25f + 0.75f * spin, 1);
        drawTreasureCoin(COIN_RADIUS, shine);
        popMatrix();
    }
    for (const Sparkle& s : s_sparkles) drawSparkle(s);
}
