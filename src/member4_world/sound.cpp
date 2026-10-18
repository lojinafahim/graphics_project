// =====================================================================
//  MEMBER 4 - sound.cpp   (BONUS - optional)
//
//  Everyone can already call playSound("attack") etc. - right now these
//  functions do nothing, so the game works with or without sound.
//
//  TODO (Member 4, if the team does the bonus):
//   [ ] Add miniaudio (single header: miniaudio.h) to src/core or external/
//   [ ] Background music + at least 4 effects:
//         "attack", "death", "select", "place", "coin", "win", "lose"
//   [ ] Sound Settings screen (from main menu): volume up / down / mute,
//       and it must change the sounds that are playing
// =====================================================================
#include "../core/config.h"
#include "../core/shapes.h"
#include "../core/input.h"
#include "../core/ui_style.h"
#include "../core/game_state.h"
#include "../core/interfaces.h"

static float s_volume = 0.8f;
static bool  s_muted = false;

void sound_init() {}
void playSound(const char* name) {}
void playMusic(const char* name) {}
void stopMusic() {}
void setVolume(float v) { s_volume = v < 0 ? 0 : (v > 1 ? 1 : v); }
float getVolume() { return s_muted ? 0.0f : s_volume; }
void toggleMute() { s_muted = !s_muted; }

void soundSettings_update(float dt) {
    world_update(dt);
    if (keyPressed(GLFW_KEY_UP))     setVolume(s_volume + 0.1f);
    if (keyPressed(GLFW_KEY_DOWN))   setVolume(s_volume - 0.1f);
    if (keyPressed(GLFW_KEY_ESCAPE)) setScreen(SCREEN_MAIN_MENU);
}

void soundSettings_draw() {
    world_drawBackground();
    drawDimOverlay(0.5f, rgb(20, 10, 30));
    float pw = 440, ph = 360, px = SCREEN_W / 2.0f - pw / 2, py = 180;
    drawPanel(px, py, pw, ph);
    float cx = SCREEN_W / 2.0f;
    drawTextCentered("SOUND", cx, py + ph - 36, 40, PAL_UI_GOLD);

    // volume bar
    float bw = 300, bx = cx - bw / 2, by = py + 210;
    drawRoundedRect(bx - 3, by - 3, bw + 6, 24, 6, PAL_UI_GOLD_DK);
    drawRoundedRect(bx, by, bw, 18, 5, rgb(10, 16, 30));
    if (getVolume() > 0) drawRoundedRect(bx, by, bw * getVolume(), 18, 5, PAL_UI_GOLD);
    int pct = (int)(getVolume() * 100 + 0.5f);
    drawTextCentered(s_muted ? "MUTED" : ("Volume " + std::to_string(pct) + "%"), cx, by + 50, 16, PAL_PARCHMENT);

    if (drawButton("-", cx - 110, py + 150, 70, 46)) setVolume(s_volume - 0.1f);
    if (drawButton(s_muted ? "UNMUTE" : "MUTE", cx, py + 150, 130, 46)) toggleMute();
    if (drawButton("+", cx + 110, py + 150, 70, 46)) setVolume(s_volume + 0.1f);
    if (drawButton("BACK", cx, py + 70, 200, 46)) setScreen(SCREEN_MAIN_MENU);
}
