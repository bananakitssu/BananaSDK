cmake_minimum_required(VERSION 3.22)
project(bananasdk)

add_library(native_app_glue STATIC
    ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
)

add_library(bananasdk SHARED
    include/src/AndroidApp.cpp
    include/src/Renderer.cpp
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
