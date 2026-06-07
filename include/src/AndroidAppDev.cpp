#include "BananaSDK/AndroidAppDev.h"
#include "BananaSDK/Log.h"
#include <cmath>

AndroidAppDev::AndroidAppDev() {
    m_LastShakeTime    = std::chrono::steady_clock::now();
    m_ShakeWindowStart = std::chrono::steady_clock::now();
}

AndroidAppDev::~AndroidAppDev() {
    if (m_SensorQueue && m_Accelerometer)
        ASensorEventQueue_disableSensor(m_SensorQueue, m_Accelerometer);
}

void AndroidAppDev::_Init(android_app* state) {
    AndroidApp::_Init(state); // calls Main() — user listeners added first

    _SetupSensors(state);

    // windowready fires AFTER user's, so EGL is already active
    addListener("windowready", [this, state]() {
        int w = ANativeWindow_getWidth(getWindow());
        int h = ANativeWindow_getHeight(getWindow());
        m_DevUI.Init(GetActivity(), this, nullptr, w, h);
        _SetupDevMenu(state);
    });

    addListener("touchstart", [this]() {
        if (m_DevMenu.IsVisible())
            m_DevMenu.OnTouch(GetTouchX(), GetTouchY());
    });

    addListener("touchend", [this]() {
        m_DevMenu.OnRelease();
    });
}

void AndroidAppDev::_SetupDevMenu(android_app* state) {
    m_DevMenu.AddItem("Close App", [state]() {
        ANativeActivity_finish(state->activity);
    });
    m_DevMenu.AddLabel("BananaSDK Developer Mode, v1");
    m_MenuSetup = true;
    _BANANA_LOGI("[DevMode] Menu ready");
}

void AndroidAppDev::_SetupSensors(android_app* state) {
    m_SensorManager = ASensorManager_getInstance();
    m_Accelerometer = ASensorManager_getDefaultSensor(
        m_SensorManager, ASENSOR_TYPE_ACCELEROMETER);
    m_SensorQueue = ASensorManager_createEventQueue(
        m_SensorManager, state->looper, LOOPER_ID_USER, nullptr, nullptr);
    if (m_Accelerometer && m_SensorQueue) {
        ASensorEventQueue_enableSensor(m_SensorQueue, m_Accelerometer);
        ASensorEventQueue_setEventRate(m_SensorQueue, m_Accelerometer, (1000L/30)*1000);
        _BANANA_LOGI("[DevMode] Accelerometer ready");
    }
}

void AndroidAppDev::DrawDevOverlay() {
    _PollSensors();
    if (m_DevMenu.IsVisible())
        m_DevMenu.Draw(m_DevUI);
}

void AndroidAppDev::_PollSensors() {
    if (!m_SensorQueue) return;
    ASensorEvent events[8];
    int n;
    while ((n = ASensorEventQueue_getEvents(m_SensorQueue, events, 8)) > 0) {
        for (int i = 0; i < n; i++) {
            if (events[i].type == ASENSOR_TYPE_ACCELEROMETER)
                _DetectShake(events[i].acceleration.x,
                             events[i].acceleration.y,
                             events[i].acceleration.z);
        }
    }
}

void AndroidAppDev::_DetectShake(float x, float y, float z) {
    float mag   = sqrtf(x*x + y*y + z*z);
    float force = fabsf(mag - 9.81f);
    auto  now   = std::chrono::steady_clock::now();

    float windowTime    = std::chrono::duration<float>(now - m_ShakeWindowStart).count();
    float timeSinceLast = std::chrono::duration<float>(now - m_LastShakeTime).count();

    if (windowTime > 2.0f) {
        m_ShakeCount       = 0;
        m_ShakeWindowStart = now;
    }

    if (force > 5.0f && timeSinceLast > 0.1f) {
        m_ShakeCount++;
        m_LastShakeTime = now;
    }

    if (m_ShakeCount >= 2 && !m_DevMenu.IsVisible() && m_MenuSetup) {
        m_ShakeCount = 0;
        m_DevMenu.Show(m_DevUI);
        _BANANA_LOGI("[DevMode] Dev menu opened by shake");
    }
}
