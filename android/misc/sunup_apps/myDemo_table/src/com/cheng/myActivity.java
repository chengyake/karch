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
import android.widget.LinearLayout;
import android.widget.ListView;

import java.util.ArrayList;
import java.util.List;
import java.util.HashMap;
import java.util.Map;

public class myActivity extends TabActivity {
    
    ListView tableListView;
    List<Map<String, ?>> data;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.lstview);
        initData();
        LinearLayout titleLayout = (LinearLayout) this.findViewById(R.id.title_layout);
        tableListView = (ListView) this.findViewById(R.id.table_listView);
        aActivity adapter = new aActivity(myActivity.this, data, R.layout.list_item, 
                new String[]{ "id","name","dest"}, 
                new int[] { R.id.code,R.id.name,R.id.dest},
                new String[]{"订单号","客户名称","目的地"} );

        titleLayout.addView(adapter.getTitleView());
        tableListView.setAdapter(adapter);
        }
    
    public void initData() {
        

        data = new ArrayList<Map<String, ?>>();
        for (int i = 0; i < 10; i++) {
            Map<String, String> m = new HashMap<String, String>();
            m.put("id", "id" + i);
            m.put("name", "name" + i);
            m.put("dest", "dest" + i);
            data.add(m);
        }

    }


}
