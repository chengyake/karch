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

public class MainActivity extends TabActivity {

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);

        Intent i  = new Intent();  
        i.setClass(MainActivity.this, ComService.class);  
        getApplicationContext().startService(i);


        Resources res = getResources(); 
        TabHost tabHost = getTabHost();
        TabHost.TabSpec spec;
        Intent intent;

        intent = new Intent().setClass(this, PUSActivity.class);

        spec = tabHost.newTabSpec("a").setIndicator("PUS",
                res.getDrawable(R.drawable.icon))
            .setContent(intent);
        tabHost.addTab(spec);

        intent = new Intent().setClass(this, PUSActivity.class);
        spec = tabHost.newTabSpec("b").setIndicator("PUM",
                res.getDrawable(R.drawable.icon))
            .setContent(intent);
        tabHost.addTab(spec);

        tabHost.setCurrentTab(0);
    }
}

