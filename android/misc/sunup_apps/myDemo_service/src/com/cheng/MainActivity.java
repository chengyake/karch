package com.cheng;

import android.app.Activity;  
import android.content.ComponentName;  
import android.content.Context;  
import android.content.Intent;  
import android.content.ServiceConnection;  
import android.os.Bundle;  
import android.os.IBinder;  
import android.view.View;  
import android.view.View.OnClickListener;  
import android.widget.Button;  
import android.widget.TextView;  
import android.os.RemoteException;


public class MainActivity extends Activity implements OnClickListener{  
     
    //private ComService  mComService;  
    private IComService mIComService;
    private TextView mTextView;  
    private Button startServiceButton;  
    private Button stopServiceButton;  
    private Button bindServiceButton;  
    private Button unbindServiceButton;  
    private Context mContext;  
      
    //这里需要用到ServiceConnection在Context.bindService和context.unBindService()里用到  
    private ServiceConnection mServiceConnection = new ServiceConnection() {  
        //当我bindService时，让TextView显示ComService里getSystemTime()方法的返回值   
        public void onServiceConnected(ComponentName name, IBinder service) {  
            // TODO Auto-generated method stub  
            //mComService = ((ComService.MyBinder)service).getService();  
            //mTextView.setText("I am frome Service :" + mComService.getSystemTime());  

            mIComService = IComService.Stub.asInterface(service);
            try {
                mTextView.setText("I am frome Service aidl:" + mIComService.getSystemTime());
            } catch (RemoteException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
        }  
          
        public void onServiceDisconnected(ComponentName name) {  
            // TODO Auto-generated method stub  
              
        }  
    };  
    public void onCreate(Bundle savedInstanceState) {  
        super.onCreate(savedInstanceState);  
        setContentView(R.layout.main);  
        setupViews();  
    }  
      
    public void setupViews(){  
      
        mContext = MainActivity.this;  
        mTextView = (TextView)findViewById(R.id.text);  
          
          
          
        startServiceButton = (Button)findViewById(R.id.startservice);  
        stopServiceButton = (Button)findViewById(R.id.stopservice);  
        bindServiceButton = (Button)findViewById(R.id.bindservice);  
        unbindServiceButton = (Button)findViewById(R.id.unbindservice);  
          
        startServiceButton.setOnClickListener(this);  
        stopServiceButton.setOnClickListener(this);  
        bindServiceButton.setOnClickListener(this);  
        unbindServiceButton.setOnClickListener(this);  
    }  
     
    public void onClick(View v) {  
        // TODO Auto-generated method stub  
        if(v == startServiceButton){  
            Intent i  = new Intent();  
            i.setClass(MainActivity.this, ComService.class);  
            mContext.startService(i);  
        }else if(v == stopServiceButton){  
            Intent i  = new Intent();  
            i.setClass(MainActivity.this, ComService.class);  
            mContext.stopService(i);  
        }else if(v == bindServiceButton){  
            Intent i  = new Intent();  
            i.setClass(MainActivity.this, ComService.class);  
            mContext.bindService(i, mServiceConnection, BIND_AUTO_CREATE);  
        }else{  
            mContext.unbindService(mServiceConnection);  
        }  
    }  
} 
