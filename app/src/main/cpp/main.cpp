#include <BananaSDK/Android.h>
#include <BananaSDK/Renderer.h>
#include <BananaSDK/UI.h>
#include <BananaSDK/Button.h>

class MyApp : public AndroidApp {
public:
    Renderer renderer;
    UIRenderer ui;
    Button myButton;

    void Main() override {

        addListener("windowready", [this]() {
            renderer.Init(getWindow());
            renderer.SetClearColor(0.96f, 0.77f, 0.09f);

            int w = ANativeWindow_getWidth(getWindow());
            int h = ANativeWindow_getHeight(getWindow());
            ui.Init(getActivity(), w, h);

            myButton = Button(100, 300, 400, 80, "");
            myButton.SetColor(0.1f, 0.1f, 0.1f);
            myButton.SetTextColor(1.0f, 1.0f, 1.0f);
            myButton.SetRadius(20.0f);
            myButton.SetOnClick([]() {
                _BANANA_LOGI("Button clicked!");
            });
        });

        addListener("frame", [this]() {
            renderer.BeginFrame();
            myButton.Draw(ui);
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
