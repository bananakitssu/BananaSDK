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
            auto myCircleProgress = std::make_shared<CircleProgress>(
                myTextarea->GetX() + myTextarea->GetW() + 12.0f,
                myTextarea->GetY(),
                60.0f, 60.0f
            );
            myCircleProgress->SetProgress(-1.0f);
            addElement(myCircleProgress);
            auto myProgress = std::make_shared<Progress>(
                myInput->GetX(),
                myInput->GetY() + myInput->GetH() + 12.0f,
                myInput->GetW(),
                36.0f
            );
            myProgress->SetProgress(-1.0f);
            auto myScrollBox = std::make_shared<ScrollBox>(
                myProgress->GetX(),
                myProgress->GetY() + myProgress->GetH() + 12.0f,
                250.0f, 200.0f
            );
            myScrollBox->SetBgColor(0.92f, 0.92f, 0.92f, 1.0f);

            float itemY = 0.0f;
            for (int i = 0; i < 10; i++) {
                auto label = std::make_shared<Text>(0.0f, itemY + 12.0f, "Item " + std::to_string(i+1), 0.0f, 28.0f);
                myScrollBox->AddElement(label);
                itemY += 50.0f;
            }
            myScrollBox->SetContentHeight(itemY);

            auto myCheckBox = std::make_shared<CheckBox>(myScrollBox->GetX() + myScrollBox->GetW() + 12.0f, myScrollBox->GetY(), 32.0f);
            myCheckBox->SetLabel("Uhhh");
            addElement(myCheckBox);

            auto myRadioGroup = std::make_shared<RadioGroup>(
                myCheckBox->GetX(),
                myCheckBox->GetY() + myCheckBox->GetH() + 12.0f,
                180.0f, 150.0f
            );
            myRadioGroup->AddOption("Option A");
            myRadioGroup->AddOption("Option B");
            myRadioGroup->AddOption("Option C");
            myRadioGroup->SetSelected(0);
            addRadioGroup(myRadioGroup);
            addScrollBox(myScrollBox);
            auto myDropdown = std::make_shared<Dropdown>(
                myRadioGroup->GetX() + myRadioGroup->GetW() + 12.0f,
                myRadioGroup->GetY(),
                180.0f, 48.0f
            );
            myDropdown->AddOption("Red");
            myDropdown->AddOption("Green");
            myDropdown->AddOption("Blue");
            myDropdown->AddOption("Yellow");
            myDropdown->AddOption("Purple");
            myDropdown->SetSelected(0);
            auto myMultiDropdown = std::make_shared<MultiDropdown>(
                myDropdown->GetX(),
                myDropdown->GetY() + myDropdown->GetH() + 12.0f,
                180.0f, 48.0f
            );
            myMultiDropdown->AddOption("Apples");
            myMultiDropdown->AddOption("Bananas");
            myMultiDropdown->AddOption("Cherries");
            myMultiDropdown->AddOption("Dates");
            myMultiDropdown->SetPlaceholder("Pick fruits");
            addElement(myMultiDropdown);
            addElement(myDropdown);
            addElement(myProgress);
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
