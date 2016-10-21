LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)


LOCAL_C_INCLUDES := \
        $(LOCAL_PATH)/include


LOCAL_SHARED_LIBRARIES := liblog libcutils

LOCAL_SRC_FILES := \
        opencl_mu1.c


LOCAL_CFLAGS :=  -O3 -mfloat-abi=softfp -mfpu=neon-vfpv4
#LOCAL_CFLAGS :=   -mfloat-abi=softfp -mfpu=neon-vfpv4 -S

LOCAL_ARM_NEON := true
#TARGET_ARCH_ABI := armeabi-v7a
LOCAL_ARM_MODE := arm

LOCAL_MODULE := libOpencl_MU1
LOCAL_MODULE_TAGS := optional



#include $(BUILD_STATIC_LIBRARY)
include $(BUILD_EXECUTABLE)









#include $(BUILD_SHARED_LIBRARY)


#gcc main.c cmd.c matrix.c core.c debug.c core_kernel.cl  -I/home/chengyake/AMD/AMDAPPSDK-3.0/include/       -L/home/chengyake/AMD/AMDAPPSDK-3.0/lib/x86_64/ -lOpenCL  \
#                                                         -I/home/chengyake/project/none/plans/planB/utils/  -L/home/chengyake/project/none/plans/planB/utils -lnone_utils

# g++ HelloWorld.cpp -I /home/chengyake/AMD/AMDAPPSDK-3.0/include/   -L/home/chengyake/AMD/AMDAPPSDK-3.0/lib/x86_64/ -lOpenCL
