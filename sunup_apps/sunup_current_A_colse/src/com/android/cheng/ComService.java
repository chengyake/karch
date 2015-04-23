package com.android.cheng;

import com.android.cheng.CodeInfo;
import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;
import android.text.format.Time;
import android.util.Log;
import android.os.RemoteException;
import java.lang.ref.WeakReference;

import android.content.BroadcastReceiver;  
import android.os.Handler;
import android.os.Message;
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

import java.lang.String;
import android.widget.Toast;
import android.app.AlertDialog;
import android.app.AlertDialog.Builder;
import android.content.DialogInterface;

public class ComService extends Service {
    private static final String TAG = "chengyake ComService";

    private static final boolean debug = true;

    private static final String UART_UNCONNECTED = "com.android.cheng.uart.unconnect";
    private static final String UART_CONNECTED = "com.android.cheng.uart.connect";
    private static final String UART_INIT = "com.android.cheng.uart.init";
    private static final String UART_HANDSHAKE = "com.android.cheng.uart.handshake";

    private static final String CPU_NONE = "com.android.cheng.cpu.NONE";
    private static final String CPU_PUS = "com.android.cheng.cpu.PUS";
    private static final String CPU_PUM = "com.android.cheng.cpu.PUM";


    static final String ACTION_BEATS = "com.android.cheng.action.BEATS";
    static final String ACTION_UART_HANDSHAKE = "com.android.cheng.action.UART_HANDSHAKE";
    static final String ACTION_GET_INFO = "com.android.cheng.action.GET_INFO";
    static final String ACTION_INTO_PUS = "com.android.cheng.action.INTO_PUS";
    static final String ACTION_EXIT_PUS = "com.android.cheng.action.EXIT_PUS";
    static final String ACTION_INTO_PUM = "com.android.cheng.action.INTO_PUM";
    static final String ACTION_EXIT_PUM = "com.android.cheng.action.EXIT_PUM";
    static final String ACTION_UPLOAD_PUS_CONSOLE  = "com.android.cheng.action.UPLOAD_PUS_CONSOLE";
    static final String ACTION_DOWNLOAD_PUS_CONSOLE  = "com.android.cheng.action.DOWNLOAD_PUS_CONSOLE";
    static final String ACTION_GET_PUS_ERROR = "com.android.cheng.action.GET_PUS_ERROR";
    static final String ACTION_DEL_PUS_ERROR = "com.android.cheng.action.DEL_PUS_ERROR";
    static final String ACTION_UPLOAD_PUS_JOBDATA = "com.android.cheng.action.UPLOAD_PUS_JOBDATA";
    static final String ACTION_DOWNLOAD_PUS_JOBDATA = "com.android.cheng.action.DOWNLOAD_PUS_JOBDATA";
    static final String ACTION_GET_PUS_TIME = "com.android.cheng.action.GET_PUS_TIME";
    static final String ACTION_SET_PUS_TIME = "com.android.cheng.action.SET_PUS_TIME";
    static final String ACTION_UPLOAD_PUM_CONSOLE = "com.android.cheng.action.UPLOAD_PUM_CONSOLE";
    static final String ACTION_DOWNLOAD_PUM_CONSOLE = "com.android.cheng.action.DOWNLOAD_PUM_CONSOLE";
    static final String ACTION_GET_PUM_ERROR = "com.android.cheng.action.GET_PUM_ERROR";
    static final String ACTION_DEL_PUM_ERROR = "com.android.cheng.action.DEL_PUM_ERROR";
    static final String ACTION_UPLOAD_PUM_JOBDATA = "com.android.cheng.action.UPLOAD_PUM_JOBDATA";
    static final String ACTION_DOWNLOAD_PUM_JOBDATA = "com.android.cheng.action.DOWNLOAD_PUM_JOBDATA";
    static final String ACTION_FINISH = "com.android.cheng.action.FINISH";
    static final String ACTION_DIALOG = "com.android.cheng.action.DIALOG";
    static final String ACTION_DEBUG = "com.android.cheng.action.DEBUG";


    private static final int MESSAGE_DEBUG = 0;
    private static final int MESSAGE_BEATS = 1;
    private static final int MESSAGE_DIALOG = 2;
    private static final int MESSAGE_FINISH = 3;
    private static String debug_action;
    private static String debug_string;
    private static boolean mutex=false;



    private UsbManager mUsbManager;
    private UsbSerialDriver mDriver;
    private SerialInputOutputManager mSerialIoManager;


    private final ExecutorService mExecutor = Executors.newSingleThreadExecutor();



    private Handler mHandler = new Handler() {

        public void dispatchMessage(Message msg) {
            if (MESSAGE_BEATS == msg.what) {
                debugWrite(CodeInfo.BEATS);
                mHandler.sendMessageDelayed(mHandler.obtainMessage(MESSAGE_BEATS), 1000);
            } else if (MESSAGE_DIALOG == msg.what) {
                Intent mIntent = new Intent();
                mIntent.setAction(ACTION_DIALOG);
                sendBroadcast(mIntent);
                mHandler.sendMessageDelayed(mHandler.obtainMessage(MESSAGE_FINISH), 2000);
            } else if (MESSAGE_FINISH == msg.what) {
                Intent mIntent = new Intent();
                mIntent.setAction(ACTION_FINISH);
                sendBroadcast(mIntent);
            }

        };  
    };

    private Handler mDebug = new Handler() {

        public void dispatchMessage(Message msg) {
            if (MESSAGE_DEBUG == msg.what) {

                String rsp = detectCodeDebug(debug_string);
                String chk = detectCode(rsp);
                
                Log.e("mDebug Handler", "---------4---------- " + rsp + " chk:" + chk);
                if(chk == null) {
                    Log.e("getVersionInfo", "---------4.1----------");
                    //1->1
                    Intent intent = new Intent();
                    intent.setAction(debug_action);
                    intent.putExtra("string", rsp);
                    sendBroadcast(intent);
                    mutex = false;
                    //Toast.makeText(getApplicationContext(), "send success", Toast.LENGTH_SHORT).show();
                } else {
                    //1->1 1->1
                    //1->111
                    Log.e("getVersionInfo", "---------4.2----------");  
                    debug_string = chk;
                    mDebug.sendMessageDelayed(mDebug.obtainMessage(MESSAGE_DEBUG), 200);
                }

            }
        };
    };

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

    private int probeCp2102() {

        mUsbManager = (UsbManager) getSystemService(Context.USB_SERVICE);
        //find driver
        for (final UsbDevice device : mUsbManager.getDeviceList().values()) {
            final List<UsbSerialDriver> drivers =
                UsbSerialProber.probeSingleDevice(mUsbManager, device);
            if (!drivers.isEmpty()) {
                for (UsbSerialDriver driver : drivers) {
                    Log.d(TAG, "  + " + driver);
                    mDriver = driver;
                    Log.e(TAG, "Found usb device: " + device + driver);
                }
            } else {
            Log.e(TAG, "Found usb device:sdf debug");
                return -1;
            }
        }

        //open and set driver
        try {
            Log.e(TAG, "Found usb device: debug");
            mDriver.open();
            mDriver.setParameters(115200, 8, UsbSerialDriver.STOPBITS_1, UsbSerialDriver.PARITY_NONE);
            mDriver.setRTS(true);
            stopIoManager();
            startIoManager();
            Log.e(TAG, "Found usb device: debug");
        } catch (IOException e) {
            Log.e(TAG, "Error setting up device: " + e.getMessage(), e);
            try {
                mDriver.close();
            } catch (IOException e2) {
                // Ignore.
            }
            mDriver = null;
            return -1;
        }

        return 0;

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
    public IBinder onBind(Intent intent) {
        Log.e(TAG, "start IBinder");
        return aBinder;
    }

    @Override
    public void onCreate() {
        Log.e(TAG, "start onCreate");
        super.onCreate();


        Log.e("chengyake", "in service ComService onCreate");

        //mHandler.sendMessageDelayed(mHandler.obtainMessage(MESSAGE_BEATS), 1000);
    }


    @Override
    public void onStart(Intent intent, int startId) {
        Log.e(TAG, "start onStart~~~");
        super.onStart(intent, startId);

        if(probeCp2102() != 0) {

            Log.e("chengyake", "-------------1----------------------");
            mHandler.sendMessageDelayed(mHandler.obtainMessage(MESSAGE_DIALOG), 300);

        }
        Log.e("chengyake", "in service ComService onStart");
    }

    @Override
    public void onDestroy() {
        Log.e(TAG, "start onDestroy~~~");
        super.onDestroy();
    }

    @Override
    public boolean onUnbind(Intent intent) {
        Log.e(TAG, "start onUnbind ");
        return super.onUnbind(intent);
    }

    public String getSystemTime(){

        Log.e(TAG, "chengyake start getSystemTime ");
        Time t = new Time();
        t.setToNow();
        return t.toString();
    }

    public String debugWrite(String data){

        Log.e("getVersionInfo", "---------3----------");  
        debug_string = data;
        mDebug.sendMessageDelayed(mDebug.obtainMessage(MESSAGE_DEBUG), 500);

        return null;

    }

    public int writeAsync(String data){
        Log.e(TAG, "chengyake write Bytes");
        
        if(mutex == true) return -1;
        mutex = true;

        if(debug != true) {
            //convert
            //mSerialIoManager.writeAsync(data);
        } else {
            Log.e("getVersionInfo", "---------2----------");  
            debugWrite(data);
        }
        return 0;
    }


        public String detectCodeDebug(String data) {
            if(data.equals(CodeInfo.BEATS)) {
                debug_action=ACTION_BEATS;
                return CodeInfo.BEATS;
            } else if (data.equals(CodeInfo.UART_HANDSHAKE_1C)) {
                debug_action=ACTION_UART_HANDSHAKE;
                return CodeInfo.UART_HANDSHAKE_1A;
            } else if (data.equals(CodeInfo.UART_HANDSHAKE_2C)) {
                debug_action=ACTION_UART_HANDSHAKE;
                return CodeInfo.UART_HANDSHAKE_2A;
            } else if (data.equals(CodeInfo.GET_INFO_1C)) {
                debug_action=ACTION_GET_INFO;
                return CodeInfo.GET_INFO_1A;
            } else if (data.equals(CodeInfo.GET_INFO_2C)) {
                debug_action=ACTION_GET_INFO;
                return CodeInfo.GET_INFO_2A;
            } else if (data.equals(CodeInfo.GET_INFO_3C)) {
                debug_action=ACTION_GET_INFO;
                return CodeInfo.GET_INFO_3A;
            } else if (data.equals(CodeInfo.GET_INFO_4C)) {
                debug_action=ACTION_GET_INFO;
                return CodeInfo.GET_INFO_4A;
            } else if (data.equals(CodeInfo.GET_INFO_5C)) {
                debug_action=ACTION_GET_INFO;
                return CodeInfo.GET_INFO_5A;
            } else if (data.equals(CodeInfo.INTO_PUS_C)) {
                debug_action=ACTION_INTO_PUS;
                return CodeInfo.INTO_PUS_A;
            } else if (data.equals(CodeInfo.EXIT_PUS_C)) {
                debug_action=ACTION_EXIT_PUS;
                return CodeInfo.EXIT_PUS_A;
            } else if (data.equals(CodeInfo.INTO_PUM_C)) {
                debug_action=ACTION_INTO_PUM;
                return CodeInfo.INTO_PUM_A;
            } else if (data.equals(CodeInfo.EXIT_PUM_C)) {
                debug_action=ACTION_EXIT_PUM;
                return CodeInfo.EXIT_PUM_A;
            } else if (data.equals(CodeInfo.UPLOAD_PUS_CONSOLE_C)) {
                debug_action=ACTION_UPLOAD_PUS_CONSOLE;
                return CodeInfo.UPLOAD_PUS_CONSOLE_C;
            } else if (data.equals(CodeInfo.DOWNLOAD_PUS_CONSOLE_C)) {
                debug_action=ACTION_UPLOAD_PUS_CONSOLE;
                return CodeInfo.DOWNLOAD_PUS_CONSOLE_A;
            } else if (data.equals(CodeInfo.GET_PUS_ERROR_C)) {
                debug_action=ACTION_GET_PUS_ERROR;
                return CodeInfo.GET_PUS_ERROR_A;
            } else if (data.equals(CodeInfo.DEL_PUS_ERROR_C)) {
                debug_action=ACTION_DEL_PUS_ERROR;
                return CodeInfo.DEL_PUS_ERROR_A;
            } else if (data.equals(CodeInfo.UPLOAD_PUS_JOBDATA_C)) {
                debug_action=ACTION_UPLOAD_PUS_JOBDATA;
                return CodeInfo.UPLOAD_PUS_JOBDATA_A;
            } else if (data.equals(CodeInfo.DOWNLOAD_PUS_JOBDATA_PREPARE_C)) {
                debug_action=ACTION_DOWNLOAD_PUS_JOBDATA;
                return CodeInfo.DOWNLOAD_PUS_JOBDATA_PREPARE_A;
            } else if (data.equals(CodeInfo.DOWNLOAD_PUS_JOBDATA_RAM_C)) {
                debug_action=ACTION_DOWNLOAD_PUS_JOBDATA;
                return CodeInfo.DOWNLOAD_PUS_JOBDATA_RAM_A;
            } else if (data.equals(CodeInfo.DOWNLOAD_PUS_JOBDATA_FLASH_C)) {
                debug_action=ACTION_DOWNLOAD_PUS_JOBDATA;
                return CodeInfo.DOWNLOAD_PUS_JOBDATA_FLASH_A;
            } else if (data.equals(CodeInfo.GET_PUS_TIME_C)) {
                debug_action=ACTION_GET_PUS_TIME;
                return CodeInfo.GET_PUS_TIME_A;
            } else if (data.equals(CodeInfo.SET_PUS_TIME_C)) {
                debug_action=ACTION_SET_PUS_TIME;
                return CodeInfo.SET_PUS_TIME_A;
            } else if (data.equals(CodeInfo.UPLOAD_PUM_CONSOLE_C)) {
                debug_action=ACTION_UPLOAD_PUM_CONSOLE;
                return CodeInfo.UPLOAD_PUM_CONSOLE_A; 
            } else if (data.equals(CodeInfo.DOWNLOAD_PUM_CONSOLE_C)) {
                debug_action=ACTION_DOWNLOAD_PUM_CONSOLE;
                return CodeInfo.DOWNLOAD_PUM_CONSOLE_A;
            } else if (data.equals(CodeInfo.GET_PUM_ERROR_C)) {
                debug_action=ACTION_GET_PUM_ERROR;
                return CodeInfo.GET_PUM_ERROR_A;
            } else if (data.equals(CodeInfo.DEL_PUM_ERROR_C)) {
                debug_action=ACTION_DEL_PUM_ERROR;
                return CodeInfo.DEL_PUM_ERROR_A;
            } else if (data.equals(CodeInfo.UPLOAD_PUM_JOBDATA_C)) {
                debug_action=ACTION_UPLOAD_PUM_JOBDATA;
                return CodeInfo.UPLOAD_PUM_JOBDATA_A;
            } else if (data.equals(CodeInfo.DOWNLOAD_PUM_JOBDATA_PREPARE_C)) {
                debug_action=ACTION_DOWNLOAD_PUM_JOBDATA;
                return CodeInfo.DOWNLOAD_PUM_JOBDATA_PREPARE_A;
            } else if (data.equals(CodeInfo.DOWNLOAD_PUM_JOBDATA_RAM_C)) {
                debug_action=ACTION_DOWNLOAD_PUM_JOBDATA;
                return CodeInfo.DOWNLOAD_PUM_JOBDATA_RAM_A;
            } else if (data.equals(CodeInfo.DOWNLOAD_PUM_JOBDATA_FLASH_C)) {
                debug_action=ACTION_DOWNLOAD_PUM_JOBDATA;
                return CodeInfo.DOWNLOAD_PUM_JOBDATA_FLASH_A;
            }
            
            return null;
        }

        public String detectCode(String data) {
            if(data.equals(CodeInfo.BEATS)) {
                return CodeInfo.BEATS;
            } else if (data.equals(CodeInfo.UART_HANDSHAKE_1A)) {
                return CodeInfo.UART_HANDSHAKE_2C;
            } else if (data.equals(CodeInfo.GET_INFO_1A)) {
                return CodeInfo.GET_INFO_2C;
            } else if (data.equals(CodeInfo.GET_INFO_2A)) {
                return CodeInfo.GET_INFO_3C; 
            } else if (data.equals(CodeInfo.GET_INFO_3A)) {
                return CodeInfo.GET_INFO_4C; 
            } else if (data.equals(CodeInfo.GET_INFO_4A)) {
                return CodeInfo.GET_INFO_5C; 
            } else if (data.equals(CodeInfo.DOWNLOAD_PUS_JOBDATA_PREPARE_A)) {
                return CodeInfo.DOWNLOAD_PUS_JOBDATA_RAM_C; 
            } else if (data.equals(CodeInfo.DOWNLOAD_PUS_JOBDATA_RAM_A)) {
                return CodeInfo.DOWNLOAD_PUS_JOBDATA_FLASH_C;
            } else if (data.equals(CodeInfo.DOWNLOAD_PUM_JOBDATA_PREPARE_A)) {
                return CodeInfo.DOWNLOAD_PUM_JOBDATA_RAM_C;
            } else if (data.equals(CodeInfo.DOWNLOAD_PUM_JOBDATA_RAM_A)) {
                return CodeInfo.DOWNLOAD_PUM_JOBDATA_FLASH_C;
            }

            return null;
        }

        public int uploadPUSConsole(String addr, String size) {
            
            int i;

            if(addr == null || size == null){
                return -1;
            }

            StringBuffer addrBuf=new StringBuffer(); 
            StringBuffer sizeBuf=new StringBuffer();


            int addrLen = addr.length();
            int sizeLen = size.length();
                
            for(i=0; i<8-addrLen; i++) {
                addrBuf.append("0");
            }
            addrBuf.append(addr);

            for(i=0; i<4-sizeLen; i++) {
                sizeBuf.append("0");
            }

            sizeBuf.append(size);
            
            String mAddr = addrBuf.substring(6, 8) + addrBuf.substring(4, 6) +
                           addrBuf.substring(2, 4) + addrBuf.substring(0, 2);
            String mSize = sizeBuf.substring(2, 4) + sizeBuf.substring(0, 2);

            //Log.e("chengyake", "-------------" + mAddr + " ----- " + mSize);
            //Log.e("chengyake", "------" + addZz(CodeInfo.UPLOAD_PUS_CONSOLE_C + mAddr + mSize + "0000"));

            return writeAsync(addZz(CodeInfo.UPLOAD_PUS_CONSOLE_C + mAddr + mSize + "0000"));
        }

        public String addZz(String str) {
            int i;
            byte[] bufs = HexDump.hexStringToByteArray(str);
            byte buf = 0;

            for(i=0; i<bufs.length; i++) {
                buf=(byte)(buf ^ bufs[i]);
            }

            return str + HexDump.toHexString(buf);
        }

        public int checkZz(String str) {

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

        public int writeAsync(String data) {
            return mService.get().writeAsync(data);
        }


/**************************************************/
        public int probeCp2102() {
            return mService.get().probeCp2102();
        }
        public boolean getSyncStatus() {
            return mutex;
        }
        public String getUartStatus() {
            return null;
        }
        public String getCPUStatus() {
            return null;
        }
        public int getVersionInfo() {
            return mService.get().writeAsync(CodeInfo.GET_INFO_1C);
        }
        public int heartBeats( ) {
            return mService.get().writeAsync(CodeInfo.BEATS);
        }

        //into exit cpu
        public int intoPUS( ) {
            return mService.get().writeAsync(CodeInfo.INTO_PUS_C);
        }
        public int exitPUS( ) {
            return mService.get().writeAsync(CodeInfo.EXIT_PUS_C);
        }
        public int intoPUM( ) {
            return mService.get().writeAsync(CodeInfo.INTO_PUM_C);
        }
        public int exitPUM( ) {
            return mService.get().writeAsync(CodeInfo.EXIT_PUM_C);
        }

        //pus console
        public int uploadPUSConsole(String addr, String size) {
            return mService.get().uploadPUSConsole(addr, size);
        }
        public int downloadPUSConsole( ) {
            return mService.get().writeAsync(CodeInfo.DOWNLOAD_PUS_CONSOLE_C);
        }

        //pus error data
        public int getPUSError( ) {
            return mService.get().writeAsync(CodeInfo.GET_PUS_ERROR_C);
        }
        public int delPUSError( ) {
            return mService.get().writeAsync(CodeInfo.DEL_PUS_ERROR_C);
        }

        //pus jobdata
        public int uploadPUSJobdata( ) {
            return mService.get().writeAsync(CodeInfo.UPLOAD_PUS_JOBDATA_C);
        }
        public int downloadPUSJobdata( ) {
            return mService.get().writeAsync(CodeInfo.DOWNLOAD_PUS_JOBDATA_PREPARE_C);
        }

        //pus sync time
        public int getPUSTime( ) {
            return mService.get().writeAsync(CodeInfo.GET_PUS_TIME_C);
        }
        public int setPUSTime( ) {
            return mService.get().writeAsync(CodeInfo.SET_PUS_TIME_C);
        }
        //pum console
        public int uploadPUMConsole( ) {
            return mService.get().writeAsync(CodeInfo.UPLOAD_PUM_CONSOLE_C);
        }
        public int downloadPUMConsole( ) {
            return mService.get().writeAsync(CodeInfo.DOWNLOAD_PUM_CONSOLE_C);
        }

        //pum error data
        public int getPUMError( ) {
            return mService.get().writeAsync(CodeInfo.GET_PUM_ERROR_C);
        }
        public int delPUMError( ) {
            return mService.get().writeAsync(CodeInfo.DEL_PUM_ERROR_C);
        }

        //pum jobdata
        public int uploadPUMJobdata( ) {
            return mService.get().writeAsync(CodeInfo.UPLOAD_PUM_JOBDATA_C);
        }
        public int downloadPUMJobdata( ) {
            return mService.get().writeAsync(CodeInfo.DOWNLOAD_PUM_JOBDATA_PREPARE_C);
        }

    };
    private final IBinder aBinder = new IComServiceStub(this);
}



