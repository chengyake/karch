#include <stdio.h>
#include <opencv/highgui.h>

  



int main(int argc,char**argv){  

    //IplImage *img = cvLoadImage("timg.jpg", CV_LOAD_IMAGE_GRAYSCALE);
/*
   int i;

    IplImage *img=cvCreateImage(cvSize(400,300), IPL_DEPTH_8U,3);

    for(i=0; i<400*3; i++) {
        img->imageData[i]=0x55;
    }

    cvNamedWindow("Image:",1);
    cvShowImage("Image:",img);
    cvWaitKey(1000);
    cvDestroyWindow("Image:");
    cvReleaseImage(&img);
 */

    int i, j;

    IplImage *img=cvCreateImage(cvSize(400,300), IPL_DEPTH_8U,1);

    for(i=0; i<400; i++) {
        img->imageData[i]=0x55;
    }
    cvNamedWindow("Image:",1);

    for(j=0; j<300; j++) {
        for(i=400*j; i<400+400*j; i++) {
            img->imageData[i]=0x55;
        }
        cvShowImage("Image:",img);
        cvWaitKey(100);
    }

    cvDestroyWindow("Image:");
    cvReleaseImage(&img);

}  
