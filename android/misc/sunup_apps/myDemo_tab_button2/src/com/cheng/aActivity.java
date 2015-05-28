package com.cheng;

import com.cheng.DirChooserDialog;

import android.content.Intent;
import android.view.View;
import android.widget.Button;
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

import java.io.File;
import android.util.Log;
import jxl.Cell;
import jxl.CellReferenceHelper;
import jxl.CellType;
import jxl.Range;
import jxl.Sheet;
import jxl.Workbook;
import jxl.format.CellFormat;
import jxl.format.Colour;
import jxl.format.UnderlineStyle;
import jxl.read.biff.BiffException;
import jxl.write.Blank;
import jxl.write.DateFormat;
import jxl.write.DateFormats;
import jxl.write.DateTime;
import jxl.write.Formula;
import jxl.write.Label;
import jxl.write.Number;
import jxl.write.NumberFormat;
import jxl.write.WritableCell;
import jxl.write.WritableCellFeatures;
import jxl.write.WritableCellFormat;
import jxl.write.WritableFont;
import jxl.write.WritableHyperlink;
import jxl.write.WritableImage;
import jxl.write.WritableSheet;
import jxl.write.WritableWorkbook;
import jxl.write.WriteException;

import android.widget.TextView;
import android.widget.EditText;

import android.app.Notification;
import android.app.NotificationManager;

import android.app.ProgressDialog;
/**
 * @author nwang
 * 
 *	android»ù±¾²¼¾Ö¡¢×é¼þµÈµÈ²Ù×÷£¡
 */
public class aActivity extends Activity {
    /** Called when the activity is first created. */
	
	private Button frameLayout;
	private Button relativeLayout;
	private Button relativeAndLinear;
	private Button tableLayout;
	
    Notification n;  
    NotificationManager nm;   

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main_taba);
        
        frameLayout=(Button)findViewById(R.id.frameLayout);
        relativeLayout=(Button)findViewById(R.id.relativeLayout);
        relativeAndLinear=(Button)findViewById(R.id.relativeAndLinear);
        tableLayout=(Button)findViewById(R.id.tableLayout);
          
        //FrameLayout²¼¾ÖÊ¹ÓÃ
        frameLayout.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) { 
			    /*
				Intent intent=new Intent();
				intent.setClass(MainActivity.this, FrameLayoutActivity.class);
				startActivity(intent);
				*/
                //searchExcel("/sdcard/CONSOLE_ERRORCODE.xls", "2081");
            //myNotify();
            //jindu1();

                    String path = null;  
                    String [] fileType = {"dst"};//要过滤的文件类型列表  
                    DirChooserDialog dlg = new DirChooserDialog(aActivity.this,2,fileType,path);  
                    dlg.setTitle("Choose dst file dir");  
                    dlg.show();  

			}
		});
        
        //RelativeLayout²¼¾ÖÊ¹ÓÃ
        relativeLayout.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
			    /*
				Intent intent=new Intent();
				intent.setClass(MainActivity.this, RelativeLayoutActivity.class);
				startActivity(intent);
				*/
            //createExcel();
            //nm.cancel(1);  
            jindu2();
			}
		});
        
        //RelativeLayoutºÍLinearLayout×ÛºÏÊ¹ÓÃ
        relativeAndLinear.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
			    /*
				Intent intent=new Intent();
				intent.setClass(MainActivity.this, RelativeAndLinearActivity.class);
				startActivity(intent);
				*/
            //new AlertDialog.Builder(aActivity.this, AlertDialog.THEME_HOLO_DARK)  
            new AlertDialog.Builder(aActivity.this)  
            .setTitle("title")
            .setMessage("Message")
            .setPositiveButton("ok", null)
            .show();

			}
		});
        
        //TableLayout²¼¾ÖÊ¹ÓÃ
        tableLayout.setOnClickListener(new View.OnClickListener() {
			
			@Override
			public void onClick(View v) {
			    /*
				Intent intent=new Intent();
				intent.setClass(MainActivity.this, TableLayoutActivity.class);
				startActivity(intent);
				*/

            new AlertDialog.Builder(aActivity.this) 
            .setTitle("确认")
            .setMessage("确定吗？")
            .setPositiveButton("是", new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int i) {
                    Log.e("chengyake", "pppppppppppppp");
                }
            })
            .setNegativeButton("否", new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int i) {
                    Log.e("chengyake", "nnnnnnnnnnnnn");
                }
            })
            .show();

            /*
            new AlertDialog.Builder(aActivity.this)
            .setTitle("请输入")
            .setIcon(android.R.drawable.ic_dialog_info)
            .setView(new EditText(aActivity.this))
            .setPositiveButton("确定", null)
            .setNegativeButton("取消", null)
            .show();*/
			}
		});
        
    }

    private static String searchExcel(String file, String eCode) {

        File inputWorkbook;
        try {
            inputWorkbook = new File(file);
            Workbook book = Workbook.getWorkbook(inputWorkbook);
            book.getNumberOfSheets();  
            Sheet sheet = book.getSheet(0);
            int Rows = sheet.getRows();  
            int Cols = sheet.getColumns();  
            Log.e("chengyake", "当前工作表的名字:" + sheet.getName());  
            Log.e("chengyake", "总行数:" + Rows);  
            Log.e("chengyake", "总列数:" + Cols);  
            for (int i = 0; i < Cols; ++i) {  
                for (int j = 0; j < Rows; ++j) {  
                    Log.e("chengyake", (sheet.getCell(i, j)).getContents() + "\t");  
                }  
            }  
            Cell cell1 = sheet.getCell(0, 0);  
            String result = cell1.getContents();  
            System.out.println(result);  
            book.close();  
        } catch (Exception e) {  
            System.out.println(e);  
        }  

        return null;
    }



    public void createExcel() {  
        try {  
            WritableWorkbook book = Workbook.createWorkbook(new File(
                    "/sdcard/test.xls"));  
  
            WritableSheet sheet1 = book.createSheet("第一页", 0);  
            WritableSheet sheet2 = book.createSheet("第三页", 2);  
  
            Label label = new Label(0, 0, "test");  
  
            sheet1.addCell(label);  
  
 
            jxl.write.Number number = new jxl.write.Number(1, 0, 555.12541);  
            sheet2.addCell(number);  
  
            book.write();  
            book.close();  
        } catch (Exception e) {  
            System.out.println(e);  
        }  
    }  

    public void myNotify() {

        nm = (NotificationManager)this.getSystemService(NOTIFICATION_SERVICE);  

        n = new Notification();
        int icon = R.drawable.icon;
        String tickerText = "Test Notifaction";
        long when = System.currentTimeMillis();

        n.icon = icon;
        n.tickerText = tickerText;
        n.when = when;
        n.flags = Notification.FLAG_NO_CLEAR;  
        n.flags = Notification.FLAG_ONGOING_EVENT;   
        n.setLatestEventInfo(aActivity.this, "My Title", "My Content", null);  
        nm.notify(1, n);  

    }

    public void jindu1() {
        ProgressDialog dialog = new ProgressDialog(this); 
        dialog.setProgressStyle(ProgressDialog.STYLE_SPINNER); 
        dialog.setTitle("进度对话框"); 
        dialog.setMessage("圆形进度条"); 
        dialog.setIcon(android.R.drawable.ic_dialog_map); 
        dialog.setButton("确定", new ProgressDialog.OnClickListener(){ 
            @Override 
            public void onClick(DialogInterface dialog, int which) { 
                
            } 
        });
        dialog.setIndeterminate(false); 
        dialog.setCancelable(false); //jinzhi
        dialog.show(); 
    }


    public void jindu2() {

        ProgressDialog dialog = new ProgressDialog(this); 
        dialog.setProgressStyle(ProgressDialog.STYLE_HORIZONTAL); 
        dialog.setTitle("进度对话框"); 
        dialog.setMessage("长方形进度条"); 
        dialog.setIcon(android.R.drawable.ic_dialog_alert); 
        dialog.setMax(100); 
        dialog.setButton("确定", new ProgressDialog.OnClickListener(){ 
            @Override 
            public void onClick(DialogInterface dialog, int which) { 
                
            } 
        }); 
        dialog.setCancelable(true); 
        dialog.show(); 
        dialog.setProgress(50); 
    }
}








