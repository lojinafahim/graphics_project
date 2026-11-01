// =====================================================================
//  input.cpp  -  keyboard + mouse, read once per frame.
//  Based on learnopengl.com: Hello Window (glfwGetKey) and
//  Camera > "Mouse input" (glfwGetCursorPos).
// =====================================================================
#include "core/input.h"
#include "core/config.h"

static GLFWwindow* s_window = nullptr;

// Key states: this frame and the previous frame
static bool s_keys[GLFW_KEY_LAST + 1];
static bool s_prevKeys[GLFW_KEY_LAST + 1];

// Mouse
static float s_mouseX = 0, s_mouseY = 0;
static bool  s_mouseDown = false, s_prevMouseDown = false;

void input_init(GLFWwindow* window) {
    s_window = window;
}

void input_update() {
    if (!s_window) return;

    // keyboard: remember last frame, then read every key now
    for (int k = GLFW_KEY_SPACE; k <= GLFW_KEY_LAST; k++) {
        s_prevKeys[k] = s_keys[k];
        s_keys[k] = glfwGetKey(s_window, k) == GLFW_PRESS;
    }

    // mouse button
    s_prevMouseDown = s_mouseDown;
    s_mouseDown = glfwGetMouseButton(s_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

    // mouse position: window pixels (top-left origin) -> world pixels (bottom-left origin)
    double cx, cy;
    int winW, winH;
    glfwGetCursorPos(s_window, &cx, &cy);
    glfwGetWindowSize(s_window, &winW, &winH);
    if (winW > 0 && winH > 0) {
        s_mouseX = (float)(cx / winW * SCREEN_W);
        s_mouseY = (float)(SCREEN_H - cy / winH * SCREEN_H);
    }
}

bool keyDown(int key) {
    if (key < 0 || key > GLFW_KEY_LAST) return false;
    return s_keys[key];
}

bool keyPressed(int key) {
    if (key < 0 || key > GLFW_KEY_LAST) return false;
    return s_keys[key] && !s_prevKeys[key];
}

float mouseX()       { return s_mouseX; }
float mouseY()       { return s_mouseY; }
bool  mouseDown()    { return s_mouseDown; }
bool  mouseClicked() { return s_mouseDown && !s_prevMouseDown; }

bool mouseInRect(float x, float y, float w, float h) {
    return s_mouseX >= x && s_mouseX <= x + w &&
           s_mouseY >= y && s_mouseY <= y + h;
}
