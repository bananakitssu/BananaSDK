#pragma once
#include "AndroidApp.h"
#include "UI.h"
#include "PopupMenu.h"
#include <android/sensor.h>
#include <chrono>

class AndroidAppDev : public AndroidApp {
public:
    AndroidAppDev();
    ~AndroidAppDev();

    void _Init(android_app* state) override;
    void DrawDevOverlay(); // call in frame listener before EndFrame

protected:
    bool m_DevMode = true;

private:
    void _SetupSensors(android_app* state);
    void _PollSensors();
    void _DetectShake(float x, float y, float z);
    void _SetupDevMenu(android_app* state);

    ASensorManager*    m_SensorManager = nullptr;
    const ASensor*     m_Accelerometer = nullptr;
    ASensorEventQueue* m_SensorQueue   = nullptr;

    int  m_ShakeCount = 0;
    std::chrono::steady_clock::time_point m_LastShakeTime;
    std::chrono::steady_clock::time_point m_ShakeWindowStart;

    UIRenderer m_DevUI;
    PopupMenu  m_DevMenu;
    bool       m_MenuSetup = false;
};
