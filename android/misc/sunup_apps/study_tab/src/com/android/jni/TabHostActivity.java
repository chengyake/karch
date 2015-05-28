package com.ljq.activity;

import android.app.TabActivity;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.widget.TabHost;

public class TabHostActivity extends TabActivity{
    private TabHost tab=null;
    
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        tab=this.getTabHost();
        LayoutInflater.from(this).inflate(R.layout.main, tab.getTabContentView(), true);
        tab.addTab(tab.newTabSpec("选项卡一").setIndicator("选项卡一", 
                   getResources().getDrawable(R.drawable.png1)).setContent(R.id.tab01));
        tab.addTab(tab.newTabSpec("选项卡二").setIndicator("选项卡二", 
                getResources().getDrawable(R.drawable.png2)).setContent(R.id.tab02));
        tab.addTab(tab.newTabSpec("选项卡三").setIndicator("选项卡三", 
                getResources().getDrawable(R.drawable.png3)).setContent(R.id.tab03));
    }
}
