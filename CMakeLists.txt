cmake_minimum_required(VERSION 3.22)
project(bananasdk)

add_library(native_app_glue STATIC
    ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
)

add_library(bananasdk SHARED
    include/src/AndroidApp.cpp
    include/src/Renderer.cpp
    include/src/UI.cpp
    include/src/Button.cpp
    include/src/AndroidAppDev.cpp
    include/src/Box.cpp
    include/src/PopupMenu.cpp
    include/src/ContextMenu.cpp
    include/src/Text.cpp
    include/src/InputField.cpp
    include/src/Textarea.cpp
    include/src/JNIBridge.cpp
    include/src/Progress.cpp
    include/src/CircleProgress.cpp
    include/src/ScrollBox.cpp
    include/src/RadioGroup.cpp
    include/src/CheckBox.cpp
    app/src/main/cpp/main.cpp
)

target_include_directories(bananasdk PRIVATE
    include
    ${ANDROID_NDK}/sources/android/native_app_glue
)

target_link_options(bananasdk PRIVATE -u ANativeActivity_onCreate)

target_link_libraries(bananasdk
    native_app_glue
    android
    log
    EGL
    GLESv2
)
