LOCAL_PATH:= $(call my-dir)

include $(LOCAL_PATH)/ExternalDependencies.mk
FRAMEWORK_PATH := $(realpath $(LOCAL_PATH)/../../../Framework/)

include $(CLEAR_VARS)

LOCAL_MODULE       := libNuanceur
LOCAL_SRC_FILES    := ../../src/Operations.cpp \
                      ../../src/GlslShaderGenerator.cpp \
                      ../../src/ShaderBuilder.cpp \
                      ../../src/SpirvShaderGenerator.cpp
LOCAL_C_INCLUDES   := $(VULKAN_PATH) $(FRAMEWORK_PATH)/include $(LOCAL_PATH)/../../include
LOCAL_CPP_FEATURES := exceptions rtti

include $(BUILD_STATIC_LIBRARY)
