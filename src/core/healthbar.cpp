// =====================================================================
//  healthbar.cpp  -  SHARED.
// =====================================================================
#include "healthbar.h"
#include "shapes.h"

void drawHealthBar(float cx, float y, int hp, int maxHp, float width) {
    if (maxHp <= 0) return;
    float frac = hp < 0 ? 0.0f : (float)hp / maxHp;
    if (frac > 1) frac = 1;
    const float h = 7.0f;
    float x = cx - width / 2;

    drawRect(x - 1, y - 1, width + 2, h + 2, PAL_BLACK);               // border
    drawRect(x, y, width, h, PAL_HP_RED);                             // missing hp
    drawRect(x, y, width * frac, h, frac > 0.3f ? PAL_HP_GREEN : PAL_GOLD); // remaining
}
