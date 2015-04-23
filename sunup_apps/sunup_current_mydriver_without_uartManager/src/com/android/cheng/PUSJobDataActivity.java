package com.android.cheng;

import android.os.Handler;
import android.os.Message;
import android.os.Bundle;
import android.os.IBinder;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.text.InputType;
import android.util.Log;
import java.io.IOException;
import android.graphics.Color;
import android.app.Activity;
import android.view.MenuItem;
import android.view.ContextMenu;
import android.view.ContextMenu.ContextMenuInfo;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnCreateContextMenuListener;
import android.view.View.OnFocusChangeListener;
import android.widget.AdapterView;
import android.widget.ListView;
import android.widget.SimpleAdapter;
import android.widget.AdapterView.OnItemClickListener;
import android.content.Context;
import android.content.ServiceConnection;
import android.content.ComponentName;

import java.util.List;
import java.util.Map;
import java.util.HashMap;
import java.util.ArrayList;
import java.util.HashMap;
import android.content.BroadcastReceiver;  

import android.view.ViewGroup;
import android.content.Intent;
import android.content.IntentFilter;  

public class PUSJobDataActivity extends Activity {

    private ArrayList<HashMap<String, Object>> listItem = new ArrayList<HashMap<String, Object>>();
    private MyAdapter listItemAdapter;
    private static int i=0;
    private ListView list;
    private IComService mIComService;

	private Button upload;
	private Button download;
	private Button keyA;
	private Button keyB;
	private Button keyC;
	private Button keyD;
	private Button keyE;
	private Button keyF;
	private Button key0;
	private Button key1;
	private Button key2;
	private Button key3;
	private Button key4;
	private Button key5;
	private Button key6;
	private Button key7;
	private Button key8;
	private Button key9;
	private EditText addr;
	private EditText size;

    private String mAddr="";
    private String mSize="";

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
            if(action.equals(ComService.ACTION_GET_INFO)){
                String info = intent.getExtras().getString("string"); 
            } else if (action.equals(ComService.ACTION_FINISH)) {
                Log.e("chengyake", "in PUSJobDataActivity finish");
                finish();
            }
        }

    };

    private class MyAdapter extends SimpleAdapter {
        int currentPos = -1;
        int count = 0;
        private ArrayList<HashMap<String, Object>> mItemList;
        public MyAdapter(Context context, ArrayList<HashMap<String, Object>> data,
                int resource, String[] from, int[] to) {
            super(context, data, resource, from, to);
            mItemList = (ArrayList<HashMap<String, Object>>) data;
            if(data == null) {
                count = 0;
            } else {
                count = data.size();
            }
        }

        public void setCurrent(int pos) {
            currentPos = pos;
        }

        public int getCount() {
            return mItemList.size();
        }

        public Object getItem(int pos) {
            return pos;
        }

        public long getItemId(int pos) {
            return pos;
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            View view = super.getView(position, convertView, parent);
            TextView mNew = (TextView)view.findViewById(R.id.ItemNew);
            TextView mOld = (TextView)view.findViewById(R.id.ItemOld);

            if((!mNew.getText().toString().equals("")) && 
                    (!mNew.getText().toString().equals(mOld.getText().toString()))) {

                mNew.setTextColor(Color.RED);
                mNew.setText(mOld.getText().toString());

            }

            if(position == currentPos)  {
                mNew.setTextColor(Color.GREEN);
            }
            return view;
        }
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.pus_job_data);

		IntentFilter myIntentFilter = new IntentFilter();
		myIntentFilter.addAction(ComService.ACTION_FINISH);
		registerReceiver(mBroadcastReceiver, myIntentFilter);


        list = (ListView) findViewById(R.id.pus_console_listview);
        upload=(Button)findViewById(R.id.pus_console_upload);
        download=(Button)findViewById(R.id.pus_console_download);
        keyA=(Button)findViewById(R.id.pus_console_keyA);
        keyB=(Button)findViewById(R.id.pus_console_keyB);
        keyC=(Button)findViewById(R.id.pus_console_keyC);
        keyD=(Button)findViewById(R.id.pus_console_keyD);
        keyE=(Button)findViewById(R.id.pus_console_keyE);
        keyF=(Button)findViewById(R.id.pus_console_keyF);
        key0=(Button)findViewById(R.id.pus_console_key0);
        key1=(Button)findViewById(R.id.pus_console_key1);
        key2=(Button)findViewById(R.id.pus_console_key2);
        key3=(Button)findViewById(R.id.pus_console_key3);
        key4=(Button)findViewById(R.id.pus_console_key4);
        key5=(Button)findViewById(R.id.pus_console_key5);
        key6=(Button)findViewById(R.id.pus_console_key6);
        key7=(Button)findViewById(R.id.pus_console_key7);
        key8=(Button)findViewById(R.id.pus_console_key8);
        key9=(Button)findViewById(R.id.pus_console_key9);

        addr=(EditText)findViewById(R.id.pus_console_addr);
        size=(EditText)findViewById(R.id.pus_console_size);

        addr.setInputType(InputType.TYPE_NULL);
        size.setInputType(InputType.TYPE_NULL);

        addr.setOnFocusChangeListener(new View.OnFocusChangeListener() { 
           @Override 
           public void onFocusChange(View v, boolean hasFocus) { 
                if(hasFocus){ 
                    addr.setBackgroundColor(Color.GREEN);
                } else {
                    addr.setBackgroundColor(Color.WHITE);
                } 
           } 
        });

        size.setOnFocusChangeListener(new View.OnFocusChangeListener() { 
           @Override 
           public void onFocusChange(View v, boolean hasFocus) { 
                if(hasFocus){ 
                    size.setBackgroundColor(Color.GREEN);
                } else {
                    size.setBackgroundColor(Color.WHITE);
                } 
           } 
        });

        upload.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) { 
               try {
                   //mIComService.uploadPUSJobData(mAddr, mSize);
                   //list.setSelection(14);
                   //if(size.hasFocus()) {
               } catch  (Exception e) {
                   throw new IllegalStateException("upload error", e);
               }
			}
		});
        download.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) { 
               try {
                   //mIComService.downloadPUSJobData();
               } catch  (Exception e) {
                   throw new IllegalStateException("download error", e);
               }
			}
		});

		//key 0 - key 9 key A- key F
        keyA.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) { 
               try {
                   if(addr.hasFocus() && mAddr.length()<8 ) {
                       mAddr=mAddr + "A";
                       addr.setText(mAddr);
                   }
                   if(size.hasFocus() && mSize.length()<4) {
                       mSize=mSize + "A";
                       size.setText(mSize);
                   }
               } catch  (Exception e) {
                   throw new IllegalStateException("keyA error", e);
               }
			}
		});

        keyB.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) { 
               try {
                   if(addr.hasFocus() && mAddr.length()<8 ) {
                       mAddr=mAddr + "B";
                       addr.setText(mAddr);
                   }
                   if(size.hasFocus() && mSize.length()<4) {
                       mSize=mSize + "B";
                       size.setText(mSize);
                   }
               } catch  (Exception e) {
                   throw new IllegalStateException("keyB error", e);
               }
			}
		});

        keyC.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) { 
               try {
                   if(addr.hasFocus() && mAddr.length()<8 ) {
                       mAddr=mAddr + "C";
                       addr.setText(mAddr);
                   }
                   if(size.hasFocus() && mSize.length()<4) {
                       mSize=mSize + "C";
                       size.setText(mSize);
                   }
               } catch  (Exception e) {
                   throw new IllegalStateException("keyC error", e);
               }
			}
		});

        keyD.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) { 
               try {
                   if(addr.hasFocus() && mAddr.length()<8 ) {
                       mAddr=mAddr + "D";
                       addr.setText(mAddr);
                   }
                   if(size.hasFocus() && mSize.length()<4) {
                       mSize=mSize + "D";
                       size.setText(mSize);
                   }
               } catch  (Exception e) {
                   throw new IllegalStateException("keyA error", e);
               }
			}
		});

        keyE.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) { 
               try {
                   if(addr.hasFocus() && mAddr.length()<8 ) {
                       mAddr=mAddr + "E";
                       addr.setText(mAddr);
                   }
                   if(size.hasFocus() && mSize.length()<4) {
                       mSize=mSize + "E";
                       size.setText(mSize);
                   }
               } catch  (Exception e) {
                   throw new IllegalStateException("keyA error", e);
               }
			}
		});

        keyF.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) { 
               try {
                   if(addr.hasFocus() && mAddr.length()<8 ) {
                       mAddr=mAddr + "F";
                       addr.setText(mAddr);
                   }
                   if(size.hasFocus() && mSize.length()<4) {
                       mSize=mSize + "F";
                       size.setText(mSize);
                   }
               } catch  (Exception e) {
                   throw new IllegalStateException("keyA error", e);
               }
			}
		});

        key0.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) { 
               try {
                   if(addr.hasFocus() && mAddr.length()<8 ) {
                       mAddr=mAddr + "0";
                       addr.setText(mAddr);
                   }
                   if(size.hasFocus() && mSize.length()<4) {
                       mSize=mSize + "0";
                       size.setText(mSize);
                   }
               } catch  (Exception e) {
                   throw new IllegalStateException("key0 error", e);
               }
			}
		});

        key0.setOnLongClickListener(new View.OnLongClickListener() {
            @Override
			public boolean onLongClick(View v) { 
               try {
                   if(addr.hasFocus()) {
                       mAddr="";
                       addr.setText("0");
                   }
                   if(size.hasFocus()) {
                       mSize="";
                       size.setText("0");
                   }
               } catch  (Exception e) {
                   throw new IllegalStateException("key0 error", e);
               }

               return true;
			}
		});

        key1.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) { 
               try {
                   if(addr.hasFocus() && mAddr.length()<8 ) {
                       mAddr=mAddr + "1";
                       addr.setText(mAddr);
                   }
                   if(size.hasFocus() && mSize.length()<4) {
                       mSize=mSize + "1";
                       size.setText(mSize);
                   }
               } catch  (Exception e) {
                   throw new IllegalStateException("key1 error", e);
               }
			}
		});

        key2.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) { 
               try {
                   if(addr.hasFocus() && mAddr.length()<8 ) {
                       mAddr=mAddr + "2";
                       addr.setText(mAddr);
                   }
                   if(size.hasFocus() && mSize.length()<4) {
                       mSize=mSize + "2";
                       size.setText(mSize);
                   }
               } catch  (Exception e) {
                   throw new IllegalStateException("key2 error", e);
               }
			}
		});

        key3.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) { 
               try {
                   if(addr.hasFocus() && mAddr.length()<8 ) {
                       mAddr=mAddr + "3";
                       addr.setText(mAddr);
                   }
                   if(size.hasFocus() && mSize.length()<4) {
                       mSize=mSize + "3";
                       size.setText(mSize);
                   }
               } catch  (Exception e) {
                   throw new IllegalStateException("key3 error", e);
               }
			}
		});
        key4.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) { 
               try {
                   if(addr.hasFocus() && mAddr.length()<8 ) {
                       mAddr=mAddr + "4";
                       addr.setText(mAddr);
                   }
                   if(size.hasFocus() && mSize.length()<4) {
                       mSize=mSize + "4";
                       size.setText(mSize);
                   }
               } catch  (Exception e) {
                   throw new IllegalStateException("key4 error", e);
               }
			}
		});

        key5.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) { 
               try {
                   if(addr.hasFocus() && mAddr.length()<8 ) {
                       mAddr=mAddr + "5";
                       addr.setText(mAddr);
                   }
                   if(size.hasFocus() && mSize.length()<4) {
                       mSize=mSize + "5";
                       size.setText(mSize);
                   }
               } catch  (Exception e) {
                   throw new IllegalStateException("key5 error", e);
               }
			}
		});

        key6.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) { 
               try {
                   if(addr.hasFocus() && mAddr.length()<8 ) {
                       mAddr=mAddr + "6";
                       addr.setText(mAddr);
                   }
                   if(size.hasFocus() && mSize.length()<4) {
                       mSize=mSize + "6";
                       size.setText(mSize);
                   }
               } catch  (Exception e) {
                   throw new IllegalStateException("key6 error", e);
               }
			}
		});

        key7.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) { 
               try {
                   if(addr.hasFocus() && mAddr.length()<8 ) {
                       mAddr=mAddr + "7";
                       addr.setText(mAddr);
                   }
                   if(size.hasFocus() && mSize.length()<4) {
                       mSize=mSize + "7";
                       size.setText(mSize);
                   }
               } catch  (Exception e) {
                   throw new IllegalStateException("key7 error", e);
               }
			}
		});

        key8.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) { 
               try {
                   if(addr.hasFocus() && mAddr.length()<8 ) {
                       mAddr=mAddr + "8";
                       addr.setText(mAddr);
                   }
                   if(size.hasFocus() && mSize.length()<4) {
                       mSize=mSize + "8";
                       size.setText(mSize);
                   }
               } catch  (Exception e) {
                   throw new IllegalStateException("key8 error", e);
               }
			}
		});

        key9.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) { 
               try {
                   if(addr.hasFocus() && mAddr.length()<8 ) {
                       mAddr=mAddr + "9";
                       addr.setText(mAddr);
                   }
                   if(size.hasFocus() && mSize.length()<4) {
                       mSize=mSize + "9";
                       size.setText(mSize);
                   }
               } catch  (Exception e) {
                   throw new IllegalStateException("key9 error", e);
               }
			}
		});


        //for(i=0; i<15; i++)
        {
            HashMap<String, Object> map = new HashMap<String, Object>();
            map.put("ItemAddr", "org");
            map.put("ItemOld", "1");
            map.put("ItemNew", "");
            listItem.add(map);
        }

        listItemAdapter = new MyAdapter(this,listItem, 
                R.layout.pus_console_item,
                new String[] {"ItemAddr","ItemOld", "ItemNew"}, 
                new int[] {R.id.ItemAddr,R.id.ItemOld,R.id.ItemNew}
                );

        list.setAdapter(listItemAdapter);

        list.setOnItemClickListener(new OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int arg2, long arg3) {
            //setTitle("点击第"+arg2+"个项目");
            listItem.remove(0);
            HashMap<String, Object> map = new HashMap<String, Object>();
            map.put("ItemAddr", "insert");
            map.put("ItemOld", "2");
            map.put("ItemNew", "");
            listItem.add(0, map);
            //listItemAdapter.setCurrent(0);
            listItemAdapter.notifyDataSetChanged();
            }

        });

        list.setOnCreateContextMenuListener(new OnCreateContextMenuListener() {
            @Override
            public void onCreateContextMenu(ContextMenu menu, View v,ContextMenuInfo menuInfo) {
                //menu.setHeaderTitle("长按菜单-ContextMenu");   

                HashMap<String, Object> map = new HashMap<String, Object>();
                map.put("ItemAddr", "add");
                map.put("ItemOld", "3");
                map.put("ItemNew", "");
                listItem.add(map);
                listItemAdapter.mItemList = listItem;

                listItemAdapter.notifyDataSetChanged();
            }
        }); 



        Intent j  = new Intent();  
        j.setClass(PUSJobDataActivity.this, ComService.class);
        getApplicationContext().bindService(j, mServiceConnection, BIND_AUTO_CREATE);

        Log.e("chengyake", "in PUSJobDataActivity onCreate");
    }

    @Override
    public boolean onContextItemSelected(MenuItem item) {
        setTitle("长按"+item.getItemId()); 

        
        return super.onContextItemSelected(item);
    }

}

