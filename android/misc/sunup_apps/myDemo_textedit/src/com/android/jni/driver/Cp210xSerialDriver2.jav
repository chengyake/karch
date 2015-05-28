/* Copyright 2011 Google Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 *
 * Project home page: http://code.google.com/p/usb-serial-for-android/
 */
 
package com.hoho.android.usbserial.driver;
 
import android.hardware.usb.UsbConstants;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbEndpoint;
import android.hardware.usb.UsbInterface;
import android.hardware.usb.UsbRequest;
import android.util.Log;
 
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.LinkedHashMap;
import java.util.Map;
 
/**
 * A {@link UsbSerialDriver} implementation for a variety of CP210x devices
 * <p>
 * This driver is based on the source codes from linux kernel 3.7.
 *
 * </p>
 * <p>
 * Not all devices are tested.
 * <ul>
 * <li>Read and write of serial data (see {@link #read(byte[], int)} and
 * {@link #write(byte[], int)}.
 * <li>Setting baud rate (see {@link #setBaudRate(int)}).
 * </ul>
 * </p>
 *  *
 * @author Jongmoo Sohn (jmthegrey@gmail.com)
 * @see <a href="http://code.google.com/p/usb-serial-for-android/">USB Serial
 * for Android project page</a>
 */
public class Cp210xSerialDriver2 extends CommonUsbSerialDriver {
 
    private final String TAG = Cp210xSerialDriver2.class.getSimpleName();
 
    /* Config request types */
    private static final int REQTYPE_HOST_TO_INTERFACE  = 0x41;
    private static final int REQTYPE_INTERFACE_TO_HOST  = 0xc1;
    private static final int REQTYPE_HOST_TO_DEVICE     = 0x40;
    private static final int REQTYPE_DEVICE_TO_HOST     = 0xc0;
 
    /* Config request codes */
    private static final int CP210X_IFC_ENABLE      = 0x00;
    private static final int CP210X_SET_BAUDDIV     = 0x01;
    private static final int CP210X_GET_BAUDDIV     = 0x02;
    private static final int CP210X_SET_LINE_CTL    = 0x03;
    private static final int CP210X_GET_LINE_CTL    = 0x04;
    private static final int CP210X_SET_BREAK       = 0x05;
    private static final int CP210X_IMM_CHAR        = 0x06;
    private static final int CP210X_SET_MHS         = 0x07;
    private static final int CP210X_GET_MDMSTS      = 0x08;
    private static final int CP210X_SET_XON         = 0x09;
    private static final int CP210X_SET_XOFF        = 0x0A;
    private static final int CP210X_SET_EVENTMASK   = 0x0B;
    private static final int CP210X_GET_EVENTMASK   = 0x0C;
    private static final int CP210X_SET_CHAR        = 0x0D;
    private static final int CP210X_GET_CHARS       = 0x0E;
    private static final int CP210X_GET_PROPS       = 0x0F;
    private static final int CP210X_GET_COMM_STATUS = 0x10;
    private static final int CP210X_RESET           = 0x11;
    private static final int CP210X_PURGE           = 0x12;
    private static final int CP210X_SET_FLOW        = 0x13;
    private static final int CP210X_GET_FLOW        = 0x14;
    private static final int CP210X_EMBED_EVENTS    = 0x15;
    private static final int CP210X_GET_EVENTSTATE  = 0x16;
    private static final int CP210X_SET_CHARS       = 0x19;
    private static final int CP210X_GET_BAUDRATE    = 0x1D;
    private static final int CP210X_SET_BAUDRATE    = 0x1E;
 
    /* CP210X_IFC_ENABLE */
    private static final int UART_ENABLE            = 0x0001;
    private static final int UART_DISABLE           = 0x0000;
 
    /* CP210X_(SET|GET)_BAUDDIV */
    private static final int BAUD_RATE_GEN_FREQ     = 0x384000;
 
    /* CP210X_(SET|GET)_LINE_CTL */
    private static final int BITS_DATA_MASK     = 0X0f00;
    private static final int BITS_DATA_5        = 0X0500;
    private static final int BITS_DATA_6        = 0X0600;
    private static final int BITS_DATA_7        = 0X0700;
    private static final int BITS_DATA_8        = 0X0800;
    private static final int BITS_DATA_9        = 0X0900;
 
    private static final int BITS_PARITY_MASK   = 0x00f0;
    private static final int BITS_PARITY_NONE   = 0x0000;
    private static final int BITS_PARITY_ODD    = 0x0010;
    private static final int BITS_PARITY_EVEN   = 0x0020;
    private static final int BITS_PARITY_MARK   = 0x0030;
    private static final int BITS_PARITY_SPACE  = 0x0040;
 
    private static final int BITS_STOP_MASK     = 0x000f;
    private static final int BITS_STOP_1        = 0x0000;
    private static final int BITS_STOP_1_5      = 0x0001;
    private static final int BITS_STOP_2        = 0x0002;
 
    /* CP210X_SET_BREAK */
    private static final int BREAK_ON           = 0x0001;
    private static final int BREAK_OFF          = 0x0000;
 
    /* CP210X_(SET_MHS|GET_MDMSTS) */
    private static final int CONTROL_DTR        = 0x0001;
    private static final int CONTROL_RTS        = 0x0002;
    private static final int CONTROL_CTS        = 0x0010;
    private static final int CONTROL_DSR        = 0x0020;
    private static final int CONTROL_RING       = 0x0040;
    private static final int CONTROL_DCD        = 0x0080;
    private static final int CONTROL_WRITE_DTR  = 0x0100;
    private static final int CONTROL_WRITE_RTS  = 0x0200;
 
     
    private int mBulkInSize = 256;
 
    private int mBaudRate = 0;
    private int mDataBits = 0;
    private int mParity = 0;
    private int mStopBits = 0;
     
    private static final int DEFAULT_BAUD_RATE = 115200;
    private static final int DEFAULT_DATA_BITS = DATABITS_8;
    private static final int DEFAULT_PARITY = PARITY_NONE;
    private static final int DEFAULT_STOP_BITS = STOPBITS_1;
 
    /**
     * Due to http://b.android.com/28023 , we cannot use UsbRequest async reads
     * since it gives no indication of number of bytes read. Set this to
     * {@code true} on platforms where it is fixed.
     * Safe to assume anything less than API level 17 is broken.
     */
    private static final boolean ENABLE_ASYNC_READS = true;
     
    private UsbEndpoint mEndpointOut = null;
    private UsbEndpoint mEndpointIn = null;
     
    public static final int USB_CTRL_GET_TIMEOUT_MILLIS = 5000;
    public static final int USB_CTRL_SET_TIMEOUT_MILLIS = 5000;
 
    private boolean mRts = false;
    private boolean mDtr = false;
 
    /**
     * Constructor.
     *
     * @param usbDevice the {@link UsbDevice} to use
     * @param usbConnection the {@link UsbDeviceConnection} to use
     * @throws UsbSerialRuntimeException if the given device is incompatible
     *             with this driver
     */
    public Cp210xSerialDriver2(UsbDevice usbDevice, UsbDeviceConnection usbConnection) {
        super(usbDevice, usbConnection);
         
        UsbInterface intf = findInterface(usbDevice);
        if (setInterface(usbDevice, intf)) {
            UsbEndpoint epOut = null;
            UsbEndpoint epIn = null;
            // Look for bulk endpoints
            for (int i = 0; i < intf.getEndpointCount(); i++) {
                UsbEndpoint ep = intf.getEndpoint(i);
                if (UsbConstants.USB_ENDPOINT_XFER_BULK == ep.getType()) {
                    if (UsbConstants.USB_DIR_OUT == ep.getDirection()) {
                        epOut = ep;
                        Log.d(TAG, "endpointOut: " + i);
                    } else {
                        epIn = ep;
                        Log.d(TAG, "endpointIn: " + i);
                    }
                }
            }
            if (epOut == null || epIn == null) {
                throw new IllegalArgumentException("Not all endpoints found");
            }
            mEndpointOut = epOut;
            mEndpointIn = epIn;
        }
    }
     
    private UsbInterface findInterface(UsbDevice device) {
        Log.d(TAG, "findInterface " + device);
        int count = device.getInterfaceCount();
        for (int i =0 ; i < count ; i++) {
            UsbInterface intf = device.getInterface(i);
            Log.d(TAG, "Interface: " + count + " Class: " + intf.getInterfaceClass() +
                    " Subclass: " + intf.getInterfaceSubclass() +
                    " Protocol: " + intf.getInterfaceProtocol());
            if (intf.getInterfaceClass() == 255
                    && intf.getInterfaceSubclass() == 00
                    && intf.getInterfaceProtocol() == 0) {
                return intf;
            }
        }
        return null;
    }
     
    private boolean setInterface(UsbDevice device, UsbInterface intf) {
        if (mConnection != null) {
            Log.d(TAG, "Open succeeded.");
            if (mConnection.claimInterface(intf, false)) {
                Log.d(TAG, "Claim interface succeeded.");
                return true;
            } else {
                Log.d(TAG, "Claim interface failed.");
            }
        } else {
            Log.d(TAG, "No connection to set interface.");
        }
         
        return false;
    }
     
    private int cp210x_set_config(int request, int value, byte[] buffer, int length) throws IOException {
        int result = mConnection.controlTransfer(REQTYPE_HOST_TO_INTERFACE, request,
                    value, 0x0 /* index */, buffer, length, USB_CTRL_SET_TIMEOUT_MILLIS);
 
        if (result != length) {
            throw new IOException("cp210x_set_config failed: result=" + result);
        }
         
        return result;
    }
     
    private int cp210x_set_config_single(int request, int value) throws IOException {
        return cp210x_set_config(request, value, null, 0x0);
    }
     
    // From Sniffed Data
    private static final byte[] CP210X_SET_CHARS_DATA = {
        (byte)0x04, (byte)0x00, (byte)0x00, (byte)0x0A, (byte)0x00, (byte)0x00 };
     
    // From Sniffed Data
    private static final byte[] CP210X_SET_FLOW_DATA = {
        (byte)0x01, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00,
        (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00 };
 
    @Override
    public void open() throws IOException {
        boolean opened = false;
        try {
            for (int i = 0; i < mDevice.getInterfaceCount(); i++) {
                if (mConnection.claimInterface(mDevice.getInterface(i), true)) {
                    Log.d(TAG, "claimInterface " + i + " SUCCESS");
                } else {
                    Log.d(TAG, "claimInterface " + i + " FAIL");
                }
            }
            // Configure Single Port
//            cp210x_set_config_single(CP210X_IFC_ENABLE, UART_ENABLE);
             
            // From Sniffed Data
            cp210x_set_config_single(CP210X_IFC_ENABLE, 0xFFFF);    // UART_ENABLE = 0x0001? 0xFFFF?
            cp210x_set_config(CP210X_SET_CHARS, 0x0000, CP210X_SET_CHARS_DATA, CP210X_SET_CHARS_DATA.length);
            cp210x_set_config(CP210X_SET_FLOW, 0x0000, CP210X_SET_FLOW_DATA, CP210X_SET_FLOW_DATA.length);
             
            // Configure the termios structure
            //  cp210x_get_termios() NOT IMPLEMENTED !
            //  Instead, following default values are set.
            if (0 == mBaudRate) mBaudRate = DEFAULT_BAUD_RATE;
            if (0 == mDataBits) mDataBits = DEFAULT_DATA_BITS;
            if (0 == mParity) mParity = DEFAULT_PARITY;
            if (0 == mStopBits) mStopBits = DEFAULT_STOP_BITS;
             
            // The baud rate must be initialised on cp2104
//            cp210x_change_speed(mBaudRate);
            setParameters(mBaudRate, mDataBits, mStopBits, mParity);
             
            // From Sniffed Data
            cp210x_set_config_single(CP210X_SET_MHS, CONTROL_WRITE_RTS);    // 0x0200
            cp210x_set_config_single(CP210X_SET_MHS, CONTROL_WRITE_DTR + CONTROL_DTR);  // 0x0101
            cp210x_set_config_single(CP210X_SET_BREAK, BREAK_OFF);
             
            opened = true;
        } finally {
            if (!opened) {
                close();
            }
        }
    }
 
    @Override
    public void close() {
        try {
            // From Sniffed Data
            cp210x_set_config_single(CP210X_SET_BREAK, BREAK_OFF);
            cp210x_set_config_single(CP210X_IFC_ENABLE, UART_DISABLE);
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
         
        mConnection.close();
    }
 
    @Override
    public int read(byte[] dest, int timeoutMillis) throws IOException {
 
        if (ENABLE_ASYNC_READS) {
            final int readAmt;
            synchronized (mReadBufferLock) {
                // mReadBuffer is only used for maximum read size.
                readAmt = Math.min(dest.length, mReadBuffer.length);
            }
 
            final UsbRequest request = new UsbRequest();
            request.initialize(mConnection, mEndpointIn);
 
            final ByteBuffer buf = ByteBuffer.wrap(dest);
            if (!request.queue(buf, readAmt)) {
                throw new IOException("Error queueing request.");
            }
 
            final UsbRequest response = mConnection.requestWait();
            if (response == null) {
                throw new IOException("Null response");
            }
 
            final int BytesRead = buf.position();
            if (BytesRead > 0) {
//                Log.d(TAG, HexDump.dumpHexString(dest, 0, Math.min(32, dest.length)));
                return BytesRead;
            } else {
                return 0;
            }
        } else {
            final int totalBytesRead;
 
            synchronized (mReadBufferLock) {
                final int readAmt = Math.min(dest.length, mReadBuffer.length);
                totalBytesRead = mConnection.bulkTransfer(mEndpointIn, mReadBuffer,
                        readAmt, timeoutMillis);
            }
 
            if (totalBytesRead > 0) {
                System.arraycopy(mReadBuffer, 0, dest, 0, totalBytesRead);
            }
            return totalBytesRead;
        }
    }
 
    @Override
    public int write(byte[] src, int timeoutMillis) throws IOException {
        int offset = 0;
 
        while (offset < src.length) {
            final int writeLength;
            final int amtWritten;
 
            synchronized (mWriteBufferLock) {
                final byte[] writeBuffer;
 
                writeLength = Math.min(src.length - offset, mWriteBuffer.length);
                if (offset == 0) {
                    writeBuffer = src;
                } else {
                    // bulkTransfer does not support offsets, make a copy.
                    System.arraycopy(src, offset, mWriteBuffer, 0, writeLength);
                    writeBuffer = mWriteBuffer;
                }
 
                amtWritten = mConnection.bulkTransfer(mEndpointOut, writeBuffer, writeLength,
                        timeoutMillis);
            }
 
            if (amtWritten <= 0) {
                throw new IOException("Error writing " + writeLength
                        + " bytes at offset " + offset + " length=" + src.length);
            }
 
            Log.d(TAG, "Wrote amtWritten=" + amtWritten + " attempted=" + writeLength);
            offset += amtWritten;
        }
        return offset;
    }
     
    public int setBaudRate(int baudRate) throws IOException {
        mBaudRate = cp210x_quantise_baudrate(baudRate);
        cp210x_change_speed(mBaudRate);
        return mBaudRate;
    }
 
    @Override
    public void setParameters(int baudRate, int dataBits, int stopBits, int parity)
            throws IOException {
         
        mBaudRate = cp210x_quantise_baudrate(baudRate);
        cp210x_change_speed(mBaudRate);
 
        int config = 0x0000;
        switch (dataBits) {
            case DATABITS_5:
                config |= BITS_DATA_5;
                break;
            case DATABITS_6:
                config |= BITS_DATA_6;
                break;
            case DATABITS_7:
                config |= BITS_DATA_7;
                break;
            case DATABITS_8:
                config |= BITS_DATA_8;
                break;
            default:
                throw new IllegalArgumentException("Unknown dataBits value: " + dataBits);
        }
 
        switch (parity) {
            case PARITY_NONE:
                config |= BITS_PARITY_NONE;
                break;
            case PARITY_ODD:
                config |= BITS_PARITY_ODD;
                break;
            case PARITY_EVEN:
                config |= BITS_PARITY_EVEN;
                break;
            case PARITY_MARK:
                config |= BITS_PARITY_MARK;
                break;
            case PARITY_SPACE:
                config |= BITS_PARITY_SPACE;
                break;
            default:
                throw new IllegalArgumentException("Unknown parity value: " + parity);
        }
 
        switch (stopBits) {
            case STOPBITS_1:
                config |= BITS_STOP_1;
                break;
            case STOPBITS_1_5:
                config |= BITS_STOP_1_5;
                break;
            case STOPBITS_2:
                config |= BITS_STOP_2;
                break;
            default:
                throw new IllegalArgumentException("Unknown stopBits value: " + stopBits);
        }
 
        int result = mConnection.controlTransfer(REQTYPE_HOST_TO_INTERFACE,
                CP210X_SET_LINE_CTL, config, 0 /* index */,
                null, 0, USB_CTRL_SET_TIMEOUT_MILLIS);
        if (result != 0) {
            throw new IOException("Setting parameters failed: result=" + result);
        }
 
        mParity = parity;
        mStopBits = stopBits;
        mDataBits = dataBits;
         
        verifyConfig(baudRate, config);
    }
 
 
    //convert byte array to integer type value
    //possible returned value: byte/short/int/long
    //Note: max support long -> input byte array length should not exceed 8
    public static long byteArrToInteger(byte[] byteArr){
        long convertedInterger = 0;
         
        //follow works:
        //int readbackBaudrate= (baudrateByteArr[3]<<24)&0xff000000|(baudrateByteArr[2]<<16)&0xff0000|(baudrateByteArr[1]<<8)&0xff00|(baudrateByteArr[0]<<0)&0xff;
         
        //115200 == [0, -62, 1, 0]
        //1200==[-80, 4, 0, 0]
        for(int i = 0; i < byteArr.length; i++){
            //long curValue = byteArr[i];
            //int curValue = byteArr[i];
            byte curValue = byteArr[i];
            long shiftedValue = curValue << (i * 8);
            long mask = 0xFF << (i * 8);
            long maskedShiftedValue = shiftedValue & mask;
            //0x0, 0xC200, 0x10000, 0x0 -> 115200==0x1C200
            //0xB0, 0x400, 0x0, 0x0-> 1200==0x4B0
            convertedInterger |= maskedShiftedValue;
        }
         
       return convertedInterger;
    }
     
    //convert interger type value to byte array
    //possible input value: byte/short/int/long
    //Note: max return byte array is 8 -> max support long
    public static byte[] integerToByteArr(long inputIntergerValue, int byteLen){
        byte[] convertedByteArr = new byte[byteLen];
         
        for (int i = 0; i < convertedByteArr.length; i++) {
            convertedByteArr[i] = (byte) ((inputIntergerValue >> (8 * i)) & 0xFF);
            //115200 == [0, -62, 1, 0, 0, 0, 0, 0]
            //1200==[-80, 4, 0, 0]
            //600==[88, 2]
            //32==[32]
        }
 
       return convertedByteArr;
    }
     
    private void verifyConfig(int writtenBaudrate, int writtenLineControl){
        //1. verify baudrate
        byte[] baudrateByteArr = new byte[]{0,0,0,0};
        mConnection.controlTransfer(REQTYPE_INTERFACE_TO_HOST, CP210X_GET_BAUDRATE,
                0, 0/* index */, baudrateByteArr, 4, USB_CTRL_GET_TIMEOUT_MILLIS);
        //int readbackBaudrate = (baudrateByteArr[0]) | (baudrateByteArr[1] << 8) | (baudrateByteArr[2] << 16) | (baudrateByteArr[3] << 24);
        //int readbackBaudrate= (baudrateByteArr[3]<<24)&0xff000000|(baudrateByteArr[2]<<16)&0xff0000|(baudrateByteArr[1]<<8)&0xff00|(baudrateByteArr[0]<<0)&0xff;
        int readbackBaudrate = (int)byteArrToInteger(baudrateByteArr);
        if(writtenBaudrate == readbackBaudrate){
            //ok
        }
        else{
            //false
        }
         
        //2.verify other config
        byte[] lineControlByteArr = new byte[]{0,0};
        mConnection.controlTransfer(REQTYPE_INTERFACE_TO_HOST, CP210X_GET_LINE_CTL,
                0, 0/* index */, lineControlByteArr, 2, USB_CTRL_GET_TIMEOUT_MILLIS);
        //int readbackLineControl = (lineControlByteArr[0]) | (lineControlByteArr[1] << 8);
        int readbackLineControl = (int)byteArrToInteger(lineControlByteArr);
        if(writtenLineControl == readbackLineControl){
            //ok
        }
        else{
            //false
        }
    }
     
    @Override
    public boolean getCD() throws IOException {
        return false;
    }
 
    @Override
    public boolean getCTS() throws IOException {
        return false;
    }
 
    @Override
    public boolean getDSR() throws IOException {
        return false;
    }
 
    @Override
    public boolean getDTR() throws IOException {
        return mDtr;
        //return true;
    }
 
    @Override
    public void setDTR(boolean value) throws IOException {
        mDtr = value;
         
        short control = 0;
         
        if (mDtr) {
            control |= CONTROL_DTR;
            control |= CONTROL_WRITE_DTR;
        }
        else{
            control &= ~CONTROL_DTR;
            control |= CONTROL_WRITE_DTR;
        }
 
        try {
            cp210x_set_config_single(CP210X_SET_MHS, control);
             
            verifyRtsDtr(control);
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }
 
    @Override
    public boolean getRI() throws IOException {
        return false;
    }
 
 
    @Override
    public boolean getRTS() throws IOException {
        return mRts;
        //return true;
    }
 
    @Override
    public void setRTS(boolean value) throws IOException {
        mRts = value;
         
        short control = 0;
 
        if (mRts) {
            control |= CONTROL_RTS;
            control |= CONTROL_WRITE_RTS;
        }
        else {
            control &= ~CONTROL_RTS;
            control |= CONTROL_WRITE_RTS;
        }
         
        try {
            cp210x_set_config_single(CP210X_SET_MHS, control);
             
            verifyRtsDtr(control);
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }
 
    /*
    // set/clear RTS/CTS and DTR/DSR
    private void setDtrRts() {
        short control = 0;
 
        if (mRts) {
            control |= CONTROL_RTS;
            control |= CONTROL_WRITE_RTS;
        }
        else {
            control &= ~CONTROL_RTS;
            control |= CONTROL_WRITE_RTS;
        }
         
        if (mDtr) {
            control |= CONTROL_DTR;
            control |= CONTROL_WRITE_DTR;
        }
        else{
            control &= ~CONTROL_DTR;
            control |= CONTROL_WRITE_DTR;
        }
 
        try {
            cp210x_set_config_single(CP210X_SET_MHS, control);
             
            verifyRtsDtr(control);
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }
    */
 
    private void verifyRtsDtr(int writtenMhsControl){
        //1. verify RTS DTR
        int writtenControlValue = writtenMhsControl & 0xFF; // only lowest 8 bit used for indicate RTS, DTR, ...
//        byte[] mhsControlByteArr = new byte[]{0};
        byte[] mhsControlByteArr = new byte[]{0}; // so here only need to read 1 byte
        mConnection.controlTransfer(REQTYPE_INTERFACE_TO_HOST, CP210X_GET_MDMSTS,
                0, 0/* index */, mhsControlByteArr, mhsControlByteArr.length, USB_CTRL_GET_TIMEOUT_MILLIS);
        int readbackMhsControl = (int)byteArrToInteger(mhsControlByteArr);
        //seem here can not verify written ok or not
        //for those bit not reflect whether happen RTS or DTR
        if(writtenControlValue == readbackMhsControl){
            //ok
            //written 769==0x301 -> read back: 0x01  => OK
            //higher bits auto become 0 after written
        }
        else{
            //false
        }
    }
    /*
    public static Map<Integer, int[]> getSupportedDevices() {
        final Map<Integer, int[]> supportedDevices = new LinkedHashMap<Integer, int[]>();
        supportedDevices.put(Integer.valueOf(UsbId.VENDOR_SILAB),
                new int[] {
                    UsbId.SILAB_CP210X_DEFAULT_0,
                    UsbId.SILAB_CP210X_DEFAULT_1,
                    UsbId.SILAB_CP210X_DEFAULT_2,
                    UsbId.SILAB_CP210X_DEFAULT_3
                });
        return supportedDevices;
    }*/

    public static Map<Integer, int[]> getSupportedDevices() {
        final Map<Integer, int[]> supportedDevices = new LinkedHashMap<Integer, int[]>();
        supportedDevices.put(Integer.valueOf(UsbId.VENDOR_SILAB),
                new int[] {
                        UsbId.SILAB_CP2102
                });
        return supportedDevices;
    }

    /*
     * cp210x_quantise_baudrate
     * Quantises the baud rate as per AN205 Table 1
     */
    private int cp210x_quantise_baudrate(int baud) {
        if (baud <= 300)
            baud = 300;
        else if (baud <= 600)      baud = 600;
        else if (baud <= 1200)     baud = 1200;
        else if (baud <= 1800)     baud = 1800;
        else if (baud <= 2400)     baud = 2400;
        else if (baud <= 4000)     baud = 4000;
        else if (baud <= 4803)     baud = 4800;
        else if (baud <= 7207)     baud = 7200;
        else if (baud <= 9612)     baud = 9600;
        else if (baud <= 14428)    baud = 14400;
        else if (baud <= 16062)    baud = 16000;
        else if (baud <= 19250)    baud = 19200;
        else if (baud <= 28912)    baud = 28800;
        else if (baud <= 38601)    baud = 38400;
        else if (baud <= 51558)    baud = 51200;
        else if (baud <= 56280)    baud = 56000;
        else if (baud <= 58053)    baud = 57600;
        else if (baud <= 64111)    baud = 64000;
        else if (baud <= 77608)    baud = 76800;
        else if (baud <= 117028)   baud = 115200;
        else if (baud <= 129347)   baud = 128000;
        else if (baud <= 156868)   baud = 153600;
        else if (baud <= 237832)   baud = 230400;
        else if (baud <= 254234)   baud = 250000;
        else if (baud <= 273066)   baud = 256000;
        else if (baud <= 491520)   baud = 460800;
        else if (baud <= 567138)   baud = 500000;
        else if (baud <= 670254)   baud = 576000;
        else if (baud < 1000000)
            baud = 921600;
        else if (baud > 2000000)
            baud = 2000000;
        return baud;
    }
     
    /*
     * CP2101 supports the following baud rates:
     *
     *  300, 600, 1200, 1800, 2400, 4800, 7200, 9600, 14400, 19200, 28800,
     *  38400, 56000, 57600, 115200, 128000, 230400, 460800, 921600
     *
     * CP2102 and CP2103 support the following additional rates:
     *
     *  4000, 16000, 51200, 64000, 76800, 153600, 250000, 256000, 500000,
     *  576000
     *
     * The device will map a requested rate to a supported one, but the result
     * of requests for rates greater than 1053257 is undefined (see AN205).
     *
     * CP2104, CP2105 and CP2110 support most rates up to 2M, 921k and 1M baud,
     * respectively, with an error less than 1%. The actual rates are determined
     * by
     *
     *  div = round(freq / (2 x prescale x request))
     *  actual = freq / (2 x prescale x div)
     *
     * For CP2104 and CP2105 freq is 48Mhz and prescale is 4 for request <= 365bps
     * or 1 otherwise.
     * For CP2110 freq is 24Mhz and prescale is 4 for request <= 300bps or 1
     * otherwise.
     */
    private void cp210x_change_speed(int baudrate) throws IOException {
//        byte[] rawData = new byte[4];
//       
//        for (int i = 0; i < rawData.length; i++) {
//            rawData[i] = (byte) ((baudrate >> (8 * i)) & 0xFF);
//        }
//    
//        cp210x_set_config(CP210X_SET_BAUDRATE, 0x0000, rawData, rawData.length);
        byte[] baudrateByteArr = integerToByteArr(baudrate, 4);
        cp210x_set_config(CP210X_SET_BAUDRATE, 0x0000, baudrateByteArr, baudrateByteArr.length);
    }
}
