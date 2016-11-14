#!/bin/bash

#sudo apt-get install sendEmail


email_reciver="chengyake@lonshinetech.com"
#发送者邮箱
email_sender=chengyake@lonshinetech.com
#邮箱用户名
email_username=chengyake@lonshinetech.com
#邮箱密码
email_password=cheng429198

file1_path="/home/chengyake/download/gprof2dot.py"

#smtp服务器地址
email_smtphost=smtp.lonshinetech.com:25

email_title="iOS客户端更新"
email_content="谢谢!"

sendEmail -f ${email_sender} -t ${email_reciver} -s ${email_smtphost} -u ${email_title} -xu ${email_username} -xp ${email_password} -m ${email_content} -a ${file1_path} -o message-charset=utf-8
