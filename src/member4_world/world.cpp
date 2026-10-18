// =====================================================================
//  MEMBER 4 - world.cpp   (background, tomb, environment)
//
//  Starter version follows the reference picture: dusk sky, distant
//  pyramids, dunes, obelisks, palms, broken columns, torches, and the
//  tomb entrance on the left. It is a BASIC version - Member 4 owns it
//  and should add detail (hieroglyph carvings, statues, cracks, shadows,
//  more palm fronds, pottery, birds...).
//
//  TODO (Member 4):
//   [ ] Add more detail to every element (brief: no single-primitive objects,
//       realistic shapes)
//   [ ] Animate: torch flames (already flicker), palm sway, sun glow, birds
//   [ ] Optional spare environment element: pottery jars / hieroglyph wall / statue
//   [ ] Keep lanes readable: decoration lives above the grid (y > GRID_TOP),
//       on the tomb, or very low-contrast inside lanes
// =====================================================================
#include "../core/config.h"
#include "../core/shapes.h"
#include "../core/game_state.h"
#include "../core/interfaces.h"
#include <cmath>

static float s_time = 0;

void world_init() {}

void world_update(float dt) {
    s_time += dt;
}

// ------------------------------------------------------------ elements
static void drawPyramid(float cx, float baseY, float w, float h) {
    drawTriangle(cx - w / 2, baseY, cx, baseY, cx, baseY + h, rgb(214, 150, 100));   // lit side
    drawTriangle(cx, baseY, cx + w / 2, baseY, cx, baseY + h, rgb(168, 104, 84));    // shade side
    for (int i = 1; i < 6; i++) {                                                     // block rows
        float yy = baseY + h * i / 6.0f, half = w / 2 * (1 - i / 6.0f);
        drawLine(cx - half, yy, cx + half, yy, 1, rgb(150, 95, 75, 120));
    }
}

static void drawObelisk(float cx, float baseY, float h) {
    float wBase = h * 0.13f, wTop = h * 0.08f;
    drawRect(cx - wBase * 0.8f, baseY, wBase * 1.6f, h * 0.06f, PAL_STONE_DARK);      // plinth
    float y0 = baseY + h * 0.06f, y1 = baseY + h * 0.88f;
    drawPolygon({ {cx - wBase / 2, y0}, {cx + wBase / 2, y0},
                  {cx + wTop / 2, y1}, {cx - wTop / 2, y1} }, rgb(204, 160, 110));
    drawPolygon({ {cx, y0}, {cx + wBase / 2, y0},
                  {cx + wTop / 2, y1}, {cx, y1} }, rgb(176, 130, 92));                  // shaded half
    drawTriangle(cx - wTop / 2, y1, cx + wTop / 2, y1, cx, baseY + h, PAL_GOLD);     // gold tip
    for (int i = 0; i < 7; i++) {                                                     // hieroglyphs
        float yy = y0 + 12 + i * (y1 - y0 - 24) / 7;
        drawRect(cx - 3, yy, 6, 3, rgb(130, 95, 65));
        if (i % 2) drawCircle(cx, yy + 6, 2, rgb(130, 95, 65));
    }
}

static void drawPalm(float x, float baseY, float h, float swayPhase) {
    float sway = std::sin(s_time * 1.3f + swayPhase) * 3.0f;
    // curved trunk: stacked segments
    int segs = 9;
    float px = x, py = baseY;
    for (int i = 0; i < segs; i++) {
        float t = (float)(i + 1) / segs;
        float nx = x + std::sin(t * 1.4f) * h * 0.18f + sway * t;
        float ny = baseY + h * t;
        drawLine(px, py, nx, ny, 9 - i * 0.5f, i % 2 ? rgb(120, 82, 50) : rgb(98, 66, 40));
        px = nx; py = ny;
    }
    // fronds
    for (int i = 0; i < 7; i++) {
        float ang = -20 + i * 35 + std::sin(s_time * 1.6f + i + swayPhase) * 4;
        pushMatrix();
        translate(px, py);
        rotate(ang);
        drawEllipse(h * 0.2f, 0, h * 0.22f, 6, i % 2 ? PAL_PALM_GREEN : rgb(44, 100, 48));
        drawTriangle(h * 0.35f, -5, h * 0.48f, -14, h * 0.38f, 2, rgb(44, 100, 48));
        popMatrix();
    }
    drawCircle(px - 3, py - 4, 4, rgb(110, 70, 30));   // dates
    drawCircle(px + 4, py - 5, 4, rgb(110, 70, 30));
}

static void drawBrokenColumn(float cx, float baseY, float h) {
    float w = 26;
    drawRect(cx - w / 2 - 6, baseY, w + 12, 8, PAL_STONE_DARK);                       // base
    drawRect(cx - w / 2, baseY + 8, w, h - 8, PAL_STONE);
    for (int i = 0; i < 3; i++)                                                       // flutes
        drawRect(cx - w / 2 + 5 + i * 7, baseY + 10, 2, h - 14, PAL_STONE_DARK);
    drawPolygon({ {cx - w / 2, baseY + h}, {cx + w / 2, baseY + h},
                  {cx + w / 2, baseY + h + 6}, {cx + 2, baseY + h + 16},
                  {cx - w / 2, baseY + h + 9} }, PAL_STONE);                           // broken top
    drawRect(cx + w / 2 + 6, baseY, 16, 10, PAL_STONE);                               // fallen drum
}

static void drawTorch(float cx, float baseY) {
    drawRect(cx - 3, baseY, 6, 46, PAL_WOOD);
    drawPolygon({ {cx - 11, baseY + 46}, {cx + 11, baseY + 46},
                  {cx + 7, baseY + 56}, {cx - 7, baseY + 56} }, PAL_BRONZE);
    float f = 1.0f + 0.15f * std::sin(s_time * 13 + cx) + 0.08f * std::sin(s_time * 7.3f + cx);
    drawCircle(cx, baseY + 66, 20 * f, rgb(255, 170, 60, 50));                       // glow
    drawEllipse(cx, baseY + 64, 8 * f, 13 * f, PAL_FIRE);
    drawEllipse(cx, baseY + 61, 4.5f * f, 7 * f, rgb(255, 220, 90));
}

static void drawTomb() {
    float top = GRID_TOP + 120;
    // sloped pylon walls
    drawPolygon({ {0, GRID_BOTTOM - 10}, {TOMB_RIGHT_X, GRID_BOTTOM - 10},
                  {TOMB_RIGHT_X - 16, top}, {0, top} }, rgb(196, 150, 100));
    drawPolygon({ {TOMB_RIGHT_X - 40, GRID_BOTTOM - 10}, {TOMB_RIGHT_X, GRID_BOTTOM - 10},
                  {TOMB_RIGHT_X - 16, top}, {TOMB_RIGHT_X - 50, top} }, rgb(160, 118, 82));
    for (float yy = GRID_BOTTOM + 20; yy < top; yy += 34)                             // stone courses
        drawLine(0, yy, TOMB_RIGHT_X - 16 * (yy - GRID_BOTTOM) / (top - GRID_BOTTOM), yy, 1.5f,
                 rgb(140, 100, 70, 160));
    // cornice + winged sun disk
    drawRect(-4, top, TOMB_RIGHT_X - 8, 14, PAL_STONE_DARK);
    drawRect(-4, top + 14, TOMB_RIGHT_X - 4, 8, rgb(196, 150, 100));
    drawCircle(62, top - 22, 10, PAL_GOLD);
    drawPolygon({ {50, top - 20}, {18, top - 12}, {22, top - 24}, {50, top - 26} }, PAL_LAPIS);
    drawPolygon({ {74, top - 20}, {106, top - 12}, {102, top - 24}, {74, top - 26} }, PAL_LAPIS);
    // pillars with lotus capitals
    float pillarX[2] = { 14, 112 };
    for (float px : pillarX) {
        drawRect(px, GRID_BOTTOM, 26, top - 60 - GRID_BOTTOM, rgb(214, 176, 120));
        for (float yy = GRID_BOTTOM + 30; yy < top - 80; yy += 46) drawRect(px, yy, 26, 5, PAL_LAPIS);
        drawPolygon({ {px - 6, top - 60}, {px + 32, top - 60}, {px + 26, top - 44}, {px, top - 44} }, PAL_GOLD);
    }
    // doorway + sealed door
    float dx = 46, dw = 60, dTop = GRID_BOTTOM + 230;
    drawRect(dx - 8, GRID_BOTTOM, dw + 16, dTop - GRID_BOTTOM + 14, PAL_STONE_DARK);
    drawRect(dx, GRID_BOTTOM, dw, dTop - GRID_BOTTOM, rgb(60, 40, 30));
    drawRect(dx + 4, GRID_BOTTOM, dw / 2 - 5, dTop - GRID_BOTTOM - 6, rgb(110, 70, 40));
    drawRect(dx + dw / 2 + 1, GRID_BOTTOM, dw / 2 - 5, dTop - GRID_BOTTOM - 6, rgb(110, 70, 40));
    drawCircle(dx + dw / 2, GRID_BOTTOM + 120, 9, PAL_GOLD);                          // seal
    drawCircle(dx + dw / 2, GRID_BOTTOM + 120, 5, rgb(180, 40, 30));
    // cartouche above the door
    drawEllipse(dx + dw / 2, dTop + 40, 18, 28, PAL_GOLD);
    drawEllipse(dx + dw / 2, dTop + 40, 13, 23, rgb(240, 214, 140));
    for (int i = 0; i < 3; i++) drawRect(dx + dw / 2 - 6, dTop + 24 + i * 12, 12, 4, PAL_UI_GOLD_DK);
    // steps
    drawRect(0, GRID_BOTTOM - 10, TOMB_RIGHT_X + 8, 10, PAL_STONE_DARK);
    drawRect(0, GRID_BOTTOM - 4, TOMB_RIGHT_X + 2, 6, PAL_STONE);
}

// --------------------------------------------------------- background
void world_drawBackground() {
    // sky
    drawGradientRect(0, GRID_TOP - 40, SCREEN_W, SCREEN_H - GRID_TOP + 40, PAL_DUSK_LOW, PAL_DUSK_MID, 30);
    drawGradientRect(0, GRID_TOP + 90, SCREEN_W, SCREEN_H - GRID_TOP - 90, PAL_DUSK_MID, PAL_DUSK_TOP, 30);
    // sun with glow
    float glow = 1 + 0.04f * std::sin(s_time * 1.5f);
    drawCircle(860, GRID_TOP + 40, 70 * glow, rgb(255, 210, 140, 60));
    drawCircle(860, GRID_TOP + 40, 34, PAL_SUN);
    // distant pyramids + dunes
    drawPyramid(700, GRID_TOP - 6, 220, 120);
    drawPyramid(880, GRID_TOP - 6, 150, 82);
    drawEllipse(300, GRID_TOP - 10, 260, 40, rgb(226, 170, 110));
    drawEllipse(1080, GRID_TOP - 12, 320, 46, rgb(222, 164, 104));
    drawEllipse(600, GRID_TOP - 16, 220, 30, rgb(232, 182, 120));
    // sand behind the lanes and below them (visible on menus / cutscene)
    drawRect(0, 0, SCREEN_W, GRID_TOP, PAL_SAND);
    drawGradientRect(0, 0, SCREEN_W, GRID_BOTTOM, rgb(200, 158, 100), PAL_SAND_DARK, 8);

    // skyline environment (behind the grid)
    drawObelisk(1010, GRID_TOP - 8, 150);
    drawObelisk(1190, GRID_TOP - 8, 118);
    drawObelisk(460, GRID_TOP - 8, 130);
    drawPalm(300, GRID_TOP - 8, 120, 0.0f);
    drawPalm(560, GRID_TOP - 8, 96, 1.7f);
    drawPalm(1110, GRID_TOP - 8, 104, 3.1f);
    drawBrokenColumn(640, GRID_TOP - 8, 46);
    drawBrokenColumn(1250, GRID_TOP - 8, 60);
    drawBrokenColumn(380, GRID_TOP - 8, 34);

    // lanes: sandy paths separated by low stone borders
    for (int lane = 0; lane < NUM_LANES; lane++) {
        float y = laneBottomY(lane);
        drawGradientRect(TOMB_RIGHT_X, y, SCREEN_W - TOMB_RIGHT_X, LANE_HEIGHT,
                         lane % 2 ? rgb(224, 186, 124) : rgb(232, 196, 134),
                         lane % 2 ? rgb(232, 196, 136) : rgb(240, 206, 146), 6);
        // ripples in the sand
        for (int i = 0; i < 9; i++) {
            float rx = TOMB_RIGHT_X + 60 + i * 125 + (lane * 37) % 60;
            drawArc(rx, y + 30 + (i % 3) * 14, 18, 20, 160, 1.5f, rgb(205, 165, 105, 140), 10);
        }
        // stone border at the top of each lane
        float by = y + LANE_HEIGHT - 6;
        for (float bx = TOMB_RIGHT_X; bx < SCREEN_W; bx += 42) {
            drawRect(bx, by, 40, 7, rgb(184, 150, 108));
            drawRect(bx, by, 40, 2, rgb(150, 118, 84));
        }
    }
    // bottom lane edge
    for (float bx = TOMB_RIGHT_X; bx < SCREEN_W; bx += 42) drawRect(bx, GRID_BOTTOM - 6, 40, 7, rgb(184, 150, 108));

    drawTomb();
    drawTorch(TOMB_RIGHT_X + 14, GRID_TOP - 6);
    drawTorch(TOMB_RIGHT_X + 14, GRID_BOTTOM + 4);
}

void world_drawForeground() {
    // a palm leaning in at the bottom-right corner (in front of characters)
    pushMatrix();
    translate(SCREEN_W + 10, GRID_BOTTOM - 20);
    rotate(18);
    for (int i = 0; i < 4; i++) {
        pushMatrix();
        rotate(110 + i * 22);
        drawEllipse(34, 0, 38, 6, i % 2 ? PAL_PALM_GREEN : rgb(44, 100, 48));
        popMatrix();
    }
    popMatrix();
}
