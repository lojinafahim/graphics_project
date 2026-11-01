// =====================================================================
//  main.cpp  -  Window, game loop and screen flow.
//  Owner: Member 1. Other members should NOT need to edit this file:
//  everything you write is called from here through interfaces.h.
// =====================================================================
#define GLAD_GL_IMPLEMENTATION      // only in THIS file
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "core/config.h"
#include "core/shapes.h"
#include "core/input.h"
#include "core/game_state.h"
#include "core/interfaces.h"

static bool s_showLaneDebug = false;   // press F1 in game to show lanes

// Debug overlay: lane boundaries + numbers (helps everyone place things)
static void drawLaneDebug() {
    for (int lane = 0; lane < NUM_LANES; lane++) {
        drawRectOutline(TOMB_RIGHT_X, laneBottomY(lane), GRID_RIGHT_X - TOMB_RIGHT_X,
                        LANE_HEIGHT, 1.5f, rgb(255, 0, 255));
        drawText("lane " + std::to_string(lane), TOMB_RIGHT_X + 6, laneBottomY(lane) + LANE_HEIGHT - 10,
                 12, rgb(255, 0, 255));
    }
    for (int col = 0; col <= NUM_COLS; col++)
        drawLine(colLeftX(col), GRID_BOTTOM, colLeftX(col), GRID_TOP,
                 1, rgb(255, 0, 255, 120));
    drawText("x " + std::to_string((int)mouseX()) + "  y " + std::to_string((int)mouseY()),
             SCREEN_W / 2.0f - 60, SCREEN_H - 14, 12, rgb(255, 0, 255));
}

// Order matters: later = drawn on top
static void drawGameScene() {
    world_drawBackground();
    grid_draw();
    coins_draw();
    players_draw();
    enemies_draw();
    world_drawForeground();
    hud_draw();
    if (s_showLaneDebug) drawLaneDebug();
}

static void updateGame(float dt) {
    grid_update(dt);
    hud_update(dt);
    world_update(dt);
    waves_update(dt);
    players_update(dt);
    enemies_update(dt);
    combat_update(dt);
    coins_update(dt);
    winLose_update(dt);
    cleanupDead();
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to init GLFW\n";
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCREEN_W, SCREEN_H, GAME_TITLE, nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);   // vsync

    if (gladLoadGL(glfwGetProcAddress) == 0) {
        std::cerr << "Failed to initialize OpenGL context\n";
        return -1;
    }

    shapes_init();
    text_init();          // FreeType font (learnopengl Text Rendering)
    input_init(window);
    sound_init();
    ui_init();
    world_init();
    players_init();
    enemies_init();
    resetGame();
    if (g_screen == SCREEN_CUTSCENE) cutscene_start();

    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window) && g_screen != SCREEN_QUIT) {
        double now = glfwGetTime();
        float dt = (float)(now - lastTime);
        lastTime = now;
        if (dt > 0.05f) dt = 0.05f;   // avoid huge jumps after dragging the window

        glfwPollEvents();
        input_update();

        int fbw, fbh;
        glfwGetFramebufferSize(window, &fbw, &fbh);
        glViewport(0, 0, fbw, fbh);
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        resetTransform();
        resetEffects();

        if (keyPressed(GLFW_KEY_F1)) s_showLaneDebug = !s_showLaneDebug;

        switch (g_screen) {
        case SCREEN_MAIN_MENU:
            mainMenu_update(dt);
            mainMenu_draw();
            break;

        case SCREEN_SOUND_SETTINGS:
            soundSettings_update(dt);
            soundSettings_draw();
            break;

        case SCREEN_CUTSCENE:
            cutscene_update(dt);
            cutscene_draw();
            if (cutscene_finished()) setScreen(SCREEN_PLAYING);
            break;

        case SCREEN_PLAYING:
            if (keyPressed(GLFW_KEY_ESCAPE)) {
                setScreen(SCREEN_PAUSED);
                drawGameScene();
                break;
            }
            updateGame(dt);
            drawGameScene();
            break;

        case SCREEN_PAUSED:          // game frozen underneath the menu
            drawGameScene();
            pauseMenu_update(dt);
            pauseMenu_draw();
            break;

        case SCREEN_WIN:
        case SCREEN_LOSE:            // everything stops, menu on top
            drawGameScene();
            endMenu_update(dt);
            endMenu_draw();
            break;

        case SCREEN_QUIT:
            break;
        }

        shapes_flush();       // draw everything collected this frame in one batch
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
