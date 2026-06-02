package com.bananasdk.app;

import android.app.NativeActivity;
import android.os.Bundle;

public class MainActivity extends NativeActivity {
    static {
        System.loadLibrary("bananasdk");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }
}
