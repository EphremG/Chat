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

 #include <stdlib.h>
 #include <stdio.h>
 #include <string.h>
 #include <ctype.h>

char Cipher(char ch, int key)
{
	if (!isalpha(ch))
		return ch;

	char offset = isupper(ch) ? 'A' : 'a';
	return (char)((((ch + key) - offset) % 26) + offset);
}

char* Encipher(char* input, int key)
{
	int inputLen = strlen(input);
	char* output = (char*)malloc(inputLen + 1);

	for (int i = 0; i < inputLen; ++i)
		output[i] = Cipher(input[i], key);

	output[inputLen] = '\0';
	return output;
}

char* Decipher(char* input, int key)
{
	return Encipher(input, 26 - key);
}
