#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/mman.h>
#include <errno.h>


int main() {


    return 0;

}






uint8_t open_selflog() {


    if(access(FILE_OF_MATRIX, F_OK|R_OK|W_OK) != 0) { //don't exist
        uint8_t data = 0;

        fd = open(FILE_OF_MATRIX, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);
        if(fd < 0) {
            syslog(LOG_ERR, "create %s error", FILE_OF_MATRIX);
            return fd;
        }
        for(i=0; i<(SIZE_OF_MATRIX); i++) {
            ret = write(fd, &data, 1);
            if(ret < 0) {
                syslog(LOG_ERR, "write TDataBase.db error");
                return ret;
            }
        }
    } else {
        //restore db queue
        fd = open(FILE_OF_MATRIX, O_RDWR, S_IRUSR|S_IWUSR);
        if(fd < 0) {
            syslog(LOG_ERR, "open %s error", FILE_OF_MATRIX);
            return ret;
        }
    }

    ret = lseek(fd, 0, SEEK_SET);
    if(ret < 0) {
        syslog(LOG_ERR, "create %s fixed space error", FILE_OF_MATRIX);
        return ret;
    }

    return 0;
}





uint8_t close_selflog() {


    return 0;
}





