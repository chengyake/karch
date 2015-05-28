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

import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import android.content.Context;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.GridView;
import android.widget.ImageView;

import java.util.ArrayList;
import java.util.HashMap;
import android.util.Log;

import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.SimpleAdapter; 

public class PUSActivity extends Activity {


	private TextView copyright = null;


    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.pus_tab);
        GridView gridview = (GridView) findViewById(R.id.gridview);

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
        map4.put("ItemText", "GetInfo");
        lstImageItem.add(map4);

        SimpleAdapter saImageItems = new SimpleAdapter(this,  lstImageItem, R.layout.icon,
                new String[] {"ItemImage","ItemText"},  new int[] {R.id.ItemImage,R.id.ItemText});

        gridview.setAdapter(saImageItems);
        gridview.setOnItemClickListener(new ItemClickListener());


		copyright = (TextView) findViewById(R.id.copyright);

    }

    class  ItemClickListener implements OnItemClickListener
    {
        public void onItemClick(AdapterView<?> arg0,//The AdapterView where the click happened 
                View arg1,//The view within the AdapterView that was clicked
                int arg2,//The position of the view in the adapter
                long arg3//The row id of the item that was clicked
                ) {
            //HashMap<String, Object> item=(HashMap<String, Object>) arg0.getItemAtPosition(arg2);
            //setTitle((String)item.get("ItemText"));
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
                case 4:
                    intent.setClass(PUSActivity.this, PUSTimeActivity.class);
                    startActivity(intent);
                case 5:
                    intent.setClass(PUSActivity.this, PUSCLPWActivity.class);
                    startActivity(intent);
                    break;
                default:
                    break;
            }

        }

    }

}


