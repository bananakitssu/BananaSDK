package com.bananasdk.app;

import android.app.NativeActivity;
import android.os.Bundle;
import android.util.Log;
import java.io.FileWriter;
import android.view.WindowInsets;
import android.view.WindowInsetsController;
import android.text.InputType;
import android.view.inputmethod.BaseInputConnection;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputConnection;

public class MainActivity extends NativeActivity {
    private static final String TAG = "BananaSDK";

    static {
        try {
            System.loadLibrary("bananasdk");
            Log.i(TAG, "Native library loaded successfully");
        } catch (UnsatisfiedLinkError e) {
            Log.e(TAG, "Failed to load native library: " + e.getMessage(), e);
            throw new RuntimeException("Unable to load native library", e);
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getWindow().setStatusBarColor(0xFF000000);
    }

    public native void nativeOnTextCommit(String text);

    @Override
    public InputConnection onCreateInputConnection(EditorInfo outAttrs) {
        outAttrs.inputType  = InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_FLAG_MULTI_LINE;
        outAttrs.imeOptions = EditorInfo.IME_FLAG_NO_EXTRACT_UI;

        return new BaseInputConnection(getWindow().getDecorView(), false) {
            @Override
            public boolean commitText(CharSequence text, int newCursorPosition) {
                nativeOnTextCommit(text.toString());
                return true;
            }
        };
    }
}
