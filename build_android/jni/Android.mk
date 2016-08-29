LOCAL_PATH:= $(call my-dir)

FRAMEWORK_PATH := $(realpath $(LOCAL_PATH)/../../../Framework/)

include $(CLEAR_VARS)

LOCAL_MODULE       := libNuanceur
LOCAL_SRC_FILES    := ../../src/builder/Operations.cpp \
                      ../../src/builder/ShaderBuilder.cpp \
                      ../../src/generators/GlslShaderGenerator.cpp \
                      ../../src/generators/SpirvShaderGenerator.cpp
LOCAL_C_INCLUDES   := $(FRAMEWORK_PATH)/include $(LOCAL_PATH)/../../include
LOCAL_CPP_FEATURES := exceptions rtti

include $(BUILD_STATIC_LIBRARY)
