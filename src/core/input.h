// =====================================================================
//  input.h  -  SHARED keyboard + mouse helpers (any member can use).
//  Mouse position will be in world pixels (bottom-left = 0,0).
//  The real code comes in Step 11; until then these are stubs.
// =====================================================================
#pragma once
#define GLFW_INCLUDE_NONE   // don't let GLFW pull in the system OpenGL header (GLAD provides it)
#include <GLFW/glfw3.h>

void  input_init(GLFWwindow* window);
void  input_update();                 // called once per frame by main

bool  keyDown(int key);               // held down, e.g. keyDown(GLFW_KEY_W)
bool  keyPressed(int key);            // pressed THIS frame only (menus, toggles)

float mouseX();
float mouseY();
bool  mouseClicked();                 // left button pressed this frame
bool  mouseDown();                    // left button held
bool  mouseInRect(float x, float y, float w, float h);
