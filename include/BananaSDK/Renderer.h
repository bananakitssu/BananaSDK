#pragma once

#include <android/native_window.h>
#include <android/log.h>

extern "C" {
    #include <EGL/egl.h>
    #include <GLES2/gl2.h>
}

class Renderer {
public:
    Renderer();
    ~Renderer();

    bool Init(ANativeWindow* window);
    void BeginFrame();
    void EndFrame();
    void Destroy();

    bool IsReady() const { return m_Ready; }

    void SetClearColor(float r, float g, float b) {
        m_R = r; m_G = g; m_B = b;
    }

private:
    EGLDisplay m_Display;
    EGLSurface m_Surface;
    EGLContext m_Context;
    bool       m_Ready;
    float      m_R, m_G, m_B;
};
