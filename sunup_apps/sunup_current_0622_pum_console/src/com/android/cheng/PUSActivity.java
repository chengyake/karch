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
import android.content.IntentFilter;  
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import android.content.Context;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.GridView;
import android.widget.ImageView;
import android.content.BroadcastReceiver;  
import java.util.ArrayList;
import java.util.HashMap;
import android.util.Log;

import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.SimpleAdapter; 
import android.content.ServiceConnection;
import android.content.ComponentName;
import android.os.IBinder;
import android.os.SystemClock;

import android.content.SharedPreferences;
public class PUSActivity extends Activity {


	private TextView copyright = null;
    private IComService mIComService;

    private BroadcastReceiver mBroadcastReceiver = new BroadcastReceiver(){
        @Override
            public void onReceive(Context context, Intent intent) {
                String action = intent.getAction();
                if (action.equals(ComService.ACTION_FINISH)) {
                    Log.e("chengyake", "in PUSActivity finish");
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
        setContentView(R.layout.pus_tab);
        GridView gridview = (GridView) findViewById(R.id.gridview);

		IntentFilter myIntentFilter = new IntentFilter();
		myIntentFilter.addAction(ComService.ACTION_FINISH);
		registerReceiver(mBroadcastReceiver, myIntentFilter);

        Intent j  = new Intent();  
        j.setClass(PUSActivity.this, ComService.class);
        getApplicationContext().bindService(j, mServiceConnection, BIND_AUTO_CREATE);

        ArrayList<HashMap<String, Object>> lstImageItem = new ArrayList<HashMap<String, Object>>();
        
        //console
        HashMap<String, Object> map1 = new HashMap<String, Object>();
        map1.put("ItemImage", R.drawable.console);
        map1.put("ItemText", "Console");
        lstImageItem.add(map1);
                
        //error data
        HashMap<String, Object> map2 = new HashMap<String, Object>();
        map2.put("ItemImage", R.drawable.errordata);
        map2.put("ItemText", "Error Data");
        lstImageItem.add(map2);
                
        //Job Data
        HashMap<String, Object> map3 = new HashMap<String, Object>();
        map3.put("ItemImage", R.drawable.jobdata);
        map3.put("ItemText", "JOB Data");
        lstImageItem.add(map3);

        //date time
        HashMap<String, Object> map5 = new HashMap<String, Object>();
        map5.put("ItemImage", R.drawable.datetime);
        map5.put("ItemText", "Date&Time");
        lstImageItem.add(map5);

        //Clear pw
        HashMap<String, Object> map6 = new HashMap<String, Object>();
        map6.put("ItemImage", R.drawable.clearpw);
        map6.put("ItemText", "Clear PW");
        lstImageItem.add(map6);

        //GetInfo
        HashMap<String, Object> map4 = new HashMap<String, Object>();
        map4.put("ItemImage", R.drawable.program);
        map4.put("ItemText", "Version Info");
        lstImageItem.add(map4);

        SimpleAdapter saImageItems = new SimpleAdapter(this,  lstImageItem, R.layout.icon,
                new String[] {"ItemImage","ItemText"},  new int[] {R.id.ItemImage,R.id.ItemText});

        gridview.setAdapter(saImageItems);
        gridview.setOnItemClickListener(new ItemClickListener());

		copyright = (TextView) findViewById(R.id.copyright);

        Log.e("chengyake", "in PUSActivity onCreate func");
    }

    @Override  
    protected void onResume() {  
        super.onResume();
        Intent mIntent = new Intent();
        mIntent.setAction(ComService.ACTION_INTO_PUS);
        sendBroadcast(mIntent);
        Log.e("chengyake", "onResume called.");
    }  

    @Override
    protected void onDestroy() {
          this.unregisterReceiver(mBroadcastReceiver);
          getApplicationContext().unbindService(mServiceConnection);

          super.onDestroy();
    }



    @Override  
    protected void onPause() {  
        super.onPause();  
        Log.e("chengyake", "onPause called.");
        /*
        try {
            if(mIComService.getUartStatus() == ComService.UART_PUS) {
                if(mIComService.exitPUS() != 0) {
                    //busy wait...
                    //return;
                }
            }
        } catch  (Exception e) {
            throw new IllegalStateException("upload error", e);
        }*/
    }  


    class  ItemClickListener implements OnItemClickListener
    {
        public void onItemClick(AdapterView<?> arg0,//The AdapterView where the click happened 
                View arg1,//The view within the AdapterView that was clicked
                int arg2,//The position of the view in the adapter
                long arg3//The row id of the item that was clicked
                ) {
            /*
            try {
                if(!mIComService.getUartStatus().equals(ComService.UART_HANDSHAKED)) {
                    Toast.makeText(getApplicationContext(), "UART is not ready",
                            Toast.LENGTH_SHORT).show();
                    return;
                }
            } catch  (Exception e) {
                throw new IllegalStateException("upload error", e);
            }


            if(arg2 < 5) {
                //check register status
                SharedPreferences shareEdit = getSharedPreferences("data", 0); 
                if(shareEdit.getString("sunup_lisence", "false").equals("true") == false) {
                    Toast.makeText(getApplicationContext(), "you need register by Verison Info",
                            Toast.LENGTH_SHORT).show();
                    return;
                }


                try {
                    if(mIComService.intoPUS() != 0) {
                        Toast.makeText(getApplicationContext(), "uart is busy now, wait and try again later",
                                Toast.LENGTH_SHORT).show();
                        return;
                    }
                } catch  (Exception e) {
                    throw new IllegalStateException("upload error", e);
                }
            }*/
            
            Intent intent=new Intent();
            switch(arg2) {
                case 0:
                    intent.setClass(PUSActivity.this, PUSConsoleActivity.class);
                    startActivity(intent);
                    break;
                case 1:
                    intent.setClass(PUSActivity.this, PUSErrorDataActivity.class);
                    startActivity(intent);
                    break;
                case 2:
                    intent.setClass(PUSActivity.this, PUSJobDataActivity.class);
                    startActivity(intent);
                    break;
                case 3:
                    intent.setClass(PUSActivity.this, PUSTimeActivity.class);
                    startActivity(intent);
                    break;
                case 4:
                    intent.setClass(PUSActivity.this, PUSCLPWActivity.class);
                    startActivity(intent);
                    break;
                case 5:
                    intent.setClass(PUSActivity.this, PUSGetInfoActivity.class);
                    startActivity(intent);
                    break;
                default:
                    break;
            }

        }

    }
}


