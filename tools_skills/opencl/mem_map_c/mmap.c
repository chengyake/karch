#include <stdio.h>
#include <stdlib.h>
#include <alloca.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
//#define CL_USE_DEPRECATED_OPENCL_2_0_APIS

#include <CL/cl.h>

#define SUCCESS 0
#define FAILURE 1


int read_cl(char *filename, char *text) {

    int fd, len, size;


    fd = open(filename, O_RDONLY);
    if(fd < 0) {
        printf("open upload file error\n");
        return fd;
    }

    len = lseek(fd, 0, SEEK_END);
    if(len < 0) {
        printf("create TDataBase.db fixed space error\n");
        close(fd);
        return len;
    }

    lseek(fd, 0, SEEK_SET);

    size = read(fd, text, len);
    if(len != size) {
        printf("file len %d   %d\n", len, size);
        return -1;
    }

    text[size]='\0';

    return 0;
}

int main(int argc, char* argv[]) {
    
    cl_int i;
	cl_uint plat_num;
	cl_platform_id plat_id = NULL;
	cl_int	ret = clGetPlatformIDs(0, NULL, &plat_num);
	if (ret != CL_SUCCESS) {
		printf("Error: Getting plat_ids!\n");
		return FAILURE;
	}

	if(plat_num > 0)
	{
		cl_platform_id* plat_ids = (cl_platform_id* )malloc(plat_num* sizeof(cl_platform_id));
		ret = clGetPlatformIDs(plat_num, plat_ids, NULL);
		plat_id = plat_ids[0];
		free(plat_ids);
	}

	cl_uint				dev_num = 0;
	cl_device_id        *devices;
	ret = clGetDeviceIDs(plat_id, CL_DEVICE_TYPE_GPU, 0, NULL, &dev_num);	
	if (dev_num == 0) {
		printf("No GPU device available.\n");
		printf("Choose CPU as default device.\n");
		ret = clGetDeviceIDs(plat_id, CL_DEVICE_TYPE_CPU, 0, NULL, &dev_num);	
		devices = (cl_device_id*)malloc(dev_num * sizeof(cl_device_id));
		ret = clGetDeviceIDs(plat_id, CL_DEVICE_TYPE_CPU, dev_num, devices, NULL);
	} else {
		devices = (cl_device_id*)malloc(dev_num * sizeof(cl_device_id));
		ret = clGetDeviceIDs(plat_id, CL_DEVICE_TYPE_GPU, dev_num, devices, NULL);
	}
	
	cl_context context = clCreateContext(NULL,1, devices,NULL,NULL,NULL);
	
	cl_command_queue commandQueue = clCreateCommandQueue(context, devices[0], 0, NULL);
    
	char filename[] = "mmap.cl";
	char file_context[4096]={0};
	const char *source = &file_context[0];

    ret = read_cl(filename, &file_context[0]);

	size_t sourceSize[10] = {strlen(source)};
	cl_program program = clCreateProgramWithSource(context, 1, &source, &sourceSize[0], NULL);
	
	ret = clBuildProgram(program, 1, devices, NULL, NULL, NULL);
    if(ret < 0) {
        printf("clBuildProgram error\n");
    }

    //input and output
	const char input[4] = {1,2,3,4};
	size_t strlength = 4;
    printf("input:\n%d, %d, %d, %d\n", input[0], input[1], input[2], input[3]);
	char *output = (char*) malloc(strlength);

	cl_mem inputBuffer = clCreateBuffer(context, CL_MEM_READ_WRITE |CL_MEM_COPY_HOST_PTR, (strlength) * sizeof(char),(void *) input, NULL);
	cl_mem outputBuffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY , (strlength) * sizeof(char), NULL, NULL);

	cl_kernel kernel = clCreateKernel(program, "mmap", NULL);

	ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&inputBuffer);
	ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&outputBuffer);

    size_t global_work_size[1] = {strlength};



    time_t dec=0;

    for(i=0; i<1000; i++) {
        ret = clEnqueueNDRangeKernel(commandQueue, kernel, 1, NULL, global_work_size, NULL, 0, NULL, NULL);
        if(ret < 0) {
            printf("clEnqueueNDRangeKernel error\n");
        }
    }






	//ret = clEnqueueReadBuffer(commandQueue, outputBuffer, CL_TRUE, 0, strlength * sizeof(char), output, 0, NULL, NULL);
	//ret = clEnqueueReadBuffer(commandQueue, inputBuffer, CL_TRUE, 0, strlength * sizeof(char), output, 0, NULL, NULL);

    char* p = (char*) clEnqueueMapBuffer(commandQueue, inputBuffer, CL_TRUE, CL_MAP_READ, 0, 4, 0, NULL, NULL, &ret);
    if(ret < 0) {
        printf("clEnqueueMapBuffer failed. \n");
    }

	clFlush(commandQueue);


	//printf("output:\n%d, %d, %d, %d\n", output[0], output[1], output[2], output[3]);
	printf("output:\n%d, %d, %d, %d\n", p[0], p[1], p[2], p[3]);

    ret = clEnqueueUnmapMemObject(commandQueue, inputBuffer, p, 0, NULL, NULL);

	ret = clReleaseKernel(kernel);				
	ret = clReleaseProgram(program);	
	ret = clReleaseMemObject(inputBuffer);
	ret = clReleaseMemObject(outputBuffer);
	ret = clReleaseCommandQueue(commandQueue);
	ret = clReleaseContext(context);

	if (output != NULL)
	{
		free(output);
		output = NULL;
	}

	if (devices != NULL)
	{
		free(devices);
		devices = NULL;
	}
    printf("Passed!\n");

	return SUCCESS;
}


