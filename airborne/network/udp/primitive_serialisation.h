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

#ifndef PRIMITIVE_SERIALISATION_H
#define PRIMITIVE_SERIALISATION_H

#include <stdint.h>
#include <sys/time.h>


/**
 *	Packs the data-type into the buffer
 *
 *	Serialises the supplied argument, writes this serialised representation to 
 *	the buffer and returns the length written to the buffer in octects (bytes)
 *
 *	@param	buf		unsigned char*			Buffer to write the serialised representation of the integer
 *	@param	i		(fixed length integer)	Integer to write to the buffer
 *	@return			int						length of data written to buf
 */
int PackInt8(unsigned char *buf, int8_t i);
int PackInt16(unsigned char *buf, int16_t i);
int PackInt32(unsigned char *buf, int32_t i);
int PackInt64(unsigned char *buf, int64_t i);
int PackUInt8(unsigned char *buf, uint8_t i);
int PackUInt16(unsigned char *buf, uint16_t i);
int PackUInt32(unsigned char *buf, uint32_t i);
int PackUInt64(unsigned char *buf, uint64_t i);


int PackFloat32(unsigned char *buf, float f);	// With the assumption that the datatype float conforms to binary32
int PackFloat64(unsigned char *buf, double d);	// With the assumption that the datatype double conforms to binary64
int PackUChar(unsigned char *buf, unsigned char c);
int PackChar(unsigned char *buf, char c);
int PackBytes(unsigned char *buf, unsigned char *bytes, uint32_t length);  // This will also pack length into the buffer
int PackString(unsigned char *buf, char *str);
int PackTimevalStruct(unsigned char *buf, struct timeval t);


/**
 *	Unpacks the data-type from the buffer
 *
 *	Reconstructs (unserialises) a variable from the buffer and returns the 
 *	number of bytes it took up in the buffer
 *
 *	@param	buf		unsigned char*		Buffer pointing to the start of the serialised representation of the integer
 *	@param	i		(pointer to fixed	Destination for the unserialised integer
 *					length integer)	
 *	@return			int					number of bytes used to store the serialised integer in the buffer
 */
int UnpackInt8(const unsigned char *buf, int8_t *i);
int UnpackInt16(const unsigned char *buf, int16_t *i);
int UnpackInt32(const unsigned char *buf, int32_t *i);
int UnpackInt64(const unsigned char *buf, int64_t *i);
int UnpackUInt8(const unsigned char *buf, uint8_t *i);
int UnpackUInt16(const unsigned char *buf, uint16_t *i);
int UnpackUInt32(const unsigned char *buf, uint32_t *i);
int UnpackUInt64(const unsigned char *buf, uint64_t *i);


int UnpackFloat32(const unsigned char *buf, float *f);	// With the assumption that the datatype float conforms to binary32
int UnpackFloat64(const unsigned char *buf, double *d);	// With the assumption that the datatype double conforms to binary64
int UnpackUChar(const unsigned char *buf, unsigned char *c);
int UnpackChar(const unsigned char *buf, char *c);
int UnpackBytes(const unsigned char *buf, unsigned char *bytes, uint32_t maxLength, uint32_t *resultantByteStringLength);
int UnpackString(const unsigned char *buf, char *str, uint32_t maxLength); 
int UnpackTimevalStruct(const unsigned char *buf, struct timeval *t);


#endif
