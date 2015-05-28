package com.android.cheng;

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
import android.text.InputType;
import android.util.Log;
import java.util.ArrayList;
import java.util.List;
import java.io.IOException;
import android.graphics.Color;

import android.widget.Toast;
import android.os.IBinder;
import android.content.Intent;
import android.content.ServiceConnection;

import android.content.ComponentName;
import android.os.SystemClock;
import android.view.View.OnFocusChangeListener;

public class PUSCLPWActivity extends Activity {
    


    private IComService mIComService;
	private Button clpw;

    private ServiceConnection mServiceConnection = new ServiceConnection() {
        public void onServiceConnected(ComponentName name, IBinder service) {  
            mIComService = IComService.Stub.asInterface(service);
        } 

        public void onServiceDisconnected(ComponentName name) {  
        }  
    }; 

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		
		setContentView(R.layout.pus_clpw);

        Intent j  = new Intent();  
        j.setClass(this, ComService.class);
        getApplicationContext().bindService(j, mServiceConnection, BIND_AUTO_CREATE);
        
        clpw=(Button)findViewById(R.id.clpw);
        clpw.setOnClickListener(new View.OnClickListener() {
	    public void onClick(View v) {


           try {
               if(! ComService.UART_PUS.equals(mIComService.getUartStatus())) {
                   Toast.makeText(getApplicationContext(), "UART Error",
                       Toast.LENGTH_SHORT).show();
                       return;
               }
               while(0!=mIComService.downloadPUSConsole("84020000", "00")) {
                    SystemClock.sleep(10);
               }
               while(0!=mIComService.downloadPUSConsole("84020001", "00")) {
                    SystemClock.sleep(10);
               }
               while(0!=mIComService.downloadPUSConsole("84020002", "00")) {
                    SystemClock.sleep(10);
               }
               while(0!=mIComService.downloadPUSConsole("84020003", "00")) {
                    SystemClock.sleep(10);
               }

               while(0!=mIComService.preDownloadPUSJobdata()) {
                    SystemClock.sleep(200);
               }

               while(0!=mIComService.downloadPUSJobdata("0061F019", "00")) {
                    SystemClock.sleep(10);
               }
               while(0!=mIComService.downloadPUSJobdata("0061F01A", "00")) {
                    SystemClock.sleep(10);
               }
               while(0!=mIComService.downloadPUSJobdata("0061F01B", "00")) {
                    SystemClock.sleep(10);
               }
               while(0!=mIComService.downloadPUSJobdata("0061F01C", "00")) {
                    SystemClock.sleep(10);
               }
               while(0!=mIComService.downloadPUSJobdata("0061F01D", "00")) {
                    SystemClock.sleep(10);
               }
               while(0!=mIComService.downloadPUSJobdata("0061F01E", "00")) {
                    SystemClock.sleep(10);
               }
               while(0!=mIComService.downloadPUSJobdata("0061F01F", "00")) {
                    SystemClock.sleep(10);
               }
               while(0!=mIComService.downloadPUSJobdata("0061F020", "19")) {
                    SystemClock.sleep(10);
               }
               while(0!=mIComService.downloadPUSJobdata("0061F021", "00")) {
                    SystemClock.sleep(10);
               }
               while(0!=mIComService.downloadPUSJobdata("0061F022", "00")) {
                    SystemClock.sleep(10);
               }
               while(0!=mIComService.downloadPUSJobdata("0061F023", "00")) {
                    SystemClock.sleep(10);
               }
               while(0!=mIComService.downloadPUSJobdata("0061F024", "1E")) {
                    SystemClock.sleep(10);
               }
               while(0!=mIComService.flashPUSJobdata()) {
                    SystemClock.sleep(200);
               }
               Toast.makeText(getApplicationContext(), "Clear PW Success",
                       Toast.LENGTH_SHORT).show();

              
           } catch  (Exception e) {
               throw new IllegalStateException("getVersionInfo error", e);
           }


			}
		});

	}

    @Override
    protected void onDestroy() {
        getApplicationContext().unbindService(mServiceConnection);
        super.onDestroy();
    }



}



