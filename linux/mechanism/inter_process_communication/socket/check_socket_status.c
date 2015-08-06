int SocketConnected(int sock)
{
    if(sock<=0)
        return 0;
    struct tcp_info info;
    int len=sizeof(info);
    getsockopt(sock, IPPROTO_TCP, TCP_INFO, &info, (socklen_t *)&len);
    if((info.tcpi_state==TCP_ESTABLISHED))
    {
        //myprintf("socket connected\n");
        return 1;
    }
    else
    {
        //myprintf("socket disconnected\n");
        return 0;
    }
}
