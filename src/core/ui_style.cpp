// =====================================================================
//  ui_style.cpp  -  SHARED.
// =====================================================================
#include "ui_style.h"
#include "input.h"
#include "config.h"

void drawPanel(float x, float y, float w, float h, Color fill, Color border) {
    drawRoundedRect(x - 3, y - 3, w + 6, h + 6, 10, PAL_UI_GOLD_DK);
    drawRoundedRect(x - 1.5f, y - 1.5f, w + 3, h + 3, 9, border);
    drawRoundedRect(x + 1.5f, y + 1.5f, w - 3, h - 3, 8, fill);
    // corner studs
    drawCircle(x + 6, y + 6, 2.5f, border);
    drawCircle(x + w - 6, y + 6, 2.5f, border);
    drawCircle(x + 6, y + h - 6, 2.5f, border);
    drawCircle(x + w - 6, y + h - 6, 2.5f, border);
}

void drawParchmentPanel(float x, float y, float w, float h) {
    drawRoundedRect(x - 4, y - 4, w + 8, h + 8, 8, PAL_UI_GOLD_DK);
    drawRoundedRect(x, y, w, h, 6, PAL_PARCHMENT);
    drawRectOutline(x + 10, y + 10, w - 20, h - 20, 2, rgb(170, 140, 90));
    // scroll rolls top and bottom
    drawRoundedRect(x - 14, y + h - 6, w + 28, 16, 8, rgb(190, 160, 110));
    drawRoundedRect(x - 14, y - 10, w + 28, 16, 8, rgb(190, 160, 110));
    // hieroglyph-like marks down both sides
    for (int i = 0; i < 6; i++) {
        float yy = y + 40 + i * (h - 80) / 5;
        drawAnkh(x + 24, yy, 18, rgb(150, 115, 65));
        drawAnkh(x + w - 24, yy, 18, rgb(150, 115, 65));
    }
}

bool drawButton(const std::string& label, float cx, float cy, float w, float h) {
    float x = cx - w / 2, y = cy - h / 2;
    bool hover = mouseInRect(x, y, w, h);
    drawRoundedRect(x - 2, y - 2, w + 4, h + 4, 8, PAL_UI_GOLD);
    drawGradientRect(x + 2, y + 2, w - 4, h - 4,
                     hover ? rgb(40, 78, 150) : rgb(22, 44, 92),
                     hover ? rgb(70, 118, 200) : rgb(40, 74, 140), 12);
    float size = h * 0.42f;
    drawTextCentered(label, cx, cy + size / 2, size, hover ? PAL_WHITE : rgb(240, 225, 185));
    return hover && mouseClicked();
}

void drawAnkh(float cx, float cy, float h, Color c) {
    float t = h * 0.13f;                       // stroke thickness
    float loopR = h * 0.18f;
    float loopCy = cy + h / 2 - loopR - t / 2;
    pushMatrix();                                                      // loop (oval ring)
    translate(cx, loopCy);
    scale(1.0f, 1.2f);
    drawArc(0, 0, loopR, 0, 360, t / 1.1f, c, 28);
    popMatrix();
    drawRect(cx - h * 0.3f, loopCy - loopR * 1.2f - t, h * 0.6f, t, c);  // cross bar
    drawRect(cx - t / 2, cy - h / 2, t, h * 0.55f, c);                   // stem
}

void drawCoinIcon(float cx, float cy, float r) {
    drawCircle(cx, cy, r, PAL_UI_GOLD_DK);
    drawCircle(cx, cy, r * 0.86f, PAL_GOLD);
    drawCircle(cx - r * 0.25f, cy + r * 0.25f, r * 0.35f, rgb(255, 225, 120));  // shine
    drawAnkh(cx, cy, r * 1.1f, PAL_UI_GOLD_DK);
}

void drawDimOverlay(float alpha, Color c) {
    c.a = alpha;
    drawRect(0, 0, SCREEN_W, SCREEN_H, c);
}
