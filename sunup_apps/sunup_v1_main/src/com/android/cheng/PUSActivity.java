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

import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.SimpleAdapter; 

public class PUSActivity extends Activity {


	private TextView pusInfo = null;
	private TextView pumInfo = null;
	private TextView jobInfo = null;
	private TextView jobDateInfo = null;
	private TextView pusContent = null;
	private TextView pumContent = null;
	private TextView jobContent = null;
	private TextView jobDateContent = null;
	private TextView copyright = null;


    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.pustab);
        GridView gridview = (GridView) findViewById(R.id.gridview);

        ArrayList<HashMap<String, Object>> lstImageItem = new ArrayList<HashMap<String, Object>>();
        /*
        for(int i=0; i<6; i++)
        {
            HashMap<String, Object> map = new HashMap<String, Object>();
            map.put("ItemImage", R.drawable.icon);
            map.put("ItemText", "NO."+String.valueOf(i));
            lstImageItem.add(map);
        }
        */
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
        //Program
        HashMap<String, Object> map4 = new HashMap<String, Object>();
        map4.put("ItemImage", R.drawable.program);
        map4.put("ItemText", "Program");
        lstImageItem.add(map4);
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


        SimpleAdapter saImageItems = new SimpleAdapter(this,  lstImageItem, R.layout.icon,
                new String[] {"ItemImage","ItemText"},  new int[] {R.id.ItemImage,R.id.ItemText});

        gridview.setAdapter(saImageItems);
        gridview.setOnItemClickListener(new ItemClickListener());

		pusInfo = (TextView) findViewById(R.id.pusinfo);
		pumInfo = (TextView) findViewById(R.id.puminfo);
		jobInfo = (TextView) findViewById(R.id.jobinfo);
		jobDateInfo = (TextView) findViewById(R.id.jobdateinfo);
		pusContent = (TextView) findViewById(R.id.puscontent);
		pumContent = (TextView) findViewById(R.id.pumcontent);
		jobContent = (TextView) findViewById(R.id.jobcontent);
		jobDateContent = (TextView) findViewById(R.id.jobdatecontent);
		copyright = (TextView) findViewById(R.id.copyright);

        pusInfo.setText("PUS : ");
        pumInfo.setText("PUM: ");
        jobInfo.setText("JOB : ");
        jobDateInfo.setText("JOB Date:");
        pusContent.setText("\t\t\t01.56");
        pumContent.setText("\t\t\t01.50");
        jobContent.setText("\t\t\tGA121993-190");
        jobDateContent.setText("\t2014-04-06");

    }

    class  ItemClickListener implements OnItemClickListener
    {
        public void onItemClick(AdapterView<?> arg0,//The AdapterView where the click happened 
                View arg1,//The view within the AdapterView that was clicked
                int arg2,//The position of the view in the adapter
                long arg3//The row id of the item that was clicked
                ) {
            HashMap<String, Object> item=(HashMap<String, Object>) arg0.getItemAtPosition(arg2);
            setTitle((String)item.get("ItemText"));
        }

    }

}


