package com.soccerlegacy;

import android.app.NativeActivity;

// We subclass NativeActivity purely as a passthrough.
// All real logic lives in C++ via the NDK.
public class MainActivity extends NativeActivity {
    // Intentionally empty — C++ handles everything
}
