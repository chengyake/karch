package com.android.cheng;

import android.os.Handler;
import android.os.Message;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import android.content.Context;
import android.text.InputType;

import android.util.Log;
import java.util.List;

import java.io.IOException;
import android.graphics.Color;
import android.view.View.OnFocusChangeListener;


import java.util.ArrayList;
import java.util.HashMap;

import android.app.Activity;
import android.os.Bundle;
import android.view.ContextMenu;
import android.view.MenuItem;
import android.view.View;
import android.view.ContextMenu.ContextMenuInfo;
import android.view.View.OnCreateContextMenuListener;
import android.widget.AdapterView;
import android.widget.ListView;
import android.widget.SimpleAdapter;
import android.widget.AdapterView.OnItemClickListener;


public class ConsoleActivity extends Activity {
    

    private ArrayList<HashMap<String, Object>> listItem;
    private SimpleAdapter listItemAdapter;
    private static int i=0;
    private ListView list;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);

        listItem = new ArrayList<HashMap<String, Object>>();
        list = (ListView) findViewById(R.id.consoleList);
        for(i=0; i<15; i++)
        {
            HashMap<String, Object> map = new HashMap<String, Object>();
            map.put("ItemImage", "chengyake");
            map.put("ItemTitle", "Level "+i);
            map.put("ItemText", "Fine");
            listItem.add(map);
        }
        listItemAdapter = new SimpleAdapter(this,listItem, 
                R.layout.consolelistview,
                new String[] {"ItemImage","ItemTitle", "ItemText"}, 
                new int[] {R.id.ItemImage,R.id.ItemTitle,R.id.ItemText}
                );

        list.setAdapter(listItemAdapter);

        list.setOnItemClickListener(new OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> arg0, View arg1, int arg2, long arg3) {
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
    }

    @Override
    public boolean onContextItemSelected(MenuItem item) {

        setTitle("点击了长按菜单里面的第"+item.getItemId()+"个项目"); 
        
        return super.onContextItemSelected(item);
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









