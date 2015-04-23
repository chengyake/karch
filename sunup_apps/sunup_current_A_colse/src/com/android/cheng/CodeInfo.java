package com.android.cheng;

public final class CodeInfo {

    //handshake
    public static final String UART_HANDSHAKE_1C = "3030303000000100010000";
    public static final String UART_HANDSHAKE_1A = "3031303000000100010001";
    public static final String UART_HANDSHAKE_2C = "3031303000000100010001";
    public static final String UART_HANDSHAKE_2A = "3031303000000100010001";
    
    //heartbeats
    public static final String BEATS = "!";

    //get info
    public static final String GET_INFO_1C = "4348435000000100010018";
    public static final String GET_INFO_1A = "2E4E4B010400010001004348435036";

    public static final String GET_INFO_2C = "4349435000000100010019";
    public static final String GET_INFO_2A = "2E4E4B210400010001004349435017";

    public static final String GET_INFO_3C = "434A43500000010001001A";
    public static final String GET_INFO_3A = "2E4E4B21040001000100434A435014";

    public static final String GET_INFO_4C = "434B43500000010001001B";
    public static final String GET_INFO_4A = "2E4E4B21040001000100434B435015";

    public static final String GET_INFO_5C = "4350525600000100010017";
    public static final String GET_INFO_5A = 
"43505256720001000100505553353030000030313536000000000000000000000000000000000000000047413132313939332D31393000000000323031333033303800000000000000000000000000000000000000000000000000000000000000000000000000000000000000005001FFFFFF0000003768224D328B3A";





    //into pus
    public static final String INTO_PUS_C = "43505441020001000100010005";
    public static final String INTO_PUS_A = "43505441640001000100505553353030000030313536000000000000000000000000000000000000000047413132313939332D31393000000000323031333033303800000000000000000000000000000000000000000000000000000000000000000000000000000000000000001A";

    public static final String EXIT_PUS_C = "43505444020001000100010000";
    public static final String EXIT_PUS_A = "2E414B000400010001004350544423";

    public static final String INTO_PUM_C = "43505441020001000100020006";
    public static final String INTO_PUM_A = "435054410800010001000302FFFFFF000000F0";

    public static final String EXIT_PUM_C = "43505444020001000100020003";
    public static final String EXIT_PUM_A = "2E414B000400010001004350544423";






    //pus upload
    //public static final String UPLOAD_PUS_CONSOLE_C = "43535243080001000100XXXXXXXXYYYY0000ZZ"; //38 
    public static final String UPLOAD_PUS_CONSOLE_C = "43535243080001000100"; //38
    public static final String UPLOAD_PUS_CONSOLE_A = "43535243YYYY000100XXXXZZ."; //....lianxu

    //pus download
    public static final String DOWNLOAD_PUS_CONSOLE_C = "435357430C0001000100XXXXXXXX01000000YY000000ZZ";//46
    public static final String DOWNLOAD_PUS_CONSOLE_A = "2E414B000400010001004353574324";

    //pus all error
    public static final String GET_PUS_ERROR_C = "43535245020001000100020007";//26
    public static final String GET_PUS_ERROR_A =  "435352450001010020001301200207213202030C090909048120080000005D74000000000000000000001301200207282902030C090909048120070000003A6C000000000000000000001301200207285202030C09090904812008000000E873000000000000000000001301200207305002030C09090904812007000000176D000000000000000000001301200207375502030C09090904812008000000FE73000000000000000000001301200207400902030C090909048120070000004B6A000000000000000000001301200207412302030C09090904812007000000A468000000000000000000001301200207420602030C09090904812006000000A1630000000000000000000010";

    //pus del
    public static final String DEL_PUS_ERROR_C = "43534345020001000100020016";//26
    public static final String DEL_PUS_ERROR_A = "2E414B000400010001004353434536";

    //pus jobdata
    public static final String UPLOAD_PUS_JOBDATA_C = "4353524A080001000100XXXXXXXXYYYY0000ZZ"; //38
    public static final String UPLOAD_PUS_JOBDATA_A = "4353524AYYYY010001000XXXXZZ"; //lianxu

    public static final String DOWNLOAD_PUS_JOBDATA_PREPARE_C = "435342420100010001006879"; //（准备写入FLASH） //24
    public static final String DOWNLOAD_PUS_JOBDATA_PREPARE_A = "2E414B000400010001004353424230"; // (准备OK)
    public static final String DOWNLOAD_PUS_JOBDATA_RAM_C = "4353574A050001000100XXXXXXXXYYZZ"; //32
    public static final String DOWNLOAD_PUS_JOBDATA_RAM_A = "2E414B000400010001004353574A2D";

    public static final String DOWNLOAD_PUS_JOBDATA_FLASH_C ="4353424D0000010001001F"; // （开始写入FLASH） //22
    public static final String DOWNLOAD_PUS_JOBDATA_FLASH_A ="2E414B000400010001004353424D3F";

    //pus date time
    public static final String GET_PUS_TIME_C ="4353525A00000100010018";
    public static final String GET_PUS_TIME_A ="4353525A090001000100YYMMDDWWXXFFEE0000ZZ";

    public static final String SET_PUS_TIME_C ="4353575A080001000100YYMMDDWWXXFFEE00ZZ";
    public static final String SET_PUS_TIME_A ="2E414B000400010001004353575A3D";



    //pum upload
    public static final String UPLOAD_PUM_CONSOLE_C = "434D5243080001000100XXXXXXXXYYYY0000ZZ"; //38
    public static final String UPLOAD_PUM_CONSOLE_A = "434D5243YYYY01000100XXXX...ZZ"; //....lianxu

    //pum download
    public static final String DOWNLOAD_PUM_CONSOLE_C = "434D57430C0001000100XXXXXXXX02000000YYYY0000ZZ";//46
    public static final String DOWNLOAD_PUM_CONSOLE_A = "2E414B00040001000100434D57433A";


    //pum all error
    public static final String GET_PUM_ERROR_C = "434D5245020001000100020019";//26
    public static final String GET_PUM_ERROR_A =  "434D52450001010011001301200208413102030C090909040322000003000000410071000000000000001301200208415102030C090909040322000004000000410063000000000000001301200208422802030C0909090403220000050000004B0263000000000000001301200208424202030C0909090403220000060000004B0263000000000000001301200208431302030C090909040322000007000000950163000000000000001301200208454002030C090909040322000000000000000000000000000000001301200208454402030C090909040AF300000100000000000000ED0F000000001302010009091302030C09090904032200000200000000000000000000000000B2";

    //pum del
    public static final String DEL_PUM_ERROR_C = "434D4345020001000100020008";//26
    public static final String DEL_PUM_ERROR_A = "2E414B00040001000100434D434528";

    //pum jobdata
    public static final String UPLOAD_PUM_JOBDATA_C = "434D5243080001000100XXXXXXXXYYYY0000ZZ"; //38
    public static final String UPLOAD_PUM_JOBDATA_A = "4353524AYYYY010001000XXXXZZ"; //lianxu

    public static final String DOWNLOAD_PUM_JOBDATA_PREPARE_C = "435342420100010001006879"; //（准备写入FLASH） //24
    public static final String DOWNLOAD_PUM_JOBDATA_PREPARE_A = "2E414B000400010001004353424230"; // (准备OK)
    public static final String DOWNLOAD_PUM_JOBDATA_RAM_C = "4353574A050001000100XXXXXXXXYYZZ"; //32
    public static final String DOWNLOAD_PUM_JOBDATA_RAM_A = "2E414B000400010001004353574A2D";

    public static final String DOWNLOAD_PUM_JOBDATA_FLASH_C ="4353424D0000010001001F"; // （开始写入FLASH） //22
    public static final String DOWNLOAD_PUM_JOBDATA_FLASH_A ="2E414B000400010001004353424D3F";








    private CodeInfo() {
        throw new IllegalStateException("Non-instantiable class.");
    }

}
