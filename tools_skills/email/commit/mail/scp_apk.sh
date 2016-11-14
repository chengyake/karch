#!/usr/bin/expect -f
spawn scp wangxiaobo@192.168.50.16:/home/wangxiaobo/code/mobile_scan/mscan/bin/MobileScan-release.apk  /home/chengyake/mail/upload
set timeout 20
expect "password"
exec sleep 2
send "wangxiaobo\r"
interact
