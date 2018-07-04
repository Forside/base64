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

//#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define ENCODING_METHOD 3

/*char encodingTable[] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
	'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
};
uint8_t decodingTable[] = {

};*/

char encodeChar(uint8_t charBits)
{
	charBits &= 0x3F;
	if (charBits <= 25)
		return 'A' + charBits;
	else if (charBits <= 51)
		return 'a' + charBits-26;
	else if (charBits <= 61)
		return '0' + charBits-62;
	else if (charBits == 62)
		return '+';
	else if (charBits == 63)
		return '/';
	else
		return '=';
}

uint8_t decodeChar(char b64char)
{
	if (b64char >= 'A' && b64char <= 'Z')
		return b64char - 'A';
	else if (b64char >= 'a' && b64char <= 'z')
		return b64char - 'a' + 26;
	else if (b64char >= '0' && b64char <= '9')
		return b64char - '0' + 62;
	else if (b64char == '+')
		return 62;
	else if (b64char == '/')
		return 63;
	else
		return 0xFF;
}

#if ENCODING_METHOD == 1
char *base64encode(const uint8_t *data, size_t size)
{
	const uint8_t missing = (3 - (size % 3)) % 3;
	const size_t b64size = ((size + missing) / 3) * 4;

	char *b64text = (char*) malloc(b64size+1);
	b64text[b64size] = '\0';
	size_t b64inc = 0;

	//for (size_t i=b64size-missing; i<b64size; ++i)
	//	b64text[i] = '=';
	
	for (size_t i=0; i<size; i+=3) {
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

#elif ENCODING_METHOD == 2

char *base64encode(const uint8_t *data, size_t size)
{
	const uint8_t excess = size % 3;
	const uint8_t missing = (3 - excess) % 3;
	const size_t b64size = ((size + missing) / 3) * 4;

	char *b64text = (char*) malloc(b64size+1);
	b64text[b64size] = '\0';
	size_t b64inc = 0;
	
	uint8_t chBits;

	if (size >= 3) {
		for (const uint8_t *d=data; d<data+(size-excess); d+=3) {
			// char 1
			chBits = *d >> 2;
			b64text[b64inc++] = encodeChar(chBits);

			// char 2
			chBits = ((*d << 4) | (*(d+1) >> 4)) & 0x3F;
			b64text[b64inc++] = encodeChar(chBits);

			// char 3
			chBits = ((*(d+1) << 2) | *(d+2) >> 6) & 0x3F;
			b64text[b64inc++] = encodeChar(chBits);

			// char 4
			chBits = *(d+2) & 0x3F;
			b64text[b64inc++] = encodeChar(chBits);
		}

		/*const uint8_t *dataEndWithoutExcess = data+(size-excess);
		while (data != dataEndWithoutExcess) {
			// char 1
			chBits = *data >> 2;
			b64text[b64inc++] = encodeChar(chBits);

			// char 2
			chBits = ((*data << 4) | (*++data >> 4)) & 0x3F;
			b64text[b64inc++] = encodeChar(chBits);

			// char 3
			chBits = ((*data << 2) | *++data >> 6) & 0x3F;
			b64text[b64inc++] = encodeChar(chBits);

			// char 4
			chBits = *data++ & 0x3F;
			b64text[b64inc++] = encodeChar(chBits);
		}*/
	}

	switch (missing) {
		case 2: {
			const uint8_t *d = size > 3
				? data+(size-excess)
				: data;

			// char 1
			chBits = *d >> 2;
			b64text[b64inc++] = encodeChar(chBits);

			// char 2
			chBits = (*d << 4) & 0x3F;
			b64text[b64inc++] = encodeChar(chBits);

			// char 3
			b64text[b64inc++] = '=';

			// char 4
			b64text[b64inc++] = '=';
		} break;

		case 1: {
			const uint8_t *d = size > 3
				? data+(size-excess)
				: data;

			// char 1
			chBits = *d >> 2;
			b64text[b64inc++] = encodeChar(chBits);

			// char 2
			chBits = ((*d << 4) | (*(d+1) >> 4)) & 0x3F;
			b64text[b64inc++] = encodeChar(chBits);

			// char 3
			chBits = (*(d+1) << 2) & 0x3F;
			b64text[b64inc++] = encodeChar(chBits);

			// char 4
			b64text[b64inc++] = '=';
		} break;
	}

	return b64text;
}

#elif ENCODING_METHOD == 3

char *base64encode(const uint8_t *data, size_t size, size_t *destSize = NULL)
{
	const uint8_t missing = (3 - (size % 3)) % 3;
	const size_t b64size = ((size + missing) / 3) * 4;

	char *b64text = (char*) malloc(b64size+1);
	b64text[b64size] = '\0';
	size_t b64inc = 0;

	for (char *c=b64text+(b64size-missing); c<b64text+b64size; ++c)
		*c = '=';

	uint8_t chBits;

	for (const uint8_t *d=data; d<data+size; ++d) {
		switch ((d - data) % 3) {
			case 0: {
				chBits = *d >> 2;
				b64text[b64inc++] = encodeChar(chBits);
				chBits = *d << 4;
			} break;

			case 1: {
				chBits |= *d >> 4;
				b64text[b64inc++] = encodeChar(chBits);
				chBits = *d << 2;
			} break;

			case 2: {
				chBits |= *d >> 6;
				b64text[b64inc++] = encodeChar(chBits);
				chBits = *d;
				b64text[b64inc++] = encodeChar(chBits);
			} break;
		}
	}

	if (missing)
		b64text[b64inc] = encodeChar(chBits);

	if (destSize != NULL)
		*destSize = b64size;

	return b64text;
}

#endif


uint8_t *base64decode(const char *b64text, size_t length, size_t *destSize = NULL)
{
	const size_t maxDataSize = length / 4 * 3;
	uint8_t *data = (uint8_t*) malloc(maxDataSize);
	size_t dataInc = 0;

	uint8_t dataBits;

	for (const char *c=b64text; c<b64text+length && *c!='='; ++c) {
		switch ((c - b64text) % 4) {
			case 0: {
				dataBits = *c << 2;
			} break;
			
			case 1: {
				dataBits |= *c >> 4;
				data[dataInc++] = decodeChar(dataBits);
				dataBits = *c << 4;
			} break;
			
			case 2: {
				dataBits |= *c >> 2;
				data[dataInc++] = decodeChar(dataBits);
				dataBits = *c << 6;
			} break;
			
			case 3: {
				dataBits |= *c;
				data[dataInc++] = decodeChar(dataBits);
			} break;
		}
	}

	if (dataInc <= maxDataSize)
		data[dataInc] = decodeChar(dataBits);

	if (destSize != NULL)
		*destSize = dataInc-1;

	return data;
}
