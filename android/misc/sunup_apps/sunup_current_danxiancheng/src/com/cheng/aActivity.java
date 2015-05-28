package com.cheng;

import android.content.Intent;
import android.view.View;
import android.widget.Button;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.TabActivity;
import android.content.DialogInterface;
import android.graphics.Color;
import android.os.Bundle;
import android.widget.TabHost;
import android.widget.Toast;
import android.widget.TabHost.OnTabChangeListener;

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
import android.app.NotificationManager;
import android.app.Notification;
import android.app.PendingIntent;

import android.os.SystemClock;


/**
 * @author nwang
 * 
 *	android»ù±¾²¼¾Ö¡¢×é¼þµÈµÈ²Ù×÷£¡
 */
public class aActivity extends Activity {
    /** Called when the activity is first created. */
	
    //handshake
    public static final String UART_HANDSHAKE_1C = "3030303000000100010000";
    public static final String UART_HANDSHAKE_1A = "3031303000000100010001";
    public static final String UART_HANDSHAKE_2C = "3031303000000100010001";
    public static final String UART_HANDSHAKE_2A = "3031303000000100010001";
    public static final String UART_HANDSHAKE_3C = "304C44470000010001007F";




    //into pus
    public static final String INTO_PUS_C = "43505441020001000100010005";
    public static final String INTO_PUS_A = "43505441640001000100505553353030000030313536000000000000000000000000000000000000000047413132313939332D31393000000000323031333033303800000000000000000000000000000000000000000000000000000000000000000000000000000000000000001A";

    public static final String EXIT_PUS_C = "43505444020001000100010000";
    public static final String EXIT_PUS_A = "2E414B000400010001004350544423";

    public static final String INTO_PUM_C = "43505441020001000100020006";
    public static final String INTO_PUM_A = "435054410800010001000302FFFFFF000000F0";

    public static final String EXIT_PUM_C = "43505444020001000100020003";
    public static final String EXIT_PUM_A = "2E414B000400010001004350544423";


    public static final String GET_INFO_1C = "4348435000000100010018";
    public static final String GET_INFO_1A = "2E4E4B010400010001004348435036";

    public static final String GET_INFO_2C = "4349435000000100010019";
    public static final String GET_INFO_2A = "2E4E4B210400010001004349435017";

    public static final String GET_INFO_3C = "434A43500000010001001A";
    public static final String GET_INFO_3A = "2E4E4B21040001000100434A435014";

    public static final String GET_INFO_4C = "434B43500000010001001B";
    public static final String GET_INFO_4A = "2E4E4B21040001000100434B435015";

    public static final String GET_INFO_5C = "4350525600000100010017";
    public static final String GET_INFO_5A = 
"43505256720001000100505553353030000030313536000000000000000000000000000000000000000047413132313939332D31393000000000323031333033303800000000000000000000000000000000000000000000000000000000000000000000000000000000000000005001FFFFFF0000003768224D328B3A";




	private Button frameLayout;
	private Button relativeLayout;
	private Button relativeAndLinear;
	private Button tableLayout;
	
    private int flag = 0;
    private int getinfo = 0;
    private int bigData = 0;

    private UsbManager mUsbManager;
    private UsbSerialDriver mDriver;

    public int writeBeats(){
        
        byte[] beats = {0x02, 0x21, 0x03};
        Log.e("chengyake", "write------------------ 0x21");
        try {
            mDriver.write(beats, 200);
        } catch (IOException e) {
            Log.e("chengyake", "Error setting up device: " + e.getMessage(), e);
        }

        return 0;
    }

    class ReadThread extends Thread {  
        public void run() {  
            int len=0;
            int i;
            while(true) {
                byte[] mReadBuffer = new byte[4096];
                    synchronized (mReadBuffer) {
                try {
                    len = mDriver.read(mReadBuffer, 200);
                } catch (IOException e) {
                    Log.e("chengyake", "Error setting up device: " + e.getMessage(), e);
                }

/*
                if(mReadBuffer[1] == 0x21) { //|| (len > 100 && status != UART_PUS && status != UART_PUM)) {
                    Log.e("chengyake", "------------------ rsp beats");
                    writeBeats();
                    return;
                }
*/


                if(len <= 2) continue;

                if(len > 128) {
                    StringBuffer rspBuf=new StringBuffer(); 
                    rspBuf.append(new String(mReadBuffer));
                    String rsp=rspBuf.substring(1, len-1);
                    bigData++;
                    String rsp1=rspBuf.substring(len-3, len-1);
                    Log.e("chengyake", "read------------------" + rsp);
                    if(bigData < 7) {
                        writeBeats();
                    } else if(bigData == 7){
                        try {
                            flag = 2;
                            String tmp = "\2" + INTO_PUS_C  + "\3";
                            mDriver.write(tmp.getBytes(), 200);
                            Log.e("chengyake", "write ----------" + INTO_PUS_C);
                        } catch (IOException e) {
                            Log.e("chengyake", "Error setting up device: " + e.getMessage(), e);
                        }
                    } else if(bigData == 8) {
                        try {
                            String tmp = "\2" + GET_INFO_1C   + "\3";
                            mDriver.write(tmp.getBytes(), 200);
                            Log.e("chengyake", "write ----------" + GET_INFO_1C );
                        } catch (IOException e) {
                            Log.e("chengyake", "Error setting up device: " + e.getMessage(), e);
                        }
                    } else if(bigData == 9) {
                        writeBeats();
                    }   
                    continue;
                }

                if(mReadBuffer[1] == 0x21) {
                    Log.e("chengyake", "read------------------" + 0x21);
                    writeBeats();
                    continue;
                }

                StringBuffer rspBuf=new StringBuffer(); 
                rspBuf.append(new String(mReadBuffer));
                String rsp=rspBuf.substring(1, len-1);
                Log.e("chengyake", "read------------------" + rsp);


                if(rsp.equals(UART_HANDSHAKE_2A) && flag == 1)  {
                    try {
                        flag = 2;
                        String tmp = "\2" + UART_HANDSHAKE_3C + "\3";
                        mDriver.write(tmp.getBytes(), 200);
                        Log.e("chengyake", "write ----------" + UART_HANDSHAKE_3C);
                    } catch (IOException e) {
                        Log.e("chengyake", "Error setting up device: " + e.getMessage(), e);
                    }
                }

                if(rsp.equals(UART_HANDSHAKE_1A) && flag == 0)  {
                    try {
                        flag = 1;
                        String tmp = "\2" + UART_HANDSHAKE_2C + "\3";
                        mDriver.write(tmp.getBytes(), 200);
                        Log.e("chengyake", "write ----------" + UART_HANDSHAKE_2C);
                    } catch (IOException e) {
                        Log.e("chengyake", "Error setting up device: " + e.getMessage(), e);
                    }
                }

                if(rsp.equals(GET_INFO_1A))  {
                    try {
                        String tmp = "\2" + GET_INFO_2C + "\3";
                        mDriver.write(tmp.getBytes(), 200);
                        Log.e("chengyake", "write ----------" + GET_INFO_2C);
                    } catch (IOException e) {
                        Log.e("chengyake", "Error setting up device: " + e.getMessage(), e);
                    }
                }

                if(rsp.equals(GET_INFO_2A))  {
                    try {
                        String tmp = "\2" + GET_INFO_3C + "\3";
                        mDriver.write(tmp.getBytes(), 200);
                        Log.e("chengyake", "write ----------" + GET_INFO_3C);
                    } catch (IOException e) {
                        Log.e("chengyake", "Error setting up device: " + e.getMessage(), e);
                    }
                }

                if(rsp.equals(GET_INFO_3A))  {
                    try {
                        String tmp = "\2" + GET_INFO_4C + "\3";
                        mDriver.write(tmp.getBytes(), 200);
                        Log.e("chengyake", "write ----------" + GET_INFO_4C);
                    } catch (IOException e) {
                        Log.e("chengyake", "Error setting up device: " + e.getMessage(), e);
                    }
                }

                if(rsp.equals(GET_INFO_4A))  {
                    try {
                        String tmp = "\2" + GET_INFO_5C + "\3";
                        mDriver.write(tmp.getBytes(), 200);
                        Log.e("chengyake", "write ----------" + GET_INFO_5C);
                    } catch (IOException e) {
                        Log.e("chengyake", "Error setting up device: " + e.getMessage(), e);
                    }
                }

/*
                String chk = null; //detectCode(rsp);

                if(chk == null) {
                    //1->1
                } else {
                    //1->1 1->1
                    //1->111
                    try {
                        mDriver.write(chk.getBytes(), 200);
                    } catch (IOException e) {
                        Log.e("chengyake", "Error setting up device: " + e.getMessage(), e);
                    }
                }
                */
                }
            }
        }  
    }  


    private int probeCp2102() {

    


        mUsbManager = (UsbManager) getSystemService(Context.USB_SERVICE);
        //find driver
        
        for (final UsbDevice device : mUsbManager.getDeviceList().values()) {
            final List<UsbSerialDriver> drivers =
                UsbSerialProber.probeSingleDevice(mUsbManager, device);
            if (!drivers.isEmpty()) {
                for (UsbSerialDriver driver : drivers) {
                    Log.d("chengyake", "  + " + driver);
                    mDriver = driver;
                    Log.e("chengyake", "Found usb device: " + device + driver);
                }
            } 
        }

        
        if(mDriver == null) {
            return -1;
        }
        //open and set driver
        try {
            Log.e("chengyake", "Found usb device: debug");
            mDriver.open();
            mDriver.setParameters(115200, 8, UsbSerialDriver.STOPBITS_2, UsbSerialDriver.PARITY_ODD);
        } catch (IOException e) {
            Log.e("chengyake", "Error setting up device: " + e.getMessage(), e);
            try {
                mDriver.close();
            } catch (IOException e2) {
            }
            mDriver = null;
            return -1;

        }

        return 0;

     }




    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main_taba);
        
        frameLayout=(Button)findViewById(R.id.frameLayout);
        relativeLayout=(Button)findViewById(R.id.relativeLayout);
        relativeAndLinear=(Button)findViewById(R.id.relativeAndLinear);
        tableLayout=(Button)findViewById(R.id.tableLayout);
          
        //FrameLayout²¼¾ÖÊ¹ÓÃ
        frameLayout.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) { 
                probeCp2102();
			}
		});
        
        //RelativeLayout²¼¾ÖÊ¹ÓÃ
        relativeLayout.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
                new ReadThread().start();
			}
		});
        
        //RelativeLayoutºÍLinearLayout×ÛºÏÊ¹ÓÃ
        relativeAndLinear.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
			}
		});
        
        //TableLayout²¼¾ÖÊ¹ÓÃ
        tableLayout.setOnClickListener(new View.OnClickListener() {
			
			@Override
			public void onClick(View v) {

                try {
                    String tmp = "\2" + UART_HANDSHAKE_1C + "\3";
                    mDriver.write(tmp.getBytes(), 200);
                    Log.e("chengyake", "write ----------" + UART_HANDSHAKE_1C);
                } catch (IOException e) {
                    Log.e("chengyake", "Error setting up device: " + e.getMessage(), e);
                }

			}
		});
        
    }
}



