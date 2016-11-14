#!/usr/bin/expect -f
spawn scp /home/chengyake/mail/attachment/BEE_C_CURVE.rpd  /home/chengyake/mail/attachment/BEE_C_LINE.rpd /home/chengyake/mail/attachment/BEE_CURVE.rpd  /home/chengyake/mail/attachment/BEE_LINE.rpd wangxiaobo@192.168.50.16:/home/wangxiaobo/code/mobile_scan/mscan/assets
set timeout 20
expect "password"
exec sleep 2
send "wangxiaobo\r"
interact
