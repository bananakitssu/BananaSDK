#pragma once
#include <GLES2/gl2.h>
#include <android/native_activity.h>
#include <string>
#include <variant>

class AndroidApp;
class AndroidAppDev;
class Renderer;

class UIRenderer {
public:
    UIRenderer() = default;
    ~UIRenderer() { Destroy(); }

    bool Init(ANativeActivity* activity, std::variant<AndroidApp*, AndroidAppDev*> app, int width, int height, Renderer* renderer = nullptr);
    void Destroy();

    // x, y = top-left corner in screen pixels (y-down)
    void DrawRect(float x, float y, float w, float h,
                  float r, float g, float b, float a = 1.0f,
                  float radius = 0.0f);

    void DrawText(float x, float y, const std::string& text,
                  float r, float g, float b, float a = 1.0f,
                  float fontSize = 32.0f);

    void DrawRing(float cx, float cy, float radius, float stroke,
               float startAngle, float sweepAngle,
               float r, float g, float b, float a);

    float MeasureText(const std::string& text, float fontSize);

    int  GetWidth()  const { return m_Width; }
    int  GetHeight() const { return m_Height; }
    bool IsReady()   const { return m_Ready; }

    void PushScissor(float x, float y, float w, float h);
    void PopScissor();
    void PushRoundedScissor(float x, float y, float w, float h, float radius);
    void PopRoundedScissor();

private:
    GLuint CompileShader(GLenum type, const char* src);
    GLuint CreateProgram(const char* vert, const char* frag);

    struct TextTexture { GLuint id = 0; int w = 0; int h = 0; };
    TextTexture MakeTextTexture(const std::string& text, float fontSize,
                                 float r, float g, float b, float a);

    ANativeActivity* m_Activity = nullptr;
    unsigned int m_StencilProg = 0;
    unsigned int m_RingProg = 0;
    GLuint m_RectProg = 0;
    GLuint m_TexProg  = 0;
    int    m_Width    = 0;
    int    m_Height   = 0;
    bool   m_Ready    = false;
};
