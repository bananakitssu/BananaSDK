#include <BananaSDK/Android.h>
#include <BananaSDK/Renderer.h>
#include <BananaSDK/UI.h>
#include <BananaSDK/Button.h>

class MyApp : public AndroidAppDev {
public:
    Renderer renderer;
    UIRenderer ui;
    Button myButton;
    float buttonRadius = 20.0f;

    void Main() override {

        addListener("windowready", [this]() {
            renderer.Init(getWindow());
            renderer.SetClearColor(0.96f, 0.77f, 0.09f);

            int w = ANativeWindow_getWidth(getWindow());
            int h = ANativeWindow_getHeight(getWindow());
            ui.Init(getActivity(), w, h);

            myButton = Button(20, 60, 250, 70, "Click");
            myButton.SetRadius(buttonRadius);
            myButton.SetOnClick([this]() {
                buttonRadius -= 2.0f;
                if (buttonRadius < 0.0f) buttonRadius = 20.0f;
                myButton.SetRadius(buttonRadius);
                _BANANA_LOGI("Button clicked! Radius: %f", buttonRadius);
            });
        });

        addListener("touchstart", [this]() {
            myButton.OnTouch(GetTouchX(), GetTouchY());
        });

        addListener("touchend", [this]() {
            myButton.OnRelease();
        });

        addListener("frame", [this]() {
            renderer.BeginFrame();
            myButton.Draw(ui);
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
