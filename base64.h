/**
 * MIT License
 * Copyright (c) 2018 Jonas Hülsermann
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

/*char encodingTable[] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
	'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
};
uint8_t decodingTable[] = {

};*/

char encodeChar(uint8_t charBits) {
	if (charBits <= 25)
		return 'A' + charBits;
	else if (charBits <= 51)
		return 'a' + charBits;
	else if (charBits <= 61)
		return '0' + charBits;
	else if (charBits == 62)
		return '+';
	else if (charBits == 63)
		return '/';
	else
		return '=';
}

uint8_t decodeChar(char b64char) {

}

char *base64encode(uint8_t *data, size_t size) {
	uint8_t padding = size % 3;
	size_t b64size = (size/3)*4+padding;
	char *b64text = (char*) malloc(b64size+1);
	b64text[b64size] = '\0';
	size_t b64inc = 0;

	//for (size_t i=b64size-padding; i<b64size; ++i)
	//	b64text[i] = '=';
	
	for (size_t i=0; i<size; i=i+3) {
		uint8_t ch1bits, ch2bits, ch3bits, ch4bits;

		ch1bits = data[i] >> 2;
		
		ch2bits = (data[i] & 0x03) << 4;
		if (i+1 < size) {			
			ch2bits |= data[i+1] >> 4;

			ch3bits = (data[i+1] << 2) & 0x3F;
		} else ch3bits = 0xFF;

		if (i+2 < size) {
			ch3bits |= data[i+2] >> 6;
			
			ch4bits = data[i+2] & 0x3F;
		} else ch4bits = 0xFF;

		b64text[b64inc++] = encodeChar(ch1bits);
		b64text[b64inc++] = encodeChar(ch2bits);
		b64text[b64inc++] = encodeChar(ch3bits);
		b64text[b64inc++] = encodeChar(ch4bits);
	}

	return b64text;
}
