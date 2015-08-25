package com.android.server;

import android.content.Context;
import android.os.IVdevService;
import java.io.InputStream;
import java.net.Socket;
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
import android.os.VdevParam;
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


public class VdevService extends IVdevService.Stub {

    private static final String TAG = "VdevService";
    private static final boolean DBG = true;

    private static final String STATE_SOCKET_ADDR = "127.0.0.1";
    private static final int STATE_SOCKET_PORT = 8891;

    private final Context mContext;
    private Socket mSocket = null;
    private VdevReadThread mReadThread = new  VdevReadThread();

    public VdevService(Context context) {
        super();
        try {
            mSocket = new Socket(STATE_SOCKET_ADDR, STATE_SOCKET_PORT);
            if(!mSocket .isConnected()) {
                Log.d(TAG, "state Connect to socket FAILED");
            }
            Log.d(TAG, "state Connected to socket");
        } catch(IOException e) {
            Log.d(TAG, "state Connect to socket FAILED "+e);
        }

        mContext = context;
        mReadThread.start();
    }

    private class VdevReadThread extends Thread {
        byte[] buffer;
        public int process_cmd_resp(byte[] bf) {

            if(bf[0] == VdevParam.CMD_REQUEST_PIN) {
                Intent Intent = new Intent(VdevParam.ACTION_REQUEST_PIN_RSP);
                Intent.putExtra(VdevParam.EXTRA_REQUEST_PIN_NUM,(int)bf[1]);
                Intent.putExtra(VdevParam.EXTRA_REQUEST_PIN_RET,(int)bf[2]);
                mContext.sendBroadcast(Intent);
            } else if(bf[0] == VdevParam.CMD_GET_PIN_CONFIG) {
                Intent Intent = new Intent(VdevParam.ACTION_GET_PIN_CONFIG_RSP);
                Intent.putExtra(VdevParam.EXTRA_GET_PIN_CONFIG_NUM,(int)bf[1]);
                Intent.putExtra(VdevParam.EXTRA_GET_PIN_CONFIG_RET,(int)bf[2]);
                mContext.sendBroadcast(Intent);
            } else if(bf[0] == VdevParam.CMD_SET_PIN_LEVEL) {
                Intent Intent = new Intent(VdevParam.ACTION_SET_PIN_LEVEL_RSP);
                Intent.putExtra(VdevParam.EXTRA_SET_PIN_LEVEL_NUM,(int)bf[1]);
                Intent.putExtra(VdevParam.EXTRA_SET_PIN_LEVEL_RET,(int)bf[2]);
                mContext.sendBroadcast(Intent);
            } else if(bf[0] == VdevParam.CMD_GET_PIN_LEVEL) {
                Intent Intent = new Intent(VdevParam.ACTION_GET_PIN_LEVEL_RSP);
                Intent.putExtra(VdevParam.EXTRA_GET_PIN_LEVEL_NUM,(int)bf[1]);
                Intent.putExtra(VdevParam.EXTRA_GET_PIN_LEVEL_RET,(int)bf[2]);
                mContext.sendBroadcast(Intent);
            } else if(bf[0] == VdevParam.CMD_FREE_PIN) {
                Intent Intent = new Intent(VdevParam.ACTION_FREE_PIN_RSP);
                Intent.putExtra(VdevParam.EXTRA_FREE_PIN_NUM,(int)bf[1]);
                Intent.putExtra(VdevParam.EXTRA_FREE_PIN_RET,(int)bf[2]);
                mContext.sendBroadcast(Intent);
            } else if(bf[0] == VdevParam.CMD_IRQ_TRIGGER) {
                Intent Intent = new Intent(VdevParam.ACTION_IRQ_TRIGGER);
                Intent.putExtra(VdevParam.EXTRA_IRQ_TRIGGER_NUM,(int)bf[1]);
                Intent.putExtra(VdevParam.EXTRA_IRQ_TRIGGER_RET,(int)bf[2]);
                mContext.sendBroadcast(Intent);
            } else if(bf[0] == VdevParam.CMD_READ_I2C) {
            } else if(bf[0] == VdevParam.CMD_WRITE_I2C) {
            }

            return 0;
        }
 
        public void run() {
            while(true) {
                buffer = recvData();
                process_cmd_resp(buffer);
            }
        }

    }



    private int sendData(byte[] data) {

        DataOutputStream out;
        if(mSocket == null) {
            Log.d(TAG, "mOutputStream == null; send Data failed");
            return -1;
        }
        try {
            out = new DataOutputStream(mSocket.getOutputStream()); 
            out.write(data);

        } catch(IOException e) {
            e.printStackTrace();
        }
        return 0;
    }

    private byte[] recvData() {
        DataInputStream in;
        byte[] bytes = new byte[3];

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

    //PINS API
    public int requestPin(int idx, int type, int value) {
        Log.d(TAG, "requestPin : id " + idx + " type " + type + " value " + value);
        byte[] data = new byte[4];
        data[0] = VdevParam.CMD_REQUEST_PIN;
        data[1] = (byte)idx;
        data[2] = (byte)type;
        data[3] = (byte)value;
        return sendData(data);
    }

    public int getPinConfig(int idx) {
        Log.d(TAG, "get Pin " + idx + " config  ");
        byte[] data = new byte[2];
        data[0] = VdevParam.CMD_GET_PIN_CONFIG;
        data[1] = (byte)idx;
        return sendData(data);
    }

    public int setPinLevel(int idx, int value) {
        Log.d(TAG, "setPinLevel  " + idx + " value " + value);
        byte[] data = new byte[3];
        data[0] = VdevParam.CMD_SET_PIN_LEVEL;
        data[1] = (byte)idx;
        data[2] = (byte)value;
        return sendData(data);
    }

    public int getPinLevel(int idx) {
        Log.d(TAG, "getPinLevel id " + idx);
        byte[] data = new byte[2];
        data[0] = VdevParam.CMD_GET_PIN_LEVEL;
        data[1] = (byte)idx;
        return sendData(data);
    }

    public int freePin(int idx) {
        Log.d(TAG, "free id " + idx);
        byte[] data = new byte[2];
        data[0] = VdevParam.CMD_FREE_PIN;
        data[1] = (byte)idx;
        return sendData(data);
    }

    public byte[] readI2c(int addr, int len) {
        return null;
    }
    public int writeI2c(int addr, byte[] data, int len) {
        return 0;
    }

}
