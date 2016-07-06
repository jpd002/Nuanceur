@echo off
CALL %ANDROID_NDK%/ndk-build clean NDK_DEBUG=0
CALL %ANDROID_NDK%/ndk-build clean NDK_DEBUG=1
