package com.cheng;

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

public class aActivity extends Activity {

    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main_taba);
        GridView gridview = (GridView) findViewById(R.id.gridview);

        ArrayList<HashMap<String, Object>> lstImageItem = new ArrayList<HashMap<String, Object>>();
        for(int i=0;i<6;i++)
        {
            HashMap<String, Object> map = new HashMap<String, Object>();
            map.put("ItemImage", R.drawable.play);
            map.put("ItemText", "NO."+String.valueOf(i));
            lstImageItem.add(map);
        }
        SimpleAdapter saImageItems = new SimpleAdapter(this,  lstImageItem, R.layout.night_item,
                new String[] {"ItemImage","ItemText"},  new int[] {R.id.ItemImage,R.id.ItemText});

        gridview.setAdapter(saImageItems);
        gridview.setOnItemClickListener(new ItemClickListener());
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

