#pragma once

#include <android/log.h>

#define _BANANA_TAG  "BananaSDK"
#define _BANANA_LOGI(...) __android_log_print(ANDROID_LOG_INFO,  _BANANA_TAG, __VA_ARGS__)
#define _BANANA_LOGE(...) __android_log_print(ANDROID_LOG_ERROR, _BANANA_TAG, __VA_ARGS__)
