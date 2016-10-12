#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <utils/Log.h>

#define FRAME_X 1024
#define FRAME_Y 512
#define FRAME_Z 16

#define OUTPUT_MAX_X   800
#define OUTPUT_MAX_Y   1280


unsigned int idx=0, init_flag=0;

short (*latest_ip)[FRAME_Y/2][FRAME_X];
short (*latest_qp)[FRAME_Y/2][FRAME_X];
int (*table_i2p)[FRAME_Z-1][FRAME_Y/2][FRAME_X];
int (*table_q2p)[FRAME_Z-1][FRAME_Y/2][FRAME_X];
float (*output_vp)[FRAME_Y][FRAME_X];


int close_mu1_opencl();

int init_mu1_opencl() {
    
    idx = 0;
    init_flag = 0;

    latest_ip = malloc(FRAME_X*FRAME_Y/2*sizeof(short));
    latest_qp = malloc(FRAME_X*FRAME_Y/2*sizeof(short));
    table_i2p = malloc(FRAME_X*FRAME_Y/2*sizeof(int)*(FRAME_Z-1));
    table_q2p = malloc(FRAME_X*FRAME_Y/2*sizeof(int)*(FRAME_Z-1));
    output_vp = malloc(FRAME_X*FRAME_Y*sizeof(float));
    
    if(latest_ip== NULL || latest_qp==NULL || table_i2p==NULL || table_q2p==NULL || output_vp==NULL) {
        LOGD("MU1: malloc storage space error\n");
        close_mu1_opencl();
        return -1;
    }

    LOGI("MU1: init mu1 opencl-lib success\n");

    return 0;
}

void process_cfm_by_gpu(unsigned char *dst_p, int dst_w, int dst_h, short *src_p, int src_w, int src_h) { //fixed src_w=frame_x, src_h=frame_y
    
    int i, j, k;
    int size = FRAME_X*FRAME_Y; //half bytes 

    
    if(dst_p==NULL || dst_w==0 || dst_h==0 || src_p==NULL || src_w==0 || src_h==0) {
        LOGE("MU1: process_cfm_by_gpu param error: zero or null");
        return;
    }

    LOGE("MU1: process_cfm_by_gpu  stage A");
    //1. src -> table -> latest
    if(init_flag != 0) {
        for(i=0; i<FRAME_Y/2; i++) {
            for(j=0; j<FRAME_X; j++) {
                (*table_i2p)[idx][i][j] = src_p[FRAME_X*i+j] * (*latest_ip)[i][j];
                (*table_q2p)[idx][i][j] = src_p[FRAME_X*(i+FRAME_Y/2)+j] * (*latest_qp)[i][j];
            }
        }

        memcpy((char *)latest_ip, (char *)(&src_p[0]), size);
        memcpy((char *)latest_qp, (char *)(&src_p[size/2]), size);
    } else {
        LOGD("MU1: dst:%dx%d src:%dx%d\n", dst_w, dst_h, src_w, src_h);
        memcpy((char *)latest_ip, (char *)(&src_p[0]), size);
        memcpy((char *)latest_qp, (char *)(&src_p[size/2]), size);

        for(i=0; i<FRAME_Y/2; i++) {
            for(j=0; j<FRAME_X; j++) {
                (*table_i2p)[idx][i][j] = src_p[FRAME_X*i+j] * (*latest_ip)[i][j];
                (*table_q2p)[idx][i][j] = src_p[FRAME_X*(i+FRAME_Y/2)+j] * (*latest_qp)[i][j];
            }
        }
    
        for(i=1; i<FRAME_Z-1;  i++) {
            memcpy((char *)(&(*table_i2p)[i][0][0]), (char *)(&(*table_i2p)[idx][0][0]), size*sizeof(int)/2);
            memcpy((char *)(&(*table_q2p)[i][0][0]), (char *)(&(*table_q2p)[idx][0][0]), size*sizeof(int)/2);
        }
        init_flag = 1;
    }

    idx++;
    if(idx >= FRAME_Z-1) {
        idx = 0;
    }
    
    LOGE("MU1: process_cfm_by_gpu  stage B");
    //2. add -> V(x,y)
    for(i=0; i<FRAME_Y/2; i++) {
        for(j=0; j<FRAME_X; j++) {

            (*output_vp)[i][j] = 0;
            for(k=0; k<FRAME_Z-1; k++) {
                (*output_vp)[i][j] += ((*table_i2p)[k][i][j] - (*table_q2p)[k][i][j]) / ((*table_i2p)[k][i][j] + (*table_q2p)[k][i][j]);
            }
        }
    }

    LOGE("MU1: process_cfm_by_gpu  stage C");
    
    //3. cut -> dst
    float rate_h = (FRAME_Y/2.0)/ dst_h;
    float rate_w = (FRAME_X/1.0)/ dst_w;
    int x,y;
    for(i=0; i<dst_h; i++) {
        y=i*rate_h;
        for(j=0; j<dst_w; j++) {
            x=j*rate_w;
            dst_p[i*dst_w + j] = ((*output_vp)[y][x] + 128) > 255 ? 255: (((*output_vp)[y][x] + 128) < 0 ? 0: ((*output_vp)[y][x] + 128));
        }
    }

    LOGE("MU1: process_cfm_by_gpu  stage D");
}


int close_mu1_opencl() {
    
    if(table_q2p != NULL) free(table_q2p);
    if(table_i2p != NULL) free(table_i2p);
    if(latest_ip != NULL) free(latest_ip);
    if(latest_qp != NULL) free(latest_qp);
    if(output_vp != NULL) free(output_vp);
    
    LOGI("MU1: close mu1 opencl-lib success\n");

    return 0;
}














