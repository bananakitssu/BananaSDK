package com.bananasdk.app;

import android.app.NativeActivity;
import android.os.Bundle;
import android.util.Log;
import java.io.FileWriter;

public class MainActivity extends NativeActivity {
    private static final String TAG = "BananaSDK";

    static {
        try {
            // Write to file IMMEDIATELY to see if we even get here
            FileWriter fw = new FileWriter("/sdcard/bananasdk_start.txt");
            fw.write("MainActivity class loading...\n");
            fw.close();
        } catch (Exception e) {
            // Silently fail
        }

        try {
            System.loadLibrary("bananasdk");
            Log.i(TAG, "Native library loaded successfully");
            
            FileWriter fw = new FileWriter("/sdcard/bananasdk_start.txt", true);
            fw.write("Native library loaded successfully\n");
            fw.close();
        } catch (UnsatisfiedLinkError e) {
            Log.e(TAG, "Failed to load native library: " + e.getMessage(), e);
            try {
                FileWriter fw = new FileWriter("/sdcard/bananasdk_start.txt", true);
                fw.write("ERROR loading native library: " + e.getMessage() + "\n");
                fw.close();
            } catch (Exception ex) {}
            throw new RuntimeException("Unable to load native library", e);
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        try {
            super.onCreate(savedInstanceState);
            Log.i(TAG, "MainActivity created successfully");
            
            FileWriter fw = new FileWriter("/sdcard/bananasdk_start.txt", true);
            fw.write("onCreate() reached\n");
            fw.close();
        } catch (Exception e) {
            Log.e(TAG, "Error in onCreate: " + e.getMessage(), e);
            try {
                FileWriter fw = new FileWriter("/sdcard/bananasdk_start.txt", true);
                fw.write("ERROR in onCreate: " + e.getMessage() + "\n");
                fw.close();
            } catch (Exception ex) {}
            throw new RuntimeException(e);
        }
    }
}
