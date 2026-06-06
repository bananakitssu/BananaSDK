#include <BananaSDK/Android.h>
#include <BananaSDK/Renderer.h>
#include <BananaSDK/UI.h>
#include <BananaSDK/Button.h>

class MyApp : public AndroidAppDev {
public:
    Renderer renderer;
    UIRenderer ui;
    Button myButton;
    int frameCount = 0;

    void Main() override {

        addListener("windowready", [this]() {
            renderer.Init(getWindow());
            renderer.SetClearColor(0.96f, 0.77f, 0.09f);

            int w = ANativeWindow_getWidth(getWindow());
            int h = ANativeWindow_getHeight(getWindow());
            ui.Init(getActivity(), w, h);

            myButton = Button(20, 60, 250, 70, "Click");
            myButton.SetOnClick([]() {
                _BANANA_LOGI("Button clicked!");
            });
        });

        addListener("touchstart", [this]() {
            myButton.OnTouch(GetTouchX(), GetTouchY());
        });

        addListener("touchend", [this]() {
            myButton.OnRelease();
        });

        addListener("frame", [this]() {
            frameCount++;
            renderer.BeginFrame();
            myButton.Draw(ui);
            
            // DEBUG: Draw frame counter and shake status
            ui.DrawText(10, 10, "Frame: " + std::to_string(frameCount), 
                       1.0f, 1.0f, 1.0f, 1.0f, 20.0f);
            ui.DrawText(10, 40, "Shake to open menu", 
                       1.0f, 1.0f, 1.0f, 1.0f, 20.0f);
            
            DrawDevOverlay();
            renderer.EndFrame();
        });

        addListener("windowlost", [this]() {
            ui.Destroy();
            renderer.Destroy();
        });

        addListener("destroy", [this]() {
            ui.Destroy();
            renderer.Destroy();
        });
    }
};

startAndroid(MyApp);
