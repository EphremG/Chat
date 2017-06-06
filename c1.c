/*--------------------------------------                
* Chat system implementation usng UDP  -
*---------------------------------------
* Caesar Cipher Encryption             -
* Segmentation                         -  
* Keep alive msg                       -
* socket                 -
* --------------------------------------
* Author: Ephrem Getachew Demesa
*         Kayode Omotoye
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <math.h>
#include "md5.h"
#include "crypto.h"
#include "crypto.c"

#define MIN_LEN 462
#define PKT_LEN 512
#define HOP_CNT 8	//any vlaue

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
int i, j, k, z;	//loop variables + pkt length variable

//s-flag to keep track if we have encountered segmentation
//pktlast to keep track how many segments
//r-flag to keep track how many has been received (save until r < pktlast)
int s, pktlast,r,totalSize;
struct header **seg_table,*hdr;	//to keep all segments of a message if any : size = pktlast

int main(int argc, char **argv){
	
	int x, z;
	char *host_ip = "127.0.0.1";
	short host_port = 2222;
	char *remote_ip = "127.0.0.1";
	short remote_port = 3333;
	int host_sock, remote_sock;
	char datagram[5120];	//increase to any size
	unsigned int remote_len;
	char *data;

	struct sockaddr_in host_add;
	struct sockaddr_in remote_add;

	host_sock = socket(AF_INET, SOCK_DGRAM, 0);
	
	host_add.sin_family = AF_INET;
	host_add.sin_port = htons(host_port);
	host_add.sin_addr.s_addr = inet_addr(host_ip);

	z = bind(host_sock, (struct sockaddr *)&host_add, sizeof(host_add));

	
	while(1){	
             
		printf("\nYou: ");
		bzero(datagram, 5120);
		fgets(datagram, 5120, stdin);
		//strcpy(datagram, "0xxxxxxxxxX1xxxxxxxxxX2xxxxxxxxxX3xxxxxxxxxX4xxxxxxxxxX5xxxxxxxxxX6xxxxxxxxxX7xxxxxxxxxX8xxxxxxxxxX9xxxxxxxxxX0xxxxxxxxxX1xxxxxxxxxX2xxxxxxxxxX3xxxxxxxxxX4xxxxxxxxxX5xxxxxxxxxX6xxxxxxxxxX7xxxxxxxxxX8xxxxxxxxxX9xxxxxxxxxX0xxxxxxxxxX1xxxxxxxxxX2xxxxxxxxxX3xxxxxxxxxX4xxxxxxxxxX5xxxxxxxxxX6xxxxxxxxxX7xxxxxxxxxX8xxxxxxxxxX9xxxxxxxxxX0xxxxxxxxxX1xxxxxxxxxX2xxxxxxxxxX3xxxxxxxxxX4xxxxxxxxxX5xxxxxxxxxX6xxxxxxxxxX7xxxxxxxxxX8xxxxxxxxxX9xxxxxxxxxX0xxxxxxxxxX1xxxxxxxxxX2xxxxxxxxxX3xxxxxxxxxX4xxxxxxxxxX5xxxxxxxxxX#@");
		
		remote_sock = socket(AF_INET, SOCK_DGRAM, 0);
		remote_add.sin_family = AF_INET;
		remote_add.sin_port = htons(remote_port);
		remote_add.sin_addr.s_addr = inet_addr(remote_ip);

		/* Here we are sending the udp message as datagram to remote machine */

		char* cipherText = Encipher(datagram, 3);
		/////////////////////////////////////////////////////////////////////////////////////////


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
			x = sendto(remote_sock, buffer, hdr->sizeOf + 50, 0, (struct sockaddr *)&remote_add, sizeof(remote_add));
			if (x != -1) {
				//	puts("sent");
			}
			//assuming that we receive it
			//getMessage();
			//reset
			bzero(buffer, 512);
		}
		
		/////////////////////////////////////////////////////////////////////////////////////////
		//x = sendto(remote_sock, cipherText, strlen(datagram), 0, (struct sockaddr *)&remote_add, sizeof(remote_add));
		//if(x != -1){
		//    //	puts("sent");
		//}
		close(remote_sock);
		bzero(datagram, 512);
        
		/* Here we are waiting for recieving a message from remote machine */

		int flag = 1;

		while(flag != 0){		

			while (1) {
				bzero(datagram, 512);

				/* Here we are waiting for recieving a message from remote machine */
				z = recvfrom(host_sock, datagram, 512, 0, (struct sockaddr *)&remote_add, &remote_len);
				datagram[z] = 0;

				//first of all extract the crc
				//reset crc field to 0's
				for (j = 0; j < 32; j++) {
					crc_backup_buffer[j] = datagram[j];
					datagram[j] = '\0';
				}
				crc_backup_buffer[j] = '\0';

				//calculate crc
				//if crc is not correct discard this message
				if (strcmp(getMD5(z, datagram), crc_backup_buffer) != 0) {
					printf("CRC Validation failed\n");
					return 0;
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
					if (r == pktlast) {
						data = (char*)malloc(1 + totalSize);	//allocate enough memory
						for (i = 0, k = 0; i < pktlast; i++) {
							//for each segment
							for (j = 0; j < seg_table[i]->sizeOf; j++) {
								//copy all data of chunk
								data[k++] = seg_table[i]->chunk[j];
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





			char* plainText = Decipher(data, 3);

			//printf("%s",plainText);
		
		        if(strcmp("A",plainText) != 0){	
		
			    printf("\nClient:%s", plainText);
			    flag = 0;}
			else {
			   
			    flag = 1;}
	           }

	}
	close(host_sock);
	return 0;
}
