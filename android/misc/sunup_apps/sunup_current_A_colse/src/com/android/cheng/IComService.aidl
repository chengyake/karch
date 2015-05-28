package com.android.cheng;


interface IComService {
    int probeCp2102(); 
    String getSystemTime();
    int writeAsync(String data );
    String getUartStatus();
    String getCPUStatus(); 
    int getVersionInfo(); 
    int heartBeats(); 
    int intoPUS(); 
    int exitPUS(); 
    int intoPUM(); 
    int exitPUM(); 
    int uploadPUSConsole(String addr, String size); 
    int downloadPUSConsole(); 
    int getPUSError(); 
    int delPUSError(); 
    int uploadPUSJobdata(); 
    int downloadPUSJobdata(); 
    int getPUSTime(); 
    int setPUSTime(); 
    int uploadPUMConsole(); 
    int downloadPUMConsole(); 
    int getPUMError(); 
    int delPUMError(); 
    int uploadPUMJobdata(); 
    int downloadPUMJobdata(); 

}
