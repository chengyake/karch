flag=0


while true 
do

echo "-------------------------------"
flag=0
getmail --rcfile=/home/chengyake/.getmail/getmailrc
echo "check e-mail server done"

munpack /home/chengyake/.getmail/maildir/new/* -C /home/chengyake/mail/attachment

if [ -f /home/chengyake/mail/attachment/*.so ] ; then
echo "copy BEE* to server 16"
./scp_so.sh 
let flag+=1
fi

if [ -f /home/chengyake/mail/attachment/BEE* ] ; then
echo "copy *.so to server 16"
./scp_bee.sh 
let flag+=1
fi

if [ $flag -gt 0 ] ; then
    sleep 300
    ./scp_apk.sh
    time=`date`
    echo "send email at $time" | mail -s "Compile" -t  chengyake@lonshinetech.com    -a From:ykcheng@lonshinetech.com -A /home/chengyake/mail/upload/MobileScan-release.apk
    echo "send email at $time"
fi
rm /home/chengyake/mail/attachment/* &> /dev/null
rm /home/chengyake/mail/upload/* &> /dev/null
rm /home/chengyake/.getmail/maildir/new/* &> /dev/null
echo ""
echo ""

sleep 300






done
