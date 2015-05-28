package com.cheng;  

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

public class myActivity extends Activity {

    private ArrayList<HashMap<String, Object>> listItem;
    private SimpleAdapter listItemAdapter;
    private static int i=0;
    private ListView list;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);

        listItem = new ArrayList<HashMap<String, Object>>();
        list = (ListView) findViewById(R.id.ListView01);
        for(i=0; i<15; i++)
        {
            HashMap<String, Object> map = new HashMap<String, Object>();
            map.put("ItemImage", "chengyake");
            map.put("ItemTitle", "Level "+i);
            map.put("ItemText", "Fine");
            listItem.add(map);
        }
        listItemAdapter = new SimpleAdapter(this,listItem, 
                R.layout.listview,
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



