#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>

struct Color {
    float r, g, b, a;
};

inline Color rgb(int r, int g, int b, int a = 255) {
    return { r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f };
}


// Sky and light
const Color PAL_DUSK_TOP     = rgb( 58,  52, 110);
const Color PAL_DUSK_MID     = rgb(214, 120,  92); 
const Color PAL_DUSK_LOW     = rgb(250, 186, 104);   
const Color PAL_SUN          = rgb(255, 226, 150);
const Color PAL_SUN_GLOW     = rgb(255, 210, 140, 60);   
const Color PAL_SKY_NIGHT    = rgb( 22,  28,  58);   

// Ground and stone
const Color PAL_SAND         = rgb(237, 201, 133);
const Color PAL_SAND_DARK    = rgb(205, 165,  98);   
const Color PAL_STONE        = rgb(181, 160, 125);   
const Color PAL_STONE_DARK   = rgb(128, 109,  83);   
const Color PAL_PYRAMID_LIT  = rgb(214, 150, 100);   
const Color PAL_PYRAMID_DARK = rgb(168, 104,  84);  
const Color PAL_PYRAMID_LINE = rgb(150,  95,  75, 120);  
const Color PAL_DUNE_FAR     = rgb(222, 164, 104);
const Color PAL_DUNE_MID     = rgb(226, 170, 110);
const Color PAL_DUNE_NEAR    = rgb(232, 182, 120);
const Color PAL_GROUND_LOW   = rgb(200, 158, 100);   // bottom edge of the ground
const Color PAL_OBELISK_LIT  = rgb(204, 160, 110);
const Color PAL_OBELISK_DARK = rgb(176, 130,  92);
const Color PAL_CARVING      = rgb(130,  95,  65);   // hieroglyph marks
const Color PAL_LANE_A_LOW   = rgb(232, 196, 134);   // even lanes (0, 2, 4): bottom of fade
const Color PAL_LANE_A_HIGH  = rgb(240, 206, 146);   //   top of fade
const Color PAL_LANE_B_LOW   = rgb(224, 186, 124);   // odd lanes (1, 3): bottom of fade
const Color PAL_LANE_B_HIGH  = rgb(232, 196, 136);   //   top of fade
const Color PAL_RIPPLE       = rgb(205, 165, 105, 140);   // sand ripples (see-through)
const Color PAL_BORDER       = rgb(184, 150, 108);   // stone blocks between lanes
const Color PAL_BORDER_DARK  = rgb(150, 118,  84);   // their shaded top edge

// Tomb
const Color PAL_TOMB_WALL     = rgb(196, 150, 100);   // sunlit pylon wall
const Color PAL_TOMB_SHADE    = rgb(160, 118,  82);   // shaded wall edge
const Color PAL_TOMB_LINE     = rgb(140, 100,  70, 160);   // stone course lines
const Color PAL_TOMB_PILLAR   = rgb(214, 176, 120);
const Color PAL_DOORWAY       = rgb( 60,  40,  30);   // dark opening
const Color PAL_DOOR          = rgb(110,  70,  40);   // wooden door panels
const Color PAL_SEAL_RED      = rgb(180,  40,  30);   // wax seal
const Color PAL_CARTOUCHE     = rgb(240, 214, 140);   // inside of the cartouche

// Nature and objects
const Color PAL_PALM_GREEN   = rgb( 58, 125,  60);
const Color PAL_PALM_DARK    = rgb( 44, 100,  48);   // alternate frond shade
const Color PAL_TRUNK_LIGHT  = rgb(120,  82,  50);   // palm trunk bands
const Color PAL_TRUNK_DARK   = rgb( 98,  66,  40);
const Color PAL_DATES        = rgb(110,  70,  30);   // fruit under the fronds
const Color PAL_WOOD         = rgb(110,  72,  40);   
const Color PAL_GOLD         = rgb(232, 182,  42);  
const Color PAL_FIRE         = rgb(255, 120,  20);   
const Color PAL_FIRE_CORE    = rgb(255, 220,  90);   // bright inner flame
const Color PAL_TORCH_GLOW   = rgb(255, 170,  60, 50);   // see-through light around flames
const Color PAL_NILE         = rgb( 46, 120, 160);

// Guards (players)
const Color PAL_SKIN         = rgb(176, 120,  78);
const Color PAL_WHITE_KILT   = rgb(240, 236, 222);
const Color PAL_BRONZE       = rgb(184, 115,  51);   
const Color PAL_LAPIS        = rgb( 38,  70, 160);   

// Raiders (enemies)
const Color PAL_RAIDER_RAGS  = rgb(107,  91,  74);
const Color PAL_RAIDER_DARK  = rgb( 60,  50,  42);

// UI
const Color PAL_UI_NAVY      = rgb( 18,  30,  54);   
const Color PAL_UI_NAVY_LT   = rgb( 34,  56,  96);   
const Color PAL_UI_GOLD      = rgb(226, 180,  76);   
const Color PAL_UI_GOLD_DK   = rgb(150, 108,  40);
const Color PAL_PARCHMENT    = rgb(232, 212, 168);   
const Color PAL_HP_GREEN     = rgb( 70, 200,  70);
const Color PAL_HP_RED       = rgb(200,  40,  40);

// Basics
const Color PAL_WHITE        = rgb(255, 255, 255);
const Color PAL_BLACK        = rgb(  0,   0,   0);

//shapes
void shapes_init();
void shapes_flush();          // call once at the end of every frame
void drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, Color c);
void drawRect(float x, float y, float w, float h, Color c);
void drawCircle(float cx, float cy, float radius, Color c, int segments = 32);
void drawEllipse(float cx, float cy, float rx, float ry, Color c, int segments = 32);
void drawLine(float x1, float y1, float x2, float y2, float thickness, Color c);
void drawPolygon(const std::vector<glm::vec2>& points, Color c);
void drawGradientRect(float x, float y, float w, float h, Color bottom, Color top, int steps = 48);
void drawRectCentered(float cx, float cy, float w, float h, Color c);
void drawRectOutline(float x, float y, float w, float h, float thickness, Color c);
void drawRoundedRect(float x, float y, float w, float h, float radius, Color c);
void drawArc(float cx, float cy, float radius, float startDeg, float endDeg, float thickness, Color c, int segments = 24);

//transformations
void translate(float x, float y);
void resetTransform();
void rotate(float degrees);
void scale(float sx, float sy);
void pushMatrix();
void popMatrix();

//color effects
void setTint(float amount);
void setGrayscale(bool on);
void setAlpha(float a);
void resetEffects();

//text
void text_init();
void drawText(const std::string& text, float x, float y, float size, Color c);
void drawTextCentered(const std::string& text, float cx, float y, float size, Color c);
float textWidth(const std::string& text, float size);
