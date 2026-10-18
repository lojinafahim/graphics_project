// =====================================================================
//  MEMBER 4 - cutscene.cpp   (10-20 second animated intro)
//
//  Starter version: dusk at the tomb, raiders walk in from the right, a
//  guard steps out of the tomb, subtitles tell the story, then it fades
//  into the game. Member 4 owns it and should make it richer:
//
//  TODO (Member 4):
//   [ ] More animation: brute points at the tomb, a sling stone flies and the
//       guard DUCKS it (shows the duck mechanic), torches flare, camera pan
//   [ ] Check the length stays 10-20 seconds
//   [ ] Optional: speaker portraits next to the subtitles
//
//  Uses drawPlayerModel / drawEnemyModel from Members 2 and 3, so it
//  automatically gets better as their models improve.
// =====================================================================
#include "../core/config.h"
#include "../core/shapes.h"
#include "../core/input.h"
#include "../core/ui_style.h"
#include "../core/interfaces.h"
#include <cmath>

static float s_time = 0;
static const float CUTSCENE_LENGTH = 14.0f;

struct Line { float start; const char* speaker; const char* text1; const char* text2; };
static const Line LINES[] = {
    { 0.0f, "",          "Long ago, the Pharaoh's tomb was a place of eternal rest.",
                         "But tonight, tomb raiders come for the treasures within..." },
    { 5.0f, "RAIDER",    "The Pharaoh's gold will be ours before sunrise!", "" },
    { 8.5f, "GUARD",     "These treasures belong to the Pharaoh.", "Not one step closer!" },
    { 11.5f, "",         "Defend the tomb... at all costs.", "" },
};

void cutscene_start() { s_time = 0; }

void cutscene_update(float dt) {
    s_time += dt;
    world_update(dt);
    if (keyPressed(GLFW_KEY_ENTER) || keyPressed(GLFW_KEY_SPACE)) s_time = CUTSCENE_LENGTH;
}

static float clamp01(float v) { return v < 0 ? 0 : (v > 1 ? 1 : v); }

void cutscene_draw() {
    world_drawBackground();
    drawDimOverlay(0.25f, rgb(40, 20, 60));                    // evening tone

    // raiders sneak in from the right and stop
    float rk = clamp01(s_time / 6.0f);
    float rx = 1400 - 520 * rk;
    drawEnemyModel(ENEMY_HARD,   rx,       200, 1.8f, s_time);
    drawEnemyModel(ENEMY_EASY,   rx + 140, 230, 1.6f, s_time);
    drawEnemyModel(ENEMY_MEDIUM, rx + 250, 190, 1.6f, s_time);

    // guard steps out of the tomb at ~6.5 s
    float gk = clamp01((s_time - 6.5f) / 2.0f);
    float gx = 110 + 260 * gk;
    drawPlayerModel(PLAYER_MELEE, gx, 200, 1.9f, gk < 1 ? 0 : s_time);

    // letterbox bars
    drawRect(0, SCREEN_H - 50, SCREEN_W, 50, PAL_BLACK);
    drawRect(0, 0, SCREEN_W, 120, PAL_BLACK);

    // subtitles
    const Line* cur = &LINES[0];
    for (const Line& l : LINES) if (s_time >= l.start) cur = &l;
    float y = 92;
    if (cur->speaker[0]) {
        drawTextCentered(cur->speaker, SCREEN_W / 2.0f, y, 14,
                         cur->speaker[0] == 'G' ? PAL_UI_GOLD : rgb(220, 90, 70));
        y -= 24;
    }
    drawTextCentered(cur->text1, SCREEN_W / 2.0f, y, 18, PAL_WHITE);
    if (cur->text2[0]) drawTextCentered(cur->text2, SCREEN_W / 2.0f, y - 26, 18, PAL_WHITE);

    if (drawButton("SKIP", SCREEN_W - 70, 30, 90, 34)) s_time = CUTSCENE_LENGTH;

    // fade in at the start, fade out at the end
    float fade = std::fmax(clamp01(1 - s_time / 1.0f), clamp01((s_time - (CUTSCENE_LENGTH - 1)) / 1.0f));
    if (fade > 0) drawDimOverlay(fade, PAL_BLACK);
}

bool cutscene_finished() { return s_time >= CUTSCENE_LENGTH; }
