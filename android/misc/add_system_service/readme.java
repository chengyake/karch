Java层服务顾名思义即为从 Java层提供的服务，它与 C++层的服务不同之处在于其服务进程的运行由系统（ ServiceManager）统一维护。在文件 frameworks /base /services /java /com /android /server /SystemServer.java 中我们可以看到以下代码：

AlarmManagerService  alarm = new AlarmManagerService (context );
ServiceManager .addService (Context .ALARM_SERVICE , alarm );



这个操作在系统启动时完成，由 ServiceManager 负责创建服务进程并运行之。所以我们要做的事就是实现一个 java 服务，并将其添加到这里并由系统运行起来，以下是具体实现步骤：



实现自己的 java层 service

1.       在目录

frameworks/base/core/java/android/os中增加自己的 AIDL文件用来申明服务：
BelyService.aidl:
package android.os;
interface IBelyService {

    int calculateSqu(int value);

}

2.       在目录
frameworks/base/services/java/com/android/server增加 service的实现文件：
BelyService.java:
package com.android.server;
import android.content.Context;
import android.os.IBelyService;
public class BelyService extends IBelyService.Stub {
    public BelyService(Context context){
        super();
    }

    public int calculateSqu(int val){
        return val*val;
    }
}

3.       将 java服务添加到 ServiceManager中：
BelyService bs = new BelyService(context);
ServiceManager.addService("BelyService", bs);

自此，重新编译 Android并运行，我们所创建的服务即可访问，下面是演示如何调用：

同样在 package/apps下任意创建一个应用，调用服务的方法如下：

import android.os.ServiceManager;

import android.os.IBelyService;

IBelyService bs = IBelyService.Stub.asInterface(ServiceManager.getService("BelyService"));

int ret = bs.calculateSqu(9);




4.add *aidl file to /frameworks/base/Android.mk 


上面我们使用的是 Android内部的方法来获取服务，当然也可以使用公开的 API: context.getSystemService来获取。
