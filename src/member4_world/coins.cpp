// =====================================================================
//  MEMBER 4 - coins.cpp   (treasure collectibles: gold ankh coins)
//
//  TODO (Member 4):
//   [ ] More detailed coin model (rim notches, shine that moves)
//   [ ] coins_update: if a player in the same lane touches it ->
//         addCoins(c.value), c.alive = false, playSound("coin")
//   [ ] Optional: small sparkle when collected
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

void spawnCoin(int lane, float x) {
    Collectible c;
    c.lane  = lane;
    c.x     = x;
    c.y     = laneBottomY(lane) + 22;
    c.value = COIN_DROP_VALUE;
    g_collectibles.push_back(c);
}

void coins_reset() {}

void coins_update(float dt) {
    for (Collectible& c : g_collectibles) c.animTime += dt;
    // TODO: pickup

    // TEST KEYS (remove before submitting)
    int lane = laneAtY(mouseY());
    if (keyPressed(GLFW_KEY_C)) spawnCoin(lane < 0 ? 2 : lane, mouseX());
}

void coins_draw() {
    for (const Collectible& c : g_collectibles) {
        float bob = std::sin(c.animTime * 3) * 3;
        float spin = std::fabs(std::cos(c.animTime * 2));        // fake 3D spin
        drawEllipse(c.x, c.y - 14, 10, 3, rgb(120, 90, 50, 90)); // shadow
        pushMatrix();
        translate(c.x, c.y + bob);
        scale(0.35f + 0.65f * spin, 1);
        drawCoinIcon(0, 0, 13);
        popMatrix();
    }
}
