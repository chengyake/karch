package com.android.jni;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import android.content.Context;


import android.util.Log;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import java.io.IOException;

import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbManager;

import com.hoho.android.usbserial.driver.UsbSerialDriver;
import com.hoho.android.usbserial.driver.UsbSerialProber;
import com.hoho.android.usbserial.util.SerialInputOutputManager;
import com.hoho.android.usbserial.util.HexDump;

public class JNIActivity extends Activity implements OnClickListener{

    private final String TAG = "chengyake";

    private static final int MESSAGE_REFRESH = 101;
    private static final long REFRESH_TIMEOUT_MILLIS = 1000;

	private EditText mInputText = null;
	private Button mCommit = null;
	private TextView mShowText = null;

    private final ExecutorService mExecutor = Executors.newSingleThreadExecutor();

    private static UsbSerialDriver sDriver = null;
    private UsbManager mUsbManager;

    private SerialInputOutputManager mSerialIoManager;
    private final SerialInputOutputManager.Listener mListener =
            new SerialInputOutputManager.Listener() {

        @Override
        public void onRunError(Exception e) {
            Log.d(TAG, "Runner stopped.");
        }

        @Override
        public void onNewData(final byte[] data) {
                    mShowText.setText(HexDump.dumpHexString(data) + "\n\n");

        }
    };

    private final Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case MESSAGE_REFRESH:
                    mHandler.sendEmptyMessageDelayed(MESSAGE_REFRESH, REFRESH_TIMEOUT_MILLIS);
                    refreshDeviceList();
                    break;
                default:
                    super.handleMessage(msg);
                    break;
            }
        }

    };

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.main);
        mUsbManager = (UsbManager) getSystemService(Context.USB_SERVICE);
        
        initView();
    }
    @Override
    protected void onResume() {
        super.onResume();
        mHandler.sendEmptyMessage(MESSAGE_REFRESH);

    }

    @Override
        protected void onPause() {
            super.onPause();
            if(sDriver != null) {

                try {
                    sDriver.close();
                } catch (IOException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                }

            } else {
                mHandler.removeMessages(MESSAGE_REFRESH);
            }

            finish();
        }

    private void stopIoManager() {
        if (mSerialIoManager != null) {
            mSerialIoManager.stop();
            mSerialIoManager = null;
        }
    }

    private void startIoManager() {
        if (sDriver != null) {
            mSerialIoManager = new SerialInputOutputManager(sDriver, mListener);
            mExecutor.submit(mSerialIoManager);
        }
    }

    private void onDeviceStateChange() {
        stopIoManager();
        startIoManager();
    }

    private void refreshDeviceList() {
        for (final UsbDevice device : mUsbManager.getDeviceList().values()) {
            final List<UsbSerialDriver> drivers =
                UsbSerialProber.probeSingleDevice(mUsbManager, device);
            if (!drivers.isEmpty()) {
                for (UsbSerialDriver driver : drivers) {
                    mHandler.removeMessages(MESSAGE_REFRESH);
                    sDriver = driver;
                    try {
                        sDriver.open();
                        sDriver.setParameters(115200, 8, UsbSerialDriver.STOPBITS_1, 
                            UsbSerialDriver.PARITY_NONE);
                        //sDriver.setDTR(true);
                        //sDriver.setRTS(true);

                        mShowText.setText("sDriver open success");
                    } catch (IOException e) {
                        try {
                            sDriver.close();
                            mShowText.setText("sDriver open error");
                        } catch (IOException e2) {
                            // Ignore.
                        }
                        sDriver = null;
                        return;
                    }

                    onDeviceStateChange();
                    break;
                }
                break;
            }
        }
    }

	private void initView() {
		mInputText = (EditText) findViewById(R.id.inputText);
		mCommit = (Button) findViewById(R.id.commit);
		mCommit.setOnClickListener(this);
		mShowText = (TextView) findViewById(R.id.showText);
		
	}

	public void onClick(View v) {
		switch (v.getId()) {
		case R.id.commit:
			commit();
			break;

		default:
			break;
		}
	}

    private void commit() {
        if(sDriver != null) {
            String inputStr = mInputText.getText().toString();
            byte[] tmp = HexDump.StringFormatByteArray(inputStr);
            int i;
            for(i=0; i<tmp.length; i++)
            {
                Log.d("chengyake", "send data " + tmp[i]);
            }
            //mSerialIoManager.writeAsync(tmp);

            try {
                sDriver.write(tmp, 200);
            } catch (IOException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }


        } else {
            mShowText.setText("No device or driver");
        }

	}
}



