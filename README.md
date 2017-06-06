# Chat
Chat System


================================
Testing
================================

Chat.c

gcc Chat.c -pthread -o chat

open 3 different terminal and paste the following separately. 
Segmentation and assembling doesn't work fine here.

 ./chat 1111 127.0.0.1 2222 3333                                                                                     
 ./chat 2222 127.0.0.1 1111 3333                                                                                   
 ./chat 3333 127.0.0.1 1111 2222

c1.c & c2.c

To check compatibility with another way of implementation we made c1 and c2 (Single thread). Segmentation and assembling works here. c1 requests to chat, and c2 can only reply after getting a message.

gcc c1.c -o c1 && gcc c2.c -o c2

./c1 ./c2 on different terminal.


