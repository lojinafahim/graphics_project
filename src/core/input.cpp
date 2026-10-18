// =====================================================================
//  input.cpp  -  SHARED.
// =====================================================================
#include "input.h"
#include "config.h"

static GLFWwindow* s_window = nullptr;
static bool s_now[GLFW_KEY_LAST + 1]  = {};
static bool s_mouseNow = false;
static float s_mx = 0, s_my = 0;

// Presses are recorded by callbacks so even very quick taps are never missed.
static bool s_pressEvent[GLFW_KEY_LAST + 1] = {};
static bool s_pressedThisFrame[GLFW_KEY_LAST + 1] = {};
static bool s_clickEvent = false, s_clickedThisFrame = false;

static void keyCallback(GLFWwindow*, int key, int, int action, int) {
    if (key >= 0 && key <= GLFW_KEY_LAST && action == GLFW_PRESS) s_pressEvent[key] = true;
}
static void mouseButtonCallback(GLFWwindow*, int button, int action, int) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) s_clickEvent = true;
}

void input_init(GLFWwindow* window) {
    s_window = window;
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
}

void input_update() {
    for (int k = GLFW_KEY_SPACE; k <= GLFW_KEY_LAST; k++) {
        s_now[k] = glfwGetKey(s_window, k) == GLFW_PRESS;
        s_pressedThisFrame[k] = s_pressEvent[k];
        s_pressEvent[k] = false;
    }
    s_mouseNow = glfwGetMouseButton(s_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    s_clickedThisFrame = s_clickEvent;
    s_clickEvent = false;

    double cx, cy;
    int ww, wh;
    glfwGetCursorPos(s_window, &cx, &cy);
    glfwGetWindowSize(s_window, &ww, &wh);
    if (ww > 0 && wh > 0) {
        s_mx = (float)(cx / ww * SCREEN_W);
        s_my = (float)(SCREEN_H - cy / wh * SCREEN_H);   // flip: world y goes up
    }
}

bool keyDown(int key)    { return key >= 0 && key <= GLFW_KEY_LAST && s_now[key]; }
bool keyPressed(int key) { return key >= 0 && key <= GLFW_KEY_LAST && s_pressedThisFrame[key]; }
float mouseX() { return s_mx; }
float mouseY() { return s_my; }
bool mouseClicked() { return s_clickedThisFrame; }
bool mouseDown() { return s_mouseNow; }
bool mouseInRect(float x, float y, float w, float h) {
    return s_mx >= x && s_mx <= x + w && s_my >= y && s_my <= y + h;
}
