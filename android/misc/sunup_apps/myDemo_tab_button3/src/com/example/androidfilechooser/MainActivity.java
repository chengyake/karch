package com.example.androidfilechooser;

import android.os.Bundle;
import android.os.Environment;
import android.app.Activity;
import android.content.Intent;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.widget.Button;
import android.widget.Toast;

public class MainActivity extends Activity implements View.OnClickListener {
	
	private static String TAG = "MainActivity";
	
	private Button mBtOpenFile ;

	private Intent fileChooserIntent ;
	
	private static final int REQUEST_CODE = 1;
	public static final String EXTRA_FILE_CHOOSER = "file_chooser";
	
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		setContentView(R.layout.main);
		
		mBtOpenFile = (Button)findViewById(R.id.btOpenFile);
		mBtOpenFile.setOnClickListener(this);
		
		fileChooserIntent =  new Intent(this , 
					FileChooserActivity.class);
	}

	public void onClick(View v){
		switch(v.getId()){
		    case R.id.btOpenFile :
		    	if(Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED))
				    startActivityForResult(fileChooserIntent , REQUEST_CODE);
		    	else
		    		toast(getText(R.string.sdcard_unmonted_hint));
				break ;
	        default :
	    	    break ;
		}
	}
	
	private void toast(CharSequence hint){
	    Toast.makeText(this, hint , Toast.LENGTH_SHORT).show();
	}
	
	public void onActivityResult(int requestCode , int resultCode , Intent data){
		
		Log.v(TAG, "onActivityResult#requestCode:"+ requestCode  + "#resultCode:" +resultCode);
		if(resultCode == RESULT_CANCELED){
			toast(getText(R.string.open_file_none));
			return ;
		}
		if(resultCode == RESULT_OK && requestCode == REQUEST_CODE){
			//获取路径名
			String pptPath = data.getStringExtra(EXTRA_FILE_CHOOSER);
			Log.v(TAG, "onActivityResult # pptPath : "+ pptPath );
			if(pptPath != null){
				toast("Choose File : " + pptPath);
			}
			else
				toast(getText(R.string.open_file_failed));
		}
	}

}
