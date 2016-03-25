PROJECT(hello)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

AUX_SOURCE_DIRECTORY(. DIR_SRCS)

INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR} /home/chengyake/AMD/AMDAPPSDK-3.0/include/ )

LINK_DIRECTORIES( /home/chengyake/AMD/AMDAPPSDK-3.0/lib/x86_64/ )

#gcc main.c cmd.c matrix.c core.c debug.c core_kernel.cl  -I/home/chengyake/AMD/AMDAPPSDK-3.0/include/       -L/home/chengyake/AMD/AMDAPPSDK-3.0/lib/x86_64/ -lOpenCL  \
#                                                         -I/home/chengyake/project/none/plans/planB/utils/  -L/home/chengyake/project/none/plans/planB/utils -lnone_utils

# g++ HelloWorld.cpp -I /home/chengyake/AMD/AMDAPPSDK-3.0/include/   -L/home/chengyake/AMD/AMDAPPSDK-3.0/lib/x86_64/ -lOpenCL



ADD_EXECUTABLE( hello ${DIR_SRCS} )

TARGET_LINK_LIBRARIES(hello libOpenCL.so.1 libpthread.so.1)


