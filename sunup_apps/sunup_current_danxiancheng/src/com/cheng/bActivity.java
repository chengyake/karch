package com.cheng;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

/**
 * @author nwang
 * 
 *	android»ù±¾²¼¾Ö¡¢×é¼þµÈµÈ²Ù×÷£¡
 */
public class bActivity extends Activity {
    /** Called when the activity is first created. */
	
	private Button frameLayout;
	private Button relativeLayout;
	private Button relativeAndLinear;
	private Button tableLayout;
	
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
			    /*
				Intent intent=new Intent();
				intent.setClass(MainActivity.this, FrameLayoutActivity.class);
				startActivity(intent);
				*/
			}
		});
        
        //RelativeLayout²¼¾ÖÊ¹ÓÃ
        relativeLayout.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
			    /*
				Intent intent=new Intent();
				intent.setClass(MainActivity.this, RelativeLayoutActivity.class);
				startActivity(intent);
				*/
			}
		});
        
        //RelativeLayoutºÍLinearLayout×ÛºÏÊ¹ÓÃ
        relativeAndLinear.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
			    /*
				Intent intent=new Intent();
				intent.setClass(MainActivity.this, RelativeAndLinearActivity.class);
				startActivity(intent);
				*/
			}
		});
        
        //TableLayout²¼¾ÖÊ¹ÓÃ
        tableLayout.setOnClickListener(new View.OnClickListener() {
			
			@Override
			public void onClick(View v) {
			    /*
				Intent intent=new Intent();
				intent.setClass(MainActivity.this, TableLayoutActivity.class);
				startActivity(intent);
				*/
			}
		});
        
    }
}
