package android.os;

interface IVdevService {
        int requestPin(int idx, int type, int value);
        int getPinConfig(int idx);
        int setPinLevel(int idx, int value);
        int getPinLevel(int idx);
        int freePin(int idx);
        byte[] readI2c(int addr, int len);
        int writeI2c(int addr,in byte[] data, int len);
}




