package com.android.server;

import android.content.Context;
import java.io.InputStream;
import java.net.Socket;
import android.app.Service;
import java.io.PrintWriter;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import android.content.BroadcastReceiver;
import android.content.ContentResolver;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.os.Binder;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.os.SystemService;
import android.os.SystemProperties;
import android.provider.Settings;
import android.os.PowerManager;
import android.os.PowerManager.WakeLock;
import android.util.Log;
import java.io.UnsupportedEncodingException;
import java.lang.StringBuilder;
import android.os.SystemProperties;
import java.util.Date;
import java.io.IOException;


public class VdevService {

    private static final String TAG = "VdevService";
    private static final boolean DBG = true;

    private static final String STATE_SOCKET_ADDR = "127.0.0.1";
    private static final int STATE_SOCKET_PORT = 8892;

    private Socket mSocket = null;
    private VdevReadThread mReadThread = new  VdevReadThread();
    private WakeLock wakeLock;  

    public VdevService(Context context) {
        super();
        PowerManager pm = (PowerManager) context.getSystemService(Context.POWER_SERVICE);  
        wakeLock = pm.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK,  "VDEV");  
        try {
            mSocket = new Socket(STATE_SOCKET_ADDR, STATE_SOCKET_PORT);
            if(!mSocket .isConnected()) {
                Log.d(TAG, "state Connect to socket FAILED");
            }
            Log.d(TAG, "state Connected to socket SUCCESS");
        } catch(IOException e) {
            Log.d(TAG, "state Connect to socket FAILED "+e);
        }

        mReadThread.start();
    }

    private class VdevReadThread extends Thread {
        byte[] buffer;
        public void run() {
            while(true) {
                Log.d(TAG, "loop get date");
                if(mSocket == null) {
                    Log.d(TAG, "loop get date return");
                    return;
                }
                buffer = recvData();
                wakeLock.acquire(2*1000);  
                Log.d(TAG, "irq comming ----------->");

            }
        }

        private byte[] recvData() {
            DataInputStream in;
            byte[] bytes = new byte[16];

            if(mSocket == null) {
                Log.d(TAG, "mInputStream == null; recv Data failed");
                return null;
            }
            try {
                in = new DataInputStream(mSocket.getInputStream());
                in.read(bytes);
            } catch(IOException e) {
                e.printStackTrace();
            }
            return bytes;
        }

    }
}





