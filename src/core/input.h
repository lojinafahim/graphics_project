// =====================================================================
//  input.h  -  SHARED keyboard + mouse helpers (any member can use).
//  Mouse position is already converted to world coords (bottom-left 0,0).
// =====================================================================
#pragma once
#include <GLFW/glfw3.h>

void  input_init(GLFWwindow* window);
void  input_update();                 // called once per frame by main

bool  keyDown(int key);               // held down       e.g. keyDown(GLFW_KEY_W)
bool  keyPressed(int key);            // pressed THIS frame only (use for menus/toggles)

float mouseX();
float mouseY();
bool  mouseClicked();                 // left button pressed this frame
bool  mouseDown();                    // left button held
bool  mouseInRect(float x, float y, float w, float h);
