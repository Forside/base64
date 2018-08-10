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

#ifndef BASE64_H_
#define BASE64_H_

#include <stdint.h>
#include <stdlib.h>

#define GET_MISSING_BYTES_FOR_B64(size) ((3 - (size % 3)) % 3)
#define GET_B64_ENCODED_SIZE(size) (((size + GET_MISSING_BYTES_FOR_B64(size)) / 3) * 4)
#define GET_B64_ENCODED_SIZE2(size, missing) (((size + missing) / 3) * 4)
#define GET_B64_MAX_DECODED_SIZE(length) (length / 4 * 3)

static char encodeChar(uint8_t charBits)
{
	charBits &= 0x3F;
	if (charBits <= 25)
		return 'A' + charBits;
	else if (charBits <= 51)
		return 'a' + charBits-26;
	else if (charBits <= 61)
		return '0' + charBits-52;
	else if (charBits == 62)
		return '+';
	else if (charBits == 63)
		return '/';
	//else
	//	return '=';
}

static uint8_t decodeChar(char b64char)
{
	if (b64char >= 'A' && b64char <= 'Z')
		return b64char - 'A';
	else if (b64char >= 'a' && b64char <= 'z')
		return b64char - 'a' + 26;
	else if (b64char >= '0' && b64char <= '9')
		return b64char - '0' + 52;
	else if (b64char == '+')
		return 62;
	else if (b64char == '/')
		return 63;
	else
		return 0xFF;
}

static size_t base64encode(const uint8_t *data, size_t size, char *dest)
{
	const uint8_t missing = GET_MISSING_BYTES_FOR_B64(size); // (3 - (size % 3)) % 3;
	const size_t b64size = GET_B64_ENCODED_SIZE2(size, missing); // ((size + missing) / 3) * 4;

	dest[b64size] = '\0';
	size_t b64inc = 0;

	for (size_t i=b64size-missing; i<b64size; ++i)
		dest[i] = '=';

	uint8_t chBits;

	for (size_t i=0; i<size; ++i) {
		switch (i % 3) {
			case 0: {
				chBits = data[i] >> 2;
				dest[b64inc++] = encodeChar(chBits);
				chBits = data[i] << 4;
			} break;

			case 1: {
				chBits |= data[i] >> 4;
				dest[b64inc++] = encodeChar(chBits);
				chBits = data[i] << 2;
			} break;

			case 2: {
				chBits |= data[i] >> 6;
				dest[b64inc++] = encodeChar(chBits);
				chBits = data[i];
				dest[b64inc++] = encodeChar(chBits);
			} break;
		}
	}

	if (missing)
		dest[b64inc] = encodeChar(chBits);

	return b64size;
}

static size_t base64encodeA(const uint8_t *data, size_t size, char **dest)
{
	//const uint8_t missing = GET_MISSING_BYTES_FOR_B64(size); // (3 - (size % 3)) % 3;
	const size_t b64size = GET_B64_ENCODED_SIZE(size); // ((size + missing) / 3) * 4;

	*dest = (char*) malloc(b64size+1);
	return base64encode(data, size, *dest);	
}

static size_t base64decode(const char *b64text, size_t length, uint8_t *dest)
{
	size_t dataInc = 0;
	uint8_t dataBits;

	for (size_t i=0; i<length && b64text[i] != '='; ++i) {
		switch (i % 4) {
			case 0: {
				dataBits = decodeChar(b64text[i]) << 2;
			} break;
			
			case 1: {
				dataBits |= decodeChar(b64text[i]) >> 4;
				dest[dataInc++] = dataBits;
				dataBits = decodeChar(b64text[i]) << 4;
			} break;
			
			case 2: {
				dataBits |= decodeChar(b64text[i]) >> 2;
				dest[dataInc++] = dataBits;
				dataBits = decodeChar(b64text[i]) << 6;
			} break;
			
			case 3: {
				dataBits |= decodeChar(b64text[i]);
				dest[dataInc++] = dataBits;
			} break;
		}
	}

	return dataInc;
}

static size_t base64decodeA(const char *b64text, size_t length, uint8_t **dest)
{
	const size_t maxDataSize = GET_B64_MAX_DECODED_SIZE(length); // length / 4 * 3;
	*dest = (uint8_t*) malloc(maxDataSize);
	return base64decode(b64text, length, *dest);
}

#endif
