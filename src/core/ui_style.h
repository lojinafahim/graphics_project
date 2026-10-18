// =====================================================================
//  ui_style.h  -  SHARED look for menus, panels, buttons and icons
//  (navy panels with gold trim, like the reference picture).
// =====================================================================
#pragma once
#include <string>
#include "shapes.h"

// Navy panel with a gold border.
void drawPanel(float x, float y, float w, float h,
               Color fill = PAL_UI_NAVY, Color border = PAL_UI_GOLD);

// Parchment (scroll) panel - used by the pause menu.
void drawParchmentPanel(float x, float y, float w, float h);

// Button centred at (cx, cy). Draws itself, lights up on hover,
// and returns true on the frame it is clicked.
bool drawButton(const std::string& label, float cx, float cy, float w, float h);

// Small gold coin with an ankh, centred at (cx, cy).
void drawCoinIcon(float cx, float cy, float radius);

// Ankh symbol (gold by default), centred at (cx, cy), total height h.
void drawAnkh(float cx, float cy, float h, Color c = PAL_UI_GOLD);

// Dark see-through layer over the whole screen (behind menus).
void drawDimOverlay(float alpha = 0.55f, Color c = PAL_BLACK);
