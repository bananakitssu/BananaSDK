package com.bananasdk.app;

import android.app.NativeActivity;
import android.os.Bundle;
import android.util.Log;

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
        try {
            super.onCreate(savedInstanceState);
            Log.i(TAG, "MainActivity created successfully");
        } catch (Exception e) {
            Log.e(TAG, "Error in onCreate: " + e.getMessage(), e);
            throw e;
        }
    }
}
