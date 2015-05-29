###socket模型


    TCP Client                TCP Server



    socket()                   socket()
       |                          |
       |                          V
       |                        bind()
       |                          |
       |                          V
       |                       listen()
       |                          |
       |                          V
       |                       accept()
       |      setup connection    |
   connect() <-----------------> wait...
       |                          |
       V         request data     V
    write()  <-----------------> read()
       |                          |
       |                          |
       |                          |
       |                          |
       V          respon data     V
    read()  <-----------------> write()
       |                          |
       |                          |
       |                          |
       V                          V
    close()                     close()
