// =====================================================================
//  MEMBER 4 - cutscene.cpp   (animated intro, ~17 seconds)
//
//  Story beats (seconds):
//    0.0 - 4.0   camera pulls back from the desert to reveal the tomb
//    3.0 - 7.0   raiders sneak in from the right
//    6.5 - 8.0   a gold glint flashes on the tomb - the treasure
//    7.0 - 9.5   the axe brute charges forward, shaking his axe
//    9.0 - 11.0  a guard steps out of the tomb door
//   12.0 - 13.2  the slingshot raider throws a stone - the guard DUCKS
//   13.2 - 13.9  the stone smashes into the tomb wall (dust puff)
//   14.5 - 16.0  torches flare, the guard raises his khopesh
//   16.0 - 17.0  fade to black -> gameplay
//
//  Uses drawPlayerModel / drawEnemyModel from Members 2 and 3, so it
//  automatically gets better as their models improve.
//
//  TODO (Member 4, optional):
//   [ ] Speaker portraits next to the subtitles
// =====================================================================
#include "../core/config.h"
#include "../core/shapes.h"
#include "../core/input.h"
#include "../core/ui_style.h"
#include "../core/interfaces.h"
#include <cmath>

static float s_time = 0;
static const float CUTSCENE_LENGTH = 17.0f;

// ---------------------------------------------------------------- timing
static const float T_RAIDERS_IN   = 3.0f;    // raiders start walking in
static const float T_RAIDERS_STOP = 7.0f;
static const float T_GLINT        = 6.5f;    // gold flash on the tomb
static const float T_BRUTE_CHARGE = 7.0f;
static const float T_GUARD_OUT    = 9.0f;    // guard leaves the doorway
static const float T_THROW        = 12.0f;   // stone leaves the sling
static const float STONE_FLIGHT   = 1.2f;
static const float T_DUCK_START   = 12.3f;
static const float T_DUCK_END     = 13.6f;
static const float T_FLARE        = 14.5f;

// ------------------------------------------------------------- positions
static const float GROUND_Y     = 200.0f;    // feet of everyone in the cutscene
static const float GUARD_SCALE  = 1.9f;
static const float GUARD_END_X  = 380.0f;
static const float TOMB_WALL_X  = 150.0f;    // where the stone hits
static const float CARTOUCHE_X  = 76.0f, CARTOUCHE_Y = GRID_BOTTOM + 270.0f;
static const float TORCH_X      = TOMB_RIGHT_X + 14.0f;
static const float TORCH_TOP_Y  = GRID_TOP + 58.0f;
static const float TORCH_LOW_Y  = GRID_BOTTOM + 68.0f;

// --------------------------------------------------------------- dialogue
struct Line { float start; const char* speaker; const char* text1; const char* text2; };
static const Line LINES[] = {
    {  0.0f, "",       "Long ago, the Pharaoh's tomb was a place of eternal rest.",
                       "Its treasures were sealed away... forever." },
    {  3.5f, "",       "But tonight, tomb raiders have come for the gold.", "" },
    {  7.0f, "RAIDER", "There it is! The Pharaoh's gold will be ours before sunrise!", "" },
    {  9.8f, "GUARD",  "These treasures belong to the Pharaoh.", "Not one step closer!" },
    { 12.0f, "RAIDER", "Then fall, guard!", "" },
    { 13.6f, "",       "Duck high attacks with S / Down - jump low ones with W / Up.", "" },
    { 15.0f, "GUARD",  "Guards of the Pharaoh... defend the tomb!", "" },
};

void cutscene_start() { s_time = 0; }

void cutscene_update(float dt) {
    s_time += dt;
    world_update(dt);
    if (keyPressed(GLFW_KEY_ENTER) || keyPressed(GLFW_KEY_SPACE)) s_time = CUTSCENE_LENGTH;
}

// ---------------------------------------------------------------- helpers
static float clamp01(float v) { return v < 0 ? 0 : (v > 1 ? 1 : v); }
static float smooth(float v) { v = clamp01(v); return v * v * (3 - 2 * v); }   // ease in/out
static float lerp(float a, float b, float k) { return a + (b - a) * k; }

// 0 -> 1 -> 0 bump between two times (used for duck, glint, flare)
static float pulse(float t, float start, float end, float ramp) {
    if (t < start || t > end) return 0;
    return smooth((t - start) / ramp) * smooth((end - t) / ramp);
}

// Slingshot raider position (also where the stone starts)
static float slingerX() {
    float k = smooth((s_time - T_RAIDERS_IN) / (T_RAIDERS_STOP - T_RAIDERS_IN));
    return lerp(1520, 1000, k);   // starts off-screen
}

// Stone position along its arc; returns false if not flying
static bool stonePos(float& x, float& y) {
    float k = (s_time - T_THROW) / STONE_FLIGHT;
    if (k < 0 || k > 1) return false;
    float startX = slingerX() - 10, startY = 330;
    x = lerp(startX, TOMB_WALL_X, k);
    y = lerp(startY, 300, k) + 70 * 4 * k * (1 - k);   // parabola: high in the middle
    return true;
}

static void drawStar(float cx, float cy, float r, Color c) {
    drawPolygon({ {cx, cy + r}, {cx + r * 0.22f, cy + r * 0.22f}, {cx + r, cy},
                  {cx + r * 0.22f, cy - r * 0.22f}, {cx, cy - r},
                  {cx - r * 0.22f, cy - r * 0.22f}, {cx - r, cy},
                  {cx - r * 0.22f, cy + r * 0.22f} }, c);
}

// ----------------------------------------------------------------- scene
static void drawScene() {
    world_drawBackground();
    drawDimOverlay(0.25f, rgb(40, 20, 60));                    // evening tone

    // gold glint on the tomb's cartouche: the treasure the raiders want
    float glint = pulse(s_time, T_GLINT, T_GLINT + 1.5f, 0.35f);
    if (glint > 0) {
        drawCircle(CARTOUCHE_X, CARTOUCHE_Y, 40 * glint, rgb(255, 220, 120, 90));
        pushMatrix();
        translate(CARTOUCHE_X, CARTOUCHE_Y);
        rotate(s_time * 90);
        drawStar(0, 0, 26 * glint, rgb(255, 245, 200));
        popMatrix();
    }

    // torches flare up when the guard calls the defenders
    float flare = smooth((s_time - T_FLARE) / 0.6f);
    if (flare > 0) {
        float flick = 1 + 0.12f * std::sin(s_time * 17);
        drawCircle(TORCH_X, TORCH_TOP_Y, 55 * flare * flick, rgb(255, 160, 50, 70));
        drawCircle(TORCH_X, TORCH_LOW_Y, 55 * flare * flick, rgb(255, 160, 50, 70));
        drawEllipse(TORCH_X, TORCH_TOP_Y + 6, 11 * flare * flick, 20 * flare * flick, PAL_FIRE);
        drawEllipse(TORCH_X, TORCH_LOW_Y + 6, 11 * flare * flick, 20 * flare * flick, PAL_FIRE);
    }

    // ---- raiders sneak in, then the brute charges ahead
    float walkBob = (s_time > T_RAIDERS_IN && s_time < T_RAIDERS_STOP)
                    ? std::fabs(std::sin(s_time * 8)) * 4 : 0;
    float sx = slingerX();
    float charge = smooth((s_time - T_BRUTE_CHARGE) / 1.2f);
    float bruteX = lerp(sx - 130, sx - 330, charge);
    float bruteAnim = s_time * (charge > 0 && charge < 1 ? 3.0f : 1.0f);   // faster axe while charging
    drawEnemyModel(ENEMY_MEDIUM, sx + 120, GROUND_Y - 10 + walkBob, 1.6f, s_time);
    drawEnemyModel(ENEMY_EASY,   sx,       GROUND_Y + 30 + walkBob, 1.6f, s_time);
    drawEnemyModel(ENEMY_HARD,   bruteX,   GROUND_Y + walkBob,      1.8f, bruteAnim);

    // ---- guard steps out of the doorway, ducks the stone, raises khopesh
    float gk = smooth((s_time - T_GUARD_OUT) / 2.0f);
    float gx = lerp(110, GUARD_END_X, gk);
    float guardAlpha = clamp01((s_time - T_GUARD_OUT + 0.4f) / 0.6f);   // emerges from the dark door
    float duck = pulse(s_time, T_DUCK_START, T_DUCK_END, 0.25f);
    float guardAnim = (s_time > T_FLARE) ? s_time : 0;                  // swing only at the end
    if (guardAlpha > 0) {
        setAlpha(guardAlpha);
        pushMatrix();
        translate(gx, GROUND_Y);
        scale(1, 1 - 0.38f * duck);          // duck = squash down toward the feet
        translate(-gx, -GROUND_Y);
        drawPlayerModel(PLAYER_MELEE, gx, GROUND_Y, GUARD_SCALE, guardAnim);
        popMatrix();
        resetEffects();
    }

    // ---- the sling stone
    float stx, sty;
    if (stonePos(stx, sty)) {
        drawCircle(stx + 9, sty - 2, 7, rgb(255, 255, 255, 60));   // motion trail
        drawCircle(stx + 18, sty - 4, 5, rgb(255, 255, 255, 35));
        drawCircle(stx, sty, 10, rgb(110, 102, 92));
        drawCircle(stx - 3, sty + 3, 4, rgb(170, 160, 145));
    }

    // ---- dust puff where the stone hits the tomb wall
    float hit = T_THROW + STONE_FLIGHT;
    float dk = (s_time - hit) / 0.7f;
    if (dk > 0 && dk < 1) {
        setAlpha(1 - dk);
        for (int i = 0; i < 6; i++) {
            float a = i * 60.0f * 3.14159265f / 180.0f;
            drawCircle(TOMB_WALL_X + std::cos(a) * 26 * dk, 300 + std::sin(a) * 20 * dk,
                       8 + 10 * dk, rgb(196, 160, 110));
        }
        resetEffects();
    }
}

// -------------------------------------------------------------- subtitles
static void drawSubtitles() {
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
}

void cutscene_draw() {
    // camera: starts zoomed in on the desert and slowly pulls back
    float camK  = smooth(s_time / 4.0f);
    float zoom  = lerp(1.25f, 1.0f, camK);
    float focX  = lerp(760.0f, SCREEN_W / 2.0f, camK);   // stays inside the 1280-wide world
    float focY  = lerp(430.0f, SCREEN_H / 2.0f, camK);
    pushMatrix();
    translate(SCREEN_W / 2.0f, SCREEN_H / 2.0f);
    scale(zoom, zoom);
    translate(-focX, -focY);
    drawScene();
    popMatrix();

    // letterbox bars + subtitles (not affected by the camera)
    drawRect(0, SCREEN_H - 50, SCREEN_W, 50, PAL_BLACK);
    drawRect(0, 0, SCREEN_W, 120, PAL_BLACK);
    drawSubtitles();

    if (drawButton("SKIP", SCREEN_W - 70, 30, 90, 34)) s_time = CUTSCENE_LENGTH;

    // fade in at the start, fade out at the end
    float fade = std::fmax(clamp01(1 - s_time / 1.0f), clamp01((s_time - (CUTSCENE_LENGTH - 1)) / 1.0f));
    if (fade > 0) drawDimOverlay(fade, PAL_BLACK);
}

bool cutscene_finished() { return s_time >= CUTSCENE_LENGTH; }
