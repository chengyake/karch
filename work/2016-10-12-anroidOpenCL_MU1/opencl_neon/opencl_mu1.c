#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <linux/types.h>
#include <fcntl.h>
#include <utils/Log.h>


#define NEON_MU1

#ifdef NEON_MU1
#include <arm_neon.h>
#endif



int table_i[5120][1024], table_q[5120][1024];
int table_o[5120][1024];


int init_mu1_opencl() {

    int i,j;
    
    for(i=0; i<5120; i++) {
        for(j=0; j<1024; j++) {
            table_i[i][j]=20;
            table_q[i][j]=10;
            table_o[i][j]=1;
        }
    }

    return 0;
}

void process_cfm_by_gpu(unsigned char *pDataDst, int nDstWidth, int nDstHeight, short *pSrcData, int nSrcWidth, int nSrcHeight) {

    int i,j, k;

#ifndef NEON_MU1
    LOGD("MU1 ---------------------- start CPU");
    for(i=0; i<5120; i++) {
        for(j=0; j<1024; j++) {
            table_o[i][j] = (table_i[i][j] + table_q[i][j]) / (table_i[i][j] - table_q[i][j]);
            table_o[i][j] += (table_i[i][j] + table_q[i][j] - 1) / (table_i[i][j] - table_q[i][j] + 1);
        }
    }
    LOGD("MU1 ---------------------- end");
#else


    int32x4_t z = vdupq_n_s32(1);
    LOGD("MU1 ---------------------- start NEON");
    for(i=0; i<5120; i++) {

#if 0
        for(j=0; j<1024; j+=4) {
              int32x4_t a = vld1q_s32(&table_i[i][j]);
              int32x4_t b = vld1q_s32(&table_q[i][j]);
              int32x4_t c = vaddq_s32(a, b);
              int32x4_t d = vsubq_s32(a, b);
                        a = vsubq_s32(c, z);
                        b = vaddq_s32(d, z);
            float32x4_t x = vrecpeq_f32(vcvtq_f32_s32(d));
            float32x4_t y = vrecpeq_f32(vcvtq_f32_s32(b));
            float32x4_t m = vmulq_f32(x, vcvtq_f32_s32(c));
            float32x4_t n = vmulq_f32(y, vcvtq_f32_s32(a));
                        x = vaddq_f32(m, n);
            vst1q_s32(&table_o[i][j], vcvtq_s32_f32(x));
        }

#else

        for(j=0; j<1024; j+=8) {

              int32x4_t a1 = vld1q_s32(&table_i[i][j]);
              int32x4_t b1 = vld1q_s32(&table_q[i][j]);
              int32x4_t a2 = vld1q_s32(&table_i[i][j+4]);
              int32x4_t b2 = vld1q_s32(&table_q[i][j+4]);

              int32x4_t c1 = vaddq_s32(a1, b1);
              int32x4_t d1 = vsubq_s32(a1, b1);
              int32x4_t c2 = vaddq_s32(a2, b2);
              int32x4_t d2 = vsubq_s32(a2, b2);

                        a1 = vsubq_s32(c1, z);
                        b1 = vaddq_s32(d1, z);
                        a2 = vsubq_s32(c2, z);
                        b2 = vaddq_s32(d2, z);

            float32x4_t x1 = vrecpeq_f32(vcvtq_f32_s32(d1));
            float32x4_t y1 = vrecpeq_f32(vcvtq_f32_s32(b1));
            float32x4_t x2 = vrecpeq_f32(vcvtq_f32_s32(d2));
            float32x4_t y2 = vrecpeq_f32(vcvtq_f32_s32(b2));

            float32x4_t m1 = vmulq_f32(x1, vcvtq_f32_s32(c1));
            float32x4_t n1 = vmulq_f32(y1, vcvtq_f32_s32(a1));
            float32x4_t m2 = vmulq_f32(x1, vcvtq_f32_s32(c2));
            float32x4_t n2 = vmulq_f32(y1, vcvtq_f32_s32(a2));

                        x1 = vaddq_f32(m1, n1);
                        x2 = vaddq_f32(m2, n2);

            vst1q_s32(&table_o[i][j], vcvtq_s32_f32(x1));
            vst1q_s32(&table_o[i][j+4], vcvtq_s32_f32(x2));

        }
#endif
    }
    LOGD("MU1 ---------------------- end");





#endif
}

int close_mu1_opencl() {

    
    return 0;
}


int main() {

    init_mu1_opencl();
    
    process_cfm_by_gpu(NULL, 0, 0, NULL, 0, 0);

    
    LOGD("MU1: table_o: %d %d %d\n", table_o[0][0], table_o[17][17], table_o[511][1023]);

    close_mu1_opencl();

    return 0;

}










#if 0
    int32x4_t z = vdupq_n_s32(1);
    LOGD("MU1 ---------------------- start C");
    for(i=0; i<5120; i++) {
        for(j=0; j<1024; j+=4) {
              int32x4_t a = vld1q_s32(&table_i[i][j]);
              int32x4_t b = vld1q_s32(&table_q[i][j]);
              int32x4_t c = vaddq_s32(a, b);
              int32x4_t d = vsubq_s32(a, b);
                        a = vsubq_s32(c, z);
                        b = vaddq_s32(d, z);
            float32x4_t x = vrecpeq_f32(vcvtq_f32_s32(d));
            float32x4_t y = vrecpeq_f32(vcvtq_f32_s32(b));
            float32x4_t m = vmulq_f32(x, vcvtq_f32_s32(c));
            float32x4_t n = vmulq_f32(y, vcvtq_f32_s32(a));
                        x = vaddq_f32(m, n);
            vst1q_s32(&table_o[i][j], vcvtq_s32_f32(x));
        }
    }
    LOGD("MU1 ---------------------- end");



    table_o[i][j+p] = (table_i[i][j+p] + table_q[i][j+p]) / (table_i[i][j+p] - table_q[i][j+p]);
    table_o[i][j+p] += (table_i[i][j+p] + table_q[i][j+p] - 1) / (table_i[i][j+p] - table_q[i][j+p] + 1);

#endif





