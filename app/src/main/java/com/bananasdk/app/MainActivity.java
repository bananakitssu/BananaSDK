package com.bananasdk.app;

import android.app.NativeActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.WindowInsets;
import android.view.WindowInsetsController;
import android.view.inputmethod.BaseInputConnection;
import android.view.inputmethod.InputConnection;
import android.text.Editable;
import android.text.TextWatcher;
import android.text.InputType;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputMethodManager;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.graphics.Color;

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

    private EditText hiddenInput;
private boolean ignoreTextChange = false;

public native void nativeOnTextChanged(String text);
public native void nativeOnSubmit(String text);
private void setupHiddenInput() {
    hiddenInput = new EditText(this);
    hiddenInput.setBackgroundColor(Color.TRANSPARENT);
    hiddenInput.setTextColor(Color.TRANSPARENT);
    hiddenInput.setCursorVisible(false);

    FrameLayout.LayoutParams params = new FrameLayout.LayoutParams(1, 1);
    params.leftMargin = -100;
    params.topMargin  = -100;
    addContentView(hiddenInput, params);

    hiddenInput.addTextChangedListener(new TextWatcher() {
        @Override public void beforeTextChanged(CharSequence s, int a, int b, int c) {}
        @Override public void onTextChanged(CharSequence s, int a, int b, int c) {
            if (ignoreTextChange) return;
            nativeOnTextChanged(s.toString());
        }
        @Override public void afterTextChanged(Editable s) {}
    });

    hiddenInput.setOnEditorActionListener((v, actionId, event) -> {
        if (actionId == EditorInfo.IME_ACTION_DONE
            || actionId == EditorInfo.IME_ACTION_SEND
            || actionId == EditorInfo.IME_ACTION_GO) {
            nativeOnSubmit(hiddenInput.getText().toString());
            return true;
        }
        return false;
    });
}

public void showEditInput(final String currentText, final boolean multiline) {
    runOnUiThread(() -> {
        ignoreTextChange = true;
        hiddenInput.setInputType(multiline
            ? InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_FLAG_MULTI_LINE
            : InputType.TYPE_CLASS_TEXT);
        hiddenInput.setImeOptions(multiline ? EditorInfo.IME_ACTION_NONE : EditorInfo.IME_ACTION_DONE);
        hiddenInput.setText(currentText);
        hiddenInput.setSelection(currentText.length());
        ignoreTextChange = false;

        hiddenInput.requestFocus();
        InputMethodManager imm = (InputMethodManager) getSystemService(INPUT_METHOD_SERVICE);
        imm.showSoftInput(hiddenInput, InputMethodManager.SHOW_FORCED);
    });
}

public void hideEditInput() {
    runOnUiThread(() -> {
        InputMethodManager imm = (InputMethodManager) getSystemService(INPUT_METHOD_SERVICE);
        imm.hideSoftInputFromWindow(hiddenInput.getWindowToken(), 0);
        hiddenInput.clearFocus();
    });
}

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getWindow().setStatusBarColor(0xFF000000);
        setupHiddenInput();
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
