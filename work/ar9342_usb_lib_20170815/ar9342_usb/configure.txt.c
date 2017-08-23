       image = 0        usb port 6
controlparam = 1        usb port 0
   tabledata = 2
   framehead = 3        usb port 6
   probecode = 4        NC
       port5 = 5        NC
     chip_rw = 6
       port7 = 7        NC



//1、初始化AFE芯片
//参数下发USB端口号：6
//依次下发CHIP文件夹中的1-17个表格，表格中的数据为BYTE(8位)型，
//每次下发首先进行端口写使能，再下发表格。
//
//windows参考代码：
static const byte write_enable_command	= 0xb2;	// 写使能
bool ccyusbbase::write(byte nport, const byte* pbuffer, ulong ulcount)
{
    if(pbuffer == null || ulcount <= 0 || m_pusbdev==null) return false;
    bool bres = false;
    ::entercriticalsection(&m_cs);	
    if(this->writecommand(write_enable_command, nport))
    {
	bres = this->writedata(pbuffer, ulcount);
    }	
    ::leavecriticalsection(&m_cs);
    return bres;
}





//2、初始化FPGA
//参数下发USB端口号：2
//依次下发FPGA文件夹中的10个表格，表格中的数据为BYTE(8位)型,过USB端口1，
//首先通过USB端口1，把表格下发的寄存器地址和ID写下去，
//参考代码：
BOOL CCyUsbBase::Write(BYTE nPort, const BYTE* pBuffer, ULONG ulCount)
{
	if(pBuffer == NULL || ulCount <= 0 || m_pUsbDev==NULL) return FALSE;
	BOOL bRes = FALSE;
	::EnterCriticalSection(&m_cs);	
	if(this->WriteCommand(WRITE_ENABLE_COMMAND, nPort))
	{
		bRes = this->WriteData(pBuffer, ulCount);
	}	
	::LeaveCriticalSection(&m_cs);
	return bRes;
}
//其中nPort为1，pBuffer为把表格下发的寄存器地址和ID（共占两个BYTE），寄存器地址0x06，表ID为配置文件的后缀，
//如‘W_FPGA_2.txt’表ID为2。
//下发完成后，再通过端口2下发表格，参考代码跟上述一样。





//3、运行
//FPGA配置完成后，设置运行模式
//通过USB端口1，把命令下发的寄存器地址和指令写下去，
//参考代码：
BOOL CCyUsbBase::Write(BYTE nPort, const BYTE* pBuffer, ULONG ulCount)
{
	if(pBuffer == NULL || ulCount <= 0 || m_pUsbDev==NULL) return FALSE;
	BOOL bRes = FALSE;
	::EnterCriticalSection(&m_cs);	
	if(this->WriteCommand(WRITE_ENABLE_COMMAND, nPort))
	{
		bRes = this->WriteData(pBuffer, ulCount);
	}	
	::LeaveCriticalSection(&m_cs);
	return bRes;
}

//其中nPort为1，pBuffer为把表格下发的寄存器地址和指令（共占两个BYTE），寄存器地址0x07，指令运行为0，冻结为1


//4、采集数据
//图像数据通过USB端口0读取，首先要对端口进行读使能，然后再进行数据读取
static const BYTE WRITE_ENABLE_COMMAND	= 0xB3;	// 读使能
this->WriteCommand(READ_ENABLE_COMMAND, nPort)


帧头第五个字节

Bus 001 Device 004: ID 04b4:8613 Cypress Semiconductor Corp. CY7C68013 EZ-USB FX2 USB 2.0 Development Kit
Couldn't open device, some information will be missing
Device Descriptor:
  bLength                18
  bDescriptorType         1
  bcdUSB               2.00
  bDeviceClass            0 (Defined at Interface level)
  bDeviceSubClass         0 
  bDeviceProtocol         0 
  bMaxPacketSize0        64
  idVendor           0x04b4 Cypress Semiconductor Corp.
  idProduct          0x8613 CY7C68013 EZ-USB FX2 USB 2.0 Development Kit
  bcdDevice            0.00
  iManufacturer           1 
  iProduct                2 
  iSerial                 0 
  bNumConfigurations      1
  Configuration Descriptor:
    bLength                 9
    bDescriptorType         2
    wTotalLength           46
    bNumInterfaces          1
    bConfigurationValue     1
    iConfiguration          0 
    bmAttributes         0x80
      (Bus Powered)
    MaxPower              100mA
    Interface Descriptor:
      bLength                 9
      bDescriptorType         4
      bInterfaceNumber        0
      bAlternateSetting       0
      bNumEndpoints           4
      bInterfaceClass       255 Vendor Specific Class
      bInterfaceSubClass      0 
      bInterfaceProtocol      0 
      iInterface              0 
      Endpoint Descriptor:
        bLength                 7
        bDescriptorType         5
        bEndpointAddress     0x02  EP 2 OUT
        bmAttributes            2
          Transfer Type            Bulk
          Synch Type               None
          Usage Type               Data
        wMaxPacketSize     0x0200  1x 512 bytes
        bInterval               0
      Endpoint Descriptor:
        bLength                 7
        bDescriptorType         5
        bEndpointAddress     0x04  EP 4 OUT
        bmAttributes            2
          Transfer Type            Bulk
          Synch Type               None
          Usage Type               Data
        wMaxPacketSize     0x0200  1x 512 bytes
        bInterval               0
      Endpoint Descriptor:
        bLength                 7
        bDescriptorType         5
        bEndpointAddress     0x86  EP 6 IN
        bmAttributes            2
          Transfer Type            Bulk
          Synch Type               None
          Usage Type               Data
        wMaxPacketSize     0x0200  1x 512 bytes
        bInterval               0
      Endpoint Descriptor:
        bLength                 7
        bDescriptorType         5
        bEndpointAddress     0x88  EP 8 IN
        bmAttributes            2
          Transfer Type            Bulk
          Synch Type               None
          Usage Type               Data
        wMaxPacketSize     0x0200  1x 512 bytes
        bInterval               0












