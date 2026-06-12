#include <jni.h>
#include <string>
#include "BananaSDK/AndroidApp.h"

AndroidApp* g_AppInstance = nullptr;

extern "C"
JNIEXPORT void JNICALL
Java_com_bananasdk_app_MainActivity_nativeOnTextChanged(JNIEnv* env, jobject, jstring text) {
    const char* chars = env->GetStringUTFChars(text, nullptr);
    std::string str(chars);
    env->ReleaseStringUTFChars(text, chars);
    if (g_AppInstance) g_AppInstance->OnTextChanged(str);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_bananasdk_app_MainActivity_nativeOnSubmit(JNIEnv* env, jobject, jstring text) {
    const char* chars = env->GetStringUTFChars(text, nullptr);
    std::string str(chars);
    env->ReleaseStringUTFChars(text, chars);
    if (g_AppInstance) g_AppInstance->OnSubmit(str);
}
