/*
 * Crypto.h
 * Header file for crypto handler
 * Author: 
 */

#ifndef __crypto_H_
#define __crypto_H_

char Cipher(char ch, int key);
char* Encipher(char* input, int key);
char* Decipher(char* input, int key);

#endif 
