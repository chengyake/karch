package android.os;


public class VdevParam {

    private static final String TAG = "VdevService";
    private static final boolean DBG = true;

	public static final String ACTION_REQUEST_PIN_RSP = "action.com.android.server.REQUEST_PIN_RSP";
	public static final String EXTRA_REQUEST_PIN_NUM = "action.com.android.server.EXTRA_REQUEST_PIN_NUM";
	public static final String EXTRA_REQUEST_PIN_RET = "action.com.android.server.EXTRA_REQUEST_PIN_RET";
	public static final String ACTION_GET_PIN_CONFIG_RSP = "action.com.android.server.GET_PIN_CONFIG_RSP";
	public static final String EXTRA_GET_PIN_CONFIG_NUM = "action.com.android.server.EXTRA_GET_PIN_CONFIG_NUM";
	public static final String EXTRA_GET_PIN_CONFIG_RET = "action.com.android.server.EXTRA_GET_PIN_CONFIG_RET";
	public static final String ACTION_GET_PIN_LEVEL_RSP = "action.com.android.server.GET_PIN_VALUE_RSP";
	public static final String EXTRA_GET_PIN_LEVEL_NUM = "action.com.android.server.EXTRA_GET_PIN_VALUE_NUM";
	public static final String EXTRA_GET_PIN_LEVEL_RET = "action.com.android.server.EXTRA_GET_PIN_VALUE_RET";
	public static final String ACTION_SET_PIN_LEVEL_RSP = "action.com.android.server.REQUEST_PIN_RSP";
	public static final String EXTRA_SET_PIN_LEVEL_NUM = "action.com.android.server.EXTRA_REQUEST_PIN_NUM";
	public static final String EXTRA_SET_PIN_LEVEL_RET = "action.com.android.server.EXTRA_REQUEST_PIN_RET";
	public static final String ACTION_IRQ_TRIGGER = "action.com.android.server.IRQ_TRIGGER";
	public static final String EXTRA_IRQ_TRIGGER_NUM = "action.com.android.server.EXTRA_IRQ_TRIGGER_NUM";
	public static final String EXTRA_IRQ_TRIGGER_RET = "action.com.android.server.EXTRA_IRQ_TRIGGER_RET";
	public static final String ACTION_FREE_PIN_RSP = "action.com.android.server.FREE_PIN_RSP";
	public static final String EXTRA_FREE_PIN_NUM = "action.com.android.server.EXTRA_FREE_PIN_NUM";
	public static final String EXTRA_FREE_PIN_RET = "action.com.android.server.EXTRA_FREE_PIN_RET";
	public static final String ACTION_READ_I2C_RSP = "action.com.android.server.READ_I2C_RSP";
	public static final String EXTRA_READ_I2C_NUM = "action.com.android.server.EXTRA_READ_I2C_NUM";
	public static final String EXTRA_READ_I2C_RET = "action.com.android.server.EXTRA_READ_I2C_RET";
	public static final String ACTION_WRITE_I2C_RSP = "action.com.android.server.WRITE_I2C_RSP";
	public static final String EXTRA_WRITE_I2C_NUM = "action.com.android.server.EXTRA_WRITE_I2C_NUM";
	public static final String EXTRA_WRITE_I2C_RET = "action.com.android.server.EXTRA_WRITE_I2C_RET";

    public static final int CMD_REQUEST_PIN = 0;
    public static final int CMD_GET_PIN_CONFIG = 1;
    public static final int CMD_SET_PIN_LEVEL = 2;
    public static final int CMD_GET_PIN_LEVEL = 3;
    public static final int CMD_IRQ_TRIGGER = 4;
    public static final int CMD_FREE_PIN = 5;
    public static final int CMD_READ_I2C = 6;
    public static final int CMD_WRITE_I2C = 7;

    public static final int PIN_0 = 0;
    public static final int PIN_1 = 1;
    public static final int PIN_2 = 2;
    public static final int PIN_3 = 3;
    public static final int PIN_4 = 4;
    public static final int PIN_5 = 5;
    public static final int PIN_6 = 6;
    public static final int PIN_7 = 7;
    public static final int PIN_8 = 8;
    public static final int PIN_9 = 9;

    public static final int PIN_TYPE_NULL = 0;
    public static final int PIN_TYPE_IN = 1;
    public static final int PIN_TYPE_OUT = 2;
    public static final int PIN_TYPE_IRQ = 3;

    public static final int PIN_OUTPUT_LOW = 0;
    public static final int PIN_OUTPUT_HIGH = 1;

    public static final int PIN_TRIGER_EAGE_RISING = 0;
    public static final int PIN_TRIGER_EAGE_FALLING = 1;
    public static final int PIN_TRIGER_EAGE_BOTH = 2;

}
