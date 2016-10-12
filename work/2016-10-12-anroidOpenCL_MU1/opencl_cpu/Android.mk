LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)


LOCAL_SHARED_LIBRARIES := libGLES_mali

LOCAL_C_INCLUDES := \
        $(LOCAL_PATH)/include

#LOCAL_CFLAGS := $(LOCAL_C_INCLUDES:%=-I%)
#LOCAL_LDLIBS := -L$(TARGET_OUT)/system/vendor/lib/egl

LOCAL_SHARED_LIBRARIES := liblog libcutils

LOCAL_SRC_FILES := \
        opencl_mu1.c


LOCAL_MODULE := libOpenCL_MU1
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)
#include $(BUILD_EXECUTABLE)










#include $(BUILD_SHARED_LIBRARY)


#gcc main.c cmd.c matrix.c core.c debug.c core_kernel.cl  -I/home/chengyake/AMD/AMDAPPSDK-3.0/include/       -L/home/chengyake/AMD/AMDAPPSDK-3.0/lib/x86_64/ -lOpenCL  \
#                                                         -I/home/chengyake/project/none/plans/planB/utils/  -L/home/chengyake/project/none/plans/planB/utils -lnone_utils

# g++ HelloWorld.cpp -I /home/chengyake/AMD/AMDAPPSDK-3.0/include/   -L/home/chengyake/AMD/AMDAPPSDK-3.0/lib/x86_64/ -lOpenCL
