# 思路


---
#### 总的设计原则：
1. 逻辑与构架分离
2. 使用简单的脚本语言开发应用
3. 脚本文件式的调试，部署，升级
3. shell交互式调试代码
4. 断点、单步等尽量支持
5. crash dump
6. 若能可选地提供lua->c的转换，并部署二进制更好



---
#### 想法收集：
- 类似android,但底层os可不拘泥，符合API即可。
- 使用CIT(Customer Interface Test)方式检验API规范
- CIT two level： LUA层 LNI层（LUA Native Interface）
- CIT可内建到shell中，作为一个命令存在；执行则执行并打印结果
- 学校推广：电子设计比赛和毕业设计
- 提供3-5中开发板硬件平台，及各种外设模块

