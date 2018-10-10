# Chat
Chat System

Message encryption and decryption (C) : Done
Packet packet formation, fragmentation and assembling (C and C++) : Done
Packet hashing and hash table function (C++) : Done in C++ on windows
Keep alive message ( C ) : Done
Acknowledgement (C) : Done
Compatibility with other code : To test with another C code we built (Although not working perfectly, and tested with a python sending code.

=============
Testing
=============
Chat.c

gcc Chat.c -pthread -o chat

open 3 different terminal and paste the following separately. 

 ./chat 1111 127.0.0.1 2222 3333                                                                                     
 ./chat 2222 127.0.0.1 1111 3333                                                                                   
 ./chat 3333 127.0.0.1 1111 2222

c1.c & c2.c

To check compatibility with another way of implementation we made c1 and c2. Segmentation and assembling works here. c1 requests to chat, and c2 can only reply after getting a message.

gcc c1.c -o c1 && gcc c2.c -o c2

./c1 ./c2 on different terminal.


consoleApplication6.cpp can run on windows to show segmentation and assembling works.

=================================================================================


The final packet structure used in our implementation is below which has been implemented in C++:

UDP 
CRC
HOP
Order
PkLast 
SizeOf
PKTID
msgId
PktChunk

		32	  	2	4		2	2		4	4		
Max packet size is 512 bytes ← Including header (but not UDP)

UDP – This is the UDP header that gets automatically attached when sending through UDP. We do not create this nor do we have a use. This is here as we need to physically send the packets through UDP.

HOP – 2 byte – Time to Live – This is the time the packet has to live in the In-Transit table
				Not sure if we’re going to do hop count or time but 2 bytes should be 				enough
Order – 4 bytes – This is the sequence number for the packet eg – the order in which it will be arranged

Size of – 2 bytes – Length of the packet

CRC – 32 bytes – Checksum(md5) of the entire packet, used to detect errors in transmission

PacketID – 4 bytes – Randomly generated ID for identifying packet in In-Transmission table for reliability

msgId – 4 Bytes – Randomly generated ID to indetify to which full message packet is part of

PktChunk (Packet Chunk) – 462 Bytes (Max) – This is the payload, has to be encrypted

PktLast – 2 bytes – Number of last chunk of message, in essence the total size(in chunks) of the message



CRC 0 – 31
HOP 32-33
ORDER 34-37
LAST 38-39
SIZE  40-41
PKTID 42- 45
msgID 46-49
Message payload 50 - 


Packet Fragmentation and Formation

This code is built in the purpose to provide fragmentation in case the amount of sendable data bigger than allowed by MTU.  Code is built in C++.

This code was built as a part of Protocol Design homework to implement a chat messenger. This code was built as a groupwork consisting Tigranuhi Mkhitaryan and Alar Paas. And thanks to Kayode Omotoye and Ephrem Demesa.

OpenSSL/md5 library was used to hash the whole packet from the “server” side, the packet is rehashed with the same function at the client side to confirm the integrity of the packet. To fragment the packet, program first checks the size of the packet chunk. If the size of the entire chunk is less than 462byte, then no fragmentation is needed; instead a padding is added to complete the packet. In a case where the entire chunk exceeds 462 bytes, fragmentation is done. The entire chunk is divided by 462bytes and every 462bytes gotten is used as a particular packet chunk adding a sequence number following the order.

READ input and segment the packet if necessary:

1) INPUT DATA (keyboard + ENTER; or SENDFILE function) = payload

2) Call CRYPTO to encrypt the payload (encrypt payload with public key of the recepient)

3) Create the „BIG_PACKET“ with complete HEADER+crypted payload

3.1 Packet structure: PACKET(CRC=md5;HOP=15; ORDER =0; PKT_LAST=0; SIZE_OF=length of

the packet;PKTID=random; MSGID=random;PKT_CHUNK=CRYPTED_PAYLOAD)

4) If size/length of PKT_CHUNK = &lt; 462 bytes then call FLOODING and flood the“BIG_PACKET“

away.

5) If size/length of PKT_CHUNK &gt;462 bytes then call SEGMENTATION and build „SMALL_PACKETS“
with updated PKT_LAST and ORDER number according to packet sequence.

A consoleApplication6.cpp has been built which runs on windows to test it works.

It has also been implemented in C and embedded in the final code.


Packet Hashing and Hash Table Function 

This project contains the implementation of:
- Hash packets and save in the list.
- Check the existence of the packet in the list.
- Hash times out every 10 minutes.

The code was implemented on windows with C++

Notice: This implementation is using SHA256 algorithm. In order to be able to use this implementation openssl library should be downloaded and installed.
	The last version of openssl library can be found and downloaded here --> https://www.openssl.org/source/ .
	The detiled explanation how to install it can be found here --> https://habrahabr.ru/post/277935/ .

Users are able to call just two functions:  
			AddPacketToHashingList(string content); - in order to add new packet in the hash list
                        IsPacketAlreadyExists(string content); -  in order to check if a packet already exist in the list or not

Example of usage:
			PacketHashing ph; 
			if(IsPacketAlreadyExists("p1"))
				ph.AddPacketToHashingList("p1");
			else dropThePacket();


Keep Alive Message

PROTOCOL

In the given protocol, KeepAlive message is created and connect with socket and flood to 
neighbours every 5 second. The message is a character "A". 

it sends for the neighbours parallarly while chatting using POSIX(threading in C)

REQUIRMENTS

This part of the implementation handles the keep alive messages 
for the given chat protocol. 
    
IMPLEMENTATION
First we created the thread in the base (main function).  
pthread_create(&aliveThread, 0, keep_alive_msg, &t);
pthread_detach(aliveThread);

and the keep alive message is defined as void* keep_alive_msg(void* ptr)


Message Encryption and Decryption

Done using a Caesar cipher with a shift of 3 in C. 
TO use we call two methods called Encryption and Decryption

