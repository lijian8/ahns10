/**
 *  Collection of serialisation functions for serialising (packing) and unserialising (unpacking) primitives
 *
 *	Most of this code has been taken from section 7.4. "Serialization—How to Pack Data" 
 *	of "Beej's Guide to Network Programming" which can be found 
 *	at http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html#serialization
 *
 *	These functions handle the issue of converting from host byte-order to network byte-order
 *	by always explicitly converting the logical bitwise representation of the primitive
 *	data-type into a string representation in network byte-order (and back again to unpack).
 *	Unless I'm horribly deluded as to how this can be done.
 *
 *	There is probably a performance hit in utilising the serialisation functions of
 *	primitive_serialisation, as oppossed to alternate methods that use hton functions.
 *
 *	All of the data-types supported in this module are ones that have explicit sizes 
 *	defined (uint8_t, uint16_t, int32_t, float32, etc).
 *	This should make them portable when serialising/unserialising data within programs
 *	that work in different memory environments, ie 32 bit vs 64 bit.  However none of this
 *	has been tested extensively.
 *
 *  @creation 6/05/2010
 *	@author Joel Dawkins
 */

#include "primitive_serialisation.h"
#include <string.h>


#define FLOAT_BINARY32_EXPONENT_WIDTH 8		// bits (number of bits used to store the exponent of a IEEE754 binary32 floating point number)
#define FLOAT_BINARY32_TOTAL_WIDTH 32		// bits (total size IEEE754 binary32 floating point number (single precision float))
#define FLOAT_BINARY64_EXPONENT_WIDTH 11	// bits (number of bits used to store the exponent of a IEEE754 binary64 floating point number)
#define FLOAT_BINARY64_TOTAL_WIDTH 64		// bits (total size IEEE754 binary64 floating point number (double precision float))

// Prototypes
int32_t convertFloatToFloat754Binary32(float f);
float convertFloat754Binary32ToFloat(int32_t iFloat754Binary32);
int64_t convertDoubleToFloat754Binary64(double d);
double convertFloat754Binary64ToDouble(int64_t iFloat754Binary64);
long long ConvertToFloat754(long double f, unsigned bits, unsigned expbits);
long double ConvertFromFloat754(long long i, unsigned bits, unsigned expbits);

int32_t convertFloatToFloat754Binary32(float f)
{
	return ConvertToFloat754(f, FLOAT_BINARY32_TOTAL_WIDTH, FLOAT_BINARY32_EXPONENT_WIDTH);
}

float convertFloat754Binary32ToFloat(int32_t iFloat754Binary32)
{
	return ConvertFromFloat754(iFloat754Binary32, FLOAT_BINARY32_TOTAL_WIDTH, FLOAT_BINARY32_EXPONENT_WIDTH);
}

int64_t convertDoubleToFloat754Binary64(double d)
{
	return ConvertToFloat754(d, FLOAT_BINARY64_TOTAL_WIDTH, FLOAT_BINARY64_EXPONENT_WIDTH);
}

double convertFloat754Binary64ToDouble(int64_t iFloat754Binary64)
{
	return ConvertFromFloat754(iFloat754Binary64, FLOAT_BINARY64_TOTAL_WIDTH, FLOAT_BINARY64_EXPONENT_WIDTH);
}

long long ConvertToFloat754(long double f, unsigned bits, unsigned expbits)
{
    long double fnorm;
    int shift;
    long long sign, exp, significand;
    unsigned significandbits = bits - expbits - 1; // -1 for sign bit
	
    if (f == 0.0) return 0; // get this special case out of the way
	
    // check sign and begin normalization
    if (f < 0) { sign = 1; fnorm = -f; }
    else { sign = 0; fnorm = f; }
	
    // get the normalized form of f and track the exponent
    shift = 0;
    while(fnorm >= 2.0) { fnorm /= 2.0; shift++; }
    while(fnorm < 1.0) { fnorm *= 2.0; shift--; }
    fnorm = fnorm - 1.0;
	
    // calculate the binary form (non-float) of the significand data
    significand = fnorm * ((1LL<<significandbits) + 0.5f);
	
    // get the biased exponent
    exp = shift + ((1<<(expbits-1)) - 1); // shift + bias
	
    // return the final answer
    return (sign<<(bits-1)) | (exp<<(bits-expbits-1)) | significand;
}

long double ConvertFromFloat754(long long i, unsigned bits, unsigned expbits)
{
    long double result;
    long long shift;
    unsigned bias;
    unsigned significandbits = bits - expbits - 1; // -1 for sign bit
	
    if (i == 0) return 0.0;
	
    // pull the significand
    result = (i&((1LL<<significandbits)-1)); // mask
    result /= (1LL<<significandbits); // convert back to float
    result += 1.0f; // add the one back on
	
    // deal with the exponent
    bias = (1<<(expbits-1)) - 1;
    shift = ((i>>significandbits)&((1LL<<expbits)-1)) - bias;
    while(shift > 0) { result *= 2.0; shift--; }
    while(shift < 0) { result /= 2.0; shift++; }
	
    // sign it
    result *= (i>>(bits-1))&1? -1.0: 1.0;
	
    return result;
}


int PackInt16(unsigned char *buf, int16_t i)
{
	// Record each octet in descending order of significance
	buf[0] = (unsigned char)(i>>8);
	buf[1] = (unsigned char)i;
	
	// Return the number of octets written to the buffer
	return 2;
}

int UnpackInt16(const unsigned char *buf, int16_t *i)
{
	// Rebuild the 16 bit integer
	*i = (buf[0])<<8 | buf[1];
	
	// Return the number of octets read from the buffer
	return 2;
}

int PackUInt16(unsigned char *buf, uint16_t i)
{
	// Record each octet in descending order of significance
	buf[0] = (unsigned char)(i>>8);
	buf[1] = (unsigned char)i;
	
	// Return the number of octets written to the buffer
	return 2;
}

int UnpackUInt16(const unsigned char *buf, uint16_t *i)
{
	// Rebuild the 16 bit unsigned integer
	*i = (buf[0])<<8 | buf[1];
	
	// Return the number of octets read from the buffer
	return 2;
}

int PackInt32(unsigned char *buf, int32_t i)
{
	// Record each octet in descending order of significance
	buf[0] = (unsigned char)(i>>24);
	buf[1] = (unsigned char)(i>>16);
	buf[2] = (unsigned char)(i>>8);
	buf[3] = (unsigned char)(i);
	
	// Return the number of octets written to the buffer
	return 4;
}

int UnpackInt32(const unsigned char *buf, int32_t *i)
{
	// Rebuild the 32 bit integer
	*i = (buf[0])<<24 | (buf[1])<<16 | (buf[2])<<8 | buf[3];
	
	// Return the number of octets read from the buffer
	return 4;
}

int PackUInt32(unsigned char *buf, uint32_t i)
{
	// Record each octet in descending order of significance
	buf[0] = (unsigned char)(i>>24);
	buf[1] = (unsigned char)(i>>16);
	buf[2] = (unsigned char)(i>>8);
	buf[3] = (unsigned char)(i);
	
	// Return the number of octets written to the buffer
	return 4;
}

int UnpackUInt32(const unsigned char *buf, uint32_t *i)
{
	// Rebuild the 32 bit unsigned integer
	*i = (buf[0])<<24 | (buf[1])<<16 | (buf[2])<<8 | buf[3];
	
	// Return the number of octets read from the buffer
	return 4;
}

int PackInt64(unsigned char *buf, int64_t i)
{
	// Record each octet in descending order of significance
	buf[0] = (unsigned char)(i>>56);
	buf[1] = (unsigned char)(i>>48);
	buf[2] = (unsigned char)(i>>40);
	buf[3] = (unsigned char)(i>>32);
	buf[4] = (unsigned char)(i>>24);
	buf[5] = (unsigned char)(i>>16);
	buf[6] = (unsigned char)(i>>8);
	buf[7] = (unsigned char)(i);
	
	// Return the number of octets written to the buffer
	return 8;
}

int UnpackInt64(const unsigned char *buf, int64_t *i)
{
	// Rebuild the 64 bit integer
	*i =	(int64_t)(buf[0])<<56 | (int64_t)(buf[1])<<48 | 
			(int64_t)(buf[2])<<40 | (int64_t)(buf[3])<<32 | 
			(int64_t)(buf[4])<<24 | (int64_t)(buf[5])<<16 | 
			(int64_t)(buf[6])<<8 | (int64_t)buf[7];
	
	// Return the number of octets read from the buffer
	return 8;
}

int PackUInt64(unsigned char *buf, uint64_t i)
{
	// Record each octet in descending order of significance
	buf[0] = (unsigned char)(i>>56);
	buf[1] = (unsigned char)(i>>48);
	buf[2] = (unsigned char)(i>>40);
	buf[3] = (unsigned char)(i>>32);
	buf[4] = (unsigned char)(i>>24);
	buf[5] = (unsigned char)(i>>16);
	buf[6] = (unsigned char)(i>>8);
	buf[7] = (unsigned char)(i);
	
	// Return the number of octets written to the buffer
	return 8;
}

int UnpackUInt64(const unsigned char *buf, uint64_t *i)
{
	// Rebuild the 64 bit integer
	*i =	(int64_t)(buf[0])<<56 | (int64_t)(buf[1])<<48 | 
			(int64_t)(buf[2])<<40 | (int64_t)(buf[3])<<32 | 
			(int64_t)(buf[4])<<24 | (int64_t)(buf[5])<<16 | 
			(int64_t)(buf[6])<<8 | (int64_t)buf[7];
	
	// Return the number of octets read from the buffer
	return 8;
}


int PackFloat32(unsigned char *buf, float f)
{
	// Convert "f" to the IEEE754 binary32 floating point number format (single precision), and pack it as if it were an int
	/*
	 *	int32_t intInIEEE754Binary32Format;
	 *	intInIEEE754Binary32Format = convertFloatToFloat754Binary32(f);
	 *	return PackInt32(buf, intInIEEE754Binary32Format);
	 */
	
	// However, C compilers should already use the binary32 format for floats, so the conversion step can be skipped
	// Cast the memory address of the float (&f) to a pointer of an int32_t (int32_t*), and then pass the value that this points to (*), to the PackInt32 function
	return PackInt32(buf, *((int32_t*)&f));
}

int UnpackFloat32(const unsigned char *buf, float *f)
{
	// Ussuming PackFloat32 was used to pack the Float, do the reverse to unpack it
	/*
	 *	int32_t intInIEEE754Binary32Format;
	 *	UnpackInt32(buf, &intInIEEE754Binary32Format);
	 *	*f = convertFloat754Binary32ToFloat(intInIEEE754Binary32Format);
	 *	return 4;
	 */
	
	// If PackFloat32 skipped the step where the float is converted to binary32 format...
	return UnpackInt32(buf, (int32_t*)f);
}

int PackFloat64(unsigned char *buf, double d)
{
	// Convert "d" to the IEEE754 binary64 floating point number format (double precision), and pack it as if it were an int
	/*
	 *	int64_t intInIEEE754Binary64Format;
	 *	intInIEEE754Binary64Format = convertDoubleToFloat754Binary64(d);
	 *	return PackInt64(buf, intInIEEE754Binary64Format);
	 */
	
	// However, C compilers should already use the binary64 format for doubles, so the conversion step can be skipped
	// Cast the memory address of the double (&d) to a pointer of an int64_t (int64_t*), and then pass the value that this points to (*), to the PackInt64 function
	return PackInt64(buf, *((int64_t*)&d));
}

int UnpackFloat64(const unsigned char *buf, double *d)
{
	// Ussuming PackFloat64 was used to pack the Double, do the reverse to unpack it
	/*
	 *	int64_t intInIEEE754Binary64Format;
	 *	UnpackInt64(buf, &intInIEEE754Binary64Format);
	 *	*d = convertFloat754Binary64ToDouble(intInIEEE754Binary64Format);
	 *	return 8;
	 */
		
	// If PackFloat64 skipped the step where the double is converted to binary64 format...
	return UnpackInt64(buf, (int64_t*)d);
}

// Any object can be treated as an array of unsigned char's
// This is why this method can be used by both int8_t and uint8_t.  They get type cast to an unsigned char because that is the defined type of the argument "c"
int PackUChar(unsigned char *buf, unsigned char c)
{
	// Record the char
	buf[0] = c;
	
	// Return the number of octets written to the buffer
	return 1;
}

int UnpackUChar(const unsigned char *buf, unsigned char *c)
{
	// Retrieve the char
	*c = buf[0];
	
	// Return the number of octets read from the buffer
	return 1;
}

int PackChar(unsigned char *buf, char c)
{
	return PackUChar(buf, c);
}

// I've only included "signed" versions of the Pack/UnpackChar so that a warning isn't generated when you use UnpackUChar() passing a pointer to a signed char 
int UnpackChar(const unsigned char *buf, char *c)
{
	return UnpackUChar(buf, (unsigned char*)c);
}

int PackInt8(unsigned char *buf, int8_t i)
{
	return PackUChar(buf, i);
}

int UnpackInt8(const unsigned char *buf, int8_t *i)
{
	return UnpackUChar(buf, (unsigned char*)i);
}

int PackUInt8(unsigned char *buf, uint8_t i)
{
	return PackUChar(buf, i);
}

int UnpackUInt8(const unsigned char *buf, uint8_t *i)
{
	return UnpackUChar(buf, (unsigned char*)i);
}

int PackBytes(unsigned char *buf, unsigned char *bytes, uint32_t length)
{
	int bufferOffset = 0;
	
	// Pack the length into the buffer
	bufferOffset += PackUInt32(buf, length);
	
	// Now copy the byte string into it
	memcpy(&buf[bufferOffset], bytes, length);

	// Return the number of octefts written to the buffer = length + bufferOffset
	return length + bufferOffset;
}

int UnpackBytes(const unsigned char *buf, unsigned char *bytes, uint32_t maxLength, uint32_t *resultantByteStringLength)
{
	uint32_t byteStringLength, lengthToCopy, bufferOffset = 0;
	
	// The first 4 bytes that buf points to should be the length of the following packed byte string
	bufferOffset = UnpackUInt32(buf, &byteStringLength);
	
	lengthToCopy = (byteStringLength < maxLength)? byteStringLength : maxLength;
	
	// Copy the byte string
	memcpy(bytes, &buf[bufferOffset], lengthToCopy);
	
	// Set resultantByteStringLength
	*resultantByteStringLength = lengthToCopy;
	
	// Return the number of octets that the buffer should progress
	return bufferOffset + byteStringLength;
}

int PackString(unsigned char *buf, char *str)
{
	// Just pack it as a byte string (note that this doesn't pack the terminating null char
	return PackBytes(buf, (unsigned char*)str, strlen(str));
}

// PRE: maxLength >= 1
int UnpackString(const unsigned char *buf, char *str, uint32_t maxLength)
{
	uint32_t resultantStringLength, bufferOffset = 0;
	
	// Pass through maxLength-1 so that there is always room left for the terminating null char
	bufferOffset = UnpackBytes(buf, (unsigned char*)str, maxLength-1, &resultantStringLength);
	
	// Add the terminating null char
	str[resultantStringLength] = '\0';
	
	return bufferOffset;
}

int PackTimevalStruct(unsigned char *buf, struct timeval t)
{
	int bufferOffset = 0;
	
	// Add each component of the struct (typecasting them to 32 bit signed ints should be safe)
	bufferOffset += PackInt32(&buf[bufferOffset], t.tv_sec);
	bufferOffset += PackInt32(&buf[bufferOffset], t.tv_usec);
	
	return bufferOffset;
}

int UnpackTimevalStruct(const unsigned char *buf, struct timeval *t)
{
	int bufferOffset = 0;
	
	// This is done, just in case either of these is pointing to an integer which is larger than 32 bit
	t->tv_sec	= 0;
	t->tv_usec	= 0;
	
	// Add each component of the struct (typecasting them to 32 bit signed ints should be safe)
	bufferOffset += UnpackInt32(&buf[bufferOffset], (int32_t*)&t->tv_sec);
	bufferOffset += UnpackInt32(&buf[bufferOffset], (int32_t*)&t->tv_usec);
	
	return bufferOffset;
	
}


