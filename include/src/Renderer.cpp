#include "BananaSDK/Renderer.h"
#include "BananaSDK/Log.h"

Renderer::Renderer()
    : m_Display(EGL_NO_DISPLAY),
      m_Surface(EGL_NO_SURFACE),
      m_Context(EGL_NO_CONTEXT),
      m_Ready(false),
      m_R(0.2f), m_G(0.2f), m_B(0.2f)
{}

Renderer::~Renderer() {
    Destroy();
}

bool Renderer::Init(ANativeWindow* window) {
    // 1. Get EGL display
    m_Display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (m_Display == EGL_NO_DISPLAY) {
        _BANANA_LOGE("Failed to get EGL display.");
        return false;
    }

    // 2. Initialize EGL
    EGLint major, minor;
    if (!eglInitialize(m_Display, &major, &minor)) {
        _BANANA_LOGE("Failed to initialize EGL.");
        return false;
    }

    // 3. Choose config
    const EGLint attribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
        EGL_BLUE_SIZE,  8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE,   8,
        EGL_STENCIL_SIZE, 8,
        EGL_NONE
    };

    EGLConfig config;
    EGLint numConfigs;
    if (!eglChooseConfig(m_Display, attribs, &config, 1, &numConfigs)) {
        _BANANA_LOGE("Failed to choose EGL config.");
        return false;
    }

    // 4. Create window surface
    m_Surface = eglCreateWindowSurface(m_Display, config, window, nullptr);
    if (m_Surface == EGL_NO_SURFACE) {
        _BANANA_LOGE("Failed to create EGL surface.");
        return false;
    }

    // 5. Create context (OpenGL ES 2.0)
    const EGLint contextAttribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };
    m_Context = eglCreateContext(m_Display, config, EGL_NO_CONTEXT, contextAttribs);
    if (m_Context == EGL_NO_CONTEXT) {
        _BANANA_LOGE("Failed to create EGL context.");
        return false;
    }

    // 6. Bind everything together
    if (!eglMakeCurrent(m_Display, m_Surface, m_Surface, m_Context)) {
        _BANANA_LOGE("Failed to make EGL context current.");
        return false;
    }

    m_Ready = true;
    _BANANA_LOGI("Renderer initialized successfully.");
    return true;
}

void Renderer::BeginFrame() {
    if (!m_Ready) return;
    glDisable(GL_SCISSOR_TEST);
    glClearColor(m_R, m_G, m_B, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::EndFrame() {
    if (!m_Ready) return;
    eglSwapBuffers(m_Display, m_Surface);
}

void Renderer::Destroy() {
    if (m_Display != EGL_NO_DISPLAY) {
        eglMakeCurrent(m_Display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

        if (m_Context != EGL_NO_CONTEXT)
            eglDestroyContext(m_Display, m_Context);

        if (m_Surface != EGL_NO_SURFACE)
            eglDestroySurface(m_Display, m_Surface);

        eglTerminate(m_Display);
    }

    m_Display = EGL_NO_DISPLAY;
    m_Surface = EGL_NO_SURFACE;
    m_Context = EGL_NO_CONTEXT;
    m_Ready   = false;

    _BANANA_LOGI("Renderer destroyed.");
}
