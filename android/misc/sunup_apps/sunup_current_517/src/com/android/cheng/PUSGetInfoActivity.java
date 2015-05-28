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
import android.telephony.TelephonyManager;
import java.io.IOException;
import android.graphics.Color;
import android.view.View.OnFocusChangeListener;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date; 
import java.util.GregorianCalendar;
import android.widget.Toast;
import android.content.BroadcastReceiver;  
import android.content.Intent;
import android.content.IntentFilter;  


import android.content.SharedPreferences;
public class PUSGetInfoActivity extends Activity {
    
    private EditText edit_id;
    private EditText edit_lisence;
	private Button get_id;
	private Button check_lisence;
    
    private String deviceId;



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


	@Override
	public void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		
		setContentView(R.layout.pus_get_info);

		IntentFilter myIntentFilter = new IntentFilter();
		myIntentFilter.addAction(ComService.ACTION_FINISH);
		registerReceiver(mBroadcastReceiver, myIntentFilter);

        deviceId = ((TelephonyManager) getApplicationContext().getSystemService(Context.TELEPHONY_SERVICE)).getDeviceId();

        edit_id = (EditText)findViewById(R.id.device_id);
        edit_lisence = (EditText)findViewById(R.id.lisence);
        edit_id.setInputType(InputType.TYPE_NULL);

        get_id=(Button)findViewById(R.id.get_id);
        check_lisence=(Button)findViewById(R.id.check_lisence);

        get_id.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) {
			    edit_id.setText(getRandomString());
			}
		});

        check_lisence.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) {
			    checkLisence(getRandomString(), edit_lisence.getText().toString());
			}
		});
        
	}

    public String getCurrentDate() {
        SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd");
        String tmp = df.format(new Date()); 

        StringBuffer dateBuf=new StringBuffer(); 
        dateBuf.append(tmp);
        //return df.format(new Date()); 
        return dateBuf.substring(8, 10);
    }

    public String getRandomString() { 
        return deviceId + getCurrentDate(); 
    }

    public boolean checkLisence(String devId, String lis) {

        SharedPreferences shareEdit = getSharedPreferences("data", 0); 

        if(shareEdit.getString("sunup_lisence", "false").equals("true")) {
            Toast.makeText(getApplicationContext(), "you have registed success",
                    Toast.LENGTH_SHORT).show();
            return true;
        }

        if(devId.equals(lis)) {
            SharedPreferences.Editor sharedata = getSharedPreferences("data", 0).edit(); 
            sharedata.putString("sunup_lisence","true"); 
            sharedata.commit();

            Toast.makeText(getApplicationContext(), "register success",
                    Toast.LENGTH_SHORT).show();
            return true;
        } else {
            Toast.makeText(getApplicationContext(), "register failed",
                    Toast.LENGTH_SHORT).show();
            return false;
        }
    }

}


