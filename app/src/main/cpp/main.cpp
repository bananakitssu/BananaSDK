#include <BananaSDK/Android.h>
#include <BananaSDK/Renderer.h>
#include <BananaSDK/UI.h>
#include <BananaSDK/Button.h>

class MyApp : public AndroidAppDev {
public:
    Renderer renderer;
    UIRenderer ui;
    std::shared_ptr<Button> myButton;
    float buttonRadius = 20.0f;

    void Main() override {

        addListener("windowready", [this]() {
            renderer.Init(getWindow());
            renderer.SetClearColor(0.96f, 0.77f, 0.09f);

            int number = 20.0f;
            int w = ANativeWindow_getWidth(getWindow());
            int h = ANativeWindow_getHeight(getWindow());
            ui.Init(getActivity(), this, w, h, &renderer);

            myButton = std::make_shared<Button>(20, 60, 250, 70, "Click");
            //myButton.SetColor(1.0f, 0.0f, 0.0f);
            //myButton.SetTextColor(1.0f, 1.0f, 1.0f);
            //myButton.SetRadius(20.0f);
            auto myInput = std::make_shared<InputField>(
                myButton->GetX(),
                myButton->GetY() + myButton->GetH() + 12.0f,
                myButton->GetW(),
                myButton->GetH()
            );
            myInput->SetActivity(getActivity());
            myInput->SetPlaceholder("Type something...");
            myInput->SetOnSubmit([](const std::string& text) {
                // do absolutely nothing
            });
            auto myTextarea = std::make_shared<Textarea>(
                myButton->GetX() + myButton->GetW() + 12.0f,
                myButton->GetY(),
                200.0f,
                myButton->GetH() + myInput->GetH() + 12.0f
            );
            myTextarea->SetActivity(getActivity());
            myTextarea->SetPlaceholder("Type here...");
            addElement(myTextarea);
            addElement(myInput);
            addElement(myButton);
            myButton->SetOnClick([&]() {
                buttonRadius -= 1.0f;
                myButton->SetRadius(buttonRadius);
                _BANANA_LOGI("Button clicked!");
            });
        });

        addListener("touchstart", [this]() { DispatchTouch(GetTouchX(), GetTouchY()); });
addListener("touchmove",  [this]() { DispatchTouchMove(GetTouchX(), GetTouchY()); });
addListener("touchend",   [this]() { DispatchRelease(GetTouchX(), GetTouchY()); });
addListener("keydown",    [this]() { DispatchKey(GetLastKeyCode(), GetLastUnicode()); });

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
