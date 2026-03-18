#include <android_native_app_glue.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <android/log.h>

#define LOG_TAG "SoccerLegacy"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// Holds our EGL state
struct Engine {
    EGLDisplay display = EGL_NO_DISPLAY;
    EGLSurface surface = EGL_NO_SURFACE;
    EGLContext context = EGL_NO_CONTEXT;
    bool running = false;
};

// Initialize EGL — creates the OpenGL ES surface
static bool initDisplay(Engine* engine, ANativeWindow* window) {
    engine->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(engine->display, nullptr, nullptr);

    const EGLint attribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,  // Request OpenGL ES 3.0
        EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
        EGL_BLUE_SIZE,       8,
        EGL_GREEN_SIZE,      8,
        EGL_RED_SIZE,        8,
        EGL_NONE
    };

    EGLConfig config;
    EGLint numConfigs;
    eglChooseConfig(engine->display, attribs, &config, 1, &numConfigs);

    const EGLint contextAttribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3,   // OpenGL ES 3.0
        EGL_NONE
    };

    engine->surface = eglCreateWindowSurface(engine->display, config, window, nullptr);
    engine->context = eglCreateContext(engine->display, config, nullptr, contextAttribs);

    if (eglMakeCurrent(engine->display,
                       engine->surface,
                       engine->surface,
                       engine->context) == EGL_FALSE) {
        LOGE("eglMakeCurrent failed!");
        return false;
    }

    engine->running = true;
    LOGI("OpenGL ES 3.0 initialized successfully!");
    return true;
}

// Render one frame — solid green field color for now
static void renderFrame(Engine* engine) {
    if (engine->display == EGL_NO_DISPLAY) return;

    // Soccer field green  🟢
    glClearColor(0.13f, 0.55f, 0.13f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    eglSwapBuffers(engine->display, engine->surface);
}

// Tear down EGL cleanly
static void destroyDisplay(Engine* engine) {
    if (engine->display != EGL_NO_DISPLAY) {
        eglMakeCurrent(engine->display,
                       EGL_NO_SURFACE,
                       EGL_NO_SURFACE,
                       EGL_NO_CONTEXT);
        if (engine->context != EGL_NO_CONTEXT)
            eglDestroyContext(engine->display, engine->context);
        if (engine->surface != EGL_NO_SURFACE)
            eglDestroySurface(engine->display, engine->surface);
        eglTerminate(engine->display);
    }
    engine->display = EGL_NO_DISPLAY;
    engine->surface = EGL_NO_SURFACE;
    engine->context = EGL_NO_CONTEXT;
    engine->running = false;
}

// Android event handler — window, focus, input events
static void handleAppCmd(android_app* app, int32_t cmd) {
    Engine* engine = (Engine*)app->userData;
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            if (app->window != nullptr) {
                initDisplay(engine, app->window);
                renderFrame(engine);
            }
            break;
        case APP_CMD_TERM_WINDOW:
            destroyDisplay(engine);
            break;
        case APP_CMD_GAINED_FOCUS:
            engine->running = true;
            break;
        case APP_CMD_LOST_FOCUS:
            engine->running = false;
            renderFrame(engine);   // Render one dark frame on pause
            break;
        default:
            break;
    }
}

// ─── Entry Point ───────────────────────────────────────────────
void android_main(android_app* app) {
    Engine engine;
    app->userData = &engine;
    app->onAppCmd = handleAppCmd;

    LOGI("SoccerLegacy starting...");

    // Main game loop
    while (true) {
        int events;
        android_poll_source* source;

        // Process all pending system events
        while (ALooper_pollAll(engine.running ? 0 : -1,
                               nullptr,
                               &events,
                               (void**)&source) >= 0) {
            if (source) source->process(app, source);
            if (app->destroyRequested) {
                destroyDisplay(&engine);
                return;
            }
        }

        // Render if active
        if (engine.running) {
            renderFrame(engine);
        }
    }
}
