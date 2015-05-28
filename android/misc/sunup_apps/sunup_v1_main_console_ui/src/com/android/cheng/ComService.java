package com.android.cheng;

import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;
import android.text.format.Time;
import android.util.Log;
import android.os.RemoteException;
import java.lang.ref.WeakReference;

import com.android.cheng.driver.UsbSerialDriver;
import com.android.cheng.driver.UsbSerialProber;
import com.android.cheng.util.SerialInputOutputManager;
import com.android.cheng.util.HexDump;

import java.util.ArrayList;
import java.util.List;

import android.content.Context;

import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbManager;

import java.io.IOException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class ComService extends Service {
    private static final String TAG = "chengyake ComService";

    private UsbManager mUsbManager;
    private UsbSerialDriver mDriver;
    private SerialInputOutputManager mSerialIoManager;


    private final ExecutorService mExecutor = Executors.newSingleThreadExecutor();


    private final SerialInputOutputManager.Listener mListener =
            new SerialInputOutputManager.Listener() {

        @Override
        public void onRunError(Exception e) {
            Log.d(TAG, "Runner stopped.");
        }

        @Override
        public void onNewData(final byte[] data) {
            int i;
            Log.e(TAG, "get data");
            for(i=0; i<data.length; i++) {
                Log.e(TAG, "" + data[i]);
            }

            /*
            SerialConsoleActivity.this.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                //SerialConsoleActivity.this.updateReceivedData(data);
                }
            });
            */
        }
    };


    @Override
    public IBinder onBind(Intent intent) {
        Log.e(TAG, "start IBinder~~~");
        return aBinder;
    }
    @Override
    public void onCreate() {
        Log.e(TAG, "start onCreate~~~");
        super.onCreate();

        mUsbManager = (UsbManager) getSystemService(Context.USB_SERVICE);

        //find driver
        for (final UsbDevice device : mUsbManager.getDeviceList().values()) {
            final List<UsbSerialDriver> drivers =
                UsbSerialProber.probeSingleDevice(mUsbManager, device);
            Log.d(TAG, "Found usb device: " + device);
            if (!drivers.isEmpty()) {
                for (UsbSerialDriver driver : drivers) {
                    Log.d(TAG, "  + " + driver);
                    mDriver = driver;
                }
            }
        }
        //open and set driver
        try {
            mDriver.open();
            mDriver.setParameters(115200, 8, UsbSerialDriver.STOPBITS_1, UsbSerialDriver.PARITY_NONE);
            mDriver.setRTS(true);
        } catch (IOException e) {
            Log.e(TAG, "Error setting up device: " + e.getMessage(), e);
            try {
                mDriver.close();
            } catch (IOException e2) {
                // Ignore.
            }
            mDriver = null;
            return;
        }

        stopIoManager();
        startIoManager();

    }

    private void stopIoManager() {
        if (mSerialIoManager != null) {
            Log.i(TAG, "Stopping io manager ..");
            mSerialIoManager.stop();
            mSerialIoManager = null;
        }
    }

    private void startIoManager() {
        if (mDriver != null) {
            Log.i(TAG, "Starting io manager ..");
            mSerialIoManager = new SerialInputOutputManager(mDriver, mListener);
            mExecutor.submit(mSerialIoManager);
        }
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

    public String getSystemTime(){

        Log.e(TAG, "chengyake start getSystemTime ~~~");
        Time t = new Time();
        t.setToNow();
        return t.toString();
    }

    public int writeAsync(byte[] data){
        Log.e(TAG, "chengyake write Bytes");
        mSerialIoManager.writeAsync(data);
        return 0;
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

        public int writeAsync(byte[] data) {
            return mService.get().writeAsync(data);
        }

    };
    private final IBinder aBinder = new IComServiceStub(this);
}





