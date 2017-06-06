# Chat
Chat System


--------------------------------
Testing
--------------------------------
gcc Chat.c -pthread -o chat

open 3 different terminal and paste the following separately.

 ./chat 1111 127.0.0.1 2222 3333                                                                                     
 ./chat 2222 127.0.0.1 1111 3333                                                                                   
 ./chat 3333 127.0.0.1 1111 2222


To check compatibility with another way of implementation
gcc c1.c -o c1

./chat 3333 127.0.0.1 2222 1111                                                                                                         
./c1
