package com.android.cheng;

import android.app.Activity;
import android.widget.EditText;
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
    /** Called when the activity is first created. */
	private EditText mInputText = null;
	private Button mCommit = null;
	private TextView mShowText = null;


    private IComService mIComService;
    private Context mContext;  

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
        
        initView();

        mContext = MainActivity.this;  

        Intent i  = new Intent();  
        i.setClass(MainActivity.this, ComService.class);  
        mContext.startService(i);

        Intent j  = new Intent();  
        j.setClass(MainActivity.this, ComService.class);  
        mContext.bindService(j, mServiceConnection, BIND_AUTO_CREATE);  

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

        try {
            mShowText.setText("I am frome Service aidl:" + mIComService.getSystemTime());
        } catch (RemoteException e) {
            e.printStackTrace();
        }
        //String inputStr = mInputText.getText().toString();
        //mShowText.setText(inputStr);

    }
}
