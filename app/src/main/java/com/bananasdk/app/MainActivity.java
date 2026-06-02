package com.bananasdk.app;

import android.app.NativeActivity;
import android.os.Bundle;
import android.util.Log;
import java.io.FileWriter;

public class MainActivity extends NativeActivity {
    private static final String TAG = "BananaSDK";

    static {
        try {
            // Try multiple log paths - including /documents for Android 14+
            String[] logPaths = {
                "/sdcard/bananasdk_start.txt",
                "/storage/emulated/0/bananasdk_start.txt",
                "/storage/emulated/0/Documents/bananasdk_start.txt",
                "/data/data/com.bananasdk.app/bananasdk_start.txt",
                "/data/local/tmp/bananasdk_start.txt",
                "/storage/Documents/bananasdk_start.txt",
                "/documents/bananasdk_start.txt"
            };
            
            String logFile = null;
            for (String path : logPaths) {
                try {
                    FileWriter fw = new FileWriter(path);
                    fw.write("MainActivity class loading...\n");
                    fw.close();
                    logFile = path;
                    Log.i(TAG, "Using log file: " + path);
                    break;
                } catch (Exception e) {
                    // Try next path
                }
            }

            System.loadLibrary("bananasdk");
            Log.i(TAG, "Native library loaded successfully");
            
            if (logFile != null) {
                try {
                    FileWriter fw = new FileWriter(logFile, true);
                    fw.write("Native library loaded successfully\n");
                    fw.close();
                } catch (Exception ex) {}
            }
        } catch (UnsatisfiedLinkError e) {
            Log.e(TAG, "Failed to load native library: " + e.getMessage(), e);
            try {
                FileWriter fw = new FileWriter("/data/local/tmp/bananasdk_start.txt", true);
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
            
            try {
                FileWriter fw = new FileWriter("/data/local/tmp/bananasdk_start.txt", true);
                fw.write("onCreate() reached\n");
                fw.close();
            } catch (Exception ex) {}
        } catch (Exception e) {
            Log.e(TAG, "Error in onCreate: " + e.getMessage(), e);
            try {
                FileWriter fw = new FileWriter("/data/local/tmp/bananasdk_start.txt", true);
                fw.write("ERROR in onCreate: " + e.getMessage() + "\n");
                fw.close();
            } catch (Exception ex) {}
            throw new RuntimeException(e);
        }
    }
}
