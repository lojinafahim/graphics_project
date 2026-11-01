#include "../core/config.h"
#include "../core/shapes.h"
#include "../core/input.h"
#include "../core/ui_style.h"
#include "../core/game_state.h"
#include "../core/interfaces.h"

static bool s_selected[NUM_LANES] = {};
static int s_hoverLane = -1;
static float s_hintTimer = 0;   

struct Card { EntityType type; int cost; const char* name; };
static const Card CARDS[3] = {
    { PLAYER_MELEE,   COST_MELEE,   "SOLDIER" },
    { PLAYER_RANGED,  COST_RANGED,  "ARCHER"  },
    { PLAYER_DEFENSE, COST_DEFENSE, "SHIELD"  },
};
static const float CARD_W = 100, CARD_H = 94, CARD_Y = 8;
static float cardX(int i) { return 18 + i * (CARD_W + 14); }

bool isLaneSelected(int lane) {
    return lane >= 0 && lane < NUM_LANES && s_selected[lane];
}

void grid_update(float dt) {
    s_hoverLane = laneAtY(mouseY());
    if (mouseX() < TOMB_RIGHT_X) s_hoverLane = -1;

    if (mouseClicked() && s_hoverLane != -1) {
        s_selected[s_hoverLane] = !s_selected[s_hoverLane];
    }

    bool jump = keyPressed(GLFW_KEY_W) || keyPressed(GLFW_KEY_UP);
    bool duck = keyPressed(GLFW_KEY_S) || keyPressed(GLFW_KEY_DOWN);

    for (int lane = 0; lane < NUM_LANES; lane++) {
        if (!s_selected[lane]) continue;
        if (jump) playerJump(lane);
        if (duck) playerDuck(lane);
    }
}

void grid_draw() {
    for (int lane = 0; lane < NUM_LANES; lane++) {
        float y = laneBottomY(lane);
        if (s_selected[lane]) {
            drawRect(TOMB_RIGHT_X, y, SCREEN_W - TOMB_RIGHT_X, LANE_HEIGHT, rgb(226, 180, 76, 70));
            drawRectOutline(TOMB_RIGHT_X, y, SCREEN_W - TOMB_RIGHT_X, LANE_HEIGHT, 3, PAL_UI_GOLD);
        }
        if (lane == s_hoverLane) {
            drawRect(TOMB_RIGHT_X, y, SCREEN_W - TOMB_RIGHT_X, LANE_HEIGHT, rgb(255, 255, 255, 40));
        }
    }
}

void hud_update(float dt) {
    // pause button (top-left badge)
    if (mouseClicked() && mouseInRect(16, SCREEN_H - 46, 34, 34)) setScreen(SCREEN_PAUSED);
        if (s_hintTimer > 0) s_hintTimer -= dt;

    for (int i = 0; i < 3; i++) {
        const Card& c = CARDS[i];
        bool unlocked = canAfford(c.cost);
        if (!unlocked || !mouseClicked() || !mouseInRect(cardX(i), CARD_Y, CARD_W, CARD_H)) continue;

        bool anySelected = false;
        for (int lane = 0; lane < NUM_LANES; lane++) {
            if (!s_selected[lane]) continue;
            anySelected = true;
            if (spendCoins(c.cost)) spawnPlayer(c.type, lane);
        }
        if (!anySelected) s_hintTimer = 2.0f;
    }

    // TEST KEYS (remove before submitting)
    if (keyPressed(GLFW_KEY_EQUAL)) addCoins(10);     // '+' key
    if (keyPressed(GLFW_KEY_MINUS)) spendCoins(10);
}

static void drawCard(int i) {
    const Card& c = CARDS[i];
    float x = cardX(i), y = CARD_Y;
    bool unlocked = canAfford(c.cost);
    bool hover = unlocked && mouseInRect(x, y, CARD_W, CARD_H);
    if (hover) y += 4;                                           // lift on hover

    if (!unlocked) setGrayscale(true);
    drawPanel(x, y, CARD_W, CARD_H, hover ? PAL_UI_NAVY_LT : rgb(26, 44, 80),
              hover ? PAL_WHITE : PAL_UI_GOLD);
    drawGradientRect(x + 6, y + 26, CARD_W - 12, CARD_H - 32, rgb(70, 90, 130), rgb(200, 150, 100), 10);
    drawPlayerModel(c.type, x + CARD_W / 2 - 4, y + 28, 0.68f, 0);
    // cost strip
    drawRect(x + 6, y + 5, CARD_W - 12, 20, rgb(14, 22, 40));
    drawCoinIcon(x + 22, y + 15, 8);
    drawText(std::to_string(c.cost), x + 36, y + 22, 14, PAL_UI_GOLD);
    resetEffects();
    if (!unlocked) {
        drawRoundedRect(x, y, CARD_W, CARD_H, 8, rgb(0, 0, 0, 110));
        drawTextCentered("LOCKED", x + CARD_W / 2, y + CARD_H / 2 + 6, 12, rgb(200, 200, 200));
    }
}

void hud_draw() {
    // bottom bar
    drawGradientRect(0, 0, SCREEN_W, HUD_TOP, rgb(10, 18, 34), PAL_UI_NAVY, 10);
    drawRect(0, HUD_TOP - 3, SCREEN_W, 3, PAL_UI_GOLD);
    for (int i = 0; i < 3; i++) drawCard(i);

    // help text in the middle
    drawText("CLICK lanes to select   |   W / S  jump / duck   |   ESC pause",
             400, 64, 13, rgb(170, 180, 205));

    // coin counter 
    drawPanel(SCREEN_W - 200, SCREEN_H - 58, 180, 50);
    drawCoinIcon(SCREEN_W - 170, SCREEN_H - 33, 16);
    drawText(std::to_string(g_coins), SCREEN_W - 140, SCREEN_H - 20, 26, PAL_UI_GOLD);

    // wave badge + pause button (top-left)
    drawPanel(12, SCREEN_H - 50, 210, 42);
    drawRoundedRect(18, SCREEN_H - 44, 30, 30, 5, PAL_UI_NAVY_LT);
    drawRect(26, SCREEN_H - 38, 5, 18, PAL_WHITE);
    drawRect(35, SCREEN_H - 38, 5, 18, PAL_WHITE);
    drawText("Wave " + std::to_string(currentWave()) + " / " + std::to_string(NUM_WAVES),
             60, SCREEN_H - 21, 16, PAL_WHITE);

    
    if (s_hintTimer > 0) {
        drawTextCentered("Select a lane first!", SCREEN_W / 2.0f, HUD_TOP + 40, 22, PAL_SUN);
    }


}
