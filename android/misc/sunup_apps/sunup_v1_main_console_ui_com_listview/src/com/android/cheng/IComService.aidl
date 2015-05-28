package com.android.cheng;


interface IComService {
    String getSystemTime();
    int writeAsync(in byte[] data);
}
