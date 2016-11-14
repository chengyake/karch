1.1 安装sendemail

      在Ubuntu下可以用新立得软件包管理器搜索安装，或者在终端运行：
              sudo apt-get install sendemail
      建议在安装前先安装另外两个包：libio-socket-ssl-perl, libnet-ssleay-perl
 
 2.2 使用sendemail和举例

        如果你想用你的邮箱christ@gmail.com发送邮件到buddha@qq.com，在终端输入:
                 sendemail -s smtp.gmail.com -f christ@gmail.com -t buddha@qq.com -u hello -m "A hello from Christans to buddhists via gmail" -xu christ -xp password -o tls=auto
解释：

-s smtp.gmail.com 指定服务器域名，邮件发送一般通过SMTP协议实现，其域名一般为smtp.***.com，比如qq邮箱的服务器为smtp.qq.com，163邮箱则为smtp.163.com
-f christ@gmail.com指定发送邮箱地址
-t buddha@qq.com 指定目的邮箱地址
-u hello 邮件标题
-m "A hello from Christans to buddhists via gmail" 邮件正文，较长的正文可以先存在文本文件中，不妨命名为mail.txt，然后换用-o message-file=mail.txt
-xu christ 指定邮箱用户名，即邮箱地址@之前的部分
-xp password 指定发送邮箱的密码
 -o tls=auto 加密方式在none, tls, ssl中自动选择
如果想在邮件中粘帖附件
 -a attachment_file1 attachment_file2 attachment_file3













2.2 配置getmail4和简单举例

 

sudo apt-get install getmail4
sudo apt-get install mpack #用于分离附件
#Maildir方式不能用root账号
adduser mailuser;passwd mailuser;
su mailuser;
mkdir -p {/home/mailuser/.getmail/maildir/new,/home/mailuser/.getmail/maildir/cur,/home/mailuser/.getmail/maildir/tmp};

cd /home/mailuser/.getmail;vim getmailrc #创建配置文件：
[retriever]
type = SimpleIMAPSSLRetriever
server = imap.gmail.com
username = abcuser
password = 123456

[destination]
type = Maildir
path = /home/mailuser/.getmail/maildir/
user = mailuser

[options]
read_all = False #只接受以前没有收取的邮件，如果改成True则收取邮箱中所有邮件
delete = False #下载邮件后不在服务器上删除该邮件，如果改成True则删除

#接收邮件命令
getmail --rcfile=/home/mailuser/.getmail/getmailrc

cd /home/mailuser/.getmail/maildir/new/
munpack 1365012300.M674923P25103Q15R829c9a6f48b836d7.295968

       getmail会自动收取邮件，下载的邮件会保存在~/.getmail/maildir/new/下。

 2.3 用munpack从邮件中抽取附件
       邮件正文和附件是作为一个整体文件被保存的，附件以MIME格式附着在整体文件最后，必须用程序抽取出来。
       在Ubuntu下可以用新立得软件包管理器搜索安装mpack，或者
                 sudo apt-get install mpack
       在终端中运行
                munpack mail_file
       程序会自动识别附件并抽取出来
