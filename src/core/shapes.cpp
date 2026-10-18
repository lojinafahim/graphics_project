// =====================================================================
//  shapes.cpp  -  SHARED. Implementation of the drawing helpers.
//  You normally don't need to read this file - just use shapes.h.
// =====================================================================
#include "shapes.h"
#include "config.h"
#include <glad/gl.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <iostream>

#include "stb_easy_font.h"   // tiny public-domain bitmap font (single header)

static const float PI = 3.14159265358979f;

static GLuint s_program = 0, s_vao = 0, s_vbo = 0;
static GLint  s_uProj, s_uModel, s_uColor, s_uTint, s_uGray, s_uAlpha;

static std::vector<glm::mat4> s_stack;
static glm::mat4 s_model(1.0f);
static float s_tint = 0.0f, s_alpha = 1.0f;
static bool  s_gray = false;

static const char* VERT_SRC = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
uniform mat4 uProj;
uniform mat4 uModel;
void main() {
    gl_Position = uProj * uModel * vec4(aPos, 0.0, 1.0);
}
)";

static const char* FRAG_SRC = R"(
#version 330 core
out vec4 FragColor;
uniform vec4  uColor;
uniform float uTint;    // 0..1 red damage tint
uniform int   uGray;    // 1 = grayscale
uniform float uAlpha;   // global fade
void main() {
    vec3 c = uColor.rgb;
    if (uTint > 0.0) {
        c = mix(c, vec3(0.9, 0.05, 0.05), uTint * 0.75);
    }
    if (uGray == 1) {
        float l = dot(c, vec3(0.299, 0.587, 0.114));
        c = vec3(l) * 0.8;
    }
    FragColor = vec4(c, uColor.a * uAlpha);
}
)";

static GLuint compile(GLenum type, const char* src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    int ok;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[512];
        glGetShaderInfoLog(s, 512, nullptr, log);
        std::cerr << "[shapes] shader error:\n" << log << "\n";
    }
    return s;
}

void shapes_init() {
    GLuint vs = compile(GL_VERTEX_SHADER, VERT_SRC);
    GLuint fs = compile(GL_FRAGMENT_SHADER, FRAG_SRC);
    s_program = glCreateProgram();
    glAttachShader(s_program, vs);
    glAttachShader(s_program, fs);
    glLinkProgram(s_program);
    glDeleteShader(vs);
    glDeleteShader(fs);

    s_uProj  = glGetUniformLocation(s_program, "uProj");
    s_uModel = glGetUniformLocation(s_program, "uModel");
    s_uColor = glGetUniformLocation(s_program, "uColor");
    s_uTint  = glGetUniformLocation(s_program, "uTint");
    s_uGray  = glGetUniformLocation(s_program, "uGray");
    s_uAlpha = glGetUniformLocation(s_program, "uAlpha");

    glGenVertexArrays(1, &s_vao);
    glGenBuffers(1, &s_vbo);
    glBindVertexArray(s_vao);
    glBindBuffer(GL_ARRAY_BUFFER, s_vbo);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glUseProgram(s_program);
    glm::mat4 proj = glm::ortho(0.0f, (float)SCREEN_W, 0.0f, (float)SCREEN_H, -1.0f, 1.0f);
    glUniformMatrix4fv(s_uProj, 1, GL_FALSE, glm::value_ptr(proj));

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

// Sends a list of (x,y) points and draws them with the current settings.
static void submit(const std::vector<float>& verts, GLenum mode, Color c) {
    if (verts.empty()) return;
    glUseProgram(s_program);
    glUniformMatrix4fv(s_uModel, 1, GL_FALSE, glm::value_ptr(s_model));
    glUniform4f(s_uColor, c.r, c.g, c.b, c.a);
    glUniform1f(s_uTint, s_tint);
    glUniform1i(s_uGray, s_gray ? 1 : 0);
    glUniform1f(s_uAlpha, s_alpha);
    glBindVertexArray(s_vao);
    glBindBuffer(GL_ARRAY_BUFFER, s_vbo);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_DYNAMIC_DRAW);
    glDrawArrays(mode, 0, (GLsizei)(verts.size() / 2));
}

// ---------------------------------------------------------------- shapes
void drawRect(float x, float y, float w, float h, Color c) {
    std::vector<float> v = { x, y,  x + w, y,  x + w, y + h,
                             x, y,  x + w, y + h,  x, y + h };
    submit(v, GL_TRIANGLES, c);
}

void drawRectCentered(float cx, float cy, float w, float h, Color c) {
    drawRect(cx - w / 2, cy - h / 2, w, h, c);
}

void drawGradientRect(float x, float y, float w, float h, Color b, Color t, int steps) {
    float sh = h / steps;
    for (int i = 0; i < steps; i++) {
        float k = (i + 0.5f) / steps;
        Color c = { b.r + (t.r - b.r) * k, b.g + (t.g - b.g) * k,
                    b.b + (t.b - b.b) * k, b.a + (t.a - b.a) * k };
        drawRect(x, y + i * sh, w, sh + 0.5f, c);
    }
}

void drawRoundedRect(float x, float y, float w, float h, float r, Color c) {
    if (r * 2 > w) r = w / 2;
    if (r * 2 > h) r = h / 2;
    std::vector<glm::vec2> pts;
    const int seg = 6;
    float cx[4] = { x + w - r, x + r, x + r, x + w - r };
    float cy[4] = { y + h - r, y + h - r, y + r, y + r };
    for (int corner = 0; corner < 4; corner++)
        for (int i = 0; i <= seg; i++) {
            float a = (corner * 90.0f + 90.0f * i / seg) * PI / 180.0f;
            pts.push_back({ cx[corner] + r * std::cos(a), cy[corner] + r * std::sin(a) });
        }
    drawPolygon(pts, c);
}

void drawRectOutline(float x, float y, float w, float h, float t, Color c) {
    drawRect(x, y, w, t, c);
    drawRect(x, y + h - t, w, t, c);
    drawRect(x, y, t, h, c);
    drawRect(x + w - t, y, t, h, c);
}

void drawEllipse(float cx, float cy, float rx, float ry, Color c, int segments) {
    std::vector<float> v = { cx, cy };
    for (int i = 0; i <= segments; i++) {
        float a = 2 * PI * i / segments;
        v.push_back(cx + rx * std::cos(a));
        v.push_back(cy + ry * std::sin(a));
    }
    submit(v, GL_TRIANGLE_FAN, c);
}

void drawCircle(float cx, float cy, float r, Color c, int segments) {
    drawEllipse(cx, cy, r, r, c, segments);
}

void drawArc(float cx, float cy, float r, float startDeg, float endDeg, float t, Color c, int segments) {
    std::vector<float> v;
    float a0 = startDeg * PI / 180.0f, a1 = endDeg * PI / 180.0f;
    float rIn = r - t / 2, rOut = r + t / 2;
    for (int i = 0; i <= segments; i++) {
        float a = a0 + (a1 - a0) * i / segments;
        v.push_back(cx + rOut * std::cos(a)); v.push_back(cy + rOut * std::sin(a));
        v.push_back(cx + rIn  * std::cos(a)); v.push_back(cy + rIn  * std::sin(a));
    }
    submit(v, GL_TRIANGLE_STRIP, c);
}

void drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, Color c) {
    std::vector<float> v = { x1, y1, x2, y2, x3, y3 };
    submit(v, GL_TRIANGLES, c);
}

void drawLine(float x1, float y1, float x2, float y2, float t, Color c) {
    float dx = x2 - x1, dy = y2 - y1;
    float len = std::sqrt(dx * dx + dy * dy);
    if (len < 0.0001f) return;
    float nx = -dy / len * t / 2, ny = dx / len * t / 2;
    std::vector<float> v = { x1 + nx, y1 + ny,  x2 + nx, y2 + ny,  x2 - nx, y2 - ny,
                             x1 + nx, y1 + ny,  x2 - nx, y2 - ny,  x1 - nx, y1 - ny };
    submit(v, GL_TRIANGLES, c);
}

void drawPolygon(const std::vector<glm::vec2>& pts, Color c) {
    if (pts.size() < 3) return;
    std::vector<float> v;
    for (const auto& p : pts) { v.push_back(p.x); v.push_back(p.y); }
    submit(v, GL_TRIANGLE_FAN, c);
}

// ------------------------------------------------------------------ text
// stb_easy_font letters are ~7 units tall; we scale them to `size` pixels.
static const float FONT_BASE_HEIGHT = 7.0f;

float textWidth(const std::string& text, float size) {
    return stb_easy_font_width((char*)text.c_str()) * (size / FONT_BASE_HEIGHT);
}

void drawText(const std::string& text, float x, float y, float size, Color c) {
    if (text.empty()) return;
    static std::vector<char> buffer;
    buffer.resize(text.size() * 1000 + 256);   // some letters need many quads
    int quads = stb_easy_font_print(0, 0, (char*)text.c_str(), nullptr,
                                    buffer.data(), (int)buffer.size());
    float s = size / FONT_BASE_HEIGHT;
    std::vector<float> v;
    v.reserve(quads * 12);
    const float* q = (const float*)buffer.data();
    for (int i = 0; i < quads; i++) {
        // each vertex = x, y, z, color(4 bytes) = 4 floats; 4 vertices per quad
        float px[4], py[4];
        for (int k = 0; k < 4; k++) {
            px[k] = x + q[(i * 4 + k) * 4 + 0] * s;
            py[k] = y - q[(i * 4 + k) * 4 + 1] * s;   // font is y-down, world is y-up
        }
        float tri[12] = { px[0], py[0], px[1], py[1], px[2], py[2],
                          px[0], py[0], px[2], py[2], px[3], py[3] };
        v.insert(v.end(), tri, tri + 12);
    }
    submit(v, GL_TRIANGLES, c);
}

void drawTextCentered(const std::string& text, float cx, float y, float size, Color c) {
    drawText(text, cx - textWidth(text, size) / 2, y, size, c);
}

// ------------------------------------------------------------ transforms
void pushMatrix() { s_stack.push_back(s_model); }
void popMatrix() {
    if (!s_stack.empty()) { s_model = s_stack.back(); s_stack.pop_back(); }
}
void translate(float x, float y) { s_model = glm::translate(s_model, glm::vec3(x, y, 0)); }
void rotate(float deg) { s_model = glm::rotate(s_model, glm::radians(deg), glm::vec3(0, 0, 1)); }
void scale(float sx, float sy) { s_model = glm::scale(s_model, glm::vec3(sx, sy, 1)); }

// --------------------------------------------------------------- effects
void setTint(float amount) { s_tint = amount < 0 ? 0 : (amount > 1 ? 1 : amount); }
void setGrayscale(bool on) { s_gray = on; }
void setAlpha(float a) { s_alpha = a < 0 ? 0 : (a > 1 ? 1 : a); }
void resetEffects() { s_tint = 0; s_gray = false; s_alpha = 1; }
