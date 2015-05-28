package com.android.jni;

import com.android.jni.HexDump;
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

import java.io.IOException;


public class JNIActivity extends Activity {

   
	private EditText mInputText = null;
	private TextView mShowText = null;
	private Button mGen = null;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.main);
		mInputText = (EditText) findViewById(R.id.inputText);
		mShowText = (TextView) findViewById(R.id.showText);
		mGen = (Button) findViewById(R.id.gen);

        mGen.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) {
            String data = mInputText.getText().toString();
            if(data.length()!=8) {
			    mShowText.setText("ID Error");
            } else {
			    mShowText.setText(accZz(data));
            }
			}
		});
    }


    private String accZz(String devId) {
        //35752305150042929
        int i;
        byte[] bufs = HexDump.hexStringToByteArray(devId);
        byte buf1 = 0;
        byte buf2 = 0;
        byte buf3 = 0;

        for(i=0; i<bufs.length; i++) {
            buf1=(byte)(buf1 ^ bufs[i]);
            buf2=(byte)(buf2 + bufs[i] + (byte)i);
            buf3=(byte)(buf2 + buf3 + bufs[i] );
        }


        StringBuffer IdBuf=new StringBuffer(); 
        IdBuf.append(devId);

        return ("" + HexDump.toHexString(buf1) + IdBuf.substring(0,1) + HexDump.toHexString(buf2) + IdBuf.substring(1,2) + HexDump.toHexString(buf3));

    }

}


