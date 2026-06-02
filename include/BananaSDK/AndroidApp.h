#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
#include <android/log.h>
#include <android/native_window.h>
#include <android_native_app_glue.h>

#define _BANANA_TAG "BananaSDK"
#define _BANANA_LOGI(...) __android_log_print(ANDROID_LOG_INFO,  _BANANA_TAG, __VA_ARGS__)
#define _BANANA_LOGE(...) __android_log_print(ANDROID_LOG_ERROR, _BANANA_TAG, __VA_ARGS__)

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

    ANativeWindow* getWindow() const { return m_Window; }

    void _Init(android_app* state);
    void _Emit(const std::string& event) { m_Listeners.emit(event); }

    ANativeWindow* m_Window = nullptr;

private:
    _BananaInternal::ListenerMap m_Listeners;
    static void _HandleCmd(android_app* state, int32_t cmd);
};
