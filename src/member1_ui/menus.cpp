// =====================================================================
//  MEMBER 1 - menus.cpp
//  Main menu, pause menu, win/lose menu, win/lose detection.
//
//  Starter version: themed menus like the reference picture (navy + gold,
//  parchment pause scroll, victory / defeat screens). Buttons already work
//  with the mouse. Member 1 owns these and can polish them further.
//
//  TODO (Member 1):
//   [ ] Polish: hover sound (playSound("select")), small animations
//   [ ] Check every menu option required by the brief works correctly
//   [ ] Restart must fully reset everything (resetGame does the shared part)
// =====================================================================
#include "../core/config.h"
#include "../core/shapes.h"
#include "../core/input.h"
#include "../core/ui_style.h"
#include "../core/game_state.h"
#include "../core/interfaces.h"
#include <cmath>

static float s_time = 0;

void ui_init() {}
void ui_reset() {}

// Starts a new game from the beginning (cutscene first).
static void startNewGame() {
    resetGame();
    cutscene_start();
    setScreen(SCREEN_CUTSCENE);
}

static void restartGame() {
    resetGame();
    setScreen(SCREEN_PLAYING);
}

// ------------------------------------------------------------ main menu
void mainMenu_update(float dt) {
    s_time += dt;
    world_update(dt);                                  // keeps torches / palms alive behind the menu
    if (keyPressed(GLFW_KEY_ENTER)) startNewGame();
    if (keyPressed(GLFW_KEY_ESCAPE)) setScreen(SCREEN_QUIT);
}

void mainMenu_draw() {
    world_drawBackground();
    drawDimOverlay(0.45f, rgb(20, 10, 30));
    // two guard "statues" either side of the panel
    drawPlayerModel(PLAYER_DEFENSE, 330, 150, 3.0f, 0);
    pushMatrix(); translate(950, 0); scale(-1, 1); translate(-950, 0);
    drawPlayerModel(PLAYER_MELEE, 950, 150, 3.0f, 0);
    popMatrix();

    float pw = 420, ph = 420, px = SCREEN_W / 2.0f - pw / 2, py = 140;
    drawPanel(px, py, pw, ph);
    drawAnkh(px + 30, py + ph - 34, 30);
    drawAnkh(px + pw - 30, py + ph - 34, 30);
    float bob = std::sin(s_time * 2) * 3;
    drawTextCentered("GUARDIANS", SCREEN_W / 2.0f + 2, py + ph - 40 + bob - 2, 46, PAL_UI_GOLD_DK);  // shadow
    drawTextCentered("GUARDIANS", SCREEN_W / 2.0f, py + ph - 40 + bob, 46, PAL_UI_GOLD);
    drawTextCentered("OF THE PHARAOH", SCREEN_W / 2.0f, py + ph - 104, 22, PAL_PARCHMENT);
    drawLine(px + 60, py + ph - 140, px + pw - 60, py + ph - 140, 2, PAL_UI_GOLD_DK);

    float cx = SCREEN_W / 2.0f;
    if (drawButton("PLAY", cx, py + 210, 250, 54))     startNewGame();
    if (drawButton("SETTINGS", cx, py + 140, 250, 54)) setScreen(SCREEN_SOUND_SETTINGS);
    if (drawButton("QUIT", cx, py + 70, 250, 54))      setScreen(SCREEN_QUIT);
}

// ----------------------------------------------------------- pause menu
void pauseMenu_update(float dt) {
    if (keyPressed(GLFW_KEY_ESCAPE)) setScreen(SCREEN_PLAYING);
}

void pauseMenu_draw() {
    drawDimOverlay(0.5f);
    float pw = 340, ph = 400, px = SCREEN_W / 2.0f - pw / 2, py = 170;
    drawParchmentPanel(px, py, pw, ph);
    drawTextCentered("PAUSED", SCREEN_W / 2.0f, py + ph - 36, 40, rgb(110, 70, 30));
    float cx = SCREEN_W / 2.0f;
    if (drawButton("RESUME", cx, py + 250, 200, 46))    setScreen(SCREEN_PLAYING);
    if (drawButton("RESTART", cx, py + 186, 200, 46))   restartGame();
    if (drawButton("MAIN MENU", cx, py + 122, 200, 46)) setScreen(SCREEN_MAIN_MENU);
    if (drawButton("QUIT", cx, py + 58, 200, 46))       setScreen(SCREEN_QUIT);
}

// --------------------------------------------------------- win/lose menu
void endMenu_update(float dt) {
    s_time += dt;
}

void endMenu_draw() {
    bool won = (g_screen == SCREEN_WIN);
    drawDimOverlay(0.55f, won ? rgb(60, 40, 0) : rgb(70, 0, 0));
    float pw = 460, ph = 330, px = SCREEN_W / 2.0f - pw / 2, py = 200;
    drawPanel(px, py, pw, ph, won ? rgb(40, 32, 18) : rgb(40, 12, 12), won ? PAL_UI_GOLD : rgb(200, 60, 50));
    float pulse = 1 + 0.04f * std::sin(s_time * 4);
    if (won) {
        drawCoinIcon(SCREEN_W / 2.0f - 185, py + ph - 70, 22 * pulse);
        drawCoinIcon(SCREEN_W / 2.0f + 185, py + ph - 70, 22 * pulse);
        drawTextCentered("VICTORY!", SCREEN_W / 2.0f, py + ph - 46, 46 * pulse, PAL_UI_GOLD);
        drawTextCentered("The Pharaoh's tomb is safe.", SCREEN_W / 2.0f, py + ph - 130, 18, PAL_PARCHMENT);
    } else {
        drawTextCentered("DEFEAT...", SCREEN_W / 2.0f, py + ph - 46, 52, rgb(220, 60, 50));
        drawTextCentered("The raiders have broken into the tomb.", SCREEN_W / 2.0f, py + ph - 130, 18, PAL_PARCHMENT);
    }
    float cx = SCREEN_W / 2.0f;
    if (drawButton(won ? "PLAY AGAIN" : "RETRY", cx, py + 120, 240, 50)) restartGame();
    if (drawButton("MAIN MENU", cx, py + 56, 240, 50))                   setScreen(SCREEN_MAIN_MENU);
}

// ------------------------------------------------------ win/lose checks
void winLose_update(float dt) {
    if (enemyReachedTomb()) { setScreen(SCREEN_LOSE); playSound("lose"); }
    else if (allWavesCleared()) { setScreen(SCREEN_WIN); playSound("win"); }

    // TEST KEYS (remove before submitting)
    if (keyPressed(GLFW_KEY_F9))  setScreen(SCREEN_WIN);
    if (keyPressed(GLFW_KEY_F10)) setScreen(SCREEN_LOSE);
}
