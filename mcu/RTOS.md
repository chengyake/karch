# 嵌入式系统评估：

#### 参考原则：
1. 时间成本，学习及开发周期
2. 可移植性，已经兼容多少mcu，易于移植到哪些mcu
3. 可靠性， 运行稳定性，优化可靠性
4. 实时性， 任务切换，抢断，中断响应等
5. 功能性， 支持多少类型的模块扩展，如FS，TCP等  
6. 工具链， 包括开发编码，调试，编译，部署，升级等
7. 支持度， 免费的要考虑开放资料多少





#### 表格对比：


类型 | RTX | uc/os II | FreeRTOS | embOS | uc/os III | RTEMs | eCOS
---|---|---|---|---|---|---|---
RAM:| 128-300B | 4KB | 2KB | 50B | 4KB | 4KB | 10KB
FLASH:| <4kB | 10-26kB | 6-10KB | 1k+ | 10K+ | 30KB | 30KB
>- 以上数据仅供参考，并未考证。
>- air200的底层os是SXR，商业的。没找到相关数据


#### 实时性：
- 这里提供一组实时性测试方面的数据，通过任务主动释放CPU权利来测试任务的切换速度
- 测试条件 ：STM32F103VET6，Cortex-M3内核，72Mhz，

os | version | Hz
---|---|---
RTX1 | V4.5 | 252
uCOS-II | V2.92.07 | 354
embOS | V3.86 | 389
FreeRTOS | V7.4.2 | 514\|374
uCOS-III | V3.03.01 | 576





#### 备注：
>2017-03-24 yake 数据多是网络搜集




