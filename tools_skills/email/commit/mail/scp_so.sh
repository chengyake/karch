#!/usr/bin/expect -f
spawn scp /home/chengyake/mail/attachment/libCSDK-curve.so /home/chengyake/mail/attachment/libCSDK-line.so wangxiaobo@192.168.50.16:/home/wangxiaobo/code/mobile_scan/mscan/jni/hw/libs/armeabi-v7a/
set timeout 20
expect "password"
exec sleep 2
send "wangxiaobo\r"
interact
