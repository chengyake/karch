package com.cheng;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.TabActivity;
import android.content.DialogInterface;
import android.graphics.Color;
import android.os.Bundle;
import android.widget.TabHost;
import android.widget.TabWidget;
import android.widget.Toast;
import android.widget.TextView;
import android.widget.TabHost.OnTabChangeListener;
import android.content.res.Resources;
import android.content.Intent;

public class myActivity extends TabActivity {

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);

        Resources res = getResources(); // Resource object to get Drawables
        TabHost tabHost = getTabHost();  // The activity TabHost
        TabHost.TabSpec spec;  // Resusable TabSpec for each tab
        Intent intent;  // Reusable Intent for each tab

        TabWidget tabWidget = tabHost.getTabWidget();

        // Create an Intent to launch an Activity for the tab (to be reused)
        intent = new Intent().setClass(this, aActivity.class);

        // Initialize a TabSpec for each tab and add it to the TabHost
        spec = tabHost.newTabSpec("a").setIndicator("A", null).setContent(intent);
        tabHost.addTab(spec);

        // Do the same for the other tabs
        intent = new Intent().setClass(this, bActivity.class);
        spec = tabHost.newTabSpec("b").setIndicator("B", null).setContent(intent);
        tabHost.addTab(spec);

        for (int i =0; i < tabWidget.getChildCount(); i++) {
            tabWidget.getChildAt(i).getLayoutParams().height = 80;
            tabWidget.getChildAt(i).getLayoutParams().width = 125;
            TextView tv = (TextView) tabWidget.getChildAt(i).findViewById(android.R.id.title);
            tv.setTextSize(40);
            tv.setTextColor(this.getResources().getColorStateList(android.R.color.white));
            tv.setBackgroundColor(Color.RED);
            //tabWidget.setBackgroundResource(R.drawable.background);
        }

        tabHost.setCurrentTab(0);
    }
}


