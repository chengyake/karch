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

import android.content.ServiceConnection;
import android.content.ComponentName;
import android.os.IBinder;

import android.content.Intent;
import android.content.IntentFilter;  

import android.content.BroadcastReceiver;  
import java.util.HashMap;
import android.view.ContextMenu;
import android.view.MenuItem;
import android.view.ContextMenu.ContextMenuInfo;
import android.view.View.OnCreateContextMenuListener;
import android.widget.AdapterView;
import android.widget.ListView;
import android.widget.SimpleAdapter;
import android.widget.AdapterView.OnItemClickListener;

import android.widget.Toast;

public class PUSErrorDataActivity extends Activity {
    

    private ArrayList<HashMap<String, Object>> listItem;
    private SimpleAdapter listItemAdapter;
    private static int i=0;
    private ListView list;

    private IComService mIComService;

	private Button allerr;
	private Button errdel;
	private Button fileout;

    private ServiceConnection mServiceConnection = new ServiceConnection() {
        public void onServiceConnected(ComponentName name, IBinder service) {  
            mIComService = IComService.Stub.asInterface(service);
        } 

        public void onServiceDisconnected(ComponentName name) {  
        }  
    }; 
    private BroadcastReceiver mBroadcastReceiver = new BroadcastReceiver(){
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            if(action.equals(ComService.ACTION_GET_PUS_ERROR)){
                String info = intent.getExtras().getString("string"); 
                
                StringBuffer dataBuf=new StringBuffer(); 
                dataBuf.append(info);

                if(dataBuf.substring(48, 52).equals("0000")) {
                    //Toast.makeText(getApplicationContext(), "Get PUS Error Log Over and NO Error Item", Toast.LENGTH_SHORT).show();
                    return;
                }

                for(i=0; i<8; i++) {
                   HashMap<String, Object> map = new HashMap<String, Object>();
                   map.put("etime", dataBuf.substring(20, 34));
                   map.put("ecode", dataBuf.substring(48, 52));
                   map.put("estatus", dataBuf.substring(34, 48));
                   map.put("esuppdata", dataBuf.substring(52, 84));
                   listItem.add(0, map);
                }
                listItemAdapter.notifyDataSetChanged();

            } else if(action.equals(ComService.ACTION_DEL_PUS_ERROR)){
                Toast.makeText(getApplicationContext(), "Delete PUS ERROR Over", Toast.LENGTH_SHORT).show();
            } else if (action.equals(ComService.ACTION_FINISH)) {
                Log.e("chengyake", "in PUSConsoleActivity finish");
                finish();
            }
        }

    };


    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.pus_error_data);

        allerr=(Button)findViewById(R.id.all_error);
        errdel=(Button)findViewById(R.id.del);
        fileout=(Button)findViewById(R.id.file_out);

		IntentFilter myIntentFilter = new IntentFilter();
		myIntentFilter.addAction(ComService.ACTION_GET_PUS_ERROR);
		myIntentFilter.addAction(ComService.ACTION_DEL_PUS_ERROR);
		registerReceiver(mBroadcastReceiver, myIntentFilter);


        allerr.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) {

                listItem.clear();
                 try {
                     if(mIComService.getPUSError() != 0) {
                         return;
                     }
                 } catch  (Exception e) {
                     throw new IllegalStateException("getVersionInfo error", e);
                 }


			}
		});
        errdel.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) {
                 try {
                     if(mIComService.delPUSError() != 0) {
                         return;
                     }
                 } catch  (Exception e) {
                     throw new IllegalStateException("getVersionInfo error", e);
                 }


			}
		});




        listItem = new ArrayList<HashMap<String, Object>>();
        list = (ListView) findViewById(R.id.pus_error_data_listview);

        listItemAdapter = new SimpleAdapter(this,listItem, 
                R.layout.pus_error_data_item,
                new String[] {"etime","ecode", "estatus", "esuppdata"}, 
                new int[] {R.id.etime,R.id.ecode,R.id.estatus, R.id.esuppdata}
                );

        list.setAdapter(listItemAdapter);

        list.setOnItemClickListener(new OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int arg2, long arg3) {
                setTitle("点击第"+arg2+"个项目");

                }
        });

        list.setOnCreateContextMenuListener(new OnCreateContextMenuListener() {
            @Override
            public void onCreateContextMenu(ContextMenu menu, View v,ContextMenuInfo menuInfo) {
                menu.setHeaderTitle("长按菜单-ContextMenu");   
                menu.add(0, 0, 0, "弹出长按菜单0");
                menu.add(0, 1, 0, "弹出长按菜单1");   
            }
        }); 

        Intent j  = new Intent();  
        j.setClass(this, ComService.class);
        getApplicationContext().bindService(j, mServiceConnection, BIND_AUTO_CREATE);

    }

    @Override
    public boolean onContextItemSelected(MenuItem item) {

        setTitle("点击了长按菜单里面的第"+item.getItemId()+"个项目"); 

        
        return super.onContextItemSelected(item);
    }


    @Override
    protected void onDestroy() {
        Log.e("chengyake", "Override in PUSErrorDataActivity onDestroy func");
        getApplicationContext().unbindService(mServiceConnection);
        this.unregisterReceiver(mBroadcastReceiver);
        super.onDestroy();
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









