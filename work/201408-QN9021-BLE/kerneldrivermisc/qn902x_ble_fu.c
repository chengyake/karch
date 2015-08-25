/* drivers/misc/qn902x_ble_fu.c
 *
 * Copyright (C) 2014 lonsine
 *
 * Component: qn9021 driver firmwate update
 *
 */

#include <linux/fs.h>
#include <linux/firmware.h>
#include <linux/miscdevice.h>
#include "qn902x_ble_fu.h"

unsigned int  payload_len;
unsigned char payload[UART_BUF_SIZE];

const struct firmware *cust_firmware;

unsigned char g_FlashCmd[8]=
{
    0x05,       //Read Status Register
    0x06,       //Write Enable
    0x20,       //4K Sector Erase
    0x52,       //32K Block Erase
    0x60,       //Chip Erase
    0xB9,       //Deep Power Down
    0xAB,       //Release form Deep Power Down
    0x01,       //Reserved , this active value is from 0x01 to 0xFE
};

const unsigned short Table_CRC[256] =
{
    0X0000L,  0X1021L,  0X2042L,  0X3063L,
    0X4084L,  0X50a5L,  0X60c6L,  0X70e7L,
    0X8108L,  0X9129L,  0Xa14aL,  0Xb16bL,
    0Xc18cL,  0Xd1adL,  0Xe1ceL,  0Xf1efL,
    0X1231L,  0X0210L,  0X3273L,  0X2252L,
    0X52b5L,  0X4294L,  0X72f7L,  0X62d6L,
    0X9339L,  0X8318L,  0Xb37bL,  0Xa35aL,
    0Xd3bdL,  0Xc39cL,  0Xf3ffL,  0Xe3deL,
    0X2462L,  0X3443L,  0X0420L,  0X1401L,
    0X64e6L,  0X74c7L,  0X44a4L,  0X5485L,
    0Xa56aL,  0Xb54bL,  0X8528L,  0X9509L,
    0Xe5eeL,  0Xf5cfL,  0Xc5acL,  0Xd58dL,
    0X3653L,  0X2672L,  0X1611L,  0X0630L,
    0X76d7L,  0X66f6L,  0X5695L,  0X46b4L,
    0Xb75bL,  0Xa77aL,  0X9719L,  0X8738L,
    0Xf7dfL,  0Xe7feL,  0Xd79dL,  0Xc7bcL,
    0X48c4L,  0X58e5L,  0X6886L,  0X78a7L,
    0X0840L,  0X1861L,  0X2802L,  0X3823L,
    0Xc9ccL,  0Xd9edL,  0Xe98eL,  0Xf9afL,
    0X8948L,  0X9969L,  0Xa90aL,  0Xb92bL,
    0X5af5L,  0X4ad4L,  0X7ab7L,  0X6a96L,
    0X1a71L,  0X0a50L,  0X3a33L,  0X2a12L,
    0XdbfdL,  0XcbdcL,  0XfbbfL,  0Xeb9eL,
    0X9b79L,  0X8b58L,  0Xbb3bL,  0Xab1aL,
    0X6ca6L,  0X7c87L,  0X4ce4L,  0X5cc5L,
    0X2c22L,  0X3c03L,  0X0c60L,  0X1c41L,
    0XedaeL,  0Xfd8fL,  0XcdecL,  0XddcdL,
    0Xad2aL,  0Xbd0bL,  0X8d68L,  0X9d49L,
    0X7e97L,  0X6eb6L,  0X5ed5L,  0X4ef4L,
    0X3e13L,  0X2e32L,  0X1e51L,  0X0e70L,
    0Xff9fL,  0XefbeL,  0XdfddL,  0XcffcL,
    0Xbf1bL,  0Xaf3aL,  0X9f59L,  0X8f78L,
    0X9188L,  0X81a9L,  0Xb1caL,  0Xa1ebL,
    0Xd10cL,  0Xc12dL,  0Xf14eL,  0Xe16fL,
    0X1080L,  0X00a1L,  0X30c2L,  0X20e3L,
    0X5004L,  0X4025L,  0X7046L,  0X6067L,
    0X83b9L,  0X9398L,  0Xa3fbL,  0Xb3daL,
    0Xc33dL,  0Xd31cL,  0Xe37fL,  0Xf35eL,
    0X02b1L,  0X1290L,  0X22f3L,  0X32d2L,
    0X4235L,  0X5214L,  0X6277L,  0X7256L,
    0Xb5eaL,  0Xa5cbL,  0X95a8L,  0X8589L,
    0Xf56eL,  0Xe54fL,  0Xd52cL,  0Xc50dL,
    0X34e2L,  0X24c3L,  0X14a0L,  0X0481L,
    0X7466L,  0X6447L,  0X5424L,  0X4405L,
    0Xa7dbL,  0Xb7faL,  0X8799L,  0X97b8L,
    0Xe75fL,  0Xf77eL,  0Xc71dL,  0Xd73cL,
    0X26d3L,  0X36f2L,  0X0691L,  0X16b0L,
    0X6657L,  0X7676L,  0X4615L,  0X5634L,
    0Xd94cL,  0Xc96dL,  0Xf90eL,  0Xe92fL,
    0X99c8L,  0X89e9L,  0Xb98aL,  0Xa9abL,
    0X5844L,  0X4865L,  0X7806L,  0X6827L,
    0X18c0L,  0X08e1L,  0X3882L,  0X28a3L,
    0Xcb7dL,  0Xdb5cL,  0Xeb3fL,  0Xfb1eL,
    0X8bf9L,  0X9bd8L,  0XabbbL,  0Xbb9aL,
    0X4a75L,  0X5a54L,  0X6a37L,  0X7a16L,
    0X0af1L,  0X1ad0L,  0X2ab3L,  0X3a92L,
    0Xfd2eL,  0Xed0fL,  0Xdd6cL,  0Xcd4dL,
    0XbdaaL,  0Xad8bL,  0X9de8L,  0X8dc9L,
    0X7c26L,  0X6c07L,  0X5c64L,  0X4c45L,
    0X3ca2L,  0X2c83L,  0X1ce0L,  0X0cc1L,
    0Xef1fL,  0Xff3eL,  0Xcf5dL,  0Xdf7cL,
    0Xaf9bL,  0XbfbaL,  0X8fd9L,  0X9ff8L,
    0X6e17L,  0X7e36L,  0X4e55L,  0X5e74L,
    0X2e93L,  0X3eb2L,  0X0ed1L,  0X1ef0L
};

static unsigned short CRC16( unsigned char * aData, unsigned int aSize )
{
    unsigned long  i;
    unsigned short nAccum = 0;

    for ( i = 0; i < aSize; i++ )
        nAccum = ( nAccum << 8 ) ^ ( unsigned short )Table_CRC[( nAccum >> 8 ) ^ *aData++];
    return nAccum;
}

static unsigned char CommTxPayload(unsigned char cmd, unsigned int data,unsigned char *pBuf, unsigned int payloadLen, int tryFlag)
{
    unsigned int index,daLen,crc,tryCnt,ret;

    /*TX the head code*/
    index = 0;
    payload[index++] = HEAD_CODE;
    /*TX CMD code*/
    payload[index++] = cmd;
    /*TX the Payload length*/
    daLen = payloadLen;
    payload[index++] = (unsigned char)daLen;    //low byte
    payload[index++] = (unsigned char)(daLen >> 8);//middle byte
    payload[index++] = (unsigned char)(daLen >> 16);//high byte
    /*TX data bytes*/
    if(data != 0)
    {
        payload[index++] = (unsigned char)data; // low byte
        payload[index++] = (unsigned char)(data >> 8); // middle1 byte
        payload[index++] = (unsigned char)(data >> 16); // middle1 byte
        payload[index++] = (unsigned char)(data >> 24); // high byte
    }
    else
    {
        while(daLen)
        {
            payload[index++] = *pBuf++;
            daLen--;
        }
    }
    /*crc a payload*/
    crc = CRC16(&payload[1], index-1);
    payload[index++] = (unsigned char)crc;  //crc low byte
    payload[index++] = (unsigned char)(crc >> 8);//crc high byte
    /*flag*/
    payload[index++] = (unsigned char)tryFlag;

    tryCnt = 0;
    while (tryCnt < CMD_TRY_TIMES)
    {
        payload_len = index;
        ret = vble_firmware_ioctl(CMD_FU_BURN,&payload[0],&payload_len);
        if (ret == 0)
        {
            return payload[0];
        }
        tryCnt++;
    }
    return 0;
}


static bool devices_upgrade_start(void)
{

    //unsigned int tryCnt;
    unsigned int nPage, nByte, i;
    unsigned int m_random;
    unsigned int m_crc;
    unsigned int startAddr;
    unsigned int ver;

    TRACE("flash_programming start!");

    //check version
    if(0 != vble_firmware_ioctl(CMD_FU_VERS,&payload[0],&payload_len))
    {
        TRACE("Get device version Fail!");
        return false;
    }
    TRACE("Get device version Success!");

    TRACE("Device version: %s \n", &payload[0]);
    TRACE("Firmware version: %s \n", FIRMWARE_VER);
    if(memcmp (&FIRMWARE_VER, &payload[0], sizeof(FIRMWARE_VER)-1) == 0)
    {
        TRACE("version of the same \n");
        return false;
    }
    TRACE("different version \n");

    //check file
    TRACE("cust_firmware->size = %d \n",cust_firmware->size);
    if (cust_firmware->size % 4)
    {
        TRACE("Application file length is not 4 integer times, Please check it!");
        return false;
    }

    if (cust_firmware->size > 49920)//49920=48.75 * 1024
    {
        TRACE("Application file size is out of range!");
        return false;
    }

    if (cust_firmware->size < 4)
    {
        TRACE("Application file size is small than 4 bytes!");
        return false;
    }

    if(0 != vble_firmware_ioctl(CMD_FU_BAUD,&payload[0],&payload_len))
    {
        TRACE("Build connection with CPU Fail!");
        return false;
    }
    TRACE("Build connection with CPU Success!");

    //read bootloader version
    if (CMD_CONFIRM_OK != CommTxPayload(RD_BL_VER_CMD, 0, NULL, 0, NOT_WAIT_CMD))
    {
        TRACE("read bootloader version Fail!");
        return false;
    }
    else
    {
        if(payload[1] == HEAD_CODE)
        {
            /*
            for(i=0;i<payload[3];i++)
            {
                TRACE("payload[%d]=%x\n",i,payload[6+i]);
            }
            */
            ver = payload[6];
            ver |= (unsigned int)payload[7] << 8;
            ver |= (unsigned int)payload[8] << 16;
            ver |= (unsigned int)payload[9] << 24;
            TRACE("Device ver is 0x%08x\n", ver);
        }
    }
    TRACE("read bootloader version Success!");


    //set app location in flash
    if (!CommTxPayload(SET_APP_LOC_CMD, 1, NULL, 4, NOT_WAIT_CMD))
    {
        return false;
    }
    TRACE("set app location in flash Success!");

    //Set flash program start address
    if (CMD_CONFIRM_OK != CommTxPayload(SET_ST_ADDR_CMD, ERASE_FIRST_ADDR, NULL, 4, NOT_WAIT_CMD))
    {
        return false;
    }
    TRACE("Set flash program start address %d Success!",ERASE_FIRST_ADDR);

    /*erasing 60K except 4K NVDS*/
    if (CMD_EXE_OK != CommTxPayload(SE_FLASH_CMD, ERASE_LENGTH, NULL, 4, WAIT_CMD))
    {
        TRACE("cmd erasing 60K except 4K NVDS fail!\n");
        return false;
    }
    TRACE("cmd erasing 60K except 4K NVDS %d Success!",ERASE_LENGTH);


    //write random data to BL
    get_random_bytes(&m_random,4);
    if (CMD_EXE_OK != CommTxPayload(WR_RANDOM_DATA_CMD, m_random, NULL, 4, WAIT_CMD))
    {
        TRACE("write random data to BL fail!\n");
        return false;
    }
    TRACE("Write Random Data 0x%08x Success! \n", m_random);


    /*set flash clock and save it to flash*/
    if (CMD_EXE_OK != CommTxPayload(SET_FLASH_CLK_CMD, 8000000, NULL, 4, WAIT_CMD))
    {
        TRACE("Write flash clock 8000000Hz fail!\n");
        return false;
    }
    TRACE("Write flash clock 8000000Hz Success! \n");



    /*writing flash commands after erasing flash success*/
    if (CMD_EXE_OK != CommTxPayload(SETUP_FLASH_CMD_CMD, 0, g_FlashCmd, sizeof(g_FlashCmd), WAIT_CMD))
    {
        TRACE("Write Flash Commands fail!\n");
        return false;
    }
    TRACE("Write Flash Commands Success! \n");


    /*set flash the address of starting program */
    if (CMD_EXE_OK != CommTxPayload(SET_APP_IN_RAM_ADDR_CMD, 0x10000000, NULL, 4, WAIT_CMD))
    {
        TRACE("set app entry address fail!\n");
        return false;
    }
    TRACE("set app entry address Success! \n");


    /*set flash the address of starting program */
    if (CMD_EXE_OK != CommTxPayload(SET_APP_RESET_ADDR_CMD, 0x100000d4, NULL, 4, WAIT_CMD))
    {
        TRACE("set app reset address failed!\n");
        return false;
    }
    TRACE("set app reset address success! \n");


    /*set app in flash address*/
    if (CMD_EXE_OK != CommTxPayload(SET_APP_IN_FLASH_ADDR_CMD, 0x00001100, NULL, 4, WAIT_CMD))
    {
        TRACE("set app in flash address failed!\n");
        return false;
    }
    TRACE("set app in flash address Success! \n");



    //Set flash program start address
    if (CMD_CONFIRM_OK != CommTxPayload(SET_ST_ADDR_CMD, 0x00001100, NULL, 4, NOT_WAIT_CMD))
    {
        return false;
    }
    TRACE("Set flash program start address 0x00001100 Success! \n");

    /* set size of app*/
    if (CMD_EXE_OK != CommTxPayload(SET_APP_SIZE_CMD, cust_firmware->size, NULL, 4, WAIT_CMD))
    {
        TRACE("Set App Size fail!\n");
        return false;
    }
    TRACE("Set App Size Success! \n");


    /*set App's crc*/
    m_crc = CRC16((unsigned char*)cust_firmware->data, cust_firmware->size);
    TRACE("Set file crc is 0x%08x\n", m_crc);
    if (CMD_EXE_OK != CommTxPayload(SET_APP_CRC_CMD, m_crc, NULL, 4, WAIT_CMD))
    {
        TRACE("Set App CRC fail!\n");
        return false;
    }
    TRACE("Set App CRC %d Success!\n",m_crc);


    /*start programming flash*/
    nPage = cust_firmware->size/256;
    nByte = cust_firmware->size%256;
    i = 0;
    startAddr = 0x00001100;
    while (nPage)
    {
        TRACE("Programming %d Page.........................\n", i+1);
        //program 256 bytes
        if (CMD_EXE_OK != CommTxPayload(PROGRAM_CMD, 0, (unsigned char*)cust_firmware->data + i*256,256, WAIT_CMD))
        {
            TRACE("Page Program Failed!\n");
            return false;
        }


        i++;
        startAddr += 256;
        TRACE("Page Program success!\n" );

        //Set flash program start address
        if (CMD_CONFIRM_OK != CommTxPayload(SET_ST_ADDR_CMD, startAddr, NULL, 4, NOT_WAIT_CMD))
        {
          return false;
        }
        nPage--;
    }

    //program the remainder bytes
    if(nByte > 0)
    {
        TRACE("Programming remained bytes %d.........................\n", nByte);
        //program remained bytes
        if (CMD_EXE_OK != CommTxPayload(PROGRAM_CMD, 0, (unsigned char*)cust_firmware->data + i*256, nByte, WAIT_CMD))
        {
            TRACE("Programming remained bytes Failed!\n");
            return false;
        }
        TRACE("Programming remained bytes success!\n");

    }

    //verify ISP code
    if (CMD_EXE_OK != CommTxPayload(VERIFY_CMD, 0, 0, 0, WAIT_CMD))
    {
        TRACE("Verify Program Failed!\n");
        return false;
    }
    TRACE("Verify Program Success!\n");

    /*reboot mcu*/
    if (CMD_CONFIRM_OK != CommTxPayload(REBOOT_CMD, 0, NULL, 0, NOT_WAIT_CMD))
    {
        TRACE("Reboot mcu Failed!\n");
        return false;
    }
    TRACE("Reboot mcu Success!\n");

    return true;

}


void ble_firmwate_update(void)
{
    int ret;
    int num = 3;

    TRACE("test: firmware_test\n");
    //return;
    ret = request_firmware(&cust_firmware, FIRMWARE_NAME, misc.this_device);
    if (ret < 0)
    {
        TRACE("test: ble_qn902x.bin request firmware failed(%d)\n",ret);
    }
    else
    {
        /* check and start upgrade */
        while((devices_upgrade_start() == false) && (num > 0))
        {
            num--;
            TRACE("test: devices_upgrade_start retry number %d\n", 3-num);
        }

        release_firmware(cust_firmware);
        TRACE("test: release_firmware\n");
    }
}


MODULE_AUTHOR("lonshinetech");
MODULE_LICENSE("GPL");
