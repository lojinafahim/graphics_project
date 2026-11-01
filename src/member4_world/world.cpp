#include "core/shapes.h"
#include "core/config.h"
#include "member4_world/world.h"
#include <cmath>

static float s_time = 0;

void world_update(float dt) {
    s_time += dt;
}

static void drawPyramid(float cx, float baseY, float w, float h) {
    drawTriangle(cx - w / 2, baseY, cx, baseY, cx, baseY + h, PAL_PYRAMID_LIT);
    drawTriangle(cx, baseY, cx + w / 2, baseY, cx, baseY + h, PAL_PYRAMID_DARK);

    for (int i = 1; i < 6; i++) {
        float y    = baseY + h * i / 6.0f;
        float half = w / 2 * (1 - i / 6.0f);
        drawLine(cx - half, y, cx + half, y, 1, PAL_PYRAMID_LINE);
    }
}

static void drawObelisk(float cx, float baseY, float h) {
    float wBase = h * 0.13f;
    float wTop  = h * 0.08f;

    drawRect(cx - wBase * 0.8f, baseY, wBase * 1.6f, h * 0.06f, PAL_STONE_DARK);

    float y0 = baseY + h * 0.06f;
    float y1 = baseY + h * 0.88f;
    drawPolygon({ {cx - wBase / 2, y0}, {cx + wBase / 2, y0},
                  {cx + wTop / 2, y1}, {cx - wTop / 2, y1} }, PAL_OBELISK_LIT);
    drawPolygon({ {cx, y0}, {cx + wBase / 2, y0},
                  {cx + wTop / 2, y1}, {cx, y1} }, PAL_OBELISK_DARK);

    drawTriangle(cx - wTop / 2, y1, cx + wTop / 2, y1, cx, baseY + h, PAL_GOLD);

    for (int i = 0; i < 7; i++) {
        float yy = y0 + 12 + i * (y1 - y0 - 24) / 7;
        drawRect(cx - 3, yy, 6, 3, PAL_CARVING);
        if (i % 2) drawCircle(cx, yy + 6, 2, PAL_CARVING);
    }
}

static void drawPalm(float x, float baseY, float h, float swayPhase) {
    float sway = std::sin(s_time * 1.3f + swayPhase) * 3.0f;

    int segs = 9;
    float px = x, py = baseY;
    for (int i = 0; i < segs; i++) {
        float t  = (float)(i + 1) / segs;
        float nx = x + std::sin(t * 1.4f) * h * 0.18f + sway * t;
        float ny = baseY + h * t;
        drawLine(px, py, nx, ny, 9 - i * 0.5f, i % 2 ? PAL_TRUNK_LIGHT : PAL_TRUNK_DARK);
        px = nx;
        py = ny;
    }

    for (int i = 0; i < 7; i++) {
        float ang = -20 + i * 35 + std::sin(s_time * 1.6f + i + swayPhase) * 4;
        pushMatrix();
        translate(px, py);
        rotate(ang);
        drawEllipse(h * 0.2f, 0, h * 0.22f, 6, i % 2 ? PAL_PALM_GREEN : PAL_PALM_DARK);
        drawTriangle(h * 0.35f, -5, h * 0.48f, -14, h * 0.38f, 2, PAL_PALM_DARK);
        popMatrix();
    }

    drawCircle(px - 3, py - 4, 4, PAL_DATES);
    drawCircle(px + 4, py - 5, 4, PAL_DATES);
}

static void drawBrokenColumn(float cx, float baseY, float h) {
    float w = 26;

    drawRect(cx - w / 2 - 6, baseY, w + 12, 8, PAL_STONE_DARK);

    drawRect(cx - w / 2, baseY + 8, w, h - 8, PAL_STONE);

    for (int i = 0; i < 3; i++) {
        drawRect(cx - w / 2 + 5 + i * 7, baseY + 10, 2, h - 14, PAL_STONE_DARK);
    }

    drawPolygon({ {cx - w / 2, baseY + h}, {cx + w / 2, baseY + h},
                  {cx + w / 2, baseY + h + 6}, {cx + 2, baseY + h + 16},
                  {cx - w / 2, baseY + h + 9} }, PAL_STONE);

    drawRect(cx + w / 2 + 6, baseY, 16, 10, PAL_STONE);
}

static void drawTorch(float cx, float baseY) {
    drawRect(cx - 3, baseY, 6, 46, PAL_WOOD);

    drawPolygon({ {cx - 11, baseY + 46}, {cx + 11, baseY + 46},
                  {cx + 7, baseY + 56}, {cx - 7, baseY + 56} }, PAL_BRONZE);

    float f = 1.0f + 0.15f * std::sin(s_time * 13 + cx) + 0.08f * std::sin(s_time * 7.3f + cx);

    drawCircle(cx, baseY + 66, 20 * f, PAL_TORCH_GLOW);
    drawEllipse(cx, baseY + 64, 8 * f, 13 * f, PAL_FIRE);
    drawEllipse(cx, baseY + 61, 4.5f * f, 7 * f, PAL_FIRE_CORE);
}

static void drawTomb() {
    float top = GRID_TOP + 120;

    drawPolygon({ {0, GRID_BOTTOM - 10}, {TOMB_RIGHT_X, GRID_BOTTOM - 10},
                  {TOMB_RIGHT_X - 16, top}, {0, top} }, PAL_TOMB_WALL);
    drawPolygon({ {TOMB_RIGHT_X - 40, GRID_BOTTOM - 10}, {TOMB_RIGHT_X, GRID_BOTTOM - 10},
                  {TOMB_RIGHT_X - 16, top}, {TOMB_RIGHT_X - 50, top} }, PAL_TOMB_SHADE);

    for (float yy = GRID_BOTTOM + 20; yy < top; yy += 34) {
        drawLine(0, yy, TOMB_RIGHT_X - 16 * (yy - GRID_BOTTOM) / (top - GRID_BOTTOM), yy, 1.5f, PAL_TOMB_LINE);
    }

    drawRect(-4, top, TOMB_RIGHT_X - 8, 14, PAL_STONE_DARK);
    drawRect(-4, top + 14, TOMB_RIGHT_X - 4, 8, PAL_TOMB_WALL);
    drawCircle(62, top - 22, 10, PAL_GOLD);
    drawPolygon({ {50, top - 20}, {18, top - 12}, {22, top - 24}, {50, top - 26} }, PAL_LAPIS);
    drawPolygon({ {74, top - 20}, {106, top - 12}, {102, top - 24}, {74, top - 26} }, PAL_LAPIS);
    
    float pillarX[2] = { 14, 112 };
    for (float px : pillarX) {
        drawRect(px, GRID_BOTTOM, 26, top - 60 - GRID_BOTTOM, PAL_TOMB_PILLAR);
        for (float yy = GRID_BOTTOM + 30; yy < top - 80; yy += 46) {
            drawRect(px, yy, 26, 5, PAL_LAPIS);
        }
        drawPolygon({ {px - 6, top - 60}, {px + 32, top - 60}, {px + 26, top - 44}, {px, top - 44} }, PAL_GOLD);
    }

    float dx = 46, dw = 60, dTop = GRID_BOTTOM + 230;
    drawRect(dx - 8, GRID_BOTTOM, dw + 16, dTop - GRID_BOTTOM + 14, PAL_STONE_DARK);
    drawRect(dx, GRID_BOTTOM, dw, dTop - GRID_BOTTOM, PAL_DOORWAY);
    drawRect(dx + 4, GRID_BOTTOM, dw / 2 - 5, dTop - GRID_BOTTOM - 6, PAL_DOOR);
    drawRect(dx + dw / 2 + 1, GRID_BOTTOM, dw / 2 - 5, dTop - GRID_BOTTOM - 6, PAL_DOOR);
    drawCircle(dx + dw / 2, GRID_BOTTOM + 120, 9, PAL_GOLD);
    drawCircle(dx + dw / 2, GRID_BOTTOM + 120, 5, PAL_SEAL_RED);

    drawEllipse(dx + dw / 2, dTop + 40, 18, 28, PAL_GOLD);
    drawEllipse(dx + dw / 2, dTop + 40, 13, 23, PAL_CARTOUCHE);
    for (int i = 0; i < 3; i++) {
        drawRect(dx + dw / 2 - 6, dTop + 24 + i * 12, 12, 4, PAL_UI_GOLD_DK);
    }

    drawRect(0, GRID_BOTTOM - 10, TOMB_RIGHT_X + 8, 10, PAL_STONE_DARK);
    drawRect(0, GRID_BOTTOM - 4, TOMB_RIGHT_X + 2, 6, PAL_STONE);
}

void world_drawBackground() {
    drawGradientRect(0, GRID_TOP - 40, SCREEN_W, SCREEN_H - GRID_TOP + 40, PAL_DUSK_LOW, PAL_DUSK_MID, 30);
    drawGradientRect(0, GRID_TOP + 90, SCREEN_W, SCREEN_H - GRID_TOP - 90, PAL_DUSK_MID, PAL_DUSK_TOP, 30);

    float glow = 1 + 0.04f * std::sin(s_time * 1.5f);
    drawCircle(860, GRID_TOP + 40, 70 * glow, PAL_SUN_GLOW);
    drawCircle(860, GRID_TOP + 40, 34, PAL_SUN);

    drawPyramid(700, GRID_TOP - 6, 220, 120);
    drawPyramid(880, GRID_TOP - 6, 150, 82);

    drawEllipse(300,  GRID_TOP - 10, 260, 40, PAL_DUNE_MID);
    drawEllipse(1080, GRID_TOP - 12, 320, 46, PAL_DUNE_FAR);
    drawEllipse(600,  GRID_TOP - 16, 220, 30, PAL_DUNE_NEAR);

    drawRect(0, 0, SCREEN_W, GRID_TOP, PAL_SAND);
    drawGradientRect(0, 0, SCREEN_W, GRID_BOTTOM, PAL_GROUND_LOW, PAL_SAND_DARK, 8);

    drawObelisk(1010, GRID_TOP - 8, 150);
    drawObelisk(1190, GRID_TOP - 8, 118);
    drawObelisk(460,  GRID_TOP - 8, 130);

    drawPalm(300,  GRID_TOP - 8, 120, 0.0f);
    drawPalm(560,  GRID_TOP - 8, 96,  1.7f);
    drawPalm(1110, GRID_TOP - 8, 104, 3.1f);

    drawBrokenColumn(640,  GRID_TOP - 8, 46);
    drawBrokenColumn(1250, GRID_TOP - 8, 60);
    drawBrokenColumn(380,  GRID_TOP - 8, 34);

    drawTorch(TOMB_RIGHT_X + 14, GRID_TOP - 6);
    

        for (int lane = 0; lane < NUM_LANES; lane++) {
        float y = laneBottomY(lane);

        drawGradientRect(TOMB_RIGHT_X, y, SCREEN_W - TOMB_RIGHT_X, LANE_HEIGHT,
                         lane % 2 ? PAL_LANE_B_LOW  : PAL_LANE_A_LOW,
                         lane % 2 ? PAL_LANE_B_HIGH : PAL_LANE_A_HIGH, 6);

        for (int i = 0; i < 9; i++) {
            float rx = TOMB_RIGHT_X + 60 + i * 125 + (lane * 37) % 60;
            drawArc(rx, y + 30 + (i % 3) * 14, 18, 20, 160, 1.5f, PAL_RIPPLE, 10);
        }

        float by = y + LANE_HEIGHT - 6;
        for (float bx = TOMB_RIGHT_X; bx < SCREEN_W; bx += 42) {
            drawRect(bx, by, 40, 7, PAL_BORDER);
            drawRect(bx, by, 40, 2, PAL_BORDER_DARK);
        }
    }

    for (float bx = TOMB_RIGHT_X; bx < SCREEN_W; bx += 42) {
        drawRect(bx, GRID_BOTTOM - 6, 40, 7, PAL_BORDER);
    }

    drawTorch(TOMB_RIGHT_X + 14, GRID_BOTTOM + 4);

    drawTomb();
}
