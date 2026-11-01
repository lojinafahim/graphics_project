#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "core/shapes.h"
#include "core/config.h"
#include <cmath>
#include <vector>
#include <map>
#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H

static const char* vertexShaderSrc = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec4 aColor;
out vec4 vColor;
uniform mat4 projection;
void main() {
    gl_Position = projection * vec4(aPos, 0.0, 1.0);
    vColor = aColor;
}
)";

static const char* fragmentShaderSrc = R"(
#version 330 core
in vec4 vColor;
out vec4 FragColor;
void main() {
    FragColor = vColor;
}
)";

// ---------- Text shaders (from learnopengl.com: In Practice > Text Rendering) ----------
static const char* textVertexSrc = R"(
#version 330 core
layout (location = 0) in vec4 vertex;
out vec2 TexCoords;
uniform mat4 projection;
uniform mat4 model;
void main() {
    gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
}
)";

static const char* textFragmentSrc = R"(
#version 330 core
in vec2 TexCoords;
out vec4 FragColor;
uniform sampler2D text;
uniform vec4 textColor;
void main() {
    float coverage = texture(text, TexCoords).r;
    FragColor = vec4(textColor.rgb, textColor.a * coverage);
}
)";

static GLuint compileShader(GLenum type, const char* src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    GLint ok;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[512];
        glGetShaderInfoLog(s, 512, nullptr, log);
        std::cerr << "Shader error: " << log << "\n";
    }
    return s;
}

static GLuint program, vao, vbo;
static glm::mat4 currentModel = glm::mat4(1.0f);
static std::vector<glm::mat4> matrixStack;
static float currentTint = 0.0f;
static bool currentGray = false;
static float currentAlpha = 1.0f;

// ---------- Batching: triangles are collected here and sent to the GPU together ----------
// Each corner = 6 floats: x, y, r, g, b, a
static const int FLOATS_PER_VERTEX = 6;
static const int MAX_VERTICES = 60000;
static std::vector<float> batch;

// ---------- Text (one texture per letter, loaded once by text_init) ----------
struct Character {
    unsigned int textureID;
    glm::ivec2   size;
    glm::ivec2   bearing;
    unsigned int advance;
};

static std::map<char, Character> characters;
static GLuint textProgram, textVAO, textVBO;
static int textModelLoc, textColorLoc;
static const float FONT_LOAD_SIZE = 48.0f;
static float fontAscent = 0;

void shapes_init() {
    GLuint vs = compileShader(GL_VERTEX_SHADER, vertexShaderSrc);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSrc);
    program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glDeleteShader(vs);
    glDeleteShader(fs);

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, MAX_VERTICES * FLOATS_PER_VERTEX * sizeof(float), nullptr, GL_STREAM_DRAW);
    // attribute 0: position (2 floats), attribute 1: color (4 floats)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, FLOATS_PER_VERTEX * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, FLOATS_PER_VERTEX * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glUseProgram(program);
    glm::mat4 projection = glm::ortho(0.0f, (float)SCREEN_W, 0.0f, (float)SCREEN_H, -1.0f, 1.0f);
    glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    batch.reserve(MAX_VERTICES * FLOATS_PER_VERTEX);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
}

void text_init() {
    GLuint vs = compileShader(GL_VERTEX_SHADER, textVertexSrc);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, textFragmentSrc);
    textProgram = glCreateProgram();
    glAttachShader(textProgram, vs);
    glAttachShader(textProgram, fs);
    glLinkProgram(textProgram);
    glDeleteShader(vs);
    glDeleteShader(fs);

    glUseProgram(textProgram);
    glm::mat4 projection = glm::ortho(0.0f, (float)SCREEN_W, 0.0f, (float)SCREEN_H, -1.0f, 1.0f);
    glUniformMatrix4fv(glGetUniformLocation(textProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    textModelLoc = glGetUniformLocation(textProgram, "model");
    textColorLoc = glGetUniformLocation(textProgram, "textColor");

    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        std::cerr << "ERROR::FREETYPE: Could not init FreeType Library\n";
        return;
    }

    FT_Face face;
    if (FT_New_Face(ft, ASSET_DIR "/fonts/Cinzel.ttf", 0, &face)) {
        std::cerr << "ERROR::FREETYPE: Failed to load font\n";
        FT_Done_FreeType(ft);
        return;
    }

    FT_Set_Pixel_Sizes(face, 0, (int)FONT_LOAD_SIZE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c = 0; c < 128; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cerr << "ERROR::FREETYPE: Failed to load Glyph\n";
            continue;
        }
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,
                     face->glyph->bitmap.width, face->glyph->bitmap.rows,
                     0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character ch = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            (unsigned int)face->glyph->advance.x
        };
        characters[(char)c] = ch;
        if (ch.bearing.y > fontAscent) fontAscent = (float)ch.bearing.y;
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);
    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
}


// Sends everything collected so far to the GPU in ONE draw call.
void shapes_flush() {
    if (batch.empty()) return;
    glUseProgram(program);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // new storage each time ("orphaning"), so the GPU never has to wait for the old data
    glBufferData(GL_ARRAY_BUFFER, MAX_VERTICES * FLOATS_PER_VERTEX * sizeof(float), nullptr, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, batch.size() * sizeof(float), batch.data());
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(batch.size() / FLOATS_PER_VERTEX));
    batch.clear();
}

void drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, Color c) {
    // effects, done here on the CPU (same math as the old fragment shader)
    float r = c.r, g = c.g, b = c.b;
    if (currentGray) {
        float gray = 0.2126f * r + 0.7152f * g + 0.0722f * b;
        r = g = b = gray;
    }
    r = r + (1.0f - r) * currentTint;      // mix toward red (1, 0, 0)
    g = g + (0.0f - g) * currentTint;
    b = b + (0.0f - b) * currentTint;
    float a = c.a * currentAlpha;

    // transforms, also on the CPU: move each corner with the current model matrix
    float xs[3] = { x1, x2, x3 };
    float ys[3] = { y1, y2, y3 };
    for (int i = 0; i < 3; i++) {
        glm::vec4 p = currentModel * glm::vec4(xs[i], ys[i], 0.0f, 1.0f);
        float v[FLOATS_PER_VERTEX] = { p.x, p.y, r, g, b, a };
        batch.insert(batch.end(), v, v + FLOATS_PER_VERTEX);
    }

    if (batch.size() >= (size_t)(MAX_VERTICES - 3) * FLOATS_PER_VERTEX) shapes_flush();
}

void drawRect(float x, float y, float w, float h, Color c) {
    drawTriangle(x, y,   x + w, y,       x + w, y + h, c);
    drawTriangle(x, y,   x + w, y + h,   x, y + h,     c);
}

void drawCircle(float cx, float cy, float radius, Color c, int segments) {
    const float TWO_PI = 6.2831853f;
    float step = TWO_PI / segments;

    for (int i = 0; i < segments; i++) {
        float a1 = i * step;
        float a2 = (i + 1) * step;

        float x1 = cx + radius * cos(a1);
        float y1 = cy + radius * sin(a1);
        float x2 = cx + radius * cos(a2);
        float y2 = cy + radius * sin(a2);

        drawTriangle(cx, cy, x1, y1, x2, y2, c);
    }
}

void drawEllipse(float cx, float cy, float rx, float ry, Color c, int segments) {
    const float TWO_PI = 6.2831853f;
    float step = TWO_PI / segments;

    for (int i = 0; i < segments; i++) {
        float a1 = i * step;
        float a2 = (i + 1) * step;

        float x1 = cx + rx * cos(a1);
        float y1 = cy + ry * sin(a1);
        float x2 = cx + rx * cos(a2);
        float y2 = cy + ry * sin(a2);

        drawTriangle(cx, cy, x1, y1, x2, y2, c);
    }
}

void drawLine(float x1, float y1, float x2, float y2, float thickness, Color c) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    float len = sqrt(dx * dx + dy * dy);
    if (len == 0) return;

    float nx = -dy / len * thickness / 2;
    float ny =  dx / len * thickness / 2;

    drawTriangle(x1 + nx, y1 + ny,   x1 - nx, y1 - ny,   x2 - nx, y2 - ny, c);
    drawTriangle(x1 + nx, y1 + ny,   x2 - nx, y2 - ny,   x2 + nx, y2 + ny, c);
}

void drawPolygon(const std::vector<glm::vec2>& points, Color c) {
    if (points.size() < 3) return;

    for (size_t i = 1; i + 1 < points.size(); i++) {
        drawTriangle(points[0].x, points[0].y,
                     points[i].x, points[i].y,
                     points[i + 1].x, points[i + 1].y, c);
    }
}

void drawGradientRect(float x, float y, float w, float h, Color bottom, Color top, int steps) {
    float stripH = h / steps;

    for (int i = 0; i < steps; i++) {
        float t = (i + 0.5f) / steps;
        Color c = {
            bottom.r + (top.r - bottom.r) * t,
            bottom.g + (top.g - bottom.g) * t,
            bottom.b + (top.b - bottom.b) * t,
            bottom.a + (top.a - bottom.a) * t
        };
        drawRect(x, y + i * stripH, w, stripH, c);
    }
}

void drawRectCentered(float cx, float cy, float w, float h, Color c) {
    drawRect(cx - w / 2, cy - h / 2, w, h, c);
}

void drawRectOutline(float x, float y, float w, float h, float thickness, Color c) {
    drawRect(x, y, w, thickness, c);                                       // bottom
    drawRect(x, y + h - thickness, w, thickness, c);                       // top
    drawRect(x, y + thickness, thickness, h - 2 * thickness, c);           // left
    drawRect(x + w - thickness, y + thickness, thickness, h - 2 * thickness, c);  // right
}

void drawRoundedRect(float x, float y, float w, float h, float radius, Color c) {
    float r = radius;
    drawRect(x + r, y, w - 2 * r, h, c);              // middle, full height
    drawRect(x, y + r, r, h - 2 * r, c);              // left strip
    drawRect(x + w - r, y + r, r, h - 2 * r, c);      // right strip

    drawCircle(x + r,     y + r,     r, c, 12);       // bottom-left corner
    drawCircle(x + w - r, y + r,     r, c, 12);       // bottom-right
    drawCircle(x + r,     y + h - r, r, c, 12);       // top-left
    drawCircle(x + w - r, y + h - r, r, c, 12);       // top-right
}

void drawArc(float cx, float cy, float radius, float startDeg, float endDeg, float thickness, Color c, int segments) {
    float inner = radius - thickness / 2;
    float outer = radius + thickness / 2;
    float start = glm::radians(startDeg);
    float step = (glm::radians(endDeg) - start) / segments;

    for (int i = 0; i < segments; i++) {
        float a1 = start + i * step;
        float a2 = start + (i + 1) * step;

        float ix1 = cx + inner * cos(a1), iy1 = cy + inner * sin(a1);
        float ox1 = cx + outer * cos(a1), oy1 = cy + outer * sin(a1);
        float ix2 = cx + inner * cos(a2), iy2 = cy + inner * sin(a2);
        float ox2 = cx + outer * cos(a2), oy2 = cy + outer * sin(a2);

        drawTriangle(ix1, iy1, ox1, oy1, ox2, oy2, c);
        drawTriangle(ix1, iy1, ox2, oy2, ix2, iy2, c);
    }
}

void translate(float x, float y) {
    currentModel = glm::translate(currentModel, glm::vec3(x, y, 0.0f));
}

void resetTransform() {
    currentModel = glm::mat4(1.0f);
    matrixStack.clear();
}

void rotate(float degrees) {
    currentModel = glm::rotate(currentModel, glm::radians(degrees), glm::vec3(0.0f, 0.0f, 1.0f));
}

void scale(float sx, float sy) {
    currentModel = glm::scale(currentModel, glm::vec3(sx, sy, 1.0f));
}

void pushMatrix() {
    matrixStack.push_back(currentModel);
}

void popMatrix() {
    if (matrixStack.empty()) return;
    currentModel = matrixStack.back();
    matrixStack.pop_back();
}

void setTint(float amount) {
    currentTint = amount;
}

void setGrayscale(bool on) {
    currentGray = on;
}

void setAlpha(float a) {
    currentAlpha = a;
}

void resetEffects() {
    currentTint = 0.0f;
    currentGray = false;
    currentAlpha = 1.0f;
}

// ---------- Text drawing (adapted from RenderText in learnopengl.com) ----------
float textWidth(const std::string& text, float size) {
    float scale = size / FONT_LOAD_SIZE;
    float w = 0;
    for (char c : text) w += (characters[c].advance >> 6) * scale;
    return w;
}

void drawText(const std::string& text, float x, float y, float size, Color c) {
    float scale = size / FONT_LOAD_SIZE;
    float baseline = y - fontAscent * scale;

    shapes_flush();   // draw the shapes collected so far first, so text lands on top
    glUseProgram(textProgram);
    glUniformMatrix4fv(textModelLoc, 1, GL_FALSE, glm::value_ptr(currentModel));
    glUniform4f(textColorLoc, c.r, c.g, c.b, c.a * currentAlpha);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(textVAO);

    for (char letter : text) {
        Character ch = characters[letter];

        float xpos = x + ch.bearing.x * scale;
        float ypos = baseline - (ch.size.y - ch.bearing.y) * scale;
        float w = ch.size.x * scale;
        float h = ch.size.y * scale;

        float vertices[6][4] = {
            { xpos,     ypos + h, 0.0f, 0.0f },
            { xpos,     ypos,     0.0f, 1.0f },
            { xpos + w, ypos,     1.0f, 1.0f },

            { xpos,     ypos + h, 0.0f, 0.0f },
            { xpos + w, ypos,     1.0f, 1.0f },
            { xpos + w, ypos + h, 1.0f, 0.0f }
        };

        glBindTexture(GL_TEXTURE_2D, ch.textureID);
        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += (ch.advance >> 6) * scale;
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}

void drawTextCentered(const std::string& text, float cx, float y, float size, Color c) {
    drawText(text, cx - textWidth(text, size) / 2, y, size, c);
}
