#include <BananaSDK/Android.h>
#include <BananaSDK/Renderer.h>

class MyApp : public AndroidApp {
public:
    Renderer renderer;

    void Main() override {

        addListener("windowready", [this]() {
            renderer.Init(getWindow());
            // Yellow! Like a banana 🍌
            renderer.SetClearColor(0.96f, 0.77f, 0.09f);
        });

        addListener("frame", [this]() {
            renderer.DrawFrame();
        });

        addListener("windowlost", [this]() {
            renderer.Destroy();
        });

        addListener("destroy", [this]() {
            renderer.Destroy();
        });
    }
};

startAndroid(MyApp);
