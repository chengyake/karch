#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <opencl.h>
#include <linux/types.h>
#include <fcntl.h>
#include <utils/Log.h>

#define MMAP

//#define DEBUG_API_CL

int table_i[512][1024], table_q[512][1024];
int table_o[512][1024];

size_t global_work_size[2] = {512, 1024};
size_t local_work_size[2] = {16, 16};

#ifdef DEBUG_API_CL
cl_event event = 0;
cl_ulong start_time=0;
cl_ulong end_time=0;
#endif


int ret;

static cl_context context;
static cl_command_queue commandQueue;
static cl_program program;
static cl_kernel kernel;
static cl_mem inputBuffer_i;
static cl_mem inputBuffer_q;
static cl_mem inputBuffer_o;

static int read_cl(char *filename, char *text) {

    int fd, len, size;
    fd = open(filename, O_RDONLY);
    if(fd < 0) {
        LOGD("MU1 Error: open kernel CL file error\n");
        return fd;
    }
    len = lseek(fd, 0, SEEK_END);
    if(len < 0) {
        LOGD("MU1 Error: get file length error\n");
        close(fd);
        return len;
    }
    lseek(fd, 0, SEEK_SET);
    size = read(fd, text, len);
    if(len != size) {
        LOGD("MU1 Error: file len %d   %d\n", len, size);
        return -1;
    }
    text[size]='\0';

    LOGD("MU1: read cl success");

    return 0;
}



int32_t init_kernel_platform() {

	cl_uint plat_num;
	cl_platform_id plat_id = NULL;
	cl_uint dev_num = 0;
	cl_device_id *devices;

	ret = clGetPlatformIDs(0, NULL, &plat_num);
	if (ret < 0) {
		LOGD("MU1 Error: Getting plat_ids!\n");
		return -1;
	}

	if(plat_num > 0)
	{
		cl_platform_id* plat_ids = (cl_platform_id* )malloc(plat_num* sizeof(cl_platform_id));
		ret = clGetPlatformIDs(plat_num, plat_ids, NULL);
		plat_id = plat_ids[0];
		free(plat_ids);
	}

	ret = clGetDeviceIDs(plat_id, CL_DEVICE_TYPE_GPU, 0, NULL, &dev_num);	
	if (dev_num == 0) {
		LOGD("MU1: No GPU device available.\n");
		LOGD("MU1: Choose CPU as default device.\n");
		ret = clGetDeviceIDs(plat_id, CL_DEVICE_TYPE_CPU, 0, NULL, &dev_num);	
		devices = (cl_device_id*)malloc(dev_num * sizeof(cl_device_id));
		ret = clGetDeviceIDs(plat_id, CL_DEVICE_TYPE_CPU, dev_num, devices, NULL);
	} else {
		LOGD("MU1: Choose GPU as default device. dev_num %d\n", dev_num);
		devices = (cl_device_id*)malloc(dev_num * sizeof(cl_device_id));
		ret = clGetDeviceIDs(plat_id, CL_DEVICE_TYPE_GPU, dev_num, devices, NULL);
	}
	
	context = clCreateContext(NULL,1, devices,NULL,NULL,NULL);
#ifdef DEBUG_API_CL
	commandQueue = clCreateCommandQueue(context, devices[0], CL_QUEUE_PROFILING_ENABLE, &ret);
#else
	commandQueue = clCreateCommandQueue(context, devices[0], 0, NULL);
#endif
    
	char filename[] = "/data/mu1_kernel.cl";
	char file_context[10*1024]={0};
	const char *source = &file_context[0];

    ret = read_cl(filename, &file_context[0]);

	size_t sourceSize[10] = {strlen(source)};
	cl_program program = clCreateProgramWithSource(context, 1, &source, &sourceSize[0], NULL);
	
	ret = clBuildProgram(program, 1, devices, NULL, NULL, NULL);
    if(ret < 0) {
        LOGD("MU1 Error: clBuildProgram error\n");
        return 0;
    }

	kernel = clCreateKernel(program, "process_iq", NULL);

	inputBuffer_i = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 
            512*1024*4, (void *)(&table_i[0]), NULL);
	inputBuffer_q = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 
            512*1024*4, (void *)(&table_q[0]), NULL);
	inputBuffer_o = clCreateBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, 
            512*1024*4, (void *)(&table_o[0]), NULL);


	ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&inputBuffer_i);
	ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&inputBuffer_q);
	ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&inputBuffer_o);



    if(devices != NULL) { free(devices);}

    LOGD("MU1: init cl plat success");
    return 0;
}

#ifdef MMAP
static int32_t set_input_i_to_kernel() {
    
    char* p = (char*) clEnqueueMapBuffer(commandQueue, inputBuffer_i, CL_TRUE, CL_MAP_WRITE, 0, 
            512*1024*4, 0, NULL, NULL, &ret);
    if(ret < 0) {
        LOGD("MU1 Error: clEnqueueMapBuffer failed. when set input\n");
        return ret;
    }

    memcpy(p, (char *)(table_i), 512*1024*4);

    ret = clEnqueueUnmapMemObject(commandQueue, inputBuffer_i, p, 0, NULL, NULL);
	clFlush(commandQueue);
	return 0;
}
#else 

static int32_t set_input_i_to_kernel() {

    ret = clEnqueueWriteBuffer(commandQueue, inputBuffer_i, CL_TRUE, 0, 512*1024*4, table_i, 0, NULL, NULL);

    return 0;
}

#endif



#ifdef MMAP
static int32_t set_input_o_to_kernel() {
    
    char* p = (char*) clEnqueueMapBuffer(commandQueue, inputBuffer_q, CL_TRUE, CL_MAP_WRITE, 0,
            512*1024*4, 0, NULL, NULL, &ret);
    if(ret < 0) {
        LOGD("MU1 Error: clEnqueueMapBuffer failed. when set input\n");
        return ret;
    }

    memcpy(p, (char *)(table_q), 512*1024*4);

    ret = clEnqueueUnmapMemObject(commandQueue, inputBuffer_q, p, 0, NULL, NULL);
	clFlush(commandQueue);
    return 0;
}
#else 

static int32_t set_input_i_to_kernel() {

    ret = clEnqueueWriteBuffer(commandQueue, inputBuffer_o, CL_TRUE, 0, 512*1024*4, table_o, 0, NULL, NULL);

    return 0;
}
#endif


static int32_t run_kernel() {
#ifdef DEBUG_API_CL
    size_t info;
    ret = clEnqueueNDRangeKernel(commandQueue, kernel, 2, NULL, global_work_size, local_work_size, 0, NULL, &event);
#else
    ret = clEnqueueNDRangeKernel(commandQueue, kernel, 2, NULL, global_work_size, local_work_size, 0, NULL, NULL);
    //ret = clEnqueueNDRangeKernel(commandQueue, kernel, 2, NULL, global_work_size, NULL, 0, NULL, NULL);
#endif
    if(ret < 0) {
        LOGD("MU1 Error: clEnqueueNDRangeKernel error\n");
    }

    clFinish(commandQueue);

#ifdef DEBUG_API_CL

    clWaitForEvents(1, &event);
    cl_int status=1;
    clGetEventInfo(event, CL_EVENT_COMMAND_EXECUTION_STATUS, 4, &status, NULL);
    clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start_time, &info);
    clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end_time, &info);
    
    LOGD("MU1: clGetEventProilingInfo run kernel %f ms, status: %d", (end_time - start_time)*1.0/1000000, status);
#endif
    return ret;

}

static int32_t get_output_from_kernel() {

    char* p = (char*) clEnqueueMapBuffer(commandQueue, inputBuffer_o, CL_TRUE, CL_MAP_READ, 0, 512*1024*4, 0, NULL, NULL, &ret);
    if(ret < 0) {
        LOGD("MU1 Error: clEnqueueMapBuffer failed. when get matrix\n");
        return ret;
    }
    memcpy((char *)table_o, p, 512*1024*4);

    ret = clEnqueueUnmapMemObject(commandQueue, inputBuffer_o, p, 0, NULL, NULL);
	clFlush(commandQueue);

    return 0;
}


int32_t close_kernel_platform() {

	ret = clReleaseKernel(kernel);				
	ret = clReleaseProgram(program);	
	ret = clReleaseMemObject(inputBuffer_i);
	ret = clReleaseMemObject(inputBuffer_q);
	ret = clReleaseMemObject(inputBuffer_o);
	ret = clReleaseCommandQueue(commandQueue);
	ret = clReleaseContext(context);

    LOGD("release kernel");

    return 0;

}


int init_mu1_opencl() {

    int i,j;

    ret = init_kernel_platform();
    if(ret < 0) {
        LOGD("init opencl mu1 error");
        return ret;
    }
    
    for(i=0; i<512; i++) {
        for(j=0; j<1024; j++) {
            table_i[i][j] = 20;
            table_q[i][j] = 10;
            table_o[i][j] = 1;
        }
    }

    return 0;
}

void process_cfm_by_gpu(unsigned char *pDataDst, int nDstWidth, int nDstHeight, short *pSrcData, int nSrcWidth, int nSrcHeight) {

    int i,j;

#if 1

    LOGD("MU1 ---------------------- input start");
    set_input_i_to_kernel();
    set_input_o_to_kernel();
    LOGD("MU1 ---------------------- run kernel");
    run_kernel();
    LOGD("MU1 ---------------------- get output");
    get_output_from_kernel();
    LOGD("MU1 ---------------------- end");

#else

    LOGD("MU1 ---------------------- start C");
    for(i=0; i<512; i++) {
        for(j=0; j<1024; j++) {
            table_o[i][j] = (table_i[i][j] + table_q[i][j]) / (table_i[i][j] - table_q[i][j]);
            table_o[i][j] += (table_i[i][j] + table_q[i][j] - 1) / (table_i[i][j] - table_q[i][j] + 1);
        }
    }
    LOGD("MU1 ---------------------- end");

#endif

}

int close_mu1_opencl() {

    close_kernel_platform();
    
    return 0;
}


int main() {

    init_mu1_opencl();
    
    process_cfm_by_gpu(NULL, 0, 0, NULL, 0, 0);

    
    LOGD("MU1: table_o: %d %d %d\n", table_o[0][0], table_o[17][17], table_o[511][1023]);

    close_mu1_opencl();

    return 0;

}


