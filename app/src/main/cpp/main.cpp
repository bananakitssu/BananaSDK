#include <BananaSDK/Android.h>
#include <BananaSDK/Renderer.h>
#include <cstdio>

class MyApp : public AndroidApp {
public:
    Renderer renderer;

    void Main() override {
        FILE* f = fopen("/data/local/tmp/bananasdk_log.txt", "a");
        if (f) {
            fprintf(f, "[MyApp::Main] Starting Main()\n");
            fflush(f);
        }

        try {
            addListener("windowready", [this, f]() {
                if (f) {
                    FILE* flog = fopen("/data/local/tmp/bananasdk_log.txt", "a");
                    if (flog) {
                        fprintf(flog, "[windowready] Listener called\n");
                        fflush(flog);
                        fclose(flog);
                    }
                }
                
                if (f) {
                    FILE* flog = fopen("/data/local/tmp/bananasdk_log.txt", "a");
                    if (flog) {
                        fprintf(flog, "[windowready] About to call renderer.Init()\n");
                        fflush(flog);
                        fclose(flog);
                    }
                }
                
                renderer.Init(getWindow());
                
                if (f) {
                    FILE* flog = fopen("/data/local/tmp/bananasdk_log.txt", "a");
                    if (flog) {
                        fprintf(flog, "[windowready] renderer.Init() completed\n");
                        fflush(flog);
                        fclose(flog);
                    }
                }
                
                // Yellow! Like a banana 🍌
                renderer.SetClearColor(0.96f, 0.77f, 0.09f);
                
                if (f) {
                    FILE* flog = fopen("/data/local/tmp/bananasdk_log.txt", "a");
                    if (flog) {
                        fprintf(flog, "[windowready] SetClearColor completed\n");
                        fflush(flog);
                        fclose(flog);
                    }
                }
            });

            if (f) {
                fprintf(f, "[MyApp::Main] Added windowready listener\n");
                fflush(f);
            }

            addListener("frame", [this, f]() {
                renderer.DrawFrame();
            });

            if (f) {
                fprintf(f, "[MyApp::Main] Added frame listener\n");
                fflush(f);
            }

            addListener("windowlost", [this, f]() {
                renderer.Destroy();
            });

            if (f) {
                fprintf(f, "[MyApp::Main] Added windowlost listener\n");
                fflush(f);
            }

            addListener("destroy", [this, f]() {
                renderer.Destroy();
            });

            if (f) {
                fprintf(f, "[MyApp::Main] Main() completed successfully\n");
                fflush(f);
            }
        } catch (const std::exception& e) {
            if (f) {
                fprintf(f, "[MyApp::Main] Exception: %s\n", e.what());
                fflush(f);
            }
            throw;
        } catch (...) {
            if (f) {
                fprintf(f, "[MyApp::Main] Unknown exception\n");
                fflush(f);
            }
            throw;
        }

        if (f) fclose(f);
    }
};

startAndroid(MyApp);
