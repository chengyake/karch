#include <stdio.h>
#include <stdlib.h>
#include <alloca.h>

#include <CL/cl.h>






static void display(cl_device_id id, cl_device_info name) {
    cl_int ret;
    size_t size;

    ret = clGetDeviceInfo(id, name, 0, NULL, &size);
    if (ret != CL_SUCCESS) {
        printf("Failed to find OpenCL device info size \n");
        return;
    }

    void * info = (void *)alloca(64 * size);
    ret = clGetDeviceInfo(id, name, size, info, NULL);
    if (ret != CL_SUCCESS) {
        printf("Failed to find OpenCL device info param \n");
        return;
    }
    
    switch (name) {
        case CL_DEVICE_TYPE: {
            unsigned int flag = (*(cl_device_type *)info);
            printf("\t\t");
            if(flag == CL_DEVICE_TYPE_ALL) {
                printf("CL_DEVICE_TYPE_ALL\n");
                break;
            }
            if(flag & CL_DEVICE_TYPE_CPU) {
                printf("CL_DEVICE_TYPE_CPU ");
            }
            if(flag & CL_DEVICE_TYPE_GPU) {
                printf("CL_DEVICE_TYPE_GPU ");
            }
            if(flag & CL_DEVICE_TYPE_ACCELERATOR) {
                printf("ACCELERATOR ");
            }
            if(flag & CL_DEVICE_TYPE_DEFAULT) {
                printf("DEFAULT ");
            }
            printf("\n");
            break;


        } case CL_DEVICE_MAX_COMPUTE_UNITS: {
            unsigned int value = (*(cl_device_type *)info);
            printf("\t\tCL_DEVICE_MAX_COMPUTE_UNITS %d\n", value);
            break;
        } case CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS: {
            unsigned int value = (*(cl_device_type *)info);
            printf("\t\tCL_DEVICE_MAX_WORK_ITEM_DIMENSIONS %d\n", value);
            break;
        } case CL_DEVICE_MAX_WORK_ITEM_SIZES: {
            size_t *value = (size_t *)info;
            printf("\t\tCL_DEVICE_MAX_WORK_ITEM_SIZES [%lu][%lu][%lu]\n", value[0], value[1], value[2]);
            break;
        } case CL_DEVICE_MAX_CLOCK_FREQUENCY: {
            unsigned int value = (*(cl_device_type *)info);
            printf("\t\tCL_DEVICE_MAX_CLOCK_FREQENCY %d\n", value);
            break;
        }

        default:
            break;

    }

}
        

void display_platform_info(cl_platform_id id, cl_platform_info name, char str[]) {

	cl_int ret;
	size_t size;

	ret = clGetPlatformInfo(id, name, 0, NULL, &size);
	if (ret != CL_SUCCESS) {
		printf("Failed to find OpenCL platform param %s size.\n", str);
		return;
	}

	char * info = (char *)alloca(sizeof(char) * size);
	ret = clGetPlatformInfo(id, name, size, info, NULL);
	if (ret != CL_SUCCESS) {
		printf("Failed to find OpenCL platform param %s.\n", str);
		return;
	}
	printf("\t%s:\t%s\n", str, info);

}




void display_info() {
    
    cl_int ret;
    cl_int i, j;

    cl_uint platforms_num;
    cl_uint devices_num;

    ret = clGetPlatformIDs(0, NULL, &platforms_num);
    if (ret != CL_SUCCESS || platforms_num <= 0) {
		printf("Failed to find any OpenCL platform.\n");
		return;
    }


    cl_platform_id * platforms_ids = (cl_platform_id *)alloca(sizeof(cl_platform_id)*platforms_num);
    ret = clGetPlatformIDs(platforms_num, platforms_ids, NULL);
    if (ret != CL_SUCCESS) {
		printf("Failed to find any OpenCL platforms.\n");
		return;
    }



	for (i=0; i<platforms_num; i++) {

        printf("Number of platforms:%d \n", platforms_num); 
		display_platform_info(platforms_ids[i], CL_PLATFORM_PROFILE, "CL_PLATFORM_PROFILE");
		display_platform_info(platforms_ids[i], CL_PLATFORM_VERSION, "CL_PLATFORM_VERSION");
		display_platform_info(platforms_ids[i], CL_PLATFORM_VENDOR, "CL_PLATFORM_VENDOR");
		display_platform_info(platforms_ids[i], CL_PLATFORM_EXTENSIONS, "CL_PLATFORM_EXTENSIONS");

		ret = clGetDeviceIDs(platforms_ids[i], CL_DEVICE_TYPE_ALL, 0, NULL, &devices_num);
		if (ret != CL_SUCCESS) {
			printf("Failed to find OpenCL devices num.\n");
			return;
		}

		cl_device_id * devices = (cl_device_id *)alloca(sizeof(cl_device_id) * devices_num);
		ret = clGetDeviceIDs(platforms_ids[i], CL_DEVICE_TYPE_ALL, devices_num, devices, NULL);
		if (ret != CL_SUCCESS) {
			printf("Failed to find OpenCL devices param.\n");
			return;
		}

        for (j=0; j<devices_num; j++) {
            printf("\n\tNumber of devices:%d \n", j);

            display(devices[j], CL_DEVICE_TYPE);
            display(devices[j], CL_DEVICE_MAX_COMPUTE_UNITS);
            display(devices[j], CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS);
            display(devices[j], CL_DEVICE_MAX_WORK_ITEM_SIZES);


            display(devices[j], CL_DEVICE_MAX_CLOCK_FREQUENCY);



        }



    }



}


int main() {

    display_info();
    return 0;
}
