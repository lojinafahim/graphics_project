// =====================================================================
//  shapes.h  -  SHARED drawing helpers. Everyone draws with these.
//
//  Coordinates: world pixels, (0,0) = bottom-left, 1280 x 720.
//
//  Quick example - a guard standing in lane 2:
//      pushMatrix();
//        translate(x, laneBottomY(2));
//        setTint(guard.damageAmount());            // redder as hp drops
//        drawRect(-15, 0, 30, 50, PAL_WHITE_KILT);  // body
//        drawCircle(0, 62, 12, PAL_SKIN);           // head
//        pushMatrix();                              // sword arm
//          translate(15, 40);
//          rotate(swingAngle);
//          drawRect(0, -3, 35, 6, PAL_BRONZE);
//        popMatrix();
//        resetEffects();
//      popMatrix();
// =====================================================================
#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>

struct Color {
    float r, g, b, a;
};
// Color from 0-255 values:  rgb(237, 201, 133)
inline Color rgb(int r, int g, int b, int a = 255) {
    return { r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f };
}

// ---------- Shared palette (keeps the art consistent) ------------------
const Color PAL_SKY_DAY     = rgb(135, 196, 235);
const Color PAL_SKY_NIGHT   = rgb( 22,  28,  58);
const Color PAL_SAND        = rgb(237, 201, 133);
const Color PAL_SAND_DARK   = rgb(205, 165,  98);
const Color PAL_STONE       = rgb(181, 160, 125);
const Color PAL_STONE_DARK  = rgb(128, 109,  83);
const Color PAL_GOLD        = rgb(232, 182,  42);
const Color PAL_NILE        = rgb( 46, 120, 160);
const Color PAL_PALM_GREEN  = rgb( 58, 125,  60);
const Color PAL_WOOD        = rgb(110,  72,  40);
const Color PAL_SKIN        = rgb(176, 120,  78);
const Color PAL_WHITE_KILT  = rgb(240, 236, 222);
const Color PAL_BRONZE      = rgb(184, 115,  51);
const Color PAL_LAPIS       = rgb( 38,  70, 160);   // guards' accent colour
const Color PAL_RAIDER_RAGS = rgb(107,  91,  74);   // raiders' cloth
const Color PAL_RAIDER_DARK = rgb( 60,  50,  42);
const Color PAL_FIRE        = rgb(255, 120,  20);
const Color PAL_BLACK       = rgb(  0,   0,   0);
const Color PAL_WHITE       = rgb(255, 255, 255);
const Color PAL_HP_GREEN    = rgb( 70, 200,  70);
const Color PAL_HP_RED      = rgb(200,  40,  40);
// Dusk sky (reference picture)
const Color PAL_DUSK_TOP    = rgb( 58,  52, 110);
const Color PAL_DUSK_MID    = rgb(214, 120,  92);
const Color PAL_DUSK_LOW    = rgb(250, 186, 104);
const Color PAL_SUN         = rgb(255, 226, 150);
// UI (navy panels with gold trim)
const Color PAL_UI_NAVY     = rgb( 18,  30,  54);
const Color PAL_UI_NAVY_LT  = rgb( 34,  56,  96);
const Color PAL_UI_GOLD     = rgb(226, 180,  76);
const Color PAL_UI_GOLD_DK  = rgb(150, 108,  40);
const Color PAL_PARCHMENT   = rgb(232, 212, 168);

// ---------- Setup (called once by main) --------------------------------
void shapes_init();

// ---------- Shapes ------------------------------------------------------
void drawRect(float x, float y, float w, float h, Color c);        // (x,y) = bottom-left corner
void drawRectCentered(float cx, float cy, float w, float h, Color c);
void drawGradientRect(float x, float y, float w, float h, Color bottom, Color top, int steps = 48);
void drawRoundedRect(float x, float y, float w, float h, float radius, Color c);
void drawRectOutline(float x, float y, float w, float h, float thickness, Color c);
void drawCircle(float cx, float cy, float radius, Color c, int segments = 32);
void drawEllipse(float cx, float cy, float rx, float ry, Color c, int segments = 32);
void drawArc(float cx, float cy, float radius, float startDeg, float endDeg, float thickness, Color c, int segments = 24);
void drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, Color c);
void drawLine(float x1, float y1, float x2, float y2, float thickness, Color c);
// Convex polygons only (points in order around the shape).
// For concave shapes, split them into several convex pieces.
void drawPolygon(const std::vector<glm::vec2>& points, Color c);

// ---------- Text --------------------------------------------------------
// (x, y) = top-left of the text. size = height of letters in pixels (~ 12-60).
void  drawText(const std::string& text, float x, float y, float size, Color c);
void  drawTextCentered(const std::string& text, float cx, float y, float size, Color c);
float textWidth(const std::string& text, float size);

// ---------- Transforms (like a stack of "move the paper") ---------------
void pushMatrix();
void popMatrix();
void translate(float x, float y);
void rotate(float degrees);            // counter-clockwise
void scale(float sx, float sy);

// ---------- Effects (stay on until you reset them) ----------------------
void setTint(float amount);            // 0 = normal, 1 = fully red (damage)
void setGrayscale(bool on);            // locked player cards
void setAlpha(float a);                // 0..1 fade (cutscene fades)
void resetEffects();
