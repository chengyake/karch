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
import android.view.View.OnFocusChangeListener;

public class JobDataActivity extends Activity {
    
    EditText edit1 = null;
    EditText edit2 = null;
    EditText edit3 = null;
    EditText edit4 = null;
    EditText edit5 = null;
    EditText edit6 = null;
    EditText edit7 = null;
    EditText edit8 = null;
    EditText edit9 = null;
    EditText edit10 = null;

    EditText edit_addr = null;
    EditText edit_size = null;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		
		setContentView(R.layout.jobdata);


        edit1 = (EditText)findViewById(R.id.edit1);
        edit1.setInputType(InputType.TYPE_NULL);
        edit2 = (EditText)findViewById(R.id.edit2);
        edit2.setInputType(InputType.TYPE_NULL);
        edit3 = (EditText)findViewById(R.id.edit3);
        edit3.setInputType(InputType.TYPE_NULL);
        edit4 = (EditText)findViewById(R.id.edit4);
        edit4.setInputType(InputType.TYPE_NULL);
        edit5 = (EditText)findViewById(R.id.edit5);
        edit5.setInputType(InputType.TYPE_NULL);
        edit6 = (EditText)findViewById(R.id.edit6);
        edit6.setInputType(InputType.TYPE_NULL);
        edit7 = (EditText)findViewById(R.id.edit7);
        edit7.setInputType(InputType.TYPE_NULL);
        edit8 = (EditText)findViewById(R.id.edit8);
        edit8.setInputType(InputType.TYPE_NULL);
        edit9 = (EditText)findViewById(R.id.edit9);
        edit9.setInputType(InputType.TYPE_NULL);
        edit10 = (EditText)findViewById(R.id.edit10);
        edit10.setInputType(InputType.TYPE_NULL);


        edit_size = (EditText)findViewById(R.id.edit_size);
        edit_size.setInputType(InputType.TYPE_NULL);

        edit_addr = (EditText)findViewById(R.id.edit_addr);
        edit_addr.setInputType(InputType.TYPE_NULL);
        edit_addr.setOnFocusChangeListener(new OnFocusChangeListener() {
            public void onFocusChange(View v, boolean hasFocus) {
                if (hasFocus) {
                    edit_addr.setBackgroundColor(Color.RED);
                } else {
                    edit_addr.setBackgroundColor(Color.WHITE);
                }
            }
        });


	}



}





/*
EditText editor = ....;
LayoutParams params = new LinearLayout.LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT, 1);
editor.setLayoutParams(params);
editor.setGravity(Gravity.LEFT | Gravity.TOP);
editor.setBackgroundColor(Color.WHITE);
editor.setImeOptions(EditorInfo.IME_FLAG_NO_EXTRACT_UI);
editor.setTextColor(Color.BLACK);
editor.setFocusableInTouchMode(true);
editor.setInputType(EditorInfo.TYPE_CLASS_TEXT | EditorInfo.TYPE_TEXT_FLAG_MULTI_LINE | EditorInfo.TYPE_TEXT_FLAG_IME_MULTI_LINE);
editor.setMaxLines(Integer.MAX_VALUE);
editor.setHorizontallyScrolling(false);
editor.setTransformationMethod(null);
*/









