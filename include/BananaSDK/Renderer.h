#pragma once

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <android/native_window.h>
#include <android/log.h>

#define _BANANA_TAG "BananaSDK"
#define _BANANA_LOGI(...) __android_log_print(ANDROID_LOG_INFO,  _BANANA_TAG, __VA_ARGS__)
#define _BANANA_LOGE(...) __android_log_print(ANDROID_LOG_ERROR, _BANANA_TAG, __VA_ARGS__)

class Renderer {
public:
    Renderer();
    ~Renderer();

    // Call when window is ready
    bool Init(ANativeWindow* window);

    // Call every frame
    void DrawFrame();

    // Call when window is lost
    void Destroy();

    bool IsReady() const { return m_Ready; }

    // Set clear color (r, g, b — values 0.0 to 1.0)
    void SetClearColor(float r, float g, float b) {
        m_R = r; m_G = g; m_B = b;
    }

private:
    EGLDisplay m_Display;
    EGLSurface m_Surface;
    EGLContext m_Context;
    bool       m_Ready;

    float m_R, m_G, m_B;
};
