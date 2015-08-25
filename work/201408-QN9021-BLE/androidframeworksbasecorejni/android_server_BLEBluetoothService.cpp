/*
 ** Copyright 2010, The Android Open Source Project
 **
 ** Licensed under the Apache License, Version 2.0 (the "License");
 ** you may not use this file except in compliance with the License.
 ** You may obtain a copy of the License at
 **
 **     http://www.apache.org/licenses/LICENSE-2.0
 **
 ** Unless required by applicable law or agreed to in writing, software
 ** distributed under the License is distributed on an "AS IS" BASIS,
 ** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 ** See the License for the specific language governing permissions and
 ** limitations under the License.
 */

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
#include <pthread.h>

#include <utils/Log.h>

#include "jni.h"
#include "JNIHelp.h"
#include <android_runtime/AndroidRuntime.h>
#include "hardware_legacy/power.h"

#define BLE_WAKE_LOCAK_NAME "BLE_JNI"

#define VBLE      0xBE
#define BUFFER_SIZE      40


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
#define IOCTL_CMD_ENABLE_G         _IOW(VBLE, 0xCA, unsigned char *)
#define IOCTL_CMD_DISABLE_G    _IOW(VBLE, 0xCB, unsigned char *)
#define IOCTL_CMD_ENABLE_E         _IOW(VBLE, 0xCC, unsigned char *)
#define IOCTL_CMD_DISABLE_E    _IOW(VBLE, 0xCD, unsigned char *)
#define IOCTL_CMD_ENABLE_STEP_DETECT       _IOW(VBLE, 0xCE, unsigned char *)
#define IOCTL_CMD_DISABLE_STEP_DETECT      _IOW(VBLE, 0xCF, unsigned char *)
#define IOCTL_CMD_ENABLE_STEP_STORE        _IOW(VBLE, 0xD0, unsigned char *)
#define IOCTL_CMD_DISABLE_STEP_STORE       _IOW(VBLE, 0xD1, unsigned char *)
#define IOCTL_CMD_SET_VBLE_TIME                _IOW(VBLE, 0xD2, unsigned char *)



namespace android {

typedef enum {
    SLEEP,
    ADVERTISING,
    CONNECT_QPPS,//all of the notifications have been confige
    CONNECT,
} vble_mode;

struct event_loop_native_data_t {
    JavaVM *vm;
    int envVer;
    jobject me;
};
typedef event_loop_native_data_t native_data_t;


pthread_mutex_t mutex;

static int fd;
static unsigned char data[BUFFER_SIZE];
static unsigned char name[BUFFER_SIZE];
static unsigned char mac[6];
static int all_status;
static int dev_status = SLEEP;
static native_data_t *nat = NULL;  // global native data

static jmethodID method_onStateChanged;
static jmethodID method_onDataComing;

int get_status();

static void classInitNative(JNIEnv* env, jclass clazz)
{
    method_onStateChanged = env->GetMethodID(clazz, "onStateChanged", "(I)V");
    method_onDataComing = env->GetMethodID(clazz, "onDataComing", "([B)Z");
}

static void initializeNativeDataNative(JNIEnv* env, jobject object)
{
    LOGE(" in %s:", __FUNCTION__);
	nat = (native_data_t *)calloc(1, sizeof(native_data_t));
    if (NULL == nat) {
        LOGE("%s: out of memory!", __FUNCTION__);
        return;
    }
    env->GetJavaVM( &(nat->vm) );
    nat->envVer = env->GetVersion();
    nat->me = env->NewGlobalRef(object);
    pthread_mutex_init(&mutex, NULL);

}


static void signal_handler(int sig)
{

    JNIEnv *env = NULL;

    LOGE(" in %s\n", __FUNCTION__);

    pthread_mutex_lock(&mutex);
    acquire_wake_lock( PARTIAL_WAKE_LOCK, BLE_WAKE_LOCAK_NAME );

	int all_status_old = all_status;
	all_status = get_status();
	if(all_status_old != all_status)
	{

	    if(nat->vm->GetEnv((void**)&env, nat->envVer) == JNI_OK)
	    {
		    env->CallVoidMethod(nat->me, method_onStateChanged, all_status);
	    }
		else
		{
			LOGE(" signal_handler nat->vm->GetEnv ERR\n");

		}
	}

    release_wake_lock( BLE_WAKE_LOCAK_NAME );
    pthread_mutex_unlock(&mutex);

}

int open_ble()
{
	int ret = 0;
    unsigned int f_flags;

    LOGE(" in %s:\n", __FUNCTION__);

    fd = open("/dev/vble", O_RDWR);
    if (fd < 0) {
        LOGE(" open device vble error\n");
        return fd;
    }

    //signal(SIGIO, signal_handler);
    if(ret < 0) {
        LOGE(" registe signal handler error\n");
        return ret;
    }

    ret = fcntl(fd, F_SETOWN, getpid());
    if(ret < 0) {
        LOGE(" registe pid to kernel error\n");
        return ret;
    }

    f_flags = fcntl(fd, F_GETFL);
    ret = fcntl(fd, F_SETFL, f_flags | FASYNC);
    if(ret < 0) {
        LOGE(" config fasync error\n");
        return ret;
    }

    return 0;
}

int reset_ble()  //reset
{

    dev_status = SLEEP;

    LOGE(" in %s:\n", __FUNCTION__);

    if(fd <= 0)
        return -1;

    return ioctl(fd, IOCTL_CMD_RESET, NULL);
}

int adv_start()
{
    LOGE(" in %s:\n", __FUNCTION__);

    if(fd <= 0)
        return -1;

    return ioctl(fd, IOCTL_CMD_ADV_START, NULL);
}

int adv_end()
{
    LOGE(" in %s:\n", __FUNCTION__);

    if(fd <= 0)
        return -1;

    return ioctl(fd, IOCTL_CMD_ADV_END, NULL);
}

int dis_conn()
{
    LOGE(" in %s:\n", __FUNCTION__);

    if(fd <= 0)
        return -1;

    return ioctl(fd, IOCTL_CMD_DISCONNECT, NULL);
}



int get_mac_addr()
{
    int ret;

    LOGE(" in %s:\n", __FUNCTION__);

    if(fd <= 0)
        return -1;

    memset(&mac, 0, sizeof(mac));
    ret = ioctl(fd, IOCTL_CMD_READ_MAC, mac);
    if(ret < 0)
    {
        LOGE(" ioctl get mac addr error\n");
    }
    return ret;
}

int get_status()
{
    int ret;
    int all_status_t;
	unsigned char s_data[BUFFER_SIZE] = {0};

    LOGE(" in %s:\n", __FUNCTION__);

    if(fd <= 0)
        return -1;

    ret = ioctl(fd, IOCTL_CMD_QUERY_STATUS, s_data);
    if(ret < 0)
    {
        LOGE("get_status ioctl error\n");
    }

	all_status_t = (0xFF&s_data[1]) | ((0xFF&s_data[2])<<8) | ((0xFF&s_data[3])<<16);
    dev_status = s_data[1];

    LOGE(" get status : 0x%x\n", all_status_t);

    return all_status_t;
}

int enable_ble()
{
    LOGE(" in %s:\n", __FUNCTION__);

    if(fd <= 0)
        return -1;

    return ioctl(fd, IOCTL_CMD_ENABLE_BLE, NULL);
}

int disable_ble()
{
    LOGE(" in %s:\n", __FUNCTION__);

    if(fd <= 0)
        return -1;

    return ioctl(fd, IOCTL_CMD_DISABLE_BLE, NULL);
}


static jboolean init(JNIEnv* env, jobject clazz) {

    LOGE(" in %s:\n", __FUNCTION__);

    if(open_ble() != 0) {
        LOGE(" in %s: open ble error\n", __FUNCTION__);
        return false;
    }

    if(enable_ble() != 0) {
        LOGE(" in %s: enable ble error\n", __FUNCTION__);
        return false;
    }

    //if(get_status() < 0) {
    //    LOGE(" in %s: get_status ble error\n", __FUNCTION__);
    //    return false;
    //}
    return true;
}

static jboolean reset(JNIEnv* env, jobject clazz) {

    LOGE(" in %s:\n", __FUNCTION__);

    if(open_ble() != 0) {
        LOGE(" in %s: open ble error\n", __FUNCTION__);
        return false;
    }

    if(reset_ble() != 0) {
        LOGE(" in %s: reset ble error\n", __FUNCTION__);
        return false;
    }

    if(enable_ble() != 0) {
        LOGE(" in %s: enable ble error\n", __FUNCTION__);
        return false;
    }

    return true;
}

static jboolean advertising(JNIEnv* env, jobject clazz) {

    if(dev_status == SLEEP) {
        LOGE(" in %s:\n", __FUNCTION__);
        if(adv_start() < 0) {
            LOGE(" in %s: error \n", __FUNCTION__);
            return false;
        }
		return true;
    }
    return false;
}
static jboolean advertising_stop(JNIEnv* env, jobject clazz) {

    if(dev_status == ADVERTISING) {
        LOGE(" in %s:\n", __FUNCTION__);
        if(adv_end() < 0) {
            LOGE(" in %s: error \n", __FUNCTION__);
            return false;
        }
	    return true;
	}
    return false;
}
static jboolean disconnect(JNIEnv* env, jobject clazz) {

    if((dev_status == CONNECT_QPPS)||(dev_status == CONNECT)) {
        LOGE(" in %s:\n", __FUNCTION__);
        if(dis_conn() < 0) {
            LOGE(" in %s: error \n", __FUNCTION__);
            return false;
	    }
	    return true;
	}
    return false;
}


static jboolean disable(JNIEnv* env, jobject clazz) {

    LOGE(" in %s:", __FUNCTION__);

    if(fd <= 0) {
    	return false;
    }

	if(dev_status == ADVERTISING)
	{
		if(adv_end() != 0) {
			LOGE(" in %s: adv_end ble error\n", __FUNCTION__);
			return false;
		}

	}
	else if((dev_status == CONNECT_QPPS) || (dev_status == CONNECT))
	{
		if(dis_conn() != 0) {
			LOGE(" in %s: dis_conn ble error\n", __FUNCTION__);
			return false;
		}

	}

	if(disable_ble() != 0) {
		LOGE(" in %s: disable_ble ble error\n", __FUNCTION__);
		return false;
	}

    return true;

}

static jint write(JNIEnv* env, jobject clazz, jbyteArray jbuf, jint jlen) {

    int i, ret;

    if(fd <= 0 && (jlen > 20 || jlen <= 0))
        return -1;

    jbyte *p = env->GetByteArrayElements(jbuf, NULL);

    LOGE(" in %s:", __FUNCTION__);

    memset(&data, 0, sizeof(data));

    for(i=1; i<=jlen; i++)
    {
        data[i] = p[i-1];
    }
    data[0] = jlen;

    //ret = ioctl(fd, IOCTL_CMD_SEND_DATA, data);
    ret=-1;
    if(ret < 0)
    {
        LOGE("write ioctl IOCTL_CMD_SEND_DATA error\n");
    }

    env->ReleaseByteArrayElements(jbuf, p, 0);

    return ret;

}

static jint getStatus(JNIEnv* env, jobject clazz) {

    LOGE(" in %s:", __FUNCTION__);

    return get_status();
}

static void setStatus(JNIEnv* env, jobject clazz, jint status) {

    LOGE(" in %s:", __FUNCTION__);

    dev_status = status&0xFF;
}

static jstring getStringMacAddress(JNIEnv *env, jobject object)
{
    int length = 0;
	jchar *pNewStr = NULL;
	jstring str;

	if ( get_mac_addr() < 0 )
    {
        str = NULL;
    }
	else
	{
		const char *pStr = (const char*)mac;

		length = 6*sizeof(jchar);
		pNewStr = (jchar*)malloc( length );
		if ( pNewStr )
		{
			memset( pNewStr, 0x00, length );

			for ( unsigned iI = 0; iI < 6; iI++ )
			{
				pNewStr[iI] = (jchar)pStr[iI];
			}

			str = env->NewString( (const jchar*)pNewStr, 6 );
			int tmpLen = env->GetStringLength( str );

			LOGE("[BLE JNI]:%s, tmpLen=%d", __FUNCTION__,tmpLen);
		}
		else
		{
		    str = NULL;
			LOGE("[BLE JNI]:%s, malloc failed", __FUNCTION__);
		}
	}

    if(pNewStr)
    {
		free(pNewStr);
		pNewStr = NULL;
    }

	return str;
}

static jint setName(JNIEnv* env, jobject clazz, jstring js, jint jlen) {

    int i, ret;

    if(fd <= 0 || js == NULL  || jlen > BUFFER_SIZE-1) {
        return -1;
    }

    const char* p = env->GetStringUTFChars(js, 0);
    LOGE(" in %s:", __FUNCTION__);

    memset(&name, 0, sizeof(name));
    int len = env->GetStringUTFLength(js);
    LOGE(" in %s: len = %d", __FUNCTION__, len);

    for(i=1; i<=len; i++)
    {
        name[i] = p[i-1];
    }
    name[i+1] = '\0';
    name[0] = len+1;

    ret = ioctl(fd, IOCTL_CMD_SET_DEVNAME, name);
    if(ret < 0)
    {
        LOGE("set name ioctl error\n");
    }

    env->ReleaseStringUTFChars(js, p);
    return ret;

}

static jstring getName(JNIEnv* env, jobject clazz) {

	jstring str;
    int ret;

    LOGE(" in %s:", __FUNCTION__);

    if(fd <= 0) {
        LOGE(" in %s: File handler error", __FUNCTION__);
        return NULL;
    }

    memset(&name, 0, sizeof(name));
    ret = ioctl(fd, IOCTL_CMD_GET_DEVNAME, name);
    if(ret < 0)
    {
        LOGE("get name ioctl error\n");
        return NULL;
    }
    LOGE(" in %s: name[0] = %d", __FUNCTION__, name[0]);

    char* CharName = (char*)&name[1];
    LOGE(" in %s: CharName = %s", __FUNCTION__, CharName);
    str = env->NewStringUTF(CharName);

	return str;


}

static jboolean enableStepDetect(JNIEnv* env, jobject clazz) {
    
    int ret;

    LOGE(" in %s:", __FUNCTION__);

    if(fd <= 0) {
        LOGE(" in %s: File handler error", __FUNCTION__);
        return false;
    }

    ret = ioctl(fd, IOCTL_CMD_ENABLE_STEP_DETECT, data);
    if(ret < 0)
    {
        LOGE("enableStepDetect ioctl error:%d\n", ret);
    }
    return ret >=0 ? true : false;

}

static jboolean disableStepDetect(JNIEnv* env, jobject clazz) {
    
    int ret;

    LOGE(" in %s:", __FUNCTION__);

    if(fd <= 0) {
        LOGE(" in %s: File handler error", __FUNCTION__);
        return false;
    }

    ret = ioctl(fd, IOCTL_CMD_DISABLE_STEP_DETECT, data);
    if(ret < 0)
    {
        LOGE("disableStepDetect ioctl error\n");
    }
    return ret >=0 ? true : false;

}

static jboolean enableStepStore(JNIEnv* env, jobject clazz) {
    
    int ret;

    LOGE(" in %s:", __FUNCTION__);

    if(fd <= 0) {
        LOGE(" in %s: File handler error", __FUNCTION__);
        return false;
    }

    ret = ioctl(fd, IOCTL_CMD_ENABLE_STEP_STORE, data);

    return ret >=0 ? true : false;
}

static jboolean disableStepStore(JNIEnv* env, jobject clazz) {
    
    int ret;

    LOGE(" in %s:", __FUNCTION__);

    if(fd <= 0) {
        LOGE(" in %s: File handler error", __FUNCTION__);
        return false;
    }

    ret = ioctl(fd, IOCTL_CMD_DISABLE_STEP_STORE, data);


    return ret >=0 ? true : false;

}

static jboolean setTime(JNIEnv* env, jobject clazz, jbyteArray jbuf, jint jlen) {

    int i, ret;

    if(fd <= 0 || jbuf == NULL  || jlen != 6) {
        return -1;
    }

    jbyte *p = env->GetByteArrayElements(jbuf, NULL);
    LOGE(" in %s:", __FUNCTION__);

    memset(&data, 0, sizeof(data));
    int len = jlen;
    LOGE(" in %s: len = %d", __FUNCTION__, len);

    for(i=0; i<=len; i++)
    {
        data[i] = p[i];
    }

    ret = ioctl(fd, IOCTL_CMD_SET_VBLE_TIME, data);
    if(ret < 0)
    {
        LOGE("set time ioctl error\n");
    }

    env->ReleaseByteArrayElements(jbuf, p, 0);
    return ret >= 0 ? true : false;

}




static const JNINativeMethod g_methods[] = {
    {"classInitNative", "()V", (void *)classInitNative},
    {"initializeNativeDataNative", "()V", (void *)initializeNativeDataNative},
    { "native_init", "()Z", (void*)init },
    { "native_reset", "()Z", (void*)reset },
    { "native_advertising", "()Z", (void*)advertising },
    { "native_disable", "()Z", (void*)disable },
    { "native_write", "([BI)I", (void*)write },
    { "native_getStatus", "()I", (void*)getStatus},
    { "native_setStatus", "(I)V", (void*)setStatus},
    { "getStringMacAddress", "()Ljava/lang/String;", (void*)getStringMacAddress},
    { "native_get_name", "()Ljava/lang/String;", (void*)getName},
    { "native_set_name", "(Ljava/lang/String;I)I", (void*)setName },
    { "native_advertising_end", "()Z", (void*)advertising_stop },
    { "native_disconnect", "()Z", (void*)disconnect },
    { "native_enable_step_detect", "()Z", (void*)enableStepDetect },
    { "native_disable_step_detect", "()Z", (void*)disableStepDetect },
    { "native_enable_step_store", "()Z", (void*)enableStepStore },
    { "native_disable_step_store", "()Z", (void*)disableStepStore },
    { "native_set_ble_time", "([BI)Z", (void*)setTime },


};

int register_android_server_BLEBluetoothService(JNIEnv* env) {

    return AndroidRuntime::registerNativeMethods(env, "android/server/BLEBluetoothService",
                                                 g_methods, NELEM(g_methods));
}

}


