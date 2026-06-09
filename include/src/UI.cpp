#include "BananaSDK/UI.h"
#include "BananaSDK/Log.h"
#include "BananaSDK/AndroidApp.h"
#include "BananaSDK/AndroidAppDev.h"
#include "BananaSDK/Renderer.h"
#include "BananaSDK/Button.h"
#include "BananaSDK/Box.h"
#include <jni.h>
#include <vector>
#include <string>
#include <any>
#include <variant>

// ── Shaders ──────────────────────────────────────────────────────────────────

static const char* RECT_VERT = R"glsl(
attribute vec2 a_pos;
uniform   vec2 u_res;
void main() {
    vec2 clip = (a_pos / u_res) * 2.0 - 1.0;
    gl_Position = vec4(clip.x, -clip.y, 0.0, 1.0);
}
)glsl";

static const char* RECT_FRAG = R"glsl(
precision mediump float;
uniform vec4  u_color;
uniform vec2  u_tl;
uniform vec2  u_size;
uniform float u_radius;
uniform float u_sh;

float sdfRoundRect(vec2 p, vec2 halfSize, float r) {
    vec2 q = abs(p) - halfSize + r;
    return length(max(q, 0.0)) - r;
}

void main() {
    vec2  fc     = vec2(gl_FragCoord.x, u_sh - gl_FragCoord.y);
    vec2  center = u_tl + u_size * 0.5;
    float d      = sdfRoundRect(fc - center, u_size * 0.5, u_radius);
    float alpha  = 1.0 - smoothstep(-0.5, 0.5, d);
    gl_FragColor = vec4(u_color.rgb, u_color.a * alpha);
}
)glsl";

static const char* TEX_VERT = R"glsl(
attribute vec2 a_pos;
attribute vec2 a_uv;
uniform   vec2 u_res;
varying   vec2 v_uv;
void main() {
    vec2 clip = (a_pos / u_res) * 2.0 - 1.0;
    gl_Position = vec4(clip.x, -clip.y, 0.0, 1.0);
    v_uv = a_uv;
}
)glsl";

static const char* TEX_FRAG = R"glsl(
precision mediump float;
uniform sampler2D u_tex;
varying vec2      v_uv;
void main() {
    gl_FragColor = texture2D(u_tex, v_uv);
}
)glsl";

// ── Helpers ───────────────────────────────────────────────────────────────────

GLuint UIRenderer::CompileShader(GLenum type, const char* src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    GLint ok;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char buf[512];
        glGetShaderInfoLog(s, 512, nullptr, buf);
        _BANANA_LOGE("Shader error: %s", buf);
        glDeleteShader(s);
        return 0;
    }
    return s;
}

GLuint UIRenderer::CreateProgram(const char* vert, const char* frag) {
    GLuint vs = CompileShader(GL_VERTEX_SHADER,   vert);
    GLuint fs = CompileShader(GL_FRAGMENT_SHADER, frag);
    if (!vs || !fs) return 0;
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);
    glDeleteShader(vs);
    glDeleteShader(fs);
    GLint ok;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) {
        char buf[512];
        glGetProgramInfoLog(prog, 512, nullptr, buf);
        _BANANA_LOGE("Program link error: %s", buf);
        glDeleteProgram(prog);
        return 0;
    }
    return prog;
}

// ── Init / Destroy ────────────────────────────────────────────────────────────

bool UIRenderer::Init(ANativeActivity* activity, std::variant<AndroidApp*, AndroidAppDev*> app, int w, int h, Renderer* renderer) {
    m_Activity = activity;
    m_Width    = w;
    m_Height   = h;
    m_RectProg = CreateProgram(RECT_VERT, RECT_FRAG);
    m_TexProg  = CreateProgram(TEX_VERT,  TEX_FRAG);
    if (!m_RectProg || !m_TexProg) {
        _BANANA_LOGE("UIRenderer: shader init failed");
        return false;
    }
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_Ready = true;
    if (renderer != nullptr) {
        app->addListener("frame", [this, app, renderer]() {
            if (renderer != nullptr) {
                renderer->BeginFrame();
            }
        
            for (auto& element : app->getElements()) {
                std::visit([this](auto& visualItem) {
                    visualItem->Draw(*this);
                    //using ItemType = std::decay_t<decltype(visualItem)>;
                    //const_cast<ItemType&>(visualItem)->Draw(*this);
                }, element);
            }
            
            std::visit([this](auto* app_) {
                if constexpr (std::is_same_v<decltype(app), AndroidAppDev*>) {
                    app_->DrawDevOverlay();
                }
            }, app);
        
            if (renderer != nullptr) {
                renderer->EndFrame();
            }
        });
    } else {
        /*app->addListener("beforeendframe", [this, app, renderer]() {
            if constexpr (std::is_same_v<decltype(app), AndroidAppDev*>) {
                app->DrawDevOverlay();
            }
        });*/
    }
    _BANANA_LOGI("UIRenderer ready (%dx%d)", w, h);
    return true;
}

void UIRenderer::Destroy() {
    if (m_RectProg) { glDeleteProgram(m_RectProg); m_RectProg = 0; }
    if (m_TexProg)  { glDeleteProgram(m_TexProg);  m_TexProg  = 0; }
    m_Ready = false;
}

// ── DrawRect ──────────────────────────────────────────────────────────────────

void UIRenderer::DrawRect(float x, float y, float w, float h,
                           float r, float g, float b, float a,
                           float radius) {
    if (!m_Ready) return;
    float verts[] = { x, y,  x+w, y,  x, y+h,  x+w, y+h };
    glUseProgram(m_RectProg);
    glUniform2f(glGetUniformLocation(m_RectProg, "u_res"),    (float)m_Width, (float)m_Height);
    glUniform4f(glGetUniformLocation(m_RectProg, "u_color"),  r, g, b, a);
    glUniform2f(glGetUniformLocation(m_RectProg, "u_tl"),     x, y);
    glUniform2f(glGetUniformLocation(m_RectProg, "u_size"),   w, h);
    glUniform1f(glGetUniformLocation(m_RectProg, "u_radius"), radius);
    glUniform1f(glGetUniformLocation(m_RectProg, "u_sh"),     (float)m_Height);
    GLint pos = glGetAttribLocation(m_RectProg, "a_pos");
    glVertexAttribPointer(pos, 2, GL_FLOAT, GL_FALSE, 0, verts);
    glEnableVertexAttribArray(pos);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDisableVertexAttribArray(pos);
}

// ── Text (JNI Canvas → GL Texture) ───────────────────────────────────────────

UIRenderer::TextTexture UIRenderer::MakeTextTexture(
    const std::string& text, float fontSize,
    float r, float g, float b, float a)
{
    TextTexture result;
    if (!m_Activity || text.empty()) return result;

    // Attach to JVM only if needed
    JNIEnv* env = nullptr;
    bool attached = false;
    jint res = m_Activity->vm->GetEnv((void**)&env, JNI_VERSION_1_6);
    if (res == JNI_EDETACHED) {
        m_Activity->vm->AttachCurrentThread(&env, nullptr);
        attached = true;
    } else if (res != JNI_OK) return result;

    jclass clsBitmap = env->FindClass("android/graphics/Bitmap");
    jclass clsCfg    = env->FindClass("android/graphics/Bitmap$Config");
    jclass clsCanvas = env->FindClass("android/graphics/Canvas");
    jclass clsPaint  = env->FindClass("android/graphics/Paint");
    jclass clsFMI    = env->FindClass("android/graphics/Paint$FontMetricsInt");

    // Build Paint
    jobject paint = env->NewObject(clsPaint, env->GetMethodID(clsPaint, "<init>", "()V"));
    env->CallVoidMethod(paint, env->GetMethodID(clsPaint, "setTextSize", "(F)V"), (jfloat)fontSize);
    env->CallVoidMethod(paint, env->GetMethodID(clsPaint, "setAntiAlias", "(Z)V"), (jboolean)JNI_TRUE);
    jint color = ((int)(a*255)<<24)|((int)(r*255)<<16)|((int)(g*255)<<8)|(int)(b*255);
    env->CallVoidMethod(paint, env->GetMethodID(clsPaint, "setColor", "(I)V"), color);

    jstring jtext = env->NewStringUTF(text.c_str());

    // Measure
    float textW = env->CallFloatMethod(paint,
        env->GetMethodID(clsPaint, "measureText", "(Ljava/lang/String;)F"), jtext);

    jobject fmi = env->NewObject(clsFMI, env->GetMethodID(clsFMI, "<init>", "()V"));
    env->CallIntMethod(paint,
        env->GetMethodID(clsPaint, "getFontMetricsInt",
            "(Landroid/graphics/Paint$FontMetricsInt;)I"), fmi);
    int top    = env->GetIntField(fmi, env->GetFieldID(clsFMI, "top",    "I"));
    int bottom = env->GetIntField(fmi, env->GetFieldID(clsFMI, "bottom", "I"));

    int bmpW = (int)textW + 4;
    int bmpH = bottom - top + 4;
    if (bmpW < 1) bmpW = 1;
    if (bmpH < 1) bmpH = 1;

    // Create Bitmap + Canvas
    jobject cfg = env->GetStaticObjectField(clsCfg,
        env->GetStaticFieldID(clsCfg, "ARGB_8888", "Landroid/graphics/Bitmap$Config;"));
    jobject bitmap = env->CallStaticObjectMethod(clsBitmap,
        env->GetStaticMethodID(clsBitmap, "createBitmap",
            "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;"),
        (jint)bmpW, (jint)bmpH, cfg);
    jobject canvas = env->NewObject(clsCanvas,
        env->GetMethodID(clsCanvas, "<init>", "(Landroid/graphics/Bitmap;)V"), bitmap);

    // Draw text
    env->CallVoidMethod(canvas,
        env->GetMethodID(clsCanvas, "drawText",
            "(Ljava/lang/String;FFLandroid/graphics/Paint;)V"),
        jtext, (jfloat)2.0f, (jfloat)(-top + 2), paint);

    // Extract pixels
    jintArray pixArr = env->NewIntArray(bmpW * bmpH);
    env->CallVoidMethod(bitmap,
        env->GetMethodID(clsBitmap, "getPixels", "([IIIIIII)V"),
        pixArr, 0, bmpW, 0, 0, bmpW, bmpH);
    jint* px = env->GetIntArrayElements(pixArr, nullptr);

    // ARGB → RGBA conversion
    std::vector<uint8_t> rgba(bmpW * bmpH * 4);
    for (int i = 0; i < bmpW * bmpH; i++) {
        uint32_t argb = (uint32_t)px[i];
        rgba[i*4+0] = (argb >> 16) & 0xFF;
        rgba[i*4+1] = (argb >>  8) & 0xFF;
        rgba[i*4+2] = (argb >>  0) & 0xFF;
        rgba[i*4+3] = (argb >> 24) & 0xFF;
    }
    env->ReleaseIntArrayElements(pixArr, px, JNI_ABORT);

    // Upload to GL
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bmpW, bmpH, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, rgba.data());

    // Cleanup JNI
    env->DeleteLocalRef(pixArr);
    env->DeleteLocalRef(canvas);
    env->DeleteLocalRef(bitmap);
    env->DeleteLocalRef(fmi);
    env->DeleteLocalRef(jtext);
    env->DeleteLocalRef(paint);
    env->DeleteLocalRef(clsFMI);
    env->DeleteLocalRef(clsPaint);
    env->DeleteLocalRef(clsCanvas);
    env->DeleteLocalRef(clsCfg);
    env->DeleteLocalRef(clsBitmap);
    if (attached) m_Activity->vm->DetachCurrentThread();

    result.id = tex;
    result.w  = bmpW;
    result.h  = bmpH;
    return result;
}

void UIRenderer::DrawText(float x, float y, const std::string& text,
                           float r, float g, float b, float a,
                           float fontSize) {
    if (!m_Ready || text.empty()) return;
    TextTexture tt = MakeTextTexture(text, fontSize, r, g, b, a);
    if (!tt.id) return;

    float verts[] = {
        x,        y,        0.0f, 0.0f,
        x + tt.w, y,        1.0f, 0.0f,
        x,        y + tt.h, 0.0f, 1.0f,
        x + tt.w, y + tt.h, 1.0f, 1.0f,
    };

    glUseProgram(m_TexProg);
    glUniform2f(glGetUniformLocation(m_TexProg, "u_res"), (float)m_Width, (float)m_Height);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tt.id);
    glUniform1i(glGetUniformLocation(m_TexProg, "u_tex"), 0);

    GLint posLoc = glGetAttribLocation(m_TexProg, "a_pos");
    GLint uvLoc  = glGetAttribLocation(m_TexProg, "a_uv");
    glVertexAttribPointer(posLoc, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), verts);
    glVertexAttribPointer(uvLoc,  2, GL_FLOAT, GL_FALSE, 4*sizeof(float), verts + 2);
    glEnableVertexAttribArray(posLoc);
    glEnableVertexAttribArray(uvLoc);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDisableVertexAttribArray(posLoc);
    glDisableVertexAttribArray(uvLoc);

    glDeleteTextures(1, &tt.id);
}
