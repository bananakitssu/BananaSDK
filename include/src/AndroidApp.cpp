#include "BananaSDK/Android.h"

void AndroidApp::_Init(android_app* state) {
    state->userData  = this;
    state->onAppCmd  = _HandleCmd;

    // Write log to readable file
    FILE* f = fopen("/sdcard/bananasdk_log.txt", "w");
    if (f) { fprintf(f, "BananaSDK started\n"); fclose(f); }

    Main();

    while (true) {
        int events;
        android_poll_source* source;
        int ret = ALooper_pollAll(0, nullptr, &events, (void**)&source);

        if (ret >= 0 && source)
            source->process(state, source);

        if (state->destroyRequested) {
            FILE* f2 = fopen("/sdcard/bananasdk_log.txt", "a");
            if (f2) { fprintf(f2, "Destroy requested\n"); fclose(f2); }
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
