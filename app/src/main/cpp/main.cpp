#include <BananaSDK/Android.h>

class MyApp : public AndroidApp {
public:
    void Main() override {

        addListener("resume", []() {
            _BANANA_LOGI("App resumed!");
        });

        addListener("pause", []() {
            _BANANA_LOGI("App paused!");
        });

        addListener("destroy", []() {
            _BANANA_LOGI("App destroyed!");
        });

    }
};

startAndroid(MyApp);
