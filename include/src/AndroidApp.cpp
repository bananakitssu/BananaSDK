#include <signal.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <android_native_app_glue.h>
#include "BananaSDK/Button.h"
#include "BananaSDK/Box.h"
#include "BananaSDK/AndroidApp.h"
#include "BananaSDK/UI.h"

// Global log file for crash detection
FILE* g_crashLog = nullptr;

void signalHandler(int sig) {
    if (g_crashLog) {
        fprintf(g_crashLog, "SIGNAL %d received - CRASH!\n", sig);
        fflush(g_crashLog);
        fclose(g_crashLog);
    }
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

    // Write log to multiple paths
    FILE* f = nullptr;
    const char* logPaths[] = {
        "/sdcard/bananasdk_log.txt",
        "/data/local/tmp/bananasdk_log.txt"
    };
    for (int i = 0; i < 2; i++) {
        f = fopen(logPaths[i], "w");
        if (f) break;
    }
    
    g_crashLog = f;
    
    if (f) { 
        fprintf(f, "BananaSDK native code started\n"); 
        fflush(f);
    }

    try {
        if (f) { fprintf(f, "Calling Main()...\n"); fflush(f); }
        Main();
        if (f) { fprintf(f, "Main() completed successfully\n"); fflush(f); }
    } catch (const std::exception& e) {
        if (f) { 
            fprintf(f, "Exception in Main(): %s\n", e.what()); 
            fflush(f);
        }
        if (f) fclose(f);
        throw;
    } catch (...) {
        if (f) { 
            fprintf(f, "Unknown exception in Main()\n"); 
            fflush(f);
        }
        if (f) fclose(f);
        throw;
    }

    while (true) {
        int events;
        android_poll_source* source;
        int ret = ALooper_pollAll(0, nullptr, &events, (void**)&source);

        if (ret >= 0 && source)
            source->process(state, source);

        if (state->destroyRequested) {
            if (f) { 
                fprintf(f, "Destroy requested\n"); 
                fflush(f);
            }
            _Emit("destroy");
            break;
        }

        _Emit("frame");
    }
    
    if (f) fclose(f);
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
    return 0;
}
