       image = 0        usb port 6
controlparam = 1        usb port 0
   tabledata = 2
   framehead = 3        usb port 6
   probecode = 4        NC
       port5 = 5        NC
     chip_rw = 6
       port7 = 7        NC



//1����ʼ��AFEоƬ
//�����·�USB�˿ںţ�6
//�����·�CHIP�ļ����е�1-17����񣬱���е�����ΪBYTE(8λ)�ͣ�
//ÿ���·����Ƚ��ж˿�дʹ�ܣ����·����
//
//windows�ο����룺
static const byte write_enable_command	= 0xb2;	// дʹ��
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





//2����ʼ��FPGA
//�����·�USB�˿ںţ�2
//�����·�FPGA�ļ����е�10����񣬱���е�����ΪBYTE(8λ)��,��USB�˿�1��
//����ͨ��USB�˿�1���ѱ���·��ļĴ�����ַ��IDд��ȥ��
//�ο����룺
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
//����nPortΪ1��pBufferΪ�ѱ���·��ļĴ�����ַ��ID����ռ����BYTE�����Ĵ�����ַ0x06����IDΪ�����ļ��ĺ�׺��
//�确W_FPGA_2.txt����IDΪ2��
//�·���ɺ���ͨ���˿�2�·���񣬲ο����������һ����





//3������
//FPGA������ɺ���������ģʽ
//ͨ��USB�˿�1���������·��ļĴ�����ַ��ָ��д��ȥ��
//�ο����룺
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

//����nPortΪ1��pBufferΪ�ѱ���·��ļĴ�����ַ��ָ���ռ����BYTE�����Ĵ�����ַ0x07��ָ������Ϊ0������Ϊ1


//4���ɼ�����
//ͼ������ͨ��USB�˿�0��ȡ������Ҫ�Զ˿ڽ��ж�ʹ�ܣ�Ȼ���ٽ������ݶ�ȡ
static const BYTE WRITE_ENABLE_COMMAND	= 0xB3;	// ��ʹ��
this->WriteCommand(READ_ENABLE_COMMAND, nPort)


֡ͷ������ֽ�

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












