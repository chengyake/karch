package com.android.cheng;

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
import android.content.res.Resources;
import android.content.Intent;

import java.io.IOException;
import android.os.RemoteException;

import android.content.BroadcastReceiver;  
import android.content.Intent;
import android.content.IntentFilter; 
import android.content.Context;
import android.util.Log;

import android.widget.Toast;
import android.app.AlertDialog;
import android.app.AlertDialog.Builder;
import android.content.DialogInterface;


import android.os.IBinder;
import android.content.ServiceConnection;
import android.content.ComponentName;

public class MainActivity extends TabActivity {

    private IComService mIComService;


	AlertDialog.Builder builder;

    private BroadcastReceiver mBroadcastReceiver = new BroadcastReceiver(){
        @Override
            public void onReceive(Context context, Intent intent) {
                String action = intent.getAction();
                if (action.equals(ComService.ACTION_DIALOG)) {
                    Log.e("chengyake", "in MainActivity finish");

                    builder.show();
                } else if(action.equals(ComService.ACTION_FINISH)) {
                    Intent mIntent = new Intent();
                    mIntent.setAction(ComService.ACTION_FINISH);
                    sendBroadcast(mIntent);
                    finish();
                }
            }

    };

    private ServiceConnection mServiceConnection = new ServiceConnection() {  
        public void onServiceConnected(ComponentName name, IBinder service) {  
            mIComService = IComService.Stub.asInterface(service);
        } 

        public void onServiceDisconnected(ComponentName name) {  
        }  
    };  


    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);


		IntentFilter myIntentFilter = new IntentFilter();
		myIntentFilter.addAction(ComService.ACTION_FINISH);
		myIntentFilter.addAction(ComService.ACTION_DIALOG);
		registerReceiver(mBroadcastReceiver, myIntentFilter);



        Resources res = getResources(); 
        TabHost tabHost = getTabHost();
        TabHost.TabSpec spec;
        Intent intent;

        intent = new Intent().setClass(this, PUSActivity.class);

        spec = tabHost.newTabSpec("a").setIndicator("PUS",
                res.getDrawable(R.drawable.icon))
            .setContent(intent);
        tabHost.addTab(spec);

        intent = new Intent().setClass(this, PUSActivity.class);
        spec = tabHost.newTabSpec("b").setIndicator("PUM",
                res.getDrawable(R.drawable.icon))
            .setContent(intent);
        tabHost.addTab(spec);

        tabHost.setCurrentTab(0);






        builder = new Builder(this);
        builder.setTitle("标题");
        builder.setMessage("简单消息框");
        builder.setPositiveButton("确定", null);

        Intent i  = new Intent();  
        i.setClass(MainActivity.this, ComService.class);  
        getApplicationContext().startService(i);

        Intent j  = new Intent();  
        j.setClass(MainActivity.this, ComService.class);
        getApplicationContext().bindService(j, mServiceConnection, BIND_AUTO_CREATE);

        Log.e("chengyake", "in MainActivity onCreate func");
    }
}

