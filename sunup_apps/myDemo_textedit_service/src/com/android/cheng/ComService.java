package com.android.cheng;

import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;
import android.text.format.Time;
import android.util.Log;
import android.os.RemoteException;
import java.lang.ref.WeakReference;

public class ComService extends Service {
    //定义个一个Tag标签
    private static final String TAG = "chengyake ComService";
    //这里定义吧一个Binder类，用在onBind()有方法里，这样Activity那边可以获取到


    @Override
        public IBinder onBind(Intent intent) {
            Log.e(TAG, "start IBinder~~~");
            //return mBinder; //未使用aidl
            return aBinder; //使用aidl
        }
    @Override
        public void onCreate() {
            Log.e(TAG, "start onCreate~~~");
            super.onCreate();
        }

    @Override
        public void onStart(Intent intent, int startId) {
            Log.e(TAG, "start onStart~~~");
            super.onStart(intent, startId);
        }

    @Override
        public void onDestroy() {
            Log.e(TAG, "start onDestroy~~~");
            super.onDestroy();
        }


    @Override
        public boolean onUnbind(Intent intent) {
            Log.e(TAG, "start onUnbind~~~");
            return super.onUnbind(intent);
        }

    //这里我写了一个获取当前时间的函数，不过没有格式化就先这么着吧
    public String getSystemTime(){

        Log.e(TAG, "chengyake start getSystemTime ~~~");
        Time t = new Time();
        t.setToNow();
        return t.toString();
    }

    public class MyBinder extends Binder{
        ComService getService()
        {
            return ComService.this;
        }
    }
    private MyBinder mBinder = new MyBinder();  



    static class IComServiceStub extends IComService.Stub {	

        WeakReference<ComService> mService;
        IComServiceStub(ComService service) {
            mService = new WeakReference<ComService>(service);
        }
        public String getSystemTime() throws RemoteException {
            return mService.get().getSystemTime();
        }

    };
    private final IBinder aBinder = new IComServiceStub(this);
} 
