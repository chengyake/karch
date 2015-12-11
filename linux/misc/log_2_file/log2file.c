#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <time.h>



int fd;
unsigned char file_name[64]={0};

int8_t get_new_filename_by_time(int8_t *data) {
    
    time_t tmp;
    struct tm *p;
    
    if(data == NULL) {
        printf("params of get_current_time_string() error");
        return -1;
    }
    
    tmp = time(NULL);
    p=localtime(&tmp);
    sprintf(data, "%4d%02d%02d_%02d%02d%02d.selflog",
        (1900+p->tm_year), (1+p->tm_mon), p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);

    return 0;

}

int open_selflog() {


    if(access(&file_name[0], F_OK|R_OK|W_OK) != 0) { //don't exist
        unsigned char data = 0;

        fd = open(file_name, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);
        if(fd < 0) {
            printf("create %s error", file_name);
            return fd;
        }
        for(i=0; i<(SIZE_OF_MATRIX); i++) {
            ret = write(fd, &data, 1);
            if(ret < 0) {
                printf("write TDataBase.db error");
                return ret;
            }
        }
    } else {
        //restore db queue
        fd = open(file_name, O_RDWR, S_IRUSR|S_IWUSR);
        if(fd < 0) {
            printf("open %s error", file_name);
            return ret;
        }
    }

    ret = lseek(fd, 0, SEEK_END);
    if(ret < 0) {
        printf("create %s fixed space error", file_name);
        return ret;
    }

    return 0;
}

int32_t selflog(char *log) {
    int ret;
    ret = write(fd, log, strlen(log));
    
    return ret;
}



uint8_t close_selflog() {

    close(fd)
    return 0;
}


int main() {

    open_selflog();
    
    selflog("chengyake1");
    selflog("chengyake2\n");
    selflog("chengyake3\r\n");



    close_selflog();

}
