cmake_minimum_required(VERSION 3.22.1)
project("soccerlegacy")

# Find the android_native_app_glue source
# It ships with the NDK itself
get_filename_component(
    NDK_MODULE_PATH
    "${ANDROID_NDK}/sources/android/native_app_glue"
    ABSOLUTE
)

add_library(
    soccerlegacy        # Must match android:value="soccerlegacy" in Manifest
    SHARED              # Android loads .so shared libraries
    src/main/cpp/main.cpp
    ${NDK_MODULE_PATH}/android_native_app_glue.c
)

target_include_directories(
    soccerlegacy PRIVATE
    ${NDK_MODULE_PATH}
)

# Link required Android & OpenGL ES libraries
target_link_libraries(
    soccerlegacy
    android         # Core Android NDK
    log             # __android_log_print (logcat)
    EGL             # OpenGL ES surface management
    GLESv3          # OpenGL ES 3.0
)
