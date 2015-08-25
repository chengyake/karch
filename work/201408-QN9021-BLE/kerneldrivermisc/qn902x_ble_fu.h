/* drivers/misc/qn902x_ble_fu.h
 *
 * Copyright (C) 2014 lonsine
 *
 * Component: qn9021 driver firmwate update
 *
 */

#ifndef _QN902X_BLE_FU_H_
#define _QN902X_BLE_FU_H_

#include <nk/nkern.h>


#define DEBUG_DRV
#define VBLE            0xBE
#define DEVICE_NAME     "vble"
#define FIRMWARE_NAME   "ble_qn902x.bin"
#define FIRMWARE_VER    "ver0.695"   //Consistent with the character of firmware
#define BUFFER_SIZE     300

#ifdef DEBUG_DRV
#define DEBUG(fmt,args...)  printk(KERN_INFO"vble BLE-FE info %d:"fmt,__LINE__, ##args)
#define ERROR(fmt,args...)  printk(KERN_ERR"vble BLE-FE error %d:"fmt,__LINE__, ##args)
#define FUNC_ENTRY()        printk(KERN_INFO "vble BLE-FE: Entering function:%s", __FUNCTION__)
#define FUNC_EXIT()         printk(KERN_INFO "vble BLE-FE: Exiting function:%s ", __FUNCTION__)
#else
#define DEBUG(fmt,args...)
#define ERROR(fmt,args...)  printk(KERN_ERR"vble BLE-FE error:"fmt,##args)
#define FUNC_ENTRY()
#define FUNC_EXIT()
#endif

/* IOCTLs for APPs */
#define IOCTL_CMD_RESET         _IOW(VBLE, 0xB1, void *)
#define IOCTL_CMD_READ_MAC      _IOW(VBLE, 0xB2, unsigned char *)
#define IOCTL_CMD_GET_DEVNAME   _IOW(VBLE, 0xB3, unsigned char *)
#define IOCTL_CMD_SET_DEVNAME   _IOW(VBLE, 0xB4, unsigned char *)
#define IOCTL_CMD_GET_VERSION   _IOW(VBLE, 0xB5, unsigned char *)
#define IOCTL_CMD_QUERY_STATUS  _IOW(VBLE, 0xB6, unsigned char *)
#define IOCTL_CMD_TEST          _IOW(VBLE, 0xB7, unsigned char *)
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

//ancs
#define ANCS_UUID 0xC000


///EACI Message Type
enum
{
    ///Reserved
    EACI_MSG_TYPE_RSV = 0x0,
    ///Command
    EACI_MSG_TYPE_CMD = 0xEA,
    ///Data Request
    EACI_MSG_TYPE_DATA_REQ,
    ///Data Indication
    EACI_MSG_TYPE_DATA_IND,
    ///Event
    EACI_MSG_TYPE_EVT,
    ///Data Error
    EACI_MSG_TYPE_DATA_ERROR = 0xFA,
    ///Number of Message Type
    EACI_MSG_TYPE_MAX
};
///EACI Command
enum
{
    ///Reserved
    EACI_MSG_CMD_RSV = 0x0,         //0
    ///Advertising
    EACI_MSG_CMD_ADV,               //1
    ///Scan
    EACI_MSG_CMD_SCAN,              //2
    ///Connect
    EACI_MSG_CMD_CONN,              //3
    ///Central Disconnect
    EACI_MSG_CMD_DISC,              //4
    ///Peripheral Set Device Name
    EACI_MSG_CMD_SET_DEVNAME = 0x05,//5
    ///Bond
    EACI_MSG_CMD_BOND,              //6
    ///Central Update Param
    EACI_MSG_CMD_CEN_UPDATE_PARAM,  //7
    ///Peripheral Update Param
    EACI_MSG_CMD_PER_UPDATE_PARAM,  //8
 //ls
    EACI_MSG_CMD_GET_MAC = 0x09,    //9

    EACI_MSG_CMD_GET_DEVNAME,       //10    A

    EACI_MSG_CMD_GET_VERSION,       //11    B

    EACI_MSG_CMD_QPPS_SEND_DATA,    //12    C

    //add by chengyake for sensor
    EACI_MSG_CMD_SET_SYS_TIME,      //13    D

    EACI_MSG_CMD_ENABLE_G,          //14    E

    EACI_MSG_CMD_DISABLE_G,         //15    F

    EACI_MSG_CMD_ENABLE_E,          //16    0x10

    EACI_MSG_CMD_DISABLE_E,         //17    0x11

    EACI_MSG_CMD_ENABLE_STEP_DETECT,//18    0x12

    EACI_MSG_CMD_DISABLE_STEP_DETECT,//19   0x13

    EACI_MSG_CMD_ENABLE_STEP_STORE, //20    0x14

    EACI_MSG_CMD_DISABLE_STEP_STORE,//21    0x15

    ///Number of Command
    EACI_MSG_CMD_MAX
};
///EACI Event
enum
{
    ///Reserved
    EACI_MSG_EVT_RSV = 0x0,         //0
    ///Advertising Status
    EACI_MSG_EVT_ADV,               //1
    ///Scan result
    EACI_MSG_EVT_INQ_RESULT,        //2
    ///Scan complete
    EACI_MSG_EVT_INQ_CMP,           //3
    ///Connect Status
    EACI_MSG_EVT_CONN,              //4
    ///Disconnect Status
    EACI_MSG_EVT_DISC = 0x05,       //5
    ///Peripheral Device Name
    EACI_MSG_EVT_SET_DEVNAME,       //6
    ///Bond Status
    EACI_MSG_EVT_BOND,              //7
    ///Central Update param State
    EACI_MSG_EVT_CEN_UPDATE_PARAM,  //8
    ///SMPC Security started indication to host
    EACI_MSG_EVT_SMP_SEC,           //9
    ///Peripheral Update param
    EACI_MSG_EVT_UPDATE_PARAM,      //10    A
//ls
    EACI_MSG_EVT_GET_MAC = 0x0B,    //11    B

    EACI_MSG_EVT_GET_DEVNAME,       //12    C

    EACI_MSG_EVT_GET_VERSION,       //13    D
    ///Connect Status qpps can transfer data
    EACI_MSG_EVT_CONN_QPPS,         //14    E

    EACI_MSG_EVT_QPPS_SEND_DATA,    //15    F

    EACI_MSG_EVT_QPPS_RECV_DATA,    //16    0x10

    EACI_MSG_EVT_GSENSOR_DATA,      //17    0x11

    EACI_MSG_EVT_ESENSOR_DATA,      //18    0x12

    EACI_MSG_EVT_STEP_DATA,         //19    0x13

    EACI_MSG_EVT_SENSOR_TRIM,         //20    0x14
    ///Number of Event
    EACI_MSG_EVT_MAX

};

//
typedef struct {
    unsigned char 	type;
    unsigned char	cmd;
    unsigned char	length;
    unsigned char   data[1];
} eaci_cmd ;

typedef struct {
    unsigned char ble_status;
    unsigned char ble_bond;
    unsigned char ancc_status;
    unsigned char reset_status;
    unsigned char con_addr[6];
} ble_info;


//linux<=>mex
typedef enum {
    CMD_NONE            =0,
    //cmd
    CMD_RESET           =1,
    CMD_ENABLE_BLE      =2,
    CMD_DISABLE_BLE     =3,
    CMD_SEND_DATA       =4,
    CMD_DONE            =5,
    //irq
    CMD_QUERY_IRQ       =6,
    //firmware_updata
    CMD_FU_VERS         =7,
    CMD_FU_BAUD         =8,
    CMD_FU_BURN         =9,
    CMD_SUSPEND         =10,
    CMD_RESUME          =11,
    //max
    CMD_MAX             =12,

} mex_cmd;

//ANCS
enum {
    CMD_ANCC_ENABLE     =1,
    CMD_ANCC_SET_NTFN   =2,
    CMD_ANCC_SET_NTFD   =3,
    CMD_ANCC_GET_MESG   =4,
    CMD_ANCC_GET_DATA   =5,
} ;


typedef enum {
	SLEEP,
	ADVERTISING,
	CONNECT_QPPS,
	CONNECT,
} vble_mode;

typedef enum {
    SUCCESS = 0,
    FAILURE = -1,
    ERROR_OUT_OF_MEMORY = -2,
    ERROR_INVALID_HANDLE = -3,
    ERROR_OUT_OF_RANGE_PARAM = -4,
    ERROR_INVALID_PARAM = -5,
    ERROR_TOO_SMALL_BUF_PARAM = -6,
    ERROR_NOT_SUPPORTED = -7,
    ERROR_TIMEOUT = -8,
    ERROR_WRONG_STATE = -9,
    ERROR_BAD_FORMAT = -10,
    ERROR_INSUFFICIENT_PERMISSIONS = -11,
    ERROR_IO_ERROR = -12,
    ERROR_OUT_OF_HANDLES = -13,
    ERROR_OPERATION_PENDING = -14,
    ERROR_SPECIFIC = -100,
} RESULT;

typedef struct {
    mex_cmd cmd;
    unsigned char buffer[BUFFER_SIZE];
    unsigned int length;
    RESULT  result;
    mex_cmd irq;
    unsigned char irq_buffer[BUFFER_SIZE];
    unsigned int irq_length;
} VRing;

typedef struct {
    NkDevVlink*  vlink;
    NkPhAddr     plink;
    VRing*       vring;
    NkXIrq       s_xirq;
    NkXIrq       c_xirq;
    NkXIrqId     xid;
} fb_info;

typedef struct {
    wait_queue_head_t wait_cmd;
    fb_info *fb_info;
    struct mutex lock;
    struct semaphore  irq_sem;
    struct semaphore  timer_sem;
    struct semaphore  ancc_sem;
    unsigned char ancc_next_type;
    unsigned char ancc_next_cmd;
    struct fasync_struct *async;
} vble_dev;




/*definition the payload format code*/
//definition the head code of a Package
#define HEAD_CODE               0x71    //is "Q" chat assc code
#define CMD_TRY_TIMES           3
#define FLASH_SIZE              (64 * 1024)
#define FLASH_BLOCK_SIZE        (32 * 1024)
#define FLASH_SECTOR_SIZE       (4 * 1024)

#define ERASE_FIRST_ADDR        (4*1024)
#define ERASE_LENGTH            (60*1024)/FLASH_SECTOR_SIZE
#define UART_BUF_SIZE           300     //one byte CMD + 3 bytes data length + number of byes of data + 2 bytes crc
#define TRACE(fmt,args...)      printk(KERN_INFO"vble_fu: %d:"fmt,__LINE__, ##args)




/*ISP commands response code*/
enum ISP_RSP
{
    CMD_CONFIRM_OK = 0x01,  //BL receives a payload successfully
    CMD_CONFIRM_ERR,        //BL receives a payload unsuccessfully
    CMD_EXE_OK,             //BL executes a ISP command successfully
    CMD_EXE_FAIL            //BL executes a ISP command unsuccessfully
};

/*definition ISP commands*/
enum ISP_CMD
{
    B_C_CMD = 0x33,     //Build connection with CPU
    SET_BR_CMD,         //Set CPU UART band rate
    SET_FLASH_CLK_CMD,  //set flash clock
    RD_BL_VER_CMD,       //Read Bootloader version number
    RD_CHIP_ID_CMD,       //Read QN9020 chip ID
    RD_FLASH_ID_CMD,     //Read flash Manufacture ID and Device ID
    SET_APP_LOC_CMD,         //Set ISP application routine location, internal IIRAM or external flash
    SETUP_FLASH_CMD_CMD,    //Setup the flash usual commands
    SET_ST_ADDR_CMD,    //Set the start address of Read,Program,Erase flash
    SET_APP_SIZE_CMD,     //Set the ISP application routine size
    RD_APP_SIZE_CMD,        //read flash application routine size
    SET_APP_CRC_CMD,     //set the ISP application routine crc(16bits)
    RD_APP_CRC_CMD,        //read flash application routine CRC
    SET_APP_IN_FLASH_ADDR_CMD,     //set application in flash address
    RD_APP_IN_FLASH_ADDR_CMD,     //read application in flash address
    SE_FLASH_CMD,       //Sector Erase flash
    BE_FLASH_CMD,       //Block Erase flash
    CE_FLASH_CMD,       //Chip Erase flash
    PROGRAM_CMD,        //Program flash or IRAM
    RD_CMD,             //Read flash or IRAM
    VERIFY_CMD,         //Verify the ISP application routine in flash or in IRAM with CRC
    PROTECT_CMD,        //protect the application routine
    RUN_APP_CMD,        //Run application routine
    REBOOT_CMD,          //reboot bootloader
    WR_RANDOM_DATA_CMD,           //write a random data to bootloader information space
    SET_APP_IN_RAM_ADDR_CMD,  //set app entry address
    SET_APP_RESET_ADDR_CMD
};
enum BLE_MOD
{
    NORMAL,
    INIT_UPDATE
};
enum {
    NOT_WAIT_CMD,
    WAIT_CMD,
};


#define BLE_RETRY_MAX  3
#define BLE_RETRY_DELAY_TIME_5   5 //s
#define BLE_RETRY_DELAY_TIME_10 10 //s
#define BLE_RETRY_DELAY_TIME_20 20 //s
#define BLE_RETRY_DELAY_TIME_30 30 //s

typedef struct {
    struct timer_list   timer;
    unsigned char   type;
    unsigned char   cmd;
    unsigned char   num;
    unsigned char   delay;
} vble_retry;




extern ble_info vble_info;
extern struct wake_lock vble_wake_lock;

extern struct miscdevice misc;
extern vble_dev *vble_devp;
extern unsigned int vble_send_cmd_mex(mex_cmd cmd);
extern unsigned int vble_send_cmd_mex_resp(mex_cmd cmd);

void vble_irq_handler(void);
void printf_string(int len, unsigned char* str);

//firmware update
void get_random_bytes(void *buf, int nbytes);
int  vble_firmware_ioctl(int cmd ,unsigned char * data,int* length);
void ble_firmwate_update(void);

//ancc
int vble_ancc_init(void);
int vble_ancc_enable(void);
int vble_ancc0_socket_send(char* data ,int len);
int vble_ancc1_socket_send(char* data ,int len);
void vble_ancc_receive(eaci_cmd* rec_buf);
int status_socket_send(char* data ,int len);

extern void vble_retry_set(unsigned char type,unsigned char cmd,unsigned char delay);

#endif

