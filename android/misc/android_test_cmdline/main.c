#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>

#include <utils/Log.h>


#define VBLE      0xBE

/* IOCTLs for APPs */
#define IOCTL_CMD_RESET         _IOW(VBLE, 0xB1, void *)
#define IOCTL_CMD_READ_MAC      _IOW(VBLE, 0xB2, unsigned char *)
#define IOCTL_CMD_GET_DEVNAME   _IOW(VBLE, 0xB3, unsigned char *)
#define IOCTL_CMD_SET_DEVNAME   _IOW(VBLE, 0xB4, unsigned char *)
#define IOCTL_CMD_GET_VERSION   _IOW(VBLE, 0xB5, unsigned char *)
#define IOCTL_CMD_QUERY_STATUS  _IOW(VBLE, 0xB6, unsigned char *)
    //ls qpps
#define IOCTL_CMD_ENABLE_BLE    _IOW(VBLE, 0xC0, void *)
#define IOCTL_CMD_DISABLE_BLE   _IOW(VBLE, 0xC1, void *)
#define IOCTL_CMD_BOND       	_IOW(VBLE, 0xC2, unsigned char *)
#define IOCTL_CMD_ADV_START 	_IOW(VBLE, 0xC3, unsigned char *)
#define IOCTL_CMD_ADV_END 		_IOW(VBLE, 0xC4, unsigned char *)
#define IOCTL_CMD_DISCONNECT 	_IOW(VBLE, 0xC5, unsigned char *)
    //firmware update
#define IOCTL_CMD_FW_UPDATE 	_IOW(VBLE, 0xC6, unsigned char *)
    //ancc enable
#define IOCTL_CMD_ANCC_ENABLE 	_IOW(VBLE, 0xC7, unsigned char *)
#define IOCTL_CMD_SET_ACCEL 	_IOW(VBLE, 0xC8, unsigned char *)
#define IOCTL_CMD_SET_ECOMP 	_IOW(VBLE, 0xC9, unsigned char *)
//sensor
#define IOCTL_CMD_ENABLE_G 	    _IOW(VBLE, 0xCA, unsigned char *)
#define IOCTL_CMD_DISABLE_G 	_IOW(VBLE, 0xCB, unsigned char *)
#define IOCTL_CMD_ENABLE_E 	    _IOW(VBLE, 0xCC, unsigned char *)
#define IOCTL_CMD_DISABLE_E 	_IOW(VBLE, 0xCD, unsigned char *)
#define IOCTL_CMD_ENABLE_STEP_DETECT 	    _IOW(VBLE, 0xCE, unsigned char *)
#define IOCTL_CMD_DISABLE_STEP_DETECT 	    _IOW(VBLE, 0xCF, unsigned char *)
#define IOCTL_CMD_ENABLE_STEP_STORE 	    _IOW(VBLE, 0xD0, unsigned char *)
#define IOCTL_CMD_DISABLE_STEP_STORE 	    _IOW(VBLE, 0xD1, unsigned char *)
#define IOCTL_CMD_SET_VBLE_TIME 	        _IOW(VBLE, 0xD2, unsigned char *)



static int fd;
static unsigned char buffer[21];

int open_ble()
{   
    int ret = 0;

    LOGE("chengyake in %s:\n", __FUNCTION__);

    fd = open("/dev/vble", O_RDWR);
    if (fd < 0) {
        LOGE("chengyake open device vble error\n");
        return fd;
    }

    return 0;
}

int reset_ble()  //reset
{   
    LOGE("chengyake in %s:\n", __FUNCTION__);

    return ioctl(fd, IOCTL_CMD_RESET, NULL);
}

int enAccel()
{
    int ret;

    LOGE("chengyake in %s:\n", __FUNCTION__);

    memset(&buffer, 0, sizeof(buffer));
    buffer[0]=1;
    ret = ioctl(fd, IOCTL_CMD_ENABLE_G, buffer);
    if(ret < 0) {
        LOGE("chengyake ioctl %s error\n", __FUNCTION__);
    }
    return ret;
}
int disAccel()
{
    int ret;

    LOGE("chengyake in %s:\n", __FUNCTION__);

    memset(&buffer, 0, sizeof(buffer));
    ret = ioctl(fd, IOCTL_CMD_ENABLE_G, buffer);
    if(ret < 0) {
        LOGE("chengyake ioctl %s error\n", __FUNCTION__);
    }
    return ret;
}
int enEcomp()
{
    int ret;

    LOGE("chengyake in %s:\n", __FUNCTION__);

    memset(&buffer, 0, sizeof(buffer));
    buffer[0]=1;
    ret = ioctl(fd, IOCTL_CMD_ENABLE_E, buffer);
    if(ret < 0) {
        LOGE("chengyake ioctl %s error\n", __FUNCTION__);
    }
    return ret;
}
int disEcomp()
{
    int ret;

    LOGE("chengyake in %s:\n", __FUNCTION__);

    memset(&buffer, 0, sizeof(buffer));
    ret = ioctl(fd, IOCTL_CMD_ENABLE_E, buffer);
    if(ret < 0) {
        LOGE("chengyake ioctl %s error\n", __FUNCTION__);
    }
    return ret;
}

int enDetect()
{
    int ret;

    LOGE("chengyake in %s:\n", __FUNCTION__);

    memset(&buffer, 0, sizeof(buffer));
    ret = ioctl(fd, IOCTL_CMD_ENABLE_STEP_DETECT, buffer);
    if(ret < 0) {
        LOGE("chengyake ioctl %s error\n", __FUNCTION__);
    }
    return ret;
}
int disDetect()
{
    int ret;

    LOGE("chengyake in %s:\n", __FUNCTION__);

    memset(&buffer, 0, sizeof(buffer));
    ret = ioctl(fd, IOCTL_CMD_DISABLE_STEP_DETECT, buffer);
    if(ret < 0) {
        LOGE("chengyake ioctl %s error\n", __FUNCTION__);
    }
    return ret;
}
int enStore()
{
    int ret;

    LOGE("chengyake in %s:\n", __FUNCTION__);

    memset(&buffer, 0, sizeof(buffer));
    ret = ioctl(fd, IOCTL_CMD_ENABLE_STEP_STORE, buffer);
    if(ret < 0) {
        LOGE("chengyake ioctl %s error\n", __FUNCTION__);
    }
    return ret;
}
int disStore()
{
    int ret;

    LOGE("chengyake in %s:\n", __FUNCTION__);

    memset(&buffer, 0, sizeof(buffer));
    ret = ioctl(fd, IOCTL_CMD_DISABLE_STEP_STORE, buffer);
    if(ret < 0) {
        LOGE("chengyake ioctl %s error\n", __FUNCTION__);
    }
    return ret;
}
int setTime()
{
    int ret;

    LOGE("chengyake in %s:\n", __FUNCTION__);

    memset(&buffer, 0, sizeof(buffer));
    ret = ioctl(fd, IOCTL_CMD_SET_VBLE_TIME, buffer);
    if(ret < 0) {
        LOGE("chengyake ioctl %s error\n", __FUNCTION__);
    }
    return ret;
}
int update_ble()
{
    int ret;

    LOGE("chengyake in %s:\n", __FUNCTION__);

    memset(&buffer, 0, sizeof(buffer));
    ret = ioctl(fd, IOCTL_CMD_FW_UPDATE, buffer);
    if(ret < 0) {
        LOGE("chengyake ioctl %s error\n", __FUNCTION__);
    }
    return ret;
}

int getMac()
{
    int ret;

    LOGE("chengyake in %s:\n", __FUNCTION__);

    memset(&buffer, 0, sizeof(buffer));
    ret = ioctl(fd, IOCTL_CMD_READ_MAC, buffer);
    if(ret < 0) {
        LOGE("chengyake ioctl %s error\n", __FUNCTION__);
    }
    return ret;
}
/*****************main func****************/
int main(int argc, char **argv)
{

    char cmdline[16], cmd[16];
    char *p;
    LOGE("This is chengyake's test program...\n");
    while(1)
    {
        LOGE("\n>>");

        memset(&cmdline, 0, sizeof(cmdline));
        memset(&cmd, 0, sizeof(cmd));

        gets(&cmdline[0]);

        p=NULL;
        p = strtok(cmdline, " ");
        if (p) sprintf(&cmd[0], "%s", p);


        if(!strcmp(&cmd[0], "open")) {
            open_ble();
        } else if (!strcmp(&cmd[0], "reset")) {
            reset_ble();
        } else if (!strcmp(&cmd[0], "enAccel")) {
            enAccel();
        } else if (!strcmp(&cmd[0], "disAccel")) {
            disAccel();
        } else if (!strcmp(&cmd[0], "enEcomp")) {
            enEcomp();
        } else if (!strcmp(&cmd[0], "disEcomp")) {
            disEcomp();
        } else if (!strcmp(&cmd[0], "enDetect")) {
            enDetect();
        } else if (!strcmp(&cmd[0], "disDetect")) {
            disDetect();
        } else if (!strcmp(&cmd[0], "enStore")) {
            enStore();
        } else if (!strcmp(&cmd[0], "disStore")) {
            disStore();
        } else if (!strcmp(&cmd[0], "setTime")) {
            setTime();
        } else if (!strcmp(&cmd[0], "update")) {
            update_ble();
        } else if (!strcmp(&cmd[0], "getMac")) {
            getMac();
        } else if (!strcmp(&cmd[0], "exit")) {
            LOGE("chengyake cmd: exit\n");
            break;
        } else {
            LOGE("----------------------------------------\n");
            LOGE("cmd\topen\n");
            LOGE("cmd\treset\n");
            LOGE("cmd\tenAccel\n");
            LOGE("cmd\tdisAccel\n");
            LOGE("cmd\tenEcomp\n");
            LOGE("cmd\tdisEcomp\n");
            LOGE("cmd\tenDetect\n");
            LOGE("cmd\tdisDetect\n");
            LOGE("cmd\tenStore\n");
            LOGE("cmd\tdisStore\n");
            LOGE("cmd\tsetTime\n");
            LOGE("cmd\tupdate\n");
            LOGE("cmd\tgetMac\n");
            LOGE("cmd\texit\n");
            LOGE("----------------------------------------\n");
        }

    }

    LOGE("chengyake end\n");
    close(fd);

    return 0;
}





