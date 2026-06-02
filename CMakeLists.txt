cmake_minimum_required(VERSION 3.22)
project(bananasdk)

add_library(bananasdk SHARED
    include/src/AndroidApp.cpp
    app/src/main/cpp/main.cpp
)

target_include_directories(bananasdk PRIVATE
    include
)

target_link_libraries(bananasdk
    android
    log
    android_native_app_glue
)
