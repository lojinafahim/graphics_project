// =====================================================================
//  healthbar.h  -  SHARED health bar for players AND enemies.
// =====================================================================
#pragma once

// Draws a bar centred at (cx, y). Green part shrinks as hp drops.
// Call it in WORLD coordinates (outside pushMatrix/translate) and after
// resetEffects(), so the bar isn't tinted red or moved with the character.
// Typical use, above a character:  drawHealthBar(e.x, e.y + e.height + 8, e.hp, e.maxHp);
void drawHealthBar(float cx, float y, int hp, int maxHp, float width = 46.0f);
