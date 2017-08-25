#pragma once

//探头类型，和"ScanProcessor.java"内定义保持一致
#define TYPE_LINE 1		//线阵探头
#define TYPE_CURVE 2	//凸阵探头
#define TYPE_MICRO_CURVE 3	//微凸探头
#define TYPE_INTRACAVITY 4	//腔内探头
#define TYPE_LINE_H 5		//高频线阵探头

/*!
* @函数说明	初始化图像处理的各种参数；
* @传入参数             type:	探头类型；
* @返回参数             TRUE：   配置成功； FALSE：配置失败；
*/
bool initParam(int type);


/*!
* @函数说明	选择不同的检查场景，如小器官，肌肉，血管。 
			线阵：8M，小器官；7M，肌肉；7M，血管；10M，浅表，只对应3cm
* @传入参数             index：不同的临床优化方案对应的索引值：线阵：0，小器官 1，肌肉，2。血管   3 浅表；
                                                            线阵：0，颈动脉 1，静脉，2。浅表血管
                                                            凸阵：0－通用，1－产科
                                                            微凸：0 通用 1 胸腹 2 心脏
* @返回参数             TRUE：   配置成功； FALSE：配置失败；
*/
bool setSolution(int index);


/*!
* @函数说明	初始化硬件工作的各种参数和表格；
* @传入参数             NULL；
* @返回参数             TRUE：   配置成功； FALSE：配置失败；
*/
bool initSystem();

/*!
* @函数说明	初始化彩超硬件工作的各种参数和表格；
* @传入参数             NULL；
* @返回参数             TRUE：   配置成功； FALSE：配置失败；
*/
bool initSystem_CFM();


/*!
* @函数说明	获取USB设备文件路径，以便应用程序获取Root权限；
* @传入参数            vid－设备的VID，pid－设备的PID，pbuffer－存储USB设备路径的指针；
* @返回参数             TRUE：   获取成功； FALSE：获取失败；
*/
bool getUsbDevicePath(unsigned short vid,unsigned short pid,char *pBuffer);


/*!
* @函数说明	配置超声硬件设备，处理流程包括下载FPGA代码，初始化硬件正常工作所需的参数，设备置为工作状态；
* @传入参数             pData：FPGA二进制配置文件内容； Filename：FPGA二进制配置文件长度；
* @返回参数             TRUE：   配置成功； FALSE：配置失败；
*/
bool configDevice(unsigned char *pData, unsigned long ulLen);

/*!
* @函数说明	AS模式下载，处理流程包括下载FPGA代码，初始化硬件正常工作所需的参数，设备置为工作状态；
* @传入参数             pData：FPGA二进制配置文件内容； Filename：FPGA二进制配置文件长度；nEPCS_ID：EPCS号码
* @返回参数             TRUE：   配置成功； FALSE：配置失败；
*/
bool download2epcs(unsigned char *pData, unsigned long ulLen, unsigned char nEPCS_ID);

/*!
* @函数说明	AS模式启动，处理流程包括初始化硬件正常工作所需的参数，设备置为工作状态；
* @传入参数             nEPCS_ID：EPCS号码
* @返回参数             TRUE：   配置成功； FALSE：配置失败；
*/
bool configDeviceAS(unsigned char nEPCS_ID);

/*!
* @函数说明	获取原始的图像数据；
* @传入参数             NULL；
* @返回参数             采集的256*512个原始图像数据，；
*/
unsigned short* getImageData(int &nRow, int &nColumn , unsigned long &ulFrameNum);

/*!
* @函数说明	M超获取原始的图像数据；
* @传入参数             NULL；
* @返回参数             采集的256*512个原始图像数据，；
*/
bool getImageData(unsigned short* pBData, unsigned short* pMData, int &nRow, int &nColumn , unsigned long &ulFrameNum);

/*!
* @函数说明	设置M线；
* @传入参数             iLine－M线；
* @返回参数             NULL；
*/
bool setMScanLine(int iLine);

/*!
* @函数说明	对采集的原始数据进行图像处理；
* @传入参数             pDataDst－处理后的图像数据，nDstWidth－处理后的图像宽度，nDstHeight－处理后的图像高度，pSrcData－处理前的图像数据，nSrcWidth－处理前的图像宽度，nSrcHeight－处理前的图像高度；
* @返回参数             NULL；
*/
bool ProcessM(unsigned char *pDataDst, int nDstWidth, int nDstHeight, unsigned short *pSrcData, int nSrcWidth, int nSrcHeight);

/*!
* @函数说明	对采集的原始数据进行图像处理；
* @传入参数             pDataDst－处理后的图像数据，nDstWidth－处理后的图像宽度，nDstHeight－处理后的图像高度，pSrcData－处理前的图像数据，nSrcWidth－处理前的图像宽度，nSrcHeight－处理前的图像高度；
* @返回参数             NULL；
*/
void Process(unsigned char *pDataDst, int nDstWidth, int nDstHeight, unsigned short *pSrcData, int nSrcWidth, int nSrcHeight);

/*!
* @函数说明	对采集的原始数据进行图像处理；
* @传入参数             pDataDst－处理后的图像数据，nDstWidth－处理后的图像宽度，nDstHeight－处理后的图像高度，pSrcData－处理前的图像数据，nSrcWidth－处理前的图像宽度，nSrcHeight－处理前的图像高度；
* @返回参数             NULL；
*/
void ProcessCfm(unsigned char *pDataDst, int nDstWidth, int nDstHeight, short *pSrcData, int nSrcWidth, int nSrcHeight);

/*!
* @函数说明	设置设备工作状态，运行或者冻结；
* @传入参数             bRun：TURE－运行，FALSE－冻结；
* @返回参数             TRUE：   配置成功； FALSE：配置失败；
*/
bool setRun(bool bRun);

/*!
* @函数说明	获取设备工作状态，运行或者冻结；
* @传入参数             NULL；
* @返回参数             TRUE：   运行； FALSE：冻结；
*/
bool isRun();


/*!
* @函数说明	设置不同的工作频率；
* @传入参数             index：不同的工作频率对应的索引值:线阵： 0－6M, 1-7M, 2-8M, 3-10M；腔内：0-6M,1-6.5M,2-7M,3-8M
                                16通道线阵：0－ 6.5, 1－7.2, 2－8.0, 3－8.8
* @返回参数             TRUE：   配置成功； FALSE：配置失败；
*/
bool setFreq(int index);


/*!
* @函数说明	设置不同的工作频率；
* @传入参数             index：不同的工作频率对应的索引值:线阵： 0－6M, 1-7M, 2-8M, 3-10M；腔内：0-6M,1-6.5M,2-7M,3-8M
                        cfmindex:B模的时候设成0就可以了
                                 彩色的时候
                                 小器官－0，血管－2，浅表-3
                                 彩色每次模式切换的时候都调下这个接口
* @返回参数             TRUE：   配置成功； FALSE：配置失败；
*/
bool setFreq(int index, int cfmindex);


/*!
* @函数说明	获取当前的工作频率；
* @传入参数             不同的工作频率对应的索引值,同setFreq；
* @返回参数             当前设备的工作频率；；
*/
double getFreq(int index);


/*!
* @函数说明	设置不同的扫描深度；
* @传入参数             index：不同的工作频率对应的索引值，例如：线阵、腔内：0－30mm，1－40mm，2－50mm，3－60mm；凸阵、微凸：0－140mm，1－160mm，2－180mm；
* @返回参数             TRUE：   配置成功； FALSE：配置失败；
*/
bool setDepth(int index);


/*!
* @函数说明	获取当前的扫描深度；
* @传入参数             NULL；
* @返回参数             当前设备的扫描深度；
*/
int getDepth();


/*!
* @函数说明	设置当前增益；
* @传入参数             nGain：不同的增益值，增益的取值范围0~100；
* @返回参数             TRUE：   设置成功； FALSE：设置失败；
*/
bool setGain(int nGain);


/*!
* @函数说明	获取当前的增益；
* @传入参数             NULL；
* @返回参数             当前的增益值，增益的取值范围0~100；
*/
int getGain();


/*!
* @函数说明	获取每MM显示的像素点数；
* @传入参数             NULL；
* @返回参数             每mm显示的像素点数；
*/
double getPixelPerMM();

/*!
* @函数说明	获取dsc坐标转换后对应的x坐标；
* @传入参数             nLineX:线位置；nDotY：点位置；nWidth：位图宽度；nHeight：位图高度；nOffsetX：位图在x方向的偏移
* @返回参数             返回dsc转换后对应的x坐标；
*/
int GetDscPointX (int nLineX, int nDotY, int nWidth, int nHeight, int nOffsetX);

/*!
* @函数说明	获取dsc坐标转换后对应的y坐标；
* @传入参数             nLineX:线位置；nDotY：点位置；nWidth：位图宽度；nHeight：位图高度；nOffsetY：位图在y方向的偏移
* @返回参数             返回dsc转换后对应的y坐标；
*/
int GetDscPointY (int nLineX, int nDotY, int nWidth, int nHeight, int nOffsetY);

/*!
* @函数说明	roi位置发生变化时设置CFM的扫描参数；
* @传入参数             iStartPos:起始线位置；iEndPos：结束线位置；nRepeat：重复次数；nLineSpace：扫描线间距
* @返回参数              TRUE：   设置成功； FALSE：设置失败；
*/
bool setCfmScan(int iStartPos, int iEndPos, int nRepeat, int nLineSpace);

/*!
* @函数说明	roi位置发生变化时设置CFM扫描起始和结束深度；
* @传入参数             nRoiTop:起始点位置；nRoiBtm：结束点位置；
* @返回参数              TRUE：   设置成功； FALSE：设置失败；
*/
bool setCfmDepth(int nRoiTop, int nRoiBtm);

/*!
* @函数说明	设置CFM的PRF调节参数；
* @传入参数             nDepthIndex:不同的工作频率对应的索引值 nPrf:PRF索引；
* @返回参数              TRUE：   设置成功； FALSE：设置失败；
*/
bool setCfmPrf(int nDepthIndex, int nIndex);

/*!
* @函数说明	获取CFM的PRF调节参数；
* @传入参数             nDepthIndex:不同的工作频率对应的索引值 nIndex:PRF索引；
* @返回参数              PRF值；
*/
int getCfmPrf(int nDepthIndex, int nIndex);

/*!
* @函数说明	设置CFM的WF调节参数；
* @传入参数             nDepthIndex:不同的工作频率对应的索引值 nPrf:WF索引；
* @返回参数              TRUE：   设置成功； FALSE：设置失败；
*/
bool SetWallFilter(int nDepthIndex, int nIndex);

/*!
* @函数说明	获得CFM的WF调节参数；
* @传入参数
* @返回参数             WF调节参数；
*/
int getWallFilter();

/*!
* @函数说明	设置当前工作模式；
* @传入参数             bBmode: TRUE：   B mode； FALSE：CFM mode；
* @返回参数              TRUE：   设置成功； FALSE：设置失败；
*/
bool setScanMode(bool bBmode);

/*!
* @函数说明	获取当前工作模式；
* @传入参数             NULL；
* @返回参数              TRUE：  B mode； FALSE：CFM mode；
*/
bool IsBmode();

/*!
* @函数说明	设置CFM工作参数；
* @传入参数             NULL
* @返回参数             TRUE：   设置成功； FALSE：设置失败；
*/
bool SetCfmInfo();

/*!
* @函数说明	设置当前CFM增益；
* @传入参数             nGain：不同的增益值，增益的取值范围0~100；
* @返回参数             TRUE：   设置成功； FALSE：设置失败；
*/
bool SetCfmGain(int nGain);

/*!
* @函数说明	获取当前CFM的增益；
* @传入参数             NULL；
* @返回参数             当前的增益值，增益的取值范围0~100；
*/
int GetCfmGain();

/*!
* @函数说明	FPGA风扇温度控制；
* @传入参数           nTemp：FPGA风扇温度控制参数；一个bit代表0.5摄氏度
* @返回参数            TRUE：   设置成功； FALSE：设置失败；
*/
bool setFanCtrl(unsigned char nTemp);

/*!
* @函数说明	获得帧号；
* @传入参数
* @返回参数
*/
unsigned long GetFrameNum();

/*!
* @函数说明	获得帧时间戳；
* @传入参数
* @返回参数
*/
unsigned long GetFrameTime();

/*!
* @函数说明	图像优化等级调节；
* @传入参数
* @返回参数
*/
int SetProcMode(int nIndex);

/*!
* @函数说明	获取灰度表；
* @传入参数
* @返回参数
*/
unsigned char* GetGammaLUT();

/*!
* @函数说明	硬件调节电压；
* @传入参数
* @返回参数
*/
bool SetVoltage();

void setMMode(bool enable);

void getPresetFunc(void *);
bool setPresetInt(int, int);
bool setPresetDouble(int, double);
int getPresetInt(int);
double getPresetDouble(int);

//RF采集接口
bool setParam(unsigned char nParam);
bool setRfMode(bool bRf);
void enableRfCapture();
