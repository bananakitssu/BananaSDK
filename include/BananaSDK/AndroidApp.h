#pragma once

#include <functional>
#include <string>
#include <any>
#include <unordered_map>
#include <vector>
#include <variant>
#include "Log.h"
#include <android/native_window.h>
#include <android_native_app_glue.h>
#include <android/input.h>
#include "BananaSDK/UIElements.h"

#define startAndroid(AppClass)                            \
    extern "C" {                                          \
        void android_main(android_app* _state) {          \
            AppClass _app;                                \
            _app._Init(_state);                           \
        }                                                 \
    }

namespace _BananaInternal {

    using Callback = std::function<void()>;

    struct ListenerEntry {
        int      id;
        Callback fn;
    };

    class ListenerMap {
    public:
        int add(const std::string& event, Callback fn) {
            int id = m_NextId++;
            m_Map[event].push_back({ id, fn });
            return id;
        }

        void remove(const std::string& event, int id = -1) {
            auto it = m_Map.find(event);
            if (it == m_Map.end()) return;
            if (id == -1) {
                it->second.clear();
                return;
            }
            auto& vec = it->second;
            vec.erase(
                std::remove_if(vec.begin(), vec.end(),
                    [id](const ListenerEntry& e) { return e.id == id; }),
                vec.end()
            );
        }

        void emit(const std::string& event) {
            auto it = m_Map.find(event);
            if (it == m_Map.end()) return;
            for (auto& entry : it->second)
                entry.fn();
        }

    private:
        int m_NextId = 0;
        std::unordered_map<std::string, std::vector<ListenerEntry>> m_Map;
    };

} // namespace _BananaInternal


class AndroidApp {
public:
    AndroidApp()  = default;
    virtual ~AndroidApp() = default;

    virtual void Main() {}

    int addListener(const std::string& event, _BananaInternal::Callback fn) {
        return m_Listeners.add(event, fn);
    }

    void removeListener(const std::string& event, int id) {
        m_Listeners.remove(event, id);
    }

    void removeListener(const std::string& event) {
        m_Listeners.remove(event, -1);
    }

    void addElement(const UIEl::UIElement& element) {
        m_Elements.push_back(element);
    }

    const std::vector<UIEl::UIElement>& getElements() const { 
        return m_Elements; 
    }

    float GetTouchX() const { return m_TouchX; }
    float GetTouchY() const { return m_TouchY; }

    float m_TouchX = 0, m_TouchY = 0;
    std::vector<UIEl::UIElement> m_Elements;

    ANativeWindow* getWindow() const { return m_Window; }
    ANativeActivity* GetActivity() const { return m_State->activity; }
    ANativeActivity* getActivity() const { return m_State->activity; }

    virtual void _Init(android_app* state);
    void _Emit(const std::string& event) { m_Listeners.emit(event); }

    ANativeWindow* m_Window = nullptr;

private:
    _BananaInternal::ListenerMap m_Listeners;
    static void _HandleCmd(android_app* state, int32_t cmd);
    static int32_t _HandleInput(android_app*, AInputEvent*);
    android_app* m_State = nullptr;
};
