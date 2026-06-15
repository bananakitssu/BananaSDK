#include <signal.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <android_native_app_glue.h>
#include "BananaSDK/Button.h"
#include "BananaSDK/Box.h"
#include "BananaSDK/AndroidApp.h"
#include "BananaSDK/UI.h"

extern AndroidApp* g_AppInstance;

void signalHandler(int sig) {
    std::exit(1);
}

void AndroidApp::_Init(android_app* state) {
    // Setup crash handler
    signal(SIGSEGV, signalHandler);
    signal(SIGABRT, signalHandler);
    
    m_State          = state;
    state->userData  = this;
    state->onAppCmd  = _HandleCmd;
    state->onInputEvent = _HandleInput;

    g_AppInstance = this;
    
    Main();

    while (true) {
        int events;
        android_poll_source* source;
        int ret = ALooper_pollAll(0, nullptr, &events, (void**)&source);

        if (ret >= 0 && source)
            source->process(state, source);

        if (state->destroyRequested) {
            _Emit("destroy");
            break;
        }

        _Emit("frame");
    }
}

void AndroidApp::_HandleCmd(android_app* state, int32_t cmd) {
    AndroidApp* self = static_cast<AndroidApp*>(state->userData);
    if (!self) return;

    switch (cmd) {
        case APP_CMD_RESUME:      self->_Emit("resume");      break;
        case APP_CMD_PAUSE:       self->_Emit("pause");       break;
        case APP_CMD_STOP:        self->_Emit("stop");        break;
        case APP_CMD_START:       self->_Emit("start");       break;
        case APP_CMD_GAINED_FOCUS: self->_Emit("focus");      break;
        case APP_CMD_LOST_FOCUS:  self->_Emit("blur");        break;
        case APP_CMD_INIT_WINDOW:
            self->m_Window = state->window;
            _BANANA_LOGI("windowready");
            self->_Emit("windowready");
            break;
        case APP_CMD_TERM_WINDOW: self->_Emit("windowlost");  break;
        case APP_CMD_DESTROY:     self->_Emit("destroy");     break;
        default: break;
    }
}

int32_t AndroidApp::_HandleInput(android_app* state, AInputEvent* event) {
    AndroidApp* self = static_cast<AndroidApp*>(state->userData);
    if (!self) return 0;
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        int32_t action = AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK;
        self->m_TouchX = AMotionEvent_getX(event, 0);
        self->m_TouchY = AMotionEvent_getY(event, 0);
        if (action == AMOTION_EVENT_ACTION_DOWN) self->_Emit("touchstart");
        else if (action == AMOTION_EVENT_ACTION_UP) self->_Emit("touchend");
        else if (action == AMOTION_EVENT_ACTION_MOVE)  self->_Emit("touchmove");
        return 1;
    }

    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY) {
        if (self->IsImeFocused()) return 0;
        
        if (AKeyEvent_getAction(event) == AKEY_EVENT_ACTION_DOWN) {
            int32_t keyCode   = AKeyEvent_getKeyCode(event);
            int32_t metaState = AKeyEvent_getMetaState(event);

            JNIEnv* env = nullptr;
            bool attached = false;
            jint res = self->GetActivity()->vm->GetEnv((void**)&env, JNI_VERSION_1_6);
            if (res == JNI_EDETACHED) {
                self->GetActivity()->vm->AttachCurrentThread(&env, nullptr);
                attached = true;
            }

            jclass  kc  = env->FindClass("android/view/KeyEvent");
            jobject ke  = env->NewObject(kc, env->GetMethodID(kc, "<init>", "(II)V"),
                                         (jint)AKEY_EVENT_ACTION_DOWN, (jint)keyCode);
            int32_t uni = (int32_t)env->CallIntMethod(ke,
                            env->GetMethodID(kc, "getUnicodeChar", "(I)I"), (jint)metaState);
            env->DeleteLocalRef(ke);
            env->DeleteLocalRef(kc);
        
            if (attached) self->GetActivity()->vm->DetachCurrentThread();

            self->m_LastKeyCode = keyCode;
            self->m_LastUnicode = uni;
            self->_Emit("keydown");
        }
        return 1;
    }

    return 0;
}

bool AndroidApp::DispatchTouch(float x, float y) {
    for (auto& rg : m_RadioGroups)
        if (rg->OnTouch(x, y)) return true;
    for (auto& box : m_ScrollBoxes)
        if (box->OnTouch(x, y)) return true;

    bool closedSomething = false;
    for (auto& el : m_Elements) {
        std::visit([x, y, &closedSomething](auto& ptr) {
            using T = std::decay_t<decltype(*ptr)>;
            if constexpr (std::is_same_v<T, Dropdown> || std::is_same_v<T, MultiDropdown>) {
                if (!ptr->HitTest(x, y) && ptr->IsOpen()) {
                    ptr->Close();
                    closedSomething = true;
                }
            }
        }, el);
    }
    if (closedSomething) return true;

    for (auto& el : m_Elements) {
        std::visit([&](auto& ptr) {
            using T = std::decay_t<decltype(*ptr)>;
            if constexpr (std::is_same_v<T, InputField> || std::is_same_v<T, Textarea>)
                if (!ptr->HitTest(x, y) && ptr->IsFocused())
                    ptr->OnFocusLost();
        }, el);
    }
    for (int i = (int)m_Elements.size() - 1; i >= 0; i--) {
        bool hit = std::visit([x, y](auto& ptr) -> bool {
            return ptr->HitTest(x, y);
        }, m_Elements[i]);
        if (hit) {
            std::visit([x, y](auto& ptr) { ptr->OnTouch(x, y); }, m_Elements[i]);
            return true;
        }
    }
    return false;
}

void AndroidApp::DispatchTouchMove(float x, float y) {
    for (auto& rg : m_RadioGroups)
        rg->OnTouchMove(x, y);
    for (auto& box : m_ScrollBoxes)
        box->OnTouchMove(x, y);
    for (auto& el : m_Elements)
        std::visit([x, y](auto& ptr) { ptr->OnTouchMove(x, y); }, el);
}

void AndroidApp::DispatchRelease(float x, float y) {
    for (auto& rg : m_RadioGroups)
        rg->OnRelease(x, y);
    for (auto& box : m_ScrollBoxes)
        box->OnRelease(x, y);
    for (auto& el : m_Elements)
        std::visit([x, y](auto& ptr) { ptr->OnRelease(x, y); }, el);
}

void AndroidApp::DispatchKey(int32_t keyCode, int32_t unicode) {
    for (auto& el : m_Elements) {
        std::visit([keyCode, unicode](auto& ptr) {
            using T = std::decay_t<decltype(*ptr)>;
            if constexpr (std::is_same_v<T, InputField> || std::is_same_v<T, Textarea>)
                ptr->OnKey(keyCode, unicode);
        }, el);
    }
}

void AndroidApp::OnTextChanged(const std::string& text) {
    for (auto& el : m_Elements) {
        std::visit([&text](auto& ptr) {
            using T = std::decay_t<decltype(*ptr)>;
            if constexpr (std::is_same_v<T, InputField> || std::is_same_v<T, Textarea>)
                if (ptr->IsFocused())
                    ptr->SetTextFromIME(text);
        }, el);
    }
}

void AndroidApp::OnSubmit(const std::string& text) {
    for (auto& el : m_Elements) {
        std::visit([&text](auto& ptr) {
            using T = std::decay_t<decltype(*ptr)>;
            if constexpr (std::is_same_v<T, InputField>)
                if (ptr->IsFocused())
                    ptr->TriggerSubmit(text);
        }, el);
    }
}
