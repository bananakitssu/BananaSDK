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

            int number = 20.0f;
            int w = ANativeWindow_getWidth(getWindow());
            int h = ANativeWindow_getHeight(getWindow());
            ui.Init(getActivity(), this, renderer, w, h);

            myButton = Button(20, 60, 250, 70, "Click");
            //myButton.SetColor(1.0f, 0.0f, 0.0f);
            //myButton.SetTextColor(1.0f, 1.0f, 1.0f);
            //myButton.SetRadius(20.0f);
            myButton.SetOnClick([&]() {
                buttonRadius -= 1.0f;
                myButton.SetRadius(buttonRadius);
                _BANANA_LOGI("Button clicked!");
            });
        });

        addListener("touchstart", [this]() {
            myButton.OnTouch(GetTouchX(), GetTouchY());
        });

        addListener("touchend", [this]() {
            myButton.OnRelease();
        });

        /*addListener("frame", [this]() {
            renderer.BeginFrame();
            myButton.Draw(ui);
            DrawDevOverlay();
            renderer.EndFrame();
        });*/

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
