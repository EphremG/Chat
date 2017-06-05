/*--------------------------------------                
* Chat system implementation usng UDP  -
*---------------------------------------
* Caesar Cipher Encryption             -
* Segmentation                         -  
* Keep alive msg                       -
* socket and flooding                  -
* --------------------------------------
* Author: Ephrem Getachew Demesa
*         Kayode Omotoye
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include "crypto.h"
#include "crypto.c"
#include "md5.h"

#define MIN_LEN 462
#define PKT_LEN 512
#define HOP_CNT 8

struct header {
	char *crc;
	short hop;
	int order;
	short pkLast;
	short sizeOf;
	int pktId;
	int msgId;
	char *chunk;
};

char buffer[PKT_LEN + 1];
char crc_backup_buffer[33];
char datagram[512];	//increase to any size
char *data;
int i, j, k, z;	//loop variables + pkt length variable

//s-flag to keep track if we have encountered segmentation
//pktlast to keep track how many segments
//r-flag to keep track how many has been received (save until r < pktlast)
int s, pktlast,r,totalSize;
struct header **seg_table,*hdr;	//to keep all segments of a message if any : size = pktlast

  unsigned long local_port;
  unsigned long remote_port;
  unsigned long remote_port2;
  int sock_fd;
  int p1,p2;

  struct sockaddr_in server_addr;
  struct sockaddr_in peer_addr;
  struct sockaddr_in peer_addr2;

/*
*  keep_alive_msg sends keep alive msgs 
*  and flood to the network 
*/
void* keep_alive_msg(void* ptr){

  peer_addr.sin_family = AF_INET;
  peer_addr.sin_port = htons(remote_port);
  
  peer_addr2.sin_family = AF_INET;
  peer_addr2.sin_port = htons(remote_port2);
  
	 const  char alive_msg[] = "Alive";
	 while(1){

		p1 = sendto(sock_fd, alive_msg, sizeof(alive_msg), 0,(struct sockaddr *)&peer_addr, sizeof(struct sockaddr_in));
              /*  if(p1){
                printf("KEEP Alive msg \n");} */
		p2 = sendto(sock_fd, alive_msg, sizeof(alive_msg), 0,(struct sockaddr *)&peer_addr2, sizeof(struct sockaddr_in));
		fflush(stdout);

		sleep(5);
	      } 
}



void start_chat(int sock_fd, struct sockaddr_in *peer, struct sockaddr_in *peer2)
{
  peer_addr.sin_family = AF_INET;
  peer_addr.sin_port = htons(remote_port);
  
  peer_addr2.sin_family = AF_INET;
  peer_addr2.sin_port = htons(remote_port2);


  s = 0;
  int ret;
  ssize_t bytes;
  char output_buffer[1024];
  struct pollfd fds[2];

  /* Descriptor zero is stdin */
  fds[0].fd = 0;
  fds[1].fd = sock_fd;
  fds[0].events = POLLIN | POLLPRI;
  fds[1].events = POLLIN | POLLPRI;

  /* Check an exit condition  */
 int exit = 1;
  while (exit !=0) {
  printf("\nType msg: ");
    ret = poll(fds, 2, -1);

    if (ret < 0) {
      printf("Error - poll returned error: %s\n", strerror(errno));
      break;
    }
    if (ret > 0) {

     
      if (fds[0].revents & (POLLERR | POLLHUP | POLLNVAL)) {
        printf("Error - poll indicated stdin error\n");
        break;
      }
      if (fds[1].revents & (POLLERR | POLLHUP | POLLNVAL)) {
        printf("Error - poll indicated socket error\n");
        break;
      }


      if (fds[0].revents & (POLLIN | POLLPRI)) {
	
        bytes = read(0, output_buffer, sizeof(output_buffer));
        if (bytes < 0) {
          printf("Error - stdin error: %s\n", strerror(errno));
          break;
        }
        printf("You: %.*s\n", (int)bytes, output_buffer);
	
	//Encipher message using caesar cipher
	char* cipherText = Encipher(output_buffer, 3);


	//prepare following: pktLast, msgID
	short pktLast;
	int msgId;
	struct header *hdr;

	//if pktLast > 1 that means we need to split it
	int dataSize = strlen(cipherText);
	if (dataSize <= MIN_LEN)
		pktLast = 1;
	else{
		pktLast = dataSize/ MIN_LEN;
		if(dataSize % MIN_LEN != 0)
			pktLast += 1;
	}
	
	srand(time(0));
	msgId = rand();

	//prepare datagram
	bzero(buffer, PKT_LEN + 1);	//clear datagram
	hdr = (struct header*)malloc(sizeof(struct header));	//prepare header
															//add static values
	hdr->hop = HOP_CNT;	//put hop vlaue
				//order in loop
	hdr->pkLast = pktLast;
	//size in loop
	hdr->pktId = rand();
	hdr->msgId = msgId;
	//pktchunk in loop
	for (i = 0; i < pktLast; i++) {
		//order in loop
		hdr->order = i;
		//size in loop:
		if (1 == pktLast)
			hdr->sizeOf = dataSize;
		else {
			if (i + 1 == pktLast)	//for last packet
				hdr->sizeOf = dataSize % MIN_LEN;
			else
				hdr->sizeOf = MIN_LEN;
		}
		//pktchunk in loop
		hdr->chunk = malloc(hdr->sizeOf + 1);
		memcpy(hdr->chunk, &cipherText[i*MIN_LEN], hdr->sizeOf);
		hdr->chunk[hdr->sizeOf] = '\0';

		for (j = 0; j < 32; j++)
			buffer[j] = 0;
		buffer[32] = hdr->hop >> 8; buffer[33] = hdr->hop;
		buffer[34] = hdr->order >> 24; buffer[35] = hdr->order >> 16;
		buffer[36] = hdr->order >> 8; buffer[37] = hdr->order;
		buffer[38] = hdr->pkLast >> 8; buffer[39] = hdr->pkLast;
		buffer[40] = hdr->sizeOf >> 8; buffer[41] = hdr->sizeOf;
		buffer[42] = hdr->pktId >> 24; buffer[43] = hdr->pktId >> 16;
		buffer[44] = hdr->pktId >> 8; buffer[45] = hdr->pktId;
		buffer[46] = hdr->msgId >> 24; buffer[47] = hdr->msgId >> 16;
		buffer[48] = hdr->msgId >> 8; buffer[49] = hdr->msgId;

		for (j = 50, k = 0; k < hdr->sizeOf; j++, k++)
			buffer[j] = hdr->chunk[k];
		buffer[j] = '\0';

		//calculate crc and add
		hdr->crc = getMD5(hdr->sizeOf + 50, buffer);

		for (j = 0; j < 32; j++)
			buffer[j] = hdr->crc[j];
		//send it
		//printf("sending length: %d\n", 50 + hdr->sizeOf);
		//z = hdr->sizeOf + 50;
		
		bytes = sendto(sock_fd, buffer, hdr->sizeOf + 50, 0,(struct sockaddr *)peer, sizeof(struct sockaddr_in));
                bytes = sendto(sock_fd, buffer, hdr->sizeOf + 50, 0,(struct sockaddr *)peer2, sizeof(struct sockaddr_in));
		if (bytes < 0) {
		  printf("Error - sendto error: %s\n", strerror(errno));
		  break;
		}
		//x = sendto(remote_sock, buffer, hdr->sizeOf + 50, 0, (struct sockaddr *)&remote_add, sizeof(remote_add));
		
		bzero(buffer, 512);
	}


//	printf("Message sent\n");



        //bytes = sendto(sock_fd, cipherText, bytes, 0,(struct sockaddr *)peer, sizeof(struct sockaddr_in));
	
        //if (bytes < 0) {
        //  printf("Error - sendto error: %s\n", strerror(errno));
        //  break;
        //}
      }

      /* Check if data to read from socket */
      if (fds[1].revents & (POLLIN | POLLPRI)) {
       // bytes = recvfrom(sock_fd, datagram, 512,
        //                 0, NULL, NULL);
	




	while (1) {
		bzero(datagram, 512);

		/* Here we are waiting for recieving a message from remote machine */
		//z = recvfrom(sock_fd, datagram, 512, 0, (struct sockaddr *)&remote_add, &remote_len);
		//datagram[z] = 0;
		//printf("%s",datagram);
		bytes = recvfrom(sock_fd, datagram, 512, 0, NULL, NULL);
            //    printf("%s",datagram);
		datagram[bytes] = 0;
		

		//first of all extract the crc
		//reset crc field to 0's
		for (j = 0; j < 32; j++) {
			crc_backup_buffer[j] = datagram[j];
			datagram[j] = '\0';
		}
		crc_backup_buffer[j] = '\0';

		//calculate crc
		//if crc is not correct discard this message
		if (strcmp(getMD5(bytes, datagram), crc_backup_buffer) != 0) {
			printf("CRC Validation failed\n");
			printf("Expected: %s\nReceived: %s\n",crc_backup_buffer,getMD5(z, datagram));
			return;
		}

		//convert datagram to pieces
		hdr = (struct header*)malloc(sizeof(struct header));
		//don't need crc anymore
		//only need following:
		hdr->order = (unsigned char)datagram[34]; hdr->order <<= 8;
		hdr->order += (unsigned char)datagram[35]; hdr->order <<= 8;
		hdr->order += (unsigned char)datagram[36]; hdr->order <<= 8;
		hdr->order += (unsigned char)datagram[37];
		hdr->pkLast = (unsigned char)datagram[38]; hdr->pkLast <<= 8;
		hdr->pkLast += (unsigned char)datagram[39];
		hdr->sizeOf = (unsigned char)datagram[40]; hdr->sizeOf <<= 8;
		hdr->sizeOf += (unsigned char)datagram[41];
		hdr->chunk = (char*)malloc(hdr->sizeOf + 1);
		for (j = 0; j < hdr->sizeOf; j++)
			hdr->chunk[j] = datagram[50 + j];

		//second step is to check if packet is segmented
		if (s == 0)
			if (hdr->pkLast > 1) {
				seg_table = (struct header**)malloc(pktlast * sizeof(struct header));
				r = 0;
				s = 1;
				pktlast = hdr->pkLast;
				totalSize = 0;
			}

		if (s) {	//if it is segmented:
					//keep receiving until all received
			seg_table[hdr->order] = hdr;
			r++;
			totalSize += hdr->sizeOf;
			//decrypt and return when all received
			//printf("Raw: \n");
			if (r == pktlast) {
		//		printf("total size %d\n",totalSize);
				data = (char*)malloc(1 + totalSize);	//allocate enough memory
				for (i = 0, k = 0; i < pktlast; i++) {
					//for each segment
					for (j = 0; j < seg_table[i]->sizeOf; j++) {
						//copy all data of chunk
						data[k++] = seg_table[i]->chunk[j];
						//printf("%c",seg_table[i]->chunk[j]);
					}
				}
				s = 0;
				data[k] = '\0';
				break;
			}
		}
		else {		//if it is not segmented
					//extract data - return after decrypting
			data = (char*)malloc(hdr->sizeOf + 1);
			strcpy(data, hdr->chunk);
			data[hdr->sizeOf] = '\0';
			break;
		}
	}




     //   printf("%s",data);

	//Decipher message

        
	char* plainText = Decipher(data, 3);
        
	
        if (bytes < 0) {
          printf("Error - recvfrom error: %s\n", strerror(errno));
          break;
        }
        if (bytes > 0) {
          printf("Received: %s\n", plainText);
        }
      }
    }
  }
}


int main(int argc, char *argv[])
{
  unsigned long local_port;
  unsigned long remote_port;
  unsigned long remote_port2;
  int sock_fd;
  struct sockaddr_in server_addr;
  struct sockaddr_in peer_addr;
  struct sockaddr_in peer_addr2;

  /* Parse command line arguments for port numbers */
  if (argc < 4) {
    printf("Usage: %s <local port> <remote host> <remote port>\n", argv[0]);
    return 1;
  }
  local_port = strtoul(argv[1], NULL, 0);
  if (local_port < 1 || local_port > 65535) {
    printf("Error - invalid local port '%s'\n", argv[1]);
    return 1;
  }
  remote_port = strtoul(argv[3], NULL, 0);
  remote_port2 = strtoul(argv[4], NULL, 0);


  if (remote_port < 1 || remote_port > 65535) {
    printf("Error - invalid remote port '%s'\n", argv[3]);
    return 1;
  }

  /* Parse command line argument for remote host address */
  peer_addr.sin_family = AF_INET;
  peer_addr.sin_port = htons(remote_port);

  peer_addr2.sin_family = AF_INET;
  peer_addr2.sin_port = htons(remote_port2);

  if (inet_aton(argv[2], &peer_addr.sin_addr) == 0) {
    printf("Error - invalid remote address '%s'\n", argv[2]);
    return 1;
  }

  /* Create UDP socket */
  sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock_fd < 0) {
    printf("Error - failed to open socket: %s\n", strerror(errno));
    return 1;
  }

  /* Bind socket */
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(local_port);
  if (bind(sock_fd, (struct sockaddr *)(&server_addr),
           sizeof(server_addr)) < 0) {
    printf("Error - failed to bind socket: %s\n", strerror(errno));
    return 1;
  }

   pthread_t aliveThread;  
   int t = 0;
/* 
*   Creating thread for keep alive message to be excanged every 5 seconds
*   This is implemeted using thread.
*/

   pthread_create(&aliveThread, 0, keep_alive_msg, &t);
   pthread_detach(aliveThread);   
   

  /* Call chat handler to loop */
  start_chat(sock_fd, &peer_addr, &peer_addr2);

  close(sock_fd);

  return 0;
}
